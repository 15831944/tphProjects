#include "StdAfx.h"
#include "FlightMovementsToClearance.h"

#include "FlightMovementsOnResource.h"
#include "./FlightTakeoffOnRunway.h"
#include "./FlightTaxiingMovements.h"


void FlightMovementsToClearance::End( Clearance * pClearance )
{
	ASSERT(pClearance);	
	if(m_pFlight->GetFlightMode() == OnBirth)	
	{
		FlightBirthing birth(m_pFlight);
		birth.End(pClearance);
		m_vMovements.insert(m_vMovements.end(),birth.GetStateList().begin(),birth.GetStateList().end());
	}

	else if( m_pFlight->GetFlightMode() == OnLanding )
	{
		FlightLanding landing(m_pFlight);		
		landing.End(pClearance);
		m_vMovements.insert(m_vMovements.end(),landing.GetStateList().begin(),landing.GetStateList().end());
	}
	else if(m_pFlight->GetFlightMode() == OnTakeoff)
	{
		FlightTakeoff takeoff(m_pFlight);		
		takeoff.End(pClearance);
		m_vMovements.insert(m_vMovements.end(),takeoff.GetStateList().begin(),takeoff.GetStateList().end());
	}
	else if(m_pFlight->GetFlightMode() == OnPreTakeoff)
	{
		FlightPreTakeoff preTakeoff(m_pFlight);		
		preTakeoff.BeginState(m_pFlight->GetFlightState());
		preTakeoff.End(pClearance);
		m_vMovements.insert(m_vMovements.end(),preTakeoff.GetStateList().begin(),preTakeoff.GetStateList().end());
	}
	else if(m_pFlight->GetFlightMode() == OnFinalApproach)
	{
		FlightFinalApproach finalApproach(m_pFlight);		
		finalApproach.BeginState(m_pFlight->GetFlightState());
		finalApproach.End(pClearance);
		m_vMovements.insert(m_vMovements.end(),finalApproach.GetStateList().begin(),finalApproach.GetStateList().end());
	}
	else if(m_pFlight->IsOnTaxiing())
	{
		FlightTaxiing taxiing(m_pFlight);
		taxiing.BeginState(m_pFlight->GetFlightState());
		taxiing.End(pClearance);
		m_vMovements.insert(m_vMovements.end(),taxiing.GetStateList().begin(),taxiing.GetStateList().end());
	}
	else if(m_pFlight->GetFlightMode() == OnExitStand )
	{
		FlightPushOutStand pushOut(m_pFlight);
		pushOut.BeginState(m_pFlight->GetFlightState());
		pushOut.End(pClearance);
		m_vMovements.insert(m_vMovements.end(),pushOut.GetStateList().begin(),pushOut.GetStateList().end());
	}
	else if(m_pFlight->GetFlightMode() == OnHeldAtStand)
	{
		FlightHeldAtStand heldAtStand(m_pFlight);
		heldAtStand.End(pClearance);
		m_vMovements.insert(m_vMovements.end(),heldAtStand.GetStateList().begin(),heldAtStand.GetStateList().end());
	}
	else 
		FlightMovements::End(pClearance);

}


