// DlgVehicleRoute.cpp : implementation file
//

#include "stdafx.h"
//#include "AirsideGUI.h"
#include "Resource.h"
#include "DlgVehicleRoute.h"
#include ".\dlgVehicleroute.h"
#include "..\InputAirside\ALTAirport.h"
#include "..\InputAirside\InputAirside.h"
#include "..\inputairside\IntersectionNode.h"

// CDlgVehicleRoute dialog
static const UINT ID_NEW_ROUTE = 10;
static const UINT ID_DEL_ROUTE = 11;

static const UINT ID_NEW_STRETCHNODE = 20;
static const UINT ID_DEL_STRETCHNODE = 21;

IMPLEMENT_DYNAMIC(CDlgVehicleRoute, CXTResizeDialog)
CDlgVehicleRoute::CDlgVehicleRoute(int nProjID, CWnd* pParent /*=NULL*/)
	: CXTResizeDialog(CDlgVehicleRoute::IDD, pParent)
	, m_nProjID(nProjID)
{
}

CDlgVehicleRoute::~CDlgVehicleRoute()
{
}

void CDlgVehicleRoute::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LISTBOX_ROUTELIST, m_wndListBoxRouteList);
	DDX_Control(pDX, IDC_TREE_ROUTE, m_wndTreeRoute);
}


BEGIN_MESSAGE_MAP(CDlgVehicleRoute, CXTResizeDialog)
	ON_WM_CREATE()
	ON_BN_CLICKED(IDC_BUTTON_SAVE, OnBnClickedButtonSave)
	ON_COMMAND(ID_NEW_ROUTE, OnNewRoute)
	ON_COMMAND(ID_DEL_ROUTE, OnDelRoute)
	ON_COMMAND(ID_NEW_STRETCHNODE, OnNewStretchNode)
	ON_COMMAND(ID_DEL_STRETCHNODE, OnDelStretchNode)
	ON_LBN_SELCHANGE(IDC_LISTBOX_ROUTELIST, OnLbnSelchangeListboxRoutelist)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_ROUTE, OnTvnSelchangedTreeRoute)
	ON_MESSAGE(WM_LBITEMCHANGED,OnListboxRouteListKillFocus)
	ON_WM_CONTEXTMENU()
END_MESSAGE_MAP()


// CDlgVehicleRoute message handlers

int CDlgVehicleRoute::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here
	if (!m_wndToolbarRouteList.CreateEx(this, TBSTYLE_FLAT,WS_CHILD|WS_VISIBLE|CBRS_ALIGN_TOP)
		|| !m_wndToolbarRouteList.LoadToolBar(IDR_TOOLBAR_ADD_DEL))
	{
		return -1;
	}

	if (!m_wndToolbarRoute.CreateEx(this,TBSTYLE_FLAT,WS_CHILD|WS_VISIBLE|CBRS_ALIGN_TOP,CRect(0,0,0,0),m_wndToolbarRouteList.GetDlgCtrlID()+1)
		|| !m_wndToolbarRoute.LoadToolBar(IDR_TOOLBAR_ADD_DEL))
	{
		return -1;
	}

	CToolBarCtrl& toolbarCtrlRouteList = m_wndToolbarRouteList.GetToolBarCtrl();
	toolbarCtrlRouteList.SetCmdID(0, ID_NEW_ROUTE);
	toolbarCtrlRouteList.SetCmdID(1, ID_DEL_ROUTE);

	CToolBarCtrl& toolbarCtrlStretchNode = m_wndToolbarRoute.GetToolBarCtrl();
	toolbarCtrlStretchNode.SetCmdID(0, ID_NEW_STRETCHNODE);
	toolbarCtrlStretchNode.SetCmdID(1, ID_DEL_STRETCHNODE);

	return 0;
}

