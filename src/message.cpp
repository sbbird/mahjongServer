using namespace	std;

#include "config.h"

#include "message.h"
#include "stringqueue.h"

extern StringQueue SQ;

// class constructor
Message::Message(string	*sNewMessage){
	#ifdef DEBUG
		LOGSTREAM << "Message::Message\n";
	#endif
	sMessage	= sNewMessage;
	iNOFUsers	= 0;
}

// class destructor
Message::~Message(){
	#ifdef DEBUG
		LOGSTREAM << "Message::~Message\n";
	#endif
	SQ.delete_string(sMessage);
}
