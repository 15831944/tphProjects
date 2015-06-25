#include "StdAfx.h"
#include ".\vehiclerouteinsim.h"
#include ".\VehicleStretchInSim.h"
#include "./AirsideVehicleInSim.h"
#include "AirsideFlightInSim.h"
#include "../../Common/ARCMathCommon.h"
#include "TaxiwayResource.h"
#include "VehicleRouteResourceManager.h"
#include "AirsidePaxBusInSim.h"
#include "AirsidePaxBusParkSpotInSim.h"
#include "../BagCartsParkingSpotInSim.h"
#include "../AirsideBaggageTrainInSim.h"


class VehicleConflictionInLane
{
public:
	VehicleConflictionInLane(AirsideVehicleInSim* pVehicle,DistanceUnit distInLane, VehicleLaneInSim* pLane){
		m_pVehicle = pVehicle;
		m_pLane = pLane;
		m_distInLane = distInLane;
		m_bConflict = false;
	}

	virtual void DoCheck() =0;
	bool bConflict()const{return m_bConflict;}
protected:
	AirsideVehicleInSim* m_pVehicle;
	VehicleLaneInSim* m_pLane;
	DistanceUnit m_distInLane;
	bool m_bConflict;
};

class ConflictionWithLeadVehicle : public VehicleConflictionInLane
{
public:
	ConflictionWithLeadVehicle(AirsideVehicleInSim* pVehicle,DistanceUnit distInLane, VehicleLaneInSim* pLane)
		:VehicleConflictionInLane(pVehicle,distInLane,pLane){
			m_pLeadVehicle = NULL;
		}

		virtual void DoCheck()
		{
			AirsideVehicleInSim * pVehicleAhead = m_pLane->GetVehicleAhead(m_distInLane);
			if(pVehicleAhead && pVehicleAhead!= m_pVehicle)
			{
				DistanceUnit distToVehicleAhead = pVehicleAhead->GetDistInResource() - m_distInLane;
				DistanceUnit sepDist = m_pVehicle->GetSeparationDist(pVehicleAhead) + 0.5*(m_pVehicle->GetVehicleLength() + pVehicleAhead->GetVehicleLength() );

				m_dSafeDistInLane = m_distInLane + distToVehicleAhead - sepDist;
				m_tSafeTimeInLane = pVehicleAhead->GetTime();
				m_vSafeSpdInLane = pVehicleAhead->GetSpeed();
				m_pLeadVehicle = pVehicleAhead;
				m_bConflict = true;
				return;
			}
		}

		DistanceUnit m_dSafeDistInLane;
		ElapsedTime m_tSafeTimeInLane;
		double m_vSafeSpdInLane;

public:
	AirsideVehicleInSim * m_pLeadVehicle;
};

//class ConflictionWithTaxiwayFlight : public VehicleConflictionInLane
//{
//public:
//	ConflictionWithTaxiwayFlight(AirsideVehicleInSim* pVehicle,ElapsedTime atTime , DistanceUnit dCurDist,DistanceUnit dNextDist, VehicleLaneInSim* pLane)
//		:VehicleConflictionInLane(pVehicle,dCurDist,pLane){
//			m_tCurTime = atTime;
//			m_pConflictFlight = NULL;
//			m_dNextDist = dNextDist;
//		}
//
//		virtual void DoCheck()
//		{
//			for(int i=0;i<m_pLane->GetTaxiwayIntersectionCount();i++)
//			{
//				DistanceUnit intersectDist = m_pLane->FltGroundRouteIntersectionAt(i)->GetIntersectDistAtLane();
//				if( intersectDist > m_distInLane && intersectDist < m_dNextDist )
//				{
//					AirsideFlightInSim * pLatestFlight =  m_pLane->GetConflictFlightArroundIntersection(m_pLane->FltGroundRouteIntersectionAt(i),100*SCALE_FACTOR, m_tCurTime );
//					if(pLatestFlight)
//					{					
//						m_tSafeTimeInLane = pLatestFlight->GetTime();
//						m_vSafeSpdInLane = 0;
//						m_pConflictFlight = pLatestFlight;
//						m_bConflict =true;
//						return ;
//					}
//				}
//			}
//
//		}
//
//public:
//	ElapsedTime m_tSafeTimeInLane;
//	double m_vSafeSpdInLane;
//	AirsideFlightInSim* m_pConflictFlight;
//
//protected:
//	ElapsedTime m_tCurTime;
//	DistanceUnit m_dNextDist;
//
//};



