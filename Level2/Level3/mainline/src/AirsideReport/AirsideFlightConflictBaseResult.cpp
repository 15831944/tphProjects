#include "StdAfx.h"
#include ".\airsideflightconflictbaseresult.h"
#include "FlightConflictReportData.h"
#include "AirsideFlightConflictPara.h"

AirsideFlightConflictBaseResult::AirsideFlightConflictBaseResult(CParameters * parameter)
{
	m_pParameter = (AirsideFlightConflictPara*)parameter;
	m_pCBGetLogFilePath = NULL;
	m_pChartResult = NULL;
	m_AirportDB = NULL;
	m_bLoadFromFile = false;
	m_pReportData = NULL;
}

AirsideFlightConflictBaseResult::~AirsideFlightConflictBaseResult(void)
{
}

void AirsideFlightConflictBaseResult::SetReportData(FlightConflictReportData* pReportData)
{
	m_pReportData = pReportData;
}

void AirsideFlightConflictBaseResult::SetCBGetFilePath( CBGetLogFilePath pFunc )
{
	m_pCBGetLogFilePath= pFunc;
}

void AirsideFlightConflictBaseResult::InitListHead( CXListCtrl& cxListCtrl, CSortableHeaderCtrl* piSHC )
{
	cxListCtrl.DeleteAllItems();

	while(cxListCtrl.GetHeaderCtrl()->GetItemCount() >0)
		cxListCtrl.DeleteColumn(0);
}

void AirsideFlightConflictBaseResult::FillListContent( CXListCtrl& cxListCtrl)
{

}

void AirsideFlightConflictBaseResult::RefreshReport(CParameters * parameter )
{

}

CAirsideReportBaseResult * AirsideFlightConflictBaseResult::GetReportResult()
{
	return m_pChartResult;
}

BOOL AirsideFlightConflictBaseResult::WriteReportData( ArctermFile& _file )
{
	_file.writeField(_T("Columns"));
	_file.writeLine();

	//columns
	CString strColumns= _T("Time, AC UId, AC1, 2nd UId, 2nd party, Conflict type, Delay, Action, AreaID, AreaName, Operation, Location");
	_file.writeField(strColumns);	
	_file.writeLine();

	//write data
	int nConflictCount = m_pReportData->m_vDataItemList.size() ;
	_file.writeField(_T("Conflict Count:"));
	_file.writeInt(nConflictCount);
	_file.writeLine();

	for (int i = 0; i < nConflictCount; i++)
	{
		FlightConflictReportData::ConflictDataItem* pConflict = m_pReportData->m_vDataItemList.at(i);
		_file.writeInt(long(pConflict->m_tTime));
		_file.writeInt(pConflict->m_nACID);
		_file.writeField(pConflict->m_strACName);
		_file.writeInt(pConflict->m_n2ndPartyID);
		_file.writeField(pConflict->m_str2ndPartyName);
		_file.writeInt(pConflict->m_nConflictType);
		_file.writeInt(long(pConflict->m_tDelay));
		_file.writeInt(pConflict->m_nAreaID);
		_file.writeField(pConflict->m_strAreaName);
		_file.writeInt(pConflict->m_nActionType);
		_file.writeInt(pConflict->m_nOperationType);
		_file.writeInt(pConflict->m_nLocationType);

		_file.writeLine();

	}
	return TRUE;
}

BOOL AirsideFlightConflictBaseResult::ReadReportData( ArctermFile& _file )
{
	_file.getLine();	//"Columns"
	_file.getLine();	//Head

	int nCount = 0;
	_file.skipField(1);		//skip "conflict count"
	_file.getInteger(nCount);
	_file.getLine();

	m_pReportData->ClearData();
	for (int i = 0; i < nCount; ++i)
	{
		FlightConflictReportData::ConflictDataItem* pConflict = new FlightConflictReportData::ConflictDataItem;

		long tTime;
		_file.getInteger(tTime);
		pConflict->m_tTime.setPrecisely(tTime);
		_file.getInteger(pConflict->m_nACID);
		_file.getField(pConflict->m_strACName.GetBuffer(1024),1024);
		pConflict->m_strACName.ReleaseBuffer();
		_file.getInteger(pConflict->m_n2ndPartyID);
		_file.getField(pConflict->m_str2ndPartyName.GetBuffer(1024),1024);
		pConflict->m_str2ndPartyName.ReleaseBuffer();
		_file.getInteger(pConflict->m_nConflictType);
		_file.getInteger(tTime);
		pConflict->m_tDelay.setPrecisely(tTime);
		_file.getInteger(pConflict->m_nAreaID);
		_file.getField(pConflict->m_strAreaName.GetBuffer(1024),1024);
		pConflict->m_strAreaName.ReleaseBuffer();
		_file.getInteger(pConflict->m_nActionType);
		_file.getInteger(pConflict->m_nOperationType);
		_file.getInteger(pConflict->m_nLocationType);
		
		m_pReportData->AddConflictItem(pConflict);
		_file.getLine();
	}

	return TRUE;
}

void AirsideFlightConflictBaseResult::setLoadFromFile( bool bLoad )
{
	m_bLoadFromFile = bLoad;
}

bool AirsideFlightConflictBaseResult::IsLoadFromFile()
{
	return m_bLoadFromFile;

}