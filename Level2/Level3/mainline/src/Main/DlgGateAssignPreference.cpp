// DlgGateAssignPreference.cpp : implementation file
//

#include "stdafx.h"
#include "TermPlan.h"
#include "DlgGateAssignPreference.h"
#include "DlgGateSelect.h"
#include "FlightDialog.h"
#include <Inputs/IN_TERM.H>
// CDlgGateAssignPreference dialog
#include "Engine/gate.h"
//#include "DlgGateAdjacency.h"
IMPLEMENT_DYNAMIC(CDlgGateAssignPreference, CXTResizeDialog)
CDlgGateAssignPreference::CDlgGateAssignPreference(CGateAssignPreferenceMan* _PreferenceMan,InputTerminal* _terminal,CWnd* pParent)
	:m_PreferenceMan(_PreferenceMan),m_terminal(_terminal), CXTResizeDialog(CDlgGateAssignPreference::IDD, pParent)
{
	m_pGateAssignmentMgr = m_PreferenceMan->GetGateAssignMgr() ;
}

CDlgGateAssignPreference::~CDlgGateAssignPreference()
{
	for (int i = 0 ; i < (int)m_NodeTypes.size() ;i++)
	{
		delete m_NodeTypes[i] ;
	}
	m_NodeTypes.clear() ;
}

void CDlgGateAssignPreference::DoDataExchange(CDataExchange* pDX)
{
	CXTResizeDialog::DoDataExchange(pDX);
	DDX_Control(pDX,IDC_TREE_PREFERENCE,m_TreeCtrl) ;
}


BEGIN_MESSAGE_MAP(CDlgGateAssignPreference, CXTResizeDialog)
	ON_WM_CREATE() 
	ON_WM_SIZE() 
	ON_COMMAND(ID_PREFERENCE_ADD,OnNewButton) 
	ON_COMMAND(ID_PREFERENCE_DEL,OnDelButton) 
	ON_COMMAND(ID_PREFERENCE_EDIT,OnEditButton) 
	ON_NOTIFY(TVN_SELCHANGED,IDC_TREE_PREFERENCE,OnTvnChangedTree)
    ON_BN_CLICKED(IDC_BUTTON_GATE_ADJACENCY, OnBnClickedButtonAdjacency)
END_MESSAGE_MAP()

