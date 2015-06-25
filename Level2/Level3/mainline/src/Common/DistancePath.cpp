// DistancePath.cpp: implementation of the CDistancePath class.
//
//////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "DistancePath.h"
#include <assert.h>
#include "states.h"
#include "ARCVector.h"
#include "elaptime.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


const DistanceUnit DELTASPEED = 2.0;     // m/s

CDistancePath::CDistancePath()
{

}

CDistancePath::~CDistancePath()
{
	if( m_pDistance )
		delete [] m_pDistance;
}



void CDistancePath::Calculate()
{
	const int nDiv = 4;

	int nOrigHitCount = getCount();
	m_dTotalLength = GetOverallLength();
	m_dAccLength = GetAccLength( m_dAccelerateSpeed, m_dOrigSpeed );
	m_dDeccLength = GetAccLength( m_dDecelerateSpeed, m_dOrigSpeed );
	m_dActualSpeed = m_dOrigSpeed;
	while( m_dAccLength + m_dDeccLength > m_dTotalLength )
	{
		m_dActualSpeed -= DELTASPEED;
		if( m_dActualSpeed < 0.0 )
			m_dActualSpeed = 0.0;
		m_dAccLength = GetAccLength( m_dAccelerateSpeed, m_dActualSpeed );
		m_dDeccLength = GetAccLength( m_dDecelerateSpeed, m_dActualSpeed );
	}


	double dSeconds = sqrtl( m_dAccLength * 2.0 / m_dAccelerateSpeed );
	m_AccPtTime = ElapsedTime( dSeconds );
	m_TotalTime = m_AccPtTime;

	DistanceUnit dLengthOnCruise = m_dTotalLength - m_dAccLength - m_dDeccLength;
	assert( dLengthOnCruise >= 0 );
	dSeconds = dLengthOnCruise / m_dActualSpeed;
	m_TotalTime += ElapsedTime( dSeconds );

	dSeconds = sqrtl( m_dDeccLength * 2.0 / m_dDecelerateSpeed );
	m_DecPtTime = ElapsedTime( dSeconds );
	m_TotalTime += m_DecPtTime;



	int nIdxInOrigList = 0;
	int i=0;
	// now calculate the accelerate segment
	CStateTimePoint stateTimePoint;
	stateTimePoint.SetState( TrainAccelerate );
	for(  i=0; i<nDiv; i++ )
	{
		DistanceUnit dCurAccLen = m_dAccLength / nDiv * ( i + 1 );

		// process all the point in the orig list whose distance < dCurAccLen
		while( nIdxInOrigList < nOrigHitCount && m_pDistance[nIdxInOrigList] < dCurAccLen )
		{
			// add point for m_pDistance[nIdxInOrigList] into result list.

			stateTimePoint.SetPoint( points[nIdxInOrigList] );
			stateTimePoint.SetTime( GetHitTime(nIdxInOrigList) );
			m_resultList.push_back(  stateTimePoint );
			nIdxInOrigList++;
		}

		Point pt;
		GetPointOnPath( dCurAccLen, pt );
		if( i == nDiv - 1 )
			stateTimePoint.SetState( TrainOnNormalSpeed );
		stateTimePoint.SetPoint( pt );
		stateTimePoint.SetTime( GetHitTime(dCurAccLen) );
		m_resultList.push_back( stateTimePoint );
	}

	// calculate the cruise segment
	stateTimePoint.SetState( TrainOnNormalSpeed );
	DistanceUnit dDecLengthFromStart = m_dTotalLength - m_dDeccLength ;
	while( nIdxInOrigList < nOrigHitCount && m_pDistance[nIdxInOrigList] < dDecLengthFromStart )
	{
		stateTimePoint.SetPoint( points[nIdxInOrigList] );
		stateTimePoint.SetTime( GetHitTime(nIdxInOrigList) );
		m_resultList.push_back(  stateTimePoint );
		nIdxInOrigList++;
	}


	//int nCount = getCount();
	//Point ptt = getPoint(nCount-1);
	// calculate the decelerate segmetn
	stateTimePoint.SetState( TrainDecelerate );
	for( i=nDiv-1; i>=0; i-- )
	{
		DistanceUnit dCurDecLen = m_dDeccLength / nDiv * ( i + 1 );
		DistanceUnit dCurDecLenFromStart = m_dTotalLength - dCurDecLen;

		// process all the point in the orig list whose distance < dCurAccLen
		while( nIdxInOrigList < nOrigHitCount && m_pDistance[nIdxInOrigList] < dCurDecLenFromStart )
		{
			// add point for m_pDistance[nIdxInOrigList] into result list.
			//points[nIdxInOrigList].setZ(ptt.getZ());
			stateTimePoint.SetPoint( points[nIdxInOrigList] );
			stateTimePoint.SetTime( GetHitTime(nIdxInOrigList) );
			m_resultList.push_back(  stateTimePoint );
			nIdxInOrigList++;
		}

		Point pt;
		GetPointOnPath( dCurDecLenFromStart, pt );
		//pt.setZ(ptt.getZ());
		stateTimePoint.SetPoint( pt );
		stateTimePoint.SetTime( GetHitTime(dCurDecLenFromStart) );
		m_resultList.push_back( stateTimePoint );
	}

	while( nIdxInOrigList < nOrigHitCount )
	{
		if( nIdxInOrigList == nOrigHitCount - 1 )
			stateTimePoint.SetState( TrainAtStation );
//		points[nIdxInOrigList].setZ(ptt.getZ());
		stateTimePoint.SetPoint( points[nIdxInOrigList] );
		stateTimePoint.SetTime( GetHitTime(nIdxInOrigList) );
		m_resultList.push_back(  stateTimePoint );
		nIdxInOrigList++;
	}


}


