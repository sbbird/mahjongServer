using namespace	std;

#include "config.h"
#include "xmlparser.h"
#include "server.h"

#include "httpconnection.h"

HTTPConnection::HTTPConnection(){
	iPort		= 80;
	bValid		= false;
	bFinished	= false;
	iError		= 0;
	oServer		= NULL;
}
HTTPConnection::HTTPConnection(std::string *sNewURL){
	iPort		= 80;
	bValid		= false;
	bFinished	= false;
	iError		= 0;
	init(sNewURL);
}

HTTPConnection::~HTTPConnection(){
}

bool HTTPConnection::init(std::string *sNewURL){
	sURL=*sNewURL;

	int	iTagHTTP=sURL.find("http://",0);
	if(iTagHTTP!=string::npos){

		// Find	the	domain end (next slash)
		int	iNextSlash=sURL.find("/",iTagHTTP+7);
		if(iNextSlash==string::npos){
			iNextSlash=sURL.length();
		}

		sDomain=sURL.substr(iTagHTTP+7,iNextSlash-(iTagHTTP+7));

		// Check for port
		int	iTagPort=sURL.find(":",iTagHTTP+7);
		if(iTagPort!=string::npos){
			if(iTagPort<iNextSlash){
				sDomain=sURL.substr(iTagHTTP+7,iTagPort-(iTagHTTP+7));
				std::string	sPort=sURL.substr(iTagPort,iNextSlash-iTagPort);
				int	iTempPort=atoi(sPort.c_str());
				if(iTempPort>0){
					iPort=iTempPort;
				}
			}
		}

		// Find	path
		sPath=sURL.substr(iNextSlash);

		return true;

	}else{
		return false;
	}
}

bool HTTPConnection::getContent(std::string	sPOST,std::string *sReadBuffer){

	hostent *oHostName;
	sockaddr_in oAddress;

	if( (iSocket = socket(AF_INET,SOCK_STREAM,0))==-1 ){
	    cerr << "socket() error\n";
		return false;
	}

	if( (oHostName=gethostbyname(sDomain.c_str()))==0 ) {
	    cerr << "gethostbyname error\n";
		return false;
	}
	oAddress.sin_family = AF_INET;
	oAddress.sin_port = htons(iPort);
	oAddress.sin_addr = *((in_addr *)oHostName->h_addr);

	if(connect(iSocket, (sockaddr *) &oAddress, sizeof(sockaddr))) {
	    cerr << "getContent error on connect\n";
	    return false;
	}

	urlEncode(sPOST);
//	sPOST="xml="+sPOST;
	char sContentLength[100];
	sprintf(sContentLength,"Content-length:	%d",sPOST.length());

	sendLine("POST "+sPath+" HTTP/1.0");
	sendLine("Host: "+sDomain);
//	sendLine("Content-Type: application/x-www-form-urlencoded");
	sendLine( sContentLength );
	sendLine("");
	sendLine(sPOST);
	sendLine("");

	recieve(sReadBuffer);

	return true;
}

void HTTPConnection::sendLine(std::string sLine){
	 sLine+="\n";
	 send(iSocket,sLine.c_str(),sLine.length(),0);
}

void HTTPConnection::recieve(std::string *sBuffer){
	 char sTemp[1025];
	 int iRead=0;
	 do{
	 	iRead = recv(iSocket,sTemp,1024,0);
		sBuffer->append(sTemp,iRead);
	 }while(iRead>0);


}

bool HTTPConnection::urlEncode(std::string&	sString){

	static	char hex[] = "0123456789ABCDEF";
	std::string	sResult="";

	for(int	i =	0; i < sString.length(); i++){
		if(isalnum(sString[i])){
			sResult	+= sString[i];
		}else{
			if(sString[i] == ' '){
				sResult	+= '+';
			}else{
				sResult	+= '%';
				sResult	+= hex[sString[i] /	16];
				sResult	+= hex[sString[i] %	16];
			}
		}
	}
	sString.swap(sResult);
}



