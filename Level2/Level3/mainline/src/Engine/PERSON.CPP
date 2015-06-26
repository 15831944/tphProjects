#include "stdafx.h"

#include "person.h"

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
#include <Inputs\ProcessorDistributionWithPipe.h>
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
#include "TerminalMobElementBehavior.h"
#include "AirsideMobElementBehavior.h"
#include "PaxOnboardBaseBehavior.h"

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
#include "ARCportEngine.h"
#include "AirsideMobElementBaseBehavior.h"

extern std::vector<ConvergePair *> GreetPool;

const long PAX_PICKUP_BAG_INTERUPT_SECOND = 1;
float * far Person::m_pRotation = new float[MAX_GROUP];

//Initializes rotaiton.
void Person::initSpacing (void)
{
    float rotDegrees[8] = {0.0, 90.0, 270.0, 30.0, 310.0, 130.0, 230.0};
    memcpy (m_pRotation, rotDegrees, MAX_GROUP * sizeof (float));
}

//Initializes a person according to pax log entry input.
//Set person state "Birth"; set persont m_type either "A"rrival or "D"eparture.
//Set person speed, side step, m_in step.
Person::Person (MobLogEntry& p_entry, CARCportEngine *_pEngine ) :
MobileElement (p_entry.getID(), 0)
{
	PLACE_METHOD_TRACK_STRING();
	// Mobile element birth in terminal mode. (default)
//	MobileElement::setBehavior( new TerminalMobElementBehavior( this ));
	MobileElement::setState( Birth );
	m_iPreState = state;
	
	m_pEngine = _pEngine;

	m_pTerm = GetTerminal();
    m_logEntry = p_entry;
	m_logEntry.SetEventLog( GetTerminal()->m_pMobEventLog );
	m_logEntry.setTempLogFile(GetTerminal()->m_pTempMobEventLog);
	
		
    // get flight related info
    //int flightIndex;
    if (m_logEntry.isDeparting())
    {
        m_ilightIndex = m_logEntry.getDepFlight();
        m_type = initType (m_ilightIndex, 'D');
		m_type.setFlightIndex(m_ilightIndex);
	    Flight *pDepFlight = getEngine()->m_simFlightSchedule.getItem (m_ilightIndex);
		int iArrFltIndex = m_logEntry.getArrFlight();
		if( iArrFltIndex > -1 )
		{
			Flight* pArrFlight = getEngine()->m_simFlightSchedule.getItem ( iArrFltIndex );
			m_nBagCarosel = pArrFlight->getBaggageCarosel();
		}
		else
			m_nBagCarosel = -1;
		m_nGate = pDepFlight->getDepGateIndex();	
    }
    else
    {
        m_ilightIndex = m_logEntry.getArrFlight();
        m_type = initType (m_ilightIndex, 'A');
		m_type.setFlightIndex(m_ilightIndex);
		Flight *aFlight = getEngine()->m_simFlightSchedule.getItem (m_ilightIndex);
	    m_nBagCarosel = aFlight->getBaggageCarosel();
		m_nGate = aFlight->getArrGateIndex();
    }
	if(m_logEntry.GetMobileType() == 0)
		m_type.SetMobileElementType(enum_MobileElementType_PAX);
	else
		m_type.SetMobileElementType(enum_MobileElementType_NONPAX);

	m_type.SetTypeIndex( m_logEntry.GetMobileType() );
	m_type.getNonpaxCount().init();


		
	assert( m_pTerm );
	m_type.SetInputTerminal( m_pTerm );

    // speed	
    speed = p_entry.getSpeed();
	speed *= SCALE_FACTOR;
		
    // m_pSideStep
    CMobileElemConstraintDatabase *sideStepDB = GetTerminal()->paxDataList->getSideStep();
    m_pSideStep = sideStepDB->FindFit(m_type);
    if (m_pSideStep == NULL)
        throw new StringError ("No side step available");

    // inStep
    CMobileElemConstraintDatabase *inStepDB = GetTerminal()->paxDataList->getInStep();
    const ProbabilityDistribution *inStep = inStepDB->FindFit(m_type);
    if (inStep == NULL)
        throw new StringError ("No m_in step available");

	CMobileElemConstraintDatabase* pImpactInStepDB = GetTerminal()->paxDataList->getImpactInStep();
	double dImpact = 0.0;
	const ProbabilityDistribution* pImpactInStep = pImpactInStepDB->FindFit(m_type);
	if(pImpactInStep != NULL)
	{
		dImpact = pImpactInStep->getMean()/100.0;	
	}

	m_in = inStep->getRandomValue();
	

    //m_spacing = (DistanceUnit)(inStep->getRandomValue() * SCALE_FACTOR * 0.75);

	if( p_entry.getGroupMember() == 0 || (p_entry.GetMobileType() == 2 && p_entry.getGroupMember() == 0))
	{
		m_pGroupInfo = new CGroupLeaderInfo( p_entry.getGroupSize(), this );
	}
	else 
		m_pGroupInfo = new CGroupFollowerInfo( p_entry.getGroupSize() );

}

