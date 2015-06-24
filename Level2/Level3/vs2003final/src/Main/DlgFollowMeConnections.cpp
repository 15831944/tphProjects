// DlgFollowMeConnections.cpp : implementation file
//

#include "stdafx.h"
#include "TermPlan.h"
#include "DlgFollowMeConnections.h"
#include ".\dlgfollowmeconnections.h"
#include "FlightDialog.h"
#include "DlgSelectALTObject.h"
#include "DlgSelectAirsideNode.h"
#include "DlgTimeRange.h"
#include "../Common/elaptime.h"
#include "..\Common\AirportDatabase.h"
#include "..\AirsideGUI\DlgSelectRunway.h"
#include "..\InputAirside\Runway.h"
#include "DlgSelectIntersectionNode.h"
// CDlgFollowMeConnections dialog

IMPLEMENT_DYNAMIC(CDlgFollowMeConnections, CDialog)
CDlgFollowMeConnections::CDlgFollowMeConnections(int _proID ,CAirportDatabase* _AirportDB ,CWnd* pParent /*=NULL*/)
	: CDialog(CDlgFollowMeConnections::IDD, pParent),m_ProID(_proID),m_Data(_AirportDB)
{
	m_Data.ReadData() ;
}

CDlgFollowMeConnections::~CDlgFollowMeConnections()
{
}

void CDlgFollowMeConnections::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE_DATA, m_TreeCtrl);
	DDX_Control(pDX, IDC_STATIC_TOOL, m_StaticTool);
	DDX_Control(pDX, IDC_STATIC_TOOL2, m_StaticMeeting);
	DDX_Control(pDX, IDC_LIST_MEETING, m_MeetingListBox);
	DDX_Control(pDX, IDC_COMBO_TYPE, m_ComboType);
	DDX_Control(pDX, IDC_COMBO_BEFORE, m_SettingCombo);
	DDX_Control(pDX, IDC_EDIT_VAL, m_EditDIS);
}


BEGIN_MESSAGE_MAP(CDlgFollowMeConnections, CDialog)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_DATA, OnTvnSelchangedTreeData)
	ON_CBN_SELCHANGE(IDC_COMBO_TYPE, OnCbnSelchangeComboType)
	ON_CBN_SELCHANGE(IDC_COMBO_BEFORE, OnCbnSelchangeComboBefore)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN1, OnDeltaposSpin1)


	ON_COMMAND(ID_FOLLOW_FLIGHT_ADD,OnFlightToolBarAdd)
	ON_COMMAND(ID_FOLLOW_FLIGHT_DEL,OnFlightToolBarDel)
	ON_COMMAND(ID_FOLLOW_FLIGHT_EDIT,OnFlightToolBarEdit) 

	ON_COMMAND(ID_FOLLOW_MEETPOINT_ADD,OnAddMeetingPoint) 
	ON_COMMAND(ID_FOLLOW_MEETPOINT_DEL,OnDeleteMeetingPoint)

	ON_WM_CREATE()
	ON_LBN_SELCHANGE(IDC_LIST_MEETING, OnLbnSelchangeListMeeting)
	ON_EN_UPDATE(IDC_EDIT_VAL, OnEnUpdateEditVal)
END_MESSAGE_MAP()

CDlgFollowMeConnections::TY_NODE CDlgFollowMeConnections::GetNodeType(HTREEITEM _item)
{
	HTREEITEM HParentItem = m_TreeCtrl.GetParentItem(_item) ;
	if(HParentItem == NULL)
		return TY_FLIGHT_ROOT ;
	HParentItem = m_TreeCtrl.GetParentItem(HParentItem) ;
	if(HParentItem == NULL)
		return TY_FLIGHT ;
	HParentItem = m_TreeCtrl.GetParentItem(HParentItem) ;
	if(HParentItem == NULL)
		return TY_RANGETIME ;
	HParentItem = m_TreeCtrl.GetParentItem(HParentItem) ;
	if(HParentItem == NULL)
		return TY_RUNWAY ;
	return TY_STAND ;
}
void CDlgFollowMeConnections::OnFlightToolBarAdd()
{
	HTREEITEM HSelItem = m_TreeCtrl.GetSelectedItem() ;
	if(HSelItem == NULL)
		return ;
	switch(GetNodeType(HSelItem))
	{
	case TY_FLIGHT_ROOT:
		OnAddFlightType() ;
		break ;
	case TY_FLIGHT:
		OnAddFlightRangeItem() ;
		break ;
	case TY_RANGETIME:
		OnAddRunwayItem() ;
		break ;
	case TY_RUNWAY:
		OnAddStandItem() ;
		break ;
	}
}
void CDlgFollowMeConnections::OnFlightToolBarEdit()
{
	HTREEITEM HSelItem = m_TreeCtrl.GetSelectedItem() ;
	switch(GetNodeType(HSelItem))
	{
	case TY_FLIGHT:
		OnEditFlightType();
		break;
	case TY_RANGETIME:
		OnEditFlightRangeItem() ;
		break ;
	case TY_RUNWAY:
		OnEditRunwayItem() ;
		break ;
	case TY_STAND:
		OnEditStandItem() ;
		break ;
	}
}
void CDlgFollowMeConnections::OnFlightToolBarDel()
{

	HTREEITEM HSelItem = m_TreeCtrl.GetSelectedItem() ;
	switch(GetNodeType(HSelItem))
	{
	case TY_FLIGHT:
		OnDeleteFlightType();
		break;
	case TY_RANGETIME:
		OnDeleteRangeItem() ;
		break ;
	case TY_RUNWAY:
		OnDeleteRunwayItem() ;
		break ;
	case TY_STAND:
		OnDeleteStandItem() ;
		break ;
	}
}

