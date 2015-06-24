// FireEvacuationEvent.cpp: implementation of the FireEvacuationEvent class.
//
//////////////////////////////////////////////////////////////////////
#include"stdafx.h"
#include "FireEvacuationEvent.h"
#include "engine\movevent.h"
#include "common\point.h"
#include "engine\mobile.h"
#include "engine\person.h"
#include "engine\procq.h"
#include "inputs\paxdata.h"
#include "engine\lineproc.h"
#define  Default_Person_Response_Time 20l
#define  Default_Person_Speed_Impact 3l
#include "common\CodeTimeTest.h"
#include "hold.h"
#include "../Common/ARCTracker.h"
#include "Engine/ARCportEngine.h"
#include "terminal.h"
#include <Common/ProbabilityDistribution.h>
#include "TerminalMobElementBehavior.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

FireEvacuationEvent::FireEvacuationEvent( )
{

}

FireEvacuationEvent::~FireEvacuationEvent()
{

}
int FireEvacuationEvent::process ( CARCportEngine* pEngine  )
{
	PLACE_METHOD_TRACK_STRING();
	TerminalMobElementBehavior::SetFireEvacuationTime( this->time );
	ProcessActivePerson( pEngine->getTerminal() );
	ProcessInActivePerson( pEngine->getTerminal() );
	return TRUE;
}

//process all person who still generate event
void FireEvacuationEvent::ProcessActivePerson( InputTerminal* _pInTerm )
{
	typedef std::pair<Person*,ElapsedTime> PersonTime;
	std::vector<Event*>vOtherEvent;
	std::vector<PersonTime>vNewEvent;

	while( m_pEventList->getCount() )
	{
		Event* pEvent = m_pEventList->getItem( 0 );
		m_pEventList->removeItem( 0 );
		if( pEvent->getEventType() == MobileMovement )
		{
			MobileElementMovementEvent* pMobileEvent = (MobileElementMovementEvent*)pEvent;
			MobileElement* pMobileElement = pMobileEvent->GetMobileElement();
			Person* pPerson = (Person*)pMobileElement;
			TerminalMobElementBehavior* spTerminalBehavior = pPerson->getTerminalBehavior();
			if (spTerminalBehavior == NULL)
			{
				continue;
			}

			//when person walk on moving side walk or in train,  let him complete this moving process
			// so we do not be interest in that kind of event.
			if( pPerson->getPreState() == OnMovingSideWalk )
			{
				vOtherEvent.push_back( pEvent );
				continue;
			}

			if( spTerminalBehavior->IfPersonInStationSystem() )			
			{
				vOtherEvent.push_back( pEvent );
				continue;
			}

			if( spTerminalBehavior->m_bEvacuationWhenInPipe )
			{
				pPerson->setState( EvacuationFire );
				spTerminalBehavior->SetFireEvacuateFlag( true );
				vOtherEvent.push_back( pEvent );
				continue;
			}

			Point ptCurPosition = CaculateCurPositionOfPerson( pMobileEvent->getTime(), pMobileElement );
			
			pPerson->setTerminalDestination( ptCurPosition );
			pPerson->writeLogEntry( this->time, false );

			long lSeconds = Default_Person_Response_Time;
			long lSpeed = Default_Person_Speed_Impact;
			const ProbabilityDistribution* pDistTime = _pInTerm->paxDataList->getResponseTime()->FindFit( pPerson->getType() );
			if( pDistTime )
			{
				lSeconds = (long)pDistTime->getRandomValue();
			}

			const ProbabilityDistribution* pDistSpeed = _pInTerm->paxDataList->getImpactEmer()->FindFit( pPerson->getType() );
			if( pDistSpeed )
			{
				lSpeed = (long)pDistSpeed->getRandomValue();
			}
			
			pPerson->setSpeed( pPerson->getSpeed() + lSpeed );
			pPerson->writeLogEntry( this->time + lSeconds, false );

			pPerson->setState( EvacuationFire );
			spTerminalBehavior->SetFireEvacuateFlag( true );
			spTerminalBehavior->DisablePersonOnStation();

			PersonTime temp;
			temp.first = pPerson;
			temp.second = this->time + (long)lSeconds;

			vNewEvent.push_back( temp );	
			delete pEvent;	
		}
		else if( pEvent->getEventType() == ElevatorMovingEvent )
		{
			vOtherEvent.push_back( pEvent );
		}
	}
	
	// generate new event to let person evacuate
	std::vector<PersonTime>::const_iterator iter = vNewEvent.begin();
	std::vector<PersonTime>::const_iterator iterEnd = vNewEvent.end();
	for( ; iter != iterEnd; ++iter )
	{
		Person* pPerson = iter->first;
		pPerson->generateEvent( iter->second,false );
	}

	std::vector<Event*>::const_iterator iterOther = vOtherEvent.begin();
	std::vector<Event*>::const_iterator iterOtherEnd = vOtherEvent.end();
	for( ; iterOther != iterOtherEnd; ++iterOther )
	{
		(*iterOther)->addEvent();
	}

}

