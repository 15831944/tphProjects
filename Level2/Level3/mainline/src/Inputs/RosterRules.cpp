#include "StdAfx.h"
#include ".\rosterrules.h"

CRosterRules::CRosterRules(InputTerminal* inputTer):openTime(0l),closeTime(0l),isDaily(TRUE),n_num(0),p_input(inputTer)
{
	assign.addConstraint(CMobileElemConstraint(inputTer)) ;
}
CRosterRules::CRosterRules(const CRosterRules& rule) 
{
	openTime = rule.openTime ;
	closeTime = rule.closeTime;
	this->isDaily= rule.isDaily;
	assign = rule.assign ;
	n_num = rule.n_num ;
}
// do not realize
void CRosterRules::ReadDataFromFile(ArctermFile& p_file, InputTerminal* _pInTerm )
{
  p_file.getInteger(n_num) ;
  p_file.getInteger(isDaily) ;
  p_file.getTime(openTime) ;
  p_file.getTime(closeTime) ;
  assign.readConstraints(p_file,_pInTerm) ;

}
void CRosterRules::WriteDataFromFile(ArctermFile& p_file)
{
 p_file.writeInt(n_num) ;
 p_file.writeInt(isDaily) ;
 p_file.writeTime(openTime) ;
 p_file.writeTime(closeTime) ;
 assign.writeConstraints(p_file) ;
}
CRosterRules::~CRosterRules(void)
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
   CRosterRules* rule = NULL ;
   for (int i = 0 ; i < getCount() ; i++)
   {
	   rule = getItem(i) ;
	   if(rule != NULL)
		   delete rule ;
   }
}
CProcessGroupRules::CProcessGroupRules(const CProcessGroupRules& grouprules) 
{
  clear() ;
  CRosterRules* rule = NULL ;
  for (int i = 0 ; i< grouprules.getCount() ; i++)
  {
      rule = new CRosterRules(*(grouprules.getItem(i))) ;
	  addItem(rule) ;
  }
  pro_ID = grouprules.pro_ID ;
}
int CProcessGroupRules::FindRosterRule(CRosterRules* rule)
{
	for (int i = 0 ; i < getCount() ;i++)
	{
		if(getItem(i) == rule)
			return i ;
	}
	return INT_MAX ;
}
CProcessGroupRules::~CProcessGroupRules()
{
	clear() ;
}
CAutoRosterRulesDB::CAutoRosterRulesDB(InputTerminal* input_ter):DataSet(RosterRulesFile)
{
 SetInputTerminal(input_ter) ;
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
 void CAutoRosterRulesDB::AddRosterRules(CRosterRules* rule,const ProcessorID& pro_id)
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
	 assert( m_pInTerm );
	 CRosterRules *rules = NULL; 
	 clear();
	 ProcessorIDList idList;
	 idList.ownsElements( 1 );//cause idList.readIDList (p_file, m_pInTerm ); will alloc memory 
	 ProcAssignment procAssign( m_pInTerm );
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
		 rules = new CRosterRules(GetInputTerminal()) ;
		rules->ReadDataFromFile(p_file,m_pInTerm) ;
		AddRosterRules(rules,*(idList.getID(0))) ;
	 }
 }
 void CAutoRosterRulesDB::writeData(ArctermFile& p_file)const
 {
	 char procName[256] = {0};
	 CProcessGroupRules *database;
	 CRosterRules* rosterRule = NULL ;
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