#pragma once
#include "Common/elaptime.h"

namespace MultiRunsReport
{
	namespace Summary
	{
		template<typename DataType>
		class MultiRunsReportDataLoader
		{
		public:
			
			typedef std::vector<DataType> DataList;

			MultiRunsReportDataLoader(LPCTSTR lpszFile)
			: m_strFileName(lpszFile)
			{
			}

			void LoadData();
			DataList& GetData() { return m_dataList; }
			//DataType& GetData() { return m_data; }
		protected:
			CString m_strFileName;
			DataList m_dataList;
			//DataType m_data;


		};

		struct PaxDataValue
		{
			float fMinValue;
			float fAveValue;
			float fMaxValue;

			PaxDataValue()
			: fMinValue(.0f)
			, fAveValue(.0f)
			, fMaxValue(.0f)
			{}
		};
		struct SummaryQueueTimeValue
		{
			CString strPaxType;
			ElapsedTime eMinimum;
			ElapsedTime eAverage;
			ElapsedTime eMaximum;
			int			nCount;
			ElapsedTime eQ1;
			ElapsedTime eQ2;
			ElapsedTime eQ3;
			ElapsedTime eP1;
			ElapsedTime eP5;
			ElapsedTime eP10;
			ElapsedTime eP90;
			ElapsedTime eP95;
			ElapsedTime eP99;
			ElapsedTime eSigma;

			SummaryQueueTimeValue()
				:strPaxType(_T("DEFAULT"))
				,eMinimum(0L)
				,eAverage(0L)
				,eMaximum(0L)
				,nCount(0)
				,eQ1(0L)
				,eQ2(0L)
				,eQ3(0L)
				,eP1(0L)
				,eP5(0L)
				,eP10(0L)
				,eP90(0L)
				,eP95(0L)
				,eP99(0L)
				,eSigma(0L)
			{
			}

		};
		//time in service, summary report data
		struct SummaryTimeServiceValue
		{
			CString strPaxType;
			ElapsedTime eMinimum;
			ElapsedTime eAverage;
			ElapsedTime eMaximum;
			int			nCount;
			ElapsedTime eQ1;
			ElapsedTime eQ2;
			ElapsedTime eQ3;
			ElapsedTime eP1;
			ElapsedTime eP5;
			ElapsedTime eP10;
			ElapsedTime eP90;
			ElapsedTime eP95;
			ElapsedTime eP99;
			ElapsedTime eSigma;

			SummaryTimeServiceValue()
				:strPaxType(_T("DEFAULT"))
				,eMinimum(0L)
				,eAverage(0L)
				,eMaximum(0L)
				,nCount(0)
				,eQ1(0L)
				,eQ2(0L)
				,eQ3(0L)
				,eP1(0L)
				,eP5(0L)
				,eP10(0L)
				,eP90(0L)
				,eP95(0L)
				,eP99(0L)
				,eSigma(0L)
			{
			}

		};
		struct SummaryCriticalQueueValue
		{	
			CString strProcName;
			int nGroupSize;
			float fAvgQueue;
			int nMaxQueue;
			ElapsedTime eMaxQueueTime;

			
			SummaryCriticalQueueValue()
				:nGroupSize(0)
				,fAvgQueue(0.0f)
				,nMaxQueue(0)
				,eMaxQueueTime(0L)
			{
			}


		};
		struct PaxData
		{
			CString strPaxType;
			PaxDataValue value;
		};

		typedef std::map<CString, std::vector<PaxDataValue> > PaxDataList;

		struct ActivityBreakdownData
		{
		};

		struct OccupancyData
		{

		};
		typedef MultiRunsReportDataLoader<SummaryQueueTimeValue> SummaryQTimeLoader;
		typedef MultiRunsReportDataLoader<SummaryCriticalQueueValue> SummaryCriticalTimeLoader;
		typedef MultiRunsReportDataLoader<SummaryTimeServiceValue> SummaryTimeServiceLoader;

