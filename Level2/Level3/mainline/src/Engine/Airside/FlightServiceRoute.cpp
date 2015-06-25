#include "StdAfx.h"
#include ".\flightserviceroute.h"
#include "AirsideFlightInSim.h"
#include "../../common/DistanceLineLine.h"
#include "../../common/GetIntersection.h"
#include "FlightServiceEvent.h"
#include "AirsideVehicleInSim.h"
#include "VehicleMoveEvent.h"
#include "StandLeadInLineInSim.h"
#include <limits>
#include "AirsidePaxBusInSim.h"
FlightServiceLocationInSim::FlightServiceLocationInSim(const FlightTypeRelativeElement& element)
{
	m_serviceLocation = element;
	m_bHasServiced = false;
//	m_pServiceVehicle = NULL;
	m_pOccupyVehicle = NULL;

}
FlightServiceLocationInSim::~FlightServiceLocationInSim()
{


}

//////////////////////////////////////////////////////////////////////////


CFlightServiceRoute::CFlightServiceRoute(AirsideFlightInSim* pFlightInSim)
:m_pFlightInSim(pFlightInSim)
,m_bInitialized(false)
,m_bArrivalAtStand(false)
{
	m_pStand = NULL;
}

CFlightServiceRoute::~CFlightServiceRoute(void)
{
}
CPoint2008 CFlightServiceRoute::GetDistancePoint(double dist)const
{
	CPath2008 path = m_ringRoad.GetPath();
	return path.GetDistPoint(dist);
}
bool CFlightServiceRoute::VehicleRoutePoint::HasServicePoint(AirsideVehicleInSim* pVehicle)
{
	size_t nServicePoint = vServicePoint.size();
	if (nServicePoint >0)
	{
		std::vector<FlightServiceLocationInSim>::iterator iterServicePoint = vServicePoint.begin();

		for (;iterServicePoint != vServicePoint.end(); ++ iterServicePoint)
		{

			if((*iterServicePoint).m_bHasServiced)
			{
			}
			else if(pVehicle->GetVehicleTypeID() == (*iterServicePoint).m_serviceLocation.GetTypeID())
				return true;
		}
	}

	return false;
}
void CFlightServiceRoute::VehicleRoutePoint::VehicleServicePoint(AirsideVehicleInSim* pVehicle)
{
	size_t nServicePoint = vServicePoint.size();
	if (nServicePoint >0)
	{
		std::vector<FlightServiceLocationInSim>::iterator iterServicePoint = vServicePoint.begin();

		for (;iterServicePoint != vServicePoint.end(); ++ iterServicePoint)
		{

			if((*iterServicePoint).m_bHasServiced)
			{
			}
			else if(pVehicle->GetVehicleTypeID() == (*iterServicePoint).m_serviceLocation.GetTypeID())
			{

			}
		}
	}
}
int CFlightServiceRoute::VehicleRoutePoint::TheVehicleCanService(AirsideVehicleInSim* pVehicle,CFlightServiceRoute *pServiceRoute)
{
	size_t nServicePoint = vServicePoint.size();
	if (nServicePoint >0)
	{
		std::vector<FlightServiceLocationInSim>::iterator iterServicePoint = vServicePoint.begin();

		for (;iterServicePoint != vServicePoint.end(); ++ iterServicePoint)
		{

			//if((*iterServicePoint).m_bHasServiced)
			//{
			//}
			//else if((*iterServicePoint).m_pOccupyVehicle  == NULL &&pVehicle->GetVehicleTypeID() == (*iterServicePoint).m_serviceLocation.GetTypeID())
			//{
			//	
			//	ElapsedTime elapMoveTime = ElapsedTime((point.getDistanceTo(pVehicle->GetPosition())/pVehicle->GetSpeed()));

			//	pVehicle->WirteLog(point,pVehicle->GetSpeed(),pVehicle->GetTime() + elapMoveTime);

			//	pVehicle->SetPosition(point);

			//	pVehicle->SetTime(pVehicle->GetTime() + elapMoveTime);							
			//	pVehicle->SetMode(OnService);

			//	pServiceRoute->GenerateNextEvent(pVehicle,pVehicle->GetTime());

			//	pOccupyVehicle = pVehicle;

			//	return true;
			//}

			if (pVehicle->GetVehicleBaseType() != VehicleType_PaxTruck && pVehicle->GetVehicleBaseType() != VehicleType_BaggageTug)
			{	
				if (pVehicle->GetVehicleBaseType() == VehicleType_TowTruck && pVehicle->GetVehicleTypeID() == (*iterServicePoint).m_serviceLocation.GetTypeID())
				{
					ElapsedTime elapMoveTime = ElapsedTime((point.getDistanceTo(pVehicle->GetPosition())/pVehicle->GetSpeed()));

					pVehicle->WirteLog(point,pVehicle->GetSpeed(),pVehicle->GetTime() + elapMoveTime);

					pVehicle->SetPosition(point);

					pVehicle->SetTime(pVehicle->GetTime() + elapMoveTime);							
					pVehicle->SetMode(OnService);

					pServiceRoute->GenerateNextEvent(pVehicle,pVehicle->GetTime());

					pOccupyVehicle = pVehicle;

					return true;
				}
				

				if((*iterServicePoint).m_bHasServiced || (*iterServicePoint).m_pOccupyVehicle != NULL)
				{

				}
				else if(pVehicle->GetVehicleTypeID() == (*iterServicePoint).m_serviceLocation.GetTypeID())
				{

					ElapsedTime elapMoveTime = ElapsedTime((point.getDistanceTo(pVehicle->GetPosition())/pVehicle->GetSpeed()));

					pVehicle->WirteLog(point,pVehicle->GetSpeed(),pVehicle->GetTime() + elapMoveTime);

					pVehicle->SetPosition(point);

					pVehicle->SetTime(pVehicle->GetTime() + elapMoveTime);							
					pVehicle->SetMode(OnService);

					pServiceRoute->GenerateNextEvent(pVehicle,pVehicle->GetTime());

					pOccupyVehicle = pVehicle;

					return true;
				}
			}
			else
			{

				if(pVehicle->GetVehicleTypeID() == (*iterServicePoint).m_serviceLocation.GetTypeID()/* && !((CAirsidePaxBusInSim*)(pVehicle))->IsFull()*/)
				{

					if((*iterServicePoint).m_pOccupyVehicle != NULL)
					{
						vWaitingVehicle.push_back(pVehicle);
						return 2;
					}

					ElapsedTime elapMoveTime = ElapsedTime((point.getDistanceTo(pVehicle->GetPosition())/pVehicle->GetSpeed()));

					pVehicle->WirteLog(point,pVehicle->GetSpeed(),pVehicle->GetTime() + elapMoveTime);

					pVehicle->SetPosition(point);

					pVehicle->SetTime(pVehicle->GetTime() + elapMoveTime);
					if(!pVehicle->IsArrivedAtStand())
					{
						pVehicle->SetMode(OnService);

						pServiceRoute->GenerateNextEvent(pVehicle,pVehicle->GetTime());
					}
					pOccupyVehicle = pVehicle;

					return 1;
				}
			}
		}
	}

	return 0;
}
void CFlightServiceRoute::VehicleRoutePoint::WakeupWaitingVehicle(CFlightServiceRoute *pServiceRoute)
{

	int i = 0;
	std::vector<AirsideVehicleInSim *>::iterator iterVehicle = vWaitingVehicle.begin();
	for (; iterVehicle != vWaitingVehicle.end(); ++ iterVehicle)
	{
		AirsideVehicleInSim * pWaitingVehicle = *iterVehicle;
		if(pWaitingVehicle != NULL)//wake up the waiting vehicle
		{
		//	pWaitingVehicle->WirteLog(pWaitingVehicle->GetPosition(),pWaitingVehicle->GetSpeed(),pOccupyVehicle->GetTime());
			pServiceRoute->GenerateNextEvent(pWaitingVehicle,pOccupyVehicle->GetTime() + ElapsedTime(0.01 * i));
			i+= 1;
		}
	}
	vWaitingVehicle.clear();

	pOccupyVehicle = NULL;
}

