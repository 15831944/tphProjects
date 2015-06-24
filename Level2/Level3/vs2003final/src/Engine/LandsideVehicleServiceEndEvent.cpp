#include "StdAfx.h"
#include ".\landsidevehicleserviceendevent.h"
#include "LandsideResidentVehicleInSim.h"

//
//LandsideVehicleServiceEndEvent::LandsideVehicleServiceEndEvent( LandsideVehicleInSim *pVehicle, const ElapsedTime& eEventTime )
//:m_pVehicle(pVehicle)
//,m_bClosed(false)
//{
//	setTime(eEventTime);
//}
//
//LandsideVehicleServiceEndEvent::~LandsideVehicleServiceEndEvent(void)
//{
//
//
//}
//
//int LandsideVehicleServiceEndEvent::process( CARCportEngine* pEngine )
//{
//	//LandsideResourceInSim* pResource = m_pVehicle->getState().getLandsideRes();
//	//if(pResource == NULL)
//	//	return 1;
//	//ASSERT(!IsEventClosed());
//
//	if(m_pVehicle && !IsEventClosed())
//	{
//		CloseEvent(true);
//		if(m_pVehicle->m_pServiceEndEvt==this)
//		{
//			m_pVehicle->m_pServiceEndEvt = NULL;
//			m_pVehicle->OnServiceTimeEnd(pEngine,getTime());
//		}
//	}
//	return FALSE;
//}
//
//int LandsideVehicleServiceEndEvent::kill( void )
//{
//	return 0;
//}
//
//const char * LandsideVehicleServiceEndEvent::getTypeName( void ) const
//{
//	return _T("VehicleServiceEndEvent");
//}
//
//int LandsideVehicleServiceEndEvent::getEventType( void ) const
//{
//	return Landside_VehicleServiceEndEvent;
//}
//
//bool LandsideVehicleServiceEndEvent::IsEventClosed()
//{
//	return m_bClosed;
//}
//
//void LandsideVehicleServiceEndEvent::CloseEvent( bool bClose )
//{
//	m_bClosed = bClose;
//}
