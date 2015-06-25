#include "StdAfx.h"
#include "AirportDatabase.h"
#include "ACTypesManager.h"
#include "AirlineManager.h"
#include "FlightManager.h"
#include "AirportsManager.h"
#include "probdistmanager.h"
#include "common\termfile.h"
#include "exeption.h"
#include "ProjectManager.h"
//#include ".\AircraftComponentModelDatabase.h"
#include <inputonboard\AircraftComponentModelDatabase.h>
#include "AirportDatabaseList.h"
#include "../Common/ProgressBar.h"
#include "../Database/ARCportADODatabase.h"
#include "../InputOnboard/AircraftFurnishingSection.h"
#include "AircraftModel.h"
#include "ACTypeDoor.h"
#include <Shlwapi.h>
#include "AircraftModelDatabase.h"
#include "AircraftAliasManager.h"
////////////////////////////////




static const CString _strProbDistFileName	= "probdist.pbd";
////ARCportDatabase implement
CAirportDatabase::CAirportDatabase()
:m_lIndex(-1l)
,m_pAcMan( NULL)
,m_pAirlineMan( NULL )
,m_pAirportMan( NULL )
,m_pACDoorMan(NULL)
,m_pFlightMan(NULL)
,m_pProbDistMan (NULL)
,m_ID(-1)
,m_proVersion(PROJECT_VERSION)
,m_nVersion(AIRPORTDB_VERSION)
,m_pACCompModelDB(NULL)
,m_pAircraftFurnishingMan(NULL)
,m_pAircraftModeList(NULL)
{
	
}

CAirportDatabase::~CAirportDatabase()
{

}

bool CAirportDatabase::readAirport()
{
	CString strSQL(_T(""));
	strSQL.Format(_T("SELECT * FROM TB_PROJECT_AIRPORTDB ")) ;
	CADORecordset adoRecordset;
	long  lCount = 0 ;
	try
	{
		CDatabaseADOConnetion::ExecuteSQLStatement(strSQL,lCount,adoRecordset,GetAirportConnection());
	}
	catch (CADOException& e)
	{
		e.ErrorMessage();
		return false;
	}

	readAirport(adoRecordset);
	return true;
}

bool CAirportDatabase::readAirport(CADORecordset& recordset)
{
	if (!recordset.IsEOF())
	{
		recordset.GetFieldValue(_T("ID"),m_ID);
		SetID(m_ID);
		recordset.GetFieldValue(_T("AIRPORTDB_NAME"),m_strName);
		setName(m_strName);
		recordset.GetFieldValue(_T("VERSION"),m_nVersion);
		SetProjectVersion(m_nVersion);
	}

	return true;
}

void CAirportDatabase::SaveAirport()
{
	if (GetID() == -1)
	{
		CString strSQL(_T(""));
		strSQL.Format(_T("INSERT INTO TB_PROJECT_AIRPORTDB (AIRPORTDB_NAME,VERSION) VALUES('%s',%d) "),getName(),AIRPORTDB_VERSION);
		try
		{
			int nID = CDatabaseADOConnetion::ExecuteSQLStatementAndReturnScopeID(strSQL,GetAirportConnection());
			SetID(nID);
		}
		catch (CADOException& e)
		{
			e.ErrorMessage();
		}
	}
	else
	{
		UpdateAirport();
	}	
}

void CAirportDatabase::DeleteAirport()
{
	CString strSQL(_T(""));
	strSQL.Format(_T("DELETE * FROM TB_PROJECT_AIRPORTDB WHERE ID = %d"),m_ID);
	try
	{
		CDatabaseADOConnetion::ExecuteSQLStatement(strSQL,GetAirportConnection()) ;
	}
	catch (CADOException& e)
	{
		e.ErrorMessage();
	}
}

void CAirportDatabase::UpdateAirport()
{
	CString strSQL(_T(""));

	strSQL.Format(_T("UPDATE TB_PROJECT_AIRPORTDB SET AIRPORTDB_NAME = '%s',VERSION = %d WHERE ID = %d"),m_strName,AIRPORTDB_VERSION,m_ID);
	try
	{
		CDatabaseADOConnetion::ExecuteSQLStatement(strSQL,GetAirportConnection());
	}
	catch (CADOException& e)
	{
		e.ErrorMessage();
	}
}

bool CAirportDatabase::loadDatabase()
{
	clearData();

	if(!PathFileExists(m_strDataFilePath))
	{
		CString strError(_T(""));
		strError.Format(_T("%s can not find "),m_strName);
		MessageBox(NULL,strError,"Warning",MB_OK);
		return false;
	}
		
	m_pAcMan = new CACTypesManager(this);
	m_pAcMan->loadDatabase(this);

 	m_pACDoorMan = new ACTypeDoorManager(m_pAcMan);
// 	m_pACDoorMan->loadDatabase(this);

	m_pAirlineMan = new CAirlinesManager(this);
	m_pAirlineMan->loadDatabase(this);

	m_pAirportMan = new CAirportsManager(this);
	m_pAirportMan->loadDatabase(this);

	m_pFlightMan = new FlightManager();
	m_pFlightMan->loadDatabase(this);

	//load aircraft component model data
	m_pACCompModelDB = new CAircraftComponentModelDatabase(this);
	m_pACCompModelDB->loadDatabase(this);

	m_pAircraftModeList = new CAircraftModelDatabase(this);
	m_pAircraftModeList->loadDatabase(this);

	m_pProbDistMan = new CProbDistManager();
	m_pProbDistMan->loadDatabase(this);

	m_pAircraftFurnishingMan = new CAircraftFurnishingSectionManager(this);
	m_pAircraftFurnishingMan->loadDatabase(this);
	return true;
}


