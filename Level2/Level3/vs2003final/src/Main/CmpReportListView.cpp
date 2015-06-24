#include "stdafx.h"
#include "TermPlan.h"
#include "CmpReportListView.h"
#include "CompareReportDoc.h"
#include ".\compare\ComparativeList.h"



#define  IDC_CMPREPORT_LISTCTRL 0x01

IMPLEMENT_DYNCREATE(CCmpReportListView, CFormView)

CCmpReportListView::CCmpReportListView()
	: CFormView(CCmpReportListView::IDD)
{
	m_pCmpReport = NULL;
	m_categoryType = -1;
}

CCmpReportListView::~CCmpReportListView()
{
}

void CCmpReportListView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CCmpReportListView, CFormView)
	ON_WM_CREATE()
	ON_WM_SIZE()
END_MESSAGE_MAP()

#ifdef _DEBUG
void CCmpReportListView::AssertValid() const
{
	CFormView::AssertValid();
}

void CCmpReportListView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG


void CCmpReportListView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();
	m_pCmpReport = ((CCompareReportDoc*)m_pDocument)->GetCmpReport();
}

void CCmpReportListView::OnSize(UINT nType, int cx, int cy)
{
	CFormView::OnSize(nType, cx, cy);

	if (m_wndListCtrl.GetSafeHwnd())
	{
		CRect rc;
		GetClientRect(&rc);
		m_wndListCtrl.MoveWindow(&rc);
	}

	SetScaleToFitSize(CSize(cx,cy));
}
void CCmpReportListView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint)
{
	if(!IsWindowVisible())
		return;
	switch(m_categoryType)
	{
	case 0:
	case -1:
		{
			CComparativeProject* pCompProj = m_pCmpReport->GetComparativeProject();
			CInputParameter* inputParam = pCompProj->GetInputParam();
			CReportsManager* reportsManager = inputParam->GetReportsManagerPtr();
			std::vector<CReportToCompare>& vReportList = reportsManager->GetReportsList();

			CModelsManager* modelsManager = inputParam->GetModelsManagerPtr();
			std::vector<CModelToCompare *>& vModelList = modelsManager->GetModelsList();
			const CComparativeReportResultList &crrList = pCompProj->GetCompReportResultList();
			const CmpReportResultVector& vReport = crrList.GetReportResult();

			for(int i = 0; i < static_cast<int>(vReport.size()); i++)
			{
				m_wndListCtrl.SetExtendedStyle(LVS_EX_GRIDLINES);
				CComparativeList cmpList(m_pCmpReport->GetTerminal(), m_wndListCtrl, vModelList);
				cmpList.RefreshData(*vReport[i]);
				break;
			}
		}
		break;
	default:
		break;
	}
}
int CCmpReportListView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFormView::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rtEmpty;
	rtEmpty.SetRectEmpty();
	if(m_wndListCtrl.Create(LBS_NOTIFY|LBS_HASSTRINGS|WS_CHILD|WS_VISIBLE|WS_BORDER|WS_VSCROLL,
		rtEmpty, this,IDC_CMPREPORT_LISTCTRL))
	{
		DWORD dwStyle =dwStyle = m_wndListCtrl.GetExtendedStyle();
		dwStyle |= LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES;
		m_wndListCtrl.SetExtendedStyle( dwStyle );

		m_ctlHeaderCtrl.SubclassWindow(m_wndListCtrl.GetHeaderCtrl()->m_hWnd );
	}

	return 0;
}
