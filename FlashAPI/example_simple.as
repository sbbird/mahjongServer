
oRoom=new XMLSocketd.Room("chat");

oRoom.onConnect		= connected;
oRoom.onJoin		= joinedRoom;
oRoom.onUserChange	= updateUserList;
oRoom.onError		= addLog;
oRoom.onMessage		= message;

oRoom.connect();

aMessages=new Array();

/*****************************************************
function connected

	Description:
		Called when socket is connected or not

	Author:
		2005-03-22	Daniel Önnerby

*****************************************************/
function connected(bSuccess){
	if(bSuccess){
		addLog("Connected successfully");
	}else{
		addLog("Connected unsuccessfully");
	}
}

/*****************************************************
function joinedRoom

	Description:
		Called when user has tried to join the room

	Author:
		2005-03-22	Daniel Önnerby

*****************************************************/
function joinedRoom(bSuccess){
	if(bSuccess){
		addLog("Joined successfully");
	}else{
		addLog("Joined unsuccessfully");
	}
}

/*****************************************************
function error

	Description:
		Called on error

	Author:
		2005-03-22	Daniel Önnerby

*****************************************************/
function error(sError){
}


/*****************************************************
function message

	Description:
		Called when a message-xmltag has come in.

	Author:
		2005-03-22	Daniel Önnerby

*****************************************************/
function message( oXML,oUser ){
	for(var i=0;i<oXML.firstChild.childNodes.length;i++){
		if(oXML.firstChild.childNodes[i].nodeName=="chat"){
			addLog(oXML.firstChild.childNodes[i].firstChild);

			var iMessageId = _root.aMessages.length;
			_root.aMessages[iMessageId] = "<font color=\"#000000\"><b>"+oUser.oServerPreferences+":</b></font>&nbsp;&nbsp;<font color=\"#0000ff\">"+oXML.firstChild.childNodes[i].firstChild+"</font>";
		}
	}
	renderTexts();
}


/*****************************************************
function updateUserList

	Description:
		Called when the userlist is updated

	Author:
		2005-03-22	Daniel Önnerby

*****************************************************/
function updateUserList(){
	var sList:String	= "";
	for(var i=0;i<this.aUsers.length;i++){
		sList	+= this.aUsers[i].oServerPreferences+"\r";
	}
	_root["userlist"].text=sList;
}


/*****************************************************
function renderTexts

	Description:


	Author:
		2005-03-22	Daniel Önnerby

*****************************************************/
function renderTexts( ){
	var sText="";
	for(var i=_root.aMessages.length-1;i>=0;i--){
		sText+=_root.aMessages[i]+"<br>";
	}
	_root["chat_text"].htmlText=sText;
}



_root["inputtext"].onChanged=function (){
	var sMessage=_root["inputtext"].text;
	if( sMessage.substr(sMessage.length-2)=="\r\r" ){
		_root["inputtext"].text="";
		sMessage=sMessage.substring(0,sMessage.length-2);
		var oXML		= new XML("<message></message>");
		var oMessage	= oXML.firstChild;
		var oChat		= oXML.createElement("chat");
		var oChatText	= oXML.createTextNode(sMessage);

		oChat.appendChild(oChatText);
		oMessage.appendChild(oChat);

		_root.oRoom.sendMessage(oXML);

	}
}