#pragma once
#include "summarydelayresult.h"
#include "AirsideFlightDelayReport.h"

class CSummaryStandDelayResult :
	public CSummaryDelayResult
{
public:
	CSummaryStandDelayResult(void);
	virtual ~CSummaryStandDelayResult(void);

public:
	virtual void Draw3DChart(CARC3DChart& chartWnd, CParameters *pParameter);
	virtual bool PrepareData(std::vector<CAirsideFlightDelayReport::FltTypeDelayItem>& fltDelayData, FlightConstraint& fltConstraint, vector<long>& vDelayTime, CStatisticalTools<double>& statisticalTool);
	virtual CAirsideFlightDelayReport::subReportType getType(){ return CAirsideFlightDelayReport::SRT_SUMMARY_STANDDELAY;} 
};
