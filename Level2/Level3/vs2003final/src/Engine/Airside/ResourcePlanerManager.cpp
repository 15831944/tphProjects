#include "StdAfx.h"
#include ".\resourceplanermanager.h"
#include "FlightInAirsideSimulation.h"


#include "AirportResource.h"


ResourcePlanerManager::ResourcePlanerManager(void)
{
}

ResourcePlanerManager::~ResourcePlanerManager(void)
{
}

void ResourcePlanerManager::setAirportResource(AirportResource _airportResource)
{
	m_pAirportResource = _airportResource;
}

AirsideResource* ResourcePlanerManager::NextResourceAssign(FlightInAirsideSimulation* pFlight)
{
	if (pFlight->getFlightState() == FlightGenerate )
	{
		if (pFlight->getFlightMode() == 'D')
		{
		  return m_taxiwayPlaner.ResourceAssignment(pFlight);
		}
		else
		{
			return m_airspacePlaner.ResourceAssignment(pFlight);
		}
	}

	if (pFlight->getFlightState() == OnCruise || pFlight->getFlightState() == Climbout 
		||pFlight->getFlightState() == OnTerminal || pFlight->getFlightState() == TakeoffOnRunway)
	{
		return m_airspacePlaner.ResourceAssignment(pFlight);
	}
	if (pFlight->getFlightState() == OnApproach )
	{
		return m_runwayPlaner.ResourceAssignment(pFlight);
	}
	if (pFlight->getFlightState() == LandingOnRuwway || pFlight->getFlightState() == TaxiwayIn 
		|| pFlight->getFlightState() == TaxiwayOut || pFlight->getFlightState() == OnGate)
	{
		return m_taxiwayPlaner.ResourceAssignment(pFlight);
	}

	return NULL;

}