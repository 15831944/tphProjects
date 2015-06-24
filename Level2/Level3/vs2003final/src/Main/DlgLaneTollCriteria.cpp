// DlgLaneTollCriteria.cpp : implementation file
//

#include "stdafx.h"
#include "TermPlan.h"
#include "DlgLaneTollCriteria.h"
#include "../Common/WinMsg.h"
#include "DlgSelectLandsideVehicleType.h"
#include "DlgTimeRange.h"
#include "TermPlanDoc.h"
#include "MainFrm.h"
#include "landside/LandsideLayoutObject.h"
#include "landside/InputLandside.h"
#include "DlgSelectLandsideObject.h"
#include "DlgLaneRange.h"

// CDlgLaneTollCriteria dialog
const CString strSeviceString[3] = {"Exclusive","Inclusive","Not"};

namespace
{
	const UINT ID_LANE_NEW = 20;
	const UINT ID_LANE_DEL = 21;
}


IMPLEMENT_DYNAMIC(CDlgLaneTollCriteria, CXTResizeDialog)
CDlgLaneTollCriteria::CDlgLaneTollCriteria(CTermPlanDoc* pDoc,CWnd* pParent /*=NULL*/)
	: CXTResizeDialog(CDlgLaneTollCriteria::IDD, pParent)
	,m_pDoc(pDoc)
{
}

CDlgLaneTollCriteria::~CDlgLaneTollCriteria()
{
}

void CDlgLaneTollCriteria::DoDataExchange(CDataExchange* pDX)
{
	CXTResizeDialog::DoDataExchange(pDX);
	DDX_Control(pDX,IDC_TREE_DATA,m_wndTreeCtrl);
}


BEGIN_MESSAGE_MAP(CDlgLaneTollCriteria, CXTResizeDialog)
	ON_WM_CREATE()
	ON_WM_CONTEXTMENU()
	ON_NOTIFY(TVN_SELCHANGED,IDC_TREE_DATA,OnLvnItemchangedList)
	ON_BN_CLICKED(IDC_BUTTON_SAVE,OnBtnSave)
	ON_MESSAGE(WM_LBUTTONDBLCLK,OnLeftDoubleClick)
	ON_COMMAND(ID_LANE_NEW,OnCmdNewItem)
	ON_COMMAND(ID_LANE_DEL,OnCmdDeleteItem)
END_MESSAGE_MAP()


// CDlgLaneTollCriteria message handlers

BOOL CDlgLaneTollCriteria::OnInitDialog()
{
	CXTResizeDialog::OnInitDialog();

	m_laneTollCriteria.ReadData(-1);

	OnInitToolbar();
	UpdateToolBarState();
	LoadTreeContent();
	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(FALSE);

	SetResize(IDC_STATIC_GROUP,SZ_TOP_LEFT,SZ_BOTTOM_RIGHT);
	SetResize(m_wndToolbar.GetDlgCtrlID(),SZ_TOP_LEFT,SZ_TOP_LEFT);
	SetResize(IDC_TREE_DATA,SZ_TOP_LEFT,SZ_BOTTOM_RIGHT);
	SetResize(IDC_BUTTON_SAVE,SZ_BOTTOM_RIGHT,SZ_BOTTOM_RIGHT);
	SetResize(IDOK,SZ_BOTTOM_RIGHT,SZ_BOTTOM_RIGHT);
	SetResize(IDCANCEL,SZ_BOTTOM_RIGHT,SZ_BOTTOM_RIGHT);

	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),TRUE);
	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),FALSE);
	return TRUE;
}

int CDlgLaneTollCriteria::OnCreate( LPCREATESTRUCT lpCreateStruct )
{
	if (CXTResizeDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!m_wndToolbar.CreateEx(this, TBSTYLE_FLAT,WS_CHILD|WS_VISIBLE|CBRS_ALIGN_TOP)
		|| !m_wndToolbar.LoadToolBar(IDR_ADDDELTOOLBAR))
	{
		return -1;
	}
	return 0;
}

void CDlgLaneTollCriteria::OnOK()
{
	try
	{
		CADODatabase::BeginTransaction();
		m_laneTollCriteria.SaveData(-1);
		CADODatabase::CommitTransaction();
	}
	catch (CADOException& e)
	{
		e.ErrorMessage();
		CADODatabase::RollBackTransation();
		MessageBox(_T("Save database error"),_T("ERROR"),MB_OK);
		return;
	}
	CXTResizeDialog::OnOK();
}

