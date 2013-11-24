
#ifndef	STRINGQUEUE_H
#define	STRINGQUEUE_H

#include <iostream>
#include <string>
#include <cstdio>
#include <stack>
#include <stdio.h>

#define	MAX_STRING_QUEUE 20
/*
 * No description
 */
class StringQueue
{
	public:
		// class constructor
		StringQueue();
		// class destructor
		~StringQueue();

		std::string	*new_string(bool bClear);
		std::string	*new_string();
		void delete_string(std::string *sString);

		std::stack<std::string *> aQueue;
		int	iNOFStrings;

};

#endif // STRINGQUEUE_H
