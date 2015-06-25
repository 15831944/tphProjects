// EditListCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "EditListCtrl.h"
#include <windowsx.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//#define IDC_EDIT  0X01
#define PROPERTY_ITEM 0x02
#define PROPERTY_SUB  0x03
/////////////////////////////////////////////////////////////////////////////
// CEditListCtrl

CEditListCtrl::CEditListCtrl()
{
   m_edit.m_hWnd = NULL;	
}

CEditListCtrl::~CEditListCtrl()
{

}

BEGIN_MESSAGE_MAP(CEditListCtrl, CListCtrl)
	//{{AFX_MSG_MAP(CEditListCtrl)
	ON_WM_LBUTTONDBLCLK()
	ON_WM_PARENTNOTIFY()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_USER_EDIT_END,OnEditEnd)
END_MESSAGE_MAP()


void CEditListCtrl::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	CRect rcCtrl;
	LVHITTESTINFO lvhti;
    lvhti.pt = point;
	int nItem = CListCtrl::SubItemHitTest(&lvhti);
	if(nItem == -1)
	   return;
	int nSubItem = lvhti.iSubItem;
	CListCtrl::GetSubItemRect(nItem,nSubItem,LVIR_LABEL,rcCtrl); 
    ShowEdit(TRUE,nItem,nSubItem,rcCtrl);
	CListCtrl::OnLButtonDblClk(nFlags, point);
}
void CEditListCtrl::ShowEdit(BOOL bShow,int nItem,int nIndex,CRect rcCtrl)
{
    if(m_edit.m_hWnd == NULL)
	{
		m_edit.Create(ES_AUTOHSCROLL|WS_CHILD|ES_LEFT|ES_WANTRETURN|WS_BORDER,CRect(0,0,0,0),this,IDC_EDIT);
		m_edit.ShowWindow(SW_HIDE);

		CFont tpFont;
		tpFont.CreateStockObject(DEFAULT_GUI_FONT);
		m_edit.SetFont(&tpFont);
		tpFont.DeleteObject();
	}
	if(bShow == TRUE)
	{
		CString strItem = CListCtrl::GetItemText(nItem,nIndex);
		m_edit.MoveWindow(rcCtrl);
		m_edit.ShowWindow(SW_SHOW);
		m_edit.SetWindowText(strItem);
		::SetFocus(m_edit.GetSafeHwnd());
		m_edit.SetSel(-1);  
		m_edit.SetCtrlData(MAKEWPARAM(nIndex,nItem));	
	}
	else
		m_edit.ShowWindow(SW_HIDE);
}
LRESULT CEditListCtrl::OnEditEnd(WPARAM wParam,LPARAM lParam)
{
	if(wParam == TRUE)
	{
		CString strText(_T(""));
		m_edit.GetWindowText(strText);
		DWORD dwData = m_edit.GetCtrlData();
		int nItem= dwData>>16;
		int nIndex = dwData&0x0000ffff;
		CListCtrl::SetItemText(nItem,nIndex,strText);
	}
	else
	{
	   
	}

    if(lParam == FALSE)
	    m_edit.ShowWindow(SW_HIDE);
	return 0L;
}

