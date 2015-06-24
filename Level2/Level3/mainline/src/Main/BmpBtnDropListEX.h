// BmpBtnDropList.h: interface for the CBmpBtnDropList class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BMPBTNDROPLIST_H__F82D26B1_C314_44B6_9424_4FE0E7B7B97F__INCLUDED_)
#define AFX_BMPBTNDROPLIST_H__F82D26B1_C314_44B6_9424_4FE0E7B7B97F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ListWndEX.h"
class CBmpBtnDropListEX :
	public CBitmapButton
{
public:
	BOOL Create(CRect rect,UINT nID,CWnd* pParent);
	void DisplayListWnd();
	CBmpBtnDropListEX();
	virtual ~CBmpBtnDropListEX();
public:
	BOOL CreateListWnd();
	void HideCtrl();
	void CalculateDroppedRect(LPRECT lpDroppedRect);
	CListWndEX	m_listWnd;
	void SetShapeSetIndex(int nShapeSetIndex);
protected:
	int m_nShapeSetIndex;
protected:
	//{{AFX_MSG(CBmpBtnDropListEX)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

};

#endif // !defined(AFX_BMPBTNDROPLIST_H__F82D26B1_C314_44B6_9424_4FE0E7B7B97F__INCLUDED_)