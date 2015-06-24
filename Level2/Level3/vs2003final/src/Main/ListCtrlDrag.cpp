// ListCtrlDrag.cpp : implementation file
//

#include "stdafx.h"
#include "termplan.h"
#include "ListCtrlDrag.h"
#include "ImportFlightFromFileDlg.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CListCtrlDrag

CListCtrlDrag::CListCtrlDrag()
{
	m_nColIndexDragged=-1;
}

CListCtrlDrag::~CListCtrlDrag()
{
}


BEGIN_MESSAGE_MAP(CListCtrlDrag, CListCtrl)
	//{{AFX_MSG_MAP(CListCtrlDrag)
	ON_WM_VSCROLL()
	ON_WM_HSCROLL()
	ON_WM_KEYDOWN()
	ON_WM_MOUSEWHEEL()
	ON_WM_LBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CListCtrlDrag message handlers

void CListCtrlDrag::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	if (nSBCode/256==SB_THUMBTRACK || (nSBCode & 0xFF)==SB_THUMBTRACK ||
        nSBCode/256==SB_THUMBPOSITION || (nSBCode & 0xFF)==SB_THUMBPOSITION)
    {
        SCROLLINFO sinfo;
        sinfo.cbSize=sizeof(sinfo);
        sinfo.fMask=SIF_TRACKPOS;
        ::GetScrollInfo(m_hWnd, SB_VERT, &sinfo);
        nPos=sinfo.nTrackPos;
    }
	
    CListCtrl::OnVScroll(nSBCode, nPos, pScrollBar);
    //m_pViewPos->VertSynchro(m_nNumCtrl, nSBCode, nPos, pScrollBar);
}

void CListCtrlDrag::RedirectHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
    // Only for WinNT/2000/XP - the beginning of the code
    if (nSBCode/256==SB_THUMBTRACK || (nSBCode & 0xFF)==SB_THUMBTRACK)
    {
        int iX = ((int)nPos - (int)GetScrollPos(SB_HORZ));
        Scroll(CSize(iX, 0));
    }
    // Only for WinNT/2000/XP - the end of the code
    CListCtrl::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CListCtrlDrag::RedirectVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
    // Only for WinNT/2000/XP - the beginning of the code
    if (nSBCode/256==SB_THUMBTRACK || (nSBCode & 0xFF)==SB_THUMBTRACK)
    {
        int iY = ((int)nPos - (int)GetScrollPos(SB_VERT)) * m_nItemHeight;
        Scroll(CSize(0, iY));
    }
    // Only for WinNT/2000/XP - the end of the code
    CListCtrl::OnVScroll(nSBCode, nPos, pScrollBar);
}

void CListCtrlDrag::RedirectKeyScroll(UINT nChar)
{
    switch (nChar)
    {
	case VK_UP:
		OnVScroll(SB_LINEUP, 0, NULL);
		break;
	case VK_DOWN:
		OnVScroll(SB_LINEDOWN, 0, NULL);
		break;
	case VK_LEFT:
		OnHScroll(SB_LINELEFT, 0, NULL);
		break;
	case VK_RIGHT:
		OnHScroll(SB_LINERIGHT, 0, NULL);
		break;
	case VK_HOME:
		OnVScroll(SB_TOP, 0, NULL);
		break;
	case VK_END:
		OnVScroll(SB_BOTTOM,0,NULL);
		break;
	case VK_PRIOR:
		OnVScroll(SB_PAGEUP, 0, NULL);
		break;
	case VK_NEXT:
		OnVScroll(SB_PAGEDOWN, 0, NULL);
		break;
    }


}

int CListCtrlDrag::GetItemHeight()
{
	CRect ItemRect;
	GetSubItemRect(1, 1, LVIR_BOUNDS, ItemRect);
	return ItemRect.bottom - ItemRect.top;
	
}

void CListCtrlDrag::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
    CListCtrl::OnHScroll(nSBCode, nPos, pScrollBar);
    //m_pViewPos->HorzSynchro(m_nNumCtrl, nSBCode, nPos, pScrollBar);
}

void CListCtrlDrag::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	//m_pViewPos->KeySynchro(m_nNumCtrl, nChar);

}

void CListCtrlDrag::RedirectSelectSynchro(UINT nFlags, CPoint point)
{
	CListCtrl::OnLButtonDown(nFlags,point);
	//SetItemState( unIndex , LVIS_SELECTED, LVIS_SELECTED);
}

BOOL CListCtrlDrag::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) 
{
	// TODO: Add your message handler code here and/or call default
	
	BOOL b=CListCtrl::OnMouseWheel(nFlags, zDelta, pt);
	//m_pViewPos->MouseWheel(m_nNumCtrl,nFlags,zDelta,pt);
	return b;
}


void CListCtrlDrag::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	CListCtrl::OnLButtonDown(nFlags, point);
	POSITION pos = GetFirstSelectedItemPosition();
	while (pos)
	{
		int nItem = GetNextSelectedItem(pos);
		//m_pViewPos->SelectSynchro(m_nNumCtrl,nFlags,point);
	}	
}

void CListCtrlDrag::RedirectMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	CListCtrl::OnMouseWheel(nFlags,zDelta,pt);
}

void CListCtrlDrag::BeginDrag(CPoint point,int nColNum)
{
	NM_LISTVIEW nmLV;
	nmLV.ptAction=point;
	m_nColIndexDragged=GetColIndexUnderPoint(point,nColNum);
	CWnd* pWnd=GetParent();
	if(m_nColIndexDragged!=-1)
	{
		pWnd->SendMessage(UM_BEGIN_DRAG,(WPARAM)m_nColIndexDragged,(LPARAM)this);
	}

	
}

void CListCtrlDrag::SubClassHeaderCtrl()
{
	m_headerCtrl.SubclassWindow( ::GetDlgItem(m_hWnd,0) );
	
}

void CListCtrlDrag::EndDrag()
{
	if(m_headerCtrl.m_hWnd)
	{
		m_headerCtrl.OnLButtonUp(NULL,CPoint(0,0));
		m_nColIndexDragged=-1;
		GetHeaderCtrl()->Invalidate();
	}
}

int CListCtrlDrag::GetColIndexUnderPoint(CPoint point,int nColNum)
{
	int i=0, cx = 0;
	if( point.x > 0 )
	{
		for( i = 0; i < nColNum; i++ )
		{
			
			if( point.x >= cx && point.x < cx + GetColumnWidth(i) )
				break;
			cx += GetColumnWidth(i);
		}
		return i;
	}
	return -1;
}
