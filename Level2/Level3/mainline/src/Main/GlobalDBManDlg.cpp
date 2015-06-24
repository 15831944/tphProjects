// GlobalDBManDlg.cpp : implementation file
//

#include "stdafx.h"
#include "GlobalDBManDlg.h"

#include "TermPlanDoc.h"
#include "GlobaldbSheet.h"
#include "DlgDBAircraftTypes.h"
#include "DlgDBAirline.h"
#include "DlgDBAirports.h"
#include "DlgProbDist.h"
#include "DlgAirportDBNameInput.h"

#include "common\airportdatabaselist.h"
#include "common\airportdatabase.h"
#include "common\fileman.h"
#include "common\winmsg.h"

#include <assert.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define DefaultAirportDBFolder "Databases"
#define CustomerAirportDBFolder "ImportDB"

static const CString _strACFileName		= "acdata.acd";
static const CString _strACCatFileName		= "category.cat";
static const CString _strAirlineFileName	= "Airlines.dat";
static const CString _strSubAirlineFileName	= "subairlines.sal";
static const CString _strAirportFileName	= "Airports.dat";
static const CString _strSectorFileName		= "Sector.sec";
static const CString _strProbDistFileName	= "probdist.pbd";
/////////////////////////////////////////////////////////////////////////////
// CGlobalDBManDlg dialog


CGlobalDBManDlg::CGlobalDBManDlg(CWnd* pParent /*=NULL*/)
	: CPropertyPage(CGlobalDBManDlg::IDD), m_pAllProjInfo(NULL)
{
	//{{AFX_DATA_INIT(CGlobalDBManDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CGlobalDBManDlg::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGlobalDBManDlg)
	DDX_Control(pDX, IDOK, m_btnSave);
	DDX_Control(pDX, IDC_STATIC_TITLE, m_staticTitle);
	DDX_Control(pDX, IDC_LIST_GLOBALDB, m_listDBMan);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CGlobalDBManDlg, CPropertyPage)
	//{{AFX_MSG_MAP(CGlobalDBManDlg)
	ON_WM_CREATE()
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_GLOBALDB, OnItemchangedListGlobaldb)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_GLOBALDB, OnDblclkListGlobaldb)
	ON_COMMAND(IDC_GLOBALDBMAN_COPY, OnGlobaldbmanCopy)
	ON_COMMAND(IDC_GLOBALDBMAN_DELETE, OnGlobaldbmanDelete)
	ON_COMMAND(IDC_GLOBALDBMAN_AC, OnGlobaldbmanAc)
	ON_COMMAND(IDC_GLOBALDBMAN_Airline, OnGLOBALDBMANAirline)
	ON_COMMAND(IDC_GLOBALDBMAN_AIRPORT, OnGlobaldbmanAirport)
	ON_COMMAND(IDC_GLOBALDBMAN_PROBDIST, OnGlobaldbmanProbdist)
	ON_WM_DESTROY()
	ON_COMMAND(IDC_GLOBALDBMAN_NEW, OnGlobaldbmanNew)
	ON_COMMAND(IDC_GLOBALDBMAN_EDIT, OnGlobaldbmanEdit)
	//}}AFX_MSG_MAP
	ON_MESSAGE( WM_INPLACE_EDIT, OnInplaceEdit)
	ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTW, 0, 0xFFFF, OnToolTipText)
	ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTA, 0, 0xFFFF, OnToolTipText)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGlobalDBManDlg message handlers


int CGlobalDBManDlg::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CPropertyPage::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	if (!m_ToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_TOOLTIPS ) ||
		!m_ToolBar.LoadToolBar(IDR_TOOLBAR_GLOBALDBMAN))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}		
	return 0;
}


void CGlobalDBManDlg::OnDestroy() 
{
	CPropertyPage::OnDestroy();
	
}


BOOL CGlobalDBManDlg::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	//////////////////////////////////////////////////////////////////////////
	InitToolBar();
	InitListCtrl();
	
	LoadDataToList();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CGlobalDBManDlg::InitToolBar( void )
{
	CRect mRect;
	m_staticTitle.GetWindowRect( &mRect );
	ScreenToClient( &mRect );
	
	m_ToolBar.MoveWindow( &mRect );
	m_ToolBar.ShowWindow( SW_SHOW );
	m_staticTitle.ShowWindow( SW_HIDE );
	
	EnableAllToolBarButton( FALSE );
}

