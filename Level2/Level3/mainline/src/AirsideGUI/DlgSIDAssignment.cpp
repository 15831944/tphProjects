// DlgSIDAssignment.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "DlgSIDAssignment.h"
//#include "InputAirside/FlightUseSID.h"
//#include "InputAirside/FlightUseSIDList.h"
//#include "InputAirside/LandingRunwaySIDAssignment.h"
//#include "InputAirside/LandingRunwaySIDAssignmentList.h"
#include "InputAirside/InputAirside.h"
#include "InputAirside/ALTAirport.h"
#include "InputAirside/Runway.h"
#include "InputAirside/AirRoute.h"
#include "../Common/WinMsg.h"
#include "DlgTimeRange.h"
#include "StarAssignmentDlg.h"
#include "../InputAirside/FlightSIDAssignment.h"

namespace
{
	const UINT ID_NEW_TIME      = 10;
	const UINT ID_EDIT_TIME     = 11;
	const UINT ID_DEL_TIME      = 12;

	const UINT ID_NEW_SIDASSIGNMENT  = 20;
	const UINT ID_DEL_SIDASSIGNMENT  = 21;
}

// CDlgSIDAssignment dialog
IMPLEMENT_DYNAMIC(CDlgSIDAssignment, CDialog)
CDlgSIDAssignment::CDlgSIDAssignment(int nProjID, PSelectFlightType pSelectFlightType, CAirportDatabase *pAirPortdb, CWnd* pParent /* = NULL */)
: CXTResizeDialog(CDlgSIDAssignment::IDD, pParent)
, m_pSelectFlightType(pSelectFlightType)
, m_nProjID(nProjID)
, m_CurrentTreeItemCategory(TREEITEMCATE_NONE)
, m_pAirportDB(pAirPortdb)
//, m_pCurLandingRunwaySIDAssignment(NULL)
//, m_pCurFlightUseSIDList(NULL)
//, m_pCurFlightUseSID(NULL)
{
	//m_pSIDAssignmentList = new CLandingRunwaySIDAssignmentList();
	//m_pSIDAssignmentList->SetAirportDatabase(pAirPortdb);
	//m_pSIDAssignmentList->ReadData(m_nProjID);
	m_pSIDAssignmentList = new FlightSIDAssignmentList;
	m_pSIDAssignmentList->SetAirportDatabase(pAirPortdb);
	m_pSIDAssignmentList->ReadData(m_nProjID);

	//read air routelist
	AirRouteList airRouteList;
	airRouteList.ReadData(m_nProjID);

	int nAirRouteCount = airRouteList.GetAirRouteCount();

	for (int i =0,j=0; i < nAirRouteCount; i++)
	{
		CAirRoute *pAirRoute = airRouteList.GetAirRoute(i);

		ASSERT(pAirRoute != NULL);

		if (CAirRoute::SID == pAirRoute->getRouteType())
		{
			m_SIDNameList.AddTail(pAirRoute->getName());
			m_AirRouteID.insert(Int_Pair(j, pAirRoute->getID()));
			j++;
		}
	}

}

CDlgSIDAssignment::~CDlgSIDAssignment()
{
	delete m_pSIDAssignmentList;
	m_pSIDAssignmentList = NULL;

	int nCount = (int)m_vFlightTypeDelList.size();
	for (int i = 0; i < nCount; i++)
	{
		FlightSIDAssignment* pItem = m_vFlightTypeDelList.at(i);
		if (pItem != NULL)
		{
			delete pItem;
			pItem = NULL;
		}

	}
	m_vFlightTypeDelList.clear();
}

void CDlgSIDAssignment::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_TIME, m_StaticToolBarTime);
	DDX_Control(pDX, IDC_STATIC_FILGHTTYPE, m_StaticToolBarSIDAssignment);
	DDX_Control(pDX, IDC_TREE_LANDINGRUNWAY, m_wndTreeFltTime);
	DDX_Control(pDX, IDC_LIST_SIDASSIFNMENT, m_SIDAssigmentCtrl);
}


BEGIN_MESSAGE_MAP(CDlgSIDAssignment, CDialog)
	ON_WM_CREATE()
	ON_BN_CLICKED(IDC_BUTTON_SAVE, OnBnClickedButtonSave)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_LANDINGRUNWAY, OnTvnSelchangedTreeFlightType)
	ON_COMMAND(ID_NEW_TIME, OnCmdNew)
	ON_COMMAND(ID_DEL_TIME, OnCmdDelete)
	ON_COMMAND(ID_EDIT_TIME, OnCmdEdit)
	ON_COMMAND(ID_NEW_SIDASSIGNMENT, OnCmdNewSIDAssignment)
	ON_COMMAND(ID_DEL_SIDASSIGNMENT, OnCmdDeleteSIDAssignment)
	ON_MESSAGE(WM_INPLACE_COMBO_KILLFOUCUS, OnMsgComboChange)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_SIDASSIFNMENT, OnLvnItemchangedListSIDAssignment)
	ON_NOTIFY(NM_SETFOCUS, IDC_LIST_SIDASSIFNMENT, OnNMSetfocusListSIDAssignment)
	ON_NOTIFY(NM_KILLFOCUS, IDC_LIST_SIDASSIFNMENT, OnNMKillfocusListSIDAssignment)
	ON_NOTIFY(LVN_ENDLABELEDIT, IDC_LIST_SIDASSIFNMENT, OnLvnEndlabeleditListContents)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
	ON_COMMAND(ID_ADD_TIMERANGE, OnAddTimeRange)
	ON_COMMAND(ID_DEL_TIMERANGE, OnDelTimeRange)
	ON_COMMAND(ID_EDIT_TIMERANGE, OnEditTimeRange)
	ON_WM_CONTEXTMENU()
