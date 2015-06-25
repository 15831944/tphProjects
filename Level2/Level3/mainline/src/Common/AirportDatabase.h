#pragma once
#include "../Database/ARCportDatabaseConnection.h"
#include "CommonDll.h"
#include "../Database/ADODatabase.h"
class CACTypesManager;
class CAirlinesManager;
class CAirportsManager;
class CProbDistManager;
class FlightManager;
class ArctermFile;
class CAircraftComponentModelDatabase;
class CAircraftModelDatabase;
class ACTypeDoorManager;
class AirportDatabaseConnection;
class CAircraftFurnishingSectionManager;
class CAirline;
class CAirport;
class CACType;
#define AIRPORTDB_VERSION 1000
///database  
class COMMON_TRANSFER CAirportDatabase
{
public:
	CAirportDatabase();
	virtual ~CAirportDatabase();

protected:
	long								 m_lIndex;	// unique index
	CString								 m_strName;	// name
	CString								 m_strFolder;// folder path
	CACTypesManager*					 m_pAcMan;			
	CAirlinesManager*					 m_pAirlineMan;
	CAirportsManager*					 m_pAirportMan;
	ACTypeDoorManager*					 m_pACDoorMan;
	FlightManager*						 m_pFlightMan;
	CAircraftModelDatabase*					 m_pAircraftModeList;
	CProbDistManager*					 m_pProbDistMan;
	CAircraftComponentModelDatabase*	 m_pACCompModelDB;
	CAircraftFurnishingSectionManager*   m_pAircraftFurnishingMan;
	int									 m_proVersion;//project version
	AirportDatabaseConnection*			 m_pConnectionPtr;// connect to the database
	CString								 m_strDataFilePath;//database file path

public:
	//interface for database control
	virtual bool loadDatabase();
	virtual bool loadCommonDatabase(){return true;};
	virtual bool saveDatabase();
	virtual bool deleteDatabase();
	virtual bool loadPreVersionData(DATABASESOURCE_TYPE type);
	virtual bool loadDataFromFile();//for old version load

	virtual bool loadFromOtherDatabase(CAirportDatabase* pOtherDatabase);
	virtual bool saveAsTemplateDatabase(CAirportDatabase* pOtherDatabase);

	virtual void loadAirlineFromDatabase(CAirportDatabase* pOtherDatabase){}
	virtual void loadAirportFromDatabase(CAirportDatabase* pOtherDatabase){}
	virtual void loadAircraftFromDatabase(CAirportDatabase* pOtherDatabase){}

	//for the airport database version
	virtual bool readAirport();
	virtual bool readAirport(CADORecordset& recordset);
	virtual void SaveAirport();
	virtual void DeleteAirport();
	virtual void UpdateAirport();

public:
	virtual void SetProjectVersion(int _proVer) { m_proVersion = _proVer ;}
	// index
	virtual void setIndex( long _idx ) { m_lIndex = _idx; };
	virtual long getIndex( void ) const { return m_lIndex; };
	// name
	virtual void setName( const CString& _strName ){ m_strName = _strName; }
	virtual const CString& getName( void ) const{ return m_strName; };
	// folder
	virtual void setFolder( const CString& _strFolder ){ m_strFolder = _strFolder; }
	virtual const CString& getFolder( void ) const{ return m_strFolder; }

	//database file
	virtual const CString& getDatabaseFilePath()const{return m_strDataFilePath;}
	// ac manager
	virtual CACTypesManager* getAcMan( void ) { return m_pAcMan; }
	// airlines manager
	virtual CAirlinesManager* getAirlineMan( void ) { return m_pAirlineMan; }
	// airports manager
	virtual CAirportsManager* getAirportMan( void ) { return m_pAirportMan; }
	// ac door manager
	virtual ACTypeDoorManager* getAcDoorMan()const { return m_pACDoorMan; }
	// Probability Distribution manager 
	virtual CProbDistManager* getProbDistMan( void )const { return m_pProbDistMan; }
	// aircraft component database
	virtual CAircraftComponentModelDatabase * getACCompModelDB() const { return m_pACCompModelDB; }
	//flight group database
	virtual FlightManager* getFlightMan(void)const { return m_pFlightMan;}
	// aircraft model database
	virtual CAircraftModelDatabase* getModeList(void)const{return m_pAircraftModeList;}
	virtual CAircraftFurnishingSectionManager* getFurnishingMan(void)const {return m_pAircraftFurnishingMan;}