void CDlgFollowMeConnections::OnAddMeetingPoint()
{
	HTREEITEM HSelItem = m_TreeCtrl.GetSelectedItem() ;
	if(HSelItem == NULL)
		return ;

	CFollowMeConnectionStand* PStand = (CFollowMeConnectionStand*)m_TreeCtrl.GetItemData(HSelItem) ;
	CDlgSelectALTObject dlgSelect(m_ProID,ALT_MEETINGPOINT,this) ;

	if(dlgSelect.DoModal() == IDOK)
	{
		m_MeetingListBox.AddString(dlgSelect.GetALTObject().GetIDString()) ;
		PStand->AddMeetingPoint(dlgSelect.GetALTObject().GetIDString()) ;
	}
}
void CDlgFollowMeConnections::OnDeleteMeetingPoint()
{
	HTREEITEM HSelItem = m_TreeCtrl.GetSelectedItem() ;
	if(HSelItem == NULL)
		return ;

	CFollowMeConnectionStand* PStand = (CFollowMeConnectionStand*)m_TreeCtrl.GetItemData(HSelItem) ;
	
	int ndx = m_MeetingListBox.GetCurSel() ;

	CString Val ;
	m_MeetingListBox.GetText(ndx ,Val) ;

	m_MeetingListBox.DeleteString(ndx) ;

	PStand->RemoveMeetingPoint(Val) ;
}
//////////////////////////////////////////////////////////////////////////


BOOL CDlgFollowMeConnections::OnInitDialog()
{
	CDialog::OnInitDialog() ;
	InitTreeCtrl() ;
	InitFlightToolBar() ;
	InitMeetingToolBar() ;
	InitTypeComboBox() ;
	InitBeforeComboBox() ;
	EnableRightCtrl(FALSE) ;
	return TRUE;
}
int CDlgFollowMeConnections::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if(CDialog::OnCreate(lpCreateStruct) == -1 )
		return -1 ;
	if( !m_FlightTool.CreateEx(this,  TBSTYLE_FLAT,WS_CHILD|WS_VISIBLE|CBRS_ALIGN_TOP ) ||
		!m_FlightTool.LoadToolBar(IDR_FOLLOW_FLIGHT))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}
	if(!m_MeetingTool.CreateEx(this,TBSTYLE_FLAT,WS_CHILD|WS_VISIBLE|CBRS_ALIGN_TOP) || 
		!m_MeetingTool.LoadToolBar(IDR_FOLLOW_MEETINGPOINT))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}
	return 0 ;
}

void CDlgFollowMeConnections::InitFlightToolBar()
{
	CRect rect ;
	m_StaticTool.ShowWindow(SW_HIDE) ;
	m_StaticTool.GetWindowRect(&rect) ;
	ScreenToClient(&rect) ;

	m_FlightTool.MoveWindow(&rect) ;

	m_FlightTool.GetToolBarCtrl().EnableButton(ID_FOLLOW_FLIGHT_ADD,TRUE) ;
	m_FlightTool.GetToolBarCtrl().EnableButton(ID_FOLLOW_FLIGHT_DEL,FALSE) ;
	m_FlightTool.GetToolBarCtrl().EnableButton(ID_FOLLOW_FLIGHT_EDIT,FALSE) ;

	m_FlightTool.ShowWindow(SW_SHOW) ;
}