void CDlgLaneTollCriteria::OnLvnItemchangedList( NMHDR *pNMHDR, LRESULT *pResult )
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	*pResult = 0;
	if (pNMTreeView->itemNew.hItem)
	{
		m_wndTreeCtrl.SelectItem(pNMTreeView->itemNew.hItem);
		if (pNMTreeView->itemNew.hItem != pNMTreeView->itemOld.hItem)
		{
			UpdateToolBarState();
		}
		return;
	}
	
	m_wndToolbar.GetToolBarCtrl().EnableButton(ID_LANE_NEW,TRUE);
	m_wndToolbar.GetToolBarCtrl().EnableButton(ID_LANE_DEL,FALSE);
}

void CDlgLaneTollCriteria::OnContextMenu( CWnd* pWnd, CPoint point )
{
	CRect rectTree;
	m_wndTreeCtrl.GetWindowRect(&rectTree);
	if (!rectTree.PtInRect(point)) 
		return;

	m_wndTreeCtrl.SetFocus();
	CPoint pt = point;
	m_wndTreeCtrl.ScreenToClient(&pt);
	UINT iRet;
	HTREEITEM hRClickItem = m_wndTreeCtrl.HitTest(pt, &iRet);
	if (iRet != TVHT_ONITEMLABEL)
	{
		hRClickItem = NULL;
		return;
	}

	m_wndTreeCtrl.SelectItem(hRClickItem);
	TreeNodeData* pNodeData = (TreeNodeData*)m_wndTreeCtrl.GetItemData(hRClickItem);
	if (pNodeData == NULL)
		return;

	CMenu menuPopup; 
	menuPopup.CreatePopupMenu();
	switch (pNodeData->m_emType)
	{
	case TreeNodeData::VehicleType_Node:
		{
			menuPopup.AppendMenu(MF_POPUP,ID_LANE_NEW,_T("Add Characteristic"));
			menuPopup.AppendMenu(MF_POPUP,ID_LANE_DEL,_T("Delete Vehicle Type"));		
			menuPopup.TrackPopupMenu(TPM_LEFTALIGN, point.x, point.y, this);
		}
		break;
	case TreeNodeData::Character_Node:
		{
			menuPopup.AppendMenu(MF_POPUP,ID_LANE_NEW,_T("Add Time Range"));
			menuPopup.AppendMenu(MF_POPUP,ID_LANE_DEL,_T("Delete Characteristic"));
			menuPopup.TrackPopupMenu(TPM_LEFTALIGN, point.x, point.y, this);
		}
		break;
	case TreeNodeData::TimeRange_Node:
		{
			menuPopup.AppendMenu(MF_POPUP,ID_LANE_NEW,_T("Add Lane or Toll Gate"));
			menuPopup.AppendMenu(MF_POPUP,ID_LANE_DEL,_T("Delete Time Range"));
			menuPopup.TrackPopupMenu(TPM_LEFTALIGN, point.x, point.y, this);
		}
		break;
	case TreeNodeData::LaneToll_Node:
		{
			menuPopup.AppendMenu(MF_POPUP,ID_LANE_DEL,_T("Delete Lane or Toll Gate"));
			menuPopup.TrackPopupMenu(TPM_LEFTALIGN, point.x, point.y, this);
		}
		break;
	default:
		break;
	}
	menuPopup.DestroyMenu();
}

void CDlgLaneTollCriteria::OnBtnSave()
{
	try
	{
		CADODatabase::BeginTransaction();
		m_laneTollCriteria.SaveData(-1);
		CADODatabase::CommitTransaction();
	}
	catch (CADOException& e)
	{
		e.ErrorMessage();
		CADODatabase::RollBackTransation();
		MessageBox(_T("Save database error"),_T("ERROR"),MB_OK);
		return;
	}
	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(FALSE);
}

