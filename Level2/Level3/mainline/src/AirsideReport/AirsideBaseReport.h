#pragma once
#include "../Database/ARCportDatabaseConnection.h"
#include "../common/template.h"
#include "TypeDefs.h"
#include "../MFCExControl/XListCtrl.h"
#include "ParameterCaptionDefine.h"
#include "../Common/termfile.h"
#include "../MFCExControl/SortableHeaderCtrl.h"

#include "AirsideReportAPI.h"

class CAirportDatabase ;
class CParameters;
typedef CString (*CBGetLogFilePath)(InputFiles);
class AIRSIDEREPORT_API CAirsideBaseReport
{
public:
	CAirsideBaseReport(CBGetLogFilePath pFunc);
	virtual ~CAirsideBaseReport(void);

public:
	//load flight log files, and then generate report data
	virtual void GenerateReport(CParameters * parameter) = 0 ;

	//no need to load log, filter sub report data from the base data.
	virtual void RefreshReport(CParameters * parameter);

	virtual int GetReportType() = 0;

	virtual void InitListHead(CXListCtrl& cxListCtrl, enumASReportType_Detail_Summary reportType, CSortableHeaderCtrl* piSHC=NULL) = 0;

	virtual void FillListContent(CXListCtrl& cxListCtrl, CParameters * parameter);

	virtual CString GetObjectName(int objectType,int objID);
protected:
	CBGetLogFilePath m_pCBGetLogFilePath;

	//import sim file path 
	CString m_DescFile ;
	CString m_EventFile ;
public:
	void SetDescFile(CString& _descfile) { m_DescFile = _descfile ;};
	void SetEventFile(CString& _Eventfile) { m_EventFile = _Eventfile ;};

	void SetReportFileDir(const CString& strReportPath);

	 CString  GetReportFilePath();

//public:
//	virtual void FillListContentFromFile(CXListCtrl& cxListCtrl,ArctermFile& _file) ;
//	virtual void ExportListContentToFile(CXListCtrl& cxListCtrl,ArctermFile& _file)  ;
//
//	virtual void Draw3DChartFromFile(CARC3DChart& chartWnd, ArctermFile& _file,int isdetail) = 0 ;
//	virtual void ExportFileFrom3DChart(CARC3DChart& chartWnd,  ArctermFile& _file, CParameters *pParameter) = 0 ;
public:
    virtual BOOL ExportReportData(ArctermFile& _file,CString& Errmsg,enumASReportType_Detail_Summary reportType = ASReportType_Detail) = 0 ;
	virtual BOOL ImportReportData(ArctermFile& _file,CString& Errmsg,enumASReportType_Detail_Summary reportType = ASReportType_Detail) = 0 ;

	BOOL SaveReportData();
	BOOL LoadReportData();
	BOOL LoadReportData(const CString& strReportFile);
protected:
	virtual BOOL WriteReportData(ArctermFile& _file);
	virtual BOOL ReadReportData(ArctermFile& _file);


protected:
	CAirportDatabase* m_AirportDB ;
	CXListCtrl* m_ListCtrl ;
	CString			m_strReportFilePath;
public:
	void SetAirportDB(CAirportDatabase* _AirportDB) { m_AirportDB = _AirportDB ;} ;
public:
	BOOL ExportListCtrlToCvsFile(ArctermFile& _file,CXListCtrl& cxListCtrl,CParameters * parameter) ;
protected:
	virtual BOOL ExportListData(ArctermFile& _file,CParameters * parameter) ;

	virtual CString GetReportFileName();
protected:
	BOOL m_HasGenerate ;
public :
	BOOL IsReportGenerate() {return m_HasGenerate ;};
	void IsReportGenerate(BOOL _res) { m_HasGenerate = _res ;} ;
};
