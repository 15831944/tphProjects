// PaxTypeDefDlg.cpp : implementation file
//

#include "stdafx.h"
#include "termplan.h"
#include "PaxTypeDefDlg.h"
#include "MFCExControl\SmartEdit.h"
#include "..\inputs\typelist.h"
#include "..\inputs\distlist.h"
#include "common\WinMsg.h"
#include "termplandoc.h"
#include ".\paxtypedefdlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPaxTypeDefDlg dialog


CPaxTypeDefDlg::CPaxTypeDefDlg(CWnd* pParent)
: CDialog(CPaxTypeDefDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPaxTypeDefDlg)
	//}}AFX_DATA_INIT
	m_bStructureChanged = false;
}


void CPaxTypeDefDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPaxTypeDefDlg)
	DDX_Control(pDX, IDC_STATIC_TOOLBARCONTENTER, m_toolbarcontenter);
	DDX_Control(pDX, IDC_BUTTON_SAVE, m_btnSave);
	DDX_Control(pDX, IDC_TREE_PAX, m_treePax);
	DDX_Control(pDX, IDC_COMBO_LEVEL, m_comboLevel);
	DDX_Control(pDX, IDC_LIST_PAXNAME, m_listboxName);
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_EDIT_NAME, m_EditLevelName);
}


BEGIN_MESSAGE_MAP(CPaxTypeDefDlg, CDialog)
//{{AFX_MSG_MAP(CPaxTypeDefDlg)
	ON_WM_CREATE()
	ON_LBN_DBLCLK(IDC_LIST_PAXNAME, OnDblclkListPaxname)
	ON_MESSAGE(WM_END_EDIT,OnEndEdit)
	ON_CBN_SELCHANGE(IDC_COMBO_LEVEL, OnSelchangeComboLevel)
	ON_CBN_DROPDOWN(IDC_COMBO_LEVEL, OnDropdownComboLevel)
	ON_LBN_SELCHANGE(IDC_LIST_PAXNAME, OnSelchangeListPaxname)
	ON_BN_CLICKED(IDC_BUTTON_SAVE, OnButtonSave)
	ON_COMMAND(ID_PEOPLEMOVER_NEW, OnPeoplemoverNew)
	ON_COMMAND(ID_PEOPLEMOVER_DELETE, OnPeoplemoverDelete)
	ON_NOTIFY(TVN_ITEMEXPANDING,IDC_TREE_PAX, OnTvnItemexpanding)
	//}}AFX_MSG_MAP

	ON_EN_KILLFOCUS(IDC_EDIT_NAME, OnEnKillfocusEditName)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPaxTypeDefDlg message handlers

BOOL CPaxTypeDefDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	InitToolbar();
	
	ReloadLevelCombo();
	
	ReloadName( 0 );
	
	ReloadTree();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


void CPaxTypeDefDlg::OnDblclkListPaxname() 
{
	// TODO: Add your control notification handler code here
	m_nSelIdx = m_listboxName.GetCurSel();
	if( m_nSelIdx == LB_ERR )
		return;
	
	EditItem( m_nSelIdx );
	m_btnSave.EnableWindow( true );
}


void CPaxTypeDefDlg::EditItem( int _nIdx )
{
	CRect rectItem;
	CRect rectLB;
	m_listboxName.GetWindowRect( &rectLB );
	ScreenToClient( &rectLB );
	m_listboxName.GetItemRect( _nIdx, &rectItem );
	
	rectItem.OffsetRect( rectLB.left+2, rectLB.top+1 );
	
	rectItem.right += 1;
	rectItem.bottom += 4;
	
	CString csItemText;
	m_listboxName.GetText( _nIdx, csItemText );
	CSmartEdit* pEdit = new CSmartEdit( csItemText );
	DWORD dwStyle = ES_LEFT | WS_BORDER|WS_CHILD|WS_VISIBLE|ES_AUTOHSCROLL;
	pEdit->Create( dwStyle, rectItem, this, IDC_IPEDIT );	

	m_ToolBar.GetToolBarCtrl().EnableButton( ID_PEOPLEMOVER_NEW,FALSE );
	m_ToolBar.GetToolBarCtrl().EnableButton( ID_PEOPLEMOVER_DELETE,FALSE );
}

