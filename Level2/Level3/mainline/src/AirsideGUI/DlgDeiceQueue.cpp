// DlgDeiceQueue.cpp : implementation file
//

#include "stdafx.h"
#include "Resource.h"
//#include "AirsideGUI.h"
#include "../MFCExControl/ComboBoxListBox.h"
#include "../MFCExControl/CoolTree.h"
#include "../MFCExControl/ARCTreeCtrl.h"
#include "./InputAirside/ALTObject.h"
#include "./InputAirside/ALTAirport.h"
#include "./InputAirside/Taxiway.h"
#include "./InputAirside/GroundRoutes.h"
#include "./InputAirside/GroundRoute.h"
#include "../InputAirside/AirportGroundNetwork.h"
#include "./InputAirside/IntersectionNode.h"
#include "./InputAirside/DeicePadQueue.h"
#include "./InputAirside/ALTObjectGroup.h"
#include "./Common/WinMsg.h"
#include "DlgDeiceQueue.h"
#include "DlgSelectDeicePads.h"



// CDlgDeiceQueue dialog

IMPLEMENT_DYNAMIC(CDlgDeiceQueue, CXTResizeDialog)	
CDlgDeiceQueue::CDlgDeiceQueue(int nProjID,int nAirportID,CWnd* pParent )
	: CXTResizeDialog(CDlgDeiceQueue::IDD, pParent)
{
	m_nProjID = nProjID;
	m_nAirportID = nAirportID;
	m_QueueList = new DeicePadQueueList();
	m_vPadList.clear();
	ALTAirport::GetDeicePadList(m_nAirportID,m_vPadList);
}

CDlgDeiceQueue::~CDlgDeiceQueue()
{
	if (m_QueueList != NULL)
	{
		delete m_QueueList;
		m_QueueList = NULL;
	}
}

void CDlgDeiceQueue::DoDataExchange(CDataExchange* pDX)
{
	CXTResizeDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_DEICEPAD, m_wndListbox);
	DDX_Control(pDX, IDC_TREE_ROUTEPATH, m_wndTreeCtrl);
}


BEGIN_MESSAGE_MAP(CDlgDeiceQueue, CXTResizeDialog)
	ON_WM_CREATE()
	ON_COMMAND(ID_TOOLBAR_ADD,OnNewDeicePad)
	ON_COMMAND(ID_TOOLBAR_DEL,OnDelDeicePad)
	ON_COMMAND(ID_TOOLBAR_QUEUE_ADD,OnNewTaxiWayPath)
	ON_COMMAND(ID_TOOLBAR_QUEUE_DEL,OnDelTaxiWayPath)
	ON_LBN_SELCHANGE(IDC_LIST_DEICEPAD, OnLbnSelchangeListDeicepad)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_ROUTEPATH, OnTvnSelchangedTreeRoutepath)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON_SAVE, OnBnClickedButtonSave)
END_MESSAGE_MAP()


// CDlgDeiceQueue message handlers
BOOL CDlgDeiceQueue::OnInitDialog()
{
	CXTResizeDialog::OnInitDialog();
	m_QueueList->ReadData(m_nProjID);
	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(FALSE);
	DisplayDeicePad();
	OnInitToolbar();
	m_wndDeicePadToolbar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_DEL,FALSE);
	m_wndQueueToolbar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_QUEUE_ADD,FALSE);
	m_wndQueueToolbar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_QUEUE_DEL,FALSE);
	GetAllDeicePad();

	m_wndQueueToolbar.SetDlgCtrlID(m_wndDeicePadToolbar.GetDlgCtrlID()+1);
	SetResize(m_wndDeicePadToolbar.GetDlgCtrlID(), SZ_TOP_LEFT, SZ_TOP_LEFT);
	SetResize(m_wndQueueToolbar.GetDlgCtrlID(), SZ_TOP_CENTER, SZ_BOTTOM_CENTER);
	SetResize(IDC_STATIC_QUEUE, SZ_TOP_LEFT, SZ_BOTTOM_CENTER);
	SetResize(IDC_LIST_DEICEPAD, SZ_TOP_LEFT, SZ_BOTTOM_CENTER);
	SetResize(IDC_STATIC_PATH, SZ_TOP_CENTER, SZ_BOTTOM_RIGHT);
	SetResize(IDC_TREE_ROUTEPATH, SZ_TOP_CENTER, SZ_BOTTOM_RIGHT);
	SetResize(IDC_BUTTON_SAVE, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);
	SetResize(IDOK, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);
	SetResize(IDCANCEL, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);

	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),TRUE);
	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),FALSE);
	return TRUE;
}

