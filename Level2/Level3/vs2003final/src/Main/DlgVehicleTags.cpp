// DlgVehicleTags.cpp : implementation file
//

#include "stdafx.h"
#include "DlgVehicleTags.h"

#include "TermPlanDoc.h"
#include ".\dlgvehicletags.h"

#include "ViewMsg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



static const UINT checkboxIDs[] = {
	IDC_CHK_SHOWTAG,	
		IDC_CHK_VEHICLEID,
		IDC_CHK_POOL,
		IDC_CHK_SERVICEFLIGHT,
		IDC_CHK_SERVICESTAND,	
		IDC_CHK_SERVICECOUNTLEFT,
};

/////////////////////////////////////////////////////////////////////////////
// CDlgVehicleTags dialog


CDlgVehicleTags::CDlgVehicleTags(CWnd* pParent /*=NULL*/)
: CXTResizeDialog(CDlgVehicleTags::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgVehicleTags)
	m_bShowTag = FALSE;
	m_bChkVehicleID = FALSE;
	m_bChkPool = FALSE;
	m_bChkServiceFlight = FALSE;
	m_bChkServiceStand = FALSE;
	m_bChkServiceCountLeft = FALSE;

	//}}AFX_DATA_INIT
	m_nSelectedSet = -1;
	m_bNewSet = FALSE;
}


void CDlgVehicleTags::DoDataExchange(CDataExchange* pDX)
{
	CXTResizeDialog::DoDataExchange(pDX); 
	DDX_Control(pDX, IDC_STATIC_TAGPROPERTIES, m_btnTagProperties);
	DDX_Control(pDX, IDC_STATIC_PAXTYPES, m_btnPaxTypes);
	DDX_Control(pDX, IDCANCEL, m_btnCancel);
	DDX_Control(pDX, IDOK, m_btnOk);
	DDX_Control(pDX, IDC_STATIC_TOOLBARCONTENTER2, m_toolbarcontenter2);
	DDX_Control(pDX, IDC_LIST_DISPPROP, m_lstSets);
	DDX_Control(pDX, IDC_LIST_PAXTYPES, m_lstFlightTypes);
	DDX_Control(pDX, IDC_BUTTON_SAVE, m_btnSave);
	DDX_Control(pDX, IDC_STATIC_TOOLBARCONTENTER, m_toolbarcontenter);
	DDX_Check(pDX, IDC_CHK_SHOWTAG, m_bShowTag);
	DDX_Check(pDX, IDC_CHK_VEHICLEID, m_bChkVehicleID);
	DDX_Check(pDX, IDC_CHK_POOL, m_bChkPool);
	DDX_Check(pDX, IDC_CHK_SERVICEFLIGHT, m_bChkServiceFlight);
	DDX_Check(pDX, IDC_CHK_SERVICESTAND, m_bChkServiceStand);
	DDX_Check(pDX, IDC_CHK_SERVICECOUNTLEFT, m_bChkServiceCountLeft); 
}


BEGIN_MESSAGE_MAP(CDlgVehicleTags, CXTResizeDialog)
	//{{AFX_MSG_MAP(CDlgVehicleTags)
	ON_WM_CREATE()
	ON_COMMAND(ID_TOOLBARBUTTONADD, OnToolbarbuttonadd)
	ON_COMMAND(ID_TOOLBARBUTTONDEL, OnToolbarbuttondel)
	//ON_NOTIFY(LVN_GETDISPINFO, IDC_LIST_PAXTYPES, OnGetDispInfo)
	ON_LBN_SELCHANGE(IDC_LIST_PAXTYPES, OnListItemClick)
	ON_BN_CLICKED(IDC_CHK_SHOWTAG, OnChkShowtag)
	ON_BN_CLICKED(IDC_CHK_VEHICLEID, OnChkVehicleId)
	ON_BN_CLICKED(IDC_CHK_POOL, OnChkPool)
	ON_BN_CLICKED(IDC_CHK_SERVICEFLIGHT, OnChkServiceFlight)
	ON_BN_CLICKED(IDC_CHK_SERVICESTAND, OnChkServiceStand)
	ON_BN_CLICKED(IDC_CHK_SERVICECOUNTLEFT, OnChkServiceCountLeft) 
	ON_MESSAGE(WM_APP_LB_ITEM_EDITED,OnEditedListBox)
	ON_BN_CLICKED(IDC_BUTTON_SAVE, OnSave)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
	ON_LBN_SELCHANGE(IDC_LIST_DISPPROP, OnItemChangedListSets)
	ON_COMMAND(ID_PEOPLEMOVER_NEW, OnAddSet)
	ON_COMMAND(ID_PEOPLEMOVER_DELETE, OnDeleteSet)
	//ON_LBN_SELCANCEL(IDC_LIST_DISPPROP, OnEndLabelEditListSets)	
