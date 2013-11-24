using namespace	std;

#include "config.h"
#include "xmlparser.h"
#include "user.h"
#include "room.h"
#include "message.h"
#include "httpconnection.h"
#include "authorizehttpconnection.h"
#include "stringqueue.h"
#include "roommember.h"

#include "server.h"

extern StringQueue			SQ;
extern pthread_mutex_t		oHTTPThreadMutex;
extern pthread_cond_t		oHTTPThreadCondition;
extern int 					iPort;

/*****************************************************
Server::Server

	Description:
		Default	constructor

	Author:
		2004-10-13	Daniel Önnerby

*****************************************************/
Server::Server(){

	#ifdef DEBUG
		LOGSTREAM << "Server::Server\n";
	#endif
	bExit=false;
	iMaxSocket=0;

}


/*****************************************************
Server::~Server

	Description:
		Destructor

	Author:
		2004-10-13	Daniel Önnerby

*****************************************************/
Server::~Server(){
	#ifdef DEBUG
		LOGSTREAM << "Server::~Server\n";
	#endif

	shutdown(iSocket, SHUT_RDWR);
	#ifdef _WIN32
		closesocket(iSocket);
	#else
		close(iSocket);
	#endif

//	pthread_mutex_destroy(&oHTTPThreadMutex);
//	pthread_cond_destroy(&oHTTPThreadCondition);

}

/*****************************************************
Server::socketStartup

	Description:
		Initialize the socket and start	listening

	Returns:


	Author:
		2004-10-13	Daniel Önnerby

*****************************************************/
bool Server::socketStartup(){
	#ifdef DEBUG
		LOGSTREAM << "Server::socketStartup	FD_SETSIZE=" <<	FD_SETSIZE << "\n";
	#endif

	#ifdef _WIN32
		WSADATA	wsaData;
		if(WSAStartup(MAKEWORD(1, 1), &wsaData)){
			cerr <<	"WSAStartup	error\n";
			return false;
		}
	#endif

	if ((iSocket = socket(AF_INET, SOCK_STREAM,	0))	== -1) {
		cerr <<	"Unable	to create listener socket.\n";
		return false;
	}
	#ifdef DEBUG
		LOGSTREAM  << "LISTEN ON " <<	iSocket	<< "\n";
	#endif

	oListenerSocket.sin_family			= AF_INET;
	oListenerSocket.sin_port			= htons(iPort);
	oListenerSocket.sin_addr.s_addr		= htonl(INADDR_ANY);

	// Bind	the	listener socket
	if(bind(iSocket,(struct	sockaddr *)&oListenerSocket,sizeof(struct sockaddr)) ==	-1)	{
		cerr <<	"Unable	to bind	listener socket.\n";
		return false;
	}

	
#ifdef _EPOLL
   	if ( make_socket_non_blocking(iSocket) == -1){
		cerr << "Make nonblocking failed.\n";
		return false;
	}
#endif

	
	
	// Try to listen on	the	socket
	if(	listen(iSocket,SOCKETQUEUE)==-1	){
		cerr <<	"Unable	to listen on the listener socket.\n";
		return false;
	}
	iMaxSocket	= iSocket+1;

#ifdef _EPOLL
	if((epfd = epoll_create1(0)) == -1){
		cerr << "epoll create failed.\n";
		return false
		      
	};

	event.data.fd = iSocket;
	event.events = EPOLLING | EPOLLET;
	if ( epoll_ctl(epfd, EPOLL_CTL_ADD, iSocket, &event) == -1){
		cerr << "epoll register failed.\n";
		return false;
	}
	events = malloc(MAXEVENTS, sizeof event);
#endif
	

	return true;

}

#ifdef _EPOLL
int make_socket_non_blocking (int sfd)
{
  int flags, s;

  flags = fcntl (sfd, F_GETFL, 0);
  if (flags == -1)
    {
      perror ("fcntl");
      return -1;
    }

  flags |= O_NONBLOCK;
  s = fcntl (sfd, F_SETFL, flags);
  if (s == -1)
    {
      perror ("fcntl");
      return -1;
    }

  return 0;
}
#endif