//void GetNextDistInLane(AirsideVehicleInSim* pVehicle, VehicleLaneExit * pLaneExit, DistanceUnit dCurDist, DistanceUnit& nextDist, bool& bMayConflictWithFlight)
//{
//	VehicleLaneInSim* pLane = pLaneExit->GetLane();
//	DistanceUnit exitDist = pLaneExit->GetDistInLane() - pVehicle->GetVehicleLength()*0.5;
//	nextDist = min(exitDist,dCurDist+10000);
//	bMayConflictWithFlight = false;
//
//	int nextIntersectIdx = pLane->GetFirstTaxiwayIntersection( dCurDist );
//	if( nextIntersectIdx >= 0 )
//	{
//		DistanceUnit dHold1 = pLane->FltGroundRouteIntersectionAt(nextIntersectIdx).GetHold1() - pVehicle->GetVehicleLength() * 0.5;
//
//		DistanceUnit dHold2 = pLane->FltGroundRouteIntersectionAt(nextIntersectIdx).GetHold2() + pVehicle->GetVehicleLength() *0.5;
//		for(int i= nextIntersectIdx+1;i<pLane->GetTaxiwayIntersectionCount();i++)//check close intersection
//		{
//			DistanceUnit dNextHold1 = pLane->FltGroundRouteIntersectionAt(i).GetHold1() - pVehicle->GetVehicleLength() * 0.5; 
//			if( dNextHold1 < dHold2 ) //two Intersection are close , check it also
//			{
//				dHold2 = pLane->FltGroundRouteIntersectionAt(i).GetHold2() + pVehicle->GetVehicleLength() *0.5;
//			}
//			else
//			{
//				break;
//			}
//		}
//
//		if( dCurDist < dHold1 )
//		{
//			nextDist = dHold1;
//			bMayConflictWithFlight = false;
//		}
//		else
//		{
//			nextDist = min(dHold2,exitDist) ;
//			bMayConflictWithFlight = true;
//		}
//
//	}
//}
//
bool MayConflictWithFlight(AirsideVehicleInSim* pVehicle, VehicleLaneExit * pLaneExit, DistanceUnit dCurDist, DistanceUnit& dHold1,DistanceUnit& dHold2, std::vector<LaneFltGroundRouteIntersectionInSim*>& laneIntesectlist)
{
	VehicleLaneInSim* pLane = pLaneExit->GetLane();
	DistanceUnit exitDist = pLaneExit->GetDistInLane() - pVehicle->GetVehicleLength()*0.5;	

	int nextIntersectIdx = pLane->GetFirstTaxiwayIntersection( dCurDist );
	if( nextIntersectIdx >= 0 && nextIntersectIdx<pLane->GetTaxiwayIntersectionCount() )
	{
		dHold1 = pLane->FltGroundRouteIntersectionAt(nextIntersectIdx)->GetHold1() - pVehicle->GetVehicleLength() * 0.5;

		dHold2 = pLane->FltGroundRouteIntersectionAt(nextIntersectIdx)->GetHold2() + pVehicle->GetVehicleLength() *0.5;

		laneIntesectlist.push_back(pLane->FltGroundRouteIntersectionAt(nextIntersectIdx) );
		for(int i= nextIntersectIdx+1;i<pLane->GetTaxiwayIntersectionCount();i++)//check close intersection
		{
			DistanceUnit dNextHold1 = pLane->FltGroundRouteIntersectionAt(i)->GetHold1() - pVehicle->GetVehicleLength() * 0.5; 
			if( dNextHold1 < dHold2 ) //two Intersection are close , check it also
			{
				dHold2 = pLane->FltGroundRouteIntersectionAt(i)->GetHold2() + pVehicle->GetVehicleLength() *0.5;
			}
			else
			{
				return true;
			}
			laneIntesectlist.push_back(pLane->FltGroundRouteIntersectionAt(i));
		}
		return true;
	}
	return false;
}

