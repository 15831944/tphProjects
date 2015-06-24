#if !defined(AFX_DLGDBAIRLINE_H__3C89AF41_CDF7_4384_AF17_6671496F7933__INCLUDED_)
#define AFX_DLGDBAIRLINE_H__3C89AF41_CDF7_4384_AF17_6671496F7933__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgDBAirline.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgDBAirline dialog
#include <MFCExControl/SplitterControl.h>
#include "DlgSubairlineProperties.h"
#include "afxwin.h"
#include "CoolBtn.h"
#include "../MFCExControl/SortAndPrintListCtrl.h"

class Terminal;
class CAirlinesManager;
class CAirportDatabase;
class CAirline ;


class CDlgDBAirline : public CDialog
{
// Construction
public:
	CDlgDBAirline(BOOL bMenu,Terminal* _pTerm, CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgDBAirline();

	void setAirlinesMan( CAirlinesManager* _pAirlinesMan );
	void OnClickMultiBtn();

// Dialog Data
	//{{AFX_DATA(CDlgDBAirline)
	enum { IDD = IDD_AIRLINEDB };
	CStatic	m_cstLable;
	CButton	m_btnOk;
	CSortAndPrintListCtrl	m_lstAirlines;
	CListBox	m_lstSubAirlines;
	CSortAndPrintListCtrl	m_lstSALMembers;
	CButton	m_btnCheck;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgDBAirline)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CDlgDBAirline)
	virtual BOOL OnInitDialog();
	afx_msg void OnItemChangedSALList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDestroy();
	afx_msg void OnCtxNewAirline();
	afx_msg void OnCtxEditAirline();
	afx_msg void OnCtxNewSubairline();
	afx_msg void OnCtxAddAirlines();
	afx_msg void OnBegindragLstAirlines(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnCtxDeleteSubairline();
	afx_msg void OnCtxDeleteAirline();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnClose();
	afx_msg void OnClickLstAirlines(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelchangedTreeSubairlines(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg void OnBnClickedCheckFlightSchedule();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

protected:
	virtual void OnOK();
	virtual void OnCancel();
	void AddDraggedItemToList();
	void DoResize(int nDelta);
	void PopulateSubairlineList();
	void PopulateSubairlineMembersList(int _nItem);
	void PopulateAirlineList();
	void AddAirlineItem( CAirline* pAirline) ;
	void EditAirlineItem(int selItem , CAirline* pAirline) ;
	void FreeItemMemory();

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnLbnSelchangeListSubairlines();
	afx_msg void OnBnClickedButtonLoaddefaultAirline();

	CAirportDatabase*	m_pCurAirportDatabase;
	CAirlinesManager*	m_pAirlinesMan;

	CCoolBtn			m_button_Load;

private:
	Terminal*		m_pTerm;
	BOOL			m_IsEdit ;
	BOOL			m_commit_Operation ;
	BOOL			m_Load_Operation ;
	BOOL			m_bDragging;
	BOOL			m_bMenu;
	CPoint			m_ptLastMouse;
	int				m_nItemDragSrc;
	int				m_nItemDragDes;


	CToolBar			m_wndToolBarBottom;
	CToolBar			m_wndToolBarTop;
	CButton				m_buttonCancel;
	CImageList			m_ilNodes;
	CImageList*			m_pDragImage;
	CSplitterControl	m_wndSplitter;

protected:
	afx_msg void OnLoadFromTemplate();
	afx_msg void OnSaveAsTemplate();
};
class CDlgDBAirlineForDefault : public CDlgDBAirline
{
public:
	CDlgDBAirlineForDefault(BOOL bMenu,Terminal* _pTerm,CAirportDatabase* _theAirportDB, CWnd* pParent = NULL);
	~CDlgDBAirlineForDefault();
	BOOL OnInitDialog() ;
	virtual void OnOK() ;
	virtual void OnCancel() ;
protected:
	// used CDlgDBAirline::m_pCurAirportDatabase instead
// 	CAirportDatabase* m_pCurAirportDatabase ;
};
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGDBAIRLINE_H__3C89AF41_CDF7_4384_AF17_6671496F7933__INCLUDED_)
