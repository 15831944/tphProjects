// SpaceThroughtoutResult.cpp: implementation of the CSpaceThroughtputResult class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "SpaceThroughtoutResult.h"
#include "assert.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSpaceThroughtputResult::CSpaceThroughtputResult()
{

}

CSpaceThroughtputResult::~CSpaceThroughtputResult()
{

}
void CSpaceThroughtputResult::InitItem( ElapsedTime _startTime, ElapsedTime _intervalTime,int _iIntervalCount )
{
	m_vResult.clear();
	THROUGHTOUTITEM temp;
	temp.m_lThroughtout = 0L;
	for( int i=0; i< _iIntervalCount-1; ++i )
	{
		temp.m_startTime = _startTime;
		_startTime += _intervalTime;
		temp.m_endTime = _startTime;
		m_vResult.push_back( temp );
	}

}
void CSpaceThroughtputResult::IncreaseCount( ElapsedTime& _time,int nIncNum /*= 1*/ )
{
	THROUGHTOUTRESULT::iterator iter = m_vResult.begin();
	THROUGHTOUTRESULT::iterator iterEnd = m_vResult.end();
	for( ; iter != iterEnd; ++iter )
	{
		if ( (*iter).m_startTime <= _time &&
			 (*iter).m_endTime >= _time 
			)
		{
			(*iter).m_lThroughtout += nIncNum;
			break;
		}
	}
}



THROUGHTOUTITEM& CSpaceThroughtputResult::operator [] ( int _iIndex )
{
	assert( _iIndex >= 0 && _iIndex < (int)m_vResult.size() );
	return m_vResult[ _iIndex ];
}