// ImportIntoOldProjectDlg.cpp : implementation file
//

#include "stdafx.h"
#include "termplan.h"
#include "ImportIntoOldProjectDlg.h"
#include "common\fileman.h"
#include "Common\ZipInfo.h"
//#include "common\InfoZip.h"
#include "common\termfile.h"
#include "common\airportdatabaselist.h"
#include "common\airportdatabase.h"
#include "Common\INIFileHandle.h"
#include "INI.h"
#include "DlgAirportDBNameInput.h"

#include "Shlwapi.h"

#include "ImportExportManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CImportIntoOldProjectDlg dialog


CImportIntoOldProjectDlg::CImportIntoOldProjectDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CImportIntoOldProjectDlg::IDD, pParent), m_lPrjectDB(0l)
{
	//{{AFX_DATA_INIT(CImportIntoOldProjectDlg)
	m_strSourceFile = _T("");
	//}}AFX_DATA_INIT
}


void CImportIntoOldProjectDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CImportIntoOldProjectDlg)
	DDX_Control(pDX, IDC_EDIT_SOURCE_ZIPFILE, m_editSrcZip);
	DDX_Control(pDX, IDC_LIST_EXIST_PROJECT, m_listExistProject);
	DDX_Text(pDX, IDC_EDIT_SOURCE_ZIPFILE, m_strSourceFile);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CImportIntoOldProjectDlg, CDialog)
	//{{AFX_MSG_MAP(CImportIntoOldProjectDlg)
	ON_BN_CLICKED(IDC_BUTTON_BROWSE, OnButtonBrowse)
	ON_NOTIFY(LVN_GETDISPINFO, IDC_LIST_EXIST_PROJECT, OnGetdispinfoListExistProject)
	ON_NOTIFY(LVN_COLUMNCLICK, IDC_LIST_EXIST_PROJECT, OnColumnclickListExistProject)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CImportIntoOldProjectDlg message handlers


BOOL CImportIntoOldProjectDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	CString colname;
	CRect listrect;
	m_listExistProject.GetClientRect(&listrect);
	colname.LoadString(IDS_PSD_PROJNAME);
	m_listExistProject.InsertColumn(0, colname, LVCFMT_LEFT, listrect.Width()/4);
	colname.LoadString(IDS_PSD_USERNAME);
	m_listExistProject.InsertColumn(1, colname, LVCFMT_LEFT, listrect.Width()/6);
	colname.LoadString(IDS_PSD_MACHINENAME);
	m_listExistProject.InsertColumn(2, colname, LVCFMT_LEFT, listrect.Width()/6);
	colname.LoadString(IDS_PSD_DATECREATED);
	m_listExistProject.InsertColumn(3, colname, LVCFMT_LEFT, listrect.Width()/4);
	colname.LoadString(IDS_PSD_DATEMODIFIED);
	m_listExistProject.InsertColumn(4, colname, LVCFMT_LEFT, listrect.Width()/4);

	m_listExistProject.ModifyStyle(LVS_TYPEMASK, LVS_REPORT);

	ListView_SetExtendedListViewStyle(m_listExistProject.GetSafeHwnd(),LVS_EX_FULLROWSELECT);

	
	//now populate the list
	std::vector<CString> vList;

	CProjectManager* pProjectManager=CProjectManager::GetInstance();
	pProjectManager->GetProjectList(vList);
	for(int i=0; i<static_cast<int>(vList.size()); i++) 
	{
		CString projName = vList[i];
		PROJECTINFO* pi = new PROJECTINFO();
		PROJMANAGER->GetProjectInfo(projName, pi);
		LV_ITEM lvi;
		lvi.mask = LVIF_TEXT |  LVIF_PARAM;
		lvi.iItem = i;
		lvi.iSubItem = 0;
		lvi.iImage = 0;
		lvi.pszText = LPSTR_TEXTCALLBACK;
		lvi.lParam = (LPARAM) pi;
		if(pi->isDeleted !=1)	// don't include deleted project
		{
			m_listExistProject.InsertItem(&lvi);
		}
	}	

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CImportIntoOldProjectDlg::OnGetdispinfoListExistProject(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
	
	if(pDispInfo->item.mask & LVIF_TEXT) {
		PROJECTINFO* pInfo = (PROJECTINFO*) pDispInfo->item.lParam;
		switch(pDispInfo->item.iSubItem) {
		case 0: //proj name
			lstrcpy(pDispInfo->item.pszText, pInfo->name);
			break;
		case 1: //user
			lstrcpy(pDispInfo->item.pszText, pInfo->user);
			break;
		case 2: //machine
			lstrcpy(pDispInfo->item.pszText, pInfo->machine);
			break;
		case 3:
			lstrcpy(pDispInfo->item.pszText, pInfo->createtime.Format("%x %X"));
			break;
		case 4:
			lstrcpy(pDispInfo->item.pszText, pInfo->modifytime.Format("%x %X"));
			break;
		}
	}
	
	*pResult = 0;
}

