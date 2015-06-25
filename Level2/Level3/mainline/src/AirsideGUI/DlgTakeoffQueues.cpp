#include "stdafx.h"
#include "Resource.h"
#include "DlgTakeoffQueues.h"
#include "../InputAirside/InputAirside.h"
#include "../InputAirside/ALTAirport.h"
#include "../InputAirside/Runway.h"
#include ".\dlgtakeoffqueues.h"
#include "../InputAirside/TakeoffQueuePosi.h"
#include "../InputAirside/TakeoffQueuesItem.h"
#include "../InputAirside/Taxiway.h"
#include "DlgSelectTakeoffPosi.h"
#include "../Common/AirportDatabase.h"
#include "DlgTakeOffPositionSelection.h"


namespace
{
	const UINT ID_NEW_TAKEOFF = 10;
	const UINT ID_DEL_TAKEOFF = 11;
	const UINT ID_NEW_QUEUE = 20;
	const UINT ID_DEL_QUEUE = 21;
//	const UINT ID_EDIT_QUEUE = 22;
}

CDlgTakeoffQueues::CDlgTakeoffQueues(int nProjID, CAirportDatabase* pAirportDB, CWnd* pParent /*=NULL*/)
 : CXTResizeDialog(CDlgTakeoffQueues::IDD, pParent)
 , m_nProjID(nProjID)
 ,m_TakeoffQueues(nProjID)
 ,m_pAirportDB(pAirportDB)
{
}

CDlgTakeoffQueues::~CDlgTakeoffQueues()
{
}

void CDlgTakeoffQueues::DoDataExchange(CDataExchange* pDX)
{
	CXTResizeDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE_QUEUENODES, m_wndQueueNodesTree);
	DDX_Control(pDX, IDC_LIST_TAKEOFFQUEUE, m_TakeoffListBox);
}

BEGIN_MESSAGE_MAP(CDlgTakeoffQueues, CXTResizeDialog)
	ON_BN_CLICKED(IDC_BUTTON_SAVE, OnSave)
	ON_WM_CREATE()
	ON_COMMAND(ID_NEW_TAKEOFF, OnNewTakeoff)
	ON_COMMAND(ID_DEL_TAKEOFF, OnDelTakeoff)
	ON_COMMAND(ID_NEW_QUEUE, OnNewQueue)
	ON_COMMAND(ID_DEL_QUEUE, OnDelQueue)
//	ON_COMMAND(ID_EDIT_QUEUE, OnEditQueue)
	ON_LBN_SELCHANGE(IDC_LIST_TAKEOFFQUEUE, OnLbnSelchangeListTakeoff)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_QUEUENODES, OnTvnSelchangedTreeQueuenodes)
	ON_NOTIFY(NM_CLICK, IDC_TREE_QUEUENODES, OnNMClickTreeQueuenodes)
	ON_WM_CONTEXTMENU()
END_MESSAGE_MAP()


