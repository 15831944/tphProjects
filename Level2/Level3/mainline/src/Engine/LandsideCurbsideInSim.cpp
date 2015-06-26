#include "StdAfx.h"
#include ".\landsidecurbsideinsim.h"
#include "Landside/LandsideCurbSide.h"
#include "LandsideResourceManager.h"
#include "Common/ARCRay.h"
#include "Common/Pollygon2008.h"
#include "LandsideVehicleInSim.h"
#include ".\laneseginsim.h"
#include <limits>
#include "PaxLandsideBehavior.h"
#include "Person.h"
#include "ARCportEngine.h"
#include "LandsideSimulation.h"
#include "LandsideParkingLotInSim.h"
#include "Landside\CurbsideBehavior.h"


#define RADIUS_CONVERN 500

void LandsideCurbSideInSim::InitRelateWithOtherObject( LandsideResourceManager* allRes )
{
	LandsideCurbSide* pCurbInput= getCurbInput();
	if(!pCurbInput->getStrech())
		return;
	LandsideStretchInSim*pStretch = allRes->getStretchByID(pCurbInput->getStrech()->getID());
	if(!pStretch)
		return;

	mParkingSpots.Init(pCurbInput, allRes, this);
	//init 


	//add next to lane
	for(int i=0;i<pStretch->GetLaneCount();i++)
	{
		LandsideStretchLaneInSim* pLane = pStretch->GetLane(i);		
		DistanceUnit distF = pLane->GetPath().GetIndexDist(mParkingSpots.GetEntryIndexInStrech() );
		DistanceUnit distT = pLane->GetPath().GetIndexDist(mParkingSpots.GetExitIndexInStretch() );

		if(!pLane->HasParkingSpot())
		{
			{
				LandsideCurbsideSideExit* pCurbExit = new LandsideCurbsideSideExit;
				pCurbExit->SetFromRes(this);
				pCurbExit->SetDistRangeInlane(distF,distT);
				pCurbExit->SetPosition(pLane,distF-RADIUS_CONVERN);

				m_vLaneEntries.push_back(pCurbExit);
				pLane->AddLaneNode(pCurbExit);
			}
			{
				LandsideCursideSideEntry* pCurbEntry = new LandsideCursideSideEntry;
				pCurbEntry->SetToRes(this);
				pCurbEntry->SetDistRangeInlane(distF,distT);
				pCurbEntry->SetPosition(pLane,distF);

				m_vLaneExits.push_back(pCurbEntry);
				pLane->AddLaneNode(pCurbEntry);
			}
		}
		else
		{
			LandsideLaneExit* pLaneExit = new LandsideLaneExit();
			pLaneExit->SetPosition(pLane,distF);
			pLaneExit->SetToRes(this);
			m_vLaneExits.push_back(pLaneExit);
			pLane->AddLaneNode(pLaneExit);

			LandsideLaneEntry* pLaneEntry = new LandsideLaneEntry();
			pLaneEntry->SetPosition(pLane,distT);
			pLaneEntry->SetFromRes(this);				
			pLane->AddLaneNode(pLaneEntry);
			m_vLaneEntries.push_back(pLaneEntry);
		}		
	}
}






CString LandsideCurbSideInSim::print() const
{
	return getInput()->getName().GetIDString();
}

CPoint2008 LandsideCurbSideInSim::GetPaxWaitPos() const
{
	CPollygon2008 polygon;
	polygon.init(getCurbInput()->m_PickArea.getCount(),getCurbInput()->m_PickArea.getPointList());
	CPoint2008 ret=  polygon.getRandPoint();
	ret.z = m_dHeight;
	return ret;
}

bool LandsideCurbSideInSim::IsLinkToRoad() const
{
	return !m_vLaneEntries.empty() && !m_vLaneExits.empty();
}

//////////////////////////////////////////////////////////////////////////
LandsideVehicleInSim* LandsideCurbSideInSim::GetAheadVehicle( LandsideVehicleInSim* mpVehicle, LandsideLaneInSim* pLane,
															 double& distInlane ) const
{
	LandsideVehicleInSim* pAheadVehicle = NULL;
	for(int i=0;i<GetInResVehicleCount();i++)
	{		
		if(mpVehicle==GetInResVehicle(i))
			break;		
		pAheadVehicle = GetInResVehicle(i);		
	}	
	if(pAheadVehicle && pAheadVehicle->getState().getLandsideRes()==pLane)
	{
		distInlane = pAheadVehicle->getState().distInRes;
	}
	return NULL;
}

LandsideLaneNodeInSim* LandsideCurbSideInSim::GetExitNode( LandsideLaneNodeInSim* pFromNode )const
{
	ASSERT(pFromNode->toExit());
	return GetExitNode(pFromNode->mpLane);	
}

