#include "stdafx.h"
#include "DlgGroundRoutes.h"
#include "../InputAirside/InputAirside.h"
#include "../InputAirside/ALTAirport.h"
#include "../InputAirside/Runway.h"
#include "../InputAirside/Taxiway.h"
#include "../InputAirside/GroundRoute.h"
#include ".\dlggroundroutes.h"
#include "../InputAirside/GroundRoutes.h"

namespace
{
const UINT ID_NEW_GROUNDROUTE = 10;
const UINT ID_DEL_GROUNDROUTE = 11;

const UINT ID_NEW_GROUNDROUTEITEM  = 20;
const UINT ID_DEL_GROUNDROUTEITEM  = 21;
const UINT ID_EDIT_GROUNDROUTEITEM = 22;
}

CDlgGroundRoutes::CDlgGroundRoutes(int nProjectID, int nAirportID, CWnd* pParent /*=NULL*/)
 : CXTResizeDialog(IDD, pParent)
 , m_nProjectID(nProjectID)
 , m_nAirPortID(nAirportID)
 , m_pCurGroundRoute(NULL)
 , m_groundRoutes(NULL)
 , m_curParentItem(NULL)
{
	m_groundRoutes = new GroundRoutes(m_nAirPortID);
	m_groundRoutes->ReadData();

	try
	{
		CADODatabase::BeginTransaction();
		m_groundRoutes->VerifyData();
		CADODatabase::CommitTransaction();

	}
	catch (CADOException &e)
	{
		CADODatabase::RollBackTransation();
		e.ErrorMessage();
	}
}

CDlgGroundRoutes::~CDlgGroundRoutes()
{
	if (NULL != m_groundRoutes)
	{
		delete m_groundRoutes;
	}
}

void CDlgGroundRoutes::DoDataExchange(CDataExchange* pDX)
{
	CXTResizeDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE_GROUNDROUTEITEM, m_wndTreeGroundRouteItem);
	DDX_Control(pDX, IDC_LISTBOX_GROUNDROUTE, m_wndGroundRouteListBox);
}

BEGIN_MESSAGE_MAP(CDlgGroundRoutes, CXTResizeDialog)
	ON_BN_CLICKED(IDC_BUTTON_SAVE, OnSave)
	ON_WM_CREATE()
	ON_COMMAND(ID_NEW_GROUNDROUTE, OnNewGroundRoute)
	ON_COMMAND(ID_DEL_GROUNDROUTE, OnDelGroundRoute)
	ON_COMMAND(ID_NEW_GROUNDROUTEITEM, OnNewGroundRouteItem)
	ON_COMMAND(ID_DEL_GROUNDROUTEITEM, OnDelGroundRouteItem)
	ON_COMMAND(ID_EDIT_GROUNDROUTEITEM, OnEditGroundRouteItem)
	ON_WM_DESTROY()
	ON_LBN_SELCHANGE(IDC_LISTBOX_GROUNDROUTE, OnLbnSelchangeListboxGroundroute)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_GROUNDROUTEITEM, OnTvnSelchangedGroundRouteItem)
	ON_MESSAGE(WM_LBITEMCHANGED,OnMessageModifyGroundRouteName)
	ON_WM_CONTEXTMENU()
END_MESSAGE_MAP()

int CDlgGroundRoutes::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CXTResizeDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!m_wndGroundRouteItemToolbar.CreateEx(this, TBSTYLE_FLAT,WS_CHILD|WS_VISIBLE|CBRS_ALIGN_TOP)
		|| !m_wndGroundRouteItemToolbar.LoadToolBar(IDR_TOOLBAR_ADD_DEL_EDIT))
	{
		return -1;
	}

	CToolBarCtrl& toolbar = m_wndGroundRouteItemToolbar.GetToolBarCtrl();
	toolbar.SetCmdID(0, ID_NEW_GROUNDROUTEITEM);
	toolbar.SetCmdID(1, ID_DEL_GROUNDROUTEITEM);
	toolbar.SetCmdID(2, ID_EDIT_GROUNDROUTEITEM);

	if (!m_wndGroundRouteToolbar.CreateEx(this, TBSTYLE_FLAT,WS_CHILD|WS_VISIBLE|CBRS_ALIGN_TOP)
		|| !m_wndGroundRouteToolbar.LoadToolBar(IDR_TOOLBAR_ADD_DEL))
	{
		return -1;
	}

	CToolBarCtrl& groundRouteToolbar = m_wndGroundRouteToolbar.GetToolBarCtrl();
	groundRouteToolbar.SetCmdID(0, ID_NEW_GROUNDROUTE);
	groundRouteToolbar.SetCmdID(1, ID_DEL_GROUNDROUTE);

	return 0;
}

