// ImportIntoNewProject.cpp : implementation file
//

#include "stdafx.h"
#include "termplan.h"
#include "ImportIntoNewProject.h"
#include "common\InfoZip.h"
#include "Common\ZipInfo.h"
#include "common\fileman.h"
#include "common\termfile.h"
#include "common\airportdatabaselist.h"
#include "common\airportdatabase.h"
#include "Common\INIFileHandle.h"
#include "DlgAirportDBNameInput.h"
#include "../InputAirside/GlobalDatabase.h"
#include "TermPlan.h"

#include <direct.h>
#include "assert.h"
#include "INI.h"

#include "Shlwapi.h"
#include "../InputAirside/InputAirside.h"
#include "ImportExportManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CImportIntoNewProject dialog


CImportIntoNewProject::CImportIntoNewProject(CWnd* pParent /*=NULL*/)
	: CDialog(CImportIntoNewProject::IDD, pParent)
{
	//{{AFX_DATA_INIT(CImportIntoNewProject)
	m_strProjectName = _T("");
	m_strFileName = _T("");
	//}}AFX_DATA_INIT
}


void CImportIntoNewProject::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CImportIntoNewProject)
	DDX_Control(pDX, IDC_EDIT_PROJECT_NAME, m_controlProjectName);
	DDX_Text(pDX, IDC_EDIT_PROJECT_NAME, m_strProjectName);
	DDX_Text(pDX, IDC_EDIT_FILE_NAME, m_strFileName);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CImportIntoNewProject, CDialog)
	//{{AFX_MSG_MAP(CImportIntoNewProject)
	ON_BN_CLICKED(IDC_BUTTON_BROWSE, OnButtonBrowse)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CImportIntoNewProject message handlers

BOOL CImportIntoNewProject::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	
	// init all db combox
/*
	for( int i=0; i< GOLBALDBLIST->getDBCount(); i++ )
	{
		CGlobalDB* _pDB = GOLBALDBLIST->getGlobalDBByIndex(i);
		int _idx = m_comboAllDB.InsertString( -1, _pDB->getName() );
		m_comboAllDB.SetItemData( _idx, (long) _pDB );
	}
	m_comboAllDB.SetCurSel( 0 );*/

	
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CImportIntoNewProject::OnButtonBrowse() 
{
	UpdateData();
	CFileDialog dlgFile( TRUE, ".zip",NULL,OFN_HIDEREADONLY,"zip Files (*.zip)|*.zip||" );	
	if (dlgFile.DoModal() == IDOK)
	{
		m_strFileName = dlgFile.GetPathName();
		UpdateData( FALSE );		
	}
}

void CImportIntoNewProject::OnOK() 
{
	UpdateData();
	m_strProjectName.Trim();
	if( m_strFileName == "" || !( FileManager::IsFile( m_strFileName )) )
	{
		AfxMessageBox("Invalid file location ");
		return;
	}

	if( m_strProjectName =="")
	{
		AfxMessageBox("Invalid project name.");
		m_controlProjectName.SetFocus();
		return;
	}

	CString sProjectName = PROJMANAGER->m_csRootProjectPath + _T("\\") + m_strProjectName;
	if((_mkdir( sProjectName ))!=0)//Director exist
	{
		AfxMessageBox("A project with the specified name already exists", MB_ICONEXCLAMATION | MB_OK);
		m_controlProjectName.SetFocus();
		m_controlProjectName.SetSel(0,-1);
		return;
	}
	//check database, the project name is valid or not

	DoCreateNewProject();

	CString strTempPathFile;
	CreateTempZipFile(strTempPathFile);


	CString strOldProjectName;
	if(DoExtractInputFiles( m_projInfo.path +"\\INPUT",strTempPathFile,strOldProjectName))
	{
		//unzip economic files
		CString sEconomicPath = m_projInfo.path + CString(_T("\\ECONOMIC"));
		FileManager::DeleteDirectory( sEconomicPath );
		CreateDirectory(sEconomicPath,NULL);
		DoExtractEconomicFiles(strOldProjectName,sEconomicPath,strTempPathFile);
		if(CImportExportManager::ImportProject(m_projInfo.path,m_projInfo.name,false) == false)
		{
			//MessageBox(_T("Import project failed."));
			return;
		}

		// update project info file
		PROJMANAGER->UpdateProjInfoFile( m_projInfo );
		AfxGetApp()->OpenDocumentFile( sProjectName );
	}
	else
	{
		FileManager::DeleteDirectory( m_projInfo.path );
	}

	DeleteTempZipFile(strTempPathFile);


	CDialog::OnOK();
}