END_MESSAGE_MAP()


// CDlgSIDAssignment message handlers
int CDlgSIDAssignment::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
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

	if (!m_ToolBarSIDAssignment.CreateEx(this, TBSTYLE_FLAT,WS_CHILD|WS_VISIBLE|CBRS_ALIGN_TOP)
		|| !m_ToolBarSIDAssignment.LoadToolBar(IDR_TOOLBAR_ADD_DEL))
	{
		return -1;
	}

	CToolBarCtrl& toolbarAssignment = m_ToolBarSIDAssignment.GetToolBarCtrl();
	toolbarAssignment.SetCmdID(0, ID_NEW_SIDASSIGNMENT);
	toolbarAssignment.SetCmdID(1, ID_DEL_SIDASSIGNMENT);

	return 0;
}


BOOL CDlgSIDAssignment::OnInitDialog()
{
	CDialog::OnInitDialog();

	InitToolBar();
	SetResize(IDC_TREE_LANDINGRUNWAY,SZ_TOP_LEFT,SZ_BOTTOM_LEFT);
	SetResize(m_ToolBarSIDAssignment.GetDlgCtrlID(),SZ_TOP_LEFT,SZ_TOP_LEFT);
	SetResize(IDC_LIST_SIDASSIFNMENT,SZ_TOP_LEFT,SZ_BOTTOM_RIGHT);
	SetResize(IDC_BUTTON_SAVE,SZ_BOTTOM_RIGHT,SZ_BOTTOM_RIGHT);
	SetResize(IDOK,SZ_BOTTOM_RIGHT,SZ_BOTTOM_RIGHT);
	SetResize(IDCANCEL,SZ_BOTTOM_RIGHT,SZ_BOTTOM_RIGHT);
	InitFlightTypeTree();
	InitListCtrl();

	//maybe the data is modify,so save it
	//SaveData();

	GetDlgItem(IDC_SAVE)->EnableWindow(FALSE);

	return TRUE;
}


void CDlgSIDAssignment::InitToolBar()
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

	//SID Assignment
	m_StaticToolBarSIDAssignment.GetWindowRect( &rect );
	ScreenToClient( &rect );

	m_ToolBarSIDAssignment.MoveWindow( &rect );
	m_ToolBarSIDAssignment.ShowWindow( SW_SHOW );
	m_StaticToolBarSIDAssignment.ShowWindow( SW_HIDE );

	m_ToolBarSIDAssignment.GetToolBarCtrl().EnableButton(ID_NEW_SIDASSIGNMENT, FALSE);
	m_ToolBarSIDAssignment.GetToolBarCtrl().EnableButton(ID_DEL_SIDASSIGNMENT, FALSE);
}

void CDlgSIDAssignment::UpdateToolBar(void)
{
	switch(m_CurrentTreeItemCategory) 
	{
	case TREEITEMCATE_FLIGHTTYPE:
		{
			m_ToolBarTime.GetToolBarCtrl().EnableButton( ID_NEW_TIME, TRUE );
			m_ToolBarTime.GetToolBarCtrl().EnableButton( ID_EDIT_TIME, TRUE );
			m_ToolBarTime.GetToolBarCtrl().EnableButton( ID_DEL_TIME, TRUE );

			m_ToolBarSIDAssignment.GetToolBarCtrl().EnableButton(ID_NEW_SIDASSIGNMENT, FALSE);
			m_ToolBarSIDAssignment.GetToolBarCtrl().EnableButton(ID_DEL_SIDASSIGNMENT, FALSE);
		}
		break;

	case TREEITEMCATE_TIME:
		{
			m_ToolBarTime.GetToolBarCtrl().EnableButton( ID_NEW_TIME, FALSE );
			m_ToolBarTime.GetToolBarCtrl().EnableButton( ID_EDIT_TIME, TRUE );
			m_ToolBarTime.GetToolBarCtrl().EnableButton( ID_DEL_TIME, TRUE );

			m_ToolBarSIDAssignment.GetToolBarCtrl().EnableButton(ID_NEW_SIDASSIGNMENT, TRUE);
			m_ToolBarSIDAssignment.GetToolBarCtrl().EnableButton(ID_DEL_SIDASSIGNMENT, FALSE);
		}
		break;

	case TREEITEMCATE_NONE:
		{
			m_ToolBarTime.GetToolBarCtrl().EnableButton( ID_NEW_TIME, TRUE );
			m_ToolBarTime.GetToolBarCtrl().EnableButton( ID_EDIT_TIME, FALSE );
			m_ToolBarTime.GetToolBarCtrl().EnableButton( ID_DEL_TIME, FALSE );

			m_ToolBarSIDAssignment.GetToolBarCtrl().EnableButton(ID_DEL_SIDASSIGNMENT, FALSE);
			m_ToolBarSIDAssignment.GetToolBarCtrl().EnableButton(ID_DEL_SIDASSIGNMENT, FALSE);
		}
		break;
	}
}

