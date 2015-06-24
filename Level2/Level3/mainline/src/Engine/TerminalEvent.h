#pragma once
#include "common\containr.h"
#include "common\elaptime.h"
#include "common\util.h"
#include "enginedll.h"
#include "inputs\in_term.h"
#include "event.h"


// Subclass types


class TerminalEvent : public Event
{
public:
	TerminalEvent(void);
	~TerminalEvent(void);


};
