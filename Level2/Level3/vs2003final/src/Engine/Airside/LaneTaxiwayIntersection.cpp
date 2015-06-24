#include "StdAfx.h"
#include "./LaneTaxiwayIntersection.h"
#include "./taxiwayresource.h"
#include "./Common/DistanceLineLine.h"

/************************************************************************/
/*         LaneTaxiwayIntersectionInSim                                 */
/************************************************************************/
const static DistanceUnit InvalidDist = -ARCMath::DISTANCE_INFINITE;

LaneTaxiwayIntersectionInSim::LaneTaxiwayIntersectionInSim(  VehicleLaneInSim * plane ,TaxiwaySegInSim * pTaxiSeg ,  int idx)
{
	m_pLane = plane; 
	m_pTaxiSeg = pTaxiSeg;	
	m_idx = idx;
	m_holdDist1 = InvalidDist;
	m_holdDist2 = InvalidDist;
	m_intersectDistAtLane = 0.0;
	m_intersectDistAtTaxiway = 0.0;

}


void LaneTaxiwayIntersectionInSim::SetIntersectDistAtLane( const DistanceUnit& dist )
{
	m_intersectDistAtLane = dist;
	CPoint2008 pt = m_pLane->GetPath().GetDistPoint(dist);
	CPath2008 taxisegpath  = m_pTaxiSeg->GetPositiveDirSeg()->GetPath();
	DistancePointPath3D ptpath(pt,taxisegpath);
	ptpath.GetSquared();
	DistanceUnit distInSeg = taxisegpath.GetRealDist((float)ptpath.m_fPathParameter);
	SetIntersectDistAtTaxiwaySeg(distInSeg);
}


//AirsideFlightInSim * LaneTaxiwayIntersectionInSim::GetLatestFlightInSight( const DistanceUnit& radius,const ElapsedTime&tTime )const
//{
//	CPoint2008 pos = GetPosition();
//	ElapsedTime maxTime = ElapsedTime(0L);
//	AirsideFlightInSim * latestFlight = NULL;	
//	ARCVector2 veldir;
//	veldir = m_pLane->GetPath().GetDirection( m_pLane->GetPath().GetRelateDist(m_intersectDistAtLane) );
//	{//check dirseg1
//		TaxiwayDirectSegInSim * pPosSeg = m_pTaxiSeg->GetPositiveDirSeg();
//
//		std::set<ARCMobileElement*> vMobiles = pPosSeg->getPlanMobiles();
//		for(std::set<ARCMobileElement*>::iterator itr =vMobiles.begin();itr!=vMobiles.end();++itr)
//		{
//			ARCMobileElement* pMob = *itr;
//			if(pMob && pMob->IsKindof( typeof(AirsideFlightInSim) ) )
//			{
//				AirsideFlightInSim * pFlight = (AirsideFlightInSim*)pMob;
//				DistanceUnit dist;
//				DistanceUnit dHalfFltLen = pFlight->GetLength()*0.5;
//				ARCVector2 vfltdir;
//				if( GetFlightDistToHere(pFlight, tTime, dist,vfltdir) )
//				{		
//					//bool bdirConflict = (abs(veldir.GetCosOfTwoVector(vfltdir)) > 0.1;
//					if(dist > -radius-dHalfFltLen && dist < pFlight->GetIntersectionBuffer()+ dHalfFltLen )
//					{
//						return pFlight;
//					}
//				}
//			}
//		}
//	}
//
//	{//check dirseg2
//		TaxiwayDirectSegInSim * pNegSeg = m_pTaxiSeg->GetNegativeDirSeg();
//		std::set<ARCMobileElement*> vMobiles = pNegSeg->getPlanMobiles();
//		for(std::set<ARCMobileElement*>::iterator itr =vMobiles.begin();itr!=vMobiles.end();++itr)
//		{
//			ARCMobileElement* pMob = *itr;
//			if(pMob && pMob->IsKindof( typeof(AirsideFlightInSim) ) )
//			{
//				AirsideFlightInSim * pFlight = (AirsideFlightInSim*)pMob;
//				DistanceUnit dist;
//				DistanceUnit dHalfFltLen = pFlight->GetLength()*0.5;
//				ARCVector2 vfltdir;
//				if( GetFlightDistToHere(pFlight, tTime, dist, vfltdir) )
//				{			
//					//bool bdirConflict = abs(veldir.GetCosOfTwoVector(vfltdir)) > 0.1;
//					if(dist > -radius-dHalfFltLen && dist < pFlight->GetIntersectionBuffer()+dHalfFltLen  )
//					{
//						return pFlight;
//					}
//				}
//			}
//		}
//	}
//	return NULL;
//}
//
CPoint2008 LaneTaxiwayIntersectionInSim::GetPosition() const
{
	ASSERT(m_pTaxiSeg);
	return m_pTaxiSeg->GetPositiveDirSeg()->GetPath().GetDistPoint(m_intersectDistAtTaxiway);
}

