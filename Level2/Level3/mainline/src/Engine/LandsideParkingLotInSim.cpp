#include "StdAfx.h"
#include ".\landsideparkinglotinsim.h"
#include "LandsideResourceManager.h"
#include <limits>
#include "LandsideParkingLotGraph.h"
#include "common/DistanceLineLine.h"
#include "LandsideVehicleInSim.h"
#include "Landside/LandsideParkingSpotConstraints.h"
#include "common/ARCPipe.h"
#include "WalkwayInSim.h"
#include "Landside/LandsideWalkway.h"
#include "PaxLandsideBehavior.h"
#include "Common\States.h"

LandsideParkingLotInSim::~LandsideParkingLotInSim(void)
{
	Clear();
}

void LandsideParkingLotInSim::InitRelateWithOtherObject( LandsideResourceManager* allRes )
{
	for(size_t i=0;i<m_vLevels.size();i++)
	{
		LandsideParkingLotLevelInSim* pLevel = m_vLevels.at(i);
		pLevel->InitRelation(allRes);
	}

	InitGraph();
}

LandsideParkingLotInSim::LandsideParkingLotInSim( LandsideParkingLot* pInput,bool bLeftDrive )
:LandsideLayoutObjectInSim(pInput)
{
	//mpLot = pInput;
	//init spots
	
	//LandsideParkingLotLevelInSim* pLevel = new LandsideParkingLotLevelInSim(this,LandsideParkingLotLevelInSim::_base,0);
	//m_vLevels.push_back(pLevel);
	for(int i=0;i<getLotInput()->GetLevelCount();i++)
	{
		LandsideParkingLotLevelInSim* pLevel = new LandsideParkingLotLevelInSim(this,i);
		m_vLevels.push_back(pLevel);
	}

	for(size_t i=0;i<m_vLevels.size();i++)
	{
		LandsideParkingLotLevelInSim* pLevel = m_vLevels[i];
		pLevel->Init(bLeftDrive);
	}

	m_pDriveGraph = new LandsideParkingLotGraph();	
	m_bInit = false;
	m_pLandsideParkingSpotCon = NULL;

}





//
CString LandsideParkingLotInSim::print() const
{
	return getInput()->getName().GetIDString();
}

//------------------------------------------------------------------------------
//Summary:
//		intersect with area and select the shortest walkway
//Parameters:
//		pt[in]: current point to calculate the distance to select best walkway
//		LandsideResourceManager[in]: get walkway list to select best walkway
//Return:
//		retrieve the best walkway
//------------------------------------------------------------------------------
CLandsideWalkTrafficObjectInSim* LandsideParkingLotInSim::getBestTrafficObjectInSim( const CPoint2008& pt,LandsideResourceManager* allRes )
{
	InitTrafficObjectOverlap(allRes);

	CLandsideWalkTrafficObjectInSim* pBestWalkwayInSim = NULL;
	int nCount = (int)getLotInput()->GetLevelCount();//.size();
	for (int i = 0; i < nCount; i++)
	{
		LandsideParkingLotLevel* pLevel = getLotInput()->GetLevel(i);
		double dHeight = getLotInput()->GetLevelHeight(i);
		double dDeta = fabsl(dHeight - pt.getZ());
		if (dDeta > (std::numeric_limits<double>::min)())
			continue;
	
		const ParkingLotAreas& areaList = pLevel->m_areas;
		double dShortestDist = (std::numeric_limits<double>::max)();
		for (int i = 0; i < (int)areaList.m_areas.size(); i++)
		{
			CPollygon2008 pollygon = areaList.m_areas[i].mpoly;
			pollygon.SetBoundsPoint();
			if (pollygon.contains2(pt))
			{
				for (size_t j = 0; j < allRes->m_vTrafficObjectList.size(); j++)
				{
					CLandsideWalkTrafficObjectInSim* pTrafficInSim = allRes->m_vTrafficObjectList[j];

					if (pTrafficInSim->GetTrafficType() != CLandsideWalkTrafficObjectInSim::Walkway_Type)
						continue;

					double dLevel = pTrafficInSim->GetLevel();
					double dZdist = fabsl(dLevel - dHeight);
					if (dZdist > (std::numeric_limits<double>::min)())
						continue;

					if (pTrafficInSim->InterSectWithPollygon(pollygon))
					{	
						double dDist = pTrafficInSim->GetParkinglotPointDist((LandsideResourceInSim*)this,pt);
						if (dDist < dShortestDist)
						{
							pBestWalkwayInSim = pTrafficInSim;
							dShortestDist = dDist;
						}
					}	
				}
			}
		}
	}
	
	return pBestWalkwayInSim;
}

//--------------------------------------------------------------------------
//Summary:
//		parking lot intersect with traffic object
//--------------------------------------------------------------------------
void LandsideParkingLotInSim::InitTrafficObjectOverlap( LandsideResourceManager* allRes )
{
	if (m_bInit)
		return;
	
	m_bInit = true;
	const ParkingLotAreas& areaList = getLotInput()->GetLevel(0)->m_areas;

	for (int i = 0; i < (int)areaList.m_areas.size(); i++)
	{
		CPollygon2008 pollygon = areaList.m_areas[i].mpoly;
		pollygon.SetBoundsPoint();
		for (size_t j = 0; j < allRes->m_vTrafficObjectList.size(); j++)
		{
			CLandsideWalkTrafficObjectInSim* pTrafficInSim = allRes->m_vTrafficObjectList[j];

			if (pTrafficInSim->GetTrafficType() != CLandsideWalkTrafficObjectInSim::Walkway_Type)
				continue;

			if (pTrafficInSim->InterSectWithPollygon(pollygon))
			{	
				ParkingLotIntersectTrafficObject trafficObjectInfo;
				trafficObjectInfo.m_nIdxpolygon = i;
				trafficObjectInfo.m_pLandsideResInSim = this;
				std::vector<CPath2008> pathList;
				GetInsidePathOfPolygon(pTrafficInSim->GetCenterPath(),pollygon,pathList);
				trafficObjectInfo.m_OverlapPathList = pathList;
				pTrafficInSim->AddTafficParkinglot(trafficObjectInfo);
			}	
		}
	}
}
void LandsideParkingLotInSim::Clear()
{
	for(size_t i=0;i<m_vLevels.size();i++)
		delete m_vLevels.at(i);
	m_vLevels.clear();

	delete m_pDriveGraph;
	m_pDriveGraph = NULL;
}

