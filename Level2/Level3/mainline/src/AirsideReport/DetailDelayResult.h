#pragma once
#include "airsideflightdelaybaseresult.h"
#include <iostream>
#include <vector>
using namespace std;
class CDetailFlightDelayData;

class CDetailDelayResult :
	public CAirsideFlightDelayBaseResult
{
public:
	CDetailDelayResult(void);
	virtual ~CDetailDelayResult(void);

public:
	virtual void GenerateResult(vector<CAirsideFlightDelayReport::FltTypeDelayItem>& fltTypeDelayData,CParameters *pParameter);
	virtual void Draw3DChart(CARC3DChart& chartWnd, CParameters *pParameter) = 0;
	virtual CAirsideFlightDelayReport::subReportType getType() = 0;

protected:
	virtual int GetFlightCountInIntervalTime(FlightConstraint& fltConstraint, ElapsedTime& estMinDelayTime, ElapsedTime& estMaxDelayTime, vector<CAirsideFlightDelayReport::FltTypeDelayItem>& fltTypeDelayData) = 0;
	void ClearAllData();

protected:
	vector<CDetailFlightDelayData*>    m_vFlightData;     //the infomation about the delayed flight
	vector<CString>                    m_vTimeRange;
	ElapsedTime                        m_estStartTime;
	ElapsedTime                        m_estEndTime;
};