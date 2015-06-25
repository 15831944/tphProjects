// StateTimePoint.cpp: implementation of the CStateTimePoint class.
//
//////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "StateTimePoint.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CStateTimePoint::CStateTimePoint()
{

}

CStateTimePoint::~CStateTimePoint()
{

}

CStateTimePoint::CStateTimePoint( const CStateTimePoint& _timePoint )
	:CTimePoint( _timePoint )
{
	m_state = _timePoint.m_state;		
}


void CStateTimePoint::SetState( char _state )
{
	m_state = _state;	
}


char CStateTimePoint::GetState() const
{
	return m_state;
}
