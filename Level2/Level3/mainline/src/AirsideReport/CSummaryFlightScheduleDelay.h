#pragma once
#include "summarydelayresult.h"
#include "AirsideFlightDelayReport.h"

class CSummaryFlightScheduleDelayResult :
	public CSummaryDelayResult
{
public:
	CSummaryFlightScheduleDelayResult(void);
	~CSummaryFlightScheduleDelayResult(void);

public:
	virtual void Draw3DChart(CARC3DChart& chartWnd, CParameters *pParameter);
	virtual bool PrepareData(std::vector<CAirsideFlightDelayReport::FltTypeDelayItem>& fltDelayData, FlightConstraint& fltConstraint, vector<long>& vDelayTime, CStatisticalTools<double>& statisticalTool);
	virtual CAirsideFlightDelayReport::subReportType getType(){ return CAirsideFlightDelayReport::SRT_SUMMARY_SCHEDULEDELAY;} 
	long GetFlightScheduleDelayTime(CAirsideFlightDelayReport::FltDelayItem& fltDelayItem)const;
};