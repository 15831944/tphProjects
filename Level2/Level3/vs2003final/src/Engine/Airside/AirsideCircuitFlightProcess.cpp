#include "StdAfx.h"
#include ".\airsidecircuitflightprocess.h"
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
AirsideCircuitFlightProcess::AirsideCircuitFlightProcess(AirsideCircuitFlightInSim* pFlight,ClearanceItem& lastItem, Clearance& newClearance)
:m_pFlight(pFlight)
,m_lastItem(lastItem)
,m_newClearance(newClearance)
{
}

AirsideCircuitFlightProcess::~AirsideCircuitFlightProcess(void)
{
}

//-------------------------------------------------------------
//Summary:
//			Process order: start-->intension-->end
//-------------------------------------------------------------
void AirsideCircuitFlightProcess::Process()
{
	if (m_pFlight == NULL)
		return;
	
	AirTrafficController* pATC = m_pFlight->GetAirTrafficController();
	if (pATC == NULL)
		return;

	bool btillEnd = false;
	if(m_pFlight->GetCircuitState() == AirsideCircuitFlightInSim::START_PROCESS)
	{
		if (StartBegine())
		{
			btillEnd = ExecuteBegineStart(pATC);
	
			if(btillEnd) return;
		}
		else
		{
			btillEnd =ExecuteBegineStand(pATC);

			if(btillEnd) return;
		}
		m_pFlight->SetCircuitState(AirsideCircuitFlightInSim::INTENTION_PROCESS);
	}

	if(m_pFlight->GetCircuitState() == AirsideCircuitFlightInSim::INTENTION_PROCESS)
	{
		btillEnd = ExecuteIntensions(pATC);
		if(btillEnd) return;
	}
	

	if(m_pFlight->GetCircuitState() == AirsideCircuitFlightInSim::END_PROCESS)
	{
		if (SIDEnd())
		{
			btillEnd = ExecuteEndSID(pATC);
			if(btillEnd) return;
		}
		else
		{
			btillEnd = ExecuteEndStand(pATC);
			if(btillEnd) return;
		}
	}
	
}

bool AirsideCircuitFlightProcess::StartBegine() const
{
	if (m_pFlight == NULL)
		return false;
	
	return m_pFlight->getArrStand().IsBlank();
}

bool AirsideCircuitFlightProcess::SIDEnd() const
{
	if (m_pFlight == NULL)
		return false;

	return m_pFlight->getDepStand().IsBlank();
}

bool AirsideCircuitFlightProcess::ExecuteBegineStart(AirTrafficController* pATC)
{
	FlightRouteInSim * pArrPlan = m_pFlight->GetArrFlightPlan();
	if(pArrPlan && (m_lastItem.GetMode()< OnTerminal || m_lastItem.GetMode() == OnWaitInHold))
	{
		bool btillEnd = pATC->GetAirsideResourceManager()->GetAirspaceResource()->FindClearanceInConcern(m_pFlight,pArrPlan, m_lastItem,m_newClearance);
		if(btillEnd) return true;
	}


	//STAR
	FlightRouteInSim * pSTAR  = m_pFlight->GetSTAR();
	if(pSTAR && ((m_lastItem.GetMode() != OnApproach &&m_lastItem.GetMode() != OnLanding)|| m_lastItem.GetMode() == OnWaitInHold)) 
	{
		bool btillEnd = pATC->GetAirsideResourceManager()->GetAirspaceResource()->FindCircuitFlightLandingClearance(m_pFlight,pSTAR, m_lastItem,m_newClearance,m_pFlight->GetStartLandingOperation());			
		if(btillEnd) return true; 
	}

	return false;
}