LRESULT CDlgLaneTollCriteria::DefWindowProc( UINT message, WPARAM wParam, LPARAM lParam )
{
	switch(message)
	{
	case UM_CEW_COMBOBOX_BEGIN:
		{
			HTREEITEM hItem=(HTREEITEM)wParam;
			CRect rectWnd;
			CWnd* pWnd=m_wndTreeCtrl.GetEditWnd(hItem);
			m_wndTreeCtrl.GetItemRect(hItem,rectWnd,TRUE);
			pWnd->SetWindowPos(NULL,rectWnd.right,rectWnd.top,0,0,SWP_NOSIZE);
			CComboBox* pCB=(CComboBox*)pWnd;
			pCB->ResetContent();
			if (pCB->GetCount() == 0)
			{
				pCB->AddString(strSeviceString[0]);
				pCB->AddString(strSeviceString[1]);
				pCB->AddString(strSeviceString[2]);
			}
		}
		break;
	case UM_CEW_COMBOBOX_SELCHANGE:
		{
			HTREEITEM hItem=(HTREEITEM)wParam;
			int nSel = m_wndTreeCtrl.GetCmbBoxCurSel(hItem);
			TreeNodeData* pNodeData = (TreeNodeData*)m_wndTreeCtrl.GetItemData(hItem);
			LandsideLaneTollData* pData = (LandsideLaneTollData*)(pNodeData->m_dwData);
			if (pData)
			{
				pData->SetServiceType(LandsideLaneTollData::LaneTollServiceType(nSel));
				m_wndTreeCtrl.SetItemText(hItem,strSeviceString[nSel]);
				GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
			}
		}
		break;
	default:
		break;
	}
	return CXTResizeDialog::DefWindowProc(message,wParam,lParam);
}

void CDlgLaneTollCriteria::OnInitToolbar()
{
	CRect rect;
	m_wndTreeCtrl.GetWindowRect( &rect );
	ScreenToClient( &rect );
	rect.top -= 26;
	rect.bottom = rect.top + 22;
	rect.left += 2;
	m_wndToolbar.MoveWindow(&rect);

	CToolBarCtrl& spotBarCtrl = m_wndToolbar.GetToolBarCtrl();
	spotBarCtrl.SetCmdID(0,ID_LANE_NEW);
	spotBarCtrl.SetCmdID(1,ID_LANE_DEL);

	m_wndToolbar.GetToolBarCtrl().EnableButton( ID_LANE_NEW, TRUE);
	m_wndToolbar.GetToolBarCtrl().EnableButton( ID_LANE_DEL, FALSE);
}

void CDlgLaneTollCriteria::UpdateToolBarState()
{
	HTREEITEM hItem = m_wndTreeCtrl.GetSelectedItem();
	if (hItem == NULL)
	{
		m_wndToolbar.GetToolBarCtrl().EnableButton(ID_LANE_NEW,TRUE);
		m_wndToolbar.GetToolBarCtrl().EnableButton(ID_LANE_DEL,FALSE);
	}
	else
	{
		TreeNodeData* pNodeData = (TreeNodeData*)m_wndTreeCtrl.GetItemData(hItem);
		if (pNodeData == NULL)
		{
			m_wndToolbar.GetToolBarCtrl().EnableButton(ID_LANE_NEW,TRUE);
			m_wndToolbar.GetToolBarCtrl().EnableButton(ID_LANE_DEL,FALSE);
		}
		else
		{
			if (pNodeData->m_emType == TreeNodeData::LaneToll_Node)
			{
				m_wndToolbar.GetToolBarCtrl().EnableButton(ID_LANE_NEW,FALSE);
				m_wndToolbar.GetToolBarCtrl().EnableButton(ID_LANE_DEL,TRUE);
			}
			else if (pNodeData->m_emType == TreeNodeData::Service_Node || pNodeData->m_emType == TreeNodeData::Lane_Node ||
				pNodeData->m_emType == TreeNodeData::Facility_Node)
			{
				m_wndToolbar.GetToolBarCtrl().EnableButton(ID_LANE_NEW,FALSE);
				m_wndToolbar.GetToolBarCtrl().EnableButton(ID_LANE_DEL,FALSE);
			}
			else
			{
				m_wndToolbar.GetToolBarCtrl().EnableButton(ID_LANE_NEW,TRUE);
				m_wndToolbar.GetToolBarCtrl().EnableButton(ID_LANE_DEL,TRUE);
			}
		}
	}
}

