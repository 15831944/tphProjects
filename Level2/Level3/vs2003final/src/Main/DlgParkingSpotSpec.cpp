// DlgParkingSpotSpec.cpp : implementation file
//

#include "stdafx.h"
#include "TermPlan.h"
#include "DlgParkingSpotSpec.h"
#include <algorithm>
#include "DlgSelectLandsideVehicleType.h"
#include "DlgTimeRange.h"
#include "DlgSelectLandsideObject.h"
#include "TermPlanDoc.h"
#include "landside/LandsideLayoutObject.h"
#include "landside/InputLandside.h"
#include "Landside/LandsideParkingLot.h"

// CDlgParkingSpotSpec dialog
namespace
{
	const UINT ID_PARKINGLOT_NEW = 10;
	const UINT ID_PARKINGLOT_DEL = 11;

	const UINT ID_MENU_ADD = 15;
	const UINT ID_MENU_DEL = 16;
	const UINT ID_MENU_VEHICLE = 17;
}

IMPLEMENT_DYNAMIC(CDlgParkingSpotSpec, CXTResizeDialog)
CDlgParkingSpotSpec::CDlgParkingSpotSpec(CWnd* pParent /*=NULL*/)
	: CXTResizeDialog(CDlgParkingSpotSpec::IDD, pParent)
{
}

CDlgParkingSpotSpec::~CDlgParkingSpotSpec()
{
	ClearTreeNodeData();
}

void CDlgParkingSpotSpec::DoDataExchange(CDataExchange* pDX)
{
	CXTResizeDialog::DoDataExchange(pDX);
	DDX_Control(pDX,IDC_TREE_DATA,m_wndTreeCtrl);
}


BEGIN_MESSAGE_MAP(CDlgParkingSpotSpec, CXTResizeDialog)
	ON_WM_CREATE()
	ON_WM_CONTEXTMENU()
	ON_NOTIFY(TVN_SELCHANGED,IDC_TREE_DATA,OnLvnItemchangedList)
	ON_BN_CLICKED(IDC_BUTTON_SAVE,OnBtnSave)
	ON_COMMAND(ID_PARKINGLOT_NEW,OnCmdNewItem)
	ON_COMMAND(ID_PARKINGLOT_DEL,OnCmdDeleteItem)
	ON_COMMAND(ID_MENU_ADD,OnAddParkingSpot)
	ON_COMMAND(ID_MENU_DEL,OnDeleteParkingSpot)
	ON_COMMAND(ID_MENU_VEHICLE, OnAddVehicleType)
END_MESSAGE_MAP()

