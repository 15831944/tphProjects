#include "StdAfx.h"
#include ".\globaldatabase.h"
#include "../Common/LogFile.h"
#include <Shlwapi.h>
#include "DatabaseFatalError.h"
#include "../Common/INIFileHandle.h"
#include "../Common/ReadRegister.h"
#include "../Database/ADODatabase.h"
CMasterDatabase::CMasterDatabase(const CString& strAppPath)
:m_strAppPath(strAppPath)
{
	m_bImportedProject = false;

}

CMasterDatabase::~CMasterDatabase(void)
{
	Close();

	//CoUninitialize();
}
void CMasterDatabase::Log(const CString& strProjName,const CString& strLogMessage)
{
	CString strProjPath;
	strProjPath.Format(_T("%s\\Project\\%s\\INPUT\\ProjectLog.log"),m_strAppPath,strProjName);

	CTime t = CTime::GetCurrentTime();
	CString strTime = t.Format("%Y-%m-%d,%H:%M:%S");

	char filename[_MAX_PATH];
	strcpy_s(filename,strProjPath);

	CLogFile file;
	file.openFile (filename, APPEND);

	//write the time
	file.writeField(strTime);
	//write the content
	file.writeField(strLogMessage);
	file.writeLine();
	file.closeOut();

}
//connect to master database
bool CMasterDatabase::Connect()
{
	//close connection
	Close();
	
	//connect
	try
	{
		m_pConnection.CreateInstance(__uuidof(ADODB::Connection));
	}
	catch (_com_error& e)
	{
		::MessageBox(NULL,  e.ErrorMessage(), "Error!", MB_OK);
		return false;
	}
	
	if( !ReadConnectInfoFromReg())
		return false;

	m_strDataBase = _T("master");

	char OpenFormatString[] = "Provider=SQLOLEDB;Data Source=%s;Initial Catalog=%s;User ID=%s;Password=%s";
	size_t nBufferLength = 20 + strlen(OpenFormatString)
		+ m_strServer.size()
		+ m_strDataBase.size()
		+ m_strUser.size()
		+ m_strPassWord.size();

	char* lpszConnectionString = new char[nBufferLength];

	sprintf_s(lpszConnectionString, nBufferLength, OpenFormatString,
		m_strServer.c_str(), m_strDataBase.c_str(), m_strUser.c_str(), m_strPassWord.c_str());

	try
	{
		if(m_pConnection)
			if (ADODB::adStateOpen == m_pConnection->State)
				m_pConnection->Close();

		m_pConnection->ConnectionTimeout = 100;
		m_pConnection->Open(_bstr_t(lpszConnectionString), _bstr_t(""), _bstr_t(""), ADODB::adModeUnknown);
	}
	catch (_com_error& connError)
	{	
		DATABASECONNECTION.writeLog(CString( lpszConnectionString) + (char *)connError.Description());

		try
		{
			//need to check the arcport is attached or not
			HandleConnectError();

			try
			{
				if(m_pConnection)
					if (ADODB::adStateOpen == m_pConnection->State)
						m_pConnection->Close();

				m_pConnection->ConnectionTimeout = 100;
				m_pConnection->Open(_bstr_t(lpszConnectionString), _bstr_t(""), _bstr_t(""), ADODB::adModeUnknown);

			}
			catch(_com_error& e)
			{
				CString strError = e.Description();
	//			MessageBox(NULL,_T("Cann't connect to database: ARCPORT. Error Message: ") + strError,_T("Warning"),MB_OK);
				DATABASECONNECTION.writeLog(_T("Connect to arcport error") + strError);
				throw CDatabaseFatalError(_T("Cann't connect to database: ARCPORT. Error Message: ") + strError);
				delete[] lpszConnectionString;
				return false;
			}
		}
		catch (CDatabaseFatalError& e)
		{
			CString stErroMsg =  e.GetErrorMessage();
			delete[] lpszConnectionString;
			return false;
		}
	



	}

	delete[] lpszConnectionString;
	return true;

}
void CMasterDatabase::SetImportedProjectName(const CString& strName,const CString& strDBName)
{
	m_strImportedProjName = strName;
	m_strImportedProjDatabase = strDBName;
	m_bImportedProject = true;
}
bool CMasterDatabase::IsOpenImportedProject()
{
	return m_bImportedProject;
}
void CMasterDatabase::CancelImportedProjectFlag()
{
	m_bImportedProject = false;
}
CString CMasterDatabase::GetImportedProjectName()
{
	return m_strImportedProjName;
}
CString CMasterDatabase::GetImportedProjectDatabase()
{
	return m_strImportedProjDatabase;
}

