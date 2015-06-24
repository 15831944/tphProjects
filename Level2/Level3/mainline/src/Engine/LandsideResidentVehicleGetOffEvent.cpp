#include "StdAfx.h"
#include ".\landsideresidentvehiclegetoffevent.h"
#include "LandsideResidentVehicleInSim.h"

LandsideResidentVehicleGetOffEvent::LandsideResidentVehicleGetOffEvent(LandsideResidentVehicleInSim *pVehicle, const ElapsedTime& eEventTime)
:m_pResidentVehicleInSim(pVehicle)
{
	setTime(eEventTime);
}

LandsideResidentVehicleGetOffEvent::~LandsideResidentVehicleGetOffEvent(void)
{
}

int LandsideResidentVehicleGetOffEvent::process( CARCportEngine* pEngine )
{
	if (m_pResidentVehicleInSim)
	{
		m_pResidentVehicleInSim->SetPaxGetoffDoneTag(true);
	}

	return 0;
}

int LandsideResidentVehicleGetOffEvent::kill( void )
{
	return 0;
}

const char * LandsideResidentVehicleGetOffEvent::getTypeName( void ) const
{
	return _T("PaxGetoffVehicleEndEvent");
}

int LandsideResidentVehicleGetOffEvent::getEventType( void ) const
{
	return Landside_PaxGetoffEndEvent;
}

