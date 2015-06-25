// DlgDeicePadRoute.cpp : implementation file
//

#include "stdafx.h"
//#include "AirsideGUI.h"
#include "resource.h"
#include "./InputAirside/PreferRouteList.h"
#include "./MFCExControl/CoolTree.h"
#include "./MFCExControl/ARCTreeCtrl.h"
#include "./InputAirside/ALTObject.h"
#include "./InputAirside/ALTAirport.h"
#include "./InputAirside/Taxiway.h"
#include "./InputAirside/GroundRoutes.h"
#include "./InputAirside/GroundRoute.h"
#include "../InputAirside/AirportGroundNetwork.h"
#include "./InputAirside/IntersectionNode.h"
#include "DlgDeicePadRoute.h"
#include ".\dlgdeicepadroute.h"


// CDlgDeicePadRoute dialog

IMPLEMENT_DYNAMIC(CDlgDeicePadRoute, CDialog)
CDlgDeicePadRoute::CDlgDeicePadRoute(int nProjID,int nAirportID,CWnd* pParent /*=NULL*/)
	: CDialog(CDlgDeicePadRoute::IDD, pParent)
{
	m_nProjID = nProjID;
	m_nAirportID = nAirportID;
	m_pRouteList = new CPreferRouteList();
	m_pCurRouteItem = NULL;
	m_pCurDeiceItem = NULL;
	m_pCurRoutePath = NULL;
}

CDlgDeicePadRoute::~CDlgDeicePadRoute()
{
	if (m_pRouteList)
	{
		delete m_pRouteList;
		m_pRouteList = NULL;
	}
}

void CDlgDeicePadRoute::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE_ROUTEPATH, m_wndTreeCtrl);
}


BEGIN_MESSAGE_MAP(CDlgDeicePadRoute, CDialog)
	ON_WM_CREATE()
	ON_BN_CLICKED(IDC_RADIO_ROUTE, OnBnClickedRadioPath)
	ON_BN_CLICKED(IDC_RADIO_PATH, OnBnClickedRadioRoute)
	ON_COMMAND(ID_TOOLBAR_ADD,OnNewStandAndRoutepathItem)
	ON_COMMAND(ID_TOOLBAR_DEL,OnDelStandAndRoutepathItem)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_ROUTEPATH, OnTvnSelchangedTreeItem)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDC_SAVE, OnBnClickedSave)
END_MESSAGE_MAP()


// CDlgDeicePadRoute message handlers

BOOL CDlgDeicePadRoute::OnInitDialog()
{
	CDialog::OnInitDialog();
	OnInitToolbar();
	OnInitTreeCtrl();
 
	m_emType = m_pRouteList->getPreferRoute();
	if (m_emType == PreferType)
	{
		CheckRadioButton(IDC_RADIO_PATH,IDC_RADIO_ROUTE,IDC_RADIO_ROUTE);
	}
	else
	{
		CheckRadioButton(IDC_RADIO_PATH,IDC_RADIO_ROUTE,IDC_RADIO_PATH);
	}

	m_bDClick = FALSE;
	ModifySize(m_emType);
	GetDlgItem(IDC_SAVE)->EnableWindow(FALSE);
	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),TRUE);
	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),FALSE);
	return TRUE;
}

int CDlgDeicePadRoute::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if(CDialog::OnCreate(lpCreateStruct) == -1)
	{
		return -1;
	}
	if (!m_wndStadToolbar.CreateEx(this,TBSTYLE_FLAT,WS_VISIBLE|WS_CHILD|CBRS_ALIGN_TOP|CBRS_TOOLTIPS)
		|| !m_wndStadToolbar.LoadToolBar(IDR_STAND_TOOLBAR))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}
	return 0;
}

void CDlgDeicePadRoute::OnInitToolbar()
{
	CRect rect;
	GetDlgItem(IDC_STATIC_STAND)->GetWindowRect(&rect);
	ScreenToClient(&rect);
	m_wndStadToolbar.MoveWindow(&rect,true);
	m_wndStadToolbar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_DEL,TRUE);
	m_wndStadToolbar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_ADD,TRUE);
	m_wndStadToolbar.ShowWindow(SW_SHOW);
}

