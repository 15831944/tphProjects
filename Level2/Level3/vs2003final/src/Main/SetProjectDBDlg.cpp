// SetProjectDBDlg.cpp : implementation file
//

#include "stdafx.h"
#include "termplan.h"
#include "SetProjectDBDlg.h"
#include "common\airportdatabaselist.h"
#include "common\airportdatabase.h"
#include "assert.h"
#include "../Database/ARCportADODatabase.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#include "../Database/ADODatabase.h"
// include
#include "common\winmsg.h"

/////////////////////////////////////////////////////////////////////////////
// CSetProjectDBDlg dialog


CSetProjectDBDlg::CSetProjectDBDlg(CWnd* pParent /*=NULL*/)
	: CPropertyPage(CSetProjectDBDlg::IDD), m_pAllProjInfo( NULL )
{
	//{{AFX_DATA_INIT(CSetProjectDBDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CSetProjectDBDlg::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSetProjectDBDlg)
	DDX_Control(pDX, IDOK, m_btnSave);
	DDX_Control(pDX, IDC_STATIC_TITLE, m_staticTitle);
	DDX_Control(pDX, IDC_LIST_PROJECT, m_listProject);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSetProjectDBDlg, CPropertyPage)
	//{{AFX_MSG_MAP(CSetProjectDBDlg)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_PROJECT, OnDblclkListProject)
	//}}AFX_MSG_MAP
	ON_MESSAGE( WM_INPLACE_COMBO_KILLFOUCUS, OnInplaceCombox)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSetProjectDBDlg message handlers

int CSetProjectDBDlg::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CPropertyPage::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here
	
	return 0;
}

void CSetProjectDBDlg::OnDestroy() 
{
	CPropertyPage::OnDestroy();
	
}

BOOL CSetProjectDBDlg::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	//////////////////////////////////////////////////////////////////////////
	InitToolBar();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CSetProjectDBDlg::InitToolBar( void )
{

}

void CSetProjectDBDlg::InitListCtrl( void )
{
	// delete all column
	int _iColumnCount = m_listProject.GetHeaderCtrl()->GetItemCount();
	for( int c=0; c< _iColumnCount; c++ )
		m_listProject.DeleteColumn( 0 );

	// set list_ctrl	
	DWORD dwStyle = m_listProject.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_EX_GRIDLINES;
	m_listProject.SetExtendedStyle( dwStyle );
	
	LV_COLUMNEX	lvc;
	lvc.mask = LVCF_WIDTH | LVCF_TEXT ;
	char columnLabel[][128]=
	{
		"Project Name",
		"User",
		"Airport Database"
	};
	
	CRect rcList;
	m_listProject.GetWindowRect(rcList);
	int defaultColumnWidth[] = { rcList.Width()/3, rcList.Width()/6, rcList.Width()/2 };
	int _iFormat[] = {	LVCFMT_CENTER|LVCFMT_NOEDIT , LVCFMT_CENTER | LVCFMT_NOEDIT, LVCFMT_CENTER | LVCFMT_DROPDOWN };
	
	// load all global db
	CAirportDatabase* _pDB;
	CStringList strList;
	for( int k=0; k<AIRPORTDBLIST->getDBCount(); k++ )
	{
		CString _strItem;

		 _pDB= AIRPORTDBLIST->getAirportDBByIndex( k );
		 if(_pDB->getName().CompareNoCase("DEFAULTDB")!=0)
		 {
			 _strItem.Format("%s", _pDB->getName() );
			 strList.InsertAfter( strList.GetTailPosition(), _strItem );
		 }
	}
	
	for( int i=0; i<3; i++ )
	{
		lvc.csList	= &strList;
		lvc.pszText = columnLabel[i];
		lvc.cx		= defaultColumnWidth[i];
		lvc.fmt		= _iFormat[i];
		m_listProject.InsertColumn( i, &lvc );
	}
}

void CSetProjectDBDlg::LoadDataToList( void )
{
	// first clear old data
	m_listProject.DeleteAllItems();

	//////////////////////////////////////////////////////////////////////////
	for( int i=0; i< static_cast<int>(m_pAllProjInfo->size()); i++ )
	{
		if( m_pAllProjInfo->at(i)->version <= 300)
			continue ;
		int _idx = m_listProject.InsertItem( m_listProject.GetItemCount(), m_pAllProjInfo->at(i)->name );

		m_listProject.SetItemText( _idx, 1, m_pAllProjInfo->at(i)->user );

		CString _str;
		_str.Format("%s", m_pAllProjInfo->at(i)->dbname );
		m_listProject.SetItemText( _idx, 2, _str );

		m_listProject.SetItemData( _idx, (long)(m_pAllProjInfo->at(i)) );
	}
}




