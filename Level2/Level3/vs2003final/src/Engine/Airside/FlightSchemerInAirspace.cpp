#include "StdAfx.h"
#include ".\flightschemerinairspace.h"
#include "Clearance.h"
#include "AirspaceResource.h"
#include "ConflictConcernIntersectionInAirspace.h"

#include "AirsideFlightInSim.h"
#include "AirTrafficController.h"
#include "FlightPerformanceManager.h"
#include "FlightRunwayTakeOffEvent.h"
#include "FlightConflictAtWaypointDelay.h"
#include "../../Results/OutputAirside.h"
#include "HeadingAirRouteSegInSim.h"


FlightSchemerInAirspace::FlightSchemerInAirspace(void)
{
	m_pAirspaceConflictConcern = NULL;
}

FlightSchemerInAirspace::~FlightSchemerInAirspace(void)
{
}

bool FlightSchemerInAirspace::FlightSchemeOnAirRoute(AirsideFlightInSim* pFlight,FlightRouteInSim *pAirRouteInSim, AirsideResourceList& ResourceList,ClearanceItem& lastItem,Clearance& newClearance)
{	
	CRunwaysController *pRunwaysController =  pFlight->GetAirTrafficController()->GetRunwaysController();
	std::vector<CRunwaysController::RunwayOccupyInfo> vRunwayArrangeInfo;
	vRunwayArrangeInfo.clear();
	bool bNeedCreateTakeOffEvent = false;
	ElapsedTime eTakeOffTime = ElapsedTime(0L);
	
	ClearanceItem preItem = lastItem;
	//take off	
	if (pFlight->GetMode() >= OnHeldAtStand)
	{	
		ASSERT(pFlight->GetUID() != 808);
		ASSERT(pFlight->GetUID() != 605);
		ClearanceItem foreItem = lastItem;
		TimeRange eTakeOffTimeRange(ElapsedTime(0L),ElapsedTime(0L));
		ElapsedTime dTimeFromTaxiwayToRunway = ElapsedTime(0L);
		{
			RunwayExitInSim * pTakeoffPos = pFlight->GetTakeoffPosition();		
			DistanceUnit takeoffPos = 0; 
			if(pTakeoffPos)
			{
				takeoffPos = pTakeoffPos->GetEnterRunwayPos();
				TaxiwayDirectSegInSim * pSeg = pTakeoffPos->GetTaxiwaySeg(); // add at the end of the taxiway item
				if(pSeg && pSeg->GetOppositeSegment())
				{
					TaxiwayDirectSegInSim * pLastSeg  = pSeg->GetOppositeSegment();
					ClearanceItem lastInSegItem(pLastSeg,OnTaxiToRunway,pLastSeg->GetExitHoldDist());
					lastInSegItem.SetSpeed(pFlight->GetPerformance()->getTaxiOutSpeed(pFlight));
					dTimeFromTaxiwayToRunway = pFlight->GetTimeBetween(lastItem, lastInSegItem);
				}
			}

		}
		pRunwaysController->ApplyForTakeoffTime(pFlight,pFlight->GetTime() + dTimeFromTaxiwayToRunway ,eTakeOffTimeRange,vRunwayArrangeInfo);
		Clearance takeOffClearce;
		pRunwaysController->GetFlightTakeOffClearance(pFlight,eTakeOffTimeRange.GetStartTime(),foreItem,takeOffClearce);
		//get lift off time
		ElapsedTime eLiftNeedTime;
		if(takeOffClearce.GetItemCount()>0)
		{
			eLiftNeedTime = takeOffClearce.ItemAt(takeOffClearce.GetItemCount()-1).GetTime() - preItem.GetTime();
		}
		//take off
		//eTakeOffTimeRange.SetStartTime(eTakeOffTimeRange.GetStartTime() + eLiftNeedTime);
		foreItem.SetTime(eTakeOffTimeRange.GetStartTime() + eLiftNeedTime);
		if (eTakeOffTimeRange.GetEndTime() > ElapsedTime(0L))
		{
			eTakeOffTimeRange.SetEndTime(eTakeOffTimeRange.GetEndTime() + eLiftNeedTime);
		}

		FlightSchemeOnAirRouteTakeOff(pFlight,pAirRouteInSim,ResourceList,foreItem,eTakeOffTimeRange,vRunwayArrangeInfo);
		
		ElapsedTime eFlightTime = eTakeOffTimeRange.GetStartTime();	

		if (lastItem.GetTime() + dTimeFromTaxiwayToRunway < eFlightTime)
		{
			bNeedCreateTakeOffEvent = true;
			eTakeOffTime = eFlightTime - dTimeFromTaxiwayToRunway;
		}
		preItem.SetTime(eFlightTime- dTimeFromTaxiwayToRunway);
		pRunwaysController->WriteRunwayTakeOffLogs(pFlight,vRunwayArrangeInfo,eFlightTime- dTimeFromTaxiwayToRunway,preItem,newClearance);
		preItem = newClearance.GetItem(newClearance.GetItemCount() -1);


	}
	
	TimeRange eLandingTimeRange(ElapsedTime(0L),ElapsedTime(0L));
	//landing
	if (pFlight->GetMode() < OnTaxiToStand)
	{
		ElapsedTime eTotalTime = CalculateOptimisticTimeThrouhRoute(pFlight,pAirRouteInSim,ResourceList,0);
		pRunwaysController->ApplyForLandingTime(pFlight,pFlight->GetTime() + eTotalTime,eLandingTimeRange,vRunwayArrangeInfo);

		FlightSchemeOnAirRoute(pFlight,pAirRouteInSim,ResourceList,lastItem,eLandingTimeRange,pFlight->GetTime(),vRunwayArrangeInfo);
	}



	int nCount = (int)ResourceList.size();
	for (int i =0; i < nCount; i++)
	{
		AirsideResource* pResource = ResourceList.at(i);
		if (pResource->GetType() == AirsideResource::ResType_HeadingAirRouteSegment)
		{
			((HeadingAirRouteSegInSim*)pResource)->CalculateClearanceItems(pFlight,preItem,ResourceList.at(i+1),newClearance);
		}

		if (pResource->GetType() == AirsideResource::ResType_AirRouteSegment)
		{
			AirRoutePointInSim* pFirstIntersection = ((AirRouteSegInSim*)pResource)->GetFirstConcernIntersection()->getInputPoint();
			FlightPlanPropertyInSim property = pAirRouteInSim->GetRoutePointProperty(pFirstIntersection);
			OccupancyInstance instance = pFirstIntersection->GetOccupyInstance(pFlight);
			ElapsedTime tDelayAtPoint = instance.GetEnterTime() - preItem.GetTime();

			int nDelayId = -1;
			if (preItem.GetResource() == NULL && tDelayAtPoint > 0L)		//delay at first waypoint when arrival
			{
				FlightConflictAtWaypointDelay* pDelay = new FlightConflictAtWaypointDelay;
				//pDelay->SetAtObjectID(pFirstIntersection->getID());
				pDelay->SetFlightID(pFlight->GetUID());
				pDelay->SetDelayResult(FltDelayReason_AirHold);
				pDelay->SetDelayTime(tDelayAtPoint);
				pDelay->SetStartTime(preItem.GetTime());
				if (pFlight->GetOutput())
				{
					nDelayId = pFlight->GetOutput()->LogFlightDelay(pDelay);
				}
			}

			ClearanceItem newItem(pFirstIntersection,property.GetMode(),0);
			newItem.SetPosition(pFirstIntersection->GetPosition());
			newItem.SetAltitude(property.GetAlt());
			newItem.SetSpeed(instance.GetSpeed());
			newItem.SetTime(instance.GetExitTime());
			if (nDelayId > -1)
				newItem.SetDelayId(nDelayId);

			preItem = newItem;
			newClearance.AddItem(preItem);



			ElapsedTime tExit = -1L;
			if (((AirRouteSegInSim*)pResource)->GetSecondConcernIntersection())
			{
				AirRoutePointInSim* pSecondIntersection = ((AirRouteSegInSim*)pResource)->GetSecondConcernIntersection()->getInputPoint();
				tExit = pSecondIntersection->GetOccupyInstance(pFlight).GetEnterTime();
			}
			if(tExit < 0L)
				tExit = preItem.GetTime() + pAirRouteInSim->GetFlightTimeInSeg(pFlight,(AirRouteSegInSim*)pResource);

			((AirRouteSegInSim*)pResource)->CalculateClearanceItems(pFlight,pAirRouteInSim,preItem,tExit,newClearance);

			if ( ((AirRouteSegInSim*)pResource)->GetSecondConcernIntersection())
			{
				AirRoutePointInSim* pSecondIntersection = ((AirRouteSegInSim*)pResource)->GetSecondConcernIntersection()->getInputPoint();
				FlightPlanPropertyInSim Pointprop = pAirRouteInSim->GetRoutePointProperty(pSecondIntersection);
				ClearanceItem secondItem(pSecondIntersection,Pointprop.GetMode(),0);
				secondItem.SetPosition(pSecondIntersection->GetPosition());
				secondItem.SetAltitude(Pointprop.GetAlt());

				instance = pSecondIntersection->GetOccupyInstance(pFlight);
				if (instance.GetFlight())
				{
					secondItem.SetSpeed(instance.GetSpeed());
					secondItem.SetTime(instance.GetExitTime());
				}
				else
				{
					secondItem.SetSpeed(Pointprop.GetSpeed());
					secondItem.SetTime(tExit);
				}
				preItem = secondItem;
				newClearance.AddItem(preItem);					
			}
		}

		if (pResource->GetType() == AirsideResource::ResType_RunwayPort && pFlight->GetMode() < OnTaxiToStand)			//runway of star
		{
			
			pRunwaysController->WriteRunwayLogs(pFlight,vRunwayArrangeInfo,preItem.GetTime(),preItem,newClearance);

		}
	}

	if (bNeedCreateTakeOffEvent)
	{
		//wait log
		ClearanceItem waitItem = lastItem;
		waitItem.SetTime(eTakeOffTime);

		ElapsedTime tDelay = eTakeOffTime - lastItem.GetTime();
		if (tDelay > 0L)		//delay when takeoff
		{
			int nDelayId = -1;
			FlightConflictAtWaypointDelay* pDelay = new FlightConflictAtWaypointDelay;
			if (pFlight->GetTakeoffRunway())
			{
				int nRunwayID = pFlight->GetTakeoffRunway()->GetRunwayInSim()->GetRunwayInput()->getID();
				pDelay->SetFlightID(pFlight->GetUID());
				pDelay->SetDelayResult(FltDelayReason_AirHold);
				pDelay->SetDelayTime(tDelay);
				pDelay->SetStartTime(lastItem.GetTime());
				if (pFlight->GetOutput())
				{
					nDelayId = pFlight->GetOutput()->LogFlightDelay(pDelay);
				}
			}
			waitItem.SetDelayId(nDelayId);
		}

		pFlight->PerformClearanceItem(waitItem);
		
		Clearance logclearance;
		logclearance.AddItem(waitItem);




		
		{
			Clearance clearance = logclearance;
			char fltstr[255];
			pFlight->getFullID(fltstr);
			//log to flight file
			CString debugfileName;
			debugfileName.Format( "C:\\FlightDebug\\%s(%d).txt", fltstr,pFlight->GetUID() );
			std::ofstream outfile;
			outfile.open(debugfileName,std::ios::app);
			outfile << pFlight->GetTime()<<"," << long(pFlight->GetTime())<<std::endl;
			outfile << pFlight->GetPosition().getX() <<" ," <<pFlight->GetPosition().getY() << std::endl;
			outfile << clearance;
			outfile.close();
			//log to all file
			CString allfile = "C:\\FlightDebug\\AllFlight.txt";
			std::ofstream alloutfile;
			alloutfile.open(allfile,std::ios::app);		
			alloutfile<<fltstr<<"("<<pFlight->GetUID()<<"),"<<pFlight->GetTime()<<"," << long(pFlight->GetTime())<<std::endl;
			alloutfile<<clearance;		
			alloutfile.close();
		}
		
		//create take off event, take off at takeoff time
		CFlightRunwayTakeOffEvent * pTakeOffEvent = new CFlightRunwayTakeOffEvent(pFlight,newClearance);
		pTakeOffEvent->setTime(eTakeOffTime);
		pFlight->SetTime(eTakeOffTime);
		pTakeOffEvent->addEvent();

		newClearance.ClearItems();
	}

	return true;
}
bool FlightSchemerInAirspace::FlightSchemeOnAirRoute(AirsideFlightInSim* pFlight,FlightRouteInSim *pAirRouteInSim,AirsideResourceList& ResourceList,ClearanceItem& lastItem,TimeRange& eLandingTimeRange,
													 ElapsedTime tNewBirthTime,std::vector<CRunwaysController::RunwayOccupyInfo>& vRunwayArrangeInfo)
{	
	ElapsedTime tDelay = 0L;
	CRunwaysController *pRunwaysController =  pFlight->GetAirTrafficController()->GetRunwaysController();

	size_t nSegmentCount = ResourceList.size();

	// the last resource is runway port resource
	RunwayPortInSim *pRunwayPort = pFlight->GetLandingRunway();
	ClearanceItem runwayItem( pRunwayPort,OnLanding,0);
	runwayItem.SetTime(eLandingTimeRange.GetStartTime());
	runwayItem.SetSpeed(pFlight->GetPerformance()->getLandingSpeed(pFlight));
	runwayItem.SetAltitude(0.0);
	lastItem = runwayItem;
	//ClearanceItem nextItem = runwayItem;
	
	//initialization
	std::vector<ClearanceItem > vClearanceItem;
	std::vector<SegmentTimeInfo> vSegTimeInfo;
	for (size_t i = 0; i< nSegmentCount; ++i)
	{
		vClearanceItem.push_back(ClearanceItem(NULL,OnBirth,0.0));
		vSegTimeInfo.push_back(SegmentTimeInfo());
	}

	//write the runway port info
	vClearanceItem[nSegmentCount-1] = lastItem;
	vSegTimeInfo[nSegmentCount-1].minTime = eLandingTimeRange.GetStartTime();
	vSegTimeInfo[nSegmentCount-1].maxTime = eLandingTimeRange.GetEndTime();
	vSegTimeInfo[nSegmentCount-1].curTime = eLandingTimeRange.GetStartTime();
	


	int nSeg = (int)nSegmentCount -2;
	while (nSeg >= 0)
	{	
		AirsideResource* pNextRes = ResourceList.at(nSeg);
		AirsideResource* pLastSeg = ResourceList.at(nSeg+1);
		ElapsedTime eTimeEstmated = -1L;

		if (pNextRes->GetType() == AirsideResource::ResType_HeadingAirRouteSegment)
		{
			HeadingAirRouteSegInSim* pNextSeg = (HeadingAirRouteSegInSim*)pNextRes;

			ASSERT(nSeg != (int)nSegmentCount - 2);	//the heading segment shouldn't be last segment

			ClearanceItem newItem = lastItem;
		
			bool bFit = pNextSeg->CalculateFlightClearance(pFlight,pLastSeg,pAirRouteInSim,lastItem,newItem,tNewBirthTime);
			if (bFit)
			{
				vSegTimeInfo[nSeg].curTime = newItem.GetTime();
				vClearanceItem[nSeg] = newItem;
				lastItem = newItem;

				nSeg = nSeg -1;
				continue;
			}

			eTimeEstmated = newItem.GetTime();
			pNextSeg->GetFirstIntersection()->GetAvailableTimeAtIntersection(pFlight,pNextRes,eTimeEstmated,vSegTimeInfo[nSeg].minTime,vSegTimeInfo[nSeg].maxTime);
	
			tDelay = vSegTimeInfo[nSeg].minTime - eTimeEstmated;
		}
		else
		{
			AirRouteSegInSim* pCurrentSegment =(AirRouteSegInSim *)pNextRes;

			ElapsedTime eSegTime;
			if (nSeg == (int)nSegmentCount -2)//the last segment Item 
			{
				eSegTime = CalculateTimeFromLastSegToRunwayPort(pFlight,pAirRouteInSim,pCurrentSegment);
			}
			else
			{
				eSegTime = pAirRouteInSim->GetFlightTimeInSeg(pFlight,pCurrentSegment);
			}

			AirRouteIntersectionInSim* pConflictIntersection = pCurrentSegment->GetFirstConcernIntersection();

			ClearanceItem newItem(pConflictIntersection->getInputPoint(),OnBirth,0.0);
			newItem.SetTime(lastItem.GetTime() - eSegTime);
		
			double dSpeed = pAirRouteInSim->GetRoutePointProperty(pConflictIntersection->getInputPoint()).GetSpeed();
			newItem.SetSpeed(dSpeed);

			bool bFit = pConflictIntersection->ConverseCalculateClearanceAtIntersection(pFlight,pLastSeg,pCurrentSegment,pAirRouteInSim,lastItem,newItem);
			if (bFit)
			{
				pConflictIntersection->GetAvailableTimeAtIntersection(pFlight,pCurrentSegment,lastItem.GetTime() - eSegTime,vSegTimeInfo[nSeg].minTime,vSegTimeInfo[nSeg].maxTime);
				vSegTimeInfo[nSeg].curTime = newItem.GetTime();
				vClearanceItem[nSeg] = newItem;
				lastItem = newItem;

				nSeg = nSeg -1;
				continue;
			}

			eTimeEstmated = lastItem.GetTime() - eSegTime;
			pConflictIntersection->GetAvailableTimeAtIntersection(pFlight,pCurrentSegment,eTimeEstmated,vSegTimeInfo[nSeg].minTime,vSegTimeInfo[nSeg].maxTime);
			
			tDelay = vSegTimeInfo[nSeg].minTime - eTimeEstmated;				
		}

		//adjust the time range
		if (eTimeEstmated <= vSegTimeInfo[nSeg].minTime)
		{
			if (AdjustMinTime(vSegTimeInfo,vClearanceItem,nSeg))
			{

				nSeg = nSeg -1;
				continue;			
			}
			else
			{
				//recalculate
				ElapsedTime nSegsTime = CalculateOptimisticTimeThrouhRoute(pFlight,pAirRouteInSim,ResourceList,0);
				ElapsedTime eEstmatedRunwayTime =  tNewBirthTime  + nSegsTime + tDelay;

				pRunwaysController->ApplyForLandingTime(pFlight,eEstmatedRunwayTime,eLandingTimeRange,vRunwayArrangeInfo);

				FlightSchemeOnAirRoute(pFlight,pAirRouteInSim,ResourceList,lastItem,eLandingTimeRange,tNewBirthTime + tDelay,vRunwayArrangeInfo);

				break;

			}
		}
		else if(eTimeEstmated >= vSegTimeInfo[nSeg].maxTime)
		{
			if (AdjustMaxTime(vSegTimeInfo,vClearanceItem,nSeg))
			{
				nSeg = nSeg -1;
				continue;	
			}
			else
			{	
				ElapsedTime nSegsTime = CalculateOptimisticTimeThrouhRoute(pFlight,pAirRouteInSim,ResourceList,0);
				ElapsedTime eEstmatedRunwayTime =  tNewBirthTime + nSegsTime + tDelay;
				pRunwaysController->ApplyForLandingTime(pFlight,eEstmatedRunwayTime,eLandingTimeRange,vRunwayArrangeInfo);

				FlightSchemeOnAirRoute(pFlight,pAirRouteInSim,ResourceList,lastItem,eLandingTimeRange,tNewBirthTime + tDelay,vRunwayArrangeInfo);

				break;
			}

		}
	}

    
	return true;

}
bool FlightSchemerInAirspace::FlightSchemeOnAirRouteTakeOff(AirsideFlightInSim* pFlight,FlightRouteInSim *pAirRouteInSim,AirsideResourceList& ResourceList,ClearanceItem& lastItem, TimeRange& eTakeingOffTime,std::vector<CRunwaysController::RunwayOccupyInfo>& vRunwayArrangeInfo)
{

	size_t nSegmentCount = ResourceList.size();

	// the last resource is runway port resource
	RunwayPortInSim *pRunwayPort = pFlight->GetLandingRunway();
	ClearanceItem runwayItem( pRunwayPort,OnLanding,0);
	runwayItem.SetTime(eTakeingOffTime.GetStartTime());
	runwayItem.SetSpeed(pFlight->GetLiftoffSpeed());
	runwayItem.SetAltitude(0.0);


	std::vector<ClearanceItem > vClearanceItem;
	std::vector<SegmentTimeInfo> vSegTimeInfo;
	for (size_t i = 0; i< nSegmentCount; ++i)
	{
		vClearanceItem.push_back(ClearanceItem(NULL,OnBirth,0.0));
		vSegTimeInfo.push_back(SegmentTimeInfo());
	}

	//write the runway port info
	vClearanceItem[0] = lastItem;
	vSegTimeInfo[0].minTime = eTakeingOffTime.GetStartTime();
	vSegTimeInfo[0].maxTime = eTakeingOffTime.GetEndTime();
	vSegTimeInfo[0].curTime = eTakeingOffTime.GetStartTime();
	


	int nSeg = 1;//start from the second one
	ClearanceItem preItem = lastItem;
	while (nSeg < (int)nSegmentCount)
	{	
		AirRouteSegInSim* pCurrentSegment =(AirRouteSegInSim *)ResourceList.at(nSeg);
		AirsideResource* pLastSeg = ResourceList.at(nSeg -1);
		ElapsedTime eSegTime;

		if (nSeg == 1)//the first segment with only one waypoint
		{
			eSegTime = CalculateTimeFromFirstSegToRunwayPortTakeOff(pFlight,pAirRouteInSim,pCurrentSegment);
		}
		else
		{
			eSegTime = pAirRouteInSim->GetFlightTimeInSeg(pFlight,(AirRouteSegInSim*)pLastSeg);
		}
		AirRouteIntersectionInSim* pConflictIntersection = NULL;

		pConflictIntersection = pCurrentSegment->GetFirstConcernIntersection();

		ClearanceItem newItem(pConflictIntersection->getInputPoint(),OnTakeoff,0.0);

		newItem.SetTime(preItem.GetTime() + eSegTime);
		
		double dSpeed = pAirRouteInSim->GetRoutePointProperty(pConflictIntersection->getInputPoint()).GetSpeed();
		newItem.SetSpeed(dSpeed);


		bool bFit = pConflictIntersection->CalculateClearanceAtIntersection(pFlight,pLastSeg,pCurrentSegment,pAirRouteInSim,preItem,newItem);
		if (bFit )
		{
			pConflictIntersection->GetAvailableTimeAtIntersection(pFlight,pCurrentSegment,lastItem.GetTime() + eSegTime,vSegTimeInfo[nSeg].minTime,vSegTimeInfo[nSeg].maxTime);
			vSegTimeInfo[nSeg].curTime = newItem.GetTime();
			vClearanceItem[nSeg] = newItem;

			nSeg = nSeg +1;
			continue;
		}
		else
		{
			ElapsedTime eTimeEstmated = lastItem.GetTime() + eSegTime;

			pConflictIntersection->GetAvailableTimeAtIntersection(pFlight,pCurrentSegment,eTimeEstmated,vSegTimeInfo[nSeg].minTime,vSegTimeInfo[nSeg].maxTime);
			
			
			//adjust the time range
			if (eTimeEstmated <= vSegTimeInfo[nSeg].minTime)
			{
				if (AdjustMinTime(vSegTimeInfo,vClearanceItem,nSeg))
				{

					nSeg = nSeg +1;
					continue;			
				}
				else
				{
					//recalculate
					//ElapsedTime nSegsTime = CalculateOptimisticTimeThrouhRoute(pFlight,pAirRouteInSim,ResourceList,nSeg);
					//ElapsedTime eEstmatedRunwayTime = vSegTimeInfo[nSeg].minTime + nSegsTime+ ElapsedTime(1L);
					// 
					//CRunwaysController *pRunwaysController =  pFlight->GetAirTrafficController()->GetRunwaysController();

					//pRunwaysController->ApplyForTakeoffTime(pFlight,eEstmatedRunwayTime,eTakeingOffTime,vRunwayArrangeInfo);
					//Clearance takeOffClearce;
					//pRunwaysController->GetFlightTakeOffClearance(pFlight,eTakeingOffTime.GetStartTime(),lastItem,takeOffClearce);
					////get lift off time
					//ElapsedTime eLiftNeedTime;
					//if(takeOffClearce.GetItemCount()>0)
					//{
					//	eLiftNeedTime = takeOffClearce.ItemAt(takeOffClearce.GetItemCount()-1).GetTime() - takeOffClearce.ItemAt(0).GetTime();
					//}
					////take off
					//eTakeingOffTime.SetStartTime(eTakeingOffTime.GetStartTime() + eLiftNeedTime + ElapsedTime(1L));
					//if (eTakeingOffTime.GetEndTime() > ElapsedTime(0L))
					//{
					//	eTakeingOffTime.SetEndTime(eTakeingOffTime.GetEndTime() + eLiftNeedTime + ElapsedTime(1L));
					//}
					//FlightSchemeOnAirRouteTakeOff(pFlight,pAirRouteInSim,ResourceList,lastItem,eTakeingOffTime,vRunwayArrangeInfo);

					//break;
					ElapsedTime nSegsTime = CalculateOptimisticTimeThrouhRouteTakeOff(pFlight,pAirRouteInSim,ResourceList,nSeg);
					
					CRunwaysController *pRunwaysController =  pFlight->GetAirTrafficController()->GetRunwaysController();
					Clearance takeOffClearce;
					ClearanceItem lastItemCopy = lastItem;
					pRunwaysController->GetFlightTakeOffClearance(pFlight,eTakeingOffTime.GetStartTime(),lastItemCopy,takeOffClearce);
					//get lift off time
					ElapsedTime eLiftNeedTime;
					if(takeOffClearce.GetItemCount()>0)
					{
						eLiftNeedTime = takeOffClearce.ItemAt(takeOffClearce.GetItemCount()-1).GetTime() - takeOffClearce.ItemAt(0).GetTime();
					}


					ElapsedTime eEstmatedRunwayTime =  vSegTimeInfo[nSeg].minTime+ ElapsedTime(1L)  - nSegsTime- eLiftNeedTime;
					ElapsedTime dTimeFromTaxiwayToRunway = ElapsedTime(0L);
					{
						RunwayExitInSim * pTakeoffPos = pFlight->GetTakeoffPosition();		
						DistanceUnit takeoffPos = 0; 
						if(pTakeoffPos)
						{
							takeoffPos = pTakeoffPos->GetEnterRunwayPos();
							TaxiwayDirectSegInSim * pSeg = pTakeoffPos->GetTaxiwaySeg(); // add at the end of the taxiway item
							if(pSeg && pSeg->GetOppositeSegment())
							{
								TaxiwayDirectSegInSim * pLastSeg  = pSeg->GetOppositeSegment();
								ClearanceItem lastInSegItem(pLastSeg,OnTaxiToRunway,pLastSeg->GetExitHoldDist());
								lastInSegItem.SetSpeed(pFlight->GetPerformance()->getTaxiOutSpeed(pFlight));
								dTimeFromTaxiwayToRunway = pFlight->GetTimeBetween(lastItem, lastInSegItem);
							}
						}

					}

					pRunwaysController->ApplyForTakeoffTime(pFlight,eEstmatedRunwayTime + dTimeFromTaxiwayToRunway,eTakeingOffTime,vRunwayArrangeInfo);


					//take off
					eTakeingOffTime.SetStartTime(eTakeingOffTime.GetStartTime()/* + eLiftNeedTime*/);	
					lastItemCopy.SetTime(eTakeingOffTime.GetStartTime() + eLiftNeedTime);

					if (eTakeingOffTime.GetEndTime() > ElapsedTime(0L))
					{
						eTakeingOffTime.SetEndTime(eTakeingOffTime.GetEndTime() + eLiftNeedTime);
					}
					FlightSchemeOnAirRouteTakeOff(pFlight,pAirRouteInSim,ResourceList,lastItemCopy,eTakeingOffTime,vRunwayArrangeInfo);

					break;


				}
			}
			else if(eTimeEstmated >= vSegTimeInfo[nSeg].maxTime)
			{
				if (AdjustMaxTime(vSegTimeInfo,vClearanceItem,nSeg))
				{
					nSeg = nSeg +1;
					continue;	
				}
				else
				{
					ElapsedTime nSegsTime = CalculateOptimisticTimeThrouhRouteTakeOff(pFlight,pAirRouteInSim,ResourceList,nSeg);

					Clearance takeOffClearce;
					ClearanceItem lastItemCopy = lastItem;
					CRunwaysController *pRunwaysController =  pFlight->GetAirTrafficController()->GetRunwaysController();
					pRunwaysController->GetFlightTakeOffClearance(pFlight,eTakeingOffTime.GetStartTime(),lastItemCopy,takeOffClearce);
					//get lift off time
					ElapsedTime eLiftNeedTime;
					if(takeOffClearce.GetItemCount()>0)
					{
						eLiftNeedTime = takeOffClearce.ItemAt(takeOffClearce.GetItemCount()-1).GetTime() - takeOffClearce.ItemAt(0).GetTime();
					}


					ElapsedTime eEstmatedRunwayTime =  vSegTimeInfo[nSeg].minTime+ ElapsedTime(1L)  - nSegsTime- eLiftNeedTime;

					pRunwaysController->ApplyForTakeoffTime(pFlight,eEstmatedRunwayTime,eTakeingOffTime,vRunwayArrangeInfo);
					

					//take off
					eTakeingOffTime.SetStartTime(eTakeingOffTime.GetStartTime() /*+ eLiftNeedTime*/);
					lastItemCopy.SetTime(eTakeingOffTime.GetStartTime() + eLiftNeedTime);

					if (eTakeingOffTime.GetEndTime() > ElapsedTime(0L))
					{
						eTakeingOffTime.SetEndTime(eTakeingOffTime.GetEndTime() + eLiftNeedTime);
					}
					FlightSchemeOnAirRouteTakeOff(pFlight,pAirRouteInSim,ResourceList,lastItemCopy,eTakeingOffTime,vRunwayArrangeInfo);

					break;
				}

			}
		}
	}

    
	return false;
}