BOOL CDlgGroundRoutes::OnInitDialog()
{
	CXTResizeDialog::OnInitDialog();

	InitToolbar();

	m_wndTreeGroundRouteItem.DeleteAllItems();
	InitGroundRoutesListBox();

	GetDlgItem(IDC_SAVE)->EnableWindow(FALSE);

	SetResize(m_wndGroundRouteToolbar.GetDlgCtrlID(), SZ_TOP_LEFT, SZ_TOP_LEFT);
	SetResize(m_wndGroundRouteItemToolbar.GetDlgCtrlID(), SZ_TOP_CENTER, SZ_TOP_CENTER);

	SetResize(IDC_STATIC_GROUNDROUTE, SZ_TOP_LEFT, SZ_BOTTOM_CENTER);
	SetResize(IDC_STATIC_GROUNDROUTEITEM, SZ_TOP_CENTER , SZ_BOTTOM_RIGHT);
	SetResize(IDC_LISTBOX_GROUNDROUTE, SZ_TOP_LEFT, SZ_BOTTOM_CENTER);
	SetResize(IDC_TREE_GROUNDROUTEITEM, SZ_TOP_CENTER , SZ_BOTTOM_RIGHT);
	SetResize(IDC_BUTTON_SAVE, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);
	SetResize(IDOK, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);
	SetResize(IDCANCEL, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);
	//SetResize(IDC_STATIC_INCLUDE_ALL, SZ_TOP_LEFT, SZ_BOTTOM_RIGHT);

	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),TRUE);
	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),FALSE);

	return TRUE;
}

void CDlgGroundRoutes::InitToolbar()
{
	// set the position of the toolbar
	CRect rectToolbar;
	m_wndTreeGroundRouteItem.GetWindowRect(&rectToolbar);
	ScreenToClient(&rectToolbar);
	rectToolbar.top -= 24;
	rectToolbar.bottom = rectToolbar.top + 22;
	rectToolbar.left += 4;
	m_wndGroundRouteItemToolbar.MoveWindow(&rectToolbar);

	m_wndGroundRouteItemToolbar.GetToolBarCtrl().EnableButton(ID_NEW_GROUNDROUTEITEM, FALSE);
	m_wndGroundRouteItemToolbar.GetToolBarCtrl().EnableButton(ID_DEL_GROUNDROUTEITEM, FALSE);
	m_wndGroundRouteItemToolbar.GetToolBarCtrl().EnableButton(ID_EDIT_GROUNDROUTEITEM, FALSE);

	m_wndGroundRouteListBox.GetWindowRect(&rectToolbar);
	ScreenToClient(&rectToolbar);
	rectToolbar.top -= 24;
	rectToolbar.bottom = rectToolbar.top + 22;
	rectToolbar.left += 4;
	m_wndGroundRouteToolbar.MoveWindow(&rectToolbar);
	m_wndGroundRouteToolbar.GetToolBarCtrl().EnableButton(ID_NEW_GROUNDROUTE);
	m_wndGroundRouteToolbar.GetToolBarCtrl().EnableButton(ID_DEL_GROUNDROUTE, FALSE);
}

void CDlgGroundRoutes::UpdateGroundRouteItemToolBar(BOOL bGroundRouteChange)
{
	if (LB_ERR != m_wndGroundRouteListBox.GetCurSel() && !bGroundRouteChange)
	{
		//the root item is selected
		if (NULL != m_wndTreeGroundRouteItem.GetSelectedItem()
			&& m_wndTreeGroundRouteItem.GetSelectedItem() == m_wndTreeGroundRouteItem.GetRootItem())
		{
			//the root item have child item
			if (NULL != m_wndTreeGroundRouteItem.GetChildItem(m_wndTreeGroundRouteItem.GetRootItem()))
			{
				m_wndGroundRouteItemToolbar.GetToolBarCtrl().EnableButton(ID_NEW_GROUNDROUTEITEM, FALSE);
				m_wndGroundRouteItemToolbar.GetToolBarCtrl().EnableButton(ID_DEL_GROUNDROUTEITEM, FALSE);
				m_wndGroundRouteItemToolbar.GetToolBarCtrl().EnableButton(ID_EDIT_GROUNDROUTEITEM, FALSE);
			}
			//the root item haven't child item
			else
			{
				m_wndGroundRouteItemToolbar.GetToolBarCtrl().EnableButton(ID_NEW_GROUNDROUTEITEM, TRUE);
				m_wndGroundRouteItemToolbar.GetToolBarCtrl().EnableButton(ID_DEL_GROUNDROUTEITEM, FALSE);
				m_wndGroundRouteItemToolbar.GetToolBarCtrl().EnableButton(ID_EDIT_GROUNDROUTEITEM, FALSE);
			}
			
		}
		else
		{
			//selected child item
			if (NULL != m_wndTreeGroundRouteItem.GetSelectedItem())
			{
				m_wndGroundRouteItemToolbar.GetToolBarCtrl().EnableButton(ID_NEW_GROUNDROUTEITEM, TRUE);
				m_wndGroundRouteItemToolbar.GetToolBarCtrl().EnableButton(ID_DEL_GROUNDROUTEITEM, TRUE);
				m_wndGroundRouteItemToolbar.GetToolBarCtrl().EnableButton(ID_EDIT_GROUNDROUTEITEM, TRUE);
			}
			//nothing selected
			else
			{
				m_wndGroundRouteItemToolbar.GetToolBarCtrl().EnableButton(ID_NEW_GROUNDROUTEITEM, FALSE);
				m_wndGroundRouteItemToolbar.GetToolBarCtrl().EnableButton(ID_DEL_GROUNDROUTEITEM, FALSE);
				m_wndGroundRouteItemToolbar.GetToolBarCtrl().EnableButton(ID_EDIT_GROUNDROUTEITEM, FALSE);
			}			
		}
	}
	else
	{
		m_wndGroundRouteItemToolbar.GetToolBarCtrl().EnableButton(ID_NEW_GROUNDROUTEITEM, FALSE);
		m_wndGroundRouteItemToolbar.GetToolBarCtrl().EnableButton(ID_DEL_GROUNDROUTEITEM, FALSE);
		m_wndGroundRouteItemToolbar.GetToolBarCtrl().EnableButton(ID_EDIT_GROUNDROUTEITEM, FALSE);
	}
}

