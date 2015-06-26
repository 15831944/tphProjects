#pragma once
#include "BaseLog.h"
#include "MobAgentLogEntry.h"

class MobileAgentLog : public BaseLog < MobAgentDescStruct, 
	MobAgentEventStruct, MobAgentLogEntry>
{
public:
	MobileAgentLog(void){};
	virtual ~MobileAgentLog(void){};

};