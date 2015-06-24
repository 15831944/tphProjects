#include "StdAfx.h"
#include ".\repgraphviewbaseoperator.h"
#include "RepGraphView.h"
#include "TermPlanDoc.h"


CRepGraphViewBaseOperator::CRepGraphViewBaseOperator(CRepGraphView *pGraphView)
:m_pGraphView(pGraphView)
,m_MSChartCtrl(pGraphView->GetMSChartCtrl())
,m_comboChartType(pGraphView->GetComboCharType())
,m_comboChartSelect(pGraphView->GetComboCharSelect())
,m_comboSubType(pGraphView->GetComboSubType())
{
	m_pGraphView = pGraphView;
}

CRepGraphViewBaseOperator::~CRepGraphViewBaseOperator(void)
{
}


CMSChart1& CRepGraphViewBaseOperator::GetMSChartCtrl()
{
	ASSERT(m_pGraphView != NULL);
	return m_pGraphView->GetMSChartCtrl();
}
CTermPlanDoc * CRepGraphViewBaseOperator::GetTermPlanDoc()
{
	ASSERT(m_pGraphView != NULL);
	return (CTermPlanDoc*)(m_pGraphView->GetDocument());
}

void CRepGraphViewBaseOperator::ShowReportProcessFlow( MathResult& result )
{
	ASSERT(NULL);
}

void CRepGraphViewBaseOperator::LoadReportProcessFlow( CMathResultManager *pManager, enum ENUM_REPORT_TYPE _enumRepType )
{
	ASSERT(NULL);
}

void CRepGraphViewBaseOperator::OnSelchangeChartSelectCombo()
{
	ASSERT(NULL);
}

void CRepGraphViewBaseOperator::OnCbnSelChangeReportSubType()
{

}