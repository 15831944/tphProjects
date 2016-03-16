#include <windows.h>
#include <string>
#include <vector>
#include <stdio.h>
#include <sys/stat.h>
#include "CrackCaptchaAPI.h"
#include "XMLWrap.h"

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

bool GetVCodeFromNetWork(const std::string & imagePath, std::string & sText)
{
	if(ReadDama2Config())
		return false;
	//Init
	int nRet = ::Init(SoftwareName.c_str(), key.c_str());
	if (nRet == ERR_CC_SUCCESS)
		printf("Init success\n");
	else
		printf("Init failed: %d(origError=%d)\n", nRet, ::GetOrigError());

	//Login
	char szSysAnnURL[4096], szAppAnnURL[4096];
	CHAR * sDyncVCode = NULL;

	int retryCount = 3;
ReLogin:
	nRet = ::Login(username.c_str(), password.c_str(), sDyncVCode, szSysAnnURL, szAppAnnURL);
	if (nRet == ERR_CC_SUCCESS)
		printf("Login success\n");
	else if (nRet == ERR_CC_NEED_DYNC_VCODE && retryCount > 0)
	{
		goto ReLogin;
		retryCount--;
	}
	else
	{
		printf("Login failed: %d(origError=%d)\n", nRet, ::GetOrigError());
	}

	//Decode buf
	ULONG m_ulRequestID;
	FILE* f = fopen((LPCTSTR)(imagePath.c_str()), "rb");
	if (f == NULL)
	{
		printf("Open file failed(%s)\n", (LPCTSTR)imagePath.c_str());
		return false;
	}
	fseek( f, 0, SEEK_END );
	ULONG ulLen = ftell(f);
	fseek( f, 0, SEEK_SET );
	if (ulLen == 0)
	{
		printf("File is empty(%s)\n", (LPCTSTR)imagePath.c_str());
		return false;
	}
	char *pData = new char[ulLen];
	memset(pData, 0 , ulLen*sizeof(char));
	fread(pData, ulLen, 1, f);
	fclose(f);

	int retryCountGetResult = 3;
ReGetResult:
	nRet = ::DecodeBuf(pData, ulLen, "PNG", 
		6,		//length
		120,	//timeout, s
		54,	//type id
		&m_ulRequestID);
	if (nRet == ERR_CC_SUCCESS)
		printf("DecodeBuf success(RequestID=%ld)\n", m_ulRequestID);
	else
		printf("DecodeBuf failed: %d(origError=%d)\n", nRet, ::GetOrigError());

	//Get result
	ULONG m_ulVCodeID;
	char szVCode[100] = { 0 };
	char szRetCookie[4096];
	nRet = ::GetResult(m_ulRequestID, 10 * 1000, 
		szVCode, sizeof(szVCode), &m_ulVCodeID, szRetCookie, sizeof(szRetCookie));
	if (nRet == ERR_CC_SUCCESS)
	{
		printf("GetResult(RequestID=%ld) success(Code=%s, id=%ld)\n", 
			m_ulRequestID, szVCode, m_ulVCodeID);
	}
	else
	{
		printf("GetResult(RequestID=%ld) failed: %d(origError=%d)\n", 
			m_ulRequestID, nRet, ::GetOrigError());
	}

	printf("Dama2 result: %s\n", szVCode);
	if ((!strcmp(szVCode, "") || !strcmp(szVCode, "IERROR") || !strcmp(szVCode, "ERROR")) && retryCountGetResult > 0)
	{
		retryCountGetResult--;
		goto ReGetResult;
	}

	sText = szVCode;
	return !sText.empty();
}

