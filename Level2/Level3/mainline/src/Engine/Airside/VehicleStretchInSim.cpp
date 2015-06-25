#include "stdafx.h"
#include "./Common/ARCPipe.h"
#include "TaxiwayResource.h"
#include "./Common/DistanceLineLine.h"
#include "AirsideFlightInSim.h"
#include "./AirsideVehicleInSim.h"
#include ".\vehiclestretchinsim.h"
#include "./VehicleRoadIntersectionInSim.h"
#include "..\..\Results\AirsideflightEventLog.h"
#include "VehicleStretchLaneInSim.h"
#include "FlightRouteSegInSim.h"

CPoint2008 VehicleLaneExit::GetPosition() const
{
	return m_pLane->GetDistancePoint(m_atDist);
}

CPoint2008 VehicleLaneEntry::GetPosition() const
{
	return m_pLane->GetDistancePoint(m_atDist);
}
/************************************************************************/
/*         LaneFltGroundRouteIntersectionInSim                                 */
/************************************************************************/
const static DistanceUnit InvalidDist = -ARCMath::DISTANCE_INFINITE;

LaneFltGroundRouteIntersectionInSim::LaneFltGroundRouteIntersectionInSim( VehicleLaneInSim * plane ,FlightGroundRouteSegInSim * pFltRouteSeg ,  int idx)
:m_pLane(plane) 
,m_pFltRouteSeg(pFltRouteSeg)	
,m_idx(idx)
,m_holdDist1(InvalidDist)
,m_holdDist2(InvalidDist)
,m_intersectDistAtLane(0.0)
,m_intersectDistAtTaxiway(0.0)
{


}

LaneFltGroundRouteIntersectionInSim::LaneFltGroundRouteIntersectionInSim()
{
	m_pLane = NULL; 
	m_pFltRouteSeg = NULL;
	m_idx = -1;
	m_holdDist1 = InvalidDist;
	m_holdDist2 = InvalidDist;
	m_intersectDistAtLane = 0.0;
	m_intersectDistAtTaxiway = 0.0;
}
void LaneFltGroundRouteIntersectionInSim::SetIntersectDistAtLane( const DistanceUnit& dist )
{
	m_intersectDistAtLane = dist;
	CPoint2008 pt = m_pLane->GetPath().GetDistPoint(dist);
	CPath2008 taxisegpath  = m_pFltRouteSeg->GetPositiveDirSeg()->GetPath();
	DistancePointPath3D ptpath;
	ptpath.GetSquared(pt,taxisegpath);
	DistanceUnit distInSeg = taxisegpath.GetIndexDist(ptpath.m_fPathParameter);
	SetIntersectDistAtTaxiwaySeg(distInSeg);
}


AirsideFlightInSim * LaneFltGroundRouteIntersectionInSim::GetLatestFlightInSight( const DistanceUnit& radius,const ElapsedTime&tTime )const
{
	CPoint2008 pos = GetPosition();
	ElapsedTime maxTime = ElapsedTime(0L);
	AirsideFlightInSim * latestFlight = NULL;	
	ARCVector2 veldir;
	veldir = m_pLane->GetPath().GetDistDir( m_intersectDistAtLane );
	{//check dirseg1
		FlightGroundRouteDirectSegInSim * pPosSeg = m_pFltRouteSeg->GetPositiveDirSeg();
		
		std::set<ARCMobileElement*> vMobiles = pPosSeg->getPlanMobiles();
		for(std::set<ARCMobileElement*>::iterator itr =vMobiles.begin();itr!=vMobiles.end();++itr)
		{
			ARCMobileElement* pMob = *itr;
			if(pMob && pMob->IsKindof( typeof(AirsideFlightInSim) ) )
			{
				AirsideFlightInSim * pFlight = (AirsideFlightInSim*)pMob;
				DistanceUnit dist;
				DistanceUnit dHalfFltLen = pFlight->GetLength()*0.5;
				ARCVector2 vfltdir;
				if( GetFlightDistToHere(pFlight, tTime, dist,vfltdir) )
				{		
					//bool bdirConflict = (abs(veldir.GetCosOfTwoVector(vfltdir)) > 0.1;
					if(dist > -radius-dHalfFltLen && dist < pFlight->GetIntersectionBuffer()+ dHalfFltLen )
					{
						return pFlight;
					}
				}
			}
		}
	}

	{//check dirseg2
		FlightGroundRouteDirectSegInSim * pNegSeg = m_pFltRouteSeg->GetNegativeDirSeg();
		std::set<ARCMobileElement*> vMobiles = pNegSeg->getPlanMobiles();
		for(std::set<ARCMobileElement*>::iterator itr =vMobiles.begin();itr!=vMobiles.end();++itr)
		{
			ARCMobileElement* pMob = *itr;
			if(pMob && pMob->IsKindof( typeof(AirsideFlightInSim) ) )
			{
				AirsideFlightInSim * pFlight = (AirsideFlightInSim*)pMob;
				DistanceUnit dist;
				DistanceUnit dHalfFltLen = pFlight->GetLength()*0.5;
				ARCVector2 vfltdir;
				if( GetFlightDistToHere(pFlight, tTime, dist, vfltdir) )
				{			
					//bool bdirConflict = abs(veldir.GetCosOfTwoVector(vfltdir)) > 0.1;
					if(dist > -radius-dHalfFltLen && dist < pFlight->GetIntersectionBuffer()+dHalfFltLen  )
					{
						return pFlight;
					}
				}
			}
		}
	}
	return NULL;
}

