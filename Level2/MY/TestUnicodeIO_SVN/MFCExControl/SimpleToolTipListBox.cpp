#include "stdafx.h"
#include "SimpleToolTipListBox.h"
// CSimpleToolTipListBox

IMPLEMENT_DYNAMIC(CSimpleToolTipListBox, CListBox)
CSimpleToolTipListBox::CSimpleToolTipListBox()
{
}

CSimpleToolTipListBox::~CSimpleToolTipListBox()
{
}


BEGIN_MESSAGE_MAP(CSimpleToolTipListBox, CListBox)
	ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTW, 0, 0xFFFF, OnToolTipText)
	ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTA, 0, 0xFFFF, OnToolTipText)
END_MESSAGE_MAP()



// CSimpleToolTipListBox message handlers
void CSimpleToolTipListBox::PreSubclassWindow() 
{
	CListBox::PreSubclassWindow();

	EnableToolTips(TRUE);
}

int CSimpleToolTipListBox::OnToolHitTest(CPoint point, TOOLINFO * pTI) const
{
	int row;
	RECT cellrect;
	BOOL tmp = FALSE;
	row  = ItemFromPoint(point,tmp);

	if ( row == -1 ) return -1;

	GetItemRect(row,&cellrect);
	pTI->rect = cellrect;
	pTI->hwnd = m_hWnd;
	pTI->uId = (UINT)((row));
	pTI->lpszText = LPSTR_TEXTCALLBACK;
	return (int)pTI->uId;

}


BOOL CSimpleToolTipListBox::OnToolTipText( UINT id, NMHDR * pNMHDR, LRESULT * pResult )
{
	TOOLTIPTEXTA* pTTTA = (TOOLTIPTEXTA*)pNMHDR;
	TOOLTIPTEXTW* pTTTW = (TOOLTIPTEXTW*)pNMHDR;
	CString strTipText;
	UINT nID = (UINT)pNMHDR->idFrom;


	GetText( nID ,strTipText);

#ifndef _UNICODE
	if (pNMHDR->code == TTN_NEEDTEXTA)
		lstrcpyn(pTTTA->szText, strTipText, 255);
	else
		_mbstowcsz(pTTTW->szText, strTipText, 255);
#else
	if (pNMHDR->code == TTN_NEEDTEXTA)
		_wcstombsz(pTTTA->szText, strTipText, 255);
	else
		lstrcpyn(pTTTW->szText, strTipText, 255);
#endif
	*pResult = 0;

	return TRUE;    
}