void CDlgDeicePadRoute::ModifySize(RouteType emType)
{
	switch(emType)
	{
	case PreferType:
		PreferSize();
		break;
	case ShortType:
		ShortSize();
		break;
	default:
		break;
	}
}

void CDlgDeicePadRoute::PreferSize()
{
	m_wndTreeCtrl.ShowWindow(SW_SHOW);
	CRect rect;
	if (m_bDClick == TRUE)
	{
		GetClientRect(&rect);
		ClientToScreen(&rect);
		this->SetWindowPos(NULL,rect.left,rect.top,rect.Width()+12,rect.Height()*2+117,SWP_NOMOVE);
 		GetDlgItem(IDC_SAVE)->GetWindowRect(&rect);
 		ScreenToClient(&rect);
 		GetDlgItem(IDC_SAVE)->SetWindowPos(NULL,rect.left,(rect.top+10)*3,rect.Width(),rect.Height(),NULL);
 		GetDlgItem(IDOK)->GetWindowRect(&rect);
 		ScreenToClient(&rect);
 		GetDlgItem(IDOK)->SetWindowPos(NULL,rect.left,(rect.top+10)*3,rect.Width(),rect.Height(),NULL);
 		GetDlgItem(IDCANCEL)->GetWindowRect(&rect);
 		ScreenToClient(&rect);
 		GetDlgItem(IDCANCEL)->SetWindowPos(NULL,rect.left,(rect.top+10)*3,rect.Width(),rect.Height(),NULL);
	}
	m_wndStadToolbar.ShowWindow(SW_SHOW);
	m_bDClick = FALSE;
}

void CDlgDeicePadRoute::ShortSize()
{
	m_wndTreeCtrl.ShowWindow(SW_HIDE);
	m_wndStadToolbar.ShowWindow(SW_HIDE);
	CRect rect;
	if (m_bDClick == FALSE)
	{
		GetClientRect(&rect);
		ClientToScreen(&rect);
		this->SetWindowPos(NULL,rect.left,rect.top,rect.Width(),rect.Height()/2-10,SWP_NOMOVE);
  		GetDlgItem(IDC_SAVE)->GetWindowRect(&rect);
  		ScreenToClient(&rect);
  		GetDlgItem(IDC_SAVE)->SetWindowPos(NULL,rect.left,rect.top/3-10,rect.Width(),rect.Height(),NULL);
  		GetDlgItem(IDOK)->GetWindowRect(&rect);
  		ScreenToClient(&rect);
  		GetDlgItem(IDOK)->SetWindowPos(NULL,rect.left,rect.top/3-10,rect.Width(),rect.Height(),NULL);
  		GetDlgItem(IDCANCEL)->GetWindowRect(&rect);
  		ScreenToClient(&rect);
  		GetDlgItem(IDCANCEL)->SetWindowPos(NULL,rect.left,rect.top/3-10,rect.Width(),rect.Height(),NULL);
	}
	m_bDClick = TRUE;
}
void CDlgDeicePadRoute::OnBnClickedRadioPath()
{
	// TODO: Add your control notification handler code here
	m_emType = PreferType;
	ModifySize(m_emType);
}

void CDlgDeicePadRoute::OnBnClickedRadioRoute()
{
	// TODO: Add your control notification handler code here
	m_emType = ShortType;
	ModifySize(m_emType);
}

