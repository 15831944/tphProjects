#pragma once
#include "AirsideNodeDelayReport.h"
#include "SummaryNodeDelayDataItem.h"
#include "../Reports/StatisticalTools.h"
#include "AirsideReportBaseResult.h"
#include <vector>
using namespace std;
class CARC3DChart;
class CParameters;

class CSummaryNodeDelayResult : public CAirsideReportBaseResult
{
public:
	CSummaryNodeDelayResult(void);
	~CSummaryNodeDelayResult(void);

public:
	void GenerateResult(std::vector<CAirsideNodeDelayReport::nodeDelay>& fltDelayData,CParameters *pParameter);
	//bool PrepareData(std::vector<CAirsideFlightDelayReport::FltTypeDelayItem>& fltDelayData, FlightConstraint& fltConstraint, vector<long>& vDelayTime, CStatisticalTools<double>& statisticalTool);
	void Draw3DChart(CARC3DChart& chartWnd, CParameters *pParameter);

	virtual CAirsideFlightDelayReport::subReportType getType() { return CAirsideFlightDelayReport::SRT_SUMMARY_NODEDELAY; }

	void GetSummaryResult(vector<CSummaryNodeDelayDataItem>& vSummaryResult);
protected:
	//find airsideReportNode in nodeDelayData, if exist, return true and nIndex, else return false
	bool IsNodeExist(std::vector<CAirsideNodeDelayReport::nodeDelay>& nodeDelayData, CAirsideReportNode& airsideReportNode, int& nIndex);
	//if no data return false else return true
	bool PrepareData(CAirsideNodeDelayReport::nodeDelay& reportNodeDelay, vector<long>& vDelayTime, CStatisticalTools<double>& statisticalTool);

private:
	vector<CSummaryNodeDelayDataItem>    m_vData;
};