void CDlgVehicleRoute::InitToolBar()
{
	CRect rcToolbar;

	// set the position of the RouteList toolbar	
	m_wndListBoxRouteList.GetWindowRect(&rcToolbar);
	ScreenToClient(&rcToolbar);
	rcToolbar.left += 4;
	rcToolbar.top -= 23;
	rcToolbar.bottom = rcToolbar.top + 22;
	m_wndToolbarRouteList.MoveWindow(rcToolbar);

	m_wndToolbarRouteList.GetToolBarCtrl().EnableButton(ID_NEW_ROUTE);
	m_wndToolbarRouteList.GetToolBarCtrl().EnableButton(ID_DEL_ROUTE,FALSE);

	// set the position of the Route toolbar	
	m_wndTreeRoute.GetWindowRect(&rcToolbar);
	ScreenToClient(&rcToolbar);
	rcToolbar.top -= 23;
	rcToolbar.bottom = rcToolbar.top + 22;
	rcToolbar.left += 4;
	m_wndToolbarRoute.MoveWindow(rcToolbar);

	m_wndToolbarRoute.GetToolBarCtrl().EnableButton(ID_NEW_STRETCHNODE,FALSE);
	m_wndToolbarRoute.GetToolBarCtrl().EnableButton(ID_DEL_STRETCHNODE,FALSE);
}

BOOL CDlgVehicleRoute::OnInitDialog()
{
	CXTResizeDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	InitToolBar();
	m_wndListBoxRouteList.SetHorizontalExtent(120);
	m_vehicleRouteList.ReadData(m_nProjID);
	GetAllStretchAndIntersections();
	SetListBoxRouteList();

	SetResize(m_wndToolbarRouteList.GetDlgCtrlID(), SZ_TOP_LEFT, SZ_TOP_LEFT);
	SetResize(m_wndToolbarRoute.GetDlgCtrlID(), SZ_TOP_CENTER, SZ_TOP_CENTER);

	SetResize(IDC_LISTBOX_ROUTELIST, SZ_TOP_LEFT, SZ_BOTTOM_CENTER);
	SetResize(IDC_GROUPBOX_VEHICLEROUTE, SZ_TOP_LEFT, SZ_BOTTOM_CENTER);
	SetResize(IDC_GROUPBOX_ROUTE, SZ_TOP_CENTER , SZ_BOTTOM_RIGHT);
	SetResize(IDC_TREE_ROUTE, SZ_TOP_CENTER , SZ_BOTTOM_RIGHT);
	SetResize(IDC_BUTTON_SAVE, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);
	SetResize(IDOK, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);
	SetResize(IDCANCEL, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);

	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),TRUE);
	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSErf
}

void CDlgVehicleRoute::SetListBoxRouteList()
{
	int nCount = m_vehicleRouteList.GetVehicleRouteCount();
	for(int i=0; i<nCount; i++)
	{
		CVehicleRoute* pItem = m_vehicleRouteList.GetVehicleRouteItem(i);
		CString strRouteName = pItem->GetRouteName();
		int nIndex = m_wndListBoxRouteList.AddString(strRouteName);
		m_wndListBoxRouteList.SetItemData(nIndex,(DWORD_PTR)pItem);
	}
	if(nCount>0)
	{
		m_wndListBoxRouteList.SetCurSel(0);
		OnLbnSelchangeListboxRoutelist();
	}
	else
	{
		UpdateToolBar();
		m_wndTreeRoute.DeleteAllItems();
	}
}

void CDlgVehicleRoute::OnOK()
{
	// TODO: Add your specialized code here and/or call the base class
	OnBnClickedButtonSave() ;
	CXTResizeDialog::OnOK();
}

void CDlgVehicleRoute::OnBnClickedButtonSave()
{
	// TODO: Add your control notification handler code here
	try
	{
		CADODatabase::BeginTransaction() ;
		m_vehicleRouteList.SaveData(m_nProjID);
		CADODatabase::CommitTransaction() ;
	}
	catch (CADOException e)
	{
		CADODatabase::RollBackTransation() ;
	}
	
	//CDialog::OnOK();
}

