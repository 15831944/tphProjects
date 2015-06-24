// MoveSideWalkPaxDistDlg.cpp : implementation file
//

#include "stdafx.h"
#include "termplan.h"
#include "MoveSideWalkPaxDistDlg.h"
#include "inputs\pax_cnst.h"
#include "inputs\probab.h"
#include "TermPlanDoc.h"
#include "..\common\probdistmanager.h"
#include "..\inputs\SideMoveWalkDataSet.h"
#include "..\inputs\SideMoveWalkPaxDist.h"
#include "PassengerTypeDialog.h"
#include <Common/ProbabilityDistribution.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMoveSideWalkPaxDistDlg property page

IMPLEMENT_DYNCREATE(CMoveSideWalkPaxDistDlg, CPropertyPage)


CMoveSideWalkPaxDistDlg::CMoveSideWalkPaxDistDlg( CWnd* pView ) : CPropertyPage(CMoveSideWalkPaxDistDlg::IDD)
{
	m_pView = pView ;

	m_treeProc.setInputTerminal( GetInputTerminal() );
}

CMoveSideWalkPaxDistDlg::~CMoveSideWalkPaxDistDlg()
{
	delete m_pDefaultEntry;
}

void CMoveSideWalkPaxDistDlg::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMoveSideWalkPaxDistDlg)
	DDX_Control(pDX, IDC_STATIC_TOOLBARCONTENTER, m_staticTool);
	DDX_Control(pDX, IDC_BUTTON_NEW, m_btnNew);
	DDX_Control(pDX, IDC_BUTTON_DEL, m_btnDel);
	DDX_Control(pDX, IDC_LIST_PAX, m_listPax);
	DDX_Control(pDX, IDC_MOVINGSIDE_TREE, m_treeProc);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMoveSideWalkPaxDistDlg, CPropertyPage)
	//{{AFX_MSG_MAP(CMoveSideWalkPaxDistDlg)
	ON_WM_CREATE()
	ON_BN_CLICKED(IDC_BUTTON_NEW, OnButtonNew)
	ON_BN_CLICKED(IDC_BUTTON_DEL, OnButtonDel)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_PAX, OnItemchangedListPax)
	ON_NOTIFY(TVN_SELCHANGED, IDC_MOVINGSIDE_TREE, OnSelchangedMovingsideTree)
	ON_COMMAND(ID_PEOPLEMOVER_NEW, OnToolBarButtonNew)
	ON_COMMAND(ID_PEOPLEMOVER_DELETE, OnToolBarButtonDelete)
	ON_NOTIFY(LVN_ENDLABELEDIT, IDC_LIST_PAX, OnEndlabeleditListPax)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMoveSideWalkPaxDistDlg message handlers

BOOL CMoveSideWalkPaxDistDlg::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

	CMobileElemConstraint* pPaxConstr = new CMobileElemConstraint(GetInputTerminal());
	pPaxConstr->initDefault();
	ConstantDistribution* pDist = new ConstantDistribution( 0 );

	m_pDefaultEntry = new ConstraintEntry();
	m_pDefaultEntry->initialize( pPaxConstr, pDist );
	m_treeProc.m_bDisplayAll=TRUE;
	InitToolbar();
	ReloadDatabase();
	SetListCtrl();
	ReloadListData(NULL);

	m_listPax.DeleteAllItems();
	HTREEITEM hItem = m_treeProc.GetSelectedItem();
	if( hItem == NULL )
	{
		m_btnNew.EnableWindow(FALSE);
		m_btnDel.EnableWindow(FALSE);
		m_ToolBar.GetToolBarCtrl().EnableButton( ID_PEOPLEMOVER_DELETE,FALSE );
		m_ToolBar.GetToolBarCtrl().EnableButton(ID_PEOPLEMOVER_NEW,FALSE );
		return TRUE;
	}
	int nDBIdx = m_treeProc.GetItemData( hItem );
	if(nDBIdx == -1)
	{
		m_btnNew.EnableWindow(TRUE);
		m_btnDel.EnableWindow(FALSE);
		m_ToolBar.GetToolBarCtrl().EnableButton( ID_PEOPLEMOVER_DELETE,FALSE );
		m_ToolBar.GetToolBarCtrl().EnableButton(ID_PEOPLEMOVER_NEW,FALSE );
	}
	else
	{
		m_btnNew.EnableWindow(FALSE);
		m_btnDel.EnableWindow(TRUE);
		m_ToolBar.GetToolBarCtrl().EnableButton( ID_PEOPLEMOVER_DELETE,GetSelectedListItem() != -1 );
		m_ToolBar.GetToolBarCtrl().EnableButton( ID_PEOPLEMOVER_NEW,TRUE);	
	}
	return TRUE;  // return TRUE unless you set the focus to a control
	
}

