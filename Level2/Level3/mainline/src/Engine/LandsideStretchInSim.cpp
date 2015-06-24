#include "StdAfx.h"
#include ".\landsidestretchinsim.h"
#include "LandsideVehicleInSim.h"
#include <algorithm>
#include "Common/ARCRay.h"
#include "Common/Pollygon2008.h"
#include "LandsideObjectInSim.h"
#include "landside/LandsideStretch.h"
#include "common/ARCPipe.h"
#include "./CrossWalkInSim.h"
#include "LandsideResourceManager.h"
#include "landside\StretchSpeedConstraintList.h"
#include "landside\VehicleItemDetail.h"
#include "common/ARCUnit.h"
#include "LandsideStretchSegmentInSim.h"
#include "Common/Range.h"
//////////////////////////////////////////////////////////////////////////

LandsideResourceInSim* LandsideLaneEntry::getToRes()const
{
	return mpLane;
}

LandsideResourceInSim* LandsideLaneExit::getToRes() const
{
	return mpToRes;
}

LandsideResourceInSim* LandsideLaneExit::getFromRes() const
{
	return mpLane;
}

bool LandsideLaneExit::IsLaneDirectToCurb() const
{
	if(mpToRes)
		return mpToRes->toCurbSide()!=NULL;
	return false;
}
//////////////////////////////////////////////////////////////////////////
LandsideStretchInSim::LandsideStretchInSim( LandsideStretch* pStretch )
:LandsideLayoutObjectInSim(pStretch)
{

	//mpStretch = pStretch;
	m_Path = pStretch->getControlPath().getBuildPath();
	//double dWidth = pStretch->getLaneNum()*pStretch->getLaneWidth();
	//ARCPipe pipe(m_Path, dWidth);

	for(int i=0;i<pStretch->getLaneNum();i++)
	{
		//LandsideLane& lane = pStretch->getLane(i);
		LandsideStretchLaneInSim* newLane = new LandsideStretchLaneInSim(this,i);
		m_vLanes.push_back(newLane);
		CPath2008 lanePath;
		pStretch->GetLanePath(i,lanePath);		
		newLane->SetPath(lanePath);
		//add lane entry exit
        LandsideLaneEntry* newLaneEntry = new LandsideLaneEntry();
		newLaneEntry->SetPosition(newLane,0);
		newLane->SetFirstLaneEntry(newLaneEntry);

		LandsideLaneExit* newExit = new LandsideLaneExit();
		newExit->SetPosition(newLane,newLane->GetLength());
		newLane->SetLastLaneExit(newExit);
	}

}

LandsideStretchLaneInSim* LandsideStretchInSim::GetLane( int idx )const
{
	if(idx>=0 && idx<GetLaneCount())
		return m_vLanes.getItem(idx);
	return NULL;
}

void LandsideStretchInSim::AddCrossWalk( CCrossWalkInSim* pCross )
{
	for(int i=0;i<(int)m_vLanes.size();i++)
	{
		LandsideStretchLaneInSim* pLane = m_vLanes[i];
		double dDistInStretch = pCross->getAtStretchPos();
		CPoint2008 pt = m_Path.GetDistPoint(dDistInStretch);		
		pLane->AddCross(pCross,pt);
	}
}



void LandsideStretchInSim::InitGraphNodes( LandsideRouteGraph* pGraph ) const
{	
	for(int j=0;j<GetLaneCount();j++)
	{
		LandsideStretchLaneInSim* pLane = GetLane(j);
		pGraph->AddNode(pLane->GetFirstLaneEntry());
		pGraph->AddNode(pLane->GetLastLaneExit());
		for(size_t k=0;k<pLane->mvEntryExits.size();k++)
		{
			pGraph->AddNode(pLane->mvEntryExits[k]);
		}
	}
}