void LandsideParkingLotInSim::InitGraph()
{
	for(size_t i=0;i<m_vLevels.size();i++)
	{
		LandsideParkingLotLevelInSim* pLevel = m_vLevels.at(i);
		pLevel->InitGraph(m_pDriveGraph);
	}
	
	m_pDriveGraph->BuildGraph();
}



LandsideLayoutObjectInSim* LandsideParkingLotInSim::getLayoutObject() const
{
	return const_cast<LandsideParkingLotInSim*>(this);
}

bool LandsideParkingLotInSim::IsCellPhone()const
{
	LandsideLayoutObjectInSim* pObject = getLayoutObject();
	if (pObject == NULL)
		return false;
	
	LandsideParkingLot* pParkingLot = (LandsideParkingLot*)pObject->getInput();
	return pParkingLot->IsCellPhone()?true:false;
}

bool LandsideParkingLotInSim::IsSpotLess() const
{
	for (size_t i=0;i<m_vLevels.size();i++)
	{
		LandsideParkingLotLevelInSim* pLevel = m_vLevels.at(i);
		if( !pLevel->IsSpotLess() )
			return false;
	}
	return true;	
}

LandsideParkingSpotInSim* LandsideParkingLotInSim::getFreeSpot( LandsideVehicleInSim* pVehicle )
{
	//level
	CString strVehicleType;
	if (!pVehicle->getName().isAll())
	{
		strVehicleType = pVehicle->getName().toString();
	}
	 
	ElapsedTime eTime = pVehicle->getLastState().time;
	CString strParkingLot = getLotInput()->getName().GetIDString();
	std::vector<LandsideParkingSpotInSim*> availableParkingSpotVector;
	for (size_t i=0;i<m_vLevels.size();i++)
	{
		LandsideParkingLotLevelInSim* pLevel = m_vLevels.at(i);
		//Row
		for (int j = 0; j < pLevel->GetRowCount(); j++)
		{
			LandsideParkingSpaceInSim* pRow = pLevel->GetRowParkingSpace(j);
			//spot
			for (int n = 0; n < pRow->getSpotCount(); n++)
			{
				LandsideParkingSpotInSim* pSpot = pRow->getSpot(n);
				if (pSpot->getOccupyVehicle())
					continue;

				int nLevel = pLevel->GetLevelID();
				int nRow = j + 1;
				int Idx = n + 1;
				if (m_pLandsideParkingSpotCon->isValid(nLevel,nRow,Idx,strParkingLot,strVehicleType,eTime))
				{
					availableParkingSpotVector.push_back(pSpot);
				}
			}
		}
	}

	if (availableParkingSpotVector.empty())
	{
		return NULL;
	}

	int nCount = (int)availableParkingSpotVector.size();
	int nRand = random(nCount);
	LandsideParkingSpotInSim* pSpot = availableParkingSpotVector[nRand];
	pSpot->setOccupyVehicle(pVehicle);

	return pSpot;
}

LandsideParkingLotDoorInSim* LandsideParkingLotInSim::getExitDoor( LandsideVehicleInSim* pVehicle )
{
	for (size_t i=0;i<m_vLevels.size();i++)
	{
		LandsideParkingLotLevelInSim* pLevel = m_vLevels.at(i);
		if(LandsideParkingLotDoorInSim* pSpot  = pLevel->getExitDoor(pVehicle) )
			return pSpot;
	}
	return NULL;	
}

//LandsideParkingLotDoorInSim* LandsideParkingLotInSim::getEntryDoor( LandsideVehicleInSim* pVehicle )
//{
//	for (size_t i=0;i<m_vLevels.size();i++)
//	{
//		LandsideParkingLotLevelInSim* pLevel = m_vLevels.at(i);
//		if(LandsideParkingLotDoorInSim* pSpot  = pLevel->getExitDoor(pVehicle) )
//			return pSpot;
//	}
//	return NULL;	
//}

DistanceUnit LandsideParkingLotInSim::getLevelIndexHeight( double indexZ ) const
{
	int nLevel = (int)(indexZ+1);
	double noffset = indexZ+1- nLevel;
	
	DistanceUnit dH1 = getLevelHeight(nLevel);
	DistanceUnit dH2 = getLevelHeight(nLevel+1);
	return dH1*(1-noffset)+ dH2*(noffset);

}

DistanceUnit LandsideParkingLotInSim::getLevelHeight( int idx ) const
{
	idx = MIN(idx,getLevelCount()-1);
	idx = MAX(idx,0);
	return getLevel(idx)->getHeight();
}


LandsideParkingLotDoorInSim* LandsideParkingLotInSim::getNearestEntryDoor( LandsideResourceManager* pRes ,LandsideVehicleInSim* pVehicle )
{
	LandsideLaneNodeList mRoutePath;
	LandsideResourceInSim* pOrign = pVehicle->getLastState().getLandsideRes();
	for (size_t i=0;i<m_vLevels.size();i++)
	{
		LandsideParkingLotLevelInSim* pLevel = m_vLevels.at(i);
		for(int j=0;j<pLevel->getDoorCount();++j)
		{
			LandsideParkingLotDoorInSim* pDoor = pLevel->getDoor(j);
			bool btrue =  pRes->mRouteGraph.FindRouteResToRes(pOrign,pDoor,mRoutePath);
			if(btrue)
				return pDoor;
		}			
	}
	return NULL;
	
}

