#if !defined(AFX_DLGWALKTHROUGH_H__E9FA9584_AC15_40AD_9C51_6B981EF6C43D__INCLUDED_)
#define AFX_DLGWALKTHROUGH_H__E9FA9584_AC15_40AD_9C51_6B981EF6C43D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgWalkthrough.h : header file
//


class CTermPlanDoc;
class C3DView;
class CMovie;

/////////////////////////////////////////////////////////////////////////////
// CDlgWalkthrough dialog

class CDlgWalkthrough : public CXTResizeDialog
{
// Construction
public:
	CDlgWalkthrough(CTermPlanDoc* pTPDoc, C3DView* pView, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgWalkthrough)
	enum { IDD = IDD_DIALOG_WALKTHROUGH };
	CEdit	m_editPax;
	CListCtrl	m_lstWalkthrough;
	CComboBox	m_cmbViewpoint;
	COleDateTime	m_endTime;
	COleDateTime	m_startTime;
	//}}AFX_DATA

	void SelectPAX(UINT nPaxID, const CString& sDesc);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgWalkthrough)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:
	BOOL m_bNewMovie;
	BOOL m_bInitialized;
	CToolBar m_ToolBar;
	CTermPlanDoc* m_pDoc;
	C3DView* m_pView;
	int m_nSelectedPaxWalkthrough; // terminal
	int m_nSelectedFlightWalkthrough; // airside
    BOOL m_bIsSetData;
	UINT m_nSelectedPaxID;
	CWnd* m_pParent;
	long m_nMinTime;
	long m_nMaxTime;

	void UpdateList();
	void UpdatePAXData();
	void EnableControls(BOOL bEnable);
	void SetDataFromID(UINT nPaxID);
	DECLARE_MESSAGE_MAP()

	virtual BOOL OnInitDialog();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSelchangeComboViewpoint();
	
	afx_msg void OnDatetimepickerChangeStart(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDatetimepickerChangeEnd(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnItemChanged(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnAddMovie();
	afx_msg void OnDeleteMovie();
	afx_msg void OnRecordMovie();
	afx_msg void OnPreviewMovie();
	afx_msg void OnGetDispInfo(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnEndLabelEdit(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg BOOL OnToolTipText(UINT, NMHDR* pNMHDR, LRESULT* pResult);

	
	virtual void OnOK();
	virtual void OnCancel();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGWALKTHROUGH_H__E9FA9584_AC15_40AD_9C51_6B981EF6C43D__INCLUDED_)