int CALLBACK CImportIntoOldProjectDlg::CompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	//static int sortorder = 0;
	PROJECTINFO* pItem1 = (PROJECTINFO*) lParam1;
	PROJECTINFO* pItem2 = (PROJECTINFO*) lParam2;
	int nResult;
	switch(lParamSort) {
	case 0: // sort by proj name
		nResult = pItem1->name.CompareNoCase(pItem2->name);
		break;
	case 1: // sort by user
		nResult = pItem1->user.CompareNoCase(pItem2->user);
		break;
	case 2: // sort by machine
		nResult = pItem1->machine.CompareNoCase(pItem2->machine);
		break;
	case 3: // sort by create date
		nResult = (pItem1->createtime < pItem2->createtime) ? 1 : -1;
		if(pItem1->createtime == pItem2->createtime)
			nResult = 0;
		break;
	case 4: // sort by modify date
		nResult = (pItem1->modifytime < pItem2->modifytime) ? 1 : -1;
		if(pItem1->modifytime == pItem2->modifytime)
			nResult = 0;
		break;
	}
	//if(sortorder = (++sortorder % 2))
	//	nResult = -nResult;
	return nResult;
}

void CImportIntoOldProjectDlg::OnColumnclickListExistProject(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	m_listExistProject.SortItems(CompareFunc, pNMListView->iSubItem);		
	*pResult = 0;
}
void CImportIntoOldProjectDlg::OnButtonBrowse() 
{
	CFileDialog dlgFile( TRUE, ".zip",NULL,OFN_HIDEREADONLY,"zip Files (*.zip)|*.zip||", NULL, 0, FALSE  );	
	if (dlgFile.DoModal() == IDOK)
	{
		m_strSourceFile = dlgFile.GetPathName();
		//UpdateData( FALSE );		
		m_editSrcZip.SetWindowText( m_strSourceFile );
	}
	
}

void CImportIntoOldProjectDlg::OnOK() 
{
	UpdateData();
	if( m_strSourceFile == "" || !( FileManager::IsFile( m_strSourceFile )) )
	{
		AfxMessageBox("Invalid file location ");
		return;
	}
	
	if( m_listExistProject.GetSelectedCount() <= 0 )
	{
		AfxMessageBox("Please select a project to import into.");
		return;
	}

	if(!DoImport())
		return;

	CDialog::OnOK();
}

