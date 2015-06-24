#include "stdafx.h"
#include "Resource.h"
#include "DlgFlightPlan.h"
#include "../common/flt_cnst.h"
#include "FlightDialog.h"
#include "DlgProbDist.h"
#include "../Engine/terminal.h"
#include "../common/ProbDistManager.h"
#include "ProcesserDialog.h"
#include "../Common/WinMsg.h"
#include "../AirsideGUI/DlgSelectAirRoute.h"
#include "../Database/ARCportDatabaseConnection.h"
#include "../InputAirside/AirRoute.h"
#include "../InputAirside/ALTAirport.h"
#include "../InputAirside/Runway.h"
#include "../InputAirside/InputAirside.h"

using namespace ns_FlightPlan;

namespace
{

const UINT ID_NEW_FLTPLAN = 10;
const UINT ID_DEL_FLTPLAN = 11;
const UINT ID_EDIT_FLTPLAN = 12;
const UINT ID_NEW_AIRROUTEPLAN = 20;
const UINT ID_DEL_AIRROUTEPLAN = 21;

struct TreeRouteData
{
	TreeRouteData(HTREEITEM hItemFix, HTREEITEM hItemAlt, HTREEITEM hItemSpd, HTREEITEM hItemPhase,WayPointProperty* pWayPointProp)
	{
		m_hItemFix = hItemFix;
		m_hItemAlt = hItemAlt;
		m_hItemSpd = hItemSpd;
		m_hItemPhase = hItemPhase;
		m_pWayPointProp = pWayPointProp;
	}

	HTREEITEM			m_hItemFix;
	HTREEITEM			m_hItemAlt;
	HTREEITEM			m_hItemSpd;
	HTREEITEM			m_hItemPhase;
	WayPointProperty*	m_pWayPointProp;
};

int GetTreeItemDepthIndex(const CTreeCtrl& treeCtrl, const HTREEITEM hItem)
{
	int nDepth = 0;
	HTREEITEM hTempItem = hItem;
	while (hTempItem = treeCtrl.GetParentItem(hTempItem))
	{
		++nDepth;
	}

	return nDepth;
}

int GetTreeItemWidthIndex(const CTreeCtrl& treeCtrl, const HTREEITEM hItem)
{
	int nWidth = 0;
	HTREEITEM hTempItem = hItem;
	while (hTempItem = treeCtrl.GetPrevSiblingItem(hTempItem))
	{
		++nWidth;
	}

	return nWidth;
}

}

CDlgFlightPlan::CDlgFlightPlan(int nProjID,CAirportDatabase* pAirportDB, CWnd* pParent /*=NULL*/)
 : CXTResizeDialog(CDlgFlightPlan::IDD, pParent)
 , m_flightPlans(nProjID,pAirportDB)
{
	m_nProjID = nProjID;
	m_pCurFltPlan = NULL;
	m_hRClickItem = NULL;
	m_pAirportDB = pAirportDB;
}

CDlgFlightPlan::~CDlgFlightPlan()
{
	for (int i = 0; i < (int)m_vNodeList.size(); i++)
	{
		delete m_vNodeList[i];
	}
	m_vNodeList.clear();
}

void CDlgFlightPlan::DoDataExchange(CDataExchange* pDX)
{
	CXTResizeDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_FLTTYPE, m_lstFltType);
	DDX_Control(pDX, IDC_TREE_PROPERTIES, m_treeFltPlan);
	DDX_Control(pDX, IDC_COMBO_VERTICALPROFILE, m_comboOperationType);
}

BEGIN_MESSAGE_MAP(CDlgFlightPlan, CXTResizeDialog)
	ON_WM_CREATE()
	ON_COMMAND(ID_NEW_FLTPLAN, OnNewFlightPlan)
	ON_COMMAND(ID_DEL_FLTPLAN, OnDelFlightPlan)
	ON_COMMAND(ID_EDIT_FLTPLAN, OnEditPlan)
	ON_COMMAND(ID_NEW_AIRROUTEPLAN, OnNewAirRoute)
	ON_COMMAND(ID_DEL_AIRROUTEPLAN, OnDelAirRoute)
	ON_COMMAND(ID_FLTPLAN_MODIFYALTITUDE, OnFltplanModifyaltitude)
	ON_COMMAND(ID_FLTPLAN_MODIFYSPEED, OnFltplanModifyspeed)
	ON_COMMAND(ID_FLTPLANPHASE_MODIFYPHASE, OnFltplanModifyPhase)
	ON_COMMAND(ID_FLTPLANG_MODIFYVALUE,OnFltModifyValue)
	ON_LBN_SELCHANGE(IDC_LIST_FLTTYPE, OnLbnSelChangeListFltType)
	ON_WM_CONTEXTMENU()
	ON_BN_CLICKED(IDC_FLIGHTPLAN_SAVE, OnSave)
	ON_CBN_SELCHANGE(IDC_COMBO_VERTICALPROFILE, OnOperationTypeChanged)
	ON_MESSAGE(WM_INPLACE_EDIT, OnEndEditTreeValue)
	ON_MESSAGE(WM_INPLACE_COMBO2, OnEndEditComboBoxTreeValue)
END_MESSAGE_MAP()

int CDlgFlightPlan::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CXTResizeDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	if(m_wndFlightPlanToolBar.CreateEx(this,TBSTYLE_FLAT,WS_CHILD|WS_VISIBLE|CBRS_ALIGN_TOP,CRect(0,0,0,0))&&
		!m_wndFlightPlanToolBar.LoadToolBar(IDR_TOOLBAR_FLIGHTPLAN))
	{
		AfxMessageBox("Cann't create toolbar!");
		return FALSE;
	}

	m_wndFlightPlanToolBar.GetToolBarCtrl().SetCmdID(0, ID_NEW_FLTPLAN);
	m_wndFlightPlanToolBar.GetToolBarCtrl().SetCmdID(1, ID_DEL_FLTPLAN);
	m_wndFlightPlanToolBar.GetToolBarCtrl().SetCmdID(2, ID_EDIT_FLTPLAN);

	if(m_wndAirRouteToolBar.CreateEx(this,TBSTYLE_FLAT,WS_CHILD|WS_VISIBLE|CBRS_ALIGN_TOP,CRect(0,0,0,0))&&
		!m_wndAirRouteToolBar.LoadToolBar(IDR_TOOLBAR_FLIGHTPLAN))
	{
		AfxMessageBox("Cann't create toolbar!");
		return FALSE;
	}

	m_wndAirRouteToolBar.SetDlgCtrlID(m_wndFlightPlanToolBar.GetDlgCtrlID()+1);
	m_wndAirRouteToolBar.GetToolBarCtrl().DeleteButton(2);
	m_wndAirRouteToolBar.GetToolBarCtrl().SetCmdID(0, ID_NEW_AIRROUTEPLAN);
	m_wndAirRouteToolBar.GetToolBarCtrl().SetCmdID(1, ID_DEL_AIRROUTEPLAN);

	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),TRUE);
	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),FALSE);

	return 0;
}

