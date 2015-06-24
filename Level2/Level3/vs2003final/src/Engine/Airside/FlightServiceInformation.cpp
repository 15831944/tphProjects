#include "StdAfx.h"
#include ".\flightserviceinformation.h"
#include "AirsideFlightInSim.h"

CFlightServiceInformation::CFlightServiceInformation(AirsideFlightInSim* pFlightInSim)

{
	m_fltServiceRoute = new CFlightServiceRoute(pFlightInSim);
}

CFlightServiceInformation::~CFlightServiceInformation(void)
{
}

void CFlightServiceInformation::SetFltServiceLocations(const FlightTypeServiceLoactions& fltServiceLocations)
{
//	m_fltServiceLocations = fltServiceLocations;
	m_fltServiceRoute->SetFltServiceLocations(fltServiceLocations);
}
void CFlightServiceInformation::SetRingRoute(const RingRoad& ringRoad)
{
	m_fltServiceRoute->SetRingRoute(ringRoad);
}

int CFlightServiceInformation::GetFltServicePointCount(int nVehicleTypeID)
{
	int nCount = m_fltServiceRoute->GetFltServicePointCount(nVehicleTypeID);
	return nCount;
}