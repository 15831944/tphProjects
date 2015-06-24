#include "StdAfx.h"
#include ".\airsidecircuitflightprocess2.h"
#include "AirTrafficController.h"
#include "AirsideResourceManager.h"
#include "SimulationErrorShow.h"
#include "StandInSim.h"
#include "StandAssignmentRegisterInSim.h"
#include "TakeoffSequencingInSim.h"
#include "ResourceEntryEvent.h"
#include "../MobileDynamics.h"
#include "FlightGetClearanceEvent.h"
#include "FlightRunwayTakeOffEvent.h"
#include "Common/DynamicMovement.h"
#include "FlightConflictAtWaypointDelay.h"
#include "../../Results/OutputAirside.h"


//-------------------------------------------------------------
//Summary:
//			Process order: start-->intension-->end
//-------------------------------------------------------------
bool AirsideCircuitFlightProcess2::Process(ClearanceItem& lastItem, Clearance& newClearance)
{

	if(m_state == ps_completed)
		return false;
	if(m_state == ps_inactive)
	{
		if( m_pFlight->isAllIntensionDone() ){
			m_state =  ps_completed;
			return false;
		}
		m_state = ps_active;    
	}


	if(m_pFlight->GetCircuitState() == AirsideCircuitFlightInSim::START_PROCESS)
	{
		if(StartBegine()){
			m_pFlight->SetCircuitState(AirsideCircuitFlightInSim::INTENTION_PROCESS);			
		}
		else
		{
			if(!m_pBeginProcess){
				m_pBeginProcess = new CircuteFlightBeginStandProcess(m_pFlight);
			}
			if( m_pBeginProcess->Process(lastItem,newClearance) )
			{
				return true;
			}	
			m_pFlight->SetCircuitState(AirsideCircuitFlightInSim::INTENTION_PROCESS);
		}
	}

	if(m_pFlight->GetCircuitState() == AirsideCircuitFlightInSim::INTENTION_PROCESS){
		if( _ProcessIntenion(lastItem,newClearance) )
			return true;
		else
		{
			m_pFlight->SetCircuitState(AirsideCircuitFlightInSim::END_PROCESS);
		}
	}
	if(m_pFlight->GetCircuitState() == AirsideCircuitFlightInSim::END_PROCESS){
		if( _ProcessEnd(lastItem,newClearance) )
			return true;
	}	

	m_state = ps_completed;
	return false;
}

bool AirsideCircuitFlightProcess2::StartBegine() const
{
	if (m_pFlight == NULL)
		return false;	
	return m_pFlight->getArrStand().IsBlank();
}

bool AirsideCircuitFlightProcess2::SIDEnd() const
{
	if (m_pFlight == NULL)
		return false;
	return m_pFlight->getDepStand().IsBlank();
}





AirsideCircuitFlightProcess2::AirsideCircuitFlightProcess2( AirsideCircuitFlightInSim* pFlight ) :ICircuteFlightProcess(pFlight)
{
	m_pIntensionProcess = NULL;
	m_pBeginProcess = NULL;
	m_pEndProcess = NULL;
}

AirsideCircuitFlightProcess2::~AirsideCircuitFlightProcess2( void )
{
	cpputil::autoPtrReset(m_pBeginProcess);
	cpputil::autoPtrReset(m_pIntensionProcess);
	cpputil::autoPtrReset(m_pEndProcess);
}


bool AirsideCircuitFlightProcess2::_ProcessIntenion( ClearanceItem& lastItem, Clearance& newClearance )
{
	if(m_pIntensionProcess)
	{
		bool btillEnd  = m_pIntensionProcess->Process(lastItem,newClearance);
		if(btillEnd) return true;
		//		
		if(m_pIntensionProcess->isCompleted())
		{
			//start a new Intension
			if(!m_pFlight->isAllIntensionDone()){
				ICirciteIntensionProcess* pIntensionProc = new ICirciteIntensionProcess(m_pFlight);
				pIntensionProc->SetLandingTakeoffRoute(m_pFlight->GetLandingCircuit(),m_pFlight->GetTakeoffCircuit());
				m_pIntensionProcess = pIntensionProc;
				return _ProcessIntenion(lastItem,newClearance);
			}
			else
			{
				m_pFlight->SetCircuitState(AirsideCircuitFlightInSim::END_PROCESS);
			}
		}
	}	
	//first time
	if(!m_pIntensionProcess){
		//ASSERT INTension 
		//A
		ASSERT(!m_pFlight->isAllIntensionDone());
		//create first intension
		ICirciteIntensionProcess* pIntensionProc = new ICirciteIntensionProcess(m_pFlight);
		if(StartBegine())
		{
			pIntensionProc->SetLandingTakeoffRoute(m_pFlight->GetSTAR(),m_pFlight->GetTakeoffCircuit());
		}
		else
		{
			pIntensionProc->SetLandingTakeoffRoute(m_pFlight->GetLandingCircuit(),m_pFlight->GetTakeoffCircuit());
		}	
		m_pIntensionProcess = pIntensionProc;
		return _ProcessIntenion(lastItem,newClearance);
	}	

	return false;

}

bool AirsideCircuitFlightProcess2::_ProcessEnd( ClearanceItem& lastItem, Clearance& newClearance )
{	

	if(!m_pEndProcess){
		if(SIDEnd()){
			m_pEndProcess = new CircuteFligthEndSIDProcess(m_pFlight);
		}
		else{
			m_pEndProcess = new CircuteFlightEndStandProcess(m_pFlight);
		}
		m_pFlight->ChangeToArrival();
	}
	if(m_pEndProcess){
		bool btillEnd = m_pEndProcess->Process(lastItem,newClearance);
		if(btillEnd)return true ;
	}
	//add last item

	return false;
	
}




