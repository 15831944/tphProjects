// GatePriorityDlg.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "termplan.h"
#include "DlgStd2GateMapping.h"
#include "inputs\GatePriorityInfo.h"
#include "GateAssignDlg.h"
#include "ProcesserDialog.h"
#include "inputs\in_term.h"
#include ".\SelectALTObjectDialog.h"
#include "DlgNewStd2GateMapping.h"
#include "DlgShowStd2GateMappingDetail.h"
#include "DlgGateAssignPreference.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGatePriorityDlg dialog

CDlgStd2GateMapping::CDlgStd2GateMapping(InputTerminal* _pInTerm,int nAirportID, CString strProjectPath, EnumGateType enumGateType, CStand2GateConstraint * pConstraint, CWnd* pParent /*=NULL*/)
: CDialog(CDlgStd2GateMapping::IDD, pParent)
,m_pConstraint(pConstraint)
,m_pInTerm(_pInTerm)
,m_nAirportID(nAirportID)
,m_strProjectPath(strProjectPath)
,m_constrTemp(0)
{
	m_enumGateType = enumGateType;
	ASSERT(m_pConstraint);
	m_constrTemp = pConstraint;
}

CDlgStd2GateMapping::~CDlgStd2GateMapping(void)
{
}

void CDlgStd2GateMapping::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGatePriorityDlg)
	DDX_Control(pDX, IDC_LIST_GATE_PRIORITY, m_listctrlData);
	DDX_Control(pDX, IDC_STATIC_TOPIC, m_wndStatic);
	DDX_Control(pDX, IDC_STATIC_TOOLBARCONTENTER, m_toolbarcontenter);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgStd2GateMapping, CDialog)
	//{{AFX_MSG_MAP(CGatePriorityDlg)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_COMMAND(ID_PEOPLEMOVER_DELETE, OnPeoplemoverDelete)
	ON_COMMAND(ID_PEOPLEMOVER_NEW, OnPeoplemoverNew)
	ON_BN_CLICKED(IDC_BUTTON_SAVE, OnButtonPreference)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_GATE_PRIORITY, OnItemchangedListGatePriority)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_GATE_PRIORITY, OnDblclkListGatePriority)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGatePriorityDlg message handlers

BOOL CDlgStd2GateMapping::OnInitDialog() 
{
	CDialog::OnInitDialog();

// 	SetResize(IDC_STATIC_TOPIC,SZ_TOP_LEFT,SZ_TOP_RIGHT);
// 	SetResize(m_ToolBar.GetDlgCtrlID(),SZ_TOP_LEFT,SZ_TOP_LEFT);
// 	SetResize(IDC_LIST_GATE_PRIORITY,SZ_TOP_LEFT,SZ_BOTTOM_RIGHT);
// 	SetResize(IDC_BUTTON_SAVE,SZ_BOTTOM_RIGHT,SZ_BOTTOM_RIGHT);
// 	SetResize(IDOK,SZ_BOTTOM_RIGHT,SZ_BOTTOM_RIGHT);
// 	SetResize(IDCANCEL,SZ_BOTTOM_RIGHT,SZ_BOTTOM_RIGHT);
	//create the tool bar here.
	InitToolbar();

	//set the style of the list control.
	SetListCtrl();

	ShowList();

	//GetDlgItem(IDOK)->EnableWindow(FALSE);
	if (m_enumGateType == 0x1) {
		SetWindowText(_T("Stand to Arrival Gate Mapping Specification"));
	}else if (m_enumGateType == 0x2) {
		SetWindowText(_T("Stand to Departure Gate Mapping Specification"));
	}

	GetDlgItem(IDC_BUTTON_SAVE)->SetWindowText(_T("Constraints"));
	GetDlgItem(IDC_STATIC_TOPIC)->SetWindowText(_T("Mapping list:"));

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgStd2GateMapping::InitToolbar()
{
	CRect rc;
	m_toolbarcontenter.GetWindowRect( &rc );
	ScreenToClient(&rc);
	m_ToolBar.MoveWindow(&rc);
	m_ToolBar.ShowWindow(SW_SHOW);
	m_toolbarcontenter.ShowWindow(SW_HIDE);
	m_ToolBar.GetToolBarCtrl().EnableButton( ID_PEOPLEMOVER_NEW );
	m_ToolBar.GetToolBarCtrl().EnableButton( ID_PEOPLEMOVER_DELETE,FALSE );
	m_ToolBar.GetToolBarCtrl().HideButton( ID_BUTTON_UP, TRUE);
	m_ToolBar.GetToolBarCtrl().HideButton( ID_BUTTON_DOWN, TRUE);
	m_ToolBar.GetToolBarCtrl().HideButton( ID_BUTTON_EDIT, TRUE);
}

int CDlgStd2GateMapping::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO: Add your specialized creation code here
	if (!m_ToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_ToolBar.LoadToolBar(IDR_PRIORITY_ASSIGNMENT))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}	
	return 0;
}

