#pragma once
#include "afxwin.h"
#include "DlgBaseResident.h"
#include "Landside/LandsideResidentRelatedContainer.h"

class CDlgLandsideNonRelatedPlan : public CDlgBaseResident
{
	DECLARE_DYNAMIC(CDlgLandsideNonRelatedPlan)
public:
	CDlgLandsideNonRelatedPlan(InputTerminal* pInTerm, InputLandside* pInputLandside,CWnd* pParent = NULL);
	~CDlgLandsideNonRelatedPlan(void);

protected:
	virtual void OnOK();

	void FillPlanTreeContent();
	void InsertPlanTreeItem(NonResidentVehicleTypePlan* pVehiclePlan);

	void InitListCtrlHeader();
	void FllListCtrlContent();
	void InsertListCtrlItem(NonResidentVehicleTypeData* pResidentVehicle);

	virtual void InsertVehicleRouteItem(HTREEITEM hItem,ResidentVehicleRoute* pVehicleRoute,ResidentVehicleRouteFlow* pVehicleRouteFlow);
	virtual CString GetTitle()const;
	DECLARE_MESSAGE_MAP()

	afx_msg void OnNewVehicleType();
	afx_msg void OnDeleteVehicleType();
	virtual void OnNewVehiclePlan();
	virtual void OnDeleteVehilcePlan();
	afx_msg void OnLvnEndlabeleditListContents( NMHDR *pNMHDR, LRESULT *pResult );

	virtual LRESULT OnDbClickListCtrl( WPARAM wParam, LPARAM lParam );
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnBtnSave();
	LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);

protected:
	ResidentVehicleRouteFlow* GetVehicleRouteFlow(HTREEITEM hItem);
	virtual int PopChildMenu();
	virtual void ReadInputData();
private:
	NonResidentVehicleTypeContainer m_residentVehicleContainer;

};
