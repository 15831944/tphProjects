#include "StdAfx.h"
#include "resource.h"
#include "DlgLandingRunwayExit.h"

#include "InputAirside/RunwayExitStrategies.h"
#include "InputAirside/FlightTypeRunwayExitStrategyItem.h"
#include "InputAirside/TimeRangeRunwayExitStrategyItem.h"
#include "InputAirside/LandingRunwayExitStrategies.h"
#include "InputAirside/RunwayExitStandStrategyItem.h"

#include "InputAirside/ALTAirport.h"

#include "../Database/ARCportDatabaseConnection.h"

#include "DlgStandFamily.h"

#include "DlgTimeRange.h"
#include "DlgSelectRunway.h"
#include ".\dlglandingrunwayexit.h"
#include "../InputAirside/Taxiway.h"
#include "..\InputAirside\InputAirside.h"
#include "..\Common\AirportDatabase.h"

#include "DlgSelectExitOfRunway.h"
#include "DlgTaxiwaySegmentSelect.h"

static const UINT ID_NEW_FLTTYPE = 10;
static const UINT ID_DEL_FLTTYPE = 11;
static const UINT ID_EDIT_FLTTYPE = 12;
static const UINT ID_NEW_RUNWAY = 20;
static const UINT ID_DEL_RUNWAY = 21;
static const UINT ID_EDIT_RUNWAY = 22;
static const UINT ID_ADD_TAXISEG = 23;
static const UINT ID_EDIT_TAXISEG = 24;
static const UINT ID_DEL_TAXISEG = 25;

static const char* strComboxString[] = {"No","Yes"};
CDlgLandingRunwayExit::CDlgLandingRunwayExit(int nProjID, PFuncSelectFlightType pSelectFlightType, InputAirside* pInputAirside, CWnd* pParent/* = NULL*/)
: CXTResizeDialog(IDD_LANDIINGRUNWAYEXIT, pParent)
,m_nProjID(nProjID)
,m_pSelectFlightType(pSelectFlightType)
,m_hTimeRangeTreeItem(NULL)
{
	m_pAirportDB = pInputAirside->m_pAirportDB;
}

CDlgLandingRunwayExit::~CDlgLandingRunwayExit()
{
}

void CDlgLandingRunwayExit::DoDataExchange(CDataExchange* pDX)
{
	CXTResizeDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_ASSIGNSTRATEGY, m_wndCmbExitStrategy);
	DDX_Control(pDX, IDC_TREE_FLTTIME, m_wndTreeFltTime);
	DDX_Control(pDX, IDC_TREE_RUNWAYEXIT, m_wndTreeExitStrategy);
	DDX_Control(pDX, IDC_RADIO_PROB_BASED, m_radioProbBased);
	DDX_Control(pDX, IDC_RADIO_PRIO_BASED, m_radioPrioBased);
}

BEGIN_MESSAGE_MAP(CDlgLandingRunwayExit, CXTResizeDialog)
	ON_NOTIFY_EX_RANGE(TTN_NEEDTEXT,0,0xFFFF,OnToolTipText )
	//ON_WM_CREATE()
	
	ON_BN_CLICKED(IDC_BUTTON_SAVE, OnMsgSave)
	ON_COMMAND(ID_TOOLBAR_FLIGHT_TYPE_ADD, OnMsgNewFltType)
	ON_COMMAND(ID_TOOLBAR_FLIGHT_TYPE_DEL, OnMsgDelFltType)
	ON_COMMAND(ID_TOOLBAR_FLIGHT_TYPE_EDIT, OnMsgEditFltType)

	ON_COMMAND(ID_NEW_RUNWAY, OnMsgNewRunway)
	ON_COMMAND(ID_DEL_RUNWAY, OnMsgDelRunway)
	//ON_COMMAND(ID_EDIT_RUNWAY, OnMsgEditRunway)

	ON_COMMAND(ID_NEW_STANDGROUP, OnMsgAddStandFamily)
	ON_COMMAND(ID_CUT_STANDGROUP, OnMsgDelStandFamily)
	ON_COMMAND(ID_MODIFY_STANDGROUP, OnMsgEditStandFamily)

	/*ON_COMMAND(ID_ADD_TIMERANGE, OnMsgAddTimeRange)
	ON_COMMAND(ID_DEL_TIMERANGE, OnMsgDelTimeRange)
	ON_COMMAND(ID_EDIT_TIMERANGE, OnMsgEditTimeRange)*/

	ON_COMMAND(ID_NEW_TIMERANGE, OnMsgAddTimeRange)
	ON_COMMAND(ID_CUT_TIMERANGE, OnMsgDelTimeRange)
	ON_COMMAND(ID_MODIFY_TIMERANGE, OnMsgEditTimeRange)


	ON_BN_CLICKED(IDC_RADIO_PROB_BASED, OnBnClickedProbabilityBased)
	ON_BN_CLICKED(IDC_RADIO_PRIO_BASED, OnBnClickedPriorityBased)

	ON_COMMAND(ID_ADD_EXIT_PRIORITY, OnMsgAddExitPriority)
	ON_COMMAND(ID_DEL_EXIT_PRIORITY, OnMsgDelExitPriority)

	ON_COMMAND(ID_BACKTRACK, OnMsgBacktrack)
	ON_COMMAND(ID_NOBACKTRACK, OnMsgNoBacktrack)
	ON_COMMAND(ID_ADD_TAXISEG,OnAddTaxiwaySegment)
	ON_COMMAND(ID_EDIT_TAXISEG,OnEditTaxiwaySegment)
	ON_COMMAND(ID_DEL_TAXISEG,OnDeleteTaxiwaySegment)
	
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_FLTTIME, OnSelChangedFltTimeTree)
	ON_WM_CONTEXTMENU()
	ON_CBN_SELCHANGE(IDC_COMBO_ASSIGNSTRATEGY, OnMsgExitStrategyChange)
	ON_WM_SIZE() 
	//ON_WM_MOUSEMOVE()
	

	//	ON_WM_COPYDATA()
END_MESSAGE_MAP()
void CDlgLandingRunwayExit::OnSize(UINT nType, int cx, int cy)
{
	CXTResizeDialog::OnSize(nType,cx,cy) ;
}
int CDlgLandingRunwayExit::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CXTResizeDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	
	
	return 0;
}
BOOL CDlgLandingRunwayExit::PreTranslateMessage(MSG* pMsg)
{
	m_Tips.RelayEvent(pMsg);
	return CXTResizeDialog::PreTranslateMessage(pMsg);
}
BOOL CDlgLandingRunwayExit::OnToolTipText(UINT, NMHDR* pNMHDR, LRESULT* pResult)

{
	ASSERT(pNMHDR->code == TTN_NEEDTEXTA );

	
	TOOLTIPTEXTA* pTTTA = (TOOLTIPTEXTA*)pNMHDR;
	TOOLTIPTEXTW* pTTTW = (TOOLTIPTEXTW*)pNMHDR;

	CString strTipText;
	UINT nID = pNMHDR->idFrom;
	
	HTREEITEM hItem;
	hItem=m_wndTreeFltTime.GetSelectedItem();
	if (pNMHDR->code == TTN_NEEDTEXTA && (pTTTA->uFlags & TTF_IDISHWND))
	{
		
		nID = ::GetDlgCtrlID((HWND)nID);
	}


	if (nID != 0&& (nID==ID_TOOLBAR_FLIGHT_TYPE_ADD ||nID==ID_TOOLBAR_FLIGHT_TYPE_DEL||nID==ID_TOOLBAR_FLIGHT_TYPE_EDIT)) 
	{
		if (hItem==NULL)
		{
			strTipText="Add Flight Type";
		}
		else if (GetFltTypeTreeItemByType(TREENODE_FLTTYPE,hItem))
		{
			if (nID==ID_TOOLBAR_FLIGHT_TYPE_EDIT)
			{
				strTipText="Edit Flight Type";
			}
			if (nID==ID_TOOLBAR_FLIGHT_TYPE_DEL)
			{
				strTipText="Delete  Flight Type";
			}
			if (nID==ID_TOOLBAR_FLIGHT_TYPE_ADD)
			{
				strTipText="Add Stand Group";
			}
		}
		else if (GetFltTypeTreeItemByType(TREENODE_STAND,hItem))
		{
			if (nID==ID_TOOLBAR_FLIGHT_TYPE_EDIT)
			{
				strTipText="Edit Stand Group";
			}
			if (nID==ID_TOOLBAR_FLIGHT_TYPE_DEL)
			{
				strTipText="Delete Stand Group";
			}
			if (nID==ID_TOOLBAR_FLIGHT_TYPE_ADD)
			{
				strTipText="Add Time Range";
			}
		}
		else if (GetFltTypeTreeItemByType(TREENODE_TIMERANGE,hItem))
		{
			if (nID==ID_TOOLBAR_FLIGHT_TYPE_EDIT)
			{
				strTipText="Edit Time Range";
			}
			if (nID==ID_TOOLBAR_FLIGHT_TYPE_DEL)
			{
				strTipText="Delete Time Range";
			}
			
		}
		
        
		if (pNMHDR->code == TTN_NEEDTEXTA)
		{
			lstrcpyn(pTTTA->szText, strTipText, sizeof(pTTTA->szText));
		}
		else
		{
			_mbstowcsz(pTTTW->szText, strTipText, sizeof(pTTTW->szText));
		}


		*pResult = 0;

		
		::SetWindowPos(pNMHDR->hwndFrom, HWND_TOP, 0, 0, 0, 0,SWP_NOACTIVATE|
			SWP_NOSIZE|SWP_NOMOVE|SWP_NOOWNERZORDER); 
		return TRUE;
	}
	return TRUE;
}
void CDlgLandingRunwayExit::OnMouseMove(UINT nFlags, CPoint point)
{
	CRect rect;
	CPoint pt=point;
	m_wndFltToolbar.ScreenToClient(&point);
	m_wndFltToolbar.GetWindowRect(&rect);
	
}
void CDlgLandingRunwayExit::ShowTips(const CString & tmpTips)
{
	
		m_Tips.DelTool(&m_wndFltToolbar);
		m_Tips.AddTool(&m_wndFltToolbar,tmpTips);
}



