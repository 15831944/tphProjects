#include "stdafx.h"
#include "resource.h"
#include "SortAndPrintListCtrl.h"

#define HEADER_HEIGHT       2    // For printing
#define FOOTER_HEIGHT       2
#define LEFT_MARGIN         4
#define RIGHT_MARGIN        4
#define TOP_MARGIN          1
#define BOTTOM_MARGIN       1 
#define GAP                 1

#define GVL_NONE                0
#define GVL_HORZ                1
#define GVL_VERT                2
#define GVL_BOTH                3

IMPLEMENT_DYNAMIC(CMultiSortHeaderCtrl, CHeaderCtrl)
CMultiSortHeaderCtrl::CMultiSortHeaderCtrl()
{
	m_bmpArrowUp.LoadBitmap(IDB_UPARROW);
	m_bmpArrowDown.LoadBitmap(IDB_DOWNARROW);
}

CMultiSortHeaderCtrl::~CMultiSortHeaderCtrl()
{
	m_bmpArrowDown.DeleteObject();
	m_bmpArrowUp.DeleteObject();
}

BEGIN_MESSAGE_MAP(CMultiSortHeaderCtrl, CHeaderCtrl)
END_MESSAGE_MAP()

int CMultiSortHeaderCtrl::SetSortImage( int nCol, BOOL bAsc )
{
	CBitmap *pTempBmp = NULL;
	if( bAsc )
		pTempBmp = &m_bmpArrowUp;
	else
		pTempBmp = &m_bmpArrowDown;

	HD_ITEM Item;	
	Item.mask = HDI_FORMAT;
	GetItem( nCol, &Item );
	Item.mask = HDI_BITMAP | HDI_FORMAT;	
	Item.fmt |= HDF_BITMAP;
	Item.hbm = (HBITMAP)pTempBmp->GetSafeHandle();
	SetItem( nCol, &Item );
	return 1;
}

void CMultiSortHeaderCtrl::RemoveAllSortImages()
{
	int iCount = GetItemCount();
	for( int i = 0; i < iCount; i++ )
		RemoveSortImage( i );
}

void CMultiSortHeaderCtrl::RemoveSortImage( int iItem )
{
	if( iItem != -1 )
	{
		HD_ITEM hditem;	
		hditem.mask = HDI_FORMAT;
		GetItem( iItem, &hditem );
		hditem.mask = HDI_FORMAT;	
		hditem.fmt &= ~HDF_BITMAP;
		SetItem( iItem, &hditem );
	}
}
/////////////////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CSortAndPrintListCtrl, CListCtrl)
CSortAndPrintListCtrl::CSortAndPrintListCtrl()
{
	m_bAscending = true;
	memset( m_aCombinedSortedColumns, -1, 32 );
	m_nGridLines  = GVL_BOTH;
}

CSortAndPrintListCtrl::~CSortAndPrintListCtrl()
{
}


BEGIN_MESSAGE_MAP(CSortAndPrintListCtrl, CListCtrl)
	ON_NOTIFY_REFLECT(LVN_COLUMNCLICK, OnLvnColumnclick)
	ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, OnCustomDraw)
END_MESSAGE_MAP()

void CSortAndPrintListCtrl::SubClassHeadCtrl()
{
	m_wndSortableHeaderCtrl.SubclassWindow( GetHeaderCtrl()->m_hWnd );
}

void CSortAndPrintListCtrl::OnLvnColumnclick(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here
	int nTestIndex = pNMListView->iSubItem;
	if( nTestIndex >= 0 )
	{
		CWaitCursor	wCursor;
		if(::GetKeyState( VK_CONTROL ) < 0 ) // Is <CTRL> Key Down
		{
			SortColumn( nTestIndex, 1);
		}
		else
		{
			SortColumn( nTestIndex, 0);
		}
	}	
	GetParent()->SendMessage(SORT_INDEX_COMPLETE_MSG,(WPARAM)pNMHDR,(LPARAM)nTestIndex);
	*pResult = 0;
}

int CSortAndPrintListCtrl::FindItemInCombinedSortedList( int p_nItem )
{
	int nNumCombinedSortedColumns = GetNumCombinedSortedColumns();
	for( int i = 0; i < nNumCombinedSortedColumns; i++ )
		if( m_aCombinedSortedColumns[i] == p_nItem )
			return i;
	return -1;
}

int CSortAndPrintListCtrl::GetNumCombinedSortedColumns()
{
	for( int i = 0; i < 32; i++ )
		if( m_aCombinedSortedColumns[i] == -1 )
			return i;
	return 32;
}

