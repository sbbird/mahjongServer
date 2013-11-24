class XMLSocketd.Room{

	private var oConnection		= false;

	var sIdentifier:String		= "";
	var onConnect				= false;
	var onJoin					= false;
	var onError					= false;
	var onMessage				= false;
	var onUserChange			= false;
	var sState:String			= "disconnected";

	var aUsers					= false;

	var iMyUserId:Number		= 0;
	var oAuthorizeXML;


	function Room( sIdentifier:String,oAuthorizeXML,oConnection ){
		this.oAuthorizeXML	= oAuthorizeXML;
		this.aUsers			= new Array();
		this.sIdentifier	= sIdentifier;

		if(oConnection){
			this.oConnection	= oConnection;
		}else{
			this.oConnection				= new XMLSocketd.Connection();
			this.oConnection.currentRoom	= this;
			this.oConnection.onConnect = function (bSuccess){
				if(this.currentRoom.onConnect){
					this.currentRoom.onConnect(bSuccess);
				}
				if(bSuccess){
					this.currentRoom.join(this.currentRoom.oAuthorizeXML);
				}
			}
			this.oConnection.onError = function(sError){
				this.currentRoom.callError(sError);
			}
		}

		this.oConnection.addRoom(this);
	}

	function connect(){
		this.oConnection.connect();
	}

	function join(oAuthorizeXML){
		this.sState	= "joining";

/*
		var oXML						= new XML();
		var oNode						= oXML.createElement("room:join");
		oNode.attributes["room_id"]		= this.sIdentifier;
		oNode.attributes["room_url"]	= this.oConnection.sURL;
		oNode.appendChild(oAuthorizeXML);
		oXML.appendChild(oNode);
*/
		var oXML	= new XML("<room:join room_id=\""+this.sIdentifier+"\" room_url=\""+this.oConnection.sURL+"\">"+oAuthorizeXML+"</room:join>");

		if( !this.oConnection.sendMessage(oXML) ){
			this.callError("Can not send join-message.");
		}

	}

	function sendMessage(oXML){
		if(oXML.firstChild!=null){
			if(oXML.firstChild.nodeName=="message"){
				oXML.firstChild.attributes["room_id"]	= sIdentifier;
				oXML.firstChild.attributes["room_url"]	= this.oConnection.sURL;

				this.oConnection.sendMessage(oXML);

			}else{
				this.callError("Root-node in message must be \"message\"");
				return false;
			}
		}else{
			this.callError("Empty XML");
			return false;
		}
	}

	private function callError(sError){
		trace(sError);
		if(this.onError){
			this.onError(sError);
		}
	}

	function incommingMessage(oXML){
		var oFirst		= oXML.firstChild;
		switch(oFirst.nodeName){
			case "room:authorized":
				this.sState		= "authorized";
				this.iMyUserId	= oFirst.attributes["user_id"];
				if(this.onJoin){
					this.onJoin( true );
				}
				break;
			case "room:unauthorized":
				this.sState="unauthorized";
				if(this.onJoin){
					this.onJoin( false );
				}
				break;
			case "message":
				if(this.onMessage){
					this.onMessage(oXML,this.getUser(oFirst.attributes["user_id"]));
				}
				break;
			case "userlist":
				this.updateUserlist(oFirst);
				if(this.onUserChange){
					this.onUserChange();
				}
				break;
		}
	}

	function getUser(iId){
		for(var i=0;i<this.aUsers.length;i++){
			if(iId==this.aUsers[i].iId){
				return this.aUsers[i];
			}
		}
		return false;
	}


	function updateUserlist(oXML){
		for(var i=0;i<oXML.childNodes.length;i++){
			var iUserId:Number	= int(oXML.childNodes[i].attributes["id"]);

			if(oXML.childNodes[i].nodeName=="add"){
				var iNewId=this.aUsers.length;
				this.aUsers[iNewId]=new XMLSocketd.User(oXML.childNodes[i],this);
			}

			if(oXML.childNodes[i].nodeName=="del"){
				for(var j=0;j<this.aUsers.length;j++){
					if(this.aUsers[j].iId==iUserId){
						this.aUsers.splice(j,1);
					}
				}
			}

			if(oXML.childNodes[i].nodeName=="change"){
				for(var j=0;j<this.aUsers.length;j++){
					if(this.aUsers[j].iId==iUserId){
						this.aUsers[j].parseXML(oXML.childNodes[i]);
					}
				}
			}

		}
	}

}
