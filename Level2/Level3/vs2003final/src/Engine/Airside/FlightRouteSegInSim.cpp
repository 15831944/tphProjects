#include "StdAfx.h"
#include ".\flightrouteseginsim.h"
#include "../../Common/ARCVector.h"
#include ".\AirsideflightInSim.h"
#include "VehicleStretchInSim.h"
#include "VehicleStretchLaneInSim.h"
#include "./Common/ARCPipe.h"
FlightGroundRouteDirectSegInSim::FlightGroundRouteDirectSegInSim(FlightGroundRouteSegInSim *pSeg, FlightGroundRouteSegDirection dir )
{
	m_pSeg = pSeg;
	m_dir = dir;
}

FlightGroundRouteDirectSegInSim::~FlightGroundRouteDirectSegInSim()
{

}


double FlightGroundRouteDirectSegInSim::GetEndDist() const
{
	return m_distance;
}

IntersectionNodeInSim * FlightGroundRouteDirectSegInSim::GetEntryNode()const
{
	ASSERT(m_pSeg);
	if(IsPositiveDir())
		return m_pSeg->GetNode1();
	else
		return m_pSeg->GetNode2();
}

IntersectionNodeInSim * FlightGroundRouteDirectSegInSim::GetExitNode()const
{
	ASSERT(m_pSeg);
	if(IsPositiveDir())
		return m_pSeg->GetNode2();
	else 
		return m_pSeg->GetNode1();
}

CPoint2008 FlightGroundRouteDirectSegInSim::GetDistancePoint( double dist ) const
{
	return m_dirPath.GetDistPointEx(dist);
}

DistanceUnit FlightGroundRouteDirectSegInSim::GetLength() const
{
	return m_distance;
}

DistanceUnit FlightGroundRouteDirectSegInSim::GetPosInTaxiway( DistanceUnit distInSeg ) const
{
	if(IsPositiveDir())
	{
		return  GetEnterPosInTaxiway() + distInSeg;
	}
	else{
		return GetEnterPosInTaxiway() - distInSeg;
	}
}

CPath2008 FlightGroundRouteDirectSegInSim::GetPath() const
{
	return m_dirPath;
}

DistanceUnit FlightGroundRouteDirectSegInSim::GetEnterPosInTaxiway() const
{
	if(IsPositiveDir()) 
		return m_pSeg->GetNode1Pos(); 
	else 
		return m_pSeg->GetNode2Pos();
}

DistanceUnit FlightGroundRouteDirectSegInSim::GetExitPosInTaxiway() const
{
	if(IsPositiveDir())
		return m_pSeg->GetNode2Pos(); 
	else 
		return m_pSeg->GetNode1Pos();
}
double FlightGroundRouteDirectSegInSim::GetExitHoldDist() const
{
	if(m_dir == PositiveDirection)
	{
		return GetEndDist() + min(m_pSeg->GetUserHold2Offset(),m_pSeg->GetHold2Offset());
	}
	else
	{
		return GetEndDist() - max(m_pSeg->GetUserHold1Offset(),m_pSeg->GetHold1Offset());
	}
}

double FlightGroundRouteDirectSegInSim::GetEntryHoldDist() const
{
	if(m_dir == NegativeDirection)
	{
		return -m_pSeg->GetHold2Offset();
	}
	else
	{
		return m_pSeg->GetHold1Offset();
	}
}

FlightGroundRouteSegInSim * FlightGroundRouteDirectSegInSim::GetRouteSegInSim()
{
	return m_pSeg;
}

FlightGroundRouteDirectSegInSim  * FlightGroundRouteDirectSegInSim::GetOppositeSegment()
{
	if(IsPositiveDir()) 
		return (FlightGroundRouteDirectSegInSim *)m_pSeg->GetNegativeDirSeg();
	else 
		return (FlightGroundRouteDirectSegInSim *)m_pSeg->GetPositiveDirSeg();
}

