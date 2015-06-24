#include "StdAfx.h"


#include "AirportPlaner.h"
#include "FlightInAirsideSimulation.h"

#include "AirportResourceManager.h"
bool AirportPlaner::Init( int nPrjID,int nAirportID, AirportResourceManager* pAirportRes, ,CAirportDatabase *pAirportDatabase  )
{
	m_pAirportRes = pAirportRes;
	//m_TaxiwayPlaner.Init(nPrjID,nAirportID,&pAirportRes->m_TaxiwayRes);
	m_RunwayPlaner.Init(nPrjID,nAirportID,&pAirportRes->m_RunwayRes, pAirportDatabase);
	m_StandPlaner.Init(nPrjID,nAirportID,&pAirportRes->m_StandRes);
	
	//m_TaxiwayPlaner.m_pAirportPlaner = this;
	return true;
}

AirsideResource* AirportPlaner::ResourceAssignment( FlightInAirsideSimulation* pFlight )
{


	if(pFlight->getFlightState() == Generate && (!pFlight->IsArrival()) ){
		return m_StandPlaner.ResourceAssignment(pFlight);
	}
	

	if(pFlight->getFlightState() == OnApproach || pFlight->getFlightState() == ExitTaxiway  || pFlight->getFlightState() == PreTakeOff || pFlight->getFlightState() == LandingOnRuwway ){
		return m_RunwayPlaner.ResourceAssignment(pFlight);
	}

	//if(pFlight->getFlightState() == ExitRunway || pFlight->getFlightState()== PushOutGate || pFlight->getFlightState() == TaxiwayIn || pFlight->getFlightState() == TaxiwayOut)
	//{
	//	return m_TaxiwayPlaner.ResourceAssignment(pFlight);
	//}

	if(pFlight->getFlightState() == PushInGate || pFlight->getFlightState() == OnGate ){
		return m_StandPlaner.ResourceAssignment(pFlight);
	}


	return NULL;
}

AirportPlaner::AirportPlaner()
{
	
}