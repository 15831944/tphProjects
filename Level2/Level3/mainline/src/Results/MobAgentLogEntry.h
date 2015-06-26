#pragma once

#include "logitem.h"
#include "Common\ONBOARD_BIN.h"
#include "Common\fsstream.h"

class MobAgentLogEntry : public LogItem < MobAgentDescStruct, MobAgentEventStruct >
{
public:
	MobAgentLogEntry(void){};
	virtual ~MobAgentLogEntry(void){};

};