#include <windows.h>
#include <string>
#include <vector>
#include <stdio.h>
#include <sys/stat.h>
#include "CrackCaptchaAPI.h"
#include "XMLWrap.h"
#include "Tools.h"

#include <log4cplus/logger.h>
#include <log4cplus/configurator.h>
#include <log4cplus/loggingmacros.h>
#include <log4cplus/tstring.h>
#include <tchar.h>

using namespace log4cplus;

static std::string SoftwareName = "";
static std::string key = "";
static std::string username = "";
static std::string password = "";

int ReadDama2Config()
{
	char szCurrDir [MAX_PATH] =  "";
	GetModuleFileNameA(NULL, szCurrDir, sizeof(szCurrDir));
	strrchr(szCurrDir, '\\')[1] = '\0';
	std::string strPath(szCurrDir);
	strPath += "\\Dama2Account.xml";

	CXMLWrap xmlWrap;
	if(!xmlWrap.ParseXmlFile(strPath.c_str()))
		return -1;

	TiXmlElement * pParentElem(NULL);
	pParentElem = xmlWrap.GetRootElement();
	if(pParentElem == NULL)
		return -2;

	SoftwareName = xmlWrap.GetChildElementValue(pParentElem, "SoftwareName");
	key = xmlWrap.GetChildElementValue(pParentElem, "key");
	username = xmlWrap.GetChildElementValue(pParentElem, "username");
	password = xmlWrap.GetChildElementValue(pParentElem, "password");
	return 0;
}

#define MyLOG4CPLUS_INFO(logger, logEvent, logMutex)						\
	WaitForSingleObject(logMutex, INFINITE);								\
	LOG4CPLUS_INFO(logger, logEvent);										\
	ReleaseMutex(logMutex);

#define MyLOG4CPLUS_ERROR(logger, logEvent, logMutex)						\
	WaitForSingleObject(logMutex, INFINITE);								\
	LOG4CPLUS_ERROR(logger, logEvent);										\
	ReleaseMutex(logMutex);

bool GetVCodeFromNetWork(const std::string & imagePath, std::string & sText, HANDLE& logMutex)
{
	//Change dir to module path
	char szModuleDir [MAX_PATH] =  "";
	GetModuleFileNameA(NULL, szModuleDir, sizeof(szModuleDir));
	strrchr(szModuleDir, '\\')[1] = '\0';
	SetCurrentDirectoryA(szModuleDir);


	PropertyConfigurator::doConfigure(LOG4CPLUS_TEXT("log.properties"));
	Logger logger = Logger::getRoot();

	if(ReadDama2Config())
		return false;
	//Init
	if(strlen(key.c_str()) < 2)
	{
		MyLOG4CPLUS_ERROR(logger, "Key is invalid", logMutex);
		return false;
	}
	int nRet = ::Init(SoftwareName.c_str(), key.c_str());
	if (nRet == ERR_CC_SUCCESS)
	{
		MyLOG4CPLUS_INFO(logger, "Init success", logMutex);
	}
	else
	{
		LOG4CPLUS_ERROR(logger, "Init failed: "<<nRet<<"(origError="<<::GetOrigError()<<")", logMutex);
		return false;
	}

	//Login
	char szSysAnnURL[4096], szAppAnnURL[4096];
	CHAR * sDyncVCode = NULL;

	int retryCount = 3;
ReLogin:
	nRet = ::Login(username.c_str(), password.c_str(), sDyncVCode, szSysAnnURL, szAppAnnURL);
	if (nRet == ERR_CC_SUCCESS)
	{
		MyLOG4CPLUS_INFO(logger, "Login success", logMutex);
	}
	else if (nRet == ERR_CC_NEED_DYNC_VCODE && retryCount > 0)
	{
		goto ReLogin;
		retryCount--;
	}
	else
	{
		MyLOG4CPLUS_ERROR(logger,"Login failed: "<<nRet<<"(origError="<<::GetOrigError()<<")", logMutex);
		return false;
	}

	//Change dir to temp path
	char szTempPath [MAX_PATH] = "";
	GetTempPathA(sizeof(szTempPath), szTempPath);
	SetCurrentDirectoryA(szTempPath);
	//Decode buf
	ULONG m_ulRequestID;
	FILE* f = fopen(imagePath.c_str(), "rb");
	if (f == NULL)
	{
		MyLOG4CPLUS_ERROR(logger,"Open file failed("<<imagePath.c_str()<<")", logMutex);
		return false;
	}
	fseek( f, 0, SEEK_END );
	ULONG ulLen = ftell(f);
	fseek( f, 0, SEEK_SET );
	if (ulLen == 0)
	{
		MyLOG4CPLUS_ERROR(logger,"File is empty("<<imagePath.c_str()<<")", logMutex);
		return false;
	}
	char *pData = new char[ulLen];
	memset(pData, 0 , ulLen*sizeof(char));
	fread(pData, ulLen, 1, f);
	fclose(f);

	//Change dir to module path
	SetCurrentDirectoryA(szModuleDir);
	int retryCountGetResult = 3;
ReGetResult:
	nRet = ::DecodeBuf(pData, ulLen, "PNG", 
		6,		//length
		30,	//timeout, s
		54,	//type id
		&m_ulRequestID);
	if (nRet == ERR_CC_SUCCESS)
	{
		MyLOG4CPLUS_INFO(logger, "DecodeBuf success(RequestID="<<m_ulRequestID<<")", logMutex);
	}
	else
	{
		MyLOG4CPLUS_ERROR(logger,"DecodeBuf failed: "<<nRet<<"(origError="<<::GetOrigError()<<")", logMutex);
		return false;
	}

	//Get result
	ULONG m_ulVCodeID;
	char szVCode[100] = { 0 };
	char szRetCookie[4096];
	nRet = ::GetResult(m_ulRequestID, 15 * 1000, 
		szVCode, sizeof(szVCode), &m_ulVCodeID, szRetCookie, sizeof(szRetCookie));
	if (nRet == ERR_CC_SUCCESS)
	{
		MyLOG4CPLUS_INFO(logger, "GetResult(RequestID="<<m_ulRequestID
			<<") success(Code="<<ToUnicodeString(szVCode).c_str()<<", id="<<m_ulVCodeID<<")", logMutex);
	}
	else
	{
		MyLOG4CPLUS_ERROR(logger,"GetResult(RequestID="<<m_ulRequestID<<") failed: "<<nRet<<"(origError="<<::GetOrigError()<<")", logMutex);
	}

	MyLOG4CPLUS_INFO(logger, "Dama2 result: "<<ToUnicodeString(szVCode).c_str()<<"", logMutex);
	if ((!strcmp(szVCode, "") || !strcmp(szVCode, "IERROR") || !strcmp(szVCode, "ERROR")) && retryCountGetResult > 0)
	{
		retryCountGetResult--;
		goto ReGetResult;
	}

	//Change dir to temp path
	SetCurrentDirectoryA(szTempPath);

	sText = szVCode;
	return !sText.empty();
}

