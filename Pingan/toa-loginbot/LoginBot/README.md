#Configurations

###Dog
- config/config.js

###Bot

##### host
- bot/config/loginbot.json
- bot/config/statuscode.js

##### script
- bot/config/statuscode.js
- bot/config/statem.js
- bot/config/alerts.js
- bot/config/banks.js

#Host APIs

- GetVCodeText
- QueryVCode
- GetLock
- ActiveWindow
- KeyInput
- IsKeyInputDone
- ReportStatus
- QueryPhoneCode
- Exit
- SetElementFocus
- GetHttpOnlyCookie
- PutLog

#Scripting

### Web Scripting
- bot/script/json.js
- bot/script/main.js
- bot/script/login.js
- bot/script/phone.js
- bot/script/monitor.js

### Host Scripting
- bot/script/handler.js

#LoginDog
- a watch dog for login bots, used as a frontend to communicate with server and watch bots.
- this is a pure node application, which forks a http server and listen to server & bots http request and make response;