void CDlgDeicePadRoute::OnNewStandAndRoutepathItem()
{
	HTREEITEM hItem = m_wndTreeCtrl.GetSelectedItem();	
	HTREEITEM hchildItem = NULL;
	COOLTREE_NODE_INFO cni;
	ALTObjectList vObject;
	CCoolTree::InitNodeInfo(this,cni);
	cni.nt = NT_NORMAL;
	cni.net = NET_COMBOBOX;
	switch(GetTreeCount(hItem))
	{
	case 0:
		{
			//ALTObject::GetObjectList(ALT_STAND,m_nProjID,vObject);
			ALTAirport::GetStandList(m_nAirportID,vObject);
			if ((int)vObject.size() == 0)
			{
				::AfxMessageBox(_T("No stand to be selected"),MB_OK|MB_ICONQUESTION);
				return;
			}
			m_pCurRouteItem = new CPreferRouteItem();
			hchildItem = m_wndTreeCtrl.InsertItem(_T("Stand Family :"),cni,FALSE,FALSE,m_wndTreeCtrl.GetRootItem(),TVI_LAST);
			m_pRouteList->AddItem(m_pCurRouteItem);
			m_wndTreeCtrl.SetItemData(hchildItem,(DWORD_PTR)m_pCurRouteItem->getStandID());
		}
		break;
	case 1:
		{	
			//ALTObject::GetObjectList(ALT_DEICEBAY,m_nProjID,vObject);
			ALTAirport::GetDeicePadList(m_nAirportID,vObject);
			if ((int)vObject.size() == 0)
			{
				::AfxMessageBox(_T("No deice pad to be selected"),MB_OK|MB_ICONQUESTION);
				return;
			}
			m_pCurDeiceItem = new CDeicePadItem();
			hchildItem = m_wndTreeCtrl.InsertItem(_T("Deice Pad :"),cni,FALSE,FALSE,hItem,TVI_LAST);
			m_pCurRouteItem->AddItem(m_pCurDeiceItem);
			m_wndTreeCtrl.SetItemData(hchildItem,(DWORD_PTR)m_pCurDeiceItem->getDeicePadID());
		}
		break;
	case 2:
		{
			//ALTObject::GetObjectList(ALT_TAXIWAY,m_nProjID,vObject);
			ALTAirport::GetTaxiwayList(m_nAirportID,vObject);
			if ((int)vObject.size() == 0)
			{
				::AfxMessageBox(_T("No taxiway to be selected"),MB_OK|MB_ICONQUESTION);
				return;
			}
			m_pCurRoutePath = new CRoutePath();
			hchildItem = m_wndTreeCtrl.InsertItem(_T("Route :"),cni,FALSE,FALSE,hItem,TVI_LAST);
			m_pCurDeiceItem->AddItem(m_pCurRoutePath);
			m_pCurRoutePath->Additem(-1);
			m_wndTreeCtrl.SetItemData(hchildItem,(DWORD_PTR)m_pCurRoutePath);
		}
		break;
	default:
		{
			hchildItem = m_wndTreeCtrl.InsertItem(_T(""),cni,FALSE,FALSE,hItem,TVI_LAST);
			m_pCurRoutePath->Additem(-1);
		}
		break;

	}
	GetDlgItem(IDC_SAVE)->EnableWindow(TRUE);
	m_wndTreeCtrl.SetFocus();
	m_wndTreeCtrl.SelectItem(hchildItem);
	m_wndTreeCtrl.DoEdit(hchildItem);
	m_wndTreeCtrl.Expand(hItem,TVE_EXPAND);
}

