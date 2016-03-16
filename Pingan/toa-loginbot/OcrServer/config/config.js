/**
* Configuration for login dog
* 
*/
var OcrConfig = {
	/**
	* Settings
	*
	*/
    protocol : "http:",
    
    address : "127.0.0.1",
    
    port : 3338,
    
    ocrRequest : "/ocrrequest",
    
    ocrbot : './bin/SundayOcrWrapper.exe',
    
    /**
    * Utilities 
    *
    */
    forkOptions: {
        detached : true,
        stdio: ['ignore', 'pipe', 'ignore']
    },
    
    getCurrTime : function(){
        return Math.round(new Date().getTime()/1000.0) ;
    },
    
    dummy : ""
};
