#pragma once
#include "procdb.h"
#include "BoardingCallPaxTypeDatabase.h"
#include "FlightConWithProcIDDatabase.h"

class BoardingCallStandEntry : public ProcessorDataElement
{
protected:
	BoardingCallPaxTypeDatabase m_paxTypeDB;
public:
	BoardingCallStandEntry(const ProcessorID& id);
	~BoardingCallStandEntry();

	void AddPax(CMobileElemConstraint* pInputConst);
	void AddPaxFromOld(ConstraintWithProcIDEntry* pConstEntry);

	BoardingCallPaxTypeDatabase* GetPaxTypeDatabase(){ return &m_paxTypeDB; }
	void SetProcessorID(ProcessorID newID){ procID = newID; }
	int GetPaxCount(){ return (int)m_paxTypeDB.getCount(); }
};

class BoardingCallStandDatabase : public ProcessorDatabase
{
public:
	BoardingCallStandDatabase();
	~BoardingCallStandDatabase();

	virtual void addEntry (const ProcessorID& pID);
	virtual void deleteItem(BoardingCallStandEntry* pStandEntry);
	void writeDatabase(ArctermFile& p_file, InputTerminal* _pInTerm);
};