BOOL CDlgLandingRunwayExit::OnInitDialog()
{
	CXTResizeDialog::OnInitDialog();
	if (!m_wndFltToolbar.CreateEx(this, TBSTYLE_FLAT,WS_CHILD|WS_VISIBLE|CBRS_ALIGN_TOP|CBRS_TOOLTIPS)
		|| !m_wndFltToolbar.LoadToolBar(IDR_TOOLBAR_FLIGHT_TYPE))
	{
		return -1;
	}


	if (!m_wndRunwayToolbar.CreateEx(this,TBSTYLE_FLAT,WS_CHILD|WS_VISIBLE|CBRS_ALIGN_TOP,CRect(0,0,0,0),1001)
		|| !m_wndRunwayToolbar.LoadToolBar(IDR_TOOLBAR_ADD_DEL_EDIT))
	{
		return -1;
	}


	CToolBarCtrl& runwayToolbar = m_wndRunwayToolbar.GetToolBarCtrl();
	runwayToolbar.SetCmdID(0, ID_NEW_RUNWAY);
	runwayToolbar.SetCmdID(1, ID_DEL_RUNWAY);
	runwayToolbar.SetCmdID(2, ID_EDIT_RUNWAY);
	runwayToolbar.HideButton(ID_EDIT_RUNWAY,TRUE);
	InitExitStrategyComboBox();

	// initialize radio buttons
	m_radioProbBased.SetCheck(BST_CHECKED);
	m_radioPrioBased.SetCheck(BST_UNCHECKED);


	// set the position of the flt toolbar
	CRect rectFltToolbar;
	m_wndTreeFltTime.GetWindowRect(&rectFltToolbar);
	ScreenToClient(&rectFltToolbar);
	rectFltToolbar.top -= 26;
	rectFltToolbar.bottom = rectFltToolbar.top + 22;
	rectFltToolbar.left += 2;
	m_wndFltToolbar.MoveWindow(&rectFltToolbar);
    

	if (m_Tips.Create(&m_wndFltToolbar,TTS_ALWAYSTIP)&& m_Tips.AddTool(&m_wndFltToolbar))
	{
		//m_Tips.SendMessage(TTM_SETMAXTIPWIDTH,0,300);
	}
	m_Tips.Activate(TRUE);

	// set the position of the runway toolbar
	CRect rectRunwayToolbar;
	m_wndTreeExitStrategy.GetWindowRect(&rectRunwayToolbar);
	ScreenToClient(&rectRunwayToolbar);
	rectRunwayToolbar.top -= 26;
	rectRunwayToolbar.bottom = rectRunwayToolbar.top + 22;
	rectRunwayToolbar.left += 4;
	m_wndRunwayToolbar.MoveWindow(&rectRunwayToolbar);

	m_wndRunwayToolbar.GetToolBarCtrl().EnableButton(ID_NEW_RUNWAY);
	m_wndRunwayToolbar.GetToolBarCtrl().EnableButton(ID_DEL_RUNWAY);
	//m_wndRunwayToolbar.GetToolBarCtrl().HideButton(ID_EDIT_RUNWAY);

	SetResize(IDC_STATIC_FLIGHTTY, SZ_TOP_LEFT, SZ_BOTTOM_CENTER);
	SetResize(m_wndFltToolbar.GetDlgCtrlID(), SZ_TOP_LEFT, SZ_TOP_LEFT);
	SetResize(IDC_TREE_FLTTIME, SZ_TOP_LEFT, SZ_BOTTOM_CENTER);

	SetResize(IDC_STATIC_STRATEGY_TITLE, SZ_TOP_CENTER, SZ_BOTTOM_RIGHT);
	SetResize(IDC_COMBO_ASSIGNSTRATEGY, SZ_TOP_CENTER, SZ_TOP_RIGHT);
	SetResize(IDC_RADIO_PROB_BASED, SZ_TOP_CENTER, SZ_TOP_RIGHT);
	SetResize(IDC_RADIO_PRIO_BASED, SZ_TOP_CENTER, SZ_TOP_RIGHT);
	SetResize(IDC_STATIC_RUNWAYLST, SZ_TOP_CENTER, SZ_TOP_RIGHT);
	SetResize(m_wndRunwayToolbar.GetDlgCtrlID(), SZ_TOP_CENTER, SZ_TOP_CENTER);
	SetResize(IDC_TREE_RUNWAYEXIT, SZ_TOP_CENTER, SZ_BOTTOM_RIGHT);

	SetResize(IDC_BUTTON_SAVE, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);
	SetResize(IDOK, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);
	SetResize(IDCANCEL, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);
	ReadData();
	ResetFltTimeTreeCtrlContent();
	UpdateState();
	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),TRUE);
	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),FALSE);
	return TRUE;
}

void CDlgLandingRunwayExit::ReadData()
{
	try
	{
		m_inputAirside.InitData(m_nProjID);
	}
	catch(CADOException&e)
	{
		e.ErrorMessage();
		//MessageBox(_T("Error while initialize airside object."));
	}
	m_pStrategies = new LandingRunwayExitStrategies(m_nProjID);
	m_pStrategies->SetAirportDB(m_pAirportDB);
	m_pStrategies->ReadData();

}

void CDlgLandingRunwayExit::ResetExitStrategyContents()
{
	m_wndTreeExitStrategy.DeleteAllItems();

	HTREEITEM hItem;
	if( !GetFltTypeTreeItemByType(TREENODE_TIMERANGE, hItem) )
		return;

	TimeRangeRunwayExitStrategyItem* pTimeRangeItem = (TimeRangeRunwayExitStrategyItem*)m_wndTreeFltTime.GetItemData(hItem);
	ASSERT(pTimeRangeItem != NULL);

	ResetExitStrategyContents(pTimeRangeItem);

}

void CDlgLandingRunwayExit::ResetExitStrategyContents( TimeRangeRunwayExitStrategyItem* pTimeRangeItem )
{
	m_wndTreeExitStrategy.DeleteAllItems();
	if (pTimeRangeItem->getStrategyType() != ManagedExit)
	{
		return;
	}
	m_wndTreeExitStrategy.SetRedraw(FALSE);

	if (pTimeRangeItem->getStrategyManagedExitMode() == TimeRangeRunwayExitStrategyItem::ProbabilityBased)
	{
		RunwayMarkExitStrategyVector& vRunwayMarkExitItems = pTimeRangeItem->GetRunwayMarkExitItems();
		RunwayMarkExitStrategyIter iter = vRunwayMarkExitItems.begin();
		for (;iter != vRunwayMarkExitItems.end(); iter++)
		{
			AddRunwayExitItemToTree(*iter);
		}
	} 
	else if (pTimeRangeItem->getStrategyManagedExitMode() == TimeRangeRunwayExitStrategyItem::PriorityBased)
	{
		RunwayExitStrategyPriorityiesVector& vRunwayPrioritiesItems = pTimeRangeItem->GetRunwayExitPriorities();
		RunwayExitStrategyPrioritiesIter iter = vRunwayPrioritiesItems.begin();
		for (;iter != vRunwayPrioritiesItems.end(); iter++)
		{
			AddRunwayExitPrioritiesItemToTree(*iter);
		}
	}
	else
	{
		ASSERT(FALSE);
	}

	m_wndTreeExitStrategy.SetRedraw(TRUE);
}

void CDlgLandingRunwayExit::OnSelChangedFltTimeTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	if(GetFltTypeTreeItemByType(TREENODE_TIMERANGE, m_hTimeRangeTreeItem))
	{
		TimeRangeRunwayExitStrategyItem *pTimeRangeItem = 
			(TimeRangeRunwayExitStrategyItem*)m_wndTreeFltTime.GetItemData(m_hTimeRangeTreeItem);
		m_wndCmbExitStrategy.SetCurSel( (int)pTimeRangeItem->getStrategyType());
		m_radioProbBased.SetCheck(
			pTimeRangeItem->getStrategyManagedExitMode() == TimeRangeRunwayExitStrategyItem::ProbabilityBased);
		m_radioPrioBased.SetCheck(
			pTimeRangeItem->getStrategyManagedExitMode() == TimeRangeRunwayExitStrategyItem::PriorityBased);
	}

	ResetExitStrategyContents();

	UpdateState();
}

BOOL CDlgLandingRunwayExit::GetFltTypeTreeItemByType(FltTypeTreeNodeType nodeType, HTREEITEM& hTreeItem)
{
	hTreeItem = NULL;
	HTREEITEM hTreeItemSel = m_wndTreeFltTime.GetSelectedItem();
	if (hTreeItemSel == NULL)
		return FALSE;

	int nLevel = 0;
	for(HTREEITEM hItemTmp = hTreeItemSel;hItemTmp;nLevel++)
	{
		hItemTmp = m_wndTreeFltTime.GetParentItem(hItemTmp);
	}

	if (nLevel - 1 == nodeType)
	{
		hTreeItem = hTreeItemSel;
		return TRUE;
	}

	return FALSE;
}

BOOL CDlgLandingRunwayExit::GetRunwayTreeItemByType(RunwayTreeNodeType nodeType, HTREEITEM& hTreeItem)
{
	// calc levels
	HTREEITEM hTreeItemSel = m_wndTreeExitStrategy.GetSelectedItem();
	if (hTreeItemSel == NULL)
		return FALSE;
	int nLevel = 0;
	for(HTREEITEM hItemTmp = hTreeItemSel;hItemTmp;nLevel++)
	{
		hItemTmp = m_wndTreeExitStrategy.GetParentItem(hItemTmp);
	}

	// type
	HTREEITEM hFltTypeTreeItem;
	if (FALSE == GetFltTypeTreeItemByType(TREENODE_TIMERANGE, hFltTypeTreeItem))
	{
		return FALSE;
	}
	TimeRangeRunwayExitStrategyItem* pTimeRangeItem =
		(TimeRangeRunwayExitStrategyItem*)m_wndTreeFltTime.GetItemData(hFltTypeTreeItem);
	if (pTimeRangeItem->getStrategyManagedExitMode() == TimeRangeRunwayExitStrategyItem::PriorityBased)
	{
		nLevel += TREENODE_PRIO_RUNWAY;
	}

	if (nLevel - 1 == nodeType)
	{
		hTreeItem = hTreeItemSel;
		return TRUE;
	}

	return FALSE;
}



void CDlgLandingRunwayExit::OnMsgNewFltType()
{
	if (m_pSelectFlightType == NULL)
		return;
	HTREEITEM hTreeItem;
	FlightConstraint fltType;
	


// 	CString strFltType;
// 	fltType.screenPrint(strFltType);

	// init default value
	hTreeItem=m_wndTreeFltTime.GetSelectedItem();
	if (hTreeItem==NULL)
	{
		if (FALSE == (*m_pSelectFlightType)(NULL, fltType))
		{
			return;
		}
		TimeRangeRunwayExitStrategyItem* pTimeRangeItem = new TimeRangeRunwayExitStrategyItem(-1);

		RunwayExitStandStrategyItem* pStandStrategyItem = new RunwayExitStandStrategyItem(-1);
		pStandStrategyItem->AddTimeRangeRWAssignStrategyItem(pTimeRangeItem);

		FlightTypeRunwayExitStrategyItem* pNewItem = new FlightTypeRunwayExitStrategyItem(m_nProjID);
		pNewItem->AddRunwayExitStandStrategyItem(pStandStrategyItem);
		pNewItem->SetAirportDatabase(m_pAirportDB);
		pNewItem->SetFltType(fltType);

		m_pStrategies->AddStrategyItem(pNewItem);

		// 	m_wndTreeFltTime.SetImageList(m_wndTreeFltTime.GetImageList(TVSIL_NORMAL),TVSIL_NORMAL);

		// 	CString strTmp;
		// 	strTmp.Format(_T("Flight Type: %s"), strFltType);
		COOLTREE_NODE_INFO cni;
		CCoolTree::InitNodeInfo(this, cni);
		// 	cni.nt = NT_NORMAL;
		// 	cni.net = NET_STATIC;
		// 	HTREEITEM hItem = m_wndTreeFltTime.InsertItem(strTmp, cni, FALSE);
		// 	m_wndTreeFltTime.SetItemData(hItem, (DWORD_PTR)pNewItem);
		AddOneExitStrategyToTree(pNewItem, cni);
	}
	else if (GetFltTypeTreeItemByType(TREENODE_STAND,hTreeItem)&&hTreeItem)
	{
		OnMsgAddTimeRange();
	}
	else if(GetFltTypeTreeItemByType(TREENODE_FLTTYPE,hTreeItem)&&hTreeItem)
	{
		OnMsgAddStandFamily();
	}
	
}

void CDlgLandingRunwayExit::OnMsgDelFltType()
{
	HTREEITEM hItem;
	hItem=m_wndTreeFltTime.GetSelectedItem();

	if(GetFltTypeTreeItemByType(TREENODE_FLTTYPE, hItem))
	{
			
		FlightTypeRunwayExitStrategyItem* pItem = (FlightTypeRunwayExitStrategyItem*)m_wndTreeFltTime.GetItemData(hItem);
		m_pStrategies->DeleteStrategyItem(pItem);

		m_wndTreeFltTime.DeleteItem(hItem);
		ResetExitStrategyContents();
	}
	else if(GetFltTypeTreeItemByType(TREENODE_STAND,hItem))
	{
		OnMsgDelStandFamily();
	}
	else if (GetFltTypeTreeItemByType(TREENODE_TIMERANGE,hItem))
	{
		OnMsgDelTimeRange();
	}
}

void CDlgLandingRunwayExit::OnMsgEditFltType()
{
	HTREEITEM hTreeItem;
	if(GetFltTypeTreeItemByType(TREENODE_FLTTYPE, hTreeItem))
	{
		FlightTypeRunwayExitStrategyItem * pItem =
			(FlightTypeRunwayExitStrategyItem *)m_wndTreeFltTime.GetItemData(hTreeItem);


		if (m_pSelectFlightType == NULL)
			return;
		FlightConstraint fltType;
		if (FALSE == (*m_pSelectFlightType)(NULL, fltType))
		{
			return;
		}

		CString strFltType;
		fltType.screenPrint(strFltType);

		pItem->SetFltType(fltType);

		CString strTmp;
		strTmp.Format(_T("Flight Type: %s"), strFltType);
		m_wndTreeFltTime.SetItemText(hTreeItem, strTmp);
	}
	else if (GetFltTypeTreeItemByType(TREENODE_TIMERANGE,hTreeItem))
	{
		OnMsgEditTimeRange();
	}
	else if (GetFltTypeTreeItemByType(TREENODE_STAND,hTreeItem))
	{
		OnMsgEditStandFamily();
	}
		
	
}