LONG CPaxTypeDefDlg::OnEndEdit( WPARAM p_wParam, LPARAM p_lParam )
{
	if( p_lParam != IDC_IPEDIT )
		return 0;
	
	if( m_nSelIdx == LB_ERR )
		return 0;
	
	CString csStr( (LPTSTR)p_wParam );
	if( csStr.IsEmpty() )
	{
		MessageBox( "Empty String", "Error", MB_OK|MB_ICONWARNING );
		m_listboxName.DeleteString( m_nSelIdx );
		m_listboxName.SetCurSel( -1 );
		OnSelchangeListPaxname();
	}
	else
	{
		// check if dup
		int i=0;
		for( ; i<m_listboxName.GetCount(); i++ )
		{
			if (i!=m_nSelIdx)
			{
				CString csTemp;
				m_listboxName.GetText( i, csTemp );
				if( csTemp.CompareNoCase( csStr ) == 0 )
				{
					MessageBox( "Duplicated Name", "Error", MB_OK|MB_ICONWARNING );
					m_listboxName.DeleteString( m_nSelIdx );
					m_listboxName.SetCurSel( -1 );
					OnSelchangeListPaxname();
					break;
				}
			}
		}

		if( i == m_listboxName.GetCount() )
		{

			m_listboxName.InsertString( m_nSelIdx, (LPTSTR)p_wParam );
			m_listboxName.DeleteString( m_nSelIdx + 1 );
			
			int strIndex = GetInputTerminal()->inTypeList->getTypeIndex(csStr);
			if(strIndex != 0)
			{
				int nLevel = GetInputTerminal()->inTypeList->getLevel(strIndex);
				if(nLevel != 0 && nLevel != m_comboLevel.GetCurSel()+1)
				{
					CString strMsg;
					strMsg.Format("The name string has been existed in Level %d, please input again.", nLevel);
					MessageBox(strMsg, "Error", MB_OK|MB_ICONWARNING);
					EditItem(m_nSelIdx);
					return -1;
				}
			}

			// insert to the database
			int nLevelIdx = m_comboLevel.GetCurSel();
			int nBranchCount = GetInputTerminal()->inTypeList->getBranchesAtLevel( nLevelIdx );
			if( m_nSelIdx == nBranchCount )
			{
				// new item
				GetInputTerminal()->inTypeList->addCell( nLevelIdx, csStr );
			}
			else
			{
				GetInputTerminal()->inTypeList->replaceString( nLevelIdx, m_nSelIdx, csStr );
			}
			m_listboxName.SetCurSel( m_nSelIdx );
			OnSelchangeListPaxname();
			ReloadTree();
		}
	}
	m_ToolBar.GetToolBarCtrl().EnableButton( ID_PEOPLEMOVER_NEW,TRUE );
	m_ToolBar.GetToolBarCtrl().EnableButton( ID_PEOPLEMOVER_DELETE,TRUE );
	return 0;
}


void CPaxTypeDefDlg::ReloadName(int _nIdxLevel)
{
	m_listboxName.ResetContent();
	int nBranch = GetInputTerminal()->inTypeList->getBranchesAtLevel( _nIdxLevel );
	for( int i=0; i<nBranch; i++ )
	{
		m_listboxName.AddString( GetInputTerminal()->inTypeList->getTypeName( i, _nIdxLevel ));
	}
	m_EditLevelName.SetWindowText(GetInputTerminal()->inTypeList->GetLevelName(_nIdxLevel)) ;
	m_listboxName.SetCurSel( -1 );
	OnSelchangeListPaxname();
}

void CPaxTypeDefDlg::ReloadTree()
{
	m_treePax.DeleteAllItems();
	int nIdx = m_comboLevel.GetCurSel();
	InsertPaxItem( TVI_ROOT, 0);
}

