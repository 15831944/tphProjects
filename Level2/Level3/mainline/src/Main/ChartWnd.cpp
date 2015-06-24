// ChartWnd.cpp : implementation file
//

#include "stdafx.h"
#include "ChartWnd.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CChartWnd

IMPLEMENT_DYNCREATE(CChartWnd, CFormView)

CChartWnd::CChartWnd()
: CFormView(CChartWnd::IDD)
{
	iWidthPerHour=40;
	//{{AFX_DATA_INIT(CChartWnd)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CChartWnd::~CChartWnd()
{/*
	//destroy the windows
	for( int i=0; i<MAX_NUM_ROWS_ONSCREEN; i++ )
	{
		if(m_pRowWnd[i]!=NULL)
		{
			delete m_pRowWnd[i];
		}
	}*/
}

void CChartWnd::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CChartWnd)
	// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CChartWnd, CFormView)
//{{AFX_MSG_MAP(CChartWnd)
ON_WM_CREATE()
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CChartWnd diagnostics

#ifdef _DEBUG
void CChartWnd::AssertValid() const
{
	CFormView::AssertValid();
}

void CChartWnd::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CChartWnd message handlers

int CChartWnd::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CFormView::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here
	CreateRowWindows();
	return 0;
}

void CChartWnd::Clear()
{
	
}

void CChartWnd::CreateRowWindows()
{/*
	CRect rectClient;
	GetClientRect( rectClient );
	rectClient.bottom = rectClient.top +GANNROW_HEIGHT/2;
	
	// create time line
	for( int i=0; i<MAX_NUM_ROWS_ONSCREEN; i++ )
	{
		rectClient.top = rectClient.bottom;
		rectClient.bottom = rectClient.top + GANNROW_HEIGHT;
		rectClient.right=120*24+2;
		m_pRowWnd[i] = new CRowWnd;
		m_pRowWnd[i]->Create( NULL, "CGanntRow", WS_CHILD|WS_VISIBLE, rectClient, this, IDD_FORMVIEW, NULL);
	}*/
}

void CChartWnd::OnDraw(CDC* pDC) 
{/*
	// TODO: Add your specialized code here and/or call the base class
	extern CBottomFormView * pBottomFormView;
	if(pBottomFormView==NULL || pBottomFormView->m_pScrollBar ==NULL)
	{
		return;
	}
	extern int g_Zoom;
    iWidthPerHour=g_Zoom;//pBottomFormView->m_ScaleSliderCtrl.GetPos();
	CRect rc(0,0,24*iWidthPerHour,40);
	pDC->DrawEdge( &rc, EDGE_SUNKEN, BF_BOTTOM);
	rc.bottom-=2;
	pDC->DrawEdge( &rc, EDGE_RAISED, BF_BOTTOM);
	CString s;
	rc=CRect(0,35,0,40);
	for(int j=0;j<=24;j++)
	{
		pDC->DrawEdge( &rc,EDGE_SUNKEN, BF_LEFT);
		rc.left-=1;
		pDC->DrawEdge( &rc, EDGE_RAISED, BF_LEFT);
		rc.left+=iWidthPerHour+1;
		s.Format("%d",j);
		pDC->TextOut(j*iWidthPerHour-4,40-20,s);
		pDC->ExtTextOut(j*iWidthPerHour-4,40-20,ETO_OPAQUE,CRect(j*iWidthPerHour-4,40-20,j*iWidthPerHour-4+iWidthPerHour,40-20+6),s,NULL);
	}
	SetScrollSizes(MM_TEXT,CSize(j*iWidthPerHour,20));
	for(int t=0;t<MAX_NUM_ROWS_ONSCREEN;t++)
	{
		m_pRowWnd[t]->Invalidate();
	}
//	extern CGateList g_GateList;
//	extern CBottomFormView * pBottomFormView;
//	if(pBottomFormView==NULL || pBottomFormView->m_pScrollBar ==NULL)
//	{
//		return;
//	}
//	int nCount=g_GateList.m_pList.GetCount()-pBottomFormView->m_pScrollBar->GetScrollPos();
//	for(int t=0;t<nCount;t++)
//	{
//		if(m_pRowWnd[t]!=NULL)
//		{
//			m_pRowWnd[t]->ShowWindow(SW_SHOW);
//		}
//		
//	}
//	for(t=nCount;t<MAX_NUM_ROWS_ONSCREEN;t++)
//	{
//		if(m_pRowWnd[t]!=NULL)
//		{
//			m_pRowWnd[t]->ShowWindow(SW_HIDE);
//		}
//	}*/
}

void CChartWnd::CalcWindowRect(LPRECT lpClientRect, UINT nAdjustType) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	CFormView::CalcWindowRect(lpClientRect, nAdjustType);
}

void CChartWnd::SetData(int _nStartIndex)
{/*
	for( int i=0; i<MAX_NUM_ROWS_ONSCREEN; i++ )
	{
		m_pRowWnd[i]->iGate=i+_nStartIndex;
		extern CGateList g_GateList;
		if(i<g_GateList.m_pList.GetCount()-_nStartIndex)
		{
			m_pRowWnd[i]->ShowWindow( SW_SHOW );
		}
		else
		{
			m_pRowWnd[i]->ShowWindow( SW_HIDE);
		}
		
	}*/
}
