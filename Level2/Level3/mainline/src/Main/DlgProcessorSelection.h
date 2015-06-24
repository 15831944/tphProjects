#pragma once
#include "afxwin.h"
#include "commondata/procid.h"
#include "afxcmn.h"

// CDlgProcessorSelection dialog
class InputTerminal;
class CDlgProcessorSelection : public CDialog
{
	DECLARE_DYNAMIC(CDlgProcessorSelection)

public:
	CDlgProcessorSelection(const ProcessorID& ProcID, InputTerminal* _pInTerm, CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgProcessorSelection();

// Dialog Data
	enum { IDD = IDD_DLGPROCESSORSELECTION };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();

	const CString& GetSelProcName();
private:
	void AddProcToTree( HTREEITEM _hParent, const ProcessorID& _id );

private:
	CTreeCtrl m_ctlProcTree;
	ProcessorID m_ProcName;
	CString m_strSelProc;
	InputTerminal* m_pInTerm;
public:

	afx_msg void OnTvnSelchangedTreeProc(NMHDR *pNMHDR, LRESULT *pResult);
};
