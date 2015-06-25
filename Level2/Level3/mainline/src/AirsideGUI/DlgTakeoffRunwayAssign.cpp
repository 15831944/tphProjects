// DlgTakeoffRunwayAssign.cpp : implementation file
//
#include "stdafx.h"
#include "Resource.h"
#include "InputAirside/TakeoffRunwayAssignmentStrategies.h"
#include ".\dlgtakeoffrunwayassign.h"
#include "..\InputAirside\FlightTypeRunwayAssignmentStrategyItem.h"
#include "..\InputAirside\TakeOffTimeRangeRunwayAssignItem.h"
#include "DlgStandFamily.h"
#include "DlgSelectRunway.h"

#include "DlgTimeRange.h"
#include "DlgAirsideIntersectionSelect.h"
#include "..\InputAirside\Runway.h"
#include "DlgTakeoffPositionSelection.h"


#define ID_WND_SPLITTER 0x0001
CDlgTakeoffRunwayAssign::CDlgTakeoffRunwayAssign(int nProjID, PSelectFlightType pSelectFlightType,CAirportDatabase *pAirPortdb,int nAirPortID,CWnd* pParent /*=NULL*/)
: CDialog(CDlgTakeoffRunwayAssign::IDD, pParent)
 ,m_AirportDB(pAirPortdb)
 ,m_Data(nProjID,pAirPortdb)
 ,m_ProID(nProjID)
 ,m_FlightType(pSelectFlightType)
 ,m_nAirPortID(nAirPortID)
{
	m_Data.ReadData() ;
	m_hTimeRangeTreeItem = NULL ;
//	m_dlgTakeoffPos=new CDlgSelectTakeoffPosition (m_ProID,-1,-1, this,true) ;
}

CDlgTakeoffRunwayAssign::~CDlgTakeoffRunwayAssign()
{
}

void CDlgTakeoffRunwayAssign::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_ASSIGNSTRATEGY, m_wndCmbAssignStrategy);
	DDX_Control(pDX, IDC_TREE_FLTTIME, m_wndTreeFltTime);
	DDX_Control(pDX, IDC_TREE_PRIORITY, m_wndListPriority);
	DDX_Control(pDX, IDC_STATIC_FLIGHTTYPE,m_staticFlightType);
	DDX_Control(pDX, IDC_STATIC_STRATEGY_TITLE,m_staticRwyAssign);
}

BEGIN_MESSAGE_MAP(CDlgTakeoffRunwayAssign, CDialog)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_CONTEXTMENU()
	ON_BN_CLICKED(IDC_BUTTON_SAVE, OnSave)

	ON_COMMAND(ID_NEW_FLTTYPE, OnAddLeftToolBar)
	ON_COMMAND(ID_DEL_FLTTYPE, OnDeleteLeftToolBar)
	ON_COMMAND(ID_EDIT_FLTTYPE, OnEditLeftToolBar)

	ON_COMMAND(ID_NEW_RUNWAY, OnAddRightToolBar)
	ON_COMMAND(ID_DEL_RUNWAY, OnDeleteRightToolBar)
	ON_COMMAND(ID_EDIT_RUNWAY, OnEditRightToolBar)

	ON_CBN_SELCHANGE(IDC_COMBO_ASSIGNSTRATEGY, OnAssignmentStrategyChange)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_FLTTIME, OnTvnSelchangedFltTimeTree)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_PRIORITY, OnTvnSelchangedPriorityTree)

	ON_COMMAND(ID_ADD_TIMERANGE, OnAddTimeRange)
	ON_COMMAND(ID_DEL_TIMERANGE, OnDeleteTimeRange)
	ON_COMMAND(ID_EDIT_TIMERANGE, OnEditTimeRange)

	ON_COMMAND(ID_MENU_NEWPRIORITYRWY, OnAddPriorityRunway)
 	ON_COMMAND(ID_MENU_NEWTAKEOFFPOS, OnAddTakeoffPosition)

	ON_COMMAND(ID_TAKEOFFFLTTIME_NEWDEPSTANDFAMILY, OnAddStand)
	ON_COMMAND(ID_TAKEOFFFLTTIME_NEWFLIGHTTYPE, OnAddFlight)
	ON_COMMAND(ID_TAKEOFFFLTTIME_NEWTIMERANGE, OnAddTimeRange)

	ON_COMMAND(ID_STANDPOPMENU_EDIT, OnEditStand)
	ON_COMMAND(ID_STANDPOPMENU_DELETE, OnDeleteStand)

	ON_COMMAND(ID_FLIGHTTYPEPOPMENU_EDIT, OnEditFlight)
	ON_COMMAND(ID_FLIGHTTYPEPOPMENU_DELETE, OnDeleteFlight)

	ON_COMMAND(ID_TIMEPOPMENU_EDIT, OnEditTimeRange)
	ON_COMMAND(ID_TIMEPOPMENU_DELETE, OnDeleteTimeRange)

	ON_COMMAND(ID_MENU_NEWDEPSTAND, OnAddStand)
	ON_COMMAND(ID_STANDPOPMENU_NEWFLIGHTTYPE, OnAddFlight)
	ON_COMMAND(ID_FLIGHTTYPEPOPMENU_NEWTIMEWINDOW, OnAddTimeRange)

	ON_COMMAND(ID_RUNWAYPOPMENU_EDIT, OnEditPriorityRunway)
	ON_COMMAND(ID_RUNWAYPOPMENU_DELETE, OnDeleteRunway)
	ON_COMMAND(ID_TAKEOFFPOSPOPMENU_EDITPOS, OnEditTakeoffPos)
	ON_COMMAND(ID_TAKEOFFPOSPOPMENU_DELETEPOS, OnDeleteTakeoffPosition)

	ON_COMMAND(ID_POPMENU_ADD_INTERSECTION,OnAddIntersection)
	ON_COMMAND(ID_POPMENU_DEL_INTERSECTION,OnDelIntersection)

	
END_MESSAGE_MAP()

void CDlgTakeoffRunwayAssign::EnableRightCtrlView(BOOL _isOrNo)
{
	m_wndListPriority.EnableWindow(_isOrNo) ;
	m_wndRunwayToolbar.EnableWindow(_isOrNo) ;
	m_wndRunwayToolbar.GetToolBarCtrl().EnableButton(ID_NEW_RUNWAY,_isOrNo);
	m_wndRunwayToolbar.GetToolBarCtrl().EnableButton(ID_DEL_RUNWAY,_isOrNo);
	m_wndRunwayToolbar.GetToolBarCtrl().EnableButton(ID_EDIT_RUNWAY,_isOrNo);
}

void CDlgTakeoffRunwayAssign::InitRightCtrlData(CTakeOffTimeRangeRunwayAssignItem* _TimeRangeItem)
{
	if(_TimeRangeItem == NULL)
		return ;
		m_wndCmbAssignStrategy.EnableWindow(TRUE);
	if(_TimeRangeItem->GetTimeRangeType() == ManagedAssignment)
	{
		EnableRightCtrlView(TRUE) ;
		m_wndRunwayToolbar.GetToolBarCtrl().EnableButton(ID_NEW_RUNWAY,TRUE);
		m_wndRunwayToolbar.GetToolBarCtrl().EnableButton(ID_DEL_RUNWAY,FALSE);
		m_wndRunwayToolbar.GetToolBarCtrl().EnableButton(ID_EDIT_RUNWAY,FALSE);

 		m_wndRunwayToolbar.AddDisabledMenuItem(ID_MENU_NEWTAKEOFFPOS);
	}
	else
	{
		m_wndListPriority.DeleteAllItems() ;
		m_wndRunwayToolbar.GetToolBarCtrl().EnableButton(ID_NEW_RUNWAY,FALSE);
		m_wndRunwayToolbar.GetToolBarCtrl().EnableButton(ID_DEL_RUNWAY,FALSE);
		m_wndRunwayToolbar.GetToolBarCtrl().EnableButton(ID_EDIT_RUNWAY,FALSE);
	}
	m_wndCmbAssignStrategy.SetCurSel(_TimeRangeItem->GetTimeRangeType()) ;
	InitPriorityTree(_TimeRangeItem) ;
}

void CDlgTakeoffRunwayAssign::OnTvnSelchangedPriorityTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	HTREEITEM hSelItem = m_wndListPriority.GetSelectedItem() ;
	m_wndRunwayToolbar.ClearDisabledMenuItems();
	if(hSelItem == NULL)
	{
		m_wndRunwayToolbar.AddDisabledMenuItem(ID_MENU_NEWTAKEOFFPOS);
		m_wndRunwayToolbar.GetToolBarCtrl().EnableButton(ID_NEW_RUNWAY,TRUE);
		m_wndRunwayToolbar.GetToolBarCtrl().EnableButton(ID_DEL_RUNWAY,FALSE);
		m_wndRunwayToolbar.GetToolBarCtrl().EnableButton(ID_EDIT_RUNWAY,FALSE);
		return ;
	}
	TY_PRIORITYTREE_NODE NodeType =(TY_PRIORITYTREE_NODE) m_wndListPriority.GetItemNodeInfo(hSelItem)->nMaxCharNum ;

	if(NodeType == TY_TAKEOFF_POSITION_ITEM)
	{
 		m_wndRunwayToolbar.AddDisabledMenuItem(ID_MENU_NEWTAKEOFFPOS);
		m_wndRunwayToolbar.GetToolBarCtrl().EnableButton(ID_NEW_RUNWAY,TRUE);
		m_wndRunwayToolbar.GetToolBarCtrl().EnableButton(ID_DEL_RUNWAY,TRUE);
		m_wndRunwayToolbar.GetToolBarCtrl().EnableButton(ID_EDIT_RUNWAY,FALSE);
		return;
	}

	if(NodeType == TY_PRIORITY)
	{
 		m_wndRunwayToolbar.AddDisabledMenuItem(ID_MENU_NEWTAKEOFFPOS);
		m_wndRunwayToolbar.GetToolBarCtrl().EnableButton(ID_NEW_RUNWAY,TRUE);
		m_wndRunwayToolbar.GetToolBarCtrl().EnableButton(ID_DEL_RUNWAY,TRUE);
		m_wndRunwayToolbar.GetToolBarCtrl().EnableButton(ID_EDIT_RUNWAY,FALSE);
		return;
	}

	if (NodeType == TY_TAKEOFF_POSITION||NodeType == TY_DECISIONPOINT||NodeType == TY_DECISIONPOINT_DEPSTAND\
		||NodeType == TY_DECISIONPOINT_INTERSECTION||NodeType == TY_DECISIONPOINT_INTERSECTION_ITEM||NodeType == TY_BACKTRACK)