const FlightGroundRouteDirectSegInSim * FlightGroundRouteDirectSegInSim::GetOppositeSegment() const
{
	if(IsPositiveDir())
		return (FlightGroundRouteDirectSegInSim *) m_pSeg->GetNegativeDirSeg();
	else
		return (FlightGroundRouteDirectSegInSim *) m_pSeg->GetPositiveDirSeg();
}
bool FlightGroundRouteDirectSegInSim::IsDirectSegementOnRight(FlightGroundRouteDirectSegInSim* pDirectSegment)
{
	CPoint2008 ptFrom;
	CPoint2008 ptTo;
	CPoint2008 pt;
	if (m_dir == PositiveDirection)
	{
		ptFrom = m_pSeg->GetNode1()->GetNodeInput().GetPosition();
		ptTo = m_pSeg->GetNode2()->GetNodeInput().GetPosition();
	}
	else
	{
		ptTo = m_pSeg->GetNode1()->GetNodeInput().GetPosition();
		ptFrom = m_pSeg->GetNode2()->GetNodeInput().GetPosition();
	}

	if (pDirectSegment->IsPositiveDir())
	{
		pt = pDirectSegment->GetRouteSegInSim()->GetNode1()->GetNodeInput().GetPosition();
	}
	else
	{
		pt = pDirectSegment->GetRouteSegInSim()->GetNode2()->GetNodeInput().GetPosition();
	}

	ARCVector3 vSegDir;
	ARCVector3 vPtDir;
	vSegDir = ptFrom - ptTo;
	vPtDir = pt - ptFrom;


	ARCVector3 vX = vSegDir ^ vPtDir;
	return vX[VZ] > 0;

}

RouteResource* FlightGroundRouteDirectSegInSim::getRouteResource() const
{
	return m_pSeg;
}

int FlightGroundRouteDirectSegInSim::GetEntryHoldID() const
{
	if(m_dir == PositiveDirection)
	{
		return  m_pSeg->GetHold1Id();
	}
	else
	{
		return m_pSeg->GetHold2Id();
	}
}

int FlightGroundRouteDirectSegInSim::GetExitHoldID() const
{
	if(m_dir == PositiveDirection)
	{
		return  m_pSeg->GetHold2Id();
	}
	else
	{
		return m_pSeg->GetHold1Id();
	}
}