BOOL CDlgFlightPlan::OnInitDialog()
{
	CXTResizeDialog::OnInitDialog();

	// set the position of the flight plan toolbar
	CRect rcFltPlanToolbar;
	m_lstFltType.GetWindowRect(&rcFltPlanToolbar);
	ScreenToClient(&rcFltPlanToolbar);
	rcFltPlanToolbar.top -= 28;
	rcFltPlanToolbar.bottom = rcFltPlanToolbar.top + 22;
	rcFltPlanToolbar.left +=2;
	m_wndFlightPlanToolBar.MoveWindow(rcFltPlanToolbar);

	m_wndFlightPlanToolBar.GetToolBarCtrl().EnableButton(ID_NEW_FLTPLAN);
	m_wndFlightPlanToolBar.GetToolBarCtrl().EnableButton(ID_DEL_FLTPLAN);
	m_wndFlightPlanToolBar.GetToolBarCtrl().EnableButton(ID_EDIT_FLTPLAN);

	// set the position of the air route toolbar
	CRect rectAirRouteToolbar;
	m_treeFltPlan.GetWindowRect(&rectAirRouteToolbar);
	ScreenToClient(&rectAirRouteToolbar);
	rectAirRouteToolbar.top -= 28;
	rectAirRouteToolbar.bottom = rectAirRouteToolbar.top + 22;
	rectAirRouteToolbar.left += 4;
	m_wndAirRouteToolBar.MoveWindow(&rectAirRouteToolbar);

	m_wndAirRouteToolBar.GetToolBarCtrl().EnableButton(ID_NEW_AIRROUTEPLAN);
	m_wndAirRouteToolBar.GetToolBarCtrl().EnableButton(ID_DEL_AIRROUTEPLAN);

	SetOperationTypeContents();
	LoadFlightPlans();
	InitializeAirRouteList();

	SetResize(m_wndFlightPlanToolBar.GetDlgCtrlID(), SZ_TOP_LEFT, SZ_TOP_LEFT);
	SetResize(m_wndAirRouteToolBar.GetDlgCtrlID(), SZ_TOP_CENTER, SZ_TOP_CENTER);

	SetResize(IDC_LIST_FLTTYPE, SZ_TOP_LEFT, SZ_BOTTOM_CENTER);
	SetResize(IDC_STATIC_PROPERTIES, SZ_TOP_CENTER , SZ_BOTTOM_RIGHT);
	SetResize(IDC_STATIC_VERTICAL, SZ_TOP_CENTER, SZ_TOP_CENTER);
	SetResize(IDC_COMBO_VERTICALPROFILE, SZ_TOP_CENTER, SZ_TOP_RIGHT);
	SetResize(IDC_STATIC_GROUP, SZ_TOP_CENTER, SZ_BOTTOM_RIGHT);
	SetResize(IDC_TREE_PROPERTIES, SZ_TOP_CENTER, SZ_BOTTOM_RIGHT);

	SetResize(IDC_FLIGHTPLAN_SAVE, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);
	SetResize(IDOK, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);
	SetResize(IDCANCEL, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);
	SetResize(IDC_STATIC_FLTTYPE,SZ_TOP_LEFT,SZ_BOTTOM_CENTER);

	return TRUE;
}
void CDlgFlightPlan::InitializeAirRouteList()
{

}
void CDlgFlightPlan::SetOperationTypeContents()
{
	m_comboOperationType.ResetContent();
	m_comboOperationType.AddString(_T("Take off generic"));
	m_comboOperationType.AddString(_T("Take off specific"));
	m_comboOperationType.AddString(_T("Landing generic"));
	m_comboOperationType.AddString(_T("Landing specific"));
	m_comboOperationType.AddString(_T("Enroute generic"));
	m_comboOperationType.AddString(_T("Enroute specific"));
	m_comboOperationType.AddString(_T("Circuit generic"));
	m_comboOperationType.AddString(_T("Circuit specific"));
}

void CDlgFlightPlan::LoadFlightPlans()
{
	try
	{
		m_flightPlans.Read();
	}
	catch (CADOException& e)
	{
		AfxMessageBox(_T("Read flight plan error:  +")+e.ErrorMessage());
		return;
	}
	size_t nCount = m_flightPlans.GetCount();
	for (size_t i = 0; i < nCount; ++i)
	{
		FlightConstraint fltCnst;
		fltCnst.SetAirportDB( m_pAirportDB );
		fltCnst.setConstraintWithVersion(m_flightPlans.GetFlightPlan(i)->GetFlightType());
		CString fltcnststr;
		fltCnst.screenPrint(fltcnststr);
		m_lstFltType.AddString(fltcnststr);
	}

	if (nCount > 0)
	{
		m_pCurFltPlan = m_flightPlans.GetFlightPlan(0);
		m_lstFltType.SetCurSel(0);
	}
	else
		m_pCurFltPlan = NULL;

	UpdateCurrentPlanContent();
}

