#include "StdAfx.h"
#include "TermPlan.h"
#include ".\dlgparkinglotstrategy.h"
#include "landside/LandsideLayoutObject.h"
#include "TermPlanDoc.h"
#include "landside/InputLandside.h"
#include "Landside/LandsideParkingLot.h"
#include "DlgSelectLandsideVehicleType.h"
#include "DlgSelectLandsideObject.h"
#include "DlgProbDist.h"
#include "Inputs/IN_TERM.H"
#include "Inputs/PROBAB.H"
#include "Common/ProbDistManager.h"

IMPLEMENT_DYNAMIC(CDlgParkingLotStrategy, CDlgLandsideStrategy)
CDlgParkingLotStrategy::CDlgParkingLotStrategy(CWnd* pParent /*=NULL*/)
: CDlgLandsideStrategy(pParent)
{
	m_canDeleteNode.push_back(TreeNodeData::VEHICLE_Node);
	m_canDeleteNode.push_back(TreeNodeData::PARKINGLOT_Node);
	m_canDeleteNode.push_back(TreeNodeData::PRIORITY_Node);

	m_canNewNode.push_back(TreeNodeData::VEHICLE_Node);
	m_canNewNode.push_back(TreeNodeData::PARKINGLOT_Node);
}

CDlgParkingLotStrategy::~CDlgParkingLotStrategy(void)
{
}

void CDlgParkingLotStrategy::SetTreeContent()
{
	for (int i = 0; i < m_parkingLotContainer.GetItemCount(); i++)
	{
		LandsideParkingLotVehicleStrategy* pVehicle = m_parkingLotContainer.GetItem(i);
		InsertVehicleItem(TVI_ROOT,pVehicle);
	}
}

void CDlgParkingLotStrategy::ReadData()
{
	m_parkingLotContainer.ReadData(-1);
}

void CDlgParkingLotStrategy::SaveData()
{
	m_parkingLotContainer.SaveData(-1);
}

HTREEITEM CDlgParkingLotStrategy::InsertVehicleItem( HTREEITEM hItem, LandsideParkingLotVehicleStrategy* pVehicle )
{
	COOLTREE_NODE_INFO cni;
	CCoolTree::InitNodeInfo(this,cni);

	TreeNodeData* pNodeData = new TreeNodeData();
	pNodeData->m_emType = TreeNodeData::VEHICLE_Node;
	pNodeData->m_dwData = (DWORD)pVehicle;
	cni.nt = NT_NORMAL;
	cni.net = NET_SHOW_DIALOGBOX;
	CString strGroupName;
	strGroupName.Format(_T("Vehicle Type: %s"),pVehicle->GetVehicleType());
	HTREEITEM hVehicleItem = m_wndTreeCtrl.InsertItem(strGroupName,cni,FALSE,FALSE,hItem);
	m_wndTreeCtrl.SetItemData(hVehicleItem,(DWORD)pNodeData);
	m_vNodeData.push_back(pNodeData);

	for (int i = 0; i < pVehicle->GetCount(); i++)
	{
		LandsideParkingLotSettingData* pParkingLot = pVehicle->GetItem(i);
		InsertParkingLotItem(hVehicleItem,pParkingLot);
	}

	m_wndTreeCtrl.Expand(hVehicleItem,TVE_EXPAND);
	return hVehicleItem;
}

