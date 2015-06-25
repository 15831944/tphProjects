// Sector.cpp: implementation of the CSector class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Sector.h"
#include "AirportsManager.h"
#include "template.h"
#include "assert.h"
#include "../Database/ADODatabase.h"
#include "../Database/ARCportADODatabase.h"
#include "AirportDatabase.h"
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSector::CSector( CAirportsManager* _pAirportMan  ) : m_pAirportMan( _pAirportMan ),m_ID(-1)
{
}

CSector::~CSector()
{
}


void AddToList(CAIRPORTLIST& APList, CAirport* pAP)
{
	for(unsigned i=0; i<APList.size(); i++) {
		if(APList[i]->m_sIATACode.CompareNoCase(pAP->m_sIATACode) == 0) { //found the airport
			return;
		}
	}
	APList.push_back(pAP);
}

void CSector::GetAirportList(CAIRPORTLIST& APList)
{
	for(unsigned i=0; i<m_vAirportFilters.size(); i++) {
		CAIRPORTFILTER airportFilter = m_vAirportFilters[i];
		CAIRPORTLIST TempList;
		assert( m_pAirportMan );
		m_pAirportMan->GetAirportsByFilter(airportFilter, TempList);
		for(unsigned j=0; j<TempList.size(); j++)
			AddToList(APList, TempList[j]);
		TempList.clear();
	}
}

void CSector::AddFilter(CAIRPORTFILTER airportFilter)
{
	m_vAirportFilters.push_back(airportFilter);
}


int CSector::contains (const char *p_airport) const
{
    char str[AIRPORT_LEN];
	CString strCode;
    for (unsigned i = 0; i < m_vAirportFilters.size(); i++)
    {
		strCode = m_vAirportFilters[i].sIATACode;
		if (strCode.GetLength() >= AIRPORT_LEN)
			strcpy( str, strCode.Left(AIRPORT_LEN -1));
		else
			strcpy( str, /*m_vAirportFilters[i].sIATACode*/strCode);
        
		// replace wildcards
        for (int j = 0; str[j] && p_airport[j]; j++)
        {
            if (str[j] == '?')
                str[j] = p_airport[j];
            else if (str[j] == '*')
                strcpy (str + j, p_airport + j);
        }
        if( !_strcmpi (p_airport, str) )
            return 1;
    }
    return 0;
}

void CSector::setCode( const char *p_str )
{ 
	m_sName = p_str; 
}

/////////new version database read and save/////////////////////////////////////////////////
bool CSector::loadDatabase(CADORecordset& aodRecordset,CAirportDatabase* pAirportDatabase)
{
	ASSERT(pAirportDatabase);
	CString strValue(_T(""));
	int nID = -1;

	if (!aodRecordset.IsEOF())
	{
		aodRecordset.GetFieldValue(_T("SECTOR_NAME"),strValue);
		m_sName = strValue ;

		aodRecordset.GetFieldValue(_T("ID"),nID);
		m_ID = nID;

		loadSectorFilter(pAirportDatabase);
	}
	return true;
}

bool CSector::loadSectorFilter(CAirportDatabase* pAirportDatabase)
{
	ASSERT(pAirportDatabase);

	CString strSQL(_T(""));
	strSQL.Format(_T("SELECT * FROM TB_AIRPORTDB_SECTOR_FILTER WHERE SECTOR_ID = %d"),m_ID);

	CADORecordset adoRecordset;
	long lCount = 0;
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
	while (!adoRecordset.IsEOF())
	{
		CAIRPORTFILTER filter;
		adoRecordset.GetFieldValue(_T("FILTER_NAME"),strValue);
		filter.sIATACode = strValue;
		AddFilter(filter);
		adoRecordset.MoveNextData();
	}
	return true;
}
bool CSector::saveDatabase(CAirportDatabase* pAirportDatabase)
{
	ASSERT(pAirportDatabase);
	int nAirportID = pAirportDatabase->GetID();
	if (nAirportID == -1)
	{
		return false;
	}

	if (m_ID == -1)
	{
		CString strSQL(_T(""));
		strSQL.Format(_T("INSERT INTO TB_AIRPORTDB_SECTOR (SECTOR_NAME,AIRPORTDB_ID) VALUES('%s',%d)"),m_sName,nAirportID);
		m_ID = CDatabaseADOConnetion::ExecuteSQLStatementAndReturnScopeID(strSQL,pAirportDatabase->GetAirportConnection());
	}
	else
	{
		updateDatabase(pAirportDatabase);
	}

	saveSectorFilter(pAirportDatabase);
	return true;
}

