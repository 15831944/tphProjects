#ifndef __DLGVEHICLETAGS_H__
#define __DLGVEHICLETAGS_H__

#pragma once

// DlgPaxTags.h : header file
//
#include "resource.h"
#include "../InputAirside/VehicleTagItem.h"
#include "../InputAirside/VehicleTagNode.h"
#include "VehicleTagListCtrl.h"
#include "EditableListBox.h"
/////////////////////////////////////////////////////////////////////////////
// CDlgAircraftTags dialog

class CTermPlanDoc;
class CDlgVehicleTags : public CXTResizeDialog
{
	// Construction
public:
	CDlgVehicleTags(CWnd* pParent = NULL);   // standard constructor

	// Dialog Data
	//{{AFX_DATA(CDlgAircraftTags)
	enum { IDD = IDD_DIALOG_VEHICLETAGS };
	CButton	m_btnTagProperties;
	CButton	m_btnPaxTypes;
	CButton	m_btnCancel;
	CButton	m_btnOk;
	CStatic	m_toolbarcontenter2;
	CEditableListBox	m_lstSets;
	CButton	m_btnBarFrame2;
	CButton	m_btnBarFrame;
	CVehicleTagListCtrl	m_lstFlightTypes;
	CButton	m_btnSave;
	CStatic	m_toolbarcontenter;
	BOOL	m_bShowTag;
	BOOL	m_bChkVehicleID;	
	BOOL	m_bChkPool;
	BOOL	m_bChkServiceFlight;
	BOOL	m_bChkServiceStand;
	BOOL	m_bChkServiceCountLeft;
	//}}AFX_DATA


	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgAircraftTags)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	// Implementation
protected:

	CToolBar m_ToolBar;
	CToolBar m_ToolBar2;

	std::vector<CVehicleTagItem *> m_vrTagItemDeleted;
	std::vector<CVehicleTagNode *> m_vrTagNodeDeleted;
	void FlushDeletedItem(void);//if Cancel , then don't invoke this function.

	int m_nSelectedSet;

	BOOL m_bNewSet;

	CTermPlanDoc* GetDocument() const;

	void EnableAllCheckboxes(BOOL bEnable = TRUE);
	void EnableSubCheckboxes(BOOL bEnable = TRUE);
	void SetAllCheckboxes();
	void InitToolbar();

	void UpdateSetList();
	void UpdateTagData();
	void UpdateVehicleTagIn3D();
	void SaveCurrentData();
	void OnChkItem(DWORD dwMask, BOOL bIsCheck);

	// Generated message map functions
	//{{AFX_MSG(CDlgAircraftTags)
	virtual BOOL OnInitDialog();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnToolbarbuttonadd();
	afx_msg void OnToolbarbuttondel();
	afx_msg void OnGetDispInfo(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnListItemClick( void);
	afx_msg void OnChkShowtag();
	afx_msg void OnChkVehicleId();
	afx_msg void OnChkPool();
	afx_msg void OnChkServiceFlight();
	afx_msg void OnChkServiceStand();
	afx_msg void OnChkServiceCountLeft();

	//afx_msg void OnChkTime();
	//afx_msg void OnChkHeading();
	//afx_msg void OnChkDelay();
	virtual void OnOK();
	afx_msg void OnSave();
	virtual void OnCancel();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	afx_msg void OnItemChangedListSets(void);
	afx_msg void OnAddSet();
	afx_msg void OnDeleteSet();
	afx_msg void OnEndLabelEditListSets(void);
	afx_msg LRESULT OnEditedListBox( WPARAM, LPARAM );
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif __DLGVEHICLETAGS_H__