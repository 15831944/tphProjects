#pragma once
#include "ParameterCaptionDefine.h"
#include <map>
class CARC3DChart;
class CXListCtrl;
class CParameters;
class CSortableHeaderCtrl;

typedef std::map<CString,CString> DelayResultPath;
class AIRSIDEREPORT_API CAirsideMultipleRunResult
{
public:
	CAirsideMultipleRunResult(void);
	~CAirsideMultipleRunResult(void);

	virtual void LoadMultipleRunReport(long iInterval) = 0;
	virtual void InitListHead(CXListCtrl& cxListCtrl, CParameters * parameter, int iType = 0,CSortableHeaderCtrl* piSHC=NULL) = 0;
	virtual void FillListContent(CXListCtrl& cxListCtrl, CParameters * parameter,int iType = 0) = 0;
	virtual void Draw3DChart(CARC3DChart& chartWnd, CParameters *pParameter,int iType = 0) = 0;

	void AddSimResultPath(const CString& strSimResult, const CString& strSimPath);
	void ClearResultPath();
protected:
	DelayResultPath m_mapResultPath;
};