void CDlgSIDAssignment::InitListCtrl(void)
{
	// set list control window style
	DWORD dwStyle = m_SIDAssigmentCtrl.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_EX_GRIDLINES;
	m_SIDAssigmentCtrl.SetExtendedStyle(dwStyle);

	CStringList strList;
	strList.RemoveAll();
	CString strCaption;

	LV_COLUMNEX lvc;
	lvc.mask = LVCF_WIDTH|LVCF_TEXT;

	strCaption.LoadString(IDS_USESID);
	lvc.pszText = (LPSTR)(LPCTSTR)strCaption;
	lvc.cx = 180;
	lvc.fmt = LVCFMT_DROPDOWN;
	lvc.csList = &m_SIDNameList;
	m_SIDAssigmentCtrl.InsertColumn(0, &lvc);

	strCaption.LoadString(IDS_PERCENT);
	lvc.pszText = (LPSTR)(LPCTSTR)strCaption;
	lvc.cx = 100;
	lvc.fmt = LVCFMT_NUMBER;
	m_SIDAssigmentCtrl.InsertColumn(1, &lvc);
}

void CDlgSIDAssignment::SetListContent(void)
{
	m_SIDAssigmentCtrl.DeleteAllItems();

	//if (NULL == m_pCurFlightUseSIDList)
	//{
	//	return;
	//}

	//size_t count = m_pCurFlightUseSIDList->GetElementCount();
	//for (size_t i=0; i<count; i++)
	//{
	//	CFlightUseSID *pFlightUseSID = m_pCurFlightUseSIDList->GetItem(i);

	//	CString strFlightSID;
	//	pFlightUseSID->GetFlightType().screenPrint(strFlightSID.GetBuffer(1024));
	//	m_SIDAssigmentCtrl.InsertItem(i, strFlightSID);
	//	m_SIDAssigmentCtrl.SetItemData(i, (DWORD_PTR)pFlightUseSID);

	//	int nSidID = pFlightUseSID->GetSidID();
	//	CString strSIDName;

	//	map<int, int>::const_iterator itor = m_AirRouteID.begin();
	//	for (int j=0; itor != m_AirRouteID.end(); itor++,j++)
	//	{
	//		if (nSidID == itor->second)
	//		{
	//			strSIDName = m_SIDNameList.GetAt(m_SIDNameList.FindIndex(j));

	//			m_SIDAssigmentCtrl.SetItemText(i, 1, strSIDName);
	//		}
	//	}
	//}
	HTREEITEM hItem = m_wndTreeFltTime.GetSelectedItem();
	if(hItem == NULL)
		return;


	FlightSIDAssignment* pItem = (FlightSIDAssignment*)m_wndTreeFltTime.GetItemData(hItem);
	ASSERT(pItem != NULL);

	int nCount = pItem->GetSIDCount();
	if (nCount == 0)
		return;

	for(int i =0; i< nCount; i++)
	{
		SIDPercentage* pStarItem = pItem->GetSIDItem(i);

		int nStarID = pStarItem->GetSIDID();
		CString strStarName;

		map<int, int>::const_iterator itor = m_AirRouteID.begin();
		for (int j=0; itor != m_AirRouteID.end(); itor++,j++)
		{
			if (nStarID == itor->second)
			{
				strStarName = m_SIDNameList.GetAt(m_SIDNameList.FindIndex(j));
				break;
			}
		}
		m_SIDAssigmentCtrl.InsertItem(i,strStarName);

		CString strValue;
		strValue.Format("%d",pStarItem->GetPercent());
		m_SIDAssigmentCtrl.SetItemText(i, 1, strValue);

		m_SIDAssigmentCtrl.SetItemData(i, (DWORD_PTR)pStarItem);
	}
}