Person::~Person()
{	
	if( m_pGroupInfo )
		delete m_pGroupInfo;

	int nPairCount =  m_vpConvergePair.size();

	for(int nPair = 0; nPair < nPairCount; ++nPair)
	{
		std::vector<ConvergePair *>::iterator iterFind = std::find(GreetPool.begin(), GreetPool.end(),m_vpConvergePair[nPair]); 
		if(iterFind == GreetPool.end())
			continue;
		delete *iterFind;
		GreetPool.erase(iterFind);
	}

	m_vpConvergePair.clear();


}


//Sets and returns passenger m_type.
CMobileElemConstraint Person::initType (int p_flight, char p_mode)
{
	PLACE_METHOD_TRACK_STRING();
	CMobileElemConstraint mobType(m_pTerm);
	assert( m_pTerm );
    Flight *aFlight = getEngine()->m_simFlightSchedule.getItem (p_flight);
    (FlightConstraint &)mobType = aFlight->getType (p_mode);
    mobType.setIntrinsicType (m_logEntry.getIntrinsic());
    int types[MAX_PAX_TYPES];
    m_logEntry.getPaxType (types);
    mobType.setUserType (types);
    return mobType;
}


DistanceUnit Person::getSideStep (void) const
{
    return (DistanceUnit)(m_pSideStep->getRandomValue() * SCALE_FACTOR);
}

/**
	Person::move():
	Function to provide Mobile Element Movement Action in ALTO Mode.
	In different Mode( Airside, Landside, Terminal, OnBoard), Mobile Element has
	concrete movement logic. These logic are implemented in concrete movement behavior class.
	
	NOTE:	* "Person" is not a people, it is a mobile element which has movement capability in ALTO System.
			* Mobile element has terminal mode movement capability as default. (TerminalMobElementBehavior)

	Parameters:
		p_time: Movement driving event's time.
		bNoLog: Identify if need write "previous" action's log.

	Return:
		TRUE:	Most situation return TRUE,
				1.MobElement find next destination. (will get on with movement)
				2.MobElement cannot find next destination. (will stop to move)
				3.MobElement arrive at last destination in ALTO System.(will stop to move)
	
		FALSE:	Exceptive situation: when MobElement MoveAlongOutConstraint (in Terminal mode)
				1. MobElement cannot find next destination. (will stop to move)
				(* TODO:: should avoid return false)

	Mode Transfer:
	When Mobile element arrive at last destination in current mode, "ModeTransfer" event generated.
	In ModeTransfer state, mobile element ask MobElmenets transfer service for next destination.
	
	If need move into next mode, Appropriate behavior object will be append to MobElement, 
	New movement driving event will be generated, and then, MobElement has fresh movement capability
	in next mode.  
	Otherwise, MobElement arrive at last destination in ALTO system, and will stop to move.
*/
int Person::move(ElapsedTime p_time,bool bNoLog)
{
	PLACE_METHOD_TRACK_STRING();
	if(getCurrentBehavior() == NULL)
	{
		flushLog (p_time);
		CString strErrorMsg = getPersonErrorMsg();
		throw new ARCSimEngineException(_T("Lost movement capability"),strErrorMsg, p_time.printTime());
	}

	// set interested mobile elements id: "CTRL + F8" 
	MobElementsDiagnosis()->Diagnose( this, p_time);

	return getCurrentBehavior()->performanceMove( p_time, bNoLog );

}

// MOVEMENTS' methods.
// It deciphers Person's state to determine movement logic
// Returns true if Person is to be deleted.