bool FlightSchemerInAirspace::AdjustMinTime(std::vector<SegmentTimeInfo>& vSegTimeInfo, std::vector<ClearanceItem >& vClearancItem,int nCurSeg)
{
	return false;

	ElapsedTime eTimeSep = vSegTimeInfo[nCurSeg].minTime - vSegTimeInfo[nCurSeg].curTime;

	//calculate the max adjust time
	ElapsedTime eMaxAdjustTime= ElapsedTime(0L);
	for (int i = nCurSeg + 1; i < (int)vSegTimeInfo.size(); ++ i)
	{
		eMaxAdjustTime += vSegTimeInfo[i].curTime - vSegTimeInfo[i].minTime;
	}

	if (eMaxAdjustTime < eTimeSep)
		return false;

	//adjust time
	vSegTimeInfo[nCurSeg].curTime = vSegTimeInfo[nCurSeg].minTime;

	for (int nSeg = nCurSeg + 1; nSeg < (int)vSegTimeInfo.size(); ++nSeg)
	{
		if (vSegTimeInfo[nSeg].curTime -eTimeSep - vSegTimeInfo[nSeg].minTime < ElapsedTime(0L)) // cann't adjust
			return false;

		ElapsedTime nSegTimeAdjust = vSegTimeInfo[nSeg].curTime - vSegTimeInfo[nSeg].minTime;
		


		if (nSegTimeAdjust > eTimeSep)
		{
			vSegTimeInfo[nSeg].curTime = vSegTimeInfo[nSeg].curTime - eTimeSep;
			break;
		}
		else
		{
			vSegTimeInfo[nSeg].curTime = vSegTimeInfo[nSeg].minTime;

			eTimeSep = eTimeSep - nSegTimeAdjust;
		}

	}

	return true;
}

