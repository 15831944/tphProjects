#pragma once


// LandsideReportChildSplitFrame frame with splitter

class LandsideReportChildSplitFrame : public CMDIChildWnd
{
	DECLARE_DYNCREATE(LandsideReportChildSplitFrame)
protected:
	LandsideReportChildSplitFrame();           // protected constructor used by dynamic creation
	virtual ~LandsideReportChildSplitFrame();

public:
	CSplitterWnd m_wndSplitter1;
	CSplitterWnd m_wndSplitter2;

protected:
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);

	DECLARE_MESSAGE_MAP()
	virtual void OnUpdateFrameTitle(BOOL bAddToTitle);
};


