// Sector.h: interface for the CSector class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SECTOR_H__20229026_1F3C_464C_9BCA_880033302826__INCLUDED_)
#define AFX_SECTOR_H__20229026_1F3C_464C_9BCA_880033302826__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#pragma warning( disable : 4786 )
#include <vector>
#include "Airport.h"
#include "commondll.h"

class CAirportsManager;
class CSector ;
class CAirportDatabase;
typedef std::vector<CSector*> CSECTORLIST;
class COMMON_TRANSFER CSector : public CObject
{
public:
	CSector( CAirportsManager* _pAirportMan );
	virtual ~CSector();

	void AddFilter(CAIRPORTFILTER airportFilter);

	void GetAirportList(CAIRPORTLIST& APList);

	CString m_sName;
	std::vector<CAIRPORTFILTER> m_vAirportFilters;

	int contains (const char *p_airport) const;

    void getCode (char *p_str) const { strcpy (p_str, m_sName); }

	void setCode( const char *p_str );

    int operator == (const CSector& aSect) const
	{ 
		return m_sName.CompareNoCase( aSect.m_sName) == 0 ;}

	int operator < (const CSector& aSect) const 
	{ return m_sName.CompareNoCase(aSect.m_sName) < 0; }

///////new version database read and save//////////////////////////////////////////////////
public:
	bool loadDatabase(CADORecordset& aodRecordset,CAirportDatabase* pAirportDatabase);
	bool saveDatabase(CAirportDatabase* pAirportDatabase);
	static bool deleteAllDatabase(CAirportDatabase* pAirportDatabase);
	bool deleteDatabase(CAirportDatabase* pAirportDatabase);
	bool updateDatabase(CAirportDatabase* pAirportDatabase);
	bool deleteSectorFilter(CAirportDatabase* pAirportDatabase);
protected:
	bool loadSectorFilter(CAirportDatabase* pAirportDatabase);
	bool saveSectorFilter(CAirportDatabase* pAirportDatabase);
///////////old database version read and save///////////////////////////////////////////////
protected:
	CAirportsManager*	m_pAirportMan;
protected:
	int m_ID ;
public:
	int GetID() { return m_ID ;} ;
	void SetID(int _id) { m_ID = _id ;} ;
public:
    static void ReadDataFromDB(CSECTORLIST& _dataSet ,CAirportsManager* _pAirportMan , int _airportDBID,DATABASESOURCE_TYPE type) ;
	static void WriteDataToDB(CSECTORLIST& _dataSet ,int _airportDBID,DATABASESOURCE_TYPE type) ;
    static void DeleteDataFromDB(CSECTORLIST& _dataSet,DATABASESOURCE_TYPE type) ;
	static void DeleteAllDataFromDB(int _airportDBID,DATABASESOURCE_TYPE type) ;
protected:
    static void WriteData(CSector* _sector,int _airportDBID,DATABASESOURCE_TYPE type) ;
	static void UpdateData(CSector* _sector,int _airportDBID,DATABASESOURCE_TYPE type) ;
		static void DeleteSector(CSector* _sector,DATABASESOURCE_TYPE type) ;
protected:
	static void ReadSectorData(CSector* _sector,DATABASESOURCE_TYPE type) ;
	static void WriteSectorData(CSector* _sector,DATABASESOURCE_TYPE type) ;
	static void UpdateSectorData(CSector* _sector,DATABASESOURCE_TYPE type) ;
	
};



#endif // !defined(AFX_SECTOR_H__20229026_1F3C_464C_9BCA_880033302826__INCLUDED_)
