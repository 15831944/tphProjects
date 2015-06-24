#include "StdAfx.h"
#include ".\towingserviceevent.h"
#include "TaxiRouteInSim.h"
#include "../../InputAirside/VehicleDefine.h"
#include "Clearance.h"
#include "TaxiwayResource.h"
#include "../../Common/elaptime.h"
#include "AirsideTowTruckInSim.h"
#include "StandInSim.h"
#include "StandLeadInLineInSim.h"
#include "StandLeadOutLineInSim.h"
#include "FlightPerformanceManager.h"
#include "SimulationErrorShow.h"
#include "AirTrafficController.h"
#include "AirsideResourceManager.h"
#include "AirspaceResourceManager.h"
#include "../../InputAirside/TowOffStandAssignmentData.h"
#include "FlightInHoldEvent.h"
#include "TowTruckServiceRequest.h"
#include "AirsideSimulation.h"
#include "TaxiOutboundRouteInSim.h"
#include "TaxiToReleasePointInSim.h"
#include "FlightPerformancesInSim.h"



TowingServiceEvent::TowingServiceEvent(CAirsideMobileElement* pMobileElement)
:CAirsideMobileElementEvent(pMobileElement)
{
	m_bWaiting = false;
	m_pTowTruck = ((AirsideFlightInSim*)m_pMobileElement)->GetTowingServiceVehicle();
}

TowingServiceEvent::~TowingServiceEvent(void)
{
}

int TowingServiceEvent::Process()
{
	if(((AirsideFlightInSim*)m_pMobileElement)->IsTowingServiceCompleted())
	{
		((AirsideFlightInSim*)m_pMobileElement)->SetTowingServiceVehicle(NULL);
		return TRUE;
	}

	if (((AirsideFlightInSim*)m_pMobileElement)->GetMode() == OnHeldAtStand)
	{
		ExitStand();
	}	
	if (((AirsideFlightInSim*)m_pMobileElement)->GetMode() == OnExitStand || ((AirsideFlightInSim*)m_pMobileElement)->GetMode() == OnTowToDestination)
	{
		bool bEnd = TowingToDestination();

		if (m_bWaiting)
			return TRUE;

		if (m_pTowTruck->GetReleasePoint())
		{
			if (bEnd && m_pTowTruck->GetReleasePoint()->GetType() == AirsideResource::ResType_Stand)
			{
				bEnd = EnterStand();
			}

			if (bEnd && m_pTowTruck->GetReleasePoint()->GetType() == AirsideResource::ResType_DeiceStation)
			{
				bEnd = EnterDeicePad();

				if (bEnd && !m_bWaiting)
					ServiceFinish();

				return TRUE;
				
			}

			if(bEnd && m_pTowTruck->GetReleasePoint()->GetType() == AirsideResource::ResType_LogicRunway)
			{
				//bEnd = EnterRunway();
				ServiceFinish();
				return TRUE;
			}
		}

		if (bEnd)		//already at release point
		{
			ServiceFinish();
			return TRUE;
		}
	}

	TowingServiceEvent* pEvent = new TowingServiceEvent(m_pMobileElement);
	pEvent->setTime(((AirsideFlightInSim*)m_pMobileElement)->GetTime());
	pEvent->addEvent();

	return TRUE;
}

bool TowingServiceEvent::EnterStand()
{
	AirsideFlightInSim* pFlight = (AirsideFlightInSim*)m_pMobileElement;
	StandInSim* pStand = (StandInSim*)m_pTowTruck->GetReleasePoint();

	ElapsedTime tParkingTime = -1L;
	if (pStand == pFlight->GetIntermediateParking())		//intermediate stand 
	{
		ElapsedTime tTime = pFlight->getDepTime() - pFlight->GetTowOffData()->getFltEnplaneTime() - ElapsedTime(600L);
		tParkingTime =  tTime > pFlight->GetTime() ? tTime : pFlight->GetTime();
	}
	else
		tParkingTime = pFlight->GetDepTime();

	ClearanceItem lastItem(pFlight->GetResource(),pFlight->GetMode(),pFlight->GetDistInResource());
	lastItem.SetSpeed(pFlight->GetSpeed());
	lastItem.SetTime(pFlight->GetTime());
	lastItem.SetPosition(pFlight->GetPosition());

	Clearance newClearance;
	pStand->GetEnterStandClearance(pFlight,lastItem,tParkingTime,newClearance);
	pFlight->WriteLog();
	pFlight->PerformClearance(newClearance);

	return true;
}