HTREEITEM CDlgParkingLotStrategy::InsertParkingLotItem( HTREEITEM hItem, LandsideParkingLotSettingData* pParkingLot )
{
	COOLTREE_NODE_INFO cni;
	CCoolTree::InitNodeInfo(this,cni);

	TreeNodeData* pNodeData = new TreeNodeData();
	pNodeData->m_emType = TreeNodeData::PARKINGLOT_Node;
	pNodeData->m_dwData = (DWORD)pParkingLot;
	cni.nt = NT_NORMAL;
	cni.net = NET_SHOW_DIALOGBOX;
	CString strCurbside;
	strCurbside.Format(_T("Parking Lot: %s"),pParkingLot->GetParkingLot().GetIDString());
	HTREEITEM hCurbsideItem = m_wndTreeCtrl.InsertItem(strCurbside,cni,FALSE,FALSE,hItem);
	m_wndTreeCtrl.SetItemData(hCurbsideItem,(DWORD)pNodeData);
	m_vNodeData.push_back(pNodeData);

	//insert first priority
	if(pParkingLot->GetCount() > 0)
	{
		LandsideParkingLotVehicleSinglePriority* pFirstPriority = pParkingLot->GetItem(0);
		InsertFirstPriorityItem(hCurbsideItem,pFirstPriority);
	}
	//single priority
	for (int i = 1; i < pParkingLot->GetCount(); i++)
	{
		LandsideParkingLotVehicleSinglePriority* pSinglePriority = pParkingLot->GetItem(i);
		InsertPriorityItem(hCurbsideItem,i+1,pSinglePriority);
	}

	//final priority
	pNodeData = new TreeNodeData();
	pNodeData->m_emType = TreeNodeData::FINALPRIORIY_Node;
	pNodeData->m_dwData = (DWORD)pParkingLot;
	cni.nt = NT_NORMAL;
	cni.net = NET_NORMAL;
	HTREEITEM hFinalItem = m_wndTreeCtrl.InsertItem("Final Priority",cni,FALSE,FALSE,hCurbsideItem);
	m_wndTreeCtrl.SetItemData(hFinalItem,(DWORD)pNodeData);
	m_vNodeData.push_back(pNodeData);
	{
		pNodeData = new TreeNodeData();
		pNodeData->m_emType = TreeNodeData::RETURN_Node;
		pNodeData->m_dwData = (DWORD)pParkingLot;
		cni.nt = NT_NORMAL;
		cni.net = NET_SHOW_DIALOGBOX;
		CString strRetRes;
		strRetRes.Format(_T("Return to Parking Lot: %s"),pParkingLot->GetFinalPriority().GetReturnParkingLot().GetIDString());
		HTREEITEM hRetItem = m_wndTreeCtrl.InsertItem(strRetRes,cni,FALSE,FALSE,hFinalItem);
		m_wndTreeCtrl.SetItemData(hRetItem,(DWORD)pNodeData);
		m_vNodeData.push_back(pNodeData);

		pNodeData = new TreeNodeData();
		pNodeData->m_emType = TreeNodeData::RECYCLE_Node;
		pNodeData->m_dwData = (DWORD)pParkingLot;
		cni.nt = NT_NORMAL;
		cni.net = NET_EDIT_INT;
		cni.fMinValue = static_cast<float>(pParkingLot->GetFinalPriority().GetRecyleLimit());
		CString strLimit;
		strLimit.Format(_T("Recycle Limit: %d"),pParkingLot->GetFinalPriority().GetRecyleLimit());
		HTREEITEM hLimitItem = m_wndTreeCtrl.InsertItem(strLimit,cni,FALSE,FALSE,hFinalItem);
		m_wndTreeCtrl.SetItemData(hLimitItem,(DWORD)pNodeData);
		m_vNodeData.push_back(pNodeData);

		pNodeData = new TreeNodeData();
		pNodeData->m_emType = TreeNodeData::PARKING_Node;
		pNodeData->m_dwData = (DWORD)pParkingLot;
		cni.nt = NT_NORMAL;
		cni.net = NET_SHOW_DIALOGBOX;
		CString strParkingLot;
		if (pParkingLot->GetFinalPriority().GetParkingLot().IsBlank())
		{
			strParkingLot = _T("Parking lot: All");
		}
		else
		{
			strParkingLot.Format(_T("Parking lot: %s"),pParkingLot->GetFinalPriority().GetParkingLot().GetIDString());
		}

		HTREEITEM hParkingItem = m_wndTreeCtrl.InsertItem(strParkingLot,cni,FALSE,FALSE,hFinalItem);
		m_wndTreeCtrl.SetItemData(hParkingItem,(DWORD)pNodeData);
		m_vNodeData.push_back(pNodeData);
	}
	m_wndTreeCtrl.Expand(hFinalItem,TVE_EXPAND);
	m_wndTreeCtrl.Expand(hCurbsideItem,TVE_EXPAND);
	m_wndTreeCtrl.Expand(hItem,TVE_EXPAND);
	return hCurbsideItem;
}

HTREEITEM CDlgParkingLotStrategy::InsertPriorityItem( HTREEITEM hItem, int idx,LandsideParkingLotVehicleSinglePriority* pSinglePriority )
{
	COOLTREE_NODE_INFO cni;
	CCoolTree::InitNodeInfo(this,cni);

	TreeNodeData* pNodeData = new TreeNodeData();
	pNodeData->m_emType = TreeNodeData::PRIORITY_Node;
	pNodeData->m_dwData = (DWORD)pSinglePriority;
	cni.nt = NT_NORMAL;
	cni.net = NET_NORMAL;
	CString strIndex;
	strIndex.Format(_T("Priority%d"),idx);
	HTREEITEM hPriorityItem = m_wndTreeCtrl.InsertItem(strIndex,cni,FALSE,FALSE,hItem);
	m_wndTreeCtrl.SetItemData(hPriorityItem,(DWORD)pNodeData);

	pNodeData = new TreeNodeData();
	pNodeData->m_emType = TreeNodeData::STOP_Node;
	pNodeData->m_dwData = (DWORD)pSinglePriority;
	cni.nt = NT_NORMAL;
	cni.net = NET_SHOW_DIALOGBOX;
	CString strCurbside;
	strCurbside.Format(_T("Park in: %s"),pSinglePriority->GetStopResource().GetIDString());
	HTREEITEM hCurbsideItem = m_wndTreeCtrl.InsertItem(strCurbside,cni,FALSE,FALSE,hPriorityItem);
	m_wndTreeCtrl.SetItemData(hCurbsideItem,(DWORD)pNodeData);
	m_vNodeData.push_back(pNodeData);

	pNodeData = new TreeNodeData();
	pNodeData->m_emType = TreeNodeData::SPOT_Node;
	pNodeData->m_dwData = (DWORD)pSinglePriority;
	cni.nt = NT_NORMAL;
	cni.net = NET_COMBOBOX;
	CString strSpot;
	strSpot.Format(_T("Change if wait for spot exceeds(secs): %s"),pSinglePriority->GetProDist().getPrintDist());
	HTREEITEM hRetItem = m_wndTreeCtrl.InsertItem(strSpot,cni,FALSE,FALSE,hPriorityItem);
	m_wndTreeCtrl.SetItemData(hRetItem,(DWORD)pNodeData);
	m_vNodeData.push_back(pNodeData);

	m_wndTreeCtrl.Expand(hPriorityItem,TVE_EXPAND);
	return hPriorityItem;
}

