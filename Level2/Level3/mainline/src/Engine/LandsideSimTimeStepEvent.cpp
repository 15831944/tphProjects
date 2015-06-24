#include "StdAfx.h"
#include "LandsideSimTimeStepEvent.h"
#include "LandsideSimulation.h"
#include "LandsideSimTimeStepManager.h"
//extern tAwarenessTime;

LandsideSimTimeStepEvent::LandsideSimTimeStepEvent(const ElapsedTime& timeStep, LandsideSimTimeStepManager *pTimeStepManager)
:m_eTimeStep(timeStep)
,m_pTimeStepManager(pTimeStepManager)
{

}


int LandsideSimTimeStepEvent::getEventType( void ) const
{
	return EnumLandsideSimTimeStepEvent;
}

const char * LandsideSimTimeStepEvent::getTypeName( void ) const
{
	return "Landside Engine Time Step Event";
}

int LandsideSimTimeStepEvent::process( CARCportEngine *_pEngine )
{
	LandsideSimulation::tAwarenessTime = m_eTimeStep;
	if(m_pTimeStepManager)
		m_pTimeStepManager->ScheduleNextEvent();
	return TRUE;
}

int LandsideSimTimeStepEvent::kill( void )
{
	return 1;
}

ElapsedTime LandsideSimTimeStepEvent::removeMoveEvent(  )
{
	ElapsedTime aTime = -1l;	

	//int nCount = m_pEventList->getCount();

	//for( int i=0; i<nCount; i++ )
	//{
	//	Event* pEventInList = m_pEventList->getItem( nCount- i -1);	

	//	if( pEventInList->getEventType() == getEventType() && pEventInList->getTypeName()==getTypeName() )
	//	{
	//		LandsideVehicleEvent *pMoveEvent = (LandsideVehicleEvent*)pEventInList;
	//		if( pMoveEvent->mpVehicle == anElement)
	//		{
	//			aTime = pMoveEvent->time;				
	//			m_pEventList->deleteItem( nCount- i -1 );
	//			break;
	//		}
	//	}
	//}
	return aTime;
}

void LandsideSimTimeStepEvent::addEvent()
{
	__super::addEvent();
}