bool CircuteFlightBeginStandProcess::Process( ClearanceItem& lastItem, Clearance& newClearance )
{
	if(m_state == ps_completed)
		return false;
	if(m_state == ps_inactive)
	{
		m_pFlight->ChangeToDeparture();
		m_state = ps_active;   
	}
	if(lastItem.GetMode()==OnFinalApproach)
	{
		m_state =  ps_completed;
		return false;
	}

	AirTrafficController* pATC = m_pFlight->GetAirTrafficController();

	//stand process
	StandInSim* pArrStand = m_pFlight->GetPlanedParkingStand(ARR_PARKING);
	if(pArrStand == NULL || !pArrStand->TryLock(m_pFlight))
	{
		CString strWarn= _T("no Stand for circuit Flight");
		CString strError = _T("AIRCRAFT TERMINATE");
		AirsideSimErrorShown::SimWarning(m_pFlight,strWarn,strError);

		pATC->RemoveFlightPlanedStandOccupancyTime(m_pFlight, ARR_PARKING);

		ClearanceItem newItem(NULL,OnTerminate,0);
		newItem.SetTime(lastItem.GetTime());
		newClearance.AddItem(newItem);

		m_pFlight->PerformClearance(newClearance);
		return true;

	}	
	if(lastItem.GetMode()< OnExitStand  )
	{	
		bool btillEnd = pArrStand->FindClearanceInConcern(m_pFlight,lastItem,m_pFlight->GetDepTime(),newClearance);	
		if(btillEnd)return true;	
	}


	//outbound process			 
	if(!m_pBeginOutBoundProcess){
		TaxiRouteInSim * pOutBound  = m_pFlight->GetAndAssignCircuitOutBoundRoute();	
		if (pOutBound == NULL)
		{
			CString strWarn = _T("No outbound route for the flight");
			CString strError = _T("AIRCRAFT TERMINATE");
			AirsideSimErrorShown::SimWarning(m_pFlight,strWarn,strError);

			ClearanceItem newItem(NULL,OnTerminate,0);
			newItem.SetTime(lastItem.GetTime());
			newClearance.AddItem(newItem);

			m_pFlight->PerformClearance(newClearance);
			return true;
		}
		m_pBeginOutBoundProcess = new OutBoundRouteProcess(m_pFlight,pOutBound);
	}

	ASSERT(m_pBeginOutBoundProcess);
	if( m_pBeginOutBoundProcess->Process(lastItem,newClearance) )
		return true;

	//takeoff process
	{
		FlightRouteInSim* pTakeoffCircuitRoute = m_pFlight->GetTakeoffCircuit();
		ClearanceItem currentItem = lastItem;

		if (lastItem.GetMode() == OnTakeoff)
		{
			ClearanceItem climoutItem = lastItem;
			if(GenerateClimoutItem(lastItem,climoutItem))
			{
				lastItem = climoutItem;
				newClearance.AddItem(climoutItem);
			}
		}
		else if(lastItem.GetMode() != OnClimbout)
		{
			bool bEnterSystem = CheckIsTakeoffDelay(currentItem,newClearance,pTakeoffCircuitRoute,AirsideCircuitFlightInSim::ForceAndGo_Operation,true);
			if ( !bEnterSystem)
			{
				newClearance.ClearItems();
				return true;
			}

			return true;
		}
	}

	ASSERT(lastItem.GetMode() == OnFinalApproach);
	m_state = ps_completed;
	return false;
}

bool ICircuteFlightProcess::GenerateClimoutItem( ClearanceItem& lastItem,ClearanceItem& climoutItem )
{
	FlightRouteInSim* pTakeoffCircuitRoute = m_pFlight->GetTakeoffCircuit();
	if (pTakeoffCircuitRoute == NULL)
	{
		return false;
	}
	ClearanceItem RwyLiftOffItem = lastItem;

	AirRoutePointInSim* pRoutePoint = ((AirRouteSegInSim*)pTakeoffCircuitRoute->GetItem(1))->GetFirstConcernIntersection()->getInputPoint();

	ClearanceItem missItem(pRoutePoint,OnClimbout,0);		
	missItem.SetSpeed(pTakeoffCircuitRoute->GetRoutePointProperty(pRoutePoint).GetSpeed());
	double dAlt = pTakeoffCircuitRoute->GetRoutePointProperty(pRoutePoint).GetAlt();
	missItem.SetAltitude(dAlt);

	CPoint2008 curPos = RwyLiftOffItem.GetPosition();
	curPos.setZ(RwyLiftOffItem.GetAltitude());
	CPoint2008 nextPos = pRoutePoint->GetPosition();
	missItem.SetPosition(nextPos);
	ElapsedTime intervalTime;
	double dSpeed = (RwyLiftOffItem.GetSpeed() + missItem.GetSpeed()) * 0.5;
	if (dSpeed > 0)
	{
		intervalTime = ElapsedTime(curPos.distance(nextPos)/dSpeed);
	}
	missItem.SetTime(RwyLiftOffItem.GetTime() + intervalTime);

	climoutItem = missItem;
	return true;
}

