#include "StdAfx.h"
#include "TermPlan.h"
#include "Common/WinMsg.h"
#include ".\dlglandsidenonrelatedplan.h"
#include "Landside/LandsideLayoutObject.h"
#include "DlgSelectLandsideVehicleType.h"
#include "DlgTimeRange.h"
#include "Landside/InputLandside.h"
#include "PassengerTypeDialog.h"

IMPLEMENT_DYNAMIC(CDlgLandsideNonRelatedPlan, CDlgBaseResident)
CDlgLandsideNonRelatedPlan::CDlgLandsideNonRelatedPlan(InputTerminal* pInTerm, InputLandside* pInputLandside,CWnd* pParent /*=NULL*/)
: CDlgBaseResident(pInTerm,pInputLandside,this)
{
	
}

CDlgLandsideNonRelatedPlan::~CDlgLandsideNonRelatedPlan(void)
{
}

BEGIN_MESSAGE_MAP(CDlgLandsideNonRelatedPlan, CDlgBaseResident)
	ON_COMMAND(ID_VEHICLE_NEW,OnNewVehicleType)
	ON_COMMAND(ID_VEHICLE_DEL,OnDeleteVehicleType)
	ON_COMMAND(ID_RESIDENT_ADD,OnNewVehiclePlan)
	ON_COMMAND(ID_RESIDENT_DEL,OnDeleteVehilcePlan)
	ON_NOTIFY(LVN_ENDLABELEDIT, IDC_LIST_DATA, OnLvnEndlabeleditListContents)
	ON_MESSAGE(WM_NOEDIT_DBCLICK,OnDbClickListCtrl)
	ON_BN_CLICKED(IDC_BUTTON_SAVE,OnBtnSave)
END_MESSAGE_MAP()


void CDlgLandsideNonRelatedPlan::InitListCtrlHeader()
{
	CDlgBaseResident::InitListCtrlHeader();

	//Distribution string list
	CStringList strList;

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

	//Type
	lvc.mask = LVCF_WIDTH | LVCF_TEXT;
	lvc.fmt = LVCFMT_NOEDIT;
	lvc.pszText = _T("Pax Type");
	lvc.cx = 300;
	m_wndListCtrl.InsertColumn(2,&lvc);
}