//<TODO::>  Reconstruction: Migrate terminal mode movement logic code to TerminalMobElementBehavior class.
extern long eventNumber;
int Person::processTerminalModeMovement(ElapsedTime p_time,bool bNoLog)
{
	PLACE_METHOD_TRACK_STRING();
	id;
	eventNumber;
    
	TerminalMobElementBehavior* spTerminalBehavior = (TerminalMobElementBehavior *)getBehavior(MobElementBehavior::TerminalBehavior);
	if (spTerminalBehavior == NULL)
	{
		return 0;
	}

	if( getEngine()->GetFireOccurFlag() && spTerminalBehavior->m_timeFireEvacuation <= p_time && 
		!spTerminalBehavior->m_bHasProcessedFireEvacuation && !spTerminalBehavior->m_bPersonInStationSystem )
	{
		spTerminalBehavior->ProcessFireEvacuation( p_time );
		return TRUE;
	}

	// update checked bag's state on reclaim  if need.
	spTerminalBehavior->updateCheckedBagStateOnReclaim( p_time );

	// process person on station if need.
	bool _needReturn, _localResult;
	spTerminalBehavior->processPersonOnStation(p_time, _needReturn, _localResult);
	if(_needReturn)
		return _localResult;


   int nReturnValue = 1;
    int nPersonID = getID();
    CString strProc = spTerminalBehavior->getProcessor()->getIDName();
	switch (state)
    {

        // determine first m_pProcessor and starting location
        case Birth:
			if (bNoLog == false)
			{ 
				writeLogEntry (p_time, false);
			}
            spTerminalBehavior->processBirth (p_time);
			break;

		case WalkOnBridge:
			if (bNoLog == false)
			{ 
				writeLogEntry (p_time, false);
			}
			 spTerminalBehavior->processBridge(p_time);
			break;
		case EntryAirside:
			if (bNoLog == false)
			{ 
				writeLogEntry (p_time, false);
			}
			 spTerminalBehavior->processEntryAirside (p_time);
			break;
		case EntryOnboard://if onboard  needs simulation, move to onboard; if no, flush log
			{
				if(bNoLog == false)
					writeLogEntry (p_time, false);
			 
				spTerminalBehavior->processEntryOnboard (p_time);
			}
			break;
		case EntryLandside:
			{
				//if(bNoLog == false)
				//	writeLogEntry(p_time,false);

				spTerminalBehavior->processEntryLandside(p_time);
			}
			break;
        // determine next Processor and find a direct path to it
        case FreeMoving:
			if (bNoLog == false)
			{ 
				writeLogEntry (p_time, false);
			}
             spTerminalBehavior->processGeneralMovement (p_time);
            break;


        case MoveAlongInConstraint:
			if (bNoLog == false)
			{ 
				writeLogEntry (p_time, false);
			}
             spTerminalBehavior->processInConstraint (p_time);
            break;
		case LeaveInConstraint:
			if (bNoLog == false)
			{ 
				writeLogEntry (p_time, false);
			}
			 spTerminalBehavior->processGeneralMovement (p_time);
            break;

        case WaitInQueue: case MoveToQueue: case StartMovement:
			if (bNoLog == false)
			{ 
				writeLogEntry (p_time, false);
			}
             spTerminalBehavior->processQueueMovement (p_time);
            break;

        case LeaveQueue:
			if (bNoLog == false)
			{ 
				writeLogEntry (p_time, false);
			}
             spTerminalBehavior->processQueueDeparture (p_time);
            break;

        case ArriveAtServer:
			if (spTerminalBehavior->getProcessor()->getProcessorType() != StairProc) //StairProc need Bidirectional
			{
				if (bNoLog == false)
				{ 
					writeLogEntry (p_time, false);
				}			
			}            
             spTerminalBehavior->processServerArrival (p_time);
            break;
		case Greeting :
			if (bNoLog == false)
			{ 
				writeLogEntry (p_time, false);
			}
			 spTerminalBehavior->processGreeting( p_time );

			break;
        case LeaveServer:
			if (bNoLog == false)
			{ 
				writeLogEntry (p_time, false);
			}
            nReturnValue =  spTerminalBehavior->processServerDeparture (p_time);
			break;

        // Used only to get next destination after proceeding to end of
        // out constraint
        case MoveAlongOutConstraint:
			if (bNoLog == false)
			{ 
				writeLogEntry (p_time, false);
			}
			setState( FreeMoving );
			////stick this person when dest processor exceed its capacity.
			//if(StickForDestProcsOverload(p_time) ==true)
			//	return TRUE;

            if (! spTerminalBehavior->getNextProcessor (p_time))
			{
				if(getState() == EntryAirside)
					return TRUE;// arrive at End Processor. try to process transfer event.
				else
					return FALSE;// cannot move to next processor, will death.
			}

            generateEvent (p_time + moveTime(),false);
            break;

        case WaitInHoldingArea:
			if (bNoLog == false)
			{ 
				writeLogEntry (p_time, false);
			}
            spTerminalBehavior->m_nWaiting = TRUE;
            break;
		case EvacuationFire:
			 spTerminalBehavior->ProcessFireEvacuation( p_time );
			break;
		case AvoidDensitying:
			if (bNoLog == false)
			{ 
				writeLogEntry (p_time, false);
			}
			break;
		case WalkOnPipe:
			if (bNoLog == false)
			{ 
				writeLogEntry (p_time, false);
			}
			 spTerminalBehavior->processGeneralMovement (p_time);
			break;
		case MoveToInterestedEntryPoint:
			spTerminalBehavior->processMoveToInterestedEntryPoint(p_time);
			break;
		default:
			break;
    }

    return 1;
}   


void Person::setCurrentPosition (ElapsedTime moveTime)
{
	PLACE_METHOD_TRACK_STRING();
    MobileElementMovementEvent dummy;
    ElapsedTime destTime = dummy.removeMoveEvent (this);
	TerminalMobElementBehavior* spTerminalBehavior = getTerminalBehavior();

    if (destTime >= ElapsedTime(0l))
	{
        spTerminalBehavior->setDestination (spTerminalBehavior->location.calcMidPoint (spTerminalBehavior->m_ptDestination, speed, (destTime - moveTime)));
	}
    writeLogEntry (moveTime, false);
}

#define MIN_DISTANCE    50l

Point Person::offsetCoords (Point p1, Point p2, float dir, DistanceUnit dist)
{
    Point vector = p1 - p2;
    vector.length ((double)dist);
    vector.rotate ((double)dir);
    return p2 + vector;
}

 
void Person::adjustCoordinates (MobEventStruct *list, int count,
								float direction, DistanceUnit distance)
{
	PLACE_METHOD_TRACK_STRING();
    if (direction == 0.0 || distance == 0l)
        return;

    Point p1, p2;
    p2.setPoint (list[0].x, list[0].y, list[0].z);
    Point curPoint = p2;
	//char tempPoints[126];
    for (int i = 1; i < count-1; i++)
    {
        p1 = p2;
        p2.setPoint (list[i].x, list[i].y, list[i].z);

		//p2.printPoint(tempPoints , 1 );
		//// TRACE ("\n%s\n", tempPoints);
        if (p1 != p2)
            curPoint = offsetCoords (p1, p2, direction, distance);
		else
		{
			if( p1.getZ() != p2.getZ() )
			{
				curPoint = p2;
			}
		}
        curPoint.getPoint (list[i].x, list[i].y, list[i].z);
    }
}

