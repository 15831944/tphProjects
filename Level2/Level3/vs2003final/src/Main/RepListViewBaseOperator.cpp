#include "StdAfx.h"
#include "resource.h"
#include ".\replistviewbaseoperator.h"
#include "RepListView.h"
#include "TermPlanDoc.h"
CRepListViewBaseOperator::CRepListViewBaseOperator(ARCGridCtrl *pListCtrl,CARCReportManager *pReportManager,CFormView *pListView)
{
	m_pListCtrl = pListCtrl;
	m_pReportManager = pReportManager;
	m_pListView = pListView;
}

CRepListViewBaseOperator::~CRepListViewBaseOperator(void)
{

}

ARCGridCtrl& CRepListViewBaseOperator::GetListCtrl()
{
	return *m_pListCtrl;
}
CReportParameter* CRepListViewBaseOperator::GetReportParameter()
{
	if(m_pReportManager)
		return m_pReportManager->GetReportPara();

	ASSERT(NULL);
	return NULL;
}
void CRepListViewBaseOperator::LoadReport(LPARAM lHint, CObject* pHint)
{

}

CTermPlanDoc * CRepListViewBaseOperator::GetTermPlanDoc()
{
	if(m_pListView)
		return (CTermPlanDoc*)(m_pListView->GetDocument());
	
	ASSERT(NULL);
	return NULL;
}

void CRepListViewBaseOperator::Invalidate( BOOL bErase /*= TRUE*/ )
{
	if(m_pListView)
		m_pListView->Invalidate(bErase);
	else
		ASSERT(NULL);
}

void CRepListViewBaseOperator::ResetAllContent()
{
	GetListCtrl().DeleteAllItems();
	GetListCtrl().SetFixedColumnCount(1);
	GetListCtrl().SetFixedRowCount(1);
	GetListCtrl().SetColumnToResize(FALSE);
	GetListCtrl().AutoSize();
}

void CRepListViewBaseOperator::OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult)
{

}

void CRepListViewBaseOperator::OnListviewExport()
{

}

void CRepListViewBaseOperator::LoadReportProcessFlow( CMathResultManager* pManager, enum ENUM_REPORT_TYPE _enumRepType )
{

}

void CRepListViewBaseOperator::ShowReportProcessFlow( MathResult& result )
{

}