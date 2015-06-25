// AirportDatabaseList.h: interface for the CAirportDatabaseList class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AIRPORTDATABASELIST_H__B7542F45_BF42_4F4E_AD04_4029F2DE1A84__INCLUDED_)
#define AFX_AIRPORTDATABASELIST_H__B7542F45_BF42_4F4E_AD04_4029F2DE1A84__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "dataset.H"
#include <vector>
#include "../Common/Singleton.h"
using namespace std;
#define  GlobalDB_VERSION 1000
// declare
class CAirportDatabase;
class AirportDatabaseConnection;
class CAirportDatabase;
class CShareTemplateDatabase;

typedef std::vector< CShareTemplateDatabase* > AIRPORTDB_LIST;

#define AIRPORTDBLIST CAirportDatabaseList::GetInstance()
#define DESTROYAIRPORTDBLIST CAirportDatabaseList::ReleaseInstance()

class CAirportDatabaseList : public Singleton<CAirportDatabaseList>,public DataSet 
{
public:
	CAirportDatabaseList();
	virtual ~CAirportDatabaseList();
public:
	virtual void clear();
	virtual void readData (ArctermFile& p_file);
	virtual void writeData (ArctermFile& p_file) const;
	virtual void readObsoleteData( ArctermFile& p_file ){loadPreVersionData(p_file);}
	bool loadDatabase();//load current version airport list data
	void loadPreVersionData(ArctermFile& p_file);
	
	bool updateShareTemplateDatabase();
	virtual const char *getTitle (void) const;
	virtual const char *getHeaders (void) const;

	float getVersion()const {return m_fVersion;}

protected:
	bool ConvertAiportDatabaseToNewVersion();
//////////////////////////////////////Old version code///////////////////////////////////////
//should not be modify and use///////////////////////////////////////
private:
	AIRPORTDB_LIST m_vAirportDB;
	AIRPORTDB_LIST m_vDelAirportDB ;
	long	  m_lAirportDBIndex;			
	
protected:
	void CopyDataFromFileToDataBase() ;
	void ResetDefaultDataBase() ;
public:
	static CAirportDatabaseList* GetInstance( void );
	static void ReleaseInstance( void );
	// Init Value in case no file can be read.
	virtual void initDefaultValues();
	// get unique name
	CString getUniqueName( const CString& _strName );


	CShareTemplateDatabase* getAirportDBByName(const CString strName);
	
	// get the size of Airport db
	int getDBCount( void ) const;
	
	// get DB by index
	CAirportDatabase* getAirportDBByIndex( int _idx );
	
	//// copy db 
	bool CopyDB( CString& _sourceDBPath, const CString& _strDestPath );
	//
	//// delete db
	//bool DeleteDB( const CAirportDatabase* _pDelDB, bool _bDeletePath = false );
	
	// check name if repeat
	bool checkNameIfRepeat( const CString& _strName, const CAirportDatabase* _pGLDB );
	
	// get all db file name
	void getAllDBFileName( CAirportDatabase* _pGLDB, std::vector< CString >& _vAllFileName ) const;
	
	//// add a new db entry
	//long addNewDBEntry( const CString& _strDBName, const CString& _strDBPath, bool _bNeedToSaveData = true );
	//
	//// get db index by name
	//long getIDByName( const CString& _strDBName );

	//copy all default db file to destfolder
	void CopyDefaultDB(const CString& sDestDBName,const CString& sDestPath ="");

	void _LoadDataSet(const CString strPath);
//////////////////////////////////////////////////////////////////////////
// the interface for converting arcportDB From file to DataBase
// added by cj
//////////////////////////////////////////////////////////////////////////
public:
	//read all arcportDB , new instance for CAirportDatabaseList ,not load arcport DB  data .
	void  ReadAirportDBData() ;
	//write all arcportDB , new instance for CAirportDatabaseList ,not load arcport DB  data .
	void  WriteAiportDBData() ;
	//delete airport DB from global DB
	void DeleteAirportDBData(CAirportDatabase* _DataBase) ;
	//_Id : the id of airportDB in global Database ,if id is invalid ,do nothing 
protected:
	void WriteAirport(CAirportDatabase* _DataBase) ;
	void UpdateAirport(CAirportDatabase* _DataBase) ;
public:
	void DeleteAirportDBData(int _id) ;
	//_name : the name of airportDB , if _name existed , delete airportDB Item ,else do nothing
	void DeleteAirportDBData(const CString& _name) ;
    //load airport Db form global DB ,if no airportDB ,return false else reurn true .
	BOOL LoadAirporDBByName(CString _Name , CAirportDatabase* _DataBase) ;
	//write data to global DB,first check the _Name ,if _Name has existed , get ID and Update Data . 
	//else create new record data for _Name . 
	BOOL WriteAirportDBByName(CString _Name , CAirportDatabase* _DataBase) ;

protected:
	void clearDelAirport() ;
public:
    CAirportDatabase* addNewDBEntry( const CString& _strDBName);
	CAirportDatabase* addOldDBEntry(const CString& _strDBName);

	double GetGloabalVersion() ;
	void SetGloabalVersion(double _version) ;

	void RemoveAirportDB(CAirportDatabase* _database) ;
	void RemoveAIrportDB(const CString& _strName) ;
public:
	static BOOL IsAirportFile(CString filename) ;
	protected:
	void CopyTheUpdateCommonFileToDefaultFolder();
//////////////////////////////////////Old version code///////////////////////////////////////
};


#endif // !defined(AFX_AIRPORTDATABASELIST_H__B7542F45_BF42_4F4E_AD04_4029F2DE1A84__INCLUDED_)