void CDlgVehicleRoute::UpdateToolBar()
{
	int nSel = m_wndListBoxRouteList.GetCurSel();
	if (nSel != LB_ERR)
	{
		m_wndToolbarRouteList.GetToolBarCtrl().EnableButton(ID_DEL_ROUTE, TRUE);
		m_wndToolbarRoute.GetToolBarCtrl().EnableButton(ID_NEW_STRETCHNODE, TRUE);

		HTREEITEM hSelTreeItem = m_wndTreeRoute.GetSelectedItem();
		if (hSelTreeItem && m_wndTreeRoute.GetParentItem(hSelTreeItem))
			m_wndToolbarRoute.GetToolBarCtrl().EnableButton(ID_DEL_STRETCHNODE, TRUE);
		else
			m_wndToolbarRoute.GetToolBarCtrl().EnableButton(ID_DEL_STRETCHNODE, FALSE);
	}
	else
	{
		m_wndToolbarRouteList.GetToolBarCtrl().EnableButton(ID_DEL_ROUTE, FALSE);
		m_wndToolbarRoute.GetToolBarCtrl().EnableButton(ID_NEW_STRETCHNODE, FALSE);
		m_wndToolbarRoute.GetToolBarCtrl().EnableButton(ID_DEL_STRETCHNODE, FALSE);
	}
}

void CDlgVehicleRoute::OnNewRoute()
{
	CVehicleRoute* pNewItem = new CVehicleRoute;
	m_vehicleRouteList.AddVehicleRouteItem(pNewItem);
	int nCount = m_wndListBoxRouteList.GetCount();
	CString strRouteName;
	strRouteName.Format("New Route %d", nCount + 1);

	int nItem = m_wndListBoxRouteList.AddString(strRouteName);
	m_wndListBoxRouteList.SetItemData(nItem, (DWORD_PTR)pNewItem);
	m_wndListBoxRouteList.SetCurSel(nItem);	
	m_wndListBoxRouteList.EditItem(nItem);
	pNewItem->SetRouteName(strRouteName);

	m_wndTreeRoute.DeleteAllItems();
	m_wndTreeRoute.SetRedraw(FALSE);
	COOLTREE_NODE_INFO cni;
	CCoolTree::InitNodeInfo(this,cni);
	HTREEITEM hRootTreeItem = m_wndTreeRoute.InsertItem(_T("Start"),cni,FALSE);
	m_wndTreeRoute.SetRedraw(TRUE);
}

void CDlgVehicleRoute::OnDelRoute()
{
	int nSel = m_wndListBoxRouteList.GetCurSel();
	if (nSel == LB_ERR)
		return;
	CVehicleRoute* pItem = (CVehicleRoute*)m_wndListBoxRouteList.GetItemData(nSel);
	m_vehicleRouteList.DelVehicleRouteItem(pItem);
	m_wndListBoxRouteList.DeleteString(nSel);

	int nCount = m_wndListBoxRouteList.GetCount();
	if(nCount - 1 >= 0)
	{
		if(nSel != 0)
			m_wndListBoxRouteList.SetCurSel(nSel-1);
		else
			m_wndListBoxRouteList.SetCurSel(0);
		OnLbnSelchangeListboxRoutelist();
	}
	else
		m_wndTreeRoute.DeleteAllItems();
}

