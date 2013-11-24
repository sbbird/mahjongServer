#ifndef	HTTPCONNECTION_H
#define	HTTPCONNECTION_H


#include <iostream>
#include <string>
#include <cstdio>
#include <vector>
#include <stdio.h>
#include <time.h>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>

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


using namespace	std;


class HTTPConnection{
    void sendLine(std::string sLine);

	public:
		// class constructor
		HTTPConnection();
		HTTPConnection(std::string *sNewURL);

		// class destructor
		~HTTPConnection();

		bool init(std::string	*sNewURL);
		bool urlEncode(std::string&	sString);
		bool getContent(std::string	sPOST,std::string *sReadBuffer);
		void recieve(std::string *sBuffer);


	public:
		std::string	 sURL;
		int			 iPort;
		bool		 bValid;
		bool		 bFinished;
		std::string	 sDomain;
		std::string	 sPath;
		std::string	 sReadBuffer;

		int	iSocket;
		sockaddr_in	oSocketArrd;
		int	iError;

		class Server *oServer;

};

#endif