bool VehicleRouteInSim::FindClearanceInConcern( AirsideVehicleInSim * pVehicle,ClearanceItem& lastItem, DistanceUnit radius, Clearance& newClearance )
{
	//check to see if this Vehicle has traveled all the route
	if(m_nNextItemIdx >= (int)m_vRouteItems.size() )
	{
		return false;	
	}

	if (pVehicle->GetVehicleBaseType() == VehicleType_TowTruck 
		&& (pVehicle->GetResource()->GetType() != AirsideResource::ResType_VehicleStretchLane && pVehicle->GetResource()->GetType() != AirsideResource::ResType_VehicleTaxiLane))	//just finish tow flight
	{
		VehicleRouteNodePairDist nextItem = m_vRouteItems.at(0);
		VehicleLaneInSim* pLane = dynamic_cast<VehicleLaneInSim*>(nextItem.GetNodeFrom()->GetOrignResource());
		if (pLane)
		{
			pVehicle->SetResource(pLane);			
			pVehicle->SetDistInResource(nextItem.GetDistance());
			m_nCurItemIdx  =0;
			m_nNextItemIdx =1;
		}
	}
	//get the next target node
	const double dVehicleSpd = pVehicle->GetOnRouteSpeed();

	//VehicleRouteNode* pNextNode = NULL;	
	if( m_nNextItemIdx > m_nCurItemIdx ){//going to enter next lane	
		VehicleLaneEntry * pEntry = NULL;
		VehicleRouteNodePairDist nextItem = m_vRouteItems.at(m_nNextItemIdx);
		if( nextItem.GetNodeFrom()->GetType() == VehicleRouteNode::LANE_ENTRY )
		{
			pEntry = (VehicleLaneEntry*)nextItem.GetNodeFrom();	
		}
		else if(nextItem.GetNodeTo()->GetType() == VehicleRouteNode::LANE_ENTRY )
		{
			pEntry = (VehicleLaneEntry*)nextItem.GetNodeTo(); 
			m_nNextItemIdx ++;			
		}
		ASSERT(pEntry);
		//check to see if can enter next item	
		

		ConflictionWithLeadVehicle conflictLead(pVehicle, pEntry->GetDistInLane()-1, pEntry->GetLane() );		
		conflictLead.DoCheck();		
		if( conflictLead.bConflict() && conflictLead.m_dSafeDistInLane <= pEntry->GetDistInLane() ) // wait for lead vehicle at next lane
		{			
			pVehicle->DependOnAgents(conflictLead.m_pLeadVehicle);			
			return true;
		}	

		DistanceUnit dSafeDistInRes = 0.0;
		AirsideVehicleInSim* pConflictVehicle = GetParkSpotConflictLeadPaxBus(pVehicle,lastItem,pEntry->GetDistInLane(),dSafeDistInRes);
		if (pConflictVehicle && dSafeDistInRes <= pEntry->GetDistInLane())
		{
			pVehicle->DependOnAgents(pConflictVehicle);
			return true;
		}
		//go to next lane
		CPoint2008 vehiclePos = pVehicle->GetPosition();
		if(m_nCurItemIdx>=0)
			vehiclePos = lastItem.GetPosition();

		DistanceUnit distToEntry = vehiclePos.distance( pEntry->GetPosition() );
		ElapsedTime dT = ElapsedTime(distToEntry / dVehicleSpd);
		ElapsedTime AtEntryTime = lastItem.GetTime() + dT;
		ClearanceItem enterItem(pEntry->GetLane(), m_mode, pEntry->GetDistInLane() );
		enterItem.SetSpeed( dVehicleSpd );
		enterItem.SetTime(AtEntryTime);
		lastItem = enterItem;
		newClearance.AddItem(lastItem);
		m_nCurItemIdx = m_nNextItemIdx;
	}
	else  if(m_nCurItemIdx == m_nNextItemIdx )//move on this lane
	{
		VehicleLaneExit* pExit = NULL;
		VehicleRouteNodePairDist curItem = m_vRouteItems.at(m_nCurItemIdx);

		if( curItem.GetNodeTo()->GetType() == VehicleRouteNode::LANE_EXIT )
		{
			pExit = (VehicleLaneExit*)curItem.GetNodeTo();
		}
		else if( curItem.GetNodeFrom()->GetType() == VehicleRouteNode::LANE_EXIT )
		{
			pExit = (VehicleLaneExit*)curItem.GetNodeFrom();
			ASSERT(false);
		}
		//check conflict  with taxiway intersection and vehicle ahead
		DistanceUnit exitLaneDist = pExit->GetDistInLane() - pVehicle->GetVehicleLength() *0.5;
		DistanceUnit dCurDist = lastItem.GetDistInResource();

		DistanceUnit dStep = 100*SCALE_FACTOR;
		DistanceUnit dNextDist = dCurDist + dStep;
		DistanceUnit dHold1, dHold2;//Hold1 or Hold2
		std::vector<LaneFltGroundRouteIntersectionInSim*> vLantIntersects;
		bool bMayIntersectWithFlight = MayConflictWithFlight(pVehicle, pExit, dCurDist, dHold1, dHold2,vLantIntersects); // may conflict with flight ,should not  stop between (current dist,dNextDist) 
	
		if( bMayIntersectWithFlight && dCurDist>= dHold1) //Vehicle arrival at the hold check to see conflict with flight
		{
			for(int i=0;i<(int)vLantIntersects.size();i++)
			{
				LaneFltGroundRouteIntersectionInSim* intesect = vLantIntersects[i];
				if(!intesect->IsActiveFor(pVehicle,NULL))
				{
					pVehicle->DependOnAgents(intesect);
					return true;
				}
			}
		}
		
		//no flight conflict, now check conflict with lead vehicle
		ConflictionWithLeadVehicle conflictLead(pVehicle, lastItem.GetDistInResource(), pExit->GetLane() );
		conflictLead.DoCheck();
		if( conflictLead.bConflict() )
		{	
			dNextDist = min(dNextDist, conflictLead.m_dSafeDistInLane); //keep separation with lead flight
		}

		if(bMayIntersectWithFlight) //next dist should not between hold1 and Hold2 and should not exceeds hold2 for unknown holds
		{
			if( dCurDist < dHold1 )
			{
				dNextDist = min(dNextDist, dHold1);
			}
			else
			{				
				if(conflictLead.bConflict() && conflictLead.m_dSafeDistInLane < dHold2) //conflict with lead and can not go cross taxiway
					dNextDist = dHold1;
				else  //between hold1, and hold2
					dNextDist = dHold2;
			}
		}

		DistanceUnit dSafeDistInRes = 0.0;
		AirsideVehicleInSim* pConflictVehicle = GetParkSpotConflictLeadPaxBus(pVehicle,lastItem,dNextDist,dSafeDistInRes);
		if (pConflictVehicle)
		{
			dNextDist = min(dNextDist,dSafeDistInRes);
		}

		if(dNextDist<= dCurDist  ) //vehicle can not move? 
		{
			if(conflictLead.bConflict())
			{
				pVehicle->DependOnAgents(conflictLead.m_pLeadVehicle);					
				return true;
			}
			if (pConflictVehicle)
			{
				pVehicle->DependOnAgents(pConflictVehicle);
				return true;
			}
			dNextDist = dCurDist;
		}


		if(dNextDist>=exitLaneDist)
		{
			dNextDist = exitLaneDist;
			m_nNextItemIdx++;
		}
		ClearanceItem nextItem = lastItem;
		nextItem.SetDistInResource(dNextDist);
		nextItem.SetSpeed(dVehicleSpd);
		ElapsedTime dT = pVehicle->GetTimeBetween(lastItem,nextItem);
		nextItem.SetTime(dT + lastItem.GetTime() );		
		lastItem = nextItem;
		newClearance.AddItem(lastItem);
		return true;		
	}
	else 
	{
		ASSERT( false);
	}


	return true;
}