END_MESSAGE_MAP()


void CDlgVehicleTags::EnableAllCheckboxes(BOOL bEnable)
{
	CWnd* pWnd = GetDlgItem(IDC_CHK_SHOWTAG);
	if(!pWnd)return;
	pWnd->EnableWindow(bEnable);
	EnableSubCheckboxes(bEnable);
}

void CDlgVehicleTags::EnableSubCheckboxes(BOOL bEnable)
{
	CWnd* pWnd = NULL;
	int nCount = sizeof(checkboxIDs)/sizeof(checkboxIDs[0]);
	for(int i=1;i< nCount; i++)
	{
		pWnd = GetDlgItem(checkboxIDs[i]);
		if(!pWnd)continue;
		pWnd->EnableWindow(bEnable);		
	}

}

void CDlgVehicleTags::InitToolbar()
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
void CDlgVehicleTags::SaveCurrentData()
{ 
	int idx = m_lstFlightTypes.GetCurSel();
	if(idx != LB_ERR )
	{
		DWORD taginfo = 0x0000000000000000;
		taginfo += m_bChkVehicleID?VEHICLE_TAG_FLAG_VEHICLE_ID:0x0000000000000000;
		taginfo += m_bChkPool?VEHICLE_TAG_FLAG_POOL:0x0000000000000000;
		taginfo += m_bChkServiceFlight?VEHICLE_TAG_FLAG_SERVICE_FLIGHT:0x0000000000000000;
		taginfo += m_bChkServiceStand?VEHICLE_TAG_FLAG_SERVICE_STAND:0x0000000000000000;
		taginfo += m_bChkServiceCountLeft?VEHICLE_TAG_FLAG_SERVICE_COUNT_LEFT:0x0000000000000000;


		CVehicleTagItem* pItem = (CVehicleTagItem*) m_lstFlightTypes.GetItemData(idx);
		pItem->m_dwTagInfo = taginfo;
	}
}
void CDlgVehicleTags::SetAllCheckboxes()
{
	int idx = m_lstFlightTypes.GetCurSel();
	if(idx != LB_ERR ) {
		//enable "show tags" checkbox
		CWnd* pWnd = GetDlgItem(IDC_CHK_SHOWTAG);
		if(!pWnd)return;
		pWnd->EnableWindow(TRUE);

		//get item data
		CVehicleTagItem* pItem = (CVehicleTagItem*) m_lstFlightTypes.GetItemData(idx);
		ASSERT(pItem != NULL);
		//show tags?
		m_bShowTag = pItem->m_bShowTags;
		EnableSubCheckboxes(m_bShowTag);
		DWORD dwTagInfo = pItem->m_dwTagInfo;
		m_bChkVehicleID = dwTagInfo & VEHICLE_TAG_FLAG_VEHICLE_ID?TRUE:FALSE;
		m_bChkPool = dwTagInfo & VEHICLE_TAG_FLAG_POOL?TRUE:FALSE;
		m_bChkServiceFlight = dwTagInfo & VEHICLE_TAG_FLAG_SERVICE_FLIGHT?TRUE:FALSE;
		m_bChkServiceStand = dwTagInfo & VEHICLE_TAG_FLAG_SERVICE_STAND?TRUE:FALSE;
		m_bChkServiceCountLeft = dwTagInfo & VEHICLE_TAG_FLAG_SERVICE_COUNT_LEFT?TRUE:FALSE;

		UpdateData(FALSE);
	}
	else {
		EnableAllCheckboxes(FALSE);
	}
}

