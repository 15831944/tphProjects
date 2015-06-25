#pragma once

#include "AirsideBaseReport.h"
//#include "TypeDefs.h"
#include "AirsideReportObjectIntersecNodeIDNameMap.h"

class Terminal;
class CParameters;
class CAirsideBaseReport;
class CAirportDatabase ;
class CARCUnitManager;
class AIRSIDEREPORT_API CAirsideReportManager
{
public:
	CAirsideReportManager();
	~CAirsideReportManager(void);

	//members
public:
	void SetReportType(reportType rpType);
	reportType GetReportType();

	void SetProjectID(int prjID){ m_nProjID = prjID;}

	void SetCBGetLogFilePath(CBGetLogFilePath pFunc){ m_pGetLogFilePath = pFunc;}
	CBGetLogFilePath GetCBGetLogFilePath(){ return m_pGetLogFilePath;}

	void SetReportPath(const CString& strReportPath);
	void SetTerminalAndProjectPath(Terminal* pTerminal,const CString& _csProjPath);

protected:
	CParameters* m_pParamters;	//report parameters 
	CAirsideBaseReport* m_pAirsideReport;			//report result


	reportType				m_emReportType;

	int m_nProjID;
	CBGetLogFilePath		m_pGetLogFilePath;
	
	//report file path, such as d:\\arcterm\\aaa\\simresult0
	CString					m_strReportFilePath;
	Terminal*				m_pTerminal;
	CString					m_csProjPath;


	CAirsideReportObjectIntersecNodeIDNameMap m_projObjectData;
	//operation
public:
	void InitUpdate(int nProjID);
	void GenerateResult();
	void InitReport();
	void DisplayReport();
	void RefreshReport(int nUnit);
	void Update3DChart();
	void Set3DChartType(AirsideThreeDChartType enmChartType);
	void Set3DChartZoomMode(void);
	void Set3DChartMoveMode(void);

	void InitReportList(CXListCtrl& cxListCtrl, CSortableHeaderCtrl* piSHC = NULL);
	void SetReportListContent(CXListCtrl& cxListCtrl);
		

	//PRINT GRAPH ,EXPORT GRAPH
	void PrintGraph();
	void ExportGraph(BSTR bstrFileName);

	void PrintListData();

	CParameters* GetParameters();

	CAirsideBaseReport * GetReport();


	CString GetCurrentReportName();
protected:
	CAirportDatabase* m_airportDB ;
public:
	void SetAirportDB(CAirportDatabase* _airportDB ) { m_airportDB = _airportDB ;} ;
public:
	BOOL ImportReportFromFile(ArctermFile& _file,CString& ErrorMsg,int nProID) ;
	BOOL ExportReportToFile(ArctermFile& _file,CString& ErrorMsg) ;
	BOOL ImportParaFromFile(ArctermFile& _file);
	BOOL ExportParaToFile(ArctermFile& _file);
	BOOL GenerateCvsFileFormList(ArctermFile& _file,CXListCtrl& cxListCtrl,CString& ErrorMsg);
public:
	void SetGlobalUnit(CARCUnitManager* _UnitManger) ;
};
