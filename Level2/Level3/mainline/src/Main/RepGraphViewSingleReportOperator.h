#pragma once
#include "repgraphviewbaseoperator.h"
#include "reports\ReportType.h"
#include "common/csv_file.h"
#include "common/termfile.h"
#include "MathResultManager.h"
#include "ARCReportManager.h"
class CRepGraphViewSingleReportOperator :
	public CRepGraphViewBaseOperator
{
public:
	CRepGraphViewSingleReportOperator(CRepGraphView *pGraphView);
	~CRepGraphViewSingleReportOperator(void);


public:
	virtual void LoadReport(LPARAM lHint, CObject* pHint);



protected:
	void LoadGrapgh();



protected:
	CString GetAbsDateTime(long seconds, BOOL needsec);
	CString NoUnderline(CString _str);
	void SpaceThroughputCount();
	void PaxDensityM2Pax();
	void PaxDensityPaxM2();
	void PaxDensityCount();
	void BagCountMinSummary();
	void BagCountCombinedSummary();
	void BagCountMaxSummary();
	void BagCountAvgSummary();
	void BagCountDetailed();
	void BagWaitTimeSummary();
	void BagWaitTimeDetailed();
	void ActBKDownPerBKDetailed();
	void ActBKDownTimeBKDetailed();
	void ActBKDownPerBKSummary();
	void ActBKDownTimeBKSummary();
	void ServiceTimeSummary();
	void ServiceTimeDetailed();
	void DurationSummary();
	void DurationDetailed();
	void AvgQueueLenMaxQueueTimeHisDetailed();
	void AvgQueueLenTimeMaxQueDetailed();
	void AvgQueueLenDetailed();
	void AvgQueueLenSummary();
	void AvgQueueLenTimeMaxQueSummary();
	void AvgQueueLenMaxQueTimeHisSummary();

	void QueueLengthCombinedSummary();
	void QueueLengthMaxSummary();
	void QueueLengthAvgSummary();
	void QueueLengthMinSummary();
	void QueueLengthDetailed();

	void ConveyorWaitLengthDetailed();
	void ConveyorWaitLengthMaxSummary();
	void ConveyorWaitLengthAvgSummary();
	void ConveyorWaitLengthMinSummary();
	void ConveyorWaitLengthCombinedSummary();

	void ThroughputProcPerHourSummary();
	void ThroughputGroupPerHourSummary();
	void ThroughputPaxPerProcSummary();
	void ThroughputTotalPaxSummary();
	void ThroughputAveTimeDetailed();
	void ThroughputAvePaxDetailed();
	void ThroughputTotalPaxDetailed();

	void QueueTimeSummary();
	void QueueTimeDetailed();

	void ConveyorWatiTimeSummary();
	void ConveyorWatiTimeDetailed();

	void InitToolbar();
	void PaxCountTimeRange();
	void PaxCountTotalPaxCount();
	void PaxCountMaxPaxCount();
	void PaxCountPaxPeriod();
	void PaxCountDetailed();
	void CollisionCount();
	void UtilizationPercentageSummary();
	void UtilizationTimeAvailableSummary();
	void UtilizationTimeSummary();
	void UtilizationPercentageDetailed();
	void UtilizationTimeAvailableDetailed();
	void UtilizationTimeDetailed();
	void DistanceTravelledDetailed();
	void DistanceTravelledSummary();
	void PaxLogCartCountDistribution();
	void PaxLogBagCountDistribution();
	void PaxLogGroupSizeDistribution();
	CString FormatTime(int nMinute);
	void PaxLogSysExitDistribution();
	void PaxLogSysEntryDistribution();
	void SetHeadXYTitle(CString strTitle, CString strXAxisTitle, CString strYAxisTitle,CString strFootnote);
	void AcOpArrivalCount();
	void AcOpDepartCount();
	void AcOpTotal();
	void BagDepartDistCount();
	void BagArrivalDistCount();
	void BagDeliveryCount();
	void BagDeliverySummary();
	void FireEvacuationDetailed();
	void FireEvacuationSummary();
	void MissFlightDetailed();
	void BillboardExposureIncidenceDetailed();
	void BillboardExposureIncidenceSummary();

	void BillboardLinkedProcIncrementalVisitDetailed();
	void BillboardLinkedProcIncrementalVisitSummary();

	//--------------------------------------------------------------------
	//Summary:
	//		Draw graph from file data
	//--------------------------------------------------------------------
	void RetailRevenuesDetailed();
	void RetailPenetrationDetailed();
	void RetailLostSalesDetailed();
	void RetailFailedPenetrationDetailed();

	void RetailRevenuesSummary();
	//---------------------------------------------------------------------

	int m_nSummary;
	int GetInterval(char *time);
	void getTime( const CString& strText, ElapsedTime& begineTime, ElapsedTime& endTime ) const;


	void DealRowLabel(CString &strRowLabel);

	void ShowReportProcessFlow(MathResult& result);
	void LoadReportProcessFlow(CMathResultManager *pManager, enum ENUM_REPORT_TYPE _enumRepType);


	LoadReportType GetLoadReportType(void) ;
	const CString& GetUserLoadReportFile( void );
	virtual void OnSelchangeChartSelectCombo();
private:
	int m_nReportFileType;

	ArctermFile chartFile;
};