//linkages of the lane
class LaneNodeLinkage : public LaneNodeLinkageInSim
{
public:	
	LandsideLaneEntry* mpLaneEntry;
	LandsideLaneExit* mpLaneExit;
	virtual LandsideLaneNodeInSim* getFromNode()const{ return mpLaneEntry; }
	virtual LandsideLaneNodeInSim* getToNode()const{ return mpLaneExit; };
	virtual double getDistance()const{ return mpLaneExit->m_distInlane - mpLaneEntry->m_distInlane; }
};
void InitLinkage(const LandsideStretchInSim* pStretch, std::vector<LaneNodeLinkage>& m_vLinkages)
{
	for(int i=0;i<pStretch->GetLaneCount();i++)
	{
		LandsideStretchLaneInSim* pLane = pStretch->GetLane(i);	
		pLane->sortEntryExits();

		for(LandsideLaneEntry* pFirst = pLane->GetFirstLaneEntry();pFirst;pFirst=pLane->GetNextLaneEntry(pFirst))
		{
			LandsideLaneExitList vExits = pLane->GetLaneExitsAfterEntry(pFirst);
			for(size_t j=0;j<vExits.size();++j)
			{
				LandsideLaneExit* pExt = vExits[j];
				LaneNodeLinkage linkage;
				linkage.mpLaneEntry = pFirst;
				linkage.mpLaneExit = pExt;					
				m_vLinkages.push_back(linkage);
				if(pExt->IsLaneDirectToCurb())
					break;
			}
		}
	}
}


void LandsideStretchInSim::InitGraphLink( LandsideRouteGraph* pGraph ) const
{

	std::vector<LaneNodeLinkage> m_vLinkages;
	InitLinkage(this, m_vLinkages);

	for( size_t i=0;i< m_vLinkages.size(); i++)
	{
		LaneNodeLinkageInSim& linkage = m_vLinkages[i];
		pGraph->AddEdge(linkage.getFromNode(),linkage.getToNode(),linkage.getDistance());
	}

}

void LandsideStretchInSim::InitRelateWithOtherObject( LandsideResourceManager* pRes )
{
	for(size_t i=0 ;i<pRes->m_vStretchSegs.size();i++){
		LandsideStretchSegmentInSim* pSeg = pRes->m_vStretchSegs[i];
		if(!pSeg->GetLandsideSeg()->IsLinkToStretch(getInput()))
			continue;

		for(size_t j=0;j<m_vLanes.size();j++){
			LandsideStretchLaneInSim* pLane = m_vLanes.ItemAt(j);
			int nLane = pLane->GetLaneIndex() + 1;
			if( pSeg->GetLandsideSeg()->isLaneSeg( nLane/*pLane->GetLaneIndex()*/ ) )
			{
				pLane->AddLaneSeg(pSeg);
			}
		}
	}
}

LandsideLayoutObjectInSim* LandsideStretchInSim::getLayoutObject() const
{
	return const_cast<LandsideStretchInSim*>(this);
}


CString LandsideStretchInSim::print() const
{
	return getInput()->getName().GetIDString();
}



//////////////////////////////////////////////////////////////////////////
LandsideStretchLaneInSim::LandsideStretchLaneInSim( LandsideStretchInSim* pStretch, int idx )
{
	mpParent = pStretch;
	m_Idx = idx;
	m_bHasParkingSpot = false;
	//m_dWidth = pStretch->mpStretch->getLaneWidth();
}

void LandsideLaneInSim::Clear()
{
	for(size_t i=0;i<mvEntryExits.size();i++)
		delete mvEntryExits[i];
	mvEntryExits.clear();	
}

LandsideLaneInSim* LandsideStretchLaneInSim::getLeft(  ) const
{	
	return mpParent->GetLane(m_Idx-1);	
}

LandsideLaneInSim* LandsideStretchLaneInSim::getRight(  ) const
{
	return mpParent->GetLane(m_Idx+1);	
}

bool LandsideStretchLaneInSim::IsSameDirToLane( LandsideLaneInSim* plane ) const
{
	if(LandsideStretchLaneInSim*pOtherLane =  plane->toStretchLane())
	{
		 return IsPositiveDir() == pOtherLane->IsPositiveDir();
	}
	return false;
}