bool AirsideCircuitFlightProcess::ExecuteBegineStand(AirTrafficController* pATC)
{
	if (m_lastItem.GetMode() == OnFinalApproach)
	{
		return false;
	}

	StandInSim* pArrStand = m_pFlight->GetPlanedParkingStand(ARR_PARKING);
	if(pArrStand == NULL || !pArrStand->TryLock(m_pFlight))
	{
		CString strWarn= _T("no Stand for circuit Flight");
		CString strError = _T("AIRCRAFT TERMINATE");
		AirsideSimErrorShown::SimWarning(m_pFlight,strWarn,strError);

		pATC->RemoveFlightPlanedStandOccupancyTime(m_pFlight, ARR_PARKING);

		ClearanceItem newItem(NULL,OnTerminate,0);
		newItem.SetTime(m_lastItem.GetTime());
		m_newClearance.AddItem(newItem);

		m_pFlight->PerformClearance(m_newClearance);
		return true;
		
	}	

	if (pArrStand && pArrStand->TryLock(m_pFlight))
	{
		pArrStand->GetLock(m_pFlight);
	}

	if(m_lastItem.GetMode()< OnExitStand  )
	{	
		bool btillEnd = pArrStand->FindClearanceInConcern(m_pFlight,m_lastItem,m_pFlight->GetDepTime(),m_newClearance);	
		if(btillEnd)return true;	
	}

	LogicRunwayInSim * pTakeoffRunway = m_pFlight->GetAndAssignTakeoffRunway();

	//OutBoundRoute			 
	TaxiRouteInSim * pOutBound  = m_pFlight->GetAndAssignCircuitOutBoundRoute();	
	TakeoffQueueInSim* pTakeoffQueue = pATC->GetAirsideResourceManager()->GetAirportResource()->getTakeoffQueuesManager()->GetRunwayExitQueue(m_pFlight->GetAndAssignTakeoffPosition());
	if(pOutBound && (m_lastItem.GetMode() == OnExitStand || m_lastItem.GetMode() == OnTaxiToRunway ))
	{
		pOutBound  = m_pFlight->GetOutBoundRoute();	
		double taxispd = m_pFlight->GetPerformance()->getTaxiOutNormalSpeed(m_pFlight);
		pOutBound->SetSpeed(taxispd);
		bool btillEnd = pOutBound->FindClearanceInConcern(m_pFlight,m_lastItem,50000, m_newClearance);
		if(btillEnd) return true;	
	}

	if (pOutBound == NULL)
	{
		CString strWarn = _T("No outbound route for the flight");
		CString strError = _T("AIRCRAFT TERMINATE");
		AirsideSimErrorShown::SimWarning(m_pFlight,strWarn,strError);

		ClearanceItem newItem(NULL,OnTerminate,0);
		newItem.SetTime(m_lastItem.GetTime());
		m_newClearance.AddItem(newItem);

		m_pFlight->PerformClearance(m_newClearance);
		return true;
	}

	//takeoff Queue
	if(pTakeoffQueue && (m_lastItem.GetMode() == OnTaxiToRunway || m_lastItem.GetMode() == OnQueueToRunway))
	{

		bool btillEnd = pTakeoffQueue->FindClearanceInConcern(m_pFlight,m_lastItem,m_newClearance);
		if(btillEnd) return true;
	}

	//Queue to take off
	if(pTakeoffRunway && m_lastItem.GetMode()< OnTakeOffEnterRunway)
	{
		if (pATC->GetTakeoffSequenceInSim()->IsWaitingInQueue(m_pFlight,m_lastItem.GetTime()))
		{
			m_pFlight->GetWakeUpCaller().OberverSubject((CAirsideSubject*)pATC->GetRunwayOccupancyStatus());
		
			return true;
		}
	}

	FlightRouteInSim* pCircuiteRoute = m_pFlight->GetTakeoffCircuit();
	bool bEnterSystem = CheckIsTakeoffDelay(pATC,pCircuiteRoute);
	if ( !bEnterSystem)
		m_newClearance.ClearItems();


	if (m_lastItem.GetMode() != OnClimbout)
	{
		m_pFlight->ChangeToDeparture();
		CRunwaySystem *pRunwaysController =  m_pFlight->GetAirTrafficController()->GetRunwaysController();
		std::vector<CRunwaySystem::RunwayOccupyInfo> vRunwayArrangeInfo;
		vRunwayArrangeInfo.clear();

		ElapsedTime dTimeToRwy;
		AirsideResource * pRes = m_lastItem.GetResource(); // add at the end of the taxiway item

		if (pRes && pRes->GetType() == AirsideResource::ResType_TaxiwayDirSeg)
		{
			CPath2008 filletPath = m_pFlight->GetFilletPathFromTaxiwayToRwyport(m_pFlight->GetAndAssignTakeoffRunway()->getLogicRunwayType());
			MobileTravelTrace travelTrace(m_pFlight->mGroundPerform,filletPath.GetTotalLength(),m_lastItem.GetSpeed(),0 );
			travelTrace.BeginUse();
			ElapsedTime dTimeToRwy = travelTrace.getEndTime();
		}

		//pRunwaysController->GetCircuitFlightTakeoffClearance(m_pFlight, dTimeToRwy, m_lastItem,m_newClearance,AirsideCircuitFlightInSim::LowAndOver_Operation);
		pRunwaysController->WriteCircuitFlightRunwayTakeoffLogs(m_pFlight,vRunwayArrangeInfo,m_newClearance);

		ClearanceItem RwyLiftOffItem = m_newClearance.GetItem(m_newClearance.GetItemCount() -1);

		AirRoutePointInSim* pRoutePoint = ((AirRouteSegInSim*)pCircuiteRoute->GetItem(1))->GetFirstConcernIntersection()->getInputPoint();

		ClearanceItem missItem(pRoutePoint,OnClimbout,0);		
		missItem.SetSpeed(pCircuiteRoute->GetRoutePointProperty(pRoutePoint).GetSpeed());
		double dAlt = pCircuiteRoute->GetRoutePointProperty(pRoutePoint).GetAlt();
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
		m_newClearance.AddItem(missItem);

		return true;
	}
		
	

	FlightRouteInSim* pLandCiruitRoute = m_pFlight->GetLandingCircuit();
	if(pCircuiteRoute && (m_lastItem.GetMode() == OnClimbout || m_lastItem.GetMode() == OnWaitInHold)) 
	{
		m_pFlight->ChangeToArrival();
		bool btillEnd = pATC->GetAirsideResourceManager()->GetAirspaceResource()->FindCircuitFlightLandingClearance(m_pFlight, pLandCiruitRoute, m_lastItem,m_newClearance,m_pFlight->GetLandingOperation());
 		if(btillEnd) return true; 
	}
	
	return false;
}

