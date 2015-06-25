#include "StdAfx.h"
#include "BoardingCallPaxTypeDatabase.h"
#include "PROBAB.H"

BoardingCallPaxTypeEntry::BoardingCallPaxTypeEntry()
{
	m_vTriggers.clear();
}


BoardingCallPaxTypeEntry::~BoardingCallPaxTypeEntry(void)
{
}

// initialize triggers
void BoardingCallPaxTypeEntry::InitTriggerDatabase()
{
	BoardingCallTrigger trigger;
	m_vTriggers.push_back(trigger);
	trigger.SetTriggerTime(-1200L);
	trigger.SetTriggerProp(20);
	m_vTriggers.push_back(trigger);
	trigger.SetTriggerTime(-1000L);
	trigger.SetTriggerProp(70);
	m_vTriggers.push_back(trigger);
	trigger.SetTriggerTime(-600L);
	trigger.SetTriggerProp(10);
	m_vTriggers.push_back(trigger);

	AddResidualTrigger();
}

void BoardingCallPaxTypeEntry::InitTriggerDBFromOld(ConstraintWithProcIDEntry* pConstEntry)
{
	
	if(pConstEntry->getValue()->getProbabilityType() == HISTOGRAM)
	{
		const HistogramDistribution* pDistribution = (HistogramDistribution*)pConstEntry->getValue();
		int count = pDistribution->getCount();
		BoardingCallTrigger trigger;
		for(int i=0; i<count; i++)
		{
			trigger.SetTriggerTime((long)pDistribution->getValue(i));
			double prob;
			if(i == 0)
			{
				prob = pDistribution->getProb(i);
			}
			else
			{
				prob = pDistribution->getProb(i) - pDistribution->getProb(i-1);
			}
			trigger.SetTriggerProp(prob);
			m_vTriggers.push_back(trigger);
		}
	}
	AddResidualTrigger();
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
	int seconds;
	double prop;
	for(int i=0; i<triggerCount; i++)
	{
		seconds = 0;
		prop = 0.0;
		p_file.getLine();
		p_file.skipField(1);/* 'TriggerN' */
		trigger.readTrigger(p_file);
		m_vTriggers.push_back(trigger);
	}
}

void BoardingCallPaxTypeEntry::writeTriggerDatabase( ArctermFile& p_file)
{
	int count = GetTriggerCount();
	CString strTrigger;
	for(int i=0; i<count; i++)
	{
		strTrigger.Format("Trigger%d", i);
		p_file.writeField(strTrigger.GetBuffer());
		m_vTriggers[i].writeTrigger(p_file);
		p_file.writeLine();
	}
}

void BoardingCallPaxTypeEntry::AddResidualTrigger()
{
	BoardingCallTrigger trigger;
	trigger.SetTriggerTime(-500L);
	trigger.SetTriggerProp(-1);
	m_vTriggers.push_back(trigger);/* The 'RESIDUAL' trigger. */
}

BoardingCallPaxTypeDatabase::BoardingCallPaxTypeDatabase()
{
}

BoardingCallTrigger::BoardingCallTrigger(ProbabilityDistribution* _time, ProbabilityDistribution* _prop)
{
	m_time = _time;
	m_prop = _prop;
}

void BoardingCallTrigger::InitTrigger()
{
	if(m_time)
	{
		delete m_time;
	}
	m_time = new ConstantDistribution;
	((ConstantDistribution*)m_time)->init(0);

	if(m_prop)
	{
		delete m_prop;
	}
	m_prop = new ConstantDistribution;
	((ConstantDistribution*)m_time)->init(0);
}

BoardingCallTrigger::~BoardingCallTrigger()
{
	if(m_time)
	{
		delete m_time;
		m_time = NULL;
	}
	if(m_prop)
	{
		delete m_prop;
		m_prop = NULL;
	}
}

void BoardingCallTrigger::SetTriggerTime(ProbabilityDistribution* _time)
{
	if(m_time)
		delete m_time;
	m_time = ProbabilityDistribution::CopyProbDistribution(_time);
}

void BoardingCallTrigger::SetTriggerProp( ProbabilityDistribution* _prop )
{
	if(m_prop)
		delete m_prop;
	m_prop = ProbabilityDistribution::CopyProbDistribution(_prop);
}

void BoardingCallTrigger::writeTrigger(ArctermFile& p_file)
{
	m_time->writeDistribution(p_file);
	m_prop->writeDistribution(p_file);
}

void BoardingCallTrigger::readTrigger(ArctermFile& p_file)
{
	m_time = GetProbDistribution(p_file);
	m_prop = GetProbDistribution(p_file);
}


BoardingCallPaxTypeDatabase::~BoardingCallPaxTypeDatabase()
{
}

void BoardingCallPaxTypeDatabase::deleteItem( ConstraintEntry* pConst )
{
	removeItem(pConst);
	delete pConst;
}

void BoardingCallPaxTypeDatabase::AddPaxType(CMobileElemConstraint* pInputConst, InputTerminal* _pInTerm)
{
	CMobileElemConstraint* pMBConst = new CMobileElemConstraint();
	if(pInputConst)
	{
		*pMBConst = *pInputConst;
	}
	else
	{
		ASSERT(_pInTerm);
		pMBConst->SetInputTerminal(_pInTerm);
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
		CMobileElemConstraint paxConst;
		paxConst.SetInputTerminal(_pInTerm);
		paxConst.readConstraint(p_file);
		AddPaxType(&paxConst, _pInTerm);

		int triggerCount;
		p_file.getInteger(triggerCount);
		((BoardingCallPaxTypeEntry*)getItem(i))->readTriggerDatabase(p_file, triggerCount);
	}
}

void BoardingCallPaxTypeDatabase::writeDatabase(ArctermFile& p_file)
{
	int paxCount = getCount();
	for(int i=0; i<paxCount; i++)
	{
		CString strPax;
		BoardingCallPaxTypeEntry* pPaxEntry = (BoardingCallPaxTypeEntry*)getItem(i);
		CMobileElemConstraint* pPaxConst = (CMobileElemConstraint*)pPaxEntry->getConstraint();
		pPaxConst->writeConstraint(p_file);
		int triggerCount = pPaxEntry->GetTriggerCount();
		p_file.writeInt(triggerCount);
		p_file.writeLine();

		pPaxEntry->writeTriggerDatabase(p_file);
	}
}

// For version 2.6 or older.
void BoardingCallPaxTypeDatabase::AddPaxTypeFor260AndOlder( ConstraintWithProcIDEntry* pConstEntry, InputTerminal* _pInTerm)
{
	CMobileElemConstraint* pMBConst = new CMobileElemConstraint();
	pMBConst->SetInputTerminal(_pInTerm);
	pMBConst->SetMobileElementType(enum_MobileElementType_ALL); /* Set 'Passenger Type': DEFAULT */
	BoardingCallPaxTypeEntry* pPaxEntry = new BoardingCallPaxTypeEntry();
	pPaxEntry->initialize(pMBConst, NULL);
	pPaxEntry->InitTriggerDBFromOld(pConstEntry);
	addEntry(pPaxEntry, true);
}

int BoardingCallPaxTypeDatabase::FindItemByConstraint( CMobileElemConstraint* pInputConst )
{
	int nEntryCount = getCount();
	for( int i = 0; i < nEntryCount; i++ )
	{
		const CMobileElemConstraint* pConstraint = (CMobileElemConstraint*)getItem(i)->getConstraint();
		if( pConstraint->isEqual(pInputConst) )
			return i;
	}
	return NULL;
}
