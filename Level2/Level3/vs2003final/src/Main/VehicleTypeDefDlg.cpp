#include "StdAfx.h"
#include "termplan.h"
#include "MFCExControl\SmartEdit.h"
#include "common\WinMsg.h"
#include ".\vehicletypedefdlg.h"
#include "Common\template.h"
#include "Landside\InputLandside.h"
#include "Landside\VehicleTypeDistribution.h"
#include "TermPlanDoc.h"
#include "Landside/LandsideVehicleTypeList.h"

CVehicleTypeDefDlg::CVehicleTypeDefDlg(CWnd* pParent)
: CDialog(CVehicleTypeDefDlg::IDD, pParent)
{
}

CVehicleTypeDefDlg::~CVehicleTypeDefDlg(void)
{
}

void CVehicleTypeDefDlg::DoDataExchange( CDataExchange* pDX )
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

BEGIN_MESSAGE_MAP(CVehicleTypeDefDlg, CDialog)
	//{{AFX_MSG_MAP(CPaxTypeDefDlg)
	ON_WM_CREATE()
	ON_LBN_DBLCLK(IDC_LIST_PAXNAME, OnDblclkListVehiclename)
	ON_MESSAGE(WM_END_EDIT,OnEndEdit)
	ON_CBN_SELCHANGE(IDC_COMBO_LEVEL, OnSelchangeComboLevel)
	ON_CBN_DROPDOWN(IDC_COMBO_LEVEL, OnDropdownComboLevel)
	ON_LBN_SELCHANGE(IDC_LIST_PAXNAME, OnSelchangeListVehiclename)
	ON_BN_CLICKED(IDC_BUTTON_SAVE, OnButtonSave)
	ON_COMMAND(ID_PEOPLEMOVER_NEW, OnVehicleTypeNew)
	ON_COMMAND(ID_PEOPLEMOVER_DELETE, OnVehicleTypeDelete)
	ON_NOTIFY(TVN_ITEMEXPANDING,IDC_TREE_PAX, OnTvnItemexpanding)
	//}}AFX_MSG_MAP

	ON_EN_KILLFOCUS(IDC_EDIT_NAME, OnEnKillfocusEditName)
END_MESSAGE_MAP()

BOOL CVehicleTypeDefDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	m_pVehicleTypeList = GetInputLandside()->getLandsideVehicleTypeList();
	// TODO: Add extra initialization here
	InitToolbar();

	ReloadLevelCombo();

	ReloadName( 0 );

	ReloadTree();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CVehicleTypeDefDlg::InitToolbar()
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

void CVehicleTypeDefDlg::InsertVehicleItem( HTREEITEM _hParent, int _nIdxMinLevel )
{
	if (_nIdxMinLevel >= LEVELS)
		return;

	LandsideVehicleType* pVehicleType = m_pVehicleTypeList->GetItem(_nIdxMinLevel);
	if (pVehicleType == NULL)
		return;
	
	int nBranch = pVehicleType->GetLeafCount();
	for( int i=0; i<nBranch; i++ )
	{
		HTREEITEM hItem = m_treePax.InsertItem( pVehicleType->GetLeaf(i),  _hParent );
		m_treePax.SetItemData(hItem,_nIdxMinLevel);
		if (_nIdxMinLevel >= LEVELS - 1)
			continue;

		int nNextLevel = _nIdxMinLevel + 1;
		if (nNextLevel < 0 || nNextLevel >= m_pVehicleTypeList->GetItemCount())
			continue;
		LandsideVehicleType* pVehicleType = m_pVehicleTypeList->GetItem(_nIdxMinLevel + 1);
		// add a dummy child if the node has children
		if (pVehicleType && pVehicleType->GetLeafCount() != 0)
		{
			m_treePax.InsertItem("error",hItem);
		}	
	}
}

void CVehicleTypeDefDlg::DeleteAllChildren( HTREEITEM hItem )
{
	HTREEITEM hChild = m_treePax.GetChildItem(hItem);
	while(hChild)
	{
		HTREEITEM hNext = m_treePax.GetNextSiblingItem(hChild);
		m_treePax.DeleteItem(hChild);
		hChild = hNext;
	} 
}

void CVehicleTypeDefDlg::ReloadLevelCombo()
{
	m_comboLevel.ResetContent();
	m_pVehicleTypeList->DeleteInvalidData();
	int nLevelCount = m_pVehicleTypeList->GetItemCount();
	for( int i=0; i<LEVELS; i++ )
	{
		CString csLabel;
		csLabel.Format( "Level %d", i+1 );
		m_comboLevel.AddString( csLabel );
		if(i >= nLevelCount)
			break;
	}
	m_comboLevel.SetCurSel( 0 );
}

