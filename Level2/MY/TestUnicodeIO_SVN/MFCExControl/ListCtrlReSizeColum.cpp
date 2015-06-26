#include "StdAfx.h"
#include ".\listctrlresizecolum.h"
#pragma warning (disable:4244)
#pragma warning (disable:4267)

CReSizeColumnListCtrl::CReSizeColumnListCtrl() 
	: CListCtrl()
{
	memset( m_percent,0,sizeof(float) * 1024) ;
}
CReSizeColumnListCtrl::~CReSizeColumnListCtrl()
{

}
void CReSizeColumnListCtrl::InitColumnWidthPercent()
{
	CRect rc ;
	GetWindowRect(&rc) ;
	ScreenToClient(&rc) ;
	float total =(float)(rc.right -rc.left ) ;
	for (int i = 0; i < GetColumnCount()&&i<1024 ; i++)
	{
		m_percent[i] = GetColumnWidth(i) /total ;
	}
}
int CReSizeColumnListCtrl::GetColumnCount()
{
	// if we are not in report mode return error.
	if ((GetWindowLong(m_hWnd, GWL_STYLE) & LVS_TYPEMASK) != LVS_REPORT)
		return -1;

	// get a pointer to the header control, if NULL return error.
	CHeaderCtrl* pHeaderCtrl = GetHeaderCtrl();
	if (pHeaderCtrl == NULL)
		return -1;

	return pHeaderCtrl->GetItemCount();
}
void CReSizeColumnListCtrl::ResizeColumnWidth()
{
	CRect rc ;
	GetWindowRect(&rc) ;
	ScreenToClient(&rc) ;
	float total =(float)(rc.right -rc.left ) ;
	for (int i =0 ; i < GetColumnCount()&&i<1024 ; i++)
	{
		SetColumnWidth(i,(int)(m_percent[i]*total)) ;
	}
}


CSortListCtrlEx::CSortListCtrlEx():CReSizeColumnListCtrl()
{
	m_fAsc=TRUE;
}

CSortListCtrlEx::~CSortListCtrlEx()
{
}


BEGIN_MESSAGE_MAP(CSortListCtrlEx, CReSizeColumnListCtrl)
	//{{AFX_MSG_MAP(CSortListCtrl)
	ON_NOTIFY_REFLECT(LVN_COLUMNCLICK, OnColumnClick)
	//}}AFX_MSG_MAP


END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSortListCtrl message handlers

//*******************************************************************
//
// sort function 
//
//*******************************************************************
int CALLBACK ListCompare(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	CSortListCtrlEx* pV=(CSortListCtrlEx*)lParamSort; 
	CString strItem1, strItem2; 
	LVFINDINFO info; 
	int nIndex; 
	info.flags = LVFI_PARAM;                       //
	info.lParam = lParam1;                         //
	if ( (nIndex=pV->FindItem(&info)) != -1) 
		strItem1 = pV->GetItemText(nIndex, pV->m_nSortedCol); 
	info.lParam = lParam2;                         //
	if ( (nIndex=pV->FindItem(&info)) != -1)
		strItem2 = pV->GetItemText(nIndex, pV->m_nSortedCol); 
	int iCompRes ;
	iCompRes = wcscmp(strItem1, strItem2);     //
	if(pV->m_fAsc) 
		return iCompRes;
	else 
		return iCompRes*-1;
}

