#include "stdafx.h"
#include "CARCTipComboBox.h"
#include ".\carctipcombobox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

///////////////////////////////////////////////////////////////////////////////
// CARCTipComboBox

BEGIN_MESSAGE_MAP(CARCTipComboBox, CComboBox)
	//{{AFX_MSG_MAP(CARCTipComboBox)
	ON_WM_CTLCOLOR()
	ON_WM_DESTROY()
	ON_WM_MOUSEMOVE()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
	ON_WM_CREATE()
END_MESSAGE_MAP()

///////////////////////////////////////////////////////////////////////////////
// ctor
CARCTipComboBox::CARCTipComboBox()
{
	m_hWndToolTip = NULL;
	m_hExeInstance = NULL;
}

///////////////////////////////////////////////////////////////////////////////
// PreSubclassWindow
void CARCTipComboBox::PreSubclassWindow() 
{
	TRACE(_T("in CARCTipComboBox::PreSubclassWindow\n"));

	CComboBox::PreSubclassWindow();

	// create tooltip
	//if(!m_hWndToolTip)
	//{
	//	m_hWndToolTip = ::CreateWindowEx(WS_EX_TOPMOST,
	//		TOOLTIPS_CLASS,
	//		NULL,
	//		TTS_NOPREFIX | TTS_ALWAYSTIP ,
	//		CW_USEDEFAULT,
	//		CW_USEDEFAULT,
	//		CW_USEDEFAULT,
	//		CW_USEDEFAULT,
	//		m_hWnd,
	//		NULL,
	//		m_hExeInstance,
	//		NULL);
	//		ASSERT(m_hWndToolTip);

	//	// initialize toolinfo struct
	//	memset(&m_ToolInfo, 0, sizeof(m_ToolInfo));
	//	m_ToolInfo.cbSize = sizeof(m_ToolInfo);
	//	m_ToolInfo.uFlags = TTF_TRACK | TTF_TRANSPARENT;
	//	m_ToolInfo.hwnd = m_hWnd;

	//	// add combo box
	//	::SendMessage(m_hWndToolTip, TTM_SETMAXTIPWIDTH, 0, SHRT_MAX);
	//	::SendMessage(m_hWndToolTip, TTM_ADDTOOL, 0, (LPARAM) (LPTOOLINFO) &m_ToolInfo);
	//	::SendMessage(m_hWndToolTip, TTM_SETTIPBKCOLOR, ::GetSysColor(COLOR_HIGHLIGHT), 0);
	//	::SendMessage(m_hWndToolTip, TTM_SETTIPTEXTCOLOR, ::GetSysColor(COLOR_HIGHLIGHTTEXT), 0);

	//	// reduce top & bottom margins
	//	CRect rectMargins(0,-1,0,-1);
	//	::SendMessage(m_hWndToolTip, TTM_SETMARGIN, 0, (LPARAM)&rectMargins);

	//	// set font
	//	CFont *pFont = GetFont();
	//	::SendMessage(m_hWndToolTip, WM_SETFONT, (WPARAM)(HFONT)*pFont, FALSE);
	//}
}

///////////////////////////////////////////////////////////////////////////////
// OnCtlColor
HBRUSH CARCTipComboBox::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	if (nCtlColor == CTLCOLOR_LISTBOX)
	{
		if (m_listbox.GetSafeHwnd() == NULL)
		{
			TRACE(_T("subclassing listbox\n"));
			m_listbox.SubclassWindow(pWnd->GetSafeHwnd());
		}
	}
	HBRUSH hbr = CComboBox::OnCtlColor(pDC, pWnd, nCtlColor);
	return hbr;
}

///////////////////////////////////////////////////////////////////////////////
// OnDestroy
void CARCTipComboBox::OnDestroy() 
{
	if (m_listbox.GetSafeHwnd() != NULL)
		m_listbox.UnsubclassWindow();

	CComboBox::OnDestroy();
}

