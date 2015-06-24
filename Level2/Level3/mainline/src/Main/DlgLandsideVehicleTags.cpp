// DlgVehicleTags.cpp : implementation file
//

#include "stdafx.h"
#include "DlgLandsideVehicleTags.h"
#include "TermPlanDoc.h"
#include "ViewMsg.h"
#include "DlgSelectLandsideVehicleType.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



static const UINT checkboxIDs[] = {
	IDC_CHK_SHOWTAG,	
		IDC_CHK_MBID,
		IDC_CHK_FLTID,
		IDC_CHK_MBTYPE,
		IDC_CHK_STATE,	
		IDC_CHK_POS,
		IDC_CHK_PLACE,
		IDC_CHK_SPEED
};

/////////////////////////////////////////////////////////////////////////////
// CDlgLandsideVehicleTags dialog

IMPLEMENT_DYNAMIC(CDlgLandsideVehicleTags, CXTResizeDialog)
CDlgLandsideVehicleTags::CDlgLandsideVehicleTags(CWnd* pParent /*=NULL*/)
: CXTResizeDialog(CDlgLandsideVehicleTags::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgLandsideVehicleTags)
	m_bShowTag = FALSE;
	m_bChkMbID = FALSE;
	m_bChkFltID = FALSE;
	m_bChkMbType = FALSE;
	m_bChkState = FALSE;
	m_bChkPos = FALSE;
	m_bChkPlace = FALSE;
	m_bChkSpeed = FALSE;

	//}}AFX_DATA_INIT
	m_nSelectedSet = -1;
	m_bNewSet = FALSE;
	m_landsideVehicleTags.ReadData(GetDocument()->GetProjectID());
}

CDlgLandsideVehicleTags::~CDlgLandsideVehicleTags()
{

}

void CDlgLandsideVehicleTags::DoDataExchange(CDataExchange* pDX)
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
	DDX_Check(pDX, IDC_CHK_MBID, m_bChkMbID);
	DDX_Check(pDX, IDC_CHK_FLTID, m_bChkFltID);
	DDX_Check(pDX, IDC_CHK_MBTYPE, m_bChkMbType);
	DDX_Check(pDX, IDC_CHK_STATE, m_bChkState);
	DDX_Check(pDX, IDC_CHK_POS, m_bChkPos); 
	DDX_Check(pDX, IDC_CHK_PLACE, m_bChkPlace);
	DDX_Check(pDX, IDC_CHK_SPEED, m_bChkSpeed);
}


BEGIN_MESSAGE_MAP(CDlgLandsideVehicleTags, CXTResizeDialog)
	//{{AFX_MSG_MAP(CDlgLandsideVehicleTags)
	ON_WM_CREATE()
	ON_COMMAND(ID_TOOLBARBUTTONADD, OnToolbarbuttonadd)
	ON_COMMAND(ID_TOOLBARBUTTONDEL, OnToolbarbuttondel)
	//ON_NOTIFY(LVN_GETDISPINFO, IDC_LIST_PAXTYPES, OnGetDispInfo)
	ON_LBN_SELCHANGE(IDC_LIST_PAXTYPES, OnListItemClick)
	ON_BN_CLICKED(IDC_CHK_SHOWTAG, OnChkShowtag)
	ON_BN_CLICKED(IDC_CHK_MBID, OnChkMobileID)
	ON_BN_CLICKED(IDC_CHK_FLTID, OnChkFlightID)
	ON_BN_CLICKED(IDC_CHK_MBTYPE, OnChkMobileType)
	ON_BN_CLICKED(IDC_CHK_STATE, OnChkState)
	ON_BN_CLICKED(IDC_CHK_POS, OnChkPostion) 
	ON_BN_CLICKED(IDC_CHK_PLACE, OnChkPlace) 
	ON_BN_CLICKED(IDC_CHK_SPEED, OnChkSpeed) 
	ON_MESSAGE(WM_APP_LB_ITEM_EDITED,OnEditedListBox)
	ON_BN_CLICKED(IDC_BUTTON_SAVE, OnSave)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
	ON_LBN_SELCHANGE(IDC_LIST_DISPPROP, OnItemChangedListSets)
	ON_COMMAND(ID_PEOPLEMOVER_NEW, OnAddSet)
	ON_COMMAND(ID_PEOPLEMOVER_DELETE, OnDeleteSet)
	//ON_LBN_SELCANCEL(IDC_LIST_DISPPROP, OnEndLabelEditListSets)	
