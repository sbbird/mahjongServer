#ifndef	_USER_H_
#define	_USER_H_

#include <iostream>
#include <string>
#include <cstdio>
#include <vector>
#include <queue>
#include <stdio.h>
#include <time.h>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>

#ifdef _WIN32
	#include <winsock2.h>
	#include <map>
	#define	SHUT_RDWR 2
#else
	#include <sys/socket.h>
	#include <netinet/in.h>
	#include <netdb.h>
	#include <arpa/inet.h>
#endif



class User{
	public:
		User();
		User(int iNewSocket,sockaddr_in	oNewSocketArrd);
		User(class Server *oMainServer,int iNewSocket,sockaddr_in oNewSocketArrd);
		bool initSocket();
		~User();
		bool read(char *sBuffer);
		bool write();
		bool interpretBuffer();
		void sendMessage(class Message *oMessage);
		bool operator==(User *oUser);
		bool operator==(int iUserSocket);

		string sName;
		bool bNameLock;
		string sId;
		string sUserXML;
		string sReadBuffer;

		std::queue<class Message*>	aOutgoingMessages;
		int	iCurrentMsgWritten;
		bool bWriting;

		std::vector<class RoomMember*> aMemberOf;

		bool bDelete;
		class Server *oServer;

		int	iSocket;
		sockaddr_in	oSocketArrd;
		// No description
		bool removeMessage(Message *oMessage);

};

#endif
