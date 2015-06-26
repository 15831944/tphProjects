#include "stdafx.h"
#include "ListCtrlEx.h"
#include "WinMsg.h"
#include "InPlaceEdit.h"
#include "resource.h"
#include "listctrlex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

static const int ID_DATETIMEPICKER_TIME = 101;
static const int ID_IPEDIT = 102;

CListCtrlEx::CListCtrlEx()
{
    CurrentSelection = -1;
    LastTimeSelection = -1;
    m_iSpinDisplayType = 0;
    m_bHHmmFormat = FALSE ;
    m_nLastDblClkRow = -1;
    m_nLastDlbClkColumn = -1;
}

CListCtrlEx::~CListCtrlEx()
{
    for(int i = (int)ddStyleList.GetUpperBound(); i >= 0; i--)
        delete ddStyleList[i];

    ddStyleList.RemoveAll();
}


BEGIN_MESSAGE_MAP(CListCtrlEx, CListCtrl)
    ON_WM_HSCROLL()
    ON_WM_VSCROLL()
    ON_WM_LBUTTONDOWN()
    ON_NOTIFY_REFLECT_EX(LVN_ENDLABELEDIT, OnEndlabeledit)
    ON_WM_LBUTTONDBLCLK()
    ON_WM_RBUTTONDOWN()
    ON_WM_KEYDOWN( )
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CListCtrlEx message handlers