bool CImportIntoOldProjectDlg::DoImport()
{
	POSITION pos;
	int selectNum=m_listExistProject.GetSelectedCount();

	if (selectNum>0)
	{
		pos = m_listExistProject.GetFirstSelectedItemPosition();		  
		if(pos!=NULL)
		{
			int nItem = m_listExistProject.GetNextSelectedItem(pos);		  
		  	PROJECTINFO* pi =(PROJECTINFO* ) m_listExistProject.GetItemData(nItem);

			CString strTempPathFile;
			CreateTempZipFile(strTempPathFile);

			CString sInputPath = pi->path + "\\INPUT";
			FileManager::DeleteDirectory( sInputPath );
			CreateDirectory(sInputPath,NULL);
//			m_lPrjectDB = 0l;
			CString strOldProjectName;
			if(DoExtractInputFiles( sInputPath , pi,strTempPathFile,strOldProjectName))
			{	
				//unzip economic files
				CString strProjectName;
				strProjectName = pi->name;
				CString sEconomicPath = pi->path + CString(_T("\\ECONOMIC"));
				FileManager::DeleteDirectory( sEconomicPath );
				CreateDirectory(sEconomicPath,NULL);
				DoExtractEconomicFiles(strOldProjectName,sEconomicPath,strTempPathFile);

				// update project info file
				pi->modifytime = pi->createtime = CTime::GetCurrentTime();
				//pi->lUniqueIndex = m_lPrjectDB;
				PROJMANAGER->UpdateProjInfoFile( *pi );


				if(CImportExportManager::ImportProject(pi->path,pi->name,true) == false)
				{
					//MessageBox(_T("Import project failed."));
					return false;
				}



				AfxGetApp()->OpenDocumentFile( pi->path );
			}	
		}
	}
	return true;
}
bool CImportIntoOldProjectDlg::DoExtractFiles(CString _strPath,PROJECTINFO* pi )
{
	BeginWaitCursor();
	CString strTempPath = getTempPath();
	strTempPath.TrimRight('\\');
	CString _strTempExtractPath = strTempPath + "\\" + "ARCTempExtractPath";
	FileManager::DeleteDirectory( _strTempExtractPath );
	CreateDirectory( _strTempExtractPath, NULL );

	// extract zip to temp path
	if( !unzipFiles( m_strSourceFile, _strTempExtractPath ) )
	{
		EndWaitCursor();
		FileManager::DeleteDirectory( _strTempExtractPath );
		return false;
	}	
	// check if have arcterm_export.ini file
	if( !FileManager::IsFile( _strTempExtractPath + "\\" + "arcterm_export.ini" ) )  // no ini file( old versin )
	{
		// only copy the temp path to input path
		FileManager::CopyDirectory( _strTempExtractPath, _strPath,NULL );
		FileManager::DeleteDirectory( _strTempExtractPath );
	//	m_lPrjectDB = 0l;
		CDlgAirportDBNameInput dlg( "Please Enter The Airport Name for the Imported Project:" );
		CString sInput("");
		if(dlg.DoModal()==IDCANCEL)
		{
			return false;
		}
		sInput=dlg.m_sAirportDBName;
		
		pi->dbname = sInput ;
		pi->version = 300 ;
		EndWaitCursor();
		return true;
	}
	// open ini file
	CString _strInputZip, _strDBZip;
	BOOL bNotForceDB;
	readIniFileData( _strTempExtractPath, _strInputZip, _strDBZip,bNotForceDB,pi);
	
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
	//airside export files
	unzipAirsideExportFiles(strOldProjName,_strPath);
	
	//airport files
	unzipAirportFiles(strOldProjName,_strPath);
     
	if(m_ZipFileVersion > 2)
         UnZipImportDBFiles(_strTempExtractPath,strOldProjName,_strPath);
	if(pi->version <= 300)
	{
		// unzip db file
		if( !unzipDBFiles(  _strDBZip,bNotForceDB?true:false,_strTempExtractPath,pi,_strPath ) )
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
CString CImportIntoOldProjectDlg::GetDBName(const CString& _strTempExtractPath)
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
void CImportIntoOldProjectDlg::UnZipImportDBFiles(const CString& _strTempExtractPath , const CString OldProjName,const CString& _strPath)
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
bool CImportIntoOldProjectDlg::unzipInputFiles( const CString& _strInputZip, const CString& _strPath )
{
	return unzipFiles( _strInputZip, _strPath);
}


bool CImportIntoOldProjectDlg::unzipFiles( const CString& _strZip, const CString& _strPath )
{
	if(PathFileExists(_strZip) == FALSE)
		return true;

	BeginWaitCursor();
	
	// extract zip to temp path
	CZipInfo InfoZip;
// 	if (!InfoZip.InitializeUnzip())
// 	{
// 		EndWaitCursor();
// 		AfxMessageBox("Must be initialized", MB_OK);
// 		return false;
// 	}
	
	if (!InfoZip.ExtractFiles(_strZip, CString(_strPath + "\\")))
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

bool CImportIntoOldProjectDlg::unzipDBFiles( const CString& _strDBZip,bool bNotForceDB,const CString& _strTempExtractPath ,PROJECTINFO* pi,const CString& DesstrDirectory)
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
		pi->dbname = sDBNameFromProjInfo ;
	}else
	{
		CDlgAirportDBNameInput dlg( "Please Enter The Airport Name for the Imported Project:" );
		CString sInput("");
		if(dlg.DoModal()==IDCANCEL)
		{
			return false;
		}
		pi->dbname = dlg.m_sAirportDBName;
	}
	CString arcportDbDirectory = DesstrDirectory + "\\AirportDB" +"\\" +  pi->dbname;
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


CString CImportIntoOldProjectDlg::getTempPath( const CString& _strDefault )
{
	char _szTempPath[ MAX_PATH + 1];
	if( GetTempPath( MAX_PATH, _szTempPath ) == 0 )
		return  _strDefault;
	return CString( _szTempPath );
}


bool CImportIntoOldProjectDlg::readIniFileData( const CString& _strTempExtractPath, CString& _strInputZip, CString& _strDBZip ,BOOL& bNotForceDB,PROJECTINFO* pi)
{
	ArctermFile inifile;
	inifile.openFile( _strTempExtractPath + "\\" + "arcterm_export.ini", READ );

	int _iVersion = 0;
	inifile.getLine();
	inifile.getInteger( _iVersion );
	m_ZipFileVersion = _iVersion ;

	if(_iVersion >= 4)
	{
		inifile.getLine() ;
		inifile.skipField(1) ;
		inifile.getInteger(pi->version) ;
	}else
		pi->version = 300 ;

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

	if(_iVersion>=2)	// Has Not_Force_DB Info
	{
		inifile.getLine();
		inifile.skipField(1);
		inifile.getInteger(bNotForceDB);
	}

	return true;
}

// pop folder selection, return true ok, return false cancel
bool CImportIntoOldProjectDlg::SelectDirectory(  const CString& _strTitle, CString& _strPath )
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

void CImportIntoOldProjectDlg::unzipAirportFiles(const CString OldProjName,const CString& _strPath)
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
		if(!CreateDirectory(airportPath,NULL))
			continue;
		if (!unzipFiles(strZipAirportPath,airportPath))
			continue;

		//delete the airport zip file
		DeleteFile((LPCSTR)strZipAirportPath) ;

		//unzip airside files
		CString strZipAirsidePath = airportPath+_T("\\") + OldProjName +"airside.zip";
		CString airsidePath = airportPath + _T("\\airside");

		if(!CreateDirectory(airsidePath,NULL))
			continue;
		if (!unzipFiles(strZipAirsidePath,airsidePath))
			continue;
		//delete the airside zip file
		DeleteFile((LPCSTR)strZipAirsidePath);

		//unzip landside files
		CString strZipLandsidePath = airportPath+_T("\\") + OldProjName + "landside.zip";
		CString landsidepath = airportPath +_T("\\landside");

		if(!CreateDirectory(landsidepath,NULL)) continue;
		if(!unzipFiles(strZipLandsidePath,landsidepath)) continue;

		DeleteFile((LPCSTR)strZipLandsidePath );

		//Next airport file
		nAirport += 1;
		strZipAirportPath.Format(_T("%s\\%sAirport%d.zip"),_strPath,OldProjName,nAirport);
	}

	return;
}

