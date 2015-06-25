//CDlgTrainFlightsManagement.cpp : implementation file
//

#include "stdafx.h"
#include "Resource.h"

#include "DlgTrainFlightsManagement.h"
#include "DlgTimeRange.h"
#include "InputAirside\TakeOffTimeRangeRunwayAssignItem.h"
#include "DlgTakeOffPositionSelection.h"
#include <limits>

// CDlgTrainFlightsManagement dialog

IMPLEMENT_DYNAMIC(CDlgTrainFlightsManagement, CXTResizeDialog)
CDlgTrainFlightsManagement::CDlgTrainFlightsManagement(CAirportDatabase* pAirportDB,int nProjID,CWnd* pParent /*=NULL*/)
	: CXTResizeDialog(CDlgTrainFlightsManagement::IDD, pParent)
	,m_nProjID(nProjID)
	,m_pAirportDB(pAirportDB)
{
	m_Data.ReadData();
}

CDlgTrainFlightsManagement::~CDlgTrainFlightsManagement()
{
}

void CDlgTrainFlightsManagement::DoDataExchange(CDataExchange* pDX)
{
	CXTResizeDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE_DATA, m_wndTreeCtrl);
}


BEGIN_MESSAGE_MAP(CDlgTrainFlightsManagement, CXTResizeDialog)

	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_FLIGHTSTYPEPOPUPMENU_NEWTIMEWINDOW, OnAddTimeRange)
	ON_COMMAND(ID_TIMERANGEPOPUPMENU_NEWTAKEOFFPOSITION, OnAddTakeoffPosition)
	
	ON_COMMAND(ID_TIMERANGEPOPUPMENU_EDIT, OnEditTimeRange)
	ON_COMMAND(ID_TAKEOFFPOSITION_EDIT, OnEditTakeoffPosition)

	ON_COMMAND(ID_TIMERANGEPOPUPMENU_DELET, OnDeleteTimeRange)
	ON_COMMAND(ID_TAKEOFFPOSITION_DELETE, OnDeleteTakeoffPosition)

	ON_BN_CLICKED(IDC_BUTTON_SAVE, OnSave)
	ON_BN_CLICKED(IDC_BUTTON_OK, OnOk)
	ON_BN_CLICKED(IDC_BUTTON_CANCEL, OnCancel)
END_MESSAGE_MAP()


// CDlgTrainFlightsManagement message handlers

BOOL CDlgTrainFlightsManagement::OnInitDialog()
{
	CXTResizeDialog::OnInitDialog();
	SetResize(IDC_TREE_DATA,SZ_TOP_LEFT,SZ_BOTTOM_RIGHT);
	SetResize(IDC_BUTTON_SAVE, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);
	SetResize(IDC_BUTTON_OK, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);
	SetResize(IDC_BUTTON_CANCEL, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);
	InitFlightTree();
	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(FALSE);
	return TRUE;
}

void CDlgTrainFlightsManagement::InitFlightTree()
{
	int Count = (int)m_Data.GetFlayTypeData()->size();
	for (int i = 0; i < Count; i++)
	{
		CTrainingFlightTypeManage* FlightType= m_Data.GetFlayTypeData()->at(i);
		FlightType->GetID();
		CString str;
		str.Format(_T("%s"),FlightType->GetFlightType());
		AddFlightTreeItem(m_Data.GetFlayTypeData()->at(i),str);
	}
}

void CDlgTrainFlightsManagement::AddFlightTreeItem(CTrainingFlightTypeManage* _flightItem,CString str)
{
	if(!str)
		return;
	COOLTREE_NODE_INFO info;
	m_wndTreeCtrl.InitNodeInfo(this ,info);
	info.net = NET_NORMAL;
	info.nt = NT_NORMAL;
	info.bAutoSetItemText = FALSE;
	HTREEITEM HFlightItem = m_wndTreeCtrl.InsertItem(str,info,FALSE,FALSE);
	m_wndTreeCtrl.SetItemData(HFlightItem,(DWORD_PTR)_flightItem);

	int nCount = _flightItem->GetTimeRangeData()->size();
	for (int i =0; i < nCount; i++)
	{
		CTrainingTimeRangeManage* pFlightType = _flightItem->GetItem(i);
		AddTimeRangeTreeItem(pFlightType , HFlightItem);
	}
	m_wndTreeCtrl.Expand(HFlightItem,TVE_EXPAND);
	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
}