void CDlgLandingRunwayExit::OnMsgAddTimeRange()
{
	HTREEITEM hTreeItem;
	if(!GetFltTypeTreeItemByType(TREENODE_STAND, hTreeItem))
		return;
	m_wndTreeFltTime.SelectItem(hTreeItem);

	ElapsedTime etStart, etEnd;
	CDlgTimeRange dlg(etStart, etEnd);
	if(IDOK!= dlg.DoModal())
		return;
	TimeRange tRange(dlg.GetStartTime(), dlg.GetEndTime());

	CString strTimeRange;
	strTimeRange.Format("%s - %s", dlg.GetStartTimeString(), dlg.GetEndTimeString());

	HTREEITEM hItem = m_wndTreeFltTime.GetChildItem(hTreeItem);
	while (hItem)
	{
		TimeRangeRunwayExitStrategyItem* pTimeRangeItem = (TimeRangeRunwayExitStrategyItem*)m_wndTreeFltTime.GetItemData(hItem);
		if (pTimeRangeItem->getEndTime() <= tRange.GetStartTime() || pTimeRangeItem->getStartTime() >= tRange.GetEndTime())
		{
			hItem = m_wndTreeFltTime.GetNextItem(hItem , TVGN_NEXT);
		}
		else
		{
			MessageBox(_T("Time Range: \" ") +strTimeRange+ _T(" \" conflict."));
			return;
		}
	
	}

	RunwayExitStandStrategyItem* pStandStrategyItem = 
		(RunwayExitStandStrategyItem*)m_wndTreeFltTime.GetItemData(hTreeItem);
	ASSERT(pStandStrategyItem != NULL);

	TimeRangeRunwayExitStrategyItem *pNewTimeRangeItem = new TimeRangeRunwayExitStrategyItem(pStandStrategyItem->getID());
	pNewTimeRangeItem->SetTimeRange(tRange);
	pStandStrategyItem->AddTimeRangeRWAssignStrategyItem(pNewTimeRangeItem);

	m_wndTreeFltTime.SetRedraw(FALSE);

	COOLTREE_NODE_INFO cni;
	CCoolTree::InitNodeInfo(this, cni);
	AddOneTimeRangeToTree(pNewTimeRangeItem, cni, hTreeItem);

	m_wndTreeFltTime.Expand(hTreeItem, TVE_EXPAND);
	m_wndTreeFltTime.SetRedraw(TRUE);
}

void CDlgLandingRunwayExit::OnMsgEditTimeRange()
{
	HTREEITEM hTreeItem;
	if(!GetFltTypeTreeItemByType(TREENODE_TIMERANGE, hTreeItem))
		return;
	m_wndTreeFltTime.SelectItem(hTreeItem);
	HTREEITEM hParentTreeItem = m_wndTreeFltTime.GetParentItem(hTreeItem);

	TimeRangeRunwayExitStrategyItem *pTimeRangeItem = 
		(TimeRangeRunwayExitStrategyItem *)m_wndTreeFltTime.GetItemData(hTreeItem);
	TimeRange trValue = pTimeRangeItem->GetTimeRange();
	CDlgTimeRange dlg(trValue.GetStartTime(), trValue.GetEndTime());
	if(IDOK!= dlg.DoModal())
		return;

	CString strTimeRange;
	strTimeRange.Format("%s - %s", dlg.GetStartTimeString(), dlg.GetEndTimeString());
	TimeRange tRange(dlg.GetStartTime(), dlg.GetEndTime());

	HTREEITEM hItem = m_wndTreeFltTime.GetChildItem(hParentTreeItem);
	while (hItem)
	{
		if(hItem != hTreeItem)
		{
			TimeRangeRunwayExitStrategyItem* pTimeRangeItem = (TimeRangeRunwayExitStrategyItem*)m_wndTreeFltTime.GetItemData(hItem);
			if (pTimeRangeItem->getEndTime() <= tRange.GetStartTime() || pTimeRangeItem->getStartTime() >= tRange.GetEndTime())
			{
				hItem = m_wndTreeFltTime.GetNextItem(hItem , TVGN_NEXT);
			}
			else
			{
				MessageBox(_T("Time Range: \" ") +strTimeRange+ _T(" \" conflict."));
				//	break;
				return;
			}
		}
		hItem = m_wndTreeFltTime.GetNextItem(hItem , TVGN_NEXT);
	}

	pTimeRangeItem->SetTimeRange(tRange);

	m_wndTreeFltTime.SetItemText(hTreeItem, strTimeRange);
}

void CDlgLandingRunwayExit::OnMsgDelTimeRange()
{
	HTREEITEM hTreeItem;
	if(!GetFltTypeTreeItemByType(TREENODE_TIMERANGE, hTreeItem))
		return;
	HTREEITEM hParantItem = m_wndTreeFltTime.GetParentItem(hTreeItem);

	RunwayExitStandStrategyItem* pStandStrategyItem = 
		(RunwayExitStandStrategyItem*)m_wndTreeFltTime.GetItemData(hParantItem);
	ASSERT(pStandStrategyItem != NULL);

	TimeRangeRunwayExitStrategyItem *pTimeRangeItem = 
		(TimeRangeRunwayExitStrategyItem *)m_wndTreeFltTime.GetItemData(hTreeItem);

	pStandStrategyItem->DeleteTimeRangeRWAssignStrategyItem(pTimeRangeItem);
	m_wndTreeFltTime.DeleteItem(hTreeItem);
}

void CDlgLandingRunwayExit::OnMsgBacktrack()
{
	CString strData = _T("Backtrack Operation");


	HTREEITEM hTreeItem = NULL;
	HTREEITEM hBackTrackItem = NULL;
	if (GetRunwayTreeItemByType(TREENODE_PROB_MARKING, hTreeItem))
	{
		hBackTrackItem = m_wndTreeExitStrategy.GetChildItem(hTreeItem);
		RunwayExitPercentItem* pPercentItem =
			(RunwayExitPercentItem*)m_wndTreeExitStrategy.GetItemData(hTreeItem);
		pPercentItem->SetBacktrack(true);
	}
	else if (GetRunwayTreeItemByType(TREENODE_PRIO_MARKING, hTreeItem))
	{
		//back track doesn't have child item
		HTREEITEM hItem;
		hItem = m_wndTreeExitStrategy.GetChildItem(hTreeItem);
		if (m_wndTreeExitStrategy.GetChildItem(hItem) == NULL)
		{
			hBackTrackItem = m_wndTreeExitStrategy.GetChildItem(hTreeItem);
			RunwayExitPriorityItem* pPriorityItem =
				(RunwayExitPriorityItem*)m_wndTreeExitStrategy.GetItemData(hTreeItem);
			pPriorityItem->setBacktrack(true);
		}
	}
	else
	{
		ASSERT(FALSE);
	}

	m_wndTreeExitStrategy.SetRedraw(FALSE);

	if (hBackTrackItem)
	{
		m_wndTreeExitStrategy.SetItemText( hBackTrackItem,strData);
	}
	else
	{
		COOLTREE_NODE_INFO cni;	
		CCoolTree::InitNodeInfo(this,cni);
		cni.nt=NT_NORMAL; 
		cni.net=NET_STATIC;
		cni.bEnable = FALSE;
		hBackTrackItem = m_wndTreeExitStrategy.InsertItem(strData,cni,FALSE,FALSE,hTreeItem,TVI_FIRST);
	}

	m_wndTreeExitStrategy.Expand(hTreeItem, TVE_EXPAND);
	m_wndTreeExitStrategy.SetRedraw(TRUE);
}

void CDlgLandingRunwayExit::OnMsgNoBacktrack()
{
	HTREEITEM hTreeItem = NULL;
	HTREEITEM hBackTrackItem = NULL;
	if (GetRunwayTreeItemByType(TREENODE_PROB_MARKING, hTreeItem))
	{
		hBackTrackItem = m_wndTreeExitStrategy.GetChildItem(hTreeItem);
		RunwayExitPercentItem* pPercentItem =
			(RunwayExitPercentItem*)m_wndTreeExitStrategy.GetItemData(hTreeItem);
		pPercentItem->SetBacktrack(false);
	}
	else if (GetRunwayTreeItemByType(TREENODE_PRIO_MARKING, hTreeItem))
	{
		//back track doesn't have child item
		HTREEITEM hItem;
		hItem = m_wndTreeExitStrategy.GetChildItem(hTreeItem);
		if (m_wndTreeExitStrategy.GetChildItem(hItem) == NULL)
		{
			hBackTrackItem = m_wndTreeExitStrategy.GetChildItem(hTreeItem);
			RunwayExitPriorityItem* pPriorityItem =
				(RunwayExitPriorityItem*)m_wndTreeExitStrategy.GetItemData(hTreeItem);
			pPriorityItem->setBacktrack(false);
		}
	}
	else
	{
		ASSERT(FALSE);
	}

	m_wndTreeExitStrategy.SetRedraw(FALSE);

	if (hBackTrackItem)
	{
		m_wndTreeExitStrategy.DeleteItem( hBackTrackItem);
	}

	m_wndTreeExitStrategy.SetRedraw(TRUE);

}

void CDlgLandingRunwayExit::OnMsgNewRunway()
{
	// left tree ctrl must select the TIMERANGE node
	HTREEITEM hTreeItem;
	if(!GetFltTypeTreeItemByType(TREENODE_TIMERANGE, hTreeItem))
		return;

	// prompt dialog to input
	CDlgSelectRunway dlg(m_nProjID);
	if(IDOK != dlg.DoModal())
		return;
	int nAirportID = dlg.GetSelectedAirport();
	int nRunwayID = dlg.GetSelectedRunway();
	int nRunwayMarkIndex = (int)dlg.GetSelectedRunwayMarkingIndex();


	TimeRangeRunwayExitStrategyItem *pTimeRangeItem =
		(TimeRangeRunwayExitStrategyItem*)m_wndTreeFltTime.GetItemData(hTreeItem);

	if (pTimeRangeItem->getStrategyManagedExitMode() == TimeRangeRunwayExitStrategyItem::ProbabilityBased)
	{
		AddNewRunwayMarkExitItem(pTimeRangeItem, nRunwayID, nRunwayMarkIndex);
		return;
	} 
	else if (pTimeRangeItem->getStrategyManagedExitMode() == TimeRangeRunwayExitStrategyItem::PriorityBased)
	{
		AddNewRunwayMarkExitPriorities(pTimeRangeItem, nRunwayID, nRunwayMarkIndex);
		return;
	}
	else
	{
		ASSERT(FALSE);
	}
}

void CDlgLandingRunwayExit::GetRunwayExitName(RunwayExit *pRunwayExit, RunwayExitList *pRunwayExitList, CString& strRunwayExitName)
{
	ASSERT(pRunwayExit);
	ASSERT(pRunwayExitList);

	int nTaxiwayID = pRunwayExit->GetTaxiwayID();
	//Taxiway taxiway;
	//taxiway.ReadObject(nTaxiwayID);
	//ALTObject * pObject = m_inputAirside.GetObjectByID(nTaxiwayID);
//	if(pObject == NULL)
//		return;

	BOOL bDisplayLeftRight = FALSE;
	BOOL bDisplayIndex     = FALSE;

	RunwayExitList::iterator iter = pRunwayExitList->begin();
	for (; iter!=pRunwayExitList->end(); iter++)
	{
		RunwayExit *pOtherRunwayExit = &(*iter);
		ASSERT(pOtherRunwayExit);

		//skip self
		if (pOtherRunwayExit->GetID() == pRunwayExit->GetID())
		{
			continue;
		}

		//have more intersect node with a taxiway
		if ( pOtherRunwayExit->GetTaxiwayID() == pRunwayExit->GetTaxiwayID() )
		{
			bDisplayLeftRight = TRUE;

			if (pOtherRunwayExit->GetSideType() == pRunwayExit->GetSideType())
			{
				bDisplayIndex = TRUE;
			}
		}
	}

	Taxiway *pTaxiway = m_inputAirside.GetTaxiwayByID(nTaxiwayID);
	CString strShowMark;
	if(pTaxiway)
	{
		strShowMark.Format(_T("%s"), pTaxiway->GetMarking().c_str());
	}
	else 
	{
		Runway *pRunway = m_inputAirside.GetRunwayByID(nTaxiwayID);
		if(pRunway)
			strShowMark.Format(_T("%s"), pRunway->GetMarking1().c_str());
	}

	if (bDisplayLeftRight)
	{
		if (pRunwayExit->GetSideType() == RunwayExit::EXIT_LEFT)
		{
			strShowMark += "&LEFT";
		}
		else
		{
			strShowMark += "&RIGHT";
		}

		if (bDisplayIndex)
		{
			CString strIndex;
			strIndex.Format(_T("%d"), pRunwayExit->GetIntesectionNode().GetIndex());
			strShowMark += "&";
			strShowMark += strIndex;
		}
	}

	strRunwayExitName.Format(_T("%s"), strShowMark);
}

