oTempXML=new XML("<tjo>test</tjo>");

oRoom=new XMLSocketd.Room("whiteboard",oTempXML);
oRoom.onUserChange=updateUserList;
oRoom.onMessage=message;
oRoom.connect();

/*
function connected(bSuccess){
	if(bSuccess){
		addLog("Connected successfully");
		_root.oRoom=new XMLSocketd.Room(_root.oConnection);
		_root.oRoom.onConnect=joinedRoom;
		_root.oRoom.onUserChange=updateUserList;
		_root.oRoom.onError=addLog;
		_root.oRoom.onMessage=message;
		_root.oRoom.join();
	}else{
		addLog("Connected unsuccessfully");
	}
}

function joinedRoom(bSuccess){

	if(bSuccess){
		addLog("Joined successfully");
	}else{
		addLog("Joined unsuccessfully");
	}

}
*/

function message( oXML,oUser ){
	if(!oUser.isMe()){
		for(var i=0;i<oXML.firstChild.childNodes.length;i++){
			if(oXML.firstChild.childNodes[i].nodeName=="draw"){
				var aPoints=oXML.firstChild.childNodes[i].attributes["points"].split(",");

				_root.otherBoard.lineStyle(1,0x000000,100);
				_root.otherBoard.moveTo(aPoints[0],aPoints[1]);
				for(var j=2;j<aPoints.length-1;j+=2){
					_root.otherBoard.lineTo(aPoints[j],aPoints[j+1]);
				}

			}
		}
	}

}

function addLog(sMessage){
//	trace("MSG "+sMessage);
//	_root["chat_text"].text+=sMessage+"\r";
}

function updateUserList(){
	var sList:String	= "";
	for(var i=0;i<this.aUsers.length;i++){
		sList	+= this.aUsers[i].oServerPreferences+"\r";
	}
	_root["userlist"].text=sList;

}
