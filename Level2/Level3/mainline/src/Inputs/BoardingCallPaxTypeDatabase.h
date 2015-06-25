#pragma once
#include "mobileelemconstraintdatabase.h"
#include "con_db.h"
#include "FlightConWithProcIDDatabase.h"
#include "..\Common\ProbabilityDistribution.h"

class BoardingCallTrigger
{
protected:
	ConstantDistribution m_time;
	ConstantDistribution m_prop;
public:
	BoardingCallTrigger(){ InitTrigger(); }
	BoardingCallTrigger(long _seconds, double _prop);
	~BoardingCallTrigger();

	BoardingCallTrigger& operator=( const BoardingCallTrigger& _entry )
	{
		m_time = _entry.m_time;
		m_prop = _entry.m_prop; 
		return *this;
	}

	void InitTrigger();
	void SetTriggerTime(long _seconds);
	void SetTriggerProp(double _prop);
	double GetTriggerTimeValue(){ return m_time.getConstant(); }
	double GetTriggerPropValue(){ return m_prop.getConstant(); }
	CString GetTriggerTimeStr(){ return "";}//m_time.screenPrint(); }
	CString GetTriggerPropStr(){ return ""; }
	void writeTrigger(ArctermFile& p_file);
	void readTrigger(ArctermFile& p_file);
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
	void SetTriggerProportion(int index, double prop){ m_vTriggers[index].SetTriggerProp(prop); }
	void AddNewTrigger();
	void SetTriggerCount(int count);

	void readTriggerDatabase( ArctermFile& p_file, int triggerCount);
	void writeTriggerDatabase( ArctermFile& p_file);
private:
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