LandsideLaneEntry* LandsideStretchLaneInSim::GetNextLaneEntry( double dist ) const
{
	if( mpFirstEntry && mpFirstEntry->m_distInlane>dist)
		return mpFirstEntry;

	for(size_t i=0;i<mvEntryExits.size();i++)
	{
		LandsideLaneNodeInSim * pNode = mvEntryExits.at(i);
		if(LandsideLaneEntry * pEntry = pNode->toEntry())
		{
			if(pEntry->m_distInlane>dist)
				return pEntry;
		}
	}
	return NULL;
}

LandsideLaneEntry* LandsideStretchLaneInSim::GetNextLaneEntry( LandsideLaneEntry* _pEntry ) const
{
	bool bFirstEntry = (mpFirstEntry==_pEntry);

	for(size_t i=0;i<mvEntryExits.size();i++)
	{
		LandsideLaneNodeInSim * pNode = mvEntryExits.at(i);
		if(LandsideLaneEntry * pEntry = pNode->toEntry())
		{
			if(bFirstEntry)
				return pEntry;
			else
			{
				if( _pEntry == pEntry)
					bFirstEntry = true;
			}
		}
	}
	return NULL;
}


LandsideLaneExit* LandsideStretchLaneInSim::GetNextLaneExit( double dist ) const
{
	for(size_t i=0;i<mvEntryExits.size();i++)
	{
		LandsideLaneNodeInSim * pNode = mvEntryExits.at(i);
		if(LandsideLaneExit * pExit = pNode->toExit())
		{
			if(pExit->m_distInlane>dist)
				return pExit;
		}
	}

	if(mpLastExit && mpLastExit->m_distInlane > dist)
		return mpLastExit;

	return NULL;
}

LandsideLaneExitList LandsideStretchLaneInSim::GetLaneExitsAfterEntry( LandsideLaneEntry* _pEntry ) const
{
	bool bFindEntry = (mpFirstEntry==_pEntry);
	LandsideLaneExitList pExits;
	for(size_t i=0;i<mvEntryExits.size();i++)
	{
		LandsideLaneNodeInSim * pNode = mvEntryExits.at(i);
		if(pNode==_pEntry)
			bFindEntry = true;
		
		if(bFindEntry)
		{
			if(LandsideLaneExit * pExit = pNode->toExit())
			{
				pExits.push_back(pExit);
			}

		}
	}

	if(mpLastExit && mpLastExit->m_distInlane > _pEntry->m_distInlane)
	{
		pExits.push_back(mpLastExit);
	}

	return pExits;

}

LandsideLaneExitList LandsideStretchLaneInSim::GetLaneExitsAfterDist( double dist ) const
{
	LandsideLaneExitList extlist;
	for(size_t i=0;i<mvEntryExits.size();i++)
	{
		LandsideLaneNodeInSim * pNode = mvEntryExits.at(i);
		if(LandsideLaneExit * pExit = pNode->toExit())
		{
			if(pExit->m_distInlane>dist)
			{	
				extlist.push_back(pExit);
			}
		}
	}

	if(mpLastExit )
	{
		if( mpLastExit->m_distInlane > dist)
			extlist.push_back(mpLastExit);
	}
	return extlist;
}

bool LandsideStretchLaneInSim::IsPositiveDir() const
{
	LandsideStretch* pStretch = (LandsideStretch*)mpParent->getInput();
	return pStretch->getLane(m_Idx).m_dir == LandsideLane::_positive;
}

CString LandsideStretchLaneInSim::print() const
{
	CString strRet;
	LandsideStretch* pStretch = (LandsideStretch*)mpParent->getInput();
	strRet.Format("%s(%d)",pStretch->getName().GetIDString(),m_Idx);
	return strRet;
}




LandsideVehicleInSim* LandsideLaneInSim::GetBehindVehicle( LandsideVehicleInSim* mpVehicle, double distInLane )
{
	ElapsedTime curt = mpVehicle->curTime();
	LandsideVehicleInSim* pRet = NULL;
	double maxDist; 
	for(int i=0;i<GetInResVehicleCount();i++)
	{
		LandsideVehicleInSim* pVeh = GetInResVehicle(i);
		if(pVeh!=mpVehicle)
		{
			if(pVeh->getState(curt).getLandsideRes() == this )
			{
				double otherDist = pVeh->getState(curt).distInRes;
				if(otherDist <= distInLane )
				{
					if(!pRet  || otherDist > maxDist){
						maxDist = otherDist;
						pRet = pVeh;
					}
				}

			}
		}
	}
	return pRet;
}