// CDlgTakeoffQueues message handlers
BOOL CDlgTakeoffQueues::OnInitDialog()
{
	CXTResizeDialog::OnInitDialog();

	//std::vector<int> vAirportIds;
	//InputAirside::GetAirportList(m_nProjID, vAirportIds);
	//for (std::vector<int>::iterator iterAirportID = vAirportIds.begin(); iterAirportID != vAirportIds.end(); ++iterAirportID)
	//{
	//	ALTAirport airport;
	//	airport.ReadAirport(*iterAirportID);
	//	airport.getName();
	//	HTREEITEM hAirport = m_wndRunwayTree.InsertItem(airport.getName());
	//	m_wndRunwayTree.SetItemData(hAirport, *iterAirportID);

	//	std::vector<int> vRunways;
	//	ALTAirport::GetRunwaysIDs(*iterAirportID, vRunways);
	//	for (std::vector<int>::iterator iterRunwayID = vRunways.begin(); iterRunwayID != vRunways.end(); ++iterRunwayID)
	//	{
	//		Runway runway;
	//		runway.ReadObject(*iterRunwayID);
	//		HTREEITEM hRunwayItem = m_wndRunwayTree.InsertItem(runway.GetObjNameString(), hAirport);
	//		m_wndRunwayTree.SetItemData(hRunwayItem, *iterRunwayID);

	//		HTREEITEM hMarking1Item = m_wndRunwayTree.InsertItem(runway.GetMarking1().c_str(), hRunwayItem);
	//		m_wndRunwayTree.SetItemData(hMarking1Item, (int)RUNWAYMARK_FIRST);
	//		HTREEITEM hMarking2Item = m_wndRunwayTree.InsertItem(runway.GetMarking2().c_str(), hRunwayItem);
	//		m_wndRunwayTree.SetItemData(hMarking2Item, (int)RUNWAYMARK_SECOND);

	//		m_wndRunwayTree.Expand(hRunwayItem, TVE_EXPAND);

	//	}
	//	m_wndRunwayTree.Expand(hAirport, TVE_EXPAND);
	//}
	InitTakeoffToolBar();
	m_TakeoffQueues.ReadData();
	InitTakeoffListBox();
	//for (int i=0; i<m_TakeoffQueues.GetTakeoffQueuePosiCount(); i++)
	//{
	//	TakeoffQueuePosi* pTakeoffQueuePosi = m_TakeoffQueues.GetItem(i);
	//	m_strListTakeoffQueues.InsertAfter(m_strListTakeoffQueues.GetTailPosition(), pTakeoffQueuePosi->GetStrRunTaxiway());
	//}
	SetResize(m_TakeoffToolBar.GetDlgCtrlID(), SZ_TOP_LEFT, SZ_TOP_LEFT);
	SetResize(m_TakeoffQueueToolBar.GetDlgCtrlID(), SZ_TOP_CENTER, SZ_TOP_CENTER);
	SetResize(IDC_STATIC_TAKEOFFQUEUES, SZ_TOP_LEFT, SZ_BOTTOM_CENTER);
	SetResize(IDC_STATIC_TAKEOFFQUEUENODE, SZ_TOP_CENTER , SZ_BOTTOM_RIGHT);
	SetResize(IDC_LIST_TAKEOFFQUEUE, SZ_TOP_LEFT, SZ_BOTTOM_CENTER);
	SetResize(IDC_TREE_QUEUENODES, SZ_TOP_CENTER , SZ_BOTTOM_RIGHT);
	SetResize(IDC_BUTTON_SAVE, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);
	SetResize(IDOK, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);
	SetResize(IDCANCEL, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);

	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),TRUE);
	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),FALSE);

	return TRUE;
}

void CDlgTakeoffQueues::OnSave()
{
	try
	{
		CADODatabase::BeginTransaction() ;
		m_TakeoffQueues.SaveData();
		CADODatabase::CommitTransaction() ;

	}
	catch (CADOException e)
	{
		CADODatabase::RollBackTransation() ;
		AfxMessageBox(_T("Save data failed."));
	}
	
	//CXTResizeDialog::OnOK();
}

void CDlgTakeoffQueues::OnOK()
{
	try
	{
		CADODatabase::BeginTransaction() ;
		m_TakeoffQueues.SaveData();
		CADODatabase::CommitTransaction() ;

	}
	catch (CADOException e)
	{
		CADODatabase::RollBackTransation() ;
		return;
	}
	CXTResizeDialog::OnOK();
}

void CDlgTakeoffQueues::OnCancel()
{
	CXTResizeDialog::OnCancel();
}

int CDlgTakeoffQueues::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CXTResizeDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here
	//
	if (!m_TakeoffToolBar.CreateEx(this, TBSTYLE_FLAT,WS_CHILD|WS_VISIBLE|CBRS_ALIGN_TOP)
		|| !m_TakeoffToolBar.LoadToolBar(IDR_TOOLBAR_ADD_DEL))
	{
		return -1;
	}

	m_TakeoffToolBar.GetToolBarCtrl().SetCmdID(0, ID_NEW_TAKEOFF);
	m_TakeoffToolBar.GetToolBarCtrl().SetCmdID(1, ID_DEL_TAKEOFF);

	if (!m_TakeoffQueueToolBar.CreateEx(this, TBSTYLE_FLAT,WS_CHILD|WS_VISIBLE|CBRS_ALIGN_TOP,CRect(0,0,0,0),m_TakeoffToolBar.GetDlgCtrlID()+1)
		|| !m_TakeoffQueueToolBar.LoadToolBar(IDR_TOOLBAR_ADD_DEL))
	{
		return -1;
	}

	m_TakeoffQueueToolBar.GetToolBarCtrl().SetCmdID(0, ID_NEW_QUEUE);
	m_TakeoffQueueToolBar.GetToolBarCtrl().SetCmdID(1, ID_DEL_QUEUE);