void CDlgGroundRoutes::InitGroundRoutesListBox()
{
	m_wndGroundRouteListBox.ResetContent();

	int nCount = m_groundRoutes->GetCount();
	for (int i=0; i<nCount; i++)
	{
		GroundRoute *pGroundRoute = m_groundRoutes->GetItem(i);

		int nItem = m_wndGroundRouteListBox.AddString(pGroundRoute->GetObjNameString());
		m_wndGroundRouteListBox.SetItemData(nItem, (DWORD_PTR)pGroundRoute);
	}

}

void CDlgGroundRoutes::OnNewGroundRoute()
{
	m_pCurGroundRoute = new GroundRoute(m_groundRoutes->GetAirportID());
	m_groundRoutes->AddItem(m_pCurGroundRoute);
	

	int nGrountRouteCount = m_wndGroundRouteListBox.GetCount();
	CString strGrountRouteName;
	strGrountRouteName.Format("New Ground Route %d", nGrountRouteCount + 1);

	int nItem = m_wndGroundRouteListBox.AddString(strGrountRouteName);
	m_wndGroundRouteListBox.SetCurSel(nItem);
	m_wndGroundRouteListBox.SetItemData(nItem, (DWORD_PTR)m_pCurGroundRoute);
	m_wndGroundRouteListBox.EditItem(nItem);

	m_pCurGroundRoute->SetName(strGrountRouteName);

	m_wndGroundRouteToolbar.GetToolBarCtrl().EnableButton(ID_DEL_GROUNDROUTE, TRUE);

	UpdateGroundRouteItemTree();
	UpdateGroundRouteItemToolBar(TRUE);

	GetDlgItem(IDC_SAVE)->EnableWindow(TRUE);
}

void CDlgGroundRoutes::OnDelGroundRoute()
{
	ASSERT(m_pCurGroundRoute);

	if (0 < m_wndGroundRouteListBox.GetCount())
	{
		int nSel = m_wndGroundRouteListBox.GetCurSel();
		if (LB_ERR != nSel)
		{
			m_groundRoutes->DeleteItem(m_pCurGroundRoute);
			int nCount = m_wndGroundRouteListBox.DeleteString(nSel);

			if (0 == nCount)
			{
				m_wndGroundRouteToolbar.GetToolBarCtrl().EnableButton(ID_DEL_GROUNDROUTE, FALSE);
				m_pCurGroundRoute = NULL;
			}
			else
			{
				if (nCount-1 < nSel)
				{
					m_wndGroundRouteListBox.SetCurSel(nCount-1);
					m_pCurGroundRoute = (GroundRoute*)m_wndGroundRouteListBox.GetItemData(nCount-1);
				}
				else
				{
					int i = m_wndGroundRouteListBox.SetCurSel(nSel);
					m_pCurGroundRoute = (GroundRoute*)m_wndGroundRouteListBox.GetItemData(nSel);
				}
				m_wndGroundRouteToolbar.GetToolBarCtrl().EnableButton(ID_DEL_GROUNDROUTE, TRUE);
			}
		}
	}

	UpdateGroundRouteItemToolBar(TRUE);
	UpdateGroundRouteItemTree();

	GetDlgItem(IDC_SAVE)->EnableWindow(TRUE);
}

