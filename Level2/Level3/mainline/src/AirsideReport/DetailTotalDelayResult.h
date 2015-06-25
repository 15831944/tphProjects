#pragma once
#include "DetailDelayResult.h"
#include <iostream>
#include <vector>
using namespace std;

class CDetailFlightDelayData;

class CDetailTotalDelayResult :
	public CDetailDelayResult
{
public:
	CDetailTotalDelayResult(void);
	~CDetailTotalDelayResult(void);

public:
	void Draw3DChart(CARC3DChart& chartWnd, CParameters *pParameter);
	void GenerateResult(vector<CAirsideFlightDelayReport::FltTypeDelayItem>& fltTypeDelayData,CParameters *pParameter);
	virtual CAirsideFlightDelayReport::subReportType getType(){ return CAirsideFlightDelayReport::SRT_DETAIL_FLIGHTTOTALDELAY;} 

	virtual long GetMinDelayTime(vector<CAirsideFlightDelayReport::FltTypeDelayItem>& fltTypeDelayData);
	virtual long GetMaxDelayTime(vector<CAirsideFlightDelayReport::FltTypeDelayItem>& fltTypeDelayData);
protected:
	int GetFlightCountInIntervalTime(FlightConstraint& fltConstraint, ElapsedTime& estMinDelayTime, ElapsedTime& estMaxDelayTime, vector<CAirsideFlightDelayReport::FltTypeDelayItem>& fltTypeDelayData);
};