void CDlgDeicePadRoute::OnDelStandAndRoutepathItem()
{
	HTREEITEM hItem = m_wndTreeCtrl.GetSelectedItem();
	HTREEITEM hParentITem = m_wndTreeCtrl.GetParentItem(hItem);
	int nObjID = (int)m_wndTreeCtrl.GetItemData(hItem);

	switch(GetTreeCount(hItem))
	{
	case 0:
		break;
	case 1:
		m_pRouteList->DeleteItemData(nObjID);
		break;
	case 2:
		m_pCurRouteItem->DeleteItemData(nObjID);
		break;
	case 3:
		{
			m_pCurRoutePath = (CRoutePath*)m_wndTreeCtrl.GetItemData(hItem);
			m_pCurDeiceItem->DeleteItem((CRoutePath*)m_wndTreeCtrl.GetItemData(hItem));
			while (m_wndTreeCtrl.GetChildItem(hItem) != NULL)
			{
				hItem = m_wndTreeCtrl.GetChildItem(hItem);
				m_wndTreeCtrl.DeleteItem(hItem);
			}
		}
		break;
	default:
		m_pCurRoutePath->DeleteItem(GetTreeCount(hItem)-3);
		break;
	}
	m_wndTreeCtrl.DeleteItem(hItem);
	m_wndTreeCtrl.SelectItem(hParentITem);
	if(m_wndTreeCtrl.GetRootItem() == hParentITem)
	{
		m_wndStadToolbar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_ADD,TRUE);
		m_wndStadToolbar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_DEL,FALSE);
	}
	else
	{
		m_wndStadToolbar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_ADD,TRUE);
		m_wndStadToolbar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_DEL,TRUE);
	}
	GetDlgItem(IDC_SAVE)->EnableWindow(TRUE);
}

void CDlgDeicePadRoute::OnInitTreeCtrl()
{
	COOLTREE_NODE_INFO cni;
	CCoolTree::InitNodeInfo(this,cni);
	cni.nt = NT_NORMAL;
	cni.net = NET_COMBOBOX;
	HTREEITEM hChildItem = NULL;
	HTREEITEM hCurItem = NULL;
	HTREEITEM hItem = NULL;
	HTREEITEM hParentItem = NULL;
	HTREEITEM hRouteItem = NULL;
	ALTObjectID objName;
	hChildItem = m_wndTreeCtrl.InsertItem(_T("Preferred route"),cni,FALSE,FALSE,TVI_ROOT,TVI_LAST);
	m_pRouteList->ReadData(m_nProjID);
	for (int nIndex = 0; nIndex < m_pRouteList->getPreferRouteCount(); nIndex++)
	{
		m_pCurRouteItem = m_pRouteList->GetItem(nIndex);
		ALTObject* pObj = ALTObject::ReadObjectByID(m_pCurRouteItem->getStandID());
 		if (pObj == NULL)
 		{
 			m_pRouteList->DeleteItem(nIndex);
 			nIndex--;
 			continue;
 		}
		pObj->getObjName(objName);
		hCurItem = m_wndTreeCtrl.InsertItem(_T("Stand Family :") + objName.GetIDString(),cni,FALSE,FALSE,hChildItem,TVI_LAST);
		m_wndTreeCtrl.SetItemData(hCurItem,(DWORD_PTR)m_pCurRouteItem->getStandID());
		for (int i = 0; i < m_pCurRouteItem->getDeicePadCount();i++)
		{
			hItem = hCurItem;
			m_pCurDeiceItem = m_pCurRouteItem->GetItem(i);
			ALTObject* pDeiceObj = ALTObject::ReadObjectByID(m_pCurDeiceItem->getDeicePadID());
 			if (pDeiceObj == NULL)
 			{
 				m_pCurRouteItem->DeleteItem(i);
 				i--;
 				continue;
 			}
			pDeiceObj->getObjName(objName);
			hItem = m_wndTreeCtrl.InsertItem(_T("Deice Pad :") + objName.GetIDString(),cni,FALSE,FALSE,hItem,TVI_LAST);
			m_wndTreeCtrl.Expand(hCurItem,TVE_EXPAND);
			m_wndTreeCtrl.SetItemData(hItem,(DWORD_PTR)m_pCurDeiceItem->getDeicePadID());
			for (int j = 0; j < m_pCurDeiceItem->GetCount(); j++)
			{
				hRouteItem = hItem;
				m_pCurRoutePath = m_pCurDeiceItem->GetItem(j);
				for (int k = 0 ; k < m_pCurRoutePath->getRouteIDListCount(); k++)
				{
					int nObjID = m_pCurRoutePath->getRouteID(k);
					ALTObject* pRouteObj = ALTObject::ReadObjectByID(m_pCurRoutePath->getRouteID(k));
 					if (pRouteObj == NULL)
 					{
 						for (int m = k; m < m_pCurRoutePath->getRouteIDListCount();m++)
 						{
 							m_pCurRoutePath->DeleteItem(m);
 						}
 						continue;
 					}
					switch(pRouteObj->GetType())
					{
					case ALT_TAXIWAY:
						{
							if (k == 0)
							{	
								hParentItem = hRouteItem;
								hRouteItem = m_wndTreeCtrl.InsertItem(_T("Route :") + CString(((Taxiway*)pRouteObj)->GetMarking().c_str()),cni,FALSE,FALSE,hRouteItem,TVI_LAST);
								m_wndTreeCtrl.SetItemData(hRouteItem,(DWORD_PTR)m_pCurRoutePath);
								m_wndTreeCtrl.Expand(hParentItem,TVE_EXPAND);
							}
							else
							{
								hParentItem = hRouteItem;
								hRouteItem = m_wndTreeCtrl.InsertItem(((Taxiway*)pRouteObj)->GetMarking().c_str(),cni,FALSE,FALSE,hRouteItem,TVI_LAST);
								m_wndTreeCtrl.SetItemData(hRouteItem,(DWORD_PTR)m_pCurRoutePath->getRouteID(k));
								m_wndTreeCtrl.Expand(hParentItem,TVE_EXPAND);
							}
						}
						break;
					case ALT_GROUNDROUTE:
						{
							pRouteObj->getObjName(objName);
							if (k == 0)
							{
								hParentItem = hRouteItem;
								hItem = m_wndTreeCtrl.InsertItem(_T("Route :") + objName.GetIDString(),cni,FALSE,FALSE,hRouteItem,TVI_LAST);
								m_wndTreeCtrl.SetItemData(hRouteItem,(DWORD_PTR)m_pCurRoutePath);
								m_wndTreeCtrl.Expand(hParentItem,TVE_EXPAND);
							}
							else
							{
								hParentItem = hRouteItem;
								hRouteItem = m_wndTreeCtrl.InsertItem(objName.GetIDString(),cni,FALSE,FALSE,hRouteItem,TVI_LAST);
								m_wndTreeCtrl.SetItemData(hRouteItem,(DWORD_PTR)m_pCurRoutePath->getRouteID(k));
								m_wndTreeCtrl.Expand(hParentItem,TVE_EXPAND);
							}
						}
						break;
					default:
						break;
					}
				}
			}
		}
	}
	m_wndTreeCtrl.SelectItem(m_wndTreeCtrl.GetRootItem());
	m_wndTreeCtrl.Expand(hChildItem,TVE_EXPAND);
}

