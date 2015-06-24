// PaxDispPropDlg.cpp : implementation file
//

#include "stdafx.h"
#include "termplan.h"
#include "VehicleDispPropDlg.h"
#include "TermPlanDoc.h"
#include "PaxFilterDlg.h"
#include "..\common\winmsg.h"
#include ".\vehicledisppropdlg.h"

#include "ViewMsg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPaxDispPropDlg dialog


CVehicleDispPropDlg::CVehicleDispPropDlg(int nProjID,CWnd* pParent)
: CDialog(CVehicleDispPropDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CVehicleDispPropDlg)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_nProjID = nProjID;
	m_nSelectedPDPSet = -1;
	m_nSelectedPropItem = -1;
	m_bNewPDPSet = FALSE;
}


void CVehicleDispPropDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CVehicleDispPropDlg)
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


BEGIN_MESSAGE_MAP(CVehicleDispPropDlg, CDialog)
	//{{AFX_MSG_MAP(CVehicleDispPropDlg)
	ON_BN_CLICKED(IDC_BUTTON_SAVE, OnButtonSave)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_COMMAND(ID_TOOLBARBUTTONADD, OnToolbarbuttonadd)
	ON_COMMAND(ID_TOOLBARBUTTONDEL, OnToolbarbuttondel)
	ON_COMMAND(ID_TOOLBARBUTTONEDIT, OnToolbarbuttonedit)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_DISPPROP, OnClickListDispprop)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_DISPPROP2, OnItemChangedListPDPSets)
	ON_MESSAGE(WM_PAXPROC_EDIT,OnPaxProcEdit)
	ON_NOTIFY(LVN_ENDLABELEDIT, IDC_LIST_DISPPROP2, OnEndLabelEditListPDPSets)
	ON_WM_CLOSE()
	ON_COMMAND(ID_PEOPLEMOVER_NEW, OnPDPSetAdd)
	ON_COMMAND(ID_PEOPLEMOVER_DELETE, OnPDPSetDelete)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CVehicleDispPropDlg message handlers

BOOL CVehicleDispPropDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	// TODO: Add extra initialization here

	DWORD dwStyle = m_listctrlProp.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_EX_GRIDLINES;
	m_listctrlProp.SetExtendedStyle( dwStyle );
	// Create column
	m_listctrlProp.InsertColumn( 0, "Vehicle Type", LVCFMT_CENTER, 100 );
	m_listctrlProp.InsertColumn( 1, "Visible", LVCFMT_CENTER, 100 );
	m_listctrlProp.InsertColumn( 2, "Shape", LVCFMT_CENTER, 100 );
	m_listctrlProp.InsertColumn( 3, "Line Type", LVCFMT_CENTER, 150 );
	m_listctrlProp.InsertColumn( 4, "Color", LVCFMT_CENTER, 100 );
	m_listctrlProp.InsertColumn( 5, "Leave Trail", LVCFMT_CENTER, 100 );
	m_listctrlProp.InsertColumn( 6, "Tracer Density", LVCFMT_CENTER, 100 );

	dwStyle = m_listNamedSets.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_EX_GRIDLINES;
	m_listNamedSets.SetExtendedStyle( dwStyle );
	// Create column
	CStringList strList;

	LV_COLUMNEX lvc;
	lvc.mask = LVCF_WIDTH|LVCF_TEXT;
	lvc.pszText = _T("Name");
	lvc.cx = 100;
	lvc.fmt = LVCFMT_EDIT;
	lvc.csList = &strList;
	m_listNamedSets.InsertColumn(0, &lvc);

	CTermPlanDoc* pDoc = GetDocument();
	if(!pDoc)return FALSE;
	if(pDoc->m_vehicleDispProps.GetVehicleCount() < 1)
		pDoc->m_vehicleDispProps.ReadData(m_nProjID);

	// Load data.
	UpdatePDPSetList();
	/////
	m_bNewPDPSet = FALSE;

	SetWindowText(_T("Vehicle Property Display"));
	InitToolbar();
	m_btnSave.EnableWindow( FALSE );

	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),TRUE);
	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CVehicleDispPropDlg::OnButtonSave() 
{
	for (std::vector<CVehicleDispPropItem *>::iterator itrDeletedItem = m_vrDeletedItem.begin();\
		itrDeletedItem != m_vrDeletedItem.end();++itrDeletedItem) {
			if(*itrDeletedItem)
			{
				(*itrDeletedItem)->DeleteData();
				delete (*itrDeletedItem);
			}
	}
	for (std::vector<CVehicleDispPropNode *>::iterator itrDeletedNode = m_vrDeletedNode.begin();\
		itrDeletedNode != m_vrDeletedNode.end();++itrDeletedNode) {
			if(*itrDeletedNode)
			{
				(*itrDeletedNode)->DeleteData();
				delete (*itrDeletedNode);
			}
	}
	CTermPlanDoc* pDoc = GetDocument();
	if(!pDoc)return;
	try
	{
		CADODatabase::BeginTransaction() ;
		pDoc->m_vehicleDispProps.SaveData();
		CADODatabase::CommitTransaction() ;

	}
	catch (CADOException e)
	{
		CADODatabase::RollBackTransation() ;
	}
	
}