void CDlgFollowMeConnections::InitMeetingToolBar()
{
	CRect rect ;
	m_StaticMeeting.ShowWindow(SW_HIDE) ;
	m_StaticMeeting.GetWindowRect(&rect) ;
	ScreenToClient(&rect) ;

	m_MeetingTool.MoveWindow(&rect) ;
	m_StaticMeeting.ShowWindow(SW_HIDE) ;
	m_MeetingTool.GetToolBarCtrl().EnableButton(ID_FOLLOW_MEETPOINT_ADD,TRUE) ;
	m_MeetingTool.GetToolBarCtrl().EnableButton(ID_FOLLOW_MEETPOINT_DEL,FALSE) ;

	m_MeetingTool.ShowWindow(SW_SHOW) ;
}

void CDlgFollowMeConnections::InitTreeCtrl()
{
	std::vector<CFollowMeConnectionItem*> _pData = m_Data.GetData() ;
	HTREEITEM RootItem = m_TreeCtrl.InsertItem(_T("Flight Type")) ;
	m_TreeCtrl.SetItemData(RootItem,NULL) ;
	for (int i = 0 ; i < (int)_pData.size() ;i++)
	{
		AddFlightItem(_pData.at(i),RootItem) ;
	}
	m_TreeCtrl.Expand(RootItem,TVE_EXPAND) ;
}
void CDlgFollowMeConnections::AddFlightItem(CFollowMeConnectionItem* _item,HTREEITEM _rootItem )
{
	if(_item == NULL)
		return ;
	CString flightType ;
	_item->GetFlightConstraint().screenPrint(flightType) ;
	HTREEITEM FlightItem = m_TreeCtrl.InsertItem(flightType,_rootItem) ;
	m_TreeCtrl.SetItemData(FlightItem,(DWORD_PTR)_item) ;

	CFollowMeConnectionTimeRangeItem* PRangeItem = NULL ;
	for (int i = 0 ; i < (int)_item->GetData()->size() ;i++)
	{
			PRangeItem = _item->GetData()->at(i) ;
			AddFlightRangeItem(PRangeItem,FlightItem) ;
	}
	m_TreeCtrl.Expand(_rootItem,TVE_EXPAND) ;
	m_TreeCtrl.Expand(FlightItem,TVE_EXPAND) ;
}
void CDlgFollowMeConnections::EditFlightItem(CFollowMeConnectionItem* _item,HTREEITEM _fltItem)
{
	CString flightType ;
	_item->GetFlightConstraint().screenPrint(flightType) ;

	m_TreeCtrl.SetItemText(_fltItem,flightType) ;
}
void CDlgFollowMeConnections::AddFlightRangeItem(CFollowMeConnectionTimeRangeItem* _rangerItem,HTREEITEM _fltItem)
{
	if(_rangerItem == NULL || _fltItem == NULL)
		return ;
	CString StrVal ;
	StrVal = FormatTimeRanger(_rangerItem) ;

	HTREEITEM rangeItem = m_TreeCtrl.InsertItem(StrVal,_fltItem) ;
	m_TreeCtrl.SetItemData(rangeItem,(DWORD_PTR)_rangerItem) ;

	CFollowMeConnectionRunway* PRunway = NULL ;

	for (int i = 0 ; i < (int)_rangerItem->m_Data.size() ;i++)
	{
		PRunway = _rangerItem->m_Data.at(i) ;
		AddRunwayItem(PRunway,rangeItem) ;
	}
	m_TreeCtrl.Expand(_fltItem,TVE_EXPAND) ;
	m_TreeCtrl.Expand(rangeItem,TVE_EXPAND) ;
}
void CDlgFollowMeConnections::EditRangeItem(CFollowMeConnectionTimeRangeItem* _rangerItem,HTREEITEM _RangeItem)
{
	if(_RangeItem == NULL || _RangeItem == NULL)
		return ;
	CString strval ;

	strval = FormatTimeRanger(_rangerItem) ;

	m_TreeCtrl.SetItemText(_RangeItem,strval) ;
}

CString CDlgFollowMeConnections::FormatTimeRanger(CFollowMeConnectionTimeRangeItem* _item)
{
	CString SQL ;

	SQL.Format(_T("%s - %s"),_item->GetStartTime().PrintDateTime(),_item->GetEndTime().PrintDateTime()) ;
	return SQL ;
}
void CDlgFollowMeConnections::AddRunwayItem(CFollowMeConnectionRunway* _runway ,HTREEITEM _RangeItem)
{
	if(_runway == NULL || _RangeItem == NULL)
		return ;
	CString strVal ;

	strVal = _runway->GetRunwayName() ;
	HTREEITEM item = m_TreeCtrl.InsertItem(strVal,_RangeItem) ;
	m_TreeCtrl.SetItemData(item,(DWORD_PTR)_runway) ;

	CFollowMeConnectionStand* PStand = NULL ;

	for (int i = 0 ; i < (int)_runway->m_Data.size() ;i++)
	{
		PStand = _runway->m_Data.at(i) ;
		AddStandItem(PStand,item) ;
	}
	m_TreeCtrl.Expand(_RangeItem,TVE_EXPAND) ;
	m_TreeCtrl.Expand(item,TVE_EXPAND) ;
}

