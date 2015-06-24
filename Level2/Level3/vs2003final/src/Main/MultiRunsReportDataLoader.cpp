#include "StdAfx.h"
#include "MultiRunsReportDataLoader.h"
#include "../common/UnitsManager.h"
#include "Common/Termfile.h"
#include <limits>
namespace MultiRunsReport
{
	namespace Summary
	{
		void DistTravelledDataLoader::LoadData()
		{
			//m_dataList.clear();

			ArctermFile file;
			file.init(m_strFileName, READ);
			file.reset();
			file.skipLines(3);

			char buf[128] = { 0 };

			while (file.getLine())
			{
				PaxData data;

				file.setToField(0);
				file.getField(buf, 127);
				data.strPaxType = buf;

				file.setToField(1);
				file.getFloat(data.value.fMinValue);

				file.setToField(2);
				file.getFloat(data.value.fAveValue);

				file.setToField(3);
				file.getFloat(data.value.fMaxValue);

				//m_dataList.push_back(data);
			}
		}

		void ActivityBreakdownDataLoader::LoadData()
		{
			//m_dataList.clear();
		}

		void OccupancyDataLoader::LoadData()
		{
			//m_dataList.clear();
		}

		void SummaryQTimeLoader::LoadData()
		{
			ArctermFile file;
			file.init(m_strFileName, READ);
			file.reset();
			file.skipLines(3);

			char buf[128] = { 0 };

			while (file.getLine())
			{
				SummaryQueueTimeValue data;

				file.setToField(0);
				file.getField(buf, 127);
				data.strPaxType = buf;

				file.getTime(data.eMinimum, TRUE);
				file.getTime (data.eAverage, TRUE);
				file.getTime (data.eMaximum, TRUE);
				file.getInteger(data.nCount);

				file.getTime(data.eQ1 , TRUE );//Q1
				file.getTime(data.eQ2 , TRUE );//Q2
				file.getTime(data.eQ3 , TRUE );//Q3

				file.getTime(data.eP1 , TRUE );//P1
				file.getTime(data.eP5 , TRUE );//P5
				file.getTime(data.eP10 ,TRUE );//P10

				file.getTime(data.eP90 , TRUE );//P90
				file.getTime(data.eP95 , TRUE );//P95
				file.getTime(data.eP99 , TRUE );//P99

				file.getTime( data.eSigma, TRUE );//Sigma

				m_dataList.push_back(data);
			}

		}

		void SummaryTimeServiceLoader::LoadData()
		{
			ArctermFile file;
			file.init(m_strFileName, READ);
			file.reset();
			file.skipLines(3);

			char buf[128] = { 0 };

			while (file.getLine())
			{
				SummaryTimeServiceValue data;

				file.setToField(0);
				file.getField(buf, 127);
				data.strPaxType = buf;

				file.getTime(data.eMinimum, TRUE);
				file.getTime (data.eAverage, TRUE);
				file.getTime (data.eMaximum, TRUE);
				file.getInteger(data.nCount);

				file.getTime(data.eQ1 , TRUE );//Q1
				file.getTime(data.eQ2 , TRUE );//Q2
				file.getTime(data.eQ3 , TRUE );//Q3

				file.getTime(data.eP1 , TRUE );//P1
				file.getTime(data.eP5 , TRUE );//P5
				file.getTime(data.eP10 ,TRUE );//P10

				file.getTime(data.eP90 , TRUE );//P90
				file.getTime(data.eP95 , TRUE );//P95
				file.getTime(data.eP99 , TRUE );//P99

				file.getTime( data.eSigma, TRUE );//Sigma

				m_dataList.push_back(data);
			}

		}

		void SummaryCriticalTimeLoader::LoadData()
		{
			ArctermFile file;
			file.init(m_strFileName, READ);
			file.reset();
			file.skipLines(3);

			char buf[128] = { 0 };

			while (file.getLine())
			{
				SummaryCriticalQueueValue data;
				char buf[512]= {0};
				file.getField(buf,512);
				data.strProcName = buf;
				file.getInteger(data.nGroupSize);
				file.getFloat(data.fAvgQueue);
				file.getInteger(data.nMaxQueue);
				file.getTime(data.eMaxQueueTime);

				m_dataList.push_back(data);
			}
		}
	}

