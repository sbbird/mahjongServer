#ifndef	XMLPARSER_H
#define	XMLPARSER_H

#include <iostream>
#include <string>
#include <cstdio>
#include <vector>
#include <stdio.h>
#include <unistd.h>


class XMLParser{
	public:
		// class constructor
		XMLParser();
		// class destructor
		~XMLParser();

		bool parse(string *sXML);
		string sEmptyString;
		string sInnerXML;
		string sOuterXML;

		string *getAttribute(const char	*sAttribute);

	public:
		void getAttributes(string *sXML,int	iCurrentPos,int	iEndAttrib);
		string *getRootTag();

	// Variables
	public:
		vector<string> aXMLPath;


	// Variables
	private:
		vector<string> aAttributes,aAttributeValues;
};

#endif // XMLPARSER_H

