#pragma once
#include "AirsideReportBaseResult.h"

class FlightConflictReportData;
class AIRSIDEREPORT_API FltConflictChartResultByOperationAndTimeOfDay: public CAirsideReportBaseResult
{
public:
	FltConflictChartResultByOperationAndTimeOfDay(void);
	~FltConflictChartResultByOperationAndTimeOfDay(void);

public:
	void GenerateResult(FlightConflictReportData* pData);
	virtual void Draw3DChart(CARC3DChart& chartWnd, CParameters *pParameter);
private:
	FlightConflictReportData*  m_pData;
};
