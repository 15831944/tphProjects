// DlgAircraftTags.cpp : implementation file
//

#include "stdafx.h"
#include "DlgAircraftTags.h"

#include "AircraftFilterDlg.h"
#include "TermPlanDoc.h"
#include ".\dlgaircrafttags.h"

#include "ViewMsg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



static const UINT checkboxIDs[] = {
	IDC_CHK_SHOWTAG,	
	IDC_CHK_FLIGHTID,
	IDC_CHK_OPERATION_MODE,
	IDC_CHK_SPEED,
	IDC_CHK_GATE,	
	IDC_CHK_RUNWAY,
	IDC_CHK_SIDSTAR,
	IDC_CHK_ORIGDEST,
	IDC_CHK_ACTYPE,		
	IDC_CHK_ALTITUDE,
	IDC_CHK_STATE,
	//IDC_CHK_DELAY,
};

/////////////////////////////////////////////////////////////////////////////
// CDlgAircraftTags dialog


CDlgAircraftTags::CDlgAircraftTags(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgAircraftTags::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgAircraftTags)
	m_bShowTag = FALSE;
	m_bChkFlightID = FALSE;
	m_bChkACType = FALSE;
	m_bChkOrigDest = FALSE;

	//}}AFX_DATA_INIT
	m_nSelectedSet = -1;
	m_bNewSet = FALSE;
}


void CDlgAircraftTags::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgAircraftTags)
	DDX_Control(pDX, IDC_STATIC_TAGPROPERTIES, m_btnTagProperties);
	DDX_Control(pDX, IDC_STATIC_PAXTYPES, m_btnPaxTypes);
	DDX_Control(pDX, IDCANCEL, m_btnCancel);
	DDX_Control(pDX, IDOK, m_btnOk);
	DDX_Control(pDX, IDC_STATIC_TOOLBARCONTENTER2, m_toolbarcontenter2);
	DDX_Control(pDX, IDC_LIST_DISPPROP, m_lstSets);
	DDX_Control(pDX, IDC_STATIC_BARFRAME2, m_btnBarFrame2);
	DDX_Control(pDX, IDC_STATIC_BARFRAME, m_btnBarFrame);
	DDX_Control(pDX, IDC_LIST_PAXTYPES, m_lstFlightTypes);
	DDX_Control(pDX, IDC_BUTTON_SAVE, m_btnSave);
	DDX_Control(pDX, IDC_STATIC_TOOLBARCONTENTER, m_toolbarcontenter);
	DDX_Check(pDX, IDC_CHK_SHOWTAG, m_bShowTag);
	DDX_Check(pDX, IDC_CHK_FLIGHTID, m_bChkFlightID);
	DDX_Check(pDX, IDC_CHK_ACTYPE, m_bChkACType);
	DDX_Check(pDX, IDC_CHK_ORIGDEST, m_bChkOrigDest);
	DDX_Check(pDX, IDC_CHK_OPERATION_MODE, m_bChkOpMode);
	DDX_Check(pDX, IDC_CHK_GATE, m_bChkGate);
	DDX_Check(pDX, IDC_CHK_RUNWAY, m_bChkRunway);
	DDX_Check(pDX, IDC_CHK_SIDSTAR, m_bChkSidStar);
	DDX_Check(pDX, IDC_CHK_SPEED, m_bChkSpeed);
	DDX_Check(pDX, IDC_CHK_STATE, m_bChkState);
	DDX_Check(pDX, IDC_CHK_ALTITUDE, m_bChkAltitude);
	//DDX_Check(pDX, IDC_CHK_DELAY, m_bChkDelay);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgAircraftTags, CDialog)
	//{{AFX_MSG_MAP(CDlgAircraftTags)
	ON_WM_CREATE()
	ON_COMMAND(ID_TOOLBARBUTTONADD, OnToolbarbuttonadd)
	ON_COMMAND(ID_TOOLBARBUTTONDEL, OnToolbarbuttondel)
	ON_NOTIFY(LVN_GETDISPINFO, IDC_LIST_PAXTYPES, OnGetDispInfo)
	ON_NOTIFY(NM_CLICK, IDC_LIST_PAXTYPES, OnListItemClick)
	ON_BN_CLICKED(IDC_CHK_SHOWTAG, OnChkShowtag)
	ON_BN_CLICKED(IDC_CHK_FLIGHTID, OnChkFlightId)
	ON_BN_CLICKED(IDC_CHK_OPERATION_MODE, OnChkOperationMode)
	ON_BN_CLICKED(IDC_CHK_SPEED, OnChkSpeed)
	ON_BN_CLICKED(IDC_CHK_GATE, OnChkGate)
	ON_BN_CLICKED(IDC_CHK_RUNWAY, OnChkRunway)
	ON_BN_CLICKED(IDC_CHK_SIDSTAR, OnChkSidStar)
	ON_BN_CLICKED(IDC_CHK_ORIGDEST, OnChkOrigDest)
	ON_BN_CLICKED(IDC_CHK_ACTYPE, OnChkACType)
	ON_BN_CLICKED(IDC_CHK_ALTITUDE, OnChkAltitude)
	ON_BN_CLICKED(IDC_CHK_STATE, OnChkState)

	ON_BN_CLICKED(IDC_BUTTON_SAVE, OnSave)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_DISPPROP, OnItemChangedListSets)
	ON_COMMAND(ID_PEOPLEMOVER_NEW, OnAddSet)
	ON_COMMAND(ID_PEOPLEMOVER_DELETE, OnDeleteSet)
	ON_NOTIFY(LVN_ENDLABELEDIT, IDC_LIST_DISPPROP, OnEndLabelEditListSets)	
