// TermPlanView.cpp : implementation of the CTermPlanView class
//

#include "stdafx.h"
#include "TermPlan.h"

#include "TermPlanDoc.h"
#include "TermPlanView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTermPlanView

IMPLEMENT_DYNCREATE(CTermPlanView, CView)

BEGIN_MESSAGE_MAP(CTermPlanView, CView)
	//{{AFX_MSG_MAP(CTermPlanView)
	ON_WM_CREATE()
	ON_WM_LBUTTONDOWN()
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTermPlanView construction/destruction

CTermPlanView::CTermPlanView()
{
	// TODO: add construction code here
}

CTermPlanView::~CTermPlanView()
{
}

BOOL CTermPlanView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CTermPlanView drawing

void CTermPlanView::OnDraw(CDC* pDC)
{
	CTermPlanDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	// TODO: add draw code for native data here
}

/////////////////////////////////////////////////////////////////////////////
// CTermPlanView printing

BOOL CTermPlanView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CTermPlanView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CTermPlanView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

/////////////////////////////////////////////////////////////////////////////
// CTermPlanView diagnostics

#ifdef _DEBUG
void CTermPlanView::AssertValid() const
{
	CView::AssertValid();
}

void CTermPlanView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CTermPlanDoc* CTermPlanView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CTermPlanDoc)));
	return (CTermPlanDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CTermPlanView message handlers

int CTermPlanView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	
	return 0;
}

void CTermPlanView::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	
	CView::OnLButtonDown(nFlags, point);
//	GetDocument()->SetModifiedFlag(TRUE);
}