bool FlightSchemerInAirspace::AdjustMaxTime(std::vector<SegmentTimeInfo>& vSegTimeInfo, std::vector<ClearanceItem >& vClearancItem,int nCurSeg)
{
	return false;
	ElapsedTime eTimeSep = vSegTimeInfo[nCurSeg].curTime- vSegTimeInfo[nCurSeg].maxTime ;

	//calculate the max adjust time
	ElapsedTime eMaxAdjustTime= ElapsedTime(0L);
	for (int i = nCurSeg + 1; i < (int)vSegTimeInfo.size(); ++ i)
	{
		eMaxAdjustTime += vSegTimeInfo[i].maxTime - vSegTimeInfo[i].curTime;
	}

	if (eMaxAdjustTime < eTimeSep)
		return false;

	//adjust time
	vSegTimeInfo[nCurSeg].curTime = vSegTimeInfo[nCurSeg].minTime;

	for (int nSeg = nCurSeg + 1; nSeg < (int)vSegTimeInfo.size(); ++nSeg)
	{	

		if ( vSegTimeInfo[nSeg].curTime + eTimeSep - vSegTimeInfo[nSeg].maxTime> ElapsedTime(0L)) // cann't adjust
			return false;


		ElapsedTime nSegTimeAdjust =vSegTimeInfo[nSeg].maxTime - vSegTimeInfo[nSeg].curTime;
		


		if (nSegTimeAdjust > eTimeSep)
		{
			vSegTimeInfo[nSeg].curTime = vSegTimeInfo[nSeg].curTime - eTimeSep;
			break;
		}
		else
		{
			vSegTimeInfo[nSeg].curTime = vSegTimeInfo[nSeg].minTime;

			eTimeSep = eTimeSep - nSegTimeAdjust;
		}

	}

	return true;
}