void CVehicleTypeDefDlg::ReloadTree()
{
	m_treePax.DeleteAllItems();
	int nIdx = m_comboLevel.GetCurSel();
	InsertVehicleItem( TVI_ROOT, 0);
}

void CVehicleTypeDefDlg::ReloadName( int _nIdxLevel )
{
	m_listboxName.ResetContent();
	if (_nIdxLevel >= m_pVehicleTypeList->GetItemCount())
	{
		LandsideVehicleType* pVehicleType = new LandsideVehicleType();
		CString strLevel;
		strLevel.Format(_T("LEVEL %d"),_nIdxLevel + 1);
		pVehicleType->SetLevelName(strLevel);
		m_pVehicleTypeList->AddItem(pVehicleType);
	}

	if (_nIdxLevel < 0 || _nIdxLevel >= m_pVehicleTypeList->GetItemCount())
		return;
	
	LandsideVehicleType* pVehicleType = m_pVehicleTypeList->GetItem(_nIdxLevel);
	if (pVehicleType == NULL)
		return;
	
	int nBranch = pVehicleType->GetLeafCount();
	for( int i=0; i<nBranch; i++ )
	{
		CString strLeaf = pVehicleType->GetLeaf(i);
		
		m_listboxName.AddString( strLeaf);
	}
	m_EditLevelName.SetWindowText(pVehicleType->GetLevelName()) ;
	m_listboxName.SetCurSel( -1 );
	OnSelchangeListVehiclename();
}

void CVehicleTypeDefDlg::EditItem( int _nIdx )
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

int CVehicleTypeDefDlg::OnCreate( LPCREATESTRUCT lpCreateStruct )
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

void CVehicleTypeDefDlg::OnDblclkListVehiclename()
{
	// TODO: Add your control notification handler code here
	m_nSelIdx = m_listboxName.GetCurSel();
	if( m_nSelIdx == LB_ERR )
		return;

	EditItem( m_nSelIdx );
	m_btnSave.EnableWindow( true );
}

LONG CVehicleTypeDefDlg::OnEndEdit( WPARAM p_wParam, LPARAM p_lParam )
{
	if( p_lParam != IDC_IPEDIT )
		return 0;

	if( m_nSelIdx == LB_ERR )
		return 0;

	CString csStr( (LPTSTR)p_wParam );
	if( csStr.IsEmpty() )
	{
		MessageBox( "Empty String", "Error", MB_OK|MB_ICONWARNING );
		int nLevelIdx = m_comboLevel.GetCurSel();
		LandsideVehicleType* pVehicleType = m_pVehicleTypeList->GetItem(nLevelIdx);
		if (pVehicleType)
		{
			pVehicleType->DeleteLeaf(m_nSelIdx);
		}
		m_listboxName.DeleteString( m_nSelIdx );
		m_listboxName.SetCurSel( -1 );
		ReloadTree();
		OnSelchangeListVehiclename();
	}
	else
	{
		// check if dup
		for( int i=0; i<m_listboxName.GetCount(); i++ )
		{
			if (i!=m_nSelIdx)
			{
				CString csTemp;
				m_listboxName.GetText( i, csTemp );
				if( csTemp.CompareNoCase( csStr ) == 0 )
				{
					MessageBox( "Duplicated Name", "Error", MB_OK|MB_ICONWARNING );
					
					int nLevelIdx = m_comboLevel.GetCurSel();
					LandsideVehicleType* pVehicleType = m_pVehicleTypeList->GetItem(nLevelIdx);
					if (pVehicleType)
					{
						pVehicleType->DeleteLeaf(m_nSelIdx);
					}
					m_listboxName.DeleteString( m_nSelIdx );
					m_listboxName.SetCurSel( -1 );
					OnSelchangeListVehiclename();
					ReloadTree();
					break;
				}
			}
		}

		if( i == m_listboxName.GetCount() )
		{

			m_listboxName.InsertString( m_nSelIdx, (LPTSTR)p_wParam );
			m_listboxName.DeleteString( m_nSelIdx + 1 );

			// insert to the database
			int nLevelIdx = m_comboLevel.GetCurSel();
			LandsideVehicleType* pVehicleType = m_pVehicleTypeList->GetItem(nLevelIdx);
			if (pVehicleType)
			{
				int nBranchCount = pVehicleType->GetLeafCount();
				if( m_nSelIdx == nBranchCount )
				{
					// new item
					m_pVehicleTypeList->AddVehicleType(nLevelIdx,csStr);
				}
				else
				{
					m_pVehicleTypeList->UpdateVehicleType( nLevelIdx, m_nSelIdx, csStr );
				}
				m_listboxName.SetCurSel( m_nSelIdx );
				OnSelchangeListVehiclename();
				ReloadTree();
			}
			
		}
	}
	m_ToolBar.GetToolBarCtrl().EnableButton( ID_PEOPLEMOVER_NEW,TRUE );
	m_ToolBar.GetToolBarCtrl().EnableButton( ID_PEOPLEMOVER_DELETE,TRUE );
	return 0;
}