void CVehicleDispPropDlg::OnCancel() 
{
	CDialog::OnCancel();

	CTermPlanDoc* pDoc = GetDocument();
	if(!pDoc)return;
	pDoc->m_vehicleDispProps.ReadData(m_nProjID);
	UpdateVehicleDispPropIn3D();
}

void CVehicleDispPropDlg::OnOK() 
{
	OnButtonSave();
	CDialog::OnOK();
}


void CVehicleDispPropDlg::InitToolbar()
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

int CVehicleDispPropDlg::OnCreate(LPCREATESTRUCT lpCreateStruct) 
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

void CVehicleDispPropDlg::OnSize(UINT nType, int cx, int cy) 
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

void CVehicleDispPropDlg::OnToolbarbuttonadd() 
{	
	CTermPlanDoc* pDoc	= GetDocument();	
	if(!pDoc)return;
	CVehicleDispPropNode* pNode = pDoc->m_vehicleDispProps.GetVehicleDispProp(m_nSelectedPDPSet);
	if(!pNode)return;
	CVehicleDispPropItem* pItem = new CVehicleDispPropItem(m_nProjID);
	if(!pItem)return;
	pNode->AddVehicleDispPropItem(pItem);
	int nCount = m_listctrlProp.GetItemCount();
	m_listctrlProp.InsertPaxDispItem( nCount, pItem );
	m_listctrlProp.SetItemState( nCount,LVIS_SELECTED,LVIS_SELECTED );
	m_btnSave.EnableWindow();
	UpdateVehicleDispPropIn3D();
}

void CVehicleDispPropDlg::OnToolbarbuttondel() 
{
	// TODO: Add your command handler code here
	POSITION pos = m_listctrlProp.GetFirstSelectedItemPosition();
	if (pos == NULL)
		return;
	else
	{
		int nItem = m_listctrlProp.GetNextSelectedItem(pos);
		if (nItem == 0)
		{
			MessageBox( "Can't delete the Default item!" );
			return;
		}
		CTermPlanDoc* pDoc	= GetDocument();	
		// delete from db
		CVehicleDispPropNode* pNode = pDoc->m_vehicleDispProps.GetVehicleDispProp(m_nSelectedPDPSet);
		if(!pNode)return;
		m_vrDeletedItem.push_back(pNode->DeleteVehicleDispPropItem(nItem)); 
		m_listctrlProp.DeleteItem( nItem );
		m_listctrlProp.SetItemState(pNode->GetCulSelItem(), LVIS_SELECTED, LVIS_SELECTED );
	}
}


void CVehicleDispPropDlg::OnToolbarbuttonedit() 
{
	// TODO: Add your command handler code here

}

