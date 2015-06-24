// BaggageProcUtil.cpp: implementation of the BaggageProcUtil class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BaggageProcUtil.h"
#include "common\path.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

BaggageProcUtil::BaggageProcUtil()
{

}

BaggageProcUtil::~BaggageProcUtil()
{

}
// divide baggage moving path as several point
void BaggageProcUtil::Init( Path* _ptList, DistanceUnit& _dSpeed  )
{
	m_vPointAndBags.clear();
	int iCount = _ptList->getCount();
	ASSERT( iCount >= 3 );
	bool bNotEnd = true;
	
	for(int i=0, j=1 ; j<=iCount && bNotEnd; ++i,++j )
	{
		if( j == iCount )
		{
			i = iCount -1;
			j = 0;
			bNotEnd = false;
		}
		Point pt1 = _ptList->getPoint( i );
		Point pt2 = _ptList->getPoint( j );
		DistanceUnit dDistance = pt1.distance( pt2 );
		long iPointCount = (long)(dDistance / _dSpeed);
		
		PointAndBagsInBagProcessor temp;
		temp.SetPosPoint( pt1 );
		m_vPointAndBags.push_back( temp );

		if( iPointCount > 0 )
		{
			Point pVector;		
			for( int j=0; j<iPointCount-1; ++j )
			{
				pVector.initVector( pt1, pt2 );	
				pVector.length( (j+1)* _dSpeed );
				Point sevicePos( pt1 ) ;
				sevicePos += pVector;
				
				temp.SetPosPoint( sevicePos );
				m_vPointAndBags.push_back( temp );

			}
		}
	}

	iCount = m_vPointAndBags.size();
	for(int ii=0, jj=1; jj<iCount; ++ii, ++jj )
	{
		DistanceUnit dDistance = m_vPointAndBags[ii].GetPosPoint().distance( m_vPointAndBags[jj].GetPosPoint() );
		ElapsedTime costTime ( dDistance / _dSpeed );
		m_vPointAndBags[ii].SetCostTime( costTime );

		m_costTimeOfCircle += costTime;
	}

	
}
// get next point on baggage moving path according to current index
Point BaggageProcUtil::GetNextPos( int _iCurrentIdx ) const
{
	int iSize = m_vPointAndBags.size();
	_iCurrentIdx = ( _iCurrentIdx + 1 ) % iSize;
	return m_vPointAndBags[ _iCurrentIdx ].GetPosPoint();
}
// add a _pPerson into baggage device's _iCurrentIdx 
void BaggageProcUtil::AddPerson( int _iCurrentIdx , long _newPerson )
{
	ASSERT( _iCurrentIdx >=0 )	;
	m_vPointAndBags[ _iCurrentIdx ].AddPerson( _newPerson );
}

// Erase a _pPerson into baggage device's _iCurrentIdx 
void BaggageProcUtil::ErasePerson( int _iCurrentIdx , long _delPerson )
{
	ASSERT( _iCurrentIdx >=0 )	;
	m_vPointAndBags[ _iCurrentIdx ].ErasePerson( _delPerson );
}

// get the nearest point's index in baggae moving path 
int BaggageProcUtil::GetNearestPosIdx ( const Point& _ptTestPoint  ) const
{
	DistanceUnit dShortestDistance = 999999999999.0;
	int iPointCount = m_vPointAndBags.size();
	ASSERT( iPointCount >= 0 );
	int iReturnValue=0;
	for( int i=0; i<iPointCount; ++i )
	{
		Point ptService ( m_vPointAndBags[ i ].GetPosPoint() );
		DistanceUnit dTestDistance = ptService.distance( _ptTestPoint );
		if( dTestDistance < dShortestDistance )
		{
			dShortestDistance = dTestDistance;
			iReturnValue = i;			
		}
	}

	return iReturnValue;
}

// get a point from baggage moving path which is the nearest point from _ptTestPoint
void BaggageProcUtil::GetNearestPoint ( const Point& _ptTestPoint , Point& _ptResult ) const
{
	DistanceUnit dShortestDistance = 999999999999.0;
	int iPointCount = m_vPointAndBags.size();
	ASSERT( iPointCount >= 0 );
	int iReturnValue=0;
	for( int i=0; i<iPointCount; ++i )
	{
		Point ptService ( m_vPointAndBags[ i ].GetPosPoint() );
		DistanceUnit dTestDistance = ptService.distance( _ptTestPoint );
		if( dTestDistance < dShortestDistance )
		{
			dShortestDistance = dTestDistance;
			_ptResult = ptService;			
		}
	}

}

// Passenger pick up bags on current pos
void BaggageProcUtil::PickUpBags( Passenger* _pOwner , int _iCurrentPos, BAGS& _vBags )
{
	int iPointCount = m_vPointAndBags.size();
	ASSERT( _iCurrentPos >= 0 );
	m_vPointAndBags[ _iCurrentPos ].PickUpBags( _pOwner, _vBags ); 
}