void LandsideParkingLotInSim::SetParkingLotConstraint( LandsideParkingSpotConstraints* pParkingLotCon )
{
	m_pLandsideParkingSpotCon = pParkingLotCon;
}

LandsideParkingLotDoorInSim* LandsideParkingLotInSim::getDoorToSpot( LandsideVehicleInSim* pVehicle, LandsideParkingSpotInSim*pSpot )
{
	for (size_t i=0;i<m_vLevels.size();i++)
	{
		LandsideParkingLotLevelInSim* pLevel = m_vLevels.at(i);
		for(int j=0;j<pLevel->getDoorCount();++j)
		{
			LandsideParkingLotDoorInSim* pDoor = pLevel->getDoor(j);
			DrivePathInParkingLot path;
			bool btrue =  m_pDriveGraph->FindPath(pDoor,pSpot,path);
			if(btrue)
				return pDoor;
		}			
	}
	return NULL;
}

void LandsideParkingLotInSim::ClearEmbedBusStation()
{
	m_vEmbedBusStationInSim.clear();
}

int LandsideParkingLotInSim::GetEmbedBusStationCount() const
{
	return static_cast<int>(m_vEmbedBusStationInSim.size());
}

LandsideBusStationInSim * LandsideParkingLotInSim::GetEmbedBusStation( int nIndex )
{
	if (nIndex < 0 || nIndex >= GetEmbedBusStationCount())
		return NULL;

	return m_vEmbedBusStationInSim[nIndex];
}

void LandsideParkingLotInSim::AddEmbedBusStation( LandsideBusStationInSim * pStation )
{
	m_vEmbedBusStationInSim.push_back(pStation);
}

//////////////////////////////////////////////////////////////////////////
void LandsideParkingSpotInSim::OnVehicleEnter( LandsideVehicleInSim* pVehicle, DistanceUnit dist,const ElapsedTime& t )
{
	__super::OnVehicleEnter(pVehicle,dist,t);
	getParkingLot()->OnVehicleEnter(pVehicle,dist,t);
}

LandsideParkingLotInSim* LandsideParkingSpotInSim::getParkingLot()const
{
	return mpSpace->getParkingLot();
}

LandsideParkingSpotInSim::LandsideParkingSpotInSim( LandsideParkingSpaceInSim* pSpaceInSim,int idx, int nParkLotLevel )
{	
	mpSpace = pSpaceInSim;
	m_idx = idx;
	m_nParkinglotLevel = nParkLotLevel;
	
}

void LandsideParkingSpotInSim::setPosDir( const ARCPoint3& pos ,const ARCVector3& dir )
{
	m_pos = pos;
	m_dir = dir;
}

LandsideVehicleInSim* LandsideParkingSpotInSim::getOccupyVehicle() const
{
	ASSERT(GetInResVehicleCount()<2);	
	if(GetInResVehicleCount())
	{
		return GetInResVehicle(0);
	}
	return NULL;
}

void LandsideParkingSpotInSim::OnVehicleExit( LandsideVehicleInSim* pVehicle,const ElapsedTime& t )
{
	__super::OnVehicleExit(pVehicle,t);
	getParkingLot()->OnVehicleExit(pVehicle,t);
	
}

CString LandsideParkingSpotInSim::print() const
{
	LandsideParkingSpotInSim* pSpot = (LandsideParkingSpotInSim*)this;
	CString strRet;
	strRet.Format("%s Spot(Level%d,Space%d,Index%d)"
		,pSpot->getParkingLot()->print().GetString()
		,getParkingLotLevelIndex()+1,mpSpace->GetIndex()+1,m_idx+1); 
	return strRet;	
}



bool LandsideParkingSpotInSim::Is_parallel() const
{
	return mpSpace->mspace.m_type == ParkingSpace::_parallel;
}

LandsideLayoutObjectInSim* LandsideParkingSpotInSim::getLayoutObject()const
{
	return getParkingLot();
}

int LandsideParkingSpotInSim::getParkingLotLevelIndex() const
{
	return m_nParkinglotLevel;
}

void LandsideParkingSpotInSim::setParkingLotLevelIndex( int nLevel )
{
	m_nParkinglotLevel = nLevel;
}

void LandsideParkingSpotInSim::PassengerMoveInto( PaxLandsideBehavior *pPaxBehvior, ElapsedTime eTime )
{
	LandsideVehicleInSim* pVehicle = pPaxBehvior->getVehicle();
	if(pVehicle == NULL)
	{
		pPaxBehvior->setState(Death);
		pPaxBehvior->GenerateEvent(eTime);
		return;

	}
	LandsideResourceInSim *pVehicleCurResource = pVehicle->getLastState().getLandsideRes();
	ASSERT(pVehicleCurResource == this);
	pPaxBehvior->setState(MoveToVehicle);
	pPaxBehvior->GenerateEvent(eTime);
}

void LandsideParkingSpotInSim::PassengerMoveOut( PaxLandsideBehavior *pPaxBehvior, ElapsedTime eTime )
{
	//nothing to do
	//ASSERT(0);
}

CPoint2008 LandsideParkingSpotInSim::GetPaxWaitPos() const
{
	return getEntryPos2();
}

int LandsideParkingSpotInSim::GetParkingLotLevelID()
{
	return mpSpace->getParkingLotLevel()->GetLevelID();
}

