// StarAssignmentDlg.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "StarAssignmentDlg.h"
//#include "InputAirside/FlightUseStar.h"
//#include "InputAirside/FlightUseStarList.h"
//#include "InputAirside/LandingRunwayStarAssignment.h"
//#include "InputAirside/LandingRunwayStarAssignmentList.h"
#include "InputAirside/InputAirside.h"
#include "InputAirside/ALTAirport.h"
#include "InputAirside/Runway.h"
#include "InputAirside/AirRoute.h"
#include "../Common/WinMsg.h"
#include "DlgTimeRange.h"
#include "../InputAirside/FlightStarAssignment.h"

namespace
{
	const UINT ID_NEW_TIME      = 10;
	const UINT ID_EDIT_TIME     = 11;
	const UINT ID_DEL_TIME      = 12;

	const UINT ID_NEW_STARTASSIGNMENT  = 20;
	const UINT ID_DEL_STARTASSIGNMENT  = 21;
}

// CStarAssignmentDlg dialog

IMPLEMENT_DYNAMIC(CStarAssignmentDlg, CXTResizeDialog)
CStarAssignmentDlg::CStarAssignmentDlg(int nProjID, PSelectFlightType pSelectFlightType, CAirportDatabase *pAirPortdb, CWnd* pParent /* = NULL */)
	: CXTResizeDialog(IDD_DIALOG_STARASSIGNMENT, pParent)
	, m_pSelectFlightType(pSelectFlightType)
	, m_nProjID(nProjID)
	, m_CurrentTreeItemCategory(TREEITEMCATE_NONE)
	, m_pAirportDB(pAirPortdb)

{
	//m_pLandingRunwayStarAssignmentList = new CLandingRunwayStarAssignmentList();
	//m_pLandingRunwayStarAssignmentList->SetAirportDatabase(pAirPortdb);
	//m_pLandingRunwayStarAssignmentList->ReadData(m_nProjID);
	m_pStarAssignmentList = new FlightStarAssignmentList;
	m_pStarAssignmentList->SetAirportDatabase(pAirPortdb);
	m_pStarAssignmentList->ReadData(m_nProjID);

	//read air routelist
	AirRouteList airRouteList;
	airRouteList.ReadData(m_nProjID);
    
	int nAirRouteCount = airRouteList.GetAirRouteCount();

	for (int i =0,j=0; i < nAirRouteCount; i++)
	{
		CAirRoute *pAirRoute = airRouteList.GetAirRoute(i);

		ASSERT(pAirRoute != NULL);
		
		if (CAirRoute::STAR == pAirRoute->getRouteType())
		{
			m_StarNameList.AddTail(pAirRoute->getName());
			m_AirRouteID.insert(Int_Pair(j, pAirRoute->getID()));
			j++;
		}
	}

	//DeleteDataWhichStarIsNotExist();

}

CStarAssignmentDlg::~CStarAssignmentDlg()
{
	//delete m_pLandingRunwayStarAssignmentList;
	delete m_pStarAssignmentList;
	m_pStarAssignmentList = NULL;
	int nCount = (int)m_vFlightTypeDelList.size();
	for (int i = 0; i < nCount; i++)
	{
		FlightStarAssignment* pItem = m_vFlightTypeDelList.at(i);
		if (pItem != NULL)
		{
			delete pItem;
			pItem = NULL;
		}

	}
	m_vFlightTypeDelList.clear();
}

void CStarAssignmentDlg::DoDataExchange(CDataExchange* pDX)
{
	CXTResizeDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_TIME, m_StaticToolBarTime);
	DDX_Control(pDX, IDC_STATIC_FILGHTTYPE, m_StaticToolBarStarAssignment);
	DDX_Control(pDX, IDC_TREE_LANDINGRUNWAY, m_wndTreeFltTime);
	DDX_Control(pDX, IDC_LIST_STARASSIGNMENT, m_StarAssigmentCtrl);
}


BEGIN_MESSAGE_MAP(CStarAssignmentDlg, CXTResizeDialog)
	ON_WM_CREATE()
	ON_BN_CLICKED(IDC_BUTTON_SAVE, OnBnClickedButtonSave)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_LANDINGRUNWAY, OnTvnSelchangedTreeFlightType)
	ON_COMMAND(ID_NEW_TIME, OnCmdNew)
	ON_COMMAND(ID_DEL_TIME, OnCmdDelete)
	ON_COMMAND(ID_EDIT_TIME, OnCmdEdit)
	ON_COMMAND(ID_NEW_STARTASSIGNMENT, OnCmdNewStarAssignment)
	ON_COMMAND(ID_DEL_STARTASSIGNMENT, OnCmdDeleteStarAssignment)
	ON_MESSAGE(WM_INPLACE_COMBO_KILLFOUCUS, OnMsgComboChange)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_STARASSIGNMENT, OnLvnItemchangedListStarassignment)
	ON_NOTIFY(NM_SETFOCUS, IDC_LIST_STARASSIGNMENT, OnNMSetfocusListStarassignment)
	ON_NOTIFY(NM_KILLFOCUS, IDC_LIST_STARASSIGNMENT, OnNMKillfocusListStarassignment)
	ON_NOTIFY(LVN_ENDLABELEDIT, IDC_LIST_STARASSIGNMENT, OnLvnEndlabeleditListContents)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
	ON_COMMAND(ID_ADD_TIMERANGE, OnAddTimeRange)
	ON_COMMAND(ID_DEL_TIMERANGE, OnDelTimeRange)
	ON_COMMAND(ID_EDIT_TIMERANGE, OnEditTimeRange)
	ON_WM_CONTEXTMENU()
END_MESSAGE_MAP()


// CStarAssignmentDlg message handlers

int CStarAssignmentDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CXTResizeDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!m_ToolBarTime.CreateEx(this, TBSTYLE_FLAT,WS_CHILD|WS_VISIBLE|CBRS_ALIGN_TOP)
		|| !m_ToolBarTime.LoadToolBar(IDR_TOOLBAR_ADD_DEL_EDIT))
	{
		return -1;
	}

	CToolBarCtrl& toolbarTime = m_ToolBarTime.GetToolBarCtrl();
	toolbarTime.SetCmdID(0, ID_NEW_TIME);
	toolbarTime.SetCmdID(1, ID_DEL_TIME);
	toolbarTime.SetCmdID(2, ID_EDIT_TIME);

	if (!m_ToolBarStarAssignment.CreateEx(this, TBSTYLE_FLAT,WS_CHILD|WS_VISIBLE|CBRS_ALIGN_TOP)
		|| !m_ToolBarStarAssignment.LoadToolBar(IDR_TOOLBAR_ADD_DEL))
	{
		return -1;
	}

	CToolBarCtrl& toolbarAssignment = m_ToolBarStarAssignment.GetToolBarCtrl();
	toolbarAssignment.SetCmdID(0, ID_NEW_STARTASSIGNMENT);
	toolbarAssignment.SetCmdID(1, ID_DEL_STARTASSIGNMENT);

	return 0;
}

BOOL CStarAssignmentDlg::OnInitDialog()
{
	CXTResizeDialog::OnInitDialog();
	
	InitToolBar();
	SetResize(IDC_TREE_LANDINGRUNWAY,SZ_TOP_LEFT,SZ_BOTTOM_LEFT);
	SetResize(m_ToolBarStarAssignment.GetDlgCtrlID(),SZ_TOP_LEFT,SZ_TOP_LEFT);
	SetResize(IDC_LIST_STARASSIGNMENT,SZ_TOP_LEFT,SZ_BOTTOM_RIGHT);
	SetResize(IDC_BUTTON_SAVE,SZ_BOTTOM_RIGHT,SZ_BOTTOM_RIGHT);
	SetResize(IDOK,SZ_BOTTOM_RIGHT,SZ_BOTTOM_RIGHT);
	SetResize(IDCANCEL,SZ_BOTTOM_RIGHT,SZ_BOTTOM_RIGHT);
	InitFlightTypeTree();
	InitListCtrl();

	//DeleteDataWhichRunwayIsNotExist();

	//maybe the data is modify,so save it
	//SaveData();

	GetDlgItem(IDC_SAVE)->EnableWindow(FALSE);

	return TRUE;
}

void CStarAssignmentDlg::InitToolBar(void)
{
	//Time toolbar
	CRect rect;
	m_StaticToolBarTime.GetWindowRect( &rect );
	ScreenToClient( &rect );

	m_ToolBarTime.MoveWindow( &rect );
	m_ToolBarTime.ShowWindow( SW_SHOW );
	m_StaticToolBarTime.ShowWindow( SW_HIDE );

	m_ToolBarTime.GetToolBarCtrl().EnableButton( ID_NEW_TIME, TRUE );
	m_ToolBarTime.GetToolBarCtrl().EnableButton( ID_EDIT_TIME, FALSE );
	m_ToolBarTime.GetToolBarCtrl().EnableButton( ID_DEL_TIME, FALSE );

	//Star Assignment
	m_StaticToolBarStarAssignment.GetWindowRect( &rect );
	ScreenToClient( &rect );

	m_ToolBarStarAssignment.MoveWindow( &rect );
	m_ToolBarStarAssignment.ShowWindow( SW_SHOW );
	m_StaticToolBarStarAssignment.ShowWindow( SW_HIDE );

	m_ToolBarStarAssignment.GetToolBarCtrl().EnableButton(ID_NEW_STARTASSIGNMENT, FALSE);
	m_ToolBarStarAssignment.GetToolBarCtrl().EnableButton(ID_DEL_STARTASSIGNMENT, FALSE);
}

void CStarAssignmentDlg::UpdateToolBar(void)
{
	switch(m_CurrentTreeItemCategory) 
	{
	case TREEITEMCATE_FLIGHTTYPE:
		{
			m_ToolBarTime.GetToolBarCtrl().EnableButton( ID_NEW_TIME, TRUE );
			m_ToolBarTime.GetToolBarCtrl().EnableButton( ID_EDIT_TIME, TRUE );
			m_ToolBarTime.GetToolBarCtrl().EnableButton( ID_DEL_TIME, TRUE );

			m_ToolBarStarAssignment.GetToolBarCtrl().EnableButton(ID_NEW_STARTASSIGNMENT, FALSE);
			m_ToolBarStarAssignment.GetToolBarCtrl().EnableButton(ID_DEL_STARTASSIGNMENT, FALSE);
		}
		break;

	case TREEITEMCATE_TIME:
		{
			m_ToolBarTime.GetToolBarCtrl().EnableButton( ID_NEW_TIME, FALSE );
			m_ToolBarTime.GetToolBarCtrl().EnableButton( ID_EDIT_TIME, TRUE );
			m_ToolBarTime.GetToolBarCtrl().EnableButton( ID_DEL_TIME, TRUE );

			m_ToolBarStarAssignment.GetToolBarCtrl().EnableButton(ID_NEW_STARTASSIGNMENT, TRUE);
			m_ToolBarStarAssignment.GetToolBarCtrl().EnableButton(ID_DEL_STARTASSIGNMENT, FALSE);
		}
		break;

	case TREEITEMCATE_NONE:
		{
			m_ToolBarTime.GetToolBarCtrl().EnableButton( ID_NEW_TIME, TRUE );
			m_ToolBarTime.GetToolBarCtrl().EnableButton( ID_EDIT_TIME, FALSE );
			m_ToolBarTime.GetToolBarCtrl().EnableButton( ID_DEL_TIME, FALSE );

			m_ToolBarStarAssignment.GetToolBarCtrl().EnableButton(ID_NEW_STARTASSIGNMENT, FALSE);
			m_ToolBarStarAssignment.GetToolBarCtrl().EnableButton(ID_DEL_STARTASSIGNMENT, FALSE);
		}
		break;
	}
}

void CStarAssignmentDlg::InitListCtrl(void)
{
	// set list control window style
	DWORD dwStyle = m_StarAssigmentCtrl.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_EX_GRIDLINES;
	m_StarAssigmentCtrl.SetExtendedStyle(dwStyle);

	CString strCaption;

	LV_COLUMNEX lvc;
	lvc.mask = LVCF_WIDTH|LVCF_TEXT;

	strCaption.LoadString(IDS_USESTAR);
	lvc.pszText = (LPSTR)(LPCTSTR)strCaption;
	lvc.cx = 180;
	lvc.fmt = LVCFMT_DROPDOWN;
	lvc.csList = &m_StarNameList;
	m_StarAssigmentCtrl.InsertColumn(0, &lvc);

	strCaption.LoadString(IDS_PERCENT);
	lvc.pszText = (LPSTR)(LPCTSTR)strCaption;
	lvc.cx = 100;
	lvc.fmt = LVCFMT_NUMBER;
	m_StarAssigmentCtrl.InsertColumn(1, &lvc);

}

