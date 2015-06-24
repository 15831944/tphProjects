#include "StdAfx.h"
#include ".\comparativereportctrl.h"

CComparativeReportCtrl::CComparativeReportCtrl()
: m_ReportManager(this)
{
	m_emReportType = UnknownType;
	m_emLengthUnit = ASUM_LEN_METERS;
}

CComparativeReportCtrl::~CComparativeReportCtrl()
{
}

bool CComparativeReportCtrl::Create(LPCTSTR lpszWindowName, DWORD dwStyle,
								const RECT& rect, CWnd* pParentWnd, UINT nID)
{
	return m_wndReportFrame.Create(lpszWindowName, dwStyle, rect, pParentWnd, nID);
}


void CComparativeReportCtrl::UpdateReport()
{
	m_ReportManager.InitUpdate();
	m_ReportManager.GenerateResult();
	m_ReportManager.DisplayReport();
}

void CComparativeReportCtrl::PrintListData()
{
}

void CComparativeReportCtrl::PrintGraph()
{
}

void CComparativeReportCtrl::ExportGraph(LPCTSTR lpszFilePath)
{
}