void CDlgGroundRoutes::OnNewGroundRouteItem()
{
	HTREEITEM hSelItem = m_wndTreeGroundRouteItem.GetSelectedItem();	

	if (NULL == hSelItem)
	{
		return;
	}

	m_curParentItem = hSelItem;

	COOLTREE_NODE_INFO cni;
	CCoolTree::InitNodeInfo(this,cni);
	cni.net=NET_COMBOBOX;
	cni.nt=NT_NORMAL;

	CString strRouteItem;
	strRouteItem.Format("No route item");
	HTREEITEM hInsertItem = m_wndTreeGroundRouteItem.InsertItem(strRouteItem, cni, FALSE, FALSE, m_curParentItem);

	m_wndTreeGroundRouteItem.Expand(m_curParentItem, TVE_EXPAND);

	m_wndTreeGroundRouteItem.DoEdit(hInsertItem);

	CWnd* pWnd = m_wndTreeGroundRouteItem.GetEditWnd(hInsertItem);
	CComboBox* pCB=(CComboBox*)pWnd;

	CString strDefaultRouteItem;
	if (0 < pCB->GetCount()
		&& pCB->GetCurSel() == 0)
	{
		pCB->GetLBText(0, strDefaultRouteItem);

		m_wndTreeGroundRouteItem.SetItemText(hInsertItem, strDefaultRouteItem);
		AddGroundRouteItem();
	}

	GetDlgItem(IDC_SAVE)->EnableWindow(TRUE);
}

void CDlgGroundRoutes::OnDelGroundRouteItem()
{
	HTREEITEM hSelItem = m_wndTreeGroundRouteItem.GetSelectedItem();
	GroundRouteItem *pGroundRouteItem = (GroundRouteItem*)m_wndTreeGroundRouteItem.GetItemData(hSelItem);
	ASSERT(pGroundRouteItem);

	HTREEITEM hParentItem = m_wndTreeGroundRouteItem.GetParentItem(hSelItem);

	if (hParentItem == m_wndTreeGroundRouteItem.GetRootItem())
	{
		ASSERT(m_pCurGroundRoute);
		m_pCurGroundRoute->DeleteContent();
	}
	else
	{
		GroundRouteItem *pParentGroundRouteItem = (GroundRouteItem*)m_wndTreeGroundRouteItem.GetItemData(hParentItem);
		ASSERT(pParentGroundRouteItem);
		pParentGroundRouteItem->DeleteChildItem(pGroundRouteItem);
	}

	m_wndTreeGroundRouteItem.DeleteItem(hSelItem);
	m_wndTreeGroundRouteItem.SelectItem(hParentItem);

	UpdateGroundRouteItemToolBar(FALSE);

	GetDlgItem(IDC_SAVE)->EnableWindow(TRUE);
}

void CDlgGroundRoutes::OnEditGroundRouteItem()
{
	HTREEITEM hSelItem = m_wndTreeGroundRouteItem.GetSelectedItem();
	ASSERT(hSelItem);

	m_wndTreeGroundRouteItem.DoEdit(hSelItem);
}

void CDlgGroundRoutes::OnSave()
{
	//Check if the route if a circle
	for (int i=0; i<m_groundRoutes->GetCount(); i++)
	{
		GroundRoute *pGroundRoute = m_groundRoutes->GetItem(i);

		GroundRouteItem *pFirstItem = pGroundRoute->GetFirstItem();

		if (NULL != pFirstItem)
		{
			GroundRouteItemVector groundRouteTails = pGroundRoute->GetTailItems();

			GroundRouteItemIter iter = groundRouteTails.begin();
			for (; iter!=groundRouteTails.end(); iter++)
			{
				GroundRouteItem *pTailItem = *iter;
				if (pTailItem->GetALTObjectID() == pFirstItem->GetALTObjectID()
					&& pTailItem->GetRunwayMarking() == pFirstItem->GetRunwayMarking()
					&& pTailItem != pFirstItem)
				{
					AfxMessageBox(_T("Can't define route as a circle!"), MB_OK|MB_ICONEXCLAMATION);

					return;
				}
			}
		}
	}

	try
	{
		CADODatabase::BeginTransaction();
		m_groundRoutes->SaveData();
		CADODatabase::CommitTransaction();

	}
	catch (CADOException &e)
	{
		CADODatabase::RollBackTransation();
		e.ErrorMessage();
	}

	GetDlgItem(IDC_SAVE)->EnableWindow(FALSE);
}

void CDlgGroundRoutes::OnOK()
{
	OnSave();
	CXTResizeDialog::OnOK();	
}

void CDlgGroundRoutes::OnCancel()
{
	CXTResizeDialog::OnCancel();
}

void CDlgGroundRoutes::OnDestroy()
{
	CXTResizeDialog::OnDestroy();
}

void CDlgGroundRoutes::OnLbnSelchangeListboxGroundroute()
{
	// TODO: Add your control notification handler code here
	int nCurSel = m_wndGroundRouteListBox.GetCurSel();

	m_pCurGroundRoute = (GroundRoute*)m_wndGroundRouteListBox.GetItemData(nCurSel);

	if (LB_ERR != nCurSel)
	{
		m_wndGroundRouteToolbar.GetToolBarCtrl().EnableButton(ID_DEL_GROUNDROUTE, TRUE);
	}
	else
	{
		m_wndGroundRouteToolbar.GetToolBarCtrl().EnableButton(ID_DEL_GROUNDROUTE, FALSE);
	}

	UpdateGroundRouteItemTree();
	UpdateGroundRouteItemToolBar(TRUE);
}