END_MESSAGE_MAP()


void CDlgAircraftTags::EnableAllCheckboxes(BOOL bEnable)
{
	CWnd* pWnd = GetDlgItem(IDC_CHK_SHOWTAG);
	pWnd->EnableWindow(bEnable);
	EnableSubCheckboxes(bEnable);
}

void CDlgAircraftTags::EnableSubCheckboxes(BOOL bEnable)
{
	CWnd* pWnd = NULL;
	int nCount = sizeof(checkboxIDs)/sizeof(UINT);
	for(int i=1;i< nCount; i++)
	{
		pWnd = GetDlgItem(checkboxIDs[i]);
		pWnd->EnableWindow(bEnable);		
	}
	
}

void CDlgAircraftTags::InitToolbar()
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
void CDlgAircraftTags::SaveCurrentData()
{
	POSITION pos = m_lstFlightTypes.GetFirstSelectedItemPosition();
	int idx = m_lstFlightTypes.GetNextSelectedItem(pos);
	if(idx!=-1)
	{
		DWORD taginfo = 0x00000000;
		taginfo += m_bChkFlightID?ACTAG_FLIGHTID:0x00000000;
		taginfo += m_bChkOpMode?  ACTAG_OPERATEMODE:0x00000000;
		taginfo += m_bChkSpeed?   ACTAG_SPEED:0x00000000;
		taginfo += m_bChkGate?    ACTAG_GATE:0x00000000;
		taginfo += m_bChkRunway?  ACTAG_RUNWAY:0x00000000;
		taginfo += m_bChkSidStar? ACTAG_SIDSTAR:0x00000000;
		taginfo += m_bChkOrigDest?ACTAG_ORIGDEST:0x00000000;
		taginfo += m_bChkACType?  ACTAG_ACTYPE:0x00000000;
		taginfo += m_bChkAltitude?ACTAG_ALTITUDEC:0x00000000;
		taginfo += m_bChkState?   ACTAG_STATE:0x00000000;

		CAircraftTagItem* pItem = (CAircraftTagItem*) m_lstFlightTypes.GetItemData(idx);
		pItem->SetTagInfo(taginfo);
	}
}
void CDlgAircraftTags::SetAllCheckboxes()
{
	POSITION pos = m_lstFlightTypes.GetFirstSelectedItemPosition();
	int idx = m_lstFlightTypes.GetNextSelectedItem(pos);
	if(idx != -1) {
		//enable "show tags" checkbox
		CWnd* pWnd = GetDlgItem(IDC_CHK_SHOWTAG);
		pWnd->EnableWindow(TRUE);
		//get item data
		CAircraftTagItem* pItem = (CAircraftTagItem*) m_lstFlightTypes.GetItemData(idx);
		ASSERT(pItem != NULL);
		//show tags?
		m_bShowTag = pItem->IsShowTags();
		EnableSubCheckboxes(m_bShowTag);
		DWORD dwTagInfo = pItem->GetTagInfo();
		m_bChkFlightID = dwTagInfo & ACTAG_FLIGHTID?TRUE:FALSE;
		m_bChkOpMode = dwTagInfo & ACTAG_OPERATEMODE?TRUE:FALSE;
		m_bChkSpeed = dwTagInfo & ACTAG_SPEED?TRUE:FALSE;
		m_bChkGate = dwTagInfo & ACTAG_GATE?TRUE:FALSE;
		m_bChkRunway = dwTagInfo & ACTAG_RUNWAY?TRUE:FALSE;
		m_bChkSidStar = dwTagInfo & ACTAG_SIDSTAR?TRUE:FALSE;
		m_bChkOrigDest = dwTagInfo & ACTAG_ORIGDEST?TRUE:FALSE;
		m_bChkACType = dwTagInfo & ACTAG_ACTYPE?TRUE:FALSE;
		m_bChkAltitude = dwTagInfo & ACTAG_ALTITUDEC?TRUE:FALSE;
		m_bChkState = dwTagInfo & ACTAG_STATE?TRUE:FALSE;
		
		UpdateData(FALSE);
	}
	else {
		EnableAllCheckboxes(FALSE);
	}
}

