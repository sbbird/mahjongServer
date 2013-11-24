using namespace	std;

#include "config.h"
#include "stringqueue.h" //	class's	header file

// class constructor
StringQueue::StringQueue(){
	iNOFStrings=0;
}

// class destructor
StringQueue::~StringQueue(){
	std::string	*sTemp;
	while(!aQueue.empty()){
		sTemp =	aQueue.top();
		delete sTemp;
		aQueue.pop();
	}
}

std::string	*StringQueue::new_string(bool bClear=false){
	if(iNOFStrings!=0){
		std::string	*sString=aQueue.top();
		aQueue.pop();
		iNOFStrings--;
		if(bClear){
			sString->assign("");
		}
		return sString;
	}else{
		return (new	string());
	}
}

std::string	*StringQueue::new_string(){
	if(iNOFStrings!=0){
		std::string	*sString=aQueue.top();
		aQueue.pop();
		iNOFStrings--;
		return sString;
	}else{
		return (new	string());
	}
}

void StringQueue::delete_string(std::string	*sString){
	if( iNOFStrings>=MAX_STRING_QUEUE || sString->size()>READBUFFER_SIZE ){
		delete sString;
	}else{
		aQueue.push(sString);
		iNOFStrings++;
	}
}