int CDlgDeiceQueue::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if(CXTResizeDialog::OnCreate(lpCreateStruct) == -1)
	{
		return -1;
	}
	if (!m_wndDeicePadToolbar.CreateEx(this,TBSTYLE_FLAT,WS_VISIBLE|WS_CHILD|CBRS_ALIGN_TOP|CBRS_TOOLTIPS,CRect(0,0,0,0),IDR_ROUTEPATH_TOOLBAR)
		|| !m_wndDeicePadToolbar.LoadToolBar(IDR_ROUTEPATH_TOOLBAR))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	if (!m_wndQueueToolbar.CreateEx(this,TBSTYLE_FLAT,WS_VISIBLE|WS_CHILD|CBRS_ALIGN_TOP|CBRS_TOOLTIPS,CRect(0,0,0,0),IDR_ROUTEPATH_TOOLBAR)
		|| !m_wndQueueToolbar.LoadToolBar(IDR_DEICEQUEUE_TOOLBAR))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}
	return 0;
}

void CDlgDeiceQueue::OnInitToolbar()
{
	CRect rect;
	GetDlgItem(IDC_STATIC_DEICE)->GetWindowRect(&rect);
	ScreenToClient(&rect);
	m_wndDeicePadToolbar.MoveWindow(&rect,true);
	m_wndDeicePadToolbar.ShowWindow(SW_SHOW);
	m_wndDeicePadToolbar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_ADD,TRUE);
	m_wndDeicePadToolbar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_DEL,TRUE);

	GetDlgItem(IDC_STATIC_ROUTE)->GetWindowRect(&rect);
	ScreenToClient(&rect);
	m_wndQueueToolbar.MoveWindow(&rect,true);
	m_wndQueueToolbar.ShowWindow(SW_SHOW);
	m_wndQueueToolbar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_ADD,TRUE);
	m_wndQueueToolbar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_DEL,TRUE);
}

void CDlgDeiceQueue::GetAllDeicePad()
{
	ALTObject* pAltObject = NULL;
	//std::vector<ALTObject> vObject;
	ALTObjectID objName;
	int nCount = 0;
	//ALTObject::GetObjectList(ALT_DEICEBAY,m_nProjID,vObject);
	nCount = (int)m_vPadList.size();
	for (int i = 0; i< nCount; i++)
	{
		pAltObject = m_vPadList.at(i).get();
		pAltObject->getObjName(objName);
		m_strDeicePadList.AddHead(objName.GetIDString());
	}
}

void CDlgDeiceQueue::OnNewDeicePad()
{

	//std::vector<ALTObject> vObject;
	//ALTObject::GetObjectList(ALT_DEICEBAY,m_nProjID,vObject);
	if ((int)m_vPadList.size() == 0)
	{
		::AfxMessageBox(_T("No deice pad to be selected"),MB_OK|MB_ICONQUESTION);
		return;
	}
	CDlgSelectDeicePads dlg(m_nProjID);
	if (dlg.DoModal() == IDOK)
	{
		for (int i = 0; i < m_QueueList->GetCount(); i++)
		{
			DeicePadQueueItem* pItem = m_QueueList->GetItem(i);
			if (dlg.GetSelDeicepadsFamilyID() == pItem->getDeicePadID())
			{
				::AfxMessageBox(_T("The deicepad is exist"));
				return;
			}
		}
		m_pCurDeiceItem = new DeicePadQueueItem();
		m_pCurDeiceItem->setDeicePadID(dlg.GetSelDeicepadsFamilyID());
		int nIndex = m_wndListbox.AddString(dlg.GetSelDeicepadsFamilyName());
		m_wndListbox.SetItemData(nIndex,(DWORD_PTR)m_pCurDeiceItem);
		m_QueueList->AddItem(m_pCurDeiceItem);
		DisplayQueue(m_pCurDeiceItem);
		GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
		m_wndDeicePadToolbar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_DEL,FALSE);
	}
}

void CDlgDeiceQueue::OnDelDeicePad()
{
	int nIndex = m_wndListbox.GetCurSel();
	m_pCurDeiceItem = (DeicePadQueueItem*)m_wndListbox.GetItemData(nIndex);
	ASSERT(m_pCurDeiceItem);
	for (int i = 0 ; i < m_pCurDeiceItem->GetCount(); i++)
	{
		m_pCurDeiceItem->DeleteItem(i);
	}
	m_QueueList->DeleteItem(nIndex);
	m_wndListbox.DeleteString(nIndex);
	m_wndTreeCtrl.DeleteAllItems();
	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
}

