// CDlgOutboundRouteAssignment2.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "DlgBoundRouteAssignment2.h"
#include "../InputAirside/BoundRouteAssignment.h"
#include "../InputAirside/TaxiRoute.h"
#include "../InputAirside/RouteItem.h"
#include "InputAirside\ALTObjectGroup.h"
#include "InputAirside\Runway.h"
#include "InputAirside\Taxiway.h"
#include "DlgSelectAirsideOrigDest.h"
#include "RunwayExitAndStandFamilySelectDlg.h"
#include "SelectInboundRouteItemDlg.h"
#include "Common/AirportDatabase.h"
#include "DlgTimeRange.h"
#include "../Database/DBElementCollection_Impl.h"
#include ".\dlgboundrouteassignment2.h"


namespace
{
	const UINT ID_NEW_RUNWAYEXITANDSTANDFAMILY      = 10000;
	const UINT ID_EDIT_RUNWAYEXITANDSTANDFAMILY     = 10001;
	const UINT ID_DEL_RUNWAYEXITANDSTANDFAMILY      = 10002;

	const UINT ID_NEW_ROUTE  = 20000;
	const UINT ID_DEL_ROUTE  = 20001;
	const UINT ID_EDIT_ROUTE  = 20002;
	const UINT ID_MOVE_NEXT = 20003;
	const UINT ID_MOVE_PREV = 20004;

	const UINT ID_NEW_FLIGHTTY = 30000 ;
	const UINT ID_DEL_FLIGHTTY = 30001 ;
	const UINT ID_EDIT_FLIGHTTY = 30002 ;


	const UINT ID_WND_SPLITTER_VER = 40001 ;
	const UINT ID_WND_SPLITTER_HOR = 40002 ;
}

// CDlgBoundRouteAssignment2 dialog
static const char* szBuff[] = {_T("No"),_T("Yes")};

IMPLEMENT_DYNAMIC(CDlgBoundRouteAssignment2, CXTResizeDialogEx)

CDlgBoundRouteAssignment2::CDlgBoundRouteAssignment2(int nProjID,PFuncSelectFlightType pSelectFlightType,CAirportDatabase *pAirPortdb, LPCTSTR lpszTitle, CWnd* pParent /*=NULL*/)
	: CXTResizeDialogEx(CXTResizeDialogEx::styleUseInitSizeAsMinSize, CDlgBoundRouteAssignment2::IDD, pParent)
	, m_nProjID(nProjID)
	, m_pCurBoundRoute(NULL)
	, m_pCurTimeAssignment(NULL)
	, m_pSelectFlightType(pSelectFlightType)
	, m_pAirportDB(pAirPortdb)
	, m_strTitle(lpszTitle)
{

}

CDlgBoundRouteAssignment2::~CDlgBoundRouteAssignment2()
{
	if(m_pBoundRouteAssignment)
	{
		delete m_pBoundRouteAssignment;
		m_pBoundRouteAssignment = NULL;
	}

}

void CDlgBoundRouteAssignment2::DoDataExchange(CDataExchange* pDX)
{
	CXTResizeDialogEx::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_TREE_FLIGHTTYPE_TIME, m_wndTreeFltTime);
	DDX_Control(pDX, IDC_LIST_RUNWAYEXITANDSTANDFAMILY, m_listOrgDst);
	DDX_Control(pDX, IDC_TREE_ROUTE, m_TreeCtrlRoute);
	DDX_Control(pDX, IDC_RADIO_SHORTESTPATH, m_btnShortestPath);
	DDX_Control(pDX, IDC_RADIO_PREFERREDROUTE, m_btnPreferredRoute);
}


BEGIN_MESSAGE_MAP(CDlgBoundRouteAssignment2, CXTResizeDialogEx)
	ON_WM_CREATE()
	ON_COMMAND(ID_NEW_RUNWAYEXITANDSTANDFAMILY, OnCmdNewOrgDst)
	ON_COMMAND(ID_EDIT_RUNWAYEXITANDSTANDFAMILY, OnCmdEditOrgDst)
	ON_COMMAND(ID_DEL_RUNWAYEXITANDSTANDFAMILY, OnCmdDelOrgDst)
	ON_COMMAND(ID_NEW_FLIGHTTY ,OnCmdNewFlightTime) 
	ON_COMMAND(ID_DEL_FLIGHTTY , OnCmdDelFlightTime)
	ON_COMMAND(ID_EDIT_FLIGHTTY, OnCmdEditFlightTime) 
	ON_COMMAND(ID_NEW_ROUTE, OnCmdNewRoute)
	ON_COMMAND(ID_DEL_ROUTE, OnCmdDelRoute)
	ON_COMMAND(ID_EDIT_ROUTE, OnCmdEditRoute)
	ON_COMMAND(ID_MOVE_NEXT, OnCmdMoveNextPriority)
	ON_COMMAND(ID_MOVE_PREV, OnCmdMovePreviousPriority)
	ON_COMMAND(ID_ADD_TIMERANGE,OnAddTimeRange)
	ON_COMMAND(ID_EDIT_TIMERANGE,OnEditTimeRange)
	ON_COMMAND(ID_DEL_TIMERANGE,OnDelTimeRange)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_RUNWAYEXITANDSTANDFAMILY, OnLvnItemchangedOrgDst)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_ROUTE, OnTvnSelchangedTreeRoute)
	ON_BN_CLICKED(IDC_BUTTON_SAVE, OnBnClickedButtonSave)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_FLIGHTTYPE_TIME, OnTvnSelchangedTreeFlighttypeTime)
	ON_WM_CONTEXTMENU()
	ON_BN_CLICKED(IDC_RADIO_SHORTESTPATH, OnBnClickedRadioShortestpath)
	ON_BN_CLICKED(IDC_RADIO_PREFERREDROUTE, OnBnClickedRadioPreferredroute)
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CDlgBoundRouteAssignment2 message handlers
int CDlgBoundRouteAssignment2::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CXTResizeDialogEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!m_toolbarOrgDst.CreateEx(this, TBSTYLE_FLAT,WS_CHILD|WS_VISIBLE|CBRS_ALIGN_TOP)
		|| !m_toolbarOrgDst.LoadToolBar(IDR_TOOLBAR_ADD_DEL_EDIT))
	{
		return -1;
	}

	CToolBarCtrl& tbOrgDst = m_toolbarOrgDst.GetToolBarCtrl();
	tbOrgDst.SetCmdID(0, ID_NEW_RUNWAYEXITANDSTANDFAMILY);
	tbOrgDst.SetCmdID(1, ID_DEL_RUNWAYEXITANDSTANDFAMILY);
	tbOrgDst.SetCmdID(2, ID_EDIT_RUNWAYEXITANDSTANDFAMILY);

	if (!m_ToolBarRoute.CreateEx(this, TBSTYLE_FLAT,WS_CHILD|WS_VISIBLE|CBRS_ALIGN_TOP)
		|| !m_ToolBarRoute.LoadToolBar(IDR_TOOLBAR_ROUTE_PRIORITY))
	{
		return -1;
	}
 
//	m_ToolBarRoute.SetDlgCtrlID(m_toolbarOrgDst.GetDlgCtrlID()+1);

	CToolBarCtrl& tbRoute = m_ToolBarRoute.GetToolBarCtrl();
	tbRoute.SetCmdID(0, ID_NEW_ROUTE);
	tbRoute.SetCmdID(1, ID_DEL_ROUTE);
	tbRoute.SetCmdID(2, ID_EDIT_ROUTE);
	tbRoute.SetCmdID(3, ID_MOVE_PREV);
	tbRoute.SetCmdID(4, ID_MOVE_NEXT);

	if(!m_ToolBarFlightType.CreateEx(this,TBSTYLE_FLAT,WS_CHILD|WS_VISIBLE|CBRS_ALIGN_TOP)
		|| !m_ToolBarFlightType.LoadToolBar(IDR_TOOLBAR_ADD_DEL_EDIT))
	{
		return -1 ;
	}
	CToolBarCtrl& tbFlightTY = m_ToolBarFlightType.GetToolBarCtrl() ;
	tbFlightTY.SetCmdID(0,ID_NEW_FLIGHTTY);
	tbFlightTY.SetCmdID(1,ID_DEL_FLIGHTTY) ;
	tbFlightTY.SetCmdID(2,ID_EDIT_FLIGHTTY) ;

	// must set control id, otherwise their id will all be AFX_IDW_TOOLBAR
	m_ToolBarFlightType.SetDlgCtrlID(AFX_IDW_TOOLBAR + 1) ;
	m_toolbarOrgDst.SetDlgCtrlID(AFX_IDW_TOOLBAR + 2) ;
	m_ToolBarRoute.SetDlgCtrlID(AFX_IDW_TOOLBAR + 3) ;

	m_wndSplitterVer.Create(WS_CHILD | WS_VISIBLE, CRect(0,0,0,0), this, ID_WND_SPLITTER_VER);
	m_wndSplitterHor.Create(WS_CHILD | WS_VISIBLE, CRect(0,0,0,0), this, ID_WND_SPLITTER_HOR);

	return 0;
}

BOOL CDlgBoundRouteAssignment2::OnInitDialog()
{
	CXTResizeDialogEx::OnInitDialog();
	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),TRUE);
	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),FALSE);
	SetWindowText(m_strTitle);

	m_wndSplitterVer.SetStyle(SPS_VERTICAL);
	m_wndSplitterVer.SetBuddies(GetDlgItem(IDC_STATIC_FLIGHTTYPE), GetDlgItem(IDC_STATIC_ORIGDEST));
	m_wndSplitterHor.SetStyle(SPS_HORIZONTAL);
	m_wndSplitterHor.SetBuddies(GetDlgItem(IDC_STATIC_ORIGDEST), GetDlgItem(IDC_STATIC_ROUTECHOICE));

	UpdateSplitterRange();

	InitToolBar();
	InitListCtrl();
	InitFlightTimeTree() ;
	UpdateRouteChoiceBtns();

	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(FALSE);

	SetAllControlsResize();

	CRect rect;
	GetClientRect(rect);
	SetWindowPos(NULL,0,0,rect.Width(),rect.Height(),SWP_NOMOVE|SWP_NOACTIVATE|SWP_NOZORDER);

	m_TreeCtrlRoute.EnableWindow(FALSE);
	m_listOrgDst.EnableWindow(FALSE);	

	return TRUE;
}

void CDlgBoundRouteAssignment2::InitToolBar(void)
{	
	CRect rect;
	//Flight type
	GetDlgItem(IDC_STATIC_FlIGHTTOOLBAR)->GetWindowRect(rect);
	ScreenToClient(rect);
	m_ToolBarFlightType.MoveWindow(&rect);

	m_ToolBarFlightType.GetToolBarCtrl().EnableButton( ID_NEW_FLIGHTTY, TRUE );
	m_ToolBarFlightType.GetToolBarCtrl().EnableButton( ID_DEL_FLIGHTTY, FALSE );
	m_ToolBarFlightType.GetToolBarCtrl().EnableButton( ID_EDIT_FLIGHTTY, FALSE );

	//Runway exit and stand family
	GetDlgItem(IDC_STATIC_ORIGDEST_TOOLBAR)->GetWindowRect(rect);
	ScreenToClient(rect);
	m_toolbarOrgDst.MoveWindow(&rect);

	m_toolbarOrgDst.GetToolBarCtrl().EnableButton( ID_NEW_RUNWAYEXITANDSTANDFAMILY, FALSE );
	m_toolbarOrgDst.GetToolBarCtrl().EnableButton( ID_DEL_RUNWAYEXITANDSTANDFAMILY, FALSE );
	m_toolbarOrgDst.GetToolBarCtrl().EnableButton( ID_EDIT_RUNWAYEXITANDSTANDFAMILY, FALSE );

	//route
	GetDlgItem(IDC_STATIC_ROUTEASSIGN_TOOLBAR)->GetWindowRect(rect);
	ScreenToClient(rect);
	m_ToolBarRoute.MoveWindow(&rect);

	m_ToolBarRoute.GetToolBarCtrl().EnableButton(ID_NEW_ROUTE, FALSE);
	m_ToolBarRoute.GetToolBarCtrl().EnableButton(ID_DEL_ROUTE, FALSE);
	m_ToolBarRoute.GetToolBarCtrl().EnableButton(ID_EDIT_ROUTE, FALSE);
	m_ToolBarRoute.GetToolBarCtrl().EnableButton(ID_MOVE_PREV, FALSE);
	m_ToolBarRoute.GetToolBarCtrl().EnableButton(ID_MOVE_NEXT, FALSE);
}

