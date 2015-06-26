#include "StdAfx.h"
#include ".\projectdatabase.h"
#include "../InputAirside/GlobalDatabase.h"
//#include "../Main/TermPlan.h"
#include "../Database/ARCportDatabaseConnection.h"
#include <Shlwapi.h>
#include "DBUpdater.h"
#include "../Common/INIFileHandle.h"
#include "../Common/ReadRegister.h"
#include "../Database/ProjectUpdater.h"
//#include "../Database/DatabaseUpdater.h"
#include "../Database/ADODatabase.h"
#include "../Database/GlobalDBUpdater.h"
#include "../Database/PartsAndShareTemplateUpdate.h"
#include "../Database/ARCportADODatabase.h"
#include "../Common/fileman.h"
#include "../Common/ProjectManager.h"
#include <WinSvc.h>

#include "../Database/CompactRepairDatabase.h"
#include "Database/VersionUpdater.h"
#include "InputAirside.h"
#include "ALTAirport.h"

CProjectDatabase::CProjectDatabase(const CString& strAppPath)
{
	m_strAppPath = strAppPath;
	m_bOpen = FALSE;
}

CProjectDatabase::~CProjectDatabase(void)
{
}
void CProjectDatabase::CompactRepairMDB( const CString& strFileName, const CString& strPath ) const
{
	CString strCurFile;
	strCurFile.Format(_T("%s\\%s"),strPath, strFileName );

	CString strTmpFile;
	strTmpFile.Format(_T("%s\\tmp%s"),strPath,strFileName);

	//CString strBackup;
	//strBackup.Format(_T("%s\\%s.bak"),strPath,strFileName);


	////back up
	//FileManager::DeleteFile(strBackup);
	//FileManager::CopyFile(strCurFile, strBackup);

	//create tmp file
	FileManager::DeleteFile(strTmpFile);

	//repair parts database
	CompactRepairDatabase cpDatabase;
	if(cpDatabase.CompactRepair(strCurFile, strTmpFile))
	{
		DeleteFile(strCurFile);
		CopyFile(strTmpFile, strCurFile, FALSE);
	}
}

class ARCProjectExtraUpdate : public DataBaseUpdateCallBack
{
public:
	ARCProjectExtraUpdate(const CString& sProjectName):m_sProjName(sProjectName){}
	virtual void preVersionUpdate(VersionUpdater* pUpdater)
	{
		if(pUpdater->GetVerison()==1300)
		{
			int nProjID = InputAirside::GetProjectID(m_sProjName);
			std::vector<int> vAirportIDs;
			InputAirside::GetAirportList(nProjID, vAirportIDs);
			for(size_t i=0;i<vAirportIDs.size();i++)
			{
				ALTAirport::UpdateRunwayExitsNames(vAirportIDs[i]);
			}
		}
	}
	CString m_sProjName;
};

