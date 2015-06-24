// DlgLandsideVehicleOperationPlan.cpp : implementation file
//

#include "stdafx.h"
#include "TermPlan.h"
#include "DlgLandsideVehicleOperationPlan.h"
#include "Landside/LandsideResidentVehilceRoute.h"
#include "Inputs/IN_TERM.H"
#include "Landside/InputLandside.h"
#include "Landside/LandsideLayoutObject.h"
#include "Common/ProbDistManager.h"
#include "Common/ProbDistEntry.h"
#include "DlgSelectLandsideVehicleType.h"
#include "Common/WinMsg.h"
#include "DlgProbDist.h"
#include "DlgSelectLandsideObject.h"
#include "DlgTimeRange.h"
#include ".\dlglandsidevehicleoperationplan.h"


// CDlgLandsideVehicleOperationPlan dialog
IMPLEMENT_DYNAMIC(CDlgLandsideVehicleOperationPlan, CDlgBaseResident)
CDlgLandsideVehicleOperationPlan::CDlgLandsideVehicleOperationPlan(InputTerminal* pInTerm, InputLandside* pInputLandside,CWnd* pParent /*=NULL*/)
	: CDlgBaseResident(pInTerm,pInputLandside,pParent)
{
	m_residentVehicleContainer = NULL;
}

CDlgLandsideVehicleOperationPlan::~CDlgLandsideVehicleOperationPlan()
{
	if (m_residentVehicleContainer)
	{
		delete m_residentVehicleContainer;
	}
}

BEGIN_MESSAGE_MAP(CDlgLandsideVehicleOperationPlan, CDlgBaseResident)
	ON_COMMAND(ID_VEHICLE_NEW,OnNewVehicleType)
	ON_COMMAND(ID_VEHICLE_DEL,OnDeleteVehicleType)
	ON_COMMAND(ID_RESIDENT_ADD,OnNewVehiclePlan)
	ON_COMMAND(ID_RESIDENT_DEL,OnDeleteVehilcePlan)
	ON_NOTIFY(LVN_ENDLABELEDIT, IDC_LIST_DATA, OnLvnEndlabeleditListContents)
	ON_MESSAGE(WM_NOEDIT_DBCLICK,OnDbClickListCtrl)
	ON_BN_CLICKED(IDC_BUTTON_SAVE,OnBtnSave)
END_MESSAGE_MAP()


void CDlgLandsideVehicleOperationPlan::InitListCtrlHeader()
{
	CDlgBaseResident::InitListCtrlHeader();

	//Distribution string list
	CStringList strList;
	CProbDistManager* pProbDistMan = 0;	
	CProbDistEntry* pPDEntry = 0;
	int nCount = -1;
	if(m_pInTerm)
		pProbDistMan = m_pInTerm->m_pAirportDB->getProbDistMan();
	strList.AddTail(_T("New Probability Distribution..."));
	if(pProbDistMan)
		nCount = pProbDistMan->getCount();
	for( int i=0; i< nCount; i++ )
	{
		pPDEntry = pProbDistMan->getItem( i );
		if(pPDEntry)
			strList.AddTail(pPDEntry->m_csName);
	}

	//Index
	LV_COLUMNEX	lvc;
	lvc.csList = &strList;	
	lvc.mask = LVCF_WIDTH | LVCF_TEXT;
	lvc.fmt = LVCFMT_NOEDIT;
	lvc.pszText = _T("Index");
	lvc.cx = 40;
	m_wndListCtrl.InsertColumn(0,&lvc);

	//Type
	lvc.mask = LVCF_WIDTH | LVCF_TEXT;
	lvc.fmt = LVCFMT_NOEDIT;
	lvc.pszText = _T("Type");
	lvc.cx = 110;
	m_wndListCtrl.InsertColumn(1,&lvc);
	
	//Home base
	lvc.mask = LVCF_WIDTH | LVCF_TEXT;
	lvc.fmt = LVCFMT_NOEDIT;
	lvc.pszText = _T("Home base");
	lvc.cx = 100;
	m_wndListCtrl.InsertColumn(2,&lvc);

	//Number
	lvc.mask = LVCF_WIDTH | LVCF_TEXT;
	lvc.fmt = LVCFMT_EDIT;
	lvc.pszText = _T("Number");
	lvc.cx = 60;
	m_wndListCtrl.InsertColumn(3,&lvc);


	//T/A Distribution
	lvc.mask = LVCF_WIDTH | LVCF_TEXT;
	lvc.fmt = LVCFMT_DROPDOWN;
	lvc.pszText = _T("T/A Distribution (mins)");
	lvc.cx = 130;
	m_wndListCtrl.InsertColumn(4,&lvc);
}