void CSortAndPrintListCtrl::MoveItemInCombinedSortedListToEnd( int p_nItem )
{
	int nNumCombinedSortedColumns;
	int aCombinedSortedColumns[32];
	int nItemIndex;


	nNumCombinedSortedColumns = GetNumCombinedSortedColumns();
	memset( aCombinedSortedColumns, -1, 32 );
	nItemIndex = FindItemInCombinedSortedList( p_nItem );
	if( nItemIndex != -1 )
	{
		if( nItemIndex > 0 )
		{
			memcpy( aCombinedSortedColumns, m_aCombinedSortedColumns, nItemIndex * sizeof( int ) );
			memcpy( &aCombinedSortedColumns[nItemIndex], &m_aCombinedSortedColumns[nItemIndex + 1], ( nNumCombinedSortedColumns - nItemIndex - 1) * sizeof(int) );
		}
	}
	aCombinedSortedColumns[ nNumCombinedSortedColumns - 1 ] = p_nItem;
	memcpy( m_aCombinedSortedColumns, aCombinedSortedColumns, 32 * sizeof(int) );
}


bool CSortAndPrintListCtrl::NotInCombinedSortedColumnList( int p_nItem )
{
	int nNumCombinedSortedColumns = GetNumCombinedSortedColumns();
	for( int i = 0; i < nNumCombinedSortedColumns; i++ )
		if( m_aCombinedSortedColumns[i] == p_nItem )
			return false;
	return true;
}

void CSortAndPrintListCtrl::SortColumn( int nSubItem, int nSortingMultipleColumns/* = 0 */)
{	
	int nNumCombinedSortedCols;

	nNumCombinedSortedCols = GetNumCombinedSortedColumns();

	if (nSortingMultipleColumns)
	{
		if( NotInCombinedSortedColumnList( nSubItem ) )
		{
			m_aCombinedSortedColumns[nNumCombinedSortedCols++] = nSubItem;
		}
		else
		{
			MoveItemInCombinedSortedListToEnd( nSubItem );
			m_bAscending = !m_bAscending;
		}
	}
	else
	{
		m_wndSortableHeaderCtrl.RemoveAllSortImages();
		if( !NotInCombinedSortedColumnList( nSubItem ) )
		{
			m_bAscending = !m_bAscending;
		}
		memset( m_aCombinedSortedColumns, -1, 32 );
		m_aCombinedSortedColumns[0] = nSubItem;
	}
	
	nNumCombinedSortedCols = GetNumCombinedSortedColumns();
	for (int i = 0; i < nNumCombinedSortedCols;i++)
	{
		m_wndSortableHeaderCtrl.SetSortImage(m_aCombinedSortedColumns[i],m_bAscending);
	}
	SortTextItems(nSubItem, m_bAscending,0,-1);
}
int CSortAndPrintListCtrl::GridCtrlMultiItemCompare(int nRow,int nMid)
{
	int nNumSortedCols = GetNumCombinedSortedColumns();
	int	nSortedCol;

	for( int i = 0; i < nNumSortedCols; i++ )
	{
		nSortedCol = m_aCombinedSortedColumns[i];
		CString midItem = GetItemText(nMid,nSortedCol);
		CString strValue = GetItemText(nRow,nSortedCol);
		int nValue = GridCtrlItemCompare(nSortedCol,strValue,midItem);
		if (nValue)
		{
			return nValue;
		}
	}
	return 0;
}

int CSortAndPrintListCtrl::GridCtrlItemCompare(int nSortedCol,const CString& csText1, const CString& csText2)
{
	switch( m_arrColType[nSortedCol] )
	{
	case  ITEM_INT:
		{
			int nValue1 = wcstol(csText1, NULL, 10);
			int nValue2 = wcstol(csText2, NULL, 10);
			if( nValue1 != nValue2)
			{
				if (nValue1 > nValue2)
				{
					return 1;
				}
				else
				{
					return -1;
				}
			}
		}
		break;

	case  ITEM_DOUBLE:
		{
			double fValue1 = wcstol(csText1, NULL, 10);
			double fValue2 = wcstol(csText2, NULL, 10);
			if( fValue1 != fValue2 )
			{
				if (fValue1 > fValue2)
				{
					return 1;
				}
				else
				{
					return -1;
				}
			}
		}
		break;

	case  ITEM_DATATIME:
		{
			COleDateTime oleDT1, oleDT2;
			if( oleDT1.ParseDateTime( csText1 ) && oleDT2.ParseDateTime( csText2 ) )
				return ( oleDT1 > oleDT2) ;
			else if( csText1.IsEmpty() && !csText2.IsEmpty() )
				return ( -1);
			else if( !csText1.IsEmpty() && csText2.IsEmpty() )
				return ( 1);
		}
		break;

	case  ITEM_STRING:
		{
			int nValue = csText1.CompareNoCase( csText2 );
			if( nValue )
				return ( nValue );
		}
		break;
	case ITEM_TIME:
		{
			int nPos1 = csText1.Find(':');
			CString strHour1 = csText1.Left(nPos1);
			CString strMin1 = csText1.Right(csText1.GetLength() - nPos1 - 1);
			int nPos2 = csText2.Find(':');
			CString strHour2 = csText2.Left(nPos2);
			CString strMin2 = csText2.Right(csText2.GetLength() - nPos2 - 1);

			int nData1 = wcstol(strHour1, NULL, 10) * 60 + wcstol(strMin1, NULL, 10);
			int nData2 = wcstol(strHour2, NULL, 10) * 60 + wcstol(strMin2, NULL, 10);

			if (nData1 != nData2)
			{
				if (nData1 > nData2)
				{
					return 1;
				}
				else
				{
					return -1;
				}
			}
		}
	default:
		ASSERT(_T("Error: attempt to sort a column without type."));
		return 0;
	}
	return 0;
}

