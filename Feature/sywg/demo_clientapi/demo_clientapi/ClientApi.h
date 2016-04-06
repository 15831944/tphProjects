//---------------------------------------------------------------------
// 模块名称:    总线连接服务端api
// 文件名称:    Serverapi.h
// 创建人:      qinjian
// 创建日期:    20140724
// 修改历史:
// 备    注:    
//----------------------------------------------------------------------
#ifndef CLIENTAPI_H
#define CLIENTAPI_H
#ifdef CLIENTAPI_EXPORTS
#define CLIENTAPI_LIB __declspec(dllexport)
#else
#ifdef WIN32
#define CLIENTAPI_LIB __declspec(dllexport)
#else
#define CLIENTAPI_LIB
#endif
#endif

#include "BusStruct.h"
class CPackage;

//extern "C" CPackage* CreatePackage();
//extern "C" void ReleasePackage(CPackage * pPackage);

//回调接口
class CClientApiSpi
{
public:
	//状态通知
	virtual void OnNtyStatus(NTY_STATUS_INFO *pStatus){};
	virtual void OnRcvRspData(CPackage *pPackage,int &nEventHandle){};
	virtual void OnRcvFile(char *FileName){};
	//virtual void OnRcvReq(int &ReqNo,CPackage *pPara,CHANEL_HANDLE &handle){};
};


class CLIENTAPI_LIB CClientApi
{
public:

//---------------------------------------------------------------------------
	//创建一个数据包
	static CPackage * CreatePackage();
	//删除数据包
	static void ReleasePackage(CPackage * pPackage);
	//创建api实例
	static CClientApi *CreateClientApi();
	//注册回调接口
	virtual void RegisterSpi(CClientApiSpi *pGeneralSpi) = 0;
	//释放资源
	virtual void Release() = 0;
	//初始化
	virtual int Init(ERROR_INFO &ErrMsg)=0;
	
	//连接到总线
	virtual bool Connect(char *routerip, int port, ERROR_INFO &ErrMsg) = 0;
	//注册一个服务号到总线
	//virtual int AddService(int seriveno,ERROR_INFO &ErrMsg)= 0;

	//登陆
	virtual bool Login(char *username, char *password, ERROR_INFO &ErrMsg) = 0;
	//登出
	virtual bool Logout(ERROR_INFO &ErrMsg) = 0;

	//发送文件到指定的总线节点
	virtual int SendFileToService(char *filename,int seriveno,ERROR_INFO &ErrMsg)=0;
	//发送文件到客户
	//virtual int SendFileToClient(char *filename,char *clientID,ERROR_INFO &ErrMsg)=0;

	//发送数据包到指定的总线节点
	virtual int SendDataToService(int nSeriveNo,CPackage *pData,ERROR_INFO &ErrMsg)=0;
	//发送请求到总线节点
	virtual int SendRequestToService(int nSeriveNo,int nReqNo,CPackage *Para,ERROR_INFO &ErrMsg)=0;
	//发送数据包到客户
	//virtual int SendDataToClient(char *ClientID,CPackage *pData,ERROR_INFO &ErrMsg)=0;

public:
	virtual ~CClientApi();

};

#endif