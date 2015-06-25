#pragma once
#include "CARC3DChart.h"
#include "AirsideFlightDelayReport.h"
class AIRSIDEREPORT_API CAirsideReportBaseResult
{
public:
	CAirsideReportBaseResult(void);
	virtual ~CAirsideReportBaseResult(void);

	virtual void Draw3DChart(CARC3DChart& chartWnd, CParameters *pParameter) = 0;
	//virtual CAirsideFlightDelayReport::subReportType getType() = 0;
	long  ClacValueRange(double& dMaxValue,double& dMinValue,double& dInterval);
};
