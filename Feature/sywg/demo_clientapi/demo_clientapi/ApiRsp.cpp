#include "stdafx.h"
#include "ApiRsp.h"
#include "..\\..\\public\\Package.h"
#include "demo_clientapi.h"
#include "demo_clientapiDlg.h"
#include "..\\..\\public\\clientapi.h"
#include "requestid.h"
extern CClientApi *pApi ;
extern Cdemo_clientapiApp theApp;
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
	if(pHeader->nFuncType == RSPTYPE )//如果是应答包
	{
		switch(pHeader->nFuncNo)
		{
		case EVENT_RSP://发送文件应答
			{
				ERROR_INFO errInfo;
				int nPre;
				bool bRet = pPackage->GetFieldValue(0,"rspid",nPre,errInfo);
				int nType;
				bRet = pPackage->GetFieldValue(0,"msgtype",nType,errInfo);
				if(nType == MSG_PROGRESS)
				{
					if(nPre == 9999)
					{
						string str;
						bRet = pPackage->GetFieldValue(0,"rspinfo",str,errInfo);
						char buf1[1024];
						strcpy(buf1,str.c_str());
						((Cdemo_clientapiDlg *)(theApp.m_pMainWnd))->InsertInfo(buf1);
					}
					else
					{
						theApp.m_pMainWnd->PostMessage(WM_ADDINFO,(WPARAM)nPre,(LPARAM)0);
					}
				}
				else if(nType == EVENT_RSP)
				{//取消文件传输
					string str;
					bRet = pPackage->GetFieldValue(0,"rspinfo",str,errInfo);
					char buf1[1024];
					strcpy(buf1,str.c_str());
					((Cdemo_clientapiDlg *)(theApp.m_pMainWnd))->InsertInfo(buf1);
					//((Cdemo_clientapiDlg *)(theApp.m_pMainWnd))->InsertInfo("取消文件传输成功");
					((Cdemo_clientapiDlg *)(theApp.m_pMainWnd))->m_Progress.SetPos(0);

				}
			}
			break;
		case -3000://登陆应答
			{
				string msg;
				ERROR_INFO errInfo;
				int nReturnCode ;
				bool bRet = pPackage->GetFieldValue(0,"msgid",nReturnCode,errInfo);
				//bRet = pPackage->GetFieldValue(0,"msginfo",msg,errInfo);
				if(bRet == true)
				{
					if(nReturnCode == 0)
					{
						((Cdemo_clientapiDlg *)(theApp.m_pMainWnd))->InsertInfo("登陆成功!!!");
					}
					else 
					{
						CString s ;
						s.Format("登陆失败:%s!!!",msg.c_str());
						((Cdemo_clientapiDlg *)(theApp.m_pMainWnd))->InsertInfo(s.GetBuffer(0));
					}
				}
			}
			break;
		case START_SETTLEMENT:
			{
				string msg;
				ERROR_INFO errInfo;
				int nReturnCode ;
				bool bRet = pPackage->GetFieldValue(0,"msgid",nReturnCode,errInfo);
				bRet = pPackage->GetFieldValue(0,"msginfo",msg,errInfo);
				
				CString s ;
				s.Format("结算状态:%s",msg.c_str());
				if(s.GetLength() > 30) 
					s = s.Right(s.GetLength()-23);
				((Cdemo_clientapiDlg *)(theApp.m_pMainWnd))->InsertInfo(s.GetBuffer(0));

			}
			break;
		case QRY_MATCH://查成交
		case QRY_POSI://查持仓
		case QRY_FUND://查资金
			{
				CString strField;
				switch(pHeader->nFuncNo)
				{
					case QRY_MATCH://查成交
						strField.Format("收到成交查询返回记录(%d条记录)",pHeader->nTotalRecordNum);
						((Cdemo_clientapiDlg *)(theApp.m_pMainWnd))->SetQryTitle("成交查询结果");
						break;
					case QRY_POSI://查持仓
						strField.Format("收到持仓查询返回记录(%d条记录)",pHeader->nTotalRecordNum);
						((Cdemo_clientapiDlg *)(theApp.m_pMainWnd))->SetQryTitle("持仓查询结果");
						break;
					case QRY_FUND://查资金
						strField.Format("收到资金查询返回记录(%d条记录)",pHeader->nTotalRecordNum);
						((Cdemo_clientapiDlg *)(theApp.m_pMainWnd))->SetQryTitle("资金查询结果");
						break;
					default:break;
				}

				((Cdemo_clientapiDlg *)(theApp.m_pMainWnd))->InsertInfo(strField.GetBuffer(0));
				string msg;
				ERROR_INFO errInfo;
				bool bRet = false;
				vector<FIELDINFO> * pFieldInfo = pPackage->GetFieldInfo();
				if(pHeader->nPackageIndex == 0)
				{
					((Cdemo_clientapiDlg *)(theApp.m_pMainWnd))->SetListHeader(pFieldInfo);
				}
				string strvalue;
				int    intvalue;
				double doublevalue;
				for(int i = 0;i<pPackage->GetRecordCount();i++)
				{
					for(int j = 0;j<pPackage->GetFieldCount();j++)
					{
						FIELDINFO* pField = GetField(pFieldInfo,j);
						switch(pField->FieldDefine.FieldType)
						{
						case STRING_TYPE:
							bRet = pPackage->GetFieldValue(i,pField->FieldDefine.FieldName,strvalue,errInfo);
							strField = strvalue.c_str();
							break;
						case INT_TYPE :
							bRet = pPackage->GetFieldValue(i,pField->FieldDefine.FieldName,intvalue,errInfo);
							strField.Format("%d",intvalue);
							break;
						case DOUBLE_TYPE :
							bRet = pPackage->GetFieldValue(i,pField->FieldDefine.FieldName,doublevalue,errInfo);
							strField.Format("%.2f",doublevalue);
							break;
						default:break;
						}
						((Cdemo_clientapiDlg *)(theApp.m_pMainWnd))->AddFieldValue(j,strField.GetBuffer(0));
					}
				}
			}
			break;

		default:break;
		}
	}
}
void CApiRsp::OnRcvFile(char *FileName)
{
	CString str;
	str.Format("收到文件%s",FileName);
	((Cdemo_clientapiDlg *)(theApp.m_pMainWnd))->InsertInfo(str.GetBuffer(0));
}
/*void CApiRsp::OnRcvReq(int &ReqNo,CPackage *pPara,CHANEL_HANDLE &handle)
{
	PACKAGEHEADER *pRcvHead = pPara->GetPackageHeader();
	
	PACKAGEHEADER head;
	memset(&head,0,sizeof(PACKAGEHEADER));
	head.nFuncType = RSPTYPE;
	head.nFuncNo = 100;
	head.nPackageIndex = 0;
	head.nTotalPackageNum = 1;
	head.nTotalRecordNum = 1;
	head.nEventHandle = pRcvHead->nEventHandle;


	ERROR_INFO ErrMsg;
	CPackage *pPackage = pApi->CreatePackage();
	pPackage->SetPackageHeader(&head);
	int iRec = pPackage->AppendBlankRecord(ErrMsg);
	pPackage->SetFieldValue(iRec,"test","测试",ErrMsg);
	pApi->SendDataToClient(handle.buf,pPackage,ErrMsg);
	pApi->ReleasePackage(pPackage);
}*/


FIELDINFO * CApiRsp::GetField(vector<FIELDINFO> * pFieldInfo,int nGetIndex)
{
	int nIndex = 0;
	vector <FIELDINFO>::iterator iter;
	for(iter = pFieldInfo->begin(); iter != pFieldInfo->end();iter++)
	{
		FIELDINFO *pItem = &(*iter);
		if(nIndex == nGetIndex) return pItem;
		nIndex++;
	}
	return NULL;
}