	virtual AirportDatabaseConnection* GetAirportConnection()const{return m_pConnectionPtr;}
	virtual AirportDatabaseConnection*GetArcportConnection()const{return m_pConnectionPtr;}
	// free **Manager pointer 
	virtual void clearData();
	// check if have load **Manager data 
	virtual bool hadLoadDatabase() const;
//backward compatible,the implement should not be modified	
///////////////////////////////////Old Version code//////////////////////////////////////////////
public:
	int	m_nVersion ;//arcport database version
protected:
	int m_ID ;
public:
	virtual void SetID(int _id) { m_ID = _id;} 
	virtual int  GetID()const { return m_ID;} 

	virtual void LoadDataFromDB(DATABASESOURCE_TYPE type = DATABASESOURCE_TYPE_ACCESS) ;
	virtual void SaveDataToDB(DATABASESOURCE_TYPE type = DATABASESOURCE_TYPE_ACCESS) ;
	virtual void ReadAirportDataBase(DATABASESOURCE_TYPE type = DATABASESOURCE_TYPE_ACCESS) ;
	virtual void WriteAiportDataBase(DATABASESOURCE_TYPE type = DATABASESOURCE_TYPE_ACCESS) ;
	virtual void ResetAllID() ;
	virtual void DeleteAllData(DATABASESOURCE_TYPE type = DATABASESOURCE_TYPE_ACCESS) ;

	static BOOL CheckTheNameIsAvailably(const CString& _name ,CString& _errChar) ;
///////////////////////////////////Old Version code//////////////////////////////////////////////////
};

///////////////////////////////////////////////////////////////////////////////////////
///Parts template database for arcport
class CPartsTemplateDatabase :public CAirportDatabase
{
public:
	CPartsTemplateDatabase(const CString& strDatabaseFilePath);
	~CPartsTemplateDatabase();


///////////old version code//////////////////////////////////////////////////////
	void loadFromFile( ArctermFile& file );
};

/////////////////////////////////////////////////////////////////////////////////////////
/// Parts database
class CPartsDatabase: public CAirportDatabase
{
public:
	CPartsDatabase(const CString& strDatabaseFilePath);
	~CPartsDatabase();
};

/////////////////////////////////////////////////////////////////////////////////////////
// common data, user can not modify
class CARCDatabase : public CAirportDatabase
{
public:
	CARCDatabase(const CString& strDatabaseFilePath);
	~CARCDatabase();

public:
	virtual bool loadCommonDatabase();
	CAirline* GetArcDatabaseAirline(const CString& sAirline);
	CAirport* GetArcDatabaseAirport(const CString& sAirport);
	CACType* GetArcDatabaseActype(const CString& sActype);
};

//////////////////////////////////////////////////////////////////////////////////////////
///Project database is ARCDatabase + PartsDatabase
class CShareTemplateDatabase;
class COMMON_TRANSFER CARCProjectDatabase : public CAirportDatabase
{
public:
	CARCProjectDatabase(const CString& strARCDatabaseFilePath,const CString& strProjectDatabaseFilePath);
	~CARCProjectDatabase();

public:
	virtual bool loadDatabase();
	virtual bool saveDatabase();
	virtual bool deleteDatabase();
	virtual bool loadPreVersionData(DATABASESOURCE_TYPE type);
	virtual bool loadDataFromFile();
	virtual bool loadFromOtherDatabase(CAirportDatabase* pOtherDatabase);
	virtual bool saveAsTemplateDatabase(CAirportDatabase* pOtherDatabase);
	virtual AirportDatabaseConnection* GetAirportConnection()const{return m_pPartsDatabase->GetAirportConnection();}
	virtual AirportDatabaseConnection*GetArcportConnection()const{return m_pARCDatabase->GetAirportConnection();}
	CAirline* GetArcDatabaseAirline(const CString& sAirline);
	CAirport* GetArcDatabaseAirport(const CString& sAirport);
	CACType* GetArcDatabaseActype(const CString& sActype);