void CDlgVehicleRoute::OnNewStretchNode()
{
	m_wndTreeRoute.SetRedraw(FALSE);
	int nSel = m_wndListBoxRouteList.GetCurSel();
	if (nSel == LB_ERR)
		return;
	CVehicleRoute* pVehicleRouteItem = (CVehicleRoute*)m_wndListBoxRouteList.GetItemData(nSel);
	int nMaxUniID = pVehicleRouteItem->GetMaxUniID();
	
	CStretchNode* pNewItem = new CStretchNode;
	pNewItem->SetUniqueID(nMaxUniID+1);

	COOLTREE_NODE_INFO cni;
	CCoolTree::InitNodeInfo(this,cni);
	cni.nt=NT_NORMAL;
	cni.net=NET_COMBOBOX;
	cni.bAutoSetItemText= false;
	CString strObjName = "Please Select";

	HTREEITEM hRootTreeItem = m_wndTreeRoute.GetRootItem();
	HTREEITEM hSelTreeItem = m_wndTreeRoute.GetSelectedItem();
	HTREEITEM hStretchNodeTreeItem;
	
	if(hSelTreeItem && hSelTreeItem != hRootTreeItem)
	{
		CStretchNode* pParentItem = (CStretchNode*)m_wndTreeRoute.GetItemData(hSelTreeItem);
		int nParentStretchID = pParentItem->GetStretchID();
		IntersectionNodeList vNodeList;
		IntersectionNode::ReadIntersecNodeListWithObjType(nParentStretchID,ALT_STRETCH,vNodeList);
		
		bool bHasStretch = false;
		for (IntersectionNodeList::iterator iter = vNodeList.begin(); iter != vNodeList.end(); ++iter)	
		{
			IntersectionNode& theNode = *iter;
			if( theNode.HasObject(nParentStretchID) ){
				bHasStretch = true;
				break;
			}
		}
		for(std::vector<Intersections>::iterator iter = m_vIntersections.begin();iter!= m_vIntersections.end(); ++iter)
		{
			Intersections& theNode = *iter;
			if(theNode.HasObject(nParentStretchID))
			{
				bHasStretch = true;
				break;
			}
		}

		if( bHasStretch )
		{
			hStretchNodeTreeItem = m_wndTreeRoute.InsertItem(strObjName,cni,FALSE,FALSE,hSelTreeItem);
			m_wndTreeRoute.Expand(hSelTreeItem,TVE_EXPAND);
			pNewItem->SetParentID(pParentItem->GetUniqueID());
			m_wndTreeRoute.SetItemData(hStretchNodeTreeItem,(DWORD_PTR)pNewItem);
			pVehicleRouteItem->AddStretchNodeItem(pNewItem);
			m_wndTreeRoute.DoEdit(hStretchNodeTreeItem);
		}
		else
			AfxMessageBox("Sorry,There is no Stretch.");
	}
	else
	{
		if((int)m_vStretch.size())
		{
			hStretchNodeTreeItem = m_wndTreeRoute.InsertItem(strObjName,cni,FALSE,FALSE,hRootTreeItem);
			m_wndTreeRoute.Expand(hRootTreeItem,TVE_EXPAND);
			pNewItem->SetParentID(-1);
			m_wndTreeRoute.SetItemData(hStretchNodeTreeItem,(DWORD_PTR)pNewItem);
			pVehicleRouteItem->AddStretchNodeItem(pNewItem);
			m_wndTreeRoute.DoEdit(hStretchNodeTreeItem);
		}
		else
			AfxMessageBox("Sorry,There is no Stretch.");

	}
	m_wndTreeRoute.SetRedraw(TRUE);
}

void CDlgVehicleRoute::OnDelStretchNode()
{
	int nSel = m_wndListBoxRouteList.GetCurSel();
	if (nSel == LB_ERR)
		return;
	CVehicleRoute* pVehicleRouteItem = (CVehicleRoute*)m_wndListBoxRouteList.GetItemData(nSel);

	HTREEITEM hSelTreeItem = m_wndTreeRoute.GetSelectedItem();
	if(hSelTreeItem)
	{
		CStretchNode* pItem = (CStretchNode*)m_wndTreeRoute.GetItemData(hSelTreeItem);
		pVehicleRouteItem->DelFromvStretchNode(pItem);
		m_wndTreeRoute.DeleteItem(hSelTreeItem);
	}
//	UpdateToolBar();	
}

