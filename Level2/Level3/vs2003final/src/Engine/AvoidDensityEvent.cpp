// AvoidDensityEvent.cpp: implementation of the AvoidDensityEvent class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "AvoidDensityEvent.h"
#include "inputs\in_term.h"
#include "..\inputs\areasportals.h"
#include "engine\mobile.h"
#include "engine\person.h"
#include "common\CodeTimeTest.h"
#include "../Common/ARCTracker.h"
#include "Engine/ARCportEngine.h"
#include "terminal.h"
#include "TerminalMobElementBehavior.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

AvoidDensityEvent::AvoidDensityEvent()
{

}

AvoidDensityEvent::~AvoidDensityEvent()
{

}

void AvoidDensityEvent::InitEx(  MobileElement *aMover, ElapsedTime eventTime, 
								  int _iRealState, const Point& _realPt, const INTERSECT_INFO_LIST& _infoList,bool bMoLog )
{
	m_iRealState		= _iRealState;
	m_ptRealDestination = _realPt;
	m_vIntersectInfo	= _infoList;
	
	MobileElementMovementEvent::init( aMover, eventTime ,m_bNoLog);
}

// processor event
int AvoidDensityEvent::process (CARCportEngine *_pEngine )
{
	PLACE_METHOD_TRACK_STRING();
	bool _bAllCanEntry = true;
	INTERSECT_INFO _cannotEntryInfo;
	std::vector<CArea*> _allCanEntryArea;
	TerminalMobElementBehavior* spTerminalBehavior = (TerminalMobElementBehavior*)((Person*)mobileElement)->getBehavior(MobElementBehavior::TerminalBehavior);
	
	for( UINT i=0; i<m_vIntersectInfo.size(); i++ )
	{
		INTERSECT_INFO _intersectInfo = m_vIntersectInfo[i];
		CArea* _pArea = _intersectInfo.pArea;

		switch( _intersectInfo.state )
		{
		case LEAVE_AREA:	// leave area
			// decrease count
			_pArea->personLeaveArea( (Person*)mobileElement, time );
			
			break;
			
		case ENTRY_AREA:	// entry area
			// if the area is full, and the real destination is not in the area
			// shall avoid the area
			if(spTerminalBehavior&&spTerminalBehavior->checkAreaIsFull( _pArea )
				&& !_pArea->containsPoint( m_ptRealDestination ) )	
			{
				_bAllCanEntry = false;
				_cannotEntryInfo = _intersectInfo;
			}
			else
			{
				_allCanEntryArea.push_back( _pArea );
			}
			
			break;

		case MOVETO_CORNER:
			mobileElement->writeLogEntry( time, false );
			if (spTerminalBehavior)
			{
				spTerminalBehavior->m_intersectPointList.clear();
			}
			break;
		
		case  ARRIVAL_DEST:
			// arrival the real destination
			mobileElement->setState( m_iRealState );
			if (spTerminalBehavior)
			{
				spTerminalBehavior->doSomethingAfterAvoid( time );
				getPersonRealtimeDest();
				mobileElement->MobileElement::generateEvent( time,m_bNoLog );
			}
			return 1;

		default:
			assert( 0 );
			break;
		}

	}

	if( _bAllCanEntry )	// all area can entry
	{
		for( UINT i=0; i< _allCanEntryArea.size(); i++ )
			_allCanEntryArea[i]->personEntryArea( (Person*)mobileElement, time );
	}
	else				// else avoid area
	{	
		mobileElement->writeLogEntry( time, false );
		avoidDensityArea( _cannotEntryInfo, _pEngine->getTerminal() );
	}
	
	// generate next event
	generateNextEvent();
	return 1;
}