CPoint2008 LaneFltGroundRouteIntersectionInSim::GetPosition() const
{
	ASSERT(m_pFltRouteSeg);
	return m_pFltRouteSeg->GetPositiveDirSeg()->GetPath().GetDistPoint(m_intersectDistAtTaxiway);
}

#include "TaxiRouteInSim.h"
bool LaneFltGroundRouteIntersectionInSim::GetFlightDistToHere( AirsideFlightInSim* pFlight, const ElapsedTime& t, double& dist, ARCVector2& dir )const
{
	TaxiRouteInSim * pTaxiRoute = pFlight->GetCurrentTaxiRoute();
	FlightGroundRouteSegInSim * pTaxiSeg = GetFltRouteSegment();
	if(pTaxiRoute && pTaxiSeg)
	{	

		const AirsideFlightState& fltPreState = pFlight->GetPreState();
		const AirsideFlightState& fltCurState = pFlight->GetCurState(); 

		int preIdx = pTaxiRoute->GetItemIndex(fltPreState.m_pResource);
		int nextIdx = pTaxiRoute->GetItemIndex(fltCurState.m_pResource);
		if(preIdx>=0 && nextIdx>=0)
		{
			DistanceUnit flightDistInRoutePre = pTaxiRoute->GetDistInRoute(preIdx, fltPreState.m_dist);
			DistanceUnit flightDistInRouteNext = pTaxiRoute->GetDistInRoute(nextIdx,fltCurState.m_dist);
			
			{//check positive seg
				FlightGroundRouteDirectSegInSim * pDirSeg = pTaxiSeg->GetPositiveDirSeg();
				int idxIntersect = pTaxiRoute->GetItemIndex(pDirSeg);
				if( idxIntersect >=0 )
				{
					DistanceUnit intersectionDistInRoute = pTaxiRoute->GetDistInRoute(idxIntersect, GetIntersectDistAtTaxiwaySeg() );
					DistanceUnit fltDist;
					if( t>= fltPreState.m_tTime && t < fltCurState.m_tTime )
					{
						double dRate = (t-fltPreState.m_tTime)/(fltCurState.m_tTime-fltPreState.m_tTime);
						fltDist = flightDistInRoutePre*(1-dRate) + flightDistInRouteNext*dRate;						
					}
					else if(t<fltPreState.m_tTime)
					{
						fltDist = flightDistInRoutePre;						
					}
					else
					{
						fltDist = flightDistInRouteNext;						
					}
					dist = fltDist - intersectionDistInRoute;
					dir = pDirSeg->GetPath().GetDistDir( fltDist );
					return true;					
				}
			}
			{//check negative seg
				FlightGroundRouteDirectSegInSim * pDirSeg2 = pTaxiSeg->GetNegativeDirSeg();				
				int idxIntersect = pTaxiRoute->GetItemIndex(pDirSeg2);
				if( idxIntersect >=0 )
				{
					DistanceUnit intersectionDistInRoute = pTaxiRoute->GetDistInRoute(idxIntersect, pTaxiSeg->GetNodesDistance()  - GetIntersectDistAtTaxiwaySeg() );						
					DistanceUnit fltDist;
					if( t>= fltPreState.m_tTime && t < fltCurState.m_tTime )
					{
						double dRate = (t-fltPreState.m_tTime)/(fltCurState.m_tTime-fltPreState.m_tTime);
						fltDist = flightDistInRoutePre*(1-dRate) + flightDistInRouteNext*dRate;						
					}
					else if(t<fltPreState.m_tTime)
					{
						fltDist = flightDistInRoutePre;						
					}
					else
					{
						fltDist = flightDistInRouteNext;						
					}
					dist = fltDist - intersectionDistInRoute;
					dir = pDirSeg2->GetPath().GetDistDir( fltDist );
					return true;
				}
			}

		}

	
	}
	
	return false;
}

