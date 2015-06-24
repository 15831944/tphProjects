#pragma once


// CDlgGateTypedefine dialog
#include "resource.h"
#include "afxcmn.h"
#include "..\MFCExControl\ListCtrlEx.h"
class CDlgGateTypedefine : public CDialog
{
	DECLARE_DYNAMIC(CDlgGateTypedefine)

public:
	CDlgGateTypedefine(StringDictionary* _pStrDict,ProcessorList* proList,CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgGateTypedefine();

// Dialog Data
	enum { IDD = IDD_DIALOG_GATETYPE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()

public:
	void OnInitListCtrl();
	void InitGateType();
public:
	ProcessorList* m_pCurProList;
	CListCtrlEx m_wndListCtrl;
//	InputTerminal * m_pInTerm;
	StringDictionary* m_pStrDict;
	afx_msg void OnBnClickedOk();
	afx_msg void OnLvnEndlabeleditListGate(NMHDR *pNMHDR, LRESULT *pResult);
protected:
	virtual void PostNcDestroy();
};
