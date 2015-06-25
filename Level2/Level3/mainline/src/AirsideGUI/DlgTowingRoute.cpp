// DlgTowingRoute.cpp : implementation file
//

#include "stdafx.h"
//#include "AirsideGUI.h"
#include "Resource.h"
#include "DlgTowingRoute.h"
#include ".\dlgtowingroute.h"
#include "..\InputAirside\ALTObjectGroup.h"
#include "..\InputAirside\InputAirside.h"
#include "..\InputAirside\ALTAirport.h"
#include "..\InputAirside\IntersectionNode.h"
#include "DlgTowingRouteFromAndTo.h"
#include "InputAirside/AirportGroundNetwork.h"
#include <set>

// CDlgTowingRoute dialog
static const UINT ID_NEW_FROMTO = 10;
static const UINT ID_DEL_FROMTO = 11;
static const UINT ID_EDIT_FROMTO = 12;

static const UINT ID_NEW_TAXIWAYNODE = 20;
static const UINT ID_DEL_TAXIWAYNODE = 21;

IMPLEMENT_DYNAMIC(CDlgTowingRoute, CXTResizeDialog)
CDlgTowingRoute::CDlgTowingRoute(int nProjID, CWnd* pParent /*=NULL*/)
	: CXTResizeDialog(CDlgTowingRoute::IDD, pParent)
	, m_nProjID(nProjID)
	,m_pCurRoute(NULL)
{
	m_pAltNetwork = new CAirportGroundNetwork(nProjID);
}

CDlgTowingRoute::~CDlgTowingRoute()
{
	delete m_pAltNetwork;

	for (size_t i = 0 ; i < m_vectRouteNode.size(); i++)
		delete m_vectRouteNode.at(i);

}

void CDlgTowingRoute::DoDataExchange(CDataExchange* pDX)
{
	CXTResizeDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_ROUTENAME, m_wndListBox);
	DDX_Control(pDX, IDC_TREE_TOWINGROUTE, m_wndTree);
}


Taxiway* CDlgTowingRoute::GetTaxiwayByID(int nID)
{
	size_t nCount = m_vTaxiway.size();
	for(size_t i =0; i < nCount; i++)
	{
		if ( m_vTaxiway.at(i).getID() == nID)
			return &m_vTaxiway.at(i);
	}
	return NULL;
}

BEGIN_MESSAGE_MAP(CDlgTowingRoute, CXTResizeDialog)
	ON_WM_CREATE()
	ON_BN_CLICKED(IDC_BUTTON_SAVE, OnBnClickedButtonSave)
	ON_COMMAND(ID_NEW_FROMTO, OnNewRoute)
	ON_COMMAND(ID_DEL_FROMTO, OnDelRoute)
	ON_COMMAND(ID_EDIT_FROMTO,OnEditRoute)
	ON_COMMAND(ID_NEW_TAXIWAYNODE, OnNewTaxiwayNode)
	ON_COMMAND(ID_DEL_TAXIWAYNODE, OnDelTaxiwayNode)
	ON_LBN_SELCHANGE( IDC_LIST_ROUTENAME, OnLvnItemchangedListTowingroute)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_TOWINGROUTE, OnTvnSelchangedTreeTowingroute)
	ON_MESSAGE(WM_LBITEMCHANGED,OnMessageModifyRouteName)
	ON_WM_CONTEXTMENU()
END_MESSAGE_MAP()


// CDlgTowingRoute message handlers