void Person::adjustCoordinates (std::vector<MobEventStruct>& list, int count, 
								float direction, DistanceUnit distance )
{
	PLACE_METHOD_TRACK_STRING();
	if (direction == 0.0 || distance == 0l)
        return;
	
    Point p1, p2;
    p2.setPoint (list[0].x, list[0].y, list[0].z);
    Point curPoint = p2;
	//char tempPoints[126];
    for (int i = 1; i < count-1; i++)
    {
        p1 = p2;
        p2.setPoint (list[i].x, list[i].y, list[i].z);
		
		//p2.printPoint(tempPoints , 1 );
		//// TRACE ("\n%s\n", tempPoints);
        if (p1 != p2)
            curPoint = offsetCoords (p1, p2, direction, distance);
		else
		{
			if( p1.getZ() != p2.getZ() )
			{
				curPoint = p2;
			}
		}
        curPoint.getPoint (list[i].x, list[i].y, list[i].z);
    }
}


void Person::adjustCoordinates (std::vector<MobEventStruct>& list, int _beginPos, int _endPos, 
								float direction, DistanceUnit distance )
{
	PLACE_METHOD_TRACK_STRING();
	if (direction == 0.0 || distance == 0l)
        return;
	
	assert( _endPos>=_beginPos );

    Point p1, p2;
    p2.setPoint (list[_beginPos].x, list[_beginPos].y, list[_beginPos].z);
    Point curPoint = p2;
	//char tempPoints[126];
    for (int i = _beginPos+1; i < _endPos; i++)
    {
        p1 = p2;
        p2.setPoint (list[i].x, list[i].y, list[i].z);
		
		//p2.printPoint(tempPoints , 1 );
		//// TRACE ("\n%s\n", tempPoints);
        if (p1 != p2)
            curPoint = offsetCoords (p1, p2, direction, distance);
		else
		{
			if( p1.getZ() != p2.getZ() )
			{
				curPoint = p2;
			}
		}
        curPoint.getPoint (list[i].x, list[i].y, list[i].z);
    }
}

void Person::writeTerminalBeginLogEntry(ElapsedTime p_time)
{
	PLACE_METHOD_TRACK_STRING();
	int nState = state;
	state = TerminalBegin;
	writeLogEntry(p_time,false);
	state = nState;
}