void CDlgFlightPlan::UpdateCurrentPlanContent()
{
	if (m_pCurFltPlan == NULL)
	{
		m_comboOperationType.EnableWindow(FALSE);
		m_treeFltPlan.EnableWindow(FALSE);
		m_treeFltPlan.DeleteAllItems();
		m_wndAirRouteToolBar.GetToolBarCtrl().EnableButton(ID_NEW_AIRROUTEPLAN, FALSE);
		m_wndAirRouteToolBar.GetToolBarCtrl().EnableButton(ID_DEL_AIRROUTEPLAN, FALSE);
		return;
	}

	m_treeFltPlan.EnableWindow();
	m_wndAirRouteToolBar.GetToolBarCtrl().EnableButton(ID_NEW_AIRROUTEPLAN);
	m_wndAirRouteToolBar.GetToolBarCtrl().EnableButton(ID_DEL_AIRROUTEPLAN);

	// set operation type
	m_comboOperationType.EnableWindow();
	int nCurSel = 0;

	// set the air route contents
	m_treeFltPlan.SetRedraw(FALSE);
	m_treeFltPlan.DeleteAllItems();

	if (m_pCurFltPlan->GetProcfileType() == ns_FlightPlan::FlightPlanBase::Specific_Profile)
	{
		m_comboOperationType.SetCurSel(m_pCurFltPlan->GetOperationType() * 2 + 1);

		ns_FlightPlan::FlightPlanSpecific* pSpecificProfile = (ns_FlightPlan::FlightPlanSpecific*)m_pCurFltPlan;
		size_t nAirRouteCount = pSpecificProfile->GetAirRouteCount();

		for (size_t i = 0; i < nAirRouteCount; ++i)
		{
			AirRouteProperty* airRouteProp = pSpecificProfile->GetItem(i);

			HTREEITEM hAirRouteItem = m_treeFltPlan.InsertItem(airRouteProp->GetAirRoute().getName());

			//if (m_pCurFltPlan->GetOperationType() == ns_FlightPlan::FlightPlan::takeoff && i == 0)
			//{
			//	int nRunwayID = m_pCurFltPlan->GetItem(0)->GetAirRoute().getRunwayID();
			//	int nRunwayMark = m_pCurFltPlan->GetItem(0)->GetAirRoute().getRunwayMarkIndex();
			//	AddRunwayMarkToTree(nRunwayID,nRunwayMark, hAirRouteItem);
			//	m_treeFltPlan.Expand(hAirRouteItem ,TVE_EXPAND);
			//}

			size_t nWayPointCount = airRouteProp->GetWayPointCount();
			for (size_t j = 0; j < nWayPointCount; ++j)
			{
				WayPointProperty* wayPointProp = airRouteProp->GetItem(j);
				AddWayPointToTree(wayPointProp, hAirRouteItem);
				m_treeFltPlan.Expand(hAirRouteItem ,TVE_EXPAND);
			}

			//if (m_pCurFltPlan->GetOperationType() == ns_FlightPlan::FlightPlan::landing && i == nAirRouteCount-1 )
			//{
			//	int nRunwayID = m_pCurFltPlan->GetItem(nAirRouteCount-1)->GetAirRoute().getRunwayID();
			//	int nRunwayMark = m_pCurFltPlan->GetItem(nAirRouteCount-1)->GetAirRoute().getRunwayMarkIndex();
			//	AddRunwayMarkToTree(nRunwayID,nRunwayMark, hAirRouteItem);
			//	m_treeFltPlan.Expand(hAirRouteItem ,TVE_EXPAND);
			//}
		}
	}
	else
	{
		m_comboOperationType.SetCurSel(m_pCurFltPlan->GetOperationType()*2);

		ns_FlightPlan::FlightPlanGeneric* pGenericProfile = (ns_FlightPlan::FlightPlanGeneric*)m_pCurFltPlan;
		int nCount = pGenericProfile->GetCount();
		for (int i = 0; i < nCount; ++i)
		{
			ns_FlightPlan::FlightPlanGenericPhase* pleafData = (ns_FlightPlan::FlightPlanGenericPhase*)pGenericProfile->GetItem(i);
			AddGenericDataToTree(pGenericProfile,pleafData);
		}
	}


	m_treeFltPlan.SetRedraw(TRUE);
}

void CDlgFlightPlan::AddRunwayMarkToTree(int nRunwayID, int nMarkIdx,HTREEITEM hAirRouteItem)
{
	HTREEITEM hMarkingItem;
	std::vector<int> vAirportIds;
	InputAirside::GetAirportList(m_nProjID, vAirportIds);
	for (std::vector<int>::iterator iterAirportID = vAirportIds.begin(); iterAirportID != vAirportIds.end(); ++iterAirportID)
	{
		ALTAirport airport;
		airport.ReadAirport(*iterAirportID);

		std::vector<int> vRunways;
		ALTAirport::GetRunwaysIDs(*iterAirportID, vRunways);
		for (std::vector<int>::iterator iterRunwayIdx = vRunways.begin(); iterRunwayIdx != vRunways.end(); ++iterRunwayIdx)
		{
			Runway runway;
			runway.ReadObject(*iterRunwayIdx);
			if (runway.getID() == nRunwayID)
			{
				if (nMarkIdx == 0)
				{
					CString strMark = runway.GetMarking1().c_str();
					hMarkingItem = m_treeFltPlan.InsertItem(strMark, hAirRouteItem);
					return;
				}
				if (nMarkIdx == 1)
				{
					CString strMark = runway.GetMarking2().c_str();
					hMarkingItem = m_treeFltPlan.InsertItem(strMark, hAirRouteItem);
					return;
				}
			}
		}
	}
}