HTREEITEM CDlgParkingLotStrategy::InsertPrioritySpecifiedItem( HTREEITEM hItem, int idx,LandsideParkingLotVehicleSinglePriority* pSinglePriority )
{
	COOLTREE_NODE_INFO cni;
	CCoolTree::InitNodeInfo(this,cni);

	HTREEITEM hFinalItem;

	if(!FindFinalPriorityItem(hItem,hFinalItem))
		return hItem;

	TreeNodeData* pNodeData = new TreeNodeData();
	pNodeData->m_emType = TreeNodeData::PRIORITY_Node;
	pNodeData->m_dwData = (DWORD)pSinglePriority;
	cni.nt = NT_NORMAL;
	cni.net = NET_NORMAL;
	CString strIndex;
	strIndex.Format(_T("Priority%d"),idx);
	HTREEITEM hPriorityItem = m_wndTreeCtrl.InsertItem(strIndex,cni,FALSE,FALSE,hItem,hFinalItem);
	m_wndTreeCtrl.SetItemData(hPriorityItem,(DWORD)pNodeData);

	pNodeData = new TreeNodeData();
	pNodeData->m_emType = TreeNodeData::STOP_Node;
	pNodeData->m_dwData = (DWORD)pSinglePriority;
	cni.nt = NT_NORMAL;
	cni.net = NET_SHOW_DIALOGBOX;
	CString strCurbside;
	strCurbside.Format(_T("Park in: %s"),pSinglePriority->GetStopResource().GetIDString());
	HTREEITEM hCurbsideItem = m_wndTreeCtrl.InsertItem(strCurbside,cni,FALSE,FALSE,hPriorityItem);
	m_wndTreeCtrl.SetItemData(hCurbsideItem,(DWORD)pNodeData);
	m_vNodeData.push_back(pNodeData);

	pNodeData = new TreeNodeData();
	pNodeData->m_emType = TreeNodeData::SPOT_Node;
	pNodeData->m_dwData = (DWORD)pSinglePriority;
	cni.nt = NT_NORMAL;
	cni.net = NET_COMBOBOX;
	CString strSpot;
	strSpot.Format(_T("Change if wait for spot exceeds(secs): %s"),pSinglePriority->GetProDist().getPrintDist());
	HTREEITEM hRetItem = m_wndTreeCtrl.InsertItem(strSpot,cni,FALSE,FALSE,hPriorityItem);
	m_wndTreeCtrl.SetItemData(hRetItem,(DWORD)pNodeData);
	m_vNodeData.push_back(pNodeData);

	m_wndTreeCtrl.Expand(hPriorityItem,TVE_EXPAND);
	return hPriorityItem;
}

HTREEITEM CDlgParkingLotStrategy::InsertFirstPriorityItem( HTREEITEM hItem, LandsideParkingLotVehicleSinglePriority* pSinglePriority )
{
	COOLTREE_NODE_INFO cni;
	CCoolTree::InitNodeInfo(this,cni);

	TreeNodeData* pNodeData = new TreeNodeData();
	pNodeData->m_emType = TreeNodeData::FIRST_Node;
	pNodeData->m_dwData = (DWORD)pSinglePriority;
	cni.nt = NT_NORMAL;
	cni.net = NET_NORMAL;
	HTREEITEM hPriorityItem = m_wndTreeCtrl.InsertItem("Priority1",cni,FALSE,FALSE,hItem);
	m_wndTreeCtrl.SetItemData(hPriorityItem,(DWORD)pNodeData);

	pNodeData = new TreeNodeData();
	pNodeData->m_emType = TreeNodeData::STOP_Node;
	pNodeData->m_dwData = (DWORD)pSinglePriority;
	cni.nt = NT_NORMAL;
	cni.net = NET_NORMAL;
	CString strCurbside;
	strCurbside.Format(_T("Park in Parking Lot: %s"),pSinglePriority->GetStopResource().GetIDString());
	HTREEITEM hCurbsideItem = m_wndTreeCtrl.InsertItem(strCurbside,cni,FALSE,FALSE,hPriorityItem);
	m_wndTreeCtrl.SetItemData(hCurbsideItem,(DWORD)pNodeData);
	m_vNodeData.push_back(pNodeData);

	pNodeData = new TreeNodeData();
	pNodeData->m_emType = TreeNodeData::SPOT_Node;
	pNodeData->m_dwData = (DWORD)pSinglePriority;
	cni.nt = NT_NORMAL;
	cni.net = NET_COMBOBOX;
	CString strSpot;
	strSpot.Format(_T("Change if wait for spot exceeds(secs): %s"),pSinglePriority->GetProDist().getPrintDist());
	HTREEITEM hRetItem = m_wndTreeCtrl.InsertItem(strSpot,cni,FALSE,FALSE,hPriorityItem);
	m_wndTreeCtrl.SetItemData(hRetItem,(DWORD)pNodeData);
	m_vNodeData.push_back(pNodeData);

	m_wndTreeCtrl.Expand(hPriorityItem,TVE_EXPAND);
	return hPriorityItem;
}

