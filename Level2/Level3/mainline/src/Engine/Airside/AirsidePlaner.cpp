#include "StdAfx.h"


#include "AirsidePlaner.h"

#include "AirsideResourceManager.h"
#include "FlightInAirsideSimulation.h"

bool AirsidePlaner::Init( int nPrjID,  InputTerminal *pInterm, AirsideResourceManager * pAirsideRes ,,CAirportDatabase *pAirportDatabase  )
{
	
	if( ! m_AirspacePlaner.Init(nPrjID,pInterm,pAirsideRes) ) return false;

	
	for(AirsideResourceManager::AirportResourceList::iterator itr = pAirsideRes->m_vAirportRes.begin(); itr!= pAirsideRes->m_vAirportRes.end();itr++){
		if(! m_vAirportPlaner[(*itr).first].Init( nPrjID,itr->first, &(*itr).second, pAirportDatabase ) ) return false; 
	}

	return true;
}

AirsideResource* AirsidePlaner::ResourceAssignment( FlightInAirsideSimulation* pFlight )
{
	
	AirspacePlaner * pAirspacePlaner = &m_AirspacePlaner;

	AirportPlaner * pAirportPlaner =& m_vAirportPlaner.begin()->second;

	FlightState fltState = pFlight->getFlightState();

	if(fltState == Generate){
		if(!pFlight->IsArrival()) return pAirportPlaner->ResourceAssignment(pFlight);
		else return pAirspacePlaner->ResourceAssignment(pFlight);
	}
	if(fltState == Terminiate ){ return NULL; }

	if(fltState == OnCruiseA || fltState == OnTerminal || fltState == TakeoffOnRunway || fltState == Climbout || fltState == OnCruiseD ){
		return pAirspacePlaner->ResourceAssignment(pFlight);
	}
	else{
		return pAirportPlaner->ResourceAssignment(pFlight);
	}



	return NULL;
}

AirspacePlaner  * AirsidePlaner::GetAirspacePlaner()
{
	return & m_AirspacePlaner;
}

AirportPlaner * AirsidePlaner::GetAirportPlaner( int nAirportID )
{
	if(nAirportID < 0 && m_vAirportPlaner.size() >0 ) {
		return &m_vAirportPlaner.begin()->second;
	}

	if( m_vAirportPlaner.end() == m_vAirportPlaner.find(nAirportID) ) return NULL;
	
	
	return &m_vAirportPlaner[nAirportID];
}


double AirsidePlaner::getFlightPlanSpeedInWaypoint(FlightInAirsideSimulation *pFlight, int id)
{
	AirspacePlaner * pAirspacePlaner = &m_AirspacePlaner;
	return pAirspacePlaner->getFlightPlanedSpeed(pFlight,id);
}
	
