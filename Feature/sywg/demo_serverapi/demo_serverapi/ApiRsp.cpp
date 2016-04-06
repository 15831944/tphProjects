#include "stdafx.h"
#include "ApiRsp.h"
#include "..\\..\public\\Package.h"
#include "demo_serverapi.h"
#include "demo_serverapiDlg.h"
#include "..\\..\public\\serverapi.h"
#include "requestid.h"
#include "dbf.h"
extern CServerApi *pApi ;

extern Cdemo_serverapiApp theApp;
CApiRsp::CApiRsp()
{
}
CApiRsp::~CApiRsp()
{
}
//结算状态通知
void CApiRsp::OnNtyStatus(NTY_STATUS_INFO *pStatus)
{
}
void CApiRsp::OnRcvRspData(CPackage *pPackage,int &nEventHandle)
{
	PACKAGEHEADER *pHeader = pPackage->GetPackageHeader();
	if(pHeader->nFuncType == EVENT_RSP)//如果是应答包
	{
		string msg;
		ERROR_INFO errInfo;
		bool bRet = pPackage->GetFieldValue(0,"rspinfo",msg,errInfo);
		
		//((Cdemo_serverapiDlg *)theApp.m_pMainWnd)->m_Button.SetWindowText(msg.c_str());
		//((Cdemo_serverapiDlg *)theApp.m_pMainWnd)->m_Button.RedrawWindow();
		//AfxMessageBox(msg.c_str());

	}
}
void CApiRsp::OnRcvFile(char *FileName)
{
	CString str;
	str.Format("收到文件%s",FileName);
	((Cdemo_serverapiDlg *)(theApp.m_pMainWnd))->InsertInfo(str.GetBuffer(0));
}
void CApiRsp::OnRcvReq(int &ReqNo,CPackage *pPara,USER_HANDLE &handle)
{
	switch(ReqNo)
	{
	case 100:
		{
			PACKAGEHEADER *pRcvHead = pPara->GetPackageHeader();

			PACKAGEHEADER head;
			memset(&head,0,sizeof(PACKAGEHEADER));
			head.nFuncType = RSPTYPE;
			head.nFuncNo = 100;
			head.nEventHandle = pRcvHead->nEventHandle;
			head.nPackageIndex = 0;
			head.nTotalPackageNum = 1;
			head.nTotalRecordNum = 1;

			ERROR_INFO ErrMsg;
			CPackage *pPackage = pApi->CreatePackage();
			pPackage->SetPackageHeader(&head);
			int iRec = pPackage->AppendBlankRecord(ErrMsg);
			pPackage->SetFieldValue(iRec,"test","测试",ErrMsg);
			pApi->SendDataToClient(&handle,pPackage,ErrMsg);
			pApi->ReleasePackage(pPackage);
		}
		break;
	case 6:
		{
			PACKAGEHEADER *pRcvHead = pPara->GetPackageHeader();

			PACKAGEHEADER head;
			memset(&head,0,sizeof(PACKAGEHEADER));
			head.nFuncType = RSPTYPE;
			head.nFuncNo = 100;
			head.nEventHandle = pRcvHead->nEventHandle;
			head.nPackageIndex = 0;
			head.nTotalPackageNum = 1;
			head.nTotalRecordNum = 1;

			ERROR_INFO ErrMsg;
			pApi->SendFileToClient("hello.txt",&handle,ErrMsg);
		}
		break;
	case -3000://登陆
		{
			PACKAGEHEADER *pRcvHead = pPara->GetPackageHeader();
			((Cdemo_serverapiDlg *)(theApp.m_pMainWnd))->InsertInfo("处理登陆请求");
			PACKAGEHEADER head;
			memset(&head,0,sizeof(PACKAGEHEADER));
			head.nFuncType = RSPTYPE;
			head.nFuncNo = ReqNo;
			head.nEventHandle = pRcvHead->nEventHandle;
			head.nPackageIndex = 0;
			head.nTotalPackageNum = 1;
			head.nTotalRecordNum = 1;
			ERROR_INFO ErrMsg;
			CPackage *pPackage = pApi->CreatePackage();
			pPackage->SetPackageHeader(&head);
			int iRec = pPackage->AppendBlankRecord(ErrMsg);
			string user;
			string password;
			pPara->GetFieldValue(0,"user",user,ErrMsg);
			pPara->GetFieldValue(0,"password",password,ErrMsg);
			CString struser = user.c_str();
			CString strpassword = password.c_str();
			if(struser == "test" && strpassword == "88888888")
			{
				pPackage->SetFieldValue(iRec,"msgid",0,ErrMsg);
				pPackage->SetFieldValue(iRec,"msginfo","登陆成功",ErrMsg);
			}
			else if(struser != "test")
			{
				pPackage->SetFieldValue(iRec,"msgid",-1,ErrMsg);
				pPackage->SetFieldValue(iRec,"msginfo","用户名不存在",ErrMsg);
			}
			else
			{
				pPackage->SetFieldValue(iRec,"msgid",-1,ErrMsg);
				pPackage->SetFieldValue(iRec,"msginfo","密码错误",ErrMsg);
			}
			pApi->SendDataToClient(&handle,pPackage,ErrMsg);
			pApi->ReleasePackage(pPackage);
		}
		break;
		case QRY_MATCH://查成交
		case QRY_POSI://查持仓
		case QRY_FUND://查资金
			{
				PACKAGEHEADER *pRcvHead = pPara->GetPackageHeader();

				PACKAGEHEADER head;
				memset(&head,0,sizeof(PACKAGEHEADER));
				head.nFuncType = RSPTYPE;
				head.nFuncNo = ReqNo;
				head.nEventHandle = pRcvHead->nEventHandle;
				head.nPackageIndex = 0;
				head.nTotalPackageNum = 1;
				head.nTotalRecordNum = 1;

				ERROR_INFO ErrMsg;
				CPackage *pPackage = pApi->CreatePackage();
				pPackage->SetPackageHeader(&head);
				int nRet;
				CDBF m_fDBF;
				if(ReqNo == QRY_MATCH)
				{
					nRet = m_fDBF.d4use("成交.dbf");
					((Cdemo_serverapiDlg *)(theApp.m_pMainWnd))->InsertInfo("处理成交查询请求");
				}
				else if(ReqNo == QRY_POSI)
				{
					nRet = m_fDBF.d4use("持仓.dbf");
					((Cdemo_serverapiDlg *)(theApp.m_pMainWnd))->InsertInfo("处理持仓查询请求");
				}
				else if(ReqNo == QRY_FUND)
				{
					nRet = m_fDBF.d4use("资金.dbf");
					((Cdemo_serverapiDlg *)(theApp.m_pMainWnd))->InsertInfo("处理资金查询请求");
				}

				if(DBF_OK != nRet)
				{
					int iRec = pPackage->AppendBlankRecord(ErrMsg);
					pPackage->SetFieldValue(iRec,"msgid",-1,ErrMsg);
					pPackage->SetFieldValue(iRec,"msginfo","文件打开错误",ErrMsg);
					pApi->SendDataToClient(&handle,pPackage,ErrMsg);
					pApi->ReleasePackage(pPackage);
					break;
				}
				head.nTotalRecordNum = m_fDBF.d4reccount();
				pPackage->SetPackageHeader(&head);
				m_fDBF.d4top();
				for(int i = 0;i<m_fDBF.d4reccount();i++)
				{
					int iRec = pPackage->AppendBlankRecord(ErrMsg);
					for(int j = 0;j<m_fDBF.f4getfieldcount();j++)
					{
						TField * tfield = m_fDBF.f4field(j);
						switch(tfield->f_type)
						{
						case 'B'://double
							{
								double value = m_fDBF.f4double(tfield->name);
								pPackage->SetFieldValue(iRec,tfield->name,value,ErrMsg);
							}
							break;
						case 'I'://int
							{
								int value = m_fDBF.f4int(tfield->name);
								pPackage->SetFieldValue(iRec,tfield->name,value,ErrMsg);
							}
							break;
						case 'C'://字符
							{
								char *pret = m_fDBF.f4str(tfield->name);
								CString s = pret;
								s.TrimLeft();
								s.TrimRight();
								pPackage->SetFieldValue(iRec,tfield->name,s.GetBuffer(0),ErrMsg);
							}
							break;
						default:
							{
								char *pret = m_fDBF.f4str(tfield->name);
								pPackage->SetFieldValue(iRec,tfield->name,pret,ErrMsg);
							}
							break;
						}
					}
					m_fDBF.d4next();
				}
				m_fDBF.d4close();
				pApi->SendDataToClient(&handle,pPackage,ErrMsg);
				pApi->ReleasePackage(pPackage);
			}
			break;
	default:break;
	}

}