/////////////////////////////////////////////////////////////////////////////
// CDlgAircraftTags message handlers

BOOL CDlgVehicleTags::OnInitDialog() 
{
	CXTResizeDialog::OnInitDialog(); 

	CTermPlanDoc* pDoc = (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();
	if(!pDoc)return (FALSE);
	m_lstFlightTypes.SetProjID(pDoc->GetProjectID());

	UpdateSetList();
	InitToolbar();

	SetResize(m_ToolBar.GetDlgCtrlID(), SZ_TOP_LEFT, SZ_TOP_LEFT);
	SetResize(IDC_STATIC_TOOLBARCONTENTER2, SZ_TOP_LEFT, SZ_TOP_LEFT);
	SetResize(IDC_STATIC_BARFRAME2, SZ_TOP_LEFT, SZ_TOP_RIGHT);
	SetResize(IDC_LIST_DISPPROP, SZ_TOP_LEFT, SZ_TOP_RIGHT);

	SetResize(m_ToolBar2.GetDlgCtrlID(), SZ_TOP_LEFT, SZ_TOP_LEFT);
	SetResize(IDC_STATIC_TOOLBARCONTENTER, SZ_TOP_LEFT, SZ_TOP_LEFT);
	SetResize(IDC_STATIC_PAXTYPES, SZ_TOP_LEFT, SZ_BOTTOM_CENTER);
	SetResize(IDC_LIST_PAXTYPES, SZ_TOP_LEFT, SZ_BOTTOM_CENTER);

	SetResize(IDC_STATIC_TAGPROPERTIES, SZ_TOP_CENTER, SZ_BOTTOM_RIGHT);
	SetResize(IDC_CHK_SHOWTAG, SZ_MIDDLE_CENTER, SZ_MIDDLE_CENTER);
	SetResize(IDC_CHK_VEHICLEID, SZ_MIDDLE_CENTER, SZ_MIDDLE_CENTER);
	SetResize(IDC_CHK_POOL, SZ_MIDDLE_CENTER, SZ_MIDDLE_CENTER);
	SetResize(IDC_CHK_SERVICEFLIGHT, SZ_MIDDLE_CENTER, SZ_MIDDLE_CENTER);
	SetResize(IDC_CHK_SERVICESTAND, SZ_MIDDLE_CENTER, SZ_MIDDLE_CENTER);
	SetResize(IDC_CHK_SERVICECOUNTLEFT, SZ_MIDDLE_CENTER, SZ_MIDDLE_CENTER);

	SetResize(IDC_BUTTON_SAVE, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);
	SetResize(IDOK, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);
	SetResize(IDCANCEL, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);

	SetAllCheckboxes();
	m_btnSave.EnableWindow( FALSE );

	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),TRUE);
	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

int CDlgVehicleTags::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CXTResizeDialog::OnCreate(lpCreateStruct) == -1)
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

void CDlgVehicleTags::OnToolbarbuttonadd() 
{
	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument(); 
	if(!pDoc)return;
	pDoc->m_vehicleTags.AddTagItemToNode(m_nSelectedSet,-1,FALSE,0x0);
	CVehicleTagNode * pNode = pDoc->m_vehicleTags.GetNode(m_nSelectedSet);
	if(!pNode)return;
	CVehicleTagItem * pItem = pNode->GetItem(pNode->GetItemCount() - 1);
	int nCount = m_lstFlightTypes.GetCount();

	nCount = m_lstFlightTypes.InsertString(nCount,pItem->m_vehicleItem.getName()); 
	m_lstFlightTypes.SetItemData(nCount,(DWORD_PTR)pItem);
	pDoc->m_vehicleTags.SetCurSelItemIndexInCurSelNode(pNode->GetItemCount() - 1);
	m_lstFlightTypes.SetCurSel(nCount);

	SetAllCheckboxes();
	m_btnSave.EnableWindow();
	UpdateVehicleTagIn3D();
}

