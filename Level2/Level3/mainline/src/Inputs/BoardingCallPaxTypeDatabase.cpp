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
	trigger.SetTrigger(40000L, -1);
	m_vTriggers.push_back(trigger);
}

void BoardingCallPaxTypeEntry::AddNewTrigger()
{
	BoardingCallTrigger trigger(0, 0);
	m_vTriggers.insert(m_vTriggers.end()-1, trigger);
}

void BoardingCallPaxTypeEntry::SetTriggerCount( int count )
{
	if(m_vTriggers.size() == count) 
		return;
	while((int)m_vTriggers.size() > count)
	{
		m_vTriggers.erase(m_vTriggers.end()-2);
	}
	while(count > (int)m_vTriggers.size())
	{
		AddNewTrigger();
	}
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

void BoardingCallPaxTypeDatabase::deleteItem( ConstraintEntry* pConst )
{
	removeItem(pConst);
	delete pConst;
}
