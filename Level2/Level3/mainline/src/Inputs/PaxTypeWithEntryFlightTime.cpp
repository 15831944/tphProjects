#include "StdAfx.h"
#include "PaxTypeWithEntryFlightTime.h"


PaxTypeWithEntryFlightTime::PaxTypeWithEntryFlightTime(void)
{
}

PaxTypeWithEntryFlightTime::~PaxTypeWithEntryFlightTime(void)
{}

void PaxTypeWithEntryFlightTime::read( ArctermFile& p_file, InputTerminal* _pInTerm )
{
	p_file.getLine();
	constraint->readConstraint(p_file);
	m_entryFlightTime->readDistribution(p_file);
}

void PaxTypeWithEntryFlightTime::write( ArctermFile& p_file )
{
	constraint->writeConstraint(p_file);
	m_entryFlightTime->writeDistribution(p_file);
	p_file.writeLine();
}

ElapsedTime PaxTypeWithEntryFlightTime::getTime()
{
	long seconds = 0;
	seconds = m_entryFlightTime->getRandomValue();
	return ElapsedTime(seconds);
}

void PaxTypeWithEntryFlightTime::setTime(ProbabilityDistribution* pProbDist)
{
	if(m_entryFlightTime != NULL)
	{
		delete m_entryFlightTime;
	}
	m_entryFlightTime = pProbDist;
}

//////////////////////////////////////////////////////////////////////////////////////////////


EntryFlightTimeDB::EntryFlightTimeDB()
{
}

EntryFlightTimeDB::~EntryFlightTimeDB()
{

}

void EntryFlightTimeDB::deleteItem(ConstraintEntry* pConst)
{
	removeItem(pConst);
	delete pConst;
}

int EntryFlightTimeDB::FindItemByConstraint( CMobileElemConstraint* pInputConst )
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

void EntryFlightTimeDB::AddPaxType( CMobileElemConstraint* pInputConst, InputTerminal* _pInTerm )
{
	CMobileElemConstraint* pMBConst = new CMobileElemConstraint();
	if(pInputConst != NULL)
	{
		ASSERT(pInputConst->GetInputTerminal() != NULL);
		*pMBConst = *pInputConst;
		PaxTypeWithEntryFlightTime* pPaxEntry = new PaxTypeWithEntryFlightTime();
		pPaxEntry->initialize(pMBConst, NULL);
		addEntry(pPaxEntry);// If the PaxEntry with PaxType 'pInputConst' exists, it will be replaced.
	}
	else
	{
		ASSERT(_pInTerm);
		pMBConst->SetInputTerminal(_pInTerm);
		pMBConst->SetMobileElementType(enum_MobileElementType_ALL); /* Set 'Passenger Type': DEFAULT */
		PaxTypeWithEntryFlightTime* pPaxEntry = new PaxTypeWithEntryFlightTime();
		pPaxEntry->initialize(pMBConst, NULL);
		addEntry(pPaxEntry);
	}
}

void EntryFlightTimeDB::readDatabase(ArctermFile& p_file, InputTerminal* _pInTerm)
{
	clear();
	int paxCount = 0;
	p_file.getLine();
	p_file.getInteger(paxCount);
	for(int i=0; i<paxCount; i++)
	{
		PaxTypeWithEntryFlightTime* pPaxType = new PaxTypeWithEntryFlightTime();
		pPaxType->read(p_file, _pInTerm);
	}
}

void EntryFlightTimeDB::writeDatabase(ArctermFile& p_file)
{
	int paxCount = getCount();
	p_file.writeInt(paxCount);
	p_file.writeLine();
	PaxTypeWithEntryFlightTime* pPaxEntry = NULL;
	for(int i=0; i<paxCount; i++)
	{
		pPaxEntry = (PaxTypeWithEntryFlightTime*)getItem(i);
		pPaxEntry->write(p_file);
	}
}
