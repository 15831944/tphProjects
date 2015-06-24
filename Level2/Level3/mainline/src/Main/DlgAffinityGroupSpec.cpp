// DlgAffinityGroupSpec.cpp : implementation file
//

#include "stdafx.h"
#include "TermPlan.h"
#include "DlgAffinityGroupSpec.h"
#include ".\dlgaffinitygroupspec.h"
#include "../InputOnBoard/AffinityGroup.h"
#include "../Common/NewPassengerType.h"
#include "DlgNewPassengerType.h"
// CDlgAffinityGroupSpec dialog
#include "../Common/FLT_CNST.H"
#include "FlightDialog.h"
#include "../MFCExControl/ARCBaseTree.h"
IMPLEMENT_DYNAMIC(CDlgAffinityGroupSpec, CXTResizeDialog)
CDlgAffinityGroupSpec::CDlgAffinityGroupSpec(InputTerminal* _terminal ,CWnd* pParent /*=NULL*/)
	: m_terminal(_terminal),CXTResizeDialog(CDlgAffinityGroupSpec::IDD, pParent)
{
	m_GroupManager = new CAffinityGroupManager ;
}

CDlgAffinityGroupSpec::~CDlgAffinityGroupSpec()
{
	delete m_GroupManager ;
	for (int i = 0 ; i < (int)m_TreeNodeData.size() ;i++)
	{
		delete m_TreeNodeData[i] ;
	}
	m_TreeNodeData.clear() ;
}

void CDlgAffinityGroupSpec::DoDataExchange(CDataExchange* pDX)
{
	CXTResizeDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC, m_StaToolBar);
	DDX_Control(pDX, IDC_TREE_AFFINITY, m_treeGroup);
// 	DDX_Control(pDX, IDC_EDIT_CAPTION, m_Caption);
// 	DDX_Control(pDX, IDC_STATIC_CAP, m_StaticCap);
	//DDX_Control(pDX, IDC_STATIC_TREE, m_sta_tree);
}

void CDlgAffinityGroupSpec::OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI) 
{
	// TODO: Add your message handler code here and/or call default
	lpMMI->ptMinTrackSize.x = 750;
	lpMMI->ptMinTrackSize.y = 500;
	CXTResizeDialog::OnGetMinMaxInfo(lpMMI);
}
BEGIN_MESSAGE_MAP(CDlgAffinityGroupSpec, CXTResizeDialog)
	ON_BN_CLICKED(IDC_BUTTON_SAVE, OnBnClickedButtonSave)
	ON_BN_CLICKED(IDOK,OnOK)
    ON_BN_CLICKED(IDCANCEL,OnCancel)
	ON_COMMAND(ID_TOOLBAR_NEWGROUP,OnNewToolBar)
	ON_COMMAND(ID_TOOLBAR_DELGROUP,OnDelToolBar)
	//ON_COMMAND(ID_TOOLBAR_EDITGROUP,OnEditToolBar)
  //  ON_WM_CONTEXTMENU()
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_AFFINITY, OnTvnSelchangedTree)
    ON_WM_CREATE() 
	ON_COMMAND(ID_AFFINITY_ASLEADER,HandleLeader)
	ON_COMMAND(ID_AFFINITY_ASPACER,HandlePacer)
	ON_WM_SIZE()