void CDlgStd2GateMapping::SetListCtrl()
{
	DWORD dwStyle = m_listctrlData.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_EX_GRIDLINES;
	m_listctrlData.SetExtendedStyle( dwStyle );

	LV_COLUMNEX	lvc;
	lvc.mask = LVCF_WIDTH | LVCF_TEXT ;
	char columnLabel[3][128] = {0};
	sprintf( columnLabel[0], "Stand (Group)" );
	if (m_enumGateType == 0x1) {//arr
		sprintf( columnLabel[1], "Arrival Gate (Group)" );
	}else if (m_enumGateType == 0x2) {//dep
		sprintf( columnLabel[1], "Departure Gate (Group)" );
	}
	sprintf( columnLabel[2], "Comment" );
	int DefaultColumnWidth[] = { 150,150,150};	
	int nFormat[] = {	LVCFMT_CENTER, LVCFMT_CENTER,LVCFMT_CENTER };
	for( int i=0; i<3; i++ )
	{
		//temporary string list.
		CStringList strList;
		strList.RemoveAll();
		lvc.csList = &strList;
		lvc.pszText = columnLabel[i];
		lvc.cx = DefaultColumnWidth[i];
		lvc.fmt = LVCFMT_POPUP_GATE_SELECTION;
		m_listctrlData.InsertColumn( i, &lvc );	
	}
}


void CDlgStd2GateMapping::OnPeoplemoverDelete()
{
	int nIndex = m_listctrlData.GetCurSel();
	if(nIndex < 0 )
		return;
	m_listctrlData.DeleteItem(nIndex);

	if (m_enumGateType == GateType_ARR)
	{
		m_constrTemp->m_pStand2Arrgateconstraint.erase(m_constrTemp->m_pStand2Arrgateconstraint.begin() + nIndex);
	}
	else if (m_enumGateType == GateType_DEP)
	{
		m_constrTemp->m_pStand2Depgateconstraint.erase(m_constrTemp->m_pStand2Depgateconstraint.begin() + nIndex);
	}
	GetDlgItem(IDOK)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
}

void CDlgStd2GateMapping::OnPeoplemoverNew()
{
	CDlgNewStd2GateMapping dlgNew(m_pInTerm,m_nAirportID,m_enumGateType);
	if(IDOK == dlgNew.DoModal())
	{
		CStand2GateMapping itemNew;
		int nCount = m_listctrlData.GetItemCount();
		itemNew.m_StandID = dlgNew.m_StandID;
		m_listctrlData.InsertItem(nCount, dlgNew.m_StandID.GetIDString());

		if (m_enumGateType == GateType_ARR)
		{//arr
			itemNew.m_ArrGateID = dlgNew.m_GateID;
			m_listctrlData.SetItemText(nCount, 1,dlgNew.m_GateID.GetIDString());
			itemNew.m_DepGateID.SetStrDict(m_pInTerm->inStrDict);
		}
		else if(m_enumGateType == GateType_DEP)
		{//dep
			itemNew.m_DepGateID = dlgNew.m_GateID;
			m_listctrlData.SetItemText(nCount, 1,dlgNew.m_GateID.GetIDString());
			itemNew.m_ArrGateID.SetStrDict(m_pInTerm->inStrDict);
		}
		itemNew.m_enumMappingFlag = (CStand2GateMapping::MapType)dlgNew.m_iMappingFlag;

		if (dlgNew.m_iMappingFlag == CStand2GateMapping::MapType_1to1) 
			m_listctrlData.SetItemText(nCount, 2,_T("1:1 mapping"));
		else if (dlgNew.m_iMappingFlag == CStand2GateMapping::MapType_Random) 
			m_listctrlData.SetItemText(nCount, 2,_T("random mapping."));
		m_listctrlData.SetItemData(nCount,nCount);


		if (m_enumGateType == GateType_ARR)
		{
			m_constrTemp->m_pStand2Arrgateconstraint.push_back(itemNew);
		}
		else if(m_enumGateType = GateType_DEP)
		{
			m_constrTemp->m_pStand2Depgateconstraint.push_back(itemNew);
		}
		GetDlgItem(IDOK)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
	}
}

