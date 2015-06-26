#include "StdAfx.h"
#include "TerminalMobElementBehavior.h"
#include "PaxTerminalBehavior.h"
#include "Person.h"
#include "main\termplandoc.h"
#include "main\Floor2.h"

#include "pax.h"
#include "process.h"
#include "baggage.h"
#include "barrier.h"
#include "proclist.h"
#include "hold.h"
#include "movevent.h"
#include "terminal.h"
#include "greet.h"
#include "turnarnd.h"
#include "rule.h"
#include "groupleaderinfo.h"
#include "groupfollowerinfo.h"
#include "procq.h"
#include "sequencq.h"
#include "AvoidDensityEvent.h"
#include "conveyor.h"
#include "dep_srce.h"
#include "dep_sink.h"
#include "anServeProcList.h"

#include "common\elaptime.h"
#include "common\term_bin.h"
#include "common\point.h"
#include "common\line.h"
#include "term_err.h"
#include "common\ARCException.h"
#include "common\CodeTimeTest.h"
#include "inputs\pax_db.h"
#include "inputs\paxdata.h"
#include "inputs\probab.h"
#include "inputs\procdist.h"
#include "inputs\schedule.h"
#include "inputs\in_term.h"
#include "inputs\flight.h"
#include "inputs\simparameter.h"
#include "inputs\pipeindexlist.h"
#include "inputs\pipe.h"
#include "inputs\PipeDataSet.h"
#include "inputs\MobileElemConstraintDatabase.h"
#include "inputs\SubFlow.h"
#include "inputs\SubFlowList.h"
#include "inputs\AllCarSchedule.h"
#include "inputs\bagdata.h"
#include "inputs\pipegraph.h"
#include "inputs\resourcepool.h"
#include "inputs\allgreetingprocessors.h"
#include "main\ProcDataPage.h"
#include "main\ProcDataSheet.h"
#include "results\paxlog.h"
#include "results\out_term.h"
#include "results\ProcLog.h"
#include "results\paxdestdiagnoseinfo.h"
#include "inputs\SingleProcessFlow.h"
#include "BillboardProc.h"
#include "SimFlowSync.h"

#include "MobElementsDiagnosis.h"
#include "SimEngineConfig.h"
#include "AirsideMobElementBehavior.h"

#include <algorithm>
#include <iostream>
#include <math.h>
#include <fstream>
#include "Airside/AirsideSimulation.h"
#include "Airside/FlightServiceReqirementInSim.h"
#include "engine/gate.h"
#include "inputs/NonPaxRelativePosSpec.h"
#include "BridgeConnector.h"
#include "../Common/ARCTracker.h"
#include <limits>
#include "Common/ARCTracker.h"
#include <common/ARCVector4.h>

#include "ARCportEngine.h"

#include <Inputs/ProcessorDistributionWithPipe.h>
#include <Inputs/PROCIDList.h>
#include "DEP_SINK.H"
#include "PaxEnplaneBehavior.h"
#include "PaxLandsideBehavior.h"
#include "ArrivalPaxLandsideBehavior.h"
#include "OnboardSimulation.h"
#include "OnboardFlightInSim.h"
#include "OnboardSeatInSim.h"
#include "OnboardDoorInSim.h"
#include "Airside/AirsidePaxBusInSim.h"
#include "SimFlowSync.h"
#include "LandsideSimulation.h"
#include "MoveToInterestedEntryEvent.h"
#include "FIXEDQ.H"

ElapsedTime TerminalMobElementBehavior::m_timeFireEvacuation(23*3600+59*60+59l);

TerminalMobElementBehavior::TerminalMobElementBehavior(Person* _pPerson)
:m_pPerson( _pPerson )
,m_bInBillboardLinkedproc(false)
,m_nWaiting(0)
,m_nStranded(0)
,m_nCurrentIndex(0)
,m_nNextIndex(0)
,m_nNextState(Birth)
,m_nQueuePathSegmentIndex(0)
,m_nAvoidingBarrier(0)
,m_pPersonOnStation(NULL)
,m_bWalkOnMovingSideWalk(false)
,m_dCurMovingSidewalkGap(0.0)
,m_nInGateDetailMode(NORMAL_MODE)
,m_iPosIdxOnBaggageProc(0)
,m_bGerentGreetEventOrWait(false)
,m_pWantToGreetPerson(NULL)
,m_iNumberOfPassedGateProc(0)
,m_iNumberOfPassedReclaimProc(0)
,m_timeEntryQueueTime(0l)
,m_bWaitInLinkedArea(false)
,m_pBackToProcFromWaitArea(NULL)
,m_iSourceFloor(-1)
,m_iDestFloor(-1)
,m_bHasProcessedFireEvacuation(false)
,m_bPersonInStationSystem(false)
,m_iDestinationSlot(-1)
,m_iSorterEntryExitProcs(-1)
,m_pDirectGoToNextProc(NULL)
,m_bHaveServiceByResource(false)
,m_lGroupAttachLogIndex(0l)
,m_iStopReason(-1)
,m_nextHoldAiearPoTag(false)
,m_bus(NULL)
,m_emBridgeState(NonState)
,m_pFlowList(NULL)
,m_pPreFlowList(NULL)
,m_pTerm(_pPerson->m_pTerm)
,m_bEvacuationWhenInPipe(false)
,m_bUserPipes(false)
,m_pFlightAirside(NULL)
,m_TransferState(TRANSFER_ARRIVAL)
,m_pLoaderProc(NULL)
,m_bNeedLoaderToHoldArea(false)
,m_IsArrivedFixQueue(FALSE)
,m_bFromLandSide(FALSE)
,inElevotor(false)
,m_iAvoidDirection(0)
,m_bCanServeWhenClosing(0)
,m_bIsArrivalDoor(false)
,m_IsWalkOnBridge(FALSE)
,m_bhasBusServer(FALSE)
//,m_nBridgeIndex(-1)
,m_entryPointCorner(-1)
,m_pLastTerminalProc(NULL)
{
	m_pProcessor = m_pTerm->procList->getProcessor (START_PROCESSOR_INDEX);

	// speed	
	m_desiredSpeed = _pPerson->getSpeed();

	m_pDiagnoseInfo = new CPaxDestDiagnoseInfo( m_pTerm, _pPerson->getID() );
}

TerminalMobElementBehavior::~TerminalMobElementBehavior()
{
	if( m_pDiagnoseInfo )
		delete m_pDiagnoseInfo;
	m_pDiagnoseInfo = NULL;
}

extern long eventNumber;
int TerminalMobElementBehavior::performanceMove(ElapsedTime p_time,bool bNoLog)
{
	return m_pPerson->processTerminalModeMovement(p_time, bNoLog);
}

CARCportEngine * TerminalMobElementBehavior::getEngine()
{
	return m_pPerson->getEngine();
}
//process fire evacuation logic
// that is , select shortest path to let person evacuate from current position
void TerminalMobElementBehavior::ProcessFireEvacuation( const ElapsedTime& _timeToEvacuation ){
	/********************************************************************************
	// we don't care about how check bag, hand bag.....ect evacuate from fire, just let
	// them stay at current position
	if( m_pPerson->getType().GetTypeIndex() > 1 )
	{
	flushLog (_timeToEvacuation);
	return ;
	}
	**********************************************************************************/
	m_bEvacuationWhenInPipe = false;

	// leave current process
	m_pProcessor->removePerson( m_pPerson );

	// find all exit door
	std::vector<Processor*>vCanSelected;
	for( int i=0; i<getTerminal()->procList->getProcessorCount(); ++i )
	{
		Processor* pProc = getTerminal()->procList->getProcessor(i);
		if( pProc->getEmergentFlag() && 
			pProc->getServicePoint(0).getZ() == getLocation().getZ() )
		{
			vCanSelected.push_back( pProc );
		}
	}

	if( vCanSelected.size() == 0 )	// no emergent exit
	{
		CString StringError;
		StringError.Format("no emergent exit on %d layer", (int)(getLocation().getZ() / SCALE_FACTOR ) );

		// add by hans 2005-10-9
		char str[64];
		ElapsedTime tmptime(_timeToEvacuation.asSeconds() % WholeDay);
		tmptime.printTime ( str );

		throw new ARCNoEmergeExitError( StringError, "", ClacTimeString(_timeToEvacuation) );

		//		throw new ARCNoEmergeExitError( StringError, "", _timeToEvacuation.printTime() );
	}

	// find the nearest exit door
	double	dNearestDistance = (double)INT_MAX;
	int		iNearestIndex = 0;
	for( UINT j=0; j<vCanSelected.size(); j++ )
	{
		double dDistance = getLocation().distance( vCanSelected[j]->getServicePoint(0) );
		if( dDistance < dNearestDistance )
		{
			dNearestDistance = dDistance;
			iNearestIndex = j;
		}
	}
	m_pProcessor = vCanSelected[iNearestIndex];

	m_pPerson->setState( EvacuationFire );
	writeLogEntry( _timeToEvacuation, false );

	m_pPerson->setState( FreeMoving );

	// move to the exit door by pipe if there are some pipe on the layer
	// else move to the exit door direction
	ElapsedTime timeTmp = _timeToEvacuation;
	WalkAlongShortestPathForEvac( getLocation(), m_pProcessor->AcquireServiceLocation( m_pPerson ), timeTmp );

	m_bHasProcessedFireEvacuation = true;
	m_pProcessor->addPerson (m_pPerson);
	m_pProcessor->AddRecycleFrequency(m_pPerson);
	generateEvent( timeTmp + moveTime() ,false);
}

// checked bags state's update is passive, need to be update ASAP. 
void TerminalMobElementBehavior::updateCheckedBagStateOnReclaim(const ElapsedTime& _curTime)
{
	ProcessorArray baggateList;
	getTerminal()->procList->GetBaggateProcessorList( baggateList );
	if(baggateList.getCount() == 0 )
		return;

	for (int i=0 ;i < baggateList.getCount(); i++)
	{
		BaggageProcessor *pRecliam = (BaggageProcessor *)baggateList.getItem( i );
		ASSERT(pRecliam);
		pRecliam->updateBaggageOnReclaimState(_curTime);
	}

}

void TerminalMobElementBehavior::processPersonOnStation(ElapsedTime p_time, bool& _needReturn, bool& _localResult)
{
	PLACE_METHOD_TRACK_STRING();
	_needReturn = false;

	if(m_pPersonOnStation && m_pPerson->getState() != WalkOnPipe)
	{
		if( !m_pPersonOnStation->Process( p_time ) )
		{
			if( m_pPersonOnStation->IsDone() )
			{
				m_pPerson->setState( FreeMoving );
				if( !getNextProcessor( p_time ) )
				{
					delete m_pPersonOnStation;
					m_pPersonOnStation = NULL;

					_needReturn = true;
					_localResult = false;
				}
				generateEvent( p_time,false );
			}
			else
			{
				m_pPerson->kill( p_time );
			}
			delete m_pPersonOnStation;
			m_pPersonOnStation = NULL;
		}
		_needReturn = true;
		_localResult = true;
	}
}

//It gets first Processor, first state, destination and schedule event.
//Note that first Processor should not have a queue.
//Always returns true.
int TerminalMobElementBehavior::processBirth (ElapsedTime p_time)
{
	if (getEngine()->GetLandsideSimulation()&&simEngineConfig()->IsLandsideSel() && m_pPerson->getType().isArrival())
	{
		LandsideSimulation* pLandsideSim = getEngine()->GetLandsideSimulation();
		pLandsideSim->GenerateIndividualArrivalPaxConverger(getEngine(),m_pPerson->getID(),p_time);
	}

	//The passenger might not be generated based on the bulk profile
	if(m_pPerson->getLogEntry().getBirthExceptionCode() == BEC_BULKCAPACITY ||
		m_pPerson->getLogEntry().getBirthExceptionCode() == BEC_LANDSIDEVEHICLEOVERCAPACITY)
	{//the passenger would be terminate here, for him cannot take Bulk

		int nDepFlightIdx = m_pPerson->getLogEntry().getDepFlight();
		if( nDepFlightIdx < 0 )
			return false;
		Flight *pFlight = getEngine()->m_simFlightSchedule.getItem( nDepFlightIdx );

		ElapsedTime actureDepTimeWithDelay = pFlight->getDepTime() + pFlight->getDepDelay();

		char szBuffer[128] = {0};
		CString sPrint;
		FORMATMSGEX msgEx;

		CStartDate sDate = m_pTerm->m_pSimParam->GetStartDate();
		bool bAbsDate;
		COleDateTime date;
		int nDtIdx;
		COleDateTime time;
		CString sDateTimeStr;
		sDate.GetDateTime( p_time, bAbsDate, date, nDtIdx, time );
		if( bAbsDate )
		{
			sDateTimeStr = date.Format(_T("%Y-%m-%d"));
		}
		else
		{
			sDateTimeStr.Format("Day %d", nDtIdx + 1 );
		}
		sDateTimeStr += "  ";
		ElapsedTime tmptime(p_time.asSeconds() % WholeDay);
		sDateTimeStr += tmptime.printTime();

		msgEx.strTime			= sDateTimeStr;
		msgEx.strMsgType		= "MISSED FLIGHT";
		msgEx.strProcessorID	= getProcessor() ? getProcessor()->getID()->GetIDString() : "";
		msgEx.strPaxID.Format("%ld", m_pPerson->getID() );
		pFlight->getFullID( szBuffer, 'D' );
		//szBuffer[7] = NULL;
		msgEx.strFlightID.Format(_T("%s"),szBuffer) ;		
		m_pPerson->getType().screenPrint( sPrint, 0, 128 ); 
		msgEx.strMobileType		= sPrint;//CString( szBuffer );
		if(m_pPerson->m_pGroupInfo->IsFollower() && m_pPerson->m_pGroupInfo->GetGroupLeader() != NULL)
		{
			long lID = m_pPerson->m_pGroupInfo->GetGroupLeader()->getID();
			msgEx.strOther.Format("missed its flight by leader %d in group", lID);
		}
		else if(m_pPerson->getLogEntry().getBirthExceptionCode() == BEC_BULKCAPACITY)
		{
			msgEx.strOther.Format( "Cannot take Bulk for having no enough capacity,please refer to Bulk Profile settings.");
		}
		else if(m_pPerson->getLogEntry().getBirthExceptionCode() == BEC_LANDSIDEVEHICLEOVERCAPACITY)
		{
			msgEx.strOther.Format( "Has no landside vehicle to serve the passenger due to the capacity limitation.");
		}
		else
		{
			ASSERT(0);
		}
		msgEx.diag_entry.eType	= MISS_FLIGHT;
		msgEx.diag_entry.lData	= m_pPerson->getLogEntry().getIndex();
		m_pPerson->getLogEntry().SetMissFlightFlg(true);
		m_pPerson->getLogEntry().SetMissFlightTime(p_time);
		m_pPerson->getLogEntry().SetRealDepartureTime(actureDepTimeWithDelay);

		getEngine()->SendSimFormatExMessage( msgEx );

		if( m_pProcessor )
		{
			m_pProcessor->removePerson( m_pPerson );
			m_pProcessor->makeAvailable( m_pPerson, p_time, false );
		}

		m_pPerson->writeTerminalBeginLogEntry(p_time);
		m_pPerson->writeTerminalEndLogEntry(p_time);

		flushLog (p_time,true);

		return FALSE;
	}


	if (!getNextProcessor (p_time)) 
		return FALSE;

	int nState = FreeMoving;
	m_pProcessor->getNextState (nState,m_pPerson);
	m_pPerson->setState(nState);
	m_pProcessor->getNextLocation (m_pPerson);

	//   location = m_ptDestination;		//??
	if (m_pPerson->getState() == MoveToQueue)
		m_pProcessor->addToQueue (m_pPerson, p_time);

 	if (!m_IsWalkOnBridge)
 	{
        if(!m_bFromLandSide)
 		    location = m_ptDestination;
 	}

	getTerminal()->m_pAreas->handlePersonBirth( m_pPerson, getLocation(), p_time );
	//enter terminal mode
	ElapsedTime dt = moveTime();
	m_pPerson->writeTerminalBeginLogEntry(p_time + dt);

	if (m_pProcessor->getProcessorType() == BridgeConnectorProc)
	{
	//	m_bIsArrivalDoor = true;
		m_pPerson->SetFollowerArrivalDoor(true);
	}
	generateEvent (p_time + dt,false);
	return TRUE;
}

//	Mobile Elements transferring process in between ALTO Modes.
//  corrupt logic implementation. 
//  <TODO::> Passenger's mode transfer action need handled by ALTO MobElement Mode Transfer service.
void TerminalMobElementBehavior::processEntryAirside(ElapsedTime p_time)
{
	//sim terminal mode alone, just flush log.
	if( simEngineConfig()->isSimTerminalModeAlone() )
	{
		flushLog (p_time);
		return;
	}

	// do not need departing to airside mode.
	if( !simEngineConfig()->isSimAirsideMode())
	{
		flushLog( p_time );
		return;
	}

	//----------------------------------------------------
	// sim Terminal and airside mode together.

	// arriving passenger do not need departing to airside.
	if(m_pPerson->getLogEntry().isArriving() && !m_pPerson->getLogEntry().isTurnaround())
	{
		flushLog( p_time );
		return;
	}

	// not arriving and departing, impossible, just flush log.
	if(false == m_pPerson->getLogEntry().isDeparting())
	{
		flushLog( p_time );
		return;
	}

	// handle departing passenger's mode transfer action.

	//Must be Passenger!!!  (Passenger*)m_pPerson
	//By now, AirsideMobElementBehavior can just handle passenger's movement logic.
	if(m_pPerson->getLogEntry().GetMobileType() == 0)
	{
		EntityEvents State = _DEATH ;

		if(HasBusForThisFlight(m_pPerson->getLogEntry().getDepFlight()),m_pPerson)
			State = WALK_OUT_GATE ;

		if(State == WALK_OUT_GATE)
		{
			if(EnterFromTerminalModeToOtherMode())//onboard mode
			{
				setState(EntryOnboard);
				generateEvent (p_time,false);
			}
			else
			{
				AirsideFlightInSim* Airflight = getEngine()->GetAirsideSimulation()->GetAirsideFlight(m_pPerson->GetCurFlightIndex()) ;
				if (Airflight)
					Airflight->AddPaxInFlight();
				generateEvent( p_time, false);
				m_pPerson->setBehavior( new AirsidePassengerBehavior( m_pPerson,State));// departing to Airside mode.
			}
			return;
		}
		else// just stop departing to Airside.(NO stand defined for passenger's flight)
		{
			flushLog (p_time);
			return;
		}
	}
	else
	{
		// NonPax type(visitor) exit terminal mode.
		//ASSERT(m_pPerson->getLogEntry().GetMobileType() == 1);

		// if pax type is 2, baggage, throw exception.
		// by now, simEngine does not support baggage move alone to airside mode.

		flushLog (p_time);
		return;
	}


	ASSERT(FALSE);
	return;
}
void TerminalMobElementBehavior::processEntryOnboard( ElapsedTime p_time )
{
	// do not need departing to onboard mode.
	if (m_pProcessor)
	{
		m_pProcessor->removePerson(m_pPerson);
	}

	if( !simEngineConfig()->isSimOnBoardMode())// if no selected, 
	{
		flushLog( p_time );
		return;
	}

	//----------------------------------------------------
	// sim Terminal and airside mode together.

	// arriving passenger do not need departing to airside.
	if(m_pPerson->getLogEntry().isArriving() && !m_pPerson->getLogEntry().isTurnaround())
	{
		flushLog( p_time );
		return;
	}

	// not arriving and departing, impossible, just flush log.
	if(false == m_pPerson->getLogEntry().isDeparting())
	{
		flushLog( p_time );
		return;
	}
	// handle departing passenger's mode transfer action.

	//Must be Passenger!!!  (Passenger*)m_pPerson
	//By now, AirsideMobElementBehavior can just handle passenger's movement logic.
	if(m_pPerson->getLogEntry().GetMobileType() == 0)
	{
		//set enplane behavior
		m_pProcessor->removePerson(m_pPerson);
		PaxEnplaneBehavior* pEnplaneBehavior = new PaxEnplaneBehavior(m_pPerson);
		m_pPerson->setBehavior(pEnplaneBehavior); 
		pEnplaneBehavior->SetGroupBehavior();
		pEnplaneBehavior->SetFollowBehavior();
		
		generateEvent(p_time,false);
		return;
	}
	else
	{
		// NonPax type(visitor) exit terminal mode.
		//ASSERT(m_pPerson->getLogEntry().GetMobileType() == 1);

		// if pax type is 2, baggage, throw exception.
		// by now, simEngine doesnot support baggage move alone to airside mode.

		flushLog (p_time);
		return;
	}

}
//It calculates next state and destination recursively until a direct route to the Processor is found.
//It determines whether Person can travel a direct route to his destination. If not, an intermediary destination will be assigned.
void TerminalMobElementBehavior::processGeneralMovement (ElapsedTime p_time)
{
	//	// TRACE("\nid = %s\n",m_pProcessor->getID()->GetIDString() );
	bool bUsedPipe = false;
	if(!m_vPipePointList.empty())
	{
		setDestination(m_vPipePointList.front().pt);

		//write landside walkway with pipe log
		if (getEngine()->IsLandSel())
		{
			int nCurPipe = m_vPipePointList.front().m_nCurPipe;
			if (nCurPipe >= 0 && nCurPipe < m_pTerm->m_pPipeDataSet->GetPipeCount())
			{
				CPipe* pPipe = m_pTerm->m_pPipeDataSet->GetPipeAt(m_vPipePointList.front().m_nCurPipe);
				bool bCreate = (m_vPipePointList.front().m_nCurPipe != m_vPipePointList.front().m_nPrePipe ? true : false);
				ElapsedTime tMoveTime = moveTime();
				if (pPipe)
				{
					pPipe->WritePipePax(getEngine()->GetLandsideSimulation(),getLocation(),getDest(),m_pPerson->getID(),p_time,tMoveTime,bCreate);
				}	
			}
		}
		
		m_vPipePointList.erase(m_vPipePointList.begin());

		if(m_vPipePointList.empty())
		{
			if(m_pProcessor->GetQueue() && m_pProcessor->GetQueue()->isFixed() == 'Y')
			{
				MoveToInterestedEntryEvent* pEvent = new MoveToInterestedEntryEvent;
				pEvent->init(m_pPerson, p_time + moveTime(), false);
				FixedQueue* pFixQ = (FixedQueue*)m_pProcessor->GetQueue();
				ASSERT(pFixQ->isFixed() == 'Y');
				Point ptQEntry = pFixQ->corner(pFixQ->getFixQEntryPointIndex());
				pEvent->calculateMovingPipe(m_ptDestination, ptQEntry);
				m_entryPointCorner = pFixQ->getFixQEntryPointIndex();
				pEvent->addEvent();
				m_pPerson->SetPrevEventTime(pEvent->getTime());
				return;
			}
			m_pPerson->setState(FreeMoving);
		}

		bUsedPipe = TRUE;
	}
	else
	{
		if (!m_nAvoidingBarrier || m_nNextState == MoveToQueue)
		{
			//// TRACE(m_pProcessor->getID()->GetIDString().GetBuffer(0));
			int nState = m_pPerson->getState();
			m_pProcessor->getNextState (nState,m_pPerson);
			m_pPerson->setState(nState);
			m_nNextState = m_pPerson->getState();
			m_pProcessor->getNextLocation (m_pPerson);
			m_nextPoint = m_ptDestination;
		}
		else
		{
			m_pPerson->setState(m_nNextState);
			setDestination (m_nextPoint);
		}
		m_nAvoidingBarrier = 0;

		// determines whether Person can travel a direct route to his
		// destination. If not, an intermediary destination will be assigned
		if (!hasClearPath(p_time))
		{
			m_nAvoidingBarrier = 1;
			m_pPerson->setState(m_pPerson->getState());

			if(m_pPerson->m_iPreState == FreeMoving )
				processBillboard(p_time);

			m_pPerson->setState(FreeMoving);
			generateEvent (p_time + moveTime(),false);
			return;
		}

	}

	if( getTerminal()->m_pSimParam->GetCongestionImpactFlag()>0 && 
		getEngine()->getCongestionAreaManager().CheckAndHandleCongestion(m_pPerson, p_time, TRUE))
		return ;
	else
	{
		if(m_pPerson->m_iPreState == FreeMoving && bUsedPipe == false)
			processBillboard(p_time);
		processGeneralMovementHelp(p_time);
	}
}

void TerminalMobElementBehavior::processGeneralMovementHelp (ElapsedTime p_time)
{
	if (m_pPerson->getState() == MoveToQueue)
		m_pProcessor->addToQueue (m_pPerson, p_time);


	generateEvent (p_time + moveTime(),false);
}

//It goes through passengers who are already at first InConstraint location and sets them next destination.
//Finally it calls processGeneralMovement().
void TerminalMobElementBehavior::processInConstraint (ElapsedTime p_time)
{
	m_nCurrentIndex = 1;
	while (m_nCurrentIndex < m_pProcessor->inConstraintLength())
	{
		setDestination (m_pProcessor->inConstraint (m_nCurrentIndex++));
		p_time += moveTime();
		writeLogEntry (p_time, false);
	}

	m_pPerson->setState(LeaveInConstraint);
	generateEvent (p_time ,false);
	//processGeneralMovement (p_time);
}



// It simply notify queue, who will either tell the next m_nWaiting person to advance or adjust the tail location (possibly end of overflow).
// If arrival, set personal flag to m_nWaiting.
// Else, notify p of arrival for adjustment of tail void.
void TerminalMobElementBehavior::processQueueMovement (ElapsedTime p_time)
{
	/*
	//Sync with person's family
	//group lead must be a Passenger
	if( m_pSimFlowSync != NULL && (false == m_pSimFlowSync->CanIGoFurther(m_pPerson, *m_pProcessor->getID(), p_time)) )
	{
	return;
	}
	*/
	if(m_pProcessor->GetQueue() == NULL)
	{
		if(m_pProcessor->getProcessorType() == BridgeConnectorProc)
		{
			//terminate passenger
			if( IsLateForDepFlight( p_time, m_pProcessor->GetTerminateTime() ) )
			{
				m_pProcessor->removePerson( m_pPerson );
			}
		}
		flushLog (p_time,true);
		return ;
		ASSERT(0);
	}


	// must first check for relayAdvance event
	if (m_pPerson->getState() == StartMovement)
		m_pProcessor->moveThroughQueue (m_pPerson, p_time);
	// m_nWaiting flag will be set to TRUE when pax arrives at queue tail
	// allows distinction between arrive at tail and movement within queue
	else if (!m_nWaiting)
		m_pProcessor->arriveAtQueue (m_pPerson, p_time, false );
	else
		m_pProcessor->notifyQueueAdvance (m_pPerson, p_time, false );
}

//It informs queue of departure (start whiplash effect).
//Next state is set. Destination is obtained.
//It processes next point immediately, no decisions required.
void TerminalMobElementBehavior::processQueueDeparture (ElapsedTime p_time )
{
	//Sync with person's family
	//group lead must be a Passenger
// 	if( m_pSimFlowSync != NULL && 
// 		(false == m_pSimFlowSync->CanIGoFurther(m_pPerson, *m_pProcessor->getID(), p_time)) )
// 	{
// 		return;
// 	}
	if(ProcessPassengerSync(p_time) == false)
		return;
	/*
	ProcessorDistribution* pNextFlow=m_pProcessor->getNextDestinations ( m_pPerson->getType(), m_nInGateDetailMode );
	bool bNeedCheckNextProc=!(m_pProcessor->getProcessorType()==DepSourceProc && ((DependentSource*)m_pProcessor)->GetReservedSink()>=0);

	if(bNeedCheckNextProc && pNextFlow)
	{
	//SelectNextProcessor(), m_pPerson function can select processor and restore person's info.
	Processor* pNextProcs = NULL;

	try
	{
	pNextProcs = SelectNextProcessor( pNextFlow, p_time);
	}
	catch ( ARCSimEngineException* _pSimEx ) 
	{
	delete _pSimEx;
	}

	if(pNextProcs==NULL || pNextProcs->isClosed())
	{
	const ProcessorID* nextGroup = pNextFlow->getDestinationGroup (RANDOM);
	if(pNextProcs==NULL || m_pTerm->procList->FindWorthWaitedProcs(nextGroup, p_time, m_pPerson->getType() ,m_nGate))
	{
	generateEvent(p_time+ElapsedTime(1L));
	return;
	}

	}
	}
	*/

	m_pProcessor->leaveQueue ( m_pPerson, p_time, false );
	m_nWaiting = FALSE;

	m_pPerson->setState(ArriveAtServer);
	m_pProcessor->getNextLocation (m_pPerson);
	Point nextPoint = m_ptDestination;
	// determines whether Person can travel a direct route to his
	// destination. if not, an intermediary destination will be assigned
	while (!hasClearPath(p_time))
	{
		p_time += moveTime();
		m_pPerson->setState(MoveToServer);
		writeLogEntry (p_time, false);
		setDestination (nextPoint);
		m_pPerson->setState(ArriveAtServer);
	}
	generateEvent (p_time + moveTime(),false);
}

//Only visitors use m_pPerson method.
//Porcessor's beginService() is invoked.
void TerminalMobElementBehavior::processServerArrival (ElapsedTime p_time)
{
	m_pPerson->setState(LeaveServer);
	//	// TRACE("\nid = %s\n",m_pProcessor->getID()->GetIDString() );
	m_pProcessor->beginService (m_pPerson, p_time);
}

// simulate bag check's time after greeting. 
void TerminalMobElementBehavior::processGreeting(ElapsedTime p_time)
{
	ElapsedTime tempTime = p_time;
	if(m_pProcessor->getProcessorType() == HoldAreaProc)// always be true
	{
		if( ((HoldingArea*) m_pProcessor)->getApplyServiceTimeAfterGreeting() )
		{
			tempTime += m_pProcessor->getServiceTime( m_pPerson,  p_time );
		}
	}

	m_pPerson->setState(LeaveServer);

	writeLogEntry (tempTime, false);
	generateEvent (tempTime, false);
}

