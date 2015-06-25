#pragma once
#include "mobileelemconstraintdatabase.h"
#include "con_db.h"
#include "FlightConWithProcIDDatabase.h"
#include "..\Common\ProbabilityDistribution.h"

class BoardingCallTrigger
{
public:
	ProbabilityDistribution* m_time;
	ProbabilityDistribution* m_prop;
public:
	BoardingCallTrigger();
	BoardingCallTrigger(long _seconds, double _prop);
	~BoardingCallTrigger();

	void writeTrigger(ArctermFile& p_file);
	void readTrigger(ArctermFile& p_file, InputTerminal* _pInterm);

	BoardingCallTrigger& operator=( const BoardingCallTrigger& _entry )
	{
		*m_time = *_entry.m_time;
		*m_prop = *_entry.m_prop;
		return *this;
	}
};

class BoardingCallPaxTypeEntry : public ConstraintEntry
{
protected:
	std::vector<BoardingCallTrigger*> m_vTriggers;
public:
	BoardingCallPaxTypeEntry();
	~BoardingCallPaxTypeEntry();
	void InitTriggerDatabase();
	void InitTriggerDBFromOld(ConstraintWithProcIDEntry* pConstEntry);
	std::vector<BoardingCallTrigger*>& GetTriggersDatabase(){ return m_vTriggers; }
	void DeleteAllTrigger(){ m_vTriggers.clear(); }
	void DeleteTrigger(BoardingCallTrigger* pTrigger);
	int GetTriggerCount(){ return (int)m_vTriggers.size(); }
	void SetTriggerCount(int count);

	void readTriggerDatabase( ArctermFile& p_file, InputTerminal* _pInTerm, int triggerCount);
	void writeTriggerDatabase( ArctermFile& p_file);

	void AddRegularTrigger(long _seconds, double _prop);
	void AddResidualTrigger();
};

class BoardingCallPaxTypeDatabase : public CMobileElemConstraintDatabase
{
public:
	BoardingCallPaxTypeDatabase();
	~BoardingCallPaxTypeDatabase();

	virtual void deleteItem(ConstraintEntry* pConst);
	int FindItemByConstraint(CMobileElemConstraint* pInputConst);
	void AddPaxType(CMobileElemConstraint* pInputConst, InputTerminal* _pInTerm);
	void AddPaxTypeFor260AndOlder(ConstraintWithProcIDEntry* pConstEntry, InputTerminal* _pInTerm);
	void readDatabase(ArctermFile& p_file, InputTerminal* _pInTerm, int paxCount);
	void writeDatabase(ArctermFile& p_file);
};

