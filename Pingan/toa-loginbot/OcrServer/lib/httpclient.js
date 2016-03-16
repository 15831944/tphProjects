/*
* Class: Http Client Wrapper
* 
*/

function HttpClient(){
// Member functions by: 
	// None	
// * Daynamic prototype
if ((typeof HttpClient._initialized === 'undefined')){
//////////////////////////////////////////////////////////////////////////////////////////////////
	HttpClient.prototype.parseUrl = function(url){
		if ( ('undefined' === typeof url) || null == url || "" == url ){
			console.error("invliad paramters: " + url);
			return null;
		}
		var options = URL.parse(url);
		if (options == null || (typeof options.protocol === 'undefined')){
			console.error("url parse failed");
			return null;
		}
		return options;
	};
	
	HttpClient.prototype.getClient = function(options){
		var client = null;
		if (options.protocol == "https:"){
			client = HTTPS;
		}else if (options.protocol == "http:"){
			client = HTTP;
		}else{
			console.error("protocol not supported");
		}
		return client;
	};
	
// Http Get request
	HttpClient.prototype.httpRequest = function (req){
		var options = this.parseUrl(req.url);
		if (options == null){
			return ;
		}
		
		var client = this.getClient(options);
		if (client == null){
			return ;
		}
		
		if ((typeof req.body) !== 'undefined'){
			options.method = "POST";
		}
		
		var request = client.request(options, (incMsg) => {
			console.log(`Got http response, status code: ${incMsg.statusCode}`);
			if (incMsg.statusCode != 200){
				console.error("Http request failed");
				return ;
			}
			
			var body = "";

			incMsg.on("data", function onData(chunk){
				//console.log("" + chunk);
				body += chunk;
			});
			incMsg.on("end", function onEnd(){
				console.info("Http response done");
				if ('undefined' !== typeof req.callback){
					req.callback(req.url, incMsg.headers, body);
				}
				return ;
			});
			incMsg.on("error", function onError(){
				console.error("Failed to get http response");
				return ;
			});
		});
		
		if ('undefined' !== typeof req.body){
			request.setHeader("content-type", "application/json; charset=utf-8");
			request.setHeader("accept", "*/*");
			request.write(JSON.stringify(req.body));
		}		
		
		request.on("error", function onError(){
			console.error("Failed to http request via GET");
			return ;
		});
		
		request.end();	
	};
//////////////////////////////////////////////////////////////////////////////////////////////////
// * Endof Dynamic prototype
	HttpClient._initialized = true;
}
}

 /* Unit test
 *
 var hc2 = new HttpClient();
 hc2.httpRequest({url: "http://192.168.56.101/handle.php", body: {'hi':'world'}, callback : function(u, h, b){console.log(b);}});
 hc2.httpRequest({url: "http://192.168.56.101/handle.php", callback : function(u, h, b){console.log(b);}});
 hc2.httpRequest({url: "http://192.168.56.101/handle.php"});
 */

