#include "StdAfx.h"
#include "../../Common/point.h"
#include ".\airtrafficcontroller.h"
#include "./AirsideFlightInSim.h"
#include "Clearance.h"
#include "./FlightPlanInSim.h"
#include "./StandInSim.h"
#include "./RunwayInSim.h"
#include "./AirsideResourceManager.h"
#include "./TaxiRouteInSim.h"
#include "./FlightPerformanceManager.h"
#include "../../InputAirside/FlightPlan.h"
#include "../../common/ALTObjectID.h"
//#include "InboundRouteInSimList.h"
//#include "OutboundRouteInSimList.h"
#include "AircraftClassificationManager.h"
#include "DynamicStandAssignmentInSim.h"
#include "OccupiedAssignedStandActionInSim.h"
#include "FlightServiceReqirementInSim.h"
#include "TemporaryParkingInSim.h"
#include "TempParkingSegmentsInSim.h"
#include "TaxiwayResource.h"

#include "../../Common/AirportDatabase.h"
#include "Clearance.h"
#include "PushBackClearanceInSim.h"
#include "StandBufferInSim.h"
#include "MissApproachFindClearanceInConcern.h"
#include "TaxiwayConflictResolutionInEngine.h"
#include "../../Inputs/GateAssignmentConstraints.h"
#include "../../Engine/terminal.h"
#include "TakeoffSeparationInSim.h"
#include "InTrailSeparationInSim.h"
#include "ApproachSeparationInSim.h"
#include "RunwaysController.h"

#include "AirportStatusManager.h"
#include "InboundStatus.h"
#include "OutboundStatus.h"
#include "TakeoffQueueStatus.h"
#include "AirsideObserver.h"
#include "SimulationErrorShow.h"
#include "PushbackClearanceDelay.h"
#include "TakeoffSequencingInSim.h"
#include "RunwayOccupancyStatus.h"
#include "TaxiInTempParkingDelay.h"
#include "../../Inputs/Stand2GateConstraint.h"
#include "StandInSim.h"
#include "FlightConflictAtWaypointDelay.h"
#include "StandAssignmentRegisterInSim.h"
#include "../../InputAirside/TowOffStandAssignmentData.h"
//#include "../../InputAirside/ALTObjectGroup.h"
#include "../../Inputs/flight.h"
#include "In_OutBoundRouteAssignmentInSim.h"
#include "AirspaceResourceManager.h"
//#include "../BoardingCallPolicy.h"
#include "AirsideItinerantFlightInSim.h"
#include "FlightGroundRouteResourceManager.h"
#include "../../Inputs/FlightPriorityInfo.h"
#include "../../InputAirside/ACTypeStandConstraint.h"
#include "TaxiInboundRouteInSim.h"
#include "TaxiOutboundRouteInSim.h"
#include "TaxiTostandRouteInSim.h"
#include ".\WeatherInSim.h"
#include ".\AircraftInitSurfaceSettingInSim.h"
#include "FlightOnToDeice.h"
#include <inputAirside/DeiceAndAnticeManager.h>
#include <common/ProbabilityDistribution.h>
#include <common/ProDistrubutionData.h>
#include "./VehicleRequestDispatcher.h"
#include "Push_TowOperationSpecInSim.h"
#include "./StandLeadInLineInSim.h"
#include "./StandLeadOutLineInSim.h"
#include "AirsideFlightDeiceRequirment.h"
#include "DeiceVehicleServiceRequest.h"
#include "..\..\InputAirside\TaxiInterruptTimes.h"
#include "../../InputAirside/TowOffStandAssignmentDataList.h"
#include "../../InputAirside/TowOffStandAssignmentData.h"
#include "../../Common/ARCTracker.h"
#include "../../Results/AirsideFllightRunwayDelayLog.h"
#include "../FltOperatingDoorSpecInSim.h"
#include "FlightApplyForTowServiceEvent.h"
#include "./DeiceRouteInSim.h"
#include "AirsideFollowMeCarInSim.h"
#include "FlightMoveToMeetingPointEvent.h"
#include "AirsideMeetingPointInSim.h"
#include "DeicepadGroup.h"
#include "../ARCportEngine.h"
#include "AirsideSimulation.h"
#include "..\..\InputAirside\OccupiedAssignedStandAction.h"
#include "../OnboardFlightInSim.h"
#include "DynamicConflictGraph.h"
#include "AirsideCircuitFlightInSim.h"
#include "AirsideCircuitFlightProcess.h"

//static DistanceUnit RadiusOfConcernOnAir = 100000;  // 1000 meters
static DistanceUnit RadiusOfConcernOnGround = 50000;   //200 meters




AirTrafficController::AirTrafficController()
:m_pPushAndTowOperationSpecInSim(NULL)
,m_pAirportDB(NULL)
,m_pFltOperatingDoorSpec(NULL)
{
	PLACE_METHOD_TRACK_STRING();
	//m_pInboundRouteInSimList = new CInboundRouteInSimList;
	//m_pOutboundrouteInSimList = new COutboundRouteInSimList;
	m_pBoundRouteAssignment = new In_OutBoundRouteAssignmentInSim;
	m_pDynamicStandReassignment = new CDynamicStandAssignmentInSim;
	//m_pAircraftSeparationManager = new AircraftSeparationManager;
	m_pOccupiedAssignedStandActionInSim = new COccupiedAssignedStandActionInSim;
	m_pFlightServiceRequirement = new FlightServiceReqirementInSim;
	m_pPushBackClearance = new PushBackClearanceInSim;
	m_pTemporaryParking = new TemporaryParkingInSim;
	m_pStandBuffer = new StandBufferInSim;
	m_pTaxiwayConflictResolution = NULL;

	m_pFlightPlan = new FlightPlanInSim;

	m_pInTrailSeparationInSim  = new CInTrailSeparationInSim;


	m_pRunwaysController = new CRunwaySystem();
	m_pArrivalDelayTrigger = new ArrivalDelayTriggerInSim();

	m_pAirportStatusManager = new AirportStatusManager;
	m_pInboundStatus = new InboundStatus(m_pAirportStatusManager);
	m_pOutboundStatus = new OutboundStatus(m_pAirportStatusManager);
	m_pTakeoffQueueStatus = new TakeoffQueueStatus(m_pAirportStatusManager);
	m_pRunwayOccupancyStatus = new RunwayOccupancyStatus(m_pAirportStatusManager);

	m_pTakeoffSequencingInSim = new TakeoffSequencingInSim();
	m_pResources = NULL;

//	m_pBoardingCallPolicy = new CBoardingCallPolicy() ;

//	m_pPaxTakeOffFlightPolicy = new CPassengerTakeOffCallPolicy() ;
	m_pTowOffCriteria = NULL;

	m_pConfilctGraph = new DynamicConflictGraph();

	//
	m_pWeatherScript = NULL;
	m_aircftSurSet = NULL;
	m_pDeiceManager =  NULL;
	m_pDeiceDemand = NULL;

	m_pInterruptTimes = NULL;
	m_pStand2gateConstraint=NULL;
}

AirTrafficController::~AirTrafficController(void)
{
	PLACE_METHOD_TRACK_STRING();
// 	if(m_pBoardingCallPolicy != NULL)
// 		delete m_pBoardingCallPolicy ;
// 	if(m_pPaxTakeOffFlightPolicy != NULL)
// 		delete m_pPaxTakeOffFlightPolicy ;
	//delete m_pInboundRouteInSimList;
	//delete m_pOutboundrouteInSimList;
	//delete m_pAircraftSeparationManager;

	delete m_pBoundRouteAssignment;
	m_pBoundRouteAssignment = NULL;

	delete m_pDynamicStandReassignment;
	m_pDynamicStandReassignment = NULL;

	delete m_pOccupiedAssignedStandActionInSim;
	m_pOccupiedAssignedStandActionInSim = NULL;

	delete m_pFlightServiceRequirement;
	m_pFlightServiceRequirement = NULL;

	delete m_pPushBackClearance;
	m_pPushBackClearance = NULL;

	delete m_pTemporaryParking;
	m_pTemporaryParking = NULL;

	delete m_pStandBuffer;
	m_pStandBuffer = NULL;

	delete m_pFlightPlan;
	m_pFlightPlan = NULL;

	delete m_pArrivalDelayTrigger;
	m_pArrivalDelayTrigger = NULL;



	if (NULL != m_pInTrailSeparationInSim)
	{
		delete m_pInTrailSeparationInSim;
		m_pInTrailSeparationInSim = NULL;
	}

	delete m_pAirportStatusManager;
	m_pAirportStatusManager = NULL;

	delete m_pInboundStatus;
	m_pInboundStatus = NULL;

	delete m_pOutboundStatus;
	m_pOutboundStatus = NULL;

	delete m_pTakeoffQueueStatus;
	m_pTakeoffQueueStatus = NULL;

	delete m_pRunwayOccupancyStatus;
	m_pRunwayOccupancyStatus = NULL;

	delete m_pTakeoffSequencingInSim;
	m_pTakeoffSequencingInSim = NULL;

	delete m_pTemporaryParking;

	delete m_pWeatherScript; m_pWeatherScript = NULL;
	delete m_aircftSurSet; m_aircftSurSet = NULL;
	delete m_pDeiceManager; m_pDeiceManager = NULL;
	delete m_pDeiceDemand; m_pDeiceDemand = NULL;

	delete m_pPushAndTowOperationSpecInSim;
	m_pPushAndTowOperationSpecInSim = NULL;

	delete m_pInterruptTimes;
	m_pInterruptTimes = NULL;

	delete m_pConfilctGraph;
	m_pConfilctGraph = NULL;
}

//--------------------------------------------------------------------------------------------------------------------------
//Summary:
//			Start-->Intension-->End Point
//Start: Start/Stand
//Intension: 
//		1. Low and over
//		2. Touch and go
//		3. Stop and go
//End: SID/Stand
//--------------------------------------------------------------------------------------------------------------------------
void AirTrafficController::GetNextCircuitClearance(AirsideCircuitFlightInSim* pFlight,ClearanceItem& lastItem, Clearance& newClearance )
{
	PLACE_METHOD_TRACK_STRING();
	AirsideCircuitFlightProcess circuitFlightProcess(pFlight,lastItem,newClearance);
	circuitFlightProcess.Process();
	newClearance = circuitFlightProcess.GetClearance();
}

