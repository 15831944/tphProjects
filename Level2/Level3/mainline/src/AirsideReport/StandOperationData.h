#pragma once
#include "../Reports/StatisticalTools.h"
#include "AirsideReportSummaryData.h"
#include "../Common/ALTObjectID.h"
#include "../Reports/StatisticalTools.h"

class ArctermFile;

///stand operation data for detail and summary base class/////////////////////////////////////////////
class AIRSIDEREPORT_API CStandOperationReportData
{
public:
	CStandOperationReportData();
	~CStandOperationReportData();

public:
	CString m_sID;//airline + ID
	long m_lFlightIndex;
	CString m_sACType;
	char m_fltmode;

	CString m_sSchedName;
	long m_lSchedOn;
	long m_lSchedOff;
	long m_lSchedOccupancy;
	long m_lSchedAvailableOccupancy;

	CString m_sActualName;
	long m_lActualOn;
	long m_lActualOff;
	long m_lActualOccupancy;
	long m_lActualAvailableOccupancy;

	long m_lDelayEnter;
	long m_lDueStandOccupied;

	long m_lDelayLeaving;
	long m_lDueTaxiwayOccupied;
	long m_lDueGSE;
	long m_lDuePushbackClearance;

	bool IsConflict();
	bool IsDelay();



};

//////////////////////////////////////////////////////////////////////////////////////////
class CSummaryStandOperationData
{
public:
	CSummaryStandOperationData();
	virtual ~CSummaryStandOperationData();

	enum StandSummaryDataType
	{
		SchedStandUtilizeSummaryData =0,
		SchedStandIdleSummaryData,
		ActualStandUtilizeSummaryData,
		ActualStandIdleSummaryData,
		StandDelaySummaryData,
		StandConflictSummaryData
	};

	CString m_sName;//stand family that user select
	StandSummaryDataType m_eDataType;
	double m_dTotal;
	CStatisticalTools<double> m_SummaryData;

};
