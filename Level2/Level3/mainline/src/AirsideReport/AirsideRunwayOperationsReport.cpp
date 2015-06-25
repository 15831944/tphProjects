#include "StdAfx.h"
#include ".\airsiderunwayoperationsreport.h"
#include "AirsideRunwayOperationReportBaseResult.h"
#include "AirsideRunwayOperationReportDetailResult.h"
#include "AirsideRunwayOperationReportSummaryResult.h"
#include "AirsideRunwayOperationReportParam.h"
AirsideRunwayOperationsReport::AirsideRunwayOperationsReport(CBGetLogFilePath pFunc)
:CAirsideBaseReport(pFunc)
{
	m_pBaseResult = NULL;

}

AirsideRunwayOperationsReport::~AirsideRunwayOperationsReport(void)
{
}

void AirsideRunwayOperationsReport::GenerateReport( CParameters * parameter )
{
	ASSERT(parameter != NULL);
	if(parameter == NULL)
		return;
	
	//delete old result if exist
	delete m_pBaseResult;
	m_pBaseResult = NULL;

	if(parameter->getReportType() == ASReportType_Detail)
	{  //detail report
		m_pBaseResult = new CAirsideRunwayOperationReportDetailResult;
	}
	else
	{	//summary report
		m_pBaseResult = new CAirsideRunwayOperationReportSummaryResult;
	}

	if(m_pBaseResult)
	{
		m_pBaseResult->SetCBGetFilePath(m_pCBGetLogFilePath);//get file path method pointer
		m_pBaseResult->SetAirportDB(m_AirportDB);//
		m_pBaseResult->GenerateResult(parameter);
		SaveReportData();
	}

}

void AirsideRunwayOperationsReport::RefreshReport( CParameters * parameter )
{
	if(m_pBaseResult)
	{
		m_pBaseResult->RefreshReport(parameter);
	}

}

int AirsideRunwayOperationsReport::GetReportType()
{
	return Airside_RunwayOperaitons;
}

BOOL AirsideRunwayOperationsReport::ExportReportData( ArctermFile& _file,CString& Errmsg ,enumASReportType_Detail_Summary reportType )
{
	return FALSE;
}

BOOL AirsideRunwayOperationsReport::ImportReportData( ArctermFile& _file,CString& Errmsg ,enumASReportType_Detail_Summary reportType )
{
	return FALSE;
}

void AirsideRunwayOperationsReport::InitListHead( CXListCtrl& cxListCtrl, enumASReportType_Detail_Summary reportType, CSortableHeaderCtrl* piSHC )
{
	if(m_pBaseResult)
	{
		m_pBaseResult->InitListHead(cxListCtrl, piSHC);
	}
}

void AirsideRunwayOperationsReport::FillListContent( CXListCtrl& cxListCtrl, CParameters * parameter )
{
	if(m_pBaseResult)
	{
		m_pBaseResult->FillListContent(cxListCtrl,parameter);
	}
}

CAirsideReportBaseResult* AirsideRunwayOperationsReport::GetReportResult()
{	
	if(m_pBaseResult)
	{
		return m_pBaseResult->GetReportResult();
	}
	return NULL;

}

CString AirsideRunwayOperationsReport::GetReportFileName()
{
	return _T("RunwayOperations\\RunwayOperations.rep");
}

BOOL AirsideRunwayOperationsReport::WriteReportData( ArctermFile& _file )
{
	if(m_pBaseResult)
	{
		return m_pBaseResult->WriteReportData(_file);
	}
	return FALSE;
}

BOOL AirsideRunwayOperationsReport::ReadReportData( ArctermFile& _file )
{
	if(m_pBaseResult != NULL)
		delete m_pBaseResult;
	_file.getLine();
	m_pBaseResult = NULL;
	int nDetailOrSummary = 0;
	_file.getInteger(nDetailOrSummary);
	if(nDetailOrSummary == ASReportType_Detail)
	{  //detail report
		m_pBaseResult = new CAirsideRunwayOperationReportDetailResult;
	}
	else
	{	//summary report
		m_pBaseResult = new CAirsideRunwayOperationReportSummaryResult;
	}

	if(m_pBaseResult)
	{
		m_pBaseResult->setLoadFromFile(true);
		m_pBaseResult->SetCBGetFilePath(m_pCBGetLogFilePath);//get file path method pointer
		m_pBaseResult->SetAirportDB(m_AirportDB);//
		return m_pBaseResult->ReadReportData(_file);
	}

	return FALSE;
}