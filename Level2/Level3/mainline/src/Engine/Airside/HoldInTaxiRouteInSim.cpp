#include "stdafx.h"
#include "TaxiRouteInSim.h"
#include "HoldInTaxiRouteInSim.h"
#include "AirsideFlightInSim.h"
#include ".\FlightPerformanceManager.h"
#include "..\..\InputAirside\HoldShortLines.h"
#include "./AirTrafficController.h"


void FlightHoldListInTaxiRoute::Init( TaxiRouteInSim& theRoute,AirsideFlightInSim*pFlight,bool bLanding )
{
	if(!pFlight)
	{
		return;
	}
	if(pFlight == m_pFlight )
		return; 

	double dMaxFltWidth = pFlight->mOtherFlightMaxWingSpan*0.5;

	
	{
		m_pFlight = pFlight;
		const DistanceUnit dHalfFltLen = pFlight->GetLength()*0.5;
		const DistanceUnit dNodeBuffer = (theRoute.GetMode()<OnHeldAtStand)?pFlight->GetPerformance()->getTaxiInboundIntersectionBuffer(pFlight):	pFlight->GetPerformance()->getTaxiOutboundIntersectionBuffer(pFlight);
		m_vHoldList.clear();	

		if( theRoute.GetItemCount() < 1)
			return;

		//DistanceUnit AcummDist = 0;
		for(int i=0;i< theRoute.GetItemCount();i++)
		{
			const TaxiRouteItemInSim& theItem = theRoute.ItemAt(i);

			DistanceUnit AcummDist = theRoute.GetItemBeginDist(i);

			FlightGroundRouteDirectSegInSim* pSeg = (FlightGroundRouteDirectSegInSim*) theRoute.ItemAt(i).GetResource();
			FlightGroundRouteDirectSegInSim* pPreSeg = NULL;
			FlightGroundRouteDirectSegInSim* pNextSeg = NULL;
			if(i>0){ 
				pPreSeg = (FlightGroundRouteDirectSegInSim*) theRoute.ItemAt(i-1).GetResource(); 
			}
			if(i<theRoute.GetItemCount()-1){
				pNextSeg = (FlightGroundRouteDirectSegInSim*) theRoute.ItemAt(i+1).GetResource();
			}
			if( i==0 ) //first node entry hold
			{
				HoldInTaxiRoute holdEnterEntryNode( HoldInTaxiRoute::ENTRYNODE, -dHalfFltLen, pSeg->GetEntryNode(), pSeg,pSeg->GetEntryHoldID());
				holdEnterEntryNode.SetLinkDirSeg(pSeg);

				if(bLanding && holdEnterEntryNode.IsRunwayHold() ){ //it is a hold of runway exit node
					holdEnterEntryNode.m_bIsRunwayExit = true;
				}
				m_vHoldList.push_back( holdEnterEntryNode );

			}

			double dAccEndDist= AcummDist+ pSeg->GetEndDist();
			//previous segment exit hold
			bool bIsCrossNode = pSeg->GetEntryNode()->IsNoParking(pFlight);
			DistanceUnit dHoldDist = bIsCrossNode?max(pSeg->GetEntryHoldDist(), dNodeBuffer) : pSeg->GetEntryHoldDist();
			DistanceUnit entryHoldDist = dHoldDist + dHalfFltLen;
			DistanceUnit dHoldDistInRoute = min(AcummDist+entryHoldDist,dAccEndDist);//can not exceed the end dist
			
			HoldInTaxiRoute holdExitEntryNode(HoldInTaxiRoute::EXITNODE_BUFFER, dHoldDistInRoute, pSeg->GetEntryNode(), pSeg ,pSeg->GetEntryHoldID());
			holdExitEntryNode.SetLinkDirSeg(pPreSeg);

			if(i==0 && bLanding && holdExitEntryNode.IsRunwayHold() ){
				holdExitEntryNode.m_bIsRunwayExit = true;				
			}
			m_vHoldList.push_back( holdExitEntryNode );
			

			//add exit node
			entryHoldDist = pSeg->GetEntryHoldDist() + dHalfFltLen;
			dHoldDistInRoute = min(AcummDist+entryHoldDist,dAccEndDist);
			HoldInTaxiRoute holdExitNode(HoldInTaxiRoute::EXITNODE,dHoldDistInRoute,pSeg->GetEntryNode(),pSeg,pSeg->GetEntryHoldID() );
			holdExitNode.SetLinkDirSeg(pPreSeg);		
			if(i==0 && bLanding && holdExitNode.IsRunwayHold() ){
				holdExitNode.m_bIsRunwayExit = true;				
			}
			m_vHoldList.push_back( holdExitNode );



			//next segment entry hold
			//DistanceUnit dHoldDist = min(pSeg->GetExitHoldDist(), pSeg->GetEndDist() - dNodeBuffer);
			//DistanceUnit exitHoldDist = dHoldDist - dHalfFltLen;  exitholdDist is the dist exit the 
			DistanceUnit exitHoldDist = min(pSeg->GetExitHoldDist(),pSeg->GetEndDist()-dMaxFltWidth) - dHalfFltLen;
			HoldInTaxiRoute holdEntryExitNode(HoldInTaxiRoute::ENTRYNODE,exitHoldDist + AcummDist , pSeg->GetExitNode(), pSeg ,pSeg->GetExitHoldID());
			holdEntryExitNode.SetLinkDirSeg(pNextSeg);
			m_vHoldList.push_back( holdEntryExitNode );



			if( i == theRoute.GetItemCount()-1 )
			{
				HoldInTaxiRoute holdExitExitNode(HoldInTaxiRoute::EXITNODE_BUFFER, theRoute.GetEndDist() + dHalfFltLen+ dNodeBuffer, pSeg->GetExitNode(), pSeg,pSeg->GetEntryHoldID() );
				holdExitExitNode.SetLinkDirSeg(pSeg);
				m_vHoldList.push_back( holdExitExitNode);
				HoldInTaxiRoute holdExitNode(HoldInTaxiRoute::EXITNODE,theRoute.GetEndDist()+dHalfFltLen,pSeg->GetExitNode(), pSeg,pSeg->GetEntryHoldID() );
				holdExitNode.SetLinkDirSeg(pSeg);
				m_vHoldList.push_back( holdExitNode );
			}

		}
		std::sort( m_vHoldList.begin(), m_vHoldList.end() );	
	}	
	
}