bool AirsideCircuitFlightProcess::ExecuteEndSID(AirTrafficController* pATC)
{
 	if (m_lastItem.GetMode() == OnFinalApproach)
 	{
 		m_pFlight->ChangeToDeparture();
 	}
	FlightRouteInSim * pSID  = m_pFlight->GetSID();
	//SID
	if(pSID && (m_lastItem.GetMode() <= OnTakeoff || m_lastItem.GetMode() == OnWaitInHold)) 
	{
		bool btillEnd = pATC->GetAirsideResourceManager()->GetAirspaceResource()->FindCircuitFlightTakeoffClearance(m_pFlight, pSID, m_lastItem,m_newClearance,AirsideCircuitFlightInSim::TouchAndGo_Operaiton);
		if(btillEnd) return true; 
	}

	//Departure Flight Plan
	FlightRouteInSim* pDepPlan = m_pFlight->GetDepFlightPlan();
	if(pDepPlan && (m_lastItem.GetMode() <= OnCruiseDep || m_lastItem.GetMode() == OnWaitInHold))
	{
		bool btillEnd = pATC->GetAirsideResourceManager()->GetAirspaceResource()->FindClearanceInConcern(m_pFlight,pDepPlan, m_lastItem,m_newClearance);
		if(btillEnd) return true; 
	}

	return false;
}

bool AirsideCircuitFlightProcess::ExecuteEndStand(AirTrafficController* pATC)
{
	if (m_lastItem.GetMode() == OnFinalApproach)
	{
		CRunwaySystem *pRunwaysController =  m_pFlight->GetAirTrafficController()->GetRunwaysController();
		ClearanceItem TouchdownItem = pRunwaysController->GetRunwayTouchdownClearanceItem(m_pFlight,m_lastItem.GetTime(),m_lastItem);
		m_newClearance.AddItem(TouchdownItem);


		m_pFlight->GetAirTrafficController()->GetRunwaysController()->GetRunwayLandingClearance(m_pFlight,TouchdownItem,m_newClearance);
		m_pFlight->ChangeToDeparture();
		return true;
	}

	StandInSim* pDepStand = m_pFlight->GetPlanedParkingStand(DEP_PARKING);
	if(pDepStand == NULL || !pDepStand->TryLock(m_pFlight))
	{
		pDepStand->GetLock(m_pFlight);
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
		newItem.SetTime(m_lastItem.GetTime());
		m_newClearance.AddItem(newItem);

		m_pFlight->PerformClearance(m_newClearance);
		return true;
	}

	if (pDepStand && pDepStand->TryLock(m_pFlight))
	{
		pDepStand->GetLock(m_pFlight);
	}

	// on exit Runway
	if(m_lastItem.GetMode() == OnExitRunway )
	{
		RunwayExitInSim* pRunwayExit = m_pFlight->GetRunwayExit(); 
		if(!pRunwayExit) // no runway exit to choose available
		{
			CString strWarn = _T("There is no available Runway Exit for Flight ");
			CString strError = _T("AIRCRAFT TERMINATE");
			AirsideSimErrorShown::SimWarning(m_pFlight,strWarn,strError);
			ClearanceItem newItem(NULL,OnTerminate,0);
			newItem.SetTime(m_lastItem.GetTime());
			m_newClearance.AddItem(newItem);
			if (pDepStand)
			{
				int nIdx = pDepStand->GetStandAssignmentRegister()->FindRecord(m_pFlight, DEP_PARKING);
				pDepStand->GetStandAssignmentRegister()->DeleteRecord(nIdx);
			}

			m_pFlight->PerformClearance(m_newClearance);
			return true;
		}
	}

	//route to stand
	TaxiRouteInSim * pRouteToStand = m_pFlight->GetAndAssignCircuitInBoundRoute();
	if( pRouteToStand && m_lastItem.GetMode() <= OnTaxiToStand ) //taxi to the parking stand
	{
		double taxispd = m_pFlight->GetPerformance()->getTaxiInNormalSpeed(m_pFlight);
		pRouteToStand->SetSpeed(taxispd);
		bool btillEnd = pRouteToStand->FindClearanceInConcern(m_pFlight,m_lastItem,5000,m_newClearance);
		if(btillEnd) return true;
	}

	if (pRouteToStand == NULL)
	{
		CString strWarn = _T("No inbound route for the flight");
		CString strError = _T("AIRCRAFT TERMINATE");
		AirsideSimErrorShown::SimWarning(m_pFlight,strWarn,strError);

		ClearanceItem newItem(NULL,OnTerminate,0);
		newItem.SetTime(m_lastItem.GetTime());
		m_newClearance.AddItem(newItem);
		m_pFlight->PerformClearance(m_newClearance);
		return true;
	}

	//Stand
	if(pDepStand && m_lastItem.GetMode() < OnHeldAtStand )		//to arrival flight
	{	

		m_pFlight->SetArrTime(m_lastItem.GetTime());		//update arrival time

		ElapsedTime tParkingTime = m_lastItem.GetTime() + m_pFlight->GetFlightInput()->getServiceTime();
		bool btillEnd = pDepStand->FindClearanceInConcern(m_pFlight,m_lastItem,tParkingTime,m_newClearance);		
		if(btillEnd) return true;
	}

	if (!m_pFlight->IsDeparture() && pDepStand && m_lastItem.GetMode() == OnHeldAtStand)
	{
		bool btillEnd = pDepStand->FindClearanceInConcern(m_pFlight,m_lastItem,m_lastItem.GetTime(),m_newClearance);		
		if(btillEnd) return true;
	}
	return false;
}