/////////////////////////////////////////////////////////////////////////////
// CDlgAircraftTags message handlers

BOOL CDlgAircraftTags::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	DWORD dwStyle = m_lstFlightTypes.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_EX_GRIDLINES;
	m_lstFlightTypes.SetExtendedStyle( dwStyle );
	// Create column
	m_lstFlightTypes.InsertColumn( 0, "Name", LVCFMT_CENTER, 100 );
	m_lstFlightTypes.InsertColumn( 1, "Flight Type Description", LVCFMT_LEFT, 400 );

	dwStyle = m_lstSets.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_EX_GRIDLINES;
	m_lstSets.SetExtendedStyle( dwStyle );
	// Create column
	m_lstSets.InsertColumn( 0, "Name", LVCFMT_CENTER, 400 );

	UpdateSetList();

	InitToolbar();
	SetAllCheckboxes();
	m_btnSave.EnableWindow( FALSE );

	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),TRUE);
	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

int CDlgAircraftTags::OnCreate(LPCREATESTRUCT lpCreateStruct) 
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
		TRACE0("Failed to create toolbar 2\n");
		return -1;      // fail to create
	}

	return 0;
}

void CDlgAircraftTags::OnToolbarbuttonadd() 
{
	CAircraftFilterDlg dlg( m_pParentWnd );
	if( dlg.DoModal() == IDCANCEL )
		return;
	else
	{
		CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();
		CAircraftTagItem* pItem = new CAircraftTagItem( dlg.GetFlightType().GetName() );
		pItem->SetFlightType( dlg.GetFlightType() );
		pDoc->m_aircraftTags.AddTagItem( pItem );
		int nCount = m_lstFlightTypes.GetItemCount();

		LVITEM lvi; 
		lvi.mask = LVIF_PARAM | LVIF_TEXT;
		lvi.iSubItem = 0;
		lvi.pszText = LPSTR_TEXTCALLBACK;
		lvi.iItem = nCount;
		lvi.lParam = (LPARAM) pItem;
		m_lstFlightTypes.InsertItem(&lvi);
		m_lstFlightTypes.SetItemState( nCount,LVIS_SELECTED,LVIS_SELECTED );
		SetAllCheckboxes();

		m_btnSave.EnableWindow();
		UpdateAircrafTagIn3D();
	}
}

void CDlgAircraftTags::OnToolbarbuttondel() 
{
	POSITION pos = m_lstFlightTypes.GetFirstSelectedItemPosition();
	if(pos == NULL)
		return;
	int idx = m_lstFlightTypes.GetNextSelectedItem(pos);
	if(idx != -1) {
		if(idx == 0) {
			AfxMessageBox("Cannot delete the item \"DEFAULT\"");
			return;
		}
		else if(idx == 1) {
			AfxMessageBox("Cannot delete the item \"OVERLAP\"");
			return;
		}
		else {
			CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();	
			// delete from db
			pDoc->m_aircraftTags.DeleteItem(idx-2);
			m_lstFlightTypes.DeleteItem(idx);
			SetAllCheckboxes();
			UpdateAircrafTagIn3D();
		}
	}
}

void CDlgAircraftTags::OnGetDispInfo(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
	if(pDispInfo->item.mask & LVIF_TEXT) {
		CAircraftTagItem* pItem = (CAircraftTagItem*) pDispInfo->item.lParam;
		switch(pDispInfo->item.iSubItem) {
		case 0: //name
			lstrcpy(pDispInfo->item.pszText, pItem->GetFlightType()->GetName());
			break;
		case 1: //flight type description
			lstrcpy(pDispInfo->item.pszText, pItem->GetFlightType()->ScreenPrint());
			break;
		}
	}
	*pResult = 0;
}
/*
BOOL CDlgAircraftTags::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
{
	int i;
	NMHDR* pNMHDR = (NMHDR*)lParam;
	switch(pNMHDR->code) {
	case LVN_ITEMACTIVATE:
		i++;
		break;
	default:
		break;
	}
	
	return CDialog::OnNotify(wParam, lParam, pResult);
}
*/