LRESULT CDlgDeicePadRoute::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
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
		if (pCB->GetCount() == 0)
		{
			return CDialog::DefWindowProc(message,wParam,lParam);
		}

		int nObjID = -1;
		if (GetTreeCount((HTREEITEM)wParam) == 3)
		{
			m_pCurRoutePath = (CRoutePath*)m_wndTreeCtrl.GetItemData((HTREEITEM)wParam);
			nObjID = m_pCurRoutePath->getRouteID(0);
		}
		else
		{
			nObjID = m_wndTreeCtrl.GetItemData((HTREEITEM)wParam);
		}

 		if (-1 == nObjID)
 		{
			ALTObject* AltObj = NULL;
 			pCB->SetCurSel(0);
			if (GetTreeCount((HTREEITEM)wParam) == 3)
			{
				m_pCurRoutePath->UpdataItem(0,pCB->GetItemData(0));
				m_wndTreeCtrl.SetItemData((HTREEITEM)wParam,(DWORD_PTR)m_pCurRoutePath);
			}
			else
			{
				m_wndTreeCtrl.SetItemData((HTREEITEM)wParam,(DWORD_PTR)pCB->GetItemData(0));
			}
			AltObj = ALTObject::ReadObjectByID((int)pCB->GetItemData(0));
			CString strItemText = _T("");
			pCB->GetLBText(0,strItemText);
			SetTreeItemText((HTREEITEM)wParam,strItemText);
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
				ALTObject* AltObj = NULL;
				pCB->SetCurSel(0);
				if (GetTreeCount((HTREEITEM)wParam) == 3)
				{
					m_pCurRoutePath->UpdataItem(0,pCB->GetItemData(0));
					m_wndTreeCtrl.SetItemData((HTREEITEM)wParam,(DWORD_PTR)m_pCurRoutePath);
				}
				else
				{
					m_wndTreeCtrl.SetItemData((HTREEITEM)wParam,(DWORD_PTR)pCB->GetItemData(0));
				}
				AltObj = ALTObject::ReadObjectByID((int)pCB->GetItemData(0));
				CString strItemText = _T("");
				pCB->GetLBText(0,strItemText);
				SetTreeItemText((HTREEITEM)wParam,strItemText);
			}
 		}
		GetDlgItem(IDC_SAVE)->EnableWindow(TRUE);
	}
	else if(message == UM_CEW_COMBOBOX_SELCHANGE)
	{
		CString strItemText = m_wndTreeCtrl.GetItemText((HTREEITEM)wParam);
		CWnd* pWnd = m_wndTreeCtrl.GetEditWnd((HTREEITEM)wParam);
		CComboBox* pCB=(CComboBox*)pWnd;
		for (int i = 0; i < pCB->GetCount(); i++)
		{
			CString strComboxItemtext;
			pCB->GetLBText(i,strComboxItemtext);
			if (!strItemText.CompareNoCase(strComboxItemtext))
			{
				if (GetTreeCount((HTREEITEM)wParam) == 3)
				{
					m_pCurRoutePath->UpdataItem(0,pCB->GetItemData(i));
					m_wndTreeCtrl.SetItemData((HTREEITEM)wParam,(DWORD_PTR)m_pCurRoutePath);
				}
				else
				{
					m_wndTreeCtrl.SetItemData((HTREEITEM)wParam,(DWORD_PTR)pCB->GetItemData(i));
				}
				break;
			}
		}
		SetTreeItemText((HTREEITEM)wParam,strItemText);
	}
	else if(message == UM_CEW_COMBOBOX_SETWIDTH)
	{
		m_wndTreeCtrl.SetWidth(200);
	}
	else if (message == UM_CEW_COMBOBOX_END)
	{
		CString strItemText = m_wndTreeCtrl.GetItemText((HTREEITEM)wParam);
		SetTreeItemText((HTREEITEM)wParam,strItemText);
	}
	return CDialog::DefWindowProc(message, wParam, lParam);
}

