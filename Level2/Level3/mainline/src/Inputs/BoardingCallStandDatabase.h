#pragma once
#include "procdb.h"
#include "BoardingCallPaxTypeDatabase.h"

class BoardingCallStandEntry : public ProcessorDataElement
{
protected:
	BoardingCallPaxTypeDatabase m_paxTypeDB;
public:
	BoardingCallStandEntry(const ProcessorID& id);
	~BoardingCallStandEntry();
	BoardingCallPaxTypeDatabase* GetPaxTypeDatabase(){ return &m_paxTypeDB; }
	void SetProcessorID(ProcessorID newID){ procID = newID; }
};

class BoardingCallStandDatabase : public ProcessorDatabase
{
public:
	virtual void addEntry (const ProcessorID& pID);
	virtual void deleteItem(BoardingCallStandEntry* pStandEntry);
	BoardingCallStandDatabase();
	~BoardingCallStandDatabase();
};