void CDlgParkingLotStrategy::OnAddTreeItem()
{
	HTREEITEM hItem = m_wndTreeCtrl.GetSelectedItem();
	if (hItem == NULL)
	{
		CDlgSelectLandsideVehicleType dlg;
		if(dlg.DoModal() == IDOK)
		{
			CString strVehicleType = dlg.GetName();
			if (!strVehicleType.IsEmpty() && !m_parkingLotContainer.ExistSameVehicleType(strVehicleType))//non empty
			{
				LandsideParkingLotVehicleStrategy* pVehicle = new LandsideParkingLotVehicleStrategy();
				pVehicle->SetVehicleType(strVehicleType);
				m_parkingLotContainer.AddItem(pVehicle);
				HTREEITEM hVehicleItem = InsertVehicleItem(TVI_ROOT,pVehicle);
				m_wndTreeCtrl.SelectItem(hVehicleItem);
				m_wndTreeCtrl.SetItemState(hVehicleItem,TVIS_SELECTED,TVIS_SELECTED);
				GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
			}
			else
			{
				MessageBox(_T("Exist the same vehicle type!!!"),_T("Warning"),MB_OK);
			}
		}
		return;
	}

	TreeNodeData* pNodeData = (TreeNodeData*)m_wndTreeCtrl.GetItemData(hItem);
	if (pNodeData == NULL)
		return;

	switch (pNodeData->m_emType)
	{
	case TreeNodeData::VEHICLE_Node:
		{
			LandsideParkingLotVehicleStrategy* pVehicle = (LandsideParkingLotVehicleStrategy*)(pNodeData->m_dwData);
			if (pVehicle == NULL)
				return;

			LandsideFacilityLayoutObjectList* pLayoutList = GetLandsideLayoutObjectList();
			CDlgSelectLandsideObject dlg(GetLandsideLayoutObjectList(),this);
			dlg.AddObjType(ALT_LPARKINGLOT);
			if(dlg.DoModal() == IDOK)
			{
				if (pLayoutList->getObjectByName(dlg.getSelectObject()))
				{
					CString strObject = dlg.getSelectObject().GetIDString();
					if (pVehicle->ExistSameParkingLot(strObject))
					{
						MessageBox(_T("Exist the same parking lot item!!!"),_T("Warning"),MB_OK);
					}
					else
					{
						LandsideParkingLotSettingData* pParkingLot = new LandsideParkingLotSettingData();
						ALTObjectID parkingName;
						parkingName.FromString(strObject);
						pParkingLot->SetParkingLot(parkingName);
						pParkingLot->InitDefaultData();
						pVehicle->AddItem(pParkingLot);
						HTREEITEM hParkingLotItem = InsertParkingLotItem(hItem,pParkingLot);
						m_wndTreeCtrl.SelectItem(hParkingLotItem);
						m_wndTreeCtrl.SetItemState(hParkingLotItem,TVIS_SELECTED,TVIS_SELECTED);
						GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
					}
				}
				else
				{
					MessageBox(_T("Can't select group of parking lot!!!"),_T("Warning"),MB_OK);
				}
			}
		}
		break;
	case TreeNodeData::PARKINGLOT_Node:
		{
			LandsideParkingLotSettingData* pParkingLot = (LandsideParkingLotSettingData*)(pNodeData->m_dwData);
			if (pParkingLot == NULL)
				return;

			LandsideFacilityLayoutObjectList* pLayoutList = GetLandsideLayoutObjectList();
			CDlgSelectLandsideObject dlg(GetLandsideLayoutObjectList(),this);
			dlg.AddObjType(ALT_LCURBSIDE);
			dlg.AddObjType(ALT_LPARKINGLOT);
			if(dlg.DoModal() == IDOK)
			{
				if (pLayoutList->getObjectByName(dlg.getSelectObject()))
				{
					CString strObject = dlg.getSelectObject().GetIDString();
					if (pParkingLot->ExistSameStopResource(strObject))
					{
						MessageBox(_T("Exist the same priority item!!!"),_T("Warning"),MB_OK);
					}
					else
					{
						LandsideParkingLotVehicleSinglePriority* pSinglePriority = new LandsideParkingLotVehicleSinglePriority();
						ALTObjectID stopRes;
						stopRes.FromString(strObject);
						pSinglePriority->SetStopResource(stopRes);
						int idx = pParkingLot->GetCount();
						pParkingLot->AddItem(pSinglePriority);

						HTREEITEM hPriorityItem = InsertPrioritySpecifiedItem(hItem,idx+1,pSinglePriority);
						m_wndTreeCtrl.SelectItem(hPriorityItem);
						m_wndTreeCtrl.SetItemState(hPriorityItem,TVIS_SELECTED,TVIS_SELECTED);
						GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
					}
				}
				else 
				{
					MessageBox(_T("Can't select group of facility!!!"),_T("Warning"),MB_OK);
				}
			}
		}
		break;
	default:
		break;
	}
}