// 	{
// 		m_wndRunwayToolbar.GetToolBarCtrl().EnableButton(ID_NEW_RUNWAY,TRUE);
// 		m_wndRunwayToolbar.GetToolBarCtrl().EnableButton(ID_DEL_RUNWAY,FALSE);
// 		m_wndRunwayToolbar.GetToolBarCtrl().EnableButton(ID_EDIT_RUNWAY,TRUE);
// 	}
// 	else
	{
// 		m_wndRunwayToolbar.AddDisabledMenuItem(ID_MENU_NEWTAKEOFFPOS);
		m_wndRunwayToolbar.GetToolBarCtrl().EnableButton(ID_NEW_RUNWAY,TRUE);
		m_wndRunwayToolbar.GetToolBarCtrl().EnableButton(ID_DEL_RUNWAY,FALSE);
		m_wndRunwayToolbar.GetToolBarCtrl().EnableButton(ID_EDIT_RUNWAY,FALSE);
	}
}

void CDlgTakeoffRunwayAssign::OnTvnSelchangedFltTimeTree(NMHDR *pNMHDR, LRESULT *pResult)
{	
	m_wndListPriority.DeleteAllItems() ;
	m_wndCmbAssignStrategy.EnableWindow(FALSE) ;
	EnableRightCtrlView(FALSE) ;

	m_wndFltToolbar.ClearDisabledMenuItems();

	HTREEITEM SelItem = m_wndTreeFltTime.GetSelectedItem() ;

	if (SelItem == NULL)
	{		
		m_wndFltToolbar.AddDisabledMenuItem( ID_TAKEOFFFLTTIME_NEWFLIGHTTYPE );
		m_wndFltToolbar.AddDisabledMenuItem( ID_TAKEOFFFLTTIME_NEWTIMERANGE );

		m_wndFltToolbar.GetToolBarCtrl().EnableButton(ID_NEW_FLTTYPE,TRUE);
		m_wndFltToolbar.GetToolBarCtrl().EnableButton(ID_DEL_FLTTYPE,FALSE);
		m_wndFltToolbar.GetToolBarCtrl().EnableButton(ID_EDIT_FLTTYPE,FALSE);
	}

	if(GetTreeItemNodeType(SelItem) == TY_TREENODE_TIMERANGE)
	{
		m_wndFltToolbar.AddDisabledMenuItem( ID_TAKEOFFFLTTIME_NEWFLIGHTTYPE );
		m_wndFltToolbar.AddDisabledMenuItem( ID_TAKEOFFFLTTIME_NEWTIMERANGE );

		m_wndFltToolbar.GetToolBarCtrl().EnableButton(ID_NEW_FLTTYPE,TRUE);
		m_wndFltToolbar.GetToolBarCtrl().EnableButton(ID_DEL_FLTTYPE,TRUE);
		m_wndFltToolbar.GetToolBarCtrl().EnableButton(ID_EDIT_FLTTYPE,TRUE);
		m_hTimeRangeTreeItem = SelItem ;
		CTakeOffTimeRangeRunwayAssignItem* _timeRangeItem  =  (CTakeOffTimeRangeRunwayAssignItem*)m_wndTreeFltTime.GetItemData(SelItem);
		InitRightCtrlData(_timeRangeItem) ;
		return ;
	}
	else
	{
		
		if (GetTreeItemNodeType(SelItem) == TY_TREENODE_FLIGHT)
			m_wndFltToolbar.AddDisabledMenuItem( ID_TAKEOFFFLTTIME_NEWFLIGHTTYPE );
		if (GetTreeItemNodeType(SelItem) == TY_TREENODE_STAND)
			m_wndFltToolbar.AddDisabledMenuItem( ID_TAKEOFFFLTTIME_NEWTIMERANGE );		

		m_wndFltToolbar.GetToolBarCtrl().EnableButton(ID_NEW_FLTTYPE,TRUE);
		m_wndFltToolbar.GetToolBarCtrl().EnableButton(ID_DEL_FLTTYPE,TRUE);
		m_wndFltToolbar.GetToolBarCtrl().EnableButton(ID_EDIT_FLTTYPE,TRUE);
	}

}

void CDlgTakeoffRunwayAssign::OnSave()
{
	m_Data.WriteData() ;
}

void CDlgTakeoffRunwayAssign::OnOK()
{
	OnSave();
	CDialog::OnOK() ;
}

void CDlgTakeoffRunwayAssign::OnCancel()
{
	m_Data.ReadData() ;
	CDialog::OnCancel() ;
}

int CDlgTakeoffRunwayAssign::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!m_wndFltToolbar.CreateEx(this, TBSTYLE_FLAT,WS_CHILD|WS_VISIBLE|CBRS_ALIGN_TOP,CRect(0,0,0,0),1000)
		|| !m_wndFltToolbar.LoadToolBar(IDR_TOOLBAR_ADD_DEL_EDIT))
	{
		return -1;
	}

	if (!m_wndRunwayToolbar.CreateEx(this,TBSTYLE_FLAT,WS_CHILD|WS_VISIBLE|CBRS_ALIGN_TOP,CRect(0,0,0,0),1001)
		|| !m_wndRunwayToolbar.LoadToolBar(IDR_TOOLBAR_ADD_DEL_EDIT_RUNWAYASSIGN))
	{
		return -1;
	}

	CToolBarCtrl& fltToolbar = m_wndFltToolbar.GetToolBarCtrl();
	fltToolbar.SetCmdID(0, ID_NEW_FLTTYPE);
	fltToolbar.SetCmdID(1, ID_DEL_FLTTYPE);
	fltToolbar.SetCmdID(2, ID_EDIT_FLTTYPE);
	m_wndFltToolbar.AddDropDownBtn(ID_NEW_FLTTYPE, IDR_MENU_TAKEOFFFLTTIME);

	CToolBarCtrl& runwayToolbar = m_wndRunwayToolbar.GetToolBarCtrl();
	runwayToolbar.SetCmdID(0, ID_NEW_RUNWAY);
	runwayToolbar.SetCmdID(1, ID_DEL_RUNWAY);
	runwayToolbar.SetCmdID(2, ID_EDIT_RUNWAY);

	m_wndRunwayToolbar.AddDropDownBtn(ID_NEW_RUNWAY,IDR_MENU_TAKEOFFRWYASSIGN);

	return 0;
}

BOOL CDlgTakeoffRunwayAssign::OnInitDialog()
{
	CDialog::OnInitDialog() ;
	InitStandFlightTree() ;
	
	m_wndListPriority.ShowWindow(SW_SHOW) ;
	GetDlgItem(IDC_LIST_TAKEOFFRUNWAYASSIGN)->ShowWindow(SW_HIDE) ;
	GetDlgItem(IDC_LIST_TAKEOFFRUNWAYASSIGN)->EnableWindow(FALSE) ;
	//////////////////////////////////////////////////////////////////////////
	InitComboBox() ;
	// set the position of the flt toolbar
	CRect rectFltToolbar;
	m_wndTreeFltTime.GetWindowRect(&rectFltToolbar);
	ScreenToClient(&rectFltToolbar);
	rectFltToolbar.top -= 26;
	rectFltToolbar.bottom = rectFltToolbar.top + 22;
	rectFltToolbar.left +=2;
	m_wndFltToolbar.MoveWindow(&rectFltToolbar);

	m_wndFltToolbar.GetToolBarCtrl().EnableButton(ID_NEW_FLTTYPE);
	m_wndFltToolbar.GetToolBarCtrl().EnableButton(ID_DEL_FLTTYPE,FALSE);
	m_wndFltToolbar.GetToolBarCtrl().EnableButton(ID_EDIT_FLTTYPE,FALSE);

	// set the position of the runway toolbar
	CRect rectRunwayToolbar;
	m_wndListPriority.GetWindowRect(&rectRunwayToolbar);
	ScreenToClient(&rectRunwayToolbar);
	rectRunwayToolbar.top -= 26;
	rectRunwayToolbar.bottom = rectRunwayToolbar.top + 22;
	rectRunwayToolbar.left += 4;
	m_wndRunwayToolbar.MoveWindow(&rectRunwayToolbar);

	m_wndRunwayToolbar.GetToolBarCtrl().EnableButton(ID_NEW_RUNWAY);
	m_wndRunwayToolbar.GetToolBarCtrl().EnableButton(ID_DEL_RUNWAY);
	m_wndRunwayToolbar.GetToolBarCtrl().EnableButton(ID_EDIT_RUNWAY);

	//////////////////////////////////////////////////////////////////////////

	CRect rect;
	GetClientRect(rect);
	SetWindowPos(NULL,0,0,rect.Width(),rect.Height(),SWP_NOMOVE|SWP_NOACTIVATE|SWP_NOZORDER);

	m_wndCmbAssignStrategy.EnableWindow(FALSE) ;
	EnableRightCtrlView(FALSE) ;

	m_wndFltToolbar.AddDisabledMenuItem( ID_TAKEOFFFLTTIME_NEWFLIGHTTYPE );
	m_wndFltToolbar.AddDisabledMenuItem( ID_TAKEOFFFLTTIME_NEWTIMERANGE );
	return TRUE ;
}

void CDlgTakeoffRunwayAssign::InitComboBox()
{
	m_wndCmbAssignStrategy.AddString(_T("Flight Planned Runway"));
	m_wndCmbAssignStrategy.AddString(_T("Closest Runway"));
	m_wndCmbAssignStrategy.AddString(_T("Managed assignment"));
	m_wndCmbAssignStrategy.SetCurSel(0);
}

void CDlgTakeoffRunwayAssign::InitStandFlightTree()
{
	for (int i =  0 ; i < (int)m_Data.GetStandData()->size() ;i++)
	{
		AddStandTreeItem(m_Data.GetStandData()->at(i)) ;
	}
}

