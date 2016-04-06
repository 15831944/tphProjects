//---------------------------------------------------------------------
// 模块名称:    总线连接服务端api
// 文件名称:    Serverapi.h
// 创建人:      qinjian
// 创建日期:    20140724
// 修改历史:
// 备    注:    
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

//回调接口
class CServerApiSpi
{
public:
	//结算状态通知
	virtual void OnNtyStatus(NTY_STATUS_INFO *pStatus){};
	virtual void OnRcvRspData(CPackage *pPackage,int &nEventHandle,USER_HANDLE &handle){};
	virtual void OnRcvFile(char *FileName){};
	virtual void OnRcvReq(int &ReqNo,CPackage *pPara,USER_HANDLE &handle){};
};


class SERVERAPI_LIB CServerApi
{
public:

//---------------------------------------------------------------------------
	//创建一个数据包
	virtual CPackage * CreatePackage() = 0;
	//删除数据包
	virtual void ReleasePackage(CPackage * pPackage) = 0;
	//创建api实例
	static CServerApi *CreateServerApi();
	//注册回调接口
	virtual void RegisterSpi(CServerApiSpi *pGeneralSpi) = 0;
	//释放资源
	virtual void Release() = 0;
	//初始化
	virtual int Init(ERROR_INFO &ErrMsg)=0;
	
	//连接到总线
	virtual bool Connect(char *routerip, int port, ERROR_INFO &ErrMsg) = 0;
	//注册一个服务号到总线
	virtual int AddService(int seriveno,ERROR_INFO &ErrMsg)= 0;

	//发送文件到指定的总线节点
	virtual int SendFileToService(char *filename,int seriveno,ERROR_INFO &ErrMsg)=0;
	//发送文件到客户
	virtual int SendFileToClient(char *filename,USER_HANDLE *UserHandle,ERROR_INFO &ErrMsg)=0;

	//发送数据包到指定的总线节点
	virtual int SendDataToService(int nSeriveNo,CPackage *pData,ERROR_INFO &ErrMsg)=0;
	//发送请求到总线节点
	virtual int SendRequestToService(int nSeriveNo,int nReqNo,CPackage *Para,ERROR_INFO &ErrMsg)=0;
	//发送数据包到客户
	virtual int SendDataToClient(USER_HANDLE *UserHandle,CPackage *pData,ERROR_INFO &ErrMsg)=0;

public:
	virtual ~CServerApi();

};

#endif