void CFlightServiceRoute::VehicleRoutePoint::AddWaitingVehicle( AirsideVehicleInSim *pWaitingVehicle )
{
	if(std::find(vWaitingVehicle.begin(), vWaitingVehicle.end(), pWaitingVehicle) == vWaitingVehicle.end())
		vWaitingVehicle.push_back(pWaitingVehicle);

}
bool CFlightServiceRoute::FindClearanceInConcern( AirsideVehicleInSim * pVehicle, ClearanceItem& lastItem, DistanceUnit radius, Clearance& newClearance )
{


	ASSERT(pVehicle != NULL);

	InitResource(pVehicle);
	ASSERT(m_vVehicleRoutePoint.size() >0);

	if (m_vVehicleRoutePoint.empty())
	{
		pVehicle->SetMode(OnService);
		return true;
	}

	AirsideMobileElementMode currentMode =pVehicle->GetMode();

	//CString strState = "unknown";
	////log vehicle
	//{
	//	AirsideFlightInSim *pFlight = pVehicle->GetServiceFlight();
	//	CString strFlightID = "unknown";
	//	pFlight->getFullID(strFlightID.GetBuffer(1024));
	//	CPoint2008 ptFlightPos = pFlight->GetPosition();
	//	CPoint2008 ptVehiclePos =  pVehicle->GetPosition();
	//
	//	if (currentMode <MODE_COUNT && currentMode >=0 )
	//	{
	//		strState = AIRSIDEMOBILE_MODE_NAME[currentMode];
	//	}
	//	
	//	ofsstream echoFile ("c:\\CFlightServiceRoute.log", stdios::app);
	//	echoFile << "Vehicle ID: "<<pVehicle->m_id
	//		<< " Vehicle type: "<<pVehicle->GetVehicleTypeID()
	//		<<" Vehicle Pos:" <<pVehicle->m_pCurrentPointIndex
	//		<<"  State = "<< strState.GetBuffer()
	//		<< ",x = " << ptVehiclePos.getX()
	//		<< ",y = " << ptVehiclePos.getY()
	//		<< ",z = " <<ptVehiclePos.getZ()
	//		<<"Flight ID = "<<strFlightID.GetBuffer()
	//		<<" Index : "<<pFlight->GetUID()
	//		<< ",x = " << ptFlightPos.getX()
	//		<< ",y = " << ptFlightPos.getY()
	//		<< ",z = " <<ptFlightPos.getZ()			

	//		<< ',' << pVehicle->GetTime()<<"\r\n\r\n";
	//		echoFile.close();

	//}
	pVehicle->NotifyOtherAgents(SimMessage().setTime(pVehicle->GetTime()));//pVehicle->NotifyObservers();

	switch(currentMode)
	{
	case OnMoveToRingRoute:
		{
			VehicleOnMoveToRingRoute(pVehicle,lastItem,radius,newClearance);
		}
		break;

	case OnServiceMovement:
		{
			VehicleOnServiceMovement(pVehicle,lastItem,radius,newClearance);			
		}
		break;
	case OnVehicleMoveToTempParking:
		{
			VehicleOnVehicleMoveToTempParking(pVehicle,lastItem,radius,newClearance);
		}
		break;

	case OnVehicleMoveOutTempParking:
		{
			VehicleOnVehicleMoveOutTempParking(pVehicle,lastItem,radius,newClearance);
		}
		break;
	case OnService:
		{
			VehicleOnService(pVehicle,lastItem,radius,newClearance);
		}
		break;
	case OnLeaveServicePoint:
		{	
			VehicleOnLeaveServicePoint(pVehicle,lastItem,radius,newClearance);
		}
		break;
	case OnLeaveServiceMovement:
			VehicleOnLeaveServiceMovement(pVehicle,lastItem,radius,newClearance);
		break;

	case OnLeaveRingRoute:
		{
			VehicleOnLeaveRingRoute(pVehicle,lastItem,radius,newClearance);
		}
		break;
	default:
		//AfxMessageBox(strState);
		break;
	}

	//AirsideResource * pResouce = lastItem.GetResource();
	//if (pResouce != this)

	return false;
}
void CFlightServiceRoute::GenerateNextEvent(AirsideVehicleInSim *pVehicle,const ElapsedTime& eTime)
{
	//pVehicle->WirteLog(pVehicle->GetPosition(),pVehicle->GetSpeed(),pVehicle->GetTime());
	CFlightServiceEvent * pNextEvent = new CFlightServiceEvent(pVehicle);
	pVehicle->SetTime(eTime);
	pNextEvent->setTime(eTime + ElapsedTime(0.01));
	pVehicle->GenerateNextEvent(pNextEvent);
	 
	//pNextEvent->addEvent();
}
int CFlightServiceRoute::GetHasNotServicedPointCount(int nVehicleType)
{
	int nCount = 0;
	//std::vector<FlightServiceLocationInSim>::iterator iterEntryPoint = m_vFltServiceLoctInSim.begin();

	//for (;iterEntryPoint != m_vFltServiceLoctInSim.end(); ++ iterEntryPoint)
	//{
	//	if((*iterEntryPoint).m_serviceLocation.GetTypeID() == nVehicleType)
	//	{
	//		if (!(*iterEntryPoint).m_bHasServiced)
	//			nCount += 1;
	//	}
	//}

	return nCount;
}

void CFlightServiceRoute::SetFlightServiceStand(StandInSim* pStand)
{
	if (m_pStand == pStand)
		return;

	m_pStand = pStand;
	m_bInitialized = false;
}

void CFlightServiceRoute::InitResource(AirsideVehicleInSim  * pVehicle)
{
	if (m_bInitialized)
	 return;

	
	ASSERT(m_pFlightInSim != NULL);
	if (m_pStand == NULL)
		m_pStand = m_pFlightInSim->GetOperationParkingStand();

	StandLeadInLineInSim* pLeadInLineInSim = NULL;

	if (m_pStand)
		pLeadInLineInSim = m_pStand->AssignLeadInLine(m_pFlightInSim);
	
	if (pLeadInLineInSim == NULL)
		return;

	m_vVehicleRoutePoint.clear();
	m_vVehicleTempPark.clear();
	m_vWaitingFlightVehicle.clear();
	m_vUnreachableVehicleType.clear();

	//central pos
	double dFlightLength = m_pFlightInSim->GetLength();
	DistanceUnit LeadInLineEndDist = pLeadInLineInSim->GetEndDist();
	CPoint2008 fltPos = pLeadInLineInSim->GetDistancePoint(LeadInLineEndDist- dFlightLength*0.5);
	CPoint2008 fltPosExt = pLeadInLineInSim->GetDistancePoint(LeadInLineEndDist- dFlightLength*0.5 -1.0 );

	//get the header position index
	{
		int nHeadPos = 0;
		DistanceUnit nHeadPosY = 0;
		CPath2008 tempSmoothPath = m_OriginalRingRoad.GetSmoothPath();
		for (int k = 0; k < tempSmoothPath.getCount(); ++k)
		{
			if (tempSmoothPath.getPoint(k).getY() < nHeadPosY)
			{
				nHeadPos = k;
				nHeadPosY = tempSmoothPath.getPoint(k).getY();
			}
		}

		ConvertPathLocationsIntoLocalCoordinate(fltPos, CPoint2008(fltPos-fltPosExt).getHeading());
	
		m_ptHead = m_ringRoad.GetPath().getPoint(nHeadPos);
	}

	//init service locations
	//calculate the entry point
	CPoint2008 pt = pVehicle->GetPosition();
	DistanceUnit distToVehicle = 0;
	CPoint2008 closePoint;
	CPath2008 smoothPath = m_ringRoad.GetPath();
	closePoint = smoothPath.getClosestPoint(pt);

	int nEntryPos = 0;
	for (; nEntryPos < smoothPath.getCount(); ++nEntryPos)
	{
		if (closePoint == smoothPath.getPoint(nEntryPos))
			break;
	}
	
	CPath2008 ringPath;
	ringPath.init(smoothPath);
	while (ringPath.getCount() > 0)
	{
		if(ringPath.getPoint(0) == closePoint)
			break;
		else
			ringPath.RemovePointAt(0);
	}

	for (int i =0; i< nEntryPos; ++i)
	{
		ringPath.insertPointAfterAt(smoothPath.getPoint(i),ringPath.getCount()-1);
	}

	InitVehicleRoute(ringPath);

	CPath2008 path = m_OriginalRingRoad.GetPath();
	int nEndPos = path.getCount() - 1;
	path.insertPointAfterAt(path.getPoint(0),nEndPos);
	RingRoad OriginalPath;
	OriginalPath.SetPath(path);

	OriginalPath.ConvertPathIntoLocalCoordinate(fltPos, CPoint2008(fltPos-fltPosExt).getHeading());
	CPoint2008 vDir = CPoint2008(fltPos-fltPosExt);
	vDir.Normalize();
	CPoint2008 ParkingPos = fltPos + vDir*1000.0;

	m_VehicleParkingPlaceMgr.InitResource(OriginalPath.GetPath(),ParkingPos,vDir);

	m_bInitialized = true;

}
void CFlightServiceRoute::InitVehicleRoute(CPath2008& routePath)
{
	//init the vehicle route 
	for (int i =0; i< routePath.getCount(); ++i)
	{
		CPoint2008 pt =routePath.getPoint(i);
		VehicleRoutePoint vehicleRoutePoint(pt);
		m_vVehicleRoutePoint.push_back(vehicleRoutePoint);
	}

	//put the service point into vehicle route
	const FlightTypeServiceLoactions::ElementList serviceloctList = m_fltServiceLocations.GetEventList();
	FlightTypeServiceLoactions::ElementList::const_iterator const_iter = serviceloctList.begin();
	for (; const_iter != serviceloctList.end(); ++ const_iter)
	{
		FlightTypeRelativeElement  element = *const_iter;

		FlightServiceLocationInSim servicePoint(element);
	
		CPoint2008 entryPoint = routePath.getClosestPoint(element.GetServiceLocation());

		for (size_t j =0; j < m_vVehicleRoutePoint.size(); ++j)
		{

			if(m_vVehicleRoutePoint[j].point == entryPoint)
			{
				m_vVehicleRoutePoint[j].vServicePoint.push_back(servicePoint);
				break;
			}
		}
	}
}