struct LaneNodeLess
{
	bool operator()(LandsideLaneNodeInSim* pNode1, LandsideLaneNodeInSim* pNode2)
	{
		return pNode1->m_distInlane < pNode2->m_distInlane;
	}
};
void LandsideLaneInSim::sortEntryExits()
{
	std::sort(mvEntryExits.begin(),mvEntryExits.end(), LaneNodeLess() );
}

void LandsideLaneInSim::SetPath( const CPath2008& path )
{
	m_path = path; 
	m_dLength = path.GetTotalLength();
}

void LandsideLaneInSim::AddLaneNode( LandsideLaneNodeInSim* pNode )
{
	ASSERT(pNode->getToRes());
	ASSERT(pNode->getFromRes());
	mvEntryExits.push_back(pNode);
}

CPath2008 LandsideLaneInSim::GetPathLeft( DistanceUnit distF ) const
{
	return m_path.GetSubPathToEndDist(distF);
}

DistanceUnit LandsideLaneInSim::GetPointDist( const CPoint2008& pos ) const
{
	return m_path.GetPointDist(pos);
}

CPoint2008 LandsideLaneInSim::GetDistPoint( DistanceUnit distF ) const
{
	return CPoint2008(m_path.GetDistPoint(distF));
}

ARCVector3 LandsideLaneInSim::GetDistDir( DistanceUnit dist ) const
{
	return m_path.GetDistDir(dist);
}

//LandsideVehicleInSim* LandsideLaneInSim::GetVehicleAhead( LandsideVehicleInSim* pVehicle ) const
//{
//	VehicleList::const_iterator itr = std::find(m_vInResVehicle.begin(),m_vInResVehicle.end(),pVehicle);
//	if(itr!=m_vInResVehicle.end()){
//		if(itr!=m_vInResVehicle.begin()){
//			itr--;
//			return *itr;
//		}
//		else
//			return NULL;
//	}
//	if(!m_vInResVehicle.empty()){
//		return m_vInResVehicle.back();
//	}
//	return NULL;
//}

//LandsideLaneExitList LandsideLaneInSim::GetLaneExitsAfterDist( double dist ) const
//{
//	ASSERT(false);
//	return LandsideLaneExitList();
//}

bool LandsideLaneInSim::isSpaceEmpty(LandsideVehicleInSim* pV, DistanceUnit distF, DistanceUnit distT ) const
{
	typedef CRange<DistanceUnit> DistaneRange;
	DistaneRange distIn(distF,distT);

	for(size_t i=0;i<m_vInResVehicle.size();i++)
	{
		LandsideVehicleInSim* pVehicle =  m_vInResVehicle.at(i);
		if(pV==pVehicle)continue;

		DistanceUnit idistT =  pVehicle->getLastState().distInRes + pVehicle->GetSafeDistToFront();
		DistanceUnit idistF = pVehicle->getLastState().distInRes - pVehicle->GetLength()*0.5;
		if( distIn.isOverlap( DistaneRange(idistF,idistT) ) )
			return false;	
	}

	return true;
}




void LandsideStretchLaneInSim::AddCross( CCrossWalkInSim* pCross,const CPoint2008& crosspos )
{
	CrossWalkInlane cross;
	cross.pCross = pCross;
	cross.distInlane = m_path.GetPointDist(crosspos);
	m_crosswalks.push_back(cross);
	std::sort(m_crosswalks.begin(),m_crosswalks.end());
}

CCrossWalkInSim* LandsideStretchLaneInSim::GetAheadCross( LandsideVehicleInSim* pVeh, double dfromDist, double& dCrossDist )const
{
	for(CrossWalkInlaneList::const_iterator itr=m_crosswalks.begin();itr!=m_crosswalks.end();++itr)
	{		
		CCrossWalkInSim* pCross = itr->pCross;
		if(itr->distInlane  >= dfromDist) 
		{
			if(!pCross->IsVehicleOccupied(pVeh))
			{
				dCrossDist=itr->distInlane;
				return itr->pCross;
			}
		}
	}
	return NULL;
}