void CDlgFlightPlan::AddGenericDataToTree(ns_FlightPlan::FlightPlanGeneric* pGenericProfile,ns_FlightPlan::FlightPlanGenericPhase* pLeafData)
{
	AirsideObjectTreeCtrlItemDataEx aoidDataEx;
	ItemStringSectionColor isscStringColor;
	CString strColorText;

	//state phase 
	CString strStatePhase;
	int nIdx = pGenericProfile->GetIndex(pLeafData);
	if (nIdx == 0)
	{
		strStatePhase.Format(_T("Initial state Phase %s at: "),ns_FlightPlan::Phase_NameString[pLeafData->GetPhase()]);
	}
	else
	{
		strStatePhase.Format(_T("Change %d Phase %s at: "),nIdx,ns_FlightPlan::Phase_NameString[pLeafData->GetPhase()]);
	}
	HTREEITEM hItemState = m_treeFltPlan.InsertItem(strStatePhase, TVI_ROOT);
	aoidDataEx.lSize = sizeof(aoidDataEx);
	aoidDataEx.dwptrItemData = 0;
	aoidDataEx.vrItemStringSectionColorShow.clear();
	isscStringColor.colorSection = RGB(0,0,255); 
	isscStringColor.strSection = ns_FlightPlan::Phase_NameString[pLeafData->GetPhase()];
	aoidDataEx.vrItemStringSectionColorShow.push_back(isscStringColor);
	m_treeFltPlan.SetItemDataEx(hItemState,aoidDataEx);
	TreeNodeData* pNodeData = new TreeNodeData();
	pNodeData->wordData = (DWORD)pLeafData;
	pNodeData->emType = TreeNodeData::State_Phase;
	m_treeFltPlan.SetItemData(hItemState,(DWORD_PTR)pNodeData);
	m_vNodeList.push_back(pNodeData);


	//Distance to runway
	CString strDistToRunway;
	strDistToRunway.Format(_T("Distance to run %.2f (nm)"),pLeafData->GetDistanceToRunway());
	HTREEITEM hDistToRunway = m_treeFltPlan.InsertItem(strDistToRunway,hItemState);
	aoidDataEx.lSize = sizeof(aoidDataEx);
	aoidDataEx.dwptrItemData = 0;
	aoidDataEx.vrItemStringSectionColorShow.clear();
	isscStringColor.colorSection = RGB(0,0,255); 
	strColorText.Format(_T("%.2f"),pLeafData->GetDistanceToRunway()) ;
	isscStringColor.strSection = strColorText;
	aoidDataEx.vrItemStringSectionColorShow.push_back(isscStringColor);
	m_treeFltPlan.SetItemDataEx(hDistToRunway,aoidDataEx);
	
	pNodeData = new TreeNodeData();
	pNodeData->wordData = (DWORD)pLeafData;
	pNodeData->emType = TreeNodeData::Distance_Runway;
	m_vNodeList.push_back(pNodeData);
	m_treeFltPlan.SetItemData(hDistToRunway,(DWORD_PTR)pNodeData);

	//Upper altitude
	CString strUpAlt;
	strUpAlt.Format(_T("Upper altitude %.2f (ft)"),pLeafData->GetUpAltitude());
	HTREEITEM hUpItem = m_treeFltPlan.InsertItem(strUpAlt,hDistToRunway);
	aoidDataEx.lSize = sizeof(aoidDataEx);
	aoidDataEx.dwptrItemData = 0;
	aoidDataEx.vrItemStringSectionColorShow.clear();
	isscStringColor.colorSection = RGB(0,0,255); 
	strColorText.Format(_T("%.2f"),pLeafData->GetUpAltitude()) ;
	isscStringColor.strSection = strColorText;
	aoidDataEx.vrItemStringSectionColorShow.push_back(isscStringColor);
	m_treeFltPlan.SetItemDataEx(hUpItem,aoidDataEx);
	pNodeData = new TreeNodeData();
	pNodeData->wordData = (DWORD)pLeafData;
	pNodeData->emType = TreeNodeData::Upper_Altitude;
	m_vNodeList.push_back(pNodeData);
	m_treeFltPlan.SetItemData(hUpItem,(DWORD_PTR)pNodeData);

	//Down altitude
	CString strDownAlt;
	strDownAlt.Format(_T("Lower altitude %.2f (ft)"),pLeafData->GetDownAltitude());
	HTREEITEM hDownItem = m_treeFltPlan.InsertItem(strDownAlt,hDistToRunway);
	aoidDataEx.lSize = sizeof(aoidDataEx);
	aoidDataEx.dwptrItemData = 0;
	aoidDataEx.vrItemStringSectionColorShow.clear();
	isscStringColor.colorSection = RGB(0,0,255); 
	strColorText.Format(_T("%.2f"),pLeafData->GetDownAltitude()) ;
	isscStringColor.strSection = strColorText;
	aoidDataEx.vrItemStringSectionColorShow.push_back(isscStringColor);
	m_treeFltPlan.SetItemDataEx(hDownItem,aoidDataEx);
	pNodeData = new TreeNodeData();
	pNodeData->wordData = (DWORD)pLeafData;
	pNodeData->emType = TreeNodeData::Lower_Altitude;
	m_vNodeList.push_back(pNodeData);
	m_treeFltPlan.SetItemData(hDownItem,(DWORD_PTR)pNodeData);

	//Altitude seperation
	CString strAltSep;
	strAltSep.Format(_T("Altitude seperation %.2f (ft)"),pLeafData->GetAltitudeSeparate());
	HTREEITEM hAltSep = m_treeFltPlan.InsertItem(strAltSep,hDistToRunway);
	aoidDataEx.lSize = sizeof(aoidDataEx);
	aoidDataEx.dwptrItemData = 0;
	aoidDataEx.vrItemStringSectionColorShow.clear();
	isscStringColor.colorSection = RGB(0,0,255); 
	strColorText.Format(_T("%.2f"),pLeafData->GetAltitudeSeparate()) ;
	isscStringColor.strSection = strColorText;
	aoidDataEx.vrItemStringSectionColorShow.push_back(isscStringColor);
	m_treeFltPlan.SetItemDataEx(hAltSep,aoidDataEx);
	pNodeData = new TreeNodeData();
	pNodeData->wordData = (DWORD)pLeafData;
	pNodeData->emType = TreeNodeData::Altitude_Sep;
	m_vNodeList.push_back(pNodeData);
	m_treeFltPlan.SetItemData(hAltSep,(DWORD_PTR)pNodeData);

	//Lower Speed
	CString strLowerSpeed;
	strLowerSpeed.Format(_T("Lower speed %.2f (kts)"),pLeafData->GetDownSpeed());
	HTREEITEM hLowerSpeed = m_treeFltPlan.InsertItem(strLowerSpeed,hDistToRunway);
	aoidDataEx.lSize = sizeof(aoidDataEx);
	aoidDataEx.dwptrItemData = 0;
	aoidDataEx.vrItemStringSectionColorShow.clear();
	isscStringColor.colorSection = RGB(0,0,255); 
	strColorText.Format(_T("%.2f"),pLeafData->GetDownSpeed()) ;
	isscStringColor.strSection = strColorText;
	aoidDataEx.vrItemStringSectionColorShow.push_back(isscStringColor);
	m_treeFltPlan.SetItemDataEx(hLowerSpeed,aoidDataEx);
	pNodeData = new TreeNodeData();
	pNodeData->wordData = (DWORD)pLeafData;
	pNodeData->emType = TreeNodeData::Lower_Speed;
	m_vNodeList.push_back(pNodeData);
	m_treeFltPlan.SetItemData(hLowerSpeed,(DWORD_PTR)pNodeData);

	//Upper speed
	CString strUpSpeed;
	strUpSpeed.Format(_T("Upper speed %.2f (kts)"),pLeafData->GetUpSpeed());
	HTREEITEM hUpSpeed = m_treeFltPlan.InsertItem(strUpSpeed,hDistToRunway);
	aoidDataEx.lSize = sizeof(aoidDataEx);
	aoidDataEx.dwptrItemData = 0;
	aoidDataEx.vrItemStringSectionColorShow.clear();
	isscStringColor.colorSection = RGB(0,0,255); 
	strColorText.Format(_T("%.2f"),pLeafData->GetUpSpeed()) ;
	isscStringColor.strSection = strColorText;
	aoidDataEx.vrItemStringSectionColorShow.push_back(isscStringColor);
	m_treeFltPlan.SetItemDataEx(hUpSpeed,aoidDataEx);
	pNodeData = new TreeNodeData();
	pNodeData->wordData = (DWORD)pLeafData;
	pNodeData->emType = TreeNodeData::Upper_Speed;
	m_vNodeList.push_back(pNodeData);
	m_treeFltPlan.SetItemData(hUpSpeed,(DWORD_PTR)pNodeData);

	m_treeFltPlan.Expand(hItemState,TVE_EXPAND);

}