void CDlgLandsideNonRelatedPlan::FllListCtrlContent()
{
	for (int i = 0; i < m_residentVehicleContainer.GetItemCount(); i++)
	{
		NonResidentVehicleTypeData* pResidentVehicle = m_residentVehicleContainer.GetItem(i);
		InsertListCtrlItem(pResidentVehicle);
	}

	if (m_wndListCtrl.GetItemCount() > 0)
	{
		m_wndListCtrl.SetCurSel(0);
		m_wndListCtrl.SetItemState(0, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
	}
}


void CDlgLandsideNonRelatedPlan::InsertListCtrlItem(NonResidentVehicleTypeData* pResidentVehicle )
{
	int idx = m_wndListCtrl.GetItemCount();
	LandsideFacilityLayoutObjectList& layoutObjectList = m_pInputLandside->getObjectList();

	CString strIndex;
	strIndex.Format(_T("%d"),idx+1);
	m_wndListCtrl.InsertItem(idx,strIndex);

	//type
	m_wndListCtrl.SetItemText(idx,1,pResidentVehicle->GetVehicleType());

	//Pax type
	m_wndListCtrl.SetItemText(idx,2,pResidentVehicle->GetPaxType());

	m_wndListCtrl.SetItemData(idx,(DWORD)pResidentVehicle);
	m_wndListCtrl.SetItemState(idx,LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
}

void CDlgLandsideNonRelatedPlan::FillPlanTreeContent()
{
	CDlgBaseResident::FillPlanTreeContent();

	int nSel = m_wndListCtrl.GetCurSel();
	if (nSel == LB_ERR)
		return;

	NonResidentVehicleTypeData* pVehicleTypeData = (NonResidentVehicleTypeData*)m_wndListCtrl.GetItemData(nSel);
	if (pVehicleTypeData == NULL)
		return;

	for (int i = 0; i < pVehicleTypeData->GetCount(); i++)
	{
		NonResidentVehicleTypePlan* pVehicleTypePlan = (NonResidentVehicleTypePlan*)pVehicleTypeData->GetItem(i);
		InsertPlanTreeItem(pVehicleTypePlan);
	}
}

void CDlgLandsideNonRelatedPlan::InsertPlanTreeItem( NonResidentVehicleTypePlan* pVehiclePlan )
{
	m_wndPlanCtrl.SetImageList(m_wndPlanCtrl.GetImageList(TVSIL_NORMAL),TVSIL_NORMAL);
	LandsideFacilityLayoutObjectList& layoutObjectList = m_pInputLandside->getObjectList();
	COOLTREE_NODE_INFO cni;
	CCoolTree::InitNodeInfo(this,cni);

	//Time Range

	CString strTimeRange;
	strTimeRange.Format(_T("Time Window: %s"),pVehiclePlan->GetTimeRange().GetString());
	cni.nt = NT_NORMAL;
	cni.net = NET_SHOW_DIALOGBOX;
	HTREEITEM hTimeRangeItem = m_wndPlanCtrl.InsertItem(strTimeRange,cni,FALSE);
	m_wndPlanCtrl.SetItemData(hTimeRangeItem,(DWORD)pVehiclePlan);
	m_vTimeRange.push_back(hTimeRangeItem);
	{
		//route
		ResidentVehicleRouteList* pRouteList = pVehiclePlan->GetStartRoute();
		cni.nt = NT_NORMAL;
		cni.net = NET_NORMAL;
		HTREEITEM hRouteItem = m_wndPlanCtrl.InsertItem(_T("Route"),cni,FALSE,FALSE,hTimeRangeItem);
		m_wndPlanCtrl.SetItemData(hRouteItem,(DWORD)&m_startRoute);
		m_vRoute.push_back(hRouteItem);
		if (pRouteList)
		{
			for (int i = 0; i < pRouteList->GetDestCount(); i++)
			{
				ResidentVehicleRoute* pChildeRoute = pRouteList->GetDestObjectInfo(i);
				InsertVehicleRouteItem(hRouteItem,pChildeRoute,pVehiclePlan->GetVehicleRouteFlow());
			}
		}
	}
	m_wndPlanCtrl.Expand(hTimeRangeItem,TVE_EXPAND);
}


void CDlgLandsideNonRelatedPlan::OnNewVehicleType()
{
	CDlgSelectLandsideVehicleType dlg;
	if(dlg.DoModal() == IDOK)
	{
		CString strGroupName = dlg.GetName();
// 		if (m_residentVehicleContainer.ExistSameVehicleType(strGroupName))
// 		{
// 			MessageBox(_T("Exist same vehicle type!!."),_T("Warning"),MB_OK);
// 			return;
// 		}

		if (!strGroupName.IsEmpty())//non empty
		{
			NonResidentVehicleTypeData* pResidentTypeData = new NonResidentVehicleTypeData;
			pResidentTypeData->SetVehicleType(strGroupName);
			pResidentTypeData->SetPaxType(CMobileElemConstraint((InputTerminal*)m_pInputLandside->getTerminal()));
			m_residentVehicleContainer.AddNewItem(pResidentTypeData);
			InsertListCtrlItem(pResidentTypeData);
			GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
		}
	}
}

void CDlgLandsideNonRelatedPlan::OnDeleteVehicleType()
{
	int nSel = m_wndListCtrl.GetCurSel();
	if (nSel ==LB_ERR)
		return;

	NonResidentVehicleTypeData* pResidentVehicle = (NonResidentVehicleTypeData*)m_wndListCtrl.GetItemData(nSel);
	if (pResidentVehicle == NULL)
		return;

	m_residentVehicleContainer.DeleteItem(pResidentVehicle);
	m_wndListCtrl.DeleteItem(nSel);
	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
}

void CDlgLandsideNonRelatedPlan::OnNewVehiclePlan()
{
	int nSel = m_wndListCtrl.GetCurSel();
	if (nSel == LB_ERR)
		return;

	NonResidentVehicleTypeData* pVehicleTypeData = (NonResidentVehicleTypeData*)m_wndListCtrl.GetItemData(nSel);
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
		NonResidentVehicleTypePlan* pVehicleTypePlan = new NonResidentVehicleTypePlan();
		pVehicleTypePlan->SetTimeRange(timeRange.GetStartTime(),timeRange.GetEndTime());
		pVehicleTypeData->AddItem(pVehicleTypePlan);
		InsertPlanTreeItem(pVehicleTypePlan);
		GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
	}
}

void CDlgLandsideNonRelatedPlan::OnDeleteVehilcePlan()
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

		NonResidentVehicleTypeData* pVehicleTypeData = (NonResidentVehicleTypeData*)m_wndListCtrl.GetItemData(nSel);
		if (pVehicleTypeData == NULL)
			return;

		NonResidentVehicleTypePlan* pVehiclePlan = (NonResidentVehicleTypePlan*)m_wndPlanCtrl.GetItemData(hItem);
		m_vTimeRange.erase(iter);
		DeleteRouteItem(hItem);
		pVehicleTypeData->DeleteItem(pVehiclePlan);
		m_wndPlanCtrl.DeleteItem(hItem);
		GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
	}
}


