#pragma once
#include "afxcmn.h"
#include "afxwin.h"
#include "../MFCExControl/ListCtrlEx.h"
#include "CabinCrewDispListCtrl.h"

class InputTerminal;
class CabinCrewGeneration;
class CTermPlanDoc;
class DlgCabinCrewGeneration: public CXTResizeDialog
{
	DECLARE_DYNAMIC(DlgCabinCrewGeneration)
public:
	DlgCabinCrewGeneration(InputTerminal * pInterm,CTermPlanDoc* pDoc , CWnd* pParent = NULL);
	~DlgCabinCrewGeneration(void);

	// Dialog Data
	enum { IDD = IDD_DIALOG_PAXCHARADISP };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual void OnOK();
	DECLARE_MESSAGE_MAP()

protected:
	void OnInitToolbar();
	void OnInitListCtrl();
	void DisplayData();
	void OnUpdateToolbar();
	void OnListItemChanged(int nItem, int nSubItem);

	afx_msg void OnAddCrew();
	afx_msg void OnRemoveCrew();
	afx_msg void OnSave();
	afx_msg void OnEndlabeledit(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelChangeListCtrl(NMHDR *pNMHDR, LRESULT *pResult);
public:
	CCabinCrewDispListCtrl m_wndListCtrl;
	CToolBar  m_wndToolbar;
	InputTerminal * m_pInterm;
	CabinCrewGeneration* m_pCabinCrewGeneration;
};
