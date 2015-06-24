#pragma once
#include "afxcmn.h"
#include "afxwin.h"
#include "../MFCExControl/ListCtrlEx.h"
#include "PaxProDispListCtrl.h"
#include "../MFCExControl/XTResizeDialog.h"

class InputTerminal;

// CDlgPaxCharaDisp dialog

class CDlgPaxCharaDisp : public CXTResizeDialog
{
	DECLARE_DYNAMIC(CDlgPaxCharaDisp)

public:
	CDlgPaxCharaDisp(InputTerminal * pInterm,int nProjID,CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgPaxCharaDisp();

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

	afx_msg void OnAddPaxType();
	afx_msg void OnRemovePaxType();
	afx_msg void OnSave();
	afx_msg void OnEndlabeledit(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg LRESULT OnDBClick(WPARAM wParam, LPARAM lParam);
	afx_msg void OnSelChangePassengerType(NMHDR *pNMHDR, LRESULT *pResult);
public:
	CPaxProDispListCtrl m_wndListCtrl;
	CToolBar  m_wndToolbar;

	InputTerminal * m_pInterm;
	PaxPhysicalCharatieristicsList m_PaxCharaList;
	int m_nProj;
};