bool LaneFltGroundRouteIntersectionInSim::MakeValid()
{
	if( m_holdDist1== InvalidDist && m_holdDist2 == InvalidDist )
		return false;
	
	if(m_holdDist1 == InvalidDist && m_holdDist2!= InvalidDist )
	{
		m_holdDist1 = m_intersectDistAtLane - abs(m_holdDist2 - m_intersectDistAtLane);
	}

	else if( m_holdDist1!= InvalidDist && m_holdDist2 == InvalidDist )
	{
		m_holdDist2 = m_intersectDistAtLane + abs(m_intersectDistAtLane - m_holdDist1);
	}
	return true;
}

bool LaneFltGroundRouteIntersectionInSim::IsLockedfor( ARCMobileElement* pMob ) const
{
	if(mvLockMobs.find(pMob)!=mvLockMobs.end() && mvLockMobs.size()==1)
		return false;
	return mvLockMobs.size()!=0;
}

void LaneFltGroundRouteIntersectionInSim::OnFlightEnter( AirsideFlightInSim* pFlt,const ElapsedTime& t )
{
	mvLockMobs.insert(pFlt); OnMobileEnter(t);
}

void LaneFltGroundRouteIntersectionInSim::OnFlightExit( AirsideFlightInSim* pFlt,const ElapsedTime& t )
{
	mvLockMobs.erase(pFlt); OnMobielExit(t);
}

bool LaneFltGroundRouteIntersectionInSim::bMobileInResource( ARCMobileElement* pmob ) const
{
	return mvLockMobs.find(pmob)!=mvLockMobs.end();
}

AirsideResource::ResourceType LaneFltGroundRouteIntersectionInSim::GetType() const
{
	return AirsideResource::ResType_LaneTaxiwayIntersection;
}

CString LaneFltGroundRouteIntersectionInSim::GetTypeName() const
{
	return _T("Lane Taxiway Intersection");
}

CString LaneFltGroundRouteIntersectionInSim::PrintResource() const
{
	CString strName;
	strName.Format("%s&%s", m_pLane->PrintResource(),"");

	return strName;
}

CPoint2008 LaneFltGroundRouteIntersectionInSim::GetDistancePoint( double dist ) const
{
	return GetPosition();
}

void LaneFltGroundRouteIntersectionInSim::getDesc( ResourceDesc& resDesc )
{
	resDesc.resid = m_idx;
	resDesc.resType = GetType();
	resDesc.strRes = PrintResource();
}
//////////////////////////////////////////////////////////////////////
class LaneTaxiwayIntersectionInSimCompare
{
public:
	bool operator() (const LaneFltGroundRouteIntersectionInSim::RefPtr IntA, const LaneFltGroundRouteIntersectionInSim::RefPtr IntB)
	{
		return IntA.get()->GetIntersectDistAtLane() < IntB.get()->GetIntersectDistAtLane();

	}
};


/************************************************************************/
/*     VehicleLaneInSim                                                 */
/************************************************************************/
VehicleLaneInSim::VehicleLaneInSim(RouteResource* pParRoute, int laneNo)
:m_pParRoute(pParRoute)
,m_nLaneNo(laneNo)
,m_pBeginEntry(NULL)
,m_pEndExit(NULL)
{

}

VehicleLaneInSim::~VehicleLaneInSim()
{
	for(int i=0;i< (int)m_vEntrys.size();i++)
	{
		delete m_vEntrys.at(i);
	}
	for(int i=0;i<(int)m_vExits.size();i++)
	{
		delete m_vExits.at(i);
	}
}

CPoint2008 VehicleLaneInSim::GetDistancePoint( double dist ) const
{
	return GetPath().GetDistPoint(dist);
}

int VehicleLaneInSim::GetFirstTaxiwayIntersection( DistanceUnit dist ) const
{
	for(int i=0; i < GetTaxiwayIntersectionCount(); i++)
	{
		const LaneFltGroundRouteIntersectionInSim* intersect = FltGroundRouteIntersectionAt(i);
		if(intersect->GetHold1() > dist )
		{
			 return i;
		}
	}
	return -1;
}

