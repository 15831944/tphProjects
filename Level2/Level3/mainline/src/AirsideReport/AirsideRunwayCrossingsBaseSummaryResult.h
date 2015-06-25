#pragma once
#include "AirsideRunwayCrossingsBaseResult.h"
class CARC3DChart;
class CParameters;
class CRunwayCrossingBaseSummaryResult
{
public:
	CRunwayCrossingBaseSummaryResult();
	virtual ~CRunwayCrossingBaseSummaryResult();

protected:
	vector<CAirsideRunwayCrossingsBaseSummaryResult::SummaryRunwayCrossingsItem> m_vResult;
	std::vector<std::pair<CString,ALTObjectID> > GetRunwayList(std::vector<CAirsideRunwayCrossingsBaseSummaryResult::SummaryRunwayCrossingsItem>& vResult);
public:
	virtual void GenerateResult(vector<CAirsideRunwayCrossingsBaseSummaryResult::SummaryRunwayCrossingsItem>& vResult,CParameters *pParameter) = 0;
	virtual void Draw3DChart(CARC3DChart& chartWnd, CParameters *pParameter) = 0;
};

/////////////////////////////////////////////////////////////////////////////////////////
class CRunwayCrossingSummaryCountResult : public CRunwayCrossingBaseSummaryResult
{
public:
	CRunwayCrossingSummaryCountResult();
	~CRunwayCrossingSummaryCountResult();

public:
	virtual void GenerateResult(vector<CAirsideRunwayCrossingsBaseSummaryResult::SummaryRunwayCrossingsItem>& vResult,CParameters *pParameter);
	virtual void Draw3DChart(CARC3DChart& chartWnd, CParameters *pParameter);
};

//////////////////////////////////////////////////////////////////////////////////////////
class CRunwayCrossingSummaryWaitTimeResult : public CRunwayCrossingBaseSummaryResult
{
public:
	CRunwayCrossingSummaryWaitTimeResult();
	~CRunwayCrossingSummaryWaitTimeResult();

public:
	virtual void GenerateResult(vector<CAirsideRunwayCrossingsBaseSummaryResult::SummaryRunwayCrossingsItem>& vResult,CParameters *pParameter);
	virtual void Draw3DChart(CARC3DChart& chartWnd, CParameters *pParameter);
};