void CDlgGroundRoutes::UpdateGroundRouteItemTree()
{
	m_wndTreeGroundRouteItem.DeleteAllItems();
	m_wndTreeGroundRouteItem.SelectItem(NULL);

	if (m_wndGroundRouteListBox.GetCount() <= 0)
	{
		return;
	}

	ASSERT(m_pCurGroundRoute);

	COOLTREE_NODE_INFO cni;
	CCoolTree::InitNodeInfo(this,cni);

	HTREEITEM hItem = m_wndTreeGroundRouteItem.InsertItem(_T("Start"), cni, FALSE);

	cni.nt=NT_NORMAL;
	cni.net=NET_COMBOBOX;

	GroundRouteItem *pGroundRouteItem = m_pCurGroundRoute->GetFirstItem();

	if (NULL == pGroundRouteItem)
	{
		return;
	}

	AddGroundRouteItem(pGroundRouteItem, hItem);
	m_wndTreeGroundRouteItem.Expand(hItem, TVE_EXPAND);
}

void CDlgGroundRoutes::AddGroundRouteItem(GroundRouteItem *pGroundRouteItem, HTREEITEM hParentItem)
{
	ASSERT(pGroundRouteItem);

	COOLTREE_NODE_INFO cni;
	CCoolTree::InitNodeInfo(this,cni);
	cni.nt=NT_NORMAL;
	cni.net=NET_COMBOBOX;

	int nALTObjID = pGroundRouteItem->GetALTObjectID();
	ALTObject altObj;
	ALTObject *pALTObj = altObj.ReadObjectByID(nALTObjID);
	ASSERT(pALTObj);

	CString strALTObjName;
	if (pALTObj->GetType() == ALT_RUNWAY)
	{
		Runway runway;
		runway.ReadObject(nALTObjID);

		RUNWAY_MARK runwayMark = (RUNWAY_MARK)pGroundRouteItem->GetRunwayMarking();
		if (RUNWAYMARK_FIRST == runwayMark)
		{
			strALTObjName.Format("Runway:%s", runway.GetMarking1().c_str());
		}
		else if (RUNWAYMARK_SECOND == runwayMark)
		{
			strALTObjName.Format("Runway:%s", runway.GetMarking2().c_str());
		}
		else
		{
			ASSERT(FALSE);
		}

	}
	else if (pALTObj->GetType() == ALT_TAXIWAY)
	{
		Taxiway taxiway;
		taxiway.ReadObject(nALTObjID);

		strALTObjName.Format("Taxiway:%s", taxiway.GetMarking().c_str());
	}
	else
	{
		ASSERT(FALSE);
	}

	HTREEITEM hItem = m_wndTreeGroundRouteItem.InsertItem(strALTObjName, cni, FALSE, FALSE, hParentItem);
	m_wndTreeGroundRouteItem.SetItemData(hItem, (DWORD)pGroundRouteItem);

	GroundRouteItemVector childItems = pGroundRouteItem->GetChildItems();
	GroundRouteItemIter iter = childItems.begin();
	for (; iter!=childItems.end(); iter++)
	{
		GroundRouteItem *childItem = *iter;
		AddGroundRouteItem(childItem, hItem);
		m_wndTreeGroundRouteItem.Expand(hItem, TVE_EXPAND);
	}
}

void CDlgGroundRoutes::OnTvnSelchangedGroundRouteItem(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	UpdateGroundRouteItemToolBar(FALSE);

	HTREEITEM hItem = m_wndTreeGroundRouteItem.GetSelectedItem();

	if (hItem == m_wndTreeGroundRouteItem.GetRootItem())
	{
		m_curParentItem = hItem;
	}
	else
	{
		m_curParentItem = m_wndTreeGroundRouteItem.GetParentItem(hItem);
	}

	*pResult = 0;
}

LRESULT CDlgGroundRoutes::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	if (NULL == m_pCurGroundRoute)
	{
		return CDialog::DefWindowProc(message, wParam, lParam);
	}

	// TODO: Add your specialized code here and/or call the base class
	if(message == UM_CEW_COMBOBOX_BEGIN)	
	{
		CWnd* pWnd = m_wndTreeGroundRouteItem.GetEditWnd((HTREEITEM)wParam);
		CRect rectWnd;
		m_wndTreeGroundRouteItem.GetItemRect((HTREEITEM)wParam,rectWnd,TRUE);
		pWnd->SetWindowPos(NULL,rectWnd.right,rectWnd.top,0,0,SWP_NOSIZE);
		CComboBox* pCB=(CComboBox*)pWnd;		

		//Clear comboBox Item
		if(pCB->GetCount()!=0)
		{
			pCB->ResetContent();
		}

		SetComboBoxItem(pCB);

		GroundRouteItem *pGroundRouteItem = (GroundRouteItem*)m_wndTreeGroundRouteItem.GetItemData((HTREEITEM)wParam);
		CString strItemText = m_wndTreeGroundRouteItem.GetItemText((HTREEITEM)wParam);

		if (NULL == pGroundRouteItem)
		{
			pCB->SetCurSel(0);
		}
		else
		{
			int i;
			for (i=0; i<pCB->GetCount(); i++)
			{
				int nALTObjID = (int)pCB->GetItemData(i);
				if (nALTObjID == pGroundRouteItem->GetALTObjectID())
				{
					CString strCoboBoxItemText;
					pCB->GetLBText(i, strCoboBoxItemText);

					if (!strItemText.CompareNoCase(strCoboBoxItemText))
					{
						pCB->SetCurSel(i);

						break;
					}
				}
			}

			if (i==pCB->GetCount())
			{
				pCB->SetCurSel(0);
			}
		}
	}
	else if(message == UM_CEW_COMBOBOX_SELCHANGE)
	{
		AddGroundRouteItem();
	}
	else if(message == UM_CEW_COMBOBOX_SETWIDTH)
	{
		m_wndTreeGroundRouteItem.SetWidth(200);
	}

	return CDialog::DefWindowProc(message, wParam, lParam);
}