HoldInTaxiRoute::HoldInTaxiRoute( TYPE holdType,const DistanceUnit&dist, IntersectionNodeInSim* pNode, FlightGroundRouteDirectSegInSim* pSeg,int nHoldID  )
{
	m_hHoldType = holdType;
	m_dDistInRoute = dist;
	m_pNode = pNode;
	m_pDirSeg = pSeg;
	m_nHoldId = nHoldID;
	mLinkDirSeg = NULL;
	m_bCrossed = false;
	m_bIsRunwayExit = false;
}

HoldInTaxiRoute::HoldInTaxiRoute()
{
	m_hHoldType = NONE;
	m_dDistInRoute = 0;
	m_pNode = NULL;
	m_pDirSeg = NULL;
	m_nHoldId =  -1;
	mLinkDirSeg = NULL;
	m_bCrossed = false;
	m_bIsRunwayExit= false;
}

bool HoldInTaxiRoute::IsRunwayHold() const
{
	return m_pNode->GetNodeInput().IsRunwayIntersectionNode() && m_pDirSeg->GetType()!= AirsideResource::ResType_RunwayDirSeg;
}

bool HoldInTaxiRoute::IsTakeoffPosHold( RunwayExitInSim* pExit ) const
{
	if(pExit && m_pNode)
	{
		return pExit->GetExitInput().GetIntersectNodeID() == m_pNode->GetID();
	}
	return false;
}
HoldInTaxiRoute* FlightHoldListInTaxiRoute::GetEntryHold( IntersectionNodeInSim* pNode, bool bLast)const
{
	if(bLast)
	{
		for(int i=GetCount()-1;i>=0;i--)
		{
			const HoldInTaxiRoute& theHold = ItemAt(i);
			if(theHold.m_pNode == pNode && theHold.m_hHoldType == HoldInTaxiRoute::ENTRYNODE	)
				return (HoldInTaxiRoute*)&theHold;
		}
	}
	else
	{
		for(int i=0;i< GetCount();i++)
		{
			const HoldInTaxiRoute& theHold = ItemAt(i);
			if(theHold.m_pNode == pNode && theHold.m_hHoldType == HoldInTaxiRoute::ENTRYNODE	)
				return (HoldInTaxiRoute*)&theHold;
		}
	}
	return NULL;
}