void LandsideStretchLaneInSim::ReleaseCrossOcys( LandsideVehicleInSim* pVeh,double dDist )
{
	for(CrossWalkInlaneList::const_iterator itr=m_crosswalks.begin();itr!=m_crosswalks.end();++itr)
	{
		double dCrossHalfWidth = itr->pCross->getHalfWidth();
		if(itr->distInlane+dCrossHalfWidth<dDist-pVeh->GetLength()*0.5 && itr->pCross->IsVehicleOccupied(pVeh))
		{			
			itr->pCross->ReleaseVehilceOccupied(pVeh);
		}
	}
}

void LandsideStretchLaneInSim::LeaveCrosswalkQueue(LandsideVehicleInSim* pVeh,double dDist)
{
	for(CrossWalkInlaneList::const_iterator itr=m_crosswalks.begin();itr!=m_crosswalks.end();++itr)
	{
		double dCrossHalfWidth = itr->pCross->getHalfWidth();
		if(itr->distInlane+dCrossHalfWidth<dDist-pVeh->GetLength()*0.5 && itr->pCross->IsVehicleOccupied(pVeh))
		{			
			if (pVeh->WaitInCrosswalkQueue(itr->pCross))
			{
				LandsideResourceQueueItem* pQueue = new LandsideResourceQueueItem;
				pQueue->m_nVehicleID = pVeh->getID();
				pQueue->m_nResourceID = itr->pCross->GetCrosswalk()->getID();
				pQueue->m_strResName = itr->pCross->GetCrosswalk()->getName().GetIDString();
				pQueue->m_eTime = pVeh->curTime();
				pQueue->m_enumOperation = LandsideResourceQueueItem::QO_Exit;

				pVeh->LeaveCrosswalkQueue(itr->pCross);
			}
			
		}
	}
}

void LandsideStretchLaneInSim::Clear()
{
	__super::Clear();
	delete mpFirstEntry; mpFirstEntry = NULL;
	delete mpLastExit;mpLastExit = NULL;
}



LandsideLayoutObjectInSim* LandsideStretchLaneInSim::getLayoutObject() const
{
	return mpParent;
}

StretchSpeedControlData* LandsideStretchLaneInSim::GetSpeedControl( double dist ) const
{
	for(size_t i=0;i<m_vStretchSegment.size();i++){
		LandsideStretchSegmentInSim* pseg =  m_vStretchSegment[i];
		//if( pseg->GetLandsideSeg()->isDistInseg(dist))
		if (pseg->GetLandsideSeg()->isDistInseg(m_Idx,dist))
		{
			if(StretchSpeedControlData *pData = pseg->GetSpeedControlData() )
			{
				return pData;
			}
		}
		
	}
	if(LandsideLayoutObjectInSim* pobj = getLayoutObject() ){
		return pobj->GetSpeedControlData();
	}
	
	return  NULL;
}

CPath2008 LandsideStretchLaneInSim::GetSubPathDist( DistanceUnit distF, DistanceUnit distT ) const
{
	return m_path.GetSubPath(distF,distT);
}

void LandsideStretchLaneInSim::AddLaneSeg( LandsideStretchSegmentInSim* pobj )
{
	m_vStretchSegment.push_back(pobj);
}

LandsideLaneEntry* LandsideStretchLaneInSim::GetFirstLaneEntry() const
{
	return mpFirstEntry;
}

LandsideLaneExit* LandsideStretchLaneInSim::GetLastLaneExit() const
{
	return mpLastExit;
}

