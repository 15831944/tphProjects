// OutboundRouteAssignmentDlg.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "OutBoundRouteAssignmentDlg.h"
#include "../InputAirside/BoundRouteAssignment.h"
#include "../InputAirside/TaxiRoute.h"
#include "../InputAirside/RouteItem.h"
#include "InputAirside\ALTObjectGroup.h"
#include "InputAirside\Runway.h"
#include "InputAirside\Taxiway.h"
#include "SelectStandFamilyAndTakeoffPositionDlg.h"
#include "RunwayExitAndStandFamilySelectDlg.h"
#include "SelectInboundRouteItemDlg.h"
#include "Common/AirportDatabase.h"
#include "DlgTimeRange.h"
#include "../Database/DBElementCollection_Impl.h"
#include "DlgParkingStandList.h"


namespace
{
	const UINT ID_NEW_RUNWAYEXITANDSTANDFAMILY      = 10;
	const UINT ID_EDIT_RUNWAYEXITANDSTANDFAMILY     = 11;
	const UINT ID_DEL_RUNWAYEXITANDSTANDFAMILY      = 12;

	const UINT ID_NEW_ROUTE  = 20;
	const UINT ID_DEL_ROUTE  = 21;
	const UINT ID_EDIT_ROUTE  = 22;
	const UINT ID_MOVE_NEXT = 23;
	const UINT ID_MOVE_PRE = 24;

	const UINT ID_NEW_FLIGHTTY = 30 ;
	const UINT ID_DEL_FLIGHTTY = 31 ;
	const UINT ID_EDIT_FLIGHTTY = 32 ;
}

// CBoundRouteAssignmentDlg dialog
static const char* szBuff[] = {_T("No"),_T("Yes")};
#define ID_WND_SPLITTER 1
IMPLEMENT_DYNAMIC(CBoundRouteAssignmentDlg, CDialog)
CBoundRouteAssignmentDlg::CBoundRouteAssignmentDlg(int nProjID,PFuncSelectFlightType pSelectFlightType,CAirportDatabase *pAirPortdb, LPCTSTR lpszTitle, CWnd* pParent /*=NULL*/)
	: CDialog(CBoundRouteAssignmentDlg::IDD, pParent)
	, m_nProjID(nProjID)
	, m_pCurBoundRoute(NULL)
	, m_pCurTimeAssignment(NULL)
	, m_pSelectFlightType(pSelectFlightType)
	, m_pAirportDB(pAirPortdb)
	, m_strTitle(lpszTitle)
{

}

CBoundRouteAssignmentDlg::~CBoundRouteAssignmentDlg()
{
	if(m_pBoundRouteAssignment)
	{
		delete m_pBoundRouteAssignment;
		m_pBoundRouteAssignment = NULL;
	}

}

void CBoundRouteAssignmentDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//DDX_Control(pDX, IDC_RADIO_SHORTESTPATH, m_btnShortPathToStand);
	//DDX_Control(pDX, IDC_RADIO_TIMESHORTEST, m_btnTmieShortPathToStand);
	//DDX_Control(pDX, IDC_RADIO_PREFERREDROUTE, m_btnPreferredRoute);
	DDX_Control(pDX, IDC_TREE_FLIGHTTYPE_TIME, m_wndTreeFltTime);
	DDX_Control(pDX, IDC_COMBO_ROUTECHOICE, m_cmbRouteType);
	DDX_Control(pDX, IDC_LIST_RUNWAYEXITANDSTANDFAMILY, m_ListCtrlExitPositionAndStandFamily);
	DDX_Control(pDX, IDC_TREE_ROUTE, m_TreeCtrlRoute);
	DDX_Control(pDX, IDC_STATIC_FLIGHTTYPE,m_staticFlightType);
	DDX_Control(pDX, IDC_STATIC_ROUTEASSIGNMENT,m_staticRouteAssign);
	DDX_Control(pDX, IDC_STATIC_ROUTECHOICE,m_static);
	DDX_Control(pDX, IDC_BUTTON_SAVE,m_btnSave);
	DDX_Control(pDX, IDOK,m_btOk);
	DDX_Control(pDX, IDCANCEL,m_btCancel);
}


BEGIN_MESSAGE_MAP(CBoundRouteAssignmentDlg, CDialog)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	//ON_BN_CLICKED(IDC_RADIO_SHORTESTPATH, OnBnClickedRadioShortestpath)
	//ON_BN_CLICKED(IDC_RADIO_TIMESHORTEST, OnBnClickedRadioTimeshortest)
	//ON_BN_CLICKED(IDC_RADIO_PREFERREDROUTE, OnBnClickedRadioPreferredroute)
	ON_COMMAND(ID_NEW_RUNWAYEXITANDSTANDFAMILY, OnCmdNewRunwayExitAndStandFamily)
	ON_COMMAND(ID_EDIT_RUNWAYEXITANDSTANDFAMILY, OnCmdEditRunwayExitAndStandFamily)
	ON_COMMAND(ID_DEL_RUNWAYEXITANDSTANDFAMILY, OnCmdDelRunwayExitAndStandFamily)
	ON_COMMAND(ID_NEW_FLIGHTTY ,OnCmdNewFlightTime) 
	ON_COMMAND(ID_DEL_FLIGHTTY , OnCmdDelFlightTime)
	ON_COMMAND(ID_EDIT_FLIGHTTY, OnCmdEditFlightTime) 
	ON_COMMAND(ID_NEW_ROUTE, OnCmdNewRoute)
	ON_COMMAND(ID_DEL_ROUTE, OnCmdDelRoute)
	ON_COMMAND(ID_EDIT_ROUTE, OnCmdEditRoute)
	ON_COMMAND(ID_MOVE_NEXT, OnCmdMoveNextPriority)
	ON_COMMAND(ID_MOVE_PRE, OnCmdMovePreviousPriority)
	ON_COMMAND(ID_ADD_TIMERANGE,OnAddTimeRange)
	ON_COMMAND(ID_EDIT_TIMERANGE,OnEditTimeRange)
	ON_COMMAND(ID_DEL_TIMERANGE,OnDelTimeRange)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_RUNWAYEXITANDSTANDFAMILY, OnLvnItemchangedListRunwayexitandstandfamily)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_ROUTE, OnTvnSelchangedTreeRoute)
	ON_BN_CLICKED(IDC_BUTTON_SAVE, OnBnClickedButtonSave)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_FLIGHTTYPE_TIME, OnTvnSelchangedTreeFlighttypeTime)
	ON_CBN_SELCHANGE(IDC_COMBO_ROUTECHOICE, OnCbnSelchangeComboRoutechoice)
	ON_WM_CONTEXTMENU()
END_MESSAGE_MAP()