// avoid the density area
void AvoidDensityEvent::avoidDensityArea( const INTERSECT_INFO& _info,InputTerminal* _pInTerm )
{
	Person* _person = (Person*)mobileElement;
	TerminalMobElementBehavior* spTerminalBehavior = (TerminalMobElementBehavior*)_person->getBehavior(MobElementBehavior::TerminalBehavior);
	if (spTerminalBehavior == NULL)
	{
		return;
	}
	spTerminalBehavior->m_intersectPointList.clear();

	CArea* _pArea = _info.pArea;
	assert( _pArea );
	
	// get the next seg to avoid _pArea;
	int _iNextSeg;
	Point purePt;
	Point destPt;
	Point locationPt;
	_person->getTerminalPureLocation(purePt);
	_person->getTerminalDest(destPt);
	_person->getTerminalPoint(locationPt);

	int _iIsSegPt = _pArea->getPointByIndex( _info.iSegment).preciseCompare( purePt );
	if( spTerminalBehavior->m_iAvoidDirection == 0 ) // to less direction
		_iNextSeg = (_info.iSegment - _iIsSegPt + _pArea->getPointCount() ) % _pArea->getPointCount();
	else								  // to great direction	
		_iNextSeg = (_info.iSegment + 1) % _pArea->getPointCount();
	
	//try to go to the next destination
	Point _ptNextDest = _pArea->getPointByIndex( _iNextSeg );
	_person->setTerminalDestination( _ptNextDest );
	_pInTerm->m_pAreas->getAllIntersectPoint1( locationPt, purePt, destPt,
										       time, mobileElement->getSpeed(),spTerminalBehavior->m_intersectPointList ) ;
	
	// check person if can move follow this direction
	bool _bTryEntryArea = false;
	if( spTerminalBehavior->m_intersectPointList.size() >0 )
	{
		AreaIntersectPoint _firstPt = spTerminalBehavior->m_intersectPointList[0];	
		if( _firstPt.point.preciseCompare( purePt ) )
		{
			for( UINT i=0; i< _firstPt.intersectInfo.size(); i++ )
			{
				if( _firstPt.intersectInfo[i].state == ENTRY_AREA &&
				   spTerminalBehavior->checkAreaIsFull( _firstPt.intersectInfo[i].pArea ) ) 
				{
					_bTryEntryArea = true;
					break;
				}
			}

		}	
	}
	
	// the direction can not move, try to next direction
	if( _bTryEntryArea )	
	{
		spTerminalBehavior->m_iAvoidDirection = !spTerminalBehavior->m_iAvoidDirection;

		if( spTerminalBehavior->m_iAvoidDirection == 0 ) // to less
			_iNextSeg = (_info.iSegment - _iIsSegPt + _pArea->getPointCount()) % _pArea->getPointCount();
		else								  // to great	
			_iNextSeg = (_info.iSegment + 1) % _pArea->getPointCount();
		
		_ptNextDest = _pArea->getPointByIndex( _iNextSeg );
		//try to go to the next destination
		_person->setTerminalDestination( _ptNextDest );
		_pInTerm->m_pAreas->getAllIntersectPoint1( locationPt, purePt, destPt,
												   time, mobileElement->getSpeed(),spTerminalBehavior->m_intersectPointList ) ;	
	}

	// add the end point to the intersect point list
	AreaIntersectPoint _endPoint;
	_endPoint.point = _ptNextDest;
	_endPoint.intersectTime = time + _person->moveTime();
	_endPoint.intersectInfo.push_back( INTERSECT_INFO( MOVETO_CORNER, _pArea, _info.iSegment ) );

	spTerminalBehavior->m_intersectPointList.push_back( _endPoint );	
}


// generate next event
void AvoidDensityEvent::generateNextEvent( )
{
	Person* _pPerson = (Person*)mobileElement;
	TerminalMobElementBehavior* spTerminalBehavior = (TerminalMobElementBehavior*)_pPerson->getBehavior(MobElementBehavior::TerminalBehavior);

	if (spTerminalBehavior == NULL)
	{
		return;
	}

	if( spTerminalBehavior->m_intersectPointList.size()==0 )	//no next intersect point, go to real destination
	{
		_pPerson->setState( m_iRealState );
		spTerminalBehavior->doSomethingAfterAvoid( time );
		getPersonRealtimeDest();
		_pPerson->generateEvent( time + _pPerson->moveTime(),false );
	}
	else											// have next intersect, walk follow the intersect point list
	{
		_pPerson->setTerminalDestination( spTerminalBehavior->m_intersectPointList[0].point );
		_pPerson->generateAvoidDensityEvent( spTerminalBehavior->m_intersectPointList[0], m_iRealState, m_ptRealDestination ,false );
		spTerminalBehavior->m_intersectPointList.erase( spTerminalBehavior->m_intersectPointList.begin() );
	}
	
}


void AvoidDensityEvent::getPersonRealtimeDest( void )
{
	Person* _pPerson = (Person*)mobileElement;

	TerminalMobElementBehavior* spTerminalBehavior = (TerminalMobElementBehavior*)_pPerson->getBehavior(MobElementBehavior::TerminalBehavior);

	if (spTerminalBehavior == NULL)
	{
		return;
	}
	
	switch( _pPerson->getState()) 
	{
	case MoveToQueue:
		spTerminalBehavior->getProcessor()->GetQueueTailPoint( _pPerson );
		break;
	
	default:
		_pPerson->setTerminalDestination( m_ptRealDestination );
		break;
	}
}