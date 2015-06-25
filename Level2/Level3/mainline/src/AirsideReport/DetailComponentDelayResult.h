#pragma once
#include "airsideflightdelaybaseresult.h"
#include "DetailComponentData.h"

class CDetailComponentDelayResult :
	public CAirsideFlightDelayBaseResult
{
public:
	CDetailComponentDelayResult(void);
	virtual ~CDetailComponentDelayResult(void);

public:
	void GenerateResult(vector<CAirsideFlightDelayReport::FltTypeDelayItem>& fltTypeDelayData,CParameters *pParameter);
	void Draw3DChart(CARC3DChart& chartWnd, CParameters *pParameter);
	virtual CAirsideFlightDelayReport::subReportType getType(){ return CAirsideFlightDelayReport::SRT_DETAIL_COMPONENTDELAY;} 
protected:

	int GetFlightCountInIntervalTime(FltDelayReason delayReason, CParameters *pParameter, ElapsedTime& estMinDelayTime, ElapsedTime& estMaxDelayTime, vector<CAirsideFlightDelayReport::FltTypeDelayItem>& fltTypeDelayData);
	void ClearAllData();

private:
	vector<CDetailComponentData*>       m_vComponentData;
	vector<CString>                     m_vTimeRange;
	ElapsedTime                         m_estStartTime;
	ElapsedTime                         m_estEndTime;
};
