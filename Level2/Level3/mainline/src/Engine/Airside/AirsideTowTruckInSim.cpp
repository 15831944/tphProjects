#include "StdAfx.h"
#include ".\airsidetowtruckinsim.h"
#include "AirsideFlightInSim.h"
#include "VehicleMoveEvent.h"
#include "VehiclePoolInSim.h"
#include "TowingServiceEvent.h"
#include "TaxiRouteInSim.h"
#include "Clearance.h"
#include "SimulationErrorShow.h"
#include "VehicleRouteResourceManager.h"
#include "StandLeadInLineInSim.h"
#include "TowTruckServiceRequest.h"
#include "AirTrafficController.h"
#include "AirsideResourceManager.h"
#include "AirportResourceManager.h"
#include "TaxiwayToPoolRouteFinder.h"
#include "VehicleStretchLaneInSim.h"


AirsideTowTruckInSim::AirsideTowTruckInSim(int id,int nPrjID,CVehicleSpecificationItem *pVehicleSpecItem)
:AirsideVehicleInSim(id,nPrjID,pVehicleSpecItem)
,m_ReturnRoute()
,m_pServiceRequest(NULL)
{
}

AirsideTowTruckInSim::~AirsideTowTruckInSim(void)
{
}

void AirsideTowTruckInSim::SetServiceFlight(AirsideFlightInSim* pFlight, double vStuffPercent)
{
	if (GetResource()&& (GetResource()->GetType() == AirsideResource::ResType_TaxiwayDirSeg\
		||GetResource()->GetType() == AirsideResource::ResType_VehicleTaxiLane))
	{
		if(m_bAvailable == false)
			return;

		StandInSim* pStand = NULL;	
		if (pFlight->GetResource() && pFlight->GetResource()->GetType() == AirsideResource::ResType_Stand)
			pStand = (StandInSim*)pFlight->GetResource();
		else if (pFlight->GetResource() && pFlight->GetResource()->GetType() == AirsideResource::ResType_StandLeadInLine)
			pStand = ((StandLeadInLineInSim*)pFlight->GetResource())->GetStandInSim();
		else
			pStand = pFlight->GetOperationParkingStand();

		m_pServiceFlight = pFlight;
		VehicleRouteInSim resltRoute;
		if( !GetRouteToServiceStand(pStand,resltRoute) )
		{
			if( GetResource() && pStand )
			{
				CString strWarn;
				CString standName = pStand->GetStandInput()->GetObjNameString();
				CString resName = GetResource()->PrintResource();

				strWarn.Format("Can't Find Route From %s to %s", resName, standName);
				CString strError ="VEHICLE ERROR";
				AirsideSimErrorShown::VehicleSimWarning(this,strWarn,strError);
			}
		}

		m_Route = resltRoute;
		VehicleLaneInSim* pLane = const_cast<VehicleLaneInSim*>(m_Route.GetFirstLaneResource());
		if (pLane)
		{
			SetResource(pLane);
			SetDistInResource(pLane->GetPath().GetPointDist(GetPosition()));
			m_Route.SetCurItem(0);
		}
				
		m_Route.SetMode(OnMoveToService);

		m_bAvailable = false;

		ElapsedTime tCurTime = GetTime();
		if (m_pResource->GetType() == AirsideResource::ResType_VehiclePool)
			tCurTime =  m_tLeavePoolTime;

		VehicleMoveEvent* newEvent = new VehicleMoveEvent(this);			
		newEvent->setTime(tCurTime);

		SetMode(OnMoveToService) ;
		WirteLog(GetPosition(),GetSpeed(),GetTime());
		GenerateNextEvent(newEvent);

		SetMode(OnBeginToService);
		WirteLog(GetPosition(),GetSpeed(),tCurTime);
		SetMode(OnMoveToService) ;
	}
	else
		AirsideVehicleInSim::SetServiceFlight(pFlight,vStuffPercent);

	pFlight->SetTowingServiceVehicle(this);	
}

void AirsideTowTruckInSim::GetNextCommand()
{
	SetAvailable(true);
	m_pVehiclePool->GetNextServiceRequest();
}

bool AirsideTowTruckInSim::IsReadyToService(AirsideFlightInSim* pFlight)
{
	if (GetMode() == OnService)
		return true;

	return false;
}

//void AirsideTowTruckInSim::_ChangeToService()
//{
//	SetMode(OnWaitForService);
//	WirteLog(GetPosition(),GetSpeed(),GetTime());
//	m_pServiceFlight->WakeUp(GetTime());
//}

void AirsideTowTruckInSim::SetServiceType(TOWOPERATIONTYPE _type)
{
	m_ServiceType = _type;
}

TOWOPERATIONTYPE AirsideTowTruckInSim::GetServiceType()
{
	return m_ServiceType;
}

void AirsideTowTruckInSim::SetReturnRoute(const VehicleRouteInSim& pReturnRoute)
{
	m_ReturnRoute = pReturnRoute;
}