//	m_TakeoffQueueToolBar.GetToolBarCtrl().SetCmdID(2, ID_EDIT_QUEUE);

	return 0;
}
void CDlgTakeoffQueues::InitTakeoffToolBar()
{
	CRect rcToolbar;

	// set the position of the Takeoff list toolbar	
	m_TakeoffListBox.GetWindowRect(&rcToolbar);
	ScreenToClient(&rcToolbar);
	rcToolbar.left += 4;
	rcToolbar.top -= 26;
	rcToolbar.bottom = rcToolbar.top + 22;
	m_TakeoffToolBar.MoveWindow(rcToolbar);

	m_TakeoffToolBar.GetToolBarCtrl().EnableButton(ID_NEW_TAKEOFF);
	m_TakeoffToolBar.GetToolBarCtrl().EnableButton(ID_DEL_TAKEOFF, FALSE);

	// set the position of the Takeoff queue toolbar	
	m_wndQueueNodesTree.GetWindowRect(&rcToolbar);
	ScreenToClient(&rcToolbar);
	rcToolbar.left += 4;
	rcToolbar.top -= 26;
	rcToolbar.bottom = rcToolbar.top + 22;
	m_TakeoffQueueToolBar.MoveWindow(rcToolbar);

	m_TakeoffQueueToolBar.GetToolBarCtrl().EnableButton(ID_NEW_QUEUE, FALSE);
	m_TakeoffQueueToolBar.GetToolBarCtrl().EnableButton(ID_DEL_QUEUE, FALSE);
//	m_TakeoffQueueToolBar.GetToolBarCtrl().EnableButton(ID_EDIT_QUEUE, FALSE);

}
void CDlgTakeoffQueues::OnNewTakeoff()
{
	//if (m_TakeoffQueues.GetTakeoffQueuePosiCount() <= 0)
	//{
	//	MessageBox("You have not established the TakeoffPosition!");
	//	return;
	//}

	// control handling
	//CDlgSelectTakeoffPosi dlg(&m_TakeoffQueues);
	//if(IDOK != dlg.DoModal())
	//	return;
	CDlgSelectTakeoffPosition dlg(m_nProjID, -1, -1, this);
	dlg.SetAirportDB(m_pAirportDB);
	if(dlg.DoModal() != IDOK)
		return;

	if (dlg.m_arSelName.GetSize() == 0)
	{
		return;
	}

	CString strName = dlg.m_arSelName[0];
	//CString strRunTaxiway = dlg.GetStrRunTaxiway();

	bool bSelected = false;
	RunwayExit runwayExit = dlg.m_vRunwayExitList[0];
	for (int i=0; i<m_TakeoffQueues.GetTakeoffQueuePosiCount(); i++)
	{
		TakeoffQueuePosi* pTakeoffQueuePosi = m_TakeoffQueues.GetItem(i);

		if( runwayExit.GetID() ==pTakeoffQueuePosi->GetTakeoffPosiInterID() )
		{
			bSelected = true;
			break;
		}
	}
	if(bSelected == false)
	{
		TakeoffQueuePosi* pTakeoffQueuePosi = new TakeoffQueuePosi;
		pTakeoffQueuePosi->InitFromRunwayExit(runwayExit);
		int nSel = m_TakeoffListBox.AddString(strName);
		m_TakeoffListBox.SetItemData(nSel, (DWORD_PTR)pTakeoffQueuePosi);
		m_TakeoffQueues.AddTakeoffQueuePosi(pTakeoffQueuePosi);
	}
	else
	{
		MessageBox("the selected TakeoffPosition has been existed.");
		return;
	}
	UpdateToolBar();	
}
void CDlgTakeoffQueues::OnDelTakeoff()
{
	int nSel = m_TakeoffListBox.GetCurSel();
	if (nSel == LB_ERR)
		return;
	TakeoffQueuePosi* pTakeoffQueuePosi = (TakeoffQueuePosi*)m_TakeoffListBox.GetItemData(nSel);
	m_TakeoffQueues.DeleteTakeoffQueuePosi(pTakeoffQueuePosi);
	m_TakeoffListBox.DeleteString(nSel);
	m_wndQueueNodesTree.DeleteAllItems();
	UpdateToolBar();	
}

