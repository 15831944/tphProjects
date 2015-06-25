#pragma once
#include "../Common/elaptime.h"
#include <vector>
#include "AirsideFlightRunwayDelayBaseResult.h"

class AirsideFlightRunwayDelayData;
class AirsideFlightRunwayDelayReportPara;
class AIRSIDEREPORT_API DetailRunwayDelayResult: public AirsideFlightRunwayDelayBaseResult
{
public:
	DetailRunwayDelayResult(CParameters * parameter);
	~DetailRunwayDelayResult(void);

public:
	void InitListHead(CXListCtrl& cxListCtrl, CSortableHeaderCtrl* piSHC=NULL);
	void FillListContent(CXListCtrl& cxListCtrl);
	//refresh chart
	void RefreshReport(CParameters * parameter );

protected:
	void GetListColumns(std::vector<CString>& lstColumns);

private:
	std::vector<CString> m_lstColumns;
	AirsideFlightRunwayDelayReportPara* m_pParameter;
};
