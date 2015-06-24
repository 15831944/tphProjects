#pragma once
#include "PaxTerminalBehavior.h"

class TurnaroundPassenger;
class TurnaroundPaxTerminalBehavior : public PaxTerminalBehavior
{
public:
	TurnaroundPaxTerminalBehavior(TurnaroundPassenger* pTurnPax);
	~TurnaroundPaxTerminalBehavior();

public:
	virtual void processServerArrival (ElapsedTime p_time);
	virtual bool NeedCheckLeadToGate() const;
};