bool AirsideCircuitFlightProcess::ExecuteIntensions(AirTrafficController* pATC)
{
	bool btillEnd = ExecuteLowAndOver(pATC);
	if(btillEnd) return true;

	btillEnd = ExecuteTouchGo(pATC);
	if(btillEnd) return true;

	btillEnd = ExecuteStopGo(pATC);
	if(btillEnd) return true;

	btillEnd = ExecuteForceGo(pATC);
	if (btillEnd) return true;

	m_pFlight->SetCircuitState(AirsideCircuitFlightInSim::END_PROCESS);
	return false;
}

bool AirsideCircuitFlightProcess::ExecuteLowAndOver(AirTrafficController* pATC)
{
	if (!m_pFlight->LowOverDone()&&m_pFlight->MoveOnCircuitComplete())
	{
		m_pFlight->ReleaseLowOver();
		m_pFlight->ClearMoveCircuitRouteState();
	}

	if (!m_pFlight->LowOverDone())
	{
		//takeoff part
		FlightRouteInSim* pTakeoffCircuitRoute = m_pFlight->GetLandingCircuit();
		if (pTakeoffCircuitRoute)
		{
			if (m_lastItem.GetMode() == OnFinalApproach)
			{
				CRunwaySystem *pRunwaysController =  m_pFlight->GetAirTrafficController()->GetRunwaysController();
				//pRunwaysController->GenerateLowAndOverClearance(m_pFlight,pTakeoffCircuitRoute,m_lastItem,m_newClearance);
				m_pFlight->SetTakeoffComplete(true);
				return true;
			}
		}

		FlightRouteInSim * pCircuitPlan = m_pFlight->GetLandingCircuit();
		if(pCircuitPlan)
		{
			//Landing part
			if (m_lastItem.GetMode() != OnFinalApproach || m_lastItem.GetMode() == OnWaitInHold)
			{
				m_pFlight->ChangeToArrival();
				bool btillEnd = pATC->GetAirsideResourceManager()->GetAirspaceResource()->FindCircuitFlightLandingClearance(m_pFlight,pCircuitPlan, m_lastItem,m_newClearance,AirsideCircuitFlightInSim::LowAndOver_Operation);
				m_pFlight->SetLandingComplete(CheckOperationComplete(OnFinalApproach));
				if(btillEnd) return true;
			}
		}

	}
	
	return false;
}