LandsideParkingLotDoorInSim::LandsideParkingLotDoorInSim( LandsideParkingLotLevelInSim* pLot,ParkLotEntryExit& door )
{
	mpLotLevel = pLot;
	m_door = door;
	mVehicleOrder.resize(getPortCount(),NULL);
}

CPoint2008 LandsideParkingLotDoorInSim::getPos() const
{
	int nPtCount = m_door.m_line.getCount();
	if(nPtCount>0)
	{
		return (m_door.m_line.getPoint(0) + m_door.m_line.getPoint(nPtCount-1))*0.5;
	}
	return CPoint2008(0,0,0);
}

CString LandsideParkingLotDoorInSim::print() const
{
	LandsideParkingLotDoorInSim* pSpot = (LandsideParkingLotDoorInSim*)this;

	CString strRet;
	ARCVector3 pos = getPos();
	strRet.Format("ParkingLot %s Door (%.2f,%.2f)", pSpot->getParkingLot()->print().GetString()
		,pos[VX],pos[VY]); 
	return strRet;
}

int LandsideParkingLotDoorInSim::getVehiclePortIndex( LandsideVehicleInSim* pVeh ) const
{
	if(!bHasVehicle(pVeh))
		return -1;
	for(int i=0;i<(int)mVehicleOrder.size();i++)
	{
		if(mVehicleOrder[i]==pVeh)
			return i;
	}
	return -1;
}

int LandsideParkingLotDoorInSim::allocatVehicleToPort( LandsideVehicleInSim* pVeh )
{
	if(!bHasVehicle(pVeh))
	{
		for(int i=0;i<(int)mVehicleOrder.size();i++)
		{
			LandsideVehicleInSim* pPosVeh = mVehicleOrder[i];
			if( pPosVeh==pVeh || !bHasVehicle(pPosVeh) )
			{
				mVehicleOrder[i] = pVeh;
				return i;
			}
		}
	}
	return -1;
}

CPoint2008 LandsideParkingLotDoorInSim::getPortPos( int idx ) const
{
	double dlen = m_door.m_line.GetTotalLength();
	int nPortCount = getPortCount();
	CPoint2008 pt =  m_door.m_line.GetDistPoint(dlen*(idx+0.5)/nPortCount);
	pt.z = mpLotLevel->getHeight();
	return pt;
}

void LandsideParkingLotDoorInSim::OnVehicleExit( LandsideVehicleInSim* pVehicle,const ElapsedTime& t )
{
	__super::OnVehicleExit(pVehicle,t);
	
	NotifyObservers();
	//SendSignal(new ParkingLotStateChange());
		
}

LandsideLayoutObjectInSim* LandsideParkingLotDoorInSim::getLayoutObject()const
{
	return getParkingLot();
}

LandsideParkingLotInSim* LandsideParkingLotDoorInSim::getParkingLot() const
{
	return mpLotLevel->getLot();
}
//////////////////////////////////////////////////////////////////////////
LandsideParkingSpaceInSim::LandsideParkingSpaceInSim( LandsideParkingLotLevelInSim* pLot,ParkingSpace& space,int idx)
{
	mpLotLevel = pLot;
	mspace = space;
	m_index = idx;

	int i=0;
	CPoint2008 pos;
	ARCVector3 dir;
	CPoint2008 entrypos;
	double dHeight = pLot->getHeight();
	while(mspace.GetSpotDirPos(i,pos,dir,entrypos))
	{
		pos.setZ(dHeight);
		entrypos.setZ(dHeight);

		LandsideParkingSpotInSim* pSpot = new LandsideParkingSpotInSim(this, i, pLot->getLevelIndex());		

		pSpot->setPosDir(pos,dir);
		pSpot->setEntryPos(entrypos);


		if(pSpot->Is_parallel())
			pSpot->setEntryPos2(pos -entrypos + pos);
		m_vSpots.push_back(pSpot);
		++i;
	}

}

void LandsideParkingSpaceInSim::Clear()
{
	for(size_t i=0;i<m_vSpots.size();i++)
		delete m_vSpots[i];
	m_vSpots.clear();
}

LandsideParkingLotInSim* LandsideParkingSpaceInSim::getParkingLot() const
{
	return mpLotLevel->getLot();
}

LandsideParkingLotLevelInSim* LandsideParkingSpaceInSim::getParkingLotLevel() const
{
	return mpLotLevel;
}

LandsideParkingLotDrivePipeInSim::LandsideParkingLotDrivePipeInSim( LandsideParkingLotLevelInSim* pLotLevel,const ParkingDrivePipe& dpipe ,bool bLeftDrive)
{
	mpLotLevel = pLotLevel;
	m_drivelane = dpipe;

	int nLevelIndex = mpLotLevel->getLevelIndex();
	m_path = dpipe.GetPathRealZ((LandsideParkingLot*)getLayoutObject()->getInput(),nLevelIndex);

	ARCPipe pipe;
	pipe.Init(m_path,dpipe.m_width);
	for(int i=0;i<dpipe.m_nLaneNum;i++)
	{
		bool bOtherDir = false;
		if(dpipe.m_nType == ParkingDrivePipe::_bidirectional )
		{
			if(bLeftDrive)
				bOtherDir = (i*2>= dpipe.m_nLaneNum);
			else
				bOtherDir = (i*2 < dpipe.m_nLaneNum);
		}
		
		double drat = (i+.5)/dpipe.m_nLaneNum;
		LandsideParkinglotDriveLaneInSim* plane = new LandsideParkinglotDriveLaneInSim(this,bOtherDir );
		for(int k=0;k<m_path.getCount();k++)
		{

			plane->m_arcpath.push_back( pipe.m_sidePath1[k] *(1-drat) + pipe.m_sidePath2[k]*drat );
		}
		//reverse path
		if(bOtherDir)
			plane->m_arcpath.Reverse();

		plane->m_path = plane->m_arcpath.getPath2008();

		m_vLanes.push_back(plane);
	}
	
}