void AirTrafficController::GetNextClearance( AirsideFlightInSim * pFlight, Clearance& newclearance )
{
	PLACE_METHOD_TRACK_STRING();
	bool btillEnd = false;	
	//pFlight->SetListenFlight(NULL);

	ClearanceItem lastClearanceItem(pFlight->GetResource(),pFlight->GetMode(),pFlight->GetDistInResource());
	lastClearanceItem.SetSpeed(pFlight->GetSpeed());
	lastClearanceItem.SetTime(pFlight->GetTime());
	lastClearanceItem.SetAltitude(pFlight->GetCurState().m_dAlt);

	CPoint2008 pos;
	pos.setX(pFlight->GetPosition().getX());
	pos.setY(pFlight->GetPosition().getY());
	lastClearanceItem.SetPosition(pos);
	lastClearanceItem.SetPushback(pFlight->GetCurState().m_bPushBack);

	if(pFlight->IsCircuitFlight())
	{
		AirsideCircuitFlightInSim* pCircuitFlight = (AirsideCircuitFlightInSim*)pFlight;
		if (pCircuitFlight->GetLandingCircuit() == NULL || pCircuitFlight->GetTakeoffCircuit() == NULL)
		{
			CString strWarn = _T("No circuit route for the flight");
			CString strError = _T("AIRCRAFT TERMINATE");
			AirsideSimErrorShown::SimWarning(pFlight,strWarn,strError);

			ClearanceItem newItem(NULL,OnTerminate,0);
			newItem.SetTime(lastClearanceItem.GetTime());
			newclearance.AddItem(newItem);

			pFlight->PerformClearance(newclearance);
			return;
		}
		return pCircuitFlight->GetNextClearance(lastClearanceItem,newclearance);
		//return GetNextCircuitClearance(pCircuitFlight,lastClearanceItem,newclearance);
	}

	if (pFlight->IsItinerantFlight() && pFlight->IsThroughOut())
	{
		FlightRouteInSim* pEnRoute = ((AirsideItinerantFlightInSim*)pFlight)->GetEnroute(m_pResources->GetAirspaceResource());
		if (pEnRoute == NULL)
		{
			CString strWarn = _T("There is no available en-route for itinerant flight through out");
			CString strError = _T("AIRCRAFT TERMINATE");
			AirsideSimErrorShown::SimWarning(pFlight,strWarn,strError);

			ClearanceItem newItem(NULL,OnTerminate,0);
			newItem.SetTime(lastClearanceItem.GetTime());
			newclearance.AddItem(newItem);
			pFlight->PerformClearance(newclearance);
			return;
		}
		if (pEnRoute &&(lastClearanceItem.GetMode() == OnCruiseThrough ||lastClearanceItem.GetMode() == OnBirth))
		{
			btillEnd = m_pResources->GetAirspaceResource()->FindClearanceInConcern(pFlight,pEnRoute, lastClearanceItem,newclearance);
			if(btillEnd) return ;
		}
	}

	if (pFlight->IsThroughOut())
		return;

	// for flight arrival
	if(pFlight->IsArrivingOperation())
	{

		//Arr Flight Plan
		FlightRouteInSim * pArrPlan = pFlight->GetArrFlightPlan();
		if(pArrPlan && (lastClearanceItem.GetMode()< OnTerminal || lastClearanceItem.GetMode() == OnWaitInHold))
		{

			btillEnd = m_pResources->GetAirspaceResource()->FindClearanceInConcern(pFlight,pArrPlan, lastClearanceItem,newclearance);
			if(btillEnd) return ;
		}


		//STAR
		FlightRouteInSim * pSTAR  = pFlight->GetSTAR();
		if(pSTAR && (lastClearanceItem.GetMode() < OnExitRunway || lastClearanceItem.GetMode() == OnWaitInHold)) 
		{
			btillEnd = m_pResources->GetAirspaceResource()->FindClearanceInConcern(pFlight,pSTAR, lastClearanceItem,newclearance);			
			if(btillEnd) return; 
		}

		if (pFlight->GetOperationParkingStand() == NULL)		//to get arrival stand
		{
			AssignOperationParkingStand(pFlight, ARR_PARKING);

			if (pFlight->GetOperationParkingStand() == NULL)		//if reassigned and no stand parking show error
			{
				CString strWarn = _T("no Stand for Arrival Flight ,need temporary parking place");
				CString strError = _T("AIRCRAFT TERMINATE");
				AirsideSimErrorShown::SimWarning(pFlight,strWarn,strError);

				StandInSim* pPlanedStand = pFlight->GetPlanedParkingStand(ARR_PARKING);
				if (pPlanedStand)
				{
					int oldIdx = pPlanedStand->GetStandAssignmentRegister()->FindRecord(pFlight,ARR_PARKING);
					pPlanedStand->GetStandAssignmentRegister()->DeleteRecord(oldIdx);
				}

				ClearanceItem newItem(NULL,OnTerminate,0);
				newItem.SetTime(lastClearanceItem.GetTime());
				newclearance.AddItem(newItem);

				pFlight->PerformClearance(newclearance);
				return;
			}
		}


		StandInSim* pParkingStand = pFlight->GetOperationParkingStand();
		// on exit Runway
		if(lastClearanceItem.GetMode() == OnExitRunway )
		{
			RunwayExitInSim* pRunwayExit = pFlight->GetRunwayExit(); 
			if(!pRunwayExit) // no runway exit to choose available
			{
				CString strWarn = _T("There is no available Runway Exit for Flight ");
				CString strError = _T("AIRCRAFT TERMINATE");
				AirsideSimErrorShown::SimWarning(pFlight,strWarn,strError);
				ClearanceItem newItem(NULL,OnTerminate,0);
				newItem.SetTime(lastClearanceItem.GetTime());
				newclearance.AddItem(newItem);
				if (pParkingStand)
				{
					int nIdx = pParkingStand->GetStandAssignmentRegister()->FindRecord(pFlight, ARR_PARKING);
					pParkingStand->GetStandAssignmentRegister()->DeleteRecord(nIdx);
				}

				pFlight->PerformClearance(newclearance);
				return;
			}

			//if (pParkingStand && !pParkingStand->TryLock(pFlight))
			//	ResolveStandConflict(pFlight,ARR_PARKING);

			//check to see if the flight need temp parking
			if(pFlight->GetTemporaryParking() != NULL ) //taxi to the temp parking stand
			{

				if (pFlight->GetTemporaryParking()->GetParkingType() == TempParkingInSim::TempStand)
				{
					pFlight->GetTemporaryParking()->GetLock(pFlight);
					pFlight->ApplyForFollowMeCarService(((TempParkingStandInSim*)pFlight->GetTemporaryParking())->GetStand());
				}

				//FlightGroundRouteDirectSegInSim * pDirSeg = pRunwayExit->GetRouteSeg();
				//ClearanceItem AtTaxiwayItem(pDirSeg,OnTaxiToTempParking, pRunwayExit->GetEnterTaxiwayPos());
				//AtTaxiwayItem.SetSpeed(lastClearanceItem.GetSpeed());
				//ElapsedTime dT = pFlight->GetTimeBetween(lastClearanceItem,AtTaxiwayItem);
				//ElapsedTime nextTime = lastClearanceItem.GetTime() + dT;
				//AtTaxiwayItem.SetTime(nextTime);
				//newclearance.AddItem(AtTaxiwayItem);

				//Set Flight Current Delay
				CString fltID;
				pFlight->getFullID(fltID.GetBuffer(255), 'A');
				CString strStand = pParkingStand->GetStandInput()->GetObjNameString();
				CString tempparkStr  = pFlight->GetTemporaryParking()->PrintTempParking();

				CString strdetail; 
				strdetail.Format(" dest stand %s is not available, parking at %s ", fltID, strStand, tempparkStr );

				TaxiInTempParkingDelay * pFlightDelay = new TaxiInTempParkingDelay(pFlight->GetTemporaryParking()->GetParkingType(), pFlight->GetUID(), lastClearanceItem.GetTime() );
				pFlightDelay->SetDetailString(strdetail);
				pFlight->SetDelayed(pFlightDelay);
				//return ;


			}
			else
				pFlight->ApplyForFollowMeCarService(pFlight->GetOperationParkingStand());

			if (pFlight->GetServiceFollowMeCar() != NULL)
			{
				if (pFlight->GetRouteToMeetingPoint() == NULL)
				{
					TaxiRouteInSim* pRouteToDest = NULL;
					if (pFlight->GetTemporaryParking())
						pRouteToDest = pFlight->GetRouteToTempParking();
					else
						pRouteToDest = pFlight->GetRouteToStand();

					if (pRouteToDest)
					{
						FlightGroundRouteDirectSegList segList;
						AirsideMeetingPointInSim* pMeetPoint = pFlight->GetServiceFollowMeCar()->GetMeetingPoint();
						pRouteToDest->GetSubRouteToMeetingPoint(pMeetPoint,segList);

						pFlight->m_pRouteToMeetingPoint = new TaxiRouteInSim(OnTaxiToMeetingPoint,pRunwayExit->GetLogicRunway(),pMeetPoint);
						pFlight->m_pRouteToMeetingPoint->AddTaxiwaySegList(segList,pFlight->GetEngine()->GetAirsideSimulation()->AllowCyclicGroundRoute());
					}

				}
				pFlight->GetRouteToMeetingPoint()->FindClearanceInConcern(pFlight,lastClearanceItem,RadiusOfConcernOnGround,newclearance);
				return;
			}
		}

		//Route To Temporary parking
		if (pFlight->GetTemporaryParking() != NULL && lastClearanceItem.GetMode() <= OnTaxiToTempParking)
		{
			
			TaxiRouteInSim * pRouteToTempParking = pFlight->GetRouteToTempParking();
			if(pRouteToTempParking)
			{
				double taxispd = pFlight->GetPerformance()->getTaxiInNormalSpeed(pFlight);
				pRouteToTempParking->SetSpeed(taxispd);
				btillEnd = pRouteToTempParking->FindClearanceInConcern(pFlight,lastClearanceItem,RadiusOfConcernOnGround,newclearance);
				if(btillEnd) return ;
			}
			else
			{
				CString strWarn = _T("No taxi route to temporary parking for the flight");
				CString strError = _T("AIRCRAFT TERMINATE");
				AirsideSimErrorShown::SimWarning(pFlight,strWarn,strError);

				ClearanceItem newItem(NULL,OnTerminate,0);
				newItem.SetTime(lastClearanceItem.GetTime());
				newclearance.AddItem(newItem);

				if (pParkingStand)
				{
					int nIdx = pParkingStand->GetStandAssignmentRegister()->FindRecord(pFlight, ARR_PARKING);
					pParkingStand->GetStandAssignmentRegister()->DeleteRecord(nIdx);
				}
				pFlight->PerformClearance(newclearance);
				return;
			}
		}
		//enter temporary parking
		if (pFlight->GetTemporaryParking() != NULL && (lastClearanceItem.GetMode() == OnHeldAtTempParking|| lastClearanceItem.GetMode()==OnTaxiToTempParking) )
		{
			if (pParkingStand)
			{
				pFlight->StartDelay(lastClearanceItem, pParkingStand->GetLockedFlight(), FlightConflict::TEMPPARKING,FlightConflict::ENTERINGSTAND,FltDelayReason_Stop,_T("Temp stand delay"));	//temp parking delay
				
				AirsideFlightStandOperationLog* pDelayLog = new AirsideFlightStandOperationLog(lastClearanceItem.GetTime().getPrecisely(), pFlight, pParkingStand->GetID(), pParkingStand->GetStandInput()->GetObjectName(),
					AirsideFlightStandOperationLog::StandDelay);
				pDelayLog->m_eDelayReason = AirsideFlightStandOperationLog::StandOccupied;
				pDelayLog->m_eParkingOpType = ARR_PARKING;
				pFlight->LogEventItem(pDelayLog);			
			}

			btillEnd = pFlight->GetTemporaryParking()->FindClearanceInConcern(pFlight,lastClearanceItem,RadiusOfConcernOnGround,newclearance);
			if(btillEnd) return ;

			if (pParkingStand)
			{
				AirsideFlightStandOperationLog* pDelayLog = new AirsideFlightStandOperationLog(lastClearanceItem.GetTime().getPrecisely(), pFlight, pParkingStand->GetID(), pParkingStand->GetStandInput()->GetObjectName(),
					AirsideFlightStandOperationLog::EndDelay);
				pDelayLog->m_eDelayReason = AirsideFlightStandOperationLog::StandOccupied;
				pDelayLog->m_eParkingOpType = ARR_PARKING;
				pFlight->LogEventItem(pDelayLog);			
			}

			pFlight->EndDelay(lastClearanceItem);			//end temp parking delay
		}	

		//route to stand
		TaxiRouteInSim * pRouteToStand = pFlight->GetRouteToStand();
		if( pRouteToStand && lastClearanceItem.GetMode() <= OnTaxiToStand && (!pFlight->m_bTowingToIntStand && !pFlight->m_bTowingToDepStand)) //taxi to the parking stand
		{
			//Apply for vehicle service
			pFlight->ApplyForVehicleService();	
			pFlight->ApplyForPaxBusService(true);

			double taxispd = pFlight->GetPerformance()->getTaxiInNormalSpeed(pFlight);
			pRouteToStand->SetSpeed(taxispd);
			btillEnd = pRouteToStand->FindClearanceInConcern(pFlight,lastClearanceItem,RadiusOfConcernOnGround,newclearance);
			if(btillEnd) return ;
		}

		if (pRouteToStand == NULL)
		{
			CString strWarn = _T("No inbound route for the flight");
			CString strError = _T("AIRCRAFT TERMINATE");
			AirsideSimErrorShown::SimWarning(pFlight,strWarn,strError);

			ClearanceItem newItem(NULL,OnTerminate,0);
			newItem.SetTime(lastClearanceItem.GetTime());
			newclearance.AddItem(newItem);
			pFlight->PerformClearance(newclearance);
			return;
		}

		//Stand
		pParkingStand = pFlight->GetOperationParkingStand();
		if(pParkingStand && lastClearanceItem.GetMode() < OnHeldAtStand 
			&& (!pFlight->NeedMoveToIntStand() && !pFlight->NeedTowingToDepStand()))		//to arrival flight
		{	

			if(pParkingStand != pFlight->GetPlanedParkingStand(ARR_PARKING) && pFlight->GetPlanedParkingStand(ARR_PARKING))  //reassigned stand
			{
				pFlight->StartDelay(lastClearanceItem,pFlight->GetPlanedParkingStand(ARR_PARKING)->GetLockedFlight(), FlightConflict::REASSIGN, FlightConflict::ENTERINGSTAND,FltDelayReason_Stop,_T("Reassigned stand delay"));
				pFlight->EndDelay(lastClearanceItem);			//end reassign delay, there is no delay in reassigning stand
			}

			pFlight->SetArrTime(lastClearanceItem.GetTime());		//update arrival time
			if(pFlight->IsDeparture())			// update departure time 	
			{
				ElapsedTime tMinTurnAround = pFlight->GetPerformance()->getStandMinTurnAroundTime(pFlight);
				ElapsedTime tPaxTime = 0L;
				if (pFlight->GetTowOffData())
					tPaxTime = pFlight->GetTowOffData()->getFltEnplaneTime() + pFlight->GetTowOffData()->getFltDeplaneTime();
				else
					tPaxTime = pFlight->GetFlightInput()->getEnplaneTime() + pFlight->GetFlightInput()->getDeplaneTime();

				if (tMinTurnAround < tPaxTime)
					tMinTurnAround = tPaxTime;

				pFlight->SetDepTime(max(pFlight->GetDepTime(),lastClearanceItem.GetTime() + tMinTurnAround));
			}


			ElapsedTime tParkingTime = pFlight->GetDepTime();
			FLIGHTTOWCRITERIATYPE eTowType = pFlight->GetTowOffStandType();

			if (eTowType == FREEUPSTAND)//need tow to intermediate stand
			{			
				tParkingTime = lastClearanceItem.GetTime() + pFlight->GetTowOffData()->getFltDeplaneTime();
				//pFlight->m_bTowingToIntStand = true;
				pFlight->ApplyTowingToIntStand();
			}

			if (eTowType == REPOSITIONFORDEP)	//tow to dep stand
			{
				if (!pFlight->GetTowOffData()->IsTow())
				{
					tParkingTime = pFlight->GetArrTime() + (pFlight->GetDepTime() - pFlight->GetArrTime())/2L;
				}
				else
				{
					if (pFlight->GetTowOffData()->GetTimeLeaveArrStandType() == CTowOffStandAssignmentData::AfterArrival)
						tParkingTime = lastClearanceItem.GetTime() + ElapsedTime( pFlight->GetTowOffData()->GetTimeLeaveArrStand()*60L);
					else
						tParkingTime = pFlight->GetDepTime() - ElapsedTime( pFlight->GetTowOffData()->GetTimeLeaveArrStand()*60L);
				}

			//	pFlight->m_bTowingToDepStand = true;
				pFlight->ApplyTowingToDepStand();
			}

			if (eTowType == NOTTOW && lastClearanceItem.GetMode() < OnEnterStand)
			{
				if (!pFlight->IsDeparture())
					tParkingTime = lastClearanceItem.GetTime() + pFlight->GetFlightInput()->getServiceTime();
				else
				{
					if (pFlight->GetTowOffData())
						tParkingTime = pFlight->GetDepTime() - pFlight->GetTowOffData()->getFltEnplaneTime();
					else
						tParkingTime = pFlight->GetDepTime() - pFlight->GetFlightInput()->getEnplaneTime();

					pFlight->SetParkingStand(pParkingStand,DEP_PARKING);
					pFlight->m_pIntermediateParking = NULL;
				}
			}

			//flight planing parking time in airport more than max parking time, but the actually parking time less than max parking time
			//need modify the stand register information
			if (eTowType == NOTTOW && !pFlight->GetFlightInput()->getIntermediateStand().IsBlank())
				RemoveFlightPlanedStandOccupancyTime(pFlight, INT_PARKING);

			btillEnd = pParkingStand->FindClearanceInConcern(pFlight,lastClearanceItem,tParkingTime,newclearance);		
			if(btillEnd) return ;

			//// record Leaving log of only arriving flight when death
			//if (!pFlight->IsDeparture())
			//{
			//	AirsideFlightStandOperationLog* pStandOffLog = new AirsideFlightStandOperationLog(lastClearanceItem.GetTime().getPrecisely(), pFlight, pParkingStand->GetID(), 
			//		pParkingStand->GetStandInput()->GetObjectName(), AirsideFlightStandOperationLog::LeavingStand);
			//	pFlight->LogEventItem(pStandOffLog);
			//}

		}
	
		if (!pFlight->IsDeparture() && pParkingStand && lastClearanceItem.GetMode() == OnHeldAtStand && !pFlight->NeedMoveToIntStand())		//only arrival flight, judge whether flight can be terminate
		{
			btillEnd = pParkingStand->FindClearanceInConcern(pFlight,lastClearanceItem,lastClearanceItem.GetTime(),newclearance);		
			if(btillEnd) return ;

			AirsideFlightStandOperationLog* pStandOffLog = new AirsideFlightStandOperationLog(lastClearanceItem.GetTime().getPrecisely(), pFlight, pParkingStand->GetID(), 
				pParkingStand->GetStandInput()->GetObjectName(), AirsideFlightStandOperationLog::LeavingStand);
			pFlight->LogEventItem(pStandOffLog);
		}
	
		if(pFlight->GetTowOffStandType() != NOTTOW && !pFlight->HasTowingToIntStand())
		{
			if(!pFlight->IsAllVehicleServiceExceptTowingCompleted())
				return;
 			btillEnd = ArrivalFlightIntermediateStandProcess(pFlight,newclearance,lastClearanceItem);
 			if(btillEnd)
 				return;
		}

		if( pFlight->IsDeparture())
		{
			if (!pFlight->IsAllVehicleServiceExceptTowingCompleted())
				return;

			OnboardFlightInSim* pOnboardFlightInSim = pFlight->GetOnboardFlight();
			if (pOnboardFlightInSim && !pOnboardFlightInSim->ReadyCloseDoor())
			{
				pOnboardFlightInSim->SetWaitAirsideFlight(pFlight);
				return;
			}

			pFlight->ChangeToDeparture();
			pFlight->ResetVehicleService();
		}	
	}
	
	//for flight departure
	if(pFlight->IsDepartingOperation())
	{
		if (!pFlight->IsArrival() && pFlight->GetMode() == OnBirth && pFlight->GetPlanedParkingStand(INT_PARKING)\
			&&!pFlight->NeedTowingToDepStand())		//only departure, birth				
		{
			CTowOffStandAssignmentData* pTowData = const_cast<CTowOffStandAssignmentData*>(pFlight->GetTowOffData());
			if (pTowData == NULL)
			{
				ALTObjectID depStand = pFlight->GetFlightInput()->getDepStand();
				pTowData = m_pTowOffCriteria->GetFlightFitData(pFlight->GetFlightInput(),depStand);
				pFlight->SetTowOffData(pTowData);
			}
			pFlight->ApplyTowingToDepStand();
		}

		FLIGHTTOWCRITERIATYPE eTowType = pFlight->GetTowOffStandType();
 		if(eTowType != NOTTOW && !pFlight->HasTowingToDepStand())
 		{
 			if(!pFlight->IsAllVehicleServiceExceptTowingCompleted())
 				return;
 
 		//	btillEnd = GetNextClearanceOfIntermediateParkingPart(pFlight,newclearance,lastClearanceItem);
			btillEnd = DepartureFlightIntermediateStandProcess(pFlight,newclearance,lastClearanceItem);
 			if(btillEnd)
 				return;
 
 		}

		if(pFlight->GetOperationParkingStand() == NULL )// if the departure flight cann't assign a parking stand, return and terminate it.
		{
			AssignOperationParkingStand(pFlight,DEP_PARKING);

			if (pFlight->GetOperationParkingStand() == NULL)
			{
				CString strWarn= _T("no Stand for Departure Flight,need temporary parking place");
				CString strError = _T("AIRCRAFT TERMINATE");
				AirsideSimErrorShown::SimWarning(pFlight,strWarn,strError);

				RemoveFlightPlanedStandOccupancyTime(pFlight, DEP_PARKING);

				ClearanceItem newItem(NULL,OnTerminate,0);
				newItem.SetTime(lastClearanceItem.GetTime());
				newclearance.AddItem(newItem);

				pFlight->PerformClearance(newclearance);
				return;
			}
		}		

		FlightRouteInSim * pSID  = pFlight->GetSID();		//must before flight push back assign its takeoff runway
		//exit stand
		if(lastClearanceItem.GetMode()< OnExitStand  )
		{	
			StandInSim * pParkingStand = pFlight->GetOperationParkingStand();
			//if(pParkingStand == NULL || !pParkingStand->GetLock(pFlight))// need check again, if the departure flight cann't assign a parking stand, return and terminate it.
			//{
			//	AssignOperationParkingStand(pFlight,DEP_PARKING);

			//	pParkingStand = pFlight->GetOperationParkingStand();

			//	if (pParkingStand == NULL)
			//	{
			//		CString strWarn= _T("no Stand for Departure Flight,need temporary parking place");
			//		CString strError = _T("AIRCRAFT TERMINATE");
			//		AirsideSimErrorShown::SimWarning(pFlight,strWarn,strError);

			//		RemoveFlightPlanedStandOccupancyTime(pFlight, DEP_PARKING);

			//		ClearanceItem newItem(NULL,OnTerminate,0);
			//		newItem.SetTime(lastClearanceItem.GetTime());
			//		newclearance.AddItem(newItem);
			//		return;
			//	}
			//}			
			pFlight->ApplyForVehicleService();
			pFlight->ApplyForPaxBusService(false);			

			if(lastClearanceItem.GetMode()== OnHeldAtStand && pFlight->getDeiceServiceRequest() == NULL && !pFlight->GetDeiceDecision().bAfterDeice)
			{
				FlightApplyForDeiceServiceEvent* pEvent = new FlightApplyForDeiceServiceEvent(pFlight);
				pEvent->setTime(pFlight->GetDepTime() - 1L);
				pEvent->addEvent();		
				pFlight->GetDeiceDecision().bAfterDeice = true;
			}

			btillEnd = pParkingStand->FindClearanceInConcern(pFlight,lastClearanceItem,pFlight->GetDepTime(),newclearance);	
			if(btillEnd)return;	
			if(newclearance.GetItemCount())
				return;
			pFlight->GetFlightInput()->SetAirsideFlightDepartured(true);

		}
			
		//if( lastClearanceItem.GetMode() == OnExitStand )
		{
			//if(lastClearanceItem.GetResource() && lastClearanceItem.GetResource()->GetType()==AirsideResource::ResType_StandLeadOutLine)
			if( pFlight->getDeiceServiceRequest() && pFlight->GetDeiceDecision().m_pDeicepadGroup && lastClearanceItem.GetMode()!=OnTaxiToDeice)
			{
				AssignRouteToDeice(pFlight);
				lastClearanceItem.SetMode(OnTaxiToDeice);
				newclearance.AddItem(lastClearanceItem);
				return;
			}
		}

		if(lastClearanceItem.GetMode() == OnTaxiToDeice)
		{
			bool btillEnd = FlightOnToDeice::FindClearanceInConcern(pFlight,
				m_pResources->GetAirportResource()->getGroundRouteResourceManager(), lastClearanceItem, newclearance);
			if(btillEnd)
			{
				return;	
			}
            lastClearanceItem.SetMode(OnTaxiToRunway);
			newclearance.AddItem(lastClearanceItem);
			return;
		}

		LogicRunwayInSim * pTakeoffRunway = pFlight->GetAndAssignTakeoffRunway();

		//OutBoundRoute			 
		TaxiRouteInSim * pOutBound  = pFlight->GetAndAssignOutBoundRoute();	
		TakeoffQueueInSim* pTakeoffQueue = m_pResources->GetAirportResource()->getTakeoffQueuesManager()->GetRunwayExitQueue(pFlight->GetAndAssignTakeoffPosition());
		if(pOutBound && (lastClearanceItem.GetMode() == OnExitStand || lastClearanceItem.GetMode() == OnTaxiToRunway ))
		{
			if(pFlight->GetTowingRoute())
			{
				pFlight->GetTowingRoute()->FlightExitRoute(pFlight,pFlight->GetTime());
				pFlight->SetTowingRoute(NULL);
			}

			ChangeFlightTakeoffPos(pFlight,lastClearanceItem.GetTime());
			pOutBound  = pFlight->GetOutBoundRoute();	
			double taxispd = pFlight->GetPerformance()->getTaxiOutNormalSpeed(pFlight);
			pOutBound->SetSpeed(taxispd);
			btillEnd = pOutBound->FindClearanceInConcern(pFlight,lastClearanceItem,RadiusOfConcernOnGround, newclearance);
			if(btillEnd){			
				
				return;	


			}
			if (pTakeoffQueue == NULL)
			{
				pFlight->StartDelay(lastClearanceItem, NULL, FlightConflict::STOPPED, FlightConflict::RUNWAY,FltDelayReason_Stop,_T("Enter runway delay"));			//enter runway delay
			}
		}

		if (pOutBound == NULL)
		{
			CString strWarn = _T("No outbound route for the flight");
			CString strError = _T("AIRCRAFT TERMINATE");
			AirsideSimErrorShown::SimWarning(pFlight,strWarn,strError);

			ClearanceItem newItem(NULL,OnTerminate,0);
			newItem.SetTime(lastClearanceItem.GetTime());
			newclearance.AddItem(newItem);

			pFlight->PerformClearance(newclearance);
			return;
		}

		//takeoff Queue
		if(pTakeoffQueue && (lastClearanceItem.GetMode() == OnTaxiToRunway || lastClearanceItem.GetMode() == OnQueueToRunway))
		{

			btillEnd = pTakeoffQueue->FindClearanceInConcern(pFlight,lastClearanceItem,newclearance);
			if(btillEnd) 
				return;

			pFlight->StartDelay(lastClearanceItem, NULL, FlightConflict::STOPPED, FlightConflict::RUNWAY,FltDelayReason_Stop,_T("Enter runway delay"));		//enter runway delay
		}

		//Queue to take off
		if(pTakeoffRunway && lastClearanceItem.GetMode()< OnTakeOffEnterRunway)
		{
			int nHeadCount = pTakeoffRunway->GetAheadFlightCountInQueueToTakeoffPos(pFlight);
			if (nHeadCount >0 && pTakeoffRunway->GetQueueList().GetQueueCount() >1)
			{
				ResourceDesc resDesc;
				pTakeoffRunway->getDesc(resDesc);
				AirsideFlightRunwayDelayLog* pLog = new AirsideFlightRunwayDelayLog(resDesc,lastClearanceItem.GetTime().asSeconds(), OnTakeoff, lastClearanceItem.GetTime().asSeconds(), 0l, FlightRunwayDelay::InQueue);
				CString strReason;
				strReason.Format("%d aircrafts ahead in all queue",nHeadCount);
				pLog->sReasonDetail = strReason.GetString(); 
				pFlight->LogEventItem(pLog);
			}

			if (m_pTakeoffSequencingInSim->IsWaitingInQueue(pFlight,lastClearanceItem.GetTime()))
			{
				pFlight->GetWakeUpCaller().OberverSubject(m_pRunwayOccupancyStatus);
				FlightConflictAtWaypointDelay* pDelay = new FlightConflictAtWaypointDelay;
				if (pFlight->GetAndAssignTakeoffRunway())
				{
					int nRunwayID = pFlight->GetAndAssignTakeoffRunway()->GetRunwayInSim()->GetRunwayInput()->getID();
					pDelay->SetFlightID(pFlight->GetUID());
					pDelay->SetDelayResult(FltDelayReason_RunwayHold);
					pDelay->SetStartTime(lastClearanceItem.GetTime());
					pFlight->SetDelayed(pDelay);
				}
				return;
			}
		}

		//SID
		if(pSID && (lastClearanceItem.GetMode() <= OnPreTakeoff || lastClearanceItem.GetMode() == OnWaitInHold)) 
		{
			pFlight->SetDelayed(NULL);
			btillEnd = m_pResources->GetAirspaceResource()->FindClearanceInConcern(pFlight, pSID, lastClearanceItem,newclearance);
			if(btillEnd) return; 
		}

		//Departure Flight Plan
		FlightRouteInSim* pDepPlan = pFlight->GetDepFlightPlan();
		if(pDepPlan && (lastClearanceItem.GetMode() <= OnCruiseDep || lastClearanceItem.GetMode() == OnWaitInHold))
		{
			btillEnd = m_pResources->GetAirspaceResource()->FindClearanceInConcern(pFlight,pDepPlan, lastClearanceItem,newclearance);
			if(btillEnd)return;	
		}
	}

	//end item 
	ClearanceItem newItem(NULL,OnTerminate,0);
	newItem.SetTime(lastClearanceItem.GetTime());
	newclearance.AddItem(newItem);	

	pFlight->PerformClearance(newclearance);
}

