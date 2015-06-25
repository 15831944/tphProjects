#include "stdafx.h"
#include "CmpReportListView.h"
#include "CompareReportDoc.h"
#include ".\compare\ComparativeList.h"
#include "CmpReportTreeView.h"
#include "CmpReportGraphView.h"

#define IDC_CMPREPORT_LISTCTRL		0x01
#define MENU_MAXIMIZE				0x101
#define MENU_NORMAL					0x102
#define MENU_PRINT					0x103
#define MENU_EXPORT					0x104
#define MENU_UNAVAILABLE			0x200

IMPLEMENT_DYNCREATE(CCmpReportListView, CFormView)

CCmpReportListView::CCmpReportListView()
	: CFormView(CCmpReportListView::IDD), m_sExtension(_T("csv"))
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
	const CCmpReportManager &crrList = pCompProj->GetCompReportResultList();
	const CmpReportResultVector& vReport = crrList.GetReportResult();

	CString strFocusReport = m_pCmpReport->GetFocusReportName();
	CComparativeList cmpList(m_pCmpReport->GetTerminal(), m_wndListCtrl, &m_ctlHeaderCtrl);
	for(int i = 0; i < static_cast<int>(vReport.size()); i++)
	{
		CString reportName = vReport[i]->GetCmpReportName();
		if(reportName.CompareNoCase(strFocusReport) == 0)
		{
			m_wndListCtrl.ShowWindow(SW_SHOW);
			cmpList.RefreshData(*vReport[i]);
			return;
		}
	}

	// no data to show, hide the list control
	m_wndListCtrl.ShowWindow(SW_HIDE);
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
		OnMenuItemMax();
		break;
	case MENU_NORMAL:
		OnMenuItemNormal();
		break;
	case MENU_PRINT:
		OnMenuItemPrint();
		break;
	case MENU_EXPORT:
		OnMenuItemExport();
		break;
	default:
		break;
	}
	return;
}

void CCmpReportListView::OnMenuItemMax()
{
	CRect rectMax;
	GetParentFrame()->GetClientRect(&rectMax);
	rectMax.top+=2;

	CCompareReportDoc* pDoc = (CCompareReportDoc*)(GetDocument());
	CView * pTempView;
	POSITION pos = pDoc->GetFirstViewPosition();
	while (pos != NULL)
	{
		pTempView = pDoc->GetNextView(pos);
		if(pTempView->IsKindOf(RUNTIME_CLASS(CCmpReportTreeView)))
		{
			pTempView->ShowWindow(SW_HIDE);
		}
		if(pTempView->IsKindOf(RUNTIME_CLASS(CCmpReportGraphView)))
		{
			pTempView->ShowWindow(SW_HIDE);
		}
	}
	GetParent()->MoveWindow(&rectMax);
	MoveWindow(&rectMax);
	GetParentFrame()->Invalidate();
}

void CCmpReportListView::OnMenuItemNormal()
{

	CCompareReportDoc* pDoc = (CCompareReportDoc*)(GetDocument());
	CView * pTempView;
	POSITION pos = pDoc->GetFirstViewPosition();
	while (pos != NULL)
	{
		pTempView = pDoc->GetNextView(pos);
		if(pTempView->IsKindOf(RUNTIME_CLASS(CCmpReportTreeView)))
		{
			pTempView->ShowWindow(SW_SHOW);
		}
		if(pTempView->IsKindOf(RUNTIME_CLASS(CCmpReportGraphView)))
		{
			pTempView->ShowWindow(SW_SHOW);
		}
	}
	GetParentFrame()->ShowWindow(SW_NORMAL);
	GetParentFrame()->ShowWindow(SW_MAXIMIZE);
}