// CBoundRouteAssignmentDlg message handlers
int CBoundRouteAssignmentDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!m_ToolBarRunwayExitAndStandFamily.CreateEx(this, TBSTYLE_FLAT,WS_CHILD|WS_VISIBLE|CBRS_ALIGN_TOP)
		|| !m_ToolBarRunwayExitAndStandFamily.LoadToolBar(IDR_TOOLBAR_ADD_DEL_EDIT))
	{
		return -1;
	}

	CToolBarCtrl& toolbarRunwayExitAndStandFamily = m_ToolBarRunwayExitAndStandFamily.GetToolBarCtrl();
	toolbarRunwayExitAndStandFamily.SetCmdID(0, ID_NEW_RUNWAYEXITANDSTANDFAMILY);
	toolbarRunwayExitAndStandFamily.SetCmdID(1, ID_DEL_RUNWAYEXITANDSTANDFAMILY);
	toolbarRunwayExitAndStandFamily.SetCmdID(2, ID_EDIT_RUNWAYEXITANDSTANDFAMILY);

	if (!m_ToolBarRoute.CreateEx(this, TBSTYLE_FLAT,WS_CHILD|WS_VISIBLE|CBRS_ALIGN_TOP)
		|| !m_ToolBarRoute.LoadToolBar(IDR_TOOLBAR_ROUTE_PRIORITY))
	{
		return -1;
	}
 
//	m_ToolBarRoute.SetDlgCtrlID(m_ToolBarRunwayExitAndStandFamily.GetDlgCtrlID()+1);

	CToolBarCtrl& toolbarRoute = m_ToolBarRoute.GetToolBarCtrl();
	toolbarRoute.SetCmdID(0, ID_NEW_ROUTE);
	toolbarRoute.SetCmdID(1, ID_DEL_ROUTE);
	toolbarRoute.SetCmdID(2, ID_EDIT_ROUTE);
	toolbarRoute.SetCmdID(3, ID_MOVE_NEXT);
	toolbarRoute.SetCmdID(4, ID_MOVE_PRE);

	if(!m_ToolBarFlightType.CreateEx(this,TBSTYLE_FLAT,WS_CHILD|WS_VISIBLE|CBRS_ALIGN_TOP)
		|| !m_ToolBarFlightType.LoadToolBar(IDR_TOOLBAR_ADD_DEL_EDIT))
	{
		return -1 ;
	}
	m_ToolBarFlightType.SetDlgCtrlID(m_ToolBarRoute.GetDlgCtrlID() + 1) ;
	CToolBarCtrl& toolbarflightTY = m_ToolBarFlightType.GetToolBarCtrl() ;
	toolbarflightTY.SetCmdID(0,ID_NEW_FLIGHTTY);
	toolbarflightTY.SetCmdID(1,ID_DEL_FLIGHTTY) ;
	toolbarflightTY.SetCmdID(2,ID_EDIT_FLIGHTTY) ;

	return 0;
}

void CBoundRouteAssignmentDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	if (m_wndTreeFltTime.GetSafeHwnd())
	{
		CRect rectWnd;
		int nSplitterWidth=9;
		int nLeft=nSplitterWidth;
		
		CRect leftRect;
		m_staticFlightType.SetWindowPos(NULL,nLeft,nSplitterWidth,cx/2 - 12 - 10,cy - 45,SWP_NOACTIVATE|SWP_NOZORDER);
		m_staticFlightType.GetWindowRect(leftRect);
		ScreenToClient(leftRect);
		//Processor Tree
		int nTop=leftRect.top+40;

		m_wndTreeFltTime.SetWindowPos(NULL,nLeft,nTop,cx/2 - 12 - 10,leftRect.Height() + 8 - nTop,SWP_NOACTIVATE|SWP_NOZORDER);
		m_wndTreeFltTime.GetWindowRect(rectWnd);
		ScreenToClient(rectWnd);
		//Splitter1
		if(!m_wndSplitter1.m_hWnd)//Create Splitter1
		{
			m_wndSplitter1.Create(WS_CHILD | WS_VISIBLE, CRect(0,0,0,0), this, ID_WND_SPLITTER);
			m_wndSplitter1.SetStyle(SPS_VERTICAL);
		}

		m_wndSplitter1.SetWindowPos(NULL,rectWnd.right,leftRect.top,4,leftRect.Height(),SWP_NOACTIVATE|SWP_NOZORDER);
		m_wndSplitter1.GetWindowRect(rectWnd);
		ScreenToClient(rectWnd);
		m_wndSplitter1.SetRange(leftRect.left,cx - 2 * nSplitterWidth);
		//Route choice
		CRect rect;
		m_static.GetWindowRect(rectWnd);
		m_static.SetWindowPos(NULL,leftRect.right + 8,leftRect.top,rectWnd.Width(),rectWnd.Height(),SWP_NOACTIVATE|SWP_NOZORDER);
		m_static.GetWindowRect(rect);
		ScreenToClient(rect);

		m_cmbRouteType.SetWindowPos(NULL,rect.right + 2,rect.top,cx/2 - 12 + 10 - rect.Width() - 2,rect.Height(),SWP_NOACTIVATE | SWP_NOZORDER);

		//route assignment
		m_staticRouteAssign.SetWindowPos(NULL,rect.left,rect.bottom + 8,cx/2 - 12 + 10,leftRect.bottom - rect.bottom - 8,SWP_NOACTIVATE|SWP_NOZORDER);
		m_staticRouteAssign.GetWindowRect(rectWnd);
		ScreenToClient(rectWnd);

		m_ToolBarRunwayExitAndStandFamily.SetWindowPos(NULL,rectWnd.left + 4,rectWnd.top + 10,cx/2 - 24 + 10,20,SWP_NOACTIVATE|SWP_NOZORDER);
		m_ToolBarRunwayExitAndStandFamily.GetWindowRect(rectWnd);
		ScreenToClient(rectWnd);

		//Process Tree
		nTop=rectWnd.bottom+2;
		m_ListCtrlExitPositionAndStandFamily.SetWindowPos(NULL,rect.left,nTop,cx/2 - 12 + 10,cy/4,SWP_NOACTIVATE|SWP_NOZORDER);
		m_ListCtrlExitPositionAndStandFamily.GetWindowRect(rectWnd);
		ScreenToClient(rectWnd);	

		m_ToolBarRoute.SetWindowPos(NULL,rectWnd.left + 4, rectWnd.bottom + 8,cx/2 - 12 + 10,20,SWP_NOACTIVATE|SWP_NOZORDER);
		m_ToolBarRoute.GetWindowRect(rectWnd);
		ScreenToClient(rectWnd);

		m_TreeCtrlRoute.SetWindowPos(NULL,rect.left,rectWnd.bottom + 2,cx/2 - 12 + 10,leftRect.bottom - rectWnd.bottom - 2,SWP_NOACTIVATE|SWP_NOZORDER);
		m_TreeCtrlRoute.GetWindowRect(rectWnd);
		ScreenToClient(rectWnd);

		CRect rcBtn;
		GetDlgItem(IDCANCEL)->GetWindowRect(&rcBtn);
		GetDlgItem(IDCANCEL)->SetWindowPos(NULL,rectWnd.right - rcBtn.Width() - 2,rectWnd.bottom + 8,rcBtn.Width(),rcBtn.Height(),NULL);
		GetDlgItem(IDCANCEL)->GetWindowRect(&rcBtn);
		ScreenToClient(rcBtn);
		GetDlgItem(IDOK)->SetWindowPos(NULL,rcBtn.left - rcBtn.Width() - 6,rectWnd.bottom + 8,rcBtn.Width(),rcBtn.Height(), NULL);
		GetDlgItem(IDOK)->GetWindowRect(&rcBtn);
		ScreenToClient(rcBtn);
		CRect rcSave;
		GetDlgItem(IDC_BUTTON_SAVE)->GetWindowRect(&rcSave);
		GetDlgItem(IDC_BUTTON_SAVE)->SetWindowPos(NULL,rcBtn.left - rcSave.Width() - 10,rectWnd.bottom+8,rcSave.Width(),rcSave.Height(),NULL);
	}
	Invalidate(FALSE);
}