void CDlgTrainFlightsManagement::OnAddTimeRange()
{
	HTREEITEM HSelItem = m_wndTreeCtrl.GetSelectedItem();
	CTrainingFlightTypeManage* PFlt_type = (CTrainingFlightTypeManage*)m_wndTreeCtrl.GetItemData(HSelItem);
	if(PFlt_type == NULL)
		return;
	ElapsedTime StartTime;
	ElapsedTime EndTime;
	CDlgTimeRange dlg(StartTime,EndTime);

	if(dlg.DoModal() == IDOK)
	{
		TimeRange Ptime;
		Ptime.SetStartTime(dlg.GetStartTime());
		Ptime.SetEndTime(dlg.GetEndTime());
		CTrainingTimeRangeManage* PTimeRange = PFlt_type->AddTrainingTimeRange(Ptime);
		if(PTimeRange == NULL)
		{			
			CString Errormsg;
			CString time = FormatTimeRangeString(Ptime.GetStartTime(),Ptime.GetEndTime());
			Errormsg.Format(_T("This time \'%s\' is invalid !"),time);
			MessageBox(Errormsg,MB_OK);
			return;
		}
		AddTimeRangeTreeItem(PTimeRange,HSelItem);
	}
	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
}

void CDlgTrainFlightsManagement::OnEditTimeRange()
{
	HTREEITEM HSelItem = m_wndTreeCtrl.GetSelectedItem();
	HTREEITEM HParItem = m_wndTreeCtrl.GetParentItem(HSelItem);
	CTrainingFlightTypeManage* pFlightData = (CTrainingFlightTypeManage*)m_wndTreeCtrl.GetItemData(HParItem);
	if(pFlightData == NULL)
		return;

	CTrainingTimeRangeManage* pTimeRange = (CTrainingTimeRangeManage*)m_wndTreeCtrl.GetItemData(HSelItem);
	if(pTimeRange == NULL)
		return;
	CDlgTimeRange dlg(pTimeRange->GetStartTime(),pTimeRange->GetEndTime());
	if(dlg.DoModal() == IDOK)
	{
		TimeRange timeRange;
		timeRange.SetStartTime(dlg.GetStartTime());
		timeRange.SetEndTime(dlg.GetEndTime());

		if(pFlightData->FindTrainingTimeRangee(timeRange) != pTimeRange)
		{
			CString Errormsg;
			CString time = FormatTimeRangeString(timeRange.GetStartTime(),timeRange.GetEndTime());
			Errormsg.Format(_T("This time \'%s\' is invalid !"),time);
			MessageBox(Errormsg,MB_OK);
			return;
		}

		pTimeRange->SetStartTime(timeRange.GetStartTime());
		pTimeRange->SetEndTime(timeRange.GetEndTime());
		EditTimeRangeTreeItem(pTimeRange,HSelItem);
	}
	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
}

void CDlgTrainFlightsManagement::OnDeleteTimeRange()
{
	HTREEITEM HSelItem = m_wndTreeCtrl.GetSelectedItem();
	HTREEITEM HParItem = m_wndTreeCtrl.GetParentItem(HSelItem);

	CTrainingFlightTypeManage* PFltType = (CTrainingFlightTypeManage*)m_wndTreeCtrl.GetItemData(HParItem);
	if(PFltType == NULL)
		return;

	CTrainingTimeRangeManage* PTimeItem = (CTrainingTimeRangeManage*)m_wndTreeCtrl.GetItemData(HSelItem);
	if(PTimeItem == NULL)
		return;

	PFltType->RemoveTrainingTimeRangeeItem(PTimeItem);
	m_wndTreeCtrl.DeleteItem(HSelItem);
	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
}

