#include "stdafx.h"
#include "AirsideTaxiwayUtilizationReport.h"
#include "AirsideTaxiwayUtilizationResult.h"
#include "Parameters.h"

CAirsideTaxiwayUtilizationReport::CAirsideTaxiwayUtilizationReport(CBGetLogFilePath pFunc)
:CAirsideBaseReport(pFunc)
,m_pResult(NULL)
{

}

CAirsideTaxiwayUtilizationReport::~CAirsideTaxiwayUtilizationReport()
{

}

void CAirsideTaxiwayUtilizationReport::GenerateReport(CParameters * parameter)
{
	ASSERT(parameter != NULL);
	if(parameter == NULL)
		return;

	if (parameter->getReportType() == ASReportType_Detail)
	{
		m_pResult = new CAirsideTaxiwayUtilizationDetailResult();
	}
	else
	{
		m_pResult = new CAirsideTaxiwayUtilizationSummaryResult();
	}

	if(m_pResult)
	{
		m_pResult->GenerateResult(m_pCBGetLogFilePath,parameter);
		m_pResult->SetCBGetFilePath(m_pCBGetLogFilePath);
		SaveReportData();
	}
}

void CAirsideTaxiwayUtilizationReport::RefreshReport(CParameters * parameter)
{
	if(m_pResult)
	{
		m_pResult->RefreshReport(parameter);
	}
}

void CAirsideTaxiwayUtilizationReport::InitListHead(CXListCtrl& cxListCtrl, enumASReportType_Detail_Summary reportType, CSortableHeaderCtrl* piSHC/* =NULL */)
{
	if(m_pResult)
	{
		m_pResult->InitListHead(cxListCtrl,reportType,piSHC);
	}
}

void CAirsideTaxiwayUtilizationReport::FillListContent(CXListCtrl& cxListCtrl, CParameters * parameter)
{
	if(m_pResult)
	{
		m_pResult->FillListContent(cxListCtrl,parameter);
	}
}

BOOL CAirsideTaxiwayUtilizationReport::ExportReportData(ArctermFile& _file,CString& Errmsg,enumASReportType_Detail_Summary reportType /* = ASReportType_Detail */)
{
	if (m_pResult)
	{
		m_pResult->ExportReportData(_file,Errmsg);
	}
	return TRUE ;
}

BOOL CAirsideTaxiwayUtilizationReport::ImportReportData(ArctermFile& _file,CString& Errmsg,enumASReportType_Detail_Summary reportType /* = ASReportType_Detail */)
{
	if(m_pResult != NULL)
		delete m_pResult;
	m_pResult = NULL;

	//	_file.getLine();
	if(reportType == ASReportType_Detail)
	{  //detail report
		m_pResult = new CAirsideTaxiwayUtilizationDetailResult;
	}
	else
	{	//summary report
		m_pResult = new CAirsideTaxiwayUtilizationSummaryResult;
	}
	if (m_pResult)
	{
		m_pResult->ImportReportData(_file,Errmsg);
	}
	return TRUE ;
}

BOOL CAirsideTaxiwayUtilizationReport::ReadReportData( ArctermFile& _file )
{
	if(m_pResult != NULL)
		delete m_pResult;
	m_pResult = NULL;
	int nDetailOrSummary = 0;
	_file.getInteger(nDetailOrSummary);
	_file.getLine();
	if(nDetailOrSummary == ASReportType_Detail)
	{  //detail report
		m_pResult = new CAirsideTaxiwayUtilizationDetailResult;
	}
	else
	{	//summary report
		m_pResult = new CAirsideTaxiwayUtilizationSummaryResult;
	}

	if(m_pResult)
	{
		m_pResult->setLoadFromFile(true);
		m_pResult->SetCBGetFilePath(m_pCBGetLogFilePath);//get file path method pointer

		return m_pResult->ReadReportData(_file);
	}

	return FALSE;
}

BOOL CAirsideTaxiwayUtilizationReport::WriteReportData( ArctermFile& _file )
{
	if(m_pResult)
	{
		return m_pResult->WriteReportData(_file);
	}
	return FALSE;
}

CString CAirsideTaxiwayUtilizationReport::GetReportFileName()
{
	return _T("TaxiwayUtilization\\TaxiwayUtilization.rep");
}