BOOL CBoundRouteAssignmentDlg::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}

void CBoundRouteAssignmentDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	HWND hwndT=::GetWindow(m_hWnd, GW_CHILD);
	CRect rectCW;
	CRgn rgnCW;
	rgnCW.CreateRectRgn(0,0,0,0);
	while (hwndT != NULL)
	{
		CWnd* pWnd=CWnd::FromHandle(hwndT)  ;
		if(
			pWnd->IsKindOf(RUNTIME_CLASS(CListCtrlEx))||
			pWnd->IsKindOf(RUNTIME_CLASS(CTreeCtrl))||
			pWnd->IsKindOf(RUNTIME_CLASS(CComboBox)))
		{
			if(!pWnd->IsWindowVisible())
			{
				hwndT=::GetWindow(hwndT,GW_HWNDNEXT);
				continue;
			}
			pWnd->GetWindowRect(rectCW);
			ScreenToClient(rectCW);
			CRgn rgnTemp;
			rgnTemp.CreateRectRgnIndirect(rectCW);
			rgnCW.CombineRgn(&rgnCW,&rgnTemp,RGN_OR);
		}
		hwndT=::GetWindow(hwndT,GW_HWNDNEXT);

	}
	CRect rect;
	GetClientRect(&rect);
	CRgn rgn;
	rgn.CreateRectRgnIndirect(rect);
	CRgn rgnDraw;
	rgnDraw.CreateRectRgn(0,0,0,0);
	rgnDraw.CombineRgn(&rgn,&rgnCW,RGN_DIFF);
	CBrush br(GetSysColor(COLOR_BTNFACE));
	dc.FillRgn(&rgnDraw,&br);
	CRect rectRight=rect;
	rectRight.left=rectRight.right-10;
	dc.FillRect(rectRight,&br);
	rectRight=rect;
	rectRight.top=rect.bottom-44;
	dc.FillRect(rectRight,&br);
}

BOOL CBoundRouteAssignmentDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetWindowText(m_strTitle);

	InitToolBar();
	InitListCtrl();
	InitComboBox();
	InitFlightTimeTree() ;

	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(FALSE);

	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),TRUE);
	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),FALSE);

	CRect rect;
	GetClientRect(rect);
	SetWindowPos(NULL,0,0,rect.Width(),rect.Height(),SWP_NOMOVE|SWP_NOACTIVATE|SWP_NOZORDER);
	m_cmbRouteType.EnableWindow(FALSE);
	SetCtrlState(FALSE);

	return TRUE;
}

void CBoundRouteAssignmentDlg::InitToolBar(void)
{	
	CRect rect;
	//Flight type
	m_wndTreeFltTime.GetWindowRect( &rect );
	ScreenToClient( &rect );
	rect.top -= 26;
	rect.bottom = rect.top + 22;
	rect.left += 4;
	m_ToolBarFlightType.MoveWindow(&rect);

	m_ToolBarFlightType.GetToolBarCtrl().EnableButton( ID_NEW_FLIGHTTY, TRUE );
	m_ToolBarFlightType.GetToolBarCtrl().EnableButton( ID_DEL_FLIGHTTY, FALSE );
	m_ToolBarFlightType.GetToolBarCtrl().EnableButton( ID_EDIT_FLIGHTTY, FALSE );

	//Runway exit and stand family
	m_ListCtrlExitPositionAndStandFamily.GetWindowRect( &rect );
	ScreenToClient( &rect );
	rect.top -= 26;
	rect.bottom = rect.top + 22;
	rect.left += 4;
	m_ToolBarRunwayExitAndStandFamily.MoveWindow(&rect);

	m_ToolBarRunwayExitAndStandFamily.GetToolBarCtrl().EnableButton( ID_NEW_RUNWAYEXITANDSTANDFAMILY, FALSE );
	m_ToolBarRunwayExitAndStandFamily.GetToolBarCtrl().EnableButton( ID_DEL_RUNWAYEXITANDSTANDFAMILY, FALSE );
	m_ToolBarRunwayExitAndStandFamily.GetToolBarCtrl().EnableButton( ID_EDIT_RUNWAYEXITANDSTANDFAMILY, FALSE );

	//route
	m_TreeCtrlRoute.GetWindowRect( &rect );
	ScreenToClient( &rect );
	rect.top -= 26;
	rect.bottom = rect.top + 22;
	rect.left += 4;
	m_ToolBarRoute.MoveWindow(&rect);

	m_ToolBarRoute.GetToolBarCtrl().EnableButton(ID_NEW_ROUTE, FALSE);
	m_ToolBarRoute.GetToolBarCtrl().EnableButton(ID_DEL_ROUTE, FALSE);
	m_ToolBarRoute.GetToolBarCtrl().EnableButton(ID_EDIT_ROUTE, FALSE);
}

void CBoundRouteAssignmentDlg::InitListCtrl(void)
{
	// set list control window style

}

void CBoundRouteAssignmentDlg::SetCtrlState(BOOL bState)
{
	if (!bState)
	{
		m_TreeCtrlRoute.DeleteAllItems();
		m_ListCtrlExitPositionAndStandFamily.DeleteAllItems();

	}

	m_TreeCtrlRoute.EnableWindow(bState);
	m_ListCtrlExitPositionAndStandFamily.EnableWindow(bState);	
}
void CBoundRouteAssignmentDlg::InitComboBox()
{
	int  index = 0 ;
	index = m_cmbRouteType.AddString(_T("Shortest path to stand"));
	m_cmbRouteType.SetItemData(index ,(DWORD_PTR) BOUNDROUTEASSIGNMENT_SHORTPATH) ;

	index = m_cmbRouteType.AddString(_T("Time shortest path to stand")) ;
	m_cmbRouteType.SetItemData(index ,(DWORD_PTR) BOUNDROUTEASSIGNMENT_TIMESHORTPATH);

	index = m_cmbRouteType.AddString(_T("Preferred route"));
	m_cmbRouteType.SetItemData(index,(DWORD_PTR) BOUNDROUTEASSIGNMENT_PREFERREDROUTE);
}
void CBoundRouteAssignmentDlg::InitFlightTimeTree()
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

void CBoundRouteAssignmentDlg::EditOriginAndDestination()
{

}

void CBoundRouteAssignmentDlg::NewOriginAndDestination()
{

}