bool CImportIntoNewProject::DoExtractFiles(CString _strPath)
{
	BeginWaitCursor();

	CString strTempPath = getTempPath();
	strTempPath.TrimRight('\\');
	CString _strTempExtractPath = strTempPath + "\\" + "ARCTempExtractPath";
	FileManager::DeleteDirectory( _strTempExtractPath );
	CreateDirectory( _strTempExtractPath, NULL );
	
	// extract zip to temp path
	if( !unzipFiles( m_strFileName,_strTempExtractPath) )
	{
		EndWaitCursor();
		FileManager::DeleteDirectory( _strTempExtractPath );
		return false;
	}

	// check if have arcterm_export.ini file
	if( !FileManager::IsFile( _strTempExtractPath + "\\" + "arcterm_export.ini" ) )  // no ini file( old versin )
	{
		// only copy the temp path to input path
		FileManager::CopyDirectory( _strTempExtractPath, _strPath ,NULL);
		FileManager::DeleteDirectory( _strTempExtractPath );

		CDlgAirportDBNameInput dlg( "Please Enter The Airport Name for the Imported Project:" );
		CString sInput("");
		if(dlg.DoModal()==IDCANCEL)
		{
			return false;
		}
		sInput=dlg.m_sAirportDBName;
		
        m_projInfo.dbname = sInput ;
		m_projInfo.lUniqueIndex = -1;
		m_projInfo.version = 300 ; //set the version to old version .
		EndWaitCursor();
		return true;
	}

	// open ini file
	CString _strInputZip, _strDBZip, _strMathematicZip, _strMathResultZip;
	BOOL bNotForceDB;
	readIniFileData( _strTempExtractPath, _strInputZip, _strDBZip, _strMathematicZip,
		_strMathResultZip, bNotForceDB);
	

	// unzip input file
	if( !unzipInputFiles( _strInputZip, _strPath ) )
	{
		EndWaitCursor();
		FileManager::DeleteDirectory( _strTempExtractPath );
		return false;
	}

	//..\input\airport1\airside\
	//Unzip Airport file
	CString strOldProjName;
	int nPositon = _strInputZip.ReverseFind('\\');
	strOldProjName = _strInputZip.Mid(nPositon + 1);
	nPositon = strOldProjName.ReverseFind('_');
	strOldProjName = strOldProjName.Mid(0,nPositon + 1);

	unzipAirportFiles(strOldProjName,_strPath);

	unzipAirsideExportFiles(strOldProjName,_strPath);

	if(m_ZipFileVersion > 2)
		UnZipImportDBFiles(_strTempExtractPath,strOldProjName,_strPath);
	int nPos = _strPath.ReverseFind('\\');
	CString strDestPath = _strPath.Left(nPos);
	// unzip Mathematic file
	if (!_strMathematicZip.IsEmpty())
	{
		if (!unzipFiles(_strMathematicZip, strDestPath + "\\Mathematic"))
		{
			EndWaitCursor();
			FileManager::DeleteDirectory( _strTempExtractPath );
			return false;
		}
	}

	// unzip MathResult file
	if (!_strMathResultZip.IsEmpty())
	{
		if (!unzipFiles(_strMathResultZip, strDestPath + "\\MathResult"))
		{
			EndWaitCursor();
			FileManager::DeleteDirectory( _strTempExtractPath );
			return false;
		}
	}
	if(m_projInfo.version <= 300)
	{
		// unzip db file
		if( !unzipDBFiles( _strDBZip,bNotForceDB?true:false,_strTempExtractPath,_strPath ) )
		{
		EndWaitCursor();
		FileManager::DeleteDirectory( _strTempExtractPath );
		return false;
		}
	}

	
	EndWaitCursor();
	FileManager::DeleteDirectory( _strTempExtractPath );
	return true;
}
CString CImportIntoNewProject::GetDBName(const CString& _strTempExtractPath)
{
	CString sProjInfoFile=_strTempExtractPath+"\\ProjectDB.txt";
	TCHAR sDBNameFromProjInfo[MAX_PATH]={'\0'};
	if(FileManager::IsFile((LPCSTR)sProjInfoFile))
	{
		fsstream fProjInfo(sProjInfoFile,stdios::in);
		fProjInfo.getline(sDBNameFromProjInfo,MAX_PATH);
	}
	return CString(sDBNameFromProjInfo) ;
}
void CImportIntoNewProject::UnZipImportDBFiles(const CString& _strTempExtractPath , const CString OldProjName,const CString& _strPath)
{
	CString strZipAirportDBPath;
	CString DBPath ;
	strZipAirportDBPath.Format(_T("%s\\%sAirportDB.zip"),_strPath , OldProjName);

	int nAirport = 1;
	//make sure the airport file is exist
	if(PathFileExists((LPCSTR)strZipAirportDBPath))
	{
		//unzip this airport file
		//generate the airport path
		FileManager fm ;
		CString airportPath ;
		airportPath.Format(_T("%s\\AirportDB"),_strPath,nAirport);
		fm.DeleteDirectory(airportPath) ;
		if(CreateDirectory(airportPath,NULL))
			unzipFiles(strZipAirportDBPath,airportPath);
		DeleteFile((LPCSTR)strZipAirportDBPath) ;



		CString DBName = GetDBName(_strTempExtractPath) ;

		//unzip ImportDB files
		DBPath = airportPath+_T("\\") + OldProjName +DBName + ".zip";
		CString airportDBNnamePath = airportPath + _T("\\") + DBName;
		fm.DeleteDirectory(airportDBNnamePath) ;
		if(CreateDirectory(airportDBNnamePath,NULL))
		    unzipFiles(DBPath,airportDBNnamePath);
	    DeleteFile((LPCSTR)DBPath);	
	}	
	return;
}
bool CImportIntoNewProject::unzipAirsideExportFiles(const CString& oldProjName, const CString& _strPath)
{
	//unzip airside zip
	CString strAirsideZipPath = _T("");
	strAirsideZipPath.Format(_T("%s//%sAirside.zip"),_strPath,oldProjName);

	if (PathFileExists((LPCSTR)strAirsideZipPath))
	{
		CString strAirsidePath = _T("");
		strAirsidePath.Format(_T("%s\\Airside"),_strPath);
		//if (FileManager::pa)
		//{
		//}
		if(!PathFileExists(strAirsidePath))
		{
			if (!CreateDirectory(strAirsidePath,NULL))
			{
				return false;
			}
		}

		if (!unzipFiles(strAirsideZipPath,strAirsidePath))
			return false;

		DeleteFile((LPCSTR)strAirsideZipPath );
	}

	return true;
}