void CDlgFollowMeConnections::EditRunwayItem(CFollowMeConnectionRunway* _runway ,HTREEITEM _runwayItem)
{
	if(_runway == NULL || _runwayItem == NULL)
		return ;
	CString strVal ;

	strVal = _runway->GetRunwayName() ;

	m_TreeCtrl.SetItemText(_runwayItem,strVal) ;
}

void CDlgFollowMeConnections::AddStandItem(CFollowMeConnectionStand* _stand,HTREEITEM _RunwayItem)
{
	if(_stand == NULL || _RunwayItem == NULL)
		return ;
	CString StrVal ;
	StrVal = _stand->m_StandInSim ;
	HTREEITEM _StandItem = m_TreeCtrl.InsertItem(StrVal,_RunwayItem) ;
	m_TreeCtrl.SetItemData(_StandItem,(DWORD_PTR)_stand) ;

	m_TreeCtrl.Expand(_StandItem,TVE_EXPAND) ;
}

void CDlgFollowMeConnections::EditStandItem(CFollowMeConnectionStand* _stand,HTREEITEM _standItem)
{
	if(_stand == NULL || _standItem == NULL)
		return ;
	CString strVal ;
	strVal = _stand->m_StandInSim ;

	m_TreeCtrl.SetItemText(_standItem,strVal) ;
}

// toolbar function for flight type


// begin  -----------------  flight type -------------------------------
void CDlgFollowMeConnections::OnAddFlightType()
{
	CFlightDialog  fltDlg(m_pParentWnd) ;
	if(fltDlg.DoModal() == IDOK)
	{
		CFollowMeConnectionItem* PFollowMeConnection = m_Data.AddNewItem(fltDlg.GetFlightSelection()) ;
		AddFlightItem(PFollowMeConnection,m_TreeCtrl.GetSelectedItem()) ;
	}
}
void CDlgFollowMeConnections::OnEditFlightType()
{
	HTREEITEM hPItem = m_TreeCtrl.GetSelectedItem() ;
	if(hPItem == NULL)
		return ;
	CFollowMeConnectionItem* PFollowMeConnection = NULL ;
	PFollowMeConnection = (CFollowMeConnectionItem*)m_TreeCtrl.GetItemData(hPItem);
	
	ASSERT(PFollowMeConnection) ;

	if(PFollowMeConnection == NULL)
		return ;

	CFlightDialog fltDlg(m_pParentWnd) ;
	if (fltDlg.DoModal() == IDOK)
	{
		PFollowMeConnection->SetFlightConstraint(fltDlg.GetFlightSelection()) ;
		EditFlightItem(PFollowMeConnection,hPItem) ;
	}
}
void CDlgFollowMeConnections::OnDeleteFlightType()
{
	HTREEITEM hSelectItem = m_TreeCtrl.GetSelectedItem() ;
	CFollowMeConnectionItem* hPItemData = NULL ;
	hPItemData = (CFollowMeConnectionItem*)m_TreeCtrl.GetItemData(hSelectItem) ;

	m_Data.DeleteItem(hPItemData) ;

	m_TreeCtrl.DeleteItem(hSelectItem) ;
}
//end --------------------------------------------------------------------------------------