#include "TaxiRouteInSim.h"
//bool LaneTaxiwayIntersectionInSim::GetFlightDistToHere( AirsideFlightInSim* pFlight, const ElapsedTime& t, double& dist, ARCVector2& dir )const
//{
//	TaxiRouteInSim * pTaxiRoute = pFlight->GetCurrentTaxiRoute();
//	TaxiwaySegInSim * pTaxiSeg = GetTaxiwaySegment();
//	if(pTaxiRoute && pTaxiSeg)
//	{	
//
//		const AirsideFlightState& fltPreState = pFlight->GetPreState();
//		const AirsideFlightState& fltCurState = pFlight->GetCurState(); 
//
//		int preIdx = pTaxiRoute->GetItemIndex(fltPreState.m_pResource);
//		int nextIdx = pTaxiRoute->GetItemIndex(fltCurState.m_pResource);
//		if(preIdx>=0 && nextIdx>=0)
//		{
//			DistanceUnit flightDistInRoutePre = pTaxiRoute->GetDistInRoute(preIdx, fltPreState.m_dist);
//			DistanceUnit flightDistInRouteNext = pTaxiRoute->GetDistInRoute(nextIdx,fltCurState.m_dist);
//
//			{//check positive seg
//				TaxiwayDirectSegInSim * pDirSeg = pTaxiSeg->GetPositiveDirSeg();
//				int idxIntersect = pTaxiRoute->GetItemIndex(pDirSeg);
//				if( idxIntersect >=0 )
//				{
//					DistanceUnit intersectionDistInRoute = pTaxiRoute->GetDistInRoute(idxIntersect, GetIntersectDistAtTaxiwaySeg() );
//					DistanceUnit fltDist;
//					if( t>= fltPreState.m_tTime && t < fltCurState.m_tTime )
//					{
//						double dRate = (t-fltPreState.m_tTime)/(fltCurState.m_tTime-fltPreState.m_tTime);
//						fltDist = flightDistInRoutePre*(1-dRate) + flightDistInRouteNext*dRate;						
//					}
//					else if(t<fltPreState.m_tTime)
//					{
//						fltDist = flightDistInRoutePre;						
//					}
//					else
//					{
//						fltDist = flightDistInRouteNext;						
//					}
//					dist = fltDist - intersectionDistInRoute;
//					dir = pDirSeg->GetPath().GetDirection( pDirSeg->GetPath().GetRelateDist(fltDist) );
//					return true;					
//				}
//			}
//			{//check negative seg
//				TaxiwayDirectSegInSim * pDirSeg2 = pTaxiSeg->GetNegativeDirSeg();				
//				int idxIntersect = pTaxiRoute->GetItemIndex(pDirSeg2);
//				if( idxIntersect >=0 )
//				{
//					DistanceUnit intersectionDistInRoute = pTaxiRoute->GetDistInRoute(idxIntersect, pTaxiSeg->GetSegmentLength() - GetIntersectDistAtTaxiwaySeg() );						
//					DistanceUnit fltDist;
//					if( t>= fltPreState.m_tTime && t < fltCurState.m_tTime )
//					{
//						double dRate = (t-fltPreState.m_tTime)/(fltCurState.m_tTime-fltPreState.m_tTime);
//						fltDist = flightDistInRoutePre*(1-dRate) + flightDistInRouteNext*dRate;						
//					}
//					else if(t<fltPreState.m_tTime)
//					{
//						fltDist = flightDistInRoutePre;						
//					}
//					else
//					{
//						fltDist = flightDistInRouteNext;						
//					}
//					dist = fltDist - intersectionDistInRoute;
//					dir = pDirSeg2->GetPath().GetDirection( pDirSeg2->GetPath().GetRelateDist(fltDist) );
//					return true;
//				}
//			}
//
//		}
//
//
//	}
//
//	return false;
//}
//
bool LaneTaxiwayIntersectionInSim::MakeValid()
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

bool LaneTaxiwayIntersectionInSim::IsLockedfor( ARCMobileElement* pMob ) const
{
	if(mvLockMobs.find(pMob)!=mvLockMobs.end() && mvLockMobs.size()==1)
		return false;
	return mvLockMobs.size()!=0;
}

#include "./AirsideflightInSim.h"
void LaneTaxiwayIntersectionInSim::OnFlightEnter( AirsideFlightInSim* pFlt,const ElapsedTime& t )
{
	mvLockMobs.insert(pFlt); OnMobileEnter(t);
}

void LaneTaxiwayIntersectionInSim::OnFlightExit( AirsideFlightInSim* pFlt,const ElapsedTime& t )
{
	mvLockMobs.erase(pFlt); OnMobielExit(t);
}