VehicleRouteInSim::VehicleRouteInSim()
{
	m_nCurItemIdx = -1; m_mode = OnVehicleBirth; m_nNextItemIdx = 0 ;
}

VehicleRouteInSim::VehicleRouteInSim( const std::vector<VehicleRouteNodePairDist>& vroute )
{
	m_vRouteItems.assign(vroute.begin(),vroute.end());
	m_nCurItemIdx = -1;
	m_mode = OnVehicleBirth; 
	m_nNextItemIdx = 0;

}

void VehicleRouteInSim::CopyDataToRoute( VehicleRouteInSim& route )
{
	route.AddData(m_vRouteItems);
}

void VehicleRouteInSim::AddData( const std::vector<VehicleRouteNodePairDist>& vroute )
{
	m_vRouteItems.insert(m_vRouteItems.end(),vroute.begin(),vroute.end());
}

void VehicleRouteInSim::AddData(const VehicleRouteNodePairDist& nodePair)
{
	m_vRouteItems.push_back(nodePair);
}

void VehicleRouteInSim::PopBack()
{
	m_vRouteItems.pop_back();
}

bool VehicleRouteInSim::IsEmpty() const
{
	return m_vRouteItems.empty();
}

const AirsideResource* VehicleRouteInSim::GetDestResource()
{
	if (m_vRouteItems.empty())
		return NULL;

	int nCount = m_vRouteItems.size();
	VehicleRouteNodePairDist endNode = m_vRouteItems.at(nCount-1);

	return endNode.GetNodeTo()->GetDestResource();
}