//begin ------------------ range time item -------------------------------
void CDlgFollowMeConnections::OnAddFlightRangeItem()
{
	HTREEITEM pSelectItem = m_TreeCtrl.GetSelectedItem() ;

	CFollowMeConnectionItem* PFollowMeConnection = NULL ;
	PFollowMeConnection = (CFollowMeConnectionItem*)m_TreeCtrl.GetItemData(pSelectItem);

	ElapsedTime startTime ;
	ElapsedTime endtime ;
	CDlgTimeRange DlgTimeRanger(startTime,endtime) ;
	if(DlgTimeRanger.DoModal() == IDOK)
	{
		CFollowMeConnectionTimeRangeItem* PNewItem = PFollowMeConnection->AddRangeItem(DlgTimeRanger.GetStartTime() ,DlgTimeRanger.GetEndTime()) ;
		if (PNewItem == NULL)
			return ;
		AddFlightRangeItem(PNewItem,pSelectItem) ;
	}
	m_TreeCtrl.Expand(pSelectItem,TVE_EXPAND) ;
}
void CDlgFollowMeConnections::OnEditFlightRangeItem()
{
	HTREEITEM hSelectItem = m_TreeCtrl.GetSelectedItem() ;
	
	CFollowMeConnectionTimeRangeItem* pRangerItem = (CFollowMeConnectionTimeRangeItem*)m_TreeCtrl.GetItemData(hSelectItem) ;

	CDlgTimeRange DlgTimeRanger(pRangerItem->GetStartTime(),pRangerItem->GetEndTime()) ;
	if(DlgTimeRanger.DoModal() == IDOK)
	{
		pRangerItem->SetStartTime(DlgTimeRanger.GetStartTime()) ;
		pRangerItem->SetEndTime(DlgTimeRanger.GetEndTime()) ;
		EditRangeItem(pRangerItem,hSelectItem) ;
	}
	m_TreeCtrl.Expand(hSelectItem,TVE_EXPAND) ;
}
void CDlgFollowMeConnections::OnDeleteRangeItem()
{
	HTREEITEM hSelItem = m_TreeCtrl.GetSelectedItem() ;

	HTREEITEM hParentItem = m_TreeCtrl.GetParentItem(hSelItem) ;

	CFollowMeConnectionItem* PFollowMeConnection = NULL ;
	PFollowMeConnection = (CFollowMeConnectionItem*)m_TreeCtrl.GetItemData(hParentItem);

	CFollowMeConnectionTimeRangeItem* pRangerItem = (CFollowMeConnectionTimeRangeItem*)m_TreeCtrl.GetItemData(hSelItem) ;

	PFollowMeConnection->RemoveRangeItem(pRangerItem) ;

	m_TreeCtrl.DeleteItem(hSelItem) ;
}
//end ----------------------------------------------------------------------------

//begin ---------------------runway item ------------------------------------------
void CDlgFollowMeConnections::OnAddRunwayItem()
{
	HTREEITEM pSelItem = m_TreeCtrl.GetSelectedItem() ;

	CFollowMeConnectionTimeRangeItem* pRangerItem = (CFollowMeConnectionTimeRangeItem*)m_TreeCtrl.GetItemData(pSelItem) ;
	
	CDlgSelectRunway dlg(m_ProID);
	if(IDOK != dlg.DoModal())
		return;
	int nAirportID = dlg.GetSelectedAirport();
	int nRunwayID = dlg.GetSelectedRunway();

	ALTAirport altAirport;
	altAirport.ReadAirport(nAirportID);
	Runway altRunway;
	altRunway.ReadObject(nRunwayID);	

	CString strMarking = (RUNWAYMARK_FIRST == dlg.GetSelectedRunwayMarkingIndex() ? altRunway.GetMarking1().c_str() : altRunway.GetMarking2().c_str());
	CString strRunwayInfo;
	strRunwayInfo.Format("%s:Runway:%s", altAirport.getName(), strMarking);
	

	CFollowMeConnectionRunway* RunwayItem = pRangerItem->AddRunwayItem(strRunwayInfo,nRunwayID,dlg.GetSelectedRunwayMarkingIndex()) ;
	if (RunwayItem)
	{
			AddRunwayItem(RunwayItem,pSelItem) ;
	}
	m_TreeCtrl.Expand(pSelItem,TVE_EXPAND) ;
}
void CDlgFollowMeConnections::OnEditRunwayItem()
{
	HTREEITEM pSelItem = m_TreeCtrl.GetSelectedItem() ;
	CFollowMeConnectionRunway* pRunwayItem = (CFollowMeConnectionRunway*)m_TreeCtrl.GetItemData(pSelItem) ;

	CDlgSelectRunway dlg(m_ProID);
	if(IDOK != dlg.DoModal())
		return;
	int nAirportID = dlg.GetSelectedAirport();
	int nRunwayID = dlg.GetSelectedRunway();

	ALTAirport altAirport;
	altAirport.ReadAirport(nAirportID);
	Runway altRunway;
	altRunway.ReadObject(nRunwayID);	

	CString strMarking = (RUNWAYMARK_FIRST == dlg.GetSelectedRunwayMarkingIndex() ? altRunway.GetMarking1().c_str() : altRunway.GetMarking2().c_str());
	CString strRunwayInfo;
	strRunwayInfo.Format("%s:Runway:%s", altAirport.getName(), strMarking);
	
	pRunwayItem->SetRunwayID(nRunwayID) ;
	pRunwayItem->SetRunwayMarketID(dlg.GetSelectedRunwayMarkingIndex()) ;
	pRunwayItem->SetRunwayName(strRunwayInfo) ;
	EditRunwayItem(pRunwayItem,pSelItem) ;
}
void CDlgFollowMeConnections::OnDeleteRunwayItem()
{
	HTREEITEM pSelItem = m_TreeCtrl.GetSelectedItem() ;
	CFollowMeConnectionRunway* pRunwayItem = (CFollowMeConnectionRunway*)m_TreeCtrl.GetItemData(pSelItem) ;
	HTREEITEM pParentItem = m_TreeCtrl.GetParentItem(pSelItem) ;
	CFollowMeConnectionTimeRangeItem* pRangerItem = (CFollowMeConnectionTimeRangeItem*)m_TreeCtrl.GetItemData(pParentItem) ;
	pRangerItem->RemoveRunwayItem(pRunwayItem) ;
	m_TreeCtrl.DeleteItem(pSelItem) ;
}
//end ------ runway item ------------------------

