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
void BoardingCallPaxTypeEntry::InitTriggerDatabase()
{
	BoardingCallTrigger trigger(10000L, 20.0);
	m_vTriggers.push_back(trigger);
	trigger.SetTrigger(20000L, 20.0);
	m_vTriggers.push_back(trigger);
	trigger.SetTrigger(30000L, 20.0);
	m_vTriggers.push_back(trigger);
	trigger.SetTrigger(40000L, -1);/* The 'RESIDUAL' trigger. */
	m_vTriggers.push_back(trigger);
}

void BoardingCallPaxTypeEntry::InitTriggerDBFromOld(ConstraintWithProcIDEntry* pConstEntry)
{
	const HistogramDistribution* pDistribution = (HistogramDistribution*)pConstEntry->getValue();
	ASSERT(pDistribution->getProbabilityType() == HISTOGRAM);
	BoardingCallTrigger trigger;
	int count = pDistribution->getCount();
	for(int i=0; i<count; i++)
	{
		long seconds = (long)pDistribution->getValue(i);
		double prob;
		if(i == 0)
		{
			prob = pDistribution->getProb(i);
		}
		else
		{
			prob = pDistribution->getProb(i) - pDistribution->getProb(i-1);
		}
		trigger.SetTrigger(seconds, prob);
		m_vTriggers.push_back(trigger);
	}
	trigger.SetTrigger(40000L, -1);/* The 'RESIDUAL' trigger. */
	m_vTriggers.push_back(trigger);
}

void BoardingCallPaxTypeEntry::AddNewTrigger()
{
	BoardingCallTrigger trigger(0, 0);
	m_vTriggers.insert(m_vTriggers.end()-1, trigger);
}

void BoardingCallPaxTypeEntry::SetTriggerCount(int count)
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

void BoardingCallPaxTypeEntry::readTriggerDatabase( ArctermFile& p_file, int triggerCount)
{
	DeleteAllTrigger();
	BoardingCallTrigger trigger;
	int time;
	double prop;
	for(int i=0; i<triggerCount; i++)
	{
		time = 0;
		prop = 0.0;
		p_file.getLine();
		p_file.skipField(1);
		p_file.getInteger(time);
		p_file.getFloat(prop);

		trigger.InitTrigger();
		trigger.SetTrigger(time, prop);
		m_vTriggers.push_back(trigger);
	}
}

void BoardingCallPaxTypeEntry::writeTriggerDatabase( ArctermFile& p_file, InputTerminal* _pInTerm )
{
	int count = GetTriggerCount();
	CString strTrigger;
	for(int i=0; i<count; i++)
	{
		strTrigger.Format("Trigger%d", i);
		p_file.writeField(strTrigger.GetBuffer());
		p_file.writeInt(m_vTriggers[i].GetTriggerTime().getPrecisely());
		p_file.writeDouble(m_vTriggers[i].GetTriggerProportion());
		p_file.writeLine();
	}
}

BoardingCallPaxTypeDatabase::BoardingCallPaxTypeDatabase()
{
}

BoardingCallTrigger::BoardingCallTrigger(long _time, double _prop)
{
	m_time = _time;
	m_proportion = _prop;
}


BoardingCallPaxTypeDatabase::~BoardingCallPaxTypeDatabase()
{
}

void BoardingCallPaxTypeDatabase::deleteItem( ConstraintEntry* pConst )
{
	removeItem(pConst);
	delete pConst;
}

void BoardingCallPaxTypeDatabase::AddPax( CMobileElemConstraint* pInputConst )
{
	CMobileElemConstraint* pMBConst = new CMobileElemConstraint();
	if(pInputConst)
	{
		*pMBConst = *pInputConst;
	}
	else
	{
		pMBConst->SetMobileElementType(enum_MobileElementType_ALL); /* Set 'Passenger Type': DEFAULT */
	}
	BoardingCallPaxTypeEntry* pPaxEntry = new BoardingCallPaxTypeEntry();
	pPaxEntry->initialize(pMBConst, NULL);
	pPaxEntry->InitTriggerDatabase();	/* InitTriggersDatabase(): The 4 initial triggers will be added here. */
	addEntry(pPaxEntry, true);/* Replace if exists. */
}

void BoardingCallPaxTypeDatabase::readDatabase( ArctermFile& p_file, InputTerminal* _pInTerm, int paxCount)
{
	clear();
	for(int i=0; i<paxCount; i++)
	{
		p_file.getLine();
		CMobileElemConstraint mobileConst;
		mobileConst.SetInputTerminal(_pInTerm);
		mobileConst.readConstraint(p_file);
		AddPax(&mobileConst);

		int triggerCount;
		p_file.getInteger(triggerCount);
		((BoardingCallPaxTypeEntry*)getItem(i))->readTriggerDatabase(p_file, triggerCount);
	}
}

void BoardingCallPaxTypeDatabase::writeDatabase( ArctermFile& p_file, InputTerminal* _pInTerm )
{
	int paxCount = getCount();
	for(int i=0; i<paxCount; i++)
	{
		CString strPax;
		BoardingCallPaxTypeEntry* pPaxEntry = (BoardingCallPaxTypeEntry*)getItem(i);
		CMobileElemConstraint* pPaxConst = (CMobileElemConstraint*)pPaxEntry->getConstraint();
		pPaxConst->screenPrint(strPax);
		p_file.writeField(strPax);
		int triggerCount = pPaxEntry->GetTriggerCount();
		p_file.writeInt(triggerCount);
		p_file.writeLine();

		pPaxEntry->writeTriggerDatabase(p_file, _pInTerm);
	}
}

void BoardingCallPaxTypeDatabase::AddPaxFromOld( ConstraintWithProcIDEntry* pConstEntry )
{
	CMobileElemConstraint* pMBConst = new CMobileElemConstraint();
	pMBConst->SetMobileElementType(enum_MobileElementType_ALL); /* Set 'Passenger Type': DEFAULT */
	BoardingCallPaxTypeEntry* pPaxEntry = new BoardingCallPaxTypeEntry();
	pPaxEntry->initialize(pMBConst, NULL);
	pPaxEntry->InitTriggerDBFromOld(pConstEntry);
	addEntry(pPaxEntry, true);
}
