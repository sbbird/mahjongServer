<?
$sPOST=urldecode($HTTP_RAW_POST_DATA);

$oFile = @fopen("last.log","a");
@fwrite($oFile,$sPOST."\n\n");
@fclose($oFile);

$sName="test";
if(preg_match("/user_ip=\"([^\"]+)\"/i",$sPOST,$aMatch)){
	$sName=gethostbyaddr($aMatch[1]);
}


?><room:authorized><?echo $sName?></room:authorized>