ElapsedTime AirTrafficController::GetSeparationTime( AirsideFlightInSim * pFlightLead, AirsideMobileElementMode leadmode, AirsideFlightInSim * pFlightTrail, AirsideMobileElementMode trailmode )
{
	if(leadmode <= OnLanding || leadmode >= OnPreTakeoff)
	{
		return ElapsedTime(120L);
	}
	
	return ElapsedTime(10L);
}

DistanceUnit AirTrafficController::GetSeparationDistance( AirsideFlightInSim * pFlightLead, AirsideMobileElementMode leadmode, AirsideFlightInSim * pFlightTrail, AirsideMobileElementMode trailmode )
{
	ASSERT(0);
	if( trailmode == OnTaxiToRunway )
	{	
		return pFlightTrail->GetPerformance()->getTaxiOutboundSeparationDistInQ(pFlightTrail);
	}
	else if(trailmode <= OnLanding || trailmode >= OnPreTakeoff)
	{
		return 300000;
	}	
	return 5000;
}

void AirTrafficController::AssignLandingRunway( AirsideFlightInSim * pFlight )
{
	PLACE_METHOD_TRACK_STRING();
	RunwayResourceManager * pRunwayRes =  NULL;
	if( m_pResources && m_pResources->GetAirportResource() )
		pRunwayRes = m_pResources->GetAirportResource()->getRunwayResource();

	if(NULL == pRunwayRes)
		return;

	if(pRunwayRes->GetRunwayCount())
	{
		LogicRunwayInSim * pRunway = m_RunwayAssignmentStrategies.GetArrivalRunway(pFlight,pRunwayRes);
		if (pRunway)
			pFlight->m_pLandingRunway = pRunway;
	}

}

void AirTrafficController::AssignTakeoffRunway( AirsideFlightInSim * pFlight )
{
	PLACE_METHOD_TRACK_STRING();
	RunwayResourceManager * pRunwayRes = m_pResources->GetAirportResource()->getRunwayResource();
	LogicRunwayInSim * pRunway =NULL;

	if(NULL == pRunwayRes)
		return;

	if(pRunwayRes->GetRunwayCount())
	{
		if (pFlight->IsCircuitFlight())
		{
			AirsideCircuitFlightInSim* pCircuitFlight = (AirsideCircuitFlightInSim*)pFlight;
			RunwayExitInSim* pTakeoff = pCircuitFlight->GetTakeoffPosition();
			if(pTakeoff)
			{
				pFlight->SetTakeoffPosition(pTakeoff);		
				pFlight->m_pDepatrueRunway = pTakeoff->GetLogicRunway();
			}
		}
		else
		{
			RunwayExitInSim* pTakeoff = m_RunwayAssignmentStrategies.GetTakeoffPosition(pFlight,pRunwayRes);
			if (pTakeoff)
			{
				pFlight->SetTakeoffPosition(pTakeoff);		
				pFlight->m_pDepatrueRunway = pTakeoff->GetLogicRunway();
			}
		}
	
	}

}

void AirTrafficController::AssignARRFlightPlan( AirsideFlightInSim * pFlight )
{
	PLACE_METHOD_TRACK_STRING();
	FlightRouteInSim* pRoute = NULL;
	bool bFlightPlan = m_pFlightPlan->GetLandingEnrouteFlightPlan(pFlight,pRoute);
	if (bFlightPlan)
		pFlight->m_pArrivalPlan = pRoute;
	
}

void AirTrafficController::AssignDEPFlightPlan( AirsideFlightInSim * pFlight )
{
	PLACE_METHOD_TRACK_STRING();
	FlightRouteInSim* pRoute = NULL;
	bool bFlightPlan = m_pFlightPlan->GetTakeoffEnrouteFlightPlan(pFlight,pRoute);
	if (bFlightPlan)
		pFlight->m_pDeparturePlan = pRoute;

}

void AirTrafficController::GetCircuitRoute(AirsideCircuitFlightInSim* pFlight)
{
	PLACE_METHOD_TRACK_STRING();
	RunwayExitInSim* pRwExit = pFlight->GetRunwayExit();
	RunwayExitInSim* pTakeoffPos = pFlight->GetTakeoffPosition();

	if (pRwExit == NULL || pTakeoffPos == NULL)
		return;

	TakeoffQueueInSim * pQueue = m_pResources->GetAirportResource()->getTakeoffQueuesManager()->GetRunwayExitQueue(pTakeoffPos);
	FlightGroundRouteDirectSegList vSegmets;
	vSegmets.clear();
	m_pBoundRouteAssignment->GetCircuitBoundRoute(pFlight,pQueue,pRwExit,pTakeoffPos, vSegmets);

	if (vSegmets.empty())
	{
		CString strWarn;
		strWarn.Format("There is no taxi route for the flight from the runway exit(%s) to takeoff position(%s)",  pRwExit->GetExitInput().GetName(),pTakeoffPos->GetExitInput().GetName() );
		CString strError = _T("FLIGHT ERROR");
		AirsideSimErrorShown::SimWarning(pFlight,strWarn,strError);
		return ;
	}

	pFlight->m_pCircuitBoundRoute = new TaxiOutboundRouteInSim(OnTaxiToRunway,pRwExit->GetLogicRunway(),pTakeoffPos->GetLogicRunway());
	bool bCyclicGroundRoute=pFlight->GetEngine()->GetAirsideSimulation()->AllowCyclicGroundRoute();
	pFlight->m_pCircuitBoundRoute->AddTaxiwaySegList(vSegmets,bCyclicGroundRoute);
}

void AirTrafficController::AssignCircuitInBoundRoute(AirsideCircuitFlightInSim * pFlight)
{
	PLACE_METHOD_TRACK_STRING();
	StandInSim* pStand = pFlight->GetPlanedParkingStand(DEP_PARKING);
	RunwayExitInSim* pRwExit = pFlight->GetRunwayExit();

	if(pStand == NULL || pRwExit == NULL) 
		return;

	FlightGroundRouteDirectSegList vSegmets;
	vSegmets.clear();
	m_pBoundRouteAssignment->GetInboundRoute(pFlight,pRwExit,pStand,vSegmets);

	if (vSegmets.empty())
	{
		CString strWarn;
		strWarn.Format("There is no taxi route for the flight from the exit(%s) to the stand(%s)", pRwExit->GetExitInput().GetName(), pStand->PrintResource() );
		CString strError = _T("FLIGHT ERROR");
		AirsideSimErrorShown::SimWarning(pFlight,strWarn,strError);
		return ;
	}
	pFlight->m_pRouteToStand = new TaxiInboundRouteInSim(OnTaxiToStand,pRwExit->GetLogicRunway(),pStand);
	bool bCyclicGroundRoute=pFlight->GetEngine()->GetAirsideSimulation()->AllowCyclicGroundRoute();
	pFlight->m_pRouteToStand->AddTaxiwaySegList(vSegmets,bCyclicGroundRoute);
}

void AirTrafficController::AssignInBoundRoute( AirsideFlightInSim * pFlight )
{
	PLACE_METHOD_TRACK_STRING();
	StandInSim* pStand = pFlight->m_pArrParkingStand;
	RunwayExitInSim* pRwExit = pFlight->GetRunwayExit();

	if(pStand == NULL || pRwExit == NULL) 
		return;
	
	FlightGroundRouteDirectSegList vSegmets;
	vSegmets.clear();
	m_pBoundRouteAssignment->GetInboundRoute(pFlight,pRwExit,pStand,vSegmets);

	if (vSegmets.empty())
	{
		CString strWarn;
		strWarn.Format("There is no taxi route for the flight from the exit(%s) to the stand(%s)", pRwExit->GetExitInput().GetName(), pStand->PrintResource() );
		CString strError = _T("FLIGHT ERROR");
		AirsideSimErrorShown::SimWarning(pFlight,strWarn,strError);
		return ;
	}
	pFlight->m_pRouteToStand = new TaxiInboundRouteInSim(OnTaxiToStand,pRwExit->GetLogicRunway(),pStand);
	bool bCyclicGroundRoute=pFlight->GetEngine()->GetAirsideSimulation()->AllowCyclicGroundRoute();
	pFlight->m_pRouteToStand->AddTaxiwaySegList(vSegmets,bCyclicGroundRoute);
	

}

void AirTrafficController::AssignCircuitOutBoundRoute(AirsideCircuitFlightInSim * pFlight)
{
	PLACE_METHOD_TRACK_STRING();
	AirsideResource* pCurRes = pFlight->GetResource();
	StandInSim* pStand = pFlight->m_pArrParkingStand;
	RunwayExitInSim* pTakeoffPos = pFlight->GetAndAssignTakeoffPosition();

	if(pCurRes->GetType() == AirsideResource::ResType_StandLeadInLine)
	{
		StandLeadInLineInSim* pleadInline = (StandLeadInLineInSim*)pCurRes;
		pStand = pleadInline->GetStandInSim();
	}
	if(pCurRes->GetType() == AirsideResource::ResType_StandLeadOutLine)
	{
		StandLeadOutLineInSim* pleadOutline = (StandLeadOutLineInSim*)pCurRes;
		pStand = pleadOutline->GetStandInSim();
	}
	if(pCurRes->GetType() == AirsideResource::ResType_DeiceStation)
	{
		DeicePadInSim* pPad = (DeicePadInSim*)pCurRes;
		TakeoffQueueInSim * pQueue = m_pResources->GetAirportResource()->getTakeoffQueuesManager()->GetRunwayExitQueue(pFlight->GetAndAssignTakeoffPosition());
		FlightGroundRouteDirectSegList vSegmets;		
		m_pBoundRouteAssignment->GetRouteFDeiceTTakoff(pFlight,pPad,pTakeoffPos, vSegmets);
		pFlight->m_pOutBoundRoute = new TaxiOutboundRouteInSim(OnTaxiToRunway,pPad,pTakeoffPos->GetLogicRunway());
		// 		pFlight->m_pOutBoundRoute->AddTaxiwaySegList(vSegmets);

		bool bCyclicGroundRoute=pFlight->GetEngine()->GetAirsideSimulation()->AllowCyclicGroundRoute();
		pFlight->m_pOutBoundRoute->AddTaxiwaySegList(vSegmets,bCyclicGroundRoute);

		return;
	}


	if (pStand == NULL || pTakeoffPos == NULL)
		return;

	TakeoffQueueInSim * pQueue = m_pResources->GetAirportResource()->getTakeoffQueuesManager()->GetRunwayExitQueue(pFlight->GetAndAssignTakeoffPosition());
	FlightGroundRouteDirectSegList vSegmets;
	vSegmets.clear();
	m_pBoundRouteAssignment->GetOutboundRoute(pFlight,pQueue,pStand,NULL,pTakeoffPos, vSegmets);

	if (vSegmets.empty())
	{
		CString strWarn;
		strWarn.Format("There is no taxi route for the flight from the stand(%s) to takeoff position(%s)", pStand->PrintResource(),pTakeoffPos->GetExitInput().GetName() );
		CString strError = _T("FLIGHT ERROR");
		AirsideSimErrorShown::SimWarning(pFlight,strWarn,strError);
		return ;
	}

	pFlight->m_pOutBoundRoute = new TaxiOutboundRouteInSim(OnTaxiToRunway,pStand,pTakeoffPos->GetLogicRunway());
	bool bCyclicGroundRoute=pFlight->GetEngine()->GetAirsideSimulation()->AllowCyclicGroundRoute();
	pFlight->m_pOutBoundRoute->AddTaxiwaySegList(vSegmets,bCyclicGroundRoute);
}