void CDlgTakeoffRunwayAssign::AddStandTreeItem(CTakeoffStandRunwayAssignItem* _standItem)
{
	if(_standItem == NULL )
		return ;
	HTREEITEM PStandItem = m_wndTreeFltTime.InsertItem(_standItem->GetStand(),TVI_ROOT,TVI_LAST) ;

	m_wndTreeFltTime.SetItemData(PStandItem,(DWORD_PTR)_standItem) ;

	for (int i = 0 ; i < (int)_standItem->m_FlightData.size() ;i++)
	{
		AddFlightTreeItem(_standItem->m_FlightData.at(i),PStandItem) ;
	}
	m_wndTreeFltTime.Expand(PStandItem,TVE_EXPAND) ;
}

void CDlgTakeoffRunwayAssign::EditStandTreeItem(CTakeoffStandRunwayAssignItem* _standItem,HTREEITEM _HStandItem)
{
	if(_standItem == NULL || _HStandItem == NULL)
		return ;
	m_wndTreeFltTime.SetItemText(_HStandItem,_standItem->GetStand()) ;
	m_wndTreeFltTime.SetItemData(_HStandItem,(DWORD_PTR)_standItem) ;
}


void CDlgTakeoffRunwayAssign::AddFlightTreeItem(CTakeoffFlightTypeRunwayAssignment* _fltItem , HTREEITEM _standItem)
{
	if(_fltItem == NULL || _standItem == NULL)
		return ;
	CString str ;
	_fltItem->GetFlightType().screenPrint(str) ;
	HTREEITEM pFlightItem = m_wndTreeFltTime.InsertItem(str,_standItem) ;
	m_wndTreeFltTime.SetItemData(pFlightItem,(DWORD_PTR)_fltItem) ;

	for (int i = 0 ; i < (int)_fltItem->GetTimeRangeData()->size() ;i++)
	{
		AddTimeRangeTreeItem(_fltItem->GetTimeRangeData()->at(i),pFlightItem) ;
	}
	m_wndTreeFltTime.Expand(pFlightItem,TVE_EXPAND) ;
	m_wndTreeFltTime.Expand(_standItem,TVE_EXPAND) ;

}
void CDlgTakeoffRunwayAssign::EditFlightTreeItem(CTakeoffFlightTypeRunwayAssignment* _fltItem,HTREEITEM _FltItem)
{
	if(_fltItem == NULL || _fltItem == NULL)
		return ;
	CString str ;
	_fltItem->GetFlightType().screenPrint(str) ;
	m_wndTreeFltTime.SetItemText(_FltItem,str) ;
	m_wndTreeFltTime.SetItemData(_FltItem,(DWORD_PTR)_fltItem) ;
}

CString CDlgTakeoffRunwayAssign::FormatTimeRangeString(ElapsedTime& _startTime ,ElapsedTime& _endTime)
{
	CString SQL ;
	SQL.Format(_T("%s - %s"),_startTime.PrintDateTime(),_endTime.PrintDateTime()) ;
	return SQL ;
}

void CDlgTakeoffRunwayAssign::AddTimeRangeTreeItem(CTakeOffTimeRangeRunwayAssignItem* _TimeRange , HTREEITEM _FltItem)
{
	if(_TimeRange == NULL || _FltItem == NULL)
		return ;
	CString ItemText ;
	ItemText = FormatTimeRangeString(_TimeRange->GetStartTime(),_TimeRange->GetEndTime()) ;

	HTREEITEM PTimerange = m_wndTreeFltTime.InsertItem(ItemText,_FltItem) ;
	m_wndTreeFltTime.SetItemData(PTimerange,(DWORD_PTR)_TimeRange) ;
	m_wndTreeFltTime.Expand(PTimerange,TVE_EXPAND) ;
	m_wndTreeFltTime.Expand(_FltItem,TVE_EXPAND) ;
}

void CDlgTakeoffRunwayAssign::EditTimeRangeTreeItem(CTakeOffTimeRangeRunwayAssignItem* _TimeRange,HTREEITEM _TimeItem) 
{
	if(_TimeRange == NULL || _TimeItem == NULL)
		return ;
	CString ItemText ;
	ItemText = FormatTimeRangeString(_TimeRange->GetStartTime(),_TimeRange->GetEndTime()) ;
	m_wndTreeFltTime.SetItemText(_TimeItem,ItemText) ;
	m_wndTreeFltTime.SetItemData(_TimeItem,(DWORD_PTR)_TimeRange) ;
}

CDlgTakeoffRunwayAssign::TY_NODE CDlgTakeoffRunwayAssign::GetTreeItemNodeType( HTREEITEM _item )
{
	HTREEITEM HParItem = m_wndTreeFltTime.GetParentItem(_item) ;
	if(HParItem == NULL)
		return TY_TREENODE_STAND ;

	HParItem = m_wndTreeFltTime.GetParentItem(HParItem) ;
	if(HParItem == NULL)
		return TY_TREENODE_FLIGHT ;

	return TY_TREENODE_TIMERANGE ;
}

void CDlgTakeoffRunwayAssign::OnAssignmentStrategyChange()
{
	if(m_hTimeRangeTreeItem == NULL)
		return;

	CTakeOffTimeRangeRunwayAssignItem *pTimeRangeItem = 
		(CTakeOffTimeRangeRunwayAssignItem*)m_wndTreeFltTime.GetItemData(m_hTimeRangeTreeItem);

	//pTimeRangeItem->setStrategyType((RunwayAssignmentStrategyType)m_wndCmbAssignStrategy.GetCurSel());
	pTimeRangeItem->SetTimeRangeType((RunwayAssignmentStrategyType)m_wndCmbAssignStrategy.GetCurSel());

	m_wndTreeFltTime.SetFocus();
	m_wndTreeFltTime.SelectItem(m_hTimeRangeTreeItem);
	
	if(pTimeRangeItem->GetTimeRangeType() == ManagedAssignment)
	{
		EnableRightCtrlView(TRUE) ;
		InitPriorityTree(pTimeRangeItem) ;

		m_wndRunwayToolbar.GetToolBarCtrl().EnableButton(ID_NEW_RUNWAY,TRUE);
		m_wndRunwayToolbar.GetToolBarCtrl().EnableButton(ID_DEL_RUNWAY,FALSE);
		m_wndRunwayToolbar.GetToolBarCtrl().EnableButton(ID_EDIT_RUNWAY,FALSE);
	}
	else
	{
		m_wndListPriority.DeleteAllItems() ;
		m_wndListPriority.EnableWindow(FALSE);
		m_wndRunwayToolbar.GetToolBarCtrl().EnableButton(ID_NEW_RUNWAY,FALSE);
		m_wndRunwayToolbar.GetToolBarCtrl().EnableButton(ID_DEL_RUNWAY,FALSE);
		m_wndRunwayToolbar.GetToolBarCtrl().EnableButton(ID_EDIT_RUNWAY,FALSE);
	}
}

void CDlgTakeoffRunwayAssign::InitPriorityTree(CTakeOffTimeRangeRunwayAssignItem* _PTimeRange)
{
	if(_PTimeRange == NULL)
		return ;
	m_wndListPriority.DeleteAllItems() ;
	if(_PTimeRange->GetTimeRangeType() != ManagedAssignment)
		return;
	
	for (int i = 0 ; i < (int)_PTimeRange->m_Data.size() ;i++)
	{
		if (i==(int)_PTimeRange->m_Data.size()-1)
		{
			((CManageAssignPriorityData*)_PTimeRange->m_Data.at(i))->SetHasChangeCondition(false);
		}else
		{
			((CManageAssignPriorityData*)_PTimeRange->m_Data.at(i))->SetHasChangeCondition(true);
		}
		AddPriorityItem(_PTimeRange->m_Data.at(i), i+1) ;
	}
}

