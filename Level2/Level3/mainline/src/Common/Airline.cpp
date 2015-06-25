// Airline.cpp: implementation of the CAirline class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Common.h"
#include "Airline.h"
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

CAirline::CAirline():m_ID(-1)
{
	unImageIndex = 0;
	m_HasEdit = FALSE ;
	m_bNeedUpdate = true;
}

CAirline::~CAirline()
{

}

IMPLEMENT_SERIAL(CAirline, CObject, 1);

void CAirline::Serialize(CArchive& ar)
{
	CObject::Serialize(ar);
	if(ar.IsStoring()) {
		ar << m_sAirlineIATACode;
		ar << m_sLongName;
        ar << m_sICAOCode;
        ar << m_sOtherCodes;
        ar << m_sStreetAddress;
        ar << m_sCity;
        ar << m_sCountry;
        ar << m_sHQZipCode;
        ar << m_sContinent;
        ar << m_sTelephone;
        ar << m_sFax;
        ar << m_sEMail;
        ar << m_sDirector;
        ar << m_sFleet;
	}
	else {
		ar >> m_sAirlineIATACode;
		ar >> m_sLongName;
        ar >> m_sICAOCode;
        ar >> m_sOtherCodes;
        ar >> m_sStreetAddress;
        ar >> m_sCity;
        ar >> m_sCountry;
        ar >> m_sHQZipCode;
        ar >> m_sContinent;
        ar >> m_sTelephone;
        ar >> m_sFax;
        ar >> m_sEMail;
        ar >> m_sDirector;
        ar >> m_sFleet;
	}
}

CAirline::CAirline(const CAirline& airline)
{
	m_sAirlineIATACode = airline.m_sAirlineIATACode;
	m_sLongName = airline.m_sLongName;
	m_sICAOCode = airline.m_sICAOCode;
	m_sOtherCodes = airline.m_sOtherCodes;
	m_sStreetAddress = airline.m_sOtherCodes;
	m_sCity = airline.m_sCity;
	m_sCountry = airline.m_sCountry;
	m_sHQZipCode = airline.m_sHQZipCode;
	m_sContinent = airline.m_sContinent;
	m_sTelephone = airline.m_sTelephone;
	m_sFax = airline.m_sFax;
	m_sEMail = airline.m_sEMail;
	m_sDirector = airline.m_sDirector;
	m_sFleet = airline.m_sFleet;
	unImageIndex = airline.unImageIndex;
	m_HasEdit = airline.m_HasEdit;
	m_bNeedUpdate = airline.m_bNeedUpdate;
}
///////////////////new database version read and save///////////////////////////////////////
bool CAirline::loadDatabase(CADORecordset& aodRecordset)
{
	CString strValue(_T(""));
	if (!aodRecordset.IsEOF())
	{
		aodRecordset.GetFieldValue(_T("ID"),m_ID);
		aodRecordset.GetFieldValue(_T("AIRLINE_IATACODE"),m_sAirlineIATACode);
		aodRecordset.GetFieldValue(_T("LONGNAME"),m_sLongName);
		aodRecordset.GetFieldValue(_T("ICAOCODE"),m_sICAOCode);
		aodRecordset.GetFieldValue(_T("OTHERCODE"),m_sOtherCodes);
		aodRecordset.GetFieldValue(_T("STREETADDRESS"),m_sStreetAddress);
		aodRecordset.GetFieldValue(_T("CITY"),m_sCity);
		aodRecordset.GetFieldValue(_T("COUNTRY"),m_sCountry);
		aodRecordset.GetFieldValue(_T("HQZIPCODE"),m_sHQZipCode);
		aodRecordset.GetFieldValue(_T("CONTINENT"),m_sContinent);
		aodRecordset.GetFieldValue(_T("TELEPHONE"),m_sTelephone);
		aodRecordset.GetFieldValue(_T("FAX"),m_sFax);
		aodRecordset.GetFieldValue(_T("MAIL"),m_sEMail);
		aodRecordset.GetFieldValue(_T("DIRECTOR"),m_sDirector);
		aodRecordset.GetFieldValue(_T("FLEET"),m_sFleet);
	}
	m_bNeedUpdate = false;
	return true;
}