int CDlgTowingRoute::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CXTResizeDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here
	if (!m_wndRoutebar.CreateEx(this, TBSTYLE_FLAT,WS_CHILD|WS_VISIBLE|CBRS_ALIGN_TOP)
		|| !m_wndRoutebar.LoadToolBar(IDR_TOOLBAR_ADD_DEL_EDIT))
	{
		return -1;
	}

	if (!m_wndTaxiwayNodeToolbar.CreateEx(this,TBSTYLE_FLAT,WS_CHILD|WS_VISIBLE|CBRS_ALIGN_TOP,CRect(0,0,0,0),m_wndRoutebar.GetDlgCtrlID()+1)
		|| !m_wndTaxiwayNodeToolbar.LoadToolBar(IDR_TOOLBAR_ADD_DEL))
	{
		return -1;
	}

	CToolBarCtrl& fromToToolbarCtrl = m_wndRoutebar.GetToolBarCtrl();
	fromToToolbarCtrl.SetCmdID(0, ID_NEW_FROMTO);
	fromToToolbarCtrl.SetCmdID(1, ID_DEL_FROMTO);
	fromToToolbarCtrl.SetCmdID(2, ID_EDIT_FROMTO);

	CToolBarCtrl& taxiwayNodeToolbarCtrl = m_wndTaxiwayNodeToolbar.GetToolBarCtrl();
	taxiwayNodeToolbarCtrl.SetCmdID(0, ID_NEW_TAXIWAYNODE);
	taxiwayNodeToolbarCtrl.SetCmdID(1, ID_DEL_TAXIWAYNODE);

	return 0;
}

void CDlgTowingRoute::InitToolBar()
{
	CRect rect;
	GetDlgItem(IDC_STATIC_NAME)->GetWindowRect(&rect);
	GetDlgItem(IDC_STATIC_NAME)->ShowWindow(FALSE);
	ScreenToClient(&rect);
	m_wndRoutebar.MoveWindow(&rect,true);
	m_wndRoutebar.ShowWindow(SW_SHOW);

	m_wndRoutebar.GetToolBarCtrl().EnableButton(ID_NEW_FROMTO);
	m_wndRoutebar.GetToolBarCtrl().EnableButton(ID_DEL_FROMTO,FALSE);
	m_wndRoutebar.GetToolBarCtrl().EnableButton(ID_EDIT_FROMTO,FALSE);


	GetDlgItem(IDC_STATIC_ROUTE)->GetWindowRect(&rect);
	GetDlgItem(IDC_STATIC_ROUTE)->ShowWindow(FALSE);
	ScreenToClient(&rect);
	m_wndTaxiwayNodeToolbar.MoveWindow(&rect,true);
	m_wndTaxiwayNodeToolbar.ShowWindow(SW_SHOW);

	m_wndTaxiwayNodeToolbar.GetToolBarCtrl().EnableButton(ID_NEW_TAXIWAYNODE,FALSE);
	m_wndTaxiwayNodeToolbar.GetToolBarCtrl().EnableButton(ID_DEL_TAXIWAYNODE,FALSE);
}
void CDlgTowingRoute::InitListBox()
{
	m_wndListBox.ResetContent();

	int nCount = m_towingRouteList.GetTowingRouteCount();
	for (int i=0; i<nCount; i++)
	{
		CTowingRoute* pRoute = m_towingRouteList.GetTowingRouteItem(i);

		int nItem = m_wndListBox.AddString(pRoute->GetName());
		m_wndListBox.SetItemData(nItem, (DWORD_PTR)pRoute);
	}

}
BOOL CDlgTowingRoute::OnInitDialog()
{
	CXTResizeDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	InitToolBar();
	m_towingRouteList.ReadData();

	InitListBox();
	GetAllTaxiway();
	SetResize(m_wndRoutebar.GetDlgCtrlID(), SZ_TOP_LEFT, SZ_TOP_LEFT);
	SetResize(m_wndTaxiwayNodeToolbar.GetDlgCtrlID(), SZ_TOP_CENTER, SZ_TOP_CENTER);

	SetResize(IDC_STATIC_NAMETEXT,  SZ_TOP_LEFT, SZ_BOTTOM_CENTER);
	SetResize(IDC_LIST_ROUTENAME, SZ_TOP_LEFT, SZ_BOTTOM_CENTER);

	SetResize(IDC_STATIC_TEXT,  SZ_TOP_CENTER, SZ_BOTTOM_RIGHT);
	SetResize(IDC_TREE_TOWINGROUTE, SZ_TOP_CENTER, SZ_BOTTOM_RIGHT);

	SetResize(IDC_BUTTON_SAVE, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);
	SetResize(IDOK, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);
	SetResize(IDCANCEL, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);

	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),TRUE);
	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSErf
}

void CDlgTowingRoute::OnOK()
{
	// TODO: Add your specialized code here and/or call the base class
	try
	{
		CADODatabase::BeginTransaction() ;
		m_towingRouteList.SaveData();
		CADODatabase::CommitTransaction();

	}
	catch (CADOException e)
	{
		CADODatabase::RollBackTransation() ;
	}
	
	CXTResizeDialog::OnOK();
}