LaneFltGroundRouteIntersectionInSim* VehicleLaneInSim::GetFltGroundRouteIntersection( FlightGroundRouteSegInSim * pSeg, int idx )
{
	for(int i=0 ; i<(int) m_vTaxiwayIntersections.size();i++ )
	{
		LaneFltGroundRouteIntersectionInSim* item = m_vTaxiwayIntersections[i].get();
		if( item->m_pFltRouteSeg == pSeg && item->m_idx == idx )
		{
			return item;
		}
	}

	LaneFltGroundRouteIntersectionInSim* pNode = new LaneFltGroundRouteIntersectionInSim(this, pSeg,idx);
	m_vTaxiwayIntersections.push_back(pNode);

	return pNode;

}

struct MobileElementDistLess
{
	bool operator()(AirsideVehicleInSim* e1, AirsideVehicleInSim* e2)const
	{	
		return e1->GetDistInResource() < e2->GetDistInResource();
	}
};

AirsideVehicleInSim * VehicleLaneInSim::GetVehicleAhead( DistanceUnit dist )
{
	std::vector<CAirsideMobileElement* > vMobileElment = GetInResouceMobileElement();

	std::vector<AirsideVehicleInSim*> vSortedVehicle;
	for(int i=0;i<(int)vMobileElment.size();i++)
	{
		CAirsideMobileElement * pMobile = vMobileElment.at(i);
		if(pMobile && pMobile->GetType() == CAirsideMobileElement::AIRSIDE_VEHICLE)
		{
			vSortedVehicle.push_back( (AirsideVehicleInSim*)pMobile );
		}
	}
	std::sort(vSortedVehicle.begin(),vSortedVehicle.end(),MobileElementDistLess());
	
	for(int i=0;i<(int)vSortedVehicle.size();i++)
	{
		AirsideVehicleInSim * pVehicle = vSortedVehicle.at(i);
		if(pVehicle->GetDistInResource() > dist) 
			return pVehicle;
	}

	return NULL;
}


//
void VehicleLaneInSim::SortFltGroundRouteIntersections()
{
	std::vector<LaneFltGroundRouteIntersectionInSim::RefPtr>::iterator iter = m_vTaxiwayIntersections.begin();
	std::vector<LaneFltGroundRouteIntersectionInSim::RefPtr>::iterator iterDel;
	for (; iter!= m_vTaxiwayIntersections.end(); iter++)
	{
		if (!iter->get()->MakeValid())
		{
			iterDel = iter;
			iter--;
			m_vTaxiwayIntersections.erase(iterDel);

		}
	}
	//std::vector<LaneFltGroundRouteIntersectionInSim*> validIntersections;
	//validIntersections.reserve(m_vTaxiwayIntersections.size());
	//int nCount = (int)m_vTaxiwayIntersections.size();
	//for(int i=0; i<nCount; i++)
	//{
	//	if( m_vTaxiwayIntersections[i].MakeValid() )
	//		validIntersections.push_back(m_vTaxiwayIntersections[i]);
	//}
	//m_vTaxiwayIntersections.clear();
	//m_vTaxiwayIntersections.assign(validIntersections.begin(),validIntersections.end());
	std::sort(m_vTaxiwayIntersections.begin(),m_vTaxiwayIntersections.end(),LaneTaxiwayIntersectionInSimCompare() );
}

AirsideFlightInSim * VehicleLaneInSim::GetConflictFlightArroundIntersection( const LaneFltGroundRouteIntersectionInSim& curIntersection ,const DistanceUnit& radius,const ElapsedTime&tTime)
{
	AirsideFlightInSim *pLasteFlight = NULL;
	//LaneFltGroundRouteIntersectionInSim curIntersection = _curIntersection;
	//check the current intersection node, whether any flight moving in the taxiway route
	pLasteFlight = curIntersection.GetLatestFlightInSight(radius,tTime);
	if (pLasteFlight != NULL)
		return pLasteFlight;

	int nCount = GetTaxiwayIntersectionCount();
	int nCurIndex = 0;
	for (nCurIndex =0; nCurIndex < nCount; ++nCurIndex )
	{
		if (FltGroundRouteIntersectionAt(nCurIndex)->m_intersectDistAtLane == curIntersection.GetIntersectDistAtLane())
			break;
	}

	//if can pass this intersection, Find the closest intersection ahead, 
	while ((nCurIndex = FindConflictIntersectionAhead(nCurIndex)) > 0)
	{
		pLasteFlight = FltGroundRouteIntersectionAt(nCurIndex)->GetLatestFlightInSight(radius,tTime);
		if (pLasteFlight != NULL)
			break;
	}

	return pLasteFlight;
}