//begin ----------------stand item ------------------
void CDlgFollowMeConnections::OnAddStandItem()
{
	HTREEITEM hSelItem = m_TreeCtrl.GetSelectedItem() ;
	CFollowMeConnectionRunway* pRunwayItem = (CFollowMeConnectionRunway*)m_TreeCtrl.GetItemData(hSelItem) ;

	CDlgSelectAirsideNodeByType dlg(m_ProID,AIRSIDE_STAND,this) ;

	if(dlg.DoModal() == IDOK)
	{
		CString StandValue ;
		StandValue = dlg.GetSelectNode().GetNodeName() ;
		if(StandValue.IsEmpty())
		{
			MessageBox(_T("You should select one stand group"),MB_OK) ;
			return ;
		}
		CFollowMeConnectionStand* Pstand = pRunwayItem->AddStandItem(StandValue) ;
		if(Pstand)
			AddStandItem(Pstand,hSelItem) ;
		else
		{
			CString errorMsg ;
			errorMsg.Format(_T("\'%s\' has selected!"),StandValue) ;
			MessageBox(errorMsg,_T("Warning"),MB_OK) ;
		}
	}
	m_TreeCtrl.Expand(hSelItem,TVE_EXPAND) ;
}
void CDlgFollowMeConnections::OnEditStandItem()
{
	HTREEITEM hSelItem = m_TreeCtrl.GetSelectedItem() ;
	CFollowMeConnectionStand* Pstand  = (CFollowMeConnectionStand*)m_TreeCtrl.GetItemData(hSelItem) ;
	
	CDlgSelectAirsideNodeByType dlg(m_ProID,AIRSIDE_STAND,this) ;

	if(dlg.DoModal() == IDOK)
	{
		CString StandValue ;
		StandValue = dlg.GetSelectNode().GetNodeName() ;
		if(StandValue.IsEmpty())
		{
			MessageBox(_T("You should select one stand group"),MB_OK) ;
			return ;
		}
		Pstand->m_StandInSim = StandValue ;

		EditStandItem(Pstand,hSelItem) ;
	}
}
void CDlgFollowMeConnections::OnDeleteStandItem()
{
	HTREEITEM HSelItem = m_TreeCtrl.GetSelectedItem() ;
	CFollowMeConnectionStand* Pstand  = (CFollowMeConnectionStand*)m_TreeCtrl.GetItemData(HSelItem) ; 

	HTREEITEM hParentItem = m_TreeCtrl.GetParentItem(HSelItem) ;
	CFollowMeConnectionRunway* pRunwayItem = (CFollowMeConnectionRunway*)m_TreeCtrl.GetItemData(hParentItem) ;

	pRunwayItem->RemoveStandItem(Pstand) ;

	m_TreeCtrl.DeleteItem(HSelItem) ;
}
// CDlgFollowMeConnections message handlers