void CDlgVehicleTags::OnToolbarbuttondel() 
{
	int idx = m_lstFlightTypes.GetCurSel();
	if(idx != LB_ERR ){
		if(idx == 0) {
			AfxMessageBox(_T("Can not delete the item \"DEFAULT\"") );
			return;
		} 
		else {
			CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();	
			// delete from db
			m_vrTagItemDeleted.push_back(pDoc->m_vehicleTags.DeleteItemInNode(m_nSelectedSet,idx) );
			m_lstFlightTypes.DeleteString(idx);
			m_lstFlightTypes.SetCurSel(pDoc->m_vehicleTags.GetCurSelItemIndexInCurSelNode());
			SetAllCheckboxes();
			UpdateVehicleTagIn3D();
		}
	}
}

void CDlgVehicleTags::OnGetDispInfo(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
	if(pDispInfo->item.mask & LVIF_TEXT) {
		CVehicleTagItem * pItem = (CVehicleTagItem*) pDispInfo->item.lParam;
		if(!pItem)return;
		switch(pDispInfo->item.iSubItem) {
		case 0: //name
			CString strName = _T("");
			if( pItem->m_vehicleItem.GetID() == -1)
				strName = _T("Default");
			else
				strName = pItem->m_vehicleItem.getName();
			lstrcpy(pDispInfo->item.pszText,strName);
			break;
		}
	}
	*pResult = 0;
}

