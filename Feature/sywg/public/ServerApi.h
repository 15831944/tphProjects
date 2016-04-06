//---------------------------------------------------------------------
// ģ������:    �������ӷ����api
// �ļ�����:    Serverapi.h
// ������:      qinjian
// ��������:    20140724
// �޸���ʷ:
// ��    ע:    
//----------------------------------------------------------------------
#ifndef SERVERAPI_H
#define SERVERAPI_H
#ifdef SERVERAPI_EXPORTS
#define SERVERAPI_LIB __declspec(dllexport)
#else
#ifdef WIN32
#define SERVERAPI_LIB __declspec(dllexport)
#else
#define SERVERAPI_LIB
#endif
#endif

#include "BusStruct.h"

class CPackage;

//�ص��ӿ�
class CServerApiSpi
{
public:
	//����״̬֪ͨ
	virtual void OnNtyStatus(NTY_STATUS_INFO *pStatus){};
	virtual void OnRcvRspData(CPackage *pPackage,int &nEventHandle,USER_HANDLE &handle){};
	virtual void OnRcvFile(char *FileName){};
	virtual void OnRcvReq(int &ReqNo,CPackage *pPara,USER_HANDLE &handle){};
};


class SERVERAPI_LIB CServerApi
{
public:

//---------------------------------------------------------------------------
	//����һ�����ݰ�
	virtual CPackage * CreatePackage() = 0;
	//ɾ�����ݰ�
	virtual void ReleasePackage(CPackage * pPackage) = 0;
	//����apiʵ��
	static CServerApi *CreateServerApi();
	//ע��ص��ӿ�
	virtual void RegisterSpi(CServerApiSpi *pGeneralSpi) = 0;
	//�ͷ���Դ
	virtual void Release() = 0;
	//��ʼ��
	virtual int Init(ERROR_INFO &ErrMsg)=0;
	
	//���ӵ�����
	virtual bool Connect(char *routerip, int port, ERROR_INFO &ErrMsg) = 0;
	//ע��һ������ŵ�����
	virtual int AddService(int seriveno,ERROR_INFO &ErrMsg)= 0;

	//�����ļ���ָ�������߽ڵ�
	virtual int SendFileToService(char *filename,int seriveno,ERROR_INFO &ErrMsg)=0;
	//�����ļ����ͻ�
	virtual int SendFileToClient(char *filename,USER_HANDLE *UserHandle,ERROR_INFO &ErrMsg)=0;

	//�������ݰ���ָ�������߽ڵ�
	virtual int SendDataToService(int nSeriveNo,CPackage *pData,ERROR_INFO &ErrMsg)=0;
	//�����������߽ڵ�
	virtual int SendRequestToService(int nSeriveNo,int nReqNo,CPackage *Para,ERROR_INFO &ErrMsg)=0;
	//�������ݰ����ͻ�
	virtual int SendDataToClient(USER_HANDLE *UserHandle,CPackage *pData,ERROR_INFO &ErrMsg)=0;

public:
	virtual ~CServerApi();

};

#endif