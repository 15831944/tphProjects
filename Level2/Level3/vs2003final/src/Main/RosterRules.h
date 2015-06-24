#pragma once
#include "..\common\containr.h"
#include "..\common\dataset.h"
#include "..\inputs\procdb.h"
#include <CommonData/procid.h>
#include "..\inputs\assign.h"
#include "../Engine/proclist.h"
#define  TY_SCHEDULE_RULE 1 
#define  TY_RULE   2 
class CADORecordset ;
class CRosterRule
{
public:
	enum INTERVAL_TIME { BEFORE , AFTER} ;
protected:
	ElapsedTime openTime;
	ElapsedTime closeTime;
	CMultiMobConstraint assign;	
	BOOL isDaily ;
	int n_num;
	InputTerminal* p_input ;
	int m_Relation ;
	int ID ;
	INTERVAL_TIME m_OpentAdvanceOrNot ;
	INTERVAL_TIME m_ClosetAdvanceOrNot ;
public:
	CRosterRule(InputTerminal* inputTer);
	CRosterRule(const CRosterRule& rule) ;


	ElapsedTime GetOpenTime() {return openTime ;} ;
	ElapsedTime GetCloseTime() {return closeTime ;} ;
	CMultiMobConstraint *GetAssignment (void)  { return &assign; } ;
	BOOL IsDaily() { return isDaily ;} ;
	int  GetCount() { return n_num ;} ;
	void SetOpenTime(const ElapsedTime& time) { openTime = time ;} ;
	void SetCloseTime(const ElapsedTime& time ) { closeTime = time ;} ;
	void SetAssigment(const CMultiMobConstraint& constraint) { assign = constraint ;} ;
	void IsDaily(BOOL daily) {  isDaily = daily ;} ;
	void SetCount(int num) { n_num = num ;} ;
    
	INTERVAL_TIME GetOpenIfAdvance() { return m_OpentAdvanceOrNot ;} ;
	void SetOpenAdvance(INTERVAL_TIME _advance) { m_OpentAdvanceOrNot = _advance ;} ;

	INTERVAL_TIME GetCloseIfAdvance() { return m_ClosetAdvanceOrNot ;} ;
	void SetCloseAdvance(INTERVAL_TIME _advance) { m_ClosetAdvanceOrNot = _advance ;} ;

	void SetRelation(int _rel) { m_Relation = _rel ;} ;
	int  GetRelation() { return m_Relation ;} ;
	virtual  void ReadDataFromFile(ArctermFile& p_file, InputTerminal* _pInTerm ) ;
	virtual  void WriteDataFromFile(ArctermFile& p_file) ;
    virtual void AddRosterToSchedule(ProcessorRosterSchedule* _schedule) ;
	int operator == (const CRosterRule& p_entry) 
	{ return openTime == p_entry.openTime; }
	int operator < (const CRosterRule& p_entry) 
	{ return openTime < p_entry.openTime; }
	int operator = (const CRosterRule& p_entry)
	{
		openTime = p_entry.openTime ;
		closeTime = p_entry.closeTime ;
		assign = p_entry.assign ;
		isDaily = p_entry.isDaily ;
		n_num = p_entry.n_num ;
		m_Relation = p_entry.m_Relation ;
		return 1 ;
	}
	virtual ~CRosterRule(void);

	int GetID() { return ID ;} ;
	void SetID(int id) { ID = id ;} ;
protected:
	virtual void CreateProcessorRoster(ProcessorRoster& pro_Ass)
	{
		pro_Ass.setOpenTime(openTime);
		pro_Ass.setCloseTime(closeTime);
		pro_Ass.setAbsOpenTime(openTime);
		pro_Ass.setAbsCloseTime(closeTime);
		pro_Ass.setAssignment(assign) ;
		pro_Ass.setFlag(1) ;
		pro_Ass.setAssRelation(Inclusive) ;
	}
};



//////////////////////////////////////////////////////////////////////////////////////
class CRosterRulesBySchedule : public CRosterRule
{
public:
	CRosterRulesBySchedule(InputTerminal* inputTer) ;
	virtual ~CRosterRulesBySchedule() ;
protected:
	virtual void CreateProcessorRoster(ProcessorRoster& pro_Ass,Flight* _flight,int _mode) ;
	  void AddProcAssigment(ProcessorRosterSchedule* _schedule,Flight* pFlight ,char _mode) ;
public:
	virtual void AddRosterToSchedule(ProcessorRosterSchedule* _schedule) ;
};



class CProcessGroupRules :public SortedContainer<CRosterRule>
{
private:
	ProcessorID pro_ID ;
public:
	CProcessGroupRules(void) ;
	CProcessGroupRules(const ProcessorID& pro_id) ;
	CProcessGroupRules(const CProcessGroupRules& grouprules) ;
    virtual ~CProcessGroupRules() ;
	void SetProcessorId(const ProcessorID& pro_id) { pro_ID = pro_id ;};
	const ProcessorID* GetProcessorId() {return &pro_ID ;} ;

	int operator ==(const CProcessGroupRules& rules)
	{
		return pro_ID == rules.pro_ID ;
	}
	virtual void clear() ;
	void clearDelete() ;
	int FindRosterRule(CRosterRule* rule) ;
	void RemoveRule(CRosterRule* rule) ;
	const std::vector<CRosterRule*>* GetDelItem() { return &m_DelRules ;} ;
private:
	std::vector<CRosterRule*> m_DelRules ;
};

class CAutoRosterRulesDB : public DataSet ,  public SortedContainer<CProcessGroupRules>
{
public:
    CAutoRosterRulesDB(InputTerminal* input_ter);
    virtual ~CAutoRosterRulesDB() ;

	virtual void readData(ArctermFile& p_file) ;
	virtual void writeData(ArctermFile& p_file)const ;

	CProcessGroupRules*  AddGroupRules(const ProcessorID& pro_id) ;
	void AddRosterRules(CRosterRule* rule,const ProcessorID& pro_id) ;

	CProcessGroupRules* FindGroupRules(const ProcessorID& pro_id) ;

	virtual const char *getTitle (void) const
	{ return "Auto Assignment file"; }
	virtual const char *getHeaders (void) const
	{ return "Processor,IsDaily,Time,PassengerTy"; }
	virtual void clear() ;

	virtual void ReadDataFromDB() ;
	virtual void WriteDataToDB() ;
protected:
	int  TY ;
	ProcessorID ReadRules(CRosterRule* _rule ,CADORecordset* recordSet ) ;
	void writeRules(CString& _prostr ,CRosterRule* _rule) ;
	void DeleteRules(CRosterRule* _rule) ;


};
class CAutoRosterByScheduleRulesDB : public CAutoRosterRulesDB
{
public:
	CAutoRosterByScheduleRulesDB(InputTerminal* input_ter) ;
    virtual ~CAutoRosterByScheduleRulesDB() ;
public:
	virtual void readData(ArctermFile& p_file) ;
	virtual void writeData(ArctermFile& p_file)const ;

};