void CDlgTakeoffQueues::UpdateToolBar()
{
	if (m_TakeoffListBox.GetCount() > 0 && m_TakeoffListBox.GetCurSel() != -1)
	{
		m_TakeoffToolBar.GetToolBarCtrl().EnableButton(ID_DEL_TAKEOFF, TRUE);
		m_TakeoffQueueToolBar.GetToolBarCtrl().EnableButton(ID_NEW_QUEUE, TRUE);

		if (m_wndQueueNodesTree.GetSelectedItem() != NULL)
		{
			m_TakeoffQueueToolBar.GetToolBarCtrl().EnableButton(ID_DEL_QUEUE, TRUE);
		}
		else
		{
			m_TakeoffQueueToolBar.GetToolBarCtrl().EnableButton(ID_DEL_QUEUE, FALSE);
		}
	}
	else
	{
		m_TakeoffToolBar.GetToolBarCtrl().EnableButton(ID_DEL_TAKEOFF, FALSE);
		m_TakeoffQueueToolBar.GetToolBarCtrl().EnableButton(ID_NEW_QUEUE, FALSE);
		m_TakeoffQueueToolBar.GetToolBarCtrl().EnableButton(ID_DEL_QUEUE, FALSE);
	}
}
void CDlgTakeoffQueues::GetListByTaxiwayID(int nTaxiwayID)
{
	if (nTaxiwayID == -1)	// have no taxiway
	{
		m_vNodeList.clear();
		return;
	}

	// get the list of intersect node
	m_vNodeList.clear();
	IntersectionNode::ReadIntersecNodeList(nTaxiwayID, m_vNodeList);
	int nCurNodeCount = m_vNodeList.size();
	if (nCurNodeCount <= 0)
		return;

	m_strListNode.RemoveAll();
	std::vector<IntersectionNode>::iterator iter = m_vNodeList.begin();
//	m_nCurFirtNodeID = (*iter).m_nUniqueID;
	for (; iter != m_vNodeList.end(); ++iter)
	{
		if( iter->HasObject(nTaxiwayID) && iter->GetTaxiwayIntersectItemList().size()>1 )
		{
			CString strNodeName = iter->GetName();
			m_strListNode.InsertAfter(m_strListNode.GetTailPosition(), strNodeName);
		}
		
		/*ALTObject * pObj  = (*iter).GetOtherObject(nTaxiwayID);
		if(pObj && pObj->GetType() == ALT_TAXIWAY)
		{
			CString strObjName = pObj->GetObjNameString();
			CString strNode;
			strNode.Format("(%s)-%d", strObjName, ((*iter).GetIndex())+1);
			m_strListNode.InsertAfter(m_strListNode.GetTailPosition(), strNode);
		}*/
	}
}

