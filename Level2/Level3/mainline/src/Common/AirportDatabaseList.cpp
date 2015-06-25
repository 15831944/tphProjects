// AirportDatabaseList.cpp: implementation of the CAirportDatabaseList class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "AirportDatabaseList.h"
#include "airportdatabase.h"
#include "projectmanager.h"
#include "common\fileman.h"
#include "assert.h"
#include "../common/exeption.h"
#include "../Common/ProgressBar.h"
#include "../Common/ACTypesManager.h"
#include "../Common/ACTypeDoor.h"
#include "Database/ARCportADODatabase.h"
#include "../Common/AirportDatabase.h"
#include "../Database/PartsAndShareTemplateUpdate.h"
#include <Shlwapi.h>
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
#define DefaultAirportDBFolder "Databases"
#define CustomerAirportDBFolder "ImportDB"


const char keyBuff[] = {'/',':','*','?','"','<','>','|'};


//file name can not include keybuff character,get the position of the key word
int checkIfIncludeKeyWords(char* buff)
{
	CString strValue(buff);
	int nPos = -1;
	int nCount = sizeof(keyBuff)/sizeof(char);
	for (int i = 0; i < nCount; i++)
	{
		nPos = strValue.Find(keyBuff[i]);
		if (nPos != -1)
		{
			return nPos;
		}
	}
	return -1;
}

//CAirportDatabaseList* CAirportDatabaseList::m_pInstance = NULL;

#define DB_FILE_COUNT 8
const CString __database_file[] =
{
	"acdata.acd",
		"category.cat",
		"Airlines.dat",
		"subairlines.sal",
		"Airports.dat",
		"Sector.sec",
		"probdist.pbd",
		"FlightGroup.frp"
};

#define DB_PATH_COUNT 0
const CString __database_path[] = 
{
	"MobImages",
		"Shapes"
};

CAirportDatabaseList::CAirportDatabaseList()
:DataSet( GlobalDBListFile,2.3f)
,m_lAirportDBIndex(0l)
{
}

CAirportDatabaseList::~CAirportDatabaseList()
{
	clear();
}


CAirportDatabaseList* CAirportDatabaseList::GetInstance( void )
{
	if( s_pTheInstance == NULL )
	{
		s_pTheInstance = Singleton<CAirportDatabaseList>::CreateTheInstance();
		s_pTheInstance->loadDataSet(CString(_T("")));
	}
	
	return s_pTheInstance;
}

void CAirportDatabaseList::ReleaseInstance( void )
{
	if( s_pTheInstance )
	{
		s_pTheInstance->saveDataSet(CString(_T("")), false );
		Singleton<CAirportDatabaseList>::ReleaseInstance();
	}

	s_pTheInstance = NULL;
}

// clear data structure before load data from the file.
void CAirportDatabaseList::clear()
{
	for( unsigned i=0; i<m_vAirportDB.size(); i++ )
	{
		delete m_vAirportDB[i];
	}
	m_vAirportDB.clear();
	clearDelAirport();

}

void CAirportDatabaseList::clearDelAirport()
{
	for( unsigned i=0; i<m_vDelAirportDB.size(); i++ )
	{
		delete m_vDelAirportDB[i];
	}
	m_vDelAirportDB.clear();
}
// Init Value in case no file can be read.
void CAirportDatabaseList::initDefaultValues()
{
	clear();
}

//read current airport information
void CAirportDatabaseList::readData (ArctermFile& p_file)
{
	CString strARCPath(_T(""));
	strARCPath.Format(_T("%s\\%s"),PROJMANAGER->GetAppPath(),_T("Databases\\arc.mdb"));

	int nCount = 0;
	p_file.getLine();
	p_file.getInteger(nCount);
	p_file.getLine();

	char buff[255];
	for (int i = 0; i < nCount; i++)
	{
		p_file.getField(buff,255);

		CString strSharePath(_T(""));
		strSharePath.Format(_T("%s\\ImportDb\\%s.mdb"),PROJMANAGER->GetAppPath(),buff);
		CShareTemplateDatabase* pARCportDatabase  = new CShareTemplateDatabase(strARCPath,strSharePath);
		pARCportDatabase->setName(buff);

		m_vAirportDB.push_back(pARCportDatabase);
		p_file.getLine();
	}

	updateShareTemplateDatabase();
}