void CDlgVehicleRoute::LoadTree()
{
	m_wndTreeRoute.DeleteAllItems();
	m_wndTreeRoute.SetRedraw(FALSE);
	COOLTREE_NODE_INFO cni;
	CCoolTree::InitNodeInfo(this,cni);
	HTREEITEM hRootTreeItem = m_wndTreeRoute.InsertItem(_T("Start"),cni,FALSE);
	int nSel = m_wndListBoxRouteList.GetCurSel();
	if (nSel == LB_ERR)
		return;
	CVehicleRoute* pVehicleRouteItem = (CVehicleRoute*)m_wndListBoxRouteList.GetItemData(nSel);
	for(int i=0;i<pVehicleRouteItem->GetStretchNodeCount();i++)
	{
		CStretchNode* pItem = pVehicleRouteItem->GetStretchNodeItem(i);
		int nStretchID= pItem->GetStretchID();
		std::vector<ALTObject>::iterator iter = m_vStretch.begin();
		for (; iter != m_vStretch.end(); ++iter)	
		{
			if((*iter).getID() == nStretchID)
				break;
		}
		cni.nt=NT_NORMAL;
		cni.net=NET_COMBOBOX;
		cni.bAutoSetItemText= false;
		HTREEITEM hStretchNodeTreeItem;
		CString strObjName = _T("Stretch:") + (*iter).GetObjNameString();
		if(pItem->GetParentID() != -1)
		{	
			HTREEITEM hDataTreeItem = m_wndTreeRoute.GetChildItem(hRootTreeItem);
			HTREEITEM hTreeItem = NULL;
			TraverseAll(hDataTreeItem,pItem->GetParentID(),hTreeItem);
			hStretchNodeTreeItem = m_wndTreeRoute.InsertItem(strObjName,cni,FALSE,FALSE,hTreeItem);			
		}
		else
			hStretchNodeTreeItem = m_wndTreeRoute.InsertItem(strObjName,cni,FALSE,FALSE,hRootTreeItem);

		m_wndTreeRoute.SetItemData(hStretchNodeTreeItem,(DWORD_PTR)pItem);		
	}
	m_wndTreeRoute.Expand(hRootTreeItem,TVE_EXPAND);
	m_wndTreeRoute.SetRedraw(TRUE);
	UpdateToolBar();
}

void CDlgVehicleRoute::OnLbnSelchangeListboxRoutelist()
{
	// TODO: Add your control notification handler code here
	LoadTree();
}

void CDlgVehicleRoute::OnTvnSelchangedTreeRoute(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	UpdateToolBar();
	*pResult = 0;
}

LRESULT CDlgVehicleRoute::OnListboxRouteListKillFocus( WPARAM wParam, LPARAM lParam )
{
	int nSel = m_wndListBoxRouteList.GetCurSel();
	if (nSel == LB_ERR)
		return NULL;
	CVehicleRoute* pItem = (CVehicleRoute*)m_wndListBoxRouteList.GetItemData(nSel);
	pItem->SetRouteName((LPCSTR)lParam);

	return 1;
}

void CDlgVehicleRoute::GetAllStretchAndIntersections()
{
	std::vector<int> vAirportIds;
	InputAirside::GetAirportList(m_nProjID, vAirportIds);
	for (std::vector<int>::iterator iterAirportID = vAirportIds.begin(); iterAirportID != vAirportIds.end(); ++iterAirportID)
	{
		ALTAirport airport;
		airport.ReadAirport(*iterAirportID);

		std::vector<ALTObject> vStretch;	
		ALTObject::GetObjectList(ALT_STRETCH,*iterAirportID,vStretch);
		std::vector<ALTObject>::iterator iter = vStretch.begin();
		for (; iter != vStretch.end(); ++iter)	
			m_vStretch.push_back(*iter);

		std::vector<int> vIntersectionID;
		ALTAirport::GetRoadIntersectionsIDs(*iterAirportID, vIntersectionID);		
		std::vector<int>::iterator iterInsectionID = vIntersectionID.begin();
		for (; iterInsectionID != vIntersectionID.end(); ++iterInsectionID)	
		{
			Intersections intersection;
			intersection.ReadObject(*iterInsectionID);
			m_vIntersections.push_back(intersection);
		}
	}
}

void CDlgVehicleRoute::TraverseOne(HTREEITEM hDataTreeItem,int nParentID,HTREEITEM& hTreeItem)
{
	CStretchNode* pItem = (CStretchNode*)m_wndTreeRoute.GetItemData(hDataTreeItem);
	if(pItem->GetUniqueID() == nParentID)
		hTreeItem = hDataTreeItem;	
	else
	{
		if(m_wndTreeRoute.ItemHasChildren(hDataTreeItem))
		{
			HTREEITEM hNextItem;
			HTREEITEM hChildItem = m_wndTreeRoute.GetChildItem(hDataTreeItem);

			while (hChildItem != NULL)
			{
				hNextItem = m_wndTreeRoute.GetNextItem(hChildItem, TVGN_NEXT);
				TraverseOne(hChildItem,nParentID,hTreeItem);
				if(hTreeItem != NULL)
					break;
				else
					hChildItem = hNextItem;
			}
		}
	}
}