bool CAirline::saveDatabase(CAirportDatabase* pAirportDatabase)
{
	ASSERT(pAirportDatabase);

	//judge airline whether update database
	if (m_bNeedUpdate == false)
		return true;
	
	m_sAirlineIATACode.Replace("\'" ,"\'\'") ;
	m_sLongName.Replace("\'" ,"\'\'") ;
	m_sICAOCode.Replace("\'" ,"\'\'") ;
	m_sOtherCodes.Replace("\'" ,"\'\'") ;
	m_sStreetAddress.Replace("\'" ,"\'\'");
	m_sCity.Replace("\'" ,"\'\'");
	m_sCountry.Replace("\'" ,"\'\'");
	m_sHQZipCode.Replace("\'" ,"\'\'");
	m_sContinent.Replace("\'" ,"\'\'");
	m_sTelephone.Replace("\'" ,"\'\'");
	m_sFax.Replace("\'" ,"\'\'");
	m_sEMail.Replace("\'" ,"\'\'");
	m_sDirector.Replace("\'" ,"\'\'");
	m_sFleet.Replace("\'" ,"\'\'");

	if (m_ID == -1)
	{
		CString strSQL(_T(""));
		strSQL.Format(_T("INSERT INTO TB_AIRPORTDB_AIRLINE (AIRLINE_IATACODE,LONGNAME,ICAOCODE,OTHERCODE,STREETADDRESS,CITY,COUNTRY,HQZIPCODE,CONTINENT,TELEPHONE,FAX,MAIL,DIRECTOR,FLEET) \
					  VALUES('%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s')"),
					  m_sAirlineIATACode,
					  m_sLongName,
					  m_sICAOCode,
					  m_sOtherCodes,
					  m_sStreetAddress,
					  m_sCity,
					  m_sCountry,
					  m_sHQZipCode,
					  m_sContinent,
					  m_sTelephone,
					  m_sFax,
					  m_sEMail,
					  m_sDirector,
					  m_sFleet);

		m_ID = CDatabaseADOConnetion::ExecuteSQLStatementAndReturnScopeID(strSQL,pAirportDatabase->GetAirportConnection());
	}
	else
	{
		updateDatabase(pAirportDatabase);
	}
	return true;
}

bool CAirline::deleteAllDatabase(CAirportDatabase* pAirportDatabase)
{
	ASSERT(pAirportDatabase);

	CString strSQL(_T(""));
	strSQL.Format(_T("DELETE * FROM TB_AIRPORTDB_AIRLINE")) ;
	try
	{
		CDatabaseADOConnetion::ExecuteSQLStatement(strSQL,pAirportDatabase->GetAirportConnection());
	}
	catch (CADOException e)
	{
		e.ErrorMessage() ;
		return false;
	}
	return true;
}

bool CAirline::deleteDatabase(CAirportDatabase* pAirportDatabase)
{
	ASSERT(pAirportDatabase);

	CString strSQL(_T(""));
	strSQL.Format(_T("DELETE * FROM TB_AIRPORTDB_AIRLINE WHERE ID = %d"),m_ID);
	CDatabaseADOConnetion::ExecuteSQLStatement(strSQL,pAirportDatabase->GetAirportConnection());
	return true;
}

