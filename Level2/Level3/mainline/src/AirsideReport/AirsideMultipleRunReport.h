#pragma once
#include <map>
#include "AirsideReport/TypeDefs.h"
#include "ParameterCaptionDefine.h"
enum InputFiles;
class Terminal;
class CParameters;
class CAirsideMultipleRunResult;
class CXListCtrl;
class CSortableHeaderCtrl;
class CARC3DChart;

typedef CString (*CBGetLogFilePath)(InputFiles);
typedef void (*CBSetCurrentSimResult)(int);
class AIRSIDEREPORT_API CAirsideMultipleRunReport
{
public:
	CAirsideMultipleRunReport(void);
	~CAirsideMultipleRunReport(void);
	void AddReportWhatToGen(reportType _reportType,CParameters * parameter);
	//generate airside report
	void GenerateReport(Terminal* pTerminal,CBGetLogFilePath pFunc,CBSetCurrentSimResult pSimFunc,const CString& strPorjectPath,reportType _reportType,CParameters * parameter);

	void InitListHead(reportType _reportType,CXListCtrl& cxListCtrl,CParameters * parameter,  int iType = 0,CSortableHeaderCtrl* piSHC=NULL);
	void FillListContent(reportType _reportType,CXListCtrl& cxListCtrl, CParameters * parameter,  int iType = 0);
	void Draw3DChart(reportType _reportType,CARC3DChart& chartWnd, CParameters *pParameter,  int iType = 0);
    CAirsideMultipleRunResult* GetMultipleRunResultByReportType(reportType reportT) const { return m_mapMutiRunResult.at(reportT); }
private:
	void ClearMutipleRunResult();
private:
	std::map<reportType,CAirsideMultipleRunResult*>m_mapMutiRunResult;
};