void CDlgTowingRoute::OnBnClickedButtonSave()
{
	// TODO: Add your control notification handler code here
	try
	{
		CADODatabase::BeginTransaction() ;
		m_towingRouteList.SaveData();
		CADODatabase::CommitTransaction();

	}
	catch (CADOException e)
	{
		CADODatabase::RollBackTransation() ;
	}
	CXTResizeDialog::OnOK();
}

void CDlgTowingRoute::UpdateToolBar()
{
	if (m_wndListBox.GetCurSel() != -1)
	{
		m_wndRoutebar.GetToolBarCtrl().EnableButton(ID_DEL_FROMTO, TRUE);
		m_wndRoutebar.GetToolBarCtrl().EnableButton(ID_EDIT_FROMTO, TRUE);
		m_wndTaxiwayNodeToolbar.GetToolBarCtrl().EnableButton(ID_NEW_TAXIWAYNODE, TRUE);

		HTREEITEM hSelTreeItem = m_wndTree.GetSelectedItem();
		if (hSelTreeItem && m_wndTree.GetParentItem(hSelTreeItem))
			m_wndTaxiwayNodeToolbar.GetToolBarCtrl().EnableButton(ID_DEL_TAXIWAYNODE, TRUE);
		else
			m_wndTaxiwayNodeToolbar.GetToolBarCtrl().EnableButton(ID_DEL_TAXIWAYNODE, FALSE);
	}
	else
	{
		m_wndRoutebar.GetToolBarCtrl().EnableButton(ID_DEL_FROMTO, FALSE);
		m_wndRoutebar.GetToolBarCtrl().EnableButton(ID_EDIT_FROMTO, FALSE);
		m_wndTaxiwayNodeToolbar.GetToolBarCtrl().EnableButton(ID_NEW_TAXIWAYNODE, FALSE);
		m_wndTaxiwayNodeToolbar.GetToolBarCtrl().EnableButton(ID_DEL_TAXIWAYNODE, FALSE);
	}
}

void CDlgTowingRoute::OnLvnItemchangedListTowingroute()
{
	// TODO: Add your control notification handler code here
	int nCurSel = m_wndListBox.GetCurSel();
	m_pCurRoute = (CTowingRoute*)m_wndListBox.GetItemData(nCurSel);
	LoadTree();
	UpdateToolBar();
}

void CDlgTowingRoute::OnNewRoute()
{

	CTowingRoute* pNewItem = new CTowingRoute;
	int nCount = m_wndListBox.GetCount();
	CString strName;
	strName.Format("Towing Route %d", nCount + 1);
	pNewItem->SetName(strName);
	m_towingRouteList.AddTowingRouteItem(pNewItem);

	int nItem = m_wndListBox.AddString(strName);
	m_wndListBox.SetCurSel(nItem);
	m_wndListBox.SetItemData(nItem, (DWORD_PTR)pNewItem);

	m_pCurRoute = pNewItem;
	m_wndTree.DeleteAllItems();
	UpdateToolBar();
	m_wndListBox.EditItem(nItem);
}
void CDlgTowingRoute::OnDelRoute()
{
	if (m_wndListBox.GetCount() == 0)
		return;

	int nSel = m_wndListBox.GetCurSel();
	if (nSel == -1)
		return;

	CTowingRoute* pRouteItem = (CTowingRoute*)m_wndListBox.GetItemData(nSel);	
	m_towingRouteList.DelTowingRouteItem(pRouteItem);
	m_wndListBox.DeleteString(nSel);
	int nCount = m_wndListBox.GetCount();
	
	if(nCount >= 1)
	{
		if(nSel != 0)
			m_wndListBox.SetCurSel(nSel-1);	
		else
			m_wndListBox.SetCurSel(0);	
		LoadTree();
	}
	else
		m_wndTree.DeleteAllItems();

}

void CDlgTowingRoute::OnEditRoute()
{
	m_wndTaxiwayNodeToolbar.GetToolBarCtrl().EnableButton(ID_NEW_TAXIWAYNODE, FALSE);

	int nSel = m_wndListBox.GetCurSel();
	m_wndListBox.EditItem(nSel);
}