void CDlgParkingLotStrategy::OnDeleteTreeItem()
{
	HTREEITEM hItem = m_wndTreeCtrl.GetSelectedItem();
	if (hItem == NULL)
		return;

	TreeNodeData* pNodeData = (TreeNodeData*)m_wndTreeCtrl.GetItemData(hItem);
	if (pNodeData == NULL)
		return;

	switch (pNodeData->m_emType)
	{
	case TreeNodeData::VEHICLE_Node:
		{
			LandsideParkingLotVehicleStrategy* pVehicle = (LandsideParkingLotVehicleStrategy*)(pNodeData->m_dwData);
			m_parkingLotContainer.DeleteItem(pVehicle);
			m_wndTreeCtrl.DeleteItem(hItem);
			DeleteTreeNodeData(pNodeData);
			GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
		}
		break;
	case TreeNodeData::PARKINGLOT_Node:
		{
			HTREEITEM hParentItem = m_wndTreeCtrl.GetParentItem(hItem);
			if (hParentItem == NULL)
				return;
			TreeNodeData* pParentData = (TreeNodeData*)m_wndTreeCtrl.GetItemData(hParentItem);
			if (pParentData == NULL)
				return;

			LandsideParkingLotVehicleStrategy* pVehicle = (LandsideParkingLotVehicleStrategy*)(pParentData->m_dwData);
			if (pVehicle == NULL)
				return;

			LandsideParkingLotSettingData* pParkingLot = (LandsideParkingLotSettingData*)(pNodeData->m_dwData);
			pVehicle->DeleteItem(pParkingLot);
			m_wndTreeCtrl.DeleteItem(hItem);
			DeleteTreeNodeData(pNodeData);
			GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
		}
		break;
	case TreeNodeData::PRIORITY_Node:
		{
			HTREEITEM hParentItem = m_wndTreeCtrl.GetParentItem(hItem);
			if (hParentItem == NULL)
				return;
			TreeNodeData* pParentData = (TreeNodeData*)m_wndTreeCtrl.GetItemData(hParentItem);
			if (pParentData == NULL)
				return;

			LandsideParkingLotSettingData* pParkingLot = (LandsideParkingLotSettingData*)(pParentData->m_dwData);
			if (pParkingLot == NULL)
				return;

			LandsideParkingLotVehicleSinglePriority* pSinglePriority = (LandsideParkingLotVehicleSinglePriority*)(pNodeData->m_dwData);
			int idx = pParkingLot->FindItem(pSinglePriority);
			HTREEITEM hBrotherItem = m_wndTreeCtrl.GetNextSiblingItem(hItem);
			while(hBrotherItem)
			{
				TreeNodeData* pBrotherData = (TreeNodeData*)m_wndTreeCtrl.GetItemData(hBrotherItem);
				if (pBrotherData && pBrotherData->m_emType == TreeNodeData::PRIORITY_Node)
				{
					CString strTitle;
					strTitle.Format(_T("Priority%d"),++idx);
					m_wndTreeCtrl.SetItemText(hBrotherItem,strTitle);
				}
				hBrotherItem = m_wndTreeCtrl.GetNextSiblingItem(hBrotherItem);
			}

			pParkingLot->DeleteItem(pSinglePriority);
			m_wndTreeCtrl.DeleteItem(hItem);
			DeleteTreeNodeData(pNodeData);
			GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
		}
		break;
	default:
		ASSERT(FALSE);
		break;
	}
}

void CDlgParkingLotStrategy::OnDisplayMenu(TreeNodeData* pNodeData,CPoint point)
{
	CMenu menuPopup; 
	menuPopup.CreatePopupMenu();
	switch (pNodeData->m_emType)
	{
	case TreeNodeData::VEHICLE_Node:
		{
			menuPopup.AppendMenu(MF_POPUP,ID_TOOLBAR_NEW,_T("Add Parking Lot"));
			menuPopup.AppendMenu(MF_POPUP,ID_TOOLBAR_DELETE,_T("Delete Vehicle Type"));	
			menuPopup.TrackPopupMenu(TPM_LEFTALIGN, point.x, point.y, this);
		}
		break;
	case TreeNodeData::PARKINGLOT_Node:
		{
			menuPopup.AppendMenu(MF_POPUP,ID_TOOLBAR_NEW,_T("Add Priority"));
			menuPopup.AppendMenu(MF_POPUP,ID_TOOLBAR_DELETE,_T("Delete Parking Lot"));
			menuPopup.TrackPopupMenu(TPM_LEFTALIGN, point.x, point.y, this);
		}
		break;
	case TreeNodeData::PRIORITY_Node:
		{
			menuPopup.AppendMenu(MF_POPUP,ID_TOOLBAR_DELETE,_T("Delete Priority"));
			menuPopup.TrackPopupMenu(TPM_LEFTALIGN, point.x, point.y, this);
		}
		break;
	default:
		break;
	}
	menuPopup.DestroyMenu();
}

