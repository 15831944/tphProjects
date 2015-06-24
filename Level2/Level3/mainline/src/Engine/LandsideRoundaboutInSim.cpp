#include "StdAfx.h"
#include ".\landsideroundaboutinsim.h"
#include "landside\LandsideRoundabout.h"
#include "LandsideResourceManager.h"
#include "LandsideVehicleInSim.h"
#include "Common\DistanceLineLine.h"

LandsideRoundaboutLaneInSim::LandsideRoundaboutLaneInSim( LandsideRoundaboutInSim* pabout, int idx )
:m_pParent(pabout)
,m_idx(idx)
{

}


LandsideLaneInSim* LandsideRoundaboutLaneInSim::getLeft() const
{
	if(m_pParent->IsClockWise())
	{
		return m_pParent->getLane(m_idx+1);
	}
	else
		return m_pParent->getLane(m_idx-1);
}

LandsideLaneInSim* LandsideRoundaboutLaneInSim::getRight() const
{
	if(m_pParent->IsClockWise())
	{
		return m_pParent->getLane(m_idx-1);
	}
	else
		return m_pParent->getLane(m_idx+1);
}

CString LandsideRoundaboutLaneInSim::print() const
{
	CString sret;
	sret.Format(_T("%s(%d)"),__super::print().GetString(),m_idx );
	return sret;
}

bool LandsideRoundaboutLaneInSim::IsSameDirToLane( LandsideLaneInSim* plane ) const
{
	if(LandsideRoundaboutLaneInSim* pAboutlane  = plane->toAboutLane())
	{
		if(pAboutlane->getAbout() == getAbout())
		{
			return true;
		}
	}
	return false;
}

ARCPath3 operator+(const ARCPath3& p1, const ARCPath3&p2)
{
	ARCPath3 p;
	p.insert(p.end(),p1.begin(),p1.end());
	p.insert(p.end(),p2.begin(),p2.end());
	return p;
}

CPath2008 LandsideRoundaboutLaneInSim::GetSubPathDist( DistanceUnit distF, DistanceUnit distT ) const
{
	if(distT<distF)
	{
		return m_path.GetSubPathToEndDist(distF) + m_path.GetSubPathFromBeginDist(distT);
	}
	
	return m_path.GetSubPath(distF,distT);
}

ARCVector3 LandsideRoundaboutLaneInSim::GetDistDir( DistanceUnit dist ) const
{
	
	DistanceUnit d = fmod(dist,GetLength());
	return __super::GetDistDir(d);
}

CPoint2008 LandsideRoundaboutLaneInSim::GetDistPoint( DistanceUnit dist ) const
{
	DistanceUnit d = fmod(dist,GetLength());
	return __super::GetDistPoint(d);
}

LandsideLayoutObjectInSim* LandsideRoundaboutLaneInSim::getLayoutObject() const
{
	return m_pParent;
}

LandsideLaneExitList LandsideRoundaboutLaneInSim::GetLaneExitsAfterDist( double ) const
{
	return LandsideLaneExitList();
}