void CDlgBoundRouteAssignment2::InitListCtrl(void)
{
	ASSERT(FALSE);

}

void CDlgBoundRouteAssignment2::InitFlightTimeTree()
{
	int nCount = m_pBoundRouteAssignment->GetElementCount();
	CString strFilghtType;
	HTREEITEM hFltTypeTreeItem;

	for (int i=0; i<nCount; i++)
	{
		CFlightTypeRouteAssignment* pItem =m_pBoundRouteAssignment->GetItem(i);
		m_wndTreeFltTime.SetRedraw(FALSE);

		char szBuffer[1024] = {0};
		pItem->GetFltType().screenPrint(szBuffer);

		strFilghtType = szBuffer;
		hFltTypeTreeItem = m_wndTreeFltTime.InsertItem(strFilghtType);
		m_wndTreeFltTime.SetItemData(hFltTypeTreeItem, (DWORD_PTR)pItem);

		CString strTimeRange = _T(""),strDay = _T("");		

		int nTimeCount = pItem->GetElementCount();
		for (int j =0; j < nTimeCount; j++)
		{
			CTimeRangeRouteAssignment* pTimeItem = pItem->GetItem(j);

			ElapsedTime etInsert = pTimeItem->GetStartTime();
			long lSecond = etInsert.asSeconds();
			strDay.Format(_T("Day%d %02d:%02d:%02d"),lSecond/86400 + 1,(lSecond % 86400)/3600,(lSecond % 86400 % 3600)/60,lSecond % 86400 % 3600 % 60);
			strTimeRange = strDay + _T(" - ");

			etInsert = pTimeItem->GetEndTime();
			lSecond = etInsert.asSeconds();
			strDay.Format(_T("Day%d %02d:%02d:%02d"),lSecond/86400 + 1,(lSecond % 86400)/3600,(lSecond % 86400 % 3600)/60,lSecond % 86400 % 3600 % 60);
			strTimeRange += strDay;

			HTREEITEM hTimeRangeTreeItem = m_wndTreeFltTime.InsertItem(strTimeRange, hFltTypeTreeItem, TVI_LAST);
			m_wndTreeFltTime.SetItemData(hTimeRangeTreeItem, (DWORD_PTR)pTimeItem);
		}

		m_wndTreeFltTime.Expand(hFltTypeTreeItem, TVE_EXPAND);

		m_wndTreeFltTime.SetRedraw(TRUE);
	}
}

void CDlgBoundRouteAssignment2::EditOrgDst()
{
	ASSERT(FALSE);
}

void CDlgBoundRouteAssignment2::NewOrgDst()
{
	ASSERT(FALSE);
}

void CDlgBoundRouteAssignment2::SetOrgDstListContent(void)
{
	//virtual function
	ASSERT(FALSE);
}

void CDlgBoundRouteAssignment2::OnBnClickedRadioShortestpath()
{
	// TODO: Add your control notification handler code here
	m_btnShortestPath.SetCheck(BST_CHECKED);
	m_btnPreferredRoute.SetCheck(BST_UNCHECKED);
	m_pCurBoundRoute->SetBoundRouteChoice(RouteChoice_ShortestPath);

	UpdateRouteToolBar();
	SetTreeContent();
}

void CDlgBoundRouteAssignment2::OnBnClickedRadioPreferredroute()
{
	// TODO: Add your control notification handler code here
	m_btnShortestPath.SetCheck(BST_UNCHECKED);
	m_btnPreferredRoute.SetCheck(BST_CHECKED);
	m_pCurBoundRoute->SetBoundRouteChoice(RouteChoice_Preferred);

	UpdateRouteToolBar();
	SetTreeContent();
}

void CDlgBoundRouteAssignment2::UpdateOrgDstToolBar()
{
	HTREEITEM hSelItem = m_wndTreeFltTime.GetSelectedItem();
	BOOL bSelTimeRange = hSelItem && m_wndTreeFltTime.GetParentItem(hSelItem);
	BOOL bSelOrgDstItem = bSelTimeRange && m_listOrgDst.GetCurSel()>=0;
	m_toolbarOrgDst.GetToolBarCtrl().EnableButton( ID_NEW_RUNWAYEXITANDSTANDFAMILY, bSelTimeRange );
	m_toolbarOrgDst.GetToolBarCtrl().EnableButton( ID_DEL_RUNWAYEXITANDSTANDFAMILY, bSelOrgDstItem );
	m_toolbarOrgDst.GetToolBarCtrl().EnableButton( ID_EDIT_RUNWAYEXITANDSTANDFAMILY, bSelOrgDstItem );
}
void CDlgBoundRouteAssignment2::UpdateRouteToolBar()
{
	HTREEITEM hSelItem = m_wndTreeFltTime.GetSelectedItem();
	BOOL bSelTimeRange = hSelItem && m_wndTreeFltTime.GetParentItem(hSelItem);
	BOOL bSelOrgDstItem = bSelTimeRange && m_listOrgDst.GetCurSel()>=0;
	BOOL bSelPreferred = bSelOrgDstItem && m_btnPreferredRoute.GetCheck() == BST_CHECKED;

	BOOL bNewEnable = FALSE;
	BOOL bDelEnable = FALSE;
	BOOL bEditEnable = FALSE;
	BOOL bNextEnable = FALSE;
	BOOL bPrevEnable = FALSE;

	if (bSelPreferred)
	{
		HTREEITEM hSelItem = m_TreeCtrlRoute.GetSelectedItem();
		if (hSelItem)
		{
			CTreeData* pNodeData = (CTreeData*)m_TreeCtrlRoute.GetItemData(hSelItem);
			if (pNodeData)
			{
				switch (pNodeData->m_emType)
				{
				case CTreeData::Route_Item:
					{
						bNewEnable = bDelEnable = bEditEnable = TRUE;
					}
					break;
				case CTreeData::Priority_Item:
					{
						bNewEnable = bDelEnable = bEditEnable = TRUE;
						bNextEnable = m_TreeCtrlRoute.GetNextSiblingItem(hSelItem) != NULL;
						bPrevEnable = m_TreeCtrlRoute.GetPrevSiblingItem(hSelItem) != NULL;
					}
					break;
				case CTreeData::Start_Item:
					{
						bNewEnable = TRUE;
					}
					break;
				default:
					{
// 						ASSERT(FALSE);
					}
					break;
				}
			}
			else
				ASSERT(FALSE);
		}
		else
		{
			bNewEnable = TRUE;
		}
	}
	m_ToolBarRoute.GetToolBarCtrl().EnableButton(ID_NEW_ROUTE, bNewEnable);
	m_ToolBarRoute.GetToolBarCtrl().EnableButton(ID_DEL_ROUTE, bDelEnable);
	m_ToolBarRoute.GetToolBarCtrl().EnableButton(ID_EDIT_ROUTE, bEditEnable);
	m_ToolBarRoute.GetToolBarCtrl().EnableButton(ID_MOVE_PREV, bPrevEnable);
	m_ToolBarRoute.GetToolBarCtrl().EnableButton(ID_MOVE_NEXT, bNextEnable);
}

void CDlgBoundRouteAssignment2::OnCmdNewOrgDst()
{
	NewOrgDst();
}

void CDlgBoundRouteAssignment2::OnCmdDelOrgDst()
{
	ASSERT(m_pCurBoundRoute);

	int nSelItem = m_listOrgDst.GetCurSel();

	m_TreeCtrlRoute.DeleteAllItems();
	m_listOrgDst.DeleteItemEx(nSelItem);

	m_pCurTimeAssignment->DeleteItem(m_pCurBoundRoute);

	nSelItem = m_listOrgDst.GetCurSel();
	if(nSelItem != -1)
		m_pCurBoundRoute = (CIn_OutBoundRoute*)m_listOrgDst.GetItemData(nSelItem);
	else
		m_pCurBoundRoute = NULL;

	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);

	UpdateRouteChoiceBtns();
	UpdateRouteToolBar();
	SetTreeContent();
}

void CDlgBoundRouteAssignment2::OnCmdEditOrgDst()
{
	EditOrgDst();
}

void CDlgBoundRouteAssignment2::OnCmdMovePreviousPriority()//up move
{
	HTREEITEM hItem = m_TreeCtrlRoute.GetSelectedItem();
	ASSERT(hItem);
	CTreeData* pNodeData = (CTreeData*)m_TreeCtrlRoute.GetItemData(hItem);
	ASSERT(pNodeData->m_emType == CTreeData::Priority_Item);
	CRoutePriority* pPriorityUp = (CRoutePriority*)pNodeData->m_pData;

	HTREEITEM hUpItem = m_TreeCtrlRoute.GetPrevSiblingItem(hItem);
	CTreeData* pNodeDownData = (CTreeData*)m_TreeCtrlRoute.GetItemData(hUpItem);
	ASSERT(pNodeDownData->m_emType == CTreeData::Priority_Item);
	CRoutePriority* pPriorityDown = (CRoutePriority*)pNodeDownData->m_pData;

	pNodeData->m_pData = (DWORD)pPriorityDown;
	pNodeDownData->m_pData = (DWORD)pPriorityUp;

	m_pCurBoundRoute->ExchangePriorityItem(pPriorityDown,pPriorityUp);
	DeletePriorityItem(hItem);
	InitPrioirtyItemContent(pPriorityDown,hItem);

	CString strPriority(_T(""));
	if (pPriorityDown->m_RepConData.m_sName.IsEmpty())
	{
		strPriority.Format(_T("Priority %d"),pPriorityDown->m_RepConData.m_nPriorityIndex + 1);
	}
	else
	{
		strPriority.Format(_T("Priority %d: %s"),pPriorityDown->m_RepConData.m_nPriorityIndex + 1,pPriorityDown->m_RepConData.m_sName);
	}
	m_TreeCtrlRoute.SetItemText(hItem,strPriority);

	DeletePriorityItem(hUpItem);
	InitPrioirtyItemContent(pPriorityUp,hUpItem);

	if (pPriorityUp->m_RepConData.m_sName.IsEmpty())
	{
		strPriority.Format(_T("Priority %d"),pPriorityUp->m_RepConData.m_nPriorityIndex + 1);
	}
	else
	{
		strPriority.Format(_T("Priority %d: %s"),pPriorityUp->m_RepConData.m_nPriorityIndex + 1,pPriorityUp->m_RepConData.m_sName);
	}
	m_TreeCtrlRoute.SetItemText(hUpItem,strPriority);
}

