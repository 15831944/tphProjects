#include "StdAfx.h"
#include "TurnaroundPaxTerminalBehavior.h"
#include "turnarnd.h"

#define DEPARTING   1

TurnaroundPaxTerminalBehavior::TurnaroundPaxTerminalBehavior(TurnaroundPassenger* pTurnPax)
:PaxTerminalBehavior(pTurnPax)
{

}

TurnaroundPaxTerminalBehavior::~TurnaroundPaxTerminalBehavior()
{

}

void TurnaroundPaxTerminalBehavior::processServerArrival (ElapsedTime p_time)
{
	if( m_pProcessor->getProcessorType() == GateProc )
	{
		m_pPerson->m_nGate = ((TurnaroundPassenger*)m_pPerson)->departingGate;
		((TurnaroundPassenger*)m_pPerson)->direction = 1;
	}
	PaxTerminalBehavior::processServerArrival( p_time );
}

// yes after arr gate.
bool TurnaroundPaxTerminalBehavior::NeedCheckLeadToGate() const
{
	if( m_pPerson->getGateIndex() < 0 )
		return false;

	if( m_pPerson->m_type.GetTypeIndex() == 0 && m_iNumberOfPassedGateProc < 2 )
		return true;
	return false;
}