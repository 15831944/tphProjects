// DocHostSite.h: interface for the CDocHostSite class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_DOCHOSTSITE_H_)
#define _DOCHOSTSITE_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CDocHostSite : public COleControlSite  
{
public:
	CDocHostSite(COleControlContainer * pOCC);
	virtual ~CDocHostSite();

private:
	class CDocHostHtmlView * m_pView;
	friend class CDocHostHtmlView;

public:
	void SetView(class CDocHostHtmlView * pView)
	{
		m_pView = pView;
	}

// IDocHostShowUI
protected:
	BEGIN_INTERFACE_PART(DocHostShowUI, IDocHostShowUI)
		INIT_INTERFACE_PART(CDocHostSite, DocHostShowUI)
		STDMETHOD(ShowHelp)(
			/* [in ] */	HWND hwnd,
			/* [in ] */	LPOLESTR pszHelpFile,
			/* [in ] */	UINT uCommand,
			/* [in ] */	DWORD dwData,
			/* [in ] */	POINT ptMouse,
			/* [out] */	IDispatch __RPC_FAR *pDispatchObjectHit);
		STDMETHOD(ShowMessage)(
			/* [in ] */	HWND hwnd,
			/* [in ] */	LPOLESTR lpstrText,
			/* [in ] */	LPOLESTR lpstrCaption,
			/* [in ] */	DWORD dwType,
			/* [in ] */	LPOLESTR lpstrHelpFile,
			/* [in ] */	DWORD dwHelpContext,
			/* [out] */	LRESULT __RPC_FAR *plResult);
	END_INTERFACE_PART(DocHostShowUI)

// IDocHostUIHandler
protected:
	BEGIN_INTERFACE_PART(DocHostUIHandler, IDocHostUIHandler)
		STDMETHOD(ShowContextMenu)(/* [in] */ DWORD dwID,
			/* [in] */ POINT __RPC_FAR *ppt,
			/* [in] */ IUnknown __RPC_FAR *pcmdtReserved,
			/* [in] */ IDispatch __RPC_FAR *pdispReserved);
		STDMETHOD(GetHostInfo)( 
			/* [out][in] */ DOCHOSTUIINFO __RPC_FAR *pInfo);
		STDMETHOD(ShowUI)( 
			/* [in] */ DWORD dwID,
			/* [in] */ IOleInPlaceActiveObject __RPC_FAR *pActiveObject,
			/* [in] */ IOleCommandTarget __RPC_FAR *pCommandTarget,
			/* [in] */ IOleInPlaceFrame __RPC_FAR *pFrame,
			/* [in] */ IOleInPlaceUIWindow __RPC_FAR *pDoc);
		STDMETHOD(HideUI)(void);
		STDMETHOD(UpdateUI)(void);
		STDMETHOD(EnableModeless)(/* [in] */ BOOL fEnable);
		STDMETHOD(OnDocWindowActivate)(/* [in] */ BOOL fEnable);
		STDMETHOD(OnFrameWindowActivate)(/* [in] */ BOOL fEnable);
		STDMETHOD(ResizeBorder)( 
			/* [in] */ LPCRECT prcBorder,
			/* [in] */ IOleInPlaceUIWindow __RPC_FAR *pUIWindow,
			/* [in] */ BOOL fRameWindow);
		STDMETHOD(TranslateAccelerator)( 
			/* [in] */ LPMSG lpMsg,
			/* [in] */ const GUID __RPC_FAR *pguidCmdGroup,
			/* [in] */ DWORD nCmdID);
		STDMETHOD(GetOptionKeyPath)( 
			/* [out] */ LPOLESTR __RPC_FAR *pchKey,
			/* [in] */ DWORD dw);
		STDMETHOD(GetDropTarget)(
			/* [in] */ IDropTarget __RPC_FAR *pDropTarget,
			/* [out] */ IDropTarget __RPC_FAR *__RPC_FAR *ppDropTarget);
		STDMETHOD(GetExternal)( 
			/* [out] */ IDispatch __RPC_FAR *__RPC_FAR *ppDispatch);
		STDMETHOD(TranslateUrl)( 
			/* [in] */ DWORD dwTranslate,
			/* [in] */ OLECHAR __RPC_FAR *pchURLIn,
			/* [out] */ OLECHAR __RPC_FAR *__RPC_FAR *ppchURLOut);
		STDMETHOD(FilterDataObject)( 
			/* [in] */ IDataObject __RPC_FAR *pDO,
			/* [out] */ IDataObject __RPC_FAR *__RPC_FAR *ppDORet);
	END_INTERFACE_PART(DocHostUIHandler)

#if (_WIN32_IE >= 0x0501) // IE 5.5 and higher
protected:
// INewWindowManager
	BEGIN_INTERFACE_PART(NewWindowManager, INewWindowManager)
		STDMETHOD(EvaluateNewWindow)(
			LPCWSTR pszUrl,
			LPCWSTR pszName,
			LPCWSTR pszUrlContext,
			LPCWSTR pszFeatures,
			BOOL fReplace,
			DWORD dwFlags,
			DWORD dwUserActionTime);
	END_INTERFACE_PART(NewWindowManager);

// IServiceProvider
	BEGIN_INTERFACE_PART(ServiceProvider, IServiceProvider)
		STDMETHOD(QueryService)(REFGUID, REFIID, void**);
	END_INTERFACE_PART(ServiceProvider)

// IHostDialogHelper
	BEGIN_INTERFACE_PART(CustHostDialogHelper, IHostDialogHelper)
		STDMETHODIMP ShowHTMLDialog(
			HWND hwndParent,
			IMoniker *pMk,
			VARIANT *pvarArgIn,
			WCHAR *pchOptions,
			VARIANT *pvarArgOut,
			IUnknown *punkHost);
	END_INTERFACE_PART(CustHostDialogHelper)

	BEGIN_INTERFACE_PART(InternetSecurityManager, IInternetSecurityManager)
		STDMETHOD(GetSecurityId)(LPCWSTR pwszUrl, BYTE *pbSecurityId, DWORD *pcbSecurityId, DWORD_PTR dwReserved);
		STDMETHOD(GetSecuritySite)(IInternetSecurityMgrSite **ppSite);
		STDMETHOD(GetZoneMappings)(DWORD dwZone, IEnumString **ppenumString, DWORD dwFlags);
		STDMETHOD(MapUrlToZone)(LPCWSTR pwszUrl, DWORD *pdwZone, DWORD dwFlags);
		STDMETHOD(ProcessUrlAction)(LPCWSTR pwszUrl, DWORD dwAction, BYTE *pPolicy, DWORD cbPolicy, BYTE *pContext, DWORD cbContext, DWORD dwFlags, DWORD dwReserved);
		STDMETHOD(QueryCustomPolicy)(LPCWSTR pwszUrl, REFGUID guidKey, BYTE **ppPolicy, DWORD *pcbPolicy, BYTE *pContext, DWORD cbContext, DWORD dwReserved);
		STDMETHOD(SetSecuritySite)(IInternetSecurityMgrSite *pSite);
		STDMETHOD(SetZoneMapping)(DWORD dwZone, LPCWSTR lpszPattern, DWORD dwFlags);
	END_INTERFACE_PART(InternetSecurityManager)

	/*
	* Ref: http://www.koders.com/cpp/fid0B685E49FE00FEF257A3C8C6D150F09D2922BE6A.aspx
	*/
	BEGIN_INTERFACE_PART(OleCommandTarget, IOleCommandTarget)
		STDMETHOD(Exec)(
			/* [unique][in] */ const GUID __RPC_FAR *pguidCmdGroup,
			/* [in] */ DWORD nCmdID,
			/* [in] */ DWORD nCmdexecopt,
			/* [unique][in] */ VARIANT __RPC_FAR *pvaIn,
			/* [unique][out][in] */ VARIANT __RPC_FAR *pvaOut);

		STDMETHOD(QueryStatus)(
			/* [unique][in] */ const GUID __RPC_FAR *pguidCmdGroup,
			/* [in] */ ULONG cCmds,
			/* [out][in][size_is] */ OLECMD __RPC_FAR prgCmds[],
			/* [unique][out][in] */ OLECMDTEXT __RPC_FAR *pCmdText);
	END_INTERFACE_PART(OleCommandTarget)
#endif

	DECLARE_INTERFACE_MAP()
};

class CDocHostOccManager : public COccManager
{
public:
	CDocHostOccManager();

	virtual COleControlSite * CreateSite(COleControlContainer * pOCC);

private:
	class CDocHostHtmlView * m_pView;
	CDocHostSite * m_pSite;

public:
	void SetView(class CDocHostHtmlView * pView)
	{
		m_pView = pView;
	}
};

#endif // !defined(_DOCHOSTSITE_H_)
