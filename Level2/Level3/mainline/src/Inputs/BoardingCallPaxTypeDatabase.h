#pragma once
#include "mobileelemconstraintdatabase.h"
#include "con_db.h"
#include "FlightConWithProcIDDatabase.h"
#include "..\Common\ProbabilityDistribution.h"

class BoardingCallTrigger
{
protected:
	ElapsedTime m_time;
	double m_proportion;
public:
	BoardingCallTrigger(){ m_time = 0L; m_proportion = 0.0; }
	BoardingCallTrigger(long seconds, double _prop);
	~BoardingCallTrigger(){}
	int operator==( const BoardingCallTrigger& _entry ) const
	{
		return (m_time == _entry.m_time && m_proportion == _entry.m_proportion); 
	}

	BoardingCallTrigger& operator=( const BoardingCallTrigger& _entry )
	{
		m_time = _entry.m_time;
		m_proportion = _entry.m_proportion; 
		return *this;
	}

	void InitTrigger(){ SetTrigger(0,0); }
	void SetTrigger(const long _seconds, const double prop){ m_time.set(_seconds); m_proportion = prop; }
	void SetTriggerTime(const ElapsedTime _time){ m_time = _time; }
	void SetTriggerTime(const long _seconds){ m_time.set(_seconds); }
	ElapsedTime& GetTriggerTime(){ return m_time; }
	void SetTriggerProportion(const double prop){ m_proportion = prop; }
	double& GetTriggerProportion(){ return m_proportion; }
};

class BoardingCallPaxTypeEntry : public ConstraintEntry
{
protected:
	std::vector<BoardingCallTrigger> m_vTriggers;
public:
	BoardingCallPaxTypeEntry();
	~BoardingCallPaxTypeEntry();
	void InitTriggerDatabase();
	void InitTriggerDBFromOld(ConstraintWithProcIDEntry* pConstEntry);
	std::vector<BoardingCallTrigger>* GetTriggersDatabase(){ return &m_vTriggers; }
	void DeleteAllTrigger(){ m_vTriggers.clear(); }
	void DeleteTrigger(int index){ m_vTriggers.erase( m_vTriggers.begin() + index); }
	int GetTriggerCount(){ return (int)m_vTriggers.size(); }
	void SetTriggerTime(int index, long time){ m_vTriggers[index].SetTriggerTime(time); }
	void SetTriggerProportion(int index, double prop){ m_vTriggers[index].SetTriggerProportion(prop); }
	void AddNewTrigger();
	void SetTriggerCount(int count);

	void readTriggerDatabase( ArctermFile& p_file, int triggerCount);
	void writeTriggerDatabase( ArctermFile& p_file);
};

class BoardingCallPaxTypeDatabase : public CMobileElemConstraintDatabase
{
public:
	BoardingCallPaxTypeDatabase();
	~BoardingCallPaxTypeDatabase();

	virtual void deleteItem(ConstraintEntry* pConst);
	void AddPaxType(CMobileElemConstraint* pInputConst, InputTerminal* _pInTerm, BOOL bDefault=FALSE);
	void AddPaxTypeFor260OrOlder(ConstraintWithProcIDEntry* pConstEntry, InputTerminal* _pInTerm);
	void readDatabase(ArctermFile& p_file, InputTerminal* _pInTerm, int paxCount);
	void writeDatabase(ArctermFile& p_file);
};