void CDlgStd2GateMapping::ShowList()
{
	m_listctrlData.DeleteAllItems();

	int nCount;
	int i;
	if (m_enumGateType == GateType_ARR)
	{
		nCount = (int)m_constrTemp->m_pStand2Arrgateconstraint.size();
		for ( i = 0; i < nCount; ++i)
		{
			m_listctrlData.InsertItem(i,m_constrTemp->m_pStand2Arrgateconstraint[i].m_StandID.GetIDString());
			m_listctrlData.SetItemText(i, 1,m_constrTemp->m_pStand2Arrgateconstraint[i].m_ArrGateID.GetIDString());
			if (m_constrTemp->m_pStand2Arrgateconstraint[i].m_enumMappingFlag == 0x1)
				m_listctrlData.SetItemText(i, 2,_T("1:1 mapping"));
			else if (m_constrTemp->m_pStand2Arrgateconstraint[i].m_enumMappingFlag == 0x2)
			{
				m_listctrlData.SetItemText(i, 2,_T("random mapping"));
			}
			m_listctrlData.SetItemData(i,i);
		}
	}
	else if (m_enumGateType == GateType_DEP)
	{
		nCount = (int)m_constrTemp->m_pStand2Depgateconstraint.size();
		for ( i = 0; i < nCount; ++i)
		{
			m_listctrlData.InsertItem(i,m_constrTemp->m_pStand2Depgateconstraint[i].m_StandID.GetIDString());
			m_listctrlData.SetItemText(i, 1,m_constrTemp->m_pStand2Depgateconstraint[i].m_DepGateID.GetIDString());
			if (m_constrTemp->m_pStand2Depgateconstraint[i].m_enumMappingFlag == 0x1)
				m_listctrlData.SetItemText(i, 2,_T("1:1 mapping"));
			else if (m_constrTemp->m_pStand2Depgateconstraint[i].m_enumMappingFlag == 0x2)
			{
				m_listctrlData.SetItemText(i, 2,_T("random mapping"));
			}
			m_listctrlData.SetItemData(i,i);
		}
	}
}

void CDlgStd2GateMapping::OnButtonSave() 
{
	m_pConstraint->saveDataSet(m_strProjectPath,false);
	//GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(FALSE);
}
void CDlgStd2GateMapping::OnButtonPreference()
{
	if (m_enumGateType == GateType_ARR) 
	{
		CDlgArrivalGateAssignPreference DlgArrivalGate(m_pConstraint->GetArrivalPreferenceMan(),m_pInTerm,this) ;
		DlgArrivalGate.DoModal() ;
	}
	else if (m_enumGateType == GateType_DEP) 
	{
		CDlgDepGateAssignPreference DlgArrivalGate(m_pConstraint->GetDepPreferenceMan(),m_pInTerm,this) ;
		DlgArrivalGate.DoModal() ;	
	}
}
void CDlgStd2GateMapping::OnOK() 
{
	OnButtonSave();
	CDialog::OnOK();
}
void CDlgStd2GateMapping::OnCancel()
{
  m_pConstraint->loadDataSet(m_strProjectPath) ;
  CDialog::OnCancel() ;
}
void CDlgStd2GateMapping::OnItemchangedListGatePriority(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

	POSITION pos = m_listctrlData.GetFirstSelectedItemPosition();
	if (pos)
		m_ToolBar.GetToolBarCtrl().EnableButton( ID_PEOPLEMOVER_DELETE,TRUE );
	else
		m_ToolBar.GetToolBarCtrl().EnableButton( ID_PEOPLEMOVER_DELETE,FALSE );

	*pResult = 0;
}

