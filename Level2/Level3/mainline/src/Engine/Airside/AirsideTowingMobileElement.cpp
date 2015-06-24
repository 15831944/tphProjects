#include "StdAfx.h"
#include ".\airsidetowingmobileelement.h"
#include "AirsideTowTruckInSim.h"
#include "TowingServiceEvent.h"
#include "StandInSim.h"
#include "Clearance.h"
#include "TaxiRouteInSim.h"
#include "StandLeadInLineInSim.h"
#include "StandLeadOutLineInSim.h"
#include "FlightPerformanceManager.h"
#include "../../InputAirside/TowOffStandAssignmentData.h"
#include "../../Common/ARCTracker.h"

AirsideTowingMobileElement::AirsideTowingMobileElement(AirsideFlightInSim* pFlight, AirsideTowTruckInSim* pVehicle)
:AirsideFlightInSim(pFlight->GetFlightInput(),pFlight->GetUID())
,m_pTowingFlight(pFlight)
,m_pTowTruck(pVehicle)
{
	m_pFlightPerformanceManager = m_pTowingFlight->GetPerformance();
	m_pATC = m_pTowingFlight->GetAirTrafficController();
	m_curState = m_pTowingFlight->GetCurState();
	m_preState = m_pTowingFlight->GetPreState();
	m_bServiceFinished = false;
	m_bWaiting = false;
}

AirsideTowingMobileElement::~AirsideTowingMobileElement(void)
{
}

double AirsideTowingMobileElement::GetLength() const
{
	return m_pTowingFlight->GetLength()/*+m_pTowTruck->GetVehicleLength()*/;
}

void AirsideTowingMobileElement::ServiceFinish()
{
	m_pTowingFlight->WakeUp(GetTime());
	m_pTowTruck->WakeUp(GetTime());

	m_bServiceFinished = true;
	m_bTowingToReleasePoint = false;
}

ElapsedTime AirsideTowingMobileElement::GetStandParkingTime()
{
	if (m_pTowTruck->GetReleasePoint()->GetType() != AirsideResource::ResType_Stand)
		return -1L;

	if (m_pTowTruck->GetReleasePoint() == m_pTowingFlight->GetIntermediateParking())		//intermediate stand 
	{
		ElapsedTime tTime = m_pTowingFlight->getDepTime() - m_pTowingFlight->GetTowOffData()->getFltEnplaneTime() - ElapsedTime(600L);
		return tTime>GetTime()?tTime:GetTime();
	}

	return m_pTowingFlight->GetDepTime();
}

TaxiRouteInSim* AirsideTowingMobileElement::GetFlightOutboundRoute()
{
	return m_pTowingFlight->GetOutBoundRoute();
}

//void AirsideTowingMobileElement::PerformClearance(const Clearance& clearance)
//{
//	int nCount = clearance.GetItemCount();
//	for (int i = 0; i < nCount; i++)
//	{
//		ClearanceItem nextItem = clearance.GetItem(i);
//		PerformClearanceItem(nextItem);
//	}
//
//	//update mobile element state
//	SetMode(m_pTowingFlight->GetMode());
//	SetPosition(m_pTowingFlight->GetPosition());
//	SetTime(m_pTowingFlight->GetTime());
//	SetSpeed(m_pTowingFlight->GetSpeed());
//}

void AirsideTowingMobileElement::PerformClearanceItem(const ClearanceItem& clearanceItem)
{
	m_pTowingFlight->PerformClearanceItem(clearanceItem);

	m_curState = m_pTowingFlight->GetCurState();

	//need minus the distance to flight
	double dist = clearanceItem.GetDistInResource();		
	if (clearanceItem.GetMode() == OnExitStand)
		dist = dist - (m_pTowingFlight->GetLength()*0.5 - m_pTowTruck->GetVehicleLength()*0.5);
	else
		dist = dist + (m_pTowingFlight->GetLength()*0.5 - m_pTowTruck->GetVehicleLength()*0.5);

	ClearanceItem towtruckItem(clearanceItem.GetResource(),clearanceItem.GetMode(),dist);
	towtruckItem.SetTime(clearanceItem.GetTime());
	towtruckItem.SetSpeed(clearanceItem.GetSpeed());
	CPoint2008 pos = clearanceItem.GetResource()->GetDistancePoint(dist);
	towtruckItem.SetPosition(pos);

	m_pTowTruck->UpdateState(towtruckItem);
	m_pTowTruck->WirteLog(towtruckItem.GetPosition(),towtruckItem.GetSpeed(),towtruckItem.GetTime());
}

//void AirsideTowingMobileElement::UpdateState(const AirsideFlightState& fltState)
//{
//	m_pTowingFlight->UpdateState(fltState);
//
//	double dist = fltState.m_dist;		
//	if (fltState.m_fltMode == OnExitStand)
//		dist = dist - (m_pTowingFlight->GetLength()*0.5 - m_pTowTruck->GetVehicleLength()*0.5);
//	else
//		dist = dist + (m_pTowingFlight->GetLength()*0.5 - m_pTowTruck->GetVehicleLength()*0.5);
//
//	CPoint2008 pos = fltState.m_pResource->GetDistancePoint(dist);
//
//	m_pTowTruck->WirteLog(pos,fltState.m_vSpeed,fltState.m_tTime);
//}