// process all person who are inactive ( such as ,visitor wait in holding area for his owner, person wait in queue...)
void FireEvacuationEvent::ProcessInActivePerson( InputTerminal* _pInTerm )
{
	int iProcessorCount = _pInTerm->procList->getProcessorCount();
	for( int i=0; i<iProcessorCount; ++i )
	{
		Processor* pProc = _pInTerm->procList->getProcessor( i );
		if( pProc->getProcessorType() == HoldAreaProc  )
		{
			HoldingArea* pHoldArea = (HoldingArea*)pProc;
			SortedMobileElementList* pOccupants = pHoldArea->GetOccupants();
			int iPersonCount = pOccupants->getCount();
			for( int j=0; j<iPersonCount; ++j )
			{
				Person* pPerson = (Person*)pOccupants->getItem( j );
				TerminalMobElementBehavior* spTerminalBehavior = pPerson->getTerminalBehavior();
				if( spTerminalBehavior&&!spTerminalBehavior->IfAlreadyEvacuate() )
				{
					long lSeconds = Default_Person_Response_Time;
					long lSpeed = Default_Person_Speed_Impact;
					const ProbabilityDistribution* pDistTime = _pInTerm->paxDataList->getResponseTime()->FindFit( pPerson->getType() );
					if( pDistTime )
					{
						lSeconds = (long)pDistTime->getRandomValue();
					}

					const ProbabilityDistribution* pDistSpeed = _pInTerm->paxDataList->getImpactEmer()->FindFit( pPerson->getType() );
					if( pDistSpeed )
					{
						lSpeed = (long)pDistSpeed->getRandomValue();
					}
					pPerson->setSpeed( pPerson->getSpeed() + lSpeed );
					pPerson->setState( EvacuationFire );
					spTerminalBehavior->SetFireEvacuateFlag( true );
					pPerson->generateEvent( this->time + lSeconds,false);
				}
			}
		}/*
		else if( pProc->getProcessorType() == LineProc )
		{
			LineProcessor* pLineProc = (LineProcessor*)pProc;
			SortedMobileElementList* pOccupants = pLineProc->GetOccupants();
			int iPersonCount = pOccupants->getCount();
			for( int j=0; j<iPersonCount; ++j )
			{
				Person* pPerson = (Person*)pOccupants->getItem( j );
				if( !pPerson->IfAlreadyEvacuate() )
				{
					long lSeconds = Default_Person_Response_Time;
					long lSpeed = Default_Person_Speed_Impact;
					const ProbabilityDistribution* pDistTime = _pInTerm->paxDataList->getResponseTime()->FindFit( pPerson->getType() );
					if( pDistTime )
					{
						lSeconds = pDistTime->getRandomValue();
					}

					const ProbabilityDistribution* pDistSpeed = _pInTerm->paxDataList->getSpeedImpact()->FindFit( pPerson->getType() );
					if( pDistSpeed )
					{
						lSpeed = pDistSpeed->getRandomValue();
					}
					pPerson->setSpeed( pPerson->getSpeed() + lSpeed );
					pPerson->setState( EvacuationFire );
					pPerson->SetFireEvacuateFlag( true );
					pPerson->generateEvent( this->time + lSeconds);
				}
			}
		}*/
		else if( pProc->getProcessorType() == IntegratedStationProc )
		{
			IntegratedStation* pStation = ( IntegratedStation* )pProc;
			SortedMobileElementList* pOccupants = pStation->GetOccupants();
			int iPersonCount = pOccupants->getCount();
			for( int j=0; j<iPersonCount; ++j )
			{
				Person* pPerson = (Person*)pOccupants->getItem( j );
				TerminalMobElementBehavior* spTerminalBehavior = pPerson->getTerminalBehavior();
				if( spTerminalBehavior&&pPerson->getState() != OnVehicle && !spTerminalBehavior->IfAlreadyEvacuate() )
				{
					long lSeconds = Default_Person_Response_Time;
					long lSpeed = Default_Person_Speed_Impact;
					const ProbabilityDistribution* pDistTime = _pInTerm->paxDataList->getResponseTime()->FindFit( pPerson->getType() );
					if( pDistTime )
					{
						lSeconds = (long)pDistTime->getRandomValue();
					}

					const ProbabilityDistribution* pDistSpeed = _pInTerm->paxDataList->getImpactEmer()->FindFit( pPerson->getType() );
					if( pDistSpeed )
					{
						lSpeed = (long)pDistSpeed->getRandomValue();
					}
					pPerson->setSpeed( pPerson->getSpeed() + lSpeed );
					pPerson->setState( EvacuationFire );
					spTerminalBehavior->SetFireEvacuateFlag( true );
					pPerson->generateEvent( this->time + lSeconds ,false);
					spTerminalBehavior->DisablePersonOnStation();//leave station now
				}
			}
		}
		else
		{
			ProcessorQueue* pQueue = pProc->GetProcessorQueue();
			if( pQueue )
			{
				MobileElementList* pWaitList = pQueue->GetWaitList();
				int iCount = pWaitList->getCount();
				for( int j=0; j<iCount; ++j )
				{
					Person* pPerson = (Person*) pWaitList->getItem( j );
					TerminalMobElementBehavior* spTerminalBehavior = pPerson->getTerminalBehavior();
					if(spTerminalBehavior&&!spTerminalBehavior->IfAlreadyEvacuate() )
					{
						long lSeconds = Default_Person_Response_Time;
						long lSpeed = Default_Person_Speed_Impact;
						const ProbabilityDistribution* pDistTime = _pInTerm->paxDataList->getResponseTime()->FindFit( pPerson->getType() );
						if( pDistTime )
						{
							lSeconds = (long)pDistTime->getRandomValue();
						}

						const ProbabilityDistribution* pDistSpeed = _pInTerm->paxDataList->getImpactEmer()->FindFit( pPerson->getType() );
						if( pDistSpeed )
						{
							lSpeed = (long)pDistSpeed->getRandomValue();
						}
						pPerson->setSpeed( pPerson->getSpeed() + lSpeed );
						pPerson->setState( EvacuationFire );
						spTerminalBehavior->SetFireEvacuateFlag( true );
						pPerson->generateEvent( this->time + lSeconds ,false);
					}
				}
			}
		}
	}
}

//caculate the mid point of two log point
Point FireEvacuationEvent::CaculateCurPositionOfPerson( const ElapsedTime& _timeEvent , MobileElement* _pElement)
{
	Person* pPerson = (Person*)_pElement;
	Point locationPt,destPt;
	pPerson->getTerminalPoint(locationPt);
	pPerson->getTerminalDest(destPt);
	if( _timeEvent == time )
	{
		return destPt;
	}

	DistanceUnit dLength = ( _timeEvent.asSeconds() - time.asSeconds() ) * _pElement->getSpeed();
	Point ptCurrentPosition( destPt);
	
	if( ptCurrentPosition == locationPt )
	{
		return ptCurrentPosition;
	}
	
	Point ptVector( ptCurrentPosition, locationPt );
	ptVector.length( dLength );

	ptCurrentPosition += ptVector;

	ptCurrentPosition.setZ( destPt.getZ() );
	
	return ptCurrentPosition;
}
