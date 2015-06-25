#pragma once
#include "../MFCExControl/CoolTree.h"
#include "NodeViewDbClickHandler.h"
#include "../MFCExControl/XTResizeDialog.h"


// DlgStandCriteriaAssignment dialog
class CAirportDatabase;
class StandCriteriaAssignmentList;
class DlgStandCriteriaAssignment : public CXTResizeDialog
{
	DECLARE_DYNAMIC(DlgStandCriteriaAssignment)

public:
	DlgStandCriteriaAssignment(int nProjID,CAirportDatabase *pAirPortdb,PSelectFlightType pSelectFlightType,CWnd* pParent = NULL);   // standard constructor
	virtual ~DlgStandCriteriaAssignment();

// Dialog Data
	enum { IDD = IDD_STANDCRITERIAASSIGNMENT };
	enum TreeNodeType
	{
		TREENODE_NONE = 0,
		TREENODE_LEADINLINE,
		TREENODE_LEADOUTLINE
	};

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual int OnCreate(LPCREATESTRUCT lpCreateStruct);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void AddStandItem();
	afx_msg void RemoveStandItem();
	afx_msg void AddFlightTypeItem();
	afx_msg void EditFlightItem();
	afx_msg void RemoveFlightItem();
	afx_msg void OnSelchangedTree(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	afx_msg void OnBnClickedOk();
	afx_msg void SaveData();
protected:
	void OnInitToolbar();
	void OnInitTreeCtrl();
	void SetComboBoxItem(CComboBox* pCB);
	void SetLineInComBoxItem(CComboBox* pCB,int nObjID);
	void SetLineOutComBoxItem(CComboBox* pCB,int nObjID);
	void DisplayTreeItem(HTREEITEM hItem);
private:
	int m_nProjID;
	CAirportDatabase* m_pAirportDB;
	CToolBar m_wndToolbar;
	CCoolTree m_wndTreeCtrl;
	StandCriteriaAssignmentList* m_pStandCriteriaList;
	PSelectFlightType	m_pSelectFlightType;
	HTREEITEM m_hRclickItem;
};
