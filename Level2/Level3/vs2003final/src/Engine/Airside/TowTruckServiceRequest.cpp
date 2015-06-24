#include "StdAfx.h"
#include ".\towtruckservicerequest.h"
#include "TaxiRouteInSim.h"
#include "AirsideResource.h"

TowTruckServiceRequest::TowTruckServiceRequest(TOWOPERATIONTYPE type, AirsideFlightInSim* pFlight)
:VehicleServiceRequest()
,m_ServiceType(type)
,m_ReturnRoute()
{
	m_pServiceFlight = pFlight;
	m_pReleasePoint = NULL;
	m_pOutboundRoute = NULL;
}

TowTruckServiceRequest::~TowTruckServiceRequest(void)
{
}