void CGlobalDBManDlg::EnableAllToolBarButton( BOOL _bEnable )
{
	m_ToolBar.GetToolBarCtrl().EnableButton( IDC_GLOBALDBMAN_NEW, TRUE );
	m_ToolBar.GetToolBarCtrl().EnableButton( IDC_GLOBALDBMAN_EDIT, _bEnable );
	m_ToolBar.GetToolBarCtrl().EnableButton( IDC_GLOBALDBMAN_DELETE, _bEnable );

	m_ToolBar.GetToolBarCtrl().EnableButton( IDC_GLOBALDBMAN_AC, _bEnable );
	m_ToolBar.GetToolBarCtrl().EnableButton( IDC_GLOBALDBMAN_Airline, _bEnable );
	m_ToolBar.GetToolBarCtrl().EnableButton( IDC_GLOBALDBMAN_AIRPORT, _bEnable );
	m_ToolBar.GetToolBarCtrl().EnableButton( IDC_GLOBALDBMAN_PROBDIST, _bEnable );
}

void CGlobalDBManDlg::InitListCtrl( void )
{
	// first delete old data
	m_listDBMan.DeleteAllItems();

	// set list_ctrl	
	DWORD dwStyle = m_listDBMan.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_EX_GRIDLINES;
	m_listDBMan.SetExtendedStyle( dwStyle );
	
	LV_COLUMNEX	lvc;
	lvc.mask = LVCF_WIDTH | LVCF_TEXT ;
	char columnLabel[][128]=
	{
		"ID",
		"Airport Name",
//		"Folder"
	};

	CRect rcList;
	m_listDBMan.GetWindowRect(rcList);
	int defaultColumnWidth[] = { rcList.Width()/6, rcList.Width()/3/*, rcList.Width()/2*/ };
	int _iFormat[] = {	LVCFMT_CENTER|LVCFMT_NOEDIT , LVCFMT_CENTER | LVCFMT_EDIT, LVCFMT_CENTER | LVCFMT_NOEDIT };
	
	CStringList strList;

//	for( int i=0; i<3; i++ )
	for( int i=0; i<2; i++ )
	{
		lvc.csList	= &strList;
		lvc.pszText = columnLabel[i];
		lvc.cx		= defaultColumnWidth[i];
		lvc.fmt		= _iFormat[i];
		m_listDBMan.InsertColumn( i, &lvc );
	}	
}

void CGlobalDBManDlg::LoadDataToList( void )
{	
	// first delete old data
	m_listDBMan.DeleteAllItems();
	//
	CString _strItem;
	int		_iIdx;
	int _iCount = AIRPORTDBLIST->getDBCount();
	for( int i=0; i< _iCount; i++ )
	{
		CAirportDatabase* _pDB = AIRPORTDBLIST->getAirportDBByIndex( i );

		if(_pDB->getName().CompareNoCase("DEFAULT DB")!=0)
		{
			_strItem.Format("%ld", i+1 );
			_iIdx = m_listDBMan.InsertItem( m_listDBMan.GetItemCount(), _strItem );
			
			m_listDBMan.SetItemText( _iIdx, 1, _pDB->getName() );
			
			//		m_listDBMan.SetItemText( _iIdx, 2, _pDB->getFolder() );
			
			m_listDBMan.SetItemData( _iIdx, (long) _pDB );
		}
		
	}
}

void CGlobalDBManDlg::OnItemchangedListGlobaldb(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here
	int _iCurSel = m_listDBMan.GetCurSel();
	if( _iCurSel<0 || _iCurSel>=m_listDBMan.GetItemCount() )
		EnableAllToolBarButton( FALSE );
	else
		EnableAllToolBarButton( TRUE );

	*pResult = 0;
}