BOOL CSortAndPrintListCtrl::SortTextItems(int nCol, BOOL bAscending, int low, int high)
{
	if (nCol >= GetHeaderCtrl()->GetItemCount()) return FALSE;

	if (nCol != m_aCombinedSortedColumns[0])
	{
		nCol = m_aCombinedSortedColumns[0];
	}

	if (high == -1) high = GetItemCount() - 1;

	int lo = low;
	int hi = high;

	if( hi <= lo ) return FALSE;

	int nMid = low;

	while( lo <= hi )
	{
		if( bAscending )
			while (lo < high  && GridCtrlMultiItemCompare(lo,nMid) < 0)
				++lo;
		else
			while (lo < high &&  GridCtrlMultiItemCompare(lo,nMid)> 0)
				++lo;

		if( bAscending )
			while (hi > low && GridCtrlMultiItemCompare(hi,nMid) > 0)
				--hi;
		else
			while (hi > low && GridCtrlMultiItemCompare(hi,nMid) < 0)
				--hi;

		if (lo <= hi)
		{
			if (GridCtrlMultiItemCompare(lo,hi))
			{
				for (int col = 0; col < m_wndSortableHeaderCtrl.GetItemCount(); col++)
				{
					CString strItemText = GetItemText(hi,col);
					SetItemText(hi,col,GetItemText(lo,col));
					SetItemText(lo,col,strItemText);
				}
				DWORD_PTR pData = (DWORD_PTR)CListCtrl::GetItemData(hi);
				CListCtrl::SetItemData(hi,CListCtrl::GetItemData(lo));
				CListCtrl::SetItemData(lo,pData);
			}

			++lo;
			--hi;
		}
	}

	if( low < hi )
		SortTextItems(nCol, bAscending, low, hi);

	if( lo < high )
		SortTextItems(nCol, bAscending, lo, high);

	return TRUE;
}

BOOL CSortAndPrintListCtrl::SetItemData(int nItem, DWORD_PTR dwData)
{
	SortItemData* pItem = new SortItemData;
	pItem->m_dwData = dwData;
	pItem->crTextColor = ::GetSysColor(COLOR_WINDOWFRAME);
	pItem->crBkColor = ::GetSysColor(COLOR_WINDOW);
	
	return CListCtrl::SetItemData(nItem,(DWORD_PTR)pItem);
}

DWORD_PTR CSortAndPrintListCtrl::GetItemData(int nItem)const
{
	SortItemData* pItem = (SortItemData*)CListCtrl::GetItemData(nItem);
	return pItem->m_dwData;
}

BOOL CSortAndPrintListCtrl::DeleteItem(int nItem)
{
	SortItemData* pItem = (SortItemData*)CListCtrl::GetItemData(nItem);
	if (pItem)
	{
		delete pItem;
		pItem = NULL;
	}
	VERIFY( CListCtrl::SetItemData(nItem,NULL));

	return CListCtrl::DeleteItem(nItem);
}

