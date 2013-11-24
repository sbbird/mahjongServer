#ifndef	_SERVER_H_
#define	_SERVER_H_

#include <istream>
#include <string>
#include <cstdio>
#include <vector>
#include <queue>
#include <stdio.h>
#include <time.h>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>


#ifdef _WIN32
	#include <winsock2.h>
	#include <map>
	#define	SHUT_RDWR 2
#else
	#include <sys/socket.h>
	#include <netinet/in.h>
	#include <netdb.h>
#endif


#define MAXEVENTS 64

using namespace	std;

class Server {
	public:
		Server();
		~Server();

		bool socketStartup();
		bool mainLoop();
		bool init();
		void exit();
		void cleanUp();
		bool joinRoom(class User *oUser,string *sRoomIdentifier,string *sRoomURL,string	*sAuthorizationXML);
//		bool joinRoomAuthorize(class User *oUser,class Room	*oRoom,string *sAuthorizationXML);
		bool sendToRoom(class Message *oMessage,string *sRoomIdentifier,string *sRoomURL,User *oUser);
		bool waitForURLRequest();

	public:

		bool bExit;
		XMLParser oXMLParser;
		XMLParser oXMLParserThread;
		char sSocketReadBuffer[READBUFFER_SIZE];

		int	iSocket;
		int iMaxSocket;
		#ifdef _EPOLL
		int     epfd;
		struct epoll_event event;
		struct epoll_event *events;
		#endif
		struct sockaddr_in oListenerSocket;
		fd_set aAllReadSockets;
		fd_set aAllWriteSockets;
		
		
		
		std::vector<class User*> aWriteUsers;
		std::vector<class User*> aUsers;
		std::vector<class Room*> aRooms;
		std::vector<class AuthorizeHTTPConnection*>	aAuthorizeHTTPConnections;

};

#endif

