// ResDispPropDlg.cpp: implementation of the CResDispPropDlg class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ResDispPropDlg.h"

#include "TermPlanDoc.h"
#include "ResourcePoolDlg.h"

#include "common\winmsg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CResDispPropDlg::CResDispPropDlg(CWnd* pParent, CTermPlanDoc* pDoc)
	: CDialog(CResDispPropDlg::IDD, pParent), m_listctrlProp(pDoc)
{
	//{{AFX_DATA_INIT(CPaxDispPropDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_nSelectedRDPSet = -1;
	m_bNewRDPSet = FALSE;
}


void CResDispPropDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CResDispPropDlg)
	DDX_Control(pDX, IDC_STATIC_TOOLBARCONTENTER2, m_toolbarcontenter2);
	DDX_Control(pDX, IDC_STATIC_BARFRAME2, m_btnBarFrame2);
	DDX_Control(pDX, IDC_LIST_DISPPROP2, m_listNamedSets);
	DDX_Control(pDX, IDC_STATIC_BARFRAME, m_btnBarFrame);
	DDX_Control(pDX, IDOK, m_btnOk);
	DDX_Control(pDX, IDCANCEL, m_btnCancel);
	DDX_Control(pDX, IDC_BUTTON_SAVE, m_btnSave);
	DDX_Control(pDX, IDC_LIST_DISPPROP, m_listctrlProp);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CResDispPropDlg, CDialog)
	//{{AFX_MSG_MAP(CResDispPropDlg)
	ON_BN_CLICKED(IDC_BUTTON_SAVE, OnButtonSave)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_NOTIFY(NM_CLICK, IDC_LIST_DISPPROP, OnClickListDispprop)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_DISPPROP2, OnItemChangedListRDPSets)
	ON_NOTIFY(LVN_ENDLABELEDIT, IDC_LIST_DISPPROP2, OnEndLabelEditListRDPSets)
	ON_WM_CLOSE()
	ON_COMMAND(ID_PEOPLEMOVER_NEW, OnRDPSetAdd)
	ON_COMMAND(ID_PEOPLEMOVER_DELETE, OnRDPSetDelete)
	//}}AFX_MSG_MAP
	//ON_COMMAND(ID_TOOLBARBUTTONADD, OnToolbarbuttonadd)
	//ON_COMMAND(ID_TOOLBARBUTTONDEL, OnToolbarbuttondel)
	//ON_COMMAND(ID_TOOLBARBUTTONEDIT, OnToolbarbuttonedit)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CResDispPropDlg message handlers

BOOL CResDispPropDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here

	DWORD dwStyle = m_listctrlProp.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_EX_GRIDLINES;
	m_listctrlProp.SetExtendedStyle( dwStyle );
	// Create column
	m_listctrlProp.InsertColumn( 0, "Name", LVCFMT_CENTER, 100 );
	m_listctrlProp.InsertColumn( 1, "Visible", LVCFMT_CENTER, 100 );
	m_listctrlProp.InsertColumn( 2, "Shape", LVCFMT_CENTER, 100 );
	m_listctrlProp.InsertColumn( 3, "Color", LVCFMT_CENTER, 100 );

	dwStyle = m_listNamedSets.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_EX_GRIDLINES;
	m_listNamedSets.SetExtendedStyle( dwStyle );
	// Create column
	m_listNamedSets.InsertColumn( 0, "Name", LVCFMT_CENTER, 400 );
	

	// Load data.
	UpdateRDPSetList();
	/////
	m_bNewRDPSet = FALSE;

	InitToolbar();
	m_btnSave.EnableWindow( FALSE );
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}