bool AirsideCircuitFlightProcess::ExecuteTouchGo(AirTrafficController* pATC)
{
	if (!m_pFlight->TouchGoDone()&&m_pFlight->MoveOnCircuitComplete())
	{
		m_pFlight->ReleaseTouchGo();
		m_pFlight->ClearMoveCircuitRouteState();
	}

	if (!m_pFlight->TouchGoDone())
	{
		if (!m_pFlight->IsTakeoffComplete())
		{
			FlightRouteInSim * pCircuitPlan = m_pFlight->GetTakeoffCircuit();
			if(pCircuitPlan)
			{
				//takeoff part
				if(m_lastItem.GetMode() == OnFinalApproach || m_lastItem.GetMode() == OnWaitInHold)
				{
					m_pFlight->ChangeToDeparture();
					CRunwaySystem *pRunwaysController =  m_pFlight->GetAirTrafficController()->GetRunwaysController();
					std::vector<CRunwaySystem::RunwayOccupyInfo> vRunwayArrangeInfo;
					vRunwayArrangeInfo.clear();

					ElapsedTime dTimeToRwy;
					AirsideResource * pRes = m_lastItem.GetResource(); // add at the end of the taxiway item
				
					if (pRes && pRes->GetType() == AirsideResource::ResType_AirRouteSegment)
					{
						ClearanceItem landingItem = pRunwaysController->GetRunwayTouchdownClearanceItem(m_pFlight,m_lastItem.GetTime(),m_lastItem);
						dTimeToRwy = landingItem.GetTime() - m_lastItem.GetTime();
					}
					
					ElapsedTime tMoveTime;
					//pRunwaysController->GetCircuitFlightTakeoffClearance(m_pFlight, dTimeToRwy, m_lastItem,m_newClearance,AirsideCircuitFlightInSim::LowAndOver_Operation);
					pRunwaysController->WriteCircuitFlightRunwayTakeoffLogs(m_pFlight,vRunwayArrangeInfo,m_newClearance);

					//pRunwaysController->WriteCircuitFlightRunwayTakeoffLogs(m_pFlight,vRunwayArrangeInfo,tMoveTime,dTimeToRwy, m_lastItem,m_newClearance,AirsideCircuitFlightInSim::TouchAndGo_Operaiton);
					ClearanceItem RwyLiftOffItem = m_newClearance.GetItem(m_newClearance.GetItemCount() -1);

					AirRoutePointInSim* pRoutePoint = ((AirRouteSegInSim*)pCircuitPlan->GetItem(1))->GetFirstConcernIntersection()->getInputPoint();

					ClearanceItem missItem(pRoutePoint,OnClimbout,0);		
					missItem.SetSpeed(pCircuitPlan->GetRoutePointProperty(pRoutePoint).GetSpeed());
					double dAlt = pCircuitPlan->GetRoutePointProperty(pRoutePoint).GetAlt();
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
					m_newClearance.AddItem(missItem);

					m_pFlight->SetTakeoffComplete(true);
					return true;

				}
			}
		}
		
		if (!m_pFlight->IsLandingComplete())
		{
			//landing part
			FlightRouteInSim * pLandingCircuitPlan = m_pFlight->GetLandingCircuit();
			if (pLandingCircuitPlan)
			{
				if (m_lastItem.GetMode() == OnClimbout || m_lastItem.GetMode() == OnWaitInHold)
				{
					m_pFlight->ChangeToArrival();
					bool btillEnd = pATC->GetAirsideResourceManager()->GetAirspaceResource()->FindCircuitFlightLandingClearance(m_pFlight,pLandingCircuitPlan, m_lastItem,m_newClearance,AirsideCircuitFlightInSim::TouchAndGo_Operaiton);
					m_pFlight->SetLandingComplete(CheckOperationComplete(OnFinalApproach));
					if(btillEnd) return true;
				}
			}
		}
	
	}

	return false;
}

bool AirsideCircuitFlightProcess::ExecuteStopGo(AirTrafficController* pATC)
{
	if (!m_pFlight->StopGoDone()&&m_pFlight->MoveOnCircuitComplete())
	{
		m_pFlight->ReleaseStopGo();
		m_pFlight->ClearMoveCircuitRouteState();
	}

	if (!m_pFlight->StopGoDone())
	{
		if (!m_pFlight->IsTakeoffComplete())
		{
			FlightRouteInSim * pCircuitPlan = m_pFlight->GetTakeoffCircuit();
			if(pCircuitPlan)
			{
				//takeoff part
				if(m_lastItem.GetMode() == OnFinalApproach || m_lastItem.GetMode() == OnWaitInHold)
				{
					m_pFlight->ChangeToDeparture();
					CRunwaySystem *pRunwaysController =  m_pFlight->GetAirTrafficController()->GetRunwaysController();
					std::vector<CRunwaySystem::RunwayOccupyInfo> vRunwayArrangeInfo;
					vRunwayArrangeInfo.clear();

					ElapsedTime dTimeToRwy;
					AirsideResource * pRes = m_lastItem.GetResource(); // add at the end of the taxiway item

					if (pRes && pRes->GetType() == AirsideResource::ResType_AirRouteSegment)
					{
						ClearanceItem landingItem = pRunwaysController->GetRunwayTouchdownClearanceItem(m_pFlight,m_lastItem.GetTime(),m_lastItem);
						dTimeToRwy = landingItem.GetTime() - m_lastItem.GetTime();
					}

					//pRunwaysController->GetCircuitFlightTakeoffClearance(m_pFlight, dTimeToRwy, m_lastItem,m_newClearance,AirsideCircuitFlightInSim::LowAndOver_Operation);
					pRunwaysController->WriteCircuitFlightRunwayTakeoffLogs(m_pFlight,vRunwayArrangeInfo,m_newClearance);

					//ElapsedTime tMoveTime;
					//pRunwaysController->WriteCircuitFlightRunwayTakeoffLogs(m_pFlight,vRunwayArrangeInfo,tMoveTime,dTimeToRwy, m_lastItem,m_newClearance,AirsideCircuitFlightInSim::StopAndGo_Operaiton);
					ClearanceItem RwyLiftOffItem = m_newClearance.GetItem(m_newClearance.GetItemCount() -1);

					AirRoutePointInSim* pRoutePoint = ((AirRouteSegInSim*)pCircuitPlan->GetItem(1))->GetFirstConcernIntersection()->getInputPoint();

					ClearanceItem missItem(pRoutePoint,OnClimbout,0);		
					missItem.SetSpeed(pCircuitPlan->GetRoutePointProperty(pRoutePoint).GetSpeed());
					double dAlt = pCircuitPlan->GetRoutePointProperty(pRoutePoint).GetAlt();
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
					m_newClearance.AddItem(missItem);

					m_pFlight->SetTakeoffComplete(true);
					return true;

				}
			}
		}

		if (!m_pFlight->IsLandingComplete())
		{
			//landing part
			FlightRouteInSim * pLandingCircuitPlan = m_pFlight->GetLandingCircuit();
			if (pLandingCircuitPlan)
			{
				if (m_lastItem.GetMode() == OnClimbout || m_lastItem.GetMode() == OnWaitInHold)
				{
					m_pFlight->ChangeToArrival();
					bool btillEnd = pATC->GetAirsideResourceManager()->GetAirspaceResource()->FindCircuitFlightLandingClearance(m_pFlight,pLandingCircuitPlan, m_lastItem,m_newClearance,AirsideCircuitFlightInSim::StopAndGo_Operaiton);
					m_pFlight->SetLandingComplete(CheckOperationComplete(OnFinalApproach));
					if(btillEnd) return true;
				}
			}
		}
	}

	return false;
}


