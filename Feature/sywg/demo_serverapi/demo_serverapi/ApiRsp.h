#ifndef __API_RSP
#define __API_RSP
#include "..\\..\public\\serverapi.h"

class CApiRsp :public CServerApiSpi
{
public:
    CApiRsp();
    ~CApiRsp();

    //½áËã×´Ì¬Í¨Öª
    virtual void OnNtyStatus(NTY_STATUS_INFO *pStatus);
    virtual void OnRcvRspData(CPackage *pPackage,int &nEventHandle);
    virtual void OnRcvFile(char *FileName);
    virtual void OnRcvReq(int &ReqNo,CPackage *pPara,USER_HANDLE &handle);
};
#endif