	namespace Detail
	{
		void MultiRunsReportDataLoader<RangeCount>::LoadData()
		{
			m_dataList.clear();
			m_dataList.resize(11);

			ArctermFile file;
			file.init(m_strFileName, READ);
			file.reset();
			file.skipLines(3);

			// find the max value
			int maxXValue = 0;
			int minXValue = (std::numeric_limits<int>::max)();
			float ft = .0f;

			while (file.getLine())
			{
				file.setToField(1);
				file.getFloat(ft);
				if (ft > maxXValue)
					maxXValue = static_cast<int>(ft);

				if (ft < minXValue)
				{
					minXValue = static_cast<int>(ft);

				}
			}
			
			// calculate the scale
			//maxXValue = maxXValue - (maxXValue % 50) + 50;
			//int nScale = 0;
			//int n = maxXValue % 50; // 7 Column
			//if (n == 0)
			//{
			//	nScale = maxXValue / 50;
			//}
			//else
			//{
			//	nScale = maxXValue / 50 + 1;
			//}

			//m_dataList[0].ValueRange.first = 0;
			//for (int i = 0; i < 10; i++)
			//{
			//	m_dataList[i + 1].ValueRange.first = m_dataList[i].ValueRange.second = m_dataList[i].ValueRange.first + 50;
			//}
			
			//according to the min and max value, calculate the scale
			
				//calculate the min value
			if (maxXValue <=minXValue )
				return;

				int nStepValue = 10;
				int nDiff = maxXValue - minXValue;
				while (nDiff/nStepValue > 8)
				{
					nStepValue +=10;
				}


				int nNumRangeCount = nDiff/nStepValue + 2 ;
				
				int nStartValue =0;
				//0
				m_dataList.resize(nNumRangeCount);
				{
					//
					if(minXValue/nStepValue <0)
					{

					}
					else
					{
						nStartValue = minXValue - minXValue%nStepValue;
					}
				}

				for (int i =0; i < nNumRangeCount; ++i)
				{
					m_dataList[i].ValueRange.first = nStartValue;
					m_dataList[i].ValueRange.second = nStartValue + nStepValue;

					nStartValue += nStepValue;
				}

			file.reset();
			file.skipLines(3);
			while (file.getLine())
			{
				file.setToField (1);
				file.getFloat(ft);
				for(int nItem = 0; nItem < nNumRangeCount; nItem++)
				{
					if(m_dataList[nItem].Fit(ft))
					{
						m_dataList[nItem].nCount++;
					}
				}
			}
		}

		// input: minTime, maxTime, minNumCol
		// output: define all RangeData with count = 0
		void TimeInTerminalDataLoader::DecideRangeOnX( ElapsedTime _minTime, ElapsedTime _maxTime, int _nMinNumCol )
		{
			ElapsedTime timeDiff = _maxTime - _minTime;

			ElapsedTime eUnitTimeOnX;

			int nTimeDiffAsHours = timeDiff.asHours();
			int nTimeDiffAsMin = timeDiff.asMinutes();
			int nTimeDiffAsSec = timeDiff.asSeconds();

			bool bInSecond = false;
			if( nTimeDiffAsHours > _nMinNumCol )
			{
				// if hh  > _nMinNumCol, display column up to 2*_nMinNumCol
				int nMinHours = nTimeDiffAsHours / (2*_nMinNumCol) + 1;
				eUnitTimeOnX.set( nMinHours, 0, 0 );
				_minTime.SetMinute( 0 );
				_minTime.SetSecond( 0 );
				if( nTimeDiffAsHours > (2*_nMinNumCol) )
					_nMinNumCol = (2*_nMinNumCol);
				else
					_nMinNumCol = nTimeDiffAsHours;
			}
			else if( nTimeDiffAsMin > _nMinNumCol )
			{
				// else if hhmm > nNumCol min, so the column unit should be hhmm % 2 nNumCol min
				int nMinMinutes = nTimeDiffAsMin / ( 2*_nMinNumCol ) + 1;
				eUnitTimeOnX.set( nMinMinutes / 60, nMinMinutes % 60, 0 );
				_minTime.SetSecond( 0 );
				if( nTimeDiffAsMin > 2 * _nMinNumCol )
					_nMinNumCol = 2 * _nMinNumCol;
				else
					_nMinNumCol = nTimeDiffAsMin;
			}
			else if( nTimeDiffAsSec > _nMinNumCol )
			{
				// be hhmmss % 2 nNumCol sec.
				int mMinSeconds = nTimeDiffAsSec / ( 2*_nMinNumCol ) + 1;
				eUnitTimeOnX.set( mMinSeconds / 3600, mMinSeconds / 60, mMinSeconds % 60 );
				if( nTimeDiffAsSec > 2 * _nMinNumCol )
					_nMinNumCol = 2 * _nMinNumCol;
				else
					_nMinNumCol = nTimeDiffAsSec;
				bInSecond = true;
			}

			m_dataList.resize( _nMinNumCol );

			m_nRangeCountOnX = _nMinNumCol;
			m_dataList[0].bInSecond = bInSecond;
			m_dataList[0].TimeValueRange.first = _minTime;
			for (int i = 0; i < _nMinNumCol-1; i++)
			{
				m_dataList[i].TimeValueRange.second = m_dataList[i].TimeValueRange.first + eUnitTimeOnX;
				m_dataList[i+1].TimeValueRange.first = m_dataList[i].TimeValueRange.second;
				m_dataList[i+1].bInSecond = m_dataList[i].bInSecond; 
			}
			m_dataList[i].TimeValueRange.second = m_dataList[i].TimeValueRange.first + eUnitTimeOnX;
		}

