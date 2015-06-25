#pragma once

#include "resource.h"
#include "afxwin.h"
#include "../MFCExControl/XTResizeDialog.h"
// CDlgSelectSector dialog

class CDlgSelectSector : public CXTResizeDialog
{
	DECLARE_DYNAMIC(CDlgSelectSector)

public:
	CDlgSelectSector(int nProjID, CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgSelectSector();
	virtual BOOL OnInitDialog();
	void InitListBox();

	int m_nProjID;

// Dialog Data
	enum { IDD = IDD_DIALOG_SECTOR_SELECT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CListBox m_wndListCtrlSector;
	CListBox m_wndListCtrlProperty;
	CString m_strSector;
	int m_nSectorID;
public:
	CString GetListBoxItemText(CListBox& hListBox);
	afx_msg void OnLbnSelchangeSectorList();
	CString GetListSector()   { return m_strSector;}
	int GetSectorID()		  { return m_nSectorID;}
};
