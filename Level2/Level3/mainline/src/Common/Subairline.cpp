// Subairline.cpp: implementation of the CSubairline class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Subairline.h"
#include "AirlineManager.h"
#include "template.h"
#include "assert.h"
#include "../Database/ADODatabase.h"
#include <algorithm>
#include "AirportDatabase.h"
#include "../Database/ARCportADODatabase.h"
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSubairline::CSubairline( CAirlinesManager* _pAirlineMan) :m_pAirlineMan( _pAirlineMan ),m_ID(-1)
{

}

CSubairline::~CSubairline()
{

}

void AddToList(CAIRLINELIST& ALList, CAirline* pAL)
{
	for(unsigned i=0; i<ALList.size(); i++) {
		if(ALList[i]->m_sAirlineIATACode.CompareNoCase(pAL->m_sAirlineIATACode) == 0) { //found the airline
			return;
		}
	}
	ALList.push_back(pAL);
}

void CSubairline::GetAirlineList(CAIRLINELIST& ALList)
{
	for(unsigned i=0; i<m_vAirlineFilters.size(); i++) {
		CAIRLINEFILTER airlineFilter = m_vAirlineFilters[i];
		CAIRLINELIST TempList;
		assert( m_pAirlineMan );
		m_pAirlineMan->GetAirlinesByFilter(airlineFilter, TempList);
		for(unsigned j=0; j<TempList.size(); j++)
			AddToList(ALList, TempList[j]);
		TempList.clear();
	}
}

void CSubairline::AddFilter(CAIRLINEFILTER airlineFilter)
{
	if(std::find(m_vAirlineFilters.begin() ,m_vAirlineFilters.end() ,airlineFilter) == m_vAirlineFilters.end())
		m_vAirlineFilters.push_back(airlineFilter);
}


int CSubairline::contains (const char *p_airline) const
{
    char str[AIRLINE_LEN];
    for (unsigned i = 0; i < m_vAirlineFilters.size(); i++)
    {
		strcpy( str, m_vAirlineFilters[i].code );
        
		// replace wildcards
        for (int j = 0; str[j] && p_airline[j]; j++)
        {
            if (str[j] == '?')
                str[j] = p_airline[j];
            else if (str[j] == '*')
			{
                strcpy (str + j, p_airline + j);
				break;
			}
        }
        if( !_strcmpi (p_airline, str) )
            return 1;
    }
    return 0;
}

void CSubairline::setCode( const char *p_str )
{ 
	m_sName = p_str; 
}

////////////////new version database read and save/////////////////////////////////////////
bool CSubairline::loadDatabase(CADORecordset& aodRecordset,CAirportDatabase* pAirportDatabase)
{
	ASSERT(pAirportDatabase);
	if (!aodRecordset.IsEOF())
	{
		aodRecordset.GetFieldValue(_T("SUB_NAME"),m_sName);
		aodRecordset.GetFieldValue(_T("ID"),m_ID);

		loadAirlineFilter(pAirportDatabase);
	}
	return true;
}

bool CSubairline::saveDatabase(CAirportDatabase* pAirportDatabase)
{
	ASSERT(pAirportDatabase);

	m_sName.Replace("\'" ,"\'\'");

	if (m_ID == -1)
	{
		CString strSQL(_T(""));
		strSQL.Format(_T("INSERT INTO TB_AIRPORTDB_SUBAIRLINE (SUB_NAME) VALUES('%s')") ,m_sName);
		m_ID = CDatabaseADOConnetion::ExecuteSQLStatementAndReturnScopeID(strSQL,pAirportDatabase->GetAirportConnection());
	}
	else
	{
		updateDatabase(pAirportDatabase);
	}

	saveAirlineFilter(pAirportDatabase);

	return true;
}

bool CSubairline::updateDatabase(CAirportDatabase* pAirportDatabase)
{
	ASSERT(pAirportDatabase);

	CString strSQL(_T(""));
	strSQL.Format(_T("UPDATE TB_AIRPORTDB_SUBAIRLINE SET SUB_NAME = '%s' WHERE ID = %d "),m_sName,m_ID);
	CDatabaseADOConnetion::ExecuteSQLStatement(strSQL,pAirportDatabase->GetAirportConnection());
	return true;
}

bool CSubairline::deleteAllDatabase(CAirportDatabase* pAirportDatabase)
{
	ASSERT(pAirportDatabase);
	int nAirportID = pAirportDatabase->GetID();

	CString strSQL(_T(""));
	strSQL.Format(_T("DELETE * FROM TB_AIRPORTDB_SUBAIRLINE"));
	try
	{
		CDatabaseADOConnetion::ExecuteSQLStatement(strSQL,pAirportDatabase->GetAirportConnection());
	}
	catch (CADOException& e)
	{
		e.ErrorMessage();
		return false;
	}
	return true;
}

