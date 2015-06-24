#pragma once


// CAirsideReportChildFrameSplit frame with splitter

class CAirsideReportChildFrameSplit : public CMDIChildWnd
{
	DECLARE_DYNCREATE(CAirsideReportChildFrameSplit)
protected:
	CAirsideReportChildFrameSplit();           // protected constructor used by dynamic creation
	virtual ~CAirsideReportChildFrameSplit();

	//CSplitterWnd m_wndSplitter;
public:
	CSplitterWnd    m_wndSplitter1;		// Horizontal splitter
	CSplitterWnd    m_wndSplitter2;		// Vertical splitter
protected:
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
	virtual void OnUpdateFrameMenu (BOOL bActivate, CWnd* pActivateWnd, HMENU hMenuAlt);

	DECLARE_MESSAGE_MAP()
	virtual void OnUpdateFrameTitle(BOOL bAddToTitle);
};