void CDlgVehicleRoute::TraverseAll(HTREEITEM hDataTreeItem,int nParentID,HTREEITEM& hTreeItem)
{
	while (hDataTreeItem != NULL)
	{
		TraverseOne(hDataTreeItem,nParentID,hTreeItem);
		if(hTreeItem != NULL)
			break;
		else
			hDataTreeItem = m_wndTreeRoute.GetNextSiblingItem(hDataTreeItem);
	}
}



LRESULT CDlgVehicleRoute::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	// TODO: Add your specialized code here and/or call the base class
	switch(message){	
		case UM_CEW_COMBOBOX_BEGIN:
			{
				CWnd* pWnd = m_wndTreeRoute.GetEditWnd((HTREEITEM)wParam);
				CRect rectWnd;
				m_wndTreeRoute.GetItemRect((HTREEITEM)wParam,rectWnd,TRUE);
				pWnd->SetWindowPos(NULL,rectWnd.right,rectWnd.top,0,0,SWP_NOSIZE);
				CComboBox* pCB = (CComboBox*)pWnd;
				pCB->ResetContent();
				
				int nSel = m_wndListBoxRouteList.GetCurSel();
				if (nSel == LB_ERR)
					return 0;
				CVehicleRoute* pVehicleRouteItem = (CVehicleRoute*)m_wndListBoxRouteList.GetItemData(nSel);

				HTREEITEM hSelTreeItem=(HTREEITEM)wParam;
				HTREEITEM hParentTreeItem = m_wndTreeRoute.GetParentItem(hSelTreeItem);
				HTREEITEM hRootTreeItem = m_wndTreeRoute.GetRootItem();
				
				if(hParentTreeItem && hParentTreeItem != hRootTreeItem)
				{				
					CStretchNode* pParentItem = (CStretchNode*)m_wndTreeRoute.GetItemData(hParentTreeItem);
					
					int nParentStretchID = pParentItem->GetStretchID();
				

					IntersectionNodeList vNodeList;
					IntersectionNode::ReadIntersecNodeListWithObjType(nParentStretchID,ALT_STRETCH,vNodeList);

					std::vector<ALTObject*> vOtherStretchs;
					
					for (IntersectionNodeList::iterator iter = vNodeList.begin(); iter != vNodeList.end(); ++iter)	
					{
						IntersectionNode& theNode = *iter;
						if( theNode.HasObject(nParentStretchID) )
						{
							std::vector<StretchIntersectItem*> vStretchItmes = theNode.GetStretchIntersectItem();
							for(int iIdx = 0; iIdx < (int)vStretchItmes.size(); ++iIdx)
							{
								StretchIntersectItem* theItem = vStretchItmes[iIdx];
								if( theItem->GetObjectID()!= nParentStretchID && theItem->GetObject() )
								{
									int iObjidx =0 ;
									for(;iObjidx <(int)vOtherStretchs.size(); ++iObjidx)
									{
										ALTObject* pObj = vOtherStretchs[iObjidx];
										if(pObj&& pObj->getID() == theItem->GetObjectID())
											break;
									}
									if(iObjidx==vOtherStretchs.size())
										vOtherStretchs.push_back(theItem->GetObject());
								}
							}
						}	
					}		
					for(std::vector<Intersections>::iterator iter= m_vIntersections.begin(); iter!=m_vIntersections.end();iter++)
					{
						Intersections& theNode = *iter;
						if(theNode.HasObject(nParentStretchID))
						{
							InsecObjectPartVector& partVec = theNode.GetIntersectionsPart();
							for(int iIdx = 0;iIdx < (int)partVec.size(); ++iIdx)
							{
								InsecObjectPart& thePart = partVec[iIdx];
								if(thePart.GetObjectID()!= nParentStretchID && thePart.GetObject())
								{
									int iObjidx =0;
									for(;iObjidx <(int)vOtherStretchs.size(); ++iObjidx)
									{
										ALTObject* pObj = vOtherStretchs[iObjidx];
										if(pObj&& pObj->getID() == thePart.GetObjectID())
											break;
									}
									if(iObjidx==vOtherStretchs.size())
										vOtherStretchs.push_back(thePart.GetObject());
								}
							}
						}
					}
				
							
					for (std::vector<ALTObject*>::iterator iter = vOtherStretchs.begin(); iter != vOtherStretchs.end(); ++iter)
					{				
						ALTObject* pObj = (*iter);					
						CString strObjName = pObj->GetObjNameString();
						int nItem = pCB->AddString(strObjName);
						pCB->SetItemData(nItem,pObj->getID());
				
					}
				}
				else
				{
					std::vector<ALTObject>::iterator iter = m_vStretch.begin();
					for (; iter != m_vStretch.end(); ++iter)	
					{
						int nItem = pCB->AddString((*iter).GetObjNameString());
						pCB->SetItemData(nItem,(*iter).getID());
					}
				}
			}
			break;
		case UM_CEW_COMBOBOX_SELCHANGE:
			{
				CWnd* pWnd = m_wndTreeRoute.GetEditWnd((HTREEITEM)wParam);
				CComboBox* pCB = (CComboBox*)pWnd;
				HTREEITEM hItem = (HTREEITEM)wParam;
				CString strText =  _T("Stretch:") +  *(CString*)lParam;
				CString strTextOld =m_wndTreeRoute.GetItemText(hItem);
				if(strText == strTextOld)
					return 0;
				m_wndTreeRoute.SetItemText(hItem,strText);

				CStretchNode* pItem = (CStretchNode*)m_wndTreeRoute.GetItemData(hItem);
				int nSel = pCB->GetCurSel();		
				pItem->SetStretchID(pCB->GetItemData(nSel));

				if(m_wndTreeRoute.ItemHasChildren(hItem))
				{
					int nSel = m_wndListBoxRouteList.GetCurSel();
					if (nSel == LB_ERR)
						return 0;
					CVehicleRoute* pVehicleRouteItem = (CVehicleRoute*)m_wndListBoxRouteList.GetItemData(nSel);
					pVehicleRouteItem->UpdatevStretchNode(pItem);
					HTREEITEM hNextItem;
					HTREEITEM hChildItem = m_wndTreeRoute.GetChildItem(hItem);
					while (hChildItem != NULL)
					{
						hNextItem = m_wndTreeRoute.GetNextItem(hChildItem, TVGN_NEXT);
						m_wndTreeRoute.DeleteItem(hChildItem);
						hChildItem = hNextItem;
					}
				}
			}
			break;
		default:
			break;
	}

	return CDialog::DefWindowProc(message, wParam, lParam);
}

