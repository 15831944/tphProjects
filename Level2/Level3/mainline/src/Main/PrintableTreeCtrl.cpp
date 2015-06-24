// PrintableTreeCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "termplan.h"
#include "PrintableTreeCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPrintableTreeCtrl

CPrintableTreeCtrl::CPrintableTreeCtrl()
{
	m_bNewPrint = TRUE;
}

CPrintableTreeCtrl::~CPrintableTreeCtrl()
{
}


BEGIN_MESSAGE_MAP(CPrintableTreeCtrl, CColorTreeCtrl)
//{{AFX_MSG_MAP(CPrintableTreeCtrl)
// NOTE - the ClassWizard will add and remove mapping macros here.
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPrintableTreeCtrl message handlers

//Setting up the environment of Print tast and drive it 
//In: _sHeader
//Out: void
void CPrintableTreeCtrl::PrintTree( CString _sHeader, CString _sFoot )
{
	m_sHeader = _sHeader;
	m_sFoot   = _sFoot;
	
	CPrintInfo printInfo;
	
	DOCINFO docInfo;
	memset(&docInfo, 0, sizeof(DOCINFO));
	docInfo.cbSize = sizeof(DOCINFO);
	docInfo.lpszDocName = _T("Flight Schedule");

	OnPreparePrinting(&printInfo);

	// setup the printing DC
	CDC dcPrint;
	CPrintDialog printDlg(FALSE);
	if(printDlg.DoModal() == IDCANCEL )
		return;
	dcPrint.Attach( printDlg.GetPrinterDC() );
	dcPrint.m_bPrinting = TRUE;

	//Init MaxPage etc
	OnBeginPrinting(&dcPrint, &printInfo);

	// start document printing process
	if( dcPrint.StartDoc(&docInfo) == SP_ERROR)
	{
		OnEndPrinting(&dcPrint, &printInfo);
		dcPrint.Detach();
		AfxMessageBox(AFX_IDP_FAILED_TO_START_PRINT);
		return;
	}
	// Guarantee values are in the valid range
	UINT nEndPage = printInfo.GetToPage();
	UINT nStartPage = printInfo.GetFromPage();

	if (nEndPage < printInfo.GetMinPage())
		nEndPage = printInfo.GetMinPage();
	if (nEndPage > printInfo.GetMaxPage())
		nEndPage = printInfo.GetMaxPage();

	if (nStartPage < printInfo.GetMinPage())
		nStartPage = printInfo.GetMinPage();
	if (nStartPage > printInfo.GetMaxPage())
		nStartPage = printInfo.GetMaxPage();

	int nStep = (nEndPage >= nStartPage) ? 1 : -1;
	nEndPage = (nEndPage == 0xffff) ? 0xffff : nEndPage + nStep;

	BOOL bError = FALSE;
	// begin page printing loop
	for (printInfo.m_nCurPage = nStartPage;
		printInfo.m_nCurPage != nEndPage; printInfo.m_nCurPage += nStep)
	{
		OnPrepareDC(&dcPrint, &printInfo);

		// check for end of print
		if (!printInfo.m_bContinuePrinting)
			break;
		// set up drawing rect to entire page (in logical coordinates)
		printInfo.m_rectDraw.SetRect(0, 0,
			dcPrint.GetDeviceCaps(HORZRES),
			dcPrint.GetDeviceCaps(VERTRES));
		dcPrint.DPtoLP(&printInfo.m_rectDraw);

		// attempt to start the current page
		if (dcPrint.StartPage() < 0)
		{
			bError = TRUE;
			break;
		}

		// must call OnPrepareDC on newer versions of Windows because
		// StartPage now resets the device attributes.
		//	if (afxData.bMarked4)
		OnPrepareDC(&dcPrint, &printInfo);

		ASSERT(printInfo.m_bContinuePrinting);

		// page successfully started, so now render the page
		OnPrint(&dcPrint, &printInfo);
		m_bNewPrint = FALSE;

		if (dcPrint.EndPage() < 0 )
		{
			bError = TRUE;
			break;
		}
	}
	// cleanup document printing process
	if (!printInfo.m_bDocObject)
	{
		if (!bError)
			dcPrint.EndDoc();
		else
			dcPrint.AbortDoc();
	}
//	AfxGetMainWnd()->EnableWindow();    // enable main window
	OnEndPrinting(&dcPrint, &printInfo);    // clean up after printing
	dcPrint.Detach();   // will be cleaned up by CPrintInfo destructor

	m_bNewPrint = TRUE;
}
//Set Default printInfo's value
BOOL CPrintableTreeCtrl::OnPreparePrinting(CPrintInfo* printInfo)
{
	printInfo->m_pPD->m_pd.nFromPage = (WORD)printInfo->GetMinPage();
	printInfo->m_pPD->m_pd.nToPage = (WORD)printInfo->GetMaxPage();
	printInfo->m_nNumPreviewPages = 0;
	return TRUE;
}
//Set Print style
void CPrintableTreeCtrl::OnPrepareDC(CDC* pDC, CPrintInfo* pInfo /* = NULL */)
{
	pDC->SetMapMode(MM_ANISOTROPIC);
	CClientDC dcScreen(NULL);
	pDC->SetWindowExt(dcScreen.GetDeviceCaps(LOGPIXELSX),dcScreen.GetDeviceCaps(LOGPIXELSX));
	pDC->SetViewportExt(pDC->GetDeviceCaps(LOGPIXELSX),pDC->GetDeviceCaps(LOGPIXELSX));
}