void CDlgDeiceQueue::OnDelTaxiWayPath()
{
	HTREEITEM hItem = m_wndTreeCtrl.GetSelectedItem();
	m_pCurRoute = (DeicePadQueueRoute*)m_wndTreeCtrl.GetItemData(GetParentItem(hItem,1));
	ASSERT(m_pCurRoute);
	if (GetTreeCount(hItem) == 1)
	{
		int nIndex = GetIndexByTree(hItem);
		m_pCurDeiceItem->DeleteItem(nIndex);
	}
	m_pCurRoute->DeleteItem(GetTreeCount(hItem)-1);
	m_wndTreeCtrl.DeleteItem(hItem);
	m_wndTreeCtrl.SelectItem(NULL);
	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
}

LRESULT CDlgDeiceQueue::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if(message == UM_CEW_COMBOBOX_BEGIN)	
	{

		CWnd* pWnd = m_wndTreeCtrl.GetEditWnd((HTREEITEM)wParam);
		CRect rectWnd;
		HTREEITEM hItem = (HTREEITEM)wParam;
		m_wndTreeCtrl.GetItemRect((HTREEITEM)wParam,rectWnd,TRUE);
		pWnd->SetWindowPos(NULL,rectWnd.right,rectWnd.top,0,0,SWP_NOSIZE);
		CComboBox* pCB=(CComboBox*)pWnd;		

		if(pCB->GetCount()!=0)
		{
			pCB->ResetContent();
		}

		SetComboBoxItem(pCB);


		int nObjID = -1;
		switch(GetTreeCount((HTREEITEM)wParam))
		{
		case 0:
			return 0;
		case 1:
			{
				m_pCurRoute = (DeicePadQueueRoute*)m_wndTreeCtrl.GetItemData((HTREEITEM)wParam);
				nObjID = m_pCurRoute->getRouteID(0);
			}
			break;
		default:
			nObjID = m_wndTreeCtrl.GetItemData((HTREEITEM)wParam);
			break;
		}

		if (-1 == nObjID)
		{
			ALTObject* AltObj = NULL;
			pCB->SetCurSel(0);
			if (GetTreeCount((HTREEITEM)wParam) == 1)
			{
				m_pCurRoute->UpdataItem(0,pCB->GetItemData(0));
				m_wndTreeCtrl.SetItemData((HTREEITEM)wParam,(DWORD_PTR)m_pCurRoute);
			}
			else
			{
				m_wndTreeCtrl.SetItemData((HTREEITEM)wParam,(DWORD_PTR)pCB->GetItemData(0));
			}
			AltObj = ALTObject::ReadObjectByID((int)pCB->GetItemData(0));
			CString strItemText = _T("");
			pCB->GetLBText(0,strItemText);
			setTreeItemText((HTREEITEM)wParam,AltObj,strItemText);
		}
		else
		{
			int i;
			for (i=0; i<pCB->GetCount(); i++)
			{
				int nALTObjID = (int)pCB->GetItemData(i);
				if (nALTObjID == nObjID)
				{			
					pCB->SetCurSel(i);
					break;
				}
			}

			if (i==pCB->GetCount())
			{
				pCB->SetCurSel(0);	
				ALTObject* AltObj = NULL;
				pCB->SetCurSel(0);
				if (GetTreeCount((HTREEITEM)wParam) == 1)
				{
					m_pCurRoute->UpdataItem(0,pCB->GetItemData(0));
					m_wndTreeCtrl.SetItemData((HTREEITEM)wParam,(DWORD_PTR)m_pCurRoute);
				}
				else
				{
					m_wndTreeCtrl.SetItemData((HTREEITEM)wParam,(DWORD_PTR)pCB->GetItemData(0));
				}
				AltObj = ALTObject::ReadObjectByID((int)pCB->GetItemData(0));
				CString strItemText = _T("");
				pCB->GetLBText(0,strItemText);
				setTreeItemText((HTREEITEM)wParam,AltObj,strItemText);
			}
		}
	}
	else if(message == UM_CEW_COMBOBOX_SELCHANGE)
	{
		CString strItemText = m_wndTreeCtrl.GetItemText((HTREEITEM)wParam);
		CWnd* pWnd =  m_wndTreeCtrl.GetEditWnd((HTREEITEM)wParam);
		CComboBox* pCB=(CComboBox*)pWnd;
		for (int i = 0; i < pCB->GetCount(); i++)
		{
			CString strComboxItemtext;
			pCB->GetLBText(i,strComboxItemtext);
			if (!strItemText.CompareNoCase(strComboxItemtext))
			{
				if (GetTreeCount((HTREEITEM)wParam) == 1)
				{
					m_pCurRoute->UpdataItem(0,pCB->GetItemData(i));
					m_wndTreeCtrl.SetItemData((HTREEITEM)wParam,(DWORD_PTR)m_pCurRoute);
				}
				else
				{
					m_wndTreeCtrl.SetItemData((HTREEITEM)wParam,(DWORD_PTR)pCB->GetItemData(i));
				}
				break;
			}
		}
		ALTObject* AltObj = NULL;
		if (GetTreeCount((HTREEITEM)wParam) == 1)
		{
		 	m_pCurRoute = (DeicePadQueueRoute*)m_wndTreeCtrl.GetItemData((HTREEITEM)wParam);
		 	AltObj = ALTObject::ReadObjectByID(m_pCurRoute->getRouteID(0));
		}
		else
		{
		 	AltObj = ALTObject::ReadObjectByID((int)m_wndTreeCtrl.GetItemData((HTREEITEM)wParam));
		}
		setTreeItemText((HTREEITEM)wParam,AltObj,strItemText);
		GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
	}
	else if (message == UM_CEW_COMBOBOX_END)
	{
		CString strItemText = m_wndTreeCtrl.GetItemText((HTREEITEM)wParam);
		ALTObject* AltObj = NULL;
		if (GetTreeCount((HTREEITEM)wParam) == 1)
		{
			m_pCurRoute = (DeicePadQueueRoute*)m_wndTreeCtrl.GetItemData((HTREEITEM)wParam);
			AltObj = ALTObject::ReadObjectByID(m_pCurRoute->getRouteID(0));
		}
		else
		{
			AltObj = ALTObject::ReadObjectByID((int)m_wndTreeCtrl.GetItemData((HTREEITEM)wParam));
		}
		setTreeItemText((HTREEITEM)wParam,AltObj,strItemText);
	}
	else if(message == UM_CEW_COMBOBOX_SETWIDTH)
	{
		m_wndTreeCtrl.SetWidth(200);
	}
