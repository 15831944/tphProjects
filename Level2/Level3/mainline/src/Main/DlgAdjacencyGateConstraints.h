#pragma once
#include "../MFCExControl/ToolTipDialog.h"
#include "../MFCExControl/ListCtrlEx.h"
#include "../Inputs/AdjacencyGateConstraint.h"

class InputTerminal;

class CDlgAdjacencyGateConstraints : public CToolTipDialog
{
public:
	CDlgAdjacencyGateConstraints(InputTerminal* pInTerm,AdjacencyGateConstraintList* pAdjGateConstraints,int nProjID,CWnd* pParent = NULL);
	virtual ~CDlgAdjacencyGateConstraints();
	enum { IDD = IDD_ADJACENCYGATE_CONSTRAINTS };

protected:
	DECLARE_MESSAGE_MAP()
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();

	void RefreshLayout(int cx = -1, int cy = -1);
	void UpdateToolBarState();
	void InitListControl();
	int GetSelectedListItem();
	void AddConstraintItem(AdjacencyGateConstraint& conItem);
	void SetListItemContent(int nIndex,AdjacencyGateConstraint& conItem);
	CString GetProjPath();

	int m_nProjID;
	InputTerminal*	m_pInputTerminal;
	AdjacencyGateConstraintList* m_pAdjGateConstraints;
	CToolBar		m_wndToolBar;
	CListCtrlEx		m_wndListCtrl;

	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI);
	afx_msg void OnCmdNewItem();
	afx_msg void OnCmdDeleteItem();
	afx_msg void OnCmdEditItem();
	afx_msg void OnLvnItemchangedList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnEndlabeleditListContents(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnSave();
	afx_msg LRESULT OnCollumnIndex(WPARAM wParam,  LPARAM lParam);
};