void CStarAssignmentDlg::SetListContent(void)
{
	m_StarAssigmentCtrl.DeleteAllItems();

	//if (NULL == m_pStarAssignmentList)
	//{
	//	return;
	//}

	//size_t count = m_pCurFlightUseStartList->GetElementCount();
	//for (size_t i=0; i<count; i++)
	//{
	//	CFlightUseStar *pFlightUseStar = m_pCurFlightUseStartList->GetItem(i);

	//	CString strFlightType;
	//	pFlightUseStar->GetFlightType().screenPrint(strFlightType.GetBuffer(1024));
	//	m_StarAssigmentCtrl.InsertItem(i, strFlightType);
	//	m_StarAssigmentCtrl.SetItemData(i, (DWORD_PTR)pFlightUseStar);

	//	int nStarID = pFlightUseStar->GetStarID();
	//  CString strStarName;

	//	map<int, int>::const_iterator itor = m_AirRouteID.begin();
	//	for (int j=0; itor != m_AirRouteID.end(); itor++,j++)
	//	{
	//		if (nStarID == itor->second)
	//		{
	//			strStarName = m_StarNameList.GetAt(m_StarNameList.FindIndex(j));

	//			m_StarAssigmentCtrl.SetItemText(i, 1, strStarName);
	//		}
	//	}
	//}

	HTREEITEM hItem = m_wndTreeFltTime.GetSelectedItem();
	if(hItem == NULL)
		return;


	FlightStarAssignment* pItem = (FlightStarAssignment*)m_wndTreeFltTime.GetItemData(hItem);
	ASSERT(pItem != NULL);

	int nCount = pItem->GetSTARCount();
	if (nCount == 0)
		return;

	for(int i =0; i< nCount; i++)
	{
		StarPercentage* pStarItem = pItem->GetStarItem(i);

		int nStarID = pStarItem->GetStarID();
		CString strStarName;

		map<int, int>::const_iterator itor = m_AirRouteID.begin();
		for (int j=0; itor != m_AirRouteID.end(); itor++,j++)
		{
			if (nStarID == itor->second)
			{
				strStarName = m_StarNameList.GetAt(m_StarNameList.FindIndex(j));
				break;
			}
		}
		m_StarAssigmentCtrl.InsertItem(i,strStarName);

		CString strValue;
		strValue.Format("%d",pStarItem->GetPercent());
		m_StarAssigmentCtrl.SetItemText(i, 1, strValue);

		m_StarAssigmentCtrl.SetItemData(i, (DWORD_PTR)pStarItem);
	}
}

void CStarAssignmentDlg::InitFlightTypeTree(void)
{
	//HTREEITEM hTreeItem       = NULL;
	//HTREEITEM hChildTreeItem  = NULL;

	//std::vector<int> vAirportIds;
	//InputAirside::GetAirportList(m_nProjID, vAirportIds);
	//for (std::vector<int>::iterator iterAirportID = vAirportIds.begin(); iterAirportID != vAirportIds.end(); iterAirportID++)
	//{
	//	ALTAirport airport;
	//	airport.ReadAirport(*iterAirportID);

	//	std::vector<int> vRunways;
	//	ALTAirport::GetRunwaysIDs(*iterAirportID, vRunways);
	//	std::vector<int>::iterator iterRunwayID = vRunways.begin();
	//	for (; iterRunwayID != vRunways.end(); iterRunwayID++)
	//	{
	//		Runway runway;
	//		runway.ReadObject(*iterRunwayID);

	//		CString strRunway;
	//		strRunway.Format("%s", runway.GetObjNameString());
	//		hTreeItem = m_LandingRunwayCtrl.InsertItem(strRunway);
	//		m_LandingRunwayCtrl.SetItemData(hTreeItem, (DWORD_PTR)(*iterRunwayID));

	//		CString strRunwayMarking;
	//		strRunwayMarking.Format("%s", runway.GetMarking1().c_str());
	//		hChildTreeItem = m_LandingRunwayCtrl.InsertItem(strRunwayMarking, hTreeItem, TVI_LAST);
	//		m_LandingRunwayCtrl.SetItemData(hChildTreeItem, (DWORD_PTR)(0));

	//		LandingRunwayAddchildItem(hChildTreeItem, *iterRunwayID, 0);
	//		m_LandingRunwayCtrl.Expand(hTreeItem, TVE_EXPAND);

	//		strRunwayMarking.Format("%s", runway.GetMarking2().c_str());
	//		hChildTreeItem = m_LandingRunwayCtrl.InsertItem(strRunwayMarking, hTreeItem, TVI_LAST);
	//		m_LandingRunwayCtrl.SetItemData(hChildTreeItem, (DWORD_PTR)(1));

	//		LandingRunwayAddchildItem(hChildTreeItem, *iterRunwayID, 1);
	//		m_LandingRunwayCtrl.Expand(hTreeItem, TVE_EXPAND);
	//	}
	//}

	////Select first item
	//if (0 < m_LandingRunwayCtrl.GetCount())
	//{
	//	m_LandingRunwayCtrl.SelectItem(0);
	//}
	int nCount = m_pStarAssignmentList->GetSTARCount();
	CString strFilghtType = "";
	HTREEITEM hFltTypeTreeItem;
	for (int i=0; i<nCount; i++)
	{
		FlightStarAssignment* pItem =m_pStarAssignmentList->GetStarItem(i);
		m_wndTreeFltTime.SetRedraw(FALSE);

		char szBuffer[1024] = {0};
		pItem->GetFlightConstraint().screenPrint(szBuffer);

		CString strBuffer = szBuffer;
		if (strFilghtType != strBuffer)		//new item
		{
			strFilghtType = strBuffer;
			hFltTypeTreeItem = m_wndTreeFltTime.InsertItem(strFilghtType);

			FlightStarAssignment* pAssignment = new FlightStarAssignment;
			pAssignment->SetAirportDatabase(m_pAirportDB);
			pAssignment->setFlightType(pItem->GetFlightTypeName());
			m_wndTreeFltTime.SetItemData(hFltTypeTreeItem,(DWORD_PTR)pAssignment);
			m_vFlightTypeDelList.push_back(pAssignment);
		}

		CString strTimeRange = _T(""),strDay = _T("");

		ElapsedTime etInsert = pItem->GetStartTime();
		long lSecond = etInsert.asSeconds();
		strDay.Format(_T("Day%d %02d:%02d:%02d"),lSecond/86400 + 1,(lSecond % 86400)/3600,(lSecond % 86400 % 3600)/60,lSecond % 86400 % 3600 % 60);
		strTimeRange = strDay + _T(" - ");

		etInsert = pItem->GetEndTime();
		lSecond = etInsert.asSeconds();
		strDay.Format(_T("Day%d %02d:%02d:%02d"),lSecond/86400 + 1,(lSecond % 86400)/3600,(lSecond % 86400 % 3600)/60,lSecond % 86400 % 3600 % 60);
		strTimeRange += strDay;

		HTREEITEM hTimeRangeTreeItem = m_wndTreeFltTime.InsertItem(strTimeRange, hFltTypeTreeItem, TVI_LAST);
		m_wndTreeFltTime.SetItemData(hTimeRangeTreeItem, (DWORD_PTR)pItem);

		m_wndTreeFltTime.Expand(hFltTypeTreeItem, TVE_EXPAND);
		m_wndTreeFltTime.SetRedraw(TRUE);
	}
}