bool CImportIntoNewProject::unzipInputLandsideFile(const CString oldProjName,const CString& _strPath)
{
	//unzip landside zip
	CString strLandsideZipPath = _T("");
	strLandsideZipPath.Format(_T("%s//%sLandside.zip"),_strPath,oldProjName);

	if (PathFileExists((LPCSTR)strLandsideZipPath))
	{
		CString strAirsidePath = _T("");
		strAirsidePath.Format(_T("%s\\Landside"),_strPath);
		if (!PathFileExists((LPCSTR)strAirsidePath))
		{
			if (!CreateDirectory(strAirsidePath,NULL))
			{
				return false;
			}
		}
		if (!unzipFiles(strLandsideZipPath,strAirsidePath))
			return false;

		DeleteFile((LPCSTR)strLandsideZipPath );
	}
	return true;
}
void CImportIntoNewProject::DoCreateNewProject()
{
	_rmdir(PROJMANAGER->m_csRootProjectPath + _T("\\") + m_strProjectName);
	PROJMANAGER->CreateProject( m_strProjectName,&m_projInfo, false);
}

// pop folder selection, return true ok, return false cancel
bool CImportIntoNewProject::SelectDirectory(  const CString& _strTitle, CString& _strPath )
{
	BROWSEINFO   bi;
    TCHAR        szDir[MAX_PATH];
    LPITEMIDLIST pidl;
    LPMALLOC     pMalloc;
	bool		 _bResult = false;	
	
	if( SUCCEEDED( SHGetMalloc( &pMalloc ) ) )
	{
		ZeroMemory( &bi,sizeof(bi) );
		bi.hwndOwner      = m_hWnd;
		bi.lpszTitle      = _strTitle;
        bi.pszDisplayName = 0;
        bi.pidlRoot       = 0;
        bi.ulFlags        = BIF_EDITBOX | BIF_RETURNONLYFSDIRS;
        bi.lpfn           = NULL;
		
        pidl = SHBrowseForFolder( &bi );
		
        if( pidl )
		{
			if( SHGetPathFromIDList( pidl, szDir ) )
			{
				_strPath = szDir;
				_bResult = true;
			}
			
			pMalloc->Free( pidl );
			pMalloc->Release();
        }
	}
	
	return _bResult;
}

