#include "stdafx.h"
#include "ControllerInterventionData.h"
#include "../Database/ADODatabase.h"
#include "../InputAirside/ALTObject.h"
#include <algorithm>
 const char* CControllerInterventionData::HOlD_Priority[2] = {"At Fix" ,"At arbitrary position on route"} ;
 int CControllerInterventionData::n_holdpriority = 2 ;
 const char* CControllerInterventionData::Priority_item[4] = {"Speed control","Vector","Hold","Altitude change"}  ;
 int CControllerInterventionData::n_priorityitemSize = 4 ;

int CControllerInterventionData::getHoldPrioritysize()
 {
   return n_holdpriority ;
 }
 int CControllerInterventionData::getPriorityitemSize()
 {
	 return n_priorityitemSize ;
 }
 CControllerInterventionData::CControllerInterventionData(int projID):m_nProjID(projID)
{

}
CControllerInterventionData::CControllerInterventionData()
{

}
void CControllerInterventionData::setProjID(int projID )
{
	m_nProjID = projID ;
}
void  CControllerInterventionData::AddSector( CSectorData* P_data)
{
   if(P_data != NULL)
	   sector.push_back(P_data);
   return  ;
}
CSectorData* CControllerInterventionData::createNewSector()
{

	CSectorData* handle = new CSectorData;
	AddSector(handle);
    return handle ;
}
void CControllerInterventionData::DelSector( CSectorData* P_data)
{

	if(P_data != NULL)
	{
		ITERATOR_SECTOR iter = find(sector.begin(),sector.end(),P_data) ;
		if(iter != sector.end())
		{
		   sector.erase(std::find(sector.begin(),sector.end(),P_data))  ;
		   del_sector.push_back(P_data) ;
		}
	}
}
void CControllerInterventionData::DelSectorToDB()
{
	ITERATOR_SECTOR iter = del_sector.begin() ;
	for ( ;iter != del_sector.end() ;++iter)
	{
		if((*iter) != NULL)
		{
			if((*iter)->getID() != -1 )
			{
				CString strSQL;
				strSQL.Format(_T("DELETE FROM SECTORCONTROLLERINTERVENTION \
								 WHERE ID = %d "), (*iter)->getID() );
				CADODatabase::ExecuteSQLStatement(strSQL) ;
				delete (*iter) ;
			}
		}
	}
}
const CControllerInterventionData::SECTOR_VECTOR& CControllerInterventionData::getSectors()
{
	getDataFromDB();
    return  sector ;
}
CControllerInterventionData::~CControllerInterventionData()
{
   ITERATOR_SECTOR iter = sector.begin() ;
   for(;iter != sector.end();++iter)
   {
	   if((*iter) != NULL)
		   delete  *iter ; 
   }
}
BOOL CControllerInterventionData::getDataFromDB()
{
	//CADODatabase::ExecuteSQLStatement()
	CString strSQL;
	strSQL.Format(_T("SELECT * FROM SECTORCONTROLLERINTERVENTION"),m_nProjID);
    
	long lRecordCount = 0;
	CADORecordset adoRecordset;
	CADODatabase::ExecuteSQLStatement(strSQL,lRecordCount,adoRecordset);
// 	if(lRecordCount != 0)
// 	{
		while(!adoRecordset.IsEOF())
		{
			CString val ;
			int n_val = 0 ;
			double D_val = 0 ;

			CSectorData* p_sector = createNewSector() ;
			adoRecordset.GetFieldValue(_T("ID"),n_val) ;
			p_sector->setID(n_val) ;

			adoRecordset.GetFieldValue(_T("SECTORID"),n_val);
			p_sector->setSectorName( ALTObject::ReadObjectByID(n_val)->GetObjNameString() ) ;
			p_sector->setNameID(n_val) ;

			adoRecordset.GetFieldValue(_T("REQUIREMENTID"),val);
			p_sector->setInterventionRequirementSelectCode(val.GetString()) ;
	        

			adoRecordset.GetFieldValue(_T("RADIUS"),D_val) ;
			p_sector->setAdiusOfconcern(D_val) ;
	        

			adoRecordset.GetFieldValue(_T("HOLDPRIORITYID"),n_val);
			p_sector->setHoldPriority(n_val) ;

			//set fix ,no implement now 
			adoRecordset.GetFieldValue(_T("FIXSELECTEDID"),n_val) ;
			p_sector->setFixID(n_val) ;
			//
			adoRecordset.GetFieldValue(_T("MEHHODSANDPRIORITYID"),val) ;
			p_sector->setInterventionPriorityCode(val.GetString()) ;
			adoRecordset.MoveNextData();
		}
	//}
		return TRUE ;
}
BOOL CControllerInterventionData::saveDataToDB()
{
   ITERATOR_SECTOR iter = sector.begin() ;
   CString Str_SQL  ;
	   for ( ; iter != sector.end() ;++iter)
	   {
		   if((*iter)->getID() == -1 )
		   {
			   Str_SQL.Format(_T("INSERT INTO SECTORCONTROLLERINTERVENTION\
								 (SECTORID ,REQUIREMENTID,RADIUS ,HOLDPRIORITYID ,FIXSELECTEDID ,MEHHODSANDPRIORITYID)\
								 VALUES (%d ,'%s' , %.2f ,%d ,%d ,'%s')" ) , 
								 (*iter)->getNameID(),
								 (*iter)->getInterventionRequirementSelect().getCode(),
								 (*iter)->getAdiusOfconcern(),
								 (*iter)->getHoldPriority().getSelected() ,
								 (*iter)->getFixID(),
								 (*iter)->getInterventionPriority().getCode()
								 );
			   CADODatabase::ExecuteSQLStatement(Str_SQL) ;
		   }
		   else
		   {
			   if( (*iter)->isModified() )
			   {
				   Str_SQL.Format(_T("UPDATE SECTORCONTROLLERINTERVENTION\
									 SET SECTORID = %d,REQUIREMENTID = '%s',RADIUS = %.2f ,HOLDPRIORITYID = %d,\
									 FIXSELECTEDID = %d ,MEHHODSANDPRIORITYID = '%s'\
									 WHERE ID = %d") ,
									 (*iter)->getNameID(),
									 (*iter)->getInterventionRequirementSelect().getCode(),
									 (*iter)->getAdiusOfconcern(),
									 (*iter)->getHoldPriority().getSelected() ,
									 (*iter)->getFixID(),
									 (*iter)->getInterventionPriority().getCode(),
									 (*iter)->getID()
									 ) ;        
				   CADODatabase::ExecuteSQLStatement(Str_SQL) ;
			   }
		   }
	   }
    return TRUE ;
}
std::vector<Item>  CControllerInterventionData::getFixName()
{
	std::vector<ALTObject>  fix_vector ;
	FIX_VECTOR Item_vector ;
	ALTObject::GetObjectList(ALT_WAYPOINT,m_nProjID,fix_vector);
	std::vector<ALTObject>::iterator iter = fix_vector.begin() ;
	for (  ;iter != fix_vector.end() ;++iter)
	{
           Item item ;
		   item.id = (*iter).getID() ;
		   item.name = (*iter).GetObjNameString() ;
		   Item_vector.push_back(item) ;
	}
	return Item_vector ;
}