#include "StdAfx.h"
#include ".\projectexport.h"
#include "TermPlanDoc.h"
#include "MainFrm.h"
//#include "../Common/InfoZip.h"
#include "../Common/ZipInfo.h"
#include "TermPlan.h"
#include "../AirsideReport/ReportManager.h"
#include "../InputAirside/ProjectDatabase.h"
#include "ImportExportManager.h"
#include "../Common/AirportDatabaseList.h"
#include "../Common/AirportDatabase.h"
#include "DlgAirportDBNameInput.h"
#include "./InputAirside/GlobalDatabase.h"
#include "../Database/ARCportADODatabase.h"
#include "mapi.h"

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

CProjectExport::CProjectExport( CMainFrame *pMainFrame,CTermPlanDoc *pTermplanDoc )
{
	m_pMainFrame = pMainFrame;
	m_pTermPlanDoc = pTermplanDoc;

}
CProjectExport::~CProjectExport(void)
{
}

void CProjectExport::ExportAsInputZip( const CString& strProjName, const CString& strProjPath,bool bNotForceDB,bool bOpenProject  /*= true*/ )
{
 	SYSTEMTIME systime ;
 	CString str_Time ;
 	GetSystemTime(&systime) ;
 	COleDateTime datetime = systime;//(systime.wYear,systime.wMonth,systime.wDay,systime.wHour,systime.wMinute,systime.wSecond) ;
	if(datetime.GetStatus() == COleDateTime::valid)
        str_Time =  datetime.Format((_T("%B-%d-%Y %H %M %S"))) + _T(" UTC");
	else
	{	
		
		str_Time.Format(_T("%4d-%02d-%02d %d %d %d UTC"),systime.wYear,systime.wMonth,systime.wDay,systime.wHour,systime.wMinute,systime.wSecond);

		//str_Time =  COleDateTime::GetCurrentTime().Format((_T("%B-%d-%Y %H %M %S"))) + _T(" Local Time");
	}

	CFileDialog dlgFile( FALSE,
		".zip",
		strProjName+ " " + str_Time +(bNotForceDB?"_project Not Force DB":"" ),
		OFN_OVERWRITEPROMPT|OFN_HIDEREADONLY,
		"zip Files (*.zip)|*.zip||",
		NULL,
		0,
		FALSE );
	
	// use following code instead.
	// if COleDateTime::GetCurrentTime(), COleDateTime::Format(...) works, it works.
	//CFileDialog dlgFile( FALSE, ".zip",
	//	strProjName
	//	+ " "
	//	+ COleDateTime::GetCurrentTime().Format((_T("%B-%d-%Y %H %M %S")))
	//	+(bNotForceDB?"_project Not Force DB":"" ),
	//	OFN_OVERWRITEPROMPT|OFN_HIDEREADONLY,"zip Files (*.zip)|*.zip||"
	//	);

	
	CString sZipFileName;
    
	if (dlgFile.DoModal() == IDOK)
	{
		sZipFileName = dlgFile.GetPathName();
	}
	else
	{
		return;
	}
	(_T(""));
	DeleteFile( sZipFileName );
	(_T(""));
	BeginWaitCursor();

	CString _strTempInputZipFile;
	//in the process the database connection will be closed
	if(!ZipInputFiles(strProjName,strProjPath,_strTempInputZipFile))
	{
		//open database connection
		if(bOpenProject)
		{
			CMasterDatabase *pMasterDatabase = ((CTermPlanApp *)AfxGetApp())->GetMasterDatabase();
			CProjectDatabase projectDatabase(PROJMANAGER->GetAppPath());
			projectDatabase.ExportProjectEnd(pMasterDatabase,strProjName,strProjPath);
		}
		return;
	}

	//open database connection
	if(bOpenProject)
	{
		CMasterDatabase *pMasterDatabase = ((CTermPlanApp *)AfxGetApp())->GetMasterDatabase();
		CProjectDatabase projectDatabase(PROJMANAGER->GetAppPath());
		projectDatabase.ExportProjectEnd(pMasterDatabase,strProjName,strProjPath);
	}

	CString _strTempDBZipFile;
	// zip project db
	//if( !zipProjectDB( strProjName, _strTempDBZipFile ) )
	//{
	//	return;
	//}

	CString strEconomic = strProjPath + "\\ECONOMIC";
	CString strTempEconomicZipFile;
	if (!ZipEconomicFiles(strProjName,strEconomic,strTempEconomicZipFile))
	{
		AfxMessageBox("Failed to export project! ECONOMIC");
		return;
	}

	//	zip Mathematic
	//	by Kevin
	CString _strFolder = strProjPath + "\\Mathematic";
	CString _strTempMathematicZipFile;
	if (!ZipMathmaticFiles(strProjName, _strFolder, _strTempMathematicZipFile))
	{
		AfxMessageBox("Failed to export project! Mathematic");
		return;
	}
	//	zip mathresult
	CString _strTempMathResultZipFile;
	_strFolder = strProjPath + "\\MathResult";
	if (!ZipMathResultFiles(strProjName, _strFolder, _strTempMathResultZipFile))
	{
		AfxMessageBox("Failed to export project! MathResult");
		return;
	}	
	// zip temp input zip and temp db zip to a zip file
	if( !zipTempZipToTermzip(strProjName,strProjPath, _strTempInputZipFile, _strTempDBZipFile, 
		_strTempMathematicZipFile, _strTempMathResultZipFile,strTempEconomicZipFile,
		sZipFileName,bNotForceDB ) )

	{
		return;
	}
	// delete temp file
	DeleteFile( _strTempInputZipFile );
//	DeleteFile( _strTempDBZipFile );
	if (!_strTempMathematicZipFile.IsEmpty())
		DeleteFile(_strTempMathematicZipFile);
	if (!_strTempMathResultZipFile)
		DeleteFile(_strTempMathResultZipFile);


	EndWaitCursor();
	AfxMessageBox("Export project successfully !");
}

