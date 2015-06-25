#pragma once

#include "MFCExControl\ListCtrlEx.h"
#include "Inputs\PaxTypeWithEntryFlightTime.h"
#include "afxwin.h"


class DlgEntryFlightTime : public CDialog
{
	DECLARE_DYNAMIC(DlgEntryFlightTime)

public:
	DlgEntryFlightTime(InputTerminal* pTerminal, CWnd* pParent = NULL); 
	virtual ~DlgEntryFlightTime();

	enum { IDD = IDD_DIALOG_ENTRYFLIGHTTIME };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CButton m_btnOk;
	CButton m_btnSave;
	CButton m_btnImport;
	CButton m_btnCancel;
	CToolBar m_toolbar;
	CListCtrlEx m_listCtrl;
protected:
	InputTerminal* m_pTerminal;
	EntryFlightTimeDB* m_pEntryFltTimeDB;

protected:
	void ReloadData();
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnBnClickedOk();

};
