#pragma once
#include "airsideflightdelaybaseresult.h"
#include "SummarySegDelayData.h"
#include <vector>
using namespace std;

class CSummarySegmentDelayResult :
	public CAirsideFlightDelayBaseResult
{
public:
	CSummarySegmentDelayResult(void);
	virtual ~CSummarySegmentDelayResult(void);

	virtual void GenerateResult(vector<CAirsideFlightDelayReport::FltTypeDelayItem>& fltTypeDelayData,CParameters *pParameter);
	virtual void Draw3DChart(CARC3DChart& chartWnd, CParameters *pParameter);
	void PrepareData(std::vector<CAirsideFlightDelayReport::FltTypeDelayItem>& fltDelayData, CParameters *pParameter, CSummarySegDelayData& summarySegDelayData);
	virtual CAirsideFlightDelayReport::subReportType getType(){ return CAirsideFlightDelayReport::SRT_SUMMARY_SEGMENTDELAY;} 
private:
	vector<CSummarySegDelayData>    m_vSummarySegDelayData;
};