void CDlgAircraftTags::OnListItemClick(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NMITEMACTIVATE* pNMIA = (NMITEMACTIVATE*) pNMHDR;

	SetAllCheckboxes();
	
	*pResult = 0;
}

void CDlgAircraftTags::OnChkShowtag() 
{
	UpdateData();
	POSITION pos = m_lstFlightTypes.GetFirstSelectedItemPosition();
	int idx = m_lstFlightTypes.GetNextSelectedItem(pos);
	if(idx != -1) {
		//get item data
		CAircraftTagItem* pItem = (CAircraftTagItem*) m_lstFlightTypes.GetItemData(idx);
		ASSERT(pItem != NULL);
		//set "show tags" data
		pItem->SetShowTags(m_bShowTag);
		UpdateAircrafTagIn3D();
	}
	SetAllCheckboxes();
}

void CDlgAircraftTags::OnChkItem(DWORD dwMask, BOOL bIsCheck)
{
	POSITION pos = m_lstFlightTypes.GetFirstSelectedItemPosition();
	int idx = m_lstFlightTypes.GetNextSelectedItem(pos);
	if(idx != -1) {
		//get item data
		CAircraftTagItem* pItem = (CAircraftTagItem*) m_lstFlightTypes.GetItemData(idx);
		ASSERT(pItem != NULL);
		//set data
		DWORD dwTagInfo = pItem->GetTagInfo();
		if(bIsCheck)
			dwTagInfo = dwTagInfo | dwMask;
		else
			dwTagInfo = dwTagInfo ^ dwMask;
		pItem->SetTagInfo(dwTagInfo);
	}
	else {
		ASSERT(FALSE);
	}
}

void CDlgAircraftTags::OnChkFlightId() 
{
	UpdateData();
	OnChkItem(ACTAG_FLIGHTID, m_bChkFlightID);
	SaveCurrentData();
	UpdateAircrafTagIn3D();
}
void CDlgAircraftTags::OnChkOperationMode() 
{
	UpdateData();
	OnChkItem(ACTAG_OPERATEMODE, m_bChkOpMode);
	SaveCurrentData();
	UpdateAircrafTagIn3D();
}
void CDlgAircraftTags::OnChkSpeed() 
{
	UpdateData();
	OnChkItem(ACTAG_SPEED, m_bChkSpeed);
	SaveCurrentData();
	UpdateAircrafTagIn3D();
}
void CDlgAircraftTags::OnChkGate() 
{
	UpdateData();
	OnChkItem(ACTAG_GATE, m_bChkGate);
	SaveCurrentData();
	UpdateAircrafTagIn3D();
}

void CDlgAircraftTags::OnChkRunway() 
{
	UpdateData();
	OnChkItem(ACTAG_RUNWAY, m_bChkRunway);
	SaveCurrentData();
	UpdateAircrafTagIn3D();
}

void CDlgAircraftTags::OnChkSidStar() 
{
	UpdateData();
	OnChkItem(ACTAG_SIDSTAR, m_bChkSidStar);
	SaveCurrentData();
	UpdateAircrafTagIn3D();
}

void CDlgAircraftTags::OnChkOrigDest() 
{
	UpdateData();
	OnChkItem(ACTAG_ORIGDEST, m_bChkOrigDest);
	SaveCurrentData();
	UpdateAircrafTagIn3D();
}

void CDlgAircraftTags::OnChkACType() 
{
	UpdateData();
	OnChkItem(ACTAG_ACTYPE, m_bChkACType);
	SaveCurrentData();
	UpdateAircrafTagIn3D();
}
void CDlgAircraftTags::OnChkAltitude() 
{
	UpdateData();
	OnChkItem(ACTAG_ALTITUDEC, m_bChkAltitude);
	SaveCurrentData();
	UpdateAircrafTagIn3D();
}
void CDlgAircraftTags::OnChkState() 
{
	UpdateData();
	OnChkItem(ACTAG_STATE, m_bChkState);
	SaveCurrentData();
	UpdateAircrafTagIn3D();
}

