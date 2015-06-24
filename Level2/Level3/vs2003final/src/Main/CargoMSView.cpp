// CargoMSView.cpp : implementation file
//

#include "stdafx.h"
#include "TermPlan.h"
#include "CargoMSView.h"


// CCargoMSView

IMPLEMENT_DYNCREATE(CCargoMSView, CView)

CCargoMSView::CCargoMSView()
{
}

CCargoMSView::~CCargoMSView()
{
}

BEGIN_MESSAGE_MAP(CCargoMSView, CView)
END_MESSAGE_MAP()


// CCargoMSView drawing
static const LOGFONT lfTitle = 
{
	20, 0, 0, 0, FW_BOLD, 0, 0, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FF_DONTCARE, "Arial"
};

void CCargoMSView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: add draw code here

	int nHorzres = pDC->GetDeviceCaps( HORZRES );
	int nVertres = pDC->GetDeviceCaps( VERTRES );

	//postion
	CRect rcClient;
	GetClientRect(&rcClient);

	//draw title
	CFont fTitle; 
	fTitle.CreateFontIndirect(&lfTitle);
	CFont* pOldFont = pDC->SelectObject(&fTitle);

	CString strText(_T("Under construction"));
	pDC->TextOut(rcClient.Width()/5,rcClient.Height()/5,strText,strText.GetLength());
	pDC->SelectObject(pOldFont);
}


// CCargoMSView diagnostics

#ifdef _DEBUG
void CCargoMSView::AssertValid() const
{
	CView::AssertValid();
}

void CCargoMSView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif //_DEBUG


// CCargoMSView message handlers