/*****************************************************
Server::mainLoop
	Description:


	Returns:


	Author:
		2004-10-13	Daniel Önnerby

*****************************************************/
bool Server::mainLoop(){
	#ifdef DEBUG
		LOGSTREAM << "Server::mainLoop\n";
	#endif

	// declare variables
	struct timeval oTimeout;
	fd_set aReadResultSockets,aWriteResultSockets;
	int	iNewSocket=0;
	int	iNOFReadySockets=0;

	// Add the listener-socket
	FD_ZERO(&aAllReadSockets);
	FD_SET(iSocket,&aAllReadSockets);


/*
#ifdef _EPOLL
	while(!bExit){
		int n, i;
		if((n = epoll_wait(epfd, events, MAXEVENTS, -1)) == -1){
			cerr << "epoll wait error.\n";
			exit(EXIT_FAILURE);
		};

		for( i = 0; i < n; i++){
		       
			if ( events[i].events & EPOLLIN){ // read event
				
				if(iSocket = events[i].data.fd){
					// a new connection
					sockaddr_in	oNewClientSocket;
					int	iNewSocket;

#ifdef _WIN32
					int	iNewClientSocketLength = sizeof(oNewClientSocket);
#else
					socklen_t iNewClientSocketLength = sizeof(oNewClientSocket);
#endif
					if(	(iNewSocket=accept(iSocket, einterpret_cast<sockaddr *>(&oNewClientSocket), &iNewClientSocketLength)) != -1 ){
						// New connection accepted
						make_socket_non_blocking (iNewSocket);

						//register epoll event
						
						event.events = EPOLLIN | EPOLLET;
						event.data.fd = iNewSocket;
						if ( epoll_ctl(epfd, EPOLL_CTL_ADD, iNewSocket, &event) == -1){
							cerr << "epoll register failed.\n";
							return false;
						}
						
						
						// Create a	new	User in	the	aUsers-array
						User* newUser = new User(this,iNewSocket,oNewClientSocket); 

					        aUsers.push_back(newUser);
						//iGame.add(newUser);
						aUsers.back()->sendMessage(new Message(new string("hello")));
						
						//
						
						

						
					 	if(iNewSocket>=iMaxSocket){
				   		    iMaxSocket = iNewSocket+1;
				   		}
					}else{
						cerr <<	"Socket	accept failed.\n";
					}
					
				} else {
					// read data
				
			
				}
			} else if ( events[i].events & EPOLLOUT){ // write event
				// flush write
				
			} else {

				// other eventskkk
			}
		
		}

	};
		
#else
*/
	while(!bExit){

		//Make a temporary fd_set as a copy	of all the sockets
		aReadResultSockets	= aAllReadSockets;
		aWriteResultSockets	= aAllWriteSockets;

		// Set the timer every time	(counts	down on	linux-systems but not on windows)
		oTimeout.tv_sec=MAINLOOPTIMEOUT;
		oTimeout.tv_usec=0;
		if ( (iNOFReadySockets=select(iMaxSocket, &aReadResultSockets, &aWriteResultSockets, NULL, &oTimeout)) < 0){
			LOGSTREAM << "Socket select	interupted\n";
		}else{

			if(iNOFReadySockets!=0){
				
				// Check if	there are new connections waiting
				if(	FD_ISSET(iSocket,&aReadResultSockets) ){
					sockaddr_in	oNewClientSocket;
					int	iNewSocket;

					#ifdef _WIN32
						int	iNewClientSocketLength = sizeof(oNewClientSocket);
					#else
						socklen_t iNewClientSocketLength = sizeof(oNewClientSocket);
					#endif


					// Try to accept the new connection
					if(	(iNewSocket=accept(iSocket,	reinterpret_cast<sockaddr *>(&oNewClientSocket), &iNewClientSocketLength)) != -1 ){

						// New connection accepted
				    
						// Create a	new	User in	the	aUsers-array
						User* newUser = new User(this,iNewSocket,oNewClientSocket); 
					        aUsers.push_back(newUser);
						//iGame.add(newUser);
						aUsers.back()->sendMessage(new Message(new string("Welcome\n")));
						
						//
					 	if(iNewSocket>=iMaxSocket){
				   		    iMaxSocket = iNewSocket+1;
				   		}
					}else{
						cerr <<	"Socket	accept failed.\n";
					}
					iNOFReadySockets--;
				}

				for(int i=0;i<aUsers.size();i++){
					if(aUsers[i]->bDelete){
						delete aUsers[i];
						i--;
					}
				}

				// Check for writing sockets
				vector<User*>::iterator	oUser =	aWriteUsers.begin();
				for(; oUser<aWriteUsers.end() && iNOFReadySockets>0; ){
					bool bEraseUserFromQueue=false;
					if(	FD_ISSET( (*oUser)->iSocket,&aWriteResultSockets) ){
						if(!(*oUser)->write()){
							// Remove	the	user from the aWriteUsers vector
							bEraseUserFromQueue=true;
						}
						iNOFReadySockets--;
					}
					if(bEraseUserFromQueue){
						oUser=aWriteUsers.erase(oUser);
					}else{
						oUser++;
					}
				}


				// Check for reading
				// vector<User*>::iterator oUser = aUsers.begin();
				oUser =	aUsers.begin();
				for(; oUser<aUsers.end() &&	iNOFReadySockets>0;	oUser++){
					if(	FD_ISSET( (*oUser)->iSocket,&aReadResultSockets) ){
						(*oUser)->read(sSocketReadBuffer);
						iNOFReadySockets--;
					}
				}

			}else{

				// I got a	timeout, lets do some cleanup
				cleanUp();
			}


			// Check for finished authorizations
			if( pthread_mutex_trylock(&oHTTPThreadMutex)==0 ){
				vector<AuthorizeHTTPConnection*>::iterator oConnection = aAuthorizeHTTPConnections.begin();
				while( oConnection<aAuthorizeHTTPConnections.end() ){

					if( (*oConnection)->bFinished && !(*oConnection)->bRemoved && !(*oConnection)->bDelete ){
						if( (*oConnection)->bResultAuthorized ){
							(*oConnection)->oMember->authorized();
						}else{
							(*oConnection)->oMember->reject();
						}
					}

					if( (*oConnection)->bFinished || (*oConnection)->bRemoved ){
						(*oConnection)->bDelete=true;
					}

					oConnection++;
				}
				pthread_mutex_unlock(&oHTTPThreadMutex);
			}

		}

	}
	
//#endif  // ifdef _EPOLL


	this->exit();
	// clear all users from the arrays
	while( !aUsers.empty() ){
		User *oTempUser=aUsers.back();
		delete oTempUser;
	}

	vector<Room*>::iterator	oRoom =	aRooms.begin();
	// clear all elements from the array
	for(; oRoom<aRooms.end(); oRoom++){
		delete *oRoom;
		aRooms.erase(oRoom);
	}


	return true;

}

