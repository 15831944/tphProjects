#include "StdAfx.h"
#include ".\airsidefollowmecarinsim.h"
#include "AirsideMeetingPointInSim.h"
#include "AirsideFlightInSim.h"
#include "VehicleMoveEvent.h"
#include "../../InputAirside/MeetingPoint.h"
#include "SimulationErrorShow.h"
#include "VehicleRouteResourceManager.h"
#include "VehiclePoolInSim.h"
#include "VehicleStretchLaneInSim.h"
#include "AirsideFollowMeCarServiceEvent.h"
#include "../../InputAirside/FollowMeConnectionData.h"
#include "AirTrafficController.h"
#include "AirsideResourceManager.h"
#include "AirportResourceManager.h"
#include "TaxiwayToPoolRouteFinder.h"
#include "FlightInHoldEvent.h"
#include "../../Results/AirsideFlightLogEntry.h"

AirsideFollowMeCarInSim::AirsideFollowMeCarInSim(int id,int nPrjID,CVehicleSpecificationItem *pVehicleSpecItem)
:AirsideVehicleInSim(id,nPrjID,pVehicleSpecItem)
,m_ReturnRoute()
,m_pMeetingPoint(NULL)
,m_pAbandonData(NULL)
,m_pDestRes(NULL)
{
}

AirsideFollowMeCarInSim::~AirsideFollowMeCarInSim(void)
{
}

bool AirsideFollowMeCarInSim::IsReadyToService( AirsideFlightInSim* pFlight )
{
	if (GetMode() == OnWaitForService)
		return true;

	return false;
}

void AirsideFollowMeCarInSim::GetNextCommand()
{
	SetAvailable(true);

	m_pServiceFlight = NULL;
	m_pMeetingPoint = NULL;
	m_pAbandonData = NULL;

	ReturnVehiclePool(NULL);
}

void AirsideFollowMeCarInSim::ServicingFlight()
{
	double dDist = m_pServiceFlight->GetDistInResource() + m_pServiceFlight->GetLogEntry().GetAirsideDescStruct().dLength;
	CPoint2008 CarPos = m_pServiceFlight->GetResource()->GetDistancePoint(dDist);

	SetMode(OnService);
	SetSpeed(m_pServiceFlight->GetSpeed());
	ElapsedTime tTime = ElapsedTime(2.0*CarPos.distance(GetPosition())/GetSpeed()) + GetTime();
	SetTime(tTime);
	if(GetResource() != m_pServiceFlight->GetResource())
		GetResource()->SetExitTime(this,tTime);
	SetResource(m_pServiceFlight->GetResource());
	SetPosition(CarPos);
	SetDistInResource(dDist);
	WirteLog(CarPos,GetSpeed(),tTime);

	m_pServiceFlight->SetBeginFollowMeCarService();

	AirsideFollowMeCarServiceEvent* pEvent = new AirsideFollowMeCarServiceEvent(m_pServiceFlight);
	pEvent->setTime(tTime);
	pEvent->addEvent();
}

void AirsideFollowMeCarInSim::SetReturnRoute( const VehicleRouteInSim& pReturnRoute )
{
	m_ReturnRoute = pReturnRoute;
}

void AirsideFollowMeCarInSim::SetAbandonData( CAbandonmentPoint* pAbandonData )
{
	m_pAbandonData = pAbandonData;
}

CAbandonmentPoint* AirsideFollowMeCarInSim::GetAbandonData()
{
	return m_pAbandonData;
}

void AirsideFollowMeCarInSim::UpdateState( const ClearanceItem& item )
{
	SetTime(item.GetTime());
	SetResource(item.GetResource());
	SetPosition(item.GetPosition());
	SetSpeed(item.GetSpeed());
	SetDistInResource(item.GetDistInResource());
	SetMode(item.GetMode());	
}

void AirsideFollowMeCarInSim::WakeUp( const ElapsedTime& tTime )
{
	SetTime(tTime);
	GetNextCommand();
}

void AirsideFollowMeCarInSim::ReturnVehiclePool( ProbabilityDistribution* pTurnAroundDistribute )
{
	if(GetMode() == OnBackPool || GetMode() == OnVehicleBirth || GetMode() == OnParkingPool)
		return;

	if (pTurnAroundDistribute)
		tMinTurnAroundTime = long( pTurnAroundDistribute->getRandomValue());
	else
		tMinTurnAroundTime = 0L;

	m_Route = m_ReturnRoute;
	m_Route.SetMode(OnBackPool);
	SetMode(OnBackPool);
	m_ReturnRoute.ClearRouteItems();

	VehicleMoveEvent* newEvent = new VehicleMoveEvent(this);
	newEvent->setTime(GetTime());
	newEvent->addEvent(); 
}

