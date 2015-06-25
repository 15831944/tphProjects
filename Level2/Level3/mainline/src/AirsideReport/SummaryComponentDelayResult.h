#pragma once
#include "airsideflightdelaybaseresult.h"
#include <vector>
#include "SummaryComponentData.h"
using namespace std;

class CSummaryComponentDelayResult :
	public CAirsideFlightDelayBaseResult
{
public:
	CSummaryComponentDelayResult(void);
	virtual ~CSummaryComponentDelayResult(void);

	virtual void GenerateResult(vector<CAirsideFlightDelayReport::FltTypeDelayItem>& fltTypeDelayData,CParameters *pParameter);
	virtual void Draw3DChart(CARC3DChart& chartWnd, CParameters *pParameter);
	void PrepareData(std::vector<CAirsideFlightDelayReport::FltTypeDelayItem>& fltDelayData, CParameters *pParameter, CSummaryComponentData& summaryComponentDelayData);
	virtual CAirsideFlightDelayReport::subReportType getType(){ return CAirsideFlightDelayReport::SRT_SUMMARY_COMPONENTDELAY;} 
private:
	vector<CSummaryComponentData>   m_vSummaryComponentData;
};