void CDlgAircraftTags::OnOK() 
{
	CTermPlanDoc* pDoc = (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();
	pDoc->m_aircraftTags.saveInputs(pDoc->m_ProjInfo.path, true);
	
	CDialog::OnOK();
}

void CDlgAircraftTags::OnSave() 
{
	CTermPlanDoc* pDoc = (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();
	pDoc->m_aircraftTags.saveInputs(pDoc->m_ProjInfo.path, true);
}

void CDlgAircraftTags::OnCancel() 
{
	CDialog::OnCancel();

	CTermPlanDoc* pDoc = (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();
	pDoc->m_aircraftTags.loadInputs(pDoc->m_ProjInfo.path, &(pDoc->GetTerminal()));
}

void CDlgAircraftTags::OnSize(UINT nType, int cx, int cy) 
{
	if(cx < 390) cx=390;
	if(cy < 465) cy=465;

	CDialog::OnSize(nType, cx, cy);
	// TRACE("OnSize(%d, %d)\n", cx, cy);
	
	CRect btnrc,toolbarrc,rc;
	m_btnSave.GetWindowRect( &btnrc );

	
	m_toolbarcontenter.GetWindowRect( &toolbarrc );

	int onethirdsheight = cy/3;
	int halfwidth = cx/2;

	m_btnPaxTypes.MoveWindow(9, onethirdsheight+4, halfwidth-18, (2*onethirdsheight)-25-btnrc.Height());
	m_btnBarFrame.MoveWindow( 15, onethirdsheight+20,halfwidth-30,(2*onethirdsheight)-45-btnrc.Height() );
	m_toolbarcontenter.MoveWindow( 17,onethirdsheight+26,halfwidth-50,toolbarrc.Height() );
	m_toolbarcontenter.GetWindowRect( &rc );
	ScreenToClient(&rc);
	m_toolbarcontenter.ShowWindow( SW_HIDE );
	m_ToolBar.MoveWindow(&rc);
	m_btnBarFrame.GetWindowRect( &rc );
	ScreenToClient(&rc);
	rc.top += 32;
	m_lstFlightTypes.MoveWindow( rc.left,rc.top,rc.Width(),rc.Height() );

	m_btnTagProperties.MoveWindow(halfwidth+9, onethirdsheight+4, halfwidth-18, (2*onethirdsheight)-25-btnrc.Height());
	int nCount = sizeof(checkboxIDs)/sizeof(UINT);
	// TRACE("size of checkboxIDs array: %d\n", nCount);
	CRect rcCheck;
	CWnd* pWnd = GetDlgItem(checkboxIDs[0]);
	pWnd->GetWindowRect(&rcCheck);
	pWnd->MoveWindow(halfwidth+40, onethirdsheight+55, rcCheck.Width(), rcCheck.Height());
	for(int i=1; i<nCount; i++) {
		pWnd = GetDlgItem(checkboxIDs[i]);
		pWnd->MoveWindow(halfwidth+75, onethirdsheight+55+i*(rcCheck.Height()+5), rcCheck.Width()+30, rcCheck.Height());
	}


	m_btnBarFrame2.MoveWindow( 9,4,cx-20,onethirdsheight-25 );
	m_toolbarcontenter2.MoveWindow( 11,10,cx-40,toolbarrc.Height() );
	m_toolbarcontenter2.GetWindowRect( &rc );
	ScreenToClient(&rc);
	m_toolbarcontenter2.ShowWindow( SW_HIDE );
	m_ToolBar2.MoveWindow(&rc);
	m_btnBarFrame2.GetWindowRect( &rc );
	ScreenToClient(&rc);
	rc.top = 35;
	m_lstSets.MoveWindow( 9,35,rc.Width(),rc.Height() );
	
	m_btnSave.MoveWindow( cx-20-3*btnrc.Width(),cy-10-btnrc.Height(),btnrc.Width(),btnrc.Height() );
	m_btnOk.MoveWindow( cx-15-2*btnrc.Width(),cy-10-btnrc.Height(),btnrc.Width(),btnrc.Height() );
	m_btnCancel.MoveWindow( cx-10-btnrc.Width(),cy-10-btnrc.Height(),btnrc.Width(),btnrc.Height() );

	InvalidateRect(NULL);
	
}

void CDlgAircraftTags::UpdateSetList()
{
	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();

	m_lstSets.DeleteAllItems();

	int nSetCount = pDoc->m_aircraftTags.GetSetCount();
	for(int i=0; i<nSetCount; i++) {
		m_lstSets.InsertItem(LVIF_TEXT | LVIF_PARAM, i, pDoc->m_aircraftTags.GetSetName(i), 0, 0, 0, (LPARAM) i);
	}
	m_nSelectedSet = pDoc->m_aircraftTags.GetCurrentSetIdx();
	m_lstSets.SetItemState(m_nSelectedSet, LVIS_SELECTED, LVIS_SELECTED );

}

void CDlgAircraftTags::UpdateTagData()
{
	m_lstFlightTypes.DeleteAllItems();

	// Load data.
	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();	
	
	LVITEM lvi; 
    lvi.mask = LVIF_PARAM | LVIF_TEXT;
	lvi.iSubItem = 0;
	lvi.pszText = LPSTR_TEXTCALLBACK;
	
	CAircraftTagItem* pItem = pDoc->m_aircraftTags.GetDefaultTags();
	ASSERT(pItem != NULL);
    lvi.iItem = 0;
	lvi.lParam = (LPARAM) pItem;
	m_lstFlightTypes.InsertItem(&lvi);

	pItem = pDoc->m_aircraftTags.GetOverLapTags();
	ASSERT(pItem != NULL);
	lvi.iItem = 1;
	lvi.lParam = (LPARAM) pItem;
	m_lstFlightTypes.InsertItem(&lvi);
	int nCount = pDoc->m_aircraftTags.GetCount();
	for( int i=0; i<nCount; i++ )
	{
		pItem = pDoc->m_aircraftTags.GetTagItem( i );
		lvi.iItem = 2+i;
		lvi.lParam = (LPARAM) pItem;
		m_lstFlightTypes.InsertItem( &lvi );
	}
}

void CDlgAircraftTags::OnItemChangedListSets(NMHDR* pNMHDR, LRESULT* pResult)
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
		m_nSelectedSet = (int) m_lstSets.GetItemData(pnmv->iItem);
		CTermPlanDoc* pDoc = (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();
		pDoc->m_aircraftTags.SetCurrentSet(m_nSelectedSet);
	}
	else
		ASSERT(0);

	UpdateTagData();
}

void CDlgAircraftTags::OnAddSet()
{
	CTermPlanDoc* pDoc = (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();

	int nSetCount = pDoc->m_aircraftTags.GetSetCount();
	pDoc->m_aircraftTags.AddSet("undefined", new CAircraftTagItem("DEFAULT"), new CAircraftTagItem("OVERLAP"));
	m_lstSets.InsertItem(LVIF_TEXT | LVIF_PARAM, nSetCount, "         ", 0, 0, 0, nSetCount);
	m_bNewSet = TRUE;
	m_lstSets.SetItemState(nSetCount, LVIS_SELECTED, LVIS_SELECTED);
	m_lstSets.SetFocus();
	m_lstSets.EditLabel(nSetCount);

}

void CDlgAircraftTags::OnDeleteSet()
{
	if(m_lstSets.GetItemCount() < 2) {
		AfxMessageBox("At least one Named Property Set must exist.");
		return;
	}
	CTermPlanDoc* pDoc = (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();
	ASSERT(m_nSelectedSet >= 0 && m_nSelectedSet < pDoc->m_aircraftTags.GetSetCount());
	pDoc->m_aircraftTags.DeletSet(m_nSelectedSet);

	m_nSelectedSet = pDoc->m_aircraftTags.GetCurrentSetIdx();

	UpdateSetList();

}

void CDlgAircraftTags::OnEndLabelEditListSets(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
	*pResult = 0;

	CTermPlanDoc* pDoc = (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();

	if(!pDispInfo->item.pszText) {
		if(m_bNewSet) {
			//delete this new pdp set
			OnDeleteSet();	
		}
		return;
	}

	
	m_bNewSet=FALSE;

	int nIdx = pDispInfo->item.iItem;

	ASSERT(pDoc->m_aircraftTags.GetCurrentSetIdx() == nIdx);
	pDoc->m_aircraftTags.RenameCurrentSet(pDispInfo->item.pszText);

	m_lstSets.SetItemText(nIdx, 0, pDispInfo->item.pszText);

	*pResult =1;
}

void CDlgAircraftTags::UpdateAircrafTagIn3D() const
{
	GetDocument()->UpdateAllViews(NULL, VM_RELOAD_FLIGHT_TAG_PROPERTY);
}

CTermPlanDoc* CDlgAircraftTags::GetDocument() const
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