void CStarAssignmentDlg::OnBnClickedButtonSave()
{
	// TODO: Add your control notification handler code here
	SaveData();

	GetDlgItem(IDC_SAVE)->EnableWindow(FALSE);
}

void CStarAssignmentDlg::OnTvnSelchangedTreeFlightType(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	HTREEITEM hItem = m_wndTreeFltTime.GetSelectedItem();

	m_CurrentTreeItemCategory  = GetCurSeleltTreeItemState(hItem);

	switch(m_CurrentTreeItemCategory) 
	{
	case TREEITEMCATE_FLIGHTTYPE:
		{
			m_StarAssigmentCtrl.DeleteAllItems();
		}
		break;
	case TREEITEMCATE_TIME:
		{
			SetListContent();
		}
		break;

	default:
		break;
	}

    UpdateToolBar();

	*pResult = 0;
}

void CStarAssignmentDlg::OnCmdNew()
{

	//if ( m_CurrentTreeItemCategory == TREEITEMCATE_NONE)
	//{
		if (m_pSelectFlightType == NULL)
			return;

		FlightConstraint fltType = (*m_pSelectFlightType)(NULL);

		char szBuffer[1024] = {0};
		fltType.screenPrint(szBuffer);

		CString strBuffer = szBuffer;
		HTREEITEM hItem = m_wndTreeFltTime.GetRootItem();
		while (hItem)
		{
			if (m_wndTreeFltTime.GetItemText(hItem) == strBuffer)
			{
				MessageBox(_T("Flight Type: \" ") +strBuffer+ _T(" \" already exists."));
				return;
			}
			hItem = m_wndTreeFltTime.GetNextItem(hItem , TVGN_NEXT);
		}


		m_wndTreeFltTime.SetRedraw(FALSE);
		fltType.screenPrint(szBuffer);

		hItem = m_wndTreeFltTime.InsertItem(szBuffer);
		fltType.WriteSyntaxStringWithVersion(szBuffer);
		
		FlightStarAssignment* pAssignment = new FlightStarAssignment;
		pAssignment->SetAirportDatabase(m_pAirportDB);
		pAssignment->setFlightType(szBuffer);
		m_wndTreeFltTime.SetItemData(hItem,(DWORD_PTR)pAssignment);
		m_vFlightTypeDelList.push_back(pAssignment);
		m_wndTreeFltTime.SetRedraw(TRUE);
	//}


	//ElapsedTime estFromTime;
	//ElapsedTime estToTime;

	//CDlgTimeRange dlg(estFromTime, estToTime);

	//if(IDOK!= dlg.DoModal())
	//	return;

	//CString strTimeRange;
	//strTimeRange.Format("%s - %s", dlg.GetStartTimeString(), dlg.GetEndTimeString());

	//estFromTime = dlg.GetStartTime();
	//estToTime = dlg.GetEndTime();

	//CFlightUseStarList *pFlightUseStarLst = new CFlightUseStarList();
	//pFlightUseStarLst->SetFromTime(estFromTime);
	//pFlightUseStarLst->SetToTime(estToTime);

	//HTREEITEM hCurItem = m_LandingRunwayCtrl.GetSelectedItem();
	//HTREEITEM hParentItem = m_LandingRunwayCtrl.GetParentItem(hCurItem);
	//int nRunwayID;
	//nRunwayID = (int)m_LandingRunwayCtrl.GetItemData(hParentItem);	

	//BOOL bIsConflict = FALSE;
	//bIsConflict = m_pLandingRunwayStarAssignmentList->IsConflictTime(nRunwayID, pFlightUseStarLst);

	////if the time is exist
	//if (bIsConflict)
	//{
	//	delete pFlightUseStarLst;

	//	MessageBox(_T("Time Range: \" ") +strTimeRange+ _T(" \" conflict."));

	//	return;
	//}

	//if (NULL == m_pCurLandingRunwayStarAssignment)
	//{
	//	CLandingRunwayStarAssignment *pLandingRunwayStarAssignment = new CLandingRunwayStarAssignment();

	//	HTREEITEM hCurItem = m_LandingRunwayCtrl.GetSelectedItem();
	//	int nMarkingID;
	//	nMarkingID = (int)m_LandingRunwayCtrl.GetItemData(hCurItem);

	//	HTREEITEM hParentItem = m_LandingRunwayCtrl.GetParentItem(hCurItem);
	//	int nRunwayID;
	//	nRunwayID = (int)m_LandingRunwayCtrl.GetItemData(hParentItem);		

	//	pLandingRunwayStarAssignment->SetLandingRunwayID(nRunwayID);
	//	pLandingRunwayStarAssignment->SetRunwayMarkingIndex(nMarkingID);

	//	pLandingRunwayStarAssignment->AddNewItem(pFlightUseStarLst);
	//	m_pLandingRunwayStarAssignmentList->AddNewItem(pLandingRunwayStarAssignment);
	//	m_pLandingRunwayStarAssignmentList->GetItemByRunwayIDAndMarking(nRunwayID, nMarkingID, &m_pCurLandingRunwayStarAssignment);
	//}
	//else
	//{
	//	m_pCurLandingRunwayStarAssignment->AddNewItem(pFlightUseStarLst);
	//}

	//HTREEITEM hTreeItem = m_LandingRunwayCtrl.GetSelectedItem();
	//HTREEITEM hChildTreeItem = m_LandingRunwayCtrl.InsertItem(strTimeRange, hTreeItem, TVI_LAST);
	//m_LandingRunwayCtrl.SetItemData(hChildTreeItem, (DWORD_PTR)(pFlightUseStarLst));
	//m_LandingRunwayCtrl.Expand(hTreeItem, TVE_EXPAND);

	GetDlgItem(IDC_SAVE)->EnableWindow(TRUE);
}

