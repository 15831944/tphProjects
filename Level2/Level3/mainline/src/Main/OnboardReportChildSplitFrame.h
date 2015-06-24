#pragma once


// OnboardReportChildSplitFrame frame with splitter

class OnboardReportChildSplitFrame : public CMDIChildWnd
{
	DECLARE_DYNCREATE(OnboardReportChildSplitFrame)
protected:
	OnboardReportChildSplitFrame();           // protected constructor used by dynamic creation
	virtual ~OnboardReportChildSplitFrame();

	CSplitterWnd m_wndSplitter1;
public:
	CSplitterWnd m_wndSplitter2;	
protected:
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
	virtual void OnUpdateFrameMenu (BOOL bActivate, CWnd* pActivateWnd, HMENU hMenuAlt);

	DECLARE_MESSAGE_MAP()

	virtual void OnUpdateFrameTitle(BOOL bAddToTitle);
};


