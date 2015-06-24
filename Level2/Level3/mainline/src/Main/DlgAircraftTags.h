#ifndef __DLGAIRCRAFTTAGS_H__
#define __DLGAIRCRAFTTAGS_H__

#pragma once

// DlgPaxTags.h : header file
//
#include "resource.h"

/////////////////////////////////////////////////////////////////////////////
// CDlgAircraftTags dialog

class CTermPlanDoc;
class CDlgAircraftTags : public CDialog
{
// Construction
public:
	CDlgAircraftTags(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgAircraftTags)
	enum { IDD = IDD_DIALOG_AIRCRAFTTAGS };
	CButton	m_btnTagProperties;
	CButton	m_btnPaxTypes;
	CButton	m_btnCancel;
	CButton	m_btnOk;
	CStatic	m_toolbarcontenter2;
	CListCtrl	m_lstSets;
	CButton	m_btnBarFrame2;
	CButton	m_btnBarFrame;
	CListCtrl	m_lstFlightTypes;
	CButton	m_btnSave;
	CStatic	m_toolbarcontenter;
	BOOL	m_bShowTag;
	BOOL	m_bChkFlightID;	
	BOOL	m_bChkOpMode;
	BOOL	m_bChkSpeed;
	BOOL	m_bChkGate;
	BOOL	m_bChkRunway;
	BOOL	m_bChkSidStar;
	BOOL	m_bChkOrigDest;
	BOOL	m_bChkACType;
	BOOL	m_bChkAltitude;
	BOOL	m_bChkState;

	//BOOL	m_bChkDelay;
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

	int m_nSelectedSet;

	BOOL m_bNewSet;

	CTermPlanDoc* GetDocument() const;
	void EnableAllCheckboxes(BOOL bEnable = TRUE);
	void EnableSubCheckboxes(BOOL bEnable = TRUE);
	void SetAllCheckboxes();
	void InitToolbar();

	void UpdateSetList();
	void UpdateTagData();
	void UpdateAircrafTagIn3D() const;
	void SaveCurrentData();
	void OnChkItem(DWORD dwMask, BOOL bIsCheck);

	// Generated message map functions
	//{{AFX_MSG(CDlgAircraftTags)
	virtual BOOL OnInitDialog();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnToolbarbuttonadd();
	afx_msg void OnToolbarbuttondel();
	afx_msg void OnGetDispInfo(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnListItemClick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnChkShowtag();
	afx_msg void OnChkFlightId();
	afx_msg void OnChkOperationMode();
	afx_msg void OnChkSpeed();
	afx_msg void OnChkGate();
	afx_msg void OnChkRunway();
	afx_msg void OnChkSidStar();
	afx_msg void OnChkOrigDest();
	afx_msg void OnChkACType();
	afx_msg void OnChkAltitude();
	afx_msg void OnChkState();

	//afx_msg void OnChkTime();
	//afx_msg void OnChkHeading();
	//afx_msg void OnChkDelay();
	virtual void OnOK();
	afx_msg void OnSave();
	virtual void OnCancel();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	afx_msg void OnItemChangedListSets(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnAddSet();
	afx_msg void OnDeleteSet();
	afx_msg void OnEndLabelEditListSets(NMHDR* pNMHDR, LRESULT* pResult);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif __DLGAIRCRAFTTAGS_H__