void CStarAssignmentDlg::OnCmdDelete()
{
	HTREEITEM hSelItem = m_wndTreeFltTime.GetSelectedItem();
	m_CurrentTreeItemCategory = GetCurSeleltTreeItemState(hSelItem);
	if (m_CurrentTreeItemCategory == TREEITEMCATE_NONE)
		return;
	m_wndTreeFltTime.SetRedraw(FALSE);
	if ( m_CurrentTreeItemCategory == TREEITEMCATE_FLIGHTTYPE)
	{
		FlightStarAssignment* pAssignment = (FlightStarAssignment*)m_wndTreeFltTime.GetItemData(hSelItem);
		CString strFlightType = pAssignment->GetFlightTypeName();

		int nCount = m_pStarAssignmentList->GetSTARCount();
		for (int  i = 0; i < nCount ; i++)
		{
			FlightStarAssignment* pItem = m_pStarAssignmentList->GetStarItem(i);
			if (pItem == NULL)
				continue;
			if (pItem->GetFlightTypeName() == strFlightType)
			{
				m_pStarAssignmentList->DeleteStarItem(pItem);
			}
		}
		m_wndTreeFltTime.DeleteItem(hSelItem);
	}
	if (m_CurrentTreeItemCategory == TREEITEMCATE_TIME)
	{
		OnDelTimeRange();
	}
	//if (NULL == m_pCurLandingRunwayStarAssignment)
	//{
	//	return;
	//}

	//HTREEITEM hTreeItem = m_LandingRunwayCtrl.GetSelectedItem();

	//CFlightUseStarList *pFlightUseStarLst = (CFlightUseStarList*)m_LandingRunwayCtrl.GetItemData(hTreeItem);

	//m_pCurLandingRunwayStarAssignment->DeleteItemByTime(pFlightUseStarLst->GetFromTime(), pFlightUseStarLst->GetToTime());

	//m_LandingRunwayCtrl.DeleteItem(hTreeItem);
	m_wndTreeFltTime.SetRedraw(TRUE);

	GetDlgItem(IDC_SAVE)->EnableWindow(TRUE);

}


void CStarAssignmentDlg::OnCmdEdit()
{
	//if (NULL == m_pCurLandingRunwayStarAssignment)
	//{
	//	return;
	//}

	//HTREEITEM hTreeItem = m_LandingRunwayCtrl.GetSelectedItem();

	//CFlightUseStarList *pFlightUseStarLst = (CFlightUseStarList*)m_LandingRunwayCtrl.GetItemData(hTreeItem);

	//if (NULL == pFlightUseStarLst)
	//{
	//	return;
	//}

	//ElapsedTime estFromTime = pFlightUseStarLst->GetFromTime();
	//ElapsedTime estToTime = pFlightUseStarLst->GetToTime();

	//CDlgTimeRange dlg(estFromTime, estToTime);

	//if(IDOK!= dlg.DoModal())
	//{
	//	return;
	//}

	//HTREEITEM hParentTree = m_LandingRunwayCtrl.GetParentItem(hTreeItem);
	//HTREEITEM hRunwayItem = m_LandingRunwayCtrl.GetParentItem(hParentTree);
	//int nRunwayID = (int)m_LandingRunwayCtrl.GetItemData(hRunwayItem);

	//pFlightUseStarLst->SetFromTime(dlg.GetStartTime());
	//pFlightUseStarLst->SetToTime(dlg.GetEndTime());

	//BOOL bIsConflict = FALSE;
	//bIsConflict = m_pLandingRunwayStarAssignmentList->IsConflictTime(nRunwayID, pFlightUseStarLst);

	//CString strTimeRange;
	//strTimeRange.Format("%s - %s", dlg.GetStartTimeString(), dlg.GetEndTimeString());

	////the new time is exist
	//if (bIsConflict)
	//{
	//	//resume
	//	pFlightUseStarLst->SetFromTime(estFromTime);
	//	pFlightUseStarLst->SetToTime(estToTime);

	//	MessageBox(_T("Time Range: \" ") +strTimeRange+ _T(" \" conflict."));

	//	return;
	//}

	//m_LandingRunwayCtrl.SetItemText(hTreeItem, strTimeRange);

	HTREEITEM hSelItem = m_wndTreeFltTime.GetSelectedItem();
	m_CurrentTreeItemCategory = GetCurSeleltTreeItemState(hSelItem);
	if (m_CurrentTreeItemCategory == TREEITEMCATE_NONE)
		return;

	if ( m_CurrentTreeItemCategory == TREEITEMCATE_FLIGHTTYPE)
	{
		FlightStarAssignment* pAssignment = (FlightStarAssignment*)m_wndTreeFltTime.GetItemData(hSelItem);
		CString strFlightType = pAssignment->GetFlightTypeName();

		FlightConstraint fltType = (*m_pSelectFlightType)(NULL);

		char szBuffer[1024] = {0};
		fltType.screenPrint(szBuffer);

		CString strBuffer = szBuffer;
		HTREEITEM hItem = m_wndTreeFltTime.GetRootItem();

		while (hItem)
		{
			if (hSelItem != hItem && m_wndTreeFltTime.GetItemText(hItem) == strBuffer)
			{
				MessageBox(_T("Flight: \" ") +strBuffer+ _T(" \" already exists."));
				return;
			}
			hItem = m_wndTreeFltTime.GetNextItem(hItem , TVGN_NEXT);
		}
		m_wndTreeFltTime.SetItemText(hSelItem, szBuffer);

		fltType.WriteSyntaxStringWithVersion(szBuffer);
		pAssignment->setFlightType(szBuffer);

		int nCount = m_pStarAssignmentList->GetSTARCount();
		for (int  i = 0; i < nCount ; i++)
		{
			FlightStarAssignment* pItem = m_pStarAssignmentList->GetStarItem(i);
			if (pItem == NULL)
				continue;
			if (pItem->GetFlightTypeName() == strFlightType)
			{
				pItem->setFlightType(szBuffer);
			}
		}
	}
	if (m_CurrentTreeItemCategory == TREEITEMCATE_TIME)
	{
		OnEditTimeRange();
	}

	GetDlgItem(IDC_SAVE)->EnableWindow(TRUE);
}