END_MESSAGE_MAP()


void CDlgLandsideVehicleTags::EnableAllCheckboxes(BOOL bEnable)
{
	CWnd* pWnd = GetDlgItem(IDC_CHK_SHOWTAG);
	if(!pWnd)return;
	pWnd->EnableWindow(bEnable);
	EnableSubCheckboxes(bEnable);
}

void CDlgLandsideVehicleTags::EnableSubCheckboxes(BOOL bEnable)
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

void CDlgLandsideVehicleTags::InitToolbar()
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
void CDlgLandsideVehicleTags::SaveCurrentData()
{ 
	int idx = m_lstFlightTypes.GetCurSel();
	if(idx != LB_ERR )
	{
		DWORD taginfo = 0x0000000000000000;
		taginfo += m_bChkMbID?0x0000000000000001:0x0000000000000000;
		taginfo += m_bChkFltID?0x0000000000000002:0x0000000000000000;
		taginfo += m_bChkMbType?0x0000000000000004:0x0000000000000000;
		taginfo += m_bChkState?0x0000000000000008:0x0000000000000000;
		taginfo += m_bChkPos?0x0000000000000010:0x0000000000000000;
		taginfo += m_bChkPlace?0x0000000000000020:0x0000000000000000;
		taginfo += m_bChkSpeed?0x0000000000000040:0x0000000000000000;

		CLandsideVehicleTypes* pItem = (CLandsideVehicleTypes*) m_lstFlightTypes.GetItemData(idx);
		pItem->m_dwTagInfo = taginfo;
	}
}
void CDlgLandsideVehicleTags::SetAllCheckboxes()
{
	int idx = m_lstFlightTypes.GetCurSel();
	if(idx != LB_ERR ) {
		//enable "show tags" checkbox
		CWnd* pWnd = GetDlgItem(IDC_CHK_SHOWTAG);
		if(!pWnd)return;
		pWnd->EnableWindow(TRUE);

		//get item data
		CLandsideVehicleTypes* pItem = (CLandsideVehicleTypes*) m_lstFlightTypes.GetItemData(idx);
		ASSERT(pItem != NULL);
		//show tags?
		m_bShowTag = pItem->m_bShowTags;
		EnableSubCheckboxes(m_bShowTag);
		DWORD dwTagInfo = pItem->m_dwTagInfo;
		m_bChkMbID = dwTagInfo & 0x0000000000000001?TRUE:FALSE;
		m_bChkFltID = dwTagInfo & 0x0000000000000002?TRUE:FALSE;
		m_bChkMbType = dwTagInfo & 0x0000000000000004?TRUE:FALSE;
		m_bChkState = dwTagInfo & 0x0000000000000008?TRUE:FALSE;
		m_bChkPos = dwTagInfo & 0x0000000000000010?TRUE:FALSE;
		m_bChkPlace = dwTagInfo & 0x0000000000000020?TRUE:FALSE;
		m_bChkSpeed = dwTagInfo & 0x0000000000000040?TRUE:FALSE;

		UpdateData(FALSE);
	}
	else {
		EnableAllCheckboxes(FALSE);
	}
}

/////////////////////////////////////////////////////////////////////////////
// CDlgAircraftTags message handlers

BOOL CDlgLandsideVehicleTags::OnInitDialog() 
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
	SetResize(IDC_CHK_MBID, SZ_MIDDLE_CENTER, SZ_MIDDLE_CENTER);
	SetResize(IDC_CHK_FLTID, SZ_MIDDLE_CENTER, SZ_MIDDLE_CENTER);
	SetResize(IDC_CHK_MBTYPE, SZ_MIDDLE_CENTER, SZ_MIDDLE_CENTER);
	SetResize(IDC_CHK_STATE, SZ_MIDDLE_CENTER, SZ_MIDDLE_CENTER);
	SetResize(IDC_CHK_POS, SZ_MIDDLE_CENTER, SZ_MIDDLE_CENTER);
	SetResize(IDC_CHK_PLACE, SZ_MIDDLE_CENTER, SZ_MIDDLE_CENTER);
	SetResize(IDC_CHK_SPEED, SZ_MIDDLE_CENTER, SZ_MIDDLE_CENTER);

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