END_MESSAGE_MAP()
void CDlgAffinityGroupSpec::OnSize(UINT nType, int cx, int cy) 
{
	CXTResizeDialog::OnSize(nType,cx,cy) ;
}
int CDlgAffinityGroupSpec::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CXTResizeDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!m_wndToolbar.CreateEx(this,  TBSTYLE_FLAT,WS_CHILD|WS_VISIBLE|CBRS_ALIGN_TOP ) ||
		!m_wndToolbar.LoadToolBar(ID_TOOLBAR_EDITAFFINITYGROUP))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}
	return 0;
}
// CDlgAffinityGroupSpec message handlers
BOOL CDlgAffinityGroupSpec::OnInitDialog()
{
	CXTResizeDialog::OnInitDialog() ;
	m_GroupManager->ReadGroupFromDB() ;

	OnInitToolBar() ;
	OnInitTree() ;
// 	m_Caption.SetWindowText("Affinity Group Setting:");
// 	SetResize(IDC_STATIC_CAP,SZ_TOP_LEFT,SZ_TOP_RIGHT) ;
	//SetResize(IDC_STATIC_TREE,SZ_TOP_LEFT,SZ_BOTTOM_RIGHT);
// 	SetResize(IDC_EDIT_CAPTION,SZ_TOP_LEFT,SZ_TOP_LEFT) ;
	SetResize(IDC_TREE_AFFINITY,SZ_TOP_LEFT,SZ_BOTTOM_RIGHT) ;
	SetResize(m_wndToolbar.GetDlgCtrlID(),SZ_TOP_LEFT,SZ_TOP_LEFT);
	SetResize(IDC_BUTTON_SAVE,SZ_BOTTOM_RIGHT,SZ_BOTTOM_RIGHT);
	SetResize(IDOK,SZ_BOTTOM_RIGHT,SZ_BOTTOM_RIGHT);
	SetResize(IDCANCEL,SZ_BOTTOM_RIGHT,SZ_BOTTOM_RIGHT);
	return TRUE;
}
void CDlgAffinityGroupSpec::OnInitToolBar()
{
	CRect rect ;
	m_StaToolBar.GetWindowRect(&rect);
	ScreenToClient(&rect);
	m_wndToolbar.MoveWindow(rect);
	m_StaToolBar.ShowWindow(SW_HIDE) ;
	m_wndToolbar.ShowWindow(SW_SHOW) ;
	m_wndToolbar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_NEWGROUP,TRUE) ;
	m_wndToolbar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_DELGROUP,FALSE) ;
	//m_wndToolbar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_EDITGROUP,FALSE) ;

}
void CDlgAffinityGroupSpec::OnInitTree()
{
	//m_treeGroup.SetDisplayType(WM_INPLACE_EDIT) ;
	CAffinityGroupManager::TY_GROUP_CONTAINER_ITER iter = m_GroupManager->GetGroups()->begin() ;
	for ( ; iter != m_GroupManager->GetGroups()->end() ;iter++)
	{
         AddFlightTyNode(*iter) ;
	}
}
void CDlgAffinityGroupSpec::AddFlightTyNode(CAffinityData* _data)
{
	COOLTREE_NODE_INFO info ;
	CCoolTree::InitNodeInfo(this,info);
	info.nt=NT_NORMAL;
	info.net = NET_NORMAL;
	info.bAutoSetItemText = FALSE ;
	CString flightNode ;
	flightNode.Format("Flight Type : %s",_data->m_FliTY) ;
	HTREEITEM root = m_treeGroup.InsertItem(flightNode,info,TRUE,FALSE) ;
    NodeData* nodedata = new NodeData ;
	nodedata->nodeTy = TY_FLIGHTTYPE_NODE ;
	nodedata->pData = _data ;
	m_TreeNodeData.push_back(nodedata);
	m_treeGroup.SetItemData(root,(DWORD_PTR)nodedata) ;

	CAffinityData::TY_GROUP_ITER iter = _data->m_groups.begin() ;
	for ( ; iter != _data->m_groups.end() ;iter++)
	{
		AddGroupTreeNode(*iter,root) ;
	}
	m_treeGroup.Expand(root,TVE_EXPAND) ;
}
CString CDlgAffinityGroupSpec::FormatGroupTreeNodeName(CString _name)
{
	return "Group Name :" + _name ;
}
CString CDlgAffinityGroupSpec::FormatPaxTYNodeName(CString _name)
{
    return "Passenger Type: " +  _name ;
}
CString CDlgAffinityGroupSpec::FormatGroupNumOnAircraftNodeName(int _num)
{
	CString str ;
	str.Format("Number of groups on aircraft : %d",_num) ;
	return str ;
}
CString CDlgAffinityGroupSpec::FormatGroupCohensionNodeName(BOOL _res)
{
	CString str ;
	if(_res)
		str.Format("Cohesion : %s","YES");
	else
		str.Format("Cohesion : %s","NO");
	return str ;

}
CString CDlgAffinityGroupSpec::FormatNumberInPaxTyNodeName(int _num)
{
	CString str ;
	str.Format("Number in groups : %d",_num) ;
	return str ;
}
CString CDlgAffinityGroupSpec::FormatPassengerTyLeaderNodeName(BOOL _res)
{
	CString str ;
	if(_res)
		str.Format("Leader : %s","TRUE");
	else
		str.Format("Leader : %s","FALSE");
	return str ;
}
CString CDlgAffinityGroupSpec::FormatPassengerTyPacerNodeName(BOOL _res)
{
	CString str ;
	if(_res)
		str.Format("Pacer : %s","TRUE");
	else
		str.Format("Pacer : %s","FALSE");
	return str ;
}
void CDlgAffinityGroupSpec::AddGroupTreeNode(CAffinityGroup* _group,HTREEITEM _FlightNode)
{
    if(_group == NULL)
		return ;
	COOLTREE_NODE_INFO info ;
	CCoolTree::InitNodeInfo(this,info);
	info.nt=NT_NORMAL;
	info.net = NET_LABLE;
	info.bAutoSetItemText = FALSE ;
	CString nodename = FormatGroupTreeNodeName(_group->GetGroupName()) ;
	HTREEITEM item = m_treeGroup.InsertItem(nodename,info,TRUE,FALSE,_FlightNode) ;

	NodeData* nodedata = new NodeData ;
	nodedata->nodeTy = TY_GROUP_NODE ;
	nodedata->pData = _group ;
	m_TreeNodeData.push_back(nodedata) ;
	m_treeGroup.SetItemData(item,(DWORD_PTR)nodedata);

	//insert number groups On Aircraft node
	 nodedata = new NodeData ;
	nodedata->nodeTy = TY_OTHER_NODE ;
	info.net = NET_EDIT_INT ;
	info.fMinValue = 1 ;
	info.fMaxValue = 1000 ;
	HTREEITEM childItem = m_treeGroup.InsertItem(FormatGroupNumOnAircraftNodeName(_group->GetGroupsNumber()),info,True,FALSE,item) ;
	m_TreeNodeData.push_back(nodedata) ;
	m_treeGroup.SetItemData(childItem,(DWORD_PTR)nodedata) ;

    //insert cohesion node 
    nodedata = new NodeData ;
	nodedata->nodeTy = TY_COHESION_NODE ;
	info.net = NET_COMBOBOX ;
	info.dwItemData = TY_COHESION_NODE ;
	childItem = m_treeGroup.InsertItem(FormatGroupCohensionNodeName(_group->IsCohesion()),info,TRUE,FALSE,item);
	m_TreeNodeData.push_back(nodedata) ;
	m_treeGroup.SetItemData(childItem,(DWORD_PTR)nodedata) ;

	//add passenger ty root node 
	info.net = NET_NORMAL ;
	nodedata = new NodeData ;
	nodedata->nodeTy = TY_PAXTY_ROOT_NODE ;
	childItem = m_treeGroup.InsertItem("Passenger Types Node",info,TRUE,FALSE,item) ;
	m_TreeNodeData.push_back(nodedata);
	m_treeGroup.SetItemData(childItem,(DWORD_PTR)nodedata) ;

	CAffinityGroup::TY_PASSENGERTY_CONTAINER_ITER iter = _group->GetPaxTYs()->begin() ;
	for ( ; iter != _group->GetPaxTYs()->end() ; iter++)
	{
             AddGroupPaxTyTreeNode(*iter,childItem) ;
	}
	m_treeGroup.Expand(item,TVE_EXPAND) ;
}
void CDlgAffinityGroupSpec::AddGroupPaxTyTreeNode(CAffinityPassengerTy* _paxty,HTREEITEM item)
{
    if(_paxty == NULL)
		return ;
	COOLTREE_NODE_INFO info ;
	CCoolTree::InitNodeInfo(this,info);
	info.nt=NT_NORMAL;
	info.net = NET_SHOW_DIALOGBOX;
	info.bAutoSetItemText = FALSE ;
	info.unMenuID = IDR_MENU_AFFINITY ;
    //insert Passenger Type Node 
	CString PaxName ;
	PaxName = FormatPaxTYNodeName(_paxty->GetName()) ;
	HTREEITEM paxtyNode = m_treeGroup.InsertItem(PaxName,info,TRUE,FALSE,item) ;
	NodeData* pnodedata = new NodeData ;
	pnodedata->nodeTy = TY_PAXTY_NODE ;
	pnodedata->pData = _paxty ;
	m_TreeNodeData.push_back(pnodedata) ;
	m_treeGroup.SetItemData(paxtyNode,(DWORD_PTR)pnodedata) ;
   
	//insert number in Groups node 

	CString numIngroup ;
	numIngroup = FormatNumberInPaxTyNodeName(_paxty->GetNumber()) ;
	info.net = NET_EDIT_INT ;
	info.fMinValue = 1 ;
	info.fMaxValue = 1000 ;
	pnodedata = new NodeData ;
	pnodedata->nodeTy = TY_PAXTY_NUMBER_NODE ;
	HTREEITEM childItem = m_treeGroup.InsertItem(numIngroup,info,TRUE,FALSE,paxtyNode) ;
	m_TreeNodeData.push_back(pnodedata);
	m_treeGroup.SetItemData(childItem,(DWORD_PTR)pnodedata) ;

	//insert leader show node 
	if(_paxty->IsLeader())
	   InsertLeaderItem(paxtyNode,_paxty) ;

	//insert Pacer show node 
	if(_paxty->IsPacer())
	InserPacerItem(paxtyNode,_paxty) ;

	m_treeGroup.Expand(item,TVE_EXPAND) ;
}
void CDlgAffinityGroupSpec::InsertLeaderItem(HTREEITEM item ,CAffinityPassengerTy* _paxty)
{
	COOLTREE_NODE_INFO info ;
	CCoolTree::InitNodeInfo(this,info);
	info.net = NET_NORMAL ;
	info.nt=NT_NORMAL;
	info.dwItemData = TY_LEADER_NODE ;
	NodeData* pnodedata = new NodeData ;
	pnodedata->nodeTy = TY_LEADER_NODE ;
	m_TreeNodeData.push_back(pnodedata) ;
	HTREEITEM childItem = m_treeGroup.InsertItem(FormatPassengerTyLeaderNodeName(_paxty->IsLeader()),info,TRUE,FALSE,item);
	m_treeGroup.SetItemData(childItem,(DWORD_PTR)pnodedata) ;
}
void CDlgAffinityGroupSpec::InserPacerItem(HTREEITEM item ,CAffinityPassengerTy* _paxty)
{
	COOLTREE_NODE_INFO info ;
	CCoolTree::InitNodeInfo(this,info);
	info.net = NET_NORMAL ;
	info.nt=NT_NORMAL;
	info.dwItemData = TY_PACER_NODE ;
	NodeData* pnodedata = new NodeData ;
	pnodedata->nodeTy = TY_PACER_NODE ;
	m_TreeNodeData.push_back(pnodedata) ;
	HTREEITEM childItem = m_treeGroup.InsertItem(FormatPassengerTyPacerNodeName(_paxty->IsPacer()),info,TRUE,FALSE,item);
	m_treeGroup.SetItemData(childItem,(DWORD_PTR)pnodedata) ;
}
void CDlgAffinityGroupSpec::InitComboString(CComboBox* _box,HTREEITEM Item)
{
   if(_box == NULL || Item == NULL)
	   return ;
   NodeData* nodeTy =(NodeData*) m_treeGroup.GetItemData(Item) ;
   if(nodeTy->nodeTy == TY_COHESION_NODE)
   {
	   _box->AddString("YES") ;
	   _box->AddString("NO") ;
   }
   if(nodeTy->nodeTy == TY_LEADER_NODE || nodeTy->nodeTy == TY_PACER_NODE)
   {
	   _box->AddString("YES");
	   _box->AddString("NO") ;
   }
}
void CDlgAffinityGroupSpec::SetGroupCohesion(CString str ,CAffinityGroup* _group)
{
	int res = 0 ;
	if(strcmp(str,"YES") == 0)
		res = 1 ;
	if(_group == NULL)
		return ;
	_group->IsCohesion(res) ;
	//m_GroupManager->WriteGroupToDB(_group)  ;
}
void CDlgAffinityGroupSpec::SetPassengerTyLeader(CString str , CAffinityPassengerTy* _paxty)
{
	int res = 0 ;
	if(strcmp(str,"YES") == 0)
		res = 1 ;
	if(_paxty == NULL)
		return ;
	_paxty->IsLeader(res) ;
	//_paxty->SaveDataToDB() ;
}
void CDlgAffinityGroupSpec::SetPassengerTyPacer(CString str , CAffinityPassengerTy* _paxty)
{
	int res = 0 ;
	if(strcmp(str,"YES") == 0)
		res = 1 ;
	if(_paxty == NULL)
		return ;
	_paxty->IsPacer(res) ;
}
void CDlgAffinityGroupSpec::HandleComboxChange(CComboBox* _box ,HTREEITEM _item)
{
	if(_box == NULL || _item == NULL)
		return ;
	NodeData* nodeTy =(NodeData*) m_treeGroup.GetItemData(_item) ;
	switch(nodeTy->nodeTy)
	{
	case TY_COHESION_NODE:
		{
			HTREEITEM parent = m_treeGroup.GetParentItem(_item) ;
			//CAffinityGroup* _group = (CAffinityGroup*)m_treeGroup.GetItemData(parent) ;
			NodeData* piNode = (NodeData*)m_treeGroup.GetItemData(parent);
			CAffinityGroup* _group = (CAffinityGroup*)piNode->pData;
            int index = _box->GetCurSel() ;
			CString str ;
			_box->GetLBText(index,str) ;
            SetGroupCohesion(str,_group) ;
			m_treeGroup.SetItemText(_item,FormatGroupCohensionNodeName(_group->IsCohesion())) ;
		}
		break;
	case TY_LEADER_NODE:
		{
           HTREEITEM parent = m_treeGroup.GetParentItem(_item) ;
		   CAffinityPassengerTy* paxty = (CAffinityPassengerTy*)m_treeGroup.GetItemData(parent) ;
		   int index = _box->GetCurSel() ;
		   CString str ;
		   _box->GetLBText(index,str) ;
		   SetPassengerTyLeader(str,paxty) ;
		   m_treeGroup.SetItemText(_item,FormatPassengerTyLeaderNodeName(paxty->IsLeader())) ;   
		}
		break;
	case TY_PACER_NODE:
		{
			HTREEITEM parent = m_treeGroup.GetParentItem(_item) ;
			CAffinityPassengerTy* paxty = (CAffinityPassengerTy*)m_treeGroup.GetItemData(parent) ;
			int index = _box->GetCurSel() ;
			CString str ;
			_box->GetLBText(index,str) ;
			SetPassengerTyPacer(str,paxty) ;
			m_treeGroup.SetItemText(_item,FormatPassengerTyPacerNodeName(paxty->IsPacer())) ;   
		}
	    break;
	default:
	    break;
	}
}
void CDlgAffinityGroupSpec::HandleEditLable(CString _val ,int NodeTy,HTREEITEM _item)
{
  if(_item == NULL)
	  return ;
   switch(NodeTy)
   {
   case TY_OTHER_NODE:
	   {
          HTREEITEM lparent = m_treeGroup.GetParentItem(_item) ;
		  NodeData* node = (NodeData*)m_treeGroup.GetItemData(lparent);
		  CAffinityGroup* _group = (CAffinityGroup*)(node->pData) ;
		  _group->SetGroupsNumber(atoi(_val)) ;
		  m_treeGroup.SetItemText(_item,FormatGroupNumOnAircraftNodeName(_group->GetGroupsNumber())) ;
	   }
   	break;
   case TY_PAXTY_NUMBER_NODE:
	   {
		   HTREEITEM lparent = m_treeGroup.GetParentItem(_item) ;
		   NodeData* node = (NodeData*)m_treeGroup.GetItemData(lparent);
		   CAffinityPassengerTy* _paxTY = (CAffinityPassengerTy*)(node->pData) ;
		   _paxTY->SetNumber(atoi(_val)) ;
		   m_treeGroup.SetItemText(_item,FormatNumberInPaxTyNodeName(_paxTY->GetNumber())) ;
	   }
   	break;
   case TY_GROUP_NODE:
	   {
		   NodeData* node = (NodeData*)m_treeGroup.GetItemData(_item);
		   CAffinityGroup* _group = (CAffinityGroup*)(node->pData) ;
		   _group->SetGroupName(_val) ;
		   m_treeGroup.SetItemText(_item,FormatGroupTreeNodeName(_group->GetGroupName())) ;
	   }
       break;
   case TY_PAXTY_NODE:
	   {
		   NodeData* node = (NodeData*)m_treeGroup.GetItemData(_item);
		   CAffinityPassengerTy*  _paxTY = (CAffinityPassengerTy*)(node->pData) ;
		   _paxTY->SetName(_val) ;
		   m_treeGroup.SetItemText(_item,FormatPaxTYNodeName(_paxTY->GetName())) ;
	   }
       break;
   default:
       break;
   }
}
LRESULT CDlgAffinityGroupSpec::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{

   switch(message)
   {
   case WM_LBUTTONDBLCLK:
	   {
		    HTREEITEM hItem = (HTREEITEM)wParam;
			if(hItem == NULL)
				break ;
			   NodeData* nodeTy = (  NodeData*)m_treeGroup.GetItemData(hItem) ;
			 if(nodeTy != NULL)
			 {
                    if(nodeTy->nodeTy == TY_GROUP_NODE)
						m_treeGroup.EditLabel(hItem) ;
			 }

	   }
	   break;
   case ENDLABELEDIT:
	   OnEndLabelEdit(wParam,lParam);
	   break;
   case BEGINLABELEDIT:
	   OnBeginLabelEdit(wParam,lParam);
	   break;
   case UM_CEW_COMBOBOX_BEGIN:
	   {
		   CWnd* pWnd = m_treeGroup.GetEditWnd((HTREEITEM)wParam);
		   CRect rectWnd;
		   HTREEITEM hItem = (HTREEITEM)wParam;
	
		   m_treeGroup.GetItemRect((HTREEITEM)wParam,rectWnd,TRUE);
		   pWnd->SetWindowPos(NULL,rectWnd.right,rectWnd.top,0,0,SWP_NOSIZE);
		   CComboBox* pCB=(CComboBox*)pWnd;
		   if (pCB->GetCount() != 0)
		   {
			   pCB->ResetContent();
		   }
		   if (pCB->GetCount() == 0)
		   {
			   InitComboString(pCB,hItem) ;
			   pCB->SetCurSel(0);
		   }
	   }
   	break;
   case UM_CEW_COMBOBOX_SELCHANGE:
   case UM_CEW_COMBOBOX_END:
	   {
		   HTREEITEM _item = (HTREEITEM)wParam;
		   NodeData* nodeTy = (  NodeData*)m_treeGroup.GetItemData(_item) ;
		   CWnd* pWnd=m_treeGroup.GetEditWnd(_item);
		   CComboBox* pTCB=(CComboBox*)pWnd;
		   HandleComboxChange(pTCB,_item) ;
	   }
   	break;
   case UM_CEW_SHOW_DIALOGBOX_BEGIN:
	   {
		   HTREEITEM _item = (HTREEITEM)wParam;
		   NodeData* nodeTy = (  NodeData*)m_treeGroup.GetItemData(_item) ;
		   COnBoardPassengerType PaxTY ;
           CDlgNewPassengerType DlgPaxType(&PaxTY,m_terminal,this) ;
		   if(DlgPaxType.DoModal() == IDOK )
               HandleEditLable(PaxTY.PrintStringForShow(),nodeTy->nodeTy,_item) ;
	   }
	   break;
   case UM_CEW_EDITSPIN_END:
   case UM_CEW_EDIT_END:
	   {
		   HTREEITEM _item = (HTREEITEM)wParam;
		   NodeData* nodeTy = (  NodeData*)m_treeGroup.GetItemData(_item) ;
		   CString strValue = *((CString*)lParam);
           HandleEditLable(strValue,nodeTy->nodeTy,_item) ;
	   }
       break;
   case UM_CEW_LABLE_END:
	   {
		     HTREEITEM _item = (HTREEITEM)wParam;
			    NodeData* nodeTy = (  NodeData*)m_treeGroup.GetItemData(_item) ;
            if(nodeTy == NULL)
				break;
		   CString strValue = *((CString*)lParam);
		   HandleEditLable(strValue,nodeTy->nodeTy,_item) ;
	   }
	   break;
   case UM_CEW_SHOWMENU_READY:
	   {
		     HTREEITEM _item = (HTREEITEM)wParam;
			   m_hRClickItem = _item ;
			   NodeData* nodeTy = (  NodeData*)m_treeGroup.GetItemData(_item) ;
			   if(nodeTy->nodeTy != TY_PAXTY_NODE)
				   break ;
			CAffinityPassengerTy* _paxTy = (CAffinityPassengerTy*)nodeTy->pData ; 
		   CMenu* pmenu = (CMenu*)lParam ;
		   if(_paxTy->IsLeader())
		   pmenu->CheckMenuItem(ID_AFFINITY_LEADER,MF_CHECKED) ;
		   else
		   pmenu->CheckMenuItem(ID_AFFINITY_LEADER,MF_UNCHECKED) ;

		   if(_paxTy->IsPacer())
		   pmenu->CheckMenuItem(ID_AFFINITY_PACER,MF_CHECKED) ;
		   else
		   pmenu->CheckMenuItem(ID_AFFINITY_PACER,MF_UNCHECKED) ;
		   pmenu->EnableMenuItem(ID_AFFINITY_LEADER,TRUE) ;
		   pmenu->EnableMenuItem(ID_AFFINITY_PACER,TRUE) ;
	   }
	   break ;
	case UM_CEW_SHOWMENU_END:
		{
			HTREEITEM _item = (HTREEITEM)wParam;
			m_hRClickItem = _item ;
		}
		break ;
   default:
       break;
   }
   return CXTResizeDialog::DefWindowProc(message,wParam,lParam) ;
}
void CDlgAffinityGroupSpec::OnBnClickedButtonSave()
{
	try
	{
		CADODatabase::BeginTransaction() ;
        m_GroupManager->SaveDataToDB() ;
		CADODatabase::CommitTransaction() ;
	}
	catch (CADOException e)
	{
		CADODatabase::RollBackTransation() ;
	}
	
	// TODO: Add your control notification handler code here
}
 void CDlgAffinityGroupSpec::OnTvnSelchangedTree(NMHDR *pNMHDR, LRESULT *pResult)
 {
	 HTREEITEM item = m_treeGroup.GetSelectedItem() ;
	 if(item == NULL)
	 {
		 m_wndToolbar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_NEWGROUP,TRUE) ;
		 m_wndToolbar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_DELGROUP,FALSE) ;
		// m_wndToolbar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_EDITGROUP,FALSE) ;
		 return ;
	 }
	 NodeData* nodedata = (NodeData*)m_treeGroup.GetItemData(item) ;
	 if(nodedata->nodeTy == TY_FLIGHTTYPE_NODE || nodedata->nodeTy == TY_PAXTY_ROOT_NODE)
	 {
		 m_wndToolbar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_NEWGROUP,TRUE) ;
		 m_wndToolbar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_DELGROUP,FALSE) ;
		  return ;
		// m_wndToolbar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_EDITGROUP,FALSE) ;
	 }
	 if(nodedata->nodeTy == TY_PAXTY_NODE || nodedata->nodeTy == TY_GROUP_NODE)
	 {
		 m_wndToolbar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_NEWGROUP,FALSE) ;
		 m_wndToolbar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_DELGROUP,TRUE) ;
		  return ;
	//	 m_wndToolbar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_EDITGROUP,TRUE) ;
	 }else
	 {
		 m_wndToolbar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_NEWGROUP,FALSE) ;
		 m_wndToolbar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_DELGROUP,FALSE) ;
		// m_wndToolbar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_EDITGROUP,FALSE) ;
	 }