bool CSubairline::deleteDatabase(CAirportDatabase* pAirportDatabase)
{
	CString strSQL(_T(""));
	strSQL.Format(_T("DELETE * FROM TB_AIRPORTDB_SUBAIRLINE WHERE ID = %d"),m_ID);
	CDatabaseADOConnetion::ExecuteSQLStatement(strSQL,pAirportDatabase->GetAirportConnection());
	return true;
}

bool CSubairline::loadAirlineFilter(CAirportDatabase* pAirportDatabase)
{
	CString strSQL(_T(""));
	strSQL.Format(_T("SELECT * FROM TB_AIRPORTDB_SUBAIRLINE_FILTER WHERE SUBAIRLINE_ID = %d "),m_ID);
	CADORecordset adoRecordset;
	long lCount = 0 ;
	try
	{
		CDatabaseADOConnetion::ExecuteSQLStatement(strSQL,lCount,adoRecordset,pAirportDatabase->GetAirportConnection());
	}
	catch (CADOException& e)
	{
		e.ErrorMessage();
		return false;
	}

	CString strValue(_T(""));

	while(!adoRecordset.IsEOF())
	{
		CAIRLINEFILTER filter;
		adoRecordset.GetFieldValue(_T("FILTER_NAME"),strValue);
		filter.code = strValue;
		AddFilter(filter);
		adoRecordset.MoveNextData();
	}
	return true;
}

bool CSubairline::saveAirlineFilter(CAirportDatabase* pAirportDatabase)
{
	deleteAirlineFilter(pAirportDatabase);

	CString strSQL(_T(""));
	std::vector<CAIRLINEFILTER>::iterator iter = m_vAirlineFilters.begin();
	for (; iter != m_vAirlineFilters.end(); iter++)
	{
		strSQL.Format(_T("INSERT INTO TB_AIRPORTDB_SUBAIRLINE_FILTER (SUBAIRLINE_ID,FILTER_NAME) VALUES(%d,'%s')"),m_ID,(*iter).code);
		CDatabaseADOConnetion::ExecuteSQLStatement(strSQL,pAirportDatabase->GetAirportConnection());
	}
	return true;
}