// 	else if (message == WM_COMBO_IN_LISTBOX)
// 	{
// 		ALTObject* pAltObject = NULL;
// 		ALTObjectID objName;
//  		int nSelItem = (int)lParam;
//  		CString strDeiceName = m_strDeicePadList.GetAt(m_strDeicePadList.FindIndex(nSelItem));
// 		//std::vector<ALTObject> vObject;
// 		int nObjID = -1;
// 		//ALTObject::GetObjectList(ALT_DEICEBAY,m_nProjID,vObject);
// 		for (int i = 0; i< (int)m_vPadList.size(); i++)
// 		{
// 			pAltObject = m_vPadList.at(i).get();
// 			pAltObject->getObjName(objName);
// 			if (!objName.GetIDString().CompareNoCase(strDeiceName))
// 			{
// 				 nObjID = pAltObject->getID();
// 				 break;
// 			}
// 		}
// 		m_pCurDeiceItem->setDeicePadID(nObjID);
// 		CString strPadName = _T("");
// 		for (int i = 0; i < m_wndListbox.GetCount()-1; i++)
// 		{
// 			m_wndListbox.GetText(i,strPadName);
// 			if (!strDeiceName.CompareNoCase(strPadName))
// 			{
// 				m_wndTreeCtrl.DeleteAllItems();
// 				delete m_pCurDeiceItem;
// 				m_pCurDeiceItem = NULL;
// 				break;
// 			}
// 		}
// 		if (m_pCurDeiceItem != NULL)
// 		{
// 			m_QueueList->AddItem(m_pCurDeiceItem);
// 		}
// 		DisplayDeicePad();
// 		GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
// 	}
	return CXTResizeDialog::DefWindowProc(message, wParam, lParam);
}

int CDlgDeiceQueue::GetTreeCount(HTREEITEM hItem)
{
	int nCount = 0;
	if (hItem == NULL)
	{
		return -1;
	}
	while (m_wndTreeCtrl.GetParentItem(hItem))
	{
		nCount++;
		hItem = m_wndTreeCtrl.GetParentItem(hItem);
	}
	return nCount;
}

