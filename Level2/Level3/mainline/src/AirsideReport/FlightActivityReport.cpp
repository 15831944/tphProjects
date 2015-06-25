#include "StdAfx.h"
#include ".\flightactivityreport.h"
#include "FlightActivityParam.h"
#include <limits>
#include "AirsideFlightActivityReportBaseResult.h"


const char* CFlightActivityReport::subReportName[] = {

	_T("System Entry Distribution"),
		_T("System Exit Distribution")

};

CFlightActivityReport::CFlightActivityReport(CBGetLogFilePath pFunc)
:CAirsideBaseReport(pFunc),
m_pResult(NULL)
{
}

CFlightActivityReport::~CFlightActivityReport(void)
{
}

void CFlightActivityReport::GenerateReport(CParameters * parameter)
{
	if (!parameter)
		return;

	if (NULL != m_pResult)
	{
		delete m_pResult;
		m_pResult = NULL;
	}
	if (parameter->getReportType() == ASReportType_Detail)
	{
		m_pResult = new CFlightDetailActivityResult();
	}
	else
	{
		m_pResult =  new CFlightSummaryActivityResult();
		
	}

	if(m_pResult)
	{
		m_pResult->SetCBGetFilePath(m_pCBGetLogFilePath);//get file path method pointer
		m_pResult->GenerateResult(m_pCBGetLogFilePath,parameter);
		SaveReportData();
	}
}

void CFlightActivityReport::RefreshReport(CParameters * parameter)
{
	CFlightActivityParam* pParam = (CFlightActivityParam*)parameter;
	ASSERT(pParam != NULL);
	
	CFlightActivityBaseResult* pResult = NULL;
	if (NULL != m_pResult)
	{
		pResult = m_pResult;
	}

	switch(pParam->getReportType())
	{
	case ASReportType_Detail:
		{
			m_pResult = new CFlightDetailActivityResult();
			if (pResult != NULL)
			{
				m_pResult->SetReportResult(pResult);
			}
			m_pResult->RefreshReport(parameter);
		}
		break;
	case ASReportType_Summary:
		{
			m_pResult = new CFlightSummaryActivityResult();
			if (pResult != NULL)
			{
				m_pResult->SetReportResult(pResult);
			}
			m_pResult->RefreshReport(parameter);
		}
	    break;
	default:
	    break;
	}

	if (pResult != NULL)
	{
		delete pResult;
		pResult = NULL;
	}
}

void CFlightActivityReport::InitListHead(CXListCtrl& cxListCtrl, enumASReportType_Detail_Summary reportType, CSortableHeaderCtrl* piSHC)
{
	if (m_pResult)
	{
		m_pResult->InitListHead(cxListCtrl,reportType,piSHC);
	}
}

void CFlightActivityReport::FillListContent(CXListCtrl& cxListCtrl, CParameters * parameter)
{
	if (m_pResult)
	{
		m_pResult->FillListContent(cxListCtrl,parameter);
	}
}

BOOL CFlightActivityReport::ExportReportData(ArctermFile& _file,CString& Errmsg,enumASReportType_Detail_Summary reportType )
{
	 if (m_pResult)
	 {
		 m_pResult->ExportReportData(_file,Errmsg);
	 }
 	return TRUE ;
}

BOOL CFlightActivityReport::ImportReportData(ArctermFile& _file,CString& Errmsg,enumASReportType_Detail_Summary reportType )
{
	if(m_pResult != NULL)
		delete m_pResult;
	m_pResult = NULL;
	if(reportType == ASReportType_Detail)
	{  //detail report
		m_pResult = new CFlightDetailActivityResult;
	}
	else
	{	//summary report
		m_pResult = new CFlightSummaryActivityResult;
	}

	 if (m_pResult)
	 {
		 m_pResult->ImportReportData(_file,Errmsg);
	 }
 	return TRUE ;
}

 CString CFlightActivityReport::GetReportFileName()
 {
	 return _T("FlightActivity\\FlightActivity.rep");
 }

 BOOL CFlightActivityReport::ReadReportData(ArctermFile& _file)
 {
	 if(m_pResult != NULL)
		 delete m_pResult;
	 m_pResult = NULL;
	 int nDetailOrSummary = 0;
	 _file.getInteger(nDetailOrSummary);
	 _file.getLine();
	 if(nDetailOrSummary == ASReportType_Detail)
	 {  //detail report
		 m_pResult = new CFlightDetailActivityResult;
	 }
	 else
	 {	//summary report
		 m_pResult = new CFlightSummaryActivityResult;
	 }

	 if(m_pResult)
	 {
		 m_pResult->setLoadFromFile(true);
		 m_pResult->SetCBGetFilePath(m_pCBGetLogFilePath);//get file path method pointer

		 return m_pResult->ReadReportData(_file);
	 }

	 return FALSE;
 }

 BOOL CFlightActivityReport::WriteReportData(ArctermFile& _file)
 {
	 if(m_pResult)
	 {
		 return m_pResult->WriteReportData(_file);
	 }
	 return FALSE;
 }

 BOOL CFlightActivityReport::ExportListData( ArctermFile& _file,CParameters * parameter )
 {
	 if(m_pResult == NULL)
		 return FALSE;
	 BOOL res = FALSE ;
	if(parameter->getReportType() == ASReportType_Detail )
			res = m_pResult->ExportListData(_file,parameter) ;
	else
			res = CAirsideBaseReport::ExportListData(_file,parameter) ;
	 return res ;
 }