void CDlgTrainFlightsManagement::OnContextMenu( CWnd* pWnd, CPoint point)
{
	CMenu* pMenu=NULL;
	CMenu menuPopup;

	CRect  rtFltTypeTree;
	m_wndTreeCtrl.GetWindowRect(&rtFltTypeTree);
	if( rtFltTypeTree.PtInRect(point))
	{
		m_wndTreeCtrl.SetFocus();
		CPoint pt = point;
		m_wndTreeCtrl.ScreenToClient( &pt );
		UINT iRet;
		HTREEITEM hRClickItem = m_wndTreeCtrl.HitTest( pt, &iRet );
		if (iRet != TVHT_ONITEMLABEL)
		{
			hRClickItem = NULL;
			return;
		}
		m_wndTreeCtrl.SelectItem(hRClickItem);

		TY_NODE nodeType = GetTreeItemNodeType(hRClickItem);
		switch(nodeType)
		{
		case TY_TREENODE_FLIGHT:
			{
				menuPopup.LoadMenu( IDR_MENU_FLIGHTSTYPEMANAGEMENT );
				pMenu = menuPopup.GetSubMenu(0);
			}
			break;
		case TY_TREENODE_TIMERANGE:
			{
				menuPopup.LoadMenu( IDR_MENU_TIMENRANGE );
				pMenu = menuPopup.GetSubMenu(0);
			}
			break;
		case TY_TREENODE_TAKEOFFPOS:
			{						
				menuPopup.LoadMenu( IDR_MENU_TAKEOFFFLIGHTSPOSITION );
				pMenu = menuPopup.GetSubMenu(0);
			}
			break;
		default:
			break;
		}
	}
	if (pMenu)
		pMenu->TrackPopupMenu(TPM_LEFTALIGN, point.x, point.y, this);	
}

CString CDlgTrainFlightsManagement::FormatTimeRangeString(ElapsedTime& _startTime ,ElapsedTime& _endTime)
{
	CString SQL;
	SQL.Format(_T("%s - %s"),_startTime.PrintDateTime(), _endTime.PrintDateTime());
	return SQL;
}

void CDlgTrainFlightsManagement::AddTimeRangeTreeItem(CTrainingTimeRangeManage* _TimeRange, HTREEITEM _FltItem)
{
	if(_TimeRange == NULL || _FltItem == NULL)
		return;
	COOLTREE_NODE_INFO info;
	m_wndTreeCtrl.InitNodeInfo(this ,info);
	info.net = NET_NORMAL;
	info.nt = NT_NORMAL;
	info.bAutoSetItemText  = FALSE;
	CString ItemText;
	ItemText = FormatTimeRangeString(_TimeRange->GetStartTime(),_TimeRange->GetEndTime());
	HTREEITEM PTimerange = m_wndTreeCtrl.InsertItem(ItemText,info,FALSE,FALSE,_FltItem);
	m_wndTreeCtrl.SetItemData(PTimerange,(DWORD_PTR)_TimeRange);

	int nCount = _TimeRange->GetTakeOffPosData()->size();
	for (int i =0; i < nCount; i++)
	{		
		std::vector<RunwayExit> vExitList;
		CTrainingTakeOffPosition* pTakeoffPotion = _TimeRange->GetTakeOffPosData()->at(i);
		for (int i = 0; i < (int)pTakeoffPotion->m_runwayIdList.size(); i++)
		{
			RunwayExit vWayExit;
			vWayExit.ReadData(pTakeoffPotion->m_runwayIdList.at(i));
			vExitList.push_back(vWayExit);
		}
		AddTakeOffPositionItem(pTakeoffPotion,vExitList,PTimerange);
	}
	m_wndTreeCtrl.Expand(PTimerange,TVE_EXPAND);
	m_wndTreeCtrl.Expand(_FltItem,TVE_EXPAND);
}

void CDlgTrainFlightsManagement::EditTimeRangeTreeItem(CTrainingTimeRangeManage* _TimeRange,HTREEITEM _TimeItem) 
{
	if(_TimeRange == NULL || _TimeItem == NULL)
		return;
	CString ItemText;
	ItemText = FormatTimeRangeString(_TimeRange->GetStartTime(),_TimeRange->GetEndTime());
	m_wndTreeCtrl.SetItemText(_TimeItem,ItemText);
	m_wndTreeCtrl.SetItemData(_TimeItem,(DWORD_PTR)_TimeRange);
}

