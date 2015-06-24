#include "StdAfx.h"
#include "TermPlan.h"
#include "Common/WinMsg.h"
#include ".\dlgvehicleoperationnonrelated.h"
#include "Landside/LandsideLayoutObject.h"
#include "Landside/InputLandside.h"
#include "DlgTimeRange.h"
#include "Landside/NonPaxVehicleAssignment.h"

IMPLEMENT_DYNAMIC(CDlgVehicleOperationNonRelated, CDlgLandsideNonRelatedPlan)

CDlgVehicleOperationNonRelated::CDlgVehicleOperationNonRelated(InputTerminal* pInTerm, InputLandside* pInputLandside,CWnd* pParent /*=NULL*/)
:CDlgLandsideNonRelatedPlan(pInTerm,pInputLandside,this)
{
	
}

CDlgVehicleOperationNonRelated::~CDlgVehicleOperationNonRelated(void)
{
}

BEGIN_MESSAGE_MAP(CDlgVehicleOperationNonRelated,CDlgLandsideNonRelatedPlan)
	ON_BN_CLICKED(IDC_BUTTON_SAVE,OnBtnSave)
END_MESSAGE_MAP()


void CDlgVehicleOperationNonRelated::OnOK()
{
	try
	{
		CADODatabase::BeginTransaction();
		m_operationVehicleContainer.SaveData(-1);
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

void CDlgVehicleOperationNonRelated::InitListCtrlHeader()
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

	//Demand
	lvc.mask = LVCF_WIDTH | LVCF_TEXT;
	lvc.fmt = LVCFMT_NOEDIT;
	lvc.pszText = _T("Demand");
	lvc.cx = 110;
	m_wndListCtrl.InsertColumn(1,&lvc);
}

void CDlgVehicleOperationNonRelated::FllListCtrlContent()
{
	for (int i = 0; i < m_operationVehicleContainer.GetItemCount(); i++)
	{
		VehicleDemandAndPoolData* pDemandData = m_operationVehicleContainer.GetItem(i);
		InsertListCtrlItem(pDemandData);
	}

	if (m_wndListCtrl.GetItemCount() > 0)
	{
		m_wndListCtrl.SetCurSel(0);
		m_wndListCtrl.SetItemState(0, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
	}

	m_wndVehicleBar.EnableWindow(FALSE);
}

void CDlgVehicleOperationNonRelated::InsertListCtrlItem( VehicleDemandAndPoolData* pDemand )
{
	LandsideFacilityLayoutObjectList& layoutObjectList = m_pInputLandside->getObjectList();

	NonPaxVehicleAssignment* pNonPaxVehicleAssignment = m_pInputLandside->getNonPaxVehicleAssignment();
	if (pNonPaxVehicleAssignment == NULL)
		return;

	int idx = m_wndListCtrl.GetItemCount();
	
	CString strIndex;
	strIndex.Format(_T("%d"),idx+1);
	m_wndListCtrl.InsertItem(idx,strIndex);

	if (pDemand->GetDemandType() == VehicleDemandAndPoolData::Demand_Strategy)
	{
		NonPaxVehicleStrategy* pStrategy = pNonPaxVehicleAssignment->GetByID(pDemand->GetDemandID());
		if (pStrategy)
		{
			m_wndListCtrl.SetItemText(idx,1,pStrategy->GetStrategyName());
		}
	}
	else
	{
		LandsideFacilityLayoutObject* pLandsideObject = layoutObjectList.getObjectByID(pDemand->GetDemandID());
		if (pLandsideObject)
		{
			m_wndListCtrl.SetItemText(idx,1,pLandsideObject->getName().GetIDString());
		}
	}
	m_wndListCtrl.SetItemData(idx,(DWORD)pDemand);
	m_wndListCtrl.SetItemState(idx,LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
}

void CDlgVehicleOperationNonRelated::OnBtnSave()
{
	try
	{
		CADODatabase::BeginTransaction();
		m_operationVehicleContainer.SaveData(-1);
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

void CDlgVehicleOperationNonRelated::FillPlanTreeContent()
{
	CDlgBaseResident::FillPlanTreeContent();

	int nSel = m_wndListCtrl.GetCurSel();
	if (nSel == LB_ERR)
		return;

	VehicleDemandAndPoolData* pDemand = (VehicleDemandAndPoolData*)m_wndListCtrl.GetItemData(nSel);
	if (pDemand == NULL)
		return;

	for (int i = 0; i < pDemand->GetCount(); i++)
	{
		NonResidentVehicleTypePlan* pVehicleTypePlan = (NonResidentVehicleTypePlan*)pDemand->GetItem(i);
		InsertPlanTreeItem(pVehicleTypePlan);
	}
}

CString CDlgVehicleOperationNonRelated::GetTitle() const
{
	return _T("Vehicle Operations Plan Non Pax Related");
}

void CDlgVehicleOperationNonRelated::OnNewVehiclePlan()
{
	int nSel = m_wndListCtrl.GetCurSel();
	if (nSel == LB_ERR)
		return;

	VehicleDemandAndPoolData* pDemand = (VehicleDemandAndPoolData*)m_wndListCtrl.GetItemData(nSel);
	if (pDemand == NULL)
		return;

	TimeRange timeRange;
	CDlgTimeRange dlg(timeRange.GetStartTime(),timeRange.GetEndTime());
	if (dlg.DoModal() == IDOK)
	{
		TimeRange timeRange;
		timeRange.SetStartTime(dlg.GetStartTime());
		timeRange.SetEndTime(dlg.GetEndTime());
		if (pDemand->CheckTimeRange(timeRange))
		{
			MessageBox(_T("Time range conflict with exist item.!!"),_T("Warning"),MB_OK);
			return;
		}
		NonResidentVehicleTypePlan* pVehicleTypePlan = new NonResidentVehicleTypePlan();
		pVehicleTypePlan->SetTimeRange(timeRange.GetStartTime(),timeRange.GetEndTime());
		pVehicleTypePlan->SetResidentType(2);
		pDemand->AddItem(pVehicleTypePlan);
		InsertPlanTreeItem(pVehicleTypePlan);
		GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
	}
}

void CDlgVehicleOperationNonRelated::OnDeleteVehilcePlan()
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

		VehicleDemandAndPoolData* pDemand = (VehicleDemandAndPoolData*)m_wndListCtrl.GetItemData(nSel);
		if (pDemand == NULL)
			return;

		NonResidentVehicleTypePlan* pVehiclePlan = (NonResidentVehicleTypePlan*)m_wndPlanCtrl.GetItemData(hItem);
		m_vTimeRange.erase(iter);
		DeleteRouteItem(hItem);
		pDemand->DeleteItem(pVehiclePlan);
		m_wndPlanCtrl.DeleteItem(hItem);
		GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
	}
}

void CDlgVehicleOperationNonRelated::OnLvnItemchangedList( NMHDR *pNMHDR, LRESULT *pResult )
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

	NM_LISTVIEW* pnmv = (NM_LISTVIEW*)pNMHDR;

	*pResult = 0;

	if((pnmv->uChanged & LVIF_STATE) != LVIF_STATE)
		return;

	if(!(pnmv->uOldState & LVIS_SELECTED) && !(pnmv->uNewState & LVIS_SELECTED))
	{
		return;
	}

	if(pnmv->uOldState & LVIS_SELECTED) 
	{
		if(!(pnmv->uNewState & LVIS_SELECTED)) 
		{
			m_wndPlanBar.GetToolBarCtrl().EnableButton(ID_RESIDENT_ADD,FALSE);
			m_wndPlanBar.GetToolBarCtrl().EnableButton(ID_RESIDENT_DEL,FALSE);
			m_wndPlanBar.GetToolBarCtrl().EnableButton(ID_RESIDENT_PERCENT,FALSE);

			m_wndListCtrl.SetCurSel(-1);
			FillPlanTreeContent();
		}	
	}
	else if(pnmv->uNewState & LVIS_SELECTED) {
		m_wndListCtrl.SetCurSel(pnmv->iItem);
		m_wndPlanBar.GetToolBarCtrl().EnableButton(ID_RESIDENT_ADD,TRUE);
		m_wndPlanBar.GetToolBarCtrl().EnableButton(ID_RESIDENT_PERCENT,FALSE);
		m_wndPlanBar.GetToolBarCtrl().EnableButton(ID_RESIDENT_DEL,FALSE);
		FillPlanTreeContent();
	}
	else
		ASSERT(0);
}

void CDlgVehicleOperationNonRelated::LayoutFigure()
{
	//change list control header title and disable toolbar
	{
		GetDlgItem(IDC_STATIC_TYPE)->SetWindowText(_T("DEMAND AND POOLS"));
		m_wndVehicleBar.GetToolBarCtrl().EnableButton( ID_VEHICLE_NEW, FALSE);
		m_wndVehicleBar.GetToolBarCtrl().EnableButton( ID_VEHICLE_DEL, FALSE);
	}

}

void CDlgVehicleOperationNonRelated::ReadInputData()
{
	m_operationVehicleContainer.ReadData(m_pInputLandside);
}

LRESULT CDlgVehicleOperationNonRelated::OnDbClickListCtrl( WPARAM wParam, LPARAM lParam )
{
	return TRUE;
}