void CDlgStd2GateMapping::OnDblclkListGatePriority(NMHDR* pNMHDR, LRESULT* pResult) 
{	
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	if(pNMListView->iItem < 0)
		return;
	
//	int nSelItem = -1;
//
//	nSelItem = m_listctrlData.GetItemData(pNMListView->iItem);
//	if (nSelItem >= 0) {
//		CDlgNewStd2GateMapping dlgDetail(m_pInTerm,m_nAirportID,m_nFlag);
//// 		dlgDetail.m_StandID = m_constrTemp[nSelItem].m_StandID;
//// 		if (m_nFlag == 0x1) 
//// 			dlgDetail.m_GateID  = m_constrTemp[nSelItem].m_ArrGateID;
//// 		else if (m_nFlag == 0x2) 
//// 			dlgDetail.m_GateID  = m_constrTemp[nSelItem].m_DepGateID;
//// 		dlgDetail.m_iMappingFlag = m_constrTemp[nSelItem].m_iMappingFlag;
//
//		if(IDOK == dlgDetail.DoModal())
//		{
//			if (m_nFlag == 0x1)
//			{
//				m_constrTemp->m_pStand2Arrgateconstraint[nSelItem].m_StandID = dlgDetail.m_StandID;
//				m_constrTemp->m_pStand2Arrgateconstraint[nSelItem].m_ArrGateID = dlgDetail.m_GateID;
//				m_listctrlData.SetItemText(nSelItem,0,m_constrTemp->m_pStand2Arrgateconstraint[nSelItem].m_StandID.GetIDString());
//				m_listctrlData.SetItemText(nSelItem,1,m_constrTemp->m_pStand2Arrgateconstraint[nSelItem].m_ArrGateID.GetIDString());
//			}
//			else if (m_nFlag == 0x2)
//			{
//				CString strTemp = dlgDetail.m_GateID.GetIDString();
//				m_constrTemp->m_pStand2Depgateconstraint[nSelItem].m_StandID = dlgDetail.m_StandID;
//				m_constrTemp->m_pStand2Depgateconstraint[nSelItem].m_DepGateID = dlgDetail.m_GateID;
//				m_listctrlData.SetItemText(nSelItem,0,m_constrTemp->m_pStand2Depgateconstraint[nSelItem].m_StandID.GetIDString());
//				m_listctrlData.SetItemText(nSelItem,1,m_constrTemp->m_pStand2Depgateconstraint[nSelItem].m_DepGateID.GetIDString());
//			}
//		}
//	}
	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
	GetDlgItem(IDOK)->EnableWindow(TRUE);
	*pResult = 0;
}

void CDlgStd2GateMapping::OnSize (UINT nType, int cx,int cy )
{
	CDialog::OnSize(nType,cx,cy);
	CRect rect;
	CRect rc;

	if(m_wndStatic.m_hWnd != NULL)
	{
		m_wndStatic.SetWindowPos(NULL,14,12,cx-14,20,NULL);
		m_wndStatic.GetWindowRect(&rect);
		ScreenToClient(&rect);


		m_ToolBar.SetWindowPos(NULL,14,rect.bottom+2,cx-14,26,NULL);
		m_ToolBar.GetWindowRect(&rect);
		ScreenToClient(&rect);

		m_listctrlData.SetWindowPos(NULL,rect.left,rect.bottom + 2,cx-2*rect.left,cy-2*rect.bottom+12,NULL);
		m_listctrlData.GetWindowRect(&rc);
		ScreenToClient(&rc);
		GetDlgItem(IDCANCEL)->GetWindowRect(&rect);
		ScreenToClient(&rect);
		
		GetDlgItem(IDC_BUTTON_SAVE)->SetWindowPos(NULL,rc.right-3*rect.Width()-12,rc.bottom+15,rect.Width(),rect.Height(),NULL);
		GetDlgItem(IDOK)->SetWindowPos(NULL,cx-2*rect.Width()-17,rc.bottom+15,rect.Width(),rect.Height(),NULL);
		GetDlgItem(IDCANCEL)->SetWindowPos(NULL,cx-rect.Width()-10,rc.bottom+15,rect.Width(),rect.Height(),NULL);

		InvalidateRect(CRect(0,0,cx,cy),TRUE);
	}
}