LandsideLayoutObjectInSim* LandsideParkingLotDrivePipeInSim::getLayoutObject()const
{
	return mpLotLevel->getLot();
}

bool LandsideParkingLotDrivePipeInSim::IsBiDirection() const
{
	if(m_drivelane.m_nLaneNum>1 && m_drivelane.m_nType == ParkingDrivePipe::_bidirectional){
		return true;
	}
	return false;
}

LandsideParkinglotDriveLaneInSim* LandsideParkingLotDrivePipeInSim::getLane( bool breverse )
{
	for(int i=0;i<(int)m_vLanes.size();i++){
		LandsideParkinglotDriveLaneInSim* pLane = m_vLanes[i];
		if(pLane->m_botherdir == breverse){
			return pLane;
		}
	}
	return NULL;
}


void LandsideParkingLotDrivePipeInSim::InitRelateWithOtherRes( LandsideResourceManager* allRes )
{
	for(size_t i=0;i<m_vLanes.size();++i){
		m_vLanes[i]->InitRelateWithOtherRes(allRes);
	}
}

LandsideParkingLotLevelInSim::LandsideParkingLotLevelInSim( LandsideParkingLotInSim* plot ,/* level_type t,*/ int idx )
{	
	m_pLot = plot;
	//m_type = t;
	m_idx = idx;	
	
}

void LandsideParkingLotLevelInSim::InitRelation( LandsideResourceManager* allRes )
{
	for(size_t i=0;i<m_vDoors.size();i++)
	{
		LandsideParkingLotDoorInSim* pDoor = m_vDoors[i];
		for(size_t j=0;j<pDoor->m_door.m_vLinkNodes.size();j++)
		{
			LaneNode& node = pDoor->m_door.m_vLinkNodes[j];
			LandsideLaneNodeInSim* pNodeInSim = allRes->GetlaneNode(node);

			if(pNodeInSim)
			{
				if( LandsideLaneEntry* pentry = pNodeInSim->toEntry() )
				{
					pentry->SetFromRes(pDoor);
					pDoor->m_vLaneEntries.push_back(pentry);
				}
				else if(LandsideLaneExit* pexit = pNodeInSim->toExit())
				{
					pexit->SetToRes(pDoor);
					pDoor->m_vLaneExits.push_back(pexit);
				}
			}			
		}		
	}
	//init drive pipe
	for(size_t j=0;j<m_vDriveLanes.size();++j){
		LandsideParkingLotDrivePipeInSim* pdp = m_vDriveLanes[j];
		pdp->InitRelateWithOtherRes(allRes);
	}
}

LandsideParkingSpotInSim* LandsideParkingLotLevelInSim::getFreeSpot( LandsideVehicleInSim* pVehicle )
{
	std::vector<LandsideParkingSpaceInSim*> vRamSpaceList = m_vParkSpaces;

	int nRandC = rand()%100;
	LandsideParkingSpotInSim* pFreeSpot = NULL;
	int nFreeCount =0;
	for(size_t i=0;i<vRamSpaceList.size();i++)
	{
		LandsideParkingSpaceInSim* pSpace = vRamSpaceList[i];
		for(int j=0;j<pSpace->getSpotCount();++j )
		{
			LandsideParkingSpotInSim* pSpot = pSpace->getSpot(j);
			if(pSpot->getOccupyVehicle() == NULL)
			{
				pFreeSpot = pSpot;
				nFreeCount++;
				if(nFreeCount>nRandC)
				{
					return pFreeSpot;
				}
			}
		}		
	}
	return pFreeSpot;
}

LandsideParkingLotDoorInSim* LandsideParkingLotLevelInSim::getExitDoor( LandsideVehicleInSim* pVehicle )
{
	for(size_t i=0;i<m_vDoors.size();i++)
	{
		LandsideParkingLotDoorInSim* pDoor = m_vDoors[i];
		if(!pDoor->m_vLaneEntries.empty())
			return pDoor;
	}
	return NULL;
}

bool LandsideParkingLotLevelInSim::IsSpotLess() const
{
	for(size_t i=0;i<m_vParkSpaces.size();i++)
	{
		LandsideParkingSpaceInSim* pSpace = m_vParkSpaces[i];
		if(pSpace->getSpotCount())
			return false;		
	}
	return true;
}

//
//LandsideParkingLotDoorInSim* LandsideParkingLotLevelInSim::getEntryDoor( LandsideVehicleInSim* pVehicle )
//{
//	for(size_t i=0;i<m_vDoors.size();i++)
//	{
//		LandsideParkingLotDoorInSim* pDoor = m_vDoors[i];
//		if(!pDoor->m_vLaneExits.empty())
//			return pDoor;
//	}
//	return NULL;
//}

void LandsideParkingLotLevelInSim::Clear()
{
	for(size_t i=0;i<m_vParkSpaces.size();i++)
		delete m_vParkSpaces[i];
	for(size_t i=0;i<m_vDoors.size();i++)
		delete m_vDoors[i];
	for(size_t i=0;i<m_vDriveLanes.size();i++)
		delete m_vDriveLanes[i];
	m_vDoors.clear();
	m_vParkSpaces.clear();
	m_vDriveLanes.clear();
}

