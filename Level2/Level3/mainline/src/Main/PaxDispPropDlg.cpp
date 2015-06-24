// PaxDispPropDlg.cpp : implementation file
//

#include "stdafx.h"
#include "termplan.h"

#include "PaxDispPropDlg.h"
#include "TermPlanDoc.h"
#include "PaxFilterDlg.h"
#include "..\common\winmsg.h"
#include "..\Inputs\MobileElemConstraint.h"
#include ".\paxdisppropdlg.h"
#include "ViewMsg.h"
#include "3DView.h"
#include "ChildFrm.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPaxDispPropDlg dialog


CPaxDispPropDlg::CPaxDispPropDlg(CWnd* pParent)
	: CDialog(CPaxDispPropDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPaxDispPropDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_nSelectedPDPSet = -1;
	m_bNewPDPSet = FALSE;
}


void CPaxDispPropDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPaxDispPropDlg)
	DDX_Control(pDX, IDC_STATIC_TOOLBARCONTENTER2, m_toolbarcontenter2);
	DDX_Control(pDX, IDC_STATIC_BARFRAME2, m_btnBarFrame2);
	DDX_Control(pDX, IDC_LIST_DISPPROP2, m_listNamedSets);
	DDX_Control(pDX, IDC_STATIC_BARFRAME, m_btnBarFrame);
	DDX_Control(pDX, IDOK, m_btnOk);
	DDX_Control(pDX, IDCANCEL, m_btnCancel);
	DDX_Control(pDX, IDC_BUTTON_SAVE, m_btnSave);
	DDX_Control(pDX, IDC_STATIC_TOOLBARCONTENTER, m_toolbarcontenter);
	DDX_Control(pDX, IDC_LIST_DISPPROP, m_listctrlProp);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPaxDispPropDlg, CDialog)
	//{{AFX_MSG_MAP(CPaxDispPropDlg)
	ON_BN_CLICKED(IDC_BUTTON_SAVE, OnButtonSave)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_COMMAND(ID_TOOLBARBUTTONADD, OnToolbarbuttonadd)
	ON_COMMAND(ID_TOOLBARBUTTONDEL, OnToolbarbuttondel)
	ON_COMMAND(ID_TOOLBARBUTTONEDIT, OnToolbarbuttonedit)
	ON_NOTIFY(NM_CLICK, IDC_LIST_DISPPROP, OnClickListDispprop)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_DISPPROP2, OnItemChangedListPDPSets)
	ON_MESSAGE(WM_PAXPROC_EDIT,OnPaxProcEdit)
	ON_NOTIFY(LVN_ENDLABELEDIT, IDC_LIST_DISPPROP2, OnEndLabelEditListPDPSets)
	ON_WM_CLOSE()
	ON_COMMAND(ID_PEOPLEMOVER_NEW, OnPDPSetAdd)
	ON_COMMAND(ID_PEOPLEMOVER_DELETE, OnPDPSetDelete)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CPaxDispPropDlg message handlers

BOOL CPaxDispPropDlg::OnInitDialog() 
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
	m_listctrlProp.InsertColumn( 3, "Line Type", LVCFMT_CENTER, 150 );
	m_listctrlProp.InsertColumn( 4, "Color", LVCFMT_CENTER, 100 );
	m_listctrlProp.InsertColumn( 5, "Leave Trail", LVCFMT_CENTER, 100 );
	m_listctrlProp.InsertColumn( 6, "Tracer Density", LVCFMT_CENTER, 100 );
	m_listctrlProp.InsertColumn( 7, "Mobile Element Type Description", LVCFMT_LEFT, 300 );

	dwStyle = m_listNamedSets.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_EX_GRIDLINES;
	m_listNamedSets.SetExtendedStyle( dwStyle );
	// Create column
	m_listNamedSets.InsertColumn( 0, "Name", LVCFMT_CENTER, 400 );
	

	// Load data.
	UpdatePDPSetList();
	/////
	m_bNewPDPSet = FALSE;

	InitToolbar();
	m_btnSave.EnableWindow( FALSE );
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}




void CPaxDispPropDlg::OnButtonSave() 
{
	CTermPlanDoc* pDoc = GetDocument();
	pDoc->m_paxDispProps.saveInputs(pDoc->m_ProjInfo.path, true);
}

