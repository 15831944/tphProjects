#if !defined(AFX_LINKAGEDLG_H__65D2FD00_A086_4E32_948A_BA633C06F571__INCLUDED_)
#define AFX_LINKAGEDLG_H__65D2FD00_A086_4E32_948A_BA633C06F571__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// LinkageDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CLinkageDlg dialog
#include "..\inputs\in_term.h"
#include <CommonData/procid.h>
#include "..\inputs\procdata.h"
#include <MFCExControl/SplitterControl.h>
class CLinkageDlg : public CDialog
{
// Construction
public:
	void DoResize(int delta,UINT nIDSplitter);
	void DoSize(int cx,int cy);
	CSplitterControl m_wndSplitter1;
	CSplitterControl m_wndSplitter2;
	void ShowDialog(CWnd* parentWnd);
	void HideDialog(CWnd* parentWnd);
	void AddProc();
	BOOL m_bByProc;
	void AddGate();
	BOOL m_bDragging;
	HTREEITEM m_hItemDragSrc;
	~CLinkageDlg();
	CImageList* m_pDragImage;
	void IsEnableBtnAddProc();
	BOOL GetProcID(ProcessorID& id);
	BOOL SelectGate(ProcessorID _id);
	BOOL GetGateID(ProcessorID& id);
	void EnableBtnOfTB(UINT nID,BOOL bEnable);
	void InitGateTree();
	CImageList m_ImageList;
	std::vector<ProcessorID> m_vectID;
	std::vector<ProcessorID> m_vectIDGate;
	BOOL InitProcTree(int nProcType,CTreeCtrl& treeCtrl,UINT nIDImage,std::vector<ProcessorID>& vectID);
	void InitToolBar();
	InputTerminal* m_pInputTerm;
	CToolBar m_ToolBar;
	CLinkageDlg(InputTerminal* _pInTerm,CString& strProjectPath,CWnd* pParent = NULL);   // standard constructor
	CString szProjectPath;
// Dialog Data
	//{{AFX_DATA(CLinkageDlg)
	enum { IDD = IDD_LINKAGE };
	CButton	m_btnCancel;
	CButton	m_btnOK;
	CStatic	m_staticSeledGate;
	CStatic	m_staticProcessor;
	CStatic	m_staticGate;
	CTreeCtrl	m_TreeGate;
	CTreeCtrl	m_TreeProc;
	CTreeCtrl	m_TreeSeledGate;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLinkageDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:
	HTREEITEM IsRootOfGate( CTreeCtrl* p_tree, CString& strLable );
	HTREEITEM IsChildItem( CTreeCtrl* p_tree, CString& strLable, HTREEITEM hParent );
	bool AddElementToDB( ProcessorID& _procID, ProcessorID& _gateID );
	void DeleteElementFromDB( ProcessorID& _procID,ProcessorID& _gateID );
	void SetOne2OneFlag( ProcessorID& _procID, ProcessorID& _gateID ); 
	MiscDataElement* SelectEntryProc(ProcessorID _id,int m_nProcDataType);
	int GetProcssorType( ProcessorID _id);
	void sortGateTree();
protected:

	// Generated message map functions
	//{{AFX_MSG(CLinkageDlg)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangedTreeProcessor(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelchangedTreeGate(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelchangedTreeSelectedGate(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBegindragTreeProcessor(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnBegindragTreeGate(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI);
	//}}AFX_MSG
	afx_msg void OnAddGate();
	afx_msg void OnAddProc();
	afx_msg void OnDel();
	afx_msg void OnPickGate();
	afx_msg void OnPickProc();
	afx_msg void OnOneToOne();
	afx_msg LRESULT OnTempFallbackFinished(WPARAM wParam,LPARAM lParam);
	afx_msg BOOL OnToolTipText(UINT nID, NMHDR* pNMHDR, LRESULT* pResult);
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LINKAGEDLG_H__65D2FD00_A086_4E32_948A_BA633C06F571__INCLUDED_)
