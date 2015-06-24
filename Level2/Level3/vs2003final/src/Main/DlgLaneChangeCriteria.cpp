// DlgLaneChangeCriteria.cpp : implementation file
//

#include "stdafx.h"
#include "TermPlan.h"
#include "DlgLaneChangeCriteria.h"
#include "../Common/AirportDatabase.h"
#include "../Inputs/IN_TERM.H"
#include "../Common/WinMsg.h"
#include "landside/LandsideLayoutObject.h"
#include "TermPlanDoc.h"
#include "landside/InputLandside.h"
#include "DlgSelectLandsideVehicleType.h"
#include "DlgSelectLandsideObject.h"
#include "../Common/ProbDistEntry.h"
#include "DlgProbDist.h"
#include "Inputs/PROBAB.H"
#include "Common/ProbDistManager.h"

// CDlgLaneChangeCriteria dialog

IMPLEMENT_DYNAMIC(CDlgLaneChangeCriteria, CXTResizeDialog)
CDlgLaneChangeCriteria::CDlgLaneChangeCriteria(CTermPlanDoc* pDoc, InputTerminal* pInTerm,CWnd* pParent /*=NULL*/)
	: CXTResizeDialog(CDlgLaneChangeCriteria::IDD, pParent)
	,m_pDoc(pDoc)
	,m_pInTerm(pInTerm)
{
	m_canDeleteNode.push_back(TreeNodeData::STRETCH_Node);
	m_canDeleteNode.push_back(TreeNodeData::INTERSECTION_Node);
	m_canDeleteNode.push_back(TreeNodeData::APPROACHVEHICLE_Node);
	m_canDeleteNode.push_back(TreeNodeData::RANDOMVEHICLE_Node);
	m_canDeleteNode.push_back(TreeNodeData::OVERTAKING_Node);
	m_canDeleteNode.push_back(TreeNodeData::OVERTAKEN_Node);

	m_canNewNode.push_back(TreeNodeData::STRETCH_Node);
	m_canNewNode.push_back(TreeNodeData::INTERSECTION_Node);
	m_canNewNode.push_back(TreeNodeData::RANDOMVEHICLE_Node);
	m_canNewNode.push_back(TreeNodeData::OVERTAKING_Node);
}

CDlgLaneChangeCriteria::~CDlgLaneChangeCriteria()
{
	ClearTreeNode();
}

void CDlgLaneChangeCriteria::DoDataExchange(CDataExchange* pDX)
{
	CXTResizeDialog::DoDataExchange(pDX);
	DDX_Control(pDX,IDC_TREE_DATA, m_wndTreeCtrl);
	DDX_Control(pDX,IDC_LIST_DATA, m_wndListCtrl);
}


BEGIN_MESSAGE_MAP(CDlgLaneChangeCriteria, CXTResizeDialog)
	ON_WM_CREATE()
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_TOOLBAR_ADD,OnCmdNewItem)
	ON_COMMAND(ID_TOOLBAR_DEL,OnCmdDeleteItem)
	ON_BN_CLICKED(IDC_BUTTON_SAVE,OnBtnSave)
	ON_NOTIFY(TVN_SELCHANGED,IDC_TREE_DATA,OnLvnTreeItemchangedList)
	ON_LBN_SELCHANGE(IDC_LIST_DATA, OnLbnSelchangeListBox)
	ON_MESSAGE(WM_LBUTTONDBLCLK,OnLeftDoubleClick)
END_MESSAGE_MAP()

// CDlgLaneChangeCriteria message handlers
BOOL CDlgLaneChangeCriteria::OnInitDialog()
{
	CXTResizeDialog::OnInitDialog();

	m_laneChangeCriteria.ReadData(-1);
	OnInitToolbar();
	UpdateToolBarState();
	SetListBoxContent();

	SetResize(IDC_BUTTON_SAVE, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);
	SetResize(IDOK, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);
	SetResize(IDCANCEL, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);
	SetResize(IDC_STATIC_GROUP, SZ_TOP_LEFT, SZ_BOTTOM_RIGHT);
	SetResize(IDC_STATIC_CRITERIA,SZ_TOP_LEFT,SZ_TOP_LEFT);
	SetResize(m_wndToolBar.GetDlgCtrlID(), SZ_TOP_CENTER, SZ_TOP_RIGHT);
	SetResize(IDC_LIST_DATA, SZ_TOP_LEFT, SZ_BOTTOM_CENTER);
	SetResize(IDC_TREE_DATA, SZ_TOP_CENTER, SZ_BOTTOM_RIGHT);

	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(FALSE);

	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),TRUE);
	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),FALSE);
	return TRUE;
}

int CDlgLaneChangeCriteria::OnCreate( LPCREATESTRUCT lpCreateStruct )
{
	if (CXTResizeDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT,WS_CHILD|WS_VISIBLE|CBRS_ALIGN_TOP)
		|| !m_wndToolBar.LoadToolBar(IDR_ADDDELTOOLBAR))
	{
		return -1;
	}

	return 0;
}

void CDlgLaneChangeCriteria::OnOK()
{
	try
	{
		CADODatabase::BeginTransaction();
		m_laneChangeCriteria.SaveData(-1);
		CADODatabase::CommitTransaction();
	}
	catch (CADOException& e)
	{
		e.ErrorMessage();
		CADODatabase::RollBackTransation();
		MessageBox(_T("Save database error"),_T("ERROR"),MB_OK);
		return;
	}
	CXTResizeDialog::OnOK();;
}

void CDlgLaneChangeCriteria::OnInitToolbar()
{
	CRect rect;
	m_wndTreeCtrl.GetWindowRect( &rect );
	ScreenToClient( &rect );
	rect.top -= 26;
	rect.bottom = rect.top + 22;
	rect.left += 4;
	m_wndToolBar.MoveWindow(&rect);

	m_wndToolBar.GetToolBarCtrl().EnableButton( ID_TOOLBAR_ADD, FALSE);
	m_wndToolBar.GetToolBarCtrl().EnableButton( ID_TOOLBAR_DEL, FALSE);
}
	
void CDlgLaneChangeCriteria::UpdateToolBarState()
{
	if (m_wndTreeCtrl.GetSelectedItem())
	{
		m_wndToolBar.GetToolBarCtrl().EnableButton( ID_TOOLBAR_ADD, TRUE);
		m_wndToolBar.GetToolBarCtrl().EnableButton( ID_TOOLBAR_DEL, TRUE);
	}
	else
	{
		m_wndToolBar.GetToolBarCtrl().EnableButton( ID_TOOLBAR_ADD, TRUE);
		m_wndToolBar.GetToolBarCtrl().EnableButton( ID_TOOLBAR_DEL, FALSE);
	}
	
}