CDlgTrainFlightsManagement::TY_NODE CDlgTrainFlightsManagement::GetTreeItemNodeType( HTREEITEM _item )
{
	HTREEITEM HParItem = m_wndTreeCtrl.GetParentItem(_item);
	if(HParItem == NULL)
		return TY_TREENODE_FLIGHT;

	HParItem = m_wndTreeCtrl.GetParentItem(HParItem);
	if(HParItem == NULL)
		return TY_TREENODE_TIMERANGE;

	HParItem = m_wndTreeCtrl.GetParentItem(HParItem);
	if(HParItem == NULL)
		return TY_TREENODE_TAKEOFFPOS;

	return TY_TREENODE_NULL;
}

void CDlgTrainFlightsManagement::OnAddTakeoffPosition()
{
	HTREEITEM HSelItem = m_wndTreeCtrl.GetSelectedItem();
	if (HSelItem == NULL)
		return;

	CTrainingTimeRangeManage *pTimeRangeItem = (CTrainingTimeRangeManage*)m_wndTreeCtrl.GetItemData(HSelItem);
		if(pTimeRangeItem == NULL)
		return ;
	
	CDlgSelectTakeoffPosition *m_dlgTakeoffPos=new CDlgSelectTakeoffPosition(m_nProjID,-1,-1, this,true);
	m_dlgTakeoffPos->SetAirportDB(m_pAirportDB);

	if(m_dlgTakeoffPos->DoModal() != IDOK)
	{
		delete m_dlgTakeoffPos;
		m_dlgTakeoffPos = NULL;
		return;
	}
	std::vector<RunwayExit> vExitList;
	if (m_dlgTakeoffPos->m_bSelAll)
	{
		RunwayExit runwauexit;
		runwauexit.ReadData(-1);
		vExitList.push_back(runwauexit);
	}
	else
	{
		vExitList = m_dlgTakeoffPos->m_vRunwayExitList;
	}

	if (m_dlgTakeoffPos)
	{
		delete m_dlgTakeoffPos;
		m_dlgTakeoffPos = NULL;
	}
	
	if(pTimeRangeItem->FindTakeoffPosition(vExitList))
	{
		MessageBox(_T("The take off position is already exit!"),_T("Warning"),MB_OK);
		return;
	}
	CTrainingTakeOffPosition* TrainTakeOffPosItem = pTimeRangeItem->AddTrainTakeoffPosition(vExitList);
	AddTakeOffPositionItem(TrainTakeOffPosItem,vExitList,HSelItem);
	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
}

void CDlgTrainFlightsManagement::OnEditTakeoffPosition()
{
	HTREEITEM hSelItem = m_wndTreeCtrl.GetSelectedItem();
	if (hSelItem == NULL)
		return;

	CTrainingTakeOffPosition* pTakeoffPos = (CTrainingTakeOffPosition*)m_wndTreeCtrl.GetItemData(hSelItem);
	if (pTakeoffPos == NULL)
		return;
	HTREEITEM hParItem = m_wndTreeCtrl.GetParentItem(hSelItem);
	CTrainingTimeRangeManage* pRwyData = (CTrainingTimeRangeManage*)m_wndTreeCtrl.GetItemData(hParItem);


	CDlgSelectTakeoffPosition *m_dlgTakeoffPos=new CDlgSelectTakeoffPosition(m_nProjID,-1,-1, this,true);
	m_dlgTakeoffPos->SetAirportDB(m_pAirportDB);

	if(m_dlgTakeoffPos->DoModal() != IDOK)
	{
		delete m_dlgTakeoffPos;
		m_dlgTakeoffPos = NULL;
		return;
	}
	std::vector<RunwayExit> vExitList;
	if (m_dlgTakeoffPos->m_bSelAll)
	{
		RunwayExit runwauexit;
		runwauexit.ReadData(-1);
		vExitList.push_back(runwauexit);
	}
	else
	{
		vExitList = m_dlgTakeoffPos->m_vRunwayExitList;
	}
	if (m_dlgTakeoffPos)
	{
		delete m_dlgTakeoffPos;
		m_dlgTakeoffPos = NULL;
	}

	if (pRwyData->FindTakeoffPosition(vExitList))
	{
		MessageBox(_T("The take off position is the same!"),_T("Warning"),MB_OK);
		return;
	}
	std::vector<int> IDLIST;
	for (int i = 0; i < (int)vExitList.size(); i++)
	{
		IDLIST.push_back(vExitList.at(i).GetID());
	}
	pTakeoffPos->m_runwayIdList = IDLIST;
	CString Str;
	Str = FormatTakeOffItemText(vExitList);
	m_wndTreeCtrl.SetItemText(hSelItem, Str);
	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
}

