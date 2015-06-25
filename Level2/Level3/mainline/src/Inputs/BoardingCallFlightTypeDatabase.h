#pragma once
#include "FLT_DB.h"
#include "con_db.h"
#include "BoardingCallStandDatabase.h"
#include "FlightConWithProcIDDatabase.h"

class BoardingCallFlightTypeEntry : public ConstraintEntry
{
protected:
	BoardingCallStandDatabase m_standDB;
public:
	BoardingCallFlightTypeEntry();
	~BoardingCallFlightTypeEntry();
	BoardingCallStandDatabase* GetStandDatabase(){ return &m_standDB; }
};


class BoardingCallFlightTypeDatabase : public FlightConstraintDatabase
{
public:
	BoardingCallFlightTypeDatabase();
	~BoardingCallFlightTypeDatabase();

	virtual void deleteItem(ConstraintEntry* pConst);
	void DoDataCompatibleWith260AndEarlier(FlightConWithProcIDDatabase* pOldStructor);
};