void CStarAssignmentDlg::OnAddTimeRange()
{
	HTREEITEM hSelItem = m_wndTreeFltTime.GetSelectedItem();
	m_CurrentTreeItemCategory = GetCurSeleltTreeItemState(hSelItem);
	if (m_CurrentTreeItemCategory == TREEITEMCATE_FLIGHTTYPE)
	{
		ElapsedTime estFromTime;
		ElapsedTime estToTime;

		CDlgTimeRange dlg(estFromTime, estToTime);

		if(IDOK!= dlg.DoModal())
			return;

		CString strTimeRange;
		strTimeRange.Format("%s - %s", dlg.GetStartTimeString(), dlg.GetEndTimeString());

		estFromTime = dlg.GetStartTime();
		estToTime = dlg.GetEndTime();
		if (!IsvalidTimeRange(estFromTime,estToTime))
		{
			MessageBox(_T("Time Range: \" ") +strTimeRange+ _T(" \" conflict."));
			return;
		}

		FlightStarAssignment *pItem = new FlightStarAssignment();
		pItem->SetStartTime(estFromTime);
		pItem->SetEndTime(estToTime);

		FlightStarAssignment* pAssignment = (FlightStarAssignment*)m_wndTreeFltTime.GetItemData(hSelItem);
		CString strFlightType = pAssignment->GetFlightTypeName();

		pItem->SetAirportDatabase(m_pAirportDB);
		pItem->setFlightType(strFlightType);
		m_pStarAssignmentList->AddStarItem(pItem);
		HTREEITEM hChildTreeItem = m_wndTreeFltTime.InsertItem(strTimeRange, hSelItem, TVI_LAST);
		m_wndTreeFltTime.SetItemData(hChildTreeItem, (DWORD_PTR)(pItem));
		m_wndTreeFltTime.Expand(hSelItem, TVE_EXPAND);
		GetDlgItem(IDC_SAVE)->EnableWindow(TRUE);

	}

}

void CStarAssignmentDlg::OnEditTimeRange()
{
	HTREEITEM hSelItem = m_wndTreeFltTime.GetSelectedItem();
	m_CurrentTreeItemCategory = GetCurSeleltTreeItemState(hSelItem);
	if (m_CurrentTreeItemCategory == TREEITEMCATE_TIME)
	{
		FlightStarAssignment* pItem = (FlightStarAssignment*)m_wndTreeFltTime.GetItemData(hSelItem);
		CDlgTimeRange dlg(pItem->GetStartTime(), pItem->GetEndTime());
		if(IDOK!= dlg.DoModal())
			return;

		ElapsedTime tStartTime = dlg.GetStartTime();
		ElapsedTime tEndTime = dlg.GetEndTime();

		CString strTimeRange;
		strTimeRange.Format("%s - %s", dlg.GetStartTimeString(), dlg.GetEndTimeString());

		if (!IsvalidTimeRange(tStartTime,tEndTime))
		{
			MessageBox(_T("TimeRange: \" ") +strTimeRange+ _T(" \" invalid."));
			return;
		}
		pItem->SetStartTime(tStartTime);
		pItem->SetEndTime(tEndTime);
		m_wndTreeFltTime.SetItemText(hSelItem, strTimeRange);
		GetDlgItem(IDC_SAVE)->EnableWindow(TRUE);

	}
}

void CStarAssignmentDlg::OnDelTimeRange()
{
	HTREEITEM hSelItem = m_wndTreeFltTime.GetSelectedItem();
	m_CurrentTreeItemCategory = GetCurSeleltTreeItemState(hSelItem);

	if (m_CurrentTreeItemCategory == TREEITEMCATE_TIME)
	{
		FlightStarAssignment* pItem = (FlightStarAssignment*)m_wndTreeFltTime.GetItemData(hSelItem);
		if(pItem == NULL)
			return;
		m_pStarAssignmentList->DeleteStarItem(pItem);
		m_wndTreeFltTime.DeleteItem(hSelItem);
		GetDlgItem(IDC_SAVE)->EnableWindow(TRUE);

	}
}

