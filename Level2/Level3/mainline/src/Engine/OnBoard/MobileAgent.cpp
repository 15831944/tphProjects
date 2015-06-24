#include "stdafx.h"
#include "MobileAgent.h"
#include "../terminal.h"
#include "OnBoardSimEngineConfig.h"
#include "../OnBoardPaxInformation.h"
#include "InputOnBoard/KineticsAndSpace.h"
#include "carrierspace/carrierDefs.h"
#include "LocatorSite.h"
#include "PathFinding/Pathfinder.h"
#include "Engine/ARCportEngine.h"
MobileAgent::MobileAgent(MobLogEntry& p_entry, CARCportEngine* pEngine)
:Person(p_entry, pEngine)
,m_pCarrierSpace(NULL)
,m_pConfig(NULL)
,m_pSeat(NULL)
,m_pLocatorSite(NULL)
,m_pFinalLocatorSite(NULL)
,m_pMovePath(new MobAgentMovementPath)
,mMobAgentType(pEngine->getTerminal()->inStrDict)
{
	memset(&mKineticsAndSpace,0,sizeof(KineticsAndSpace));
	memset(&mBehaviorData,0,sizeof(BehaviorData));
	memset(&mHuman,0,sizeof(AI));


	// Mobile Agent Type.
	CString strMobElement;
	Person::m_type.screenMobPrint( strMobElement );
	mMobAgentType.FormatLevelPaxTypeFromString( strMobElement );


	// common config.
	assignCommonConfig();

}

MobileAgent::~MobileAgent()
{
	delete m_pMovePath;

}

void MobileAgent::kill(ElapsedTime killTime)
{
	flushLog( killTime, false);
}

void MobileAgent::assignCommonConfig()
{
	OnBoardSimEngineConfig* pConfig = 
		(OnBoardSimEngineConfig*)(GetTerminal()->m_pVoidOnBoardConfig);
	ASSERT(pConfig != NULL);
	m_pConfig = pConfig;

	// avoid ambiguous error, disabled data load in current version.
	return;

	ASSERT(m_pConfig->m_pAgentInfo != NULL);
	//Kinetics And Space
	mKineticsAndSpace.mVarAgentSpeed
		=  m_pConfig->m_pAgentInfo->GetPaxKinetics( mMobAgentType, Speed);
	mKineticsAndSpace.mVarFreeInStep
		=  m_pConfig->m_pAgentInfo->GetPaxKinetics( mMobAgentType, FreeInStep);
	mKineticsAndSpace.mVarCongesitionInStep
		=  m_pConfig->m_pAgentInfo->GetPaxKinetics( mMobAgentType, CongestionInStep);
	mKineticsAndSpace.mVarQueueInStep
		=  m_pConfig->m_pAgentInfo->GetPaxKinetics( mMobAgentType, QueueInStep);
	mKineticsAndSpace.mVarFreeSideStep
		=  m_pConfig->m_pAgentInfo->GetPaxKinetics( mMobAgentType, FreeSideStep);
	mKineticsAndSpace.mVarCongesitionSideStep
		=  m_pConfig->m_pAgentInfo->GetPaxKinetics( mMobAgentType, CongestionSideStep);
	mKineticsAndSpace.mVarQueueSideStep
		=  m_pConfig->m_pAgentInfo->GetPaxKinetics( mMobAgentType, QueueSideStep);
	mKineticsAndSpace.mVarInteractionDist
		=  m_pConfig->m_pAgentInfo->GetPaxKinetics( mMobAgentType, InteractionDistance);
	mKineticsAndSpace.mVarAffinityFreeDist
		=  m_pConfig->m_pAgentInfo->GetPaxKinetics( mMobAgentType, AffinityFreeDistance);
	mKineticsAndSpace.mVarAffinityCongestionDist
		=  m_pConfig->m_pAgentInfo->GetPaxKinetics( mMobAgentType, AffinityCongestionDistance);
	mKineticsAndSpace.mVarSqeezability
		=  m_pConfig->m_pAgentInfo->GetPaxKinetics( mMobAgentType, Squeezability);


	//Behavior Data
	mBehaviorData.mVardDocDispTime
		= m_pConfig->m_pAgentInfo->GetDocumentDisplayTime( mMobAgentType );
	mBehaviorData.mVarWayFindingIntervention
		= m_pConfig->m_pAgentInfo->GetWayFindingIntervention( mMobAgentType );
	//mBehaviorData.mVarUnimpededStateTransTime
	//	= m_pConfig->m_pAgentInfo->GetUnimpededStateTransTime( mMobAgentType );
	mBehaviorData.mVarTransitionTimeModifier_Inbibitor
		= m_pConfig->m_pAgentInfo->GetTransitionTimeModifier_Inbibitor( mMobAgentType );
	mBehaviorData.mVarTransitionTimeModifier_MTrigger
		= m_pConfig->m_pAgentInfo->GetTransitionTimeModifier_MTrigger( mMobAgentType );
	mBehaviorData.mVarTransitionTimeModifier_Factor
		= m_pConfig->m_pAgentInfo->GetTransitionTimeModifier_Factor( mMobAgentType );
	mBehaviorData.mVarTargetLocation
		= m_pConfig->m_pAgentInfo->GetTargetLocation( mMobAgentType );
	mBehaviorData.mVarUnexpectedBehavior
		= m_pConfig->m_pAgentInfo->GetUnexpectedBehaviorVariable( mMobAgentType );
	mBehaviorData.mVarSeatingPerformance
		= m_pConfig->m_pAgentInfo->GetSeatingPerformance( mMobAgentType );

}

