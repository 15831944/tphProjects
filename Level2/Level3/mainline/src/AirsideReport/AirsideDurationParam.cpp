#include "StdAfx.h"
#include "airsidedurationparam.h"

using namespace ADODB;

CAirsideDurationParam::CAirsideDurationParam(void)
{
	m_startTime = 0L;
	m_endTime	= 18960000L;
	strcpy(m_modelName,"Unknown");
}

CAirsideDurationParam::~CAirsideDurationParam(void)
{

}
void CAirsideDurationParam::LoadParameter()
{

}
void CAirsideDurationParam::InitParameter(CXTPPropertyGrid* pGrid)
{
	try
	{
//		if(CreatePrameterTable() == true)
//		{
//			return;
//		}

		_variant_t RecordsAffected(0L);
		_RecordsetPtr pRecordset;
		TCHAR lpszStringSQL[128] = {0};
		_stprintf(lpszStringSQL, _T("SELECT * FROM DURATIONPARAMETER WHERE PROJID = %d"),m_nProjID);
		_bstr_t stringSQL(lpszStringSQL);

		//pRecordset.CreateInstance("ADODB.Recordset");
		pRecordset =DATABASECONNECTION.GetConnection()->Execute(stringSQL,&RecordsAffected,adCmdText);
		if (RecordsAffected.lVal == 0)
		{
			if (pRecordset)
				if (pRecordset->State == adStateOpen)
					pRecordset->Close();

			char itemSQL[1024];
			sprintf(itemSQL,"Insert INTO DURATIONPARAMETER(STARTTIME ,ENDTIME,PROJID) VALUES('%d' , '%d', %d)",m_startTime,m_endTime,m_nProjID);
			DATABASECONNECTION.GetConnection()->Execute((_bstr_t)itemSQL,&RecordsAffected,adCmdText);
			return;
		}
		if (!pRecordset->adoEOF)
		{	
			//get the parameters
			_variant_t vStartTime , vEndTime ;
			vStartTime = pRecordset->GetCollect("STARTTIME");
			vEndTime = pRecordset->GetCollect("ENDTIME");
			m_startTime = vStartTime.lVal;
			m_endTime = vEndTime.lVal;
		}
		if (pRecordset)
			if (pRecordset->State == adStateOpen)
				pRecordset->Close();
	}
	catch (_com_error e)
	{
		const TCHAR* errorMsg = e.ErrorMessage();
	}

	return;  
}

void CAirsideDurationParam::UpdateParameter()
{
	char szSQL[1024];

	sprintf(szSQL,"UPDATE DURATIONPARAMETER SET STARTTIME = '%d' ,ENDTIME = '%d' WHERE PROJID = %d",m_startTime,m_endTime,m_nProjID);

	try
	{
		_variant_t RecordsAffected(0L);

		HRESULT hr =DATABASECONNECTION.GetConnection()->Execute((_bstr_t)szSQL,&RecordsAffected,adCmdText);

		ATLASSERT(SUCCEEDED(hr));
	}
	catch (_com_error e)
	{
		const TCHAR* errorMsg = e.ErrorMessage();
	}

	return;
}

bool CAirsideDurationParam::CreatePrameterTable()
{
	//char szSQL[1024];
	//sprintf(szSQL,"select COUNT(*) from sysobjects where id = object_id(N'DURATIONPARAMETER') and OBJECTPROPERTY(id, N'IsUserTable') = 1");
	//try
	//{
	//	_variant_t RecordsAffected(0L);
	//	_RecordsetPtr pRecordset;

	//	//pRecordset.CreateInstance("ADODB.Recordset");
	//	pRecordset =REPORTDB->Execute((_bstr_t)szSQL,&RecordsAffected,adCmdText);
	//	_variant_t vIndex = (long)0;
	//	_variant_t vCount = pRecordset->GetCollect(vIndex);
	//	pRecordset->Close();
	//	if (vCount.lVal == 0)
	//	{
	//		//createTable
	//		char tableSQL[1024];
	//		strcpy(tableSQL,"CREATE TABLE DURATIONPARAMETER ( \
	//						ModelName nchar(10)  NOT NULL ,\
	//						STARTTIME numeric(18, 0) NULL ,\
	//						ENDTIME numeric(18, 0) NULL ,\
	//						[ProjID] [int] NOT NULL \
	//						)"
	//						);
	//		REPORTDB->Execute((_bstr_t)tableSQL,&RecordsAffected,adCmdText);
	//		char itemSQL[1024];
	//		sprintf(itemSQL,"Insert INTO DURATIONPARAMETER(MODELNAME , STARTTIME ,ENDTIME,PROJID) VALUES('%s' , '%d' , '%d', %d)",m_modelName,m_startTime,m_endTime,m_nProjID);
	//		REPORTDB->Execute((_bstr_t)itemSQL,&RecordsAffected,adCmdText);
	//		
	//		return true;
	//	}
	//	else
	//	{
	//		char itemSQL[1024];
	//		sprintf(itemSQL,"Insert INTO DURATIONPARAMETER(MODELNAME , STARTTIME ,ENDTIME,PROJID) VALUES('%s' , '%d' , '%d', %d)",m_modelName,m_startTime,m_endTime,m_nProjID);
	//		REPORTDB->Execute((_bstr_t)itemSQL,&RecordsAffected,adCmdText);

	//	}
	//}
	//catch (_com_error e)
	//{
	//	const TCHAR *ErrorMsg = e.ErrorMessage();
	//}
	return false;
}