void AirsideTowingMobileElement::WakeUp(const ElapsedTime& tTime)
{
	TRACK_CLASS_METHOD(AirsideTowingMobileElement::WakeUp(const ElapsedTime&));
	SetTime(tTime);
	TowingServiceEvent* pEvent = new TowingServiceEvent(this);
	pEvent->setTime(tTime);
	pEvent->addEvent();
}

bool AirsideTowingMobileElement::EnterStand(ElapsedTime tParkingTime)
{
	StandInSim* pStand = (StandInSim*)m_pTowTruck->GetReleasePoint();

	ClearanceItem lastItem(GetResource(),GetMode(),GetDistInResource());
	lastItem.SetSpeed(GetSpeed());
	lastItem.SetTime(GetTime());
	lastItem.SetPosition(GetPosition());

	Clearance newClearance;
	pStand->GetEnterStandClearance(m_pTowingFlight,lastItem,tParkingTime,newClearance);
	PerformClearance(newClearance);

	return true;
}

bool AirsideTowingMobileElement::ExitStand()
{
	StandInSim * pParkingStand = NULL;
	if (GetResource()->GetType() == AirsideResource::ResType_StandLeadInLine)
		pParkingStand = ((StandLeadInLineInSim*)GetResource())->GetStandInSim();
	else if (GetResource()->GetType() == AirsideResource::ResType_StandLeadOutLine)
		pParkingStand = ((StandLeadOutLineInSim*)GetResource())->GetStandInSim();
	else
		pParkingStand = (StandInSim*)GetResource();

	ClearanceItem lastItem(GetResource(),GetMode(),GetDistInResource());
	lastItem.SetSpeed(GetSpeed());
	lastItem.SetTime(GetTime());
	lastItem.SetPosition(GetPosition());

	Clearance newClearance;
	bool bEnd = pParkingStand->GetExitStandClearance(m_pTowingFlight,lastItem,newClearance);
	PerformClearance(newClearance);

	return true;
}

bool AirsideTowingMobileElement::TowingToDestination()
{
	TaxiRouteInSim* pTowRoute = m_pTowingFlight->GetTowingRoute();
	if (pTowRoute)
	{
		m_bWaiting = false;
		ClearanceItem lastItem(GetResource(),GetMode(),GetDistInResource());
		lastItem.SetSpeed(GetSpeed());
		lastItem.SetTime(GetTime());
		lastItem.SetPosition(GetPosition());

		Clearance newClearance;
		double taxispd = m_pTowingFlight->GetTowingSpeed() ;
		pTowRoute->SetSpeed(taxispd);
		bool bEnd = pTowRoute->FindClearanceInConcern(m_pTowingFlight,lastItem,20000,newClearance);
		if (newClearance.GetItemCount() ==0)
		{
			m_bWaiting = true;
			return false;
		}
		PerformClearance(newClearance);


		if (m_pTowTruck->GetServiceType() == TOWOPERATIONTYPE_PUSHBACKONLY 
			&& m_preState.m_pResource->GetType() == AirsideResource::ResType_StandLeadOutLine && GetResource()->GetType() == AirsideResource::ResType_TaxiwayDirSeg)
		{
			m_bTowingToReleasePoint = false;
			return true;
		}
		

		if (bEnd)
			return false;
	}
	return true;
}

bool AirsideTowingMobileElement::EnterDeicePad()
{
	FlightDeiceStrategyDecision& deiceStragy = m_pTowingFlight->GetDeiceDecision();

	ClearanceItem lastItem(GetResource(),GetMode(),GetDistInResource());
	lastItem.SetSpeed(GetSpeed());
	lastItem.SetTime(GetTime());
	lastItem.SetPosition(GetPosition());

	if(lastItem.GetResource() != deiceStragy.m_deicePlace ) //enter the deice place
	{
		AirsideFlightInSim* pInResFlight = deiceStragy.m_deicePlace->GetLastInResourceFlight() ;
		if( pInResFlight)
		{
			m_pTowingFlight->DependOnAgents(pInResFlight);
			return false;
		}
		else
		{
			ClearanceItem atDeiceplaceItem(deiceStragy.m_deicePlace, OnTaxiToDeice,deiceStragy.m_DistInResource);
			ElapsedTime dT = m_pTowingFlight->GetTimeBetween(lastItem,atDeiceplaceItem);
			atDeiceplaceItem.SetTime( lastItem.GetTime() + dT);
			lastItem = atDeiceplaceItem;

			PerformClearanceItem(lastItem);
		}
	}
	return true;
}

bool AirsideTowingMobileElement::EnterRunway()
{
	RunwayPortInSim* pRunway = m_pTowingFlight->GetTakeoffRunway();

	return true;
}