void CDlgTakeoffRunwayAssign::AddPriorityItem(CManageAssignPriorityData* pPriorityData, int nPriority)
{
	CTakeOffPriorityRunwayItem* pTakeoffRwy = pPriorityData->GetRunwayItem();
	ASSERT(pTakeoffRwy);

	CString strName ;
	strName.Format("Priority %d",nPriority);
	pPriorityData->m_Name.Format("%d", nPriority);
		 
	COOLTREE_NODE_INFO info;
	CARCTreeCtrl::InitNodeInfo(this,info);
	info.nMaxCharNum = TY_PRIORITY;
	info.net = NET_NORMAL;

	HTREEITEM hPriorityRwy = m_wndListPriority.InsertItem(strName,info,FALSE,FALSE);
	m_wndListPriority.SetItemData(hPriorityRwy,(DWORD_PTR)pPriorityData);

	info.nMaxCharNum = TY_TAKEOFF_POSITION;
	info.net = NET_SHOW_DIALOGBOX;
	strName = "TakeOff Positions";
	HTREEITEM hPositionItem  = m_wndListPriority.InsertItem(strName,info,FALSE,FALSE,hPriorityRwy,TVI_LAST);
 	m_wndListPriority.SetItemData(hPositionItem, (DWORD_PTR)pTakeoffRwy);

	size_t nSize = pTakeoffRwy->m_TakeOffPosition.size();
	for (size_t i = 0; i < nSize; i++)
	{
		AddTakeOffPositionItem(pTakeoffRwy->m_TakeOffPosition.at(i),hPositionItem) ;
	}
	if(pPriorityData->HasChangeCondition())	
	{
		info.nMaxCharNum = TY_DECISIONPOINT ;
		info.net = NET_NORMAL ;
		HTREEITEM HDecisionPoint = m_wndListPriority.InsertItem(_T("At Decision Point"),info,FALSE,FALSE,hPriorityRwy) ;
		m_wndListPriority.SetItemData(HDecisionPoint, (DWORD_PTR)pTakeoffRwy);


		info.nMaxCharNum = TY_DECISIONPOINT_DEPSTAND ;
		info.net = NET_NORMAL ;
		info.nt=NT_RADIOBTN;
		HTREEITEM HDepStand=m_wndListPriority.InsertItem(_T("Dep Stand"),info,FALSE,FALSE,HDecisionPoint) ;	
		info.nMaxCharNum = TY_DECISIONPOINT_INTERSECTION ;
		HTREEITEM HIntersection=m_wndListPriority.InsertItem(_T("Intersection"),info,FALSE,FALSE,HDecisionPoint) ;
		for (int i=0;i<pTakeoffRwy->GetIntersectionNum();i++)
		{
			info.nMaxCharNum = TY_DECISIONPOINT_INTERSECTION_ITEM;		
			info.nt=NT_NORMAL;
			HTREEITEM HItem=m_wndListPriority.InsertItem(pTakeoffRwy->GetIntersectionName(i),info,FALSE,FALSE,HIntersection) ;
			m_wndListPriority.SetItemData(HItem,(DWORD)pTakeoffRwy->GetIntersectionID(i));
		}
		m_wndListPriority.Expand(HIntersection,TVE_EXPAND) ;

		m_wndListPriority.SetItemData(HDepStand, (DWORD_PTR)DP_DepStand);
		m_wndListPriority.SetItemData(HIntersection, (DWORD_PTR)DP_Intersection);	

		if (pTakeoffRwy->GetDecisionPointType()==DP_DepStand)
		{
			m_wndListPriority.DoRadio(HDepStand);
		}else if (pTakeoffRwy->GetDecisionPointType()==DP_Intersection)
		{
			m_wndListPriority.DoRadio(HIntersection);
		}
		m_wndListPriority.Expand(HDecisionPoint,TVE_EXPAND) ;
	}

	info.nMaxCharNum = TY_BACKTRACK ;
	info.net = NET_COMBOBOX ;
	info.nt=NT_NORMAL;
	CString strBacktrack=_T("Backtrack Option:");
	switch(pTakeoffRwy->GetTakeOffBackTrack())
	{
	case BT_NoBacktrack:
		{
			strBacktrack+=_T("No Backtrack");
			break;
		}
	case BT_MinBacktrack:
		{
			strBacktrack+=_T("Min Backtrack");
			break;
		}
	case BT_MaxBacktrack:
		{
			strBacktrack+=_T("Max Backtrack");
			break;
		}
	}
	HTREEITEM HBackTrack = m_wndListPriority.InsertItem(strBacktrack,info,FALSE,FALSE,hPriorityRwy) ;

	if(pPriorityData->HasChangeCondition())
	{
		info.nMaxCharNum = TY_CONDITION ;
		info.net = NET_NORMAL ;
		info.nt=NT_NORMAL;
		HTREEITEM HChangeCondition = m_wndListPriority.InsertItem(_T("Change Condition"),info,FALSE,FALSE,hPriorityRwy) ;
		m_wndListPriority.SetItemData(HChangeCondition, (DWORD_PTR)pTakeoffRwy);

		info.net = NET_EDIT_INT ;
		info.nMaxCharNum = TY_NUMBER ;
		HTREEITEM hItemText = m_wndListPriority.InsertItem(FormatNumberOfAircraft(pPriorityData->m_AircraftNum),info,FALSE,FALSE,HChangeCondition) ;
		
		info.nMaxCharNum = TY_TIME ;
		m_wndListPriority.InsertItem(FormatLastAircratTime(pPriorityData->m_time),info,FALSE,FALSE,HChangeCondition) ;

		info.net = NET_NORMAL ;
		info.nt = NT_CHECKBOX ;
		info.nMaxCharNum = TY_TAKEOFF;
		HTREEITEM hTakeOff = m_wndListPriority.InsertItem(_T("Departing ac could not takeoff within slot time"),info,FALSE,FALSE,HChangeCondition) ;
		m_wndListPriority.SetEnableStatus(hTakeOff,TRUE);
		m_wndListPriority.SetCheckStatus(hTakeOff,pTakeoffRwy->CanTakeOffInSlotTime());
		

		info.nMaxCharNum = TY_TRAIL;
		HTREEITEM hTrail = m_wndListPriority.InsertItem(_T("Departing ac has factor for trailing ac separation"),info,FALSE,FALSE,HChangeCondition) ;
		m_wndListPriority.SetEnableStatus(hTrail,TRUE);
		m_wndListPriority.SetCheckStatus(hTrail,pTakeoffRwy->NoFactorForTrailingACSep());
		m_wndListPriority.Expand(HChangeCondition,TVE_EXPAND) ;
	}

	

	m_wndListPriority.Expand(hPriorityRwy,TVE_EXPAND) ;
}

CString CDlgTakeoffRunwayAssign::FormatNumberOfAircraft(int _number)
{
	CString SQL ;
	SQL.Format(_T("Number of A/C taxing or in Q for position(s) > %d aircraft"),_number) ;
	return SQL ;
}

CString CDlgTakeoffRunwayAssign::FormatLastAircratTime(int _time)
{
	CString SQL ;
	SQL.Format(_T("Last aircraft in Q at these positions waiting > %d mins"),_time) ;
	return SQL ;
}

CString CDlgTakeoffRunwayAssign::FormatTakeOffItemText(CTakeOffPosition* _str)
{
	CString SQL ;
	SQL.Format(_T("Take off postion:%s (%0.2f%%)"),_str->m_strName,_str->m_dPercent) ;
	return SQL ;
}

void CDlgTakeoffRunwayAssign::AddTakeOffPositionItem(CTakeOffPosition* _positionStr,HTREEITEM _PositionsItem)
{
	COOLTREE_NODE_INFO info ;
	CARCTreeCtrl::InitNodeInfo(this,info) ;
	info.nMaxCharNum = TY_TAKEOFF_POSITION_ITEM ;
	info.bAutoSetItemText = FALSE ;
	info.net = NET_EDIT_INT ;

	CString Str ;
	Str = FormatTakeOffItemText(_positionStr)  ;
	HTREEITEM _TakeOffItem = m_wndListPriority.InsertItem(Str,info,FALSE,FALSE,_PositionsItem,TVI_FIRST) ;
	m_wndListPriority.SetItemData(_TakeOffItem,(DWORD_PTR)_positionStr) ;
	m_wndListPriority.Expand(_TakeOffItem,TVE_EXPAND);
	m_wndListPriority.Expand(_PositionsItem,TVE_EXPAND);
}

void CDlgTakeoffRunwayAssign::EditTakeOffPositionItem(CTakeOffPosition* _positionStr,HTREEITEM _takeoffPositionItem)
{
	CString Str ;
	Str = FormatTakeOffItemText(_positionStr)  ;
	m_wndListPriority.SetItemText(_takeoffPositionItem,Str) ;
	m_wndListPriority.SetItemData(_takeoffPositionItem,(DWORD_PTR)_positionStr) ;
}
//////////////////////////////////////////////////////////////////////////
void CDlgTakeoffRunwayAssign::OnAddLeftToolBar()
{
	HTREEITEM HSelItem = m_wndTreeFltTime.GetSelectedItem() ;
	if(HSelItem == NULL)
	{
		OnAddStand();
		return ;
	}
	switch(GetTreeItemNodeType(HSelItem))
	{
		case TY_TREENODE_STAND:
			OnAddFlight() ;
			break ;
		case TY_TREENODE_FLIGHT:
			OnAddTimeRange() ;
			break ;
	}
}
void CDlgTakeoffRunwayAssign::OnEditLeftToolBar()
{
	HTREEITEM HSelItem = m_wndTreeFltTime.GetSelectedItem() ;
	if(HSelItem == NULL)
		return ;
	switch(GetTreeItemNodeType(HSelItem))
	{
	case TY_TREENODE_STAND:
		OnEditStand() ;
		break ;
	case TY_TREENODE_FLIGHT:
		OnEditFlight();
		break;
	case TY_TREENODE_TIMERANGE:
		OnEditTimeRange();
		break ;
	}
}
void CDlgTakeoffRunwayAssign::OnDeleteLeftToolBar()
{
	HTREEITEM HSelItem = m_wndTreeFltTime.GetSelectedItem() ;
	if(HSelItem == NULL)
		return ;
	switch(GetTreeItemNodeType(HSelItem))
	{
	case TY_TREENODE_STAND:
		OnDeleteStand() ;
		break ;
	case TY_TREENODE_FLIGHT:
		OnDeleteFlight();
		break;
	case TY_TREENODE_TIMERANGE:
		OnDeleteTimeRange();
		break ;
	}
}
void CDlgTakeoffRunwayAssign::OnAddStand()
{
	CDlgStandFamily dlg(m_ProID,NULL,this) ;

	if(dlg.DoModal() == IDOK)
	{
		CString StandValue ;
		StandValue = dlg.GetSelStandFamilyName();
		if(StandValue.IsEmpty())
		{
			MessageBox(_T("You should select one stand group"),MB_OK) ;
			return ;
		}
		CTakeoffStandRunwayAssignItem* PStandItem =	m_Data.AddTakeoffStandItem(StandValue) ;
		if(PStandItem == NULL)
		{
			CString Errormsg ;
			Errormsg.Format(_T("%s has selected !"),StandValue) ;
			MessageBox(Errormsg,MB_OK) ;
			return ;
		}
		AddStandTreeItem(PStandItem) ;
	}
}
void CDlgTakeoffRunwayAssign::OnEditStand()
{
	HTREEITEM HSelItem = m_wndTreeFltTime.GetSelectedItem() ;

	CTakeoffStandRunwayAssignItem* PStandItem = (CTakeoffStandRunwayAssignItem*)m_wndTreeFltTime.GetItemData(HSelItem) ;
	if(PStandItem == NULL)
		return ;
	CDlgStandFamily dlg(m_ProID,NULL,this) ;
	if(dlg.DoModal() == IDOK)
	{
		CString StandValue ;
		StandValue = dlg.GetSelStandFamilyName() ;
		if(StandValue.IsEmpty())
		{
			MessageBox(_T("You should select one stand group"),MB_OK) ;
			return ;
		}
		if(m_Data.FindStandItem(StandValue) == PStandItem)
		{
			CString Errormsg ;
			Errormsg.Format(_T("%s has selected !"),StandValue) ;
			MessageBox(Errormsg,MB_OK) ;
			return ;
		}
		PStandItem->SetStand(StandValue) ;
		EditStandTreeItem(PStandItem,HSelItem) ;
	}
}
void CDlgTakeoffRunwayAssign::OnDeleteStand()
{
	HTREEITEM HSelItem = m_wndTreeFltTime.GetSelectedItem() ;

	CTakeoffStandRunwayAssignItem* PStandItem = (CTakeoffStandRunwayAssignItem*)m_wndTreeFltTime.GetItemData(HSelItem) ;

	m_Data.RemoveStandItem(PStandItem) ;

	m_wndTreeFltTime.DeleteItem(HSelItem) ;
}