void CFlightServiceRoute::AddFltServiceLoctInSim(const FlightServiceLocationInSim& fltsvcloct)
{
	//size_t nCount = m_vFltServiceLoctInSim.size();
	//if (nCount == 0)
	//	m_vFltServiceLoctInSim.push_back(fltsvcloct);
	//else
	//{
	//	bool bAdd = false;
	//	for (size_t i = 0; i < nCount; ++i)
	//	{
	//		if (m_vFltServiceLoctInSim[i].m_dDistInPath>= fltsvcloct.m_dDistInPath)
	//		{
	//			m_vFltServiceLoctInSim.insert(m_vFltServiceLoctInSim.begin() + i,fltsvcloct);
	//			bAdd = true;
	//			break;
	//		}
	//	}

	//	if (bAdd == false)
	//	{
	//		m_vFltServiceLoctInSim.push_back(fltsvcloct);
	//	}
	//}


}
void CFlightServiceRoute::SetFltServiceLocations(const FlightTypeServiceLoactions& fltServiceLocations)
{
	m_originalFltServiceLocations = fltServiceLocations;
}

void CFlightServiceRoute::ConvertPathLocationsIntoLocalCoordinate(const CPoint2008& pt,const double& dRotatedegree)
{
	CPath2008 ringPath = m_OriginalRingRoad.GetSmoothPath();
	//for (int i =0; i< ringPath.getCount();++ i)
	//{
	//	ringPath[i] = ringPath[i]* 100;
	//}
	//ringPath.Rotate(dRotatedegree);
	//ringPath.DoOffset(pt.getX(),pt.getY(),pt.getZ());

	m_ringRoad = m_OriginalRingRoad;
	m_ringRoad.SetPath(ringPath);
	m_ringRoad.ConvertPathIntoLocalCoordinate(pt,dRotatedegree);	
	m_fltServiceLocations = m_originalFltServiceLocations;
	m_fltServiceLocations.ConvertPointIntoLocalCoordinate(pt, dRotatedegree);

}
CPoint2008 CFlightServiceRoute::GetRingRouteEntryPoint()
{

	//int nCount = m_vVehicleRoutePoint.size();
	//ASSERT(nCount > 0);

	//return m_ringRoutePath.getPoint(0);

CPoint2008 pt;
return pt;
}
CPoint2008 CFlightServiceRoute::GetRingRouteExitPoint()
{
	CPoint2008 pt;
	return pt;
	//int nCount = m_ringRoad.GetPath().getCount();
	//ASSERT(nCount > 0);

	//return m_ringRoad.GetPath().getPoint(nCount -1);
}
void CFlightServiceRoute::ClearVehicleOccupyEntryPoint(AirsideVehicleInSim *pVehicle)
{
	//std::vector<FlightServiceLocationInSim>::iterator iterEntryPoint = m_vFltServiceLoctInSim.begin();

	//for (;iterEntryPoint != m_vFltServiceLoctInSim.end(); ++ iterEntryPoint)
	//{
	//	if((*iterEntryPoint).m_pOccupyVehicle == pVehicle)
	//	{
	//		(*iterEntryPoint).m_pOccupyVehicle = NULL;

	//	}
	//}
}
void CFlightServiceRoute::WriteVehicleLog(AirsideVehicleInSim *pVehicle,ElapsedTime ePreTime,DistanceUnit dPreDist, DistanceUnit dCurDist,bool bPushBack)
{

	CPath2008 vehiclePath = GetSubPath(m_ringRoad.GetPath() ,dPreDist,dCurDist);
	for(int i=0;i<vehiclePath.getCount();i++)
	{
		DistanceUnit dist = vehiclePath.GetIndexDist((float)i);
		ElapsedTime dT = ElapsedTime(dist / pVehicle->GetOnRouteSpeed());
		ASSERT(dT >= ElapsedTime(0.0));
		pVehicle->WirteLog( vehiclePath.getPoint(i), pVehicle->GetSpeed(), ePreTime+dT,bPushBack);
		pVehicle->SetTime(ePreTime+dT);
	}
}
CPath2008 CFlightServiceRoute::GetRingRoutePath()
{
	return m_ringRoad.GetPath();

}
void CFlightServiceRoute::WriteVehicleLog(AirsideVehicleInSim *pVehicle,ElapsedTime ePreTime,CPoint2008& dPrePoint, CPoint2008& dCurPoint,bool bPushBack /* = false */)
{
	CPath2008 ringPath = GetRingRoutePath();
	DistancePointPath3D predistptPath;
	predistptPath.GetSquared(dPrePoint,ringPath);
	DistanceUnit dPreDistInPath = ringPath.GetIndexDist(static_cast<float>(predistptPath.m_fPathParameter));

	DistancePointPath3D curdistptPath;
	curdistptPath.GetSquared(dCurPoint,ringPath);
	DistanceUnit dCurDistInPath =  ringPath.GetIndexDist(static_cast<float>(curdistptPath.m_fPathParameter));

	WriteVehicleLog(pVehicle,ePreTime,dPreDistInPath,dCurDistInPath,bPushBack);


}
void CFlightServiceRoute::WakeupTheWaitingVehicleInRingRoad(const ElapsedTime &eTime)
{
	m_VehicleParkingPlaceMgr.ExitParkingPlace(this,eTime);
	m_bArrivalAtStand = true;
	m_tArrivalAtStand = eTime;
// 	for (size_t i =0; i< m_vVehicleTempPark.size(); ++i)
// 	{
// 		AirsideVehicleInSim *pInVehicle = m_vVehicleTempPark[i].pVehicle;
// 		
// 		if (pInVehicle->GetTime() >= eTime)
// 		{
// 			pInVehicle->WirteLog(pInVehicle->GetPosition(), pInVehicle->GetSpeed(), pInVehicle->GetTime());
// 			pInVehicle->SetMode(OnVehicleMoveOutTempParking);
// 			GenerateNextEvent(pInVehicle,pInVehicle->GetTime() + ElapsedTime(0.01 * i));
// 		}
// 		else
// 		{
// 			pInVehicle->WirteLog(pInVehicle->GetPosition(),pInVehicle->GetSpeed(),eTime);
// 			pInVehicle->SetMode(OnVehicleMoveOutTempParking);
// 			GenerateNextEvent(pInVehicle,eTime +ElapsedTime(0.01 * i));
// 		}
// 	}
}

