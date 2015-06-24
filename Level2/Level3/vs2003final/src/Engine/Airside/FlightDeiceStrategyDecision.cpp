#include "stdafx.h"


#include "FlightDeiceStrategyDecision.h"


FlightDeiceStrategyDecision::FlightDeiceStrategyDecision()
{
	m_deicePlace = NULL;
	m_DistInResource = 0;
	mtype = DepStand_Type;    //
	mPosMethod = POWER_IN;
	mEngState = Off;  //when deicing turn engine on /off
	m_deiceRoute = NULL; 
	m_InspecAndHoldTime = ElapsedTime(0L);
	bAfterDeice= false;

	m_pDeicepadGroup = NULL;
	m_pDeicepadGroupEntry = NULL;
	m_deicePlace = NULL;

}
