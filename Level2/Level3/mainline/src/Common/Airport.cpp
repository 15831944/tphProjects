// Airport.cpp: implementation of the CAirport class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "Airport.h"
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

CAirport::CAirport():m_ID(-1),m_HasEdit(FALSE)
,m_bNeedUpdate(true)
{

}

CAirport::CAirport(const CAirport& airport)
{
	m_sIATACode = airport.m_sIATACode;
	m_sLongName = airport.m_sLongName;
	m_sICAOCode = airport.m_sICAOCode;
	m_sARPCoordinates = airport.m_sARPCoordinates;
	m_sCity = airport.m_sCity;
	m_sCountry = airport.m_sCountry;
	m_sCountryCode = airport.m_sCountryCode;
	m_sElevation = airport.m_sElevation;
	m_sNumberOfRunways = airport.m_sNumberOfRunways;
	m_sMaximumRunwayLength = airport.m_sMaximumRunwayLength;
	m_sAlternateAirportICAOCode = airport.m_sAlternateAirportICAOCode;
	m_HasEdit = airport.m_HasEdit;
	m_bNeedUpdate = airport.m_bNeedUpdate;
}

CAirport::~CAirport()
{

}

IMPLEMENT_SERIAL(CAirport, CObject, 1);

void CAirport::Serialize(CArchive& ar)
{
	CObject::Serialize(ar);
	if(ar.IsStoring()) {
		ar << m_sIATACode;
		ar << m_sLongName;
	}
	else {
		ar >> m_sIATACode;
		ar >> m_sLongName;
	}
}
///new version read and save////////////////////////////////////////////////////////////////
bool CAirport::loadDatabase(CADORecordset& aodRecordset)
{
	CString strValue(_T(""));
	int nID = -1;

	if (!aodRecordset.IsEOF())
	{
		aodRecordset.GetFieldValue(_T("IATACODE"),strValue) ;
		m_sIATACode = strValue; 

		aodRecordset.GetFieldValue(_T("LONGNAME"),strValue) ;
		m_sLongName = strValue;

		aodRecordset.GetFieldValue(_T("ICAOCODE"),strValue);
		m_sICAOCode = strValue;

		aodRecordset.GetFieldValue(_T("ARPCOORDINATES"),strValue);
		m_sARPCoordinates = strValue;

		aodRecordset.GetFieldValue(_T("CITY"),strValue);
		m_sCity = strValue;

		aodRecordset.GetFieldValue(_T("COUNTRY"),strValue);
		m_sCountry = strValue;

		aodRecordset.GetFieldValue(_T("COUNTRYCODE"),strValue);
		m_sCountryCode = strValue;

		aodRecordset.GetFieldValue(_T("ELEVATION"),strValue);
		m_sElevation = strValue ;

		aodRecordset.GetFieldValue(_T("NUMBEROFRUNWAY"),strValue);
		m_sNumberOfRunways = strValue;

		aodRecordset.GetFieldValue(_T("MAX_RUNWAY"),strValue);
		m_sMaximumRunwayLength = strValue;

		aodRecordset.GetFieldValue(_T("ALTERNATEAIRPORTICAOCODE"),strValue);
		m_sAlternateAirportICAOCode = strValue;

		aodRecordset.GetFieldValue(_T("ID"),nID);
		m_ID = nID;
	}
	m_bNeedUpdate = false;

	return true;
}

bool CAirport::saveDatabase(CAirportDatabase* pAirportDatabase)
{
	ASSERT(pAirportDatabase);

	//judge airport whether update database
	if (m_bNeedUpdate == false)
		return true;

	m_sIATACode.Replace("\'" ,"\'\'");
	m_sLongName.Replace("\'" ,"\'\'");
	m_sICAOCode.Replace("\'" ,"\'\'");
	m_sARPCoordinates.Replace("\'" ,"\'\'");
	m_sCity.Replace("\'" ,"\'\'");
	m_sCountry.Replace("\'" ,"\'\'");
	m_sCountryCode.Replace("\'" ,"\'\'");
	m_sElevation.Replace("\'" ,"\'\'");
	m_sNumberOfRunways.Replace("\'" ,"\'\'");
	m_sMaximumRunwayLength.Replace("\'" ,"\'\'");
	m_sAlternateAirportICAOCode.Replace("\'" ,"\'\'");

	if (m_ID == -1)
	{
		CString strSQL ;
		strSQL.Format(_T("INSERT INTO TB_AIRPORTDB_AIRPORT (IATACODE,LONGNAME,ICAOCODE,ARPCOORDINATES,CITY,COUNTRY,COUNTRYCODE,ELEVATION,NUMBEROFRUNWAY,MAX_RUNWAY,ALTERNATEAIRPORTICAOCODE)\
					  VALUES('%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s')"),
					  m_sIATACode,
					  m_sLongName,
					  m_sICAOCode,
					  m_sARPCoordinates,
					  m_sCity,
					  m_sCountry,
					  m_sCountryCode,
					  m_sElevation,
					  m_sNumberOfRunways,
					  m_sMaximumRunwayLength,
					  m_sAlternateAirportICAOCode) ;
		m_ID = CDatabaseADOConnetion::ExecuteSQLStatementAndReturnScopeID(strSQL,pAirportDatabase->GetAirportConnection());
		
	}
	else
	{
		updateDatabase(pAirportDatabase);
	}

	return true;
}

