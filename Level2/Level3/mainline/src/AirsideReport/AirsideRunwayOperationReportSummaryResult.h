#pragma once
#include "airsiderunwayoperationreportbaseresult.h"
#include <vector>
#include <map>
#include "../Results/AirsideFlightLog.h"
#include "../Results/AirsideFlightLogEntry.h"
#include "../Results/OutputAirside.h"
#include "../common/FLT_CNST.H"
#include "../Results/AirsideRunwayOperationLog.h"
#include "../Engine\Airside\AircraftClassificationManager.h"
#include "AirsideReportBaseResult.h"
#include "AirsideRunwayOperationsReport.h"

class CAirsideReportBaseResult;
class CAirsideRunwayOperationReportSummaryResult :
	public CAirsideRunwayOperationReportBaseResult
{
public:
	class StatisticsSummaryItem
	{
	public:
		StatisticsSummaryItem()
		{
			//min
			m_minCount = 0;
			//the time range that the min operations take place
			m_strMinInterval = _T("");

			m_nAverageCount = 0;

			m_maxCount = 0;
			m_strMaxInterval = _T("");

			//quarter
			m_nQ1 = 0;
			m_nQ2 = 0;
			m_nQ3 = 0;

			//percentage
			m_nP1 = 0;
			m_nP5 = 0;
			m_nP10 = 0;
			m_nP90 = 0;
			m_nP95 = 0;
			m_nP99 = 0;
			m_nStdDev = 0;
		}
		~StatisticsSummaryItem()
		{

		}

	public:
		//min
		int m_minCount;
		//the time range that the min operations take place
		CString m_strMinInterval;

		int m_nAverageCount;

		int m_maxCount;
		CString m_strMaxInterval;

		//quarter
		int m_nQ1;
		int m_nQ2;
		int m_nQ3;

		//percentage
		int m_nP1;
		int m_nP5;
		int m_nP10;
		int m_nP90;
		int m_nP95;
		int m_nP99;
		int m_nStdDev;
	};


	//time count
	//9:00  20
	class RunwayOperationTimeCount
	{
	public:
		
		RunwayOperationTimeCount()
		{
			m_eTime = ElapsedTime(0L);
			m_nOperationCount = 0;
			m_nLandingCount = 0;
			m_nTakeOffCount = 0;

		}
		~RunwayOperationTimeCount()
		{

		}

	public:
		ElapsedTime m_eTime;
		int			m_nOperationCount;
		int			m_nLandingCount;
		int			m_nTakeOffCount;

	protected:
	};
	//one runways' all time count
	class RunwayOperationStatisticItem
	{
	public:
		RunwayOperationStatisticItem()
		{

		}
		~RunwayOperationStatisticItem()
		{

		}
	public:
		void Init(ElapsedTime eStartTim, ElapsedTime eEndTime, ElapsedTime eInterval);


	public:
		CAirsideReportRunwayMark m_runWaymark;
		std::vector<RunwayOperationTimeCount> m_vRunwayStatistic;

		StatisticsSummaryItem m_landingStastic;
		StatisticsSummaryItem m_TakeOffStastic;
		StatisticsSummaryItem m_MovementStastic;
	};
public:
	CAirsideRunwayOperationReportSummaryResult(void);
	~CAirsideRunwayOperationReportSummaryResult(void);

public:
	void InitListHead(CXListCtrl& cxListCtrl, CSortableHeaderCtrl* piSHC=NULL);

	void FillListContent(CXListCtrl& cxListCtrl, CParameters * parameter);

	void RefreshReport(CParameters * parameter);
	//if the report load from file , here is the columns 
	std::vector<CString> m_lstColumns;
	void GetListColumns(std::vector<CString>& lstColumns);

protected:
	virtual BOOL WriteReportData(ArctermFile& _file);
	virtual BOOL ReadReportData(ArctermFile& _file);

public:
	void GenerateResult(CParameters *pParameter);
	//this function only called in GenerateResult
	//it will generate statistic values
	void StatisticData(AirsideRunwayOperationsReport::ChartType enumChartType);

	void Draw3DChart();

public:
	std::vector<RunwayOperationStatisticItem *> m_vOperationStatistic;
	void StatisticAllRunwayData();
	void StatisticData(CAirsideRunwayOperationReportSummaryResult::RunwayOperationStatisticItem *pAllRunwayStastic,
		AirsideRunwayOperationsReport::ChartType enumChartType,
		CAirsideRunwayOperationReportSummaryResult::StatisticsSummaryItem& statisticItem);

protected:
	
	CParameters *m_pParameter;

	void InitRunway( CParameters * parameter );
	bool fits(CParameters * parameter,const AirsideFlightDescStruct& fltDesc);
	RunwayOperationStatisticItem* GetRunwayStastic( CAirsideReportRunwayMark& runwayMark );

	CString FormatIntToString(int nValue);

};

namespace AirsideRunwayOperationReportSummary
{


	class RunwayOperationalStatisticsChartResult :public CAirsideReportBaseResult
	{
	public:
		RunwayOperationalStatisticsChartResult(AirsideRunwayOperationsReport::ChartType chartType);
		~RunwayOperationalStatisticsChartResult();		

	public:
		void GenerateResult(std::vector<CAirsideRunwayOperationReportSummaryResult::RunwayOperationStatisticItem *>* vRunwayOperationStastic);
		virtual void Draw3DChart(CARC3DChart& chartWnd, CParameters *pParameter);
	protected:
		std::vector<CAirsideRunwayOperationReportSummaryResult::RunwayOperationStatisticItem *> * m_vRunwayOperationStastic;

	protected:
		AirsideRunwayOperationsReport::ChartType m_enumChartType;


	};
}