void CDlgBoundRouteAssignment2::OnCmdMoveNextPriority()//down move
{
	HTREEITEM hItem = m_TreeCtrlRoute.GetSelectedItem();
	ASSERT(hItem);
	CTreeData* pNodeData = (CTreeData*)m_TreeCtrlRoute.GetItemData(hItem);
	ASSERT(pNodeData->m_emType == CTreeData::Priority_Item);
	CRoutePriority* pPriorityDown = (CRoutePriority*)pNodeData->m_pData;

	HTREEITEM hDwonItem = m_TreeCtrlRoute.GetNextSiblingItem(hItem);
	ASSERT(hDwonItem);
	CTreeData* pNodeUpData = (CTreeData*)m_TreeCtrlRoute.GetItemData(hDwonItem);
	ASSERT(pNodeUpData->m_emType == CTreeData::Priority_Item);
	CRoutePriority* pPriorityUp = (CRoutePriority*)pNodeUpData->m_pData;

	pNodeData->m_pData = (DWORD)pPriorityUp;
	pNodeUpData->m_pData = (DWORD)pPriorityDown;

	m_pCurBoundRoute->ExchangePriorityItem(pPriorityDown,pPriorityUp);
	DeletePriorityItem(hItem);
	InitPrioirtyItemContent(pPriorityUp,hItem);

	CString strPriority(_T(""));
	if (pPriorityUp->m_RepConData.m_sName.IsEmpty())
	{
		strPriority.Format(_T("Priority %d"),pPriorityUp->m_RepConData.m_nPriorityIndex + 1);
	}
	else
	{
		strPriority.Format(_T("Priority %d: %s"),pPriorityUp->m_RepConData.m_nPriorityIndex + 1,pPriorityUp->m_RepConData.m_sName);
	}
	
	m_TreeCtrlRoute.SetItemText(hItem,strPriority);

	DeletePriorityItem(hDwonItem);
	InitPrioirtyItemContent(pPriorityDown,hDwonItem);

	if (pPriorityDown->m_RepConData.m_sName.IsEmpty())
	{
		strPriority.Format(_T("Priority %d"),pPriorityDown->m_RepConData.m_nPriorityIndex + 1);
	}
	else
	{
		strPriority.Format(_T("Priority %d: %s"),pPriorityDown->m_RepConData.m_nPriorityIndex + 1,pPriorityDown->m_RepConData.m_sName);
	}

	m_TreeCtrlRoute.SetItemText(hDwonItem,strPriority);
}

void CDlgBoundRouteAssignment2::OnCmdDelRoute()
{
	ASSERT(m_pCurBoundRoute);

	HTREEITEM hItem = m_TreeCtrlRoute.GetSelectedItem();
	ASSERT(hItem);

	HTREEITEM hParentItem = m_TreeCtrlRoute.GetParentItem(hItem);
	
	if (hParentItem == NULL)
	{
		CTreeData* pNodeData = (CTreeData*)m_TreeCtrlRoute.GetItemData(hItem);
		if (m_pCurBoundRoute->GetElementCount() == 2)
		{
			HTREEITEM hRootItem = m_TreeCtrlRoute.GetRootItem();
			HTREEITEM hNextItem;
			if (hRootItem != hItem)
			{
				hNextItem = m_TreeCtrlRoute.GetChildItem(hRootItem);
				HTREEITEM hConItem;
				while(hNextItem)
				{
					hConItem = hNextItem;
					hNextItem = m_TreeCtrlRoute.GetNextSiblingItem(hNextItem);
				}
				m_TreeCtrlRoute.DeleteItem(hConItem);
			}
			else
			{
				HTREEITEM hCapItem;
				hNextItem = m_TreeCtrlRoute.GetChildItem(m_TreeCtrlRoute.GetNextSiblingItem(hItem));
				while(hNextItem)
				{
					hCapItem = hNextItem;
					hNextItem = m_TreeCtrlRoute.GetNextSiblingItem(hNextItem);
				}
				m_TreeCtrlRoute.DeleteItem(hCapItem);
			}
		}
		else
		{
			if(pNodeData->m_emType == CTreeData::Priority_Item)
			{
				HTREEITEM hPreItem = m_TreeCtrlRoute.GetPrevSiblingItem(hItem);
				HTREEITEM hSecondItem = m_TreeCtrlRoute.GetNextSiblingItem(hItem);
				HTREEITEM hChildItem;
				HTREEITEM hRecapItem;
				if (!hPreItem && hSecondItem)
				{
					hChildItem = m_TreeCtrlRoute.GetChildItem(hSecondItem);
					while(hChildItem)
					{
						hRecapItem = hChildItem;
						hChildItem = m_TreeCtrlRoute.GetNextSiblingItem(hChildItem);
					}
					m_TreeCtrlRoute.DeleteItem(hRecapItem);
					if (!m_TreeCtrlRoute.GetNextSiblingItem(hSecondItem))
					{
						CTreeData* pSecondNodeData = (CTreeData*)m_TreeCtrlRoute.GetItemData(hSecondItem);
						CRoutePriority* pSecondPriority = (CRoutePriority*)pSecondNodeData->m_pData;

						COOLTREE_NODE_INFO cni;
						CCoolTree::InitNodeInfo(this,cni);
						HTREEITEM hConItem = m_TreeCtrlRoute.InsertItem(_T("Change condition"), cni, FALSE,FALSE,hSecondItem);
						pSecondNodeData = new CTreeData;
						pSecondNodeData->m_emType = CTreeData::Non_Type;
						m_TreeCtrlRoute.SetItemData(hConItem,(DWORD_PTR)pSecondNodeData);
						{
							CString strContent(_T(""));
							cni.nt = NT_NORMAL;
							cni.net = NET_EDIT_INT;
							cni.fMinValue = static_cast<float>(pSecondPriority->m_RepConData.m_nAcOnRouteCount);
							strContent.Format(_T("Number of AC on route >: %d"),pSecondPriority->m_RepConData.m_nAcOnRouteCount);
							pSecondNodeData = new CTreeData;
							pSecondNodeData->m_emType = CTreeData::Count_Item;
							pSecondNodeData->m_pData = (DWORD)pSecondPriority;
							HTREEITEM hCountItem = m_TreeCtrlRoute.InsertItem(strContent, cni, FALSE, FALSE, hConItem);
							m_TreeCtrlRoute.SetItemData(hCountItem, (DWORD)pSecondNodeData);

							strContent.Format(_T("AC on route stopped > (sec): %d"),pSecondPriority->m_RepConData.m_nAcOnRouteStopTime);
							pSecondNodeData = new CTreeData;
							pSecondNodeData->m_emType = CTreeData::Time_Item;
							pSecondNodeData->m_pData = (DWORD)pSecondPriority;
							cni.fMinValue = static_cast<float>(pSecondPriority->m_RepConData.m_nAcOnRouteStopTime);
							HTREEITEM hTimeItem = m_TreeCtrlRoute.InsertItem(strContent, cni, FALSE, FALSE, hConItem);
							m_TreeCtrlRoute.SetItemData(hTimeItem, (DWORD)pSecondNodeData);

							strContent.Format(_T("Preceding aircraft on route slower by: %d%%"),pSecondPriority->m_RepConData.m_nPrecdingAcSlowerPercent);
							pSecondNodeData = new CTreeData;
							pSecondNodeData->m_emType = CTreeData::Preceding_Item;
							pSecondNodeData->m_pData = (DWORD)pSecondPriority;
							cni.fMinValue = static_cast<float>(pSecondPriority->m_RepConData.m_nPrecdingAcSlowerPercent);
							HTREEITEM hPrecedingItem = m_TreeCtrlRoute.InsertItem(strContent, cni, FALSE, FALSE, hConItem);
							m_TreeCtrlRoute.SetItemData(hPrecedingItem, (DWORD)pSecondNodeData);

							m_TreeCtrlRoute.Expand(hConItem,TVE_EXPAND);
						}
					}
				}
				else if(!hSecondItem && hPreItem && m_TreeCtrlRoute.GetPrevSiblingItem(hPreItem))
				{
					hChildItem = m_TreeCtrlRoute.GetChildItem(hPreItem);
					while(hChildItem)
					{
						CTreeData* pChangeNodeData = (CTreeData*)m_TreeCtrlRoute.GetItemData(hChildItem);
						if (pChangeNodeData && pChangeNodeData->m_emType == CTreeData::Non_Type)
						{
							break;
						}
						hChildItem = m_TreeCtrlRoute.GetNextSiblingItem(hChildItem);
					}
					m_TreeCtrlRoute.DeleteItem(hChildItem);
				}
			}
		}
		CRoutePriority* pPriority = (CRoutePriority*)pNodeData->m_pData;
		m_pCurBoundRoute->DeleteItem(pPriority);
		m_pCurBoundRoute->ResetPriorityIndex(pPriority);
		HTREEITEM hNextItem = m_TreeCtrlRoute.GetNextSiblingItem(hItem);
		while(hNextItem)
		{
			CString strPrioirty(_T(""));
			pNodeData = (CTreeData*)m_TreeCtrlRoute.GetItemData(hNextItem);
			CRoutePriority* pNextPriority = (CRoutePriority*)pNodeData->m_pData;
			if (pNextPriority->m_RepConData.m_sName.IsEmpty())
			{
				strPrioirty.Format(_T("Priority %d"),pNextPriority->m_RepConData.m_nPriorityIndex+1);
			}
			else
			{
				strPrioirty.Format(_T("Priority %d: %s"),pNextPriority->m_RepConData.m_nPriorityIndex+1,pNextPriority->m_RepConData.m_sName);
			}
			m_TreeCtrlRoute.SetItemText(hNextItem,strPrioirty);
			hNextItem = m_TreeCtrlRoute.GetNextSiblingItem(hNextItem);
		}
	}
	else
	{
		CTreeData* pNodeData = (CTreeData*)m_TreeCtrlRoute.GetItemData(hParentItem);

		//head item
		if (pNodeData->m_emType == CTreeData::Start_Item)
		{
			CRoutePriority* pPriority = (CRoutePriority*)pNodeData->m_pData;
			pNodeData = (CTreeData*)m_TreeCtrlRoute.GetItemData(hItem);
			CRouteItem *pHeadItem = (CRouteItem *)pNodeData->m_pData;
			ASSERT(pHeadItem);
			pHeadItem->DeleteData();
			pPriority->DeleteItem(pHeadItem);		
		}
		//child item
		else if(pNodeData->m_emType == CTreeData::Route_Item)
		{
			pNodeData = (CTreeData*)m_TreeCtrlRoute.GetItemData(hItem);
			CRouteItem * pOutboundRouteItem = (CRouteItem *)pNodeData->m_pData;
			ASSERT(pOutboundRouteItem);
			pOutboundRouteItem->DeleteData();

			HTREEITEM hParentParentItem = m_TreeCtrlRoute.GetParentItem(hParentItem);
			ASSERT(hParentParentItem);

			pNodeData = (CTreeData*)m_TreeCtrlRoute.GetItemData(hParentItem);
			CRouteItem * pParentItem = (CRouteItem*)pNodeData->m_pData;
			ASSERT(pParentItem);
			pParentItem->DeleteItem(pOutboundRouteItem);
		}


		//delete item in tree
		DeleteChildItemInTree(hItem);
	}

	m_TreeCtrlRoute.DeleteItem(hItem);
	m_pCurBoundRoute->AssignPriorityChangeCondition();
	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);

	UpdateRouteToolBar();
}

void CDlgBoundRouteAssignment2::OnCmdEditRoute()
{
	ASSERT(m_pCurBoundRoute);

	HTREEITEM hItem = m_TreeCtrlRoute.GetSelectedItem();
	ASSERT(hItem);

	HTREEITEM hParentItem = m_TreeCtrlRoute.GetParentItem(hItem);
	ASSERT(hParentItem);

	int nID = -1;
	int nIntersectNodeIDInALTObj = -1;
	CTreeData* pNodeData = (CTreeData*)m_TreeCtrlRoute.GetItemData(hParentItem);

	if(pNodeData->m_emType != CTreeData::Start_Item)
	{
		CRouteItem *pParentItem = (CRouteItem *)pNodeData->m_pData;
		ASSERT(pParentItem);
		nID = pParentItem->GetALTObjectID();
		nIntersectNodeIDInALTObj = pParentItem->GetIntersectNodeID();
	}

	pNodeData = (CTreeData*)m_TreeCtrlRoute.GetItemData(hItem);
	CRouteItem *pOutboundRouteItem = (CRouteItem *)pNodeData->m_pData;
	ASSERT(pOutboundRouteItem);

	int  nSelALTObjID;
	int nIntersectNodeID;
	CString strSelALTObjName;

	nSelALTObjID     = pOutboundRouteItem->GetALTObjectID();
	nIntersectNodeID = pOutboundRouteItem->GetIntersectNodeID();
	strSelALTObjName = pOutboundRouteItem->GetItemName();

	CSelectInboundRouteItemDlg dlg(m_nProjID, nID, nIntersectNodeIDInALTObj);
	dlg.SetSelALTObjID(nSelALTObjID);
	dlg.SetSelALTObjName(strSelALTObjName);
	dlg.SetIntersectNodeID(nIntersectNodeID);

	if (dlg.DoModal() != IDOK)
	{
		return;
	}

	//haven't modify
	if (nSelALTObjID == dlg.GetSelALTObjID()
		&& nIntersectNodeID == dlg.GetIntersectNodeID()
		&& strSelALTObjName == dlg.GetSelALTObjName())
	{
		return;
	}

	nSelALTObjID     = dlg.GetSelALTObjID();
	nIntersectNodeID = dlg.GetIntersectNodeID();
	strSelALTObjName = dlg.GetSelALTObjName();

	m_TreeCtrlRoute.SetItemText(hItem, strSelALTObjName);

	pOutboundRouteItem->SetALTObjectID(nSelALTObjID);
	pOutboundRouteItem->SetIntersectNodeID(nIntersectNodeID);
	pOutboundRouteItem->SetItemName(strSelALTObjName);

	//delete all child 
	pOutboundRouteItem->DeleteData();

	//delete all child in tree
	DeleteChildItemInTree(hItem);

	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
}