void CMasterDatabase::Close()
{
	if (m_pConnection)
	{
		if (m_pConnection->State == ADODB::adStateOpen )
			m_pConnection->Close();
		m_pConnection.Release();
		m_pConnection= NULL;
	}

}
void CMasterDatabase::Execute(const CString& strSQL)
{
	try
	{
		if (m_pConnection.GetInterfacePtr() == NULL)
		{
			CString strError = "Haven't connected to the database.";
			Connect();

			//ASSERT(0);
			TRACE(_T("Exception - void CMasterDatabase::Execute(const CString& strSQL)"));
		}
		_variant_t varCount;
		m_pConnection->Execute((_bstr_t)strSQL,&varCount,ADODB::adOptionUnspecified );
	}
	catch (_com_error &e)
	{
		CString strError = (char*)_bstr_t(e.Description());
		
	//	ASSERT(0);
		//throw CADOException(strError);
	}
}
//read connect string from regedit
bool CMasterDatabase::ReadConnectInfoFromReg()
{

	//TCHAR RegSubKey[] = _T("SOFTWARE\\Aviation Research Corporation\\ARCTerm\\DataBase");
	//TCHAR RegServerName[] = _T("Server");
	//TCHAR RegDataBaseName[] = _T("DataBase");
	//TCHAR RegUserName[] = _T("UserName");
	//TCHAR RegPassWord[] = _T("Password");

	//HKEY RegKey;
	//std::string strTemp;

	//if (RegOpenKey(HKEY_LOCAL_MACHINE, RegSubKey, &RegKey) != ERROR_SUCCESS)
	//	return false;

	//// server name
	//if (!ReadDBItem(RegKey, RegServerName, strTemp))
	//	return false;
	//m_strServer = strTemp;

	//// database name
	//if (!ReadDBItem(RegKey, RegDataBaseName, strTemp))
	//	return false;
	//m_strDataBase = strTemp;

	//// user name
	//if (!ReadDBItem(RegKey, RegUserName, strTemp))
	//	return false;
	//m_strUser = strTemp;

	//// password
	//if (!ReadDBItem(RegKey, RegPassWord, strTemp))
	//	return false;
	//m_strPassWord = strTemp;


	//RegCloseKey(RegKey);

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



	return true;

}
bool CMasterDatabase::ReadDBItem(HKEY hKey, LPCSTR lpEntry, std::string& strResult)
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
bool CMasterDatabase::IsDatabaseAttached(const CString& strDatabaseName)
{
	CString strSQL;
	strSQL.Format(_T("select name from sys.sysdatabases where name =N'%s'"),strDatabaseName);

	_variant_t varCount;
	ADODB::_RecordsetPtr pRecordset= m_pConnection->Execute((_bstr_t)strSQL,&varCount,ADODB::adOptionUnspecified );

	if(pRecordset->GetadoEOF() == VARIANT_FALSE)
	{
		return true;
	}

	return false;
}
bool CMasterDatabase::GetProjectDatabaseNameFromDB(const CString& strProjName,CString& strDBName)
{
	CString strLogMessage = _T("");
	strLogMessage = _T("Get project database name from master database...");
	Log(strProjName,strLogMessage);


	bool bRet = false;
	CString strSQL;
	strSQL.Format(_T("SELECT DBNAME FROM PROJECTINFO WHERE NAME = N'%s'"),strProjName);
	if (m_pConnection.GetInterfacePtr() == NULL)
	{	
		CString strError = "Haven't connected to the database.";
		Log(strProjName,strError);
		Connect();
	}
	if (m_pConnection.GetInterfacePtr() == NULL)
	{
		CString strError = "Haven't connected to the database.";
		Log(strProjName,strError);
		
	}
	else
	{
		ADODB::_RecordsetPtr pRecordset= NULL;
		try
		{	
			_variant_t varCount;
			pRecordset= m_pConnection->Execute((_bstr_t)strSQL,&varCount,ADODB::adOptionUnspecified );

			if(pRecordset->GetadoEOF() == VARIANT_FALSE)
			{
				_variant_t varValue = pRecordset->GetCollect("DBNAME");
				if(varValue.vt != VT_NULL)
				{
					strDBName = CString(varValue.bstrVal);
					bRet = true;
				}
			}
		}
		catch (_com_error& e)
		{
			strLogMessage = _T("Read database failed: ");
			strLogMessage += strSQL;
			strLogMessage += _T("       Error Message: ");
			 strLogMessage += CString((char *)e.Description());
			Log(strProjName,strLogMessage);			
		}
		
		if (pRecordset)
			if(pRecordset->State == ADODB::adStateOpen)
				pRecordset->Close();
	}
	if (bRet)
	{
		strLogMessage = _T("Project databse name :");
		strLogMessage += strDBName;
	}
	else
	{
		strLogMessage = _T("not find");
	}

	Log(strProjName,strLogMessage);

	return bRet;
}
bool CMasterDatabase::GetProjectDatabaseName(const CString& strProjName,CString& strDBName)
{
	CString strLogMessage = _T("");
	strLogMessage = _T("Get Project Database Name ...");
	Log(strProjName,strLogMessage);

	if (GetProjectDatabaseNameFromDB(strProjName,strDBName))
	{
		return true;
	}
	else
	{

		//add new item
		
		CTime currentTime = CTime::GetCurrentTime();

		CString strDatabaseName = _T("");
		strDatabaseName.Format(_T("%d%d%d%d%d%d"),
			currentTime.GetYear(),currentTime.GetMonth(),currentTime.GetDay(),
			currentTime.GetHour(),currentTime.GetMinute(),currentTime.GetSecond());


		try
		{	
			while (IsDatabaseNameExist(strDatabaseName))
			{
				Sleep(100);

				currentTime = CTime::GetCurrentTime();

				strDatabaseName.Empty();

				strDatabaseName.Format(_T("%d%d%d%d%d%d"),
					currentTime.GetYear(),currentTime.GetMonth(),currentTime.GetDay(),
					currentTime.GetHour(),currentTime.GetMinute(),currentTime.GetSecond());
			}

			CString strInsertSQL = _T("");
			strInsertSQL.Format(_T("INSERT INTO PROJECTINFO(NAME,DBNAME) VALUES(N'%s', N'%s')"),strProjName,strDatabaseName);

			Execute(strInsertSQL);
		}
		catch (_com_error& e)
		{
			strLogMessage = _T("Create new database error, Error Message: ");
			strLogMessage += CString((char *)e.Description());
			
			Log(strProjName,strLogMessage);
			
			return false;
		}

		strDBName = strDatabaseName;
		strLogMessage = _T("Create new database : ");
		strLogMessage += strDatabaseName;
		Log(strProjName,strLogMessage);
	}
	return true;
}
bool CMasterDatabase::InsertProjectDatabaseToDB(const CString& strProjName,const CString& strDBName)
{
	CString strInsertSQL = _T("");
	strInsertSQL.Format(_T("INSERT INTO PROJECTINFO(NAME,DBNAME) VALUES(N'%s', N'%s')"),strProjName,strDBName);
	Execute(strInsertSQL);

	return true;
}
bool CMasterDatabase::CreateNewprojectDatabaseName(const CString& strProjName,CString& strDBName)
{
	return true;
}
bool CMasterDatabase::IsDatabaseNameExist(const CString& strDBName)
{
	CString strSQL;
	strSQL.Format(_T("SELECT DBNAME FROM PROJECTINFO WHERE DBNAME = N'%s'"),strDBName);

	_variant_t varCount;
	ADODB::_RecordsetPtr pRecordset= NULL;
	try
	{
		pRecordset= m_pConnection->Execute((_bstr_t)strSQL,&varCount,ADODB::adOptionUnspecified );

		if(pRecordset->GetadoEOF() == VARIANT_FALSE)
		{
			if (pRecordset)
				if(pRecordset->State == ADODB::adStateOpen)
					pRecordset->Close();
			return true;
		}
	}
	catch (_com_error& e)
	{	
		if (pRecordset)
			if(pRecordset->State == ADODB::adStateOpen)
				pRecordset->Close();
		throw _com_error(e);
	}

	if (pRecordset)
		if(pRecordset->State == ADODB::adStateOpen)
			pRecordset->Close();

	return false;
}