void CDlgLaneChangeCriteria::OnCmdNewItem()
{
	int nSel = m_wndListCtrl.GetCurSel();
	if (nSel == LB_ERR)
		return;
	
	landsideLaneChangeCriteria::NewCriteriaType emType = (landsideLaneChangeCriteria::NewCriteriaType)m_wndListCtrl.GetItemData(nSel);
	HTREEITEM hItem = m_wndTreeCtrl.GetSelectedItem();
	if (hItem == NULL)
	{
		LandsideFacilityLayoutObjectList* pLayoutList = &(m_pDoc->GetInputLandside()->getObjectList());
		CDlgSelectLandsideObject dlg(pLayoutList,this);
		dlg.AddObjType(ALT_LSTRETCH);
		if(dlg.DoModal() == IDOK)
		{
			ALTObjectID stretchName = dlg.getSelectObject();
			if (!stretchName.IsBlank() && !m_laneChangeCriteria.ExistSameStretch(emType,stretchName))//non empty
			{
				landsideLaneChangeCriteria* pCriteria = m_laneChangeCriteria.CreateObject(emType);
				pCriteria->SetStretch(stretchName);
				m_laneChangeCriteria.AddItem(pCriteria);
				HTREEITEM hVehicleItem = InsertStretchItem(pCriteria);
				m_wndTreeCtrl.SelectItem(hVehicleItem);
				m_wndTreeCtrl.SetItemState(hVehicleItem,TVIS_SELECTED,TVIS_SELECTED);
				GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
			}
			else
			{
				MessageBox(_T("Exist the Same Stretch!!!"),_T("Warning"),MB_OK);
			}
		}
		return;
	}

	TreeNodeData* pNodeData = (TreeNodeData*)m_wndTreeCtrl.GetItemData(hItem);
	if (pNodeData == NULL)
		return;

	switch (pNodeData->m_emType)
	{
	case TreeNodeData::STRETCH_Node:
		{
			if (emType == landsideLaneChangeCriteria::Approach_Intersection_Type || emType == landsideLaneChangeCriteria::Approach_Tollgates_Type)
			{
				LandsideFacilityLayoutObjectList* pLayoutList = &(m_pDoc->GetInputLandside()->getObjectList());
				CDlgSelectLandsideObject dlg(pLayoutList,this);
				dlg.AddObjType(ALT_LINTERSECTION);
				if(dlg.DoModal() == IDOK)
				{
					ALTObjectID stretchName = dlg.getSelectObject();
					LandsideApproachCriteria* pApproachLane = (LandsideApproachCriteria*)pNodeData->m_dwData;
					if (!stretchName.IsBlank() && !pApproachLane->ExistSameIntersection(stretchName))
					{
						LandsideApproachIntersection* pLane = new LandsideApproachIntersection();
						pLane->SetIntersectionNode(stretchName);
						
						pApproachLane->m_laneContainer.AddItem(pLane);
						InsertIntersectionItem(hItem,pLane);
					}
					else
					{
						MessageBox(_T("Exist the Same Intersection!!!"),_T("Warning"),MB_OK);
					}
				}
			}
			else if (emType == landsideLaneChangeCriteria::Random_Behavior_Type)
			{
				CDlgSelectLandsideVehicleType dlg;
				if(dlg.DoModal() == IDOK)
				{
					CString strVehicleType = dlg.GetName();
					LandsideRandomCriteria* pBehavior = (LandsideRandomCriteria*)pNodeData->m_dwData;
					if (!strVehicleType.IsEmpty() && !pBehavior->ExistSameVehicleType(strVehicleType))
					{
						LandsideRandomVehicle* pVehicle = new LandsideRandomVehicle();
						pVehicle->SetVehicleType(strVehicleType);
						pBehavior->m_randomContainter.AddItem(pVehicle);
						InsertRandomVehicle(hItem,pVehicle);
						GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
					}
					else
					{
						MessageBox(_T("Exist the Same Vehicle Type!!!"),_T("Warning"),MB_OK);
					}
				}
			}
			else if (emType == landsideLaneChangeCriteria::OverTaking_SlowerVehicle_Type)
			{
				CDlgSelectLandsideVehicleType dlg;
				if(dlg.DoModal() == IDOK)
				{
					CString strVehicleType = dlg.GetName();
					LandsideOvertakingCriteria* pSlowerData = (LandsideOvertakingCriteria*)(pNodeData->m_dwData);
					if (!strVehicleType.IsEmpty())
					{
						LandsideOverTakingVehicle* pOvertaking = new LandsideOverTakingVehicle();
						pOvertaking->SetOverTakingVehicle(strVehicleType);
						pSlowerData->m_dataContainer.AddItem(pOvertaking);
						InsertOvertakingSlowerVehicleItem(hItem,pOvertaking);
						GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
					}
				}
			}
		}
		break;
	case TreeNodeData::INTERSECTION_Node:
		{
			CDlgSelectLandsideVehicleType dlg;
			if(dlg.DoModal() == IDOK)
			{
				CString strVehicleType = dlg.GetName();
				LandsideApproachIntersection* pLane = (LandsideApproachIntersection*)(pNodeData->m_dwData);
				if (!strVehicleType.IsEmpty() && !pLane->ExistSameVehicleType(strVehicleType))
				{
					LandsideApproachVehicle* pVehicleData = new LandsideApproachVehicle();
					pVehicleData->SetVehicleType(strVehicleType);
					pLane->m_approachContainer.AddItem(pVehicleData);
					InsertApproachVehicleItem(hItem,pVehicleData);
					GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
				}
				else
				{
					MessageBox(_T("Exist the Same Vehicle Type!!!"),_T("Warning"),MB_OK);
				}
			}
		}
		break;
	case TreeNodeData::OVERTAKING_Node:
		{
			CDlgSelectLandsideVehicleType dlg;
			if(dlg.DoModal() == IDOK)
			{
				CString strVehicleType = dlg.GetName();
				HTREEITEM hParentItem = m_wndTreeCtrl.GetParentItem(hItem);
				TreeNodeData* pParentData = (TreeNodeData*)m_wndTreeCtrl.GetItemData(hParentItem);
				LandsideOvertakingCriteria* pSlowerVehicle = (LandsideOvertakingCriteria*)(pParentData->m_dwData);
				LandsideOverTakingVehicle* pOvertaking = (LandsideOverTakingVehicle*)pNodeData->m_dwData;
				if (!strVehicleType.IsEmpty()&& !pSlowerVehicle->ExistSameVehiclePair(pOvertaking->GetOverTakingVehicle(),strVehicleType))
				{
					LandsideOvertakenVehicle* pOvertaken = new LandsideOvertakenVehicle();
					pOvertaken->SetOverTakenVehicle(strVehicleType);
					pOvertaking->AddItem(pOvertaken);
					InsertOvertakenSlowerVehicleItem(hItem,pOvertaken);
					GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
				}
				else
				{
					MessageBox(_T("Exist the Same Overtaking Vehicle Type and Overtaken Vehicle Type!!!"),_T("Warning"),MB_OK);
				}
			}
		}
		break;
	default:
		break;
	}
}