void CDlgLaneTollCriteria::LoadTreeContent()
{
	m_wndTreeCtrl.DeleteAllItems();
	m_wndTreeCtrl.SetImageList(m_wndTreeCtrl.GetImageList(TVSIL_NORMAL),TVSIL_NORMAL);

	for (int i = 0; i < m_laneTollCriteria.GetItemCount(); i++)
	{
		LandsideVehicleAllocation* pVehicle = m_laneTollCriteria.GetItem(i);
		InsertVehicleAllocationItem(TVI_ROOT,pVehicle);
	}

	HTREEITEM hItem = m_wndTreeCtrl.GetRootItem();
	if (hItem)
	{
		m_wndTreeCtrl.SelectSetFirstVisible(hItem);
	}
}

void CDlgLaneTollCriteria::InsertVehicleAllocationItem( HTREEITEM hItem,LandsideVehicleAllocation* pVehicle )
{
	COOLTREE_NODE_INFO cni;
	CCoolTree::InitNodeInfo(this,cni);

	TreeNodeData* pNodeData = new TreeNodeData();
	pNodeData->m_emType = TreeNodeData::VehicleType_Node;
	pNodeData->m_dwData = (DWORD)pVehicle;
	cni.nt = NT_NORMAL;
	cni.net = NET_SHOW_DIALOGBOX;
	CString strVehicleType;
	strVehicleType.Format(_T("Vehicle Type: %s"),pVehicle->GetVehicleType());
	HTREEITEM hVehicleTypeItem = m_wndTreeCtrl.InsertItem(strVehicleType,cni,FALSE,FALSE,hItem);
	m_wndTreeCtrl.SetItemData(hVehicleTypeItem,(DWORD)pNodeData);
	m_vNodeData.push_back(pNodeData);

	for (int i = 0; i < pVehicle->m_charaContainer.GetItemCount(); i++)
	{
		LandsideCharactersitscAllocation* pChara = pVehicle->m_charaContainer.GetItem(i);
		InsertCharacterstiscItem(hVehicleTypeItem,pChara);
	}
	m_wndTreeCtrl.Expand(hVehicleTypeItem,TVE_EXPAND);
}

void CDlgLaneTollCriteria::InsertCharacterstiscItem( HTREEITEM hItem,LandsideCharactersitscAllocation* pChara )
{
	COOLTREE_NODE_INFO cni;
	CCoolTree::InitNodeInfo(this,cni);

	TreeNodeData* pNodeData = new TreeNodeData();
	pNodeData->m_emType = TreeNodeData::Character_Node;
	pNodeData->m_dwData = (DWORD)pChara;
	cni.nt = NT_NORMAL;
	cni.net = NET_NORMAL;
	HTREEITEM hCharaItem = m_wndTreeCtrl.InsertItem("Characteristics",cni,FALSE,FALSE,hItem);
	m_wndTreeCtrl.SetItemData(hCharaItem,(DWORD)pNodeData);
	m_vNodeData.push_back(pNodeData);

	for (int i = 0; i < pChara->m_timeRangeContainer.GetItemCount(); i++)
	{
		LandsideTimeRangeAllocation* pTimeRange =pChara->m_timeRangeContainer.GetItem(i);
		InsertTimeWindowItem(hCharaItem,pTimeRange);
	}
	m_wndTreeCtrl.Expand(hCharaItem,TVE_EXPAND);
}

void CDlgLaneTollCriteria::InsertTimeWindowItem( HTREEITEM hItem,LandsideTimeRangeAllocation* pTimeRange )
{
	COOLTREE_NODE_INFO cni;
	CCoolTree::InitNodeInfo(this,cni);

	TreeNodeData* pNodeData = new TreeNodeData();
	pNodeData->m_emType = TreeNodeData::TimeRange_Node;
	pNodeData->m_dwData = (DWORD)pTimeRange;
	cni.nt = NT_NORMAL;
	cni.net = NET_SHOW_DIALOGBOX;
	CString strTimeRange;
	strTimeRange.Format(_T("During This Time window: %s"),pTimeRange->GetTimeRange().ToString());
	HTREEITEM hTimeRangeItem = m_wndTreeCtrl.InsertItem(strTimeRange,cni,FALSE,FALSE,hItem);
	m_wndTreeCtrl.SetItemData(hTimeRangeItem,(DWORD)pNodeData);
	m_vNodeData.push_back(pNodeData);

	for (int i = 0; i < pTimeRange->m_dataContainer.GetItemCount(); i++)
	{
		LandsideLaneTollData* pData = pTimeRange->m_dataContainer.GetItem(i);
		InsertLaneTollItem(hTimeRangeItem,pData);
	}
	m_wndTreeCtrl.Expand(hTimeRangeItem,TVE_EXPAND);
}

