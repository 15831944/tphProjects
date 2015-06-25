#pragma once

#include "..\MFCExControl\ListCtrlEx.h"
#include "..\InputAirside\VehicleSpecifications.h"
#include "../MFCExControl/XTResizeDialog.h"

// CDlgVehicleSpecification dialog

class CDlgVehicleSpecification : public CXTResizeDialog
{
	DECLARE_DYNAMIC(CDlgVehicleSpecification)

public:
	CDlgVehicleSpecification(int nProjID,CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgVehicleSpecification();

// Dialog Data
	//enum { IDD = IDD_DIALOG_VEHICLESPECIFICATION };
protected:
	CListCtrlEx m_lstCtrl;
	CToolBar m_wndToolBar;

	int m_nProjID;
private:
	CVehicleSpecifications m_vehicleSpecifications;
protected:

	void InitToolBar();
	void InitListCtrl();
	void SetListContent();

	void EditItemText(size_t i);

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
	afx_msg void OnNewItem();
	afx_msg void OnDelItem();
	afx_msg void OnEditItem();
	int GetSelectedListItem();
	void UpdateToolBar();
public:
	virtual BOOL OnInitDialog();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnLvnItemchangedListContents(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnEndlabeleditListContents(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg LRESULT OnChangeVehicleType( WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnPreEditListCtrlItem( WPARAM wParam, LPARAM lParam);
	void OnListItemChanged(int nItem, int nSubItem);
protected:
	virtual void OnOK();
	afx_msg void OnBnClickedSave();
};
