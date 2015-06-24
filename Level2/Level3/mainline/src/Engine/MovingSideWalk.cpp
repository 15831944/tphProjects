// MovingSideWalk.cpp: implementation of the MovingSideWalk class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MovingSideWalk.h"
#include "..\common\termfile.h"
#include "person.h"
#include "inputs\SideMoveWalkProcData.h"
#include "inputs\SideMoveWalkDataSet.h"
#include "inputs\prochrch.h"
#include "inputs\probab.h"
#include "inputs\SideMoveWalkPaxDist.h"
#include "common\pollygon.h"
#include "engine\terminal.h"
#include <Common/ProbabilityDistribution.h>
#include "TerminalMobElementBehavior.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

MovingSideWalk::MovingSideWalk()
{
	m_dMovingSpeed = -1.0;
	m_dWidth = -1.0;
	m_nCapacity = 0;
}

MovingSideWalk::~MovingSideWalk()
{

}

void MovingSideWalk::initServiceLocation (int pathCount, const Point *pointList)
{
	 if (pathCount < 2)
        throw new StringError ("Moving side walk must have at least 2 point service locations");

    m_location.init (pathCount, pointList);
	UpdateMinMax();
}


// It is called by Person::processServerArrival
// The different processor types serve clients in different ways.
void MovingSideWalk::beginService( Person* _pPerson, ElapsedTime _curTime )
{
	// decide if walk or stand.
	assert( m_dMovingSpeed >= 0 );

	//// TRACE ("\n%s\n",_curTime.printTime() );
	ElapsedTime eventTime = _curTime;
	int nCount = m_location.getCount();
	_pPerson->setState( OnMovingSideWalk );

	TerminalMobElementBehavior* spTerminalBehavior = _pPerson->getTerminalBehavior();
	if (spTerminalBehavior == NULL)
	{
		return;
	}

	DistanceUnit dGap = spTerminalBehavior->GetCurMovingSidewalkGap();

	for( int i=1; i<nCount; i++ )
	{
		Point ptSource = m_location.getPoint( i - 1 );
		Point ptDest = m_location.getPoint( i );
        Point vector( ptSource, ptDest );
        Point latPoint( vector.perpendicular() );		


		latPoint.length ( dGap );
		ptDest += latPoint;

		_pPerson->setTerminalDestination( ptDest  );
		bool bWalk = false;
		if( m_nLanes > 1 )
			bWalk = spTerminalBehavior->IsWalkOnMovingSideWalk();
		eventTime += _pPerson->moveTime( m_dMovingSpeed, bWalk );
		_pPerson->writeLogEntry( eventTime, false );																				
	}
	//// TRACE ("\n%s\n",eventTime.printTime() );
	_pPerson->setState( LeaveServer );
	_pPerson->generateEvent( eventTime ,false);
}
void MovingSideWalk::CaculatePointTime( PTONSIDEWALK& _vResult, int nPipeIndex,int _iStartIdx, int _iEndIdx ,DistanceUnit _dStartDistance,DistanceUnit _dEndDistance  )const
{
	//assert( m_dMovingSpeed >= 0 );
	ElapsedTime eventTime ;
	DistanceUnit dSpeed = GetMovingSpeed();
	assert( dSpeed >= 0 );
	int iProcIdx = this->getIndex();
	CTimePointOnSideWalk temp;
	temp.SetSideWalkIdx( iProcIdx );
	temp.SetOnSideWalkFlag( true );
	temp.SetPipeIdx(nPipeIndex);
	temp.SetStartPointIdxInPipe( _iStartIdx );
	temp.SetEndPointIdxInPipe( _iEndIdx );
	temp.SetStartDistance( _dStartDistance );
	temp.SetEndDistance( _dEndDistance );
	//_vResult.push_back( temp );	

	int nCount = m_location.getCount();
	for( int i=0, j=1; j<nCount; ++j,++i )
	{	
		Point ptSource = m_location.getPoint( i );
		temp.SetPoint( ptSource );
		temp.SetTime( eventTime );	
		_vResult.push_back( temp );

		Point ptDest = m_location.getPoint( j );
		eventTime += (long)(ptSource.distance( ptDest ) / dSpeed);
		temp.SetPoint( ptDest );
		temp.SetTime( eventTime );
		
		_vResult.push_back( temp );		
	}	
}
// Acquire service m_location.
Point MovingSideWalk::AcquireServiceLocation( Person* _pPerson  )
{
	bool bWalkOnMovingSideWalk = false;
	
	ProcessorEntry* pEntry = (ProcessorEntry *)m_pTerm->m_pMovingSideWalkPaxDist->getEntryPoint( name );
	if( pEntry )
	{
		const ProbabilityDistribution* pProb = pEntry->hierarchyLookup( _pPerson->getType() );
	    if( pProb && random( 100 ) < pProb->getRandomValue() )
			bWalkOnMovingSideWalk = true;
	}
	TerminalMobElementBehavior* spTerminalBehavior = _pPerson->getTerminalBehavior();

	
	spTerminalBehavior->SetWalkOnMovingSideWalk( bWalkOnMovingSideWalk );

	// pick the lane number and gap
	int nLaneIdx = 0;
	if( bWalkOnMovingSideWalk && m_nLanes > 1 )
		nLaneIdx = random( m_nLanes - 1 ) + 1;

	DistanceUnit dGap = m_vGap[nLaneIdx];
	spTerminalBehavior->SetCurMovingSidewalkGap( dGap );
	
	
	Point ptSource = m_location.getPoint( 0 );
	Point ptDest = m_location.getPoint( 1 );
    Point vector( ptSource, ptDest );
    Point latPoint( vector.perpendicular() );		
	latPoint.length ( dGap );
	ptSource += latPoint;
	return ptSource;
}



