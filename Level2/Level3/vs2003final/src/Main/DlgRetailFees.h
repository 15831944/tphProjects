#pragma once

 #include "ProcessorTreeCtrl.h"
#include "..\MFCExControl\ListCtrlEx.h"
#include "..\inputs\in_term.h"
// #include "inputs\pax_cnst.h"
 #include "economic\EconomicManager.h"
#include "Engine/process.h"
#include "Engine/proclist.h"
#include "Engine/RetailProcessor.h"
#include "MFCExControl/CoolTree.h"
#include "..\Inputs\ProductStockingList.h"
// CDlgRetailFees dialog

class CDlgRetailFees : public CDialog
{
	DECLARE_DYNAMIC(CDlgRetailFees)

public:
	CDlgRetailFees(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgRetailFees();

// Dialog Data
	enum { IDD = IDD_DIALOG_RETAILPERSFREE };
	CButton	m_btnDel;
	CButton	m_btnNew;
	CStatic	m_toolbarcontenter;
	CButton	m_btnSave;
	CProcessorTreeCtrl	m_treeProc;
	//CCoolTree m_treeProc;
	CListCtrlEx	m_listCtrl;
	 CToolBar m_ToolBar;
	 ProductStockingList m_ProductStockingList;

	//ProcessorList* m_pProcList;

public:
// 	virtual BOOL PreTranslateMessage(MSG* pMsg);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

protected:
// 	void ShowTips(int iItemData);
// 
// 	//show tips for listex
// 	CMoblieElemTips m_toolTips;
// 	int m_nListPreIndex;
// 
//  	void GetDatabase();
// 
 	int GetSelectedListItem();
// 	void InitToolbar();
 	InputTerminal* GetInputTerminal();
 	CString GetProjPath();
 	void InitListCtrl();
// 	void ReloadDatabase();
// 	enum FEESTYPE m_enumType;
 	void ShowDefaultListData();
 	void ShowListData();
 	void GetDBIdx( int& nDBIdx, BOOL& bOwn );
// 	void ModifyItemData( ProbabilityDistribution* _pProbDist,int nSubItem );
// 
	void InitToolbar();
	void SetRetailTreeContent();
//     void AddStrToTree(const CString &strRetail);
// 	bool StrExistUnderItem(HTREEITEM &hItem, const CString& str);
	
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
// 	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
