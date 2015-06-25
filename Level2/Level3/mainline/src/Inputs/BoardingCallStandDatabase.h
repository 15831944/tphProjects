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
	void AddStand(char* strProc, InputTerminal* _pInTerm);
	void AddStandFromOld(ConstraintWithProcIDEntry* pConstEntry, InputTerminal* _pInTerm);
	virtual void deleteItem(BoardingCallStandEntry* pStandEntry);
	void readDatabase(ArctermFile& p_file, InputTerminal* _pInTerm, int standCount);
	void writeDatabase(ArctermFile& p_file);
};

