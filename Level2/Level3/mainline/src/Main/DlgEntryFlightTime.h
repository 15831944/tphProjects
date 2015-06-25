#pragma once

#include "MFCExControl\ListCtrlEx.h"
#include "Inputs\PaxTypeWithEntryFlightTime.h"
#include "afxwin.h"


class DlgEntryFlightTime : public CDialogEx
{
	DECLARE_DYNAMIC(DlgEntryFlightTime)

public:
	DlgEntryFlightTime(InputTerminal* pTerminal, EntryFlightTimeDB* pDB, CWnd* pParent = NULL); 
	virtual ~DlgEntryFlightTime();

	enum { IDD = IDD_ENTRYFLIGHTTIME };

protected:
	InputTerminal* m_pTerminal;
	CToolBar m_toolbar;
	CListCtrlEx m_listCtrl;
	EntryFlightTimeDB* m_pEntryFltTimeDB;
	CButton m_btnSave;
	CButton m_btnOk;
	CButton m_btnCancel;
	CButton m_btnImport;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	void ReloadData();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	
};