int FlightGroundRouteDirectSegInSim::GetInPathAirsideFlightList( std::vector<AirsideFlightInSim* >& vecFlights ) const
{
	int nAddedFlights = 0;
	std::vector<ARCMobileElement*>::const_iterator ite = vInPathMobiles.begin();
	std::vector<ARCMobileElement*>::const_iterator iteEn = vInPathMobiles.end();
	for(;ite!=iteEn;ite++)
	{
		ARCMobileElement* pMobileElement = *ite;
		if (pMobileElement->IsKindof(typeof(AirsideFlightInSim)))
		{
			AirsideFlightInSim* pAirsideFlight = (AirsideFlightInSim*)pMobileElement;
			vecFlights.push_back(pAirsideFlight);
			nAddedFlights++;
		}
	}
	return nAddedFlights;
}
//bool FlightGroundRouteDirectSegInSim::IsFlightLockedResource(AirsideFlightInSim* pFlight)
//{
//	if (std::find(m_vLockFlights.begin(),m_vLockFlights.end(),pFlight) == m_vLockFlights.end())
//		return false;
//
//	return true;
//}
//
//bool FlightGroundRouteDirectSegInSim::IsDirectionActive() const
//{
//	TRACE("FlightGroundRouteDirectSegInSim::IsDirectionActive()");
//	ASSERT(0);
//	return true;
//}
//
//bool FlightGroundRouteDirectSegInSim::TryLockDir( AirsideFlightInSim* pFlight )
//{
//	TRACE("FlightGroundRouteDirectSegInSim::TryLockDir( AirsideFlightInSim* pFlight )");
//	ASSERT(0);
//	return true;
//}
//
//bool FlightGroundRouteDirectSegInSim::GetDirLock( AirsideFlightInSim* pFlight )
//{
//	TRACE("FlightGroundRouteDirectSegInSim::GetDirLock( AirsideFlightInSim* pFlight )");
//	ASSERT(0);
//	return false;
//}
//
//void FlightGroundRouteDirectSegInSim::ReleaseDirLock( AirsideFlightInSim* pFlight, const ElapsedTime& tTime )
//{
//	TRACE("FlightGroundRouteDirectSegInSim::ReleaseDirLock( AirsideFlightInSim* pFlight, const ElapsedTime& tTime )");
//	ASSERT(0);
//}
//
//TaxiwaySegDirectionLock* FlightGroundRouteDirectSegInSim::GetDirectionLock()
//{
//	TRACE("TaxiwaySegDirectionLock* FlightGroundRouteDirectSegInSim::GetDirectionLock()");
//	ASSERT(0);
//	return NULL;
//}
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//////////////////////////////////////////////////////////////////////////
//FlightGroundRouteSegInSim
FlightGroundRouteSegInSim::FlightGroundRouteSegInSim(IntersectionNodeInSim * pNodeFrom , IntersectionNodeInSim * pNodeTo, FlightGroundRouteInSim * pFlightRoute)
{
	m_pNode1 = pNodeFrom;
	m_pNode2 = pNodeTo;
	m_pFlightRouteInSim = pFlightRoute;	

	m_nHoldId1 = m_nHoldId2 = -1;

	m_positiveDirSeg = NULL;
	m_negativeDirSeg = NULL;
	m_dHoldOffsetNode1 = m_dHoldOffsetNode2 = m_dHoldOffsetNode1Ext = m_dHoldOffsetNode2Ext = 0;
}

FlightGroundRouteSegInSim::~FlightGroundRouteSegInSim(void)
{

}

double FlightGroundRouteSegInSim::GetNode1Pos() const
{
	ASSERT(0);
	return 0;

}

double FlightGroundRouteSegInSim::GetNode2Pos() const
{
	ASSERT(0);
	return 0;
}
DistanceUnit FlightGroundRouteSegInSim::GetNodesDistance() const
{
	return GetNode2Pos() - GetNode1Pos();
}

bool FlightGroundRouteSegInSim::CanServeFlight( AirsideFlightInSim *pFlight )
{
	return true;
}

bool FlightGroundRouteSegInSim::IsActiveFor( ARCMobileElement* pmob,const RouteDirPath* pPath ) const
{
	if( pmob->IsA(typeof(AirsideFlightInSim)) && pPath->IsKindof(typeof(FlightGroundRouteDirectSegInSim)) )
	{
		return _IsActiveForFlight((AirsideFlightInSim*)pmob, (const FlightGroundRouteDirectSegInSim*)pPath );
	}	
	return false;
}

bool FlightGroundRouteSegInSim::_IsActiveForFlight( AirsideFlightInSim* pFlight, const FlightGroundRouteDirectSegInSim* pPath ) const
{
	if(GetPositiveDirSeg()!=pPath && GetPositiveDirSeg()->bHaveInPathMoibleExcept(pFlight) )
	{
		return false;
	}
	if(GetNegativeDirSeg()!=pPath && GetNegativeDirSeg()->bHaveInPathMoibleExcept(pFlight) )
	{
		return false;
	}
	//if the next queue item is the flight 
	ARCMobileElement* pMob = pPath->getFirstQueueMobile();
	if(pMob && pMob!= pFlight)
	{
		return false;
	}
	return true;
}

bool FlightGroundRouteSegInSim::bMayHaveConflict( const RouteDirPath *pPath ) const
{
	return GetPositiveDirSeg()->getPlanMobileCount()!=0 && GetNegativeDirSeg()->getPlanMobileCount()!=0;
}