void CSortAndPrintListCtrl::OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult)
{
	//draw each item.set txt color,bkcolor....
	NMLVCUSTOMDRAW* pLVCD = reinterpret_cast<NMLVCUSTOMDRAW*>(pNMHDR);

	// Take the default processing unless we set this to something else below.
	*pResult = CDRF_DODEFAULT;

	// First thing - check the draw stage. If it's the control's prepaint
	// stage, then tell Windows we want messages for every item.

	if (pLVCD->nmcd.dwDrawStage == CDDS_PREPAINT)
	{
		*pResult = CDRF_NOTIFYITEMDRAW;
	}
	else if (pLVCD->nmcd.dwDrawStage == CDDS_ITEMPREPAINT)
	{
		// This is the notification message for an item.  We'll request
		// notifications before each subitem's prepaint stage.

		*pResult = CDRF_NOTIFYSUBITEMDRAW;
	}
	else if (pLVCD->nmcd.dwDrawStage == (CDDS_ITEMPREPAINT | CDDS_SUBITEM))
	{
		// This is the prepaint stage for a subitem. Here's where we set the
		// item's text and background colors. Our return value will tell
		// Windows to draw the subitem itself, but it will use the new colors
		// we set here.

		int nItem = static_cast<int> (pLVCD->nmcd.dwItemSpec);
		int nSubItem = pLVCD->iSubItem;

		SortItemData *pXLCD = (SortItemData *) pLVCD->nmcd.lItemlParam;
		//ASSERT(pXLCD);

		COLORREF crText  = ::GetSysColor(COLOR_WINDOWFRAME);
		COLORREF crBkgnd = ::GetSysColor(COLOR_WINDOW);


		if (pXLCD){
			crText  = pXLCD->crTextColor;
			crBkgnd = pXLCD->crBkColor;
		}



		// store the colors back in the NMLVCUSTOMDRAW struct
		pLVCD->clrText = crText;
		pLVCD->clrTextBk = crBkgnd;

		CDC* pDC = CDC::FromHandle(pLVCD->nmcd.hdc);
		CRect rect;
		GetSubItemRect(nItem, nSubItem, LVIR_BOUNDS, rect);
		if (GetItemState(nItem, LVIS_SELECTED))
			DrawText(nItem, nSubItem, pDC,::GetSysColor(COLOR_HIGHLIGHTTEXT),::GetSysColor(COLOR_HIGHLIGHT),rect);
		else
			DrawText(nItem, nSubItem, pDC, crText, crBkgnd, rect);

		*pResult = CDRF_SKIPDEFAULT;	// We've painted everything.
	}
}

void CSortAndPrintListCtrl::DrawText(int nItem, int nSubItem, CDC *pDC, COLORREF crText, COLORREF crBkgnd, CRect &rect)
{
	ASSERT(pDC);

	pDC->FillSolidRect(&rect, crBkgnd);

	CString str;
	str = GetItemText(nItem, nSubItem);

	if (!str.IsEmpty())
	{
		// get text justification
		HDITEM hditem;
		hditem.mask = HDI_FORMAT;
		m_wndSortableHeaderCtrl.GetItem(nSubItem, &hditem);
		int nFmt = hditem.fmt & HDF_JUSTIFYMASK;
		UINT nFormat = DT_VCENTER | DT_SINGLELINE;
		if (nFmt == HDF_CENTER)
			nFormat |= DT_CENTER;
		else if (nFmt == HDF_LEFT)
			nFormat |= DT_LEFT;
		else
			nFormat |= DT_RIGHT;


		pDC->SetBkMode(TRANSPARENT);
		pDC->SetTextColor(crText);
		pDC->SetBkColor(crBkgnd);
		rect.DeflateRect(4,0,0,0);
		pDC->DrawText(str, &rect, nFormat);

	}

}

BOOL CSortAndPrintListCtrl::SetItemColor(int nItem,COLORREF crTextColor,COLORREF crBkColor)
{
	SortItemData* pItem = (SortItemData*)CListCtrl::GetItemData(nItem);
	pItem->crTextColor = crTextColor;
	pItem->crBkColor = crBkColor;
	return TRUE;
}

/////////////print function////////////////////////////////////////////////////////////////////////////////////
void CSortAndPrintListCtrl::PrintList( CString _sHeader, CString _sFoot )
{
	CDC dc;
	CPrintDialog printDlg(FALSE);
	if(printDlg.DoModal() == IDCANCEL )
		return;
	dc.Attach( printDlg.GetPrinterDC() );
	dc.m_bPrinting = TRUE;
	CString sTitle;
	sTitle = _sHeader;
	DOCINFO di;
	::ZeroMemory( &di, sizeof(DOCINFO) );
	di.cbSize = sizeof( DOCINFO );
	di.lpszDocName = sTitle;

	BOOL bPrintingOK = dc.StartDoc( &di );

	CPrintInfo info;
	info.m_rectDraw.SetRect( 0,0,dc.GetDeviceCaps( HORZRES ),dc.GetDeviceCaps( VERTRES ));

	AutoSize();
	OnBeginPrinting( &dc, &info );
	for (UINT page = info.GetMinPage(); page <= info.GetMaxPage() && bPrintingOK; page++)
	{
		dc.StartPage();                         // begin new page
		info.m_nCurPage = page;
		OnPrint(&dc, &info);                    // Print page
		bPrintingOK = (dc.EndPage() > 0);       // end page
	}
	OnEndPrinting( &dc, &info );

	if( bPrintingOK )
		dc.EndDoc();
	else
		dc.AbortDoc();
	dc.Detach();

}