bool  CFlightServiceRoute::CheckTheServiceFlightAllServiceCompleted(AirsideFlightInSim *pFlight)
{	
	bool bAllServiced = true;

	if (pFlight == NULL)
		return bAllServiced;
	
	

	//std::vector<FlightServiceLocationInSim>::iterator iterEntryPoint = m_vFltServiceLoctInSim.begin();

	//for (;iterEntryPoint != m_vFltServiceLoctInSim.end(); ++ iterEntryPoint)
	//{
	//	if(!(*iterEntryPoint).m_bHasServiced)
	//	{
	//		bAllServiced =false;
	//		break;
	//	}
	//	
	//}

	return bAllServiced;
}
int CFlightServiceRoute::GetVehiclePostionInRingRoute(CPoint2008 pt)
{

	return 0;
}
int CFlightServiceRoute::GetVehiclePostionInRoute(AirsideVehicleInSim *pVehicle)
{
	//for (size_t i =0; i< m_vVehicleRoutePoint.size();++i)
	//{
	//	if (m_vVehicleRoutePoint[i].pOccupyVehicle == pVehicle)
	//		return i;
	//}

	return pVehicle->m_pCurrentPointIndex;
}
//0, cann't move forward
//1. can move  to next point
//2. exit
int CFlightServiceRoute::TheVehicleCanMoveForward(AirsideVehicleInSim *pVehicle,AirsideVehicleInSim *& pPreVehicle,int& nextPointIndex)
{
	
	int nCurIndex = GetVehiclePostionInRoute(pVehicle);
	
	if (nCurIndex == -1) //error
	{
		ASSERT(0);
	}

	if (nCurIndex == m_vVehicleRoutePoint.size() -1)
	{
		return 2;
	}


	//Get the vehicle ahead
	AirsideVehicleInSim *pVehicleAhead = NULL;
	int nPointIndexVehicleAhead = nCurIndex + 1;
	for (; nPointIndexVehicleAhead< (int)m_vVehicleRoutePoint.size();++nPointIndexVehicleAhead)
	{
		if (m_vVehicleRoutePoint[nPointIndexVehicleAhead].pOccupyVehicle != NULL)
		{
			pVehicleAhead = m_vVehicleRoutePoint[nPointIndexVehicleAhead].pOccupyVehicle;
			break;
		}
	}
	//nextPointIndex = nCurIndex + 1;
	if(pVehicleAhead)
	{
		int nLength = GetVehicleLength(pVehicleAhead);
		if(nCurIndex + nLength >= nPointIndexVehicleAhead)
		{	
			nextPointIndex = nPointIndexVehicleAhead;
			pPreVehicle = pVehicleAhead;
			return 0;
		}
		;
	//	CPoint2008 nextPoint;
		//if (m_vVehicleRoutePoint[nextPointIndex].pOccupyVehicle != NULL)
		//{
			//pPreVehicle = m_vVehicleRoutePoint[nextPointIndex].pOccupyVehicle;
			//return 0;
		//}
	}
	
	nextPointIndex = nCurIndex + 1;
	
	return 1;
}
//AirsideVehicleInSim * CFlightServiceRoute::IsThePointOccupanyByVehicle(AirsideVehicleInSim*pVehicle, CPoint2008 pt)
//{
//	std::vector<CAirsideMobileElement *> vMobileElement = GetInResouceMobileElement();
//	std::vector<CAirsideMobileElement *>::iterator iter= vMobileElement.begin();
//	for (;iter != vMobileElement.end();++iter)
//	{
//		AirsideVehicleInSim *pInVehicle = reinterpret_cast<AirsideVehicleInSim *>(*iter);
//		if(pInVehicle == pVehicle)
//			continue;
//
//
//		if(pInVehicle->GetPosition() == pt )
//		{
//			return pInVehicle;
//		}
//	}
//
//	return NULL;
//}

int  CFlightServiceRoute::GetTempParkingPoint()
{
	//get head position
	int nHeadPosition = 0;
	for (size_t i =0; i< m_vVehicleRoutePoint.size();++i)
	{
		if (m_vVehicleRoutePoint[i].point == m_ptHead)
		{
			nHeadPosition = i;
			break;
		}
	}

	int nRemainCount = (int)m_vVehicleRoutePoint.size() - nHeadPosition;
	int nMaxCicle = nRemainCount > nHeadPosition ? nRemainCount: nHeadPosition;

	CPoint2008 ptRet;
	for (int j =0; j< nMaxCicle; j+=1)
	{
		if (nHeadPosition - j >= 0)
		{
			if(m_vVehicleRoutePoint[nHeadPosition -j].pTempParkingVehicle == NULL)
			{
				return nHeadPosition -j;
			}
		}

		if (nHeadPosition +j < (int)m_vVehicleRoutePoint.size())
		{
			if(m_vVehicleRoutePoint[nHeadPosition +j].pTempParkingVehicle == NULL)
			{
				return nHeadPosition +j;
			}
		}
	}


	return -1;

}

bool CFlightServiceRoute::IsTheParkingPointOccupany(CPoint2008 pt)
{
	//for (size_t i =0; i< m_vVehicleTempPark.size(); ++i)
	//{
	//	if (m_vVehicleTempPark[i].pt == pt )
	//		return true;
	//}

	return false;
}
bool CFlightServiceRoute::CheckServicePoint(AirsideVehicleInSim *pVehicle, CPoint2008 ptNext)
{
	//CPoint2008 ptCurrent = pVehicle->GetPosition();

	////calculate current dist in path
	//DistanceUnit ptCurDist = 0.0;
	//DistanceUnit ptNextDist =0.0;
	//CPoint2008 pt = m_ringRoutePath.getPoint(0);
	//for (int i = 1; i< m_ringRoutePath.getCount(); ++i)
	//{
	//	if(m_ringRoutePath.getPoint(i) == ptCurrent)
	//		break;

	//	ptCurDist += pt.distance(m_ringRoutePath.getPoint(i));

	//	pt = m_ringRoutePath.getPoint(i);
	//}

	//ptCurDist += pt.distance(ptCurrent);
	//ptNextDist = ptCurDist + ptCurrent.distance(ptNext);

	////check the service point in the distance range


	//	std::vector<FlightServiceLocationInSim>::iterator iterEntryPoint = m_vFltServiceLoctInSim.begin();
	//	
	//	for (;iterEntryPoint != m_vFltServiceLoctInSim.end(); ++ iterEntryPoint)
	//	{
	//		if((*iterEntryPoint).m_dDistInPath >= ptCurDist && (*iterEntryPoint).m_dDistInPath <= ptNextDist )
	//		{
	//			if((*iterEntryPoint).m_bHasServiced)
	//			{

	//			}
	//			else if(pVehicle->GetVehicleTypeID() == (*iterEntryPoint).m_serviceLocation.GetTypeID())
	//			{
	//				//generate on service event
	//				if ((*iterEntryPoint).m_pOccupyVehicle == NULL && (*iterEntryPoint).m_pServiceVehicle == NULL)
	//				{
	//				
	//					CPoint2008 vehiclePos = pVehicle->GetPosition();
	//					double vehicleSpeed = pVehicle->GetSpeed();

	//					(*iterEntryPoint).m_exitPoint = ptNext;
	//					ptNext = (*iterEntryPoint).m_entryPoint;


	//					ElapsedTime elapMoveTime = ElapsedTime((ptNext.getDistanceTo(vehiclePos)/vehicleSpeed));

	//					pVehicle->WirteLog(ptNext,pVehicle->GetTime() + elapMoveTime);

	//					pVehicle->SetPosition(ptNext);

	//					pVehicle->SetTime(pVehicle->GetTime() + elapMoveTime);							
	//					pVehicle->SetMode(OnService);

	//					GenerateNextEvent(pVehicle,pVehicle->GetTime());
	//					return true;
	//				}
	//			}
	//		}
	//	}


	return false;
}

void CFlightServiceRoute::RemoveVehicleTempParkList(AirsideVehicleInSim *pVehicle)
{
	for (size_t i =0; i< m_vVehicleTempPark.size(); ++i)
	{
		if (m_vVehicleTempPark[i].pVehicle == pVehicle )
		{
			m_vVehicleTempPark.erase(m_vVehicleTempPark.begin() + i);
		}
	}
}
bool CFlightServiceRoute::IsTheVehicleInTempParkList(AirsideVehicleInSim *pVehicle)
{
	for (size_t i =0; i< m_vVehicleTempPark.size(); ++i)
	{
		if (m_vVehicleTempPark[i].pVehicle == pVehicle )
			return true;
	}
	return false;
}
bool CFlightServiceRoute::IsExistServicePointVehicleCanServe(AirsideVehicleInSim *pVehicle)
{
	std::vector<VehicleRoutePoint>::iterator iterRoutePoint = m_vVehicleRoutePoint.begin();

	for (;iterRoutePoint != m_vVehicleRoutePoint.end(); ++ iterRoutePoint)
	{
		if ((*iterRoutePoint).HasServicePoint(pVehicle))
			return true;
	}

	return false;
}
bool CFlightServiceRoute::SetVehicleServicePointServiced(int nVehicleTypeID)
{
	//std::vector<FlightServiceLocationInSim>::iterator iterEntryPoint = m_vFltServiceLoctInSim.begin();

	//for (;iterEntryPoint != m_vFltServiceLoctInSim.end(); ++ iterEntryPoint)
	//{

	//	if((*iterEntryPoint).m_bHasServiced)
	//	{

	//	}
	//	else if(nVehicleTypeID == (*iterEntryPoint).m_serviceLocation.GetTypeID())
	//	{
	//		(*iterEntryPoint).m_bHasServiced = true;
	//	}

	//}

	return true;
}
void CFlightServiceRoute::SetUnreachableVehicleType(int vehicleType)
{
	m_vUnreachableVehicleType.push_back(vehicleType);
	SetVehicleServicePointServiced(vehicleType);
}
int CFlightServiceRoute::GetFltServicePointCount(int nVehicleTypeID)
{
	return m_originalFltServiceLocations.GetElementCount(nVehicleTypeID);
}

