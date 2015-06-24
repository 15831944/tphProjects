#pragma once

#include "../MFCExControl/ListCtrlEx.h"
#include "InputOnBoard/WayFindingIntervention.h"
#include "../MFCExControl/XTResizeDialog.h"


// CDlgWayFindingIntervention dialog
class InputTerminal;
class CDlgWayFindingIntervention : public CXTResizeDialog
{
	DECLARE_DYNAMIC(CDlgWayFindingIntervention)

public:
	CDlgWayFindingIntervention(InputTerminal * pInterm,int nProjID,CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgWayFindingIntervention();

// Dialog Data
	enum { IDD = IDD_DIALOG_WAYFINDINGINTERVENTION };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual	BOOL OnInitDialog();
	virtual int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual void OnOK();

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnLvnItemchangedListData(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnAddPaxType();
	afx_msg void OnRemovePaxType();
	afx_msg void OnEditPaxType();
	afx_msg void OnEndlabeledit(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg LRESULT OnDBClick(WPARAM wParam, LPARAM lParam);
	afx_msg void OnSave();
protected:
	CToolBar m_wndToolBar;
	CListCtrlEx m_wndListCtrl;
	InputTerminal* m_pInterm;
	int m_nProjID;
	CWayFindingInterventionList m_wayFindingInterventionList;

	void OnInitToolbar();
	void OnInitListCtrl();
	void DisplayData();
	CString getString(BOOL bSelect);
	CString getTypeString(PaxType emPaxType);
	BOOL getValueFromCString(CString strValue);
	PaxType getPaxTypeFromCString(CString strPaxType);
	void OnUpdateToolbar();
};
