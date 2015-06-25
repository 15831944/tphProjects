#pragma once
#include "airsideflightdelaybaseresult.h"
#include "SegmentDelayData.h"

class CDetailSegmentDelayResult :
	public CAirsideFlightDelayBaseResult
{
public:
	CDetailSegmentDelayResult(void);
	virtual ~CDetailSegmentDelayResult(void);

public:
	void GenerateResult(vector<CAirsideFlightDelayReport::FltTypeDelayItem>& fltTypeDelayData,CParameters *pParameter);
	void Draw3DChart(CARC3DChart& chartWnd, CParameters *pParameter);
	virtual CAirsideFlightDelayReport::subReportType getType(){ return CAirsideFlightDelayReport::SRT_DETAIL_SEGMENTDELAY;} 
protected:

	int GetFlightCountInIntervalTime(CAirsideFlightDelayReport::FltDelaySegment delaySegmentType, CParameters *pParameter, ElapsedTime& estMinDelayTime, ElapsedTime& estMaxDelayTime, vector<CAirsideFlightDelayReport::FltTypeDelayItem>& fltTypeDelayData);
	void ClearAllData();

private:
	vector<CSegmentDelayData*>          m_vSegmentData;
	vector<CString>                     m_vTimeRange;
	ElapsedTime                         m_estStartTime;
	ElapsedTime                         m_estEndTime;
};