void AirTrafficController::AssignOutBoundRoute( AirsideFlightInSim * pFlight )
{	
	PLACE_METHOD_TRACK_STRING();
	AirsideResource* pCurRes = pFlight->GetResource();
	StandInSim* pStand = pFlight->m_pDepParkingStand;
	RunwayExitInSim* pTakeoffPos = pFlight->GetAndAssignTakeoffPosition();

	if(pCurRes->GetType() == AirsideResource::ResType_StandLeadInLine)
	{
		StandLeadInLineInSim* pleadInline = (StandLeadInLineInSim*)pCurRes;
		pStand = pleadInline->GetStandInSim();
	}
	if(pCurRes->GetType() == AirsideResource::ResType_StandLeadOutLine)
	{
		StandLeadOutLineInSim* pleadOutline = (StandLeadOutLineInSim*)pCurRes;
		pStand = pleadOutline->GetStandInSim();
	}
	if(pCurRes->GetType() == AirsideResource::ResType_DeiceStation)
	{
		DeicePadInSim* pPad = (DeicePadInSim*)pCurRes;
		TakeoffQueueInSim * pQueue = m_pResources->GetAirportResource()->getTakeoffQueuesManager()->GetRunwayExitQueue(pFlight->GetAndAssignTakeoffPosition());
		FlightGroundRouteDirectSegList vSegmets;		
		m_pBoundRouteAssignment->GetRouteFDeiceTTakoff(pFlight,pPad,pTakeoffPos, vSegmets);
		pFlight->m_pOutBoundRoute = new TaxiOutboundRouteInSim(OnTaxiToRunway,pPad,pTakeoffPos->GetLogicRunway());
// 		pFlight->m_pOutBoundRoute->AddTaxiwaySegList(vSegmets);

		bool bCyclicGroundRoute=pFlight->GetEngine()->GetAirsideSimulation()->AllowCyclicGroundRoute();
		pFlight->m_pOutBoundRoute->AddTaxiwaySegList(vSegmets,bCyclicGroundRoute);

		return;
	}
		
	
	if (pStand == NULL || pTakeoffPos == NULL)
		return;

	TakeoffQueueInSim * pQueue = m_pResources->GetAirportResource()->getTakeoffQueuesManager()->GetRunwayExitQueue(pFlight->GetAndAssignTakeoffPosition());
	FlightGroundRouteDirectSegList vSegmets;
	vSegmets.clear();
	m_pBoundRouteAssignment->GetOutboundRoute(pFlight,pQueue,pStand,NULL,pTakeoffPos, vSegmets);

	if (vSegmets.empty())
	{
		CString strWarn;
		strWarn.Format("There is no taxi route for the flight from the stand(%s) to takeoff position(%s)", pStand->PrintResource(),pTakeoffPos->GetExitInput().GetName() );
		CString strError = _T("FLIGHT ERROR");
		AirsideSimErrorShown::SimWarning(pFlight,strWarn,strError);
		return ;
	}

	pFlight->m_pOutBoundRoute = new TaxiOutboundRouteInSim(OnTaxiToRunway,pStand,pTakeoffPos->GetLogicRunway());
	bool bCyclicGroundRoute=pFlight->GetEngine()->GetAirsideSimulation()->AllowCyclicGroundRoute();
	pFlight->m_pOutBoundRoute->AddTaxiwaySegList(vSegmets,bCyclicGroundRoute);
// 	pFlight->m_pOutBoundRoute->AddTaxiwaySegList(vSegmets);
}


bool AirTrafficController::getOutBoundSeg( AirsideResource* pFrom, RunwayExitInSim* pTakeoffPos,AirsideFlightInSim* pFlight,FlightGroundRouteDirectSegList& vSegmets )
{
	AirsideResource* pCurRes = pFlight->GetResource();
	//RunwayExitInSim* pTakeoffPos = pFlight->GetAndAssignTakeoffPosition();

	if (pFrom == NULL || pTakeoffPos == NULL)
		return false;

	TakeoffQueueInSim * pQueue = m_pResources->GetAirportResource()->getTakeoffQueuesManager()->GetRunwayExitQueue(pTakeoffPos);
	vSegmets.clear();

	if (pCurRes->GetType() == AirsideResource::ResType_TaxiwayDirSeg )
	{
		TaxiwayDirectSegInSim* pCurSeg = (TaxiwayDirectSegInSim*)pCurRes;

		m_pBoundRouteAssignment->GetOutboundRoute(pFlight,pQueue,pFrom,pCurSeg,pTakeoffPos, vSegmets);
		vSegmets.insert(vSegmets.begin(),pCurSeg) ;
	}
	else
		m_pBoundRouteAssignment->GetOutboundRoute(pFlight,pQueue,pFrom,NULL,pTakeoffPos, vSegmets);

	if (vSegmets.empty())
	{
		return false;
	}
	return true;
}

bool AirTrafficController::getOutBoundSeg( AirsideResource* pFrom, FlightGroundRouteDirectSegInSim* pCurSeg,AirsideFlightInSim* pFlight,FlightGroundRouteDirectSegList& vSegmets )
{	
	RunwayExitInSim* pTakeoffPos = pFlight->GetAndAssignTakeoffPosition();

	if (pFrom == NULL || pTakeoffPos == NULL)
		return false;

	TakeoffQueueInSim * pQueue = m_pResources->GetAirportResource()->getTakeoffQueuesManager()->GetRunwayExitQueue(pFlight->GetAndAssignTakeoffPosition());
	vSegmets.clear();		

	m_pBoundRouteAssignment->GetOutboundRoute(pFlight,pQueue,pFrom,pCurSeg,pTakeoffPos, vSegmets);
	vSegmets.insert(vSegmets.begin(),pCurSeg) ;
	
	if (vSegmets.empty())
	{
		return false;
	}
	return true;
}

void AirTrafficController::AssignOutboundRoute( AirsideResource* pFrom, AirsideFlightInSim* pFlight )
{
	PLACE_METHOD_TRACK_STRING();
	AirsideResource* pCurRes = pFlight->GetResource();
	RunwayExitInSim* pTakeoffPos = pFlight->GetAndAssignTakeoffPosition();

	if (pFrom == NULL || pTakeoffPos == NULL)
		return;

	TakeoffQueueInSim * pQueue = m_pResources->GetAirportResource()->getTakeoffQueuesManager()->GetRunwayExitQueue(pFlight->GetAndAssignTakeoffPosition());
	FlightGroundRouteDirectSegList vSegmets;
	if(!getOutBoundSeg(pFrom,pTakeoffPos,pFlight,vSegmets))
	{
		CString strWarn;
		strWarn.Format("There is no taxi route for the flight from %s to takeoff position(%s)", pFrom->PrintResource(),pTakeoffPos->GetExitInput().GetName());
		CString strError = _T("FLIGHT ERROR");
		AirsideSimErrorShown::SimWarning(pFlight,strWarn,strError);
		return ;

	}

	pFlight->m_pOutBoundRoute = new TaxiOutboundRouteInSim(OnTaxiToRunway,pFrom,pTakeoffPos->GetLogicRunway());
// 	pFlight->m_pOutBoundRoute->AddTaxiwaySegList(vSegmets);
	bool bCyclicGroundRoute=pFlight->GetEngine()->GetAirsideSimulation()->AllowCyclicGroundRoute();
	pFlight->m_pOutBoundRoute->AddTaxiwaySegList(vSegmets,bCyclicGroundRoute);

}

void AirTrafficController::AssignOperationParkingStand( AirsideFlightInSim * pFlight ,FLIGHT_PARKINGOP_TYPE type)
{
	PLACE_METHOD_TRACK_STRING();

	if (type == INT_PARKING
		||(type == ARR_PARKING && !pFlight->IsArrival()) || (type == DEP_PARKING && !pFlight->IsDeparture()))
		return;

	pFlight->SetParkingStand(NULL,type);	//empty flight parking stand
	StandResourceManager * pStandRes = m_pResources->GetAirportResource()->getStandResource();

	StandInSim * pStand = pFlight->GetPlanedParkingStand(type);

	if(pStand == NULL)
	{
		if(pStandRes->GetStandCount()>0)
		{
			pStand = m_pDynamicStandReassignment->StandReassignment(pFlight,type);
			pFlight->SetParkingStand(pStand,type);
		}
	}
	else
	{
		if(m_pDynamicStandReassignment->StandAssignConflict(pFlight,pStand))
		{
			if (type == ARR_PARKING)
			{
				pStand = ResolveStandConflict(pFlight, ARR_PARKING);
			}
			else
			{
				pStand = m_pDynamicStandReassignment->StandReassignment(pFlight,type);
			}

			pFlight->SetParkingStand(pStand,type);		// Stand not available now
		}
		else
		{
			if (pStand->TryLock(pFlight) )
			{
				pStand->GetLock(pFlight);
				pFlight->SetParkingStand(pStand,type) ;			
			}
			else
			{
				if (type == ARR_PARKING)
				{
					pStand = ResolveStandConflict(pFlight, ARR_PARKING);
				}
				else
				{
					pStand = m_pDynamicStandReassignment->StandReassignment(pFlight,type);
				}

				pFlight->SetParkingStand(pStand,type);		// Stand not available now
			}
		}
	}	

// 	if (pFlight != NULL && pStand != NULL && pStand->GetStandInput() && pFlight->GetFlightInput())
// 	{
// 		ALTObjectID standID = pStand->GetStandInput()->GetObjectName();
// 		if ( standID.GetIDString() != pFlight->GetFlightInput()->getStand().GetIDString())
// 		{
// 			if (type == DEP_PARKING)
// 				pFlight->setDepGateInSim(m_pStand2gateConstraint->GetDepGateIDInSim(standID).GetIDString());
// 			else if (type == ARR_PARKING)
// 				pFlight->setArrGateInSim(m_pStand2gateConstraint->GetArrGateIDInSim(standID).GetIDString());
// 			else
// 			{
// 				pFlight->setDepGateInSim(m_pStand2gateConstraint->GetDepGateIDInSim(standID).GetIDString());
// 				pFlight->setArrGateInSim(m_pStand2gateConstraint->GetArrGateIDInSim(standID).GetIDString());
// 			}
// 
// 		}
// 	}

	if (type != DEP_PARKING && pStand != NULL)
	{
		CTowOffStandAssignmentData* pTowData = m_pTowOffCriteria->GetFlightFitData(pFlight->GetFlightInput(),pStand->GetStandInput()->GetObjectName());
		pFlight->SetTowOffData(pTowData);
	}
}

void AirTrafficController::AssignRunwayExit( AirsideFlightInSim * pFlight )
{
	PLACE_METHOD_TRACK_STRING();
	LogicRunwayInSim * pLandRunway = pFlight->GetLandingRunway();
	RunwayExitInSim * pExit = NULL;

	if(!pLandRunway) return;
	ASSERT(pLandRunway->GetExitCount() > 0);
	pExit =  m_RunwayExitAssignmentStrategies.GetAvailableRunwayExit(pFlight,m_pAirportDB,m_pResources->GetAirportResource()->getStandResource(),false);
    pFlight->SetRunwayExit( pExit );
	ASSERT( pExit && pExit->GetPosAtRunway() > 30000 );
}

void AirTrafficController::AssignTakeoffPosition( AirsideFlightInSim * pFlight )
{
	PLACE_METHOD_TRACK_STRING();
	AssignTakeoffRunway(pFlight);

	if(!pFlight->GetTakeoffPosition())
	{
		LogicRunwayInSim * pRunway = pFlight->GetAndAssignTakeoffRunway();

		if(pRunway && pRunway->GetExitCount())
		{
			RunwayExitInSim * pExit = m_TakeOffPositionAssignment.GetTakeoffPositionByFlightType(pRunway,pFlight);
			ASSERT(pExit);
			pFlight->SetTakeoffPosition(pExit);
		}
	}

}

bool AirTrafficController::Init( int nPrjId, AirsideResourceManager * pResources, AircraftClassificationManager* pAircraftClassification,CAirportDatabase * pAirportDB,
								CStand2GateConstraint* pStand2gateConstraint,OutputAirside *pOutput)
{
	PLACE_METHOD_TRACK_STRING();
	m_pFlightPlan->Init(nPrjId, pResources,pAirportDB);
	m_pResources = pResources;
	m_pAirportDB = pAirportDB;
	m_pAircraftClassifications = pAircraftClassification;
	m_RunwayExitAssignmentStrategies.Init(nPrjId,pAirportDB);
	m_RunwayAssignmentStrategies.Init(nPrjId,pAirportDB);
	m_SID_STARAssignment.Init(nPrjId,pAirportDB);

	//m_pInboundRouteInSimList->Init(nPrjId, nPrjId, m_pResources->GetAirportResource()->getTaxiwayResource());
	//m_pOutboundrouteInSimList->Init(nPrjId, nPrjId, m_pResources->GetAirportResource()->getTaxiwayResource());

	m_pBoundRouteAssignment->Init(nPrjId,pAirportDB, 
									m_pResources->GetAirportResource()->getTaxiwayResource(),
									m_pResources->GetAirportResource()->getRunwayResource(),
									m_pResources->GetAirportResource()->getGroundRouteResourceManager(),
									&m_pResources->GetAirportResource()->GetIntersectionNodeList());

	m_pInTrailSeparationInSim->Init(m_pAircraftClassifications,nPrjId);

	m_pOccupiedAssignedStandActionInSim->Init(nPrjId,pAirportDB);

	m_pFlightServiceRequirement->Init(nPrjId,pAirportDB);

	m_pPushBackClearance->Init(nPrjId,pAirportDB);

	m_TakeOffPositionAssignment.Init(nPrjId,pAirportDB,m_pResources->GetAirportResource()->getRunwayResource());

	m_pTemporaryParking->Init(nPrjId,pResources->GetAirportResource());
	pResources->GetAirportResource()->InitGroundRouteResourceManager(m_pTemporaryParking);// SetParkingSegments(m_pTemporaryParking);

	m_pStandBuffer->Init(nPrjId,pAirportDB);
	if (m_pTaxiwayConflictResolution ==NULL)
	{
		m_pTaxiwayConflictResolution = new CTaxiwayConflictResolutionInEngine(nPrjId,m_pAirportDB);
	}

	m_pArrivalDelayTrigger->Init(nPrjId,pResources->GetAirportResource(),pAirportDB);

	//m_missApproachController.Init(nPrjId,pResources->GetAirspaceResource());
	m_pRunwaysController->Initlization(nPrjId, this, &m_RunwayExitAssignmentStrategies, &m_TakeOffPositionAssignment,pOutput);
	m_pTakeoffSequencingInSim->Init(nPrjId, m_pResources->GetAirspaceResource(), m_pAirportDB);

	m_pStand2gateConstraint = pStand2gateConstraint;


	//
	m_pWeatherScript = new WeatherInSim();
	m_aircftSurSet = new CAircraftInitSurfaceSettingInSim(pAirportDB);
	m_pDeiceManager = new CDeiceAndAnticeManager(pAirportDB);
	m_pDeiceManager->ReadData(nPrjId);
	m_pDeiceManager->SortByFltType();
	m_pDeiceDemand = new CAircraftDeicingAndAnti_icingsSettings(pAirportDB);
	m_pDeiceDemand->ReadData();
	m_pDeiceDemand->SortByFlightTy();

	m_pPushAndTowOperationSpecInSim = new Push_TowOperationSpecInSim(nPrjId,m_pResources->GetAirportResource(),pAirportDB);
	

	m_pInterruptTimes = new CTaxiInterruptTimes(pAirportDB);
	m_pInterruptTimes->ReadData();

	m_towoffStandConstraint.SetAirportDatabase(pAirportDB);
	m_towoffStandConstraint.ReadData(nPrjId);
	TowoffStandStrategy* pActiveStrategy = m_towoffStandConstraint.GetActiveStrategy();
	if(pActiveStrategy != NULL)
	{
		m_pTowOffCriteria = pActiveStrategy->GetTowoffStandDataList();
	}
	if(m_pTowOffCriteria == NULL)
	{
		//no active strategy selected
		//it must be some places wrong
		ASSERT(0);
		m_pTowOffCriteria->ReadData(nPrjId);
		m_pTowOffCriteria->SetAirportDatabase(pAirportDB);
	}


	m_pDynamicStandReassignment->Init(m_pResources->GetAirportResource()->getStandResource(), m_pTowOffCriteria);

	return true;
}