void CDlgGroundRoutes::AddGroundRouteItem()
{
	HTREEITEM hItemSeled =  m_wndTreeGroundRouteItem.GetSelectedItem();
	int nObjID = m_wndTreeGroundRouteItem.GetCurSelCmbBoxItemData(hItemSeled);

	CString strItemText = m_wndTreeGroundRouteItem.GetItemText(hItemSeled);
	CString strObjName;
	CString strMarking;
	int nPos = strItemText.Find(':');
	int nLength = strItemText.GetLength();
	strObjName = strItemText.Left(nPos);
	strMarking = strItemText.Right(nLength - nPos - 1);

	GroundRouteItem *pGroundRouteItem = (GroundRouteItem*)m_wndTreeGroundRouteItem.GetItemData(hItemSeled);

	BOOL bIsNewItem = FALSE;
	if (NULL == pGroundRouteItem)
	{
		bIsNewItem = TRUE;
		pGroundRouteItem = new GroundRouteItem();
	}

	int nOldObjID = pGroundRouteItem->GetALTObjectID();
	int nOldMark  = pGroundRouteItem->GetRunwayMarking();

	pGroundRouteItem->SetALTObjectID(nObjID);
	//runway
	if (!strObjName.CompareNoCase("Runway"))
	{
		Runway runway;
		runway.ReadObject(nObjID);

		if (!strMarking.CompareNoCase(runway.GetMarking1().c_str()))
		{
			pGroundRouteItem->SetRunwayMarking((int)RUNWAYMARK_FIRST);
		}

		if (!strMarking.CompareNoCase(runway.GetMarking2().c_str()))
		{
			pGroundRouteItem->SetRunwayMarking((int)RUNWAYMARK_SECOND);
		}
	}

	if (bIsNewItem)
	{
		m_wndTreeGroundRouteItem.SetItemData(hItemSeled, (DWORD)pGroundRouteItem);

		HTREEITEM hParentItem = m_wndTreeGroundRouteItem.GetParentItem(hItemSeled);

		ASSERT(hParentItem);
		if (hParentItem == m_wndTreeGroundRouteItem.GetRootItem())
		{
			ASSERT(m_pCurGroundRoute);
			m_pCurGroundRoute->SetFirstItem(pGroundRouteItem);
		}
		else
		{
			GroundRouteItem *pParentGroundRouteItem = (GroundRouteItem*)m_wndTreeGroundRouteItem.GetItemData(hParentItem);
			ASSERT(pParentGroundRouteItem);
			pParentGroundRouteItem->AddChildItem(pGroundRouteItem);
		}
	}
	//edit
	else
	{
		//delete the child of the modify item
		if (nOldObjID != pGroundRouteItem->GetALTObjectID()
			|| nOldMark != pGroundRouteItem->GetRunwayMarking())
		{
			HTREEITEM hNextItem;
			HTREEITEM hChildItem = m_wndTreeGroundRouteItem.GetChildItem(hItemSeled);

			while (hChildItem != NULL)
			{
				hNextItem = m_wndTreeGroundRouteItem.GetNextItem(hChildItem, TVGN_NEXT);

				GroundRouteItem *pChildGroundRouteItem = (GroundRouteItem*)m_wndTreeGroundRouteItem.GetItemData(hChildItem);
				ASSERT(pChildGroundRouteItem);
				pGroundRouteItem->DeleteChildItem(pChildGroundRouteItem);

				m_wndTreeGroundRouteItem.DeleteItem(hChildItem);
				hChildItem = hNextItem;
			}

			GetDlgItem(IDC_SAVE)->EnableWindow(TRUE);
		}
	}
}