#include "..\..\Results\AirsideFlightEventLog.h"
void CFlightServiceRoute::getDesc( ResourceDesc& resDesc )
{
	resDesc.resid = 0;
	resDesc.resType = GetType();
	resDesc.strRes = PrintResource();
}

int CFlightServiceRoute::GetVehicleLength( AirsideVehicleInSim *pVehicle )
{
	ASSERT(pVehicle != NULL);
	if(pVehicle == NULL)
		return 1;

	if(m_mapVehicleLength.find((int)pVehicle->GetType()) != m_mapVehicleLength.end())
		return m_mapVehicleLength[(int)pVehicle->GetType()];

	//calculate the vehicle length
	
	if(m_ringRoad.GetPath().getCount() < 2)
		return 1;

	CPoint2008 pt1 = m_ringRoad.GetPath().getPoint(0);
	CPoint2008 pt2 = m_ringRoad.GetPath().getPoint(1);

	double dDistance = pt1.distance(pt2);
	//
	double nVehicleLength = pVehicle->GetVehicleLength();
	
	int nPointCount = nVehicleLength/dDistance > 0.0 ? static_cast<int>(nVehicleLength/dDistance) + 1:1;

	m_mapVehicleLength[(int)pVehicle->GetType()] = nPointCount;


	return nPointCount;

}

void CFlightServiceRoute::VehicleOnMoveToRingRoute( AirsideVehicleInSim * pVehicle, ClearanceItem& lastItem, DistanceUnit radius, Clearance& newClearance )
{
	//can enter the resource
	if(pVehicle->GetResource())
		pVehicle->GetResource()->SetExitTime(pVehicle,pVehicle->GetTime());

	AirsideVehicleInSim * pPreVehicle = m_vVehicleRoutePoint[0].pOccupyVehicle;
	if ( pPreVehicle!= NULL )//&& pPreVehicle != pVehicle)
	{
		//pVehicle->SetMode(OnVehicleMoveToTempParking);
		pVehicle->WirteLog(pVehicle->GetPosition(),pVehicle->GetSpeed(),pPreVehicle->GetTime());

		//GenerateNextEvent(pVehicle,pPreVehicle->GetTime());
		//if(m_vVehicleRoutePoint[0].pWaitingVehicle == NULL)
		//{
		//	m_vVehicleRoutePoint[0].pWaitingVehicle = pVehicle;
		//	pVehicle->m_pCurrentPointIndex = 0;
		//}
		//else
		{
			if(pVehicle->GetTime()< pPreVehicle->GetTime())
			{
				pVehicle->SetTime(pPreVehicle->GetTime()+ ElapsedTime(0.01));
			//	GenerateNextEvent(pVehicle,pPreVehicle->GetTime() + ElapsedTime(0.01));
				m_vVehicleRoutePoint[0].AddWaitingVehicle(pVehicle);
			}
			else
			{
				pVehicle->SetTime(pVehicle->GetTime() + ElapsedTime(0.01));
			//	GenerateNextEvent(pVehicle,pVehicle->GetTime());
				m_vVehicleRoutePoint[0].AddWaitingVehicle(pVehicle);
			}
		}
		return;
	}

	pVehicle->SetResource(this);
	//calculate enter time
	CPoint2008 entryPoint = m_vVehicleRoutePoint[0].point;
	CPoint2008 vehiclePos = pVehicle->GetPosition();
	double vehicleSpeed = pVehicle->GetSpeed();
	if (vehicleSpeed <=0)
		vehicleSpeed = pVehicle->GetOnRouteSpeed();
	ElapsedTime elapMoveTime = ElapsedTime((entryPoint.getDistanceTo(vehiclePos)/vehicleSpeed));


	SetEnterTime(pVehicle,elapMoveTime+ pVehicle->GetTime(),OnMoveToRingRoute);

	pVehicle->WirteLog(pVehicle->GetPosition(),vehicleSpeed,pVehicle->GetTime());

	pVehicle->WirteLog(entryPoint, vehicleSpeed, elapMoveTime + pVehicle->GetTime());

	//set next vehicle new mode
	pVehicle->SetPosition(entryPoint);
	pVehicle->SetMode(OnServiceMovement);
	pVehicle->SetDistInResource(10000L);
	pVehicle->SetTime(elapMoveTime+ pVehicle->GetTime());
	m_vVehicleRoutePoint[0].pOccupyVehicle = pVehicle;
	pVehicle->m_pCurrentPointIndex = 0;

	pVehicle->WirteLog(entryPoint, vehicleSpeed,pVehicle->GetTime());	
	//generate next event
	//GenerateNextEvent(pVehicle,pVehicle->GetTime());


	if(m_vVehicleRoutePoint.size() > 0 && (m_vVehicleRoutePoint[0].TheVehicleCanService(pVehicle,this) == 1)&& !pVehicle->IsArrivedAtStand())
	{
		//generate next event
		//GenerateNextEvent(pVehicle,pVehicle->GetTime());
		//current point
	}
	else if(m_vVehicleRoutePoint.size() > 0)
	{
		CPoint2008 ptNext = m_vVehicleRoutePoint[0].point;

		ElapsedTime dT = ElapsedTime(ptNext.distance(pVehicle->GetPosition()) / pVehicle->GetOnRouteSpeed());

		pVehicle->SetPosition(ptNext);
		pVehicle->SetMode(OnServiceMovement);

		ASSERT(dT >= ElapsedTime(0.0));
		pVehicle->WirteLog( ptNext, pVehicle->GetSpeed(),pVehicle->GetTime()+dT,false);
		pVehicle->SetTime( pVehicle->GetTime()+dT);
		GenerateNextEvent(pVehicle,pVehicle->GetTime());
	}
}

