#pragma once
#include "airsiderunwayutilizationreportbaseresult.h"
#include "AirsideReportBaseResult.h"
namespace AirsideRunwayUtilizationReport
{	
	//detail report result
	class AIRSIDEREPORT_API CDetailResult :
		public CAirsideRunwayUtilizationReportBaseResult
	{

	public:
		CDetailResult(void);
		~CDetailResult(void);

	public:
		void InitListHead(CXListCtrl& cxListCtrl, CSortableHeaderCtrl* piSHC=NULL);

		void FillListContent(CXListCtrl& cxListCtrl, CParameters * parameter);
		//refresh chart
		void RefreshReport(CParameters * parameter);

	public:
		void Draw3DChart();

		//file operations, save and read data from file
	public:
		virtual BOOL WriteReportData(ArctermFile& _file);
		virtual BOOL ReadReportData(ArctermFile& _file);
	};
	
	//for chart CDetailRunwayOccupancyChart, means one runway's landing and take off information
	class AIRSIDEREPORT_API CDetailRunwayOccupancyItem
	{
	public:
		//runway information
		CAirsideReportRunwayMark runwayMark;

		//landing
		std::vector<DetailFlightRunwayResult *> m_vLanding;
		//take off
		std::vector<DetailFlightRunwayResult *> m_vTakeOff;
	public:
		//get the operation count in the range from eOccTimeFrom to  eOccTimeTo
		void GetOperationCount(ElapsedTime eOccTimeFrom, ElapsedTime eOccTimeTo, int& nLandingCount, int& nTakeOffCount);

	};
    //detail report, runway occupancy chart data
	class AIRSIDEREPORT_API CDetailRunwayOccupancyChart: public CAirsideReportBaseResult
	{
	public:
		CDetailRunwayOccupancyChart();
		~CDetailRunwayOccupancyChart();

	public:
		void GenerateResult(std::vector<DetailFlightRunwayResult *>& vDetailResult,std::vector<CAirsideReportRunwayMark>& vRunwayParamMark);
		virtual void Draw3DChart(CARC3DChart& chartWnd, CParameters *pParameter);

	public:
		std::vector<CDetailRunwayOccupancyItem *> m_vRunwayOccu;

	protected:
		void InitResult(std::vector<CAirsideReportRunwayMark>& vRunwayParamMark);

		CDetailRunwayOccupancyItem *GetRunwayResult(const CAirsideReportRunwayMark& runwayMark);

		void GetMinMaxOccupancyTime(ElapsedTime& eMinTime, ElapsedTime& eMaxTime);
	};

	//detail runway utilization report chart data
	class AIRSIDEREPORT_API CDetailRunwayUtilizationChart: public CAirsideReportBaseResult
	{
	public:
		CDetailRunwayUtilizationChart();
		~CDetailRunwayUtilizationChart();

	public:
		void GenerateResult(std::vector<DetailFlightRunwayResult *>& vDetailResult,bool bParameterIsDefault);
		virtual void Draw3DChart(CARC3DChart& chartWnd, CParameters *pParameter);

		void GetOccupyTimeBetween(ElapsedTime eTimeStart, ElapsedTime eTimeEnd, ElapsedTime& eTimeOccupied);
		int GetActiveRunwayCount();
	public:
		std::vector<DetailFlightRunwayResult *> *m_pvDetailResult;
	
	protected:
		//from parameter, if the parameter is not default, show "Base on partial data"
		bool m_bParameterIsDefault;



	};

}

