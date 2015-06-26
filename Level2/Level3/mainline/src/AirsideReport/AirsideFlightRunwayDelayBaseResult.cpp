#include "StdAfx.h"
#include ".\airsideflightrunwaydelaybaseresult.h"
#include "AirsideFlightRunwayDelayData.h"
#include "AirsideFlightRunwayDelayReportPara.h"

AirsideFlightRunwayDelayBaseResult::AirsideFlightRunwayDelayBaseResult(CParameters * parameter)
:m_pParameter((AirsideFlightRunwayDelayReportPara*)parameter)
,m_pCBGetLogFilePath(NULL)
,m_pChartResult(NULL)
,m_AirportDB(NULL)
,m_bLoadFromFile(false)
,m_pReportData(NULL)
{

}

AirsideFlightRunwayDelayBaseResult::~AirsideFlightRunwayDelayBaseResult(void)
{
}

void AirsideFlightRunwayDelayBaseResult::SetReportData(AirsideFlightRunwayDelayData* pReportData)
{
	m_pReportData = pReportData;
}

void AirsideFlightRunwayDelayBaseResult::SetCBGetFilePath( CBGetLogFilePath pFunc )
{
	m_pCBGetLogFilePath= pFunc;
}

void AirsideFlightRunwayDelayBaseResult::InitListHead( CXListCtrl& cxListCtrl, CSortableHeaderCtrl* piSHC )
{
	cxListCtrl.DeleteAllItems();

	while(cxListCtrl.GetHeaderCtrl()->GetItemCount() >0)
		cxListCtrl.DeleteColumn(0);
}

void AirsideFlightRunwayDelayBaseResult::FillListContent( CXListCtrl& cxListCtrl)
{

}

void AirsideFlightRunwayDelayBaseResult::RefreshReport(CParameters * parameter )
{

}

CAirsideReportBaseResult * AirsideFlightRunwayDelayBaseResult::GetReportResult()
{
	return m_pChartResult;
}

BOOL AirsideFlightRunwayDelayBaseResult::WriteReportData( ArctermFile& _file )
{
	_file.writeField(_T("Columns"));
	_file.writeLine();

	//columns
	CString strColumns= _T("Runway, AC UId, AC, Flight Type, Operation, Position, Start Time(hh:mm:ss), Expected Op Time(secs), Real Op Time(secs), \
						   Total delay(secs), Reason Count, Reason1, Delay1(secs), Reason2, Delay2(secs), Reason3, Delay3(secs)");
	_file.writeField(strColumns);
	_file.writeLine();

	//write data
	int nConflictCount = m_pReportData->m_vDataItemList.size() ;
	_file.writeField(_T("Delay Count:"));
	_file.writeInt(nConflictCount);
	_file.writeLine();

	for (int i = 0; i < nConflictCount; i++)
	{
		AirsideFlightRunwayDelayData::RunwayDelayItem* pItem = m_pReportData->m_vDataItemList.at(i);
		_file.writeInt(pItem->m_nRwyID);
		_file.writeInt(pItem->m_nRwyMark);
		_file.writeField(pItem->m_strRunwayMark);
		_file.writeInt(pItem->m_nACID);
		_file.writeField(pItem->m_strACName);
		_file.writeField(pItem->m_sACType);
		_file.writeInt(pItem->m_OperationType);
		_file.writeInt(pItem->m_PositionType);
		_file.writeInt(long(pItem->m_tStartTime));

		_file.writeInt(pItem->m_lExpectOpTime);
		_file.writeInt(pItem->m_lRealityOpTime);
		_file.writeInt(pItem->m_lTotalDelay);
		int nCount = pItem->m_vReasonDetail.size() < 3 ? pItem->m_vReasonDetail.size() :3;
		_file.writeInt(nCount);
		for (int j = 0; j < nCount; j++)
		{
			AirsideFlightRunwayDelayData::DETAILINFO info = pItem->m_vReasonDetail.at(j);
			_file.writeField(info.second);
			_file.writeInt(info.first);
		}
		_file.writeLine();


	}
	return TRUE;
}

BOOL AirsideFlightRunwayDelayBaseResult::ReadReportData( ArctermFile& _file )
{
	_file.getLine();	//"Columns"
	_file.getLine();   //Head

	int nCount = 0;
	_file.skipField(1);		//skip "delay count"
	_file.getInteger(nCount);
	_file.getLine();

	m_pReportData->ClearData();
	for (int i = 0; i < nCount; ++i)
	{
		AirsideFlightRunwayDelayData::RunwayDelayItem* pItem = new AirsideFlightRunwayDelayData::RunwayDelayItem;

		_file.getInteger(pItem->m_nRwyID);
		_file.getInteger(pItem->m_nRwyMark);
		_file.getField(pItem->m_strRunwayMark.GetBuffer(1024),1024);
		pItem->m_strRunwayMark.ReleaseBuffer();
		_file.getInteger(pItem->m_nACID);
		_file.getField(pItem->m_strACName.GetBuffer(1024),1024);
		pItem->m_strACName.ReleaseBuffer();
		_file.getField(pItem->m_sACType.GetBuffer(1024),1024);
		pItem->m_sACType.ReleaseBuffer();
		_file.getInteger(pItem->m_OperationType);
		_file.getInteger(pItem->m_PositionType);
		long tTime;
		_file.getInteger(tTime);
		pItem->m_tStartTime.setPrecisely(tTime);
		_file.getInteger(pItem->m_lExpectOpTime);
		_file.getInteger(pItem->m_lRealityOpTime);
		_file.getInteger(pItem->m_lTotalDelay);
		int nCount =0;
		_file.getInteger(nCount);
		for (int j = 0; j <nCount; j++)
		{
			AirsideFlightRunwayDelayData::DETAILINFO info;
			CString strReason;
			_file.getField(strReason.GetBuffer(1024),1024);
			strReason.ReleaseBuffer();
			info.second = strReason;
			_file.getInteger(info.first);
			pItem->m_vReasonDetail.push_back(info);
		}
		_file.getLine();

		m_pReportData->m_vDataItemList.push_back(pItem);
	}

	return TRUE;
}

void AirsideFlightRunwayDelayBaseResult::setLoadFromFile( bool bLoad )
{
	m_bLoadFromFile = bLoad;
}

bool AirsideFlightRunwayDelayBaseResult::IsLoadFromFile()
{
	return m_bLoadFromFile;

}