void CDlgTakeoffRunwayAssign::OnAddFlight()
{
	HTREEITEM HSelItem = m_wndTreeFltTime.GetSelectedItem() ;

	CTakeoffStandRunwayAssignItem* PStandItem = (CTakeoffStandRunwayAssignItem*)m_wndTreeFltTime.GetItemData(HSelItem) ;

	if(PStandItem == NULL)
		return ;

	FlightConstraint pSelFltType = (*m_FlightType)(NULL) ;

	CTakeoffFlightTypeRunwayAssignment* PFlt_Type =   PStandItem->AddTakeoffFlightType(pSelFltType) ;
	if(PFlt_Type == NULL)
	{
		CString Errormsg ;
		CString fltStr ;
		pSelFltType.screenPrint(fltStr) ;
		Errormsg.Format(_T("%s has selected !"),fltStr) ;
		MessageBox(Errormsg,MB_OK) ;
		return ;
	}
	AddFlightTreeItem(PFlt_Type,HSelItem) ;
}

void CDlgTakeoffRunwayAssign::OnEditFlight()
{
	HTREEITEM HSelItem = m_wndTreeFltTime.GetSelectedItem() ;
	HTREEITEM HParItem = m_wndTreeFltTime.GetParentItem(HSelItem) ;
	CTakeoffStandRunwayAssignItem* PStandItem = (CTakeoffStandRunwayAssignItem*)m_wndTreeFltTime.GetItemData(HParItem) ;

	if(PStandItem == NULL)
		return ;
	CTakeoffFlightTypeRunwayAssignment* PFlt_type = (CTakeoffFlightTypeRunwayAssignment*)m_wndTreeFltTime.GetItemData(HSelItem) ;
	if(PFlt_type == NULL)
		return ;
	FlightConstraint pSelFltType = (*m_FlightType)(NULL) ;

	if(PStandItem->FindFlightTypeItem(pSelFltType) == PFlt_type)
	{
		CString Errormsg ;
		CString fltStr ;
		pSelFltType.screenPrint(fltStr) ;
		Errormsg.Format(_T("%s has selected !"),fltStr) ;
		MessageBox(Errormsg,MB_OK) ;
		return ;
	}

	PFlt_type->setFlightType(pSelFltType) ;
	EditFlightTreeItem(PFlt_type,HSelItem) ;
}

void CDlgTakeoffRunwayAssign::OnDeleteFlight()
{
	HTREEITEM HSelItem = m_wndTreeFltTime.GetSelectedItem() ;
	HTREEITEM HParItem = m_wndTreeFltTime.GetParentItem(HSelItem) ;
	CTakeoffStandRunwayAssignItem* PStandItem = (CTakeoffStandRunwayAssignItem*)m_wndTreeFltTime.GetItemData(HParItem) ;

	if(PStandItem == NULL)
		return ;
	CTakeoffFlightTypeRunwayAssignment* PFlt_type = (CTakeoffFlightTypeRunwayAssignment*)m_wndTreeFltTime.GetItemData(HSelItem) ;
	if(PFlt_type == NULL)
		return ;

	PStandItem->RemoveTakeOffFlightType(PFlt_type) ;
	m_wndTreeFltTime.DeleteItem(HSelItem) ;
}

void CDlgTakeoffRunwayAssign::OnAddTimeRange()
{
	HTREEITEM HSelItem = m_wndTreeFltTime.GetSelectedItem() ;
	CTakeoffFlightTypeRunwayAssignment* PFlt_type = (CTakeoffFlightTypeRunwayAssignment*)m_wndTreeFltTime.GetItemData(HSelItem) ;
	if(PFlt_type == NULL)
		return ;
	ElapsedTime StartTime ;
	ElapsedTime EndTime ;
	CDlgTimeRange dlg(StartTime,EndTime) ;

	if(dlg.DoModal() == IDOK)
	{
		TimeRange Ptime ;
		Ptime.SetStartTime(dlg.GetStartTime()) ;
		Ptime.SetEndTime(dlg.GetEndTime()) ;
		CTakeOffTimeRangeRunwayAssignItem* PTimeRange = PFlt_type->AddTakeoffTimeRangeeItem(Ptime) ;
		if(PTimeRange == NULL)
		{			
			CString Errormsg ;
			CString time = FormatTimeRangeString(Ptime.GetStartTime(),Ptime.GetEndTime()) ;
			Errormsg.Format(_T("This time \'%s\' is invalid !"),time) ;
			MessageBox(Errormsg,MB_OK) ;
			return ;
		}
		AddTimeRangeTreeItem(PTimeRange,HSelItem) ;
	}
}

void CDlgTakeoffRunwayAssign::OnEditTimeRange()
{
	HTREEITEM HSelItem = m_wndTreeFltTime.GetSelectedItem() ;
	HTREEITEM HParItem = m_wndTreeFltTime.GetParentItem(HSelItem) ;
	CTakeoffFlightTypeRunwayAssignment* pFlightData = (CTakeoffFlightTypeRunwayAssignment*)m_wndTreeFltTime.GetItemData(HParItem) ;
	if(pFlightData == NULL)
		return ;

	CTakeOffTimeRangeRunwayAssignItem* pTimeRange = (CTakeOffTimeRangeRunwayAssignItem*)m_wndTreeFltTime.GetItemData(HSelItem) ;
	if(pTimeRange == NULL)
		return ;
	CDlgTimeRange dlg(pTimeRange->GetStartTime(),pTimeRange->GetEndTime()) ;
	if(dlg.DoModal() == IDOK)
	{
		TimeRange timeRange ;
		timeRange.SetStartTime(dlg.GetStartTime()) ;
		timeRange.SetEndTime(dlg.GetEndTime()) ;

		if(pFlightData->FindTakeoffTimeRangeeItem(timeRange) != pTimeRange)
		{
			CString Errormsg ;
			CString time = FormatTimeRangeString(timeRange.GetStartTime(),timeRange.GetEndTime()) ;
			Errormsg.Format(_T("This time \'%s\' is invalid !"),time) ;
			MessageBox(Errormsg,MB_OK) ;
			return ;
		}

		pTimeRange->SetStartTime(timeRange.GetStartTime()) ;
		pTimeRange->SetEndTime(timeRange.GetEndTime()) ;
		EditTimeRangeTreeItem(pTimeRange,HSelItem) ;
	}
}

void CDlgTakeoffRunwayAssign::OnDeleteTimeRange()
{
	HTREEITEM HSelItem = m_wndTreeFltTime.GetSelectedItem() ;
	HTREEITEM HParItem = m_wndTreeFltTime.GetParentItem(HSelItem) ;

	CTakeoffFlightTypeRunwayAssignment* PFlt_type = (CTakeoffFlightTypeRunwayAssignment*)m_wndTreeFltTime.GetItemData(HParItem) ;
	if(PFlt_type == NULL)
		return ;

	CTakeOffTimeRangeRunwayAssignItem* PTimeItem = (CTakeOffTimeRangeRunwayAssignItem*)m_wndTreeFltTime.GetItemData(HSelItem) ;
	if(PTimeItem == NULL)
		return ;
	
	PFlt_type->RemoveTakeoffTimeRangeeItem(PTimeItem) ;

	m_wndTreeFltTime.DeleteItem(HSelItem) ;
	m_wndListPriority.DeleteAllItems() ;
}
//////////////////////////////////////////////////////////////////////////

void CDlgTakeoffRunwayAssign::OnAddRightToolBar()
{
	HTREEITEM HSelItem = m_wndListPriority.GetSelectedItem() ;
	if(HSelItem == NULL)
	{
		OnAddPriorityRunway() ;
		return ;
	}
	TY_PRIORITYTREE_NODE NodeType = (TY_PRIORITYTREE_NODE)m_wndListPriority.GetItemNodeInfo(HSelItem)->nMaxCharNum ;
	if (NodeType == TY_PRIORITY)
	{
		OnAddPriorityRunway() ;
	}
	if (NodeType == TY_TAKEOFF_POSITION)
	{		
		OnAddTakeoffPosition() ;
	}

}

void CDlgTakeoffRunwayAssign::OnAddPriorityRunway()
{
	if(m_hTimeRangeTreeItem == NULL)
		return;

	CTakeOffTimeRangeRunwayAssignItem *pTimeRangeItem = (CTakeOffTimeRangeRunwayAssignItem*)m_wndTreeFltTime.GetItemData(m_hTimeRangeTreeItem);

// 	CDlgSelectRunway dlg(m_ProID);
// 	if(IDOK != dlg.DoModal())
// 		return;

// 	int nRunwayID = dlg.GetSelectedRunway();

// 	Runway altRunway;
// 	altRunway.ReadObject(nRunwayID);
// 	CString strMarking = (RUNWAYMARK_FIRST == dlg.GetSelectedRunwayMarkingIndex() ? altRunway.GetMarking1().c_str() : altRunway.GetMarking2().c_str());

	int nPriority = (int)pTimeRangeItem->m_Data.size() +1;
	CString strPriority;
	strPriority.Format("%d",nPriority);
	CManageAssignPriorityData* pPriorityData = pTimeRangeItem->AddPriorityRwyData(strPriority,-1,-1);

	pPriorityData->SetHasChangeCondition(false);

	InitPriorityTree(pTimeRangeItem);
// 	if (pPriorityData == NULL)
// 	{
// 		CString ErrorMsg ;
// 		ErrorMsg.Format(_T("%s has been selected!"),strMarking) ;
// 		MessageBox(ErrorMsg,_T("Warning"),MB_OK) ;
// 		return;
// 	}

// 	pPriorityData->GetRunwayItem()->SetRunwayName( strMarking );
	//AddPriorityItem(pPriorityData, nPriority);

}