void CSetProjectDBDlg::OnDblclkListProject(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	int nSubItem = pNMListView->iSubItem;

	if(nSubItem!=2)
	{
		*pResult = 0;
		return;
	}
	if(pNMListView->iItem!=-1 && nSubItem==2)
	{
		CStringList strList;
		long _lItemData = m_listProject.GetItemData( pNMListView->iItem );
		PROJECTINFO* _prj_info = (PROJECTINFO*)_lItemData;
		CAirportDatabase* pDB=AIRPORTDBLIST->getAirportDBByName(_prj_info->dbname);
        if(pDB == NULL)
			return ;
		CString _strItem;
		for(int i=0; i<AIRPORTDBLIST->getDBCount(); i++)
		{
			_strItem.Format("%s",AIRPORTDBLIST->getAirportDBByIndex(i)->getName());
			strList.InsertAfter( strList.GetTailPosition(), _strItem );
		}
		if(pDB->getName().CompareNoCase("DEFAULTDB")!=0)
		{
			POSITION pos=strList.Find("DEFAULTDB");
			if(pos!=NULL)strList.RemoveAt(pos);
		}
		LV_COLUMNEX	lvc;
		lvc.csList	= &strList;
		lvc.pszText = "Airport Database";
		
		lvc.fmt		= LVCFMT_CENTER | LVCFMT_DROPDOWN;
	//	m_listProject.SetColumn( nSubItem, (LVCOLUMN*)&lvc );
		
	}
	*pResult = 0;

}


LRESULT CSetProjectDBDlg::OnInplaceCombox( WPARAM wParam, LPARAM lParam)
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)lParam;
	LV_ITEM* plvItem = &pDispInfo->item;
	
	int _iCurSel = m_listProject.GetCurSel();
	if( _iCurSel<0 || _iCurSel>= m_listProject.GetItemCount() )
		return TRUE;
	
	long _lItemData = m_listProject.GetItemData( _iCurSel );
	PROJECTINFO* _prj_info = (PROJECTINFO*)_lItemData;
	assert( _prj_info );
     _prj_info->m_IsChanged = TRUE ;
	CString _strDBName = plvItem->pszText;
	_prj_info->dbname = _strDBName;
	
	//////////////////////////////////////////////////////////////////////////
	// enable save button
	SetModified();

	return TRUE;
}

void CSetProjectDBDlg::OnOK() 
{
	CWaitCursor wc;
	//re_save all project info file
	for( int i=0; i<static_cast<int>(m_pAllProjInfo->size()); i++ )
	{
		if(m_pAllProjInfo->at(i)->version <= 302 || !m_pAllProjInfo->at(i)->m_IsChanged)
			continue ;
		if(m_pAllProjInfo->at(i)->version <= 302 && m_pAllProjInfo->at(i)->m_IsChanged)
		{
			CString errormsg ;
			errormsg.Format(_T("The version of Project '%s' is too older,please open this project by the lastest application firstly before you change the AirportDB!"),m_pAllProjInfo->at(i)->name) ;
			MessageBox(errormsg,_T("Warninng"),MB_OK) ;
			continue ;
		}
		if(!UpdateAirportDBBNameFOrDataBase(m_pAllProjInfo->at(i)))
			continue ;
		PROJMANAGER->UpdateProjInfoFile( *(m_pAllProjInfo->at(i)) );
	}
	
	CPropertyPage::OnOK();
}
BOOL CSetProjectDBDlg::UpdateAirportDBBNameFOrDataBase(PROJECTINFO* _proInfo)
{
	if(_proInfo == NULL)
		return FALSE;

	//change the data ,load data from airportDB template 
	CAirportDatabase* AIrportDbTemplate = AIRPORTDBLIST->getAirportDBByName(_proInfo->dbname);
	if(AIrportDbTemplate == NULL)
	{
		CString msg ;
		msg.Format(_T("%s AirportDB template is not exist!Copy Data Failed!"),_proInfo->dbname) ;
		MessageBox(msg,_T("Warning"),MB_OK) ;	
	}else
	{
		CString strParts(_T(""));
		CString strARCPath(_T(""));
		strParts.Format(_T("%s\\Input\\parts.mdb"),_proInfo->path);
		strARCPath.Format(_T("%s\\%s"),PROJMANAGER->GetAppPath(),_T("Databases\\arc.mdb"));

		CARCProjectDatabase* airport = new CARCProjectDatabase(strARCPath,strParts);
		AIrportDbTemplate->saveAsTemplateDatabase(airport);
// 		try
// 		{
// 			CDatabaseADOConnetion::BeginTransaction(airport->GetAirportConnection()) ; //begin transaction for local .
// 			airport->deleteDatabase(); //first delete all  data from project db  .
// 			airport->loadFromOtherDatabase(AIrportDbTemplate);
// 			airport->saveDatabase(); //save to local airpor     tDB .
// 			CDatabaseADOConnetion::CommitTransaction(airport->GetAirportConnection()) ;
// 		}
// 		catch (CADOException& e)
// 		{
// 			e.ErrorMessage();
// 			CDatabaseADOConnetion::RollBackTransation(airport->GetAirportConnection()) ;
// 		}
	}
//	DATABASECONNECTION.CloseConnection() ;
	return TRUE;
}
void CSetProjectDBDlg::OnCancel() 
{
	CWaitCursor wc;
	// reload project info
	for( int i=0; i< static_cast<int>(m_pAllProjInfo->size()); i++ )
	{
		PROJMANAGER->GetProjectInfo( m_pAllProjInfo->at(i)->name, m_pAllProjInfo->at(i) );
	}

	// update list ctrl
	//LoadDataToList();
	
	CPropertyPage::OnCancel();
}

BOOL CSetProjectDBDlg::OnSetActive() 
{
	InitListCtrl();
	LoadDataToList();
	
	return CPropertyPage::OnSetActive();
}