bool CAirportDatabaseList::updateShareTemplateDatabase()
{
	for (int i = 0; i < (int)m_vAirportDB.size(); i++)
	{
		CShareTemplateDatabase* pShareTemplateDatabase = m_vAirportDB.at(i);
		CString strSharePath(_T(""));
		strSharePath.Format(_T("%s\\ImportDb\\%s.mdb"),PROJMANAGER->GetAppPath(),pShareTemplateDatabase->getName());
		if (PathFileExists(strSharePath))
		{
			/*CString strPartsSourceFile(_T(""));

			strPartsSourceFile.Format(_T("%s\\ArctermDB\\temp\\project.mdb"),PROJMANAGER->GetAppPath());
			CopyFile(strPartsSourceFile,strSharePath,FALSE);*/
			AirportDatabaseConnection pConnectionPtr(strSharePath);
			CPartsAndShareTemplateUpdate shareTemplateDataBaseUpdater;
			if(!shareTemplateDataBaseUpdater.Update(&pConnectionPtr)) 
			{
				CString strError(_T(""));
				strError.Format(_T("Update the Share Template DataBase error."));
				MessageBox(NULL,strError,"Warning",MB_OK);
				return false;
			}
		}
	}
	return true;
}

bool CAirportDatabaseList::ConvertAiportDatabaseToNewVersion()
{
	int nCount = (int)m_vAirportDB.size();
	CProgressBar bar(_T("Converting airport database..."),100,nCount,TRUE);

	for (int i = 0; i < (int)m_vAirportDB.size(); i++)
	{
		bar.StepIt();
		CShareTemplateDatabase* pShareTemplateDatabase = m_vAirportDB.at(i);
		pShareTemplateDatabase->loadPreVersionData(DATABASESOURCE_TYPE_ACCESS_GLOBALDB);
		pShareTemplateDatabase->SetID(-1);
		pShareTemplateDatabase->ResetAllID();
		pShareTemplateDatabase->saveDatabase();
	}
	return true;
}

//if version lower than the current version, rewrite
void CAirportDatabaseList::writeData (ArctermFile& p_file) const
{
	int nCount = (int)m_vAirportDB.size();
	p_file.writeInt(nCount);
	p_file.writeLine();

 	for (int i = 0; i < (int)m_vAirportDB.size(); i++)
 	{
 		CShareTemplateDatabase* pARCportDatabase = m_vAirportDB[i];
		p_file.writeField(pARCportDatabase->getName());
		p_file.writeLine();
 	}
}

///////////////Old version code, should not be modified//////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
double CAirportDatabaseList::GetGloabalVersion()
{
	CString SQL ;
	SQL.Format(_T("SELECT * FROM TB_DATABASE_VERSION")) ;
	CADORecordset recordset ;
	long count = -1 ;
	try
	{
		CADODatabase::ExecuteSQLStatement(SQL,count,recordset,DATABASESOURCE_TYPE_ACCESS_GLOBALDB) ;
	}
	catch (CADOException e)
	{
		e.ErrorMessage() ;
		return 1.0;
	}
	double version = 1.0 ; 
	while(!recordset.IsEOF())
	{
		recordset.GetFieldValue(_T("VERSION"),version) ;
		break ;
	}
	return version ;
}
void CAirportDatabaseList::SetGloabalVersion(double _version)
{
	CString SQL ;
	SQL.Format(_T("DELETE * FROM TB_DATABASE_VERSION")) ;
	try
	{
		CADODatabase::ExecuteSQLStatement(SQL,DATABASESOURCE_TYPE_ACCESS_GLOBALDB) ;

		SQL.Format(_T("INSERT INTO TB_DATABASE_VERSION (VERSION) VALUES(%f) "),_version) ;

		CADODatabase::ExecuteSQLStatement(SQL,DATABASESOURCE_TYPE_ACCESS_GLOBALDB) ;
	}
	catch (CADOException e)
	{
		e.ErrorMessage() ;
		return ;
	}
}