BOOL CProjectExport::ExportAndEmail( const CString& strProjName,const CString& strProjPath,bool bNotForceDB,bool bOpenProject /*= true*/ )
{

 	SYSTEMTIME systime ;
 	CString str_Time ;
	GetSystemTime(&systime) ;

 	COleDateTime datetime = systime;
	//the time is valid
	if (datetime.GetStatus() == COleDateTime::valid)
	{
		str_Time =  datetime.Format((_T("%B-%d-%Y %H %M %S"))) + _T(" UTC");
	}
	else
	{
		str_Time.Format(_T("%4d-%02d-%02d %d %d %d UTC"),systime.wYear,systime.wMonth,systime.wDay,systime.wHour,systime.wMinute,systime.wSecond);
		//str_Time =  COleDateTime::GetCurrentTime().Format((_T("%B-%d-%Y %H %M %S"))) + _T(" Local Time");

	}

 	CString sZipFileName;
 	sZipFileName.Format("%s %s.zip",  strProjName+(bNotForceDB?" Not Force DB":""),str_Time);
 		
	DeleteFile( sZipFileName );
	BeginWaitCursor();

	CString _strTempInputZipFile;
	//in the process the database connection will be closed
	if(!ZipInputFiles(strProjName,strProjPath,_strTempInputZipFile))
	{
		//open database connection
		if(bOpenProject)
		{
			CMasterDatabase *pMasterDatabase = ((CTermPlanApp *)AfxGetApp())->GetMasterDatabase();
			CProjectDatabase projectDatabase(PROJMANAGER->GetAppPath());
			projectDatabase.ExportProjectEnd(pMasterDatabase,strProjName,strProjPath);
		}
		return FALSE;
	}

	//open database connection
	if(bOpenProject)
	{
		CMasterDatabase *pMasterDatabase = ((CTermPlanApp *)AfxGetApp())->GetMasterDatabase();
		CProjectDatabase projectDatabase(PROJMANAGER->GetAppPath());
		projectDatabase.ExportProjectEnd(pMasterDatabase,strProjName,strProjPath);
	}

	CString _strTempDBZipFile;
	// zip project db
	//if( !zipProjectDB( strProjName, _strTempDBZipFile ) )
	//{
	//	return FALSE;
	//}

	//	zip Mathematic
	//	by Kevin
	CString strEconomic = strProjPath + "\\ECONOMIC";
	CString strTempEconomicZipFile;
	if (!ZipEconomicFiles(strProjName,strEconomic,strTempEconomicZipFile))
	{
		AfxMessageBox("Failed to export project! ECONOMIC");
		return FALSE;
	}

	CString _strFolder = strProjPath + "\\Mathematic";
	CString _strTempMathematicZipFile;
	if (!ZipMathmaticFiles(strProjName, _strFolder, _strTempMathematicZipFile))
	{
		AfxMessageBox("Failed to export project! Mathematic");
		return FALSE;
	}
	//	zip mathresult
	CString _strTempMathResultZipFile;
	_strFolder = strProjPath + "\\MathResult";
	if (!ZipMathResultFiles(strProjName, _strFolder, _strTempMathResultZipFile))
	{
		AfxMessageBox("Failed to export project! MathResult");
		return FALSE;
	}	
	// zip temp input zip and temp db zip to a zip file
	if( !zipTempZipToTermzip(strProjName,strProjPath, _strTempInputZipFile, _strTempDBZipFile, 
		_strTempMathematicZipFile, _strTempMathResultZipFile,strTempEconomicZipFile,
		strProjPath + "\\" +sZipFileName,bNotForceDB ) )

	{
		return FALSE;
	}
	// delete temp file
	DeleteFile( _strTempInputZipFile );
	//DeleteFile( _strTempDBZipFile );
	if (!_strTempMathematicZipFile.IsEmpty())
		DeleteFile(_strTempMathematicZipFile);
	if (!_strTempMathResultZipFile)
		DeleteFile(_strTempMathResultZipFile);


	EndWaitCursor();



	//send mail

	HMODULE hMod = LoadLibrary("MAPI32.DLL");

	if (hMod == NULL)	
	{
		AfxMessageBox(AFX_IDP_FAILED_MAPI_LOAD);
		return FALSE;
	}

	ULONG (PASCAL *lpfnSendMail)(ULONG, ULONG, MapiMessage*, FLAGS, ULONG);
	(FARPROC&)lpfnSendMail = GetProcAddress(hMod, "MAPISendMail");

	if (lpfnSendMail == NULL)
	{
		AfxMessageBox(AFX_IDP_INVALID_MAPI_DLL);
		return FALSE;
	}



	int iFileCount = 1;// only one attachment
	MapiFileDesc* pFileDesc = (MapiFileDesc*)malloc(sizeof(MapiFileDesc) * iFileCount);
	memset(pFileDesc,0,sizeof(MapiFileDesc) * iFileCount);


	CString path ;
	path.Format(_T("%s\\%s"),strProjPath,sZipFileName);

	pFileDesc->lpszFileName = NULL;
	pFileDesc->lpszPathName = path.GetBuffer(path.GetLength());
	pFileDesc->nPosition = (ULONG)-1;


	MapiRecipDesc recip;
	memset(&recip,0,sizeof(MapiRecipDesc));
	recip.lpszAddress	= NULL;
	recip.ulRecipClass = MAPI_TO;


	MapiMessage message;
	memset(&message, 0, sizeof(message));
	message.nFileCount	= iFileCount;				
	message.lpFiles		= pFileDesc;				
	message.nRecipCount = 0;						
	message.lpRecips 	= &recip;					
	message.lpszSubject	= "";	
	message.lpszNoteText= "";	


	CWnd* pParentWnd = CWnd::GetSafeOwner(NULL, NULL);


	int nError = lpfnSendMail(0, 0,
		&message, MAPI_LOGON_UI|MAPI_DIALOG, 0);

	if (nError != SUCCESS_SUCCESS && nError != MAPI_USER_ABORT 
		&& nError != MAPI_E_LOGIN_FAILURE)
	{
		DWORD errorcode = GetLastError() ;
		AfxMessageBox(AFX_IDP_FAILED_MAPI_SEND);
	}


	pParentWnd->SetActiveWindow();


	free(pFileDesc);

	FreeLibrary(hMod);

	DeleteFile( strProjPath + "\\" +sZipFileName );
	return FALSE;
}

