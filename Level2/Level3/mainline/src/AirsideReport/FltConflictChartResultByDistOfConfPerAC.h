#pragma once
#include "AirsideReportBaseResult.h"

class FlightConflictReportData;
class CParameters;
class AIRSIDEREPORT_API FltConflictChartResultByDistOfConfPerAC: public CAirsideReportBaseResult
{
public:
	FltConflictChartResultByDistOfConfPerAC(void);
	~FltConflictChartResultByDistOfConfPerAC(void);
public:
	void GenerateResult(FlightConflictReportData* pData);
	virtual void Draw3DChart(CARC3DChart& chartWnd, CParameters *pParameter);

private:
	FlightConflictReportData*  m_pData;
};
