#include "StdAfx.h"
#include ".\sectordata.h"
CInterventionrequirement::CInterventionrequirement()
{
	wsprintf(p_code,_T("0%s0%s0%s0%s0"),CURRENT_CHAR,CURRENT_CHAR,CURRENT_CHAR,CURRENT_CHAR);
}


const TCHAR* CInterventionrequirement::getCode()
{
	return p_code ;
}

BOOL CInterventionrequirement::isSelected(int index)
{
	int res = 0 ;
	res = CParseStringToInt::getInt(index , p_code ,PARSE_CHAR) ;
   if(res == 1)
	   return  TRUE ;
   else
	   return FALSE ;
}
void CInterventionrequirement::setCode(const TCHAR* code)
{
	_tcscpy(p_code , code);
}
void CInterventionrequirement::setSelected(int index)
{
	CParseStringToInt::setInt(index,1,p_code,PARSE_CHAR) ;
}
void CInterventionrequirement::setNoSelected(int index)
{
	CParseStringToInt::setInt(index,0,p_code,PARSE_CHAR) ;
}
CHoldPriority::CHoldPriority():index(1)
{

}
void CHoldPriority::setSelected(int index)
{
	this->index = index ;
}
int CHoldPriority::getSelected()
{
	return index ;
}
CInterventionPriority::CInterventionPriority()
{
	_tcscpy(code,_T("0 1 2 3"));
}
int CInterventionPriority::getSelectedIndex(int priority)
{
	return CParseStringToInt::getInt(priority,code,PARSE_CHAR) ;
}
void CInterventionPriority::setPrioritySelect(int priority , int selected_index)
{
	CParseStringToInt::setInt(priority,selected_index,code,PARSE_CHAR) ;
}
void CInterventionPriority::setCode(const char* p_code)
{
	if(p_code != NULL)
		strcpy(code,p_code) ;
}
const TCHAR* CInterventionPriority::getCode()
{
	return code ;
}
CSectorData::CSectorData(void):adiusOfconcern(0) ,ID(-1),is_Modify(FALSE),fixID(0)
{

}
double  CSectorData::getAdiusOfconcern()
{
	return adiusOfconcern ;
}
CHoldPriority& CSectorData::getHoldPriority()
{
   return HoldPriority ;
}
CInterventionPriority& CSectorData::getInterventionPriority()
{
 return InterventionPriority ;
}
CInterventionrequirement& CSectorData::getInterventionRequirementSelect()
{
	    return requirement;
}

CString CSectorData::getName()
{
    return  name ;
}
void CSectorData::setAdiusOfconcern(double val)
{
	adiusOfconcern = val ;
}
void CSectorData::setHoldPriority(int index)
{
   HoldPriority.setSelected(index) ;
}
void CSectorData::setInterventionPriority(int priority ,int index)
{
   InterventionPriority.setPrioritySelect(priority,index) ;
}
void CSectorData::setInterventionPriorityCode(const TCHAR* p_code)
{
	InterventionPriority.setCode(p_code) ;
}
void CSectorData::setInterventionRequirementSelect(int index)
{
    requirement.setSelected(index) ;
}
void CSectorData::setInterventionRequirementNoSelect(int index )
{
   requirement.setNoSelected(index) ;
}
void CSectorData::setInterventionRequirementSelectCode(const TCHAR* code)
{
	requirement.setCode(code) ;
}
void CSectorData::setSectorName(const CString& name)
{
	this->name = name ;
}
void CSectorData::setFixID(int id)
{
	fixID = id ;
}
int CSectorData::getFixID()
{
	return fixID ;
}
void  CSectorData::setID(int id)
{
   ID = id ;
}
int CSectorData::getID()
{
	return ID ;
}
void CSectorData::modified()
{
	is_Modify = TRUE ;
}
void CSectorData::setNameID(int id)
{
	nameid = id ;
}
int CSectorData::getNameID()
{
	return nameid ;
}
void CSectorData::noModified()
{
	is_Modify = FALSE ;
}
BOOL  CSectorData::isModified()
{
    return is_Modify ;
}
CSectorData::~CSectorData(void)
{
}
int CParseStringToInt::getInt(int index , const TCHAR* p_string ,const TCHAR* p_strTok)
{
	TCHAR str_parse [1024] = {0};
	_tcscpy(str_parse , p_string) ;
	TCHAR* p_Item =  _tcstok(str_parse,p_strTok) ;
   int i = 0 ;
   for (;p_Item != NULL  ;p_Item = _tcstok(NULL , p_strTok) ,++i)
   {
       if(i == index && p_Item != NULL)
           return atoi(p_Item) ;
   }
   return -1 ;
}
int CParseStringToInt::setInt(int index ,int val , TCHAR* p_string ,const TCHAR* p_strTok )
{
	 CString formatStr ;
	 TCHAR* p_Item =  _tcstok(p_string,p_strTok) ;
 
	 for (int i = 0 ;p_Item != NULL  ;p_Item = _tcstok(NULL , p_strTok) ,++i)
	  {
		  if( p_Item != NULL)
		  {
			  if(i != 0)
				   formatStr.Append(p_strTok);
			  if( i == index )
				  formatStr.Append("%d");
			  else		      
				  formatStr.Append(p_Item) ; 
			 
		  }
		 
	  }
	  wsprintf(p_string,formatStr.GetString(),val) ;	
	  return val ;
}