//return TRUE:Exist, return FALSE: doesn't exist
BOOL CDlgGroundRoutes::IsIntersectObjExistInComboBox(CComboBox* pCB, int nObjID)
{
	ASSERT(pCB);

	for (int i=0; i<pCB->GetCount(); i++)
	{
		int nExistObjID = (int)pCB->GetItemData(i);

		if (nExistObjID == nObjID)
		{
			return TRUE;
		}
	}

	return FALSE;
}
void CDlgGroundRoutes::SetComboBoxItem(CComboBox* pCB)
{
	ASSERT(pCB);

	if (m_curParentItem == m_wndTreeGroundRouteItem.GetRootItem())
	{
		std::vector<int> vRunways;
		ALTAirport::GetRunwaysIDs(m_nAirPortID, vRunways);
		for (std::vector<int>::iterator iterRunwayID = vRunways.begin(); iterRunwayID != vRunways.end(); ++iterRunwayID)
		{
			Runway runway;
			runway.ReadObject(*iterRunwayID);

			//Marking one
			CString strRunwayMarkOneName;
			strRunwayMarkOneName.Format("Runway:%s", runway.GetMarking1().c_str());
            int nItemIndex = pCB->AddString(strRunwayMarkOneName);
			pCB->SetItemData(nItemIndex, (DWORD_PTR)(*iterRunwayID));
			//Marking Two
			CString strRunwayMarkTwoName;
			strRunwayMarkTwoName.Format("Runway:%s", runway.GetMarking2().c_str());
			nItemIndex = pCB->AddString(strRunwayMarkTwoName);
			pCB->SetItemData(nItemIndex, (DWORD_PTR)(*iterRunwayID));
		}

		std::vector<int> vTaxiwayIDs;
		ALTAirport::GetTaxiwaysIDs(m_nAirPortID, vTaxiwayIDs);
		for (std::vector<int>::iterator iter = vTaxiwayIDs.begin(); iter != vTaxiwayIDs.end(); ++iter)
		{
			Taxiway taxiway;
			taxiway.ReadObject(*iter);

			CString strTaxiwayMark;
			strTaxiwayMark.Format("Taxiway:%s", taxiway.GetMarking().c_str());
			int nItemIndex = pCB->AddString(strTaxiwayMark);
			pCB->SetItemData(nItemIndex, (DWORD_PTR)(*iter));
		}
	}
	else
	{
		//Get current Item info
		GroundRouteItem* pGroundRouteItem = (GroundRouteItem*)m_wndTreeGroundRouteItem.GetItemData(m_curParentItem);
		ASSERT(pGroundRouteItem);
		int nALTObjID = pGroundRouteItem->GetALTObjectID();
		ALTObject altObj;
		ALTObject *pALTObj = altObj.ReadObjectByID(nALTObjID);
		ASSERT(pALTObj);

		if (ALT_RUNWAY == pALTObj->GetType())
		{
			Runway runway;
			runway.ReadObject(nALTObjID);
			IntersectionNodeList runwayINodeList = runway.GetIntersectNodes();

			for(int i=0;i< (int)runwayINodeList.size();i++)
			{
				
				std::vector<ALTObject*> vOtherObjList = runwayINodeList.at(i).GetOtherObjectList(nALTObjID);

				for(int iOtherIdx=0;iOtherIdx<(int)vOtherObjList.size();iOtherIdx++)
				{
					ALTObject *otherObj = vOtherObjList.at(iOtherIdx);//runwayINodeList.at(iOtherIdx).GetOtherObjectList(nALTObjID);
					if( otherObj && otherObj->GetType() == ALT_RUNWAY )	
					{
						int nRunwayID = otherObj->getID();
						Runway intersectRunway;
						intersectRunway.ReadObject(nRunwayID);

						//the runway is exist
						if (IsIntersectObjExistInComboBox(pCB, nRunwayID))
						{
							continue;
						}

						//Marking one
						CString strRunwayMarkOneName;
						strRunwayMarkOneName.Format("Runway:%s", intersectRunway.GetMarking1().c_str());
						int nItemIndex = pCB->AddString(strRunwayMarkOneName);
						pCB->SetItemData(nItemIndex, (DWORD_PTR)(nRunwayID));
						//Marking Two
						CString strRunwayMarkTwoName;
						strRunwayMarkTwoName.Format("Runway:%s", intersectRunway.GetMarking2().c_str());
						nItemIndex = pCB->AddString(strRunwayMarkTwoName);
						pCB->SetItemData(nItemIndex, (DWORD_PTR)(nRunwayID));
					}

					if (otherObj && otherObj->GetType() == ALT_TAXIWAY)
					{
						int nTaxiwayID = otherObj->getID();
						Taxiway intersectTaxiway;
						intersectTaxiway.ReadObject(nTaxiwayID);

						//the taxiway is exist
						if (IsIntersectObjExistInComboBox(pCB, nTaxiwayID))
						{
							continue;
						}

						CString strTaxiwayMark;
						strTaxiwayMark.Format("Taxiway:%s", intersectTaxiway.GetMarking().c_str());
						int nItemIndex = pCB->AddString(strTaxiwayMark);
						pCB->SetItemData(nItemIndex, (DWORD_PTR)(nTaxiwayID));
					}
				}
				
			}
		}
		else if (ALT_TAXIWAY == pALTObj->GetType())
		{
			Taxiway taxiway;
			taxiway.ReadObject(nALTObjID);
			IntersectionNodeList taxiwayINodeList = taxiway.GetIntersectNodes();

			for(int i=0;i< (int)taxiwayINodeList.size();i++)
			{
				
				std::vector<ALTObject*> vOtherObjList = taxiwayINodeList.at(i).GetOtherObjectList(nALTObjID);

				for(int iOtherIdx=0;iOtherIdx<(int)vOtherObjList.size();iOtherIdx++)
				{
					ALTObject *otherObj = vOtherObjList.at(iOtherIdx);
					if( otherObj && otherObj->GetType() == ALT_RUNWAY )	
					{
						int nRunwayID = otherObj->getID();
						Runway intersectRunway;
						intersectRunway.ReadObject(nRunwayID);

						//the runway is exist
						if (IsIntersectObjExistInComboBox(pCB, nRunwayID))
						{
							continue;
						}

						//Marking one
						CString strRunwayMarkOneName;
						strRunwayMarkOneName.Format("Runway:%s", intersectRunway.GetMarking1().c_str());
						int nItemIndex = pCB->AddString(strRunwayMarkOneName);
						pCB->SetItemData(nItemIndex, (DWORD_PTR)(nRunwayID));
						//Marking Two
						CString strRunwayMarkTwoName;
						strRunwayMarkTwoName.Format("Runway:%s", intersectRunway.GetMarking2().c_str());
						nItemIndex = pCB->AddString(strRunwayMarkTwoName);
						pCB->SetItemData(nItemIndex, (DWORD_PTR)(nRunwayID));
					}

					if (otherObj && otherObj->GetType() == ALT_TAXIWAY)
					{
						int nTaxiwayID = otherObj->getID();
						Taxiway intersectTaxiway;
						intersectTaxiway.ReadObject(nTaxiwayID);

						//the taxiway is exist
						if (IsIntersectObjExistInComboBox(pCB, nTaxiwayID))
						{
							continue;
						}

						CString strTaxiwayMark;
						strTaxiwayMark.Format("Taxiway:%s", intersectTaxiway.GetMarking().c_str());
						int nItemIndex = pCB->AddString(strTaxiwayMark);
						pCB->SetItemData(nItemIndex, (DWORD_PTR)(nTaxiwayID));
					}
				}				
			}
		}
		else
		{
			ASSERT(FALSE);
		}
	}
}