void CBoundRouteAssignmentDlg::SetListContent(void)
{
	//virtual function
}

void CBoundRouteAssignmentDlg::UpdateRunwayExitAndStandToolBar()
{
	BOUNDROUTEASSIGNMENTSTATUS raStatus = (BOUNDROUTEASSIGNMENTSTATUS)m_cmbRouteType.GetItemData(m_cmbRouteType.GetCurSel()) ;
	if (raStatus == BOUNDROUTEASSIGNMENT_PREFERREDROUTE)
	{
		m_ToolBarRunwayExitAndStandFamily.GetToolBarCtrl().EnableButton( ID_NEW_RUNWAYEXITANDSTANDFAMILY, TRUE );

		int nItemCount = m_ListCtrlExitPositionAndStandFamily.GetItemCount();

		if (0 < nItemCount)
		{
			int nSelCount = m_ListCtrlExitPositionAndStandFamily.GetSelectedCount();
			if (0 < nSelCount)
			{
				m_ToolBarRunwayExitAndStandFamily.GetToolBarCtrl().EnableButton( ID_DEL_RUNWAYEXITANDSTANDFAMILY, TRUE );
				m_ToolBarRunwayExitAndStandFamily.GetToolBarCtrl().EnableButton( ID_EDIT_RUNWAYEXITANDSTANDFAMILY, TRUE );
			}
			else
			{
				m_ToolBarRunwayExitAndStandFamily.GetToolBarCtrl().EnableButton( ID_DEL_RUNWAYEXITANDSTANDFAMILY, FALSE );
				m_ToolBarRunwayExitAndStandFamily.GetToolBarCtrl().EnableButton( ID_EDIT_RUNWAYEXITANDSTANDFAMILY, FALSE );
			}
		}
		else
		{
			m_ToolBarRunwayExitAndStandFamily.GetToolBarCtrl().EnableButton( ID_DEL_RUNWAYEXITANDSTANDFAMILY, FALSE );
			m_ToolBarRunwayExitAndStandFamily.GetToolBarCtrl().EnableButton( ID_EDIT_RUNWAYEXITANDSTANDFAMILY, FALSE );
		}
	}
	else
	{
		m_ToolBarRunwayExitAndStandFamily.GetToolBarCtrl().EnableButton( ID_NEW_RUNWAYEXITANDSTANDFAMILY, FALSE );
		m_ToolBarRunwayExitAndStandFamily.GetToolBarCtrl().EnableButton( ID_DEL_RUNWAYEXITANDSTANDFAMILY, FALSE );
		m_ToolBarRunwayExitAndStandFamily.GetToolBarCtrl().EnableButton( ID_EDIT_RUNWAYEXITANDSTANDFAMILY, FALSE );
	}
}
void CBoundRouteAssignmentDlg::UpdateRouteToolBar()
{
	BOUNDROUTEASSIGNMENTSTATUS raStatus = (BOUNDROUTEASSIGNMENTSTATUS)m_cmbRouteType.GetItemData(m_cmbRouteType.GetCurSel()) ; 
	if (raStatus == BOUNDROUTEASSIGNMENT_PREFERREDROUTE)
	{
		int nItemCount = m_ListCtrlExitPositionAndStandFamily.GetItemCount();

		if (0 < nItemCount)
		{
			int nSelCount = m_ListCtrlExitPositionAndStandFamily.GetSelectedCount();
			if (0 < nSelCount)
			{
				m_ToolBarRoute.GetToolBarCtrl().EnableButton(ID_NEW_ROUTE, TRUE);

				HTREEITEM hSelItem = m_TreeCtrlRoute.GetSelectedItem();
				if (NULL !=hSelItem)
				{
					CTreeData* pNodeData = (CTreeData*)m_TreeCtrlRoute.GetItemData(hSelItem);
					if (pNodeData)
					{
						if (pNodeData->m_emType == CTreeData::Route_Item )
						{
							m_ToolBarRoute.GetToolBarCtrl().EnableButton(ID_DEL_ROUTE, TRUE);
							m_ToolBarRoute.GetToolBarCtrl().EnableButton(ID_EDIT_ROUTE, TRUE);
							m_ToolBarRoute.GetToolBarCtrl().EnableButton(ID_MOVE_NEXT, FALSE);
							m_ToolBarRoute.GetToolBarCtrl().EnableButton(ID_MOVE_PRE, FALSE);
						}
						else if (pNodeData->m_emType == CTreeData::Priority_Item)
						{
							m_ToolBarRoute.GetToolBarCtrl().EnableButton(ID_DEL_ROUTE, TRUE);
							m_ToolBarRoute.GetToolBarCtrl().EnableButton(ID_EDIT_ROUTE, FALSE);
							
							if (m_TreeCtrlRoute.GetNextSiblingItem(hSelItem))
							{
								m_ToolBarRoute.GetToolBarCtrl().EnableButton(ID_MOVE_PRE, TRUE);
							}
							else
							{
								m_ToolBarRoute.GetToolBarCtrl().EnableButton(ID_MOVE_PRE, FALSE);
							}
	
							if (m_TreeCtrlRoute.GetPrevSiblingItem(hSelItem))
							{
								m_ToolBarRoute.GetToolBarCtrl().EnableButton(ID_MOVE_NEXT, TRUE);
							}
							else
							{
								m_ToolBarRoute.GetToolBarCtrl().EnableButton(ID_MOVE_NEXT, FALSE);
							}
							
						}
						else if (pNodeData->m_emType == CTreeData::Start_Item)
						{
							m_ToolBarRoute.GetToolBarCtrl().EnableButton(ID_NEW_ROUTE, TRUE);
							m_ToolBarRoute.GetToolBarCtrl().EnableButton(ID_DEL_ROUTE, FALSE);
							m_ToolBarRoute.GetToolBarCtrl().EnableButton(ID_EDIT_ROUTE, FALSE);
							m_ToolBarRoute.GetToolBarCtrl().EnableButton(ID_MOVE_NEXT, FALSE);
							m_ToolBarRoute.GetToolBarCtrl().EnableButton(ID_MOVE_PRE, FALSE);
						}
						else
						{
							m_ToolBarRoute.GetToolBarCtrl().EnableButton(ID_NEW_ROUTE, FALSE);
							m_ToolBarRoute.GetToolBarCtrl().EnableButton(ID_DEL_ROUTE, FALSE);
							m_ToolBarRoute.GetToolBarCtrl().EnableButton(ID_EDIT_ROUTE, FALSE);
							m_ToolBarRoute.GetToolBarCtrl().EnableButton(ID_MOVE_NEXT, FALSE);
							m_ToolBarRoute.GetToolBarCtrl().EnableButton(ID_MOVE_PRE, FALSE);
						}
					}
				}
				else
				{
					m_ToolBarRoute.GetToolBarCtrl().EnableButton(ID_DEL_ROUTE, FALSE);
					m_ToolBarRoute.GetToolBarCtrl().EnableButton(ID_EDIT_ROUTE, FALSE);
					m_ToolBarRoute.GetToolBarCtrl().EnableButton(ID_MOVE_NEXT, FALSE);
					m_ToolBarRoute.GetToolBarCtrl().EnableButton(ID_MOVE_PRE, FALSE);
				}
			}
			else
			{
				m_ToolBarRoute.GetToolBarCtrl().EnableButton(ID_NEW_ROUTE, FALSE);
				m_ToolBarRoute.GetToolBarCtrl().EnableButton(ID_DEL_ROUTE, FALSE);
				m_ToolBarRoute.GetToolBarCtrl().EnableButton(ID_EDIT_ROUTE, FALSE);
				m_ToolBarRoute.GetToolBarCtrl().EnableButton(ID_MOVE_NEXT, FALSE);
				m_ToolBarRoute.GetToolBarCtrl().EnableButton(ID_MOVE_PRE, FALSE);
			}
		}
		else
		{
			m_ToolBarRoute.GetToolBarCtrl().EnableButton(ID_NEW_ROUTE, FALSE);
			m_ToolBarRoute.GetToolBarCtrl().EnableButton(ID_DEL_ROUTE, FALSE);
			m_ToolBarRoute.GetToolBarCtrl().EnableButton(ID_EDIT_ROUTE, FALSE);
			m_ToolBarRoute.GetToolBarCtrl().EnableButton(ID_MOVE_NEXT, FALSE);
			m_ToolBarRoute.GetToolBarCtrl().EnableButton(ID_MOVE_PRE, FALSE);
		}
	}
	else
	{
		m_ToolBarRoute.GetToolBarCtrl().EnableButton(ID_NEW_ROUTE, FALSE);
		m_ToolBarRoute.GetToolBarCtrl().EnableButton(ID_DEL_ROUTE, FALSE);
		m_ToolBarRoute.GetToolBarCtrl().EnableButton(ID_EDIT_ROUTE, FALSE);
		m_ToolBarRoute.GetToolBarCtrl().EnableButton(ID_MOVE_NEXT, FALSE);
		m_ToolBarRoute.GetToolBarCtrl().EnableButton(ID_MOVE_PRE, FALSE);
	}
}