//There is no event required:
//if no OutConstraint, it is ready to choose next Processor;
//if there is OutConstraint, there is no decision made until OutConstaint traversed and event scheduled then.
int TerminalMobElementBehavior::processServerDeparture (ElapsedTime p_time)
{
	//Sync person's family if need 
// 	if(m_pProcessor->getProcessorType() == HoldAreaProc)
// 	{
// 		if(m_pSimFlowSync != NULL && 
// 			(false == m_pSimFlowSync->CanIGoFurther(m_pPerson, *m_pProcessor->getID(), p_time)))
// 			return TRUE;
// 	}

	if( waitForResourceIfNeed( p_time ) )
		return TRUE;

	//add Resource serviceTime
	long _lServiceTime = 0;
	CResourcePool* _pool = NULL;
	_pool = m_pProcessor->getBestResourcePoolByType( m_pPerson->getType(), _lServiceTime );

	if (StickVisitorListForDestProcesOverload(p_time) == true)
	{
		return TRUE;
	}

	//stick m_pPerson person when dest processor exceed its capacity.
	if(StickForDestProcsOverload(p_time) ==true)
		return TRUE;

	//Sync person's family if need 
	//if(m_pProcessor->getProcessorType() == HoldAreaProc)
	{
		if(ProcessPassengerSync(p_time) == false)
			return TRUE;
// 		if(m_pSimFlowSync != NULL && 
// 			(false == m_pSimFlowSync->ProcessPassengerSync(m_pPerson, *m_pProcessor->getID(), p_time)))
	}

	m_bHaveServiceByResource	= false;
	m_bCanServeWhenClosing		= false;
	// because when call getNextProcessor function, it maybe change the p_time
	// so store the time for call makeAvailable.
	ElapsedTime tempTime = p_time;	
	if (m_bInBillboardLinkedproc)
	{
		m_pProcessor->writeLogEvent (m_pPerson, p_time, CompleteServiceBillboardLinkedProc);
		m_bInBillboardLinkedproc = false;
	}
	//else
	//{
	//		m_pProcessor->writeLogEvent (m_pPerson, p_time, CompleteService);	
	//}
	//m_pProcessor->makeAvailable (m_pPerson, p_time, false);
	// modified by tutu at 10/8/2003 .move m_pPerson after calling getNextProcessor ,
	// because before call getNextProcessor, m_pPerson person has not been added into next processor,
	// then when we call makeAvailable on current processor, if current processor has queue and next
	// processor has capacity limitied, errors will occur.
	int nState = m_pPerson->getState();
	m_pProcessor->getNextState (nState,m_pPerson);
	m_pPerson->setState(nState);

	if (m_pPerson->getState() == MoveAlongOutConstraint)
	{
		// must remove m_pPerson person , because after call makeAvailable ,
		// processor will notify all its source processor in conveyor system, if its source is pusher
		// then pusher will release a baggage ramdomly, if not remove person from m_pPerson processor, then
		// when baggage which is just be released from pusher getNextProcessor, maybe can not find next processor
		// because of the capacity problem. so should remove it before any makeAvailable
		m_pProcessor->removePerson (m_pPerson);
		if (_pool == NULL)
		{
			m_pProcessor->makeAvailable (m_pPerson, p_time, false);//has pool service and not make nexavailable
		}
		else if( _pool->getPoolType() == ConcurrentType)
		{
			m_pProcessor->makeAvailable (m_pPerson, p_time, false);
		}
		processOutConstraint (p_time);
		return TRUE;
	}
	//	// TRACE("\nid = %s\n",m_pProcessor->getID()->GetIDString() );
	Processor* pCurrentProc = m_pProcessor;	
	if( m_pProcessor->getProcessorType() == DepSourceProc )
	{

		int idxReseveredSink = ((DependentSource*)m_pProcessor)->GetReservedSink();
		((DependentSource*)m_pProcessor)->SetReservedSink(-1);
		if( idxReseveredSink >= 0 )
		{
			Processor* pNextProc = getTerminal()->procList->getProcessor( idxReseveredSink );
			m_nStranded = FALSE;
			m_pProcessor->removePerson (m_pPerson);

			m_nInGateDetailMode = NORMAL_MODE;

			if (m_pFlowList == NULL)
			{
				m_pPerson->regetAttatchedNopax();
				m_pFlowList = m_pProcessor->getNextDestinations ( m_pPerson->getType(), m_nInGateDetailMode );
				if ( m_pFlowList != NULL)
				{
					m_pFlowList->getDestinationGroup (RANDOM);
					ProcessPipe(pNextProc,p_time,NULL);					
				}
			}			

			m_pProcessor = pNextProc;
			m_pProcessor->addPerson (m_pPerson);
			m_pProcessor->AddRecycleFrequency(m_pPerson);
			((DependentSink*)m_pProcessor)->SetReserved( false );
			m_pFlowList = NULL;

			generateEvent (p_time + moveTime(),false);	
			if (_pool == NULL)
			{
				pCurrentProc->makeAvailable (m_pPerson, tempTime, false);//has pool service and not make nexavailable
			}
			else if( _pool->getPoolType() == ConcurrentType)
			{
				pCurrentProc->makeAvailable (m_pPerson, tempTime, false);
			}
			return TRUE;
		}
	}
	Processor *pPreProcessor = m_pProcessor;

	if( !getNextProcessor (p_time) )
	{
		if (_pool == NULL)
		{
			pCurrentProc->makeAvailable (m_pPerson, tempTime, false);//has pool service and not make nexavailable
		}
		else if( _pool->getPoolType() == ConcurrentType)
		{
			pCurrentProc->makeAvailable (m_pPerson, tempTime, false);
		}
		return FALSE;
	}

	if(m_pProcessor->getProcessorType()==ConveyorProc && 
		static_cast<Conveyor*>(m_pProcessor)->GetSubConveyorType()==SORTER)
	{
		//ensure that delay time = 0 from entry conveyor to sorter, and skip into sorter.
	
		m_pPerson->setState(ArriveAtServer);
		m_pProcessor->getNextLocation( m_pPerson);
		location =m_ptDestination;
		if (_pool == NULL)//has pool service and not make nexavailable
		{
			pCurrentProc->makeAvailable (m_pPerson, tempTime, false);
		}
		else if( _pool->getPoolType() == ConcurrentType)
		{
			pCurrentProc->makeAvailable (m_pPerson, tempTime, false);
		}
		m_pPerson->move( p_time,false );//force to enter sorter before sorter's stepit event.
		return TRUE;
	}

	generateEvent (p_time + moveTime(),false);	

	if (_pool == NULL)//has pool service and not make next available
	{ 
		pCurrentProc->makeAvailable (m_pPerson, tempTime, false);
	}
	else if( _pool->getPoolType() == ConcurrentType)
	{
		pCurrentProc->makeAvailable (m_pPerson, tempTime, false);
	}

	if(m_pPerson->getType().isTurround())
	{
		//if the passenger passed bridge processor, it changes to DEPARTURE mode
		if(pPreProcessor && pPreProcessor->getProcessorType() == BridgeConnectorProc)
		{
			SetTransferTypeState(TRANSFER_DEPARTURE);
		}
		if(pPreProcessor && pPreProcessor->getProcessorType() == GateProc)
		{
			//GateProcessor *pGateProc = (GateProcessor *)pPreProcessor;
			//if(pGateProc && pGateProc->getGateType() == ArrGate)//no matter arrival or depature gate
			{
				SetTransferTypeState(TRANSFER_DEPARTURE);
			}
		}
	}

	return TRUE;
}

void TerminalMobElementBehavior::writeLogEntry( ElapsedTime time, bool _bBackup, bool bOffset /*= true */ )
{
	if(m_pPerson)
		m_pPerson->writeLogEntry(time,_bBackup,bOffset);

}

void TerminalMobElementBehavior::generateEvent( ElapsedTime eventTime,bool bNoLog )
{
	if(m_pPerson)
		m_pPerson->generateEvent(eventTime,bNoLog);

}

Point& TerminalMobElementBehavior::getLocation()
{
	return location;
}
CString TerminalMobElementBehavior::ClacTimeString(const ElapsedTime& _curTime)
{
	char str[64];
	ElapsedTime tmptime(_curTime.asSeconds() % WholeDay);
	tmptime.printTime ( str );

	CStartDate sDate = getTerminal()->m_pSimParam->GetStartDate();
	bool bAbsDate;
	COleDateTime date;
	int nDtIdx;
	COleDateTime time;
	CString sDateTimeStr;
	sDate.GetDateTime( _curTime, bAbsDate, date, nDtIdx, time );
	if( bAbsDate )
	{
		sDateTimeStr = date.Format(_T("%Y-%m-%d"));
	}
	else
	{
		sDateTimeStr.Format("Day %d", nDtIdx + 1 );
	}
	sDateTimeStr += " ";
	sDateTimeStr += str;

	return sDateTimeStr;

}

void TerminalMobElementBehavior::flushLog( ElapsedTime p_time, bool bmissflight /*= false*/ )
{
	m_pPerson->flushLog(p_time,bmissflight);
}

void TerminalMobElementBehavior::processBridge( ElapsedTime p_time )
{
	PLACE_METHOD_TRACK_STRING();
	if (m_emBridgeState == ArrBridge)
	{

		if (m_pProcessor == getTerminal()->procList->getProcessor(START_PROCESSOR_INDEX))
		{
			setState(Birth);
			generateEvent(p_time,false);
			EnterFromOnboardModeToTerminalMode(p_time);
		}
		else
		{
			if (NULL == m_pProcessor || m_pProcessor->getProcessorType() != BridgeConnectorProc)
			{
				ASSERT(FALSE);
				return;
			}
			BridgeConnector* pBridgePro =(BridgeConnector*)m_pProcessor;

			m_pProcessor->removePerson(m_pPerson);
			Point startPs, endPs;
	
			pBridgePro->GetStartEndPoint(startPs,endPs);
			m_pPerson->setTerminalDestination(startPs);

			location = endPs;	
			m_pProcessor = getTerminal()->procList->getProcessor(START_PROCESSOR_INDEX);

			//cal person move from flight to gate
			ElapsedTime t;
			double dLxy = m_ptDestination.distance(getLocation());
			double dLz = 0.0;
			double dL = dLxy;
			dLz = fabs(m_ptDestination.getZ() - getLocation().getZ());
			Point pt(dLxy, dLz, 0.0);
			dL = pt.length();
			double time = dL;
			t = (float) (time / (double)m_pPerson->speed );
			m_pPerson->SetWalkOnBridge(TRUE);// = TRUE ;
			generateEvent (p_time + t,false);
		}
	}
	else if(m_emBridgeState == DepBridge)
	{
		if (m_pProcessor == NULL)
			return;

		BridgeConnector* pBridgePro = (BridgeConnector*)m_pProcessor;
		Point startPs, endPs;
		pBridgePro->GetStartEndPoint(startPs,endPs);
		setDestination(endPs);	
		m_pPerson->SetFollowerArrivalDoor(true);

		ElapsedTime t;
		double dLxy = m_ptDestination.distance(getLocation());
		double dLz = 0.0;
		double dL = dLxy;
		dLz = fabs(m_ptDestination.getZ() - getLocation().getZ());
		Point pt(dLxy, dLz, 0.0);
		dL = pt.length();
		double time = dL;
		t = (float) (time / (double)m_pPerson->speed );

		m_pPerson->SetWalkOnBridge(TRUE);//m_IsWalkOnBridge = TRUE ;
		m_pPerson->ChangeZ(startPs.z);

		setState(EntryOnboard);//move to onboard in next event, if no onboard selected, flush log
		generateEvent (p_time + t,false);
	}
}

void TerminalMobElementBehavior::setDestination( Point p)
{
	
	m_ptDestination = p;

	m_pPerson->m_pGroupInfo->SetFollowerDestination( location, m_ptDestination, Person::m_pRotation );
}
void TerminalMobElementBehavior::setState (short newState)
{
	m_pPerson->setState(newState);
}