void CPaxTypeDefDlg::InsertPaxItem(HTREEITEM _hParent, int _nIdxMinLevel )
{
	if (_nIdxMinLevel >= LEVELS)
		return;
	
	int nBranch = GetInputTerminal()->inTypeList->getBranchesAtLevel( _nIdxMinLevel );
	for( int i=0; i<nBranch; i++ )
	{
		HTREEITEM hItem = m_treePax.InsertItem( GetInputTerminal()->inTypeList->getTypeName( i, _nIdxMinLevel ),  _hParent );
		m_treePax.SetItemData(hItem,_nIdxMinLevel);
		if (_nIdxMinLevel >= LEVELS - 1)
			continue;
		int nChildCount = GetInputTerminal()->inTypeList->getBranchesAtLevel( _nIdxMinLevel + 1 );
		// add a dummy child if the node has children
		if(nChildCount != 0)
			m_treePax.InsertItem("error",hItem);
	}
}

// just load one level extra
void CPaxTypeDefDlg::ReloadLevelCombo()
{
	m_comboLevel.ResetContent();
	for( int i=0; i<LEVELS; i++ )
	{
		CString csLabel;
		csLabel.Format( "Level %d", i+1 );
		m_comboLevel.AddString( csLabel );
		int nBranch = GetInputTerminal()->inTypeList->getBranchesAtLevel( i );
		if( nBranch == 0 )
			break;
	}
	m_comboLevel.SetCurSel( 0 );
}





void CPaxTypeDefDlg::OnSelchangeComboLevel() 
{
	// TODO: Add your control notification handler code here
	int nIdx = m_comboLevel.GetCurSel();
	
	ReloadName( nIdx );
}

void CPaxTypeDefDlg::OnDropdownComboLevel() 
{
	// TODO: Add your control notification handler code here
	ReloadLevelCombo();	
}



void CPaxTypeDefDlg::OnSelchangeListPaxname() 
{
	// TODO: Add your control notification handler code here
	int nCurSel = m_listboxName.GetCurSel();
	if( nCurSel == LB_ERR )
	{
		m_ToolBar.GetToolBarCtrl().EnableButton( ID_PEOPLEMOVER_DELETE,FALSE );
	//	m_btnDel.EnableWindow( false );
	}
	else
	{
		m_ToolBar.GetToolBarCtrl().EnableButton( ID_PEOPLEMOVER_DELETE);
	//	m_btnDel.EnableWindow( true );
	}
	
}

void CPaxTypeDefDlg::OnButtonSave() 
{
	// TODO: Add your control notification handler code here
	GetInputTerminal()->inTypeList->saveDataSet( GetProjPath(), true );
	m_btnSave.EnableWindow( false );
	if(	m_bStructureChanged )
	{
		GetInputTerminal()->paxDistList->clear();
		GetInputTerminal()->paxDistList->initDefaultValues();
		GetInputTerminal()->paxDistList->saveDataSet( GetProjPath(), true );
		m_bStructureChanged = false;
	}
}

void CPaxTypeDefDlg::OnCancel() 
{
	// TODO: Add extra cleanup here
	if( !m_btnSave.IsWindowEnabled() )  // nonzero if the window was previously disabled
	{
		CDialog::OnCancel();
		return;
	}
	
	AfxGetApp()->BeginWaitCursor();
	try
	{
		GetInputTerminal()->inTypeList->loadDataSet( GetProjPath() );
	}
	catch( FileVersionTooNewError* _pError )
	{
		char szBuf[128];
		_pError->getMessage( szBuf );
		MessageBox( szBuf, "Error", MB_OK|MB_ICONWARNING );
		delete _pError;			
	}
	AfxGetApp()->EndWaitCursor();
	
	CDialog::OnCancel();
}

void CPaxTypeDefDlg::OnOK() 
{
	// TODO: Add extra validation here
	if( !m_btnSave.IsWindowEnabled() )  // nonzero if the window was previously disabled
	{
		CDialog::OnOK();
		return;
	}
	
	AfxGetApp()->BeginWaitCursor();
	OnButtonSave();
	AfxGetApp()->EndWaitCursor();

	CDialog::OnOK();
}


InputTerminal* CPaxTypeDefDlg::GetInputTerminal()
{
	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();	
    return (InputTerminal*)&pDoc->GetTerminal();
}


CString CPaxTypeDefDlg::GetProjPath()
{
	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();	
	return pDoc->m_ProjInfo.path;
}