void CDlgLaneTollCriteria::InsertLaneTollItem( HTREEITEM hItem,LandsideLaneTollData* pData )
{
	COOLTREE_NODE_INFO cni;
	CCoolTree::InitNodeInfo(this,cni);

	TreeNodeData* pNodeData = new TreeNodeData();
	pNodeData->m_emType = TreeNodeData::LaneToll_Node;
	pNodeData->m_dwData = (DWORD)pData;
	cni.nt = NT_NORMAL;
	cni.net = NET_NORMAL;
	HTREEITEM hDataItem = m_wndTreeCtrl.InsertItem(_T("Lane or Toll Gate"),cni,FALSE,FALSE,hItem);
	m_wndTreeCtrl.SetItemData(hDataItem,(DWORD)pNodeData);
	m_vNodeData.push_back(pNodeData);
	{
		//stretch name or toll gate name
		pNodeData = new TreeNodeData();
		pNodeData->m_emType = TreeNodeData::Facility_Node;
		pNodeData->m_dwData = (DWORD)pData;
		cni.nt = NT_NORMAL;
		cni.net = NET_SHOW_DIALOGBOX;
		CString strFacility;
		strFacility.Format("%s",pData->GetName().GetIDString());
		HTREEITEM hFacilityItem = m_wndTreeCtrl.InsertItem(strFacility,cni,FALSE,FALSE,hDataItem);
		m_wndTreeCtrl.SetItemData(hFacilityItem,(DWORD)pNodeData);
		m_vNodeData.push_back(pNodeData);

		//stretch lane
		pNodeData = new TreeNodeData();
		pNodeData->m_emType = TreeNodeData::Lane_Node;
		pNodeData->m_dwData = (DWORD)pData;
		cni.nt = NT_NORMAL;
		cni.net = NET_SHOW_DIALOGBOX;
		CString strLane;
		strLane.Format(_T("Lane Range: %s"),pData->GetLane().GetLaneRange());
		HTREEITEM hLaneItem = m_wndTreeCtrl.InsertItem(strLane,cni,FALSE,FALSE,hDataItem);
		m_wndTreeCtrl.SetItemData(hLaneItem,(DWORD)pNodeData);
		m_vNodeData.push_back(pNodeData);

		//service type
		pNodeData = new TreeNodeData();
		pNodeData->m_emType = TreeNodeData::Service_Node;
		pNodeData->m_dwData = (DWORD)pData;
		cni.nt = NT_NORMAL;
		cni.net = NET_COMBOBOX;
		CString strSevice;
		strSevice.Format("%s",strSeviceString[pData->GetServiceType()]);
		HTREEITEM hServiceItem = m_wndTreeCtrl.InsertItem(strSevice,cni,FALSE,FALSE,hDataItem);
		m_wndTreeCtrl.SetItemData(hServiceItem,(DWORD)pNodeData);
		m_vNodeData.push_back(pNodeData);
	}

	m_wndTreeCtrl.Expand(hDataItem,TVE_EXPAND);
}