int TerminalMobElementBehavior::getNextProcessor (ElapsedTime& p_time)
{	
	//DWORD dStepTime1 = 0L;
	//DWORD dStepTime2 = 0L;
	//DWORD dStepTime3 = 0L;
	//DWORD dStepTime4 = 0L;
	//DWORD dStepTime5 = 0L;
	///Test code
	//DWORD dStartTime = GetTickCount();
	///
	m_pDiagnoseInfo->clear();


	//if(state != WalkOnBridge && !HasBusForThisFlight(GetCurFlightIndex())&&m_pTerm->procList->getProcessor (START_PROCESSOR_INDEX) == m_pProcessor && m_pBridgePro)
	//{
	//	Point startPs, endPs;
	//	BridgeConnector* pBridgePro = (BridgeConnector*)m_pBridgePro;
	//	if(pBridgePro->GetRandomPoint(startPs,endPs))
	//	{
	//		setDestination(endPs);
	//		m_pProcessor = m_pBridgePro;
	//		m_pGroupInfo->setFollowerArrivalDoor(true);
	//		generateEvent(p_time,false);
	//		setState( WalkOnBridge );
	//		return FALSE;
	//	}
	//}

	PLACE_METHOD_TRACK_STRING();
	if( IsLateForDepFlight( p_time, m_pProcessor->GetTerminateTime() ) )
	{
		//flushLog (p_time,true);
		//cancel take on bus
		if(CAirsidePaxBusInSim* pBus = GetAirsideBus())
		{
			pBus->CancelTakeOnPassenger(m_pPerson,p_time);
		}

		if( m_pProcessor )
		{
			m_pProcessor->removePerson( m_pPerson );
// 			if(m_pProcessor->getProcessorType() == DepSinkProc && m_pProcessor->outConstraintLength() > 0)
// 			{
// 				//do nothing, 
// 				//for the depend sink has been called a person when the pre-person lefte the server
// 				//when the pre-person  come out the constraint and then find it has been late for the flight
// 				//the person just end, and does not need to call the process release next perosn
// 			}
// 			else
// 				m_pProcessor->makeAvailable( m_pPerson, p_time, false );

			if (m_pProcessor->getProcessorType() == ConveyorProc)
			{
				Conveyor* pConveyor = ( Conveyor*)( m_pProcessor );
				pConveyor->RemoveHeadPerson(p_time);
			}
		}
		flushLog (p_time,true);
		return FALSE;
	}

	//dStepTime1 = GetTickCount();
	// must re-get the NonPaxCount before call getNextDestinations
	// because the regetAttatchedNopax re-construct the m_pPerson->getType().
	m_pPerson->regetAttatchedNopax();
	m_pFlowList = m_pProcessor->getNextDestinations ( m_pPerson->getType(), m_nInGateDetailMode );

	if( getEngine()->GetFireOccurFlag() && 
		m_timeFireEvacuation <= p_time && 
		!m_pPersonOnStation )
	{
		flushLog (p_time);
		if( m_pProcessor )
		{
			m_pProcessor->removePerson( m_pPerson );
		}

		if (m_pProcessor->getProcessorType() == ConveyorProc)
		{
			Conveyor* pConveyor = ( Conveyor*)( m_pProcessor );
			pConveyor->RemoveHeadPerson(p_time);
		}
		return FALSE;
	}
	else if( getEngine()->GetFireOccurFlag() && 
		m_timeFireEvacuation <= p_time && 
		!m_bHasProcessedFireEvacuation )
	{
		ProcessFireEvacuation( p_time );
		return TRUE;
	}


	//dStepTime2 = GetTickCount();


	Processor* nextProc = NULL;
	CString strTmp;

	// for throw exception
	CString szMobType = getPersonErrorMsg();
	CString strProcNameTmp = m_pProcessor ? m_pProcessor->getID()->GetIDString() : "";
	// 	

	bool bHoldAutoPipe = false;
	PLACE_TRACK_STRING("2010223-11:17:26");
	//if( m_pSubFlow == NULL )	// no PROCESS flow
	{
		// person back from wait area
		if( m_bWaitInLinkedArea && m_pBackToProcFromWaitArea )
		{
			nextProc = m_pBackToProcFromWaitArea;
			m_pFlowList = NULL;
			m_bWaitInLinkedArea = false;
			m_pBackToProcFromWaitArea = NULL;

			bHoldAutoPipe = true;
		}
		else if( m_pDirectGoToNextProc !=	NULL )//person should go to juction proc
		{
			nextProc = m_pDirectGoToNextProc;
			m_pDirectGoToNextProc = NULL;
		}
		else
		{
			// Persons m_nStranded m_in hold areas still have previous Processor's
			// Transition Row. Non m_nStranded pax must get destination list
			if (m_pFlowList == NULL)
			{
				// Get the pax flow entry
				m_pFlowList = m_pProcessor->getNextDestinations ( m_pPerson->getType(), m_nInGateDetailMode );
			}

			// give the special jump for FROM_GATE_PROCESSOR / TO_GATE_PROCESSOR
			if( m_pFlowList == NULL && m_nInGateDetailMode == FROM_GATE_MODE )
			{
				m_nInGateDetailMode = NORMAL_MODE;
				m_pFlowList = getTerminal()->procList->getProcessor(FROM_GATE_PROCESS_INDEX)->getNextDestinations ( m_pPerson->getType(), m_nInGateDetailMode );
			}

			if( m_pFlowList == NULL && m_nInGateDetailMode == TO_GATE_MODE )
			{
				m_nInGateDetailMode = NORMAL_MODE;
				m_pFlowList = getTerminal()->procList->getProcessor(TO_GATE_PROCESS_INDEX)->getNextDestinations ( m_pPerson->getType(), m_nInGateDetailMode );
			}

			// handle case where Processor has no ProcessorDistribution
			if (m_pFlowList == NULL)
			{
				//throw new NoFlowError (m_pPerson);
				m_pPerson->kill(p_time);
				throw new ARCFlowUndefinedError( szMobType,strProcNameTmp,"",ClacTimeString(p_time));
			}
			m_pPreFlowList = m_pFlowList;

			nextProc = selectProcessor( p_time );

			//dStepTime3 = GetTickCount();
		}
	}
	///////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////
	//	divideGroupIfNeed( nextProc, p_time );
	HandleLoaderHoldLogic( &nextProc, p_time );

	PLACE_TRACK_STRING("2010223-11:17:29");
	if( HandleSorterLogic(&nextProc, p_time )==false ) return FALSE;

	//dStepTime4 = GetTickCount();
	// if nextProc is the FROM_GATE / TO_GATE do the special process.
	bool bUsePrevPipeList = false;
	CFlowItemEx flowItem;
	if( nextProc == getTerminal()->procList->getProcessor (FROM_GATE_PROCESS_INDEX) )
	{
		m_nInGateDetailMode = FROM_GATE_MODE;

		std::vector<CFlowItemEx>& vFlowItems = ( (CProcessorDistributionWithPipe*)m_pFlowList )->GetPipeVector();
		if( !vFlowItems.empty() )
		{
			flowItem = vFlowItems[m_pFlowList->GetCurGroupIndex()];
			bUsePrevPipeList = true;
		}				

		m_pFlowList = nextProc->getNextDestinations( m_pPerson->getType(), m_nInGateDetailMode );
		m_pPreFlowList = m_pFlowList;
		if (m_pFlowList == NULL)
		{
			//throw new NoFlowError (m_pPerson);
			m_pPerson->kill(p_time);

			throw new ARCFlowUndefinedError( szMobType,strProcNameTmp,"IN FROM GATE", ClacTimeString(p_time));
		}

		++m_iNumberOfPassedGateProc;
		//ASSERT(	m_nGate >= 0 );
		if( m_pPerson->m_nGate < 0 )
			throw new StringError("Not all flight have gate assignment info!");
		nextProc =  getTerminal()->procList->getProcessor( m_pPerson->m_nGate );
		//nextProc = selectProcessor();
	}

	else if( nextProc == getTerminal()->procList->getProcessor (TO_GATE_PROCESS_INDEX) )
	{
		m_nInGateDetailMode = TO_GATE_MODE;

		std::vector<CFlowItemEx>& vFlowItems = ( (CProcessorDistributionWithPipe*)m_pFlowList )->GetPipeVector();
		if( !vFlowItems.empty() )
		{
			flowItem = vFlowItems[m_pFlowList->GetCurGroupIndex()];
			bUsePrevPipeList = true;
		}				

		m_pFlowList = nextProc->getNextDestinations( m_pPerson->getType(), m_nInGateDetailMode );
		m_pPreFlowList = m_pFlowList;
		if (m_pFlowList == NULL)
		{
			//throw new NoFlowError (m_pPerson);
			m_pPerson->kill(p_time);

			throw new ARCFlowUndefinedError( szMobType,strProcNameTmp,"IN TO GATE", ClacTimeString(p_time));
		}
		++m_iNumberOfPassedGateProc;

		if( m_pPerson->m_nGate < 0 )
			throw new StringError("Not all flight have gate assignment info!");
		nextProc = getTerminal()->procList->getProcessor( m_pPerson->m_nGate );
		//nextProc = selectProcessor();
	}
	else if( nextProc == getTerminal()->procList->getProcessor ( BAGGAGE_DEVICE_PROCEOR_INDEX ) )
	{
		std::vector<CFlowItemEx>& vFlowItems = ( (CProcessorDistributionWithPipe*)m_pFlowList )->GetPipeVector();
		if( !vFlowItems.empty() )
		{
			flowItem = vFlowItems[m_pFlowList->GetCurGroupIndex()];
			bUsePrevPipeList = true;
		}		


		m_nInGateDetailMode = NORMAL_MODE;
		ProcessorEntry* pDestEntry = nextProc->GetNormalDestDistribution();
		m_pFlowList = nextProc->getNextDestinations ( m_pPerson->getType(), m_nInGateDetailMode );
		m_pPreFlowList = m_pFlowList;
		if (m_pFlowList == NULL)// to get its owner's flow
		{
			CMobileElemConstraint tempType(getTerminal());
			tempType = m_pPerson->getType();
			tempType.SetTypeIndex( 0 );
			tempType.SetMobileElementType(enum_MobileElementType_PAX);
			m_pFlowList = nextProc->getNextDestinations ( tempType, m_nInGateDetailMode );
			m_pPreFlowList = m_pFlowList;
			if( m_pFlowList == NULL )
			{
				//throw new NoFlowError (m_pPerson);
				kill(p_time);

				throw new ARCFlowUndefinedError( szMobType,strProcNameTmp,"IN BAGGAGE DEVICE", ClacTimeString(p_time));
			}
			++m_iNumberOfPassedReclaimProc;
		}
		const ProcessorID* pDestID = NULL;
		if (m_pPerson->m_logEntry.isArrival())
		{
			int nIndex = m_pPerson->m_logEntry.getArrFlight();
			Flight* pFlight = getEngine()->m_simFlightSchedule.getItem(nIndex);
			pDestID = &(pFlight->getBaggageDevice());
		}

		//		ProcessorDistribution* pDistribution = (ProcessorDistribution*)m_pTerm->bagData->getFlightAssignToBaggageDeviceDB()->lookup( m_pPerson->m_pPerson->getType() ) ;

		if( pDestID == NULL )
		{
			//throw new NoValidFlowError (m_pPerson);
			m_pPerson->kill(p_time);

			throw new ARCNoBaggageDeviceAssignError( szMobType ,"", ClacTimeString(p_time));
		}

		//pdis
		//	const ProcessorID* pDestID = pDistribution->getDestinationGroup( RANDOM );
		//// TRACE("\n%s\n",pDestID->GetIDString());
		nextProc = getTerminal()->procList->getProcessor( pDestID->GetIDString() );
		if( !nextProc )
		{
			//throw new NoValidFlowError (m_pPerson);
			m_pPerson->kill(p_time);

			throw new ARCProcessNameInvalideError( pDestID->GetIDString(),"", ClacTimeString(p_time) );
		}

		//nextProc = selectProcessor();
		//// TRACE("\nid = %s\n",nextProc->getID()->GetIDString() );
		nextProc->SetNormalDestDistribution( pDestEntry );
	}

	///////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////
	// Process pipe.
	PLACE_TRACK_STRING("2010223-11:17:33");
	if( nextProc )
		strTmp = nextProc->getID()->GetIDString();



	if(m_pPerson->m_logEntry.isArriving() \
		&&getTerminal()->procList->getProcessor (START_PROCESSOR_INDEX) == m_pProcessor \
	&& nextProc && nextProc->getProcessorType() == GateProc && m_emBridgeState == NonState)
	{
		Point startPs, endPs;
		if (m_pPerson->m_logEntry.isArrival())
		{
			m_pPerson->getTerminalBehavior()->SetTransferTypeState(TRANSFER_ARRIVAL);
		}

		BridgeConnector* pBridgePro = (BridgeConnector*)GetBridgeConnector(nextProc);
		if (pBridgePro)
		{
			int m_nBridgeIndex = pBridgePro->GetRandomPoint(startPs,endPs,m_pPerson);
			if(m_nBridgeIndex != -1)
			{
				m_pPerson->SetBridgeState(ArrBridge);//m_emBridgeState = ArrBridge;
				m_pPerson->setTerminalDestination(endPs);
				m_pProcessor = pBridgePro;
				m_pProcessor->addPerson(m_pPerson);
				m_pPerson->SetFollowerArrivalDoor(true);
				generateEvent(p_time,false);
				setState( WalkOnBridge );
				return FALSE;
			}
		}
	}


	//------------------------------Added by frank 05-11-21
	//process pipe in process



	if( m_pFlowList != NULL)	
	{
		if( bUsePrevPipeList )
			ProcessPipe( nextProc, p_time, &flowItem );
		else
			ProcessPipe( nextProc, p_time, NULL );
	}

	//passenger from link hold area should do auto pipe
	if(true == bHoldAutoPipe)
	{
		ProcessHoldingAreaPipe( nextProc, p_time );
	}

	//------------------------------end added

	// handle the station
	if( nextProc && nextProc->getProcessorType() == IntegratedStationProc )
	{
		assert( getTerminal() );		
		Processor* pCurrentProc = m_pProcessor ;
		m_pProcessor = nextProc;
		m_pFlowList = nextProc->getNextStation( m_pPerson->getType() );
		m_pPersonOnStation = new CPersonOnStation( m_pPerson, nextProc->getIndex(), getTerminal() ,m_pFlowList );
		int procIndex = GetDestStation( p_time, nextProc );

		m_pProcessor = pCurrentProc;
		if( procIndex < 0 )
		{
			//throw new NoValidFlowError (m_pPerson);
			ReleaseResource(p_time);
			kill(p_time);

			throw new ARCDestProcessorUnavailableError( szMobType, strProcNameTmp,"IN RAILWAY SYSTEM",ClacTimeString(p_time) );

			//			throw new ARCDestProcessorUnavailableError( szMobType, strProcNameTmp,"IN RAILWAY SYSTEM",p_time.printTime() );
		}
		m_pPersonOnStation->SetDestProcIndex( procIndex );
		//		m_pPersonOnStation->Process( p_time );
	}
	else if( nextProc && nextProc->getProcessorType() == Elevator )
	{
		if( m_pProcessor )
		{
			m_iSourceFloor = (int)(  getLocation().getZ() / SCALE_FACTOR );
			m_nInGateDetailMode = NORMAL_MODE;
			CString strProcName = nextProc->getID()->GetIDString();
			ProcessorDistribution*  pFlowList = NULL;
			pFlowList = nextProc->getNextDestinations ( m_pPerson->getType(), m_nInGateDetailMode );
			Processor *pNextProcAfterElevator = TryToGetElevatorDestProc(p_time, nextProc);

			if( pNextProcAfterElevator )
			{
				// TRACE("\n PROCID: %s\n",m_pTerm->procList->getProcessor( groupIdx.start )->getID()->GetIDString());
				m_iDestFloor = pNextProcAfterElevator->getFloor() / (int)SCALE_FACTOR;
				if( m_iSourceFloor == m_iDestFloor )
				{
					ReleaseResource(p_time);
					kill(p_time);

					throw new ARCFlowError( szMobType, strProcName, "person's destination floor is equal with current floor when use elevator " + strProcName, ClacTimeString(p_time) );
				}
			}
			else 
			{
                ReleaseResource(p_time);
                kill(p_time);
                m_pDiagnoseInfo->flushDiagnoseInfo();
				ARCDestProcessorUnavailableError* pException = new ARCDestProcessorUnavailableError( szMobType, strProcName, "IN ELEVATOR SYSTEM", ClacTimeString(p_time));
                pException->setDiagType( PAXDEST_DIAGNOS );
                pException->setDiagData( m_pPerson->id );
                throw pException;
				//						throw new ARCDestProcessorUnavailableError( szMobType, strProcName, "IN ELEVATOR SYSTEM", p_time.printTime() );
			}
		}
	}

	PLACE_TRACK_STRING("2010223-11:17:37");
	if( nextProc && nextProc->getProcessorType() == StairProc ) //StairProc need Bidirectional
	{
		//// TRACE(m_pProcessor->getID()->GetIDString());
		m_iSourceFloor = (int)(getLocation().getZ() / SCALE_FACTOR); //m_pProcessor->getFloor() / SCALE_FACTOR ;		
		int iFloorPtFirst = (int) (nextProc->getServicePoint( 0 ).getZ() / SCALE_FACTOR );
		int iFloorPtLast = (int) (nextProc->getServicePoint( nextProc->serviceLocationLength()-1).getZ() / SCALE_FACTOR) ;

		if( m_iSourceFloor == iFloorPtFirst )
			m_iDestFloor = iFloorPtLast;
		else
			m_iDestFloor = iFloorPtFirst;
	}

	if( nextProc && nextProc->getProcessorType() != Elevator && 
		nextProc->getProcessorType() != EscalatorProc &&
		nextProc->getProcessorType() != StairProc &&
		m_pProcessor->getProcessorType() == StairProc ) 
	{
		//// TRACE(m_pProcessor->getID()->GetIDString());
		CString strEnd("END");		
		m_iDestFloor = nextProc->getFloor() / (int)SCALE_FACTOR ;

		if ( (m_iDestFloor != getPoint().getZ() / SCALE_FACTOR &&  
			strEnd != nextProc->getID()->GetIDString())
			&&(nextProc->getProcessorType()==StairProc&&
			(nextProc->getServicePoint( nextProc->serviceLocationLength()-1).getZ() / SCALE_FACTOR !=getPoint().getZ() / SCALE_FACTOR)))  //not at the same floor
		{
			CString szMobType = getPersonErrorMsg();			
			CString strProcName = m_pProcessor->getID()->GetIDString();			
			ReleaseResource(p_time);
			kill(p_time);

			throw new ARCDestProcessorUnavailableError( szMobType, strProcName ,"IN STAIRPROC SYSTEM", ClacTimeString(p_time) );						


			//			throw new ARCDestProcessorUnavailableError( szMobType, strProcName ,"IN STAIRPROC SYSTEM", p_time.printTime() );						
		}		
	}

	if( nextProc && nextProc->getProcessorType() == EscalatorProc ) //EscalatorProc have only one direction
	{
		//// TRACE(m_pProcessor->getID()->GetIDString());
		m_iSourceFloor = (int) ( getLocation().getZ() / SCALE_FACTOR );		
		m_iDestFloor = (int)(nextProc->getServicePoint( nextProc->serviceLocationLength()-1).getZ() / SCALE_FACTOR );


		if ( nextProc->getFloor() != getPoint().getZ() )  //not at the same floor
		{
			CString strProcName = nextProc->getID()->GetIDString();			
			ReleaseResource(p_time);
			kill(p_time);
			CString errmes;
			errmes.Format(_T("The floor of pax's next processor(%s) is not equal with the exit floor of the elevator"),strProcName);
			throw new ARCDestProcessorUnavailableError( szMobType, strProcName ,errmes, ClacTimeString(p_time) );	
			//			throw new ARCDestProcessorUnavailableError( szMobType, strProcName ,"IN ESCALATORPROC SYSTEM", p_time.printTime() );			
		} 		
	}	

	if (nextProc && nextProc->getProcessorType() != Elevator &&
		nextProc->getProcessorType() != EscalatorProc && 
		nextProc->getProcessorType() != StairProc && 
		m_pProcessor->getProcessorType() == EscalatorProc  )
	{
		//// TRACE(m_pProcessor->getID()->GetIDString());
		CString strEnd("END");
		m_iDestFloor = nextProc->getFloor() / (int)SCALE_FACTOR ;
		if ( m_iDestFloor != (int)(getPoint().getZ() / SCALE_FACTOR) &&
			nextProc->getID()->GetIDString() != strEnd)  //not at the same floor
		{
			CString szMobType = getPersonErrorMsg();			
			CString strProcName = m_pProcessor->getID()->GetIDString();			
			ReleaseResource(p_time);
			kill(p_time);
			CString errmes;
			errmes.Format(_T("The floor of pax's next processor(%s) is not equal with the exit floor of the elevator"),strProcName);
			throw new ARCDestProcessorUnavailableError( szMobType, strProcName ,errmes, ClacTimeString(p_time) );						
		}		
	}
	// handle case where no destination Processor can serve Person
	if (nextProc == NULL)
	{
		ReleaseResource(p_time);
		m_pDiagnoseInfo->flushDiagnoseInfo();


		ARCDestProcessorUnavailableError* pException = new ARCDestProcessorUnavailableError( szMobType, strProcNameTmp,"", ClacTimeString(p_time) );

		//		ARCDestProcessorUnavailableError* pException = new ARCDestProcessorUnavailableError( szMobType, strProcNameTmp,"", p_time.printTime() );
		pException->setDiagType( PAXDEST_DIAGNOS );
		pException->setDiagData( m_pPerson->id );

		/*m_pPerson->*/kill(p_time);

		throw pException;
	}
	else
	{
		m_nStranded = FALSE;
	}

	if( m_pProcessor &&
		( m_pProcessor->getProcessorType() == Elevator || m_pProcessor->getProcessorType() == StairProc || m_pProcessor->getProcessorType() == EscalatorProc ) && 
		nextProc->getProcessorType() != Elevator && 
		nextProc->getProcessorType() != StairProc && 
		nextProc->getProcessorType() != EscalatorProc  )
	{
		// when person leave elevator and have got next processor
		m_iDestFloor = -1;//let dest floor 
	}
	if (m_pPerson->getState() != Birth)
		m_pProcessor->removePerson (m_pPerson);

	if (m_pProcessor->getProcessorType() == ConveyorProc)
	{
		Conveyor* pConveyor = ( Conveyor*)( m_pProcessor );
		pConveyor->RemoveHeadPerson(p_time);
	}



	assert( getTerminal() );
	// handle case where Person exits Terminal (arrive at End Processor)
	if (nextProc == getTerminal()->procList->getProcessor (END_PROCESSOR_INDEX))
	{
		m_pPerson->writeTerminalEndLogEntry(p_time);
		if (m_pPerson->m_logEntry.isDeparting())
		{
			m_pPerson->getTerminalBehavior()->SetTransferTypeState(TRANSFER_DEPARTURE);
		}


		if(m_pPerson->m_logEntry.isArrival()) //for arrival pax it will go to landside
		{
			setLastTerminalProc(m_pProcessor);
			setState(EntryLandside);
			generateEvent(p_time, false);
			return TRUE;
		}
		else // for other pax go to bridge or airside
		{
			BridgeConnector* pProce = GetBridgeConnector(m_pProcessor);		
			if(!pProce )
			{
				setState( EntryAirside );
				generateEvent (p_time,false);
				return FALSE;
			}
			else
			{
				Point startPs, endPs;
				BridgeConnector* pBridgePro = (BridgeConnector*)pProce;
				int m_nBridgeIndex = pBridgePro->GetRandomPoint(startPs,endPs,m_pPerson);
				if(m_nBridgeIndex != -1 &&  m_pPerson->m_logEntry.isDeparting())
				{
					m_pPerson->SetBridgeState(DepBridge);//m_emBridgeState = DepBridge;
					m_pPerson->setTerminalDestination(startPs);
					m_pProcessor = pProce;
					EnterFromTerminalModeToOtherMode();
					setState( WalkOnBridge );
					m_pProcessor->addPerson(m_pPerson);
					generateEvent(p_time + moveTime(),false);
					return TRUE;
				}
				else
				{
					setState( EntryAirside );
					generateEvent (p_time,false);
					return FALSE;
				}
			}
		}
		
	}

	if(nextProc && nextProc->getProcessorType()==BridgeConnectorProc)
	{
		m_pPerson->SetBridgeState(NonState);
		if(m_pPerson->m_logEntry.isArriving() && m_pProcessor ==  getTerminal()->procList->getProcessor (START_PROCESSOR_INDEX) )
		{
			m_pPerson->SetBridgeState(ArrBridge);
		}
		else if(m_pPerson->m_logEntry.isDeparting())
		{
			m_pPerson->SetBridgeState(DepBridge);
		}
		else
		{
			//arrival flow, there has processor before bridge
			ASSERT(0);
		}

	}

	m_pProcessor = nextProc;
	m_pProcessor->addPerson (m_pPerson);
	m_pProcessor->AddRecycleFrequency(m_pPerson);


	if (m_pProcessor->getProcessorType() == ConveyorProc || m_bNeedLoaderToHoldArea == true)
	{
		if (m_bNeedLoaderToHoldArea)
		{	
			if(!m_pPerson->m_pGroupInfo->IsFollower())
			{
				// get a link holding area( rand )
				HoldingArea* _linkHold = (HoldingArea*)( getTerminal()->procList->GetWaitArea(m_pLoaderProc->getID() ) );

				((CGroupLeaderInfo *)m_pPerson->m_pGroupInfo)->ProcessLoaderProc(p_time,m_pLoaderProc ,m_pProcessor );
			}
		}
		else
		{
			Conveyor* pConveyor = ( Conveyor*)( m_pProcessor );
			// if the processor is loader and it is full
			if( pConveyor->GetSubConveyorType() == LOADER )
			{
				if(!m_pPerson->m_pGroupInfo->IsFollower())
				{
					// get a link holding area( rand )
					HoldingArea* _linkHold = pConveyor->GetRandomHoldingArea();//(HoldingArea*)( m_pTerm->procList->GetWaitArea( m_pProcessor->getID() ) );
					int nEmptyCount = pConveyor->GetEmptySlotCount();

					if( _linkHold == NULL && m_pPerson->m_pGroupInfo->GetAdminGroupSize() > nEmptyCount)	// can not get link hold area
					{
						CString szMobType = getPersonErrorMsg();
						CString strProcNameTmp = m_pProcessor ? m_pProcessor->getID()->GetIDString() : "";

						ReleaseResource(p_time);
						kill(p_time);


						CString strLoader;
						strLoader.Format(_T("This Loader( %s ) is out of capacity, suggest define a holding area for this Loader or increase capacity of this Loader"),strProcNameTmp);
						throw new ARCDestProcessorUnavailableError( szMobType, strProcNameTmp,strLoader, ClacTimeString(p_time) );

					}
					((CGroupLeaderInfo *)m_pPerson->m_pGroupInfo)->ProcessLoaderProc(p_time,m_pProcessor ,_linkHold );
				}
			}
		}

		m_bNeedLoaderToHoldArea = false;
		m_pLoaderProc = NULL;
	}

	// m_nStranded people still need their Transition Row
	if (!m_nStranded)
		m_pFlowList = NULL;	// base on test so far, m_pPerson is always true.


	//// TRACE(nextProc->getID()->GetIDString().GetBuffer(0));
	// fix required (not all hold areas are m_nGate m_nWaiting areas)
	if (nextProc->getProcessorType() == HoldAreaProc)
	{
		// temporary measure to have hold areas with service times not
		// strand passengers (concession)
		m_pPerson->regetAttatchedNopax();
		if (nextProc->getServiceTime (m_pPerson->getType(), p_time ))
		{
			m_nStranded = FALSE;
			return TRUE;
		}

		//m_nStranded = TRUE;
		// check to see if missed last boarding call
		int flightIndex = m_pPerson->m_logEntry.getDepFlight();
		if( flightIndex >= 0 )
		{
			Flight *pFlight = getEngine()->m_simFlightSchedule.getItem (flightIndex);
			int iGateIdx = pFlight->getDepGateIndex();
			if( iGateIdx >= 0 && iGateIdx < INT_MAX  )
			{
				std::vector<int> vHoldingArea;
				getTerminal()->procList->GetHoldingAraOfGate( iGateIdx, vHoldingArea );
				int iWaitAreaCount = vHoldingArea.size();
				for( int i=0 ; i<iWaitAreaCount; i++ )
				{
					HoldingArea* pProc =(HoldingArea*) getTerminal()->procList->getProcessor( vHoldingArea[ i ] );
					if( pProc == nextProc && m_pPerson->getLogEntry().GetMobileType() != 1 )
					{
						m_pPerson->SetTypeInputTeminal( getTerminal() );
						if( ( m_pPerson->getType().isDeparture() || m_pPerson->getType().isTurround() ) && 
							!pProc->IsCanTakeOnFlight((Passenger*)m_pPerson))
							m_nStranded = TRUE;
						break;
					}
				}

			}
		}
	}

	if(nextProc && nextProc->getProcessorType()== BridgeConnectorProc)
	{
		CString strNextProc = nextProc->getIDName();
		Point startPs, endPs;
		BridgeConnector* pBridgePro = (BridgeConnector*)nextProc;
		int nFltId = m_pPerson->GetCurFlightIndex();
		if( !pBridgePro->IsBridgeConnectToFlight(nFltId) )
		{
			char strTmp[128];
			m_pTerm->flightSchedule->getFlightID(strTmp,nFltId,'D');//m_pPerson->getEngine()->GetAirsideSimulation()->GetAirsideFlight(m_pPerson->GetCurFlightIndex());
			
			CString errmes;
			errmes.Format(_T("The %s is not Connected to Flight %s"),strNextProc, strTmp );
			throw new ARCDestProcessorUnavailableError( szMobType, strProcNameTmp ,errmes, ClacTimeString(p_time) );	
			return FALSE;
		}
		/*int m_nBridgeIndex = pBridgePro->GetRandomPoint(startPs,endPs,m_pPerson);
		if(m_nBridgeIndex != -1 &&  m_pPerson->m_logEntry.isDeparting())
		{
		m_emBridgeState = DepBridge;
		m_pPerson->setTerminalDestination(startPs);
		EnterFromTerminalModeToOtherMode();
		setState( WalkOnBridge );
		generateEvent(p_time + moveTime(),false);
		return TRUE;
		}*/
	}


	//if Baggage device has capacity, at m_pPerson time, need let it to know someone(checked bag) will arrival 
	if(nextProc->getProcessorType() == BaggageProc && m_pPerson->getType().GetTypeIndex() == 2)/*checked bag*/
	{
		( (BaggageProcessor*)nextProc )->addBaggageIntoApproachList( m_pPerson );
	}
	return TRUE;
}
// Exception: ARCNoBaggageDeviceAssignError
// Exception: ARCProcessNameInvalideError
// Exception: ARCDestProcessorUnavailableError
// m_pPerson only called in no PROCESS
Processor *TerminalMobElementBehavior::selectProcessor ( ElapsedTime _curTime, bool _bStationOnly , bool _bExcludeFlowBeltProc/* = false */, bool bTryExcludeElevatorProc /*= false*/)
{

	//DWORD dStepTime1 = 0L;
	//DWORD dStepTime2 = 0L;
	//DWORD dStepTime3 = 0L;
	//DWORD dStepTime4 = 0L;
	//DWORD dStepTime5 = 0L;
	/////Test code
	//DWORD dStartTime = GetTickCount();

	const ProcessorID *nextGroup = NULL;

	ProcessorArray getProcs;
	ProcessorArray destProcs;
	int procCount = 0;
	std::vector<ProcessorID> vRootProc;

	// for throw exception
	CString szMobType = getPersonErrorMsg();
	CString strProcNameTmp = m_pProcessor ? m_pProcessor->getID()->GetIDString() : "";


	///////////////////////////////////////////////////////
	// 

	// try randomly selected group of Processors
	nextGroup = m_pFlowList->getDestinationGroup (RANDOM);
	ASSERT( getTerminal() );

	//dStepTime1 = GetTickCount();

	if( IfBaggageExistInDist( m_pFlowList ) )//dest flow have baggage processor ?
	{
		// Process the bypass logic, when the current processor is the baggage processor.
		// bypass linkage first, then flow bypass
		//	ProcessorDistribution* pDistribution = (ProcessorDistribution*)m_pTerm->bagData->getFlightAssignToBaggageDeviceDB()->lookup(getArrivalType());//lookup( m_pPerson->m_pPerson->getType() ) ;
		int nIndex = m_pPerson->getLogEntry().getArrFlight();
		Flight* pFlight = getEngine()->m_simFlightSchedule.getItem(nIndex);
		const ProcessorID* pBaggageID = &(pFlight->getBaggageDevice());
		if( pBaggageID == NULL )// any pax who don not have assigned baggage device, such as dep pax, visitor....
		{
			//throw new NoValidFlowError (m_pPerson);
			kill(_curTime);
			throw new ARCNoBaggageDeviceAssignError( szMobType,"", ClacTimeString(_curTime) );
		}

		// get single  baggage proc
		//		const ProcessorID* pBaggageID = pDistribution->getDestinationGroup( RANDOM ) ;
		Processor* pProc = getTerminal()->procList->getProcessor( pBaggageID->GetIDString() );

		if( m_pFlowList->IfProcessorExistInDist( pBaggageID ) )// dest flow have assigned baggage processor
		{
			if( m_pPerson->getType().GetTypeIndex() == 0 )// is pax
			{
				if( ((Passenger*)m_pPerson)->GetBagsCount() > 0 )// have checked bags
				{
					ASSERT( pProc->getProcessorType() == BaggageProc );
					return pProc;
				}
				else // no bags, go to bypass proc
				{		
					ProcessorDistribution* pByPass = ((BaggageProcessor *)pProc)->getBypassProcessors();
					if (pByPass)//get linkage bypass proc
					{
						const ProcessorID* pNextID = pByPass->getDestinationGroup( RANDOM );
						ASSERT( pNextID );
						GroupIndex byPassGroup = getTerminal()->procList->getGroupIndex( *pNextID );
						ASSERT( byPassGroup.start >=0 );
						int iProcIdx = byPassGroup.start + random ( byPassGroup.end - byPassGroup.start + 1 );				
						return getTerminal()->procList->getProcessor( iProcIdx );
					}// if linkage do not define bypass proc, then go to bypass which defined on pax flow.
				}
			}
			else if( m_pPerson->getType().GetTypeIndex() == 2 )//check bag
			{
				return pProc;
			}
		}
	}


	Processor* _pProc = NULL;
	for(std::vector<ConvergePair*>::iterator it= m_pPerson->m_vpConvergePair.begin();
		it!=m_pPerson->m_vpConvergePair.end();
		it++)
	{
		if((*it)->AmNowConvergeProcessor(m_pPerson->getID(),&_pProc,m_pPerson,_bStationOnly,_curTime))//true means want to convert,waiting
		{
			return _pProc;
		}
	}

	//dStepTime2 = GetTickCount();

	//handle wether nedd to visit billboard linked processor
	_pProc = NULL;
	if (FindBillboardLinkedProcessorFromDestination(&_pProc,_curTime,_bStationOnly))
	{
		ASSERT(_pProc);
		m_bInBillboardLinkedproc = true;
		return _pProc;
	}

	// For the possible destination, select one processor
	//dStepTime3 = GetTickCount();

	nextGroup = m_pFlowList->getDestinationGroup (RANDOM);

	//here is temporarily solution for bridge processor
	//for transfer passenger, arrival and departure passenger are all have bridge processor
	//that leads to a cycle
	//but for arrival passenger, if have another choice to pick normal processor, except END
	//he should not pick END processor
	//similar to the Departure passengers, the departure passenger should=ld select END processor
	//the follow logic should be refactored in near future

	/*********/
	
	if(m_pProcessor && m_pProcessor->getProcessorType() == BridgeConnectorProc)
	{ 
		if(m_pPerson->getType().isTurround())//special for Trunaround passenger, those passengers might use the bridge processors twice
		{
			if(m_pFlowList )
			{
				BridgeConnector *pBridgeConnector = (BridgeConnector *)m_pProcessor;
				if(pBridgeConnector)
				{
					if(m_TransferState == TRANSFER_ARRIVAL)
					{
						ASSERT(m_TransferState == TRANSFER_ARRIVAL);
						while (nextGroup != NULL)//find not END processor
						{
							GroupIndex group = getTerminal()->GetProcessorList()->getGroupIndex (*nextGroup);
							if (group.start == END_PROCESSOR_INDEX && group.start == group.end)
							{
								nextGroup = m_pFlowList->getDestinationGroup (SEQUENTIAL);
							}
							else
								break;
						}
					}
					else if(m_TransferState == TRANSFER_DEPARTURE)
					{
						//while (nextGroup != NULL) //Find END processor
						//{
						//	GroupIndex group = getTerminal()->GetProcessorList()->getGroupIndex (*nextGroup);
						//	if (group.start != END_PROCESSOR_INDEX )
						//	{
						//		nextGroup = m_pFlowList->getDestinationGroup (SEQUENTIAL);
						//	}
						//	else
						//		break;
						//}
						//this passenger end here, there has not allow processor after departure bridge processor
						Processor * nextProc = getTerminal()->procList->getProcessor (END_PROCESSOR_INDEX);
						return nextProc;

					}
					else 
					{
						ASSERT(FALSE);
					}
				}
			}
		}

	}


	/**********/



	const ProcessorID* pToGate = getTerminal()->procList->getProcessor(TO_GATE_PROCESS_INDEX)->getID();
	const ProcessorID* pFromGate = getTerminal()->procList->getProcessor(FROM_GATE_PROCESS_INDEX)->getID();
	const ProcessorID* pBaggage = getTerminal()->procList->getProcessor(BAGGAGE_DEVICE_PROCEOR_INDEX)->getID();

	// no proc can be selected because of the roster
	bool bRosterReason = false;

	//CString strRandomProcGroup = nextGroup->GetIDString();
	do 
	{
		if(nextGroup!=NULL)
		{
			std::vector<const CRule*> vFlowRules;
			const CFlowItemEx* pFlowItem = static_cast<CProcessorDistributionWithPipe*>(m_pFlowList)->GetCurFlowItem();
			pFlowItem->GetRules( vFlowRules );
			bool bBranchVote = true;
			for(std::vector<const CRule*>::const_iterator iter = vFlowRules.begin(); iter!=vFlowRules.end(); iter++)
			{
				if(!(*iter)->Vote(*m_pPerson))
				{
					bBranchVote = false;
				}
			}
			if(bBranchVote==false)continue;
		}

		bool bNeedCheckLeadToGate = NeedCheckLeadToGate();	// only check the gate after arrival gate for the turnaround passenger.
		if( *pToGate == *nextGroup || *pFromGate == *nextGroup || m_pPersonOnStation )
		{
			bNeedCheckLeadToGate = false;
		}

		bool bNeedCheckLeadToReclaim = NeddCheckLeadToReclaim();
		if (*pBaggage == *nextGroup || m_pPersonOnStation)
		{
			bNeedCheckLeadToReclaim = false;
		}
		// generate list of open processors
		getTerminal()->procList->buildArray (nextGroup, &destProcs,
			m_pPerson->m_type, m_pPerson->m_nGate, bNeedCheckLeadToGate, m_pPerson->m_nBagCarosel,bNeedCheckLeadToReclaim, FALSE, 
			m_pProcessor, _curTime,getEngine(), bRosterReason,(_bStationOnly||bTryExcludeElevatorProc)? -1:m_iDestFloor, 
			m_pDiagnoseInfo, m_pPerson, FALSE, m_vAltLandsideSelectedProc);


		// if none open, generate list of closed processors
		if (destProcs.getCount() == 0)
			getTerminal()->procList->buildArray (nextGroup, &destProcs,
			m_pPerson->m_type, m_pPerson->m_nGate, bNeedCheckLeadToGate, m_pPerson->m_nBagCarosel,bNeedCheckLeadToReclaim, TRUE, 
			m_pProcessor, _curTime, getEngine(), bRosterReason, (_bStationOnly||bTryExcludeElevatorProc)? -1:m_iDestFloor, 
			m_pDiagnoseInfo , m_pPerson, FALSE, m_vAltLandsideSelectedProc);

		if( _bStationOnly )
			KeepOnlyStation( &destProcs );

		if( _bExcludeFlowBeltProc )
		{
			FilterOutFlowBeltProcs( &destProcs );
		}

		FilterOutProcessorByFlowCondition( &destProcs , _curTime );
		// get count of possible destination processors
		procCount = destProcs.getCount();


		//if(getEngine()->IsLandSel() 
		//	&& m_pProcessor == getTerminal()->procList->getProcessor(START_PROCESSOR_INDEX)
		//	&& m_vAltLandsideSelectedProc.size())
		//{	
		//	bool bLandsideConnected = false;

		//	int nAvailableCount = static_cast<int>(m_vAltLandsideSelectedProc.size());
		//	for (int nAva = 0; nAva < nAvailableCount; ++ nAva)
		//	{
		//		ProcessorID procLandsideConnected;
		//		procLandsideConnected.SetStrDict(m_pTerm->inStrDict);
		//		procLandsideConnected.setID(m_vAltLandsideSelectedProc.at(nAva).GetIDString());;
		//		for (int nProc = 0; nProc < procCount; ++ nProc)
		//		{
		//			Processor *pProc = destProcs.getItem(nProc);
		//			if(procLandsideConnected.idFits(*(pProc->getID())))
		//			{
		//				bLandsideConnected = true;
		//			}
		//		}

		//	}

		//	if(bLandsideConnected== false)//not find
		//	{
		//		destProcs.clear();
		//		procCount = 0;
		//	}	
		//}



		// if any m_pProcessor can serve Person exit loop and select
		if (procCount > 0 && noExceptions (&destProcs) && IfBaggageProcAllowed( &destProcs, m_pFlowList ) )		
			break;


		// if randomly selected group invalid, try each group m_in order
	} while ((nextGroup = m_pFlowList->getDestinationGroup (SEQUENTIAL)) != NULL);

	//CString strSelectedProcGroup = nextGroup->GetIDString();
	//if(m_pProcessor && m_pProcessor->getID()->GetIDString().CompareNoCase(_T("CUSTOMSENTRYMAIN-1")) == 0)
	//{	
	//	CString strLog;
	//	//pax id, Time, Time String,Random Proc Group, selected Proc Group
	//	strLog.Format(_T("%d, %d, %s, %s, %s"), m_pPerson->getID(), _curTime.getPrecisely(), _curTime.PrintDateTime(), strRandomProcGroup, strSelectedProcGroup);

	//	CString strPath;
	//	strPath.Format(_T("%s%s"),getTerminal()->m_strProjPath, _T("FlowBranchRandomDistribution.csv"));
	//	ofsstream echoFile (strPath, stdios::app);
	//	echoFile<<strLog<<"\n";
	//	echoFile.close();
	//}



	//---------------------------------------------------------------------------------
	// if cannot found destination processor, check spare device(lowest priority processor:0%)
	if( (procCount == 0 || nextGroup == NULL) && (nextGroup = m_pFlowList->getZeropercentDestGroup())!= NULL )
	{
		do{
			// check for PROCESS

			bool bNeedCheckLeadToGate = NeedCheckLeadToGate();
			if( *pToGate == *nextGroup || *pFromGate == *nextGroup || m_pPersonOnStation )
			{
				bNeedCheckLeadToGate = false;
			}

			bool bNeedCheckLeadToReclaim = NeddCheckLeadToReclaim();
			if (*pBaggage == *nextGroup || m_pPersonOnStation)
			{
				bNeedCheckLeadToReclaim = false;
			}
			// generate list of open processors
			getTerminal()->procList->buildArray (nextGroup, &destProcs,						
				m_pPerson->m_type, m_pPerson->m_nGate, bNeedCheckLeadToGate, m_pPerson->m_nBagCarosel,bNeedCheckLeadToReclaim, 
				FALSE, m_pProcessor, _curTime,getEngine(), bRosterReason, 
				(_bStationOnly||bTryExcludeElevatorProc)? -1:m_iDestFloor, m_pDiagnoseInfo, m_pPerson, FALSE, m_vAltLandsideSelectedProc);

			// if none open, generate list of closed processors
			if (destProcs.getCount() == 0)
				getTerminal()->procList->buildArray (nextGroup, &destProcs,
				m_pPerson->m_type, m_pPerson->m_nGate, bNeedCheckLeadToGate, m_pPerson->m_nBagCarosel,bNeedCheckLeadToReclaim,
				TRUE, m_pProcessor, _curTime, getEngine(), bRosterReason,
				(_bStationOnly||bTryExcludeElevatorProc)? -1:m_iDestFloor, m_pDiagnoseInfo, m_pPerson, FALSE, m_vAltLandsideSelectedProc);

			if( _bStationOnly )
				KeepOnlyStation( &destProcs );

			if( _bExcludeFlowBeltProc )
				FilterOutFlowBeltProcs( &destProcs );

			FilterOutProcessorByFlowCondition( &destProcs , _curTime );
			// get count of possible destination processors
			procCount = destProcs.getCount();

			// if any m_pProcessor can serve Person exit loop and select
			if (procCount > 0 && noExceptions (&destProcs) && IfBaggageProcAllowed( &destProcs, m_pFlowList ) )		
				break;

		}while((nextGroup = m_pFlowList->getZeropercentDestGroup())!= NULL);
	}





	//dStepTime4 = GetTickCount();

	// no Processor has correct assignment to serve Person
	if (nextGroup == NULL)
	{
		// test if because the roster reason 
		if( bRosterReason )
		{	
			//find the nearest opened proc and let person go to that proc and wait for opening
			nextGroup = m_pFlowList->getDestinationGroup (RANDOM);
			int days = getTerminal()->flightSchedule->GetFlightScheduleEndTime().GetDay() ;
			ElapsedTime nearestTime;
			nearestTime.set(days*WholeDay );
			Processor* pReturnedProc = NULL;
			std::vector<Processor*>vReturnToProcsCloseReason;// all proc which is closed now and will be opened in the nearest future
			std::vector<Processor*>vReturnToProcsCannotServerReason;// all proc which open now , but can not serve for m_pPerson pax type, 
			// and  will serve for m_pPerson paxtype in the nearest future.
			bool bResultType=false;
			do
			{
				ElapsedTime tempTime;
				bool bResult = false;
				Processor* pNearestProc = getTerminal()->procList->GetNearestOpenedProc(m_pProcessor, nextGroup,_curTime,m_pPerson, tempTime, bResult ) ;
				if( pNearestProc && nearestTime > tempTime )
				{
					nearestTime = tempTime;					
					bResultType = bResult;
					if( bResult )
					{
						vReturnToProcsCloseReason.clear();
						vReturnToProcsCloseReason.push_back( pNearestProc );
					}
					else
					{
						vReturnToProcsCannotServerReason.clear();
						vReturnToProcsCannotServerReason.push_back( pNearestProc );
					}
				}
				else if( pNearestProc && nearestTime == tempTime )
				{
					if( bResult )
					{
						vReturnToProcsCloseReason.push_back( pNearestProc );
					}
					else
					{					
						vReturnToProcsCannotServerReason.push_back( pNearestProc );
					}
				}
			}
			while( (nextGroup = m_pFlowList->getDestinationGroup (SEQUENTIAL)) != NULL );

			//test zeropercent destination processor.
			if( bRosterReason && vReturnToProcsCloseReason.size()==0 && (nextGroup = m_pFlowList->getZeropercentDestGroup())!= NULL)
			{
				do
				{

					bool bNeedCheckLeadToGate = NeedCheckLeadToGate();	// only check the gate after arrival gate for the turnaround passenger.
					if( *pToGate == *nextGroup || *pFromGate == *nextGroup || m_pPersonOnStation )
					{
						bNeedCheckLeadToGate = false;
					}

					bool bNeedCheckLeadToReclaim = NeddCheckLeadToReclaim();
					if (*pBaggage == *nextGroup || m_pPersonOnStation)
					{
						bNeedCheckLeadToReclaim = false;
					}
					// generate list of open processors
					getTerminal()->procList->buildArray (nextGroup, &destProcs,						
						m_pPerson->m_type, m_pPerson->m_nGate, bNeedCheckLeadToGate, m_pPerson->m_nBagCarosel,bNeedCheckLeadToReclaim, 
						FALSE, m_pProcessor, _curTime,getEngine(), bRosterReason, 
						(_bStationOnly||bTryExcludeElevatorProc)? -1:m_iDestFloor, m_pDiagnoseInfo, m_pPerson, FALSE, m_vAltLandsideSelectedProc);

					// if none open, generate list of closed processors
					if (destProcs.getCount() == 0)
						getTerminal()->procList->buildArray (nextGroup, &destProcs,
						m_pPerson->m_type, m_pPerson->m_nGate, bNeedCheckLeadToGate, m_pPerson->m_nBagCarosel,bNeedCheckLeadToReclaim,
						TRUE, m_pProcessor, _curTime, getEngine(), bRosterReason,
						(_bStationOnly||bTryExcludeElevatorProc)? -1:m_iDestFloor, m_pDiagnoseInfo, m_pPerson, FALSE, m_vAltLandsideSelectedProc);

					if( _bStationOnly )
						KeepOnlyStation( &destProcs );

					if( _bExcludeFlowBeltProc )
						FilterOutFlowBeltProcs( &destProcs );

					FilterOutProcessorByFlowCondition( &destProcs , _curTime );
					// get count of possible destination processors
					procCount = destProcs.getCount();

					// if any m_pProcessor can serve Person exit loop and select
					if (procCount > 0 && noExceptions (&destProcs) && IfBaggageProcAllowed( &destProcs, m_pFlowList ) )		
						break;

				}while((nextGroup = m_pFlowList->getZeropercentDestGroup())!= NULL);

				if (destProcs.getCount()==0)
					return NULL;
			}
			else
			{				
				if( bResultType )// closed reason
				{
					Processor *pProc =NULL;
					int iBackToProcCount = vReturnToProcsCloseReason.size();
					if( iBackToProcCount > 0 )
					{						
						for( int i=0; i<iBackToProcCount; i++ )
						{
							destProcs.addItem( vReturnToProcsCloseReason[i] );
						}
						pProc= selectBestProcessor(&destProcs);
					}
					else
					{
						return NULL;
					}

					if(!pProc->GetWaitInQueueToOpen())
					{
						m_bWaitInLinkedArea = true;
						m_pBackToProcFromWaitArea =  pProc;
						Processor* pProcWaitArea = getTerminal()->procList->GetWaitArea( m_pBackToProcFromWaitArea->getID() );
						return pProcWaitArea;			
					}
					else 
						return pProc;
				}
				else// proc is servicing for other pax type now
				{
					int iBackToProcCount = vReturnToProcsCannotServerReason.size();
					if( iBackToProcCount > 0 )
					{
						m_bWaitInLinkedArea = true;
						m_pBackToProcFromWaitArea =  vReturnToProcsCannotServerReason[ random(iBackToProcCount) ];
						//// TRACE("%s\r\n",m_pBackToProcFromWaitArea->getID()->GetIDString().GetBuffer(0));	
						Processor* pProcWaitArea = getTerminal()->procList->GetWaitArea( m_pBackToProcFromWaitArea->getID() );					
						return pProcWaitArea;					
					}
					else
					{
						return NULL;
					}
				}
			}
		}
		else
		{
			return NULL;
		}
	}

	//now 1x1 start
	if( ((CProcessorDistributionWithPipe*)m_pFlowList)->GetOneXOneState() == ChannelStart && getTerminal()->procList->getProcessor (START_PROCESSOR_INDEX) != m_pProcessor)
	{
		CFlowChannel* newChannel=new CFlowChannel(m_pProcessor);
		m_FlowChannelList.PushChannel(newChannel);
	}
	//now 1x1 end
	if( ((CProcessorDistributionWithPipe*)m_pFlowList)->GetOneXOneState() == ChannelEnd )
	{
		Processor* pDestProc;

		CFlowChannel* getChannel;
		getChannel = m_FlowChannelList.PopChannel();
		if (getChannel != NULL)
		{
			pDestProc = getChannel->GetOutChannelProc(destProcs,m_pFlowList->GetCurDestIDLength());
		}
		else
		{
			pDestProc = NULL;			
		}
		delete getChannel;
		if( pDestProc == NULL )
		{
			//throw new OneXOneFlowError( m_pPerson );
			ReleaseResource(_curTime);
			kill(_curTime);

			throw new ARCDestProcessorUnavailableError( szMobType, strProcNameTmp, "one X one", ClacTimeString(_curTime));
		}
		else
		{
			return pDestProc;
		}		
	}



	// if only one m_pProcessor is available to serve the Person,
	// select it and end selection process
	if (procCount == 1)
		return destProcs.getItem(0);

	// now 1:1
	if(getTerminal()->procList->getProcessor (START_PROCESSOR_INDEX) != m_pProcessor && m_pFlowList->IsCurOneToOne() )
	{
		CFlowChannel* newChannel=new CFlowChannel(m_pProcessor);
		m_FlowChannelList.PushChannel(newChannel);
		Processor* pDestProc;
		//		CString str = m_pProcessor->getID()->GetIDString();		
		CFlowChannel* getChannel;
		getChannel = m_FlowChannelList.PopChannel();
		if (getChannel != NULL)
		{
			pDestProc = getChannel->GetOutChannelProc(destProcs,m_pFlowList->GetCurDestIDLength());
		}
		else
		{
			pDestProc = NULL;			
		}
		delete getChannel;
		//////////////////////////////////////////////////////////////////////////		
		if( pDestProc == NULL ) 
		{
			//throw new OneToOneFlowError( m_pPerson );
			ReleaseResource(_curTime);
			kill(_curTime);

			CString strErrorInfo = _T("one to one(");
			int nProcCount = destProcs.getCount();
			for(int i=0;i<nProcCount-1;i++)
			{
				strErrorInfo += (destProcs.getItem(i)->getID()->GetIDString() + _T(";"));
			}
			strErrorInfo += (destProcs.getItem(nProcCount-1)->getID()->GetIDString() + _T(")"));
			throw new ARCDestProcessorUnavailableError( szMobType, strProcNameTmp, strErrorInfo, _curTime.PrintDateTime());
		}
		else
		{
			return pDestProc;
		}		
	}

	// first Processor is always selected randomly
	if (m_pPerson->getState() == Birth)
		return destProcs.getItem (random(procCount));

	//dStepTime5 = GetTickCount();
	// choose m_pProcessor with intest queue & smallest distance
	Processor *pProcReturn = NULL;
	pProcReturn =selectBestProcessor (&destProcs);

	////test code 
	//
	return pProcReturn;
}
// It examines each m_in list of possible processors to determine best option.
// If the input paramenter "p_procList" is a mixed group (hold and non-hold procs), select smallest count of pax.
Processor *TerminalMobElementBehavior::selectBestProcessor (const ProcessorArray *p_procList)
{
	// Here just for the purpose of selection the best next processors, if all the
	// processor exceed the capacity, let them select in the next step. leave the problem
	// to other component.

	//	FILE *fp = fopen("c:\\selectprocessor.txt","a+");
	//	fprintf(fp,"\r\nid==%d",id);

	ProcessorArray destProcs;
	int nCount = p_procList->getCount();
	for( int i=0; i<nCount; i++ )
	{
		Processor* pProc = p_procList->getItem(i);
		if( pProc->isVacant(m_pPerson) == false)
			continue;

		if(pProc->isDependentProcUsed()
			&& pProc->isAllDependentProcsOccupied(m_pPerson))
			continue;
		destProcs.addItem( pProc );

	}

	if( destProcs.getCount() == 0 )
	{
		// 
		for( int i=0; i<nCount; i++ )
		{
			Processor* pProc = p_procList->getItem(i);
			destProcs.addItem( pProc );
		}
	}


	// check holding areas linkage with baggageDevice;
	Processor *aProc=NULL;
	Processor *pPtDenstProc = NULL;
	aProc = p_procList->getItem(0);
	if(aProc->getProcessorType() == HoldAreaProc)
	{
		pPtDenstProc =GetHoldAreaLinkageFromType(m_pPerson,*p_procList);
		if(pPtDenstProc!=NULL)
			return pPtDenstProc;
	}

	// check for group of holding areas
	CMobileElemConstraint defaultType(m_pTerm);
	int allHoldAreas = 1, procCount = destProcs.getCount();

	for (int i = 0; i < procCount; i++)
	{
		aProc = destProcs.getItem(i);
		if (aProc->getProcessorType() != HoldAreaProc)
		{
			allHoldAreas = 0;
			break;
		}
	}

	if (allHoldAreas)
		return selectBestHoldArea (&destProcs);

	Processor *bestProc = destProcs.getItem(0);
	HoldingArea *holdProc;
	int count, minQueue,minOccupants;
	double minDist;

	int nFirstDepSinkCount= 0 ; // depsinkcount relax the Dsouce
	int nSinkCount=0;

	double dFirstDependResult = 999999.9;    // ==occupants / depsinkcount
	double dDenpendResult = 999999.9;

	if (bestProc->getProcessorType() == HoldAreaProc)
	{
		holdProc = (HoldingArea *) bestProc;
		minQueue = holdProc->getPaxOfType (defaultType);
	}
	else
	{
		minQueue = bestProc->getTotalQueueLength();
		if( bestProc->getProcessorType() == Elevator )
		{
			m_iSourceFloor = (int)getPoint().getZ() / (int)SCALE_FACTOR;
			minDist = getPoint().distance ( ((ElevatorProc*)bestProc)->GetServiceLocationOnFloor(m_iSourceFloor));
		}
		else
			minDist = getPoint().distance ( bestProc->GetServiceLocation());

		minOccupants = bestProc->GetOccupants()->getCount(); 

		if (bestProc->getProcessorType() == DepSourceProc)
		{
			nFirstDepSinkCount = ((DependentSource *) bestProc)->GetSinkCount();
			if(nFirstDepSinkCount!=0) 
				dFirstDependResult = ((double)minOccupants)/nFirstDepSinkCount; 

			//	  fprintf(fp,"\r\nminOccupants==%d,nFirstDepSinkCount==%d,dFirstDenpendResult==%f,minDist==%f",minOccupants,nFirstDepSinkCount,dFirstDependResult,minDist);

		}
	}


	std::vector<Processor*>vProcs;
	vProcs.push_back( bestProc );
	int nOccupantCount=0;
	for (int i = 1; i < procCount; i++)
	{
		aProc = destProcs.getItem(i);
		count = aProc->getTotalQueueLength();
		nOccupantCount = aProc->GetOccupants()->getCount();

		if (aProc->getProcessorType() == DepSourceProc)
		{
			nSinkCount = ((DependentSource *) aProc)->GetSinkCount();
			if(nSinkCount!=0) 
				dDenpendResult = ((double)nOccupantCount)/nSinkCount;
		}

		DistanceUnit dDistance;

		if( aProc->getProcessorType() == Elevator )
			dDistance = getPoint().distance(((ElevatorProc*)aProc)->GetServiceLocationOnFloor((int)getPoint().getZ() / (int)SCALE_FACTOR));
		else
			dDistance = getPoint().distance(aProc->GetServiceLocation());



		//	fprintf(fp,"\r\nproc[%d]:Occupants==%d,nDepSinkCount==%d,dDenpendResult==%f,minDist==%f",i,nOccupantCount,nSinkCount,dDenpendResult,dDistance);

		if (aProc->getProcessorType() == HoldAreaProc)
		{
			holdProc = (HoldingArea *) aProc;
			count = holdProc->getPaxOfType (defaultType);
			if (count < minQueue)
			{
				minQueue = count;
				vProcs.clear();
				vProcs.push_back( aProc );			
			}
			else if( count == minQueue )
			{
				DistanceUnit dDistance = getPoint().distance(aProc->GetServiceLocation());
				if( dDistance < minDist )
				{
					minDist = dDistance;
					vProcs.clear();
					vProcs.push_back( aProc );
				}
				else if ( dDistance = minDist )
				{
					vProcs.push_back( aProc );	 
				}					
			}
		}
		else if (aProc->getProcessorType() == DepSourceProc)
		{
			if( dDenpendResult < dFirstDependResult)
			{
				dFirstDependResult = dDenpendResult;
				vProcs.clear();
				vProcs.push_back( aProc );
			}
			else if (dDenpendResult == dFirstDependResult) 
			{
				//DistanceUnit dDistance = location.distance(aProc->getServiceLocation(m_pPerson));
				//fprintf(fp,"\r\naProc%dDistance=%f",i,dDistance); //matt
				if( dDistance < minDist )
				{
					minDist = dDistance;
					vProcs.clear();
					vProcs.push_back( aProc );
				}
				else if ( dDistance == minDist )
				{
					vProcs.push_back( aProc );	 
				}				
			}
		}
		// select proc with (1) smallest occupancy & (2) shortest distance
		else if( nOccupantCount < minOccupants)
		{
			minOccupants = nOccupantCount;
			vProcs.clear();
			vProcs.push_back( aProc );
		}
		else if (nOccupantCount == minOccupants) 
		{
			//DistanceUnit dDistance = location.distance(aProc->getServiceLocation(m_pPerson));
			//fprintf(fp,"\r\naProc%dDistance=%f",i,dDistance); //matt
			if( dDistance < minDist )
			{
				minDist = dDistance;
				vProcs.clear();
				vProcs.push_back( aProc );
			}
			else if ( dDistance == minDist )
			{
				vProcs.push_back( aProc );	 
			}				
		}
		/*
		else if (count < minQueue)
		{
		minQueue = count;
		vProcs.clear();
		vProcs.push_back( aProc );

		}
		else if( count == minQueue )
		{
		Processor* pFirstProc = vProcs[0];
		if( aProc->GetOccupants()->getCount() < pFirstProc->GetOccupants()->getCount() )
		{
		vProcs.clear();
		vProcs.push_back( aProc );
		fprintf(fp,"\r\naProc->GetOccupants()->getCount()<pFirstProc->GetOccupants()->getCount()%s","selected"); //matt
		}
		else if ( aProc->GetOccupants()->getCount() == pFirstProc->GetOccupants()->getCount() )
		{
		DistanceUnit dDistance = location.distance(aProc->getServiceLocation(m_pPerson));
		fprintf(fp,"\r\naProc%dDistance=%f",i,dDistance); //matt
		if( dDistance < minDist )
		{
		minDist = dDistance;
		vProcs.clear();
		vProcs.push_back( aProc );
		}
		else if ( dDistance == minDist )
		{
		vProcs.push_back( aProc );	 
		}				
		}			
		}*/

	}

	int iCount = vProcs.size();
	assert( iCount >= 0 );

	//// Filter out by dependent processors
	//if(vProcs.size() > 1)
	//{
	//	Processor* defaultProcHolder = vProcs.at(0);
	//	std::vector<Processor*>::iterator iter = vProcs.begin();
	//	for (; iter != vProcs.end();)
	//	{
	//		Processor* pDestProcessor = *iter;
	//		if(pDestProcessor->isDependentProcUsed()
	//			&& pDestProcessor->isAllDependentProcsOccupied(m_pPerson))
	//		{
	//			iter = vProcs.erase( iter );
	//		}
	//		else
	//			++iter;
	//	}

	//	if(vProcs.size() == 0 )
	//		vProcs.push_back( defaultProcHolder );
	//}


	int nSele = random(iCount); 

	//	fprintf(fp,"\r\nSelectvProcsCount=%d,SelctedRedom=%d",iCount,nSele); //matt

	Processor * pBestPro =vProcs[nSele];

	//	CString sIdString =pBestPro->getID()->GetIDString(); //matt
	//	fprintf(fp,"\r\ndestProcsCount%dIDString==%s",i,sIdString.GetBuffer(0));
	//	fprintf(fp,"\r\n"); //matt
	//	fclose(fp);

	return vProcs[nSele];
	//return bestProc;
}

