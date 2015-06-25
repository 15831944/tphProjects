#include "stdafx.h"
#include "AirsideTakeoffProcessReport.h"
#include "AirsideTakeoffProcessResult.h"
#include "Parameters.h"
#include "../Common/termfile.h"
#include "AirsideTakeoffProcessDetailResult.h"
#include "AirsideTakeoffProcessSummaryResult.h"
#include "AirsideReportBaseResult.h"

//--------------------------------------------------------------------------------
//Summary:
//		constructor report object
//--------------------------------------------------------------------------------
CAirsideTakeoffProcessReport::CAirsideTakeoffProcessReport(CBGetLogFilePath pFunc)
:CAirsideBaseReport(pFunc)
,m_pResult(NULL)
{

}

//-------------------------------------------------------------------------------
//Summary:
//		destroys class instance
//-------------------------------------------------------------------------------
CAirsideTakeoffProcessReport::~CAirsideTakeoffProcessReport()
{
	if (m_pResult)
	{
		delete m_pResult;
		m_pResult = NULL;
	}
}

//------------------------------------------------------------------------------
//Summary:
//		build report result for report
//Parameters:
//		parameter[in]: user define parameter
//------------------------------------------------------------------------------
void CAirsideTakeoffProcessReport::GenerateReport( CParameters * parameter )
{
	ASSERT(parameter != NULL);
	if(parameter == NULL)
		return;

	//clear old result data
	if (m_pResult)
	{
		delete m_pResult;
		m_pResult = NULL;
	}

	if (parameter->getReportType() == ASReportType_Detail)
	{
		m_pResult = new CAirsideTakeoffProcessDetailResult();
	}
	else
	{
		m_pResult = new CAirsideTakeoffProcessSummaryResult();
	}

	if(m_pResult)
	{
		m_pResult->GenerateResult(m_pCBGetLogFilePath,parameter);
		SaveReportData();
	}
}

//------------------------------------------------------------------------------
//Summary:
//		update result of report and rebuild result for display
//Parameters:
//		parameter[in]: user define parameter
//------------------------------------------------------------------------------
void CAirsideTakeoffProcessReport::RefreshReport( CParameters * parameter )
{
	if(m_pResult)
	{
		m_pResult->RefreshReport(parameter);
	}
}

//-----------------------------------------------------------------------------
//Summary:
//		retrieve report type
//-----------------------------------------------------------------------------
int CAirsideTakeoffProcessReport::GetReportType()
{
	return Airside_TakeoffProcess;
}

//------------------------------------------------------------------------------
//Summary:
//		init list ctrl header
//Parameter:
//		cxListCtrl[in/out]: handle of list ctrl
//		reportType[in]:	detail or summary report
//		CSortableHeaderCtrl[in/out]: handle of header ctrl
//-----------------------------------------------------------------------------
void CAirsideTakeoffProcessReport::InitListHead( CXListCtrl& cxListCtrl, enumASReportType_Detail_Summary reportType, CSortableHeaderCtrl* piSHC/*=NULL*/ )
{
	if (m_pResult)
	{
		m_pResult->InitListHead(cxListCtrl,reportType);
	}
}

//----------------------------------------------------------------------------
//Summary:
//		fill list ctrl with result
//Parameter:
//		cxListCtrl[in/out]: handle of list ctrl
//		parameter: user define data for report
//---------------------------------------------------------------------------
void CAirsideTakeoffProcessReport::FillListContent( CXListCtrl& cxListCtrl, CParameters * parameter )
{
	if(m_pResult)
	{
		m_pResult->FillListContent(cxListCtrl,parameter);
	}
}

//----------------------------------------------------------------------------
//Summary:
//		save data to file
//Parameter:
//		_file[in/out]:	destination file to write
//return:
//		true: success
//		false: failed
//---------------------------------------------------------------------------
BOOL CAirsideTakeoffProcessReport::WriteReportData( ArctermFile& _file )
{	
	if(m_pResult)
	{
		return m_pResult->WriteReportData(_file);
	}
	return FALSE;
}

//---------------------------------------------------------------------------
//Summary:
//		load data from file
//Parameter:
//		_filein/out[]: destination file to load
//return:
//		true: success
//		false: failed
//----------------------------------------------------------------------------
BOOL CAirsideTakeoffProcessReport::ReadReportData( ArctermFile& _file )
{
	if(m_pResult != NULL)
		delete m_pResult;
	m_pResult = NULL;
	int nDetailOrSummary = 0;
	_file.getInteger(nDetailOrSummary);
	_file.getLine();
	if(nDetailOrSummary == ASReportType_Detail)
	{  //detail report
		m_pResult = new CAirsideTakeoffProcessDetailResult;
	}
	else
	{	//summary report
		m_pResult = new CAirsideTakeoffProcessSummaryResult;
	}

	if(m_pResult)
	{
		return m_pResult->ReadReportData(_file);
	}

	return FALSE;
}

//---------------------------------------------------------------------------
//Summary:
//		export data to file
//Parameter:
//		_file:	destination file to export
//		Errmsg[out]: error message for display
//		reportType: detail or summary type
//return:
//		true: success
//		false: failed
//-----------------------------------------------------------------------------
BOOL CAirsideTakeoffProcessReport::ExportReportData( ArctermFile& _file,CString& Errmsg,enumASReportType_Detail_Summary reportType /*= ASReportType_Detail*/ )
{
	if (m_pResult)
	{
		m_pResult->ExportReportData(_file,Errmsg);
	}
	return TRUE ;
}

//---------------------------------------------------------------------------
//Summary:
//		import data to file
//Parameter:
//		_file:	destination file to import
//		Errmsg[out]: error message for display
//		reportType: detail or summary type
//return:
//		true: success
//		false: failed
//-----------------------------------------------------------------------------
BOOL CAirsideTakeoffProcessReport::ImportReportData( ArctermFile& _file,CString& Errmsg,enumASReportType_Detail_Summary reportType /*= ASReportType_Detail*/ )
{
	if(m_pResult != NULL)
		delete m_pResult;
	m_pResult = NULL;

	//	_file.getLine();
	if(reportType == ASReportType_Detail)
	{  //detail report
		m_pResult = new CAirsideTakeoffProcessDetailResult;
	}
	else
	{	//summary report
		m_pResult = new CAirsideTakeoffProcessSummaryResult;
	}
	if (m_pResult)
	{
		m_pResult->ImportReportData(_file,Errmsg);
	}
	return TRUE ;
}

//--------------------------------------------------------------------------
//Summary:
//		file name that will load or save data of result
//return:
//		name of file that load or save data
//--------------------------------------------------------------------------
CString CAirsideTakeoffProcessReport::GetReportFileName()
{
	return _T("TakeoffProcess\\TakeoffProcess.rep");
}

CAirsideReportBaseResult * CAirsideTakeoffProcessReport::GetReportResult()
{
	return m_pResult;
}