void CStarAssignmentDlg::OnCmdNewStarAssignment()
{
	//if (m_pSelectFlightType == NULL)
	//	return;

	//FlightConstraint fltType = (*m_pSelectFlightType)(NULL);

	//char szBuffer[1024] = {0};
	//fltType.screenPrint(szBuffer);

	////If the Flight Type is exist
	//int nCount = m_StarAssigmentCtrl.GetItemCount();
	//for (int i=0; i<nCount; i++)
	//{
	//	if (!m_StarAssigmentCtrl.GetItemText(i, 0).Compare(szBuffer))
	//	{
	//		MessageBox(_T("The Flight Type already exists!"));

	//		return;
	//	}
	//}
	HTREEITEM hSelItem = m_wndTreeFltTime.GetSelectedItem();
	m_CurrentTreeItemCategory = GetCurSeleltTreeItemState(hSelItem);
	if (m_CurrentTreeItemCategory != TREEITEMCATE_TIME)
		return;

	if (m_AirRouteID.size() < 1)
	{
		MessageBox(_T("No Star Exist!."));
		return;
	}

	FlightStarAssignment* pItem = (FlightStarAssignment*)m_wndTreeFltTime.GetItemData(hSelItem);
	int nPercent = 100;
	int nCount = pItem->GetSTARCount();
	for(int i = 0; i < nCount; i++)
	{
		StarPercentage* pStarData = pItem->GetStarItem(i); 
		if (pStarData == NULL)
			continue;
		nPercent -= pStarData->GetPercent();
	}
	if (nPercent <= 0)
	{
		MessageBox("The sum of star operating percent exceed 100%!");
		return;
	}

	StarPercentage *pData = new StarPercentage;

	map<int, int>::const_iterator iter = m_AirRouteID.begin();
	int nStarID = iter->second;
	pData->SetStarID(nStarID);
	pData->SetPercent(nPercent);
	
	pItem->AddStarItem(pData);	

	int nItem = m_StarAssigmentCtrl.InsertItem(m_StarAssigmentCtrl.GetItemCount(), m_StarNameList.GetHead());
	CString strPercent;
	strPercent.Format("%d", nPercent);
	m_StarAssigmentCtrl.SetItemText(nItem, 1, strPercent);
	m_StarAssigmentCtrl.SetItemData(nItem, (DWORD_PTR)pData);
	GetDlgItem(IDC_SAVE)->EnableWindow(TRUE);
}

void CStarAssignmentDlg::OnCmdDeleteStarAssignment()
{
	HTREEITEM hSelItem = m_wndTreeFltTime.GetSelectedItem();

	m_CurrentTreeItemCategory = GetCurSeleltTreeItemState(hSelItem);
	if (m_CurrentTreeItemCategory != TREEITEMCATE_TIME)
		return;

	StarPercentage* pData = GetCurFlightUseStar();
	if (pData == NULL)
		return;

	FlightStarAssignment* pItem = (FlightStarAssignment*)m_wndTreeFltTime.GetItemData(hSelItem);
	m_StarAssigmentCtrl.DeleteItem(m_StarAssigmentCtrl.GetCurSel());
	pItem->DeleteStarItem(pData);

	GetDlgItem(IDC_SAVE)->EnableWindow(TRUE);
}



LRESULT CStarAssignmentDlg::OnMsgComboChange(WPARAM wParam, LPARAM lParam)
{
	int nComboxSel = (int)wParam;
	if (nComboxSel == LB_ERR)
		return 0;

	StarPercentage* pData = GetCurFlightUseStar();
	if (NULL == pData)
	{
		return NULL;
	}

	map<int, int>::const_iterator iter = m_AirRouteID.find(nComboxSel);
	int nStarID = iter->second;
	pData->SetStarID(nStarID);

	//CString strValue = m_StarAssigmentCtrl.GetItemText(m_StarAssigmentCtrl.GetCurSel(), 1);
	//pData->SetPercent((int)(atoi(strValue)));

	GetDlgItem(IDC_SAVE)->EnableWindow(TRUE);

	return NULL;
}

void CStarAssignmentDlg::OnLvnItemchangedListStarassignment(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
		//GetCurFlightUseStar();

	if (0 < m_StarAssigmentCtrl.GetSelectedCount())
	{
		m_ToolBarStarAssignment.GetToolBarCtrl().EnableButton(ID_DEL_STARTASSIGNMENT, TRUE);
	}
	else
	{
		m_ToolBarStarAssignment.GetToolBarCtrl().EnableButton(ID_DEL_STARTASSIGNMENT, FALSE);
	}

	*pResult = 0;
}

void CStarAssignmentDlg::OnNMSetfocusListStarassignment(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here
	//GetCurFlightUseStar();

	if (0 < m_StarAssigmentCtrl.GetSelectedCount())
	{
		m_ToolBarStarAssignment.GetToolBarCtrl().EnableButton(ID_DEL_STARTASSIGNMENT, TRUE);
	}
	else
	{
		m_ToolBarStarAssignment.GetToolBarCtrl().EnableButton(ID_DEL_STARTASSIGNMENT, FALSE);
	}

	*pResult = 0;
}

void CStarAssignmentDlg::OnNMKillfocusListStarassignment(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here
	m_ToolBarStarAssignment.GetToolBarCtrl().EnableButton(ID_DEL_STARTASSIGNMENT, FALSE);

	*pResult = 0;
}

StarPercentage* CStarAssignmentDlg::GetCurFlightUseStar(void)
{
	int nSelectCount = m_StarAssigmentCtrl.GetSelectedCount();

	if (nSelectCount < 1)
	{
		return NULL;
	}

	int nSelectItem = m_StarAssigmentCtrl.GetCurSel();

	return (StarPercentage*)m_StarAssigmentCtrl.GetItemData(nSelectItem);

}

void CStarAssignmentDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	SaveData();

	OnOK();
}

void CStarAssignmentDlg::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	OnCancel();
}

TREEITEMCATE CStarAssignmentDlg::GetCurSeleltTreeItemState(HTREEITEM hTreeItem)
{
	if (NULL == hTreeItem)
	{
		return TREEITEMCATE_NONE;
	}

	HTREEITEM hParent = m_wndTreeFltTime.GetParentItem(hTreeItem);

	if (NULL == hParent)
	{
		return TREEITEMCATE_FLIGHTTYPE;
	}

	HTREEITEM hParentParent = m_wndTreeFltTime.GetParentItem(hParent);

	if (NULL == hParentParent)
	{
		return TREEITEMCATE_TIME;
	}

	return TREEITEMCATE_NONE;
}