ElapsedTime FlightSchemerInAirspace::CalculateOptimisticTimeThrouhRoute(AirsideFlightInSim *pFlight, FlightRouteInSim *pAirRoute,AirsideResourceList& ResourceList,int nStartSeg)
{
	ElapsedTime eTotalTime(0L);


	size_t nSegmentCount = ResourceList.size();
	if (nSegmentCount < 2)
		return eTotalTime;


	//the last item is runway port resource ,ignore it
	for (int nSeg  = nStartSeg; nSeg < (int)nSegmentCount -2; ++ nSeg)
	{
		AirsideResource * pTheRes  = ResourceList.at(nSeg); 
		if( pTheRes && pTheRes->GetType() == AirRouteSegInSim::ResType_AirRouteSegment )
		{
			AirRouteSegInSim* pCurrentSegment =(AirRouteSegInSim *)pTheRes;
			ElapsedTime eSegTime = pAirRoute->GetFlightTimeInSeg(pFlight,pCurrentSegment);
			eTotalTime += eSegTime;
		}
		if(pTheRes && pTheRes->GetType() == AirsideResource::ResType_HeadingAirRouteSegment)
		{
			HeadingAirRouteSegInSim* pSeg = (HeadingAirRouteSegInSim*)pTheRes;
			double dSped = pAirRoute->GetRoutePointProperty(pSeg->GetFirstIntersection()->getInputPoint()).GetSpeed();
			CPoint2008 pos1 = pSeg->GetFirstIntersection()->getInputPoint()->GetPosition();
			double dDist = pos1.distance(pSeg->GetSecondIntersection()->getInputPoint()->GetPosition());
			eTotalTime += ElapsedTime(dDist/dSped);
		}
	}
	
	AirsideResource * pLastSegResource = ResourceList.at(nSegmentCount -2);
	if( pLastSegResource && pLastSegResource->GetType()== AirsideResource::ResType_AirRouteSegment )
	{
		AirRouteSegInSim* pLastSegment =(AirRouteSegInSim *)pLastSegResource;
			ElapsedTime eLastTime = CalculateTimeFromLastSegToRunwayPort(pFlight,pAirRoute,pLastSegment);
		eTotalTime += eLastTime;
	}
	return eTotalTime;
}
ElapsedTime FlightSchemerInAirspace::CalculateOptimisticTimeThrouhRouteTakeOff(AirsideFlightInSim *pFlight, FlightRouteInSim *pAirRoute,AirsideResourceList& ResourceList,int nEndSeg)
{
	ElapsedTime eTotalTime(0L);

	size_t nSegmentCount = ResourceList.size();
	if (nSegmentCount < 2)
		return eTotalTime;

	if (nEndSeg >= (int)nSegmentCount)
	{
		ASSERT(0);
		nEndSeg = nSegmentCount;
	}

	
	//the last item is runway port resource ,ignore it
	for (int nCurSeg  = 1; nCurSeg <= nEndSeg ;++nCurSeg)
	{
		if (nCurSeg == 1)
		{
			AirRouteSegInSim* pFirstSegment =NULL;
			if (ResourceList.at(1)->GetType() == AirRouteSegInSim::ResType_AirRouteSegment )
			{
				pFirstSegment = (AirRouteSegInSim *)ResourceList.at(nCurSeg);
			}
			ElapsedTime eRunwayPortToFirstNode = CalculateTimeFromFirstSegToRunwayPortTakeOff(pFlight,pAirRoute,pFirstSegment);
			eTotalTime += eRunwayPortToFirstNode;
		}
		else
		{
			AirsideResource * pTheRes  = ResourceList.at(nCurSeg); 
			if( pTheRes && pTheRes->GetType() == AirRouteSegInSim::ResType_AirRouteSegment )
			{
				AirRouteSegInSim* pCurrentSegment =(AirRouteSegInSim *)pTheRes;
				ElapsedTime eSegTime = pAirRoute->GetFlightTimeInSeg(pFlight,pCurrentSegment);
				eTotalTime += eSegTime;
			}
		}


	}
	
	return eTotalTime;
}
ElapsedTime FlightSchemerInAirspace::CalculateTimeFromLastSegToRunwayPort(AirsideFlightInSim *pFlight,FlightRouteInSim *pAirRouteInSim,AirRouteSegInSim *pLastSegmentInSim)
{

	RunwayPortInSim *pRunwayPort = pFlight->GetLandingRunway();

	ElapsedTime eTimeLastNodeToRunwayPort= ElapsedTime(0L);
	//AirRouteIntersectionInSim *pLastSecondConflictIntersection = pLastSegmentInSim->GetSecondConcernIntersection();
	//if (pLastSecondConflictIntersection != NULL)
	//{
	//	CPoint2008 runwayPortPoint = pRunwayPort->GetDistancePoint(0.0);
	//	CPoint2008 lastSegmentNode = pLastSecondConflictIntersection->getInputPoint()->GetPosition();

	//	double dSpeed =  pFlight->GetPerformance()->getLandingSpeed(pFlight);

	//	double dTimeLastNodeToRunwayPort = 	(runwayPortPoint.distance(lastSegmentNode))/dSpeed;
	//	eTimeLastNodeToRunwayPort = ElapsedTime(dTimeLastNodeToRunwayPort);
	//}
	//else
	//{	
		AirRouteIntersectionInSim* pLastFirstConflictIntersection = pLastSegmentInSim->GetFirstConcernIntersection();

		DistanceUnit touchDist = pFlight->GetPerformance()->getDistThresToTouch(pFlight);
		CPoint2008 runwayPortPoint = pRunwayPort->GetDistancePoint(touchDist);
		CPoint2008 lastSegmentNode = pLastFirstConflictIntersection->getInputPoint()->GetPosition();
		double dSpeed =  pAirRouteInSim->GetRoutePointProperty(pLastFirstConflictIntersection->getInputPoint()).GetSpeed();

		double dTimeLastNodeToRunwayPort = 	2.0*(runwayPortPoint.distance(lastSegmentNode))/(dSpeed+ pFlight->GetPerformance()->getLandingSpeed(pFlight));
		eTimeLastNodeToRunwayPort = ElapsedTime(dTimeLastNodeToRunwayPort);
	//}

	return eTimeLastNodeToRunwayPort;
}