void CDlgLandingRunwayExit::OnMsgDelRunway()
{
	HTREEITEM hTreeItem;
	if(!GetFltTypeTreeItemByType(TREENODE_TIMERANGE, hTreeItem))
		return;
	TimeRangeRunwayExitStrategyItem* pTimeRangeItem = 
		(TimeRangeRunwayExitStrategyItem*)m_wndTreeFltTime.GetItemData(hTreeItem);

	if (pTimeRangeItem->getStrategyManagedExitMode() == TimeRangeRunwayExitStrategyItem::ProbabilityBased)
	{
		HTREEITEM hRunwayItem;
		if (GetRunwayTreeItemByType(TREENODE_PROB_RUNWAY, hRunwayItem))
		{
			RunwayExitStrategyPercentItem* pDelRunwayMarkExitItem =
				(RunwayExitStrategyPercentItem* )m_wndTreeExitStrategy.GetItemData(hRunwayItem);
			if(pDelRunwayMarkExitItem != NULL)
			{
				pTimeRangeItem->DeleteRunwayMarkExitItem(pDelRunwayMarkExitItem);
				m_wndTreeExitStrategy.DeleteItem(hRunwayItem);
			}
		}
		else
		{
			AfxMessageBox(_T("Please select a runway!"));
		}
	}
	else if (pTimeRangeItem->getStrategyManagedExitMode() == TimeRangeRunwayExitStrategyItem::PriorityBased)
	{
		HTREEITEM hRunwayItem;
		if (GetRunwayTreeItemByType(TREENODE_PRIO_RUNWAY, hRunwayItem))
		{
			RunwayExitStrategyPriorityItem* pDelRunwayExitStrategyPriorityiesItem =
				(RunwayExitStrategyPriorityItem* )m_wndTreeExitStrategy.GetItemData(hRunwayItem);
			if(pDelRunwayExitStrategyPriorityiesItem != NULL)
			{
				pTimeRangeItem->DeleteRunwayMarkExitPriorities(pDelRunwayExitStrategyPriorityiesItem);
				m_wndTreeExitStrategy.DeleteItem(hRunwayItem);
			}
		}
		else
		{
			AfxMessageBox(_T("Please select a runway!"));
		}
	}
	else
	{
		ASSERT(FALSE);
	}
}

void CDlgLandingRunwayExit::OnMsgEditRunway()
{
	HTREEITEM hTreeItem;
	if(!GetFltTypeTreeItemByType(TREENODE_TIMERANGE, hTreeItem))
		return;
	TimeRangeRunwayExitStrategyItem* pTimeRangeItem = (TimeRangeRunwayExitStrategyItem*)m_wndTreeFltTime.GetItemData(hTreeItem);
	HTREEITEM hRunwayItem = NULL;
	hRunwayItem = m_wndTreeExitStrategy.GetSelectedItem();
	if(hRunwayItem!= NULL)
	{
		HTREEITEM hParentRunwayItem = NULL;
		hParentRunwayItem = m_wndTreeExitStrategy.GetParentItem(hRunwayItem);
		if(hParentRunwayItem == NULL)
		{
			//HTREEITEM hRunwayExitItem = m_wndTreeExitStrategy.GetChildItem(hRunwayItem);
			//RunwayExitPercentItem* pItem = (RunwayExitPercentItem*)m_wndTreeExitStrategy.GetItemData(hRunwayExitItem);
			//pTimeRangeItem->DeleteRunwayExitPercentItem(pItem->m_nRunwayID);
			
			RunwayExitStrategyPercentItem *pDelRunwayMarkExitItem = (RunwayExitStrategyPercentItem *)m_wndTreeExitStrategy.GetItemData(hRunwayItem);
			if(pDelRunwayMarkExitItem != NULL)
			{
				pTimeRangeItem->DeleteRunwayMarkExitItem(pDelRunwayMarkExitItem);
			}

			CDlgSelectRunway dlg(m_nProjID);
			if(IDOK != dlg.DoModal())
				return;
			int nAirportID = dlg.GetSelectedAirport();
			int nRunwayID = dlg.GetSelectedRunway();
			int nRunwayMarkIndex = (int)dlg.GetSelectedRunwayMarkingIndex();

			ALTAirport altAirport;
			altAirport.ReadAirport(nAirportID);
			Runway altRunway;
			altRunway.ReadObject(nRunwayID);

			RunwayExitPercentItem* pNewItem = new RunwayExitPercentItem(pTimeRangeItem->getID());
			pNewItem->m_nRunwayMarkIndex = nRunwayMarkIndex;
			pNewItem->m_nRunwayID = nRunwayID;
			RunwayExitList exitlist;
			int n = altRunway.GetExitList(RUNWAY_MARK(pNewItem->m_nRunwayMarkIndex),exitlist);
			float nfPercent = (float)100.00/n;
			CString strMarking = (RUNWAYMARK_FIRST == dlg.GetSelectedRunwayMarkingIndex() ? altRunway.GetMarking1().c_str() : altRunway.GetMarking2().c_str());
			CString strRunwayInfo;
			strRunwayInfo.Format("%s: %s", altAirport.getName(), altRunway.GetObjNameString());
			COOLTREE_NODE_INFO cni;
			CCoolTree::InitNodeInfo(this,cni);

			m_wndTreeExitStrategy.SetRedraw(FALSE);
			HTREEITEM hNewRunwayItem = m_wndTreeExitStrategy.InsertItem(strRunwayInfo,cni,FALSE);
			m_wndTreeExitStrategy.SetItemData(hNewRunwayItem, (DWORD_PTR)pNewItem->m_nRunwayID);

			CString strrMarkingExitInfo;

			for (int i = 0;i < n;i++)
			{
				RunwayExitPercentItem* pNewItem = new RunwayExitPercentItem(pTimeRangeItem->getID());
				pNewItem->m_nRunwayMarkIndex = nRunwayMarkIndex;
				pNewItem->m_nRunwayID = nRunwayID;
				pNewItem->m_nExitID = exitlist[i].GetID();
				pNewItem->m_fPercent = nfPercent;
				CString runwayExitName;
				GetRunwayExitName(&exitlist[i], &exitlist, runwayExitName);
				strrMarkingExitInfo.Format("%s & %s: %.1f%%", strMarking,runwayExitName,nfPercent);//exitlist[i].m_strName,  nfPercent);
				cni.nt=NT_NORMAL; 
				cni.net=NET_EDITSPIN_WITH_VALUE;
				HTREEITEM hNewRunwayExitItem = m_wndTreeExitStrategy.InsertItem(strrMarkingExitInfo,cni,FALSE,FALSE,hNewRunwayItem,TVI_LAST);
				m_wndTreeExitStrategy.SetItemData(hNewRunwayExitItem,(DWORD_PTR)pNewItem);
				m_wndTreeExitStrategy.SetItemValueRange(hNewRunwayExitItem,0,100);
				pTimeRangeItem->AddRunwayExitPercentItem(pNewItem);
			}		
			m_wndTreeExitStrategy.Expand(hNewRunwayItem, TVE_EXPAND);
			m_wndTreeExitStrategy.SetRedraw(TRUE);
			ResetExitStrategyContents();
		}
		else
		{
			AfxMessageBox("please choose a runway");
		}
	}
	else
	{
		AfxMessageBox("please choose a runway");
	}
}

void CDlgLandingRunwayExit::OnMsgExitStrategyChange()
{
	if(m_hTimeRangeTreeItem == NULL)
		return;

	TimeRangeRunwayExitStrategyItem *pTimeRangeItem = 
		(TimeRangeRunwayExitStrategyItem*)m_wndTreeFltTime.GetItemData(m_hTimeRangeTreeItem);

	pTimeRangeItem->setStrategyType((RunwayExitStrategyType)m_wndCmbExitStrategy.GetCurSel());

	m_wndTreeFltTime.SetFocus();
	m_wndTreeFltTime.SelectItem(m_hTimeRangeTreeItem);

	ResetExitStrategyContents();

	UpdateState();
}

void CDlgLandingRunwayExit::OnMsgSave()
{
}
BOOL CDlgLandingRunwayExit::CheckExitPercent()
{
	return m_pStrategies->CheckExitPercent();     
}
void CDlgLandingRunwayExit::OnOK()
{
	if( CheckExitPercent() == FALSE )
	{
		MessageBox("Percent must be 100%",NULL,MB_OK);
		return  ;
	}
	try
	{
		CADODatabase::BeginTransaction();
		m_pStrategies->SaveData();

		CADODatabase::CommitTransaction();
	}
	catch (CADOException &e) 
	{
		CADODatabase::RollBackTransation();
		e.ErrorMessage();
		MessageBox(_T("Save Data Error."));
		return;
	}
	CXTResizeDialog::OnOK();
}


void CDlgLandingRunwayExit::OnCancel()
{
	CXTResizeDialog::OnCancel();
}