HoldInTaxiRoute* FlightHoldListInTaxiRoute::GetExitHold( IntersectionNodeInSim* pNode )const
{
	for(int i=0;i< GetCount();i++)
	{
		const HoldInTaxiRoute& theHold = ItemAt(i);
		if(theHold.m_pNode == pNode && theHold.m_hHoldType == HoldInTaxiRoute::EXITNODE_BUFFER)
			return (HoldInTaxiRoute*)&theHold;
	}
	return NULL;
}

HoldInTaxiRoute* FlightHoldListInTaxiRoute::GetExitHold( const HoldInTaxiRoute& entryHold ) const
{
	for(int i=0;i< GetCount();i++)
	{
		const HoldInTaxiRoute& theHold = ItemAt(i);
		if(theHold.m_pNode == entryHold.m_pNode && theHold.m_hHoldType == HoldInTaxiRoute::EXITNODE_BUFFER && theHold.m_dDistInRoute >= entryHold.m_dDistInRoute )
			return (HoldInTaxiRoute*)&theHold;
	}
	return NULL;

}
void FlightHoldListInTaxiRoute::SetNodeExitTime( const DistanceUnit& distInRoute, AirsideFlightInSim* pFlight, const ElapsedTime& exitTime )
{
	for(int i=0;i<(int) m_vHoldList.size();i++)
	{
		HoldInTaxiRoute& theHold = m_vHoldList[i];
		if(theHold.m_hHoldType == HoldInTaxiRoute::EXITNODE_BUFFER && theHold.m_dDistInRoute < distInRoute )
		{
			IntersectionNodeInSim* pTheNode = theHold.m_pNode;
			OccupancyInstance fligthOCy = pTheNode->GetOccupyInstance(pFlight);
			if( fligthOCy.IsValid() && !fligthOCy.IsExitTimeValid() )
			{
				pTheNode->SetExitTime(pFlight, exitTime);
			}		

		}
	}
}



std::vector<HoldInTaxiRoute> FlightHoldListInTaxiRoute::GetHoldFromTo( const DistanceUnit& distF, const DistanceUnit& distT ) const
{
	std::vector<HoldInTaxiRoute> vReslt;
	for(int i=0;i<GetCount();i++)
	{
		const HoldInTaxiRoute& theHold = ItemAt(i);
		if(distF<0 && theHold.m_dDistInRoute < 0 )
			vReslt.push_back(theHold);
		if(theHold.m_dDistInRoute >= distF && theHold.m_dDistInRoute < distT )
			vReslt.push_back(theHold);
	}
	return vReslt;
}

std::vector<HoldInTaxiRoute> FlightHoldListInTaxiRoute::GetNextRouteNodeHoldList( DistanceUnit distInRoute )const
{	
	
	std::vector<HoldInTaxiRoute> vNextHoldList;
	for(int i=0;i< GetCount();i++)
	{
		const HoldInTaxiRoute& theHold = ItemAt(i);
		if( theHold.m_dDistInRoute < 0 && distInRoute < 0)
		{
			vNextHoldList.push_back(theHold);
		}
		else if( theHold.m_dDistInRoute >= distInRoute )
		{
			vNextHoldList.push_back(theHold);
		}			
	}
	return vNextHoldList;
	
}

//get last node hold
HoldInTaxiRoute* FlightHoldListInTaxiRoute::GetLastEntryHold()const
{

	
	for(int i=m_vHoldList.size()-1; i>=0;i--)
	{
		const HoldInTaxiRoute& theHold = ItemAt(i);
		if(theHold.m_hHoldType == HoldInTaxiRoute::ENTRYNODE)
		{
			return (HoldInTaxiRoute*)&theHold;
		}
	}
	
	return NULL;
}

HoldInTaxiRoute* FlightHoldListInTaxiRoute::GetFirstEntryHold()const
{
	for(int i=0; i<(int)m_vHoldList.size();++i)
	{
		const HoldInTaxiRoute& theHold = ItemAt(i);
		if(theHold.m_hHoldType == HoldInTaxiRoute::ENTRYNODE)
		{
			return (HoldInTaxiRoute*) &theHold;
		}
	}

	return NULL;
}

HoldInTaxiRoute* FlightHoldListInTaxiRoute::GetFirstExitHold()const 
{
	for(int i=0; i<(int)m_vHoldList.size();++i)
	{
		const HoldInTaxiRoute& theHold = ItemAt(i);
		if(theHold.m_hHoldType == HoldInTaxiRoute::EXITNODE_BUFFER)
		{
			return (HoldInTaxiRoute*)&theHold;
		}
	}

	return NULL;
}

