// HeaderCtrlDrag.cpp : implementation file
//

#include "stdafx.h"
#include "termplan.h"
#include "HeaderCtrlDrag.h"
#include "ListCtrlDrag.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CHeaderCtrlDrag

CHeaderCtrlDrag::CHeaderCtrlDrag()
{
	m_bDragging=FALSE;
}

CHeaderCtrlDrag::~CHeaderCtrlDrag()
{
}


BEGIN_MESSAGE_MAP(CHeaderCtrlDrag, CHeaderCtrl)
	//{{AFX_MSG_MAP(CHeaderCtrlDrag)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHeaderCtrlDrag message handlers

void CHeaderCtrlDrag::OnMouseMove(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	if(m_bDragging)
	{
		CWnd* pWnd=GetParent();
		CListCtrlDrag* pLCD=(CListCtrlDrag*)pWnd;
		pLCD->BeginDrag(point,GetItemCount());
		
	}
	else
		CHeaderCtrl::OnMouseMove(nFlags, point);
}

void CHeaderCtrlDrag::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	if(!IsResizingItemWidth(point))
	{
		m_bDragging=TRUE;
		CWnd* pWnd=GetParent();
		CListCtrlDrag* pLCD=(CListCtrlDrag*)pWnd;
		int nIndexClicked=pLCD->GetColIndexUnderPoint(point,GetItemCount());
		pLCD->GetParent()->SendMessage(UM_HEADERCTRL_CLICK_COLUMN,(WPARAM)nIndexClicked);
	}
	CHeaderCtrl::OnLButtonDown(nFlags, point);
}

void CHeaderCtrlDrag::OnLButtonUp(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	m_bDragging=FALSE;
	CHeaderCtrl::OnLButtonUp(nFlags, point);
}


BOOL CHeaderCtrlDrag::IsResizingItemWidth(CPoint point)
{
	HCURSOR hCursor=GetCursor();
	HCURSOR hCursorStandard=::LoadCursor(NULL,IDC_ARROW);
	if(hCursor==hCursorStandard)
		return FALSE;
	return TRUE;
}
