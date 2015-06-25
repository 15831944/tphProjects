#pragma once
#include "Cmp3DChartWnd.h"
#include "CmpParametersWnd.h"
#include "ThreeDChartType.h"

class CCmpReportFrameWnd;
class CCmpReportManager;

class CCmpDisplay
{
public:
	CCmpDisplay() ;
	virtual ~CCmpDisplay();

	BOOL InitAllWnds();
	void UpdateAllWnds();
	void UpdateParaWnd();
	void UpdateListWnd();
	void Update3DChart();
	void Change3DChart(ThreeDChartType enmChartType);

	void SetReportFrameWnd(CCmpReportFrameWnd* pReportFrameWnd) { m_pReportFrameWnd = pReportFrameWnd; }
	void SetReportManager(CCmpReportManager* pRptManager) { m_pReportManager = pRptManager; }

	void PrintListData();
	void PrintGraph();
	void OnDrawListData(CDC *pDC, PRINTDLGA *pInfo);
	void ExportGraph(BSTR bstrFileName);

protected:
	CCmpReportFrameWnd* m_pReportFrameWnd;
	CCmpReportManager* m_pReportManager;
	CCmpParametersWnd m_wndParas;
	CCmp3DChartWnd m_wnd3dChart;
	CListCtrl m_wndListCtrl;

	std::vector<CListCtrl*> m_vPWndListCtrl;
	std::vector<CCmp3DChartWnd*> m_vPWnd3dChart;

	ThreeDChartType m_chartType;
};