void CDlgTakeoffRunwayAssign::OnEditPriorityRunway()
{
	if(m_hTimeRangeTreeItem == NULL)
		return;

	CTakeOffTimeRangeRunwayAssignItem *pTimeRangeItem = (CTakeOffTimeRangeRunwayAssignItem*)m_wndTreeFltTime.GetItemData(m_hTimeRangeTreeItem);

	HTREEITEM hSelItem = m_wndListPriority.GetSelectedItem() ;
	HTREEITEM hParItem = m_wndListPriority.GetParentItem(hSelItem);
	CManageAssignPriorityData* pManageAssign = (CManageAssignPriorityData*)m_wndListPriority.GetItemData(hParItem) ;

	if( pManageAssign == NULL)
		return ;

	CDlgSelectRunway dlg(m_ProID);
	if(IDOK != dlg.DoModal())
		return;

	int nRunwayID = dlg.GetSelectedRunway();

	Runway altRunway;
	altRunway.ReadObject(nRunwayID);	
	CString strMarking = (RUNWAYMARK_FIRST == dlg.GetSelectedRunwayMarkingIndex() ? altRunway.GetMarking1().c_str() : altRunway.GetMarking2().c_str());

	CManageAssignPriorityData* pExistData = pTimeRangeItem->FindPriorityRwyData(nRunwayID, dlg.GetSelectedRunwayMarkingIndex());
	if ( pExistData && pExistData != pManageAssign )
	{
		CString ErrorMsg ;
		ErrorMsg.Format(_T("%s has been selected!"),strMarking) ;
		MessageBox(ErrorMsg,_T("Warning"),MB_OK) ;
		return ;
	}
	
	CTakeOffPriorityRunwayItem* pRwyItem = pManageAssign->GetRunwayItem();

	pRwyItem->m_strRwyName = strMarking ;
	pRwyItem->m_RunwayID = nRunwayID ;
	pRwyItem->m_nRwyMark = dlg.GetSelectedRunwayMarkingIndex() ;

	CString strName;
	strName.Format("Runway %s", strMarking);
	m_wndListPriority.SetItemText(hSelItem,strName);
}

BOOL CDlgTakeoffRunwayAssign::CheckThePercentvaild(HTREEITEM _TakeoffPosition ,double _floatval)
{
	HTREEITEM _RunwayItem = m_wndListPriority.GetParentItem(_TakeoffPosition) ;
	if(_RunwayItem == NULL)
		return FALSE;
	HTREEITEM HChildeItem = m_wndListPriority.GetChildItem(_RunwayItem) ;
	int ItemCount = 0 ;
	CTakeOffPosition* pTakeOffPosition = NULL ;
	double PPercent = 0 ;
	while(HChildeItem)
	{
		pTakeOffPosition = (CTakeOffPosition*)m_wndListPriority.GetItemData(HChildeItem) ;
		if(pTakeOffPosition)
		{
			if(HChildeItem != _TakeoffPosition)
				 PPercent = PPercent + pTakeOffPosition->m_dPercent ;
			else
				PPercent = PPercent + _floatval ;
		}
		HChildeItem = m_wndListPriority.GetNextSiblingItem(HChildeItem) ;
	}
	if(PPercent < 0 || PPercent > 100)
		return FALSE ;
	else
		return TRUE ;
}

void CDlgTakeoffRunwayAssign::OnAddTakeoffPosition()
{
	HTREEITEM hSelItem = m_wndListPriority.GetSelectedItem() ;
	if (hSelItem == NULL)
		return;

	CTakeOffPriorityRunwayItem* PRwyItem= (CTakeOffPriorityRunwayItem*)m_wndListPriority.GetItemData(hSelItem) ;

 	if(PRwyItem == NULL)
 		return ;
 	CDlgSelectTakeoffPosition *m_dlgTakeoffPos=new CDlgSelectTakeoffPosition(m_ProID,-1,-1, this,true) ;
	m_dlgTakeoffPos->SetAirportDB(m_AirportDB);

	if(m_dlgTakeoffPos->DoModal() != IDOK)
		return;
	
	std::vector<int> vIDs = m_dlgTakeoffPos->m_vrSelID;
	std::vector<RunwayExit> vExitList = m_dlgTakeoffPos->m_vRunwayExitList;

	size_t nSize = vExitList.size();
	int nCount = 0;
	for (size_t i =0; i < nSize; i++)
	{
		RunwayExit exit = vExitList.at(i);
		if(PRwyItem->FindTakeoffPosition(exit))
		{
			CString ErrorMsg ;
			ErrorMsg.Format(_T("%s has selected!"),m_dlgTakeoffPos->m_arSelName.GetAt((int)i)) ;
			MessageBox(ErrorMsg,_T("Warning"),MB_OK) ;
			continue ;
		}

		CTakeOffPosition* pTakeoffPos =  PRwyItem->AddTakeoffPosition(m_dlgTakeoffPos->m_arSelName.GetAt((int)i),exit) ;
		AddTakeOffPositionItem(pTakeoffPos,hSelItem) ;
	}

	EditTakeoffPositionPercent(PRwyItem, hSelItem);

}
//////////////////////////////////////////////////////////////////////////
void CDlgTakeoffRunwayAssign::OnEditRightToolBar()
{
	HTREEITEM HSelItem = m_wndListPriority.GetSelectedItem() ;
	if(HSelItem == NULL)
		return ;


	TY_PRIORITYTREE_NODE NodeType = (TY_PRIORITYTREE_NODE)m_wndListPriority.GetItemNodeInfo(HSelItem)->nMaxCharNum ;
	if(NodeType == TY_TAKEOFF_POSITION)
	{
		OnEditPriorityRunway() ;
	}
}

void CDlgTakeoffRunwayAssign::EditTakeoffPositionPercent(CTakeOffPriorityRunwayItem* pRwyItem, HTREEITEM hRwyItem)
{
	if(pRwyItem == NULL)
		return;

	size_t nSize = pRwyItem->m_TakeOffPosition.size();

	if (nSize == 0)
	{
		return;
	}
	int nPercent = 100/((int)nSize);
	for (size_t i =0; i < nSize; i++)
	{
		if (i == nSize -1)
		{
			pRwyItem->m_TakeOffPosition.at(i)->m_dPercent = 100 - nPercent*((int)nSize -1);
			break;
		}
		pRwyItem->m_TakeOffPosition.at(i)->m_dPercent = nPercent;
	}

	HTREEITEM hChildItem = m_wndListPriority.GetChildItem(hRwyItem);
	while(hChildItem != NULL)
	{
		TY_PRIORITYTREE_NODE NodeType = (TY_PRIORITYTREE_NODE)m_wndListPriority.GetItemNodeInfo(hChildItem)->nMaxCharNum ;
		if(NodeType == TY_TAKEOFF_POSITION_ITEM)
		{
			CTakeOffPosition* pTakeoffPos = (CTakeOffPosition*)m_wndListPriority.GetItemData(hChildItem);
			EditTakeOffPositionItem(pTakeoffPos,hChildItem) ;
		}

		HTREEITEM hNextItem = m_wndListPriority.GetNextItem(hChildItem, TVGN_NEXT);
		hChildItem = hNextItem;

	}

	
}
//////////////////////////////////////////////////////////////////////////

void CDlgTakeoffRunwayAssign::OnDeleteRightToolBar()
{
	HTREEITEM HSelItem = m_wndListPriority.GetSelectedItem() ;
	if(HSelItem == NULL)
	{
		return ;
	}
	TY_PRIORITYTREE_NODE NodeType = (TY_PRIORITYTREE_NODE)m_wndListPriority.GetItemNodeInfo(HSelItem)->nMaxCharNum ;
	switch(NodeType)
	{
	case TY_TAKEOFF_POSITION_ITEM:
		OnDeleteTakeoffPosition() ;
		break ;
	case TY_PRIORITY:
		OnDeletePriorityRunway() ;
		break ;
	}
}

void CDlgTakeoffRunwayAssign::OnDeletePriorityRunway()
{
	if(m_hTimeRangeTreeItem == NULL)
		return;

	HTREEITEM HSelItem = m_wndListPriority.GetSelectedItem() ;
	if(HSelItem == NULL)
		return ;

	CTakeOffTimeRangeRunwayAssignItem *pTimeRangeItem = (CTakeOffTimeRangeRunwayAssignItem*)m_wndTreeFltTime.GetItemData(m_hTimeRangeTreeItem);

	CManageAssignPriorityData* pManageAssign = (CManageAssignPriorityData*)m_wndListPriority.GetItemData(HSelItem) ;

	pTimeRangeItem->RemovePriorityRwyData(pManageAssign) ;
	m_wndListPriority.DeleteItem(HSelItem);

	InitPriorityTree(pTimeRangeItem);
}

