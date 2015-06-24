#pragma once
#include "ToolBar24X24.h"
#include "AirRouteDropWnd.h"


class CAirRouteBar : public CToolBar24X24
{
public:
	CAirRouteBar();
	virtual ~CAirRouteBar();

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg BOOL OnDropDownPipe(NMHDR* pNMHDR, LRESULT* pResult);
	DECLARE_MESSAGE_MAP()

	CAirRouteDropWnd m_DropWnd;
};