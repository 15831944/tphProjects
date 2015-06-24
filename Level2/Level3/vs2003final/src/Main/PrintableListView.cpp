// PrintableListView.cpp : implementation file
//

#include "stdafx.h"
#include "termplan.h"
#include "PrintableListView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPrintableListView

IMPLEMENT_DYNCREATE(CPrintableListView, CListView)

CPrintableListView::CPrintableListView()
{
}

CPrintableListView::~CPrintableListView()
{
}


BEGIN_MESSAGE_MAP(CPrintableListView, CListView)
	//{{AFX_MSG_MAP(CPrintableListView)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPrintableListView drawing

void CPrintableListView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: add draw code here
}

/////////////////////////////////////////////////////////////////////////////
// CPrintableListView diagnostics

#ifdef _DEBUG
void CPrintableListView::AssertValid() const
{
	CListView::AssertValid();
}

void CPrintableListView::Dump(CDumpContext& dc) const
{
	CListView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CPrintableListView message handlers
	
void CPrintableListView::OnPrint(CDC* pDC, CPrintInfo* pInfo)
{
}

