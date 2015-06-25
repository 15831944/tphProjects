#pragma once
#include "AirsideIntersectionReportResult.h"

class CAirsideIntersectionSummaryBaseChartResult
{
public:
	CAirsideIntersectionSummaryBaseChartResult(void);
	virtual ~CAirsideIntersectionSummaryBaseChartResult(void);
public:

	virtual void GenerateResult(std::vector<CAirsideIntersectionSummarylResult::NodeSummaryResultItem>& vResult,CParameters *pParameter);
	virtual void Draw3DChart(CARC3DChart& chartWnd, CParameters *pParameter) = 0;

public:

	std::vector<CAirsideIntersectionSummarylResult::NodeSummaryResultItem>* m_pResultList;
};



class CAirsideIntersectionSummaryCrossingTimeChartResult : public CAirsideIntersectionSummaryBaseChartResult
{
public:
	CAirsideIntersectionSummaryCrossingTimeChartResult(void);
	~CAirsideIntersectionSummaryCrossingTimeChartResult(void);
public:
	virtual void Draw3DChart(CARC3DChart& chartWnd, CParameters *pParameter);
};

class CAirsideIntersectionSummaryCrossingDelayChartResult : public CAirsideIntersectionSummaryBaseChartResult
{
public:
	CAirsideIntersectionSummaryCrossingDelayChartResult(void);
	~CAirsideIntersectionSummaryCrossingDelayChartResult(void);
public:
	virtual void Draw3DChart(CARC3DChart& chartWnd, CParameters *pParameter);
};








