const OS = require('os');
const ASSERT = require('assert');
const FS = require('fs');
const CHILD_PROCESS = require('child_process');
const HTTPS = require('https');
const HTTP = require("http");
const URL = require("url");
const PATH = require("path");

// Initialize
(function(){
	/**
	* Process initialize
	*/
	process.title = "OcrServer";
	process.chdir(PATH.dirname(module.filename));
	console.log(`Current directory: [${process.cwd()}]`);
	console.log(`Server pid: ${process.pid}`);
	console.log(`startup timestamp: ${new Date()}`);
})();

// Load libraries
eval(FS.readFileSync("./lib/httpserver.js").toString());
eval(FS.readFileSync("./config/config.js").toString());

// Startup
(function(){
	/**
	* Startup http server
	*/
	(new HttpServer(OcrConfig.port)).start(onRequest);
})();

// Handlers dispatch
function onRequest(req, resp) {
	var reqBody = "";
	req.on('data', function (chunk){
		//console.log("" + chunk);
		reqBody += chunk;
	});
	req.on('end', function(){
		//console.log(reqBody);
		return onProcess(req, reqBody, resp);
	});
}		

// Dispatch 
function onProcess(req, body, resp){
	var context = URL.parse(req.url).pathname;
	console.log(`Request [${context}]`);
	//console.log(`Request [${body}]`);
	
	// For client request
	if (context == OcrConfig.ocrRequest){
		handleOcrRequest(req, JSON.parse(body), resp);
	}
};

function handleOcrRequest(req, reqobj, resp){
	var arglist = [];
	if ('undefined' !== typeof reqobj.org
		&& reqobj.org != null){
		arglist.push("-b");
		arglist.push(reqobj.org);
	}
	
	if ('undefined' !== typeof reqobj.imagedata
		&& reqobj.imagedata != null){

		if ('undefined' === typeof reqobj.ext 
			|| reqobj.ext == null
			|| reqobj.ext == ""){
			reqobj.ext = ".jpeg";
		}
		
		var tmpimagefile = OS.tmpdir()
						   + "\\OcrBot-"
						   + OcrConfig.getCurrTime() 
						   + "-"
						   + Math.random() 
						   + reqobj.ext;
						   
		FS.writeFileSync(tmpimagefile, reqobj.imagedata, {encoding: 'base64'});
		arglist.push(tmpimagefile);
	}
	
	console.log(JSON.stringify(arglist));

	const bot = CHILD_PROCESS.spawn(OcrConfig.ocrbot
									, arglist
									, OcrConfig.forkOptions);
	
	console.log(`OcrBot Created OK, pid=${bot.pid}`);
	
	var respobj = {
		text : "" // default empty string
	};

	if (typeof bot.stdout !== 'undefined' 
		&& bot.stdout != null){
		bot.stdout.on('data', (data) => {
								console.log(`OcrBot stdout: ${data}`);
								respobj.text = data.toString();
							});
	}

	bot.on('close', (code) => {
		console.log(`child process exited with code ${code}`);
		// do response
		resp.writeHead(200, { 
								'Content-Type': 'Application/Json', 
								"Accept" : "text/plain"
							});
		resp.write(JSON.stringify({result: respobj}));
		resp.end();
	});
	
	return ;
}

