// Airport.h: interface for the CAirport class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AIRPORT_H__B97DF017_3D3A_459C_A6BD_E554AF8BE5E5__INCLUDED_)
#define AFX_AIRPORT_H__B97DF017_3D3A_459C_A6BD_E554AF8BE5E5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../Database/ADODatabase.h"
#pragma warning( disable : 4786 )
#include <vector>
#include "commondll.h"
class CAirport ;
class CAirportDatabase;
typedef std::vector<CAirport*> CAIRPORTLIST;
class COMMON_TRANSFER CAirport : public CObject
{
	DECLARE_SERIAL(CAirport)
public:
	CAirport();
	CAirport(const CAirport& airport);
	virtual ~CAirport();
	virtual void Serialize(CArchive& ar);

	CString m_sIATACode;
	CString m_sLongName;
    CString m_sICAOCode;
    CString m_sARPCoordinates;

    CString m_sCity;
    CString m_sCountry;
    CString m_sCountryCode;
    CString m_sElevation;
    CString m_sNumberOfRunways;
    CString m_sMaximumRunwayLength;
    CString m_sAlternateAirportICAOCode;

	//----------------------------------------------------------------------------
	//Summary:
	//		avoid airport data does not modified and update database
	//---------------------------------------------------------------------------
	void Update(){m_bNeedUpdate = true;}
private:
	bool  m_bNeedUpdate;//airport be modified

public:
	//new version database read and save
	bool loadDatabase(CADORecordset& aodRecordset);
	bool saveDatabase(CAirportDatabase* pAirportDatabase);
	static bool deleteAllDatabase(CAirportDatabase* pAirportDatabase);
	bool deleteDatabase(CAirportDatabase* pAirportDatabase);
	bool updateDatabase(CAirportDatabase* pAirportDatabase);
//////////////////////////////////////////////////////////////////////////
protected:
	int m_ID ;
	BOOL m_HasEdit ;
public:
	void SetID(int _id) { m_ID = _id ;} ;
	int GetID() { return m_ID ;} ;
	BOOL IsEdit() { return m_HasEdit ;} ;
	void IsEdit(BOOL _res) { m_HasEdit = _res ;} ;
public:
    static void ReadDataFromDB(CAIRPORTLIST& _dataset ,int _airportDBID,DATABASESOURCE_TYPE type ) ;
	static void WriteDataToDB(CAIRPORTLIST& _dataset ,int _airportDBID,DATABASESOURCE_TYPE type ) ;
	static void DeleteDataFromDB(CAIRPORTLIST& _dataset,DATABASESOURCE_TYPE type ) ;
	static void DeleteAllDataFromDB(int _airportDBID,DATABASESOURCE_TYPE type) ;
protected:
	static void writeAirportData(CAirport* _airport,int _airportDBID,DATABASESOURCE_TYPE type ) ;
	static void UpdateAirportData(CAirport* _airport,int _airportDBID,DATABASESOURCE_TYPE type ) ;
    static void DeleteAirportData(CAirport* _airport,DATABASESOURCE_TYPE type ) ;
};



typedef struct _airportfilter {
	CString sIATACode;
    CString sICAOCode;
} CAIRPORTFILTER;


#endif // !defined(AFX_AIRPORT_H__B97DF017_3D3A_459C_A6BD_E554AF8BE5E5__INCLUDED_)