bool CAirportDatabase::loadFromOtherDatabase(CAirportDatabase* pOtherDatabase)
{
	clearData();

	m_pAcMan = new CACTypesManager(this);
	m_pAcMan->loadDatabase(pOtherDatabase);

	m_pACDoorMan = new ACTypeDoorManager(m_pAcMan);
//	m_pACDoorMan->loadDatabase(pOtherDatabase);

	m_pAirlineMan = new CAirlinesManager(this);
	m_pAirlineMan->loadDatabase(pOtherDatabase);

	m_pAirportMan = new CAirportsManager(this);
	m_pAirportMan->loadDatabase(pOtherDatabase);

	m_pFlightMan = new FlightManager();
	m_pFlightMan->loadDatabase(pOtherDatabase);

	//load aircraft component model data from file
 	m_pACCompModelDB = new CAircraftComponentModelDatabase(this);
 	m_pACCompModelDB->loadDatabase(pOtherDatabase);

	m_pAircraftModeList = new CAircraftModelDatabase(this);
	m_pAircraftModeList->loadDatabase(this);


	m_pProbDistMan = new CProbDistManager();
	m_pProbDistMan->loadDatabase(pOtherDatabase);
	return true;	
}

//save the data to new version database
bool CAirportDatabase::saveDatabase()
{
	CString strShareTemplate(_T(""));
	strShareTemplate.Format(_T("%s\\ArctermDB\\temp\\project.mdb"),PROJMANAGER->GetAppPath());
	if (!PathFileExists(m_strDataFilePath))
	{
		CopyFile(strShareTemplate,m_strDataFilePath,TRUE);
	}
	

	try
	{
		SaveAirport();
		m_pAcMan->saveDatabase(this);
		m_pACDoorMan->saveDatabase(this);
		m_pAirlineMan->saveDatabase(this);
		m_pAirportMan->saveDatabase(this);
		m_pFlightMan->saveDatabase(this);
		m_pProbDistMan->saveDatabase(this);

		if (m_pAircraftFurnishingMan == NULL)
		{
			m_pAircraftFurnishingMan =  new CAircraftFurnishingSectionManager(this);
		}
		m_pAircraftFurnishingMan->saveDatabase(this);
	}
	catch (CADOException& e)
	{
		CString sError(_T(""));
		sError.Format("%s",e.ErrorMessage());
		MessageBox(NULL,sError,"Warning",MB_OK);
		return false;
	}
	return true;
}

bool CAirportDatabase::saveAsTemplateDatabase(CAirportDatabase* pOtherDatabase)
{
	ASSERT(pOtherDatabase);
	pOtherDatabase->GetAirportConnection()->CloseConnection();
	m_pConnectionPtr->CloseConnection();
	BOOL bSucessed = CopyFile(m_strDataFilePath,pOtherDatabase->getDatabaseFilePath(),FALSE);
	return true;
}

bool CAirportDatabase::loadDataFromFile()
{
	clearData();
	CString _commonPath ;
	_commonPath.Format(_T("%s\\%s"),PROJMANAGER->GetAppPath(),"Databases") ;
	m_pAcMan = new CACTypesManager(this);
	m_pAcMan->SetCommonDBPath(_commonPath) ;
	//	m_pAcMan->SetProjectVersion(m_proVersion) ;
	m_pAcMan->LoadData( m_strFolder);

	CString _doorcommonPath ;
	_doorcommonPath.Format(_T("%s\\%s"),PROJMANAGER->GetAppPath(),"ArctermDB\\CommonAirportDB") ;

	m_pACDoorMan = new ACTypeDoorManager(m_pAcMan);
	m_pACDoorMan->ReadCommonACDoors(_doorcommonPath);

	m_pAirlineMan = new CAirlinesManager(this);
	m_pAirlineMan->SetCommonDBPath(_commonPath) ;
	m_pAirlineMan->LoadData( m_strFolder );

	m_pAirportMan = new CAirportsManager(this);
	m_pAirportMan->SetCommonDBPath(_commonPath) ;
	m_pAirportMan->LoadData( m_strFolder );

	m_pFlightMan = new FlightManager();
	m_pFlightMan->LoadData(m_strFolder);

	//load aircraft component model data from file
 	m_pACCompModelDB = new CAircraftComponentModelDatabase(this);
// 	m_pACCompModelDB->LoadData(m_strFolder);

	m_pProbDistMan = new CProbDistManager();
	try
	{
		m_pProbDistMan->loadDataSetFromOtherFile( m_strFolder + "\\" + _strProbDistFileName );
	}
	catch (FileVersionTooNewError* e) 
	{
		CString _strErrMsg;
		e->getMessage( _strErrMsg.GetBuffer(64) );
		AfxMessageBox( _strErrMsg, MB_OK|MB_ICONINFORMATION );

		delete e;
		_strErrMsg.ReleaseBuffer();
	}

	if(m_proVersion <= 301)
	{
		CShareTemplateDatabase* AirportDB = AIRPORTDBLIST->getAirportDBByName(_T("DEFAULTDB")) ;
		ASSERT(AirportDB) ;
		std::vector<CACType*> _OutData ;
		std::vector<CACType*> _FindData ;
		CACType* _LocalActype = NULL ;
		CACType* _DefaultActype = NULL ;
		if(AirportDB != NULL)
		{
			std::vector<CACType*>  actype ;
			CACType::ReadDataFromDB(actype,AirportDB->GetID(),DATABASESOURCE_TYPE_ACCESS_GLOBALDB) ;
			for (int i = 0 ; i < (int)getAcMan()->getACTypeCount() ;i++)
			{
				_LocalActype = getAcMan()->getACTypeItem(i) ;
				for (int ndx = 0 ; ndx < (int)actype.size() ;ndx++)
				{
					if( *_LocalActype == * (actype[ndx]) )
					{
						_LocalActype->m_dCabinWidth = actype[ndx]->m_dCabinWidth ;

					}
				}
			}

		}
		//load default door msg
		CString _commonPath ;
		_commonPath.Format(_T("%s\\%s"),PROJMANAGER->GetAppPath(),"ArctermDB\\CommonAirportDB") ;
		getAcDoorMan()->ReadCommonACDoors(_commonPath) ;

	}

	return true;
}