void CDlgTakeoffQueues::OnLbnSelchangeListTakeoff()
{
	// TODO: Add your control notification handler code here
	UpdateToolBar();	
	InitQueueNodesTree();	
}
void CDlgTakeoffQueues::OnNewQueue()
{
	HTREEITEM hSel = m_wndQueueNodesTree.GetSelectedItem();

	TakeoffQueuesItem* pTakeoffQueuesItem = new TakeoffQueuesItem;
	int nMaxUniID = m_TakeoffQueues.GetMaxUniID();
	nMaxUniID++;
	m_TakeoffQueues.SetMaxUniID(nMaxUniID);
	pTakeoffQueuesItem->SetUniID(nMaxUniID);

	int nSel = m_TakeoffListBox.GetCurSel();
	if (nSel == LB_ERR)
		return;
	TakeoffQueuePosi* pTakeoffQueuePosi = (TakeoffQueuePosi*)m_TakeoffListBox.GetItemData(nSel);

//	pTakeoffQueuesItem->SetTakeoffQueueID(pTakeoffQueuePosi->getID());//

	HTREEITEM hQueueNodesTreeItem;
	COOLTREE_NODE_INFO cni;
	CCoolTree::InitNodeInfo(this,cni);
	cni.nt=NT_NORMAL;
	cni.net=NET_COMBOBOX;
	cni.bAutoSetItemText= false;
	CString strObjName = "Please Select";
	if(hSel != NULL)
	{
		TakeoffQueuesItem* pParTakeoffQueuesItem = (TakeoffQueuesItem*)m_wndQueueNodesTree.GetItemData(hSel);

		m_vNodeList.clear();
		IntersectionNode::ReadIntersecNodeList(pParTakeoffQueuesItem->GetTaxiwayID(), m_vNodeList);
		std::vector<IntersectionNode>::iterator iter = m_vNodeList.begin();
		m_vNodeListTemp.clear();
		int i = 0;
		for (; iter != m_vNodeList.end(); ++iter)
		{
			//ALTObject * pObj = (*iter).GetOtherObject(pParTakeoffQueuesItem->GetTaxiwayID());
			if( iter->HasObject(pParTakeoffQueuesItem->GetTaxiwayID()) && iter->GetTaxiwayIntersectItemList().size()>1)
			{
				m_vNodeListTemp.push_back((*iter));
				++i;
			}
			
		}
		if(m_vNodeListTemp.size() != 0)
		{
			m_wndQueueNodesTree.SetRedraw(FALSE);
			hQueueNodesTreeItem = m_wndQueueNodesTree.InsertItem(strObjName,cni,FALSE,FALSE,hSel);
			m_wndQueueNodesTree.SetRedraw(TRUE);
			m_wndQueueNodesTree.Expand(hSel,TVE_EXPAND);
			pTakeoffQueuesItem->SetParentID(pParTakeoffQueuesItem->GetUniID());
			m_wndQueueNodesTree.SetItemData(hQueueNodesTreeItem,(DWORD_PTR)pTakeoffQueuesItem);
			pTakeoffQueuePosi->AddTakeoffQueuesItem(pTakeoffQueuesItem);
			m_wndQueueNodesTree.DoEdit(hQueueNodesTreeItem);
		}
		else
		{
			AfxMessageBox("The runway exit has no Taxiway, cannot define queue.");
		}
	}
	else
	{
		m_vNodeList.clear();
		RunwayExit exit;
		exit.ReadData(pTakeoffQueuePosi->GetTakeoffPosiInterID());//= pTakeoffQueuePosi->get();
		

		IntersectionNode::ReadIntersecNodeList(exit.GetTaxiwayID(), m_vNodeList);
		std::vector<IntersectionNode>::iterator iter = m_vNodeList.begin();
		m_vNodeListTemp.clear();
		int i = 0;
		for (; iter != m_vNodeList.end(); ++iter)
		{
			if( iter->HasObject(exit.GetTaxiwayID()) && iter->GetTaxiwayIntersectItemList().size()>1 )
			{
				m_vNodeListTemp.push_back((*iter));
				++i;
			}		
		}
		if(m_vNodeListTemp.size() != 0)
		{
			m_wndQueueNodesTree.SetRedraw(FALSE);
			hQueueNodesTreeItem = m_wndQueueNodesTree.InsertItem(strObjName,cni,FALSE);	
			m_wndQueueNodesTree.SetRedraw(TRUE);
			pTakeoffQueuesItem->SetParentID(-1);
			m_wndQueueNodesTree.SetItemData(hQueueNodesTreeItem,(DWORD_PTR)pTakeoffQueuesItem);
			pTakeoffQueuePosi->AddTakeoffQueuesItem(pTakeoffQueuesItem);
			m_wndQueueNodesTree.DoEdit(hQueueNodesTreeItem);
		}
		else
		{
			AfxMessageBox("The runway exit has no Taxiway, cannot define queue.");
		}
	}
	UpdateToolBar();	
}
void CDlgTakeoffQueues::OnDelQueue()
{
	int nSel = m_TakeoffListBox.GetCurSel();
	if (nSel == LB_ERR)
		return;
	TakeoffQueuePosi* pTakeoffQueuePosi = (TakeoffQueuePosi*)m_TakeoffListBox.GetItemData(nSel);

	HTREEITEM hSel = m_wndQueueNodesTree.GetSelectedItem();
	if(hSel != NULL)
	{
		TakeoffQueuesItem* pTakeoffQueuesItem = (TakeoffQueuesItem*)m_wndQueueNodesTree.GetItemData(hSel);
		pTakeoffQueuePosi->DelFromVTakeoffPosi(pTakeoffQueuesItem);
		m_wndQueueNodesTree.DeleteItem(hSel);
	}
	UpdateToolBar();	
}