void CDlgDeicePadRoute::SetToolbarStatus()
{
	HTREEITEM hItem = m_wndTreeCtrl.GetSelectedItem();

	if (hItem == NULL)
	{
		m_wndStadToolbar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_ADD,FALSE);
		m_wndStadToolbar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_DEL,FALSE);
	}
	else
	{
		switch(GetTreeCount(hItem))
		{
		case 0:
			{
				m_wndStadToolbar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_ADD,TRUE);
				m_wndStadToolbar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_DEL,FALSE);
			}
			break;
		default:
			{
				m_wndStadToolbar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_ADD,TRUE);
				m_wndStadToolbar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_DEL,TRUE);
			}
			break;
		}
	}
}
void CDlgDeicePadRoute::OnTvnSelchangedTreeItem(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	SetToolbarStatus();
	HTREEITEM hItem = m_wndTreeCtrl.GetSelectedItem();
	HTREEITEM hParentItem = m_wndTreeCtrl.GetParentItem(hItem);
	if (hItem == NULL)
	{
		return;
	}
	else
	{
		switch(GetTreeCount(hItem))
		{
		case 0:
			break;
		case 1:
			m_pCurRouteItem = m_pRouteList->GetItemData((int)m_wndTreeCtrl.GetItemData(hItem));
			break;
		case 2:
			{
				m_pCurRouteItem = m_pRouteList->GetItemData((int)m_wndTreeCtrl.GetItemData(hParentItem));
				m_pCurDeiceItem = m_pCurRouteItem->GetItemData((int)m_wndTreeCtrl.GetItemData(hItem));
			}
			break;
		case 3:
			{
				HTREEITEM hLitem = m_wndTreeCtrl.GetParentItem(hParentItem);
				m_pCurRouteItem = m_pRouteList->GetItemData((int)m_wndTreeCtrl.GetItemData(hLitem));
				m_pCurDeiceItem = m_pCurRouteItem->GetItemData((int)m_wndTreeCtrl.GetItemData(hParentItem));
				m_pCurRoutePath = (CRoutePath*)m_wndTreeCtrl.GetItemData(hItem);
			}
			break;
		default:
			{
				m_pCurRouteItem = m_pRouteList->GetItemData((int)m_wndTreeCtrl.GetItemData(GetParentItem(hItem,1)));
				m_pCurDeiceItem = m_pCurRouteItem->GetItemData((int)m_wndTreeCtrl.GetItemData(GetParentItem(hItem,2)));
				m_pCurRoutePath = (CRoutePath*)m_wndTreeCtrl.GetItemData(GetParentItem(hItem,3));
			}
			break;
		}
	}
	*pResult = 0;
}

