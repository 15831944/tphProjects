#include "StdAfx.h"
#include "AirsideDurationReport.h"
#include "AirsideDurationParam.h"
//#include "comutil.h"

using namespace ADODB;

CAirsideDurationReport::CAirsideDurationReport(CBGetLogFilePath pFunc)
:CAirsideBaseReport(pFunc)
{
	m_pRecordset	= NULL;
	m_dMinDurTime	= 0.0;
	m_dMaxDurTime	=0.0;
}

CAirsideDurationReport::~CAirsideDurationReport(void)
{
}

void CAirsideDurationReport::GenerateReport(CParameters * parameter)
{
	CAirsideDurationParam *durationParam = reinterpret_cast<CAirsideDurationParam *>(parameter); 
	
	char szSQL[1024];
	sprintf(szSQL,"SELECT FLIGHTID, STARTTIME ,ENDTIME ,ICAOCODE, ARRID ,DEPID FROM FLIGHTDESC \
		WHERE STARTTIME < ENDTIME  AND STARTTIME > %d AND ENDTIME < %d AND ProjID = %d ",
		durationParam->getStartTime(),durationParam->getEndTime(),durationParam->GetProjID());
	try
	{
		_variant_t RecordsAffected(0L);
		_RecordsetPtr pRecordset;

		//pRecordset.CreateInstance("ADODB.Recordset");
		pRecordset =DATABASECONNECTION.GetConnection()->Execute((_bstr_t)szSQL,&RecordsAffected,adCmdText);
		_variant_t vFlightID ,vStartTime , vEndTime, vICaoCode, vArrID ,vDepID;

		if(!pRecordset->adoEOF)
		{
			m_pRecordset = new AirsideDurationReportItem();

			vFlightID   = pRecordset->GetCollect("FLIGHTID") ;
			vStartTime  = pRecordset->GetCollect("STARTTIME");
			vEndTime	= pRecordset->GetCollect("ENDTIME");
			vICaoCode	= pRecordset->GetCollect("ICAOCODE");
			vArrID		= pRecordset->GetCollect("ARRID");
			vDepID		= pRecordset->GetCollect("DEPID");

			strcpy(m_pRecordset->FlightID, _com_util::ConvertBSTRToString(vFlightID.bstrVal));
			strcpy(m_pRecordset->ICaoCode, _com_util::ConvertBSTRToString(vICaoCode.bstrVal));
			strcpy(m_pRecordset->ArrID , _com_util::ConvertBSTRToString(vArrID.bstrVal));
			strcpy(m_pRecordset->DepID , _com_util::ConvertBSTRToString(vDepID.bstrVal));

			m_pRecordset->DurTime = vEndTime.lVal - vStartTime.lVal;
			
			////
			m_dMinDurTime = m_dMaxDurTime = m_pRecordset->DurTime;

			pRecordset->MoveNext();
		}

		AirsideDurationReportItem* durationReportItem = m_pRecordset;

		while (!pRecordset->adoEOF)
		{
            durationReportItem->nextItem = new AirsideDurationReportItem() ;
			AirsideDurationReportItem *tempItem = durationReportItem->nextItem;

			vFlightID   = pRecordset->GetCollect("FLIGHTID") ;
			vStartTime  = pRecordset->GetCollect("STARTTIME");
			vEndTime	= pRecordset->GetCollect("ENDTIME");
			vICaoCode	= pRecordset->GetCollect("ICAOCODE");
			vArrID		= pRecordset->GetCollect("ARRID");
			vDepID		= pRecordset->GetCollect("DEPID");

			strcpy(tempItem->FlightID, _com_util::ConvertBSTRToString(vFlightID.bstrVal));
			strcpy(tempItem->ICaoCode, _com_util::ConvertBSTRToString(vICaoCode.bstrVal));
			strcpy(tempItem->ArrID , _com_util::ConvertBSTRToString(vArrID.bstrVal));
			strcpy(tempItem->DepID , _com_util::ConvertBSTRToString(vDepID.bstrVal));

			tempItem->DurTime = vEndTime.lVal - vStartTime.lVal;

			if (tempItem->DurTime > m_dMaxDurTime)
			{
				m_dMaxDurTime = tempItem->DurTime ;
			}

			if (tempItem->DurTime < m_dMinDurTime)
			{
				m_dMinDurTime = tempItem->DurTime ;
			}
			
			durationReportItem = tempItem;

			pRecordset->MoveNext();
		}
	}
	catch (_com_error e)
	{
		const TCHAR* errMsg = e.ErrorMessage();
	}
	
	return;
}

long CAirsideDurationReport::GetCount(double start ,double end)
{
	long nCount = 0L;

	AirsideDurationReportItem *pDurReportItem = m_pRecordset;
	while (pDurReportItem != NULL)
	{
		if (pDurReportItem->DurTime >start && pDurReportItem->DurTime <end)
		{
			nCount ++ ;
		}

		pDurReportItem = pDurReportItem->nextItem;
	}

	return nCount;

}