///////////////////////////////////////////////////////////////////////////////
// OnMouseMove
void CARCTipComboBox::OnMouseMove(UINT nFlags, CPoint point) 
{
	CRect rectClient;
	GetClientRect(&rectClient);
	int nComboButtonWidth = ::GetSystemMetrics(SM_CXHTHUMB) + 2;
	rectClient.right = rectClient.right - nComboButtonWidth;

	if (rectClient.PtInRect(point))
	{
		TRACE(_T("in ccombo\n"));
		ClientToScreen(&rectClient);

		CString strText = (_T(""));
		GetWindowText(strText);
		m_ToolInfo.lpszText = (LPTSTR)(LPCTSTR)strText;

		HDC hDC = ::GetDC(m_hWnd);
		ASSERT(hDC);

		CFont *pFont = GetFont();
		HFONT hOldFont = (HFONT) ::SelectObject(hDC, (HFONT) *pFont);

		SIZE size;
		::GetTextExtentPoint32(hDC, strText, strText.GetLength(), &size);
		::SelectObject(hDC, hOldFont);
		::ReleaseDC(m_hWnd, hDC);

		if (size.cx > (rectClient.Width() - 6))
		{
			rectClient.left += 1;
			rectClient.top += 3;

			COLORREF rgbText = ::GetSysColor(COLOR_WINDOWTEXT);
			COLORREF rgbBackground = ::GetSysColor(COLOR_WINDOW);

			CWnd *pWnd = GetFocus();
			if (pWnd)
			{
				if (pWnd->m_hWnd == m_hWnd)
				{
					rgbText = ::GetSysColor(COLOR_HIGHLIGHTTEXT);
					rgbBackground = ::GetSysColor(COLOR_HIGHLIGHT);
				}
			}

			::SendMessage(m_hWndToolTip, TTM_SETTIPBKCOLOR, rgbBackground, 0);
			::SendMessage(m_hWndToolTip, TTM_SETTIPTEXTCOLOR, rgbText, 0);
			::SendMessage(m_hWndToolTip, TTM_UPDATETIPTEXT, 0, (LPARAM) &m_ToolInfo);
			::SendMessage(m_hWndToolTip, TTM_TRACKPOSITION, 0, 
				(LPARAM) MAKELONG(rectClient.left, rectClient.top));
			::SendMessage(m_hWndToolTip, TTM_TRACKACTIVATE, TRUE, (LPARAM)(LPTOOLINFO) &m_ToolInfo);
			TRACE(_T("setting timer\n"));
			SetTimer(1, 80, NULL);	// set timer for out-of-rect detection
		}
		else	// text fits inside client rect
		{
			::SendMessage(m_hWndToolTip, TTM_TRACKACTIVATE, FALSE, (LPARAM)(LPTOOLINFO) &m_ToolInfo);
		}
	}
	else	// not inside client rect
	{
		::SendMessage(m_hWndToolTip, TTM_TRACKACTIVATE, FALSE, (LPARAM)(LPTOOLINFO) &m_ToolInfo);
	}

	CComboBox::OnMouseMove(nFlags, point);
}

///////////////////////////////////////////////////////////////////////////////
// OnTimer
void CARCTipComboBox::OnTimer(UINT nIDEvent) 
{
	CPoint point;
	::GetCursorPos(&point);
	ScreenToClient(&point);

	CRect rectClient;
	GetClientRect(&rectClient);
	int nComboButtonWidth = ::GetSystemMetrics(SM_CXHTHUMB) + 2;

	rectClient.right = rectClient.right - nComboButtonWidth;

	if (!rectClient.PtInRect(point))
	{
		TRACE(_T("killing timer\n"));
		KillTimer(nIDEvent);
		::SendMessage(m_hWndToolTip, TTM_TRACKACTIVATE, FALSE, (LPARAM)(LPTOOLINFO) &m_ToolInfo);
	}
}


BOOL CARCTipComboBox::Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
{
	// TODO: Add your specialized code here and/or call the base class

	BOOL bReturn =  CComboBox::Create(dwStyle, rect, pParentWnd, nID);	
	return (bReturn);
}

int CARCTipComboBox::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CComboBox::OnCreate(lpCreateStruct) == -1)
		return -1;

	if(!m_hWndToolTip)
	{
		m_hWndToolTip = ::CreateWindowEx(WS_EX_TOPMOST,
			TOOLTIPS_CLASS,
			NULL,
			TTS_NOPREFIX | TTS_ALWAYSTIP ,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			m_hWnd,
			NULL,
			m_hExeInstance,
			NULL);
			ASSERT(m_hWndToolTip);

		// initialize toolinfo struct
		memset(&m_ToolInfo, 0, sizeof(m_ToolInfo));
		m_ToolInfo.cbSize = sizeof(m_ToolInfo);
		m_ToolInfo.uFlags = TTF_TRACK | TTF_TRANSPARENT;
		m_ToolInfo.hwnd = m_hWnd;

		// add combo box
		::SendMessage(m_hWndToolTip, TTM_SETMAXTIPWIDTH, 0, SHRT_MAX);
		::SendMessage(m_hWndToolTip, TTM_ADDTOOL, 0, (LPARAM) (LPTOOLINFO) &m_ToolInfo);
		::SendMessage(m_hWndToolTip, TTM_SETTIPBKCOLOR, ::GetSysColor(COLOR_HIGHLIGHT), 0);
		::SendMessage(m_hWndToolTip, TTM_SETTIPTEXTCOLOR, ::GetSysColor(COLOR_HIGHLIGHTTEXT), 0);

		// reduce top & bottom margins
		CRect rectMargins(0,-1,0,-1);
		::SendMessage(m_hWndToolTip, TTM_SETMARGIN, 0, (LPARAM)&rectMargins);

		// set font
		CFont *pFont = GetFont();
		::SendMessage(m_hWndToolTip, WM_SETFONT, (WPARAM)(HFONT)*pFont, FALSE);
	}
	return 0;
}