void CDlgBoundRouteAssignment2::OnLvnItemchangedOrgDst(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	int nSelItem = m_listOrgDst.GetCurSel();
	UpdateOrgDstToolBar();

	//reset the route tree
	m_TreeCtrlRoute.DeleteAllItems();

	//no selected item
	if (nSelItem < 0
		|| m_listOrgDst.GetItemCount() < (int)m_pCurTimeAssignment->GetElementCount()
		|| m_listOrgDst.GetSelectedCount() < 1)
	{
		UpdateOrgDstToolBar();
		UpdateRouteToolBar();
		return;
	}

	m_pCurBoundRoute = (CIn_OutBoundRoute*)m_listOrgDst.GetItemData(nSelItem);
	UpdateRouteChoice(m_pCurBoundRoute);
	SetTreeContent();

	UpdateOrgDstToolBar();
	UpdateRouteToolBar();

	*pResult = 0;
}

void CDlgBoundRouteAssignment2::SetTreeContent(void)
{
	m_TreeCtrlRoute.DeleteAllItems();

	if (m_listOrgDst.GetItemCount() <= 0)
	{
		return;
	}

	ASSERT(m_pCurBoundRoute);
	BOOL bHasTreeContent = m_pCurBoundRoute->GetBoundRouteChoice() == RouteChoice_Preferred;
	m_TreeCtrlRoute.EnableWindow(bHasTreeContent);
	if (!bHasTreeContent)
		return;

	COOLTREE_NODE_INFO cni;
	CCoolTree::InitNodeInfo(this,cni);


	int nMinPriority = m_pCurBoundRoute->GetMinPriorityIndex();
	int nMaxPriority = m_pCurBoundRoute->GetMaxPriorityIndex();

	for (int i=0; i<(int)m_pCurBoundRoute->GetElementCount(); i++)
	{
		CRoutePriority* pPriority = m_pCurBoundRoute->GetPriorityItem(i);
		if (pPriority == NULL)
		{
			continue;
		}

		CString strPrioirty(_T(""));
		
		if (pPriority->m_RepConData.m_sName.IsEmpty())
		{
			strPrioirty.Format(_T("Priority %d"),pPriority->m_RepConData.m_nPriorityIndex + 1);
		}
		else
		{
			strPrioirty.Format(_T("Priority %d: %s"),pPriority->m_RepConData.m_nPriorityIndex + 1,pPriority->m_RepConData.m_sName);
		}
		
		cni.nt = NT_NORMAL;
		cni.net = NET_LABLE;
		CTreeData* pNodeData = new CTreeData;
		pNodeData->m_emType = CTreeData::Priority_Item;
		pNodeData->m_pData = (DWORD)pPriority;
		HTREEITEM hPriorityItem = m_TreeCtrlRoute.InsertItem(strPrioirty, cni, FALSE);
		m_TreeCtrlRoute.SetItemData(hPriorityItem,(DWORD_PTR)pNodeData);
		

		cni.nt = NT_NORMAL;
		cni.net = NET_NORMAL;
		HTREEITEM hStartItem = m_TreeCtrlRoute.InsertItem(_T("Start"), cni, FALSE,FALSE,hPriorityItem);
		pNodeData = new CTreeData;
		pNodeData->m_emType = CTreeData::Start_Item;
		pNodeData->m_pData = (DWORD)pPriority;
		m_TreeCtrlRoute.SetItemData(hStartItem,(DWORD_PTR)pNodeData);

		int nChildItemCount = pPriority->GetElementCount();
		for (int j=0; j<nChildItemCount; j++)
		{
			CRouteItem *pChildItem = pPriority->GetItem(j);
			ASSERT(pChildItem);

			cni.nt = NT_NORMAL;
			cni.net = NET_NORMAL;
			pNodeData = new CTreeData;
			pNodeData->m_emType = CTreeData::Route_Item;
			pNodeData->m_pData = (DWORD)pChildItem;
			HTREEITEM hChildItem = m_TreeCtrlRoute.InsertItem(pChildItem->GetItemName(), cni, FALSE, FALSE, hStartItem);
			m_TreeCtrlRoute.SetItemData(hChildItem, (DWORD)pNodeData);

			SetChildItemContent(hChildItem, pChildItem);
		}
		
		m_TreeCtrlRoute.Expand(hStartItem,TVE_EXPAND);

		if ((pPriority->m_RepConData.m_nPriorityIndex == 0 && nMinPriority != nMaxPriority)||\
			pPriority->m_RepConData.m_nPriorityIndex != nMaxPriority)
		{
			cni.nt = NT_NORMAL;
			cni.net = NET_NORMAL;
			HTREEITEM hConItem = m_TreeCtrlRoute.InsertItem(_T("Change condition"), cni, FALSE,FALSE,hPriorityItem);
			pNodeData = new CTreeData;
			pNodeData->m_emType = CTreeData::Non_Type;
			m_TreeCtrlRoute.SetItemData(hConItem,(DWORD_PTR)pNodeData);
			{
				cni.nt = NT_NORMAL;
				cni.net = NET_EDIT_INT;
				CString strContent(_T(""));
				strContent.Format(_T("Number of AC on route >: %d"),pPriority->m_RepConData.m_nAcOnRouteCount);
				pNodeData = new CTreeData;
				pNodeData->m_emType = CTreeData::Count_Item;
				pNodeData->m_pData = (DWORD)pPriority;
				cni.fMinValue = static_cast<float>(pPriority->m_RepConData.m_nAcOnRouteCount);
				HTREEITEM hCountItem = m_TreeCtrlRoute.InsertItem(strContent, cni, FALSE, FALSE, hConItem);
				m_TreeCtrlRoute.SetItemData(hCountItem, (DWORD)pNodeData);

				cni.nt = NT_NORMAL;
				cni.net = NET_EDIT_INT;
				strContent.Format(_T("AC on route stopped > (sec): %d"),pPriority->m_RepConData.m_nAcOnRouteStopTime);
				pNodeData = new CTreeData;
				pNodeData->m_emType = CTreeData::Time_Item;
				pNodeData->m_pData = (DWORD)pPriority;
				cni.fMinValue = static_cast<float>(pPriority->m_RepConData.m_nAcOnRouteStopTime);
				HTREEITEM hTimeItem = m_TreeCtrlRoute.InsertItem(strContent, cni, FALSE, FALSE, hConItem);
				m_TreeCtrlRoute.SetItemData(hTimeItem, (DWORD)pNodeData);

				cni.nt = NT_NORMAL;
				cni.net = NET_EDIT_INT;
				strContent.Format(_T("Preceding aircraft on route slower by: %d%%"),pPriority->m_RepConData.m_nPrecdingAcSlowerPercent);
				pNodeData = new CTreeData;
				pNodeData->m_emType = CTreeData::Preceding_Item;
				pNodeData->m_pData = (DWORD)pPriority;
				cni.fMinValue = static_cast<float>(pPriority->m_RepConData.m_nPrecdingAcSlowerPercent);
				HTREEITEM hPrecedingItem = m_TreeCtrlRoute.InsertItem(strContent, cni, FALSE, FALSE, hConItem);
				m_TreeCtrlRoute.SetItemData(hPrecedingItem, (DWORD)pNodeData);

				m_TreeCtrlRoute.Expand(hConItem,TVE_EXPAND);
			}
		}

		if (pPriority->m_RepConData.m_nPriorityIndex != 0)
		{
			cni.nt = NT_NORMAL;
			cni.net = NET_COMBOBOX;
			CString strRecature(_T(""));
			strRecature.Format(_T("Recapture original when possible: %s"),szBuff[pPriority->m_RepConData.m_bRecapture?1:0]);
			pNodeData = new CTreeData;
			pNodeData->m_emType = CTreeData::Recapture_Item;
			pNodeData->m_pData = (DWORD)pPriority;
			HTREEITEM hPrecedingItem = m_TreeCtrlRoute.InsertItem(strRecature, cni, FALSE, FALSE, hPriorityItem);
			m_TreeCtrlRoute.SetItemData(hPrecedingItem, (DWORD)pNodeData);
		}

		m_TreeCtrlRoute.Expand(hPriorityItem, TVE_EXPAND);
	}
	m_TreeCtrlRoute.SelectItem(NULL);
}

void CDlgBoundRouteAssignment2::SetChildItemContent(HTREEITEM hParentItem, CRouteItem *pOutboundRouteItem)
{
	ASSERT(hParentItem);
	ASSERT(pOutboundRouteItem);

	COOLTREE_NODE_INFO cni;
	CCoolTree::InitNodeInfo(this,cni);

	int nChildCount = pOutboundRouteItem->GetElementCount();
	for (int i=0; i<nChildCount; i++)
	{
		CRouteItem *pChildItem = pOutboundRouteItem->GetItem(i);
		ASSERT(pChildItem);

		cni.nt = NT_NORMAL;
		cni.net = NET_NORMAL;
		CTreeData* pNodeData = new CTreeData;
		pNodeData->m_emType = CTreeData::Route_Item;
		pNodeData->m_pData = (DWORD)pChildItem;
		HTREEITEM hNewItem = m_TreeCtrlRoute.InsertItem(pChildItem->GetItemName(), cni, FALSE, FALSE, hParentItem);
		m_TreeCtrlRoute.SetItemData(hNewItem, (DWORD)pNodeData);

		SetChildItemContent(hNewItem, pChildItem);
	}

	m_TreeCtrlRoute.Expand(hParentItem, TVE_EXPAND);
}

void CDlgBoundRouteAssignment2::DeleteChildItemInTree(HTREEITEM hItem)
{
	ASSERT(hItem);

	HTREEITEM hChildItem = m_TreeCtrlRoute.GetChildItem(hItem);

	while (hChildItem != NULL)
	{
		DeleteChildItemInTree(hChildItem);

		m_TreeCtrlRoute.DeleteItem(hChildItem);

		hChildItem = m_TreeCtrlRoute.GetChildItem(hItem);
	}
}

void CDlgBoundRouteAssignment2::OnTvnSelchangedTreeRoute(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here

	UpdateRouteToolBar();

	*pResult = 0;
}

void CDlgBoundRouteAssignment2::OnBnClickedButtonSave()
{
	try
	{
		CADODatabase::BeginTransaction();
		m_pBoundRouteAssignment->SaveData(m_nProjID);
		CADODatabase::CommitTransaction();

	}
	catch (CADOException &e)
	{
		CADODatabase::RollBackTransation();
		e.ErrorMessage();
		MessageBox(_T("Save error!"), _T("Error"), MB_OK|MB_ICONEXCLAMATION );
		return;
	}

	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(FALSE);
}

void CDlgBoundRouteAssignment2::OnOK()
{
	// TODO: Add your control notification handler code here
	OnBnClickedButtonSave();
	CXTResizeDialogEx::OnOK();
}


