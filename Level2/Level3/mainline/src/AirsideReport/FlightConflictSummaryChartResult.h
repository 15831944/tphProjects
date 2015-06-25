#pragma once
#include "AirsideReportBaseResult.h"

class FlightConflictSummaryData;
class CParameters;
class FlightConflictSummaryChartResult: public CAirsideReportBaseResult
{
public:
	FlightConflictSummaryChartResult(void);
	~FlightConflictSummaryChartResult(void);

public:
	void GenerateResult(std::vector<FlightConflictSummaryData*>& vSummaryData);
	virtual void Draw3DChart(CARC3DChart& chartWnd, CParameters *pParameter);

private:
	std::vector<FlightConflictSummaryData*> m_vSummaryData;
};
