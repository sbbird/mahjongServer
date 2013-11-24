using namespace	std;

#include "config.h"
#include "xmlparser.h"
#include "server.h"
#include "message.h"
#include "room.h"
#include "stringqueue.h"
#include "authorizehttpconnection.h"
#include "roommember.h"

extern StringQueue SQ;
extern pthread_mutex_t		oHTTPThreadMutex;
extern pthread_cond_t		oHTTPThreadCondition;

#include "user.h"

/*****************************************************
User::User

	Description:
	   Default constructor (never used)

	Author:
		2004-10-13	Daniel Önnerby

*****************************************************/
User::User(){

}

/*****************************************************
User::User(int iNewSocket,sockaddr_in oNewSocketArrd)

	Description:
	   Constructor used	to submit the socketinformation

	Author:
		2004-10-13	Daniel Önnerby

*****************************************************/
User::User(class Server	*oMainServer,int iNewSocket,sockaddr_in	oNewSocketArrd){
	#ifdef DEBUG
		LOGSTREAM << "User::User(oMainServer," << iNewSocket <<	",oNewSocketArrd)\n";
	#endif
	bWriting			= false;
	bNameLock			= false;
	iCurrentMsgWritten	= 0;
	oServer				= oMainServer;
	iSocket				= iNewSocket;
	oSocketArrd			= oNewSocketArrd;

//	LOGSTREAM << "Connection from "	<< inet_ntoa(oSocketArrd.sin_addr) << "	- "	<< iSocket << "\n";

	bDelete		 = !initSocket();

	if(!bDelete){
		FD_SET(iSocket,&oServer->aAllReadSockets);
		char sTemp[16];
		sprintf(sTemp,"%d",iSocket);
		sId.append(sTemp);
	}

}


/*****************************************************
User::~User

	Description:
	   Destructor

	Author:
		2004-10-13	Daniel Önnerby

*****************************************************/
User::~User(){
	#ifdef DEBUG
		LOGSTREAM << "User::~User()\n";
	#endif
	//LOGSTREAM << "Dropped "	<< inet_ntoa(oSocketArrd.sin_addr) << "	- "	<< iSocket << "\n";

	FD_CLR (iSocket, &oServer->aAllReadSockets);
	FD_CLR (iSocket, &oServer->aAllWriteSockets);

	// Remove possible Authorizations from queue
	pthread_mutex_lock(&oHTTPThreadMutex);
	vector<AuthorizeHTTPConnection*>::iterator	oAuthorize = oServer->aAuthorizeHTTPConnections.begin();
	for(; oAuthorize<oServer->aAuthorizeHTTPConnections.end(); oAuthorize++){
		  if((*oAuthorize)->oMember->oUser==this){
		      (*oAuthorize)->bRemoved=true;
		  }
    }
	pthread_mutex_unlock(&oHTTPThreadMutex);



	// Remove from rooms
	vector<RoomMember*>::iterator oMember = aMemberOf.begin();
	for(; oMember<aMemberOf.end(); oMember++ ){
		(*oMember)->oRoom->removeUser(this);
	}

	// Remove messages
  	while(!aOutgoingMessages.empty()){
/*	    Message *oMessage = aOutgoingMessages.front();
	    oMessage->iNOFUsers--;
		if(oMessage->iNOFUsers<=0){
			delete oMessage;
		}*/
		removeMessage(aOutgoingMessages.front());
		aOutgoingMessages.pop();
    }


	// Remove from main userlist
	vector<User*>::iterator	oTempUser =	oServer->aUsers.begin();
	while( oTempUser< oServer->aUsers.end() ){
		if( (*oTempUser)->iSocket==iSocket ){
			oTempUser = oServer->aUsers.erase(oTempUser);
		}else{
			oTempUser++;
		}
	}

	// Remove from writing userlist
	oTempUser =	oServer->aWriteUsers.begin();
	while( oTempUser< oServer->aWriteUsers.end() ){
		if( (*oTempUser)->iSocket==iSocket ){
			oTempUser = oServer->aWriteUsers.erase(oTempUser);
		}else{
			oTempUser++;
		}
	}


//	shutdown(iSocket,SHUT_RDWR);
	close(iSocket);
}

bool User::operator==(User *oUser){
	return iSocket==oUser->iSocket;
}

bool User::operator==(int iUserSocket){
	return iSocket==iUserSocket;
}


/*****************************************************
User::initSocket

	Description:
	   Sets	the	socket to nonblocking

	Returns:
	   true	on success

	Author:
		2004-10-13	Daniel Önnerby

*****************************************************/
bool User::initSocket(){
	#ifdef DEBUG
		LOGSTREAM << "User::initSocket()\n";
	#endif

	int	iError=0;

	// Setting NONBLOCKING
	#ifdef _WIN32
		unsigned long tmp =	1;
		if((iError=ioctlsocket(iSocket,	FIONBIO, &tmp))	== SOCKET_ERROR){
			cerr <<	"ERROR:	ioctlsocket	" << iError	<< "\n";
			return false;
		}
	#else
		if((iError=fcntl(iSocket, F_SETFL, O_NONBLOCK))!=0){
			cerr <<	"ERROR:	fcntl "	<< iError << "\n";
			return false;
		}
	#endif

	return true;
}
/*****************************************************
User::read

	Description:

	Author:
		2004-10-13	Daniel Önnerby

*****************************************************/
bool User::read(char *sBuffer){

	int	iBytesRead=0;

	if(sReadBuffer.length()<READBUFFER_MAX_SIZE){
		iBytesRead=recv(iSocket,sBuffer,READBUFFER_SIZE,0);
		cout<<sBuffer<<endl;
		if(iBytesRead<=0){
			// iBytesRead is -1	on error and 0 if the user disconnect. In both cases delete	the	user.
			bDelete=true;
			return false;
		}else{
			// Append the buffer to	this users buffer
			sReadBuffer.append(sBuffer,iBytesRead);
			interpretBuffer();
		}

		return true;
	}else{
		// Buffer is to big, try to interpret or else it will drop the user
		interpretBuffer();
	}
}