BOOL CProjectExport::zipFloder( const CString& strProjName, const CString& _strSrcFolder, CString& _strTempZipFile )
{
	std::vector<CString>	vSubFileNames;
	TCHAR oldDir[MAX_PATH+1];
	GetCurrentDirectory(MAX_PATH, oldDir);
	if (!SetCurrentDirectory(_strSrcFolder))	//	Folder is not exist
	{
		_strTempZipFile.Empty();
		return TRUE;
	}

	WIN32_FIND_DATA FindFileData;
	HANDLE hFind = FindFirstFile("*.*", &FindFileData);	// gets "."
	FindNextFile(hFind, &FindFileData);				// gets ".."
	while(GetLastError() != ERROR_NO_MORE_FILES) 
	{
		if(FindNextFile(hFind, &FindFileData))
		{
			if(!(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) 
			{
				vSubFileNames.push_back( _strSrcFolder + "\\" + FindFileData.cFileName );
			}
		}
		else
			break;				
	}
	FindClose(hFind);
	SetCurrentDirectory(oldDir);
	SetLastError(0);

	int iFileCount = vSubFileNames.size();
	if (iFileCount == 0)
	{
		_strTempZipFile.Empty();
		return TRUE;
	}

	char **pFiles = (char **) new  int [iFileCount];
	for (int i=0; i<iFileCount; i++)
	{
		pFiles[i] = new char[MAX_PATH+1];
		strcpy( pFiles[i], vSubFileNames[i] );
	}

// 	CInfoZip InfoZip;
// 	if (!InfoZip.InitializeZip())
// 	{
// 		EndWaitCursor();
// 		AfxMessageBox("Failure to export project!", MB_OK);
// 		return FALSE;
// 	}

	//InfoZip.
	int nPos = -1;
	if ((nPos = _strSrcFolder.ReverseFind('\\')) != -1)
	{
		_strTempZipFile = getTempPath() + strProjName+"_"+_strSrcFolder.Mid(nPos + 1)+".zip";
	}
	else
	{
		EndWaitCursor();
		AfxMessageBox("Failure to export project!", MB_OK);
		return FALSE;
	}

	//CString _strTempInputZipFile = getTempPath() + CString("\\") + pDoc->m_ProjInfo.name+"_input.zip";
	DeleteFile( _strTempZipFile );

	CZipInfo InfoZip;
	if (!InfoZip.AddFiles(_strTempZipFile, pFiles, iFileCount))
	{
		EndWaitCursor();
		AfxMessageBox("Failure to export project!", MB_OK);
		return FALSE;
	}


// 	if (!InfoZip.Finalize())
// 	{
// 		EndWaitCursor();
// 		AfxMessageBox("Failure to export project!", MB_OK);
// 		return FALSE;
// 	}

	return TRUE;
}

BOOL CProjectExport::zipFloder( const CString& strProjName, const CString& _strSrcFolder,std::vector<CString> vAddFile,CString& _strTempZipFile )
{
	std::vector<CString>	vSubFileNames;
	TCHAR oldDir[MAX_PATH+1];
	GetCurrentDirectory(MAX_PATH, oldDir);
	if (!SetCurrentDirectory(_strSrcFolder))	//	Folder is not exist
	{
		_strTempZipFile.Empty();
		return TRUE;
	}

	WIN32_FIND_DATA FindFileData;
	HANDLE hFind = FindFirstFile("*.*", &FindFileData);	// gets "."
	FindNextFile(hFind, &FindFileData);				// gets ".."
	while(GetLastError() != ERROR_NO_MORE_FILES) {
		if(FindNextFile(hFind, &FindFileData)) {
			if(!(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
				vSubFileNames.push_back( _strSrcFolder + "\\" + FindFileData.cFileName );
			}
		}
		else
			break;				
	}
	FindClose(hFind);
	SetCurrentDirectory(oldDir);
	SetLastError(0);

	//add others file
	std::vector<CString>::iterator it = vSubFileNames.end();
	vSubFileNames.insert(it,vAddFile.begin(),vAddFile.end());

	int iFileCount = vSubFileNames.size();
	if (iFileCount == 0)
	{
		_strTempZipFile.Empty();
		return TRUE;
	}

	char **pFiles = (char **) new  int [iFileCount];
	for (int i=0; i<iFileCount; i++)
	{
		pFiles[i] = new char[MAX_PATH+1];
		strcpy( pFiles[i], vSubFileNames[i] );
	}

// 	CInfoZip InfoZip;
// 	if (!InfoZip.InitializeZip())
// 	{
// 		EndWaitCursor();
// 		AfxMessageBox("Failure to export project!", MB_OK);
// 		return FALSE;
// 	}
	//InfoZip.
	int nPos = -1;
	if ((nPos = _strSrcFolder.ReverseFind('\\')) != -1)
	{
		_strTempZipFile = getTempPath() + strProjName+"_"+_strSrcFolder.Mid(nPos + 1)+".zip";
	}
	else
	{
		EndWaitCursor();
		AfxMessageBox("Failure to export project!", MB_OK);
		return FALSE;
	}

	//CString _strTempInputZipFile = getTempPath() + CString("\\") + pDoc->m_ProjInfo.name+"_input.zip";
	DeleteFile( _strTempZipFile );

	CZipInfo InfoZip;
	if (!InfoZip.AddFiles(_strTempZipFile, pFiles, iFileCount))
	{
		EndWaitCursor();
		AfxMessageBox("Failure to export project!", MB_OK);
		return FALSE;
	}


// 	if (!InfoZip.Finalize())
// 	{
// 		EndWaitCursor();
// 		AfxMessageBox("Failure to export project!", MB_OK);
// 		return FALSE;
// 	}

	return TRUE;
}
BOOL CProjectExport::ZipAirportInfo( const CString& strProjName,CString projPath,std::vector<CString> &vAirportName )
{
	//CMDIChildWnd* pMDIActive = MDIGetActive();
	//ASSERT(pMDIActive != NULL);
	//CTermPlanDoc* pDoc = (CTermPlanDoc*)pMDIActive->GetActiveDocument();
	//ASSERT(pDoc != NULL);
	//now treat only one airport  in the project 

	//make sure the airport is exist ,that is make sure the airport path is valid
	int nAirport = 1;
	CString strAirportPath = projPath + _T("\\Input\\Airport1");
	while (PathFileExists((LPCSTR)strAirportPath))
	{

		std::vector<CString> vAddFile;
		//zip the airside files	
		CString strAirsidePath = strAirportPath + "\\Airside";


		if (PathFileExists((LPCSTR)strAirsidePath))
		{
			CString tempAirsidePath;
			//if (!zipFloder(strProjName,strAirsidePath,tempAirsidePath))
			//	continue;
			if (ZipInputAirportAirsideFiles(strProjName,strAirsidePath,tempAirsidePath))
			{
				vAddFile.push_back(tempAirsidePath);
			}


		}

		//zip the landside files
		CString strLandsidePath = strAirportPath + "\\Landside";
		if(PathFileExists((LPCSTR)strLandsidePath))
		{
			CString tempLandsidePath;
			//if(!zipFloder(strProjName,strLandsidePath,tempLandsidePath))
			//	continue;		
			if(ZipInputAirportLandsideFiles(strProjName,strLandsidePath,tempLandsidePath))
			{
				vAddFile.push_back(tempLandsidePath);
			}

		}	

		//airport
		CString tempAirportPath;
		//if(!zipFloder(strProjName,strAirportPath,vAddFile,tempAirportPath))
		//	continue;
		if (ZipFiles(strProjName,vAddFile,strAirportPath,tempAirportPath) == FALSE)
			continue;


		vAirportName.push_back(tempAirportPath);

		//Next Airport
		nAirport += 1;
		strAirportPath.Empty();
		strAirportPath.Format(_T("\\Input\\Airport%d"),nAirport);
		strAirportPath = projPath + strAirportPath;
	}

	return TRUE;
}
BOOL CProjectExport::ZipImportDB(const CString& strProjName,CString projPath,std::vector<CString> &vAirportName  )
{
	int nAirport = 1;
	CString strAirportPath = projPath + _T("\\Input\\AirportDB");
	if (PathFileExists((LPCSTR)strAirportPath))
	{
        CFileFind filefind ;
		std::vector<CString> vAddFile;
        BOOL res = filefind.FindFile(strAirportPath + "\\*.*") ;
		while (res)
		{
			res = filefind.FindNextFile() ;
			if(filefind.IsDirectory())
			{
				CString tempAirsidePath;
				CString strAirsidePath = strAirportPath + "\\" + filefind.GetFileName();
				if(strcmp(".",filefind.GetFileName()) == 0 || strcmp("..",filefind.GetFileName()) == 0)
					continue;
				if (ZipImportDBDataFiles(strProjName,strAirsidePath,tempAirsidePath))
				{
					if(!tempAirsidePath.IsEmpty())
						vAddFile.push_back(tempAirsidePath);
				}
			}else
			{
    //            if(strcmp("DBMessage.txt",filefind.GetFileName()) == 0)
				//{
                   vAddFile.push_back(strAirportPath + "\\" + filefind.GetFileName()) ;
				//}
			}
		}
		filefind.Close() ;
		CString tempAirportPath;
		if (!ZipFiles(strProjName,vAddFile,strAirportPath,tempAirportPath))
			return FALSE;
		if(!tempAirportPath.IsEmpty())
		vAirportName.push_back(tempAirportPath);
		return true ;
	}else
		return TRUE ;
}

BOOL CProjectExport::ZipInputlandsideFiles(const CString& strProjName,const CString& projPath, CString& strZipFilePath)
{
	CString strDirPath = _T("");
	strDirPath.Format(_T("%s\\INPUT\\Landside"),projPath);
	if (!PathFileExists((LPCSTR)strDirPath))
	{
		if(!CreateDirectory(strDirPath,NULL))
			return FALSE;
	}

	CImportExportManager::ExportProject(projPath,strProjName);


	if (!PathFileExists((LPCSTR)strDirPath))
		return FALSE;


	//zip airside
	CString strZipFileName[] = 
	{
		_T("VEHICLE.DST")
	};

	int nFileCount = sizeof(strZipFileName)/sizeof(strZipFileName[0]);

	std::vector< CString> vZipFiles;

	for (int i = 0; i < nFileCount; ++i)
	{		
		CString strFilePath = strDirPath + "\\" + strZipFileName[i];
		if(PathFileExists(strFilePath))
			vZipFiles.push_back(strFilePath);

	}

	//zipFloder(strProjName,strDirPath,strZipFilePath);
	if (ZipFiles(strProjName,vZipFiles,strDirPath,strZipFilePath))
		return TRUE;

	return FALSE;
}

BOOL CProjectExport::ZipInputAirsideFiles( const CString& strProjName,const CString& projPath, CString& strZipFilePath )
{
	CString strDirPath = _T("");
	strDirPath.Format(_T("%s\\INPUT\\Airside"),projPath);
	if (!PathFileExists((LPCSTR)strDirPath))
	{
		if(!CreateDirectory(strDirPath,NULL))
			return FALSE;
	}

	CImportExportManager::ExportProject(projPath,strProjName);

	
	if (!PathFileExists((LPCSTR)strDirPath))
		return FALSE;
	

	//zip airside
	CString strZipFileName[] = 
	{
		_T("project.prj")
	};

	int nFileCount = sizeof(strZipFileName)/sizeof(strZipFileName[0]);

	std::vector< CString> vZipFiles;

	for (int i = 0; i < nFileCount; ++i)
	{		
		CString strFilePath = strDirPath + "\\" + strZipFileName[i];
		if(PathFileExists(strFilePath))
			vZipFiles.push_back(strFilePath);

	}

	//zipFloder(strProjName,strDirPath,strZipFilePath);
	if (ZipFiles(strProjName,vZipFiles,strDirPath,strZipFilePath))
		return TRUE;

	return FALSE;
}

bool CProjectExport::zipProjectDB( const CString& strProjName, CString& _strTempDBZipFile )
{
	//const CString strProjName;
	//const CString strProjPath;
	PROJECTINFO projInfo;
	PROJMANAGER->GetProjectInfo(strProjName,&projInfo);
	CAirportDatabase* pAirportDB = m_pTermPlanDoc->GetTerminal().m_pAirportDB;

	if (pAirportDB == NULL)
	{

		CString _strErr;
		_strErr.Format("%s project's global DB index: %d does not exist in the DB list.", projInfo.name, projInfo.lUniqueIndex );
		AfxMessageBox( _strErr, MB_OK|MB_ICONINFORMATION );
		return false;
	}


	//assert( _pTermDoc );
	//CAirportDatabase* pAirportDB = _pTermDoc->GetTerminal().m_pAirportDB;
	//assert( pAirportDB );

	std::vector< CString > _vAllDBFile;
	AIRPORTDBLIST->getAllDBFileName( pAirportDB, _vAllDBFile );
	CString strDefaultDBPath = PROJMANAGER->GetDefaultDBPath();
	for (int j = 0; j < (int)_vAllDBFile.size(); j++)
	{
		if (PathFileExists(_vAllDBFile[j]) == FALSE)
		{
			CopyFile(strDefaultDBPath+_T("\\")+__database_file[j],_vAllDBFile[j],TRUE);
		}
	}

	int _iDBFileCount = _vAllDBFile.size();		
	char **_pDBFiles = (char **) new  int [_iDBFileCount];
	for (int i=0; i<_iDBFileCount; i++)
	{
		_pDBFiles[i] = new char[MAX_PATH+1];
		strcpy( _pDBFiles[i], _vAllDBFile[i] );
	}

// 	CInfoZip InfoZip;
// 	if (!InfoZip.InitializeZip())
// 	{
// 		AfxMessageBox("Failure to export database!", MB_OK);
// 		clear2VArray( _pDBFiles, _iDBFileCount );
// 		return false;
// 	}
	
	//InfoZip.
	CString strTempPath = getTempPath();
	strTempPath.TrimRight("\\");
	_strTempDBZipFile = strTempPath + "\\" + pAirportDB->getName() +"_DB.zip";
	DeleteFile( _strTempDBZipFile );

	CZipInfo InfoZip;
	if (!InfoZip.AddFiles(_strTempDBZipFile, _pDBFiles, _iDBFileCount))
	{
		AfxMessageBox("Failure to export database!", MB_OK);
		clear2VArray( _pDBFiles, _iDBFileCount );
		return false;
	}

// 	if (!InfoZip.Finalize())
// 	{
// 		AfxMessageBox("Failure to export database!", MB_OK);
// 		clear2VArray( _pDBFiles, _iDBFileCount );
// 		return false;
// 	}

	clear2VArray( _pDBFiles, _iDBFileCount );
	return true;
}

bool CProjectExport::zipTempZipToTermzip( const CString& strProjName, const CString& strProjPath, const CString& _strTempInputZip, const CString& _strTempDBZip, const CString& _strTempMathematicZip, const CString& _strTempMathResultZip,const CString& _strTmpEconZip, const CString& _strTerminalZip,bool bNotForceDB,CTermPlanDoc* _pTermDoc/*=NULL */ )
{
	CString _strTempIniFile;
	createArcExportIniFile( _strTempInputZip, _strTempDBZip, 
		_strTempMathematicZip, _strTempMathResultZip, 
		_strTempIniFile,bNotForceDB);
	//   CTermPlanDoc* pDoc;
	//if(_pDoc==NULL)
	//{
	//	CMDIChildWnd* pMDIActive = MDIGetActive();
	//    ASSERT(pMDIActive != NULL);
	//    pDoc = (CTermPlanDoc*)pMDIActive->GetActiveDocument();
	//    ASSERT(pDoc != NULL);
	//}
	//else 
	//	pDoc = _pDoc;

	//CString sProjectPath = pDoc->m_ProjInfo.path;
	CString sProjectPath = strProjPath;

	/*
	CString sExportHisFileName=sProjectPath+"\\ExportHis.ini";

	CFile file;
	// Check if Exist
	CFileFind finder;
	if(!finder.FindFile(sExportHisFileName))
	{
	// Create ExportHis.ini If Not Exist----------------------------------------------------------------------
	CFile file(sExportHisFileName,CFile::modeCreate);
	file.Close();
	//--------------------------------------------------------------------------------------------------------

	}
	// Append Export History of Local Machine If Not Exist--------------------------------------------------------
	CIniReader iniEH;
	iniEH.setINIFileName(sExportHisFileName);
	CString strDBIndex;
	strDBIndex.Format("%d",pDoc->m_ProjInfo.lUniqueIndex);
	iniEH.setKey(strDBIndex,pDoc->m_ProjInfo.machine,"HistoryList");
	//------------------------------------------------------------------------------------------------------------
	*/
	//project db info file
	//this line should be changed into pDoc->getTerminal()->m_pAirportDB, after InputTerminal be corrected.

	PROJECTINFO projInfo;
	PROJMANAGER->GetProjectInfo(strProjName,&projInfo);
	//CAirportDatabase* pAirportDB = AIRPORTDBLIST->getAirportDBByID(projInfo.lUniqueIndex);
    CAirportDatabase* pDB = NULL ;
	CAirportDatabase* pDBPoint = NULL ;
	if(m_pTermPlanDoc != NULL)
		pDB=m_pTermPlanDoc->GetTerminal().m_pAirportDB;
	else
	{
		pDB = new CAirportDatabase() ;
		pDBPoint = pDB ;
		pDB->setName(projInfo.dbname) ;
	}
	ASSERT(pDB!=NULL);
	CString sFullProjInfoFile("");
	if(ExportProjDBInfo(strProjName,pDB,sProjectPath,sFullProjInfoFile)==false)
		return false;

	int _iDBFileCount = 3;
	if (!_strTempMathematicZip.IsEmpty())
		_iDBFileCount++;
	if (!_strTempMathResultZip.IsEmpty())
		_iDBFileCount++;
	if (!_strTmpEconZip.IsEmpty())
		_iDBFileCount++;
	char **_pDBFiles ;


	_pDBFiles = (char **) new  int [_iDBFileCount];
	for (int i=0; i<_iDBFileCount; i++)
	{
		_pDBFiles[i] = new char[MAX_PATH+1];
	}
	strcpy( _pDBFiles[0], _strTempInputZip );
	//strcpy( _pDBFiles[1], _strTempDBZip );
	strcpy( _pDBFiles[1], _strTempIniFile);
	//	strcpy( _pDBFiles[3], sExportHisFileName );
	strcpy( _pDBFiles[2], sFullProjInfoFile);//project info file

	int iIndex = 3;
	if (!_strTempMathematicZip.IsEmpty())
		strcpy( _pDBFiles[iIndex++], _strTempMathematicZip );
	if (!_strTempMathResultZip.IsEmpty())
		strcpy( _pDBFiles[iIndex++], _strTempMathResultZip );
	if (!_strTmpEconZip.IsEmpty())
		strcpy(_pDBFiles[iIndex],_strTmpEconZip);
	

// 	CInfoZip InfoZip;
// 	if (!InfoZip.InitializeZip())
// 	{
// 		AfxMessageBox("Failure to export project!", MB_OK);
// 		clear2VArray( _pDBFiles, _iDBFileCount );
// 		return false;
// 	}
	CZipInfo InfoZip;
	//InfoZip.
	if (!InfoZip.AddFiles(_strTerminalZip, _pDBFiles, _iDBFileCount))
	{
		AfxMessageBox("Failure to export project!", MB_OK);
		clear2VArray( _pDBFiles, _iDBFileCount );
		return false;
	}

// 	if (!InfoZip.Finalize())
// 	{
// 		AfxMessageBox("Failure to export project!", MB_OK);
// 		clear2VArray( _pDBFiles, _iDBFileCount );
// 		return false;
// 	}
    if(pDBPoint != NULL)
		delete pDBPoint ;
	clear2VArray( _pDBFiles, _iDBFileCount );
	// delete temp file
	DeleteFile( _strTempIniFile );
	return true;
}

void CProjectExport::createArcExportIniFile( const CString& _strTempInputZip, const CString& _strTempDBZip, const CString& _strTempMathematicZip, const CString& _strTempMathResultZip, CString& _strTmpIniFile ,bool bNotForceDB )
{
	int _iPos;

	CString strTempPath = getTempPath();
	strTempPath.TrimRight("\\");
	_strTmpIniFile = strTempPath + "\\" +"arcterm_export.ini";
	DeleteFile( _strTmpIniFile );

	ArctermFile inifile;
	inifile.openFile( _strTmpIniFile,WRITE );
	inifile.writeField("arcterm export ini file");
	inifile.writeLine();
	inifile.writeField("version\t inputzip\t databasezip");
	if (!_strTempMathematicZip.IsEmpty())
		inifile.writeField("\t mathematiczip");
	if (!_strTempMathResultZip.IsEmpty())
		inifile.writeField("\t mathresultzip");
	inifile.writeLine();

	inifile.writeInt(4);	// version
	inifile.writeLine();

	inifile.writeField("PROJECT VERSION") ;
	inifile.writeField("303") ;
	inifile.writeLine() ;

	inifile.writeField("INPUTZIP");
	_iPos = _strTempInputZip.ReverseFind('\\');
	inifile.writeField( _strTempInputZip.Mid( _iPos +1) );
	inifile.writeLine();

	inifile.writeField("DATABASEZIP");
	_iPos = _strTempDBZip.ReverseFind('\\');
	inifile.writeField( _strTempDBZip.Mid( _iPos +1 ) );
	inifile.writeLine();

	if (!_strTempMathematicZip.IsEmpty())
	{
		inifile.writeField("MATHEMATICZIP");
		_iPos = _strTempMathematicZip.ReverseFind('\\');
		inifile.writeField( _strTempMathematicZip.Mid( _iPos +1 ) );
		inifile.writeLine();
	}

	if (!_strTempMathResultZip.IsEmpty())
	{
		inifile.writeField("MATHRESULTZIP");
		_iPos = _strTempMathResultZip.ReverseFind('\\');
		inifile.writeField( _strTempMathResultZip.Mid( _iPos +1 ) );
		inifile.writeLine();
	}

	inifile.writeField("Not Force DB");
	inifile.writeField(bNotForceDB?"1":"0");
	inifile.writeLine();

	inifile.endFile();
}

void CProjectExport::clear2VArray( char** _p2VArray, int _xCount )
{
	for( int i=0; i<_xCount; i++ )
	{
		delete []_p2VArray[i];
	}

	delete []_p2VArray;
}

CString CProjectExport::getTempPath( const CString& _strDefault /*= "c:\\" */ )
{
	char _szTempPath[ MAX_PATH + 1];
	if( GetTempPath( MAX_PATH, _szTempPath ) == 0 )
		return  _strDefault;
	return CString( _szTempPath );
}

void CProjectExport::BeginWaitCursor()
{
	if (m_pMainFrame)
	{
		m_pMainFrame->BeginWaitCursor();
	}
}

void CProjectExport::EndWaitCursor()
{
	if (m_pMainFrame)
	{
		m_pMainFrame->EndWaitCursor();
	}

}

bool CProjectExport::ExportProjDBInfo(const CString& strProjName,CAirportDatabase *pDB, const CString &sDestPath, CString& sFullProjInfoFile)
{
	CString sInput=pDB->getName();
	TCHAR sProjInfoFile[MAX_PATH];
	sprintf(sProjInfoFile,"%s\\%s",sDestPath,"ProjectDB.txt");
	fsstream fproj(sProjInfoFile,stdios::out|stdios::trunc);
	fproj<<(LPCSTR)sInput<<endl;
	fproj.close();

	sFullProjInfoFile=sProjInfoFile;
	return true;
}
BOOL CProjectExport::GetFileNameUnderFloder(const CString& _path, std::vector<CString>& _Filenames)
{
	CFileFind _fileFinder ;
	BOOL res = _fileFinder.FindFile(_path+ "\\*.*");
	while(res)
	{
		res = _fileFinder.FindNextFile() ;
		if(strcmp(".",_fileFinder.GetFileName()) == 0 || strcmp("..",_fileFinder.GetFileName()) == 0 || _fileFinder.IsDirectory())
			continue ;
		_Filenames.push_back(_fileFinder.GetFileName()) ;
	}
	_fileFinder.Close() ;
     return TRUE ;
}
BOOL CProjectExport::ZipImportDBDataFiles(const CString& strProjName, const CString& _strSrcFolder,CString& _strTempZipFile)
{
	/*CString strZipFileName[] = 
	{
		_T("acdata.acd"),
			_T("Airlines.dat"),
			_T("Airports.dat"),
			_T("category.cat"),
			_T("FlightGroup.frp"),
			_T("probdist.pbd"),
			_T("Sector.sec"),
			_T("subairlines.sal"),
	};*/
	std::vector<CString> m_filenames ;
	if(!GetFileNameUnderFloder(_strSrcFolder,m_filenames))
		return FALSE ;
	int nFileCcunt = m_filenames.size();

	std::vector< CString> vZipFiles;

	for (int i = 0; i < nFileCcunt; ++i)
	{	
		CString strFilePath = _strSrcFolder + "\\" + m_filenames[i];
		if(PathFileExists(strFilePath))
			vZipFiles.push_back(strFilePath);
	}
    if(vZipFiles.size() == 0)
		return FALSE ;
	if(ZipFiles(strProjName,vZipFiles,_strSrcFolder,_strTempZipFile))
		return TRUE;

	_strTempZipFile.Empty();
	return FALSE;
}
BOOL CProjectExport::ZipInputAirportAirsideFiles(const CString& strProjName, const CString& _strSrcFolder,CString& _strTempZipFile)
{
	CString strZipFileName[] = 
	{
		_T("AirPortInfo.txt"),
		_T("AirsideStructure.txt"),
		_T("ContourTreeStruct.txt"),
		_T("Filelist.txt"),
		_T("Floors.txt"),
		_T("LAYOUT.LAY"),
		_T("Placements.txt"),
		_T("SimAndReport.sim"),
		_T("Views.txt")
	};

	int nFileCcunt = sizeof(strZipFileName)/sizeof(strZipFileName[0]);
	
	std::vector< CString> vZipFiles;

	for (int i = 0; i < nFileCcunt; ++i)
	{	
		CString strFilePath = _strSrcFolder + "\\" + strZipFileName[i];
		if(PathFileExists(strFilePath))
			vZipFiles.push_back(strFilePath);
	}

	//VRInfo0.txt maybe has many files, so search the full folder

//	std::vector<CString>	vSubFileNames;
	TCHAR oldDir[MAX_PATH+1];
	GetCurrentDirectory(MAX_PATH, oldDir);
	if (!SetCurrentDirectory(_strSrcFolder))	//	Folder is not exist
	{
		_strTempZipFile.Empty();
		return TRUE;
	}

	WIN32_FIND_DATA FindFileData;
	HANDLE hFind = FindFirstFile("*.*", &FindFileData);	// gets "."
	FindNextFile(hFind, &FindFileData);				// gets ".."
	while(GetLastError() != ERROR_NO_MORE_FILES) 
	{
		if(FindNextFile(hFind, &FindFileData))
		{
			if(!(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) 
			{
				//if the file name include VRInfo, push the file into the list
				if ( strstr(FindFileData.cFileName,_T("VRInfo")) != NULL)
				{
					vZipFiles.push_back( _strSrcFolder + "\\" + FindFileData.cFileName );
				}
			}
		}
		else
			break;				
	}
	FindClose(hFind);
	SetCurrentDirectory(oldDir);
	SetLastError(0);

	if(ZipFiles(strProjName,vZipFiles,_strSrcFolder,_strTempZipFile))
		return TRUE;

	_strTempZipFile.Empty();
	return FALSE;
}

BOOL CProjectExport::ZipInputAirportLandsideFiles(const CString& strProjName, const CString& _strSrcFolder,CString& _strTempZipFile)
{
	CString strZipFileName[] = 
	{
		_T("Filelist.txt"),
			_T("Floors.txt"),
			_T("LAYOUT.LAY"),
			_T("Placements.txt"),
			_T("Stand2GateMapping.txt"),
            _T("ArrivalGateAdjacency.txt"),
            _T("DepartureGateAdjacency.txt"),
			_T("Views.txt")
	};

	int nFileCount = sizeof(strZipFileName)/sizeof(strZipFileName[0]);

	std::vector< CString> vZipFiles;

	for (int i = 0; i < nFileCount; ++i)
	{
		CString strFilePath = _strSrcFolder + "\\" + strZipFileName[i];
		if(PathFileExists(strFilePath))
			vZipFiles.push_back(strFilePath);
	}

	//VRInfo0.txt maybe has many files, so search the full folder

	//	std::vector<CString>	vSubFileNames;
	TCHAR oldDir[MAX_PATH+1];
	GetCurrentDirectory(MAX_PATH, oldDir);
	if (!SetCurrentDirectory(_strSrcFolder))	//	Folder is not exist
	{
		_strTempZipFile.Empty();
		return TRUE;
	}

	WIN32_FIND_DATA FindFileData;
	HANDLE hFind = FindFirstFile("*.*", &FindFileData);	// gets "."
	FindNextFile(hFind, &FindFileData);				// gets ".."
	while(GetLastError() != ERROR_NO_MORE_FILES) 
	{
		if(FindNextFile(hFind, &FindFileData))
		{
			if(!(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) 
			{
				//if the file name include VRInfo, push the file into the list
				if ( strstr(FindFileData.cFileName,_T("VRInfo")) != NULL)
				{
					vZipFiles.push_back( _strSrcFolder + "\\" + FindFileData.cFileName );
				}
			}
		}
		else
			break;				
	}
	FindClose(hFind);
	SetCurrentDirectory(oldDir);
	SetLastError(0);

	if(ZipFiles(strProjName,vZipFiles,_strSrcFolder,_strTempZipFile))
		return TRUE;

	_strTempZipFile.Empty();
	return FALSE;
}


BOOL CProjectExport::ZipFiles( const CString& strProjName, const std::vector<CString>& vZipFiles,const CString& _strSrcFolder, CString& _strTempZipFile )
{
	int iFileCount = vZipFiles.size();
	if (iFileCount == 0)
	{
		_strTempZipFile.Empty();
		return TRUE;
	}

	char **pFiles = (char **) new  int [iFileCount];
	for (int i=0; i<iFileCount; i++)
	{
		pFiles[i] = new char[MAX_PATH+1];
		strcpy( pFiles[i], vZipFiles[i] );
	}

// 	CInfoZip InfoZip;
// 	if (!InfoZip.InitializeZip())
// 	{
// 		EndWaitCursor();
// 		return FALSE;
// 	}
	//InfoZip.
	int nPos = -1;
	if ((nPos = _strSrcFolder.ReverseFind('\\')) != -1)
	{
		_strTempZipFile = getTempPath() + strProjName+"_"+_strSrcFolder.Mid(nPos + 1)+".zip";
	}
	else
	{
		EndWaitCursor();
		return FALSE;
	}

	//CString _strTempInputZipFile = getTempPath() + CString("\\") + pDoc->m_ProjInfo.name+"_input.zip";
	DeleteFile( _strTempZipFile );

	CZipInfo InfoZip;
	if (!InfoZip.AddFiles(_strTempZipFile, pFiles, iFileCount))
	{
	EndWaitCursor();
	return FALSE;
	}


// 	if (!InfoZip.Finalize())
// 	{
// 		EndWaitCursor();
// 		return FALSE;
// 	}

	return TRUE;
}

BOOL CProjectExport::ZipInputFiles( const CString& strProjName,const CString& strProjPath, CString& strZipFilePath )
{
	//ZIP the file of the dictionary  ..\Input\Airport1\Airside\ 
	(_T(""));
	std::vector<CString> vAirportName;
	if( ZipAirportInfo(strProjName,strProjPath,vAirportName) == FALSE)
	{
		AfxMessageBox(_T("Failure to export project! Export airport info"),MB_OK);
		return FALSE;
	}
	(_T(""));
	//zip airside export files
	CString strAirsideZipPath = _T("");
	if(ZipInputAirsideFiles(strProjName,strProjPath,strAirsideZipPath) == FALSE)
	{
		AfxMessageBox(_T("Failure to export project! Export input/airside "),MB_OK);
		return FALSE;
	}
	(_T(""));
	//zip input landside file
	CString strLandsideZipPath = _T("");
	if (ZipInputlandsideFiles(strProjName,strProjPath,strLandsideZipPath) == FALSE)
	{
		AfxMessageBox(_T("Failure to export project! Export input/landside"),MB_OK);
		return FALSE;
	}
	//zip shapebars file
	(_T(""));
	CString strUserBarsZipPath = strProjPath + "\\INPUT\\UserBars";
	CString strTempUserBarsZipPath = (_T(""));
	if(zipFloder(strProjName,strUserBarsZipPath,strTempUserBarsZipPath)==FALSE)
	{
		AfxMessageBox(_T("Failure to export project! Export input/UserBars"),MB_OK);
		return FALSE;
	}

	(_T(""));
	std::vector<CString> vImportName;
    if(ZipImportDB(strProjName,strProjPath,vImportName) == FALSE)
	{
		AfxMessageBox(_T("Failure to export project! Export input/ImportDB "),MB_OK);
		return FALSE;
	}
	(_T(""));
	//Input files 
	CString sProjectPath = strProjPath;
	sProjectPath += "\\INPUT";
	std::vector<CString> vInputFiles;
	FindInputFiles(sProjectPath,vInputFiles);

	//add airport files

	vInputFiles.insert(vInputFiles.end(),vAirportName.begin(),vAirportName.end());
    vInputFiles.insert(vInputFiles.end(),vImportName.begin(),vImportName.end()) ;
	if (!strAirsideZipPath.IsEmpty())
	{
		vInputFiles.push_back(strAirsideZipPath);
	}
	if (!strLandsideZipPath.IsEmpty())
	{
		vInputFiles.push_back(strLandsideZipPath);
	}
	if (!strTempUserBarsZipPath.IsEmpty())
	{
		vInputFiles.push_back(strTempUserBarsZipPath);
	}

	(_T(""));
	if(ZipAirsideDatabaseFiles(sProjectPath,strProjPath,vInputFiles) == FALSE)
		return FALSE;

	(_T(""));
	//close database connetion
	CMasterDatabase *pMasterDatabase = ((CTermPlanApp *)AfxGetApp())->GetMasterDatabase();
	CProjectDatabase projectDatabase(PROJMANAGER->GetAppPath());
	projectDatabase.ExportProjectBegin(pMasterDatabase,strProjName,strProjPath);
	(_T(""));

	//and airside export files
	int iFileCount = vInputFiles.size();		
	char **pFiles = (char **) new  int [iFileCount];
	for (int i=0; i<iFileCount; i++)
	{
		pFiles[i] = new char[MAX_PATH+1];
		strcpy( pFiles[i], vInputFiles[i] );
	}

// 	CInfoZip InfoZip;
// 	if (!InfoZip.InitializeZip())
// 	{
// 		EndWaitCursor();
// 		AfxMessageBox("Failure to export project! Input", MB_OK);
// 		return FALSE;
// 	}
	CZipInfo InfoZip;
	//InfoZip.
	CString strTempPath = getTempPath();
	strTempPath.TrimRight("\\");
	strZipFilePath = strTempPath+ CString("\\") + strProjName+"_input.zip";
	(_T(""));
	DeleteFile( strZipFilePath );
	(_T(""));
	//close airport database connection
	if(m_pTermPlanDoc)
		m_pTermPlanDoc->GetTerminal().m_pAirportDB->GetAirportConnection()->CloseConnection();
	(_T(""));
	if (!InfoZip.AddFiles(strZipFilePath, pFiles, iFileCount))
	{
		EndWaitCursor();
		AfxMessageBox("Failure to export project!", MB_OK);
		return FALSE;
	}
	(_T(""));
// 	if (!InfoZip.Finalize())
// 	{
// 		EndWaitCursor();
// 		AfxMessageBox("Failure to export project!", MB_OK);
// 		return FALSE;
// 	}

	return TRUE;
}
BOOL CProjectExport::ZipAirsideDatabaseFiles( const CString& sInputPath,const CString& strProjName,std::vector<CString>& vInputFiles )
{
	//add database files
	//CString strDBName = _T("");
	//CMasterDatabase *pMasterDatabase = ((CTermPlanApp *)AfxGetApp())->GetMasterDatabase();
	//if(!pMasterDatabase->GetProjectDatabaseName(strProjName,strDBName))
	//{
	//	AfxMessageBox(_T("Failed to export project! Export database name"));
	//	return FALSE;
	//}
	//CString strDBFilePath;
	//CString strDBLogPath;
	//strDBFilePath.Format(_T("%s\\%s.mdf"),sProjectPath,strDBName);
	//strDBLogPath.Format(_T("%s\\%s.ldf"),sProjectPath,strDBName);
	//if (!PathFileExists(strDBFilePath) || !PathFileExists(strDBLogPath))
	//{
	//	AfxMessageBox(_T("Failed to export project! Export database"));
	//	return FALSE;
	//}

	std::vector<CString> vFindFiles;

	CString strTempProjPath;
	strTempProjPath.Format("%s",sInputPath);
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
					strExistDatabaseName.Format(_T("%s\\%s"),sInputPath,FindFileData.cFileName);
					vFindFiles.push_back(strExistDatabaseName);
				}
				else if(CString(FindFileData.cFileName).Right(4).CompareNoCase(_T(".ldf")) == 0)
				{
					CString strExistDatabaseName = _T("");
					strExistDatabaseName.Format(_T("%s\\%s"),sInputPath,FindFileData.cFileName);
					vFindFiles.push_back(strExistDatabaseName);
				}

			}
		}
		else
			continue;				
	}
	FindClose(hFind);
	SetCurrentDirectory(oldDir);
	SetLastError(0);


	if(vFindFiles.size() >1 )
	{
		vInputFiles.insert(vInputFiles.begin(),vFindFiles.begin(),vFindFiles.end());
		return TRUE;
	}
	//else
	//{
	//	AfxMessageBox(_T("Failed to export project! Export database files"));
	//	return FALSE;
	//}

	return TRUE;
}
BOOL CProjectExport::FindInputFiles( const CString& strInputFolder, std::vector<CString >& vInputFiles )
{
	static LPCTSTR const strZipFileName[] = 
	{
		_T("ActivityDensityParams.txt"),
		_T("AdjacencyGateCon.IN"),
		_T("AircraftDisplay.txt"),
		_T("AircraftTags.txt"),
		_T("AirsideWalkthroughFile.txt"),
		_T("AirsideWallShape.txt"),
		_T("AllCarSchedule.IN"),
		_T("Areas.txt"),
		_T("ASSGNMNT.ASN"),

		_T("BAGGAGE.BAG"),
		_T("BAGGATE.IN"),

		_T("CongestionParam.txt"),

		_T("DefDisp.txt"),
		_T("DisplayPropOverrides.txt"),

		_T("Filelist.txt"),
		_T("FLIGHT.DAT"),
		_T("FlightSchduleCriteria.IN"),
		_T("Floors.txt"),
		_T("FlowSyncFile.txt"),
		_T("FLTPRIO.IN"),
		_T("FLTSCHED.SKD"),

		_T("GateAssignCon.IN"),
		_T("GateMappingDB.txt"),
		_T("GATEPRIO.IN"),

		_T("HUBBING.HUB"),

		_T("INTERMEDIATESTANDASSIGN.IN"),

		_T("LAYOUT.LAY"),

		_T("MATRIX.FLO"),
		_T("MobElemTypeStrDB.TXT"),
		_T("MobileElementsDisplay.txt"),
		_T("MobileElementTags.txt"),
		_T("MOVERS.MVR"),//railway data file for RailwayDataFile
		_T("PeopleMove.MVR"),//people move file
		_T("Movies.txt"),
		_T("MovingSideWalK.IN"),
		_T("MovingSideWalkPaxDist.IN"),

		_T("PARAMETR.TXT"),
		_T("PassengerBulkFile.txt"),
		_T("PAXDATA.MSC"),
		_T("PAXDIST.DST"),
		_T("VEHCILE.DST"),
		_T("PAXTYPE.TYP"),
		_T("PipeDataFile.IN"),
		_T("Placements.txt"),
		_T("Portals.txt"),
		_T("PROCDATA.PCM"),
		_T("ProcessUnitInternalFlow.IN"),
		_T("ProcTags.txt"),
		_T("proctoresource.in"),
		_T("ProjectLog.log"),
		_T("ProSwitchTimeDB.txt"),

		_T("ReportParameter.db"),
		_T("ResourceDisplay.txt"),
		_T("ResourcePool.in"),

		_T("SERVICE.SRV"),
		_T("SimAndReport.sim"),
		_T("SpecificFlightPaxFlow.IN"),
		_T("StationLaout.IN"),
		_T("STATIONPAXFLOW.TXT"),

		_T("TerminalStructure.txt"),
		_T("TerminalWallShape.txt"),

		_T("UserNameList.txt"),
		_T("UserShapesBar.INI"),

		_T("VideoParams.txt"),
		_T("Views.txt"),

		_T("Walkthroughs.txt"),
		_T("arcport.mdb"),
		_T("NonPaxRelPos.txt"),
		_T("parts.mdb"),
		_T("BCPAXDATA.TXT"),
        _T("UserShapeBar.txt")
	};

	int nFileCcunt = sizeof(strZipFileName)/sizeof(strZipFileName[0]);

	//std::vector< CString> vZipFiles;

	for (int i = 0; i < nFileCcunt; ++i)
	{
		CString strFilePath = strInputFolder + "\\" + strZipFileName[i];
		if(PathFileExists(strFilePath))
			vInputFiles.push_back(strFilePath);
#ifdef _DEBUG
		else
		{
// 			ASSERT(FALSE);
			TRACE(_T("Can not find file \""));
			TRACE(strFilePath);
			TRACE(_T("\" when exporting project.\n"));
		}
#endif // _DEBUG
	}



	TCHAR oldDir[MAX_PATH+1];
	GetCurrentDirectory(MAX_PATH, oldDir);
	SetCurrentDirectory(strInputFolder);

	WIN32_FIND_DATA FindFileData;
	HANDLE hFind = FindFirstFile("*.*", &FindFileData);	// gets "."
	FindNextFile(hFind, &FindFileData);				// gets ".."
	while(GetLastError() != ERROR_NO_MORE_FILES)
	{
		if(FindNextFile(hFind, &FindFileData)) 
		{
			if(!(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) 
			{
				if ( strstr(FindFileData.cFileName,_T("VRInfo")) != NULL)
				{
					vInputFiles.push_back( strInputFolder + "\\" + FindFileData.cFileName );
				}
				else if ( strstr(FindFileData.cFileName,_T("LayerInfo")) != NULL)
				{
					vInputFiles.push_back( strInputFolder + "\\" + FindFileData.cFileName );
				}
			}
		}
		else
			break;				
	}
	FindClose(hFind);
	SetCurrentDirectory(oldDir);
	SetLastError(0);

	return TRUE;
}

BOOL CProjectExport::ZipMathmaticFiles( const CString& strProjName,const CString& _strSrcFolder, CString& strZipFilePath )
{
	CString strZipFileName[] = 
	{
		_T("Flow.txt"),
		_T("Process.txt")
	};

	int nFileCcunt = sizeof(strZipFileName)/sizeof(strZipFileName[0]);

	std::vector< CString> vZipFiles;

	for (int i = 0; i < nFileCcunt; ++i)
	{	
		CString strFilePath = _strSrcFolder + "\\" + strZipFileName[i];
		if(PathFileExists(strFilePath))
			vZipFiles.push_back(strFilePath);
	}
	if (ZipFiles(strProjName,vZipFiles,_strSrcFolder,strZipFilePath))
		return TRUE;

	return FALSE;
}

BOOL CProjectExport::ZipMathResultFiles( const CString& strProjName,const CString& projPath, CString& strZipFilePath )
{
	return TRUE;
}

BOOL CProjectExport::ZipEconomicFiles( const CString& strProjName,const CString& projPath, CString& strZipFilePath )
{
	CString strZipFileName[] = 
	{
		_T("AdvertisingRevenue.txt"),
		_T("AircraftCost.txt"),
		_T("AirsideFacilitiesCost.txt"),
		_T("AnnualActivityDeduction.txt"),
		_T("BuildingDeprecCost.txt"),
		_T("CapitalBudgetingDecisions.txt"),
		_T("CaptIntrCost.txt"),
		_T("ContractsCost.txt"),
		_T("ContractsRevenue.txt"),
		_T("GateUsageRevenue.txt"),
		_T("InsuranceCost.txt"),
		_T("LaborCost.txt"),
		_T("LandingFeesRevenue.txt"),
		_T("LandsideFacilitiesCost.txt"),
		_T("LandsideTransRevenue.txt"),
		_T("LeasesRevenue.txt"),
		_T("MaintenanceCost.txt"),
		_T("ParkingFeeRevenue.txt"),
		_T("ParkingLotsDeprecCost.txt"),
		_T("PassengerCost.txt"),
		_T("PaxAirportFeeRevenue.txt"),
		_T("ProcessorCost.txt"),
		_T("ProcUsageRevenue.txt"),
		_T("RetailPercRevenue.txt"),
		_T("UtilitiesCost.txt"),
		_T("WaitingCostFactors.txt")
	};

	int nFileCcunt = sizeof(strZipFileName)/sizeof(strZipFileName[0]);

	std::vector< CString> vZipFiles;

	for (int i = 0; i < nFileCcunt; ++i)
	{	
		CString strFilePath = projPath + "\\" + strZipFileName[i];
		if(PathFileExists(strFilePath))
			vZipFiles.push_back(strFilePath);
	}
	if (ZipFiles(strProjName,vZipFiles,projPath,strZipFilePath))
		return TRUE;

	return FALSE;
}


















