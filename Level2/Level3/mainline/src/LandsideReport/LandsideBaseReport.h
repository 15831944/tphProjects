#pragma once
#include "LandsideReportAPI.h"
#include "LandsideReportManager.h"
#include "GraphChartType.h"


class LandsideBaseParam;
class LandsideBaseResult;
class ArctermFile;
class CARC3DChart;
class CSortableHeaderCtrl;
class CXListCtrl;
class InputLandside;




class LANDSIDEREPORT_API LandsideBaseReport
{
public:
	LandsideBaseReport(void);
	virtual ~LandsideBaseReport(void);



public:
	LandsideBaseParam *getParam() const{return m_pParemeter;}

public:

	void Initialize(ProjectCommon *pCommonData, const CString& strReportPath);

	CString  GetReportFilePath();

	//---------------------------------------------------------------------------------
	//Summary:
	//		retrieve passenger log path
	//Parameter:
	//		pFunc: pointer of function to get path of log file
	//---------------------------------------------------------------------------------
	void SetCBGetLogFilePath(CBGetLogFilePath pFunc);


	virtual void GenerateReport(CBGetLogFilePath pFunc, InputLandside *pLandside) = 0;

	virtual int GetReportType() const = 0;

	virtual CString GetReportFileName() = 0;
	virtual void RefreshReport()= 0;


	virtual void InitListHead(CXListCtrl& cxListCtrl, CSortableHeaderCtrl* piSHC = NULL);
	virtual void FillListContent(CXListCtrl& cxListCtrl );

	void Draw3DChart(CARC3DChart& chartWnd);

	BOOL SaveReportData();
	BOOL LoadReportData();
	BOOL LoadReportData(const CString& strReportFile);


public:
	virtual LSGraphChartTypeList GetChartList() const;
protected:
	virtual BOOL WriteReportData(ArctermFile& _file);
	virtual BOOL ReadReportData(ArctermFile& _file);






protected:
	LandsideBaseParam		*m_pParemeter;
	CString					m_strReportFilePath;
	LandsideBaseResult		*m_pResult;
	CBGetLogFilePath		m_pGetLogFilePath;//retrieve log path


};