bool TowingServiceEvent::ExitStand()
{
	AirsideFlightInSim* pFlight = (AirsideFlightInSim*)m_pMobileElement;
	if (pFlight->GetResource() == NULL)
		return true;

	StandInSim * pParkingStand = NULL;
	if (pFlight->GetResource()->GetType() == AirsideResource::ResType_StandLeadInLine)
		pParkingStand = ((StandLeadInLineInSim*)pFlight->GetResource())->GetStandInSim();
	else if (pFlight->GetResource()->GetType() == AirsideResource::ResType_StandLeadOutLine)
		pParkingStand = ((StandLeadOutLineInSim*)pFlight->GetResource())->GetStandInSim();
	else
		pParkingStand = (StandInSim*)pFlight->GetResource();

	ClearanceItem lastItem(pFlight->GetResource(),pFlight->GetMode(),pFlight->GetDistInResource());
	lastItem.SetSpeed(pFlight->GetSpeed());
	lastItem.SetTime(pFlight->GetTime());
	lastItem.SetPosition(pFlight->GetPosition());

	Clearance newClearance;

	if (m_pTowTruck->GetServiceType() == TOWOPERATIONTYPE_PUSHBACKTOTAXIWAY || m_pTowTruck->GetServiceType() == TOWOPERATIONTYPE_PUSHANDTOW
		|| m_pTowTruck->GetServiceType() == TOWOPERATIONTYPE_PUSHBACKTORELEASEPOINT)
	{
		if (pParkingStand->IsPushbackStandForFlight(pFlight))
			pParkingStand->GetExitStandClearance(pFlight,lastItem,newClearance);
		else
			pParkingStand->GetPushFromStandClearance(pFlight,lastItem,newClearance);
	}

	if (m_pTowTruck->GetServiceType() == TOWOPERATIONTYPE_PULLANDTOW)
		pParkingStand->GetPullFromStandClearance(pFlight,lastItem,newClearance);


	pFlight->WriteLog();
	pFlight->PerformClearance(newClearance);

	return true;
}