//void CStarAssignmentDlg::DeleteDataWhichStarIsNotExist()
//{
//	int nCount = m_pLandingRunwayStarAssignmentList->GetElementCount();
//
//	for (int i=0; i<nCount; i++)
//	{
//		CLandingRunwayStarAssignment *pLRStarAssignment = m_pLandingRunwayStarAssignmentList->GetItem(i);
//
//		int nLRSACount = pLRStarAssignment->GetElementCount();
//
//		for (int j=0; j<nLRSACount; j++)
//		{
//			CFlightUseStarList *pFlightUseStarList = pLRStarAssignment->GetItem(j);
//
//			int nFUSListCount = pFlightUseStarList->GetElementCount();
//
//			for (int k=0; k<nFUSListCount; k++)
//			{
//				CFlightUseStar *pFlightUseStar = pFlightUseStarList->GetItem(k);
//
//				int nStarID = pFlightUseStar->GetStarID();
//
//				map<int, int>::const_iterator iter = m_AirRouteID.begin();
//
//				BOOL bStarIDExist = FALSE;
//				while (iter != m_AirRouteID.end())
//				{
//					if (nStarID == iter->second)
//					{
//						bStarIDExist = TRUE;
//					}
//					iter++;
//				}
//
//				//not exist
//				if (!bStarIDExist)
//				{
//					pFlightUseStarList->DeleteItemFromIndex(k);
//
//					k--;
//					nFUSListCount--;
//				}
//			}
//
//			if (0 == nFUSListCount)
//			{
//				pLRStarAssignment->DeleteItemFromIndex(j);
//				j--;
//				nLRSACount--;
//			}
//		}
//	}
//}

//void CStarAssignmentDlg::DeleteDataWhichRunwayIsNotExist()
//{
//	std::vector<int> vAirportIds;
//	InputAirside::GetAirportList(m_nProjID, vAirportIds);
//
//	int nCount = m_pLandingRunwayStarAssignmentList->GetElementCount();
//
//	for (int i=0; i<nCount; i++)
//	{
//		CLandingRunwayStarAssignment *pLRStarAssignment = m_pLandingRunwayStarAssignmentList->GetItem(i);
//		int nRunwayID = pLRStarAssignment->GetLandingRunwayID();
//
//		BOOL bIsExist = FALSE;
//
//		for (std::vector<int>::iterator iterAirportID = vAirportIds.begin(); iterAirportID != vAirportIds.end(); iterAirportID++)
//		{
//			ALTAirport airport;
//			airport.ReadAirport(*iterAirportID);
//
//			std::vector<int> vRunways;
//			ALTAirport::GetRunwaysIDs(*iterAirportID, vRunways);
//			std::vector<int>::iterator iterRunwayID = vRunways.begin();
//			for (; iterRunwayID != vRunways.end(); iterRunwayID++)
//			{
//				if (nRunwayID == *iterRunwayID)
//				{
//					bIsExist = TRUE;
//				}				
//			}
//		}	
//
//		//not exist
//		if (!bIsExist)
//		{
//			m_pLandingRunwayStarAssignmentList->DeleteItemByRunwayID(nRunwayID);
//
//			i--;
//			nCount--;
//		}
//	}	
//}
bool CStarAssignmentDlg::IsvalidTimeRange(ElapsedTime tStart, ElapsedTime tEnd)
{

	return true;
}
void CStarAssignmentDlg::SaveData()
{
	try
	{
		CADODatabase::BeginTransaction() ;
		m_pStarAssignmentList->SaveData();
		CADODatabase::CommitTransaction() ;

	}
	catch (CADOException &e)
	{
		CADODatabase::RollBackTransation() ;
		e.ErrorMessage();
	}
}

void CStarAssignmentDlg::OnContextMenu(CWnd* pWnd, CPoint point)
{
	CRect  rtFltTypeTree;
	m_wndTreeFltTime.GetWindowRect(&rtFltTypeTree);
	if( !rtFltTypeTree.PtInRect(point))
		return;

	m_wndTreeFltTime.SetFocus();
	CPoint pt = point;
	m_wndTreeFltTime.ScreenToClient( &pt );
	UINT iRet;
	HTREEITEM hRClickItem = m_wndTreeFltTime.HitTest( pt, &iRet );
	if (iRet != TVHT_ONITEMLABEL)   // Must click on the label
		hRClickItem = NULL;
	if (hRClickItem==NULL)
		return;

	m_wndTreeFltTime.SelectItem(hRClickItem);

	CMenu menuPopup;
	menuPopup.LoadMenu( IDR_CTX_MENU_STRATEGY_TIMERANGE );
	CMenu* pMenu=NULL;
	pMenu = menuPopup.GetSubMenu(0);

	if( GetCurSeleltTreeItemState(hRClickItem) != TREEITEMCATE_FLIGHTTYPE)
	{
		pMenu->EnableMenuItem( ID_ADD_TIMERANGE, MF_GRAYED );
	}
	if ( GetCurSeleltTreeItemState(hRClickItem) != TREEITEMCATE_TIME)
	{
		pMenu->EnableMenuItem( ID_EDIT_TIMERANGE, MF_GRAYED );
		pMenu->EnableMenuItem( ID_DEL_TIMERANGE, MF_GRAYED );
	}


	if (pMenu)
		pMenu->TrackPopupMenu(TPM_LEFTALIGN, point.x, point.y, this);
}

void CStarAssignmentDlg::OnLvnEndlabeleditListContents(NMHDR *pNMHDR, LRESULT *pResult)
{
	int iTotals = 0 , nCount = 0;
	LV_DISPINFO* plvDispInfo = (LV_DISPINFO *)pNMHDR;
	LV_ITEM* plvItem = &plvDispInfo->item;
	int nItem = plvItem->iItem;
	int nSubItem = plvItem->iSubItem;
	if (nItem < 0|| nSubItem < 1)
		return;

	nCount = m_StarAssigmentCtrl.GetItemCount();
	for (int i = 0; i < nCount ; i++)
	{
		if(plvItem->iItem != i)
			iTotals += _ttol(m_StarAssigmentCtrl.GetItemText(i,1) );
	}
	if (iTotals + _ttol(m_StarAssigmentCtrl.GetItemText(nItem,nSubItem) ) > 100)
	{
		CString strReplace = _T("0");
		strReplace.Format(_T("%d"),100 - iTotals);
		m_StarAssigmentCtrl.SetItemText(nItem,nSubItem,strReplace);
		MessageBox(_T("The sum of operation percent can not exceed 100%."));
		return;
	}

	m_StarAssigmentCtrl.OnEndlabeledit(pNMHDR, pResult);	

	CString strValue = m_StarAssigmentCtrl.GetItemText(nItem, nSubItem);
	StarPercentage* pItem = (StarPercentage*)m_StarAssigmentCtrl.GetItemData(nItem);
	if (nSubItem == 1)
	{
		pItem->SetPercent((int)(atoi(strValue)));
	}


	*pResult = 0;
}