void CDlgDeicePadRoute::SetComboBoxItem(CComboBox* pCB)
{
	HTREEITEM hItem = m_wndTreeCtrl.GetSelectedItem();
	ALTObject* altObject = NULL;
	ALTObjectList vObject;
	ALTObjectID objName;
	int nCount = 0;

	GroundRoutes* pRoute = new GroundRoutes(m_nAirportID);
	pRoute->ReadData();

	std::vector<int> vTaxiwayIDs;
	ALTAirport::GetTaxiwaysIDs(m_nAirportID, vTaxiwayIDs);

	switch(GetTreeCount(hItem))
	{
	case 0:
		break;
	case 1:
		{
			ALTAirport::GetStandList(m_nAirportID,vObject);
			//ALTObject::GetObjectList(ALT_STAND,m_nProjID,vObject);
			nCount = (int)vObject.size();
			for (int i = 0; i< nCount; i++)
			{
				altObject = vObject.at(i).get();
				altObject->getObjName(objName);
				int nItemIndex = pCB->AddString(objName.GetIDString());
				pCB->SetItemData(nItemIndex,(DWORD_PTR)altObject->getID());
			}
			if (pCB->GetCount() == 0)
			{
				m_pRouteList->DeleteItem(GetIndexByTree(hItem));
				m_wndTreeCtrl.DeleteItem(hItem);
				::AfxMessageBox(_T("No taxiway to be selected"),MB_ICONQUESTION|MB_OK);
			}
		}
		break;
	case 2:
		{
			//ALTObject::GetObjectList(ALT_DEICEBAY,m_nProjID,vObject);
			ALTAirport::GetDeicePadList(m_nAirportID,vObject);
			nCount = (int)vObject.size();
			for (int i = 0; i< nCount; i++)
			{
				altObject = vObject.at(i).get();
				altObject->getObjName(objName);
				int nItemIndex = pCB->AddString(objName.GetIDString());
				pCB->SetItemData(nItemIndex,(DWORD_PTR)altObject->getID());
			}
			if (pCB->GetCount() == 0)
			{
				m_pCurRouteItem->DeleteItem(GetIndexByTree(hItem));
				m_wndTreeCtrl.DeleteItem(hItem);
				::AfxMessageBox(_T("No taxiway to be selected"),MB_ICONQUESTION|MB_OK);
			}
		}
		break;
	case 3:
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
			if (pCB->GetCount() == 0)
			{
				m_pCurDeiceItem->DeleteItem(GetIndexByTree(hItem));
				m_wndTreeCtrl.DeleteItem(hItem);
				::AfxMessageBox(_T("No taxiway to be selected"),MB_ICONQUESTION|MB_OK);
			}
		}
		break;
	default:
		{
			GroundRouteVector grVector;
			CAirportGroundNetwork AirportNetWork(m_nProjID);
			IntersectionNodeList InNodeList;
			HTREEITEM hParentItem = m_wndTreeCtrl.GetParentItem(hItem);
			m_pCurRoutePath = (CRoutePath*)m_wndTreeCtrl.GetItemData(GetParentItem(hItem,3));
			ALTObject *AltObj = ALTObject::ReadObjectByID(m_pCurRoutePath->getRouteID(0));
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
					m_pCurRoutePath->DeleteItem(GetTreeCount(hItem)-3);
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
					m_pCurRoutePath->DeleteItem(GetTreeCount(hItem)-3);
					m_wndTreeCtrl.DeleteItem(hItem);
					::AfxMessageBox(_T("No taxiway to be selected"),MB_ICONQUESTION|MB_OK);
				}
			}
		}
		break;
	}
}