void CBoundRouteAssignmentDlg::OnCmdNewRunwayExitAndStandFamily()
{
	NewOriginAndDestination();
}

void CBoundRouteAssignmentDlg::OnCmdDelRunwayExitAndStandFamily()
{
	ASSERT(m_pCurBoundRoute);

	int nSelItem = m_ListCtrlExitPositionAndStandFamily.GetCurSel();

	m_TreeCtrlRoute.DeleteAllItems();
	m_ListCtrlExitPositionAndStandFamily.DeleteItemEx(nSelItem);

	m_pCurTimeAssignment->DeleteItem(m_pCurBoundRoute);

	nSelItem = m_ListCtrlExitPositionAndStandFamily.GetCurSel();
	if(nSelItem != -1)
		m_pCurBoundRoute = (CIn_OutBoundRoute*)m_ListCtrlExitPositionAndStandFamily.GetItemData(nSelItem);

	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
}

void CBoundRouteAssignmentDlg::OnCmdEditRunwayExitAndStandFamily()
{
	EditOriginAndDestination();
}

void CBoundRouteAssignmentDlg::OnCmdMoveNextPriority()//down move
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

void CBoundRouteAssignmentDlg::OnCmdMovePreviousPriority()//up move
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

void CBoundRouteAssignmentDlg::OnCmdDelRoute()
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
}

void CBoundRouteAssignmentDlg::OnCmdEditRoute()
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

void CBoundRouteAssignmentDlg::OnLvnItemchangedListRunwayexitandstandfamily(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	int nSelItem = m_ListCtrlExitPositionAndStandFamily.GetCurSel();
	UpdateRunwayExitAndStandToolBar();

	//reset the route tree
	m_TreeCtrlRoute.DeleteAllItems();

	//no selected item
	if (nSelItem < 0
		|| m_ListCtrlExitPositionAndStandFamily.GetItemCount() < (int)m_pCurTimeAssignment->GetElementCount()
		|| m_ListCtrlExitPositionAndStandFamily.GetSelectedCount() < 1)
	{
		UpdateRouteToolBar();
		return;
	}

	m_pCurBoundRoute = (CIn_OutBoundRoute*)m_ListCtrlExitPositionAndStandFamily.GetItemData(nSelItem);
	SetTreeContent();

	UpdateRouteToolBar();

	*pResult = 0;
}

void CBoundRouteAssignmentDlg::SetTreeContent(void)
{
	m_TreeCtrlRoute.DeleteAllItems();

	if (m_ListCtrlExitPositionAndStandFamily.GetItemCount() <= 0)
	{
		return;
	}

	ASSERT(m_pCurBoundRoute);

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

void CBoundRouteAssignmentDlg::SetChildItemContent(HTREEITEM hParentItem, CRouteItem *pOutboundRouteItem)
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

void CBoundRouteAssignmentDlg::DeleteChildItemInTree(HTREEITEM hItem)
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

void CBoundRouteAssignmentDlg::OnTvnSelchangedTreeRoute(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here

	UpdateRouteToolBar();

	*pResult = 0;
}

void CBoundRouteAssignmentDlg::OnBnClickedButtonSave()
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

void CBoundRouteAssignmentDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	OnBnClickedButtonSave();
	OnOK();
}


void CBoundRouteAssignmentDlg::OnCmdNewFlightTime()
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

void CBoundRouteAssignmentDlg::OnCmdEditFlightTime()
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

void CBoundRouteAssignmentDlg::OnCmdDelFlightTime()
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

void CBoundRouteAssignmentDlg::OnAddTimeRange()
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

void CBoundRouteAssignmentDlg::OnEditTimeRange()
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

void CBoundRouteAssignmentDlg::OnDelTimeRange()
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

void CBoundRouteAssignmentDlg::OnContextMenu(CWnd* pWnd, CPoint point)
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

void CBoundRouteAssignmentDlg::OnTvnSelchangedTreeFlighttypeTime(NMHDR *pNMHDR, LRESULT *pResult)
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
		m_TreeCtrlRoute.DeleteAllItems();
		m_cmbRouteType.EnableWindow(FALSE);
		SetCtrlState(FALSE);
	}
	else
	{
		m_cmbRouteType.EnableWindow(TRUE);
		m_pCurTimeAssignment = (CTimeRangeRouteAssignment*)m_wndTreeFltTime.GetItemData(hItem);
		InitInboundRouteAssignmentState();

	}

	*pResult = 0;
}

void CBoundRouteAssignmentDlg::InitInboundRouteAssignmentState()
{
	HTREEITEM hSelItem = m_wndTreeFltTime.GetSelectedItem();
	if (hSelItem == NULL || m_wndTreeFltTime.GetParentItem(hSelItem) == NULL)
		return;

	CTimeRangeRouteAssignment* pTimeAssignment = (CTimeRangeRouteAssignment*)m_wndTreeFltTime.GetItemData(hSelItem);
	BOUNDROUTEASSIGNMENTSTATUS raStatus = pTimeAssignment->GetRouteAssignmentStatus();

	m_cmbRouteType.SetCurSel((int)raStatus);
	if (BOUNDROUTEASSIGNMENT_SHORTPATH == raStatus || BOUNDROUTEASSIGNMENT_TIMESHORTPATH ==  raStatus)
	{	
		SetCtrlState(FALSE);
	}
	else
	{
		SetCtrlState(TRUE);
		SetListContent();	
	}

	UpdateRunwayExitAndStandToolBar();		
	UpdateRouteToolBar();
}