void CDlgGateAssignPreference::OnSize(UINT nType, int cx, int cy)
{
	CXTResizeDialog::OnSize(nType,cx,cy) ;
}
int CDlgGateAssignPreference::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if( CXTResizeDialog::OnCreate(lpCreateStruct)== -1)
		return -1 ;
	if (!m_ToolBar.CreateEx(this,  TBSTYLE_FLAT,WS_CHILD|WS_VISIBLE|CBRS_ALIGN_TOP ) ||
		!m_ToolBar.LoadToolBar(IDR_GATE_PREFERENCE))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}
	return 0;
}
void CDlgGateAssignPreference::InitToolBar()
{
	CRect rect ;
	GetDlgItem(IDC_STATIC_TOOLBAR)->GetWindowRect(&rect) ;
	ScreenToClient(&rect) ;

	GetDlgItem(IDC_STATIC_TOOLBAR)->ShowWindow(SW_HIDE) ;
	GetDlgItem(IDC_STATIC_TOOLBAR)->EnableWindow(FALSE) ;
	m_ToolBar.MoveWindow(&rect) ;
	m_ToolBar.GetToolBarCtrl().EnableButton(ID_PREFERENCE_ADD,TRUE) ;
	m_ToolBar.GetToolBarCtrl().EnableButton(ID_PREFERENCE_DEL,FALSE) ;
	m_ToolBar.GetToolBarCtrl().EnableButton(ID_PREFERENCE_EDIT,FALSE) ;

	m_ToolBar.ShowWindow(SW_SHOW) ;

}
BOOL CDlgGateAssignPreference::OnInitDialog()
{
	CXTResizeDialog::OnInitDialog() ;
	SetWindowText(m_Caption) ;

	InitToolBar() ;
    InitTree() ;

	SetResize(IDC_STATIC_TOOLBAR,SZ_TOP_LEFT,SZ_TOP_LEFT);
	SetResize(m_ToolBar.GetDlgCtrlID(),SZ_TOP_LEFT,SZ_TOP_LEFT) ;
	SetResize(IDC_TREE_PREFERENCE,SZ_TOP_LEFT,SZ_BOTTOM_RIGHT) ;
	SetResize(IDOK,SZ_BOTTOM_RIGHT,SZ_BOTTOM_RIGHT) ;
	SetResize(IDCANCEL,SZ_BOTTOM_RIGHT,SZ_BOTTOM_RIGHT) ;
	return TRUE ;
}
void CDlgGateAssignPreference::InitTree()
{
	if(m_PreferenceMan == NULL)
		return ;
	CGateAssignPreferenceMan::DATA_TYPE_ITER iter = m_PreferenceMan->GetPreferenceData()->begin() ;
	for ( ;iter != m_PreferenceMan->GetPreferenceData()->end() ;iter++)
	{
		AddPreferenceItem(*iter) ;
	}
}
void CDlgGateAssignPreference::AddPreferenceItem(CGateAssignPreferenceItem* _itemData)
{
	if(_itemData == NULL)
		return ;
	COOLTREE_NODE_INFO info ;
	CCoolTree::InitNodeInfo(this,info) ;
	info.nt=NT_NORMAL;
	info.net = NET_SHOW_DIALOGBOX;
	HTREEITEM hItem;
	if (FindGateReferenceTreeNode(_itemData,hItem))
	{
		HTREEITEM GateItem = m_TreeCtrl.InsertItem(FormatGateNodeName(_itemData->GetGateID()),info,TRUE,FALSE,TVI_ROOT,hItem) ;
		CGateAssignPreferenceItem::DATA_TY_ITER iter =   _itemData->GetSubItemData()->begin() ;
		for ( ; iter != _itemData->GetSubItemData()->end() ;iter++)
		{
			AddPreferenceSubItem(*iter,GateItem) ;
		}
		CNodeData* nodedata = new CNodeData((void*)_itemData,Gate_NODE) ;
		m_TreeCtrl.SetItemData(GateItem,(DWORD)nodedata) ;
		m_NodeTypes.push_back(nodedata) ;
		m_TreeCtrl.Expand(GateItem,TVE_EXPAND) ;
	}
	
}
void CDlgGateAssignPreference::AddPreferenceSubItem(CGatePreferenceSubItem* _itemData, HTREEITEM _ParItem)
{
	if(_itemData == NULL )
		return ;
	COOLTREE_NODE_INFO info ;
	CCoolTree::InitNodeInfo(this,info) ;
	info.nt=NT_NORMAL;
	info.net = NET_SHOW_DIALOGBOX;
	HTREEITEM FlightItem = m_TreeCtrl.InsertItem(FormatFlightNodeName(_itemData->GetFlightTY()),info,TRUE,FALSE,_ParItem) ;
	CNodeData* nodedata = new CNodeData((void*)_itemData,FLIGHT_NODE) ;
	m_TreeCtrl.SetItemData(FlightItem,(DWORD)nodedata) ;
	m_NodeTypes.push_back(nodedata) ;

	//add duration node 
	info.nt=NT_NORMAL;
	info.net = NET_EDIT_INT;
	CString DurationNode ;
	DurationNode.Format(_T("Duration: %d(mins)"),_itemData->GetDuration()) ;
	HTREEITEM DurationItem = m_TreeCtrl.InsertItem(DurationNode,info,TRUE,FALSE,FlightItem) ;
	nodedata = new CNodeData((void*)NULL,FLIHT_NODE_DURATION) ;
	m_TreeCtrl.SetItemData(DurationItem,(DWORD_PTR)nodedata) ;
	m_NodeTypes.push_back(nodedata) ;

	//add overlap node
    CString OverLap ;
	OverLap.Format(_T("Allow overlap with flight type")) ;
	info.nt=NT_NORMAL;
	info.net = NET_NORMAL;
	HTREEITEM overlapITem = m_TreeCtrl.InsertItem(OverLap,info,TRUE,FALSE,FlightItem) ;
	nodedata = new CNodeData((void*)_itemData->GetOverLapFlightTY(),OVERLAp_NODE) ;
	m_TreeCtrl.SetItemData(overlapITem,(DWORD_PTR)nodedata) ;
	m_NodeTypes.push_back(nodedata) ;
    
	CGateOverLapFlightTY::DATA_TY_ITER iter = 	_itemData->GetOverLapFlightTY()->GetData()->begin() ;
	for ( ; iter != _itemData->GetOverLapFlightTY()->GetData()->end() ;iter++)
	{
		AddOverLapItem((*iter),overlapITem) ;
	}
    m_TreeCtrl.Expand(DurationItem,TVE_EXPAND) ;
	m_TreeCtrl.Expand(FlightItem,TVE_EXPAND) ;
}
void CDlgGateAssignPreference::AddOverLapItem(CGateOverLapFlightTYItem* _flightTy, HTREEITEM _ParItem)
{
	if(_flightTy == NULL)
		return ;
	COOLTREE_NODE_INFO info ;
	CCoolTree::InitNodeInfo(this,info) ;
	info.nt=NT_NORMAL;
	info.net = NET_SHOW_DIALOGBOX;
	HTREEITEM Item = m_TreeCtrl.InsertItem(FormatFlightNodeName(_flightTy->GetFlightTy()),info,TRUE,FALSE,_ParItem) ;
	CNodeData* nodedata = new CNodeData((void*)_flightTy,OVERLAp_NODE_FLIGHT) ;
	m_TreeCtrl.SetItemData(Item,(DWORD_PTR)nodedata) ;
	m_NodeTypes.push_back(nodedata) ;
}
CString CDlgGateAssignPreference::FormatFlightNodeName(FlightConstraint& _flight)
{
	CString ty ;
	_flight.screenPrint(ty) ;
	return _T("Flight type: ") + ty ;
}
void CDlgGateAssignPreference::OnNewButton()
{
	HTREEITEM SelItem = m_TreeCtrl.GetSelectedItem() ;
	if(SelItem == NULL)
	{
		NewGate() ;
		return ;
	}
    CNodeData* nodedata = (CNodeData*)m_TreeCtrl.GetItemData(SelItem) ;
	if(nodedata == NULL)
		return ;
	if(nodedata->m_type == Gate_NODE)
	{
		NewGateFlight((CGateAssignPreferenceItem*)nodedata->m_Data) ;
		return ;
	}
	if(nodedata->m_type == OVERLAp_NODE)
	{
		NewOverLapFlight((CGateOverLapFlightTY*)nodedata->m_Data) ;
		return ;
	}
}
void CDlgGateAssignPreference::NewGate()
{
	CDlgGateSelect dlg(m_terminal,m_Type) ;
	if(dlg.DoModal() == IDOK)
	{
		ProcessorID gateid ;
		gateid.SetStrDict(m_terminal->inStrDict) ;
		gateid.setID(dlg.GetSelGateName()) ;
		if(m_PreferenceMan->FindItemByGateID(gateid))
		{
			CString errormsg ;
			errormsg.Format(_T("You have defined one gate proceessor group which include \'%s\'"),dlg.GetSelGateName()) ;
			MessageBox(errormsg,_T("Warning"),MB_OK) ;
			return ;
		}
		CGateAssignPreferenceItem* item = m_PreferenceMan->AddPreferenceItem(gateid) ;
		if(item != NULL)
		AddPreferenceItem(item) ;
	}
}
void CDlgGateAssignPreference::NewGateFlight(CGateAssignPreferenceItem* _PreferenceItem)
{
	if(_PreferenceItem == NULL)
		return ;
	FlightConstraint fltType;
	CFlightDialog flightDlg(NULL, true);
	if (IDOK != flightDlg.DoModal())
		return;

	fltType = flightDlg.GetFlightSelection();
	CGatePreferenceSubItem* SubItem = _PreferenceItem->AddItemData(fltType) ;
	if (SubItem != NULL)
		AddPreferenceSubItem(SubItem,m_TreeCtrl.GetSelectedItem()) ;
}
void CDlgGateAssignPreference::NewOverLapFlight(CGateOverLapFlightTY* _OverFlight)
{
   if(_OverFlight == NULL)
	   return ;
   FlightConstraint fltType;
   CFlightDialog flightDlg(NULL, true);
   if (IDOK != flightDlg.DoModal())
	   return;
	fltType = flightDlg.GetFlightSelection();
	if(!_OverFlight->CheckTheFlightTYExist(fltType))
	{
		CGateOverLapFlightTYItem *  _FlightItem = _OverFlight->AddFlightTy(fltType) ;
		AddOverLapItem(_FlightItem,m_TreeCtrl.GetSelectedItem()) ;
	}
}
void CDlgGateAssignPreference::OnDelButton()
{
	HTREEITEM item = m_TreeCtrl.GetSelectedItem() ;
	if(item == NULL)
		return ;
	CNodeData* nodedata = (CNodeData*)m_TreeCtrl.GetItemData(item) ;
	if(nodedata->m_type == Gate_NODE)
	{
		m_PreferenceMan->DelPreferenceItem((CGateAssignPreferenceItem*)nodedata->m_Data) ;
		m_TreeCtrl.DeleteItem(item) ;
		return ;
	}
	if(nodedata->m_type == FLIGHT_NODE)
	{
		HTREEITEM _parItem = m_TreeCtrl.GetParentItem(item) ;
		CNodeData* Parnodedata = (CNodeData*)m_TreeCtrl.GetItemData(_parItem) ;  
		CGateAssignPreferenceItem* PreferenceItem = (CGateAssignPreferenceItem*)Parnodedata->m_Data ;
		PreferenceItem->DelItemData((CGatePreferenceSubItem*)nodedata->m_Data) ;
		m_TreeCtrl.DeleteItem(item) ;
		return ;	
	}
	if(nodedata->m_type == OVERLAp_NODE_FLIGHT)
	{
		HTREEITEM _parItem = m_TreeCtrl.GetParentItem(item) ;
		CNodeData* Parnodedata = (CNodeData*)m_TreeCtrl.GetItemData(_parItem) ;  
		CGateOverLapFlightTY* OverLapItem = (CGateOverLapFlightTY*)Parnodedata->m_Data ;
		CGateOverLapFlightTYItem* FlightItem = (CGateOverLapFlightTYItem*)nodedata->m_Data ;
		OverLapItem->DelFlightTy(FlightItem) ;
		m_TreeCtrl.DeleteItem(item) ;
		return ;	
	}

}	
void CDlgGateAssignPreference::EditGate() 
{
	HTREEITEM item = m_TreeCtrl.GetSelectedItem() ;
	if(item == NULL)
		return ;
	CNodeData* nodedata = (CNodeData*)m_TreeCtrl.GetItemData(item) ;
	CGateAssignPreferenceItem* Preference =	(CGateAssignPreferenceItem*)nodedata->m_Data ;
	if(Preference == NULL)
		return ;
	CDlgGateSelect dlg(m_terminal,m_Type) ;
	if(dlg.DoModal() == IDOK)
	{
		ProcessorID gateid ;
		gateid.SetStrDict(m_terminal->inStrDict) ;
		gateid.setID(dlg.GetSelGateName()) ;

		if(m_PreferenceMan->FindItemByGateID(gateid) != NULL && (m_PreferenceMan->FindItemByGateID(gateid) != Preference))
		{
			CString errormsg ;
			errormsg.Format(_T("You have defined one gate proceessor group which include \'%s\'"),dlg.GetSelGateName()) ;
			MessageBox(errormsg,_T("Warning"),MB_OK) ;
			return ;
		}

		Preference->SetGateID(gateid) ;
		m_TreeCtrl.SetItemText(item,FormatGateNodeName(gateid)) ;
	}
}
void CDlgGateAssignPreference::EditGateFlight() 
{
	HTREEITEM item = m_TreeCtrl.GetSelectedItem() ;
	if(item == NULL)
		return ;
	CNodeData* nodedata = (CNodeData*)m_TreeCtrl.GetItemData(item) ;
	CGatePreferenceSubItem* SubItem =	(CGatePreferenceSubItem*)nodedata->m_Data ;
	if(SubItem == NULL)
		return ; 
	FlightConstraint fltType;
	CFlightDialog flightDlg(NULL, true);
	if (IDOK != flightDlg.DoModal())
		return;
	fltType = flightDlg.GetFlightSelection();
	SubItem->SetFlightTY(fltType) ;
	m_TreeCtrl.SetItemText(item,FormatFlightNodeName(fltType)) ;
}
void CDlgGateAssignPreference::EditOverLapFlight() 
{
	HTREEITEM item = m_TreeCtrl.GetSelectedItem() ;
	if(item == NULL)
		return ;
	CNodeData* nodedata = (CNodeData*)m_TreeCtrl.GetItemData(item) ;
	CGateOverLapFlightTYItem* SubItem =	(CGateOverLapFlightTYItem*)nodedata->m_Data ;
	if(SubItem == NULL)
		return ; 
	FlightConstraint fltType;
	CFlightDialog flightDlg(NULL, true);
	if (IDOK != flightDlg.DoModal())
		return;
	fltType = flightDlg.GetFlightSelection();
	SubItem->SetFlightTy(fltType) ;
	m_TreeCtrl.SetItemText(item,FormatFlightNodeName(fltType)) ; 
}
void CDlgGateAssignPreference::OnEditButton(HTREEITEM _item ) 
{
	HTREEITEM item = NULL ;
	if(_item == NULL)
		item = m_TreeCtrl.GetSelectedItem() ;
	else
		item = _item ;
	if(item == NULL)
		return ;
	CNodeData* nodedata = (CNodeData*)m_TreeCtrl.GetItemData(item) ;
	if(nodedata->m_type == Gate_NODE)
	{
		EditGate();
	}
	if(nodedata->m_type == FLIGHT_NODE)
	{
		EditGateFlight() ;
	}
	if(nodedata->m_type == OVERLAp_NODE_FLIGHT)
		EditOverLapFlight() ;
}
void CDlgGateAssignPreference::OnEditButton()
{
  OnEditButton(NULL) ;
}
LRESULT CDlgGateAssignPreference::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
	case UM_CEW_SHOW_DIALOGBOX_BEGIN:
		{
			HTREEITEM _item = (HTREEITEM)wParam;
            OnEditButton(_item) ;
		}
		break;
	case UM_CEW_EDITSPIN_END:
	case UM_CEW_EDITWND_END:
		{
			CString strValue=*((CString*)lParam);
			CNodeData* nodedata = (CNodeData*)m_TreeCtrl.GetItemData(m_TreeCtrl.GetParentItem((HTREEITEM)wParam)) ;
			CGatePreferenceSubItem* SubItem =	(CGatePreferenceSubItem*)nodedata->m_Data ;
			SubItem->SetDuration(atoi(strValue)) ;

			CString DurationNode ;
			DurationNode.Format(_T("Duration: %s(mins)"),strValue) ;
			m_TreeCtrl.SetItemText((HTREEITEM)wParam,DurationNode) ;
		}
		break;
	default:
	    break;
	}
	return CXTResizeDialog::DefWindowProc(message,wParam,lParam) ;
}
void CDlgGateAssignPreference::OnOK()
{
	try
	{
		CADODatabase::BeginTransaction() ;
		m_PreferenceMan->SaveData() ;
		CADODatabase::CommitTransaction() ;
	}
	catch (CADOException& e)
	{
		e.ErrorMessage() ;
		CADODatabase::RollBackTransation() ;
	}
	CXTResizeDialog::OnOK() ;
}
void CDlgGateAssignPreference::OnCancel()
{
	m_PreferenceMan->ReadData(m_terminal) ;
	CXTResizeDialog::OnCancel() ;
}
// CDlgGateAssignPreference message handlers
CDlgArrivalGateAssignPreference::CDlgArrivalGateAssignPreference(CArrivalGateAssignPreferenceMan* _PreferenceMan,InputTerminal* _terminal,CWnd* pParent /* = NULL */)
:CDlgGateAssignPreference(_PreferenceMan,_terminal,pParent)
{
	m_Type = ArrGate ;
	m_Caption = _T("Arrival Gate Assignment Constraints") ;
}
CString CDlgArrivalGateAssignPreference::FormatGateNodeName(ProcessorID& _id)
{
	CString nodename ;
	nodename.Format(_T("Arrival Gate: %s"),_id.GetIDString()) ;
	return nodename ;
}
CDlgDepGateAssignPreference::CDlgDepGateAssignPreference(CDepGateAssignPreferenceMan* _PreferenceMan,InputTerminal* _terminal,CWnd* pParent /* = NULL */)
:CDlgGateAssignPreference(_PreferenceMan,_terminal,pParent)
{
	m_Type = DepGate ;
	m_Caption = _T("Departure Gate Assignment Constraints") ;
}
CString CDlgDepGateAssignPreference::FormatGateNodeName(ProcessorID& _id)
{
	CString nodename ;
	nodename.Format(_T("Departure Gate: %s"),_id.GetIDString()) ;
	return nodename ;
}
 void CDlgGateAssignPreference::OnTvnChangedTree(NMHDR *pNMHDR, LRESULT *pResult)
{
    HTREEITEM _selItem = m_TreeCtrl.GetSelectedItem() ;
	if(_selItem == NULL)
	{
		m_ToolBar.GetToolBarCtrl().EnableButton(ID_PREFERENCE_ADD,TRUE) ;
		m_ToolBar.GetToolBarCtrl().EnableButton(ID_PREFERENCE_DEL,FALSE) ;
		m_ToolBar.GetToolBarCtrl().EnableButton(ID_PREFERENCE_EDIT,FALSE) ;
	}else
	{
		CNodeData* Pnodedate = (CNodeData*)m_TreeCtrl.GetItemData(_selItem) ;
		if(Pnodedate == NULL)
		{
			m_ToolBar.GetToolBarCtrl().EnableButton(ID_PREFERENCE_ADD,TRUE) ;
			m_ToolBar.GetToolBarCtrl().EnableButton(ID_PREFERENCE_DEL,FALSE) ;
			m_ToolBar.GetToolBarCtrl().EnableButton(ID_PREFERENCE_EDIT,FALSE) ;
		}else
		{
			if(Pnodedate->m_type == Gate_NODE )
			{
				m_ToolBar.GetToolBarCtrl().EnableButton(ID_PREFERENCE_ADD,TRUE) ;
				m_ToolBar.GetToolBarCtrl().EnableButton(ID_PREFERENCE_DEL,TRUE) ;
				m_ToolBar.GetToolBarCtrl().EnableButton(ID_PREFERENCE_EDIT,TRUE) ;
				return ;
			}
			if(Pnodedate->m_type == OVERLAp_NODE)
			{
				m_ToolBar.GetToolBarCtrl().EnableButton(ID_PREFERENCE_ADD,TRUE) ;
				m_ToolBar.GetToolBarCtrl().EnableButton(ID_PREFERENCE_DEL,FALSE) ;
				m_ToolBar.GetToolBarCtrl().EnableButton(ID_PREFERENCE_EDIT,FALSE) ;
				return ;
			}
			if(Pnodedate->m_type == FLIHT_NODE_DURATION)
			{
				m_ToolBar.GetToolBarCtrl().EnableButton(ID_PREFERENCE_ADD,FALSE) ;
				m_ToolBar.GetToolBarCtrl().EnableButton(ID_PREFERENCE_DEL,FALSE) ;
				m_ToolBar.GetToolBarCtrl().EnableButton(ID_PREFERENCE_EDIT,FALSE) ;
				return ;
			}else
			{
				m_ToolBar.GetToolBarCtrl().EnableButton(ID_PREFERENCE_ADD,FALSE) ;
				m_ToolBar.GetToolBarCtrl().EnableButton(ID_PREFERENCE_DEL,TRUE) ;
				m_ToolBar.GetToolBarCtrl().EnableButton(ID_PREFERENCE_EDIT,TRUE) ;
				return ;
			}
		}
	}
}

bool CDlgGateAssignPreference::FindGateReferenceTreeNode( CGateAssignPreferenceItem* pItem,HTREEITEM& hItem )
{
	int idx = m_PreferenceMan->FindPreferenceIndex(pItem);
	if (idx == -1)
		return false;
	
	if (idx == 0)
	{
		hItem = TVI_FIRST;
		return true;
	}
	int iCircle = 0;
	HTREEITEM hRootItem = m_TreeCtrl.GetRootItem();
	hItem = hRootItem;
	while(hItem!= NULL && iCircle < idx)
	{
		hItem = m_TreeCtrl.GetNextSiblingItem(hItem);
		iCircle++;
	}

	return true;
}

void CDlgGateAssignPreference::OnBnClickedButtonAdjacency()
{
//    DlgGateAdjacency dlg(m_PreferenceMan->GetGateAdjacency());
 //   dlg.DoModal();
}
