#pragma once
#include "AirsideFlightDelayReport.h"
#include <vector>
#include "AirsideFlightDelayReport.h"
#include "AirsideReportBaseResult.h"
using namespace std;
class CARC3DChart;
class CParameters;

class CAirsideFlightDelayBaseResult : public CAirsideReportBaseResult
{
public:
	CAirsideFlightDelayBaseResult(void);
	virtual ~CAirsideFlightDelayBaseResult(void);

public:

	virtual void GenerateResult(std::vector<CAirsideFlightDelayReport::FltTypeDelayItem>& fltDelayData,CParameters *pParameter) = 0;

	virtual void Draw3DChart(CARC3DChart& chartWnd, CParameters *pParameter) = 0;

	virtual CAirsideFlightDelayReport::subReportType getType() = 0;

protected:
	virtual long GetMinDelayTime(vector<CAirsideFlightDelayReport::FltTypeDelayItem>& fltTypeDelayData);
	virtual long GetMaxDelayTime(vector<CAirsideFlightDelayReport::FltTypeDelayItem>& fltTypeDelayData);
	long ClacTimeRange(ElapsedTime& eMaxTime,ElapsedTime& eMinValue,ElapsedTime& eInterval);
};
