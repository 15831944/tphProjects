#pragma once
#include "../Common/elaptime.h"
#include <vector>
#include "AirsideFlightRunwayDelayBaseResult.h"

class AirsideFlightRunwayDelayData;
class AirsideFlightRunwayDelayReportPara;
class AirsideFlightRunwayDelaySummaryData;
class AIRSIDEREPORT_API SummaryRunwayDelayResult: public AirsideFlightRunwayDelayBaseResult
{
public:
	SummaryRunwayDelayResult(CParameters * parameter);
	~SummaryRunwayDelayResult(void);

public:

	void InitListHead(CXListCtrl& cxListCtrl, CSortableHeaderCtrl* piSHC=NULL);

	void FillListContent(CXListCtrl& cxListCtrl);
	//refresh chart
	void RefreshReport(CParameters * parameter );
	virtual BOOL ReadReportData(ArctermFile& _file);
protected:

	void GetListColumns(std::vector<CString>& lstColumns);
	void StatisticSummaryDataFromReportData();

private:
	std::vector<CString> m_lstColumns;
	std::vector<AirsideFlightRunwayDelaySummaryData*> m_vSummaryData;
};