void CDlgTowingRoute::OnNewTaxiwayNode()
{
	int nSel = m_wndListBox.GetCurSel();
	if (nSel == -1)
		return;
	CTowingRoute* pRouteItem = (CTowingRoute*)m_wndListBox.GetItemData(nSel);
	int nMaxUniID = pRouteItem->GetMaxUniID();
	pRouteItem->SetMaxUniID(++nMaxUniID);	
	CTaxiwayNode* pNewItem = new CTaxiwayNode;
	pNewItem->SetUniqueID(nMaxUniID);
	
	COOLTREE_NODE_INFO cni;
	CCoolTree::InitNodeInfo(this,cni);
	cni.nt=NT_NORMAL;
	cni.net=NET_COMBOBOX;
	cni.bAutoSetItemText= false;
	CString strObjMark = "Please Select";

	HTREEITEM hRootTreeItem = m_wndTree.GetRootItem();
	HTREEITEM hSelTreeItem = m_wndTree.GetSelectedItem();
	HTREEITEM hTaxiwayNodeTreeItem;

	if(hSelTreeItem && hSelTreeItem != hRootTreeItem)
	{
		CTaxiwayNode* pParentItem = (CTaxiwayNode*)m_wndTree.GetItemData(hSelTreeItem);
		int nParentTaxiwayID = pParentItem->GetTaxiwayID();
		std::vector<IntersectionNode> vNodeList;
		vNodeList.clear();
		IntersectionNode::ReadIntersecNodeListWithObjType(nParentTaxiwayID, ALT_TAXIWAY, vNodeList);
		if((int)vNodeList.size())
		{
			m_wndTree.SetRedraw(FALSE);
			hTaxiwayNodeTreeItem = m_wndTree.InsertItem(strObjMark,cni,FALSE,FALSE,hSelTreeItem);
			m_wndTree.SetRedraw(TRUE);
			m_wndTree.Expand(hSelTreeItem,TVE_EXPAND);
			pNewItem->SetParentID(pParentItem->GetUniqueID());
			m_wndTree.SetItemData(hTaxiwayNodeTreeItem,(DWORD_PTR)pNewItem);
			pRouteItem->AddTaxiwayNodeItem(pNewItem);
			m_wndTree.DoEdit(hTaxiwayNodeTreeItem);
		}
		else
			AfxMessageBox("Sorry,There is no Taxiway.");
	}
	else
	{
		if((int)m_vTaxiway.size())
		{
			m_wndTree.SetRedraw(FALSE);
			hTaxiwayNodeTreeItem = m_wndTree.InsertItem(strObjMark,cni,FALSE,FALSE,hRootTreeItem);
			m_wndTree.SetRedraw(TRUE);
			m_wndTree.Expand(hRootTreeItem,TVE_EXPAND);
			pNewItem->SetParentID(-1);
			m_wndTree.SetItemData(hTaxiwayNodeTreeItem,(DWORD_PTR)pNewItem);
			pRouteItem->AddTaxiwayNodeItem(pNewItem);
			m_wndTree.DoEdit(hTaxiwayNodeTreeItem);
		}
		else
			AfxMessageBox("Sorry,There is no Taxiway.");
	}
}
void CDlgTowingRoute::OnDelTaxiwayNode()
{
	int nSel = m_wndListBox.GetCurSel();
	if (nSel == -1)
		return;
	CTowingRoute* pRouteItem = (CTowingRoute*)m_wndListBox.GetItemData(nSel);

	HTREEITEM hSelTreeItem = m_wndTree.GetSelectedItem();
	if(hSelTreeItem)
	{
		CTaxiwayNode* pItem = (CTaxiwayNode*)m_wndTree.GetItemData(hSelTreeItem);
		pRouteItem->DelFromvTaxiwayNode(pItem);
		m_wndTree.DeleteItem(hSelTreeItem);
	}
}
void CDlgTowingRoute::OnTvnSelchangedTreeTowingroute(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	UpdateToolBar();
	*pResult = 0;
}