bool CAirportDatabase::loadPreVersionData(DATABASESOURCE_TYPE type)
{
	clearData() ;
	CString _commonPath ;
	_commonPath.Format(_T("%s\\%s"),PROJMANAGER->GetAppPath(),"ArctermDB\\CommonAirportDB") ;

	m_pAcMan = new CACTypesManager(this);
	m_pAcMan->SetDBPath(m_strFolder) ;
	m_pAcMan->SetCommonDBPath(_commonPath) ;
	m_pAcMan->ReadDataFromDB(GetID(),type);
	m_pAcMan->SetID(GetID()) ;

	m_pACDoorMan = new ACTypeDoorManager(m_pAcMan);
	m_pACDoorMan->ReadDataFromDB(GetID(),type);

	m_pAirlineMan = new CAirlinesManager(this);
	m_pAirlineMan->SetCommonDBPath(_commonPath) ;
	m_pAirlineMan->ReadDataFromDB(GetID(),type);
	m_pAirlineMan->SetID(GetID());

	m_pAirportMan = new CAirportsManager(this);
	m_pAirportMan->SetCommonDBPath(_commonPath) ;
	m_pAirportMan->ReadDataFromDB(GetID(),type);
	m_pAirportMan->SetID(GetID()) ;

	m_pFlightMan = new FlightManager();
	m_pFlightMan->ReadDataFromDB(GetID(),type);

	//load aircraft component model data from file
 	m_pACCompModelDB = new CAircraftComponentModelDatabase(this);
 	//m_pACCompModelDB->loadDatabase(this);

	m_pProbDistMan = new CProbDistManager();
	m_pProbDistMan->ReadDataFromDB(GetID(),type);

	if(m_nVersion <= 1000)
	{
		CShareTemplateDatabase* AirportDB = AIRPORTDBLIST->getAirportDBByName(_T("DEFAULTDB")) ;
		ASSERT(AirportDB) ;
		std::vector<CACType*> _OutData ;
		std::vector<CACType*> _FindData ;
		CACType* _LocalActype = NULL ;
		CACType* _DefaultActype = NULL ;
		if(AirportDB != NULL)
		{
			std::vector<CACType*>  actype ;
			CACType::ReadDataFromDB(actype,AirportDB->GetID(),DATABASESOURCE_TYPE_ACCESS_GLOBALDB) ;
			for (int i = 0 ; i < (int)getAcMan()->getACTypeCount() ;i++)
			{
				_LocalActype = getAcMan()->getACTypeItem(i) ;
				for (int ndx = 0 ; ndx < (int)actype.size() ;ndx++)
				{
					if( *_LocalActype == * (actype[ndx]) )
					{
						_LocalActype->m_dCabinWidth = actype[ndx]->m_dCabinWidth ;
						_LocalActype->IsEdit(TRUE) ;

					}
				}
			}

		}
		//load default door msg
		CString _commonPath ;
		_commonPath.Format(_T("%s\\%s"),PROJMANAGER->GetAppPath(),"ArctermDB\\CommonAirportDB") ;
		getAcDoorMan()->ReadCommonACDoors(_commonPath) ;
		getAcMan()->WriteDataToDB(GetID(),type) ;
		getAcDoorMan()->WriteDataToDB(GetID(),type) ;
		m_nVersion = AIRPORTDB_VERSION ;
		WriteAiportDataBase(type) ;
	}
	return true;
}
//read old version database, loadDatabase is  method for reading current version database
void CAirportDatabase::LoadDataFromDB(DATABASESOURCE_TYPE type)
{
	loadPreVersionData(type);

}

void CAirportDatabase::SaveDataToDB(DATABASESOURCE_TYPE type)
{

}

