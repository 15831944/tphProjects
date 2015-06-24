#pragma once
#include "resource.h"
#include "EditableListBox.h"
#include "VehicleTagListCtrl.h"
#include "../Landside/LandsideVehicleTags.h"
class CTermPlanDoc;
// CDlgLandsideVehicleTags dialog

class CDlgLandsideVehicleTags : public CXTResizeDialog
{
	DECLARE_DYNAMIC(CDlgLandsideVehicleTags)

public:
	CDlgLandsideVehicleTags(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgLandsideVehicleTags();

// Dialog Data
	enum { IDD = IDD_DIALOG_LANDSIDEVEHICLETAGS };
	CButton	m_btnTagProperties;
	CButton	m_btnPaxTypes;
	CButton	m_btnCancel;
	CButton	m_btnOk;
	CStatic	m_toolbarcontenter2;
	CEditableListBox	m_lstSets;
	CButton	m_btnBarFrame2;
	CButton	m_btnBarFrame;
	CLandsideVehicleTypeListCtrl	m_lstFlightTypes;
	CButton	m_btnSave;
	CStatic	m_toolbarcontenter;
	BOOL	m_bShowTag;
	BOOL	m_bChkMbID;	
	BOOL	m_bChkFltID;
	BOOL	m_bChkMbType;
	BOOL	m_bChkState;
	BOOL	m_bChkPos;
	BOOL	m_bChkPlace;
	BOOL	m_bChkSpeed;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	CToolBar m_ToolBar;
	CToolBar m_ToolBar2;
	CLandsideVehicleTagSetList m_landsideVehicleTags;
	std::vector<CLandsideVehicleTypes *> m_vrTagItemDeleted;
	std::vector<CLandsideVehicleTagSet *> m_vrTagNodeDeleted;
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

	virtual BOOL OnInitDialog();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnToolbarbuttonadd();
	afx_msg void OnToolbarbuttondel();
	afx_msg void OnGetDispInfo(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnListItemClick( void);
	afx_msg void OnChkShowtag();
	afx_msg void OnChkMobileID();
	afx_msg void OnChkFlightID();
	afx_msg void OnChkMobileType();
	afx_msg void OnChkState();
	afx_msg void OnChkPostion();
	afx_msg void OnChkPlace();
	afx_msg void OnChkSpeed();


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