void AirTrafficController::SetGateAssignmentConstraints(GateAssignmentConstraintList* pConstraints)
{
	ASSERT(pConstraints != NULL);
	ASSERT(m_pDynamicStandReassignment != NULL);

	pConstraints->GetACTypeStandConstraints()->ReadData();

	m_pDynamicStandReassignment->SetGateAssignmentConstraints(pConstraints);
}

void AirTrafficController::SetStandAssignmentPriority(FlightPriorityInfo* pInfo)
{
	ASSERT(pInfo != NULL);
	ASSERT(m_pDynamicStandReassignment != NULL);

	m_pDynamicStandReassignment->SetStandAssignmentPriority(pInfo);
}

void AirTrafficController::AssignSTAR( AirsideFlightInSim * pFlight )
{
	//if(pFlight->GetSTAR())
	//	return;

	FlightRouteInSim* pRoute = NULL;

	m_SID_STARAssignment.GetSTARRoute(pFlight,m_pResources,pRoute);
	ASSERT(pRoute != NULL);
	if(pRoute)
		pRoute->InitializeSegments(pFlight);

	pFlight->m_pSTAR = pRoute;
}

void AirTrafficController::AssignLandingCircuitRoute(AirsideFlightInSim * pFlight)
{
	FlightRouteInSim* pRoute = NULL;

//	m_SID_STARAssignment.GetLandingCircuitRoute(pFlight,m_pResources,pRoute);
	if (m_SID_STARAssignment.GetLandingCircuitRoute(pFlight,m_pResources,pRoute) == false)
	{
		if (pRoute)
		{
			delete pRoute;
			pRoute = NULL;
		}
		return;
	}
	ASSERT(pRoute != NULL);
	if(pRoute)
		pRoute->InitializeSegments(pFlight);

	pFlight->m_pLandingCircuitRoute = pRoute;
}

void AirTrafficController::AssignTakeoffCircuitRoute(AirsideFlightInSim* pFlight)
{
	FlightRouteInSim* pRoute = NULL;

//	m_SID_STARAssignment.GetTakeoffCircuitRoute(pFlight,m_pResources,pRoute);
	if (m_SID_STARAssignment.GetTakeoffCircuitRoute(pFlight,m_pResources,pRoute) == false)
	{
		if (pRoute)
		{
			delete pRoute;
			pRoute = NULL;
		}
		return;
	}
	ASSERT(pRoute != NULL);
	if(pRoute)
		pRoute->InitializeSegments(pFlight);

	pFlight->m_pTakeoffCircutRoute = pRoute;
}

void AirTrafficController::AssignSID( AirsideFlightInSim * pFlight )
{
	//if(pFlight->GetSID())
	//	return;

	FlightRouteInSim* pRoute = NULL;
	m_SID_STARAssignment.GetSIDRoute(pFlight,m_pResources,pRoute);

	ASSERT(pRoute != NULL);
	if(pRoute)
		pRoute->InitializeSegments(pFlight);

	pFlight->m_pSID = pRoute;

}

Push_TowOperationSpecInSim* AirTrafficController::GetPushAndTowOperationSpec()
{
	return m_pPushAndTowOperationSpecInSim;
}

void AirTrafficController::AssignRouteToTemporaryParking(AirsideFlightInSim* pFlight)
{
	PLACE_METHOD_TRACK_STRING();
	if (pFlight->GetTemporaryParking() == NULL || pFlight->GetRunwayExit() == NULL)
		return;	
	
		
	IntersectionNodeInSim * pNodeIn = NULL;
	IntersectionNodeInSim * pNodeOut = pFlight->GetTemporaryParking()->GetInNode();
	FlightGroundRouteResourceManager * pGroundRouteRes = m_pResources->GetAirportResource()->getGroundRouteResourceManager();
	
	if(pFlight->GetRunwayExit() && pFlight->GetRunwayExit()->GetRouteSeg())
		pNodeIn = pFlight->GetRunwayExit()->GetRouteSeg()->GetExitNode();

	if(pNodeOut == NULL)
	{
		CString strWarn;
		strWarn.Format("Define error in %s: no entry node", pFlight->GetTemporaryParking()->PrintTempParking()) ;
		CString strError = _T("DEFINE ERROR");
		AirsideSimErrorShown::SimWarning(pFlight,strWarn,strError);
		return ;
	}

	if(pNodeIn == NULL)
	{
		CString strWarn;
		strWarn.Format("Define error in runway exit(%s): no exit node",pFlight->GetRunwayExit()->GetExitInput().GetName());
		CString strError = _T("DEFINE ERROR");
		AirsideSimErrorShown::SimWarning(pFlight,strWarn,strError);
		return ;
	}

	FlightGroundRouteDirectSegList Taxisegs;
	double dPathWeight = (std::numeric_limits<double>::max)();
	pGroundRouteRes->GetRoute(pFlight->GetRunwayExit()->GetRouteSeg(),pNodeOut,pFlight,Taxisegs, dPathWeight);
	if (Taxisegs.empty())
	{
		CString strWarn;
		strWarn.Format("There is no taxi route for the flight from the exit(%s) to the temporary parking(%s)", pFlight->GetRunwayExit()->GetExitInput().GetName(), pFlight->GetTemporaryParking()->PrintTempParking() );
		CString strError = _T("FLIGHT ERROR");
		AirsideSimErrorShown::SimWarning(pFlight,strWarn,strError);
		return ;
	}

	Taxisegs.insert(Taxisegs.begin(), pFlight->GetRunwayExit()->GetRouteSeg() );
	pFlight->m_pRouteToTempParking = new TaxiInboundRouteInSim(OnTaxiToTempParking,pFlight->GetRunwayExit()->GetLogicRunway(),pNodeOut);
// 	pFlight->m_pRouteToTempParking->AddTaxiwaySegList(Taxisegs);
	bool bCyclicGroundRoute=pFlight->GetEngine()->GetAirsideSimulation()->AllowCyclicGroundRoute();
	pFlight->m_pRouteToTempParking->AddTaxiwaySegList(Taxisegs,bCyclicGroundRoute);

}

//assign route to stand 
void AirTrafficController::AssignRouteToStand(AirsideFlightInSim* pFlight)
{
	PLACE_METHOD_TRACK_STRING();
	if (pFlight->GetTemporaryParking() == NULL) //not going to temp parking, going to dest stand?
	{
		AssignInBoundRoute(pFlight);
		return;
	}
	else if(pFlight->GetOperationParkingStand())//from temp parking to parking stand
	{
		IntersectionNodeInSim * pNodeIn = pFlight->GetTemporaryParking()->GetOutNode(pFlight);
		IntersectionNodeInSim * pNodeOut = pFlight->GetOperationParkingStand()->GetMinDistInNode();
		
		if(pNodeIn == NULL)
		{
			CString strWarn;
			strWarn.Format("Define error in %s: no exit node", pFlight->GetTemporaryParking()->PrintTempParking()) ;
			CString strError = _T("DEFINE ERROR");
			AirsideSimErrorShown::SimWarning(pFlight,strWarn,strError);
			return ;
		}

		if(pNodeOut == NULL)
		{
			CString strWarn;
			strWarn.Format("Define error in stand(%s): no entry node",pFlight->GetOperationParkingStand()->PrintResource());
			CString strError = _T("DEFINE ERROR");
			AirsideSimErrorShown::SimWarning(pFlight,strWarn,strError);
			return ;
		}


		FlightGroundRouteResourceManager * pGroundRouteRes = m_pResources->GetAirportResource()->getGroundRouteResourceManager();
		FlightGroundRouteDirectSegList Taxisegs;
		double dPathWeight = (std::numeric_limits<double>::max)();
		pGroundRouteRes->GetRoute(pNodeIn,pNodeOut,pFlight,0,Taxisegs, dPathWeight);
		if (Taxisegs.empty())
		{
			CString strWarn;
			strWarn.Format("There is no taxi route for the flight from the temporary parking(%s) to the stand(%s)", pFlight->GetTemporaryParking()->PrintTempParking(),pFlight->GetOperationParkingStand()->PrintResource() );
			CString strError = _T("FLIGHT ERROR");
			AirsideSimErrorShown::SimWarning(pFlight,strWarn,strError);
			return ;
		}
		pFlight->m_pRouteToStand = new TaxiTostandRouteInSim(OnTaxiToStand,pNodeIn,pFlight->GetOperationParkingStand() );
// 		pFlight->m_pRouteToStand->AddTaxiwaySegList(Taxisegs);
		bool bCyclicGroundRoute=pFlight->GetEngine()->GetAirsideSimulation()->AllowCyclicGroundRoute();
		pFlight->m_pRouteToTempParking->AddTaxiwaySegList(Taxisegs,bCyclicGroundRoute);

	}

}

void AirTrafficController::AssignTowOffRoute(AirsideFlightInSim* pFlight,AirsideResource* pOrigin, AirsideResource* pDest)
{
	PLACE_METHOD_TRACK_STRING();
	if (pOrigin == pDest)
		return;

	ASSERT(pOrigin != NULL);		//The origin and destination should not be null
	ASSERT(pDest != NULL);

	if (pOrigin==NULL || pDest == NULL)
		return;

	IntersectionNodeInSim *pNodeFrom = NULL;
	IntersectionNodeInSim * pNodeTo = NULL;
	if (pOrigin->GetType() == AirsideResource::ResType_Stand)
		pNodeFrom = ((StandInSim*)pOrigin)->GetMaxDistOutNode();
	if (pDest->GetType() == AirsideResource::ResType_Stand)
		pNodeTo = ((StandInSim*)pDest)->GetMinDistInNode();

	if(pNodeFrom == NULL)
	{
		CString strWarn;
		strWarn.Format("Define error in stand(%s): no entry node", pOrigin->PrintResource()) ;
		CString strError = _T("DEFINE ERROR");
		AirsideSimErrorShown::SimWarning(pFlight,strWarn,strError);
		return ;
	}

	if(pNodeTo == NULL)
	{
		CString strWarn;
		strWarn.Format("Define error in stand(%s): no exit node",pDest->PrintResource());
		CString strError = _T("DEFINE ERROR");
		AirsideSimErrorShown::SimWarning(pFlight,strWarn,strError);
		return ;
	}

	FlightGroundRouteResourceManager * pGroundRouteRes = m_pResources->GetAirportResource()->getGroundRouteResourceManager();
	FlightGroundRouteDirectSegList Taxisegs;
	double dPathWeight = (std::numeric_limits<double>::max)();
	pGroundRouteRes->GetRoute(pNodeFrom,pNodeTo,pFlight,0,Taxisegs,dPathWeight);
	pFlight->m_pTowingRoute = new TaxiTostandRouteInSim(OnTaxiToStand,pOrigin,pDest);
// 	pFlight->m_pTowingRoute->AddTaxiwaySegList(Taxisegs);
	bool bCyclicGroundRoute=pFlight->GetEngine()->GetAirsideSimulation()->AllowCyclicGroundRoute();
	pFlight->m_pTowingRoute->AddTaxiwaySegList(Taxisegs,bCyclicGroundRoute);


}

void AirTrafficController::AssignIntermediateParking(AirsideFlightInSim* pFlight)
{
	PLACE_METHOD_TRACK_STRING();
	CTowOffStandAssignmentData* pTowData = const_cast<CTowOffStandAssignmentData*>(pFlight->GetTowOffData());
	if(pTowData && pTowData->GetRepositionOrFreeup() == TRUE)		//reposition for dep
		return;

	if (pTowData == NULL && !pFlight->IsArrival())
	{
		ALTObjectID depStand = pFlight->GetFlightInput()->getDepStand();
		if (depStand.IsBlank())
			return ;

		pTowData = m_pTowOffCriteria->GetFlightFitData(pFlight->GetFlightInput(),depStand);
		pFlight->SetTowOffData(pTowData);
	}

	ALTObjectID oldIntStandName = pFlight->GetFlightInput()->getIntermediateStand();
	StandInSim* pOldIntStand = m_pResources->GetAirportResource()->getStandResource()->GetStandByName(oldIntStandName);
	StandInSim* pIntermediateStand = NULL;
	if (pTowData)
	{
		int nCount = pTowData->GetPriorityCount();
		ALTObjectID IntStandName;

		for (int i =0; i < nCount; i++)
		{
			IntStandName = pTowData->GetPriorityItem(i)->GetStandName();
			if (oldIntStandName.idFits(IntStandName))
			{
				if (pOldIntStand && pOldIntStand->TryLock(pFlight))
				{
					pFlight->m_pIntermediateParking = pOldIntStand;
					if(pOldIntStand)
						pOldIntStand->GetLock(pFlight);	
					return;
				}
			}

			std::vector<StandInSim*> vStands;
			vStands.clear();
			m_pResources->GetAirportResource()->getStandResource()->GetStandListByStandFamilyName(IntStandName,vStands);

			int nStandCount = vStands.size();
			for (int idx =0; idx < nStandCount; idx++)
			{
				pIntermediateStand = vStands.at(idx);

				if (pIntermediateStand)
				{
					ElapsedTime tStart = pFlight->GetArrTime() + pTowData->getFltDeplaneTime();
					ElapsedTime tEnd = pFlight->GetDepTime() - pTowData->getFltEnplaneTime();

					if (IntStandName.GetIDString() != oldIntStandName.GetIDString()
						&& pIntermediateStand->GetStandAssignmentRegister()->IsStandIdle(tStart, tEnd)&& pIntermediateStand->TryLock(pFlight))
					{
						StandAssignRecord _record;
						_record.m_pFlight = pFlight;
						_record.m_tStart = tStart;
						_record.m_tEnd = tEnd;
						_record.m_OpType = INT_PARKING;
						pIntermediateStand->GetStandAssignmentRegister()->AddRecord(_record);

						pFlight->m_pIntermediateParking = pIntermediateStand;
						pIntermediateStand->GetLock(pFlight);

						if (pOldIntStand)
						{
							int nIdx = pOldIntStand->GetStandAssignmentRegister()->FindRecord(pFlight, INT_PARKING);
							if (nIdx >= 0)
								pOldIntStand->GetStandAssignmentRegister()->DeleteRecord(nIdx);	
						}
						return;
					}

					if (IntStandName.GetIDString() == oldIntStandName.GetIDString() && pIntermediateStand->TryLock(pFlight))
					{
						pFlight->m_pIntermediateParking = pIntermediateStand;
						pIntermediateStand->GetLock(pFlight);
						return;
					}
				}
			}
		}
	}

	if (pOldIntStand && !pOldIntStand->TryLock(pFlight))
	{
		int nIdx = pOldIntStand->GetStandAssignmentRegister()->FindRecord(pFlight, INT_PARKING);
		if (nIdx >= 0)
			pOldIntStand->GetStandAssignmentRegister()->DeleteRecord(nIdx);	

		return;
	}

	pFlight->m_pIntermediateParking = pOldIntStand;
	if(pOldIntStand)
		pOldIntStand->GetLock(pFlight);	
}

StandInSim* AirTrafficController::ResolveStandConflict(AirsideFlightInSim* pFlight,FLIGHT_PARKINGOP_TYPE type/*, ElapsedTime tStandBuffer*/)
{
	PLACE_METHOD_TRACK_STRING();
	StandInSim * pPlanStand = pFlight->GetPlanedParkingStand(type);

	COccupiedAssignedStandStrategy strategy = m_pOccupiedAssignedStandActionInSim->GetStrategyByFlightType(pFlight,m_pResources->GetAirportResource());
	StrategyTypeList strategyOrderList=strategy.GetOrder();
	for (int i = 0; i< StrategyNum; i++)
	{
		StrategyType strategyType=(StrategyType)strategyOrderList.at(i);
		switch(strategyType)
		{
		case ToIntersection:
			{
				IntersectionNodeInSim* pNode = m_pResources->GetAirportResource()->GetIntersectionNodeList().GetNodeByID(strategy.GetIntersectionID());
				if(pNode && !pFlight->GetTemporaryParking() )
				{
					pFlight->SetTempParking( new TempParkingNodeInSim(pNode));
					return pPlanStand;
				}
			}
			break;
		case ToStand:
			{
				ALTObjectGroup altObjectGroup;
				ObjIDList &standList=strategy.GetTmpStandList();
				for (int i=0;i<(int)standList.size();i++)
				{
					altObjectGroup.ReadData((int)standList.at(i));

					std::vector<StandInSim*> vStands;
					m_pResources->GetAirportResource()->getStandResource()->GetStandListByStandFamilyName(altObjectGroup.getName(),vStands);

					size_t nSize = vStands.size();
					for (size_t i = 0; i < nSize; i++)
					{
						StandInSim* pStand = vStands.at(i);
						if (pStand && !m_pDynamicStandReassignment->StandAssignConflict(pFlight,pStand))
						{
							pFlight->SetTempParking( new TempParkingStandInSim(pStand) );
							pStand->GetLock(pFlight);
							return pPlanStand;
						}	
					}
				}

			}	
			break;
		case ToTemporaryParking:
			{

				ObjIDList &tmpParkingList=strategy.GetTmpParkingList();
				for (int i=0;i<(int)tmpParkingList.size();i++)
				{
					TaxiwayInSim* pTaxiway = m_pResources->GetAirportResource()->getTaxiwayResource()->GetTaxiwayByID((int)tmpParkingList.at(i));
					TempParkingSegmentsInSim* pTempSeg = m_pTemporaryParking->GetTemporaryParkingTaxiway(pTaxiway,pFlight);
					if (pTempSeg)
					{
						pFlight->SetTempParking(pTempSeg);
						return pPlanStand;
					}
				}

			}	
			break;
		case ToDynamicalStand:
			{
				return m_pDynamicStandReassignment->StandReassignment(pFlight,type);
			}
			break;
		default:
			break;
		}
	}


/*
		for (int i = 0; i< StrategyNum; i++)
		{
			StandStrategy strategy = m_pOccupiedAssignedStandActionInSim->GetStrategyByFlightType(pFlight,i,m_pResources->GetAirportResource());
			switch(strategy.m_Strategy) 
			{			
				case ToIntersection:
					{
						IntersectionNodeInSim* pNode = m_pResources->GetAirportResource()->GetIntersectionNodeList().GetNodeByID(strategy.m_nTempResoure);
						if(pNode && !pFlight->GetTemporaryParking() )
						{
							pFlight->SetTempParking( new TempParkingNodeInSim(pNode));
							return pPlanStand;
						}
					}
					break;
				case ToStand:
					{
						ALTObjectGroup altObjectGroup;
						altObjectGroup.ReadData(strategy.m_nTempResoure);
	
						std::vector<StandInSim*> vStands;
						m_pResources->GetAirportResource()->getStandResource()->GetStandListByStandFamilyName(altObjectGroup.getName(),vStands);
	
						size_t nSize = vStands.size();
						for (size_t i = 0; i < nSize; i++)
						{
							StandInSim* pStand = vStands.at(i);
							if (pStand && !m_pDynamicStandReassignment->StandAssignConflict(pFlight,pStand))
							{
								pFlight->SetTempParking( new TempParkingStandInSim(pStand) );
								pStand->GetLock(pFlight);
								return pPlanStand;
							}	
						}
					}	
					break;
				case ToTemporaryParking:
					{
						TaxiwayInSim* pTaxiway = m_pResources->GetAirportResource()->getTaxiwayResource()->GetTaxiwayByID(strategy.m_nTempResoure);
						TempParkingSegmentsInSim* pTempSeg = m_pTemporaryParking->GetTemporaryParkingTaxiway(pTaxiway,pFlight);
						if (pTempSeg)
						{
							pFlight->SetTempParking(pTempSeg);
							return pPlanStand;
						}
					}	
					break;
				case ToDynamicalStand:
					{
						return m_pDynamicStandReassignment->StandReassignment(pFlight,type);
					}
					break;
			default:
				break;
			}
	
		}*/
	

	return m_pDynamicStandReassignment->StandReassignment(pFlight,type);
}