int VehicleLaneInSim::FindConflictIntersectionAhead( int nCurIndex )
{
	if ( nCurIndex + 1 == (int)m_vTaxiwayIntersections.size())//the last one
		return -1;

	if (FltGroundRouteIntersectionAt(nCurIndex)->GetHold2() >= FltGroundRouteIntersectionAt(nCurIndex+1)->GetHold1() )//conflict
		return nCurIndex +1;

	

	return -1;
}

VehicleLaneExit* VehicleLaneInSim::GetExit( int idx ) const
{
	if(idx >=0 && idx < GetExitCount() )
		return m_vExits.at(idx);
	return NULL;
}	

VehicleLaneEntry * VehicleLaneInSim::GetEntry( int idx )
{
	if(idx>=0 && idx<GetEntryCount())
		return m_vEntrys.at(idx);
	return NULL;
}

RouteResource* VehicleLaneInSim::getRouteResource() const
{
	return m_pParRoute;
}

LaneFltGroundRouteIntersectionInSim* VehicleLaneInSim::FltGroundRouteIntersectionAt( int idx )
{
	if (idx <0 || idx >= (int)m_vTaxiwayIntersections.size())
		return NULL;

	return m_vTaxiwayIntersections[idx].get();
}

const LaneFltGroundRouteIntersectionInSim* VehicleLaneInSim::FltGroundRouteIntersectionAt( int idx ) const
{
	if (idx <0 || idx >= (int)m_vTaxiwayIntersections.size())
		return NULL;

	return m_vTaxiwayIntersections[idx].get();
}

VehicleLaneExit* VehicleLaneInSim::GetFirstExit()
{
	int nCount = m_vExits.size();
	VehicleLaneExit* pFirstExit = NULL;
	for (int i =0; i < nCount; i++)
	{
		VehicleLaneExit* pExit = m_vExits.at(i);

		if (pFirstExit && pFirstExit->GetDistInLane() > pExit->GetDistInLane())
			pFirstExit = pExit;

		if (pFirstExit== NULL)
			pFirstExit = pExit;
	}
	return pFirstExit;
}
/************************************************************************/
/* VehicleStretchSegmentInSim                                           */
/************************************************************************/

VehicleStretchSegmentInSim::VehicleStretchSegmentInSim( VehicleStretchInSim*pStretch, DistanceUnit distF, DistanceUnit distT )
{
	m_dDistFromAtStretch = distF;
	m_dDistToAtStretch = distT;
	m_pStretchInSim = pStretch;		
}

VehicleStretchSegmentInSim::~VehicleStretchSegmentInSim()
{
	int nCount = m_vLanes.size();
	for (int i =0; i <nCount; i++)
	{
		VehicleLaneInSim* pLane = m_vLanes.at(i);

		delete pLane;
		pLane = NULL;
	}
	m_vLanes.clear();
}

VehicleLaneInSim * VehicleStretchSegmentInSim::GetLane( int idx ) const
{
	if( idx >=0 && idx < (int) m_vLanes.size() )
		return (VehicleLaneInSim*)m_vLanes.at(idx);
	return NULL;
}

void VehicleStretchSegmentInSim::InitLanes()
{
	//init Lanes
	Stretch * pStretchInput = m_pStretchInSim->GetInput();
	m_path = GetSubPath( pStretchInput->getPath(), m_dDistFromAtStretch, m_dDistToAtStretch );

	ARCPipe pipePath(m_path,pStretchInput->GetLaneNumber()* pStretchInput->GetLaneWidth());

	for(int i=0; i< pStretchInput->GetLaneNumber();i++)
	{
		VehicleLaneInSim * newlane = new VehicleStretchLaneInSim(this, i);
		VehicleRoadIntersectionInSim * pOrigNode = NULL;
		VehicleRoadIntersectionInSim * pDestNode = NULL;
		CPath2008 lanepath;		
		{ // get lane path
			int nptCnt = (int)pipePath.m_sidePath1.size();
			lanepath.init(nptCnt);
			for(int j=0;j<nptCnt;j++)
			{
				double t = (double(i)  +0.5)/pStretchInput->GetLaneNumber();
				ARCPoint3 pt = pipePath.m_sidePath1[j] *(1-t) + pipePath.m_sidePath2[j] * t;
				lanepath[j] = CPoint2008(pt[VX],pt[VY],pt[VZ]);
			}	

			if(pStretchInput->GetLaneDir(i) == Stretch::NegativeDirection)//reverse   
			{
				std::reverse(&lanepath[0],&lanepath[0] + lanepath.getCount());
				
				pOrigNode = m_pNode2;
				pDestNode = m_pNode1;
			}
			else
			{
				pOrigNode = m_pNode1;
				pDestNode = m_pNode2;
			}
		}		

		newlane->SetPath(lanepath);
		//add entry and exit
		VehicleLaneEntry * pNewEntry = new VehicleLaneEntry(newlane,pOrigNode, 0) ;
		newlane->AddEntry(pNewEntry);
		newlane->SetBeginEntry(pNewEntry);
		if(pOrigNode)
			pOrigNode->AddEntry(pNewEntry);

		VehicleLaneExit * pNewExit = new VehicleLaneExit(newlane, pDestNode, lanepath.GetTotalLength());
		newlane->AddExit( pNewExit );
		newlane->SetEndExit(pNewExit);
		if(pDestNode)
			pDestNode->AddExit( pNewExit );

		m_vLanes.push_back(newlane);
	}


}

