#pragma once
#include "listctrlex.h"

#ifndef __CHECHSTATUES__
#define __CHECHSTATUES__
enum CHECKSTATUS
{
	DISABLEUNCHECKED	= 0,
	DISABLECHECKED		= 1,
	UNCHECKED			= 2,
	CHECKED				= 3,
};
#endif
static const int ID_DATETIMEPICKER_TIME = 101;
static const int WM_LISTCTRL_SETDATETIME = WM_USER + 533;

class MFCEXCONTROL_API CListCtrlExEx :public CListCtrlEx
{
public:
	CListCtrlExEx(void);
	virtual ~CListCtrlExEx(void);
protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
public:
	CSize m_sizeCheck;
	HBITMAP m_hBitmapCheck;

	CHECKSTATUS GetItemStatus(int nItem,int nSubItem);
	void SetItemStatus(int nItem,int nSubItem,CHECKSTATUS nStatus);
	void SetDateTimeColumnFormat(int nColumn, LPCTSTR lpszFormat);
	void EditDateTimeLabelEx(int nItem, int nCol, LPCTSTR lpszFormat, SYSTEMTIME* pDateTime);

public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpdis/*lpDrawItemStruct*/);

	DECLARE_MESSAGE_MAP()

	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
};
