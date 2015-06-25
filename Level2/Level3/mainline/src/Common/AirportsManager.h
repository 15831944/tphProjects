// AirportsManager.h: interface for the CAirportsManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AIRPORTSMANAGER_H__BF1859D5_693E_4664_B14C_A087B27D11B8__INCLUDED_)
#define AFX_AIRPORTSMANAGER_H__BF1859D5_693E_4664_B14C_A087B27D11B8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Airport.h"
#include "Sector.h"
#include "../Database/ADODatabase.h"
class CAirportDatabase;
//class InputTerminal;
#define AIRPORTSMANAGER_  


typedef struct _airportinfo {
	CString code;
	CString longname;
} AIRPORTINFO;

class CLatitude;
class CLongitude;
class COMMON_TRANSFER CAirportsManager  
{
protected:

	bool IsStringAllAlpha(const CString &_str);

public:
	CAirportsManager(CAirportDatabase* pAirportDatabase);
	virtual ~CAirportsManager();

	CAIRPORTLIST* GetAirportList() { return & m_vAirports; }
	void GetAirportsByFilter(CAIRPORTFILTER airportFilter, CAIRPORTLIST& list);
	CSECTORLIST* GetSectorList() { return &m_vSectors; }

	void AddSector(CString sSectorName, CAIRPORTFILTER airportFilter);
	// return index in the list.
	int findSectItem( CSector* _pSector );
	CAirport* GetAiportByName(const CString& sAirport);

	CSector* getSectItem( int _nIdx ){ return m_vSectors[_nIdx]; }

	int getSectCount(){ return m_vSectors.size(); }

	// find the entry and return the index
	// if not find add and return the index
	// return -1 for invalid.
	int FindOrAddEntry( CString _csAirport);
	void GetAirportCoordinates(const CString strAirport, CLatitude& tlat, CLongitude& tlong );

	BOOL LoadData( const CString& _strDBPath );
	//current version read and save
	bool loadDatabase(CAirportDatabase* pAirportDatabase); //save the data to new version database
	bool saveDatabase(CAirportDatabase* pAirportDatabase);
	bool deleteDatabase(CAirportDatabase* pAirportDatabase);
	bool loadARCDatabase(CAirportDatabase* pAirportDatabase);

	BOOL LoadDataFromOtherFile(const CString& _strDBPath);
	void Clear();

private:
	CAIRPORTLIST m_vAirports;
	CSECTORLIST  m_vSectors;
	CString		 m_strDBPath;
	CAirportDatabase* m_pAirportDatabase;

//the interface for DB
//////////////////////////////////////////////////////////////////////////
public:
	 void ReadDataFromDB(int _airportDBID,DATABASESOURCE_TYPE type = DATABASESOURCE_TYPE_ACCESS) ;
	 void WriteDataToDB(int _airportDBID,DATABASESOURCE_TYPE type = DATABASESOURCE_TYPE_ACCESS) ;
protected:
     CAIRPORTLIST m_DelAirport ;
     CSECTORLIST m_DelSectors ;
	 int m_AirportID ;
public:
	void SetID(int _id) { m_AirportID = _id ;} ;
	int GetID() { return m_AirportID ;} ;
protected:
	void ClearDelAirport() ;
	void ClearDelSectors() ;
public:
	void RemveAirport(CAirport* _airport) ;
	void RemoveSector(CSector* _sector) ;
public:
	void ReSetAllID() ;
	void DeleteAllDataFromDB(int _airportDBID,DATABASESOURCE_TYPE type = DATABASESOURCE_TYPE_ACCESS) ;
    BOOL ReadCommonAirports(const CString& _path) ;
protected:
	CString m_strCommonDBPath ;
public:
	CString GetCommonDBPath() { return m_strCommonDBPath ;} ;
	void SetCommonDBPath(const CString& _path) { m_strCommonDBPath = _path ;}  ;
public:
	BOOL IsExist(CAirport* _airport) ;
};

//global function
CAirportsManager* _g_GetActiveAirportMan( CAirportDatabase* _pAirportDB );

#endif // !defined(AFX_AIRPORTSMANAGER_H__BF1859D5_693E_4664_B14C_A087B27D11B8__INCLUDED_)