CString CImportIntoNewProject::getTempPath( const CString& _strDefault  )
{
	char _szTempPath[ MAX_PATH + 1];
	if( GetTempPath( MAX_PATH, _szTempPath ) == 0 )
		return  _strDefault;
	return CString( _szTempPath );
}

bool CImportIntoNewProject::unzipFiles( const CString& _strZip, const CString& _strPath )
{
	if(PathFileExists(_strZip) == FALSE)
		return true;

	BeginWaitCursor();
	
	CZipInfo InfoZip;
// 	if (!InfoZip.InitializeUnzip())
// 	{
// 		EndWaitCursor();
// 		AfxMessageBox("Must be initialized", MB_OK);
// 		return false;
// 	}
	if (!InfoZip.ExtractFiles(_strZip,CString(_strPath + "\\")))
	{
		EndWaitCursor();
		AfxMessageBox("Unzip file failed! file: " + _strZip, MB_OK);
		return false;
	}
	
// 	if (!InfoZip.FinalizeUnzip())
// 	{
// 		EndWaitCursor();
// 		AfxMessageBox("Cannot finalize", MB_OK);
// 		return false;
// 	}
	
	EndWaitCursor();
	return true;
}


bool CImportIntoNewProject::readIniFileData( const CString& _strTempExtractPath, CString& _strInputZip, CString& _strDBZip,
											CString& _strMathematicZip, CString& _strMathResultZip, BOOL& bNotForceDB)
{
	ArctermFile inifile;
	inifile.openFile( _strTempExtractPath + "\\" + "arcterm_export.ini", READ );
	
	int _iVersion = 0;
	inifile.getLine();
	inifile.getInteger( _iVersion );
	m_ZipFileVersion = _iVersion ;

	if(_iVersion >= 4)
	{
		inifile.getLine();
		char projectversion[MAXPATH] = {0};
		inifile.getField(projectversion,MAXPATH) ;
		inifile.getInteger(  m_projInfo.version);
	}else
		m_projInfo.version = 300 ;

	char _szFileName[MAXPATH];
	inifile.getLine();
	inifile.skipField(1);
	inifile.getField( _szFileName, MAXPATH );
	_strInputZip = _strTempExtractPath + "\\" + _szFileName;

	inifile.getLine();
	inifile.skipField(1);
	inifile.getField( _szFileName, MAXPATH );
	_strDBZip = _strTempExtractPath + "\\" + _szFileName;
	
	char _szName[MAXPATH];
	inifile.getLine();
	inifile.getField( _szName, MAXPATH );
	if (strcmp(_szName, "MATHEMATICZIP") == 0)
	{
		inifile.getField(_szFileName, MAXPATH);
		_strMathematicZip = _strTempExtractPath + "\\" + _szFileName;

		inifile.getLine();
		inifile.getField(_szName, MAXPATH);
		if (strcmp(_szName, "MATHRESULTZIP") == 0)
		{
			inifile.getField(_szFileName, MAXPATH);
			_strMathResultZip = _strTempExtractPath + "\\" + _szFileName;

			if(_iVersion>=2)	// Has Not_Force_DB Info
			{
				
				inifile.getInteger(bNotForceDB);
			}
			
		}
		else
		{
			if(_iVersion>=2)	// Has Not_Force_DB Info
			{
				
				inifile.getInteger(bNotForceDB);
			}
			
		}
	}
	else if (strcmp(_szName, "MATHRESULTZIP") == 0)
	{
		inifile.getField(_szFileName, MAXPATH);
		_strMathResultZip = _strTempExtractPath + "\\" + _szFileName;

		if(_iVersion>=2)	// Has Not_Force_DB Info
		{
			
			inifile.getInteger(bNotForceDB);
		}
		
	}
	else
	{
		if(_iVersion>=2)	// Has Not_Force_DB Info
		{
			
			inifile.getInteger(bNotForceDB);
		}
		
	}

	
	
	return true;
}