void CCmpReportListView::OnMenuItemPrint()
{
	m_sHeader="List Header String";
	m_sFoot="List Foot String";
	CDC dc;
	CPrintDialog printDlg(FALSE, PD_ALLPAGES | PD_USEDEVMODECOPIES | PD_NOPAGENUMS | PD_HIDEPRINTTOFILE);
	if (m_wndListCtrl.GetSelectedCount() == 0)						// if there are no selected items in the list control
		printDlg.m_pd.Flags = printDlg.m_pd.Flags | PD_NOSELECTION; //then disable the selection radio button
	else															//else ...
		printDlg.m_pd.Flags = printDlg.m_pd.Flags | PD_SELECTION;

	if(printDlg.DoModal() == IDCANCEL )
		return;
	dc.Attach( printDlg.GetPrinterDC() );
	dc.m_bPrinting = TRUE;
	CString sTitle;
	sTitle = "Flight Schedule";
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
	if(printDlg.PrintSelection() == FALSE)	//print all.
		OnPrint( &dc, &info );
	else 
		PrintSelectedItems(&dc, &info);				//print only the selected items.
	bPrintingOK = TRUE;
	//	}
	OnEndPrinting( &dc, &info );

	if( bPrintingOK )
		dc.EndDoc();
	else
		dc.AbortDoc();
	dc.Detach();
}

void CCmpReportListView::OnPrint(CDC *pDC, CPrintInfo *pInfo)
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
		//Set the record set at the beginning
	}
	short cxInch=pDC->GetDeviceCaps(LOGPIXELSX); 
	short cyInch=pDC->GetDeviceCaps(LOGPIXELSY); 