// new a global db
void CGlobalDBManDlg::OnGlobaldbmanNew() 
{
/*
	// pop directroy dialog
	CString _strNewDBDirectory;
	if( !SelectDirectory("Select Database Directory...", _strNewDBDirectory ) )
		return;
*/
	CDlgAirportDBNameInput dlg( "Please Enter The Airport Name:" ,false );
	if(dlg.DoModal()==IDCANCEL)
	{
		return;
	}
	CString sNewDBName=dlg.m_sAirportDBName1;

	int nDBCount= AIRPORTDBLIST->getDBCount();
	for(int i=0;i<nDBCount; i++)
	{
		if(sNewDBName.CompareNoCase( AIRPORTDBLIST->getAirportDBByIndex(i)->getName())==0)
		{
			AfxMessageBox("This Airport Database has existed!");
			return;
		}
	}
	// add a new db to global db list
//	sNewDBName=AIRPORTDBLIST->getUniqueName("New DB");
	//CString sFullDBPath=CreateNewAirportDBFolder(sNewDBName);

	//long _lNewDBID = AIRPORTDBLIST->addNewDBEntry( sNewDBName, sFullDBPath, false );
	CAirportDatabase * _pNewDB = AIRPORTDBLIST->addNewDBEntry( sNewDBName );
	//AIRPORTDBLIST->CopyDefaultDB(sNewDBName);

	assert( _pNewDB );

	// update list control
	CString _strItem;
	_strItem.Format("%ld", AIRPORTDBLIST->getDBCount() );
	int _iIdx = m_listDBMan.InsertItem( m_listDBMan.GetItemCount(), _strItem );
	
	m_listDBMan.SetItemText( _iIdx, 1, _pNewDB->getName() );

	
//	m_listDBMan.SetItemText( _iIdx, 2, _pNewDB->getFolder() );
	
	m_listDBMan.SetItemData( _iIdx, (long) _pNewDB );

	SetModified();
}

// modify name
LRESULT CGlobalDBManDlg::OnInplaceEdit(WPARAM wParam, LPARAM lParam)
{
	int _iCurSel = m_listDBMan.GetCurSel();
	if( _iCurSel<0 || _iCurSel>= m_listDBMan.GetItemCount() )
		return TRUE;
	
	CString strNewName = *((CString*) lParam);
	long _lItemData = m_listDBMan.GetItemData( _iCurSel );
	CAirportDatabase* _pDB = (CAirportDatabase*) _lItemData;
	assert( _pDB );

	if(strNewName.CompareNoCase(_pDB->getName())==0)return TRUE;
	if(RenameAirportDB(_pDB,strNewName)==TRUE)
	{
		m_listDBMan.SetItemText( _iCurSel, 1, _pDB->getName() );
	}
	else
	{
//		m_listDBMan.SetItemText( _iCurSel, 2, _pDB->getFolder() );
	}
	return TRUE;
}

// modify folder
void CGlobalDBManDlg::OnDblclkListGlobaldb(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// get hit's sub item
	CPoint pt = ::GetMessagePos();
	m_listDBMan.ScreenToClient( &pt);

	LVHITTESTINFO testinfo;
	testinfo.pt = pt;
	LRESULT result = m_listDBMan.SendMessage( LVM_SUBITEMHITTEST, 0,(LPARAM)(LVHITTESTINFO FAR *)&testinfo );

	// not hits on item
	if( result == -1 )
	{
		*pResult = 0;
		return;
	}
/*

	// if hit on "folder" sub_item
	if( testinfo.iSubItem == 2 )	
	{
		CString _strNewPath;
		if( SelectDirectory( " Select Global DB Folder...", _strNewPath) )
		{
			long _lItemData = m_listDBMan.GetItemData( testinfo.iItem );
			CAirportDatabase* _pDB = ( CAirportDatabase* )_lItemData;
			assert( _pDB );

			// modify db
			_pDB->setFolder( _strNewPath );
			// update list ctrl
			m_listDBMan.SetItemText( testinfo.iItem, testinfo.iSubItem, _strNewPath );
			//////////////////////////////////////////////////////////////////////////
			// enable save button
			SetModified();
		}
	}
*/
	
	*pResult = 0;
}

//pop folder selection, return true ok, return false cancel
bool CGlobalDBManDlg::SelectDirectory( const CString& _strTitle,CString& _strPath )
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
        bi.lpfn           = BrowseCallbackProc;
		
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

//////////////////////////////////////////////////////////////////////////
// copy db
void CGlobalDBManDlg::OnGlobaldbmanCopy() 
{
	
}