HTREEITEM CDlgDeiceQueue::GetParentItem(HTREEITEM hItem,int lCount)
{
	if (GetTreeCount(hItem) <= lCount)
	{
		return hItem;
	}
	HTREEITEM hParentItem = m_wndTreeCtrl.GetParentItem(hItem);
	while (hParentItem && GetTreeCount(hParentItem) != lCount)
	{
		hParentItem = m_wndTreeCtrl.GetParentItem(hParentItem);
	}
	return hParentItem;
}

void CDlgDeiceQueue::SetComboBoxItem(CComboBox* pCB)
{
	HTREEITEM hItem = m_wndTreeCtrl.GetSelectedItem();
	ALTObject altObject;
	std::vector<ALTObject> vObject;
	ALTObjectID objName;
	int nCount = 0;

	GroundRoutes* pRoute = new GroundRoutes(m_nAirportID);
	pRoute->ReadData();

	std::vector<int> vTaxiwayIDs;
	ALTAirport::GetTaxiwaysIDs(m_nAirportID, vTaxiwayIDs);
	int n = GetTreeCount(hItem);
	switch(GetTreeCount(hItem))
	{
	case 0:
		break;
	case 1:
		{
			for (std::vector<int>::iterator iter = vTaxiwayIDs.begin(); iter != vTaxiwayIDs.end(); ++iter)
			{
				Taxiway taxiway;
				taxiway.ReadObject(*iter);

				CString strTaxiwayMark;
				strTaxiwayMark.Format("%s", taxiway.GetMarking().c_str());
				int nItemIndex = pCB->AddString(strTaxiwayMark);
				int ncount = taxiway.getID();
				pCB->SetItemData(nItemIndex, (DWORD_PTR)(taxiway.getID()));
			}
			nCount = pRoute->GetCount();
			for (int i = 0; i< nCount; i++)
			{
				pRoute->GetItem(i)->getObjName(objName);
				int nItemIndex = pCB->AddString(objName.GetIDString());
				pCB->SetItemData(nItemIndex,(DWORD_PTR)pRoute->GetItem(i)->getID());
			}
		}
		break;
	default:
		{
			GroundRouteVector grVector;
			CAirportGroundNetwork AirportNetWork(m_nProjID);
			IntersectionNodeList InNodeList;
			HTREEITEM hParentItem = m_wndTreeCtrl.GetParentItem(hItem);
			ALTObject *AltObj;
			if (GetTreeCount(hItem) == 2)
			{
				m_pCurRoute = (DeicePadQueueRoute*)m_wndTreeCtrl.GetItemData(hParentItem);
				AltObj = ALTObject::ReadObjectByID(m_pCurRoute->getRouteID(0));
			}
			else
			{
				AltObj = ALTObject::ReadObjectByID((int)m_wndTreeCtrl.GetItemData(hParentItem));
			}
			if (AltObj == NULL)
			{
				::AfxMessageBox(_T("Please define parent item type"));
				m_pCurRoute->DeleteItem(GetTreeCount(hItem)-1);
				m_wndTreeCtrl.DeleteItem(hItem);
				return;
			}
			if (AltObj->GetType() == ALT_TAXIWAY)
			{
				int nIndex = 0;
				for (std::vector<int>::iterator iter = vTaxiwayIDs.begin(); iter != vTaxiwayIDs.end(); ++iter)
				{
					Taxiway taxiway;
					taxiway.ReadObject(*iter);
					if (AltObj->getID() != taxiway.getID())
					{
						InNodeList.clear();
						AirportNetWork.GetTaxiwayTaxiwayIntersectNodeList((Taxiway*)AltObj,&taxiway,InNodeList);
						if((int)InNodeList.size() > 0)
						{
							nIndex = pCB->AddString(taxiway.GetMarking().c_str());
							pCB->SetItemData(nIndex,(DWORD_PTR)taxiway.getID());
						}
					}
				}
				AirportNetWork.GetTaxiwayIntersectGroundRoutes((Taxiway*)AltObj,&grVector);
				for (GroundRouteVector::iterator it = grVector.begin(); it != grVector.end(); ++it)
				{
					(*it)->getObjName(objName);
					int nIndexItem = pCB->AddString(objName.GetIDString());
					pCB->SetItemData(nIndexItem,(DWORD_PTR)(*it)->getID());
				}
				if (pCB->GetCount() == 0)
				{
					m_pCurRoute->DeleteItem(GetTreeCount(hItem)-1);
					m_wndTreeCtrl.DeleteItem(hItem);
					::AfxMessageBox(_T("No taxiway to be selected"),MB_ICONQUESTION|MB_OK);
				}
			}
			else if (AltObj->GetType() == ALT_GROUNDROUTE)
			{
				std::vector<IntersectionNode>::iterator it;
				for (std::vector<int>::iterator iter = vTaxiwayIDs.begin(); iter != vTaxiwayIDs.end(); ++iter)
				{
					Taxiway taxiway;
					taxiway.ReadObject(*iter);
					InNodeList.clear();
					AirportNetWork.GetTaxiwayGroundRouteIntersectNodeList(&taxiway,(GroundRoute*)AltObj,&InNodeList);
					if ((int)InNodeList.size() > 0)
					{
						int nItemIndex = pCB->AddString(taxiway.GetMarking().c_str());
						pCB->SetItemData(nItemIndex,(DWORD_PTR)taxiway.getID());
					}
				}
				int nCount = pRoute->GetCount();
				for (int i =0; i < nCount; i++)
				{
					if (pRoute->GetItem(i)->getID() != AltObj->getID()
						&& AirportNetWork.IsALTObjIntersectWithGroundRoute(AltObj->getID(),pRoute->GetItem(i)))
					{
						pRoute->GetItem(i)->getObjName(objName);
						int nItemIndex = pCB->AddString(objName.GetIDString());
						pCB->SetItemData(nItemIndex,(DWORD_PTR)pRoute->GetItem(i)->getID());
					}
				}
				if (pCB->GetCount() == 0)
				{
					m_pCurRoute->DeleteItem(GetTreeCount(hItem)-1);
					m_wndTreeCtrl.DeleteItem(hItem);
					::AfxMessageBox(_T("No taxiway to be selected"),MB_ICONQUESTION|MB_OK);
				}
			}
		}
		break;
	}
}

