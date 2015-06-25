#pragma once
#include "airsidereportbaseresult.h"
#include "AirsideDistanceTravelReport.h"


class CParameters;
class CAirsideFlightDistanceTravelReportBaseResult :
	public CAirsideReportBaseResult
{
public:
	CAirsideFlightDistanceTravelReportBaseResult(void);
	~CAirsideFlightDistanceTravelReportBaseResult(void);

	virtual void GenerateResult(std::vector<CAirsideDistanceTravelReport::DistanceTravelReportItem>& vResult,CParameters *pParameter) = 0;

};
