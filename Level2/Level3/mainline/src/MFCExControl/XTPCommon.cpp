#include "StdAfx.h"
#include "XTPCommon.h"


CXTPClientRect::CXTPClientRect(HWND hWnd)
{
	if (::IsWindow(hWnd))
		::GetClientRect(hWnd, this);
	else
		SetRectEmpty();
}

CXTPClientRect::CXTPClientRect(const CWnd* pWnd)
{
	if (::IsWindow(pWnd->GetSafeHwnd()))
		::GetClientRect(pWnd->GetSafeHwnd(), this);
	else
		SetRectEmpty();
}


//===========================================================================
// CXTPWindowRect class
//===========================================================================

CXTPWindowRect::CXTPWindowRect(HWND hWnd)
{
	if (::IsWindow(hWnd))
		::GetWindowRect(hWnd, this);
	else
		SetRectEmpty();
}

CXTPWindowRect::CXTPWindowRect(const CWnd* pWnd)
{
	if (::IsWindow(pWnd->GetSafeHwnd()))
		::GetWindowRect(pWnd->GetSafeHwnd(), this);
	else
		SetRectEmpty();
}
//===========================================================================
// CXTPBufferDC class
//===========================================================================

CXTPBufferDC::CXTPBufferDC(HDC hDestDC, const CRect& rcPaint)
	: m_hDestDC (hDestDC)
{
	m_rect = rcPaint;
	Attach (::CreateCompatibleDC (m_hDestDC));
	if (!m_hDC)
		return;

	m_bitmap.Attach (::CreateCompatibleBitmap(
		m_hDestDC, m_rect.right, m_rect.bottom));
	m_hOldBitmap = ::SelectObject (m_hDC, m_bitmap);
}

//CXTPBufferDC::CXTPBufferDC(HDC hDestDC, const CRect& rcPaint, const CXTPPaintManagerColorGradient& clrBack, const BOOL bHorz /*=FALSE*/)
//	: m_hDestDC (hDestDC)
//{
//	m_rect = rcPaint;
//	Attach (::CreateCompatibleDC (m_hDestDC));
//	if (!m_hDC)
//		return;
//
//	m_bitmap.Attach (::CreateCompatibleBitmap(
//		m_hDestDC, m_rect.right, m_rect.bottom));
//	m_hOldBitmap = ::SelectObject (m_hDC, m_bitmap);
//
//	if (!clrBack.IsNull())
//	{
//		XTPDrawHelpers()->GradientFill(this, m_rect, clrBack, bHorz);
//	}
//}

CXTPBufferDC::CXTPBufferDC(CPaintDC& paintDC)
{
	m_hDestDC = paintDC.GetSafeHdc();
	m_rect = paintDC.m_ps.rcPaint;

	Attach (::CreateCompatibleDC (m_hDestDC));
	if (!m_hDC)
		return;

	m_bitmap.Attach (::CreateCompatibleBitmap(
		m_hDestDC, max(1, m_rect.right), max(1, m_rect.bottom)));
	m_hOldBitmap = ::SelectObject (m_hDC, m_bitmap);

	CRgn rgn;
	rgn.CreateRectRgnIndirect(&m_rect);

	SelectClipRgn(&rgn);
}

CXTPBufferDC::~CXTPBufferDC()
{
	if (!m_hDC)
		return;

	if (m_hDestDC)
	{
		::BitBlt (m_hDestDC, m_rect.left, m_rect.top, m_rect.Width(),
			m_rect.Height(), m_hDC, m_rect.left, m_rect.top, SRCCOPY);
	}
	::SelectObject (m_hDC, m_hOldBitmap);
}
void CXTPBufferDC::Discard()
{
	m_hDestDC = 0;
}

CDC* CXTPBufferDC::GetDestDC()
{
	return CDC::FromHandle(m_hDestDC);
}

void CXTPBufferDC::TakeSnapshot()
{
	::BitBlt (m_hDC, m_rect.left, m_rect.top, m_rect.Width(),
		m_rect.Height(), m_hDestDC, m_rect.left, m_rect.top, SRCCOPY);
}