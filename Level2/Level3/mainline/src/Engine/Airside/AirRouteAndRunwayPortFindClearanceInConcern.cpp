#include "StdAfx.h"
#include ".\airrouteandrunwayportfindclearanceinconcern.h"
#include "AirspaceResource.h"
#include "RunwayInSim.h"
#include "AirsideFlightInSim.h"
#include "Clearance.h"
#include "FlightPerformanceManager.h"
#include "AirTrafficController.h"

AirRouteAndRunwayPortFindClearanceInConcern::AirRouteAndRunwayPortFindClearanceInConcern(void)
{
}

AirRouteAndRunwayPortFindClearanceInConcern::~AirRouteAndRunwayPortFindClearanceInConcern(void)
{
}

bool AirRouteAndRunwayPortFindClearanceInConcern::FindClearanceInConcern(AirsideFlightInSim * pFlight,  
																		 ClearanceItem& lastItem ,DistanceUnit radius, Clearance& newClearance)
{
	//AirRouteInSim *pStar            = pFlight->GetSTAR();
	//RunwayPortInSim   *pRunwayPortInSim = pFlight->GetLandingRunway();

	//ASSERT(pRunwayPortInSim);
	//
	//// get items
	//std::vector<ClearanceItem> vClearanceItems;
	//if ( lastItem.GetMode() < OnFinalApproach )
	//{
	//	//the part of star before OnFinalApproach
	//	if (NULL!=pStar && 1<pStar->GetItemCount())
	//	{
	//		int nBeginItemIndex = pStar->GetItemIndex(lastItem.GetResource());

	//		//the flight is in star
	//		if (0 <= nBeginItemIndex)
	//		{
	//			AirRouteItemInSim parItem = pStar->GetItem(nBeginItemIndex);
	//			ClearanceItem  exitClearanceItem = lastItem;
	//			exitClearanceItem.SetSpeed(parItem.m_vSpeed);
	//			exitClearanceItem.SetMode( parItem.m_mode );
	//			exitClearanceItem.SetDistInResource(parItem.m_distTo);
	//			exitClearanceItem.SetAltitude(parItem.GetAltitude(parItem.m_distTo));

	//			vClearanceItems.push_back(exitClearanceItem);
	//		}

	//		//the part of star before OnFinalApproach 
	//		for (int i=nBeginItemIndex+1; i<pStar->GetItemCount()-1; i++)
	//		{
	//			AirRouteItemInSim parItem = pStar->GetItem(i);
	//			AirsideResource   *parItemResource = parItem.m_pRes;
	//			
	//			//enter clearanceItem
	//			ClearanceItem enterItem(parItem.m_pRes, parItem.m_mode, parItem.m_distFrom);
	//			enterItem.SetSpeed(parItem.m_vSpeed);
	//			enterItem.SetAltitude(parItem.GetAltitude(enterItem.GetDistInResource()));

	//			vClearanceItems.push_back(enterItem);

	//			//exit clearanceItem
	//			ClearanceItem exitItem(parItem.m_pRes, parItem.m_mode, parItem.m_distTo);
	//			exitItem.SetSpeed(parItem.m_vSpeed);
	//			exitItem.SetAltitude(parItem.GetAltitude(exitItem.GetDistInResource()));

	//			vClearanceItems.push_back(exitItem);
	//		}
	//	}

	//	if (NULL!=pStar && 0<pStar->GetItemCount())
	//	{
	//		//OnFinalApproach

	//		AirsideResource*pFinalApproachResource = pStar->GetItem(pStar->GetItemCount()-1);

	//		ClearanceItem finalApproachEnterItem(pFinalApproachResource, OnFinalApproach, 0);
	//		finalApproachEnterItem.SetSpeed(finalApproachAirRouteItemInSim.m_vSpeed);
	//		finalApproachEnterItem.SetAltitude(finalApproachAirRouteItemInSim.GetAltitude(finalApproachEnterItem.GetDistInResource()));

	//		vClearanceItems.push_back(finalApproachEnterItem);
	//	}
	//	
	//	////landing item
	//	//double dLandSped = pFlight->GetPerformance()->getLandingSpeed(pFlight);
	//	//DistanceUnit touchDist = pFlight->GetPerformance()->getDistThresToTouch(pFlight);
	//	//ClearanceItem landingItem(pRunwayPortInSim,OnLanding,touchDist);		
	//	//landingItem.SetSpeed(dLandSped);

	//	//vClearanceItems.push_back(landingItem);			
	//}

	////get time of each item
	//{
	//	ClearanceItem preItem = lastItem;
	//	for(int i = 0;i< (int)vClearanceItems.size();i++)
	//	{
	//		ElapsedTime dT = pFlight->GetTimeBetween(preItem, vClearanceItems.at(i));
	//		//
	//		ElapsedTime nextTime = preItem.GetTime() + dT;
	//		vClearanceItems[i].SetTime(nextTime);
	//		preItem = vClearanceItems.at(i);
	//	}
	//}

	//{  // set delay time
	//	
	//	for( int i =vClearanceItems.size()-1; i>=0;i--)
	//	{
	//		ClearanceItem& theItem = vClearanceItems[i];
	//		
	//		OccupancyInstance preFlightOcy = theItem.GetResource()->GetLastOccupyInstance();
	//		if(preFlightOcy.IsValid() && preFlightOcy.IsEnterTimeValid())
	//		{
	//			ElapsedTime lastFlightEnterT = preFlightOcy.GetEnterTime();
	//			AirsideFlightInSim * preFlight = preFlightOcy.GetFlight();
	//			ElapsedTime separationT =  pFlight->GetAirTrafficController()->GetSeparationTime(preFlight,theItem.GetMode(),pFlight,theItem.GetMode());
 //               
	//			if(theItem.GetTime() - lastFlightEnterT < separationT)//delay
	//			{
	//				ElapsedTime delayT = separationT + lastFlightEnterT - theItem.GetTime();
	//				for(int j=i; j>=0;j--) // add delay time for pre items 
	//				{
	//					vClearanceItems[j].SetDelayTime(vClearanceItems[j].GetDelayTime() + delayT );
	//					vClearanceItems[j].SetTime( vClearanceItems[j].GetTime() + delayT );
	//				}
	//			}
	//		}

	//	}

	//}
	//

	//for(int i =0 ;i<(int) vClearanceItems.size(); i++)
	//{
	//	lastItem = vClearanceItems[i];
	//	newClearance.AddItem(lastItem);
	//}

	//if(vClearanceItems.size())
	//{
	//	return true;
	//}
	//else 
		return false;

}