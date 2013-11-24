#define VERSION "0.2"

#define	PORT	2584
#define	SOCKETQUEUE	10
#define	MAINLOOPTIMEOUT	1
#define	READBUFFER_SIZE	4100

#define	READBUFFER_MAX_SIZE	4096
#define MAX_MESSAGE_QUEUE 16

// XMLParser
#define	XMLTAGCHARS	"abcdefghijklmnopqrstuvwxyz1234567890:_"
#define	XMLATTRIBCHARS "abcdefghijklmnopqrstuvwxyz1234567890:_"

// Logging
#ifndef	_WIN32
	 #define LOGSTREAM cout
	 #include <syslog.h>
#else
	 #define LOGSTREAM cout
#endif