		void CPaxDenistyDataLoader::LoadData()
		{
			m_dataList.clear();
			//m_dataList.resize(11);

			ArctermFile file;
			if(file.init(m_strFileName, READ) == FALSE)
				return;

			file.reset();
			file.skipLines(3);

			// find the max time value
			ElapsedTime maxXTimeValue;
			ElapsedTime minXTimeValue;
			bool bUnsetMinValue = true;
			ElapsedTime eTime;
			int count = 0 ;
			while (file.getLine())
			{
				file.setToField(1);
				file.getTime( eTime );
				OccupancyData _type ;
				_type.startTime = eTime ;

				file.getInteger(count) ;
				_type.nPaxCount = count ;
				m_dataList.push_back(_type) ;
			}
		}
		void TimeInTerminalDataLoader::LoadData()
		{

			m_dataList.clear();
			m_dataList.resize(11);
			ArctermFile file;
			if(file.init(m_strFileName, READ) == FALSE)
				return;
			file.reset();
			file.skipLines(3);
			// find the max time value
			ElapsedTime maxXTimeValue;
			ElapsedTime minXTimeValue;
			bool bUnsetMinValue = true;
			ElapsedTime eTime;
			while (file.getLine())
			{
				file.setToField(1);
				file.getTime( eTime );
				if( eTime > maxXTimeValue)
					maxXTimeValue = eTime;
				if( bUnsetMinValue )
				{
					minXTimeValue = eTime;
					bUnsetMinValue = false;
				}
				else if( eTime < minXTimeValue )
					minXTimeValue = eTime;
			}
			DecideRangeOnX( minXTimeValue, maxXTimeValue, 5 );
			file.reset();
			file.skipLines(3);
			while (file.getLine())
			{
				file.setToField (1);
				file.getTime( eTime );
				int nRangeCount = m_dataList.size();
				for( int i = 0; i < nRangeCount; i++)
				{
					if(m_dataList[i].Fit( eTime ))
					{
						m_dataList[i].nCount++;
					}
				}
			}

		}
//		void TimeInQueueDataLoader::DecideRangeOnX( ElapsedTime _minTime, ElapsedTime _maxTime, int _nMinNumCol )
		//{
		//	ElapsedTime timeDiff = _maxTime - _minTime;

		//	ElapsedTime eUnitTimeOnX;

		//	int nTimeDiffAsHours = timeDiff.asHours();
		//	int nTimeDiffAsMin = timeDiff.asMinutes();
		//	int nTimeDiffAsSec = timeDiff.asSeconds();

