// TimePoint.cpp: implementation of the CTimePoint class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "TimePoint.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTimePoint::CTimePoint()
{

}

CTimePoint::~CTimePoint()
{

}

CTimePoint::CTimePoint( const CTimePoint& _timePoint )
	: Point( _timePoint )
{
	m_time = _timePoint.m_time;
}


void CTimePoint::SetTime( ElapsedTime _time )
{
	m_time = _time;
}

ElapsedTime CTimePoint::GetTime() const
{
	return m_time;
}


void CTimePoint::SetPoint( Point _pt )
{
	setX( _pt.getX() );
	setY( _pt.getY() );
	setZ( _pt.getZ() );
}