void CDlgVehicleTags::OnListItemClick( void) 
{	
	CTermPlanDoc* pDoc = (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();
	if(!pDoc)return;
	pDoc->m_vehicleTags.SetCurSelItemIndexInCurSelNode(m_lstFlightTypes.GetCurSel()); 
	SetAllCheckboxes(); 
}

void CDlgVehicleTags::OnChkShowtag() 
{
	UpdateData(); 
	int idx = m_lstFlightTypes.GetCurSel();
	if(idx != LB_ERR) {
		//get item data
		CVehicleTagItem* pItem = (CVehicleTagItem*) m_lstFlightTypes.GetItemData(idx);
		ASSERT(pItem != NULL);
		//set "show tags" data
		pItem->m_bShowTags = m_bShowTag;
		UpdateVehicleTagIn3D();
	}
	SetAllCheckboxes();
}

void CDlgVehicleTags::OnChkItem(DWORD dwMask, BOOL bIsCheck)
{
	int idx = m_lstFlightTypes.GetCurSel();
	if(idx != LB_ERR) {
		//get item data
		CVehicleTagItem* pItem = (CVehicleTagItem*) m_lstFlightTypes.GetItemData(idx);
		ASSERT(pItem != NULL);
		//set data
		DWORD dwTagInfo = pItem->m_dwTagInfo;
		if(bIsCheck)
			dwTagInfo = dwTagInfo | dwMask;
		else
			dwTagInfo = dwTagInfo ^ dwMask;
		pItem->m_dwTagInfo = dwTagInfo;
	}
	else {
		ASSERT(FALSE);
	}
}

void CDlgVehicleTags::OnChkVehicleId() 
{
	UpdateData();
	OnChkItem(VEHICLE_TAG_FLAG_VEHICLE_ID, m_bChkVehicleID);
	SaveCurrentData();
	UpdateVehicleTagIn3D();
}
void CDlgVehicleTags::OnChkPool() 
{
	UpdateData();
	OnChkItem(VEHICLE_TAG_FLAG_POOL, m_bChkPool);
	SaveCurrentData();
	UpdateVehicleTagIn3D();
}
void CDlgVehicleTags::OnChkServiceFlight() 
{
	UpdateData();
	OnChkItem(VEHICLE_TAG_FLAG_SERVICE_FLIGHT, m_bChkServiceFlight);
	SaveCurrentData();
	UpdateVehicleTagIn3D();
}
void CDlgVehicleTags::OnChkServiceStand() 
{
	UpdateData();
	OnChkItem(VEHICLE_TAG_FLAG_SERVICE_STAND, m_bChkServiceStand);
	SaveCurrentData();
	UpdateVehicleTagIn3D();
}

void CDlgVehicleTags::OnChkServiceCountLeft() 
{
	UpdateData();
	OnChkItem(VEHICLE_TAG_FLAG_SERVICE_COUNT_LEFT, m_bChkServiceCountLeft);
	SaveCurrentData();
	UpdateVehicleTagIn3D();
}

void CDlgVehicleTags::OnOK() 
{
	OnSave();
	CXTResizeDialog::OnOK();
}

void CDlgVehicleTags::OnSave() 
{
	CTermPlanDoc* pDoc = (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();
	if(!pDoc)return;
	pDoc->m_vehicleTags.SaveData();

	FlushDeletedItem();
}

void CDlgVehicleTags::OnCancel() 
{
	CTermPlanDoc* pDoc = (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();
	if(!pDoc)return;
	pDoc->m_vehicleTags.ReadData(pDoc->GetProjectID());
	CXTResizeDialog::OnCancel();
}

void CDlgVehicleTags::OnSize(UINT nType, int cx, int cy) 
{
	CXTResizeDialog::OnSize(nType, cx, cy);	
}

void CDlgVehicleTags::UpdateSetList()
{
	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();
	if(!pDoc)return;
	m_lstSets.ResetContent();

	int nSetCount = pDoc->m_vehicleTags.GetNodeCount();
	for(int i=0; i<nSetCount; i++) {
		int nNewIndex = m_lstSets.AddString(pDoc->m_vehicleTags.GetNodeName(i));
	}
	m_nSelectedSet = pDoc->m_vehicleTags.GetCurSelNodeIndex();
	m_lstSets.SetCurSel(m_nSelectedSet);
	UpdateTagData();
}

void CDlgVehicleTags::UpdateTagData()
{
	m_lstFlightTypes.ResetContent();

	// Load data.
	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();	
	if(!pDoc)return;
	CVehicleTagNode* pNode = 0;
	CVehicleTagItem* pItem = 0; 

	pNode = pDoc->m_vehicleTags.GetNode(m_nSelectedSet);
	if(!pNode)return;
	int nCount = pDoc->m_vehicleTags.GetItemCountInCurSelNode();
	for( int i = 0; i < nCount; i++ )
	{
		pItem = pNode->GetItem(i);
		int nNewIndex = m_lstFlightTypes.AddString(pItem->m_vehicleItem.getName());
		m_lstFlightTypes.SetItemData(nNewIndex,(DWORD_PTR)pItem);
	}
	m_lstFlightTypes.SetCurSel(pDoc->m_vehicleTags.GetCurSelItemIndexInCurSelNode());
}

void CDlgVehicleTags::OnItemChangedListSets(void)
{
	m_nSelectedSet = m_lstSets.GetCurSel();
	CTermPlanDoc* pDoc = (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();
	if(!pDoc)return;
	pDoc->m_vehicleTags.SetCurSelNodeIndex(m_nSelectedSet);
	UpdateTagData();
}

void CDlgVehicleTags::OnAddSet()
{
	CTermPlanDoc* pDoc = (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();
	if(!pDoc)return;
	pDoc->m_vehicleTags.AddNode(_T("Default"));
	int nSetCount = pDoc->m_vehicleTags.GetNodeCount();
	pDoc->m_vehicleTags.AddTagItemToNode(nSetCount - 1,-1,FALSE,0x0);
	m_nSelectedSet = m_lstSets.AddString(_T("Default"));
	m_lstSets.SetCurSel(m_nSelectedSet);
	pDoc->m_vehicleTags.SetCurSelNodeIndex(nSetCount - 1);
	UpdateTagData();	
	m_bNewSet = TRUE;
}

void CDlgVehicleTags::OnDeleteSet()
{
	if(m_lstSets.GetCount() < 2) {
		AfxMessageBox("At least one Named Property Set must exist.");
		return;
	}
	CTermPlanDoc* pDoc = (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();
	if(!pDoc)return;
	ASSERT(m_nSelectedSet >= 0 && m_nSelectedSet < pDoc->m_vehicleTags.GetNodeCount());
	m_vrTagNodeDeleted.push_back(pDoc->m_vehicleTags.DeleteNode(m_nSelectedSet) );
	m_nSelectedSet = pDoc->m_vehicleTags.GetCurSelNodeIndex();

	UpdateSetList();

}

void CDlgVehicleTags::OnEndLabelEditListSets(void) 
{

	CTermPlanDoc* pDoc = (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();
	if(!pDoc)return;

	CString strTitle = _T("");
	m_lstSets.GetText(m_lstSets.GetCurSel(),strTitle);
	if(strTitle.IsEmpty()) {
		if(m_bNewSet) {
			//delete this new pdp set
			OnDeleteSet();	
		}
		return;
	}


	m_bNewSet=FALSE;

	int nIdx = m_lstSets.GetCurSel();

	ASSERT(pDoc->m_vehicleTags.GetCurSelNodeIndex() == nIdx);
	pDoc->m_vehicleTags.UpdateNodeName(nIdx,strTitle); 

	//m_lstSets.SetItemText(nIdx, 0, pDispInfo->item.pszText);

	
}

void CDlgVehicleTags::FlushDeletedItem(void)
{
	for(std::vector<CVehicleTagItem *>::iterator itrTagItemDeleted = m_vrTagItemDeleted.begin();\
		itrTagItemDeleted != m_vrTagItemDeleted.end(); ++ itrTagItemDeleted)
	{
		bool bHasParent = false;
		for(std::vector<CVehicleTagNode *>::iterator itrTagNodeDeleted = m_vrTagNodeDeleted.begin();\
			itrTagNodeDeleted != m_vrTagNodeDeleted.end(); ++ itrTagNodeDeleted)
		{
			if((*itrTagNodeDeleted)->GetID() == -1)continue;
			if( (*itrTagItemDeleted)->GetParentID() == (*itrTagNodeDeleted)->GetID() )
			{
				bHasParent = true;
				break;
			}
		}
		if((*itrTagItemDeleted) )
		{
			if((*itrTagItemDeleted)->GetID() != -1 && !bHasParent)
				(*itrTagItemDeleted)->DeleteData();
			delete (*itrTagItemDeleted);
		}
		
	}
	m_vrTagItemDeleted.clear();

	for(std::vector<CVehicleTagNode *>::iterator itrTagNodeDeleted = m_vrTagNodeDeleted.begin();\
		itrTagNodeDeleted != m_vrTagNodeDeleted.end(); ++ itrTagNodeDeleted)
	{ 
		if(*itrTagNodeDeleted)
		{
			if((*itrTagNodeDeleted)->GetID() != -1)
				(*itrTagNodeDeleted)->DeleteData();
			delete (*itrTagNodeDeleted);
		}
	}
	m_vrTagNodeDeleted.clear();
}

LRESULT CDlgVehicleTags::OnEditedListBox( WPARAM wparam, LPARAM lparam)
{
	CTermPlanDoc* pDoc = (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();
	if(!pDoc)return (0);

	CString strTitle = _T("");
	m_lstSets.GetText(m_lstSets.GetCurSel(),strTitle);
	if(strTitle.IsEmpty()) {
		if(m_bNewSet) {
			//delete this new pdp set
			OnDeleteSet();	
		}
		return (0);
	}


	m_bNewSet=FALSE;

	int nIdx = m_lstSets.GetCurSel();

	ASSERT(pDoc->m_vehicleTags.GetCurSelNodeIndex() == nIdx);
	pDoc->m_vehicleTags.UpdateNodeName(nIdx,strTitle); 

	return (0);
}

CTermPlanDoc* CDlgVehicleTags::GetDocument() const
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

void CDlgVehicleTags::UpdateVehicleTagIn3D()
{
	GetDocument()->UpdateAllViews(NULL, VM_RELOAD_VEHICLE_TAG_PROPERTY);
}