void CDlgLandsideVehicleOperationPlan::FllListCtrlContent()
{
	for (int i = 0; i < m_residentVehicleContainer->GetItemCount(); i++)
	{
		ResidenRelatedtVehicleTypeData* pResidentVehicle = (ResidenRelatedtVehicleTypeData*)m_residentVehicleContainer->GetItem(i);
		InsertListCtrlItem(pResidentVehicle);
	}

	if (m_wndListCtrl.GetItemCount() > 0)
	{
		m_wndListCtrl.SetCurSel(0);
		m_wndListCtrl.SetItemState(0, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
	}
}

void CDlgLandsideVehicleOperationPlan::InsertListCtrlItem(ResidenRelatedtVehicleTypeData* pResidentVehicle )
{
	int idx = m_wndListCtrl.GetItemCount();
	LandsideFacilityLayoutObjectList& layoutObjectList = m_pInputLandside->getObjectList();

	CString strIndex;
	strIndex.Format(_T("%d"),idx+1);
	m_wndListCtrl.InsertItem(idx,strIndex);

	//type
	m_wndListCtrl.SetItemText(idx,1,pResidentVehicle->GetVehicleType());

	//Homebase
	CString strHomeBase;
	LandsideFacilityObject* pHomebaseObject= pResidentVehicle->GetHomeBase();
	if (pHomebaseObject->GetFacilityID() != -1)
	{
		LandsideFacilityLayoutObject* pLandsideObject = layoutObjectList.getObjectByID(pHomebaseObject->GetFacilityID());
		if (pLandsideObject)
		{
			strHomeBase = pLandsideObject->getName().GetIDString();
		}
	}
	
	m_wndListCtrl.SetItemText(idx,2,strHomeBase);

	//Num
	CString strNum;
	strNum.Format(_T("%d"),pResidentVehicle->GetNum());
	m_wndListCtrl.SetItemText(idx,3,strNum);

	//Distribution
	m_wndListCtrl.SetItemText(idx,4, pResidentVehicle->GetProDist().getDistName());
	m_wndListCtrl.SetItemData(idx,(DWORD)pResidentVehicle);
	m_wndListCtrl.SetItemState(idx,LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
}

void CDlgLandsideVehicleOperationPlan::FillPlanTreeContent()
{
	CDlgBaseResident::FillPlanTreeContent();

	int nSel = m_wndListCtrl.GetCurSel();
	if (nSel == LB_ERR)
		return;
	
	ResidenRelatedtVehicleTypeData* pVehicleTypeData = (ResidenRelatedtVehicleTypeData*)m_wndListCtrl.GetItemData(nSel);
	if (pVehicleTypeData == NULL)
		return;
	
	for (int i = 0; i < pVehicleTypeData->GetCount(); i++)
	{
		ResidentRelatedVehicleTypePlan* pVehicleTypePlan = (ResidentRelatedVehicleTypePlan*)pVehicleTypeData->GetItem(i);
		InsertPlanTreeItem(pVehicleTypePlan);
	}
}

void CDlgLandsideVehicleOperationPlan::InsertPlanTreeItem( ResidentRelatedVehicleTypePlan* pVehiclePlan )
{
	m_wndPlanCtrl.SetImageList(m_wndPlanCtrl.GetImageList(TVSIL_NORMAL),TVSIL_NORMAL);
	LandsideFacilityLayoutObjectList& layoutObjectList = m_pInputLandside->getObjectList();
	COOLTREE_NODE_INFO cni;
	CCoolTree::InitNodeInfo(this,cni);
	
	//Time Range
	
	CString strTimeRange;
	strTimeRange.Format(_T("Time Window: %s"),pVehiclePlan->GetTimeRange().ToString());
	cni.nt = NT_NORMAL;
	cni.net = NET_SHOW_DIALOGBOX;
	HTREEITEM hTimeRangeItem = m_wndPlanCtrl.InsertItem(strTimeRange,cni,FALSE);
	m_wndPlanCtrl.SetItemData(hTimeRangeItem,(DWORD)pVehiclePlan);
	m_vTimeRange.push_back(hTimeRangeItem);
	{
		//service start
		LandsideFacilityObject* pServiceStartObject = pVehiclePlan->GetServiceStartObject();
		if (pServiceStartObject == NULL)
			return;
		
		CString strServiceStart(_T("Service Start:"));
		int nFacilityID = pServiceStartObject->GetFacilityID();
		if (nFacilityID != -1)
		{
			LandsideFacilityLayoutObject* pLandsideObject = layoutObjectList.getObjectByID(nFacilityID);
			if (pLandsideObject)
			{
				strServiceStart += pLandsideObject->getName().GetIDString();
			}
		}
		
		cni.nt = NT_NORMAL;
		cni.net = NET_SHOW_DIALOGBOX;
		HTREEITEM hServiceStartItem = m_wndPlanCtrl.InsertItem(strServiceStart,cni,FALSE,FALSE,hTimeRangeItem);
		m_wndPlanCtrl.SetItemData(hServiceStartItem,(DWORD)pVehiclePlan);

		//number deploy
		cni.nt = NT_NORMAL;
		cni.net = NET_EDIT_INT;
		cni.fMinValue = static_cast<float>(pVehiclePlan->GetNumDeploy());
		CString strNum;
		strNum.Format(_T("Number Deployed: %d"),pVehiclePlan->GetNumDeploy());
		HTREEITEM hNumItem = m_wndPlanCtrl.InsertItem(strNum,cni,FALSE,FALSE,hTimeRangeItem);
		m_wndPlanCtrl.SetItemData(hNumItem,(DWORD)pVehiclePlan);

		//headway
		cni.nt = NT_NORMAL;
		cni.net = NET_DATETIMEPICKER;
		CString strHeadway;
		strHeadway.Format(_T("Headway: %02d:%02d (hh:mm)"),pVehiclePlan->GetHeadwayTime().GetHour(),pVehiclePlan->GetHeadwayTime().GetMinute());
		HTREEITEM hHeadwayItem = m_wndPlanCtrl.InsertItem(strHeadway,cni,FALSE,FALSE,hTimeRangeItem);
		m_wndPlanCtrl.SetItemData(hHeadwayItem,(DWORD)pVehiclePlan->GetHeadwayTime().asSeconds());

		//route
		cni.nt = NT_NORMAL;
		cni.net = NET_NORMAL;
		HTREEITEM hRouteItem = m_wndPlanCtrl.InsertItem(_T("Route"),cni,FALSE,FALSE,hTimeRangeItem);
		ResidentVehicleRouteFlow* pVehicleRouteFlow = pVehiclePlan->GetVehicleRouteFlow();
		m_vRoute.push_back(hRouteItem);
		{
			ResidentVehicleRouteList* pStartRouteList = pVehiclePlan->GetStartRoute();
			m_wndPlanCtrl.SetItemData(hRouteItem,(DWORD)&m_startRoute);
			if (pStartRouteList)
			{
				for (int i = 0; i < pStartRouteList->GetDestCount(); i++)
				{
					ResidentVehicleRoute* pVehicleRoute = pStartRouteList->GetDestObjectInfo(i);
					InsertVehicleRouteItem(hRouteItem,pVehicleRoute,pVehicleRouteFlow);
				}
			}
		}
	}
	m_wndPlanCtrl.Expand(hTimeRangeItem,TVE_EXPAND);
}


void CDlgLandsideVehicleOperationPlan::OnNewVehicleType()
{
	CDlgSelectLandsideVehicleType dlg;
	if(dlg.DoModal() == IDOK)
	{
		CString strGroupName = dlg.GetName();
		if (!strGroupName.IsEmpty())//non empty
		{
			ResidenRelatedtVehicleTypeData* pResidentTypeData = new ResidenRelatedtVehicleTypeData;
			pResidentTypeData->SetVehicleType(strGroupName);
			m_residentVehicleContainer->AddNewItem(pResidentTypeData);
			InsertListCtrlItem(pResidentTypeData);
			GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
		}
	}
}

void CDlgLandsideVehicleOperationPlan::OnDeleteVehicleType()
{
	int nSel = m_wndListCtrl.GetCurSel();
	if (nSel ==LB_ERR)
		return;
	
	ResidenRelatedtVehicleTypeData* pResidentVehicle = (ResidenRelatedtVehicleTypeData*)m_wndListCtrl.GetItemData(nSel);
	if (pResidentVehicle == NULL)
		return;
	
	m_residentVehicleContainer->DeleteItem(pResidentVehicle);
	m_wndListCtrl.DeleteItem(nSel);
	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
}

void CDlgLandsideVehicleOperationPlan::OnNewVehiclePlan()
{
	int nSel = m_wndListCtrl.GetCurSel();
	if (nSel == LB_ERR)
		return;

	ResidenRelatedtVehicleTypeData* pVehicleTypeData = (ResidenRelatedtVehicleTypeData*)m_wndListCtrl.GetItemData(nSel);
	if (pVehicleTypeData == NULL)
		return;

	TimeRange timeRange;
	CDlgTimeRange dlg(timeRange.GetStartTime(),timeRange.GetEndTime());
	if (dlg.DoModal() == IDOK)
	{
		TimeRange timeRange;
		timeRange.SetStartTime(dlg.GetStartTime());
		timeRange.SetEndTime(dlg.GetEndTime());
		if (pVehicleTypeData->CheckTimeRange(timeRange))
		{
			MessageBox(_T("Time range conflict with exist item.!!"),_T("Warning"),MB_OK);
			return;
		}
		ResidentRelatedVehicleTypePlan* pVehicleTypePlan = new ResidentRelatedVehicleTypePlan();
		pVehicleTypePlan->InitStartRoute();
		pVehicleTypePlan->SetHomeBaseObject(pVehicleTypeData->GetHomeBase());
		pVehicleTypePlan->SetTimeRange(timeRange.GetStartTime(),timeRange.GetEndTime());
		pVehicleTypeData->AddItem(pVehicleTypePlan);
		InsertPlanTreeItem(pVehicleTypePlan);
		GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
	}
}

void CDlgLandsideVehicleOperationPlan::OnDeleteVehilcePlan()
{
	HTREEITEM hItem = m_wndPlanCtrl.GetSelectedItem();
	if (hItem == NULL)
		return;
	
	std::vector<HTREEITEM>::iterator iter = std::find(m_vTimeRange.begin(),m_vTimeRange.end(),hItem);
	if (iter != m_vTimeRange.end())
	{
		int nSel = m_wndListCtrl.GetCurSel();
		if (nSel == LB_ERR)
			return;

		ResidenRelatedtVehicleTypeData* pVehicleTypeData = (ResidenRelatedtVehicleTypeData*)m_wndListCtrl.GetItemData(nSel);
		if (pVehicleTypeData == NULL)
			return;

		ResidentRelatedVehicleTypePlan* pVehiclePlan = (ResidentRelatedVehicleTypePlan*)m_wndPlanCtrl.GetItemData(hItem);
		m_vTimeRange.erase(iter);
		DeleteRouteItem(hItem);
		pVehicleTypeData->DeleteItem(pVehiclePlan);
		m_wndPlanCtrl.DeleteItem(hItem);
		GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
	}
}


void CDlgLandsideVehicleOperationPlan::OnLvnEndlabeleditListContents( NMHDR *pNMHDR, LRESULT *pResult )
{
	NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);

	int nCurSel = pDispInfo->item.iItem;
	if (nCurSel == LB_ERR)
		return;

	ResidenRelatedtVehicleTypeData* pResidentTypeData = (ResidenRelatedtVehicleTypeData*)m_wndListCtrl.GetItemData(nCurSel);
	if (pResidentTypeData == NULL)
		return;

	if (pDispInfo->item.iSubItem == 3)
	{
		int nNum = atoi(pDispInfo->item.pszText);
		pResidentTypeData->SetNum(nNum);
	}
	else if (pDispInfo->item.iSubItem == 4)
	{
		CString strSel;
		strSel = pDispInfo->item.pszText;
		ProbabilityDistribution* pProbDist = NULL;
		CProbDistManager* pProbDistMan = m_pInTerm->m_pAirportDB->getProbDistMan();
		if (strSel == _T("New Probability Distribution..." ) )
		{
			CProbDistEntry* pPDEntry = NULL;
			CDlgProbDist dlg(m_pInTerm->m_pAirportDB, false);
			if(dlg.DoModal() == IDOK)
			{
				pPDEntry = dlg.GetSelectedPD();
				if(pPDEntry == NULL)
				{
					return;
				}
				pResidentTypeData->SetProDist(pPDEntry);

				m_wndListCtrl.SetItemText(pDispInfo->item.iItem,pDispInfo->item.iSubItem,pPDEntry->m_csName);
			}
		}
		else
		{
			CProbDistEntry* pPDEntry = NULL;

			int nCount = pProbDistMan->getCount();
			for( int i=0; i<nCount; i++ )
			{
				pPDEntry = pProbDistMan->getItem( i );
				if(pPDEntry->m_csName == strSel)
					break;
			}
			pResidentTypeData->SetProDist(pPDEntry);
			m_wndListCtrl.SetItemText(pDispInfo->item.iItem,pDispInfo->item.iSubItem,pPDEntry->m_csName);
		}
	}

	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
	*pResult = 0;
}

LRESULT CDlgLandsideVehicleOperationPlan::OnDbClickListCtrl( WPARAM wParam, LPARAM lParam )
{
	int nCurSel = (int)wParam;
	if (nCurSel == LB_ERR)
		return FALSE;

	ResidenRelatedtVehicleTypeData* pResidentTypeData = (ResidenRelatedtVehicleTypeData*)m_wndListCtrl.GetItemData(nCurSel);
	if (pResidentTypeData == NULL)
		return FALSE;

	int iSubItem = (int)lParam;
	if (iSubItem == 2)
	{
		CDlgSelectLandsideObject dlg(&m_pInputLandside->getObjectList(),this);
		dlg.AddObjType(ALT_LPARKINGLOT);
		if(dlg.DoModal()==IDOK)
		{
			ALTObjectID objectID= dlg.getSelectObject();

			LandsideFacilityLayoutObjectList& layoutObjectList = m_pInputLandside->getObjectList();
			LandsideFacilityLayoutObject* pLandsideObject = layoutObjectList.getObjectByName(objectID);
			if (pLandsideObject == NULL)
			{
				MessageBox(_T("Please select one landside layout object !!"),_T("Warning"),MB_OK);
				return FALSE;
			}
			pResidentTypeData->GetHomeBase()->SetFacilityID(pLandsideObject->getID());
			pResidentTypeData->GetHomeBase()->SetType(LandsideFacilityObject::Landside_HomeBase_Facility);
			m_wndListCtrl.SetItemText(nCurSel,2,objectID.GetIDString());
			GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
		}
		
	}
	else if(iSubItem == 1)
	{
		CDlgSelectLandsideVehicleType dlg;
		if(dlg.DoModal() == IDOK)
		{
			CString strGroupName = dlg.GetName();
			if (!strGroupName.IsEmpty())//non empty
			{
				if (strGroupName.CompareNoCase(pResidentTypeData->GetVehicleType()) != 0)//not the same name as the old name
				{
					pResidentTypeData->SetVehicleType(strGroupName);
					m_wndListCtrl.SetItemText(nCurSel,1,strGroupName);
					GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
					return TRUE;
				}
			}
		}
	}



	return FALSE;
}


LRESULT CDlgLandsideVehicleOperationPlan::DefWindowProc( UINT message, WPARAM wParam, LPARAM lParam )
{
	switch (message)
	{
	case UM_CEW_EDITSPIN_END:
		{
			HTREEITEM hItem = (HTREEITEM)wParam;
			HTREEITEM hParentItem = m_wndPlanCtrl.GetParentItem(hItem);
			CString strValue = *((CString*)lParam);
			COOLTREE_NODE_INFO* pCNI = (COOLTREE_NODE_INFO*)m_wndPlanCtrl.GetItemNodeInfo(hItem);
			if(std::find(m_vTimeRange.begin(),m_vTimeRange.end(),hParentItem) != m_vTimeRange.end())
			{
				ResidentRelatedVehicleTypePlan* pVehicleTypePlan = (ResidentRelatedVehicleTypePlan*)m_wndPlanCtrl.GetItemData(hItem);
				int nNum = atoi(strValue);
				CString strNum;
				strNum.Format(_T("Number Deployed: %d"),nNum);
				pCNI->fMinValue = static_cast<float>(nNum);
				pVehicleTypePlan->SetNumDeploy(nNum);
				m_wndPlanCtrl.SetItemText(hItem,strNum);
				GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
			}
			else
			{
				ResidentVehicleRoute* pVehicleRoute = (ResidentVehicleRoute*)m_wndPlanCtrl.GetItemData(hItem);
				LandsideFacilityLayoutObjectList& layoutObjectList = m_pInputLandside->getObjectList();
				LandsideFacilityObject* pFacilityObect = pVehicleRoute->GetFacilityObject();
				if (pFacilityObect)
				{
					CString strRoute;
					int nPercent = atoi(strValue);
					pVehicleRoute->SetPercent(nPercent);
					if (pFacilityObect->GetType() == LandsideFacilityObject::Landside_HomeBase_Facility )
					{
						strRoute.Format(_T("HomeBase (%d%%)"),pVehicleRoute->GetPercent());
					}
					else if (pFacilityObect->GetType() == LandsideFacilityObject::Landside_ServiceStart_Facility)
					{
						strRoute.Format(_T("ServiceStart (%d%%)"),pVehicleRoute->GetPercent());
					}
					else
					{
						int nFacilityID = pFacilityObect->GetFacilityID();
						LandsideFacilityLayoutObject* pLandsideObject = layoutObjectList.getObjectByID(nFacilityID);
						if (pLandsideObject)
						{
							strRoute.Format(_T("%s: %s ( %d%% )"),pVehicleRoute->GetTypeString(),pLandsideObject->getName().GetIDString(),pVehicleRoute->GetPercent());
						}
					}
					pCNI->fMinValue = static_cast<float>(pVehicleRoute->GetPercent());
					m_wndPlanCtrl.SetItemText(hItem,strRoute);
					GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
				}
			}
		}
		break;
	case UM_CEW_SHOW_DIALOGBOX_BEGIN:
		{
			HTREEITEM hItem=(HTREEITEM)wParam;
			COOLTREE_NODE_INFO* pCNI = (COOLTREE_NODE_INFO*)m_wndPlanCtrl.GetItemNodeInfo(hItem);
			if (std::find(m_vTimeRange.begin(),m_vTimeRange.end(),hItem) != m_vTimeRange.end())
			{
				ResidentRelatedVehicleTypePlan* pVehiclePlan = (ResidentRelatedVehicleTypePlan*)m_wndPlanCtrl.GetItemData(hItem);
				CDlgTimeRange* pDlgTimeRange = new CDlgTimeRange(pVehiclePlan->GetTimeRange().GetStartTime(),pVehiclePlan->GetTimeRange().GetEndTime());
				pCNI->pEditWnd = pDlgTimeRange;
			}
			else
			{
				CDlgSelectLandsideObject* pSelectLandsideObjectDlg = new CDlgSelectLandsideObject(&m_pInputLandside->getObjectList(),this);
				pSelectLandsideObjectDlg->AddObjType(ALT_LEXT_NODE);
				pSelectLandsideObjectDlg->AddObjType(ALT_LINTERSECTION);
				pSelectLandsideObjectDlg->AddObjType(ALT_LPARKINGLOT);
				pSelectLandsideObjectDlg->AddObjType(ALT_LBUSSTATION);
				pSelectLandsideObjectDlg->AddObjType(ALT_LROUNDABOUT);
				pSelectLandsideObjectDlg->AddObjType(ALT_LCURBSIDE);
				pCNI->pEditWnd = pSelectLandsideObjectDlg;
			}
		}
		break;
	case UM_CEW_SHOW_DIALOGBOX_END:
		{
			HTREEITEM hItem=(HTREEITEM)wParam;
			COOLTREE_NODE_INFO* pCNI = (COOLTREE_NODE_INFO*)lParam;
			if (std::find(m_vTimeRange.begin(),m_vTimeRange.end(),hItem) != m_vTimeRange.end())
			{
				ResidentRelatedVehicleTypePlan* pVehicleTypePlan = (ResidentRelatedVehicleTypePlan*)m_wndPlanCtrl.GetItemData(hItem);
				CDlgTimeRange* pDlg = (CDlgTimeRange*)pCNI->pEditWnd;
				pVehicleTypePlan->SetTimeRange(pDlg->GetStartTime(),pDlg->GetEndTime());
				TimeRange timeRange = pVehicleTypePlan->GetTimeRange();
				CString strTimeRange;
				strTimeRange.Format(_T("Time Window: %s"),timeRange.ToString());
				m_wndPlanCtrl.SetItemText(hItem,strTimeRange);
				GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
				
			}
			else
			{
				CDlgSelectLandsideObject* pDlg = (CDlgSelectLandsideObject*)pCNI->pEditWnd;
				ALTObjectID objectID= pDlg->getSelectObject();

				ResidentRelatedVehicleTypePlan* pVehicleTypePlan = (ResidentRelatedVehicleTypePlan*)m_wndPlanCtrl.GetItemData(hItem);
				LandsideFacilityLayoutObjectList& layoutObjectList = m_pInputLandside->getObjectList();
				LandsideFacilityLayoutObject* pLandsideObject = layoutObjectList.getObjectByName(objectID);
				if (pLandsideObject == NULL)
				{
					MessageBox(_T("Please select one landside layout object !!"),_T("Warning"),MB_OK);
				}
				else
				{
					pVehicleTypePlan->GetServiceStartObject()->SetFacilityID(pLandsideObject->getID());
					pVehicleTypePlan->GetServiceStartObject()->SetType(LandsideFacilityObject::Landside_ServiceStart_Facility);

					CString strServiceStart(_T("Service Start:"));
					strServiceStart += pLandsideObject->getName().GetIDString();
					m_wndPlanCtrl.SetItemText(hItem,strServiceStart);
				}
				GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
			}
		}
		break;
	case UM_CEW_DATETIME_END:
		{
			HTREEITEM hItem=(HTREEITEM)wParam;
			HTREEITEM hParentItem = m_wndPlanCtrl.GetParentItem(hItem);
			ResidentRelatedVehicleTypePlan* pVehicleTypePlan = (ResidentRelatedVehicleTypePlan*)m_wndPlanCtrl.GetItemData(hParentItem);
			long lSecondTime = (long)lParam;
			ElapsedTime estTime(lSecondTime);
			CString strHeadway;
			strHeadway.Format(_T("Headway: %02d:%02d (hh:mm)"),estTime.GetHour(),estTime.GetMinute());
			pVehicleTypePlan->SetHeadwayTime(estTime);
			m_wndPlanCtrl.SetItemText(hItem,strHeadway);
			GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
		}
		break;
	}
	return CDlgBaseResident::DefWindowProc(message,wParam,lParam);
}


void CDlgLandsideVehicleOperationPlan::OnOK()
{
	CString strError;
	if (m_residentVehicleContainer->GetErrorMessage(strError))
	{
		MessageBox(strError,_T("Warning"),MB_OK);
		return;
	}
	try
	{
		CADODatabase::BeginTransaction();
		m_residentVehicleContainer->SaveData(-1);
		CADODatabase::CommitTransaction();
	}
	catch (CADOException& e)
	{
		e.ErrorMessage();
		CADODatabase::RollBackTransation();
		MessageBox(_T("Save database error"),_T("ERROR"),MB_OK);
		return;
	}
	CDlgBaseResident::OnOK();
}

void CDlgLandsideVehicleOperationPlan::OnBtnSave()
{
	CString strError;
	if (m_residentVehicleContainer->GetErrorMessage(strError))
	{
		MessageBox(strError,_T("Warning"),MB_OK);
		return;
	}
	try
	{
		CADODatabase::BeginTransaction();
		m_residentVehicleContainer->SaveData(-1);
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

ResidentVehicleRoute CDlgLandsideVehicleOperationPlan::CreateVehicleRoute(HTREEITEM hItem)
{
	ResidentVehicleRoute vehicleRoute;

	if (m_hItemDragSrc == m_hHomeBaseItem)
	{
		vehicleRoute.GetFacilityObject()->SetType(LandsideFacilityObject::Landside_HomeBase_Facility);
	}
	else if (m_hItemDragSrc == m_hServiceStartItem)
	{	
		vehicleRoute.GetFacilityObject()->SetType(LandsideFacilityObject::Landside_ServiceStart_Facility);
	}
	else
	{
		TreeNodeData* pNodeData = (TreeNodeData*)m_wndFacilityCtrl.GetItemData(hItem);
		LandsideFacilityLayoutObject* pLandsideObject = pNodeData->m_pLandsideObject;

		LandsideFacilityObject::FacilityType emType = LandsideFacilityObject::convertLandObjectType(pLandsideObject->GetType());
		vehicleRoute.GetFacilityObject()->SetFacilityID(pLandsideObject->getID());
		vehicleRoute.GetFacilityObject()->SetType(emType);

	}

	return vehicleRoute;
}

void CDlgLandsideVehicleOperationPlan::FillFacilityTreeContent()
{
	CDlgBaseResident::FillFacilityTreeContent();

	TreeNodeData* pNodeData = new TreeNodeData();
	pNodeData->m_emTye = TreeNodeData::Single_Facility;
	m_hHomeBaseItem = m_wndFacilityCtrl.InsertItem(_T("HomeBase"));
	m_wndFacilityCtrl.SetItemData(m_hHomeBaseItem,(DWORD)pNodeData);
	m_vTreeNodes.push_back(pNodeData);

	pNodeData = new TreeNodeData();
	pNodeData->m_emTye = TreeNodeData::Single_Facility;
	m_hServiceStartItem = m_wndFacilityCtrl.InsertItem(_T("ServiceStart"));
	m_wndFacilityCtrl.SetItemData(m_hServiceStartItem,(DWORD)pNodeData);
	m_vTreeNodes.push_back(pNodeData);
}

void CDlgLandsideVehicleOperationPlan::InsertVehicleRouteItem( HTREEITEM hItem,ResidentVehicleRoute* pVehicleRoute,ResidentVehicleRouteFlow* pVehicleRouteFlow )
{
	COOLTREE_NODE_INFO cni;
	CCoolTree::InitNodeInfo(this,cni);

	bool bHaveCircle = IfHaveCircle(hItem,*pVehicleRoute->GetFacilityObject());
	cni.nt = NT_NORMAL;
	cni.net = NET_EDIT_INT;
	cni.fMinValue = static_cast<float>(pVehicleRoute->GetPercent());
	LandsideFacilityObject::FacilityType emType = pVehicleRoute->GetFacilityObject()->GetType();
	CString strRoute;
	if (emType == LandsideFacilityObject::Landside_HomeBase_Facility )
	{
		cni.clrItem = RGB(255,0,0);
		strRoute.Format(_T("HomeBase (%d%%)"),pVehicleRoute->GetPercent());
	}
	else if (emType == LandsideFacilityObject::Landside_ServiceStart_Facility)
	{
		cni.clrItem = RGB(255,0,0);
		strRoute.Format(_T("ServiceStart (%d%%)"),pVehicleRoute->GetPercent());
	}
	else 
	{
		LandsideFacilityLayoutObjectList& layoutObjectList = m_pInputLandside->getObjectList();
		LandsideFacilityObject* pFacilityObect = pVehicleRoute->GetFacilityObject();
		if (pFacilityObect == NULL)
			return;

		int nFacilityID = pFacilityObect->GetFacilityID();
		LandsideFacilityLayoutObject* pLandsideObject = layoutObjectList.getObjectByID(nFacilityID);
		if (pLandsideObject == NULL)
			return;
		strRoute.Format(_T("%s: %s ( %d%% )"),pVehicleRoute->GetTypeString(),pLandsideObject->getName().GetIDString(),pVehicleRoute->GetPercent());
	}

	HTREEITEM hRouteItem = m_wndPlanCtrl.InsertItem(strRoute,cni,FALSE,FALSE,hItem);
	m_wndPlanCtrl.SetItemData(hRouteItem,(DWORD)pVehicleRoute);
	m_vRoute.push_back(hRouteItem);
	if (bHaveCircle)
	{
		cni.nt = NT_NORMAL;
		cni.net = NET_NORMAL;
		cni.clrItem = RGB(0,0,0);
		HTREEITEM hCircleItem = m_wndPlanCtrl.InsertItem(_T("[ CYCLIC ]"),cni,FALSE,FALSE,hRouteItem);
		m_wndPlanCtrl.SetItemData(hCircleItem,NULL);
		m_wndPlanCtrl.Expand(hRouteItem,TVE_EXPAND);
	}
	else
	{
		ResidentVehicleRouteList* pRouteList = pVehicleRouteFlow->GetDestVehicleRoute(*pVehicleRoute->GetFacilityObject());
		if (pRouteList)
		{
			for (int i = 0; i < pRouteList->GetDestCount(); i++)
			{
				ResidentVehicleRoute* pChildRoute = pRouteList->GetDestObjectInfo(i);
				InsertVehicleRouteItem(hRouteItem,pChildRoute,pVehicleRouteFlow);
			}
		}
	}
	

	m_wndPlanCtrl.Expand(hItem,TVE_EXPAND);
}

CString CDlgLandsideVehicleOperationPlan::GetTitle() const
{
	return _T("Vehicle Operaitons Plan   Resident Vehicles");
}

ResidentVehicleRouteFlow* CDlgLandsideVehicleOperationPlan::GetVehicleRouteFlow(HTREEITEM hItem)
{
	if (std::find(m_vTimeRange.begin(),m_vTimeRange.end(),hItem) != m_vTimeRange.end())
	{
		ResidentRelatedVehicleTypePlan* pTypePlan = (ResidentRelatedVehicleTypePlan*)m_wndPlanCtrl.GetItemData(hItem);
		if (pTypePlan)
		{
			return pTypePlan->GetVehicleRouteFlow();
		}
	}

	return NULL;
}

int CDlgLandsideVehicleOperationPlan::PopChildMenu()
{
	HTREEITEM hItem = m_wndPlanCtrl.GetSelectedItem();
	if (hItem == NULL)
		return -1;
	
	HTREEITEM hParentItem = m_wndPlanCtrl.GetParentItem(hItem);
	if(std::find(m_vTimeRange.begin(),m_vTimeRange.end(),hParentItem) != m_vTimeRange.end())
	{
		return 0;
	}

	HTREEITEM hGrandItem = m_wndPlanCtrl.GetParentItem(hParentItem);
	if(std::find(m_vTimeRange.begin(),m_vTimeRange.end(),hGrandItem) != m_vTimeRange.end())
	{
		return 1;
	}

	return 2;
}

void CDlgLandsideVehicleOperationPlan::ReadInputData()
{
	m_residentVehicleContainer= new ResidentRelatedVehiclTypeContainer();
	m_residentVehicleContainer->ReadData(-1);
}

