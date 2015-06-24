// ColorBox.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "ColorBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CColorBox
BOOL CColorBox::m_bBMLoaded = FALSE;
HBITMAP CColorBox::m_bmMultiColor = NULL;

CColorBox::CColorBox()
{
	m_Color = RGB(100, 100, 100);
	m_bIsMulti = FALSE;
	if(!m_bBMLoaded) {
		m_bmMultiColor = LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_MULTICOLOR));
		m_bBMLoaded = TRUE;
	}
}

CColorBox::~CColorBox()
{
}


BEGIN_MESSAGE_MAP(CColorBox, CStatic)
	//{{AFX_MSG_MAP(CColorBox)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CColorBox message handlers

void CColorBox::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

	CRect rect;
	GetClientRect(&rect);
	rect.DeflateRect(1, 1);

	if(!m_bIsMulti) {
		CBrush brush(m_Color);
		dc.FillRect(&rect, &brush);
	}
	else {
		CDC bitmapDC;
		bitmapDC.CreateCompatibleDC(&dc);
		HBITMAP hOldBM = (HBITMAP) ::SelectObject(bitmapDC.GetSafeHdc(),m_bmMultiColor);
		dc.BitBlt(rect.left,rect.top,rect.Width(), rect.Height(),&bitmapDC,0,0,SRCCOPY);
		::SelectObject(bitmapDC.GetSafeHdc(),hOldBM);
		bitmapDC.Detach();
	}
	
	// Do not call CStatic::OnPaint() for painting messages
}