void CFlightServiceRoute::VehicleOnServiceMovement( AirsideVehicleInSim * pVehicle, ClearanceItem& lastItem, DistanceUnit radius, Clearance& newClearance )
{
	AirsideFlightInSim *pFlight = pVehicle->GetServiceFlight();
	if (/*pFlight && pFlight->GetMode() >=OnHeldAtStand*/m_bArrivalAtStand)
	{

	}
// 	else //sleep and waiting the flight arrival
// 	{
// 		if(pFlight && pFlight->GetMode() > OnHeldAtStand )
// 		{
// 			
// 		}
	else
	{
 		pVehicle->SetMode(OnVehicleMoveToTempParking);
 		pVehicle->WirteLog( pVehicle->GetPosition(), pVehicle->GetSpeed() ,pVehicle->GetTime());
 		pVehicle->SetTime(pVehicle->GetTime());

 		GenerateNextEvent(pVehicle,pVehicle->GetTime());
		m_VehicleParkingPlaceMgr.EnterParkingPlace(pVehicle);
		return ;
	}

//	}
	//
	AirsideVehicleInSim *pPreVehicle = NULL;
	int nNextPointIndex = 0;
	int nRet = TheVehicleCanMoveForward(pVehicle,pPreVehicle,nNextPointIndex);
	ASSERT(nNextPointIndex >= 0);
	if (nRet == 0) // cann't
	{
		if(pPreVehicle != NULL)
		{
			m_vVehicleRoutePoint[nNextPointIndex].pOccupyVehicle = pPreVehicle;
			m_vVehicleRoutePoint[nNextPointIndex].AddWaitingVehicle(pVehicle);

			if(pVehicle->IsArrivedAtStand())
			{
				m_vVehicleRoutePoint[nNextPointIndex].WakeupWaitingVehicle(this);
				pVehicle->SetMode(OnLeaveRingRoute);
				pVehicle->WirteLog( pVehicle->GetPosition(), pVehicle->GetSpeed() ,pVehicle->GetTime());
				pVehicle->SetTime(pVehicle->GetTime());

				GenerateNextEvent(pVehicle,pVehicle->GetTime());
			}
			return ;
		}
		//ofsstream echoFile ("c:\\CFlightServiceRoute.log", stdios::app);
		//echoFile <<1<<"\r\n";
		//echoFile.close();
	}
	else if (nRet == 1)// can move
	{
		//ofsstream echoFile ("c:\\CFlightServiceRoute.log", stdios::app);
		//echoFile <<2<<"\r\n";
		//echoFile.close();
		ASSERT(nNextPointIndex > 0);
		if(nNextPointIndex <= 0)
			nNextPointIndex = 0;



		int nCanService = -1;

		if( (nCanService = m_vVehicleRoutePoint[nNextPointIndex].TheVehicleCanService(pVehicle,this)) && !pVehicle->IsArrivedAtStand())
		{
			if(nCanService == 1)
			{
				m_vVehicleRoutePoint[nNextPointIndex -1].WakeupWaitingVehicle(this);
				m_vVehicleRoutePoint[nNextPointIndex -1].pOccupyVehicle = NULL;
				pVehicle->m_pCurrentPointIndex = nNextPointIndex;
			}
			else if(nCanService == 2)
			{
				m_vVehicleRoutePoint[nNextPointIndex].AddWaitingVehicle(pVehicle);
			}
			//current point
			return ;
		}
		

		m_vVehicleRoutePoint[nNextPointIndex -1].WakeupWaitingVehicle(this);
		m_vVehicleRoutePoint[nNextPointIndex -1].pOccupyVehicle = NULL;
		pVehicle->m_pCurrentPointIndex = nNextPointIndex;


		m_vVehicleRoutePoint[nNextPointIndex].pOccupyVehicle = pVehicle;
		CPoint2008 ptNext = m_vVehicleRoutePoint[nNextPointIndex].point;

		ElapsedTime dT = ElapsedTime(ptNext.distance(pVehicle->GetPosition()) / pVehicle->GetOnRouteSpeed());

		pVehicle->WirteLog(pVehicle->GetPosition(),pVehicle->GetSpeed(),pVehicle->GetTime());

		pVehicle->SetPosition(ptNext);
		pVehicle->SetMode(OnServiceMovement);

		ASSERT(dT >= ElapsedTime(0.0));
		pVehicle->WirteLog( ptNext, pVehicle->GetSpeed(),pVehicle->GetTime()+dT,false);
		pVehicle->SetTime( pVehicle->GetTime()+dT);
		GenerateNextEvent(pVehicle,pVehicle->GetTime());

	}
	else if (nRet == 2)//has move to the last one
	{	
		//ofsstream echoFile ("c:\\CFlightServiceRoute.log", stdios::app);
		//echoFile <<3<<"\r\n";
		//echoFile.close();
		m_vVehicleRoutePoint[m_vVehicleRoutePoint.size() -1].WakeupWaitingVehicle(this);

		if(IsTheVehicleInTempParkList(pVehicle) && IsExistServicePointVehicleCanServe(pVehicle))// the vehicle move to the entry point
		{
			//{
			//	ofsstream echoFile ("c:\\CFlightServiceRoute.log", stdios::app);
			//	echoFile <<4<<"\r\n";
			//	echoFile.close();
			//}
			CPoint2008 ptNext = m_vVehicleRoutePoint[0].point;
			pPreVehicle = m_vVehicleRoutePoint[0].pOccupyVehicle;
			pVehicle->m_pCurrentPointIndex  = 0;
			//{
			//	ofsstream echoFile ("c:\\CFlightServiceRoute.log", stdios::app);
			//	echoFile <<8<<"   :"<<(int)pPreVehicle<<"\r\n";
			//	echoFile.close();
			//}
			if(pPreVehicle == NULL)
			{	

				if(m_vVehicleRoutePoint[0].TheVehicleCanService(pVehicle,this) == 1)
				{
					//ofsstream echoFile ("c:\\CFlightServiceRoute.log", stdios::app);
					//echoFile <<9<<"\r\n";
					//echoFile.close();
					//current point
					//m_vVehicleRoutePoint[nNextPointIndex -1].pOccupyVehicle = NULL;
					//AirsideVehicleInSim *pWaitingVehicle = m_vVehicleRoutePoint[nNextPointIndex -1].pWaitingVehicle;
					//if(pWaitingVehicle != NULL)//wake up the waiting vehicle
					//{
					//	//ofsstream echoFile ("c:\\CFlightServiceRoute.log", stdios::app);
					//	//echoFile <<10<<"\r\n";
					//	//echoFile.close();
					//	pWaitingVehicle->SetMode(OnServiceMovement);
					//	GenerateNextEvent(pWaitingVehicle,pVehicle->GetTime());
					//	//m_vVehicleRoutePoint[nNextPointIndex -1].pWaitingVehicle = NULL;
					//}
					return ;
				}
				//ofsstream echoFile ("c:\\CFlightServiceRoute.log", stdios::app);
				//echoFile <<5<<"\r\n";
				//echoFile.close();
				//move to next point
				double vehicleSpeed = pVehicle->GetSpeed();
				pVehicle->WirteLog(pVehicle->GetPosition(),pVehicle->GetSpeed(),pVehicle->GetTime());

				ElapsedTime elapMoveTime = ElapsedTime((pVehicle->GetPosition().getDistanceTo(ptNext)/vehicleSpeed));

				pVehicle->SetPosition(ptNext);
				pVehicle->SetMode(OnServiceMovement);
				pVehicle->WirteLog(ptNext,pVehicle->GetSpeed(),pVehicle->GetTime() + elapMoveTime,false);		
				pVehicle->SetTime(pVehicle->GetTime() + elapMoveTime);

				GenerateNextEvent(pVehicle,pVehicle->GetTime());



			}
			else
			{
				//ofsstream echoFile ("c:\\CFlightServiceRoute.log", stdios::app);
				//echoFile <<6<<"\r\n";
				//echoFile.close();
				pVehicle->WirteLog(pVehicle->GetPosition(),pVehicle->GetSpeed(),pVehicle->GetTime(),true);
				pVehicle->WirteLog(pVehicle->GetPosition(),pVehicle->GetSpeed(),pPreVehicle->GetTime(),true);
				pVehicle->SetTime(pPreVehicle->GetTime());
				pVehicle->SetMode(OnServiceMovement);
				GenerateNextEvent(pVehicle,pVehicle->GetTime());
			}

		}
		else //move out the ring route
		{
			//ofsstream echoFile ("c:\\CFlightServiceRoute.log", stdios::app);
			//echoFile <<7<<"\r\n";
			//echoFile.close();	
			pVehicle->WirteLog(pVehicle->GetPosition(),pVehicle->GetSpeed(),pVehicle->GetTime(),false);
			pVehicle->SetMode(OnLeaveRingRoute);
			pVehicle->WirteLog(pVehicle->GetPosition(),pVehicle->GetSpeed(),pVehicle->GetTime(),false);		
			pVehicle->SetTime(pVehicle->GetTime());

			GenerateNextEvent(pVehicle,pVehicle->GetTime());
		}
	}
}