void CDlgLaneTollCriteria::OnCmdNewItem()
{
	HTREEITEM hItem = m_wndTreeCtrl.GetSelectedItem();
	if (hItem == NULL)
	{
		CDlgSelectLandsideVehicleType dlg;
		if(dlg.DoModal() == IDOK)
		{
			CString strVehicleType = dlg.GetName();
			if (!strVehicleType.IsEmpty())
			{
				if (!m_laneTollCriteria.ExistSameVehicleType(strVehicleType))
				{
					LandsideVehicleAllocation* pVehcile = new LandsideVehicleAllocation();
					pVehcile->SetVehicleType(strVehicleType);
					m_laneTollCriteria.AddItem(pVehcile);
					InsertVehicleAllocationItem(hItem,pVehcile);
					GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
				}
				else
				{
					MessageBox(_T("Exist the Same Vehicle Type!!!"),_T("Warning"),MB_OK);
				}
			}
		}
		return;
	}

	TreeNodeData* pNodeData = (TreeNodeData*)m_wndTreeCtrl.GetItemData(hItem);
	if (pNodeData == NULL)
		return;

	switch (pNodeData->m_emType)
	{
		case TreeNodeData::VehicleType_Node:
			{
				LandsideCharactersitscAllocation* pChara = new LandsideCharactersitscAllocation();
				LandsideVehicleAllocation* pVehicle = (LandsideVehicleAllocation*)(pNodeData->m_dwData);
				pVehicle->m_charaContainer.AddItem(pChara);
				InsertCharacterstiscItem(hItem,pChara);
				m_wndTreeCtrl.Expand(hItem,TVE_EXPAND);
				GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
			}
			break;
		case TreeNodeData::Character_Node:
			{
				TimeRange timeRange;
				CDlgTimeRange dlg(timeRange.GetStartTime(),timeRange.GetEndTime());
				if (dlg.DoModal() == IDOK)
				{
					TimeRange timeRange;
					timeRange.SetStartTime(dlg.GetStartTime());
					timeRange.SetEndTime(dlg.GetEndTime());
			
					LandsideCharactersitscAllocation* pChara = (LandsideCharactersitscAllocation*)(pNodeData->m_dwData);
					if (pChara->CheckTimeRange(timeRange))
					{
						MessageBox(_T("Time range conflict with exist item.!!"),_T("Warning"),MB_OK);
						return;
					}

					LandsideTimeRangeAllocation* pTimeRange = new LandsideTimeRangeAllocation();
					pTimeRange->SetTimeRange(timeRange);
					pChara->m_timeRangeContainer.AddNewItem(pTimeRange);
					InsertTimeWindowItem(hItem,pTimeRange);
					m_wndTreeCtrl.Expand(hItem,TVE_EXPAND);
					GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
				}
			}
			break;
		case TreeNodeData::TimeRange_Node:
			{
				LandsideFacilityLayoutObjectList* pLayoutList = &(m_pDoc->GetInputLandside()->getObjectList());
				CDlgSelectLandsideObject dlg(pLayoutList,this);
				dlg.AddObjType(ALT_LSTRETCH);
				dlg.AddObjType(ALT_LTOLLGATE);
				if(dlg.DoModal() == IDOK)
				{
					ALTObjectID strFacility = dlg.getSelectObject();
					LandsideTimeRangeAllocation* pTimeRange = (LandsideTimeRangeAllocation*)pNodeData->m_dwData;
					if (!strFacility.IsBlank() && !pTimeRange->ExistSameFacility(strFacility))
					{
						LandsideLaneTollData* pData = new LandsideLaneTollData();
						pData->SetName(strFacility);

						pTimeRange->m_dataContainer.AddItem(pData);
						InsertLaneTollItem(hItem,pData);
						m_wndTreeCtrl.Expand(hItem,TVE_EXPAND);
						GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
					}
					else
					{
						MessageBox(_T("Exist the Same Stretch or Toll Gate!!!"),_T("Warning"),MB_OK);
					}
				}
			}
			break;
		default:
			break;
	}
}