std::vector<HoldInTaxiRoute*> FlightHoldListInTaxiRoute::GetNextRunwayEntryHoldList( DistanceUnit distInRoute ) const
{	
	std::vector<HoldInTaxiRoute*> vNextHoldList;
	for(int i=0;i< GetCount();i++)
	{
		const HoldInTaxiRoute& theHold = ItemAt(i);
		if( theHold.m_dDistInRoute >= distInRoute && theHold.m_hHoldType == HoldInTaxiRoute::ENTRYNODE && theHold.IsRunwayHold() )
		{
			vNextHoldList.push_back((HoldInTaxiRoute*)&theHold);
		}			
	}
	return vNextHoldList;
}

HoldInTaxiRoute* FlightHoldListInTaxiRoute::GetNextNodeEntryHold( DistanceUnit distInRoute ) const
{	
	for(int i=0;i< GetCount();i++)
	{
		const HoldInTaxiRoute& theHold = ItemAt(i);
		if( theHold.m_dDistInRoute >= distInRoute && theHold.m_hHoldType == HoldInTaxiRoute::ENTRYNODE )
		{
			return (HoldInTaxiRoute*)&theHold;
		}			
	}
	return NULL;
}

int FlightHoldListInTaxiRoute::GetHoldUntil( const DistanceUnit& distEnd ) const
{	
	for(int i=0;i<GetCount();i++)
	{
		const HoldInTaxiRoute& theHold = ItemAt(i);
		if( theHold.m_dDistInRoute < distEnd )
			continue;
		
		return i;
	}
	return -1;
}



HoldInTaxiRoute* FlightHoldListInTaxiRoute::GetLastRunwayEntryHold() const
{
	for(int i=m_vHoldList.size()-1; i>=0;i--)
	{
		const HoldInTaxiRoute& theHold = ItemAt(i);
		if(theHold.m_hHoldType == HoldInTaxiRoute::ENTRYNODE && theHold.IsRunwayHold() )
		{
			return (HoldInTaxiRoute*)&theHold;
		}
	}

	return NULL;
}

HoldInTaxiRoute* FlightHoldListInTaxiRoute::GetExitRunwayHold( const HoldInTaxiRoute& entryHold ) const
{
	for(int i=0;i< GetCount();i++)
	{
		const HoldInTaxiRoute& theHold = ItemAt(i);
		if(theHold.m_dDistInRoute < entryHold.m_dDistInRoute)
			continue;

		if( (theHold.m_hHoldType == HoldInTaxiRoute::EXITNODE_BUFFER ) && (theHold.m_pDirSeg?theHold.m_pDirSeg->GetType()!=AirsideResource::ResType_RunwayDirSeg:true) )
		{
			return (HoldInTaxiRoute*)&theHold;
		}
	}
	return NULL;
}

/************************************************************************/
/* Fillet Point in TaxiRoute                                                                     */
/************************************************************************/
void FiletPointListInTaxiRoute::Init( TaxiRouteInSim& theRoute )
{
	m_vFiletPoints.clear();
	FlightGroundRouteDirectSegInSim * pPreSeg = NULL;

	for(int i=0;i<theRoute.GetItemCount();i++)
	{
		FlightGroundRouteDirectSegInSim* pSeg = NULL;
		TaxiRouteItemInSim& routeItem = theRoute.ItemAt(i);
		if( routeItem.GetResource() && routeItem.GetResource()->IsKindof(typeof(FlightGroundRouteDirectSegInSim)) )
		{
			pSeg = (FlightGroundRouteDirectSegInSim* ) routeItem.GetResource();
		}
	
		if(pSeg && pPreSeg)
		{
			if(pSeg->GetObjectID() != pPreSeg->GetObjectID())
			{
				DistanceUnit distFrom = 0;
				DistanceUnit distTo = pSeg->GetEndDist();

				DistanceUnit PreOffset = 0;
				DistanceUnit filetEndDist =0;
								
				IntersectionNodeInSim* pNode = pSeg->GetEntryNode();			
				FilletTaxiway * pFillet = pNode->GetFilletFromTo(pPreSeg,pSeg);
				if(pFillet)
				{
					int taxiID = pSeg->GetObjectID();
					if( taxiID == pFillet->GetObject1ID() )
					{
						filetEndDist =  abs( pFillet->GetFilletPoint1().GetDistToIntersect() + pFillet->GetFilletPoint1().GetUsrDist() ) ; 
						PreOffset = abs( pFillet->GetFilletPoint2().GetDistToIntersect() + pFillet->GetFilletPoint2().GetUsrDist() );
					}
					else
					{
						filetEndDist = abs(pFillet->GetFilletPoint2().GetDistToIntersect() + pFillet->GetFilletPoint2().GetUsrDist() ) ;
						PreOffset = abs(pFillet->GetFilletPoint1().GetDistToIntersect() + pFillet->GetFilletPoint1().GetUsrDist() );
					}

					m_vFiletPoints.push_back( FiletPointInTaxiRoute(theRoute.GetItemBeginDist(i) - PreOffset,theRoute.GetItemBeginDist(i) + filetEndDist) );
					//m_vFiletPoints.push_back( FiletPointInTaxiRoute(theRoute.GetItemBeginDist(i) - PreOffset,FiletPointInTaxiRoute::FiletBegin) );
					//m_vFiletPoints.push_back( FiletPointInTaxiRoute(theRoute.GetItemBeginDist(i) + filetEndDist,FiletPointInTaxiRoute::FiletEnd) );
				}	
			}
		}	

		pPreSeg = pSeg;
	}
	std::sort(m_vFiletPoints.begin(),m_vFiletPoints.end());
}