void CFlightServiceRoute::VehicleOnVehicleMoveToTempParking( AirsideVehicleInSim * pVehicle, ClearanceItem& lastItem, DistanceUnit radius, Clearance& newClearance )
{
	m_vVehicleRoutePoint[0].WakeupWaitingVehicle(this);
	double vehicleSpeed = pVehicle->GetSpeed();
	CPoint2008 ptNext;
	CPoint2008 vDir;
	m_VehicleParkingPlaceMgr.GetVehiclePosition(pVehicle,ptNext);
	m_VehicleParkingPlaceMgr.GetVehicleDirection(pVehicle,vDir);

	ElapsedTime elapMoveTime = ElapsedTime((pVehicle->GetPosition().getDistanceTo(ptNext)/vehicleSpeed));

	pVehicle->SetPosition(ptNext);
	pVehicle->SetMode(OnVehicleMoveToTempParking);
	pVehicle->WirteLog(ptNext,pVehicle->GetSpeed(),pVehicle->GetTime() + elapMoveTime,true);		
	pVehicle->SetTime(pVehicle->GetTime() + elapMoveTime);	

	elapMoveTime += ElapsedTime((pVehicle->GetPosition().getDistanceTo(vDir)/vehicleSpeed));

	pVehicle->SetPosition(vDir);
	pVehicle->SetMode(OnVehicleMoveToTempParking);
	pVehicle->WirteLog(vDir,pVehicle->GetSpeed(),pVehicle->GetTime() + elapMoveTime,true);		
	pVehicle->SetTime(pVehicle->GetTime() + elapMoveTime);	


	//int nPointIndex = GetTempParkingPoint();
	////ASSERT( nPointIndex > 0);
	//if(nPointIndex < 0 )
	//	nPointIndex = 0;
	//CPoint2008 ptCur = pVehicle->GetPosition();

	//CPoint2008 ptNext;
	//for (int i =0; i <= nPointIndex; ++i)
	//{
	//	ptNext = m_vVehicleRoutePoint[i].point;

	//	if (i == nPointIndex)
	//	{	
	//		ElapsedTime elapMoveTime = ElapsedTime((pVehicle->GetPosition().getDistanceTo(ptNext)/vehicleSpeed));

	//		pVehicle->SetPosition(ptNext);
	//		pVehicle->SetMode(OnVehicleMoveToTempParking);
	//		pVehicle->WirteLog(ptNext,pVehicle->GetSpeed(),pVehicle->GetTime() + elapMoveTime,false);		
	//		pVehicle->SetTime(pVehicle->GetTime() + elapMoveTime);				

	//		break;
	//	}
	//	else
	//	{
	//		ElapsedTime elapMoveTime = ElapsedTime((pVehicle->GetPosition().getDistanceTo(ptNext)/vehicleSpeed));

	//		pVehicle->SetPosition(ptNext);
	//		pVehicle->SetMode(OnVehicleMoveToTempParking);
	//		pVehicle->WirteLog(ptNext,pVehicle->GetSpeed(),pVehicle->GetTime() + elapMoveTime,false);		
	//		pVehicle->SetTime(pVehicle->GetTime() + elapMoveTime);
	//	}
	//}
	//m_vVehicleRoutePoint[0].WakeupWaitingVehicle(this);
	//pVehicle->m_pCurrentPointIndex  = nPointIndex;
	//m_vVehicleRoutePoint[nPointIndex].pTempParkingVehicle = pVehicle;
	//double dLength = pVehicle->GetVehicleLength();

	//ASSERT(m_pFlightInSim != NULL);
	//StandInSim *pStandInSim = m_pFlightInSim->GetOperationParkingStand();


	//if (pStandInSim == NULL)
	//	return ;
	//StandLeadInLineInSim* pLeadInLineInSim = pStandInSim->AssignLeadInLine(m_pFlightInSim);
	//if(pLeadInLineInSim == NULL)
	//	return;

	//double dFlightLength = m_pFlightInSim->GetLength();
	//DistanceUnit LeadInLineEndDist = pLeadInLineInSim->GetEndDist();
	//CPoint2008 fltPos = pLeadInLineInSim->GetDistancePoint(LeadInLineEndDist- dFlightLength*0.5);
	////central pos
	////CPoint2008 fltPos = pLeadInLineInSim->GetDistancePoint(pLeadInLineInSim->GetEndDist());

	//CPoint2008 ptPark;
	//ptPark =  m_vVehicleRoutePoint[nPointIndex].point - fltPos ;
	//ptPark.length(dLength +dLength *0.5);

	//ptNext = ptPark + m_vVehicleRoutePoint[nPointIndex].point;

	//ElapsedTime elapMoveTime = ElapsedTime((pVehicle->GetPosition().getDistanceTo(ptNext)/vehicleSpeed));


	//pVehicle->SetPosition(ptNext);
	//pVehicle->SetMode(OnWaitingFlightArrive);
	//pVehicle->WirteLog(ptNext,pVehicle->GetSpeed(),pVehicle->GetTime() + elapMoveTime,false);		
	//pVehicle->SetTime(pVehicle->GetTime() + elapMoveTime);
	////GenerateNextEvent(pVehicle,pVehicle->GetTime());

	////add into waiting list
	//VehicleTempPark tempPark;
	////tempPark.pt = m_vVehicleRoutePoint[nPointIndex].point;
	//tempPark.nRoutepointIndex = nPointIndex;
	//tempPark.pVehicle = pVehicle;

	////push back to the right place
	//int nTempParkingVehicleCount = (int)m_vVehicleTempPark.size();
	//if(nTempParkingVehicleCount == 0)
	//	m_vVehicleTempPark.push_back(tempPark);
	//else
	//{

	//	std::vector<VehicleTempPark>::iterator iter = m_vVehicleTempPark.begin();
	//	std::vector<VehicleTempPark>::iterator iterInsert = iter;
	//	for (;iter != m_vVehicleTempPark.end(); ++iter)
	//	{
	//		if((*iter).nRoutepointIndex < nPointIndex)
	//		{
	//			m_vVehicleTempPark.insert(iterInsert,tempPark);
	//			break;
	//		}
	//		else
	//		{
	//			iterInsert = iter;
	//		}
	//	}
	//	if(iter == m_vVehicleTempPark.end())
	//		m_vVehicleTempPark.push_back(tempPark);

	//}
}

void CFlightServiceRoute::VehicleOnVehicleMoveOutTempParking( AirsideVehicleInSim * pVehicle, ClearanceItem& lastItem, DistanceUnit radius, Clearance& newClearance )
{
	int nPointIndex = pVehicle->m_pCurrentPointIndex;
	double vehicleSpeed = pVehicle->GetSpeed();
	AirsideVehicleInSim *pPreVehicle = m_vVehicleRoutePoint[nPointIndex].pOccupyVehicle;
	m_bArrivalAtStand = true;

	if (pPreVehicle == NULL || pPreVehicle == pVehicle)
	{		
		int nNextPointIndex = 0;
		int nRet = TheVehicleCanMoveForward(pVehicle,pPreVehicle,nNextPointIndex);
		if(nRet == 0)
		{
			m_vVehicleRoutePoint[nNextPointIndex].AddWaitingVehicle(pVehicle);
			m_vVehicleRoutePoint[nPointIndex].pOccupyVehicle = pVehicle;
		}
		else
		{
			CPoint2008 ptNext = m_vVehicleRoutePoint[nPointIndex].point;
			//move to the next point
			pVehicle->WirteLog(pVehicle->GetPosition(),pVehicle->GetSpeed(),pVehicle->GetTime(),false);

			ElapsedTime elapMoveTime = ElapsedTime((pVehicle->GetPosition().getDistanceTo(ptNext)/vehicleSpeed));
			pVehicle->SetPosition(ptNext);
			pVehicle->SetMode(OnServiceMovement);
			pVehicle->WirteLog(ptNext,pVehicle->GetSpeed(),pVehicle->GetTime() + elapMoveTime,false);		
			pVehicle->SetTime(pVehicle->GetTime() + elapMoveTime);
			m_vVehicleRoutePoint[nPointIndex].pOccupyVehicle = pVehicle;

			GenerateNextEvent(pVehicle,pVehicle->GetTime());

			SetEnterTime(pVehicle,pVehicle->GetTime(),pVehicle->GetMode());
		}



	}
	else
	{
//  		pVehicle->WirteLog(pVehicle->GetPosition(),pVehicle->GetSpeed(),pPreVehicle->GetTime(),true);
//  		pVehicle->SetTime(pPreVehicle->GetTime());
		pVehicle->SetMode(OnVehicleMoveOutTempParking);
		m_vVehicleRoutePoint[nPointIndex].AddWaitingVehicle(pVehicle);
	//	GenerateNextEvent(pVehicle,pVehicle->GetTime());
	}
}