void CDlgLaneTollCriteria::OnCmdDeleteItem()
{
	HTREEITEM hItem = m_wndTreeCtrl.GetSelectedItem();
	if (hItem == NULL)
		return;

	TreeNodeData* pNodeData = (TreeNodeData*)m_wndTreeCtrl.GetItemData(hItem);
	if (pNodeData == NULL)
		return;

	switch (pNodeData->m_emType)
	{
		case TreeNodeData::VehicleType_Node:
			{
				LandsideVehicleAllocation* pVehicle = (LandsideVehicleAllocation*)(pNodeData->m_dwData);
				m_laneTollCriteria.DeleteItem(pVehicle);
				m_wndTreeCtrl.DeleteItem(hItem);
				DeleteTreeNodeData(pNodeData);
				GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
			}
			break;
		case TreeNodeData::Character_Node:
			{
				HTREEITEM hParentItem = m_wndTreeCtrl.GetParentItem(hItem);
				if (hParentItem == NULL)
					return;
				TreeNodeData* pParentData = (TreeNodeData*)m_wndTreeCtrl.GetItemData(hParentItem);
				if (pParentData == NULL)
					return;

				LandsideVehicleAllocation* pVehicle = (LandsideVehicleAllocation*)(pParentData->m_dwData);
				if (pVehicle == NULL)
					return;

				LandsideCharactersitscAllocation* pChara = (LandsideCharactersitscAllocation*)(pNodeData->m_dwData);
				if (pChara == NULL)
					return;

				pVehicle->m_charaContainer.DeleteItem(pChara);
				m_wndTreeCtrl.DeleteItem(hItem);
				DeleteTreeNodeData(pNodeData);
				GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
			}
			break;
		case TreeNodeData::TimeRange_Node:
			{
				HTREEITEM hParentItem = m_wndTreeCtrl.GetParentItem(hItem);
				if (hParentItem == NULL)
					return;
				TreeNodeData* pParentData = (TreeNodeData*)m_wndTreeCtrl.GetItemData(hParentItem);
				if (pParentData == NULL)
					return;

				LandsideCharactersitscAllocation* pChara = (LandsideCharactersitscAllocation*)(pParentData->m_dwData);
				if (pChara == NULL)
					return;

				LandsideTimeRangeAllocation* pTime = (LandsideTimeRangeAllocation*)(pNodeData->m_dwData);
				if (pTime == NULL)
					return;

				pChara->m_timeRangeContainer.DeleteItem(pTime);
				m_wndTreeCtrl.DeleteItem(hItem);
				DeleteTreeNodeData(pNodeData);
				GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
			}
			break;
		case TreeNodeData::LaneToll_Node:
			{
				HTREEITEM hParentItem = m_wndTreeCtrl.GetParentItem(hItem);
				if (hParentItem == NULL)
					return;
				TreeNodeData* pParentData = (TreeNodeData*)m_wndTreeCtrl.GetItemData(hParentItem);
				if (pParentData == NULL)
					return;

				LandsideTimeRangeAllocation* pTimeRange = (LandsideTimeRangeAllocation*)(pParentData->m_dwData);
				if (pTimeRange == NULL)
					return;

				LandsideLaneTollData* pData = (LandsideLaneTollData*)(pNodeData->m_dwData);
				if (pData == NULL)
					return;

				pTimeRange->m_dataContainer.DeleteItem(pData);
				m_wndTreeCtrl.DeleteItem(hItem);
				DeleteTreeNodeData(pNodeData);
				GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
			}	
			break;
		default:
			break;			
	}
}

void CDlgLaneTollCriteria::DeleteTreeNodeData( TreeNodeData* pNodeData )
{
	std::vector<TreeNodeData*>::iterator iter = std::find(m_vNodeData.begin(),m_vNodeData.end(),pNodeData);
	if (iter != m_vNodeData.end())
	{
		m_vNodeData.erase(iter);
		delete pNodeData;
	}
}

void CDlgLaneTollCriteria::ClearTreeNode()
{
	for (int i = 0; i < (int)m_vNodeData.size(); i++)
	{
		delete m_vNodeData[i];
	}
	m_vNodeData.clear();
}


