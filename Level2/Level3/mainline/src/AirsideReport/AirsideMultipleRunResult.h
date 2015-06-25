#pragma once
#include "ParameterCaptionDefine.h"
#include <map>
#include <vector>
class CARC3DChart;
class CXListCtrl;
class CParameters;
class CSortableHeaderCtrl;

struct MultipleRunReportData
{
	MultipleRunReportData()
		:m_iStart(0)
		,m_iEnd(0)
		,m_iData(0)
	{

	}
	long m_iStart;
	long m_iEnd;
	long m_iData;
};
typedef std::map<CString,CString> DelayResultPath;
typedef std::map<CString,std::vector<long>> mapLoadResult;
typedef std::map<CString,std::vector<MultipleRunReportData>> MultiRunDetailMap;

class AIRSIDEREPORT_API CAirsideMultipleRunResult
{
public:
	CAirsideMultipleRunResult(void);
	~CAirsideMultipleRunResult(void);

	virtual void LoadMultipleRunReport(CParameters* pParameter) = 0;
	virtual void InitListHead(CXListCtrl& cxListCtrl, CParameters * parameter, int iType = 0,CSortableHeaderCtrl* piSHC=NULL) = 0;
	virtual void FillListContent(CXListCtrl& cxListCtrl, CParameters * parameter,int iType = 0) = 0;
	virtual void Draw3DChart(CARC3DChart& chartWnd, CParameters *pParameter,int iType = 0) = 0;

	void AddSimResultPath(const CString& strSimResult, const CString& strSimPath);
	void ClearResultPath();
protected:
	DelayResultPath m_mapResultPath;
};

