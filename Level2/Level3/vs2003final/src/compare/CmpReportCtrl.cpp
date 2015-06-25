#include "StdAfx.h"
#include "cmpreportctrl.h"
#pragma warning(disable:4355)

CCmpReportCtrl::CCmpReportCtrl()
: m_ReportManager(this)
{
	m_enmChartType = CMPBar_2D;
}

CCmpReportCtrl::~CCmpReportCtrl()
{
}

bool CCmpReportCtrl::Create(LPCTSTR lpszWindowName, DWORD dwStyle,
								const RECT& rect, CWnd* pParentWnd, UINT nID)
{
	return m_wndReportFrame.Create(lpszWindowName, dwStyle, rect, pParentWnd, nID);
}

void CCmpReportCtrl::InitWindow()
{
	m_ReportManager.InitUpdate();
}

void CCmpReportCtrl::UpdateAllWnds()
{
	m_ReportManager.UpdateAllWnds();
}

void CCmpReportCtrl::PrintListData()
{
}

void CCmpReportCtrl::PrintGraph()
{
}

void CCmpReportCtrl::ExportGraph(LPCTSTR lpszFilePath)
{
}

void CCmpReportCtrl::SetChartType(ThreeDChartType enmChartType)
{
	m_ReportManager.Change3DChart(enmChartType);
	m_enmChartType = enmChartType;
}