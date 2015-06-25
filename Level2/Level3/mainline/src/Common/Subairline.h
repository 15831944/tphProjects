// Subairline.h: interface for the CSubairline class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SUBAIRLINE_H__63C4F79C_F382_4A1F_B6F5_9E6DED84472A__INCLUDED_)
#define AFX_SUBAIRLINE_H__63C4F79C_F382_4A1F_B6F5_9E6DED84472A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#pragma warning( disable : 4786 )
#include <vector>
#include "Airline.h"
#include "commondll.h"
class CAirportDatabase;

class CAirlinesManager;
class CSubairline;
typedef std::vector<CSubairline*> CSUBAIRLINELIST;
class COMMON_TRANSFER CSubairline : public CObject  
{
public:
	CSubairline( CAirlinesManager* _pAirlineMan );
	virtual ~CSubairline();

	void AddFilter(CAIRLINEFILTER airlineFilter);

	void GetAirlineList(CAIRLINELIST& APList);

	CString m_sName;
	std::vector<CAIRLINEFILTER> m_vAirlineFilters;

	int contains (const char *p_airline) const;

    void getCode (char *p_str) const { strcpy (p_str, m_sName); }

	void setCode( const char *p_str );

    int operator == (const CSubairline& aSuba) const
	{ 
		return m_sName.CompareNoCase( aSuba.m_sName) == 0 ; }

	int operator < (const CSubairline& aSuba) const 
		{ return m_sName.CompareNoCase(  aSuba.m_sName) < 0; }
	
protected:
	CAirlinesManager* m_pAirlineMan;

///////new version database read and save//////////////////////////////////////////////////
public:
	bool loadDatabase(CADORecordset& aodRecordset,CAirportDatabase* pAirportDatabase);
	bool saveDatabase(CAirportDatabase* pAirportDatabase);
	static bool deleteAllDatabase(CAirportDatabase* pAirportDatabase);
	bool deleteDatabase(CAirportDatabase* pAirportDatabase);
	bool updateDatabase(CAirportDatabase* pAirportDatabase);
	bool deleteAirlineFilter(CAirportDatabase* pAirportDatabase);
protected:
	bool loadAirlineFilter(CAirportDatabase* pAirportDatabase);
	bool saveAirlineFilter(CAirportDatabase* pAirportDatabase);
//the interface for DB 
//////////////////////////////////////////////////////////////////////////
protected:
	int m_ID ;
public:
	void SetID(int _id) { m_ID = _id ;} ;
	int GetID() { return  m_ID ;} ;
public:
	static void ReadDataFromDB(CSUBAIRLINELIST& _DataSet,CAirlinesManager* _AirlineManger,int _airportDBID,DATABASESOURCE_TYPE type) ;
	static void WriteDataToDB(CSUBAIRLINELIST& _DataSet,int _airportDBID,DATABASESOURCE_TYPE type) ;
	static void DeleteDataFromDB(CSUBAIRLINELIST& _DataSet,DATABASESOURCE_TYPE type) ;
	static void DeleteAllData(int _airportDBID,DATABASESOURCE_TYPE type) ;
protected:
	static void WriteSubline(CSubairline* subline,int _airportDBID,DATABASESOURCE_TYPE type) ;
	static void UpdateSubline(CSubairline* subline,int _airportDBID,DATABASESOURCE_TYPE type) ;
	static void DeleteSubline(CSubairline* subline,DATABASESOURCE_TYPE type) ;
protected:
	static void ReadSublineFilterData(CSubairline* subline,DATABASESOURCE_TYPE type ) ;
	static void WriteSublineFilterData(CSubairline* subline,DATABASESOURCE_TYPE type) ;
};




#endif // !defined(AFX_SUBAIRLINE_H__63C4F79C_F382_4A1F_B6F5_9E6DED84472A__INCLUDED_)