bool TowingServiceEvent::TowingToDestination()
{
	AirsideFlightInSim* pFlight = ((AirsideFlightInSim*)m_pMobileElement);

	ClearanceItem lastItem(pFlight->GetResource(),pFlight->GetMode(),pFlight->GetDistInResource());
	lastItem.SetSpeed(pFlight->GetSpeed());
	lastItem.SetTime(pFlight->GetTime());
	lastItem.SetPosition(pFlight->GetPosition());
	lastItem.SetPushback(pFlight->GetCurState().m_bPushBack);

	if (m_pTowTruck->GetServiceType() == TOWOPERATIONTYPE_PUSHBACKTOTAXIWAY)
	{
		TaxiRouteInSim* pOutRoute = pFlight->GetAndAssignOutBoundRoute();
		if (pOutRoute)
		{
			Clearance newClearance;
			double taxispd = pFlight->GetPerformance()->getTaxiOutNormalSpeed(pFlight) ;
			pOutRoute->SetSpeed(taxispd);
			bool bEnd = pOutRoute->FindClearanceInConcern(pFlight,lastItem,20000,newClearance);
			if (newClearance.GetItemCount() >0)
			{
				pFlight->WriteLog();
				pFlight->PerformClearance(newClearance);
				pFlight->SetTowingServiceCompleted();
				return true;

			}
			//pFlight->SetTime(time);
			//pFlight->WriteLog();
			//pFlight->GetTowingServiceVehicle()->SetTime(time);

			m_bWaiting = true;
			return false;
		}

	}


	TaxiRouteInSim* pTowRoute = pFlight->GetTowingRoute();
	if (pTowRoute)
	{
		Clearance newClearance;
		double taxispd = pFlight->GetTowingSpeed() ;
		pTowRoute->SetSpeed(taxispd);
		bool bEnd = pTowRoute->FindClearanceInConcern(pFlight,lastItem,20000,newClearance);

		if (bEnd && newClearance.GetItemCount() ==0)
		{			
			m_bWaiting = true;
			return false;
		}

		if (newClearance.GetItemCount() > 0)
		{
			pFlight->WriteLog();
			m_pTowTruck->WirteLog(m_pTowTruck->GetPosition(),m_pTowTruck->GetSpeed(),m_pTowTruck->GetTime());
			pFlight->PerformClearance(newClearance);
		}



		//if (m_pTowTruck->GetServiceType() == TOWOPERATIONTYPE_PUSHBACKONLY 
		//	&& pFlight->GetPreState().m_pResource->GetType() == AirsideResource::ResType_StandLeadOutLine && pFlight->GetResource()->GetType() == AirsideResource::ResType_TaxiwayDirSeg)
		//{
		//	pFlight->SetTowingServiceCompleted();
		//	return true;
		//}


		if (bEnd)
			return false;

		return true;
	}
	

	//no available taxi route, show error message
	CString strWarn;
	if (m_pTowTruck->GetReleasePoint())
	{
		strWarn.Format("There is no available Towing route for flight from %s to %s ",pFlight->GetResource()->PrintResource(), m_pTowTruck->GetReleasePoint()->PrintResource());
	}
	else
	{
		strWarn.Format("There is no release point for flight. You should define release point for flight in Push back & tow operation specification");
	}
	CString strError = _T("DEFINE ERROR");
	AirsideSimErrorShown::SimWarning(pFlight,strWarn,strError);

	return true;
}

bool TowingServiceEvent::EnterDeicePad()
{
	AirsideFlightInSim* pFlight = ((AirsideFlightInSim*)m_pMobileElement);
	FlightDeiceStrategyDecision& deiceStragy = pFlight->GetDeiceDecision();

	ClearanceItem lastItem(pFlight->GetResource(),pFlight->GetMode(),pFlight->GetDistInResource());
	lastItem.SetSpeed(pFlight->GetSpeed());
	lastItem.SetTime(pFlight->GetTime());
	lastItem.SetPosition(pFlight->GetPosition());

	if(lastItem.GetResource() != deiceStragy.m_deicePlace ) //enter the deice place
	{
		AirsideFlightInSim* pInResFlight = deiceStragy.m_deicePlace->GetLastInResourceFlight() ;
		if( pInResFlight)
		{
			((AirsideFlightInSim*)m_pMobileElement)->DependOnAgents(pInResFlight);
			pFlight->SetTime(time);
			pFlight->GetTowingServiceVehicle()->SetTime(time);
			m_bWaiting = true;
			return false;
		}
		else
		{
			pFlight->GetTowingRoute()->FlightExitRoute(pFlight,lastItem.GetTime());
			ClearanceItem atDeiceplaceItem(deiceStragy.m_deicePlace, OnTaxiToDeice,deiceStragy.m_DistInResource);
			ElapsedTime dT = ((AirsideFlightInSim*)m_pMobileElement)->GetTimeBetween(lastItem,atDeiceplaceItem);
			atDeiceplaceItem.SetTime( lastItem.GetTime() + dT);
			lastItem = atDeiceplaceItem;
			
			Clearance newClearance;
			newClearance.AddItem(lastItem);
			pFlight->WriteLog();
			pFlight->PerformClearance(newClearance);
		}
	}
	return true;
}

