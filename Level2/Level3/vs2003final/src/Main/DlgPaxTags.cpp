// DlgPaxTags.cpp : implementation file
//

#include "stdafx.h"
#include "termplan.h"
#include "termplandoc.h"
#include "PaxFilterDlg.h"
#include "DlgPaxTags.h"

#include "ViewMsg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



static const UINT checkboxIDs[] = {
	IDC_CHK_SHOWTAG,
	IDC_CHK_PAXID,
	IDC_CHK_FLIGHTID,
	IDC_CHK_PAXTYPE,
	IDC_CHK_GATE,
	IDC_CHK_GATETIME,
	IDC_CHK_BAGCOUNT,
	IDC_CHK_CARTCOUNT,
	IDC_CHK_STATE,
	IDC_CHK_PROCESSOR,
	IDC_CHK_WALKINGSPEED
};

/////////////////////////////////////////////////////////////////////////////
// CDlgPaxTags dialog


CDlgPaxTags::CDlgPaxTags(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgPaxTags::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgPaxTags)
	m_bShowTag = FALSE;
	m_bChkBagCount = FALSE;
	m_bChkCartCount = FALSE;
	m_bChkFlightID = FALSE;
	m_bChkGate = FALSE;
	m_bChkGateTime = FALSE;
	m_bChkPaxID = FALSE;
	m_bChkPaxType = FALSE;
	m_bChkProcessor = FALSE;
	m_bChkState = FALSE;
	m_bChkWalkingSpeed = FALSE;
	//}}AFX_DATA_INIT
	m_nSelectedPTSet = -1;
	m_bNewPTSet = FALSE;
}


void CDlgPaxTags::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgPaxTags)
	DDX_Control(pDX, IDC_STATIC_TAGPROPERTIES, m_btnTagProperties);
	DDX_Control(pDX, IDC_STATIC_PAXTYPES, m_btnPaxTypes);
	DDX_Control(pDX, IDCANCEL, m_btnCancel);
	DDX_Control(pDX, IDOK, m_btnOk);
	DDX_Control(pDX, IDC_STATIC_TOOLBARCONTENTER2, m_toolbarcontenter2);
	DDX_Control(pDX, IDC_LIST_DISPPROP, m_lstPTSets);
	DDX_Control(pDX, IDC_STATIC_BARFRAME2, m_btnBarFrame2);
	DDX_Control(pDX, IDC_STATIC_BARFRAME, m_btnBarFrame);
	DDX_Control(pDX, IDC_LIST_PAXTYPES, m_lstPaxTypes);
	DDX_Control(pDX, IDC_BUTTON_SAVE, m_btnSave);
	DDX_Control(pDX, IDC_STATIC_TOOLBARCONTENTER, m_toolbarcontenter);
	DDX_Check(pDX, IDC_CHK_SHOWTAG, m_bShowTag);
	DDX_Check(pDX, IDC_CHK_BAGCOUNT, m_bChkBagCount);
	DDX_Check(pDX, IDC_CHK_CARTCOUNT, m_bChkCartCount);
	DDX_Check(pDX, IDC_CHK_FLIGHTID, m_bChkFlightID);
	DDX_Check(pDX, IDC_CHK_GATE, m_bChkGate);
	DDX_Check(pDX, IDC_CHK_GATETIME, m_bChkGateTime);
	DDX_Check(pDX, IDC_CHK_PAXID, m_bChkPaxID);
	DDX_Check(pDX, IDC_CHK_PAXTYPE, m_bChkPaxType);
	DDX_Check(pDX, IDC_CHK_PROCESSOR, m_bChkProcessor);
	DDX_Check(pDX, IDC_CHK_STATE, m_bChkState);
	DDX_Check(pDX, IDC_CHK_WALKINGSPEED, m_bChkWalkingSpeed);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgPaxTags, CDialog)
	//{{AFX_MSG_MAP(CDlgPaxTags)
	ON_WM_CREATE()
	ON_COMMAND(ID_TOOLBARBUTTONADD, OnToolbarbuttonadd)
	ON_COMMAND(ID_TOOLBARBUTTONDEL, OnToolbarbuttondel)
	ON_NOTIFY(LVN_GETDISPINFO, IDC_LIST_PAXTYPES, OnGetDispInfo)
	ON_NOTIFY(NM_CLICK, IDC_LIST_PAXTYPES, OnListItemClick)
	ON_BN_CLICKED(IDC_CHK_SHOWTAG, OnChkShowtag)
	ON_BN_CLICKED(IDC_CHK_PAXID, OnChkPaxId)
	ON_BN_CLICKED(IDC_CHK_FLIGHTID, OnChkFlightId)
	ON_BN_CLICKED(IDC_CHK_PAXTYPE, OnChkPaxType)
	ON_BN_CLICKED(IDC_CHK_GATE, OnChkGate)
	ON_BN_CLICKED(IDC_CHK_GATETIME, OnChkGateTime)
	ON_BN_CLICKED(IDC_CHK_BAGCOUNT, OnChkBagCount)
	ON_BN_CLICKED(IDC_CHK_CARTCOUNT, OnChkCartCount)
	ON_BN_CLICKED(IDC_CHK_STATE, OnChkState)
	ON_BN_CLICKED(IDC_CHK_PROCESSOR, OnChkProcessor)
	ON_BN_CLICKED(IDC_CHK_WALKINGSPEED, OnChkWalkingspeed)
	ON_BN_CLICKED(IDC_BUTTON_SAVE, OnSave)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_DISPPROP, OnItemChangedListPTSets)
	ON_COMMAND(ID_PEOPLEMOVER_NEW, OnAddPTSet)
	ON_COMMAND(ID_PEOPLEMOVER_DELETE, OnDeletePTSet)
	ON_NOTIFY(LVN_ENDLABELEDIT, IDC_LIST_DISPPROP, OnEndLabelEditListPTSets)