void CDlgLandsideNonRelatedPlan::OnLvnEndlabeleditListContents( NMHDR *pNMHDR, LRESULT *pResult )
{
	NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);

	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
	*pResult = 0;
}

LRESULT CDlgLandsideNonRelatedPlan::OnDbClickListCtrl( WPARAM wParam, LPARAM lParam )
{
	int nCurSel = (int)wParam;
	if (nCurSel == LB_ERR)
		return FALSE;

	NonResidentVehicleTypeData* pResidentTypeData = (NonResidentVehicleTypeData*)m_wndListCtrl.GetItemData(nCurSel);
	if (pResidentTypeData == NULL)
		return FALSE;

	int iSubItem = (int)lParam;
	if(iSubItem == 1)
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
	else if (iSubItem == 2)
	{
		CPassengerTypeDialog dlg( this ,FALSE, TRUE );
		if( dlg.DoModal() == IDOK )
		{

			pResidentTypeData->SetPaxType(dlg.GetMobileSelection());
			m_wndListCtrl.SetItemText(nCurSel,2,pResidentTypeData->GetPaxType());
			GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
			return TRUE;
		}
	}


	return FALSE;
}

LRESULT CDlgLandsideNonRelatedPlan::DefWindowProc( UINT message, WPARAM wParam, LPARAM lParam )
{
	switch (message)
	{
	case UM_CEW_EDITSPIN_END:
		{
			HTREEITEM hItem = (HTREEITEM)wParam;
			HTREEITEM hParentItem = m_wndPlanCtrl.GetParentItem(hItem);
			CString strValue = *((CString*)lParam);

			ResidentVehicleRoute* pVehicleRoute = (ResidentVehicleRoute*)m_wndPlanCtrl.GetItemData(hItem);
			COOLTREE_NODE_INFO* pCNI = (COOLTREE_NODE_INFO*)m_wndPlanCtrl.GetItemNodeInfo(hItem);
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
		break;
	case UM_CEW_SHOW_DIALOGBOX_BEGIN:
		{
			HTREEITEM hItem=(HTREEITEM)wParam;
			COOLTREE_NODE_INFO* pCNI = (COOLTREE_NODE_INFO*)m_wndPlanCtrl.GetItemNodeInfo(hItem);
			if (std::find(m_vTimeRange.begin(),m_vTimeRange.end(),hItem) != m_vTimeRange.end())
			{
				NonResidentVehicleTypePlan* pVehiclePlan = (NonResidentVehicleTypePlan*)m_wndPlanCtrl.GetItemData(hItem);
				CDlgTimeRange* pDlgTimeRange = new CDlgTimeRange(pVehiclePlan->GetTimeRange().GetStartTime(),pVehiclePlan->GetTimeRange().GetEndTime());
				pCNI->pEditWnd = pDlgTimeRange;
			}
		}
		break;
	case UM_CEW_SHOW_DIALOGBOX_END:
		{
			HTREEITEM hItem=(HTREEITEM)wParam;
			COOLTREE_NODE_INFO* pCNI = (COOLTREE_NODE_INFO*)lParam;
			if (std::find(m_vTimeRange.begin(),m_vTimeRange.end(),hItem) != m_vTimeRange.end())
			{
				NonResidentVehicleTypePlan* pVehicleTypePlan = (NonResidentVehicleTypePlan*)m_wndPlanCtrl.GetItemData(hItem);
				CDlgTimeRange* pDlg = (CDlgTimeRange*)pCNI->pEditWnd;
				pVehicleTypePlan->SetTimeRange(pDlg->GetStartTime(),pDlg->GetEndTime());
				TimeRange timeRange = pVehicleTypePlan->GetTimeRange();
				CString strTimeRange;
				strTimeRange.Format(_T("Time Window: %s"),timeRange.GetString());
				m_wndPlanCtrl.SetItemText(hItem,strTimeRange);
				GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);

			}
		}
		break;
	}
	return CDlgBaseResident::DefWindowProc(message,wParam,lParam);
}


