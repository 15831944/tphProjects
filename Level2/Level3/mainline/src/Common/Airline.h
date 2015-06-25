// Airline.h: interface for the CAirline class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AIRLINE_H__E45258A1_68B8_4777_96B4_826B5A785BDD__INCLUDED_)
#define AFX_AIRLINE_H__E45258A1_68B8_4777_96B4_826B5A785BDD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#pragma warning( disable : 4786 )
#include <vector>
//#include "commondll.h"
#include "../Database/ADODatabase.h"
class CAirportDatabase;

class CAirline : public CObject  
{
	DECLARE_SERIAL(CAirline)
public:
	CAirline();
	CAirline(const CAirline& airline);
	virtual ~CAirline();
	virtual void Serialize(CArchive& ar);

	CString m_sAirlineIATACode;
	CString m_sLongName;
    CString m_sICAOCode;
    CString m_sOtherCodes;
    CString m_sStreetAddress;
    CString m_sCity;
    CString m_sCountry;
    CString m_sHQZipCode;
    CString m_sContinent;
    CString m_sTelephone;
    CString m_sFax;
    CString m_sEMail;
    CString m_sDirector;
    CString m_sFleet;

	unsigned int unImageIndex;

	//----------------------------------------------------------------------------
	//Summary:
	//		avoid airline data does not modified and update database
	//---------------------------------------------------------------------------
	void Update(){m_bNeedUpdate = true;}
private:
	bool m_bNeedUpdate;
public:
	//new version database read and save
	bool loadDatabase(CADORecordset& aodRecordset);
	bool saveDatabase(CAirportDatabase* pAirportDatabase);
	static bool deleteAllDatabase(CAirportDatabase* pAirportDatabase);
	bool deleteDatabase(CAirportDatabase* pAirportDatabase);
	bool updateDatabase(CAirportDatabase* pAirportDatabase);
//////////////////////////////////////////////////////////////////////////
//the interface for DB
protected:
	int m_ID ;
	BOOL m_HasEdit ;
public:
	void SetID(int _id) { m_ID = _id ;} ;
	int GetID(){ return m_ID ;} ;
	BOOL IsEdit() { return m_HasEdit ;} ;
	void IsEdit(BOOL _res) { m_HasEdit = _res ;} ;
public:
	static void ReadDataFromDB(std::vector<CAirline*>& _dataset , int _airportID,DATABASESOURCE_TYPE type) ;
	static void WriteDataToDB(std::vector<CAirline*>& _dataset , int _airportID,DATABASESOURCE_TYPE type) ;
	static void DeleteDataFromDB(std::vector<CAirline*>& _dataset,DATABASESOURCE_TYPE type) ;
	static void DeleteAllData(int _airportID,DATABASESOURCE_TYPE type) ;
protected:
	static void WriteData(CAirline* _airline, int _airportID,DATABASESOURCE_TYPE type) ;
	static void UpdataData(CAirline* _airline, int _airportID,DATABASESOURCE_TYPE type) ;
	static void DeleteData(CAirline* _airline,DATABASESOURCE_TYPE type) ;
};

typedef std::vector<CAirline*> CAIRLINELIST;

typedef struct _airlinefilter {
	CString code;
	bool operator == (const _airlinefilter& _filter)
	{
		return _filter.code.CompareNoCase(code) == 0 ;
	}
} CAIRLINEFILTER;


#endif // !defined(AFX_AIRLINE_H__E45258A1_68B8_4777_96B4_826B5A785BDD__INCLUDED_)
