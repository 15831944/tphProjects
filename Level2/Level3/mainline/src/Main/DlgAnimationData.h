#if !defined(AFX_DLGANIMATIONDATA_H__CC32AB62_A252_4065_A29E_F510E21660D0__INCLUDED_)
#define AFX_DLGANIMATIONDATA_H__CC32AB62_A252_4065_A29E_F510E21660D0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgAnimationData.h : header file
//
#include "AnimationData.h"
class CTermPlanDoc;
/////////////////////////////////////////////////////////////////////////////
// CDlgAnimationData dialog

class CDlgAnimationData : public CDialog
{
// Construction
public:
	CImageList m_imageList;
	CTermPlanDoc* m_pDoc;
	CDlgAnimationData(CTermPlanDoc* pDoc,AnimationData& animData, CWnd* pParent = NULL);   // standard constructor
	~CDlgAnimationData();
// Dialog Data
	//{{AFX_DATA(CDlgAnimationData)
	enum { IDD = IDD_ANIMATIONDATA };
	CDateTimeCtrl	m_EndTimeCtrl;
	CDateTimeCtrl	m_StartTimeCtrl;
	CComboBox	m_StartDayCombCtrl;
	CComboBox	m_EndDayCombCtrl;
	COleDateTime	m_tEndTime;
	COleDateTime	m_tStartTime;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgAnimationData)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgAnimationData)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnDatetimechangeTimeStart(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDatetimechangeTimeEnd(NMHDR* pNMHDR, LRESULT* pResult);
	virtual void OnOK();
	//afx_msg void OnDatetimechangeTimeStart2(NMHDR* pNMHDR, LRESULT* pResult);
	//afx_msg void OnDatetimechangeTimeEnd2(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelchangeStartdaycomb();
	afx_msg void OnSelchangeEnddaycomb();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	long m_nMaxTime;		//max end time
	long m_nMinTime;		//min start time
	long m_nStartTime;		//start time
	long m_nEndTime;		//end time
	long m_nMinSelectTime; //max select time range.
	long m_nMaxSelectTime;

	BOOL m_bDragStart;		//are we dragging the start time?
	BOOL m_bDragEnd;		//are we dragging the end time?
	
	//CPoint m_ptMarkerClickOffset;	//offset between marker top-left and point where marker was clicked
	CPoint m_ptStartDrag;

	AnimationData* m_pAnimData;

	HBITMAP m_hMarkerBitmap;

	CRect m_rTimeBar;

private:
	bool m_bAbsDate;
	int m_StartDayIndx, m_EndDayIndx;
	double TB_PIXELSPERHOUR, TB_MINUTESPERPIXEL;
	int	m_PIXELSPERDay;
	int m_LeftPos, m_RightPos;

public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGANIMATIONDATA_H__CC32AB62_A252_4065_A29E_F510E21660D0__INCLUDED_)
