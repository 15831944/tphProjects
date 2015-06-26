// MySpin.cpp : implementation file
//

#include "stdafx.h"
#include "WinMsg.h"
#include "MySpin.h"
#include "resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

static const int ID_MYSPIN_SPIN = 101;
static const int ID_MYSPIN_EDIT = 102;


/////////////////////////////////////////////////////////////////////////////
// CMySpin

CMySpin::CMySpin():m_nSpinEditLower(0),m_nSpinEditUpper(100)
{
	m_strTitle.Empty();
	m_iPercent = 0;
	m_iDisplayType=0;
	m_bSetPosFlag = true;
}

CMySpin::~CMySpin()
{
}


BEGIN_MESSAGE_MAP(CMySpin, CWnd)
	ON_WM_SIZE()
	ON_WM_CREATE()
	ON_WM_KILLFOCUS()
	ON_NOTIFY(UDN_DELTAPOS, ID_MYSPIN_SPIN, OnDeltaposSpin)
	ON_EN_KILLFOCUS(ID_MYSPIN_EDIT, OnKillfocusMyEdit)	
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CMySpin message handlers

int CMySpin::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	CFont* font = GetParent()->GetFont();

	CRect rcClient, rcEdit, rcSpin;
	GetClientRect(&rcClient);

	int iSpinWidth = 15;

	// Edit
	DWORD dwEditStyle = WS_CHILD | WS_VISIBLE;

	rcEdit.left   = rcClient.left;
	rcEdit.top    = rcClient.top;
	rcEdit.right  = rcEdit.left + (rcClient.Width() - iSpinWidth);
	rcEdit.bottom = rcEdit.top + rcClient.Height();	
	m_Edit.Create(dwEditStyle, rcEdit, this, ID_MYSPIN_EDIT);
	m_Edit.SetFont( font );

	// Spin
	DWORD dwSpinStyle = WS_CHILD | WS_VISIBLE;

	rcSpin.left   = rcEdit.right;
	rcSpin.top    = rcEdit.top;
	rcSpin.right  = rcClient.right;
	rcSpin.bottom = rcEdit.bottom;
	m_Spin.Create(dwSpinStyle, rcSpin, this, ID_MYSPIN_SPIN);
	m_Spin.SetAccel(0, NULL);
	m_Spin.SetRange(m_nSpinEditLower, m_nSpinEditUpper);

	SetTitle(m_strTitle);
	SetPercent(m_iPercent);
	
	return 0;
}

void CMySpin::SetSpinRange( int _nLower, int _nUpper )
{
	m_nSpinEditLower = _nLower; m_nSpinEditUpper = _nUpper;
	m_Spin.SetRange( _nLower, _nUpper );
	m_bSetPosFlag = true;
}
void CMySpin::OnSize(UINT nType, int cx, int cy) 
{
	CWnd::OnSize(nType, cx, cy);
	
	CRect rcClient, rcEdit, rcSpin;
	GetClientRect(&rcClient);

	int iSpinWidth = 15;

	// Edit
	rcEdit.left   = rcClient.left;
	rcEdit.top    = rcClient.top;
	rcEdit.right  = rcEdit.left + (rcClient.Width() - iSpinWidth);
	rcEdit.bottom = rcEdit.top + rcClient.Height();	
	m_Edit.MoveWindow(&rcEdit);

	// Spin
	rcSpin.left   = rcEdit.right;
	rcSpin.top    = rcEdit.top;
	rcSpin.right  = rcClient.right;
	rcSpin.bottom = rcEdit.bottom;
	m_Spin.MoveWindow(&rcSpin);
}

void CMySpin::OnKillFocus(CWnd* pNewWnd) 
{
	CWnd::OnKillFocus(pNewWnd);
	if((pNewWnd==&m_Edit)||(pNewWnd==&m_Spin))
		return;
	SetWindowText(_T(""));
	ShowWindow(SW_HIDE);

	GetOwner()->Invalidate();

	SPINTEXT st;
	_tcscpy(st.szTitle, m_strTitle);
	st.iPercent = m_iPercent;

	GetParent()->SendMessage(WM_INPLACE_SPIN, 0, (LPARAM) &st);
}

void CMySpin::SetTitle(CString strTitle)
{
	m_strTitle = strTitle;

	if (!::IsWindow(m_Edit.GetSafeHwnd()))
		return;

	CString strEdit, strPercent;
	strEdit = m_strTitle;
	strPercent.Format(_T("  ( %d"), m_iPercent);
	if ( m_iDisplayType == 1 )
	{
		m_Edit.SetWindowText(strEdit);		
	}
	else 
	{
		if( m_iDisplayType == 0 || m_iDisplayType == 4)
		{
			strEdit += strPercent + _T("% ) ");
		}
		else 
		{
			strEdit += strPercent + _T(" )");
		}
	}
	
	m_Edit.SetWindowText(strEdit);
}

void CMySpin::SetPercent(int iPercent)
{
	m_iPercent = iPercent;
	
	if (::IsWindow(m_Edit.GetSafeHwnd()))
	{
		CString strEdit, strPercent;
		strEdit = m_strTitle;
		if (m_iDisplayType == 1)
		{
			strPercent.Format(_T("%d"), m_iPercent);
			m_Edit.SetWindowText(strPercent);
		}
		else
		{
			strPercent.Format(_T("  ( %d"), m_iPercent);
			if( m_iDisplayType == 0 || m_iDisplayType == 4)
				strEdit += strPercent + _T("% ) ");
			else 
				strEdit += strPercent + _T(" )");
			m_Edit.SetWindowText(strEdit);
		}			
	}
	
	if (::IsWindow(m_Spin.GetSafeHwnd()))
	{
		if( m_bSetPosFlag)
			m_Spin.SetPos(iPercent);
	}
}

void CMySpin::OnDeltaposSpin(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	int nPos ;
	CString strPos ;
	m_Edit.GetWindowText(strPos);
	nPos = wcstol(strPos, NULL, 10);
	if (nPos >= m_nSpinEditLower && nPos <= m_nSpinEditUpper )//&&
	{
		pNMUpDown->iPos = nPos;
	}
	
	int iNewData = pNMUpDown->iPos + pNMUpDown->iDelta ;
	if (iNewData >= m_nSpinEditLower && iNewData <= m_nSpinEditUpper )//&&
		//pNMUpDown->iPos != m_iPercent)		
	{
		m_bSetPosFlag = false;
		SetPercent( iNewData );
	}	
	*pResult = 0;
}

void CMySpin::OnKillfocusMyEdit()
{
	int nPos ;
	CString strPos ;
	m_Edit.GetWindowText(strPos);
	nPos = wcstol(strPos, NULL, 10);
	if (nPos >= m_nSpinEditLower && nPos <= m_nSpinEditUpper )
	{
		m_Spin.SetPos(nPos);
		m_iPercent = nPos;
	}
	SendMessage(WM_KILLFOCUS);	
}

void CMySpin::SetDisplayType( int _iDisplayType )
{
	m_iDisplayType=_iDisplayType;
}