const static DistanceUnit dDistZTolerance = 100;
void LandsideParkingLotLevelInSim::InitGraph(LandsideParkingLotGraph* m_pDriveGraph)
{
	//init entry exit to lanes
	for(size_t i=0;i<m_vDoors.size();i++)
	{
		LandsideParkingLotDoorInSim	* pDoor = m_vDoors[i];
		const CPath2008& path = pDoor->getInput().m_line;
		for(size_t i=0;i<m_vDriveLanes.size();i++)
		{
			LandsideParkingLotDrivePipeInSim* pLane1 = m_vDriveLanes[i];
			IntersectPathPath2D inters;
			int nCount = inters.Intersects(pLane1->getPath(),path,200);//in range 2 meter
			for(int k=0;k<nCount;k++)
			{
				double i1= inters.m_vIntersectPtIndexInPath1[k];
				double d1= pLane1->getPath().GetIndexDist((float)i1);
				double i2 = inters.m_vIntersectPtIndexInPath2[k];
				double d2 = path.GetIndexDist((float)i2);
				
				ARCVector3 pos1 = pLane1->getPath().GetIndexPoint((float)i1);
				ARCVector3 pos2 = path.GetIndexPoint((float)i2);
				if(abs(pos1.z - pos2.z) > dDistZTolerance)
				{
					continue;
				}

				LandsideResourceLinkNode node1(pLane1,d1,pDoor,d2);
				node1.mPos = pos1 ;
				LandsideResourceLinkNode node2(pDoor,d2,pLane1,d1);
				node2.mPos = node1.mPos;

				m_pDriveGraph->AddNode(node1);
				m_pDriveGraph->AddNode(node2);			
			}
		}

	}
	//init parking spot to drive lanes
	for(size_t i=0;i<m_vParkSpaces.size();i++)
	{
		LandsideParkingSpaceInSim* pSpace = m_vParkSpaces[i];
		for(int j=0;j<pSpace->getSpotCount();j++)
		{
			LandsideParkingSpotInSim* pSpot = pSpace->getSpot(j);
			ARCPoint3 entryPos = pSpot->getEntryPos();
			DistanceUnit distInlane = 0;
			DistanceUnit distToLane = 0;

			LandsideParkingLotDrivePipeInSim* pLane = getClosestLane(entryPos,distInlane, distToLane);			
			if(pLane){
				if(pSpot->Is_parallel())//use the closest entry pos for parallel
				{
					DistanceUnit otherdistInLane =0;
					DistanceUnit otherdistToLane =0;
					LandsideParkingLotDrivePipeInSim* pOtherLane = getClosestLane(pSpot->getEntryPos2(),otherdistInLane,otherdistToLane);
					if(pOtherLane ==pLane)
					{
						if(otherdistToLane < distToLane) 
						{
							distInlane = otherdistInLane;
							entryPos = pSpot->getEntryPos2();
						}
					}
					else if(pOtherLane)
					{
						LandsideResourceLinkNode node1(pSpot,0,pOtherLane,otherdistInLane);
						node1.mPos = pSpot->getEntryPos2();					
						m_pDriveGraph->AddNode(node1);

						LandsideResourceLinkNode node2(pOtherLane,otherdistInLane,pSpot,0);
						node2.mPos = pSpot->getEntryPos2();
						m_pDriveGraph->AddNode(node2);									
					}							
				}		
				

				LandsideResourceLinkNode node1(pSpot,0,pLane,distInlane);
				node1.mPos = entryPos;
				m_pDriveGraph->AddNode(node1);

				LandsideResourceLinkNode node2(pLane,distInlane,pSpot,0);
				node2.mPos = entryPos;
				m_pDriveGraph->AddNode(node2);

			}

		}
	}

	//init drive lanes between lanes
	for(size_t i=0;i<m_vDriveLanes.size();i++)
	{
		LandsideParkingLotDrivePipeInSim* pLane1 = m_vDriveLanes[i];
		for(size_t j=i+1;j<m_vDriveLanes.size();j++)
		{
			LandsideParkingLotDrivePipeInSim* pLane2 = m_vDriveLanes[j];

			IntersectPathPath2D inters;
			int nCount = inters.Intersects(pLane1->getPath(),pLane2->getPath(),pLane2->getWidth()*0.5);
			for(int k=0;k<nCount;k++)
			{
				double i1= inters.m_vIntersectPtIndexInPath1[k];
				double d1= pLane1->getPath().GetIndexDist((float)i1);
				double i2 = inters.m_vIntersectPtIndexInPath2[k];
				double d2 = pLane2->getPath().GetIndexDist((float)i2);


				ARCVector3 pos1 = pLane1->getPath().GetIndexPoint((float)i1);
				ARCVector3 pos2 = pLane2->getPath().GetIndexPoint((float)i2);
				if( abs(pos1.z - pos2.z) > ARCMath::EPSILON)
				{
					continue;
				}

				LandsideResourceLinkNode node1(pLane1,d1,pLane2,d2);
				node1.mPos = pos1;
				LandsideResourceLinkNode node2(pLane2,d2,pLane1,d1);
				node2.mPos = node1.mPos;

				m_pDriveGraph->AddNode(node1);
				m_pDriveGraph->AddNode(node2);			

			}

		}
	}


	//init lane link with other level lane
	for(int i=0;i<getLot()->getLevelCount();i++)
	{
		LandsideParkingLotLevelInSim* potherLevel = getLot()->getLevel(i);
		if(potherLevel == this)
			continue;

		for(size_t m=0;m<m_vDriveLanes.size();m++)
		{
			LandsideParkingLotDrivePipeInSim* pLane1 = m_vDriveLanes[m];
			for(size_t n =0;n<potherLevel->m_vDriveLanes.size();n++)
			{
				LandsideParkingLotDrivePipeInSim* pLane2 = potherLevel->m_vDriveLanes[n];
				DistanceUnit levelOffset  = abs(getHeight() - potherLevel->getHeight());

				IntersectPathPath2D inters;
				int nCount = inters.Intersects(pLane1->getPath(),pLane2->getPath(),pLane2->getWidth()*0.5);
				for(int k=0;k<nCount;k++)
				{
					double i1= inters.m_vIntersectPtIndexInPath1[k];
					double d1= pLane1->getPath().GetIndexDist((float)i1);
					double i2 = inters.m_vIntersectPtIndexInPath2[k];
					double d2 = pLane2->getPath().GetIndexDist((float)i2);


					ARCVector3 pos1 = pLane1->getPath().GetIndexPoint(i1);
					ARCVector3 pos2 = pLane2->getPath().GetIndexPoint(i2);
					if( abs(pos1.z - pos2.z) > levelOffset*0.1)
					{
						continue;
					}

					LandsideResourceLinkNode node1(pLane1,d1,pLane2,d2);
					node1.mPos = pos1;
					LandsideResourceLinkNode node2(pLane2,d2,pLane1,d1);
					node2.mPos = node1.mPos;

					m_pDriveGraph->AddNode(node1);
					m_pDriveGraph->AddNode(node2);			

				}

			}
		}
	}
}