#define MYFONTSIZE 14 
#define HFONTNAME "MS Sans Serif"
	//
	int nFontHeight=MulDiv(MYFONTSIZE, -cyInch, 72); 
	if(nFontHeight % 2) nFontHeight++; 

	if(!TitleFont.CreateFont(static_cast<int>(nFontHeight*0.8), 0, 0, 0, FW_NORMAL, 0, 0,0, \
		GB2312_CHARSET, OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS, \
		DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, HFONTNAME)) 
		return ; 

	if(!HeadingFont.CreateFont(static_cast<int>(nFontHeight*0.7), 0, 0, 0, FW_NORMAL, 0, 0,0, \
		GB2312_CHARSET, OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS, \
		DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, HFONTNAME)) 
		return ; 

	if(!DetailFont.CreateFont(static_cast<int>(nFontHeight*0.7), 0, 0, 0, FW_NORMAL, 0, 0,0, \
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
	CFont* OldFont = (CFont*)pDC->SelectObject(&FooterFont);	
	pDC->GetTextMetrics(&metrics);
	int nFooterHeight = metrics.tmHeight + metrics.tmExternalLeading;
	pDC->SelectObject(&TitleFont);
	//Retrieve the heading font measurements
	pDC->GetTextMetrics(&metrics);
	//Compute the heading line height
	int nLineHeight = metrics.tmHeight + metrics.tmExternalLeading;
	//Set Y to the line height.
	y += static_cast<int>(nLineHeight*1.5+nPageVMargin);
	pDC->TextOut(nPageHMargin, nPageVMargin, m_sHeader);
	//Draw a line under the heading
	pDC->MoveTo(nPageHMargin,y-nLineHeight/2);
	pDC->LineTo(nPageWidth-nPageHMargin,y-nLineHeight/2);
	//Set the Heading font
	pDC->SelectObject(&HeadingFont);
	y += nLineHeight;

	//Format the heading
	// get ListCtrl's header info
	int nCol = m_wndListCtrl.GetHeaderCtrl()->GetItemCount();
	HDITEM hi;
	hi.mask = HDI_TEXT ;
	hi.cchTextMax = 19;
	char chBuffer[20];
	hi.pszText = chBuffer;
	int nSpace=(nPageWidth-2*nPageHMargin) / nCol;
	for(int i = 0; i < nCol; i++ )
	{
		::ZeroMemory( chBuffer, 20);
		m_wndListCtrl.GetHeaderCtrl()->GetItem( i,&hi);
		pDC->TextOut(nPageHMargin+i*nSpace, y,hi.pszText);
	}

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
	int nIndex=1;
	int nRowCount = m_wndListCtrl.GetItemCount();
	while ( nIndex <= nRowCount ) 
	{
		if (pInfo && y > nPageHeight-nPageVMargin-nFooterHeight-nLineHeight-6) 
		{   // start a new page
			pInfo->SetMaxPage(pInfo->m_nCurPage + 1);
			break;
		}
		//Format the detail line
		for( int nSubitem = 0; nSubitem < nCol; nSubitem++ )
		{

			char chLine[20];
			m_wndListCtrl.GetItemText(nIndex-1, nSubitem, chLine, 19);
			pDC->TextOut(nPageHMargin+nSubitem*nSpace, y, chLine);
		}

		y += nLineHeight; //Adjust y position
		nIndex++; 
	}
	if (pInfo) 
	{
		//Draw a line above the footer
		pDC->MoveTo(nPageHMargin,static_cast<int>(nPageHeight-nPageVMargin-nFooterHeight*1.5));
		pDC->LineTo(nPageWidth-nPageHMargin,static_cast<int>(nPageHeight-nPageVMargin-nFooterHeight*1.5));
		//Set the footer font
		pDC->SelectObject(&FooterFont);
		//Format the footer
		line.Format(" \tPage %d",pInfo->m_nCurPage);
		line = m_sFoot + line;
		//Output the footer at the bottom using tabs
		pDC->TabbedTextOut(nPageHMargin, nPageHeight-nPageVMargin-nFooterHeight, line, 2, FooterTabStops, 0);
	}
	//Restore default settings
	pDC->SelectObject(OldFont);
}
void CCmpReportListView::PrintSelectedItems(CDC *pDC, CPrintInfo *pInfo)
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
	if (!pInfo || pInfo->m_nCurPage == 1)
	{
		//Set the record set at the beginning
	}
	short cxInch=pDC->GetDeviceCaps(LOGPIXELSX); 
	short cyInch=pDC->GetDeviceCaps(LOGPIXELSY); 

#define MYFONTSIZE 14 
#define HFONTNAME "MS Sans Serif"
	//
	int nFontHeight=MulDiv(MYFONTSIZE, -cyInch, 72); 
	if(nFontHeight % 2) nFontHeight++; 

	if(!TitleFont.CreateFont(static_cast<int>(nFontHeight*0.8), 0, 0, 0, FW_NORMAL, 0, 0,0, \
		GB2312_CHARSET, OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS, \
		DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, HFONTNAME)) 
		return ; 

	if(!HeadingFont.CreateFont(static_cast<int>(nFontHeight*0.7), 0, 0, 0, FW_NORMAL, 0, 0,0, \
		GB2312_CHARSET, OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS, \
		DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, HFONTNAME)) 
		return ; 

	if(!DetailFont.CreateFont(static_cast<int>(nFontHeight*0.7), 0, 0, 0, FW_NORMAL, 0, 0,0, \
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
	CFont* OldFont = (CFont*)pDC->SelectObject(&FooterFont);	
	pDC->GetTextMetrics(&metrics);
	int nFooterHeight = metrics.tmHeight + metrics.tmExternalLeading;
	pDC->SelectObject(&TitleFont);
	//Retrieve the heading font measurements
	pDC->GetTextMetrics(&metrics);
	//Compute the heading line height
	int nLineHeight = metrics.tmHeight + metrics.tmExternalLeading;
	//Set Y to the line height.
	y += static_cast<int>(nLineHeight*1.5+nPageVMargin);
	pDC->TextOut(nPageHMargin, nPageVMargin, m_sHeader);
	//Draw a line under the heading
	pDC->MoveTo(nPageHMargin,y-nLineHeight/2);
	pDC->LineTo(nPageWidth-nPageHMargin,y-nLineHeight/2);
	//Set the Heading font
	pDC->SelectObject(&HeadingFont);
	y += nLineHeight;

	//Format the heading
	// get ListCtrl's header info
	int nCol = m_wndListCtrl.GetHeaderCtrl()->GetItemCount();
	HDITEM hi;
	hi.mask = HDI_TEXT ;
	hi.cchTextMax = 19;
	char chBuffer[20];
	hi.pszText = chBuffer;
	int nSpace=(nPageWidth-2*nPageHMargin) / nCol;
	for(int i = 0; i < nCol; i++ )
	{
		::ZeroMemory( chBuffer, 20);
		m_wndListCtrl.GetHeaderCtrl()->GetItem( i,&hi);
		pDC->TextOut(nPageHMargin+i*nSpace, y,hi.pszText);
	}

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
	int nIndex=1;
	POSITION ps = m_wndListCtrl.GetFirstSelectedItemPosition();
	if(ps == NULL) 
		return;
	int curSelectedItem = m_wndListCtrl.GetNextSelectedItem(ps);
	int nRowCount = m_wndListCtrl.GetSelectedCount();
	while ( nIndex <= nRowCount ) 
	{
		if (pInfo && y > nPageHeight-nPageVMargin-nFooterHeight-nLineHeight-6) 
		{   // start a new page
			pInfo->SetMaxPage(pInfo->m_nCurPage + 1);
			break;
		}
		//Format the detail line
		for( int nSubitem = 0; nSubitem < nCol; nSubitem++ )
		{

			char chLine[20];
			m_wndListCtrl.GetItemText(curSelectedItem, nSubitem, chLine, 19);
			pDC->TextOut(nPageHMargin+nSubitem*nSpace, y,chLine);
		}
		curSelectedItem = m_wndListCtrl.GetNextSelectedItem(ps);
		y += nLineHeight; //Adjust y position
		nIndex++; 
	}
	if (pInfo) {
		//Draw a line above the footer
		pDC->MoveTo(nPageHMargin,static_cast<int>(nPageHeight-nPageVMargin-nFooterHeight*1.5));
		pDC->LineTo(nPageWidth-nPageHMargin,static_cast<int>(nPageHeight-nPageVMargin-nFooterHeight*1.5));
		//Set the footer font
		pDC->SelectObject(&FooterFont);
		//Format the footer
		line.Format(" \tPage %d",pInfo->m_nCurPage);
		line = m_sFoot + line;
		//Output the footer at the bottom using tabs
		pDC->TabbedTextOut(nPageHMargin, nPageHeight-nPageVMargin-nFooterHeight, line, 2, FooterTabStops, 0);
	}
	//Restore default settings
	pDC->SelectObject(OldFont);
}
void CCmpReportListView::OnMenuItemExport() 
{
	ASSERT(m_sExtension.IsEmpty() == false);

	CString upExtension = m_sExtension;
	upExtension.MakeUpper();
	CFileDialog filedlg( FALSE,m_sExtension, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		"Report files (*." + m_sExtension + ") | *." + m_sExtension + ";*." + upExtension + "|All type (*.*)|*.*|", NULL, 0, FALSE  );
	if(filedlg.DoModal()!=IDOK)
		return;
	CString csFileName=filedlg.GetPathName();

	ArctermFile  _file ;
	if(!_file.openFile(csFileName,WRITE) )
	{
		MessageBox(_T("Create File error"),_T("Error"),MB_OK) ;
		return  ;
	}
	CString error ;
	CCompareReportDoc* pTermPlanDoc = (CCompareReportDoc* )GetDocument();
	if(pTermPlanDoc == NULL)
		return;

	if(!ExportListCtrlToCvsFile(_file))
	{
		MessageBox(error,"Error",MB_OK) ;
	}
	else
	{
		MessageBox(_T("Export Report Successful"),"Success",MB_OK) ;
	}
}

void CCmpReportListView::OnColumnclickListCtrl(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	int nTestIndex = pNMListView->iSubItem;
	if( nTestIndex >= 0 )
	{
		CWaitCursor	wCursor;
		if(::GetKeyState( VK_CONTROL ) < 0 ) // Is <CTRL> Key Down
		{
			m_ctlHeaderCtrl.SortColumn( nTestIndex, MULTI_COLUMN_SORT );
		}
		else
		{
			m_ctlHeaderCtrl.SortColumn( nTestIndex, SINGLE_COLUMN_SORT );
		}
		m_ctlHeaderCtrl.SaveSortList();
	}	
	*pResult = 0;
}

BOOL CCmpReportListView::ExportListCtrlToCvsFile(ArctermFile& _file)
{
	//export the List Ctrl head 
	int size = m_wndListCtrl.GetColumns() ;
	LVCOLUMN column ;
	column.mask = LVCF_TEXT ;
	TCHAR th[1024] = {0};
	column.pszText = th ;
	column.cchTextMax = 1024 ;
	for (int i = 0 ;i < size ;i++)
	{
		m_wndListCtrl.GetColumn(i,&column) ;
		_file.writeField(column.pszText) ;
	}
	_file.writeLine() ;
	return  ExportListData(_file) ;
}

BOOL CCmpReportListView::ExportListData(ArctermFile& _file) 
{
	CString val;
	for (int line = 0 ; line < m_wndListCtrl.GetItemCount() ;line++)
	{
		for (int colum = 0 ; colum < m_wndListCtrl.GetColumns() ; colum++)
		{
			val = m_wndListCtrl.GetItemText(line,colum) ;
			_file.writeField(val) ;
		}
		_file.writeLine() ;
	}
	return TRUE;
}