std::vector<FiletPointInTaxiRoute> FiletPointListInTaxiRoute::GetFiletPointsBetween( const DistanceUnit& distF, const DistanceUnit& distT ) const
{
	

	std::vector<FiletPointInTaxiRoute> vreslt;
	for(int i=0;i<(int)m_vFiletPoints.size();i++)
	{
		const FiletPointInTaxiRoute& thePt = m_vFiletPoints[i];
		
		if ( thePt.GetFromDistInRoute() >= distF && thePt.GetFromDistInRoute() < distT )
		{
			vreslt.push_back(thePt);
		}
	}
	return vreslt;
}

const FiletPointInTaxiRoute* FiletPointListInTaxiRoute::IsDistInFilet( const DistanceUnit& dist ) const
{
	for(int i=0;i<(int)m_vFiletPoints.size();i++)
	{
		const FiletPointInTaxiRoute& filet = m_vFiletPoints[i];
		if( filet.GetFromDistInRoute()< dist && dist < filet.GetToDistInRoute() )
		{
			return &filet;
		}

		/*if(m_vFiletPoints.at(i).m_type == FiletPointInTaxiRoute::FiletBegin)
		{
		FiletPointInTaxiRoute *pFiletPtEnd = GetNextEndPoint(i);
		if( pFiletPtEnd )
		{
		if(m_vFiletPoints.at(i).m_distInRoute < dist && dist < pFiletPtEnd->m_distInRoute)
		{
		return (FiletPointInTaxiRoute*)&m_vFiletPoints[i];
		}
		}
		}*/
	}
	return NULL;
}

//FiletPointInTaxiRoute * FiletPointListInTaxiRoute::GetNextEndPoint( int idx ) const
//{
//	for(int i=idx;i<(int)m_vFiletPoints.size();i++)
//	{
//		if(m_vFiletPoints.at(i).m_type == FiletPointInTaxiRoute::FiletEnd)
//			return (FiletPointInTaxiRoute*)&m_vFiletPoints[i];
//	}
//	return NULL;
//}
/////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////
 LaneIntersectionNotifyPoint:: LaneIntersectionNotifyPoint( DistanceUnit dist,LaneFltGroundRouteIntersectionInSim* pIntersection, TYPE t )
{
	mDistInRoute = dist;
	mpNode = pIntersection;
	m_type = t;
}

//enter event
#include "AirsideEvent.h"
class FlightEnterLaneAreaEvent : public AirsideEvent
{
public:
	FlightEnterLaneAreaEvent(AirsideFlightInSim* pFlt, LaneFltGroundRouteIntersectionInSim* pNode){
		mpFlight = pFlt;
		mpNode = pNode;
	}
	virtual int Process() { mpNode->OnFlightEnter(mpFlight,getTime()); return 1; }

	virtual const char *getTypeName (void) const { return "FlightEnterLane"; };