void CDlgTakeoffQueues::InitQueueNodesTree()
{
	m_wndQueueNodesTree.DeleteAllItems();
	m_wndQueueNodesTree.SetRedraw(FALSE);
	int nSel = m_TakeoffListBox.GetCurSel();
	if (nSel == LB_ERR)
		return;
	TakeoffQueuePosi* pTakeoffQueuePosi = (TakeoffQueuePosi*)m_TakeoffListBox.GetItemData(nSel);
	

	int nTaxiwayID=pTakeoffQueuePosi->GetRunwayExitDescription().GetTaxiwayID();//get left tree taxiway's ID
	
	int takeoffQueueCout=pTakeoffQueuePosi->GetTakeoffQueuesItemCount();//get queue node count;
	int nUintID=0;
	int nParentTaxiwayID=0;  
	for(int i=0;i<takeoffQueueCout;i++)
	{
		TakeoffQueuesItem* pTakeoffQueuesItem = pTakeoffQueuePosi->GetItem(i);
		//Taxiway taxiway;
		//taxiway.ReadObject(pTakeoffQueuesItem->GetTaxiwayID());
		int idx = pTakeoffQueuesItem->GetIdx();
		HTREEITEM hQueueNodesTreeItem;
		COOLTREE_NODE_INFO cni;
		CCoolTree::InitNodeInfo(this,cni);
		cni.nt=NT_NORMAL;
		cni.net=NET_COMBOBOX;
		cni.bAutoSetItemText= false;
		CString strObjName;
		CString strObjTempName;
		nParentTaxiwayID=nTaxiwayID;
		/*if(pTakeoffQueuesItem->GetParentID() != -1)
		{	
			for (int j=0;j<takeoffQueueCout;j++)
			{
				nUintID=pTakeoffQueuePosi->GetItem(j)->GetUniID();
				if (nUintID==pTakeoffQueuesItem->GetParentID())
				{
					nParentTaxiwayID=pTakeoffQueuePosi->GetItem(j)->GetTaxiwayID();
					break;
				}
			}
		}*/
		/*IntersectionNode *node=IntersectionNode::GetNode(nParentTaxiwayID,pTakeoffQueuesItem->GetTaxiwayID(),pTakeoffQueuesItem->GetIdx());
		strObjName.Format("Queue Node:%s",node->GetName());*/
		if(pTakeoffQueuesItem->GetParentID() != -1)
		{	
			HTREEITEM hRootTreeItem =  m_wndQueueNodesTree.GetRootItem();
			HTREEITEM hTreeItem = NULL;
			TraverseAll(hRootTreeItem,pTakeoffQueuesItem->GetParentID(),hTreeItem);
			if (hTreeItem)
			{
				TakeoffQueuesItem *parentItem=((TakeoffQueuesItem*)m_wndQueueNodesTree.GetItemData(hTreeItem));

				nParentTaxiwayID =parentItem->GetTaxiwayID();//get parent's taxiway ID

			}
			
			if (IntersectionNode::GetNodeName(nParentTaxiwayID,pTakeoffQueuesItem->GetTaxiwayID(),pTakeoffQueuesItem->GetIdx(),strObjTempName))
			{
				strObjName.Format("Queue Node:%s",strObjTempName);
				hQueueNodesTreeItem = m_wndQueueNodesTree.InsertItem(strObjName,cni,FALSE,FALSE,hTreeItem);
				m_wndQueueNodesTree.Expand(hTreeItem,TVE_EXPAND);
				m_wndQueueNodesTree.SetItemData(hQueueNodesTreeItem,(DWORD_PTR)pTakeoffQueuesItem);	
			}
			
		}
		else
		{
			if(IntersectionNode::GetNodeName(nParentTaxiwayID,pTakeoffQueuesItem->GetTaxiwayID(),pTakeoffQueuesItem->GetIdx(),strObjTempName))
			{
				strObjName.Format("Queue Node:%s",strObjTempName);
				hQueueNodesTreeItem = m_wndQueueNodesTree.InsertItem(strObjName,cni,FALSE);
				m_wndQueueNodesTree.SetItemData(hQueueNodesTreeItem,(DWORD_PTR)pTakeoffQueuesItem);	
			}
		}	
	}
	m_wndQueueNodesTree.SetRedraw(TRUE);
}
void CDlgTakeoffQueues::InitTakeoffListBox()
{
	for(int i=0;i<m_TakeoffQueues.GetTakeoffQueuePosiCount();i++)
	{
		TakeoffQueuePosi* pTakeoffQueuePosi = m_TakeoffQueues.GetItem(i);	
		if(pTakeoffQueuePosi->getID() != -1&&pTakeoffQueuePosi->GetTakeoffQueuesItemCount() > 0)
		{
			
			int nSel = m_TakeoffListBox.AddString(pTakeoffQueuePosi->GetRunwayExitDescription().GetName());
		    
			m_TakeoffListBox.SetItemData(nSel, (DWORD_PTR)pTakeoffQueuePosi);
			
		}		
	}
}
void CDlgTakeoffQueues::TraverseOne(HTREEITEM hRootTreeItem,int nParentID,HTREEITEM& hTreeItem)
{
	TakeoffQueuesItem* pTakeoffQueuesItem = (TakeoffQueuesItem*)m_wndQueueNodesTree.GetItemData(hRootTreeItem);
	if(pTakeoffQueuesItem->GetUniID() == nParentID)
	{
		hTreeItem = hRootTreeItem;
	}
	else
	{
		if (m_wndQueueNodesTree.ItemHasChildren(hRootTreeItem))
		{
			HTREEITEM hNextItem;
			HTREEITEM hChildItem = m_wndQueueNodesTree.GetChildItem(hRootTreeItem);

			while (hChildItem != NULL)
			{
				hNextItem = m_wndQueueNodesTree.GetNextItem(hChildItem, TVGN_NEXT);
				TraverseOne(hChildItem,nParentID,hTreeItem);
				if(hTreeItem != NULL)
					break;
				else
					hChildItem = hNextItem;
			}
		}
	}
}