// HitTestEx - Determine the row index and column index for a point
// Returns   - the row index or -1 if point is not over a row
// point     - point to be tested.
// col       - to hold the column index
int CListCtrlEx::HitTestEx(CPoint& point, int* col)
{
    int colnum = 0;
    int row = HitTest(point, NULL);

    if(col) 
        *col = 0;

    // Make sure that the ListView is in LVS_REPORT
    if((GetWindowLong(m_hWnd, GWL_STYLE) & LVS_TYPEMASK) != LVS_REPORT)
        return row;

    // Get the number of columns
    CHeaderCtrl* pHeader = GetHeaderCtrl();
    int nColumnCount = pHeader->GetItemCount();

    // Get the top and bottom row visible
    row = GetTopIndex();

    int bottom = row + GetCountPerPage();
    if(bottom > GetItemCount())
        bottom = GetItemCount();

    // Loop through the visible rows
    for( ;row <=bottom; row++)
    {
        // Get bounding rect of item and check whether point falls in it.
        CRect rect;
        if( GetItemRect(row, &rect, LVIR_BOUNDS) )
        {
            if( rect.PtInRect(point) )
            {
                // Now find the column
                for( colnum = 0; colnum < nColumnCount; colnum++ )
                {
                    int colwidth = GetColumnWidth(colnum);
                    if(point.x >= rect.left && point.x <= (rect.left + colwidth))
                    {
                        if(col) 
                            *col = colnum;
                        return row;
                    }
                    rect.left += colwidth;
                }
            }
        }
        else if( col )
        {
            // Is pre item is empty
            if (row > 0 && IsPreItemEmpty(row) )
                return -1;
            if(IsItemEmpty(row))
                return -1;

            // Clicked on the 'empty' row
            int newInsertIndex = InsertItem( row + 1, _T(""));
            SetItemState( newInsertIndex, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
            GetItemRect(row, &rect, LVIR_BOUNDS);
            if(rect.PtInRect(point))
            {
                // Now find the column
                for( colnum = 0; colnum < nColumnCount; colnum++ )
                {
                    int colwidth = GetColumnWidth(colnum);
                    if(point.x >= rect.left && point.x <= (rect.left + colwidth))
                    {
                        *col = colnum;
                        return row;
                    }
                    rect.left += colwidth;
                }
            }
            else
            {
                DeleteItemEx( newInsertIndex );
            }
        }
    }
    return -1;
}

// ShowInPlaceList - Creates an in-place drop down list for any 
//                 - cell in the list view control
// Returns         - A temporary pointer to the combo-box control
// nItem           - The row index of the cell
// nCol            - The column index of the cell
// lstItems        - A list of strings to populate the control with
// nSel            - Index of the initial selection in the drop down list
CComboBox* CListCtrlEx::ShowInPlaceList( int nItem, int nCol)
{
    // The returned pointer should not be saved

    // Make sure that the item is visible
    if(!EnsureVisible(nItem, TRUE)) 
        return NULL;

    // Make sure that nCol is valid 
    CHeaderCtrl* pHeader = GetHeaderCtrl();
    int nColumnCount = pHeader->GetItemCount();
    if(nCol >= nColumnCount || GetColumnWidth(nCol) < 10)
        return NULL;

    // Get the column offset
    int offset = 0;
    for(int i = 0; i < nCol; i++)
        offset += GetColumnWidth(i);

    CRect rect;
    GetItemRect(nItem, &rect, LVIR_BOUNDS);

    // Now scroll if we need to expose the column
    CRect rcClient;
    GetClientRect(&rcClient);
    if(offset + rect.left < 0 || offset + rect.left > rcClient.right)
    {
        CSize size;
        size.cx = offset + rect.left;
        size.cy = 0;
        Scroll(size);
        rect.left -= size.cx;
    }
    rect.left += (offset + 4);
    rect.right = rect.left + GetColumnWidth(nCol) - 3;
    int height = rect.bottom - rect.top;
    rect.bottom += (10 * height);
    if(rect.right > rcClient.right) 
        rect.right = rcClient.right;

    DWORD dwStyle = WS_BORDER|WS_CHILD|WS_VISIBLE|WS_VSCROLL|CBS_DROPDOWNLIST|CBS_DISABLENOSCROLL;


    CStringList& strList = ((LVCOLDROPLIST*)ddStyleList[nCol])->List;
    CString csSelStr = GetItemText( nItem, nCol );
    POSITION pos;
    int ii = 0;
    int nSelIndex = 0;
    for( pos = strList.GetHeadPosition(); pos; ii++ )
    {

        CString csStr = strList.GetAt( pos );
        if( csStr == csSelStr )
        {
            nSelIndex = ii;
            break;
        }
        strList.GetNext( pos );
    } 

    CComboBox *pList = new CInPlaceList(nItem, nCol, &((LVCOLDROPLIST*)ddStyleList[nCol])->List, nSelIndex );
    pList->Create(dwStyle, rect, this, ID_IPEDIT);
    pList->SetItemHeight(-1, height);
    pList->SetHorizontalExtent(GetColumnWidth(nCol));
    return pList;
}

void CListCtrlEx::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
    if(GetFocus() != this ) 
        SetFocus();
    CListCtrl::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CListCtrlEx::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
    if(GetFocus() != this) 
        SetFocus();
    CListCtrl::OnVScroll(nSBCode, nPos, pScrollBar);
}

BOOL CListCtrlEx::OnEndlabeledit(NMHDR* pNMHDR, LRESULT* pResult) 
{
    LV_DISPINFO* plvDispInfo = (LV_DISPINFO *)pNMHDR;
    LV_ITEM* plvItem = &plvDispInfo->item;
    GetItemText(plvItem->iItem, plvItem->iSubItem);

    if(plvItem->pszText != NULL)
        SetItemText(plvItem->iItem, plvItem->iSubItem, plvItem->pszText);
    *pResult = FALSE;
    return FALSE;
}