void CMoveSideWalkPaxDistDlg::InitToolbar()
{
	CRect rc;
	m_staticTool.GetWindowRect( &rc );
	ScreenToClient(&rc);
	m_ToolBar.MoveWindow(&rc);
	m_ToolBar.ShowWindow(SW_SHOW);
	m_staticTool.ShowWindow(SW_HIDE);
	m_ToolBar.GetToolBarCtrl().EnableButton( ID_PEOPLEMOVER_NEW );
	m_ToolBar.GetToolBarCtrl().HideButton( ID_PEOPLEMOVER_CHANGE );
	m_ToolBar.GetToolBarCtrl().EnableButton( ID_PEOPLEMOVER_DELETE,FALSE );
}

int CMoveSideWalkPaxDistDlg::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CPropertyPage::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	if (!m_ToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_ToolBar.LoadToolBar(IDR_PEOPLEMOVEBAR))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}
	
	return 0;
}

void CMoveSideWalkPaxDistDlg::ReloadDatabase()
{
	m_treeProc.LoadData( GetInputTerminal(), (ProcessorDatabase*)GetInputTerminal()->m_pMovingSideWalkPaxDist, MovingSideWalkProc );
}

InputTerminal* CMoveSideWalkPaxDistDlg::GetInputTerminal()
{
	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pView)->GetDocument();	
    return (InputTerminal*)&pDoc->GetTerminal();
}
void CMoveSideWalkPaxDistDlg::SetListCtrl()
{
	DWORD dwStyle = m_listPax.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_EX_GRIDLINES;
	m_listPax.SetExtendedStyle( dwStyle );

	LV_COLUMNEX	lvc;
	lvc.mask = LVCF_WIDTH | LVCF_TEXT ;
	char columnLabel[2][128];
	sprintf( columnLabel[1], "Probability Distribution (%%)" );
	CString csLabel;
	strcpy( columnLabel[0], "Passenger Type" );		
	int DefaultColumnWidth[] = { 193, 225 };
	int nFormat[] = {	LVCFMT_CENTER, LVCFMT_CENTER };
	for( int i=0; i<2; i++ )
	{
		CStringList strList;
		CString s;
		s.LoadString(IDS_STRING_NEWDIST);
		strList.InsertAfter( strList.GetTailPosition(), s );
		int nCount = _g_GetActiveProbMan( GetInputTerminal() )->getCount();
		for( int m=0; m<nCount; m++ )
		{
			CProbDistEntry* pPBEntry = _g_GetActiveProbMan( GetInputTerminal() )->getItem( m );			
			strList.InsertAfter( strList.GetTailPosition(), pPBEntry->m_csName );
		}
		lvc.csList = &strList;
		lvc.pszText = columnLabel[i];
		lvc.cx = DefaultColumnWidth[i];
		if( i == 0 )
			lvc.fmt = LVCFMT_NOEDIT;
		else
			lvc.fmt = LVCFMT_DROPDOWN;
		m_listPax.InsertColumn( i, &lvc );
	}
	m_listPax.SetInputTerminal( GetInputTerminal() );
}

ConstraintDatabase* CMoveSideWalkPaxDistDlg::GetConstraintDatabase( bool& _bIsOwn )
{
		HTREEITEM hSelItem = m_treeProc.GetSelectedItem();
	if( hSelItem == NULL )
		return NULL;
	int nDBIdx = m_treeProc.GetItemData( hSelItem );
	if( nDBIdx != -1 )
		_bIsOwn = true;
	else
		_bIsOwn = false;
	while( nDBIdx == -1 && hSelItem != NULL ) 
	{
		hSelItem = m_treeProc.GetParentItem( hSelItem );
		if( hSelItem == NULL )
			break;
		nDBIdx = m_treeProc.GetItemData( hSelItem );
	}
	if(nDBIdx == -1)
	{
		return NULL;
	}
	return GetInputTerminal()->m_pMovingSideWalkPaxDist->getDatabase(nDBIdx);
	
}
void CMoveSideWalkPaxDistDlg::ReloadListData( Constraint* _pSelCon )
{
	bool bIsOwn;
	ConstraintDatabase* pConDB = GetConstraintDatabase( bIsOwn );
	if(pConDB==NULL)
	{
		m_listPax.DeleteAllItems();
		const Constraint* pCon = m_pDefaultEntry->getConstraint();
		//char szName[256];
		CString szName;
		pCon->screenPrint( szName, 0, 256 );
		const ProbabilityDistribution* pProbDist = m_pDefaultEntry->getValue();
		char szDist[1024] = {0};

		//CString szDist;
		if(pProbDist)
			pProbDist->screenPrint( szDist );
		int nIdx = m_listPax.InsertItem( 0, szName );
		m_listPax.SetItemData( nIdx, -1 );
		m_listPax.SetItemText( nIdx, 1, szDist );

	}
	else
	{
		
		m_listPax.SetCurConDB( pConDB, bIsOwn?0:-1 );
		m_listPax.ReloadData( _pSelCon );
	}

}

