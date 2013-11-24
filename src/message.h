
#ifndef	MESSAGE_H
#define	MESSAGE_H

#include <iostream>
#include <string>
#include <cstdio>
#include <vector>
#include <stdio.h>


class Message
{
	public:
		string *sMessage;
		int	iNOFUsers;

	public:
		// class constructor
		Message(string *sNewMessage);
		// class destructor
		~Message();

};

#endif // MESSAGE_H