void Person::writeTerminalEndLogEntry(ElapsedTime p_time)
{
    int nPersonID = getID();
	PLACE_METHOD_TRACK_STRING();
	int nState = state;
	setState(TerminalEnd);
	writeLogEntry(p_time,false);
	state = nState;
}

 
// It is called by writeLogEntry after passing message to all other entities.
// It writes next track to ElementTrack log.
// in:
// _bBackup: if the log is backwards.
void Person::writeLog (ElapsedTime time, bool _bBackup, bool bOffset /*= true*/ )
{
	PLACE_METHOD_TRACK_STRING();
    MobEventStruct track;
    track.time = (long)time;
	track.state = (short)state;//LOBYTE( LOWORD(state) );
	TerminalMobElementBehavior* spTerminalBehavior = getTerminalBehavior();
	if (spTerminalBehavior == NULL)
	{
		return;
	}

	if (spTerminalBehavior->getProcessor() == 0)
    	return;

	if (state == TerminalBegin)
	{
		m_logEntry.setTerminalEntryTime(time);
	}
	else if (state == TerminalEnd)
	{
		m_logEntry.setTerminalEndTime(time);
	}

	//////////////////////////for bridge////////////////////////////////////////////////
	BOOL bInBridge = spTerminalBehavior->IsWalkOnBridge() && spTerminalBehavior->getBridgeState()==DepBridge;
	BOOL bIsArrivalDoor = spTerminalBehavior->m_bIsArrivalDoor;
	BOOL bIsRealZ = bInBridge || (bIsArrivalDoor?TRUE:FALSE);
	//////////////////////////////////////////////////////////////////////////


	track.procNumber = (short)spTerminalBehavior->getProcessor()->getIndex();
	track.bDynamicCreatedProc = spTerminalBehavior->getProcessor()->GetDynamicCreatedFlag();
	track.followowner	= false;
	track.reason = spTerminalBehavior->m_iStopReason;
	track.backup = _bBackup;
	track.m_IsRealZ = bIsRealZ;

	PLACE_TRACK_STRING("2010224-11:04:47");
	if(m_type.GetTypeIndex() != 0 )
	{
		// get object's Altitude
		CNonPaxRelativePosSpec* pNonPaxRelPosSpec = m_pTerm->GetNonPaxRelativePosSpec();
		ASSERT( NULL != pNonPaxRelPosSpec );
		CPoint2008 ptRelatePos;
		ARCVector4 rotation;
		pNonPaxRelPosSpec->GetNonPaxRelatePosition(ptRelatePos, rotation, m_type.GetTypeIndex() );
		track.m_RealZ = (float)ptRelatePos.getZ();
		track.m_bWithOwner = ((VisitorTerminalBehavior*)spTerminalBehavior)->IfWalkWithOwner();
		
		
		if (track.m_bWithOwner)
		{
			track.m_bVisible = pNonPaxRelPosSpec->GetTypeItemVisible(m_type.GetTypeIndex());
		}
	}

	PLACE_TRACK_STRING("2010223-11:09:29");
	spTerminalBehavior->pure_location	= spTerminalBehavior->m_ptDestination;
	Point pre_location = spTerminalBehavior->location;
    if (spTerminalBehavior->location != spTerminalBehavior->m_ptDestination || spTerminalBehavior->location.getZ() != spTerminalBehavior->m_ptDestination.getZ())
    {
        Point vector (spTerminalBehavior->location, spTerminalBehavior->m_ptDestination);
        Point latPoint (vector.perpendicular());
		if( this->m_type.GetTypeIndex() > 1 )// only pax and visitor have side step
		{
			latPoint.length( 0.0 );
		}
		else
		{
			latPoint.length (getSideStep());
		}

		if(bOffset)
			spTerminalBehavior->location = spTerminalBehavior->m_ptDestination + latPoint;
		else
			spTerminalBehavior->location = spTerminalBehavior->m_ptDestination;

        //m_ptDestination = location;
		spTerminalBehavior->setDestination( spTerminalBehavior->location );
    }

	if (bIsRealZ)
	{
		track.m_RealZ = (float)spTerminalBehavior->location.getZ();
		spTerminalBehavior->m_bIsArrivalDoor = false;
	}

//	PLACE_METHOD_TRACK_STRING();
// 	static long s_iiiiiiiiiiTmp = -1;
// 	s_iiiiiiiiiiTmp = m_logEntry.getCurrentCount()*sizeof(MobEventStruct);
    spTerminalBehavior->location.getPoint (track.x, track.y, track.z);
    m_logEntry.addEvent (track); // if simulation stops here, it means memory insufficient.
	PLACE_TRACK_STRING("2010223-11:09:08");

	//if(id == 105 || id == 104 || id == 38)
	//{
	//	TRACE(" \r\n ------------------------------------- %d, %d, %d, %s, %s", id, track.time, (int)track.state, STATE_NAMES[(int)track.state], time.printTime());
	//}




}



// Used to determine if the gate is used as arrival purpose.
bool Person::CheckAndTurnArrState()
{
	if( m_type.GetFltConstraintMode() == ENUM_FLTCNSTR_MODE_ARR )
		return true;
	return false;
}

CString Person::getPersonErrorMsg( void ) const
{
	PLACE_METHOD_TRACK_STRING();
	// pax id
	CString paxID;
	paxID.Format("%d <@>",id );

	// get flight id;
	CString strFlightID;
	char airline[AIRLINE_LEN];
	m_type.getAirline(airline);
	if( airline[0] )
	{
		if( m_type.getFlightID() )
		{
			strFlightID.Format( "%s",m_type.getFlightID().GetValue() );
		}
		strFlightID = CString( airline) + strFlightID;
	}
	
	strFlightID +=" <@>";

	// mob type
	//char szMobType[256];
	CString sMobType;
	m_type.screenPrint( sMobType, 0, 128 );
	
	return paxID + strFlightID + sMobType + " <@>";
}

// void Person::setDestination( Point p )
// {
// 	m_ptDestination = p;
// 	m_pGroupInfo->SetFollowerDestination( location, m_ptDestination, m_pRotation );
// }


// flash group follower's log
void Person::flushGroupFollowerLog( const ElapsedTime& time, float dir, int _iGroupIndex  )
{
	PLACE_METHOD_TRACK_STRING();
	m_logEntry.setExitTime( time );

	long trackCount = m_logEntry.getCurrentCount();
	
	//adjustCoordinates ( m_logEntry.getEventList(), trackCount, dir, GROUP_OFFSET );
//	ajdustTimeforDisallowGroupProc( m_logEntry.getEventList(), trackCount, _iGroupIndex );
	
	m_logEntry.setEventList (m_logEntry.getEventList());
	
	m_pTerm->paxLog->updateItem (m_logEntry, m_logEntry.getIndex());

	m_logEntry.clearLog();
}




// copy pax event log
void Person::copyPassengerEventLog( long _lBeginIdx, long _lEndIdx, const MobLogEntry& _srcLogEntry )
{
	assert( _lEndIdx >= _lBeginIdx );

	std::copy( _srcLogEntry.getEventList().begin() + _lBeginIdx, _srcLogEntry.getEventList().begin() + _lEndIdx ,
			   std::back_inserter( m_logEntry.getEventList() ) );
}