END_MESSAGE_MAP()


void CDlgPaxTags::EnableAllCheckboxes(BOOL bEnable)
{
	CWnd* pWnd = GetDlgItem(IDC_CHK_SHOWTAG);
	pWnd->EnableWindow(bEnable);
	EnableSubCheckboxes(bEnable);
}

void CDlgPaxTags::EnableSubCheckboxes(BOOL bEnable)
{
	CWnd* pWnd = GetDlgItem(IDC_CHK_PAXID);
	pWnd->EnableWindow(bEnable);
	pWnd = GetDlgItem(IDC_CHK_FLIGHTID);
	pWnd->EnableWindow(bEnable);
	pWnd = GetDlgItem(IDC_CHK_PAXTYPE);
	pWnd->EnableWindow(bEnable);
	pWnd = GetDlgItem(IDC_CHK_GATE);
	pWnd->EnableWindow(bEnable);
	pWnd = GetDlgItem(IDC_CHK_GATETIME);
	pWnd->EnableWindow(bEnable);
	pWnd = GetDlgItem(IDC_CHK_BAGCOUNT);
	pWnd->EnableWindow(bEnable);
	pWnd = GetDlgItem(IDC_CHK_CARTCOUNT);
	pWnd->EnableWindow(bEnable);
	pWnd = GetDlgItem(IDC_CHK_STATE);
	pWnd->EnableWindow(bEnable);
	pWnd = GetDlgItem(IDC_CHK_PROCESSOR);
	pWnd->EnableWindow(bEnable);
	pWnd = GetDlgItem(IDC_CHK_WALKINGSPEED);
	pWnd->EnableWindow(bEnable);
}

void CDlgPaxTags::InitToolbar()
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

void CDlgPaxTags::SetAllCheckboxes()
{
	POSITION pos = m_lstPaxTypes.GetFirstSelectedItemPosition();
	int idx = m_lstPaxTypes.GetNextSelectedItem(pos);
	if(idx != -1) {
		//enable "show tags" checkbox
		CWnd* pWnd = GetDlgItem(IDC_CHK_SHOWTAG);
		pWnd->EnableWindow(TRUE);
		//get item data
		CPaxTagItem* pItem = (CPaxTagItem*) m_lstPaxTypes.GetItemData(idx);
		ASSERT(pItem != NULL);
		//show tags?
		m_bShowTag = pItem->IsShowTags();

		EnableSubCheckboxes(m_bShowTag);
		DWORD dwTagInfo = pItem->GetTagInfo();
		m_bChkBagCount = dwTagInfo & PAX_BAGCOUNT?TRUE:FALSE;
		m_bChkCartCount = dwTagInfo & PAX_CARTCOUNT?TRUE:FALSE;
		m_bChkFlightID = dwTagInfo & PAX_FLIGHTID?TRUE:FALSE;
		m_bChkGate = dwTagInfo & PAX_GATE?TRUE:FALSE;
		m_bChkGateTime = dwTagInfo & PAX_GATETIME?TRUE:FALSE;
		m_bChkPaxID = dwTagInfo & PAX_ID?TRUE:FALSE;
		m_bChkPaxType = dwTagInfo & PAX_PAXTYPE?TRUE:FALSE;
		m_bChkProcessor = dwTagInfo & PAX_PROCESSOR?TRUE:FALSE;
		m_bChkState = dwTagInfo & PAX_STATE?TRUE:FALSE;
		m_bChkWalkingSpeed = dwTagInfo & PAX_WALKINGSPEED ? TRUE:FALSE;
		
		UpdateData(FALSE);
	}
	else {
		EnableAllCheckboxes(FALSE);
	}
}