void CBoundRouteAssignmentDlg::OnCbnSelchangeComboRoutechoice()
{
	// TODO: Add your control notification handler code here
	HTREEITEM hSelItem = m_wndTreeFltTime.GetSelectedItem() ;
	if(hSelItem == NULL || m_wndTreeFltTime.GetParentItem(hSelItem) == NULL)
		return;

	BOUNDROUTEASSIGNMENTSTATUS raStatus = (BOUNDROUTEASSIGNMENTSTATUS)m_cmbRouteType.GetItemData(m_cmbRouteType.GetCurSel()) ;

	CTimeRangeRouteAssignment* pTimeAssignment = (CTimeRangeRouteAssignment*)m_wndTreeFltTime.GetItemData(hSelItem) ;
	if(pTimeAssignment == NULL )
		return ;

	pTimeAssignment->SetRouteAssignmentStatus(raStatus);

	if (BOUNDROUTEASSIGNMENT_SHORTPATH == raStatus || BOUNDROUTEASSIGNMENT_TIMESHORTPATH ==  raStatus)
	{	
		SetCtrlState(FALSE);
	}
	else
	{
		SetCtrlState(TRUE);
		m_TreeCtrlRoute.DeleteAllItems();	
	}

	UpdateRunwayExitAndStandToolBar();		
	UpdateRouteToolBar();
}

LRESULT CBoundRouteAssignmentDlg::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
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
			if(wParam ==ID_WND_SPLITTER)
			{
				SPC_NMHDR* pHdr = (SPC_NMHDR*) lParam;
				DoResize(pHdr->delta,wParam);
			}	
		}
		break;
	default:
		break;
	}
	 return CDialog::DefWindowProc(message,wParam,lParam);
}

void CBoundRouteAssignmentDlg::DoResize(int delta,UINT nIDSplitter)
{
	CRect rectWnd;
	if(nIDSplitter == ID_WND_SPLITTER)
	{
 		CSplitterControl::ChangeWidth(GetDlgItem(IDC_STATIC_FlIGHTTOOLBAR), delta,CW_LEFTALIGN);
 		CSplitterControl::ChangeWidth(GetDlgItem(IDC_STATIC_FLIGHTTYPE),delta,CW_LEFTALIGN);
 		CSplitterControl::ChangeWidth(&m_wndTreeFltTime, delta,CW_LEFTALIGN);
 
		CRect rect;
		m_static.GetWindowRect(&rect);
		ScreenToClient(&rect);
		m_static.SetWindowPos(NULL,rect.left + delta,rect.top,rect.Width(),rect.Height(),SWP_NOACTIVATE|SWP_NOZORDER);
		m_cmbRouteType.GetWindowRect(&rect);
		ScreenToClient(&rect);
		m_cmbRouteType.SetWindowPos(NULL,rect.left + delta,rect.top,rect.Width() - delta,rect.Height(),SWP_NOACTIVATE|SWP_NOZORDER);
 		CSplitterControl::ChangeWidth(GetDlgItem(IDC_STATIC_ROUTEASSIGNMENT),-delta,CW_RIGHTALIGN);
 		CSplitterControl::ChangeWidth(&m_ToolBarRunwayExitAndStandFamily,-delta,CW_RIGHTALIGN);
 		CSplitterControl::ChangeWidth(&m_ToolBarRoute,-delta,CW_RIGHTALIGN);
 		CSplitterControl::ChangeWidth(&m_ListCtrlExitPositionAndStandFamily,-delta,CW_RIGHTALIGN);
 		CSplitterControl::ChangeWidth(&m_TreeCtrlRoute,-delta,CW_RIGHTALIGN);
	}
}

void CBoundRouteAssignmentDlg::DeletePriorityItem(HTREEITEM hItem)
{
	ASSERT(hItem);
	HTREEITEM hChildItem = m_TreeCtrlRoute.GetChildItem(hItem);
	while(hChildItem)
	{
		m_TreeCtrlRoute.DeleteItem(hChildItem);
		hChildItem = m_TreeCtrlRoute.GetChildItem(hItem);
	}
}

void CBoundRouteAssignmentDlg::InitPrioirtyItemContent(CRoutePriority*pPriority,HTREEITEM hPriorityItem)
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

void CBoundRouteAssignmentDlg::InitPrioirtyItemContent()
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

void CBoundRouteAssignmentDlg::InitChangeConditionItemContent(HTREEITEM hItem)
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

void CBoundRouteAssignmentDlg::InitChangeConditionContent()
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

void CBoundRouteAssignmentDlg::OnCmdNewRoute()
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

/////////////////////////InboundRouteAssignmentDlg///////////////
InboundRouteAssignmentDlg::InboundRouteAssignmentDlg(int nProjID,PFuncSelectFlightType pSelectFlightType,CAirportDatabase *pAirPortdb,LPCTSTR lpszTitle,  CWnd* pParent /*= NULL*/)
:CBoundRouteAssignmentDlg(nProjID,pSelectFlightType,pAirPortdb,lpszTitle, pParent)
{
	m_pBoundRouteAssignment = new CBoundRouteAssignment(INBOUND, pAirPortdb);
	m_pBoundRouteAssignment->ReadData(m_nProjID);
}


InboundRouteAssignmentDlg::~InboundRouteAssignmentDlg()
{

}

void InboundRouteAssignmentDlg::InitListCtrl()
{
	DWORD dwStyle = m_ListCtrlExitPositionAndStandFamily.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_EX_GRIDLINES;
	m_ListCtrlExitPositionAndStandFamily.SetExtendedStyle(dwStyle);

	CStringList strList;
	strList.RemoveAll();
	CString strCaption;

	LV_COLUMNEX lvc;
	lvc.mask = LVCF_WIDTH|LVCF_TEXT;

	strCaption.LoadString(IDS_EXITPOSITION);
	lvc.pszText = (LPSTR)(LPCTSTR)strCaption;
	lvc.cx = 170;
	lvc.fmt = LVCFMT_NOEDIT;
	lvc.csList = &strList;
	m_ListCtrlExitPositionAndStandFamily.InsertColumn(0, &lvc);

	strCaption.LoadString(IDS_STANDFAMILY);
	lvc.pszText = (LPSTR)(LPCTSTR)strCaption;
	lvc.cx = 170;
	lvc.fmt = LVCFMT_NOEDIT;
	m_ListCtrlExitPositionAndStandFamily.InsertColumn(1, &lvc);
}

