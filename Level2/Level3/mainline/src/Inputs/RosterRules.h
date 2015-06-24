#pragma once
#include "common\containr.h"
#include "common\dataset.h"
#include "inputs\procdb.h"
#include "inputs\procid.h"
#include "inputs\assign.h"
#include "../Engine/proclist.h"
class CRosterRules
{
private:
	ElapsedTime openTime;
	ElapsedTime closeTime;
	CMultiMobConstraint assign;	
	BOOL isDaily ;
	int n_num;
	InputTerminal* p_input ;
public:
	CRosterRules(InputTerminal* inputTer);
	CRosterRules(const CRosterRules& rule) ;


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
    
	 void ReadDataFromFile(ArctermFile& p_file, InputTerminal* _pInTerm ) ;
	 void WriteDataFromFile(ArctermFile& p_file) ;
	void FormatProAssignment(ProcAssignment& pro_Ass)
	{
		pro_Ass.setOpenTime(openTime);
		pro_Ass.setCloseTime(closeTime);
		pro_Ass.setAbsOpenTime(openTime);
		pro_Ass.setAbsCloseTime(closeTime);
		pro_Ass.setAssignment(assign) ;
		pro_Ass.setFlag(1) ;
		pro_Ass.setAssRelation(Inclusive) ;
	}

	int operator == (const CRosterRules& p_entry) 
	{ return openTime == p_entry.openTime; }
	int operator < (const CRosterRules& p_entry) 
	{ return openTime < p_entry.openTime; }
	int operator = (const CRosterRules& p_entry)
	{
		openTime = p_entry.openTime ;
		closeTime = p_entry.closeTime ;
		assign = p_entry.assign ;
		isDaily = p_entry.isDaily ;
		n_num = p_entry.n_num ;
		return 1 ;
	}
	~CRosterRules(void);
};
class CProcessGroupRules :public SortedContainer<CRosterRules>
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
	int FindRosterRule(CRosterRules* rule) ;
};
class CAutoRosterRulesDB : public DataSet ,  public SortedContainer<CProcessGroupRules>
{
public:
    CAutoRosterRulesDB(InputTerminal* input_ter);
    virtual ~CAutoRosterRulesDB() ;

	virtual void readData(ArctermFile& p_file) ;
	virtual void writeData(ArctermFile& p_file)const ;

	CProcessGroupRules*  AddGroupRules(const ProcessorID& pro_id) ;
	void AddRosterRules(CRosterRules* rule,const ProcessorID& pro_id) ;

	CProcessGroupRules* FindGroupRules(const ProcessorID& pro_id) ;

	virtual const char *getTitle (void) const
	{ return "Auto Assignment file"; }
	virtual const char *getHeaders (void) const
	{ return "Processor,IsDaily,Time,PassengerTy"; }
	virtual void clear() ;
};