void CDlgBoundRouteAssignment2::OnCmdNewFlightTime()
{
	if (m_pSelectFlightType == NULL)
		return;

	FlightConstraint fltType;
	if( !(*m_pSelectFlightType)(NULL,fltType) )
		return;

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

	CFlightTypeRouteAssignment* pAssignment = new CFlightTypeRouteAssignment;

	pAssignment->SetAirportDB(m_pAirportDB);
	pAssignment->SetFltType(szBuffer);
	pAssignment->SetRouteType(m_pBoundRouteAssignment->GetRouteType());
	m_wndTreeFltTime.SetItemData(hItem,(DWORD_PTR)pAssignment);
	m_pBoundRouteAssignment->AddNewItem(pAssignment);
	m_wndTreeFltTime.SetRedraw(TRUE);
}

void CDlgBoundRouteAssignment2::OnCmdEditFlightTime()
{
	HTREEITEM hSelItem = m_wndTreeFltTime.GetSelectedItem();

	if (hSelItem == NULL)
		return;

	if ( m_wndTreeFltTime.GetParentItem(hSelItem) == NULL)
	{
		CFlightTypeRouteAssignment* pAssignment = (CFlightTypeRouteAssignment*)m_wndTreeFltTime.GetItemData(hSelItem);

		FlightConstraint fltType;
		if(!(*m_pSelectFlightType)(NULL,fltType))
			return;

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
		pAssignment->SetFltType(szBuffer);

	}
	else
	{
		OnEditTimeRange();
	}
	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
}

void CDlgBoundRouteAssignment2::OnCmdDelFlightTime()
{
	m_wndTreeFltTime.SetRedraw(FALSE);
	HTREEITEM hSelItem = m_wndTreeFltTime.GetSelectedItem();

	if (hSelItem == NULL)
		return;

	if ( m_wndTreeFltTime.GetParentItem(hSelItem) == NULL)
	{
		CFlightTypeRouteAssignment* pAssignment = (CFlightTypeRouteAssignment*)m_wndTreeFltTime.GetItemData(hSelItem);
		m_pBoundRouteAssignment->DeleteItem(pAssignment);
		m_wndTreeFltTime.DeleteItem(hSelItem);
	}
	else
	{
		OnDelTimeRange();
	}
	m_wndTreeFltTime.SetRedraw(TRUE);
	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
}

void CDlgBoundRouteAssignment2::OnAddTimeRange()
{
	HTREEITEM hSelItem = m_wndTreeFltTime.GetSelectedItem();
	if (hSelItem == NULL)
		return;

	if ( m_wndTreeFltTime.GetParentItem(hSelItem) == NULL)
	{
		CFlightTypeRouteAssignment* pAssignment = (CFlightTypeRouteAssignment*)m_wndTreeFltTime.GetItemData(hSelItem);

		ElapsedTime estFromTime;
		ElapsedTime estToTime;

		CDlgTimeRange dlg(estFromTime, estToTime);

		if(IDOK!= dlg.DoModal())
			return;

		CString strTimeRange;
		strTimeRange.Format("%s - %s", dlg.GetStartTimeString(), dlg.GetEndTimeString());

		estFromTime = dlg.GetStartTime();
		estToTime = dlg.GetEndTime();


		CTimeRangeRouteAssignment *pItem = new CTimeRangeRouteAssignment();
		pItem->SetRouteType(pAssignment->GetRouteType());
		pItem->SetStartTime(estFromTime);
		pItem->SetEndTime(estToTime);
		pAssignment->AddNewItem(pItem);

		HTREEITEM hChildTreeItem = m_wndTreeFltTime.InsertItem(strTimeRange, hSelItem, TVI_LAST);
		m_wndTreeFltTime.SetItemData(hChildTreeItem, (DWORD_PTR)(pItem));
		m_wndTreeFltTime.Expand(hSelItem, TVE_EXPAND);
		GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);

	}

}

void CDlgBoundRouteAssignment2::OnEditTimeRange()
{
	HTREEITEM hSelItem = m_wndTreeFltTime.GetSelectedItem();
	if (hSelItem == NULL)
		return;

	if ( m_wndTreeFltTime.GetParentItem(hSelItem) != NULL)
	{
		CTimeRangeRouteAssignment* pItem = (CTimeRangeRouteAssignment*)m_wndTreeFltTime.GetItemData(hSelItem);
		CDlgTimeRange dlg(pItem->GetStartTime(), pItem->GetEndTime());
		if(IDOK!= dlg.DoModal())
			return;

		ElapsedTime tStartTime = dlg.GetStartTime();
		ElapsedTime tEndTime = dlg.GetEndTime();

		CString strTimeRange;
		strTimeRange.Format("%s - %s", dlg.GetStartTimeString(), dlg.GetEndTimeString());


		pItem->SetStartTime(tStartTime);
		pItem->SetEndTime(tEndTime);
		m_wndTreeFltTime.SetItemText(hSelItem, strTimeRange);
		GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);

	}
}

void CDlgBoundRouteAssignment2::OnDelTimeRange()
{
	HTREEITEM hSelItem = m_wndTreeFltTime.GetSelectedItem();
	if (hSelItem == NULL)
		return;

	HTREEITEM hFlightItem = m_wndTreeFltTime.GetParentItem(hSelItem);

	if ( hFlightItem != NULL)
	{
		CTimeRangeRouteAssignment* pItem = (CTimeRangeRouteAssignment*)m_wndTreeFltTime.GetItemData(hSelItem);
		if(pItem == NULL)
			return;
		
		CFlightTypeRouteAssignment* pFlightItem = (CFlightTypeRouteAssignment*)m_wndTreeFltTime.GetItemData(hFlightItem);
		pFlightItem->DeleteItem(pItem);
		m_wndTreeFltTime.DeleteItem(hSelItem);

		hSelItem = m_wndTreeFltTime.GetSelectedItem();
		m_pCurTimeAssignment = (CTimeRangeRouteAssignment*)m_wndTreeFltTime.GetItemData(hSelItem);

		GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);

	}
}

void CDlgBoundRouteAssignment2::OnContextMenu(CWnd* pWnd, CPoint point)
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

	if(m_wndTreeFltTime.GetParentItem(hRClickItem) != NULL)
	{
		pMenu->EnableMenuItem( ID_ADD_TIMERANGE, MF_GRAYED );
	}
	else
	{
		pMenu->EnableMenuItem( ID_EDIT_TIMERANGE, MF_GRAYED );
		pMenu->EnableMenuItem( ID_DEL_TIMERANGE, MF_GRAYED );
	}


	if (pMenu)
		pMenu->TrackPopupMenu(TPM_LEFTALIGN, point.x, point.y, this);	
}

void CDlgBoundRouteAssignment2::OnTvnSelchangedTreeFlighttypeTime(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	HTREEITEM hItem = m_wndTreeFltTime.GetSelectedItem() ;
	if(hItem ==NULL)
		return;

	m_ToolBarFlightType.GetToolBarCtrl().EnableButton( ID_DEL_FLIGHTTY, TRUE );
	m_ToolBarFlightType.GetToolBarCtrl().EnableButton( ID_EDIT_FLIGHTTY, TRUE );

	if(m_wndTreeFltTime.GetParentItem(hItem) == NULL)
	{
		m_listOrgDst.DeleteAllItems();
		m_listOrgDst.EnableWindow(FALSE);
		m_btnShortestPath.EnableWindow(FALSE);
		m_btnPreferredRoute.EnableWindow(FALSE);
		m_TreeCtrlRoute.DeleteAllItems();
		m_TreeCtrlRoute.EnableWindow(FALSE);

		UpdateRouteChoiceBtns();

		UpdateOrgDstToolBar();
		UpdateRouteToolBar();
	}
	else
	{
		m_pCurTimeAssignment = (CTimeRangeRouteAssignment*)m_wndTreeFltTime.GetItemData(hItem);
		InitInboundRouteAssignmentState();
	}

	*pResult = 0;
}

void CDlgBoundRouteAssignment2::InitInboundRouteAssignmentState()
{
	HTREEITEM hSelItem = m_wndTreeFltTime.GetSelectedItem();
	if (hSelItem == NULL || m_wndTreeFltTime.GetParentItem(hSelItem) == NULL)
		return;

	CTimeRangeRouteAssignment* pTimeAssignment = (CTimeRangeRouteAssignment*)m_wndTreeFltTime.GetItemData(hSelItem);
	m_listOrgDst.EnableWindow(TRUE);

	SetOrgDstListContent();	

	UpdateOrgDstToolBar();
	UpdateRouteToolBar();
}

LRESULT CDlgBoundRouteAssignment2::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
	case UM_CEW_LABLE_BEGIN:
		{
			COOLTREE_NODE_INFO* pCNI = (COOLTREE_NODE_INFO*)lParam;
			if (pCNI && pCNI->pEditWnd)
			{
				HTREEITEM hItem = (HTREEITEM)wParam;
				CTreeData* pNodeData = (CTreeData*)m_TreeCtrlRoute.GetItemData(hItem);
				if (pNodeData)
				{
					CRoutePriority* pPriority = (CRoutePriority*)pNodeData->m_pData;
					if (pPriority)
					{
						CEdit* pEdit = (CEdit*)pCNI->pEditWnd;
						pEdit->SetWindowText(pPriority->m_RepConData.m_sName);
						pEdit->SetSel(0, -1);
					}
				}
			}
		}
		break;
	case UM_CEW_LABLE_END:
		{
			HTREEITEM hItem = (HTREEITEM)wParam;
			CString strValue = *((CString*)lParam);
			CTreeData* pNodeData = (CTreeData*)m_TreeCtrlRoute.GetItemData(hItem);
			if (pNodeData)
			{
				CRoutePriority* pPriority = (CRoutePriority*)pNodeData->m_pData;
	
				pPriority->m_RepConData.m_sName = strValue;
				CString strText(_T(""));
				if (strValue.IsEmpty())
				{
					strValue.Format(_T("Priority %d"),pPriority->m_RepConData.m_nPriorityIndex + 1);
				}
				else
				{
					strValue.Format(_T("Priority %d: %s"),pPriority->m_RepConData.m_nPriorityIndex +1,strValue);
				}
				m_TreeCtrlRoute.SetItemText(hItem,strValue);
			}
		}
		break;
	case UM_CEW_EDITSPIN_END:
		{
			HTREEITEM hItem = (HTREEITEM)wParam;
			CString strValue = *((CString*)lParam);
			CTreeData* pNodeData = (CTreeData*)m_TreeCtrlRoute.GetItemData(hItem);
			CRoutePriority* pPriority = (CRoutePriority*)pNodeData->m_pData;
			COOLTREE_NODE_INFO* cni = m_TreeCtrlRoute.GetItemNodeInfo(hItem);
			CString strData = _T("");
			switch(pNodeData->m_emType)
			{
			case CTreeData::Count_Item:
				{
					pPriority->m_RepConData.m_nAcOnRouteCount = atoi(strValue);
					strData.Format(_T("Number of AC on route >: %s"),strValue);
				}
				break;
			case CTreeData::Time_Item:
				{
					pPriority->m_RepConData.m_nAcOnRouteStopTime = atoi(strValue);
					strData.Format(_T("AC on route stopped > (sec): %s"),strValue);
				}
				break;
			case CTreeData::Preceding_Item:
				{
					pPriority->m_RepConData.m_nPrecdingAcSlowerPercent = atoi(strValue);
					strData.Format(_T("Preceding aircraft on route slower by: %s%%"),strValue);
				}
				break;
			default:
				break;
			}
			cni->fMinValue = static_cast<float>(atoi(strValue));
			m_TreeCtrlRoute.SetItemText(hItem,strData);
			GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
		}
		break;
	case UM_CEW_COMBOBOX_BEGIN:
		{
			CRect rectWnd;
			HTREEITEM hItem = (HTREEITEM)wParam;
			CWnd* pWnd=m_TreeCtrlRoute.GetEditWnd(hItem);
			m_TreeCtrlRoute.GetItemRect(hItem,rectWnd,TRUE);
			pWnd->SetWindowPos(NULL,rectWnd.right,rectWnd.top,0,0,SWP_NOSIZE);
			CComboBox* pCB=(CComboBox*)pWnd;
			pCB->ResetContent();
			if (pCB->GetCount() == 0)
			{
				pCB->AddString(szBuff[0]);
				pCB->AddString(szBuff[1]);
			}
		}
		break;

	case UM_CEW_COMBOBOX_SELCHANGE:
		{
			HTREEITEM hItem = (HTREEITEM)wParam;
			int nSel = m_TreeCtrlRoute.GetCmbBoxCurSel(hItem);
			CTreeData* pNodeData = (CTreeData*)m_TreeCtrlRoute.GetItemData(hItem);
			CString strValue = _T("");
			CRoutePriority* pPriority = (CRoutePriority*)pNodeData->m_pData;
			pPriority->m_RepConData.m_bRecapture = nSel?true:false;
			strValue.Format(_T("Recapture original when possible: %s"),szBuff[nSel]);
			m_TreeCtrlRoute.SetItemText(hItem,strValue);
			GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
		}
		break;
	case WM_NOTIFY:
		{
			if (wParam == ID_WND_SPLITTER_VER || wParam == ID_WND_SPLITTER_HOR)
			{
				SPC_NMHDR* pHdr = (SPC_NMHDR*)lParam;
				DoResize(pHdr->delta, wParam);
			}
		}
		break;
	default:
		break;
	}
	 return CXTResizeDialogEx::DefWindowProc(message,wParam,lParam);
}