bool CMasterDatabase::IsProjectDataNameExist(const CString& strDBName)
{

	CString strSQL;
	strSQL.Format(_T("SELECT ID FROM PROJECT WHERE NAME = N'%s'"),strDBName);

	_variant_t varCount;
	ADODB::_RecordsetPtr pRecordset= NULL;
	try
	{
		pRecordset= m_pConnection->Execute((_bstr_t)strSQL,&varCount,ADODB::adOptionUnspecified );

		if(pRecordset->GetadoEOF() == VARIANT_FALSE)
		{
			if (pRecordset)
				if(pRecordset->State == ADODB::adStateOpen)
					pRecordset->Close();
			return true;
		}
	}
	catch (_com_error& e)
	{	
		if (pRecordset)
			if(pRecordset->State == ADODB::adStateOpen)
				pRecordset->Close();
		throw _com_error(e);
	}

	if (pRecordset)
		if(pRecordset->State == ADODB::adStateOpen)
			pRecordset->Close();

	return false;
}
void CMasterDatabase::AttachDatabase()
{

}
void CMasterDatabase::DetachDatabase()
{

}
bool CMasterDatabase::OpenOldProject(const CString& strProjName,const CString& strAppPath)
{
	CString strLogMessage = _T("");
	strLogMessage = _T("Convert old Project ...");
	Log(strProjName,strLogMessage);
	
	CString strDBName = _T("");
	//get database name
	GetProjectDatabaseName(strProjName,strDBName);

	//close arcport database
	Close();
	CString strError= _T("");

	//connect to master database
	ADODB::_ConnectionPtr pMasterConnection;
	
	{
		strLogMessage = _T("Connect to master database ....");
		Log(strProjName,strLogMessage);

		try
		{
			try
			{
				pMasterConnection.CreateInstance(__uuidof(ADODB::Connection));
			}
			catch (_com_error& e)
			{
				strError = _T("Create ADODB::Connection Error.");
				throw _com_error(e);
			}

			if( !ReadConnectInfoFromReg())
				return false;

			m_strDataBase= _T("Master"); //connect to master database

			char OpenFormatString[] = "Provider=SQLOLEDB;Data Source=%s;Initial Catalog=%s;User ID=%s;Password=%s";
			size_t nBufferLength = 20 + strlen(OpenFormatString)
				+ m_strServer.size()
				+ m_strDataBase.size()
				+ m_strUser.size()
				+ m_strPassWord.size();

			char* lpszConnectionString = new char[nBufferLength];

			sprintf_s(lpszConnectionString,nBufferLength ,OpenFormatString,
				m_strServer.c_str(), m_strDataBase.c_str(), m_strUser.c_str(), m_strPassWord.c_str());

			try
			{
				if(pMasterConnection)
					if (ADODB::adStateOpen == pMasterConnection->State)
						pMasterConnection->Close();

				pMasterConnection->Open(_bstr_t(lpszConnectionString), _bstr_t(""), _bstr_t(""), ADODB::adModeUnknown);
			}
			catch (_com_error& e)
			{	
				
				strError = "Cann't connect to Master Database: ";
				strError += CString((char *)e.Description());
				strError += CString(lpszConnectionString);

				delete[] lpszConnectionString;
				throw _com_error(e);
			}

			delete[] lpszConnectionString;
		}
		catch(_com_error& )
		{
			strLogMessage = strError;
			Log(strProjName,strLogMessage);
			return false;

		}
	}
	//detach arcport database
	{
		strLogMessage = _T("kill all arcport connection and detach ARCPORT database....");
		Log(strProjName,strLogMessage);

		//kill all arcport connection
		CString strSQL = _T("Declare @sql   nvarchar( 500 ),\
			@temp   varchar(1000),\
			@spid    int\
			Declare getspid Cursor For Select    spid     From    sys.sysprocesses     \
			Where     status   =   N'sleeping' And spid >= 0 and spid <=32767 AND dbid In \
			( Select dbid From sys.sysdatabases Where Name= N'ARCPORT')\
			Open getspid\
			Fetch Next From getspid into @spid\
			While @@fetch_status = 0\
			Begin\
			If( @spid > 50 )\
			Begin\
			Set @temp = N'kill ' + rtrim( @spid )\
			Exec( @temp )\
			End\
			Fetch Next From getspid Into @spid\
			End\
			Close getspid\
			Deallocate getspid");

		try
		{
			_variant_t varCount;
			//kill all arcport connection

			pMasterConnection->Execute((_bstr_t)strSQL,&varCount,ADODB::adOptionUnspecified );
			//detach arcport

			strSQL.Empty();
			strSQL.Format(_T("sp_detach_db %s"),_T("ARCPort"));

			pMasterConnection->Execute((_bstr_t)strSQL,&varCount,ADODB::adOptionUnspecified );
		}
		catch (_com_error &e)
		{
			strError += strSQL;

			strError = (char*)_bstr_t(e.Description());
			
			strLogMessage = _T("Error :");
			strLogMessage += strError;

			Log(strProjName,strLogMessage);

			//attach master database
			return false;
		}


	}
	//copy arc port files
	
	CString strMasterDBFPath = _T("");
	CString strMasterLDFPath = _T("");
	{	
		strLogMessage = _T("copy arc port files ...");
		Log(strProjName,strLogMessage);

		//arc port path
		strMasterDBFPath.Format(_T("%s\\arctermdb\\ARCPort_Data.MDF"),strAppPath);
		strMasterLDFPath.Format(_T("%s\\arctermdb\\ARCPort_log.LDF"),strAppPath);

		CString strProjDBFPath = _T("");
		CString strProjLDFPath = _T("");
		strProjDBFPath.Format(_T("%s\\Project\\%s\\INPUT\\%s.MDF"),strAppPath,strProjName,strDBName);
		strProjLDFPath.Format(_T("%s\\Project\\%s\\INPUT\\%s.LDF"),strAppPath,strProjName,strDBName);
		//
		CopyFile(strMasterDBFPath,strProjDBFPath,FALSE);
		CopyFile(strMasterLDFPath,strProjLDFPath,FALSE);
	
		strLogMessage = strMasterDBFPath + _T("---------->") + strProjDBFPath;
		Log(strProjName,strLogMessage);
		strLogMessage = strMasterLDFPath + _T("---------->") + strProjLDFPath;
		Log(strProjName,strLogMessage);
	}
	//attach arc port database
	{	
		CString strSQL;
		strSQL.Format(_T("sp_attach_db @dbname =N'ARCPORT',@filename1=N'%s',@filename2=N'%s'"),strMasterDBFPath,strMasterLDFPath);
		try
		{
			_variant_t varCount;
			pMasterConnection->Execute((_bstr_t)strSQL,&varCount,ADODB::adOptionUnspecified );
		}
		catch(_com_error& e)
		{
			strError += strSQL;

			strError = (char*)_bstr_t(e.Description());

			strLogMessage = _T("Error :");
			strLogMessage += strError;

			Log(strProjName,strLogMessage);
		}



	}
	//open arc port database
	Connect();
	//open project

	return true;
}
bool CMasterDatabase::IsOldProject(const CString& strProjName)
{	
	CString strDBName = _T("");
	bool bExistProjectDatabase = GetProjectDatabaseNameFromDB(strProjName,strDBName);
	if (bExistProjectDatabase)
	{
		return false;
	}

	bool bRet = false;

	CString strSQL;
	strSQL.Format(_T("SELECT ID FROM PROJECT WHERE NAME = N'%s'"),strProjName);

	if (m_pConnection.GetInterfacePtr() == NULL)
	{
		CString strError = "Haven't connected to the database.";
		ASSERT(0);
	}
	else
	{
		_variant_t varCount;
		ADODB::_RecordsetPtr pRecordset= m_pConnection->Execute((_bstr_t)strSQL,&varCount,ADODB::adOptionUnspecified );

		if(pRecordset->GetadoEOF() == VARIANT_FALSE)
		{
			bRet =  true;
		}

		if (pRecordset)
			if(pRecordset->State == ADODB::adStateOpen)
				pRecordset->Close();
	}

	if (bRet == true)//old project
	{
		return true;
	}
	

	return false;
}
bool CMasterDatabase::IsProjectExists(const CString& strProjName)
{
	bool bRet = false;
	CString strSQL;
	strSQL.Format(_T("SELECT DBNAME FROM PROJECTINFO WHERE NAME = N'%s'"),strProjName);
	if (m_pConnection == NULL || m_pConnection.GetInterfacePtr() == NULL)
	{
		Connect();
	}
	
	if (m_pConnection.GetInterfacePtr() == NULL)
	{
		
		CString strError = "Haven't connected to the database.";
		ASSERT(0);
	}
	else
	{
		ADODB::_RecordsetPtr pRecordset= NULL;
		try
		{	
			_variant_t varCount;
			pRecordset = m_pConnection->Execute((_bstr_t)strSQL,&varCount,ADODB::adOptionUnspecified );

			if(pRecordset->GetadoEOF() == VARIANT_FALSE)
			{
				bRet =  true;
			}
		}
		catch (_com_error& e)
		{
			CString strError = (char *)e.Description();
			ASSERT(0);
		}
	
		if (pRecordset)
			if(pRecordset->State == ADODB::adStateOpen)
				pRecordset->Close();
	}

	return bRet;
	
}
bool CMasterDatabase::DeleteProjectDatabaseFromDB(const CString& strProjName)
{
	try
	{
		CString strSQL;
		strSQL.Format(_T("DELETE FROM PROJECTINFO WHERE NAME = N'%s'"),strProjName);
		Execute(strSQL);
	}
	catch (_com_error &e)
	{
		CString strError = (char*)_bstr_t(e.Description());
		ASSERT(0);
		throw _com_error(e);
	}

	return true;
}
bool CMasterDatabase::DeleteOldProject(const CString& strProjName,const CString& strAppPath)
{
	CString strError = _T("");
	//get database name
	CString strDBName = _T("");
	bool bExistProjectDatabase = GetProjectDatabaseNameFromDB(strProjName,strDBName);
	if (bExistProjectDatabase == false)
	{
		return false;//not find old database files
	}
	//Delete database entry item
	DeleteProjectDatabaseFromDB(strProjName);
	
	//detach database
	if (IsDatabaseAttached(strDBName))
	{
		//kill all arcport connection
	
		try
		{
			// status   =   N'sleeping' And
			
			CString strSQLFormat = _T("Declare @sql   nvarchar( 500 ),\
									  @temp   varchar(1000),\
									  @spid    int\
									  Declare getspid Cursor For Select    spid     From    sys.sysprocesses     \
									  Where     spid > 50 and spid <=32767 AND dbid In \
									  ( Select dbid From sys.sysdatabases Where Name= N'%s')\
									  Open getspid\
									  Fetch Next From getspid into @spid\
									  While @@fetch_status = 0\
									  Begin\
									  If( @spid > 50 )\
									  Begin\
									  Set @temp = N'kill ' + rtrim( @spid )\
									  Exec( @temp )\
									  End\
									  Fetch Next From getspid Into @spid\
									  End\
									  Close getspid\
									  Deallocate getspid");

		CString strSQL;
		strSQL.Format(strSQLFormat,strDBName);
			//kill all conn
			Execute(strSQL);
			
			//detach db

			strSQL.Empty();
			strSQL.Format(_T("sp_detach_db %s"),_T("ARCPort"));
			Execute(strSQL);
		}
		catch (_com_error &e)
		{
			strError = (char*)_bstr_t(e.Description());
			ASSERT(0);
			throw _com_error(e);
		}

	}

	//delete old files
	CString strProjDBFPath = _T("");
	CString strProjLDFPath = _T("");
	strProjDBFPath.Format(_T("%s\\Project\\%s\\INPUT\\%s.MDF"),strAppPath,strProjName,strDBName);
	strProjLDFPath.Format(_T("%s\\Project\\%s\\INPUT\\%s.LDF"),strAppPath,strProjName,strDBName);
	
	DeleteFile(strProjLDFPath);
	DeleteFile(strProjDBFPath);

	DeleteProjectDatabaseFromDB(strProjName);
	
	return true;
}
int CMasterDatabase::GetDatabaseVersion()
{
	int nVersion = -1;

	bool bRet = false;
	CString strSQL = _T("SELECT Version FROM db_version WHERE NAME = 'ARCTerm'");
	
	if (m_pConnection.GetInterfacePtr() == NULL)
	{
		Connect();
	}
	if (m_pConnection.GetInterfacePtr() == NULL)
	{
		CString strError = "Haven't connected to the database.";
	}
	else
	{
		ADODB::_RecordsetPtr pRecordset= NULL;
		try
		{	
			_variant_t varCount;
			pRecordset= m_pConnection->Execute((_bstr_t)strSQL,&varCount,ADODB::adOptionUnspecified );

			if(pRecordset->GetadoEOF() == VARIANT_FALSE)
			{
				_variant_t varValue = pRecordset->GetCollect("Version");
				if(varValue.vt != VT_NULL)
				{
					 nVersion = varValue.intVal;
				}
			}
		}
		catch (_com_error& e)
		{
			CString strLogMessage;
			strLogMessage = CString((char *)e.Description());
					
		}

		if (pRecordset)
			if(pRecordset->State == ADODB::adStateOpen)
				pRecordset->Close();
	}

	return nVersion;
}