int CDlgLandsideVehicleTags::OnCreate(LPCREATESTRUCT lpCreateStruct) 
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

void CDlgLandsideVehicleTags::OnToolbarbuttonadd() 
{
	CLandsideVehicleTagSet * pNode = m_landsideVehicleTags.GetNode(m_nSelectedSet);
	if(!pNode)return;
	
	CDlgSelectLandsideVehicleType dlg;
	CString strName;
	if(dlg.DoModal() == IDOK)
	{	
		strName = dlg.GetName();
		if (strName.IsEmpty())
		{
			MessageBox(_T("Please define vehicle type name in Vehicle Type Definition Dialog!."),_T("Warning"),MB_OK);
		}
		else if (pNode->ExistSameName(strName))
		{
			MessageBox(_T("Exist the same group name!."),_T("Warning"),MB_OK);
			return;
		}
	}
	else
		return;
	CLandsideVehicleTypes * pItem = new CLandsideVehicleTypes(NULL);
	pItem->SetVehicleTypeName(strName);
	pNode->AddItem(pItem);
	int nCount = m_lstFlightTypes.GetCount();

	nCount = m_lstFlightTypes.InsertString(nCount,pItem->m_vehicleName); 
	m_lstFlightTypes.SetItemData(nCount,(DWORD_PTR)pItem);
	m_landsideVehicleTags.SetCurSelItemIndexInCurSelNode(pNode->GetItemCount() - 1);
	m_lstFlightTypes.SetCurSel(nCount);

	SetAllCheckboxes();
	m_btnSave.EnableWindow();
}

void CDlgLandsideVehicleTags::OnToolbarbuttondel() 
{
	int idx = m_lstFlightTypes.GetCurSel();
	if(idx != LB_ERR )
	{
		if(idx == 0) 
		{
			AfxMessageBox(_T("Can not delete the item \"DEFAULT\"") );
			return;
		} 
		if(idx == 1) 
		{
			AfxMessageBox(_T("Can not delete the item \"OVERLAP\"") );
			return;
		} 
		else 
		{

			// delete from db
			m_vrTagItemDeleted.push_back(m_landsideVehicleTags.DeleteItemInNode(m_nSelectedSet,idx) );
			m_lstFlightTypes.DeleteString(idx);
			m_lstFlightTypes.SetCurSel(m_lstFlightTypes.GetCount()-1);
			SetAllCheckboxes();
		}
	}
	m_btnSave.EnableWindow();
}

