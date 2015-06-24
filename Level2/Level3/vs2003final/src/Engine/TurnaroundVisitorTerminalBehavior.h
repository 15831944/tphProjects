#pragma once

#include "VisitorTerminalBehavior.h"

class TurnaroundVisitor;

class TurnaroundVisitorTerminalBehavior :public VisitorTerminalBehavior
{
public:
	TurnaroundVisitorTerminalBehavior(TurnaroundVisitor* pTurnVisitor);
	~TurnaroundVisitorTerminalBehavior();
};