void CPaxDispPropDlg::OnCancel() 
{
	CTermPlanDoc* pDoc = GetDocument();

	CDialog::OnCancel();

	pDoc->m_paxDispProps.loadInputs(pDoc->m_ProjInfo.path, &(pDoc->GetTerminal())); // reload must after OnCancel
	pDoc->UpdateAllViews(NULL, VM_RELOAD_PAX_DISPLAY_PROPERTY);
// 	UpdatePaxDispPropIn3D();
}

void CPaxDispPropDlg::OnOK() 
{
	CTermPlanDoc* pDoc = GetDocument();
	pDoc->m_paxDispProps.saveInputs(pDoc->m_ProjInfo.path, true);
	
	C3DView *p3DView = pDoc->Get3DView();
	if(p3DView && pDoc->m_eAnimState != CTermPlanDoc::anim_none)
	{
		pDoc->LoadPaxDisplay();
		p3DView->GetParentFrame()->GenPaxShapeDLIDs(&pDoc->m_paxDispProps);
	}
	CDialog::OnOK();
}


void CPaxDispPropDlg::InitToolbar()
{
	CRect rc;
	m_toolbarcontenter.GetWindowRect( &rc );
	ScreenToClient(&rc);
	m_ToolBar.MoveWindow(&rc);
	m_ToolBar.ShowWindow(SW_SHOW);
	m_toolbarcontenter.ShowWindow(SW_HIDE);
	m_ToolBar.GetToolBarCtrl().EnableButton( ID_TOOLBARBUTTONADD );
	m_ToolBar.GetToolBarCtrl().EnableButton( ID_TOOLBARBUTTONDEL );
	m_ToolBar.GetToolBarCtrl().HideButton( ID_TOOLBARBUTTONEDIT );

	m_toolbarcontenter2.GetWindowRect( &rc );
	ScreenToClient(&rc);
	m_ToolBar2.MoveWindow(&rc);
	m_ToolBar2.ShowWindow(SW_SHOW);
	m_toolbarcontenter2.ShowWindow(SW_HIDE);
	m_ToolBar2.GetToolBarCtrl().EnableButton( ID_PEOPLEMOVER_NEW );
	m_ToolBar2.GetToolBarCtrl().EnableButton( ID_PEOPLEMOVER_DELETE );
	m_ToolBar2.GetToolBarCtrl().HideButton( ID_PEOPLEMOVER_CHANGE );
}

int CPaxDispPropDlg::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here
	if (!m_ToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_ToolBar.LoadToolBar(IDR_ADDDELEDITTOOLBAR))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	if (!m_ToolBar2.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_ToolBar2.LoadToolBar(IDR_PEOPLEMOVEBAR))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	return 0;
}

void CPaxDispPropDlg::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here
	CRect btnrc,toolbarrc,rc;
	m_btnSave.GetWindowRect( &btnrc );

	
	m_toolbarcontenter.GetWindowRect( &toolbarrc );

	m_btnBarFrame.MoveWindow( 9,(cy/3)+4,cx-20,(2*cy/3)-25-btnrc.Height() );
	m_toolbarcontenter.MoveWindow( 11,(cy/3)+10,cx-40,toolbarrc.Height() );
	m_toolbarcontenter.GetWindowRect( &rc );
	ScreenToClient(&rc);
	m_toolbarcontenter.ShowWindow( SW_HIDE );
	m_ToolBar.MoveWindow(&rc);
	m_btnBarFrame.GetWindowRect( &rc );
	ScreenToClient(&rc);
	rc.top += 30;
	m_listctrlProp.MoveWindow( 9,(cy/3)+35,rc.Width(),rc.Height() );


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

void CPaxDispPropDlg::OnToolbarbuttonadd() 
{
	// TODO: Add your command handler code here
	CPaxFilterDlg dlg( m_pParentWnd );
	if( dlg.DoModal() == IDCANCEL )
		return;
	else
	{
		CMobileElemConstraint& mobilecon = dlg.GetPaxType().GetPaxCon();
		if (mobilecon.GetTypeIndex() > NOPAX_COUNT)
		{
			::AfxMessageBox(_T("Can not add host type"));
			return;
		}
		CTermPlanDoc* pDoc	= GetDocument();	
        CPaxDispPropItem*  pItem = new CPaxDispPropItem( dlg.GetPaxType().GetName() );
		pItem->SetPaxType( dlg.GetPaxType() );
		pDoc->m_paxDispProps.AddDispPropItem( pItem );
		int nCount = m_listctrlProp.GetItemCount();
		m_listctrlProp.InsertPaxDispItem( nCount, pItem );
		m_listctrlProp.SetItemState( nCount,LVIS_SELECTED,LVIS_SELECTED );
		m_btnSave.EnableWindow();
		UpdatePaxDispPropIn3D();
	}
	
}

