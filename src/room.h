#ifndef	ROOM_H
#define	ROOM_H

#include <iostream>
#include <string>
#include <cstdio>
#include <vector>
#include <stdio.h>



class Room{
	public:
		// class constructor
		Room(string	*sRoomIdentifier,string	*sRoomURL);
		// class destructor
		~Room();
		bool sendMessage(Message *oMessage);
//		void join(User *oUser);
		void removeUser(User *oUser);
//		void unauthorized(User *oUser);
		bool authorize(User *oUser,string *sAuthorizeXML);


		string sIdentifier;
		string sURL;
		string sTitle;
		string sCurrentJoinMessage;
		int	iId;
		std::vector<class RoomMember*> aMembers;

};

#endif // ROOM_H
