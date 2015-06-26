#pragma once


//#include "../common/template.h"
#include "LandsideReportAPI.h"
#include "LandsideReportType.h"




class ProjectCommon;
class ArctermFile;
class CXListCtrl;
class LandsideBaseReport;
class LandsideBaseParam;
class CSortableHeaderCtrl;
class InputLandside;




class LANDSIDEREPORT_API LandsideReportManager
{
public:
	LandsideReportManager(void);
	~LandsideReportManager(void);


public:
	void GenerateReport(InputLandside *pLandside);


public:
	void SetReportType(LandsideReportType rpType);
	LandsideReportType GetReportType() const;

	void SetCBGetLogFilePath(CBGetLogFilePath pFunc);
	CBGetLogFilePath GetCBGetLogFilePath();

	ProjectCommon * GetCommonData() const;
	void SetCommonData(ProjectCommon * pCommon);


	void SetReportPath(const CString& strReportPath);
	LandsideBaseParam *GetParameters() const;

	//---------------------------------------------------------------------------------------------
	//Summary:
	//		export list view data to csv file
	//---------------------------------------------------------------------------------------------
	BOOL ExportListCtrlToCvsFile(ArctermFile& _file,CXListCtrl& cxListCtrl);
	BOOL ExportListData(ArctermFile& _file,CXListCtrl& cxListCtrl);
	//----------------------------------------------------------------------------------------------

public:
	//---------------------------------------------------------------------------------
	//Summary:
	//		Onboard report instance
	//Parameter:
	//		nProjID[in]: project id
	//---------------------------------------------------------------------------------
	void InitUpdate(int nProjID);
	CString GetCurrentReportName() const ;


	//---------------------------------------------------------------------------------
	//Summary:
	//		set list ctrl and fill
	//---------------------------------------------------------------------------------
	void InitReportList(CXListCtrl& cxListCtrl, CSortableHeaderCtrl* piSHC = NULL);
	void SetReportListContent(CXListCtrl& cxListCtrl);
	//--------------------------------------------------------------------------------
public:
	//---------------------------------------------------------------------------------
	//Summary:
	//		retrieve current report
	//--------------------------------------------------------------------------------
	LandsideBaseReport* GetReport();

protected:
	LandsideReportType	m_emReportType;

	CBGetLogFilePath	m_pGetLogFilePath;


	CString				m_strReportFilePath;

	LandsideBaseReport  *m_pReport;

	ProjectCommon *m_pCommonData;



};
