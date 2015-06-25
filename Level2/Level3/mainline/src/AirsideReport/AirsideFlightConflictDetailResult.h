#pragma once
#include "../Common/elaptime.h"
#include <vector>
#include "AirsideFlightConflictBaseResult.h"

class AirsideFlightConflictPara;
class FlightConflictReportData;
class AIRSIDEREPORT_API AirsideFlightConflictDetailResult: public AirsideFlightConflictBaseResult
{
public:
	AirsideFlightConflictDetailResult(CParameters * parameter);
	~AirsideFlightConflictDetailResult(void);
public:

	void InitListHead(CXListCtrl& cxListCtrl, CSortableHeaderCtrl* piSHC=NULL);

	void FillListContent(CXListCtrl& cxListCtrl);
	//refresh chart
	void RefreshReport(CParameters * parameter );

protected:
	void GetListColumns(std::vector<CString>& lstColumns);

private:
	std::vector<CString> m_lstColumns;
	AirsideFlightConflictPara* m_pParameter;
};
