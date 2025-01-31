#include "stdafx.h"
#include "CARCTipEdit.h"
#include ".\carctipedit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

///////////////////////////////////////////////////////////////////////////////
// CARCTipEdit

BEGIN_MESSAGE_MAP(CARCTipEdit, CEdit)
	//{{AFX_MSG_MAP(CARCTipEdit)
	ON_WM_MOUSEMOVE()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
	ON_WM_CREATE()
END_MESSAGE_MAP()

///////////////////////////////////////////////////////////////////////////////
// ctor
CARCTipEdit::CARCTipEdit()
{
	m_hWndToolTip = NULL;
}

///////////////////////////////////////////////////////////////////////////////
// PreSubclassWindow
void CARCTipEdit::PreSubclassWindow() 
{
	TRACE(_T("in CARCTipEdit::PreSubclassWindow\n"));

	CEdit::PreSubclassWindow();

	// create tooltip
	//if(!m_hWndToolTip)
	//{
	//	m_hWndToolTip = ::CreateWindowEx(WS_EX_TOPMOST,
	//		TOOLTIPS_CLASS,
	//		NULL,
	//		TTS_NOPREFIX | TTS_ALWAYSTIP,
	//		CW_USEDEFAULT,
	//		CW_USEDEFAULT,
	//		CW_USEDEFAULT,
	//		CW_USEDEFAULT,
	//		m_hWnd,
	//		NULL,
	//		NULL,
	//		NULL);
	//	ASSERT(m_hWndToolTip);

	//	// initialize toolinfo struct
	//	memset(&m_ToolInfo, 0, sizeof(m_ToolInfo));
	//	m_ToolInfo.cbSize = sizeof(m_ToolInfo);
	//	m_ToolInfo.uFlags = TTF_TRACK | TTF_TRANSPARENT;
	//	m_ToolInfo.hwnd = m_hWnd;

	//	// add list box
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
// OnMouseMove
void CARCTipEdit::OnMouseMove(UINT nFlags, CPoint point) 
{
	CRect rectClient;
	GetClientRect(&rectClient);
	int nEditWidth = ::GetSystemMetrics(SM_CXHTHUMB) + 2;
	rectClient.right = rectClient.right - nEditWidth;

	if (rectClient.PtInRect(point))
	{
		TRACE(_T("in ccombo\n"));
		ClientToScreen(&rectClient);

		CString strText = _T("");
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

	CEdit::OnMouseMove(nFlags, point);
}

///////////////////////////////////////////////////////////////////////////////
// OnTimer
void CARCTipEdit::OnTimer(UINT nIDEvent) 
{
	CPoint point;
	::GetCursorPos(&point);
	ScreenToClient(&point);

	CRect rectClient;
	GetClientRect(&rectClient);
	int nEditWidth = ::GetSystemMetrics(SM_CXHTHUMB) + 2;

	rectClient.right = rectClient.right - nEditWidth;

	if (!rectClient.PtInRect(point))
	{
		TRACE(_T("killing timer\n"));
		KillTimer(nIDEvent);
		::SendMessage(m_hWndToolTip, TTM_TRACKACTIVATE, FALSE, (LPARAM)(LPTOOLINFO) &m_ToolInfo);
	}
}


int CARCTipEdit::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CEdit::OnCreate(lpCreateStruct) == -1)
		return -1;

	if(!m_hWndToolTip)
	{
		m_hWndToolTip = ::CreateWindowEx(WS_EX_TOPMOST,
			TOOLTIPS_CLASS,
			NULL,
			TTS_NOPREFIX | TTS_ALWAYSTIP,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			m_hWnd,
			NULL,
			NULL,
			NULL);
		ASSERT(m_hWndToolTip);

		// initialize toolinfo struct
		memset(&m_ToolInfo, 0, sizeof(m_ToolInfo));
		m_ToolInfo.cbSize = sizeof(m_ToolInfo);
		m_ToolInfo.uFlags = TTF_TRACK | TTF_TRANSPARENT;
		m_ToolInfo.hwnd = m_hWnd;

		// add list box
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
