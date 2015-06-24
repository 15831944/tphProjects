#pragma once
#include "XPStyle/StatusBarXP.h"

class CAircraftModelEditView;
class COnboardAircraftCommanView;
class COnBoardAircraftModelFrame :
	public CMDIChildWnd
{
public:
	DECLARE_DYNCREATE(COnBoardAircraftModelFrame)
protected:
	COnBoardAircraftModelFrame();           // protected constructor used by dynamic creation
	virtual ~COnBoardAircraftModelFrame();

	CSplitterWnd m_wndSplitter1;
	CAircraftModelEditView* m_pAircraftModelEditView;
	COnboardAircraftCommanView* m_pOnBoardAircraftCommandView;

	CStatusBarXP  m_wndStatusBar;
public:
	virtual void OnUpdateFrameTitle(BOOL bAddToTitle);

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);



	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnDestroy();

};