HTREEITEM CDlgFlightPlan::AddWayPointToTree(WayPointProperty* wayPointProp, HTREEITEM hAirRouteItem)
{
	AirsideObjectTreeCtrlItemDataEx aoidDataEx;
	ItemStringSectionColor isscStringColor;
	CString strColorText;

	HTREEITEM hItemFix = m_treeFltPlan.InsertItem(wayPointProp->GetWayPoint().getWaypoint().GetObjNameString(), hAirRouteItem, TVI_LAST);

	CString strAlt;
	if(wayPointProp->GetAltitude() < 0 )
		strAlt.Format("Altitude(ft): %s", "No Limit");
	else
		strAlt.Format("Altitude(ft): %.2f", wayPointProp->GetAltitude());
	HTREEITEM hItemAlt = m_treeFltPlan.InsertItem(strAlt, hItemFix);
	aoidDataEx.lSize = sizeof(aoidDataEx);
	aoidDataEx.dwptrItemData = 0;
	aoidDataEx.vrItemStringSectionColorShow.clear();
	isscStringColor.colorSection = RGB(0,0,255); 
	if (wayPointProp->GetAltitude() < 0)
	{
		strColorText = _T("No Limit");
	}
	else
	{	
		strColorText.Format(_T("%.2f"),wayPointProp->GetAltitude()) ;
	}
	
	isscStringColor.strSection = strColorText;
	aoidDataEx.vrItemStringSectionColorShow.push_back(isscStringColor);
	m_treeFltPlan.SetItemDataEx(hItemAlt,aoidDataEx);

	CString strSpd;
	if(wayPointProp->GetSpeed()< 0) 
		strSpd.Format("Speed(nm/h): %s", "No Limit");
	else 
		strSpd.Format("Speed(nm/h): %.2f", wayPointProp->GetSpeed());

	HTREEITEM hItemSpd = m_treeFltPlan.InsertItem(strSpd, hItemFix);
	aoidDataEx.lSize = sizeof(aoidDataEx);
	aoidDataEx.dwptrItemData = 0;
	aoidDataEx.vrItemStringSectionColorShow.clear();
	isscStringColor.colorSection = RGB(0,0,255); 
	if (wayPointProp->GetSpeed() < 0)
	{
		strColorText = _T("No Limit");
	}
	else
	{	
		strColorText.Format(_T("%.2f"),wayPointProp->GetSpeed()) ;
	}

	isscStringColor.strSection = strColorText;
	aoidDataEx.vrItemStringSectionColorShow.push_back(isscStringColor);
	m_treeFltPlan.SetItemDataEx(hItemSpd,aoidDataEx);
	
	CString strPhase = wayPointProp->GetPhaseNameString();
	

	HTREEITEM  hItemPhase= m_treeFltPlan.InsertItem(strPhase,hItemFix);
	aoidDataEx.lSize = sizeof(aoidDataEx);
	aoidDataEx.dwptrItemData = 0;
	aoidDataEx.vrItemStringSectionColorShow.clear();
	isscStringColor.colorSection = RGB(0,0,255); 
	isscStringColor.strSection = strPhase;
	aoidDataEx.vrItemStringSectionColorShow.push_back(isscStringColor);
	m_treeFltPlan.SetItemDataEx(hItemPhase,aoidDataEx);

	TreeRouteData *pData = new TreeRouteData(hItemFix, hItemAlt,hItemSpd, hItemPhase,wayPointProp);

	m_treeFltPlan.SetItemData(hItemFix,(DWORD_PTR)pData);
	m_treeFltPlan.SetItemData(hItemAlt,(DWORD_PTR)pData);
	m_treeFltPlan.SetItemData(hItemSpd,(DWORD_PTR)pData);
	m_treeFltPlan.SetItemData(hItemPhase,(DWORD_PTR)pData);

	//m_treeFltPlan.Expand(hItemFix,TVE_EXPAND);
	return hItemFix;
}

void CDlgFlightPlan::OnOperationTypeChanged()
{
	int nCurSel = m_comboOperationType.GetCurSel();
	if (nCurSel == LB_ERR)
		return;

	if (m_pCurFltPlan == NULL)
		return;

	int nOperation = 0;
	if (m_pCurFltPlan->GetProcfileType() == ns_FlightPlan::FlightPlanBase::Specific_Profile)
	{
		if (nCurSel%2 != 0)
		{
			nOperation = (nCurSel - 1)/2;
			m_pCurFltPlan->SetOperationType(FlightPlanSpecific::OperationType(nOperation));
		}
		else
		{
			nOperation = nCurSel / 2;
			ns_FlightPlan::FlightPlanBase* pGenericProperty = new FlightPlanGeneric(m_nProjID);
			pGenericProperty->SetFlightType(m_pCurFltPlan->GetFlightType());
			m_flightPlans.AddFlightPlan(pGenericProperty);
			m_flightPlans.DeleteFlightPlan((FlightPlanSpecific*)m_pCurFltPlan);
			m_pCurFltPlan = pGenericProperty;
			m_pCurFltPlan->SetOperationType(FlightPlanSpecific::OperationType(nOperation));
			UpdateCurrentPlanContent();
		}
		
	}
	else
	{
		if (nCurSel%2 == 0)
		{
			nOperation = nCurSel / 2;
			m_pCurFltPlan->SetOperationType(FlightPlanSpecific::OperationType(nOperation));
		}
		else
		{
			nOperation = (nCurSel - 1)/2;
			ns_FlightPlan::FlightPlanBase* fltPlan = new FlightPlanSpecific(m_nProjID);
			fltPlan->SetFlightType(m_pCurFltPlan->GetFlightType());
			m_flightPlans.AddFlightPlan(fltPlan);
			m_flightPlans.DeleteFlightPlan(m_pCurFltPlan);
			m_pCurFltPlan = fltPlan;
			m_pCurFltPlan->SetOperationType(FlightPlanSpecific::OperationType(nOperation));
			UpdateCurrentPlanContent();
		}
	}
}