/////////////////////////////////////////////////////////////////////////////
// CDlgPaxTags message handlers

BOOL CDlgPaxTags::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	DWORD dwStyle = m_lstPaxTypes.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_EX_GRIDLINES;
	m_lstPaxTypes.SetExtendedStyle( dwStyle );
	// Create column
	m_lstPaxTypes.InsertColumn( 0, "Name", LVCFMT_CENTER, 100 );
	m_lstPaxTypes.InsertColumn( 1, "Passenger Type Description", LVCFMT_LEFT, 400 );

	dwStyle = m_lstPTSets.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_EX_GRIDLINES;
	m_lstPTSets.SetExtendedStyle( dwStyle );
	// Create column
	m_lstPTSets.InsertColumn( 0, "Name", LVCFMT_CENTER, 400 );

	UpdatePTSetList();

	InitToolbar();
	SetAllCheckboxes();
	m_btnSave.EnableWindow( FALSE );
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

int CDlgPaxTags::OnCreate(LPCREATESTRUCT lpCreateStruct) 
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
CTermPlanDoc* CDlgPaxTags::GetDocument() const
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
void CDlgPaxTags::UpdatePaxTagPropIn3D() const
{
	CTermPlanDoc* pDoc = GetDocument();
	if (pDoc)
	{
		pDoc->UpdateAllViews(NULL, VM_RELOAD_PAX_TAG_PROPERTY);
	}
}

void CDlgPaxTags::OnToolbarbuttonadd() 
{
	CPaxFilterDlg dlg( m_pParentWnd );
	if( dlg.DoModal() == IDCANCEL )
		return;
	else
	{
		CTermPlanDoc* pDoc	= GetDocument();	
        CPaxTagItem*  pItem = new CPaxTagItem( dlg.GetPaxType().GetName() );
		pItem->SetPaxType( dlg.GetPaxType() );
		pDoc->m_paxTags.AddPaxTagItem( pItem );
		int nCount = m_lstPaxTypes.GetItemCount();

		LVITEM lvi; 
		lvi.mask = LVIF_PARAM | LVIF_TEXT;
		lvi.iSubItem = 0;
		lvi.pszText = LPSTR_TEXTCALLBACK;
		lvi.iItem = nCount;
		lvi.lParam = (LPARAM) pItem;
		m_lstPaxTypes.InsertItem(&lvi);
		m_lstPaxTypes.SetItemState( nCount,LVIS_SELECTED,LVIS_SELECTED );
		SetAllCheckboxes();

		m_btnSave.EnableWindow();
		UpdatePaxTagPropIn3D();
	}
}

void CDlgPaxTags::OnToolbarbuttondel() 
{
	POSITION pos = m_lstPaxTypes.GetFirstSelectedItemPosition();
	if(pos == NULL)
		return;
	int idx = m_lstPaxTypes.GetNextSelectedItem(pos);
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
			CTermPlanDoc* pDoc	= GetDocument();	
			// delete from db
			pDoc->m_paxTags.DeleteItem(idx-2);
			m_lstPaxTypes.DeleteItem(idx);
			SetAllCheckboxes();
			UpdatePaxTagPropIn3D();
		}
	}
}