void AirsideTowTruckInSim::SetReturnToStretch(const FlightGroundRouteDirectSegList& vSegment)
{
	m_vSegments = vSegment;
}

void AirsideTowTruckInSim::SetReleasePoint(AirsideResource* pResource)
{
	m_pReleasePoint = pResource;
}

AirsideResource* AirsideTowTruckInSim::GetReleasePoint()
{
	return m_pReleasePoint;
}

void AirsideTowTruckInSim::ServicingFlight()
{
	if (m_pServiceFlight == NULL)
		return;

	m_pServiceFlight->SetBeginTowingService();

	TowingServiceEvent* pEvent = new TowingServiceEvent(m_pServiceFlight);
	pEvent->setTime(GetTime());
	pEvent->addEvent();
}

void AirsideTowTruckInSim::WakeUp(const ElapsedTime& tTime)
{

	SetTime(tTime);
	GetNextCommand();

	//should return stretch, then ask for next command
	if (m_pServiceFlight == NULL)
		ReturnVehiclePool(NULL);
}

void AirsideTowTruckInSim::UpdateState(const ClearanceItem& item)
{
	SetTime(item.GetTime());
	SetResource(item.GetResource());
	SetPosition(item.GetPosition());
	SetSpeed(item.GetSpeed());
	SetDistInResource(item.GetDistInResource());
	SetMode(item.GetMode());	
}

void AirsideTowTruckInSim::ReturnVehiclePool( ProbabilityDistribution* pTurnAroundDistribute )
{
	if(GetMode() == OnBackPool || GetMode() == OnVehicleBirth)
		return;

	if (pTurnAroundDistribute)
		tMinTurnAroundTime = long( pTurnAroundDistribute->getRandomValue());
	else
		tMinTurnAroundTime = 0L;

	m_Route = m_ReturnRoute;
	m_Route.SetMode(OnBackPool);
	SetMode(OnBackPool);
	m_ReturnRoute.ClearRouteItems();
	m_vSegments.clear();

	VehicleMoveEvent* newEvent = new VehicleMoveEvent(this);
	newEvent->setTime(GetTime());
	newEvent->addEvent(); 
}

