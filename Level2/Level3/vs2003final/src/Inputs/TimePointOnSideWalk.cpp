// TimePointOnSideWalk.cpp: implementation of the CTimePointOnSideWalk class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TimePointOnSideWalk.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTimePointOnSideWalk::CTimePointOnSideWalk(): m_bOnMovingSideWalk( false ),m_iMovingSideWalkIdx( -1 ),m_iPipeIndex(-1), m_iStartPointIdxInPipe( -1 )
											,m_iEndPointIdxInPipe( -1 ),m_dStartDistance(0.0),m_dEndDistance(0.0),m_dRealDistanceFromStart(-1.0)
{

}

CTimePointOnSideWalk::~CTimePointOnSideWalk()
{

}

bool CTimePointOnSideWalk::operator < ( const CTimePointOnSideWalk& _anotherInstance ) const
{
	if( m_dStartDistance != _anotherInstance.m_dStartDistance )
		return 	 m_dStartDistance < _anotherInstance.m_dStartDistance;
	else
	{
		return m_dRealDistanceFromStart < _anotherInstance.m_dRealDistanceFromStart;
	}
}
