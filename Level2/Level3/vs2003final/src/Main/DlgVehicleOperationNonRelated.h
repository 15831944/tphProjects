#pragma once
#include "DlgLandsideNonRelatedPlan.h"
#include "../Landside/VehicleOperationNonPaxRelatedContainer.h"

class CDlgVehicleOperationNonRelated : public CDlgLandsideNonRelatedPlan
{
	DECLARE_DYNAMIC(CDlgVehicleOperationNonRelated)
public:
	CDlgVehicleOperationNonRelated(InputTerminal* pInTerm, InputLandside* pInputLandside,CWnd* pParent = NULL);
	~CDlgVehicleOperationNonRelated(void);

protected:
	virtual void OnOK();

	void InitListCtrlHeader();
	void FllListCtrlContent();
	void InsertListCtrlItem(VehicleDemandAndPoolData* pDemand);
	void FillPlanTreeContent();

	virtual CString GetTitle()const;

	afx_msg void OnBtnSave();
	virtual void OnNewVehiclePlan();
	virtual void OnDeleteVehilcePlan();
	virtual void OnLvnItemchangedList( NMHDR *pNMHDR, LRESULT *pResult );
	virtual LRESULT OnDbClickListCtrl( WPARAM wParam, LPARAM lParam );
	DECLARE_MESSAGE_MAP()
	virtual void LayoutFigure();
	virtual void ReadInputData();
private:
	VehicleOperationNonPaxRelatedContainer m_operationVehicleContainer;
};
