#if !defined(AFX_PAXBULKLISTDLG_H__D84C2C76_E265_4506_872C_ED035334B6E0__INCLUDED_)
#define AFX_PAXBULKLISTDLG_H__D84C2C76_E265_4506_872C_ED035334B6E0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PaxBulkListDlg.h : header file
//
#include "inputs\Paxbulkinfo.h"
#include "PaxBulkListCtrl.h"
/////////////////////////////////////////////////////////////////////////////
// CPaxBulkListDlg dialog

class CPaxBulkListDlg : public CDialog
{
// Construction
public:
	void GetListData();
	void ReloadList();
	CString GetProjPath();
	InputTerminal* GetInputTerminal();
	void LoadData();
	CPaxBulkListDlg(CWnd* pParent = NULL);   // standard constructor
	~CPaxBulkListDlg();		
	CToolBar m_ToolBar;	
	CPaxBulk  m_Bulkselected;	

// Dialog Data
	//{{AFX_DATA(CPaxBulkListDlg)
	enum { IDD = IDD_DIALOG_PAXBULKlIST };
	CStatic	m_staticToolBar;
	CPaxBulkListCtrl  m_PaxBulkListCtr;
	//}}AFX_DATA
	std::vector<CString> m_vBulkPercent;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPaxBulkListDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	CString MakeString(const std::vector<int>& vPercent);
	std::vector<int> ParseString(const CString& strPercent);

// Implementation
protected:
	CPaxBulkInfo*   m_pPaxBulk;	
	void EditOneBulk();
	// Generated message map functions
	//{{AFX_MSG(CPaxBulkListDlg)
	virtual BOOL OnInitDialog();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnProcessorDataAdd();
	virtual void OnOK();
	afx_msg void OnToolbtpaxbulkdel();
	afx_msg void OnToolbtpaxbulkedit();
	afx_msg void OnClickList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclkList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnButtonsave();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
    int m_oldCx;
    int m_oldCy;
    typedef enum {TopLeft, TopRight, BottomLeft, BottomRight} LayoutRef;
    void LayoutControl(CWnd* pCtrl, LayoutRef refTopLeft, LayoutRef refBottomRight, int cx, int cy);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PAXBULKLISTDLG_H__D84C2C76_E265_4506_872C_ED035334B6E0__INCLUDED_)