void CSortAndPrintListCtrl::OnBeginPrinting(CDC *pDC, CPrintInfo *pInfo)
{
	ASSERT(pDC && pInfo);
	if (!pDC || !pInfo) return;

	int nMaxRowCount = GetItemCount();
	if (!nMaxRowCount) return;

	// Get a DC for the current window (will be a screen DC for print previewing)
	CDC *pCurrentDC = GetDC();        // will have dimensions of the client area
	if (!pCurrentDC) return;

	CSize PaperPixelsPerInch(pDC->GetDeviceCaps(LOGPIXELSX), pDC->GetDeviceCaps(LOGPIXELSY));
	CSize ScreenPixelsPerInch(pCurrentDC->GetDeviceCaps(LOGPIXELSX), pCurrentDC->GetDeviceCaps(LOGPIXELSY));

	// Create the printer font
	int nFontSize = -12;
	CString strFontName = _T("Times New Roman");
	m_PrinterFont.CreateFont(nFontSize, 0,0,0, FW_NORMAL, 0,0,0, DEFAULT_CHARSET,
		OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS, DEFAULT_QUALITY,
		DEFAULT_PITCH | FF_DONTCARE, strFontName);

	CFont *pOldFont = pDC->SelectObject(&m_PrinterFont);

	// Get the average character width (in GridCtrl units) and hence the margins
	m_CharSize = pDC->GetTextExtent(_T("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSATUVWXYZ"),52);
	m_CharSize.cx /= 52;
	int nMargins = (LEFT_MARGIN+RIGHT_MARGIN)*m_CharSize.cx;

	// Get the page sizes (physical and logical)
	m_PaperSize = CSize(pDC->GetDeviceCaps(HORZRES), pDC->GetDeviceCaps(VERTRES));
	m_LogicalPageSize.cx = GetVirtualWidth()+nMargins;

#ifdef _WIN32_WCE
	m_LogicalPageSize.cy = (m_LogicalPageSize.cx * m_PaperSize.cy) / m_PaperSize.cx;
#else
	m_LogicalPageSize.cy = MulDiv(m_LogicalPageSize.cx, m_PaperSize.cy, m_PaperSize.cx);
#endif

	m_nPageHeight = m_LogicalPageSize.cy - GetRowHeight(0)
		- (HEADER_HEIGHT+FOOTER_HEIGHT + 2*GAP)*m_CharSize.cy;

	// Get the number of pages. Assumes no row is bigger than the page size.
	int nTotalRowHeight = 0;
	int nNumPages = 1;
	for (int row = 1; row < GetItemCount(); row++)
	{
		nTotalRowHeight += GetRowHeight(row);
		if (nTotalRowHeight > m_nPageHeight) {
			nNumPages++;
			nTotalRowHeight = GetRowHeight(row);
		}
	}

	// Set up the print info
	pInfo->SetMaxPage(nNumPages);
	pInfo->m_nCurPage = 1;                        // start printing at page# 1

	ReleaseDC(pCurrentDC);
	pDC->SelectObject(pOldFont);
}