LRESULT CDlgFlightPlan::OnEndEditTreeValue(WPARAM wParam, LPARAM lParam)
{
	AirsideObjectTreeCtrlItemDataEx* pAoidDataEx = m_treeFltPlan.GetItemDataEx(m_hRClickItem);
	if (pAoidDataEx == NULL)
		return 0;
	
	CString strValue = *(CString*)lParam;
	double dValue = atof(strValue);

	if (m_pCurFltPlan->GetProcfileType() == FlightPlanGeneric::Specific_Profile)
	{
		TreeRouteData* pItemData = reinterpret_cast<TreeRouteData*>(m_treeFltPlan.GetItemData(m_hRClickItem));
		if (GetTreeItemDepthIndex(m_treeFltPlan, m_hRClickItem) == 2
			&& GetTreeItemWidthIndex(m_treeFltPlan, m_hRClickItem) == 0) // altitude item
		{
			CString strAlt;
			//const ARWaypoint& wayPoint = pItemData->m_pWayPointProp->GetWayPoint();
			//
			//if(wayPoint.getMinHeight() >=0 && wayPoint.getMinHeight() > dValue)
			//{
			//	CString strMsg;
			//	strMsg.Format(_T("The height value setted is out of range: min %d"),wayPoint.getMinHeight());
			//	MessageBox(strMsg);
			//	dValue = wayPoint.getMinHeight();
			//}
			//else if(wayPoint.getMaxHeight() >=0 && wayPoint.getMaxHeight() < dValue) 
			//{	
			//	CString strMsg;
			//	strMsg.Format(_T("The height value setted is out of range: max %d"),wayPoint.getMaxHeight());

			//	MessageBox(strMsg);
			//	dValue = wayPoint.getMaxHeight();
			//}
		
			pItemData->m_pWayPointProp->SetAltitude(dValue);
			

			if (dValue < 0)
			{	
				strAlt.Format("Altitude(ft): %s ", "No limit");
			}
			else
			{
				strAlt.Format("Altitude(ft): %.2f ", dValue);
			}
			CString strColorText;
			if(dValue < 0)
			{
				strColorText = _T("No limit");
			}
			else
			{
				strColorText.Format(_T("%.2f"),dValue);
			}
			ItemStringSectionColor isscStringColor;
			isscStringColor.colorSection = RGB(0,0,255); 
			isscStringColor.strSection = strColorText;
			pAoidDataEx->vrItemStringSectionColorShow.clear();
			pAoidDataEx->vrItemStringSectionColorShow.push_back(isscStringColor);
			m_treeFltPlan.SetItemText(m_hRClickItem, strAlt);
		}
		else if (GetTreeItemDepthIndex(m_treeFltPlan, m_hRClickItem) == 2
			&& GetTreeItemWidthIndex(m_treeFltPlan, m_hRClickItem) == 1) // speed item
		{
			//const ARWaypoint& wayPoint = pItemData->m_pWayPointProp->GetWayPoint();
			//if(wayPoint.getMinSpeed() >=0 && wayPoint.getMinSpeed() < dValue ) 
			//{	
			//	CString strMsg;
			//	strMsg.Format(_T("The speed value setted is out of range: min %d"),wayPoint.getMinSpeed());
			//	MessageBox(strMsg);

			//	dValue = wayPoint.getMinSpeed();
			//}
			//else if(wayPoint.getMaxSpeed() >=0 && wayPoint.getMaxSpeed() > dValue )
			//{	
			//	CString strMsg;
			//	strMsg.Format(_T("The speed value setted is out of range: max %d"),wayPoint.getMaxSpeed());
			//	MessageBox(strMsg);
			//	dValue = wayPoint.getMaxSpeed();
			//}
		
			pItemData->m_pWayPointProp->SetSpeed(dValue);
			
			
			CString strSpd;

			if(dValue < 0) 
				strSpd.Format("Speed(nm/h): %s", "No Limit");
			else
				strSpd.Format("Speed(nm/h): %.2f", dValue);
			
			CString strColorText;
			if(dValue < 0)
			{
				strColorText = _T("No limit");
			}
			else
			{
				strColorText.Format(_T("%.2f"),dValue);
			}
			ItemStringSectionColor isscStringColor;
			isscStringColor.colorSection = RGB(0,0,255); 
			isscStringColor.strSection = strColorText;
			pAoidDataEx->vrItemStringSectionColorShow.clear();
			pAoidDataEx->vrItemStringSectionColorShow.push_back(isscStringColor);
			m_treeFltPlan.SetItemText(m_hRClickItem, strSpd);
		}
	}
	else
	{
		TreeNodeData* pNodeData = (TreeNodeData*)m_treeFltPlan.GetItemData(m_hRClickItem);
		ASSERT(pNodeData);
		if (pNodeData == NULL)
			return 0;

		FlightPlanGenericPhase* pDataLeaf = (FlightPlanGenericPhase*)pNodeData->wordData;
		switch (pNodeData->emType)
		{
		case TreeNodeData::Distance_Runway:
			{
				CString strValue;
				strValue.Format(_T("Distance to run %.2f (nm)"),dValue);
				pDataLeaf->SetDistancetoRunway(dValue);
				m_treeFltPlan.SetItemText(m_hRClickItem, strValue);
			}
			break;
		case TreeNodeData::Upper_Altitude:
			{
				CString strValue;
				strValue.Format(_T("Upper altitude %.2f (ft)"),dValue);
				pDataLeaf->SetUpAltitude(dValue);
				m_treeFltPlan.SetItemText(m_hRClickItem, strValue);
			}
			break;
		case TreeNodeData::Lower_Altitude:
			{
				CString strValue;;
				strValue.Format(_T("Lower altitude %.2f (ft)"),dValue);
				pDataLeaf->SetDownAltitude(dValue);
				m_treeFltPlan.SetItemText(m_hRClickItem, strValue);
			}
			break;
		case TreeNodeData::Altitude_Sep:
			{
				CString strValue;
				strValue.Format(_T("Altitude seperation %.2f (ft)"),dValue);
				pDataLeaf->SetAltitudeSeparate(dValue);
				m_treeFltPlan.SetItemText(m_hRClickItem, strValue);
			}
			break;
		case TreeNodeData::Lower_Speed:
			{
				CString strValue;
				strValue.Format(_T("Lower speed %.2f (kts)"),dValue);
				pDataLeaf->SetDownSpeed(dValue);
				m_treeFltPlan.SetItemText(m_hRClickItem, strValue);
			}
			break;
		case TreeNodeData::Upper_Speed:
			{
				CString strValue;
				strValue.Format(_T("Upper speed %.2f (kts)"),dValue);
				pDataLeaf->SetUpSpeed(dValue);
				m_treeFltPlan.SetItemText(m_hRClickItem, strValue);
			}
			break;
		default:
			return 0;
		}
		CString strColorText;
		strColorText.Format(_T("%.2f"),dValue);
		ItemStringSectionColor isscStringColor;
		isscStringColor.colorSection = RGB(0,0,255); 
		isscStringColor.strSection = strColorText;
		pAoidDataEx->vrItemStringSectionColorShow.clear();
		pAoidDataEx->vrItemStringSectionColorShow.push_back(isscStringColor);
	}
	


	return 0;
}
LRESULT CDlgFlightPlan::OnEndEditComboBoxTreeValue(WPARAM wParam, LPARAM lParam)
{

	AirsideObjectTreeCtrlItemDataEx* pAoidDataEx = m_treeFltPlan.GetItemDataEx(m_hRClickItem);
	if (pAoidDataEx == NULL)
		return 0;
	if (m_pCurFltPlan->GetProcfileType() == FlightPlanGeneric::Specific_Profile)
	{
		if (GetTreeItemDepthIndex(m_treeFltPlan, m_hRClickItem) == 2
			&& GetTreeItemWidthIndex(m_treeFltPlan, m_hRClickItem) == 2)
		{
			TreeRouteData* pItemData = reinterpret_cast<TreeRouteData*>(m_treeFltPlan.GetItemData(m_hRClickItem));

			int nSelItem = m_treeFltPlan.m_comboBox.GetCurSel();
			if(nSelItem != -1)
			{
				pItemData->m_pWayPointProp->SetPhase(WayPointProperty::Phase(nSelItem));
				ItemStringSectionColor isscStringColor;
				isscStringColor.colorSection = RGB(0,0,255); 
				isscStringColor.strSection = pItemData->m_pWayPointProp->GetPhaseNameString();
				pAoidDataEx->vrItemStringSectionColorShow.clear();
				pAoidDataEx->vrItemStringSectionColorShow.push_back(isscStringColor);
				m_treeFltPlan.SetItemText(m_hRClickItem,pItemData->m_pWayPointProp->GetPhaseNameString());
			}


		}
	}
	else
	{
		TreeNodeData* pNodeData = (TreeNodeData*)m_treeFltPlan.GetItemData(m_hRClickItem);
		ASSERT(pNodeData);
		if (pNodeData == NULL)
			return 0;

		FlightPlanGenericPhase* pDataLeaf = (FlightPlanGenericPhase*)pNodeData->wordData;
		ASSERT(m_pCurFltPlan);
		FlightPlanGeneric* pGenericProperty = (FlightPlanGeneric*)m_pCurFltPlan;
		if (pNodeData->emType != TreeNodeData::State_Phase)
			return 0;
		int nSelItem = m_treeFltPlan.m_comboBox.GetCurSel();
		if(nSelItem != -1)
		{
			pDataLeaf->SetPhase(FlightPlanGenericPhase::Phase(nSelItem));
			CString strStatePhase;
			int nIdx = pGenericProperty->GetIndex(pDataLeaf);
			if (nIdx == 0)
			{
				strStatePhase.Format(_T("Initial state Phase %s at: "),ns_FlightPlan::Phase_NameString[nSelItem]);
			}
			else
			{
				strStatePhase.Format(_T("Change %d Phase %s at: "),nIdx,ns_FlightPlan::Phase_NameString[nSelItem]);
			}
			ItemStringSectionColor isscStringColor;
			isscStringColor.colorSection = RGB(0,0,255); 
			isscStringColor.strSection = ns_FlightPlan::Phase_NameString[nSelItem];
			pAoidDataEx->vrItemStringSectionColorShow.clear();
			pAoidDataEx->vrItemStringSectionColorShow.push_back(isscStringColor);
			m_treeFltPlan.SetItemText(m_hRClickItem,strStatePhase);
		}
		
	}
	
	return 0;
}
void CDlgFlightPlan::OnNewFlightPlan()
{
	FlightConstraint fltType;
	CFlightDialog flightDlg(NULL, true);
	if (IDOK != flightDlg.DoModal())
		return;

	fltType = flightDlg.GetFlightSelection();

	CString strFltType;
	fltType.screenPrint(strFltType, 0, 128);
	CString strFullFltType;
	fltType.WriteSyntaxStringWithVersion(strFullFltType.GetBuffer(1024));

	FlightPlanGeneric *pGenericProfile = new FlightPlanGeneric(m_nProjID);
	pGenericProfile->SetFlightType(strFullFltType);
	if(fltType.GetFltConstraintMode()==ENUM_FLTCNSTR_MODE_ARR)
	{
		pGenericProfile->SetOperationType(FlightPlanBase::landing);
		pGenericProfile->FillLandingValue();		
	}
	else if(fltType.GetFltConstraintMode() == ENUM_FLTCNSTR_MODE_DEP)
	{
		pGenericProfile->SetOperationType(FlightPlanBase::takeoff);
		pGenericProfile->FillTakeoffValue();		
	}
	else
	{
		pGenericProfile->SetOperationType(FlightPlanBase::enroute);
		pGenericProfile->FillDefaultValue();		
	}
	
	m_flightPlans.AddFlightPlan(pGenericProfile);
	int nIndex = m_flightPlans.GetIndex(pGenericProfile);
	ASSERT(nIndex != LB_ERR);
	if (nIndex != LB_ERR)
	{

		m_lstFltType.InsertString(nIndex,strFltType);
		m_pCurFltPlan = pGenericProfile;
		m_lstFltType.SetCurSel(nIndex);
		UpdateCurrentPlanContent();
	}
}