bool FlightGroundRouteSegInSim::bLocked(ARCMobileElement* pmob, const RouteDirPath* pPath ) const
{
	if(pPath && pPath!=GetPositiveDirSeg() )
		return GetPositiveDirSeg()->bHaveInPathMoibleExcept(pmob);
	if(pPath && pPath!= GetNegativeDirSeg())
		return GetNegativeDirSeg()->bHaveInPathMoibleExcept(pmob);
	return false;
}

void FlightGroundRouteSegInSim::InitLanes()
{
	CPath2008 path = m_pFlightRouteInSim->GetPath().GetSubPath(GetNode1Pos(), GetNode2Pos());

	ARCPipe pipePath(path,m_pFlightRouteInSim->getRouteWidth());

	std::vector<LaneFltGroundRouteIntersectionInSim*> vFltRouteNodes;

	int nCount = getIntersectLaneCount();
	for (int i = 0; i < nCount; i++)
	{
		VehicleLaneInSim* pIntLane = m_vIntersectLanes.at(i);

		LaneFltGroundRouteIntersectionInSim* pNode = pIntLane->GetFltGroundRouteIntersection(this,0);

		if (pNode)
			vFltRouteNodes.push_back(pNode);
	}
	
	int nNodeCount = vFltRouteNodes.size();
	for(int i=0; i< 2;i++)
	{
		VehicleLaneInSim * newlane = new VehicleTaxiLaneInSim(this, i);
		IntersectionNodeInSim * pOrigNode = NULL;
		IntersectionNodeInSim* pDestNode = NULL;
		CPath2008 lanepath;		
		{ // get lane path
			int nptCnt = (int)pipePath.m_sidePath1.size();
			lanepath.init(nptCnt);
			for(int j=0;j<nptCnt;j++)
			{
				double t = (double(i)  +0.5)/2;
				ARCPoint3 pt = pipePath.m_sidePath1[j] *(1-t) + pipePath.m_sidePath2[j] * t;
				lanepath[j] = CPoint2008(pt[VX],pt[VY],pt[VZ]);
			}	

			if(i ==1)//reverse   
			{
				std::reverse(&lanepath[0],&lanepath[0] + lanepath.getCount());

				pOrigNode = m_pNode2.get();
				pDestNode = m_pNode1.get();

				for (int idx =0; idx < nNodeCount; idx++)
				{
					LaneFltGroundRouteIntersectionInSim* pLaneNode = vFltRouteNodes.at(idx);
					if (!pLaneNode->MakeValid())
						continue;
					VehicleLaneExit * pNewEntry = new VehicleLaneExit(newlane,pLaneNode,GetNodesDistance()- pLaneNode->GetIntersectDistAtTaxiwaySeg()) ;
					newlane->AddExit(pNewEntry);
				}
			}
			else
			{
				pOrigNode = m_pNode1.get();
				pDestNode = m_pNode2.get();

				for (int idx =0; idx < nNodeCount; idx++)
				{
					LaneFltGroundRouteIntersectionInSim* pLaneNode = vFltRouteNodes.at(idx);
					if (!pLaneNode->MakeValid())
						continue;
					VehicleLaneExit * pNewEntry = new VehicleLaneExit(newlane,pLaneNode, pLaneNode->GetIntersectDistAtTaxiwaySeg()) ;
					newlane->AddExit(pNewEntry);
				}
			}
		}		

		newlane->SetPath(lanepath);
		//add entry and exit
		VehicleLaneEntry * pNewEntry = new VehicleLaneEntry(newlane,pOrigNode, 0) ;
		newlane->AddEntry(pNewEntry);
		newlane->SetBeginEntry(pNewEntry);


		VehicleLaneExit * pNewExit = new VehicleLaneExit(newlane, pDestNode, lanepath.GetTotalLength());
		newlane->AddExit( pNewExit );
		newlane->SetEndExit(pNewExit);




		m_vLanes.push_back(newlane);
	}


}

