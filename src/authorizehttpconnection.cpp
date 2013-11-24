using namespace	std;
#include "config.h"
#include "user.h"
#include "room.h"
#include "roommember.h"
#include "stringqueue.h"
#include "xmlparser.h"
#include "authorizehttpconnection.h"
#include "server.h"

extern StringQueue			SQ;
extern pthread_mutex_t		oHTTPThreadMutex;
extern pthread_cond_t		oHTTPThreadCondition;


// class constructor
AuthorizeHTTPConnection::AuthorizeHTTPConnection(class RoomMember *oNewMember,class Server *oNewServer){
	#ifdef DEBUG
		LOGSTREAM << "AuthorizeHTTPConnection::AuthorizeHTTPConnection()\n";
	#endif
	bRemoved	= false;
	bFinished	= false;
	bDelete		= false;
    oMember     = oNewMember;
    oServer     = oNewServer;
}

// class destructor
AuthorizeHTTPConnection::~AuthorizeHTTPConnection(){

}

bool AuthorizeHTTPConnection::authorize(){
	#ifdef DEBUG
		LOGSTREAM << "AuthorizeHTTPConnection::authorize()\n";
	#endif

	sPOSTBuffer="";
	sPOSTBuffer.append("<room:join room_id=\"");
	sPOSTBuffer.append(oMember->oRoom->sIdentifier);
	sPOSTBuffer.append("\" user_ip=\"");
	sPOSTBuffer.append(inet_ntoa(oMember->oUser->oSocketArrd.sin_addr));
	sPOSTBuffer.append("\" nof_users=\"");

	char sTemp[16];
	sprintf(sTemp,"%d",oMember->oRoom->aMembers.size());

	sPOSTBuffer.append( sTemp );
	sPOSTBuffer.append("\">");
	sPOSTBuffer.append( oMember->sAuthorizeXML );
	sPOSTBuffer.append("</room:join>");

	std::string *sJoinURL = SQ.new_string(true);
	sJoinURL->assign(oMember->oRoom->sURL+"/join/");

	if( init(sJoinURL) ){
 	    return true;
	}else{
 	    return false;
 	}
}


bool AuthorizeHTTPConnection::sendAuthorization(){
	#ifdef DEBUG
		LOGSTREAM << "AuthorizeHTTPConnection::sendAuthorization\n";
	#endif
	 std::string sRead,sTemp;
	 std::string *sRootTag,*sNickName;

	 if( getContent(sPOSTBuffer,&sRead) ){

		 if( oServer->oXMLParserThread.parse(&sRead) ){
		 	 pthread_mutex_lock(&oHTTPThreadMutex);
		 	 if(!bRemoved){
		 	     sRootTag = oServer->oXMLParserThread.getRootTag();
		 	     if( *sRootTag=="room:authorized" ){
						bResultAuthorized = true;
                        sServerXML.swap(oServer->oXMLParserThread.sInnerXML);
			     }else{
						bResultAuthorized = false;
			     }
	 		 }
		 	 pthread_mutex_unlock(&oHTTPThreadMutex);
		 }
	 	 bFinished = true;
		 return true;
     }else{

		// Invalid respons from the webserver
		bResultAuthorized = false;
	 	bFinished = true;
		return false;
     }
}


