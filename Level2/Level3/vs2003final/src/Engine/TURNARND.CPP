#include "stdafx.h"
#include "engine\turnarnd.h"
#include "results\paxentry.h"
#include "results\out_term.h"
#include "inputs\flight.h"
#include "inputs\in_term.h"
#include "inputs\schedule.h"
#include "engine\process.h"
#include "engine\baggage.h"
#include "engine\proclist.h"
#include "engine\hold.h"
#include "term_err.h"
#include "inputs\bagdata.h"
#include "inputs\SubFlow.h"
#include "inputs\SubFlowList.h"
#include "engine\movevent.h"
#include "common\CodeTimeTest.h"
#include "Engine\GroupLeaderInfo.h"
#include "engine\terminal.h"
#include "Engine\ARCportEngine.h"
#include "TurnaroundPaxTerminalBehavior.h"


#define ARRIVING    0
#define DEPARTING   1
#define TURNAROUND  2


TurnaroundPassenger::TurnaroundPassenger (MobLogEntry& p_entry, CARCportEngine* _pEngine ) :
    Passenger (p_entry, _pEngine )
{
	MobileElement::setBehavior( new TurnaroundPaxTerminalBehavior( this ));
	direction = ARRIVING;

	m_logMobEntry = p_entry;
	assert( m_pTerm );
    arrivingType = initType (p_entry.getArrFlight(), 'A');
	arrivingType.SetInputTerminal( m_pTerm );
    departingType = initType (p_entry.getDepFlight(), 'D');
	departingType.SetInputTerminal( m_pTerm );

    Flight *aFlight = getEngine()->m_simFlightSchedule.getItem (p_entry.getArrFlight());
	arrivingGate = aFlight->getArrGateIndex();
	m_nGate = arrivingGate;

    aFlight = getEngine()->m_simFlightSchedule.getItem (p_entry.getDepFlight());
	departingGate = aFlight->getDepGateIndex();
    departureTime = aFlight->getDepTime();
    m_type.setOtherFlightConstraint(arrivingType);

}

//CSinglePaxTypeFlow* TurnaroundPassenger::TryToGetPathInProcess( CSubFlow* _pProcess, const ElapsedTime& _curTime)
//{
//	bool bHaveGate				= true;
//	const ProcessorID* pGate	= NULL;
//	bool bHaveBag				= false;
//	const ProcessorID* pDestID = NULL;
//	// gate
//	if( m_nGate < 0)
//	{
//		bHaveGate = false;
//		pGate = NULL;
//	}	
//	else
//	{
//		pGate = GetTerminal()->procList->getProcessor( m_nGate )->getID();
//		bHaveGate = pGate ? true : false;
//	}
//
//	// bag device
//	bHaveBag = false;
//	if( m_type.GetTypeIndex() == 0	)	// pax
//	{
//		if( this->GetBagsCount() >0 && m_type.isArrival())
//		 bHaveBag = true;
//	}
//	else if( m_type.GetTypeIndex() == 2)	// check bag
//	{
//			bHaveBag = true;
//	}
//	
//	if( bHaveBag )
//	{
//		int nIndex = m_logMobEntry.getArrFlight();
//		Flight* pFlight = getEngine()->m_simFlightSchedule.getItem(nIndex);
//		const ProcessorID* pDestID = &(pFlight->getBaggageDevice());
//		if( pDestID == NULL )
//		{
//			CString szMobType = getPersonErrorMsg();
//			kill(_curTime);
//			throw new ARCNoBaggageDeviceAssignError(szMobType, "", _curTime.printTime());
//		}
//	}
//
//	CSinglePaxTypeFlow* pFlow = NULL;
//	if( m_nGate == arrivingGate )
//	{
//		// get single flow from PROCESS 
//		try
//		{
//			pFlow = _pProcess->GetPathInProcess(m_type,bHaveGate,*pGate,bHaveBag,*pDestID, arrivingType );
//		}
//		catch(ARCError* e)
//		{
//			delete e;	
//		}
//		catch( ARCSimEngineException* sim_exception )
//		{
//			getEngine()->SendSimFormatMessage( sim_exception->getFormatErrorMsg() );
//			delete sim_exception;
//		}
//	}	
//	else
//	{
//		try
//		{
//			pFlow = _pProcess->GetPathInProcess(m_type,bHaveGate,*pGate,bHaveBag,*pDestID, departingType );
//		}
//		catch(ARCError* e)
//		{
//			delete e;	
//		}
//		catch( ARCSimEngineException* sim_exception )
//		{
//			getEngine()->SendSimFormatMessage( sim_exception->getFormatErrorMsg() );
//			delete sim_exception;
//		}
//	}
//	return pFlow;
//}
//
//
// Used to determine if the gate is used as arrival purpose.
bool TurnaroundPassenger::CheckAndTurnArrState()
{
	if( direction == ARRIVING )
	{
		m_nGate = departingGate;
		direction = DEPARTING;
		TerminalMobElementBehavior* spTerminalBehavior = (TerminalMobElementBehavior *)getBehavior(MobElementBehavior::TerminalBehavior);
		++spTerminalBehavior->m_iNumberOfPassedGateProc;
		return true;
	}
	else
		return false;
}

// new person for group follower
void TurnaroundPassenger::newGroupFollower( void )
{
	MobLogEntry _followerEntry = m_logEntry;
	TerminalMobElementBehavior* spTerminalBehavior = (TerminalMobElementBehavior *)getBehavior(MobElementBehavior::TerminalBehavior);
	for( int i=1; i< GetAdminGroupSize(); i++ )
	{
		_followerEntry.incPax();
		Person* _pFollower = new TurnaroundPassenger( _followerEntry, getEngine()  );
		
		getEngine()->m_simBobileelemList.Register( _pFollower );
		((CGroupLeaderInfo*)m_pGroupInfo)->AddFollower( _pFollower, this );		
	}
}