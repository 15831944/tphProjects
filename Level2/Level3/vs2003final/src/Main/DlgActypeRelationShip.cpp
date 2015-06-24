// DlgActypeRelationShip.cpp : implementation file
//

#include "stdafx.h"
#include "TermPlan.h"
#include "DlgActypeRelationShip.h"
#include ".\dlgactyperelationship.h"
#include "../Common/ACTypesManager.h"
#include "DlgActypeAliasDefine.h"
// CDlgActypeRelationShip dialog

IMPLEMENT_DYNAMIC(CDlgActypeRelationShip, CXTResizeDialog)
CDlgActypeRelationShip::CDlgActypeRelationShip(CACTypesManager* _ActypeManger ,CWnd* pParent /*=NULL*/)
	: m_ActypeManager(_ActypeManger),CXTResizeDialog(CDlgActypeRelationShip::IDD, pParent)
{
	m_RClickItem = NULL ;
	m_RelationData = _ActypeManger->GetActypeRelationShip() ;
}

CDlgActypeRelationShip::~CDlgActypeRelationShip()
{
}

void CDlgActypeRelationShip::DoDataExchange(CDataExchange* pDX)
{
	CXTResizeDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_TOOLBAR, m_StaticTool);
	DDX_Control(pDX, IDC_TREE_RELATIONSHIP, m_TreeCtrl);
	DDX_Control(pDX, IDC_STATIC_CAPTION, m_Static_Cap);
	DDX_Control(pDX, IDC_STATIC_SPECIAL, m_Static_SPC);

	
}


BEGIN_MESSAGE_MAP(CDlgActypeRelationShip, CXTResizeDialog)
	//ON_NOTIFY(TVN_ENDLABELEDIT, IDC_TREE_RELATIONSHIP, OnTvnEndlabeleditTreeShip)
	ON_WM_SIZE() 
	ON_WM_CREATE() 
	ON_COMMAND(ID_EDIT_ALIASNAME,OnEditAliasName) 
	ON_COMMAND(ID_ADDSUBRELATION,OnAddSubNode) 
	ON_COMMAND(ID_REMOVE,OnRemove)
	ON_COMMAND(ID_ACTYPE_RELATION_ADD,OnAddRelation) 
	ON_COMMAND(ID_ACTYPE_RELATION_DEL,OnDelRelation) 
	ON_COMMAND(ID_ACTYPE_RELATION_EDIT,OnEditRelation)
    ON_WM_CTLCOLOR()
	ON_NOTIFY(TVN_SELCHANGED,IDC_TREE_RELATIONSHIP,OnTvnChangedTreeShip)
#ifdef _DEBUG
	ON_COMMAND(IDC_BUTTON_FIND,onfind)