void CPaxTypeDefDlg::OnPeoplemoverNew() 
{
	// TODO: Add your command handler code here
	int nCount = m_listboxName.GetCount();
	if( nCount == BRANCHES )
		return;
	
	m_nSelIdx = m_listboxName.GetCount();
	m_listboxName.InsertString( m_nSelIdx, "" );
	m_listboxName.SetCurSel( m_nSelIdx );
	OnSelchangeListPaxname();
	
	EditItem( m_nSelIdx );	
	
	m_btnSave.EnableWindow( true );
	m_bStructureChanged = true;

}

void CPaxTypeDefDlg::OnPeoplemoverDelete() 
{
	// TODO: Add your command handler code here
	int nCurSel = m_listboxName.GetCurSel();
	if( nCurSel == LB_ERR )
		return;
	
	m_listboxName.DeleteString( nCurSel );
	m_listboxName.SetCurSel( -1 );
	OnSelchangeListPaxname();
	
	char strList[BRANCHES][LEVELS];
	
	int nNameCount = m_listboxName.GetCount();
	for( int i=0; i<nNameCount; i++ )
	{
		m_listboxName.GetText( i, strList[i] );
	}
	
	int nIdxLevel = m_comboLevel.GetCurSel();
	
	GetInputTerminal()->inTypeList->UpdateLevel( nIdxLevel, nNameCount, strList );
	
	ReloadTree();
	
	m_btnSave.EnableWindow( true );
	m_bStructureChanged = true;

}

void CPaxTypeDefDlg::InitToolbar()
{
	CRect rc;
	m_toolbarcontenter.GetWindowRect( &rc );
	ScreenToClient(&rc);
	m_ToolBar.MoveWindow(&rc);
	m_ToolBar.ShowWindow(SW_SHOW);
	m_toolbarcontenter.ShowWindow(SW_HIDE);
	m_ToolBar.GetToolBarCtrl().EnableButton( ID_PEOPLEMOVER_NEW );
	m_ToolBar.GetToolBarCtrl().HideButton( ID_PEOPLEMOVER_CHANGE );
	m_ToolBar.GetToolBarCtrl().EnableButton( ID_PEOPLEMOVER_DELETE,FALSE );
	

	
	
}

int CPaxTypeDefDlg::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here
	if (!m_ToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_ToolBar.LoadToolBar(IDR_PEOPLEMOVEBAR))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}
	return 0;
}

void CPaxTypeDefDlg::OnEnKillfocusEditName()
{
	// TODO: Add your control notification handler code here
	int nCurSel = m_comboLevel.GetCurSel();
	if( nCurSel == LB_ERR )
		return;
	CString levelName ;
     m_EditLevelName.GetWindowText(levelName ) ;
	 GetInputTerminal()->inTypeList->SetLevelName(levelName,nCurSel) ;
	 m_btnSave.EnableWindow( true );
	 m_bStructureChanged = true;
}

void CPaxTypeDefDlg::DeleteAllChildren(HTREEITEM hItem)
{
	HTREEITEM hChild = m_treePax.GetChildItem(hItem);
	while(hChild)
	{
		HTREEITEM hNext = m_treePax.GetNextSiblingItem(hChild);
		m_treePax.DeleteItem(hChild);
		hChild = hNext;
	} 
}

void CPaxTypeDefDlg::OnTvnItemexpanding( NMHDR *pNMHDR, LRESULT *pResult )
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	HTREEITEM hItem = pNMTreeView->itemNew.hItem;
	*pResult = 0;

	int nIdx = (int)m_treePax.GetItemData(hItem);
	int nBranch = 0;
	nIdx++;
	if (nIdx == LB_ERR || nIdx >= LEVELS)
		return;
	if(pNMTreeView->action == TVE_EXPAND)
	{
		DeleteAllChildren(hItem);
		// add the real children (only if they themselves have children
		InsertPaxItem(hItem,nIdx);
	}
	else 
	{
		DeleteAllChildren(hItem);
		nBranch = GetInputTerminal()->inTypeList->getBranchesAtLevel( nIdx );
		// add a dummy child if the node has children
		if(nBranch != 0)
			m_treePax.InsertItem("error",hItem);
	}
}
