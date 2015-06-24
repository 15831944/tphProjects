#pragma once

#import "..\..\lib\ReportFrame.tlb"

// CReportFrameWnd
class CCmpReportFrameWnd : public CWnd
{
public:
	CCmpReportFrameWnd();
	virtual ~CCmpReportFrameWnd();

	bool Create(LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);

	ReportFrameLib::IReportFrameCtrlPtr& GetReportFramPtr()
	{
		return m_ReportFramePtr;
	}

protected:
	DECLARE_MESSAGE_MAP()
	ReportFrameLib::IReportFrameCtrlPtr m_ReportFramePtr;
};


