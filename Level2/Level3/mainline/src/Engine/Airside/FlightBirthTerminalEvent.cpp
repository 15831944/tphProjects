#include "StdAfx.h"
#include "FlightBirthTerminalEvent.h"
#include "AirsideFlightInSim.h"
#include "./StandResourceManager.h"
#include "./StandLeadInLineInSim.h"
#include "AirTrafficController.h"
#include "AirsideResourceManager.h"
#include "StandLeadOutLineInSim.h"
#include "DepatureFlightTerminalEvent.h"
#include "FlightTerminateEvent.h"

int FlightBirthTerminalEvent::Process()
{
	m_pCFlight->StartLogging();
	m_pCFlight->InitStatus();
	m_pCFlight->SetTime(time);

	Clearance theClearance;
	//go to stand straightly

	ElapsedTime tEndTime = time;
	
	if (m_pCFlight->IsArrival())		//only arrival or transfer flight
	{
		ALTObjectID standID = m_pCFlight->getArrStand();
		StandInSim * pStand = m_pStandResource->GetStandByName(standID);
		ElapsedTime tServiceTime = m_pCFlight->getServiceTime(ARR_PARKING);
		
		m_pCFlight->SetParkingStand(pStand, ARR_PARKING);

		if(GetStandClearence(pStand,time, m_pCFlight->GetArrTime()+tServiceTime, theClearance))
			tEndTime = m_pCFlight->GetArrTime()+tServiceTime;

	
		if (!m_pCFlight->IsTransfer())
		{
			//terminate
			/*ClearanceItem teminatItem(NULL,OnTerminate, 0);
			teminatItem.SetTime(tEndTime);
			theClearance.AddItem(teminatItem);*/
			AirsideFlightTerminateEvent* pEvent = new AirsideFlightTerminateEvent(m_pCFlight);
			pEvent->setTime(tEndTime);
			pEvent->addEvent();
		}
	}

	if (m_pCFlight->IsTransfer())
	{
		DepatureFlightTerminalEvent* pEvent = new DepatureFlightTerminalEvent(m_pCFlight, m_pStandResource);
		pEvent->setTime(tEndTime);
		pEvent->addEvent();
	}

	if (!m_pCFlight->IsArrival() && m_pCFlight->IsDeparture())		//only departure
	{
		ALTObjectID standID = m_pCFlight->getDepStand();
		StandInSim * pStand = m_pStandResource->GetStandByName(standID);
		if(pStand)
		{
			m_pCFlight->SetParkingStand(pStand,DEP_PARKING);
			ElapsedTime eDepEnplanTime = m_pCFlight->GetDepTime()-m_pCFlight->getServiceTime(DEP_PARKING);

			DepatureFlightTerminalEvent* pEvent = new DepatureFlightTerminalEvent(m_pCFlight, m_pStandResource);
			pEvent->setTime(eDepEnplanTime);
			pEvent->addEvent();
		}
	}
	
	


	m_pCFlight->PerformClearance(theClearance);
	return TRUE;


}

bool FlightBirthTerminalEvent::GetStandClearence(StandInSim* pStand,const ElapsedTime& tStartTime, const ElapsedTime& tEndTime,	Clearance& theClearance)
{
	if(pStand)
	{
		StandLeadInLineInSim* pLeadInLine = pStand->AssignLeadInLine(m_pCFlight);
		double dhalfFlightLen = m_pCFlight->GetLength()*0.5;

		if( pLeadInLine ) //held at the lead in line
		{			
			ClearanceItem atStandItem(pLeadInLine, OnHeldAtStand, pLeadInLine->GetEndDist()-2-dhalfFlightLen);
			atStandItem.SetSpeed(0);
			atStandItem.SetTime( tStartTime );			
			theClearance.AddItem(atStandItem);

			atStandItem.SetDistInResource( pLeadInLine->GetEndDist() -dhalfFlightLen -1);
			theClearance.AddItem(atStandItem);

			atStandItem.SetDistInResource( pLeadInLine->GetEndDist() -dhalfFlightLen);
			atStandItem.SetTime(tEndTime);
			theClearance.AddItem(atStandItem);
		}
		else
		{
			ClearanceItem atStandItem(pStand, OnHeldAtStand, -1);
			atStandItem.SetSpeed(0);
			atStandItem.SetTime(tStartTime);			
			theClearance.AddItem(atStandItem);

			atStandItem.SetTime(tStartTime);
			atStandItem.SetDistInResource(0);
			theClearance.AddItem(atStandItem);

 			atStandItem.SetTime(tEndTime);
 			theClearance.AddItem(atStandItem);
		}
		return true;

	}

	return false;
}

FlightBirthTerminalEvent::FlightBirthTerminalEvent( AirsideFlightInSim* flt, StandResourceManager* pStandres) 
:CAirsideMobileElementEvent(flt)
{
	m_pCFlight = flt;
	m_pStandResource = pStandres;


	//------------------------------------------------------------------------------------------
	//Terminal Mode Flight Delay Rule.(invoked when sim terminal mode only!!!)
	//impact flight delay data to flight lifecycle.
	Flight* pTermFlight = m_pCFlight->GetFlightInput();
	ElapsedTime arrTime = 0L;
	ElapsedTime depTime = 0L;
	ElapsedTime arrDelayTime = pTermFlight->getArrDelay();
	ElapsedTime depDelayTime = pTermFlight->getDepDelay();

	if(pTermFlight->isArriving()) // arrival or turnaround flight
		arrTime = max( ElapsedTime(0L), pTermFlight->getArrTime() + arrDelayTime );
	else // departure flight
		arrTime = max( ElapsedTime(0L), pTermFlight->getDepTime() - pTermFlight->getServiceTime() 
		+ depDelayTime );

	if(pTermFlight->isDeparting())// departure or turnaround flight
		depTime = max( ElapsedTime(0L),pTermFlight->getDepTime() + depDelayTime );
	else // arrival flight
		depTime = max( ElapsedTime(0L), pTermFlight->getArrTime() + pTermFlight->getServiceTime()
		+ arrDelayTime );

	m_pCFlight->SetArrTime( arrTime );
	m_pCFlight->SetDepTime( depTime );

	//------------------------------------------------------------------------------------------

}
