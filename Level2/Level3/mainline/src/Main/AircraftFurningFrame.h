#pragma once

#include "XPStyle/StatusBarXP.h"
// CAircraftFurningFrame frame

class CAircraftFurningFrame : public CMDIChildWnd
{
	DECLARE_DYNCREATE(CAircraftFurningFrame)
protected:
	CAircraftFurningFrame();           // protected constructor used by dynamic creation
	virtual ~CAircraftFurningFrame();

protected:
	DECLARE_MESSAGE_MAP()
protected:
	CSplitterWnd m_wndSplitter1;
	CSplitterWnd m_wndSplitter2;

	CStatusBarXP  m_wndStatusBar;
public:
	virtual void OnUpdateFrameTitle(BOOL bAddToTitle);

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
public:
	afx_msg void OnDestroy();
};


