#if !defined(AFX_DLGRETAILPESFEE_H__270F64C2_672D_49F7_A2F0_9BADCEA46E82__INCLUDED_)
#define AFX_DLGRETAILPESFEE_H__270F64C2_672D_49F7_A2F0_9BADCEA46E82__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CDlgRetailPackFee.h : header file
//

#include "ProcessorTreeCtrl.h"
#include "..\MFCExControl\ListCtrlEx.h"
#include "inputs\pax_cnst.h"
#include "economic\EconomicManager.h"
#include "inputs\MultiMobConstraint.h"
#include "MoblieELemTips.h"

/////////////////////////////////////////////////////////////////////////////
// CDlgRetailPackFee dialog

enum FEESTYPE { RETAILPERCENTAGEFEES, PACKINGFEES, LANDSIDETRANSFEE };

class CDlgRetailPackFee : public CDialog
{
// Construction
public:
	CDlgRetailPackFee(enum FEESTYPE _enumType,CWnd* pParent);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgRetailPackFee)
	enum { IDD = IDD_DIALOG_RETAILPERSFREE };
	CButton	m_btnDel;
	CButton	m_btnNew;
	CStatic	m_toolbarcontenter;
	CButton	m_btnSave;
	CProcessorTreeCtrl	m_treeProc;
	CListCtrlEx	m_listCtrl;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgRetailPackFee)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void ShowTips(int iItemData);

	//show tips for listex
	CMoblieElemTips m_toolTips;
	int m_nListPreIndex;

	void GetDatabase();
	
	int GetSelectedListItem();
	void InitToolbar();
	CToolBar m_ToolBar;
	InputTerminal* GetInputTerminal();
	CString GetProjPath();
	void InitListCtrl();
	void ReloadDatabase();
	enum FEESTYPE m_enumType;
	void ShowDefaultListData();
	void ShowListData();
	void GetDBIdx( int& nDBIdx, BOOL& bOwn );
	void ModifyItemData( ProbabilityDistribution* _pProbDist,int nSubItem );

	CEconomicManager* GetEconomicManager();


	CMultiMobConstraint m_multiPaxConstr;

	CProcEconDatabase2* m_pDB;

	// Generated message map functions
	//{{AFX_MSG(CDlgRetailPackFee)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnToolbarbuttonadd();
	afx_msg void OnToolbarbuttondel();
	virtual BOOL OnInitDialog();
	afx_msg void OnButtonSave();
	virtual void OnCancel();
	virtual void OnOK();
	afx_msg void OnSelchangedTree1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnEndlabeledit(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnButtonNew();
	afx_msg void OnButtonDel();
	afx_msg void OnItemchangedList1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGRETAILPESFEE_H__270F64C2_672D_49F7_A2F0_9BADCEA46E82__INCLUDED_)