void CDlgLaneChangeCriteria::DeleteTreeNodeData( TreeNodeData* pNodeData )
{
	std::vector<TreeNodeData*>::iterator iter = std::find(m_vNodeData.begin(),m_vNodeData.end(),pNodeData);
	if (iter != m_vNodeData.end())
	{
		m_vNodeData.erase(iter);
		delete pNodeData;
	}
}

void CDlgLaneChangeCriteria::OnCmdDeleteItem()
{
	HTREEITEM hItem = m_wndTreeCtrl.GetSelectedItem();
	if (hItem == NULL)
		return;

	TreeNodeData* pNodeData = (TreeNodeData*)m_wndTreeCtrl.GetItemData(hItem);
	if (pNodeData == NULL)
		return;

	switch (pNodeData->m_emType)
	{
	case TreeNodeData::STRETCH_Node:
		{
			landsideLaneChangeCriteria* pCriteria = (landsideLaneChangeCriteria*)(pNodeData->m_dwData);
			m_laneChangeCriteria.DeleteItem(pCriteria);
			m_wndTreeCtrl.DeleteItem(hItem);
			DeleteTreeNodeData(pNodeData);
			GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
		}
		break;
	case TreeNodeData::INTERSECTION_Node:
		{
			HTREEITEM hParentItem = m_wndTreeCtrl.GetParentItem(hItem);
			if (hParentItem == NULL)
				return;
			TreeNodeData* pParentData = (TreeNodeData*)m_wndTreeCtrl.GetItemData(hParentItem);
			if (pParentData == NULL)
				return;

			LandsideApproachCriteria* pApproachLane = (LandsideApproachCriteria*)(pParentData->m_dwData);
			if (pApproachLane == NULL)
				return;
			
			LandsideApproachIntersection* pLane = (LandsideApproachIntersection*)(pNodeData->m_dwData);
			if (pLane == NULL)
				return;

			pApproachLane->m_laneContainer.DeleteItem(pLane);
			m_wndTreeCtrl.DeleteItem(hItem);
			DeleteTreeNodeData(pNodeData);
			GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
			
		}
		break;
	case TreeNodeData::APPROACHVEHICLE_Node:
		{
			HTREEITEM hParentItem = m_wndTreeCtrl.GetParentItem(hItem);
			if (hParentItem == NULL)
				return;
			TreeNodeData* pParentData = (TreeNodeData*)m_wndTreeCtrl.GetItemData(hParentItem);
			if (pParentData == NULL)
				return;

			LandsideApproachIntersection* pLane = (LandsideApproachIntersection*)(pParentData->m_dwData);
			if (pLane == NULL)
				return;

			LandsideApproachVehicle* pVehicleData = (LandsideApproachVehicle*)(pNodeData->m_dwData);
			if (pVehicleData == NULL)
				return;

			pLane->m_approachContainer.DeleteItem(pVehicleData);
			m_wndTreeCtrl.DeleteItem(hItem);
			DeleteTreeNodeData(pNodeData);
			GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
		}
		break;
	case TreeNodeData::RANDOMVEHICLE_Node:
		{
			HTREEITEM hParentItem = m_wndTreeCtrl.GetParentItem(hItem);
			if (hParentItem == NULL)
				return;
			TreeNodeData* pParentData = (TreeNodeData*)m_wndTreeCtrl.GetItemData(hParentItem);
			if (pParentData == NULL)
				return;

			LandsideRandomCriteria* pRandomBehavior = (LandsideRandomCriteria*)(pParentData->m_dwData);
			if (pRandomBehavior == NULL)
				return;

			LandsideRandomVehicle* pRandVehicle = (LandsideRandomVehicle*)(pNodeData->m_dwData);
			if (pRandVehicle == NULL)
				return;
		
			pRandomBehavior->m_randomContainter.DeleteItem(pRandVehicle);
			m_wndTreeCtrl.DeleteItem(hItem);
			DeleteTreeNodeData(pNodeData);
			GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
		}
		break;
	case TreeNodeData::OVERTAKING_Node:
		{
			HTREEITEM hParentItem = m_wndTreeCtrl.GetParentItem(hItem);
			if (hParentItem == NULL)
				return;
			TreeNodeData* pParentData = (TreeNodeData*)m_wndTreeCtrl.GetItemData(hParentItem);
			if (pParentData == NULL)
				return;	

			LandsideOvertakingCriteria* pSlower = (LandsideOvertakingCriteria*)(pParentData->m_dwData);
			if (pSlower == NULL)
				return;	
		
			LandsideOverTakingVehicle* pOvertaking = (LandsideOverTakingVehicle*)(pNodeData->m_dwData);
			if (pOvertaking == NULL)
				return;
			
			pSlower->m_dataContainer.DeleteItem(pOvertaking);
			m_wndTreeCtrl.DeleteItem(hItem);
			DeleteTreeNodeData(pNodeData);
			GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
		}
		break;
	case TreeNodeData::OVERTAKEN_Node:
		{
			HTREEITEM hParentItem = m_wndTreeCtrl.GetParentItem(hItem);
			if (hParentItem == NULL)
				return;
			TreeNodeData* pParentData = (TreeNodeData*)m_wndTreeCtrl.GetItemData(hParentItem);
			if (pParentData == NULL)
				return;

			LandsideOverTakingVehicle* pOvertaking = (LandsideOverTakingVehicle*)(pParentData->m_dwData);
			if (pOvertaking == NULL)
				return;

			LandsideOvertakenVehicle* pOvertaken = (LandsideOvertakenVehicle*)(pNodeData->m_dwData);
			if (pOvertaken == NULL)
				return;
			
			pOvertaking->DeleteItem(pOvertaken);
			m_wndTreeCtrl.DeleteItem(hItem);
			DeleteTreeNodeData(pNodeData);
			GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
		}
		break;
	default:
		break;
	}
}

