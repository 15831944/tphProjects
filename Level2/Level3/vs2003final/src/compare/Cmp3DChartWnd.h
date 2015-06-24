#pragma once

#import "..\..\lib\3DChart.tlb" no_auto_exclude
// C3DChartWnd

class CCmp3DChartWnd : public CWnd
{
	DECLARE_DYNAMIC(CCmp3DChartWnd)

public:
	CCmp3DChartWnd();
	virtual ~CCmp3DChartWnd();

	bool Create(LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);
	N3DCHARTLib::IDualChartPtr& Get3dChartPtr()
	{
		return m_pChart;
	}
	HWND Get3DChartWnd()
	{
		return m_h3DWnd;
	}
protected:
	DECLARE_MESSAGE_MAP()

	N3DCHARTLib::IDualChartPtr m_pChart;
	HWND m_h3DWnd;
};