bool CSector::deleteAllDatabase(CAirportDatabase* pAirportDatabase)
{
	CString strSQL(_T(""));
	strSQL.Format(_T("DELETE * FROM TB_AIRPORTDB_SECTOR WHERE AIRPORTDB_ID = %d"),pAirportDatabase->GetID());
	CDatabaseADOConnetion::ExecuteSQLStatement(strSQL,pAirportDatabase->GetAirportConnection());
	return true;
}

bool CSector::deleteDatabase(CAirportDatabase* pAirportDatabase)
{
	CString strSQL(_T(""));
	strSQL.Format(_T("DELETE * FROM TB_AIRPORTDB_SECTOR WHERE ID = %d"),m_ID);
	CDatabaseADOConnetion::ExecuteSQLStatement(strSQL,pAirportDatabase->GetAirportConnection());
	return true;
}


bool CSector::deleteSectorFilter(CAirportDatabase* pAirportDatabase)
{
	ASSERT(pAirportDatabase);

	CString strSQL(_T(""));
	strSQL.Format(_T("DELETE * FROM TB_AIRPORTDB_SECTOR_FILTER WHERE  SECTOR_ID = %d"),m_ID);
	CDatabaseADOConnetion::ExecuteSQLStatement(strSQL,pAirportDatabase->GetAirportConnection());
	return true;
}

bool CSector::updateDatabase(CAirportDatabase* pAirportDatabase)
{
	ASSERT(pAirportDatabase);
	int nAirportID = pAirportDatabase->GetID();

	CString strSQL(_T(""));
	strSQL.Format(_T("UPDATE TB_AIRPORTDB_SECTOR SET SECTOR_NAME = '%s' , AIRPORTDB_ID = %d WHERE ID = %d"),m_sName,nAirportID,m_ID);
	CDatabaseADOConnetion::ExecuteSQLStatement(strSQL,pAirportDatabase->GetAirportConnection());
	return true;
}