void CAirportDatabase::ReadAirportDataBase(DATABASESOURCE_TYPE type)
{
	CString SQL ;
	SQL.Format(_T("SELECT * FROM TB_PROJECT_AIRPORTDB ")) ;
	CADORecordset recordset ;
	long  count = 0 ;
	try
	{
		CADODatabase::ExecuteSQLStatement(SQL,count,recordset,type) ;
	}
	catch (CADOException e)
	{
		e.ErrorMessage() ;
		return ;
	}
	CString DBName ;
	int id = -1 ;
	while(!recordset.IsEOF())
	{
		recordset.GetFieldValue(_T("ID"),id) ;
		SetID(id) ;

		recordset.GetFieldValue(_T("AIRPORTDB_NAME"),DBName) ;
		setName(DBName) ;

		if(recordset.GetFieldValue(_T("VERSION"),id))
			m_nVersion = id ;
		else
			m_nVersion = 0 ;

		break ;
	}
}

void CAirportDatabase::WriteAiportDataBase(DATABASESOURCE_TYPE type)
{
	CString SQL ;
	try
	{
		if(GetID() == -1)
		{
			SQL.Format(_T("INSERT INTO TB_PROJECT_AIRPORTDB (AIRPORTDB_NAME,VERSION) VALUES('%s',%d) "),getName(),AIRPORTDB_VERSION) ;
			int id = CADODatabase::ExecuteSQLStatementAndReturnScopeID(SQL,type) ;
			SetID(id) ;
		}
		else
		{
			SQL.Format(_T("UPDATE TB_PROJECT_AIRPORTDB SET AIRPORTDB_NAME = '%s',VERSION = %d WHERE ID = %d"),getName(),AIRPORTDB_VERSION,GetID()) ;
			CADODatabase::ExecuteSQLStatement(SQL,type) ;
		}
	}
	catch (CADOException e)
	{
		e.ErrorMessage() ;
		return ;
	}
}
void CAirportDatabase::ResetAllID()
{
	if (getAcMan())
	{
		getAcMan()->ReSetAllID();
	}
	
	if (getAirlineMan())
	{
		getAirlineMan()->ReSetAllID();
	}
	
	if (getAirportMan())
	{
		getAirportMan()->ReSetAllID();
	}
	
	if (getProbDistMan())
	{
		getProbDistMan()->ResetAllID();
	}
	
	if (getFlightMan())
	{
		getFlightMan()->ResetAllID();
	}
}

bool CAirportDatabase::deleteDatabase()
{
	if(!hadLoadDatabase())
		return false;
	m_pAcMan->deleteDatabase(this);
	m_pAirlineMan->deleteDatabase(this);
	m_pAirportMan->deleteDatabase(this);
	m_pFlightMan->deleteDatabase(this);
	m_pProbDistMan->deleteDatabase(this);

	return true;
}

void CAirportDatabase::DeleteAllData(DATABASESOURCE_TYPE type)
{
	if(!hadLoadDatabase())
		return ;
	m_pAcMan->DeleteAllDataFromDB(GetID(),type) ;
	m_pAirlineMan->DeleteAllDataFromDB(GetID(),type) ;
	m_pAirportMan->DeleteAllDataFromDB(GetID(),type) ;
	m_pFlightMan->DeleteAllData(GetID(),type) ;
	m_pProbDistMan->DeleteAllDataFromDB(GetID(),type) ;
}

BOOL CAirportDatabase::CheckTheNameIsAvailably(const CString& _name ,CString& _errChar)
{
	_errChar.Format(_T("%s"),_T("-")) ;
	if(_name.Find(_T('-'),0) != -1) 
	{
		_errChar.Format(_T("%s") , _T("The name contains Key Word \'-\' of system,Please replace with other characters in the defined dialog!"))  ;
		return FALSE  ;
	}
	else
		return TRUE ;
}

bool CAirportDatabase::hadLoadDatabase()const
{
	return m_pAirlineMan!=NULL && m_pAcMan!=NULL && m_pAirportMan!= NULL && m_pProbDistMan!=NULL && m_pACCompModelDB!=NULL;
}

// free **Manager pointer 
void CAirportDatabase::clearData()
{
	if( m_pAcMan )
	{
		delete m_pAcMan;
		m_pAcMan = NULL;
	}

	if( m_pAirlineMan )
	{
		delete m_pAirlineMan;
		m_pAirlineMan = NULL;
	}

	if( m_pAirportMan )
	{
		delete m_pAirportMan;
		m_pAirportMan = NULL;
	}

	if (m_pACDoorMan)
	{
		delete m_pACDoorMan;
		m_pACDoorMan = NULL;
	}

	if (m_pFlightMan)
	{
		delete m_pFlightMan;
		m_pFlightMan = NULL;
	}

	if( m_pProbDistMan )
	{
		delete m_pProbDistMan;
		m_pProbDistMan = NULL;
	}

	if(m_pACCompModelDB)
	{
		delete m_pACCompModelDB;
		m_pACCompModelDB = NULL;
	}

	if (m_pAircraftModeList)
	{
		delete m_pAircraftModeList;
		m_pAircraftModeList = NULL;
	}
}
/////////////////////////////////////////////////////////////////////////////////////////////
//CShareTemplateDatabase implement
CPartsTemplateDatabase::CPartsTemplateDatabase(const CString& strDatabaseFilePath)
{
	m_strDataFilePath = strDatabaseFilePath;
	m_pConnectionPtr = new AirportDatabaseConnection(strDatabaseFilePath);
}

