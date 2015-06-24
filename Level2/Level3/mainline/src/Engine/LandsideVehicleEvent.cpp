#include "StdAfx.h"
#include ".\landsidevehicleevent.h"
#include "LandsideVehicleInSim.h"

LandsideVehicleEvent::LandsideVehicleEvent( LandsideVehicleInSim* pVehicle )
:mpVehicle(pVehicle)
{

}


int LandsideVehicleEvent::getEventType( void ) const
{
	return VehicleMove;
}

const char * LandsideVehicleEvent::getTypeName( void ) const
{
	return "Landside Vehicle Move";
}

int LandsideVehicleEvent::process( CARCportEngine *_pEngine )
{
	if(mpVehicle)
	{
		mpVehicle->OnActive(_pEngine);
		return TRUE;
	}
	return FALSE;
}

int LandsideVehicleEvent::kill( void )
{
	return 1;
}

ElapsedTime LandsideVehicleEvent::removeMoveEvent( LandsideVehicleInSim *anElement )
{
	ElapsedTime aTime = -1l;	

	int nCount = m_pEventList->getCount();

	for( int i=0; i<nCount; i++ )
	{
		Event* pEventInList = m_pEventList->getItem( nCount- i -1);	

		if( pEventInList->getEventType() == getEventType() && pEventInList->getTypeName()==getTypeName() )
		{
			LandsideVehicleEvent *pMoveEvent = (LandsideVehicleEvent*)pEventInList;
			if( pMoveEvent->mpVehicle == anElement)
			{
				aTime = pMoveEvent->time;				
				m_pEventList->deleteItem( nCount- i -1 );
				break;
			}
		}
	}
	return aTime;
}

void LandsideVehicleEvent::addEvent()
{
	/*int nCount = m_pEventList->getCount();
	for( int i=0; i<nCount; i++ )
	{
		Event* pEventInList = m_pEventList->getItem( i );	

		if( pEventInList->getEventType() == getEventType() && pEventInList->getTypeName()==getTypeName() )
		{
			LandsideVehicleEvent *pMoveEvent = (LandsideVehicleEvent*)pEventInList;
			if( pMoveEvent->mpVehicle == mpVehicle)
			{
				if(pMoveEvent->getTime()<=getTime())
					return;
				else
				{
					m_pEventList->removeItem(i);					
					break;
				}
			}
		}
	}	*/
	__super::addEvent();
}