void InboundRouteAssignmentDlg::SetListContent()
{
	m_ListCtrlExitPositionAndStandFamily.DeleteAllItems();

	if (NULL == m_pBoundRouteAssignment )
		return;

	ASSERT(m_pCurTimeAssignment);

	size_t count = m_pCurTimeAssignment->GetElementCount();
	for (size_t i=0; i<count; i++)
	{
		CIn_OutBoundRoute *pInboundRoute = m_pCurTimeAssignment->GetItem(i);

		//set runway exit name 
		CString strRunwayExitName = "";
		if (pInboundRoute->IsAllRunway())
		{
			strRunwayExitName = "All";
		}
		else
		{
			std::vector<int> vExits;
			vExits.clear();
			pInboundRoute->GetRunwayExitIDList(vExits);
			int nSize = vExits.size();
			for (int j =0; j < nSize; j++)
			{
				int nExitID = vExits.at(j);
				RunwayExit rwyExit;
				rwyExit.ReadData(nExitID);
				if (strRunwayExitName == "")
				{
					strRunwayExitName = rwyExit.GetName();
				}
				else
				{
					strRunwayExitName += ", ";
					strRunwayExitName += rwyExit.GetName();
				}
			}
		}

		int newItem = m_ListCtrlExitPositionAndStandFamily.InsertItem(i, strRunwayExitName);
		m_ListCtrlExitPositionAndStandFamily.SetItemData(i, (DWORD_PTR)pInboundRoute);

		//set stand name 	
		CString strStandFamily = "";
		if (pInboundRoute->IsAllStand())
		{
			strStandFamily = "All";
		}
		else
		{
			std::vector<int> vStandFmailys;
			vStandFmailys.clear();
			pInboundRoute->GetStandGroupIDList(vStandFmailys);
			int nSize = vStandFmailys.size();
			for (int j =0; j < nSize; j++)
			{
				int nStandFamilyID = vStandFmailys.at(j);
				ALTObjectGroup altObjGroup;
				altObjGroup.ReadData(nStandFamilyID);
				ALTObjectID altObjID = altObjGroup.getName();
				if (j==0)
				{
					strStandFamily = altObjID.GetIDString();
				}
				else
				{
					strStandFamily += ", ";
					strStandFamily += altObjID.GetIDString();
				}
			}
		}

		m_ListCtrlExitPositionAndStandFamily.SetItemText(newItem, 1, strStandFamily);
	}
}

void InboundRouteAssignmentDlg::NewOriginAndDestination()
{
	CRunwayExitAndStandFamilySelectDlg dlg(CRunwayExitAndStandFamilySelectDlg::IDD,m_nProjID,  _T(""), _T(""));

	if (dlg.DoModal() != IDOK)
	{
		return;
	}

	CString strRunwayExitName = dlg.GetRunwayExitName();
	CString strStandFamilyName = dlg.GetStandFamilyName();
	BOOL bAllStand = dlg.IsAllStand();
	BOOL bAllRunwayExit = dlg.IsAllRunwayExit();
	std::vector<int> vExits = dlg.GetRunwayExitList();
	std::vector<int> vStands = dlg.GetStandGroupList();


	m_pCurBoundRoute = new CIn_BoundRoute;
	m_pCurBoundRoute->SetAllRunway(bAllRunwayExit);
	m_pCurBoundRoute->SetAllStand(bAllStand);
	m_pCurBoundRoute->SetRunwayExitIDList(vExits);
	m_pCurBoundRoute->SetStandGroupIDList(vStands);

	m_pCurTimeAssignment->AddNewItem(m_pCurBoundRoute);

	int newItem = m_ListCtrlExitPositionAndStandFamily.InsertItem(m_ListCtrlExitPositionAndStandFamily.GetItemCount(), strRunwayExitName);
	m_ListCtrlExitPositionAndStandFamily.SetItemData(newItem, (DWORD_PTR)m_pCurBoundRoute);
	m_ListCtrlExitPositionAndStandFamily.SetItemText(newItem, 1, strStandFamilyName);

	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
}

void InboundRouteAssignmentDlg::EditOriginAndDestination()
{
	if (m_pCurBoundRoute == NULL)
		return;

	int nSelItem = m_ListCtrlExitPositionAndStandFamily.GetCurSel();
	CString strRunwayExitName = m_ListCtrlExitPositionAndStandFamily.GetItemText(nSelItem, 0);
	CString strStandFamilyName = m_ListCtrlExitPositionAndStandFamily.GetItemText(nSelItem, 1);


	CRunwayExitAndStandFamilySelectDlg dlg(CRunwayExitAndStandFamilySelectDlg::IDD,m_nProjID, strRunwayExitName, strStandFamilyName);
	if (dlg.DoModal() != IDOK)
	{
		return;
	}

	strRunwayExitName = dlg.GetRunwayExitName();
	strStandFamilyName = dlg.GetStandFamilyName();
	BOOL bAllStand = dlg.IsAllStand();
	BOOL bAllRunwayExit = dlg.IsAllRunwayExit();
	std::vector<int> vExits = dlg.GetRunwayExitList();
	std::vector<int> vStands = dlg.GetStandGroupList();

	//modify data
	m_pCurBoundRoute->SetAllRunway(bAllRunwayExit);
	m_pCurBoundRoute->SetAllStand(bAllStand);
	m_pCurBoundRoute->SetRunwayExitIDList(vExits);
	m_pCurBoundRoute->SetStandGroupIDList(vStands);

	//m_pCurBoundRoute->SetRunwayAllExitFlag(bRunwayAllFlag);

	m_ListCtrlExitPositionAndStandFamily.SetItemText(nSelItem, 0, strRunwayExitName);
	m_ListCtrlExitPositionAndStandFamily.SetItemText(nSelItem, 1, strStandFamilyName);

	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
}


/////////////////////////OutboudRouteAssignmentDlg///////////////
OutboundRouteAssignmentDlg::OutboundRouteAssignmentDlg(int nProjID,PFuncSelectFlightType pSelectFlightType,CAirportDatabase *pAirPortdb,LPCTSTR lpszTitle,  CWnd* pParent /*= NULL*/)
:CBoundRouteAssignmentDlg(nProjID,pSelectFlightType,pAirPortdb,lpszTitle, pParent)
{
	m_pBoundRouteAssignment = new CBoundRouteAssignment(OUTBOUND, pAirPortdb);
	m_pBoundRouteAssignment->ReadData(m_nProjID);
}

OutboundRouteAssignmentDlg::~OutboundRouteAssignmentDlg()
{

}

void OutboundRouteAssignmentDlg::InitListCtrl()
{
	DWORD dwStyle = m_ListCtrlExitPositionAndStandFamily.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_EX_GRIDLINES;
	m_ListCtrlExitPositionAndStandFamily.SetExtendedStyle(dwStyle);

	CStringList strList;
	strList.RemoveAll();
	CString strCaption;

	LV_COLUMNEX lvc;
	lvc.mask = LVCF_WIDTH|LVCF_TEXT;

	strCaption.LoadString(IDS_STANDFAMILY);
	lvc.pszText = (LPSTR)(LPCTSTR)strCaption;
	lvc.cx = 170;
	lvc.fmt = LVCFMT_NOEDIT;
	lvc.csList = &strList;
	m_ListCtrlExitPositionAndStandFamily.InsertColumn(0, &lvc);

	strCaption.LoadString(IDS_TAKEOFFPOSITION);
	lvc.pszText = (LPSTR)(LPCTSTR)strCaption;
	lvc.cx = 170;
	lvc.fmt = LVCFMT_NOEDIT;
	m_ListCtrlExitPositionAndStandFamily.InsertColumn(1, &lvc);
}