void CDlgLandsideNonRelatedPlan::OnOK()
{
	CString strError;
	if (m_residentVehicleContainer.GetErrorMessage(strError))
	{
		MessageBox(strError,_T("Warning"),MB_OK);
		return;
	}
	try
	{
		CADODatabase::BeginTransaction();
		m_residentVehicleContainer.SaveData(-1);
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

void CDlgLandsideNonRelatedPlan::OnBtnSave()
{
	CString strError;
	if (m_residentVehicleContainer.GetErrorMessage(strError))
	{
		MessageBox(strError,_T("Warning"),MB_OK);
		return;
	}
	try
	{
		CADODatabase::BeginTransaction();
		m_residentVehicleContainer.SaveData(-1);
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


void CDlgLandsideNonRelatedPlan::InsertVehicleRouteItem( HTREEITEM hItem,ResidentVehicleRoute* pVehicleRoute,ResidentVehicleRouteFlow* pVehicleRouteFlow )
{
	COOLTREE_NODE_INFO cni;
	CCoolTree::InitNodeInfo(this,cni);

	bool bHaveCircle = IfHaveCircle(hItem,*pVehicleRoute->GetFacilityObject());

	cni.nt = NT_NORMAL;
	cni.net = NET_EDIT_INT;
	

	cni.fMinValue = static_cast<float>(pVehicleRoute->GetPercent());
	LandsideFacilityObject::FacilityType emType = pVehicleRoute->GetFacilityObject()->GetType();
	CString strRoute;

	LandsideFacilityLayoutObjectList& layoutObjectList = m_pInputLandside->getObjectList();
	LandsideFacilityObject* pFacilityObect = pVehicleRoute->GetFacilityObject();
	if (pFacilityObect == NULL)
		return;

	int nFacilityID = pFacilityObect->GetFacilityID();
	LandsideFacilityLayoutObject* pLandsideObject = layoutObjectList.getObjectByID(nFacilityID);
	if (pLandsideObject == NULL)
		return;
	strRoute.Format(_T("%s: %s ( %d%% )"),pVehicleRoute->GetTypeString(),pLandsideObject->getName().GetIDString(),pVehicleRoute->GetPercent());

	HTREEITEM hRouteItem = m_wndPlanCtrl.InsertItem(strRoute,cni,FALSE,FALSE,hItem);
	m_wndPlanCtrl.SetItemData(hRouteItem,(DWORD)pVehicleRoute);
	m_vRoute.push_back(hRouteItem);
	if (bHaveCircle)
	{
		cni.nt = NT_NORMAL;
		cni.net = NET_NORMAL;
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

CString CDlgLandsideNonRelatedPlan::GetTitle() const
{
	return _T("Vehicle Operations Plan   No Resident");
}


ResidentVehicleRouteFlow* CDlgLandsideNonRelatedPlan::GetVehicleRouteFlow( HTREEITEM hItem )
{
	if (std::find(m_vTimeRange.begin(),m_vTimeRange.end(),hItem) != m_vTimeRange.end())
	{
		NonResidentVehicleTypePlan* pTypePlan = (NonResidentVehicleTypePlan*)m_wndPlanCtrl.GetItemData(hItem);
		if (pTypePlan)
		{
			return pTypePlan->GetVehicleRouteFlow();
		}
	}
	
	return NULL;
}

int CDlgLandsideNonRelatedPlan::PopChildMenu()
{
	HTREEITEM hItem = m_wndPlanCtrl.GetSelectedItem();
	if (hItem == NULL)
		return -1;

	HTREEITEM hParentItem = m_wndPlanCtrl.GetParentItem(hItem);
	if(std::find(m_vTimeRange.begin(),m_vTimeRange.end(),hParentItem) != m_vTimeRange.end())
	{
		return 1;
	}
	
	return 2;
}

void CDlgLandsideNonRelatedPlan::ReadInputData()
{
	m_residentVehicleContainer.ReadData(m_pInTerm);
}