void CVehicleDispPropDlg::OnClickListDispprop(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	if( m_listctrlProp.GetSelectedCount() > 0 )
	{
		m_ToolBar.GetToolBarCtrl().EnableButton( ID_TOOLBARBUTTONDEL );
		m_ToolBar.GetToolBarCtrl().EnableButton( ID_TOOLBARBUTTONEDIT );
		CTermPlanDoc* pDoc	= GetDocument();	
		if(!pDoc)return;
		CVehicleDispPropNode* pNode = pDoc->m_vehicleDispProps.GetVehicleDispProp(m_nSelectedPDPSet);
		if(!pNode)return;
		pNode->SetCulSelItem(m_listctrlProp.GetNextItem(-1,LVNI_SELECTED));
	}
	*pResult = 0;
}

LRESULT CVehicleDispPropDlg::OnPaxProcEdit( WPARAM wParam, LPARAM lParam )
{
	//int nItem = (int)wParam;
	//if( nItem <2 )
	//	return 0;
	//CPaxFilterDlg dlg( m_pParentWnd );
	//CPaxDispPropItem* pOldItem = (CPaxDispPropItem*)m_listctrlProp.GetItemData( (int)wParam );
	//dlg.SetPaxType( *(pOldItem->GetPaxType()) );
	//if( dlg.DoModal() == IDOK )
	//{
	//	CTermPlanDoc* pDoc	= GetDocument();	
	//	CPaxDispPropItem*  pNewItem = new CPaxDispPropItem( *pOldItem );
	//	pNewItem->SetPaxType( dlg.GetPaxType() );					
	//	/*
	//	pDoc->m_paxDispProps.DeleteItem( nItem-2 );
	//	pDoc->m_paxDispProps.AddDispPropItem( pItem );
	//	*/
	//	pDoc->m_paxDispProps.DeleteAndReplaceItem( nItem-2, pNewItem );
	//	m_listctrlProp.DeleteItem( nItem );
	//	m_listctrlProp.InsertPaxDispItem( nItem, pNewItem );
	//	m_listctrlProp.SetItemState( nItem,LVIS_SELECTED,LVIS_SELECTED );
	//	m_btnSave.EnableWindow();

	//}
	return 0;
}
void CVehicleDispPropDlg::OnClose() 
{
	//Added by Tim In 2/8/2003*************************************
	//In order to fix that bug(close mobile elements dlg when pax shapes window is showing) must 
	//not call CPaxDispListCtrl::OnDrawItem() when closing this dlg,so call following line .
	m_listctrlProp.DeleteAllItems();
	CDialog::OnClose();
}

void CVehicleDispPropDlg::OnItemChangedListPDPSets(NMHDR* pNMHDR, LRESULT* pResult)
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
		pDoc->m_vehicleDispProps.SetCurSelVehicle(m_nSelectedPDPSet);
	}
	else
		ASSERT(0);

	UpdatePDP();
}


void CVehicleDispPropDlg::OnPDPSetAdd()
{
	CTermPlanDoc* pDoc = GetDocument();
	if(!pDoc)return;
	int nVehiclesCount = pDoc->m_vehicleDispProps.GetVehicleCount(); 
	pDoc->m_vehicleDispProps.AddVehicleDispProp(_T("Default"));	
	m_listNamedSets.InsertItem(LVIF_TEXT | LVIF_PARAM, nVehiclesCount, _T("Default"), 0, 0, 0, nVehiclesCount);
	m_bNewPDPSet = TRUE;
	m_listNamedSets.SetItemState(nVehiclesCount, LVIS_SELECTED, LVIS_SELECTED);
	pDoc->m_vehicleDispProps.SetCurSelVehicle(nVehiclesCount);
	pDoc->m_vehicleDispProps.GetVehicleDispProp(pDoc->m_vehicleDispProps.GetVehicleCount()-1)->SetCulSelItem(0);
	m_listNamedSets.SetFocus();
	m_listNamedSets.EditLabel(nVehiclesCount);

	UpdatePDPSetList();
}

