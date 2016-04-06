#ifndef __API_RSP
#define __API_RSP
#include "..\\..\\public\\clientapi.h"


class CApiRsp :public CClientApiSpi
{
public:
	CApiRsp();
	~CApiRsp();

	//½áËã×´Ì¬Í¨Öª
	virtual void OnNtyStatus(NTY_STATUS_INFO *pStatus);
	virtual void OnRcvRspData(CPackage *pPackage,int &nEventHandle);
	virtual void OnRcvFile(char *FileName);
	//virtual void OnRcvReq(int &ReqNo,CPackage *pPara,CHANEL_HANDLE &handle);

private:
	FIELDINFO * GetField(vector<FIELDINFO> * pFieldInfo,int nGetIndex);
};
#endif