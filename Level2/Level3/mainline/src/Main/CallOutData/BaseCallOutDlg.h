#pragma once
#include "afxcmn.h"
#include "..\..\Common\elaptime.h"
#include "..\..\AirsideReport\MSChart\mschart.h"
#include "..\..\glChart\GLChartCtrl.h"
// CBaseCallOutDlg dialog
#define UPDATE_TIME WM_USER + 0x0f
#define UPDATE_CHART WM_USER + 0xFF
#include "..\DragDialog.h"
class CTermPlanDoc;
class CBaseCallOutDlg : public CDragDialog
{
	DECLARE_DYNAMIC(CBaseCallOutDlg)

public:
	CBaseCallOutDlg(CTermPlanDoc* _TermPlanDoc,CWnd* pParent = NULL);   // standard constructor
	virtual ~CBaseCallOutDlg();

// Dialog Data
	enum { IDD = IDD_DIALOG_CALLOUT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

	enum EVET_TYPE {EVENT_REFRESH};
protected:
	CGLChartCtrl m_Chart ;

	ElapsedTime m_StartTime ;
	ElapsedTime m_EndTime ;
	ElapsedTime m_Interval ;
	ElapsedTime m_CurrentTime ;
	CString m_Processor ;
	CString m_Caption ;
	CStatic m_StaticProcssor ;
	CStatic m_StaticStartTime ;
	CStatic m_StaticEndTime ;
	CStatic m_StaticInterval ;
	CStatic m_StaticCurrentTime ;
public:
	

	void SetStartTime(const ElapsedTime& _time) { m_StartTime = _time ;};
	void SetEndTime(const ElapsedTime& _time) { m_EndTime = _time ;};
	void SetIntervalTime(const ElapsedTime& _time) { m_Interval = _time ;};
	void SetProcessorName(const CString& _ProName) { m_Processor = _ProName ;};
	void SetCaption(const CString& _Caption) { m_Caption = _Caption ;};
	void UpdateCurrentTime(const ElapsedTime& _time) ;

	CGLChartCtrl* GetChartCtrl() { return &m_Chart ;} ;
protected:
	virtual BOOL OnInitDialog() ;

	

	void OnSize(UINT nType, int cx, int cy) ;
	int OnCreate(LPCREATESTRUCT lpCreateStruct) ;
	void OnMouseMove(UINT nFlags, CPoint point);

	void OnLButtonDown(UINT nFlags, CPoint point) ;
	void OnLButtonUp(UINT nFlags, CPoint point) ;
	HBRUSH OnCtlColor( CDC* pDC, CWnd* pWnd, UINT nCtlColor );
	void OnClose();
	LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam) ;
protected:
	CPoint m_DlgPosition ;
	CPoint m_PriPoint ;
	BOOL m_ButtonDown ; 
	int m_OldX ;
	int m_OldY ;
	CButton m_CloseButton ;
	BOOL m_bMouseTracking ;
	CBrush m_brush;
	CBrush m_TitileBush ;
	CRect m_CurrentPosition ;
	BOOL m_isclose ;
	CTermPlanDoc* m_TermPlanDoc ;
public:
	void SetPosition(const CPoint& _point) { m_DlgPosition = _point ;} ;
	LRESULT OnMouseLeave(WPARAM wParam,LPARAM lParam);
	CRect GetPositionRect() { return m_CurrentPosition ;} ;

	BOOL IsClose() { return m_isclose ;};
	afx_msg void OnStnClickedStaticStartval();
	void OnCancel();
	void OnOK();
	afx_msg void OnStnClickedStaticTitle();
};
