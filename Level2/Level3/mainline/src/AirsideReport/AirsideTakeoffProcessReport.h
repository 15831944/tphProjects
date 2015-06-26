//------------------------------------------------------------------------------------
//		Class:			CAirsideTakeoffProcessReport
//		Author:			Sky.wen
//		Date:			May 12, 2011
//		Purpose:		Generate take off process report
//------------------------------------------------------------------------------------
#pragma once
#include "AirsideBaseReport.h"
class CAirsideTakeoffProcessResult;
class CParameters;
class CXListCtrl;
class ArctermFile;
class CAirsideReportBaseResult;

class AIRSIDEREPORT_API CAirsideTakeoffProcessReport : public CAirsideBaseReport
{
public:
	//--------------------------------------------------------------------------------
	//Summary:
	//		constructor report object
	//--------------------------------------------------------------------------------
	CAirsideTakeoffProcessReport(CBGetLogFilePath pFunc);
	//-------------------------------------------------------------------------------
	//Summary:
	//		destroys class instance
	//-------------------------------------------------------------------------------
	virtual ~CAirsideTakeoffProcessReport();

	//------------------------------------------------------------------------------
	//Summary:
	//		build report result for report
	//Parameters:
	//		parameter[in]: user define parameter
	//------------------------------------------------------------------------------
	virtual void GenerateReport(CParameters * parameter);
	//------------------------------------------------------------------------------
	//Summary:
	//		update result of report and rebuild result for display
	//Parameters:
	//		parameter[in]: user define parameter
	//------------------------------------------------------------------------------
	virtual void RefreshReport(CParameters * parameter);
	//-----------------------------------------------------------------------------
	//Summary:
	//		retrieve report type
	//-----------------------------------------------------------------------------
	virtual int GetReportType();

	//------------------------------------------------------------------------------
	//Summary:
	//		init list ctrl header
	//Parameter:
	//		cxListCtrl[in/out]: handle of list ctrl
	//		reportType[in]:	detail or summary report
	//		CSortableHeaderCtrl[in/out]: handle of header ctrl
	//-----------------------------------------------------------------------------
	virtual void InitListHead(CXListCtrl& cxListCtrl, enumASReportType_Detail_Summary reportType, CSortableHeaderCtrl* piSHC=NULL);

	//----------------------------------------------------------------------------
	//Summary:
	//		fill list ctrl with result
	//Parameter:
	//		cxListCtrl[in/out]: handle of list ctrl
	//		parameter: user define data for report
	//---------------------------------------------------------------------------
	virtual void FillListContent(CXListCtrl& cxListCtrl, CParameters * parameter);

	//----------------------------------------------------------------------------
	//Summary:
	//		save data to file
	//Parameter:
	//		_file[in/out]:	destination file to write
	//return:
	//		true: success
	//		false: failed
	//---------------------------------------------------------------------------
	virtual BOOL WriteReportData(ArctermFile& _file);
	//---------------------------------------------------------------------------
	//Summary:
	//		load data from file
	//Parameter:
	//		_filein/out[]: destination file to load
	//return:
	//		true: success
	//		false: failed
	//----------------------------------------------------------------------------
	virtual BOOL ReadReportData(ArctermFile& _file);

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
	virtual BOOL ExportReportData(ArctermFile& _file,CString& Errmsg,enumASReportType_Detail_Summary reportType = ASReportType_Detail);
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
	virtual BOOL ImportReportData(ArctermFile& _file,CString& Errmsg,enumASReportType_Detail_Summary reportType = ASReportType_Detail);

	//--------------------------------------------------------------------------
	//Summary:
	//		file name that will load or save data of result
	//return:
	//		name of file that load or save data
	//--------------------------------------------------------------------------
	CString GetReportFileName();

	CAirsideReportBaseResult *GetReportResult();
private:
	CAirsideTakeoffProcessResult* m_pResult;//result pointer
};