/*****************************************************
User::interpretBuffer

	Description:

	Author:
		2004-10-13	Daniel Önnerby

*****************************************************/
bool User::interpretBuffer(){
	#ifdef DEBUG
		LOGSTREAM << "User::interpretBuffer()\n";
	#endif

        string *sNewMessage=SQ.new_string(true);

		if( oServer->oXMLParser.parse(&sReadBuffer) ){
			//Everything is	ok.	Check for message.
#ifdef DEBUG
			LOGSTREAM << "Valid	message	send\n";
#endif
			
			string type = oServer->oXMLParser.aXMLPath[0]; 

			if( type =="message" ){
				
				// A chat message is	send
				sNewMessage->swap(oServer->oXMLParser.sInnerXML);
				
				Message *oNewMessage = new Message(sNewMessage);
				LOGSTREAM <<"sNewMessage: "<<*sNewMessage<<endl; 

				// join a game
				if( !oServer->sendToRoom(oNewMessage,oServer->oXMLParser.getAttribute("room_id"),oServer->oXMLParser.getAttribute("room_url"),this ) ){
					// If the message can not be send, delete it.
					delete oNewMessage;
				}
				
				
			}else if(type== "game" ){
//				string haipai = oServer->oXMLParser.getAttribute("join");
//				LOGSTREAM << "haipai" << haipai;
				
				// parse haipai; 
				
				
				
				
			}else{
				SQ.delete_string(sNewMessage);
				if(type=="room:join" ){
					
					if(!oServer->joinRoom(this,
							      oServer->oXMLParser.getAttribute("room_id"),
							      oServer->oXMLParser.getAttribute("room_url"),
							      &oServer->oXMLParser.sInnerXML) ){

						// Unable to join room, drop the user
						// This will never happen in the current state
						//bDelete=true;
						//return false;
					}

					this->sendMessage(new Message(new string("welcome to game1\n")));
					
				}else{
/*					if(	oServer->oXMLParser.aXMLPath[0]=="system:setname" ){
						// Set the name	if it is not locked
						if(	!bNameLock ){
							#ifdef DEBUG
								LOGSTREAM << "Username set to "	<< *(oServer->oXMLParser.getAttribute("name")) << "\n";
							#endif
							sName =	*(oServer->oXMLParser.getAttribute("name"));
						}
					}else{*/
						if(sReadBuffer.length()>READBUFFER_MAX_SIZE){

							// Buffer to big, drop the user.


							bDelete=true;
							return false;
						}
//					}
				}

			}
			return true;
		}else{
			SQ.delete_string( sNewMessage );

			// Output is not finished or something went	wrong
			if(sReadBuffer.length()>READBUFFER_MAX_SIZE){
				#ifdef DEBUG
					LOGSTREAM << "DELETE User::interpretBuffer 2 length=" << sReadBuffer.length() << " \n";
				#endif
				bDelete=true;
			}
			return false;
		}

}

void User::sendMessage(class Message *oMessage){
	#ifdef DEBUG
		LOGSTREAM << "User::sendMessage(" << *oMessage->sMessage << ")\n";
	#endif
	 if(!bWriting){
		 FD_SET(iSocket,&oServer->aAllWriteSockets);
		 oServer->aWriteUsers.push_back(this);
		 bWriting=true;
	 }
	 aOutgoingMessages.push(oMessage);
}

bool User::write(){
	#ifdef DEBUG
		LOGSTREAM << "User::write()\n";
	#endif
	Message *oMessage = aOutgoingMessages.front();

	int	iLength	= oMessage->sMessage->length()+1;
	int	iWritten=send( iSocket,oMessage->sMessage->c_str()+iCurrentMsgWritten,iLength-iCurrentMsgWritten,0);
	
	
	if(iWritten==-1){
		#ifdef DEBUG
			LOGSTREAM << "DELETE User::write SEND failed with -1\n";
		#endif
		bDelete=true;
	}
	
	iCurrentMsgWritten+=iWritten;
	if(iCurrentMsgWritten>=iLength-1){
/*		oMessage->iNOFUsers--;
		if(oMessage->iNOFUsers<=0){
			delete oMessage;
		}*/
		removeMessage(oMessage);
		
		aOutgoingMessages.pop();
		iCurrentMsgWritten=0;
		if(aOutgoingMessages.size()==0){

			// Queue is	empty. Remove writing socket etc.
			FD_CLR(iSocket,	&oServer->aAllWriteSockets);
			bWriting=false;
			return false;
		}
	}
	return true;
}


bool User::removeMessage(Message *oMessage){
    oMessage->iNOFUsers--;
	if(oMessage->iNOFUsers<=0){
		delete oMessage;
		return true;
	}
	return false;
}