CString MobileAgent::screenMobAgent()
{
	return mMobAgentType.PrintStringForShow();
}

bool MobileAgent::isDeplanement()
{
	return true;
	//return (NULL != getBehavior( MobElementBehavior::DeplaneBehavior ) );
}

bool MobileAgent::isEnplanement()
{
	return true;
	/*return (NULL != getBehavior( MobElementBehavior::EnplaneBehavior ) );*/
}

OnBoardSimEngineConfig* MobileAgent::getConfig()
{
	return m_pConfig;
}

void MobileAgent::setCarrierSpace(carrierSpace* pCarrier)
{
	ASSERT( pCarrier != NULL );
    m_pCarrierSpace = pCarrier;
}

carrierSpace* MobileAgent::getCarrierSpace()
{
	return m_pCarrierSpace;
}

void MobileAgent::assignSeat( carrierSeat* pSeat )
{
	ASSERT( pSeat != NULL );
	m_pSeat = pSeat;
}

carrierSeat* MobileAgent::getAssignedSeat()
{
	ASSERT( m_pSeat != NULL );
	return m_pSeat;
}

LocatorSite* MobileAgent::getLocatorSite()
{
	if(NULL == m_pLocatorSite)
	{
		CString strErrorMsg;
		strErrorMsg.Format("%s. Lost location on deck", screenMobAgent());
		//throw new OnBoardSimEngineException( new OnBoardDiagnose( strErrorMsg));
	}

	return m_pLocatorSite;
}

void MobileAgent::setLocatorSite(LocatorSite* pMobAgentSite)
{
	ASSERT( pMobAgentSite != NULL );
	m_pLocatorSite = pMobAgentSite;
	CPoint2008 ptLocation = m_pLocatorSite->getLocation();
	DistanceUnit _X, _Y, _Z;
	ptLocation.getPoint( _X, _Y, _Z);
//	MobileElement::setDestination(Point(_X, _Y,_Z));
}

// when Agent birth, init birth location.
void MobileAgent::initBirthLocation()
{
//	MobileElement::location = MobileElement::m_ptDestination;
}

LocatorSite* MobileAgent::getFinalLocatorSite()
{
	if(NULL == m_pFinalLocatorSite)
	{
		CString strErrorMsg;
		strErrorMsg.Format("%s. miss destination on deck", screenMobAgent());
		//throw new OnBoardSimEngineException( new OnBoardDiagnose( strErrorMsg));
	}

	return m_pFinalLocatorSite;
}

void  MobileAgent::setFinalLocatorSite(LocatorSite* pFinalSite)
{
	ASSERT( pFinalSite != NULL );
	m_pFinalLocatorSite = pFinalSite;
}


void MobileAgent::assignIntelligence(PathFinder* pDetector)
{
	mHuman.mDetactor = pDetector;
}

MobAgentMovementPath* MobileAgent::getMovementPath()
{
	return m_pMovePath;
}

//<TODO::>
MobAgentMovementPath* MobileAgent::getMovementPathWithInteraction()
{
	return m_pMovePath;
}


ElapsedTime MobileAgent::moveTime (void) const
{
 	ElapsedTime t;
// 	if (!location)
// 		t = 0l;
// 	else
// 	{
// 
// 		double dLxy = m_ptDestination.distance(location);
// 		double dLz = 0.0;
// 		double dL = dLxy;
// 		double time = dL;
// 		t = (float) (time / (double)speed );//<TODO::>actual speed based on agent's state.
// 	}
 	return t;
}


void MobileAgent::writeLogEntry (ElapsedTime p_time, bool _bBackup, bool bOffset /*= true*/ )
{

// 	MobEventStruct track;
// 	track.time = (long)p_time;
// 
// 	// debug purpose.
// 	if(state == WaitingForCarrierTakeOff || state == WaitingForLeaveCarrier)
// 		track.state = LOBYTE( LOWORD(2) );//SEAT
// 
// 	else
// 		track.state = LOBYTE( LOWORD(0) );//WALK
// 
// 	track.procNumber = -1;
// 	track.bDynamicCreatedProc =false;
// 	track.followowner	= false;
// 
// 	TerminalMobElementBehavior* spTerminalBehavior = (TerminalMobElementBehavior *)getBehavior(MobElementBehavior::TerminalBehavior);
// 	if (spTerminalBehavior)
// 	{
// 		track.reason = spTerminalBehavior->GetStopReason();
// 	}
// 	
// 	track.backup = _bBackup;
// 
// 	pure_location	= m_ptDestination;
// 	Point pre_location = location;
// 	if (location != m_ptDestination)
// 	{
// 		Point vector (location, m_ptDestination);
// 		Point latPoint (vector.perpendicular());
// 		latPoint.length (getSideStep());
// 
// 		location = m_ptDestination;
// 		MobileElement::setDestination( location );
// 	}
// 
// 	location.getPoint (track.x, track.y, track.z);
// 	m_logEntry.addEvent (track);

}


void MobileAgent::flushLog (ElapsedTime p_time, bool bmissflight)
{

	m_iPreState = state;

	state = Death;
	writeLogEntry (p_time, false);
	m_logEntry.setExitTime (p_time);
	long trackCount = m_logEntry.getCurrentCount();

	MobEventStruct *log = NULL;
	m_logEntry.getLog (log);

	// write first member of group
	m_logEntry.setEventList (log, trackCount);	
	delete [] log;
	log = NULL;

	assert( m_pTerm );
	m_pTerm->paxLog->updateItem (m_logEntry, m_logEntry.getIndex());

	m_pGroupInfo->FlushLogforFollower( p_time );

	m_logEntry.clearLog();	

}