static inline bool xor(bool a, bool b)
{
	return (a!=b)?true:false;
}

void VehicleStretchSegmentInSim::InitGraph()
{
	m_vEntryExitPairs.clear();
	//VehicleRouteEntrySet vEntries;
	//VehicleRouteExitSet vExits;

	//from entry to exit, in stretch
	for(int i=0;i< GetLaneCount();i++)
	{
		VehicleLaneInSim* plane = GetLane(i);
		for(int entryidx = 0; entryidx < plane->GetEntryCount(); entryidx ++)
		{
			VehicleLaneEntry * pEntry = plane->GetEntry(entryidx);
			//vEntries.insert(pEntry);

			for(int exitidx = 0 ;exitidx < plane->GetExitCount(); exitidx++ )
			{
				VehicleLaneExit * pExit = plane->GetExit(exitidx);
				//vExits.insert(pExit);

				if( pEntry->GetDistInLane() <=  pExit->GetDistInLane() )
				{
					VehicleRouteNodePairDist newPair(pEntry, pExit, pExit->GetDistInLane() - pEntry->GetDistInLane());
					m_vEntryExitPairs.push_back( newPair );
				}
			}
		}

	}
	//add entry node or exit node 
	if(m_pNode1 == NULL || m_pNode2 == NULL)
	{

		//std::ofstream echoFile ("c:\\findveh1111.log", stdios::app);
		//if(m_pNode1)
		//	echoFile<<"m_pNode1     "<< m_pNode1->GetNodeInput().GetName()<<"\r\n";
		//if(m_pNode2)
		//	echoFile<<"m_pNode2     "<< m_pNode2->GetNodeInput().GetName()<<"\r\n";
		//echoFile.close();


		int nLanCount = static_cast<int>(m_vLanes.size());
		for (int nCurLane= 0; nCurLane < nLanCount; ++nCurLane)
		{
			VehicleLaneInSim *pCurLane = m_vLanes.at(nCurLane);
			if(pCurLane == NULL)
				continue;
			VehicleLaneEntry* pLaneEntry = pCurLane->GetBeginEntry();
			if(pLaneEntry != NULL && pLaneEntry->GetOrignResource() == NULL)
			{
				for (int nNextLane =0; nNextLane < nLanCount; ++nNextLane)
				{
					if(nCurLane == nNextLane)// the same lane, ignore it
						continue;

					VehicleLaneInSim *pNextLane = m_vLanes.at(nNextLane);
					if(pNextLane == NULL)
						continue;

					VehicleLaneExit* pNextLaneExit = pNextLane->GetEndExit();

					if(pNextLaneExit != NULL && pNextLaneExit->GetDestResource() == NULL)
					{
						//add exit to entry
					/*	VehicleLaneEntry * pNewEntry = new VehicleLaneEntry(pCurLane,pNextLane, 0) ;
						pCurLane->SetBeginEntry(pNewEntry);
						pCurLane->AddEntry(pNewEntry);*/
						*pLaneEntry = VehicleLaneEntry(pLaneEntry->GetLane(), pNextLane, pLaneEntry->GetDistInLane() );
						*pNextLaneExit = VehicleLaneExit(pNextLaneExit->GetLane(), pCurLane, pNextLaneExit->GetDistInLane() );
						
						
						//VehicleLaneExit * pNewExit = new VehicleLaneExit(pNextLane, pCurLane, pNextLaneExit->GetDistInLane());
						//pNextLane->AddExit(pNewExit);
						VehicleRouteNodePairDist newPair(pNextLaneExit,pLaneEntry , 10);
						m_vEntryExitPairs.push_back( newPair );
						//std::ofstream echoFile1 ("c:\\findveh1111.log", stdios::app);
						//echoFile1<<"NextRes     "<< pNextLane->PrintResource()<<"      CurRes     "<< pCurLane->PrintResource()<<"\r\n";
						//echoFile1.close();

					}
				}
			}
		}
	}

}