//////////////////////////////////////////////////////////////////////////
// delete db
void CGlobalDBManDlg::OnGlobaldbmanDelete() 
{
	int _iCurSel = m_listDBMan.GetCurSel();
	if( _iCurSel<0 || _iCurSel>= m_listDBMan.GetItemCount() )
		return;

	long _lItemData = m_listDBMan.GetItemData( _iCurSel );
	CAirportDatabase* _pDB = (CAirportDatabase*)_lItemData;
	assert( _pDB );
	
	// this is default db
	if( _pDB->getName() == "DEFAULTDB" )
	{
		AfxMessageBox("The database is default DB, so can't delete it", MB_OK|MB_ICONINFORMATION );
		return;
	}
	AIRPORTDBLIST->RemoveAirportDB(_pDB) ;
	LoadDataToList() ;
	SetModified();
}

// AC 
void CGlobalDBManDlg::OnGlobaldbmanAc() 
{
	int _iCurSel = m_listDBMan.GetCurSel();
	if( _iCurSel<0 || _iCurSel>= m_listDBMan.GetItemCount() )
		return;
	
	long _lItemData = m_listDBMan.GetItemData( _iCurSel );
	CAirportDatabase* _pDB = (CAirportDatabase*)_lItemData;
	ASSERT( _pDB );
	
	if( !_pDB->hadLoadDatabase() )
	{
	//	ValidateDBFile(_pDB->getFolder());
		_pDB->loadDatabase();
	}

	CDlgDBAircraftTypesForDefault dlg(TRUE, NULL ,_pDB);
	dlg.setAcManager( _pDB->getAcMan() );
	dlg.DoModal();	
}

void CGlobalDBManDlg::OnGLOBALDBMANAirline() 
{
	int _iCurSel = m_listDBMan.GetCurSel();
	if( _iCurSel<0 || _iCurSel>= m_listDBMan.GetItemCount() )
		return;
	
	long _lItemData = m_listDBMan.GetItemData( _iCurSel );
	CAirportDatabase* _pDB = (CAirportDatabase*)_lItemData;
	ASSERT( _pDB );
	
	if( !_pDB->hadLoadDatabase() )
	{
//		ValidateDBFile(_pDB->getFolder());
		_pDB->loadDatabase();
	}

 	CDlgDBAirlineForDefault dlg( TRUE,NULL,_pDB);
 	dlg.setAirlinesMan( _pDB->getAirlineMan() );
 	dlg.DoModal();
}

void CGlobalDBManDlg::OnGlobaldbmanAirport() 
{
	int _iCurSel = m_listDBMan.GetCurSel();
	if( _iCurSel<0 || _iCurSel>= m_listDBMan.GetItemCount() )
		return;
	
	long _lItemData = m_listDBMan.GetItemData( _iCurSel );
	CAirportDatabase* _pDB = (CAirportDatabase*)_lItemData;
	ASSERT( _pDB );
	
	if( !_pDB->hadLoadDatabase() )
	{
//		ValidateDBFile(_pDB->getFolder());
		_pDB->loadDatabase();
	}
	
	CDlgDBAirportsForDefault dlg(TRUE,NULL,_pDB);
	dlg.setAirportMan( _pDB->getAirportMan() );
	dlg.DoModal();	
}

void CGlobalDBManDlg::OnGlobaldbmanProbdist() 
{
	int _iCurSel = m_listDBMan.GetCurSel();
	if( _iCurSel<0 || _iCurSel>= m_listDBMan.GetItemCount() )
		return;
	
	long _lItemData = m_listDBMan.GetItemData( _iCurSel );
	CAirportDatabase* _pDB = (CAirportDatabase*)_lItemData;
	ASSERT( _pDB );
	
	if( !_pDB->hadLoadDatabase() )
	{
//		ValidateDBFile(_pDB->getFolder());
		_pDB->loadDatabase();
	}
	
	CDlgProbDistForDefaultDB dlg(_pDB, false,_pDB);
	//dlg.setProbDistMan( _pDB->getProbDistMan() );
	dlg.DoModal();
}


// before delete DB, check foreign key
bool CGlobalDBManDlg::CheckForeignKey( const CAirportDatabase* _pDelDB )
{
	long _lDeleteIndex = _pDelDB->getIndex();
	std::vector< CString > _relatePrjName;
	for( int i=0; i< static_cast<int>(m_pAllProjInfo->size()); i++  )
	{
		if( m_pAllProjInfo->at(i)->lUniqueIndex == _lDeleteIndex )
			_relatePrjName.push_back( m_pAllProjInfo->at(i)->name );
	}

	if( _relatePrjName.size()== 0 )
		return true;

	CString _strMessage;
	_strMessage.Format("The db index %ld had been used in following project:\r\n", _lDeleteIndex );
	for( int k=0; k<static_cast<int>(_relatePrjName.size()); k++ )
	{
		_strMessage += "\t" + _relatePrjName[k] +"\r\n";
	}
	_strMessage +="are you sure to delete the db?";

	int _iReturn = AfxMessageBox( _strMessage, MB_YESNO |MB_ICONQUESTION );
	if( _iReturn == IDYES )
		return true;
	else//( _iReturn == IDNO )
		return false;

}