/*****************************************************
Server::init

	Description:


	Returns:


	Author:
		2004-10-13	Daniel Önnerby

*****************************************************/
bool Server::init(){
	#ifdef DEBUG
		LOGSTREAM << "Server::init\n";
	#endif
	bool bSuccess=false;

	bSuccess=socketStartup();
	return bSuccess;
}


/*****************************************************
Server::exit

	Description:


	Returns:


	Author:
		2004-10-13	Daniel Önnerby

*****************************************************/
void Server::exit(){
	#ifdef DEBUG
		LOGSTREAM << "Server::exit() 1\n";
	#endif
	bExit=true;

	pthread_mutex_lock(&oHTTPThreadMutex);
	int iRC = pthread_cond_signal(&oHTTPThreadCondition);
	pthread_mutex_unlock(&oHTTPThreadMutex);

	#ifdef DEBUG
		LOGSTREAM << "Server::exit() 2\n";
	#endif
}

/*****************************************************
Server::cleanUp

	Description:


	Returns:


	Author:
		2004-10-13	Daniel Önnerby

*****************************************************/
void Server::cleanUp(){
/*
	vector<User*>::iterator	oUser =	aUsers.begin();

	// clear all elements from the array
	for(; oUser<aUsers.end(); oUser++){
		if((*oUser)->bDelete){
			delete *oUser;
			aUsers.erase(oUser);
		}
	}
*/
}

/*****************************************************
Server::sendToRoom

	Description:
	    Sends a message to all users in a room
	    The function also manipulates the root-tag
	    attributes and adds room_id room_url and user_id

    Input:
	    *oMessage			= Message to be send
	    *sRoomIdentifier	= Rooms identifier
	    *sRoomURL			= Rooms base URL
	    *oUser				= User who is sending the message

	Author:
		2004-10-13	Daniel Önnerby

*****************************************************/
bool Server::sendToRoom(class Message *oMessage,string *sRoomIdentifier,string *sRoomURL,User *oUser){

	int	iFirstTag,iLastTag;
	string sTemp;

    if( (*oMessage->sMessage)!="" ){

        vector<RoomMember*>::iterator oMember = oUser->aMemberOf.begin();
		for(; oMember<oUser->aMemberOf.end(); oMember++){
			  if( (*oMember)->oRoom->sIdentifier == *sRoomIdentifier && (*oMember)->oRoom->sURL == *sRoomURL ){

           		  // Add root-tag with attributes
        		  oMessage->sMessage->insert(0,"<"+*(oXMLParser.getRootTag())+" room_id=\""+*sRoomIdentifier+"\" room_url=\""+*sRoomURL+"\" user_id=\""+oUser->sId+"\">");
        		  oMessage->sMessage->append("</"	+ *(oXMLParser.getRootTag()) + ">");

				  return (*oMember)->oRoom->sendMessage(oMessage);
			  }
		}
		return false;
	}else{
		#ifdef DEBUG
			LOGSTREAM << "Server::sendToRoom empty message send	" << iFirstTag << "	" << iLastTag << "\n";
		#endif
		return false;
	}


}