CPartsTemplateDatabase::~CPartsTemplateDatabase()
{
	if (m_pConnectionPtr)
	{
		delete m_pConnectionPtr;
		m_pConnectionPtr = NULL;
	}
}

//bool CPartsTemplateDatabase::loadDataFromFile()
//{
//	bool bSucceed = false ;
//	bSucceed = CAirportDatabase::loadDataFromFile();
//	m_pAircraftModeList = new CAircraftModelList;
//	m_pAircraftModeList->ReadData(GetID());
//	if(m_pAcMan != NULL)
//		m_pAcMan->InitRelationShip();
//	return 	bSucceed;
//}
//
//bool CPartsTemplateDatabase::loadPreVersionData(DATABASESOURCE_TYPE type)
//{
//	CAirportDatabase::LoadDataFromDB(type);
//	m_pAircraftModeList = new CAircraftModelList;
//	m_pAircraftModeList->ReadData(GetID());
//	if(m_pAcMan != NULL)
//		m_pAcMan->InitRelationShip();
//	return true;
//}

void CPartsTemplateDatabase::loadFromFile( ArctermFile& file )
{
	file.getLine();
	file.getInteger( m_lIndex );
	file.getField( m_strName.GetBuffer( 128 ), 128 );
	file.getField( m_strFolder.GetBuffer( 128), 128 );
	m_strName.ReleaseBuffer();
	m_strFolder.ReleaseBuffer();

	if (m_strFolder.Find(_T("\\")) < 0)
	{
		m_strFolder = PROJMANAGER->GetAppPath()+"\\ImportDb\\" + m_strFolder;
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////
//CPartsDatabase implement
CPartsDatabase::CPartsDatabase(const CString& strDatabaseFilePath)
:CAirportDatabase()
{
	m_strDataFilePath = strDatabaseFilePath;
	m_pConnectionPtr = new AirportDatabaseConnection(strDatabaseFilePath);
}

CPartsDatabase::~CPartsDatabase()
{
	if (m_pConnectionPtr)
	{
		delete m_pConnectionPtr;
		m_pConnectionPtr = NULL;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////
//CARCDatabase implement
CARCDatabase::CARCDatabase(const CString& strDatabaseFilePath)
{
	m_strDataFilePath = strDatabaseFilePath;
	m_pConnectionPtr = new AirportDatabaseConnection(strDatabaseFilePath);
}

CARCDatabase::~CARCDatabase()
{
	if (m_pConnectionPtr)
	{
		delete m_pConnectionPtr;
		m_pConnectionPtr = NULL;
	}
}

bool CARCDatabase::loadCommonDatabase()
{
	clearData();

	m_pAcMan = new CACTypesManager(this);
	m_pAcMan->loadARCDatabase(this);

	m_pAirlineMan = new CAirlinesManager(this);
	m_pAirlineMan->loadARCDatabase(this);


	m_pAirportMan = new CAirportsManager(this);
	m_pAirportMan->loadARCDatabase(this);;

	ResetAllID();
	return true;
}

CAirline* CARCDatabase::GetArcDatabaseAirline(const CString& sAirline)
{
	return getAirlineMan()->GetAirlineByName(sAirline);
}

CAirport* CARCDatabase::GetArcDatabaseAirport(const CString& sAirport)
{
	return getAirportMan()->GetAiportByName(sAirport);
}

CACType* CARCDatabase::GetArcDatabaseActype(const CString& sActype)
{
	return getAcMan()->getACTypeItem(sActype);
}
////////////////////////////////////////////////////////////////////////////////////////////
//CProjectDabase implement
CARCProjectDatabase::CARCProjectDatabase(const CString& strDatabaseFilePath,const CString& strProjectDatabaseFilePath)
:m_pARCDatabase(NULL)
,m_pPartsDatabase(NULL)
{
	m_pARCDatabase = new CARCDatabase(strDatabaseFilePath);
	m_pPartsDatabase = new CPartsDatabase(strProjectDatabaseFilePath);
}

CARCProjectDatabase::~CARCProjectDatabase()
{
	if (m_pARCDatabase)
	{
		delete m_pARCDatabase;
		m_pARCDatabase = NULL;
	}

	if (m_pPartsDatabase)
	{
		delete m_pPartsDatabase;
		m_pPartsDatabase = NULL;
	}
}

bool CARCProjectDatabase::loadDatabase()
{
	m_pPartsDatabase->loadDatabase();
	m_pARCDatabase->loadCommonDatabase();
	ACALIASMANAGER->SetCACTypeManager(getAcMan());
	ACALIASMANAGER->readData(this);
	return true;
}


bool CARCProjectDatabase::saveDatabase()
{
	m_pPartsDatabase->saveDatabase();
	return true;
}

bool CARCProjectDatabase::deleteDatabase()
{
	return m_pPartsDatabase->deleteDatabase();
}

bool CARCProjectDatabase::loadPreVersionData(DATABASESOURCE_TYPE type)
{
	m_pARCDatabase->loadCommonDatabase();

	return m_pPartsDatabase->loadPreVersionData(type);
}

bool CARCProjectDatabase::loadDataFromFile()
{
	m_pARCDatabase->loadCommonDatabase();
	return m_pPartsDatabase->loadDataFromFile();
}


bool CARCProjectDatabase::loadFromOtherDatabase(CAirportDatabase* pPartsTemplate)
{
	m_pARCDatabase->loadCommonDatabase();

	if(m_pPartsDatabase->loadFromOtherDatabase(pPartsTemplate))
	{
		ResetAllID();
		SetID(-1);
		return true;
	}

	return false;
}

bool CARCProjectDatabase::saveAsTemplateDatabase(CAirportDatabase* pOtherDatabase)
{
	return m_pPartsDatabase->saveAsTemplateDatabase(pOtherDatabase);
}

void CARCProjectDatabase::SetID(int _id)
{
	m_pPartsDatabase->SetID(_id);
}

int CARCProjectDatabase::GetID()const
{
	return m_pPartsDatabase->GetID();
}

void CARCProjectDatabase::SetProjectVersion(int _proVer)
{
	m_pPartsDatabase->SetProjectVersion(_proVer);
}

void CARCProjectDatabase::setIndex( long _idx )
{
	m_pPartsDatabase->setIndex(_idx);
}

long CARCProjectDatabase::getIndex()const
{
	return m_pPartsDatabase->getIndex();
}

void CARCProjectDatabase::setName( const CString& _strName )
{
	m_pPartsDatabase->setName(_strName);
}

const CString& CARCProjectDatabase::getName()const
{
	return m_pPartsDatabase->getName();
}

void CARCProjectDatabase::setFolder( const CString& _strFolder )
{
	m_pPartsDatabase->setFolder(_strFolder);
}

const CString& CARCProjectDatabase::getFolder()const
{
	return m_pPartsDatabase->getFolder();
}

ACTypeDoorManager* CARCProjectDatabase::getAcDoorMan()const
{
	return m_pPartsDatabase->getAcDoorMan();
}

CProbDistManager* CARCProjectDatabase::getProbDistMan()const
{
	return m_pPartsDatabase->getProbDistMan();
}

CAircraftComponentModelDatabase* CARCProjectDatabase::getACCompModelDB()const
{
	return m_pPartsDatabase->getACCompModelDB();
}

FlightManager* CARCProjectDatabase::getFlightMan()const
{
	return m_pPartsDatabase->getFlightMan();
}

CAircraftModelDatabase* CARCProjectDatabase::getModeList()const
{
	return m_pPartsDatabase->getModeList();
}

CAircraftFurnishingSectionManager* CARCProjectDatabase::getFurnishingMan()const
{
	return m_pPartsDatabase->getFurnishingMan();
}

CAirline* CARCProjectDatabase::GetArcDatabaseAirline(const CString& sAirline)
{
	return m_pARCDatabase->GetArcDatabaseAirline(sAirline);
}

CAirport* CARCProjectDatabase::GetArcDatabaseAirport(const CString& sAirport)
{
	return m_pARCDatabase->GetArcDatabaseAirport(sAirport);
}

CACType* CARCProjectDatabase::GetArcDatabaseActype(const CString& sActype)
{
	return m_pARCDatabase->GetArcDatabaseActype(sActype);
}

CACTypesManager* CARCProjectDatabase::getAcMan(void)
{
	if (m_pAcMan)
	{
		return m_pAcMan;
	}

	std::vector<CACType*> actypeList;
	m_pAcMan = m_pPartsDatabase->getAcMan();
	if (m_pAcMan == NULL)
	{
		return m_pAcMan;
	}

	for (int i = 0; i < m_pARCDatabase->getAcMan()->getACTypeCount(); i++)
	{
		CACType* pACType = m_pARCDatabase->getAcMan()->GetACTypeList()->at(i);
		if (!m_pAcMan->FindActypeByIATACode(pACType->GetIATACode(),actypeList))
		{
			CACType* pData = new CACType(*pACType);
			pData->IsEdit(TRUE);
			m_pAcMan->GetACTypeList()->push_back(pData);
		}
	}
	m_pAcMan->ClacCategoryACTypeList();
	return m_pAcMan;
}

void CARCProjectDatabase::loadAirlineFromDatabase(CAirportDatabase* pOtherDatabase)
{
	m_pAirlineMan = NULL;
	m_pPartsDatabase->getAirlineMan()->loadDatabase(pOtherDatabase);
	m_pARCDatabase->getAirlineMan()->loadARCDatabase(m_pARCDatabase);
	getAirlineMan();
}

void CARCProjectDatabase::loadAircraftFromDatabase(CAirportDatabase* pOtherDatabase)
{
	m_pAcMan = NULL;
	m_pPartsDatabase->getAcMan()->loadDatabase(pOtherDatabase);
	m_pARCDatabase->getAcMan()->loadARCDatabase(m_pARCDatabase);
	getAcMan();
}

void CARCProjectDatabase::loadAirportFromDatabase(CAirportDatabase* pOtherDatabase)
{
	m_pAirportMan = NULL;
	m_pPartsDatabase->getAirportMan()->loadDatabase(pOtherDatabase);
	m_pARCDatabase->getAirportMan()->loadARCDatabase(m_pARCDatabase);
	getAirportMan();
}

CAirlinesManager* CARCProjectDatabase::getAirlineMan(void)
{
	if (m_pAirlineMan)
	{
		return m_pAirlineMan;
	}

	m_pAirlineMan = m_pPartsDatabase->getAirlineMan();
	if (m_pAirlineMan == NULL)
	{
		return m_pAirlineMan;
	}

	for (size_t i = 0; i < m_pARCDatabase->getAirlineMan()->GetAirlineList()->size(); i++)
	{
		CAirline* pAirline = m_pARCDatabase->getAirlineMan()->GetAirlineList()->at(i);
		if (!m_pAirlineMan->IfExist(pAirline->m_sAirlineIATACode))
		{
			CAirline* pData = new CAirline(*pAirline);
			pData->IsEdit(TRUE);
			m_pAirlineMan->GetAirlineList()->push_back(pData);
		}
	}
	return m_pAirlineMan;
}

bool CARCProjectDatabase::readAirport()
{
	return m_pPartsDatabase->readAirport();
}

CAirportsManager* CARCProjectDatabase::getAirportMan(void)
{
	if (m_pAirportMan)
	{
		return m_pAirportMan;
	}

	m_pAirportMan = m_pPartsDatabase->getAirportMan();
	if (m_pAirportMan == NULL)
	{
		return m_pAirportMan;
	}

	for (size_t i  = 0; i < m_pARCDatabase->getAirportMan()->GetAirportList()->size(); i++)
	{
		CAirport* pAirport = m_pARCDatabase->getAirportMan()->GetAirportList()->at(i);
		if(!m_pAirportMan->IsExist(pAirport))
		{
			CAirport* pData = new CAirport(*pAirport);
			m_pAirportMan->GetAirportList()->push_back(pData);
			pData->IsEdit(TRUE);
		}
	}
	return m_pAirportMan;
}

bool CARCProjectDatabase::hadLoadDatabase()const
{
	return m_pPartsDatabase->hadLoadDatabase();
}

void CARCProjectDatabase::ResetAllID()
{
	m_pPartsDatabase->ResetAllID();
}
////////////////////////////////////////////////////////////////////////////////////////////
//CShareTemplateDatabase implement
CShareTemplateDatabase::CShareTemplateDatabase(const CString& strARCDatabasePath,const CString& strShareTemplateDatabasePath)
{
	m_pARCDatabase = new CARCDatabase(strARCDatabasePath);
	m_pPartsTemplateDatabase = new CPartsTemplateDatabase(strShareTemplateDatabasePath);
}

CShareTemplateDatabase::~CShareTemplateDatabase()
{
	if (m_pARCDatabase)
	{
		delete m_pARCDatabase;
		m_pARCDatabase = NULL;
	}

	if (m_pPartsTemplateDatabase)
	{
		delete m_pPartsTemplateDatabase;
		m_pPartsTemplateDatabase = NULL;
	}
}

bool CShareTemplateDatabase::loadDatabase()
{
	m_pPartsTemplateDatabase->loadDatabase();
	m_pARCDatabase->loadCommonDatabase();
	return true;
}

bool CShareTemplateDatabase::saveDatabase()
{
	m_pPartsTemplateDatabase->saveDatabase();
	return true;
}

bool CShareTemplateDatabase::deleteDatabase()
{
	return m_pPartsTemplateDatabase->deleteDatabase();
}

void CShareTemplateDatabase::SetProjectVersion(int _proVer)
{
	m_pPartsTemplateDatabase->SetProjectVersion(_proVer);
}

void CShareTemplateDatabase::setIndex( long _idx )
{
	m_pPartsTemplateDatabase->setIndex(_idx);
}

long CShareTemplateDatabase::getIndex()const
{
	return m_pPartsTemplateDatabase->getIndex();
}

int CShareTemplateDatabase::GetID()const
{
	return m_pPartsTemplateDatabase->GetID();
}

void CShareTemplateDatabase::SetID(int _id)
{
	m_pPartsTemplateDatabase->SetID(_id);
}

void CShareTemplateDatabase::setName( const CString& _strName )
{
	m_pPartsTemplateDatabase->setName(_strName);
}

const CString& CShareTemplateDatabase::getName()const
{
	return m_pPartsTemplateDatabase->getName();
}

void CShareTemplateDatabase::setFolder( const CString& _strFolder )
{
	m_pPartsTemplateDatabase->setFolder(_strFolder);
}

const CString& CShareTemplateDatabase::getFolder()const
{
	return m_pPartsTemplateDatabase->getFolder();
}

ACTypeDoorManager* CShareTemplateDatabase::getAcDoorMan()const
{
	return m_pPartsTemplateDatabase->getAcDoorMan();
}

CProbDistManager* CShareTemplateDatabase::getProbDistMan()const
{
	return m_pPartsTemplateDatabase->getProbDistMan();
}

CAircraftComponentModelDatabase* CShareTemplateDatabase::getACCompModelDB()const
{
	return m_pPartsTemplateDatabase->getACCompModelDB();
}

FlightManager* CShareTemplateDatabase::getFlightMan()const
{
	return m_pPartsTemplateDatabase->getFlightMan();
}

CAircraftModelDatabase* CShareTemplateDatabase::getModeList()const
{
	return m_pPartsTemplateDatabase->getModeList();
}

CAircraftFurnishingSectionManager* CShareTemplateDatabase::getFurnishingMan()const
{
	return m_pPartsTemplateDatabase->getFurnishingMan();
}

CAirline* CShareTemplateDatabase::GetArcDatabaseAirline(const CString& sAirline)
{
	return m_pARCDatabase->GetArcDatabaseAirline(sAirline);
}

CAirport* CShareTemplateDatabase::GetArcDatabaseAirport(const CString& sAirport)
{
	return m_pARCDatabase->GetArcDatabaseAirport(sAirport);
}

CACType* CShareTemplateDatabase::GetArcDatabaseActype(const CString& sActype)
{
	return m_pARCDatabase->GetArcDatabaseActype(sActype);
}

CACTypesManager* CShareTemplateDatabase::getAcMan(void)
{
	if (m_pAcMan)
	{
		return m_pAcMan;
	}

	std::vector<CACType*> actypeList;
	m_pAcMan = m_pPartsTemplateDatabase->getAcMan();
	if (m_pAcMan == NULL)
	{
		return m_pAcMan;
	}

	for (int i = 0; i < m_pARCDatabase->getAcMan()->getACTypeCount(); i++)
	{
		CACType* pACType = m_pARCDatabase->getAcMan()->GetACTypeList()->at(i);
		if (!m_pAcMan->FindActypeByIATACode(pACType->GetIATACode(),actypeList))
		{
			CACType* pData = new CACType(*pACType);
			pData->IsEdit(TRUE);
			m_pAcMan->GetACTypeList()->push_back(pData);
		}
	}
	return m_pAcMan;
}

CAirlinesManager* CShareTemplateDatabase::getAirlineMan(void)
{
	if (m_pAirlineMan)
	{
		return m_pAirlineMan;
	}

	m_pAirlineMan = m_pPartsTemplateDatabase->getAirlineMan();
	if (m_pAirlineMan == NULL)
	{
		return m_pAirlineMan;
	}

	for (size_t i = 0; i < m_pARCDatabase->getAirlineMan()->GetAirlineList()->size(); i++)
	{
		CAirline* pAirline = m_pARCDatabase->getAirlineMan()->GetAirlineList()->at(i);
		if (!m_pAirlineMan->IfExist(pAirline->m_sAirlineIATACode))
		{
			CAirline* pData = new CAirline(*pAirline);
			pData->IsEdit(TRUE);
			m_pAirlineMan->GetAirlineList()->push_back(pData);
		}
	}
	return m_pAirlineMan;
}

CAirportsManager* CShareTemplateDatabase::getAirportMan(void)
{
	if (m_pAirportMan)
	{
		return m_pAirportMan;
	}

	m_pAirportMan = m_pPartsTemplateDatabase->getAirportMan();
	if (m_pAirportMan == NULL)
	{
		return m_pAirportMan;
	}

	for (size_t i  = 0; i < m_pARCDatabase->getAirportMan()->GetAirportList()->size(); i++)
	{
		CAirport* pAirport = m_pARCDatabase->getAirportMan()->GetAirportList()->at(i);
		if(!m_pAirportMan->IsExist(pAirport))
		{
			CAirport* pData = new CAirport(*pAirport);
			pData->IsEdit(TRUE);
			m_pAirportMan->GetAirportList()->push_back(pData);
		}
	}
	return m_pAirportMan;
}

bool CShareTemplateDatabase::loadFromOtherDatabase(CAirportDatabase* pOtherDatabase)
{
	m_pARCDatabase->loadCommonDatabase();//ARC database should not call loadDatabase

	if(m_pPartsTemplateDatabase->loadFromOtherDatabase(pOtherDatabase))
	{
		ResetAllID();
		SetID(-1);
		return true;
	}

	return false;
}

bool CShareTemplateDatabase::saveAsTemplateDatabase(CAirportDatabase* pOtherDatabase)
{
	ASSERT(pOtherDatabase);
	return m_pPartsTemplateDatabase->saveAsTemplateDatabase(pOtherDatabase);
}

bool CShareTemplateDatabase::hadLoadDatabase()const
{
	return m_pPartsTemplateDatabase->hadLoadDatabase();
}

void CShareTemplateDatabase::ResetAllID()
{
	m_pPartsTemplateDatabase->ResetAllID();
}
//for old version database load///////////////////////////////////////////////////////////
bool CShareTemplateDatabase::loadDataFromFile()
{
	m_pARCDatabase->loadCommonDatabase();//init arc common
	return m_pPartsTemplateDatabase->loadDataFromFile();
}


bool CShareTemplateDatabase::loadPreVersionData(DATABASESOURCE_TYPE type)
{
	m_pARCDatabase->loadCommonDatabase();//init arc common
	return m_pPartsTemplateDatabase->loadPreVersionData(type);
}

void CShareTemplateDatabase::LoadDataFromDB(DATABASESOURCE_TYPE type /* = DATABASESOURCE_TYPE_ACCESS */)
{	
	m_pARCDatabase->loadCommonDatabase();//init arc common
	m_pPartsTemplateDatabase->LoadDataFromDB(type);
}

void CShareTemplateDatabase::loadFromFile( ArctermFile& file )
{
	m_pPartsTemplateDatabase->loadFromFile(file);
}
/////////////////should no more modified////////////////////////////////////////////////////
