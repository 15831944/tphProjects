#pragma once
#include "procdb.h"
#include "BoardingCallPaxTypeDatabase.h"

class BoardingCallStandEntry : public ProcessorDataElement
{
protected:
	BoardingCallPaxTypeDatabase m_paxTypeDB;
public:
	BoardingCallStandEntry();
	~BoardingCallStandEntry();
	BoardingCallPaxTypeDatabase* GetPaxTypeDatabase(){ return &m_paxTypeDB; }
};

class BoardingCallStandDatabase : public ProcessorDatabase
{
public:
	BoardingCallStandDatabase();
	~BoardingCallStandDatabase();
};