bool CAirport::deleteAllDatabase(CAirportDatabase* pAirportDatabase)
{
	ASSERT(pAirportDatabase);

	CString strSQL(_T(""));

	strSQL.Format(_T("DELETE * FROM TB_AIRPORTDB_AIRPORT"));
	try
	{
		CDatabaseADOConnetion::ExecuteSQLStatement(strSQL,pAirportDatabase->GetAirportConnection());	
	}
	catch (CADOException& e)
	{
		e.ErrorMessage() ;
		return false;
	}
	return true;
}

bool CAirport::deleteDatabase(CAirportDatabase* pAirportDatabase)
{
	ASSERT(pAirportDatabase);

	CString strSQL(_T(""));

	strSQL.Format(_T("DELETE * FROM TB_AIRPORTDB_AIRPORT WHERE ID = %d"), m_ID);

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

bool CAirport::updateDatabase(CAirportDatabase* pAirportDatabase)
{
	ASSERT(pAirportDatabase);

	CString strSQL(_T(""));
	strSQL.Format(_T("UPDATE TB_AIRPORTDB_AIRPORT SET IATACODE = '%s' ,LONGNAME = '%s',ICAOCODE = '%s',ARPCOORDINATES = '%s' ,CITY = '%s',COUNTRY = '%s',COUNTRYCODE = '%s',ELEVATION = '%s',NUMBEROFRUNWAY = '%s'\
				  ,MAX_RUNWAY = '%s', ALTERNATEAIRPORTICAOCODE = '%s' WHERE ID = %d"),
				  m_sIATACode,
				  m_sLongName,
				  m_sICAOCode,
				  m_sARPCoordinates,
				  m_sCity,
				  m_sCountry,
				  m_sCountryCode,
				  m_sElevation,
				  m_sNumberOfRunways,
				  m_sMaximumRunwayLength,
				  m_sAlternateAirportICAOCode,
				  m_ID);

	CDatabaseADOConnetion::ExecuteSQLStatementAndReturnScopeID(strSQL,pAirportDatabase->GetAirportConnection());
	return true;
}
/////////////old version read and save,no more use///////////////////////////////////////////
void CAirport::ReadDataFromDB(CAIRPORTLIST& _dataset ,int _airportDBID,DATABASESOURCE_TYPE type )
{
    if(_airportDBID == -1)
		return ;
    CString SQL ;
	CADORecordset recordset ;
	long count = 0 ;
	SQL.Format(_T("SELECT * FROM TB_AIRPORTDB_AIRPORT WHERE AIRPORTDB_ID = %d"),_airportDBID) ;
	try
	{
		CADODatabase::ExecuteSQLStatement(SQL,count,recordset,type) ;
	}
	catch (CADOException e)
	{
		e.ErrorMessage() ;
		return ;
	}
	CString val ;
	int id =-1 ;
	while(!recordset.IsEOF())
	{
        CAirport* pAirport = new CAirport ;

		recordset.GetFieldValue(_T("IATACODE"),val) ;
		pAirport->m_sIATACode = val ; 

		recordset.GetFieldValue(_T("LONGNAME"),val) ;
		pAirport->m_sLongName = val ;

		recordset.GetFieldValue(_T("ICAOCODE"),val) ;
		pAirport->m_sICAOCode = val ;

		recordset.GetFieldValue(_T("ARPCOORDINATES"),val) ;
		pAirport->m_sARPCoordinates = val ;

		recordset.GetFieldValue(_T("CITY"),val) ;
		pAirport->m_sCity = val ;

		recordset.GetFieldValue(_T("COUNTRY"),val) ;
		pAirport->m_sCountry = val ;

		recordset.GetFieldValue(_T("COUNTRYCODE"),val) ;
		pAirport->m_sCountryCode = val ;

		recordset.GetFieldValue(_T("ELEVATION"),val) ;
		pAirport->m_sElevation = val ;

		recordset.GetFieldValue(_T("NUMBEROFRUNWAY"),val) ;
		pAirport->m_sNumberOfRunways = val ;

		recordset.GetFieldValue(_T("MAX_RUNWAY"),val) ;
		pAirport->m_sMaximumRunwayLength = val ;

		recordset.GetFieldValue(_T("ALTERNATEAIRPORTICAOCODE"),val) ;
		pAirport->m_sAlternateAirportICAOCode = val ;

		recordset.GetFieldValue(_T("ID"),id) ;
		pAirport->SetID(id) ;

      _dataset.push_back(pAirport) ;
	  recordset.MoveNextData() ;
	}
}
void CAirport::WriteDataToDB(CAIRPORTLIST& _dataset ,int _airportDBID,DATABASESOURCE_TYPE type )
{
    if(_airportDBID == -1)
		return ;
	CAIRPORTLIST::iterator iter = _dataset.begin() ;
	for ( ; iter != _dataset.end() ; iter++)
	{
		(*iter)->m_sIATACode.Replace("\'" ,"\'\'");
		(*iter)->m_sLongName.Replace("\'" ,"\'\'");
		(*iter)->m_sICAOCode.Replace("\'" ,"\'\'");
		(*iter)->m_sARPCoordinates.Replace("\'" ,"\'\'");
		(*iter)->m_sCity.Replace("\'" ,"\'\'");
		(*iter)->m_sCountry.Replace("\'" ,"\'\'");
		(*iter)->m_sCountryCode.Replace("\'" ,"\'\'");
		(*iter)->m_sElevation.Replace("\'" ,"\'\'");
		(*iter)->m_sNumberOfRunways.Replace("\'" ,"\'\'");
		(*iter)->m_sMaximumRunwayLength.Replace("\'" ,"\'\'");
		(*iter)->m_sAlternateAirportICAOCode.Replace("\'" ,"\'\'");
		if((*iter)->GetID() == -1)
			CAirport::writeAirportData(*iter,_airportDBID,type) ;
		else
			CAirport::UpdateAirportData(*iter,_airportDBID,type) ;
	}
}
void CAirport::DeleteDataFromDB(CAIRPORTLIST& _dataset,DATABASESOURCE_TYPE type )
{
	CAIRPORTLIST::iterator iter = _dataset.begin() ;
	for ( ; iter != _dataset.end() ;iter++)
	{
		CAirport::DeleteAirportData(*iter,type) ;
	}
}
void CAirport::writeAirportData(CAirport* _airport,int _airportDBID,DATABASESOURCE_TYPE type )
{
	if(_airport == NULL || _airportDBID == -1)
		return ;
	CString SQL ;
	SQL.Format(_T("INSERT INTO TB_AIRPORTDB_AIRPORT (IATACODE,LONGNAME,ICAOCODE,ARPCOORDINATES,CITY,COUNTRY,COUNTRYCODE,ELEVATION,NUMBEROFRUNWAY,MAX_RUNWAY,ALTERNATEAIRPORTICAOCODE,AIRPORTDB_ID)\
				  VALUES('%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s',%d)"),
				  _airport->m_sIATACode,
				  _airport->m_sLongName,
				  _airport->m_sICAOCode,
				  _airport->m_sARPCoordinates,
				  _airport->m_sCity,
				  _airport->m_sCountry,
				  _airport->m_sCountryCode,
				  _airport->m_sElevation,
				  _airport->m_sNumberOfRunways,
				  _airport->m_sMaximumRunwayLength,
				  _airport->m_sAlternateAirportICAOCode,
				  _airportDBID) ;
		int id = CADODatabase::ExecuteSQLStatementAndReturnScopeID(SQL,type) ;
		_airport->SetID(id) ;
}
void CAirport::UpdateAirportData(CAirport* _airport,int _airportDBID,DATABASESOURCE_TYPE type )
{
	if(_airportDBID == NULL || _airportDBID == -1 || !_airport->IsEdit())
		return ;
    CString SQL ;
	SQL.Format(_T("UPDATE TB_AIRPORTDB_AIRPORT SET IATACODE = '%s' ,LONGNAME = '%s',ICAOCODE = '%s',ARPCOORDINATES = '%s' ,CITY = '%s',COUNTRY = '%s',COUNTRYCODE = '%s',ELEVATION = '%s',NUMBEROFRUNWAY = '%s'\
				  ,MAX_RUNWAY = '%s', ALTERNATEAIRPORTICAOCODE = '%s' WHERE ID = %d AND AIRPORTDB_ID = %d"),
				  _airport->m_sIATACode,
				  _airport->m_sLongName,
				  _airport->m_sICAOCode,
				  _airport->m_sARPCoordinates,
				  _airport->m_sCity,
				  _airport->m_sCountry,
				  _airport->m_sCountryCode,
				  _airport->m_sElevation,
				  _airport->m_sNumberOfRunways,
				  _airport->m_sMaximumRunwayLength,
				  _airport->m_sAlternateAirportICAOCode,
				  _airport->GetID(),
				  _airportDBID);

		CADODatabase::ExecuteSQLStatementAndReturnScopeID(SQL,type) ;
}
void CAirport::DeleteAirportData(CAirport* _airport,DATABASESOURCE_TYPE type )
{
  if(_airport== NULL || _airport->GetID() == -1)
	  return ;
  CString SQL ;
  SQL.Format(_T("DELETE * FROM TB_AIRPORTDB_AIRPORT WHERE ID = %d") , _airport->GetID()) ;
 CADODatabase::ExecuteSQLStatement(SQL,type) ;

}
void CAirport::DeleteAllDataFromDB(int _airportDBID,DATABASESOURCE_TYPE type)
{
	CString SQL ;
	SQL.Format(_T("DELETE * FROM TB_AIRPORTDB_AIRPORT WHERE AIRPORTDB_ID = %d") , _airportDBID) ;
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