void CGlobalDBManDlg::OnOK() 
{
	// save Global DB list to file
	CWaitCursor wc;

	AIRPORTDBLIST->saveDataSet(CString(_T("")),false);
	
	CPropertyPage::OnOK();
}

void CGlobalDBManDlg::OnCancel() 
{
	CWaitCursor wc;
	
	AIRPORTDBLIST->loadDataSet(CString(_T("")));
	//LoadDataToList();

	CPropertyPage::OnCancel();
}


BOOL CGlobalDBManDlg::OnToolTipText(UINT, NMHDR* pNMHDR, LRESULT* pResult)
{
	ASSERT(pNMHDR->code == TTN_NEEDTEXTA || pNMHDR->code == TTN_NEEDTEXTW);
	
    // if there is a top level routing frame then let it handle the message
    //if (GetRoutingFrame() != NULL) return FALSE;
	
    // to be thorough we will need to handle UNICODE versions of the message also !!
    TOOLTIPTEXTA* pTTTA = (TOOLTIPTEXTA*)pNMHDR;
    TOOLTIPTEXTW* pTTTW = (TOOLTIPTEXTW*)pNMHDR;
    TCHAR szFullText[512];
    CString strTipText;
    UINT nID = pNMHDR->idFrom;
	
    if (pNMHDR->code == TTN_NEEDTEXTA && (pTTTA->uFlags & TTF_IDISHWND) ||
        pNMHDR->code == TTN_NEEDTEXTW && (pTTTW->uFlags & TTF_IDISHWND))
    {
        // idFrom is actually the HWND of the tool 
        nID = ::GetDlgCtrlID((HWND)nID);
    }
	
    if (nID != 0) // will be zero on a separator
    {
		AfxLoadString(nID, szFullText);
        strTipText=szFullText;
		
#ifndef _UNICODE
        if (pNMHDR->code == TTN_NEEDTEXTA)
        {
            lstrcpyn(pTTTA->szText, strTipText, sizeof(pTTTA->szText));
        }
        else
        {
            _mbstowcsz(pTTTW->szText, strTipText, sizeof(pTTTW->szText));
        }
#else
        if (pNMHDR->code == TTN_NEEDTEXTA)
        {
            _wcstombsz(pTTTA->szText, strTipText,sizeof(pTTTA->szText));
        }
        else
        {
            lstrcpyn(pTTTW->szText, strTipText, sizeof(pTTTW->szText));
        }
#endif
		
        *pResult = 0;
		
        // bring the tooltip window above other popup windows
        ::SetWindowPos(pNMHDR->hwndFrom, HWND_TOP, 0, 0, 0, 0,
            SWP_NOACTIVATE|SWP_NOSIZE|SWP_NOMOVE|SWP_NOOWNERZORDER);
        
        return TRUE;
    }
	
    return FALSE;
}