void CListCtrlEx::OnLButtonDown(UINT nFlags, CPoint point) 
{
    SetFocus();
    int index;
    if((index = HitTestEx(point, NULL)) != -1)
    {
        LastTimeSelection = CurrentSelection;
        CurrentSelection = index;
        SetItemState(index, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
    }

    //CListCtrl::OnLButtonDown(nFlags, point);
}

// EditSubLabel  - Start edit of a sub item label
// Returns  - Temporary pointer to the new edit control
// nItem  - The row index of the item to edit
// nCol   - The column of the sub item.
CEdit* CListCtrlEx::EditSubLabel(int nItem, int nCol)
{
    // The returned pointer should not be saved

    // Make sure that the item is visible
    if(!EnsureVisible(nItem, TRUE)) 
        return NULL;

    // Make sure that nCol is valid
    CHeaderCtrl* pHeader = GetHeaderCtrl();
    int nColumnCount = pHeader->GetItemCount();
    if(nCol >= nColumnCount || GetColumnWidth(nCol) < 5)
        return NULL;

    // Get the column offset
    int offset = 0;
    for(int i = 0; i < nCol; i++)
        offset += GetColumnWidth(i);

    CRect rect;
    GetItemRect(nItem, &rect, LVIR_BOUNDS);

    // Now scroll if we need to expose the column
    CRect rcClient;
    GetClientRect(&rcClient);
    if(offset + rect.left < 0 || offset + rect.left > rcClient.right)
    {
        CSize size;
        size.cx = offset + rect.left;
        size.cy = 0;
        Scroll(size);
        rect.left -= size.cx;
    }
    // Get Column alignment
    LV_COLUMN lvcol;
    lvcol.mask = LVCF_FMT;
    GetColumn(nCol, &lvcol);
    DWORD dwStyle ;
    if((lvcol.fmt&LVCFMT_JUSTIFYMASK) == LVCFMT_LEFT)
        dwStyle = ES_LEFT;
    else if((lvcol.fmt&LVCFMT_JUSTIFYMASK) == LVCFMT_RIGHT)
        dwStyle = ES_RIGHT;
    else dwStyle = ES_CENTER;

    rect.left += offset+4;
    rect.right = rect.left + GetColumnWidth(nCol) - 3 ;
    if(rect.right > rcClient.right) 
        rect.right = rcClient.right;

    dwStyle |= WS_BORDER|WS_CHILD|WS_VISIBLE|ES_AUTOHSCROLL;

    int nUseType = IPEDIT_TYPE_NORMAL;
    if (((LVCOLDROPLIST*)ddStyleList[nCol])->Style == NUMBER_ONLY)
        nUseType = IPEDIT_TYPE_NUMBER;
    if( ((LVCOLDROPLIST*)ddStyleList[nCol])->Style == TEXT_EDIT )
        dwStyle |=ES_UPPERCASE;

    CEdit *pEdit = new CInPlaceEdit(nItem, nCol, GetItemText(nItem, nCol), nUseType);
    pEdit->Create( dwStyle, rect, this, ID_IPEDIT );
    return pEdit;
}


CEdit* CListCtrlEx::EditSpinLabel(int nItem, int nCol)
{
    if(!EnsureVisible(nItem, TRUE)) 
        return NULL;

    m_nSpinSelItem = nItem;
    m_nSpinSelCol = nCol;

    // Set title and percent
    CString strPercent;
    strPercent = GetItemText( nItem, nCol );
    strPercent.Remove('%');
    m_SpinEdit.SetTitle(_T(""));

    m_SpinEdit.SetDisplayType( m_iSpinDisplayType );

    int iPercent = 0;
    if (strPercent.GetLength() > 0)
    {
        iPercent = wcstol(strPercent, NULL, 10);
    }
    m_SpinEdit.SetPercent(iPercent);

    // Create or move it
    // Make sure that nCol is valid
    CHeaderCtrl* pHeader = GetHeaderCtrl();
    int nColumnCount = pHeader->GetItemCount();
    if(nCol >= nColumnCount || GetColumnWidth(nCol) < 5)
        return NULL;

    // Get the column offset
    int offset = 0;
    for(int i = 0; i < nCol; i++)
        offset += GetColumnWidth(i);

    CRect rect;
    GetItemRect(nItem, &rect, LVIR_BOUNDS);

    // Now scroll if we need to expose the column
    CRect rcClient;
    GetClientRect(&rcClient);
    if(offset + rect.left < 0 || offset + rect.left > rcClient.right)
    {
        CSize size;
        size.cx = offset + rect.left;
        size.cy = 0;
        Scroll(size);
        rect.left -= size.cx;
    }

    rect.left += offset+4;
    rect.right = rect.left + GetColumnWidth(nCol) - 3 ;
    if(rect.right > rcClient.right) 
        rect.right = rcClient.right;


    if (!m_SpinEdit.GetSafeHwnd())
        m_SpinEdit.Create(_T("STATIC"), NULL, WS_CHILD|WS_VISIBLE|WS_BORDER, rect, this, NULL);
    else
        m_SpinEdit.MoveWindow( &rect );

    LVCOLDROPLIST* dropList = (LVCOLDROPLIST*)ddStyleList[nCol];
    if (-1 != dropList->nParam1
        && -1 != dropList->nParam2)
    {
        m_SpinEdit.SetSpinRange(dropList->nParam1, dropList->nParam2);
    }

    m_SpinEdit.ShowWindow(SW_SHOW);
    m_SpinEdit.BringWindowToTop();
    m_SpinEdit.SetFocus();

    return NULL;  // USER CAN NOT CHARGE IT.
}


int CListCtrlEx::InsertColumn(int nCol, const LV_COLUMNEX* pColumn)
{
    if(ddStyleList.GetSize() <= nCol)
        ddStyleList.SetSize(nCol + 1);
    LVCOLDROPLIST* DropList = new LVCOLDROPLIST;

    if(pColumn->fmt & LVCFMT_NOEDIT)
        DropList->Style = NO_EDIT;
    else if(pColumn->fmt & LVCFMT_DROPDOWN)
        DropList->Style = DROP_DOWN;
    else if(pColumn->fmt & LVCFMT_SPINEDIT)
        DropList->Style = SPIN_EDIT;
    else if(pColumn->fmt & LVCFMT_NUMBER)
        DropList->Style = NUMBER_ONLY;
    else if(pColumn->fmt & LVCFMT_DATETIME)
        DropList->Style = DATE_TIME;
    else if(pColumn->fmt & LVCFMT_POPUP_GATE_SELECTION)
        DropList->Style = POPUP_GATE_SELECTION;
    if( pColumn->fmt & LVCFMT_EDIT )
        DropList->Style = TEXT_EDIT;
    if (pColumn->fmt & LVCFMT_CHECK ) 
        DropList->Style = CHECKBOX;  

    DropList->List.AddTail(pColumn->csList);
    DropList->strDateFormat = pColumn->pszDateFormat;
    DropList->strShowDateFormat = pColumn->pszShowDateFormat;

    DropList->nParam1 = pColumn->nParam1;
    DropList->nParam2 = pColumn->nParam2;

    ddStyleList[nCol] = (void*)DropList;
    // Transfer the Relevant Fields to lvc and pass to base class
    LV_COLUMN lvc;
    memcpy(&lvc, pColumn, sizeof(tagLVCOLUMNA));
    return CListCtrl::InsertColumn(nCol, &lvc);
}

LVCOLDROPLIST* CListCtrlEx::GetColumnStyle(int nColumn)
{
    if (nColumn < 0 || nColumn >= ddStyleList.GetSize())
        return 0;

    return (LVCOLDROPLIST*)ddStyleList.GetAt(nColumn);
}


void CListCtrlEx::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
    int index;
    int column;
    if((index = HitTestEx(point, &column)) != -1)
    {
        DblClickItem(index, column);
    }
    CListCtrl::OnLButtonDblClk(nFlags, point);
}