void CSortAndPrintListCtrl::OnPrint(CDC *pDC, CPrintInfo *pInfo)
{
	if (!pDC || !pInfo) return;

	//CRect rcPage(pInfo->m_rectDraw);
	CFont *pOldFont = pDC->SelectObject(&m_PrinterFont);

	// Set the page map mode to use GridCtrl units, and setup margin
	pDC->SetMapMode(MM_ANISOTROPIC);
	pDC->SetWindowExt(m_LogicalPageSize);
	pDC->SetViewportExt(m_PaperSize);
	pDC->SetWindowOrg(-LEFT_MARGIN*m_CharSize.cx, 0);

	// Header
	pInfo->m_rectDraw.top    = 0;
	pInfo->m_rectDraw.left   = 0;
	pInfo->m_rectDraw.right  = m_LogicalPageSize.cx - (LEFT_MARGIN+RIGHT_MARGIN)*m_CharSize.cx;
	pInfo->m_rectDraw.bottom = HEADER_HEIGHT*m_CharSize.cy;
	PrintHeader(pDC, pInfo);
	pDC->OffsetWindowOrg(0, -HEADER_HEIGHT*m_CharSize.cy);

	// Gap between header and column headings
	pDC->OffsetWindowOrg(0, -GAP*m_CharSize.cy);

	// Print the column headings
	pInfo->m_rectDraw.bottom = GetRowHeight(0); 
	PrintColumnHeadings(pDC, pInfo);
	

	if(pInfo->m_nCurPage == 1)
	{
		pDC->OffsetWindowOrg(0, -GetRowHeight(0) + 1); 
	}
	else
	{
		pDC->OffsetWindowOrg(0, -GetRowHeight(0)); 
	}
	// We need to find out which row to start printing for this page.
	int nTotalRowHeight = 0;
	UINT nNumPages = 1;
	int nCurrPrintRow = 0;

	while (nCurrPrintRow < GetItemCount() && nNumPages < pInfo->m_nCurPage)
	{
		nTotalRowHeight += GetRowHeight(nCurrPrintRow);
		if (nTotalRowHeight > m_nPageHeight) {
			nNumPages++;
			if (nNumPages == pInfo->m_nCurPage) break;
			nTotalRowHeight = GetRowHeight(nCurrPrintRow);
		}
		nCurrPrintRow++;
	}
	if (nCurrPrintRow >= GetItemCount()) return;

	// Draw as many rows as will fit on the printed page.
	// Clip the printed page so that there is no partially shown
	// row at the bottom of the page (the same row which will be fully
	// shown at the top of the next page).

	BOOL bFirstPrintedRow = TRUE;
	CRect rect;
	rect.bottom = -1;
	while (nCurrPrintRow < GetItemCount())
	{
		rect.top = rect.bottom+1;
		rect.bottom = rect.top + GetRowHeight(nCurrPrintRow) - 1;

		if (rect.bottom > m_nPageHeight) break;            // Gone past end of page

		rect.right = -1;
		for (int col = 0; col < m_wndSortableHeaderCtrl.GetItemCount(); col++)
		{
			rect.left = rect.right+1;
			rect.right = rect.left + GetColumnWidth(col) - 1;

			DrawItem(pDC, nCurrPrintRow, col, rect);

			if (m_nGridLines == GVL_BOTH || m_nGridLines == GVL_HORZ) 
			{
				int Overlap = (col == 0)? 0:1;
				pDC->MoveTo(rect.left-Overlap, rect.bottom);
				pDC->LineTo(rect.right, rect.bottom);
				if (nCurrPrintRow == 0) {
					pDC->MoveTo(rect.left-Overlap, rect.top);
					pDC->LineTo(rect.right, rect.top);
				}
			}
			if (m_nGridLines == GVL_BOTH || m_nGridLines == GVL_VERT) 
			{
				int Overlap = (bFirstPrintedRow)? 0:1;
				pDC->MoveTo(rect.right, rect.top-Overlap);
				pDC->LineTo(rect.right, rect.bottom);    
				if (col == 0) {
					pDC->MoveTo(rect.left, rect.top-Overlap);
					pDC->LineTo(rect.left, rect.bottom);    
				}
			}

		}
		nCurrPrintRow++;
		bFirstPrintedRow = FALSE;
	}

	// Footer
	pInfo->m_rectDraw.bottom = FOOTER_HEIGHT*m_CharSize.cy;
	pDC->SetWindowOrg(-LEFT_MARGIN*m_CharSize.cx, -m_LogicalPageSize.cy + FOOTER_HEIGHT*m_CharSize.cy);
	PrintFooter(pDC, pInfo);

	// SetWindowOrg back for next page
	pDC->SetWindowOrg(0,0);

	pDC->SelectObject(pOldFont);
}

void CSortAndPrintListCtrl::PrintColumnHeadings(CDC *pDC, CPrintInfo* /*pInfo*/)
{
	CFont *pOldFont = pDC->SelectObject(&m_PrinterFont);

	CRect rect;
	rect.bottom = -1;

	rect.top = rect.bottom+1;
	rect.bottom = rect.top + GetRowHeight(0) - 1;

	rect.right = -1;
	for (int col = 0; col < m_wndSortableHeaderCtrl.GetItemCount(); col++)
	{
		rect.left = rect.right+1;
		rect.right = rect.left + GetColumnWidth(col) - 1;

		DrawHeadItem(pDC, 0, col, rect);

		if (m_nGridLines == GVL_BOTH || m_nGridLines == GVL_HORZ) 
		{
			int Overlap = (col == 0)? 0:1;
			pDC->MoveTo(rect.left-Overlap, rect.bottom);
			pDC->LineTo(rect.right, rect.bottom);
			
			pDC->MoveTo(rect.left-Overlap, rect.top);
			pDC->LineTo(rect.right, rect.top);
			
		}
		if (m_nGridLines == GVL_BOTH || m_nGridLines == GVL_VERT) 
		{
			int Overlap = 0;
			pDC->MoveTo(rect.right, rect.top-Overlap);
			pDC->LineTo(rect.right, rect.bottom);    
			if (col == 0) {
				pDC->MoveTo(rect.left, rect.top-Overlap);
				pDC->LineTo(rect.left, rect.bottom);    
			}
		}

	}
	

	pDC->SelectObject(pOldFont);
}