void CDlgDeiceQueue::OnNewTaxiWayPath()
{
	std::vector<ALTObject> vObject;
	ALTObject::GetObjectList(ALT_TAXIWAY,m_nProjID,vObject);
	if ((int)vObject.size() == 0)
	{
		::AfxMessageBox(_T("No taxiway to be selected"),MB_OK|MB_ICONQUESTION);
		return;
	}
	HTREEITEM hItem = m_wndTreeCtrl.GetSelectedItem();	
	HTREEITEM hchildItem = NULL;
	COOLTREE_NODE_INFO cni;
	CCoolTree::InitNodeInfo(this,cni);
	cni.nt = NT_NORMAL;
	cni.net = NET_COMBOBOX;
	switch(GetTreeCount(hItem))
	{
	case 0:
		{
			m_pCurRoute = new DeicePadQueueRoute();
			hchildItem = m_wndTreeCtrl.InsertItem(_T("Taxiway :"),cni,FALSE,FALSE,hItem,TVI_LAST);
			m_pCurDeiceItem->AddItem(m_pCurRoute);
			m_pCurRoute->Additem(-1);
			m_wndTreeCtrl.SetItemData(hchildItem,(DWORD_PTR)m_pCurRoute);
		}
		break;
	default:
		{
			m_pCurRoute = (DeicePadQueueRoute*)m_wndTreeCtrl.GetItemData(GetParentItem(hItem,1));
			hchildItem = m_wndTreeCtrl.InsertItem(_T("Taxiway :"),cni,FALSE,FALSE,hItem,TVI_LAST);
			m_pCurRoute->Additem(-1);
		}
		break;

	}
	m_wndTreeCtrl.SetFocus();
	m_wndTreeCtrl.SelectItem(hchildItem);
	m_wndTreeCtrl.DoEdit(hchildItem);
	m_wndTreeCtrl.Expand(hItem,TVE_EXPAND);
}
void CDlgDeiceQueue::OnLbnSelchangeListDeicepad()
{
	// TODO: Add your control notification handler code here
	int nItem = m_wndListbox.GetCurSel();
	if (nItem != LB_ERR)
	{
		m_pCurDeiceItem = (DeicePadQueueItem*)m_wndListbox.GetItemData(nItem);
		if (m_pCurDeiceItem != NULL)
		{
			DisplayQueue(m_pCurDeiceItem);
		}
	}
	m_wndTreeCtrl.SelectItem(NULL);
	OnChangeToolbarStatus();
}