void TerminalMobElementBehavior::ReleaseResource(const ElapsedTime& _congeeTime)
{
	if(m_pProcessor && m_pProcessor->getProcessorType()==DepSinkProc)
	{
		static_cast<DependentSink*>(m_pProcessor)->makeAvailable(m_pPerson,_congeeTime,false);
	}
	if(m_pProcessor && m_pProcessor->getProcessorType() == LineProc)
	{
		static_cast<LineProcessor*>(m_pProcessor)->makeAvailable(m_pPerson,_congeeTime,false);
	}
}

void TerminalMobElementBehavior::kill (ElapsedTime killTime)
{
	if (m_pPerson->getState() != Birth)
		m_pProcessor->removePerson (m_pPerson);

	flushLog (killTime);
}

// It examines each m_in list of possible hold areas to determine best option, which is the one with the lowest density of passengers and returns it.
Processor *TerminalMobElementBehavior::selectBestHoldArea (const ProcessorArray *p_procList)
{
	HoldingArea *holdProc, *bestProc = (HoldingArea *)p_procList->getItem(0);
	double minDensity = bestProc->getDensity();
	CMobileElemConstraint defaultType(m_pTerm);
	std::vector<Processor*>vBestHoldingAreas;
	int procCount = p_procList->getCount();
	vBestHoldingAreas.push_back( bestProc );
	for (int i = 1; i < procCount; i++)
	{
		holdProc = (HoldingArea *)p_procList->getItem(i);
		//		// TRACE("\nholding area :%s\n", holdProc->getID()->GetIDString() );
		if (holdProc->getDensity() < minDensity)
		{
			vBestHoldingAreas.clear();
			vBestHoldingAreas.push_back( holdProc );
			bestProc = holdProc;
			minDensity = bestProc->getDensity();
		}
		else if( holdProc->getDensity() == bestProc->getDensity() )
		{
			vBestHoldingAreas.push_back( holdProc );
		}

	}

	int iBestCount = vBestHoldingAreas.size();
	int iSelectedIdx = random( iBestCount );
	return vBestHoldingAreas[iSelectedIdx];
}

bool TerminalMobElementBehavior::StickNonPaxDestProcsOverload(const ElapsedTime& _curTime)
{
	return false;
}

bool TerminalMobElementBehavior::StickForDestProcsOverload(const ElapsedTime& _curTime)
{
	m_pPerson->regetAttatchedNopax();
	ProcessorDistribution* pNextProcDistribution = m_pProcessor->getNextDestinations ( m_pPerson->getType(), m_nInGateDetailMode );
	if(pNextProcDistribution ==NULL ) return false;
	CSinglePaxTypeFlow* pProcessSingleFlow=NULL;
	ProcessorID* pProcessCurID=NULL;
	//	if(pNextProcDistribution != m_pPreFlowList )

	//if (id ==262053 && m_pProcessor && m_pProcessor->getID()->GetIDString().CompareNoCase(_T("SECURITYCHECK-T1-SOUTH-REGULAR")) == 0)
	//{
	//	ofsstream echoFile ("d:\\SECURITYCHECK.log", stdios::app);
	//	echoFile<<"stick"<<"\n";
	//	echoFile<<"Current proc"<<m_pProcessor->getID()->GetIDString()<<"\n";

	//	echoFile<<"m_pSubFlow"<<(int)m_pSubFlow<<"\n";

	//	echoFile.close();
	//}


	pNextProcDistribution->getDestinationGroup( RANDOM);


	ASSERT( pNextProcDistribution!=NULL);

	CFlowChannel* pInChannel=NULL;
	ProcessorArray aryOverloadDestProcs;
	ProcessorArray aryDestProcs;

	//defined for function FindDestProcessors,
	//if can not find the destination processor for 1:1 reason
	//or 1*1 reason, remember it.
	//throw exception after test every processor in distribution.
	bool bOneToOneReason = false;
	bool bOneXOneReason = false;

	do
	{
		pInChannel=NULL;
		if( static_cast<CProcessorDistributionWithPipe*>(pNextProcDistribution)->GetOneXOneState()==ChannelEnd)
		{
			pInChannel=m_FlowChannelList.PopChannel();
			m_FlowChannelList.PushChannel(pInChannel);//restore channel info for next time to use.
		}
		m_pProcessor->FindDestProcessors( m_pPerson, pNextProcDistribution, 
			pInChannel, _curTime, pProcessSingleFlow, pProcessCurID,
			aryDestProcs, bOneToOneReason, bOneXOneReason);
		//ASSERT( aryDestProcs.getCount()>0);
		/////start
		//if (id ==262053 && m_pProcessor && m_pProcessor->getID()->GetIDString().CompareNoCase(_T("SECURITYCHECK-T1-SOUTH-REGULAR")) == 0)
		//{
		//	ofsstream echoFile ("d:\\SECURITYCHECK.log", stdios::app);
		//	echoFile<<"FindDestProcessors"<<"\n";
		//	echoFile<<"Current proc"<<m_pProcessor->getID()->GetIDString()<<"\n";
		//	int nArrayCount = aryDestProcs.getCount();
		//	echoFile<<"proc count:"<<nArrayCount<<"\n";

		//	for(int i=0 ; i< nArrayCount; i++)
		//	{
		//		Processor* pProc = aryDestProcs.getItem(i);
		//		echoFile<<"Processor Name :"<< pProc->getID()->GetIDString()<<"    "
		//			<<"Proc occupancy count:"<<pProc->getQueueLength()<<"   "
		//			<<"proc occupancy  pax count :"<<pProc->getTotalGroupCount()<<"   "
		//			<<"\n";
		//	}
		//	echoFile.close();
		//}
		/////end
		//
		Processor::PruneInvalidDestProcs(getTerminal(), m_pPerson, &aryDestProcs, NULL, &aryOverloadDestProcs );

		//test code by hans . log all the available dest proc information
		////code start
		//if (id ==262053 && m_pProcessor && m_pProcessor->getID()->GetIDString().CompareNoCase(_T("SECURITYCHECK-T1-SOUTH-REGULAR")) == 0)
		//{
		//	ofsstream echoFile ("d:\\SECURITYCHECK.log", stdios::app);
		//	echoFile << "person Group Active Size :" << GetActiveGroupSize() <<"\n";
		//	echoFile<<"StickForDestProcsOverload"<<"\n";
		//	
		//	int nArrayCount = aryDestProcs.getCount();
		//	echoFile<<"available proc count:"<<nArrayCount<<"\n";

		//	for(int i=0 ; i< nArrayCount; i++)
		//	{
		//		Processor* pProc = aryDestProcs.getItem(i);
		//		echoFile<<"Processor Name :"<< pProc->getID()->GetIDString()<<"    "
		//			<<"Proc occupancy count:"<<pProc->getQueueLength()<<"   "
		//			<<"proc occupancy  pax count :"<<pProc->getTotalGroupCount()<<"   "
		//			<<"\n";
		//	}
		//	int nUnavailableCount = aryOverloadDestProcs.getCount();
		//	echoFile<<"unavailable proc count:"<<nArrayCount<<"\n";

		//	for(int i=0 ; i< nUnavailableCount; i++)
		//	{
		//		Processor* pProc = aryOverloadDestProcs.getItem(i);
		//		echoFile<<"Processor Name :"<< pProc->getID()->GetIDString()<<"    "
		//			<<"Proc occupancy count:"<<pProc->getQueueLength()<<"   "
		//			<<"proc occupancy  pax count :"<<pProc->getTotalGroupCount()<<"   "
		//			<<"\n";
		//	}
		//	echoFile.close();
		//}
		////code end

		if(aryDestProcs.getCount()>0 && noExceptions (&aryDestProcs) && IfBaggageProcAllowed( &aryDestProcs, pNextProcDistribution ))
			return false;


	}while( pNextProcDistribution->getDestinationGroup( SEQUENTIAL) ) ;

	//check zeropercent dest group.
	if(aryDestProcs.getCount()==0)
	{
		while(pNextProcDistribution->getZeropercentDestGroup())
		{
			pInChannel=NULL;
			if( static_cast<CProcessorDistributionWithPipe*>(pNextProcDistribution)->GetOneXOneState()==ChannelEnd)
			{	
				pInChannel=m_FlowChannelList.PopChannel();
				m_FlowChannelList.PushChannel(pInChannel);//restore channel info for next time to use.
			}
			m_pProcessor->FindDestProcessors( m_pPerson, pNextProcDistribution, 
				pInChannel, _curTime, pProcessSingleFlow, pProcessCurID, 
				aryDestProcs, bOneToOneReason, bOneXOneReason);
			//ASSERT( aryDestProcs.getCount()>0);

			Processor::PruneInvalidDestProcs(getTerminal(), m_pPerson, &aryDestProcs, NULL, &aryOverloadDestProcs );

			//test code by hans . log all the available dest proc information
			//code start
			////if (id ==262053 && m_pProcessor && m_pProcessor->getID()->GetIDString().CompareNoCase(_T("SECURITYCHECK-T1-SOUTH-REGULAR")) == 0)
			////{
			////	ofsstream echoFile ("d:\\SECURITYCHECK.log", stdios::app);
			////	echoFile << "person Group Active Size :" << GetActiveGroupSize() <<"\n";
			////	echoFile<<"StickForDestProcsOverload -- zero percent procs"<<"\n";

			////	int nArrayCount = aryDestProcs.getCount();
			////	echoFile<<"available proc count:"<<nArrayCount<<"\n";

			////	for(int i=0 ; i< nArrayCount; i++)
			////	{
			////		Processor* pProc = aryDestProcs.getItem(i);
			////		echoFile<<"Processor Name :"<< pProc->getID()->GetIDString()<<"    "
			////			<<"Proc occupancy count:"<<pProc->getQueueLength()<<"   "
			////			<<"proc occupancy  pax count :"<<pProc->getTotalGroupCount()<<"   "
			////			<<"\n";
			////	}
			////	int nUnavailableCount = aryOverloadDestProcs.getCount();
			////	echoFile<<"unavailable proc count:"<<nArrayCount<<"\n";

			////	for(int i=0 ; i< nUnavailableCount; i++)
			////	{
			////		Processor* pProc = aryOverloadDestProcs.getItem(i);
			////		echoFile<<"Processor Name :"<< pProc->getID()->GetIDString()<<"    "
			////			<<"Proc occupancy count:"<<pProc->getQueueLength()<<"   "
			////			<<"proc occupancy  pax count :"<<pProc->getTotalGroupCount()<<"   "
			////			<<"\n";
			////	}
			////	echoFile.close();
			////}
			//code end

			if(aryDestProcs.getCount()>0 && noExceptions (&aryDestProcs) && IfBaggageProcAllowed( &aryDestProcs, pNextProcDistribution ))
				return false;

		}
	}

	if(bOneToOneReason)
	{
		CString str = "Can't find processor to correspond to "+ m_pProcessor->getID()->GetIDString()+ " by 1:1";
		throw new ARCDestProcessorUnavailableError( getPersonErrorMsg(), 
			m_pProcessor->getID()->GetIDString(), str, _curTime.PrintDateTime());
	}
	if(bOneXOneReason)
	{
		CString str = "Cannot find processor to correspond to "+ pInChannel->GetInChannelProc()->getID()->GetIDString()+ " by 1:x:1";
		throw new ARCDestProcessorUnavailableError( getPersonErrorMsg(),
			m_pProcessor->getID()->GetIDString(), str, _curTime.PrintDateTime());
	}

	//if(aryDestProcs.getCount()>0) return false;//not need to stick.
	if(aryOverloadDestProcs.getCount()>0)
	{
		Processor* pAnOverloadProc=NULL;
		WaitingPair waitPair;
		waitPair.first= m_pPerson->getID();
		waitPair.second =m_pProcessor->getIndex();

		for( int i=0; i<aryOverloadDestProcs.getCount(); i++)
		{
			pAnOverloadProc= aryOverloadDestProcs.getItem( i );
			pAnOverloadProc->AddWaitingPair( waitPair );	// add pax id and processor idx to the dest processor.
		}
		return true;
	}
	return false;
}
CString TerminalMobElementBehavior::getPersonErrorMsg( void ) const
{
	// pax id
	CString paxID;
	paxID.Format("%d <@>",m_pPerson->getID() );

	// get flight id;
	CString strFlightID;
	char airline[AIRLINE_LEN];
	m_pPerson->getType().getAirline(airline);
	if( airline[0] )
	{
		if( m_pPerson->getType().getFlightID() )
		{
			strFlightID.Format( "%s",m_pPerson->getType().getFlightID().GetValue() );
		}
		strFlightID = CString( airline) + strFlightID;
	}

	strFlightID +=" <@>";

	// mob type
	//char szMobType[256];
	CString sMobType;
	m_pPerson->getType().screenPrint( sMobType, 0, 256 );

	return paxID + strFlightID + sMobType + " <@>";
}
//It depends upon arrival at service location of a HoldingArea, check flow to see if subsequent processors are available.
void TerminalMobElementBehavior::leaveHoldArea (ElapsedTime releaseTime)
{

	if (!m_nStranded || (m_pPerson->getState() == WaitInHoldingArea && m_pFlowList && selectProcessor( releaseTime )))
	{
		m_nStranded = FALSE;
		m_pPerson->setState(LeaveServer);
		//////because the holdarea proc has a particular process,cause it doesn't has Arrive at Server state and cann't clear the information,
		////so we need to clear the information when the pax leave holdarea.
		setNextHoldAreaPoTag(false);
		generateEvent (releaseTime,false);
	}
}
void TerminalMobElementBehavior::leaveLineProc ( const ElapsedTime& releaseTime)
{
	m_nStranded = FALSE;
	m_pPerson->setState(LeaveServer);
	setNextHoldAreaPoTag(false);
	generateEvent (releaseTime,false);
}

//It moves to end of OutConstraint, then schedules event to calculate next m_pProcessor.
//At the time the end of the out constraint is reached.
void TerminalMobElementBehavior::processOutConstraint (ElapsedTime p_time)
{
	m_nCurrentIndex = 0;
	writeLogEntry (p_time, m_pProcessor->IsBackup() );

	while (m_nCurrentIndex < m_pProcessor->outConstraintLength()-1)
	{
		setDestination (m_pProcessor->outConstraint (m_nCurrentIndex++));
		p_time += moveTime();
		writeLogEntry (p_time, m_pProcessor->IsBackup());
	}

	setDestination (m_pProcessor->outConstraint (m_nCurrentIndex));
	ElapsedTime eNextEventTime = p_time + moveTime();
	generateEvent (eNextEventTime,false);

	if(m_pProcessor->getProcessorType() == GateProc) 
	{
		//the departure gate has high priority than arrival gate, so, here, need to remember the time of last pax walk out of the out constraint
		//for departure gate will check the last person to walk out the linkage arrival gate,
		//if no person at the  arrival gate for a safe time, the pax will get through
		((GateProcessor *)m_pProcessor)->SetLastPaxOutConsTime(eNextEventTime);
	}
}

// test if the baggage proc is suitable here
bool TerminalMobElementBehavior::IfBaggageProcAllowed( const ProcessorArray *nextGroup , ProcessorDistribution* _pDistritution) const
{
	if( m_pPerson->getType().GetTypeIndex() != 0 )// is no pax
	{
		return true;
	}

	Processor* pBaggageProc = m_pTerm->procList->getProcessor ( BAGGAGE_DEVICE_PROCEOR_INDEX);
	Processor *proc = NULL;

	if( ((Passenger*)m_pPerson)->GetBagsCount() > 0 )// have checked bags
	{
		int iBaggageProcCount = 0;
		for (int i = 0; i < nextGroup->getCount(); i++)
		{
			proc = nextGroup->getItem(i);
			//			// TRACE("\n%s\n", proc->getID()->GetIDString() );
			if( proc->getProcessorType() == BaggageProc  || proc == pBaggageProc )
			{
				++iBaggageProcCount; 
			}
		}

		if( iBaggageProcCount )
		{
			// if have at least one baggage proc , then all should be baggage proc
			return iBaggageProcCount == nextGroup->getCount();
		}
		else
		{
			// if no baggage proc, then test if other path have baggage proc . ( to implement the by pass procssor logic )
			return !IfBaggageExistInDist( _pDistritution );
		}
	}


	// if no bags , then the dest proc should not have a baggageproc
	for (int i = 0; i < nextGroup->getCount(); i++)
	{
		proc = nextGroup->getItem(i);
		if( proc->getProcessorType() == BaggageProc  || proc == pBaggageProc )
		{
			return false;
		}
	}

	return true;
}
//
//void TerminalMobElementBehavior::SetNextHoldAreaValue(bool HoldAreaPoTag,const Point &HoldareaPoint)
//{
//	m_nextHoldAiearPoTag=HoldAreaPoTag;
//	m_nextHoldAiearPoint=HoldareaPoint;
//}
bool TerminalMobElementBehavior::IfBaggageExistInDist( ProcessorDistribution* _pDistritution) const
{
	const ProcessorID *nextGroup = NULL;
	Processor* pBaggageProc = m_pTerm->procList->getProcessor ( BAGGAGE_DEVICE_PROCEOR_INDEX);
	Processor *proc = NULL;
	int iDistributionCount = _pDistritution->getCount();
	for( int j=0; j<iDistributionCount; ++j )
	{
		nextGroup = _pDistritution->getGroup( j );
		GroupIndex group = m_pTerm->procList->getGroupIndex (*nextGroup);
		//TRACE("\n%s\n", nextGroup->GetIDString() );
		if( group.start <0 )
			continue;

		for (int i = group.start; i <= group.end; i++)
		{
			proc = m_pTerm->procList->getProcessor ( i );
			//TRACE("\n%s\n", proc->getID()->GetIDString() );
			if( proc->getProcessorType() == BaggageProc  || proc == pBaggageProc )
			{
				return true;
			}
		}
	}
	return false;
}

