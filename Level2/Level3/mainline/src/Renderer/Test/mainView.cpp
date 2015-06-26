// mainView.cpp : implementation of the CmainView class
//

#include "stdafx.h"
#include "main.h"

#include "mainDoc.h"
#include "mainView.h"
#include ".\mainview.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CmainView

IMPLEMENT_DYNCREATE(CmainView, C3DBaseView)

BEGIN_MESSAGE_MAP(CmainView, C3DBaseView)
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, C3DBaseView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, C3DBaseView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, C3DBaseView::OnFilePrintPreview)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()

// CmainView construction/destruction

CmainView::CmainView()
{
	// TODO: add construction code here

}

CmainView::~CmainView()
{
}

BOOL CmainView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

// CmainView drawing




// CmainView printing

BOOL CmainView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CmainView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CmainView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}


// CmainView diagnostics

#ifdef _DEBUG
void CmainView::AssertValid() const
{
	CView::AssertValid();
}

void CmainView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CmainDoc* CmainView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CmainDoc)));
	return (CmainDoc*)m_pDocument;
}
#endif //_DEBUG




void CmainView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	m_lbutDownPt = point;
	m_lastMousePt= point;

	//m_rdc.PickObject(point.x,point.y);
	C3DBaseView::OnLButtonDown(nFlags, point);
}

void CmainView::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	C3DBaseView::OnLButtonUp(nFlags, point);
}

void CmainView::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	
	if(::GetAsyncKeyState(VK_MENU) < 0)
	{
		if( MoveCamera(nFlags,point) )
			Invalidate(FALSE);
	}

	m_lastMousePt = point;
	C3DBaseView::OnMouseMove(nFlags, point);
}
