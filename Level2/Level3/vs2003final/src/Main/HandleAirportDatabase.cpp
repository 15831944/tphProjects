#include "StdAfx.h"
#include "Resource.h"
#include "HandleAirportDatabase.h"
#include "TermPlanDoc.h"
#include "../Common/AirportDatabase.h"
#include "../Common/INIFileHandle.h"
#include "../Common/AirportDatabaseList.h"
#include "../Common/ACTypesManager.h"
#include "../InputOnBoard/CInputOnboard.h"
#include "../InputOnboard/AircraftComponentModelDatabase.h"
#include "DlgAirportDBNameInput.h"
#include "../Database/PartsAndShareTemplateUpdate.h"
#include "../Database/ARCportADODatabase.h"
#include "../Landside/InputLandside.h"

///////////////////////HandleAirportDatabase////////////////////////////////////////////////////////////////
//////////////old code/////////////////////////////////////////////////////////////////////////////////////
void HandleAirportDatabase::CopyAircraftFile(CString _path,CString courseDir)
{
	FileManager fm ;
	CFileFind filefind ;
	std::vector<CString> vAddFile;
	BOOL res = filefind.FindFile(_path + "\\*.*") ;
	CString floderPath ;
	while (res)
	{
		res = filefind.FindNextFile() ;
		if(filefind.IsDirectory())
		{
			if(strcmp(".",filefind.GetFileName()) == 0 || strcmp("..",filefind.GetFileName()) == 0)
				continue;
			floderPath.Format(_T("%s\\%s"),courseDir,filefind.GetFileName()) ;
			if(!fm.IsDirectory(floderPath))
				fm.MakePath(floderPath) ;
			fm.CopyDirectory(_path + "\\" + filefind.GetFileName(),floderPath,NULL) ;
		}
		else
		{
			if(CAirportDatabaseList::IsAirportFile(filefind.GetFileName()))
				continue ;
			else
				fm.CopyFile(_path + "\\" + filefind.GetFileName(),courseDir + "\\" + filefind.GetFileName());
		}
	}
	filefind.Close() ;
}
///////////////////////////////////////old code//////////////////////////////////////////////////////////////
bool HandleAirportDatabase::loadOldDatabaseFroNewProject(CTermPlanDoc* pDoc,CAirportDatabase* pAirportDatabase)
{
	//if(pAirportDatabase == NULL)
	//	return false ;
	////load global db
	//CString projectDBFileDirectory ;
	//projectDBFileDirectory.Format("%s\\%s",pDoc->m_ProjInfo.path,"INPUT\\AirportDB");
	//FileManager fm;
	//if(!pAirportDatabase->hadLoadDatabase())
	//{
	//	if(pAirportDatabase->GetID() != -1)
	//	{
	//		//pAirportDatabase->loadPreVersionData(DATABASESOURCE_TYPE_ACCESS_GLOBALDB) ;
	//		pAirportDatabase->loadDatabase();
	//	}
	//	else
	//	{
	//		pAirportDatabase->loadDataFromFile() ;
	//		if(!fm.IsDirectory(projectDBFileDirectory))
	//			fm.MakePath( projectDBFileDirectory );
	//		AIRPORTDBLIST->CopyDB(PROJMANAGER->GetAppPath()+"\\ImportDB\\" + pAirportDatabase->getName(),projectDBFileDirectory +"\\"+  pAirportDatabase->getName()) ;
	//		pAirportDatabase->saveDatabase();
	//	}
	//}




	//if(!fm.IsDirectory(projectDBFileDirectory))
	//	fm.MakePath( projectDBFileDirectory );

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool HandleAirportDatabase::openGlobalDB(CTermPlanDoc* pDoc)
{
	// lower than 303 is old version project version
	if (pDoc->m_ProjInfo.version < 303)
	{
		//this call is for old code load and save as new version database, you should change and modify this code
		openOldVersionGlobalDatabase(pDoc);
	}
	//else
	{
		//for project database load
		CString  strProjectDataFilePath(_T(""));
		CString  strFolder(_T(""));
		CString	 strARCDatabaseFilePath(_T(""));

		strProjectDataFilePath.Format("%s\\%s",pDoc->m_ProjInfo.path,"INPUT\\parts.mdb");
		strARCDatabaseFilePath.Format(_T("%s"),PROJMANAGER->GetAppPath() + "\\Databases\\arc.mdb");
		CARCProjectDatabase* pAirportDB = new CARCProjectDatabase(strARCDatabaseFilePath,strProjectDataFilePath);

		pDoc->GetTerminal().m_pAirportDB = pAirportDB;
		pDoc->GetInputAirside().m_pAirportDB = pDoc->GetTerminal().m_pAirportDB;
		pDoc->GetInputOnboard()->SetAirportDB(pDoc->GetTerminal().m_pAirportDB);
		pDoc->getARCport()->m_pInputLandside->setAirPortDataBase(pDoc->GetTerminal().m_pAirportDB);

		pAirportDB->readAirport();

		pAirportDB->loadDatabase();

		//create share template database
		OpenProjectDBForInitNewProject(pAirportDB->getName());
	}

	return true;
}

/////////////////////////////////old version code/////////////////////////////////////////////////////////
bool HandleAirportDatabase::openOldVersionGlobalDatabase(CTermPlanDoc* pDoc)
{
	setupProjectDatabase(&pDoc->m_ProjInfo);
	if(pDoc->m_ProjInfo.version <= 300)
	{
		CString projectDBFileName ;
		CString projectDBFileDirectory ;
		BOOL IsNoProjectDB = FALSE ;
		projectDBFileDirectory.Format("%s\\%s",pDoc->m_ProjInfo.path,"INPUT\\AirportDB");
		FileManager fm;
		if(!fm.IsDirectory(projectDBFileDirectory))
		{
			fm.MakePath( projectDBFileDirectory );
			if(pDoc->m_ProjInfo.dbname.IsEmpty())
			{
				CString DBName ;
				CString _index  ;
				TCHAR _CharIndex[1024] = {0} ;
				CAirportDatabase* DB = AIRPORTDBLIST->getAirportDBByIndex(pDoc->m_ProjInfo.lUniqueIndex) ;
				if(DB != NULL)
				{
					(pDoc->m_ProjInfo).dbname.Format("%s",DB->getName()) ;
				}
				else
				{
					CDlgAirportDBNameInput dlg( "Please Enter The Airport Name for the Imported Project:" );
					CString sInput("");
					if(dlg.DoModal()==IDCANCEL)
					{
						return false;
					}
					pDoc->m_ProjInfo.dbname = dlg.m_sAirportDBName;
				}
			}
			if(!AIRPORTDBLIST->CopyDB(PROJMANAGER->GetAppPath()+"\\ImportDB\\" + pDoc->m_ProjInfo.dbname,projectDBFileDirectory +"\\"+ pDoc->m_ProjInfo.dbname))
			{
				AIRPORTDBLIST->CopyDefaultDB(pDoc->m_ProjInfo.dbname,projectDBFileDirectory) ;
			}
			IsNoProjectDB = TRUE ;
		}
		else
		{
			CString DBName ;
			CString _index  ;
			TCHAR _CharIndex[1024] = {0} ;
			if(fm.IsFile(projectDBFileDirectory + "\\DBMessage.txt"))
			{
				CCfgData DBfile;
				DBfile.LoadCfgData(projectDBFileDirectory + "\\DBMessage.txt") ;
				DBfile.SetGroup("Airport DBName");
				DBfile.GetStrData("AirportDBName",DBName) ;
				DBfile.GetStrData("DBIndex",_index) ;

				strcpy(_CharIndex,_index) ;
				(pDoc->m_ProjInfo).lUniqueIndex = atol(_CharIndex) ;
				(pDoc->m_ProjInfo).dbname.Format("%s",DBName) ;	
			}
			else
			{
				if(pDoc->m_ProjInfo.dbname.IsEmpty())
				{
					CDlgAirportDBNameInput dlg( "Please Enter The Airport Name for the Imported Project:" );
					CString sInput("");
					if(dlg.DoModal()==IDCANCEL)
					{
						return false;
					}
					pDoc->m_ProjInfo.dbname = dlg.m_sAirportDBName;
					AIRPORTDBLIST->CopyDefaultDB(pDoc->m_ProjInfo.dbname,projectDBFileDirectory) ;
				}
			}
		}
		projectDBFileName.Format("%s\\%s",projectDBFileDirectory,pDoc->m_ProjInfo.dbname) ;
		CShareTemplateDatabase* DB = AIRPORTDBLIST->getAirportDBByName(pDoc->m_ProjInfo.dbname) ;

		CString  strProjectDataFilePath(_T(""));
		strProjectDataFilePath.Format("%s\\%s",pDoc->m_ProjInfo.path,"INPUT\\parts.mdb");
		CString strARCPath(_T(""));
		strARCPath.Format(_T("%s\\%s"),PROJMANAGER->GetAppPath(),_T("Databases\\arc.mdb"));
		CARCProjectDatabase* pAirportDB = new CARCProjectDatabase(strARCPath,strProjectDataFilePath);
		pAirportDB->SetProjectVersion(pDoc->m_ProjInfo.version) ;
		pAirportDB->setFolder(projectDBFileName);
		if(DB != NULL && DB->GetID() != -1&&IsNoProjectDB) //if exist in Database ,load data form DataBase
		{
			if( ! AIRPORTDBLIST->LoadAirporDBByName(pDoc->m_ProjInfo.dbname ,pAirportDB) )
			{
				pAirportDB->loadDataFromFile() ;
			}
		}
		else
			pAirportDB->loadDataFromFile();

		CopyAircraftFile(projectDBFileName,projectDBFileDirectory) ;

		pAirportDB->setName(pDoc->m_ProjInfo.dbname) ;
		////pDoc->GetTerminal().m_pAirportDB = pAirportDB;
		////pDoc->GetInputAirside().m_pAirportDB = pDoc->GetTerminal().m_pAirportDB;
		////pDoc->GetInputOnboard()->SetAirportDB(pDoc->GetTerminal().m_pAirportDB);
		pAirportDB->getAcMan()->SetDBPath(projectDBFileDirectory) ;

		if(DB == NULL)
			AIRPORTDBLIST->addOldDBEntry(pDoc->m_ProjInfo.dbname) ;
		else
		{
			if (!PathFileExists(DB->getDatabaseFilePath()))
			{
				CString strPartsSourceFile(_T(""));
				CString strPartsDestinationFile(_T(""));

				strPartsSourceFile.Format(_T("%s\\ArctermDB\\temp\\project.mdb"),PROJMANAGER->GetAppPath());
				CopyFile(strPartsSourceFile,DB->getDatabaseFilePath(),FALSE);

				AirportDatabaseConnection pConnectionPtr(DB->getDatabaseFilePath());
				CPartsAndShareTemplateUpdate shareTemplateDataBaseUpdater;
				if(!shareTemplateDataBaseUpdater.Update(&pConnectionPtr)) 
				{
					CString strError(_T(""));
					strError.Format(_T("Update the new Template DataBase error."));
					MessageBox(NULL,strError,"Warning",MB_OK);
					return NULL;
				}

				DB->loadPreVersionData(DATABASESOURCE_TYPE_ACCESS_GLOBALDB);
				DB->saveDatabase();
				DB->GetAirportConnection()->CloseConnection();
			}
		}

		//for the first time ,save data to DB 
		pAirportDB->SetID(-1);
		pAirportDB->saveDatabase();
		delete pAirportDB;
		////pDoc->GetTerminal().m_pAirportDB->saveDatabase();

		//reload the new version database from parts
		////pDoc->GetTerminal().m_pAirportDB->loadDatabase();
	}
	else
	{
		CString  projectDBFileDirectory ;
		projectDBFileDirectory.Format("%s\\%s",pDoc->m_ProjInfo.path,"INPUT\\AirportDB");
		CString strProjectPath(_T(""));
		CString strARCPath(_T(""));
		strProjectPath.Format("%s\\%s",pDoc->m_ProjInfo.path,"INPUT\\parts.mdb");
		strARCPath.Format(_T("%s%s"),PROJMANAGER->GetAppPath(),_T("\\Databases\\arc.mdb"));
		CARCProjectDatabase* pAirportDB = new CARCProjectDatabase(strARCPath,strProjectPath);
		pAirportDB->setFolder(projectDBFileDirectory);
		pAirportDB->SetProjectVersion(pDoc->m_ProjInfo.version) ;
		////pDoc->GetTerminal().m_pAirportDB = pAirportDB;
		////pDoc->GetInputAirside().m_pAirportDB = pDoc->GetTerminal().m_pAirportDB;
		////pDoc->GetInputOnboard()->SetAirportDB(pDoc->GetTerminal().m_pAirportDB);
		pAirportDB->ReadAirportDataBase();
		pAirportDB->loadPreVersionData(DATABASESOURCE_TYPE_ACCESS) ;
		pAirportDB->SetID(-1);
		pAirportDB->ResetAllID();
		pAirportDB->saveDatabase();
		CAirportDatabase* DB = AIRPORTDBLIST->getAirportDBByName(pAirportDB->getName()) ;
		pDoc->m_ProjInfo.dbname = pAirportDB->getName() ;

		delete pAirportDB;

		if(DB == NULL)
			AIRPORTDBLIST->addOldDBEntry(pDoc->m_ProjInfo.dbname);
		else
		{
			if (!PathFileExists(DB->getDatabaseFilePath()))
			{
				CString strPartsSourceFile(_T(""));
				CString strPartsDestinationFile(_T(""));

				strPartsSourceFile.Format(_T("%s\\ArctermDB\\temp\\project.mdb"),PROJMANAGER->GetAppPath());
				CopyFile(strPartsSourceFile,DB->getDatabaseFilePath(),FALSE);

				AirportDatabaseConnection pConnectionPtr(DB->getDatabaseFilePath());
				CPartsAndShareTemplateUpdate shareTemplateDataBaseUpdater;
				if(!shareTemplateDataBaseUpdater.Update(&pConnectionPtr)) 
				{
					CString strError(_T(""));
					strError.Format(_T("Update the new Template DataBase error."));
					MessageBox(NULL,strError,"Warning",MB_OK);
					return NULL;
				}
				DB->loadPreVersionData(DATABASESOURCE_TYPE_ACCESS_GLOBALDB);
				DB->saveDatabase();
				DB->GetAirportConnection()->CloseConnection();
			}
		}

		//reload the new version database from parts
		////pAirportDB->loadDatabase();
	}

	pDoc->m_ProjInfo.version = PROJECT_VERSION;
	PROJMANAGER->UpdateProjInfoFile(pDoc->m_ProjInfo);
	return true;
}

bool HandleAirportDatabase::OpenProjectDBForNewProject(CTermPlanDoc* pDoc,CAirportDatabase* _database)
{
	if(!loadOldDatabaseFroNewProject(pDoc,_database))
		return false;

	if (_database == NULL)
	{
		return false;
	}

	setupProjectDatabase(&pDoc->m_ProjInfo);
	CString strProjectPath(_T(""));
	CString strARCPath(_T(""));
	strProjectPath.Format(_T("%s\\%s"),pDoc->m_ProjInfo.path,"INPUT\\parts.mdb");
	strARCPath.Format(_T("%s\\%s"),PROJMANAGER->GetAppPath(),_T("\\Databases\\arc.mdb"));

	CARCProjectDatabase* pAirportDB = new CARCProjectDatabase(strARCPath,strProjectPath);
	CString strDBPath ;
	strDBPath.Format(_T("%s\\%s"),pDoc->m_ProjInfo.path,"INPUT\\AirportDB");
	pAirportDB->setFolder(strDBPath) ;

	_database->saveAsTemplateDatabase(pAirportDB);
	pAirportDB->loadDatabase();
	//init project database data
	pAirportDB->setName(_database->getName());
	pDoc->GetTerminal().m_pAirportDB = pAirportDB;
	pDoc->GetInputAirside().m_pAirportDB = pDoc->GetTerminal().m_pAirportDB;
	pDoc->GetInputOnboard()->SetAirportDB(pDoc->GetTerminal().m_pAirportDB);
	pDoc->getARCport()->m_pInputLandside->setAirPortDataBase(pDoc->GetTerminal().m_pAirportDB);
	
	pAirportDB->SaveAirport();
	pAirportDB->getAcMan()->SetDBPath(strDBPath) ;
	return true;
}

CAirportDatabase* HandleAirportDatabase::OpenProjectDBForInitNewProject(const CString& sAirportName)
{
	CAirportDatabase* _AirportDB =  AIRPORTDBLIST->getAirportDBByName(sAirportName)  ;
	if(_AirportDB == NULL)
		_AirportDB = AIRPORTDBLIST->addNewDBEntry(sAirportName) ;
	else
	{
		if (!PathFileExists(_AirportDB->getDatabaseFilePath()))
		{
			CString strPartsSourceFile(_T(""));
			CString strPartsDestinationFile(_T(""));

			strPartsSourceFile.Format(_T("%s\\ArctermDB\\temp\\project.mdb"),PROJMANAGER->GetAppPath());
			CopyFile(strPartsSourceFile,_AirportDB->getDatabaseFilePath(),FALSE);

			AirportDatabaseConnection pConnectionPtr(_AirportDB->getDatabaseFilePath());
			CPartsAndShareTemplateUpdate shareTemplateDataBaseUpdater;
			if(!shareTemplateDataBaseUpdater.Update(&pConnectionPtr)) 
			{
				CString strError(_T(""));
				strError.Format(_T("Update the new Template DataBase error."));
				MessageBox(NULL,strError,"Warning",MB_OK);
				return NULL;
			}
		}
	}
	return _AirportDB;
}

bool HandleAirportDatabase::setupProjectDatabase(PROJECTINFO* _pinfo)
{
	CString strPartsSourceFile(_T(""));
	CString strPartsDestinationFile(_T(""));

	strPartsSourceFile.Format(_T("%s\\ArctermDB\\temp\\project.mdb"),PROJMANAGER->GetAppPath());
	strPartsDestinationFile.Format(_T("%s\\INPUT\\parts.mdb"),_pinfo->path);
	CString strFolder(_T(""));
	strFolder.Format(_T("%s\\INPUT"),_pinfo->path);
	FileManager::MakePath(strFolder);
	FileManager::CopyFile(strPartsSourceFile,strPartsDestinationFile,0);
	return true;
}
