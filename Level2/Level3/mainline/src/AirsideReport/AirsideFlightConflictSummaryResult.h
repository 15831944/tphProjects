#pragma once
#include "../Common/elaptime.h"
#include <vector>
#include <map>
#include "AirsideFlightConflictBaseResult.h"


class FlightConflictReportData;
class FlightConflictSummaryData;
class AIRSIDEREPORT_API AirsideFlightConflictSummaryResult: public AirsideFlightConflictBaseResult
{
public:
	AirsideFlightConflictSummaryResult(CParameters * parameter);
	~AirsideFlightConflictSummaryResult(void);
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

	std::vector<FlightConflictSummaryData*> m_vSummaryData;
};