bool CSector::saveSectorFilter(CAirportDatabase* pAirportDatabase)
{
	ASSERT(pAirportDatabase);
	deleteSectorFilter(pAirportDatabase);

	std::vector<CAIRPORTFILTER>::iterator iter = m_vAirportFilters.begin();
	for (; iter != m_vAirportFilters.end(); iter++)
	{
		CString strSQL(_T(""));
		strSQL.Format(_T("INSERT INTO TB_AIRPORTDB_SECTOR_FILTER (SECTOR_ID,FILTER_NAME) VALUES(%d,'%s')"),m_ID,(*iter).sIATACode);
		CDatabaseADOConnetion::ExecuteSQLStatementAndReturnScopeID(strSQL,pAirportDatabase->GetAirportConnection());
	}
	return true;
}
////////////////////old code, no more use///////////////////////////////////////////////////
void CSector::ReadDataFromDB(CSECTORLIST& _dataSet ,CAirportsManager* _pAirportMan , int _airportDBID,DATABASESOURCE_TYPE type)
{
	if(_pAirportMan == NULL || _airportDBID == -1)
		return ;
	CString SQL ;
	SQL.Format(_T("SELECT * FROM TB_AIRPORTDB_SECTOR WHERE AIRPORTDB_ID = %d"),_airportDBID) ;
	CADORecordset recordset ;
	long count ;
	try
	{
		CADODatabase::ExecuteSQLStatement(SQL,count,recordset,type) ;
	}
	catch (CADOException e)
	{
		e.ErrorMessage();
		return ;
	}
	CString str ;
	int id = -1 ;
	while(!recordset.IsEOF())
	{
       CSector* sector = new CSector(_pAirportMan) ;

	   recordset.GetFieldValue(_T("SECTOR_NAME"),str) ;
	   sector->m_sName = str ;

	   recordset.GetFieldValue(_T("ID"),id) ;
	   sector->SetID(id) ;

	   CSector::ReadSectorData(sector,type) ;

	   _dataSet.push_back(sector) ;
	   
	   recordset.MoveNextData() ;
	}

}
void CSector::WriteDataToDB(CSECTORLIST& _dataSet ,int _airportDBID,DATABASESOURCE_TYPE type)
{
   if(_airportDBID == -1)
	   return ;
   CSECTORLIST::iterator iter = _dataSet.begin() ;
   for ( ; iter != _dataSet.end() ;iter++)
   {
	   if((*iter)->GetID() == -1)
	       CSector::WriteData(*iter,_airportDBID,type) ;
	   else
		   CSector::UpdateData(*iter,_airportDBID,type) ;
	   CSector::WriteSectorData(*iter,type) ;
   }
}
void CSector::WriteData(CSector* _sector,int _airportDBID,DATABASESOURCE_TYPE type)
{
	if(_sector == NULL)
		return ;
	CString SQL ;
	SQL.Format(_T("INSERT INTO TB_AIRPORTDB_SECTOR (SECTOR_NAME,AIRPORTDB_ID) VALUES('%s',%d)"),_sector->m_sName,_airportDBID) ;
		int id = CADODatabase::ExecuteSQLStatementAndReturnScopeID(SQL,type) ;
		_sector->SetID(id) ;
}
void CSector::UpdateData(CSector* _sector,int _airportDBID,DATABASESOURCE_TYPE type)
{
	if(_sector == NULL || _airportDBID == -1)
	   return ;
	CString SQL ;
	SQL.Format(_T("UPDATE TB_AIRPORTDB_SECTOR SET SECTOR_NAME = '%s' , AIRPORTDB_ID = %d WHERE ID = %d"),_sector->m_sName,_airportDBID,_sector->GetID()) ;
		CADODatabase::ExecuteSQLStatement(SQL,type) ;
}
void CSector::DeleteDataFromDB(CSECTORLIST& _dataSet,DATABASESOURCE_TYPE type)
{
	CSECTORLIST::iterator iter = _dataSet.begin() ;
	for ( ; iter != _dataSet.end() ;iter++)
	{
		CSector::DeleteSector(*iter,type) ;
	}
}
void CSector::DeleteSector(CSector* _sector,DATABASESOURCE_TYPE type)
{
	if(_sector == NULL || _sector->GetID() == -1)
		return ;
	CString SQL ;
	SQL.Format(_T("DELETE * FROM TB_AIRPORTDB_SECTOR WHERE ID = %d"),_sector->GetID()) ;
		CADODatabase::ExecuteSQLStatement(SQL,type) ;
}
void CSector::DeleteAllDataFromDB(int _airportDBID,DATABASESOURCE_TYPE type)
{
	CString SQL ;
	SQL.Format(_T("DELETE * FROM TB_AIRPORTDB_SECTOR WHERE AIRPORTDB_ID = %d"),_airportDBID) ;
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
void CSector::ReadSectorData(CSector* _sector,DATABASESOURCE_TYPE type)
{
	if(_sector == NULL || _sector->GetID() == -1)
		return ;
     CString SQL ;
	 SQL.Format(_T("SELECT * FROM TB_AIRPORTDB_SECTOR_FILTER WHERE SECTOR_ID = %d"),_sector->GetID()) ;
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
	CAIRPORTFILTER filter ;
	while (!recordset.IsEOF())
	{
        recordset.GetFieldValue(_T("FILTER_NAME"),str) ;
		filter.sIATACode = str ;
		_sector->AddFilter(filter) ;
		recordset.MoveNextData() ;
	}
}
void CSector::WriteSectorData(CSector* _sector,DATABASESOURCE_TYPE type)
{
	if(_sector == NULL || _sector->GetID() == -1)
		return ;
	CString SQL ;
	int id = -1 ;
	SQL.Format(_T("DELETE * FROM TB_AIRPORTDB_SECTOR_FILTER WHERE  SECTOR_ID = %d"),_sector->GetID()) ;
		CADODatabase::ExecuteSQLStatement(SQL,type) ;
		std::vector<CAIRPORTFILTER>::iterator iter = _sector->m_vAirportFilters.begin() ;
		for ( ; iter != _sector->m_vAirportFilters.end() ;iter++)
		{
			CString SQL ;
            SQL.Format(_T("INSERT INTO TB_AIRPORTDB_SECTOR_FILTER (SECTOR_ID,FILTER_NAME) VALUES(%d,'%s')") ,_sector->GetID() ,(*iter).sIATACode) ;
			id = CADODatabase::ExecuteSQLStatementAndReturnScopeID(SQL,type) ;
		}
}