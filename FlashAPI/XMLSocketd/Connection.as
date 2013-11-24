class XMLSocketd.Connection{

	private var oSocket:XMLSocket;
	private var bConnected:Boolean	= false;
	private var aRooms:Array;

	var sState:String			= "disconnected";

	var iPort:Number;
	var sURL:String;

	// Events
	var onConnect			= false;
	var onDisconnect		= false;
	var onError				= false;


	function Connection(){

		this.aRooms			= new Array();
		XMLSocket.prototype.oObject	= this;
		this.oSocket		= new XMLSocket();
		this.iPort			= 2584;

		// Base directory for SWF-file
		this.sURL			= _root._url;
		var iLastSlash		= this.sURL.lastIndexOf("/");
		if(iLastSlash!=-1){
			this.sURL		= this.sURL.substring(0,iLastSlash);
		}


	}



	function connect(){
		this.sState			= "connecting";
		this.oSocket.onConnect	= function (bSuccess){
			this.oObject.socketOnConnect(bSuccess);
		}
		this.oSocket.onXML		= function (oXML){
			this.oObject.socketOnXML(oXML);
		}

		if(_root._url.indexOf("http:")==-1){
			this.oSocket.connect("127.0.0.1",this.iPort);
		}else{
			this.oSocket.connect(null,this.iPort);
		}
	}


	function socketOnXML(oXML){
		var oFirst=oXML.firstChild;
		if( oFirst!=null && oFirst.attributes["room_url"]==this.sURL){
			var sRoomId		= oFirst.attributes["room_id"];

			for(var i=0;i<this.aRooms.length;i++){
				if( (this.aRooms[i].sIdentifier+" ")==(sRoomId+" ") ){
					this.aRooms[i].incommingMessage(oXML);
				}
			}

		}else{
			this.callError("Invalid XML "+oXML);
		}
	}


	function socketOnConnect(bSuccess){
		if(bSuccess){
			this.sState			= "connected";
		}else{
			this.sState			= "disconnected";
		}
		this.bConnected	= bSuccess;
		if(this.onConnect){
			this.onConnect(bSuccess);
		}
	}


	function addRoom(oRoom){
		var iNewId=this.aRooms.length;
		this.aRooms[iNewId]=oRoom;
	}

	function sendMessage(oXML){
		if(this.bConnected){
			this.oSocket.send(oXML);
			return true;
		}else{
			this.callError("Socket is not connected.");
			return false;
		}
	}


	private function callError(sError){
		trace("Error "+sError);
		if(this.onError){
			this.onError(sError);
		}
	}


}
