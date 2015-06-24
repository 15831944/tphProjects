#pragma once

#include "CmpReportFrameWnd.h"
#include "CmpReportManager.h"
#include "ThreeDChartType.h"

class CCmpReportCtrl
{
public:
	CCmpReportCtrl();
	~CCmpReportCtrl();

	bool Create(LPCTSTR lpszWindowName, DWORD dwStyle,	const RECT& rect, CWnd* pParentWnd, UINT nID);

	void InitWindow();
	void UpdateAllWnds();

	CCmpReportFrameWnd* GetReportFrameWnd()
	{
		return &m_wndReportFrame;
	}

	CCmpReportManager* GetReportManager()
	{
		return &m_ReportManager;
	}

	ThreeDChartType GetChartType()
	{
		return m_enmChartType;
	}

	void SetChartType(ThreeDChartType enmChartType);

	void PrintListData();
	void PrintGraph();
	void ExportGraph(LPCTSTR lpszFilePath);

private:
	CCmpReportFrameWnd m_wndReportFrame;
	CCmpReportManager m_ReportManager;
	ThreeDChartType m_enmChartType;
};
