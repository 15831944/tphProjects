#pragma once
#include "airsiderunwayutilizationreportbaseresult.h"
#include "AirsideReportBaseResult.h"



namespace AirsideRunwayUtilizationReport
{
	class CSummaryIntervalValue
	{
	public:
		CSummaryIntervalValue()
		{
			m_dUtilziation = 0.0;
		}
		~CSummaryIntervalValue()
		{

		}

	public:
		void StatisticResult(ElapsedTime eTimeInterval);

		//read write data from file
		virtual BOOL WriteReportData(ArctermFile& _file);
		virtual BOOL ReadReportData(ArctermFile& _file);
	public:

		//time interval
		ElapsedTime m_eTimeStart;
		ElapsedTime m_eTimeEnd;

		//the operation in the interval
		std::vector<DetailFlightRunwayResult *> m_vOccupancyInfo;

		//occupy time

		//min
		ElapsedTime m_eMin;
		
		ElapsedTime m_eAverage;

		ElapsedTime m_eMax;

		//quarter
		ElapsedTime m_eQ1;
		ElapsedTime m_eQ2;
		ElapsedTime m_eQ3;

		//percentage
		ElapsedTime m_eP1;
		ElapsedTime m_eP5;
		ElapsedTime m_eP10;
		ElapsedTime m_eP90;
		ElapsedTime m_eP95;
		ElapsedTime m_eP99;
		ElapsedTime m_eStdDev;

		ElapsedTime m_eTotal;

		//utilization of the interval,percentage, *100.0 has been 
		double m_dUtilziation;


	};



	class CSummaryResult :
		public CAirsideRunwayUtilizationReportBaseResult
	{
	public:
		CSummaryResult(void);
		~CSummaryResult(void);
	
	public:
		virtual void InitListHead( CXListCtrl& cxListCtrl, CSortableHeaderCtrl* piSHC=NULL );

		virtual void FillListContent( CXListCtrl& cxListCtrl, CParameters * parameter );

		virtual void RefreshReport(CParameters * parameter);


		//read write data from file
		virtual BOOL WriteReportData(ArctermFile& _file);
		virtual BOOL ReadReportData(ArctermFile& _file);

		virtual void GenerateResult(CParameters *pParameter);


	public:
		std::vector<CSummaryIntervalValue *> m_vIntervalResult;

		//time interval, user defined
		ElapsedTime m_eTimeInterval;

	protected:
		void StatisticIntervalResult();
		void Clear();

	};
	
	class CSummaryRunwayOccupancyChart :public CAirsideReportBaseResult
	{
	public:
		CSummaryRunwayOccupancyChart();
		~CSummaryRunwayOccupancyChart();


	public:
		void GenerateResult(std::vector<CSummaryIntervalValue *>& vIntervalResult);

		virtual void Draw3DChart(CARC3DChart& chartWnd, CParameters *pParameter);

	public:
		std::vector<CSummaryIntervalValue *>* m_pvIntervalResult;
	};

	class CSummaryRunwayUtilizationChart: public CAirsideReportBaseResult
	{
		public:
			CSummaryRunwayUtilizationChart();
			~CSummaryRunwayUtilizationChart();

		public:
			void GenerateResult(std::vector<CSummaryIntervalValue *>& vIntervalResult);
			virtual void Draw3DChart(CARC3DChart& chartWnd, CParameters *pParameter);
		
	public:
			std::vector<CSummaryIntervalValue *>* m_pvIntervalResult;
			

	};



};