void CDlgFlightPlan::OnEditPlan()
{
	int nSel =  m_lstFltType.GetCurSel();
	if (nSel == LB_ERR)
		return;

	FlightConstraint fltType;
	CFlightDialog flightDlg(NULL);
	if(IDOK == flightDlg.DoModal())
		fltType = flightDlg.GetFlightSelection();
	else
		return;
	CString strFltType;
	fltType.screenFltPrint(strFltType);
	CString strFullFltType;
	fltType.WriteSyntaxStringWithVersion(strFullFltType.GetBuffer(1024));

	m_lstFltType.DeleteString(nSel);
	m_lstFltType.InsertString(nSel,strFltType);
	//data
	m_lstFltType.SetCurSel(nSel);
	m_pCurFltPlan->SetFlightType(strFullFltType);
}

void CDlgFlightPlan::OnDelFlightPlan()
{
	int nSel =  m_lstFltType.GetCurSel();
	if (nSel == LB_ERR)
		return;

	m_lstFltType.DeleteString(nSel);
	m_flightPlans.DeleteFlightPlan(m_pCurFltPlan);

	m_flightPlans.DeleteFlightPlan(m_pCurFltPlan);
	m_pCurFltPlan = NULL;
	
	//set new current flight plan
	if(m_lstFltType.GetCount() > 0)
	{
		m_lstFltType.SetCurSel(0);
		//m_pCurFltPlan = m_flightPlans.GetItem(0);
		m_pCurFltPlan = m_flightPlans.GetFlightPlan(0);
	}

	UpdateCurrentPlanContent();
}

void CDlgFlightPlan::OnNewAirRoute()
{
	if (m_pCurFltPlan->GetProcfileType() == FlightPlanGeneric::Specific_Profile)
	{
		FlightPlanSpecific* pFlightPlan = (FlightPlanSpecific*)m_pCurFltPlan;

		CDlgSelectAirRoute dlg(m_nProjID,pFlightPlan);
		if (IDOK != dlg.DoModal())
			return;

		int nAirRouteID = dlg.GetSelectedAirRoute();
		CAirRoute airRoute;
		airRoute.ReadData(nAirRouteID);

		AirRouteProperty* airRouteProp = new AirRouteProperty(airRoute, pFlightPlan->GetID());
		pFlightPlan->AddItem(airRouteProp);	
	}
	else
	{
		FlightPlanGenericPhase* pDateLeaf = new FlightPlanGenericPhase();
		FlightPlanGeneric* pGenericProperty = (FlightPlanGeneric*)m_pCurFltPlan;
		pGenericProperty->AddItem(pDateLeaf);
	}	

	UpdateCurrentPlanContent();
}

void CDlgFlightPlan::OnDelAirRoute()
{
	HTREEITEM hSelItem = m_treeFltPlan.GetSelectedItem();
	if (hSelItem == NULL)
		return;

	if (m_pCurFltPlan->GetProcfileType() == FlightPlanGeneric::Specific_Profile)
	{
		FlightPlanSpecific* pFlightPlan = (FlightPlanSpecific*)m_pCurFltPlan;
		
		if (GetTreeItemDepthIndex(m_treeFltPlan, hSelItem) != 0)
			return;

		int nIndex = GetTreeItemWidthIndex(m_treeFltPlan, hSelItem);
		pFlightPlan->DeleteItem(nIndex);
		m_treeFltPlan.DeleteItem(hSelItem);
	}
	else
	{
		FlightPlanGeneric* pGenericProperty = (FlightPlanGeneric*)m_pCurFltPlan;
		if (pGenericProperty == NULL)
			return;
		
		TreeNodeData* pNodeDate = (TreeNodeData*)m_treeFltPlan.GetItemData(hSelItem);
		if (pNodeDate == NULL || pNodeDate->emType != TreeNodeData::State_Phase)
			return;
		
		FlightPlanGenericPhase* pDataLeaf = (FlightPlanGenericPhase*)pNodeDate->wordData;
		ASSERT(pDataLeaf);
		if (pDataLeaf == NULL)
			return;
		
		pGenericProperty->DeleteItem(pDataLeaf);
		m_treeFltPlan.DeleteItem(hSelItem);	
		UpdateCurrentPlanContent();
	}
	
}