void CSortAndPrintListCtrl::PrintHeader(CDC *pDC, CPrintInfo *pInfo)
{
	CRect   rc(pInfo->m_rectDraw);
	CString strHeaderString;
	CFont   BoldFont;
	LOGFONT lf;

	//create bold font for header and footer
	VERIFY(m_PrinterFont.GetLogFont(&lf));
	lf.lfWeight = FW_BOLD;
	VERIFY(BoldFont.CreateFontIndirect(&lf));

	CFont *pNormalFont = pDC->SelectObject(&BoldFont);
	int nPrevBkMode = pDC->SetBkMode(TRANSPARENT);

	// print App title on top right margin
	strHeaderString.LoadString(AFX_IDS_APP_TITLE);
	pDC->DrawText(strHeaderString, &rc, DT_RIGHT | DT_SINGLELINE | DT_NOPREFIX | DT_VCENTER);

	// print parent window title in the centre (Gert Rijs)
	CWnd *pParentWnd = GetParent();
	while (pParentWnd)
	{
		pParentWnd->GetWindowText(strHeaderString);
		if (strHeaderString.GetLength())  // can happen if it is a CView, CChildFrm has the title
			break;
		pParentWnd = pParentWnd->GetParent();
	}
	pDC->DrawText(strHeaderString, &rc, DT_CENTER | DT_SINGLELINE | DT_NOPREFIX | DT_VCENTER);


	pDC->SetBkMode(nPrevBkMode);
	pDC->SelectObject(pNormalFont);
	BoldFont.DeleteObject();

	pDC->SelectStockObject(BLACK_PEN);
	pDC->MoveTo(rc.left, rc.bottom);
	pDC->LineTo(rc.right, rc.bottom);
}

//print footer with a line and date, and page number
void CSortAndPrintListCtrl::PrintFooter(CDC *pDC, CPrintInfo *pInfo)
{
	CRect rc(pInfo->m_rectDraw);
	CFont BoldFont;
	LOGFONT lf;

	//draw line
	pDC->MoveTo(rc.left, rc.top);
	pDC->LineTo(rc.right, rc.top);

	//create bold font for header and footer
	m_PrinterFont.GetLogFont(&lf);
	lf.lfWeight = FW_BOLD;
	BoldFont.CreateFontIndirect(&lf);

	CFont *pNormalFont = pDC->SelectObject(&BoldFont);
	int nPrevBkMode = pDC->SetBkMode(TRANSPARENT);

	// draw page number
	CString   sTemp ;
	rc.OffsetRect(0, m_CharSize.cy/2);
	sTemp.Format(_T("Page %d of %d"), pInfo->m_nCurPage, pInfo->GetMaxPage());
	pDC->DrawText(sTemp,-1,rc, DT_LEFT | DT_SINGLELINE | DT_NOPREFIX | DT_NOCLIP | DT_VCENTER);

	CTime t = CTime::GetCurrentTime();
	sTemp = t.Format(_T("%c"));
	pDC->DrawText(sTemp,-1,rc, DT_RIGHT | DT_SINGLELINE | DT_NOPREFIX | DT_NOCLIP | DT_VCENTER);

	pDC->SetBkMode(nPrevBkMode);
	pDC->SelectObject(pNormalFont);
	BoldFont.DeleteObject();
}

void CSortAndPrintListCtrl::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	m_PrinterFont.DeleteObject();
}

int CSortAndPrintListCtrl::GetRowHeight(int nRow)
{
	ASSERT(nRow >= 0 && nRow < GetItemCount());
	if (nRow < 0 || nRow >= GetItemCount()) return -1;

	return m_arRowHeights[nRow];
}

int CSortAndPrintListCtrl::GetHeadRowHeight(int nRow)
{
	int nHeight = 0;
	nHeight = GetRowHeight(0);

	return nHeight;
}

int CSortAndPrintListCtrl::GetVirtualWidth()
{
	int nWidth = 0;
	for (int i = 0; i < m_wndSortableHeaderCtrl.GetItemCount(); i++)
	{
		nWidth += m_arColWidths[i];
	}
	return nWidth;
}

