#include "StdAfx.h"
#include "BoardingCallPaxTypeDatabase.h"

BoardingCallPaxTypeEntry::BoardingCallPaxTypeEntry(void)
{
	m_vTriggers.clear();
}


BoardingCallPaxTypeEntry::~BoardingCallPaxTypeEntry(void)
{
}

// initialize triggers
void BoardingCallPaxTypeEntry::InitTriggersDatabase()
{
	BoardingCallTrigger trigger(10000L, 20.0);
	m_vTriggers.push_back(trigger);
	trigger.SetTrigger(20000L, 20.0);
	m_vTriggers.push_back(trigger);
	trigger.SetTrigger(30000L, 20.0);
	m_vTriggers.push_back(trigger);
	trigger.SetTrigger(40000L, 20.0);
	m_vTriggers.push_back(trigger);
}

BoardingCallPaxTypeDatabase::BoardingCallPaxTypeDatabase(void)
{
}

BoardingCallTrigger::BoardingCallTrigger( long _time, double _prop )
{
	m_time = _time;
	m_proportion = _prop;
}


BoardingCallPaxTypeDatabase::~BoardingCallPaxTypeDatabase(void)
{
}