bool CImportIntoNewProject::unzipInputFiles( const CString& _strInputZip, const CString& _strPath )
{
	return unzipFiles( _strInputZip, _strPath);
}

bool CImportIntoNewProject::unzipDBFiles( const CString& _strDBZip,bool bNotForceDB,const CString& _strTempExtractPath,const CString& DesstrDirectory)
{
	if(m_ZipFileVersion > 2)
		return TRUE ;
	CString sProjInfoFile=_strTempExtractPath+"\\ProjectDB.txt";
	TCHAR sDBNameFromProjInfo[MAX_PATH]={'\0'};
	BOOL bHaveValidateProjInfo=FALSE;
	if(FileManager::IsFile((LPCSTR)sProjInfoFile))
	{
		fsstream fProjInfo(sProjInfoFile,stdios::in);
		fProjInfo.getline(sDBNameFromProjInfo,MAX_PATH);
		m_projInfo.dbname = sDBNameFromProjInfo ;
	}else
	{
		CDlgAirportDBNameInput dlg( "Please Enter The Airport Name for the Imported Project:" );
		CString sInput("");
		if(dlg.DoModal()==IDCANCEL)
		{
			return false;
		}
		m_projInfo.dbname = dlg.m_sAirportDBName;
	}
	CString arcportDbDirectory = DesstrDirectory + "\\AirportDB"+ "\\" +  m_projInfo.dbname ;
	if(FileManager::IsDirectory(arcportDbDirectory))
		FileManager::DeleteDirectory(arcportDbDirectory) ;
	FileManager::MakePath(arcportDbDirectory) ;
	if(FileManager::IsFile(_strDBZip))
	{
		if(!unzipFiles(_strDBZip,arcportDbDirectory))
			return FALSE ;
	}
	return true;
}


bool CImportIntoNewProject::unzipAirportFiles(const CString OldProjName,const CString& _strPath)
{
	//unzip airport.zip

	//generate the airport path

	CString strZipAirportPath;
	strZipAirportPath.Format(_T("%s\\%sAirport1.zip"),_strPath , OldProjName);
	
	int nAirport = 1;
	//make sure the airport file is exist
	while(PathFileExists((LPCSTR)strZipAirportPath))
	{
		//unzip this airport file
		//generate the airport path
		CString airportPath ;
		airportPath.Format(_T("%s\\airport%d"),_strPath,nAirport);
		CreateDirectory(airportPath,NULL);
		if (!unzipFiles(strZipAirportPath,airportPath))
			return false;
		
		//delete the airport zip file
		DeleteFile((LPCSTR)strZipAirportPath) ;

		//unzip airside files
		CString strZipAirsidePath = airportPath+_T("\\") + OldProjName +"airside.zip";
		CString airsidePath = airportPath + _T("\\airside");
		
		CreateDirectory(airsidePath,NULL);

		if (!unzipFiles(strZipAirsidePath,airsidePath))
			return false;

		//delete the airside zip file
		DeleteFile((LPCSTR)strZipAirsidePath);

		//unzip landside files

		CString strZipLandsidePath = airportPath+_T("\\") + OldProjName + "landside.zip";
		CString landsidepath = airportPath +_T("\\landside");

		CreateDirectory(landsidepath,NULL);
		if(!unzipFiles(strZipLandsidePath,landsidepath)) 
			return false;

		DeleteFile((LPCSTR)strZipLandsidePath );

		//Next airport file
		nAirport += 1;
		strZipAirportPath.Format(_T("%s\\%sAirport%d.zip"),_strPath,OldProjName,nAirport);
	}

	return true;
}

