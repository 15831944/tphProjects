// AirlineManager.h: interface for the CAirlineManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AIRLINEMANAGER_H__3DF52B84_542B_4E17_87C2_C591C44EDAD9__INCLUDED_)
#define AFX_AIRLINEMANAGER_H__3DF52B84_542B_4E17_87C2_C591C44EDAD9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Airline.h"
#include "Subairline.h"

class CAirportDatabase;
#define AIRLINESMANAGER_ 

/*
typedef struct _airlineinfo {
	CString code;
	CString longname;
} AIRLINEINFO;*/

class COMMON_TRANSFER CAirlinesManager  
{
protected:

public:
	CAirlinesManager(CAirportDatabase* pAirportDatabase);
	virtual ~CAirlinesManager();

	CAIRLINELIST* GetAirlineList() { return & m_vAirlines; }
	void GetAirlinesByFilter(CAIRLINEFILTER airportFilter, CAIRLINELIST& list);
	CSUBAIRLINELIST* GetSubairlineList() { return &m_vSubairlines; }

	void AddSubairline(CString sSubairlineName, CAIRLINEFILTER airlineFilter);
	// return index in the list.
	int findSubairlineItem( CSubairline* _pSubairline );

	static long GetCode(CString codeName);
	static CString GetCodeStr(long code);

	CSubairline* getSubairlineItem( int _nIdx ){ return m_vSubairlines[_nIdx]; }

	int getSubairlineCount(){ return m_vSubairlines.size(); }

	// find the entry and return the index
	// if not find add and return the index
	// return -1 for invalid.
	int FindOrAddEntry( CString _csAirline);
	bool IfExist( CString _csAirline );
	CAirline* GetAirlineByName(const CString& sAirline);

	BOOL LoadData( const CString& _strDBPath );

	//current version read and save
	//we can load database from share template or project by in parameter
	bool loadDatabase(CAirportDatabase* pAirportDatabase); //save the data to new version database
	bool saveDatabase(CAirportDatabase* pAirportDatabase);
	bool deleteDatabase(CAirportDatabase* pAirportDatabase);
	bool loadARCDatabase(CAirportDatabase* pAirportDatabase);

	BOOL LoadDataFromOtherFile( const CString& _strDBPath );
	void Clear();

private:
	CAIRLINELIST	m_vAirlines;
	CSUBAIRLINELIST m_vSubairlines;		
	CString			m_strDBPath;
	CAirportDatabase* m_pAirportDatabase;//for FindOrAddEntry function
//////////////////////////////////////////////////////////////////////////
//the interface for DB
public:
	void ReadDataFromDB(int _airportID,DATABASESOURCE_TYPE type = DATABASESOURCE_TYPE_ACCESS) ;
	void WriteDataToDB(int _airportID,DATABASESOURCE_TYPE type = DATABASESOURCE_TYPE_ACCESS) ;
protected:
    CAIRLINELIST m_DelAirlines ;
    CSUBAIRLINELIST m_DelSubAirline ;
	int m_AirportID ;
public:
	void SetID(int _id) { m_AirportID = _id ;} ;
	int GetID() { return m_AirportID ;} ;
protected:
	void ClearDelAirlines() ;
	void ClearDelSubAirlines() ;
public:
	void RemoveAirline(CAirline* _airline) ;
	void RemoveSubAirline(CSubairline* _subairline) ;
public:
	void ReSetAllID() ;
	void DeleteAllDataFromDB(int _airportID,DATABASESOURCE_TYPE type = DATABASESOURCE_TYPE_ACCESS ) ;
//////////////////////////////////////////////////////////////////////////

	BOOL ReadCommonAirlineData(const CString& _Path) ;
protected:
	CString m_strCommonDBPath ;
public:
	CString GetCommonDBPath() { return m_strCommonDBPath ;} ;
	void SetCommonDBPath(const CString& _path) { m_strCommonDBPath = _path ;}  ;
};

//global function
CAirlinesManager* _g_GetActiveAirlineMan( CAirportDatabase* _pAirportDB );

#endif // !defined(AFX_AIRLINEMANAGER_H__3DF52B84_542B_4E17_87C2_C591C44EDAD9__INCLUDED_)