#endif
END_MESSAGE_MAP()
#ifdef _DEBUG
void CDlgActypeRelationShip::onfind()
{
	CString actype ;
    GetDlgItem(IDC_EDIT_ACTYPE)->GetWindowText(actype) ;
	CACType* Pactype =  m_ActypeManager->getACTypeItem(actype) ;
	if(Pactype != NULL)
       GetDlgItem(IDC_EDIT_RES)->SetWindowText(Pactype->GetIATACode()) ;
	else
        GetDlgItem(IDC_EDIT_RES)->SetWindowText(_T("not find")) ;
}
#endif
void CDlgActypeRelationShip::OnTvnChangedTreeShip(NMHDR *pNMHDR, LRESULT *pResult)
{
	HTREEITEM _item =  m_TreeCtrl.GetSelectedItem() ;
	if(_item != NULL)
	{
		m_ToolBar.GetToolBarCtrl().EnableButton(ID_ACTYPE_RELATION_DEL,TRUE) ;
		m_ToolBar.GetToolBarCtrl().EnableButton(ID_ACTYPE_RELATION_EDIT,TRUE) ;
	}else
	{
		m_ToolBar.GetToolBarCtrl().EnableButton(ID_ACTYPE_RELATION_DEL,FALSE) ;
		m_ToolBar.GetToolBarCtrl().EnableButton(ID_ACTYPE_RELATION_EDIT,FALSE) ;
	}
}
void CDlgActypeRelationShip::OnAddRelation()
{
	HTREEITEM _item = m_TreeCtrl.GetSelectedItem() ;
	SuperTypeRelationData::P_TREE_NODE nodeData = NULL ;
	if(_item == NULL)
		nodeData = m_RelationData->GetRootData() ;
	else
	    nodeData = (  SuperTypeRelationData::P_TREE_NODE )m_TreeCtrl.GetItemData(_item) ;
	CAcTypeAlias* ActypeAlias = new CAcTypeAlias ;
	CDlgActypeAliasDefine dlgAliasName(m_ActypeManager, ActypeAlias,this) ;
	if(dlgAliasName.DoModal() == IDOK)
	{
		SuperTypeRelationData::P_TREE_NODE NodeData = m_RelationData->AddTreeNode(nodeData,ActypeAlias) ;
		AddTreeItem(NodeData,_item) ;
	}else
		delete ActypeAlias ;
}
void CDlgActypeRelationShip::OnDelRelation()
{
	HTREEITEM _item = m_TreeCtrl.GetSelectedItem() ;
	if(_item == NULL)
		return;
	SuperTypeRelationData::P_TREE_NODE nodeData = (  SuperTypeRelationData::P_TREE_NODE )m_TreeCtrl.GetItemData(_item) ;
	m_RelationData->RemoveTreeNode(nodeData) ;
	m_TreeCtrl.DeleteItem(_item) ;
}
void CDlgActypeRelationShip::OnEditRelation()
{
	HTREEITEM _item = m_TreeCtrl.GetSelectedItem() ;
	if(_item == NULL)
		return;
	SuperTypeRelationData::P_TREE_NODE nodeData = (  SuperTypeRelationData::P_TREE_NODE )m_TreeCtrl.GetItemData(_item) ;
	CDlgActypeAliasDefine dlgAliasName(m_ActypeManager,nodeData->m_NodeData,this) ;
	if(dlgAliasName.DoModal() == IDOK)
	{
		m_TreeCtrl.SetItemText(_item,nodeData->m_NodeData->FormatAliasName()) ;
	}
}

void CDlgActypeRelationShip::OnEditAliasName()
{
  if(m_RClickItem == NULL)
	  return ;
  SuperTypeRelationData::P_TREE_NODE nodeData = (  SuperTypeRelationData::P_TREE_NODE )m_TreeCtrl.GetItemData(m_RClickItem) ;
  if(nodeData == NULL)
	  return ;
  CDlgActypeAliasDefine dlgAliasName(m_ActypeManager,nodeData->m_NodeData,this) ;
  if(dlgAliasName.DoModal() == IDOK)
  {
	  m_TreeCtrl.SetItemText(m_RClickItem,nodeData->m_NodeData->FormatAliasName()) ;
  }

}
void CDlgActypeRelationShip::OnAddSubNode()
{
   if(m_RClickItem == NULL)
	   return ;
  SuperTypeRelationData::P_TREE_NODE nodeData = (  SuperTypeRelationData::P_TREE_NODE )m_TreeCtrl.GetItemData(m_RClickItem) ;
  CAcTypeAlias* ActypeAlias = new CAcTypeAlias ;
  CDlgActypeAliasDefine dlgAliasName(m_ActypeManager,ActypeAlias,this) ;
  if(dlgAliasName.DoModal() == IDOK)
  {
	   SuperTypeRelationData::P_TREE_NODE NodeData = m_RelationData->AddTreeNode(nodeData,ActypeAlias) ;
	   AddTreeItem(NodeData,m_RClickItem) ;
  }else
	  delete ActypeAlias ;
}
void CDlgActypeRelationShip::OnRemove()
{
	if(m_RClickItem == NULL)
		return ;
	 SuperTypeRelationData::P_TREE_NODE nodeData = (  SuperTypeRelationData::P_TREE_NODE )m_TreeCtrl.GetItemData(m_RClickItem) ;
	 m_RelationData->RemoveTreeNode(nodeData) ;
	 m_TreeCtrl.DeleteItem(m_RClickItem) ;
}

// CDlgActypeRelationShip message handlers
void CDlgActypeRelationShip::OnSize(UINT nType, int cx, int cy)
{
	CXTResizeDialog::OnSize(nType,cx,cy) ;
}
int CDlgActypeRelationShip::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if( CXTResizeDialog::OnCreate(lpCreateStruct)== -1)
		return -1 ;
	if (!m_ToolBar.CreateEx(this,  TBSTYLE_FLAT,WS_CHILD|WS_VISIBLE|CBRS_ALIGN_TOP ) ||
		!m_ToolBar.LoadToolBar(IDR_ACTYPE_RELATION))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}
	return 0;
}

