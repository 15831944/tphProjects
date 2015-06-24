// CtrlToolBar.cpp : implementation file
//

#include "stdafx.h"
#include "termplan.h"
#include "CtrlToolBar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCtrlToolBar

CCtrlToolBar::CCtrlToolBar()
{
	m_pParentWnd = NULL;
}

CCtrlToolBar::~CCtrlToolBar()
{
}


BEGIN_MESSAGE_MAP(CCtrlToolBar, CToolBar)
	//{{AFX_MSG_MAP(CCtrlToolBar)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CCtrlToolBar message handlers


BOOL CCtrlToolBar::Create( CWnd* pParentWnd, DWORD dwStyle /*= WS_CHILD | WS_VISIBLE | CBRS_TOP*/, UINT nID /*= AFX_IDW_TOOLBAR*/ )
{
	m_pParentWnd = pParentWnd;
	return CToolBar::Create( pParentWnd, dwStyle, nID );
}


BOOL CCtrlToolBar::CreateEx(CWnd* pParentWnd, DWORD dwCtrlStyle, DWORD dwStyle, CRect rcBorders, UINT nID)
{
	m_pParentWnd = pParentWnd;
	return CToolBar::CreateEx( pParentWnd, dwCtrlStyle,dwStyle, rcBorders, nID );
}

LRESULT CCtrlToolBar::DefWindowProc( UINT message, WPARAM wParam, LPARAM lParam )
{
	if( m_pParentWnd )
	{
		switch( message )
		{
		case CBN_SELENDOK:
			m_pParentWnd->SendMessage( message, wParam, lParam );
		}
	}

	return CToolBar::DefWindowProc( message, wParam, lParam );
}