bool TowingServiceEvent::EnterRunway()
{
	AirsideFlightInSim* pFlight = ((AirsideFlightInSim*)m_pMobileElement);
	AirRouteNetworkInSim* pRouteNet = pFlight->GetAirTrafficController()->GetAirsideResourceManager()->GetAirspaceResource();
	if (pRouteNet == NULL)
		return true;

	ClearanceItem lastItem(pFlight->GetResource(),pFlight->GetMode(),pFlight->GetDistInResource());
	lastItem.SetSpeed(pFlight->GetSpeed());
	lastItem.SetTime(pFlight->GetTime());
	lastItem.SetPosition(pFlight->GetPosition());
	Clearance newClearance;

	pRouteNet->FindClearanceInConcern(pFlight,pFlight->GetSID(),lastItem,newClearance);

	if (pFlight->GetMode() != OnTakeOffWaitEnterRunway )
	{
		pFlight->WriteLog();
		pFlight->PerformClearance(newClearance);
		pFlight->WakeUp(pFlight->GetTime());
	}

	return true;
}

void TowingServiceEvent::ServiceFinish()
{
	AirsideFlightInSim* pFlight = (AirsideFlightInSim*)m_pMobileElement;
	if (pFlight->GetResource()->GetType() == AirsideResource::ResType_TaxiwayDirSeg)
	{
		if (m_pTowTruck->GetReleasePoint() && (m_pTowTruck->GetReleasePoint()->GetType() == AirsideResource::ResType_StartPosition 
			|| m_pTowTruck->GetReleasePoint()->GetType() == AirsideResource::ResType_InterruptLine) && pFlight->GetCurState().m_bPushBack)
		{
			TaxiwayDirectSegInSim* pCurSeg = NULL;

			pCurSeg = ((TaxiwayDirectSegInSim*)pFlight->GetResource())->GetOppositeSegment();
			double dDist = pCurSeg->GetEndDist() - pFlight->GetDistInResource();

			pFlight->GetCurState().m_pResource = pCurSeg;
			pFlight->GetCurState().m_dist = dDist;
			pFlight->GetCurState().m_bPushBack = false;
		}

		pFlight->SetMode(OnTaxiToRunway);
	}

	pFlight->SetTowingServiceCompleted();

	ElapsedTime tTime = pFlight->GetTime();
	ASSERT( tTime >= time);
	ElapsedTime tHookTime = pFlight->GetUnhookTime();


	RunwayExitInSim* pTakeoffPos = pFlight->GetTakeoffPosition();
	TaxiToReleasePointRouteInSim* pTowRotue = (TaxiToReleasePointRouteInSim *)pFlight->GetTowingRoute();
	
	if(pTakeoffPos && pTowRotue && !pTowRotue->GetPostRoute().empty() )
	{
		TaxiOutboundRouteInSim* pOutRoute = new TaxiOutboundRouteInSim(OnTaxiToRunway,m_pTowTruck->GetReleasePoint(),pTakeoffPos->GetLogicRunway());
		bool bCyclicGroundRoute=pFlight->GetEngine()->GetAirsideSimulation()->AllowCyclicGroundRoute();
		pOutRoute->AddTaxiwaySegList(pTowRotue->GetPostRoute(),bCyclicGroundRoute);
		pFlight->SetOutBoundRoute(pOutRoute);
	}
	else
	{
		pFlight->GetAirTrafficController()->AssignOutboundRoute(m_pTowTruck->GetReleasePoint(), pFlight);
	}

	AirsideFlightState curState = pFlight->GetCurState();
	curState.m_tTime = tTime + tHookTime;
	pFlight->UpdateState(curState,false);


	FlightWakeupEvent* pEvent = new FlightWakeupEvent(pFlight);
	pEvent->setTime(tTime + tHookTime);
	pEvent->addEvent();

	m_pTowTruck->GetCompleteReturnRouteToPool();
	m_pTowTruck->WakeUp(tTime+ ElapsedTime(tHookTime.asSeconds()*0.7));


}
