#include "StdAfx.h"
#include ".\standplaner.h"
#include "./StandResourceManager.h"
#include "../../InputAirside/ALTObjectID.h"
#include "FlightInAirsideSimulation.h"
StandPlaner::StandPlaner(void)
{
}

StandPlaner::~StandPlaner(void)
{
}

AirsideResource* StandPlaner::ResourceAssignment(FlightInAirsideSimulation * pFlight)
{

	//AirportResourceManager * airportResource = getAirportResource();

	//GateInSim *gateInSim = airportResource->getGate();
	//
	

	
	StandInSim* pStand = GetPlanedStand(pFlight);
	if(pFlight->getFlightState() == Generate){
		if(pStand){
			pFlight->setFlightState(OnGate);
			pFlight->SetBackUp(false);
			return pStand;
		}
	}

	if(pFlight->getFlightState() == OnGate ){
		if(pStand){

			if(pFlight->IsDeparture()){
				pFlight->setFlightState(PushOutGate);
				pFlight->SetBackUp(pStand->m_OutWay.bPushBack);
				return &pStand->m_OutWay;
			}else{
				pFlight->setFlightState(Terminiate);
				return NULL;
			}
			
		}
	} 

	if(pFlight->getFlightState() == TaxiwayIn ){
		if(pStand){
			pFlight->setFlightState(PushInGate);
			pFlight->SetBackUp(false);
			return &pStand->m_InWay;
		}
	}

	if(pFlight->getFlightState() == PushInGate){
		if(pStand){
			pFlight->setFlightState(OnGate);
			pFlight->SetBackUp(false);
			return pStand;
		}
	}
 
	pFlight->setFlightState(Terminiate);
	return NULL;
}

bool StandPlaner::Init( int nPrjID, int nAirportID, StandResourceManager* pAirportRes )
{
	
	m_pStandRes = pAirportRes;
	return true;
}

StandInSim * StandPlaner::GetPlanedStand( FlightInAirsideSimulation * pFlight ) const
{
	ALTObjectID standID = pFlight->GetCFlight()->getStand();
	StandInSim * pStand = m_pStandRes->GetStand(standID);

	return pStand;
}