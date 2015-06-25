#pragma once
#include "afxwin.h"

// CDlgFlightCondition dialog
#include "..\MFCExControl\ListCtrlEx.h"
class CDlgFlightCondition : public CDialog
{
	DECLARE_DYNAMIC(CDlgFlightCondition)

public:
	CDlgFlightCondition(Terminal* _pTerm,FlightGroup* pGroup,CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgFlightCondition();
	virtual BOOL OnInitDialog();

// Dialog Data
	enum { IDD = IDD_DIALOG_CONDITION };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CString m_strGroupName;
	FlightGroup* m_pCurGroup;
	int m_nCurSel;
	Terminal* m_pTerm;
	CListCtrlEx	m_wndListCtrl;
	CToolBar m_wndToolBar;
	std::vector<FlightGroup::CFlightGroupFilter* > m_vFilter;
	CStringList m_stringList;
private:
	void InitStringList();
public:
	afx_msg void OnAddFilter();
	afx_msg void OnRemoveFilter();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnEnKillfocusEditGroupName();
	void OnInitListCtrl();
	void DisplayFilter();

	//CString m_strAirline;
	//CString m_strFlightID;
	//CString m_strDay;
	afx_msg void OnLbnSelchangeListCondition();
	afx_msg void OnLvnEndlabeleditListCondition(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnItemchangingListCondition(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg LRESULT OnMsgComboChange(WPARAM wParam, LPARAM lParam);
};