LandsideParkingLotDrivePipeInSim* LandsideParkingLotLevelInSim::getClosestLane( const ARCPoint3& cpt, DistanceUnit& distInLane ,DistanceUnit& minDisTo ) const
{
	if(m_vDriveLanes.empty())
		return NULL;

	LandsideParkingLotDrivePipeInSim* pClostLane = NULL;

	CPoint2008 pt = CPoint2008(cpt.x,cpt.y,cpt.z);
	

	for(size_t i=0;i<m_vDriveLanes.size();i++)
	{
		LandsideParkingLotDrivePipeInSim* pLane = m_vDriveLanes.at(i);
		DistanceUnit thedistInLane = pLane->getPath().GetPointDist(pt);
		CPoint2008 posInLane = pLane->getPath().GetDistPoint(thedistInLane);
		if( abs( posInLane.z - pt.z )> ARCMath::EPSILON)
			continue;
		DistanceUnit distTo = pLane->getPath().GetDistPoint(thedistInLane).distance3D(pt)-pLane->getWidth()*0.5;
		if(!pClostLane)
		{
			minDisTo = distTo;
			pClostLane = pLane;
			distInLane = thedistInLane;
		}
		else if(distTo < minDisTo)
		{
			minDisTo = distTo;
			pClostLane = pLane;
			distInLane = thedistInLane;
		}

	}
	return pClostLane;
}

DistanceUnit LandsideParkingLotLevelInSim::getHeight() const
{
	LandsideParkingLotLevel* level=  getLot()->getLotInput()->GetLevel(m_idx);//[m_idx];//[m_idx(m_type==_other)?]:getLot()->getLotInput()->m_baselevel;
	ASSERT(level);
	return	level->m_dHeight;
}

int LandsideParkingLotLevelInSim::getLevelIndex() const
{
	return m_idx;//(m_type==_other)?m_idx:-1;
}

int LandsideParkingLotLevelInSim::GetLevelID()
{
	LandsideParkingLotLevel* level=  getLot()->getLotInput()->GetLevel(m_idx);
	ASSERT(level);
	return level->m_nLevelId;
}

void LandsideParkingLotLevelInSim::Init(bool bLeftDrive)
{
	LandsideParkingLotLevel* level=  getLot()->getLotInput()->GetLevel(m_idx);//(m_type==_other)?getLot()->getLotInput()->m_otherLevels[m_idx]:getLot()->getLotInput()->m_baselevel;

	ParkingSpaceList& spacelist=  level->m_parkspaces;
	for(int i=0;i<spacelist.getCount();i++)
	{
		LandsideParkingSpaceInSim* pspaceinsim = new LandsideParkingSpaceInSim(this, spacelist.getSpace(i),i);
		m_vParkSpaces.push_back(pspaceinsim);
	}

	//init doors
	ParkingLotEntryExitList& entryExit = level->m_entryexits;
	for(int i=0;i<entryExit.getCount();i++)
	{
		LandsideParkingLotDoorInSim* pDoorInSm = new LandsideParkingLotDoorInSim(this,entryExit.getEnt(i));
		m_vDoors.push_back(pDoorInSm);
	}
	//init drive lanes
	const ParkingDrivePipeList& drivelist = level->m_dirvepipes;
	for(int i=0;i<drivelist.getCount();++i)
	{
		LandsideParkingLotDrivePipeInSim* pNewLane = new LandsideParkingLotDrivePipeInSim(this, drivelist.getPipe(i),bLeftDrive);
		m_vDriveLanes.push_back(pNewLane);
	}
}

LandsideLayoutObjectInSim* LandsideParkinglotDriveLaneInSim::getLayoutObject() const
{
	return m_pPipe->getLayoutObject();
}

LandsideParkinglotDriveLaneInSim::LandsideParkinglotDriveLaneInSim( LandsideParkingLotDrivePipeInSim* pPipe ,bool otherdir)
{
	m_pPipe = pPipe;
	m_botherdir = otherdir;
	//m_arcpath = path;
}

void LandsideParkinglotDriveLaneInSim::CaculateIntesectWithWalkway( CWalkwayInSim* pRes )
{
	CPath2008 walkpath = pRes->GetCenterPath();

	IntersectWalkway inter;

	IntersectPathPath2D pathint;
	int nCount = pathint.Intersects(m_path,walkpath);
	for(int i=0;i<nCount;i++){
		double dp1 =  pathint.m_vIntersectPtIndexInPath1[i];
		double dp2 =  pathint.m_vIntersectPtIndexInPath2[i];
		CPoint2008 p1 = m_path.GetIndexPoint(dp1);
		CPoint2008 p2 = walkpath.GetIndexPoint(dp2);

		if( fabs(p1.getZ() - p2.getZ()) > 200 )  //differ height
			continue;

		double dwalkwaywidth  = pRes->GetInputWalkway()->getIndexWidth(dp2);
		inter.distInLane = m_path.GetIndexDist(dp1);
	
		inter.distHoldInLane = inter.distInLane - dwalkwaywidth*.5;
		inter.distHoldExitInLane = inter.distInLane + dwalkwaywidth*.5;
		inter.pWalkway = pRes;
		inter.distInWalkway  = walkpath.GetIndexDist(dp2);

		m_vIntersectWalkway.push_back(inter);
	}
}

