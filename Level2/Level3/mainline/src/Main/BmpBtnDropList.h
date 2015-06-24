#pragma once

#include "ListWnd.h"
class CBmpBtnDropList : public CBitmapButton  
{
public:
	BOOL Create(CRect rect,UINT nID,CWnd* pParent);
	void DisplayListWnd();
	CBmpBtnDropList();
	virtual ~CBmpBtnDropList();
public:
	BOOL CreateListWnd();
	void HideCtrl();
	void CalculateDroppedRect(LPRECT lpDroppedRect);
	CListWnd	m_listWnd;
	void SetShapeSetIndex(int nShapeSetIndex);
protected:
	int m_nShapeSetIndex;
protected:
	//{{AFX_MSG(CBmpBtnDropList)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

};