void CAirportDatabaseList::loadPreVersionData(ArctermFile& p_file)
{
///this code can not be modified,it is backward compatible
	ReadAirportDBData() ;
	int _iCount =0;

	p_file.getLine();

	p_file.getInteger( m_lAirportDBIndex );
	p_file.getInteger( _iCount );

	CString strSharePath(_T(""));
	CString strARCPath(_T(""));
	strARCPath.Format(_T("%s\\%s"),PROJMANAGER->GetAppPath(),_T("Databases\\arc.mdb"));

	for( int i=0; i< _iCount; i++ )
	{	
		CShareTemplateDatabase* _pDB = new CShareTemplateDatabase(strARCPath,strSharePath);
		_pDB->loadFromFile( p_file );
		strSharePath.Format(_T("%s\\ImportDb\\%s.mdb"),PROJMANAGER->GetAppPath(),_pDB->getName());
		m_vAirportDB.push_back( _pDB );
	}
	if(GetGloabalVersion() < GlobalDB_VERSION)
		CopyTheUpdateCommonFileToDefaultFolder() ;
	///copy data from file to DB 
	if(GetGloabalVersion() <= 1.0)
		CopyDataFromFileToDataBase() ;
	else
	{
		if(GetGloabalVersion() <= 1.2)
			ResetDefaultDataBase() ;

	}
//	updateShareTemplateDatabase();
//	ConvertAiportDatabaseToNewVersion();
///////////////////////////Old version code///////////////////////////////////////////
}

void CAirportDatabaseList::CopyTheUpdateCommonFileToDefaultFolder()
{
	CString sTempPath;

	sTempPath=PROJMANAGER->GetAppPath()+ "\\" +"ArctermDB\\CommonAirportDB";

	TCHAR sDefaultAirportDBPath[MAX_PATH]={_T('\0')};
	TCHAR sDestAirportDBPath[MAX_PATH]={_T('\0')};

	TCHAR sDestFullFileName[MAX_PATH]={_T('\0')};
	TCHAR sSourceFullFileName[MAX_PATH]={_T('\0')};

	sprintf(sDefaultAirportDBPath,"%s\\%s",PROJMANAGER->GetAppPath().GetString(),DefaultAirportDBFolder);

	if(!FileManager::IsDirectory(sTempPath) || !FileManager::IsDirectory(sDefaultAirportDBPath))
		return ;
	int nCount=sizeof(__database_file)/sizeof(CString);
	for(int i=0;i<nCount;i++)
	{
		sprintf(sDestFullFileName,"%s\\%s",sDefaultAirportDBPath,__database_file[i].GetString());
		sprintf(sSourceFullFileName,"%s\\%s",sTempPath.GetString(), __database_file[i].GetString());
		if(FileManager::IsFile(sSourceFullFileName))
			FileManager::CopyFile(sSourceFullFileName,sDestFullFileName,1);
	}
}
void CAirportDatabaseList::ResetDefaultDataBase()
{
	CAirportDatabase* AirportDB = getAirportDBByName(_T("DEFAULTDB")) ;
	if(AirportDB != NULL)
	{
		if(!AirportDB->hadLoadDatabase())
			AirportDB->LoadDataFromDB(DATABASESOURCE_TYPE_ACCESS_GLOBALDB) ;
		AirportDB->getAcMan()->DeleteAllDataFromDB(AirportDB->GetID(),DATABASESOURCE_TYPE_ACCESS_GLOBALDB);
		AirportDB->getAcMan()->ReadDataFromDB(AirportDB->GetID(),DATABASESOURCE_TYPE_ACCESS_GLOBALDB) ;
		AirportDB->getAcMan()->saveDatabase(AirportDB);

		CString _doorcommonPath ;
		_doorcommonPath.Format(_T("%s\\%s"),PROJMANAGER->GetAppPath(),"ArctermDB\\CommonAirportDB") ;
		AirportDB->getAcDoorMan()->ReadCommonACDoors(_doorcommonPath) ;
		AirportDB->getAcDoorMan()->saveDatabase(AirportDB);
	}else
		CopyDataFromFileToDataBase() ;
}
void CAirportDatabaseList::CopyDataFromFileToDataBase()
{
	//save default DB 
	CString directpath ;

	CString strSharePath(_T(""));
	CString strARCPath(_T(""));
	strSharePath.Format(_T("%s//%s"),PROJMANAGER->GetAppPath(),_T("\\ImportDb\\DEFAULTDB.mdb"));
	strARCPath.Format(_T("%s//%s"),PROJMANAGER->GetAppPath(),_T("\\Databases\\common.mdb"));

	directpath = PROJMANAGER->GetAppPath()+ "\\" + "ImportDB" ;
	CopyDefaultDB(_T("DEFAULT"),directpath) ;
	CShareTemplateDatabase* AirportDB = new CShareTemplateDatabase(strARCPath,strSharePath);
	AirportDB->setFolder(PROJMANAGER->GetAppPath() + "\\" + "ImportDB" + "\\" + "DEFAULT") ;
	AirportDB->setName("DEFAULTDB") ;
	AirportDB->loadDataFromFile() ;
	m_vAirportDB.push_back(AirportDB) ;
	AirportDB->saveDatabase();
	CString folderPath = PROJMANAGER->GetAppPath() + "\\" + "ImportDB" + "\\" + "DEFAULT" ;
	if(FileManager::IsDirectory(folderPath))
		FileManager::DeleteDirectory(folderPath) ; 
}