void CDlgSIDAssignment::InitFlightTypeTree(void)
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
	int nCount = m_pSIDAssignmentList->GetSIDCount();
	CString strFilghtType;
	HTREEITEM hFltTypeTreeItem;
	for (int i=0; i<nCount; i++)
	{
		FlightSIDAssignment* pItem =m_pSIDAssignmentList->GetSIDItem(i);
		m_wndTreeFltTime.SetRedraw(FALSE);

		char szBuffer[1024] = {0};
		pItem->GetFlightConstraint().screenPrint(szBuffer);

		CString strBuffer = szBuffer;
		if (strFilghtType != strBuffer)		//new item
		{
			strFilghtType = strBuffer;
			hFltTypeTreeItem = m_wndTreeFltTime.InsertItem(strFilghtType);

			FlightSIDAssignment* pAssignment = new FlightSIDAssignment;
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


void CDlgSIDAssignment::OnBnClickedButtonSave()
{
	// TODO: Add your control notification handler code here
	SaveData();

	GetDlgItem(IDC_SAVE)->EnableWindow(FALSE);
}

void CDlgSIDAssignment::OnTvnSelchangedTreeFlightType(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	//HTREEITEM hItem = m_LandingRunwayCtrl.GetSelectedItem();

	//m_CurrentTreeItemCategory  = GetCurSeleltTreeItemState(hItem);

	//switch(m_CurrentTreeItemCategory) 
	//{
	//case TREEITEMCATE_RUNWAY:
	//	{
	//		m_pCurLandingRunwaySIDAssignment = NULL;
	//		m_pCurFlightUseSIDList          = NULL;
	//	}
	//	break;

	//case TREEITEMCATE_RUNWAYMARKING:
	//	{
	//		HTREEITEM hCurItem = m_LandingRunwayCtrl.GetSelectedItem();

	//		int nMarkingID;
	//		nMarkingID = (int)m_LandingRunwayCtrl.GetItemData(hCurItem);

	//		HTREEITEM hParentItem = m_LandingRunwayCtrl.GetParentItem(hCurItem);

	//		int nRunwayID;
	//		nRunwayID = m_LandingRunwayCtrl.GetItemData(hParentItem);

	//		m_pCurLandingRunwaySIDAssignment = NULL;
	//		m_pSIDAssignmentList->GetItemByRunwayIDAndMarking(nRunwayID, nMarkingID, &m_pCurLandingRunwaySIDAssignment);
	//		m_pCurFlightUseSIDList = NULL;
	//	}
	//	break;

	//case TREEITEMCATE_RUNWAYTIME:
	//	{
	//		HTREEITEM hTimeItem = m_LandingRunwayCtrl.GetSelectedItem();

	//		CFlightUseSIDList *pFlightUseSIDLst = (CFlightUseSIDList*)m_LandingRunwayCtrl.GetItemData(hTimeItem);

	//		HTREEITEM hMarkingItem = m_LandingRunwayCtrl.GetParentItem(hTimeItem);
	//		int nMarkingID;
	//		nMarkingID = (int)m_LandingRunwayCtrl.GetItemData(hMarkingItem);

	//		HTREEITEM hRunwayItem = m_LandingRunwayCtrl.GetParentItem(hMarkingItem);
	//		int nRunwayID;
	//		nRunwayID = m_LandingRunwayCtrl.GetItemData(hRunwayItem);


	//		m_pCurLandingRunwaySIDAssignment = NULL;
	//		m_pSIDAssignmentList->GetItemByRunwayIDAndMarking(nRunwayID, nMarkingID, &m_pCurLandingRunwaySIDAssignment);

	//		ElapsedTime estFromTime = pFlightUseSIDLst->GetFromTime();
	//		ElapsedTime estToTime  = pFlightUseSIDLst->GetToTime();

	//		m_pCurFlightUseSIDList = NULL;
	//		m_pCurLandingRunwaySIDAssignment->GetItemByTime(estFromTime, estToTime, &m_pCurFlightUseSIDList);
	//	}
	//	break;

	//default:
	//	{
	//		m_pCurLandingRunwaySIDAssignment = NULL;
	//		m_pCurFlightUseSIDList          = NULL;
	//	}
	//	break;
	//}

	//UpdateToolBar();
	//SetListContent();
	HTREEITEM hItem = m_wndTreeFltTime.GetSelectedItem();

	m_CurrentTreeItemCategory  = GetCurSeleltTreeItemState(hItem);

	switch(m_CurrentTreeItemCategory) 
	{
		case TREEITEMCATE_FLIGHTTYPE:
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

void CDlgSIDAssignment::OnCmdNew()
{
	//ElapsedTime estFromTime;
	//ElapsedTime estToTime;

	//CDlgTimeRange dlg(estFromTime, estToTime);

	//if(IDOK!= dlg.DoModal())
	//	return;

	//CString strTimeRange;
	//strTimeRange.Format("%s - %s", dlg.GetStartTimeString(), dlg.GetEndTimeString());

	//estFromTime = dlg.GetStartTime();
	//estToTime = dlg.GetEndTime();

	//CFlightUseSIDList *pFlightUseSIDLst = new CFlightUseSIDList();
	//pFlightUseSIDLst->SetFromTime(estFromTime);
	//pFlightUseSIDLst->SetToTime(estToTime);

	//HTREEITEM hCurItem = m_LandingRunwayCtrl.GetSelectedItem();
	//HTREEITEM hParentItem = m_LandingRunwayCtrl.GetParentItem(hCurItem);
	//int nRunwayID;
	//nRunwayID = (int)m_LandingRunwayCtrl.GetItemData(hParentItem);	

	//BOOL bIsConflict = FALSE;
	//bIsConflict = m_pSIDAssignmentList->IsConflictTime(nRunwayID, pFlightUseSIDLst);

	////if the time is exist
	//if (bIsConflict)
	//{
	//	delete pFlightUseSIDLst;

	//	MessageBox(_T("Time Range: \" ") +strTimeRange+ _T(" \" conflict."));

	//	return;
	//}

	//if (NULL == m_pCurLandingRunwaySIDAssignment)
	//{
	//	CLandingRunwaySIDAssignment *pLandingRunwaySIDAssignment = new CLandingRunwaySIDAssignment();

	//	HTREEITEM hCurItem = m_LandingRunwayCtrl.GetSelectedItem();
	//	int nMarkingID;
	//	nMarkingID = (int)m_LandingRunwayCtrl.GetItemData(hCurItem);

	//	HTREEITEM hParentItem = m_LandingRunwayCtrl.GetParentItem(hCurItem);
	//	int nRunwayID;
	//	nRunwayID = (int)m_LandingRunwayCtrl.GetItemData(hParentItem);		

	//	pLandingRunwaySIDAssignment->SetLandingRunwayID(nRunwayID);
	//	pLandingRunwaySIDAssignment->SetRunwayMarkingIndex(nMarkingID);

	//	pLandingRunwaySIDAssignment->AddNewItem(pFlightUseSIDLst);
	//	m_pSIDAssignmentList->AddNewItem(pLandingRunwaySIDAssignment);
	//	m_pSIDAssignmentList->GetItemByRunwayIDAndMarking(nRunwayID, nMarkingID, &m_pCurLandingRunwaySIDAssignment);
	//}
	//else
	//{
	//	m_pCurLandingRunwaySIDAssignment->AddNewItem(pFlightUseSIDLst);
	//}

	//HTREEITEM hTreeItem = m_LandingRunwayCtrl.GetSelectedItem();
	//HTREEITEM hChildTreeItem = m_LandingRunwayCtrl.InsertItem(strTimeRange, hTreeItem, TVI_LAST);
	//m_LandingRunwayCtrl.SetItemData(hChildTreeItem, (DWORD_PTR)(pFlightUseSIDLst));
	//m_LandingRunwayCtrl.Expand(hTreeItem, TVE_EXPAND);
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

	//fltType.WriteSyntaxString(szBuffer);

	//fltType.screenPrint(szBuffer);
	//m_wndTreeFltTime.SetRedraw(FALSE);
	//hItem = m_wndTreeFltTime.InsertItem(szBuffer);
	m_wndTreeFltTime.SetRedraw(FALSE);
	fltType.screenPrint(szBuffer);

	hItem = m_wndTreeFltTime.InsertItem(szBuffer);
	fltType.WriteSyntaxStringWithVersion(szBuffer);

	FlightSIDAssignment* pAssignment = new FlightSIDAssignment;
	pAssignment->SetAirportDatabase(m_pAirportDB);
	pAssignment->setFlightType(szBuffer);
	m_wndTreeFltTime.SetItemData(hItem,(DWORD_PTR)pAssignment);
	m_vFlightTypeDelList.push_back(pAssignment);
	m_wndTreeFltTime.SetRedraw(TRUE);

	GetDlgItem(IDC_SAVE)->EnableWindow(TRUE);
}

void CDlgSIDAssignment::OnCmdDelete()
{
	//if (NULL == m_pCurLandingRunwaySIDAssignment)
	//{
	//	return;
	//}

	//HTREEITEM hTreeItem = m_LandingRunwayCtrl.GetSelectedItem();

	//CFlightUseSIDList *pFlightUseSIDLst = (CFlightUseSIDList*)m_LandingRunwayCtrl.GetItemData(hTreeItem);

	//m_pCurLandingRunwaySIDAssignment->DeleteItemByTime(pFlightUseSIDLst->GetFromTime(), pFlightUseSIDLst->GetToTime());

	//m_LandingRunwayCtrl.DeleteItem(hTreeItem);

	HTREEITEM hSelItem = m_wndTreeFltTime.GetSelectedItem();
	m_CurrentTreeItemCategory = GetCurSeleltTreeItemState(hSelItem);
	if (m_CurrentTreeItemCategory == TREEITEMCATE_NONE)
		return;

	m_wndTreeFltTime.SetRedraw(FALSE);
	if ( m_CurrentTreeItemCategory == TREEITEMCATE_FLIGHTTYPE)
	{
		FlightSIDAssignment* pAssignment = (FlightSIDAssignment*)m_wndTreeFltTime.GetItemData(hSelItem);
		CString strFlightType = pAssignment->GetFlightTypeName();

		int nCount = m_pSIDAssignmentList->GetSIDCount();
		for (int  i = 0; i < nCount ; i++)
		{
			FlightSIDAssignment* pItem = m_pSIDAssignmentList->GetSIDItem(i);
			if (pItem == NULL)
				continue;
			if (pItem->GetFlightTypeName() == strFlightType)
			{
				m_pSIDAssignmentList->DeleteSIDItem(pItem);
			}
		}
		m_wndTreeFltTime.DeleteItem(hSelItem);
	}
	if (m_CurrentTreeItemCategory == TREEITEMCATE_TIME)
	{
		OnDelTimeRange();
	}
	m_wndTreeFltTime.SetRedraw(TRUE);
	GetDlgItem(IDC_SAVE)->EnableWindow(TRUE);
}

void CDlgSIDAssignment::OnCmdEdit()
{
	//if (NULL == m_pCurLandingRunwaySIDAssignment)
	//{
	//	return;
	//}

	//HTREEITEM hTreeItem = m_LandingRunwayCtrl.GetSelectedItem();

	//CFlightUseSIDList *pFlightUseSIDLst = (CFlightUseSIDList*)m_LandingRunwayCtrl.GetItemData(hTreeItem);

	//if (NULL == pFlightUseSIDLst)
	//{
	//	return;
	//}

	//ElapsedTime estFromTime = pFlightUseSIDLst->GetFromTime();
	//ElapsedTime estToTime = pFlightUseSIDLst->GetToTime();

	//CDlgTimeRange dlg(estFromTime, estToTime);

	//if(IDOK!= dlg.DoModal())
	//{
	//	return;
	//}

	//HTREEITEM hParentTree = m_LandingRunwayCtrl.GetParentItem(hTreeItem);
	//HTREEITEM hRunwayItem = m_LandingRunwayCtrl.GetParentItem(hParentTree);
	//int nRunwayID = (int)m_LandingRunwayCtrl.GetItemData(hRunwayItem);

	//pFlightUseSIDLst->SetFromTime(dlg.GetStartTime());
	//pFlightUseSIDLst->SetToTime(dlg.GetEndTime());

	//BOOL bIsConflict = FALSE;
	//bIsConflict = m_pSIDAssignmentList->IsConflictTime(nRunwayID, pFlightUseSIDLst);

	//CString strTimeRange;
	//strTimeRange.Format("%s - %s", dlg.GetStartTimeString(), dlg.GetEndTimeString());

	////the new time is exist
	//if (bIsConflict)
	//{
	//	//resume
	//	pFlightUseSIDLst->SetFromTime(estFromTime);
	//	pFlightUseSIDLst->SetToTime(estToTime);

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
		FlightSIDAssignment* pAssignment = (FlightSIDAssignment*)m_wndTreeFltTime.GetItemData(hSelItem);
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

		int nCount = m_pSIDAssignmentList->GetSIDCount();
		for (int  i = 0; i < nCount ; i++)
		{
			FlightSIDAssignment* pItem = m_pSIDAssignmentList->GetSIDItem(i);
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

void CDlgSIDAssignment::OnAddTimeRange()
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

		FlightSIDAssignment *pItem = new FlightSIDAssignment();
		pItem->SetStartTime(estFromTime);
		pItem->SetEndTime(estToTime);

		FlightSIDAssignment* pAssignment = (FlightSIDAssignment*)m_wndTreeFltTime.GetItemData(hSelItem);
		CString strFlightType = pAssignment->GetFlightTypeName();

		pItem->SetAirportDatabase(m_pAirportDB);
		pItem->setFlightType(strFlightType);

		m_pSIDAssignmentList->AddSIDItem(pItem);
		HTREEITEM hChildTreeItem = m_wndTreeFltTime.InsertItem(strTimeRange, hSelItem, TVI_LAST);
		m_wndTreeFltTime.SetItemData(hChildTreeItem, (DWORD_PTR)(pItem));
		m_wndTreeFltTime.Expand(hSelItem, TVE_EXPAND);
		GetDlgItem(IDC_SAVE)->EnableWindow(TRUE);

	}

}

void CDlgSIDAssignment::OnEditTimeRange()
{
	HTREEITEM hSelItem = m_wndTreeFltTime.GetSelectedItem();
	m_CurrentTreeItemCategory = GetCurSeleltTreeItemState(hSelItem);
	if (m_CurrentTreeItemCategory == TREEITEMCATE_TIME)
	{
		FlightSIDAssignment* pItem = (FlightSIDAssignment*)m_wndTreeFltTime.GetItemData(hSelItem);
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

void CDlgSIDAssignment::OnDelTimeRange()
{
	HTREEITEM hSelItem = m_wndTreeFltTime.GetSelectedItem();
	m_CurrentTreeItemCategory = GetCurSeleltTreeItemState(hSelItem);

	if (m_CurrentTreeItemCategory == TREEITEMCATE_TIME)
	{
		FlightSIDAssignment* pItem = (FlightSIDAssignment*)m_wndTreeFltTime.GetItemData(hSelItem);
		if(pItem == NULL)
			return;
		m_pSIDAssignmentList->DeleteSIDItem(pItem);
		m_wndTreeFltTime.DeleteItem(hSelItem);
		GetDlgItem(IDC_SAVE)->EnableWindow(TRUE);

	}
}

void CDlgSIDAssignment::OnCmdNewSIDAssignment()
{
	//if (m_pSelectFlightType == NULL)
	//	return;

	//FlightConstraint fltType = (*m_pSelectFlightType)(NULL);

	//char szBuffer[1024] = {0};
	//fltType.screenPrint(szBuffer);

	////If the Flight Type is exist
	//int nCount = m_SIDAssigmentCtrl.GetItemCount();
	//for (int i=0; i<nCount; i++)
	//{
	//	if (!m_SIDAssigmentCtrl.GetItemText(i, 0).Compare(szBuffer))
	//	{
	//		MessageBox(_T("The Flight Type already exists!"));

	//		return;
	//	}
	//}

	//if (m_AirRouteID.size() < 1)
	//{
	//	MessageBox(_T("No SID Exist!."));

	//	return;
	//}

	//CFlightUseSID *pFlightUseSID = new CFlightUseSID();
	//pFlightUseSID->SetFlightType(fltType);
	//map<int, int>::const_iterator iter = m_AirRouteID.begin();
	//int nSidID = iter->second;
	//pFlightUseSID->SetSidID(nSidID);

	//if (NULL == m_pCurFlightUseSIDList)
	//{
	//	delete pFlightUseSID;
	//	return;
	//}

	//m_pCurFlightUseSIDList->AddNewItem(pFlightUseSID);	

	//int newItem = m_SIDAssigmentCtrl.InsertItem(m_SIDAssigmentCtrl.GetItemCount(), szBuffer);
	//m_SIDAssigmentCtrl.SetItemData(newItem, (DWORD_PTR)pFlightUseSID);
	//m_SIDAssigmentCtrl.SetItemText(newItem, 1, m_SIDNameList.GetHead());

	HTREEITEM hSelItem = m_wndTreeFltTime.GetSelectedItem();
	m_CurrentTreeItemCategory = GetCurSeleltTreeItemState(hSelItem);
	if (m_CurrentTreeItemCategory != TREEITEMCATE_TIME)
		return;

	if (m_AirRouteID.size() < 1)
	{
		MessageBox(_T("No Star Exist!."));
		return;
	}

	FlightSIDAssignment* pItem = (FlightSIDAssignment*)m_wndTreeFltTime.GetItemData(hSelItem);
	int nPercent = 100;
	int nCount = pItem->GetSIDCount();
	for(int i = 0; i < nCount; i++)
	{
		SIDPercentage* pSIDData = pItem->GetSIDItem(i); 
		if (pSIDData == NULL)
			continue;
		nPercent -= pSIDData->GetPercent();
	}
	if (nPercent <= 0)
	{
		MessageBox("The sum of star operating percent exceed 100%!");
		return;
	}

	SIDPercentage *pData = new SIDPercentage;

	map<int, int>::const_iterator iter = m_AirRouteID.begin();
	int nSIDID = iter->second;
	pData->SetSIDID(nSIDID);
	pData->SetPercent(nPercent);

	pItem->AddSIDItem(pData);	

	int nItem = m_SIDAssigmentCtrl.InsertItem(m_SIDAssigmentCtrl.GetItemCount(), m_SIDNameList.GetHead());
	CString strPercent;
	strPercent.Format("%d", nPercent);
	m_SIDAssigmentCtrl.SetItemText(nItem, 1, strPercent);
	m_SIDAssigmentCtrl.SetItemData(nItem, (DWORD_PTR)pData);
	GetDlgItem(IDC_SAVE)->EnableWindow(TRUE);

}

void CDlgSIDAssignment::OnCmdDeleteSIDAssignment()
{
	//ASSERT(m_pCurFlightUseSID != NULL);
	//ASSERT(m_pCurFlightUseSIDList != NULL);

	//m_SIDAssigmentCtrl.DeleteItem(m_SIDAssigmentCtrl.GetCurSel());

	//m_pCurFlightUseSIDList->DeleteItem(m_pCurFlightUseSID);
	//m_pCurFlightUseSID = NULL;

	//GetDlgItem(IDC_SAVE)->EnableWindow(TRUE);

	HTREEITEM hSelItem = m_wndTreeFltTime.GetSelectedItem();

	m_CurrentTreeItemCategory = GetCurSeleltTreeItemState(hSelItem);
	if (m_CurrentTreeItemCategory != TREEITEMCATE_TIME)
		return;

	SIDPercentage* pData = GetCurFlightUseSID();
	if (pData == NULL)
		return;

	FlightSIDAssignment* pItem = (FlightSIDAssignment*)m_wndTreeFltTime.GetItemData(hSelItem);
	m_SIDAssigmentCtrl.DeleteItem(m_SIDAssigmentCtrl.GetCurSel());
	pItem->DeleteSIDItem(pData);

	GetDlgItem(IDC_SAVE)->EnableWindow(TRUE);
}

LRESULT CDlgSIDAssignment::OnMsgComboChange(WPARAM wParam, LPARAM lParam)
{
	int nComboxSel = (int)wParam;
	if (nComboxSel == LB_ERR)
		return 0;

	//if (NULL == m_pCurFlightUseSID)
	//{
	//	return 0;
	//}

	//map<int, int>::const_iterator iter = m_AirRouteID.find(nComboxSel);
	//int nSidID = iter->second;
	//m_pCurFlightUseSID->SetSidID(nSidID);

	//GetDlgItem(IDC_SAVE)->EnableWindow(TRUE);

	SIDPercentage* pData = GetCurFlightUseSID();
	if (NULL == pData)
	{
		return NULL;
	}

	map<int, int>::const_iterator iter = m_AirRouteID.find(nComboxSel);
	int nStarID = iter->second;
	pData->SetSIDID(nStarID);

	//CString strValue = m_SIDAssigmentCtrl.GetItemText(m_SIDAssigmentCtrl.GetCurSel(), 1);
	//pData->SetPercent((int)(atoi(strValue)));

	GetDlgItem(IDC_SAVE)->EnableWindow(TRUE);
	return 0;
}

void CDlgSIDAssignment::OnLvnItemchangedListSIDAssignment(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	//GetCurFlightUseSID();

	if (0 < m_SIDAssigmentCtrl.GetSelectedCount())
	{
		m_ToolBarSIDAssignment.GetToolBarCtrl().EnableButton(ID_DEL_SIDASSIGNMENT, TRUE);
	}
	else
	{
		m_ToolBarSIDAssignment.GetToolBarCtrl().EnableButton(ID_DEL_SIDASSIGNMENT, FALSE);
	}

	*pResult = 0;
}

void CDlgSIDAssignment::OnNMSetfocusListSIDAssignment(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here
	//GetCurFlightUseSID();

	if (0 < m_SIDAssigmentCtrl.GetSelectedCount())
	{
		m_ToolBarSIDAssignment.GetToolBarCtrl().EnableButton(ID_DEL_SIDASSIGNMENT, TRUE);
	}
	else
	{
		m_ToolBarSIDAssignment.GetToolBarCtrl().EnableButton(ID_DEL_SIDASSIGNMENT, FALSE);
	}

	*pResult = 0;
}

void CDlgSIDAssignment::OnNMKillfocusListSIDAssignment(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here
	m_ToolBarSIDAssignment.GetToolBarCtrl().EnableButton(ID_DEL_SIDASSIGNMENT, FALSE);

	*pResult = 0;
}

SIDPercentage* CDlgSIDAssignment::GetCurFlightUseSID(void)
{
	int nSelectCount = m_SIDAssigmentCtrl.GetSelectedCount();

	if (nSelectCount < 1)
	{
		return NULL;
	}

	int nSelectItem = m_SIDAssigmentCtrl.GetCurSel();

	return (SIDPercentage*)m_SIDAssigmentCtrl.GetItemData(nSelectItem);


}

void CDlgSIDAssignment::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	SaveData();

	OnOK();
}

void CDlgSIDAssignment::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	OnCancel();
}

TREEITEMCATE CDlgSIDAssignment::GetCurSeleltTreeItemState(HTREEITEM hTreeItem)
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

void CDlgSIDAssignment::SaveData()
{
	try
	{
		CADODatabase::BeginTransaction() ;
		m_pSIDAssignmentList->SaveData();
		CADODatabase::CommitTransaction() ;
	}
	catch (CADOException &e)
	{
		CADODatabase::RollBackTransation() ;
		e.ErrorMessage();
	}
}

void CDlgSIDAssignment::OnContextMenu(CWnd* pWnd, CPoint point)
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

void CDlgSIDAssignment::OnLvnEndlabeleditListContents(NMHDR *pNMHDR, LRESULT *pResult)
{
	int iTotals = 0 , nCount = 0;
	LV_DISPINFO* plvDispInfo = (LV_DISPINFO *)pNMHDR;
	LV_ITEM* plvItem = &plvDispInfo->item;
	int nItem = plvItem->iItem;
	int nSubItem = plvItem->iSubItem;
	if (nItem < 0 || nSubItem < 1)
		return;

	nCount = m_SIDAssigmentCtrl.GetItemCount();
	for (int i = 0; i < nCount ; i++)
	{
		if(plvItem->iItem != i)
			iTotals += _ttol(m_SIDAssigmentCtrl.GetItemText(i,1) );
	}
	if (iTotals + _ttol(m_SIDAssigmentCtrl.GetItemText(nItem,nSubItem) ) > 100)
	{
		CString strReplace = _T("0");
		strReplace.Format(_T("%d"),100 - iTotals);
		m_SIDAssigmentCtrl.SetItemText(nItem,nSubItem,strReplace);
		MessageBox(_T("The sum of operation percent can not exceed 100%."));
		return;
	}

	m_SIDAssigmentCtrl.OnEndlabeledit(pNMHDR, pResult);	

	CString strValue = m_SIDAssigmentCtrl.GetItemText(nItem, nSubItem);
	SIDPercentage* pItem = (SIDPercentage*)m_SIDAssigmentCtrl.GetItemData(nItem);
	if (nSubItem == 1)
	{
		pItem->SetPercent((int)(atoi(strValue)));
	}

	*pResult = 0;
}

bool CDlgSIDAssignment::IsvalidTimeRange(ElapsedTime tStart, ElapsedTime tEnd)
{

	return true;
}