void CDlgTowingRoute::GetAllTaxiway()
{
	std::vector<int> vAirportIds;
	InputAirside::GetAirportList(m_nProjID, vAirportIds);
	for (std::vector<int>::iterator iterAirportID = vAirportIds.begin(); iterAirportID != vAirportIds.end(); ++iterAirportID)
	{
	//	ALTAirport airport;
	//	airport.ReadAirport(*iterAirportID);
		std::vector<int> vTaxiwaysIDs;
		ALTAirport::GetTaxiwaysIDs(*iterAirportID, vTaxiwaysIDs);
		std::vector<int>::iterator iterTaxiwayID = vTaxiwaysIDs.begin();
		for (; iterTaxiwayID != vTaxiwaysIDs.end(); ++iterTaxiwayID)
		{
			Taxiway taxiway;
			taxiway.ReadObject(*iterTaxiwayID);
			m_vTaxiway.push_back(taxiway);
		}
	}
}



void CDlgTowingRoute::LoadTree()
{
	m_wndTree.DeleteAllItems();
	
	COOLTREE_NODE_INFO cni;
	CCoolTree::InitNodeInfo(this,cni);
	HTREEITEM hRootTreeItem = m_wndTree.InsertItem(_T("Start"),cni,FALSE);

	int nSel = m_wndListBox.GetCurSel();
	if (nSel == -1)
		return;
	CTowingRoute* pRouteItem = (CTowingRoute*)m_wndListBox.GetItemData(nSel);
	for(int i=0;i<pRouteItem->GetTaxiwayNodeCount();i++)
	{
		CTaxiwayNode* pItem = pRouteItem->GetTaxiwayNodeItem(i);
		int nTaxiwayID = pItem->GetTaxiwayID();
	
		Taxiway* pTaxiway = GetTaxiwayByID( nTaxiwayID );
		if( pTaxiway == NULL )
			return;

		//std::vector<Taxiway>::iterator iter = m_vTaxiway.begin();
		//for (; iter != m_vTaxiway.end(); ++iter)	
		//{
		//	if((*iter).getID() == nTaxiwayID)
		//		break;
		//}
		cni.nt=NT_NORMAL;
		cni.net=NET_COMBOBOX;
		cni.bAutoSetItemText= false;
		HTREEITEM hTaxiwayNodeTreeItem;
		CString strObjMark;
		if(pItem->GetParentID() != -1)
		{
			// Get Parent Taxiway,
			Taxiway* pParentTaxiway = NULL;
			for(int j=0;j<pRouteItem->GetTaxiwayNodeCount();j++)
			{
				CTaxiwayNode* pItemOther = pRouteItem->GetTaxiwayNodeItem(j);
				if(pItemOther->GetUniqueID() == pItem->GetParentID())
				{
					pParentTaxiway = GetTaxiwayByID(pItemOther->GetTaxiwayID());
					break;
				}
			}

			//Taxiway* pParentTaxiway = GetTaxiwayByID( pItem->GetParentID() );
			if( pParentTaxiway == NULL )
				return;

			IntersectionNodeList twINodeList;
			m_pAltNetwork->GetTaxiwayTaxiwayIntersectNodeList(pParentTaxiway,
					pTaxiway , twINodeList);

			ASSERT( twINodeList.size() > 0);
			if(twINodeList.size() > 1)
			{
				strObjMark.Format("%s(%s&%d)",pTaxiway->GetMarking().c_str(),
					pParentTaxiway->GetMarking().c_str(),pItem->GetIdx());

			}
			else{
				strObjMark.Format("%s",pTaxiway->GetMarking().c_str());
			}

			//int idx = pItem->GetIdx();
			//strObjMark.Format("Taxiway : %s&%d",(*iter).GetMarking().c_str(),idx+1);
			HTREEITEM hDataTreeItem = m_wndTree.GetChildItem(hRootTreeItem);
			HTREEITEM hTreeItem = NULL;
			TraverseAll(hDataTreeItem,pItem->GetParentID(),hTreeItem);
			m_wndTree.SetRedraw(FALSE);
			hTaxiwayNodeTreeItem = m_wndTree.InsertItem(strObjMark,cni,FALSE,FALSE,hTreeItem);	
			m_wndTree.SetRedraw(TRUE);
			m_wndTree.Expand(hTreeItem,TVE_EXPAND);
		}
		else
		{
			strObjMark.Format("%s",pTaxiway->GetMarking().c_str());
			//strObjMark.Format("Taxiway : %s",(*iter).GetMarking().c_str());
			m_wndTree.SetRedraw(FALSE);
			hTaxiwayNodeTreeItem = m_wndTree.InsertItem(strObjMark,cni,FALSE,FALSE,hRootTreeItem);
			m_wndTree.SetRedraw(TRUE);
		}
		m_wndTree.SetItemData(hTaxiwayNodeTreeItem,(DWORD_PTR)pItem);		
	}
	m_wndTree.Expand(hRootTreeItem,TVE_EXPAND);
	
	UpdateToolBar();
}

