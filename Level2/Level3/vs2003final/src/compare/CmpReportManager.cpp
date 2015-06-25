#include "StdAfx.h"

#include "CmpReportManager.h"
#include "CmpReportCtrl.h"
#include "CmpDisplay.h"

#pragma warning(disable:4150)

CCmpReportManager::CCmpReportManager(CCmpReportCtrl* pReportCtrl)
{
	m_pReportCtrl = pReportCtrl;
	m_pDisplay    = NULL;
}

CCmpReportManager::~CCmpReportManager()
{
	if (m_pDisplay)
	{
		delete m_pDisplay;
		m_pDisplay = NULL;
	}
}

void CCmpReportManager::InitUpdate()
{
	if (m_pDisplay)
	{
		delete m_pDisplay;
		m_pDisplay = NULL;
	}

	m_pDisplay = new CCmpDisplay;
	if (m_pDisplay != NULL)
	{
		m_pDisplay->SetReportFrameWnd(m_pReportCtrl->GetReportFrameWnd());
		m_pDisplay->SetReportManager(this);
		m_pDisplay->InitAllWnds();
	}
}

void CCmpReportManager::UpdateAllWnds()
{
	m_pDisplay->UpdateAllWnds();
}

void CCmpReportManager::GenerateResult()
{
}

void CCmpReportManager::DisplayReport()
{
	ATLASSERT(m_pReportCtrl != NULL);

	if (m_pDisplay == NULL)
		return;

	m_pDisplay->UpdateAllWnds();
}

void CCmpReportManager::Update3DChart()
{
	if (m_pDisplay == NULL)
		return;

	m_pDisplay->Update3DChart();

}

void CCmpReportManager::Change3DChart(ThreeDChartType enmChartType)
{
	if(m_pDisplay == NULL)
		return;

	m_pDisplay->Change3DChart(enmChartType);
}

void CCmpReportManager::ExportGraph(BSTR bstrFileName)
{
	if (m_pDisplay == NULL)
		return;
	m_pDisplay->ExportGraph(bstrFileName);

}

void CCmpReportManager::PrintGraph()
{
	if (m_pDisplay == NULL)
		return;
	m_pDisplay->PrintGraph();
}

void CCmpReportManager::PrintListData()
{
	if (m_pDisplay == NULL)
		return;

	m_pDisplay->PrintListData();

}