void CDlgDeiceQueue::DisplayQueue(DeicePadQueueItem* pDeiceItem)
{
	ASSERT(pDeiceItem);
	m_wndTreeCtrl.DeleteAllItems();
	m_wndTreeCtrl.SetImageList(m_wndTreeCtrl.GetImageList(TVSIL_NORMAL),TVSIL_NORMAL);

	HTREEITEM hItem = NULL;	
	HTREEITEM hParent = NULL;
	HTREEITEM hCurItem = NULL;
	COOLTREE_NODE_INFO cni;
	CCoolTree::InitNodeInfo(this,cni);
	cni.nt = NT_NORMAL;
	cni.net = NET_COMBOBOX;
	hCurItem = m_wndTreeCtrl.InsertItem(_T("Start"), cni, FALSE);
	COOLTREE_NODE_INFO *cn = NULL;
	cn = m_wndTreeCtrl.GetItemNodeInfo(hCurItem);


	ALTObjectID objName;
	int nCount = pDeiceItem->GetCount();
	for (int i = 0; i < nCount; i++)
	{
		hItem = hCurItem;
		m_pCurRoute = pDeiceItem->GetItem(i);
		for (int j = 0; j < m_pCurRoute->GetCount(); j++)
		{
			int nObjID = m_pCurRoute->getRouteID(j);
			ALTObject* AltObj = ALTObject::ReadObjectByID(nObjID);
			if (AltObj == NULL)
			{
				for (int k = j; k < m_pCurRoute->GetCount();k++)
				{
					m_pCurRoute->DeleteItem(k);
				}
				break;
			}
			switch(AltObj->GetType())
			{
			case ALT_TAXIWAY:
				{
					if (j == 0)
					{
						hItem = m_wndTreeCtrl.InsertItem(_T("Taxiway :") + CString(((Taxiway*)AltObj)->GetMarking().c_str()),cni,FALSE,FALSE,hItem,TVI_LAST);
						m_wndTreeCtrl.SetItemData(hItem,(DWORD_PTR)m_pCurRoute);
						m_wndTreeCtrl.Expand(m_wndTreeCtrl.GetRootItem(),TVE_EXPAND);
					}
					else
					{
						hParent = hItem;
						hItem = m_wndTreeCtrl.InsertItem(_T("Taxiway :") + CString(((Taxiway*)AltObj)->GetMarking().c_str()),cni,FALSE,FALSE,hItem,TVI_LAST);
						m_wndTreeCtrl.SetItemData(hItem,(DWORD_PTR)m_pCurRoute->getRouteID(j));
						m_wndTreeCtrl.Expand(hParent,TVE_EXPAND);
					}
				}
				break;
			case ALT_GROUNDROUTE:
				{
					AltObj->getObjName(objName);
					if (j == 0)
					{;
						hItem = m_wndTreeCtrl.InsertItem(_T("Taxi route :") + objName.GetIDString(),cni,FALSE,FALSE,hItem,TVI_LAST);
						m_wndTreeCtrl.SetItemData(hItem,(DWORD_PTR)m_pCurRoute);
						m_wndTreeCtrl.Expand(m_wndTreeCtrl.GetRootItem(),TVE_EXPAND);
					}
					else
					{
						hParent = hItem;
						hItem = m_wndTreeCtrl.InsertItem(_T("Taxi route :") + objName.GetIDString(),cni,FALSE,FALSE,hItem,TVI_LAST);
						m_wndTreeCtrl.SetItemData(hItem,(DWORD_PTR)m_pCurRoute->getRouteID(j));
						m_wndTreeCtrl.Expand(hParent,TVE_EXPAND);
					}
				}
				break;
			default:
				break;
			}
		}
	}
}

