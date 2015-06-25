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
	AddResidualTrigger();
	AddRegularTrigger(-1200L, 20);
	AddRegularTrigger(-1000L, 70);
	AddRegularTrigger(-600L, 10);
}

void BoardingCallPaxTypeEntry::InitTriggerDBFromOld(ConstraintWithProcIDEntry* pConstEntry)
{
	AddResidualTrigger();
	if(pConstEntry->getValue()->getProbabilityType() == HISTOGRAM)
	{
		const HistogramDistribution* pDistribution = (HistogramDistribution*)pConstEntry->getValue();
		int count = pDistribution->getCount();
		for(int i=0; i<count; i++)
		{
			long seconds = (long)pDistribution->getValue(i);
			double prop;
			if(i == 0)
			{
				prop = pDistribution->getProb(i);
			}
			else
			{
				prop = pDistribution->getProb(i) - pDistribution->getProb(i-1);
			}
			AddRegularTrigger(seconds, prop);
		}
	}
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
		AddRegularTrigger(0, 0);
	}
}

void BoardingCallPaxTypeEntry::readTriggerDatabase( ArctermFile& p_file, InputTerminal* _pInTerm, int triggerCount)
{
	DeleteAllTrigger();
	for(int i=0; i<triggerCount; i++)
	{
		p_file.getLine();
		p_file.skipField(1);/* 'TriggerN' */
		BoardingCallTrigger* pTrigger = new BoardingCallTrigger();
		pTrigger->readTrigger(p_file, _pInTerm);
		m_vTriggers.push_back(pTrigger);
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
		m_vTriggers[i]->writeTrigger(p_file);
		p_file.writeLine();
	}
}

void BoardingCallPaxTypeEntry::AddResidualTrigger()
{
	BoardingCallTrigger* pTrigger = new BoardingCallTrigger(-500L, -1);/* The 'RESIDUAL' trigger. */
	m_vTriggers.push_back(pTrigger);
}

void BoardingCallPaxTypeEntry::AddRegularTrigger( long _seconds, double _prop )
{
	BoardingCallTrigger* pTrigger = new BoardingCallTrigger(_seconds, _prop);
	m_vTriggers.insert(m_vTriggers.end()-1, pTrigger); /* Add before the 'RESIDUAL' trigger. */
}

BoardingCallPaxTypeDatabase::BoardingCallPaxTypeDatabase()
{
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
		ASSERT(pInputConst->GetInputTerminal() != NULL);
		*pMBConst = *pInputConst;
		BoardingCallPaxTypeEntry* pPaxEntry = new BoardingCallPaxTypeEntry();
		pPaxEntry->initialize(pMBConst, NULL);
		pPaxEntry->InitTriggerDatabase();	/* InitTriggersDatabase(): The 4 initial triggers will be added here. */
		addEntry(pPaxEntry);// If the PaxEntry with PaxType 'pInputConst' exists, it will be replaced.
	}
	else
	{
		ASSERT(_pInTerm);
		pMBConst->SetInputTerminal(_pInTerm);
		pMBConst->SetMobileElementType(enum_MobileElementType_ALL); /* Set 'Passenger Type': DEFAULT */
		BoardingCallPaxTypeEntry* pPaxEntry = new BoardingCallPaxTypeEntry();
		pPaxEntry->initialize(pMBConst, NULL);
		pPaxEntry->AddResidualTrigger();
		addEntry(pPaxEntry);
	}
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
		((BoardingCallPaxTypeEntry*)getItem(i))->readTriggerDatabase(p_file, _pInTerm, triggerCount);
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
	return INT_MAX;
}

BoardingCallTrigger::BoardingCallTrigger()
{
	m_time = NULL;
	m_prop = NULL;
}

BoardingCallTrigger::BoardingCallTrigger( long _seconds, double _prop )
{
	m_time = new ConstantDistribution(_seconds);
	m_prop = new ConstantDistribution(_prop);
}

BoardingCallTrigger::~BoardingCallTrigger()
{
	if(m_time)
		delete m_time;
	if(m_prop)
		delete m_prop;
}

void BoardingCallTrigger::writeTrigger( ArctermFile& p_file )
{
	m_time->writeDistribution(p_file);
	m_prop->writeDistribution(p_file);
}

void BoardingCallTrigger::readTrigger(ArctermFile& p_file, InputTerminal * _pInterm)
{
	m_time = GetProbDistribution(p_file);
	m_prop = GetProbDistribution(p_file);
}