//Copy the Screen of TreeCtrl to clipboard as DDB 
void CPrintableTreeCtrl::OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo)
{
	// TODO: add extra initialization before printing
	HTREEITEM hItem=this->GetRootItem();
	this->GetItemRect(hItem,rcBounds,TRUE);
	m_nRowHeight = rcBounds.Height();

	// Find the total number of visible items & the right most coordinate
	int ItemCount=0;
	do
	{
		ItemCount++;
		CRect rc;
		this->GetItemRect(hItem,rc,TRUE);
		if (rc.right>rcBounds.right)
			rcBounds.right=rc.right;
		hItem=this->GetNextItem(hItem,TVGN_NEXTVISIBLE);
	}
	while (hItem);

	// Find the entire print boundary
	int ScrollMin,ScrollMax;
	GetScrollRange(SB_HORZ,&ScrollMin,&ScrollMax);
	rcBounds.left=0;
	if (ScrollMax>rcBounds.right)
		rcBounds.right=ScrollMax+1;
	rcBounds.top=0;
	rcBounds.bottom=m_nRowHeight*ItemCount;

	// Get text width
	CDC *pCtlDC = this->GetDC();
	if (NULL == pCtlDC) return;
	TEXTMETRIC tm;
	pCtlDC->GetTextMetrics(&tm);
	m_nCharWidth = tm.tmAveCharWidth;
	double d = (double)pDC->GetDeviceCaps(LOGPIXELSY)/(double)pCtlDC->GetDeviceCaps(LOGPIXELSY);
	ReleaseDC(pCtlDC);

	// Find rows per page
	int nPageHeight = pDC->GetDeviceCaps(VERTRES);
	m_nRowsPerPage = (int)((double)nPageHeight/d)/m_nRowHeight-TOP_MARGIN-BOTTOM_MARGIN;

	// Set maximum pages
	int pages=(ItemCount-1)/m_nRowsPerPage+1;
	pInfo->SetMaxPage(pages);

	// Create a memory DC compatible with the paint DC
	CPaintDC dc(this);
	CDC MemDC;
	MemDC.CreateCompatibleDC(&dc);

	// Select a compatible bitmap into the memory DC
	CBitmap bitmap;
	bitmap.CreateCompatibleBitmap(&dc, rcBounds.Width(), rcBounds.Height() );
	MemDC.SelectObject(&bitmap);

	// Enlarge window size to include the whole print area boundary
	GetWindowPlacement(&WndPlace);
	MoveWindow(0,0,::GetSystemMetrics(SM_CXEDGE)*2+rcBounds.Width(),
		::GetSystemMetrics(SM_CYEDGE)*2+rcBounds.Height(),FALSE);
	ShowScrollBar(SB_BOTH,FALSE);

	// Call the default printing
	this->EnsureVisible(this->GetRootItem());
	CWnd::DefWindowProc( WM_PAINT, (WPARAM)MemDC.m_hDC, 0 );

	// Now create a mask
	CDC MaskDC;
	MaskDC.CreateCompatibleDC(&dc);
	CBitmap maskBitmap;

	// Create monochrome bitmap for the mask
	maskBitmap.CreateBitmap( rcBounds.Width(), rcBounds.Height(), 1, 1, NULL );
	MaskDC.SelectObject( &maskBitmap );
	MemDC.SetBkColor( ::GetSysColor( COLOR_WINDOW ) );

	// Create the mask from the memory DC
	MaskDC.BitBlt( 0, 0, rcBounds.Width(), rcBounds.Height(), &MemDC,
		rcBounds.left, rcBounds.top, SRCCOPY );

	// Copy image to clipboard
	CBitmap clipbitmap;
	clipbitmap.CreateCompatibleBitmap(&dc, rcBounds.Width(), rcBounds.Height() );
	CDC clipDC;
	clipDC.CreateCompatibleDC(&dc);
	CBitmap* pOldBitmap = clipDC.SelectObject(&clipbitmap);
	clipDC.BitBlt( 0, 0, rcBounds.Width(), rcBounds.Height(), &MemDC,
		rcBounds.left, rcBounds.top, SRCCOPY);
	OpenClipboard();
	EmptyClipboard();
	SetClipboardData(CF_BITMAP, clipbitmap.GetSafeHandle());
	CloseClipboard();
	clipDC.SelectObject(pOldBitmap);
	clipbitmap.Detach();

	// Copy the image in MemDC transparently
	MemDC.SetBkColor(RGB(0,0,0));
	MemDC.SetTextColor(RGB(255,255,255));
	MemDC.BitBlt(rcBounds.left, rcBounds.top, rcBounds.Width(), rcBounds.Height(),
		&MaskDC, rcBounds.left, rcBounds.top, MERGEPAINT);

	CPalette pal;
	hDIB=DDBToDIB(bitmap, BI_RGB, &pal );

}
//cleanup after printing
void CPrintableTreeCtrl::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	GlobalFree(hDIB);
	SetWindowPlacement(&WndPlace);
	RedrawWindow();
}

