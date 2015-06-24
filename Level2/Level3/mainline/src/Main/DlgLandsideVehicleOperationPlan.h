#pragma once
#include "Landside/ResidentVehiclTypeContainer.h"
#include "DlgBaseResident.h"
// CDlgLandsideVehicleOperationPlan dialog

class CDlgLandsideVehicleOperationPlan : public CDlgBaseResident
{
	DECLARE_DYNAMIC(CDlgLandsideVehicleOperationPlan)

public:
	CDlgLandsideVehicleOperationPlan(InputTerminal* pInTerm, InputLandside* pInputLandside,CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgLandsideVehicleOperationPlan();

protected:
	virtual void OnOK();
	virtual ResidentVehicleRoute CreateVehicleRoute(HTREEITEM hItem);
	void FillPlanTreeContent();
	void InsertPlanTreeItem(ResidentRelatedVehicleTypePlan* pVehiclePlan);

	void InitListCtrlHeader();
	void FllListCtrlContent();
	void InsertListCtrlItem(ResidenRelatedtVehicleTypeData* pResidentVehicle);

	virtual void FillFacilityTreeContent();
	virtual void InsertVehicleRouteItem(HTREEITEM hItem,ResidentVehicleRoute* pVehicleRoute,ResidentVehicleRouteFlow* pVehicleRouteFlow);
	virtual CString GetTitle()const;
	DECLARE_MESSAGE_MAP()

	afx_msg void OnNewVehicleType();
	afx_msg void OnDeleteVehicleType();
	afx_msg void OnNewVehiclePlan();
	afx_msg void OnDeleteVehilcePlan();
	afx_msg void OnLvnEndlabeleditListContents( NMHDR *pNMHDR, LRESULT *pResult );
	
	afx_msg LRESULT OnDbClickListCtrl( WPARAM wParam, LPARAM lParam );
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnBtnSave();
	LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);

protected:
	ResidentVehicleRouteFlow* GetVehicleRouteFlow(HTREEITEM hItem);
	virtual int PopChildMenu();
	virtual void ReadInputData();
private:
	ResidentRelatedVehiclTypeContainer* m_residentVehicleContainer;
	HTREEITEM m_hHomeBaseItem;
	HTREEITEM m_hServiceStartItem;
};