HBRUSH CDlgActypeRelationShip::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	//switch(pWnd->GetDlgCtrlID())
	//{
	//case IDC_STATIC_CAPTION:
	//	//LOGFONT lgf ;
	//	//m_Static_Cap.GetFont()->GetLogFont(&lgf) ;
	//	//CFont* pFont= new CFont ;
	//	//lgf.lfWeight = 600 ;

	//	//pFont->CreateFontIndirect(&lgf) ;
	//	//m_Static_Cap.SetFont(pFont,TRUE) ;

	//	//CFont* def_font = pDC->SelectObject(pFont);
	//	//CRect rect ;
	//	//m_Static_Cap.GetWindowRect(&rect) ;
	//	//ScreenToClient(&rect);

	////	pDC->TextOut(5,5, "Actype Relationship");
	//	//pDC->SelectObject(def_font);
	//	// m_Static_Cap.SetWindowText(_T("Actype RelationShip"));
	//	// pFont->DeleteObject() ;
	//	 //delete pFont ;
	//}
	return CXTResizeDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	// TODO: Return a different brush if the default is not desired
}

void CDlgActypeRelationShip::OnInitToolBar()
{
	CRect rect ;
	m_StaticTool.GetWindowRect(&rect) ;
	ScreenToClient(&rect);
	m_ToolBar.MoveWindow(&rect) ;

	m_ToolBar.GetToolBarCtrl().EnableButton(ID_ACTYPE_RELATION_ADD,TRUE) ;
	m_ToolBar.GetToolBarCtrl().EnableButton(ID_ACTYPE_RELATION_DEL,FALSE) ;
	m_ToolBar.GetToolBarCtrl().EnableButton(ID_ACTYPE_RELATION_EDIT,FALSE) ;

	m_StaticTool.ShowWindow(SW_HIDE) ;
	m_ToolBar.ShowWindow(SW_SHOW) ;

}
BOOL CDlgActypeRelationShip::OnInitDialog()
{
	CXTResizeDialog::OnInitDialog() ;
	OnInitToolBar() ;
	InitTreeCtrl() ;
#ifdef _DEBUG
	GetDlgItem(IDC_EDIT_ACTYPE)->EnableWindow(TRUE) ;
	GetDlgItem(IDC_EDIT_ACTYPE)->ShowWindow(SW_SHOW) ;
	GetDlgItem(IDC_EDIT_RES)->EnableWindow(TRUE) ;
	GetDlgItem(IDC_EDIT_RES)->ShowWindow(SW_SHOW) ;
	GetDlgItem(IDC_BUTTON_FIND)->EnableWindow(TRUE) ;
	GetDlgItem(IDC_BUTTON_FIND)->ShowWindow(SW_SHOW) ;
#else
	GetDlgItem(IDC_EDIT_ACTYPE)->ShowWindow(SW_HIDE) ;
	GetDlgItem(IDC_EDIT_RES)->ShowWindow(SW_HIDE) ;
	GetDlgItem(IDC_BUTTON_FIND)->ShowWindow(SW_HIDE) ;
#endif
	
	
	SetResize(IDC_TREE_RELATIONSHIP,SZ_TOP_LEFT,SZ_MIDDLE_RIGHT) ;
	SetResize(IDC_STATIC_SHIP,SZ_MIDDLE_LEFT,SZ_BOTTOM_RIGHT) ;
	SetResize(IDC_STATIC_CAPTION,SZ_MIDDLE_LEFT,SZ_MIDDLE_LEFT) ;
	SetResize(IDC_STATIC_SPECIAL,SZ_MIDDLE_LEFT,SZ_BOTTOM_RIGHT) ;
	SetResize(IDOK,SZ_BOTTOM_RIGHT,SZ_BOTTOM_RIGHT) ;
	SetResize(IDCANCEL,SZ_BOTTOM_RIGHT,SZ_BOTTOM_RIGHT) ;
    SetResize(m_ToolBar.GetDlgCtrlID(),SZ_TOP_LEFT,SZ_TOP_LEFT) ;
#ifdef _DEBUG
	SetResize(IDC_EDIT_ACTYPE,SZ_BOTTOM_LEFT,SZ_BOTTOM_LEFT) ;
	SetResize(IDC_BUTTON_FIND,SZ_BOTTOM_RIGHT,SZ_BOTTOM_RIGHT) ;
	SetResize(IDC_EDIT_RES,SZ_BOTTOM_RIGHT,SZ_BOTTOM_RIGHT) ;
#endif
	return TRUE ;
}
void CDlgActypeRelationShip::InitTreeCtrl()
{
	if(m_RelationData == NULL)
		return ;
	for (int i =0 ; i < (int)m_RelationData->GetRootData()->m_children.size() ; i++)
	{
		AddTreeItem(m_RelationData->GetRootData()->m_children[i],NULL) ;
	}
}
void CDlgActypeRelationShip::AddTreeItem( SuperTypeRelationData::P_TREE_NODE _data ,HTREEITEM _AfterItem)
{
	if(_data == NULL)
		return ;
	COOLTREE_NODE_INFO info ;
	CCoolTree::InitNodeInfo(this,info) ;
	info.nt=NT_NORMAL;
	info.net = NET_LABLE;
	//info.bAutoSetItemText = FALSE ;
	info.unMenuID = IDR_MENU_ACTYPE_RELATION ;
	
	HTREEITEM Item = NULL ;
	if(_AfterItem != NULL) 
		Item = m_TreeCtrl.InsertItem(_data->m_NodeData->FormatAliasName(),info,TRUE,FALSE,_AfterItem) ;
	else 
		Item = m_TreeCtrl.InsertItem(_data->m_NodeData->FormatAliasName(),info,TRUE,FALSE) ;
	m_TreeCtrl.SetItemData(Item,(DWORD_PTR)_data) ;
	for (int i =0 ; i < (int)_data->m_children.size() ; i++)
	{
		AddTreeItem(_data->m_children[i],Item) ;
	}
	m_TreeCtrl.Expand(_AfterItem,TVE_EXPAND) ;
}
LRESULT CDlgActypeRelationShip::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
	   case UM_CEW_LABLE_END:
		   {
			   HTREEITEM _item = (HTREEITEM)wParam;
	
			   CString strValue = *((CString*)lParam);
			   HandleEditLable(strValue,_item) ;
		   }
		   break;
	   case UM_CEW_SHOWMENU_BEGIN:
		   {
			   m_RClickItem = (HTREEITEM)wParam;
		   }
		   break ;
	}
	return CXTResizeDialog::DefWindowProc(message,wParam,lParam) ;
}
void CDlgActypeRelationShip::HandleEditLable(CString& _val ,HTREEITEM _item) 
{
	SuperTypeRelationData::P_TREE_NODE nodeData = (  SuperTypeRelationData::P_TREE_NODE )m_TreeCtrl.GetItemData(_item) ;
	if(nodeData == NULL || _val.IsEmpty())
		return;
	CString ErrorMessage ;
	if(!m_RelationData->CheckTheAliasNameValid(nodeData->m_NodeData,_val,ErrorMessage))
	{
		MessageBox(ErrorMessage,_T("Warning"),MB_OK) ;
		return ;
	}
	//add alias name to Node Data 
	nodeData->m_NodeData->AddAliasName(_val) ;
    //Update tree node 
	m_TreeCtrl.SetItemText(_item,nodeData->m_NodeData->FormatAliasName()) ;
}
void CDlgActypeRelationShip::OnTvnEndlabeleditTreeShip(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTVDISPINFO pTVDispInfo = reinterpret_cast<LPNMTVDISPINFO>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
}
void CDlgActypeRelationShip::OnOK()
{
	try
	{
		CADODatabase::BeginTransaction() ;
	if(m_RelationData != NULL)
			m_RelationData->WriteDataToDB() ;
		CADODatabase::CommitTransaction() ;
	}
	catch (CADOException& e)
	{
		e.ErrorMessage() ;
		CADODatabase::RollBackTransation() ;
	}

   CXTResizeDialog::OnOK() ;
}
void CDlgActypeRelationShip::OnCancel()
{
	if(m_RelationData != NULL)
		m_RelationData->ReadDataFromDB() ;
  CXTResizeDialog::OnCancel() ;
}