	virtual void SetID(int _id); 
	virtual int  GetID()const;
	virtual void SetProjectVersion(int _proVer);
	virtual void setIndex( long _idx );
	virtual long getIndex( void ) const;
	virtual void setName( const CString& _strName );
	virtual const CString& getName( void ) const;
	virtual const CString& getDatabaseFilePath()const{return m_pPartsDatabase->getDatabaseFilePath();}

	virtual void setFolder( const CString& _strFolder );
	virtual const CString& getFolder( void ) const;
	virtual CACTypesManager* getAcMan( void );
	virtual CAirlinesManager* getAirlineMan(void);
	virtual CAirportsManager* getAirportMan(void);
	virtual ACTypeDoorManager* getAcDoorMan()const;
	virtual CProbDistManager* getProbDistMan( void )const;
	virtual CAircraftComponentModelDatabase * getACCompModelDB() const;
	virtual FlightManager* getFlightMan(void)const;
	virtual CAircraftModelDatabase* getModeList(void)const;
	virtual CAircraftFurnishingSectionManager* getFurnishingMan(void)const;
	virtual bool hadLoadDatabase() const;
	virtual void ResetAllID();
	virtual bool readAirport();


	void loadAirlineFromDatabase(CAirportDatabase* pOtherDatabase);
	void loadAirportFromDatabase(CAirportDatabase* pOtherDatabase);
	void loadAircraftFromDatabase(CAirportDatabase* pOtherDatabase);
protected:
	CARCDatabase*  m_pARCDatabase;//do not call this member outside
	CPartsDatabase*  m_pPartsDatabase;//do not call this member outside
};

//////////////////////////////////////////////////////////////////////////////////////////
//Share template database is ARCDabase + PartsTemplate Database
class COMMON_TRANSFER CShareTemplateDatabase : public CAirportDatabase
{
public:
	CShareTemplateDatabase(const CString& strARCDatabasePath,const CString& strShareTemplateDatabasePath);
	~CShareTemplateDatabase();

public:
	virtual bool loadDatabase();
	virtual bool saveDatabase();
	virtual bool deleteDatabase();
	virtual bool loadPreVersionData(DATABASESOURCE_TYPE type);
	virtual bool loadDataFromFile();
	virtual bool loadFromOtherDatabase(CAirportDatabase* pOtherDatabase);
	virtual bool saveAsTemplateDatabase(CAirportDatabase* pOtherDatabase);
	virtual AirportDatabaseConnection* GetAirportConnection()const{return m_pPartsTemplateDatabase->GetAirportConnection();}
	CAirline* GetArcDatabaseAirline(const CString& sAirline);
	CAirport* GetArcDatabaseAirport(const CString& sAirport);
	CACType* GetArcDatabaseActype(const CString& sActype);

	virtual void SetID(int _id);
	virtual int  GetID()const; 
	virtual void SetProjectVersion(int _proVer);
	virtual void setIndex( long _idx );
	virtual long getIndex( void ) const;
	virtual void setName( const CString& _strName );
	virtual const CString& getName( void ) const;
	virtual const CString& getDatabaseFilePath()const{return m_pPartsTemplateDatabase->getDatabaseFilePath();}

	virtual void setFolder( const CString& _strFolder );
	virtual const CString& getFolder( void ) const;
	virtual CACTypesManager* getAcMan( void );
	virtual CAirlinesManager* getAirlineMan(void);
	virtual CAirportsManager* getAirportMan(void);
	virtual ACTypeDoorManager* getAcDoorMan()const;
	virtual CProbDistManager* getProbDistMan( void )const;
	virtual CAircraftComponentModelDatabase * getACCompModelDB() const;
	virtual FlightManager* getFlightMan(void)const;
	virtual CAircraftModelDatabase* getModeList(void)const;
	virtual CAircraftFurnishingSectionManager* getFurnishingMan(void)const;
	virtual void ResetAllID();
	virtual bool hadLoadDatabase() const;
protected:
	CARCDatabase*  m_pARCDatabase;//do not call this member outside
	CPartsTemplateDatabase*  m_pPartsTemplateDatabase;//do not call this member outside

//////////////////////////////////////////////Old version code/////////////////////////////
public:
	void LoadDataFromDB(DATABASESOURCE_TYPE type = DATABASESOURCE_TYPE_ACCESS) ;
	bool loadOldDatabase();
	void loadFromFile( ArctermFile& file );
//////////////////////////////////////////////Old version code/////////////////////////////
};
