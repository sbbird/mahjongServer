using namespace	std;

#include "config.h"

#include "xmlparser.h" // class's header file

/*****************************************************
XMLParser::XMLParser

	Description:
		Constructor

	Author:
		2004-10-13	Daniel Önnerby

*****************************************************/
XMLParser::XMLParser(){

	sEmptyString="";
}

/*****************************************************
XMLParser::~XMLParser

	Description:
		Destructor

	Author:
		2004-10-13	Daniel Önnerby

*****************************************************/
XMLParser::~XMLParser(){
	aXMLPath.clear();
//	  aAttributes.clear();

}

/*****************************************************
XMLParser::parse

	Description:
		Parse a	string to XML

	Author:
		2004-10-13	Daniel Önnerby

*****************************************************/
bool XMLParser::parse(string *sXML){
	int	iCurrentPos=0;
	int	iXMLLength=sXML->length();
	int	iFoundStart;
	int	iFoundEnd;
	int iInnerStart;
	int iInnerEnd;
	bool bFound;
	bool bAnyTags=false;
	string sCurrentTag="";

	int	iXMLTagPos=0;

	do{
		bFound=false;
		iFoundStart=sXML->find("<",iCurrentPos);
		if(iFoundStart!=string::npos){
			// Found start-tag
			iFoundEnd=sXML->find(">",iFoundStart+1);
			if(iFoundEnd!=string::npos){

				// Found end of	the	tag-tag
				if(sXML->at(iFoundStart+1)=='/' ){

					// XML End-tag
					sCurrentTag	= sXML->substr(
						iFoundStart+2,
						sXML->find_first_not_of(XMLTAGCHARS,iFoundStart+2)-iFoundStart-2
					);

					if(aXMLPath[iXMLTagPos-1]==sCurrentTag){
						// This	was	the	current	tag. End it.
						iXMLTagPos--;
					}
				}else{
					int	iEndOfTag=sXML->find_first_not_of(XMLTAGCHARS,iFoundStart+1);
					// Start tag
					if(aXMLPath.size()<=iXMLTagPos){
						// If the array	is to small
						aXMLPath.push_back(sXML->substr(iFoundStart+1,iEndOfTag-iFoundStart-1));
					}else{
						// Already exists
						aXMLPath[iXMLTagPos]=sXML->substr(iFoundStart+1,iEndOfTag-iFoundStart-1);
					}

					if(	sXML->at(iFoundEnd-1)!='/' ){
						// Do not increase if it's a short tag that	closes itself
						iXMLTagPos++;
					}
					if(!bAnyTags){
						// If this is the first	tag, then get the attributes
						getAttributes(sXML,iEndOfTag,iFoundEnd-1);
						iInnerStart=iFoundEnd+1;
						bAnyTags=true;
					}
				}
				iCurrentPos=iFoundEnd+1;
				bFound=true;
			}
		}
	}while(bFound && (!bAnyTags	|| iXMLTagPos!=0) );

	if(bAnyTags	&& iXMLTagPos==0){

		// Inner XML
		iInnerEnd=iFoundStart-1;
		
		int iNextMessage=sXML->find("<",iCurrentPos);
		if(iNextMessage==string::npos ){
			sOuterXML="";
			sOuterXML.swap(*sXML);
		}else{
			sOuterXML  = sXML->substr(iNextMessage);
			sXML->erase(iCurrentPos);
			sXML->swap(sOuterXML);
		}
		
		if(iInnerEnd<=iInnerStart){
			sInnerXML="";
		}else{
			sInnerXML = sOuterXML.substr(iInnerStart,iInnerEnd-iInnerStart+1);
		}
		
	}
	
	return bAnyTags	&& iXMLTagPos==0;
}

/*****************************************************
XMLParser::parse

	Description:
		Parse a	string to XML

	Author:
		2004-10-13	Daniel Önnerby

*****************************************************/
void XMLParser::getAttributes(string *sXML,int iCurrentPos,int iEndAttrib){

	bool bFound=false;
	aAttributes.clear();
	aAttributeValues.clear();

	do{
		bFound=false;

		// Find	start attribute	char
		int	iAttribStart	= sXML->find_first_of(XMLATTRIBCHARS,iCurrentPos);
		if(iAttribStart!=string::npos && iAttribStart<iEndAttrib){

			// Find	end	attribute char
			int	iAttribEnd		= sXML->find_first_not_of(XMLATTRIBCHARS,iAttribStart);
			if(iAttribEnd!=string::npos){

				// Check for the =-sign
				if(	sXML->at(iAttribEnd)=='=' ){

					// Find	value
					int	iEndAttribValue=sXML->find_first_of("\">",iAttribEnd+2);
					if(	iEndAttribValue!=string::npos ){

						aAttributes.push_back(sXML->substr(iAttribStart,iAttribEnd-iAttribStart));
						aAttributeValues.push_back(sXML->substr(iAttribEnd+2,iEndAttribValue-iAttribEnd-2));

						bFound=true;
						iCurrentPos=iEndAttribValue+1;
					}

				}

			}
		}

	}while(	bFound );


}


/*****************************************************
XMLParser::getAttribute

	Description:
		Parse the attributes of	the	first tag from the XML

	Author:
		2004-10-13	Daniel Önnerby

*****************************************************/
string *XMLParser::getAttribute(const char *sAttribute){

	vector<string>::iterator sCurrentAttribute = aAttributes.begin();
	int	i=0;
	for(; sCurrentAttribute<aAttributes.end(); sCurrentAttribute++){
		  if( *sCurrentAttribute ==	sAttribute ){
			  return &aAttributeValues[i];
		  }
		  i++;
	}

	return &sEmptyString;
}

/*****************************************************
XMLParser::getRootTag

	Description:
		Get	the	root tag

	Author:
		2004-10-13	Daniel Önnerby

*****************************************************/
string *XMLParser::getRootTag(){
	return &aXMLPath[0];
}