void CDlgTakeoffQueues::TraverseAll(HTREEITEM hRootTreeItem,int nParentID,HTREEITEM& hTreeItem)
{
	while (hRootTreeItem != NULL)
	{
		TraverseOne(hRootTreeItem,nParentID,hTreeItem);
		if(hTreeItem != NULL)
			break;
		else
			hRootTreeItem = m_wndQueueNodesTree.GetNextSiblingItem(hRootTreeItem);
	}
}



LRESULT CDlgTakeoffQueues::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	// TODO: Add your specialized code here and/or call the base class
	
	switch(message){	
		case UM_CEW_COMBOBOX_BEGIN:
			{
				CWnd* pWnd = m_wndQueueNodesTree.GetEditWnd((HTREEITEM)wParam);
				CRect rectWnd;
				m_wndQueueNodesTree.GetItemRect((HTREEITEM)wParam,rectWnd,TRUE);
				pWnd->SetWindowPos(NULL,rectWnd.right,rectWnd.top,0,0,SWP_NOSIZE);
				CComboBox* pCB = (CComboBox*)pWnd;
				pCB->ResetContent();
				//if(pCB->GetCount()==0)
				//{
					
					
					int nSel = m_TakeoffListBox.GetCurSel();
					if (nSel == LB_ERR)
						return 0;
					TakeoffQueuePosi* pTakeoffQueuePosi = (TakeoffQueuePosi*)m_TakeoffListBox.GetItemData(nSel);
					RunwayExitDescription exit = pTakeoffQueuePosi->GetRunwayExitDescription();

					HTREEITEM hItem=(HTREEITEM)wParam;
					HTREEITEM hParItem = m_wndQueueNodesTree.GetParentItem(hItem);
					m_vNodeList.clear();
					TakeoffQueuesItem* pTakeoffQueuesItem;
					if(hParItem != NULL)
					{
						pTakeoffQueuesItem = (TakeoffQueuesItem*)m_wndQueueNodesTree.GetItemData(hParItem);
						IntersectionNode::ReadIntersecNodeList(pTakeoffQueuesItem->GetTaxiwayID(), m_vNodeList);
					}
					else
						IntersectionNode::ReadIntersecNodeList(exit.GetTaxiwayID(), m_vNodeList);

					std::vector<IntersectionNode>::iterator iter = m_vNodeList.begin();
					m_vCombdata.clear();
					for (; iter != m_vNodeList.end(); ++iter)
					{
						int nTaxiwayID;
						if(hParItem!=NULL)
							nTaxiwayID = pTakeoffQueuesItem->GetTaxiwayID();
						else
							nTaxiwayID = exit.GetTaxiwayID();
							
						std::vector<TaxiwayIntersectItem*> vTaxiwayItems = iter->GetTaxiwayIntersectItemList();
						for(int itaxiIdx = 0;itaxiIdx<(int)vTaxiwayItems.size(); itaxiIdx++)
						{
							TaxiwayIntersectItem* pTaxiItem = vTaxiwayItems[itaxiIdx];
							Taxiway * pOtherTaxiway = pTaxiItem->GetTaxiway();
							if(pOtherTaxiway && pOtherTaxiway->getID()!= nTaxiwayID)
							{
								int idx = (*iter).GetIndex();
								CString strObjNameTemp = pOtherTaxiway->GetMarking().c_str();
								CString strObjName;
								CString nodeName=(*iter).GetName();//get intersection node name which is two taxiway's marking

								//strObjName.Format("%s&%s",strObjNameTemp,nodeName);
								strObjNameTemp.Format("%s",nodeName);
								pCB->AddString(strObjNameTemp);
								combdata data;
								data.nOterObjID = pOtherTaxiway->getID();
								data.nIndex = idx;
								m_vCombdata.push_back(data);
							}
						}					
					}
			}
			break;
		case UM_CEW_COMBOBOX_SELCHANGE:
			{
				CWnd* pWnd = m_wndQueueNodesTree.GetEditWnd((HTREEITEM)wParam);
				CComboBox* pCB = (CComboBox*)pWnd;
				HTREEITEM hItem = (HTREEITEM)wParam;
				CString strText = _T("Queue Node:")+ *(CString*)lParam;
				CString strTextOld = m_wndQueueNodesTree.GetItemText(hItem);
				if(strText == strTextOld)
					return 0;
				m_wndQueueNodesTree.SetItemText(hItem,strText);
			
				TakeoffQueuesItem* pTakeoffQueuesItem = (TakeoffQueuesItem*)m_wndQueueNodesTree.GetItemData(hItem);
				int nSel = pCB->GetCurSel();

				pTakeoffQueuesItem->SetTaxiwayID(m_vCombdata[nSel].nOterObjID);
				pTakeoffQueuesItem->SetIdx(m_vCombdata[nSel].nIndex);
				if(m_wndQueueNodesTree.ItemHasChildren(hItem))
				{
					int nSel = m_TakeoffListBox.GetCurSel();
					if (nSel == LB_ERR)
						return 0;
					TakeoffQueuePosi* pTakeoffQueuePosi = (TakeoffQueuePosi*)m_TakeoffListBox.GetItemData(nSel);
					pTakeoffQueuePosi->UpdateVTakeoffPosi(pTakeoffQueuesItem);
					HTREEITEM hNextItem;
					HTREEITEM hChildItem = m_wndQueueNodesTree.GetChildItem(hItem);

					while (hChildItem != NULL)
					{
						hNextItem = m_wndQueueNodesTree.GetNextItem(hChildItem, TVGN_NEXT);
						m_wndQueueNodesTree.DeleteItem(hChildItem);
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

void CDlgTakeoffQueues::OnTvnSelchangedTreeQueuenodes(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	UpdateToolBar();
	*pResult = 0;
}

void CDlgTakeoffQueues::OnNMClickTreeQueuenodes(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here
	UpdateToolBar();
	*pResult = 0;
}

void CDlgTakeoffQueues::OnContextMenu(CWnd* pWnd, CPoint point)
{
	// TODO: Add your message handler code here
	CRect rectTree;
	m_wndQueueNodesTree.GetWindowRect(&rectTree);
	if (!rectTree.PtInRect(point)) 
		return;

	m_wndQueueNodesTree.SetFocus();
	CPoint pt = point;
	m_wndQueueNodesTree.ScreenToClient(&pt);
	UINT iRet;
	HTREEITEM hRClickItem = m_wndQueueNodesTree.HitTest(pt, &iRet);
	if (iRet != TVHT_ONITEMLABEL)
	{
		hRClickItem = NULL;
		return;
	}

	m_wndQueueNodesTree.SelectItem(hRClickItem);

	CMenu menuPopup; 
	menuPopup.CreatePopupMenu(); 
	menuPopup.AppendMenu(MF_POPUP, (UINT) ID_NEW_QUEUE, _T("Add Queue...") );
	menuPopup.AppendMenu(MF_POPUP, (UINT) ID_DEL_QUEUE, _T("Delete Queue") );
	menuPopup.TrackPopupMenu(TPM_LEFTALIGN, point.x, point.y, this);
	menuPopup.DestroyMenu();
}