void CDlgLandingRunwayExit::OnContextMenu(CWnd* pWnd, CPoint point)
{
	CRect  rtFltTypeTree;
	m_wndTreeFltTime.GetWindowRect(&rtFltTypeTree);
	if( rtFltTypeTree.PtInRect(point))
	{
		m_wndTreeFltTime.SetFocus();
		CPoint pt = point;
		m_wndTreeFltTime.ScreenToClient( &pt );
		UINT iRet;
		CMenu pMenu;
		HTREEITEM hRClickItem = m_wndTreeFltTime.HitTest( pt,&iRet );
		if (iRet != TVHT_ONITEMLABEL)   // Must click on the label
		{
			if (pMenu.GetSafeHmenu())
			{
				pMenu.DestroyMenu();
			}
			pMenu.CreatePopupMenu();
			pMenu.AppendMenu(MF_ENABLED|MF_STRING,ID_NEW_FLIGHTTYPE,_T("Add Flight Type"));
			if (pMenu)
				pMenu.TrackPopupMenu(TPM_LEFTALIGN, point.x, point.y, this);
			return;
		}
		m_wndTreeFltTime.SelectItem(hRClickItem);

		//CMenu menuPopup;
		////menuPopup.LoadMenu( IDR_CTX_MENU_EXITSTRATEGY );
		//menuPopup.LoadMenu(IDR_MENU6);
		//CMenu* pMenu=NULL;
		//pMenu = menuPopup.GetSubMenu(0);
		
		
		
		if(GetFltTypeTreeItemByType(TREENODE_FLTTYPE, hRClickItem))
		{
			/*pMenu->EnableMenuItem( ID_DEL_STAND, MF_GRAYED | MF_DISABLED );
			pMenu->EnableMenuItem( ID_EDIT_STAND, MF_GRAYED | MF_DISABLED );
			pMenu->EnableMenuItem( ID_ADD_TIMERANGE, MF_GRAYED | MF_DISABLED );
			pMenu->EnableMenuItem( ID_EDIT_TIMERANGE, MF_GRAYED | MF_DISABLED );
			pMenu->EnableMenuItem( ID_DEL_TIMERANGE, MF_GRAYED | MF_DISABLED );*/
			
			if (pMenu.GetSafeHmenu())
			{
				pMenu.DestroyMenu();
			}
			pMenu.CreatePopupMenu();
			pMenu.AppendMenu(MF_ENABLED|MF_STRING,ID_MODIFY_FLIGHTTYPE,_T("Edit Flight Type"));
			pMenu.AppendMenu(MF_STRING|MF_ENABLED,ID_CUT_FLIGHTTYPE,_T("Delete Flight Type"));
			pMenu.AppendMenu(MF_STRING|MF_ENABLED,ID_NEW_STANDGROUP,_T("Add Stand Group"));

		}
		else if (GetFltTypeTreeItemByType(TREENODE_STAND, hRClickItem))
		{
			/*pMenu->EnableMenuItem( ID_ADD_STAND, MF_GRAYED | MF_DISABLED );
			pMenu->EnableMenuItem( ID_EDIT_TIMERANGE, MF_GRAYED | MF_DISABLED );
			pMenu->EnableMenuItem( ID_DEL_TIMERANGE, MF_GRAYED | MF_DISABLED );*/
			if (pMenu.GetSafeHmenu())
			{
				pMenu.DestroyMenu();
			}
			pMenu.CreatePopupMenu();
			pMenu.AppendMenu(MF_ENABLED|MF_STRING,ID_MODIFY_STANDGROUP,_T("Edit Stand Group"));
			pMenu.AppendMenu(MF_STRING|MF_ENABLED,ID_CUT_STANDGROUP,_T("Delete Stand Group"));
			pMenu.AppendMenu(MF_STRING|MF_ENABLED,ID_NEW_TIMERANGE,_T("Add New Time Range"));
		}
		else if (GetFltTypeTreeItemByType(TREENODE_TIMERANGE, hRClickItem))
		{
			
			/*pMenu->EnableMenuItem( ID_ADD_STAND, MF_GRAYED | MF_DISABLED );
			pMenu->EnableMenuItem( ID_DEL_STAND, MF_GRAYED | MF_DISABLED );
			pMenu->EnableMenuItem( ID_EDIT_STAND, MF_GRAYED | MF_DISABLED );
			pMenu->EnableMenuItem( ID_ADD_TIMERANGE, MF_GRAYED | MF_DISABLED );*/
			if (pMenu.GetSafeHmenu())
			{
				pMenu.DestroyMenu();
			}
			pMenu.CreatePopupMenu();
			pMenu.AppendMenu(MF_ENABLED|MF_STRING,ID_MODIFY_TIMERANGE,_T("Edit Time Range"));
			pMenu.AppendMenu(MF_STRING|MF_ENABLED,ID_CUT_TIMERANGE,_T("Delete Time Range"));
			
		}


		if (pMenu)
			pMenu.TrackPopupMenu(TPM_LEFTALIGN, point.x, point.y, this);
	}

	CRect rtRunwayExitTree;
	m_wndTreeExitStrategy.GetWindowRect(&rtRunwayExitTree);
	if (rtRunwayExitTree.PtInRect(point))
	{
		m_wndTreeExitStrategy.SetFocus();
		CPoint pt = point;
		m_wndTreeExitStrategy.ScreenToClient(&pt);
		UINT iRet;
		HTREEITEM hRClickItem = m_wndTreeExitStrategy.HitTest( pt, &iRet );
		if (iRet != TVHT_ONITEMLABEL)   // Must click on the label
			hRClickItem = NULL;
		if (hRClickItem==NULL)
			return;
		m_wndTreeExitStrategy.SelectItem(hRClickItem);

		CMenu menuPopup;
		CMenu* pMenu = NULL;
		HTREEITEM hMarkingItem;
		if (GetRunwayTreeItemByType(TREENODE_PROB_MARKING, hMarkingItem))
		{
			menuPopup.LoadMenu(IDR_MENU_BACKTRACK);
			pMenu = menuPopup.GetSubMenu(0);
		}
		else if (GetRunwayTreeItemByType(TREENODE_PRIO_RUNWAY, hMarkingItem))
		{
			menuPopup.LoadMenu(IDR_CTX_MENU_STRATEGY_PRIORITY);
			pMenu = menuPopup.GetSubMenu(0);
			pMenu->EnableMenuItem( ID_DEL_EXIT_PRIORITY, MF_GRAYED | MF_DISABLED );
			pMenu->EnableMenuItem( ID_BACKTRACK, MF_GRAYED | MF_DISABLED );
			pMenu->EnableMenuItem( ID_NOBACKTRACK, MF_GRAYED | MF_DISABLED );
		}
		else if (GetRunwayTreeItemByType(TREENODE_PRIO_MARKING, hMarkingItem))
		{
			menuPopup.LoadMenu(IDR_CTX_MENU_STRATEGY_PRIORITY);
			pMenu = menuPopup.GetSubMenu(0);
			pMenu->EnableMenuItem( ID_ADD_EXIT_PRIORITY, MF_GRAYED | MF_DISABLED );
		}
		else
		{
			if (std::find(m_vOperationItem.begin(),m_vOperationItem.end(),hRClickItem) != m_vOperationItem.end())
			{
				pMenu = &menuPopup;
				menuPopup.CreatePopupMenu();

				pMenu->AppendMenu(MF_STRING|MF_ENABLED,ID_ADD_TAXISEG,_T("Add Taxiway Segment"));
			}
			else if (std::find(m_vTaxiSegItem.begin(),m_vTaxiSegItem.end(),hRClickItem) != m_vTaxiSegItem.end())
			{
				pMenu = &menuPopup;
				menuPopup.CreatePopupMenu();
				pMenu->AppendMenu(MF_STRING|MF_ENABLED,ID_EDIT_TAXISEG,_T("Eidt Taxiway Segment"));
				pMenu->AppendMenu(MF_STRING|MF_ENABLED,ID_DEL_TAXISEG,_T("Delete Taxiway Segment"));
			}
		}
		if (pMenu)
			pMenu->TrackPopupMenu(TPM_LEFTALIGN, point.x, point.y, this);
	}

}
void CDlgLandingRunwayExit::UpdateState()
{
	HTREEITEM hFltTypeTreeItem;
	hFltTypeTreeItem=m_wndTreeFltTime.GetSelectedItem();
	if (hFltTypeTreeItem==NULL)
	{
		m_wndFltToolbar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_FLIGHT_TYPE_ADD,TRUE);
		m_wndFltToolbar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_FLIGHT_TYPE_EDIT, FALSE);
		m_wndFltToolbar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_FLIGHT_TYPE_DEL, FALSE);
	}
	else if (GetFltTypeTreeItemByType(TREENODE_TIMERANGE,hFltTypeTreeItem))
	{
		m_wndFltToolbar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_FLIGHT_TYPE_ADD,FALSE);
		m_wndFltToolbar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_FLIGHT_TYPE_DEL,TRUE);
		m_wndFltToolbar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_FLIGHT_TYPE_EDIT,TRUE);
	}
	else if (GetFltTypeTreeItemByType(TREENODE_STAND,hFltTypeTreeItem)|| GetFltTypeTreeItemByType(TREENODE_FLTTYPE,hFltTypeTreeItem))
	{
		m_wndFltToolbar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_FLIGHT_TYPE_ADD,True);
		m_wndFltToolbar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_FLIGHT_TYPE_DEL,TRUE);
		m_wndFltToolbar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_FLIGHT_TYPE_EDIT,TRUE);
	}
	BOOL bEnable ;

	HTREEITEM hTreeItem;
	m_wndCmbExitStrategy.EnableWindow(GetFltTypeTreeItemByType(TREENODE_TIMERANGE, hTreeItem));

	bEnable = TRUE;
	bEnable = IsManagedExitTimeRangeTreeItem();

	m_wndRunwayToolbar.GetToolBarCtrl().EnableButton(ID_NEW_RUNWAY, bEnable);
	m_wndRunwayToolbar.GetToolBarCtrl().EnableButton(ID_DEL_RUNWAY, bEnable);
	//m_wndRunwayToolbar.GetToolBarCtrl().EnableButton(ID_EDIT_RUNWAY, bEnable);
	m_wndTreeExitStrategy.EnableWindow(bEnable);

	////	bEnable &= (m_wndListTakeoffRunwayAssign.GetItemCount() > 0);
	////	m_wndRunwayToolbar.GetToolBarCtrl().EnableButton(ID_EDIT_RUNWAY, bEnable);
	////	m_wndRunwayToolbar.GetToolBarCtrl().EnableButton(ID_DEL_RUNWAY, bEnable);

	//POSITION pos = m_wndTreeExitStrategy.GetFirstVisibleItem();
	//bEnable = TRUE;
	//if(pos == NULL)
	//	bEnable = FALSE;

	//m_wndRunwayToolbar.GetToolBarCtrl().EnableButton(ID_EDIT_RUNWAY, bEnable);
	//m_wndRunwayToolbar.GetToolBarCtrl().EnableButton(ID_DEL_RUNWAY, bEnable);

	// update the radio buttons
	BOOL bEnableRadio = m_wndCmbExitStrategy.IsWindowEnabled() && ManagedExit == m_wndCmbExitStrategy.GetCurSel();
	m_radioProbBased.EnableWindow(bEnableRadio);
	m_radioPrioBased.EnableWindow(bEnableRadio);
}
BOOL CDlgLandingRunwayExit::IsManagedExitTimeRangeTreeItem()
{
	HTREEITEM hTreeItem;
	if(!GetFltTypeTreeItemByType(TREENODE_TIMERANGE, hTreeItem))
		return FALSE;
	m_wndTreeFltTime.SelectItem(hTreeItem);

	TimeRangeRunwayExitStrategyItem* pTimeRangeItem = 
		(TimeRangeRunwayExitStrategyItem*)m_wndTreeFltTime.GetItemData(hTreeItem);

	if(ManagedExit == pTimeRangeItem->getStrategyType())
		return TRUE;

	return FALSE;
}
//BOOL CDlgLandingRunwayExit::OnCopyData(CWnd* pWnd, COPYDATASTRUCT* pCopyDataStruct)
//{
//	// TODO: Add your message handler code here and/or call default
//
//	return CXTResizeDialog::OnCopyData(pWnd, pCopyDataStruct);
//}

