#pragma once
#include "ACDispListCtrl.h"

// CAircraftDispPropDlg dialog
class CTermPlanDoc;
class CAircraftDispPropDlg : public CDialog
{
	// Construction
public:
	CAircraftDispPropDlg(CWnd* pParent);
	enum { IDD = IDD_DIALOG_PAXDISPPROP };

protected:
	CStatic	m_toolbarcontenter2;
	CButton	m_btnBarFrame2;
	CListCtrl	m_listNamedSets;
	CButton	m_btnBarFrame;
	CButton	m_btnOk;
	CButton	m_btnCancel;
	CButton	m_btnSave;
	CStatic	m_toolbarcontenter;
	CACDispListCtrl m_listctrlProp;

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	void InitToolbar();
	CToolBar m_ToolBar;
	CToolBar m_ToolBar2;

	CTermPlanDoc* GetDocument() const;
	CString GetProjPath();

	int m_nSelectedPDPSet;

	BOOL m_bNewPDPSet;

	void UpdatePDP();
	void UpdatePDPSetList();
	void UpdateAircraftDispPropIn3D() const;

	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	virtual void OnOK();

	afx_msg void OnButtonSave();
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

	DECLARE_MESSAGE_MAP()
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
};
