#pragma once

#include "ListCtrlEx.h"
// CListCtrlDef

class MFCEXCONTROL_API CListCtrlDef : public CListCtrlEx
{
	DECLARE_DYNAMIC(CListCtrlDef)

public:
	CListCtrlDef();
	virtual ~CListCtrlDef();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);

};