bool CMasterDatabase::HandleConnectError()
{
	CString strMdfPath = _T("");
	CString strLdfPath = _T("");

	strMdfPath.Format(_T("%s\\arctermdb\\ARCPort_Data.MDF"),m_strAppPath);
	strLdfPath.Format(_T("%s\\arctermdb\\ARCPort_log.LDF"),m_strAppPath);

	if (PathFileExists(strMdfPath) == FALSE || PathFileExists(strLdfPath) == FALSE)
	{	
		
		DATABASECONNECTION.writeLog(_T("ARCport files is not exist"));
		//fatel error
		throw CDatabaseFatalError("ARCport Database file are not exists");

		return false;
	}

	//check the database is attached or not
	
	CheckMasterDatabaseAttached(strMdfPath,strLdfPath);


	return true;
}

bool CMasterDatabase::CheckMasterDatabaseAttached(CString strMdfPath,CString strLdfPath)
{
	bool bAttached = true;
	//connect to master database
	CString strConnection;
	strConnection.Format("Provider=SQLOLEDB;Data Source=%s;Initial Catalog=master;User ID=%s;Password=%s",
		m_strServer.c_str(), m_strUser.c_str(), m_strPassWord.c_str());
	
	ADODB::_ConnectionPtr ptrConnection;
	ADODB::_RecordsetPtr pRes;
	try
	{
		if (FAILED(ptrConnection.CreateInstance(__uuidof(ADODB::Connection))))
		throw std::runtime_error("Failed to create COM instance of ADODB::Connection!");

		try
		{	
			ptrConnection->Open(_bstr_t(strConnection), _bstr_t(""), _bstr_t(""), ADODB::adModeUnknown);
		}
		catch (_com_error& e)
		{
			throw CDatabaseFatalError( CString(_T("Cann't connect to master database..")) + (char *)(_bstr_t)e.ErrorMessage());
			
			return false;

		}
		//check the arcport database is exist or not

		//CString strSQL =_T("");
		//strSQL.Format(_T("Select dbid From sys.sysdatabases Where Name= N'%s'"),m_strDataBase.c_str());

		//_variant_t varCount;
		//ADODB::_RecordsetPtr pRes = ptrConnection->Execute(_bstr_t(strSQL), &varCount, ADODB::adOptionUnspecified);

		//if (pRes->GetadoEOF() == VARIANT_TRUE)//have not database
		bAttached = false;


		//if (pRes)
		//	if (pRes->State == ADODB::adStateOpen )
		//		pRes->Close();

	}
	catch (_com_error& e)
	{

		if (pRes)
			if (pRes->State == ADODB::adStateOpen )
				pRes->Close();

		if (ptrConnection)
			if (ptrConnection->State == ADODB::adStateOpen )
				ptrConnection->Close();	
	
		throw CDatabaseFatalError( CString(_T("Please check SQL Server is running or not.")) + (char *)(_bstr_t)e.ErrorMessage());
 
	}
	if (bAttached == false)
	{
		try
		{
			CString strSQL = _T("");
			strSQL.Format(_T("sp_detach_db %s"),m_strDataBase.c_str());

			_variant_t varCount;
			ptrConnection->Execute(_bstr_t(strSQL), &varCount, ADODB::adOptionUnspecified);

		}
		catch (_com_error& e)
		{
			e.ErrorMessage();
		}

		CString strSQL = _T("");
		strSQL.Format(_T("sp_attach_db @dbname ='%s',@filename1='%s',@filename2='%s'"),m_strDataBase.c_str(),strMdfPath,strLdfPath);
		
		
		try
		{		
			_variant_t varCount;
			ptrConnection->Execute(_bstr_t(strSQL), &varCount, ADODB::adOptionUnspecified);
			
		}
		catch (_com_error& e)
		{
		
			throw CDatabaseFatalError(CString(_T("Cann't attach ARCPORT database: ")) + (char *)(_bstr_t)e.ErrorMessage());
			
			return false;
		}
	}


	return bAttached;
}





