#include <signal.h>
#include <iostream>
#include <time.h>
#include <sched.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#include <time.h>
#include <sys/types.h>
#ifdef _WIN32
	#include <winsock2.h>
	#include <map>
	#define	SHUT_RDWR 2
#else
	#include <sys/socket.h>
	#include <netdb.h>
#endif


using namespace	std;

#include "config.h"
#include "xmlparser.h"
#include "user.h"
#include "server.h"
#include "httpconnection.h"
#include "stringqueue.h"

Server *oServer;
StringQueue	SQ;
pthread_mutex_t	   oHTTPThreadMutex		= PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t	   oHTTPThreadCondition = PTHREAD_COND_INITIALIZER;
int iPort;

void signalHandler(int iSignal){
//	oServer->exit();
	oServer->bExit=true;
}

void *getURLThread(void	*parameter){

	oServer->waitForURLRequest();

}


int	main(int argc, char	*argv[]){
	iPort = PORT;

    // Read arguments
   	bool bUseLogFile=false;
    for(int i=0;i<argc;i++){
        if( strcmp(argv[i],"--help")==0 ){
        	cout << "\n" <<
"Usage:\n" <<
"     xmlsocketd [--help] [-D] [-p PORT]\n\r" <<
"\n\r" <<
"     Version: " << VERSION << "\n\r" <<
"\n\r" <<
"     Parameters:\n\r" <<
"\n\r" <<
"         -D        Run the server as a daemon.\n\r" <<
"         -p PORT   Set port to use.\n\r" <<
"                   Default port is 2584\n\r" <<
"         --help    Show this help.\n\r" <<
"\n\r" <<
"";
        	return 0;
        }
        if( strcmp(argv[i],"-D")==0 ){

			// Run as daemon
			#ifndef	_WIN32
				cout << "Starting XMLSocketd daemon...\n";
				daemon(0,0);
			#endif

        }
        if( strcmp(argv[i],"-p")==0 && i+1<argc ){

			// Set the port
			iPort = atoi(argv[i+1]);
			if(iPort<=0){
				iPort = PORT;
			}

        }
    }

// Get address
/*   int iError=0;
   addrinfo oAddrInfo;
   iError=getaddrinfo(
*/
/*	XMLParser test

	XMLParser oParser;
	std::string	sString;
	std::string	sTemp;
	sString.assign("<message tjo=\"frwkfewjkl\" room_id=\"123456fdsfew\" room_url=\"123456fdsfew\"><tjo><tjo><tjo><tjo></tjo></tjo></tjo></tjo></message>");
	std::string *sAttribute;

	int iStartTime=clock();

	for(int i=0;i<20000;i++){
	    oParser.parse(&sString,&sTemp);
	    sAttribute = oParser.getAttribute("room_url");
	    sTemp.swap(sString);
	}
	int iEndTime=clock();

	cout << "Start time " << iStartTime << "\n";
	cout << "End time   " << iEndTime << "\n";
	cout << "Total time " << (iEndTime-iStartTime) << "\n";
	return 0;
*/

/*
	SocketClient oURL("onnerby.se",80);
	oURL.SendLine("POST	/~daniel/test3.php HTTP/1.0");
	oURL.SendLine("Host: onnerby.se");
	oURL.SendLine("Content-length: 15");
	oURL.SendLine("");
	oURL.SendLine("tjo=tjobbadobba");
	oURL.SendLine("");


	while (1) {
	  string l = oURL.ReceiveLine();
	  if (l.empty()) break;
	  cout << l;
	  cout.flush();
	}
	return 0;
*/
	#ifndef	_WIN32
//		openlog("xmlsocketd",LOG_CONS|LOG_NOWAIT,LOG_DAEMON);
	#endif


	cout <<	"Starting server...\n";
	oServer	= new Server();

	if(!oServer){
		//cout <<	"ERROR:	Can´t create Server-object\n";
	}else{

		if(oServer->init()){


			pthread_t		oThread;
			pthread_attr_t	oThreadAttrib;
			int				*parm=NULL;
			int				iRC=0;
			iRC	= pthread_attr_init(&oThreadAttrib);
			if(iRC!=0){
				LOGSTREAM << "FAILED to	pthread_attr_init with error " << iRC << "\n";
				return 1;
			}
			iRC	= pthread_create(&oThread,NULL,getURLThread,(void *)parm);
			if(iRC!=0){
				LOGSTREAM << "FAILED to	pthread_create with	error "	<< iRC << "\n";
				return 1;
			}


			signal(	SIGINT,	signalHandler );
			signal(	SIGTERM, signalHandler );

			oServer->mainLoop();
			void *oStatus;
			pthread_join(oThread,&oStatus);

			delete oServer;

			#ifndef	_WIN32
//				closelog();
			#endif
			return 0;
		}else{
			// Can't init the server
			delete oServer;
			#ifndef	_WIN32
//				closelog();
			#endif
		}
	}
	return 1;
}