void CPaxDispPropDlg::OnToolbarbuttondel() 
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
		CTermPlanDoc* pDoc	= GetDocument();	
		// delete from db
		pDoc->m_paxDispProps.DeleteItem( nItem-2 );
		m_listctrlProp.DeleteItem( nItem );
		UpdatePaxDispPropIn3D();
	}
}


void CPaxDispPropDlg::OnToolbarbuttonedit() 
{
	// TODO: Add your command handler code here
	
}

void CPaxDispPropDlg::OnClickListDispprop(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	if( m_listctrlProp.GetSelectedCount() > 0 )
	{
		m_ToolBar.GetToolBarCtrl().EnableButton( ID_TOOLBARBUTTONDEL );
    	m_ToolBar.GetToolBarCtrl().EnableButton( ID_TOOLBARBUTTONEDIT );

	}
	*pResult = 0;
}

LRESULT CPaxDispPropDlg::OnPaxProcEdit( WPARAM wParam, LPARAM lParam )
{
	int nItem = (int)wParam;
	if( nItem <2 )
		return 0;
	CPaxFilterDlg dlg( m_pParentWnd );
	CPaxDispPropItem* pOldItem = (CPaxDispPropItem*)m_listctrlProp.GetItemData( (int)wParam );
	dlg.SetPaxType( *(pOldItem->GetPaxType()) );
	if( dlg.DoModal() == IDOK )
	{
		CTermPlanDoc* pDoc	= GetDocument();	
		CPaxDispPropItem*  pNewItem = new CPaxDispPropItem( *pOldItem );
		pNewItem->SetPaxType( dlg.GetPaxType() );					
		/*
		pDoc->m_paxDispProps.DeleteItem( nItem-2 );
		pDoc->m_paxDispProps.AddDispPropItem( pItem );
		*/
		pDoc->m_paxDispProps.DeleteAndReplaceItem( nItem-2, pNewItem );
		m_listctrlProp.DeleteItem( nItem );
		m_listctrlProp.InsertPaxDispItem( nItem, pNewItem );
		m_listctrlProp.SetItemState( nItem,LVIS_SELECTED,LVIS_SELECTED );
		m_btnSave.EnableWindow();
		
	}
	return 0;
}
void CPaxDispPropDlg::OnClose() 
{
	//Added by Tim In 2/8/2003*************************************
	//In order to fix that bug(close mobile elements dlg when pax shapes window is showing) must 
	//not call CPaxDispListCtrl::OnDrawItem() when closing this dlg,so call following line .
	m_listctrlProp.DeleteAllItems();
	CDialog::OnClose();
}

void CPaxDispPropDlg::OnItemChangedListPDPSets(NMHDR* pNMHDR, LRESULT* pResult)
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
		m_nSelectedPDPSet = (int) m_listNamedSets.GetItemData(pnmv->iItem);
		CTermPlanDoc* pDoc = GetDocument();
		pDoc->m_paxDispProps.SetCurrentPDPSet(m_nSelectedPDPSet);
	}
	else
		ASSERT(0);

	UpdatePDP();
}


void CPaxDispPropDlg::OnPDPSetAdd()
{
	CTermPlanDoc* pDoc = GetDocument();

	int nPDPSetCount = pDoc->m_paxDispProps.GetPDPSetCount();
	pDoc->m_paxDispProps.AddPDPSet("undefined", new CPaxDispPropItem("DEFAULT"), new CPaxDispPropItem("OVERLAP"));
	m_listNamedSets.InsertItem(LVIF_TEXT | LVIF_PARAM, nPDPSetCount, "         ", 0, 0, 0, nPDPSetCount);
	m_bNewPDPSet = TRUE;
	m_listNamedSets.SetItemState(nPDPSetCount, LVIS_SELECTED, LVIS_SELECTED);
	m_listNamedSets.SetFocus();
	m_listNamedSets.EditLabel(nPDPSetCount);
}