	//It returns event type
	virtual int getEventType (void) const { return FlightEnterLane; };
protected:
	AirsideFlightInSim* mpFlight;
	LaneFltGroundRouteIntersectionInSim* mpNode;
};
class FlightExitLaneAreaEvent : public AirsideEvent
{
public:
	FlightExitLaneAreaEvent(AirsideFlightInSim* pFlt, LaneFltGroundRouteIntersectionInSim* pNode){
		mpFlight = pFlt;
		mpNode = pNode;
	}
	virtual int Process() { mpNode->OnFlightExit(mpFlight,getTime());return 1; } 

	virtual const char *getTypeName (void) const{ return "FlightExitLane"; };

	//It returns event type
	virtual int getEventType (void) const { return FlightExitLane; };
protected:
	AirsideFlightInSim* mpFlight;
	LaneFltGroundRouteIntersectionInSim* mpNode;
};



void LaneIntersectionNotifyPoint::OnFlightAt( AirsideFlightInSim* pFlt, const ElapsedTime& t )
{
	if(m_type == ENTRY_POINT)
	{
		AirsideEvent* pEvent = new FlightEnterLaneAreaEvent(pFlt,mpNode);
		pEvent->setTime(t);
		pEvent->addEvent();
	}
	else
	{
		AirsideEvent* pEvent = new FlightExitLaneAreaEvent(pFlt,mpNode);
		pEvent->setTime(t);
		pEvent->addEvent();
	}
}

std::vector< LaneIntersectionNotifyPoint> LaneIntersectionNotifyPointList::GetNotifyPointsBetween( const DistanceUnit& distF, const DistanceUnit& distT ) const
{
	std::vector< LaneIntersectionNotifyPoint> vreslt;
	for(int i=0;i<(int)mvPoints.size();i++)
	{
		const  LaneIntersectionNotifyPoint& thePt = mvPoints[i];
		if ( thePt.mDistInRoute >= distF && thePt.mDistInRoute < distT )
		{
			vreslt.push_back(thePt);
		}
	}
	return vreslt;
}

void LaneIntersectionNotifyPointList::Init( TaxiRouteInSim& theRoute, AirsideFlightInSim*pFlight)
{
	if(!pFlight)
		return;

	if(mpFlight==pFlight)
	{
		return;
	}

	mvPoints.clear();
	mpFlight = pFlight;
	for(int i=0;i<theRoute.GetItemCount();i++)
	{
		TaxiwaySegInSim* pSeg = NULL;
		TaxiwayDirectSegInSim* pDirSeg = NULL;
		TaxiRouteItemInSim& routeItem = theRoute.ItemAt(i);
		if( routeItem.GetResource() && routeItem.GetResource()->GetType() == AirsideResource::ResType_TaxiwayDirSeg )
		{
			pDirSeg  = (TaxiwayDirectSegInSim* ) routeItem.GetResource();
			pSeg = pDirSeg->GetTaxiwaySeg();
		}
		if(!pSeg)continue;
		for(int jLandidx=0; jLandidx<pSeg->getIntersectLaneCount();++jLandidx)
		{
			VehicleLaneInSim* pLane = pSeg->getIntersectLane(jLandidx);
			if(pLane)
			{
				for(int idxL=0;idxL <pLane->GetTaxiwayIntersectionCount();++idxL)
				{
					LaneFltGroundRouteIntersectionInSim* intersect =  pLane->FltGroundRouteIntersectionAt(idxL);
					if( intersect->GetFltRouteSegment() == pSeg )
					{
						DistanceUnit distInSeg = intersect->GetIntersectDistAtTaxiwaySeg();
						DistanceUnit distInDirSeg = pDirSeg->IsPositiveDir()?distInSeg:(pSeg->GetSegmentLength()-distInSeg);
						DistanceUnit distInRoute = theRoute.GetItemBeginDist(i) + distInDirSeg;
						DistanceUnit distEnter = distInRoute - 200*SCALE_FACTOR;
						DistanceUnit distExit = distInRoute + pFlight->GetIntersectionBuffer() + pFlight->GetLength()*0.5;
						mvPoints.push_back(  LaneIntersectionNotifyPoint(distEnter,intersect, LaneIntersectionNotifyPoint::ENTRY_POINT) );
						mvPoints.push_back(  LaneIntersectionNotifyPoint(distExit,intersect, LaneIntersectionNotifyPoint::EXIT_POINT ) ); 
					}
				}
			}
		}
	}
	std::sort(mvPoints.begin(),mvPoints.end());		
		
}