void CVehicleTypeDefDlg::OnSelchangeComboLevel()
{
	// TODO: Add your control notification handler code here
	int nIdx = m_comboLevel.GetCurSel();

	ReloadName( nIdx );
}

void CVehicleTypeDefDlg::OnDropdownComboLevel()
{
	// TODO: Add your control notification handler code here
	ReloadLevelCombo();	
}

void CVehicleTypeDefDlg::OnSelchangeListVehiclename()
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

void CVehicleTypeDefDlg::OnButtonSave()
{
	// TODO: Add your control notification handler code here
	//GetInputTerminal()->inTypeList->saveDataSet( GetProjPath(), true );
	m_pVehicleTypeList->SaveData(-1);
	m_btnSave.EnableWindow( false );
	if(	m_bStructureChanged )
	{
		GetInputLandside()->getVehicleDistributionList()->clear();
		GetInputLandside()->getVehicleDistributionList()->initDefaultValues();
		GetInputLandside()->getVehicleDistributionList()->saveDataSet(GetProjPath(),true);
		m_bStructureChanged = false;
	}
}

void CVehicleTypeDefDlg::OnOK()
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

void CVehicleTypeDefDlg::OnVehicleTypeNew()
{
	// TODO: Add your command handler code here
	int nCount = m_listboxName.GetCount();
	if( nCount == BRANCHES )
		return;

	m_nSelIdx = m_listboxName.GetCount();
	m_listboxName.InsertString( m_nSelIdx, "" );
	m_listboxName.SetCurSel( m_nSelIdx );
	OnSelchangeListVehiclename();

	EditItem( m_nSelIdx );	

	m_btnSave.EnableWindow( true );
	m_bStructureChanged = true;
}

void CVehicleTypeDefDlg::OnVehicleTypeDelete()
{
	// TODO: Add your command handler code here
	int nCurSel = m_listboxName.GetCurSel();
	if( nCurSel == LB_ERR )
		return;

	int nIdxLevel = m_comboLevel.GetCurSel();
	m_pVehicleTypeList->DeleteVehicleType(nIdxLevel,nCurSel);

	m_listboxName.DeleteString( nCurSel );
	m_listboxName.SetCurSel( -1 );
	OnSelchangeListVehiclename();

	ReloadTree();

	m_btnSave.EnableWindow( true );
	m_bStructureChanged = true;
}

void CVehicleTypeDefDlg::OnTvnItemexpanding( NMHDR *pNMHDR, LRESULT *pResult )
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
		InsertVehicleItem(hItem,nIdx);
	}
	else 
	{
		DeleteAllChildren(hItem);
		LandsideVehicleType* pVehicle = m_pVehicleTypeList->GetItem(nIdx);
		if (pVehicle && pVehicle->GetLeafCount() != 0)
		{
			// add a dummy child if the node has children
			m_treePax.InsertItem("error",hItem);
		}
		
	}
}

void CVehicleTypeDefDlg::OnEnKillfocusEditName()
{
	// TODO: Add your control notification handler code here
	int nCurSel = m_comboLevel.GetCurSel();
	if( nCurSel == LB_ERR )
		return;
	CString levelName ;
	m_EditLevelName.GetWindowText(levelName ) ;
	LandsideVehicleType* pVehicleType = m_pVehicleTypeList->GetItem(nCurSel);
	if (pVehicleType == NULL)
		return;
	
	pVehicleType->SetLevelName(levelName);
	m_btnSave.EnableWindow( true );
	m_bStructureChanged = true;
}

CString CVehicleTypeDefDlg::GetProjPath()
{
	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();	
	return pDoc->m_ProjInfo.path;
}

InputLandside* CVehicleTypeDefDlg::GetInputLandside()
{
	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();	
	return pDoc->GetInputLandside();
}

void CVehicleTypeDefDlg::OnCancel()
{
	// TODO: Add extra cleanup here
	if( !m_btnSave.IsWindowEnabled() )  // nonzero if the window was previously disabled
	{
		CDialog::OnCancel();
		return;
	}

	AfxGetApp()->BeginWaitCursor();
	GetInputLandside()->getLandsideVehicleTypeList()->ReadData(-1);
	AfxGetApp()->EndWaitCursor();

	CDialog::OnCancel();
}


