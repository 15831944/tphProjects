#pragma once
#include "..\MFCExControl\ListCtrlEx.h"

#include "..\InputAirside\VehicleSpecifications.h"
#include "..\Engine\terminal.h"
#include "TermPlanDoc.h"
#include "../Landside/InputLandside.h"
// CDlgVehicleLandSideSpecification dialog

class CDlgVehicleLandSideSpecification : public CXTResizeDialog
{
	DECLARE_DYNAMIC(CDlgVehicleLandSideSpecification)

public:
	CDlgVehicleLandSideSpecification(CWnd* pParent = NULL);   // standard constructor
	CDlgVehicleLandSideSpecification(int nProjID,Terminal *VehicleTerminal,CTermPlanDoc *tmpDoc,CWnd *pParent);
	virtual ~CDlgVehicleLandSideSpecification();

// Dialog Data
	enum { IDD = IDD_DIALOG_SPECIFICATIONVEHICLE };

protected:
	void InitToolBar();
	void InitListCtrl();
	void SetListContent();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	afx_msg void OnNewItem();
	afx_msg void OnDelItem();
	afx_msg void OnEditItem();
	int GetSelectedListItem();
	void UpdateToolBar();
	DECLARE_MESSAGE_MAP()
public:
	CListCtrlEx m_lstCtrl;
	Terminal *m_pTerminal;
	int m_nProjID;
	CToolBar m_wndToolBar;
public:
	virtual BOOL OnInitDialog();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnLvnItemchangedListContents(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnEndlabeleditListContents(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg LRESULT OnChangeVehicleType( WPARAM wParam, LPARAM lParam);
	void OnListItemChanged(int nItem, int nSubItem);
protected:
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnBnClickedSave();
private:
	CTermPlanDoc *pDoc;
	BOOL m_isDelete;
	InputLandside *m_pLandSide;
};