double VehicleRouteInSim::GetLength()const
{
	double dDist = 0.0;
	for (int i = 0; i < (int)m_vRouteItems.size(); i++)
	{
		const VehicleRouteNodePairDist& node = m_vRouteItems[i];
		dDist += node.GetDistance();
	}
	return dDist;
}

bool VehicleRouteInSim::GetVehicleBeginPos(CPoint2008& ptBegin)
{
	if (m_vRouteItems.empty())
		return false;

	VehicleRouteNodePairDist firstPair = m_vRouteItems.at(0);
	VehicleRouteNode* pNodeFrom = firstPair.GetNodeFrom();
	if (pNodeFrom == NULL)
		return false;
	
	ptBegin = pNodeFrom->GetPosition();
	return true;
}

void VehicleRouteInSim::SetVehicleBeginPos( const CPoint2008& pos )
{
	if (m_vRouteItems.empty())
		return;

	VehicleRouteNodePairDist firstPair = m_vRouteItems.at(0);

	double dDist = pos.distance(firstPair.GetNodeFrom()->GetPosition());
	VehicleRouteNodePairDist nodePair(firstPair.GetNodeFrom(),firstPair.GetNodeTo(),dDist);

	m_vRouteItems.erase(m_vRouteItems.begin());
	m_vRouteItems.insert(m_vRouteItems.begin(),nodePair);
}

const VehicleLaneInSim* VehicleRouteInSim::GetFirstLaneResource()
{
	if (m_vRouteItems.empty())
		return NULL;

	VehicleRouteNodePairDist FirstNode = m_vRouteItems.at(0);

	return FirstNode.GetNodeFrom()->GetLane();
}

void VehicleRouteInSim::ClearRouteItems()
{
	m_vRouteItems.clear();
}

AirsideVehicleInSim* VehicleRouteInSim::GetParkSpotConflictLeadPaxBus( AirsideVehicleInSim* pVehicle,ClearanceItem& lastItem, double dNextDist,double& dSafeDistInLane)
{
	//check park spot whether occupy
	AirsideFlightInSim* pFlight = pVehicle->GetServiceFlight();
	if (pFlight == NULL)
		return NULL;

	AirsideVehicleInSim* pLeadVehicle = NULL;
	if (m_mode == OnMoveToGate)
	{
		CAirsidePaxBusInSim *pPaxBus = (CAirsidePaxBusInSim *)pVehicle;
		AirsidePaxBusParkSpotInSim* pParkSpotInSim = pFlight->GetPaxBusParking(pPaxBus->IsServiceArrival());
		if (pParkSpotInSim == NULL)
			return NULL;
		
		//test passenger whether can lock the park spot
		pLeadVehicle = pParkSpotInSim->GetLastInResourceVehicle();
	}
	
	
	if (m_mode == OnMoveToBagTrainSpot)
	{
		AirsideBaggageTrainInSim *pBagTrain = (AirsideBaggageTrainInSim *)pVehicle;
		CBagCartsParkingSpotInSim* pBagCartsSpotInSim = pBagTrain->getBagCartsSpotInSim();
		if (pBagCartsSpotInSim == NULL)
			return NULL;

		//test passenger whether can lock the park spot
		pLeadVehicle = pBagCartsSpotInSim->GetLastInResourceVehicle();

	}

	if (pLeadVehicle == NULL)
		return NULL;

	double dDist = 0.0;
	for (int i = 0; i < m_nCurItemIdx; i++)
	{
		const VehicleRouteNodePairDist& node = m_vRouteItems[i];
		dDist += node.GetDistance();
	}
	double dMoveDist = dDist + dNextDist;

	DistanceUnit sepDist = pVehicle->GetSeparationDist(pLeadVehicle) + 0.5*(pVehicle->GetVehicleLength() + pLeadVehicle->GetVehicleLength());

	double dLenght = GetLength();
	double distToVehicleAhead = dLenght - dMoveDist;
	if (distToVehicleAhead >= sepDist)
	{
		return NULL;
	}

	dSafeDistInLane = dNextDist + distToVehicleAhead - sepDist;
	return pLeadVehicle;
}

VehicleRouteNode* VehicleRouteInSim::getLastNode()
{
	if (m_vRouteItems.empty())
		return NULL;

	int nCount = m_vRouteItems.size();
	VehicleRouteNodePairDist endNode = m_vRouteItems.at(nCount-1);

	return endNode.GetNodeTo();
}
