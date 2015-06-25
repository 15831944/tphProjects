#pragma once
#include "detaildelayresult.h"
#include <iostream>
#include <vector>
using namespace std;

class CDetailFlightDelayData;

class CDetailTaxiDelayResult :
	public CDetailDelayResult
{
public:
	CDetailTaxiDelayResult(void);
	virtual ~CDetailTaxiDelayResult(void);

public:
	void Draw3DChart(CARC3DChart& chartWnd, CParameters *pParameter);
	virtual CAirsideFlightDelayReport::subReportType getType(){ return CAirsideFlightDelayReport::SRT_DETAIL_TAXIDELAY;} 

protected:
	int GetFlightCountInIntervalTime(FlightConstraint& fltConstraint, ElapsedTime& estMinDelayTime, ElapsedTime& estMaxDelayTime, vector<CAirsideFlightDelayReport::FltTypeDelayItem>& fltTypeDelayData);
};