LandsideVehicleInSim* LandsideRoundaboutLaneInSim::GetAheadVehicle( LandsideVehicleInSim* mpVehicle, DistanceUnit distInLane, DistanceUnit& aheadDist )
{
	return NULL;
	LandsideVehicleInSim* pRet = NULL;
	DistanceUnit minDist=0; 

	for(int i=0;i<GetInResVehicleCount();i++)
	{
		LandsideVehicleInSim* pVeh = GetInResVehicle(i);
		if(pVeh!=mpVehicle)
		{
			if(pVeh->getLastState().getLandsideRes() == this )
			{
				double otherDist = pVeh->getLastState().distInRes;
				otherDist = fmod(otherDist,GetLength());
				if(otherDist < distInLane )
				{
					otherDist += GetLength();
				}
				//check
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

//LandsideVehicleInSim* LandsideRoundaboutLaneInSim::CheckSpaceConflict( LandsideVehicleInSim* pVeh, const ARCPolygon& poly )
//{
//	LandsideRoundaboutInSim* pRound = m_pParent;
//	for(int i=0;i<pRound->GetInResVehicleCount();i++)
//	{
//		LandsideVehicleInSim* pOtherV = pRound->GetInResVehicle(i);
//		if(pOtherV==pVeh)continue;
//
//		LandsideResourceInSim* pOtherRes = pOtherV->getLastState().getLandsideRes();
//		if(!pOtherRes)continue;
//		
//		if( !pVeh->bCanWaitFor(pOtherV) )
//			continue;
//
//		if( poly.IsOverlapWithOtherPolygon(pOtherV->getBBox(pVeh->curTime()) ) )
//			return pOtherV;
//	}
//	return NULL;
//}

LandsideVehicleInSim* LandsideRoundaboutLaneInSim::CheckPathConflict( LandsideVehicleInSim* pVeh, const CPath2008& path, DistanceUnit& dist )
{
	LandsideRoundaboutInSim* pRound = m_pParent;
	return pRound->CheckPathConflict(pVeh,path,dist);
}

void LandsideRoundaboutLaneInSim::OnVehicleEnter( LandsideVehicleInSim* pVehicle, DistanceUnit dist,const ElapsedTime& t )
{
	m_pParent->OnVehicleEnter(pVehicle,dist,t);
	__super::OnVehicleEnter(pVehicle,dist,t);
}

void LandsideRoundaboutLaneInSim::OnVehicleExit( LandsideVehicleInSim* pVehicle,const ElapsedTime& t )
{
	m_pParent->OnVehicleExit(pVehicle,t);
	__super::OnVehicleExit(pVehicle,t);
}



LandsideRoundaboutInSim::LandsideRoundaboutInSim( LandsideRoundabout* pabout )
:LandsideLayoutObjectInSim(pabout)
{
	//m_pInput = pabout;

	//double dWidth = pabout->getOutRadius() - pabout->getInnerRadius();
	//create lanes
	for(int i=0;i<pabout->getLaneNumber();i++)
	{
		LandsideRoundaboutLaneInSim* pLane = new LandsideRoundaboutLaneInSim(this,i);
		CPath2008 path;
		pabout->getLanePath(i, path);
		pLane->SetPath(path);
		//pLane->m_dWidth = dWidth
		m_vLanes.addItem(pLane);
	}

	
}

LandsideRoundaboutLaneInSim* LandsideRoundaboutInSim::getLane( int idx ) const
{
	if(idx<m_vLanes.getCount() && idx>=0)
		return m_vLanes.getItem(idx);
	return NULL;
}

BOOL LandsideRoundaboutInSim::IsClockWise() const
{
	LandsideRoundabout* pabout = (LandsideRoundabout*)getInput();
	return pabout->isClockwise();
}

void LandsideRoundaboutInSim::InitRelateWithOtherObject( LandsideResourceManager* allRes )
{
	LandsideRoundabout* pabout = (LandsideRoundabout*)getInput();
	//init intersection with other stretch
	int nLinkCnt = pabout->getLinkStretchCount();
	for(int i=0;i<nLinkCnt;++i)
	{
		StretchSide& stretchside = 	pabout->getLinkStretch(i);
		if(!stretchside.IsValid())
			continue;
		LandsideStretchInSim* pStretch = allRes->getStretchByID(stretchside.m_pStretch->getID());
		if(!pStretch)
			continue;		
			
		int LaneCount = pStretch->GetLaneCount();
		for(int j=0;j<LaneCount;++j)
		{
			LandsideStretchLaneInSim* plane = pStretch->GetLane(j);
			if( plane->IsPositiveDir() == (stretchside.ntype==_stretch_exit) )//entry to roundabout
			{
				for(int iRALane = 0;iRALane <m_vLanes.getCount();iRALane++ )
				{
					CPath2008 entryPath;
					LandsideRoundaboutLaneInSim* pRALane = m_vLanes.getItem(iRALane);
					if(pabout->GetEntryLanePath(stretchside,j,iRALane,entryPath))
					{						
						LandsideLaneEntry*pentry = new LandsideLaneEntry();						
						pentry->SetFromRes(this);
						pentry->SetPosition(pRALane, *entryPath.points.rbegin() );
						pRALane->AddLaneNode(pentry);
						

						LandsideAboutLaneLink* pLink = new LandsideAboutLaneLink;
						plane->GetLastLaneExit()->SetToRes(this);
						pLink->pFrom = plane->GetLastLaneExit();						
						pLink->pTo = pentry;
						pLink->m_path = entryPath; ///add
						pLink->m_dist = entryPath.GetTotalLength();
						m_vLinks.addItem(pLink);
					}

				}
			}
			else//exit roundabout
			{
				for(int iRALane = 0;iRALane <m_vLanes.getCount();iRALane++ ) 
				{
					CPath2008 exitPath;
					LandsideRoundaboutLaneInSim* pRALane = m_vLanes.getItem(iRALane);
					if(pabout->GetExitLanePath(stretchside,j,iRALane,exitPath))
					{
						LandsideLaneExit* pexit = new LandsideLaneExit();
						pexit->SetToRes(this);
						pexit->SetPosition(pRALane, CPoint2008(*exitPath.points.begin()) );
						pRALane->AddLaneNode(pexit);

						LandsideAboutLaneLink* pLink = new LandsideAboutLaneLink;
						pLink->pFrom = pexit;
						plane->GetFirstLaneEntry()->SetFromRes(this);
						pLink->pTo = plane->GetFirstLaneEntry();
						pLink->m_path = exitPath;
						pLink->m_dist = exitPath.GetTotalLength();
						m_vLinks.addItem(pLink);
					}

				}
			}
		}
		
	}
	//init graph



}

void LandsideRoundaboutInSim::InitGraphNodes( LandsideRouteGraph* pGraph ) const
{
	for(int i=0;i<m_vLanes.getCount();i++ )
	{
		LandsideRoundaboutLaneInSim* plane=m_vLanes.getItem(i);
		for(size_t j=0;j<plane->mvEntryExits.size();++j)
			pGraph->AddNode(plane->mvEntryExits.at(j));
	}
}


void LandsideRoundaboutInSim::InitGraphLink( LandsideRouteGraph* pGraph ) const
{
	//add  lane node 
	for(int i=0;i<m_vLanes.getCount();i++ )
	{
		LandsideRoundaboutLaneInSim* plane=m_vLanes.getItem(i);
		std::vector<LandsideLaneEntry*> vEntries;
		std::vector<LandsideLaneExit*> vExits;
		plane->mvEntryExits.GetLaneEntryList(vEntries);
		plane->mvEntryExits.GetLaneExitList(vExits);
		for(size_t m =0;m<vEntries.size();m++)
			for(size_t n=0;n<vExits.size();n++)
			{
				LandsideLaneEntry*pentry = vEntries[m];
				LandsideLaneExit*pexit = vExits[n];
				double dist = pexit->m_distInlane - pentry->m_distInlane;
				if(dist<0)
					dist += plane->GetLength();
				pGraph->AddEdge(pentry,pexit,dist);
			}
	}
	//linkage with other lane
	for (int i=0;i<m_vLinks.getCount();i++)
	{
		LandsideAboutLaneLink* plink = m_vLinks.getItem(i);
		pGraph->AddEdge(plink->getFromNode(),plink->getToNode(),plink->getDistance());
	}
}

CString LandsideRoundaboutInSim::print() const
{
	return getInput()->getName().GetIDString();
}

bool LandsideRoundaboutInSim::GetLinkPath( LandsideLaneNodeInSim* pFrom, LandsideLaneNodeInSim* pTo, CPath2008& path ) const
{
	for (int i=0;i<m_vLinks.getCount();i++)
	{
		LandsideAboutLaneLink* plink = m_vLinks.getItem(i);
		if(plink->getFromNode() == pFrom 
			&& plink->getToNode() == pTo)
		{
			path  = plink->m_path;
			return true;
		}
	}
	return false;
}

LandsideLayoutObjectInSim* LandsideRoundaboutInSim::getLayoutObject() const
{
	return const_cast<LandsideRoundaboutInSim*>(this);
}

//LandsideVehicleInSim* LandsideRoundaboutInSim::CheckSpaceConflict( LandsideVehicleInSim* pVeh, const ARCPolygon& poly )
//{
//	for(int i=0;i<GetInResVehicleCount();i++)
//	{
//		LandsideVehicleInSim* pOtherV = GetInResVehicle(i);
//		if(pOtherV==pVeh)
//			continue;
//
//		if( !pVeh->bCanWaitFor(pOtherV) )
//			continue;
//
//		if( poly.IsOverlapWithOtherPolygon(pOtherV->getBBox(pVeh->curTime()) ) )
//			return pOtherV;
//	}
//	return NULL;
//}

LandsideVehicleInSim* LandsideRoundaboutInSim::CheckPathConflict( LandsideVehicleInSim* pVeh, const CPath2008& path , DistanceUnit& minDist)
{
	LandsideVehicleInSim* pConflict = NULL;
	for(int i=0;i<GetInResVehicleCount();i++)
	{
		LandsideVehicleInSim* pOtherV = GetInResVehicle(i);
		if(pOtherV==pVeh)
			continue;
			
		if( !pVeh->bCanWaitFor(pOtherV) )
			continue;

		IntersectPathPath2D intersect;
		double dWidth = (pVeh->GetWidth()+pOtherV->GetWidth())*0.5;
		if(intersect.Intersects(path, pOtherV->getSpanPath(),dWidth)>0)
		{
			double dIntserctIndex = intersect.m_vIntersectPtIndexInPath1.front();
			double dOtherInterIndex = intersect.m_vIntersectPtIndexInPath2.front();

			double dist = path.GetIndexDist(dIntserctIndex);
			double dOtherDist = path.GetIndexDist(dOtherInterIndex);
			if(dist < dOtherDist)
				continue;

			if(!pConflict)
			{
				minDist = dist;
				pConflict = pOtherV;
			}
			else if(minDist > dist)
			{
				minDist = dist;
				pConflict = pOtherV;						
			}					
		}
	}
	return pConflict;
}