void CDlgDeiceQueue::DisplayDeicePad()
{
	m_wndListbox.ResetContent();
	ALTObjectID objName;
	ALTObject* AltObj = NULL;
	int nIndex = 0;
	for (int i = 0; i < m_QueueList->GetCount(); i++)
	{
		m_pCurDeiceItem = m_QueueList->GetItem(i);
		ALTObjectGroup altObjGroup;	
		CString strTitle = _T("");
		if(m_pCurDeiceItem->getDeicePadID() == -1)
			strTitle.Format(_T("All Deicepads"));
		else{
			altObjGroup.ReadData(m_pCurDeiceItem->getDeicePadID());
			strTitle = altObjGroup.getName().GetIDString();
		}
		nIndex = m_wndListbox.AddString(strTitle);
		m_wndListbox.SetItemData(nIndex,(DWORD_PTR)m_pCurDeiceItem);
	}
}
void CDlgDeiceQueue::OnTvnSelchangedTreeRoutepath(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
	HTREEITEM hItem = m_wndTreeCtrl.GetSelectedItem();
	if (hItem != NULL)
	{
		m_pCurRoute = (DeicePadQueueRoute*)m_wndTreeCtrl.GetItemData(GetParentItem(hItem,1));
	}
	OnChangeToolbarStatus();
}

void CDlgDeiceQueue::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	try
	{
		CADODatabase::BeginTransaction();
			m_QueueList->SaveData(m_nProjID);
			CADODatabase::CommitTransaction() ;
	}
	catch (CADOException e)
	{
		CADODatabase::RollBackTransation() ;
	}

	OnOK();
}
void CDlgDeiceQueue::OnBnClickedButtonSave()
{
	// TODO: Add your control notification handler code here
	try
	{
		CADODatabase::BeginTransaction();
		m_QueueList->SaveData(m_nProjID);
		CADODatabase::CommitTransaction() ;
	}
	catch (CADOException e)
	{
		CADODatabase::RollBackTransation() ;
	}
	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(FALSE);
}

void CDlgDeiceQueue::OnChangeToolbarStatus()
{
	if (m_wndListbox.GetCurSel() != LB_ERR)
	{
		m_wndDeicePadToolbar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_ADD,TRUE);
		m_wndDeicePadToolbar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_DEL,TRUE);
	}
	else
	{
		m_wndDeicePadToolbar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_ADD,FALSE);
		m_wndDeicePadToolbar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_DEL,FALSE);
	}
	if (m_pCurDeiceItem && m_wndTreeCtrl.GetSelectedItem() != NULL)
	{
		if (m_wndTreeCtrl.GetRootItem() == m_wndTreeCtrl.GetSelectedItem())
		{
			m_wndQueueToolbar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_QUEUE_ADD,TRUE);
			m_wndQueueToolbar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_QUEUE_DEL,FALSE);
		}
		else
		{
			m_wndQueueToolbar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_QUEUE_ADD,TRUE);
			m_wndQueueToolbar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_QUEUE_DEL,TRUE);
		}
	}
	else
	{
		m_wndQueueToolbar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_QUEUE_ADD,FALSE);
		m_wndQueueToolbar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_QUEUE_DEL,FALSE);
	}
}

int CDlgDeiceQueue::GetIndexByTree(HTREEITEM hItem)
{
	HTREEITEM hChileItem = m_wndTreeCtrl.GetChildItem(m_wndTreeCtrl.GetRootItem());
	int nIndex = 0;
	while (hChileItem)
	{
		if (hItem == hChileItem)
		{
			return nIndex;
		}
		hChileItem = m_wndTreeCtrl.GetNextSiblingItem(hChileItem);
		nIndex++;
	}
	return -1;
}

void CDlgDeiceQueue::setTreeItemText(HTREEITEM hItem,ALTObject* AltObj,CString strItemText)
{
	ASSERT(AltObj);
	switch(GetTreeCount(hItem))
	{
	case 0:
		break;
	case 1:
		{
			if (AltObj->GetType() == ALT_TAXIWAY)
			{
				m_wndTreeCtrl.SetItemText(hItem,_T("Taxiway :") + strItemText);
			}
			else if (AltObj->GetType() == ALT_GROUNDROUTE)
			{
				m_wndTreeCtrl.SetItemText(hItem,_T("Taxi route :") + strItemText);
			}
			m_pCurRoute = (DeicePadQueueRoute*)m_wndTreeCtrl.GetItemData(hItem);
		}
		break;
	default:
		{
			if (AltObj->GetType() == ALT_TAXIWAY)
			{
				m_wndTreeCtrl.SetItemText(hItem,_T("Taxiway :") + strItemText);
			}
			else if (AltObj->GetType() == ALT_GROUNDROUTE)
			{
				m_wndTreeCtrl.SetItemText(hItem,_T("Taxi route :") + strItemText);
			}
			m_pCurRoute->UpdataItem(GetTreeCount(hItem)-1,(int)m_wndTreeCtrl.GetItemData(hItem));
		}
		break;
	}
}