#pragma once
#include "listctrlex.h"

class MFCEXCONTROL_API CListCtrlExExEx :public CListCtrlEx
{
public:
	CListCtrlExExEx(void);
	virtual ~CListCtrlExExEx(void);

	DECLARE_MESSAGE_MAP()
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
};