bool CSubairline::deleteAirlineFilter(CAirportDatabase* pAirportDatabase)
{
	CString strSQL(_T(""));
	strSQL.Format(_T("DELETE * FROM TB_AIRPORTDB_SUBAIRLINE_FILTER WHERE SUBAIRLINE_ID = %d") , m_ID);
	CDatabaseADOConnetion::ExecuteSQLStatement(strSQL,pAirportDatabase->GetAirportConnection());
	return true;
}
///////old code, no more use////////////////////////////////////////////////////////////////
void CSubairline::ReadDataFromDB(CSUBAIRLINELIST& _DataSet,CAirlinesManager* _AirlineManger,int _airportDBID,DATABASESOURCE_TYPE type)
{
	if(_AirlineManger == NULL || _airportDBID == -1)
		return ;
    CString SQL ;
	SQL.Format(_T("SELECT * FROM TB_AIRPORTDB_SUBAIRLINE WHERE AIRPORTDB_ID = %d") ,_airportDBID) ;
	CADORecordset recordset ;
	long count = 0 ;
	try
	{
		CADODatabase::ExecuteSQLStatement(SQL,count,recordset,type) ; 
	}
	catch (CADOException e)
	{
		e.ErrorMessage() ;
		return ;
	}
	CString name ;
	int id = -1 ;
	while (!recordset.IsEOF())
	{
		CSubairline* subline = new CSubairline(_AirlineManger) ;
       recordset.GetFieldValue(_T("SUB_NAME"),name) ;
	   subline->m_sName = name ;

	   recordset.GetFieldValue(_T("ID"),id) ;
	   subline->SetID(id) ;
        
	   CSubairline::ReadSublineFilterData(subline,type) ;
       
	   _DataSet.push_back(subline) ;
	   recordset.MoveNextData() ;
	}
}
void CSubairline::WriteDataToDB(CSUBAIRLINELIST& _DataSet,int _airportDBID,DATABASESOURCE_TYPE type)
{
  if(_airportDBID == -1)
	  return ;
  CSUBAIRLINELIST::iterator iter = _DataSet.begin() ;
  for ( ; iter != _DataSet.end() ; iter++)
  {
	 (*iter)->m_sName.Replace("\'" ,"\'\'");
	  if((*iter)->GetID()  != -1)
	    CSubairline::UpdateSubline(*iter,_airportDBID,type) ;
	  else
		CSubairline::WriteSubline(*iter,_airportDBID,type) ;
	  CSubairline::WriteSublineFilterData(*iter,type) ;
  }
}
void CSubairline::DeleteDataFromDB(CSUBAIRLINELIST& _DataSet,DATABASESOURCE_TYPE type)
{
	CSUBAIRLINELIST::iterator iter = _DataSet.begin() ;
	for ( ; iter != _DataSet.end() ;iter++)
	{
		CSubairline::DeleteSubline(*iter,type) ;
	}
}
void CSubairline::WriteSubline(CSubairline* subline,int _airportDBID,DATABASESOURCE_TYPE type)
{
   if(subline == NULL)
	   return ;
   CString SQL ;
   SQL.Format(_T("INSERT INTO TB_AIRPORTDB_SUBAIRLINE (SUB_NAME,AIRPORTDB_ID) VALUES('%s',%d)") ,subline->m_sName,_airportDBID) ;

     int id =   CADODatabase::ExecuteSQLStatementAndReturnScopeID(SQL,type) ;
	 subline->SetID(id) ;
}
void CSubairline::UpdateSubline(CSubairline* subline,int _airportDBID,DATABASESOURCE_TYPE type)
{
	if(subline == NULL)
		return ;
	CString SQL ;
	SQL.Format(_T("UPDATE TB_AIRPORTDB_SUBAIRLINE SET SUB_NAME = '%s',AIRPORTDB_ID = %d WHERE ID = %d ") ,subline->m_sName,_airportDBID,subline->GetID()) ;

	CADODatabase::ExecuteSQLStatement(SQL,type) ;
}
void CSubairline::DeleteSubline(CSubairline* subline,DATABASESOURCE_TYPE type)
{
	if(subline == NULL || subline->GetID() == -1)
		return ;
	CString SQL ;
	SQL.Format(_T("DELETE * FROM TB_AIRPORTDB_SUBAIRLINE WHERE ID = %d") ,subline->GetID()) ;
	CADODatabase::ExecuteSQLStatement(SQL,type) ;
}
void CSubairline::DeleteAllData(int _airportDBID,DATABASESOURCE_TYPE type)
{
	CString SQL ;
	SQL.Format(_T("DELETE * FROM TB_AIRPORTDB_SUBAIRLINE WHERE AIRPORTDB_ID = %d") ,_airportDBID) ;
	try
	{
		CADODatabase::BeginTransaction(type) ;
		CADODatabase::ExecuteSQLStatement(SQL,type) ;
		CADODatabase::CommitTransaction(type) ;
	}
	catch (CADOException e)
	{
		e.ErrorMessage() ;
		CADODatabase::RollBackTransation(type) ;
		return ;
	}
}
void CSubairline::WriteSublineFilterData(CSubairline* subline,DATABASESOURCE_TYPE type)
{
	if(subline == NULL)
		return ;
	CString SQL ;
	SQL.Format(_T("DELETE * FROM TB_AIRPORTDB_SUBAIRLINE_FILTER WHERE SUBAIRLINE_ID = %d") , subline->GetID()) ;
	CADODatabase::ExecuteSQLStatement(SQL,type) ;
	std::vector<CAIRLINEFILTER>::iterator iter = subline->m_vAirlineFilters.begin() ;
   for ( ; iter != subline->m_vAirlineFilters.end() ;iter++)
   {
       SQL.Format(_T("INSERT INTO TB_AIRPORTDB_SUBAIRLINE_FILTER (SUBAIRLINE_ID,FILTER_NAME) VALUES(%d,'%s')"),subline->GetID(),(*iter).code) ;
	  CADODatabase::ExecuteSQLStatement(SQL,type) ;
   }
}
void CSubairline::ReadSublineFilterData(CSubairline* subline ,DATABASESOURCE_TYPE type)
{
	if(subline == NULL || subline->GetID() == -1)
		return ;
    CString SQL ;
	SQL.Format(_T("SELECT * FROM TB_AIRPORTDB_SUBAIRLINE_FILTER WHERE SUBAIRLINE_ID = %d "),subline->GetID()) ;
	CADORecordset recordset ;
	long count = 0 ;
	try
	{
		CADODatabase::ExecuteSQLStatement(SQL,count,recordset,type) ;
	}
	catch (CADOException e)
	{
		e.ErrorMessage() ;
		return ;
	}
	CString str ;
	
	while(!recordset.IsEOF())
	{
		CAIRLINEFILTER filter ;
        recordset.GetFieldValue(_T("FILTER_NAME"),str) ;
		filter.code = str ;
		subline->AddFilter(filter) ;
		recordset.MoveNextData() ;
	}
}