//Generates the next movement event for the receiver based on its current state and adds it to the event list.
// when m_bEvacuationnWhenInPipe is true, shall not generate any event.
void Person::generateEvent (ElapsedTime eventTime ,bool bNoLog)
{
	PLACE_METHOD_TRACK_STRING();
	TerminalMobElementBehavior* spTerminalBehavior = getTerminalBehavior();
	if (spTerminalBehavior == NULL)
	{
		return;
	}

	if( spTerminalBehavior->m_bEvacuationWhenInPipe )
		return;
	
	bool bDisallowGroup = spTerminalBehavior->getProcessor()->GetDisallowGroupFlag()|| 
		spTerminalBehavior->getProcessor()->getProcessorType() == ConveyorProc;
	//check is Entry Onboard
	if(m_pEngine->IsOnboardSel())
	{
		if(spTerminalBehavior->getProcessor()->getProcessorType() == BridgeConnectorProc && m_type.isDeparture())
			bDisallowGroup = true;

		if(getState() == EntryOnboard)
			bDisallowGroup = true;
	}

	enum EVENT_OPERATION_DECISTION enumRes = m_pGroupInfo->ProcessEvent( eventTime, bDisallowGroup );
	if( enumRes == REMOVE_FROM_PROCESSOR_AND_NO_EVENT )
	{
		spTerminalBehavior->getProcessor()->removePerson( this );
		spTerminalBehavior->writeLogEntry(eventTime, false);
		return;
	}
	if( enumRes == NOT_GENERATE_EVENT )
	{
		spTerminalBehavior->writeLogEntry(eventTime, false);
		return;
	}

	if( !spTerminalBehavior->needToAvoidDensity() )
	{
		MobileElement::generateEvent( eventTime, bNoLog);
	}
	else		// need to modify event to avoid density
	{
		if( spTerminalBehavior->location == spTerminalBehavior->m_ptDestination  || spTerminalBehavior->location.getZ() != spTerminalBehavior->m_ptDestination.getZ() )
		{
			MobileElement::generateEvent( eventTime , bNoLog);
		}
		else
		{
			// if have no intersect point, need not to modify event
			if( !m_pTerm->m_pAreas->getAllIntersectPoint2( spTerminalBehavior->location, spTerminalBehavior->pure_location, spTerminalBehavior->m_ptDestination, eventTime, speed, spTerminalBehavior->m_intersectPointList ) )
			{
				MobileElement::generateEvent( eventTime , bNoLog);
			}
			else // need to modify event to avoid density
			{
				spTerminalBehavior->doSomethingBeforeAvoid();
				// add ARRIVAL_DEST state
				INTERSECT_INFO_LIST _info_list;
				_info_list.push_back( INTERSECT_INFO( ARRIVAL_DEST, NULL, -1 ) );
				spTerminalBehavior->m_intersectPointList.push_back( AreaIntersectPoint( spTerminalBehavior->getDest(), eventTime, _info_list) );

				Point _realDest  = spTerminalBehavior->getDest();
				int	  _realState = getState();
				AreaIntersectPoint _intersectPoint = spTerminalBehavior->m_intersectPointList[0];
				spTerminalBehavior->setDestination( _intersectPoint.point );
				setState( AvoidDensitying );
				//////////////////////////////////////////////////////////////////////////

				generateAvoidDensityEvent( _intersectPoint, _realState, _realDest ,false);
				spTerminalBehavior->m_intersectPointList.erase( spTerminalBehavior->m_intersectPointList.begin() );
			}
		}
	}
}	


// generate avoid density event
void Person::generateAvoidDensityEvent( const AreaIntersectPoint& _intersectPoint, int _iRealState, const Point& _ptRealDest ,bool bNoLog)
{
	PLACE_METHOD_TRACK_STRING();
	AvoidDensityEvent* _pAvoidDensity = new AvoidDensityEvent;
	ElapsedTime _destTime = _pAvoidDensity->removeMoveEvent (this);
	
	_pAvoidDensity->InitEx( this, _intersectPoint.intersectTime, _iRealState, _ptRealDest, _intersectPoint.intersectInfo,bNoLog );
	_pAvoidDensity->addEvent();
	
	m_prevEventTime = _pAvoidDensity->getTime();
}


// check if is passenger for visitor person.
bool Person::IsHuman()
{
	if( m_type.GetTypeIndex() <= 1 )
		return true;

	return false;
}

int Person::GetCurFlightIndex()
{
	int flightId = -1 ;
	TerminalMobElementBehavior* spTerminalBehavior = getTerminalBehavior();

	if(getLogEntry().isTurnaround() && spTerminalBehavior)
	{
		if(spTerminalBehavior->GetTransferTypeState() == TerminalMobElementBehavior::TRANSFER_ARRIVAL)
			flightId = getLogEntry().getArrFlight() ;
		else
			flightId = getLogEntry().getDepFlight() ;
	}
	else
		flightId = getLogEntry().getFlightIndex() ;

	return flightId;
}

// Repeat last log item in the m_logEntry with new destination set by former code
// this is useful for PaxVisitor to make a quick shift
// 
// NOTE: to use this method correctly, look at the implementation.
void Person::RepeatLastLogWithNewDest()
{
	//long lLogCount = m_logEntry.getCurrentCount();
	TerminalMobElementBehavior* spTerminalBehavior = getTerminalBehavior();
	MobEventStruct *pPreTrack = m_logEntry.getPreEvent();
	if( pPreTrack )
	{
		MobEventStruct eventItem = *pPreTrack;		
		spTerminalBehavior->m_ptDestination.getPoint(eventItem.x, eventItem.y, eventItem.z);

		m_logEntry.addEvent(eventItem);
	}
}