Processor* TerminalMobElementBehavior::SelectNextProcessor(ProcessorDistribution* _pFlowList, const ElapsedTime& p_time)
{
	if(_pFlowList)
	{
		ProcessorDistribution* pOldFlowList=m_pFlowList;
		m_pFlowList = _pFlowList;
		//back up data member;
		bool bGerentGreetEventOrWait = m_bGerentGreetEventOrWait;

		bool bWaitInLinkedArea= m_bWaitInLinkedArea;
		int nSourceFloor = m_iSourceFloor;
		Person* pWantToGreetPerson = m_pWantToGreetPerson;
		Processor* pBackToProcFromWaitArea = m_pBackToProcFromWaitArea;


		//back up channel data
		CProcessorDistributionWithPipe* pFlowListWithPipe=(CProcessorDistributionWithPipe*)_pFlowList;
		CFlowChannel* pBackupChannel=NULL;
		//back up data
		CFlowChannel* pChannel=m_FlowChannelList.PopChannel();
		if(pChannel!=NULL)
		{
			pBackupChannel=new CFlowChannel( pChannel->GetInChannelProc());
			m_FlowChannelList.PushChannel( pChannel);//pChannel will be deleted in selectProcessor()
		}

		//selectProcessor will change some data member , but I don't want it.
		Processor* pNextProcs = selectProcessor(p_time);
		if( pFlowListWithPipe->GetCurGroupIndex()>=0)
		{
			switch(pFlowListWithPipe->GetOneXOneState()) 
			{
			case ChannelStart:
				//clear dirty data
				delete m_FlowChannelList.PopChannel();
				delete pBackupChannel;
				break;
			case ChannelEnd:
				ASSERT( pBackupChannel!=NULL);
				//restore data
				m_FlowChannelList.PushChannel( pBackupChannel);
				pBackupChannel=NULL;
				break;
			case NonChannel:
				//selectProcessor(p_time) don't change channel data, so give up backup data.
				delete pBackupChannel;
			}
		}

		//restore data member;

		m_pBackToProcFromWaitArea = pBackToProcFromWaitArea;
		m_pWantToGreetPerson = pWantToGreetPerson;
		m_iSourceFloor = nSourceFloor;
		m_bWaitInLinkedArea = bWaitInLinkedArea;
		m_bGerentGreetEventOrWait = bGerentGreetEventOrWait;

		m_pFlowList = pOldFlowList;
		return pNextProcs;
	}


	return NULL;
}

Terminal * TerminalMobElementBehavior::getTerminal()
{
	return getEngine()->getTerminal();
}

void TerminalMobElementBehavior::ProcessHoldingAreaPipe(Processor* _pNextProc, ElapsedTime& _curTime)
{
	if( _pNextProc == NULL )
		return;

	Point ptFrom = location;

	int iCurFloor = (int)(location.getZ() / SCALE_FACTOR);
	if(m_pProcessor->getIndex()==START_PROCESSOR_INDEX)
	{
		iCurFloor = _pNextProc->getFloor() /(int) SCALE_FACTOR;
	}
	if(_pNextProc->getIndex() == END_PROCESSOR_INDEX)
	{
		iCurFloor = 0;
	}

	Point ptTo = GetPipeExitPoint(_pNextProc,iCurFloor,ClacTimeString(_curTime),ptFrom);
	m_ptOldDest = ptTo;

	m_bUserPipes = false;


	if( _pNextProc->getProcessorType()== HoldAreaProc || _pNextProc->getProcessorType()== LineProc ||	// for line proc, for now only get a point
		_pNextProc->getProcessorType() == BaggageProc )
	{
		if(_pNextProc->inConstraintLength()<=0)
		{
			ptTo = _pNextProc->GetServiceLocation();
			m_nextHoldAiearPoint = ptTo;
			m_nextHoldAiearPoTag = true;
		}
	}

	MiscDatabase* pMiscDatabase = m_pTerm->miscData;
	MiscProcessorData* pMiscProcessorData = pMiscDatabase->getDatabase(_pNextProc->getProcessorType());
	MiscDataElement* pMiscDataElement = (MiscDataElement*)pMiscProcessorData->getEntryPoint(*_pNextProc->getID());
	MiscData* pMiscData = pMiscDataElement->getData();
	MiscProcessorIDList* pMiscProcessorIDList = pMiscData->getWaitAreaList();

	int waitAreaCount = pMiscProcessorIDList->getCount();
	MiscProcessorIDWithOne2OneFlag* pIDWithOne2One = NULL;
	for(int i=0; i<waitAreaCount; i++)
	{
		if((*pMiscProcessorIDList->getItem(i)) == (*m_pProcessor->getID()))
		{
			pIDWithOne2One = (MiscProcessorIDWithOne2OneFlag*)(pMiscProcessorIDList->getItem(i));
			break;
		}
		if(pMiscProcessorIDList->getItem(i)->idFits(*m_pProcessor->getID()))
		{
			pIDWithOne2One = (MiscProcessorIDWithOne2OneFlag*)(pMiscProcessorIDList->getItem(i));
		}
	}
	ASSERT(pIDWithOne2One != NULL);
	if(pIDWithOne2One == NULL)
		return;

	std::vector<int> vPipeList1;
	if(pIDWithOne2One->GetTypeOfUsingPipe() == USE_PIPE_SYSTEM)
	{
		WalkAlongShortestPath( _pNextProc,ptTo, _curTime );
		return;
	}
	else											// use user pipe
	{
		m_bUserPipes = true;
		vPipeList1 = pIDWithOne2One->GetPipeVector();
	}

	int nPipeCount = vPipeList1.size();
	if( nPipeCount == 0 )
		return;

	m_nextHoldAiearPoTag = false;
	if(_pNextProc->getProcessorType()== HoldAreaProc || _pNextProc->getProcessorType()== LineProc
		|| _pNextProc->getProcessorType() == BaggageProc )
	{
		if(_pNextProc->inConstraintLength()<=0)
		{
			ptTo = _pNextProc->GetServiceLocation();
			m_nextHoldAiearPoint = ptTo;
			m_nextHoldAiearPoTag = true;
		}
	}

	std::vector<int> vPipeList2;
	for( int i=0; i<nPipeCount; i++ )
	{
		CPipe* pPipe = m_pTerm->m_pPipeDataSet->GetPipeAt( vPipeList1[i] );
		if( pPipe->GetZ() == ptFrom.getZ() )
		{
			vPipeList2.push_back( vPipeList1[i] );
		}
	}
	nPipeCount = vPipeList2.size();
	if( nPipeCount == 0 )
		return;

	writeLogEntry( _curTime, false ); // on leave server.

	// get the index and intersetion of the source processor with pipe.
	int nOldState = m_pPerson->getState();

	CPointToPipeXPoint entryPoint;
	CPointToPipeXPoint exitPoint;
	std::vector<CMobPipeToPipeXPoint> vMidPoint;	// num count should be nPipeCount - 1

	if(m_pProcessor->outConstraintLength()>0)
		ptFrom = m_pProcessor->outConstraint(m_pProcessor->outConstraintLength()-1);

	CPipe* pPipe1 = NULL;
	CPipe* pPipe2 = NULL;
	for(int i=0; i<nPipeCount; i++ )
	{
		if( i == 0 )
		{
			pPipe1 = m_pTerm->m_pPipeDataSet->GetPipeAt( vPipeList2[0] );
			entryPoint = pPipe1->GetIntersectionPoint( ptFrom );

			if( nPipeCount == 1 )
			{
				exitPoint = pPipe1->GetIntersectionPoint( ptTo );
			}
			else
			{
				pPipe2 = m_pTerm->m_pPipeDataSet->GetPipeAt( vPipeList2[1] );
				CMobPipeToPipeXPoint midPt;
				if( pPipe1->GetIntersectionPoint( pPipe2, entryPoint, midPt ) )
				{
					vMidPoint.push_back( midPt );
				}
				else
				{
					//throw new NoValidFlowError (m_pPerson);
					kill(_curTime);
					throw new ARCPipeNotIntersectError( pPipe1->GetPipeName(),pPipe2->GetPipeName(),"", ClacTimeString(_curTime));
				}
			}
		}
		else if( i == nPipeCount - 1 )
		{
			exitPoint = pPipe1->GetIntersectionPoint( ptTo );
			vMidPoint[vMidPoint.size()-1].SetOutInc( exitPoint );
		}
		else
		{
			pPipe2 = m_pTerm->m_pPipeDataSet->GetPipeAt( vPipeList2[i+1] );
			CMobPipeToPipeXPoint midPt;
			if( pPipe1->GetIntersectionPoint( pPipe2, vMidPoint[vMidPoint.size()-1], midPt ) )
			{
				vMidPoint[vMidPoint.size()-1].SetOutInc( midPt );
				vMidPoint.push_back( midPt );
			}
			else
			{
				//throw new NoValidFlowError (m_pPerson);
				kill(_curTime);
				throw new ARCPipeNotIntersectError( pPipe1->GetPipeName(),pPipe2->GetPipeName(),"", ClacTimeString(_curTime));
			}
		}
		pPipe1 = pPipe2;
	}

	ElapsedTime eventTime = _curTime;
	setState( WalkOnPipe );

	// process entry point
	CPipe* pPipe = m_pTerm->m_pPipeDataSet->GetPipeAt( vPipeList2[0] );

	//////////////////////////////////////////////////////////////////////////
	//begin  handle barrier
	Point old_destination = m_ptDestination;
	//m_ptDestination = entryPoint;
	setDestination( entryPoint );
	Point nextPoint = m_ptDestination;
	// determines whether Person can travel a direct route to his
	// destination. if not, an intermediary destination will be assigned
	bool bHasClearPath = true;
	while (!hasClearPath(_curTime))
	{
		eventTime += moveTime();
		m_pPerson->setState(MoveToServer);
		writeLogEntry (eventTime, false);
		setDestination (nextPoint);
		setState( WalkOnPipe );
		bHasClearPath = false;
	}

	//set back the old destination
	if(bHasClearPath)
	{
		setDestination(old_destination);
	}

	PTONSIDEWALK pointList;
	int nPercent = random( 100 );
	int nMidPoint = vMidPoint.size();

	if( nMidPoint == 0 )
	{	
		m_bUserPipes = false;
		pPipe->GetPointListForLog( vPipeList2[0],entryPoint, exitPoint, nPercent,pointList );

		WritePipeLogs( pointList, eventTime, getEngine()->GetFireOccurFlag());		
		pointList.clear();
	}
	else
	{
		pPipe->GetPointListForLog( vPipeList2[0],entryPoint, vMidPoint[0], nPercent,pointList );
		WritePipeLogs( pointList, eventTime , getEngine()->GetFireOccurFlag());

		pointList.clear();
		// process mid point
		int i=1;
		for( ; i<nMidPoint; i++ )
		{
			pPipe = m_pTerm->m_pPipeDataSet->GetPipeAt( vPipeList2[i] );
			if( vMidPoint[i-1].OrderChanged() )
				nPercent = 100 - nPercent;

			pPipe->GetPointListForLog( vPipeList2[0],vMidPoint[i-1], vMidPoint[i], nPercent ,pointList );

			WritePipeLogs( pointList, eventTime, getEngine()->GetFireOccurFlag());
			pointList.clear();
		}
		m_bUserPipes = false;

		// process exit point
		pPipe = m_pTerm->m_pPipeDataSet->GetPipeAt( vPipeList2[nPipeCount-1] );
		if( vMidPoint[i-1].OrderChanged() )
			nPercent = 100 - nPercent;
		pPipe->GetPointListForLog( vPipeList2[0],vMidPoint[nMidPoint-1], exitPoint, nPercent,pointList );

		WritePipeLogs( pointList, eventTime, getEngine()->GetFireOccurFlag() );
		pointList.clear();
	}

	_curTime = eventTime;	
	
}
/*
// Creat one CPointToPipeXPoint for the first point.
// Creat one CPointToPipeXPoint for the last point.
// Creat nNumPipe - 1 CMobPipeToPipeXPoint for any intersection point for pipes.

1. for the first point
keep pipe0->GetIntersectionPoint( const Point& _targetPoint );

2. for the last point
keep pipe(n-1)->GetIntersectionPoint( const Point& _targetPoint ); // need set up order information.
update previous index inc if there is any

3. for the pipe0 and pipe1:
push pipe0.GetIntersectionPoint( pipe1, prevPoint );
update previous index inc if there is any
*/
void TerminalMobElementBehavior::ProcessPipe( Processor* _pNextProc, 
						 ElapsedTime& _curTime,
						 CFlowItemEx* _pPrevFlowItem )
{
	if( _pNextProc == NULL )
		return;

	if(m_pFlowList != NULL)
	{
		if( m_pFlowList->getProbabilityType() != PROCESSORWITHPIPE )
			return;
	}

	Point ptFrom = location;

	int iCurFloor = (int)(location.getZ() / SCALE_FACTOR);
	if(m_pProcessor->getIndex()==START_PROCESSOR_INDEX)
	{
		iCurFloor = _pNextProc->getFloor() /(int) SCALE_FACTOR;
	}
	if(_pNextProc->getIndex() == END_PROCESSOR_INDEX)
	{
		iCurFloor = 0;
	}

	std::vector<int> vPipeList1;

	// get pipe list
	CFlowItemEx vPipeIndexList;
	if( _pPrevFlowItem  )
	{
		vPipeIndexList = *_pPrevFlowItem;
	}
	else
	{
		std::vector<CFlowItemEx>& pipeIndexList = ( (CProcessorDistributionWithPipe*)m_pFlowList )->GetPipeVector();
		if( !pipeIndexList.empty() )
		{
			vPipeIndexList = pipeIndexList[m_pFlowList->GetCurGroupIndex()];
		}		
	}
	
	/*m_nextHoldAiearPoTag = false;*/
	Point ptTo = GetPipeExitPoint(_pNextProc,iCurFloor,ClacTimeString(_curTime),ptFrom, &vPipeIndexList);
	//Point ptTo = _pNextProc->GetPipeExitPoint(iCurFloor,ClacTimeString(_curTime),ptFrom,this) ;  //Get Entry
	//save the destination point for processing congestion pipe
	m_ptOldDest = ptTo;

	if( vPipeIndexList.GetTypeOfUsingPipe() == 1 )	// use auto pipe system
	{
		m_bUserPipes = false;


 		if( _pNextProc->getProcessorType()== HoldAreaProc || _pNextProc->getProcessorType()== LineProc ||	// for line proc, for now only get a point
 			_pNextProc->getProcessorType() == BaggageProc )
 		{
 			if(_pNextProc->inConstraintLength()<=0)
 			{
 				ptTo = _pNextProc->GetServiceLocation();
 				m_nextHoldAiearPoint = ptTo;
 				m_nextHoldAiearPoTag = true;
 			}
 		}

		WalkAlongShortestPath( _pNextProc,ptTo, _curTime );
		return;
	}

	// else use user pipe
	m_bUserPipes = true;
	vPipeList1 = vPipeIndexList.GetPipeVector();

	int nPipeCount = vPipeList1.size();
	if( nPipeCount == 0 )
		return;

	m_nextHoldAiearPoTag = false;
	if(_pNextProc->getProcessorType()== HoldAreaProc || _pNextProc->getProcessorType()== LineProc
		|| _pNextProc->getProcessorType() == BaggageProc )
	{
		if(_pNextProc->inConstraintLength()<=0)
		{
			ptTo = _pNextProc->GetServiceLocation();
			m_nextHoldAiearPoint = ptTo;
			m_nextHoldAiearPoTag = true;
		}
	}

	std::vector<int> vPipeList2;
	for( int i=0; i<nPipeCount; i++ )
	{
		CPipe* pPipe = m_pTerm->m_pPipeDataSet->GetPipeAt( vPipeList1[i] );
		if( pPipe->GetZ() == ptFrom.getZ() )
		{
			vPipeList2.push_back( vPipeList1[i] );
		}
	}
	nPipeCount = vPipeList2.size();
	if( nPipeCount == 0 )
		return;

	writeLogEntry( _curTime, false ); // on leave server.

	// get the index and intersetion of the source processor with pipe.
	int nOldState = m_pPerson->getState();

	CPointToPipeXPoint entryPoint;
	CPointToPipeXPoint exitPoint;
	std::vector<CMobPipeToPipeXPoint> vMidPoint;	// num count should be nPipeCount - 1

	if(m_pProcessor->outConstraintLength()>0)
		ptFrom = m_pProcessor->outConstraint(m_pProcessor->outConstraintLength()-1);

	CPipe* pPipe1 = NULL;
	CPipe* pPipe2 = NULL;
	
	for(int ii=0; ii<nPipeCount; ii++ )
	{
		if( ii == 0 )
		{
			pPipe1 = m_pTerm->m_pPipeDataSet->GetPipeAt( vPipeList2[0] );
			entryPoint = pPipe1->GetIntersectionPoint( ptFrom );

			if( nPipeCount == 1 )
			{
				exitPoint = pPipe1->GetIntersectionPoint( ptTo );
			}
			else
			{
				pPipe2 = m_pTerm->m_pPipeDataSet->GetPipeAt( vPipeList2[1] );
				CMobPipeToPipeXPoint midPt;
				if( pPipe1->GetIntersectionPoint( pPipe2, entryPoint, midPt ) )
				{
					vMidPoint.push_back( midPt );
				}
				else
				{
					//throw new NoValidFlowError (m_pPerson);
					kill(_curTime);
					throw new ARCPipeNotIntersectError( pPipe1->GetPipeName(),pPipe2->GetPipeName(),"", ClacTimeString(_curTime));
				}
			}
		}
		else if( ii == nPipeCount - 1 )
		{
			exitPoint = pPipe1->GetIntersectionPoint( ptTo );
			vMidPoint[vMidPoint.size()-1].SetOutInc( exitPoint );
		}
		else
		{
			pPipe2 = m_pTerm->m_pPipeDataSet->GetPipeAt( vPipeList2[ii+1] );
			CMobPipeToPipeXPoint midPt;
			if( pPipe1->GetIntersectionPoint( pPipe2, vMidPoint[vMidPoint.size()-1], midPt ) )
			{
				vMidPoint[vMidPoint.size()-1].SetOutInc( midPt );
				vMidPoint.push_back( midPt );
			}
			else
			{
				//throw new NoValidFlowError (m_pPerson);
				kill(_curTime);
				throw new ARCPipeNotIntersectError( pPipe1->GetPipeName(),pPipe2->GetPipeName(),"", ClacTimeString(_curTime));
			}
		}
		pPipe1 = pPipe2;
	}

	ElapsedTime eventTime = _curTime;
	setState( WalkOnPipe );

	// process entry point
	CPipe* pPipe = m_pTerm->m_pPipeDataSet->GetPipeAt( vPipeList2[0] );

	//////////////////////////////////////////////////////////////////////////
	//begin  handle barrier
	Point old_destination = m_ptDestination;
	//m_ptDestination = entryPoint;
	setDestination( entryPoint );
	Point nextPoint = m_ptDestination;
	// determines whether Person can travel a direct route to his
	// destination. if not, an intermediary destination will be assigned
	bool bHasClearPath = true;
	while (!hasClearPath(_curTime))
	{
		eventTime += moveTime();
		m_pPerson->setState(MoveToServer);
		writeLogEntry (eventTime, false);
		setDestination (nextPoint);
		setState( WalkOnPipe );
		bHasClearPath = false;
	}
	
	//set back the old destination
	if(bHasClearPath)
	{
		setDestination(old_destination);
	}

	PTONSIDEWALK pointList;
	int nPercent = random( 100 );
	int nMidPoint = vMidPoint.size();

	if( nMidPoint == 0 )
	{	
		m_bUserPipes = false;
		pPipe->GetPointListForLog( vPipeList2[0],entryPoint, exitPoint, nPercent,pointList );

		WritePipeLogs( pointList, eventTime, getEngine()->GetFireOccurFlag());		
		pointList.clear();
	}
	else
	{
		pPipe->GetPointListForLog( vPipeList2[0],entryPoint, vMidPoint[0], nPercent,pointList );
		WritePipeLogs( pointList, eventTime , getEngine()->GetFireOccurFlag());

		pointList.clear();
		// process mid point 
		int i=1;
		for(; i<nMidPoint; i++ )
		{
			pPipe = m_pTerm->m_pPipeDataSet->GetPipeAt( vPipeList2[i] );
			if( vMidPoint[i-1].OrderChanged() )
				nPercent = 100 - nPercent;

			pPipe->GetPointListForLog( vPipeList2[0],vMidPoint[i-1], vMidPoint[i], nPercent ,pointList );

			WritePipeLogs( pointList, eventTime, getEngine()->GetFireOccurFlag());
			pointList.clear();
		}
		m_bUserPipes = false;

		// process exit point
		pPipe = m_pTerm->m_pPipeDataSet->GetPipeAt( vPipeList2[nPipeCount-1] );
		if( vMidPoint[i-1].OrderChanged() )
			nPercent = 100 - nPercent;
		pPipe->GetPointListForLog( vPipeList2[0],vMidPoint[nMidPoint-1], exitPoint, nPercent,pointList );

		WritePipeLogs( pointList, eventTime, getEngine()->GetFireOccurFlag() );
		pointList.clear();
	}

	_curTime = eventTime;
}
void TerminalMobElementBehavior::processBillboard(ElapsedTime& p_time)
{

	ProcessorArray billboardList;
	m_pTerm->procList->GetBillboardProcessorList( billboardList );
	if(billboardList.getCount() == 0 )
		return;

	//test that the pax whether in the same floor
	int nFloorFrom = (int)(location.getZ() / SCALE_FACTOR);
	int nFloorTo = (int)(m_ptDestination.getZ() / SCALE_FACTOR);

	if (nFloorFrom != nFloorTo) // the two points don't in the same floor,cann't arrac
		return;

	ElapsedTime _curTime = p_time;
	//test every billboard proc

	for (int i=0 ;i < billboardList.getCount(); i++)
	{
		BillboardProc *pBillboardProc = (BillboardProc *)billboardList.getItem( i );
		ASSERT(pBillboardProc);



		double distanceToBillboard = 0.0;//the distance between the middle point of billboard and the middle point of the trace of the pax waked through 
		double dAngel = 0.0;//the angel of the pax's direction and the billboard orientation
		Point stopPoint; 
		Point directionPoint;
		if (pBillboardProc->GetIntersectionWithLine(location,m_ptDestination,stopPoint,directionPoint,
			distanceToBillboard,dAngel))
		{
			//write processor log ,indicated the pax pass through billboard exposure area
			pBillboardProc->writeLogEvent (m_pPerson, p_time, WalkThroughBillboard);

			//has a intersection


			MiscProcessorData* pMiscData = m_pTerm->miscData->getDatabase( (int)BillboardProcessor );
			const ProcessorID* pid = pBillboardProc->getID();
			int idx = pMiscData->findBestMatch( *pid );
			if( idx== -1)
				return;
			//get wait processor list
			MiscBillboardData* pMiscBillboardData = (MiscBillboardData*)pMiscData->getEntry( idx );

			ASSERT(pMiscBillboardData);

			ProcessorIDList *pIDlist = pMiscBillboardData->GetBillBoardLinkedProcList();

			for (int ii =0; ii< pIDlist->getCount(); ii++)
			{
				for (int jj= 0; jj < static_cast<int>(m_vNeedVisitLinkedProcessor.size()); jj++)
				{
					const ProcessorID* ptempIDBill =  pIDlist->getID(ii);
					ProcessorID ptempIDlink = m_vNeedVisitLinkedProcessor[jj];
					if (ptempIDBill->idFits(ptempIDlink)||ptempIDlink.idFits(*ptempIDBill))
						return;
				}
			}
			//time left to departure
			ElapsedTime timeLeftToDep(0L) ;
			int nDepFlightIdx = m_pPerson->m_logEntry.getDepFlight();
			if( nDepFlightIdx >= 0 )//pax departure
			{
				Flight *pFlight = getEngine()->m_simFlightSchedule.getItem( nDepFlightIdx );
				timeLeftToDep = pFlight->getDepTime() - _curTime ;

				int nMin =pMiscBillboardData->getTimeRemaining();
				if (timeLeftToDep < ElapsedTime((long)nMin*60))
					return;

			}

#define PI 3.1415926
			//estimate that the pax pause and watch the billboard
			if (dAngel > (90/180)*PI) //angel larger than 90 degree
				return ;


			//coefficient
			int nPropensityCoefficient = 0;// = pBillboardProc->getCoefficient();
			int nBuyCoefficient = 0;

			pMiscBillboardData->FindCoefficient(m_pPerson->getType(),nPropensityCoefficient,nBuyCoefficient);
			//			if (bNoticedBillboard) //notice the billboard
			if(random(100) < nPropensityCoefficient)
			{
				int nPreState =  m_pPerson->getState();
				Point tempDestination = m_ptDestination;

				Processor *TempProcessor = m_pProcessor;
				m_pProcessor = pBillboardProc;
				//write processorlog
				m_pProcessor->writeLogEvent (m_pPerson, p_time, BeginService);

				setState(StopByBillboard);
				setDestination(stopPoint);
				p_time	= p_time+ moveTime();
				writeLogEntry( p_time,false );


				setDestination(directionPoint);
				p_time	= p_time+ moveTime();
				writeLogEntry( p_time,false );

				int nTimeStop = pMiscBillboardData->getTimeStop();
				if (nTimeStop <0)
				{
					nTimeStop = 30;//default 30s
				}
				if(nTimeStop == 0)
					p_time = p_time +ElapsedTime(0L);
				else
					p_time = p_time +ElapsedTime((long)random(nTimeStop)); //random stop for a few seconds
				writeLogEntry(p_time,false);
				m_pProcessor->writeLogEvent (m_pPerson, p_time, CompleteService);
				setDestination(tempDestination);
				setState(nPreState);
				m_pProcessor = TempProcessor;
				//set linked destination to linked proc vector
				if (random(100) <nBuyCoefficient)
				{
					for (int nID =0; nID< pIDlist->getCount(); nID++)
					{
						CString strID = pIDlist->getID(nID)->GetIDString();
						Processor * pProc =  m_pTerm->procList->getProcessor(strID);
						if (pProc )
						{
							m_vNeedVisitLinkedProcessor.push_back(*(pProc->getID()));
						}
					}
				}

			}

		}
	}
}

bool TerminalMobElementBehavior::FindBillboardLinkedProcessorFromDestination(Processor **destProc,ElapsedTime _curTime, bool _bStationOnly)
{
	int nNeedVistCount = static_cast<int>(m_vNeedVisitLinkedProcessor.size());
	if (nNeedVistCount<1)
		return false;

	ProcessorArray canSelectProc;
	bool bGerentGreetEventOrWait = false;
	if(FindAllPossibleDestinationProcs(m_vNeedVisitLinkedProcessor,canSelectProc,_bStationOnly,_curTime,bGerentGreetEventOrWait) == false)
		return false;
	int nCount = canSelectProc.getCount();
	if (nCount < 1)
		return false;

	*destProc = canSelectProc[random(nCount)];


	for (int i =0;i <nNeedVistCount; i++)
	{
		if (m_vNeedVisitLinkedProcessor[i].idFits(*((*destProc)->getID())))
		{
			m_vNeedVisitLinkedProcessor.erase(m_vNeedVisitLinkedProcessor.begin()+i);
			break;
		}
	}
	return true;
}
//Returns true if m_pProcessor m_pPerson->getType() is not dependent source; else if one of processorer is available; zero otherwise.
int TerminalMobElementBehavior::noExceptions (ProcessorArray *p_array)
{
	Processor *proc;

	if (m_pProcessor->getProcessorType() != DepSourceProc)
		return 1;

	for (int i = 0; i < p_array->getCount(); i++)
	{
		proc = p_array->getItem(i);
		if (proc->isAvailable())
			return 1;
	}
	// error has occurred: all sinks are occupied
	return 0;

}
void TerminalMobElementBehavior::changeQueue (Processor *newProc, ElapsedTime p_time)
{
	// write log entry m_in current position to start movement
	setState (JumpQueue);
	wait (FALSE);
	m_pPerson->setCurrentPosition (p_time);

	m_pProcessor->writeLogEvent (m_pPerson, p_time, DecreaseQueue);
	m_pProcessor->removePerson (m_pPerson);

	m_pProcessor = newProc;
	m_pProcessor->addPerson (m_pPerson);
	m_pProcessor->AddRecycleFrequency(m_pPerson);
	// force next state to MoveToQueue

	m_pPerson->setState(MoveAlongInConstraint);
	processGeneralMovement (p_time);
}

bool MobileElementCompare(MobileElement* pMob1, MobileElement* pMob2)
{
	return *pMob1 < *pMob2 ? true : false;
}

void TerminalMobElementBehavior::pickupBag( std::vector<Person*>_vBags, const ElapsedTime& _pickAtTime )
{
	//m_pProcessor->writeLogEvent( _vBags[0], _pickAtTime, BagLeaveBaggageProc );
	m_pProcessor->writeLogEvent( m_pPerson, _pickAtTime, PaxLeaveBaggageProc );
	PaxTerminalBehavior* spTerminalBehavior = (PaxTerminalBehavior*)m_pPerson->getBehavior(MobElementBehavior::TerminalBehavior);

	if (spTerminalBehavior == NULL)
		return;
	
	std::sort(_vBags.begin(),_vBags.end(),MobileElementCompare);
	spTerminalBehavior->AttachBags( _vBags, _pickAtTime );
	generateEvent (_pickAtTime,false ); 

	int index = -1;
	if(m_pPerson->getType().isArrival()||m_pPerson->getType().isTurround())
		index = m_pPerson->m_logEntry.getArrFlight();
	if(index>=0) 
	{
		Flight *flight = getEngine()->m_simFlightSchedule.getItem(index);
		if(flight->getLastTime() < _pickAtTime) 
			flight->setLastTime(_pickAtTime);
	}
}


// It tests current path for intersecting barriers. If any, the Person will be guided around the ends of them
// m_pPerson function has been optimized to reduce the number of calculations as much as possible. Modify with caution!!!!
// Returns TRUE only if a clear path all the way to the eventual destination is found.
int TerminalMobElementBehavior::hasClearPath (const ElapsedTime& _curTime)
{
	if (getPoint().getZ() != m_ptDestination.getZ())
		return 1;

	assert( m_pTerm );

	Barrier *barrier;
	int i, flag = TRUE;      // found clear path
	ProcessorArray barrierList;    
	int barrierCount =0 ;


	if (m_pTerm->m_pSimParam->GetBarrierFlag() )
	{
		m_pTerm->procList->GetBarriersList( barrierList );
		barrierCount = barrierList.getCount();
	}

	ProcessorArray avoidQueuebarrierList;
	avoidQueuebarrierList.OwnsElements( TRUE );//make sure that delete temp created barrier

	AvoidFixedQueueIfNecessary( &barrierList, &avoidQueuebarrierList );
	//AvoidOverFlowQueueIfNecessary( &barrierList, &avoidQueuebarrierList );
	barrierCount += avoidQueuebarrierList.getCount();

	Point *DisablePointArray = new Point[ 2* barrierCount];//

	try
	{
		int  iDisableArrayCount =0;//
		do
		{
			for (i = 0; i < barrierCount; i++)
			{
				barrier = (Barrier *) barrierList.getItem (i);
				if (barrier->intersects (getPoint(), m_ptDestination))
				{
					Point NewDestination;//

					if (barrier->contains (getPoint()) ||
						barrier->contains (m_ptDestination))
						continue;
					iDisableArrayCount ++;
					DisablePointArray[iDisableArrayCount - 1] = m_ptDestination;

					NewDestination = barrier->getNextPoint (getPoint(), m_ptDestination);
					int iDisableTag =0;

					for(int kk =0;kk <iDisableArrayCount;kk++)
					{
						if( DisablePointArray[kk]==NewDestination )
						{
							iDisableTag =1;
							break;
						}
					}

					if(iDisableTag)
					{
						NewDestination = barrier->getNextWorsePoint(getPoint(),m_ptDestination);
					}

					setDestination (NewDestination);

					//setDestination (barrier->getNextPoint (location, destination));
					flag = FALSE;

					delete []DisablePointArray;
					return flag;
				}
			}

			// m_pPerson check will only allow a pax to continue when they find a
			// clear path to their current destination

			// when a deviating route is required, the new path must also be
			// tested for intersecting barriers. If any additional intersections
			// are found, the passenger must be guided around them as well
		} while (i != barrierCount);
	}
	catch (StringError* strErr)
	{
		long pID = m_pPerson->getID();
		ReleaseResource(_curTime);
		kill(_curTime);
		delete []DisablePointArray;
		// for throw exception
		CString szMobType = getPersonErrorMsg();
		CString strProcNameTmp = m_pProcessor ? m_pProcessor->getID()->GetIDString() : "";
		char strErrMsg[256] = {0};
		strErr->getMessage(strErrMsg);
		delete strErr;
		throw new ARCBarrierSystemError(szMobType, strProcNameTmp, strErrMsg, ClacTimeString(_curTime));
	}
	delete []DisablePointArray;
	return flag;
}
void TerminalMobElementBehavior::AvoidFixedQueueIfNecessary( ProcessorArray* _pBarrierList , ProcessorArray* _pNewCreateBarrier )
{
	if( m_pPreFlowList )
	{
		std::vector<CFlowItemEx>& vFlowCondition = ( (CProcessorDistributionWithPipe*)m_pPreFlowList )->GetPipeVector();
		ASSERT( m_pPreFlowList->GetCurGroupIndex() < m_pPreFlowList->GetDestCount() && m_pPreFlowList->GetCurGroupIndex() >=0 );
		if( vFlowCondition.empty() )
		{
			return;
		}
		const CFlowItemEx& flowCondition = vFlowCondition[m_pPreFlowList->GetCurGroupIndex()];
		if( flowCondition.GetAvoidFixQueue() <= 0 )
		{
			return;
		}

		int iProcCount = m_pTerm->procList->getProcessorCount();
		for( int i=0; i<iProcCount; ++i )
		{
			Processor* pProc = m_pTerm->procList->getProcessor( i );
			if( pProc->getFloor() == getPoint().getZ() )
			{
				if( ProcessorQueue* pQueue = pProc->GetProcessorQueue() )
				{
					if( pQueue->isFixed() == 'Y' )
					{
						Barrier* pBarrier = new Barrier;
						pBarrier->initServiceLocation( pQueue->cornerCount(), pQueue->corner()->getPointList() );
						_pBarrierList->addItem( pBarrier );
						_pNewCreateBarrier->addItem( pBarrier );
					}
				}
			}			
		}


	}
}

