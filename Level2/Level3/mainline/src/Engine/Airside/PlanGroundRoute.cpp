#include "StdAfx.h"
#include "./PlanGroundRoute.h"
#include "AirspaceTrafficController.h"
#include "RunwayInSim.h"
#include "InputAirside/AirRoute.h"
#include "Clearance.h"
#include "AirsideFlightInSim.h"


PlanGroundRouteItem * PlanGroundRoute::GetItem( int idx )
{
	if(idx < GetItemCount()  && idx >=0)
		return &m_vRoute[idx];
	return NULL;
}

const PlanGroundRouteItem * PlanGroundRoute::GetItem( int idx ) const
{
	if(idx < GetItemCount()  && idx >=0)
		return &m_vRoute[idx];
	return NULL;
}
PlanGroundRouteItem * PlanGroundRoute::GetNextItem()
{
	for(int i=0;i < GetItemCount(); i++)
	{
		PlanGroundRouteItem  *pItem = GetItem(i);
		if(pItem && !pItem->IsFinished())
			return pItem;
	}
	return NULL;
}

void PlanGroundRoute::SetRoute( const TaxiwayDirectSegList& vSegments )
{
	m_vRoute.clear();
	m_vRoute.reserve(vSegments.size());
	for(int i =0; i<(int)vSegments.size();i++)
	{
		PlanGroundRouteItem routeItem((TaxiwayDirectSegInSim*)vSegments.at(i).get());
		m_vRoute.push_back(routeItem);
	}
	Upgrade();
}

int PlanGroundRoute::GetFirstUnDoneItemId()const
{
	for(int i=0;i < GetItemCount(); i++)
	{
		const PlanGroundRouteItem  *pItem = GetItem(i);
		if(pItem && !pItem->IsFinished())
			return i;
	}
	return -1;
}

void PlanGroundRoute::Activate()
{
	int lastActiveSeg = -1;
	int i;
	for( i =GetFirstUnDoneItemId();i< GetItemCount();i++)
	{
		PlanGroundRouteItem * pItem  = GetItem(i);
		if(pItem)
		{
			if(pItem->GetTaxiwaySeg()->IsActive(NULL))
			{
				lastActiveSeg = i;	
			}else
			{
				if( !pItem->GetTaxiwaySeg()->TryActivate() ) //can be active
				{
					break;
				}
			}
		}
	}
	if(i == GetItemCount()) // passed all try activate
		lastActiveSeg = i;

	for(i= GetFirstUnDoneItemId();i < lastActiveSeg; i++)
	{
		PlanGroundRouteItem * pItem = GetItem(i);
		if(pItem)
		{
			//pItem->GetTaxiwaySeg()->Activate();
		}
	}
}

bool PlanGroundRoute::FindClearanceInRadiusofConcern( AirsideFlightInSim * pFlight, Clearance& clearace )
{
	if(pFlight->GetMode() < OnHeldAtStand)
	{
		for(int i=0 ;i <(int) m_vRoute.size(); i++)
		{
			PlanGroundRouteItem item = m_vRoute.at(i);
			clearace.AddItem(ClearanceItem(item.GetTaxiwaySeg(),OnTaxiToStand));
			
			//add end item
			if(m_vRoute.size()-1 == i){
				TaxiwayDirectSegInSim * lastSeg = item.GetTaxiwaySeg();
				clearace.AddItem(ClearanceItem(lastSeg,OnTaxiToStand,lastSeg->GetEndDist()));
			}
		}
		return false;
	}
	
	if(pFlight->GetMode() >= OnHeldAtStand)
	{
		for(int i=0 ;i <(int) m_vRoute.size(); i++)
		{
			PlanGroundRouteItem item = m_vRoute.at(i);
			clearace.AddItem(ClearanceItem(item.GetTaxiwaySeg(),OnTaxiToRunway));
			
			//add end item
			if(m_vRoute.size()-1 == i){
				TaxiwayDirectSegInSim * lastSeg = item.GetTaxiwaySeg();
				clearace.AddItem(ClearanceItem(lastSeg,OnTaxiToRunway,lastSeg->GetEndDist()));
			}
		}
		return false;
	}
		
	return true;
}

FlightPlanningFlowInSim::FlightPlanningFlowInSim()
{
	m_pStand = NULL;
	m_pArrivalAirRoute = NULL;
	m_pDepartureAirRoute = NULL;
	m_pSID = NULL;
	m_pSTAR = NULL;
	m_pLandingRunwayPort = NULL;
	m_pTakeoffRunwayPort = NULL;

}