/*****************************************************
Server::joinRoom

	Description:
	    Locates or creates the room and tries to join

	Input:
 	    *oUser				= The User
 		*sRoomIdentifier	= Rooms identifier
 		*sRoomURL			= Rooms base URL
 		*sLogin				= Users login for the room in question
 		*sPassword			= Users password for the room in question

	Author:
		2005-03-01	Daniel Önnerby

*****************************************************/
bool Server::joinRoom(class	User *oUser,string *sRoomIdentifier,string *sRoomURL,string	*sAuthorizationXML){
	#ifdef DEBUG
		LOGSTREAM << "Server::joinRoom " <<	(*sRoomIdentifier) <<  "\n";
	#endif

	vector<Room*>::iterator	oRoom =	aRooms.begin();
	for(; oRoom<aRooms.end(); oRoom++){
		  if( (*oRoom)->sIdentifier	== *sRoomIdentifier	){
			  if( (*oRoom)->sURL ==	*sRoomURL ){
			  	  // Room found, try to authorize
                  return (*oRoom)->authorize(oUser,sAuthorizationXML);
			  }
		  }
	}

	// Room not found, create the room
	Room *oNewRoom = new Room(sRoomIdentifier,sRoomURL);
	aRooms.push_back(oNewRoom);

	// Authorize
    return oNewRoom->authorize(oUser,sAuthorizationXML);
}


/*****************************************************
Server::joinRoomAuthorize

	Description:
	    Add the user to the queue for authorization

	Input:
 	    *oUser				= The User
		*oRoom 				= Room
 		*sLogin				= Users login for the room in question
 		*sPassword			= Users password for the room in question


	Author:
		2005-03-01	Daniel Önnerby

*****************************************************/
/*bool Server::joinRoomAuthorize(class User *oUser,class Room	*oRoom,string *sAuthorizationXML){

    // Check if user is already a member
	vector<User*>::iterator	oTempUser =	oRoom->aUsers.begin();
	while( oTempUser<oRoom->aUsers.end()){
 	    if( (*oTempUser)== oUser ){
			return false;
		}
		oTempUser++;
	}


	AuthorizeHTTPConnection	*oNewConnection = new AuthorizeHTTPConnection(oUser,oRoom,this);
	oNewConnection->authorize(sLogin,sPassword);

	pthread_mutex_lock(&oHTTPThreadMutex);
	aAuthorizeHTTPConnections.push_back( oNewConnection	);
	pthread_cond_signal(&oHTTPThreadCondition);
	pthread_mutex_unlock(&oHTTPThreadMutex);

	return true;
}
*/
/*****************************************************
Server::waitForURLRequest

	Description:


	Input:


	Author:
		2005-03-01	Daniel Önnerby

*****************************************************/
bool Server::waitForURLRequest(){
	int iRC=0;
	while(!bExit){

		pthread_mutex_lock(&oHTTPThreadMutex);
		iRC=pthread_cond_wait(&oHTTPThreadCondition,&oHTTPThreadMutex);

		AuthorizeHTTPConnection *oTempConnection;

		vector<AuthorizeHTTPConnection*>::iterator oConnection = aAuthorizeHTTPConnections.begin();
		while( oConnection<aAuthorizeHTTPConnections.end() ){

			if( (*oConnection)->bDelete ){
				delete (*oConnection);
				oConnection = aAuthorizeHTTPConnections.erase(oConnection);
			}else{
				if( !(*oConnection)->bFinished ){
					pthread_mutex_unlock(&oHTTPThreadMutex);
					(*oConnection)->sendAuthorization();
					pthread_mutex_lock(&oHTTPThreadMutex);
				}
				oConnection++;
			}


		}

		pthread_mutex_unlock(&oHTTPThreadMutex);
	}

}