LandsideLaneNodeInSim* LandsideCurbSideInSim::GetExitNode( LandsideLaneInSim* pLane )const
{
	for(size_t i=0;i<m_vLaneEntries.size();i++)
	{
		LandsideLaneEntry* pEntry = m_vLaneEntries[i];
		if(pEntry->mpLane == pLane)
			return pEntry;
	}
	return NULL;
}
CLandsideWalkTrafficObjectInSim* LandsideCurbSideInSim::getBestTrafficObjectInSim( const CPoint2008& pt,LandsideResourceManager* allRes )
{
	InitTrafficObjectOverlap(allRes);
	CLandsideWalkTrafficObjectInSim* pBestTrafficObjectInSim = NULL;

	double dShortestDist = (std::numeric_limits<double>::max)();
	CPollygon2008 pollygon;
	pollygon.init(getCurbInput()->m_PickArea.getCount(),getCurbInput()->m_PickArea.getPointList());
	for (int i = 0; i < allRes->m_vTrafficObjectList.getCount(); i++)
	{
		CLandsideWalkTrafficObjectInSim* pTrafficObjectInSim = allRes->m_vTrafficObjectList.getItem(i);
		double dDeta = fabsl(pt.getZ() - pTrafficObjectInSim->GetLevel());
		if (dDeta > (std::numeric_limits<double>::min)())
			continue;

		if (pTrafficObjectInSim->InterSectWithPollygon(pollygon))
		{
			double dDist = pTrafficObjectInSim->GetParkinglotPointDist((LandsideResourceInSim*)this,pt);
			if (dDist < dShortestDist)
			{
				pBestTrafficObjectInSim = pTrafficObjectInSim;
				dShortestDist = dDist;
			}
		}
		
	}

	return pBestTrafficObjectInSim;
}
void LandsideCurbSideInSim::InitTrafficObjectOverlap( LandsideResourceManager* allRes )
{
	if (m_bInit)
		return;
	
	m_bInit = true;
	CPollygon2008 pollygon;
	pollygon.init(getCurbInput()->m_PickArea.getCount(),getCurbInput()->m_PickArea.getPointList());
	for (int i = 0; i < allRes->m_vTrafficObjectList.getCount(); i++)
	{
		CLandsideWalkTrafficObjectInSim* pTrafficObjectInSim = allRes->m_vTrafficObjectList.getItem(i);
		if (pTrafficObjectInSim->InterSectWithPollygon(pollygon))
		{
			ParkingLotIntersectTrafficObject trafficObjectInfo;
			trafficObjectInfo.m_nIdxpolygon = i;
			trafficObjectInfo.m_pLandsideResInSim = this;
			std::vector<CPath2008> pathList;
			GetInsidePathOfPolygon(pTrafficObjectInSim->GetCenterPath(),pollygon,pathList);
			trafficObjectInfo.m_OverlapPathList = pathList;
			pTrafficObjectInSim->AddTafficParkinglot(trafficObjectInfo);
		}

	}
}

LandsideCurbSideInSim::LandsideCurbSideInSim( LandsideCurbSide* pCurb ,bool bLeftDrive)
:LandsideLayoutObjectInSim(pCurb)
{
	//mpInput = pCurb;
	m_bInit = false;
	CPoint2008 ptCenter;
	pCurb->get3DCenter(ptCenter);
	m_dHeight = ptCenter.z;
	getParkingSpot().getInStretchSpots().SetLeftDrive(bLeftDrive);
}

LandsideLayoutObjectInSim* LandsideCurbSideInSim::getLayoutObject() const
{
	return const_cast<LandsideCurbSideInSim*>(this);
}

LandsideCurbSide* LandsideCurbSideInSim::getCurbInput() const
{
	return (LandsideCurbSide*)getInput();
}


//LandsideLaneInSim* LandsideCurbSideInSim::GetNotAtlane( LandsideLaneInSim* pLane ) const
//{
//	if(!GetLaneSeg(pLane))
//		return pLane;
//	LandsideLaneInSim* pleft = pLane->getLeft();
//	while(pleft)
//	{
//		if(!GetLaneSeg(pleft))
//			return pleft;
//		pleft = pleft->getLeft();
//	}
//	LandsideLaneInSim* pright = pLane->getRight();
//	while(pright)
//	{
//		if(!GetLaneSeg(pright))
//			return pright;
//		pright = pright->getRight();
//	}
//	return NULL;
//
//}

