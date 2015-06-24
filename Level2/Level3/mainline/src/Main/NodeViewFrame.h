#pragma once


// CNodeViewFrame frame
#include "./ChildFrameSplit.h"
#include "../Inputs/IN_TERM.H"
namespace MSV
{
class CNodeViewFrame : public CChildFrameSplit
{
	DECLARE_DYNCREATE(CNodeViewFrame)
protected:
	CNodeViewFrame();           // protected constructor used by dynamic creation
	virtual ~CNodeViewFrame();
public:
	void AdjustMSView(EnvironmentMode& mode);
	EnvironmentMode m_curMode;

private:
	CView *m_pTerminalView;
	CView *m_pLandsideView;
	CView *m_pAirsideView;
	CView *m_pOnBoardView;
	CView *m_pCargoView;
	CView *m_pEnvironmentView;
	CView *m_pFinancialView;
protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_OVERLAPPEDWINDOW, const RECT& rect = rectDefault, CMDIFrameWnd* pParentWnd = NULL, CCreateContext* pContext = NULL);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual void OnUpdateFrameMenu(BOOL bActive, CWnd* pActiveWnd, HMENU hMenuAlt);


protected:
	void HideViewsExceptThis(CView * pViewExcept);

	void ActiveViewOfMode(EnvironmentMode& envMode);
};


}