// filter out all the processors which is a flow belt processor( a subtype of conveyor processor )
void TerminalMobElementBehavior::FilterOutFlowBeltProcs( ProcessorArray* _pDestProcs )
{
	int nProcCount = _pDestProcs->getCount();
	for( int i=nProcCount-1; i>=0; i-- )
	{
		Processor* pProc = _pDestProcs->getItem( i );
		if( pProc->getProcessorType() == ConveyorProc )
		{
			if ( ((Conveyor*)pProc)->GetSubConveyorType() == FLOW_BELT )
			{
				_pDestProcs->removeItem( i );
			}
		}			
	}
}

// filter out processor which is not satisfied with flow condition defined in pax flow
void TerminalMobElementBehavior::FilterOutProcessorByFlowCondition( ProcessorArray* _pDestProcs ,const ElapsedTime& _curTime )
{
	if( m_pFlowList )
	{
		std::vector<CFlowItemEx>& vFlowCondition = ( (CProcessorDistributionWithPipe*)m_pFlowList )->GetPipeVector();
		ASSERT( m_pFlowList->GetCurGroupIndex() < m_pFlowList->GetDestCount() && m_pFlowList->GetCurGroupIndex() >=0 );
		if( vFlowCondition.empty() )
		{
			return;
		}
		const CFlowItemEx& flowCondition = vFlowCondition[m_pFlowList->GetCurGroupIndex()];
		long lMaxQueueLength = flowCondition.GetMaxQueueLength();
		long lMaxWaitMins = flowCondition.GetMaxWaitMins();
		long lSkipTime =flowCondition.GetMaxSkipTime();


		if( lMaxQueueLength > 0 )
		{
			int nProcCount = _pDestProcs->getCount();
			for( int i=nProcCount-1; i>=0; i-- )
			{
				Processor* pProc = _pDestProcs->getItem( i );

				if( ProcessorQueue* pProcQueue = pProc->GetProcessorQueue() )
				{
					//int iQueueLength = pProcQueue->getTotalQueueLength();

					if( pProc->GetOccupants()->getCount() >= lMaxQueueLength )
					{
						_pDestProcs->removeItem( i );
					}
				}
			}	
		}

		if( lMaxWaitMins > 0 )
		{
			int nProcCount = _pDestProcs->getCount();
			for( int i=nProcCount-1; i>=0; i-- )
			{
				Processor* pProc = _pDestProcs->getItem( i );

				if( ProcessorQueue* pProcQueue = pProc->GetProcessorQueue() )
				{
					if( pProcQueue->hasWait() )
					{
						MobileElementList* pWaitList = pProcQueue->GetWaitList();
						Person* pHeadPerson = (Person*)pWaitList->getItem( 0 );
						ElapsedTime timeEntryQueue = getEntryQueueTime();
						if( (_curTime - timeEntryQueue ) >= ElapsedTime(lMaxWaitMins*60l) )
						{
							_pDestProcs->removeItem( i );
						}
					}						
				}
			}
		}
	}	
}

void TerminalMobElementBehavior::KeepOnlyStation( ProcessorArray* _pDestProcs )
{
	int nProcCount = _pDestProcs->getCount();
	for( int i=nProcCount-1; i>=0; i-- )
	{
		Processor* pProc = _pDestProcs->getItem( i );
		if( pProc->getProcessorType() != IntegratedStationProc )
		{
			_pDestProcs->removeItem( i );
		}
		else
		{
			if( !m_pTerm->m_pAllCarSchedule->IfHaveScheduleBetweenStations((IntegratedStation*)m_pProcessor ,(IntegratedStation*)pProc ) )
			{
				_pDestProcs->removeItem( i );
			}		
		}		
	}
}


void TerminalMobElementBehavior::WritePipeLogs( PTONSIDEWALK& _vPointList, ElapsedTime& _eventTime,  bool _bNeedCheckEvacuation )
{
	m_vPipePointList.clear();
	for (PTONSIDEWALK::iterator iter = _vPointList.begin(); iter != _vPointList.end(); iter++)
	{
		PipePointInformation pipeInfor;
		if (m_vPipePointList.empty())
		{
			pipeInfor.m_nPrePipe = -1;
		}
		else
		{
			pipeInfor.m_nPrePipe = m_vPipePointList.back().m_nCurPipe;
		}
		
		pipeInfor.pt = iter->GetPointOnSideWalk();
		pipeInfor.m_nCurPipe = iter->GetPipeIdx();
		m_vPipePointList.push_back(pipeInfor);
	}

	setState(WalkOnPipe);
	generateEvent(_eventTime,false);

	if(!m_bEvacuationWhenInPipe && !m_bUserPipes)
		return;

	CTimePointOnSideWalk tempPoint, prePoint;
	ElapsedTime timeFireTime = getEngine()->GetFireEvacuation();

	if( _bNeedCheckEvacuation && m_bEvacuationWhenInPipe)
		return;

	int iMovingSideWalkIdx = -1;
	int iPipeIndex = -1;
	prePoint.SetOnSideWalkFlag( false );
	prePoint.SetPoint( location );
	
	ElapsedTime tTime = _eventTime;
	for( UINT m=0; m<_vPointList.size(); m++ )
	{
		tempPoint = _vPointList[m];

		setDestination( tempPoint);
		
		if( tempPoint.GetOnSideWalkFlag() )
		{
			if( tempPoint.GetSideWalkIdx() != iMovingSideWalkIdx )
			{
				setDestination( tempPoint);

				processBillboard(_eventTime);

				_eventTime += moveTime();
				//				writeLogEntry( _eventTime );
				iMovingSideWalkIdx = tempPoint.GetSideWalkIdx();
			}

			_eventTime += tempPoint.GetTime();	
		}
		else
		{
			//if when firing, the person is in pipe.
			if( _bNeedCheckEvacuation && _eventTime + moveTime() >= timeFireTime )
			{
				Point strandPoint;
				if( prePoint.GetOnSideWalkFlag() )
				{
					strandPoint = tempPoint.GetPoint();
				}
				else
				{
					Point vPoint( prePoint.GetPoint(), tempPoint.GetPoint() );
					double detaTime = ( timeFireTime - _eventTime ).asSeconds();
					double dLength = detaTime * m_pPerson->speed;
					vPoint.length( dLength );

					strandPoint= prePoint.GetPoint() + vPoint;
				}

				strandPoint.setZ( prePoint.GetPoint().getZ() );
				setDestination( strandPoint );
				if (getEngine()->IsLandSel())
				{
					bool bCreateNew = (tempPoint.GetPipeIdx() != iPipeIndex ? true : false);
					if (tempPoint.GetPipeIdx() != iPipeIndex)
					{
						CPipe* pPipe = m_pTerm->m_pPipeDataSet->GetPipeAt( tempPoint.GetPipeIdx() );
						pPipe->WritePipePax(getEngine()->GetLandsideSimulation(),getLocation(),strandPoint.GetPoint(),m_pPerson->getID(),_eventTime,timeFireTime + 62l);
					}
				}
				
				writeLogEntry( timeFireTime, false );
				long lReflectTime = random( 60 );
				writeLogEntry( timeFireTime + lReflectTime + 2l, false );

				setState( EvacuationFire );
				SetFireEvacuateFlag( true );
				generateEvent( timeFireTime + lReflectTime + 2l ,false);

				
				m_bEvacuationWhenInPipe = true;
				m_bUserPipes = false;
				break;
			}
			else
			{
				processBillboard(_eventTime);
			}

			_eventTime += moveTime();
		}		
		writeLogEntry( _eventTime, false );
		//write landside crosswalk
		{
			if (getEngine()->IsLandSel())
			{
				bool bCreateNew = (tempPoint.GetPipeIdx() != iPipeIndex ? true : false);
				if (tempPoint.GetPipeIdx() != iPipeIndex)
				{
					CPipe* pPipe = m_pTerm->m_pPipeDataSet->GetPipeAt( tempPoint.GetPipeIdx() );
					pPipe->WritePipePax(getEngine()->GetLandsideSimulation(),getLocation(),tempPoint.GetPoint(),m_pPerson->getID(),tTime,_eventTime);
				}
				tTime = _eventTime;
			}
			
		}
		
		prePoint = tempPoint;
	}
}

/************************************************************************
FUNCTION: Find all possible converge proc from the destination group.
IN		:_vGreetingPlace,all of the greeting proc
_lAttachedID,person id,the person want to converge
_bStationOnly,
_curTime
OUT		:canSelectProc, all the procs that can be choiced                                                                      
/************************************************************************/
bool TerminalMobElementBehavior::FindGreetingProcFromDestination( std::vector<ProcessorID>& _vGreetingPlace , ProcessorArray &canSelectProc, long _lAttachedID , bool _bStationOnly, ElapsedTime _curTime )
{
	m_bGerentGreetEventOrWait = false;
	m_pWantToGreetPerson = NULL;
	//	std::vector<Processor*>vPossibleDestProcessor;
	if(FindAllPossibleDestinationProcs(_vGreetingPlace,canSelectProc,_bStationOnly,_curTime,m_bGerentGreetEventOrWait) == false)
		return false;
	if (m_bGerentGreetEventOrWait == true)
		return true;

	//	Processor** _pProc;
	// for throw exception
	//	CString szMobType = getPersonErrorMsg();
	//	CString strProcNameTmp = m_pProcessor ? m_pProcessor->getID()->GetIDString() : "";
	//	//
	//
	//	const ProcessorID *nextGroup;

	//    ProcessorArray destProcs;
	//	CProcessorDestinationList* tempPair = NULL;
	//
	//		// no proc can be selected becuause of the roster
	//	bool bRosterReason = false;
	//	// get all possible dest processors, ( opening and can server and.....)
	//	if( m_pSubFlow == NULL )	// is moving in normal flow
	//	{
	//		nextGroup = m_pFlowList->getDestinationGroup (RANDOM) ;
	//		do 
	//		{	
	//			CString strProcName = nextGroup->GetIDString();
	//			if( m_pTerm->m_pSubFlowList->IfProcessUnitExist( strProcName ) )
	//				continue;
	//
	//			m_pTerm->procList->buildArray (nextGroup, &destProcs,
	//
	//	            m_pPerson->getType(), m_nGate, NeedCheckLeadToGate(),m_nBagCarosel, FALSE, m_pProcessor, _curTime, m_pTerm ,bRosterReason,m_iDestFloor, m_pDiagnoseInfo);
	//
	//			int iDestCount = destProcs.getCount();
	//	        // if none open, generate list of closed processors
	//	        if (iDestCount == 0)
	//	            m_pTerm->procList->buildArray (nextGroup, &destProcs,
	//	                m_pPerson->getType(), m_nGate,NeedCheckLeadToGate(), m_nBagCarosel, TRUE, m_pProcessor, _curTime, m_pTerm, bRosterReason,m_iDestFloor, m_pDiagnoseInfo);
	//
	//			FilterOutProcessorByFlowCondition( &destProcs , _curTime );
	//
	//			if( _bStationOnly )
	//				KeepOnlyStation( &destProcs );
	//
	//			iDestCount = destProcs.getCount();
	//			for( int i=0; i<iDestCount; ++i )
	//			{
	//				vPossibleDestProcessor.push_back( destProcs.getItem( i ) );
	//			}
	//			
	//			
	//		}while ((nextGroup = m_pFlowList->getDestinationGroup (SEQUENTIAL)) != NULL);
	//	}
	//	else // m_pSubFlow!= NULL	// is moving in process flow
	//	{
	//		if( m_ProcessFlowCurID.isBlank() )	// the head of PROCESS flow
	//		{
	//			std::vector<ProcessorID> vRootProc;
	//			Processor* pProc = m_pSubFlow->GetNextProcessor(m_pPerson, _curTime);
	//			m_ProcessFlowCurID = *(pProc->getID());
	//		}
	//		else
	//		{
	//			tempPair = m_pSubFlow->GetInternalFlow()->GetFlowPairAt( m_ProcessFlowCurID );
	//			if( tempPair && tempPair->GetDestCount() > 0  )
	//			{
	//				m_ProcessFlowCurID = tempPair->GetDestProcAt(0).GetProcID();
	//			}
	//			else				// can not find the pair, the end of the PROCESS flow
	//			{
	//				return false;
	//			}
	//		}
	//
	//		// no proc can be selected becuause of the roster
	//		bool bRosterReason = false;
	//		// generate list of open processors
	//	    m_pTerm->procList->buildArray (&m_ProcessFlowCurID, &destProcs,
	//	        m_pPerson->getType(), m_nGate, NeedCheckLeadToGate(),m_nBagCarosel, FALSE, m_pProcessor, _curTime, m_pTerm,bRosterReason,m_iDestFloor, m_pDiagnoseInfo);
	//
	//	    // if none open, generate list of closed processors
	//	    if (destProcs.getCount() == 0)
	//	        m_pTerm->procList->buildArray (&m_ProcessFlowCurID, &destProcs,
	//	            m_pPerson->getType(), m_nGate,NeedCheckLeadToGate(), m_nBagCarosel, TRUE, m_pProcessor, _curTime, m_pTerm,bRosterReason,m_iDestFloor, m_pDiagnoseInfo);
	//
	//		FilterOutProcessorByFlowCondition( &destProcs, _curTime);
	//
	//		if( _bStationOnly )
	//				KeepOnlyStation( &destProcs );
	//	    // get count of possible destination processors
	//		for( int i=0; i<destProcs.getCount(); ++i )
	//		{
	//			vPossibleDestProcessor.push_back( destProcs.getItem( i ) );
	//		}
	//	}
	//

	//	int iPossibleDestCount = vPossibleDestProcessor.size();
	//	int iGreetingPlaceCount = _vGreetingPlace.size();
	//	// check if his visitor has already be there waiting for him , then he must go to that proc
	//	bool bHasGreetingPlace = false;
	////	ProcessorArray canSelectProc;
	//	for( int i=0; i<iPossibleDestCount; ++i )
	//	{
	//		Processor* pProc = vPossibleDestProcessor[ i ];
	//
	//		for( int j=0; j<iGreetingPlaceCount; ++j )
	//		{
	//			if( _vGreetingPlace[ j ].idFits( *pProc->getID() ) )
	//			{
	//				bHasGreetingPlace = true;
	//				canSelectProc.addItem( pProc );
	//				break;					
	//			}
	//		}		
	//	}
	//
	//	if( !bHasGreetingPlace )
	//	{
	//		return false;
	//	}

	//	// he arrive at greeting proc first, then select a best greeting place
	//	ASSERT( canSelectProc.getCount() > 0 );
	//	int iSelectCount = canSelectProc.getCount();
	//	
	//	//////////////////////////////////////////////////////////////////////////
	//	// one to one
	//	if( m_pSubFlow != NULL	) // moving in process flow
	//	{
	//		if( tempPair && tempPair->GetDestCount()>0 && tempPair->GetDestProcArcInfo(0)->GetOneToOneFlag() )
	//		{
	//			// 1:1
	//			m_bInLeafProcessor = true;	// don't know what for. check it later.
	//			Processor* pDestProc = canSelectProc.GetOneToOneProcessor( m_pProcessor->getID(), tempPair->GetDestProcArcInfo(0)->GetProcID().idLength() );
	//			if( pDestProc == NULL )
	//			{
	//				//throw new OneToOneFlowError( m_pPerson );
	//				ReleaseResource(_curTime);
	//				kill(_curTime);
	//
	//				//add by hans 2005-10-9
	//				char str[64];
	//				ElapsedTime tmptime(_curTime.asSeconds() % WholeDay);
	//				tmptime.printTime ( str );
	//
	//				CStartDate sDate = m_pTerm->m_pSimParam->GetStartDate();
	//				bool bAbsDate;
	//				COleDateTime date;
	//				int nDtIdx;
	//				COleDateTime time;
	//				CString sDateTimeStr;
	//				sDate.GetDateTime( _curTime, bAbsDate, date, nDtIdx, time );
	//				if( bAbsDate )
	//				{
	//					sDateTimeStr = date.Format(_T("%Y-%m-%d"));
	//				}
	//				else
	//				{
	//					sDateTimeStr.Format("Day %d", nDtIdx + 1 );
	//				}
	//				sDateTimeStr += " ";
	//				sDateTimeStr += str;
	//				throw new ARCDestProcessorUnavailableError( szMobType, strProcNameTmp,"one to one", sDateTimeStr);
	//
	//
	////				throw new ARCDestProcessorUnavailableError( szMobType, strProcNameTmp,"one to one", _curTime.printTime());
	//			}
	//			else
	//			{
	//				canSelectProc.clear();
	//				canSelectProc.addItem(pDestProc);
	//				m_bGerentGreetEventOrWait = true;
	//				return true;								
	//			}		
	//		}
	//	}
	//	else						 // moving in normal flow
	//	{
	//		if( m_pFlowList->IsCurOneToOne() )
	//		{
	//			// 1:1
	//			m_bInLeafProcessor = true;	// don't know what for. check it later.
	//			Processor* pDestProc = canSelectProc.GetOneToOneProcessor( m_pProcessor->getID(), m_pFlowList->GetCurDestIDLength() );
	//			if( pDestProc == NULL )
	//			{		
	//				//throw new OneToOneFlowError( m_pPerson );
	//				ReleaseResource(_curTime);
	//				kill(_curTime);
	//
	//				//add by hans 2005-10-9
	//				char str[64];
	//				ElapsedTime tmptime(_curTime.asSeconds() % WholeDay);
	//				tmptime.printTime ( str );
	//
	//				CStartDate sDate = m_pTerm->m_pSimParam->GetStartDate();
	//				bool bAbsDate;
	//				COleDateTime date;
	//				int nDtIdx;
	//				COleDateTime time;
	//				CString sDateTimeStr;
	//				sDate.GetDateTime( _curTime, bAbsDate, date, nDtIdx, time );
	//				if( bAbsDate )
	//				{
	//					sDateTimeStr = date.Format(_T("%Y-%m-%d"));
	//				}
	//				else
	//				{
	//					sDateTimeStr.Format("Day %d", nDtIdx + 1 );
	//				}
	//				sDateTimeStr += " ";
	//				sDateTimeStr += str;
	//				throw new ARCDestProcessorUnavailableError( szMobType, strProcNameTmp,"one to one", sDateTimeStr);
	//			}
	//			else
	//			{
	//				canSelectProc.clear();
	//				canSelectProc.addItem(pDestProc);
	//				m_bGerentGreetEventOrWait = true; 
	//				//*_pProc = pDestProc;
	//				return true;								
	//			}
	//		}
	//	}
	//    
	//	////////// oneXone
	//	if( ((CProcessorDistributionWithPipe*)m_pFlowList)->GetOneXOneState() == ChannelStart )
	//	{
	//		//		CString str = m_pProcessor->getID()->GetIDString();
	//		CFlowChannel* newChannel=new CFlowChannel(m_pProcessor);
	//		m_FlowChannelList.PushChannel(newChannel);
	//	}
	//	//now 1x1 end
	//	if( ((CProcessorDistributionWithPipe*)m_pFlowList)->GetOneXOneState() == ChannelEnd )
	//	{
	//		Processor* pTempProc;
	//		//		CString str = m_pProcessor->getID()->GetIDString();		
	//		CFlowChannel* getChannel;
	//		getChannel = m_FlowChannelList.PopChannel();
	//		if (getChannel != NULL)
	//		{
	//			pTempProc = getChannel->GetOutChannelProc(canSelectProc,m_pFlowList->GetCurDestIDLength());
	//		}
	//		else
	//		{
	//			pTempProc = NULL;			
	//		}
	//		delete getChannel;
	//		if( pTempProc == NULL )
	//		{
	//			//throw new OneXOneFlowError( m_pPerson );
	//			ReleaseResource(_curTime);
	//			kill(_curTime);
	//
	//			//add by hans 2005-10-9
	//			char str[64];
	//			ElapsedTime tmptime(_curTime.asSeconds() % WholeDay);
	//			tmptime.printTime ( str );
	//
	//			CStartDate sDate = m_pTerm->m_pSimParam->GetStartDate();
	//			bool bAbsDate;
	//			COleDateTime date;
	//			int nDtIdx;
	//			COleDateTime time;
	//			CString sDateTimeStr;
	//			sDate.GetDateTime( _curTime, bAbsDate, date, nDtIdx, time );
	//			if( bAbsDate )
	//			{
	//				sDateTimeStr = date.Format(_T("%Y-%m-%d"));
	//			}
	//			else
	//			{
	//				sDateTimeStr.Format("Day %d", nDtIdx + 1 );
	//			}
	//			sDateTimeStr += " ";
	//			sDateTimeStr += str;
	//			throw new ARCDestProcessorUnavailableError( szMobType, strProcNameTmp, "one X one", sDateTimeStr);
	//		}
	//		else
	//		{
	//			canSelectProc.clear();
	//			canSelectProc.addItem(pTempProc);
	//			m_bGerentGreetEventOrWait = true; 
	//			//*_pProc = pDestProc;
	//			return true;			
	//		}
	//	}
	//	///////////oneXone
	//	
	//	iSelectCount = canSelectProc.getCount();
	//	if(iSelectCount <= 1)
	//	{
	//		m_bGerentGreetEventOrWait = true;
	//		return true;
	//	}

	Processor *TempProc = NULL; 
	TempProc =GetHoldAreaLinkageFromType(m_pPerson,canSelectProc);
	if(TempProc!=NULL)
	{
		canSelectProc.clear();
		canSelectProc.addItem(TempProc);
		m_bGerentGreetEventOrWait = true;
		return true;
	}

	m_bGerentGreetEventOrWait = true;
	return true;
}
Processor* TerminalMobElementBehavior::GetHoldAreaLinkageFromType(const Person *_pPerson,const ProcessorArray&_pArray)
{
	Processor *pTempProc=NULL; 
	CMobileElemConstraint HostType(m_pTerm);
	HostType = _pPerson->getType();
	const ProcessorID* pDestID  = NULL;

	if(_pPerson->getType().GetTypeIndex() > 0) //visitor
	{
		HostType.SetTypeIndex(0);
	}
	HostType.SetMobileElementType(enum_MobileElementType_PAX);
	if (m_pPerson->m_logEntry.isDeparting())
	{
		return NULL;
	}
	else
	{
		int nIndex = m_pPerson->m_logEntry.getArrFlight();
		Flight* pFlight = getEngine()->m_simFlightSchedule.getItem(nIndex);
		pDestID = &(pFlight->getBaggageDevice());
	}
	//	ProcessorDistribution* pDistribution = (ProcessorDistribution*)m_pTerm->bagData->getFlightAssignToBaggageDeviceDB()->lookup( HostType ) ;
	if(pDestID == NULL || pDestID->isBlank() )// any pax who don not have assigned baggage device, such as dep pax, visitor....
	{
		return NULL;
	}

	// get single  baggage proc
	//	const ProcessorID* pBaggageID = pDistribution->getDestinationGroup( RANDOM ) ;	
	GroupIndex groupID=m_pTerm->procList->getGroupIndex(*pDestID);

	TRACE( "%s\n", pDestID->GetIDString() );

	for(int h=groupID.start;h<=groupID.end;h++)//bagIDGroup
	{
		BaggageProcessor *pBagegerProc = (BaggageProcessor*)m_pTerm->procList->getProcessor(h);
		const ProcessorIDList * pHostProcIDList = pBagegerProc->GetHostIdList();

		if((pHostProcIDList==NULL)||(pHostProcIDList->getCount()<=0)) 
			continue;

		for(int i=0;i<pHostProcIDList->getCount();i++)
		{
			const ProcessorID * pHostid = pHostProcIDList->getID(i);  //hostIDGroup
			//// TRACE(" \r\nHostpro:\t%s",pHostid->GetIDString());
			GroupIndex hostGroupIndex = m_pTerm->procList->getGroupIndex(*pHostid);//getindex
			for(int f=hostGroupIndex.start;f<=hostGroupIndex.end;f++)
			{
				for(int n=0;n<_pArray.getCount();n++)
				{
					pTempProc  = _pArray.getItem(n);
					if(pTempProc->getIndex() == f)
					{
						return pTempProc;
					}
				}
			}

		} // for(int i=0;i<pHostProcIDList->getCount();i++)

	}
	return NULL;
}

// walk from source point to destination point by pipe
void TerminalMobElementBehavior::WalkAlongShortestPathForEvac( const Point& _srcPoint, const Point& _desPoint, ElapsedTime& _curTime )
{
	// if no pipe , need not to move in pipe
	if( m_pTerm->m_pPipeDataSet->m_pPipeMgr->checkCanMoveByPipe( (int)(_srcPoint.getZ()/ SCALE_FACTOR)) == false )
		return;

	CGraphVertexList shortestPath;
	if( m_pTerm->m_pPipeDataSet->m_pPipeMgr->getShortestPathFromLib(_srcPoint, _desPoint, shortestPath ) == false )
		return;

	if( shortestPath.getCount()<3 )
		return;

	//check if need to walk by pipe
	double dLocationToDest = _srcPoint.distance( _desPoint );
	double dLocationToPipe = _srcPoint.distance( shortestPath.getItem(1).getVertexPoint() );
	if( dLocationToDest < dLocationToPipe )		// need not to move by pipe;
		return;			


	writeLogEntry( _curTime, false );
	int nOldState = m_pPerson->getState();
	Point old_destination = m_ptDestination;
	setState( WalkOnPipe );

	//////////////////////////////////////////////////////////////////////////
	// process Barrier line
	//m_ptDestination = shortestPath.getItem(1).getVertexPoint();
	setDestination( shortestPath.getItem(1).getVertexPoint() );
	Point nextPoint = m_ptDestination;
	// determines whether Person can travel a direct route to his
	// destination. if not, an intermediary destination will be assigned
	while (!hasClearPath(_curTime))
	{
		_curTime += moveTime();
		setState(MoveToServer);
		writeLogEntry (_curTime, false);
		setDestination (nextPoint);
		setState( WalkOnPipe );
	}

	//setDestination( old_destination );
	// get the real path, and write log
	PTONSIDEWALK LogPointList;
	int iPercent = random(100);
	m_pTerm->m_pPipeDataSet->GetPointListForLog( shortestPath, iPercent, LogPointList );
	WritePipeLogs( LogPointList, _curTime, false );
	//comment for processing congestion pipe
	//	setState( nOldState );
	return; 	
}
Point TerminalMobElementBehavior::GetPipeExitPoint( Processor* _pNextProc,int iCurFloor, CString& _curTime,Point& outPoint, CFlowItemEx* _pFlowItem)
{

	if(_pNextProc->getProcessorType() == StairProc || _pNextProc->getProcessorType() == Elevator)
	{

		if(_pNextProc != NULL && _pNextProc->CheckIfOnTheSameFloor(iCurFloor))
		{

			return _pNextProc->GetEntryPoint(getPersonErrorMsg(),iCurFloor,_curTime);
		}

	}

	if(_pNextProc->getServicePoint( 0 ).getZ()/ SCALE_FACTOR != iCurFloor) 
	{
		//throw new ARCFlowError (getPersonErrorMsg(), _pNextProc->getID()->GetIDString(),"not the same Floor Problem! ",_curTime);
		return _pNextProc->getServicePoint( 0 );
	} 
	//AfxMessageBox("the Same Floor Problem !");
	if( _pNextProc->inConstraintLength())
		return _pNextProc->inConstraint(0);


	if(_pNextProc->getProcessorType()==BridgeConnectorProc)
	{
		return _pNextProc->getServicePoint(0);
	}
	
	ProcessorQueue* procQueue = _pNextProc->GetProcessorQueue();
	if( procQueue != NULL )
	{	
		int nQueuePointCount = _pNextProc->GetProcessorQueue()->cornerCount();
		if(nQueuePointCount <= 0)
			return _pNextProc->getServicePoint(0);

		

		if(procQueue->isFixed() == 'N')  // non fix queue, use the queue entry point as pipe entry/exit point
		{
			return _pNextProc->GetProcessorQueue()->corner(nQueuePointCount-1);
		}
		else  // processor has a fixed queue
		{
			Point entryPoint, tempPoint;
			double dTravelLength = (std::numeric_limits<double>::max)();
			std::vector<int>* pEntryPointList = ((FixedQueue*)_pNextProc->GetProcessorQueue())->GetEntryPoints();
			std::vector<int> entryList(*pEntryPointList);
			entryList.push_back(nQueuePointCount-1); // the end of fixed queue is an entry point
			// use system pipe
			if(_pFlowItem == NULL || 
                _pFlowItem->GetTypeOfUsingPipe() == USE_PIPE_SYSTEM ||
                _pFlowItem->GetTypeOfUsingPipe() == USE_USER_SELECTED_PIPES)
			{

				CPipeGraphMgr* pPipeMgr = m_pTerm->m_pPipeDataSet->m_pPipeMgr;
				for(std::vector<int>::iterator itor = entryList.begin();
					itor != entryList.end();
					itor++)
				{	
                    CGraphVertexList shortestPath;
					tempPoint = _pNextProc->GetProcessorQueue()->corner(*itor);
					if(pPipeMgr->getShortestPathFromLib(outPoint, tempPoint, shortestPath) != false)
					{
						shortestPath.ReCalculateLength();
						double walkDistance = shortestPath.GetLength();
						if(walkDistance < dTravelLength)
						{
							entryPoint = tempPoint;
							dTravelLength = walkDistance;
							m_entryPointCorner = *itor;
						}
					}
				}
			}
// 			else if(_pFlowItem->GetTypeOfUsingPipe() == USE_USER_SELECTED_PIPES)
// 			{
// 				for(std::vector<int>::iterator itor = entryList.begin();
// 					itor != entryList.end();
// 					itor++)
// 				{
// 					tempPoint = _pNextProc->GetProcessorQueue()->corner(*itor);
// 					double dDistance = CalculateWalkLengthOfUserPipe(outPoint, tempPoint, _pFlowItem->GetPipeVector());
// 					if(dDistance < dTravelLength)
// 					{
// 						dTravelLength = dDistance;
// 						entryPoint = tempPoint;
// 						m_entryPointCorner = *itor;
// 					}
// 				}
// 			}
			else if(_pFlowItem->GetTypeOfUsingPipe() == USE_NOTHING)
			{
				for(std::vector<int>::iterator itor = entryList.begin();
					itor != entryList.end();
					itor++)
				{
					tempPoint = _pNextProc->GetProcessorQueue()->corner(*itor);
					double dDistance = outPoint.distance(tempPoint);
					if(dDistance < dTravelLength)
					{
						dTravelLength = dDistance;
						entryPoint = tempPoint;
						int nPerson = m_pPerson->getID();
						m_entryPointCorner = *itor;
					}
				}
			}
			return entryPoint;
		}
	}
	return _pNextProc->getServicePoint(0);
}
//////////////////////////////////////////////////////////////////////////
void TerminalMobElementBehavior::WalkAlongShortestPath( Processor* _pNextProc, const Point &pt, ElapsedTime& _curTime)
{
	// if not same floor, need not move in pipe
	int iCurFloor = (int)(location.getZ() / SCALE_FACTOR);


	Point outPoint = location;
	if(m_pProcessor->outConstraintLength() > 0)
		outPoint = m_pProcessor->outConstraint(m_pProcessor->outConstraintLength()-1);
	Point entryPoint;
	if(_pNextProc->getProcessorType()== HoldAreaProc
		|| _pNextProc->getProcessorType() == LineProc
		|| _pNextProc->getProcessorType() == BaggageProc)

	{
		if (m_nextHoldAiearPoTag)
			entryPoint = m_nextHoldAiearPoint;
		else
		{
			entryPoint = GetPipeExitPoint(_pNextProc,iCurFloor, ClacTimeString(_curTime),outPoint);
		}
	}else
		entryPoint = GetPipeExitPoint(_pNextProc,iCurFloor, ClacTimeString(_curTime),outPoint);

	int iEntryFloor = (int)(entryPoint.getZ() / SCALE_FACTOR);
	if (iCurFloor != iEntryFloor)
		return;

	// if no pipe , need not to move in pipe
	CPipeGraphMgr* pPipeMgr = m_pTerm->m_pPipeDataSet->m_pPipeMgr;
	if (!pPipeMgr->checkCanMoveByPipe(iEntryFloor))
		return;


	CGraphVertexList shortestPath;
	if (!pPipeMgr->getShortestPathFromLib(outPoint, entryPoint, shortestPath))
		return;

	int nVertexCount = shortestPath.getCount();
	if (nVertexCount < 3)
		return;

	writeLogEntry( _curTime, false );

	setState( WalkOnPipe );

	// get the real path, and write log
	PTONSIDEWALK LogPointList;
	int iPercent = random(100);
	m_pTerm->m_pPipeDataSet->GetPointListForLog( shortestPath, iPercent, LogPointList );

	setDestination(LogPointList.at(0).GetPointOnSideWalk());

	//LogPointList.erase(LogPointList.begin());

	// determines whether Person can travel a direct route to his
	// destination. if not, an intermediary destination will be assigned
	while (!hasClearPath(_curTime))
	{
		_curTime += moveTime();
		setState(MoveToServer);
		writeLogEntry (_curTime, false);
		setDestination (m_ptDestination);
		setState( WalkOnPipe );
	}
	_curTime += moveTime();
	WritePipeLogs( LogPointList, _curTime, getEngine()->GetFireOccurFlag());
}