bool VehicleStretchSegmentInSim::IsActiveFor( ARCMobileElement* pmob,const RouteDirPath* pPath ) const
{
	if( pmob->IsA(typeof(AirsideVehicleInSim)) && pPath->IsA(typeof(VehicleLaneInSim)) )
	{
		return pPath->bHaveInPathMoibleExcept(pmob);
	}	
	return false;
}

bool VehicleStretchSegmentInSim::bMayHaveConflict( const RouteDirPath *pPath ) const
{
	return pPath->getPlanMobileCount()>0;
}

bool VehicleStretchSegmentInSim::bLocked( ARCMobileElement* pmob,const RouteDirPath* pPath ) const
{
	return pPath->bHaveInPathMoibleExcept(pmob);
}

bool VehicleStretchSegmentInSim::bMobileInResource( ARCMobileElement* pmob ) const
{
	int nCount = m_vLanes.size();
	for (int i =0; i < nCount; i++)
	{
		if (m_vLanes.at(i)->bMobileInPath(pmob))
			return true;
	}

	return false;
}

DistanceUnit VehicleStretchSegmentInSim::getWidth() const
{
	return m_pStretchInSim->getWidth();
}

/************************************************************************/
/*      VehicleStretchInSim                                             */
/************************************************************************/
class IntersectionNodeInSimCompareDistInObject
{
public:
	IntersectionNodeInSimCompareDistInObject(int nObjID){ m_nObjId = nObjID; }
	bool operator()(VehicleRoadIntersectionInSim* pNode1, VehicleRoadIntersectionInSim* pNode2){
		return pNode1->GetNodeInput().GetDistance(m_nObjId) < pNode2->GetNodeInput().GetDistance(m_nObjId);
	}
protected:
	int m_nObjId;
};

const static DistanceUnit DefaultHoldOffset = 800;

VehicleStretchInSim::VehicleStretchInSim( Stretch* pStretchInput)
{
	m_stretchInput = pStretchInput;	
}

VehicleStretchInSim::~VehicleStretchInSim()
{
	for(int i=0;i<(int)m_vSegments.size();i++)
		delete m_vSegments.at(i);
}