void CDlgPaxTags::OnGetDispInfo(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
	if(pDispInfo->item.mask & LVIF_TEXT) {
		CPaxTagItem* pItem = (CPaxTagItem*) pDispInfo->item.lParam;
		switch(pDispInfo->item.iSubItem) {
		case 0: //name
			lstrcpy(pDispInfo->item.pszText, pItem->GetPaxType()->GetName());
			break;
		case 1: //pax type description
			lstrcpy(pDispInfo->item.pszText, pItem->GetPaxType()->ScreenPrint());
			break;
		}
	}
	*pResult = 0;
}
/*
BOOL CDlgPaxTags::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
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

void CDlgPaxTags::OnListItemClick(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NMITEMACTIVATE* pNMIA = (NMITEMACTIVATE*) pNMHDR;

	SetAllCheckboxes();
	
	*pResult = 0;
}

void CDlgPaxTags::OnChkShowtag() 
{
	UpdateData();
	POSITION pos = m_lstPaxTypes.GetFirstSelectedItemPosition();
	int idx = m_lstPaxTypes.GetNextSelectedItem(pos);
	if(idx != -1) {
		//get item data
		CPaxTagItem* pItem = (CPaxTagItem*) m_lstPaxTypes.GetItemData(idx);
		ASSERT(pItem != NULL);
		//set "show tags" data
		pItem->SetShowTags(m_bShowTag);
		UpdatePaxTagPropIn3D();
	}
	SetAllCheckboxes();
}

void CDlgPaxTags::OnChkBagCount() 
{
	UpdateData();
	POSITION pos = m_lstPaxTypes.GetFirstSelectedItemPosition();
	int idx = m_lstPaxTypes.GetNextSelectedItem(pos);
	if(idx != -1) {
		//get item data
		CPaxTagItem* pItem = (CPaxTagItem*) m_lstPaxTypes.GetItemData(idx);
		ASSERT(pItem != NULL);
		//set "show tags" data
		DWORD dwTagInfo = pItem->GetTagInfo();
		if(m_bChkBagCount)
			dwTagInfo = dwTagInfo | PAX_BAGCOUNT;
		else
			dwTagInfo = dwTagInfo ^ PAX_BAGCOUNT;
		pItem->SetTagInfo(dwTagInfo);
		UpdatePaxTagPropIn3D();
	}
	else {
		ASSERT(FALSE);
	}
}

void CDlgPaxTags::OnChkCartCount() 
{
	UpdateData();
	POSITION pos = m_lstPaxTypes.GetFirstSelectedItemPosition();
	int idx = m_lstPaxTypes.GetNextSelectedItem(pos);
	if(idx != -1) {
		//get item data
		CPaxTagItem* pItem = (CPaxTagItem*) m_lstPaxTypes.GetItemData(idx);
		ASSERT(pItem != NULL);
		//set "show tags" data
		DWORD dwTagInfo = pItem->GetTagInfo();
		if(m_bChkCartCount)
			dwTagInfo = dwTagInfo | PAX_CARTCOUNT;
		else
			dwTagInfo = dwTagInfo ^ PAX_CARTCOUNT;
		pItem->SetTagInfo(dwTagInfo);
		UpdatePaxTagPropIn3D();
	}
	else {
		ASSERT(FALSE);
	}
}

void CDlgPaxTags::OnChkFlightId() 
{
	UpdateData();
	POSITION pos = m_lstPaxTypes.GetFirstSelectedItemPosition();
	int idx = m_lstPaxTypes.GetNextSelectedItem(pos);
	if(idx != -1) {
		//get item data
		CPaxTagItem* pItem = (CPaxTagItem*) m_lstPaxTypes.GetItemData(idx);
		ASSERT(pItem != NULL);
		//set "show tags" data
		DWORD dwTagInfo = pItem->GetTagInfo();
		if(m_bChkFlightID)
			dwTagInfo = dwTagInfo | PAX_FLIGHTID;
		else
			dwTagInfo = dwTagInfo ^ PAX_FLIGHTID;
		pItem->SetTagInfo(dwTagInfo);
		UpdatePaxTagPropIn3D();
	}
	else {
		ASSERT(FALSE);
	}
}

void CDlgPaxTags::OnChkGate() 
{
	UpdateData();
	POSITION pos = m_lstPaxTypes.GetFirstSelectedItemPosition();
	int idx = m_lstPaxTypes.GetNextSelectedItem(pos);
	if(idx != -1) {
		//get item data
		CPaxTagItem* pItem = (CPaxTagItem*) m_lstPaxTypes.GetItemData(idx);
		ASSERT(pItem != NULL);
		//set "show tags" data
		DWORD dwTagInfo = pItem->GetTagInfo();
		if(m_bChkGate)
			dwTagInfo = dwTagInfo | PAX_GATE;
		else
			dwTagInfo = dwTagInfo ^ PAX_GATE;
		pItem->SetTagInfo(dwTagInfo);
		UpdatePaxTagPropIn3D();
	}
	else {
		ASSERT(FALSE);
	}
}

void CDlgPaxTags::OnChkGateTime() 
{
	UpdateData();
	POSITION pos = m_lstPaxTypes.GetFirstSelectedItemPosition();
	int idx = m_lstPaxTypes.GetNextSelectedItem(pos);
	if(idx != -1) {
		//get item data
		CPaxTagItem* pItem = (CPaxTagItem*) m_lstPaxTypes.GetItemData(idx);
		ASSERT(pItem != NULL);
		//set "show tags" data
		DWORD dwTagInfo = pItem->GetTagInfo();
		if(m_bChkGateTime)
			dwTagInfo = dwTagInfo | PAX_GATETIME;
		else
			dwTagInfo = dwTagInfo ^ PAX_GATETIME;
		pItem->SetTagInfo(dwTagInfo);
		UpdatePaxTagPropIn3D();
	}
	else {
		ASSERT(FALSE);
	}
}

void CDlgPaxTags::OnChkPaxId() 
{
	UpdateData();
	POSITION pos = m_lstPaxTypes.GetFirstSelectedItemPosition();
	int idx = m_lstPaxTypes.GetNextSelectedItem(pos);
	if(idx != -1) {
		//get item data
		CPaxTagItem* pItem = (CPaxTagItem*) m_lstPaxTypes.GetItemData(idx);
		ASSERT(pItem != NULL);
		//set "show tags" data
		DWORD dwTagInfo = pItem->GetTagInfo();
		if(m_bChkPaxID)
			dwTagInfo = dwTagInfo | PAX_ID;
		else
			dwTagInfo = dwTagInfo ^ PAX_ID;
		pItem->SetTagInfo(dwTagInfo);
		UpdatePaxTagPropIn3D();
	}
	else {
		ASSERT(FALSE);
	}
}

void CDlgPaxTags::OnChkPaxType() 
{
	UpdateData();
	POSITION pos = m_lstPaxTypes.GetFirstSelectedItemPosition();
	int idx = m_lstPaxTypes.GetNextSelectedItem(pos);
	if(idx != -1) {
		//get item data
		CPaxTagItem* pItem = (CPaxTagItem*) m_lstPaxTypes.GetItemData(idx);
		ASSERT(pItem != NULL);
		//set "show tags" data
		DWORD dwTagInfo = pItem->GetTagInfo();
		if(m_bChkPaxType)
			dwTagInfo = dwTagInfo | PAX_PAXTYPE;
		else
			dwTagInfo = dwTagInfo ^ PAX_PAXTYPE;
		pItem->SetTagInfo(dwTagInfo);
		UpdatePaxTagPropIn3D();
	}
	else {
		ASSERT(FALSE);
	}
}

void CDlgPaxTags::OnChkProcessor() 
{
	UpdateData();
	POSITION pos = m_lstPaxTypes.GetFirstSelectedItemPosition();
	int idx = m_lstPaxTypes.GetNextSelectedItem(pos);
	if(idx != -1) {
		//get item data
		CPaxTagItem* pItem = (CPaxTagItem*) m_lstPaxTypes.GetItemData(idx);
		ASSERT(pItem != NULL);
		//set "show tags" data
		DWORD dwTagInfo = pItem->GetTagInfo();
		if(m_bChkProcessor)
			dwTagInfo = dwTagInfo | PAX_PROCESSOR;
		else
			dwTagInfo = dwTagInfo ^ PAX_PROCESSOR;
		pItem->SetTagInfo(dwTagInfo);
		UpdatePaxTagPropIn3D();
	}
	else {
		ASSERT(FALSE);
	}
}

void CDlgPaxTags::OnChkState() 
{
	UpdateData();
	POSITION pos = m_lstPaxTypes.GetFirstSelectedItemPosition();
	int idx = m_lstPaxTypes.GetNextSelectedItem(pos);
	if(idx != -1) {
		//get item data
		CPaxTagItem* pItem = (CPaxTagItem*) m_lstPaxTypes.GetItemData(idx);
		ASSERT(pItem != NULL);
		//set "show tags" data
		DWORD dwTagInfo = pItem->GetTagInfo();
		if(m_bChkState)
			dwTagInfo = dwTagInfo | PAX_STATE;
		else
			dwTagInfo = dwTagInfo ^ PAX_STATE;
		pItem->SetTagInfo(dwTagInfo);
		UpdatePaxTagPropIn3D();
	}
	else {
		ASSERT(FALSE);
	}
}

void CDlgPaxTags::OnOK() 
{
	CTermPlanDoc* pDoc = GetDocument();
	pDoc->m_paxTags.saveInputs(pDoc->m_ProjInfo.path, true);
	
	CDialog::OnOK();
}

void CDlgPaxTags::OnSave() 
{
	CTermPlanDoc* pDoc = GetDocument();
	pDoc->m_paxTags.saveInputs(pDoc->m_ProjInfo.path, true);
}

void CDlgPaxTags::OnCancel() 
{
	CTermPlanDoc* pDoc = GetDocument();
	CDialog::OnCancel(); // should firstly call base method, else in some case CDlgPaxTags::OnGetDispInfo() will crash because of invalid heap memory

	pDoc->m_paxTags.loadInputs(pDoc->m_ProjInfo.path, &(pDoc->GetTerminal()));
	UpdatePaxTagPropIn3D();
}

void CDlgPaxTags::OnSize(UINT nType, int cx, int cy) 
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
	m_lstPaxTypes.MoveWindow( rc.left,rc.top,rc.Width(),rc.Height() );

	m_btnTagProperties.MoveWindow(halfwidth+9, onethirdsheight+4, halfwidth-18, (2*onethirdsheight)-25-btnrc.Height());
	int nCount = sizeof(checkboxIDs)/sizeof(UINT);
	// TRACE("size of checkboxIDs array: %d\n", nCount);
	CRect rcCheck;
	CWnd* pWnd = GetDlgItem(checkboxIDs[0]);
	pWnd->GetWindowRect(&rcCheck);
	pWnd->MoveWindow(halfwidth+40, onethirdsheight+55, rcCheck.Width(), rcCheck.Height());
	for(int i=1; i<nCount; i++) {
		pWnd = GetDlgItem(checkboxIDs[i]);
		pWnd->MoveWindow(halfwidth+75, onethirdsheight+55+i*(rcCheck.Height()+5), rcCheck.Width(), rcCheck.Height());
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
	m_lstPTSets.MoveWindow( 9,35,rc.Width(),rc.Height() );
	
	m_btnSave.MoveWindow( cx-20-3*btnrc.Width(),cy-10-btnrc.Height(),btnrc.Width(),btnrc.Height() );
	m_btnOk.MoveWindow( cx-15-2*btnrc.Width(),cy-10-btnrc.Height(),btnrc.Width(),btnrc.Height() );
	m_btnCancel.MoveWindow( cx-10-btnrc.Width(),cy-10-btnrc.Height(),btnrc.Width(),btnrc.Height() );

	InvalidateRect(NULL);
	
}

void CDlgPaxTags::UpdatePTSetList()
{
	CTermPlanDoc* pDoc	= GetDocument();

	m_lstPTSets.DeleteAllItems();

	int nPTSetCount = pDoc->m_paxTags.GetPTSetCount();
	for(int i=0; i<nPTSetCount; i++) {
		m_lstPTSets.InsertItem(LVIF_TEXT | LVIF_PARAM, i, pDoc->m_paxTags.GetPTSetName(i), 0, 0, 0, (LPARAM) i);
	}
	m_nSelectedPTSet = pDoc->m_paxTags.GetCurrentPTSetIdx();
	m_lstPTSets.SetItemState(m_nSelectedPTSet, LVIS_SELECTED, LVIS_SELECTED );

}

void CDlgPaxTags::UpdatePaxTagData()
{
	m_lstPaxTypes.DeleteAllItems();

	// Load data.
	CTermPlanDoc* pDoc	= GetDocument();	
	
	LVITEM lvi; 
    lvi.mask = LVIF_PARAM | LVIF_TEXT;
	lvi.iSubItem = 0;
	lvi.pszText = LPSTR_TEXTCALLBACK;
	
	CPaxTagItem* pItem = pDoc->m_paxTags.GetDefaultPaxTags();
	ASSERT(pItem != NULL);
    lvi.iItem = 0;
	lvi.lParam = (LPARAM) pItem;
	m_lstPaxTypes.InsertItem(&lvi);

	pItem = pDoc->m_paxTags.GetOverLapPaxTags();
	ASSERT(pItem != NULL);
	lvi.iItem = 1;
	lvi.lParam = (LPARAM) pItem;
	m_lstPaxTypes.InsertItem(&lvi);
	int nCount = pDoc->m_paxTags.GetCount();
	for( int i=0; i<nCount; i++ )
	{
		pItem = pDoc->m_paxTags.GetPaxTagItem( i );
		lvi.iItem = 2+i;
		lvi.lParam = (LPARAM) pItem;
		m_lstPaxTypes.InsertItem( &lvi );
	}
}

void CDlgPaxTags::OnItemChangedListPTSets(NMHDR* pNMHDR, LRESULT* pResult)
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
		m_nSelectedPTSet = (int) m_lstPTSets.GetItemData(pnmv->iItem);
		CTermPlanDoc* pDoc = GetDocument();
		pDoc->m_paxTags.SetCurrentPTSet(m_nSelectedPTSet);
	}
	else
		ASSERT(0);

	UpdatePaxTagData();
}

void CDlgPaxTags::OnChkWalkingspeed() 
{
	// TODO: Add your control notification handler code here
	UpdateData();
	POSITION pos = m_lstPaxTypes.GetFirstSelectedItemPosition();
	int idx = m_lstPaxTypes.GetNextSelectedItem(pos);
	if(idx != -1) {
		//get item data
		CPaxTagItem* pItem = (CPaxTagItem*) m_lstPaxTypes.GetItemData(idx);
		ASSERT(pItem != NULL);
		//set "show tags" data
		DWORD dwTagInfo = pItem->GetTagInfo();
		if(m_bChkWalkingSpeed)
			dwTagInfo = dwTagInfo | PAX_WALKINGSPEED;
		else
			dwTagInfo = dwTagInfo ^ PAX_WALKINGSPEED;
		pItem->SetTagInfo(dwTagInfo);
		UpdatePaxTagPropIn3D();
	}
	else {
		ASSERT(FALSE);
	}	
}

void CDlgPaxTags::OnAddPTSet()
{
	CTermPlanDoc* pDoc = GetDocument();

	int nPTSetCount = pDoc->m_paxTags.GetPTSetCount();
	pDoc->m_paxTags.AddPTSet("undefined", new CPaxTagItem("DEFAULT"), new CPaxTagItem("OVERLAP"));
	m_lstPTSets.InsertItem(LVIF_TEXT | LVIF_PARAM, nPTSetCount, "         ", 0, 0, 0, nPTSetCount);
	m_bNewPTSet = TRUE;
	m_lstPTSets.SetItemState(nPTSetCount, LVIS_SELECTED, LVIS_SELECTED);
	m_lstPTSets.SetFocus();
	m_lstPTSets.EditLabel(nPTSetCount);

}

void CDlgPaxTags::OnDeletePTSet()
{
	if(m_lstPTSets.GetItemCount() < 2) {
		AfxMessageBox("At least one Named Property Set must exist.");
		return;
	}
	CTermPlanDoc* pDoc = GetDocument();
	ASSERT(m_nSelectedPTSet >= 0 && m_nSelectedPTSet < pDoc->m_paxTags.GetPTSetCount());
	pDoc->m_paxTags.DeletPTSet(m_nSelectedPTSet);

	m_nSelectedPTSet = pDoc->m_paxTags.GetCurrentPTSetIdx();

	UpdatePTSetList();

}

void CDlgPaxTags::OnEndLabelEditListPTSets(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
	*pResult = 0;

	CTermPlanDoc* pDoc = GetDocument();

	if(!pDispInfo->item.pszText) {
		if(m_bNewPTSet) {
			//delete this new pdp set
			OnDeletePTSet();	
		}
		return;
	}

	
	m_bNewPTSet=FALSE;

	int nIdx = pDispInfo->item.iItem;

	ASSERT(pDoc->m_paxTags.GetCurrentPTSetIdx() == nIdx);
	pDoc->m_paxTags.RenameCurrentPTSet(pDispInfo->item.pszText);

	m_lstPTSets.SetItemText(nIdx, 0, pDispInfo->item.pszText);

	*pResult =1;
}