bool CAirline::updateDatabase(CAirportDatabase* pAirportDatabase)
{
	ASSERT(pAirportDatabase);

	CString strSQL(_T(""));
	strSQL.Format(_T("UPDATE TB_AIRPORTDB_AIRLINE SET AIRLINE_IATACODE = '%s' , LONGNAME = '%s' ,ICAOCODE = '%s' , OTHERCODE = '%s',STREETADDRESS = '%s',CITY = '%s',COUNTRY = '%s',HQZIPCODE = '%s',\
				  CONTINENT = '%s' ,TELEPHONE = '%s',FAX = '%s' ,MAIL = '%s' ,DIRECTOR = '%s' ,FLEET = '%s' WHERE ID = %d"),
				  m_sAirlineIATACode,
				  m_sLongName,
				  m_sICAOCode,
				  m_sOtherCodes,
				  m_sStreetAddress,
				  m_sCity,
				  m_sCountry,
				  m_sHQZipCode,
				  m_sContinent,
				  m_sTelephone,
				  m_sFax,
				  m_sEMail,
				  m_sDirector,
				  m_sFleet,
				  m_ID) ;
	CDatabaseADOConnetion::ExecuteSQLStatement(strSQL,pAirportDatabase->GetAirportConnection());
	return true;
}
/////////old code version, no more use//////////////////////////////////////////////////////
void CAirline::ReadDataFromDB(std::vector<CAirline*>& _dataset , int _airportID,DATABASESOURCE_TYPE type)
{
   CString SQL ;
   SQL.Format(_T("SELECT * FROM TB_AIRPORTDB_AIRLINE WHERE AIRPORTDB_ID = %d"),_airportID) ;
   CADORecordset recordData ;
   long count = 0 ;
   try
   {
	   CADODatabase::ExecuteSQLStatement(SQL,count,recordData,type) ;
   }
   catch (CADOException e)
   {
   	e.ErrorMessage() ;
	return ;
   }
   CAirline* PAirline = NULL ;
   int id ;
   CString str ;
   while(!recordData.IsEOF())
   {
       PAirline = new CAirline ;
	   recordData.GetFieldValue(_T("ID"),id) ;
	   PAirline->SetID(id) ;
       
	   recordData.GetFieldValue(_T("AIRLINE_IATACODE"),str) ;
	   PAirline->m_sAirlineIATACode = str ;

	   recordData.GetFieldValue(_T("LONGNAME"),str) ;
	   PAirline->m_sLongName = str ;

	   recordData.GetFieldValue(_T("ICAOCODE"),str) ;
	   PAirline->m_sICAOCode = str ;

	   recordData.GetFieldValue(_T("OTHERCODE"),str) ;
	   PAirline->m_sOtherCodes = str ;

	   recordData.GetFieldValue(_T("STREETADDRESS"),str) ;
	   PAirline->m_sStreetAddress = str ;

	   recordData.GetFieldValue(_T("CITY"),str) ;
	   PAirline->m_sCity = str ;

	   recordData.GetFieldValue(_T("COUNTRY"),str) ;
	   PAirline->m_sCountry = str ;

	   recordData.GetFieldValue(_T("HQZIPCODE"),str) ;
	   PAirline->m_sHQZipCode = str ;

	   recordData.GetFieldValue(_T("CONTINENT"),str) ;
	   PAirline->m_sContinent = str ;

	   recordData.GetFieldValue(_T("TELEPHONE"),str) ;
	   PAirline->m_sTelephone = str ;

	   recordData.GetFieldValue(_T("FAX"),str) ;
	   PAirline->m_sFax = str ;

	   recordData.GetFieldValue(_T("MAIL"),str) ;
	   PAirline->m_sEMail = str ;

	   recordData.GetFieldValue(_T("DIRECTOR"),str) ;
	   PAirline->m_sDirector = str ;

	   recordData.GetFieldValue(_T("FLEET"),str) ;
	   PAirline->m_sFleet = str ;
       
	   _dataset.push_back(PAirline) ;
	   recordData.MoveNextData() ;
 
   }
}
void CAirline::WriteDataToDB(std::vector<CAirline*>& _dataset , int _airportID,DATABASESOURCE_TYPE type)
{
    if(_airportID == -1)
		return ;
	std::vector<CAirline*>::iterator iter = _dataset.begin() ;
	for ( ; iter != _dataset.end() ;iter++)
	{
		 (*iter)->m_sAirlineIATACode.Replace("\'" ,"\'\'") ;
		 (*iter)->m_sLongName.Replace("\'" ,"\'\'") ;
		 (*iter)->m_sICAOCode.Replace("\'" ,"\'\'") ;
		 (*iter)->m_sOtherCodes.Replace("\'" ,"\'\'") ;
		 (*iter)->m_sStreetAddress.Replace("\'" ,"\'\'") ;
		 (*iter)->m_sCity.Replace("\'" ,"\'\'");
		(*iter)->m_sCountry.Replace("\'" ,"\'\'");
			 (*iter)->m_sHQZipCode.Replace("\'" ,"\'\'");
			 (*iter)->m_sContinent.Replace("\'" ,"\'\'");
			 (*iter)->m_sTelephone.Replace("\'" ,"\'\'");
			 (*iter)->m_sFax.Replace("\'" ,"\'\'");
			 (*iter)->m_sEMail.Replace("\'" ,"\'\'");
			 (*iter)->m_sDirector.Replace("\'" ,"\'\'");
			 (*iter)->m_sFleet.Replace("\'" ,"\'\'");
		if((*iter)->GetID() == -1)
			WriteData(*iter ,_airportID,type) ;
		else
		   UpdataData(*iter ,_airportID,type) ;
	}
}
void CAirline::DeleteDataFromDB(std::vector<CAirline*>& _dataset,DATABASESOURCE_TYPE type)
{
	std::vector<CAirline*>::iterator iter = _dataset.begin() ;
	for ( ; iter != _dataset.end() ;iter++)
	{
		CAirline::DeleteData(*iter,type) ;
	}
}
void CAirline::WriteData(CAirline* _airline, int _airportID,DATABASESOURCE_TYPE type)
{
	if(_airportID == NULL || _airportID == -1)
		return ;
	CString SQL ;
	SQL.Format(_T("INSERT INTO TB_AIRPORTDB_AIRLINE (AIRLINE_IATACODE,LONGNAME,ICAOCODE,OTHERCODE,STREETADDRESS,CITY,COUNTRY,HQZIPCODE,CONTINENT,TELEPHONE,FAX,MAIL,DIRECTOR,FLEET,AIRPORTDB_ID) \
				  VALUES('%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s',%d)"),
		_airline->m_sAirlineIATACode,
		_airline->m_sLongName,
		_airline->m_sICAOCode,
		_airline->m_sOtherCodes,
		_airline->m_sStreetAddress,
		_airline->m_sCity,
		_airline->m_sCountry,
		_airline->m_sHQZipCode,
		_airline->m_sContinent,
		_airline->m_sTelephone,
		_airline->m_sFax,
		_airline->m_sEMail,
		_airline->m_sDirector,
		_airline->m_sFleet,
		_airportID) ;

		int id = CADODatabase::ExecuteSQLStatementAndReturnScopeID(SQL,type) ;
		_airline->SetID(id) ;

}
void CAirline::UpdataData(CAirline* _airline, int _airportID,DATABASESOURCE_TYPE type)
{
	if(_airportID == NULL || _airportID == -1 || !_airline->IsEdit())
		return ;
	CString SQL ;
	SQL.Format(_T("UPDATE TB_AIRPORTDB_AIRLINE SET AIRLINE_IATACODE = '%s' , LONGNAME = '%s' ,ICAOCODE = '%s' , OTHERCODE = '%s',STREETADDRESS = '%s',CITY = '%s',COUNTRY = '%s',HQZIPCODE = '%s',\
				  CONTINENT = '%s' ,TELEPHONE = '%s',FAX = '%s' ,MAIL = '%s' ,DIRECTOR = '%s' ,FLEET = '%s' WHERE AIRPORTDB_ID = %d AND ID = %d"),
				  _airline->m_sAirlineIATACode,
				  _airline->m_sLongName,
				  _airline->m_sICAOCode,
				  _airline->m_sOtherCodes,
				  _airline->m_sStreetAddress,
				  _airline->m_sCity,
				  _airline->m_sCountry,
				  _airline->m_sHQZipCode,
				  _airline->m_sContinent,
				  _airline->m_sTelephone,
				  _airline->m_sFax,
				  _airline->m_sEMail,
				  _airline->m_sDirector,
				  _airline->m_sFleet,
				  _airportID,
				  _airline->GetID()) ;
		CADODatabase::ExecuteSQLStatement(SQL,type) ;

}
void CAirline::DeleteAllData(int _airportID,DATABASESOURCE_TYPE type)
{
	CString SQL ;
	SQL.Format(_T("DELETE * FROM TB_AIRPORTDB_AIRLINE WHERE AIRPORTDB_ID = %d"),_airportID) ;
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
void CAirline::DeleteData(CAirline* _airline,DATABASESOURCE_TYPE type)
{
	if(_airline == NULL || _airline->GetID() == -1)
		return ;
	CString SQL ;
	SQL.Format(_T("DELETE * FROM TB_AIRPORTDB_AIRLINE WHERE ID = %d"),_airline->GetID()) ;
	CADODatabase::ExecuteSQLStatement(SQL,type) ;
}