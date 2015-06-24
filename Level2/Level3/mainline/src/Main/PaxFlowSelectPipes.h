#if !defined(AFX_PAXFLOWSELECTPIPES_H__1424A906_1487_4F83_8642_E88D51B329DB__INCLUDED_)
#define AFX_PAXFLOWSELECTPIPES_H__1424A906_1487_4F83_8642_E88D51B329DB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PaxFlowSelectPipes.h : header file
//
class InputTerminal;
/////////////////////////////////////////////////////////////////////////////
// CPaxFlowSelectPipes dialog
#include <vector>
typedef std::vector<int> PIPEIDX;
class CPaxFlowSelectPipes : public CDialog
{
// Construction
public:
	CPaxFlowSelectPipes(InputTerminal* _pTerm, const PIPEIDX& _pipes, int _iTypeOfUsingPipes,CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CPaxFlowSelectPipes)
	enum { IDD = IDD_DIALOG_SELECT_PIPE_TO_FLOW };
	CTreeCtrl	m_treeResult;
	CButton	m_btnReset;
	CButton	m_btnSelect;
	CListBox	m_listPipes;
	//}}AFX_DATA
	InputTerminal* m_pTerm;
	 PIPEIDX m_vPipeIdx;
	 bool m_bSelectPipe;
	 int m_iTypeOfUsingPipes;



// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPaxFlowSelectPipes)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void ReloadList();
	void DisplayPath();
	// Generated message map functions
	//{{AFX_MSG(CPaxFlowSelectPipes)
	virtual BOOL OnInitDialog();
	afx_msg void OnButtonReset();
	afx_msg void OnButtonSelect();
	afx_msg void OnDblclkListPipes();
	afx_msg void OnOk();
	//}}AFX_MSG
	afx_msg void OnUpdateTreeResult(CCmdUI* pCmdUI);
		
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PAXFLOWSELECTPIPES_H__1424A906_1487_4F83_8642_E88D51B329DB__INCLUDED_)
