#if !defined(AFX_BAGDEVDLG_H__A94FEA84_BCAD_4BBF_8744_BFFEBFD9EEAB__INCLUDED_)
#define AFX_BAGDEVDLG_H__A94FEA84_BCAD_4BBF_8744_BFFEBFD9EEAB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// BagDevDlg.h : header file
//

#include "TreeCtrlEx.h"
#include "..\inputs\in_term.h"
#include "ConDBListCtrl.h"
#include "ProcessorTreeCtrl.h"
#include "MoblieElemTips.h"

/////////////////////////////////////////////////////////////////////////////
// CBagDevDlg dialog


class CBagDevDlg : public CDialog
{
// Construction
public:
	void ReloadLagTimeList2(int _nDBIdx);
	void LoadTree2();
	CBagDevDlg(int nAirportID, CWnd* pParent);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CBagDevDlg)
	enum { IDD = IDD_DIALOG_BAGDEV };
	CConDBListCtrl	m_listctrlLagTime2;
	CTreeCtrlEx	m_AssignTree;
	CButton	m_btnSave;
	CButton	m_btnOk;
	CButton	m_btnCancel;
	CConDBListCtrl	m_listctrlLagTime;
	CStatic	m_toolbarcontenter;
	CStatic m_SetPrioritycontenter;
	CStatic m_Fbdtcontenter;
	CStatic m_Carbincontenter;
	int		m_nFligtPriority;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBagDevDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	void EvenPercent( double* _cPercent, int _iCount );
	void ShowTips( int iItemData );
	void InitToolbar();
	CToolBar m_ToolBar;
	CString GetProjPath();
	InputTerminal* GetInputTerminal();
	bool CheckPercent();
	void SetPercent( int _nPercent );
	HTREEITEM m_hRClickItem;
	void LoadTree();
	void ReloadLagTimeList( int _nIdx );
	void ReloadData( Constraint* _pSelCon );
	void SelectLagTimeList(int _nIdx);
	int GetSelectedItem();

	bool m_bFlightPriority;
	int m_nAirportID;

	CMoblieElemTips m_toolTips;
	int m_nListPreIndex;
	// Generated message map functions
	//{{AFX_MSG(CBagDevDlg)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI);
	virtual BOOL OnInitDialog();
	afx_msg void OnClickListLagtime(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnButtonAdd();
	afx_msg void OnButtonDel();
	afx_msg void OnButtonEdit();
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnBagassignAdd();
	afx_msg void OnBagassignAddbagdevice();
	afx_msg void OnBagassignDelete();
	afx_msg void OnBagassignPercent();
	afx_msg void OnButtonSave();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnPeoplemoverNew();
	afx_msg void OnPeoplemoverDelete();
	afx_msg void OnPeoplemoverChange();
	afx_msg void OnItemchangedListLagtime(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnRadioFlightpriority();
	afx_msg void OnRadioGatePriority();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnItemchangedListLagtime2(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnClickListLagtime2(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BAGDEVDLG_H__A94FEA84_BCAD_4BBF_8744_BFFEBFD9EEAB__INCLUDED_)