bool AirsideCircuitFlightProcess::ExecuteForceGo(AirTrafficController* pATC)
{
	if (m_pFlight->MoveOnCircuitComplete())
	{
		m_pFlight->ReleaseForceGo();
		m_pFlight->ClearMoveCircuitRouteState();
	}

	if (!m_pFlight->ForceGoDone())
	{
		bool btillEnd = GetForceGoNextClearance(pATC);
		if (btillEnd) return true;
	}

	return false;
}
bool AirsideCircuitFlightProcess::MoveOnCircuitRoute(AirTrafficController* pATC,AirsideCircuitFlightInSim::LandingOperation emTyp)
{

		//takeoff part
		//{
		//	FlightRouteInSim * pTakeoffCircuitPlan = m_pFlight->GetTakeoffCircuit();
		//	if(pTakeoffCircuitPlan && m_lastItem.GetMode() == OnTakeoff)
		//	{
		//		m_pFlight->GetAirTrafficController()->GetRunwaysController()->GenerateRunwayToCircuitRouteClearance(m_pFlight,pTakeoffCircuitPlan,m_lastItem,m_newClearance);
		//		if (m_pFlight->GetLandingRunway())
		//		{
		//			ResourceExitEvent* pEvent = new ResourceExitEvent(m_pFlight->GetLandingRunway(), m_pFlight);
		//			pEvent->setTime(m_lastItem.GetTime());
		//			pEvent->addEvent();	
		//			m_pFlight->GetLandingRunway()->SetExitTime(m_pFlight,m_lastItem.GetTime());
		//		}		
		//		return true;
		//	}
		//}
	
		//landing part
		{
			FlightRouteInSim * pLandingCircuitPlan = m_pFlight->GetLandingCircuit();
			if (pLandingCircuitPlan)
			{
				if (m_lastItem.GetMode() == OnClimbout || m_lastItem.GetMode() == OnWaitInHold)
				{
					bool btillEnd = pATC->GetAirsideResourceManager()->GetAirspaceResource()->FindCircuitFlightLandingClearance(m_pFlight,pLandingCircuitPlan, m_lastItem,m_newClearance,emTyp);
					if(btillEnd) return true;
				}
			}
		}
	

	return false;
}