void LaneIntersectionNotifyPointList::clearFlightLocks(AirsideFlightInSim* pFlight,const ElapsedTime& tTime)
{
	for(int i=0;i<(int)mvPoints.size();i++)
	{
		 LaneIntersectionNotifyPoint& notPts = mvPoints[i];
		 if(notPts.m_type == LaneIntersectionNotifyPoint::EXIT_POINT){
			 notPts.mpNode->OnFlightExit(pFlight,tTime);
		 }
	}
}

std::vector< LaneIntersectionNotifyPoint> LaneIntersectionNotifyPointList::GetNotifyPointsUntil( const DistanceUnit& distT ) const
{
	std::vector< LaneIntersectionNotifyPoint> vreslt;
	for(int i=0;i<(int)mvPoints.size();i++)
	{
		const  LaneIntersectionNotifyPoint& thePt = mvPoints[i];
		if ( thePt.mDistInRoute < distT )
		{
			vreslt.push_back(thePt);
		}
	}
	return vreslt;
}

//////////////////////////////////////////////////////////////////////////
std::vector< FlightInterruptPosInRoute> FlightInterruptPosListInRoute::GetNextItems( const DistanceUnit& distF ) const
{
	std::vector<FlightInterruptPosInRoute> vReslt;
	for(int i=0;i<(int)m_vData.size();i++)
	{
		const FlightInterruptPosInRoute& waitpos = m_vData[i];
		if( (int)waitpos.m_distInRoute >= (int)distF)
		{	
			vReslt.push_back(waitpos);
		}		
	}
	return vReslt;
}

void FlightInterruptPosListInRoute::Init( TaxiRouteInSim& theRoute,AirsideFlightInSim*pFlight )
{
	mpFlight = pFlight;

	int nCount = theRoute.GetItemCount();
	for (int i =0; i < nCount; i++)
	{
		TaxiRouteItemInSim& pItem = theRoute.ItemAt(i);
		if (pItem.GetResource() && pItem.GetResource()->GetType() == AirsideResource::ResType_TaxiwayDirSeg)
		{
			TaxiwayDirectSegInSim* pDirSeg = (TaxiwayDirectSegInSim*)pItem.GetResource();
			TaxiwaySegInSim* pSeg = pDirSeg->GetTaxiwaySeg();
			std::vector<TaxiInterruptLineInSim*> vLines = pSeg->GetHoldShortLine();

			if (vLines.empty())
				continue;

			for(int idxL=0;idxL<(int)vLines.size();idxL++)
			{
				TaxiInterruptLineInSim* pLineInSim = vLines[idxL];
				if(GetLineIdx(pLineInSim)>=0) //already in data
					continue; 

				bool bInSeg = false;
				DistanceUnit distInRoute=0;
				if(pLineInSim->GetInterruptLineInput()->GetFrontIntersectionNodeID() == pDirSeg->GetEntryNode()->GetID() && 
					pLineInSim->GetInterruptLineInput()->GetBackIntersectionNodeID() == pDirSeg->GetExitNode()->GetID() )
				{
					distInRoute = theRoute.GetItemBeginDist(i) + pLineInSim->GetInterruptLineInput()->GetDistToFrontIntersectionNode();
					distInRoute -= pFlight->GetWingspan()*0.5;
					bInSeg = true;
				}
				if(pLineInSim->GetInterruptLineInput()->GetFrontIntersectionNodeID() == pDirSeg->GetExitNode()->GetID() && 
					pLineInSim->GetInterruptLineInput()->GetBackIntersectionNodeID() == pDirSeg->GetEntryNode()->GetID() )
				{
					distInRoute = theRoute.GetItemEndDist(i) - pLineInSim->GetInterruptLineInput()->GetDistToFrontIntersectionNode();
					distInRoute -= pFlight->GetWingspan()*0.5;
					bInSeg = true;
				}
				
				if(bInSeg)
				{
					m_vData.push_back(FlightInterruptPosInRoute(distInRoute,pLineInSim,pDirSeg->GetEntryNode()->GetID()) );
				}				
			}			
		}

	}

	std::sort(m_vData.begin(),m_vData.end());

}

int FlightInterruptPosListInRoute::GetLineIdx( TaxiInterruptLineInSim* pLine ) const
{
	for(int i=0;i<(int)m_vData.size();++i)
	{
		if( m_vData.at(i).mpInterruptLine == pLine ) 
			return i;
	}
	return -1;
}
