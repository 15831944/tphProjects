// DocHostHtmlView.cpp : implementation file
//

#include "stdafx.h"
#include "DocHostHtmlView.h"

#if _MFC_VER >= 0x0700
#include <afxocc.h>
#else
#include <..\src\occimpl.h>
#endif

#include "CustIE.h"
#include "DocHostSite.h"

#include <mshtml.h>

/////////////////////////////////////////////////////////////////////////////
// CDocHostHtmlView

IMPLEMENT_DYNCREATE(CDocHostHtmlView, CHtmlView)

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CDocHostHtmlView, CHtmlView)
	//{{AFX_MSG_MAP(CDocHostHtmlView)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


CDocHostHtmlView::CDocHostHtmlView()
	: m_pOccManager( NULL )
{
}

CDocHostHtmlView::~CDocHostHtmlView()
{
}

BOOL CDocHostHtmlView::Create(LPCTSTR lpszClassName,
							  LPCTSTR lpszWindowName,
							  DWORD dwStyle,
							  const RECT & rect,
							  CWnd * pParentWnd,
							  UINT nID,
							  CCreateContext * pContext)
{
	if (!m_pOccManager)
	{
		m_pOccManager = new CDocHostOccManager;
		ASSERT(m_pOccManager);
		if (!m_pOccManager)
			return FALSE;

		m_pOccManager->SetView( this );
	}

	::AfxEnableControlContainer( m_pOccManager );

// The rest of the code is copied form ViewHtml.cpp MFC source file

	// create the view window itself
	m_pCreateContext = pContext;
	if (!CView::Create(lpszClassName, lpszWindowName,
				dwStyle, rect, pParentWnd,  nID, pContext))
	{
		return FALSE;
	}

// We do not want this line, so comment it out!
//	AfxEnableControlContainer();

	RECT rectClient;
	GetClientRect(&rectClient);

	// create the control window
	// AFX_IDW_PANE_FIRST is a safe but arbitrary ID
	if (!m_wndBrowser.CreateControl(CLSID_WebBrowser, lpszWindowName,
				WS_VISIBLE | WS_CHILD, rectClient, this, AFX_IDW_PANE_FIRST))
	{
		DestroyWindow();
		return FALSE;
	}

	LPUNKNOWN lpUnk = m_wndBrowser.GetControlUnknown();
	HRESULT hr = lpUnk->QueryInterface(IID_IWebBrowser2, (void**) &m_pBrowserApp);
	if (!SUCCEEDED(hr))
	{
		m_pBrowserApp = NULL;
		m_wndBrowser.DestroyWindow();
		DestroyWindow();
		return FALSE;
	}

	return TRUE;
}

#if _MFC_VER >= 0x0700

BOOL CDocHostHtmlView::CreateControlSite(COleControlContainer * pContainer,
										 COleControlSite ** ppSite,
										 UINT /*nID*/,
										 REFCLSID /*clsid*/)
{
	ASSERT(m_pOccManager);
	if (!m_pOccManager)
		return FALSE;

	*ppSite = m_pOccManager->CreateSite( pContainer );

	return (*ppSite) ? TRUE : FALSE;
}

#endif

BOOL CDocHostHtmlView::OnAmbientProperty(COleControlSite* pSite, DISPID dispid,
	VARIANT* pvar)
{
	return CHtmlView::OnAmbientProperty(pSite, dispid, pvar);
}

HRESULT CDocHostHtmlView::OnShowHelp(HWND, LPOLESTR, UINT,
									 DWORD, POINT, IDispatch *)
{
	return E_NOTIMPL;
}

HRESULT CDocHostHtmlView::OnShowMessage(HWND, LPOLESTR, LPOLESTR,
										DWORD, LPOLESTR, DWORD, LRESULT *)
{
	return S_OK;
}

HRESULT CDocHostHtmlView::EvaluateNewWindow(LPCWSTR pszUrl,	LPCWSTR,	LPCWSTR,	LPCWSTR,	BOOL,	DWORD,	DWORD)
{	
	return S_FALSE; // block the new window
}

HRESULT CDocHostHtmlView::TranslateAccelerator(LPMSG lpMsg,
	const GUID * pguidCmdGroup,
	DWORD nCmdID)
{
	return S_OK; // disable default behavior
}

void CDocHostHtmlView::OnSize(UINT nType, int cx, int cy) 
{
	CHtmlView::OnSize(nType, cx, cy);
	
	// This makes sure that the browser control isn't larger than the view
	if (::IsWindow(m_wndBrowser.GetSafeHwnd())) m_wndBrowser.SetWindowPos(NULL, 0, 0, cx, cy, SWP_NOZORDER);
}