bool CImportIntoNewProject::CreateTempZipFile( CString& strTempExtractPath )
{
	BeginWaitCursor();
	CString strTempPath = getTempPath();
	strTempPath.TrimRight('\\');
	strTempExtractPath = strTempPath + "\\" + "ARCTempExtractPath";
	FileManager::DeleteDirectory( strTempExtractPath );
	CreateDirectory( strTempExtractPath, NULL );
	// extract zip to temp path
	if( !unzipFiles( m_strFileName, strTempExtractPath ) )
	{
		return false;
	}	
	return true;
}

void CImportIntoNewProject::DeleteTempZipFile( const CString& strTempFilePath )
{
	EndWaitCursor();
	FileManager::DeleteDirectory( strTempFilePath );
}

bool CImportIntoNewProject::DoExtractInputFiles( const CString& strPath,const CString& strTempFilePath,CString& strOldProjName )
{
	// check if have arcterm_export.ini file
	if( !FileManager::IsFile( strTempFilePath + "\\" + "arcterm_export.ini" ) )  // no ini file( old versin )
	{
		// only copy the temp path to input path
		FileManager::CopyDirectory( strTempFilePath, strPath ,NULL);
		FileManager::DeleteDirectory( strTempFilePath );

		CDlgAirportDBNameInput dlg( "Please Enter The Airport Name for the Imported Project:" );
		CString sInput("");
		if(dlg.DoModal()==IDCANCEL)
		{
			return false;
		}
		sInput=dlg.m_sAirportDBName;

		m_projInfo.dbname = sInput ;
		m_projInfo.lUniqueIndex = -1;
		m_projInfo.version = 300 ; //set the version to old version .
		EndWaitCursor();
		return true;
	}

	// open ini file
	CString _strInputZip, _strDBZip, _strMathematicZip, _strMathResultZip;
	BOOL bNotForceDB;
	readIniFileData( strTempFilePath, _strInputZip, _strDBZip, _strMathematicZip,
		_strMathResultZip, bNotForceDB);


	// unzip input file
	if( !unzipInputFiles( _strInputZip, strPath ) )
	{
		return false;
	}

	//..\input\airport1\airside\
	//Unzip Airport file
	int nPositon = _strInputZip.ReverseFind('\\');
	strOldProjName = _strInputZip.Mid(nPositon + 1);
	nPositon = strOldProjName.ReverseFind('_');
	strOldProjName = strOldProjName.Mid(0,nPositon + 1);

	unzipAirportFiles(strOldProjName,strPath);

	unzipAirsideExportFiles(strOldProjName,strPath);

	unzipInputLandsideFile(strOldProjName,strPath);

	if(m_ZipFileVersion > 2)
		UnZipImportDBFiles(strTempFilePath,strOldProjName,strPath);
	int nPos = strPath.ReverseFind('\\');
	CString strDestPath = strPath.Left(nPos);
	// unzip Mathematic file
	if (!_strMathematicZip.IsEmpty())
	{
		if (!unzipFiles(_strMathematicZip, strDestPath + "\\Mathematic"))
		{
			return false;
		}
	}

	// unzip MathResult file
	if (!_strMathResultZip.IsEmpty())
	{
		if (!unzipFiles(_strMathResultZip, strDestPath + "\\MathResult"))
		{
			return false;
		}
	}
	if(m_projInfo.version <= 300)
	{
		// unzip db file
		if( !unzipDBFiles( _strDBZip,bNotForceDB?true:false,strTempFilePath,strPath ) )
		{
			return false;
		}
	}

	return true;
}

bool CImportIntoNewProject::DoExtractEconomicFiles( const CString& strProjectName,const CString& strPath,const CString& strTempFilePath )
{
	CString strTempEconomicFile;
	strTempEconomicFile = strTempFilePath + CString(_T("\\")) + strProjectName + CString(_T("ECONOMIC.zip"));

	return unzipFiles(strTempEconomicFile,strPath);
}