void CDlgLaneChangeCriteria::OnBtnSave()
{
	try
	{
		CADODatabase::BeginTransaction();
		m_laneChangeCriteria.SaveData(-1);
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

void CDlgLaneChangeCriteria::SetListBoxContent()
{
	int idx = m_wndListCtrl.AddString("APPROACHING INTERSECTION");
	m_wndListCtrl.SetItemData(idx,landsideLaneChangeCriteria::Approach_Intersection_Type);

	idx = m_wndListCtrl.AddString("APPROACHING TOLL GATES");
	m_wndListCtrl.SetItemData(idx,landsideLaneChangeCriteria::Approach_Tollgates_Type);

	idx = m_wndListCtrl.AddString("RANDOM BEHAVIOUR");
	m_wndListCtrl.SetItemData(idx,landsideLaneChangeCriteria::Random_Behavior_Type);

	idx = m_wndListCtrl.AddString("OVERTAKING SLOWER VEHICLE");
	m_wndListCtrl.SetItemData(idx,landsideLaneChangeCriteria::OverTaking_SlowerVehicle_Type);

	m_wndListCtrl.SetCurSel(0);
	SetTreeContent(landsideLaneChangeCriteria::Approach_Intersection_Type);
}

void CDlgLaneChangeCriteria::SetTreeContent( landsideLaneChangeCriteria::NewCriteriaType emType)
{
	ClearTreeNode();
	m_wndTreeCtrl.DeleteAllItems();
	m_wndTreeCtrl.SetImageList(m_wndTreeCtrl.GetImageList(TVSIL_NORMAL),TVSIL_NORMAL);

	for (int i = 0; i < m_laneChangeCriteria.GetItemCount(); i++)
	{
		landsideLaneChangeCriteria* pCriteria = m_laneChangeCriteria.GetItem(i);
		if(pCriteria->GetType() != emType)
			continue;
		
		//approach
		if (pCriteria->toApproach())
		{
			InsertApproachItem(pCriteria->toApproach());
			continue;
		}
		//Random Behavior
		if (pCriteria->toRandomBehavior())
		{
			InsertRandomItem(pCriteria->toRandomBehavior());
			continue;
		}

		//over taking
		if (pCriteria->toOverTaking())
		{
			InsertOvertakingItem(pCriteria->toOverTaking());
			continue;
		}
	}

	HTREEITEM hItem = m_wndTreeCtrl.GetRootItem();
	if (hItem)
	{
		m_wndTreeCtrl.SelectSetFirstVisible(hItem);
		m_wndTreeCtrl.SetFocus();
		m_wndTreeCtrl.SetItemState(hItem,TVIS_SELECTED,TVIS_SELECTED);
	}
}

void CDlgLaneChangeCriteria::InsertApproachItem( LandsideApproachCriteria* pCriteria )
{
	HTREEITEM hStretchItem = InsertStretchItem(pCriteria);

	for (int i = 0; i < pCriteria->m_laneContainer.GetItemCount(); i++)
	{
		LandsideApproachIntersection* pLane = pCriteria->m_laneContainer.GetItem(i);
		InsertIntersectionItem(hStretchItem,pLane);
	}
	m_wndTreeCtrl.Expand(hStretchItem,TVE_EXPAND);
}

void CDlgLaneChangeCriteria::InsertIntersectionItem( HTREEITEM hItem,LandsideApproachIntersection* pIntersection )
{
	COOLTREE_NODE_INFO cni;
	CCoolTree::InitNodeInfo(this,cni);

	TreeNodeData* pNodeData = new TreeNodeData();
	pNodeData->m_emType = TreeNodeData::INTERSECTION_Node;
	pNodeData->m_dwData = (DWORD)pIntersection;
	cni.nt = NT_NORMAL;
	cni.net = NET_SHOW_DIALOGBOX;
	CString strIntersection;
	strIntersection.Format(_T("Intersection: %s"),pIntersection->GetIntersectionNode().GetIDString());
	HTREEITEM hIntersectionItem = m_wndTreeCtrl.InsertItem(strIntersection,cni,FALSE,FALSE,hItem);
	m_wndTreeCtrl.SetItemData(hIntersectionItem,(DWORD)pNodeData);
	m_vNodeData.push_back(pNodeData);

	for (int i = 0; i < pIntersection->m_approachContainer.GetItemCount(); i++)
	{
		LandsideApproachVehicle* pVehicle = pIntersection->m_approachContainer.GetItem(i);
		InsertApproachVehicleItem(hIntersectionItem,pVehicle);
	}
	m_wndTreeCtrl.Expand(hItem,TVE_EXPAND);
}

void CDlgLaneChangeCriteria::InsertApproachVehicleItem( HTREEITEM hItem, LandsideApproachVehicle* pVehicle )
{
	COOLTREE_NODE_INFO cni;
	CCoolTree::InitNodeInfo(this,cni);

	TreeNodeData* pNodeData = new TreeNodeData();
	pNodeData->m_emType = TreeNodeData::APPROACHVEHICLE_Node;
	pNodeData->m_dwData = (DWORD)pVehicle;
	cni.nt = NT_NORMAL;
	cni.net = NET_SHOW_DIALOGBOX;
	CString strVehicleType;
	strVehicleType.Format(_T("Vehicle Type: %s"),pVehicle->GetVehicleType());
	HTREEITEM hVehicleItem = m_wndTreeCtrl.InsertItem(strVehicleType,cni,FALSE,FALSE,hItem);
	m_wndTreeCtrl.SetItemData(hVehicleItem,(DWORD)pNodeData);
	m_vNodeData.push_back(pNodeData);

	//start
	{
		pNodeData = new TreeNodeData();
		pNodeData->m_emType = TreeNodeData::START_Node;
		pNodeData->m_dwData = (DWORD)pVehicle;
		cni.nt = NT_NORMAL;
		cni.net = NET_COMBOBOX;
		CString strStart;
		strStart.Format(_T("Start consideration at from intersection(m): %s"),pVehicle->GetProDist().getPrintDist());
		HTREEITEM hStartItem = m_wndTreeCtrl.InsertItem(strStart,cni,FALSE,FALSE,hVehicleItem);
		m_wndTreeCtrl.SetItemData(hStartItem,(DWORD)pNodeData);
		m_vNodeData.push_back(pNodeData);
	}
	m_wndTreeCtrl.Expand(hItem,TVE_EXPAND);
	m_wndTreeCtrl.Expand(hVehicleItem,TVE_EXPAND);
}


void CDlgLaneChangeCriteria::InsertRandomVehicle( HTREEITEM hItem, LandsideRandomVehicle* pVehicle )
{
	COOLTREE_NODE_INFO cni;
	CCoolTree::InitNodeInfo(this,cni);

	TreeNodeData* pNodeData = new TreeNodeData();
	pNodeData->m_emType = TreeNodeData::RANDOMVEHICLE_Node;
	pNodeData->m_dwData = (DWORD)pVehicle;
	cni.nt = NT_NORMAL;
	cni.net = NET_SHOW_DIALOGBOX;
	CString strVehicleType;
	strVehicleType.Format(_T("Vehicle Type: %s"),pVehicle->GetVehicleType());
	HTREEITEM hVehicleItem = m_wndTreeCtrl.InsertItem(strVehicleType,cni,FALSE,FALSE,hItem);
	m_wndTreeCtrl.SetItemData(hVehicleItem,(DWORD)pNodeData);
	m_vNodeData.push_back(pNodeData);
	
	//percent node
	{
		pNodeData = new TreeNodeData();
		pNodeData->m_emType = TreeNodeData::RANDOMPERCENT_Node;
		pNodeData->m_dwData = (DWORD)pVehicle;
		cni.nt = NT_NORMAL;
		cni.net = NET_EDIT_FLOAT;
		CString strVehicleType;
		strVehicleType.Format(_T("Percentage of vehicles: %.2f (%%)"),pVehicle->GetPercent());
		cni.fMinValue = static_cast<float>(pVehicle->GetPercent());
		HTREEITEM hPercentItem = m_wndTreeCtrl.InsertItem(strVehicleType,cni,FALSE,FALSE,hVehicleItem);
		m_wndTreeCtrl.SetItemData(hPercentItem,(DWORD)pNodeData);
		m_vNodeData.push_back(pNodeData);

		//cycle time
		{
			cni.nt = NT_NORMAL;
			cni.net = NET_DATETIMEPICKER;
			CString strStart;
			strStart.Format(_T("Cycle Time: %02d:%02d (mm:ss)"),pVehicle->GetCycleTime().GetMinute(),pVehicle->GetCycleTime().GetSecond());
			HTREEITEM hCycleItem = m_wndTreeCtrl.InsertItem(strStart,cni,FALSE,FALSE,hPercentItem);
			m_wndTreeCtrl.SetItemData(hCycleItem,(DWORD)pVehicle->GetCycleTime().asSeconds());
		}
		m_wndTreeCtrl.Expand(hPercentItem,TVE_EXPAND);
	}
	m_wndTreeCtrl.Expand(hVehicleItem,TVE_EXPAND);
	m_wndTreeCtrl.Expand(hItem,TVE_EXPAND);
}

void CDlgLaneChangeCriteria::InsertRandomItem( LandsideRandomCriteria* pCriteria )
{
	HTREEITEM hStretchItem = InsertStretchItem(pCriteria);
	for (int i = 0; i < pCriteria->m_randomContainter.GetItemCount(); i++)
	{
		LandsideRandomVehicle* pRandomVehicle = pCriteria->m_randomContainter.GetItem(i);
		InsertRandomVehicle(hStretchItem,pRandomVehicle);
	}

	m_wndTreeCtrl.Expand(hStretchItem,TVE_EXPAND);
}

void CDlgLaneChangeCriteria::InsertOvertakenSlowerVehicleItem(HTREEITEM hItem, LandsideOvertakenVehicle* pOvertaken )
{
	COOLTREE_NODE_INFO cni;
	CCoolTree::InitNodeInfo(this,cni);

	TreeNodeData* pNodeData = new TreeNodeData();
	pNodeData->m_emType = TreeNodeData::OVERTAKEN_Node;
	pNodeData->m_dwData = (DWORD)pOvertaken;
	cni.nt = NT_NORMAL;
	cni.net = NET_SHOW_DIALOGBOX;
	CString strVehicleType;
	strVehicleType.Format(_T("Vehicle type to be overtaken: %s"),pOvertaken->GetOverTakenVehicle());
	HTREEITEM hTakenItem = m_wndTreeCtrl.InsertItem(strVehicleType,cni,FALSE,FALSE,hItem);
	m_wndTreeCtrl.SetItemData(hTakenItem,(DWORD)pNodeData);
	m_vNodeData.push_back(pNodeData);

	//percentage of vehicles
	{
		pNodeData = new TreeNodeData();
		pNodeData->m_emType = TreeNodeData::SLOWERPERCENT_Node;
		pNodeData->m_dwData = (DWORD)pOvertaken;
		cni.nt = NT_NORMAL;
		cni.net = NET_EDIT_FLOAT;
		CString strVehicleType;
		strVehicleType.Format(_T("Percentage of vehicles: %.2f (%%)"),pOvertaken->GetPercent());
		cni.fMinValue = static_cast<float>(pOvertaken->GetPercent());
		HTREEITEM hPercentItem = m_wndTreeCtrl.InsertItem(strVehicleType,cni,FALSE,FALSE,hTakenItem);
		m_wndTreeCtrl.SetItemData(hPercentItem,(DWORD)pNodeData);
		m_vNodeData.push_back(pNodeData);
	}
	m_wndTreeCtrl.Expand(hTakenItem,TVE_EXPAND);
	m_wndTreeCtrl.Expand(hItem,TVE_EXPAND);
}

void CDlgLaneChangeCriteria::InsertOvertakingSlowerVehicleItem(HTREEITEM hItem, LandsideOverTakingVehicle* pOvertaking )
{
	COOLTREE_NODE_INFO cni;
	CCoolTree::InitNodeInfo(this,cni);

	TreeNodeData* pNodeData = new TreeNodeData();
	pNodeData->m_emType = TreeNodeData::OVERTAKING_Node;
	pNodeData->m_dwData = (DWORD)pOvertaking;
	cni.nt = NT_NORMAL;
	cni.net = NET_SHOW_DIALOGBOX;
	CString strVehicleType;
	strVehicleType.Format(_T("Vehicle Type overtaking: %s"),pOvertaking->GetOverTakingVehicle());
	HTREEITEM hTakingItem = m_wndTreeCtrl.InsertItem(strVehicleType,cni,FALSE,FALSE,hItem);
	m_wndTreeCtrl.SetItemData(hTakingItem,(DWORD)pNodeData);
	m_vNodeData.push_back(pNodeData);

	for (int i = 0; i < pOvertaking->GetItemCount(); i++)
	{
		LandsideOvertakenVehicle* pOverTaken = pOvertaking->GetItem(i);
		InsertOvertakenSlowerVehicleItem(hTakingItem,pOverTaken);
	}
	m_wndTreeCtrl.Expand(hTakingItem,TVE_EXPAND);
	m_wndTreeCtrl.Expand(hItem,TVE_EXPAND);
}

void CDlgLaneChangeCriteria::InsertOvertakingItem( LandsideOvertakingCriteria* pCriteria )
{
	HTREEITEM hStretchItem = InsertStretchItem(pCriteria);


	for (int i = 0; i < pCriteria->m_dataContainer.GetItemCount(); i++)
	{
		LandsideOverTakingVehicle* pOvertaking = pCriteria->m_dataContainer.GetItem(i);
		InsertOvertakingSlowerVehicleItem(hStretchItem,pOvertaking);
	}
	m_wndTreeCtrl.Expand(hStretchItem,TVE_EXPAND);
}

HTREEITEM CDlgLaneChangeCriteria::InsertStretchItem( landsideLaneChangeCriteria* pCriteria )
{
	COOLTREE_NODE_INFO cni;
	CCoolTree::InitNodeInfo(this,cni);

	TreeNodeData* pNodeData = new TreeNodeData();
	pNodeData->m_emType = TreeNodeData::STRETCH_Node;
	pNodeData->m_dwData = (DWORD)pCriteria;
	cni.nt = NT_NORMAL;
	cni.net = NET_SHOW_DIALOGBOX;
	CString strGroupName;
	strGroupName.Format(_T("Stretch: %s"),pCriteria->GetStretch().GetIDString());
	HTREEITEM hStetchItem = m_wndTreeCtrl.InsertItem(strGroupName,cni,FALSE,FALSE,TVI_ROOT);
	m_wndTreeCtrl.SetItemData(hStetchItem,(DWORD)pNodeData);
	m_vNodeData.push_back(pNodeData);

	return hStetchItem;
}

void CDlgLaneChangeCriteria::OnContextMenu( CWnd* pWnd, CPoint point )
{
	int nSel = m_wndListCtrl.GetCurSel();
	if (nSel == LB_ERR)
		return;
	landsideLaneChangeCriteria::NewCriteriaType emType = landsideLaneChangeCriteria::NewCriteriaType(nSel);

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
	case TreeNodeData::STRETCH_Node:
		{
			if (emType == landsideLaneChangeCriteria::Approach_Intersection_Type || emType == landsideLaneChangeCriteria::Approach_Tollgates_Type)
			{
				menuPopup.AppendMenu(MF_POPUP,ID_TOOLBAR_ADD,_T("Add Intersection"));
			}
			else if (emType == landsideLaneChangeCriteria::Random_Behavior_Type)
			{
				menuPopup.AppendMenu(MF_POPUP,ID_TOOLBAR_ADD,_T("Add Vehicle Type"));
			}
			else if (emType == landsideLaneChangeCriteria::OverTaking_SlowerVehicle_Type)
			{
				menuPopup.AppendMenu(MF_POPUP,ID_TOOLBAR_ADD,_T("Add Vehicle Type Overtaking"));
			}
			
			menuPopup.AppendMenu(MF_POPUP,ID_TOOLBAR_DEL,_T("Delete Stretch"));
			menuPopup.TrackPopupMenu(TPM_LEFTALIGN, point.x, point.y, this);
		}
		break;
	case TreeNodeData::INTERSECTION_Node:
		{
			menuPopup.AppendMenu(MF_POPUP,ID_TOOLBAR_ADD,_T("Add Vehicle Type"));
			menuPopup.AppendMenu(MF_POPUP,ID_TOOLBAR_DEL,_T("Delete Intersection"));
			menuPopup.TrackPopupMenu(TPM_LEFTALIGN, point.x, point.y, this);
		}
		break;
	case TreeNodeData::APPROACHVEHICLE_Node:
		{
			menuPopup.AppendMenu(MF_POPUP,ID_TOOLBAR_DEL,_T("Delete Vehicle Type"));
			menuPopup.TrackPopupMenu(TPM_LEFTALIGN, point.x, point.y, this);
		}
		break;
	case TreeNodeData::RANDOMVEHICLE_Node:
		{
			menuPopup.AppendMenu(MF_POPUP,ID_TOOLBAR_DEL,_T("Delete Vehicle Type"));
			menuPopup.TrackPopupMenu(TPM_LEFTALIGN, point.x, point.y, this);
		}
		break;
	case TreeNodeData::OVERTAKING_Node:
		{
			menuPopup.AppendMenu(MF_POPUP,ID_TOOLBAR_ADD,_T("Add Vehicle Type to be Overtaken"));
			menuPopup.AppendMenu(MF_POPUP,ID_TOOLBAR_DEL,_T("Delete Vehicle Type Overtaking"));
			menuPopup.TrackPopupMenu(TPM_LEFTALIGN, point.x, point.y, this);
		}
		break;
	case TreeNodeData::OVERTAKEN_Node:
		{
			menuPopup.AppendMenu(MF_POPUP,ID_TOOLBAR_DEL,_T("Delete Vehicle Type to be Overtaken"));
			menuPopup.TrackPopupMenu(TPM_LEFTALIGN, point.x, point.y, this);
		}
		break;
	default:
		break;
	}
	menuPopup.DestroyMenu();
}

void CDlgLaneChangeCriteria::OnLvnTreeItemchangedList( NMHDR *pNMHDR, LRESULT *pResult )
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	*pResult = 0;
	if (pNMTreeView->itemNew.hItem)
	{
		m_wndTreeCtrl.SelectItem(pNMTreeView->itemNew.hItem);
		if (pNMTreeView->itemNew.hItem != pNMTreeView->itemOld.hItem)
		{
			HTREEITEM hItem = pNMTreeView->itemNew.hItem;
			COOLTREE_NODE_INFO* pCNI = (COOLTREE_NODE_INFO*)m_wndTreeCtrl.GetItemNodeInfo(hItem);
			if (pCNI->net != NET_DATETIMEPICKER)
			{
				TreeNodeData* pNodeData = (TreeNodeData*)m_wndTreeCtrl.GetItemData(hItem);
				if (pNodeData) 
				{
					//check whether can delete
					if (std::find(m_canDeleteNode.begin(),m_canDeleteNode.end(),pNodeData->m_emType) != m_canDeleteNode.end())
					{
						m_wndToolBar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_DEL,TRUE);
					}
					else
					{
						m_wndToolBar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_DEL,FALSE);
					}

					//check whether can new
					if (std::find(m_canNewNode.begin(),m_canNewNode.end(),pNodeData->m_emType) != m_canNewNode.end())
					{
						m_wndToolBar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_ADD,TRUE);
					}
					else
					{
						m_wndToolBar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_ADD,FALSE);
					}
				}
			}
			else
			{
				m_wndToolBar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_ADD,FALSE);
				m_wndToolBar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_DEL,FALSE);
			}
			
		}
		return;
	}
	m_wndToolBar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_ADD,TRUE);
	m_wndToolBar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_DEL,FALSE);
}