LRESULT CDlgLandingRunwayExit::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	// TODO: Add your specialized code here and/or call the base class
	if (message==UM_CEW_EDITSPIN_BEGIN)
	{
	
	}
	else if(message == UM_CEW_EDITSPIN_END)
	{
		HTREEITEM hRunwayExitItem=(HTREEITEM)wParam;
		CString strValue = *((CString*)lParam);
		float temp = (float)atof( strValue.GetBuffer() );

		RunwayExitPercentItem* pNewItem = (RunwayExitPercentItem*)m_wndTreeExitStrategy.GetItemData(hRunwayExitItem);
		if (pNewItem)
		{
			pNewItem->m_fPercent = temp;
			CString strExit;
			Runway altRunway;
			altRunway.ReadObject(pNewItem->m_nRunwayID);
			CString strMarkTemp;
			CString strExitNameTemp;
			if(pNewItem->m_nRunwayMarkIndex == 0)
				strMarkTemp = altRunway.GetMarking1().c_str();
			else
				strMarkTemp = altRunway.GetMarking2().c_str();

			RunwayExitList exitlist;
			int n = altRunway.GetExitList(RUNWAY_MARK(pNewItem->m_nRunwayMarkIndex),exitlist);

			for(int i=0;i<n;i++)
			{
				if(exitlist[i].GetID() == pNewItem->m_nExitID)
				{
					GetRunwayExitName(&exitlist[i], &exitlist, strExitNameTemp);
					break;
				}
			}

			strExit.Format("%s & %s: %.1f%%", strMarkTemp, strExitNameTemp, pNewItem->m_fPercent);
			m_wndTreeExitStrategy.SetItemText(hRunwayExitItem, strExit);
		}

	}
	else if (message==UM_CEW_COMBOBOX_BEGIN)
	{
		HTREEITEM hItem = (HTREEITEM)wParam;
		CComboBox* pCB = (CComboBox*)m_wndTreeExitStrategy.GetEditWnd(hItem);
		CRect rectWnd;
		m_wndTreeExitStrategy.GetItemRect(hItem,rectWnd,TRUE);
		pCB->SetWindowPos(NULL,rectWnd.right,rectWnd.top,0,0,SWP_NOSIZE);

		pCB->ResetContent();
		if(GetRunwayTreeItemByType(TREENODE_PRIO_MARKING,hItem))
		{
			HTREEITEM hParentItem = m_wndTreeExitStrategy.GetParentItem(hItem);
			if (hParentItem)
			{
				RunwayExitStrategyPriorityItem* pPriorities =
					(RunwayExitStrategyPriorityItem*)m_wndTreeExitStrategy.GetItemData(hParentItem);
				Runway *pRunway = m_inputAirside.GetRunwayByID(pPriorities->getRunwayID());
				if(pRunway)
				{
					CString strRunwayMark;
					if(pPriorities->getMarkIndex() == RUNWAYMARK_FIRST)
						strRunwayMark = pRunway->GetMarking1().c_str();
					else
						strRunwayMark = pRunway->GetMarking2().c_str();


					RunwayExitStrategyPriorityVector vExitItems = pPriorities->getItems();
					RunwayExitStrategyPriorityIter iter = vExitItems.begin();
					for(;iter != vExitItems.end(); iter++)
					{
						RunwayExitPriorityItem* pItem = *iter;
						CString strExit;
						CString strMarkTemp = strRunwayMark;
						CString strExitNameTemp;

						RunwayExitList exitlist;
						int n = pRunway->GetExitList(RUNWAY_MARK(pItem->m_nRunwayMarkIndex),exitlist);
						for(int i=0;i<n;i++)
						{
							if(exitlist[i].GetID() == pItem->m_nExitID)
							{
								GetRunwayExitName(&exitlist[i], &exitlist, strExitNameTemp);
								break;
							}
						}

						strExit.Format(_T("%s & %s"), strMarkTemp, strExitNameTemp);
						pCB->AddString(strExit);
					}
				}
			}
		}
		else
		{
			pCB->AddString(strComboxString[0]);
			pCB->AddString(strComboxString[1]);
		}
		
	}
	else if(message == UM_CEW_COMBOBOX_SELCHANGE)
	{
		HTREEITEM hItem = (HTREEITEM)wParam;
		CComboBox* pCB = (CComboBox*)m_wndTreeExitStrategy.GetEditWnd(hItem);
		if(GetRunwayTreeItemByType(TREENODE_PRIO_MARKING,hItem))
		{
			RunwayExitPriorityItem* pItem =
				(RunwayExitPriorityItem*)m_wndTreeExitStrategy.GetItemData(hItem);
			

			HTREEITEM hParentItem = m_wndTreeExitStrategy.GetParentItem(hItem);
			if (hParentItem)
			{
				RunwayExitStrategyPriorityItem* pPriorities =
					(RunwayExitStrategyPriorityItem*)m_wndTreeExitStrategy.GetItemData(hParentItem);
				pPriorities->ExchangeItem(pItem, pCB->GetCurSel());
				ResetExitStrategyContents();
			}
		}
		else
		{
			RunwayExitPriorityItem* pConditionData = (RunwayExitPriorityItem*)m_wndTreeExitStrategy.GetItemData(hItem);
			int nCurSel = pCB->GetCurSel();
			if(std::find(m_vOperationItem.begin(),m_vOperationItem.end(),hItem) != m_vOperationItem.end())
			{
				bool bOperation = pCB->GetCurSel()?true : false;
				pConditionData->GetConditionData().SetOperationStatus(bOperation);
				CString strOperation;
				strOperation.Format(_T("Operation on: %s"),strComboxString[nCurSel]);
				m_wndTreeExitStrategy.SetItemText(hItem,strOperation);
			}
			else
			{
				bool bArrStand = nCurSel?true : false;
				pConditionData->GetConditionData().SetArrStandOccupied(bArrStand);

				CString strArrStand;
				strArrStand.Format(_T("Arrival stand occupied: %s"),strComboxString[nCurSel]);
				m_wndTreeExitStrategy.SetItemText(hItem,strArrStand);
			}
		}
		
	}
	else if (message == UM_CEW_SHOW_DIALOGBOX_BEGIN)
	{
		HTREEITEM hItem = (HTREEITEM)wParam;

		TaxiSegmentData* pSegData = (TaxiSegmentData*)m_wndTreeExitStrategy.GetItemData(hItem);
		CDlgTaxiwaySegmentSelect dlg(m_nProjID,this);
		dlg.SetTaxiwaySegmentData(*pSegData);
		if (dlg.DoModal() == IDOK)
		{
			TaxiSegmentData segData;
			dlg.GetTaxiwaySegmentData(segData);

			*pSegData = segData;

			Taxiway* pTaxiway = m_inputAirside.GetTaxiwayByID(pSegData->m_iTaxiwayID);
			CString strTaxiway = pTaxiway->getName().GetIDString();
			IntersectionNode startNode;
			startNode.ReadData(pSegData->m_iStartNode);
			CString strStart = startNode.GetName();
			IntersectionNode endNode;
			endNode.ReadData(pSegData->m_iEndNode);
			CString strEnd = endNode.GetName();

			CString strText;
			strText.Format(_T("Taxiway [%s] between [%s] & [%s]"),strTaxiway,strStart,strEnd);

			m_wndTreeExitStrategy.SetItemText(hItem,strText);
		}
	}


	return CXTResizeDialog::DefWindowProc(message, wParam, lParam);
}

void CDlgLandingRunwayExit::AddRunwayExitItemToTree( RunwayExitStrategyPercentItem *pRunwayMarkExitItem )
{
	if(pRunwayMarkExitItem == NULL)
		return;

	Runway *pRunway = m_inputAirside.GetRunwayByID(pRunwayMarkExitItem->getRunwayID());
	if(pRunway == NULL)
		return;
	ALTAirport *pAltAirport = m_inputAirside.GetAirportbyID(pRunway->getAptID());
	if(pAltAirport == NULL)
		return;


	CString strRunwayMark;
	if(pRunwayMarkExitItem->getMarkIndex() == RUNWAYMARK_FIRST)
		strRunwayMark = pRunway->GetMarking1().c_str();
	else
		strRunwayMark = pRunway->GetMarking2().c_str();

	CString strRunway;
	strRunway.Format("%s: %s & %s", pAltAirport->getName(), pRunway->GetObjNameString(),strRunwayMark);

	COOLTREE_NODE_INFO cni;	
	CCoolTree::InitNodeInfo(this,cni);

	/*	cni.dwItemData = (DWORD)40;*/

	cni.nt=NT_NORMAL; 
	cni.net=NET_STATIC;
	HTREEITEM hRunwayItem = m_wndTreeExitStrategy.InsertItem(strRunway,cni,FALSE);	

	std::vector<RunwayExitPercentItem *> vExitItems = pRunwayMarkExitItem->getItems();

	std::vector<RunwayExitPercentItem *>::iterator iterExitItem = vExitItems.begin();
	for(;iterExitItem != vExitItems.end(); iterExitItem++)
	{
		RunwayExitPercentItem* pItem = *iterExitItem;
		CString strExit;
		CString strMarkTemp = strRunwayMark;
		CString strExitNameTemp;

		RunwayExitList exitlist;
		int n = pRunway->GetExitList(RUNWAY_MARK(pItem->m_nRunwayMarkIndex),exitlist);
		RunwayExit runwayExit;
		for(int i=0;i<n;i++)
		{
			if(exitlist[i].GetID() == pItem->m_nExitID)
			{
				GetRunwayExitName(&exitlist[i], &exitlist, strExitNameTemp);
				break;
			}
		}

		strExit.Format("%s & %s: %.1f%%", strMarkTemp, strExitNameTemp, pItem->m_fPercent);

		cni.nt=NT_NORMAL; 
		cni.net=NET_EDITSPIN_WITH_VALUE;
		HTREEITEM hRunwayExitItem = m_wndTreeExitStrategy.InsertItem(strExit,cni,FALSE,FALSE,hRunwayItem,TVI_LAST);
		m_wndTreeExitStrategy.SetItemData(hRunwayExitItem, (DWORD_PTR)pItem);
		m_wndTreeExitStrategy.SetItemValueRange(hRunwayExitItem,0,100);

		if (pItem->IsBacktrack())
		{
			cni.nt = NT_NORMAL;
			cni.net = NET_STATIC;
			HTREEITEM hBacktrackItem = m_wndTreeExitStrategy.InsertItem(_T("Backtrack Operation"),
				cni,FALSE,FALSE,hRunwayExitItem,TVI_LAST);
			m_wndTreeExitStrategy.Expand(hRunwayExitItem, TVE_EXPAND);
		}
	}
	m_wndTreeExitStrategy.SetItemData(hRunwayItem, (DWORD_PTR)pRunwayMarkExitItem);		
	m_wndTreeExitStrategy.Expand(hRunwayItem, TVE_EXPAND);
}
void CDlgLandingRunwayExit::InsertTaxiwaySegmentData(TaxiSegmentData* pSegData, HTREEITEM hItem)
{
	COOLTREE_NODE_INFO cni;	
	CCoolTree::InitNodeInfo(this,cni);
	cni.nt=NT_NORMAL; 
	cni.net=NET_SHOW_DIALOGBOX;

	Taxiway* pTaxiway = m_inputAirside.GetTaxiwayByID(pSegData->m_iTaxiwayID);
	CString strTaxiway = pTaxiway->getName().GetIDString();
	IntersectionNode startNode;
	startNode.ReadData(pSegData->m_iStartNode);
	CString strStart = startNode.GetName();
	IntersectionNode endNode;
	endNode.ReadData(pSegData->m_iEndNode);
	CString strEnd = endNode.GetName();

	CString strText;
	strText.Format(_T("Taxiway [%s] between [%s] & [%s]"),strTaxiway,strStart,strEnd);
	HTREEITEM hTaxiwayItem = m_wndTreeExitStrategy.InsertItem(strText,cni,FALSE,FALSE,hItem);
	m_wndTreeExitStrategy.SetItemData(hTaxiwayItem,(DWORD)pSegData);
	m_vTaxiSegItem.push_back(hTaxiwayItem);

	m_wndTreeExitStrategy.Expand(hItem,TVE_EXPAND);
}

void CDlgLandingRunwayExit::InsertChangeCondition( RunwayExitPriorityItem* pPriorityItem,HTREEITEM hItem )
{
	COOLTREE_NODE_INFO cni;	
	CCoolTree::InitNodeInfo(this,cni);
	cni.nt=NT_NORMAL; 
	cni.net=NET_NORMAL;

	HTREEITEM hChangeItem = m_wndTreeExitStrategy.InsertItem(_T("Change Condition"),cni,FALSE,FALSE,hItem);
	m_wndTreeExitStrategy.SetItemData(hChangeItem,(DWORD)pPriorityItem);

	//change condition
	{
		HTREEITEM hLimitItem = m_wndTreeExitStrategy.InsertItem(_T("Performance Limitation: Yes"),cni,FALSE,FALSE,hChangeItem);
		HTREEITEM hExitItem = m_wndTreeExitStrategy.InsertItem(_T("Exit blocked (arr or dep ac): Yes"),cni,FALSE,FALSE,hChangeItem);
		
		cni.net = NET_COMBOBOX;
		CString strArrStand;
		int iArrStand = pPriorityItem->GetConditionData().GetArrStandOccupied() ? 1 : 0;
		strArrStand.Format(_T("Arrival stand occupied: %s"),strComboxString[iArrStand]);
		HTREEITEM hArrStandItem = m_wndTreeExitStrategy.InsertItem(strArrStand,cni,FALSE,FALSE,hChangeItem);
		m_wndTreeExitStrategy.SetItemData(hArrStandItem,(DWORD)pPriorityItem);

		//taxiway segment
		cni.net = NET_COMBOBOX;
		CString strOperation;
		int iOperation = pPriorityItem->GetConditionData().GetOperationStatus() ? 1 : 0;
		strOperation.Format(_T("Operation on: %s"),strComboxString[iOperation]);
		HTREEITEM hOperationItem = m_wndTreeExitStrategy.InsertItem(strOperation,cni,FALSE,FALSE,hChangeItem);
		m_wndTreeExitStrategy.SetItemData(hOperationItem,(DWORD)pPriorityItem);
		m_vOperationItem.push_back(hOperationItem);
		cni.net = NET_SHOW_DIALOGBOX;
		for (int i = 0; i < pPriorityItem->GetConditionData().getTaxiConditionCount(); i++)
		{
			TaxiSegmentData* pSegData = pPriorityItem->GetConditionData().getTaxiCondition(i);
			InsertTaxiwaySegmentData(pSegData,hOperationItem);
		}
	}
	m_wndTreeExitStrategy.Expand(hChangeItem,TVE_EXPAND);
	m_wndTreeExitStrategy.Expand(hItem,TVE_EXPAND);
}


