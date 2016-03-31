// DocHostSite.cpp: implementation of the CDocHostSite class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#if _MFC_VER >= 0x0700
#include <afxocc.h>
#else
#ifdef _AFXDLL
#undef AFX_DATA
#define AFX_DATA AFX_DATA_IMPORT
#endif
#include <..\src\occimpl.h>
#ifdef _AFXDLL
#undef AFX_DATA
#define AFX_DATA AFX_DATA_EXPORT
#endif
#endif

#include <mshtmhst.h>

#include "DocHostSite.h"
#include "DocHostHtmlView.h"

#include "CustIE.h"
#include "CustIETools.h"


BEGIN_INTERFACE_MAP(CDocHostSite, COleControlSite)
	INTERFACE_PART(CDocHostSite, IID_IDocHostShowUI, DocHostShowUI)
	INTERFACE_PART(CDocHostSite, IID_IDocHostUIHandler, DocHostUIHandler)
#if (_WIN32_IE >= 0x0501) // IE 5.5 and higher
	INTERFACE_PART(CDocHostSite, IID_IHostDialogHelper,  CustHostDialogHelper)
	INTERFACE_PART(CDocHostSite, IID_IServiceProvider,   ServiceProvider)
	INTERFACE_PART(CDocHostSite, IID_INewWindowManager, NewWindowManager)
	INTERFACE_PART(CDocHostSite, IID_IInternetSecurityManager, InternetSecurityManager)
	INTERFACE_PART(CDocHostSite, IID_IOleCommandTarget, OleCommandTarget)
#endif
END_INTERFACE_MAP()

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDocHostSite::CDocHostSite(COleControlContainer * pOcc)
	: COleControlSite( pOcc ),
	m_pView( NULL )
{
}

CDocHostSite::~CDocHostSite()
{
}

ULONG CDocHostSite::XDocHostShowUI::AddRef()
{
	METHOD_PROLOGUE(CDocHostSite, DocHostShowUI);

	return pThis->ExternalAddRef();
}

ULONG CDocHostSite::XDocHostShowUI::Release()
{
	METHOD_PROLOGUE(CDocHostSite, DocHostShowUI);

	return pThis->ExternalRelease();
}

HRESULT CDocHostSite::XDocHostShowUI::QueryInterface(REFIID riid, void ** ppvObj)
{
	METHOD_PROLOGUE(CDocHostSite, DocHostShowUI);

	return pThis->ExternalQueryInterface( &riid, ppvObj );
}

HRESULT CDocHostSite::XDocHostShowUI::ShowHelp(HWND hwnd,
											   LPOLESTR pszHelpFile,
											   UINT nCommand,
											   DWORD dwData,
											   POINT ptMouse,
											   IDispatch * pDispatchObjectHit)
{
	METHOD_PROLOGUE(CDocHostSite, DocHostShowUI);

	return pThis->m_pView->OnShowHelp( hwnd, pszHelpFile, nCommand, dwData, ptMouse,
		pDispatchObjectHit );
}

HRESULT CDocHostSite::XDocHostShowUI::ShowMessage(HWND hwnd,
												  LPOLESTR lpstrText,
												  LPOLESTR lpstrCaption,
												  DWORD dwType,
												  LPOLESTR lpstrHelpFile,
												  DWORD dwHelpContext,
												  LRESULT * plResult)
{
	METHOD_PROLOGUE(CDocHostSite, DocHostShowUI);

	return pThis->m_pView->OnShowMessage( hwnd, lpstrText, lpstrCaption, dwType,
		lpstrHelpFile, dwHelpContext, plResult );
}

ULONG CDocHostSite::XDocHostUIHandler::AddRef()
{
	METHOD_PROLOGUE(CDocHostSite, DocHostUIHandler);
	return pThis->ExternalAddRef();
}

ULONG CDocHostSite::XDocHostUIHandler::Release()
{
	METHOD_PROLOGUE(CDocHostSite, DocHostUIHandler);
	return pThis->ExternalRelease();
}

HRESULT CDocHostSite::XDocHostUIHandler::QueryInterface(REFIID riid, void ** ppvObj)
{
	METHOD_PROLOGUE(CDocHostSite, DocHostUIHandler);
	return pThis->ExternalQueryInterface( &riid, ppvObj );
}

HRESULT CDocHostSite::XDocHostUIHandler::GetHostInfo(DOCHOSTUIINFO * pInfo)
{
	METHOD_PROLOGUE(CDocHostSite, DocHostUIHandler);
	return pThis->m_pView->OnGetHostInfo(pInfo);
}

