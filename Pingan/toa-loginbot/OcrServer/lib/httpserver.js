/*
* Http Server wrapper
*
*/

function HttpServer(port){
	this._Port = port;	
if (typeof HttpServer._initialized === 'undefined'){
////////////////////////////////////////////////////////////////////////////
	HttpServer.prototype.start = function(onRequest){
		var server = HTTP.createServer(onRequest);		
		server.listen(this._Port);
		console.info("Server is listening on: " + this._Port);
	};
////////////////////////////////////////////////////////////////////////////	
	HttpServer._initialized = true;
}
}
