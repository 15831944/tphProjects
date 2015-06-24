#if !defined(AFX_ECONOMIC2DIALOG_H__D9E0A3A6_F35A_4676_B62E_47D0958073B2__INCLUDED_)
#define AFX_ECONOMIC2DIALOG_H__D9E0A3A6_F35A_4676_B62E_47D0958073B2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Economic2Dialog.h : header file
//

#include "..\MFCExControl\ListCtrlEx.h"
#include "..\economic\EconomicManager.h"
#include "MoblieElemTips.h"

enum ECONOMIC2TYPE {
	PASSENGER, AIRCRAFT,
	PASSENGER_AIRPORT_FEES,
	LANDING_FEES, WAITING_COST_FACTORS
};

/////////////////////////////////////////////////////////////////////////////
// CEconomic2Dialog dialog

class CEconomic2Dialog : public CDialog
{
// Construction
public:
	CEconomic2Dialog(enum ECONOMIC2TYPE _enumType, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CEconomic2Dialog)
	enum { IDD = IDD_DIALOG_ECONOMIC2 };
	CStatic	m_listtoolbarcontenter;
	CListCtrlEx	m_List;
	CButton	m_btnSave;
	//}}AFX_DATA
	
	void SetColNameArray(const CStringArray& arColName);
	void SetTitle(LPCTSTR szTitle);
	int GetSelectedItem();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEconomic2Dialog)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void InitToolbar();
	void InitListCtrl();

	void ReloadDatabase();

	CEconomicManager* GetEconomicManager();

	CString GetProjPath();

	InputTerminal* GetInputTerminal();


protected:
	void ShowTips(int iItemData);
	enum ECONOMIC2TYPE m_enumType;
	CToolBar m_ListToolBar;
	CStringArray m_arColName;
	CString m_strTitle;

	CMoblieElemTips m_toolTips;
	int m_nListPreIndex;
	// Generated message map functions
	//{{AFX_MSG(CEconomic2Dialog)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual BOOL OnInitDialog();
	afx_msg void OnEndlabeleditListEconomic2(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnItemchangedListEconomic2(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBtnEconomic2Save();
	afx_msg void OnPeoplemoverNew();
	afx_msg void OnPeoplemoverDelete();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ECONOMIC2DIALOG_H__D9E0A3A6_F35A_4676_B62E_47D0958073B2__INCLUDED_)
