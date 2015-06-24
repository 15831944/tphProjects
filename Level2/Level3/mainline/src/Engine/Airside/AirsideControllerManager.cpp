#include "StdAfx.h"
#include ".\airsidecontrollermanager.h"
#include "FlightInAirsideSimulation.h"

AirsideControllerManager::AirsideControllerManager(void)
{
	m_airspaceController = new AirspaceController;
	m_gateController = new GateController;
	m_runwayController = new RunwayController;
	m_taxiwayController = new TaxiwayController;
}

AirsideControllerManager::~AirsideControllerManager(void)
{
	delete m_airspaceController; m_airspaceController= NULL;
	delete m_gateController; m_gateController = NULL;
	delete m_runwayController; m_runwayController = NULL;
	delete m_taxiwayController;m_taxiwayController = NULL;
}

ResourceOccupy AirsideControllerManager::FlightNextResourceAssignment(FlightInAirsideSimulation* pFlight,ResourceOccupy resource)
{
	if (pFlight->getFlightState() == OnCruise || pFlight->getFlightState() == Climbout 
		|| pFlight->getFlightState() == OnTerminal || pFlight->getFlightState() == OnApproach)
	{

			return m_airspaceController->ResourceAllocation(pFlight->GetCFlight(),resource);
	}

	if ( pFlight->getFlightState() == LandingOnRuwway || pFlight->getFlightState() == TakeoffOnRunway)
	{
		return m_runwayController->ResourceAllocation(pFlight->GetCFlight(),resource);
	}
	if ( pFlight->getFlightState() == TaxiwayIn || pFlight->getFlightState() == TaxiwayOut )
	{
		return m_taxiwayController->ResourceAllocation(pFlight->GetCFlight(),resource);
	}
	if ( pFlight->getFlightState() == OnGate)
	{
		return m_gateController->ResourceAllocation(pFlight->GetCFlight(),resource);
	}

	return resource;
}