int FlightGroundRouteSegInSim::GetLaneCount()
{
	return m_vLanes.size();
}

VehicleLaneInSim* FlightGroundRouteSegInSim::GetLane( int idx )
{
	if (idx <0 || idx >= GetLaneCount())
		return NULL;

	return m_vLanes.at(idx);
}

VehicleLaneInSim* FlightGroundRouteSegInSim::GetExitLane( IntersectionNodeInSim* pNode )
{
	int nCount = GetLaneCount();
	for (int i =0; i < nCount; i++)
	{
		VehicleLaneInSim* pLane = m_vLanes.at(i);

		if (pLane->GetEndExit()->GetDestResource() == pNode)
			return pLane;
	}

	return NULL;
}

bool FlightGroundRouteSegInSim::GetExitLane(IntersectionNodeInSim* pNode,std::vector<VehicleLaneInSim*>& vLinkedLane)
{
	int nCount = GetLaneCount();
	for (int i =0; i < nCount; i++)
	{
		VehicleLaneInSim* pLane = m_vLanes.at(i);

		if (pLane->GetEndExit()->GetDestResource() == pNode)
			vLinkedLane.push_back(pLane);
	}

	return true;
}

void FlightGroundRouteSegInSim::addIntersectionLane( VehicleLaneInSim* pLane )
{
	if( std::find(m_vIntersectLanes.begin(),m_vIntersectLanes.end(),pLane)!= m_vIntersectLanes.end() )
		return;

	m_vIntersectLanes.push_back(pLane);
}

bool FlightGroundRouteSegInSim::IsPointInSeg( const CPoint2008& pos )
{
	return m_positiveDirSeg->GetPath().bPointInPath(pos);
}

DistanceUnit FlightGroundRouteSegInSim::getDistInSegAccordToExitNode( const CPoint2008& pos, const IntersectionNodeInSim* pExitNode )
{
	if (m_positiveDirSeg->GetExitNode() == pExitNode)
		return m_positiveDirSeg->GetPath().GetPointDist(pos);

	return m_negativeDirSeg->GetPath().GetPointDist(pos);
}

///
//FlightRouteInSim

FlightGroundRouteInSim::FlightGroundRouteInSim(double dWidth, int nDBId, const CPath2008& path)
:m_dWidth(dWidth)
,m_nDBId(nDBId)
,m_path(path)
{

}

FlightGroundRouteInSim::~FlightGroundRouteInSim()
{

}

void FlightGroundRouteInSim::GetMinMaxFilletDistToNode( IntersectionNodeInSim* pNodeInSim,const int& nObjID, DistanceUnit& dMin, DistanceUnit& dMax )
{
	std::vector<FilletTaxiway>& vFillets = pNodeInSim->GetFilletList();

	for(int i=0;i< (int)vFillets.size();i++)
	{
		FilletTaxiway& theFillet = vFillets[i];
		{
			FilletPoint filetPt;
			if( theFillet.GetObject1ID() == nObjID )
			{
				filetPt = theFillet.GetFilletPoint1();
			}
			if( theFillet.GetObject2ID() == nObjID )
			{
				filetPt = theFillet.GetFilletPoint2();
			}

			DistanceUnit distToNode = filetPt.GetDistToIntersect() + filetPt.GetUsrDist();
			if( distToNode < 0 && distToNode < dMin )
			{
				dMin = distToNode;
			}
			if( distToNode > 0 && distToNode > dMax )
			{
				dMax = distToNode;
			}
		}
	}
}

double FlightGroundRouteInSim::getRouteWidth() const
{
	return m_dWidth;
}

int FlightGroundRouteInSim::GetId() const
{
	return m_nDBId;
}

const CPath2008& FlightGroundRouteInSim::GetPath()
{
	return m_path;
}