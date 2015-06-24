#include "stdafx.h"
#include "FlightOnToDeice.h"

#include <limits>

#include "AirsideFlightInSim.h"
#include "Clearance.h"
#include "TaxiRouteInSim.h"
#include "./DeiceVehicleServiceRequest.h"
#include "FlightPerformanceManager.h"
#include "DynamicConflictGraph.h"
#include "DeiceResourceManager.h"
#include "DeicepadGroup.h"
#include "FlightGroundRouteResourceManager.h"
#include "./VehicleRequestDispatcher.h"
#include "../ARCportEngine.h"
#include "AirsideSimulation.h"
//flight call for deice
bool FlightOnToDeice::FindClearanceInConcern( AirsideFlightInSim * pFlight, FlightGroundRouteResourceManager* pResource, ClearanceItem& lastItem ,Clearance& newClearance )
{
	FlightDeiceStrategyDecision& deiceStragy = pFlight->GetDeiceDecision();
	if(deiceStragy.m_deiceRoute && lastItem.GetResource() != deiceStragy.m_deicePlace &&
		!(deiceStragy.m_deicePlace && deiceStragy.m_deicePlace->IsKindof(typeof(DeicePadInSim))) ) //still on route 
	{
		bool btillEnd =  deiceStragy.m_deiceRoute->FindClearanceInConcern(pFlight,lastItem,50000, newClearance);
		if(btillEnd)
			return btillEnd;
	}

	//out of the route wait at the resource for deice vehicle serviced
	if(lastItem.GetResource() != deiceStragy.m_deicePlace ) // at the deice entry
	{
		if (deiceStragy.m_deicePlace == NULL)
		{
			DeicePadInSim* pDeicepad = deiceStragy.m_pDeicepadGroup->AssignDeicepad(pFlight);
			if (pDeicepad)
			{
				deiceStragy.m_pDeicepadGroupEntry->SetFlightWaiting(NULL);
// 				deiceStragy.m_pDeicepadGroup->RemoveApproachingFlight(pFlight);
				deiceStragy.m_deicePlace = pDeicepad;
				pFlight->GetServiceRequestDispatcher()->ServiceRequestDispatch();
				FlightGroundRouteDirectSegList segList;
				double dWeight = (std::numeric_limits<double>::max)();
				if (pResource->GetRoute(deiceStragy.m_pDeicepadGroupEntry->GetIntersectionNode(),
					pDeicepad->m_pEntryNode, pFlight, 0, segList, dWeight))
				{
					TaxiRouteInSim* pRouteInDeiceGrp = new TaxiRouteInSim(pFlight->GetMode(),
						deiceStragy.m_pDeicepadGroupEntry->GetIntersectionNode(), pDeicepad->m_pEntryNode);
					bool bCyclicGroundRoute=pFlight->GetEngine()->GetAirsideSimulation()->AllowCyclicGroundRoute();
					pRouteInDeiceGrp->AddTaxiwaySegList(segList,bCyclicGroundRoute);
					pDeicepad->SetFlightOnto(pFlight);
					pDeicepad->SetApproachingRoute(pRouteInDeiceGrp);
					bool btillEnd = pRouteInDeiceGrp->FindClearanceInConcern(pFlight,lastItem,50000, newClearance);
					if (btillEnd)
						return btillEnd;
					else
					{ //at the end of route 
						DistanceUnit distInPad = pDeicepad->GetInPath().GetTotalLength()-pFlight->GetWingspan()*0.5;
						ClearanceItem atDeiceplaceItem(deiceStragy.m_deicePlace, OnTaxiToDeice,distInPad);
						ElapsedTime dT = pFlight->GetTimeBetween(lastItem,atDeiceplaceItem);
						atDeiceplaceItem.SetTime( lastItem.GetTime() + dT);
						lastItem = atDeiceplaceItem;
						deiceStragy.m_deiceRoute->FlightExitRoute(pFlight,atDeiceplaceItem.GetTime());
						pRouteInDeiceGrp->FlightExitRoute(pFlight, atDeiceplaceItem.GetTime());
						newClearance.AddItem(lastItem);		
						return true;
					}
				}
			}
			else
			{
				ASSERT(deiceStragy.m_pDeicepadGroupEntry->GetFlightWaiting() == NULL);
				deiceStragy.m_pDeicepadGroupEntry->SetFlightWaiting(pFlight);
				return true; // no deicepad available, waiting...
			}
		}
		else if (deiceStragy.m_deicePlace->IsKindof(typeof(DeicePadInSim)))
		{
			DeicePadInSim* pDeicepad = (DeicePadInSim*)deiceStragy.m_deicePlace;
			TaxiRouteInSim* pRouteInDeiceGrp = pDeicepad->GetApproachingRoute();
			ASSERT(pRouteInDeiceGrp);
			bool btillEnd = pRouteInDeiceGrp->FindClearanceInConcern(pFlight,lastItem,50000, newClearance);
			if (btillEnd)
			{
				deiceStragy.m_deiceRoute->FlightExitRoute(pFlight,lastItem.GetTime());
				return btillEnd;
			}
			else
			{ //at the end of route 
				DistanceUnit distInPad = pDeicepad->GetInPath().GetTotalLength()-pFlight->GetWingspan()*0.5;
				ClearanceItem atDeiceplaceItem(deiceStragy.m_deicePlace, OnTaxiToDeice,distInPad);
				ElapsedTime dT = pFlight->GetTimeBetween(lastItem,atDeiceplaceItem);
				atDeiceplaceItem.SetTime( lastItem.GetTime() + dT);
				lastItem = atDeiceplaceItem;
				deiceStragy.m_deiceRoute->FlightExitRoute(pFlight,lastItem.GetTime());
				pRouteInDeiceGrp->FlightExitRoute(pFlight, atDeiceplaceItem.GetTime());
				newClearance.AddItem(lastItem);		
				return true;
			}

		}
	}
	if(lastItem.GetResource() == deiceStragy.m_deicePlace)
	{			
		DeiceVehicleServiceRequest* pRequest = pFlight->getDeiceServiceRequest();			

		if( pRequest && pRequest->GetServiceVehicleCount() && pFlight->IsGeneralVehicleAndPaxbusServiceCompleted())
		{
			pFlight->SetReadyForDeice(true);
			pFlight->SetParkingAtDeice(false);
			pRequest->flightCallDeice(lastItem.GetTime());
			pRequest->flightCallAntiIce(lastItem.GetTime());
			return true;
		}
		else //leave deice to other phase
		{
			pFlight->SetParkingAtDeice(true);
			ElapsedTime extraTime = deiceStragy.m_InspecAndHoldTime;
			lastItem.SetTime(lastItem.GetTime()+extraTime);
			newClearance.AddItem(lastItem);	
			pFlight->StartDelay(lastItem, NULL, FlightConflict::STOPPED, FlightConflict::VEHICLE,FltDelayReason_Service,_T("Deice Service delayed"));
			pFlight->RemoveServiceReques(pRequest);
			
			return false;
		}
	}
	
	return false;
}