HRESULT CDocHostSite::XDocHostUIHandler::ShowUI(DWORD dwID,
											  IOleInPlaceActiveObject * pActiveObject,
											  IOleCommandTarget * pCommandTarget,
											  IOleInPlaceFrame * pFrame,
											  IOleInPlaceUIWindow * pDoc)
{
	METHOD_PROLOGUE(CDocHostSite, DocHostUIHandler);
	return pThis->m_pView->OnShowUI( dwID, pActiveObject, pCommandTarget,
		pFrame, pDoc );
}

HRESULT CDocHostSite::XDocHostUIHandler::HideUI()
{
	METHOD_PROLOGUE(CDocHostSite, DocHostUIHandler);
	return pThis->m_pView->OnHideUI();
}

HRESULT CDocHostSite::XDocHostUIHandler::UpdateUI()
{
	METHOD_PROLOGUE(CDocHostSite, DocHostUIHandler);
	return pThis->m_pView->OnUpdateUI();
}

HRESULT CDocHostSite::XDocHostUIHandler::EnableModeless(BOOL fEnable)
{
	METHOD_PROLOGUE(CDocHostSite, DocHostUIHandler);
	return pThis->m_pView->OnEnableModeless( fEnable );
}

HRESULT CDocHostSite::XDocHostUIHandler::OnDocWindowActivate(BOOL fEnable)
{
	METHOD_PROLOGUE(CDocHostSite, DocHostUIHandler);
	return pThis->m_pView->OnDocWindowActivate( fEnable );
}

HRESULT CDocHostSite::XDocHostUIHandler::OnFrameWindowActivate(BOOL fEnable)
{
	METHOD_PROLOGUE(CDocHostSite, DocHostUIHandler);
	return pThis->m_pView->OnFrameWindowActivate( fEnable );
}

HRESULT CDocHostSite::XDocHostUIHandler::ResizeBorder(LPCRECT prcBorder,
													IOleInPlaceUIWindow * pUIWindow,
													BOOL fFrameWindow)
{
	METHOD_PROLOGUE(CDocHostSite, DocHostUIHandler);
	return pThis->m_pView->OnResizeBorder( prcBorder, pUIWindow, fFrameWindow );
}

HRESULT CDocHostSite::XDocHostUIHandler::ShowContextMenu(DWORD dwID,
													   POINT * ppt,
													   IUnknown * pcmdtReserved,
													   IDispatch * pdispReserved)
{
	METHOD_PROLOGUE(CDocHostSite, DocHostUIHandler);
	return pThis->m_pView->OnShowContextMenu( dwID, ppt, pcmdtReserved,
		pdispReserved );
}

HRESULT CDocHostSite::XDocHostUIHandler::TranslateAccelerator(LPMSG lpMsg,
															const GUID * pguidCmdGroup,
															DWORD nCmdID)
{
	METHOD_PROLOGUE(CDocHostSite, DocHostUIHandler);
	return pThis->m_pView->OnTranslateAccelerator( lpMsg,pguidCmdGroup, nCmdID );
}

HRESULT CDocHostSite::XDocHostUIHandler::GetOptionKeyPath(LPOLESTR * pchKey,
														DWORD dw)
{
	METHOD_PROLOGUE(CDocHostSite, DocHostUIHandler);
	return pThis->m_pView->OnGetOptionKeyPath( pchKey, dw );
}

HRESULT CDocHostSite::XDocHostUIHandler::GetDropTarget(IDropTarget * pDropTarget,
													 IDropTarget ** ppDropTarget)
{
	METHOD_PROLOGUE(CDocHostSite, DocHostUIHandler);
	return pThis->m_pView->OnGetDropTarget( pDropTarget, ppDropTarget );
}

HRESULT CDocHostSite::XDocHostUIHandler::GetExternal(IDispatch ** ppDispatch)
{
	METHOD_PROLOGUE(CDocHostSite, DocHostUIHandler);
	return pThis->m_pView->OnGetExternal( ppDispatch );
}

HRESULT CDocHostSite::XDocHostUIHandler::TranslateUrl(DWORD dwTranslate,
													OLECHAR * pchURLIn,
													OLECHAR ** ppchURLOut)
{
	METHOD_PROLOGUE(CDocHostSite, DocHostUIHandler);
	return pThis->m_pView->OnTranslateUrl( dwTranslate, pchURLIn, ppchURLOut );
}

HRESULT CDocHostSite::XDocHostUIHandler::FilterDataObject(IDataObject * pDO,
														IDataObject ** ppDORet)
{
	METHOD_PROLOGUE(CDocHostSite, DocHostUIHandler);
	return pThis->m_pView->OnFilterDataObject( pDO, ppDORet );
}

#if (_WIN32_IE >= 0x0501) // IE 5.5 and higher

