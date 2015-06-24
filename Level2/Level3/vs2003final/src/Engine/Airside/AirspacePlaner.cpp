#include "StdAfx.h"
#include ".\airspaceplaner.h"
#include "./InputAirside/AirRoute.h"

#include "AirsideResourceManager.h"


bool AirspacePlaner::Init( int nPrjID, InputTerminal *pInterm, AirspaceResourceManager * pAirsideRes, CAirportDatabase* pAirportDatabase )
{
	m_pAirspaceRes = pAirsideRes;
	m_pAirportDatabase = pAirportDatabase;
	m_SID_STARAssignmentInSim.Init(nPrjID);

	//m_FltPlans.Read();
	
	return true;
}

double AirspacePlaner::GetPlanSpeed( FlightInAirsideSimulation *pFlight, AirWayPointInSim * pWaypoint )
{
	
	return 100.0;
}

FlightMode AirspacePlaner::GetFlightModeInResource( FlightInAirsideSimulation *pFlight, AirsideResource * resource ) const
{
	return OnTerminal;

}

double AirspacePlaner::GetPlanAltitude( FlightInAirsideSimulation *pFlight,AirWayPointInSim * pWaypoint )
{
	return 5000;
}

CAirRoute* AirspacePlaner::GetFlightSIDByRunwayPort(RunwayPortInSim* pRunwayPort, FlightInAirsideSimulation* pFlight)
{
	CAirRoute* pSID = NULL;
	int nRunwayID = pRunwayPort->GetRunwayInSim()->GetRunwayInput()->getID();
	int nRunwayMark = pRunwayPort->getRunwayPortType() == RUNWAYMARK_FIRST ? 0:1;
	int nSidID = m_SID_STARAssignmentInSim.GetSIDRoute(pFlight, nRunwayID,nRunwayMark , m_pAirportDatabase);

	AirRouteList _RouteList = m_pAirspaceRes->GetAirRouteList(); 
	int nCount = _RouteList.GetAirRouteCount();
	for ( int i =0; i < nCount; i++)
	{
		pSID = _RouteList.GetAirRoute(i);
		if ( pSID->getID() == nSidID)
			return pSID;
	}

	return NULL;

}

CAirRoute* AirspacePlaner::GetFlightSTARByRunwayPort(RunwayPortInSim* pRunwayPort, FlightInAirsideSimulation* pFlight)
{
	CAirRoute* pSTAR = NULL;
	int nRunwayID = pRunwayPort->GetRunwayInSim()->GetRunwayInput()->getID();
	int nRunwayMark = pRunwayPort->getRunwayPortType() == RUNWAYMARK_FIRST ? 0:1;
	int nStarID = m_SID_STARAssignmentInSim.GetSTARRoute(pFlight, nRunwayID,nRunwayMark , m_pAirportDatabase);

	AirRouteList _RouteList = m_pAirspaceRes->GetAirRouteList(); 
	int nCount = _RouteList.GetAirRouteCount();
	for ( int i =0; i < nCount; i++)
	{
		pSTAR = _RouteList.GetAirRoute(i);
		if ( pSTAR->getID() == nStarID)
			return pSTAR;
	}

	return NULL;

}
