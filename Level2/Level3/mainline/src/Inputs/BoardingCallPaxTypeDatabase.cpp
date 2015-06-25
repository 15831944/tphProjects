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
	AddRegularTrigger(-1200L, 20);
	AddRegularTrigger(-1000L, 70);
	AddRegularTrigger(-600L, 10);
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
	AddRegularTrigger(-500L, -1);/* The 'RESIDUAL' trigger. */
}

void BoardingCallPaxTypeEntry::AddRegularTrigger( long _seconds, double _prop )
{
	BoardingCallTrigger trigger;
	trigger.SetTriggerTime(_seconds);
	trigger.SetTriggerProp(_prop);
	m_vTriggers.push_back(trigger);
}

BoardingCallPaxTypeDatabase::BoardingCallPaxTypeDatabase()
{
}

BoardingCallTrigger::BoardingCallTrigger(long _seconds, double _prop)
{
	m_time.init(_seconds);
	m_prop.init(_prop);
}

void BoardingCallTrigger::InitTrigger()
{
	m_time.init(0);
	m_prop.init(0);
}

BoardingCallTrigger::~BoardingCallTrigger()
{
}

void BoardingCallTrigger::SetTriggerTime(long _seconds)
{
	m_time.init(_seconds);
}

void BoardingCallTrigger::SetTriggerProp(double _prop)
{
	m_prop.init(_prop);
}

void BoardingCallTrigger::writeTrigger(ArctermFile& p_file)
{
	m_time.writeDistribution(p_file);
	m_prop.writeDistribution(p_file);
}

void BoardingCallTrigger::readTrigger(ArctermFile& p_file)
{
 	ProbabilityDistribution* pPD = GetProbDistribution(p_file);
	if(pPD->getProbabilityType() == CONSTANT)
	{
		m_time.setConstant(((ConstantDistribution*)pPD)->getConstant());
		delete pPD;
	}
	pPD = GetProbDistribution(p_file);
	if(pPD->getProbabilityType() == CONSTANT)
	{
		m_prop.setConstant(((ConstantDistribution*)pPD)->getConstant());
		delete pPD;
	}
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
