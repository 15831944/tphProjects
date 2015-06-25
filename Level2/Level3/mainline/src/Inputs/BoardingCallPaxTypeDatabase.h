#pragma once
#include "mobileelemconstraintdatabase.h"
#include "con_db.h"

class BoardingCallTrigger
{
protected:
	ElapsedTime m_time;
	double m_proportion;
public:
	BoardingCallTrigger(){ m_time = 0L; m_proportion = 0.0; }
	BoardingCallTrigger(long _time, double _prop);
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
	void InitTriggersDatabase();
	std::vector<BoardingCallTrigger>* GetTriggersDatabase(){ return &m_vTriggers; }
	void DeleteTrigger(int index){ m_vTriggers.erase( m_vTriggers.begin() + index); }
	int GetTriggerCount(){ return m_vTriggers.size(); }
	void SetTriggerTime(int index, long time){ m_vTriggers[index].SetTriggerTime(time); }
	void SetTriggerProportion(int index, double prop){ m_vTriggers[index].SetTriggerProportion(prop); }
	void AddNewTrigger();
	void SetTriggerCount(int count);
};

class BoardingCallPaxTypeDatabase : public CMobileElemConstraintDatabase
{
public:
	virtual void deleteItem(ConstraintEntry* pConst);
	BoardingCallPaxTypeDatabase();
	~BoardingCallPaxTypeDatabase();
};

