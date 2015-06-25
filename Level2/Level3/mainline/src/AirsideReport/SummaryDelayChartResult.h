#pragma once
#include "AirsideReportBaseResult.h"

class CParameters;
class AirsideFlightRunwayDelayData;
class AirsideFlightRunwayDelaySummaryData;
class AIRSIDEREPORT_API SummaryDelayChartResult : public CAirsideReportBaseResult
{
public:
	SummaryDelayChartResult(void);
	~SummaryDelayChartResult(void);

public:
	void GenerateResult(std::vector<AirsideFlightRunwayDelaySummaryData*>& vSummaryData);
	virtual void Draw3DChart(CARC3DChart& chartWnd, CParameters *pParameter);

private:
	std::vector<AirsideFlightRunwayDelaySummaryData*> m_vSummaryData;
};