void Person::SetFollowerArrivalDoor(bool bArrivalDoor)
{
	m_pGroupInfo->setFollowerArrivalDoor(bArrivalDoor);
}

void Person::SetEntryTime(const ElapsedTime& tEntryTime)
{
	m_pGroupInfo->setEntryTime(tEntryTime);
}
const Terminal* Person::GetTerminal() const
{
	return m_pEngine->getTerminal();
}

Terminal* Person::GetTerminal()
{
	return m_pEngine->getTerminal();
}
CARCportEngine * Person::getEngine()
{
	return m_pEngine;
}

const CARCportEngine * Person::getEngine() const
{
	return m_pEngine;
}

bool Person::getTerminalPoint(Point& pt)const
{
	PLACE_METHOD_TRACK_STRING();
	TerminalMobElementBehavior* spTerminalBehavior = getTerminalBehavior();
	if (spTerminalBehavior)
	{
		pt = spTerminalBehavior->getPoint();
		return true;
	}
	return false;
}

bool Person::getAirsidePoint(CPoint2008& pt)const
{
	PLACE_METHOD_TRACK_STRING();
	MobElementBehavior* spBehavior = getCurrentBehavior();
	if (spBehavior->getBehaviorType() == MobElementBehavior::AirsideBehavior)
	{
		AirsideMobElementBaseBehavior* spAirsideBehavior = getAirsideBehavior();
		pt = spAirsideBehavior->getPoint();
		return true;
	}
	return false;
}

bool Person::getTerminalDest(Point& pt)const
{
	PLACE_METHOD_TRACK_STRING();
	TerminalMobElementBehavior* spTerminalBehavior = getTerminalBehavior();
	if (spTerminalBehavior)
	{
		pt = spTerminalBehavior->getDest();
		return true;
	}
	return false;
}

bool Person::getAirsideDest(CPoint2008& pt)const
{
	PLACE_METHOD_TRACK_STRING();
	MobElementBehavior* spBehavior = getCurrentBehavior();
	if (spBehavior&&spBehavior->getBehaviorType() == MobElementBehavior::AirsideBehavior)
	{
		AirsideMobElementBaseBehavior* spAirsideBehavior = getAirsideBehavior();
		pt = spAirsideBehavior->getDest();
		return true;
	}
	return false;
}

bool Person::getTerminalPureLocation(Point& pt)const
{
	PLACE_METHOD_TRACK_STRING();
	TerminalMobElementBehavior* spTerminalBehavior = getTerminalBehavior();
	if (spTerminalBehavior)
	{
		pt = spTerminalBehavior->getPureLocation();
		return true;
	}
	return false;
}

bool Person::getAirsidePureLocation(CPoint2008& pt)const
{
	PLACE_METHOD_TRACK_STRING();
	MobElementBehavior* spBehavior = getCurrentBehavior();
	if (spBehavior&&spBehavior->getBehaviorType() == MobElementBehavior::AirsideBehavior)
	{
		AirsideMobElementBaseBehavior* spAirsideBehavior = getAirsideBehavior();
		pt = spAirsideBehavior->getPureLocation();
		return true;
	}
	return false;
}

ElapsedTime Person::moveTime (void) const
{
	PLACE_METHOD_TRACK_STRING();
	MobElementBehavior* spBehavior = getCurrentBehavior();
	if (spBehavior&&spBehavior->getBehaviorType() == MobElementBehavior::TerminalBehavior)
	{
		TerminalMobElementBehavior* spTerminalBehavior = (TerminalMobElementBehavior*)spBehavior;
		return spTerminalBehavior->moveTime();
	}
	else if (spBehavior&&spBehavior->getBehaviorType() == MobElementBehavior::AirsideBehavior)
	{
		AirsideMobElementBaseBehavior* spAirsideBehavior = (AirsideMobElementBaseBehavior*)spBehavior;
		return spAirsideBehavior->moveTime();
	}	
	return ElapsedTime();
}

ElapsedTime Person::moveTime( DistanceUnit _dExtraSpeed, bool _bExtra ) const
{
	PLACE_METHOD_TRACK_STRING();
	MobElementBehavior* spBehavior = getCurrentBehavior();
	if (spBehavior&&spBehavior->getBehaviorType() == MobElementBehavior::TerminalBehavior)
	{
		TerminalMobElementBehavior* spTerminalBehavior = (TerminalMobElementBehavior*)spBehavior;
		return spTerminalBehavior->moveTime(_dExtraSpeed,_bExtra);
	}
	else if (spBehavior&&spBehavior->getBehaviorType() == MobElementBehavior::AirsideBehavior)
	{
		AirsideMobElementBaseBehavior* spAirsideBehavior = (AirsideMobElementBaseBehavior*)spBehavior;
		return spAirsideBehavior->moveTime(_dExtraSpeed,_bExtra);
	}
	return ElapsedTime();
}