*pResult = 0;
 }
 void CDlgAffinityGroupSpec::OnNewToolBar()
 {
	 HTREEITEM item = m_treeGroup.GetSelectedItem() ;
	 if(item == NULL)
	 {
		 OnNewAffinityFlightTY() ;
		 return ;
	 }
	 NodeData* nodedata = (NodeData*)m_treeGroup.GetItemData(item) ;
	 if(nodedata->nodeTy == TY_GROUP_NODE)
	 {
	//	 OnNewAffinityGroup();
		 return ;
	 }
	 if(nodedata->nodeTy == TY_PAXTY_ROOT_NODE)
	 {
		 OnNewAffinityPassengerTy() ;
		 return ;
	 }
	 if(nodedata->nodeTy == TY_FLIGHTTYPE_NODE)
	 {
		 OnNewAffinityGroup() ;
	 }
 }
 void CDlgAffinityGroupSpec::OnDelToolBar()
 {
	 HTREEITEM item = m_treeGroup.GetSelectedItem() ;
	 if(item == NULL)
		 return ;
	 NodeData* nodedata = (NodeData*)m_treeGroup.GetItemData(item) ;
	 if(nodedata->nodeTy == TY_GROUP_NODE)
	 {
		 OnDelAffinityGroup();
		 return ;
	 }
	 if(nodedata->nodeTy == TY_PAXTY_NODE)
	 {
		 OnDelAffinityPassengerTy() ; 
		 return ;
	 }
	 if(nodedata->nodeTy == TY_FLIGHTTYPE_NODE)
		 OnDelAffinityFlightTY() ;
 }
 void CDlgAffinityGroupSpec::OnEditToolBar()
 {
	 HTREEITEM item = m_treeGroup.GetSelectedItem() ;
	 if(item == NULL)
		 return ;
	 NodeData* nodedata = (NodeData*)m_treeGroup.GetItemData(item) ;
	 //if(nodedata->nodeTy == TY_GROUP_NODE)
		// OnEditAffinityGroup();
	 //if(nodedata->nodeTy == TY_PAXTY_ROOT_NODE)
		// OnEditAffinityPassengerTy() ;
 }
 void CDlgAffinityGroupSpec::OnNewAffinityGroup()
 {
	 HTREEITEM item = m_treeGroup.GetSelectedItem() ;
	 NodeData* node = (NodeData*)m_treeGroup.GetItemData(item) ;
	 CAffinityData* affinity= (CAffinityData*)node->pData ;
	 CString flightTY = affinity->m_FliTY ;
    CAffinityGroup* pGroup =m_GroupManager->AddGroup(flightTY) ;
	AddGroupTreeNode(pGroup,item) ;
	m_treeGroup.Expand(item,TVE_EXPAND) ;
//	m_GroupManager->WriteGroupToDB(pGroup) ;
 }
 void CDlgAffinityGroupSpec::OnDelAffinityGroup()
 {
	 HTREEITEM item = m_treeGroup.GetSelectedItem() ;
	 NodeData* _nodeData = (NodeData*)m_treeGroup.GetItemData(item) ;
	 CAffinityGroup* _group = (CAffinityGroup*)_nodeData->pData ;
	 m_GroupManager->DelGroup(_group->GetFlightTY(),_group) ;
	 m_treeGroup.DeleteItem(item);
 }
 void CDlgAffinityGroupSpec::OnNewAffinityPassengerTy()
 {
	 HTREEITEM selItem = m_treeGroup.GetSelectedItem() ;
	 HTREEITEM parent = m_treeGroup.GetParentItem(selItem) ;
	 NodeData* _nodeData = (NodeData*)m_treeGroup.GetItemData(parent) ;
	 ASSERT(_nodeData->nodeTy == TY_GROUP_NODE) ;
	 CAffinityGroup* _group = (CAffinityGroup*)_nodeData->pData ;
	 CAffinityPassengerTy* _paxTy = _group->AddPaxTY() ;
	 AddGroupPaxTyTreeNode(_paxTy,selItem) ;
	// _group->WritePaxTy(_paxTy) ;
 }
 void CDlgAffinityGroupSpec::OnDelAffinityPassengerTy()
 {
	 HTREEITEM selItem = m_treeGroup.GetSelectedItem() ;
	 HTREEITEM parent = m_treeGroup.GetParentItem(selItem) ;
	  parent = m_treeGroup.GetParentItem(parent) ;
	  NodeData* _nodeData = (NodeData*)m_treeGroup.GetItemData(parent) ;
	  ASSERT(_nodeData->nodeTy == TY_GROUP_NODE) ;
      CAffinityGroup* _group = (CAffinityGroup*)_nodeData->pData ;

	  _nodeData = (NodeData*)m_treeGroup.GetItemData(selItem) ;
	 ASSERT(_nodeData->nodeTy == TY_PAXTY_NODE) ;
	 CAffinityPassengerTy* _paxTy = (CAffinityPassengerTy*)_nodeData->pData ;

	 _group->DelPaxTY(_paxTy) ;

	 m_treeGroup.DeleteItem(selItem);
 }
 void CDlgAffinityGroupSpec::OnNewAffinityFlightTY()
 {
	 //HTREEITEM SelItem = m_treeGroup.GetSelectedItem() ;
	 //NodeData* _nodedata = (NodeData*)m_treeGroup.GetItemData(SelItem) ;
	 //ASSERT(_nodedata->nodeTy == TY_FLIGHTTYPE_NODE);
	 FlightConstraint fltType;
	 CFlightDialog flightDlg(NULL);
	 if(IDOK == flightDlg.DoModal())
	 {
		 char szBuffer[1024] = {0};
		 fltType = flightDlg.GetFlightSelection();
		  fltType.screenPrint(szBuffer);
		 if(m_GroupManager->checkFlightTYExist(szBuffer))
		 {
			 MessageBox("The Flight Type has Existed",NULL,MB_OK) ;
			 return ;
		 }
		CAffinityData* affinitydata =  m_GroupManager->FindGroupsManager(szBuffer) ;
		AddFlightTyNode(affinitydata) ;
	 }
 }
 void CDlgAffinityGroupSpec::OnDelAffinityFlightTY()
 {
	 HTREEITEM selItem = m_treeGroup.GetSelectedItem() ;
	 CString text = m_treeGroup.GetItemText(selItem) ;
	 m_GroupManager->DelFlightTy(text) ;
	 m_treeGroup.DeleteItem(selItem);
 }
 void CDlgAffinityGroupSpec::OnContextMenu(CWnd* /*pWnd*/, CPoint point) 
 {
	 CRect rectTree;
	 m_treeGroup.GetWindowRect( &rectTree );
	 if( !rectTree.PtInRect(point) ) 
		 return;

	 m_treeGroup.SetFocus();
	 CPoint pt = point;
	 m_treeGroup.ScreenToClient( &pt );
	 UINT iRet;
	 m_hRClickItem = m_treeGroup.HitTest( pt, &iRet );
	 if (iRet != TVHT_ONITEMLABEL)   // Must click on the label
	 {
		 m_hRClickItem = NULL;
	 }

	 if( m_hRClickItem )
	 {
         NodeData* data = (NodeData*)m_treeGroup.GetItemData(m_hRClickItem) ;
		 if(data->nodeTy != TY_PAXTY_NODE)
			 return ;
		 CAffinityPassengerTy* _paxTy = (CAffinityPassengerTy*)(data->pData) ;
		 CMenu menuPopup;
		 menuPopup.LoadMenu( IDR_MENU_POPUP );
		 CMenu* pmenu = menuPopup.GetSubMenu(94) ;
		/*if(_paxTy->IsLeader())
			pmenu->CheckMenuItem(ID_AFFINITY_LEADER,MF_CHECKED) ;
		else
			pmenu->CheckMenuItem(ID_AFFINITY_LEADER,MF_UNCHECKED) ;

		if(_paxTy->IsPacer())
			pmenu->CheckMenuItem(ID_AFFINITY_PACER,MF_CHECKED) ;
		else
			pmenu->CheckMenuItem(ID_AFFINITY_PACER,MF_UNCHECKED) ;*/
		pmenu->EnableMenuItem(ID_AFFINITY_LEADER,TRUE) ;
		pmenu->EnableMenuItem(ID_AFFINITY_PACER,TRUE) ;
		;//menuPopup.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_RIGHTBUTTON, point.x, point.y, AfxGetMainWnd());
		pmenu->TrackPopupMenu(TPM_LEFTALIGN, point.x, point.y, this);
	 }

 }
 void CDlgAffinityGroupSpec::DelchildItemByType(HTREEITEM item ,int TY)
 {
    HTREEITEM child = m_treeGroup.GetChildItem(item) ;
	while(child)
	{
		NodeData* nodedata =(NodeData*) m_treeGroup.GetItemData(child) ;
		if(nodedata->nodeTy == TY)
		{
			m_treeGroup.DeleteItem(child) ;
			break ;
		}
        child = m_treeGroup.GetNextSiblingItem(child) ;
	}
 }
 void CDlgAffinityGroupSpec::HandleLeader()
 {
	 NodeData* data = (NodeData*)m_treeGroup.GetItemData(m_hRClickItem) ;
	 if(data->nodeTy != TY_PAXTY_NODE)
		 return ;
	 CAffinityPassengerTy* _paxTy = (CAffinityPassengerTy*)(data->pData) ;
	 if(_paxTy->IsLeader())
	 {
		 _paxTy->IsLeader(FALSE) ;
		 DelchildItemByType(m_hRClickItem,TY_LEADER_NODE) ;
	 }else
	 {
		 _paxTy->IsLeader(TRUE) ;
		 InsertLeaderItem(m_hRClickItem,_paxTy) ;
	 }
//	 _paxTy->SaveDataToDB() ;
 }
 void CDlgAffinityGroupSpec::HandlePacer()
 {
	 NodeData* data = (NodeData*)m_treeGroup.GetItemData(m_hRClickItem) ;
	 if(data->nodeTy != TY_PAXTY_NODE)
		 return ;
	 CAffinityPassengerTy* _paxTy = (CAffinityPassengerTy*)(data->pData) ;
	 if(_paxTy->IsPacer())
	 {
		 _paxTy->IsPacer(FALSE) ;
		 DelchildItemByType(m_hRClickItem,TY_PACER_NODE) ;
	 }else
	 {
		 _paxTy->IsPacer(TRUE) ;
		 InserPacerItem(m_hRClickItem,_paxTy) ;
	 }
//	  _paxTy->SaveDataToDB() ;
 }
 void CDlgAffinityGroupSpec::OnOK()
 {
	 try
	 {
		 CADODatabase::BeginTransaction() ;
		 m_GroupManager->SaveDataToDB() ;
		CADODatabase::CommitTransaction() ;

	 }
	 catch (CADOException e)
	 {
		 CADODatabase::RollBackTransation() ;
	 }

   CXTResizeDialog::OnOK() ;
 }
 void CDlgAffinityGroupSpec::OnCancel()
 {
	 CXTResizeDialog::OnCancel() ;
 } LRESULT CDlgAffinityGroupSpec::OnBeginLabelEdit(WPARAM wParam, LPARAM lParam)
 {
	 NMTVDISPINFO* pDispInfo = (NMTVDISPINFO*)wParam;
	 LRESULT* pResult = (LRESULT*)lParam;
	 NodeData* cni = ( NodeData*)m_treeGroup.GetItemData(pDispInfo->item.hItem);
	 if(cni != NULL)
	 {
		 if(cni->nodeTy != TY_GROUP_NODE)
		 {
			 *pResult = TRUE;
			 return FALSE;
		 }
	 }
	 *pResult = TRUE;
	 return FALSE;
 }
 LRESULT CDlgAffinityGroupSpec::OnEndLabelEdit(WPARAM wParam, LPARAM lParam)
 {
	 NMTVDISPINFO* pDispInfo = (NMTVDISPINFO*)wParam;

	 if (pDispInfo->item.pszText == NULL)
	 {
		 return 0;
	 }
	 HTREEITEM hItem = (HTREEITEM)pDispInfo->item.hItem;

	 CString str;
	 m_treeGroup.GetEditControl()->GetWindowText(str);
	 NodeData* data = (NodeData*)m_treeGroup.GetItemData(hItem) ;
	 if(data != NULL)
	 {
		if(data->nodeTy == TY_GROUP_NODE)
		{
			CAffinityGroup* _group = (CAffinityGroup*)data->pData ;
			_group->SetGroupName(str) ;
			m_treeGroup.SetItemText(hItem,FormatGroupTreeNodeName(str)) ;
		}
	 }
	 return 0;
	 //m_treeGroup.SendMessage(DB_CLick)
	 // TODO: Add your control notification handler code here
 }