void CPaxDispPropDlg::OnPDPSetDelete()
{
	if(m_listNamedSets.GetItemCount() < 2) {
		AfxMessageBox("At least one Named Property Set must exist.");
		return;
	}
	CTermPlanDoc* pDoc = GetDocument();
	ASSERT(m_nSelectedPDPSet >= 0 && m_nSelectedPDPSet < pDoc->m_paxDispProps.GetPDPSetCount());
	pDoc->m_paxDispProps.DeletPDPSet(m_nSelectedPDPSet);

	m_nSelectedPDPSet = pDoc->m_paxDispProps.GetCurrentPDPSetIdx();

	//m_listNamedSets.SetItemState(m_nSelectedPDPSet, LVIS_SELECTED, LVIS_SELECTED);

	UpdatePDPSetList();
}

void CPaxDispPropDlg::UpdatePDP()
{
	m_listctrlProp.DeleteAllItems();
	CTermPlanDoc* pDoc = GetDocument();
	CPaxDispPropItem* pItem = pDoc->m_paxDispProps.GetDefaultDispProp();
	m_listctrlProp.InsertPaxDispItem( 0, pItem );
	pItem = pDoc->m_paxDispProps.GetOverLapDispProp();
	m_listctrlProp.InsertPaxDispItem( 1, pItem );
	int nCount = pDoc->m_paxDispProps.GetCount();
	for( int i=0; i<nCount; i++ )
	{
		pItem = pDoc->m_paxDispProps.GetDispPropItem( i );
		m_listctrlProp.InsertPaxDispItem( i + 2, pItem );
	}
}

void CPaxDispPropDlg::OnEndLabelEditListPDPSets(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
	*pResult = 0;

	CTermPlanDoc* pDoc = GetDocument();

	if(!pDispInfo->item.pszText) {
		if(m_bNewPDPSet) {
			//delete this new pdp set
			OnPDPSetDelete();	
		}
		return;
	}

	
	m_bNewPDPSet=FALSE;

	int nIdx = pDispInfo->item.iItem;

	ASSERT(pDoc->m_paxDispProps.GetCurrentPDPSetIdx() == nIdx);
	pDoc->m_paxDispProps.RenameCurrentPDPSet(pDispInfo->item.pszText);

	m_listNamedSets.SetItemText(nIdx, 0, pDispInfo->item.pszText);

	*pResult =1;
}

void CPaxDispPropDlg::UpdatePDPSetList()
{
	CTermPlanDoc* pDoc	= GetDocument();

	m_listNamedSets.DeleteAllItems();

	int nPDPSetCount = pDoc->m_paxDispProps.GetPDPSetCount();
	for(int i=0; i<nPDPSetCount; i++) {
		m_listNamedSets.InsertItem(LVIF_TEXT | LVIF_PARAM, i, pDoc->m_paxDispProps.GetPDPSetName(i), 0, 0, 0, (LPARAM) i);
	}
	m_nSelectedPDPSet = pDoc->m_paxDispProps.GetCurrentPDPSetIdx();
	m_listNamedSets.SetItemState(m_nSelectedPDPSet, LVIS_SELECTED, LVIS_SELECTED );
}

CTermPlanDoc* CPaxDispPropDlg::GetDocument() const
{
	if (m_pParentWnd->IsKindOf(RUNTIME_CLASS(CView)))
	{
		CDocument* pDoc	= ((CView*)m_pParentWnd)->GetDocument();
		if (pDoc && pDoc->IsKindOf(RUNTIME_CLASS(CTermPlanDoc)))
		{
			return (CTermPlanDoc*)pDoc;
		}
	}
	ASSERT(FALSE);
	return NULL;
}
void CPaxDispPropDlg::UpdatePaxDispPropIn3D() const
{
	CTermPlanDoc* pDoc = GetDocument();
	if (pDoc)
	{
		pDoc->UpdateAllViews(NULL, VM_RELOAD_PAX_DISPLAY_PROPERTY);
	}
}


LRESULT CPaxDispPropDlg::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	// TODO: Add your specialized code here and/or call the base class
	if (PAX_DISP_PROP_MODIFICATION1 == message)
	{
		UpdatePaxDispPropIn3D();
	}
	return CDialog::DefWindowProc(message, wParam, lParam);
}
