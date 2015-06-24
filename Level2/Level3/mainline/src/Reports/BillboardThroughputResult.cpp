#include "StdAfx.h"
#include ".\billboardthroughputresult.h"
#include <assert.h>

CBillboardThroughputResult::CBillboardThroughputResult(void)
{
}

CBillboardThroughputResult::~CBillboardThroughputResult(void)
{
}
void CBillboardThroughputResult::InitItem( ElapsedTime _startTime, ElapsedTime _intervalTime,int _iIntervalCount )
{
	m_vResult.clear();
	BillboardThroughputItem temp;
	for( int i=0; i< _iIntervalCount-1; ++i )
	{
		temp.m_startTime = _startTime;
		_startTime += _intervalTime;
		temp.m_endTime = _startTime;
		m_vResult.push_back( temp );
	}

}
void CBillboardThroughputResult::IncreaseCount( ElapsedTime& _time,int nTotalOrExposure )
{
	BILLBOARDTHROUGHTOUTRESULT::iterator iter = m_vResult.begin();
	BILLBOARDTHROUGHTOUTRESULT::iterator iterEnd = m_vResult.end();
	for( ; iter != iterEnd; ++iter )
	{
		if ( (*iter).m_startTime <= _time &&
			(*iter).m_endTime >= _time 
			)
		{
			if (nTotalOrExposure ==0) //total throughput count
			{
				(*iter).m_lTotalThroughput ++ ;
			}
			else
			{
				(*iter).m_lExposureCount ++ ;			
			}

			break;
		}
	}
}



BillboardThroughputItem& CBillboardThroughputResult::operator [] ( int _iIndex )
{
	assert( _iIndex >= 0 && _iIndex < (int)m_vResult.size() );
	return m_vResult[ _iIndex ];
}