void CDlgFlightPlan::OnLbnSelChangeListFltType()
{
	int nSel =  m_lstFltType.GetCurSel();
	if (nSel == LB_ERR)
		return;

	//FlightPlan* pFltPlan = m_flightPlans.GetItem(nSel);
	FlightPlanBase* pFltPlan = m_flightPlans.GetFlightPlan(nSel);
	if (pFltPlan != m_pCurFltPlan)
	{
		m_pCurFltPlan = pFltPlan;
		UpdateCurrentPlanContent();
	}
}

void CDlgFlightPlan::OnContextMenu(CWnd* pWnd, CPoint point)
{
	m_hRClickItem = NULL;

	CRect rectTree;
	m_treeFltPlan.GetWindowRect( &rectTree );
	if( !rectTree.PtInRect(point) ) 
		return;

	m_treeFltPlan.SetFocus();
	CPoint pt = point;
	m_treeFltPlan.ScreenToClient( &pt );
	UINT iRet;
	m_hRClickItem = m_treeFltPlan.HitTest( pt, &iRet );
	if (iRet != TVHT_ONITEMLABEL)
	{
		m_hRClickItem = NULL;
		return;
	}

	CMenu menuPopup;
	CMenu* pSubMenu = NULL;
	menuPopup.LoadMenu( IDR_MENU_AIRSIDE_FLTPLAN );
	if (m_pCurFltPlan->GetProcfileType() == FlightPlanGeneric::Specific_Profile)
	{
		if (GetTreeItemDepthIndex(m_treeFltPlan, m_hRClickItem) == 2
			&& GetTreeItemWidthIndex(m_treeFltPlan, m_hRClickItem) == 0) // altitude item
		{
			pSubMenu = menuPopup.GetSubMenu(0);
		}
		else if (GetTreeItemDepthIndex(m_treeFltPlan, m_hRClickItem) == 2
				&& GetTreeItemWidthIndex(m_treeFltPlan, m_hRClickItem) == 1) // speed item
		{
			pSubMenu = menuPopup.GetSubMenu(1);
		}
		else if (GetTreeItemDepthIndex(m_treeFltPlan, m_hRClickItem) == 2
				&& GetTreeItemWidthIndex(m_treeFltPlan, m_hRClickItem) == 2) // phase item
		{
			pSubMenu = menuPopup.GetSubMenu(2);
		}
		else
		{
			return;
		}
	}
	else
	{
		TreeNodeData* pNodeData = (TreeNodeData*)m_treeFltPlan.GetItemData(m_hRClickItem);
		ASSERT(pNodeData);
		if (pNodeData)
		{
			pSubMenu = menuPopup.GetSubMenu(3);
		}
	}
	if (pSubMenu != NULL)
		pSubMenu->TrackPopupMenu(TPM_LEFTALIGN, point.x, point.y, this);

}

void CDlgFlightPlan::OnFltplanModifyaltitude()
{
	if (m_hRClickItem == NULL)
		return;

	TreeRouteData* pItemData = reinterpret_cast<TreeRouteData*>(m_treeFltPlan.GetItemData(m_hRClickItem));

	CString strValue;
	strValue.Format("%.2f", pItemData->m_pWayPointProp->GetAltitude());

	m_treeFltPlan.StringEditLabel(m_hRClickItem, strValue);
}

void CDlgFlightPlan::OnFltplanModifyspeed()
{
	if (m_hRClickItem == NULL)
		return;

	TreeRouteData* pItemData = reinterpret_cast<TreeRouteData*>(m_treeFltPlan.GetItemData(m_hRClickItem));

	CString strValue;
	strValue.Format("%.2f", pItemData->m_pWayPointProp->GetSpeed());

	m_treeFltPlan.StringEditLabel(m_hRClickItem, strValue);
}
void CDlgFlightPlan::OnFltplanModifyPhase()
{
	if (m_hRClickItem == NULL)
		return;
	std::vector<CString> vPhase;
	int i = WayPointProperty::Phase_cruise;
	for (;i != WayPointProperty::Phase_Count; ++ i)
	{
		vPhase.push_back(Phase_NameString[i]);
	}

	m_treeFltPlan.SetComboString(m_hRClickItem,vPhase);

}
void CDlgFlightPlan::OnSave()
{
	try
	{
		CADODatabase::BeginTransaction() ;
		m_flightPlans.Save();
		CADODatabase::CommitTransaction();
	}
	catch (CADOException& e)
	{
		AfxMessageBox(_T("Save flight plan error:  +")+e.ErrorMessage());
		CADODatabase::RollBackTransation() ;
		return;
	}

}

void CDlgFlightPlan::OnOK()
{
	OnSave();
	CXTResizeDialog::OnOK();
}

void CDlgFlightPlan::OnCancel()
{
	CXTResizeDialog::OnCancel();
}

void CDlgFlightPlan::OnFltModifyValue()
{
	TreeNodeData* pNodeData = (TreeNodeData*)m_treeFltPlan.GetItemData(m_hRClickItem);
	ASSERT(pNodeData);
	if (pNodeData == NULL)
		return;

	FlightPlanGenericPhase* pDataLeaf = (FlightPlanGenericPhase*)pNodeData->wordData;
	switch (pNodeData->emType)
	{
	case TreeNodeData::State_Phase:
		{
			std::vector<CString> vPhase;
			int i = WayPointProperty::Phase_cruise;
			for (;i != WayPointProperty::Phase_Count; ++ i)
			{
				vPhase.push_back(Phase_NameString[i]);
			}

			m_treeFltPlan.SetComboString(m_hRClickItem,vPhase);
		}
		break;
	case TreeNodeData::Distance_Runway:
		{
			CString strValue;
			strValue.Format("%.2f", pDataLeaf->GetDistanceToRunway());

			m_treeFltPlan.StringEditLabel(m_hRClickItem, strValue);
		}
		break;
	case TreeNodeData::Upper_Altitude:
		{
			CString strValue;
			strValue.Format("%.2f", pDataLeaf->GetUpAltitude());

			m_treeFltPlan.StringEditLabel(m_hRClickItem, strValue);
		}
		break;
	case TreeNodeData::Lower_Altitude:
		{
			CString strValue;
			strValue.Format("%.2f", pDataLeaf->GetDownAltitude());

			m_treeFltPlan.StringEditLabel(m_hRClickItem, strValue);
		}
		break;
	case TreeNodeData::Altitude_Sep:
		{
			CString strValue;
			strValue.Format("%.2f", pDataLeaf->GetAltitudeSeparate());

			m_treeFltPlan.StringEditLabel(m_hRClickItem, strValue);
		}
		break;
	case TreeNodeData::Lower_Speed:
		{
			CString strValue;
			strValue.Format("%.2f", pDataLeaf->GetDownSpeed());

			m_treeFltPlan.StringEditLabel(m_hRClickItem, strValue);
		}
		break;
	case TreeNodeData::Upper_Speed:
		{
			CString strValue;
			strValue.Format("%.2f", pDataLeaf->GetUpSpeed());

			m_treeFltPlan.StringEditLabel(m_hRClickItem, strValue);
		}
		break;
	default:
		break;
	}
}