void LandsideParkinglotDriveLaneInSim::InitRelateWithOtherRes( LandsideResourceManager* allRes )
{
	for(int i=0;i<allRes->m_vTrafficObjectList.getCount();i++){
		CLandsideWalkTrafficObjectInSim* pObj = allRes->m_vTrafficObjectList.getItem(i);
		if(CWalkwayInSim* pwlk = pObj->toWalkway()){
			CaculateIntesectWithWalkway(pwlk);
		}
	}
	std::sort(m_vIntersectWalkway.begin(),m_vIntersectWalkway.end());
}

int LandsideParkinglotDriveLaneInSim::GetIntersetWalks( DistanceUnit distF , DistanceUnit distT,std::vector<IntersectWalkway>& ret ) const
{
	for(size_t i=0;i<m_vIntersectWalkway.size();i++){
		const IntersectWalkway& intw = m_vIntersectWalkway[i];
		if( distF < intw.distInLane  && intw.distInLane < distT ){
			ret.push_back(intw);
		}
	}
	return (int)ret.size();
}

LandsideVehicleInSim* LandsideParkinglotDriveLaneInSim::GetAheadVehicle( LandsideVehicleInSim* mpVehicle, DistanceUnit distInLane, DistanceUnit& aheadDist )
{
	LandsideVehicleInSim* pRet = NULL;
	DistanceUnit minDist=0; 
	//bool bEntering = (mpVehicle->getLastState().getLandsideRes()!=this);

	for(int i=0;i<GetInResVehicleCount();i++)
	{
		LandsideVehicleInSim* pVeh = GetInResVehicle(i);
		if(pVeh==mpVehicle)
			continue;

		LandsideResourceInSim* pSameRes = NULL;
		DistanceUnit otherDist;
		if(pVeh->getLastState().getLandsideRes() == this)
		{
			otherDist = pVeh->getLastState().distInRes;
			pSameRes= this;
		}
		DistanceUnit buffer = 0;
		/*if(bEntering)
			buffer = mpVehicle->GetLength() + pVeh->GetLength();*/


		if(pSameRes)
		{
			if(otherDist > distInLane )
			{
				if(!pRet  || otherDist < minDist)
				{
					minDist = otherDist;
					pRet = pVeh;
				}
			}
		}

	}
	aheadDist = minDist;
	return pRet;
}
//LandsideVehicleInSim* LandsideParkinglotDriveLaneInSim::GetAheadVehicle( LandsideVehicleInSim* mpVehicle, DistanceUnit distInLane, DistanceUnit& aheadDist )
//{
//	VehicleList::const_iterator itr = std::find(m_vInResVehicle.begin(),m_vInResVehicle.end(),mpVehicle);
//	if(itr!=m_vInResVehicle.end())
//	{
//		if(itr!=m_vInResVehicle.begin())
//		{
//			VehicleList::const_iterator itrAhead = itr;
//			itrAhead--;
//			LandsideVehicleInSim* pV= *itrAhead;
//			aheadDist = pV->getLastState().distInRes;
//			ASSERT(pV->getLastState().getLandsideRes()==this);
//			return pV;
//		}
//	}
//	else
//	{
//		if(!m_vInResVehicle.empty())
//		{
//			LandsideVehicleInSim* pV= m_vInResVehicle.back();
//			aheadDist = pV->getLastState().distInRes;
//			ASSERT(pV->getLastState().getLandsideRes()==this);
//			return pV;
//		}
//	}
//	return NULL;
//}

void LandsideParkinglotDriveLaneInSim::OnVehicleEnter( LandsideVehicleInSim* pVehicle, DistanceUnit enterDist, const ElapsedTime& t )
{
	if(m_vInResVehicle.bHas(pVehicle))
	{
		ASSERT(false);
		return;
	}

	for(VehicleList::iterator itr = m_vInResVehicle.begin(); itr!=m_vInResVehicle.end(); ++itr)
	{
		LandsideVehicleInSim* pV = *itr;
		DistanceUnit otherDist = pV->getLastState().distInRes;
		if(enterDist > otherDist)
		{
			m_vInResVehicle.insert(itr,pVehicle);
			return;
		}	
	}
	m_vInResVehicle.push_back(pVehicle);
}

//LandsideVehicleInSim* LandsideParkinglotDriveLaneInSim::CheckPathConflict( LandsideVehicleInSim* pVeh, const CPath2008& path, DistanceUnit& Aheaddist )
//{
//	LandsideParkingLotLevelInSim* pLevel = m_pPipe->getLevel();
//	for(int i=0;i<pLevel->getPipeCount();i++)
//	{
//		LandsideParkingLotDrivePipeInSim* pipe = pLevel->getPipe(i);
//		for(int j=0;j<pipe->getLaneCount();++j)
//		{
//			LandsideParkinglotDriveLaneInSim* pLane = pipe->getLane(j);
//			if(pLane==this)	continue;
//
//			for(int k=0;k<pLane->GetInResVehicleCount();++k)
//			{
//				LandsideVehicleInSim* pV = pLane->GetInResVehicle(k);
//				if(pV==pVeh)continue;
//
//				DistanceUnit dist
//				
//			}
//		}
//	}
//}