// CDlgParkingSpotSpec message handlers
BOOL CDlgParkingSpotSpec::OnInitDialog()
{
	CXTResizeDialog::OnInitDialog();

	m_landsideParkingSpotCon.ReadData(-1);

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

int CDlgParkingSpotSpec::OnCreate( LPCREATESTRUCT lpCreateStruct )
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

void CDlgParkingSpotSpec::OnOK()
{
	CString strError;
	if (!m_landsideParkingSpotCon.GetErrorMessage(strError))
	{
		MessageBox(strError,_T("Warning"),MB_OK);
		return;
	}
	try
	{
		CADODatabase::BeginTransaction();
		m_landsideParkingSpotCon.SaveData(-1);
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

void CDlgParkingSpotSpec::OnBtnSave()
{
	CString strError;
	if (!m_landsideParkingSpotCon.GetErrorMessage(strError))
	{
		MessageBox(strError,_T("Warning"),MB_OK);
		return;
	}

	try
	{
		CADODatabase::BeginTransaction();
		m_landsideParkingSpotCon.SaveData(-1);
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

void CDlgParkingSpotSpec::OnCmdNewItem()
{
	ParkingSpotGroup* pSpotGroup = new ParkingSpotGroup();
	m_landsideParkingSpotCon.AddNewItem(pSpotGroup);
	InsertSpotGroupItem(TVI_ROOT,pSpotGroup);
	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
}

void CDlgParkingSpotSpec::OnCmdDeleteItem()
{
	HTREEITEM hItem = m_wndTreeCtrl.GetSelectedItem();
	if (hItem == NULL)
		return;

	TreeNodeData* pNodeData = (TreeNodeData*)m_wndTreeCtrl.GetItemData(hItem);
	if (pNodeData == NULL)
		return;
	
	ParkingSpotGroup* pSpotGroup = (ParkingSpotGroup*)(pNodeData->m_dwData);
	m_landsideParkingSpotCon.DeleteItem(pSpotGroup);
	DeleteTreeNodeData(pNodeData);
	
	m_wndTreeCtrl.DeleteItem(hItem);
	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
}

void CDlgParkingSpotSpec::OnLvnItemchangedList( NMHDR *pNMHDR, LRESULT *pResult )
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	*pResult = 0;

	if (pNMTreeView->itemNew.hItem)
	{
		if (pNMTreeView->itemNew.hItem != pNMTreeView->itemOld.hItem)
		{
			TreeNodeData* pNodeData = (TreeNodeData*)m_wndTreeCtrl.GetItemData(pNMTreeView->itemNew.hItem);
			if (pNodeData && pNodeData->m_emType == TreeNodeData::SpotGroup_Node) 
			{
				m_wndToolbar.GetToolBarCtrl().EnableButton(ID_PARKINGLOT_NEW,TRUE);
				m_wndToolbar.GetToolBarCtrl().EnableButton(ID_PARKINGLOT_DEL,TRUE);
				return;
			}
	
		}
	}

	m_wndToolbar.GetToolBarCtrl().EnableButton(ID_PARKINGLOT_NEW,TRUE);
	m_wndToolbar.GetToolBarCtrl().EnableButton(ID_PARKINGLOT_DEL,FALSE);
}

void CDlgParkingSpotSpec::OnContextMenu( CWnd* pWnd, CPoint point )
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
	case TreeNodeData::SpotGroup_Node:
		{
			menuPopup.AppendMenu(MF_POPUP,ID_MENU_ADD,_T("Select Time Window"));
			menuPopup.AppendMenu(MF_POPUP,ID_MENU_VEHICLE,_T("Select Vehicle Type"));
			menuPopup.AppendMenu(MF_POPUP,ID_MENU_DEL,_T("Delete Spot Group"));
			menuPopup.TrackPopupMenu(TPM_LEFTALIGN, point.x, point.y, this);
		}
		break;
	case TreeNodeData::TimeRange_Node:
		{
			menuPopup.AppendMenu(MF_POPUP,ID_MENU_ADD,_T("Select Parking Lot"));
			menuPopup.AppendMenu(MF_POPUP,ID_MENU_DEL,_T("Delete Time Window"));
			menuPopup.TrackPopupMenu(TPM_LEFTALIGN, point.x, point.y, this);
		}
		break;
	case TreeNodeData::VehicleType_Node:
		{
			menuPopup.AppendMenu(MF_POPUP,ID_MENU_DEL,_T("Delete Vehicle Type"));
			menuPopup.TrackPopupMenu(TPM_LEFTALIGN, point.x, point.y, this);
		}
	case TreeNodeData::ParkingLot_Node:
		{
			menuPopup.AppendMenu(MF_POPUP,ID_MENU_ADD,_T("Select Parking Lot Level"));
			menuPopup.AppendMenu(MF_POPUP,ID_MENU_DEL,_T("Delete Parking Lot"));
			menuPopup.TrackPopupMenu(TPM_LEFTALIGN, point.x, point.y, this);
		}
		break;
	case TreeNodeData::ParkingLotLevel_Node:
		{
			menuPopup.AppendMenu(MF_POPUP,ID_MENU_ADD,_T("Select Parking Lot Row"));
			menuPopup.AppendMenu(MF_POPUP,ID_MENU_DEL,_T("Delete Parking Lot Level"));
			menuPopup.TrackPopupMenu(TPM_LEFTALIGN, point.x, point.y, this);
		}
		break;
	case TreeNodeData::ParkingLotRow_Node:
		{
			menuPopup.AppendMenu(MF_POPUP,ID_MENU_DEL,_T("Delete Parking Lot Row"));
			menuPopup.TrackPopupMenu(TPM_LEFTALIGN, point.x, point.y, this);
		}
		break;
	}
	menuPopup.DestroyMenu();
}

void CDlgParkingSpotSpec::OnAddVehicleType()
{
	HTREEITEM hItem = m_wndTreeCtrl.GetSelectedItem();
	if (hItem == NULL)
		return;

	TreeNodeData* pNodeData = (TreeNodeData*)m_wndTreeCtrl.GetItemData(hItem);
	if (pNodeData == NULL)
		return;

	if (pNodeData->m_emType == TreeNodeData::SpotGroup_Node)
	{
		CDlgSelectLandsideVehicleType dlg;
		if(dlg.DoModal() == IDOK)
		{
			CString strGroupName = dlg.GetName();
			ParkingSpotGroup* pSpotGroup = (ParkingSpotGroup*)(pNodeData->m_dwData);
			if (!strGroupName.IsEmpty() && !pSpotGroup->ExistSameVehicleType(strGroupName))//non empty
			{
				ParkingSpotGroup* pSpotGroup = (ParkingSpotGroup*)(pNodeData->m_dwData);
				ParkingSpotVehicleType* pSpotVehicle = new ParkingSpotVehicleType();
				pSpotVehicle->SetVehicleType(strGroupName);
				pSpotGroup->m_parkingSpotVehilceList.AddNewItem(pSpotVehicle);
				InsertSpotVehicleTypeItem(hItem,pSpotVehicle);
				GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
			}
		}
	}
}
void CDlgParkingSpotSpec::OnAddParkingSpot()
{
	HTREEITEM hItem = m_wndTreeCtrl.GetSelectedItem();
	if (hItem == NULL)
		return;
	
	TreeNodeData* pNodeData = (TreeNodeData*)m_wndTreeCtrl.GetItemData(hItem);
	if (pNodeData == NULL)
		return;
	
	switch (pNodeData->m_emType)
	{
	case TreeNodeData::SpotGroup_Node:
		{
			ParkingSpotGroup* pSpotGroup = (ParkingSpotGroup*)(pNodeData->m_dwData);
			TimeRange timeRange;
			CDlgTimeRange dlg(timeRange.GetStartTime(),timeRange.GetEndTime());
			if (dlg.DoModal() == IDOK)
			{
				TimeRange timeRange;
				timeRange.SetStartTime(dlg.GetStartTime());
				timeRange.SetEndTime(dlg.GetEndTime());
				if (pSpotGroup->CheckTimeRange(timeRange))
				{
					MessageBox(_T("Time range conflict with exist item.!!"),_T("Warning"),MB_OK);
					return;
				}

				ParkingSpotTimeRange* pSpotTimeRange = new ParkingSpotTimeRange();
				pSpotGroup->m_parkingSpotTimeRangeList.AddNewItem(pSpotTimeRange);
				InsertSpotTimeRangeItem(hItem,pSpotTimeRange);
				m_wndTreeCtrl.Expand(hItem,TVE_EXPAND);
				GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
			}
			
		}
		break;
	case TreeNodeData::TimeRange_Node:
		{
			ParkingSpotTimeRange* pSpotTimeRange = (ParkingSpotTimeRange*)(pNodeData->m_dwData);
			LandsideFacilityLayoutObjectList* pLayoutList = GetLandsideLayoutObjectList();
			if (pLayoutList)
			{
				CDlgSelectLandsideObject dlg(GetLandsideLayoutObjectList(),this);
				dlg.AddObjType(ALT_LPARKINGLOT);
				if(dlg.DoModal() == IDOK)
				{
					if (pLayoutList->getObjectByName(dlg.getSelectObject()))
					{
						CString strObject = dlg.getSelectObject().GetIDString();
						if (!pSpotTimeRange->ExistSameName(strObject))
						{
							ParkingSpotProperty* pSpotProperty = new ParkingSpotProperty();
							pSpotProperty->SetName(strObject);
							pSpotTimeRange->m_parkingSpotPropertyList.AddNewItem(pSpotProperty);

							InsertSpotPropertyItem(hItem,pSpotProperty);
							m_wndTreeCtrl.Expand(hItem,TVE_EXPAND);
							GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
						}
						else
						{
							MessageBox(_T("Exist the same parking lot!!."),_T("Warning"),MB_OK | MB_ICONINFORMATION);
						}
					}
					else
					{
						MessageBox(_T("Can't select group of parking lot!!."),_T("Warning"),MB_OK | MB_ICONINFORMATION);
					}
				}	
			}
			
		}
		break;
	case TreeNodeData::ParkingLot_Node:
		{
			ParkingSpotProperty* pSpotProperty = (ParkingSpotProperty*)(pNodeData->m_dwData);
			ParkingSpotLevel* pSpotLevel = new ParkingSpotLevel();

			LandsideFacilityLayoutObjectList* pLayoutList = GetLandsideLayoutObjectList();
			ALTObjectID parkingLot;
			parkingLot.FromString(pSpotProperty->GetName());
			LandsideFacilityLayoutObject* pObject = pLayoutList->getObjectByName(parkingLot);
			if (pObject)
			{
				if (pObject->GetType() == ALT_LPARKINGLOT)
				{
					LandsideParkingLot* pParkingLot = (LandsideParkingLot*)pObject;
					if (pParkingLot->GetLevelCount() > 0)
					{
						LandsideParkingLotLevel* pLevel = pParkingLot->GetLevel(0);
						pSpotLevel->SetLevel(pLevel->m_nLevelId);
					}
				}
			}
			pSpotProperty->m_parkingSpotLevelList.AddNewItem(pSpotLevel);

			InsertSpotLevelItem(hItem,parkingLot,pSpotLevel);
			m_wndTreeCtrl.Expand(hItem,TVE_EXPAND);
			GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
		}
		break;
	case TreeNodeData::ParkingLotLevel_Node:
		{
			ParkingSpotLevel* pSpotLevel = (ParkingSpotLevel*)(pNodeData->m_dwData);
			ParkingSpotRow* pSpotRow = new ParkingSpotRow();
			pSpotLevel->m_parkingSpotRowList.AddNewItem(pSpotRow);

			InsertSpotRowItem(hItem,pSpotRow);
			m_wndTreeCtrl.Expand(hItem,TVE_EXPAND);
			GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
		}
		break;
	default:
		break;
	}
}

void CDlgParkingSpotSpec::OnDeleteParkingSpot()
{
	HTREEITEM hItem = m_wndTreeCtrl.GetSelectedItem();
	if (hItem == NULL)
		return;

	HTREEITEM hParentItem = m_wndTreeCtrl.GetParentItem(hItem);
	if (hParentItem == NULL)
		return;
	
	TreeNodeData* pNodeData = (TreeNodeData*)m_wndTreeCtrl.GetItemData(hItem);
	if (pNodeData == NULL)
		return;

	TreeNodeData* pSuperData = (TreeNodeData*)m_wndTreeCtrl.GetItemData(hParentItem);
	if (pSuperData == NULL)
		return;
	
	switch (pNodeData->m_emType)
	{
	case TreeNodeData::TimeRange_Node:
		{
			ParkingSpotGroup* pSpotGroup = (ParkingSpotGroup*)(pSuperData->m_dwData);
			ParkingSpotTimeRange* pSpotTimeRange = (ParkingSpotTimeRange*)(pNodeData->m_dwData);
			pSpotGroup->m_parkingSpotTimeRangeList.DeleteItem(pSpotTimeRange);

			m_wndTreeCtrl.DeleteItem(hItem);
			DeleteTreeNodeData(pNodeData);
			GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
		}
		break;
	case TreeNodeData::VehicleType_Node:
		{
			ParkingSpotGroup* pSpotGroup = (ParkingSpotGroup*)(pSuperData->m_dwData);
			ParkingSpotVehicleType* pSpotVehicle = (ParkingSpotVehicleType*)(pNodeData->m_dwData);
			pSpotGroup->m_parkingSpotVehilceList.DeleteItem(pSpotVehicle);

			m_wndTreeCtrl.DeleteItem(hItem);
			DeleteTreeNodeData(pNodeData);
			GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
		}
		break;
	case TreeNodeData::ParkingLot_Node:
		{
			ParkingSpotTimeRange* pSpotTimeRange = (ParkingSpotTimeRange*)(pSuperData->m_dwData);
			ParkingSpotProperty* pSpotProperty = (ParkingSpotProperty*)(pNodeData->m_dwData);
			pSpotTimeRange->m_parkingSpotPropertyList.DeleteItem(pSpotProperty);

			m_wndTreeCtrl.DeleteItem(hItem);
			DeleteTreeNodeData(pNodeData);
			GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
		}
		break;
	case TreeNodeData::ParkingLotLevel_Node:
		{
			ParkingSpotProperty* pSpotProperty = (ParkingSpotProperty*)(pSuperData->m_dwData);
			ParkingSpotLevel* pSpotLevel = (ParkingSpotLevel*)(pNodeData->m_dwData);
			pSpotProperty->m_parkingSpotLevelList.DeleteItem(pSpotLevel);

			m_wndTreeCtrl.DeleteItem(hItem);
			DeleteTreeNodeData(pNodeData);
			GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
		}
		break;
	case TreeNodeData::ParkingLotRow_Node:
		{
			ParkingSpotLevel* pSpotLevel = (ParkingSpotLevel*)(pSuperData->m_dwData);
			ParkingSpotRow* pSpotRow = (ParkingSpotRow*)(pNodeData->m_dwData);
			pSpotLevel->m_parkingSpotRowList.DeleteItem(pSpotRow);

			m_wndTreeCtrl.DeleteItem(hItem);
			DeleteTreeNodeData(pNodeData);
			GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
		}
		break;
	default:
		break;
	}
}

LRESULT CDlgParkingSpotSpec::DefWindowProc( UINT message, WPARAM wParam, LPARAM lParam )
{
	switch (message)
	{
     case UM_CEW_LABLE_END:
		 {
			 HTREEITEM hItem = (HTREEITEM)wParam;
			 CString strValue = *((CString*)lParam);
			 if (hItem)
			 {
				TreeNodeData* pNodeData = (TreeNodeData*)m_wndTreeCtrl.GetItemData(hItem);
				if (pNodeData)
				{
					if (pNodeData->m_emType == TreeNodeData::SpotGroup_Node)
					{
						ParkingSpotGroup* pSpotGroup = (ParkingSpotGroup*)(pNodeData->m_dwData);
						if (!strValue.IsEmpty() && strValue.CompareNoCase( pSpotGroup->GetGroupName()))
						{
							if (m_landsideParkingSpotCon.ExistSameName(strValue))
							{
								MessageBox(_T("Exist the same spot group name!!."),_T("Warning"),MB_OK);
							}
							else
							{
								pSpotGroup->SetGroupName(strValue);
								CString strGroupName;
								strGroupName.Format(_T("Spot group name: %s"),pSpotGroup->GetGroupName());
								m_wndTreeCtrl.SetItemText(hItem,strGroupName);
								GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
							}
						}
					}
				}
			 }
		 }
		 break;
	 case UM_CEW_EDITSPIN_BEGIN:
		 {
			 HTREEITEM hItem = (HTREEITEM)wParam;
			 COOLTREE_NODE_INFO* cni = m_wndTreeCtrl.GetItemNodeInfo(hItem);
			 TreeNodeData* pNodeData = (TreeNodeData*)m_wndTreeCtrl.GetItemData(hItem);
			 if (pNodeData)
			 {
				 switch (pNodeData->m_emType)
				 {
					case TreeNodeData::ParkingLotLevel_Node:
						cni->fMinValue = 0.0;
						break;
					case TreeNodeData::ParkingLotRow_Node:
					case TreeNodeData::StartPos_Node:
					case TreeNodeData::EndPos_Node:
						cni->fMinValue = 1.0;
						break;
					default:
						break;
				 }
			 }
		 }
		 break;
	case  UM_CEW_EDITSPIN_END:
		{
			HTREEITEM hItem = (HTREEITEM)wParam;
			CString strValue = *((CString*)lParam);
			TreeNodeData* pNodeData = (TreeNodeData*)m_wndTreeCtrl.GetItemData(hItem);
			COOLTREE_NODE_INFO* cni = m_wndTreeCtrl.GetItemNodeInfo(hItem);
			int nValue = atoi(strValue);
			if (pNodeData)
			{
				switch (pNodeData->m_emType)
				{
				case TreeNodeData::ParkingLotLevel_Node:
					{
						ParkingSpotLevel* pSpotLevel = (ParkingSpotLevel*)(pNodeData->m_dwData);
						pSpotLevel->SetLevel(nValue);
						CString strLevel;
						strLevel.Format(_T("Level: %d"),pSpotLevel->GetLevel());
						cni->fMinValue = static_cast<float>(nValue);
						m_wndTreeCtrl.SetItemText(hItem,strLevel);
						GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
					}
					break;
				case TreeNodeData::ParkingLotRow_Node:
					{
						ParkingSpotRow* pSpotRow = (ParkingSpotRow*)(pNodeData->m_dwData);
						nValue = nValue ? nValue : 1;
						pSpotRow->SetRow(nValue);
						CString strRow;
						strRow.Format(_T("Row: %d"),pSpotRow->GetRow());
						cni->fMinValue = static_cast<float>(nValue);
						m_wndTreeCtrl.SetItemText(hItem,strRow);
						GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
					}
					break;
				case TreeNodeData::StartPos_Node:
					{
						ParkingSpotRow* pSpotRow = (ParkingSpotRow*)(pNodeData->m_dwData);
						nValue = nValue ? nValue : 1;
						pSpotRow->SetStart(nValue);
						CString strStart;
						strStart.Format(_T("Starting spot: %d"),pSpotRow->GetStart());
						cni->fMinValue = static_cast<float>(nValue);
						m_wndTreeCtrl.SetItemText(hItem,strStart);
						GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
					}
					break;
				case TreeNodeData::EndPos_Node:
					{
						ParkingSpotRow* pSpotRow = (ParkingSpotRow*)(pNodeData->m_dwData);
						nValue = nValue ? nValue : 1;
						pSpotRow->SetEnd(nValue);
						CString strEnd;
						strEnd.Format(_T("Ending spot: %d"),pSpotRow->GetEnd());
						cni->fMinValue = static_cast<float>(nValue);
						m_wndTreeCtrl.SetItemText(hItem,strEnd);
						GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
					}
					break;
				default:
					break;
				}
			}
		}
		break;
	case UM_CEW_SHOW_DIALOGBOX_BEGIN:
		{
			HTREEITEM hItem=(HTREEITEM)wParam;
			COOLTREE_NODE_INFO* pCNI = (COOLTREE_NODE_INFO*)m_wndTreeCtrl.GetItemNodeInfo(hItem);
			TreeNodeData* pNodeData = (TreeNodeData*)m_wndTreeCtrl.GetItemData(hItem);
			if (pNodeData)
			{
				if (pNodeData->m_emType == TreeNodeData::TimeRange_Node)
				{
					ParkingSpotTimeRange* pSpotTimeRange = (ParkingSpotTimeRange*)(pNodeData->m_dwData);
					if (pSpotTimeRange)
					{
						CDlgTimeRange* pDlgTimeRange = new CDlgTimeRange(pSpotTimeRange->GetTimeRange().GetStartTime(),pSpotTimeRange->GetTimeRange().GetEndTime());
						pCNI->pEditWnd = pDlgTimeRange;
					}
					
				}
				else if (pNodeData->m_emType == TreeNodeData::VehicleType_Node)
				{
					ParkingSpotVehicleType* pSpotVehicleType = (ParkingSpotVehicleType*)(pNodeData->m_dwData);
					if (pSpotVehicleType)
					{
						CDlgSelectLandsideVehicleType* pDlgVehicleType = new CDlgSelectLandsideVehicleType();
						pCNI->pEditWnd = pDlgVehicleType;
					}
				}
				else if (pNodeData->m_emType == TreeNodeData::ParkingLot_Node)
				{
					ParkingSpotProperty* pSpotProperty= (ParkingSpotProperty*)(pNodeData->m_dwData);
					if (pSpotProperty)
					{
						CDlgSelectLandsideObject* pDlgSelectLandsideObject = new CDlgSelectLandsideObject(GetLandsideLayoutObjectList(),this);
						pDlgSelectLandsideObject->AddObjType(ALT_LPARKINGLOT);
						pCNI->pEditWnd = pDlgSelectLandsideObject;
					}
				}
			}
		
		}
		break;
	case UM_CEW_SHOW_DIALOGBOX_END:
		{
			HTREEITEM hItem=(HTREEITEM)wParam;
			COOLTREE_NODE_INFO* pCNI = (COOLTREE_NODE_INFO*)lParam;
			TreeNodeData* pNodeData = (TreeNodeData*)m_wndTreeCtrl.GetItemData(hItem);
			if (pNodeData)
			{
				if (pNodeData->m_emType == TreeNodeData::TimeRange_Node)
				{
					CDlgTimeRange* pDlg = (CDlgTimeRange*)pCNI->pEditWnd;
					ParkingSpotTimeRange* pSpotTimeRange = (ParkingSpotTimeRange*)(pNodeData->m_dwData);
					if (pSpotTimeRange)
					{
						HTREEITEM hParentItem = m_wndTreeCtrl.GetParentItem(hItem);
						if (hParentItem)
						{
							TreeNodeData* pSuperNode = (TreeNodeData*)m_wndTreeCtrl.GetItemData(hParentItem);
							if (pSuperNode && pSuperNode->m_emType == TreeNodeData::SpotGroup_Node)
							{
								ParkingSpotGroup* pSpotGroup = (ParkingSpotGroup*)(pSuperNode->m_dwData);
								TimeRange timeRange;
								timeRange.SetStartTime(pDlg->GetStartTime());
								timeRange.SetEndTime(pDlg->GetEndTime());

								if (pSpotGroup->CheckTimeRange(timeRange))
								{
									pSpotTimeRange->SetTimeRange(pDlg->GetStartTime(),pDlg->GetEndTime());
									TimeRange timeRange = pSpotTimeRange->GetTimeRange();
									CString strTimeRange;
									strTimeRange.Format(_T("Time Window: %s"),timeRange.ToString());
									m_wndTreeCtrl.SetItemText(hItem,strTimeRange);
									GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
								}
								else
								{
									MessageBox(_T("Time range conflict with exist item.!!"),_T("Warning"),MB_OK);
								}
							}
						}
					}
				}
				else if (pNodeData->m_emType == TreeNodeData::VehicleType_Node)
				{
					CDlgSelectLandsideVehicleType* pDlg = (CDlgSelectLandsideVehicleType*)pCNI->pEditWnd;
					ParkingSpotVehicleType* pSpotVehicleType = (ParkingSpotVehicleType*)(pNodeData->m_dwData);
					if (pSpotVehicleType && pSpotVehicleType->GetVehicleType().Compare(pDlg->GetName()) != 0)
					{
						HTREEITEM hParentItem = m_wndTreeCtrl.GetParentItem(hItem);
						if (hParentItem)
						{
							TreeNodeData* pSuperNode = (TreeNodeData*)m_wndTreeCtrl.GetItemData(hParentItem);
							if (pSuperNode && pSuperNode->m_emType == TreeNodeData::SpotGroup_Node)
							{
								ParkingSpotGroup* pSpotGroup = (ParkingSpotGroup*)(pSuperNode->m_dwData);
								if (pSpotGroup && !pSpotGroup->ExistSameVehicleType(pDlg->GetName()))
								{
									pSpotVehicleType->SetVehicleType(pDlg->GetName());
									CString strVehicleType;
									strVehicleType.Format(_T("Vehicle Type: %s"),pDlg->GetName());
									m_wndTreeCtrl.SetItemText(hItem,strVehicleType);
									GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
								}
								else
								{
									MessageBox(_T("Exist the same vehicle type!!."),_T("Warning"),MB_OK);
								}
							}
						}
					}
				}
				else if (pNodeData->m_emType == TreeNodeData::ParkingLot_Node)
				{
					CDlgSelectLandsideObject* pDlgSelectLandsideObject = (CDlgSelectLandsideObject*)pCNI->pEditWnd;
					ParkingSpotProperty* pSpotProperty= (ParkingSpotProperty*)(pNodeData->m_dwData);
					if (pSpotProperty && pSpotProperty->GetName().Compare(pDlgSelectLandsideObject->getSelectObject().GetIDString())!= 0)
					{
						HTREEITEM hParentItem = m_wndTreeCtrl.GetParentItem(hItem);
						if (hParentItem)
						{
							TreeNodeData* pSuperNode = (TreeNodeData*)m_wndTreeCtrl.GetItemData(hParentItem);
							if (pSuperNode && pSuperNode->m_emType == TreeNodeData::TimeRange_Node)
							{
								ParkingSpotTimeRange* pSpotTimeRange = (ParkingSpotTimeRange*)(pSuperNode->m_dwData);
								if (pSpotTimeRange && !pSpotTimeRange->ExistSameName(pDlgSelectLandsideObject->getSelectObject().GetIDString()))
								{
									LandsideFacilityLayoutObjectList* pLayoutList = GetLandsideLayoutObjectList();
									if (pLayoutList)
									{
										if (pLayoutList->getObjectByName(pDlgSelectLandsideObject->getSelectObject()) == NULL)
										{
											MessageBox(_T("Can't select group of parking lot!!."),_T("Warning"),MB_OK|MB_ICONINFORMATION);
										}
										else
										{
											pSpotProperty->SetName(pDlgSelectLandsideObject->getSelectObject().GetIDString());
											CString strObject;
											strObject.Format(_T("Parking Lot name: %s"),pSpotProperty->GetName());
											m_wndTreeCtrl.SetItemText(hItem,strObject);
											GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
										}
									}
									
								}
								else
								{
									MessageBox(_T("Exist the same parking lot!!."),_T("Warning"),MB_OK|MB_ICONINFORMATION);
								}
							}
						}
					}
				}
			}
			if (pCNI->pEditWnd)
			{
				delete pCNI->pEditWnd;
				pCNI->pEditWnd = NULL;
			}
		}
		break;
	case UM_CEW_COMBOBOX_BEGIN:
		{
			HTREEITEM hItem=(HTREEITEM)wParam;
			HTREEITEM hParentItem = m_wndTreeCtrl.GetParentItem(hItem);

			TreeNodeData* pNodeData = (TreeNodeData*)m_wndTreeCtrl.GetItemData(hParentItem);
			if (pNodeData)
			{
				ParkingSpotProperty* pParkLotProp = (ParkingSpotProperty*)(pNodeData->m_dwData);
				ALTObjectID altID;
				altID.FromString(pParkLotProp->GetName());
				LandsideFacilityLayoutObjectList* pLayoutList = GetLandsideLayoutObjectList();
				LandsideFacilityLayoutObject* pObject = pLayoutList->getObjectByName(altID);
				if (pObject && pObject->GetType() == ALT_LPARKINGLOT)
				{
					LandsideParkingLot* pParkLot = (LandsideParkingLot*)pObject;
					CRect rectWnd;
					CWnd* pWnd=m_wndTreeCtrl.GetEditWnd(hItem);
					m_wndTreeCtrl.GetItemRect(hItem,rectWnd,TRUE);
					pWnd->SetWindowPos(NULL,rectWnd.right,rectWnd.top,0,0,SWP_NOSIZE);
					CComboBox* pCB=(CComboBox*)pWnd;
					pCB->ResetContent();

					for (int i = 0; i < pParkLot->GetLevelCount(); i++)
					{
						LandsideParkingLotLevel* pLevel = pParkLot->GetLevel(i);
						pCB->AddString(pLevel->m_name);
					}
				}
			}
		}
		break;
	case UM_CEW_COMBOBOX_SELCHANGE:
		{
			HTREEITEM hItem=(HTREEITEM)wParam;
			int nSel = m_wndTreeCtrl.GetCmbBoxCurSel(hItem);
			TreeNodeData* pNodeData = (TreeNodeData*)m_wndTreeCtrl.GetItemData(hItem);
			if (pNodeData)
			{
				ParkingSpotLevel* pSpotLevel = (ParkingSpotLevel*)(pNodeData->m_dwData);

				HTREEITEM hParentItem = m_wndTreeCtrl.GetParentItem(hItem);
				TreeNodeData* pParentData = (TreeNodeData*)m_wndTreeCtrl.GetItemData(hParentItem);
				if (pParentData)
				{
					ParkingSpotProperty* pParkLotProp = (ParkingSpotProperty*)(pParentData->m_dwData);
					ALTObjectID altID;
					altID.FromString(pParkLotProp->GetName());
					LandsideFacilityLayoutObjectList* pLayoutList = GetLandsideLayoutObjectList();
					LandsideFacilityLayoutObject* pObject = pLayoutList->getObjectByName(altID);
					if (pObject && pObject->GetType() == ALT_LPARKINGLOT)
					{
						LandsideParkingLot* pParkLot = (LandsideParkingLot*)pObject;
						LandsideParkingLotLevel* pLevel = pParkLot->GetLevel(nSel);
						if (pLevel)
						{
							pSpotLevel->SetLevel(pLevel->m_nLevelId);
							CString strLevel;
							strLevel.Format(_T("Level: %s"),pLevel->m_name);
							m_wndTreeCtrl.SetItemText(hItem,strLevel);
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
	return CXTResizeDialog::DefWindowProc(message,wParam,lParam);
}

void CDlgParkingSpotSpec::OnInitToolbar()
{
	CRect rect;
	m_wndTreeCtrl.GetWindowRect( &rect );
	ScreenToClient( &rect );
	rect.top -= 26;
	rect.bottom = rect.top + 22;
	rect.left += 2;
	m_wndToolbar.MoveWindow(&rect);

	CToolBarCtrl& spotBarCtrl = m_wndToolbar.GetToolBarCtrl();
	spotBarCtrl.SetCmdID(0,ID_PARKINGLOT_NEW);
	spotBarCtrl.SetCmdID(1,ID_PARKINGLOT_DEL);

	m_wndToolbar.GetToolBarCtrl().EnableButton( ID_PARKINGLOT_NEW, TRUE);
	m_wndToolbar.GetToolBarCtrl().EnableButton( ID_PARKINGLOT_DEL, FALSE);
}

void CDlgParkingSpotSpec::UpdateToolBarState()
{
	HTREEITEM hItem = m_wndTreeCtrl.GetSelectedItem();
	if (hItem == NULL)
	{
		m_wndToolbar.GetToolBarCtrl().EnableButton(ID_PARKINGLOT_NEW,TRUE);
		m_wndToolbar.GetToolBarCtrl().EnableButton(ID_PARKINGLOT_DEL,FALSE);
	}
	else
	{
		TreeNodeData* pNodeData = (TreeNodeData*)m_wndTreeCtrl.GetItemData(hItem);
		if (pNodeData == NULL)
		{
			m_wndToolbar.GetToolBarCtrl().EnableButton(ID_PARKINGLOT_NEW,TRUE);
			m_wndToolbar.GetToolBarCtrl().EnableButton(ID_PARKINGLOT_DEL,FALSE);
		}
		else
		{
			if (pNodeData->m_emType == TreeNodeData::SpotGroup_Node)
			{
				m_wndToolbar.GetToolBarCtrl().EnableButton(ID_PARKINGLOT_NEW,TRUE);
				m_wndToolbar.GetToolBarCtrl().EnableButton(ID_PARKINGLOT_DEL,TRUE);
			}
			else
			{
				m_wndToolbar.GetToolBarCtrl().EnableButton(ID_PARKINGLOT_NEW,TRUE);
				m_wndToolbar.GetToolBarCtrl().EnableButton(ID_PARKINGLOT_DEL,FALSE);
			}
		}
	}
}

void CDlgParkingSpotSpec::LoadTreeContent()
{
	m_wndTreeCtrl.DeleteAllItems();
	m_wndTreeCtrl.SetImageList(m_wndTreeCtrl.GetImageList(TVSIL_NORMAL),TVSIL_NORMAL);

	for (int i = 0; i < m_landsideParkingSpotCon.GetItemCount(); i++)
	{
		ParkingSpotGroup* pSpotGroup = m_landsideParkingSpotCon.GetItem(i);
		InsertSpotGroupItem(TVI_ROOT,pSpotGroup);
	}

	HTREEITEM hItem = m_wndTreeCtrl.GetRootItem();
	if (hItem)
	{
		m_wndTreeCtrl.SelectSetFirstVisible(hItem);
	}
}

void CDlgParkingSpotSpec::InsertSpotGroupItem( HTREEITEM hItem, ParkingSpotGroup* pParkingSpotGroup )
{
	COOLTREE_NODE_INFO cni;
	CCoolTree::InitNodeInfo(this,cni);

	TreeNodeData* pNodeData = new TreeNodeData();
	pNodeData->m_emType = TreeNodeData::SpotGroup_Node;
	pNodeData->m_dwData = (DWORD)pParkingSpotGroup;
	cni.nt = NT_NORMAL;
	cni.net = NET_LABLE;
	CString strGroupName;
	strGroupName.Format(_T("Spot group name: %s"),pParkingSpotGroup->GetGroupName());
	HTREEITEM hSpotGroupItem = m_wndTreeCtrl.InsertItem(strGroupName,cni,FALSE,FALSE,hItem);
	m_wndTreeCtrl.SetItemData(hSpotGroupItem,(DWORD)pNodeData);
	m_vNodeData.push_back(pNodeData);

	for (int i = 0; i < pParkingSpotGroup->m_parkingSpotTimeRangeList.GetItemCount(); i++)
	{
		ParkingSpotTimeRange* pSpotTimeRange = pParkingSpotGroup->m_parkingSpotTimeRangeList.GetItem(i);
		InsertSpotTimeRangeItem(hSpotGroupItem,pSpotTimeRange);
	}

	for (i = 0; i < pParkingSpotGroup->m_parkingSpotVehilceList.GetItemCount(); i++)
	{
		ParkingSpotVehicleType* pSpotVehicleType = pParkingSpotGroup->m_parkingSpotVehilceList.GetItem(i);
		InsertSpotVehicleTypeItem(hSpotGroupItem,pSpotVehicleType);
	}
	m_wndTreeCtrl.Expand(hSpotGroupItem,TVE_EXPAND);
}

void CDlgParkingSpotSpec::InsertSpotTimeRangeItem( HTREEITEM hItem, ParkingSpotTimeRange* pParkingSpotTimeRange )
{
	COOLTREE_NODE_INFO cni;
	CCoolTree::InitNodeInfo(this,cni);

	TreeNodeData* pNodeData = new TreeNodeData();
	pNodeData->m_emType = TreeNodeData::TimeRange_Node;
	pNodeData->m_dwData = (DWORD)pParkingSpotTimeRange;
	cni.nt = NT_NORMAL;
	cni.net = NET_SHOW_DIALOGBOX;
	CString strTimeRange;
	strTimeRange.Format(_T("Time window: %s"),pParkingSpotTimeRange->GetTimeRange().ToString());
	HTREEITEM hSpotTimeRangeItem = m_wndTreeCtrl.InsertItem(strTimeRange,cni,FALSE,FALSE,hItem);
	m_wndTreeCtrl.SetItemData(hSpotTimeRangeItem,(DWORD)pNodeData);
	m_vNodeData.push_back(pNodeData);

	for (int i = 0; i < pParkingSpotTimeRange->m_parkingSpotPropertyList.GetItemCount(); i++)
	{
		ParkingSpotProperty* pSpotProperty = pParkingSpotTimeRange->m_parkingSpotPropertyList.GetItem(i);
		InsertSpotPropertyItem(hSpotTimeRangeItem,pSpotProperty);
	}
	m_wndTreeCtrl.Expand(hSpotTimeRangeItem,TVE_EXPAND);
}


void CDlgParkingSpotSpec::InsertSpotVehicleTypeItem( HTREEITEM hItem, ParkingSpotVehicleType* pParkingSpotVehicleType )
{
	COOLTREE_NODE_INFO cni;
	CCoolTree::InitNodeInfo(this,cni);

	TreeNodeData* pNodeData = new TreeNodeData();
	pNodeData->m_emType = TreeNodeData::VehicleType_Node;
	pNodeData->m_dwData = (DWORD)pParkingSpotVehicleType;
	cni.nt = NT_NORMAL;
	cni.net = NET_SHOW_DIALOGBOX;
	CString strVehicleType;
	strVehicleType.Format(_T("Vehicle Type: %s"),pParkingSpotVehicleType->GetVehicleType());
	HTREEITEM hVehicleItem = m_wndTreeCtrl.InsertItem(strVehicleType,cni,FALSE,FALSE,hItem);
	m_wndTreeCtrl.SetItemData(hVehicleItem,(DWORD)pNodeData);
	m_vNodeData.push_back(pNodeData);

}

void CDlgParkingSpotSpec::InsertSpotPropertyItem( HTREEITEM hItem, ParkingSpotProperty* pParkingSpotProperty )
{
	COOLTREE_NODE_INFO cni;
	CCoolTree::InitNodeInfo(this,cni);

	TreeNodeData* pNodeData = new TreeNodeData();
	pNodeData->m_emType = TreeNodeData::ParkingLot_Node;
	pNodeData->m_dwData = (DWORD)pParkingSpotProperty;
	cni.nt = NT_NORMAL;
	cni.net = NET_SHOW_DIALOGBOX;
	CString strParkingLot;
	strParkingLot.Format(_T("Parking Lot name: %s"),pParkingSpotProperty->GetName());
	HTREEITEM hParkingLotItem = m_wndTreeCtrl.InsertItem(strParkingLot,cni,FALSE,FALSE,hItem);
	m_wndTreeCtrl.SetItemData(hParkingLotItem,(DWORD)pNodeData);
	m_vNodeData.push_back(pNodeData);

	ALTObjectID parkLotID;
	parkLotID.FromString(pParkingSpotProperty->GetName());
	for (int i = 0; i < pParkingSpotProperty->m_parkingSpotLevelList.GetItemCount(); i++)
	{
		ParkingSpotLevel* pSpotLevel = pParkingSpotProperty->m_parkingSpotLevelList.GetItem(i);
		InsertSpotLevelItem(hParkingLotItem,parkLotID,pSpotLevel);
	}
	m_wndTreeCtrl.Expand(hParkingLotItem,TVE_EXPAND);
}

void CDlgParkingSpotSpec::InsertSpotLevelItem( HTREEITEM hItem, const ALTObjectID& altID,ParkingSpotLevel* pParkingSpotLevel )
{
	COOLTREE_NODE_INFO cni;
	CCoolTree::InitNodeInfo(this,cni);

	TreeNodeData* pNodeData = new TreeNodeData();
	pNodeData->m_emType = TreeNodeData::ParkingLotLevel_Node;
	pNodeData->m_dwData = (DWORD)pParkingSpotLevel;
	cni.nt = NT_NORMAL;
	cni.net = NET_COMBOBOX;
	cni.fMinValue = static_cast<float>(pParkingSpotLevel->GetLevel());
	LandsideFacilityLayoutObjectList* pLayoutList = GetLandsideLayoutObjectList();
	LandsideFacilityLayoutObject* pObject = pLayoutList->getObjectByName(altID);
	CString strLevel;
	if (pObject && pObject->GetType() == ALT_LPARKINGLOT)
	{
		LandsideParkingLot* pParkLot = (LandsideParkingLot*)pObject;
		LandsideParkingLotLevel* pLevel = pParkLot->GetLevelByID(pParkingSpotLevel->GetLevel());
		if (pLevel)
		{
			strLevel.Format(_T("Level: %s"),pLevel->m_name);
		}
	}
	
	HTREEITEM hLevelItem = m_wndTreeCtrl.InsertItem(strLevel,cni,FALSE,FALSE,hItem);
	m_wndTreeCtrl.SetItemData(hLevelItem,(DWORD)pNodeData);
	m_vNodeData.push_back(pNodeData);

	for (int i = 0; i < pParkingSpotLevel->m_parkingSpotRowList.GetItemCount(); i++)
	{
		ParkingSpotRow* pSpotRow = pParkingSpotLevel->m_parkingSpotRowList.GetItem(i);
		InsertSpotRowItem(hLevelItem,pSpotRow);
	}
	m_wndTreeCtrl.Expand(hLevelItem,TVE_EXPAND);
}

void CDlgParkingSpotSpec::InsertSpotRowItem( HTREEITEM hItem, ParkingSpotRow* pParkingSpotRow )
{
	COOLTREE_NODE_INFO cni;
	CCoolTree::InitNodeInfo(this,cni);
	
	TreeNodeData* pNodeData = new TreeNodeData();
	pNodeData->m_emType = TreeNodeData::ParkingLotRow_Node;
	pNodeData->m_dwData = (DWORD)pParkingSpotRow;
	cni.nt = NT_NORMAL;
	cni.net = NET_EDIT_INT;
	cni.fMinValue = static_cast<float>(pParkingSpotRow->GetRow());
	CString strRow;
	strRow.Format(_T("Row: %d"),pParkingSpotRow->GetRow());
	HTREEITEM hRowItem = m_wndTreeCtrl.InsertItem(strRow,cni,FALSE,FALSE,hItem);
	m_wndTreeCtrl.SetItemData(hRowItem,(DWORD)pNodeData);
	m_vNodeData.push_back(pNodeData);

	pNodeData = new TreeNodeData();
	pNodeData->m_emType = TreeNodeData::StartPos_Node;
	pNodeData->m_dwData = (DWORD)pParkingSpotRow;
	cni.nt = NT_NORMAL;
	cni.net = NET_EDIT_INT;
	cni.fMinValue = static_cast<float>(pParkingSpotRow->GetStart());
	CString strStart;
	strStart.Format(_T("Starting spot: %d"),pParkingSpotRow->GetStart());
	HTREEITEM hStartItem = m_wndTreeCtrl.InsertItem(strStart,cni,FALSE,FALSE,hRowItem);
	m_wndTreeCtrl.SetItemData(hStartItem,(DWORD)pNodeData);
	m_vNodeData.push_back(pNodeData);

	pNodeData = new TreeNodeData();
	pNodeData->m_emType = TreeNodeData::EndPos_Node;
	pNodeData->m_dwData = (DWORD)pParkingSpotRow;
	cni.nt = NT_NORMAL;
	cni.net = NET_EDIT_INT;
	cni.fMinValue = static_cast<float>(pParkingSpotRow->GetEnd());
	CString strEnd;
	strEnd.Format(_T("Ending spot: %d"),pParkingSpotRow->GetEnd());
	HTREEITEM hEndItem = m_wndTreeCtrl.InsertItem(strEnd,cni,FALSE,FALSE,hRowItem);
	m_wndTreeCtrl.SetItemData(hEndItem,(DWORD)pNodeData);
	m_vNodeData.push_back(pNodeData);

	m_wndTreeCtrl.Expand(hRowItem,TVE_EXPAND);
}

void CDlgParkingSpotSpec::DeleteTreeNodeData( TreeNodeData* pNodeData )
{
	std::vector<TreeNodeData*>::iterator iter = std::find(m_vNodeData.begin(),m_vNodeData.end(),pNodeData);
	if (iter != m_vNodeData.end())
	{
		m_vNodeData.erase(iter);
		delete pNodeData;
	}
}

void CDlgParkingSpotSpec::ClearTreeNodeData()
{
	for (unsigned i = 0; i < m_vNodeData.size(); i++)
	{
		delete m_vNodeData[i];
	}
	m_vNodeData.clear();
}

LandsideFacilityLayoutObjectList* CDlgParkingSpotSpec::GetLandsideLayoutObjectList()
{
	if (m_pParentWnd == NULL)
		return NULL;
	
	CTermPlanDoc* pDoc = (CTermPlanDoc*)(((CView*)m_pParentWnd)->GetDocument());
	if (pDoc == NULL)
		return NULL;
	
	InputLandside* pInputLandside = pDoc->getARCport()->GetInputLandside();
	if (pInputLandside == NULL)
		return NULL;
	
	return &pInputLandside->getObjectList();
}