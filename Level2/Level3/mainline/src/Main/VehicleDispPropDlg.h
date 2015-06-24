#if !defined(AFX_PAXDISPPROPDLG_H__2D990C4A_08FF_4F78_826C_6329FEFA25D7__INCLUDED_)
#define AFX_PAXDISPPROPDLG_H__2D990C4A_08FF_4F78_826C_6329FEFA25D7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// VehicleDispPropDlg.h : header file
//

#include "VehicleDispPropListCtrl.h"
#include "../InputAirside/VehicleDispProps.h"
#include "../MFCExControl/ListCtrlEx.h"
/////////////////////////////////////////////////////////////////////////////
// CVehicleDispPropDlg dialog

class CTermPlanDoc;
class CVehicleDispPropDlg : public CDialog
{
	// Construction
public:
	CVehicleDispPropDlg(int nProjID,CWnd* pParent);   // standard constructor

	// Dialog Data
	//{{AFX_DATA(CVehicleDispPropDlg)
	enum { IDD = IDD_DIALOG_PAXDISPPROP };
	CStatic	m_toolbarcontenter2;
	CButton	m_btnBarFrame2;
	CListCtrlEx	m_listNamedSets;
	CButton	m_btnBarFrame;
	CButton	m_btnOk;
	CButton	m_btnCancel;
	CButton	m_btnSave;
	CStatic	m_toolbarcontenter;
	CVehicleDispPropListCtrl m_listctrlProp;
	//}}AFX_DATA


	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CVehicleDispPropDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	// Implementation
protected:
	std::vector<CVehicleDispPropNode *> m_vrDeletedNode;
	std::vector<CVehicleDispPropItem *> m_vrDeletedItem;	

	void InitToolbar();
	CToolBar m_ToolBar;
	CToolBar m_ToolBar2;

	CString GetProjPath();
	CTermPlanDoc* GetDocument() const;

	int m_nSelectedPDPSet;
	int m_nSelectedPropItem;
	int m_nProjID;
	BOOL m_bNewPDPSet;

	void UpdatePDP();
	void UpdatePDPSetList();
	void UpdateVehicleDispPropIn3D() const;

	// Generated message map functions
	//{{AFX_MSG(CVehicleDispPropDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnButtonSave();
	virtual void OnCancel();
	virtual void OnOK();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnToolbarbuttonadd();
	afx_msg void OnToolbarbuttondel();
	afx_msg void OnToolbarbuttonedit();
	afx_msg void OnClickListDispprop(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg LRESULT OnPaxProcEdit( WPARAM wParam, LPARAM lParam );
	afx_msg void OnClose();
	afx_msg void OnItemChangedListPDPSets(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnPDPSetAdd();
	afx_msg void OnPDPSetDelete();
	afx_msg void OnEndLabelEditListPDPSets(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PAXDISPPROPDLG_H__2D990C4A_08FF_4F78_826C_6329FEFA25D7__INCLUDED_)