void LandsideCurbSideInSim::PassengerMoveInto( PaxLandsideBehavior *pPaxBehvior, ElapsedTime eEventTime )
{

	//passenger moves to the curbside
	ARCVector3 pos = GetPaxWaitPos(); // the position at curbside
	pPaxBehvior->setResource(this);
	//move from terminal to landside
	pPaxBehvior->setDestination(pos);			
	ElapsedTime dt = ElapsedTime(0L);	
	dt = pPaxBehvior->moveTime();		
	ElapsedTime eNextTime = eEventTime + dt;
	pPaxBehvior->WriteLogEntry(eNextTime);
	pPaxBehvior->getPerson()->setState(WaitingForVehicle);
	AddWaitingPax(pPaxBehvior->GetPersonID());

	//
	
	LandsideVehicleInSim* pVehicle = pPaxBehvior->getVehicle();
	if(!pVehicle)
	{
		pVehicle = pPaxBehvior->getPerson()->getEngine()->GetLandsideSimulation()->FindPaxVehicle(pPaxBehvior->getPerson()->getID());
		pPaxBehvior->setVehicle(pVehicle);
	}

	if(pVehicle == NULL)
		return;//the vehicle has not been generated, the passenger stays at the curbside

	//this is the resource the vehicle is on currently, it could be any landside object
	//it is curbside or parking lot(cellphone feature, if the passenger does not arrive at curbside 
	//
	LandsideResourceInSim* pParkingResource = pVehicle->getLastState().getLandsideRes();
	if (pParkingResource == NULL)
		return;

	//cellphone feature, the vehicle is parking on the parking lot and waiting till the passenger calls.
	//notify vehicle in cellphone lot
	if(pParkingResource->toParkLotSpot())
	{
		LandsideParkingSpotInSim* pParkingSpotInSim = pParkingResource->toParkLotSpot();
		LandsideParkingLotInSim* pParkingLotInSim = pParkingSpotInSim->getParkingLot();
		if(pParkingLotInSim && pParkingLotInSim->IsCellPhone())
		{
			pVehicle->Activate(eNextTime);
			return;// the passenger stays at curbside, and awaits for the vehicle to pick him up.
		}
	}
	else if(LandsideParkingLotInSim* pParkingLotInSim = pParkingResource->getLayoutObject()->toParkLot())
	{
		if (pParkingLotInSim->IsCellPhone())
		{
			pVehicle->Activate(eNextTime);
			return;// the passenger stays at curbside, and awaits for the vehicle to pick him up.
		}
	}

	//check if the vehicle is ready to loading pax
	if(!pVehicle->IsLoadingPax())//if the vehicle is not ready to serve the pax, the pax will stay at curbside
		return;

	//make sure the vehicle is parking on the curbside
	IParkingSpotInSim *pCurbsideParkingSpot = pParkingResource->toLaneSpot();
	ASSERT(pCurbsideParkingSpot != NULL);
	if(pCurbsideParkingSpot == NULL)
	{
		//the vehicle is not on curbside
		//so the passenger will stay and wait
		return;
	}
	LandsideResourceInSim *pParentResCurbsideParkingSpot = pCurbsideParkingSpot->GetParentResource();
	ASSERT(pParentResCurbsideParkingSpot != NULL);
	if(pParentResCurbsideParkingSpot == NULL || pParentResCurbsideParkingSpot->toCurbSide() == NULL)
	{
		//the vehicle is not on curbside
		//stay and wait for his vehicle
		return;
	}
	LandsideCurbSideInSim* pCurbsideVehicleParking = pParentResCurbsideParkingSpot->toCurbSide();

	//the vehicle is parking in the same curbside
	if( pCurbsideVehicleParking == this )
	{
		pPaxBehvior->setState(MoveToVehicle);
		pPaxBehvior->setVehicle(pVehicle);
		pPaxBehvior->GenerateEvent(eNextTime);
		return;
	}
	else// the vehicle's parking place is not the curbside that 
	{
		pPaxBehvior->setDestResource(pCurbsideParkingSpot);
		pPaxBehvior->setVehicle(pVehicle);
		pPaxBehvior->setState(MoveToFacility);
		pPaxBehvior->GenerateEvent(eEventTime);
	}
}

void LandsideCurbSideInSim::PassengerMoveOut( PaxLandsideBehavior *pPaxBehvior, ElapsedTime eTime )
{
	DelWaitingPax(pPaxBehvior->GetPersonID());
}

bool LandsideCurbSideInSim::IsHaveCapacityAndFull( LandsideVehicleInSim*pVeh ) const
{
	return LandsideLayoutObjectInSim::IsHaveCapacityAndFull(pVeh);
}

ElapsedTime LandsideCurbSideInSim::GetMaxStopTime() 
{
	CCurbsideBehavior* pBehavior = (CCurbsideBehavior*)GetBehavior();
	if(pBehavior)
	{
		double dt= pBehavior->GetProDist().GetProbDistribution()->getRandomValue();
		return ElapsedTime(dt);
	}
	return ElapsedTime(0L);
}

CPath2008 LandsideCurbSideInSim::GetQueue()
{
	ASSERT(FALSE);

	return getCurbInput()->m_PickArea;
}

