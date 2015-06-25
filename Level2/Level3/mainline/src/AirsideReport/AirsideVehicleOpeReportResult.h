#pragma once
#include "airsidereportbaseresult.h"
#include "Parameters.h"
class CAirsideVehicleOpeReportResult :
	public CAirsideReportBaseResult
{
public:
	CAirsideVehicleOpeReportResult(void);
	~CAirsideVehicleOpeReportResult(void);
public:
	virtual void GenerateResult() = 0;
	virtual void Draw3DChart(CARC3DChart& chartWnd, CParameters *pParameter) = 0;
};