void CDlgBoundRouteAssignment2::DoResize(int delta,UINT nIDSplitter)
{
	if(nIDSplitter == ID_WND_SPLITTER_VER)
	{
		CSplitterControl::ChangeWidth(GetDlgItem(IDC_STATIC_FLIGHTTYPE), delta, CW_LEFTALIGN);
//  	CSplitterControl::ChangeWidth(&m_ToolBarFlightType, delta, CW_LEFTALIGN);
 		CSplitterControl::ChangeWidth(&m_wndTreeFltTime, delta,CW_LEFTALIGN);

 		CSplitterControl::ChangeWidth(GetDlgItem(IDC_STATIC_ORIGDEST),-delta,CW_RIGHTALIGN);
 		CSplitterControl::ChangePos(&m_toolbarOrgDst,-delta,0);
 		CSplitterControl::ChangeWidth(&m_listOrgDst,-delta,CW_RIGHTALIGN);

		CSplitterControl::ChangeWidth(&m_wndSplitterHor,-delta,CW_RIGHTALIGN);

		CSplitterControl::ChangeWidth(GetDlgItem(IDC_STATIC_ROUTECHOICE),-delta,CW_RIGHTALIGN);
		CSplitterControl::ChangePos(&m_btnShortestPath,-delta,0);
		CSplitterControl::ChangePos(&m_btnPreferredRoute,-delta,0);

		CSplitterControl::ChangeWidth(GetDlgItem(IDC_STATIC_ROUTEASSIGNMENT),-delta,CW_RIGHTALIGN);
		CSplitterControl::ChangePos(&m_ToolBarRoute,-delta,0);
 		CSplitterControl::ChangeWidth(&m_TreeCtrlRoute,-delta,CW_RIGHTALIGN);

		SetAllControlsResize();
	}
	else if (nIDSplitter == ID_WND_SPLITTER_HOR)
	{
		CSplitterControl::ChangeHeight(GetDlgItem(IDC_STATIC_ORIGDEST), delta, CW_TOPALIGN);
// 		CSplitterControl::ChangeHeight(&m_toolbarOrgDst, delta, CW_BOTTOMALIGN);
		CSplitterControl::ChangeHeight(&m_listOrgDst, delta, CW_TOPALIGN);

		CSplitterControl::ChangeHeight(GetDlgItem(IDC_STATIC_ROUTECHOICE), -delta, CW_BOTTOMALIGN);
		CSplitterControl::ChangePos(&m_btnShortestPath, 0, delta);
		CSplitterControl::ChangePos(&m_btnPreferredRoute, 0, delta);

		CSplitterControl::ChangeHeight(GetDlgItem(IDC_STATIC_ROUTEASSIGNMENT), -delta, CW_BOTTOMALIGN);
		CSplitterControl::ChangePos(&m_ToolBarRoute, 0, delta);
		CSplitterControl::ChangeHeight(&m_TreeCtrlRoute, -delta, CW_BOTTOMALIGN);

		SetAllControlsResize();
	}
}

void CDlgBoundRouteAssignment2::DeletePriorityItem(HTREEITEM hItem)
{
	ASSERT(hItem);
	HTREEITEM hChildItem = m_TreeCtrlRoute.GetChildItem(hItem);
	while(hChildItem)
	{
		m_TreeCtrlRoute.DeleteItem(hChildItem);
		hChildItem = m_TreeCtrlRoute.GetChildItem(hItem);
	}
}

void CDlgBoundRouteAssignment2::InitPrioirtyItemContent(CRoutePriority*pPriority,HTREEITEM hPriorityItem)
{
	ASSERT(pPriority);

	COOLTREE_NODE_INFO cni;
	CCoolTree::InitNodeInfo(this,cni);

	HTREEITEM hStartItem = m_TreeCtrlRoute.InsertItem(_T("Start"), cni, FALSE,FALSE,hPriorityItem);
	CTreeData* pNodeData = new CTreeData;
	pNodeData->m_emType = CTreeData::Start_Item;
	pNodeData->m_pData = (DWORD)pPriority;
	m_TreeCtrlRoute.SetItemData(hStartItem,(DWORD_PTR)pNodeData);

	int nChildItemCount = pPriority->GetElementCount();
	for (int i=0; i<nChildItemCount; i++)
	{
		CRouteItem *pChildItem = pPriority->GetItem(i);
		ASSERT(pChildItem);

		cni.nt = NT_NORMAL;
		cni.net = NET_NORMAL;
		pNodeData = new CTreeData;
		pNodeData->m_emType = CTreeData::Route_Item;
		pNodeData->m_pData = (DWORD)pChildItem;
		HTREEITEM hChildItem = m_TreeCtrlRoute.InsertItem(pChildItem->GetItemName(), cni, FALSE, FALSE, hStartItem);
		m_TreeCtrlRoute.SetItemData(hChildItem, (DWORD)pNodeData);

		SetChildItemContent(hChildItem, pChildItem);
	}

	m_TreeCtrlRoute.Expand(hStartItem,TVE_EXPAND);

	if (m_pCurBoundRoute)
	{
		int nMinPriority = m_pCurBoundRoute->GetMinPriorityIndex();
		int nMaxPriority = m_pCurBoundRoute->GetMaxPriorityIndex();

		if ((pPriority->m_RepConData.m_nPriorityIndex == 0 && nMinPriority != nMaxPriority)||\
			pPriority->m_RepConData.m_nPriorityIndex != nMaxPriority)
		{
			HTREEITEM hConItem = m_TreeCtrlRoute.InsertItem(_T("Change condition"), cni, FALSE,FALSE,hPriorityItem);
			pNodeData = new CTreeData;
			pNodeData->m_emType = CTreeData::Non_Type;
			m_TreeCtrlRoute.SetItemData(hConItem,(DWORD_PTR)pNodeData);
			{
				CString strContent(_T(""));
				cni.nt = NT_NORMAL;
				cni.net = NET_EDIT_INT;
				cni.fMinValue = static_cast<float>(pPriority->m_RepConData.m_nAcOnRouteCount);
				strContent.Format(_T("Number of AC on route >: %d"),pPriority->m_RepConData.m_nAcOnRouteCount);
				pNodeData = new CTreeData;
				pNodeData->m_emType = CTreeData::Count_Item;
				pNodeData->m_pData = (DWORD)pPriority;
				HTREEITEM hCountItem = m_TreeCtrlRoute.InsertItem(strContent, cni, FALSE, FALSE, hConItem);
				m_TreeCtrlRoute.SetItemData(hCountItem, (DWORD)pNodeData);

				strContent.Format(_T("AC on route stopped > (sec): %d"),pPriority->m_RepConData.m_nAcOnRouteStopTime);
				pNodeData = new CTreeData;
				pNodeData->m_emType = CTreeData::Time_Item;
				pNodeData->m_pData = (DWORD)pPriority;
				cni.fMinValue = static_cast<float>(pPriority->m_RepConData.m_nAcOnRouteStopTime);
				HTREEITEM hTimeItem = m_TreeCtrlRoute.InsertItem(strContent, cni, FALSE, FALSE, hConItem);
				m_TreeCtrlRoute.SetItemData(hTimeItem, (DWORD)pNodeData);

				strContent.Format(_T("Preceding aircraft on route slower by: %d%%"),pPriority->m_RepConData.m_nPrecdingAcSlowerPercent);
				pNodeData = new CTreeData;
				pNodeData->m_emType = CTreeData::Preceding_Item;
				pNodeData->m_pData = (DWORD)pPriority;
				cni.fMinValue = static_cast<float>(pPriority->m_RepConData.m_nPrecdingAcSlowerPercent);
				HTREEITEM hPrecedingItem = m_TreeCtrlRoute.InsertItem(strContent, cni, FALSE, FALSE, hConItem);
				m_TreeCtrlRoute.SetItemData(hPrecedingItem, (DWORD)pNodeData);

				m_TreeCtrlRoute.Expand(hConItem,TVE_EXPAND);
			}
		}
	}
	
	

	if (pPriority->m_RepConData.m_nPriorityIndex != 0)
	{
		CString strRecature(_T(""));
		strRecature.Format(_T("Recapture original when possible: %s"),szBuff[pPriority->m_RepConData.m_bRecapture?1:0]);
		pNodeData = new CTreeData;
		pNodeData->m_emType = CTreeData::Recapture_Item;
		pNodeData->m_pData = (DWORD)pPriority;
		cni.nt = NT_NORMAL;
		cni.net = NET_COMBOBOX;
		HTREEITEM hPrecedingItem = m_TreeCtrlRoute.InsertItem(strRecature, cni, FALSE, FALSE, hPriorityItem);
		m_TreeCtrlRoute.SetItemData(hPrecedingItem, (DWORD)pNodeData);
	}
	

	UpdateRouteToolBar();
}

void CDlgBoundRouteAssignment2::InitPrioirtyItemContent()
{
	COOLTREE_NODE_INFO cni;
	CCoolTree::InitNodeInfo(this,cni);

	cni.nt = NT_NORMAL;
	cni.net = NET_LABLE;
	CRoutePriority *pPriority = new CRoutePriority;
	pPriority->m_RepConData.m_nPriorityIndex = m_pCurBoundRoute->GetElementCount();
	CString strPrioirty(_T(""));
	strPrioirty.Format(_T("Priority %d"),m_pCurBoundRoute->GetElementCount()+1);
	CTreeData* pNodeData = new CTreeData;
	pNodeData->m_emType = CTreeData::Priority_Item;
	pNodeData->m_pData = (DWORD)pPriority;
	HTREEITEM hPriorityItem = m_TreeCtrlRoute.InsertItem(strPrioirty, cni, FALSE);
	m_TreeCtrlRoute.SetItemData(hPriorityItem,(DWORD_PTR)pNodeData);

	m_pCurBoundRoute->AddNewItem(pPriority);
	m_pCurBoundRoute->AssignPriorityChangeCondition();
	InitPrioirtyItemContent(pPriority,hPriorityItem);

	m_TreeCtrlRoute.Expand(hPriorityItem,TVE_EXPAND);
}

