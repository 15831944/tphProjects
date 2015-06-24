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
	if(pStretch)
	{
		CPoint2008 ptFrom = pStretch->m_Path.GetDistPoint(pCurbInput->getDistFrom());
		CPoint2008 ptTo = pStretch->m_Path.GetDistPoint(pCurbInput->getDistTo());

		int iLaneForm = MIN(pCurbInput->m_nLaneTo-1,pCurbInput->m_nLaneFrom-1);
		int iLaneTo = MAX(pCurbInput->m_nLaneTo-1,pCurbInput->m_nLaneFrom-1);
		iLaneForm = MAX(0,iLaneForm); 
		iLaneForm = MIN(iLaneForm,pStretch->GetLaneCount()-1);
		iLaneTo = MAX(0,iLaneTo); 
		iLaneTo = MIN(iLaneTo,pStretch->GetLaneCount()-1);


		for(int i=iLaneForm;i<=iLaneTo;i++)
		{
			LandsideLaneInSim* pLane = pStretch->GetLane(i);
			if(pLane)
			{
				LandsideLaneExit* pLaneExit = new LandsideLaneExit();
				DistanceUnit distF = pLane->GetPointDist(ptFrom);
				DistanceUnit distT = pLane->GetPointDist(ptTo);

				pLaneExit->SetPosition(pLane,MAX(0,distF-RADIUS_CONVERN) );
				pLaneExit->SetToRes(this);			
				m_vLaneExits.push_back(pLaneExit);
				pLane->AddLaneNode(pLaneExit);

				LandsideLaneEntry* pLaneEntry = new LandsideLaneEntry();
				pLaneEntry->SetPosition(pLane,distT);
				pLaneEntry->SetFromRes(this);				
				pLane->AddLaneNode(pLaneEntry);
				m_vLaneEntries.push_back(pLaneEntry);
			
				m_vLaneOccupy.push_back(new LaneSegInSim(pLane, distF,distT));
				mParkingSpots.addLane(pLane,distF,distT,pCurbInput->m_dSpotLength,this);
			}
		}
		mParkingSpots.InitSpotRelations();

		//add next to lane
		for(int i=0;i<iLaneForm;i++)
		{
			LandsideLaneInSim* pLane = pStretch->GetLane(i);
			DistanceUnit distF = pLane->GetPointDist(ptFrom);
			DistanceUnit distT = pLane->GetPointDist(ptTo);

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
		for(int i=iLaneTo+1;i<pStretch->GetLaneCount();i++)
		{
			LandsideLaneInSim* pLane = pStretch->GetLane(i);
			DistanceUnit distF = pLane->GetPointDist(ptFrom);
			DistanceUnit distT = pLane->GetPointDist(ptTo);

			{
				LandsideCurbsideSideExit* pCurbExit = new LandsideCurbsideSideExit;
				pCurbExit->SetFromRes(this);
				pCurbExit->SetDistRangeInlane(distF,distT);
				pCurbExit->SetPosition(pLane,distF);
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
	getParkingSpot().SetLeftDrive(bLeftDrive);
}

LandsideLayoutObjectInSim* LandsideCurbSideInSim::getLayoutObject() const
{
	return const_cast<LandsideCurbSideInSim*>(this);
}

LandsideCurbSide* LandsideCurbSideInSim::getCurbInput() const
{
	return (LandsideCurbSide*)getInput();
}


LandsideLaneInSim* LandsideCurbSideInSim::GetNotAtlane( LandsideLaneInSim* pLane ) const
{
	if(!GetLaneSeg(pLane))
		return pLane;
	LandsideLaneInSim* pleft = pLane->getLeft();
	while(pleft)
	{
		if(!GetLaneSeg(pleft))
			return pleft;
		pleft = pleft->getLeft();
	}
	LandsideLaneInSim* pright = pLane->getRight();
	while(pright)
	{
		if(!GetLaneSeg(pright))
			return pright;
		pright = pright->getRight();
	}
	return NULL;

}
//
LaneSegInSim* LandsideCurbSideInSim::GetLaneSeg( LandsideLaneInSim* plane ) const
{
	for(size_t i=0;i<m_vLaneOccupy.size();++i)
	{
		LaneSegInSim* pSeg = m_vLaneOccupy[i];
		if(pSeg->mpLane == plane)
			return pSeg;
	}
	return NULL;
}


//LandsideLaneNodeList
bool LandsideCurbSideInSim::FindParkingPos( LandsideVehicleInSim* pVehicle, LandsideLaneNodeList& followPath )
{
	LandsideResourceInSim* pAtRes = (LandsideResourceInSim*)pVehicle->getLastState().getLandsideRes();
	if(!pAtRes) 
		return false;
	LandsideLaneInSim* pAtLane = pAtRes->toLane();
	if(!pAtLane) 
		return false;

	const CPoint2008& dFromPos =  pVehicle->getLastState().pos;
	//find at lane first
	for(int i=0;i<(int)m_vLaneOccupy.size();i++)
	{
		LaneSegInSim* pLaneSeg = m_vLaneOccupy[i];
		if(pLaneSeg->mpLane==pAtLane)
		{
			int iLoop = 0;
			double dRangeF;
			double dRangT;
			while(pLaneSeg->FindFreePos(pVehicle,iLoop,dRangeF,dRangT))
			{
				if(pLaneSeg->FindPathToRange(pVehicle,pAtLane,dFromPos,dRangeF,dRangT, followPath))
				{
					pLaneSeg->addVehicleOcy(pVehicle, dRangeF - pVehicle->GetLength()*0.5 );
					return true;
				}
			}
			break;
		}
	}

	//
	for(int i=0;i<(int)m_vLaneOccupy.size();i++)
	{
		LaneSegInSim* pLaneSeg = m_vLaneOccupy[i];
		if(pLaneSeg->mpLane!=pAtLane)
		{
			int iLoop = 0;
			double dRangeF;
			double dRangT;
			while(pLaneSeg->FindFreePos(pVehicle,iLoop,dRangeF,dRangT))
			{
				if(pLaneSeg->FindPathToRange(pVehicle,pAtLane,dFromPos,dRangeF,dRangT, followPath))
				{
					pLaneSeg->addVehicleOcy(pVehicle, dRangeF - pVehicle->GetLength()*0.5 );
					return true;
				}
			}
		}
	}
	return false;
}

bool LandsideCurbSideInSim::FindLeavePath( LandsideVehicleInSim* pVehicle, LandsideLaneNodeList& followPath )
{

	LandsideResourceInSim* pAtRes = (LandsideResourceInSim*)pVehicle->getLastState().getLandsideRes();
	if(!pAtRes) return false;
	LandsideLaneInSim* pAtLane = pAtRes->toLane();
	if(!pAtLane) return false;
	CPoint2008 dAtPos = pVehicle->getLastState().pos;

	LandsideLaneNodeInSim* pExitCurbside = GetExitNode(pAtLane);	
	//
	LandsideLaneEntry* pLaneEntry = new LandsideLaneEntry();
	pLaneEntry->SetPosition(pAtLane,dAtPos);
	pLaneEntry->SetFromRes(pAtLane);
	followPath.push_back(pLaneEntry);

	LandsideLaneExit* pLaneExit = new LandsideLaneExit();
	pLaneExit->SetPosition(pExitCurbside->mpLane,pExitCurbside->m_distInlane);
	pLaneExit->SetToRes(pExitCurbside->getToRes());
	followPath.push_back(pLaneExit);	

	return true;
}

void LandsideCurbSideInSim::RemoveVehicleParkPos( LandsideVehicleInSim* pVehicle )
{
	for(size_t i=0;i<m_vLaneOccupy.size();i++)
	{
		LaneSegInSim* pSeg = m_vLaneOccupy[i];
		pSeg->removeVehicleOcy(pVehicle);
	}
}

void LandsideCurbSideInSim::PassengerMoveInto( PaxLandsideBehavior *pPaxBehvior, ElapsedTime eEventTime )
{

	ARCVector3 pos = GetPaxWaitPos();
	pPaxBehvior->setResource(this);
	//move from terminal to landside
	pPaxBehvior->setDestination(pos);			
	ElapsedTime dt = ElapsedTime(0L);	
	dt = pPaxBehvior->moveTime();		
	ElapsedTime eNextTime = eEventTime + dt;
	pPaxBehvior->WriteLogEntry(eNextTime);
	pPaxBehvior->getPerson()->setState(WaitingForVehicle);
	AddWaitingPax(pPaxBehvior->GetPersonID());

	LandsideVehicleInSim* pVehicle = pPaxBehvior->getVehicle();
	if(!pVehicle)
	{
		pVehicle = pPaxBehvior->getPerson()->getEngine()->GetLandsideSimulation()->FindPaxVehicle(pPaxBehvior->getPerson()->getID());
		pPaxBehvior->setVehicle(pVehicle);
	}

	if(pVehicle == NULL)
		return;
	LandsideResourceInSim* pParkingResource = pVehicle->getLastState().getLandsideRes();
	if (pParkingResource == NULL)
		return;

	//notify vehicle in cellphone lot
	if(LandsideParkingLotInSim* pParkingLotInSim = pParkingResource->getLayoutObject()->toParkLot())
	{
		if (pParkingLotInSim->IsCellPhone())
		{
			pVehicle->Activate(eNextTime);
			return;
		}
	}

	//check if the vehicle is ready to loading pax
	if(!pVehicle->IsLoadingPax())
		return;


	//the vehicle is parking in the same curbside
	if( pParkingResource == this )
	{
		pPaxBehvior->setState(MoveToVehicle);
		pPaxBehvior->setVehicle(pVehicle);
		pPaxBehvior->GenerateEvent(eNextTime);
		return;
	}
	else
	{
		pPaxBehvior->setDestResource(pParkingResource);
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

