#include "StdAfx.h"
#include "AirsideFlightStandOperationReport.h"
#include "AirsideFlightStandOperationBaseResult.h"
#include "Parameters.h"

CAirsideFlightStandOperationReport::CAirsideFlightStandOperationReport(CBCScheduleStand pShecuduleFunc,CBGetLogFilePath pFunc)
:CAirsideBaseReport(pFunc)
,m_pScheduleStand(pShecuduleFunc)
,m_pBaseResult(NULL)
{

}

CAirsideFlightStandOperationReport::~CAirsideFlightStandOperationReport()
{
	if (m_pBaseResult)
	{
		delete m_pBaseResult;
		m_pBaseResult = NULL;
	}
}

void CAirsideFlightStandOperationReport::GenerateReport(CParameters * parameter)
{
	ASSERT(parameter != NULL);
	if(parameter == NULL)
		return;

	if (m_pBaseResult)
	{
		delete m_pBaseResult;
		m_pBaseResult = NULL;
	}
	
	if (parameter->getReportType() == ASReportType_Detail)
	{
		m_pBaseResult = new CDetailStandOperationResult();
	}
	else
	{
		m_pBaseResult = new CSummaryStandOperationResult();
	}

	if(m_pBaseResult)
	{
		m_pBaseResult->SetCBSecheduleStand(m_pScheduleStand);
		m_pBaseResult->GenerateResult(m_pCBGetLogFilePath,parameter);
		m_pBaseResult->SetCBGetFilePath(m_pCBGetLogFilePath);
		SaveReportData();
	}

}

void CAirsideFlightStandOperationReport::RefreshReport(CParameters * parameter)
{
	if(m_pBaseResult)
	{
		m_pBaseResult->RefreshReport(parameter);
	}
}

CString CAirsideFlightStandOperationReport::GetReportFileName()
{
	return _T("StandOperations\\StandOperations.rep");
}


void CAirsideFlightStandOperationReport::FillListContent(CXListCtrl& cxListCtrl, CParameters * parameter)
{
	if(m_pBaseResult)
	{
		m_pBaseResult->FillListContent(cxListCtrl,parameter);
	}
}

void CAirsideFlightStandOperationReport::InitListHead(CXListCtrl& cxListCtrl, enumASReportType_Detail_Summary reportType, CSortableHeaderCtrl* piSHC/* =NULL */)
{
	if(m_pBaseResult)
	{
		m_pBaseResult->InitListHead(cxListCtrl,reportType,piSHC);
	}
}

BOOL CAirsideFlightStandOperationReport::ExportReportData(ArctermFile& _file,CString& Errmsg,enumASReportType_Detail_Summary reportType /* = ASReportType_Detail */)
{
	if (m_pBaseResult)
	{
		m_pBaseResult->ExportReportData(_file,Errmsg);
	}
	return TRUE ;
}

BOOL CAirsideFlightStandOperationReport::ImportReportData(ArctermFile& _file,CString& Errmsg,enumASReportType_Detail_Summary reportType /* = ASReportType_Detail */)
{
	if(m_pBaseResult != NULL)
		delete m_pBaseResult;
	m_pBaseResult = NULL;

	//	_file.getLine();
	if(reportType == ASReportType_Detail)
	{  //detail report
		m_pBaseResult = new CDetailStandOperationResult;
	}
	else
	{	//summary report
		m_pBaseResult = new CSummaryStandOperationResult;
	}
	if (m_pBaseResult)
	{
		m_pBaseResult->ImportReportData(_file,Errmsg);
	}
	return TRUE ;
}

BOOL CAirsideFlightStandOperationReport::WriteReportData( ArctermFile& _file )
{
	if(m_pBaseResult)
	{
		return m_pBaseResult->WriteReportData(_file);
	}
	return FALSE;
}

BOOL CAirsideFlightStandOperationReport::ReadReportData( ArctermFile& _file )
{
	if(m_pBaseResult != NULL)
		delete m_pBaseResult;
	m_pBaseResult = NULL;
	int nDetailOrSummary = 0;
	_file.getInteger(nDetailOrSummary);
	_file.getLine();
	if(nDetailOrSummary == ASReportType_Detail)
	{  //detail report
		m_pBaseResult = new CDetailStandOperationResult;
	}
	else
	{	//summary report
		m_pBaseResult = new CSummaryStandOperationResult;
	}

	if(m_pBaseResult)
	{
		m_pBaseResult->setLoadFromFile(true);
		m_pBaseResult->SetCBGetFilePath(m_pCBGetLogFilePath);//get file path method pointer

		return m_pBaseResult->ReadReportData(_file);
	}

	return FALSE;
}