// test if i must stay at m_pPerson stage
bool TerminalMobElementBehavior::PersonShouldWaitOnThisStage( int _iStageID , const ElapsedTime& _currentTime )
{
	Flight *aFlight = getEngine()->m_simFlightSchedule.getItem (m_pPerson->m_ilightIndex);
	ASSERT( aFlight );
	std::map<int,ElapsedTime>&mapLastCalls = aFlight->GetLastCallOfEveryStage();
	if( mapLastCalls.find( _iStageID ) != mapLastCalls.end() )//find
	{
		//if the last call of m_pPerson stage have occured, then person should not wait here
		if( mapLastCalls[ _iStageID ] <= _currentTime )
		{	
			m_nStranded = FALSE;
			return false;
		}
	}
	else
	{
		return true;
	}


	if( m_mapWaitOnStage.find( _iStageID ) != m_mapWaitOnStage.end() ) //found
	{
		if( m_mapWaitOnStage[ _iStageID ] )
		{
			return true;
		}
		else
		{
			//m_nStranded = FALSE;
			return false;
		}
	}
	else
	{
		m_mapWaitOnStage.insert( std::map<int,bool>::value_type( _iStageID, true ) );
		return true;
	}
}


// if the destination is loader is full, let the bag move to loader's linked holding
bool TerminalMobElementBehavior::HandleLoaderHoldLogic( Processor** _nextproc, const ElapsedTime& _curTime )
{
	// for throw exception
 	CString szMobType = getPersonErrorMsg();
 	CString strProcNameTmp = m_pProcessor ? m_pProcessor->getID()->GetIDString() : "";
 	//////////////////////////////////////////////////////////////////////////
 
 	Processor* _proc = *_nextproc;
 
 	if( _proc == NULL )
 		return false;
 
 	if( _proc->getProcessorType() == ConveyorProc )
 	{
 
 		Conveyor* pConveyor = ( Conveyor*)( _proc );
 		// if the processor is loader and it is full
 		if( pConveyor->GetSubConveyorType() == LOADER )
 		{
 			m_pLoaderProc = _proc;
 
 			//int nEmptyCount = pConveyor->GetEmptySlotCount();
 			//int nGroupSize = 0; 
 			//if (!m_pGroupInfo->IsFollower())
 			//{
 			//	nGroupSize = ((CGroupLeaderInfo *)m_pGroupInfo)->GetGroupSize();
 			//}
 
 			//if (nEmptyCount < nGroupSize)
 			if ( pConveyor->isExceedProcCapacity() )
 			{		
 				// get a link holding area( rand )
 				HoldingArea* _linkHold = pConveyor->GetRandomHoldingArea();// (HoldingArea*)( m_pTerm->procList->GetWaitArea( _proc->getID() ) );
 				if( _linkHold == NULL )	// can not get link hold area
 				{
 					ReleaseResource(_curTime);
 					kill(_curTime);
					
					CString strLoader;
					strLoader.Format(_T("This Loader( %s ) is out of capacity, suggest define a holding area for this Loader or increase capacity of this Loader"),pConveyor->getID()->GetIDString());
 					throw new ARCDestProcessorUnavailableError( szMobType, strProcNameTmp,strLoader, ClacTimeString(_curTime) );
 
 					//				throw new ARCDestProcessorUnavailableError( szMobType, strProcNameTmp,"", _curTime.printTime() );
 				}
 
 				// let the bag moving to holding;
 				m_pPerson->m_pGroupInfo->SetNextProc(m_pLoaderProc);
 				_linkHold->storeBagtoHolding( m_pPerson, *(_proc->getID()) );
 				m_bNeedLoaderToHoldArea = true;
 				*_nextproc = _linkHold;
 				return true;
 			}
 		}
 	}

	return false;
}

bool TerminalMobElementBehavior::HandleSorterLogic(Processor** _nextproc, const ElapsedTime& _curTime )
{
	bool bReturn=true;
	//m_pPerson case: m_pProcessor is Sorter and person will exit from it. 
	//m_iSorterEntryExitProcs used to save the exit processor's index.
	if(m_iSorterEntryExitProcs>=0)
	{
		if(m_pProcessor==NULL || m_pProcessor->getProcessorType()!=ConveyorProc || static_cast<Conveyor*>(m_pProcessor)->GetSubConveyorType()!=SORTER )return bReturn;

		Processor* pProcs=static_cast<Conveyor*>(m_pProcessor)->GetSorterExitProcs(m_iSorterEntryExitProcs);
		m_iSorterEntryExitProcs=-1;
		if(pProcs!=NULL)
		{
			*_nextproc = pProcs;
		}
	}
	else if( (*_nextproc)!=NULL && (*_nextproc)->getProcessorType()==ConveyorProc && static_cast<Conveyor*>(*_nextproc)->GetSubConveyorType()==SORTER)
	{
		//in m_pPerson case: *_nextproc is Sorter and m_pProcessor is its entry processor(conveyor).
		//m_iSorterEntryExitProcs used to save the entry processor's index.
		Conveyor* pNextConveyor=static_cast<Conveyor*>(*_nextproc);
		m_iSorterEntryExitProcs=pNextConveyor->GetSorterEntryProcs( *m_pProcessor->getID());

	}


	return bReturn;
}



bool TerminalMobElementBehavior::FindAllPossibleDestinationProcs(std::vector<ProcessorID>& _vSearchProc,ProcessorArray &canSelectProc,bool _bStationOnly,ElapsedTime _curTime,bool& bGerentGreetEventOrWait )
{
	CString szMobType = getPersonErrorMsg();
	CString strProcNameTmp = m_pProcessor ? m_pProcessor->getID()->GetIDString() : "";
	//

	const ProcessorID *nextGroup;
	std::vector<Processor*>vPossibleDestProcessor;
	ProcessorArray destProcs;
	CProcessorDestinationList* tempPair = NULL;

	// no proc can be selected because of the roster
	bool bRosterReason = false;
	// get all possible dest processors, ( opening and can server and.....)
	// is moving in normal flow
	{
		nextGroup = m_pFlowList->getDestinationGroup (RANDOM) ;
		do 
		{	
			CString strProcName = nextGroup->GetIDString();

			m_pTerm->procList->buildArray (nextGroup, &destProcs,

				m_pPerson->m_type, m_pPerson->m_nGate, NeedCheckLeadToGate(),m_pPerson->m_nBagCarosel,NeddCheckLeadToReclaim(), FALSE, m_pProcessor, _curTime, getEngine() ,bRosterReason,m_iDestFloor, m_pDiagnoseInfo, m_pPerson);

			int iDestCount = destProcs.getCount();
			// if none open, generate list of closed processors
			if (iDestCount == 0)
				m_pTerm->procList->buildArray (nextGroup, &destProcs,
				m_pPerson->m_type, m_pPerson->m_nGate,NeedCheckLeadToGate(),m_pPerson->m_nBagCarosel,NeddCheckLeadToReclaim(), TRUE, m_pProcessor, _curTime, getEngine(), bRosterReason,m_iDestFloor, m_pDiagnoseInfo, m_pPerson);

			FilterOutProcessorByFlowCondition( &destProcs , _curTime );

			if( _bStationOnly )
				KeepOnlyStation( &destProcs );

			iDestCount = destProcs.getCount();
			for( int i=0; i<iDestCount; ++i )
			{
				vPossibleDestProcessor.push_back( destProcs.getItem( i ) );
			}


		}while ((nextGroup = m_pFlowList->getDestinationGroup (SEQUENTIAL)) != NULL);

		//--------------------------------------------------------------------------------
		// Filter out:: dependent processors check(stop serve if all dependent processors are occupied)

		Processor* defaultProcHolder = NULL;
		if(vPossibleDestProcessor.size() > 0 )
		{
			defaultProcHolder = vPossibleDestProcessor.at(0);

			std::vector<Processor*>::iterator iter = vPossibleDestProcessor.begin();
			for (; iter != vPossibleDestProcessor.end();)
			{
				Processor* pDestProcessor = *iter;
				if(pDestProcessor->isDependentProcUsed()
					&& pDestProcessor->isAllDependentProcsOccupied(m_pPerson))
				{
					iter = vPossibleDestProcessor.erase( iter );
				}
				else
					++iter;
			}

		}

		//---------------------------------------------------------------------------------
		// if cannot found destination processor, check spare device(lowest priority processor:0%)
		if( (vPossibleDestProcessor.size() <= 0	&& (nextGroup = m_pFlowList->getZeropercentDestGroup())!= NULL ))
		{
			ProcessorArray lowestPriorityDestProcs;
			do{
				CString strProcName = nextGroup->GetIDString();
				m_pTerm->procList->buildArray (nextGroup, &lowestPriorityDestProcs,

					m_pPerson->m_type, m_pPerson->m_nGate, NeedCheckLeadToGate(),m_pPerson->m_nBagCarosel,NeddCheckLeadToReclaim(), FALSE, m_pProcessor, _curTime, getEngine() ,bRosterReason,m_iDestFloor, m_pDiagnoseInfo, m_pPerson);

				int iDestCount = destProcs.getCount();
				// if none open, generate list of closed processors
				if (iDestCount == 0)
					m_pTerm->procList->buildArray (nextGroup, &lowestPriorityDestProcs,
					m_pPerson->m_type, m_pPerson->m_nGate,NeedCheckLeadToGate(), m_pPerson->m_nBagCarosel,NeddCheckLeadToReclaim(), TRUE, m_pProcessor, _curTime, getEngine(), bRosterReason,m_iDestFloor, m_pDiagnoseInfo, m_pPerson);

				FilterOutProcessorByFlowCondition( &lowestPriorityDestProcs , _curTime );

				if( _bStationOnly )
					KeepOnlyStation( &lowestPriorityDestProcs );

				for( int i=0; i<lowestPriorityDestProcs.getCount(); ++i )
				{
					vPossibleDestProcessor.push_back( lowestPriorityDestProcs.getItem( i ) );
				}

			}while((nextGroup = m_pFlowList->getZeropercentDestGroup())!= NULL);
		}

		//---------------------------------------------------------------------------------
		if(vPossibleDestProcessor.size() == 0 && defaultProcHolder != NULL)
			vPossibleDestProcessor.push_back( defaultProcHolder);



	}

	int iPossibleDestCount = vPossibleDestProcessor.size();
	int iSearchProcCount = _vSearchProc.size();
	// check if his visitor has already be there waiting for him , then he must go to that proc
	bool bHasSearchProc = false;
	//	ProcessorArray canSelectProc;
	for( int i=0; i<iPossibleDestCount; ++i )
	{
		Processor* pProc = vPossibleDestProcessor[ i ];

		for( int j=0; j<iSearchProcCount; ++j )
		{
			if( _vSearchProc[ j ].idFits( *pProc->getID() ) )
			{
				bHasSearchProc = true;
				canSelectProc.addItem( pProc );
				break;					
			}
		}		
	}

	if( !bHasSearchProc )
	{
		return false;
	}



	// he arrive at greeting proc first, then select a best greeting place
	ASSERT( canSelectProc.getCount() > 0 );
	int iSelectCount = canSelectProc.getCount();

	//////////////////////////////////////////////////////////////////////////
	// one to one

	// moving in normal flow
	{
		if( m_pFlowList->IsCurOneToOne() && m_pTerm->procList->getProcessor (START_PROCESSOR_INDEX) != m_pProcessor)
		{
			// 1:1
			Processor* pDestProc = canSelectProc.GetOneToOneProcessor( m_pProcessor->getID(), m_pFlowList->GetCurDestIDLength() );
			if( pDestProc == NULL )
			{		
				//throw new OneToOneFlowError( m_pPerson );
				ReleaseResource(_curTime);
				kill(_curTime);

				throw new ARCDestProcessorUnavailableError( szMobType, strProcNameTmp,"one to one", ClacTimeString(_curTime));
			}
			else
			{
				canSelectProc.clear();
				canSelectProc.addItem(pDestProc);
				bGerentGreetEventOrWait = true; 
				//*_pProc = pDestProc;
				return true;								
			}
		}
	}

	////////// oneXone
	if( ((CProcessorDistributionWithPipe*)m_pFlowList)->GetOneXOneState() == ChannelStart )
	{
		//		CString str = m_pProcessor->getID()->GetIDString();
		CFlowChannel* newChannel=new CFlowChannel(m_pProcessor);
		m_FlowChannelList.PushChannel(newChannel);
	}
	//now 1x1 end
	if( ((CProcessorDistributionWithPipe*)m_pFlowList)->GetOneXOneState() == ChannelEnd )
	{
		Processor* pTempProc;
		//		CString str = m_pProcessor->getID()->GetIDString();		
		CFlowChannel* getChannel;
		getChannel = m_FlowChannelList.PopChannel();
		if (getChannel != NULL)
		{
			pTempProc = getChannel->GetOutChannelProc(canSelectProc,m_pFlowList->GetCurDestIDLength());
		}
		else
		{
			pTempProc = NULL;			
		}
		delete getChannel;
		if( pTempProc == NULL )
		{
			//throw new OneXOneFlowError( m_pPerson );
			ReleaseResource(_curTime);
			kill(_curTime);

			throw new ARCDestProcessorUnavailableError( szMobType, strProcNameTmp, "one X one", ClacTimeString(_curTime));
		}
		else
		{
			canSelectProc.clear();
			canSelectProc.addItem(pTempProc);
			bGerentGreetEventOrWait = true; 
			//*_pProc = pDestProc;
			return true;			
		}
	}
	///////////oneXone

	iSelectCount = canSelectProc.getCount();
	if(iSelectCount <= 1)
	{
		bGerentGreetEventOrWait = true;
		return true;
	}

	return true;
}


BridgeConnector* TerminalMobElementBehavior::GetBridgeConnector(Processor* pProcessor)
{
	if(pProcessor->getProcessorType() != GateProc)
		return NULL;
	GateProcessor* pGateProc = (GateProcessor*)pProcessor;
	return pGateProc->getBridgeConnector(m_pPerson->GetCurFlightIndex());
}

stdostream& operator << (stdostream& s, const TerminalMobElementBehavior& p)
{
	char str[128];
	p.m_pProcessor->getID()->printID (str);

	CString sPrint;;
	p.m_pPerson->getType().screenPrint( sPrint ,0,256);

	return s << "Pax #" <<  p.m_pPerson->getID() << ", Type " << sPrint.GetBuffer(0) << ". Currently "
		<< p.m_pPerson->getState() << " at " << str << " queue index :" <<  p.m_nCurrentIndex <<"  queue next idx :" <<  p.m_nNextIndex <<'\n';
}

int TerminalMobElementBehavior::processorNum (void) const
{
	return m_pProcessor->getIndex();
}

// check whether the Person can jump to the Processor _proc
// make sure the _proc is at the same level of the person's current processor in flow
// that means it must be 'JUMP'
bool TerminalMobElementBehavior::canJumpToProcessor(Processor* _proc)
{
	// the changed processor can not lead to the gate of jumper's flight.
	if(m_pPerson->getGateIndex() == -1)
		return true;
	if(getTerminal()->GetProcessorList()->getProcessor(m_pPerson->getGateIndex()) == NULL)
		return true;

	if(!_proc->CanLeadTo(
		m_pPerson->getType(), 
		*getTerminal()->GetProcessorList()->getProcessor(m_pPerson->getGateIndex())->getID()))
	{
		return false;
	}

	CanServeProcList canServeProcList1;
	if (!_proc->canServe(m_pPerson->getType(), &canServeProcList1))
	{
		// if can not serve
		// just return false
		return false;
	}

	if (canServeProcList1.IsExclusiveInList(_proc->getID()))
	{
		// the _proc can serve the person exclusively
		return true;
	}

	if (NULL == m_pProcessor)
	{
		// no processor now serving the person
		// certainly the person can jump to _proc
		// but it should not be
		ASSERT(FALSE);
		return true;
	}

	CanServeProcList canServeProcList2;
	m_pProcessor->canServe(m_pPerson->getType(), &canServeProcList2);
	if (false == canServeProcList2.IsExclusiveInList(m_pProcessor->getID()))
	{
		// the _proc can not serve the person exclusively
		// and m_pProcessor can not serve the person exclusively
		// m_pPerson means the _proc can serve the person
		return true;
	}

	// other case
	// can not jump
	return false;
}

// used by persononstation, when selected dest station became the current station
void TerminalMobElementBehavior::SetDestStation( int _nIndex )
{
	m_pProcessor->removePerson( m_pPerson );
	Processor* pProc = m_pTerm->procList->getProcessor( _nIndex );
	m_pProcessor = pProc;
	m_pProcessor->addPerson( m_pPerson );
	m_pProcessor->AddRecycleFrequency(m_pPerson);
}

Processor* TerminalMobElementBehavior::TryToSelectNextProcessor(const ElapsedTime& _timeToSelect ,bool _bStationOnly/*= false */, bool _bExcludeFlowBelt/* = false */)
{
	m_pPerson->regetAttatchedNopax();

	Processor *nextProc = NULL;
	{
		m_pFlowList = m_pProcessor->getNextDestinations ( m_pPerson->getType(), m_nInGateDetailMode );
		nextProc = selectProcessor( _timeToSelect, _bStationOnly, _bExcludeFlowBelt );
	}
	//m_pFlowList = m_pProcessor->getNextDestinations ( m_pPerson->getType(), m_nInGateDetailMode );
	//return selectProcessor( _timeToSelect, _bStationOnly, _bExcludeFlowBelt );

	return nextProc;
}

// before the person begin to avoid density, do something
void TerminalMobElementBehavior::doSomethingBeforeAvoid( void )
{
	switch( m_pPerson->getState() )
	{
	case MoveToQueue:
	case WaitInQueue:
		assert( m_pProcessor );
		m_pProcessor->removePerson( m_pPerson );
		break;

	default:
		break;
	}
}

// after avoid density, do something
void TerminalMobElementBehavior::doSomethingAfterAvoid( const ElapsedTime& _time )
{
	switch( m_pPerson->getState())
	{
	case MoveToQueue:
	case WaitInQueue:
		assert( m_pProcessor );
		m_pProcessor->addPerson( m_pPerson );
		m_pProcessor->AddRecycleFrequency(m_pPerson);
		m_pProcessor->addToQueue( m_pPerson, _time );

		
		break;

	default:
		break;
	}
}

// check the area if is full
bool TerminalMobElementBehavior::checkAreaIsFull( const CArea* _pArea ) const
{
	assert( _pArea );
	ProcessorID _procID;
	_procID.init();
	if( m_pPreFlowList )
	{
		assert( m_pPreFlowList->GetCurGroupIndex() < m_pPreFlowList->GetDestCount() && m_pPreFlowList->GetCurGroupIndex() >=0 );
		_procID = *( m_pPreFlowList->getGroup( m_pPreFlowList->GetCurGroupIndex() ) );
	}

	return _pArea->isFull( m_pPerson, _procID );
}

// yes for dep, no for others.
bool TerminalMobElementBehavior::NeedCheckLeadToGate() const
{
	if( m_pPerson->getGateIndex() < 0 )
		return false;

	/*
	if( m_pPerson->getType().GetTypeIndex() == 0 && m_pPerson->getType().getMode() == 'D' && m_iNumberOfPassedGateProc < 1 )
	return true;
	*/

	//modified by tutu .
	// because we need to check arrive pax
	if( m_pPerson->getType().GetTypeIndex() == 0 && m_iNumberOfPassedGateProc < 1 )
		return true;
	return false;
}
bool TerminalMobElementBehavior::NeddCheckLeadToReclaim()const
{
	if( m_pPerson->getBagCarosel() < 0 )
		return false;

	if( m_pPerson->m_type.GetTypeIndex() == 2 && m_iNumberOfPassedReclaimProc < 1 )
		return true;
	return false;
}

void TerminalMobElementBehavior::ProcessError(ElapsedTime _curTime, const CString& szMobType, const CString& strProcNameTmp)
{
	ReleaseResource(_curTime);
	kill(_curTime);


	throw new ARCDestProcessorUnavailableError( szMobType, strProcNameTmp,"in process", ClacTimeString(_curTime));
}

CString TerminalMobElementBehavior::GetPersonContextInfo()const
{
	CString str;
	str.Format("   Element ID: %d ", m_pPerson->getID() );

	str += " Element Type: ";
	//char tempChars[ 256 ]; matt
	char tempChars[2560];
	m_pPerson->getType().WriteSyntaxStringWithVersion( tempChars );
	str += tempChars;
	if( m_pProcessor )
	{
		str += " At Processor : ";
		str += m_pProcessor->getID()->GetIDString();
	}

	return str;

}

// get dest index.
int TerminalMobElementBehavior::GetDestStation( ElapsedTime p_time, Processor* _pProcessor )
{
	m_pFlowList = _pProcessor->getNextStation( m_pPerson->getType() );

	// handle case where Processor has no ProcessorDistribution
	if( m_pFlowList == NULL )
		return -1;

	// get Processor
	Processor* pDestProc = selectProcessor( p_time, true );
	if( !pDestProc )
		return -1;

	return pDestProc->getIndex();
}
Processor* TerminalMobElementBehavior::TryToGetElevatorDestProc( ElapsedTime p_time, Processor* _pProcessor )
{
	ProcessorDistribution* pOldFlowList = m_pFlowList;
	Processor *pOldProcessor = m_pProcessor;
	bool bOldWaitInLinkedArea = m_bWaitInLinkedArea;
	Processor* pOldBackToProcFromWaitArea =  m_pBackToProcFromWaitArea;

	 m_pFlowList = _pProcessor->getNextDestinations ( m_pPerson->getType(), m_nInGateDetailMode );
	m_pProcessor = _pProcessor;


	Processor* pDestProc = NULL;
	// handle case where Processor has no ProcessorDistribution
	if( m_pFlowList != NULL )
	{
		pDestProc =	selectProcessor( p_time, false, false, true );
	}

	m_pFlowList = pOldFlowList;
	m_pProcessor = pOldProcessor;
	m_bWaitInLinkedArea = bOldWaitInLinkedArea;
	m_pBackToProcFromWaitArea = pOldBackToProcFromWaitArea;
	
	
	return pDestProc;
}
// wait for resource 
// if need to wait, send a resource request to pool, return true
// else return false
bool TerminalMobElementBehavior::waitForResourceIfNeed( const ElapsedTime& _time )
{
	///Test code
	//DWORD dStartTime = GetTickCount();
	///
	long _lServiceTime;

	// if the person have been service by resource, m_pPerson times, need to wait for resource
	if( m_bHaveServiceByResource )	
		return false;

	CResourcePool* _pool = m_pProcessor->getBestResourcePoolByType( m_pPerson->getType(), _lServiceTime );
	if( _pool == NULL )		// no define pool for the pax type
	{
		return false;
	}

	// then send  a resource request to the pool
	ResourceRequestItem request_item;
	request_item.request_mob = m_pPerson;
	request_item.request_proc = m_pProcessor;
	request_item.request_time = _time;
	request_item.request_servicetime = _lServiceTime;
	request_item.request_point = Point(getPoint());
	_pool->addRequestToList( request_item, _time );

	m_bHaveServiceByResource = true;
	//test code 
	//
	return true;
}

void TerminalMobElementBehavior::PushChannelToList(CFlowChannel* pChannel)
{
	ASSERT(pChannel != NULL);
	m_FlowChannelList.PushChannel(pChannel);
}

CFlowChannel* TerminalMobElementBehavior::PopChannelFromList()
{
	CFlowChannel* pChannel = m_FlowChannelList.PopChannel();
	ASSERT(pChannel != NULL);

	return pChannel;
}


void TerminalMobElementBehavior::ClearBeforeThrowException( const ElapsedTime& _currentTime )
{
	kill( _currentTime );
}

// check if need to generate avoid event
bool TerminalMobElementBehavior::needToAvoidDensity( void ) const
{
	if( m_pPreFlowList )
	{
		std::vector<CFlowItemEx>& vFlowCondition = ( (CProcessorDistributionWithPipe*)m_pPreFlowList )->GetPipeVector();
		assert( m_pPreFlowList->GetCurGroupIndex() < m_pPreFlowList->GetDestCount() && m_pPreFlowList->GetCurGroupIndex() >=0 );
		if( vFlowCondition.empty() )
			return false;

		CFlowItemEx& flowCondition = vFlowCondition[m_pPreFlowList->GetCurGroupIndex()];
		std::vector<CString>& tstrvec = flowCondition.GetDensityArea();

		if( tstrvec.empty() )
			return false;

		return true;
	}


	return false;
}

// if _eCurTime > FlightDepTime - _lMinutes
bool TerminalMobElementBehavior::IsLateForDepFlight( ElapsedTime _eCurTime, long _lMinutes )
{
	if( _lMinutes < 0l )
		_lMinutes = 0l;

	int nDepFlightIdx = m_pPerson->getLogEntry().getDepFlight();
	if( nDepFlightIdx < 0 )
		return false;
	Flight *pFlight = getEngine()->m_simFlightSchedule.getItem( nDepFlightIdx );

	ElapsedTime eDeltaTime;
	eDeltaTime.set( _lMinutes * 60l );
	
	bool bLate = false;
	ElapsedTime actureDepTimeWithDelay = ElapsedTime(0L);
	if(simEngineConfig()->isSimAirsideMode())
	{
		if (pFlight->IsAirsideFlightDepartured())
		{
			if (pFlight->RetrieveAirsideDepartureTime())
			{
				actureDepTimeWithDelay = pFlight->GetRealDepTime()  - eDeltaTime;

				if(_eCurTime >  actureDepTimeWithDelay)
					bLate = true;
			}
		}
	}
	else
	{
		actureDepTimeWithDelay = pFlight->getDepTime() + pFlight->getDepDelay() - eDeltaTime;
		if( _eCurTime >  actureDepTimeWithDelay)
			bLate = true;
	}

	if(bLate)
	{
		char szBuffer[128] = {0};
		CString sPrint;
		FORMATMSGEX msgEx;

		CStartDate sDate = m_pTerm->m_pSimParam->GetStartDate();
		bool bAbsDate;
		COleDateTime date;
		int nDtIdx;
		COleDateTime time;
		CString sDateTimeStr;
		sDate.GetDateTime( _eCurTime, bAbsDate, date, nDtIdx, time );
		if( bAbsDate )
		{
			sDateTimeStr = date.Format(_T("%Y-%m-%d"));
		}
		else
		{
			sDateTimeStr.Format("Day %d", nDtIdx + 1 );
		}
		sDateTimeStr += "  ";
		ElapsedTime tmptime(_eCurTime.asSeconds() % WholeDay);
		sDateTimeStr += tmptime.printTime();

		msgEx.strTime			= sDateTimeStr;
		msgEx.strMsgType		= "MISSED FLIGHT";
		msgEx.strProcessorID	= getProcessor() ? getProcessor()->getID()->GetIDString() : "";
		msgEx.strPaxID.Format("%ld", m_pPerson->getID() );
		pFlight->getFullID( szBuffer, 'D' );
		//szBuffer[7] = NULL;
		msgEx.strFlightID.Format(_T("%s"),szBuffer) ;		
		m_pPerson->getType().screenPrint( sPrint, 0, 128 ); 
		msgEx.strMobileType		= sPrint;//CString( szBuffer );
		if(m_pPerson->m_pGroupInfo->IsFollower() && m_pPerson->m_pGroupInfo->GetGroupLeader() != NULL)
		{
			long lID = m_pPerson->m_pGroupInfo->GetGroupLeader()->getID();
			msgEx.strOther.Format("missed its flight by leader %d in group", lID);
		}
		else
		{
			msgEx.strOther.Format( "missed its flight by %s" , (_eCurTime - actureDepTimeWithDelay ).printTime() );
		}
		msgEx.diag_entry.eType	= MISS_FLIGHT;
		msgEx.diag_entry.lData	= m_pPerson->getLogEntry().getIndex();
		m_pPerson->getLogEntry().SetMissFlightFlg(true);
		m_pPerson->getLogEntry().SetMissFlightTime(_eCurTime);
		m_pPerson->getLogEntry().SetRealDepartureTime(actureDepTimeWithDelay);

		getEngine()->SendSimFormatExMessage( msgEx );
		
		return true;
	}





	return false;	
}

