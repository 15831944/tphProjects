#if !defined(AFX_UTILITIESDIALOG_H__53D5F748_2442_4429_A567_C8A350FC16F2__INCLUDED_)
#define AFX_UTILITIESDIALOG_H__53D5F748_2442_4429_A567_C8A350FC16F2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// UtilitiesDialog.h : header file
//

#include "Economic\UtilityCostDatabase.h"
#include "Economic\EconomicManager.h"
#include "..\MFCExControl\ListCtrlEx.h"



/////////////////////////////////////////////////////////////////////////////
// CUtilitiesDialog dialog

class CUtilitiesDialog : public CDialog
{
// Construction
public:
	CUtilitiesDialog( CWnd* pParent );   // standard constructor

// Dialog Data
	//{{AFX_DATA(CUtilitiesDialog)
	enum { IDD = IDD_DIALOG_UTILITIES };
	CStatic	m_listtoolbarcontenter;
	CSpinButtonCtrl	m_spinMed;
	CSpinButtonCtrl	m_spinLow;
	CSpinButtonCtrl	m_spinHigh;
	CListCtrlEx	m_List;
	CEdit	m_editTotalHrs;
	CEdit	m_editMedHrs;
	CEdit	m_editMedCost;
	CEdit	m_editLowHrs;
	CEdit	m_editLowCost;
	CEdit	m_editHighHrs;
	CEdit	m_editHighCost;
	CEdit	m_editFixedCost;
	CButton	m_btnSave;
	CButton	m_btnClear;
	double	m_dFixCost;
	double	m_dHighCost;
	int		m_nHighHrs;
	double	m_dLowCost;
	int		m_nLowHrs;
	double	m_dMedCost;
	int		m_nMedHrs;
	int		m_nTotalHrs;
	//}}AFX_DATA

	int GetSelectedItem();
	void RefreshData();
	void RefreshTotalHrs();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CUtilitiesDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CUtilityCostDatabase* m_pUtilCostDB;


	void InitToolbar();
	void InitListCtrl();

	void SetData( UtilityCostDataItem* _pItem );

	void ReloadDatabase();

	CEconomicManager* GetEconomicManager();

	CString GetProjPath();

	void EnableControl( bool _bEnable );


protected:
	CToolBar m_ListToolBar;

	// Generated message map functions
	//{{AFX_MSG(CUtilitiesDialog)
	virtual BOOL OnInitDialog();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnEndlabeleditListUtilitiesComponent(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnItemchangedListUtilitiesComponent(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBtnUtilitiesClear();
	afx_msg void OnBtnUtilitiesSave();
	afx_msg void OnPeoplemoverNew();
	afx_msg void OnPeoplemoverDelete();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnKillfocusEdit();
	afx_msg void OnDeltaposSpinUtilitiesHigh(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposSpinUtilitiesLow(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposSpinUtilitiesMed(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_UTILITIESDIALOG_H__53D5F748_2442_4429_A567_C8A350FC16F2__INCLUDED_)