void CDlgFollowMeConnections::OnTvnSelchangedTreeData(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
	HTREEITEM hSelItem = m_TreeCtrl.GetSelectedItem() ;
	if (hSelItem == NULL)
		return ;
	if(GetNodeType(hSelItem) == TY_STAND)
	{
		m_FlightTool.GetToolBarCtrl().EnableButton(ID_FOLLOW_FLIGHT_ADD,FALSE) ;
		m_FlightTool.GetToolBarCtrl().EnableButton(ID_FOLLOW_FLIGHT_DEL,TRUE) ;
		m_FlightTool.GetToolBarCtrl().EnableButton(ID_FOLLOW_FLIGHT_EDIT,TRUE) ;

		EnableRightCtrl(TRUE) ;
		InitRightAllCtrlData((CFollowMeConnectionStand*)m_TreeCtrl.GetItemData(hSelItem)) ;
	}else
	{
		m_FlightTool.GetToolBarCtrl().EnableButton(ID_FOLLOW_FLIGHT_ADD,TRUE) ;
		m_FlightTool.GetToolBarCtrl().EnableButton(ID_FOLLOW_FLIGHT_DEL,TRUE) ;
		m_FlightTool.GetToolBarCtrl().EnableButton(ID_FOLLOW_FLIGHT_EDIT,TRUE) ;

		EnableRightCtrl(FALSE) ;
	}
}
void CDlgFollowMeConnections::EnableRightCtrl(BOOL _EnableOrDis)
{
	m_MeetingListBox.EnableWindow(_EnableOrDis) ;
	m_ComboType.EnableWindow(_EnableOrDis) ;
	m_SettingCombo.EnableWindow(_EnableOrDis) ;
	m_MeetingTool.GetToolBarCtrl().EnableButton(ID_FOLLOW_MEETPOINT_ADD,_EnableOrDis) ;
	m_EditDIS.EnableWindow(_EnableOrDis) ;
}
void CDlgFollowMeConnections::InitRightAllCtrlData(CFollowMeConnectionStand* _standItem)
{
	if(_standItem == NULL)
		return ;
	InitListBox(_standItem) ;
	
	int ndx = m_ComboType.FindString(0,_standItem->GetAbandonmentPoint()->m_Name) ;
	if(ndx == LB_ERR&& !_standItem->GetAbandonmentPoint()->m_Name.IsEmpty())
	{
		ndx = m_ComboType.InsertString(0,_standItem->GetAbandonmentPoint()->m_Name);
		m_ComboType.SetCurSel(ndx);
	}else
		m_ComboType.SetCurSel(ndx) ;
	if(_standItem->GetAbandonmentPoint()->m_PointType == CAbandonmentPoint::TY_Intersection)
	{
			m_EditDIS.EnableWindow(TRUE) ;
			m_SettingCombo.EnableWindow(TRUE) ;
			
			
			CString strVal ;
			strVal.Format(_T("%0.2f"),_standItem->GetAbandonmentPoint()->m_Dis) ;
			m_EditDIS.SetWindowText(strVal) ;

			m_SettingCombo.SetCurSel(_standItem->GetAbandonmentPoint()->m_SelType) ;
	}else
	{
		m_EditDIS.EnableWindow(FALSE) ;
		m_SettingCombo.EnableWindow(FALSE) ;
	}
}

void CDlgFollowMeConnections::InitTypeComboBox()
{
	int ndx = m_ComboType.AddString(_T("Select Stand...")) ;
	m_ComboType.SetItemData(ndx,CAbandonmentPoint::TY_Stand) ;
	ndx = m_ComboType.AddString(_T("Select Intersection...")) ;
	m_ComboType.SetItemData(ndx ,CAbandonmentPoint::TY_Intersection) ;
}
void CDlgFollowMeConnections::InitBeforeComboBox()
{
	int ndx = m_SettingCombo.AddString(_T("Before")) ;
	m_SettingCombo.SetItemData(ndx,0) ;

	ndx = m_SettingCombo.AddString(_T("After")) ;
	m_SettingCombo.SetItemData(ndx,1) ;
}