void CDlgLandsideVehicleTags::OnGetDispInfo(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
	if(pDispInfo->item.mask & LVIF_TEXT) {
		CLandsideVehicleTypes * pItem = (CLandsideVehicleTypes*) pDispInfo->item.lParam;
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

void CDlgLandsideVehicleTags::OnListItemClick( void) 
{	
	m_landsideVehicleTags.SetCurSelItemIndexInCurSelNode(m_lstFlightTypes.GetCurSel()); 
	m_lstFlightTypes.SetCurSel(m_lstFlightTypes.GetCurSel());
	SetAllCheckboxes(); 
}

void CDlgLandsideVehicleTags::OnChkShowtag() 
{
	UpdateData(); 
	int idx = m_lstFlightTypes.GetCurSel();
	if(idx != LB_ERR) {
		//get item data
		CLandsideVehicleTypes* pItem = (CLandsideVehicleTypes*) m_lstFlightTypes.GetItemData(idx);
		ASSERT(pItem != NULL);
		//set "show tags" data
		pItem->m_bShowTags = m_bShowTag;
	}
	SetAllCheckboxes();
	m_btnSave.EnableWindow();
}

void CDlgLandsideVehicleTags::OnChkItem(DWORD dwMask, BOOL bIsCheck)
{
	int idx = m_lstFlightTypes.GetCurSel();
	if(idx != LB_ERR) {
		//get item data
		CLandsideVehicleTypes* pItem = (CLandsideVehicleTypes*) m_lstFlightTypes.GetItemData(idx);
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
	m_btnSave.EnableWindow();
}

void CDlgLandsideVehicleTags::OnChkMobileID() 
{
	UpdateData();
	OnChkItem(0x0000000000000001, m_bChkMbID);
	SaveCurrentData();
	m_btnSave.EnableWindow();
}
void CDlgLandsideVehicleTags::OnChkFlightID() 
{
	UpdateData();
	OnChkItem(0x0000000000000002, m_bChkFltID);
	SaveCurrentData();
	m_btnSave.EnableWindow();
}
void CDlgLandsideVehicleTags::OnChkMobileType() 
{
	UpdateData();
	OnChkItem(0x0000000000000004, m_bChkMbType);
	SaveCurrentData();
	m_btnSave.EnableWindow();
}
void CDlgLandsideVehicleTags::OnChkState() 
{
	UpdateData();
	OnChkItem(0x0000000000000008, m_bChkState);
	SaveCurrentData();
	m_btnSave.EnableWindow();
}

void CDlgLandsideVehicleTags::OnChkPostion() 
{
	UpdateData();
	OnChkItem(0x0000000000000010, m_bChkPos);
	SaveCurrentData();
	m_btnSave.EnableWindow();
}

void CDlgLandsideVehicleTags::OnChkPlace() 
{
	UpdateData();
	OnChkItem(0x0000000000000020, m_bChkPlace);
	SaveCurrentData();
	m_btnSave.EnableWindow();
}

void CDlgLandsideVehicleTags::OnChkSpeed() 
{
	UpdateData();
	OnChkItem(0x0000000000000040, m_bChkSpeed);
	SaveCurrentData();
	m_btnSave.EnableWindow();
}

void CDlgLandsideVehicleTags::OnOK() 
{
	OnSave();	
	CXTResizeDialog::OnOK();
}

void CDlgLandsideVehicleTags::OnSave() 
{
	m_landsideVehicleTags.SaveData();
	UpdateVehicleTagIn3D();
	FlushDeletedItem();
	m_btnSave.EnableWindow(FALSE);
}

void CDlgLandsideVehicleTags::OnCancel() 
{
	CXTResizeDialog::OnCancel();
}

void CDlgLandsideVehicleTags::OnSize(UINT nType, int cx, int cy) 
{
	CXTResizeDialog::OnSize(nType, cx, cy);	
}

void CDlgLandsideVehicleTags::UpdateSetList()
{
	m_lstSets.ResetContent();
	int nSetCount = m_landsideVehicleTags.GetNodeCount();
	for(int i=0; i<nSetCount; i++) {
		int nNewIndex = m_lstSets.AddString(m_landsideVehicleTags.GetNodeName(i));
	}
	m_nSelectedSet = m_landsideVehicleTags.GetCurSelNodeIndex();
	m_lstSets.SetCurSel(m_nSelectedSet);
	UpdateTagData();
}

void CDlgLandsideVehicleTags::UpdateTagData()
{
	m_lstFlightTypes.ResetContent();

	// Load data.
	CLandsideVehicleTagSet* pNode = 0;
	CLandsideVehicleTypes* TypeItem = 0;

	pNode = m_landsideVehicleTags.GetNode(m_nSelectedSet);
	if(!pNode)return;

	TypeItem = pNode->GetDefaultTagItem();
	m_lstFlightTypes.AddString(TypeItem->GetVehicleTypeName());
	m_lstFlightTypes.SetItemData(0,(DWORD_PTR)TypeItem);

	TypeItem = pNode->GetOverlapTagItem();
	m_lstFlightTypes.AddString(TypeItem->GetVehicleTypeName());
	m_lstFlightTypes.SetItemData(1,(DWORD_PTR)TypeItem);

	int nCount = m_landsideVehicleTags.GetItemCountInCurSelNode();
	for( int i = 0; i < nCount; i++ )
	{
		TypeItem = pNode->GetItem(i);
		int nNewIndex = m_lstFlightTypes.AddString(TypeItem->GetVehicleTypeName());
		m_lstFlightTypes.SetItemData(nNewIndex,(DWORD_PTR)TypeItem);
	}
//	m_lstSets.SetCurSel(pDoc->m_landsideVehicleTags.GetCurSelNodeIndex());
	m_lstFlightTypes.SetCurSel(0);
	SetAllCheckboxes();
}

void CDlgLandsideVehicleTags::OnItemChangedListSets(void)
{
	m_nSelectedSet = m_lstSets.GetCurSel();

	m_landsideVehicleTags.SetCurSelNodeIndex(m_nSelectedSet);
	m_lstSets.SetCurSel(m_landsideVehicleTags.GetCurSelNodeIndex());
	UpdateTagData();
	m_btnSave.EnableWindow();
}

void CDlgLandsideVehicleTags::OnAddSet()
{
	m_landsideVehicleTags.AddNode(_T("Default"));
	int nSetCount = m_landsideVehicleTags.GetNodeCount();
	m_landsideVehicleTags.AddTagItemToNode(nSetCount - 1,-1,FALSE,0x0);
	m_nSelectedSet = m_lstSets.AddString(_T("Default"));
	m_lstSets.SetCurSel(m_nSelectedSet);
	m_landsideVehicleTags.SetCurSelNodeIndex(nSetCount - 1);
	UpdateTagData();	
	m_bNewSet = TRUE;
	m_btnSave.EnableWindow();
}

void CDlgLandsideVehicleTags::OnDeleteSet()
{
	if(m_lstSets.GetCount() < 2) {
		AfxMessageBox("At least one Named Property Set must exist.");
		return;
	}
	ASSERT(m_nSelectedSet >= 0 && m_nSelectedSet < m_landsideVehicleTags.GetNodeCount());
	m_vrTagNodeDeleted.push_back(m_landsideVehicleTags.DeleteNode(m_nSelectedSet) );
	m_landsideVehicleTags.SetCurSelNodeIndex(m_landsideVehicleTags.GetNodeCount()-1);
	UpdateSetList();
	m_btnSave.EnableWindow();
}

void CDlgLandsideVehicleTags::OnEndLabelEditListSets(void) 
{
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

	ASSERT(m_landsideVehicleTags.GetCurSelNodeIndex() == nIdx);
	m_landsideVehicleTags.UpdateNodeName(nIdx,strTitle); 
	m_btnSave.EnableWindow();
	//m_lstSets.SetItemText(nIdx, 0, pDispInfo->item.pszText);


}

void CDlgLandsideVehicleTags::FlushDeletedItem(void)
{
	for(std::vector<CLandsideVehicleTypes *>::iterator itrTagItemDeleted = m_vrTagItemDeleted.begin();\
		itrTagItemDeleted != m_vrTagItemDeleted.end(); ++ itrTagItemDeleted)
	{
		bool bHasParent = false;
		for(std::vector<CLandsideVehicleTagSet *>::iterator itrTagNodeDeleted = m_vrTagNodeDeleted.begin();\
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

	for(std::vector<CLandsideVehicleTagSet *>::iterator itrTagNodeDeleted = m_vrTagNodeDeleted.begin();\
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

LRESULT CDlgLandsideVehicleTags::OnEditedListBox( WPARAM wparam, LPARAM lparam)
{
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

	//ASSERT(pDoc->m_landsideVehicleTags.GetCurSelNodeIndex() == nIdx);
	m_landsideVehicleTags.UpdateNodeName(nIdx,strTitle); 
	m_btnSave.EnableWindow();
	return (0);
}

CTermPlanDoc* CDlgLandsideVehicleTags::GetDocument() const
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

void CDlgLandsideVehicleTags::UpdateVehicleTagIn3D()
{
	GetDocument()->UpdateAllViews(NULL, VM_RELOAD_LANDSIDE_VEHICLE_TAG_PROPERTY);
}