void CDlgBoundRouteAssignment2::InitChangeConditionItemContent(HTREEITEM hItem)
{
	CTreeData* pNodeData = NULL;
	HTREEITEM hSelItem = m_TreeCtrlRoute.GetSelectedItem();

	COOLTREE_NODE_INFO cni;
	CCoolTree::InitNodeInfo(this,cni);
	pNodeData = (CTreeData*)m_TreeCtrlRoute.GetItemData(hItem);
	if (pNodeData->m_emType == CTreeData::Priority_Item)
	{
		CRoutePriority* pPriority = (CRoutePriority*)pNodeData->m_pData;
		HTREEITEM hConItem = m_TreeCtrlRoute.InsertItem(_T("Change condition"), cni, FALSE,FALSE,hItem,m_TreeCtrlRoute.GetChildItem(hItem));
		pNodeData = new CTreeData;
		pNodeData->m_emType = CTreeData::Non_Type;
		m_TreeCtrlRoute.SetItemData(hConItem,(DWORD_PTR)pNodeData);
		{
			CString strContent(_T(""));
			cni.nt = NT_NORMAL;
			cni.net = NET_EDIT_INT;
			cni.fMinValue = static_cast<float>(pPriority->m_RepConData.m_nAcOnRouteCount);
			strContent.Format(_T("Number of AC on route >: %d"),pPriority->m_RepConData.m_nAcOnRouteCount);
			pNodeData = new CTreeData;
			pNodeData->m_emType = CTreeData::Count_Item;
			pNodeData->m_pData = (DWORD)pPriority;
			HTREEITEM hCountItem = m_TreeCtrlRoute.InsertItem(strContent, cni, FALSE, FALSE, hConItem);
			m_TreeCtrlRoute.SetItemData(hCountItem, (DWORD)pNodeData);

			strContent.Format(_T("AC on route stopped > (sec): %d"),pPriority->m_RepConData.m_nAcOnRouteStopTime);
			pNodeData = new CTreeData;
			pNodeData->m_emType = CTreeData::Time_Item;
			pNodeData->m_pData = (DWORD)pPriority;
			cni.fMinValue = static_cast<float>(pPriority->m_RepConData.m_nAcOnRouteStopTime);
			HTREEITEM hTimeItem = m_TreeCtrlRoute.InsertItem(strContent, cni, FALSE, FALSE, hConItem);
			m_TreeCtrlRoute.SetItemData(hTimeItem, (DWORD)pNodeData);

			strContent.Format(_T("Preceding aircraft on route slower by: %d%%"),pPriority->m_RepConData.m_nPrecdingAcSlowerPercent);
			pNodeData = new CTreeData;
			pNodeData->m_emType = CTreeData::Preceding_Item;
			pNodeData->m_pData = (DWORD)pPriority;
			cni.fMinValue = static_cast<float>(pPriority->m_RepConData.m_nPrecdingAcSlowerPercent);
			HTREEITEM hPrecedingItem = m_TreeCtrlRoute.InsertItem(strContent, cni, FALSE, FALSE, hConItem);
			m_TreeCtrlRoute.SetItemData(hPrecedingItem, (DWORD)pNodeData);

			m_TreeCtrlRoute.Expand(hConItem,TVE_EXPAND);
		}
	}
}

void CDlgBoundRouteAssignment2::InitChangeConditionContent()
{
	HTREEITEM hSelItem = m_TreeCtrlRoute.GetSelectedItem();

	HTREEITEM hChildItem = m_TreeCtrlRoute.GetRootItem();
	HTREEITEM hItem = hChildItem;
	while(hChildItem)
	{
		hItem = hChildItem;
		hChildItem = m_TreeCtrlRoute.GetNextSiblingItem(hChildItem);
	}

	if (hItem && ((m_TreeCtrlRoute.GetPrevSiblingItem(hItem) && !m_TreeCtrlRoute.GetParentItem(hSelItem))||
		m_pCurBoundRoute->GetElementCount() == 1))
	{
		InitChangeConditionItemContent(hItem);
	}
}

void CDlgBoundRouteAssignment2::OnCmdNewRoute()
{
	ASSERT(m_pCurBoundRoute);
	HTREEITEM hSelItem = m_TreeCtrlRoute.GetSelectedItem();
	CTreeData* pNodeData = NULL;
	
	if (NULL == hSelItem)
	{
		InitChangeConditionContent();
		InitPrioirtyItemContent();
		return;
	}
	else
	{
		pNodeData = (CTreeData*)m_TreeCtrlRoute.GetItemData(hSelItem);
		if (pNodeData->m_emType == CTreeData::Priority_Item)
		{
			InitChangeConditionContent();
			InitPrioirtyItemContent();
			return;
		}
	}
	

	if (pNodeData && (pNodeData->m_emType == CTreeData::Route_Item || pNodeData->m_emType == CTreeData::Start_Item))
	{
		int nID = -1;
		int nIntersectNodeIDInALTObj = -1;
	
		if(pNodeData->m_emType == CTreeData::Route_Item)
		{
			CRouteItem* pParentItem = (CRouteItem*)pNodeData->m_pData;
			ASSERT(pParentItem);
			nID = pParentItem->GetALTObjectID();
			nIntersectNodeIDInALTObj = pParentItem->GetIntersectNodeID();
		}

		CSelectInboundRouteItemDlg dlg(m_nProjID, nID, nIntersectNodeIDInALTObj);

		if (dlg.DoModal() != IDOK)
		{
			return;
		}

		int nALTObjID = dlg.GetSelALTObjID();
		int nIntersectNodeID = dlg.GetIntersectNodeID();
		CString strALTObjName = dlg.GetSelALTObjName();

		COOLTREE_NODE_INFO cni;
		CCoolTree::InitNodeInfo(this,cni);


		CRoutePriority* pPriority = (CRoutePriority*)pNodeData->m_pData;
		//add head Item
		if (pNodeData->m_emType == CTreeData::Start_Item)
		{
			CRouteItem *pFirstItem = new CRouteItem;
			pFirstItem->SetALTObjectID(nALTObjID);
			pFirstItem->SetIntersectNodeID(nIntersectNodeID);
			pFirstItem->SetItemName(strALTObjName);

			pPriority->AddNewItem(pFirstItem);

			pNodeData = new CTreeData;
			pNodeData->m_emType = CTreeData::Route_Item;
			pNodeData->m_pData = (DWORD)pFirstItem;
			HTREEITEM hItem = m_TreeCtrlRoute.InsertItem(strALTObjName, cni, FALSE, FALSE, hSelItem);
			m_TreeCtrlRoute.SetItemData(hItem, (DWORD)pNodeData);
			m_TreeCtrlRoute.Expand(hSelItem, TVE_EXPAND);
			m_TreeCtrlRoute.SelectItem(hItem);
		}
		// add other item
		else
		{
			CRouteItem *pOutboundRouteItem = (CRouteItem *)pNodeData->m_pData;
			if (pOutboundRouteItem)
			{
				nID = pOutboundRouteItem->GetALTObjectID();
				nIntersectNodeIDInALTObj = pOutboundRouteItem->GetIntersectNodeID();
			}

			CRouteItem *pIBRItem = new CRouteItem;
			pIBRItem->SetALTObjectID(nALTObjID);
			pIBRItem->SetIntersectNodeID(nIntersectNodeID);
			pIBRItem->SetItemName(strALTObjName);

			//HTREEITEM hParentItem = m_TreeCtrlRoute.GetParentItem(hSelItem);

			CTreeData *pParentIBRItem = (CTreeData *)m_TreeCtrlRoute.GetItemData(hSelItem);
			ASSERT(pParentIBRItem);
			CRouteItem* pRouteItem = (CRouteItem*)pParentIBRItem->m_pData;
			pRouteItem->AddNewItem(pIBRItem);

			pNodeData = new CTreeData;
			pNodeData->m_emType = CTreeData::Route_Item;
			pNodeData->m_pData = (DWORD)pIBRItem;
			HTREEITEM hItem = m_TreeCtrlRoute.InsertItem(strALTObjName, cni, FALSE, FALSE, hSelItem);
			m_TreeCtrlRoute.SetItemData(hItem, (DWORD)pNodeData);
			m_TreeCtrlRoute.Expand(hSelItem, TVE_EXPAND);
			m_TreeCtrlRoute.SelectItem(hItem);
		}
	}

	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
}

void CDlgBoundRouteAssignment2::SetAllControlsResize()
{
	CXTResizeDialogEx::Init();

	SetResize(IDC_STATIC_FLIGHTTYPE, SZ_TOP_LEFT, SZ_BOTTOM_LEFT);
	SetResize(m_ToolBarFlightType.GetDlgCtrlID(), SZ_TOP_LEFT, SZ_TOP_LEFT);
	SetResize(IDC_TREE_FLIGHTTYPE_TIME, SZ_TOP_LEFT, SZ_BOTTOM_LEFT);

	SetResize(m_wndSplitterVer.GetDlgCtrlID(), SZ_TOP_LEFT, SZ_BOTTOM_LEFT);

	SetResize(IDC_STATIC_ORIGDEST, SZ_TOP_LEFT, SZ_MIDDLE_RIGHT);
	SetResize(m_toolbarOrgDst.GetDlgCtrlID(), SZ_TOP_LEFT, SZ_TOP_CENTER);
	SetResize(IDC_LIST_RUNWAYEXITANDSTANDFAMILY, SZ_TOP_LEFT, SZ_MIDDLE_RIGHT);

	SetResize(m_wndSplitterHor.GetDlgCtrlID(), SZ_MIDDLE_LEFT, SZ_MIDDLE_RIGHT);

	SetResize(IDC_STATIC_ROUTECHOICE, SZ_MIDDLE_LEFT, SZ_BOTTOM_RIGHT);
	SetResize(m_btnShortestPath.GetDlgCtrlID(), SZ_MIDDLE_LEFT, SZ_MIDDLE_LEFT);
	SetResize(m_btnPreferredRoute.GetDlgCtrlID(), SZ_MIDDLE_LEFT, SZ_MIDDLE_LEFT);
	SetResize(IDC_STATIC_ROUTEASSIGNMENT, SZ_MIDDLE_LEFT, SZ_BOTTOM_RIGHT);
	SetResize(m_ToolBarRoute.GetDlgCtrlID(), SZ_MIDDLE_LEFT, SZ_MIDDLE_CENTER);
	SetResize(IDC_TREE_ROUTE, SZ_MIDDLE_LEFT, SZ_BOTTOM_RIGHT);

	SetResize(IDC_BUTTON_SAVE, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);
	SetResize(IDOK, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);
	SetResize(IDCANCEL, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);
}

void CDlgBoundRouteAssignment2::UpdateRouteChoice( CIn_OutBoundRoute* pCurBoundRoute )
{
	m_btnShortestPath.EnableWindow(pCurBoundRoute != NULL);
	m_btnPreferredRoute.EnableWindow(pCurBoundRoute != NULL);
	if (pCurBoundRoute)
	{
		BoundRouteChoice eRouteChoice = pCurBoundRoute->GetBoundRouteChoice();
		m_btnShortestPath.SetCheck(eRouteChoice == RouteChoice_Preferred ? BST_UNCHECKED : BST_CHECKED);
		m_btnPreferredRoute.SetCheck(eRouteChoice == RouteChoice_Preferred ? BST_CHECKED : BST_UNCHECKED);
	}
}

void CDlgBoundRouteAssignment2::OnSize(UINT nType, int cx, int cy)
{
	CXTResizeDialogEx::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
	UpdateSplitterRange();
}

void CDlgBoundRouteAssignment2::UpdateSplitterRange()
{
	const int nLeftMargin = 100;
	const int nRightMargin = 250;
	const int nTopMargin = 100;
	const int nBottomMargin = 150;
	CRect rc;
	GetClientRect(rc);
	m_wndSplitterVer.SetRange(nLeftMargin, rc.right - nRightMargin);
	m_wndSplitterHor.SetRange(nTopMargin, rc.bottom - nBottomMargin);
}