int CALLBACK BrowseCallbackProc( HWND hwnd,UINT uMsg,LPARAM lp, LPARAM pData )
{
	TCHAR szActiveDir[MAX_PATH];
	
	switch( uMsg )
	{
	case BFFM_INITIALIZED:
		{
			strcpy( szActiveDir, PROJMANAGER->GetAppPath() );
			SendMessage( hwnd, BFFM_SETSELECTION, (WPARAM)TRUE, (LPARAM)szActiveDir );
			break;
		}
	default:
		{
			break;
		}
	}
	
	return 0;
}
//validate airportdatabase file if existed, 
//and copy default file to supersede the loss file.
//you'd better call it before airportdatabase::loadDatabase()
void CGlobalDBManDlg::ValidateDBFile(const CString& sDestPath)
{
	TCHAR sDefaultAirportDBPath[MAX_PATH];
	sprintf(sDefaultAirportDBPath,"%s\\%s",PROJMANAGER->GetAppPath(),DefaultAirportDBFolder);
	CopyAirportDBFile(sDefaultAirportDBPath,sDestPath);
}
//copy airportdb file from source path into destpath
void CGlobalDBManDlg::CopyAirportDBFile(LPCSTR sSourcesDBPath, LPCSTR sDestDBPath)
{
	LPCSTR sNeedCheckFile[]=
	{
		_strACFileName,
			_strACCatFileName,
			_strAirlineFileName,
			_strSubAirlineFileName,
			_strAirportFileName,
			_strSectorFileName,
			_strProbDistFileName
	};
	TCHAR sDestFullFileName[MAX_PATH]={_T('\0')};
	TCHAR sSourceFullFileName[MAX_PATH]={_T('\0')};
	
	int nCount=sizeof(sNeedCheckFile)/sizeof(LPCSTR);
	for(int i=0;i<nCount;i++)
	{
		sprintf(sDestFullFileName,"%s\\%s",sDestDBPath,sNeedCheckFile[i]);
		if(	FileManager::IsFile(sDestFullFileName)==0)
		{
			sprintf(sSourceFullFileName,"%s\\%s",sSourcesDBPath, sNeedCheckFile[i]);
			FileManager::CopyFile(sSourceFullFileName,sDestFullFileName,0);
		}
	}
}

BOOL CGlobalDBManDlg::CheckDBIsOpening(const CString &sDbName,CString& sReturnProjectName)
{
	CWinApp* pApp=AfxGetApp();
	POSITION posTemplate=pApp->GetFirstDocTemplatePosition( );
	POSITION posDocument=NULL;
	CDocTemplate* pDocTemplate=NULL;
	CDocument* pDoc=NULL;
	CTermPlanDoc* pTermPlanDoc=NULL;

	while(posTemplate!=NULL)
	{
		pDocTemplate=pApp->GetNextDocTemplate(posTemplate);
		posDocument=pDocTemplate->GetFirstDocPosition();
		while(posDocument != NULL)
		{
			pDoc=pDocTemplate->GetNextDoc(posDocument);
			if( pDoc->IsKindOf(RUNTIME_CLASS(CTermPlanDoc)) )
			{
				pTermPlanDoc=(CTermPlanDoc*)pDoc;
				if( sDbName.CompareNoCase(pTermPlanDoc->GetTerminal().m_pAirportDB->getName())==0 )
				{
					sReturnProjectName=pTermPlanDoc->m_ProjInfo.name;
					return TRUE;
				}

			}

		}
		
	}
	return FALSE;
}

CString CGlobalDBManDlg::CreateNewAirportDBFolder(const CString &sDBName)
{
	CString sFullDBPath("");
	sFullDBPath.Format("%s\\%s\\%s",PROJMANAGER->GetAppPath(),CustomerAirportDBFolder,sDBName);
	FileManager::MakePath(sFullDBPath);		

	return sFullDBPath;
}

BOOL CGlobalDBManDlg::RenameAirportDB(CAirportDatabase *pDB, LPCSTR szNewName)
{
	CAirportDatabase* _pDB = NULL ;
	for ( int i = 0 ; i < (int)AIRPORTDBLIST->getDBCount() ; ++i)
	{
		 _pDB = AIRPORTDBLIST->getAirportDBByIndex( i );
		 if(_pDB->getName().CompareNoCase(szNewName) == 0)
		 {
			 CString _strMsg ;
			 _strMsg.Format("The name: %s has already exist!", szNewName );
			 AfxMessageBox( _strMsg, MB_OK|MB_ICONINFORMATION );
			 return FALSE ;
		 }
	}
	pDB->setName(szNewName) ;
	SetModified();
	return TRUE;
	
}



void CGlobalDBManDlg::OnGlobaldbmanEdit() 
{
	int nSel=m_listDBMan.GetCurSel();
	if(nSel==-1)return;
	CAirportDatabase* pDB=(CAirportDatabase*)m_listDBMan.GetItemData(nSel);
	ASSERT(pDB!=NULL);
	CDlgAirportDBNameInput dlg("Please Enter The Airport Name:",false);
	if(dlg.DoModal()==IDOK)
	{
		CString strNewName=dlg.m_sAirportDBName1;

		if(strNewName.CompareNoCase(pDB->getName())==0)return;
		if(RenameAirportDB(pDB,strNewName)==TRUE)
		{
			m_listDBMan.SetItemText( nSel, 1, pDB->getName() );
		}
	}
	
}