void CImportIntoOldProjectDlg::unzipInputLandsideFile(const CString oldProjName,const CString& _strPath)
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
				return;
			}
		}
		if (!unzipFiles(strLandsideZipPath,strAirsidePath))
			return;
	}
}

void CImportIntoOldProjectDlg::unzipAirsideExportFiles(const CString oldProjName,const CString& _strPath)
{
	//unzip airside zip
	CString strAirsideZipPath = _T("");
	strAirsideZipPath.Format(_T("%s//%sAirside.zip"),_strPath,oldProjName);
	
	if (PathFileExists((LPCSTR)strAirsideZipPath))
	{
		CString strAirsidePath = _T("");
		strAirsidePath.Format(_T("%s\\Airside"),_strPath);
		if (!PathFileExists((LPCSTR)strAirsidePath))
		{
			if (!CreateDirectory(strAirsidePath,NULL))
			{
				return;
			}
		}
		if (!unzipFiles(strAirsideZipPath,strAirsidePath))
			return;
	}
}

bool CImportIntoOldProjectDlg::CreateTempZipFile(CString& strTempExtractPath)
{
	BeginWaitCursor();
	CString strTempPath = getTempPath();
	strTempPath.TrimRight('\\');
	strTempExtractPath = strTempPath + "\\" + "ARCTempExtractPath";
	FileManager::DeleteDirectory( strTempExtractPath );
	CreateDirectory( strTempExtractPath, NULL );
	// extract zip to temp path
	if( !unzipFiles( m_strSourceFile, strTempExtractPath ) )
	{
		return false;
	}	
	return true;
}