/// Print tree view from DIB
//In: CDC*, CPrintInfo*
//Out: void
void CPrintableTreeCtrl::OnPrint(CDC* pDC, CPrintInfo* pInfo)
{
	// Save dc state
	int nSavedDC = pDC->SaveDC();

	// Set font
	CFont Font;
	LOGFONT lf;
	CFont *pOldFont = GetFont();
	pOldFont->GetLogFont(&lf);
	lf.lfHeight=m_nRowHeight-1;
	lf.lfWidth=0;
	Font.CreateFontIndirect(&lf);
	pDC->SelectObject(&Font);

	PrintHeadFoot(pDC,pInfo);
	pDC->SetWindowOrg(-1*(LEFT_MARGIN*m_nCharWidth),-m_nRowHeight*TOP_MARGIN);

	int height;
	if (pInfo->m_nCurPage==pInfo->GetMaxPage())
		height=rcBounds.Height()-((pInfo->m_nCurPage-1)*m_nRowsPerPage*m_nRowHeight);
	else
		height=m_nRowsPerPage*m_nRowHeight;
	int top=(pInfo->m_nCurPage-1)*m_nRowsPerPage*m_nRowHeight;

	// Print tree view from DIB
	LPBITMAPINFOHEADER lpbi;
	lpbi = (LPBITMAPINFOHEADER)hDIB;
	int nColors = lpbi->biClrUsed ? lpbi->biClrUsed : 1 << lpbi->biBitCount;
	BITMAPINFO &bmInfo = *(LPBITMAPINFO)hDIB;
	LPVOID lpDIBBits;
	if( bmInfo.bmiHeader.biBitCount > 8 )
		lpDIBBits = (LPVOID)((LPDWORD)(bmInfo.bmiColors + 
		bmInfo.bmiHeader.biClrUsed) + 
		((bmInfo.bmiHeader.biCompression == BI_BITFIELDS) ? 3 : 0));
	else
		lpDIBBits = (LPVOID)(bmInfo.bmiColors + nColors);
	HDC hDC=pDC->GetSafeHdc();
	StretchDIBits(hDC,				// hDC
		0,							// DestX
		0,							// DestY
		rcBounds.Width(),			// nDestWidth
		height,						// nDestHeight
		rcBounds.left,				// SrcX
		rcBounds.Height()-top-height,	// SrcY
		rcBounds.Width(),			// wSrcWidth
		height,						// wSrcHeight
		lpDIBBits,					// lpBits
		&bmInfo,					// lpBitsInfo
		DIB_RGB_COLORS,				// wUsage
		SRCCOPY);					// dwROP

	pDC->SelectObject(pOldFont);
	pDC->RestoreDC( nSavedDC );

}
//Print each page's Head and Foot
//In: CDC *pDc
//Out: void 
void CPrintableTreeCtrl::PrintHeadFoot(CDC *pDC, CPrintInfo *pInfo)
{
	CClientDC dcScreen(NULL);
	CRect rc;
	rc.top=m_nRowHeight*(TOP_MARGIN-2);
	rc.bottom = (int)((double)(pDC->GetDeviceCaps(VERTRES)*dcScreen.GetDeviceCaps(LOGPIXELSY))
		/(double)pDC->GetDeviceCaps(LOGPIXELSY));
	rc.left = LEFT_MARGIN*m_nCharWidth;
	rc.right = (int)((double)(pDC->GetDeviceCaps(HORZRES)*dcScreen.GetDeviceCaps(LOGPIXELSX))
		/(double)pDC->GetDeviceCaps(LOGPIXELSX))-RIGHT_MARGIN*m_nCharWidth;

	// Print App title on top left corner

	CRect header(rc);
	header.bottom=header.top+m_nRowHeight;
	if(m_bNewPrint)
		pDC->DrawText(m_sHeader, header, DT_LEFT | DT_SINGLELINE | DT_NOPREFIX | DT_VCENTER);
	pDC->MoveTo(header.TopLeft().x, header.BottomRight().y);
	pDC->LineTo(header.BottomRight().x, header.BottomRight().y);

	rc.top = rc.bottom - m_nRowHeight*(BOTTOM_MARGIN-1);
	rc.bottom = rc.top + m_nRowHeight;

	// Print draw page number at bottom center
	CString sTemp;
//	if(pInfo->m_nCurPage == pInfo->GetMaxPage())
//		sTemp.Format("%s Page %d/%d", m_sFoot, pInfo->m_nCurPage,pInfo->GetMaxPage());
//	else
//		sTemp.Format("Page %d/%d", pInfo->m_nCurPage,pInfo->GetMaxPage());
	sTemp.Format("Page %d/%d", pInfo->m_nCurPage,pInfo->GetMaxPage());
	pDC->DrawText(sTemp,rc, DT_CENTER | DT_SINGLELINE | DT_NOPREFIX | DT_VCENTER);
	pDC->MoveTo(rc.TopLeft().x, rc.BottomRight().y);
	pDC->LineTo(rc.BottomRight().x, rc.BottomRight().y);

}
// sending bitmap to the printer DC need device-independent , So change DDB to DIB
//In: CBitmap&, dwCompression, CPalette
//Out: HANDLE
HANDLE CPrintableTreeCtrl::DDBToDIB( CBitmap& bitmap, DWORD dwCompression, CPalette* pPal )
{
	BITMAP bm;
	BITMAPINFOHEADER bi;
	LPBITMAPINFOHEADER lpbi;
	DWORD dwLen;
	HANDLE hDIB;
	HANDLE handle;
	HDC hDC;
	HPALETTE hPal;

	ASSERT( bitmap.GetSafeHandle() );

	// The function has no arg for bitfields
	if ( dwCompression == BI_BITFIELDS )
		return NULL;

	// If a palette has not been supplied use defaul palette
	hPal = (HPALETTE) pPal->GetSafeHandle();
	if (hPal==NULL)
		hPal = (HPALETTE) GetStockObject(DEFAULT_PALETTE);

	// Get bitmap information
	bitmap.GetObject(sizeof(bm),(LPSTR)&bm);

	// Initialize the bitmapinfoheader
	bi.biSize               = sizeof(BITMAPINFOHEADER);
	bi.biWidth              = bm.bmWidth;
	bi.biHeight             = bm.bmHeight;
	bi.biPlanes             = 1;
	bi.biBitCount           = bm.bmPlanes * bm.bmBitsPixel;
	bi.biCompression        = dwCompression;
	bi.biSizeImage          = 0;
	bi.biXPelsPerMeter      = 0;
	bi.biYPelsPerMeter      = 0;
	bi.biClrUsed            = 0;
	bi.biClrImportant       = 0;

	// Compute the size of the  infoheader and the color table
	int nColors = (1 << bi.biBitCount);
	if ( nColors > 256 ) 
		nColors = 0;
	dwLen = bi.biSize + nColors * sizeof(RGBQUAD);

	// We need a device context to get the DIB from
	hDC = ::GetDC(NULL);
	hPal = SelectPalette(hDC,hPal,FALSE);
	RealizePalette(hDC);

	// Allocate enough memory to hold bitmapinfoheader and color table
	hDIB = GlobalAlloc(GMEM_FIXED,dwLen);

	if (!hDIB)
	{
		SelectPalette(hDC,hPal,FALSE);
		::ReleaseDC(NULL,hDC);
		return NULL;
	}

	lpbi = (LPBITMAPINFOHEADER)hDIB;

	*lpbi = bi;

	// Call GetDIBits with a NULL lpBits param, so the device driver 
	// will calculate the biSizeImage field 
	GetDIBits(hDC, (HBITMAP)bitmap.GetSafeHandle(), 0L, (DWORD)bi.biHeight,
		(LPBYTE)NULL, (LPBITMAPINFO)lpbi, (DWORD)DIB_RGB_COLORS);

	bi = *lpbi;

	// If the driver did not fill in the biSizeImage field, then compute it
	// Each scan line of the image is aligned on a DWORD (32bit) boundary
	if (bi.biSizeImage == 0)
	{
		bi.biSizeImage = ((((bi.biWidth * bi.biBitCount) + 31) & ~31) / 8) 
			* bi.biHeight;

		// If a compression scheme is used the result may infact be larger
		// Increase the size to account for this.
		if (dwCompression != BI_RGB)
			bi.biSizeImage = (bi.biSizeImage * 3) / 2;
	}

	// Realloc the buffer so that it can hold all the bits
	dwLen += bi.biSizeImage;
	if (handle = GlobalReAlloc(hDIB, dwLen, GMEM_MOVEABLE))
		hDIB = handle;
	else
	{
		GlobalFree(hDIB);

		// Reselect the original palette
		SelectPalette(hDC,hPal,FALSE);
		::ReleaseDC(NULL,hDC);
		return NULL;
	}

	// Get the bitmap bits
	lpbi = (LPBITMAPINFOHEADER)hDIB;

	// FINALLY get the DIB
	BOOL bGotBits = GetDIBits( hDC, (HBITMAP)bitmap.GetSafeHandle(),
		0L,                             // Start scan line
		(DWORD)bi.biHeight,             // # of scan lines
		(LPBYTE)lpbi                    // address for bitmap bits
		+ (bi.biSize + nColors * sizeof(RGBQUAD)),
		(LPBITMAPINFO)lpbi,             // address of bitmapinfo
		(DWORD)DIB_RGB_COLORS);         // Use RGB for color table

	if( !bGotBits )
	{
		GlobalFree(hDIB);

		SelectPalette(hDC,hPal,FALSE);
		::ReleaseDC(NULL,hDC);
		return NULL;
	}

	SelectPalette(hDC,hPal,FALSE);
	::ReleaseDC(NULL,hDC);
	return hDIB;

}




