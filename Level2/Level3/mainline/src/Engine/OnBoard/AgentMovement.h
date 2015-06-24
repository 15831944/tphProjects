#pragma once
#include "../MobElementMovementEvent.h"

class InputTerminal;

class MobileAgent;
class AgentMovement
{
public:
	AgentMovement(MobileAgent* _agent, ElapsedTime& _time);
	//int process (InputTerminal* _pInTerm ){} ; 
	//const char *getTypeName (void) const ;
	//int getEventType (void) const  ;
};