void CImportIntoOldProjectDlg::DeleteTempZipFile( const CString& strTempFilePath )
{
	EndWaitCursor();
	FileManager::DeleteDirectory( strTempFilePath );
}

bool CImportIntoOldProjectDlg::DoExtractInputFiles( const CString& strPath,PROJECTINFO* pi,const CString& strTempFilePath,CString& strOldProjName )
{
	// check if have arcterm_export.ini file
	if( !FileManager::IsFile( strTempFilePath + "\\" + "arcterm_export.ini" ) )  // no ini file( old versin )
	{
		// only copy the temp path to input path
		FileManager::CopyDirectory( strTempFilePath, strPath,NULL );
		FileManager::DeleteDirectory( strTempFilePath );
		//	m_lPrjectDB = 0l;
		CDlgAirportDBNameInput dlg( "Please Enter The Airport Name for the Imported Project:" );
		CString sInput("");
		if(dlg.DoModal()==IDCANCEL)
		{
			return false;
		}
		sInput=dlg.m_sAirportDBName;

		pi->dbname = sInput ;
		pi->version = 300 ;
		EndWaitCursor();
		return true;
	}
	// open ini file
	CString _strInputZip, _strDBZip;
	BOOL bNotForceDB;
	readIniFileData( strTempFilePath, _strInputZip, _strDBZip,bNotForceDB,pi);

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
	//airside export files
	unzipAirsideExportFiles(strOldProjName,strPath);

	//airport files
	unzipAirportFiles(strOldProjName,strPath);

	//landside file
	unzipInputLandsideFile(strOldProjName,strPath);

	if(m_ZipFileVersion > 2)
		UnZipImportDBFiles(strTempFilePath,strOldProjName,strPath);
	if(pi->version <= 300)
	{
		// unzip db file
		if( !unzipDBFiles(  _strDBZip,bNotForceDB?true:false,strTempFilePath,pi,strPath ) )
		{
			return false;
		}
	}
	return true;
}

bool CImportIntoOldProjectDlg::DoExtractEconomicFiles( const CString& strProjectName,const CString& strPath,const CString& strTempFilePath )
{
	CString strTempEconomicFile;
	strTempEconomicFile = strTempFilePath + CString(_T("\\")) + strProjectName + CString(_T("ECONOMIC.zip"));

	return unzipFiles(strTempEconomicFile,strPath);
}