BOOL TerminalMobElementBehavior::HasBusForThisFlight(int _id,HoldingArea* pHold)
{
	if(getEngine()->GetAirsideSimulation() == NULL || !getEngine()->IsAirsideSel() )
		return FALSE ;
	AirsideFlightInSim* Airflight = getEngine()->GetAirsideSimulation()->GetAirsideFlight(_id) ;
	//Airflight->AddPaxInFlight();
	if(Airflight == NULL)
		return FALSE ;
	
	return	Airflight->DeparturePaxBusService();
}

void TerminalMobElementBehavior::SetInfoFromLeader(Person* pLeader)
{
	TerminalMobElementBehavior* spTerminalBehavior = (TerminalMobElementBehavior*)pLeader->getBehavior(MobElementBehavior::TerminalBehavior);

	if (spTerminalBehavior)
	{
		setState( pLeader->getState() );
		setProcessor( spTerminalBehavior->getProcessor() );
		m_pPerson->m_nGate = pLeader->getGateIndex();
		m_FlowChannelList.InitFlowChannelList(spTerminalBehavior->GetFlowChannelList());
		m_iNumberOfPassedGateProc = spTerminalBehavior->m_iNumberOfPassedGateProc;
		m_iNumberOfPassedReclaimProc = spTerminalBehavior->m_iNumberOfPassedReclaimProc;
		
		m_iSourceFloor = spTerminalBehavior->m_iSourceFloor;
		m_iDestFloor = spTerminalBehavior->m_iDestFloor;
		m_vPipePointList = spTerminalBehavior->m_vPipePointList;
		m_nextHoldAiearPoint = spTerminalBehavior->m_nextHoldAiearPoint;
		m_nextHoldAiearPoTag = spTerminalBehavior->m_nextHoldAiearPoTag;
        m_entryPointCorner = spTerminalBehavior->m_entryPointCorner;
        m_pPreFlowList = spTerminalBehavior->m_pPreFlowList;

		m_nQueuePathSegmentIndex = spTerminalBehavior->getQueuePathSegmentIndex();
	}
	
}


bool TerminalMobElementBehavior::ifGroupOccupyHeadsOfQueue()
{
	ProcessorQueue* pQueue = m_pProcessor->GetProcessorQueue();
	if( !pQueue )
		return true;

	MobileElementList* pWaitList = pQueue->GetWaitList();

	for (int i=0; i<(int)pWaitList->size(); i++)
	{
		Person* pCurrentPerson = (Person*)pWaitList->getItem( i );
		if (false == m_pPerson->m_pGroupInfo->IsSameGroup(pCurrentPerson))
			return false;
		if (pCurrentPerson == m_pPerson)
			break;
	}

	return true;
}


//get current time person's follower and  visitor 's info
//be used for determine processor IsVacant(),
//Note: if visitor will separated with it's leader, these visitors will be skipped
void TerminalMobElementBehavior::getFollowersAndVisitorsOccupancyInfo(ProcessorOccupantsState& snapshot)
{
	if (false == m_pPerson->m_pGroupInfo->IsFollower())
	{
		snapshot.AddMobileElementOccupant(m_pPerson->getType().GetTypeIndex());

		CGroupLeaderInfo* pGroupLeaderInfo = (CGroupLeaderInfo*)m_pPerson->m_pGroupInfo;
		if(true == pGroupLeaderInfo->isInGroup())
		{
			MobileElementList& followerList = pGroupLeaderInfo->GetFollowerList();
			for (int i=0 ; i<(int)followerList.size(); i++)
			{
				Person* pFollower = (Person*)followerList[i];
				snapshot.AddMobileElementOccupant(pFollower->getType().GetTypeIndex());
			}

			//NonPax do not has visitors
			if(m_pPerson->getType().GetTypeIndex() != 0)
				return;

			//NonPax
			int nVisitorCount = ((Passenger*)m_pPerson)->m_pVisitorList.size();
			for(int j=0; j<nVisitorCount; j++)
			{
				PaxVisitor* paxVisitor = ((Passenger*)m_pPerson)->m_pVisitorList[j];
				if(paxVisitor == NULL)//has throw error msg in pre-process
					continue;

				//call processor::IsVacant() when process "Server Departure",
				//visitor will not separated with it's leader at m_pPerson time,
				//it will separated with leader till process "free moving",
				//so, at m_pPerson time we should know if visitor has its destination
				//paxVisitor will separate with it's leader
				TerminalMobElementBehavior* spTerminalBehavior = (TerminalMobElementBehavior*)paxVisitor->getBehavior(MobElementBehavior::TerminalBehavior);

				if(spTerminalBehavior&&spTerminalBehavior->getProcessor()->HasDestination(paxVisitor->getType(), spTerminalBehavior->GetInGateDetailMode()))
					continue;
				snapshot.AddMobileElementOccupant(paxVisitor->getType().GetTypeIndex());
				CGroupLeaderInfo* pNonPaxGroupLeaderInfo = (CGroupLeaderInfo*)paxVisitor->m_pGroupInfo;
				if (true == pNonPaxGroupLeaderInfo->isInGroup())
				{
					MobileElementList& nonPaxfollowerList = pNonPaxGroupLeaderInfo->GetFollowerList();
					for (int i=0 ; i<(int)nonPaxfollowerList.size(); i++)
					{
						Person* pNonPaxFollower = (Person*)nonPaxfollowerList[i];
						snapshot.AddMobileElementOccupant(pNonPaxFollower->getType().GetTypeIndex());
					}
				}
			}
		}

	}
	else
		snapshot.AddMobileElementOccupant(m_pPerson->getType().GetTypeIndex());
}

//Resets path index.
void TerminalMobElementBehavior::updateIndex (void) 
{ 
	m_nCurrentIndex = m_nNextIndex; 
}

bool TerminalMobElementBehavior::StickVisitorListForDestProcesOverload(const ElapsedTime& _curTime)
{
	return false;
}

//Releases a person by set "stranded" false and set wait in stage flag as false.
void TerminalMobElementBehavior::release ( int _iStageID )
{
	m_nStranded = FALSE;
	if( m_mapWaitOnStage.find( _iStageID ) != m_mapWaitOnStage.end() )
	{
		m_mapWaitOnStage[ _iStageID ] = false;
	}	
}


//Returns moving time to next destination.
ElapsedTime TerminalMobElementBehavior::moveTime (void) const
{
	ElapsedTime t;

	if(m_pProcessor && m_pProcessor->getProcessorType()==BridgeConnectorProc)
	{
		double dL = location.distance3D(m_ptDestination);
		t = (float) (dL / (double)m_pPerson->getSpeed());
		return t;
	}
	//	if (location.getZ() != destination.getZ() || !location)
	if (!location)
		t = 0l;
	else
	{

		double dLxy = m_ptDestination.distance(location);
		double dLz = 0.0;
		int nFloorFrom = (int)(location.getZ() / SCALE_FACTOR);
		int nFloorTo = (int)(m_ptDestination.getZ() / SCALE_FACTOR);
		double dL = dLxy;
		if( nFloorFrom != nFloorTo )
		{

			/************************************************************************/
			/*        refactor level 3                                              */
			/************************************************************************/
			CTermPlanDoc* pDoc = (CTermPlanDoc*) ((CMDIChildWnd *)((CMDIFrameWnd*)AfxGetApp()->m_pMainWnd)->GetActiveFrame())->GetActiveDocument();
			assert( pDoc );
			dLz = pDoc->GetFloorByMode(EnvMode_Terminal).GetFloor2(nFloorFrom)->RealAltitude() - pDoc->GetFloorByMode(EnvMode_Terminal).GetFloor2(nFloorTo)->RealAltitude();
			if( dLz < 0 )
				dLz = -dLz;
			Point pt(dLxy, dLz, 0.0);
			dL = pt.length();
		}

		//		double time = destination.distance(location);
		double time = dL;
		t = (float) (time / (double)m_pPerson->getSpeed());
	}
	return t;
}


//Returns moving time to next destination.
ElapsedTime TerminalMobElementBehavior::moveTime( DistanceUnit _dExtraSpeed, bool _bExtra ) const
{
	ElapsedTime t;
	if(m_pProcessor && m_pProcessor->getProcessorType()==BridgeConnectorProc)
	{
		double dL = location.distance3D(m_ptDestination);
		t = (float) (dL / (double)m_pPerson->getSpeed());
		return t;
	}
	//	if (location.getZ() != destination.getZ() || !location)
	if (!location)
		t = 0l;
	else
	{
		DistanceUnit curSpeed = _dExtraSpeed;
		if( _bExtra )
			curSpeed += m_pPerson->getSpeed();

		double dLxy = m_ptDestination.distance(location);
		double dLz = 0.0;
		int nFloorFrom = (int)(location.getZ() / SCALE_FACTOR);
		int nFloorTo = (int)(m_ptDestination.getZ() / SCALE_FACTOR);
		double dL = dLxy;
		if( nFloorFrom != nFloorTo )
		{
			CTermPlanDoc* pDoc = (CTermPlanDoc*) ((CMDIChildWnd *)((CMDIFrameWnd*)AfxGetApp()->m_pMainWnd)->GetActiveFrame())->GetActiveDocument();
			assert( pDoc );
			dLz = pDoc->GetFloorByMode(EnvMode_Terminal).m_vFloors[nFloorFrom]->RealAltitude() - pDoc->GetFloorByMode(EnvMode_Terminal).m_vFloors[nFloorTo]->RealAltitude();
			if( dLz < 0 )
				dLz = -dLz;
			Point pt(dLxy, dLz, 0.0);
			dL = pt.length();
		}

		//		double time = destination.distance(location);
		double time = dL;
		t = (float) (time / (double)curSpeed );
	}
	return t;
}

// It is called by writeLogEntry after passing message to all other entities.
// It writes next track to ElementTrack log.
// in:
// _bBackup: if the log is backwards.
void TerminalMobElementBehavior::writeLog (ElapsedTime time, bool _bBackup, bool bOffset /*= true*/ )
{
	PLACE_METHOD_TRACK_STRING();
	MobEventStruct track;
	track.time = (long)time;
	track.state = LOBYTE( LOWORD(m_pPerson->state) );

	if (getProcessor() == 0)
		return;
	//////////////////////////////////////////////////////////////////////////
	BOOL bInBridge = IsWalkOnBridge() && getBridgeState()==DepBridge;
	BOOL bIsArrivalDoor = m_bIsArrivalDoor;
	BOOL bIsRealZ = bInBridge || (bIsArrivalDoor?TRUE:FALSE);
	//////////////////////////////////////////////////////////////////////////

	track.procNumber = (short)getProcessor()->getIndex();
	track.bDynamicCreatedProc = getProcessor()->GetDynamicCreatedFlag();
	track.followowner	= false;
	track.reason = m_iStopReason;
	track.backup = _bBackup;
	track.m_IsRealZ = bInBridge || (m_bIsArrivalDoor?TRUE:FALSE);

	PLACE_TRACK_STRING("2010224-11:04:47");
	if(m_pPerson->m_type.GetTypeIndex() != 0 )
	{
		// get object's Altitude
		CNonPaxRelativePosSpec* pNonPaxRelPosSpec = m_pTerm->GetNonPaxRelativePosSpec();
		ASSERT( NULL != pNonPaxRelPosSpec );
		CPoint2008 ptRelatePos;
		ARCVector4 rotation;
		pNonPaxRelPosSpec->GetNonPaxRelatePosition(ptRelatePos, rotation, m_pPerson->m_type.GetTypeIndex() );
		track.m_RealZ = (float)ptRelatePos.getZ();
		track.m_bWithOwner = ((VisitorTerminalBehavior*)this)->IfWalkWithOwner();


		if (track.m_bWithOwner)
		{
			track.m_bVisible = pNonPaxRelPosSpec->GetTypeItemVisible(m_pPerson->m_type.GetTypeIndex());
		}
	}

	PLACE_TRACK_STRING("2010223-11:09:29");
	pure_location	= m_ptDestination;
	Point pre_location = location;
	if (location != m_ptDestination)
	{
		Point vector (location, m_ptDestination);
		Point latPoint (vector.perpendicular());
		if( m_pPerson->m_type.GetTypeIndex() > 1 )// only pax and visitor have side step
		{
			latPoint.length( 0.0 );
		}
		else
		{
			latPoint.length (m_pPerson->getSideStep());
		}

		if(bOffset)
			location = m_ptDestination + latPoint;
		else
			location = m_ptDestination;

		//m_ptDestination = location;
		setDestination( location );
	}

	if (m_bIsArrivalDoor)
	{
		track.m_RealZ = (float)location.getZ();
		m_bIsArrivalDoor = false;
	}

	//	PLACE_METHOD_TRACK_STRING();
	// 	static long s_iiiiiiiiiiTmp = -1;
	// 	s_iiiiiiiiiiTmp = m_logEntry.getCurrentCount()*sizeof(MobEventStruct);
	location.getPoint (track.x, track.y, track.z);
	m_pPerson->getLogEntry().addEvent (track); // if simulation stops here, it means memory insufficient.
	PLACE_TRACK_STRING("2010223-11:09:08");
}

void TerminalMobElementBehavior::SetRelativePosition( const Point& paxDirection,const Point& paxLocation )
{
	CGroupLeaderInfo* pGroupLeaderInfo = (CGroupLeaderInfo*)m_pPerson->m_pGroupInfo;
	if(!pGroupLeaderInfo->isInGroup() || m_pPerson->m_pGroupInfo->IsFollower())
		return;
	m_ptDestination = getPoint();

	Person* pLeader = pGroupLeaderInfo->GetGroupLeader();
	TerminalMobElementBehavior* pLeaderBehavior = pLeader->getTerminalBehavior();
	MobileElementList &elemList = pGroupLeaderInfo->GetFollowerList();
	int nFollowerCount = elemList.getCount();
	for( int i=0; i< nFollowerCount; i++ )
	{
		Person* _pFollower = (Person*) elemList.getItem( i );
		if(_pFollower == NULL)
			continue;  

		TerminalMobElementBehavior* pFollowerBehavior = _pFollower->getTerminalBehavior();
		if (pFollowerBehavior == NULL)
			continue;

		CNonPaxRelativePosSpec* pNonPaxRelPosSpec = _pFollower->GetTerminal()->GetNonPaxRelativePosSpec();
		ASSERT( NULL != pNonPaxRelPosSpec );
		CPoint2008 ptRelatePos;
		ARCVector4 rotation;
		bool bRes = pNonPaxRelPosSpec->GetNonPaxRelatePosition(ptRelatePos, rotation, _pFollower->getType().GetTypeIndex(),i+1);

		if (bRes)
		{
			Point vectorLat( ptRelatePos.getX(),ptRelatePos.getY(), 0.0);
			Point ptCurrent(paxLocation);
			Point dir(paxDirection); 
		
			dir.Normalize();
			vectorLat.rotate(dir, ptCurrent);

			pFollowerBehavior->setDestination(Point(vectorLat.getX(),vectorLat.getY(),paxLocation.getZ()));
		}

	}
}

void TerminalMobElementBehavior::processEntryLandside( ElapsedTime p_time )
{
	// do not need departing to onboard mode.
	if( !simEngineConfig()->isSimLandsideMode())// if no selected, 
	{
		flushLog( p_time );
		return;
	}

	//----------------------------------------------------
	// sim Terminal and airside mode together.

	// arriving passenger do not need .
	if(!m_pPerson->getLogEntry().isArrival() )
	{
		flushLog( p_time );
		return;
	}
	
	// handle departing passenger's mode transfer action.

	//Must be Passenger!!!  (Passenger*)m_pPerson
	//By now, AirsideMobElementBehavior can just handle passenger's movement logic.
	if(m_pPerson->getLogEntry().GetMobileType() == 0)
	{
		//set enplane behavior
		ArrivalPaxLandsideBehavior* pLandsideBehavior = new ArrivalPaxLandsideBehavior(m_pPerson);
		m_pPerson->setBehavior(pLandsideBehavior); 
		//pEnplaneBehavior->SetGroupBehavior();
		//pEnplaneBehavior->SetFollowBehavior();
// 		CTermPlanDoc* pDoc = (CTermPlanDoc*) ((CMDIChildWnd *)((CMDIFrameWnd*)AfxGetApp()->m_pMainWnd)->GetActiveFrame())->GetActiveDocument();
// 		assert( pDoc );
		//double dLz = pDoc->GetFloorByMode(EnvMode_Terminal).getVisibleAltitude(location.getZ()/100.0);

		ARCVector3 curLocation = location;
		CARCportEngine* pEngine = getEngine();
		if (pEngine)
		{
			LandsideSimulation* pLandsideSim = pEngine->GetLandsideSimulation();
			if (pLandsideSim)
			{
				int nTerminalFloor = (int)(location.getZ() / 100.0);
				int nLandsideLevel = pLandsideSim->GetLinkLandsideFloor(nTerminalFloor);
				curLocation.n[VZ] = nLandsideLevel;
			}
		}

		pLandsideBehavior->setLocation( curLocation/*ARCVector3(location.getX(),location.getY(),dLz)*/);

		generateEvent(p_time,false);
		return;
	}
	else
	{
		// NonPax type(visitor) exit terminal mode.
		//ASSERT(m_pPerson->getLogEntry().GetMobileType() == 1);

		// if pax type is 2, baggage, throw exception.
		// by now, simEngine doesnot support baggage move alone to airside mode.

		flushLog (p_time);
		return;
	}

}

//--------------------------------------------------------------------------------------------
//Summary:
//		passenger enter from terminal to onboard mode need add passenger to onboard flight and 
//		check all passenger whether enter onboard flight
//--------------------------------------------------------------------------------------------
bool TerminalMobElementBehavior::EnterFromTerminalModeToOtherMode()
{
	CARCportEngine* pArcportEngine = getEngine();
	if (pArcportEngine == NULL)
		return false;

	if (!pArcportEngine->IsOnboardSel())
		return false;
	
	AirsideSimulation* pAirsideSim = pArcportEngine->GetAirsideSimulation();
	if (pAirsideSim == NULL)
		return false;
	
	AirsideFlightInSim* pAirsideFlightInSim = pAirsideSim->GetAirsideFlight(m_pPerson->GetCurFlightIndex());
	if (pAirsideFlightInSim == NULL)
		return false;
	
	OnboardSimulation* pOnboardSim = pArcportEngine->GetOnboardSimulation();
	if (pOnboardSim == NULL)
		return false;
	
	OnboardFlightInSim* pOnboardFlightInSim = pOnboardSim->GetOnboardFlightInSim(pAirsideFlightInSim,false);
	if (pOnboardFlightInSim == NULL)
		return false;
	
	if(m_pPerson->m_pGroupInfo->IsFollower())
	{
		pOnboardFlightInSim->RegisterPax(m_pPerson->getID());
	}
	else
	{
		CGroupLeaderInfo* pGroupLeaderInfo = (CGroupLeaderInfo*)m_pPerson->m_pGroupInfo;
		//add passenger
		pOnboardFlightInSim->RegisterPax(m_pPerson->getID());		
		if(pGroupLeaderInfo->isInGroup())
		{//pax in the group
			MobileElementList &elemList = pGroupLeaderInfo->GetFollowerList();
			int nFollowerCount = elemList.getCount();
			for( int i=0; i< nFollowerCount; i++ )
			{
				Person* _pFollower = (Person*) elemList.getItem( i );
				if(_pFollower != NULL)
				{
					pOnboardFlightInSim->RegisterPax(_pFollower->getID());
				}
			}
		}
	}

	return true;
}
//--------------------------------------------------------------------------------------
//Summary:
//		passenger from onboard to enter terminal and check door whether close
//Parameters:
//		tTime[in]: close door time
//--------------------------------------------------------------------------------------
void TerminalMobElementBehavior::EnterFromOnboardModeToTerminalMode(const ElapsedTime& tTime)
{
	CARCportEngine* pArcportEngine = getEngine();
	if (pArcportEngine == NULL)
		return;

	if (!pArcportEngine->IsOnboardSel())
		return;

	AirsideSimulation* pAirsideSim = pArcportEngine->GetAirsideSimulation();
	if (pAirsideSim == NULL)
		return;

	AirsideFlightInSim* pAirsideFlightInSim = pAirsideSim->GetAirsideFlight(m_pPerson->GetCurFlightIndex());
	if (pAirsideFlightInSim == NULL)
		return;

	OnboardSimulation* pOnboardSim = pArcportEngine->GetOnboardSimulation();
	if (pOnboardSim == NULL)
		return;

	OnboardFlightInSim* pOnboardFlightInSim = pOnboardSim->GetOnboardFlightInSim(pAirsideFlightInSim,false);
	if (pOnboardFlightInSim == NULL)
		return;

	pOnboardFlightInSim->DeRegisterPax(m_pPerson->getID());

	pOnboardFlightInSim->ProcessCloseDoor(tTime);
}

bool TerminalMobElementBehavior::ProcessPassengerSync( const ElapsedTime& _curTime )
{
	return true;
}

const ProcessorID * TerminalMobElementBehavior::GetEntryProcID() const
{
	Processor* pStartProc = m_pTerm->procList->getProcessor(START_PROCESSOR_INDEX);
	if(pStartProc)
	{
		ProcessorDistribution* pFlow = pStartProc->getNextDestinations ( m_pPerson->getType(), m_nInGateDetailMode);
		if(pFlow){
			return pFlow->getDestinationGroup(RANDOM);
		}
	}
	return NULL;	
}

bool TerminalMobElementBehavior::GetAvailbleEntryProcList( std::vector<Processor *>& vAvailableProc, int nFloor ) const
{
	Processor* pStartProc = m_pTerm->procList->getProcessor(START_PROCESSOR_INDEX);
	if(pStartProc)
	{
		ProcessorDistribution* pFlowList = pStartProc->getNextDestinations ( m_pPerson->getType(), m_nInGateDetailMode);
		if(pFlowList)
		{

			// For the possible destination, select one processor
			//dStepTime3 = GetTickCount();

			const ProcessorID *nextGroup = pFlowList->getDestinationGroup (RANDOM);

			do 
			{
				if(nextGroup != NULL)
				{
					GroupIndex indexGroup = m_pTerm->procList->getGroupIndex(*nextGroup);
					if(indexGroup.start >= 0)
					{
						for(int h = indexGroup.start; h <= indexGroup.end; h++)
						{
							Processor *pCurProc = m_pTerm->procList->getProcessor(h);
							if(pCurProc && pCurProc->getFloor() == nFloor)
							{
								vAvailableProc.push_back(pCurProc);
							}
						}
					}

				}
				// if randomly selected group invalid, try each group m_in order
			} while ((nextGroup = pFlowList->getDestinationGroup (SEQUENTIAL)) != NULL);

		}
	}
	return true;	
}

void TerminalMobElementBehavior::setLandsideSelectedProc( const std::vector<ALTObjectID>& altLandsideSelectedProc )
{
	m_vAltLandsideSelectedProc = altLandsideSelectedProc;
}

const std::vector<ALTObjectID>& TerminalMobElementBehavior::GetLandsideSelectedProc() const
{
	return m_vAltLandsideSelectedProc;
}

bool TerminalMobElementBehavior::GetAvailbleEntryProcList( std::vector<Processor *>& vAvailableProc ) const
{
	Processor* pStartProc = m_pTerm->procList->getProcessor(START_PROCESSOR_INDEX);
	if(pStartProc)
	{
		ProcessorDistribution* pFlowList = pStartProc->getNextDestinations ( m_pPerson->getType(), m_nInGateDetailMode);
		if(pFlowList)
		{

			const ProcessorID *nextGroup = pFlowList->getDestinationGroup (RANDOM);

			do 
			{
				if(nextGroup != NULL)
				{
					GroupIndex indexGroup = m_pTerm->procList->getGroupIndex(*nextGroup);
					if(indexGroup.start >= 0)
					{
						for(int h = indexGroup.start; h <= indexGroup.end; h++)
						{
							Processor *pCurProc = m_pTerm->procList->getProcessor(h);
							if(pCurProc == NULL)
								continue;

							if(pCurProc->isClosed())
								continue;
							if(!pCurProc->canServe(m_pPerson->m_type))
								continue;

							vAvailableProc.push_back(pCurProc);
							
						}
					}

				}
				// if randomly selected group invalid, try each group m_in order
			} while ((nextGroup = pFlowList->getDestinationGroup (SEQUENTIAL)) != NULL);

		}
	}
	return true;	
}

Processor * TerminalMobElementBehavior::TryToSelectLandsideEntryProcessor( const ElapsedTime& _timeToSelect )
{
	Processor* pStartProc = m_pTerm->procList->getProcessor(START_PROCESSOR_INDEX);
	if(pStartProc)
	{
		ProcessorDistribution* pFlowList = pStartProc->getNextDestinations ( m_pPerson->getType(), m_nInGateDetailMode);
		if(pFlowList)
		{
			m_pFlowList = pFlowList;
			return selectProcessor(_timeToSelect, false, false, false);
		}
	}

	return NULL;
}

double TerminalMobElementBehavior::CalculateWalkLengthOfUserPipe(const Point& ptFrom, const Point& ptTo, const std::vector<int>& vPipeList)
{
	// make sure pipe is on the same floor
	std::vector<int> vPipeList2;
	int nPipeCount = static_cast<int>(vPipeList.size());
	for(int i=0; i<nPipeCount; i++)
	{
		CPipe* pPipe = m_pTerm->m_pPipeDataSet->GetPipeAt(vPipeList[i]);
		if(pPipe->GetZ() == ptFrom.getZ())
		{
			vPipeList2.push_back(vPipeList[i]);
		}
	}

	nPipeCount = vPipeList2.size();
	if(nPipeCount == 0)
		return -1;
	
	CPointToPipeXPoint entryPoint;
	CPointToPipeXPoint exitPoint;
	std::vector<CMobPipeToPipeXPoint> vMidPoint;	// num count should be nPipeCount - 1

	CPipe* pPipe1 = NULL;
	CPipe* pPipe2 = NULL;
	for(int ii=0; ii<nPipeCount; ii++)
	{
		if(ii == 0)
		{
			pPipe1 = m_pTerm->m_pPipeDataSet->GetPipeAt(vPipeList2[0]);
			entryPoint = pPipe1->GetIntersectionPoint(ptFrom);

			if(nPipeCount == 1)
			{
				exitPoint = pPipe1->GetIntersectionPoint(ptTo);
			}
			else
			{
				pPipe2 = m_pTerm->m_pPipeDataSet->GetPipeAt(vPipeList2[1]);
				CMobPipeToPipeXPoint midPt;
				if(pPipe1->GetIntersectionPoint(pPipe2, entryPoint, midPt))
				{
					vMidPoint.push_back(midPt);
				}
				else
				{
					return -1;
				}
			}
		}
		else if(ii == nPipeCount-1)
		{
			exitPoint = pPipe1->GetIntersectionPoint(ptTo);
			vMidPoint[vMidPoint.size()-1].SetOutInc(exitPoint);
		}
		else
		{
			pPipe2 = m_pTerm->m_pPipeDataSet->GetPipeAt(vPipeList2[ii+1]);
			CMobPipeToPipeXPoint midPt;
			if( pPipe1->GetIntersectionPoint(pPipe2, vMidPoint[vMidPoint.size()-1], midPt))
			{
				vMidPoint[vMidPoint.size()-1].SetOutInc(midPt);
				vMidPoint.push_back(midPt);
			}
			else
			{
				return -1;
			}
		}
		pPipe1 = pPipe2;
	}
	
	// process entry point
	CPipe* pPipe = m_pTerm->m_pPipeDataSet->GetPipeAt(vPipeList2[0]);
	PTONSIDEWALK pointList, tempPTList;
	int nPercent = random(100);
	int nMidPoint = vMidPoint.size();
	double dDistance = 0.0f;
	if(nMidPoint == 0)
	{	
		m_bUserPipes = false;
		pPipe->GetPointListForLog(vPipeList2[0],entryPoint, exitPoint, nPercent, tempPTList);
		pointList.insert(pointList.end(), tempPTList.begin(), tempPTList.end());
	}
	else
	{
		pPipe->GetPointListForLog(vPipeList2[0], entryPoint, vMidPoint[0], nPercent, pointList);
		pointList.insert(pointList.end(), tempPTList.begin(), tempPTList.end());

		// process mid point 
		for(int i=1; i<nMidPoint; i++)
		{
			pPipe = m_pTerm->m_pPipeDataSet->GetPipeAt(vPipeList2[i]);
			if(vMidPoint[i-1].OrderChanged())
				nPercent = 100 - nPercent;

			pPipe->GetPointListForLog(vPipeList2[0],vMidPoint[i-1], vMidPoint[i], nPercent ,pointList);
			pointList.insert(pointList.end(), tempPTList.begin(), tempPTList.end());
		}

		// process exit point
		pPipe = m_pTerm->m_pPipeDataSet->GetPipeAt( vPipeList2[nPipeCount-1]);
		if(vMidPoint[nPipeCount-2].OrderChanged())
			nPercent = 100 - nPercent;
		pPipe->GetPointListForLog(vPipeList2[0],vMidPoint[nMidPoint-1], exitPoint, nPercent,pointList);
		pointList.insert(pointList.end(), tempPTList.begin(), tempPTList.end());
	}

	int ptCount = static_cast<int>(pointList.size());
	for(int i=0; i<ptCount; i++)
	{
		if(i == 0)
		{
			dDistance += ptFrom.distance(pointList[0]);
			continue;
		}
		dDistance+= pointList[i-1].distance(pointList[i]);
	}
	dDistance += pointList[ptCount-1].distance(ptTo);
	return dDistance;
}

void TerminalMobElementBehavior::processMoveToInterestedEntryPoint(ElapsedTime p_time)
{
	MoveToInterestedEntryEvent* pEvent = new MoveToInterestedEntryEvent;
	FixedQueue* pFixQ = (FixedQueue*)m_pProcessor->GetQueue();
	ASSERT(pFixQ->isFixed() == 'Y');
	setDestination(pFixQ->corner(m_entryPointCorner));
	pEvent->init(m_pPerson, p_time+moveTime(), false);
	pEvent->addEvent();
	m_pPerson->SetPrevEventTime(pEvent->getTime());
	return;
}

void TerminalMobElementBehavior::SetWalkOnBridge(BOOL b)
{
	m_IsWalkOnBridge = b;
}