void CDlgFollowMeConnections::InitListBox( CFollowMeConnectionStand* _standItem )
{
	m_MeetingListBox.ResetContent() ;
	CString _MeetingPonit ;
	int ndx = 0 ;
	for (int i = 0 ; i < (int)_standItem->m_MeetPointData.size() ;i++)
	{
		_MeetingPonit = _standItem->m_MeetPointData.at(i) ;

		ndx = m_MeetingListBox.AddString(_MeetingPonit) ;
	}
}
void CDlgFollowMeConnections::OnCbnSelchangeComboType()
{
	// TODO: Add your control notification handler code here
	int ndx = m_ComboType.GetCurSel() ;
	CString str ;
	m_ComboType.GetLBText(ndx ,str) ;
	CAbandonmentPoint::Point_Type type = (CAbandonmentPoint::Point_Type) m_ComboType.GetItemData(ndx) ;
	HTREEITEM HselItem = m_TreeCtrl.GetSelectedItem() ;
	if(HselItem == NULL)
		return ;
	CFollowMeConnectionStand* PstandItem = (CFollowMeConnectionStand*)m_TreeCtrl.GetItemData(HselItem) ;
	if(type == CAbandonmentPoint::TY_Stand && str.CompareNoCase(_T("Select Stand...")) == 0)
	{
		CDlgSelectALTObject dlg(m_ProID,ALT_STAND,this) ;
		if(dlg.DoModal() == IDOK)
		{
			CString strVal ;
			strVal = dlg.GetALTObject().GetIDString() ;
			if(strVal.IsEmpty())
				strVal = "ALL" ;
			int ndx = m_ComboType.FindString(0,strVal) ;
			if(ndx == LB_ERR)
				ndx = m_ComboType.InsertString(0,strVal) ;
			m_ComboType.SetCurSel(ndx) ;
			m_ComboType.SetItemData(ndx,CAbandonmentPoint::TY_Stand) ;

			PstandItem->GetAbandonmentPoint()->m_PointType = CAbandonmentPoint::TY_Stand ;
			PstandItem->GetAbandonmentPoint()->m_Name = strVal ;
			m_EditDIS.EnableWindow(FALSE) ;
			m_SettingCombo.EnableWindow(FALSE) ;

		}
		return ;
	}
	if(str.CompareNoCase(_T("Select Intersection...")) == 0 && type == CAbandonmentPoint::TY_Intersection)
	{
		CDlgSelectIntersectionNode dlgSelAirside(m_ProID ,this) ;
		if(dlgSelAirside.DoModal() == IDOK)
		{

			PstandItem->GetAbandonmentPoint()->m_PointType  = CAbandonmentPoint::TY_Intersection ;
			PstandItem->GetAbandonmentPoint()->m_Name = dlgSelAirside.m_strSelNode ;

			int ndx = m_ComboType.FindString(0,dlgSelAirside.m_strSelNode) ;
			if(ndx == LB_ERR)
				ndx = m_ComboType.InsertString(0,dlgSelAirside.m_strSelNode) ;
			m_ComboType.SetCurSel(ndx) ;
			m_ComboType.SetItemData(ndx, CAbandonmentPoint::TY_Intersection);

			CString strVal ;
			strVal.Format(_T("%0.2f"),PstandItem->GetAbandonmentPoint()->m_Dis) ;
			m_EditDIS.SetWindowText(strVal) ;

			m_SettingCombo.SetCurSel(PstandItem->GetAbandonmentPoint()->m_SelType) ;


			m_EditDIS.EnableWindow(TRUE) ;
			m_SettingCombo.EnableWindow(TRUE) ;
		}
		return ;
	}
	PstandItem->GetAbandonmentPoint()->m_PointType = type ;
	PstandItem->GetAbandonmentPoint()->m_Name = str ;
}

void CDlgFollowMeConnections::OnCbnSelchangeComboBefore()
{
	// TODO: Add your control notification handler code here

	HTREEITEM HselItem = m_TreeCtrl.GetSelectedItem() ;
	if(HselItem == NULL)
		return ;
	CFollowMeConnectionStand* PstandItem = (CFollowMeConnectionStand*)m_TreeCtrl.GetItemData(HselItem) ;

	PstandItem->GetAbandonmentPoint()->m_SelType = m_SettingCombo.GetCurSel() ;
}

void CDlgFollowMeConnections::OnDeltaposSpin1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;

}

void CDlgFollowMeConnections::OnOK()
{
	m_Data.WriteData() ;
	CDialog::OnOK() ;
}

void CDlgFollowMeConnections::OnCancel()
{
	CDialog::OnCancel() ;
}
void CDlgFollowMeConnections::OnLbnSelchangeListMeeting()
{
	// TODO: Add your control notification handler code here
	int ndx = m_MeetingListBox.GetCurSel() ;
	if(ndx == LB_ERR )
		m_MeetingTool.GetToolBarCtrl().EnableButton(ID_FOLLOW_MEETPOINT_DEL,FALSE) ;
	else
		m_MeetingTool.GetToolBarCtrl().EnableButton(ID_FOLLOW_MEETPOINT_DEL,TRUE) ;
}

void CDlgFollowMeConnections::OnEnUpdateEditVal()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function to send the EM_SETEVENTMASK message to the control
	// with the ENM_UPDATE flag ORed into the lParam mask.

	// TODO:  Add your control notification handler code here
	HTREEITEM HselItem = m_TreeCtrl.GetSelectedItem() ;
	if(HselItem == NULL)
		return ;
	CFollowMeConnectionStand* PstandItem = (CFollowMeConnectionStand*)m_TreeCtrl.GetItemData(HselItem) ;

	CString strVal ;
	m_EditDIS.GetWindowText(strVal) ;

	PstandItem->GetAbandonmentPoint()->m_Dis = atof(strVal) ;
}