void OutboundRouteAssignmentDlg::SetListContent()
{
	m_ListCtrlExitPositionAndStandFamily.DeleteAllItems();

	if (NULL == m_pBoundRouteAssignment)
		return;

	size_t count = m_pCurTimeAssignment->GetElementCount();
	for (size_t i=0; i<count; i++)
	{
		CIn_OutBoundRoute *pOutboundRoute = m_pCurTimeAssignment->GetItem(i);

		//set stand name 	
		CString strStandFamily = "";
		if (pOutboundRoute->IsAllStand())
		{
			strStandFamily = "All";
		}
		else
		{
			std::vector<int> vStandFmailys;
			vStandFmailys.clear();
			pOutboundRoute->GetStandGroupIDList(vStandFmailys);
			int nSize = vStandFmailys.size();
			for (int j =0; j < nSize; j++)
			{
				int nStandFamilyID = vStandFmailys.at(j);
				ALTObjectGroup altObjGroup;
				altObjGroup.ReadData(nStandFamilyID);
				ALTObjectID altObjID = altObjGroup.getName();
				if (j==0)
				{
					strStandFamily = altObjID.GetIDString();
				}
				else
				{
					strStandFamily += ", ";
					strStandFamily += altObjID.GetIDString();
				}
			}
		}

		int newItem = m_ListCtrlExitPositionAndStandFamily.InsertItem(i, strStandFamily);
		m_ListCtrlExitPositionAndStandFamily.SetItemData(i, (DWORD_PTR)pOutboundRoute);

		//set takeoff position name 
		CString strRunwayExitName = "";
		if (pOutboundRoute->IsAllRunway())
		{
			strRunwayExitName = "All";
		}
		else
		{
			std::vector<int> vExits;
			vExits.clear();
			pOutboundRoute->GetRunwayExitIDList(vExits);
			int nSize = vExits.size();
			for (int j =0; j < nSize; j++)
			{
				int nExitID = vExits.at(j);
				RunwayExit rwyExit;
				rwyExit.ReadData(nExitID);
				if (j==0)
				{
					strRunwayExitName = rwyExit.GetName();
				}
				else
				{
					strRunwayExitName += ", ";
					strRunwayExitName += rwyExit.GetName();
				}
			}
		}

		m_ListCtrlExitPositionAndStandFamily.SetItemText(newItem, 1, strRunwayExitName);
	}
}

void OutboundRouteAssignmentDlg::NewOriginAndDestination()
{
	CSelectStandFamilyAndTakeoffPositionDlg dlg(CSelectStandFamilyAndTakeoffPositionDlg::IDD, m_nProjID, _T(""), _T(""));

	if (dlg.DoModal() != IDOK)
	{
		return;
	}

	CString strRunwayExitName = dlg.GetRunwayExitName();
	CString strStandFamilyName = dlg.GetStandFamilyName();
	BOOL bAllStand = dlg.IsAllStand();
	BOOL bAllRunwayExit = dlg.IsAllRunwayExit();
	std::vector<int> vExits = dlg.GetRunwayExitList();
	std::vector<int> vStands = dlg.GetStandGroupList();


	m_pCurBoundRoute = new COut_BoundRoute;
	m_pCurBoundRoute->SetAllRunway(bAllRunwayExit);
	m_pCurBoundRoute->SetAllStand(bAllStand);
	m_pCurBoundRoute->SetRunwayExitIDList(vExits);
	m_pCurBoundRoute->SetStandGroupIDList(vStands);

	m_pCurTimeAssignment->AddNewItem(m_pCurBoundRoute);

	int newItem = m_ListCtrlExitPositionAndStandFamily.InsertItem(m_ListCtrlExitPositionAndStandFamily.GetItemCount(), strStandFamilyName);
	m_ListCtrlExitPositionAndStandFamily.SetItemData(newItem, (DWORD_PTR)m_pCurBoundRoute);
	m_ListCtrlExitPositionAndStandFamily.SetItemText(newItem, 1, strRunwayExitName);

	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
}

void OutboundRouteAssignmentDlg::EditOriginAndDestination()
{
	if (m_pCurBoundRoute == NULL)
		return;

	int nSelItem = m_ListCtrlExitPositionAndStandFamily.GetCurSel();
	CString strStandFamilyName = m_ListCtrlExitPositionAndStandFamily.GetItemText(nSelItem, 0);
	CString strRunwayExitName = m_ListCtrlExitPositionAndStandFamily.GetItemText(nSelItem, 1);

	CSelectStandFamilyAndTakeoffPositionDlg dlg(CSelectStandFamilyAndTakeoffPositionDlg::IDD, m_nProjID, strRunwayExitName,strStandFamilyName);
	if (dlg.DoModal() != IDOK)
	{
		return;
	}

	strRunwayExitName = dlg.GetRunwayExitName();
	strStandFamilyName = dlg.GetStandFamilyName();
	BOOL bAllStand = dlg.IsAllStand();
	BOOL bAllRunwayExit = dlg.IsAllRunwayExit();
	std::vector<int> vExits = dlg.GetRunwayExitList();
	std::vector<int> vStands = dlg.GetStandGroupList();

	//modify data
	m_pCurBoundRoute->SetAllRunway(bAllRunwayExit);
	m_pCurBoundRoute->SetAllStand(bAllStand);
	m_pCurBoundRoute->SetRunwayExitIDList(vExits);
	m_pCurBoundRoute->SetStandGroupIDList(vStands);

	//m_pCurBoundRoute->SetRunwayAllExitFlag(bRunwayAllFlag);

	m_ListCtrlExitPositionAndStandFamily.SetItemText(nSelItem, 0, strStandFamilyName);
	m_ListCtrlExitPositionAndStandFamily.SetItemText(nSelItem, 1, strRunwayExitName);

	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
}
void OutboundRouteAssignmentDlg::InitComboBox()
{
	int  index = 0 ;
	index = m_cmbRouteType.AddString(_T("Shortest path to runway"));
	m_cmbRouteType.SetItemData(index ,(DWORD_PTR) BOUNDROUTEASSIGNMENT_SHORTPATH) ;

	index = m_cmbRouteType.AddString(_T("Time shortest path to runway")) ;
	m_cmbRouteType.SetItemData(index ,(DWORD_PTR) BOUNDROUTEASSIGNMENT_TIMESHORTPATH);

	index = m_cmbRouteType.AddString(_T("Preferred route"));
	m_cmbRouteType.SetItemData(index,(DWORD_PTR) BOUNDROUTEASSIGNMENT_PREFERREDROUTE);
}