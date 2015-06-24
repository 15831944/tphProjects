#pragma once
#include "XPStyle\tooltip2.h"

// CDlgOnboardDeckAdjust dialog
class CDeckManager;
class CAircraftLayout3DView;

class CDlgOnboardDeckAdjust : public CDialog
{
	DECLARE_DYNAMIC(CDlgOnboardDeckAdjust)

public:
	CDlgOnboardDeckAdjust(CDeckManager *pDeckManager, CView* pParent);   // standard constructor
	virtual ~CDlgOnboardDeckAdjust();

// Dialog Data
	enum { IDD = IDD_DIALOG_FLOORADJUST };

protected:
	CButton	m_btnZoomOut;
	CButton	m_btnZoomIn;

	int m_nMaxH; //maximum height shown on bar in cm (internal units);
	int m_nUnitsPerPixel;
	int m_nSelectedFloor; //-1:none
	int nHStep;
	CPoint m_ptLast;
	CToolTip2 m_ToolTip;
	BOOL m_bMouseOver;
	BOOL m_bMovingFloor;
	CRect m_rcClient;

	CDeckManager *m_pDeckManager;

	int HitTest(CPoint pt);
	void MoveDeck(double delta);
	void GetBarRect(LPRECT rc);
	void UpdateLayoutView();
	void ShowToolTip(LPCTSTR string);
	void RequestHoverMsg();
	int UnitsToPixels(double dInUnits);

	CAircraftLayout3DView *GetLayoutView()const;

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


protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
};