bool AirsideCircuitFlightProcess::GetForceGoNextClearance(AirTrafficController* pATC)
{
	if (!m_pFlight->IsTakeoffComplete())
	{
		if (m_lastItem.GetMode() == OnFinalApproach)
		{
			CRunwaySystem *pRunwaysController =  m_pFlight->GetAirTrafficController()->GetRunwaysController();
			ClearanceItem TouchdownItem = pRunwaysController->GetRunwayTouchdownClearanceItem(m_pFlight,m_lastItem.GetTime(),m_lastItem);
			m_lastItem = TouchdownItem;
			m_newClearance.AddItem(m_lastItem);


			m_pFlight->GetAirTrafficController()->GetRunwaysController()->GetRunwayLandingClearance(m_pFlight,TouchdownItem,m_newClearance);
			return true;
		}

		// on exit Runway
		if(m_lastItem.GetMode() == OnExitRunway )
		{
			RunwayExitInSim* pRunwayExit = m_pFlight->GetRunwayExit(); 
			if(!pRunwayExit) // no runway exit to choose available
			{
				CString strWarn = _T("There is no available Runway Exit for Flight ");
				CString strError = _T("AIRCRAFT TERMINATE");
				AirsideSimErrorShown::SimWarning(m_pFlight,strWarn,strError);
				ClearanceItem newItem(NULL,OnTerminate,0);
				newItem.SetTime(m_lastItem.GetTime());
				m_newClearance.AddItem(newItem);

				m_pFlight->PerformClearance(m_newClearance);
				return true;
			}
		}

		TaxiRouteInSim * pCircuitRoute = m_pFlight->GetCircuitTaxiwayRoute();
		LogicRunwayInSim * pTakeoffRunway = m_pFlight->GetAndAssignTakeoffRunway();

		//OutBoundRoute			 
		TakeoffQueueInSim* pTakeoffQueue = pATC->GetAirsideResourceManager()->GetAirportResource()->getTakeoffQueuesManager()->GetRunwayExitQueue(m_pFlight->GetAndAssignTakeoffPosition());
		if(pCircuitRoute && m_lastItem.GetMode() == OnExitRunway )
		{
			m_pFlight->SetMode(OnTaxiToRunway);
			//m_lastItem.SetMode(OnTaxiToRunway);
			pCircuitRoute  = m_pFlight->GetCircuitTaxiwayRoute();	
			double taxispd = m_pFlight->GetPerformance()->getTaxiOutNormalSpeed(m_pFlight);
			pCircuitRoute->SetSpeed(taxispd);	
			bool btillEnd = pCircuitRoute->FindClearanceInConcern(m_pFlight,m_lastItem,50000, m_newClearance);
			if(btillEnd)
				return true;	
		}

		if(m_lastItem.GetMode() == OnTaxiToRunway )
		{
			bool btillEnd = pCircuitRoute->FindClearanceInConcern(m_pFlight,m_lastItem,50000, m_newClearance);
			if(btillEnd)
				return true;	
		}


		if (pCircuitRoute == NULL)
		{
			CString strWarn = _T("No outbound route for the flight");
			CString strError = _T("AIRCRAFT TERMINATE");
			AirsideSimErrorShown::SimWarning(m_pFlight,strWarn,strError);

			ClearanceItem newItem(NULL,OnTerminate,0);
			newItem.SetTime(m_lastItem.GetTime());
			m_newClearance.AddItem(newItem);

			m_pFlight->PerformClearance(m_newClearance);
			return true;
		}

		//takeoff Queue
		if(pTakeoffQueue && (m_lastItem.GetMode() == OnTaxiToRunway || m_lastItem.GetMode() == OnQueueToRunway))
		{

			bool btillEnd = pTakeoffQueue->FindClearanceInConcern(m_pFlight,m_lastItem,m_newClearance);
			if(btillEnd) return true;
		}

		//Queue to take off
		if(pTakeoffRunway && m_lastItem.GetMode()< OnTakeOffEnterRunway)
		{
			if (pATC->GetTakeoffSequenceInSim()->IsWaitingInQueue(m_pFlight,m_lastItem.GetTime()))
			{
				m_pFlight->GetWakeUpCaller().OberverSubject((CAirsideSubject*)pATC->GetRunwayOccupancyStatus());

				return true;
			}
		}

		FlightRouteInSim* pTakeoffCircuitRoute = m_pFlight->GetTakeoffCircuit();
		bool bEnterSystem = CheckIsTakeoffDelay(pATC,pTakeoffCircuitRoute);
		if ( !bEnterSystem)
			m_newClearance.ClearItems();


		if (m_lastItem.GetMode() != OnClimbout)
		{
			m_pFlight->ChangeToDeparture();
			CRunwaySystem *pRunwaysController =  m_pFlight->GetAirTrafficController()->GetRunwaysController();
			std::vector<CRunwaySystem::RunwayOccupyInfo> vRunwayArrangeInfo;
			vRunwayArrangeInfo.clear();

			ElapsedTime dTimeToRwy;
			AirsideResource * pRes = m_lastItem.GetResource(); // add at the end of the taxiway item

			if (pRes && pRes->GetType() == AirsideResource::ResType_TaxiwayDirSeg)
			{
				CPath2008 filletPath = m_pFlight->GetFilletPathFromTaxiwayToRwyport(m_pFlight->GetAndAssignTakeoffRunway()->getLogicRunwayType());
				MobileTravelTrace travelTrace(m_pFlight->mGroundPerform,filletPath.GetTotalLength(),m_lastItem.GetSpeed(),0 );
				travelTrace.BeginUse();
				ElapsedTime dTimeToRwy = travelTrace.getEndTime();
			}

			//pRunwaysController->GetCircuitFlightTakeoffClearance(m_pFlight, dTimeToRwy, m_lastItem,m_newClearance,AirsideCircuitFlightInSim::TouchAndGo_Operaiton);
			pRunwaysController->WriteCircuitFlightRunwayTakeoffLogs(m_pFlight,vRunwayArrangeInfo,m_newClearance);

			//ElapsedTime tMoveTime;
			//pRunwaysController->WriteCircuitFlightRunwayTakeoffLogs(m_pFlight,vRunwayArrangeInfo,tMoveTime,dTimeToRwy, m_lastItem,m_newClearance,AirsideCircuitFlightInSim::TouchAndGo_Operaiton);
			ClearanceItem RwyLiftOffItem = m_newClearance.GetItem(m_newClearance.GetItemCount() -1);

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
			m_newClearance.AddItem(missItem);
			m_pFlight->SetTakeoffComplete(true);
			return true;
		}

	}
	

	if (!m_pFlight->IsLandingComplete())
	{
		FlightRouteInSim* pLandCiruitRoute = m_pFlight->GetLandingCircuit();
		if(pLandCiruitRoute && (m_lastItem.GetMode() == OnClimbout || m_lastItem.GetMode() == OnWaitInHold)) 
		{
			m_pFlight->ChangeToArrival();
			bool btillEnd = pATC->GetAirsideResourceManager()->GetAirspaceResource()->FindCircuitFlightLandingClearance(m_pFlight, pLandCiruitRoute, m_lastItem,m_newClearance,m_pFlight->GetLandingOperation());
			m_pFlight->SetLandingComplete(CheckOperationComplete(OnFinalApproach));
			if(btillEnd) return true; 
		}
	}
	

	return false;
}