void AirsideTowTruckInSim::GetCompleteReturnRouteToPool()
{
	bool bRetRoute =false;
	AirsideResource * pEndRes = NULL;
	std::vector<VehicleRouteNodePairDist> vLastNodePair;
	if (m_ReturnRoute.IsEmpty())
	{
		if (GetResource()->GetType() == AirsideResource::ResType_StandLeadInLine || GetResource()->GetType() == AirsideResource::ResType_StandLeadOutLine)
			pEndRes = (AirsideResource*)((StandLeadInLineInSim*)GetResource())->GetStandInSim();
		else if (m_ServiceType == TOWOPERATIONTYPE_PUSHBACKTOTAXIWAY)
			pEndRes = (AirsideResource*)m_pServiceFlight->GetOperationParkingStand();
		else
			pEndRes = GetResource();

		if( pEndRes && m_pVehiclePool )
		{
		 	bRetRoute = m_pRouteResMan->GetVehicleRoute(pEndRes,m_pVehiclePool,m_Route);
		
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
	else
	{

		if (GetResource()->GetType() == AirsideResource::ResType_TaxiwayDirSeg )
		{
			VehicleLaneInSim* pLane = const_cast<VehicleLaneInSim*>(m_ReturnRoute.GetFirstLaneResource());
			SetResource(pLane);
			SetDistInResource(pLane->GetPath().GetPointDist(GetPosition()));
			m_ReturnRoute.SetCurItem(0);

			//FlightGroundRouteDirectSegList vSegments;
			//vSegments.clear();

			//IntersectionNodeInSim* pEntryNode = (IntersectionNodeInSim*)(((VehicleTaxiLaneInSim*)m_pResource)->GetEndExit()->GetDestResource());
			AirportResourceManager* pAirportRes = m_pServiceFlight->GetAirTrafficController()->GetAirsideResourceManager()->GetAirportResource();		

			pAirportRes->GetVehicleLastRouteFromTaxiRoute(m_vSegments,vLastNodePair);
		}

		double dDist = (std::numeric_limits<double>::max)();
		int nMinDistanceIndex = -1;
		VehicleRouteInSim routeShortestToStretch;
		for (int i = 0; i < (int)vLastNodePair.size(); i++)
		{
			VehicleRouteNodePairDist& nodePair = vLastNodePair[i];
			pEndRes = const_cast<AirsideResource*>(nodePair.GetNodeTo()->GetDestResource());

			if( pEndRes && m_pVehiclePool )
			{
				VehicleRouteInSim routeToStretch;
				bRetRoute = m_pRouteResMan->GetVehicleRoute(pEndRes,m_pVehiclePool,routeToStretch);

				if( !bRetRoute)
					continue;

				double dLength = routeToStretch.GetLength();
				if (dDist > dLength)
				{
					dDist = dLength;
					nMinDistanceIndex = i;
					routeShortestToStretch = routeToStretch;
				}
			}
		}

		if (nMinDistanceIndex != -1)
		{
			m_ReturnRoute.PopBack();
			m_ReturnRoute.AddData(vLastNodePair[nMinDistanceIndex]);
			routeShortestToStretch.CopyDataToRoute(m_ReturnRoute);
		}
		else
		{
			pEndRes = const_cast<AirsideResource*>(m_ReturnRoute.GetDestResource());
			if (pEndRes && m_pVehiclePool)
			{
				CString strWarn;
				CString standName = pEndRes->PrintResource();
				CString poolName = m_pVehiclePool->GetPoolInput()->GetObjNameString();
				strWarn.Format("Can't Find Route From %s to %s", standName, poolName);
				CString strError ="VEHICLE ERROR";
				AirsideSimErrorShown::VehicleSimWarning(this,strWarn,strError);
			}
		}
	}
}

void AirsideTowTruckInSim::SetTowTruckServiceRequest( TowTruckServiceRequest* pRequest )
{
	m_pServiceRequest = pRequest;
}

TowTruckServiceRequest* AirsideTowTruckInSim::GetTowTruckServiceRequest()
{
	return m_pServiceRequest;
}

bool AirsideTowTruckInSim::GetRouteToServiceStand( StandInSim* pServiceStand, VehicleRouteInSim& resultRoute )
{
	bool bRetRoute =false;
	AirsideResource * pEndRes = NULL;
	std::vector<VehicleRouteNodePairDist> vLastNodePair;
	if (GetResource() && GetResource()->GetType() == AirsideResource::ResType_TaxiwayDirSeg )
	{
		FlightGroundRouteDirectSegList vSegments;
		vSegments.clear();

		AirportResourceManager* pAirportRes = m_pServiceFlight->GetAirTrafficController()->GetAirsideResourceManager()->GetAirportResource();		
		pAirportRes->GetTaxiwayToNearestStretchRouteFinder()->GetShortestPathFromTaxiIntersectionToNearestStretch(((TaxiwayDirectSegInSim*)m_pResource)->GetEntryNode(), vSegments);

		std::vector<VehicleRouteNodePairDist> vNodePairs;
		pAirportRes->GetVehiclePartOfReturnRouteFromTaxiRoute(vSegments, vNodePairs);
		resultRoute.AddData(vNodePairs);

		pAirportRes->GetVehicleLastRouteFromTaxiRoute(vSegments,vLastNodePair);
	}
	else if (GetResource() && GetResource()->GetType() == AirsideResource::ResType_VehicleTaxiLane )
	{
		FlightGroundRouteDirectSegList vSegments;
		vSegments.clear();

		AirportResourceManager* pAirportRes = m_pServiceFlight->GetAirTrafficController()->GetAirsideResourceManager()->GetAirportResource();	
		IntersectionNodeInSim* pEntryNode = (IntersectionNodeInSim*)(((VehicleTaxiLaneInSim*)m_pResource)->GetBeginEntry()->GetOrignResource());
		pAirportRes->GetTaxiwayToNearestStretchRouteFinder()->GetShortestPathFromTaxiIntersectionToNearestStretch(pEntryNode, vSegments);

		std::vector<VehicleRouteNodePairDist> vNodePairs;
		pAirportRes->GetVehiclePartOfReturnRouteFromTaxiRoute(vSegments,vNodePairs);
		resultRoute.AddData(vNodePairs);

		pAirportRes->GetVehicleLastRouteFromTaxiRoute(vSegments,vLastNodePair);
	}

	if (vLastNodePair.empty())
		return false;
	
	double dDist = (std::numeric_limits<double>::max)();
	int nMinDistanceIndex = -1;
	VehicleRouteInSim routeShortestToStretch;
	for (int i = 0; i < (int)vLastNodePair.size(); i++)
	{
		VehicleRouteNodePairDist& nodePair = vLastNodePair[i];
		pEndRes = const_cast<AirsideResource*>(nodePair.GetNodeTo()->GetDestResource());

		if( pEndRes && pServiceStand )
		{
		 	VehicleRouteInSim routeToStretch;
		 	bRetRoute = m_pRouteResMan->GetVehicleRoute(pEndRes,pServiceStand,routeToStretch);
		
		 	if( !bRetRoute)
		 		continue;
		
			double dLength = routeToStretch.GetLength();
			if (dDist > dLength)
			{
				dDist = dLength;
				nMinDistanceIndex = i;
				routeShortestToStretch = routeToStretch;
			}
		}
	}

	if (nMinDistanceIndex != -1)
	{
		resultRoute.AddData(vLastNodePair[nMinDistanceIndex]);
		routeShortestToStretch.CopyDataToRoute(resultRoute);
		return true;
	}

	return false;

}