bool CProjectDatabase::OpenProject(CMasterDatabase *pMasterDatabase,const CString& strProjName,const CString& strProjPath)
{
	CString strInputPath;
	strInputPath.Format(_T("%s\\Project\\%s\\input"),m_strAppPath,strProjName);
	//get the input path


	//get arcport.mdb path
	CString strArcportdbPath;
	strArcportdbPath.Format(_T("%s\\arcport.mdb"),strInputPath);

	CString strPartsDatabasePath(_T(""));
	strPartsDatabasePath.Format(_T("%s\\parts.mdb"),strInputPath);

	CString strBaseDatabaseFile(_T(""));
	strBaseDatabaseFile.Format(_T("%s\\ArctermDB\\temp\\project.mdb"),m_strAppPath);

	//check the parts database is exsit or not
	if (!PathFileExists(strPartsDatabasePath))
	{
		CopyFile(strBaseDatabaseFile,strPartsDatabasePath,TRUE);
	}

	//update the parts database
	{

		CompactRepairMDB("parts.mdb", strInputPath);
	
		CPartsAndShareTemplateUpdate partsDatabaseUpdater;
		AirportDatabaseConnection pConnectionPtr(strPartsDatabasePath);
		if (!partsDatabaseUpdater.Update(&pConnectionPtr))
		{
			CString strError(_T(""));
			strError.Format(_T("Update the Aiport DataBase error."));
			MessageBox(NULL,strError,"Warning",MB_OK);
			return false;
		}
	}

	DATABASECONNECTION.SetInputPath(strInputPath);
	DATABASECONNECTION.SetAccessDatabasePath(strArcportdbPath);


	{
		//check the arcport.mdb is exist or not
		if(PathFileExists(strArcportdbPath))
		{
			//get arcport.mdb version
			CProjectUpdater projUpdater(m_strAppPath,strProjName);
			long nVersion = -1;

			try
			{	
				nVersion = projUpdater.GetDatabaseCurrentVersion();
			}
			catch (CADOException& e)
			{
				DATABASECONNECTION.CloseConnection();
				CString strErrorMessage = e.ErrorMessage();
				
				HANDLE hf = ::CreateFile(strArcportdbPath,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,0,NULL);
				 if (hf == INVALID_HANDLE_VALUE)
				 {
					CString strLogMessage = _T("Can't open the project, database error.");
					MessageBox(NULL,strLogMessage,_T("ARCport Error"),MB_OK);
					DATABASECONNECTION.writeLog(strLogMessage);	
					return false;
				 }

				 int nFileSize=(int)::GetFileSize(hf, NULL);

				 if(nFileSize> 10 && nFileSize < 3000000)//the database file is not correct
				 {
					 nVersion = 1000;//it is old version
				 }
				 CloseHandle(hf);
				
			}

			if(nVersion >= 1073)
			{
				//if import project
				if (pMasterDatabase->IsOpenImportedProject())
				{
					CString strOldProjName = pMasterDatabase->GetImportedProjectName();
					CString strSQL;

					try
					{
						if(IsProjectExistsInDB(strOldProjName) == false)
						{
							MessageBox(NULL,_T("Import the project failed"),_T("ARCport"),MB_OK);
							pMasterDatabase->CancelImportedProjectFlag();
							return false;

						}


						strSQL.Format(_T("UPDATE PROJECT SET NAME = '%s' WHERE NAME = '%s'"),strProjName,strOldProjName);

						_variant_t varCount;
						DATABASECONNECTION.GetConnection()->Execute((_bstr_t)strSQL,&varCount,ADODB::adOptionUnspecified);

						//if(IsProjectExistsInDB(strProjName) == false)
						//{
						//	MessageBox(NULL,_T("Import the project failed"),_T("ARCport"),MB_OK);
						//	pMasterDatabase->CancelImportedProjectFlag();
						//	return false;
						//}

					}
					catch (_com_error& e)
					{
						CString strLogMessage;
						strLogMessage = _T("Error execute access sql statement:  ");
						strLogMessage += (char *)e.Description();
						strLogMessage += strSQL;
						pMasterDatabase->Log( strProjName,strLogMessage);
						DATABASECONNECTION.writeLog(strLogMessage);
						pMasterDatabase->CancelImportedProjectFlag();
						MessageBox(NULL,_T("Import the project failed"),_T("ARCport"),MB_OK);
						return false;

					}

					pMasterDatabase->CancelImportedProjectFlag();
				}
				bool bUpdateSuccess = false;
				try
				{
					//
					ARCProjectExtraUpdate callback(strProjName);
					bUpdateSuccess = projUpdater.UpdateProject(&callback);
					
				}
				catch(CADOException&)
				{
					//if(bUpdateSuccess == false)
					MessageBox(NULL,_T("update project Failed, please check ProjectLog.log file in Input folder for detail. "),_T("ARCport Error"),MB_OK);
					bUpdateSuccess = false;
				}
				if(bUpdateSuccess == false)
					MessageBox(NULL,_T("update project Failed, please check ProjectLog.log file in Input folder for detail. "),_T("ARCport Error"),MB_OK);

				return bUpdateSuccess;
			}
			else if(nVersion == 1000)
			{
				//delete the arcport.mdb and copy new
				DATABASECONNECTION.CloseConnection();
				CString strTempdbPath;
				strTempdbPath.Format(_T("%s\\arcport_old.mdb"),strInputPath);
				try
				{
					strTempdbPath.Format(_T("%s\\arcport_old.mdb"),strInputPath);
					if (PathFileExists(strTempdbPath))
					{
						DeleteFile(strArcportdbPath);
					}
					else
					{
						CFile::Rename(strArcportdbPath,strTempdbPath);
					}

				}
				catch(CFileException* e)
				{
					CString strFileError;
					e->GetErrorMessage(strFileError.GetBuffer(1024),1024);
					DATABASECONNECTION.writeLog(strFileError);
					//DeleteFile(strArcportdbPath);
					MessageBox(NULL,strFileError,"ARCport Error",MB_OK);
					if(PathFileExists(strArcportdbPath))
						return false;
				}
				

			}
			else
			{
				MessageBox(NULL,_T("Can not open project, please check ProjectLog.log file in Input folder for detail. "),_T("ARCport Error"),MB_OK);

                return false;
			}
		}
		//not find project database, copy a new database
		
	}

	DATABASECONNECTION.writeLog(_T("Convet old data to access start ..."));
	CString strMDFFilePath = _T("");
	CString strLDFFilePath = _T("");
	CString strDBName = _T("");
	//check the database has sql server databases file or not
	{	
		//CString strInputPath;
		//strInputPath.Format(_T("%s\\Project\\%s\\input"),m_strAppPath,strProjName);
		if(FindSQLServerDataFiles(strInputPath,strMDFFilePath,strLDFFilePath,strDBName) == false) //old project
		{
			bool bUpdateSuccess = false;
			try
			{
				OpenAccessDatabase(pMasterDatabase,strProjName,strProjPath);
				CProjectUpdater projUpdater(m_strAppPath,strProjName);

				ARCProjectExtraUpdate extraUpdate(strProjName);
				bUpdateSuccess =projUpdater.UpdateProject(&extraUpdate);					
			}
			catch(CADOException&)
			{
				MessageBox(NULL,_T("Convert project Failed, please check ProjectLog.log file in Input folder for detail. "),_T("ARCport Error"),MB_OK);
				bUpdateSuccess = false;
			}
			return bUpdateSuccess;
		}
	}


	//has sql server data files  convert sql server data to access
	{
		if(pMasterDatabase->Connect() == false)
		{
			//try to start sql server
			if(startsqlserver() == false)
			{
				CString strMessage = _T("Can not open the project. The project depends on SQL Server 2005, please contact ARC to find the solution.");

				MessageBox(NULL,strMessage,_T("ARCport Error"),MB_OK);	
				return false;
			}

			if(pMasterDatabase->Connect() == false)
			{
				CString strMessage = _T("Can not open the project. The project depends on SQL Server 2005, please contact ARC to find the solution.");

				MessageBox(NULL,strMessage,_T("ARCport Error"),MB_OK);
				return false;
			}


		}
		//check the SQL Server is installed or not

		//open the database
		{
			CString strLogMessage;
			//attach database
			CString strOldDBname = _T("ARC")+strDBName;
			DetachDatabase(pMasterDatabase,strOldDBname);

			CString strAttachName = _T("ARCConvertDB");
			//pMasterDatabase->Connect();
			DetachDatabase(pMasterDatabase,strAttachName);
			//attach 
			CString strSQLAttach;
			try
			{			
				
				strSQLAttach.Format(_T("sp_attach_db @dbname ='%s',@filename1='%s',@filename2='%s'"),strAttachName,strMDFFilePath,strLDFFilePath);
				pMasterDatabase->Execute(strSQLAttach);
			}
			catch (_com_error& e)
			{	
				strLogMessage = _T("Error:");
				strLogMessage += (char *)e.Description();
				strLogMessage += strSQLAttach;
				DATABASECONNECTION.writeLog(strLogMessage);
				MessageBox(NULL,_T("update project Failed, please check ProjectLog.log file in Input folder for detail. "),_T("ARCport Error"),MB_OK);

				
				return false;
			}

			if(ReadConnectInfoFromReg() != true)
			{
				CString strLogMessage = _T("Read Sql Server connection information error.");
				DATABASECONNECTION.writeLog(strLogMessage);

				CString strErrorString;
				strErrorString.Format(_T("Provider=SQLOLEDB;Data Source=%s;Initial Catalog=%s;User ID=%s;Password=%s"),
					m_strServer.c_str(),strAttachName, m_strUser.c_str(), m_strPassWord.c_str());
				DATABASECONNECTION.writeLog(strErrorString);

				return false;
			}

			CString strConnectString;
			strConnectString.Format(_T("Provider=SQLOLEDB;Data Source=%s;Initial Catalog=%s;User ID=%s;Password=%s"),
				m_strServer.c_str(),strAttachName, m_strUser.c_str(), m_strPassWord.c_str());
			DATABASECONNECTION.writeLog(strConnectString);	

			//connect
			try
			{

				DATABASECONNECTION.SetConnectString(strConnectString);
				DATABASECONNECTION.GetConnection(DATABASESOURCE_TYPE_SQLSERVER);

			}
			catch (_com_error& e)
			{	
				strLogMessage = _T("Error open project database connection:  ");
				strLogMessage += (char *)e.Description();
				strLogMessage += strConnectString;
				//pMasterDatabase->Log(strProjName,strLogMessage);
				DATABASECONNECTION.writeLog(strLogMessage);
				MessageBox(NULL,_T("Convert project Failed, please check ProjectLog.log file in Input folder for detail. "),_T("ARCport Error"),MB_OK);

				return false;
			}
			catch(CADOException& e)
			{
				strLogMessage = _T("Error open project database connection:  ");
				strLogMessage += e.ErrorMessage();
				strLogMessage += strConnectString;
				//pMasterDatabase->Log(strProjName,strLogMessage);
				DATABASECONNECTION.writeLog(strLogMessage);
				MessageBox(NULL,_T("Convert project Failed, please check ProjectLog.log file in Input folder for detail. "),_T("ARCport Error"),MB_OK);
				return false;
			}

			try
			{	
				strLogMessage = _T("Update Project ..:  ");
				pMasterDatabase->Log(strProjName,strLogMessage);
				UpdateProject();
			}
			catch (_com_error& e)
			{	
				strLogMessage = _T("Error Update Project ..:  ");
				strLogMessage += (char *)e.Description();
				strLogMessage += strConnectString;
				//pMasterDatabase->Log(strProjName,strLogMessage);
				DATABASECONNECTION.writeLog(strLogMessage);
				MessageBox(NULL,_T("Convert project Failed, please check ProjectLog.log file in Input folder for detail. "),_T("ARCport Error"),MB_OK);

				return false;
			}
			catch (CADOException& e)
			{	
				strLogMessage = _T("Error Update Project ..:  ");
				strLogMessage += e.ErrorMessage();
				strLogMessage += strConnectString;
				//pMasterDatabase->Log(strProjName,strLogMessage);
				DATABASECONNECTION.writeLog(strLogMessage);
				MessageBox(NULL,_T("Convert project Failed, please check ProjectLog.log file in Input folder for detail. "),_T("ARCport Error"),MB_OK);

				return false;
			}
			//if imported project
			if (pMasterDatabase->IsOpenImportedProject())
			{
				CString strOldProjName = pMasterDatabase->GetImportedProjectName();
				CString strSQL;
				try
				{	
					////check old project exist or not
					//{
					//	CString strCheckSQL =_T("");
					//	strCheckSQL.Format(_T("SELECT NAME  FROM PROJECT  WHERE NAME = '%s'"),strOldProjName);

					//	long nCheckCount = 0;
					//	CADORecordset adRecordsetcheck;
					//	CADODatabase::ExecuteSQLStatement(strCheckSQL,nCheckCount,adRecordsetcheck,DATABASESOURCE_TYPE_SQLSERVER);

					//	if(adRecordsetcheck.IsEOF())
					//	{
					//		strCheckSQL.Empty();
					//		strCheckSQL= _T("SELECT NAME  FROM PROJECT");
					//		CADORecordset adRecordsetcheckAll;
					//		CADODatabase::ExecuteSQLStatement(strCheckSQL,nCheckCount,adRecordsetcheckAll,DATABASESOURCE_TYPE_SQLSERVER);
					//		
					//		CString strFirstNameExist;
					//		if(!adRecordsetcheckAll.IsEOF())
					//		{
					//			adRecordsetcheckAll.GetFieldValue(_T("NAME"),strFirstNameExist);
					//			adRecordsetcheckAll.MoveNextData();
					//		
					//			if(!adRecordsetcheckAll.IsEOF())
					//			{
					//				MessageBox(NULL,_T("Import the project failed"),_T("ARCport"),MB_OK);
					//				pMasterDatabase->CancelImportedProjectFlag();	
					//				return false;
					//			}

					//			strProjName = strFirstNameExist;
					//		}
					//	}
					//}

					strSQL.Format(_T("UPDATE PROJECT SET NAME = '%s' WHERE NAME = '%s'"),strProjName,strOldProjName);

					_variant_t varCount;
					DATABASECONNECTION.GetConnection(DATABASESOURCE_TYPE_SQLSERVER)->Execute((_bstr_t)strSQL,&varCount,ADODB::adOptionUnspecified);
					//{
					//	CString strCheckSQL;
					//	strCheckSQL.Format(_T("SELECT NAME  FROM PROJECT  WHERE NAME = '%s'"),strProjName);

					//	long nCheckCount = 0;
					//	CADORecordset adRecordsetcheck;
					//	CADODatabase::ExecuteSQLStatement(strCheckSQL,nCheckCount,adRecordsetcheck,DATABASESOURCE_TYPE_SQLSERVER);

					//	if(adRecordsetcheck.IsEOF())
					//	{
					//		MessageBox(NULL,_T("Import the project failed"),_T("ARCport"),MB_OK);
					//		return false;
					//	}
					//}
				
				
				}
				catch (_com_error& e)
				{
					strLogMessage = _T("Error execute sql statement:  ");
					strLogMessage += (char *)e.Description();
					strLogMessage += strSQL;
					DATABASECONNECTION.writeLog(strLogMessage);
					MessageBox(NULL,_T("Convert project Failed, please check ProjectLog.log file in Input folder for detail. "),_T("ARCport Error"),MB_OK);
					pMasterDatabase->CancelImportedProjectFlag();				
					return false;

				}
				catch (CADOException& e)
				{
					strLogMessage = _T("Error execute sql statement:  ");
					strLogMessage += e.ErrorMessage();
					strLogMessage += strSQL;
					DATABASECONNECTION.writeLog(strLogMessage);
					MessageBox(NULL,_T("Convert project Failed, please check ProjectLog.log file in Input folder for detail. "),_T("ARCport Error"),MB_OK);
					pMasterDatabase->CancelImportedProjectFlag();				
					return false;

				}
				pMasterDatabase->CancelImportedProjectFlag();
			}
			SpecialUpdateForOldPeoject(pMasterDatabase,strProjName);
		}

	
		//convert the database
		CProjectUpdater projUpdater(m_strAppPath,strProjName);
		if(projUpdater.ConvertDataToAccess(strArcportdbPath))
		{
			//detach sql server database
			CString strAttachName = _T("ARCConvertDB");
			//pMasterDatabase->Connect();
			//rename sql server file
			{
				DetachDatabase(pMasterDatabase,strAttachName);
				//strInputPath
				//CString strConvertedMdfFile = strInputPath +_T("\\sql_converted.mdf");
				//CString strConvertedLdfFile = strInputPath +_T("\\sql_converted.ldf");

				//CFile::Rename(strMDFFilePath,strConvertedMdfFile);
				//CFile::Rename(strLDFFilePath,strConvertedLdfFile);


			}
			//update the database version to 1071
			{
				CString strSQL;
				strSQL.Format(_T("UPDATE SYSTEM_VERSION SET DBVERSION = 1073 WHERE NAME = 'ARCPORT'"));
				try
				{
					_variant_t varCount;
					DATABASECONNECTION.GetConnection()->Execute((_bstr_t)strSQL,&varCount,ADODB::adOptionUnspecified);

				}
				catch (_com_error& e)
				{
					CString strLogMessage;
					strLogMessage = _T("Error execute access sql statement:  ");
					strLogMessage += (char *)e.Description();
					strLogMessage += strSQL;
					pMasterDatabase->Log( strProjName,strLogMessage);
					DATABASECONNECTION.writeLog(strLogMessage);
					MessageBox(NULL,_T("Convert project Failed, please check ProjectLog.log file in Input folder for detail. "),_T("ARCport Error"),MB_OK);
					return false;

				}
				catch (CADOException& e)
				{
					CString strLogMessage;
					strLogMessage = _T("Error execute access sql statement:  ");
					strLogMessage += e.ErrorMessage();
					strLogMessage += strSQL;
					pMasterDatabase->Log( strProjName,strLogMessage);
					DATABASECONNECTION.writeLog(strLogMessage);
					MessageBox(NULL,_T("Convert project Failed, please check ProjectLog.log file in Input folder for detail. "),_T("ARCport Error"),MB_OK);
					return false;

				}

			}
			bool bUpdateSuccess = false;
			try
			{
				ARCProjectExtraUpdate extraUpdate(strProjName);
				bUpdateSuccess=	projUpdater.UpdateProject(&extraUpdate);
			}
			catch(CADOException& )
			{
				MessageBox(NULL,_T("Convert project Failed, please check ProjectLog.log file in Input folder for detail. "),_T("ARCport Error"),MB_OK);
				bUpdateSuccess= false;
			}
			return bUpdateSuccess;
		}
		else
		{
			//delete arcport.mdb
			::DeleteFile(strArcportdbPath);
			MessageBox(NULL,_T("Convert project Failed, please check ProjectLog.log file in Input folder for detail. "),_T("ARCport Error"),MB_OK);

		}
		
		//update database

		return false;
	}

	return false;

//	{
//
//	CString strLogMessage = _T("");
//	strLogMessage = _T("Open Project ");
//	pMasterDatabase->Log(strProjName,strLogMessage + strProjName); //log
//
//	strLogMessage = _T("Is Old Project .....");
//	if(pMasterDatabase->IsOldProject(strProjName))//old
//	{
//		strLogMessage += _T("YES");
//		pMasterDatabase->Log(strProjName,strLogMessage);
//
//		//convert old project
//		pMasterDatabase->OpenOldProject(strProjName,m_strAppPath);
//	}
//	else
//	{
//		strLogMessage +=_T("NO");
//		pMasterDatabase->Log(strProjName,strLogMessage);
//	}
//
//	//open project
//	CString strDatabaseName = _T("");
//	pMasterDatabase->GetProjectDatabaseName(strProjName, strDatabaseName);
//	
// 
//	try
//	{
//		CString strDataFile = _T("");
//		CString strLogFile = _T("");
//
//		strDataFile.Format(_T("%s\\input\\%s.mdf"),strProjPath,strDatabaseName);
//		strLogFile.Format(_T("%s\\input\\%s.ldf"),strProjPath,strDatabaseName);
//		
//		
//		CString strOldDBName = pMasterDatabase->GetImportedProjectDatabase();
//		strOldDBName.Trim();
//		if((pMasterDatabase->IsOpenImportedProject() == true) &&strOldDBName  != _T(""))//import
//		{
//			CString strAttachDatabaseName =	LinkDatabaseNameWithARC(strDatabaseName);
//
//			strLogMessage = _T("Open imported project ....");
//			pMasterDatabase->Log(strProjName,strLogMessage);
//
//
//			if (pMasterDatabase->IsDatabaseAttached(strAttachDatabaseName))
//			{
//				strLogMessage = _T("Detach database ....");
//				pMasterDatabase->Log(strProjName,strLogMessage);
//				//detach database
//				DetachDatabase(pMasterDatabase,strDatabaseName);
//			}
//
//			BOOL bFileExist = PathFileExists(strDataFile);
//			if(bFileExist == FALSE)
//			{	
//				CString strDataSourcePath = _T("");
//				CString strLogSourcePath = _T("");
//
//				CString strOldDatabaseName = pMasterDatabase->GetImportedProjectDatabase();
//				//copy file
//				strLogMessage = _T("Old database Name : ");
//				pMasterDatabase->Log(strProjName,strLogMessage + strOldDatabaseName);
//				strDataSourcePath.Format(_T("%s\\input\\%s.mdf"),strProjPath,strOldDatabaseName);
//				strLogSourcePath.Format(_T("%s\\input\\%s.ldf"),strProjPath,strOldDatabaseName);
//				
//				if(PathFileExists(strDataSourcePath) != FALSE && PathFileExists(strLogSourcePath) != FALSE)
//				{
//
//					try
//					{
//						CFile::Rename(strDataSourcePath,strDataFile);
//						CFile::Rename(strLogSourcePath,strLogFile);
//						strLogMessage.Format(_T("Rename file  %s -> %s, %s -> %s"),
//							strDataSourcePath,strDataFile,strLogSourcePath,strLogFile);
//						pMasterDatabase->Log(strProjName,strLogMessage);
//
//					}
//					catch (CFileException* e)
//					{
//						strLogMessage.Format(_T("Rename file failed %s -> %s, %s -> %s,cause: %d"),
//							strDataSourcePath,strDataFile,strLogSourcePath,strLogFile,e->m_cause);
//						pMasterDatabase->Log(strProjName,strLogMessage);								
//					}
//				}
//			}
//
//			//path file not exists
//	
//			bFileExist = PathFileExists(strDataFile);
//			if (bFileExist == FALSE)
//			{	
//				BOOL bFindDBFile = FALSE;
//				//search the mdf files from INPUT folder
//
//
//				CString strTempProjPath;
//				strTempProjPath.Format("%s\\input",strProjPath);
//				TCHAR oldDir[MAX_PATH+1];
//				GetCurrentDirectory(MAX_PATH, oldDir);
//				SetCurrentDirectory(strTempProjPath);
//
//
//				WIN32_FIND_DATA FindFileData;
//				HANDLE hFind = FindFirstFile("*.*", &FindFileData);	// gets "."
//				FindNextFile(hFind, &FindFileData);				// gets ".."
//				while(GetLastError() != ERROR_NO_MORE_FILES) 
//				{
//					if(FindNextFile(hFind, &FindFileData)) 
//					{
//						if(!(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) 
//						{
//							if (CString(FindFileData.cFileName).Right(4).CompareNoCase(_T(".mdf")) == 0)
//							{	
//								CString strExistDatabaseName = _T("");
//
//								CString strExistDataFileName = _T("");
//								CString strExistLogFileName = _T("");
//								strLogMessage = _T("Find database file successed  ");
//								pMasterDatabase->Log(strProjName,strLogMessage + CString(FindFileData.cFileName) );
//
//								strExistDatabaseName = FindFileData.cFileName;
//								strExistDatabaseName = strExistDatabaseName.Left(strExistDatabaseName.GetLength() - 4);
//								
//								strExistDataFileName.Format(_T("%s\\input\\%s"),strProjPath,FindFileData.cFileName);							
//								strExistLogFileName.Format(_T("%s\\input\\%s.ldf"),strProjPath,strExistDatabaseName);
//
//								if (PathFileExists(strExistLogFileName) == TRUE)
//								{
//									//find one database
//									bFindDBFile = TRUE;
//									try
//									{
//										CFile::Rename(strExistDataFileName,strDataFile);
//										CFile::Rename(strExistLogFileName,strLogFile);
//										strLogMessage.Format(_T("Rename file  %s -> %s, %s -> %s"),
//											strExistDataFileName,strDataFile,strExistLogFileName,strLogFile);
//										pMasterDatabase->Log(strProjName,strLogMessage);
//
//									}
//									catch (CFileException* e)
//									{
//										strLogMessage.Format(_T("Rename file failed %s -> %s, %s -> %s,cause: %d"),
//											strExistDataFileName,strDataFile,strExistLogFileName,strLogFile,e->m_cause);
//										pMasterDatabase->Log(strProjName,strLogMessage);								
//									}
//
//									break;
//								}
//
//							}
//
//						}
//					}
//					else
//						continue;				
//				}
//				FindClose(hFind);
// 				SetCurrentDirectory(oldDir);
// 				SetLastError(0);
//
//				//bFileExist = PathFileExists(strDataFile);
//				if (bFindDBFile == FALSE)//default database
//				{
//
//					CString strAppPath = m_strAppPath;
//					strAppPath.TrimRight("\\");
//					CString strDataSourcePath = strAppPath + _T("\\arctermdb\\temp\\ARCPort_Data.MDF");
//					CString strLogSourcePath= strAppPath + _T("\\arctermdb\\temp\\ARCPort_Log.LDF");
//
//					strLogMessage.Format(_T("Create new database files, Copy temp database file,%s->%s; %s->%s"),
//						strDataSourcePath,strDataFile,strLogSourcePath,strLogFile);
//					pMasterDatabase->Log(strProjName,strLogMessage);
//
//
//					CopyFile(strDataSourcePath,strDataFile,TRUE);
//					CopyFile(strLogSourcePath,strLogFile,TRUE);
//
//				}
//			}
//
//			CString strSQL;
//
//			strSQL.Format(_T("sp_attach_db @dbname ='%s',@filename1='%s',@filename2='%s'"),strAttachDatabaseName,strDataFile,strLogFile);
//
//			pMasterDatabase->Execute(strSQL);
//
//		}
//		else//new version
//		{
//			//if (pMasterDatabase->IsProjectDataNameExist(strProjName) == false && pMasterDatabase->IsDatabaseNameExist(strDatabaseName) ==false)
//			//{
//			//	CString strAppPath = m_strAppPath;
//			//	strAppPath.TrimRight("\\");
//			//	CString strDataSourcePath = strAppPath + _T("\\arctermdb\\temp\\ARCPort_Data.MDF");
//			//	CString strLogSourcePath= strAppPath + _T("\\arctermdb\\temp\\ARCPort_Log.LDF");
//
//			//	CopyFile(strDataSourcePath,strDataFile,TRUE);
//			//	CopyFile(strLogSourcePath,strLogFile,TRUE);
//   //            
//			//	CString strSQL;
//
//			//	strSQL.Format(_T("sp_attach_db @dbname ='%s',@filename1='%s',@filename2='%s'"),strDatabaseName,strDataFile,strLogFile);
//
//			//	pMasterDatabase->Execute(strSQL);
//			//}
//			//else
//			//{
//				AttachDatabase(pMasterDatabase,strProjPath,strDatabaseName,strProjName);
//			//}
//		}
//	}
//	catch (_com_error& e)
//	{
//		strLogMessage = _T("Error attach project database:  ");
//		strLogMessage += (char *)e.Description();
//		pMasterDatabase->Log(strProjName,strLogMessage);
//		return ;
//	}
//
//	//connect to database	
//	ReadConnectInfoFromReg();
//	m_strDataBase = strDatabaseName;//database name
//	CString strDatabaseInstanceName = LinkDatabaseNameWithARC(strDatabaseName);
//	CString strConnectString;
//	strConnectString.Format(_T("Provider=SQLOLEDB;Data Source=%s;Initial Catalog=%s;User ID=%s;Password=%s"),
//								m_strServer.c_str(),strDatabaseInstanceName, m_strUser.c_str(), m_strPassWord.c_str());
//	//connect
//	try
//	{
//		
//		DATABASECONNECTION.SetConnectString(strConnectString);
//		DATABASECONNECTION.GetConnection();
//		
//	}
//	catch (_com_error& e)
//	{	
//		strLogMessage = _T("Error open project database connection:  ");
//		strLogMessage += (char *)e.Description();
//		strLogMessage += strConnectString;
//		pMasterDatabase->Log(strProjName,strLogMessage);
//		return ;
//	}
//	try
//	{	
//		strLogMessage = _T("Update Project ..:  ");
//		pMasterDatabase->Log(strProjName,strLogMessage);
//		UpdateProject();
//	}
//	catch (_com_error& e)
//	{	
//		strLogMessage = _T("Error Update Project ..:  ");
//		strLogMessage += (char *)e.Description();
//		strLogMessage += strConnectString;
//		pMasterDatabase->Log(strProjName,strLogMessage);
//	}
//
//	//if imported project
//	if (pMasterDatabase->IsOpenImportedProject())
//	{
//		CString strOldProjName = pMasterDatabase->GetImportedProjectName();
//
//		CString strSQL;
//		strSQL.Format(_T("UPDATE PROJECT SET NAME = '%s' WHERE NAME = '%s'"),strProjName,strOldProjName);
//		try
//		{
//			_variant_t varCount;
//			//DATABASECONNECTION.GetConnection()->Execute(_T("SELECT * FROM PROJECT"),&varCount,ADODB::adOptionUnspecified);
//			DATABASECONNECTION.GetConnection()->Execute((_bstr_t)strSQL,&varCount,ADODB::adOptionUnspecified);
//
//		}
//		catch (_com_error& e)
//		{
//			strLogMessage = _T("Error execute sql statement:  ");
//			strLogMessage += (char *)e.Description();
//			strLogMessage += strSQL;
//			pMasterDatabase->Log( strProjName,strLogMessage);
//
//			MessageBox(NULL,_T("Open Project Failed."),_T("WARNING"),MB_OK);
//		}
//
//		pMasterDatabase->CancelImportedProjectFlag();
//	}
//
//	SpecialUpdateForOldPeoject(pMasterDatabase,strProjName);
//	//update database 
//	strLogMessage = _T("Open Project Success  ");
//	pMasterDatabase->Log(strProjName,strLogMessage);
//}

}
void CProjectDatabase::ShrinkDatabse(const CString& strProjName)
{
	//update database from 1045 to 1046

}
void CProjectDatabase::CloseProject(CMasterDatabase *pMasterDatabase,const CString& strProjName)
{
//	CString strDatabaseName = _T("");
//	if(pMasterDatabase->GetProjectDatabaseNameFromDB(strProjName, strDatabaseName))
//		DetachDatabase(pMasterDatabase, strDatabaseName);
	DATABASECONNECTION.CloseConnection(DATABASESOURCE_TYPE_SQLSERVER);
	DATABASECONNECTION.CloseConnection(DATABASESOURCE_TYPE_ACCESS);
}
void CProjectDatabase::ImportProject(CMasterDatabase *pMasterDatabase,const CString& strProjName,const CString& strProjPath)
{

}
void CProjectDatabase::ExportProjectBegin(CMasterDatabase *pMasterDatabase,const CString& strProjName,const CString& strProjPath)
{
	CloseProject(pMasterDatabase,strProjName);
	DATABASECONNECTION.CloseConnection(DATABASESOURCE_TYPE_ACCESS);

}
void CProjectDatabase::ExportProjectEnd(CMasterDatabase *pMasterDatabase,const CString& strProjName,const CString& strProjPath)
{
	OpenProject(pMasterDatabase,strProjName,strProjPath);

}
bool CProjectDatabase::AttachDatabase(CMasterDatabase *pMasterDatabase,const CString& strProjPath,CString& strDatabaseName,const CString& strProjName)
{	
	
	if (pMasterDatabase->IsDatabaseAttached(strDatabaseName))
	{
		//detach database
		DetachDatabase(pMasterDatabase,strDatabaseName);
		Sleep(500);
	}
	//attach database

	CString strLogMessage = _T("");
	CString strDataFile = _T("");
	CString strLogFile = _T("");

	strDataFile.Format(_T("%s\\input\\%s.mdf"),strProjPath,strDatabaseName);
	strLogFile.Format(_T("%s\\input\\%s.ldf"),strProjPath,strDatabaseName);
	BOOL bFileExist = PathFileExists(strDataFile);
	
	

	if(PathFileExists(strDataFile) == FALSE || PathFileExists(strLogFile) == FALSE)
	{
	
		BOOL bFindDBFile = FALSE;

		CString strDataFileName = _T("");
		CString strLogFileName = _T("");

		CString strDataSourcePath = _T("");
		CString strLogSourcePath = _T("");

		CString strTempProjPath;
		strTempProjPath.Format("%s\\input",strProjPath);
		TCHAR oldDir[MAX_PATH+1];
		GetCurrentDirectory(MAX_PATH, oldDir);
		SetCurrentDirectory(strTempProjPath);



		WIN32_FIND_DATA FindFileData;
		HANDLE hFind = FindFirstFile("*.*", &FindFileData);	// gets "."
		FindNextFile(hFind, &FindFileData);				// gets ".."
		while(GetLastError() != ERROR_NO_MORE_FILES)
		{
			if(FindNextFile(hFind, &FindFileData)) 
			{
				if(!(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) 
				{
					if (CString(FindFileData.cFileName).Right(4).CompareNoCase(_T(".mdf")) == 0)
					{	
						CString strExistDatabaseName = _T("");

						CString strExistDataFileName = _T("");
						CString strExistLogFileName = _T("");
						strLogMessage = _T("Find database file successed ");
						pMasterDatabase->Log(strProjName,strLogMessage);

						strExistDatabaseName = FindFileData.cFileName;
						strExistDatabaseName = strExistDatabaseName.Left(strExistDatabaseName.GetLength() - 4);

						strExistDataFileName.Format(_T("%s\\input\\%s"),strProjPath,FindFileData.cFileName);							
						strExistLogFileName.Format(_T("%s\\input\\%s.ldf"),strProjPath,strExistDatabaseName);

						if (PathFileExists(strExistLogFileName) == TRUE)
						{
							//find one database
							bFindDBFile = TRUE;
							try
							{
								CFile::Rename(strExistDataFileName,strDataFile);
								CFile::Rename(strExistLogFileName,strLogFile);
								strLogMessage.Format(_T("Rename file  %s -> %s, %s -> %s"),
									strExistDataFileName,strDataFile,strExistLogFileName,strLogFile);
								pMasterDatabase->Log(strProjName,strLogMessage);

							}
							catch (CFileException* e)
							{
								strLogMessage.Format(_T("Rename file failed %s -> %s, %s -> %s,cause: %d"),
									strExistDataFileName,strDataFile,strExistLogFileName,strLogFile,e->m_cause);
								pMasterDatabase->Log(strProjName,strLogMessage);								
							}
							
							
							//CopyFile(strExistLogFileName,strLogFile,FALSE);
							//CopyFile(strExistDataFileName,strDataFile,FALSE);

							//DeleteFile(strExistDataFileName);
							//DeleteFile(strExistLogFileName);

							break;
						}

					}

				}
			}
			else
				continue;				
		}
		FindClose(hFind);
		SetCurrentDirectory(oldDir);
		SetLastError(0);
		
		
		//bFileExist = PathFileExists(strDataFile);
		if(bFindDBFile == FALSE)
		{
			//copy file
			CString strAppPath = m_strAppPath;
			strAppPath.TrimRight("\\");
			CString strDataSourcePath = strAppPath + _T("\\arctermdb\\temp\\ARCPort_Data.MDF");
			CString strLogSourcePath= strAppPath + _T("\\arctermdb\\temp\\ARCPort_Log.LDF");
			
			strLogMessage.Format(_T("Create new database files, Copy temp database file,%s->%s; %s->%s"),
				strDataSourcePath,strDataFile,strLogSourcePath,strLogFile);
			pMasterDatabase->Log(strProjName,strLogMessage);

			CopyFile(strDataSourcePath,strDataFile,TRUE);
			CopyFile(strLogSourcePath,strLogFile,TRUE);


		}
	}

	CString strSQL;

	CString strAttachDatabaseName = LinkDatabaseNameWithARC(strDatabaseName);
	strSQL.Format(_T("sp_attach_db @dbname ='%s',@filename1='%s',@filename2='%s'"),strAttachDatabaseName,strDataFile,strLogFile);

	pMasterDatabase->Execute(strSQL);

	return true;

}
void CProjectDatabase::DetachDatabase(CMasterDatabase *pMasterDatabase,CString& strDatabaseName)
{

	CString strAttachDatabaseName = strDatabaseName;//;LinkDatabaseNameWithARC(strDatabaseName);

	if (!pMasterDatabase->IsDatabaseAttached(strAttachDatabaseName))
		return;	
	
	CString strError = _T("");
	try
	{
		DATABASECONNECTION.CloseConnection(DATABASESOURCE_TYPE_SQLSERVER);



		//kill all database connection
		//status   =   'sleeping' And 
		CString strSQLFormat = _T("Declare @sql   nvarchar( 500 ),\
							@temp   varchar(1000),\
							@spid    int\
							Declare getspid Cursor For Select    spid     From    sys.sysprocesses     \
							Where     spid > 50 and spid <=32767 AND dbid In \
							( Select dbid From sys.sysdatabases Where Name= '%s' )\
							Open getspid\
							Fetch Next From getspid into @spid\
							While @@fetch_status = 0\
							Begin\
							If( @spid > 50 )\
							Begin\
							Set @temp = 'kill ' + rtrim( @spid )\
							Exec( @temp )\
							End\
							Fetch Next From getspid Into @spid\
							End\
							Close getspid\
							Deallocate getspid");

		CString strSQLKillConn;

		strSQLKillConn.Format(strSQLFormat,strAttachDatabaseName);

		pMasterDatabase->Execute(strSQLKillConn);
	}
	catch(_com_error& e)
	{
		strError = (char *)e.Description();
		DATABASECONNECTION.writeLog("kill connection" + strError);
		ASSERT(0);
	}

	CString strSQL = _T("");

	try
	{

		strSQL.Format(_T("sp_detach_db %s"),strAttachDatabaseName);
		
		//execute detach
		pMasterDatabase->Execute(strSQL);
	}
	catch(_com_error& e)
	{
		strError = (char *)e.Description();
		DATABASECONNECTION.writeLog(_T("detach eror") + strSQL + strError);
		ASSERT(0);
	}


}
//read connect string from database
bool CProjectDatabase::ReadConnectInfoFromReg()
{

	//////////TCHAR RegSubKey[] = _T("SOFTWARE\\Aviation Research Corporation\\ARCTerm\\DataBase");
	//////////TCHAR RegServerName[] = _T("Server");
	//////////TCHAR RegDataBaseName[] = _T("DataBase");
	//////////TCHAR RegUserName[] = _T("UserName");
	//////////TCHAR RegPassWord[] = _T("Password");

	//////////HKEY RegKey;
	//////////CString strTemp;

	//////////if (RegOpenKey(HKEY_LOCAL_MACHINE, RegSubKey, &RegKey) != ERROR_SUCCESS)
	//////////	return false;
    ///read database message from file instead of register , been modified by cj ,for task 361.
	//date: 2008/9/1
	CString strTemp;
	CReadRegister regReader ;
	CString strError;
	if(!regReader.ReadRegisterPath(strError))
	{
		DATABASECONNECTION.writeLog(_T("regReader.ReadRegisterPath")+ strError);
		return FALSE ;
	}
	CINIFileForDBHandle handle ;
	if(!handle.LoadFile(regReader.m_Path.m_AppPath))
	{
		DATABASECONNECTION.writeLog(_T("handle.LoadFile(regReader.m_Path.m_AppPath)") + CString(regReader.m_Path.m_AppPath) );
		return false ;
	}
	//server name 
    if(!handle.GetDBServer(strTemp))
		return false ;
	m_strServer = strTemp;
	//database name 
	if(!handle.GetDataBaseName(strTemp))
		return false ;
	m_strDataBase = strTemp;
	//user name
	if(!handle.GetDBUserName(strTemp))
		return false ;
	m_strUser = strTemp ;
	//passeord
	if(!handle.GetDBPassword(strTemp))
		return false ;
	m_strPassWord = strTemp;


	// server name
	//if (!ReadDBItem(RegKey, RegServerName, strTemp))
	//	return false;
	//m_strServer = strTemp;

	// database name
	//if (!ReadDBItem(RegKey, RegDataBaseName, strTemp))
	//	return false;
	//m_strDataBase = strTemp;

	// user name
	//if (!ReadDBItem(RegKey, RegUserName, strTemp))
	//	return false;
	//m_strUser = strTemp;

	// password
	//if (!ReadDBItem(RegKey, RegPassWord, strTemp))
	//	return false;
	//m_strPassWord = strTemp;


	//RegCloseKey(RegKey);

	return true;

}
bool CProjectDatabase::ReadDBItem(HKEY hKey, LPCSTR lpEntry, std::string& strResult)
{
	unsigned long lDataLength;
	unsigned char* szReigterWord = new unsigned char[1024];
	//unsigned char szReigterWord[256];
	strResult.clear();

	long lQueryResult = RegQueryValueEx(hKey, lpEntry, NULL, NULL, szReigterWord, &lDataLength);
	if (lQueryResult == ERROR_SUCCESS)
	{
		if (lDataLength > 0)
			strResult = (LPSTR)szReigterWord;
	}
	else
	{
		RegCloseKey(hKey);
		delete [] szReigterWord;
		return false;
	}

	delete [] szReigterWord;
	return true;
}
void CProjectDatabase::UpdateProject()
{
	DBUpdater dpUpdate(m_strAppPath);
	dpUpdate.Update();
}

CString CProjectDatabase::LinkDatabaseNameWithARC( const CString& strFormerName )
{
	CString strReturnName = _T("ARC");
	strReturnName += strFormerName;

	return strReturnName;
}

void CProjectDatabase::SpecialUpdateForOldPeoject(CMasterDatabase *pMaseterDatabase, const CString& strprojName)
{
	//get project id and update flag
	int nProjID = -1;
	int nUpdateFlag = -1;

	{
		CString strSQL = _T("");
		strSQL.Format(_T("SELECT ID,UPDATEFLAG FROM PROJECT WHERE NAME = '%s'"),strprojName);

		ADODB::_RecordsetPtr pRecordset= NULL;
		try
		{	
			_variant_t varCount;
			pRecordset= DATABASECONNECTION.GetConnection(DATABASESOURCE_TYPE_SQLSERVER)->Execute((_bstr_t)strSQL,&varCount,ADODB::adOptionUnspecified );

			if(pRecordset->GetadoEOF() == VARIANT_FALSE)
			{
				_variant_t varValue = pRecordset->GetCollect("ID");
				if(varValue.vt != VT_NULL)
				{
					nProjID = varValue.intVal;
				}
				
				varValue.Clear();
				varValue = pRecordset->GetCollect("UPDATEFLAG");
				if(varValue.vt != VT_NULL)
				{
					nUpdateFlag = varValue.intVal;
				}
			}
		}
		catch (_com_error& e)
		{
	
			CString strLogMessage = CString((char *)e.Description());
		}
		if(pRecordset)
			if(pRecordset->State == ADODB::adStateOpen)
				pRecordset->Close();
	}

	//check the update flag 
	//default flag is 0, don't need to update

	if (nProjID >= 0 &&  nUpdateFlag == 0)
	{//update
		
		try
		{
			//delete IN_INTRAILSEPARATIONEX
			CString strSQL =_T("");
			strSQL.Format(_T("DELETE FROM IN_INTRAILSEPARATIONEX WHERE PROJID <> %d"),nProjID);
			
			_variant_t varCount;
			DATABASECONNECTION.GetConnection(DATABASESOURCE_TYPE_SQLSERVER)->CommandTimeout = 999999;
			DATABASECONNECTION.GetConnection(DATABASESOURCE_TYPE_SQLSERVER)->Execute(_bstr_t(strSQL),&varCount,ADODB::adOptionUnspecified);
		
			//APPROACHSEPARATIONCRITERIA
			strSQL.Format(_T("DELETE FROM APPROACHSEPARATIONCRITERIA WHERE PROJID <> %d"),nProjID);
			DATABASECONNECTION.GetConnection(DATABASESOURCE_TYPE_SQLSERVER)->Execute(_bstr_t(strSQL),&varCount,ADODB::adOptionUnspecified);

			//IN_TAKEOFFCLEARANCE
			strSQL.Format(_T("DELETE FROM IN_TAKEOFFCLEARANCE WHERE PROJID <> %d"),nProjID);
			DATABASECONNECTION.GetConnection(DATABASESOURCE_TYPE_SQLSERVER)->Execute(_bstr_t(strSQL),&varCount,ADODB::adOptionUnspecified);

			//IN_VEHICLESEPCIFICATION
			strSQL.Format(_T("DELETE FROM IN_VEHICLESEPCIFICATION WHERE PROJECTID <> %d"),nProjID);
			DATABASECONNECTION.GetConnection(DATABASESOURCE_TYPE_SQLSERVER)->Execute(_bstr_t(strSQL),&varCount,ADODB::adOptionUnspecified);

			CString strUpdateFlagSql = _T("");
			strUpdateFlagSql.Format(_T("UPDATE PROJECT SET UPDATEFLAG = 100"));
			DATABASECONNECTION.GetConnection(DATABASESOURCE_TYPE_SQLSERVER)->Execute(_bstr_t(strUpdateFlagSql),&varCount,ADODB::adOptionUnspecified);

		}
		catch (_com_error& e)
		{

			CString strLogMessage = CString((char *)e.Description());
			return;
		}



	}


}

bool CProjectDatabase::OpenAccessDatabase( CMasterDatabase *pMasterDatabase,const CString& strProjName,const CString& strProjPath )
{
	//check the database is exist or not
	//open Access connection
	//set access database file path

	CString strDataFile = _T("");
	CString strLogFile = _T("");
	
	strDataFile.Format(_T("%s\\input\\arcport.mdb"),strProjPath);
	//check the database is exit or not
	if(!PathFileExists(strDataFile))
	{
		//copy file
		CString strAppPath = m_strAppPath;
		strAppPath.TrimRight("\\");
		CString strDataSourcePath = strAppPath + _T("\\arctermdb\\temp\\arcport.mdb");


		CString strLogMessage;
		strLogMessage.Format(_T("Create new access database files, Copy temp database file,%s->%s"),
			strDataSourcePath,strDataFile);
		pMasterDatabase->Log(strProjName,strLogMessage);

		CopyFile(strDataSourcePath,strDataFile,TRUE);

	}

	DATABASECONNECTION.SetAccessDatabasePath(strProjPath + _T("\\input\\arcport.mdb"));
	DATABASECONNECTION.GetConnection(DATABASESOURCE_TYPE_ACCESS);

	
	return true;
}
bool CProjectDatabase::FindSQLServerDataFiles( const CString& strInputPath,CString& strMdfFile,CString& strLdfFile,CString& strFileName )
{

	bool bFindDBFile = false;

	CString strTempProjPath= strInputPath;

	TCHAR oldDir[MAX_PATH+1];
	GetCurrentDirectory(MAX_PATH, oldDir);
	SetCurrentDirectory(strTempProjPath);


	WIN32_FIND_DATA FindFileData;
	HANDLE hFind = FindFirstFile("*.*", &FindFileData);	// gets "."
	FindNextFile(hFind, &FindFileData);				// gets ".."
	while(GetLastError() != ERROR_NO_MORE_FILES) 
	{
		if(FindNextFile(hFind, &FindFileData)) 
		{
			if(!(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) 
			{
				if (CString(FindFileData.cFileName).Right(4).CompareNoCase(_T(".mdf")) == 0)
				{	
					CString strExistDatabaseName = _T("");

					CString strExistDataFileName = _T("");
					CString strExistLogFileName = _T("");


					strExistDatabaseName = FindFileData.cFileName;
					strExistDatabaseName = strExistDatabaseName.Left(strExistDatabaseName.GetLength() - 4);

					strExistDataFileName.Format(_T("%s\\%s"),strTempProjPath,FindFileData.cFileName);							
					strExistLogFileName.Format(_T("%s\\%s.ldf"),strTempProjPath,strExistDatabaseName);
					strFileName = strExistDatabaseName;
					strMdfFile = strExistDataFileName;
					strLdfFile = strExistLogFileName;
					if (PathFileExists(strExistLogFileName) == TRUE)
					{
						bFindDBFile = true ;
						break;
					}

				}

			}
		}
		else
			continue;				
	}
	FindClose(hFind);
	SetCurrentDirectory(oldDir);
	SetLastError(0);

	return bFindDBFile;

}
//check the database have the project name or not
//if only have one project in the database, it will return TRUE, and assign the database name to project name
bool CProjectDatabase::IsProjectExistsInDB(CString& strProjName)
{
	CString strSQL = _T("");
	strSQL.Format(_T("SELECT NAME  FROM PROJECT  WHERE NAME = '%s'"),strProjName);

	long nCount = 0;
	CADORecordset adRecordset;
	CADODatabase::ExecuteSQLStatement(strSQL,nCount,adRecordset);


	if(adRecordset.IsEOF())
	{	
		CString strTempName = _T("");
		CString strCheckSQL = _T("SELECT NAME  FROM PROJECT");
		CADORecordset adCheckRecordset;
		CADODatabase::ExecuteSQLStatement(strCheckSQL,nCount,adCheckRecordset);
		if(!adCheckRecordset.IsEOF())
		{
			adCheckRecordset.GetFieldValue(_T("NAME"),strTempName);

			adCheckRecordset.MoveNextData();

			if(!adCheckRecordset.IsEOF())//more than 1 project
				return false;
			else//one only
				strProjName = strTempName;

			return true;
		}

		return false;
	}
	return true;
}

bool CProjectDatabase::IsProjectExistsInDB( const CString& strProjName )
{
	CString strSQL = _T("");
	strSQL.Format(_T("SELECT NAME  FROM PROJECT  WHERE NAME = '%s'"),strProjName);

	long nCount = 0;
	CADORecordset adRecordset;
	CADODatabase::ExecuteSQLStatement(strSQL,nCount,adRecordset);

	if(adRecordset.IsEOF())
	{
		return false;
	}
	return true;
}

bool CProjectDatabase::startsqlserver()
{
	TCHAR szSvcName[80] = _T("mssql$arcport");

	SC_HANDLE schSCManager;
	SC_HANDLE schService;


		SERVICE_STATUS_PROCESS ssStatus; 
		DWORD dwOldCheckPoint; 
		DWORD dwStartTickCount;
		DWORD dwWaitTime;
		DWORD dwBytesNeeded;

		// Get a handle to the SCM database. 

		schSCManager = OpenSCManager( 
			NULL,                    // local computer
			NULL,                    // servicesActive database 
			SC_MANAGER_ALL_ACCESS);  // full access rights 

		if (NULL == schSCManager) 
		{
			printf("OpenSCManager failed (%d)\n", GetLastError());
			return false;
		}

		// Get a handle to the service.

		schService = OpenService( 
			schSCManager,         // SCM database 
			szSvcName,            // name of service 
			SERVICE_ALL_ACCESS);  // full access 

		if (schService == NULL)
		{ 
			printf("OpenService failed (%d)\n", GetLastError()); 
			CloseServiceHandle(schSCManager);
			return false;
		}    

		// Check the status in case the service is not stopped. 

		if (!QueryServiceStatusEx( 
			schService,                     // handle to service 
			SC_STATUS_PROCESS_INFO,         // information level
			(LPBYTE) &ssStatus,             // address of structure
			sizeof(SERVICE_STATUS_PROCESS), // size of structure
			&dwBytesNeeded ) )              // size needed if buffer is too small
		{
			printf("QueryServiceStatusEx failed (%d)\n", GetLastError());
			CloseServiceHandle(schService); 
			CloseServiceHandle(schSCManager);
			return false; 
		}

		// Check if the service is already running. It would be possible
		// to stop the service here, but for simplicity this example just returns. 

		if(ssStatus.dwCurrentState != SERVICE_STOPPED && ssStatus.dwCurrentState != SERVICE_STOP_PENDING)
		{
			printf("Cannot start the service because it is already running\n");
			CloseServiceHandle(schService); 
			CloseServiceHandle(schSCManager);
			return true; 
		}

		// Wait for the service to stop before attempting to start it.

		while (ssStatus.dwCurrentState == SERVICE_STOP_PENDING)
		{
			// Save the tick count and initial checkpoint.

			dwStartTickCount = GetTickCount();
			dwOldCheckPoint = ssStatus.dwCheckPoint;

			// Do not wait longer than the wait hint. A good interval is 
			// one-tenth of the wait hint but not less than 1 second  
			// and not more than 10 seconds. 

			dwWaitTime = ssStatus.dwWaitHint / 10;

			if( dwWaitTime < 3000 )
				dwWaitTime = 3000;
			else if ( dwWaitTime > 30000 )
				dwWaitTime = 30000;

			Sleep( dwWaitTime );

			// Check the status until the service is no longer stop pending. 

			if (!QueryServiceStatusEx( 
				schService,                     // handle to service 
				SC_STATUS_PROCESS_INFO,         // information level
				(LPBYTE) &ssStatus,             // address of structure
				sizeof(SERVICE_STATUS_PROCESS), // size of structure
				&dwBytesNeeded ) )              // size needed if buffer is too small
			{
				printf("QueryServiceStatusEx failed (%d)\n", GetLastError());
				CloseServiceHandle(schService); 
				CloseServiceHandle(schSCManager);
				return false; 
			}

			if ( ssStatus.dwCheckPoint > dwOldCheckPoint )
			{
				// Continue to wait and check.

				dwStartTickCount = GetTickCount();
				dwOldCheckPoint = ssStatus.dwCheckPoint;
			}
			else
			{
				if(GetTickCount()-dwStartTickCount > ssStatus.dwWaitHint)
				{
					printf("Timeout waiting for service to stop\n");
					CloseServiceHandle(schService); 
					CloseServiceHandle(schSCManager);
					return false; 
				}
			}
		}

		// Attempt to start the service.

		if (!StartService(
			schService,  // handle to service 
			0,           // number of arguments 
			NULL) )      // no arguments 
		{
			printf("StartService failed (%d)\n", GetLastError());
			CloseServiceHandle(schService); 
			CloseServiceHandle(schSCManager);
			return false; 
		}
		else printf("Service start pending...\n"); 

		// Check the status until the service is no longer start pending. 

		if (!QueryServiceStatusEx( 
			schService,                     // handle to service 
			SC_STATUS_PROCESS_INFO,         // info level
			(LPBYTE) &ssStatus,             // address of structure
			sizeof(SERVICE_STATUS_PROCESS), // size of structure
			&dwBytesNeeded ) )              // if buffer too small
		{
			printf("QueryServiceStatusEx failed (%d)\n", GetLastError());
			CloseServiceHandle(schService); 
			CloseServiceHandle(schSCManager);
			return false; 
		}

		// Save the tick count and initial checkpoint.

		dwStartTickCount = GetTickCount();
		dwOldCheckPoint = ssStatus.dwCheckPoint;

		while (ssStatus.dwCurrentState == SERVICE_START_PENDING) 
		{ 
			// Do not wait longer than the wait hint. A good interval is 
			// one-tenth the wait hint, but no less than 1 second and no 
			// more than 10 seconds. 

			dwWaitTime = ssStatus.dwWaitHint / 10;

			if( dwWaitTime < 3000 )
				dwWaitTime = 3000;
			else if ( dwWaitTime > 30000 )
				dwWaitTime = 30000;

			Sleep( dwWaitTime );

			// Check the status again. 

			if (!QueryServiceStatusEx( 
				schService,             // handle to service 
				SC_STATUS_PROCESS_INFO, // info level
				(LPBYTE) &ssStatus,             // address of structure
				sizeof(SERVICE_STATUS_PROCESS), // size of structure
				&dwBytesNeeded ) )              // if buffer too small
			{
				printf("QueryServiceStatusEx failed (%d)\n", GetLastError());
				break; 
			}

			if ( ssStatus.dwCheckPoint > dwOldCheckPoint )
			{
				// Continue to wait and check.

				dwStartTickCount = GetTickCount();
				dwOldCheckPoint = ssStatus.dwCheckPoint;
			}
			else
			{
				if(GetTickCount()-dwStartTickCount > ssStatus.dwWaitHint)
				{
					// No progress made within the wait hint.
					break;
				}
			}
		} 

		// Determine whether the service is running.

		if (ssStatus.dwCurrentState == SERVICE_RUNNING) 
		{
			printf("Service started successfully.\n"); 
			return true;
		}
		else 
		{ 
			printf("Service not started. \n");
			printf("  Current State: %d\n", ssStatus.dwCurrentState); 
			printf("  Exit Code: %d\n", ssStatus.dwWin32ExitCode); 
			printf("  Check Point: %d\n", ssStatus.dwCheckPoint); 
			printf("  Wait Hint: %d\n", ssStatus.dwWaitHint); 
		} 

		CloseServiceHandle(schService); 
		CloseServiceHandle(schSCManager);
	
	return false;
}


BOOL CGlobalAirportDataBase::OpenGoabalAirportDataBase(const CString& _AppPath)
{
	CString strDataFile = _T("");
	CString strLogFile = _T("");
    
	strDataFile.Format(_T("%s\\ImportDB\\GlobalAirportdb.mdb"),_AppPath);
	//check the existing file is valid or not.
	// for there is a invalid release in database collating sequence error, so need to delete
	//the invalid database. 
	//because there is no clear exception  in ADO operation, so, using the  file size to 
	//judge the file, the old file size is 404 KB (413,696 bytes), and the new one is 592 KB (606,208 bytes).
	//if file size is less than 500,000 bytes, assume this file is valid, delete this one
	
	if(PathFileExists(strDataFile))
	{
		FileManager fm ;
		unsigned long nFileSize = fm.FileSize(strDataFile) ;
		
		if(nFileSize> 300000 && nFileSize < 500000)
		{
			::DeleteFile(strDataFile); 
		}
	}
	
	//check the database is exit or not
	if(!PathFileExists(strDataFile))
	{
		//copy file
		
		CString strDataSourcePath = _AppPath + _T("\\arctermdb\\temp\\GlobalAirportdb.mdb");


		CString strLogMessage;
		strLogMessage.Format(_T("Create new access database files, Copy temp database file,%s->%s"),
			strDataSourcePath,strDataFile);
        FileManager fm  ;
		CString dbPath ;
        dbPath.Format(_T("%s\\ImportDB"),_AppPath);
		if(!fm.IsDirectory(dbPath))
			fm.MakePath(dbPath) ;
		if(!CopyFile(strDataSourcePath,strDataFile,TRUE))
		{
			MessageBox(NULL,strLogMessage,"Error",MB_OK) ;
			return FALSE ;
		}
	}

	DATABASECONNECTION.SetGlobalDatabasePath(strDataFile);
	DATABASECONNECTION.GetConnection(DATABASESOURCE_TYPE_ACCESS_GLOBALDB);


	//update the global database 
	CGlobalDataBaseUpdater globalDataBaseUpdater ;
	if(!globalDataBaseUpdater.Update()) 
	{
		CString str ;
		str.Format(_T("Update the Global DataBase error."));
		MessageBox(NULL,str,"Warning",MB_OK) ;
	}

// 	CString strDestinationFilePath(_T(""));
// 	CString strSourceFilePath(_T(""));
// 
// 	strSourceFilePath.Format(_T("%s\\ArctermDB\\temp\\project.mdb"),PROJMANAGER->GetAppPath());
// 	strDestinationFilePath.Format(_T("%s\\ImportDB\\shareTemplate.mdb"),_AppPath);
// 	if (!PathFileExists(strDestinationFilePath))
// 	{
// 		CopyFile(strSourceFilePath,strDestinationFilePath,TRUE);
// 	}
	
	return TRUE ;
}
BOOL CGlobalAirportDataBase::CloseGoabalAirportDataBase()
{
	DATABASECONNECTION.CloseConnection(DATABASESOURCE_TYPE_ACCESS_GLOBALDB) ;
	return TRUE ;
}