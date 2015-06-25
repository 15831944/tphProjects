#pragma once
#include "AirsideReportBaseResult.h"

class FlightConflictReportData;
class AIRSIDEREPORT_API FltConflictChartResultByTimeOfDay: public CAirsideReportBaseResult
{
public:
	FltConflictChartResultByTimeOfDay(void);
	~FltConflictChartResultByTimeOfDay(void);

public:
	void GenerateResult(FlightConflictReportData* pData);
	virtual void Draw3DChart(CARC3DChart& chartWnd, CParameters *pParameter);

private:
	FlightConflictReportData*  m_pData;
};
