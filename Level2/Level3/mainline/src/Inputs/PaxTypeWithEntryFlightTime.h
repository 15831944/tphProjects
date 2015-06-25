#pragma once
#include "con_db.h"
#include "Common\ProbabilityDistribution.h"
#include "MobileElemConstraintDatabase.h"

class PaxTypeWithEntryFlightTime : public ConstraintEntry
{
public:
	PaxTypeWithEntryFlightTime(void);
	~PaxTypeWithEntryFlightTime(void);
protected:
	ProbabilityDistribution* m_entryFlightTime;

public:
	void read( ArctermFile& p_file, InputTerminal* _pInTerm);
	void write( ArctermFile& p_file);
	ElapsedTime getTime();
	void setTime(ProbabilityDistribution* pProbDist);
};

class EntryFlightTimeDB : public CMobileElemConstraintDatabase
{
public:
	EntryFlightTimeDB();
	~EntryFlightTimeDB();

public:
	virtual void deleteItem(ConstraintEntry* pConst);
	int FindItemByConstraint(CMobileElemConstraint* pInputConst);
	void AddPaxType(CMobileElemConstraint* pInputConst, InputTerminal* _pInTerm);
	void readDatabase(ArctermFile& p_file, InputTerminal* _pInTerm);
	void writeDatabase(ArctermFile& p_file);
};