void CDlgBoundRouteAssignment2::UpdateRouteChoiceBtns()
{
	CIn_OutBoundRoute* pRoute = NULL;
	if (m_listOrgDst.GetItemCount())
	{
		int nSel = m_listOrgDst.GetCurSel();
		if (nSel>=0)
		{
			pRoute = (CIn_OutBoundRoute*)m_listOrgDst.GetItemData(nSel);
		}
	}
	UpdateRouteChoice(pRoute);
}
/////////////////////////InboundRouteAssignmentDlg///////////////
// InboundRouteAssignmentDlg::InboundRouteAssignmentDlg(int nProjID,PFuncSelectFlightType pSelectFlightType,CAirportDatabase *pAirPortdb,LPCTSTR lpszTitle,  CWnd* pParent /*= NULL*/)
// :CDlgBoundRouteAssignment2(nProjID,pSelectFlightType,pAirPortdb,lpszTitle, pParent)
// {
// 	m_pBoundRouteAssignment = new CBoundRouteAssignment(INBOUND, pAirPortdb);
// 	m_pBoundRouteAssignment->ReadData(m_nProjID);
// }
// 
// 
// InboundRouteAssignmentDlg::~InboundRouteAssignmentDlg()
// {
// 
// }
// 
// void InboundRouteAssignmentDlg::InitListCtrl()
// {
// 	DWORD dwStyle = m_listOrgDst.GetExtendedStyle();
// 	dwStyle |= LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_EX_GRIDLINES;
// 	m_listOrgDst.SetExtendedStyle(dwStyle);
// 
// 	CStringList strList;
// 	strList.RemoveAll();
// 	CString strCaption;
// 
// 	LV_COLUMNEX lvc;
// 	lvc.mask = LVCF_WIDTH|LVCF_TEXT;
// 
// 	strCaption.LoadString(IDS_EXITPOSITION);
// 	lvc.pszText = (LPSTR)(LPCTSTR)strCaption;
// 	lvc.cx = 170;
// 	lvc.fmt = LVCFMT_NOEDIT;
// 	lvc.csList = &strList;
// 	m_listOrgDst.InsertColumn(0, &lvc);
// 
// 	strCaption.LoadString(IDS_STANDFAMILY);
// 	lvc.pszText = (LPSTR)(LPCTSTR)strCaption;
// 	lvc.cx = 170;
// 	lvc.fmt = LVCFMT_NOEDIT;
// 	m_listOrgDst.InsertColumn(1, &lvc);
// }
// 
// void InboundRouteAssignmentDlg::SetOrgDstListContent()
// {
// 	m_listOrgDst.DeleteAllItems();
// 
// 	if (NULL == m_pBoundRouteAssignment )
// 		return;
// 
// 	ASSERT(m_pCurTimeAssignment);
// 
// 	size_t count = m_pCurTimeAssignment->GetElementCount();
// 	for (size_t i=0; i<count; i++)
// 	{
// 		CIn_OutBoundRoute *pInboundRoute = m_pCurTimeAssignment->GetItem(i);
// 
// 		//set runway exit name 
// 		CString strRunwayExitName = "";
// 		if (pInboundRoute->IsAllRunway())
// 		{
// 			strRunwayExitName = "All";
// 		}
// 		else
// 		{
// 			std::vector<int> vExits;
// 			vExits.clear();
// 			pInboundRoute->GetRunwayExitIDList(vExits);
// 			int nSize = vExits.size();
// 			for (int j =0; j < nSize; j++)
// 			{
// 				int nExitID = vExits.at(j);
// 				RunwayExit rwyExit;
// 				rwyExit.ReadData(nExitID);
// 				if (strRunwayExitName == "")
// 				{
// 					strRunwayExitName = rwyExit.GetName();
// 				}
// 				else
// 				{
// 					strRunwayExitName += ", ";
// 					strRunwayExitName += rwyExit.GetName();
// 				}
// 			}
// 		}
// 
// 		int newItem = m_listOrgDst.InsertItem(i, strRunwayExitName);
// 		m_listOrgDst.SetItemData(i, (DWORD_PTR)pInboundRoute);
// 
// 		//set stand name 	
// 		CString strStandFamily = "";
// 		if (pInboundRoute->IsAllStand())
// 		{
// 			strStandFamily = "All";
// 		}
// 		else
// 		{
// 			std::vector<int> vStandFmailys;
// 			vStandFmailys.clear();
// 			pInboundRoute->GetStandGroupIDList(vStandFmailys);
// 			int nSize = vStandFmailys.size();
// 			for (int j =0; j < nSize; j++)
// 			{
// 				int nStandFamilyID = vStandFmailys.at(j);
// 				ALTObjectGroup altObjGroup;
// 				altObjGroup.ReadData(nStandFamilyID);
// 				ALTObjectID altObjID = altObjGroup.getName();
// 				if (strStandFamily == "")
// 				{
// 					strStandFamily = altObjID.GetIDString();
// 				}
// 				else
// 				{
// 					strStandFamily += ", ";
// 					strStandFamily += altObjID.GetIDString();
// 				}
// 			}
// 		}
// 
// 		m_listOrgDst.SetItemText(newItem, 1, strStandFamily);
// 	}
// }
// 
// void InboundRouteAssignmentDlg::NewOrgDst()
// {
// 	CRunwayExitAndStandFamilySelectDlg dlg(CRunwayExitAndStandFamilySelectDlg::IDD,m_nProjID,  _T(""), _T(""));
// 
// 	if (dlg.DoModal() != IDOK)
// 	{
// 		return;
// 	}
// 
// 	CString strRunwayExitName = dlg.GetRunwayExitName();
// 	CString strStandFamilyName = dlg.GetOriginName();
// 	BOOL bAllStand = dlg.IsAllStand();
// 	BOOL bAllRunwayExit = dlg.IsAllRunwayExit();
// 	std::vector<int> vExits = dlg.GetRunwayExitList();
// 	std::vector<int> vStands = dlg.GetStandGroupList();
// 
// 
// 	m_pCurBoundRoute = new CIn_BoundRoute;
// 	m_pCurBoundRoute->SetAllRunway(bAllRunwayExit);
// 	m_pCurBoundRoute->SetAllStand(bAllStand);
// 	m_pCurBoundRoute->SetRunwayExitIDList(vExits);
// 	m_pCurBoundRoute->SetStandGroupIDList(vStands);
// 
// 	m_pCurTimeAssignment->AddNewItem(m_pCurBoundRoute);
// 
// 	int newItem = m_listOrgDst.InsertItem(m_listOrgDst.GetItemCount(), strRunwayExitName);
// 	m_listOrgDst.SetItemData(newItem, (DWORD_PTR)m_pCurBoundRoute);
// 	m_listOrgDst.SetItemText(newItem, 1, strStandFamilyName);
// 
// 	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
// }
// 
// void InboundRouteAssignmentDlg::EditOrgDst()
// {
// 	if (m_pCurBoundRoute == NULL)
// 		return;
// 
// 	int nSelItem = m_listOrgDst.GetCurSel();
// 	CString strRunwayExitName = m_listOrgDst.GetItemText(nSelItem, 0);
// 	CString strStandFamilyName = m_listOrgDst.GetItemText(nSelItem, 1);
// 
// 
// 	CRunwayExitAndStandFamilySelectDlg dlg(CRunwayExitAndStandFamilySelectDlg::IDD,m_nProjID, strRunwayExitName, strStandFamilyName);
// 	if (dlg.DoModal() != IDOK)
// 	{
// 		return;
// 	}
// 
// 	strRunwayExitName = dlg.GetRunwayExitName();
// 	strStandFamilyName = dlg.GetOriginName();
// 	BOOL bAllStand = dlg.IsAllStand();
// 	BOOL bAllRunwayExit = dlg.IsAllRunwayExit();
// 	std::vector<int> vExits = dlg.GetRunwayExitList();
// 	std::vector<int> vStands = dlg.GetStandGroupList();
// 
// 	//modify data
// 	m_pCurBoundRoute->SetAllRunway(bAllRunwayExit);
// 	m_pCurBoundRoute->SetAllStand(bAllStand);
// 	m_pCurBoundRoute->SetRunwayExitIDList(vExits);
// 	m_pCurBoundRoute->SetStandGroupIDList(vStands);
// 
// 	//m_pCurBoundRoute->SetRunwayAllExitFlag(bRunwayAllFlag);
// 
// 	m_listOrgDst.SetItemText(nSelItem, 0, strRunwayExitName);
// 	m_listOrgDst.SetItemText(nSelItem, 1, strStandFamilyName);
// 
// 	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
// }
// 
// 
/////////////////////////OutboudRouteAssignmentDlg///////////////
CDlgOutboundRouteAssignment2::CDlgOutboundRouteAssignment2(int nProjID,PFuncSelectFlightType pSelectFlightType,CAirportDatabase *pAirPortdb,LPCTSTR lpszTitle,  CWnd* pParent /*= NULL*/)
:CDlgBoundRouteAssignment2(nProjID,pSelectFlightType,pAirPortdb,lpszTitle, pParent)
{
	m_pBoundRouteAssignment = new CBoundRouteAssignment(OUTBOUND, pAirPortdb);
	m_pBoundRouteAssignment->ReadData(m_nProjID);
}

CDlgOutboundRouteAssignment2::~CDlgOutboundRouteAssignment2()
{

}

void CDlgOutboundRouteAssignment2::InitListCtrl()
{
	DWORD dwStyle = m_listOrgDst.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_EX_GRIDLINES;
	m_listOrgDst.SetExtendedStyle(dwStyle);

	CStringList strList;
	strList.RemoveAll();

	LV_COLUMNEX lvc;
	lvc.mask = LVCF_WIDTH|LVCF_TEXT;

	lvc.pszText = (LPSTR)_T("Origin");
	lvc.cx = 170;
	lvc.fmt = LVCFMT_NOEDIT;
	lvc.csList = &strList;
	m_listOrgDst.InsertColumn(0, &lvc);

	lvc.pszText = (LPSTR)_T("Destination");
	lvc.cx = 170;
	lvc.fmt = LVCFMT_NOEDIT;
	m_listOrgDst.InsertColumn(1, &lvc);
}

void CDlgOutboundRouteAssignment2::SetOrgDstListContent()
{
	m_listOrgDst.DeleteAllItems();

	if (NULL == m_pBoundRouteAssignment)
		return;

	size_t count = m_pCurTimeAssignment->GetElementCount();
	for (size_t i=0; i<count; i++)
	{
		COut_BoundRoute *pOutboundRoute = (COut_BoundRoute*)m_pCurTimeAssignment->GetItem(i);

		//set stand name
		int newItem = m_listOrgDst.InsertItem(i, pOutboundRoute->GetOriginString());
		m_listOrgDst.SetItemData(i, (DWORD_PTR)pOutboundRoute);

		//set takeoff position name 
		m_listOrgDst.SetItemText(newItem, 1, pOutboundRoute->GetDestinationString());
	}
}

void CDlgOutboundRouteAssignment2::NewOrgDst()
{
	CDlgSelectAirsideOrigDest dlg(CDlgSelectAirsideOrigDest::IDD, m_nProjID, _T(""), _T(""));

	if (IDOK == dlg.DoModal())
	{
		m_pCurBoundRoute = new COut_BoundRoute;
		dlg.PackIntoData(m_pCurBoundRoute);

		m_pCurTimeAssignment->AddNewItem(m_pCurBoundRoute);

		int newItem = m_listOrgDst.InsertItem(m_listOrgDst.GetItemCount(), dlg.GetOriginName());
		m_listOrgDst.SetItemData(newItem, (DWORD_PTR)m_pCurBoundRoute);
		m_listOrgDst.SetItemText(newItem, 1, dlg.GetRunwayExitName());

		GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
	}

}

void CDlgOutboundRouteAssignment2::EditOrgDst()
{
	if (m_pCurBoundRoute == NULL)
		return;

	int nSelItem = m_listOrgDst.GetCurSel();
	CString strStandFamilyName = m_listOrgDst.GetItemText(nSelItem, 0);
	CString strRunwayExitName = m_listOrgDst.GetItemText(nSelItem, 1);

	CDlgSelectAirsideOrigDest dlg(CDlgSelectAirsideOrigDest::IDD, m_nProjID, strRunwayExitName, strStandFamilyName);
	if (IDOK == dlg.DoModal())
	{
		//modify data
		dlg.PackIntoData(m_pCurBoundRoute);

		//m_pCurBoundRoute->SetRunwayAllExitFlag(bRunwayAllFlag);

		m_listOrgDst.SetItemText(nSelItem, 0, dlg.GetOriginName());
		m_listOrgDst.SetItemText(nSelItem, 1, dlg.GetRunwayExitName());

		GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
	}

}
