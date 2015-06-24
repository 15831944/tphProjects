#pragma once
#include "../MFCExControl/ToolTipDialog.h"
#include "../MFCExControl/ListCtrlEx.h"

class InputTerminal;
class GateAssignmentConstraintList;
class GateAssignmentConstraint;

class CDlgGateAssignConstraints : public CToolTipDialog
{
public:
	CDlgGateAssignConstraints(InputTerminal* pInTerm, int nAirportID,int nProjectID, GateAssignmentConstraintList* pConstraints, CWnd* pParent = NULL); 
	virtual ~CDlgGateAssignConstraints();

	enum { IDD = IDD_GATEASSIGN_CONSTRAINT };

protected:
	DECLARE_MESSAGE_MAP()
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();

	void RefreshLayout(int cx = -1, int cy = -1);
	void UpdateToolBarState();
	void InitListControl();
	void AddConstraintItem(const GateAssignmentConstraint& conItem);
	void SetListItemContent(int nIndex, const GateAssignmentConstraint& conItem);
	CString GetProjPath();
	int GetSelectedListItem();
	BOOL FindSameConstraints(std::vector<CString>&vConstraints,const char* strConstraints);
	int m_nProjectID;
	InputTerminal*	m_pInputTerminal;
	int m_nAirportID;
	GateAssignmentConstraintList* m_pConstraints;
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
	afx_msg void OnAdjacencyCon();
	afx_msg void OnSave();
public:
	afx_msg void OnBnClickedButtonActypeconstraint();
};