//*******************************************************************
//
// 
//
//*******************************************************************
void CSortListCtrlEx::OnColumnClick( NMHDR* pNMHDR, LRESULT* pResult )
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

	if( pNMListView->iSubItem == m_nSortedCol )
		m_fAsc = !m_fAsc;
	else
	{
		m_fAsc = TRUE;
		m_nSortedCol = pNMListView->iSubItem;
	}
	VERIFY(SortItems( ListCompare, DWORD_PTR( this )));

	*pResult = 0;
}
void CSortListCtrlEx::PrintList( CString _sHeader, CString _sFoot ,CString _title)
{
	m_sHeader = _sHeader;
	m_sFoot = _sFoot;
	CDC dc;
	CPrintDialog printDlg(FALSE);
	if(printDlg.DoModal() == IDCANCEL )
		return;
	dc.Attach( printDlg.GetPrinterDC() );
	dc.m_bPrinting = TRUE;
	CString sTitle;
	sTitle = _title;
	DOCINFO di;
	::ZeroMemory( &di, sizeof(DOCINFO) );
	di.cbSize = sizeof( DOCINFO );
	di.lpszDocName = sTitle;

	BOOL bPrintingOK = dc.StartDoc( &di );

	CPrintInfo info;
	info.m_rectDraw.SetRect( 0,0,dc.GetDeviceCaps( HORZRES ),dc.GetDeviceCaps( VERTRES ));

	OnBeginPrinting( &dc, &info );
	//	for( UINT page = info.GetMinPage(); page < info.GetMaxPage() && bPrintingOK; page++ )
	//	{
	//		info.m_nCurPage = page;
	OnPrint( &dc, &info );
	bPrintingOK = TRUE;
	//	}
	OnEndPrinting( &dc, &info );

	if( bPrintingOK )
		dc.EndDoc();
	else
		dc.AbortDoc();
	dc.Detach();

}
 
void CSortListCtrlEx::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CSortListCtrlEx::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}
void CSortListCtrlEx::PrintHead(CDC* pDC,int nPageWidth ,int nPageHMargin,int y)
{

	int nCol = GetHeaderCtrl()->GetItemCount();
	HDITEM hi;
	hi.mask = HDI_TEXT ;
	hi.cchTextMax = 40;
	TCHAR chBuffer[1024];
	hi.pszText = chBuffer;
	int nSpace=(nPageWidth-2*nPageHMargin) / nCol;
	for(int i = 0; i < nCol; i++ )
	{
		::ZeroMemory( chBuffer, 1024);
		GetHeaderCtrl()->GetItem( i,&hi);
		pDC->TextOut(nPageHMargin+i*nSpace, y,hi.pszText);
	}
}