bool AirTrafficController::IsDelayPushback(AirsideFlightInSim* pFlight, ClearanceItem FltCurItem)
{	
	PLACE_METHOD_TRACK_STRING();
	StandInSim* pStand = pFlight->GetOperationParkingStand();

	if (pStand == NULL)
		return false;

	double DistanceClearance = m_pPushBackClearance->GetClearanceDistanceOnTaxiway();
	int nMaxOutBoundAssignedRunwayFlightCount = m_pPushBackClearance->GetMaxOutboundToAssignedRunwayFlightCount();
	int nMaxOutBoundFlightCount = m_pPushBackClearance->GetMaxOutboundFlightCount();
	int nMaxInBoundFlightCount = m_pPushBackClearance->GetMaxInboundFlightCount(pStand);
	ALTObjectID InboundStandFamilyID = m_pPushBackClearance->GetInboundStandFamily(pStand);
	int nMaxQueueLength =m_pPushBackClearance->GetMaxTakeoffQueueFlightCount(pFlight->GetAndAssignTakeoffRunway());

	LogicRunwayInSim* pLogicRunway = pFlight->GetAndAssignTakeoffRunway();
	TaxiwayResourceManager* pResManager = m_pResources->GetAirportResource()->getTaxiwayResource();

	if(!pLogicRunway) 
		return false;

	IntersectionNodeInSim* pOutNode = pStand->GetMaxDistOutNode();

	if (!pOutNode)
	{
		return false;
		//CString strWarn = "There is no push back route between Taxiway and Stand!";
		//CString strError = _T("STAND ERROR");
		//AirsideSimErrorShown::SimWarning(pFlight,strWarn,strError);
		//return true;
	}
	
	PushbackClearanceDelay* pDelay = new PushbackClearanceDelay;
	//pDelay->SetAtObjectID(pStand->GetStandInput()->getID());
	pDelay->SetFlightID(pFlight->GetUID());
	pDelay->SetDelayResult(FltDelayReason_PushbackClearance);
	pDelay->SetStartTime(pFlight->GetTime());


	int nNowQueueLength = pLogicRunway->GetDepartureQueueLength();
	if (nNowQueueLength > nMaxQueueLength)
	{
		pFlight->GetWakeUpCaller().OberverSubject(m_pTakeoffQueueStatus);
		CString strReason;
		strReason.Format("Exceed %d flights waiting for departing on runway %s", nMaxQueueLength, pLogicRunway->PrintResource());
		pDelay->SetReasonDescription(strReason);
		pFlight->SetDelayed(pDelay);
		pFlight->StartDelay(FltCurItem, NULL, FlightConflict::STOPPED,FlightConflict::LEAVINGSTAND,FltDelayReason_Stop,_T("leave stand delay"));		//leaving stand delay

		return true;
	}

	for (int i =0; i < int(m_vFlights.size()); i++)
	{
		AirsideFlightInSim* _pflight = m_vFlights[i];

		if (_pflight == pFlight)
			continue;

		if ( _pflight->GetMode() == OnExitStand)				//other push back in distance clearance
		{
			StandInSim* pOtherStand = _pflight->GetOperationParkingStand();
			if (pOtherStand &&pOtherStand->GetLockedFlight())
			{
				int nTaxiwayId = pResManager->GetLinkedTaxiwayID(pOutNode,pOtherStand->GetMaxDistOutNode());
				if (nTaxiwayId < 0)
					continue;

				double vDist = abs(pOutNode->GetNodeInput().GetDistance(nTaxiwayId) - pOtherStand->GetMaxDistOutNode()->GetNodeInput().GetDistance(nTaxiwayId)) ;
				if (vDist <= DistanceClearance)
				{
					pFlight->DependOnAgents(_pflight);
					CString strReason;
					strReason.Format("With other flight pushing back less than %d m", int(DistanceClearance/100) );
					pDelay->SetReasonDescription(strReason);
					pFlight->SetDelayed(pDelay);
					pFlight->StartDelay(FltCurItem, _pflight, FlightConflict::STOPPED,FlightConflict::LEAVINGSTAND,FltDelayReason_Stop,_T("leave stand delay"));	//leaving stand delay

					return true;
				}
			}
		}

		if (_pflight->GetMode() == OnTaxiToStand)		//Inbound
		{
			StandInSim* pOtherStand = _pflight->GetOperationParkingStand();
			if (pOtherStand && pOtherStand->GetStandInput()->GetObjectName().idFits(InboundStandFamilyID))
			{
				nMaxInBoundFlightCount--;
				if (nMaxInBoundFlightCount <= 0)
				{
					pFlight->GetWakeUpCaller().OberverSubject(m_pInboundStatus);

					CString strReason;
					strReason.Format("Exceed %d flights inbound to same stand family", m_pPushBackClearance->GetMaxInboundFlightCount(pStand));
					pDelay->SetReasonDescription(strReason);
					pFlight->SetDelayed(pDelay);
					pFlight->StartDelay(FltCurItem, NULL, FlightConflict::STOPPED,FlightConflict::LEAVINGSTAND,FltDelayReason_Stop,_T("leave stand delay"));	//leaving stand delay

					return true;
				}
			}
		}

		if (_pflight->GetMode() == OnTaxiToRunway || (_pflight->GetMode() == OnExitStand && !_pflight->m_bTowingToIntStand && !_pflight->m_bTowingToDepStand))			//Outbound
		{
			nMaxOutBoundFlightCount--;							//all runway
			if (nMaxOutBoundFlightCount <= 0)
			{
				pFlight->GetWakeUpCaller().OberverSubject(m_pOutboundStatus);

				CString strReason;
				strReason.Format("Exceed %d flights outbound in taxiway",  m_pPushBackClearance->GetMaxOutboundFlightCount());
				pDelay->SetReasonDescription(strReason);
				pFlight->SetDelayed(pDelay);
				pFlight->StartDelay(FltCurItem, NULL, FlightConflict::STOPPED,FlightConflict::LEAVINGSTAND,FltDelayReason_Stop,_T("leave stand delay"));	//leaving stand delay

				return true;
			}

			if (_pflight->m_pDepatrueRunway == pLogicRunway)		// assigned runway
			{
				nMaxOutBoundAssignedRunwayFlightCount--;
				if (nMaxOutBoundAssignedRunwayFlightCount <= 0)
				{
					pFlight->GetWakeUpCaller().OberverSubject(m_pOutboundStatus);

					CString strReason;
					strReason.Format("Exceed %d flights outbound to runway %s", m_pPushBackClearance->GetMaxOutboundToAssignedRunwayFlightCount(), pLogicRunway->PrintResource());
					pDelay->SetReasonDescription(strReason);
					pFlight->SetDelayed(pDelay);
					pFlight->StartDelay(FltCurItem, NULL, FlightConflict::STOPPED,FlightConflict::LEAVINGSTAND,FltDelayReason_Stop,_T("leave stand delay"));	//leaving stand delay

					return true;
				}
			}

		}

	}
	delete pDelay;
	pDelay = NULL;

	return false;
}

int AirTrafficController::GetTakeoffQueueLength(LogicRunwayInSim* pRunway)
{
	return pRunway->GetDepartureQueueLength();
}

void AirTrafficController::RemoveFlightPlanedStandOccupancyTime(AirsideFlightInSim* pFlight,FLIGHT_PARKINGOP_TYPE eType )
{
	StandInSim* pStand = pFlight->GetPlanedParkingStand(eType);
	if (pStand)
	{
		int nIdx = pStand->GetStandAssignmentRegister()->FindRecord(pFlight, INT_PARKING);
		if (nIdx >= 0)
			pStand->GetStandAssignmentRegister()->DeleteRecord(nIdx);
	}
}
bool AirTrafficController::ArrivalFlightIntermediateStandProcess(AirsideFlightInSim * pFlight, Clearance& newclearance,ClearanceItem& lastClearanceItem)
{
	StandInSim* pIntermediateParking = NULL;
	if (pFlight->GetTowOffStandType() == FREEUPSTAND)
	{
		pIntermediateParking = pFlight->GetIntermediateParking();

		if ( pIntermediateParking == NULL )	//if without intermediate stand, terminate flight
		{

			CString strWarn = _T("no Stand for intermediate parking ,need intermediate parking stand");
			CString strError = _T("AIRCRAFT TERMINATE");
			AirsideSimErrorShown::SimWarning(pFlight,strWarn,strError);

			RemoveFlightPlanedStandOccupancyTime(pFlight,INT_PARKING);
			RemoveFlightPlanedStandOccupancyTime(pFlight,DEP_PARKING);

			ClearanceItem newItem(NULL,OnTerminate,0);
			newItem.SetTime(lastClearanceItem.GetTime());
			newclearance.AddItem(newItem);

			pFlight->PerformClearance(newclearance);
			return true;

		}
	}
	else
		pIntermediateParking = pFlight->m_pArrParkingStand;

	VehicleRequestDispatcher * pVehicleAssinger = pFlight->GetServiceRequestDispatcher();
	//flight to intermediate stand
	if (pFlight->NeedMoveToIntStand())		
	{

		StandInSim* pArrStand = pFlight->m_pArrParkingStand;

		if (pArrStand && pIntermediateParking != pArrStand)			//move to intermediate stand
		{
			if (lastClearanceItem.GetMode() == OnHeldAtStand && pArrStand->IsStandResource(pFlight->GetResource()))	//exit arrival stand
			{
				bool bEnd = pArrStand->FindClearanceInConcern(pFlight,lastClearanceItem,lastClearanceItem.GetTime(),newclearance);
				if(bEnd) 
					return true;
			}

			if (pFlight->m_pTowingServiceVehicle == NULL && !pVehicleAssinger->UnDispatchTowTruckForFlight(pFlight))		//if no tow truck towing, moving by itself
			{
				TaxiRouteInSim* pRouteToIntermediate = pFlight->GetTowingRoute();
				if (pRouteToIntermediate == NULL)
				{
					AssignTowOffRoute(pFlight,pArrStand,pIntermediateParking);
					pRouteToIntermediate = pFlight->GetTowingRoute();
				}

				//leave arr stand to intermediate stand
				if (pRouteToIntermediate && (lastClearanceItem.GetMode() == OnExitStand||lastClearanceItem.GetMode() == OnTaxiToStand ))	
				{
					bool bEnd = pRouteToIntermediate->FindClearanceInConcern(pFlight,lastClearanceItem,RadiusOfConcernOnGround,newclearance);
					if(bEnd) 
						return true;

					if (newclearance.GetItemCount() >0)
						lastClearanceItem = newclearance.GetItem(newclearance.GetItemCount() -1);
					if(pFlight->m_pTowingRoute)
					{
						if(pFlight->m_pTowingRoute && !pFlight->m_pTowingRoute->mpResDest->IsStandResource())
						{
							pFlight->m_pTowingRoute->FlightExitRoute(pFlight,lastClearanceItem.GetTime());
							pFlight->m_pTowingRoute = NULL;
						}						
						
					}

					if (pRouteToIntermediate->GetItemCount()<1 && lastClearanceItem.GetMode() == OnExitStand)
					{
						lastClearanceItem.SetMode(OnTaxiToStand);
					}
				}
			}
		}

		//intermediate stand clearance generate 
		if(lastClearanceItem.GetMode() <= OnHeldAtStand)
		{	

			ElapsedTime tExit = pFlight->getDepTime() - pFlight->GetTowOffData()->getFltEnplaneTime() - ElapsedTime(600L);

			if (tExit <= lastClearanceItem.GetTime())
				tExit = lastClearanceItem.GetTime()+10L;

			bool bEnd = pIntermediateParking->FindClearanceInConcern(pFlight,lastClearanceItem,tExit,newclearance);
		}

		pFlight->TowingToIntStand();
		pFlight->ApplyTowingToDepStand();

 		if(!pFlight->IsDeparture())	//only arrival
 		{
 			ElapsedTime tParkingTime = lastClearanceItem.GetTime() + pFlight->GetFlightInput()->getServiceTime();
 			pIntermediateParking->FindClearanceInConcern(pFlight,lastClearanceItem,tParkingTime,newclearance);
 		}
		return true;
	}

	if (pIntermediateParking->IsStandResource(pFlight->GetResource()) && pFlight->m_pTowingServiceVehicle != NULL)		//with tow truck service
	{
		ElapsedTime tExit =	lastClearanceItem.GetTime();

		if (pFlight->GetTowOffStandType() == FREEUPSTAND)
			tExit =	pFlight->getDepTime() - pFlight->GetTowOffData()->getFltEnplaneTime() - ElapsedTime(600L);

		if (tExit < lastClearanceItem.GetTime())
			tExit = lastClearanceItem.GetTime();

		bool bEnd = pIntermediateParking->FindClearanceInConcern(pFlight,lastClearanceItem,tExit,newclearance);
		return true;
	}

	return false;
}

