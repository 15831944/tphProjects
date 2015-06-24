#if !defined(AFX_DLGFLOORADJUST_H__E3803BB6_A96B_4772_B103_A219DB012C8C__INCLUDED_)
#define AFX_DLGFLOORADJUST_H__E3803BB6_A96B_4772_B103_A219DB012C8C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgFloorAdjust.h : header file
//
#include "resource.h"
#include "XPStyle\tooltip2.h"


class CTermPlanDoc;
/////////////////////////////////////////////////////////////////////////////
// CDlgFloorAdjust dialog

class CDlgFloorAdjust : public CDialog
{
// Construction
public:
	CDlgFloorAdjust(CTermPlanDoc* pDoc, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgFloorAdjust)
	enum { IDD = IDD_DIALOG_FLOORADJUST };
	CButton	m_btnZoomOut;
	CButton	m_btnZoomIn;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgFloorAdjust)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	int m_nMaxH; //maximum height shown on bar in cm (internal units);
	int m_nUnitsPerPixel;
	int m_nSelectedFloor; //-1:none
    double m_scale;
	int nHStep;
	CTermPlanDoc* m_pDoc;
	CPoint m_ptLast;
	CToolTip2 m_ToolTip;
	BOOL m_bMouseOver;
	BOOL m_bMovingFloor;
	CRect m_rcClient;
   
	int HitTest(CPoint pt);
	void MoveFloor(double delta);
	void GetBarRect(LPRECT rc);
	void Update3DView();
	void ShowToolTip(LPCTSTR string);
	void RequestHoverMsg();
	int UnitsToPixels(double dInUnits);


	//change scale delta
	void ModifyScale(double delta);
	//at the end of modify scale data
	void OnEndModifyScale();

	// Generated message map functions
	//{{AFX_MSG(CDlgFloorAdjust)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnButtonZoomIn();
	afx_msg void OnButtonZoomOut();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	afx_msg LRESULT OnMouseHover (WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMouseLeave(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGFLOORADJUST_H__E3803BB6_A96B_4772_B103_A219DB012C8C__INCLUDED_)
