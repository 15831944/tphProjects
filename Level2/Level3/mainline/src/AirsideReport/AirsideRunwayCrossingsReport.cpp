#include "StdAfx.h"
#include "AirsideRunwayCrossingsReport.h"
#include "AirsideRunwayCrossingsBaseResult.h"
#include "AirsideRunwayCrossingsPara.h"

CAirsideRunwayCrossingsReport::CAirsideRunwayCrossingsReport(CBGetLogFilePath pFunc)
:CAirsideBaseReport(pFunc)
,m_pResult(NULL)
{

}

CAirsideRunwayCrossingsReport::~CAirsideRunwayCrossingsReport()
{
	if (m_pResult)
	{
		delete m_pResult;
		m_pResult = NULL;
	}
}

void CAirsideRunwayCrossingsReport::GenerateReport(CParameters * parameter)
{
	ASSERT(parameter != NULL);
	if(parameter == NULL)
		return;

	if(m_pResult != NULL)
		delete m_pResult;
	m_pResult = NULL;

	if (parameter->getReportType() == ASReportType_Detail)
	{
		m_pResult = new CAirsideRunwayCrossingBaseDetailResult();
	}
	else
	{
		m_pResult = new CAirsideRunwayCrossingsBaseSummaryResult();
	}

	if(m_pResult)
	{
		m_pResult->GenerateResult(m_pCBGetLogFilePath,parameter);
		m_pResult->SetCBGetFilePath(m_pCBGetLogFilePath);
		SaveReportData();
	}

}

void CAirsideRunwayCrossingsReport::RefreshReport(CParameters * parameter)
{
	if(m_pResult)
	{
		m_pResult->RefreshReport(parameter);
	}
}

CString CAirsideRunwayCrossingsReport::GetReportFileName()
{
	return _T("RunwayCrossings\\RunwayCrossings.rep");
}


void CAirsideRunwayCrossingsReport::FillListContent(CXListCtrl& cxListCtrl, CParameters * parameter)
{
	if(m_pResult)
	{
		m_pResult->FillListContent(cxListCtrl,parameter);
	}
}

void CAirsideRunwayCrossingsReport::InitListHead(CXListCtrl& cxListCtrl, enumASReportType_Detail_Summary reportType, CSortableHeaderCtrl* piSHC/* =NULL */)
{
	if(m_pResult)
	{
		m_pResult->InitListHead(cxListCtrl,reportType);
	}
}

BOOL CAirsideRunwayCrossingsReport::ExportReportData(ArctermFile& _file,CString& Errmsg,enumASReportType_Detail_Summary reportType /* = ASReportType_Detail */)
{
	if (m_pResult)
	{
		m_pResult->ExportReportData(_file,Errmsg);
	}
	return TRUE ;
}

BOOL CAirsideRunwayCrossingsReport::ImportReportData(ArctermFile& _file,CString& Errmsg,enumASReportType_Detail_Summary reportType /* = ASReportType_Detail */)
{
	if(m_pResult != NULL)
		delete m_pResult;
	m_pResult = NULL;

//	_file.getLine();
	if(reportType == ASReportType_Detail)
	{  //detail report
		m_pResult = new CAirsideRunwayCrossingBaseDetailResult;
	}
	else
	{	//summary report
		m_pResult = new CAirsideRunwayCrossingsBaseSummaryResult;
	}
	if (m_pResult)
	{
		m_pResult->ImportReportData(_file,Errmsg);
	}
	return TRUE ;
}

BOOL CAirsideRunwayCrossingsReport::WriteReportData( ArctermFile& _file )
{
	if(m_pResult)
	{
		return m_pResult->WriteReportData(_file);
	}
	return FALSE;
}

BOOL CAirsideRunwayCrossingsReport::ReadReportData( ArctermFile& _file )
{
	if(m_pResult != NULL)
		delete m_pResult;
	m_pResult = NULL;
	int nDetailOrSummary = 0;
	_file.getInteger(nDetailOrSummary);
	_file.getLine();
	if(nDetailOrSummary == ASReportType_Detail)
	{  //detail report
		m_pResult = new CAirsideRunwayCrossingBaseDetailResult;
	}
	else
	{	//summary report
		m_pResult = new CAirsideRunwayCrossingsBaseSummaryResult;
	}

	if(m_pResult)
	{
		m_pResult->setLoadFromFile(true);
		m_pResult->SetCBGetFilePath(m_pCBGetLogFilePath);//get file path method pointer
	
		return m_pResult->ReadReportData(_file);
	}

	return FALSE;
}