void CFlightServiceRoute::VehicleOnService( AirsideVehicleInSim * pVehicle, ClearanceItem& lastItem, DistanceUnit radius, Clearance& newClearance )
{
	SetExitTime(pVehicle,pVehicle->GetTime());

	int nCurPointIndex = pVehicle->m_pCurrentPointIndex;
	ASSERT(nCurPointIndex >=0);
	CPoint2008 ptEntryPoint = m_vVehicleRoutePoint[nCurPointIndex].point;
	m_vVehicleRoutePoint[nCurPointIndex].WakeupWaitingVehicle(this);
	m_vVehicleRoutePoint[nCurPointIndex].pOccupyVehicle = NULL;
	{
		std::vector<FlightServiceLocationInSim>::iterator iterEntryPoint = m_vVehicleRoutePoint[nCurPointIndex].vServicePoint.begin();

		for (;iterEntryPoint != m_vVehicleRoutePoint[nCurPointIndex].vServicePoint.end(); ++ iterEntryPoint)
		{
			if ( (pVehicle->GetVehicleBaseType() == VehicleType_PaxTruck || !(*iterEntryPoint).m_bHasServiced )&&
				(*iterEntryPoint).m_serviceLocation.GetTypeID() == pVehicle->GetVehicleTypeID())
			{

				(*iterEntryPoint).m_vServiceVehicle.push_back(pVehicle);
				(*iterEntryPoint).m_pOccupyVehicle = pVehicle;
				m_CurServerPoint = (*iterEntryPoint).m_serviceLocation.GetServiceLocation();
				break;
			}

			if ( (pVehicle->GetVehicleBaseType() == VehicleType_BaggageTug || !(*iterEntryPoint).m_bHasServiced )&&
				(*iterEntryPoint).m_serviceLocation.GetTypeID() == pVehicle->GetVehicleTypeID())
			{

				(*iterEntryPoint).m_vServiceVehicle.push_back(pVehicle);
				(*iterEntryPoint).m_pOccupyVehicle = pVehicle;
				m_CurServerPoint = (*iterEntryPoint).m_serviceLocation.GetServiceLocation();
				break;
			}
		}
	}

	//move to service point
	{
		CPoint2008 vehiclePos = pVehicle->GetPosition();
		double vehicleSpeed = pVehicle->GetSpeed();
		ElapsedTime	elapMoveTime = ElapsedTime((m_CurServerPoint.getDistanceTo(vehiclePos)/vehicleSpeed));
		pVehicle->SetTime(pVehicle->GetTime() + elapMoveTime);
		pVehicle->WirteLog(m_CurServerPoint,pVehicle->GetSpeed(),pVehicle->GetTime());
		pVehicle->SetPosition(m_CurServerPoint) ;
	}

	//service  arrive at stand

	if (pVehicle->GetVehicleBaseType() == VehicleType_TowTruck)
	{
		AirsideFlightInSim* pServiceFlight = ((AirsideTowTruckInSim*)pVehicle)->GetServiceFlight();
		double dist = pServiceFlight->GetDistInResource() + (pServiceFlight->GetLength()*0.5 - pVehicle->GetVehicleLength()*0.5);				
		CPoint2008 pos = pServiceFlight->GetResource()->GetDistancePoint(dist);
		ElapsedTime dT = ElapsedTime(2.0*pos.distance(pVehicle->GetPosition())/pVehicle->GetSpeed());

		pVehicle->WirteLog(pos,0,pVehicle->GetTime() + dT,true);
		pVehicle->SetPosition(pos);
		pVehicle->SetSpeed(0);
		pVehicle->SetTime(pVehicle->GetTime() + dT);											
		pServiceFlight->WakeUp(pVehicle->GetTime());
		return ;
	}
	else if(pVehicle->GetVehicleBaseType() == VehicleType_PaxTruck)
	{
		pVehicle->IsArrivedAtStand(TRUE) ;
		pVehicle->ArriveAtStand(pVehicle->GetTime()) ;
	}
	else if(pVehicle->GetVehicleBaseType() == VehicleType_BaggageTug)
	{	
		pVehicle->IsArrivedAtStand(TRUE) ;
		pVehicle->ArriveAtStand(pVehicle->GetTime()) ;
	}
	else
	{
		//service
		{
			double dServiceTime = pVehicle->GetServiceTimeDistribution()->getRandomValue() * 60;
			pVehicle->WirteLog(m_CurServerPoint,pVehicle->GetSpeed(),pVehicle->GetTime() + ElapsedTime((dServiceTime)));
			pVehicle->SetTime(pVehicle->GetTime() + ElapsedTime((dServiceTime)));
		}

		//leave service point
		//back the entry position
		{	
			double vehicleSpeed = pVehicle->GetSpeed();
			ElapsedTime	elapMoveTime = ElapsedTime((m_CurServerPoint.getDistanceTo(ptEntryPoint)/vehicleSpeed));
			pVehicle->WirteLog(ptEntryPoint,pVehicle->GetSpeed(),pVehicle->GetTime() + elapMoveTime,true);

			pVehicle->SetPosition(ptEntryPoint);
			pVehicle->SetTime(pVehicle->GetTime() + elapMoveTime);							
			pVehicle->SetMode(OnLeaveServicePoint);

			GenerateNextEvent(pVehicle,pVehicle->GetTime());
		}
	}
}

void CFlightServiceRoute::VehicleOnLeaveServicePoint( AirsideVehicleInSim * pVehicle, ClearanceItem& lastItem, DistanceUnit radius, Clearance& newClearance )
{
	pVehicle->WirteLog(pVehicle->GetPosition(),0,pVehicle->GetTime());

	int nCurPointIndex = pVehicle->m_pCurrentPointIndex;
	CPoint2008 ptEntryPoint = m_vVehicleRoutePoint[nCurPointIndex].point;
	double vehicleSpeed = pVehicle->GetSpeed();
	ElapsedTime	elapMoveTime = ElapsedTime((pVehicle->GetPosition().getDistanceTo(ptEntryPoint)/vehicleSpeed));
	pVehicle->WirteLog(ptEntryPoint,pVehicle->GetSpeed(),pVehicle->GetTime() + elapMoveTime,true);
	pVehicle->SetPosition(ptEntryPoint);
	pVehicle->SetTime(pVehicle->GetTime() + elapMoveTime);							

	//////////////////////////////////////////////////////////////////////
	std::vector<FlightServiceLocationInSim>::iterator iterEntryPoint = m_vVehicleRoutePoint[nCurPointIndex].vServicePoint.begin();

	for (;iterEntryPoint != m_vVehicleRoutePoint[nCurPointIndex].vServicePoint.end(); ++ iterEntryPoint)
	{
		if ((*iterEntryPoint).m_pOccupyVehicle == pVehicle)
		{
			(*iterEntryPoint).m_bHasServiced = true;
			(*iterEntryPoint).m_vServiceVehicle.push_back(pVehicle);
			(*iterEntryPoint).m_pOccupyVehicle = NULL;
			//ptServiceLocation = (*iterEntryPoint).m_serviceLocation.GetServiceLocation();
			//ptEntryPoint = (*iterEntryPoint).m_entryPoint;

			////after vehicle leaves the ring road, then the service is compeleted. 
			//AirsideFlightInSim *pFlight = pVehicle->GetServiceFlight();
			//if (pFlight)
			//{
			//	pFlight->VehicleServiceComplete(pVehicle);
			//}
			//
			break;
		}
	}


	//CPoint2008 ptEntryPoint;
	//std::vector<FlightServiceLocationInSim>::iterator iterEntryPoint = m_vFltServiceLoctInSim.begin();

	//for (;iterEntryPoint != m_vFltServiceLoctInSim.end(); ++ iterEntryPoint)
	//{
	//	if ((*iterEntryPoint).m_pServiceVehicle == pVehicle)
	//	{

	CPoint2008 ptNext = m_vVehicleRoutePoint[nCurPointIndex].point;
	AirsideVehicleInSim *pPreVehicle =m_vVehicleRoutePoint[nCurPointIndex].pOccupyVehicle;

	if (pPreVehicle == NULL)
	{
		m_vVehicleRoutePoint[nCurPointIndex].pOccupyVehicle = pVehicle;
		//move out of the service point
		double vehicleSpeed = pVehicle->GetSpeed();
		ElapsedTime elapMoveTime = ElapsedTime((pVehicle->GetPosition().getDistanceTo(ptNext)/vehicleSpeed));

		pVehicle->WirteLog(ptNext,pVehicle->GetSpeed(),pVehicle->GetTime() + elapMoveTime,true);
		pVehicle->SetPosition(ptNext);
		pVehicle->SetTime(pVehicle->GetTime() + elapMoveTime);

		//move to the next point
		elapMoveTime = ElapsedTime((pVehicle->GetPosition().getDistanceTo(ptNext)/vehicleSpeed));
		pVehicle->SetPosition(ptNext);
		pVehicle->SetMode(OnServiceMovement);
		pVehicle->WirteLog(ptNext,pVehicle->GetSpeed(),pVehicle->GetTime() + elapMoveTime,false);		
		pVehicle->SetTime(pVehicle->GetTime() + elapMoveTime);

		GenerateNextEvent(pVehicle,pVehicle->GetTime());

		SetEnterTime(pVehicle,pVehicle->GetTime(),pVehicle->GetMode());

	}
	else
	{
		pVehicle->WirteLog(pVehicle->GetPosition(),pVehicle->GetSpeed(),pPreVehicle->GetTime(),true);
		pVehicle->SetTime(pPreVehicle->GetTime());
		pVehicle->SetMode(OnLeaveServicePoint);
		//GenerateNextEvent(pVehicle,pVehicle->GetTime());
		m_vVehicleRoutePoint[nCurPointIndex].AddWaitingVehicle(pVehicle);
	}


}

void CFlightServiceRoute::VehicleOnLeaveServiceMovement( AirsideVehicleInSim * pVehicle, ClearanceItem& lastItem, DistanceUnit radius, Clearance& newClearance )
{

}

void CFlightServiceRoute::VehicleOnLeaveRingRoute( AirsideVehicleInSim * pVehicle, ClearanceItem& lastItem, DistanceUnit radius, Clearance& newClearance )
{
	SetExitTime(pVehicle,pVehicle->GetTime());

	m_vVehicleRoutePoint[pVehicle->m_pCurrentPointIndex].WakeupWaitingVehicle(this);
	pVehicle->m_pCurrentPointIndex =-1;

	//pVehicle->SetMode(OnBackPool);
	//VehicleMoveEvent *pNextEvent = new VehicleMoveEvent(pVehicle);
	//pNextEvent->setTime(pVehicle->GetTime());
	//pNextEvent->addEvent();

	AirsideFlightInSim *pFlight = pVehicle->GetServiceFlight();


	//&& CheckTheServiceFlightAllServiceCompleted(pFlight)
	if (pFlight != NULL )
	{
		pFlight->VehicleServiceComplete(pVehicle);
		pFlight->AllVehicleFinishedServicing(pVehicle->GetTime());
	}

	pVehicle->GetNextCommand();
}