std::vector<CStateTimePoint>& CDistancePath::GetResultList()
{
	return m_resultList;
}


// Give index find time.
ElapsedTime CDistancePath::GetHitTime( int _nIdx )
{
	DistanceUnit dLength = m_pDistance[_nIdx];
	return GetHitTime( dLength );
}



// Give Acc and lenght find time.
ElapsedTime CDistancePath::GetHitTime( DistanceUnit _dLength )
{
	ElapsedTime resTime;
	if( _dLength < m_dAccLength )
	{
		double dSeconds = sqrtl( _dLength * 2.0 / m_dAccelerateSpeed );
		resTime = ElapsedTime( dSeconds );
	}
	else if( _dLength < m_dTotalLength - m_dDeccLength )
	{
		resTime = m_AccPtTime;
		DistanceUnit dLengthOnCruise = _dLength - m_dAccLength;
		double dSeconds = dLengthOnCruise / m_dActualSpeed;
		resTime = ElapsedTime( dSeconds ) + m_AccPtTime;
	}
	else
	{
		double dSeconds = sqrtl( ( m_dTotalLength - _dLength ) * 2.0 / m_dDecelerateSpeed );
		resTime = m_TotalTime - ElapsedTime( dSeconds );
	}
	return resTime;
}


void CDistancePath::SetSpeed( DistanceUnit _dAccelerateSpeed, DistanceUnit _dDecelerateSpeed, DistanceUnit _dOrigSpeed )
{
	m_dAccelerateSpeed = _dAccelerateSpeed;
	m_dDecelerateSpeed = _dDecelerateSpeed;
	m_dOrigSpeed = _dOrigSpeed;
}



// protected

// Cal the overall length.
DistanceUnit CDistancePath::GetOverallLength()
{
    int nCount = getCount();
	return m_pDistance[nCount-1];
}


// Give length, Cal the Point
bool CDistancePath::GetPointOnPath( DistanceUnit _dLength, Point& _point )
{
	if( _dLength > GetOverallLength() )
		return false;

	// get ptStart and ptEnd
    int nCount = getCount();
	int i=1;
	for(i=1; i<nCount; i++ )
	{
		DistanceUnit dLength = m_pDistance[i];
		if( dLength > _dLength )
			break;
	}

	Point ptStart = getPoint( i - 1 );
	Point ptEnd = getPoint( i );;
	DistanceUnit dStartPtLength = m_pDistance[i-1];
	

	Point v = ptEnd - ptStart;
	v.length( _dLength - dStartPtLength );
	_point = ptStart + v;

	ARCVector3 pt3Start(ptStart.getX(), ptStart.getY(), ptStart.getZ());
	ARCVector3 pt3End(ptEnd.getX(), ptEnd.getY(), ptEnd.getZ());
	ARCVector3 pt3Point(_point.getX(), _point.getY(), _point.getZ());

	DistanceUnit distToEnd = pt3Start.DistanceTo(pt3End);
	DistanceUnit distToPoint = pt3Start.DistanceTo(pt3Point);
	_point.setZ( ( ptEnd.getZ()-ptStart.getZ() ) * distToPoint / distToEnd + ptStart.getZ());
	return true;
}


// Give V and Acc give length
DistanceUnit CDistancePath::GetAccLength( DistanceUnit _acc, DistanceUnit _v )
{
	double t = _v / _acc;

	return _acc * t * t / 2.0;
}



void CDistancePath::Initialization()
{
	m_pDistance = new DistanceUnit[numPoints];
    int nCount = getCount();
	Point prevPt = getPoint( 0 );
	m_pDistance[0] = 0.0;
	for( int i=1; i<nCount; i++ )
	{
	    Point pt = getPoint( i );
		DistanceUnit dLength = pt.distance( prevPt );
		m_pDistance[i] = dLength + m_pDistance[i-1];
		prevPt = pt;
	}
}