void CDlgTrainFlightsManagement::OnDeleteTakeoffPosition()
{
	HTREEITEM HSelItem = m_wndTreeCtrl.GetSelectedItem();
	CTrainingTakeOffPosition* pTakeOffPosition = (CTrainingTakeOffPosition*) m_wndTreeCtrl.GetItemData(HSelItem);
	if(pTakeOffPosition == NULL)
		return;
	HTREEITEM HParItem = m_wndTreeCtrl.GetParentItem(HSelItem);
	CTrainingTimeRangeManage* pTimeItem = (CTrainingTimeRangeManage*)m_wndTreeCtrl.GetItemData(HParItem);

	if(pTimeItem == NULL)
		return;
	pTimeItem->RemoveTrainTakeoffPosition(pTakeOffPosition);
	m_wndTreeCtrl.DeleteItem(HSelItem);	
	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
}

CString CDlgTrainFlightsManagement::FormatTakeOffItemText(std::vector<RunwayExit> _runwayExit)
{
	CString Str;
	for(int i = 0; i < (int)_runwayExit.size(); i++)
	{
		RunwayExit runwayexit = _runwayExit.at(i);
		if (runwayexit.GetID() == -1)
		{
			Str = "ALL";
		} 
		else
		{
			Str += _runwayExit.at(i).GetName()+";";
		}
		
	}
	CString SQL;
	SQL.Format(_T("Take off postion: %s"),Str);
	return SQL;
}