void CDlgTakeoffRunwayAssign::OnDeleteTakeoffPosition()
{
	HTREEITEM HSelItem = m_wndListPriority.GetSelectedItem() ;
	CTakeOffPosition* pTakeOffPosition = (CTakeOffPosition*) m_wndListPriority.GetItemData(HSelItem) ;
	if(pTakeOffPosition == NULL)
		return ;
	HTREEITEM HParItem = m_wndListPriority.GetParentItem(HSelItem) ;
	CTakeOffPriorityRunwayItem* pRwyItem = (CTakeOffPriorityRunwayItem*)m_wndListPriority.GetItemData(HParItem) ;

	if(pRwyItem == NULL)
		return ;
	pRwyItem->RemoveTakeoffPosition(pTakeOffPosition) ;
	m_wndListPriority.DeleteItem(HSelItem) ;

}
void CDlgTakeoffRunwayAssign::OnAddIntersection() 
{
	CDlgAirsideIntersectionSelect dlg(m_nAirPortID);
	if(dlg.DoModal()==IDOK)
	{
		HTREEITEM hSelItem=m_wndListPriority.GetSelectedItem();
		if (hSelItem==NULL)
		{
			return;
		}
		HTREEITEM hParentItem=m_wndListPriority.GetParentItem(hSelItem);
		if (hParentItem==NULL)
		{
			return;
		}
		CTakeOffPriorityRunwayItem *runwayItem=(CTakeOffPriorityRunwayItem *)m_wndListPriority.GetItemData(hParentItem);
		if (runwayItem==NULL)
		{
			return;
		}
		if (runwayItem->IntersectionExist(dlg.GetSelIntersectionID()))
		{
			MessageBox(_T("Intersection Exist!"));
			return;
		}
		int nIndex=runwayItem->AddIntersection(dlg.GetSelIntersectionID());

		COOLTREE_NODE_INFO info;
		CARCTreeCtrl::InitNodeInfo(this,info);
		info.nMaxCharNum = TY_DECISIONPOINT_INTERSECTION_ITEM;		
		info.nt=NT_NORMAL;
		info.net = NET_NORMAL;
		HTREEITEM HItem=m_wndListPriority.InsertItem(runwayItem->GetIntersectionName(nIndex),info,FALSE,FALSE,hSelItem) ;
		m_wndListPriority.SetItemData(HItem,(DWORD)runwayItem->GetIntersectionID(nIndex));

		m_wndListPriority.Expand(hSelItem,TVE_EXPAND);

	}
	
}
void CDlgTakeoffRunwayAssign::OnDelIntersection() 
{
	HTREEITEM hSelItem=m_wndListPriority.GetSelectedItem();
	if (hSelItem==NULL)
	{
		return;
	}
	HTREEITEM hIntersectionItem=m_wndListPriority.GetParentItem(hSelItem);
	if (hIntersectionItem==NULL)
	{
		return;
	}
	HTREEITEM hRunwayItem=m_wndListPriority.GetParentItem(hIntersectionItem);
	CTakeOffPriorityRunwayItem *runwayItem=(CTakeOffPriorityRunwayItem *)m_wndListPriority.GetItemData(hRunwayItem);
	if (runwayItem==NULL)
	{
		return;
	}
    int nID=(int)m_wndListPriority.GetItemData(hSelItem);
	runwayItem->DelIntersection(nID);
	m_wndListPriority.DeleteItem(hSelItem);

	CTakeOffTimeRangeRunwayAssignItem *pTimeRangeItem = (CTakeOffTimeRangeRunwayAssignItem*)m_wndTreeFltTime.GetItemData(m_hTimeRangeTreeItem);

	InitPriorityTree(pTimeRangeItem);
}
LRESULT CDlgTakeoffRunwayAssign::DefWindowProc( UINT message, WPARAM wParam, LPARAM lParam )
{
	switch(message)
	{
	case UM_CEW_SHOW_DIALOGBOX_BEGIN:
		{
// 			HTREEITEM hItem = (HTREEITEM)wParam;
// 			CTakeOffPriorityRunwayItem* pRwyItem = (CTakeOffPriorityRunwayItem*)m_wndListPriority.GetItemData(hItem) ;
// 			if(pRwyItem )
// 			{
// 				m_wndListPriority.SetItemState(hItem,TVIS_SELECTED,TVIS_SELECTED) ;
// 				OnEditPriorityRunway() ;
// 			}
		}
		break;
	case UM_CEW_EDITSPIN_BEGIN:
		{
			HTREEITEM hItem = (HTREEITEM)wParam;
			CWnd* pWnd = m_wndListPriority.GetEditWnd(hItem);
			
			TY_PRIORITYTREE_NODE NodeType = (TY_PRIORITYTREE_NODE)m_wndListPriority.GetItemNodeInfo(hItem)->nMaxCharNum ;

			CString strText;
			if(NodeType == TY_TAKEOFF_POSITION_ITEM)
			{
				CTakeOffPosition* pTakeOffPosition = (CTakeOffPosition*)m_wndListPriority.GetItemData(hItem) ;
				strText.Format("%d", (int)pTakeOffPosition->m_dPercent);
			}
			else
			{
				HTREEITEM hParItem  =  m_wndListPriority.GetParentItem(hItem) ;
				hParItem = m_wndListPriority.GetParentItem(hParItem) ;
				CManageAssignPriorityData* PriorityData = (CManageAssignPriorityData*)m_wndListPriority.GetItemData(hParItem) ;

				if(NodeType == TY_NUMBER)
					strText.Format("%d", PriorityData->m_AircraftNum);
				else
					strText.Format("%d", PriorityData->m_time);
			}
			pWnd->SetWindowText(strText);
			break;
		}
	case UM_CEW_EDITSPIN_END:
		{
			HTREEITEM hItem = (HTREEITEM)wParam;
			CString strValue = *((CString*)lParam);
			int ValFloat = atoi(strValue) ;

			TY_PRIORITYTREE_NODE NodeType = (TY_PRIORITYTREE_NODE)m_wndListPriority.GetItemNodeInfo(hItem)->nMaxCharNum ;
			if(NodeType == TY_TAKEOFF_POSITION_ITEM)
			{
				HTREEITEM hParItem  =  m_wndListPriority.GetParentItem(hItem) ;
				CTakeOffPriorityRunwayItem* pRwyItem = (CTakeOffPriorityRunwayItem*)m_wndListPriority.GetItemData(hParItem) ;

				HTREEITEM _item = (HTREEITEM)wParam;
				CString strValue = *((CString*)lParam);
				double ValFloat = atof(strValue) ;
				if(!CheckThePercentvaild(_item,ValFloat))
				{
					MessageBox(_T("The total percent should be 100% !"),_T("Warning"),MB_OK) ;
					break ;
				}
				CTakeOffPosition* pTakeOffPosition = (CTakeOffPosition*)m_wndListPriority.GetItemData(_item) ;
				pTakeOffPosition->m_dPercent =  ValFloat ;
				EditTakeOffPositionItem(pTakeOffPosition,_item) ;
			}
			else
			{
				HTREEITEM hParItem  =  m_wndListPriority.GetParentItem(hItem) ;
				hParItem = m_wndListPriority.GetParentItem(hParItem) ;
				CManageAssignPriorityData* PriorityData = (CManageAssignPriorityData*)m_wndListPriority.GetItemData(hParItem) ;

				if(NodeType == TY_NUMBER)
				{
					PriorityData->m_AircraftNum = ValFloat;
					m_wndListPriority.SetItemText(hItem,FormatNumberOfAircraft(ValFloat)) ;
				}
				if(NodeType == TY_TIME)
				{
					PriorityData->m_time = ValFloat;
					m_wndListPriority.SetItemText(hItem,FormatLastAircratTime(ValFloat)) ;
				}
			}
		}
		break ;
	case WM_NOTIFY:
		{
			if(wParam ==ID_WND_SPLITTER)
			{
				SPC_NMHDR* pHdr = (SPC_NMHDR*) lParam;
				DoResize(pHdr->delta,wParam);
			}	
		}
		break;
	case UM_CEW_STATUS_CHANGE:
		{
			HTREEITEM hItem = (HTREEITEM)wParam;
			HTREEITEM hParentItem=m_wndListPriority.GetParentItem(hItem);
			CTakeOffPriorityRunwayItem *runwayItem=(CTakeOffPriorityRunwayItem *)m_wndListPriority.GetItemData(hParentItem);
			if (runwayItem!=NULL)
			{
				TY_PRIORITYTREE_NODE NodeType = (TY_PRIORITYTREE_NODE)m_wndListPriority.GetItemNodeInfo(hItem)->nMaxCharNum ;
				switch (NodeType)
				{
				case TY_DECISIONPOINT_DEPSTAND:
				case TY_DECISIONPOINT_INTERSECTION:
					{
						TakeOffDecisionPointType decisionType=(TakeOffDecisionPointType)m_wndListPriority.GetItemData(hItem);
						runwayItem->SetDecisionPointType(decisionType);
						break;
					}
				case TY_TAKEOFF:
					{
						runwayItem->SetCanTakeOffInSlotTime(m_wndListPriority.IsCheckItem(hItem)==1);
						break;
					}
				case TY_TRAIL:
					{
						runwayItem->SetNoFactorForTrailingACSep(m_wndListPriority.IsCheckItem(hItem)==1);
						break;
					}
				}
				
			}
		}
		break;
	case UM_CEW_COMBOBOX_BEGIN:
		{
			HTREEITEM hItem=(HTREEITEM)wParam;
			CWnd* pWnd=m_wndListPriority.GetEditWnd(hItem);
			CRect rectWnd;
			m_wndListPriority.GetItemRect(hItem,rectWnd,TRUE);
			pWnd->SetWindowPos(NULL,rectWnd.right,rectWnd.top,0,0,SWP_NOSIZE);
			CComboBox* pCB=(CComboBox*)pWnd;
			pCB->ResetContent();
			pCB->SetDroppedWidth(100);
			
			pCB->AddString("No Backtrack");
			pCB->AddString("Min Backtrack");
			pCB->AddString("Max Backtrack");

			break;
		}
	case UM_CEW_COMBOBOX_SELCHANGE:
		{
			CWnd* pWnd = m_wndListPriority.GetEditWnd((HTREEITEM)wParam);
			CComboBox* pCB = (CComboBox*)pWnd;
			HTREEITEM hBacktrackItem=(HTREEITEM)wParam;
			HTREEITEM hParentItem=m_wndListPriority.GetParentItem(hBacktrackItem);
			//CString strText = *(CString*)lParam;
			int nSel = pCB->GetCurSel();
			CManageAssignPriorityData *priorityData=(CManageAssignPriorityData *)m_wndListPriority.GetItemData(hParentItem);
			if (priorityData!=NULL&&nSel>=0)
			{
				priorityData->GetRunwayItem()->SetTakeOffBackTrack((TakeOffBackTrack)nSel);
				
				CString strBacktrack=_T("Backtrack Option:");
				switch((TakeOffBackTrack)nSel)
				{
				case BT_NoBacktrack:
					{
						strBacktrack+=_T("No Backtrack");
						break;
					}
				case BT_MinBacktrack:
					{
						strBacktrack+=_T("Min Backtrack");
						break;
					}
				case BT_MaxBacktrack:
					{
						strBacktrack+=_T("Max Backtrack");
						break;
					}
				}
				m_wndListPriority.SetItemText(hBacktrackItem,strBacktrack);

			}


			break;
		}

	}


	return CDialog::DefWindowProc(message,wParam,lParam) ;
}

