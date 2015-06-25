#pragma once
#include "FlightConflictReportData.h"

class FlightConflictSummaryData;
class ReportDataFilter
{
public:
	ReportDataFilter(long lStart, long lEnd, int nConType, int nAreaID, int nLocType, int nOpType, int nActType);
	~ReportDataFilter();

	bool IsFit(FlightConflictReportData::ConflictDataItem* pItem);

public:
	long m_lTimeStart;
	long m_lTimeEnd;

	int m_nConflictType;
	int m_nAreaID;
	int m_nLocationType;
	int m_nOperationType;
	int m_nActionType;	
};

class SummaryDataFilter
{
	friend class AirsideFlightConflictPara;
public:
	SummaryDataFilter( int nConType, CString strArea, int nLocType, int nOpType, int nActType);
	~SummaryDataFilter();

	bool IsFit(FlightConflictSummaryData* pData);

private:
	int m_nConflictType;
	CString m_strArea;
	int m_nLocationType;
	int m_nOperationType;
	int m_nActionType;	
};