		//	bool bInSecond = false;
		//	if( nTimeDiffAsHours > _nMinNumCol )
		//	{
		//		// if hh  > _nMinNumCol, display column up to 2*_nMinNumCol
		//		int nMinHours = nTimeDiffAsHours / (2*_nMinNumCol) + 1;
		//		eUnitTimeOnX.set( nMinHours, 0, 0 );
		//		_minTime.SetMinute( 0 );
		//		_minTime.SetSecond( 0 );
		//		if( nTimeDiffAsHours > (2*_nMinNumCol) )
		//			_nMinNumCol = (2*_nMinNumCol);
		//		else
		//			_nMinNumCol = nTimeDiffAsHours;
		//	}
		//	else if( nTimeDiffAsMin > _nMinNumCol )
		//	{
		//		// else if hhmm > nNumCol min, so the column unit should be hhmm % 2 nNumCol min
		//		int nMinMinutes = nTimeDiffAsMin / ( 2*_nMinNumCol ) + 1;
		//		eUnitTimeOnX.set( nMinMinutes / 60, nMinMinutes % 60, 0 );
		//		_minTime.SetSecond( 0 );
		//		if( nTimeDiffAsMin > 2 * _nMinNumCol )
		//			_nMinNumCol = 2 * _nMinNumCol;
		//		else
		//			_nMinNumCol = nTimeDiffAsMin;
		//	}
		//	else
		//	{
		//		// be hhmmss % 2 nNumCol sec.
		//		int mMinSeconds = nTimeDiffAsSec / ( 2*_nMinNumCol ) + 1;
		//		eUnitTimeOnX.set( mMinSeconds / 3600, mMinSeconds / 60, mMinSeconds % 60 );
		//		if( nTimeDiffAsSec > 2 * _nMinNumCol )
		//			_nMinNumCol = 2 * _nMinNumCol;
		//		else
		//			_nMinNumCol = nTimeDiffAsSec;
		//		bInSecond = true;
		//	}

		//	m_dataList.resize( _nMinNumCol );

		//	m_nRangeCountOnX = _nMinNumCol;
		//	m_dataList[0].bInSecond = bInSecond;
		//	m_dataList[0].TimeValueRange.first = _minTime;
		//	for (int i = 0; i < _nMinNumCol-1; i++)
		//	{
		//		m_dataList[i].TimeValueRange.second = m_dataList[i].TimeValueRange.first + eUnitTimeOnX;
		//		m_dataList[i+1].TimeValueRange.first = m_dataList[i].TimeValueRange.second;
		//		m_dataList[i+1].bInSecond = m_dataList[i].bInSecond; 
		//	}
		//	m_dataList[i].TimeValueRange.second = m_dataList[i].TimeValueRange.first + eUnitTimeOnX;
		//}


		//void TimeInQueueDataLoader::LoadData()
		//{

		//	m_dataList.clear();

		//	ArctermFile file;
		//	if(file.init(m_strFileName, READ) == FALSE)
		//		return;

		//	file.reset();
		//	file.skipLines(3);

		//	// find the max time value
		//	ElapsedTime maxXTimeValue;
		//	ElapsedTime minXTimeValue;
		//	bool bUnsetMinValue = true;
		//	ElapsedTime eTime;
		//	while (file.getLine())
		//	{
		//		file.setToField(1);
		//		file.getTime( eTime );
		//		if( eTime > maxXTimeValue)
		//			maxXTimeValue = eTime;
		//		if( bUnsetMinValue )
		//		{
		//			minXTimeValue = eTime;
		//			bUnsetMinValue = false;
		//		}
		//		else if( eTime < minXTimeValue )
		//			minXTimeValue = eTime;
		//	}

		//	DecideRangeOnX( minXTimeValue, maxXTimeValue, 5 );

		//	file.reset();
		//	file.skipLines(3);
		//	while (file.getLine())
		//	{
		//		file.setToField (1);
		//		file.getTime( eTime );
		//		int nRangeCount = m_dataList.size();
		//		for( int i = 0; i < nRangeCount; i++)
		//		{
		//			if(m_dataList[i].Fit( eTime ))
		//			{
		//				m_dataList[i].nCount++;
		//			}
		//		}
		//	}

		//}



		void ActivityBreakdownDataLoader::LoadData()
		{
			m_dataList.clear();
		}

		void OccupancyDataLoader::LoadData()
		{
			m_dataList.clear();

			ArctermFile file;
			if(file.init(m_strFileName, READ) == FALSE)
				return;

			file.reset();
			file.skipLines(3);
			while (file.getLine())
			{
				file.setToField (1);
				ElapsedTime eTime;
				file.getTime( eTime );
				int nCount = 0;
				file.getInteger(nCount);
				
				OccupancyData occpancyData;
				occpancyData.startTime = eTime;
				occpancyData.nPaxCount = nCount;
				m_dataList.push_back(occpancyData);
			}

		}
	}
}