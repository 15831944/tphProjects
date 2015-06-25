#pragma once
#include "AirsideNodeDelayReport.h"
#include "DetailNodeDelayDataItem.h"
#include "AirsideFlightDelayReport.h"
#include "AirsideReportBaseResult.h"
#include <vector>
using namespace std;
class CARC3DChart;
class CParameters;

class CDetailNodeDelaydResult : public CAirsideReportBaseResult
{
public:
	CDetailNodeDelaydResult(void);
	~CDetailNodeDelaydResult(void);

public:
	void GenerateResult(std::vector<CAirsideNodeDelayReport::nodeDelay>& nodeDelayData,CParameters *pParameter);
	void Draw3DChart(CARC3DChart& chartWnd, CParameters *pParameter);

	virtual CAirsideFlightDelayReport::subReportType getType() { return CAirsideFlightDelayReport::SRT_DETAIL_NODEDELAY; }

protected:
	long GetMinDelayTime(vector<CAirsideNodeDelayReport::nodeDelay>& nodeDelayData);
	long GetMaxDelayTime(vector<CAirsideNodeDelayReport::nodeDelay>& nodeDelayData);
	long ClacTimeRange(ElapsedTime& eMaxTime,ElapsedTime& eMinValue,ElapsedTime& eInterval);
	void GetNodeDelayTimeInIntervalTime(ElapsedTime& estStartTime, ElapsedTime& estEndTime, vector<CAirsideNodeDelayReport::nodeDelay>& nodeDelayData, CParameters *pParameter);
	//if reportNodeDelay is exist in result, return true, nIndex is position, if not exist , return false
	bool IsNodeInResult(CAirsideNodeDelayReport::nodeDelay reportNodeDelay, int& nIndex);
	bool IsNodeSelected(CAirsideNodeDelayReport::nodeDelay reportNodeDelay, CParameters *pParameter);

private:
	vector<CDetailNodeDelayDataItem>    m_vData;
	vector<CString>                    m_vTimeRange;
	ElapsedTime                        m_estStartTime;
	ElapsedTime                        m_estEndTime;
};
