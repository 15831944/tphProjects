#include "StdAfx.h"
#include ".\airsiderunwayutilizationreport.h"
#include "AirsideRunwayUtilizationReporDetailResult.h"
#include "AirsideRunwayUtilizationReportSummaryResult.h"
#include "AirsideRunwayUtilizationReportParam.h"

CAirsideRunwayUtilizationReport::CAirsideRunwayUtilizationReport(CBGetLogFilePath pFunc)
:CAirsideBaseReport(pFunc)
{
	m_pBaseResult = NULL;
}

CAirsideRunwayUtilizationReport::~CAirsideRunwayUtilizationReport(void)
{
}

void CAirsideRunwayUtilizationReport::GenerateReport( CParameters * parameter )
{
	ASSERT(parameter != NULL);
	if(parameter == NULL)
		return;

	//delete old result if exist
	delete m_pBaseResult;
	m_pBaseResult = NULL;

	if(parameter->getReportType() == ASReportType_Detail)
	{  //detail report
		m_pBaseResult = new AirsideRunwayUtilizationReport::CDetailResult;
	}
	else
	{	//summary report
		m_pBaseResult = new AirsideRunwayUtilizationReport::CSummaryResult;
	}

	if(m_pBaseResult)
	{
		m_pBaseResult->SetCBGetFilePath(m_pCBGetLogFilePath);//get file path method pointer
		m_pBaseResult->SetAirportDB(m_AirportDB);//
		m_pBaseResult->SetProjID(parameter->GetProjID());
		m_pBaseResult->GenerateResult(parameter);
		SaveReportData();
	}
}

void CAirsideRunwayUtilizationReport::RefreshReport( CParameters * parameter )
{	
	if(m_pBaseResult)
	{
		m_pBaseResult->RefreshReport(parameter);
	}

}

int CAirsideRunwayUtilizationReport::GetReportType()
{
	return Airside_RunwayUtilization;
}

BOOL CAirsideRunwayUtilizationReport::ExportReportData( ArctermFile& _file,CString& Errmsg ,enumASReportType_Detail_Summary reportType )
{
	
	return FALSE;
}

BOOL CAirsideRunwayUtilizationReport::ImportReportData( ArctermFile& _file,CString& Errmsg ,enumASReportType_Detail_Summary reportType )
{
	
	return FALSE;
}

void CAirsideRunwayUtilizationReport::InitListHead( CXListCtrl& cxListCtrl, enumASReportType_Detail_Summary reportType, CSortableHeaderCtrl* piSHC )
{
	if(m_pBaseResult)
	{
		m_pBaseResult->InitListHead(cxListCtrl, piSHC);
	}
}

void CAirsideRunwayUtilizationReport::FillListContent( CXListCtrl& cxListCtrl, CParameters * parameter )
{
	if(m_pBaseResult)
	{
		m_pBaseResult->FillListContent(cxListCtrl,parameter);
	}
}

CAirsideReportBaseResult * CAirsideRunwayUtilizationReport::GetReportResult()
{
	if(m_pBaseResult)
	{
		return m_pBaseResult->GetReportResult();
	}
	return NULL;

}

CString CAirsideRunwayUtilizationReport::GetReportFileName()
{
	return _T("RunwayUtilization\\RunwayUtilization.rep");
}

BOOL CAirsideRunwayUtilizationReport::WriteReportData( ArctermFile& _file )
{
	if(m_pBaseResult)
	{
		return m_pBaseResult->WriteReportData(_file);
	}
	return FALSE;
}

BOOL CAirsideRunwayUtilizationReport::ReadReportData( ArctermFile& _file )
{
	if(m_pBaseResult != NULL)
		delete m_pBaseResult;
	_file.getLine();
	m_pBaseResult = NULL;
	int nDetailOrSummary = 0;
	_file.getInteger(nDetailOrSummary);
	_file.getLine();
	if(nDetailOrSummary == ASReportType_Detail)
	{  //detail report
		m_pBaseResult = new AirsideRunwayUtilizationReport::CDetailResult;
	}
	else
	{	//summary report
		m_pBaseResult = new AirsideRunwayUtilizationReport::CSummaryResult;
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

