LandsideLaneExitList LandsideStretchLaneInSim::GetOtherLaneExitsAfterEntry( LandsideLaneEntry* pEntry ) const
{
	LandsideLaneExitList extlist;
	LandsideStretchInSim* pStreth = mpParent;
	for(int i=0;i<pStreth->GetLaneCount();i++)
	{
		LandsideLaneInSim* plane = pStreth->GetLane(i);
		if(plane == this) continue;

		DistanceUnit dist = plane->GetPointDist(pEntry->GetPosition());
		LandsideLaneExitList otherExits = plane->GetLaneExitsAfterDist(dist);
		extlist.insert(extlist.end(), otherExits.begin(),otherExits.end());
	}
	return extlist;
}

LandsideLaneEntry* LandsideStretchLaneInSim::GetPreLaneEntry( double dist ) const
{
	return GetFirstLaneEntry();
}

LandsideVehicleInSim* LandsideStretchLaneInSim::GetAheadVehicle( LandsideVehicleInSim* mpVehicle, DistanceUnit distInLane, DistanceUnit& aheadDist )
{
	LandsideVehicleInSim* pRet = NULL;
	DistanceUnit minDist=0; 

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
		else
		{
			LaneParkingSpot* lps =NULL;
			LandsideResourceInSim* pOtherRes = pVeh->getLastState().getLandsideRes();
			if(pOtherRes)
				lps = pOtherRes->toLaneSpot();
			if(lps && lps->GetLane()==this)
			{
				pSameRes = this;
				otherDist = lps->GetDistInLane();
			}
		}
		
			
		if(pSameRes)
		{
			if(otherDist > distInLane )
			{
				if(!pRet  || otherDist < minDist){
					minDist = otherDist;
					pRet = pVeh;
				}
			}
		}
		
	}
	aheadDist = minDist;
	return pRet;
}

void LandsideStretchLaneInSim::OnVehicleEnter( LandsideVehicleInSim* pVehicle, DistanceUnit enterDist, const ElapsedTime& t )
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

//bool LandsideStretchLaneInSim::isSpaceEmpty( LandsideVehicleInSim* pV, DistanceUnit distF, DistanceUnit distT ) const
//{
//	if(!__super::isSpaceEmpty(pV,distF,distT))
//		return false;
//
//	for(size_t i=0;i<m_crosswalks.size();i++)
//	{
//		const CrossWalkInlane& csInLane = m_crosswalks[i];
//		
//	}
//}



//////////////////////////////////////////////////////////////////////////
#include "Landside/LandsideIntersectionNode.h"
#include "LandsideResourceManager.h"

void LandsideLaneNodeInSim::SetPosition( LandsideLaneInSim* plane , double distInlane )
{
	mpLane = plane;
	m_distInlane = distInlane;
	//const ARCPath3& lanepath = plane->getPath();
	m_dir = plane->GetDistDir(distInlane);
	m_pos = plane->GetDistPoint(distInlane);	
}

void LandsideLaneNodeInSim::SetPosition( LandsideLaneInSim* plane, const CPoint2008& pt )
{
	mpLane = plane;
	m_pos = pt;
	m_distInlane = plane->GetPointDist(m_pos);
	//const ARCPath3& lanepath = plane->getPath();
	m_dir = plane->GetDistDir(m_distInlane);
}

void LandsideLaneNodeInSim::Offset( double dist )
{
	SetPosition(mpLane,dist+m_distInlane);
}




void LandsideLaneNodeList::GetLaneEntryList( std::vector<LandsideLaneEntry*>& vEntries )
{
	for(iterator itr = begin();itr!=end();++itr)
	{
		LandsideLaneNodeInSim* pNode = *itr;
		if(LandsideLaneEntry* pentry = pNode->toEntry())
		{
			vEntries.push_back(pentry);
		}
	}
	
}

void LandsideLaneNodeList::GetLaneExitList( std::vector<LandsideLaneExit*>& vExits )
{
	for(iterator itr = begin();itr!=end();++itr)
	{
		LandsideLaneNodeInSim* pNode = *itr;
		if(LandsideLaneExit* pexit = pNode->toExit())
		{
			vExits.push_back(pexit);
		}
	}
}

LandsideLaneNodeInSim* LandsideLaneNodeList::GetNode( int idx )
{
	if(idx >=0 && idx < (int)size())
	{
		return at(idx);
	}
	return NULL;
}