		typedef MultiRunsReportDataLoader<PaxData> DistTravelledDataLoader;
		typedef MultiRunsReportDataLoader<PaxData> TimeInQueueDataLoader;
		typedef MultiRunsReportDataLoader<PaxData> TimeInTerminalDataLoader;
		typedef MultiRunsReportDataLoader<PaxData> TimeInServiceDataLoader;
		typedef MultiRunsReportDataLoader<ActivityBreakdownData> ActivityBreakdownDataLoader;
		typedef MultiRunsReportDataLoader<OccupancyData> OccupancyDataLoader;


		typedef std::vector<std::vector<SummaryQueueTimeValue> > SummaryQTimeList;
		typedef std::vector<std::vector<SummaryCriticalQueueValue> >SummaryCriticalQueueList;

		//time in service, summary report data list
		typedef std::vector<std::vector<SummaryTimeServiceValue> > SummaryTimeServiceList;

	}

	namespace Detail
	{
		template<typename DataType>
		class MultiRunsReportDataLoader
		{
		public:
			typedef std::vector<DataType> DataList;

			MultiRunsReportDataLoader(LPCTSTR lpszFile,long Interminal = 0)
			: m_strFileName(lpszFile) ,m_interval(Interminal) 
			{
			}
			virtual ~MultiRunsReportDataLoader() {} ;
			virtual void LoadData();
			DataList& GetData() { return m_dataList; }
			int GetRangeCountOnX(){ return m_nRangeCountOnX; }

			// input: minTime, maxTime, minNumCol
			// output: define all RangeData with count = 0
			void DecideRangeOnX( ElapsedTime _minTime, ElapsedTime _maxTime, int _nMinNumCol );

		protected:
			CString m_strFileName;
			long m_interval ;

			DataList m_dataList;
			int m_nRangeCountOnX;
		};

		struct RangeCount
		{
			std::pair<int, int> ValueRange;
			int nCount;

			RangeCount()
			 : ValueRange(0, 0)
			 , nCount(0)
			{}

			bool Fit(float value)
			{
				return (value >= ValueRange.first && value < ValueRange.second);
			}
		};

		struct TimeRangeCount
		{
			std::pair<ElapsedTime, ElapsedTime> TimeValueRange;
			int nCount;
			bool bInSecond;

			TimeRangeCount()
				: nCount(0), bInSecond( false )
			{}

			bool Fit(ElapsedTime eTime)
			{
				return (eTime >= TimeValueRange.first && eTime < TimeValueRange.second);
			}
		};	
		struct PaxDensityRangerCount : TimeRangeCount
		{

		};
		struct ActivityBreakdownData
		{
		};

		struct OccupancyData
		{
			ElapsedTime startTime;
			int nPaxCount;
		};
		class CPaxDenistyDataLoader : public MultiRunsReportDataLoader<OccupancyData>
		{
		public:
			CPaxDenistyDataLoader(LPCTSTR lpszFile,long Interminal = 0)
				: MultiRunsReportDataLoader<OccupancyData>(lpszFile,Interminal) 
			{
			}
			virtual ~CPaxDenistyDataLoader() {} ;
			void LoadData();
		} ;

		// all simulation result data list
		typedef std::vector<std::vector<RangeCount> > PaxDataList;

		// all simulation result data list
		typedef std::vector<std::vector<TimeRangeCount> > TimeDataList;
		typedef std::vector<std::vector<OccupancyData> > OccupnacyDataList;


		typedef MultiRunsReportDataLoader<RangeCount> DistTravelledDataLoader;
		typedef MultiRunsReportDataLoader<TimeRangeCount> TimeInQueueDataLoader;
		typedef MultiRunsReportDataLoader<TimeRangeCount> TimeInTerminalDataLoader;
		typedef MultiRunsReportDataLoader<TimeRangeCount> TimeInServiceDataLoader;
		typedef MultiRunsReportDataLoader<ActivityBreakdownData> ActivityBreakdownDataLoader;
		typedef MultiRunsReportDataLoader<OccupancyData> OccupancyDataLoader;

	}
}