void Person::setAirsideDestination(const CPoint2008& p)
{
	PLACE_METHOD_TRACK_STRING();
	ASSERT(0);
}

void Person::setTerminalDestination(const Point& p)
{
	PLACE_METHOD_TRACK_STRING();
	TerminalMobElementBehavior* spTerminalBehavior = getTerminalBehavior();
	if (spTerminalBehavior)
	{
		spTerminalBehavior->setDestination(p);
	}
}

void Person::setAirsideLocation( const CPoint2008& _ptLocation )
{
	PLACE_METHOD_TRACK_STRING();
	MobElementBehavior* spBehavior = getCurrentBehavior();
	if (spBehavior&&spBehavior->getBehaviorType() == MobElementBehavior::AirsideBehavior)
	{
		AirsideMobElementBaseBehavior* spAirsideBehavior = getAirsideBehavior();
		spAirsideBehavior->setLocation(_ptLocation);
	}
}

void Person::setTerminalLocation( const Point& _ptLocation )
{
	PLACE_METHOD_TRACK_STRING();
	TerminalMobElementBehavior* spTerminalBehavior = getTerminalBehavior();
	if (spTerminalBehavior)
	{
		spTerminalBehavior->setLocation(_ptLocation);
	}
}

bool Person::getOnboardPoint( ARCVector3& pt ) const
{
	MobElementBehavior* spBehavior = getCurrentBehavior();
	if (spBehavior&& spBehavior->getBehaviorType() == MobElementBehavior::OnboardBehavior )
	{
		PaxOnboardBaseBehavior* pOnboardBhv = (PaxOnboardBaseBehavior*)spBehavior;
		pt = pOnboardBhv->getPoint();
		return true;
	}
	return false;
}

bool Person::getOnboardDest( ARCVector3& pt ) const
{
	MobElementBehavior* spBehavior = getCurrentBehavior();
	if (spBehavior && spBehavior->getBehaviorType() == MobElementBehavior::OnboardBehavior)
	{
		PaxOnboardBaseBehavior* pOnboardBhv = (PaxOnboardBaseBehavior*)spBehavior;
		pt = pOnboardBhv->getDest();
		return true;
	}
	return false;
}

DistanceUnit Person::getInStep( void ) const
{
	  return (m_in*SCALE_FACTOR + ((GetActiveGroupSize() < 4)? 0: ((GetActiveGroupSize() < 6)? 20: 40))); 
}

void Person::setCurrentPositionRealZ( ElapsedTime moveTime )
{
	PLACE_METHOD_TRACK_STRING();
	MobileElementMovementEvent dummy;
	ElapsedTime destTime = dummy.removeMoveEvent (this);
	TerminalMobElementBehavior* spTerminalBehavior = getTerminalBehavior();

	if (destTime >= ElapsedTime(0l))
	{
		ARCPoint3 dest(spTerminalBehavior->m_ptDestination);
		ARCPoint3 location(spTerminalBehavior->location);
		ARCVector3 vector = dest- location;
		DistanceUnit dist= (destTime - moveTime).asSeconds()*speed;
		vector.SetLength(dist);
		ARCPoint3 midPt  = dest - vector;

		spTerminalBehavior->setDestination ( Point(midPt.x,midPt.y,midPt.z) );
	}
	writeLogEntry (moveTime, false);
}

void Person::SetBridgeState( ArrDepBridgeState state )
{
	TerminalMobElementBehavior* spTerminalBehavior = getTerminalBehavior();
	if(spTerminalBehavior)
	{
		spTerminalBehavior->setBridgeState(state);	
	}

	if(!m_pGroupInfo)
		return;

	if(!m_pGroupInfo->IsFollower())
	{
		CGroupLeaderInfo* leadinfo =  (CGroupLeaderInfo*)m_pGroupInfo;
		ASSERT(leadinfo->GetGroupLeader()==this);
		MobileElementList& followers = leadinfo->GetFollowerList();
		for(int i=0;i<followers.getCount();i++)
		{
			MobileElement* elm = followers.getItem(i);
			TerminalMobElementBehavior* spTerminalBehavior = elm->getTerminalBehavior();
			if(spTerminalBehavior)
			{
				spTerminalBehavior->setBridgeState(state);				
			}
		}
	}
}

void Person::SetWalkOnBridge( bool b )
{
	TerminalMobElementBehavior* spTerminalBehavior = getTerminalBehavior();
	if(spTerminalBehavior)
	{
		spTerminalBehavior->SetWalkOnBridge(b);	
	}

	if(!m_pGroupInfo)
		return;

	if(!m_pGroupInfo->IsFollower())
	{
		CGroupLeaderInfo* leadinfo =  (CGroupLeaderInfo*)m_pGroupInfo;
		ASSERT(leadinfo->GetGroupLeader()==this);
		MobileElementList& followers = leadinfo->GetFollowerList();
		for(int i=0;i<followers.getCount();i++)
		{
			MobileElement* elm = followers.getItem(i);
			TerminalMobElementBehavior* spTerminalBehavior = elm->getTerminalBehavior();
			if(spTerminalBehavior)
			{
				spTerminalBehavior->SetWalkOnBridge(b);				
			}
		}
	}
}