int CDlgDeicePadRoute::GetTreeCount(HTREEITEM hItem)
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

HTREEITEM CDlgDeicePadRoute::GetParentItem(HTREEITEM hItem,int lCount)
{
	if (GetTreeCount(hItem) <= lCount)
	{
		return NULL;
	}
	HTREEITEM hParentItem = m_wndTreeCtrl.GetParentItem(hItem);
	while (hParentItem && GetTreeCount(hParentItem) != lCount)
	{
		hParentItem = m_wndTreeCtrl.GetParentItem(hParentItem);
	}
	return hParentItem;
}
void CDlgDeicePadRoute::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	try
	{
		CADODatabase::BeginTransaction() ;
		m_pRouteList->SaveData(m_nProjID,m_emType);
		CADODatabase::CommitTransaction() ;

	}
	catch (CADOException e)
	{
		CADODatabase::RollBackTransation() ;
	}
	
	OnOK();
}

void CDlgDeicePadRoute::OnBnClickedSave()
{
	// TODO: Add your control notification handler code here
	try
	{
		CADODatabase::BeginTransaction() ;
		m_pRouteList->SaveData(m_nProjID,m_emType);
		CADODatabase::CommitTransaction() ;

	}
	catch (CADOException e)
	{
		CADODatabase::RollBackTransation() ;
	}
	GetDlgItem(IDC_SAVE)->EnableWindow(FALSE);
}

int CDlgDeicePadRoute::GetIndexByTree(HTREEITEM hItem)
{
	HTREEITEM hChildItem = m_wndTreeCtrl.GetChildItem(m_wndTreeCtrl.GetParentItem(hItem));
	int nIndex = 0;
	while (hChildItem)
	{
		if (hItem == hChildItem)
		{
			return nIndex;
		}
		hChildItem = m_wndTreeCtrl.GetNextSiblingItem(hChildItem);
		nIndex++;
	}
	return -1;
}

void CDlgDeicePadRoute::SetTreeItemText(HTREEITEM hItem,CString strItemText)
{
	switch(GetTreeCount(hItem))
	{
	case 0:
		break;
	case 1:
		{
			m_wndTreeCtrl.SetItemText(hItem,_T("Stand Family :") + strItemText);
			m_pCurRouteItem->setStandID((int)m_wndTreeCtrl.GetItemData(hItem));
		}
		break;
	case 2:
		{
			m_wndTreeCtrl.SetItemText(hItem,_T("Deice Pad :") + strItemText);
			m_pCurDeiceItem->setDeicePadID((int)m_wndTreeCtrl.GetItemData(hItem));
		}
		break;
	case 3:
		{
			m_wndTreeCtrl.SetItemText(hItem,_T("Route :") + strItemText);
			m_pCurRoutePath = (CRoutePath*)m_wndTreeCtrl.GetItemData(hItem);
		}
		break;
	default:
		{
			m_wndTreeCtrl.SetItemText(hItem, strItemText);
			m_pCurRoutePath->UpdataItem(GetTreeCount(hItem)-3,(int)m_wndTreeCtrl.GetItemData(hItem));
		}
		break;
	}
}