void CDlgTrainFlightsManagement::AddTakeOffPositionItem(CTrainingTakeOffPosition* _positionStr,std::vector<RunwayExit> _runwayExit,HTREEITEM _PositionsItem)
{
	COOLTREE_NODE_INFO info;
	CARCTreeCtrl::InitNodeInfo(this,info);
	info.bAutoSetItemText = FALSE;
	info.net = NET_NORMAL;	
	CString Str;
	Str = FormatTakeOffItemText(_runwayExit);
	HTREEITEM _TakeOffItem = m_wndTreeCtrl.InsertItem(Str,info,FALSE,FALSE,_PositionsItem,TVI_LAST);
	m_wndTreeCtrl.SetItemData(_TakeOffItem,(DWORD)_positionStr);

	COOLTREE_NODE_INFO infuu;
	CARCTreeCtrl::InitNodeInfo(this,infuu);
	HTREEITEM _TakeOffSequenceItem=m_wndTreeCtrl.InsertItem("TakeOff Sequence",infuu,FALSE,FALSE,_TakeOffItem);
	m_wndTreeCtrl.SetItemData(_TakeOffSequenceItem,(DWORD)_positionStr);

	infuu.nMaxCharNum = TY_TRAIN_TAKEOFF_ITEM;
	infuu.nt=NT_RADIOBTN; 
	infuu.net = NET_NORMAL;
	m_Takeoff=m_wndTreeCtrl.InsertItem("FIFO",infuu,FALSE,FALSE,_TakeOffSequenceItem);
	m_wndTreeCtrl.m_hPreItem = NULL;
	if (!m_wndTreeCtrl.IsRadioItem(m_Takeoff))
		m_wndTreeCtrl.SetRadioStatus(m_Takeoff,TRUE);
	else
		m_wndTreeCtrl.SetRadioStatus(m_Takeoff,FALSE);

	infuu.nMaxCharNum = TY_TRAIN_TAKEOFFSEQUENCE_ITEM;
	infuu.nt=NT_RADIOBTN;
	infuu.net = NET_EDITSPIN_WITH_VALUE;	
	CString strTempValue;
	strTempValue.Format(_T("After (%d) Pax Flights"),_positionStr->m_TakeOffPax);
	m_hTakeOffItem=m_wndTreeCtrl.InsertItem(strTempValue,infuu,FALSE,FALSE,_TakeOffSequenceItem);
	m_wndTreeCtrl.SetItemData(m_hTakeOffItem,(DWORD)_positionStr->m_TakeOffPax);
	m_wndTreeCtrl.SetItemValueRange(m_hTakeOffItem,0,1000000);
	if (m_wndTreeCtrl.IsRadioItem(m_hTakeOffItem) || _positionStr->m_TakeOffSequence == FALSE)
		m_wndTreeCtrl.SetRadioStatus(m_hTakeOffItem,TRUE);
	else
		m_wndTreeCtrl.SetRadioStatus(m_hTakeOffItem,FALSE);



	COOLTREE_NODE_INFO infu;
	CARCTreeCtrl::InitNodeInfo(this,infu);
	HTREEITEM _ApproachSequenceItem=m_wndTreeCtrl.InsertItem("Approach Sequence",infu,FALSE,FALSE,_TakeOffItem);
	m_wndTreeCtrl.SetItemData(_ApproachSequenceItem,(DWORD)_positionStr);

	infu.nMaxCharNum = TY_TRAIN_APPROACH_ITEM;
	infu.nt=NT_RADIOBTN; 
	infu.net = NET_NORMAL;
	m_TrueRandoom=m_wndTreeCtrl.InsertItem("FIFO",infu,FALSE,FALSE,_ApproachSequenceItem);
	m_wndTreeCtrl.m_hPreItem = NULL;
	if (!m_wndTreeCtrl.IsRadioItem(m_TrueRandoom))
		m_wndTreeCtrl.SetRadioStatus(m_TrueRandoom,TRUE);
	else
		m_wndTreeCtrl.SetRadioStatus(m_TrueRandoom,FALSE);
	
	infu.nMaxCharNum = TY_TRAIN_APPROACHSEQUENCE_ITEM;
	infu.nt=NT_RADIOBTN;
	infu.net = NET_EDITSPIN_WITH_VALUE;
	CString strValue;
	strValue.Format(_T("After (%d) Pax Flights"),_positionStr->m_ApproachPax);
    m_hApproachitem=m_wndTreeCtrl.InsertItem(strValue,infu,FALSE,FALSE,_ApproachSequenceItem);
	m_wndTreeCtrl.SetItemData(m_hApproachitem,(DWORD)_positionStr->m_ApproachPax);
	m_wndTreeCtrl.SetItemValueRange(m_hApproachitem,0,1000000);
	if (m_wndTreeCtrl.IsRadioItem(m_hApproachitem) || _positionStr->m_ApproachSequence == FALSE)
		m_wndTreeCtrl.SetRadioStatus(m_hApproachitem,TRUE);
	else
		m_wndTreeCtrl.SetRadioStatus(m_hApproachitem,FALSE);

	m_wndTreeCtrl.Expand(_PositionsItem,TVE_EXPAND);
	m_wndTreeCtrl.Expand(_TakeOffSequenceItem,TVE_EXPAND);
	m_wndTreeCtrl.Expand(_ApproachSequenceItem,TVE_EXPAND);
}

