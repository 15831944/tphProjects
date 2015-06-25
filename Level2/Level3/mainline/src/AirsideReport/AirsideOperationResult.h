#pragma once
#include "airsidereportbaseresult.h"
#include "AirsideOperationReport.h"
#include "OperationReportDataItem.h"
#include <vector>
using namespace std;

class COperationReportDataItem;

class CAirsideOperationResult :
	public CAirsideReportBaseResult
{
public:
	CAirsideOperationResult(void);
	virtual ~CAirsideOperationResult(void);

public:
	void GenerateResult(std::vector<CAirsideOperationReport::AirsideFltOperationItem>& airsideFltOperationItem,CParameters *pParameter);
	void Draw3DChart(CARC3DChart& chartWnd, CParameters *pParameter);
	//virtual CAirsideFlightDelayReport::subReportType getType() { return CAirsideFlightDelayReport::SRT_DETAIL_NODEDELAY; }

private:
	void ClearAllData();
	long ClacTimeRange(ElapsedTime& eMaxTime,ElapsedTime& eMinValue,ElapsedTime& eInterval);
	int GetFlightNum(std::vector<CAirsideOperationReport::AirsideFltOperationItem>& airsideFltOperationItem, ENUMFLIGHTOPERATIONTYPE operationType, ElapsedTime& estStartTime, ElapsedTime& estEndTime, CParameters* pParameter);

private:
	vector<COperationReportDataItem*>  m_vOperationdReportData;
	vector<CString>                    m_vTimeRange;
	ElapsedTime                        m_estStartTime;
	ElapsedTime                        m_estEndTime;
};