void CDlgTowingRoute::TraverseOne(HTREEITEM hDataTreeItem,int nParentID,HTREEITEM& hTreeItem)
{
	CTaxiwayNode* pItem = (CTaxiwayNode*)m_wndTree.GetItemData(hDataTreeItem);
	if(pItem->GetUniqueID() == nParentID)
		hTreeItem = hDataTreeItem;	
	else
	{
		if(m_wndTree.ItemHasChildren(hDataTreeItem))
		{
			HTREEITEM hNextItem;
			HTREEITEM hChildItem = m_wndTree.GetChildItem(hDataTreeItem);

			while (hChildItem != NULL)
			{
				hNextItem = m_wndTree.GetNextItem(hChildItem, TVGN_NEXT);
				TraverseOne(hChildItem,nParentID,hTreeItem);
				if(hTreeItem != NULL)
					break;
				else
					hChildItem = hNextItem;
			}
		}
	}
}

void CDlgTowingRoute::TraverseAll(HTREEITEM hDataTreeItem,int nParentID,HTREEITEM& hTreeItem)
{
	while (hDataTreeItem != NULL)
	{
		TraverseOne(hDataTreeItem,nParentID,hTreeItem);
		if(hTreeItem != NULL)
			break;
		else
			hDataTreeItem = m_wndTree.GetNextSiblingItem(hDataTreeItem);
	}
}
LRESULT CDlgTowingRoute::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	// TODO: Add your specialized code here and/or call the base class
	switch(message){	
		case UM_CEW_COMBOBOX_BEGIN:
			{
				CWnd* pWnd = m_wndTree.GetEditWnd((HTREEITEM)wParam);
				CRect rectWnd;
				m_wndTree.GetItemRect((HTREEITEM)wParam,rectWnd,TRUE);
				pWnd->SetWindowPos(NULL,rectWnd.right,rectWnd.top,0,0,SWP_NOSIZE);
				CComboBox* pCB = (CComboBox*)pWnd;
				pCB->ResetContent();

				HTREEITEM hSelTreeItem=(HTREEITEM)wParam;
				HTREEITEM hParentTreeItem = m_wndTree.GetParentItem(hSelTreeItem);
				HTREEITEM hRootTreeItem = m_wndTree.GetRootItem();
			
				if(hParentTreeItem && hParentTreeItem != hRootTreeItem)
				{


					CTaxiwayNode* pParentItem = (CTaxiwayNode*)m_wndTree.GetItemData(hParentTreeItem);
					int nParentTaxiwayID = pParentItem->GetTaxiwayID();

					Taxiway* pParentTaxiway = GetTaxiwayByID(nParentTaxiwayID);
					if(pParentTaxiway == NULL )
						return 0;

					std::set<ALTObject*> vTaxiways;
					std::set<ALTObject *> vRunways;
					m_pAltNetwork->GetIntersectedTaxiwayRunway((ALTObject*)pParentTaxiway,
						vTaxiways, vRunways);

					for (std::set<ALTObject*>::iterator iterTaxiway = vTaxiways.begin(); 
						iterTaxiway != vTaxiways.end(); ++iterTaxiway)
					{
						ALTObject* pObj = (*iterTaxiway);
						if (pObj->GetType() != ALT_TAXIWAY)
							continue;

						Taxiway* pTaxiway = (Taxiway*)pObj;

						IntersectionNodeList twINodeList;
						m_pAltNetwork->GetTaxiwayTaxiwayIntersectNodeList(pParentTaxiway,
							(Taxiway*)(*iterTaxiway) , twINodeList);

						if (twINodeList.empty())
							continue;
		
						if(twINodeList.size() == 1 )
						{
							int idx = twINodeList[0].GetIndex();
							CString strObjMarkTemp = pTaxiway->GetMarking().c_str();
							CString strObjMark;
							strObjMark.Format("%s",strObjMarkTemp);
							int nIndex = pCB->AddString(strObjMark);
							//combdata data;
							//data.nOterObjID = pTaxiway->getID();
							//data.nIndex = idx;
							//m_vCombdata.push_back(data);
							RouteItemData* pNodeData = new RouteItemData;
							pNodeData->pTaxiway = pTaxiway;
							pNodeData->nIntersecIndex = idx;
							pCB->SetItemData(nIndex, (DWORD_PTR)pNodeData);
                            m_vectRouteNode.push_back( pNodeData );
						}
						else// > 1
						{
							for (int i=0; i<(int)twINodeList.size(); i++)
							{
								int idx = twINodeList[i].GetIndex();
								CString strObjMarkTemp = pTaxiway->GetMarking().c_str();
								CString strObjMark;
								strObjMark.Format("%s(%s&%d)",strObjMarkTemp,
									pParentTaxiway->GetMarking().c_str(),idx);
								int nIndex = pCB->AddString(strObjMark);
								//combdata data;
								//data.nOterObjID = pTaxiway->getID();
								//data.nIndex = idx;
								//m_vCombdata.push_back(data);
								RouteItemData* pNodeData = new RouteItemData;
								pNodeData->pTaxiway = pTaxiway;
								pNodeData->nIntersecIndex = idx;
								pCB->SetItemData(nIndex, (DWORD_PTR)pNodeData);
								m_vectRouteNode.push_back( pNodeData );
							}
						}
					
					}



				
					//std::vector<IntersectionNode> vNodeList;
					//vNodeList.clear();
					//IntersectionNode::ReadIntersecNodeListWithObjType(nParentTaxiwayID, ALT_TAXIWAY, vNodeList);
					//if((int)vNodeList.size())
					//{	
					//	std::vector<IntersectionNode>::iterator iter = vNodeList.begin();
					//	m_vCombdata.clear();
					//	for (; iter != vNodeList.end(); ++iter)
					//	{
					//		std::vector<TaxiwayIntersectItem*> vTaxiwayItms = iter->GetTaxiwayIntersectItemList();
					//		for(int i=0;i<(int)vTaxiwayItms.size();i++)
					//		{
					//			TaxiwayIntersectItem* theIntersectItem = vTaxiwayItms[i];
					//			Taxiway * pOtherTaxiway = theIntersectItem->GetTaxiway();

					//			if( pOtherTaxiway && pOtherTaxiway->getID()!= nParentTaxiwayID)
					//			{
					//				int idx = (*iter).GetIndex();
					//				CString strObjMarkTemp = pOtherTaxiway->GetMarking().c_str();
					//				CString strObjMark;
					//				strObjMark.Format("%s&%d",strObjMarkTemp,idx+1);
					//				pCB->AddString(strObjMark);
					//				combdata data;
					//				data.nOterObjID = pOtherTaxiway->getID();
					//				data.nIndex = idx;
					//				m_vCombdata.push_back(data);
					//			}
					//		}						
					//	}
					//}



				}
				else
				{
					std::vector<Taxiway>::iterator iter = m_vTaxiway.begin();
					for (; iter != m_vTaxiway.end(); ++iter)	
					{
						int nItem = pCB->AddString((*iter).GetMarking().c_str());
                        //pCB->SetItemData(nItem,(*iter).getID());
						RouteItemData* pNodeData = new RouteItemData;
						pNodeData->pTaxiway = &(*iter);
						pNodeData->nIntersecIndex = -1;
						pCB->SetItemData( nItem, (DWORD_PTR)pNodeData );
						m_vectRouteNode.push_back( pNodeData );
					}
				}
			}
			break;
		case UM_CEW_COMBOBOX_SELCHANGE:
			{
				CWnd* pWnd = m_wndTree.GetEditWnd((HTREEITEM)wParam);
				CComboBox* pCB = (CComboBox*)pWnd;
				HTREEITEM hSelTreeItem=(HTREEITEM)wParam;
				HTREEITEM hParentTreeItem = m_wndTree.GetParentItem(hSelTreeItem);
				HTREEITEM hRootTreeItem = m_wndTree.GetRootItem();
				CString strText = _T("Taxiway : ")+ *(CString*)lParam;
				CString strTextOld = m_wndTree.GetItemText(hSelTreeItem);
				if(strText == strTextOld)
					return 0;
				m_wndTree.SetItemText(hSelTreeItem,strText);

				CTaxiwayNode* pItem = (CTaxiwayNode*)m_wndTree.GetItemData(hSelTreeItem);
				int nSel = pCB->GetCurSel();	
				if(nSel == LB_ERR)
					return -1;
				RouteItemData* pNodeData = (RouteItemData*)pCB->GetItemData( nSel );
				ASSERT( pNodeData != NULL );

				if(hParentTreeItem && hParentTreeItem != hRootTreeItem)
				{
					pItem->SetTaxiwayID( pNodeData->pTaxiway->getID());
					pItem->SetIdx( pNodeData->nIntersecIndex );
					//pItem->SetTaxiwayID(m_vCombdata[nSel].nOterObjID);
					//pItem->SetIdx(m_vCombdata[nSel].nIndex);

				}
				else
					pItem->SetTaxiwayID( pNodeData->pTaxiway->getID());
				if(m_wndTree.ItemHasChildren(hSelTreeItem))
				{
					int nSel = m_wndListBox.GetCurSel();
					if (nSel == -1)
						return -1;
					CTowingRoute* pRouteItem = (CTowingRoute*)m_wndListBox.GetItemData(nSel);
					pRouteItem->UpdatevTaxiwayNode(pItem);
					HTREEITEM hNextItem;
					HTREEITEM hChildItem = m_wndTree.GetChildItem(hSelTreeItem);

					while (hChildItem != NULL)
					{
						hNextItem = m_wndTree.GetNextItem(hChildItem, TVGN_NEXT);
						m_wndTree.DeleteItem(hChildItem);
						hChildItem = hNextItem;
					}
				}
			}
			break;

		default:
			break;
	}
	return CXTResizeDialog::DefWindowProc(message, wParam, lParam);
}