void CEditListCtrl::OnParentNotify(UINT message, LPARAM lParam) 
{
	CListCtrl::OnParentNotify(message, lParam);
	//////////////////////////////////////////////////////////////////////////
	CHeaderCtrl* pHeaderCtrl = CListCtrl::GetHeaderCtrl();
	if(pHeaderCtrl == NULL)
		return;
	CRect rcHeader;
	pHeaderCtrl->GetWindowRect(rcHeader);
	ScreenToClient(rcHeader);
	//The x coordinate is in the low-order word and the y coordinate is in the high-order word.
	CPoint pt;
	pt.x = GET_X_LPARAM(lParam);
	pt.y = GET_Y_LPARAM(lParam);
	if(rcHeader.PtInRect(pt) && message == WM_LBUTTONDOWN)
	{
		if(m_edit.m_hWnd != NULL)
		{
			DWORD dwStyle = m_edit.GetStyle();
			if((dwStyle&WS_VISIBLE) == WS_VISIBLE)
			{
				m_edit.ShowWindow(SW_HIDE);
			}
		}	
	}	
}
BOOL CEditListCtrl::PreTranslateMessage(MSG* pMsg) 
{
	if(pMsg->message == WM_KEYDOWN)
	{
		if(pMsg->wParam == VK_TAB && m_edit.m_hWnd!= NULL)
		{
		 	DWORD dwStyle = m_edit.GetStyle();
			if((dwStyle&WS_VISIBLE) == WS_VISIBLE)
			{
			     OnEditEnd(TRUE,TRUE);
				 CRect rcCtrl;		 
				 int nItem;
				 int nSub;
				 if(FALSE == Key_Ctrl(nItem,nSub))
                    Key_Shift(nItem,nSub);
                 CListCtrl::GetSubItemRect(nItem,nSub,LVIR_LABEL,rcCtrl); 
				 CPoint pt(rcCtrl.left+1,rcCtrl.top+1);
				 OnLButtonDblClk(0,pt);
				 POSITION pos = CListCtrl::GetFirstSelectedItemPosition();
				 if (pos == NULL)
				 {
				 }
				 else
				 {
					 while (pos)
					 {
						 int ntpItem = CListCtrl::GetNextSelectedItem(pos);
						 CListCtrl::SetItemState(ntpItem,0,LVIS_SELECTED);
					 }
				 }
				 CListCtrl::SetItemState(nItem,  LVIS_SELECTED,  LVIS_SELECTED);
				 return TRUE;
			}
		}
	}
	
	return CListCtrl::PreTranslateMessage(pMsg);
}
BOOL CEditListCtrl::Key_Shift(int& nItem,int& nSub)
{
	int nItemCount = CListCtrl::GetItemCount();
	DWORD dwData = m_edit.GetCtrlData();
	nItem= dwData>>16;
	nSub = dwData&0x0000ffff;
	
	CHeaderCtrl* pHeader = CListCtrl::GetHeaderCtrl();
	if(pHeader == NULL)
		return FALSE;
	
	short sRet = GetKeyState(VK_SHIFT);
	int nSubcCount = pHeader->GetItemCount();
	sRet = sRet >>15;
	if(sRet == 0)
	{
		nSub += 1;
		if(nSub >= nSubcCount)
		{
			if(nItem == nItemCount-1)
			{
				nItem = 0;
				nSub  = 0;
			}
			else
			{
				nSub = 0;
				nItem += 1;
			}
		}
		if(nItem >= nItemCount)
			nItem = nItemCount-1;
		return FALSE;
	}
	else
	{
		nSub -= 1;
		if(nSub < 0)
		{
			
			nSub = nSubcCount -1;
			nItem --;
		}
		
		if(nItem < 0)
			nItem = nItemCount-1;
		return TRUE;
		
	}
	return FALSE;
}
BOOL CEditListCtrl::Key_Ctrl(int& nItem,int &nSub)
{
    short sRet = GetKeyState(VK_CONTROL);
	DWORD dwData = m_edit.GetCtrlData();
	nItem= dwData>>16;
	nSub = dwData&0x0000ffff;
	
	sRet = sRet >>15;
	int nItemCount = CListCtrl::GetItemCount();
	if(sRet == 0)
	{
		
	}
	else
	{
		nItem = nItem >=nItemCount-1? 0:nItem+=1;
		return TRUE;
	}
	
	return FALSE;
}
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

CListCtrlEdit::CListCtrlEdit()
{
}

CListCtrlEdit::~CListCtrlEdit()
{
}


BEGIN_MESSAGE_MAP(CListCtrlEdit, CEdit)
	//{{AFX_MSG_MAP(CListCtrlEdit)
	ON_WM_KILLFOCUS()
	ON_WM_SETFOCUS()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CListCtrlEdit message handlers
void CListCtrlEdit::SetCtrlData(DWORD dwData)
{
	m_dwData = dwData;
}

DWORD CListCtrlEdit::GetCtrlData()
{
   return m_dwData;
}

void CListCtrlEdit::OnKillFocus(CWnd* pNewWnd) 
{
	CEdit::OnKillFocus(pNewWnd);
	CWnd* pParent = this->GetParent();
	::PostMessage(pParent->GetSafeHwnd(),WM_USER_EDIT_END,m_bExchange,0);
}
BOOL CListCtrlEdit::PreTranslateMessage(MSG* pMsg) 
{
	
	if(pMsg->message == WM_KEYDOWN)
	{
		if(pMsg->wParam == VK_RETURN)
		{
			CWnd* pParent = this->GetParent();
			m_bExchange = TRUE;
			::PostMessage(pParent->GetSafeHwnd(),WM_USER_EDIT_END,m_bExchange,0);
		}
		else if(pMsg->wParam == VK_ESCAPE)
		{
           CWnd* pParent = this->GetParent();
		   m_bExchange = FALSE;
			::PostMessage(pParent->GetSafeHwnd(),WM_USER_EDIT_END,m_bExchange,0);
		}
	}
	
	return CEdit::PreTranslateMessage(pMsg);
}

void CListCtrlEdit::OnSetFocus(CWnd* pOldWnd) 
{
	CEdit::OnSetFocus(pOldWnd);
    m_bExchange = TRUE;	
}