void CDlgLandingRunwayExit::AddRunwayExitPrioritiesItemToTree(RunwayExitStrategyPriorityItem* pRunwayMarkPrioritiesItem)
{
	if(pRunwayMarkPrioritiesItem == NULL)
		return;

	Runway *pRunway = m_inputAirside.GetRunwayByID(pRunwayMarkPrioritiesItem->getRunwayID());
	if(pRunway == NULL)
		return;
	ALTAirport *pAltAirport = m_inputAirside.GetAirportbyID(pRunway->getAptID());
	if(pAltAirport == NULL)
		return;


	CString strRunwayMark;
	if(pRunwayMarkPrioritiesItem->getMarkIndex() == RUNWAYMARK_FIRST)
		strRunwayMark = pRunway->GetMarking1().c_str();
	else
		strRunwayMark = pRunway->GetMarking2().c_str();

	CString strRunway;
	strRunway.Format("%s: %s & %s", pAltAirport->getName(), pRunway->GetObjNameString(),strRunwayMark);

	COOLTREE_NODE_INFO cni;	
	CCoolTree::InitNodeInfo(this,cni);
	cni.nt=NT_NORMAL; 
	cni.net=NET_STATIC;
	HTREEITEM hRunwayItem = m_wndTreeExitStrategy.InsertItem(strRunway,cni,FALSE);
	m_wndTreeExitStrategy.SetItemData(hRunwayItem, (DWORD)pRunwayMarkPrioritiesItem);

	RunwayExitStrategyPriorityVector vExitItems = pRunwayMarkPrioritiesItem->getItems();
	RunwayExitStrategyPriorityIter iterExitItem = vExitItems.begin();
	for(;iterExitItem != vExitItems.end(); iterExitItem++)
	{
		RunwayExitPriorityItem* pItem = *iterExitItem;
		CString strExit;
		CString strMarkTemp = strRunwayMark;
		CString strExitNameTemp;

		RunwayExitList exitlist;
		int n = pRunway->GetExitList(RUNWAY_MARK(pItem->m_nRunwayMarkIndex),exitlist);
		RunwayExit runwayExit;
		for(int i=0;i<n;i++)
		{
			if(exitlist[i].GetID() == pItem->m_nExitID)
			{
				GetRunwayExitName(&exitlist[i], &exitlist, strExitNameTemp);
				break;
			}
		}

		strExit.Format("Priority %d: %s & %s", pItem->getPriorityIndex(), strMarkTemp, strExitNameTemp);

		cni.nt=NT_NORMAL; 
		cni.net=NET_COMBOBOX;
		HTREEITEM hRunwayExitItem = m_wndTreeExitStrategy.InsertItem(strExit,cni,FALSE,FALSE,hRunwayItem,TVI_LAST);
		m_wndTreeExitStrategy.SetItemData(hRunwayExitItem, (DWORD_PTR)pItem);

		if (pItem->IsBacktrack())
		{
			cni.nt=NT_NORMAL; 
			cni.net=NET_STATIC;
			HTREEITEM hBacktrackItem = m_wndTreeExitStrategy.InsertItem( _T("Backtrack Operation"),
				cni,FALSE,FALSE,hRunwayExitItem,TVI_LAST);
		}

		//change condition
		InsertChangeCondition(pItem,hRunwayExitItem);
		m_wndTreeExitStrategy.Expand(hRunwayExitItem, TVE_EXPAND);
	}
	m_wndTreeExitStrategy.SetItemData(hRunwayItem, (DWORD_PTR)pRunwayMarkPrioritiesItem);		
	m_wndTreeExitStrategy.Expand(hRunwayItem, TVE_EXPAND);
}


void CDlgLandingRunwayExit::InitExitStrategyComboBox()
{
	// initialize combo box
	m_wndCmbExitStrategy.AddString(_T("Max Brake Either Side"));
	m_wndCmbExitStrategy.AddString(_T("Max Brake Same Side"));
	m_wndCmbExitStrategy.AddString(_T("Normal Brake Either Side"));
	m_wndCmbExitStrategy.AddString(_T("Normal Brake Same Side"));
	m_wndCmbExitStrategy.AddString(_T("Managed Exit Assignment"));

	m_wndCmbExitStrategy.SetCurSel(0);
}



void CDlgLandingRunwayExit::ResetFltTimeTreeCtrlContent()
{
	m_wndTreeFltTime.DeleteAllItems();

	COOLTREE_NODE_INFO cni;
	CCoolTree::InitNodeInfo(this, cni);

	for (int i=0; i<m_pStrategies->GetStrategyCount(); i++)
	{
		AddOneExitStrategyToTree(m_pStrategies->GetStrategyItem(i), cni);
	}
}


void CDlgLandingRunwayExit::AddOneExitStrategyToTree( FlightTypeRunwayExitStrategyItem* pStrategyItem, COOLTREE_NODE_INFO &cni )
{
	cni.nt = NT_NORMAL;
	cni.net = NET_STATIC;

	CString strFltType;
	pStrategyItem->GetFltType().screenPrint(strFltType);
	CString strTmp;
	strTmp.Format(_T("Flight Type: %s"), strFltType);
	HTREEITEM hFltTypeTreeItem = m_wndTreeFltTime.InsertItem(strTmp, cni, FALSE);
	m_wndTreeFltTime.SetItemData(hFltTypeTreeItem, (DWORD_PTR)pStrategyItem);
	for (int j=0; j<pStrategyItem->GetRunwayExitStandStrategyCount(); j++)
	{
		AddOneStandStrategyItemToTree(pStrategyItem->GetRunwayExitStandStrategyItem(j), cni, hFltTypeTreeItem);
	}
	m_wndTreeFltTime.Expand(hFltTypeTreeItem, TVE_EXPAND);
}

void CDlgLandingRunwayExit::AddOneStandStrategyItemToTree( RunwayExitStandStrategyItem* pStandStrategyItem, COOLTREE_NODE_INFO & cni, HTREEITEM hFltTypeTreeItem )
{
	cni.nt = NT_NORMAL;
	cni.net = NET_STATIC;

	CString str;
	str.Format(_T("Stand Group: %s"), pStandStrategyItem->getStandFamilyName());
	HTREEITEM hStandStrategyTreeItem = m_wndTreeFltTime.InsertItem(str, cni, FALSE, FALSE, hFltTypeTreeItem);
	m_wndTreeFltTime.SetItemData(hStandStrategyTreeItem, (DWORD_PTR)pStandStrategyItem);
	for(int k=0;k<pStandStrategyItem->GetTimeRangeStrategyItemCount();k++)
	{
		AddOneTimeRangeToTree(pStandStrategyItem->GetTimeRangeStrategyItem(k), cni, hStandStrategyTreeItem);
	}
	m_wndTreeFltTime.Expand(hStandStrategyTreeItem, TVE_EXPAND);
}

void CDlgLandingRunwayExit::AddOneTimeRangeToTree( TimeRangeRunwayExitStrategyItem* pTimeRangeItem, COOLTREE_NODE_INFO &cni, HTREEITEM hStandStrategyTreeItem )
{
	cni.nt = NT_NORMAL;
	cni.net = NET_NORMAL;

	CString strTimeRange = _T(""),strDay = _T("");
	TimeRange trInsert = pTimeRangeItem->GetTimeRange();
	ElapsedTime etInsert = trInsert.GetStartTime();
	long lSecond = etInsert.asSeconds();
	strDay.Format(_T("Day%d %02d:%02d:%02d"),lSecond/86400+1,(lSecond % 86400)/3600,(lSecond % 86400 % 3600)/60,lSecond % 86400 % 3600 % 60);
	strTimeRange = strDay + _T(" - ");
	etInsert = trInsert.GetEndTime();
	lSecond = etInsert.asSeconds();
	strDay.Format(_T("Day%d %02d:%02d:%02d"),lSecond/86400+1,(lSecond % 86400)/3600,(lSecond % 86400 % 3600)/60,lSecond % 86400 % 3600 % 60);
	strTimeRange += strDay;

	HTREEITEM hTimeRangeTreeItem = m_wndTreeFltTime.InsertItem(strTimeRange, cni, FALSE, FALSE, hStandStrategyTreeItem, TVI_LAST);
	m_wndTreeFltTime.SetItemData(hTimeRangeTreeItem, (DWORD_PTR)pTimeRangeItem);
}


class StrategyStandFamilyCheck: public IStandFamilyFilter
{
public:
	StrategyStandFamilyCheck(FlightTypeRunwayExitStrategyItem* pStrategyItem, int nNowEditStandFamilyID = 0)
		: m_pStrategyItem(pStrategyItem)
		, m_nNowEditStandFamilyID(nNowEditStandFamilyID)
	{
		ASSERT(m_pStrategyItem);
	}
	virtual BOOL IsAllowed(int nStandFamilyID, CString& strError)
	{
		if (0 == nStandFamilyID)
		{
			strError = _T("Invliad stand group!");
			return FALSE;
		}
		if (m_nNowEditStandFamilyID == nStandFamilyID)
		{
			return TRUE; // same as the edited item
		}
		int nCount = m_pStrategyItem->GetRunwayExitStandStrategyCount();
		for(int i=0;i<nCount;i++)
		{
			RunwayExitStandStrategyItem* pStandStrategyItem = m_pStrategyItem->GetRunwayExitStandStrategyItem(i);
			if (pStandStrategyItem->getStandFamilyID() == nStandFamilyID)
			{
				strError.Format(_T("The stand group\"%s\" has already been used!"), pStandStrategyItem->getStandFamilyName());
				return FALSE;
			}
		}
		return TRUE;
	}

private:
	FlightTypeRunwayExitStrategyItem* m_pStrategyItem;
	int m_nNowEditStandFamilyID;
};

void CDlgLandingRunwayExit::OnMsgAddStandFamily()
{
	HTREEITEM hTreeItem;
	if(!GetFltTypeTreeItemByType(TREENODE_FLTTYPE, hTreeItem))
		return;
	m_wndTreeFltTime.SelectItem(hTreeItem);

	FlightTypeRunwayExitStrategyItem* pStrategyItem =
		(FlightTypeRunwayExitStrategyItem*)m_wndTreeFltTime.GetItemData(hTreeItem);

	CDlgStandFamily dlg(m_nProjID, &StrategyStandFamilyCheck(pStrategyItem));
	if (IDOK == dlg.DoModal())
	{
		// init default value
		TimeRangeRunwayExitStrategyItem* pTimeRangeItem = new TimeRangeRunwayExitStrategyItem(-1);
		RunwayExitStandStrategyItem* pStandStrategyItem = new RunwayExitStandStrategyItem(pStrategyItem->getID());
		pStandStrategyItem->AddTimeRangeRWAssignStrategyItem(pTimeRangeItem);
		pStandStrategyItem->setStandFamilyID(dlg.GetSelStandFamilyID());
		pStrategyItem->AddRunwayExitStandStrategyItem(pStandStrategyItem);

		COOLTREE_NODE_INFO cni;
		CCoolTree::InitNodeInfo(this, cni);
		AddOneStandStrategyItemToTree(pStandStrategyItem, cni, hTreeItem);
		m_wndTreeFltTime.Expand(hTreeItem, TVE_EXPAND);
	}
}

void CDlgLandingRunwayExit::OnMsgEditStandFamily()
{
	HTREEITEM hTreeItem;
	if(!GetFltTypeTreeItemByType(TREENODE_STAND, hTreeItem))
		return;
	m_wndTreeFltTime.SelectItem(hTreeItem);

	HTREEITEM hParentItem = m_wndTreeFltTime.GetParentItem(hTreeItem);
	if (NULL == hParentItem)
	{
		return;
	}
	FlightTypeRunwayExitStrategyItem* pStrategyItem =
		(FlightTypeRunwayExitStrategyItem*)m_wndTreeFltTime.GetItemData(hParentItem);
	RunwayExitStandStrategyItem* pStandStrategyItem =
		(RunwayExitStandStrategyItem*)m_wndTreeFltTime.GetItemData(hTreeItem);

	CDlgStandFamily dlg(m_nProjID, &StrategyStandFamilyCheck(pStrategyItem, pStandStrategyItem->getStandFamilyID()));
	if (IDOK == dlg.DoModal())
	{
		pStandStrategyItem->setStandFamilyID(dlg.GetSelStandFamilyID());

		CString str;
		str.Format(_T("Stand Group: %s"), pStandStrategyItem->getStandFamilyName());
		m_wndTreeFltTime.SetItemText(hTreeItem, str);
	}
}