LRESULT CDlgTrainFlightsManagement::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
	case UM_CEW_EDITSPIN_END:
		{
			HTREEITEM hItem = (HTREEITEM)wParam;
			TY_PRIORITYTREE_NODE NodeType = (TY_PRIORITYTREE_NODE)m_wndTreeCtrl.GetItemNodeInfo(hItem)->nMaxCharNum;
			HTREEITEM hParentItem = m_wndTreeCtrl.GetParentItem(hItem);
			CTrainingTakeOffPosition* pTakeOffPosition = (CTrainingTakeOffPosition*)m_wndTreeCtrl.GetItemData(hParentItem);
			CString strValue = *((CString*)lParam);
			double ValFloat = atof(strValue);
			CString strText;
			if	(NodeType == TY_TRAIN_TAKEOFFSEQUENCE_ITEM)
			{
				if(m_wndTreeCtrl.IsRadioItem(hItem))
				{
					pTakeOffPosition->m_TakeOffPax = (int)ValFloat;
					strText.Format("After (%d) Pax Flights",pTakeOffPosition->m_TakeOffPax);
					m_wndTreeCtrl.SetItemText(hItem,strText);
					m_wndTreeCtrl.SetItemData(hItem,(DWORD)pTakeOffPosition->m_TakeOffPax);
				}
				else
				{
					strText.Format("After (%d) Pax Flights", pTakeOffPosition->m_TakeOffPax);
					m_wndTreeCtrl.SetItemText(hItem,strText);
					m_wndTreeCtrl.SetItemData(hItem,(DWORD)pTakeOffPosition->m_TakeOffPax);
				}
			}
	
			if(NodeType == TY_TRAIN_APPROACHSEQUENCE_ITEM)
			{
				if(m_wndTreeCtrl.IsRadioItem(hItem))
				{
					
					pTakeOffPosition->m_ApproachPax = (int)ValFloat;
					strText.Format("After (%d) Pax Flights",pTakeOffPosition->m_ApproachPax);
					m_wndTreeCtrl.SetItemText(hItem,strText);
					m_wndTreeCtrl.SetItemData(hItem,(DWORD)pTakeOffPosition->m_ApproachPax);
				}
				else
				{
					strText.Format("After (%d) Pax Flights",pTakeOffPosition->m_ApproachPax);
					m_wndTreeCtrl.SetItemText(hItem,strText);
					m_wndTreeCtrl.SetItemData(hItem,(DWORD)pTakeOffPosition->m_ApproachPax);
				}
			}
			GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
		 }	
		 break;
	case UM_CEW_STATUS_CHANGE:
		{
			HTREEITEM hItem = (HTREEITEM)wParam;
			TY_PRIORITYTREE_NODE NodeType = (TY_PRIORITYTREE_NODE)m_wndTreeCtrl.GetItemNodeInfo(hItem)->nMaxCharNum;
			HTREEITEM hParentItem = m_wndTreeCtrl.GetParentItem(hItem);
			CTrainingTakeOffPosition* pTakeOffPosition = (CTrainingTakeOffPosition*)m_wndTreeCtrl.GetItemData(hParentItem);
			if (NodeType == TY_TRAIN_TAKEOFF_ITEM)
			{
				m_wndTreeCtrl.SetRadioStatus(hItem);
				pTakeOffPosition->m_TakeOffSequence = TRUE;
			}
			if (NodeType == TY_TRAIN_TAKEOFFSEQUENCE_ITEM)
			{
				m_wndTreeCtrl.SetRadioStatus(hItem);
				pTakeOffPosition->m_TakeOffSequence = FALSE;
			}
			if (NodeType == TY_TRAIN_APPROACH_ITEM)
			{
				m_wndTreeCtrl.SetRadioStatus(hItem);
				pTakeOffPosition->m_ApproachSequence = TRUE;
			}
			if (NodeType == TY_TRAIN_APPROACHSEQUENCE_ITEM)
			{
				m_wndTreeCtrl.SetRadioStatus(hItem);
				pTakeOffPosition->m_ApproachSequence = FALSE;
			}
			GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
		}
		break;
	default:
		break;
	}
	
	return CDialog::DefWindowProc(message,wParam,lParam);
}

void CDlgTrainFlightsManagement::OnSave()
{
	m_Data.WriteData();
	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(FALSE);
}

void CDlgTrainFlightsManagement::OnOk()
{
	OnSave();
	CXTResizeDialog::OnOK();
}

void CDlgTrainFlightsManagement::OnCancel()
{
	m_Data.ReadData();
	CXTResizeDialog::OnCancel();
}