void CDlgLaneChangeCriteria::OnLbnSelchangeListBox()
{
	int nSel = m_wndListCtrl.GetCurSel();
	if (nSel == LB_ERR)
		return;

	landsideLaneChangeCriteria::NewCriteriaType emType = (landsideLaneChangeCriteria::NewCriteriaType)m_wndListCtrl.GetItemData(nSel);
	SetTreeContent(emType);
	UpdateToolBarState();
}

LRESULT CDlgLaneChangeCriteria::OnLeftDoubleClick( WPARAM wParam, LPARAM lParam )
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
	case TreeNodeData::STRETCH_Node:
		{
			LandsideFacilityLayoutObjectList* pLayoutList = &(m_pDoc->GetInputLandside()->getObjectList());
			CDlgSelectLandsideObject dlg(pLayoutList,this);
			dlg.AddObjType(ALT_LSTRETCH);
			if(dlg.DoModal() == IDOK)
			{
				ALTObjectID stretchName = dlg.getSelectObject();
				if (!stretchName.IsBlank())
				{
					landsideLaneChangeCriteria* pCriteria = (landsideLaneChangeCriteria*)(pNodeData->m_dwData);
					if (pCriteria == NULL)
						return 0;
					
					if (stretchName == pCriteria->GetStretch())
						return 0;

					if (m_laneChangeCriteria.ExistSameStretch(pCriteria->GetType(),stretchName))
					{
						pCriteria->SetStretch(stretchName);
						CString strStretch;
						strStretch.Format(_T("Stretch: %s"),stretchName.GetIDString());
						m_wndTreeCtrl.SetItemText(hItem,strStretch);
						GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
					}
					else
					{
						MessageBox(_T("Exist the Same Stretch!!!"),_T("Warning"),MB_OK);
					}
				}
			}		
		}
		break;
	case TreeNodeData::INTERSECTION_Node:
		{
			LandsideFacilityLayoutObjectList* pLayoutList = &(m_pDoc->GetInputLandside()->getObjectList());
			CDlgSelectLandsideObject dlg(pLayoutList,this);
			dlg.AddObjType(ALT_LINTERSECTION);
			if(dlg.DoModal() == IDOK)
			{
				ALTObjectID intersectName = dlg.getSelectObject();
				if (!intersectName.IsBlank())
				{
					LandsideApproachIntersection* pIntersection = (LandsideApproachIntersection*)(pNodeData->m_dwData);
					if (pIntersection == NULL)
						return 0;

					if (intersectName == pIntersection->GetIntersectionNode())
						return 0;
					
					HTREEITEM hParentItem = m_wndTreeCtrl.GetParentItem(hItem);
					TreeNodeData* pParentData = (TreeNodeData*)m_wndTreeCtrl.GetItemData(hParentItem);
					LandsideApproachCriteria* pCriteria = (LandsideApproachCriteria*)(pParentData->m_dwData);
					if (pCriteria && !pCriteria->ExistSameIntersection(intersectName))
					{
						pIntersection->SetIntersectionNode(intersectName);
						CString strIntersect;
						strIntersect.Format(_T("Intersection: %s"),intersectName.GetIDString());
						m_wndTreeCtrl.SetItemText(hItem,strIntersect);
						GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
					}
					else
					{
						MessageBox(_T("Exist the Same Intersection!!!"),_T("Warning"),MB_OK);
					}
				}
			}	
		}
		break;
	case TreeNodeData::APPROACHVEHICLE_Node:
		{
			CDlgSelectLandsideVehicleType dlg;
			if(dlg.DoModal() == IDOK)
			{
				CString strVehicleType = dlg.GetName();
				if (!strVehicleType.IsEmpty())
				{
					HTREEITEM hParentItem = m_wndTreeCtrl.GetParentItem(hItem);
					TreeNodeData* pParentData = (TreeNodeData*)m_wndTreeCtrl.GetItemData(hParentItem);
					LandsideApproachIntersection* pIntersection = (LandsideApproachIntersection*)(pParentData->m_dwData);
					
					LandsideApproachVehicle* pVehicle = (LandsideApproachVehicle*)pNodeData->m_dwData;
					if (pVehicle->GetVehicleType() == strVehicleType)
					{
						return 0;
					}
					if (pIntersection && !pIntersection->ExistSameVehicleType(strVehicleType))
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
	case TreeNodeData::RANDOMVEHICLE_Node:
		{
			CDlgSelectLandsideVehicleType dlg;
			if(dlg.DoModal() == IDOK)
			{
				CString strVehicleType = dlg.GetName();
				if (!strVehicleType.IsEmpty())
				{
					HTREEITEM hParentItem = m_wndTreeCtrl.GetParentItem(hItem);
					TreeNodeData* pParentData = (TreeNodeData*)m_wndTreeCtrl.GetItemData(hParentItem);
					LandsideRandomCriteria* pCriteria = (LandsideRandomCriteria*)(pParentData->m_dwData);
					
					LandsideRandomVehicle* pVehicle = (LandsideRandomVehicle*)pNodeData->m_dwData;
					if (pVehicle->GetVehicleType() == strVehicleType)
						return 0;
					if (pCriteria && !pCriteria->ExistSameVehicleType(strVehicleType))
					{
						pVehicle->SetVehicleType(strVehicleType);
						CString strRandomVehicle;
						strRandomVehicle.Format(_T("Vehicle Type: %s"),strVehicleType);
						m_wndTreeCtrl.SetItemText(hItem,strRandomVehicle);
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
	case TreeNodeData::OVERTAKING_Node:
		{
			CDlgSelectLandsideVehicleType dlg;
			if(dlg.DoModal() == IDOK)
			{
				CString strVehicleType = dlg.GetName();
				if (!strVehicleType.IsEmpty())
				{
					LandsideOverTakingVehicle* pOvertaking = (LandsideOverTakingVehicle*)pNodeData->m_dwData;
					if (pOvertaking->GetOverTakingVehicle() == strVehicleType)
						return 0;
					
					pOvertaking->SetOverTakingVehicle(strVehicleType);
					CString strOvertaking;
					strOvertaking.Format(_T("Vehicle type overtaking: %s"),strVehicleType);
					m_wndTreeCtrl.SetItemText(hItem,strOvertaking);
					GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
				}
			}
		}
		break;
	case TreeNodeData::OVERTAKEN_Node:
		{
			CDlgSelectLandsideVehicleType dlg;
			if(dlg.DoModal() == IDOK)
			{
				CString strVehicleType = dlg.GetName();
				if (!strVehicleType.IsEmpty())
				{
					LandsideOvertakenVehicle* pOvertaken = (LandsideOvertakenVehicle*)pNodeData->m_dwData;
					if (strVehicleType == pOvertaken->GetOverTakenVehicle())
						return 0;
					
					HTREEITEM hParentItem = m_wndTreeCtrl.GetParentItem(hItem);
					TreeNodeData* pParentData = (TreeNodeData*)m_wndTreeCtrl.GetItemData(hParentItem);
					LandsideOverTakingVehicle* pOvertaking = (LandsideOverTakingVehicle*)(pParentData->m_dwData);

					HTREEITEM hGrandItem = m_wndTreeCtrl.GetParentItem(hParentItem);
					TreeNodeData* pGrandData = (TreeNodeData*)m_wndTreeCtrl.GetItemData(hGrandItem);
					LandsideOvertakingCriteria* pCriteria = (LandsideOvertakingCriteria*)(pGrandData->m_dwData);

					if (pCriteria && !pCriteria->ExistSameVehiclePair(pOvertaking->GetOverTakingVehicle(),pOvertaken->GetOverTakenVehicle()))
					{
						pOvertaken->SetOverTakenVehicle(strVehicleType);
						CString strOvertaken;
						strOvertaken.Format(_T("Vehicle type to be overtaken: %s"),strVehicleType);
						m_wndTreeCtrl.SetItemText(hItem,strOvertaken);
						GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
					}
					else
					{
						MessageBox(_T("Exist the Same Overtaking Vehicle Type and Overtaken Vehicle Type!!!"),_T("Warning"),MB_OK);
					}
				}
			}
		}
		break;
	default:
		break;
	}
	
	return 1;
}

LRESULT CDlgLaneChangeCriteria::DefWindowProc( UINT message, WPARAM wParam, LPARAM lParam )
{
	switch (message)
	{
	case UM_CEW_EDITSPIN_BEGIN:
		{
			HTREEITEM hItem = (HTREEITEM)wParam;
			COOLTREE_NODE_INFO* cni = m_wndTreeCtrl.GetItemNodeInfo(hItem);
			TreeNodeData* pNodeData = (TreeNodeData*)m_wndTreeCtrl.GetItemData(hItem);
			if (pNodeData)
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
			double dValue = atof(strValue);
			if (pNodeData)
			{
				if (pNodeData->m_emType == TreeNodeData::RANDOMPERCENT_Node)
				{
					LandsideRandomVehicle* pVehicle = (LandsideRandomVehicle*)(pNodeData->m_dwData);
					pVehicle->SetPercent(dValue);
					CString strPercent;
					strPercent.Format(_T("Percentage of vehicles: %.2f (%%)"),dValue);
					cni->fMinValue = static_cast<float>(dValue);
					m_wndTreeCtrl.SetItemText(hItem,strPercent);
					GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);

				}
				else if (pNodeData->m_emType == TreeNodeData::SLOWERPERCENT_Node)
				{	
					LandsideOvertakenVehicle* pOverTaken = (LandsideOvertakenVehicle*)(pNodeData->m_dwData);
					pOverTaken->SetPercent(dValue);
					CString strPercent;
					strPercent.Format(_T("Percentage of vehicles: %.2f (%%)"),dValue);
					cni->fMinValue = static_cast<float>(dValue);
					m_wndTreeCtrl.SetItemText(hItem,strPercent);
					GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
				}
			}
		}
		break;
	case UM_CEW_COMBOBOX_BEGIN:
		{
			HTREEITEM hItem=(HTREEITEM)wParam;
			TreeNodeData* pNodeData = (TreeNodeData*)m_wndTreeCtrl.GetItemData(hItem);
			if(pNodeData && pNodeData->m_emType == TreeNodeData::START_Node)
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

				InputLandside* pInputLandside = m_pDoc->GetInputLandside();
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

			if(pNodeData && pNodeData->m_emType == TreeNodeData::START_Node)
			{
				InputLandside* pInputLandside = m_pDoc->GetInputLandside();
				if (pInputLandside)
				{
					LandsideApproachVehicle* pVehicle = (LandsideApproachVehicle*)(pNodeData->m_dwData);
					if (pVehicle)
					{
						if(iSelCombox == 0)
						{
							CDlgProbDist dlg(  pInputLandside->getInputTerminal()->m_pAirportDB, true );
							if(dlg.DoModal()==IDOK) 
							{
								int idxPD = dlg.GetSelectedPDIdx();
								ASSERT(idxPD!=-1);
								CProbDistEntry* pde = _g_GetActiveProbMan( pInputLandside->getInputTerminal() )->getItem(idxPD);

								pVehicle->SetProDist(pde);
								CString strStart;
								strStart.Format(_T("Start consideration at from intersection(m): %s"),pVehicle->GetProDist().getPrintDist());
								m_wndTreeCtrl.SetItemText(hItem,strStart);
								GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
							}
						}
						else
						{
							ASSERT( iSelCombox-1 >= 0 && iSelCombox-1 < static_cast<int>(_g_GetActiveProbMan( pInputLandside->getInputTerminal()  )->getCount()) );
							CProbDistEntry* pPBEntry = _g_GetActiveProbMan( pInputLandside->getInputTerminal()  )->getItem( iSelCombox-1 );
							pVehicle->SetProDist(pPBEntry);
							CString strStart;
							strStart.Format(_T("Start consideration at from intersection(m): %s"),pVehicle->GetProDist().getPrintDist());
							m_wndTreeCtrl.SetItemText(hItem,strStart);
							GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);

						}
					}

				}

			}
		}
		break;
	case UM_CEW_DATETIME_END:
	{
		HTREEITEM hItem=(HTREEITEM)wParam;
		HTREEITEM hParentItem = m_wndTreeCtrl.GetParentItem(hItem);
		TreeNodeData* pNodeData = (TreeNodeData*)m_wndTreeCtrl.GetItemData(hParentItem);
		LandsideRandomVehicle* pVehicle = (LandsideRandomVehicle*)(pNodeData->m_dwData);
		long lSecondTime = (long)lParam;
		ElapsedTime estTime(lSecondTime);
		CString strCycle;
		strCycle.Format(_T("Cycle Time: %02d:%02d (mm:ss)"),estTime.GetMinute(),estTime.GetSecond());
		pVehicle->SetCycleTime(estTime);
		m_wndTreeCtrl.SetItemText(hItem,strCycle);
		m_wndTreeCtrl.SetItemData(hItem,(DWORD)lSecondTime);
		GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
	}
	break;
	default:
		break;
	}

	return CXTResizeDialog::DefWindowProc(message,wParam,lParam);
}

void CDlgLaneChangeCriteria::ClearTreeNode()
{
	for (int i = 0; i < (int)m_vNodeData.size(); i++)
	{
		delete m_vNodeData[i];
	}
	m_vNodeData.clear();
}

