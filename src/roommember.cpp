using namespace	std;
#include "config.h"
#include "xmlparser.h"
#include "server.h"
#include "user.h"
#include "room.h"
#include "message.h"
#include "stringqueue.h"
#include "authorizehttpconnection.h"

extern StringQueue SQ;
extern pthread_mutex_t		oHTTPThreadMutex;
extern pthread_cond_t		oHTTPThreadCondition;
extern Server *oServer;

#include "roommember.h"


RoomMember::RoomMember(class Room *oNewRoom,class User *oNewUser,string *sNewAuthorizeXML){
	#ifdef DEBUG
		LOGSTREAM << "RoomMember::RoomMember " << (*sNewAuthorizeXML) <<  "\n";
	#endif
	
	oUser = oNewUser;
	oRoom = oNewRoom;
	
    bAuthorized=false;
    bDelete=false;
    bAuthorizationOpen=true;
    
    sAuthorizeXML = *sNewAuthorizeXML;
    oAuthorization = new AuthorizeHTTPConnection(this,oServer);

    if(!oAuthorization->authorize()){
        reject();
    }else{
    	pthread_mutex_lock(&oHTTPThreadMutex);
    	oServer->aAuthorizeHTTPConnections.push_back( oAuthorization );
    	pthread_cond_signal(&oHTTPThreadCondition);
    	pthread_mutex_unlock(&oHTTPThreadMutex);
    }
}

void RoomMember::reject(){
	#ifdef DEBUG
		LOGSTREAM << "RoomMember::reject()\n";
	#endif
    bAuthorized=false;
    bDelete=true;

	if( !oUser->bDelete ){
		string *sMessage=SQ.new_string();
		sMessage->assign("<room:unauthorized room_id=\""+oRoom->sIdentifier+"\" room_url=\""+oRoom->sURL+"\"></room:unauthorized>\n");

		Message	*oNewMessage=new Message(sMessage);
		oNewMessage->iNOFUsers++;
		oUser->sendMessage(oNewMessage);
	}

}

bool RoomMember::authorized(){
	#ifdef DEBUG
		LOGSTREAM << "RoomMember::authorized() " << oAuthorization->sServerXML << "\n";
	#endif

    sServerXML.swap(oAuthorization->sServerXML);
	#ifdef DEBUG
		LOGSTREAM << "SERVERXML " << sServerXML << "\n";
	#endif

    // Send to the client that it has been authorized
	string *sMessage=SQ.new_string();
	sMessage->assign("<room:authorized room_id=\""+oRoom->sIdentifier+"\" room_url=\""+oRoom->sURL+"\" user_id=\""+oUser->sId+"\">"+sServerXML+"</room:authorized>\n");

	Message	*oNewMessage=new Message(sMessage);
	oNewMessage->iNOFUsers++;
	oUser->sendMessage(oNewMessage);


	// Send	the	new	user to	all	other users
	sMessage=SQ.new_string();
	sMessage->assign("<userlist room_id=\""+oRoom->sIdentifier+"\" room_url=\""+oRoom->sURL+"\"><add id=\""+oUser->sId+"\"><prefs:server>"+sServerXML+"</prefs:server><prefs:user>"+sUserXML+"</prefs:user></add></userlist>\n");

	oNewMessage=new	Message(sMessage);
	if( !oRoom->sendMessage(oNewMessage) ){
		delete oNewMessage;
	}

	// Send	the	whole userlist to the new user
	sMessage=SQ.new_string();
	sMessage->assign("<userlist room_id=\""+oRoom->sIdentifier+"\" room_url=\""+oRoom->sURL+"\">");

	vector<RoomMember*>::iterator oCurrentMember = oRoom->aMembers.begin();
	for(; oCurrentMember<oRoom->aMembers.end() ; oCurrentMember++ ){
		sMessage->append("<add id=\""+(*oCurrentMember)->oUser->sId+"\"><prefs:server>"+(*oCurrentMember)->sServerXML+"</prefs:server><prefs:user>"+(*oCurrentMember)->sUserXML+"</prefs:user></add>\n");
	}

	sMessage->append("</userlist>\n");
	oNewMessage=new	Message(sMessage);
	oUser->sendMessage(oNewMessage);

    bAuthorized=true;
}


RoomMember::~RoomMember(){
    if(bAuthorizationOpen){
        oAuthorization->bDelete=true;
    }
}

// No description
bool RoomMember::sendMessage(class Message *oMessage){
    if( oUser->aOutgoingMessages.size() < MAX_MESSAGE_QUEUE && bAuthorized ){
	    oUser->sendMessage(oMessage);
	    return true;
    }else{
	    return false;
    }
}