void ICircuteFlightProcess::GenerateFullStopClearance(FlightRouteInSim *pAirRouteInSim,AirsideResourceList& ResourceList,ClearanceItem& lastItem,Clearance& newClearance)
{
	ClearanceItem preItem = lastItem;
	int nCount = (int)ResourceList.size();
	for (int i =0; i < nCount; i++)
	{
		AirsideResource* pResource = ResourceList.at(i);

		if (pResource->GetType() == AirsideResource::ResType_HeadingAirRouteSegment)
		{
			((HeadingAirRouteSegInSim*)pResource)->CalculateClearanceItems(m_pFlight,pAirRouteInSim,preItem,ResourceList.at(i+1),newClearance);
		}

		if (pResource->GetType() == AirsideResource::ResType_AirRouteSegment)
		{
			AirRoutePointInSim* pFirstIntersection = ((AirRouteSegInSim*)pResource)->GetFirstConcernIntersection()->getInputPoint();
			FlightPlanPropertyInSim property = pAirRouteInSim->GetRoutePointProperty(pFirstIntersection);
			OccupancyInstance instance = pFirstIntersection->GetOccupyInstance(m_pFlight);
			ElapsedTime tDelayAtPoint = instance.GetEnterTime() - preItem.GetTime();


			ClearanceItem newItem(pResource,property.GetMode(),0);
			newItem.SetPosition(pFirstIntersection->GetPosition());

			if (preItem.GetPosition() == pFirstIntersection->GetPosition() && ((AirRouteSegInSim*)pResource)->GetFirstConcernIntersection()->GetConnectHold())
				newItem.SetAltitude(preItem.GetAltitude());
			else
				newItem.SetAltitude(property.GetAlt());

			newItem.SetSpeed(instance.GetSpeed());
			newItem.SetTime(instance.GetEnterTime());
		
			preItem = newItem;
			newClearance.AddItem(preItem);

			ElapsedTime tExit = -1L;
			if (((AirRouteSegInSim*)pResource)->GetSecondConcernIntersection())
			{
				AirRoutePointInSim* pSecondIntersection = ((AirRouteSegInSim*)pResource)->GetSecondConcernIntersection()->getInputPoint();
				tExit = pSecondIntersection->GetOccupyInstance(m_pFlight).GetEnterTime();
				if(tExit < preItem.GetTime())
				{
					pSecondIntersection->RemoveElementOccupancyInfo(m_pFlight);
					tExit = preItem.GetTime() + pAirRouteInSim->GetFlightTimeInSeg(m_pFlight,(AirRouteSegInSim*)pResource);
				}
			}
			if(tExit < 0L)
				tExit = preItem.GetTime() + pAirRouteInSim->GetFlightTimeInSeg(m_pFlight,(AirRouteSegInSim*)pResource);

			

			((AirRouteSegInSim*)pResource)->CalculateClearanceItems(m_pFlight,pAirRouteInSim,preItem,tExit,newClearance);

			if ( ((AirRouteSegInSim*)pResource)->GetSecondConcernIntersection())
			{	
				AirRouteIntersectionInSim* pIntersectionNode = ((AirRouteSegInSim*)pResource)->GetSecondConcernIntersection();
				AirRoutePointInSim* pSecondIntersection = pIntersectionNode->getInputPoint();

				FlightPlanPropertyInSim Pointprop = pAirRouteInSim->GetRoutePointProperty(pSecondIntersection);
				instance = pSecondIntersection->GetOccupyInstance(m_pFlight);

				if (pIntersectionNode->GetConnectHold())
				{
					CPoint2008 ATCManagedPoint = ((AirRouteSegInSim*)pResource)->GetATCManagedPoint(m_pFlight);
					preItem = newClearance.GetItem(newClearance.GetItemCount()-1);
					if (preItem.GetPosition() == ATCManagedPoint)
						return;

					double dRateInSegment = ATCManagedPoint.distance(preItem.GetPosition())/(preItem.GetPosition().distance(pSecondIntersection->GetPosition()));
					ElapsedTime dT = pAirRouteInSim->GetFlightTimeInSeg(m_pFlight,(AirRouteSegInSim*)pResource);
					ElapsedTime tTime = ElapsedTime(dT.asSeconds()*dRateInSegment) + preItem.GetTime();
					double dAlt = (1-dRateInSegment) * (preItem.GetAltitude() - Pointprop.GetAlt()) + Pointprop.GetAlt();
					preItem.SetAltitude(dAlt);
					preItem.SetPosition(ATCManagedPoint);
					preItem.SetTime(tTime);
					newClearance.AddItem(preItem);
					return;

				}
				else
				{

					ClearanceItem secondItem(pResource,Pointprop.GetMode(),0);
					secondItem.SetPosition(pSecondIntersection->GetPosition());
					secondItem.SetAltitude(Pointprop.GetAlt());
					if (instance.GetFlight())
					{
						secondItem.SetSpeed(instance.GetSpeed());
						secondItem.SetTime(instance.GetEnterTime());
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
		}
	}
}

bool ICircuteFlightProcess::CheckIsTakeoffDelay( ClearanceItem& lastItem, Clearance& newClearance,FlightRouteInSim *pAirRouteInSim,AirsideCircuitFlightInSim::LandingOperation emType,bool bCanWait )
{
	AirTrafficController* pATC = m_pFlight->GetAirTrafficController();
	if (pATC == NULL)
		return false;

	if (pAirRouteInSim == NULL)
		return false;

	if( pAirRouteInSim->GetItemCount() == 0)
		return false;

	AirsideResourceList resList;
	AirsideResource* pCurrentRes = lastItem.GetResource();

	ClearanceItem flightItem = lastItem;
	pAirRouteInSim->GetConcernRouteSegs(pCurrentRes,resList);
	if ( resList.at(0)->GetType()== AirsideResource::ResType_LogicRunway)	//take off	
	{	
		if (m_pFlight->GetAndAssignTakeoffRunway() == NULL)		// no takeoff runway
		{
			ClearanceItem newItem(NULL,OnTerminate,0);
			newItem.SetTime(lastItem.GetTime());
			newClearance.AddItem(newItem);

			m_pFlight->PerformClearance(newClearance);
			return true;
		}

		pATC->GetAirsideResourceManager()->GetAirspaceResource();
		CRunwaySystem *pRunwaysController =  m_pFlight->GetAirTrafficController()->GetRunwaysController();
		std::vector<CRunwaySystem::RunwayOccupyInfo> vRunwayArrangeInfo;
		vRunwayArrangeInfo.clear();
		bool bNeedCreateTakeOffEvent = false;

		CPath2008 filletPath = m_pFlight->GetFilletPathFromTaxiwayToRwyport(m_pFlight->GetAndAssignTakeoffRunway()->getLogicRunwayType());
		MobileTravelTrace travelTrace(m_pFlight->mGroundPerform,filletPath.GetTotalLength(),flightItem.GetSpeed(),0 );
		travelTrace.BeginUse();
		ElapsedTime dTimeToRwy = travelTrace.getEndTime();

		ElapsedTime tMoveTime =0L;
		bool bTakeoff = pATC->GetAirsideResourceManager()->GetAirspaceResource()->FlightSchemeOnAirRouteTakeOff(m_pFlight, pAirRouteInSim, resList, lastItem, dTimeToRwy, tMoveTime, vRunwayArrangeInfo);

		if (!bTakeoff)
			return false;

		if (lastItem.GetTime() < tMoveTime)
			bNeedCreateTakeOffEvent = true;


		flightItem.SetTime(tMoveTime);
		pRunwaysController->WriteRunwayTakeOffLogs(m_pFlight,vRunwayArrangeInfo,tMoveTime,dTimeToRwy, flightItem,newClearance);
		ClearanceItem RwyLiftOffItem = newClearance.GetItem(newClearance.GetItemCount() -1);

		GenerateFullStopClearance(pAirRouteInSim, resList, RwyLiftOffItem, newClearance);

		if (bNeedCreateTakeOffEvent)
		{
			//wait log
			//start waiting event
			ClearanceItem StartwaitItem = lastItem;	
			StartwaitItem.SetMode(OnTakeOffWaitEnterRunway);

			//wait item
			ClearanceItem waitItem = lastItem;
			//ElapsedTime tMoveTime = max((eTakeOffTime - dTimeToRwy), lastItem.GetTime());
			waitItem.SetTime(tMoveTime);

			waitItem.SetMode(OnTakeOffWaitEnterRunway);
			m_pFlight->PerformClearanceItem(StartwaitItem);
			m_pFlight->PerformClearanceItem(waitItem);

			//create take off event, take off at takeoff time
			if(0)
			{
				newClearance.ClearItems();
				FlightGetClearanceEvent * pNextEvent = new FlightGetClearanceEvent(m_pFlight);
				pNextEvent->setTime(tMoveTime);
				pNextEvent->addEvent();
			}
			else
			{
				CFlightDelayTakeoffOnRwyEvent * pTakeOffEvent = new CFlightDelayTakeoffOnRwyEvent(m_pFlight,newClearance);
				pTakeOffEvent->setTime(tMoveTime);
				pTakeOffEvent->addEvent();
				newClearance.ClearAllItems();
			}
		}
	}
	return true;
}

bool ICircuteFlightProcess::GetLandingDelayTime( ElapsedTime& eLandingDelayTime,ClearanceItem& lastItem )
{
	ClearanceItem flightItem = lastItem;
	FlightRouteInSim* pCircuitRoute = m_pFlight->GetLandingCircuit();
	if (pCircuitRoute == NULL)
		return false;

	AirTrafficController* pATC = m_pFlight->GetAirTrafficController();
	if (pATC == NULL)
		return false;

	AirsideResourceList resList;
	AirsideResource* pCurrentRes = lastItem.GetResource();
	pCircuitRoute->GetConcernRouteSegs(pCurrentRes,resList);
	if (resList.empty())
		return false;

	if (m_pFlight->GetRunwayExit() == NULL)
	{				
		m_pFlight->GetAirTrafficController()->AssignRunwayExit(m_pFlight);
		if (m_pFlight->GetRunwayExit() == NULL)
		{
			return false;
		}
	}

	TimeRange eLandingTimeRange(ElapsedTime(0L),ElapsedTime(0L));
	ElapsedTime tRwyEnter = 0L;
	CRunwaySystem *pRunwaysController =  m_pFlight->GetAirTrafficController()->GetRunwaysController();
	ElapsedTime eTotalTime = pATC->GetAirsideResourceManager()->GetAirspaceResource()->CalculateOptimisticTimeThrouhRoute(m_pFlight,pCircuitRoute,resList);
	tRwyEnter = lastItem.GetTime() + eTotalTime;

	std::vector<CRunwaySystem::RunwayOccupyInfo> vRunwayArrangeInfo;
	if(!pRunwaysController->ApplyForLandingTime(m_pFlight,tRwyEnter,eLandingTimeRange,vRunwayArrangeInfo))
	{
		return false;
	}

	ClearanceItem preItem = lastItem;
	vRunwayArrangeInfo.clear();
	pATC->GetAirsideResourceManager()->GetAirspaceResource()->FlightSchemeOnAirRouteLanding(m_pFlight,pCircuitRoute,resList,lastItem,preItem, eLandingTimeRange,m_pFlight->GetTime(),vRunwayArrangeInfo);
	AirRouteSegInSim* pSeg = (AirRouteSegInSim*)pCircuitRoute->GetItem(0);
	AirRoutePointInSim* pFirstIntersection = pSeg->GetFirstConcernIntersection()->getInputPoint();
	FlightPlanPropertyInSim property = pCircuitRoute->GetRoutePointProperty(pFirstIntersection);
	OccupancyInstance instance = pFirstIntersection->GetOccupyInstance(m_pFlight);

	eLandingDelayTime = instance.GetEnterTime() - flightItem.GetTime();
	return true;
}

bool ICircuteFlightProcess::GetMisApproach( LogicRunwayInSim* pLandRunway,ClearanceItem& lastItem, Clearance& newClearance )
{
	pLandRunway->SetHasMissedApproachFlight();
	AirTrafficController* pATC = m_pFlight->GetAirTrafficController();
	FlightRouteInSim* pAirRoute = pATC->GetAirsideResourceManager()->GetAirspaceResource()->GetMisApproachRoute(m_pFlight);
	if (pAirRoute->IsEmpty())
	{
		CString strError = "Flight cannot land and there is no available Miss Approach air route to re-enter STAR";
		CString strErrorType = "AIRCRAFT TERMINATE";
		AirsideSimErrorShown::SimWarning(m_pFlight,strError,strErrorType);

		pLandRunway->RemoveElementOccupancyInfo(m_pFlight);
		pLandRunway->RemoveLeftFlight(m_pFlight);

		ClearanceItem newItem(NULL,OnTerminate,0);
		newItem.SetTime(lastItem.GetTime());
		newClearance.AddItem(newItem);	

		m_pFlight->PerformClearance(newClearance);
		return true;
	}
	else
	{
		AirsideFlightInSim* pConflictFlt = pLandRunway->GetLastInResourceFlight();
		m_pFlight->StartDelay(lastItem,pConflictFlt,FlightConflict::MISSAPPROACH,FlightConflict::RUNWAY,FltDelayReason_AirHold,_T("Missed approach"));	//Miss approach delay
		pATC->GetAirsideResourceManager()->GetAirspaceResource()->GetMissApproachClearance(m_pFlight,pAirRoute, lastItem,newClearance);
		return true;
	}
}

bool ICircuteFlightProcess::Process( ClearanceItem& lastItem, Clearance& newClearance )
{
	if(isCompleted())
		return false;

	if(isInActive()){
		if(DoActivate(lastItem,newClearance)){
			setActive();
		}
	}
	if(DoProcess(lastItem,newClearance))
		return true;	

	setComplete();
	return false;
}


bool OutBoundRouteProcess::DoActivate(ClearanceItem& lastItem, Clearance& newClearance)
{
	m_pFlight->GetAndAssignTakeoffRunway()->RemoveLeftFlight(m_pFlight);
	m_pFlight->GetAndAssignTakeoffRunway()->SetExitTime(m_pFlight,lastItem.GetTime());
	double taxispd = m_pFlight->GetPerformance()->getTaxiOutNormalSpeed(m_pFlight);
	m_pOutBound->SetSpeed(taxispd);	
	return true;
}

bool OutBoundRouteProcess::DoProcess( ClearanceItem& lastItem, Clearance& newClearance )
{
	AirTrafficController* pATC = m_pFlight->GetAirTrafficController();

	//TaxiRouteInSim * pCircuitRoute = m_pFlight->GetCircuitTaxiwayRoute();
	LogicRunwayInSim * pTakeoffRunway = m_pFlight->GetAndAssignTakeoffRunway();

	bool btillEnd = m_pOutBound->FindClearanceInConcern(m_pFlight,lastItem,50000, newClearance);
	if(btillEnd)
		return true;	

	TakeoffQueueInSim* pTakeoffQueue = pATC->GetAirsideResourceManager()->GetAirportResource()->getTakeoffQueuesManager()->GetRunwayExitQueue(m_pFlight->GetAndAssignTakeoffPosition());
	//takeoff Queue
	if(pTakeoffQueue)
	{
		bool btillEnd = pTakeoffQueue->FindClearanceInConcern(m_pFlight,lastItem,newClearance);
		if(btillEnd) return true;
	}
	//Queue to take off
	if(pTakeoffRunway && lastItem.GetMode()< OnTakeOffEnterRunway)
	{
		if (pATC->GetTakeoffSequenceInSim()->IsWaitingInQueue(m_pFlight,lastItem.GetTime()))
		{
			m_pFlight->GetWakeUpCaller().OberverSubject((CAirsideSubject*)pATC->GetRunwayOccupancyStatus());
			return true;
		}
	}

	return false;
}

bool CircuteFlightEndStandProcess::DoProcess( ClearanceItem& lastItem, Clearance& newClearance )
{	

	if(m_pFlight->IsArrivingOperation()){
		if( _ProcessLanding(lastItem,newClearance) )
			return true;

		m_pFlight->ChangeToDeparture();
		
	}

	//
	StandInSim* pDepStand = m_pFlight->GetPlanedParkingStand(DEP_PARKING);
	if(pDepStand == NULL || !pDepStand->TryLock(m_pFlight))
	{
		CString strWarn = _T("no Stand for circuit Flight");
		CString strError = _T("AIRCRAFT TERMINATE");
		AirsideSimErrorShown::SimWarning(m_pFlight,strWarn,strError);

		StandInSim* pPlanedStand = m_pFlight->GetPlanedParkingStand(DEP_PARKING);
		if (pPlanedStand)
		{
			int oldIdx = pPlanedStand->GetStandAssignmentRegister()->FindRecord(m_pFlight,DEP_PARKING);
			pPlanedStand->GetStandAssignmentRegister()->DeleteRecord(oldIdx);
		}

		ClearanceItem newItem(NULL,OnTerminate,0);
		newItem.SetTime(lastItem.GetTime());
		newClearance.AddItem(newItem);

		m_pFlight->PerformClearance(newClearance);
		return true;
	}


	StandInSim* pParkingStand = m_pFlight->GetPlanedParkingStand(DEP_PARKING);
	// on exit Runway
	if(lastItem.GetMode() == OnExitRunway )
	{
		RunwayExitInSim* pRunwayExit = m_pFlight->GetRunwayExit(); 
		if(!pRunwayExit) // no runway exit to choose available
		{
			CString strWarn = _T("There is no available Runway Exit for Flight ");
			CString strError = _T("AIRCRAFT TERMINATE");
			AirsideSimErrorShown::SimWarning(m_pFlight,strWarn,strError);
			ClearanceItem newItem(NULL,OnTerminate,0);
			newItem.SetTime(lastItem.GetTime());
			newClearance.AddItem(newItem);
			if (pParkingStand)
			{
				int nIdx = pParkingStand->GetStandAssignmentRegister()->FindRecord(m_pFlight, DEP_PARKING);
				pParkingStand->GetStandAssignmentRegister()->DeleteRecord(nIdx);
			}

			m_pFlight->PerformClearance(newClearance);
			return true;
		}
	}

	//route to stand
	TaxiRouteInSim * pRouteToStand = m_pFlight->GetAndAssignCircuitInBoundRoute();
	if( pRouteToStand && lastItem.GetMode() <= OnTaxiToStand ) //taxi to the parking stand
	{
		double taxispd = m_pFlight->GetPerformance()->getTaxiInNormalSpeed(m_pFlight);
		pRouteToStand->SetSpeed(taxispd);
		bool btillEnd = pRouteToStand->FindClearanceInConcern(m_pFlight,lastItem,5000,newClearance);
		if(btillEnd) return true;
	}

	if (pRouteToStand == NULL)
	{
		CString strWarn = _T("No inbound route for the flight");
		CString strError = _T("AIRCRAFT TERMINATE");
		AirsideSimErrorShown::SimWarning(m_pFlight,strWarn,strError);

		ClearanceItem newItem(NULL,OnTerminate,0);
		newItem.SetTime(lastItem.GetTime());
		newClearance.AddItem(newItem);
		m_pFlight->PerformClearance(newClearance);
		return true;
	}

	//Stand
	StandInSim* pDepParkingStand = m_pFlight->GetPlanedParkingStand(DEP_PARKING);
	if(pDepParkingStand && lastItem.GetMode() < OnHeldAtStand )		//to arrival flight
	{	

		m_pFlight->SetArrTime(lastItem.GetTime());		//update arrival time

		ElapsedTime tParkingTime = lastItem.GetTime() + m_pFlight->GetFlightInput()->getServiceTime();
		bool btillEnd = pDepParkingStand->FindClearanceInConcern(m_pFlight,lastItem,tParkingTime,newClearance);		
		if(btillEnd) return true;
	}

	if (!m_pFlight->IsDeparture() && pDepParkingStand && lastItem.GetMode() == OnHeldAtStand)
	{
		bool btillEnd = pDepParkingStand->FindClearanceInConcern(m_pFlight,lastItem,lastItem.GetTime(),newClearance);		
		if(btillEnd) return true;
	}
	
	return false;
}

bool CircuteFlightEndStandProcess::DoActivate( ClearanceItem& lastItem, Clearance& newClearance )
{
	m_pLandingRoute = m_pFlight->GetLandingCircuit();
	m_pTakeoffRoute = m_pFlight->GetTakeoffCircuit();
	m_pFlight->ChangeToArrival();
	m_op = AirsideCircuitFlightInSim::ForceAndGo_Operation;
	return true;
}

bool CircuteFligthEndSIDProcess::DoActivate(ClearanceItem& lastItem, Clearance& newClearance)
{
	m_pFlight->ChangeToArrival();
	m_firstMode = lastItem.GetMode();
	m_op = AirsideCircuitFlightInSim::LowAndOver_Operation;
	return true;
}



CircuteFligthEndSIDProcess::CircuteFligthEndSIDProcess( AirsideCircuitFlightInSim* pFlight ) :ICirciteIntensionProcess(pFlight)
{
	m_pOutBound = NULL;
	SetLandingTakeoffRoute(pFlight->GetLandingCircuit(),pFlight->GetSID());
}

#include ".\AirHoldInInSim.h"


bool ICirciteIntensionProcess::DoProcess( ClearanceItem& lastItem, Clearance& newClearance )
{

	if(m_pFlight->IsArrivingOperation()){
		if( _ProcessLanding(lastItem,newClearance) )
		{
			if(newClearance.GetItemCount() && m_pFlight->GetMode()==OnBirth)  //for entry
			{
				AirEntrySystemHoldInInSim* pEntryHold = m_pFlight->GetSTAR()->GetEntrySystemHold();
				if (pEntryHold == NULL)
					return true;

				ElapsedTime tEnterSysTime = newClearance.GetItem(0).GetTime();
				pEntryHold->writeFlightLog(m_pFlight,tEnterSysTime);
				CFlightDelayOnHoldEvent* pHoldDelayEvent = new CFlightDelayOnHoldEvent(m_pFlight,pEntryHold);

				pHoldDelayEvent->setTime(tEnterSysTime);
				pHoldDelayEvent->addEvent();				
			}
			return true;
		}
		m_pFlight->ChangeToDeparture();
	}
	
	if(_ProcessPostLanding(lastItem,newClearance))
		return true;	
	return false;
}

bool ICirciteIntensionProcess::SchemeOnAirRouteOnce(const ClearanceItem& _lastItem, const AirsideResourceList& ResourceList ,ElapsedTime& tDelay )
{	
	int nSegmentCount = ResourceList.size();
	//initialization
	std::vector<AirRouteNetworkInSim::SegmentTimeInfo> vSegTimeInfo;
	vSegTimeInfo.resize(nSegmentCount+1);
	AirsideCircuitFlightInSim* pFlight = m_pFlight;
	ClearanceItem IterItem = _lastItem;

	int nSeg = 0;
	while (nSeg <= nSegmentCount)		//last segment need judge conflict of second intersection
	{	
		AirsideResource* pNextRes = NULL;
		if (nSeg < nSegmentCount)
			pNextRes = ResourceList.at(nSeg);
		else
			pNextRes = ResourceList.at(nSegmentCount -1);
		FlightRouteInSim* pAirRouteInSim = GetResourceRoute(pNextRes);

		AirsideResource* pPreRes = IterItem.GetResource();
		if (nSeg >0)
			pPreRes = ResourceList.at(nSeg -1);

		ElapsedTime eTimeEstmated = -1L;

		if (pNextRes->GetType() == AirsideResource::ResType_LogicRunway)
		{
			LogicRunwayInSim* pRunway = (LogicRunwayInSim*)pNextRes;
			Clearance clearance;
			CRunwaySystem *pRunwaysController =  pFlight->GetAirTrafficController()->GetRunwaysController();
			if(m_op == AirsideCircuitFlightInSim::LowAndOver_Operation){
				pRunwaysController->GenerateLowAndOverClearance(pFlight,pRunway,m_pTakeoffRoute,IterItem,clearance);			
			}
			else if(m_op == AirsideCircuitFlightInSim::TouchAndGo_Operaiton){
				pRunwaysController->GenerateTouchAndGoClearance(pFlight,pRunway,m_pTakeoffRoute, IterItem,clearance);
				std::vector<CRunwaySystem::RunwayOccupyInfo> vRunwayArrangeInfo;
				TimeRange eLandingTimeRange(ElapsedTime(0L),ElapsedTime(0L));
				ElapsedTime tRwyEnter = clearance.GetItem(0).GetTime();
				AirTrafficController* pATC = m_pFlight->GetAirTrafficController();
				if (m_pFlight->GetRunwayExit() == NULL)
				{				
					m_pFlight->GetAirTrafficController()->AssignRunwayExit(m_pFlight);
					if (m_pFlight->GetRunwayExit() == NULL)
					{
						return false;
					}
				}
				if(!pRunwaysController->ApplyForLandingTime(m_pFlight,tRwyEnter,eLandingTimeRange,vRunwayArrangeInfo))
				{
					tDelay = eLandingTimeRange.GetStartTime() - tRwyEnter;
					return false;
				}
			}
			else if(m_op == AirsideCircuitFlightInSim::StopAndGo_Operaiton)
			{
				pRunwaysController->GenerateStopAndGoClearance(pFlight,pRunway,m_pTakeoffRoute,IterItem,clearance);
				std::vector<CRunwaySystem::RunwayOccupyInfo> vRunwayArrangeInfo;
				TimeRange eLandingTimeRange(ElapsedTime(0L),ElapsedTime(0L));
				ElapsedTime tRwyEnter = clearance.GetItem(0).GetTime();
				AirTrafficController* pATC = m_pFlight->GetAirTrafficController();
				if (m_pFlight->GetRunwayExit() == NULL)
				{				
					m_pFlight->GetAirTrafficController()->AssignRunwayExit(m_pFlight);
					if (m_pFlight->GetRunwayExit() == NULL)
					{
						return false;
					}
				}
				if(!pRunwaysController->ApplyForLandingTime(m_pFlight,tRwyEnter,eLandingTimeRange,vRunwayArrangeInfo))
				{
					tDelay = eLandingTimeRange.GetStartTime() - tRwyEnter;
					return false;
				}
			}
			else if(m_op == AirsideCircuitFlightInSim::ForceAndGo_Operation){
				if (m_pFlight->GetRunwayExit() == NULL)
				{				
					m_pFlight->GetAirTrafficController()->AssignRunwayExit(m_pFlight);
					if (m_pFlight->GetRunwayExit() == NULL)
					{
						return false;
					}
				}
				pRunwaysController->GenerateLandingCleaerance(pFlight,pRunway,pFlight->GetRunwayExit(),true,IterItem,clearance);
				std::vector<CRunwaySystem::RunwayOccupyInfo> vRunwayArrangeInfo;
				TimeRange eLandingTimeRange(ElapsedTime(0L),ElapsedTime(0L));
				ElapsedTime tRwyEnter = clearance.GetItem(0).GetTime();
				AirTrafficController* pATC = m_pFlight->GetAirTrafficController();				
				if(!pRunwaysController->ApplyForLandingTime(m_pFlight,tRwyEnter,eLandingTimeRange,vRunwayArrangeInfo))
				{
					tDelay = eLandingTimeRange.GetStartTime() - tRwyEnter;
					return false;
				}
			}
			else { ASSERT(false); }
			nSeg++;
			continue;
		}
		else if(pNextRes->GetType() == AirsideResource::ResType_AirRouteSegment)
		{
			AirRouteSegInSim* pSeg = (AirRouteSegInSim *)pNextRes; 
			AirRouteIntersectionInSim* pConflictIntersection = NULL;
			if (nSeg < nSegmentCount)
				pConflictIntersection = ((AirRouteSegInSim *)pNextRes)->GetFirstConcernIntersection();
			else
			{
				if (((AirRouteSegInSim *)pNextRes)->GetSecondConcernIntersection() == NULL)	//last segment only one waypoint
					return true;

				pConflictIntersection = ((AirRouteSegInSim *)pNextRes)->GetSecondConcernIntersection();
				if (pConflictIntersection->GetConnectHold())
					return true;
			}

		
			ClearanceItem newItem(pNextRes,OnBirth,0.0);
			newItem.SetPosition(pConflictIntersection->getInputPoint()->GetPosition());
			FlightPlanPropertyInSim prop  = pAirRouteInSim->GetRoutePointProperty(pConflictIntersection->getInputPoint());
			newItem.SetAltitude(prop.GetAlt());
			newItem.SetSpeed(prop.GetSpeed());
			newItem.SetTime(IterItem.GetTime() + pFlight->GetTimeBetween(IterItem,newItem) );		

			ElapsedTime tEstTime = newItem.GetTime();
			ClearanceItem _lastItem  = IterItem; 
			if( pConflictIntersection->CalculateClearanceAtIntersectionCircute(pFlight,pPreRes,(nSeg<nSegmentCount)?pNextRes:NULL,pAirRouteInSim,_lastItem,newItem) )
			{
				++nSeg;			
				IterItem = newItem;	
				continue;;
			}
			else
			{
				tDelay =  newItem.GetTime()  - tEstTime;
				return false;
			}
		
		}

		break;
	}
	return true; 
}

bool ICirciteIntensionProcess::_ProcessLanding( ClearanceItem& lastItem, Clearance& newClearance )
{
	AirsideResourceList landresource;
	m_pLandingRoute->GetConcernRouteSegs(lastItem.GetResource(), landresource);

	//if(landresource.empty())
	//	return false;

	bool bIncludeRunway = false;
	AirsideResource* pLastLandResource = landresource.back();
	if(	pLastLandResource && pLastLandResource->GetType() == AirsideResource::ResType_LogicRunway){
		bIncludeRunway = true;
		if(m_op!=AirsideCircuitFlightInSim::ForceAndGo_Operation){
			AirsideResourceList takeoffresouce;
			m_pTakeoffRoute->GetConcernRouteSegs(NULL,takeoffresouce);
			landresource.insert(landresource.end(),takeoffresouce.begin()+1,takeoffresouce.end());
		}
	}	
	if(!landresource.empty())
	{
		SchemeOnAirRoute(lastItem,landresource);
		GenerateClearance( landresource, lastItem, newClearance);
		if(bIncludeRunway){
			m_pFlight->ChangeToDeparture();
		}
		return true;
	}
	return false;
}

bool ICirciteIntensionProcess::_ProcessPostLanding( ClearanceItem& lastItem, Clearance& newClearance )
{
	if(m_op!=AirsideCircuitFlightInSim::ForceAndGo_Operation)
	{
		AirsideResourceList takeoffreslist;
		m_pTakeoffRoute->GetConcernRouteSegs(lastItem.GetResource(),takeoffreslist);

		if(!takeoffreslist.empty())
		{
			SchemeOnAirRoute(lastItem,takeoffreslist);
			GenerateClearance( takeoffreslist, lastItem, newClearance);
			return true;
		}
		return false;
	}
	else
	{
		return ( _ProcessFullStopAndGo(lastItem,newClearance) );		
	}
	return false;
}

bool ICirciteIntensionProcess::DoActivate( ClearanceItem& lastItem, Clearance& newClearance )
{
	
	m_op = (m_pFlight->GetRandomLandingOperation());
	m_pFlight->ReleaseIntension(m_op);
	m_pFlight->ChangeToArrival();
	if(lastItem.GetResource() == NULL) //entry system hold
	{
		AirEntrySystemHoldInInSim* pEntryHold = m_pLandingRoute->GetEntrySystemHold();
		if( pEntryHold && pEntryHold->GetInQFlightCount() && !pEntryHold->isFlightInQ(m_pFlight)) //if the hold is not empty, and pFlight is not in hold,add pFlight to the hold and wait
		{
			pEntryHold->OnFlightEnter(m_pFlight,lastItem.GetTime() );				
			pEntryHold->AddFlightToQueue(m_pFlight);			
		}
		else if(pEntryHold && !pEntryHold->GetOccupyInstance(m_pFlight).IsValid() )
		{
			pEntryHold->AddFlightToQueue(m_pFlight);
			pEntryHold->OnFlightEnter(m_pFlight,lastItem.GetTime() );
		}
	}	
	return true;
}

	


void ICirciteIntensionProcess::GenerateClearance( const AirsideResourceList& ResourceList,ClearanceItem& preItem,Clearance& newClearance )
{
	//ClearanceItem preItem = lastItem;
	AirsideCircuitFlightInSim* pFlight = m_pFlight;

	int nCount = (int)ResourceList.size();
	for (int i =0; i < nCount; i++)
	{
		AirsideResource* pResource = ResourceList.at(i);
		FlightRouteInSim* pAirRouteInSim = GetResourceRoute(pResource); 

		if ( pResource->GetType() == AirsideResource::ResType_LogicRunway )
		{	
			LogicRunwayInSim* pRunway = (LogicRunwayInSim*)pResource;
			CRunwaySystem *pRunwaysController =  pFlight->GetAirTrafficController()->GetRunwaysController();
			if(m_op == AirsideCircuitFlightInSim::LowAndOver_Operation){
				pRunwaysController->GenerateLowAndOverClearance(pFlight,pRunway,m_pTakeoffRoute,preItem,newClearance);			
			}
			else if(m_op == AirsideCircuitFlightInSim::TouchAndGo_Operaiton){
				pRunwaysController->GenerateTouchAndGoClearance(pFlight,pRunway,m_pTakeoffRoute, preItem,newClearance,true);
				//release runway
				ElapsedTime lifttime = newClearance.GetItem(newClearance.GetItemCount()-2).GetTime();
				ResourceExitEvent* pEvent = new ResourceExitEvent(pRunway, m_pFlight);
				pEvent->setTime(lifttime);
				pEvent->addEvent();	
			}
			else if(m_op == AirsideCircuitFlightInSim::StopAndGo_Operaiton){
				pRunwaysController->GenerateStopAndGoClearance(pFlight,pRunway,m_pTakeoffRoute,preItem,newClearance,true);
				//release runway
				ElapsedTime lifttime = newClearance.GetItem(newClearance.GetItemCount()-2).GetTime();
				ResourceExitEvent* pEvent = new ResourceExitEvent(pRunway, m_pFlight);
				pEvent->setTime(lifttime);
				pEvent->addEvent();	
			}
			else if(m_op == AirsideCircuitFlightInSim::ForceAndGo_Operation){
				pRunwaysController->GenerateLandingCleaerance(pFlight,pRunway,pFlight->GetRunwayExit(),true, preItem,newClearance,true);
			}
		}	
		else if(pResource->GetType() == AirsideResource::ResType_HeadingAirRouteSegment)
		{
			HeadingAirRouteSegInSim* pHeadingSeg = (HeadingAirRouteSegInSim*)pResource;
			pHeadingSeg->GenerateClearanceItems(pFlight,pAirRouteInSim,ResourceList.at(i+1),preItem, newClearance);
		}
		else if(pResource->GetType() == AirsideResource::ResType_AirRouteSegment)
		{
			AirRouteSegInSim* pSeg = (AirRouteSegInSim*)pResource;

			AirRoutePointInSim* pFirstIntersection = pSeg->GetFirstConcernIntersection()->getInputPoint();
			FlightPlanPropertyInSim property = pAirRouteInSim->GetRoutePointProperty(pFirstIntersection);
			OccupancyInstance instance = pFirstIntersection->GetOccupyInstance(pFlight);


			ClearanceItem newItem(pResource,property.GetMode(),0);
			newItem.SetPosition(pFirstIntersection->GetPosition());

			if (preItem.GetPosition() == pFirstIntersection->GetPosition() && ((AirRouteSegInSim*)pResource)->GetFirstConcernIntersection()->GetConnectHold())
				newItem.SetAltitude(preItem.GetAltitude());
			else
				newItem.SetAltitude(property.GetAlt());

			newItem.SetSpeed(instance.GetSpeed());
			newItem.SetTime(instance.GetEnterTime());
			preItem = newItem;
			newClearance.AddItem(preItem);
			//Exit item
			
			ElapsedTime tExit = preItem.GetTime() + pAirRouteInSim->GetFlightTimeInSeg(pFlight,(AirRouteSegInSim*)pResource);
			

			pSeg->CalculateClearanceItems(pFlight,pAirRouteInSim,preItem,tExit,newClearance);

			if(AirRouteIntersectionInSim* pIntersectionNode = pSeg->GetSecondConcernIntersection())
			{
				AirRoutePointInSim* pSecondIntersection = pIntersectionNode->getInputPoint();

				FlightPlanPropertyInSim Pointprop = pAirRouteInSim->GetRoutePointProperty(pSecondIntersection);
				instance = pSecondIntersection->GetOccupyInstance(pFlight);

				if (pIntersectionNode->GetConnectHold())
				{
					CPoint2008 ATCManagedPoint = ((AirRouteSegInSim*)pResource)->GetATCManagedPoint(pFlight);
					preItem = newClearance.GetItem(newClearance.GetItemCount()-1);
					if (preItem.GetPosition() == ATCManagedPoint)
						return;

					double dRateInSegment = ATCManagedPoint.distance(preItem.GetPosition())/(preItem.GetPosition().distance(pSecondIntersection->GetPosition()));
					ElapsedTime dT = pAirRouteInSim->GetFlightTimeInSeg(pFlight,(AirRouteSegInSim*)pResource);
					ElapsedTime tTime = ElapsedTime(dT.asSeconds()*dRateInSegment) + preItem.GetTime();
					double dAlt = (1-dRateInSegment) * (preItem.GetAltitude() - Pointprop.GetAlt()) + Pointprop.GetAlt();
					preItem.SetAltitude(dAlt);
					preItem.SetPosition(ATCManagedPoint);
					preItem.SetTime(tTime);
					newClearance.AddItem(preItem);
					return;
				}
				else
				{
					ClearanceItem secondItem(pResource,Pointprop.GetMode(),0);
					secondItem.SetPosition(pSecondIntersection->GetPosition());
					secondItem.SetAltitude(Pointprop.GetAlt());
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
		}
		else{
			ASSERT(false);
		}
	}
	//lastItem = preItem;
}

bool ICirciteIntensionProcess::SchemeOnAirRoute( ClearanceItem& inOutItem, const AirsideResourceList& ResourceList )
{
	AirsideResource* pRes = ResourceList.front();
	if(pRes->GetType() == AirsideResource::ResType_HeadingAirRouteSegment)
	{
		HeadingAirRouteSegInSim* pHeadingSeg = (HeadingAirRouteSegInSim*)pRes;
		AirsideResource* pNextRes = ResourceList.at(1);
		FlightRouteInSim* pRoute = GetResourceRoute(pRes);
		pHeadingSeg->GetFirstIntersection()->getInputPoint()->SetEnterTime(m_pFlight,inOutItem.GetTime(),OnApproach,inOutItem.GetSpeed());
		pHeadingSeg->GetFirstIntersection()->getInputPoint()->SetExitTime(m_pFlight,inOutItem.GetTime());

		ClearanceItem endHeadingItem(pNextRes,OnApproach,0);
		FlightPlanPropertyInSim prop = pRoute->GetRoutePointProperty(pHeadingSeg->GetSecondIntersection()->getInputPoint());		
		endHeadingItem.SetAltitude(prop.GetAlt());
		endHeadingItem.SetSpeed(prop.GetSpeed());
		endHeadingItem.SetPosition(pHeadingSeg->GetSecondIntersection()->getInputPoint()->GetPosition());
		ElapsedTime tInHeading = pHeadingSeg->GetMinTimeInSegment(m_pFlight,inOutItem.GetSpeed(),prop.GetSpeed(),pNextRes);
		endHeadingItem.SetTime( inOutItem.GetTime() + tInHeading );

		AirsideResourceList leftRes = ResourceList;
		leftRes.erase(leftRes.begin());
		SchemeOnAirRoute(endHeadingItem, leftRes);   

		pHeadingSeg->SetEnterTime(m_pFlight,inOutItem.GetTime(), OnApproach,inOutItem.GetSpeed());
		pHeadingSeg->SetExitTime(m_pFlight,endHeadingItem.GetTime());


		pHeadingSeg->GetSecondIntersection()->getInputPoint()->SetEnterTime(m_pFlight,endHeadingItem.GetTime(),OnApproach,endHeadingItem.GetSpeed());
		pHeadingSeg->GetSecondIntersection()->getInputPoint()->SetExitTime(m_pFlight,endHeadingItem.GetTime());

		return true;
	}
	else
	{
		ClearanceItem iterItem = inOutItem;
		while(true){
			ElapsedTime tDelay;
			if( SchemeOnAirRouteOnce(iterItem,ResourceList,tDelay) )
			{
				inOutItem= iterItem;
				return true;
			}
			else
			{
				if(tDelay <= ElapsedTime(0L))
					tDelay = ElapsedTime(1L);
				iterItem.SetTime(iterItem.GetTime() + tDelay);
			}
		}
	}	
}

bool ICirciteIntensionProcess::_ProcessFullStopAndGo( ClearanceItem& lastItem, Clearance& newClearance )
{
	//do landing		
	if(lastItem.GetMode() == OnFinalApproach)
	{
		LogicRunwayInSim* pLandRunway = m_pFlight->GetLandingRunway();
		if(!pLandRunway){
			return false;
		}
		if( pLandRunway->IsRunwayClear(m_pFlight) ) //do normal landing
		{
			AirTrafficController* pATC = m_pFlight->GetAirTrafficController();
			CRunwaySystem *pRunwaysController =  pATC->GetRunwaysController();
			ClearanceItem TouchdownItem = pRunwaysController->GetRunwayTouchdownClearanceItem(m_pFlight,lastItem.GetTime(),lastItem);
			newClearance.AddItem(TouchdownItem);
			pATC->GetRunwaysController()->GetRunwayLandingClearance(m_pFlight,TouchdownItem,newClearance);
			return true;
		}
		else
		{
			GetMisApproach(pLandRunway,lastItem,newClearance);
			return true;
		}
	}
	
	//outbound process back to takeoff
	if(!m_pOutBoundProcess){
		ASSERT(lastItem.GetMode() == OnExitRunway);

		TaxiRouteInSim * pCircuitRoute = m_pFlight->GetCircuitTaxiwayRoute();
		if(!pCircuitRoute){
			CString strWarn = _T("No taxi route for the flight do the full stop and go");
			CString strError = _T("AIRCRAFT TERMINATE");
			AirsideSimErrorShown::SimWarning(m_pFlight,strWarn,strError);
			ClearanceItem newItem(NULL,OnTerminate,0);
			newItem.SetTime(lastItem.GetTime());
			newClearance.AddItem(newItem);
			m_pFlight->PerformClearance(newClearance);
			return true;
		}
		m_pOutBoundProcess = new OutBoundRouteProcess(m_pFlight,pCircuitRoute);
	}
	if(m_pOutBoundProcess->Process(lastItem,newClearance))
		return true;


	//takeoff process
	if(lastItem.GetMode() == OnTaxiToRunway || lastItem.GetMode() == OnTakeOffWaitEnterRunway){
		FlightRouteInSim * pCircuitPlan = m_pFlight->GetTakeoffCircuit();
		AirTrafficController* pATC = m_pFlight->GetAirTrafficController();
		AirRouteNetworkInSim* pA =  pATC->GetAirsideResourceManager()->GetAirspaceResource();

		/*if( pA->FindClearanceInConcern(m_pFlight, m_pTakeoffRoute, lastItem,newClearance) )
			return true;*/
		//if(btillEnd) return true; 
		if (!CheckIsTakeoffDelay(lastItem,newClearance,pCircuitPlan,AirsideCircuitFlightInSim::ForceAndGo_Operation,true))
		{
			ASSERT(FALSE);
			m_state = ps_completed;
			return false;
		}
		else
		{
			m_state = ps_completed;
			return true;
		}
	}
	return false;
}



FlightRouteInSim* ICirciteIntensionProcess::GetResourceRoute( AirsideResource* pRes ) const
{
	if(-1!= m_pLandingRoute->GetSegmentIndex(pRes))
		return m_pLandingRoute;
	if(-1!= m_pTakeoffRoute->GetSegmentIndex(pRes) )
		return m_pTakeoffRoute;

	ASSERT(false);
	return NULL;
}