void CDlgVehicleRoute::OnContextMenu(CWnd* pWnd, CPoint point)
{
	// TODO: Add your message handler code here
	CRect rectTree;
	m_wndTreeRoute.GetWindowRect(&rectTree);
	if (!rectTree.PtInRect(point)) 
		return;

	m_wndTreeRoute.SetFocus();
	CPoint pt = point;
	m_wndTreeRoute.ScreenToClient(&pt);
	UINT iRet;
	HTREEITEM hRClickItem = m_wndTreeRoute.HitTest(pt, &iRet);
	if (iRet != TVHT_ONITEMLABEL)
	{
		hRClickItem = NULL;
		return;
	}

	m_wndTreeRoute.SelectItem(hRClickItem);	
	CMenu menuPopup; 
	menuPopup.CreatePopupMenu(); 

	HTREEITEM hParentItem = 0;
	hParentItem = m_wndTreeRoute.GetParentItem(hRClickItem);
	if(!hParentItem)
	{		
		menuPopup.AppendMenu(MF_POPUP, (UINT) ID_NEW_STRETCHNODE, _T("Add Route Item") ); 
	}		
	else
	{
		menuPopup.AppendMenu(MF_POPUP, (UINT) ID_NEW_STRETCHNODE, _T("Add Route Item") );
		menuPopup.AppendMenu(MF_POPUP, (UINT) ID_DEL_STRETCHNODE, _T("Delete Route Item") );
	}
	
	menuPopup.TrackPopupMenu(TPM_LEFTALIGN, point.x, point.y, this);
	menuPopup.DestroyMenu();
}