// refresh the related data before engine start;
void MovingSideWalk::RefreshRelatedData( InputTerminal* _pInTerm )
{
	CSideMoveWalkProcData* pData = _pInTerm->m_pMovingSideWalk->GetLinkedSideWalkProcData( name );
	assert( pData );
	m_dMovingSpeed = pData->GetMoveSpeed();
	m_dWidth = pData->GetWidth();	
	m_nCapacity = pData->GetCapacity();


	// calculate

	// each 0.5 m form 1 lane.
	m_nLanes = (int)m_dWidth;
	m_nLanes /= 50;
	if( m_nLanes < 1 )
		m_nLanes = 1;

	m_vGap.clear();

	bool bEven = m_nLanes % 2 == 0;
	DistanceUnit dLaneWidth = m_dWidth / m_nLanes;

	// calculate the right most line.
	DistanceUnit dGap = m_nLanes;
	dGap -= 1.0;
	dGap /= 2.0;
	dGap *= dLaneWidth;
	dGap = -dGap;
	for( int i=0; i<m_nLanes; i++ )
	{
		m_vGap.push_back( dGap );
		dGap += dLaneWidth;
	}
}
// test if this sidewalk is contained by the specific region
bool MovingSideWalk::IsContainedBy( POLLYGONVECTOR& _regions ) const
{
	int iAreaPollygonCount = _regions.size();
	if( iAreaPollygonCount < 1 )
		return false;
	
	POLLYGONVECTOR sideWalkCovered;
	GetSideWalkCoveredRegion( sideWalkCovered );
	int iSideWalkPollygonCount = sideWalkCovered.size();

	for( int i=0; i< iSideWalkPollygonCount ; ++i )
	{
		int iSideWalkPoint = sideWalkCovered.at(i).getCount();
		
		
		for( int s=0; s<iSideWalkPoint ; ++s )
		{
			bool bContainedby = false;
			for( int j=0; j<iAreaPollygonCount ; ++j )
			{			
				if( _regions.at( j ).contains( sideWalkCovered.at( i ).getPoint(s) ) )
				{
					bContainedby = true;
					break;
				}

			}
			
			if( ! bContainedby )
				return false;
			
		}

	}	
	return true;
}

//get all region this sidewalk covered
void MovingSideWalk::GetSideWalkCoveredRegion( POLLYGONVECTOR& _regions ) const
{
	_regions.clear();
	int iPointCount = m_location.getCount();
	if( iPointCount <2 )
			return ;

	DistanceUnit dWidth = GetWidth();
	for(int i=0; i < iPointCount-1 ; ++i )
	{
		Pollygon temp;
		GetSiglePollygon( m_location.getPoint( i ), m_location.getPoint( i+1 ), dWidth/2.0/* +100*/, temp );
		_regions.push_back( temp );
	}
}

void MovingSideWalk::GetSiglePollygon( Point& _first,Point& _second, DistanceUnit _width, Pollygon& _resultPllygon) const
{
	Point pvector;
	pvector.initVector( _first, _second );
	Point vectorX = pvector.perpendicular();
	Point vectorY = pvector.perpendicularY();

	Point pArray[4];
	
	pArray[0]=vectorX ;
	pArray[1]=vectorY ;	
	pArray[0].length( _width );
	pArray[1].length( _width );
	pArray[0].DoOffset( _first.getX(),_first.getY(),0 );
	pArray[1].DoOffset( _first.getX(),_first.getY(),0 );

	pArray[2]=vectorY ;
	pArray[3]=vectorX ;	
	pArray[2].length( _width );
	pArray[3].length( _width );
	pArray[2].DoOffset( _second.getX(),_second.getY(),0 );
	pArray[3].DoOffset( _second.getX(),_second.getY(),0 );

	//distinguish different floor
	DistanceUnit dZ = this->getServicePoint( 0 ).getZ();
	for( int i=0; i<4; ++i )
	{
		pArray[i].setZ( dZ );
	}
	
	_resultPllygon.init( 4, pArray );
}
DistanceUnit MovingSideWalk::GetMovingSpeed(  ) const
{
	assert( m_pTerm );
	CSideMoveWalkProcData* pData = m_pTerm->m_pMovingSideWalk->GetLinkedSideWalkProcData( name );
	assert( pData );
	return pData->GetMoveSpeed();

}
DistanceUnit MovingSideWalk::GetWidth( ) const
{
	assert( m_pTerm );
	CSideMoveWalkProcData* pData = m_pTerm->m_pMovingSideWalk->GetLinkedSideWalkProcData( name );
	assert( pData );
	return pData->GetWidth();		
}


// check if this processor's population is exceed the capacity.
// bool MovingSideWalk::IsMyQueueExceedCapacity() const
// {
//    if( m_nCapacity > 0 )
// 		return GetTotalNumberPassengerInThisProcessor() >= m_nCapacity;
//    else
// 		return false;
// }

Processor* MovingSideWalk::CreateNewProc()
{
	Processor* pProc = new MovingSideWalk;
	return pProc;
}

void MovingSideWalk::UpdateMinMax(){
	Processor::UpdateMinMax();
}