const char* CAirportDatabaseList::getTitle (void) const
{
	return "GLOBAL DB LIST";
}

const char* CAirportDatabaseList::getHeaders (void) const
{
	return "ID,NAME,FOLDER LOCATION";
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
// Given ID, return GlobalDB;


CShareTemplateDatabase* CAirportDatabaseList::getAirportDBByName(const CString strName)
{
	CString strDBName = strName;
	int nPos = checkIfIncludeKeyWords(strDBName.GetBuffer());
	if (nPos != -1)
	{
		CString strLeft(_T(""));
		CString strRight(_T(""));
		strLeft = strDBName.Left(nPos);
		strRight = strDBName.Right(strDBName.GetLength() - nPos - 1);
		strDBName = strLeft + strRight;
	}

	int _iCount = m_vAirportDB.size();
	for( int i=0; i<_iCount; i++ )
	{
		if( (m_vAirportDB[i]->getName()).CompareNoCase(strDBName) == 0)
			return m_vAirportDB[i];
	}

	return NULL;
}
// get the size of global db
int CAirportDatabaseList::getDBCount( void ) const
{
	return m_vAirportDB.size();
}

// get DB by index
CAirportDatabase* CAirportDatabaseList::getAirportDBByIndex( int _idx )
{
	if( _idx >=0 || (unsigned)_idx < m_vAirportDB.size() )
	   return m_vAirportDB[_idx ];
	else
		return NULL ;
}



// copy db 
bool CAirportDatabaseList::CopyDB( CString& _sourceDBPath, const CString& _strDestPath )
{

	FileManager fm;
	// first copy directory
	if(!fm.IsDirectory(_sourceDBPath))
		return FALSE ;
	if(!fm.IsDirectory(_strDestPath))
		fm.MakePath(_strDestPath) ;
	// then copy file
	for( int k = 0; k< DB_FILE_COUNT; k++ )
	{
		CString strFileName = _strDestPath + "\\" + __database_file[k];
		fm.CopyFile( _sourceDBPath + "\\" + __database_file[k], strFileName, 1 );
	}
	return true;
}

//// check name if repeat
bool CAirportDatabaseList::checkNameIfRepeat( const CString& _strName, const CAirportDatabase* _pGLDB  )
{
	for( unsigned i=0; i< m_vAirportDB.size(); i++ )
	{
		if( _pGLDB!= m_vAirportDB[i] && m_vAirportDB[i]->getName() == _strName )
		{
			return true;
		}
	}

	return false;
}

// get unique name
CString CAirportDatabaseList::getUniqueName( const CString& _strName )
{
	int _iCount = 0;
	for( unsigned i=0; i< m_vAirportDB.size(); i++ )
	{
		CString _strDBName = m_vAirportDB[i]->getName();
		_strDBName = _strDBName.Left( _strName.GetLength() );
		if( _strName.CompareNoCase( _strDBName ) == 0 )
			_iCount++;
	}

	if( _iCount == 0 )
		return _strName;

	CString _strReturnName;
	_strReturnName.Format("%s %d",_strName, _iCount );
	return _strReturnName;
}

// get all db file name
void CAirportDatabaseList::getAllDBFileName( CAirportDatabase* _pGLDB, std::vector< CString >& _vAllFileName ) const
{
	assert( _pGLDB );
	if(!_pGLDB)
		return;

	for( int i=0; i< DB_FILE_COUNT; i++ )
	{
		_vAllFileName.push_back( _pGLDB->getFolder() + "\\" + __database_file[i] );
	}
}
BOOL CAirportDatabaseList::IsAirportFile(CString filename)
{
	for( int i=0; i< DB_FILE_COUNT; i++ )
	{
		if(strcmp(filename, __database_file[i]) == 0)
		   return TRUE ;
	}
	return FALSE ;
}

void CAirportDatabaseList::CopyDefaultDB(const CString& sDestDBName,const CString& sDestPath /*=""*/)
{
	CString sTempPath=sDestPath;
	if(sTempPath.IsEmpty())
	{
		sTempPath=PROJMANAGER->GetAppPath()+ "\\" +CustomerAirportDBFolder;
	}
	TCHAR sDefaultAirportDBPath[MAX_PATH]={_T('\0')};
	TCHAR sDestAirportDBPath[MAX_PATH]={_T('\0')};

	TCHAR sDestFullFileName[MAX_PATH]={_T('\0')};
	TCHAR sSourceFullFileName[MAX_PATH]={_T('\0')};

	sprintf(sDefaultAirportDBPath,"%s\\%s",PROJMANAGER->GetAppPath().GetString(),DefaultAirportDBFolder);
	sprintf(sDestAirportDBPath,"%s\\%s",sTempPath.GetString(),sDestDBName.GetString());

	FileManager::MakePath(sDestAirportDBPath);

	int nCount=sizeof(__database_file)/sizeof(CString);
	for(int i=0;i<nCount;i++)
	{
		sprintf(sDestFullFileName,"%s\\%s",sDestAirportDBPath,__database_file[i].GetString());
			sprintf(sSourceFullFileName,"%s\\%s",sDefaultAirportDBPath, __database_file[i].GetString());
			FileManager::CopyFile(sSourceFullFileName,sDestFullFileName,1);
	}
}

void CAirportDatabaseList::_LoadDataSet(const CString strPath)
{
	clear();

	char filename[_MAX_PATH];
//	PROJMANAGER->getFileName (_pProjPath, fileType, filename);
	strcpy(filename,strPath);

	std::auto_ptr<ArctermFile> autop_file(new ArctermFile());
	ArctermFile& file = *autop_file;

	try { file.openFile (filename, READ); }
	catch (FileOpenError *exception)
	{
		delete exception;
//		initDefaultValues();
//		saveDataSet(strPath, false);
		return;
	}

	float fVersionInFile = file.getVersion();

	if( fVersionInFile < m_fVersion || fVersionInFile == 21 )
	{
		readObsoleteData( file );
		file.closeIn();
	//	saveDataSet(_pProjPath, false);
	}
	else if( fVersionInFile > m_fVersion )
	{
		// should stop read the file.
		file.closeIn();
		throw new FileVersionTooNewError( filename );		
	}
	else
	{
		readData (file);
		file.closeIn();
	}
}
void CAirportDatabaseList::ReadAirportDBData()
{
	clear() ;
	CString SQL ;
    SQL.Format(_T("SELECT * FROM TB_PROJECT_AIRPORTDB")) ;
	CADORecordset recordset ;
	long count ;
	try
	{
		CADODatabase::ExecuteSQLStatement(SQL,count,recordset,DATABASESOURCE_TYPE_ACCESS_GLOBALDB) ;
	}
	catch (CADOException e)
	{
		e.ErrorMessage() ;
		return ;
	}
	CShareTemplateDatabase* _pDB = NULL; 
	int id = -1 ;
	CString _name ;

	CString strSharePath(_T(""));
	CString strARCPath(_T(""));
	strARCPath.Format(_T("%s\\%s"),PROJMANAGER->GetAppPath(),_T("Databases\\arc.mdb"));
	while(!recordset.IsEOF())
	{
	   recordset.GetFieldValue(_T("ID"),id) ;
	   recordset.GetFieldValue(_T("AIRPORTDB_NAME"),_name) ;
	  
	   int nPos = checkIfIncludeKeyWords(_name.GetBuffer());
	   if (nPos != -1)
	   {
		   CString strLeft(_T(""));
		   CString strRight(_T(""));
		   strLeft = _name.Left(nPos);
		   strRight = _name.Right(_name.GetLength() - nPos - 1);
		   _name = strLeft + strRight;
	   }

	   strSharePath.Format(_T("%s\\ImportDB\\%s.mdb"),PROJMANAGER->GetAppPath(),_name);
	   _pDB  = new CShareTemplateDatabase(strARCPath,strSharePath) ;
	   _pDB->setName(_name) ;
	   _pDB->SetID(id) ;
	   if (!getAirportDBByName(_name))
	   {
		    m_vAirportDB.push_back(_pDB) ;
	   }

	   if(recordset.GetFieldValue(_T("VERSION"),id))
		   _pDB->m_nVersion = id ;
	   else
		   _pDB->m_nVersion = 0 ;
	   recordset.MoveNextData() ;
	}
}
void CAirportDatabaseList::WriteAiportDBData()
{
	AIRPORTDB_LIST::iterator iter = m_vAirportDB.begin() ;
	for ( ; iter != m_vAirportDB.end() ;iter++)
	{
		if( (*iter)->GetID() == -1)
			WriteAirport(*iter) ;
		else
			UpdateAirport(*iter) ;
	}

	iter = m_vDelAirportDB.begin() ;
	for ( ; iter != m_vDelAirportDB.end() ;iter++)
	{
		DeleteAirportDBData(*iter) ;
	}
	clearDelAirport() ;
}
void CAirportDatabaseList::WriteAirport(CAirportDatabase* _DataBase)
{
	if(_DataBase == NULL)
		return ;
	CString SQL ;
	SQL.Format(_T("INSERT INTO TB_PROJECT_AIRPORTDB (AIRPORTDB_NAME,VERSION) VALUES('%s',%d) "),_DataBase->getName(),AIRPORTDB_VERSION) ;
	try
	{
		int id = CADODatabase::ExecuteSQLStatementAndReturnScopeID(SQL,DATABASESOURCE_TYPE_ACCESS_GLOBALDB) ;
		_DataBase->SetID(id) ;
	}
	catch (CADOException e)
	{
		e.ErrorMessage() ;
		return ;
	}

}
void CAirportDatabaseList::UpdateAirport(CAirportDatabase* _DataBase)
{
	if(_DataBase == NULL)
		return ;
	CString SQL ;
	SQL.Format(_T("UPDATE TB_PROJECT_AIRPORTDB SET AIRPORTDB_NAME = '%s',VERSION = %d WHERE ID = %d"),_DataBase->getName(),AIRPORTDB_VERSION, _DataBase->GetID()) ;
	try
	{
		CADODatabase::ExecuteSQLStatement(SQL,DATABASESOURCE_TYPE_ACCESS_GLOBALDB) ;
	}
	catch (CADOException e)
	{
		e.ErrorMessage() ;
		return ;
	}
}
void CAirportDatabaseList::DeleteAirportDBData(const CString& _name)
{
	CAirportDatabase* PAirportDB = getAirportDBByName(_name) ;
	if(PAirportDB != NULL)
	  DeleteAirportDBData(PAirportDB->GetID()) ;
}
void CAirportDatabaseList::DeleteAirportDBData(CAirportDatabase* _DataBase)
{
	if(_DataBase == NULL)
		return ;
    DeleteAirportDBData(_DataBase->GetID()) ;
}
void CAirportDatabaseList::DeleteAirportDBData(int _id)
{
	if(_id == -1)
		return ;
	CString SQL ;
	SQL.Format(_T("DELETE * FROM TB_PROJECT_AIRPORTDB WHERE ID = %d") , _id) ;
	try
	{
		CADODatabase::ExecuteSQLStatement(SQL,DATABASESOURCE_TYPE_ACCESS_GLOBALDB) ;
	}
	catch (CADOException e)
	{
		e.ErrorMessage() ;
		return ;
	}
}
BOOL CAirportDatabaseList::LoadAirporDBByName(CString _Name , CAirportDatabase* _DataBase)
{
   CAirportDatabase* PAirportDB = getAirportDBByName(_Name) ;
   if(PAirportDB == NULL)
	   return FALSE ;
//    int _id = _DataBase->GetID() ;
//    CString _Oldname = _DataBase->getName() ;
//    _DataBase->SetID(PAirportDB->GetID()) ;
//    _DataBase->m_nVersion = PAirportDB->m_nVersion ;
//    _DataBase->setName(_Name) ;
//    _DataBase->loadDatabase();
// 
// 
//    PAirportDB->m_nVersion = _DataBase->m_nVersion ; // it may change the version ,so need set to it 
//   _DataBase->ResetAllID() ;
//   _DataBase->setName(_Oldname) ;
//    _DataBase->SetID(_id) ;
   _DataBase->loadFromOtherDatabase(PAirportDB);
   return TRUE ;
}
BOOL CAirportDatabaseList::WriteAirportDBByName(CString _Name , CAirportDatabase* _DataBase)
{
	CShareTemplateDatabase* PAirportDB = getAirportDBByName(_Name) ;
	int id = -1 ;
	id = _DataBase->GetID() ;
	if(PAirportDB != NULL)
	{
        int _id = PAirportDB->GetID() ;
		PAirportDB->DeleteAllData(DATABASESOURCE_TYPE_ACCESS_GLOBALDB) ;
		PAirportDB->SetID(id);
		PAirportDB->LoadDataFromDB() ;
		PAirportDB->SetID(_id) ;
	}else
	{
		CString strSharePath(_T(""));
		CString strARCPath(_T(""));
		strSharePath.Format(_T("%s\\ImportDB\\%s.mdb"),PROJMANAGER->GetAppPath(),_Name);
		strARCPath.Format(_T("%s\\%s"),PROJMANAGER->GetAppPath(),_T("Databases\\common.mdb"));

		PAirportDB = new CShareTemplateDatabase(strARCPath,strSharePath);
		PAirportDB->SetID(id) ;
		PAirportDB->LoadDataFromDB() ;
		PAirportDB->ResetAllID() ;
		PAirportDB->setName(_Name) ;
		PAirportDB->SetID(-1) ;
		m_vAirportDB.push_back(PAirportDB) ;
	}
	PAirportDB->saveDatabase();
	return TRUE ;
}

CAirportDatabase* CAirportDatabaseList::addOldDBEntry(const CString& _strDBName)
{
	CString strName = _strDBName;
	int nPos = checkIfIncludeKeyWords(strName.GetBuffer());
	if (nPos != -1)
	{
		CString strLeft(_T(""));
		CString strRight(_T(""));
		strLeft = strName.Left(nPos);
		strRight = strName.Right(strName.GetLength() - nPos - 1);
		strName = strLeft + strRight;
	}

	for(int i=0;i<getDBCount();i++)
	{
		if(strName.CompareNoCase(getAirportDBByIndex(i)->getName())==0)
		{
			CString errormsg ;
			errormsg.Format(_T("%s has existed "),_strDBName) ;
			MessageBox(NULL,errormsg,"Warning",MB_OK) ;
			return NULL ;
		}
	}
	CString strSharePath(_T(""));
	CString strARCPath(_T(""));
	strSharePath.Format(_T("%s\\ImportDB\\%s.mdb"),PROJMANAGER->GetAppPath(),strName);
	strARCPath.Format(_T("%s\\%s"),PROJMANAGER->GetAppPath(),_T("Databases\\arc.mdb"));

	CShareTemplateDatabase* _pDB = new CShareTemplateDatabase(strARCPath,strSharePath);

	//setup database for new airport database
	CString strPartsSourceFile(_T(""));
	CString strPartsDestinationFile(_T(""));

	strPartsSourceFile.Format(_T("%s\\ArctermDB\\temp\\ForSetupnewDatabase.mdb"),PROJMANAGER->GetAppPath());
	if (!PathFileExists(strSharePath))
	{
		CopyFile(strPartsSourceFile,strSharePath,FALSE);

		AirportDatabaseConnection pConnectionPtr(strSharePath);
		CPartsAndShareTemplateUpdate shareTemplateDataBaseUpdater;
		if(!shareTemplateDataBaseUpdater.Update(&pConnectionPtr)) 
		{
			CString strError(_T(""));
			strError.Format(_T("Update the new Template DataBase error."));
			MessageBox(NULL,strError,"Warning",MB_OK);
			return NULL;
		}
	}
	_pDB->setName(strName);
	_pDB->SaveAirport();

	m_vAirportDB.push_back(_pDB) ;
	return _pDB ;
}

CAirportDatabase* CAirportDatabaseList::addNewDBEntry( const CString& _strDBName) 
{
	CString strName = _strDBName;
	int nPos = checkIfIncludeKeyWords(strName.GetBuffer());
	if (nPos != -1)
	{
		CString strLeft(_T(""));
		CString strRight(_T(""));
		strLeft = strName.Left(nPos);
		strRight = strName.Right(strName.GetLength() - nPos - 1);
		strName = strLeft + strRight;
	}

	for(int i=0;i<getDBCount();i++)
	{
		if(strName.CompareNoCase(getAirportDBByIndex(i)->getName())==0)
		{
			CString errormsg ;
			errormsg.Format(_T("%s has existed "),_strDBName) ;
			MessageBox(NULL,errormsg,"Warning",MB_OK) ;
			return NULL ;
		}
	}
	CString strSharePath(_T(""));
	CString strARCPath(_T(""));
	strSharePath.Format(_T("%s\\ImportDB\\%s.mdb"),PROJMANAGER->GetAppPath(),strName);
	strARCPath.Format(_T("%s\\%s"),PROJMANAGER->GetAppPath(),_T("Databases\\arc.mdb"));

	CShareTemplateDatabase* _pDB = new CShareTemplateDatabase(strARCPath,strSharePath);

	//setup database for new airport database
	CString strPartsSourceFile(_T(""));
	CString strPartsDestinationFile(_T(""));

	strPartsSourceFile.Format(_T("%s\\ArctermDB\\temp\\project.mdb"),PROJMANAGER->GetAppPath());
	if (!PathFileExists(strSharePath))
	{
		CopyFile(strPartsSourceFile,strSharePath,FALSE);

		AirportDatabaseConnection pConnectionPtr(strSharePath);
		CPartsAndShareTemplateUpdate shareTemplateDataBaseUpdater;
		if(!shareTemplateDataBaseUpdater.Update(&pConnectionPtr)) 
		{
			CString strError(_T(""));
			strError.Format(_T("Update the new Template DataBase error."));
			MessageBox(NULL,strError,"Warning",MB_OK);
			return NULL;
		}
	}
	_pDB->setName(strName);
	_pDB->SaveAirport();

	m_vAirportDB.push_back(_pDB) ;
	return _pDB ;
}

void CAirportDatabaseList::RemoveAirportDB(CAirportDatabase* _database)
{
	if(_database == NULL)
		return ;
	AIRPORTDB_LIST::iterator iter = m_vAirportDB.begin() ;
	for ( ; iter != m_vAirportDB.end() ;iter++)
	{
		if( *iter == _database)
		{
			m_vDelAirportDB.push_back(*iter) ;
			m_vAirportDB.erase(iter) ;
			break ;
		}
	}
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////Old version code, should not be modified//////////////////////////////////////////////////