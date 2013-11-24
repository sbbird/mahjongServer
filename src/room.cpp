using namespace	std;
#include "config.h"
#include "user.h"
#include "message.h"
#include "stringqueue.h"
#include "roommember.h"

extern StringQueue SQ;

#include "room.h"

/*****************************************************
Room::Room

	Description:
	    Room object constructor

	Input:
	    *sRoomIdentifier	= Room Identifier
		*sRoomURL			= Room URL

	Author:
		2005-03-01	Daniel Önnerby

*****************************************************/
Room::Room(string *sRoomIdentifier,string *sRoomURL){
	#ifdef DEBUG
		LOGSTREAM << "Room::Room(" << *sRoomIdentifier << "," << *sRoomURL << ")\n";
	#endif

	sIdentifier	= *sRoomIdentifier;
	sURL		= *sRoomURL;
}

/*****************************************************
Room::~Room

	Description:
	    Class destructor

	Author:
		2005-03-01	Daniel Önnerby

*****************************************************/
Room::~Room(){
}

bool Room::sendMessage(Message *oMessage){
	#ifdef DEBUG
		LOGSTREAM << "Room::sendMessage(" << (*oMessage->sMessage) << ")\n";
	#endif
	vector<RoomMember*>::iterator oMember = aMembers.begin();
	for(; oMember<aMembers.end(); oMember++){
		  if( (*oMember)->sendMessage(oMessage) ){
		      oMessage->iNOFUsers++;
		  }
	}

	return (oMessage->iNOFUsers!=0);
}

void Room::removeUser(User *oUser){
	#ifdef DEBUG
		LOGSTREAM << "Room::removeUser(" <<	oUser->iSocket << ")\n";
	#endif
	vector<RoomMember*>::iterator oCurrentMember = aMembers.begin();
	for(; oCurrentMember<aMembers.end()	; ){
		if( oUser==(*oCurrentMember)->oUser ){
            delete (*oCurrentMember);
			oCurrentMember=aMembers.erase(oCurrentMember);
		}else{
			oCurrentMember++;
		}
	}

	// Send	deleted	user to	all	other users
	string *sMessage=SQ.new_string();
	sMessage->assign("<userlist room_id=\""+sIdentifier+"\" room_url=\""+sURL+"\"><del id=\""+oUser->sId+"\"></del></userlist>\n");
	Message	*oNewMessage=new Message(sMessage);
	if( !sendMessage(oNewMessage) ){
		delete oNewMessage;
	}
}

bool Room::authorize(User *oUser,string *sAuthorizeXML){

    // Check if user is already a member
	vector<RoomMember*>::iterator oTempMember = aMembers.begin();
	while( oTempMember < aMembers.end() ){
 	    if( (*oTempMember)->oUser==oUser ){
			return false;
		}
		oTempMember++;
	}

    RoomMember *oMember=new RoomMember(this,oUser,sAuthorizeXML);

    oUser->aMemberOf.push_back(oMember);
    aMembers.push_back(oMember);

    return true;
}

