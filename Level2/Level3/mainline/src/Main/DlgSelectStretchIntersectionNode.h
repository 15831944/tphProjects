#pragma once
#include "afxcmn.h"


// CDlgSelectStretchIntersectionNode dialog

class CDlgSelectStretchIntersectionNode : public CDialog
{
	DECLARE_DYNAMIC(CDlgSelectStretchIntersectionNode)

public:
	CDlgSelectStretchIntersectionNode(int nAirportID,CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgSelectStretchIntersectionNode();

// Dialog Data
	enum { IDD = IDD_DIALOG_SELECTSTRETCHNODE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	void SetTreeContent();
public:
	int m_nSelectID;
	CString m_strName;

	DECLARE_MESSAGE_MAP()
protected:
	int m_nAirportID;
	CTreeCtrl m_wndTreeCtrl;
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnTvnSelchangedTreeStretch(NMHDR *pNMHDR, LRESULT *pResult);
};