int CSortAndPrintListCtrl::GetVirtualHeight()
{
	int nHeight = 0;
	int iRowCount = GetItemCount();
	for (int i = 0; i < iRowCount; i++)
		nHeight += m_arRowHeights[i];

	return nHeight;
}

int CSortAndPrintListCtrl::GetColumnWidth(int nCol)
{
	ASSERT(nCol >= 0 && nCol <  m_wndSortableHeaderCtrl.GetItemCount());
	if (nCol < 0 || nCol >= m_wndSortableHeaderCtrl.GetItemCount()) return -1;

	return m_arColWidths[nCol];
}

int CSortAndPrintListCtrl::GetHeadColumnWidth(int nCol)	
{
	int nWidth = 0;
	nWidth = GetColumnWidth(0);

	return nWidth;
}

CSize CSortAndPrintListCtrl::GetTextExtent(LPCTSTR str)
{
	CDC* pDC = GetDC();
	if (!pDC) return CSize(0,0);

	CFont *pOldFont;

	LOGFONT lf;
	GetParent()->GetFont()->GetLogFont( &lf );

	CFont Font;
	Font.CreateFontIndirect(&lf);

	pOldFont = pDC->SelectObject(&Font);

	CSize size = pDC->GetTextExtent(str);
	pDC->SelectObject(pOldFont);
	ReleaseDC(pDC);

	return size + CSize(8,2);
}

void CSortAndPrintListCtrl::AutoSize()
{
	int nNumColumns = m_wndSortableHeaderCtrl.GetItemCount();
	int nNumRows = GetItemCount();

	// initialize column widths to zero
	for (int nCol = 0; nCol < nNumColumns; nCol++)
		m_arColWidths[nCol] = 0;

	// initialize row heights to zero
	for (int nRow = 0; nRow < nNumRows; nRow++)
		m_arRowHeights[nRow] = 0;

	CSize size;
	for (int nCurCol = 0; nCurCol < nNumColumns; nCurCol++)
	{
		for (int nCurRow = 0; nCurRow < nNumRows; nCurRow++)
		{
			CString strValue;
			if (nCurRow == 0)
			{
				HD_ITEM hditem;
				hditem.mask = HDI_TEXT;
				hditem.cchTextMax = 39;
				TCHAR chBuffer[40];
				hditem.pszText = chBuffer;
				m_wndSortableHeaderCtrl.GetItem(nCurCol,&hditem);
				strValue = hditem.pszText;
			}
			else
			{
				strValue = GetItemText(nCurRow, nCurCol);
			}
			size = GetTextExtent(strValue);
			if (size.cx > (int) m_arColWidths[nCurCol]) 
				m_arColWidths[nCurCol] = size.cx;

			if (size.cy > (int) m_arRowHeights[nCurRow])
				m_arRowHeights[nCurRow] = size.cy;
		}
	}
}

void CSortAndPrintListCtrl::DrawHeadItem(CDC* pDC, int nRow, int nCol, CRect rect)
{
	LOGFONT lf;
	GetParent()->GetFont()->GetLogFont( &lf );

	CFont Font;
	Font.CreateFontIndirect(&lf);
	pDC->SelectObject(&Font);

	CBrush brush(::GetSysColor(COLOR_WINDOW));
	pDC->FillRect(rect, &brush);

	int nSavedDC = pDC->SaveDC();
	CString strHead;
	HD_ITEM hditem;
	hditem.mask = HDI_TEXT;
	hditem.cchTextMax = 39;
	TCHAR chBuffer[40];
	hditem.pszText = chBuffer;
	m_wndSortableHeaderCtrl.GetItem(nCol,&hditem);
	strHead = hditem.pszText;
	::DrawText(pDC->m_hDC, strHead, -1, rect, DT_CENTER|DT_VCENTER|DT_SINGLELINE);
	pDC->RestoreDC(nSavedDC);
	Font.DeleteObject();
}

void CSortAndPrintListCtrl::DrawItem(CDC* pDC, int nRow, int nCol, CRect rect)
{
	LOGFONT lf;
	GetParent()->GetFont()->GetLogFont( &lf );

	CFont Font;
	Font.CreateFontIndirect(&lf);
	pDC->SelectObject(&Font);

	CBrush brush(::GetSysColor(COLOR_WINDOW));
	pDC->FillRect(rect, &brush);

	int nSavedDC = pDC->SaveDC();
	::DrawText(pDC->m_hDC, GetItemText(nRow,nCol), -1, rect, DT_CENTER|DT_VCENTER|DT_SINGLELINE);
	pDC->RestoreDC(nSavedDC);
	Font.DeleteObject();
}