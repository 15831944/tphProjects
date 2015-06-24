#pragma once
#include "PipeDropWnd.h"
#include "ToolBar24X24.h"
#include "AirRouteDropWnd.h"
#include "AllProcessorTypeDropWnd.h"
class CPipeBar : public CToolBar24X24
{
public:
	CPipeBar();
	virtual ~CPipeBar();

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg BOOL OnDropDownPipe(NMHDR* pNMHDR, LRESULT* pResult);
	DECLARE_MESSAGE_MAP()

	CPipeDropWnd m_DropPipeWnd;
	CAirRouteDropWnd m_DropAirRouteWnd;
	CAllProcessorTypeDropWnd m_DropAllProcessorTypeWnd;
};