void CDlgParkingLotStrategy::UpdateToolbar(HTREEITEM hItem)
{
	TreeNodeData* pNodeData = (TreeNodeData*)m_wndTreeCtrl.GetItemData(hItem);
	if (pNodeData) 
	{
		//check whether can delete
		if (std::find(m_canDeleteNode.begin(),m_canDeleteNode.end(),pNodeData->m_emType) != m_canDeleteNode.end())
		{
			m_wndToolbar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_DELETE,TRUE);
		}
		else
		{
			m_wndToolbar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_DELETE,FALSE);
		}

		//check whether can new
		if (std::find(m_canNewNode.begin(),m_canNewNode.end(),pNodeData->m_emType) != m_canNewNode.end())
		{
			m_wndToolbar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_NEW,TRUE);
		}
		else
		{
			m_wndToolbar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_NEW,FALSE);
		}
		return;
	}
}

void CDlgParkingLotStrategy::UpdateDeleteOperationToolbar()
{
	HTREEITEM hRootItem = m_wndTreeCtrl.GetRootItem();
	if (hRootItem)
	{
		if(m_wndTreeCtrl.GetNextSiblingItem(hRootItem) == NULL)
		{
			m_wndToolbar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_NEW,TRUE);
			m_wndToolbar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_DELETE,FALSE);
		}
	}
}

void CDlgParkingLotStrategy::HandleTreeMessage( UINT message, WPARAM wParam, LPARAM lParam )
{
	switch (message)
	{
	case UM_CEW_EDITSPIN_BEGIN:
		{
			HTREEITEM hItem = (HTREEITEM)wParam;
			COOLTREE_NODE_INFO* cni = m_wndTreeCtrl.GetItemNodeInfo(hItem);
			TreeNodeData* pNodeData = (TreeNodeData*)m_wndTreeCtrl.GetItemData(hItem);
			if (pNodeData && pNodeData->m_emType == TreeNodeData::RECYCLE_Node)
			{
				cni->fMinValue = 0.0;
			}

		}
		break;
	case UM_CEW_EDITSPIN_END:
		{
			HTREEITEM hItem = (HTREEITEM)wParam;
			CString strValue = *((CString*)lParam);
			TreeNodeData* pNodeData = (TreeNodeData*)m_wndTreeCtrl.GetItemData(hItem);
			COOLTREE_NODE_INFO* cni = m_wndTreeCtrl.GetItemNodeInfo(hItem);
			int nValue = atoi(strValue);
			if (pNodeData && pNodeData->m_emType == TreeNodeData::RECYCLE_Node)
			{
				LandsideParkingLotSettingData* pParkingLot = (LandsideParkingLotSettingData*)(pNodeData->m_dwData);
				LandsideParkingLotVehicleFinalPriority& pFinalPriority = pParkingLot->GetFinalPriority();
				pFinalPriority.SetRecyleLimit(nValue);

				CString strLimit;
				strLimit.Format(_T("Recycle Limit: %d"),nValue);
				cni->fMinValue = static_cast<float>(nValue);
				m_wndTreeCtrl.SetItemText(hItem,strLimit);
				GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
			}
		}
		break;
	case UM_CEW_COMBOBOX_BEGIN:
		{
			HTREEITEM hItem=(HTREEITEM)wParam;
			TreeNodeData* pNodeData = (TreeNodeData*)m_wndTreeCtrl.GetItemData(hItem);
			if(pNodeData && pNodeData->m_emType == TreeNodeData::SPOT_Node)
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

				CString s;
				s.LoadString(IDS_STRING_NEWDIST);
				pCB->AddString(s);

				InputLandside* pInputLandside = GetInputLandside();
				if (pInputLandside)
				{
					int nCount = _g_GetActiveProbMan(  pInputLandside->getInputTerminal() )->getCount();
					for( int m=0; m<nCount; m++ )
					{
						CProbDistEntry* pPBEntry = _g_GetActiveProbMan( pInputLandside->getInputTerminal() )->getItem( m );		
						pCB->AddString(pPBEntry->m_csName);
					}
				}

			}
		}
		break;
	case UM_CEW_COMBOBOX_SELCHANGE:
		{
			HTREEITEM hItem=(HTREEITEM)wParam;
			TreeNodeData* pNodeData = (TreeNodeData*)m_wndTreeCtrl.GetItemData(hItem);
			int iSelCombox = m_wndTreeCtrl.GetCmbBoxCurSel(hItem);

			if(pNodeData && pNodeData->m_emType == TreeNodeData::SPOT_Node)
			{
				InputLandside* pInputLandside = GetInputLandside();
				if (pInputLandside)
				{
					LandsideParkingLotVehicleSinglePriority* pSinglePriority = (LandsideParkingLotVehicleSinglePriority*)(pNodeData->m_dwData);
					if (pSinglePriority)
					{
						if(iSelCombox == 0)
						{
							CDlgProbDist dlg(  pInputLandside->getInputTerminal()->m_pAirportDB, true );
							if(dlg.DoModal()==IDOK) 
							{
								int idxPD = dlg.GetSelectedPDIdx();
								ASSERT(idxPD!=-1);
								CProbDistEntry* pde = _g_GetActiveProbMan( pInputLandside->getInputTerminal() )->getItem(idxPD);

								pSinglePriority->SetProDist(pde);
								CString strSpot;
								strSpot.Format(_T("Change if wait for spot exceeds(secs): %s"),pSinglePriority->GetProDist().getPrintDist());
								m_wndTreeCtrl.SetItemText(hItem,strSpot);
								GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
							}
						}
						else
						{
							ASSERT( iSelCombox-1 >= 0 && iSelCombox-1 < static_cast<int>(_g_GetActiveProbMan( pInputLandside->getInputTerminal()  )->getCount()) );
							CProbDistEntry* pPBEntry = _g_GetActiveProbMan( pInputLandside->getInputTerminal()  )->getItem( iSelCombox-1 );
							pSinglePriority->SetProDist(pPBEntry);
							CString strSpot;
							strSpot.Format(_T("Change if wait for spot exceeds(secs): %s"),pSinglePriority->GetProDist().getDistName());
							m_wndTreeCtrl.SetItemText(hItem,strSpot);
							GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);

						}
					}

				}

			}
		}
		break;
	default:
		break;
	}
}

