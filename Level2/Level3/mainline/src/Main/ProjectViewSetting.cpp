#include "StdAfx.h"
#include ".\projectviewsetting.h"
#include "../Database/ADODatabase.h"

CProjectViewSetting::CProjectViewSetting(void):n_EnvironmentMode(0),n_simulation(0),id(-1),empty(true)
{
}

CProjectViewSetting::~CProjectViewSetting(void)
{
}
void CProjectViewSetting::setEnvironmentMode(int index)
{
	this->n_EnvironmentMode = index ;
}
void CProjectViewSetting::setSimulation(int index)
{
	this->n_simulation = index ;
}
int  CProjectViewSetting::getEnvironmentMode()
{
	return n_EnvironmentMode ;
}
int CProjectViewSetting::getSimulation()
{
	return n_simulation ;
}
BOOL CProjectViewSetting::IsEmpty() 
{
   return empty ;
}
void CProjectViewSetting::getDataFromDB()
{
	CString strSQL;
	strSQL.Format(_T("SELECT * FROM PROJECTUSERVIEWSETTING"));

	long lRecordCount = 0;
	CADORecordset adoRecordset;
	try{
	CADODatabase::ExecuteSQLStatement(strSQL,lRecordCount,adoRecordset);
	}catch (CADOException& e)
	{
		e.ErrorMessage() ;
	}
	if(adoRecordset.IsEOF())
			empty = TRUE ;
	while(!adoRecordset.IsEOF())
		{
			empty = FALSE ;
			adoRecordset.GetFieldValue(_T("ENVIROMENTMODE"),n_EnvironmentMode) ;

			adoRecordset.GetFieldValue(_T("SIMULATIONTYPE"),n_simulation);

            adoRecordset.GetFieldValue(_T("ID"),id);
			adoRecordset.MoveNextData();
		}
}
BOOL CProjectViewSetting::saveDataToDB()
{

     CString Str_SQL ;
	 try
	 {
		 CADODatabase::BeginTransaction();
		 if(id == -1 )
		 {
			 Str_SQL.Format(_T("INSERT INTO PROJECTUSERVIEWSETTING (ENVIROMENTMODE ,SIMULATIONTYPE) VALUES (%d ,%d )") , 
							   n_EnvironmentMode,
							   n_simulation
							   );
			 CADODatabase::ExecuteSQLStatement(Str_SQL) ;
		 }
		 else
		 {
			 Str_SQL.Format(_T("UPDATE PROJECTUSERVIEWSETTING\
							   SET ENVIROMENTMODE = %d,\
							   SIMULATIONTYPE = %d \
							   WHERE ID = %d") ,
							   n_EnvironmentMode,
							   n_simulation,
							   id
							   ) ;        
			 CADODatabase::ExecuteSQLStatement(Str_SQL) ;
		 }
     CADODatabase::CommitTransaction() ;
	 }
	 catch (CADOException& e)
	 {
		 e.ErrorMessage() ;
		 CADODatabase::RollBackTransation() ;
		 return FALSE ;
	 }
     return true ;


   
}
void CProjectViewSetting::Empty()
{
	id = -1 ;
}