LRESULT CDlgGroundRoutes::OnMessageModifyGroundRouteName( WPARAM wParam, LPARAM lParam )
{
	ASSERT(m_pCurGroundRoute);
	m_pCurGroundRoute->SetName((LPCSTR)lParam);
	if (m_pCurGroundRoute->GetObjectName().IsBlank())
	{
		MessageBox("The route name cannot be empty!");

		int nItem = m_wndGroundRouteListBox.GetCurSel();
		m_wndGroundRouteListBox.EditItem(nItem);
		return 1;
	}

	GetDlgItem(IDC_SAVE)->EnableWindow(TRUE);

	return 1;
}

void CDlgGroundRoutes::OnContextMenu(CWnd* pWnd, CPoint point)
{
	// TODO: Add your message handler code here
	CRect rectTree;
	m_wndTreeGroundRouteItem.GetWindowRect(&rectTree);
	if (!rectTree.PtInRect(point)) 
		return;

	m_wndTreeGroundRouteItem.SetFocus();
	CPoint pt = point;
	m_wndTreeGroundRouteItem.ScreenToClient(&pt);
	UINT iRet;
	HTREEITEM hRClickItem = m_wndTreeGroundRouteItem.HitTest(pt, &iRet);
	if (iRet != TVHT_ONITEMLABEL)
	{
		hRClickItem = NULL;
		return;
	}

	m_wndTreeGroundRouteItem.SelectItem(hRClickItem);	
	HTREEITEM hParentItem = 0;
	hParentItem = m_wndTreeGroundRouteItem.GetParentItem(hRClickItem);
	
	CMenu menuPopup; 
	menuPopup.CreatePopupMenu(); 

	if(!hParentItem)
	{
		menuPopup.AppendMenu(MF_POPUP, (UINT) ID_NEW_GROUNDROUTEITEM, _T("Add Route Item") );
	}
	else
	{
		menuPopup.AppendMenu(MF_POPUP, (UINT) ID_NEW_GROUNDROUTEITEM, _T("Add Route Item") );
		menuPopup.AppendMenu(MF_POPUP, (UINT) ID_DEL_GROUNDROUTEITEM, _T("Delete Route Item") );
		menuPopup.AppendMenu(MF_POPUP, (UINT) ID_EDIT_GROUNDROUTEITEM, _T("Edit Route Item") );		
	}		
	
	menuPopup.TrackPopupMenu(TPM_LEFTALIGN, point.x, point.y, this);
	menuPopup.DestroyMenu();
}