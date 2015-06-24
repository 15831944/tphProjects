#if !defined(AFX_DLGPAXTAGS_H__E7F1D88C_BEA2_4B7E_8D0D_6219208BA854__INCLUDED_)
#define AFX_DLGPAXTAGS_H__E7F1D88C_BEA2_4B7E_8D0D_6219208BA854__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgPaxTags.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgPaxTags dialog

class CDlgPaxTags : public CDialog
{
// Construction
public:
	CDlgPaxTags(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgPaxTags)
	enum { IDD = IDD_DIALOG_PAXTAGS };
	CButton	m_btnTagProperties;
	CButton	m_btnPaxTypes;
	CButton	m_btnCancel;
	CButton	m_btnOk;
	CStatic	m_toolbarcontenter2;
	CListCtrl	m_lstPTSets;
	CButton	m_btnBarFrame2;
	CButton	m_btnBarFrame;
	CListCtrl	m_lstPaxTypes;
	CButton	m_btnSave;
	CStatic	m_toolbarcontenter;
	BOOL	m_bShowTag;
	BOOL	m_bChkBagCount;
	BOOL	m_bChkCartCount;
	BOOL	m_bChkFlightID;
	BOOL	m_bChkGate;
	BOOL	m_bChkGateTime;
	BOOL	m_bChkPaxID;
	BOOL	m_bChkPaxType;
	BOOL	m_bChkProcessor;
	BOOL	m_bChkState;
	BOOL	m_bChkWalkingSpeed;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgPaxTags)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CTermPlanDoc* GetDocument() const;
	void UpdatePaxTagPropIn3D() const;

	CToolBar m_ToolBar;
	CToolBar m_ToolBar2;

	int m_nSelectedPTSet;

	BOOL m_bNewPTSet;

	void EnableAllCheckboxes(BOOL bEnable = TRUE);
	void EnableSubCheckboxes(BOOL bEnable = TRUE);
	void SetAllCheckboxes();
	void InitToolbar();

	void UpdatePTSetList();
	void UpdatePaxTagData();

	// Generated message map functions
	//{{AFX_MSG(CDlgPaxTags)
	virtual BOOL OnInitDialog();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnToolbarbuttonadd();
	afx_msg void OnToolbarbuttondel();
	afx_msg void OnGetDispInfo(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnListItemClick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnChkShowtag();
	afx_msg void OnChkBagCount();
	afx_msg void OnChkCartCount();
	afx_msg void OnChkFlightId();
	afx_msg void OnChkGate();
	afx_msg void OnChkGateTime();
	afx_msg void OnChkPaxId();
	afx_msg void OnChkPaxType();
	afx_msg void OnChkProcessor();
	afx_msg void OnChkState();
	virtual void OnOK();
	afx_msg void OnSave();
	virtual void OnCancel();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnChkWalkingspeed();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	afx_msg void OnItemChangedListPTSets(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnAddPTSet();
	afx_msg void OnDeletePTSet();
	afx_msg void OnEndLabelEditListPTSets(NMHDR* pNMHDR, LRESULT* pResult);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGPAXTAGS_H__E7F1D88C_BEA2_4B7E_8D0D_6219208BA854__INCLUDED_)