int CMoveSideWalkPaxDistDlg::GetSelectedListItem()
{
	int nCount = m_listPax.GetItemCount();
	for( int i=0; i<nCount; i++ )
	{
		if( m_listPax.GetItemState( i, LVIS_SELECTED ) == LVIS_SELECTED )
			return i;
	}
	return -1;
}

void CMoveSideWalkPaxDistDlg::OnButtonNew() 
{	

	bool bDirty = false;
	HTREEITEM hItem = m_treeProc.GetSelectedItem();
	if( hItem == NULL )
		return ;
	int nDBIdx = m_treeProc.GetItemData( hItem );
	if(nDBIdx == -1)
	{
		CPassengerTypeDialog dlg( m_pView );
		if (dlg.DoModal() == IDOK)
		{
			m_listPax.DeleteAllItems();
			
			ProcessorID id;
			CString str = m_treeProc.GetItemText( hItem );
			id.SetStrDict( GetInputTerminal()->inStrDict );
			id.setID( str );
			
			// insert into database
			
			//PassengerConstraint* pPaxConstr = new PassengerConstraint();
			CMobileElemConstraint* pMobileConstr = new CMobileElemConstraint(GetInputTerminal());
			*pMobileConstr = dlg.GetMobileSelection();
			ConstantDistribution* pDist = new ConstantDistribution( 0 );
			
			ConstraintEntry* pNewEntry = new ConstraintEntry();
			pNewEntry->initialize( pMobileConstr, pDist );
			
			GetInputTerminal()->m_pMovingSideWalkPaxDist->insert( id, pNewEntry );
			
			m_treeProc.ResetTreeData( NULL );
			
			bool bIsOwn;
			ConstraintDatabase* pConDB = GetConstraintDatabase( bIsOwn );
			
			m_listPax.SetCurConDB( pConDB, bIsOwn?0:-1 );
			m_listPax.ReloadData( NULL );
			bDirty = TRUE;
		}
		
	}

	// Refresh the state 
	hItem = m_treeProc.GetSelectedItem();
	if( hItem == NULL )
		return ;
	nDBIdx = m_treeProc.GetItemData( hItem );
	if(nDBIdx == -1)
	{
		m_btnNew.EnableWindow(TRUE);
		m_btnDel.EnableWindow(FALSE);
		m_ToolBar.GetToolBarCtrl().EnableButton( ID_PEOPLEMOVER_DELETE,FALSE );
		m_ToolBar.GetToolBarCtrl().EnableButton(ID_PEOPLEMOVER_NEW,FALSE );
	}
	else
	{
		m_btnNew.EnableWindow(FALSE);
		m_btnDel.EnableWindow(TRUE);
		m_ToolBar.GetToolBarCtrl().EnableButton( ID_PEOPLEMOVER_DELETE,GetSelectedListItem() != -1 );
		m_ToolBar.GetToolBarCtrl().EnableButton( ID_PEOPLEMOVER_NEW,TRUE);	
	}

	SetModified();
}

void CMoveSideWalkPaxDistDlg::OnButtonDel() 
{
	HTREEITEM hSelItem = m_treeProc.GetSelectedItem();
	if( hSelItem == NULL )
		return;

	int nDBIdx = m_treeProc.GetItemData( hSelItem );

	if( nDBIdx == -1 )
		return;

	m_treeProc.m_strSelectedID=m_treeProc.GetItemText(hSelItem);
	GetInputTerminal()->m_pMovingSideWalkPaxDist->deleteItem( nDBIdx );
	ReloadDatabase();
	ReloadListData(NULL);

	// Added by Xie Bo 2002.5.9
	// Ensure visible
	if(m_treeProc.hSelItem==NULL)
		return;
	HTREEITEM hParent = m_treeProc.GetParentItem(m_treeProc.hSelItem);
	if (hParent != NULL)
		m_treeProc.Expand(hParent, TVE_EXPAND);
	m_treeProc.EnsureVisible(m_treeProc.hSelItem);
	m_treeProc.SelectItem(m_treeProc.hSelItem);
	
	m_btnDel.EnableWindow(FALSE);

	SetModified();
	
}

