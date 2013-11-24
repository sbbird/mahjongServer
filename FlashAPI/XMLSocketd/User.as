class XMLSocketd.User{

	var iId:Number		= 0;
	var oUserPreferences;
	var oServerPreferences;

	private var oRoom;

	function User(oXML,oUserRoom){
		this.oRoom	= oUserRoom;
		this.parseXML(oXML);
	}

	function parseXML(oXML){
		this.iId				= int(oXML.attributes["id"]);
		for(var i=0;i<oXML.childNodes.length;i++){
			if(oXML.childNodes[i].nodeName=="prefs:user"){
				this.oUserPreferences	= oXML.childNodes[i].firstChild;
			}
			if(oXML.childNodes[i].nodeName=="prefs:server"){
				this.oServerPreferences	= oXML.childNodes[i].firstChild;
			}
		}
	}

	function isMe(){
		if(this.oRoom.iMyUserId==this.iId){
			return true;
		}else{
			return false;
		}
	}

}
