#pragma once
#include "../mfcexcontrol/listctrlex.h"
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
class MFCEXCONTROL_API CATCRWPairListCtrl :public CListCtrlEx
{
public:
	CATCRWPairListCtrl(void);
	virtual ~CATCRWPairListCtrl(void);
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CConDBListCtrlWithCheckBox)
public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpdis);
	//}}AFX_VIRTUAL

	// Implementation
public:
	CHECKSTATUS GetItemStatus(int nItem,int nSubItem);
	void SetItemStatus(int nItem,int nSubItem,CHECKSTATUS nStatus);
public:
	CSize m_sizeCheck;
	HBITMAP m_hBitmapCheck;

	DECLARE_MESSAGE_MAP()
public:

	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
};