void CMoveSideWalkPaxDistDlg::OnOK() 
{

	GetInputTerminal()->m_pMovingSideWalkPaxDist->saveDataSet( GetProjPath(), true );	
	CPropertyPage::OnOK();
}

void CMoveSideWalkPaxDistDlg::OnCancel() 
{
	try
	{
		GetInputTerminal()->m_pMovingSideWalkPaxDist->loadDataSet(GetProjPath());			
	}
	catch( FileVersionTooNewError* _pError )
	{
		char szBuf[128];
		_pError->getMessage( szBuf );
		MessageBox( szBuf, "Error", MB_OK|MB_ICONWARNING );
		delete _pError;			
	}
	CPropertyPage::OnCancel();
}
CString CMoveSideWalkPaxDistDlg::GetProjPath()
{
	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pView)->GetDocument();	
	return pDoc->m_ProjInfo.path;
}

void CMoveSideWalkPaxDistDlg::OnItemchangedListPax(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	HTREEITEM hItem = m_treeProc.GetSelectedItem();
	if( hItem == NULL )
		return ;
	int nDBIdx = m_treeProc.GetItemData( hItem );
	if(nDBIdx == -1)
	{
		return;
	}
	if( GetSelectedListItem() != -1 )
		m_ToolBar.GetToolBarCtrl().EnableButton( ID_PEOPLEMOVER_DELETE );
	*pResult = 0;
}

void CMoveSideWalkPaxDistDlg::OnSelchangedMovingsideTree(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	
	ReloadListData(NULL);
	HTREEITEM hItem = m_treeProc.GetSelectedItem();
	if( hItem == NULL )
		return ;
	int nDBIdx = m_treeProc.GetItemData( hItem );
	if(nDBIdx == -1)
	{
		m_btnNew.EnableWindow(TRUE);
		m_btnDel.EnableWindow(FALSE);
		m_ToolBar.GetToolBarCtrl().EnableButton( ID_PEOPLEMOVER_DELETE,FALSE );
		m_ToolBar.GetToolBarCtrl().EnableButton(ID_PEOPLEMOVER_NEW,FALSE );
	}
	else
	{
		m_btnNew.EnableWindow(FALSE);
		m_btnDel.EnableWindow(TRUE);
		m_ToolBar.GetToolBarCtrl().EnableButton( ID_PEOPLEMOVER_DELETE,GetSelectedListItem() != -1 );
		m_ToolBar.GetToolBarCtrl().EnableButton( ID_PEOPLEMOVER_NEW,TRUE);	
	}

	*pResult = 0;
}

void CMoveSideWalkPaxDistDlg::OnToolBarButtonNew() 
{

	bool bDirty = false;
	HTREEITEM hItem = m_treeProc.GetSelectedItem();
	if( hItem == NULL )
		return ;

	int nDBIdx = m_treeProc.GetItemData( hItem );
	if(nDBIdx == -1)
	{
		return;
	}
	
	// the item is already selected
	CPassengerTypeDialog dlg( m_pView );
	if (dlg.DoModal() == IDOK)
	{
		//PassengerConstraint newPaxCon = dlg.GetPaxSelection();
		CMobileElemConstraint mobileConstr	= dlg.GetMobileSelection();
		bool bIsOwn;
		ConstraintDatabase* pConDB = GetConstraintDatabase( bIsOwn );	
		m_listPax.SetCurConDB( pConDB, bIsOwn?0:-1 );
		bDirty = m_listPax.AddEntry( PAX_CON, &mobileConstr );	
	}
	
	SetModified();
}

void CMoveSideWalkPaxDistDlg::OnToolBarButtonDelete() 
{
	m_listPax.DeleteEntry();
	int nItemCount=m_listPax.GetItemCount();
	if(nItemCount==0)
	{
		OnButtonDel();
	}	

	SetModified();
}

void CMoveSideWalkPaxDistDlg::OnEndlabeleditListPax(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
	SetModified();
	
	*pResult = 0;
}