LRESULT CDlgLaneTollCriteria::OnLeftDoubleClick( WPARAM wParam, LPARAM lParam )
{
	HTREEITEM hItem = m_wndTreeCtrl.GetSelectedItem();
	if(hItem == NULL)
		return 0;

	COOLTREE_NODE_INFO* pCni = m_wndTreeCtrl.GetItemNodeInfo(hItem);
	if (pCni->net == NET_DATETIMEPICKER)
		return 0;

	TreeNodeData* pNodeData = (TreeNodeData*)m_wndTreeCtrl.GetItemData(hItem);
	if (pNodeData == NULL)
		return 0;

	switch (pNodeData->m_emType)
	{
	case TreeNodeData::VehicleType_Node:
		{
			CDlgSelectLandsideVehicleType dlg;
			if(dlg.DoModal() == IDOK)
			{
				CString strVehicleType = dlg.GetName();
				if (!strVehicleType.IsEmpty())
				{
					LandsideVehicleAllocation* pVehicle = (LandsideVehicleAllocation*)pNodeData->m_dwData;
					if (pVehicle->GetVehicleType() == strVehicleType)
					{
						return 0;
					}
					if (!m_laneTollCriteria.ExistSameVehicleType(strVehicleType))
					{
						pVehicle->SetVehicleType(strVehicleType);
						CString strVehicleType;
						strVehicleType.Format(_T("Vehicle Type: %s"),strVehicleType);
						m_wndTreeCtrl.SetItemText(hItem,strVehicleType);
						GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
					}
					else
					{
						MessageBox(_T("Exist the Same Vehicle Type!!!"),_T("Warning"),MB_OK);
					}	
				}
			}
		}
		break;
	case TreeNodeData::TimeRange_Node:
		{
			LandsideTimeRangeAllocation* pTimeRange = (LandsideTimeRangeAllocation*)(pNodeData->m_dwData);
			if (pTimeRange == NULL)
				return 0;
			
			CDlgTimeRange dlg(pTimeRange->GetTimeRange().GetStartTime(),pTimeRange->GetTimeRange().GetEndTime());
			if (dlg.DoModal() == IDOK)
			{
				TimeRange timeRange;
				timeRange.SetStartTime(dlg.GetStartTime());
				timeRange.SetEndTime(dlg.GetEndTime());

				HTREEITEM hParentItem = m_wndTreeCtrl.GetParentItem(hItem);
				TreeNodeData* pParentData = (TreeNodeData*)(m_wndTreeCtrl.GetItemData(hParentItem));
				if (pParentData == NULL)
					return 0;
				
				LandsideCharactersitscAllocation* pChara = (LandsideCharactersitscAllocation*)(pParentData->m_dwData);
				if (pChara == NULL)
					return 0;
				
				if (!pChara->CheckTimeRange(timeRange,pTimeRange))
				{
					pTimeRange->SetTimeRange(timeRange);
					CString strTimeRange;
					strTimeRange.Format(_T("During This Time Window: %s"),timeRange.ToString());
					m_wndTreeCtrl.SetItemText(hItem,strTimeRange);
					GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
				}
				else 
				{
					MessageBox(_T("Time range conflict with exist item.!!"),_T("Warning"),MB_OK);
				}
			}
		}
		break;
	case TreeNodeData::Facility_Node:
		{
			LandsideFacilityLayoutObjectList* pLayoutList = &(m_pDoc->GetInputLandside()->getObjectList());
			CDlgSelectLandsideObject dlg(pLayoutList,this);
			dlg.AddObjType(ALT_LSTRETCH);
			dlg.AddObjType(ALT_LTOLLGATE);
			if(dlg.DoModal() == IDOK)
			{
				ALTObjectID facilityName = dlg.getSelectObject();
				if (!facilityName.IsBlank())
				{
					LandsideLaneTollData* pData = (LandsideLaneTollData*)(pNodeData->m_dwData);
					if (pData == NULL)
						return 0;

					if (facilityName == pData->GetName())
						return 0;

					HTREEITEM hParentItem = m_wndTreeCtrl.GetParentItem(hItem);
					if (hParentItem == NULL)
						return 0;
					
					HTREEITEM hGrandItem = m_wndTreeCtrl.GetParentItem(hParentItem);
					if (hGrandItem == NULL)
						return 0;	
				
					TreeNodeData* pParentData = (TreeNodeData*)(m_wndTreeCtrl.GetItemData(hGrandItem));
					LandsideTimeRangeAllocation* pTimeRange = (LandsideTimeRangeAllocation*)(pParentData->m_dwData);
					if (pTimeRange == NULL)
						return 0;
					
					if (!pTimeRange->ExistSameFacility(facilityName))
					{
						pData->SetName(facilityName);
						m_wndTreeCtrl.SetItemText(hItem,facilityName.GetIDString());
						GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
					}
					else
					{
						MessageBox(_T("Exist the Same Stretch or Toll Gate!!!"),_T("Warning"),MB_OK);
					}
				}
			}
		}
		break;
	case TreeNodeData::Lane_Node:
		{
			LandsideLaneTollData* pData = (LandsideLaneTollData*)(pNodeData->m_dwData);
			if (pData == NULL)
				return 0;
			
			if (pData->GetLane().GetStartLane() == 0 || pData->GetLane().GetEndLane() == 0)
			{
				pData->SetStartLane(1);
				pData->SetEndLane(1);
			}

			CDlgLaneRange dlg(pData->GetLane().GetStartLane(),pData->GetLane().GetEndLane());
			if (dlg.DoModal() == IDOK)
			{
				if (dlg.IsAll())
				{
					pData->SetStartLane(0);
					pData->SetEndLane(0);
				} 
				else
				{
					pData->SetStartLane(dlg.GetStartLane());
					pData->SetEndLane(dlg.GetEndLane());

				}	
				CString strLane;
				strLane.Format(_T("Lane Range: %s"),pData->GetLane().GetLaneRange());
				m_wndTreeCtrl.SetItemText(hItem,strLane);
				GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
			}
		}
		break;
	default:
		break;
	}
	return 1;
}