// IServiceProvider
ULONG FAR EXPORT CDocHostSite::XServiceProvider::AddRef()
{
	METHOD_PROLOGUE(CDocHostSite, ServiceProvider)
	return pThis->ExternalAddRef();
}

ULONG FAR EXPORT CDocHostSite::XServiceProvider::Release()
{
	METHOD_PROLOGUE(CDocHostSite, ServiceProvider)
	return pThis->ExternalRelease();
}

HRESULT FAR EXPORT CDocHostSite::XServiceProvider::QueryInterface(REFIID riid,	void** ppvObj)
{
	METHOD_PROLOGUE(CDocHostSite, ServiceProvider)
	HRESULT hr = (HRESULT)pThis->ExternalQueryInterface(&riid, ppvObj);
	return hr;
}

STDMETHODIMP CDocHostSite::XServiceProvider::QueryService(REFGUID,	REFIID riid, void** ppvObject)
{
	METHOD_PROLOGUE(CDocHostSite, ServiceProvider);
	if (riid == IID_INewWindowManager)
	{
		return (HRESULT)pThis->ExternalQueryInterface(&riid, ppvObject);
	}
	if (riid == IID_IInternetSecurityManager)
	{
		return (HRESULT)pThis->ExternalQueryInterface(&riid, ppvObject);
	}
	else
	{
		*ppvObject = NULL;
		return E_NOINTERFACE;
	}
}

// INewWindowManager
ULONG CDocHostSite::XNewWindowManager::AddRef()
{
	METHOD_PROLOGUE(CDocHostSite, NewWindowManager);
	return pThis->ExternalAddRef();
}

ULONG CDocHostSite::XNewWindowManager::Release()
{
	METHOD_PROLOGUE(CDocHostSite, NewWindowManager);
	return pThis->ExternalRelease();
}

HRESULT CDocHostSite::XNewWindowManager::QueryInterface(REFIID riid, void ** ppvObj)
{
	METHOD_PROLOGUE(CDocHostSite, NewWindowManager);
	return pThis->ExternalQueryInterface(&riid, ppvObj);
}

HRESULT CDocHostSite::XNewWindowManager::EvaluateNewWindow(
	LPCWSTR pszUrl,
	LPCWSTR pszName,
	LPCWSTR pszUrlContext,
	LPCWSTR pszFeatures,
	BOOL fReplace,
	DWORD dwFlags,
	DWORD dwUserActionTime
	)
{
	METHOD_PROLOGUE(CDocHostSite, NewWindowManager);
	return pThis->m_pView->EvaluateNewWindow( pszUrl, pszName, pszUrlContext, pszFeatures, fReplace, dwFlags, dwUserActionTime);
}

// IHostDialogHelper
ULONG CDocHostSite::XCustHostDialogHelper::AddRef()
{
	METHOD_PROLOGUE(CDocHostSite, CustHostDialogHelper)
	return pThis->ExternalAddRef();
}

ULONG CDocHostSite::XCustHostDialogHelper::Release()
{
	METHOD_PROLOGUE(CDocHostSite, CustHostDialogHelper)
	return pThis->ExternalRelease();
}

HRESULT CDocHostSite::XCustHostDialogHelper::QueryInterface(REFIID riid, void **ppvObj)
{
	METHOD_PROLOGUE(CDocHostSite, CustHostDialogHelper)
	return (HRESULT)pThis->ExternalQueryInterface(&riid, ppvObj);
}

STDMETHODIMP CDocHostSite::XCustHostDialogHelper::ShowHTMLDialog(HWND,	IMoniker *,	VARIANT *,	WCHAR *,	VARIANT *,	IUnknown*)
{
	METHOD_PROLOGUE(CDocHostSite, CustHostDialogHelper)
	return S_OK;
}


ULONG CDocHostSite::XInternetSecurityManager::AddRef()
{
	METHOD_PROLOGUE(CDocHostSite, InternetSecurityManager)
		return pThis->ExternalAddRef();
}

ULONG CDocHostSite::XInternetSecurityManager::Release()
{
	METHOD_PROLOGUE(CDocHostSite, InternetSecurityManager)
		return pThis->ExternalRelease();
}

HRESULT CDocHostSite::XInternetSecurityManager::QueryInterface(REFIID riid, void **ppvObj)
{
	METHOD_PROLOGUE(CDocHostSite, InternetSecurityManager)
		return (HRESULT)pThis->ExternalQueryInterface(&riid, ppvObj);
}

STDMETHODIMP CDocHostSite::XInternetSecurityManager::GetSecurityId(LPCWSTR pwszUrl, BYTE *pbSecurityId, DWORD *pcbSecurityId, DWORD_PTR dwReserved)
{
	return INET_E_DEFAULT_ACTION;
}

