// PipeInterSectionPoint.cpp: implementation of the CPointToPipeXPoint class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "PipeInterSectionPoint.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPointToPipeXPoint::CPointToPipeXPoint():m_iOtherInfo(-1)
{
	m_nIdxSeg = -1;		// segment index;
	m_dDistFromStartPt = 0.0;		// distance from start of point of the segment.	
	m_pipeIndex = -1;
	m_bPointOnPipe = false;
	m_nPointIndex = -1;
}



CPointToPipeXPoint::~CPointToPipeXPoint()
{
}
bool CPointToPipeXPoint::operator < ( const CPointToPipeXPoint& _another ) const
{
	if(	m_nIdxSeg < _another.m_nIdxSeg )
	{
		return true;
	}
	else if( m_nIdxSeg > _another.m_nIdxSeg  )
	{
		return false;
	}
	else
	{
		if( m_dDistFromStartPt < _another.m_dDistFromStartPt )
		{
			return true;
		}
		else if( m_dDistFromStartPt > _another.m_dDistFromStartPt )
		{
			return false;
		}
		else
		{
			return true;
		}

	}
}
void CPointToPipeXPoint::SetPointToPipeXPoint( const CPointToPipeXPoint& _pt )
{
	Point::SetPoint( _pt );
	m_nIdxSeg = _pt.GetSegmentIndex();
	m_dDistFromStartPt = _pt.GetDistanceFromStartPt();
}


void CPointToPipeXPoint::SetSegmentIndex( int _nIdx )
{
	m_nIdxSeg = _nIdx;
}


int CPointToPipeXPoint::GetSegmentIndex() const
{
	return m_nIdxSeg;
}

void CPointToPipeXPoint::SetDistanceFromStartPt( DistanceUnit _dDist )
{
	m_dDistFromStartPt = _dDist;
}


DistanceUnit CPointToPipeXPoint::GetDistanceFromStartPt() const
{
	return m_dDistFromStartPt;
}


CPipeToPipeXPoint::CPipeToPipeXPoint()
{
	m_nOtherIdxSeg = -1;		// segment index;
	m_dOtherDistFromStartPt = 0.0;		// distance from start of point of the segment.	
	m_iPipeIndex1 = -1;
	m_iPipeIndex2 = -1;
}


CPipeToPipeXPoint::~CPipeToPipeXPoint()
{
}


void CPipeToPipeXPoint::SetPipeToPipeXPoint( const CPipeToPipeXPoint& _pt )
{
	CPointToPipeXPoint::SetPointToPipeXPoint( _pt );
	m_nOtherIdxSeg = _pt.GetOtherSegIndex();
	m_dOtherDistFromStartPt = _pt.GetOtherDistFromStart();	
	m_ptSameOrder1 = _pt.GetSameOrderPoint1();
	m_ptSameOrder2 = _pt.GetSameOrderPoint2();
	m_ptDiffOrder1 = _pt.GetDiffOrderPoint1();
	m_ptDiffOrder2 = _pt.GetDiffOrderPoint2();
}


void CPipeToPipeXPoint::SetSameOrderPoints( Point _ptSameOrder1, Point _ptSameOrder2 )
{
	m_ptSameOrder1 = _ptSameOrder1;
	m_ptSameOrder2 = _ptSameOrder2;
}


void CPipeToPipeXPoint::SetDiffOrderPoints( Point _ptDiffOrder1, Point _ptDiffOrder2 )
{
	m_ptDiffOrder1 = _ptDiffOrder1;
	m_ptDiffOrder2 = _ptDiffOrder2;
}


CMobPipeToPipeXPoint::CMobPipeToPipeXPoint()
{
	m_bInInc = true;
	m_bOutInc = true;
}


CMobPipeToPipeXPoint::~CMobPipeToPipeXPoint()
{
}


void CMobPipeToPipeXPoint::SetMobPipeToPipeXPoint( const CMobPipeToPipeXPoint& _pt )
{
	CPipeToPipeXPoint::SetPipeToPipeXPoint( _pt );
	m_bInInc = _pt.GetInInc();
	m_bOutInc = _pt.GetOutInc();	
}

/*
void CMobPipeToPipeXPoint::SetPipeToPipeXPoint( const CPipeToPipeXPoint& _pt )
{
	this->SetSameOrderPoints()
	CPipeToPipeXPoint( _pt );
}
*/

Point CMobPipeToPipeXPoint::GetPoint1() const
{
	if( ( m_bInInc && m_bOutInc ) || ( !m_bInInc && !m_bOutInc ) )
		return m_ptSameOrder1;	
	else
		return m_ptDiffOrder1;
}


Point CMobPipeToPipeXPoint::GetPoint2() const
{
	if( ( m_bInInc && m_bOutInc ) || ( !m_bInInc && !m_bOutInc ) )
		return m_ptSameOrder2;	
	else
		return m_ptDiffOrder2;
}


// get the exit point information so we can decide the outinc.
void CMobPipeToPipeXPoint::SetOutInc( const CPointToPipeXPoint& _pt )
{
	if( _pt.GetSegmentIndex() == m_nOtherIdxSeg )
	{
		m_bOutInc = ( _pt.GetDistanceFromStartPt() >= m_dOtherDistFromStartPt ) ? true : false;
	}
	else
	{
		m_bOutInc = ( _pt.GetSegmentIndex() > m_nOtherIdxSeg ) ? true : false;
	}
}