void AirsideFollowMeCarInSim::GetCompleteReturnRouteToPool()
{
	AirsideResource * pEndRes = NULL;
	if (m_pAbandonData->m_PointType == CAbandonmentPoint::TY_Intersection)
	{
		FlightGroundRouteDirectSegList vSegments;
		vSegments.clear();
		AirportResourceManager* pAirportRes = m_pServiceFlight->GetAirTrafficController()->GetAirsideResourceManager()->GetAirportResource();		
		pAirportRes->GetTaxiwayToNearestStretchRouteFinder()->GetShortestPathFromTaxiIntersectionToNearestStretch((IntersectionNodeInSim*)m_pDestRes, vSegments);

		std::vector<VehicleRouteNodePairDist> vNodePairs;
		pAirportRes->GetVehicleReturnRouteFromTaxiRoute(vSegments, vNodePairs);
		m_ReturnRoute.AddData(vNodePairs);

		pEndRes = const_cast<AirsideResource*>(m_ReturnRoute.GetDestResource());
	}
	else
		pEndRes = (StandInSim*)m_pDestRes;

	if( pEndRes && m_pVehiclePool )
	{
		bool bRetRoute = m_pRouteResMan->GetVehicleRoute(pEndRes,m_pVehiclePool,m_Route);

		if( !bRetRoute)
		{	
			CString strWarn;
			CString standName = pEndRes->PrintResource();
			CString poolName = m_pVehiclePool->GetPoolInput()->GetObjNameString();
			strWarn.Format("Can't Find Route From %s to %s", standName, poolName);
			CString strError ="VEHICLE ERROR";
			AirsideSimErrorShown::VehicleSimWarning(this,strWarn,strError);
		}
		else
		{
			m_Route.CopyDataToRoute(m_ReturnRoute);
		}
	}

}

void AirsideFollowMeCarInSim::SetMeetingPoint( AirsideMeetingPointInSim* pPoint )
{
	m_pMeetingPoint = pPoint;
}

AirsideMeetingPointInSim* AirsideFollowMeCarInSim::GetMeetingPoint()
{
	return m_pMeetingPoint;
}

void AirsideFollowMeCarInSim::_ChangeToService()
{
	CPoint2008 waitingPos = m_pMeetingPoint->GetWaitingPosition();
	ElapsedTime tTime = ElapsedTime(2.0*waitingPos.distance(GetPosition())/GetSpeed()) + GetTime();
	SetMode(OnWaitForService);
	SetTime(tTime);
	SetPosition(waitingPos);

	if (m_pServiceFlight->GetMode() == OnTaxiToStand)
	{
		m_pServiceFlight->SetBeginFollowMeCarService();
		WirteLog(waitingPos,GetSpeed(),tTime);
		ServicingFlight();
	}
	else
	{
		SetSpeed(0);
		WirteLog(waitingPos,GetSpeed(),tTime);
	}
}

void AirsideFollowMeCarInSim::SetServiceFlight( AirsideFlightInSim* pFlight, double vStuffPercent )
{
	if(m_bAvailable == false)
		return;

	//go to service stand and service
	AirsideResource * AtResource = GetResource();

	VehicleRouteInSim resltRoute;
	if( !m_pRouteResMan->GetVehicleRoute(AtResource,m_pMeetingPoint,resltRoute) )
	{
		if( AtResource && m_pMeetingPoint )
		{
			CString strWarn;
			CString standName = m_pMeetingPoint->GetMeetingPointInput()->GetObjNameString();
			CString resName = "UNKNOWN";

			if(AtResource->GetType() == AirsideResource::ResType_VehiclePool )
			{
				VehiclePoolInSim* vehiclePool = (VehiclePoolInSim*) AtResource;
				resName = vehiclePool->GetPoolInput()->GetObjNameString();
			}
			if(AtResource->GetType() == AirsideResource::ResType_VehicleStretchLane)
			{
				VehicleLaneInSim* pLane = (VehicleLaneInSim*)AtResource;
				resName = pLane->PrintResource();
			}
			if(AtResource->GetType() == AirsideResource::ResType_VehicleRoadIntersection)
			{
				VehicleRoadIntersectionInSim * pIntersect = (VehicleRoadIntersectionInSim*)AtResource;
				resName = pIntersect->GetNodeInput().GetName();
			}

			strWarn.Format("Can't Find Route From %s to %s", resName, standName);
			CString strError ="VEHICLE ERROR";
			AirsideSimErrorShown::VehicleSimWarning(this,strWarn,strError);
		}
	}
	else
	{
		m_Route = resltRoute;
	}

	m_Route.SetMode(OnMoveToService);

	SetMode(OnMoveToService) ;
	WirteLog(GetPosition(),GetSpeed(),GetTime());

	m_pServiceFlight = pFlight;
	m_bAvailable = false;


	VehicleMoveEvent* newEvent = new VehicleMoveEvent(this);
	newEvent->setTime(pFlight->GetTime());

	GenerateNextEvent(newEvent);

	SetMode(OnBeginToService);
	WirteLog(GetPosition(),GetSpeed(),pFlight->GetTime());


	SetMode(OnMoveToService) ;
}

void AirsideFollowMeCarInSim::SetDestNode( AirsideResource* pDest )
{
	m_pDestRes = pDest;
}