void CResDispPropDlg::OnButtonSave() 
{
	CTermPlanDoc* pDoc = (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();
	pDoc->m_resDispProps.saveInputs(pDoc->m_ProjInfo.path, true);
}

void CResDispPropDlg::OnCancel() 
{
	CTermPlanDoc* pDoc = (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();
	pDoc->m_resDispProps.loadInputs(pDoc->m_ProjInfo.path, &(pDoc->GetTerminal()));
	
	CDialog::OnCancel();
}

void CResDispPropDlg::OnOK() 
{
	CTermPlanDoc* pDoc = (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();
	pDoc->m_resDispProps.saveInputs(pDoc->m_ProjInfo.path, true);
	
	CDialog::OnOK();
}


void CResDispPropDlg::InitToolbar()
{
	CRect rc;
	//m_toolbarcontenter.GetWindowRect( &rc );
	//ScreenToClient(&rc);
	//m_ToolBar.MoveWindow(&rc);
	//m_ToolBar.ShowWindow(SW_SHOW);
	//m_toolbarcontenter.ShowWindow(SW_HIDE);
	//m_ToolBar.GetToolBarCtrl().EnableButton( ID_TOOLBARBUTTONADD );
	//m_ToolBar.GetToolBarCtrl().EnableButton( ID_TOOLBARBUTTONDEL );
	//m_ToolBar.GetToolBarCtrl().HideButton( ID_TOOLBARBUTTONEDIT );

	m_toolbarcontenter2.GetWindowRect( &rc );
	ScreenToClient(&rc);
	m_ToolBar2.MoveWindow(&rc);
	m_ToolBar2.ShowWindow(SW_SHOW);
	m_toolbarcontenter2.ShowWindow(SW_HIDE);
	m_ToolBar2.GetToolBarCtrl().EnableButton( ID_PEOPLEMOVER_NEW );
	m_ToolBar2.GetToolBarCtrl().EnableButton( ID_PEOPLEMOVER_DELETE );
	m_ToolBar2.GetToolBarCtrl().HideButton( ID_PEOPLEMOVER_CHANGE );
}

int CResDispPropDlg::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here
	/*
	if (!m_ToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_ToolBar.LoadToolBar(IDR_ADDDELEDITTOOLBAR))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}
*/

	if (!m_ToolBar2.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_ToolBar2.LoadToolBar(IDR_PEOPLEMOVEBAR))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	return 0;
}

void CResDispPropDlg::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here
	CRect btnrc,toolbarrc,rc;
	m_btnSave.GetWindowRect( &btnrc );
	m_toolbarcontenter2.GetWindowRect( &toolbarrc );

	m_btnBarFrame.MoveWindow( 9,(cy/3)+4,cx-20,(2*cy/3)-25-btnrc.Height() );
	m_btnBarFrame.GetWindowRect( &rc );
	ScreenToClient(&rc);
	rc.top += 6;
	m_listctrlProp.MoveWindow( 9,(cy/3)+10,rc.Width(),rc.Height() );

	m_btnBarFrame2.MoveWindow( 9,4,cx-20,(cy/3)-25 );
	m_toolbarcontenter2.MoveWindow( 11,10,cx-40,toolbarrc.Height() );
	m_toolbarcontenter2.GetWindowRect( &rc );
	ScreenToClient(&rc);
	m_toolbarcontenter2.ShowWindow( SW_HIDE );
	m_ToolBar2.MoveWindow(&rc);
	m_btnBarFrame2.GetWindowRect( &rc );
	ScreenToClient(&rc);
	rc.top = 35;
	m_listNamedSets.MoveWindow( 9,35,rc.Width(),rc.Height() );
	
	m_btnSave.MoveWindow( cx-20-3*btnrc.Width(),cy-10-btnrc.Height(),btnrc.Width(),btnrc.Height() );
	m_btnOk.MoveWindow( cx-15-2*btnrc.Width(),cy-10-btnrc.Height(),btnrc.Width(),btnrc.Height() );
	m_btnCancel.MoveWindow( cx-10-btnrc.Width(),cy-10-btnrc.Height(),btnrc.Width(),btnrc.Height() );

	InvalidateRect(NULL);
}

/*
void CResDispPropDlg::OnToolbarbuttonadd() 
{
	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();


	
	CResourcePoolDlg dlg( pDoc, m_pParentWnd );
	//ShowWindow( SW_HIDE );
	if(dlg.DoModal()==IDOK) {
		int nPoolIdx = dlg.getSelectPoolIdx();
		CResourceDispPropItem*  pItem = new CResourceDispPropItem();
		pItem->SetResourcePoolIndex( nPoolIdx );
		pDoc->m_resDispProps.AddDispPropItem( pItem );
		int nCount = m_listctrlProp.GetItemCount();
		m_listctrlProp.InsertResDispItem( nCount, pItem );
		m_listctrlProp.SetItemState( nCount,LVIS_SELECTED,LVIS_SELECTED );
		m_btnSave.EnableWindow();
	}
	//ShowWindow( SW_SHOW );
}

void CResDispPropDlg::OnToolbarbuttondel() 
{
	// TODO: Add your command handler code here
	POSITION pos = m_listctrlProp.GetFirstSelectedItemPosition();
	if (pos == NULL)
		return;
	else
	{
		int nItem = m_listctrlProp.GetNextSelectedItem(pos);
		if( nItem < 2 )
		{
			MessageBox( "Can't delete this item!" );
			return;
		}
		CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();	
		// delete from db
		pDoc->m_resDispProps.DeleteItem( nItem-2 );
		m_listctrlProp.DeleteItem( nItem );
		
	}
}


void CResDispPropDlg::OnToolbarbuttonedit() 
{
	// TODO: Add your command handler code here
	
}
*/

void CResDispPropDlg::OnClickListDispprop(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	if( m_listctrlProp.GetSelectedCount() > 0 )
	{
		//m_ToolBar.GetToolBarCtrl().EnableButton( ID_TOOLBARBUTTONDEL );
    	//m_ToolBar.GetToolBarCtrl().EnableButton( ID_TOOLBARBUTTONEDIT );

	}
	*pResult = 0;
}

void CResDispPropDlg::OnClose() 
{
	//Added by Tim In 2/8/2003*************************************
	//In order to fix that bug(close mobile elements dlg when pax shapes window is showing) must 
	//not call CPaxDispListCtrl::OnDrawItem() when closing this dlg,so call following line .
	m_listctrlProp.DeleteAllItems();
	CDialog::OnClose();
}

void CResDispPropDlg::OnItemChangedListRDPSets(NMHDR* pNMHDR, LRESULT* pResult)
{
	
	NM_LISTVIEW* pnmv = (NM_LISTVIEW*)pNMHDR;

	*pResult = 0;
	
	if ((pnmv->uChanged & LVIF_STATE) != LVIF_STATE)
		return;

	if(!(pnmv->uOldState & LVIS_SELECTED) && !(pnmv->uNewState & LVIS_SELECTED))
		return;

	

	if(pnmv->uOldState & LVIS_SELECTED) {
		if(!(pnmv->uNewState & LVIS_SELECTED)) {
			// TRACE("Unselected Item %d...\n", pnmv->iItem);
		}
	}
	else if(pnmv->uNewState & LVIS_SELECTED) {
		//a new item has been selected
		// TRACE("Selected Item %d...\n", pnmv->iItem);
		m_nSelectedRDPSet = (int) m_listNamedSets.GetItemData(pnmv->iItem);
		CTermPlanDoc* pDoc = (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();
		pDoc->m_resDispProps.SetCurrentRDPSet(m_nSelectedRDPSet);
	}
	else
		ASSERT(0);

	UpdateRDP();
}


void CResDispPropDlg::OnRDPSetAdd()
{
	CTermPlanDoc* pDoc = (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();

	int nRDPSetCount = pDoc->m_resDispProps.GetRDPSetCount();
	pDoc->m_resDispProps.AddRDPSet("undefined");
	m_listNamedSets.InsertItem(LVIF_TEXT | LVIF_PARAM, nRDPSetCount, "         ", 0, 0, 0, nRDPSetCount);
	m_bNewRDPSet = TRUE;
	m_listNamedSets.SetItemState(nRDPSetCount, LVIS_SELECTED, LVIS_SELECTED);
	m_listNamedSets.SetFocus();
	m_listNamedSets.EditLabel(nRDPSetCount);
}

void CResDispPropDlg::OnRDPSetDelete()
{
	if(m_listNamedSets.GetItemCount() < 2) {
		AfxMessageBox("At least one Named Property Set must exist.");
		return;
	}
	CTermPlanDoc* pDoc = (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();
	ASSERT(m_nSelectedRDPSet >= 0 && m_nSelectedRDPSet < pDoc->m_resDispProps.GetRDPSetCount());
	pDoc->m_resDispProps.DeletRDPSet(m_nSelectedRDPSet);

	m_nSelectedRDPSet = pDoc->m_resDispProps.GetCurrentRDPSetIdx();

	//m_listNamedSets.SetItemState(m_nSelectedRDPSet, LVIS_SELECTED, LVIS_SELECTED);

	UpdateRDPSetList();
}

void CResDispPropDlg::UpdateRDP()
{
	m_listctrlProp.DeleteAllItems();
	CTermPlanDoc* pDoc = (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();

	int nCount = pDoc->m_resDispProps.GetCount();
	for( int i=0; i<nCount; i++ )
	{
		CResourceDispPropItem* pItem = pDoc->m_resDispProps.GetDispPropItem( i );
		m_listctrlProp.InsertResDispItem( i, pItem );
	}
}

void CResDispPropDlg::OnEndLabelEditListRDPSets(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
	*pResult = 0;

	CTermPlanDoc* pDoc = (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();

	if(!pDispInfo->item.pszText) {
		if(m_bNewRDPSet) {
			//delete this new pdp set
			OnRDPSetDelete();	
		}
		return;
	}

	
	m_bNewRDPSet=FALSE;

	int nIdx = pDispInfo->item.iItem;

	ASSERT(pDoc->m_resDispProps.GetCurrentRDPSetIdx() == nIdx);
	pDoc->m_resDispProps.RenameCurrentRDPSet(pDispInfo->item.pszText);

	m_listNamedSets.SetItemText(nIdx, 0, pDispInfo->item.pszText);

	*pResult =1;
}

void CResDispPropDlg::UpdateRDPSetList()
{
	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();

	m_listNamedSets.DeleteAllItems();

	int nRDPSetCount = pDoc->m_resDispProps.GetRDPSetCount();
	for(int i=0; i<nRDPSetCount; i++) {
		m_listNamedSets.InsertItem(LVIF_TEXT | LVIF_PARAM, i, pDoc->m_resDispProps.GetRDPSetName(i), 0, 0, 0, (LPARAM) i);
	}
	m_nSelectedRDPSet = pDoc->m_resDispProps.GetCurrentRDPSetIdx();
	m_listNamedSets.SetItemState(m_nSelectedRDPSet, LVIS_SELECTED, LVIS_SELECTED );
}
