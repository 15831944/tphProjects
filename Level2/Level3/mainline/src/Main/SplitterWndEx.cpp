// SplitterWndEx.cpp : implementation file
//

#include "stdafx.h"
#include "termplan.h"
#include "SplitterWndEx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSplitterWndEx

CSplitterWndEx::CSplitterWndEx()
{
}

CSplitterWndEx::~CSplitterWndEx()
{
}


BEGIN_MESSAGE_MAP(CSplitterWndEx, CSplitterWnd)
	//{{AFX_MSG_MAP(CSplitterWndEx)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CSplitterWndEx message handlers

void CSplitterWndEx::OnDrawSplitter( CDC* pDC, ESplitType nType, const CRect& rect )
{
	int x_ActivePane, y_ActivePane;
	COLORREF hilightcolor = RGB(55,55,110);
	
	GetActivePane(&x_ActivePane, &y_ActivePane);
	if( ((GetRowCount()>1) ||(GetColumnCount()>1)) && (nType == splitBorder)) {
		int pRow = 0;
		int pCol = 0;
		if(rect.top) {
			pRow = 1;	
		}
		if(rect.left) {
			pCol = 1;
		}
		if((pCol == y_ActivePane) && (pRow == x_ActivePane)) {
			if (pDC == NULL)
			{
				RedrawWindow(rect, NULL, RDW_INVALIDATE|RDW_NOCHILDREN);
				return; 		
			}
			ASSERT_VALID(pDC);	
			CRect r = rect;
			pDC->Draw3dRect(r, hilightcolor, hilightcolor);
			int dx = -GetSystemMetrics(SM_CXBORDER);
			int dy = -GetSystemMetrics(SM_CYBORDER);
			r.InflateRect(dx,dy);
			pDC->Draw3dRect(r, hilightcolor, hilightcolor);
			return;
		}
	}

	CSplitterWnd::OnDrawSplitter(pDC,nType,rect);
}

void CSplitterWndEx::RefreshSplitBars(void)
{
	CRect rectInside;
	GetInsideRect(rectInside);
	DrawAllSplitBars(NULL, rectInside.right, rectInside.bottom);
}


BOOL CSplitterWnd2X2::Create( CWnd* pParentWnd, CCreateContext* pContext )
{
	return CSplitterWndEx::Create(pParentWnd, 2, 2, CSize(10, 10), pContext, WS_CHILD | WS_VISIBLE | SPLS_DYNAMIC_SPLIT);
}

BOOL CSplitterWnd2X2::SplitRow( int cyBefore )
{
	int nRowCount = GetRowCount();
	if (nRowCount>1)
	{
		CRect rect;
		GetWindowRect(rect);

		m_pRowInfo[0].nIdealSize = cyBefore;
		m_pRowInfo[1].nIdealSize = rect.Height() - cyBefore;
		RecalcLayout();
		return TRUE;
	}
	else
		return CSplitterWndEx::SplitRow(cyBefore);
}

BOOL CSplitterWnd2X2::SplitColumn( int cxBefore )
{
	int nColCount = GetColumnCount();
	if (nColCount>1)
	{
		CRect rect;
		GetWindowRect(rect);
		m_pColInfo[0].nIdealSize = cxBefore;
		m_pColInfo[1].nIdealSize = rect.Width() - cxBefore;
		RecalcLayout();
		return TRUE;
	}
	else
		return CSplitterWndEx::SplitColumn(cxBefore);
}