void CDlgTowingRoute::OnContextMenu(CWnd* pWnd, CPoint point)
{
	// TODO: Add your message handler code here
	CRect rectTree;
	m_wndTree.GetWindowRect(&rectTree);
	if (!rectTree.PtInRect(point)) 
		return;

	m_wndTree.SetFocus();
	CPoint pt = point;
	m_wndTree.ScreenToClient(&pt);
	UINT iRet;
	HTREEITEM hRClickItem = m_wndTree.HitTest(pt, &iRet);
	if (iRet != TVHT_ONITEMLABEL)
	{
		hRClickItem = NULL;
		return;
	}

	m_wndTree.SelectItem(hRClickItem);	
	CMenu menuPopup; 
	menuPopup.CreatePopupMenu(); 

	HTREEITEM hParentItem = 0;
	hParentItem = m_wndTree.GetParentItem(hRClickItem);
	if(!hParentItem)
	{		
		menuPopup.AppendMenu(MF_POPUP, (UINT) ID_NEW_TAXIWAYNODE, _T("Add Route Item") ); 
	}		
	else
	{
		menuPopup.AppendMenu(MF_POPUP, (UINT) ID_NEW_TAXIWAYNODE, _T("Add Route Item") );
		menuPopup.AppendMenu(MF_POPUP, (UINT) ID_DEL_TAXIWAYNODE, _T("Delete Route Item") );
	}

	menuPopup.TrackPopupMenu(TPM_LEFTALIGN, point.x, point.y, this);
	menuPopup.DestroyMenu();
}

LRESULT CDlgTowingRoute::OnMessageModifyRouteName( WPARAM wParam, LPARAM lParam )
{

	if (m_pCurRoute)
	{
		m_pCurRoute->SetName((LPCSTR)lParam);

		LoadTree();
		m_wndTaxiwayNodeToolbar.GetToolBarCtrl().EnableButton(ID_NEW_TAXIWAYNODE, FALSE);
		GetDlgItem(IDC_SAVE)->EnableWindow(TRUE);
	}


	return 1;
}