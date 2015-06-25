#include "stdafx.h"
#include "CmpReportListView.h"
#include "CompareReportDoc.h"
#include ".\compare\ComparativeList.h"

#define IDC_CMPREPORT_LISTCTRL 0x01
#define MENU_MAXIMIZE					0x101
#define MENU_NORMAL					0x102
#define MENU_PRINT					0x103
#define MENU_EXPORT					0x104
#define MENU_UNAVAILABLE		0x200

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
	ON_WM_CONTEXTMENU()
	ON_COMMAND_RANGE(MENU_MAXIMIZE, MENU_UNAVAILABLE, OnChooseMenu)
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

	CComparativeProject* pCompProj = m_pCmpReport->GetComparativeProject();
	const CComparativeReportResultList &crrList = pCompProj->GetCompReportResultList();
	const CmpReportResultVector& vReport = crrList.GetReportResult();

	CString curReportName = m_pCmpReport->GetCurReport();
	CComparativeList cmpList(m_pCmpReport->GetTerminal(), m_wndListCtrl);
	for(int i = 0; i < static_cast<int>(vReport.size()); i++)
	{
		CString reportName = vReport[i]->GetCmpReportName();
		if(reportName.CompareNoCase(curReportName) == 0)
		{
			cmpList.RefreshData(*vReport[i]);
			break;
		}
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
		DWORD dwStyle = m_wndListCtrl.GetExtendedStyle();
		dwStyle |= LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES;
		m_wndListCtrl.SetExtendedStyle( dwStyle );

		m_ctlHeaderCtrl.SubclassWindow(m_wndListCtrl.GetHeaderCtrl()->m_hWnd );
	}

	return 0;
}

void CCmpReportListView::OnContextMenu(CWnd* pWnd, CPoint point)
{
	CPoint pt = point;
	CMenu menu;
	menu.CreatePopupMenu();
	menu.AppendMenu(MF_STRING | MF_ENABLED , MENU_MAXIMIZE, _T("Maximize"));
	menu.AppendMenu(MF_STRING | MF_ENABLED , MENU_NORMAL, _T("Normal"));
	menu.AppendMenu(MF_STRING | MF_ENABLED , MENU_PRINT, _T("Print"));
	menu.AppendMenu(MF_STRING | MF_ENABLED , MENU_EXPORT, _T("Export"));
	menu.TrackPopupMenu(TPM_LEFTALIGN,point.x,point.y,this);
	return;
}

void CCmpReportListView::OnChooseMenu( UINT nID )
{
	if(nID == MENU_UNAVAILABLE)
		return;
	switch(nID)
	{
	case MENU_MAXIMIZE:
		MessageBox("TODO: Maximize.");
		break;
	case MENU_NORMAL:
		MessageBox("TODO: Normal.");
		break;
	case MENU_PRINT:
		MessageBox("TODO: Print.");
		break;
	case MENU_EXPORT:
		MessageBox("TODO: Export.");
		break;
	default:
		break;
	}
	return;

}
