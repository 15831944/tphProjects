#include "StdAfx.h"
#include ".\airsiderunwayoperationreportbaseresult.h"

CAirsideRunwayOperationReportBaseResult::CAirsideRunwayOperationReportBaseResult(void)
{
	m_pCBGetLogFilePath = NULL;
	m_pChartResult = NULL;
	m_AirportDB = NULL;
	m_bLoadFromFile = false;
}

CAirsideRunwayOperationReportBaseResult::~CAirsideRunwayOperationReportBaseResult(void)
{
}

void CAirsideRunwayOperationReportBaseResult::GenerateResult(CParameters *pParameter)
{
	ASSERT(0);
}

void CAirsideRunwayOperationReportBaseResult::Draw3DChart()
{
	ASSERT(0);
}

void CAirsideRunwayOperationReportBaseResult::SetCBGetFilePath( CBGetLogFilePath pFunc )
{
	m_pCBGetLogFilePath= pFunc;
}

void CAirsideRunwayOperationReportBaseResult::InitListHead( CXListCtrl& cxListCtrl, CSortableHeaderCtrl* piSHC )
{
	cxListCtrl.DeleteAllItems();

	while(cxListCtrl.GetHeaderCtrl()->GetItemCount() >0)
		cxListCtrl.DeleteColumn(0);
}

void CAirsideRunwayOperationReportBaseResult::FillListContent( CXListCtrl& cxListCtrl, CParameters * parameter )
{

}

void CAirsideRunwayOperationReportBaseResult::RefreshReport( CParameters * parameter )
{

}

CAirsideReportBaseResult * CAirsideRunwayOperationReportBaseResult::GetReportResult()
{
	return m_pChartResult;
}

BOOL CAirsideRunwayOperationReportBaseResult::WriteReportData( ArctermFile& _file )
{
	return FALSE;
}

BOOL CAirsideRunwayOperationReportBaseResult::ReadReportData( ArctermFile& _file )
{
	return FALSE;
}

void CAirsideRunwayOperationReportBaseResult::setLoadFromFile( bool bLoad )
{
	m_bLoadFromFile = bLoad;
}

bool CAirsideRunwayOperationReportBaseResult::IsLoadFromFile()
{
	return m_bLoadFromFile;

}