void CListCtrlEx::OnRButtonDown(UINT nFlags, CPoint point) 
{
    SetFocus();
    int index;
    int colnum;
    if((index = HitTestEx(point, &colnum)) != -1)
    {
        LastTimeSelection = CurrentSelection;    
        CurrentSelection = index;
        SetItemState(index, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
    }
    CListCtrl::OnRButtonDown(nFlags, point);
}

void CListCtrlEx::OnKeyDown( UINT nChar, UINT nRepCnt, UINT nFlags )
{
    // TODO: Add your message handler code here and/or call default

    int nCount = GetItemCount();

    switch(nChar)
    {
    case VK_UP:
        {
            if (0 < CurrentSelection
                && CurrentSelection < nCount)
            {
                CurrentSelection--;
            }
        }
        break;

    case VK_DOWN:
        {
            if (-1 < CurrentSelection
                && CurrentSelection < nCount-1)
            {
                CurrentSelection++;
            }
            else if (-1 == CurrentSelection
                && 0 < nCount)
            {
                CurrentSelection++;
            }
        }
        break;
    }

    CListCtrl::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CListCtrlEx::DeleteItemEx(int p_index)
{
    if( DeleteItem( p_index ) )
    {
        if( p_index > 0 )
        {
            CurrentSelection = p_index - 1;
            SetItemState(p_index-1, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
        }
        else if( GetItemCount()>0 )
        {
            CurrentSelection = 0;
            SetItemState(0, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
        }
        else
        {
            CurrentSelection = -1;
        }
    }
}

BOOL CListCtrlEx::DeleteAllItems()
{
    CurrentSelection = -1;
    return CListCtrl::DeleteAllItems();
}
void CListCtrlEx::SetCurSel(int nItem)
{
    SetItemState(nItem,LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
    LastTimeSelection=CurrentSelection;
    CurrentSelection=nItem;
}
LRESULT CListCtrlEx::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
    if (message == WM_INPLACE_SPIN)
    {
        LPSPINTEXT pst = (LPSPINTEXT) lParam;

        CString strPercent;
        if( m_iSpinDisplayType == 0 )
            strPercent.Format(_T("%d%%"), pst->iPercent);
        else if ( m_iSpinDisplayType == 1 ) //add by tutu 2002-11-20
            strPercent.Format(_T("%d"), pst->iPercent);
        pst->nItem = m_nSpinSelItem;
        pst->nColumn = m_nSpinSelCol;
        SetItemText( m_nSpinSelItem, m_nSpinSelCol, strPercent );
        GetParent()->SendMessage( message, wParam, lParam );
        return TRUE;
    }

    if(message == WM_INPLACE_DATETIME)
    {
        BOOL bOwn = (BOOL)wParam;
        if(bOwn) 
            return FALSE;

        COleDateTime *oTime = (COleDateTime *)lParam;
        CString strTime;
        LVCOLDROPLIST* pDropList = (LVCOLDROPLIST*)(ddStyleList[m_nTimeSelCol]);
        if (!pDropList->strShowDateFormat.IsEmpty())
            strTime = oTime->Format(pDropList->strShowDateFormat);
        else
        {
            if (m_bHHmmFormat)
            {
                strTime= oTime->Format(_T("%H:%M"));
            }
            else
            {
                strTime = oTime->Format(_T("%H:%M:%S"));
            }
        }


        SetItemText( m_nTimeSelItem , m_nTimeSelCol, strTime);
        // GetParent()->SendMessage(message, wParam, lParam);

        CString str;
        GetWindowText(str);

        // Send Notification to parent of ListView ctrl
        LV_DISPINFO dispinfo;
        dispinfo.hdr.hwndFrom = m_hWnd;
        dispinfo.hdr.idFrom = GetDlgCtrlID();
        dispinfo.hdr.code = LVN_ENDLABELEDIT;

        dispinfo.item.mask = LVIF_TEXT;
        dispinfo.item.iItem = m_nTimeSelItem;
        dispinfo.item.iSubItem = m_nTimeSelCol;
        dispinfo.item.cchTextMax = str.GetLength();

        int Id = GetDlgCtrlID();

        //GetParent()->SendMessage( WM_NOTIFY, Id, (LPARAM)&dispinfo );
        GetParent()->SendMessage( message, 1, (LPARAM)&dispinfo );
        GetParent()->SendMessage( message, 2, lParam);
        return TRUE;
    }

    if( message == WM_INPLACE_COMBO_KILLFOUCUS )
    {
        GetParent()->SendMessage( message, wParam, lParam );
        return TRUE;
    }
    return CListCtrl::DefWindowProc(message, wParam, lParam);
}


// as if click on the first column of the first empty line.
void CListCtrlEx::EditNew()
{
    SetFocus();

    int nCount = GetItemCount();

    int nNewIdx = -1;

    if (nCount > 0 && IsPreItemEmpty(nCount))
        nNewIdx = nCount - 1;
    else
        nNewIdx = InsertItem( nCount, _T(""));

    SetItemState( nNewIdx, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED );
    CurrentSelection = nNewIdx;

    UINT flag = LVIS_FOCUSED;
    switch(((LVCOLDROPLIST*)ddStyleList[0])->Style)
    {
    case NO_EDIT:
        return;
        break;

    case DROP_DOWN:

        if(GetItemState(CurrentSelection, flag) & flag)
        {
            // Add check for LVS_EDITLABELS
            // if(GetWindowLong(m_hWnd, GWL_STYLE) & LVS_EDITLABELS)
            ShowInPlaceList(CurrentSelection, 0);
        }
        else
            SetItemState(CurrentSelection, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
        break;

    case SPIN_EDIT:
        EditSpinLabel( CurrentSelection, 0 );
        break;

    case DATE_TIME:
        {
            COleDateTime oTime;
            oTime.SetTime( 0, 0, 0 );
            CString csTime = oTime.Format(_T("%X"));
            SetItemText( CurrentSelection, 0, csTime );
            EditDateTimeLabel(CurrentSelection, 0);
            break;
        }
    case POPUP_GATE_SELECTION:
        break;
    case CHECKBOX:
        break;
    default:
        EditSubLabel( CurrentSelection, 0 );
        break;
    }
}

BOOL CListCtrlEx::IsPreItemEmpty(int nItem)
{
    ASSERT(nItem >= 0);
    if (nItem < 1 || nItem > GetItemCount())
        return FALSE;

    int nPre = nItem - 1;

    BOOL bEmpty = TRUE;
    CHeaderCtrl* pHeader = GetHeaderCtrl();
    int nColumnCount = pHeader->GetItemCount();
    for (int col = 0; col < nColumnCount; col++)
    {
        CString str = GetItemText(nPre, col);
        str.TrimLeft(); str.TrimRight();
        if (!str.IsEmpty())
        {
            bEmpty = FALSE;
            break;
        }
    }

    return bEmpty;
}

BOOL CListCtrlEx::IsItemEmpty(int nItem)
{
    ASSERT(nItem >= 0);
    if ( nItem >= GetItemCount())
        return TRUE;

    int nPre = nItem ;

    BOOL bEmpty = TRUE;
    CHeaderCtrl* pHeader = GetHeaderCtrl();
    int nColumnCount = pHeader->GetItemCount();
    for (int col = 0; col < nColumnCount; col++)
    {
        CString str = GetItemText(nPre, col);
        str.TrimLeft(); str.TrimRight();
        if (!str.IsEmpty())
        {
            bEmpty = FALSE;
            break;
        }
    }

    return bEmpty;
}

COleDateTime* CListCtrlEx::EditDateTimeLabel(int nItem, int nCol)
{
    if(!EnsureVisible(nItem, TRUE)) 
        return NULL;

    m_nTimeSelItem = nItem;
    m_nTimeSelCol = nCol;

    // Set title and percent
    CString strTime;
    strTime = GetItemText( nItem, nCol );

    if (wcscmp(strTime , _T("")) == 0) 
    {
        strTime="00:00:00";
    }

    COleDateTime dt;
    dt.ParseDateTime( strTime );

    // Create or move it
    // Make sure that nCol is valid
    CHeaderCtrl* pHeader = GetHeaderCtrl();
    int nColumnCount = pHeader->GetItemCount();
    if(nCol >= nColumnCount || GetColumnWidth(nCol) < 5)
        return NULL;

    // Get the column offset
    int offset = 0;
    for(int i = 0; i < nCol; i++)
        offset += GetColumnWidth(i);

    CRect rect;
    GetItemRect(nItem, &rect, LVIR_BOUNDS);

    // Now scroll if we need to expose the column
    CRect rcClient;
    GetClientRect(&rcClient);
    if(offset + rect.left < 0 || offset + rect.left > rcClient.right)
    {
        CSize size;
        size.cx = offset + rect.left;
        size.cy = 0;
        Scroll(size);
        rect.left -= size.cx;
    }

    rect.left += offset+4;
    rect.right = rect.left + GetColumnWidth(nCol) - 3 ;
    if(rect.right > rcClient.right) 
        rect.right = rcClient.right;
    rect.bottom = rect.top + 20;

    if (!m_inPlaceDateTimeCtrl.GetSafeHwnd())
    {
        m_inPlaceDateTimeCtrl.Create( DTS_TIMEFORMAT, rect,this, ID_DATETIMEPICKER_TIME );
        if (m_bHHmmFormat)
        {
            m_inPlaceDateTimeCtrl.SetFormat(_T("HH:mm"));
        }
    }
    else
        m_inPlaceDateTimeCtrl.MoveWindow(&rect);

    m_inPlaceDateTimeCtrl.SetTime(dt);
    m_inPlaceDateTimeCtrl.ShowWindow(SW_SHOW);
    m_inPlaceDateTimeCtrl.BringWindowToTop();
    m_inPlaceDateTimeCtrl.SetFocus();

    return NULL;  // USER CAN NOT CHARGE IT.
}


// // to simulate the double-click on CListCtrlEx
void CListCtrlEx::DblClickItem(int index, int column)
{
    ASSERT(index>=0 && index<GetItemCount());
    //ASSERT(column>=0 && column<???);

    ::SendMessage( this->GetParent()->GetSafeHwnd(), WM_COLLUM_INDEX,WPARAM ( index ),column); 
    if( GetItemData( index ) == -1 )
        return;

    CurrentSelection = index;
    m_nLastDblClkRow = index;
    m_nLastDlbClkColumn = column;

    UINT flag = LVIS_FOCUSED;
    switch(((LVCOLDROPLIST*)ddStyleList[column])->Style)
    {
    case NO_EDIT:
        GetParent()->SendMessage( WM_NOEDIT_DBCLICK, index, column );
        return;
        break;
    case DROP_DOWN:
        if(GetItemState(index, flag) & flag)
        {
            // Add check for LVS_EDITLABELS
            // if(GetWindowLong(m_hWnd, GWL_STYLE) & LVS_EDITLABELS)
            ShowInPlaceList(index, column);
        }
        else
            SetItemState(index, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
        break;
    case SPIN_EDIT:
        EditSpinLabel( index, column);
        break;
    case DATE_TIME:
        EditDateTimeLabel( index, column);
        break;
    case POPUP_GATE_SELECTION:
        break;
    case CHECKBOX:
        break;
    default:
        EditSubLabel( index, column);
        break;
    }
}


int CListCtrlEx::GetLastDblClk(int& column)
{
    column = m_nLastDlbClkColumn;
    return m_nLastDblClkRow;
}

int CListCtrlEx::GetCurSel( void ) const
{
    return CurrentSelection;
}

void CListCtrlEx::SetHHmmTimeFormat()
{
    m_bHHmmFormat = TRUE;
}

void CListCtrlEx::SetEditSpinRange( int _low,int _Upp )
{
    m_SpinEdit.m_nSpinEditLower=_low; m_SpinEdit.m_nSpinEditUpper = _Upp;
}

void CListCtrlEx::SetSpinDisplayType( int _iType )
{
    m_iSpinDisplayType = _iType ;
}
