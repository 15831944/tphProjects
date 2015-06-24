#pragma once
#include "XPStyle/StatusBarXP.h"

class IRenderFrame : public CMDIChildWnd
{
	DECLARE_DYNCREATE(IRenderFrame)
public:
	IRenderFrame(void);

	//virtual void OnChangeMode(Eve);
	CStatusBarXP  m_wndStatusBar;
};