ElapsedTime FlightSchemerInAirspace::CalculateTimeFromFirstSegToRunwayPortTakeOff(AirsideFlightInSim *pFlight,FlightRouteInSim *pAirRouteInSim,AirRouteSegInSim *pLastSegmentInSim)
{

	RunwayPortInSim *pRunwayPort = pFlight->GetTakeoffRunway()->GetOtherPort();

	ElapsedTime eTimeLastNodeToRunwayPort= ElapsedTime(0L);
	AirRouteIntersectionInSim *pLastFirstonflictIntersection = pLastSegmentInSim->GetFirstConcernIntersection();

	CPoint2008 runwayPortPoint = pRunwayPort->GetDistancePoint(0.0);
	CPoint2008 lastSegmentNode = pLastFirstonflictIntersection->getInputPoint()->GetPosition();

	double dSpeed =  pFlight->GetPerformance()->getClimboutSpeed(pFlight);

	double dTimeLastNodeToRunwayPort = 	2.0*(runwayPortPoint.distance(lastSegmentNode))/(dSpeed + dSpeed);
	eTimeLastNodeToRunwayPort = ElapsedTime(dTimeLastNodeToRunwayPort);

	return eTimeLastNodeToRunwayPort;
}


ElapsedTime FlightSchemerInAirspace::CalculateTimeFromOneSegToNextSeg(AirsideFlightInSim *pFlight,FlightRouteInSim *pAirRouteInSim,AirRouteSegInSim *pCurrentSeg,AirRouteSegInSim *pNextSeg)
{

	//AirRouteIntersectionInSim *pCurrentFirstInt = pCurrentSeg->GetFirstConcernIntersection();

	ElapsedTime eSegTime = pAirRouteInSim->GetFlightTimeInSeg(pFlight,pCurrentSeg);

	return eSegTime;
}