void VehicleStretchInSim::InitSegments( std::vector<VehicleRoadIntersectionInSim*>& NodeList )
{
	std::vector<VehicleRoadIntersectionInSim*> vNodesInSim;
	VehicleRoadIntersectionInSim* pHeadNode = NULL;
	VehicleRoadIntersectionInSim* pTrailNode = NULL;

	for(int i=0;i< (int)NodeList.size();i++)
	{
		VehicleRoadIntersectionInSim* pNodeInSim = NodeList.at(i);
		if( pNodeInSim->IsHeadNodeOfStretch(this))
		{
			pHeadNode = pNodeInSim;
		}
		if( pNodeInSim->IsTrailNodeOfStretch(this) )
		{
			pTrailNode = pNodeInSim;
		}
		if( pNodeInSim->GetNodeInput().HasObject( m_stretchInput->getID() ) )
		{
			vNodesInSim.push_back( pNodeInSim );
		}

		
	}
	std::sort( vNodesInSim.begin(),vNodesInSim.end(), IntersectionNodeInSimCompareDistInObject( m_stretchInput->getID()) );

	

	if(! vNodesInSim.size() )
	{
		DistanceUnit distFrom = 0;
		DistanceUnit distTo = m_stretchInput->getPath().GetTotalLength();		


		
		if( distFrom < distTo )
		{
			VehicleStretchSegmentInSim * newSeg = new VehicleStretchSegmentInSim(this, distFrom, distTo );
			newSeg->SetNode(pHeadNode, pTrailNode);
			newSeg->InitLanes();
			m_vSegments.push_back(newSeg);
		}
		return ;
	}

	if( vNodesInSim.size() )
	{
		DistanceUnit distFrom = 0;
		VehicleRoadIntersectionInSim* pNode2= vNodesInSim.at(0);

		DistanceUnit dMin =0;DistanceUnit dMax = 0;
		pNode2->GetMinMaxHoldDistAtStretch(m_stretchInput->getID(), dMin, dMax);
		if( dMin >=0 )
			dMin = - DefaultHoldOffset;
		DistanceUnit distTo = dMin + pNode2->GetNodeInput().GetDistance(m_stretchInput->getID() ); 
		
		if( distFrom < distTo )
		{
			VehicleStretchSegmentInSim * newSeg = new VehicleStretchSegmentInSim(this, distFrom, distTo );
			newSeg->SetNode(pHeadNode, pNode2);
			newSeg->InitLanes();
			m_vSegments.push_back(newSeg);
		}

	}
	
	for(int i=0;i< (int)vNodesInSim.size()-1;i++)
	{
		VehicleRoadIntersectionInSim * pNode1 = vNodesInSim.at(i);
		VehicleRoadIntersectionInSim * pNode2 = vNodesInSim.at(i+1);

		DistanceUnit dMin =0;DistanceUnit dMax = 0;

					
		pNode1->GetMinMaxHoldDistAtStretch(m_stretchInput->getID() , dMin, dMax);
		
		if( dMax <= 0 )
			dMax = DefaultHoldOffset;

		DistanceUnit distFrom =  dMax + pNode1->GetNodeInput().GetDistance(m_stretchInput->getID());
	
		dMin = 0; dMax = 0;
		
		pNode2->GetMinMaxHoldDistAtStretch(m_stretchInput->getID(), dMin, dMax);
		
		if( dMin >=0 )
			dMin = - DefaultHoldOffset;

		DistanceUnit distTo = dMin + pNode2->GetNodeInput().GetDistance(m_stretchInput->getID() );

		if(distFrom >= distTo )
		{
			DistanceUnit dMid =0.5*(distFrom+distTo);
			distFrom = dMid - 1;
			distTo = dMid +1;
		}
		if(distFrom < distTo )
		{
			VehicleStretchSegmentInSim * newSeg = new VehicleStretchSegmentInSim(this, distFrom, distTo );
			newSeg->SetNode(pNode1, pNode2);
			newSeg->InitLanes();
			m_vSegments.push_back(newSeg);
		}
	}
	
	if( vNodesInSim.size() )
	{
		VehicleRoadIntersectionInSim* pNode1 = *vNodesInSim.rbegin();
		DistanceUnit dMin =0;DistanceUnit dMax = 0;	
		pNode1->GetMinMaxHoldDistAtStretch(m_stretchInput->getID(), dMin, dMax);
		if( dMax <= 0 )
			dMax = DefaultHoldOffset;

		DistanceUnit distFrom =  dMax + pNode1->GetNodeInput().GetDistance(m_stretchInput->getID());		
		DistanceUnit distTo = m_stretchInput->getPath().GetTotalLength();		

		if(distFrom < distTo )
		{
			VehicleStretchSegmentInSim * newSeg = new VehicleStretchSegmentInSim(this, distFrom, distTo );
			newSeg->SetNode(pNode1, pTrailNode);
			newSeg->InitLanes();
			m_vSegments.push_back(newSeg);
		}
	}
	
	

}

VehicleStretchSegmentInSim* VehicleStretchInSim::GetSegment( int idx ) const
{
	if( idx >=0 && idx < (int)m_vSegments.size() )
		return m_vSegments.at(idx);
	return NULL;
}

std::vector<VehicleStretchSegmentInSim*> VehicleStretchInSim::GetSegments( VehicleRoadIntersectionInSim*fromNode, VehicleRoadIntersectionInSim* toNode )
{
	{//
		bool bStartFlag = false;
		bool bEndFlag = false;
		std::vector<VehicleStretchSegmentInSim*>vRet;
		for(int i=0;i<(int)m_vSegments.size();++i)
		{
			VehicleStretchSegmentInSim* pSeg =m_vSegments[i];
			if(pSeg->GetNode1() == fromNode )
			{
				bStartFlag = true;
			}

			if(bStartFlag && !bEndFlag )
				vRet.push_back(pSeg);

			if(pSeg->GetNode2() == toNode)
			{
				bEndFlag = true;
			}

			if(bStartFlag  && bEndFlag )
			{
				return vRet;
			}
		}
	}
	{
		std::vector<VehicleStretchSegmentInSim*>vRet;
		bool bStartFlag = false;
		bool bEndFlag = false;
		for(int i=(int)m_vSegments.size()-1; i>=0 ; --i)
		{
			VehicleStretchSegmentInSim* pSeg =m_vSegments[i];
			if(pSeg->GetNode2() == fromNode )
			{
				bStartFlag = true;
			}

			if(bStartFlag && !bEndFlag )
				vRet.push_back(pSeg);

			if(pSeg->GetNode1() == toNode)
			{
				bEndFlag = true;
			}
			if(bStartFlag  && bEndFlag )
			{
				return vRet;
			}
		}
	}
	return std::vector<VehicleStretchSegmentInSim*>();
}
