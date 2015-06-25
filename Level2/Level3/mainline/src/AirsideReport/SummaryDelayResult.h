#pragma once
#include "airsideflightdelaybaseresult.h"
#include "SummaryFlightTypeData.h"
#include "../Reports/StatisticalTools.h"
#include <vector>
using namespace std;
class C2DChartData;
class CSummaryFlightTypeData;

class CSummaryDelayResult :
	public CAirsideFlightDelayBaseResult
{
public:
	CSummaryDelayResult(void);
	virtual ~CSummaryDelayResult(void);

public:
	virtual void GenerateResult(vector<CAirsideFlightDelayReport::FltTypeDelayItem>& fltTypeDelayData,CParameters *pParameter);
	virtual void Draw3DChart(CARC3DChart& chartWnd, CParameters *pParameter) = 0;
	//if no data return false, have data return true
	virtual bool PrepareData(std::vector<CAirsideFlightDelayReport::FltTypeDelayItem>& fltDelayData, FlightConstraint& fltConstraint, vector<long>& vDelayTime, CStatisticalTools<double>& statisticalTool) = 0;

	void ClearAllData();
	//virtual CAirsideFlightDelayReport::subReportName getType(){ return CAirsideFlightDelayReport::SRT_SUMMARY_FLIGHTTOTALDELAY;} 

	virtual void Init3DChartLegend(C2DChartData& c2dGraphData);
	virtual void Fill3DChartData(CSummaryData& summaryData,C2DChartData& c2dGraphData);

	void GetSummaryResult(vector<CSummaryFlightTypeData>& vSummaryResult);
public:
	vector<CSummaryFlightTypeData>    m_vData;
};
