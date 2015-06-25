#pragma once

#import "..\..\lib\3DChart.tlb" no_auto_exclude
// C3DChartWnd

class C3DChartWnd : public CWnd
{
	DECLARE_DYNAMIC(C3DChartWnd)

public:
	C3DChartWnd();
	virtual ~C3DChartWnd();

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