void CDlgLandingRunwayExit::OnMsgDelStandFamily()
{
	HTREEITEM hTreeItem;
	if(!GetFltTypeTreeItemByType(TREENODE_STAND, hTreeItem))
		return;
	m_wndTreeFltTime.SelectItem(hTreeItem);

	HTREEITEM hParentItem = m_wndTreeFltTime.GetParentItem(hTreeItem);
	if (NULL == hParentItem)
	{
		return;
	}
	FlightTypeRunwayExitStrategyItem* pStrategyItem =
		(FlightTypeRunwayExitStrategyItem*)m_wndTreeFltTime.GetItemData(hParentItem);
	RunwayExitStandStrategyItem* pStandStrategyItem =
		(RunwayExitStandStrategyItem*)m_wndTreeFltTime.GetItemData(hTreeItem);

	pStrategyItem->DeleteRunwayExitStandStrategyItem(pStandStrategyItem);
	m_wndTreeFltTime.DeleteItem(hTreeItem);
}

void CDlgLandingRunwayExit::OnBnClickedProbabilityBased()
{
	HTREEITEM hTreeItem;
	if (GetFltTypeTreeItemByType(TREENODE_TIMERANGE, hTreeItem))
	{
		TimeRangeRunwayExitStrategyItem* pTimeRangeItem =
			(TimeRangeRunwayExitStrategyItem*)m_wndTreeFltTime.GetItemData(hTreeItem);
		pTimeRangeItem->setStrategyManagedExitMode(TimeRangeRunwayExitStrategyItem::ProbabilityBased);

		ResetExitStrategyContents(pTimeRangeItem);
	}
}

void CDlgLandingRunwayExit::OnBnClickedPriorityBased()
{
	HTREEITEM hTreeItem;
	if (GetFltTypeTreeItemByType(TREENODE_TIMERANGE, hTreeItem))
	{
		TimeRangeRunwayExitStrategyItem* pTimeRangeItem =
			(TimeRangeRunwayExitStrategyItem*)m_wndTreeFltTime.GetItemData(hTreeItem);
		pTimeRangeItem->setStrategyManagedExitMode(TimeRangeRunwayExitStrategyItem::PriorityBased);

		ResetExitStrategyContents(pTimeRangeItem);
	}

}

void CDlgLandingRunwayExit::AddNewRunwayMarkExitItem( TimeRangeRunwayExitStrategyItem * pTimeRangeItem, int nRunwayID, int nRunwayMarkIndex )
{
	// check whether the airport&runway already exists
	if (pTimeRangeItem->FindRunwaExitPercentItem(nRunwayID, nRunwayMarkIndex))
	{
		MessageBox(_T("The selected runway already exists!"));
		return;
	}

	// data building
	RunwayExitList exitlist;
	Runway altRunway;
	altRunway.ReadObject(nRunwayID);
	int n = altRunway.GetExitList(RUNWAY_MARK(nRunwayMarkIndex),exitlist);

	float nfPercent = (float)100.00/n;
	CString strrMarkingExitInfo;
	RunwayExitStrategyPercentItem* pRunwayMarkExitItem = pTimeRangeItem->GetRunwayExitPercentItem(nRunwayID,nRunwayMarkIndex);
	for (int i = 0;i < n;i++)
	{
		RunwayExitPercentItem* pNewItem = new RunwayExitPercentItem(pTimeRangeItem->getID());
		pNewItem->m_nRunwayMarkIndex = nRunwayMarkIndex;
		pNewItem->m_nRunwayID = nRunwayID;
		pNewItem->m_nExitID = exitlist[i].GetID();
		pNewItem->m_fPercent = nfPercent;
		pTimeRangeItem->AddRunwayExitPercentItem(pNewItem);
	}

	// GUI displaying
	m_wndTreeExitStrategy.SetRedraw(FALSE);
	AddRunwayExitItemToTree(pRunwayMarkExitItem);
	m_wndTreeExitStrategy.SetRedraw(TRUE);
}

void CDlgLandingRunwayExit::AddNewRunwayMarkExitPriorities( TimeRangeRunwayExitStrategyItem * pTimeRangeItem, int nRunwayID, int nRunwayMarkIndex )
{
	// check whether the airport&runway already exists
	if (pTimeRangeItem->FindRunwayExitPriorityItem(nRunwayID, nRunwayMarkIndex))
	{
		MessageBox(_T("The selected runway already exists!"));
		return;
	}


	RunwayExitStrategyPriorityItem* pRunwayMarkPrioritiesItem = pTimeRangeItem->GetRunwayExitPriorityItem(nRunwayID,nRunwayMarkIndex);

	// GUI displaying
	m_wndTreeExitStrategy.SetRedraw(FALSE);
	AddRunwayExitPrioritiesItemToTree(pRunwayMarkPrioritiesItem);
	m_wndTreeExitStrategy.SetRedraw(TRUE);
}

class CExitOfRunwayCheck : public IExitOfRunwayFilter
{
public:
	CExitOfRunwayCheck(RunwayExitStrategyPriorityItem* pRunwayMarkPrioritiesItem, int nCurEditExitID = 0)
		: m_pRunwayMarkPrioritiesItem(pRunwayMarkPrioritiesItem)
		, m_nCurEditExitID(m_nCurEditExitID)
	{
		ASSERT(pRunwayMarkPrioritiesItem);
	}

	virtual BOOL IsAllowed(int nExitID, CString& strError)
	{
		if (0 == nExitID)
		{
			strError = _T("Invalid exit!");
			return FALSE;
		}
		if (nExitID == m_nCurEditExitID)
		{
			return TRUE;
		}
		RunwayExitStrategyPriorityVector vecPriorities = m_pRunwayMarkPrioritiesItem->getItems();
		for(size_t i=0;i<vecPriorities.size();i++)
		{
			if (vecPriorities[i]->getExitID() == nExitID)
			{
				strError = _T("The exit already exists");
				return FALSE;
			}
		}
		return TRUE;
	}
private:
	RunwayExitStrategyPriorityItem* m_pRunwayMarkPrioritiesItem;
	int m_nCurEditExitID;
};

void CDlgLandingRunwayExit::OnMsgAddExitPriority()
{
	HTREEITEM hFltTypeTreeItem;
	if (FALSE == GetFltTypeTreeItemByType(TREENODE_TIMERANGE, hFltTypeTreeItem))
	{
		return;
	}
	TimeRangeRunwayExitStrategyItem* pTimeRangeItem =
		(TimeRangeRunwayExitStrategyItem*)m_wndTreeFltTime.GetItemData(hFltTypeTreeItem);

	HTREEITEM hRunwayTreeItem;
	if (FALSE == GetRunwayTreeItemByType(TREENODE_PRIO_RUNWAY, hRunwayTreeItem))
	{
		return;
	}
	RunwayExitStrategyPriorityItem* pRunwayMarkPrioritiesItem =
		(RunwayExitStrategyPriorityItem*)m_wndTreeExitStrategy.GetItemData(hRunwayTreeItem);

	CDlgSelectExitOfRunway dlg(pRunwayMarkPrioritiesItem->getRunwayID(), pRunwayMarkPrioritiesItem->getMarkIndex(),
		&CExitOfRunwayCheck(pRunwayMarkPrioritiesItem));
	if (IDOK == dlg.DoModal())
	{
		RunwayExitPriorityItem* pPriorityItem =
			new RunwayExitPriorityItem(pTimeRangeItem->getID());
		pPriorityItem->m_nRunwayID = pRunwayMarkPrioritiesItem->getRunwayID();
		pPriorityItem->m_nRunwayMarkIndex = pRunwayMarkPrioritiesItem->getMarkIndex();
		pPriorityItem->m_nExitID = dlg.GetSelExitID();
		pPriorityItem->m_nPriorityIndex = pRunwayMarkPrioritiesItem->getItems().size();
		pTimeRangeItem->AddRunwayExitPriority(pPriorityItem);
		pRunwayMarkPrioritiesItem->ResortPriorities();

		ResetExitStrategyContents();
	}
}

void CDlgLandingRunwayExit::OnMsgDelExitPriority()
{
	HTREEITEM hRunwayTreeItem;
	if (FALSE == GetRunwayTreeItemByType(TREENODE_PRIO_MARKING, hRunwayTreeItem))
	{
		return;
	}
	RunwayExitPriorityItem* pPriorityItem =
		(RunwayExitPriorityItem*)m_wndTreeExitStrategy.GetItemData(hRunwayTreeItem);
	HTREEITEM hParentItem = m_wndTreeExitStrategy.GetParentItem(hRunwayTreeItem);
	if (NULL == hParentItem)
	{
		return;
	}
	RunwayExitStrategyPriorityItem* pRunwayMarkPrioritiesItem =
		(RunwayExitStrategyPriorityItem*)m_wndTreeExitStrategy.GetItemData(hParentItem);

	pRunwayMarkPrioritiesItem->DeleteItem(pPriorityItem);
	pRunwayMarkPrioritiesItem->ResortPriorities();

	ResetExitStrategyContents();
}

void CDlgLandingRunwayExit::OnAddTaxiwaySegment()
{
	HTREEITEM hItem = m_wndTreeExitStrategy.GetSelectedItem();
	if (hItem == NULL)
		return;
	
	CDlgTaxiwaySegmentSelect dlg(m_nProjID,this);
	if (dlg.DoModal() == IDOK)
	{
		TaxiSegmentData segData;
		dlg.GetTaxiwaySegmentData(segData);
		RunwayExitPriorityItem* pPriority = (RunwayExitPriorityItem*)m_wndTreeExitStrategy.GetItemData(hItem);
		pPriority->GetConditionData().AddTaxiCondtion(segData);

		int nCount = pPriority->GetConditionData().getTaxiConditionCount();
		int idx = nCount -1;
		TaxiSegmentData* pSegData = pPriority->GetConditionData().getTaxiCondition(idx);
		
		InsertTaxiwaySegmentData(pSegData,hItem);
	}
}

void CDlgLandingRunwayExit::OnEditTaxiwaySegment()
{
	HTREEITEM hItem = m_wndTreeExitStrategy.GetSelectedItem();
	if (hItem == NULL)
		return;	

	TaxiSegmentData* pSegData = (TaxiSegmentData*)m_wndTreeExitStrategy.GetItemData(hItem);
	CDlgTaxiwaySegmentSelect dlg(m_nProjID,this);
	dlg.SetTaxiwaySegmentData(*pSegData);
	if (dlg.DoModal() == IDOK)
	{
		TaxiSegmentData segData;
		dlg.GetTaxiwaySegmentData(segData);

		*pSegData = segData;

		Taxiway* pTaxiway = m_inputAirside.GetTaxiwayByID(pSegData->m_iTaxiwayID);
		CString strTaxiway = pTaxiway->getName().GetIDString();
		IntersectionNode startNode;
		startNode.ReadData(pSegData->m_iStartNode);
		CString strStart = startNode.GetName();
		IntersectionNode endNode;
		endNode.ReadData(pSegData->m_iEndNode);
		CString strEnd = endNode.GetName();

		CString strText;
		strText.Format(_T("Taxiway [%s] between [%s] & [%s]"),strTaxiway,strStart,strEnd);

		m_wndTreeExitStrategy.SetItemText(hItem,strText);
	}
}

void CDlgLandingRunwayExit::OnDeleteTaxiwaySegment()
{
	HTREEITEM hItem = m_wndTreeExitStrategy.GetSelectedItem();
	if (hItem == NULL)
		return;	
	HTREEITEM hParentItem = m_wndTreeExitStrategy.GetParentItem(hItem);
	RunwayExitPriorityItem* pPriority = (RunwayExitPriorityItem*)m_wndTreeExitStrategy.GetItemData(hParentItem);
	if (pPriority == NULL)
		return;
	
	TaxiSegmentData* pSegData = (TaxiSegmentData*)m_wndTreeExitStrategy.GetItemData(hItem);
	if (pSegData == NULL)
		return;
	
	pPriority->GetConditionData().DeleteTaxiCondition(*pSegData);
	m_wndTreeExitStrategy.DeleteItem(hItem);
}