STDMETHODIMP CDocHostSite::XInternetSecurityManager::GetSecuritySite(IInternetSecurityMgrSite **ppSite)
{
	return INET_E_DEFAULT_ACTION;
}

STDMETHODIMP CDocHostSite::XInternetSecurityManager::GetZoneMappings(DWORD dwZone, IEnumString **ppenumString, DWORD dwFlags)
{
	return INET_E_DEFAULT_ACTION;
}

STDMETHODIMP CDocHostSite::XInternetSecurityManager::MapUrlToZone(LPCWSTR pwszUrl, DWORD *pdwZone, DWORD dwFlags)
{
	return INET_E_DEFAULT_ACTION;
}

STDMETHODIMP CDocHostSite::XInternetSecurityManager::ProcessUrlAction(LPCWSTR pwszUrl, DWORD dwAction, BYTE *pPolicy, DWORD cbPolicy, BYTE *pContext, DWORD cbContext, DWORD dwFlags, DWORD dwReserved)
{
	if (cbPolicy >= sizeof(DWORD)) {
		*pPolicy = URLPOLICY_ALLOW;
		return S_OK;
	}
	return INET_E_DEFAULT_ACTION;
}

STDMETHODIMP CDocHostSite::XInternetSecurityManager::QueryCustomPolicy(LPCWSTR pwszUrl, REFGUID guidKey, BYTE **ppPolicy, DWORD *pcbPolicy, BYTE *pContext, DWORD cbContext, DWORD dwReserved)
{
	return INET_E_DEFAULT_ACTION;
}

STDMETHODIMP CDocHostSite::XInternetSecurityManager::SetSecuritySite(IInternetSecurityMgrSite *pSite)
{
	return INET_E_DEFAULT_ACTION;
}

STDMETHODIMP CDocHostSite::XInternetSecurityManager::SetZoneMapping(DWORD dwZone, LPCWSTR lpszPattern, DWORD dwFlags)
{
	return INET_E_DEFAULT_ACTION;
}

// IOleCommandTarget
STDMETHODIMP_(ULONG) CDocHostSite::XOleCommandTarget::AddRef()
{
	METHOD_PROLOGUE_EX_(CDocHostSite, OleCommandTarget)
	return pThis->ExternalAddRef();
}

STDMETHODIMP_(ULONG) CDocHostSite::XOleCommandTarget::Release()
{
	METHOD_PROLOGUE_EX_(CDocHostSite, OleCommandTarget)
	return pThis->ExternalRelease();
}

STDMETHODIMP CDocHostSite::XOleCommandTarget::QueryInterface(
	REFIID iid, LPVOID far* ppvObj)
{
	METHOD_PROLOGUE_EX_(CDocHostSite, OleCommandTarget)
	return pThis->ExternalQueryInterface(&iid, ppvObj);
}

HRESULT CDocHostSite::XOleCommandTarget::Exec(const GUID* pguidCmdGroup
	, DWORD nCmdID
	, DWORD nCmdexecopt
	, VARIANTARG* pvaIn
	, VARIANTARG* pvaOut)
{
	if (pguidCmdGroup /*&& IsEqualGUID(*pguidCmdGroup, CGID_DocHostCommandHandler)*/)   
	{
		if (nCmdID == OLECMDID_SHOWSCRIPTERROR) 
		{
			if (pvaOut) {
				if (theApp.ShowScriptError()){
					return   OLECMDERR_E_NOTSUPPORTED;
				}

				PUTLOG("* Suppress script error dialog");
				// Continue running scripts on the page.
				(*pvaOut).vt = VT_BOOL;
				(*pvaOut).boolVal = VARIANT_TRUE;
				return S_OK;
			}
		}
	}
	return   OLECMDERR_E_NOTSUPPORTED;
}

STDMETHODIMP CDocHostSite::XOleCommandTarget::QueryStatus(
	/* [unique][in] */ const GUID __RPC_FAR *pguidCmdGroup,
	/* [in] */ ULONG cCmds,
	/* [out][in][size_is] */ OLECMD __RPC_FAR prgCmds[],
	/* [unique][out][in] */ OLECMDTEXT __RPC_FAR *pCmdText
	)
{
	METHOD_PROLOGUE_(CDocHostSite, OleCommandTarget)
	return OLECMDERR_E_NOTSUPPORTED;
}

#endif

CDocHostOccManager::CDocHostOccManager()
	: m_pView( NULL ),
	m_pSite( NULL )
{
}

COleControlSite * CDocHostOccManager::CreateSite(COleControlContainer * pOcc)
{
	m_pSite = new CDocHostSite( pOcc );
	m_pSite->SetView( m_pView );
	return m_pSite;
}