bool AirTrafficController::DepartureFlightIntermediateStandProcess(AirsideFlightInSim * pFlight, Clearance& newclearance,ClearanceItem& lastClearanceItem)
{
	StandInSim* pIntermediateParking = NULL;
	if (pFlight->GetTowOffStandType() == FREEUPSTAND)
	{
		pIntermediateParking = pFlight->GetIntermediateParking();

		if ( pIntermediateParking == NULL )	//if without intermediate stand, terminate flight
		{

			CString strWarn = _T("no Stand for intermediate parking ,need intermediate parking stand");
			CString strError = _T("AIRCRAFT TERMINATE");
			AirsideSimErrorShown::SimWarning(pFlight,strWarn,strError);

			RemoveFlightPlanedStandOccupancyTime(pFlight,INT_PARKING);
			RemoveFlightPlanedStandOccupancyTime(pFlight,DEP_PARKING);

			ClearanceItem newItem(NULL,OnTerminate,0);
			newItem.SetTime(lastClearanceItem.GetTime());
			newclearance.AddItem(newItem);

			pFlight->PerformClearance(newclearance);
			return true;

		}
	}
	else
		pIntermediateParking = pFlight->m_pArrParkingStand;

	if (!pFlight->IsArrival() && lastClearanceItem.GetMode() == OnBirth)
	{
		pIntermediateParking->FindClearanceInConcern(pFlight,lastClearanceItem,lastClearanceItem.GetTime(),newclearance);
		StandInSim* pDepStand = pFlight->GetPlanedParkingStand(DEP_PARKING);
		if (pDepStand == NULL || (pDepStand && !pDepStand->GetLock(pFlight)))
		{
			AssignOperationParkingStand(pFlight,DEP_PARKING);
		}
		else
			pFlight->SetParkingStand(pDepStand,DEP_PARKING);

	}

	VehicleRequestDispatcher * pVehicleAssinger = pFlight->GetServiceRequestDispatcher();
	//for departure flight, flight from intermediate stand to dep stand
	if (pFlight->NeedTowingToDepStand() && pFlight->IsDeparture() 
		&& (pFlight->m_pTowingServiceVehicle == NULL && !pVehicleAssinger->UnDispatchTowTruckForFlight(pFlight)))	//if no tow truck towing, moving by itself
	{
		if ((pFlight->m_pDepParkingStand && !pFlight->m_pDepParkingStand->GetLock(pFlight))
			|| pFlight->m_pDepParkingStand == NULL)
		{
			StandInSim* pDepStand = pFlight->GetPlanedParkingStand(DEP_PARKING);
			if (pDepStand == NULL || (pDepStand && !pDepStand->GetLock(pFlight)))
			{
				AssignOperationParkingStand(pFlight,DEP_PARKING);
			}
			else
				pFlight->SetParkingStand(pDepStand,DEP_PARKING);
		}

		StandInSim* pDepStand = pFlight->m_pDepParkingStand;

		if (pDepStand == NULL)
		{
			CString strWarn= _T("no Stand for Departure Flight,need temporary parking place");
			CString strError = _T("AIRCRAFT TERMINATE");
			AirsideSimErrorShown::SimWarning(pFlight,strWarn,strError);

			RemoveFlightPlanedStandOccupancyTime(pFlight, DEP_PARKING);

			ClearanceItem newItem(NULL,OnTerminate,0);
			newItem.SetTime(lastClearanceItem.GetTime());
			newclearance.AddItem(newItem);

			pFlight->PerformClearance(newclearance);
			return true;
		}

		if (pIntermediateParking != pDepStand && lastClearanceItem.GetMode() == OnHeldAtStand)
		{
			ElapsedTime tExit = pFlight->getDepTime() - pFlight->GetTowOffData()->getFltEnplaneTime() - ElapsedTime(600L);

			if (tExit <= lastClearanceItem.GetTime())
				tExit = lastClearanceItem.GetTime();
			bool bEnd = pIntermediateParking->FindClearanceInConcern(pFlight,lastClearanceItem,tExit,newclearance);
			if(bEnd) 
				return true;
		}

		TaxiRouteInSim* pRouteToStand = pFlight->GetTowingRoute();
		if (pRouteToStand == NULL)
		{
			AssignTowOffRoute(pFlight,pIntermediateParking,pDepStand);
			pRouteToStand = pFlight->GetTowingRoute();
		}

		if (pRouteToStand && (lastClearanceItem.GetMode()== OnExitStand||lastClearanceItem.GetMode() <= OnTaxiToStand ))
		{
			bool bEnd = pRouteToStand->FindClearanceInConcern(pFlight,lastClearanceItem,RadiusOfConcernOnGround,newclearance);
			if(bEnd) 
				return true;

			if (pRouteToStand->GetItemCount()<1 && lastClearanceItem.GetMode() == OnExitStand)
			{
				lastClearanceItem.SetMode(OnTaxiToStand);
			}
		}

		pFlight->TowingToDepStand();
	}

	if(pFlight->NeedTowingToDepStand() && pFlight->GetTowOffStandType() == REPOSITIONFORDEP)
	{
		StandInSim* pDepStand = pFlight->m_pDepParkingStand;
		if (pDepStand == NULL || (pDepStand && !pDepStand->GetLock(pFlight)))
		{
			AssignOperationParkingStand(pFlight,DEP_PARKING);
		}
		pFlight->TowingToDepStand();
	}
	return false;
}
//1. flight at arrival stand and call this method to make flight move from arrival stand to intermediate stand
//2. flight at intermediate stand and call this method to make flight move from intermediate stand to departure stand
//Note: if flight at arrival stand call this method, the flight should call it again
bool AirTrafficController::GetNextClearanceOfIntermediateParkingPart(AirsideFlightInSim * pFlight, Clearance& newclearance,ClearanceItem& lastClearanceItem)
{
	PLACE_METHOD_TRACK_STRING();
	StandInSim* pIntermediateParking = NULL;
	if (pFlight->GetTowOffStandType() == FREEUPSTAND)
	{
		pIntermediateParking = pFlight->GetIntermediateParking();

		if ( pIntermediateParking == NULL )	//if without intermediate stand, terminate flight
		{

			CString strWarn = _T("no Stand for intermediate parking ,need intermediate parking stand");
			CString strError = _T("AIRCRAFT TERMINATE");
			AirsideSimErrorShown::SimWarning(pFlight,strWarn,strError);

			RemoveFlightPlanedStandOccupancyTime(pFlight,INT_PARKING);
			RemoveFlightPlanedStandOccupancyTime(pFlight,DEP_PARKING);

			ClearanceItem newItem(NULL,OnTerminate,0);
			newItem.SetTime(lastClearanceItem.GetTime());
			newclearance.AddItem(newItem);

			pFlight->PerformClearance(newclearance);
			return true;

		}
	}
	else
		pIntermediateParking = pFlight->m_pArrParkingStand;

	VehicleRequestDispatcher * pVehicleAssinger = pFlight->GetServiceRequestDispatcher();
	//flight to intermediate stand
	if (pFlight->m_bTowingToIntStand)		
	{

		StandInSim* pArrStand = pFlight->m_pArrParkingStand;

		if (pArrStand && pIntermediateParking != pArrStand)			//move to intermediate stand
		{
			if (lastClearanceItem.GetMode() == OnHeldAtStand && pArrStand->IsStandResource(pFlight->GetResource()))	//exit arrival stand
			{
				bool bEnd = pArrStand->FindClearanceInConcern(pFlight,lastClearanceItem,lastClearanceItem.GetTime(),newclearance);
				if(bEnd) 
					return true;
			}

			if (pFlight->m_pTowingServiceVehicle == NULL && !pVehicleAssinger->UnDispatchTowTruckForFlight(pFlight))		//if no tow truck towing, moving by itself
			{
				TaxiRouteInSim* pRouteToIntermediate = pFlight->GetTowingRoute();
				if (pRouteToIntermediate == NULL)
				{
					AssignTowOffRoute(pFlight,pArrStand,pIntermediateParking);
					pRouteToIntermediate = pFlight->GetTowingRoute();
				}

				//leave arr stand to intermediate stand
				if (pRouteToIntermediate && (lastClearanceItem.GetMode() == OnExitStand||lastClearanceItem.GetMode() == OnTaxiToStand ))	
				{
					bool bEnd = pRouteToIntermediate->FindClearanceInConcern(pFlight,lastClearanceItem,RadiusOfConcernOnGround,newclearance);
					if(bEnd) 
						return true;

					if (newclearance.GetItemCount() >0)
						lastClearanceItem = newclearance.GetItem(newclearance.GetItemCount() -1);
					if(pFlight->m_pTowingRoute)
					{
						pFlight->m_pTowingRoute->FlightExitRoute(pFlight,lastClearanceItem.GetTime());
						pFlight->m_pTowingRoute = NULL;
					}

					if (pRouteToIntermediate->GetItemCount()<1 && lastClearanceItem.GetMode() == OnExitStand)
					{
						lastClearanceItem.SetMode(OnTaxiToStand);
					}
				}
			}

//  			if(!pFlight->IsDepartingOperation() && pFlight->IsDeparture())
//  				pFlight->ChangeToDeparture();
		}

		if (!pFlight->IsArrival() && lastClearanceItem.GetMode() == OnBirth)
		{
			pIntermediateParking->FindClearanceInConcern(pFlight,lastClearanceItem,lastClearanceItem.GetTime(),newclearance);
			StandInSim* pDepStand = pFlight->GetPlanedParkingStand(DEP_PARKING);
			if (pDepStand == NULL || (pDepStand && !pDepStand->GetLock(pFlight)))
			{
				AssignOperationParkingStand(pFlight,DEP_PARKING);
			}
			else
				pFlight->SetParkingStand(pDepStand,DEP_PARKING);

		}

		//intermediate stand clearance generate 
		if(lastClearanceItem.GetMode() <= OnHeldAtStand)
		{	

			ElapsedTime tExit = pFlight->getDepTime() - pFlight->GetTowOffData()->getFltEnplaneTime() - ElapsedTime(600L);

			if (tExit <= lastClearanceItem.GetTime())
				tExit = lastClearanceItem.GetTime()+10L;

			bool bEnd = pIntermediateParking->FindClearanceInConcern(pFlight,lastClearanceItem,tExit,newclearance);
		}

		pFlight->m_bTowingToDepStand = true;
		pFlight->m_bTowingToIntStand = false;

		if(!pFlight->IsDeparture())	//only arrival
		{
			ClearanceItem newItem(NULL,OnTerminate,0);
			newItem.SetTime(lastClearanceItem.GetTime());
			newclearance.AddItem(newItem);

			pFlight->PerformClearance(newclearance);
		}
		return true;
	}

	if (pIntermediateParking->IsStandResource(pFlight->GetResource()) && pFlight->m_pTowingServiceVehicle != NULL)		//with tow truck service
	{
		ElapsedTime tExit =	lastClearanceItem.GetTime();

		if (pFlight->GetTowOffStandType() == FREEUPSTAND)
			tExit =	pFlight->getDepTime() - pFlight->GetTowOffData()->getFltEnplaneTime() - ElapsedTime(600L);

		if (tExit < lastClearanceItem.GetTime())
			tExit = lastClearanceItem.GetTime();

		bool bEnd = pIntermediateParking->FindClearanceInConcern(pFlight,lastClearanceItem,tExit,newclearance);
		return true;
	}

	//for departure flight, flight from intermediate stand to dep stand
	if (pFlight->m_bTowingToDepStand && pFlight->IsDeparture() 
		&& (pFlight->m_pTowingServiceVehicle == NULL && !pVehicleAssinger->UnDispatchTowTruckForFlight(pFlight)))	//if no tow truck towing, moving by itself
	{
		//if (pIntermediateParking->IsStandResource(pFlight->GetResource()))		//flight on intermediate stand and ready to leave, assign dep stand
		//{
			if ((pFlight->m_pDepParkingStand && !pFlight->m_pDepParkingStand->GetLock(pFlight))
				|| pFlight->m_pDepParkingStand == NULL)
			{
				StandInSim* pDepStand = pFlight->GetPlanedParkingStand(DEP_PARKING);
				if (pDepStand == NULL || (pDepStand && !pDepStand->GetLock(pFlight)))
				{
					AssignOperationParkingStand(pFlight,DEP_PARKING);
				}
				else
					pFlight->SetParkingStand(pDepStand,DEP_PARKING);
			}
		//}

		StandInSim* pDepStand = pFlight->m_pDepParkingStand;

		if (pDepStand == NULL)
		{
			CString strWarn= _T("no Stand for Departure Flight,need temporary parking place");
			CString strError = _T("AIRCRAFT TERMINATE");
			AirsideSimErrorShown::SimWarning(pFlight,strWarn,strError);

			RemoveFlightPlanedStandOccupancyTime(pFlight, DEP_PARKING);

			ClearanceItem newItem(NULL,OnTerminate,0);
			newItem.SetTime(lastClearanceItem.GetTime());
			newclearance.AddItem(newItem);

			pFlight->PerformClearance(newclearance);
			return true;
		}
	
		if (pIntermediateParking != pDepStand && lastClearanceItem.GetMode() == OnHeldAtStand)
			pIntermediateParking->GetExitStandClearance(pFlight,lastClearanceItem,newclearance);

		TaxiRouteInSim* pRouteToStand = pFlight->GetTowingRoute();
		if (pRouteToStand == NULL)
		{
			AssignTowOffRoute(pFlight,pIntermediateParking,pDepStand);
			pRouteToStand = pFlight->GetTowingRoute();
		}

		if (pRouteToStand && (lastClearanceItem.GetMode()== OnExitStand||lastClearanceItem.GetMode() <= OnTaxiToStand ))
		{
			bool bEnd = pRouteToStand->FindClearanceInConcern(pFlight,lastClearanceItem,RadiusOfConcernOnGround,newclearance);
			if(bEnd) 
				return true;

			if (pRouteToStand->GetItemCount()<1 && lastClearanceItem.GetMode() == OnExitStand)
			{
				lastClearanceItem.SetMode(OnTaxiToStand);
			}
		}
		pFlight->m_bTowingToDepStand = false;

	}

	if(pFlight->m_bTowingToDepStand && pFlight->GetTowOffStandType() == REPOSITIONFORDEP)
	{
		StandInSim* pDepStand = pFlight->m_pDepParkingStand;
		if (pDepStand == NULL || (pDepStand && !pDepStand->GetLock(pFlight)))
		{
			AssignOperationParkingStand(pFlight,DEP_PARKING);
		}

		pFlight->m_bTowingToDepStand = false;
	}
	return false;
}




void AirTrafficController::UpdateFlightDeiceRequirment( AirsideFlightInSim* pFlight, FlightDeiceRequirment& deiceRequir)
{
	PLACE_METHOD_TRACK_STRING();
	ElapsedTime curTime = pFlight->GetTime();
	//update surface requirement
	AircraftSurfacecondition curSurfaceCond =  m_aircftSurSet->getFlightSurfaceCondition(pFlight, curTime );
	Precipitationtype curPrecType = m_pWeatherScript->getPrecipitaionType( curTime );
	//]
	CPrecipitationtype *pDemand = NULL;
	CAircraftDeicingAndAnti_icingsSettings::TY_SETTING& settingdata = *m_pDeiceDemand->GetDataSetting();
	for(int i=0;i<(int)settingdata.size();i++)
	{
		CAircraftDeicingAndAnti_icingsSetting* pSet =  settingdata.at(i);
		if( pFlight->fits (*pSet->GetFlightTy()) ) //find the set
		{ 
			pDemand = pSet->GetData(curSurfaceCond, curPrecType );
			break;
		}
	}
	
	if(pDemand)  //get demand
	{		
		deiceRequir.m_antiicefluidReq = pDemand->GetAntiIceTime();
		deiceRequir.m_deicefluidReq = pDemand->GetDeIceTime();
		deiceRequir.m_holdoverTime = pDemand->GetHoldOverTime()*60;
		deiceRequir.m_inspectionTime = ElapsedTime( pDemand->GetInspection()->GetProbDistribution()->getRandomValue()*60 );		
	}
	//deice which way to go update deice strategy
	CDeiceAndAnticeInitionalSurfaceCondition* pStrategy = NULL;
	if(deiceRequir.bNeedDeice())
	{
		for(int i=0;i<m_pDeiceManager->getCount();i++)
		{
			CDeiceAndAnticeFlightType* pType = m_pDeiceManager->getItem(i);
			if( pFlight->fits(pType->GetFltType()) )
			{
				ALTObjectID depStandID = pFlight->getDepStand();
				if(pFlight->GetOperationParkingStand())
					depStandID = pFlight->GetOperationParkingStand()->GetStandInput()->GetObjectName();

				pStrategy = pType->GetData(depStandID,curPrecType,curSurfaceCond);
				break;
			}
		}
	}
	else
	{
		return;
	}

	if(pStrategy)
	{
		deiceRequir.m_antiiceFlowrate = pStrategy->getAntiVehicle()->m_nFlowRate;
		deiceRequir.m_antiiceVehicleNum = pStrategy->getAntiVehicle()->m_nNumber;
		deiceRequir.m_deiceFlowrate = pStrategy->getDeiceVehicle()->m_nFlowRate;
		deiceRequir.m_deiceVehicleNum = pStrategy->getDeiceVehicle()->m_nNumber;
		
		FlightDeiceStrategyDecision& deiceDecide = pFlight->GetDeiceDecision();
		for(int i=1;i<5;i++)
		{
			PriorityType proT = pStrategy->getPriotity(i);
			if( FlightFitPrority(pFlight,pStrategy,proT) )
				break;
		}
		
	}
	if(pFlight->GetDeiceDecision().m_deicePlace == NULL
		&& pFlight->GetDeiceDecision().m_pDeicepadGroup == NULL) //set default deice place
	{
		FlightDeiceStrategyDecision& deiceDecide = pFlight->GetDeiceDecision();
		deiceDecide.m_deicePlace = pFlight->GetResource();
		deiceDecide.m_DistInResource = pFlight->GetDistInResource();
		deiceDecide.mEngState = Off;
		deiceDecide.mtype = DepStand_Type;			
		deiceDecide.m_deiceRoute = 0;   // get deice route here
	}
	FlightDeiceStrategyDecision& deiceDecide = pFlight->GetDeiceDecision();
	deiceDecide.m_InspecAndHoldTime = deiceRequir.m_inspectionTime;

}

bool AirTrafficController::_FitDeicePriority( AirsideFlightInSim* pFlight, DeicePadPriority* pDeicepad )
{
	PLACE_METHOD_TRACK_STRING();
	DeiceResourceManager* pDeiceRes = m_pResources->GetAirportResource()->getDeiceResource();
	ALTObjectGroup deceGrp;
	deceGrp.ReadData(pDeicepad->m_deicePad);
	DeicepadGroup* pGroup = pDeiceRes->AssignGroup(deceGrp, pDeicepad->m_nDeicePadQueue,pFlight);
	if(!pGroup)
	{
		return false;
	}
// 	if( pGroup->getAvaileWithinTime()  > ElapsedTime(pDeicepad->m_nWaittime*60.0) )
// 	{
// 		return false;
// 	}

	//it is ok
	FlightDeiceStrategyDecision& deiceDecide = pFlight->GetDeiceDecision();
	deiceDecide.m_deicePlace = NULL;
	deiceDecide.m_pDeicepadGroup = pGroup;
	deiceDecide.m_DistInResource = 0.0/*pGroup->GetInPath().GetTotalLength() - pFlight->GetLength()*0.5*/;
	deiceDecide.mEngState = pDeicepad->m_emEngineState;
	deiceDecide.mtype = DeicePad_Type;
	deiceDecide.mPosMethod = pDeicepad->m_emType;
	deiceDecide.m_deiceRoute = 0;   // get deice route here

	pGroup->AddApproachingFlight(pFlight);

	return true;
}

