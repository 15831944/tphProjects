#include "StdAfx.h"
#include "rosterrules.h"
#include "../Inputs/schedule.h"
#include "../Inputs/Flight.h"
#include "../Database/ADODatabase.h"
#include <Inputs/IN_TERM.H>
#include <Inputs/PROCIDList.h>

CRosterRule::CRosterRule(InputTerminal* inputTer):openTime(0l),closeTime(0l),isDaily(TRUE),n_num(0),p_input(inputTer),m_Relation(0),ID(-1)
{
	assign.addConstraint(CMobileElemConstraint(inputTer)) ;
	 m_OpentAdvanceOrNot = BEFORE;
	 m_ClosetAdvanceOrNot = BEFORE;
}
CRosterRule::CRosterRule(const CRosterRule& rule) :ID(-1)
{
	openTime = rule.openTime ;
	closeTime = rule.closeTime;
	this->isDaily= rule.isDaily;
	assign = rule.assign ;
	n_num = rule.n_num ;
	m_Relation = rule.m_Relation ;
	m_OpentAdvanceOrNot = BEFORE;
	m_ClosetAdvanceOrNot = BEFORE;
}
// do not realize
void CRosterRule::ReadDataFromFile(ArctermFile& p_file, InputTerminal* _pInTerm )
{
 
  p_file.getInteger(n_num) ;
  p_file.getInteger(isDaily) ;
  p_file.getTime(openTime) ;
  p_file.getTime(closeTime) ;
  assign.readConstraints(p_file,_pInTerm) ;
  if(p_file.getVersion() > (float) 2.2)
     p_file.getInteger(m_Relation) ;
}
void CRosterRule::WriteDataFromFile(ArctermFile& p_file)
{
 p_file.writeInt(n_num) ;
 p_file.writeInt(isDaily) ;
 p_file.writeTime(openTime) ;
 p_file.writeTime(closeTime) ;
 assign.writeConstraints(p_file) ;
 p_file.writeInt(m_Relation) ;

}
void CRosterRule::AddRosterToSchedule(ProcessorRosterSchedule* _schedule) 
{
	if(_schedule == NULL)
		return ;
	ProcessorRoster* _ass = new ProcessorRoster ;
	CreateProcessorRoster(*_ass) ;
	_schedule->addItem(_ass) ;
}
CRosterRule::~CRosterRule(void)
{
}
CProcessGroupRules::CProcessGroupRules() 
{

}
CProcessGroupRules::CProcessGroupRules(const ProcessorID& pro_id) 
{
	pro_ID = pro_id ;
}
void CProcessGroupRules::clear()
{
   CRosterRule* rule = NULL ;
   for (int i = 0 ; i < getCount() ; i++)
   {
	   rule = getItem(i) ;
	   if(rule != NULL)
		   delete rule ;
   }
   clearDelete() ;
}
void CProcessGroupRules::clearDelete()
{
	CRosterRule* rule = NULL ;
	for (int m = 0 ; m < (int)m_DelRules.size() ; m++)
	{
		rule = m_DelRules[m] ;
		if(rule != NULL)
			delete rule ;
	}
	m_DelRules.clear() ;
}
CProcessGroupRules::CProcessGroupRules(const CProcessGroupRules& grouprules) 
{
  clear() ;
  CRosterRule* rule = NULL ;
  for (int i = 0 ; i< grouprules.getCount() ; i++)
  {
      rule = new CRosterRule(*(grouprules.getItem(i))) ;
	  addItem(rule) ;
  }
  pro_ID = grouprules.pro_ID ;
}
int CProcessGroupRules::FindRosterRule(CRosterRule* rule)
{
	for (int i = 0 ; i < getCount() ;i++)
	{
		if(getItem(i) == rule)
			return i ;
	}
	return INT_MAX ;
}
void CProcessGroupRules::RemoveRule(CRosterRule* rule) 
{
	if(rule == NULL)
		return ;
	int ndx = FindRosterRule(rule) ;
	if(ndx != INT_MAX)
	{
		removeItem(ndx);
		m_DelRules.push_back(rule) ;
	}
}
CProcessGroupRules::~CProcessGroupRules()
{
	clear() ;
}
CAutoRosterRulesDB::CAutoRosterRulesDB(InputTerminal* input_ter):DataSet(RosterRulesFile)
{
   SetInputTerminal(input_ter) ;
    TY = TY_RULE ;
}
 CAutoRosterRulesDB::~CAutoRosterRulesDB() 
 {
	 clear() ;
 }
 void CAutoRosterRulesDB::clear()
 {
	 CProcessGroupRules* groupRules = NULL ;
	 for (int i = 0 ; i < getCount() ; i++)
	 {
		 groupRules = getItem(i) ;
		 if(groupRules != NULL)
			 delete groupRules ;
	 }
 }
 CProcessGroupRules*  CAutoRosterRulesDB::AddGroupRules(const ProcessorID& pro_id)
 {
    CProcessGroupRules* p_groupRules = FindGroupRules(pro_id) ;
	if(p_groupRules == NULL)
	{
		p_groupRules = new CProcessGroupRules(pro_id) ;
		addItem(p_groupRules) ;
	}
	return p_groupRules ;
 }
 void CAutoRosterRulesDB::AddRosterRules(CRosterRule* rule,const ProcessorID& pro_id)
 {
	 if(rule != NULL)
	 {
		 CProcessGroupRules* p_groupRules = AddGroupRules(pro_id) ;
		 p_groupRules->addItem(rule) ;
	 }
 }
 CProcessGroupRules* CAutoRosterRulesDB::FindGroupRules(const ProcessorID& pro_id)
 {
	 CProcessGroupRules* p_groupRules = NULL ;
	 for (int i = 0 ; i < getCount() ; i++)
	 {
             p_groupRules = getItem(i) ;
		 if(*(p_groupRules->GetProcessorId()) == pro_id)
				 return p_groupRules ;
	 }
	 return NULL ;
 }
 void CAutoRosterRulesDB::readData(ArctermFile& p_file)
 {
	 if(p_file.getVersion() > (float)2.2)
	 {
		 ReadDataFromDB() ;
		 return ;
	 }
	 assert( m_pInTerm );
	 CRosterRule *rules = NULL; 
	 clear();
	 ProcessorIDList idList;
	 idList.ownsElements( 1 );//cause idList.readIDList (p_file, m_pInTerm ); will alloc memory 
	 ProcessorRoster procAssign( m_pInTerm );
     StringList grouplist ;
	 m_pInTerm->GetProcessorList()->getAllGroupNames(grouplist,-1) ;
	 CString name ;
	 ProcessorID id;
	 id.SetStrDict( GetInputTerminal()->inStrDict );
	 while (p_file.getLine())
	 {
		 idList.readIDList (p_file, m_pInTerm );
		 name = idList.getID(0)->GetIDString() ;
		 id.setID(name) ;
		GroupIndex index =  m_pInTerm->GetProcessorList()->getGroupIndex(id) ;
		 if (index.end == -1 || index.start == -1 )
			 continue ;
		 rules = new CRosterRule(GetInputTerminal()) ;
		rules->ReadDataFromFile(p_file,m_pInTerm) ;
		AddRosterRules(rules,*(idList.getID(0))) ;
	 }
 }
 void CAutoRosterRulesDB::writeData(ArctermFile& p_file)const
 {
	 if(p_file.getVersion() >(float) 2.2)
	 {
		 CAutoRosterRulesDB* DB =  const_cast<CAutoRosterRulesDB*>(this) ;
		  DB->WriteDataToDB() ;
		 return ;
	 }
	 char procName[256] = {0};
	 CProcessGroupRules *database;
	 CRosterRule* rosterRule = NULL ;
	 int entryCount = getCount();
	 for (int i = 0; i < entryCount; i++)
	 {
		 database = getItem(i) ;
		 database->GetProcessorId()->printID (procName);
		 for (int j = 0; j < database->getCount(); j++)
		 {
			 rosterRule = database->getItem(j) ;
			 p_file.writeField (procName);
			 (database->getItem(j))->WriteDataFromFile(p_file);
			 p_file.writeLine();
		 }
	 }
 }
 void CAutoRosterRulesDB::DeleteRules(CRosterRule* _rule)
 {
	 if(_rule->GetID() == -1)
		 return ;
	 CString sqlstr ;
	 sqlstr.Format(_T("DELETE FROM TB_ROSTER_RULES WHERE ID = %d"),_rule->GetID()) ;
		 CADODatabase::ExecuteSQLStatement(sqlstr,DATABASESOURCE_TYPE_ACCESS) ;
 }
 ProcessorID CAutoRosterRulesDB::ReadRules(CRosterRule* _rule ,CADORecordset* recordSet )
 {
	 ProcessorID Proid;
	 Proid.SetStrDict( GetInputTerminal()->inStrDict );
	 if(_rule == NULL || recordSet == NULL)
		 return Proid;
	 int  id = -1;
	 CString  paxTy ;
	 CString STARTTIME ;
	 CString ENDTIME ;
	 int RELATION ;
	 int ISDAILY ;
	 int PROCESSORNUM ;
	 CString ProID ;
	 int advance = 0 ;
	 recordSet->GetFieldValue(_T("ID"),id) ;
	 recordSet->GetFieldValue(_T("PAXTY"),paxTy) ;
	 recordSet->GetFieldValue(_T("STARTTIME"),STARTTIME) ;
	 recordSet->GetFieldValue(_T("ENDTIME"),ENDTIME) ;
	 recordSet->GetFieldValue(_T("RELATION"),RELATION) ;
	 recordSet->GetFieldValue(_T("ISDAILY"),ISDAILY) ;
	 recordSet->GetFieldValue(_T("PROCESSORNUM"),PROCESSORNUM) ;
	 recordSet->GetFieldValue(_T("PROCESSID"),ProID) ;
	 recordSet->GetFieldValue(_T("OPEN_ADVANCE_OR_NOT"),advance) ;
	 _rule->SetOpenAdvance((CRosterRule::INTERVAL_TIME)advance) ;

	 recordSet->GetFieldValue(_T("CLOSE_ADVANCE_OR_NOT"),advance) ;
	 _rule->SetCloseAdvance((CRosterRule::INTERVAL_TIME)advance) ;

	_rule->GetAssignment()->getConstraint(0)->setConstraintWithVersion(paxTy);
	 ElapsedTime opentime ;
	 ElapsedTime closetime ;
	 opentime.SetTime(ENDTIME) ;
	 closetime.SetTime(STARTTIME) ;
	 _rule->SetCloseTime(opentime) ;
	 _rule->SetOpenTime(closetime) ;
	 _rule->SetRelation(RELATION) ;
	 _rule->SetCount(PROCESSORNUM) ;
	 _rule->SetID(id) ;
	 _rule->IsDaily(ISDAILY) ;

	 Proid.setID(ProID) ;
	 return Proid ;

 }
 void CAutoRosterRulesDB::writeRules(CString& _prostr ,CRosterRule* _rule)
 {
	 CString  StrSQL ;
	 TCHAR paxty[1024] = {0} ;
	 _rule->GetAssignment()->getConstraint(0)->WriteSyntaxStringWithVersion(paxty) ;

	 if(_rule->GetID() == -1)
	 {
		 StrSQL.Format(_T("INSERT INTO TB_ROSTER_RULES (PAXTY, STARTTIME,ENDTIME,RELATION,ISDAILY,PROCESSORNUM,TYID,PROCESSID,OPEN_ADVANCE_OR_NOT,CLOSE_ADVANCE_OR_NOT) \
						  VALUES ('%s', '%s','%s' ,%d,%d,%d,%d,'%s',%d,%d)"),
						  paxty, 
						  _rule->GetOpenTime().printTime(),
						  _rule->GetCloseTime().printTime() ,
						  _rule->GetRelation(),
						  _rule->IsDaily(),
						  _rule->GetCount(),
						  TY,
						  _prostr,
						  (int)_rule->GetOpenIfAdvance(),
						  (int)_rule->GetCloseIfAdvance()) ;
		 int id = CADODatabase::ExecuteSQLStatementAndReturnScopeID(StrSQL,DATABASESOURCE_TYPE_ACCESS) ;
		 _rule->SetID(id) ;
	 }
	 else
	 {
		 StrSQL.Format(_T("UPDATE TB_ROSTER_RULES SET PAXTY ='%s',STARTTIME = '%s',ENDTIME = '%s',RELATION = %d,ISDAILY =  %d,PROCESSORNUM =  %d,TYID =  %d,PROCESSID = '%s', \
						  OPEN_ADVANCE_OR_NOT = %d ,CLOSE_ADVANCE_OR_NOT = %d WHERE ID = %d"),
		 paxty, 
		 _rule->GetOpenTime().printTime(),
		 _rule->GetCloseTime().printTime() ,
		 _rule->GetRelation(),
		 _rule->IsDaily(),
		 _rule->GetCount(),
		 TY,
		 _prostr,
		 (int)_rule->GetOpenIfAdvance(),
		 (int)_rule->GetCloseIfAdvance(),
		 _rule->GetID()) ;
		 CADODatabase::ExecuteSQLStatement(StrSQL,DATABASESOURCE_TYPE_ACCESS) ;
	 }
 }
 void CAutoRosterRulesDB::ReadDataFromDB()
 {
	 long nRecordCount = -1;
	 CADORecordset adoRecordset;
	 CString  strSelectSQL ;
	 strSelectSQL.Format(_T("SELECT * FROM TB_ROSTER_RULES WHERE TYID = %d") , TY) ;
	 try
	 {
		 CADODatabase::ExecuteSQLStatement(strSelectSQL, nRecordCount, adoRecordset,DATABASESOURCE_TYPE_ACCESS);
	 }
	 catch (CADOException e)
	 {
		 CString str = e.ErrorMessage() ;
		 return ;
	 }

	 CRosterRule* _rule = NULL ;
	 ProcessorID ID ;
	 while (!adoRecordset.IsEOF())
	 {
		 if(TY == TY_RULE)
			 _rule = new CRosterRule(GetInputTerminal()) ;
		 else
			 _rule = new CRosterRulesBySchedule(GetInputTerminal()) ;
		 ID = ReadRules(_rule,&adoRecordset) ;
		 AddRosterRules(_rule,ID) ;
		 adoRecordset.MoveNextData() ;
	 }
 }
 void  CAutoRosterRulesDB::WriteDataToDB()
 {
	 TCHAR procName[1024] = {0};
	 CProcessGroupRules *database;
	 CRosterRule* rosterRule = NULL ;
	 int entryCount = getCount();
	 const std::vector<CRosterRule*>* _delItem = NULL ;
	try
	{	
			CADODatabase::BeginTransaction() ;
		for (int i = 0; i < entryCount; i++)
		{
			database = getItem(i) ;
			database->GetProcessorId()->printID (procName);
			for (int j = 0; j < database->getCount(); j++)
			{
				rosterRule = database->getItem(j) ;
				writeRules(CString(procName),rosterRule) ;
			}
			_delItem = database->GetDelItem() ;
			for(int m = 0 ; m < (int)(_delItem->size()) ; m++)
			{
				rosterRule = (*_delItem)[m] ;
				DeleteRules(rosterRule) ;
			}
			database->clearDelete() ;
		}
		CADODatabase::CommitTransaction() ;
	}
	catch (CADOException e)
	{
		CADODatabase::RollBackTransation() ;
	}
}
 //////////////////////////////////////////////////////////////////////////
 //////////////////////////////////////////////////////////////////////////
 CAutoRosterByScheduleRulesDB::CAutoRosterByScheduleRulesDB(InputTerminal* input_ter):CAutoRosterRulesDB(input_ter)
 {
   TY = TY_SCHEDULE_RULE ;
 }
 void CAutoRosterByScheduleRulesDB::readData(ArctermFile& p_file)
 {
    ReadDataFromDB() ;
 }
 void CAutoRosterByScheduleRulesDB::writeData(ArctermFile& p_file)const
 {
   CAutoRosterByScheduleRulesDB* DB =  const_cast<CAutoRosterByScheduleRulesDB*>(this) ;
   DB->WriteDataToDB() ;
 }
 CAutoRosterByScheduleRulesDB::~CAutoRosterByScheduleRulesDB()
 {

 }
 CRosterRulesBySchedule::CRosterRulesBySchedule(InputTerminal* inputTer):CRosterRule(inputTer)
 {}
 CRosterRulesBySchedule::~CRosterRulesBySchedule()
 {}
 void CRosterRulesBySchedule::CreateProcessorRoster(ProcessorRoster& pro_Ass,Flight* _flight,int _mode)
 {
	 ElapsedTime _time ;
	 if(_flight == NULL)
		 return ;
	 if(_mode == 0)
		 _time = _flight->getArrTime() ;
	 if(_mode == 1)
		 _time = _flight->getDepTime() ;
	 CRosterRule::CreateProcessorRoster(pro_Ass) ;
	  ElapsedTime opentime;
	   ElapsedTime closetime ;
	  if(GetOpenIfAdvance() == BEFORE)
		  opentime = _time - GetOpenTime() ;
	  else
		  opentime = _time + GetOpenTime() ;
	  if(GetCloseIfAdvance() == BEFORE)
         closetime = _time - GetCloseTime() ;
	  else
		  closetime = _time + GetCloseTime() ;
	 pro_Ass.setOpenTime(opentime) ;
	 pro_Ass.setCloseTime(closetime) ;
	 pro_Ass.setAbsOpenTime(opentime) ;
	 pro_Ass.setAbsCloseTime(closetime) ;
	 pro_Ass.IsDaily(FALSE);

 }
 void CRosterRulesBySchedule::AddRosterToSchedule(ProcessorRosterSchedule* _schedule)
 {

	 FlightSchedule* pSchedule =  p_input->flightSchedule;
	 if(pSchedule == NULL)
		 return ;
	 for( int i=0; i<static_cast<int>(pSchedule->getCount()); i++ )
	 {
			 Flight* pFlight = pSchedule->getItem( i );
			if(pFlight->getFlightMode() == 'T')
				{
							 AddProcAssigment(_schedule,pFlight,'A') ;
							 AddProcAssigment(_schedule,pFlight,'D') ;
				}
			if(pFlight->getFlightMode() == 'D')
			{
                AddProcAssigment(_schedule,pFlight,'D') ;
			}
			if(pFlight->getFlightMode() == 'A')
			{
				 AddProcAssigment(_schedule,pFlight, 'A') ;
			}
	 }
 }
 void CRosterRulesBySchedule::AddProcAssigment(ProcessorRosterSchedule* _schedule,Flight* pFlight ,char _mode)
 {
   CMobileElemConstraint m_mobileType(p_input); 
   	 ProcessorRoster* _Ass = NULL ;
   if(pFlight == NULL)
	   return ;
   	 int mode = 0 ;
   (FlightConstraint&)m_mobileType = pFlight->getType( _mode);
   if( ((FlightConstraint*)(GetAssignment()->getConstraint(0)))->fits(m_mobileType) )
   {
	   _Ass = new ProcessorRoster ;
	   if(_mode == 'D')
		   mode = 1 ;
	   CreateProcessorRoster(*_Ass,pFlight,mode) ;
	   _schedule->addItem(_Ass) ;
   }
 }
