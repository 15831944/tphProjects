#include "StdAfx.h"
#include ".\calloutchartdata.h"



CCalloutChartData_TimeInterval* CCalloutChartData::GetTheTimeIntervalData( const ElapsedTime& _currentTime,int& _Ndx )
{

		CCalloutChartData_TimeInterval* pTimeInterval = NULL ;
		for (int i = m_TimeNdx; i < (int)m_TimeIntervalData.size() ;i++)
		{
			pTimeInterval = m_TimeIntervalData[i] ;
			if(pTimeInterval->m_StartTime <= _currentTime && _currentTime < pTimeInterval->m_EndTime )
			{
				m_TimeNdx = i ;
				_Ndx = m_TimeNdx ;
				return pTimeInterval ;
			}
		}
		return NULL ;
}

void CCalloutChartData::SetTheRefreshFlag( CCalloutChartData_TimeInterval* _timeInterval,BOOL _ref/*=TRUE*/ )
{
	CCalloutChartData_TimeInterval* pTimeInterval = NULL ;
	for (int i = 0 ; i < (int)m_TimeIntervalData.size() ;i++)
	{
		pTimeInterval = m_TimeIntervalData[i] ;
		if(pTimeInterval == _timeInterval)
			pTimeInterval->m_IsNeedRefresh = _ref ; //set current to TRUE
		else
			pTimeInterval->m_IsNeedRefresh = !_ref ;//anothers set to FALSE
	}
}

CCalloutChartData::~CCalloutChartData()
{
	for (int i = 0 ; i < (int)m_TimeIntervalData.size() ;i++)
	{
		delete m_TimeIntervalData[i] ;
	}
}