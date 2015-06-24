#include "StdAfx.h"
#include ".\flightservicemanagerinsim.h"
#include "InputAirside/FlightTypeDetailsManager.h"
#include "Inputs/IN_TERM.H"

CFlightServiceManagerInSim::CFlightServiceManagerInSim(int nPrjID, InputTerminal * pInterm)
:m_nProjID(nPrjID)
,m_pInputTerminal(pInterm)
{
	m_pFlightServiceManager = new FlightTypeDetailsManager(pInterm->m_pAirportDB, nPrjID);
	m_pFlightServiceManager->ReadData();
}

CFlightServiceManagerInSim::~CFlightServiceManagerInSim(void)
{

	delete m_pFlightServiceManager;
	m_pFlightServiceManager = NULL;
}