bool AirsideCircuitFlightProcess::CheckOperationComplete( AirsideMobileElementMode mode)const
{
	for (int i = 0; i < m_newClearance.GetItemCount(); i++)
	{
		ClearanceItem lastItem = m_newClearance.GetItem(i);
		if (lastItem.GetMode() == mode)
		{
			return true;
		}
	}
	return false;
}

bool AirsideCircuitFlightProcess::CheckIsTakeoffDelay(AirTrafficController* pATC,FlightRouteInSim *pAirRouteInSim)
{
	if( pAirRouteInSim->GetItemCount() == 0)
		return false;

	AirsideResourceList resList;
	AirsideResource* pCurrentRes = m_lastItem.GetResource();

	ClearanceItem currentItem = m_lastItem;
	pAirRouteInSim->GetConcernRouteSegs(pCurrentRes,resList);
	if ( resList.at(0)->GetType()== AirsideResource::ResType_LogicRunway)	//take off	
	{	
		if (m_pFlight->GetAndAssignTakeoffRunway() == NULL)		// no takeoff runway
		{
			ClearanceItem newItem(NULL,OnTerminate,0);
			newItem.SetTime(m_lastItem.GetTime());
			m_newClearance.AddItem(newItem);

			m_pFlight->PerformClearance(m_newClearance);
			return true;
		}

		CRunwaySystem *pRunwaysController =  m_pFlight->GetAirTrafficController()->GetRunwaysController();
		std::vector<CRunwaySystem::RunwayOccupyInfo> vRunwayArrangeInfo;
		vRunwayArrangeInfo.clear();
		bool bNeedCreateTakeOffEvent = false;

		ElapsedTime dTimeToRwy;
		AirsideResource * pRes = currentItem.GetResource(); // add at the end of the taxiway item
		if (pRes)
		{
			if(pRes && pRes->GetType() == AirsideResource::ResType_TaxiwayDirSeg)
			{
				CPath2008 filletPath = m_pFlight->GetFilletPathFromTaxiwayToRwyport(m_pFlight->GetAndAssignTakeoffRunway()->getLogicRunwayType());
				MobileTravelTrace travelTrace(m_pFlight->mGroundPerform,filletPath.GetTotalLength(),currentItem.GetSpeed(),0 );
				travelTrace.BeginUse();
				dTimeToRwy = travelTrace.getEndTime();
			}
		}
		ElapsedTime tMoveTime =0L;
		bool bTakeoff = pATC->GetAirsideResourceManager()->GetAirspaceResource()->FlightSchemeOnAirRouteTakeOff(m_pFlight, pAirRouteInSim, resList, currentItem, dTimeToRwy, tMoveTime, vRunwayArrangeInfo);

		if (!bTakeoff)
			return false;

		if (currentItem.GetTime() < tMoveTime)
			bNeedCreateTakeOffEvent = true;


		currentItem.SetTime(tMoveTime);
		//pRunwaysController->GetCircuitFlightTakeoffClearance(m_pFlight, dTimeToRwy, m_lastItem,m_newClearance,AirsideCircuitFlightInSim::LowAndOver_Operation);
		pRunwaysController->WriteCircuitFlightRunwayTakeoffLogs(m_pFlight,vRunwayArrangeInfo,m_newClearance);

		ClearanceItem RwyLiftOffItem = m_newClearance.GetItem(m_newClearance.GetItemCount() -1);

		if (bNeedCreateTakeOffEvent)
		{
			//wait log
			//start waiting event
			ClearanceItem StartwaitItem = currentItem;	
			StartwaitItem.SetMode(OnTakeOffWaitEnterRunway);

			//wait item
			ClearanceItem waitItem = m_lastItem;
			//ElapsedTime tMoveTime = max((eTakeOffTime - dTimeToRwy), lastItem.GetTime());
			waitItem.SetTime(tMoveTime);

			waitItem.SetMode(OnTakeOffWaitEnterRunway);
			m_pFlight->PerformClearanceItem(StartwaitItem);
			m_pFlight->PerformClearanceItem(waitItem);

			//create take off event, take off at takeoff time
			if(0)
			{
				m_newClearance.ClearItems();
				FlightGetClearanceEvent * pNextEvent = new FlightGetClearanceEvent(m_pFlight);
				pNextEvent->setTime(tMoveTime);
				pNextEvent->addEvent();
			}
			else
			{
				CFlightDelayTakeoffOnRwyEvent * pTakeOffEvent = new CFlightDelayTakeoffOnRwyEvent(m_pFlight,m_newClearance);
				pTakeOffEvent->setTime(tMoveTime);
				pTakeOffEvent->addEvent();
				m_newClearance.ClearItems();
			}
		}

		return false;
	}
	return true;
}


