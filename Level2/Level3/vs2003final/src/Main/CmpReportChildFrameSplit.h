#pragma once


class CCmpReportChildFrameSplit : public CMDIChildWnd
{
	DECLARE_DYNCREATE(CCmpReportChildFrameSplit)
protected:
	CCmpReportChildFrameSplit();
	virtual ~CCmpReportChildFrameSplit();

protected:
	DECLARE_MESSAGE_MAP()

public:
	CSplitterWnd m_wndSplitter1;
	CSplitterWnd m_wndSplitter2;	

protected:
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
	virtual void OnUpdateFrameTitle(BOOL bAddToTitle);
};

