#include "StdAfx.h"
#include "flightinairsidesimulation.h"
#include "../../Inputs/FlightPlans.h"
#include "../../InputAirside/PerformanceTakeoffs.h"
#include "../../AirsideInput/AirsideInput.h"
#include "../../InputAirside/PerformLandings.h"
#include "../../InputAirside/DepClimbOut.h"
#include "../../InputAirside/TaxiInbound.h"
#include "../../InputAirside/TaxiOutbound.h"
#include "../../InputAirside/ApproachLand.h"
#include "../../Common/ARCUnit.h"
#include "ResourceEntryEvent.h"
#include "../../Results/OutputAirside.h"
#include "AirsidePlaner.h"
#include "AirsideController.h"
#include "FlightMovementEvent.h"
#include "FlightGetClearanceEvent.h"



#include "FlightMovementsToClearance.h"

FlightInAirsideSimulation::FlightInAirsideSimulation( Flight* fltschedule ) : m_pflight(fltschedule) ,m_pOutput(NULL)
{	
	m_CurrentState.m_fltMode = OnBirth;
	m_pRunwayExit = NULL;
}

FlightInAirsideSimulation::~FlightInAirsideSimulation(void)
{
	
}

ElapsedTime FlightInAirsideSimulation::getFlightBirthTime()
{
	if( IsArrival() ){
		return GetCFlight()->getArrTime();
	}

	else {
		return GetCFlight()->getDepTime() - GetCFlight()->getServiceTime();
	}
}



void FlightInAirsideSimulation::SetOutput( OutputAirside* pOutput ){ m_pOutput = pOutput; }

OutputAirside * FlightInAirsideSimulation::GetOutput() const{ return m_pOutput; }



void FlightInAirsideSimulation::WriteLog( const FlightState& fltstat ) const
{
	AirsideFlightEventStruct eventStruct;

	if(fltstat.m_fltMode == OnBirth 
		|| fltstat.m_fltMode == OnTerminate)
		return;
	
	//ASSERT(fltstat.m_pResource.get());
	//if(!fltstat.m_pResource.get())return;

    Point pos = fltstat.m_pPosition;
	ARCVector3 dir = fltstat.m_vDirection;

	eventStruct.x =(float) pos.getX();
	eventStruct.y =(float) pos.getY();
	eventStruct.z = (float) pos.getZ();
	eventStruct.dirx = (float) dir[VX];
	eventStruct.diry = (float) dir[VY];
	eventStruct.dirz = (float) dir[VZ];

	eventStruct.time = fltstat.m_tTime;
	eventStruct.IsBackUp = false;
	
	m_pOutput->LogFlightEvent(GetCFlight(),eventStruct);
}

void FlightInAirsideSimulation::Move( const FlightState& fltState )
{
	WriteLog(fltState);
}




void FlightInAirsideSimulation::RequestNextClearance()
{
	Clearance * pNewClearance = NULL;
	if( pNewClearance = m_pTrafficController->AssignNextClearance(this) )
	{
		//m_pNextClearance = pNewClearance;
		ElapsedTime tTime = GetCurrentTime();
		try
		{	
			PerformMovementsToClearance(pNewClearance);
		}
		catch (CException* except)
		{
			delete except;
			ASSERT(FALSE);	
		}

		FlightGetClearanceEvent * pNewEvent = new FlightGetClearanceEvent(this);
		pNewEvent->setTime(GetCurrentTime());
		pNewEvent->addEvent();
	}	

}


bool FlightInAirsideSimulation::IsOnTaxiing() const
{
	if(!m_CurrentState.m_pResource.get()) return false;
	if(m_CurrentState.m_pResource->GetType() == AirsideResource::ResType_TaxiwayDirSeg) return true;
	if(m_CurrentState.m_pResource->GetType() == AirsideResource::ResType_IntersectionNode) return true;
	return false;
}
//default value means no constrain to the flight
Clearance::Clearance() :m_tTime(-1L),m_pResource(NULL)
{
	m_vSpeed = 0;
	m_dDist = 0;
	m_dAlt = 0;
}

Clearance::Clearance( const FlightState& fltstate )
{
	//m_dAlt = fltstate.m_dAlt;
	m_pResource = (AirsideResource*)fltstate.m_pResource.get();
	m_tTime = fltstate.m_tTime;
	m_vSpeed = fltstate.m_vSpeed;
	m_dDist = fltstate.m_dist;
	m_nMode = fltstate.m_fltMode;
}

FlightState::FlightState( const Clearance * pClearance )
{
	ASSERT(pClearance);
	if(!pClearance)return;
	m_tTime = pClearance->m_tTime;
	m_dist = pClearance->m_dDist;
	m_fltMode = pClearance->m_nMode;
	m_pResource = pClearance->m_pResource;
	m_vSpeed = pClearance->m_vSpeed;
	if(m_pResource.get())
	{
		m_pPosition = m_pResource->GetDistancePoint(m_dist);
	}
	m_pPosition.setZ( pClearance->m_dAlt );	
	m_vDirection = ARCVector3(1,0,0);
}

FlightState::FlightState()
{
	m_vSpeed = 0 ; m_dist = 0; m_fltMode = OnBirth; 
	//m_dAlt = 0; 
	m_vDirection = ARCVector3(1,0,0);
}

Point FlightState::GetPosition() const
{
	return m_pPosition;
}


//perform the movements accord to the new clearance
ElapsedTime FlightInAirsideSimulation::PerformMovementsToClearance(Clearance * pClearance)
{

	AirsideResource * pPreResource = GetCurrentResource();

	FlightMovementsToClearance flightMovementsToClearance(this);
	//flightMovementsToClearance.BeginState(GetFlightState());
	flightMovementsToClearance.End(pClearance);

	int nMoveCnt = flightMovementsToClearance.GetStateCount();
	ASSERT(nMoveCnt>0);
	
	FlightMovementEvent  * pMoveEvent = NULL;
	for(int i=0 ;i< nMoveCnt;i++)
	{
		pMoveEvent = new FlightMovementEvent(this, flightMovementsToClearance.GetState(i));
		pMoveEvent->addEvent();		
	}
	
	m_CurrentState =  flightMovementsToClearance.GetEndState();
	

	AirsideTrafficController::ModifyOcuppyTable(this,pPreResource,GetCurrentResource(),GetCurrentTime());

	return GetCurrentTime(); 
}

PlanGroundRoute& FlightInAirsideSimulation::GetCurrentRoute()
{
	if(GetFlightMode() < OnHeldAtStand)
		return GetPlanningFlow().GetInBoundRoute();
	else 
		return GetPlanningFlow().GetOutBoundRoute();
}