void CDlgTakeoffRunwayAssign::OnSize( UINT nType, int cx, int cy )
{
	CDialog::OnSize(nType,cx,cy) ;


	if (m_wndTreeFltTime.GetSafeHwnd())
	{
		CRect rectWnd;
		int nSplitterWidth=9;
		int nLeft=nSplitterWidth;

		//left
		CRect leftRect;
		m_staticFlightType.SetWindowPos(NULL,nLeft,nSplitterWidth,(cx -18)/3,cy - 45,SWP_NOACTIVATE|SWP_NOZORDER);
		m_staticFlightType.GetWindowRect(leftRect);
		ScreenToClient(leftRect);

		int nTop=leftRect.top+40;
		m_wndTreeFltTime.SetWindowPos(NULL,nLeft,nTop,(cx -18)/3, leftRect.Height() + 8 - nTop,SWP_NOACTIVATE|SWP_NOZORDER);
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


		//right
		m_staticRwyAssign.SetWindowPos(NULL,leftRect.right + 4,leftRect.top,cx - leftRect.Width() -25,leftRect.Height(),SWP_NOACTIVATE|SWP_NOZORDER);
		m_staticRwyAssign.GetWindowRect(rectWnd);
		ScreenToClient(rectWnd);

		CRect cmbRect;
		m_wndCmbAssignStrategy.SetWindowPos(NULL,rectWnd.left + 2,rectWnd.top + 20 , rectWnd.Width() -4,12,SWP_NOACTIVATE | SWP_NOZORDER);
		m_staticRwyAssign.GetWindowRect(cmbRect);
		ScreenToClient(cmbRect);	

		m_wndListPriority.SetWindowPos(NULL,rectWnd.left +2,rectWnd.top + 80, rectWnd.Width(),rectWnd.Height() - 80,SWP_NOACTIVATE|SWP_NOZORDER);
		m_wndListPriority.GetWindowRect(rectWnd);
		ScreenToClient(rectWnd);

		m_wndRunwayToolbar.SetWindowPos(NULL,rectWnd.left +2,rectWnd.top -20,rectWnd.Width(), 20,SWP_NOACTIVATE|SWP_NOZORDER);

		CRect rcBtn;
		GetDlgItem(IDCANCEL)->GetWindowRect(&rcBtn);
		GetDlgItem(IDCANCEL)->SetWindowPos(NULL,rectWnd.right -rcBtn.Width(),rectWnd.bottom + 8, rcBtn.Width(),rcBtn.Height(),NULL);
		GetDlgItem(IDCANCEL)->GetWindowRect(&rcBtn);
		ScreenToClient(rcBtn);

		GetDlgItem(IDOK)->SetWindowPos(NULL,rcBtn.left - rcBtn.Width() -5,rcBtn.top,rcBtn.Width(),rcBtn.Height(), NULL);
		GetDlgItem(IDOK)->GetWindowRect(&rcBtn);
		ScreenToClient(rcBtn);

		GetDlgItem(IDC_BUTTON_SAVE)->SetWindowPos(NULL,rcBtn.left - rcBtn.Width() -5,rcBtn.top,rcBtn.Width(),rcBtn.Height(),NULL);
	}
	Invalidate(FALSE);
}

void CDlgTakeoffRunwayAssign::OnPaint()
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


void CDlgTakeoffRunwayAssign::DoResize( int delta,UINT nIDSplitter )
{
	CRect rectWnd;
	if(nIDSplitter == ID_WND_SPLITTER)
	{
		//left;
		CSplitterControl::ChangeWidth(GetDlgItem(IDC_STATIC_FLIGHTTYPE),delta,CW_LEFTALIGN);
		CSplitterControl::ChangeWidth(&m_wndTreeFltTime, delta,CW_LEFTALIGN);
		
		//right
		CSplitterControl::ChangeWidth(GetDlgItem(IDC_STATIC_STRATEGY_TITLE),-delta,CW_RIGHTALIGN);
		CSplitterControl::ChangeWidth(&m_wndCmbAssignStrategy,-delta,CW_RIGHTALIGN);
		CSplitterControl::ChangeWidth(&m_wndRunwayToolbar,-delta,CW_RIGHTALIGN);
		CSplitterControl::ChangeWidth(&m_wndListPriority,-delta,CW_RIGHTALIGN);

	}
}

void CDlgTakeoffRunwayAssign::OnContextMenu( CWnd* pWnd, CPoint point )
{
	CMenu* pMenu=NULL;
	CMenu menuPopup;

	CRect  rtFltTypeTree;
	m_wndTreeFltTime.GetWindowRect(&rtFltTypeTree);
	if( rtFltTypeTree.PtInRect(point))
	{
		m_wndTreeFltTime.SetFocus();
		CPoint pt = point;
		m_wndTreeFltTime.ScreenToClient( &pt );
		UINT iRet;
		HTREEITEM hRClickItem = m_wndTreeFltTime.HitTest( pt, &iRet );
		if (iRet != TVHT_ONITEMLABEL)
		{
			hRClickItem = NULL;
			return;
		}
		m_wndTreeFltTime.SelectItem(hRClickItem);

		TY_NODE nodeType = GetTreeItemNodeType(hRClickItem);


		switch(nodeType)
		{
		case TY_TREENODE_STAND:
			{
				menuPopup.LoadMenu( IDR_MENU_STANDPOPMENU );
				pMenu = menuPopup.GetSubMenu(0);
			}
			break;
		case TY_TREENODE_FLIGHT:
			{
				menuPopup.LoadMenu( IDR_MENU_FLTTYPEPOPMENU );
				pMenu = menuPopup.GetSubMenu(0);
			}
			break;
		case TY_TREENODE_TIMERANGE:
			{
				menuPopup.LoadMenu( IDR_MENU_TIMEWINPOPMENU );
				pMenu = menuPopup.GetSubMenu(0);
			}
		    break;
		default:
		    break;
		}
	}
	else
	{
		CRect  rtRwyTree;
		m_wndListPriority.GetWindowRect(&rtRwyTree);
		if( !rtRwyTree.PtInRect(point))
			return;

		m_wndListPriority.SetFocus();
		CPoint pt = point;
		m_wndListPriority.ScreenToClient( &pt );
		UINT iRet;
		HTREEITEM hRClickItem = m_wndListPriority.HitTest( pt, &iRet );
		if (iRet != TVHT_ONITEMLABEL)
		{
			hRClickItem = NULL;
			return;
		}

		m_wndListPriority.SelectItem(hRClickItem);
		TY_PRIORITYTREE_NODE NodeType =(TY_PRIORITYTREE_NODE) m_wndListPriority.GetItemNodeInfo(hRClickItem)->nMaxCharNum ;
		
		if(NodeType == TY_TAKEOFF_POSITION)
		{
			menuPopup.LoadMenu( IDR_MENU_RUNWAYPOPMENU );
			pMenu = menuPopup.GetSubMenu(0);
		}

		if (NodeType == TY_TAKEOFF_POSITION_ITEM)
		{
			menuPopup.LoadMenu( IDR_MENU_TAKEOFFPOSPOPMENU );
			pMenu = menuPopup.GetSubMenu(0);
		}
		if (NodeType == TY_DECISIONPOINT_INTERSECTION)
		{
			if (m_wndListPriority.IsCheckItem(hRClickItem))
			{
				menuPopup.LoadMenu(IDR_POPUPMENU_INTERSECTION);
				pMenu = menuPopup.GetSubMenu(0);
			}
		}
		if (NodeType == TY_DECISIONPOINT_INTERSECTION_ITEM)
		{
			if (m_wndListPriority.IsCheckItem(hRClickItem))
			{
				menuPopup.LoadMenu(IDR_POPUPMENU_DEL_INTERSECTION);
				pMenu = menuPopup.GetSubMenu(0);
			}
		}
	}

	if (pMenu)
		pMenu->TrackPopupMenu(TPM_LEFTALIGN, point.x, point.y, this);	
}

void CDlgTakeoffRunwayAssign::OnDeleteRunway()
{
	if(m_hTimeRangeTreeItem == NULL)
		return;

	HTREEITEM hSelItem = m_wndListPriority.GetSelectedItem();
	if (hSelItem == NULL)
		return;

	HTREEITEM hParItem = m_wndListPriority.GetParentItem(hSelItem);
	CManageAssignPriorityData* pPriData = (CManageAssignPriorityData*)m_wndListPriority.GetItemData(hParItem);
	m_wndListPriority.SelectItem(hParItem);

	CTakeOffTimeRangeRunwayAssignItem *pTimeRangeItem = (CTakeOffTimeRangeRunwayAssignItem*)m_wndTreeFltTime.GetItemData(m_hTimeRangeTreeItem);

	pTimeRangeItem->RemovePriorityRwyData(pPriData) ;
	m_wndListPriority.DeleteItem(hParItem);

	InitPriorityTree(pTimeRangeItem);
}

void CDlgTakeoffRunwayAssign::OnEditTakeoffPos()
{
	HTREEITEM hSelItem = m_wndListPriority.GetSelectedItem();
	if (hSelItem == NULL)
		return;

	CTakeOffPosition* pTakeoffPos = (CTakeOffPosition*)m_wndListPriority.GetItemData(hSelItem);

	HTREEITEM hParItem = m_wndListPriority.GetParentItem(hSelItem);
	CTakeOffPriorityRunwayItem* pRwyData = (CTakeOffPriorityRunwayItem*)m_wndListPriority.GetItemData(hParItem);


	CDlgSelectTakeoffPosition *m_dlgTakeoffPos=new CDlgSelectTakeoffPosition(m_ProID,-1,-1, this,true) ;

	if(m_dlgTakeoffPos->DoModal() != IDOK)
		return;

	std::vector<RunwayExit> vExitList = m_dlgTakeoffPos->m_vRunwayExitList;

	size_t nSize = vExitList.size();
	if (nSize >1)
	{
		CString ErrorMsg = "Invalid edit, the number of edit takeoff position should be only one!";
		MessageBox(ErrorMsg,_T("Warning"),MB_OK) ;
		return;
	}


	RunwayExit runwayExit = vExitList[0];

	if (runwayExit.EqualToRunwayExitDescription(pTakeoffPos->m_runwayExitDescription))
		return;

	if(pRwyData->FindTakeoffPosition(runwayExit))
	{
		CString ErrorMsg ;
		ErrorMsg.Format(_T("%s has selected!"),m_dlgTakeoffPos->m_arSelName.GetAt(0)) ;
		MessageBox(ErrorMsg,_T("Warning"),MB_OK) ;
		return;
	}

	pTakeoffPos->m_TakeoffID = -1;
	pTakeoffPos->m_strName = m_dlgTakeoffPos->m_arSelName.GetAt(0);
	pTakeoffPos->m_runwayExitDescription.InitData(runwayExit);

	CString Str ;
	Str = FormatTakeOffItemText(pTakeoffPos)  ;
	m_wndListPriority.SetItemText(hSelItem, Str);



}


void checksize()
{

}