void CSortListCtrlEx::OnPrint(CDC* pDC, CPrintInfo* pInfo)
{

	CString line; //This is the print line
	TEXTMETRIC metrics; //Font measurements
	int y = 0; //Current y position on report
	CFont TitleFont; //Font for Title
	CFont HeadingFont; //Font for headings
	CFont DetailFont; //Font for detail lines
	CFont FooterFont; //Font for footer
	//Tab stops at 1 inch, 2.5 inches, and 6.5 inches
	//	int TabStops[] = {100, 200,300,400,500};
	//Tab stops at 3.5 inches and 6.5 inches
	int FooterTabStops[] = {350, 650};
	if (!pInfo || pInfo->m_nCurPage == 1) {
		//Set the recordset at the beginning
	}
	short cxInch=pDC->GetDeviceCaps(LOGPIXELSX); 
	short cyInch=pDC->GetDeviceCaps(LOGPIXELSY); 

#define MYFONTSIZE 14 
#define HFONTNAME _T("MS Sans Serif")
	//
	int nFontHeight=MulDiv(MYFONTSIZE, -cyInch, 72); 
	if(nFontHeight % 2) nFontHeight++; 

	if(!TitleFont.CreateFont(nFontHeight*0.8, 0, 0, 0, FW_NORMAL, 0, 0,0, \
		GB2312_CHARSET, OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS, \
		DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, HFONTNAME)) 
		return ; 

	if(!HeadingFont.CreateFont(nFontHeight*0.6, 0, 0, 0, FW_NORMAL, 0, 0,0, \
		GB2312_CHARSET, OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS, \
		DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, HFONTNAME)) 
		return ; 

	if(!DetailFont.CreateFont(nFontHeight*0.5, 0, 0, 0, FW_NORMAL, 0, 0,0, \
		GB2312_CHARSET, OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS, \
		DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, HFONTNAME)) 
		return ; 

	if(!FooterFont.CreateFont(nFontHeight/2, 0, 0, 0, FW_NORMAL, 0, 0,0, \
		GB2312_CHARSET, OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS, \
		DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, HFONTNAME)) 
		return ;

	// Get the system's default printer's setting
	int	nPageVMargin,nPageHMargin,nPageHeight,nPageWidth;
	nPageVMargin = pDC->GetDeviceCaps(LOGPIXELSY) / 2;
	nPageHMargin = pDC->GetDeviceCaps(LOGPIXELSX) / 2;
	nPageHeight  = pDC->GetDeviceCaps(VERTRES);
	nPageWidth   = pDC->GetDeviceCaps(HORZRES);

	//Capture default settings when setting the footer font	
	pDC->StartPage() ;
	CFont* OldFont = (CFont*)pDC->SelectObject(&FooterFont);
	pDC->GetTextMetrics(&metrics);
	int nFooterHeight = metrics.tmHeight + metrics.tmExternalLeading;
	pDC->SelectObject(&TitleFont);
	//Retrieve the heading font measurements
	pDC->GetTextMetrics(&metrics);
	//Compute the heading line height
	int nLineHeight = metrics.tmHeight + metrics.tmExternalLeading;
	//Set Y to the line height.
	y += nLineHeight*1.5+nPageVMargin;
	pDC->TextOut(nPageHMargin, nPageVMargin, m_sHeader);
	//Draw a line under the heading
	pDC->MoveTo(nPageHMargin,y-nLineHeight/2);
	pDC->LineTo(nPageWidth-nPageHMargin,y-nLineHeight/2);
	//Set the Heading font
	pDC->SelectObject(&HeadingFont);
	y += nLineHeight;

	//Format the heading
	// get listctrl's header info
	int headpos = y ;
	int nCol = GetHeaderCtrl()->GetItemCount();
	int nSpace=(nPageWidth-2*nPageHMargin) / nCol;
	PrintHead(pDC,nPageWidth,nPageHMargin,headpos) ;

	//Compute the detail line height
	nLineHeight = metrics.tmHeight + metrics.tmExternalLeading;
	y += 2*nLineHeight; //Adjust y position
	//Set the detail font
    
	pDC->SelectObject(&DetailFont);
	//Retrieve detail font measurements
	pDC->GetTextMetrics(&metrics);
	//Compute the detail line height
	nLineHeight = metrics.tmHeight + metrics.tmExternalLeading;

	//Scroll through the list
	int beginpos = y ;
	int nIndex=1;
	int nRowCount = GetItemCount();
	while ( nIndex <= nRowCount ) 
	{
		if (pInfo && y > nPageHeight-nPageVMargin-nFooterHeight-nLineHeight-6) 
		{   // start a new page
			if (pInfo) {
				//Draw a line above the footer
				pDC->MoveTo(nPageHMargin,nPageHeight-nPageVMargin-nFooterHeight*1.5);
				pDC->LineTo(nPageWidth-nPageHMargin,nPageHeight-nPageVMargin-nFooterHeight*1.5);
				//Set the footer font
				pDC->SelectObject(&FooterFont);
				//Format the footer
				line.Format(_T(" \tPage %d"),pInfo->m_nCurPage);
				line = m_sFoot + line;
				//Output the footer at the bottom using tabs
				pDC->TabbedTextOut(nPageHMargin, nPageHeight-nPageVMargin-nFooterHeight, line, 2, FooterTabStops, 0);
			}
			pDC->EndPage() ;
		    pInfo->SetMaxPage(pInfo->m_nCurPage + 1);
			pInfo->m_nCurPage++ ;
			pDC->StartPage() ;
			y = 0 ;
			{
				pDC->GetTextMetrics(&metrics);
				int nFooterHeight = metrics.tmHeight + metrics.tmExternalLeading;
				pDC->SelectObject(&TitleFont);
				//Retrieve the heading font measurements
				pDC->GetTextMetrics(&metrics);
				//Compute the heading line height
				int nLineHeight = metrics.tmHeight + metrics.tmExternalLeading;
				//Set Y to the line height.
				y += nLineHeight*1.5+nPageVMargin;
				pDC->TextOut(nPageHMargin, nPageVMargin, m_sHeader);
				//Draw a line under the heading
				pDC->MoveTo(nPageHMargin,y-nLineHeight/2);
				pDC->LineTo(nPageWidth-nPageHMargin,y-nLineHeight/2);
				//Set the Heading font
				pDC->SelectObject(&HeadingFont);
				y += nLineHeight;
			}
			pDC->SelectObject(&HeadingFont);
			PrintHead(pDC,nPageWidth,nPageHMargin,headpos) ;
			pDC->SelectObject(&DetailFont);
			y = beginpos ;
			continue;
		}
		//Format the detail line
		BOOL mutline =FALSE ;
		int lines = 0 ;
		for( int nSubitem = 0; nSubitem < nCol; nSubitem++ )
		{

			TCHAR chLine[1024];
			this->GetItemText(nIndex-1,nSubitem,chLine,1023);
			CSize size = pDC->GetTextExtent(_T("a"));
			int NewFontWidth = size.cx ;
			long str = wcslen(chLine)*NewFontWidth ;
			int x = nPageHMargin+nSubitem*nSpace ;
			int _nextx = nPageHMargin+(nSubitem+1)*nSpace ;
			if((x + str) >= _nextx)
			{
				PrintMutiLine(pDC,x,_nextx,y,chLine,NewFontWidth,metrics.tmHeight,lines) ;
				mutline =TRUE ;
			}else
			   pDC->TextOut(nPageHMargin+nSubitem*nSpace, y,chLine);
			
		}
        if(mutline)
           y += lines*metrics.tmHeight ; 
		y += nLineHeight; //Adjust y position
		nIndex++; 
	}
	if (pInfo) {
		//Draw a line above the footer
		pDC->MoveTo(nPageHMargin,nPageHeight-nPageVMargin-nFooterHeight*1.5);
		pDC->LineTo(nPageWidth-nPageHMargin,nPageHeight-nPageVMargin-nFooterHeight*1.5);
		//Set the footer font
		pDC->SelectObject(&FooterFont);
		//Format the footer
		line.Format(_T(" \tPage %d"),pInfo->m_nCurPage);
		line = m_sFoot + line;
		//Output the footer at the bottom using tabs
		pDC->TabbedTextOut(nPageHMargin, nPageHeight-nPageVMargin-nFooterHeight, line, 2, FooterTabStops, 0);
	}
	//Restore default settings
	pDC->SelectObject(OldFont);
}
void CSortListCtrlEx::PrintMutiLine(CDC* pDC,int _x,int _nextX ,int _y ,TCHAR* _val,int _charwidth,int _charHight,int& lines)
{
	int len ;
	CutStringForPrint(_x,_nextX,_val,_charwidth,len) ;
	if(len == wcslen(_val))
	{
         pDC->TextOut(_x, _y,_val);
		 return ;
	}
	TCHAR cutstr[1024] = {0} ;
	wcsncpy(cutstr,_val,len) ;
	pDC->TextOut(_x,_y,cutstr);
	 lines++ ;
	PrintMutiLine(pDC,_x, _nextX ,_y+_charHight, _val+len, _charwidth,_charHight,lines);
   
}
void CSortListCtrlEx::CutStringForPrint(int _x,int _nextX ,TCHAR* _val,int _charwidth,int& cutlen)
{
	if(_val == NULL)
		return ;
	int strlength = wcslen(_val) ;
	cutlen = strlength ;
	if((_x + strlength* _charwidth) >= _nextX)
	{
          cutlen = ((_nextX -4*_charwidth) - _x )/_charwidth ;	  
	}
}