bool AirTrafficController::_FitDeicePriority( AirsideFlightInSim* pFlight, DepStandPriority* pStand )
{
	PLACE_METHOD_TRACK_STRING();
	StandResourceManager* pStandRes = m_pResources->GetAirportResource()->getStandResource();
	ALTObjectGroup standGrp;
	standGrp.ReadData(pStand->m_Stand);
	StandInSim* pDeiceStand = pStandRes->GetStandNeedNotWithin(standGrp, pFlight->GetTime(),ElapsedTime(pStand->m_nStandWithinTime*60.0) );
	if( !pDeiceStand )
	{
		return false;
	}
	VehicleRequestDispatcher * pVehicleAssinger = pFlight->GetServiceRequestDispatcher();
	if(pVehicleAssinger)
	{
		ElapsedTime fluidVehileAvaleTime = pVehicleAssinger->getVehicleAvaibleTime(pFlight, pDeiceStand,VehicleType_DeicingTruck );
		if(fluidVehileAvaleTime > ElapsedTime( 60.0* pStand->m_nWaitFluidTime) )
			return false;
		ElapsedTime inpectionVehicleAvalTime = pVehicleAssinger->getVehicleAvaibleTime(pFlight, pDeiceStand, VehicleType_InspectionTruck);
		if( inpectionVehicleAvalTime > ElapsedTime( 60.0* pStand->m_nWaitInspectionTime) )
			return false;
	}

	StandLeadInLineInSim* parkLine = NULL;
	if( pDeiceStand && pDeiceStand->GetLeadInLineCount() )
	{
		parkLine = pDeiceStand->GetLeadInLineByIndex(0);
	}

	//this is fit the require assign this to the flight
	FlightDeiceStrategyDecision& deiceDecide = pFlight->GetDeiceDecision();
	deiceDecide.m_deicePlace = parkLine;
	deiceDecide.m_DistInResource = parkLine->GetPath().GetTotalLength() - pFlight->GetLength()*0.5;
	deiceDecide.mEngState = pStand->m_emEngineState;
	deiceDecide.mtype = DepStand_Type;
	deiceDecide.m_deiceRoute = 0;   // get deice route here

	
	return true;
}

bool AirTrafficController::_FitDeicePriority( AirsideFlightInSim* pFlight, LeadOutPriority* pLeadout )
{
	PLACE_METHOD_TRACK_STRING();
	StandInSim* pCurStand = pFlight->GetOperationParkingStand();
	
	VehicleRequestDispatcher * pVehicleAssinger = pFlight->GetServiceRequestDispatcher();
	if(pVehicleAssinger)
	{
		ElapsedTime fluidVehileAvaleTime = pVehicleAssinger->getVehicleAvaibleTime(pFlight, pCurStand,VehicleType_DeicingTruck );
		if(fluidVehileAvaleTime > ElapsedTime( 60.0* pLeadout->m_nWaitFluidTime) )
			return false;
		ElapsedTime inpectionVehicleAvalTime = pVehicleAssinger->getVehicleAvaibleTime(pFlight, pCurStand, VehicleType_InspectionTruck);
		if( inpectionVehicleAvalTime > ElapsedTime( 60.0* pLeadout->m_nWaitInspectionTime) )
			return false;
	}
	//this is fit the require assign this to the flight
	FlightDeiceStrategyDecision& deiceDecide = pFlight->GetDeiceDecision();
	deiceDecide.m_deicePlace = pCurStand;
	deiceDecide.m_DistInResource = pLeadout->m_nDistance + pFlight->GetLength()*0.5;
	deiceDecide.mEngState = pLeadout->m_emEngineState;
	deiceDecide.mtype = LeadOut_Type;
	deiceDecide.m_deiceRoute = 0;   // get deice route here	
	
	return true;
}

bool AirTrafficController::_FitDeicePriority( AirsideFlightInSim* pFlight, RemoteStandPriority* pRemoteStand )
{
	PLACE_METHOD_TRACK_STRING();
	StandResourceManager* pStandRes = m_pResources->GetAirportResource()->getStandResource();
	ALTObjectGroup standGrp;
	standGrp.ReadData(pRemoteStand->m_Stand);
	StandInSim* pDeiceStand = pStandRes->GetStandNeedNotWithin(standGrp, pFlight->GetTime(),ElapsedTime(pRemoteStand->m_nStandWithinTime*60.0) );
	if( !pDeiceStand )
	{
		return false;
	}
	VehicleRequestDispatcher * pVehicleAssinger = pFlight->GetServiceRequestDispatcher();
	if(pVehicleAssinger)
	{
		ElapsedTime fluidVehileAvaleTime = pVehicleAssinger->getVehicleAvaibleTime(pFlight, pDeiceStand,VehicleType_DeicingTruck );
		if(fluidVehileAvaleTime > ElapsedTime( 60.0* pRemoteStand->m_nWaitFluidTime) )
			return false;
		ElapsedTime inpectionVehicleAvalTime = pVehicleAssinger->getVehicleAvaibleTime(pFlight, pDeiceStand, VehicleType_InspectionTruck);
		if( inpectionVehicleAvalTime > ElapsedTime( 60.0* pRemoteStand->m_nWaitInspectionTime) )
			return false;
	}
	//this is fit the require assign this to the flight
	StandLeadInLineInSim* parkLine = NULL;
	if( pDeiceStand && pDeiceStand->GetLeadInLineCount() )
	{
		parkLine = pDeiceStand->GetLeadInLineByIndex(0);
	}
	FlightDeiceStrategyDecision& deiceDecide = pFlight->GetDeiceDecision();
	deiceDecide.m_deicePlace = parkLine;
	deiceDecide.m_DistInResource = parkLine->GetPath().GetTotalLength()-pFlight->GetLength()*0.5;
	deiceDecide.mEngState = pRemoteStand->m_emEngineState;
	deiceDecide.mtype = RemoteStand_Type;
	deiceDecide.mPosMethod = pRemoteStand->m_emType;
	deiceDecide.m_deiceRoute = 0;   // get deice route here
	
	return true;
}

bool AirTrafficController::FlightFitPrority( AirsideFlightInSim* pFlight, CDeiceAndAnticeInitionalSurfaceCondition* pStragy, PriorityType priT )
{
	PLACE_METHOD_TRACK_STRING();
	switch (priT)
	{
	case DeicePad_Type:
		return _FitDeicePriority(pFlight,pStragy->getDeicePad());
	case DepStand_Type :
		return _FitDeicePriority(pFlight,pStragy->getDepStand());
	case LeadOut_Type:
		return _FitDeicePriority(pFlight, pStragy->getLeadOut());
	case RemoteStand_Type:
		return _FitDeicePriority(pFlight, pStragy->getRemoteStand());
	default:
		return false;
	}
}

void AirTrafficController::AssignRouteToDeice( AirsideFlightInSim* pFlight )
{
	PLACE_METHOD_TRACK_STRING();
	if(pFlight && pFlight->getDeiceServiceRequest() )
	{
		FlightDeiceStrategyDecision& deiceDecide = pFlight->GetDeiceDecision();
		StandLeadOutLineInSim* pStandLeadOut =NULL;
		if(pFlight->GetResource() && pFlight->GetResource()->GetType() == AirsideResource::ResType_StandLeadOutLine)
		{
			pStandLeadOut = (StandLeadOutLineInSim*)pFlight->GetResource();
		}	
		TaxiwayDirectSegInSim* pDirSeg = NULL;
		if(pFlight->GetResource() && pFlight->GetResource()->GetType() == AirsideResource::ResType_TaxiwayDirSeg)
		{
			pDirSeg = (TaxiwayDirectSegInSim*)pFlight->GetResource();
		}
		
		if(deiceDecide.mtype == RemoteStand_Type)
		{
			
			StandLeadInLineInSim* parkLine = NULL;
			if( deiceDecide.m_deicePlace && deiceDecide.m_deicePlace->GetType() == AirsideResource::ResType_StandLeadInLine )
			{
				parkLine = (StandLeadInLineInSim*)deiceDecide.m_deicePlace;
			}

			FlightGroundRouteDirectSegList vSegmets;		
			m_pBoundRouteAssignment->GetRouteFStandTStand(pFlight,pStandLeadOut,parkLine,vSegmets);
			{
				deiceDecide.m_deiceRoute = new TaxiRouteInSim(OnTaxiToDeice,pStandLeadOut->GetStandInSim(),parkLine->GetStandInSim());
// 				deiceDecide.m_deiceRoute->AddTaxiwaySegList(vSegmets);
				bool bCyclicGroundRoute=pFlight->GetEngine()->GetAirsideSimulation()->AllowCyclicGroundRoute();
				deiceDecide.m_deiceRoute->AddTaxiwaySegList(vSegmets,bCyclicGroundRoute);
				pFlight->SetOutBoundRoute(NULL);

			}
		}
		if(deiceDecide.mtype == DepStand_Type)
		{				

			StandLeadInLineInSim* parkLine = NULL;
			if( deiceDecide.m_deicePlace && deiceDecide.m_deicePlace->GetType() == AirsideResource::ResType_StandLeadInLine )
			{
				parkLine = (StandLeadInLineInSim*)deiceDecide.m_deicePlace;
			}

			FlightGroundRouteDirectSegList vSegmets;			
			m_pBoundRouteAssignment->GetRouteFStandTStand(pFlight,pStandLeadOut,parkLine,vSegmets);
			{
				deiceDecide.m_deiceRoute = new TaxiRouteInSim(OnTaxiToDeice,pStandLeadOut->GetStandInSim(),parkLine->GetStandInSim());
// 				deiceDecide.m_deiceRoute->AddTaxiwaySegList(vSegmets);

				bool bCyclicGroundRoute=pFlight->GetEngine()->GetAirsideSimulation()->AllowCyclicGroundRoute();
				deiceDecide.m_deiceRoute->AddTaxiwaySegList(vSegmets,bCyclicGroundRoute);
				pFlight->SetOutBoundRoute(NULL);



			}


		}

		if( deiceDecide.mtype == DeicePad_Type)
		{
			FlightGroundRouteDirectSegList vSegmets;
			DeicepadGroupEntry* pEntry = NULL;
			AirsideResource* pFromRes = NULL;
			if(pStandLeadOut)
			{
				pEntry = m_pBoundRouteAssignment->GetRouteFStandTDeicepadGroup(pFlight,pStandLeadOut,deiceDecide.m_pDeicepadGroup,vSegmets);
				pFromRes = pStandLeadOut->GetStandInSim();	
			}

			if(pDirSeg)
			{
				pEntry = m_pBoundRouteAssignment->GetRouteToDeiceGroup(pFlight,pDirSeg,deiceDecide.m_pDeicepadGroup,vSegmets);
				pFromRes = pDirSeg;
			}			
			if(pEntry)
			{
				DeiceRouteInSim* pDeiceRoute= new DeiceRouteInSim(pFromRes,pEntry->GetIntersectionNode());
// 				pDeiceRoute->AddTaxiwaySegList(vSegmets);

				bool bCyclicGroundRoute=pFlight->GetEngine()->GetAirsideSimulation()->AllowCyclicGroundRoute();
				pDeiceRoute->AddTaxiwaySegList(vSegmets);
				
// 				m_pBoundRouteAssignment->GetRouteFDeiceTTakoff(pFlight,pGroup,pFlight->GetTakeoffPosition(),vSegmets);
// 				pDeiceRoute->setAfterRoute(vSegmets);
				
				deiceDecide.m_pDeicepadGroupEntry = pEntry;
				deiceDecide.m_deiceRoute = pDeiceRoute;
				pFlight->SetOutBoundRoute(NULL);
			}
		}
	}
}

void AirTrafficController::AssignAvaibleRunwayExit( AirsideFlightInSim* pFlight )
{
	PLACE_METHOD_TRACK_STRING();
	LogicRunwayInSim * pLandRunway = pFlight->GetLandingRunway();
	RunwayExitInSim * pExit = NULL;

	if(!pLandRunway) return;
	ASSERT(pLandRunway->GetExitCount() > 0);
	pExit =  m_RunwayExitAssignmentStrategies.GetAvailableRunwayExit(pFlight,m_pAirportDB,m_pResources->GetAirportResource()->getStandResource(),true);
	pFlight->SetRunwayExit( pExit );
	//ASSERT( pExit->GetPosAtRunway() > 30000 );
}

void AirTrafficController::SetFltOperatingDoorSpec( const FltOperatingDoorSpecInSim* pSpec )
{
	m_pFltOperatingDoorSpec = const_cast<FltOperatingDoorSpecInSim*>(pSpec);
}

FltOperatingDoorSpecInSim* AirTrafficController::GetFltOperatingDoorSpec()
{
	return m_pFltOperatingDoorSpec;
}

int AirTrafficController::GetTaxiToRunwayFlightCount( LogicRunwayInSim* pRwyPort )
{
	PLACE_METHOD_TRACK_STRING();
	int nCount = 0;
	for (int i =0; i < int(m_vFlights.size()); i++)
	{
		AirsideFlightInSim* _pflight = m_vFlights[i];

		if (_pflight->GetMode() == OnTaxiToRunway || (_pflight->GetMode() == OnExitStand && !_pflight->m_bTowingToIntStand && !_pflight->m_bTowingToDepStand))			//Outbound
		{
			if (_pflight->m_pDepatrueRunway != NULL && _pflight->m_pDepatrueRunway == pRwyPort )
				nCount++;
		}
	}
	return nCount;
}

void AirTrafficController::SetFlightList( std::vector<AirsideFlightInSim*>& vFlights )
{
    m_vFlights  = vFlights;	
}

void AirTrafficController::SetResourceManager( AirsideResourceManager * pRes )
{
	m_pResources=pRes;
}


StandInSim* AirTrafficController::IsCurrentOccupiedFlightAdjacencyConflict(AirsideFlightInSim* pFlight)
{
	return m_pDynamicStandReassignment->IsCurrentOccupiedFlightAdjacencyConflict(pFlight->GetOperationParkingStand(),pFlight);
}

bool AirTrafficController::IsGateAssignConstraintsConflict(AirsideFlightInSim* pFlight)
{
	return m_pDynamicStandReassignment->IsGateAssignConstraintsConflict(pFlight->GetOperationParkingStand(),pFlight);
}

void AirTrafficController::ChangeFlightTakeoffPos( AirsideFlightInSim* pFlt,const ElapsedTime& t )
{
	if(!pFlt->IsDepartingOperation())
		return;


	if(!m_RunwayAssignmentStrategies.IsAbleToChangeTakeoffPos(pFlt))
		return;

	RunwayExitInSim	* pNewTakeoffPosition = m_RunwayAssignmentStrategies.GetTakeoffPosition(pFlt,m_pResources->GetAirportResource()->getRunwayResource());
	if(pNewTakeoffPosition && pNewTakeoffPosition!=pFlt->GetTakeoffPosition())
	{
		//unplan route
		
		if(TaxiRouteInSim* pOutRoute = pFlt->GetOutBoundRoute())
		{		
			if(TaxiRouteInSim *pNewRoute = getFlightOutBoundRoute(pFlt->GetResource(),pNewTakeoffPosition,pFlt))
			{
				if( pNewRoute->IsRouteValide(pFlt) )
				{
					
					pNewRoute->InitRoute(pFlt,t);
					//pOutRoute->FlightExitRoute(pFlt,t);
					pFlt->SetOutBoundRoute(pNewRoute);
					pFlt->SetTakeoffPosition(pNewTakeoffPosition);
				
				}
				else
				{
					delete (pNewRoute);
					pNewRoute = NULL;
				}
			}
		}

	}

	//change the takeoff position and
}

TaxiRouteInSim* AirTrafficController::getFlightOutBoundRoute( AirsideResource* pFrom,RunwayExitInSim* pTakeoffPos, AirsideFlightInSim* pFlight )
{
	TakeoffQueueInSim * pQueue = m_pResources->GetAirportResource()->getTakeoffQueuesManager()->GetRunwayExitQueue(pTakeoffPos);
	FlightGroundRouteDirectSegList vSegmets;
	if(!getOutBoundSeg(pFrom,pTakeoffPos,pFlight,vSegmets))
	{
		CString strWarn;
		strWarn.Format("There is no taxi route for the flight from %s to takeoff position(%s)", pFrom->PrintResource(),pTakeoffPos->GetExitInput().GetName());
		CString strError = _T("FLIGHT ERROR");
		AirsideSimErrorShown::SimWarning(pFlight,strWarn,strError);
		return NULL ;

	}

	TaxiRouteInSim* pret =  new TaxiOutboundRouteInSim(OnTaxiToRunway,pFrom,pTakeoffPos->GetLogicRunway());
	// 	pFlight->m_pOutBoundRoute->AddTaxiwaySegList(vSegmets);
	bool bCyclicGroundRoute=pFlight->GetEngine()->GetAirsideSimulation()->AllowCyclicGroundRoute();
	pret->AddTaxiwaySegList(vSegmets,bCyclicGroundRoute);
	return pret;
}
