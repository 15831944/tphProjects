// TurnaroundVisitor.cpp: implementation of the TurnaroundVisitor class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "TurnaroundVisitor.h"
#include "inputs\schedule.h"
#include "engine\terminal.h"
#include "Engine\ARCportEngine.h"
#include "TurnaroundVisitorTerminalBehavior.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

TurnaroundVisitor::TurnaroundVisitor(MobLogEntry& _entry, CARCportEngine *_pEngine ):
	PaxVisitor(_entry,_pEngine)
{
	MobileElement::setBehavior( new TurnaroundVisitorTerminalBehavior( this ));

	arrivingType = initType (_entry.getArrFlight(), 'A');
	arrivingType.SetInputTerminal( m_pTerm );
    departingType = initType (_entry.getDepFlight(), 'D');
	departingType.SetInputTerminal( m_pTerm );
	
    Flight *aFlight = getEngine()->m_simFlightSchedule.getItem (_entry.getArrFlight());
	arrivingGate = aFlight->getArrGateIndex();
	m_nGate = arrivingGate;
	
    aFlight = getEngine()->m_simFlightSchedule.getItem (_entry.getDepFlight());
	departingGate = aFlight->getDepGateIndex();
    departureTime = aFlight->getDepTime();
	m_type.setOtherFlightConstraint(arrivingType);
}

TurnaroundVisitor::~TurnaroundVisitor()
{

}