void CVehicleDispPropDlg::OnPDPSetDelete()
{
	if(m_listNamedSets.GetItemCount() < 2) {
		AfxMessageBox("At least one Named Property Set must exist.");
		return;
	}
	CTermPlanDoc* pDoc = GetDocument();
	ASSERT(m_nSelectedPDPSet >= 0 && m_nSelectedPDPSet < pDoc->m_vehicleDispProps.GetVehicleCount());
	m_vrDeletedNode.push_back(pDoc->m_vehicleDispProps.DeleteVehicleDispProp(m_nSelectedPDPSet));
	m_nSelectedPDPSet = pDoc->m_vehicleDispProps.GetCurSelVehicle();
	m_listNamedSets.SetItemState(m_nSelectedPDPSet, LVIS_SELECTED, LVIS_SELECTED );

	UpdatePDPSetList();
}

void CVehicleDispPropDlg::UpdatePDP()
{
	m_listctrlProp.DeleteAllItems();  
	CTermPlanDoc* pDoc = GetDocument();
	if(!pDoc)return;
	CVehicleDispPropNode* pNode = pDoc->m_vehicleDispProps.GetVehicleDispProp(m_nSelectedPDPSet);
	if(!pNode)return;
	int nCount = pNode->GetVehicleDispPropCount();
	for( int i=0; i< nCount; i++ )
	{
		CVehicleDispPropItem * pItem = pNode->GetVehicleDispProp(i);
		m_listctrlProp.InsertPaxDispItem( i, pItem );
	}
	m_listctrlProp.SetItemState(pNode->GetCulSelItem(), LVIS_SELECTED, LVIS_SELECTED );
}

void CVehicleDispPropDlg::OnEndLabelEditListPDPSets(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
	*pResult = 0;

	CTermPlanDoc* pDoc = GetDocument();

	if(!pDispInfo->item.pszText) {		
		return;
	}


	m_bNewPDPSet=FALSE;

	int nIdx = pDispInfo->item.iItem;

	ASSERT(pDoc->m_vehicleDispProps.GetCurSelVehicle() == nIdx);
	pDoc->m_vehicleDispProps.GetVehicleDispProp(nIdx)->SetVehicleNodeName(pDispInfo->item.pszText);

	m_listNamedSets.SetItemText(nIdx, 0, pDispInfo->item.pszText);

	*pResult =1;
}

void CVehicleDispPropDlg::UpdatePDPSetList()
{
	CTermPlanDoc* pDoc	= GetDocument();

	m_listNamedSets.DeleteAllItems();
	int nVehicleCount = pDoc->m_vehicleDispProps.GetVehicleCount();
	for(int i = 0; i < nVehicleCount; i++) {
		m_listNamedSets.InsertItem(LVIF_TEXT | LVIF_PARAM, i, pDoc->m_vehicleDispProps.GetVehicleDispProp(i)->GetVehicleNodeName(), 0, 0, 0, (LPARAM) i);
	}
	m_nSelectedPDPSet = pDoc->m_vehicleDispProps.GetCurSelVehicle();
	m_listNamedSets.SetItemState(m_nSelectedPDPSet, LVIS_SELECTED, LVIS_SELECTED );
	UpdatePDP();
}

CTermPlanDoc* CVehicleDispPropDlg::GetDocument() const
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

void CVehicleDispPropDlg::UpdateVehicleDispPropIn3D() const
{
	GetDocument()->UpdateAllViews(NULL, VM_RELOAD_VEHICLE_DISPLAY_PROPERTY);
}
LRESULT CVehicleDispPropDlg::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	// TODO: Add your specialized code here and/or call the base class
	if (message == VEHICLE_DISP_PROP_CHANGE1)
	{
		UpdateVehicleDispPropIn3D();
	}

	return CDialog::DefWindowProc(message, wParam, lParam);
}