void CDlgParkingLotStrategy::HandLDoubleClickMessage( WPARAM wParam, LPARAM lParam )
{
	HTREEITEM hItem = m_wndTreeCtrl.GetSelectedItem();
	if(hItem)
	{
		TreeNodeData* pNodeData = (TreeNodeData*)m_wndTreeCtrl.GetItemData(hItem);
		if (pNodeData)
		{
			if (pNodeData->m_emType == TreeNodeData::VEHICLE_Node)
			{
				CDlgSelectLandsideVehicleType dlg;
				if(dlg.DoModal() == IDOK)
				{
					CString strVehicleType = dlg.GetName();
					LandsideParkingLotVehicleStrategy* pVehicle = (LandsideParkingLotVehicleStrategy*)(pNodeData->m_dwData);
					if(pVehicle && pVehicle->GetVehicleType() != strVehicleType)
					{
						if (!strVehicleType.IsEmpty() && !m_parkingLotContainer.ExistSameVehicleType(strVehicleType))//non empty
						{
							pVehicle->SetVehicleType(strVehicleType);
							CString strGroupName;
							strGroupName.Format(_T("Vehicle Type: %s"),pVehicle->GetVehicleType());
							m_wndTreeCtrl.SetItemText(hItem,strGroupName);
							GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
						}
						else
						{
							MessageBox(_T("Exist the same vehicle type!!!"),_T("Warning"),MB_OK);
						}
					}

				}
			}
			else if (pNodeData->m_emType == TreeNodeData::PARKINGLOT_Node)
			{
				LandsideFacilityLayoutObjectList* pLayoutList = GetLandsideLayoutObjectList();
				CDlgSelectLandsideObject dlg(GetLandsideLayoutObjectList(),this);
				dlg.AddObjType(ALT_LPARKINGLOT);
				if(dlg.DoModal() == IDOK)
				{
					if (pLayoutList->getObjectByName(dlg.getSelectObject()))
					{
						CString strObject = dlg.getSelectObject().GetIDString();
						HTREEITEM hParentItem = m_wndTreeCtrl.GetParentItem(hItem);
						if (hParentItem)
						{
							TreeNodeData* pParentData = (TreeNodeData*)m_wndTreeCtrl.GetItemData(hParentItem);
							if (pParentData && pParentData->m_emType == TreeNodeData::VEHICLE_Node)
							{
								LandsideParkingLotVehicleStrategy* pVehicle = (LandsideParkingLotVehicleStrategy*)(pParentData->m_dwData);
								if (pVehicle && pVehicle->GetVehicleType() != strObject)
								{
									if (pVehicle->ExistSameParkingLot(strObject))
									{
										MessageBox(_T("Exist the same parking lot item!!!"),_T("Warning"),MB_OK);
									}
									else
									{
										LandsideParkingLotSettingData* pParkingLot = (LandsideParkingLotSettingData*)(pNodeData->m_dwData);
										ALTObjectID parkingName;
										parkingName.FromString(strObject);
										pParkingLot->SetParkingLot(parkingName);

										CString strCurbside;
										strCurbside.Format(_T("Parking Lot: %s"),pParkingLot->GetParkingLot().GetIDString());
										m_wndTreeCtrl.SetItemText(hItem,strCurbside);

										HTREEITEM hChildItem = m_wndTreeCtrl.GetChildItem(hItem);
										if (hChildItem)
										{
											TreeNodeData* pChildData = (TreeNodeData*)m_wndTreeCtrl.GetItemData(hChildItem);
											if (pChildData && pChildData->m_emType == TreeNodeData::FIRST_Node)
											{
												LandsideParkingLotVehicleSinglePriority* pSinglePriority = (LandsideParkingLotVehicleSinglePriority*)(pChildData->m_dwData);
												pSinglePriority->SetStopResource(parkingName);

												HTREEITEM hParkingItem = m_wndTreeCtrl.GetChildItem(hChildItem);
												if (hParkingItem)
												{
													CString strCurbside;
													strCurbside.Format(_T("Park in Parking Lot: %s"),pSinglePriority->GetStopResource().GetIDString());
													m_wndTreeCtrl.SetItemText(hParkingItem,strCurbside);
												}
											}
										}

										GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
									}
								}

							}
						}
					}
					else
					{
						MessageBox(_T("Can't select group of parking lot!!!"),_T("Warning"),MB_OK);
					}
				}
			}
			else if (pNodeData->m_emType == TreeNodeData::STOP_Node)
			{
				HTREEITEM hParentItem = m_wndTreeCtrl.GetParentItem(hItem);
				TreeNodeData* pParentData = (TreeNodeData*)m_wndTreeCtrl.GetItemData(hParentItem);
				if (pParentData && pParentData->m_emType != TreeNodeData::FIRST_Node)
				{
					LandsideFacilityLayoutObjectList* pLayoutList = GetLandsideLayoutObjectList();
					CDlgSelectLandsideObject dlg(GetLandsideLayoutObjectList(),this);
					dlg.AddObjType(ALT_LCURBSIDE);
					dlg.AddObjType(ALT_LPARKINGLOT);
					if(dlg.DoModal() == IDOK)
					{
						if (pLayoutList->getObjectByName(dlg.getSelectObject()))
						{
							HTREEITEM hParentItem = m_wndTreeCtrl.GetParentItem(hItem);
							ASSERT(hParentItem);
							HTREEITEM hParkingLotItem = m_wndTreeCtrl.GetParentItem(hParentItem);
							ASSERT(hParkingLotItem);
							TreeNodeData* pParentData = (TreeNodeData*)m_wndTreeCtrl.GetItemData(hParkingLotItem);
							LandsideParkingLotSettingData* pParkingLot = (LandsideParkingLotSettingData*)(pParentData->m_dwData);
							ASSERT(pParkingLot);
							LandsideParkingLotVehicleSinglePriority* pSinglePriority = (LandsideParkingLotVehicleSinglePriority*)(pNodeData->m_dwData);
							if (pSinglePriority)
							{
								CString strObject = dlg.getSelectObject().GetIDString();
								if (pSinglePriority->GetStopResource().GetIDString() != strObject && pParkingLot->ExistSameStopResource(strObject))
								{
									MessageBox(_T("Exist the same priority item!!!"),_T("Warning"),MB_OK);
								}
								else
								{
									ALTObjectID stopRes;
									stopRes.FromString(strObject);
									pSinglePriority->SetStopResource(stopRes);
									CString strCurbside;
									strCurbside.Format(_T("Park in: %s"),pSinglePriority->GetStopResource().GetIDString());
									m_wndTreeCtrl.SetItemText(hItem,strCurbside);
									GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
								}
								
							}
						}
						else 
						{
							MessageBox(_T("Can't select group of facility!!!"),_T("Warning"),MB_OK);
						}
					}
				}	
			}
			else if (pNodeData->m_emType == TreeNodeData::RETURN_Node)
			{	
				LandsideFacilityLayoutObjectList* pLayoutList = GetLandsideLayoutObjectList();
				CDlgSelectLandsideObject dlg(GetLandsideLayoutObjectList(),this);
				dlg.AddObjType(ALT_LPARKINGLOT);
				if(dlg.DoModal() == IDOK)
				{
					if (pLayoutList->getObjectByName(dlg.getSelectObject()))
					{
						LandsideParkingLotSettingData* pParkingLot = (LandsideParkingLotSettingData*)(pNodeData->m_dwData);
						if (pParkingLot)
						{
							CString strObject = dlg.getSelectObject().GetIDString();
							ALTObjectID retRes;
							retRes.FromString(strObject);
							LandsideParkingLotVehicleFinalPriority& finalPriority = pParkingLot->GetFinalPriority();
							finalPriority.SetReturnParkingLot(retRes);
							CString strRetRes;
							strRetRes.Format(_T("Return to Parking Lot: %s"),pParkingLot->GetFinalPriority().GetReturnParkingLot().GetIDString());
							m_wndTreeCtrl.SetItemText(hItem,strRetRes);
							GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
						}
					}
					else
					{
						MessageBox(_T("Can't select group of parking lot!!!"),_T("Warning"),MB_OK);
					}
				}
			}
			else if (pNodeData->m_emType == TreeNodeData::PARKING_Node)
			{
				LandsideFacilityLayoutObjectList* pLayoutList = GetLandsideLayoutObjectList();
				CDlgSelectLandsideObject dlg(GetLandsideLayoutObjectList(),this);
				dlg.AddObjType(ALT_LPARKINGLOT);
				if(dlg.DoModal() == IDOK)
				{
					if (pLayoutList->getObjectByName(dlg.getSelectObject()))
					{
						LandsideParkingLotSettingData* pParkingLot = (LandsideParkingLotSettingData*)(pNodeData->m_dwData);
						if (pParkingLot)
						{
							CString strObject = dlg.getSelectObject().GetIDString();
							ALTObjectID stopRes;
							stopRes.FromString(strObject);
							LandsideParkingLotVehicleFinalPriority& finalPriority = pParkingLot->GetFinalPriority();
							finalPriority.SetParkingLot(stopRes);
							CString strParkingLot;
							strParkingLot.Format(_T("Parking lot: %s"),pParkingLot->GetFinalPriority().GetParkingLot().GetIDString());
							m_wndTreeCtrl.SetItemText(hItem,strParkingLot);
							GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
						}
					}
					else
					{
						MessageBox(_T("Can't select group of parking lot!!!"),_T("Warning"),MB_OK);
					}
				}
			}
		}
	}
}

void CDlgParkingLotStrategy::SetTitleHeard()
{
	SetWindowText(_T("Parking Lot Strategies Definition"));
}
