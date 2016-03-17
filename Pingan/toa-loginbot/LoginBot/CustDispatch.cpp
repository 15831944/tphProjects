/*
 * idispimp.CPP
 * IDispatch for Extending Dynamic HTML Object Model
 *
 * Copyright (c)1995-1999 Microsoft Corporation, All Rights Reserved
 */ 

#include "stdafx.h"
#include "CustDispatch.h"

#include "CustIE.h"

#include "CustIETools.h"

CString cszCB_GetVCodeText = _T("GetVCodeText");
CString cszCB_GetLock = _T("GetLock");
CString cszCB_ReleaseLock = _T("ReleaseLock");
CString cszCB_ActiveWindow = _T("ActiveWindow");
CString cszCB_KeyInput = _T("KeyInput");
CString cszCB_IsKeyInputDone = _T("IsKeyInputDone");
CString cszCB_ReportStatus = _T("ReportStatus");
CString cszCB_QueryVCode = _T("QueryVCode");
CString cszCB_QueryPhoneCode = _T("QueryPhoneCode");
CString cszCB_Exit = _T("Exit");
CString cszCB_SetElementFocus = _T("SetElementFocus");
CString cszCB_GetHttpOnlyCookie = _T("GetHttpOnlyCookie");
CString cszCB_PutLog = _T("PutLog");


#define DISPID_CB_GetVCodeText 1
#define DISPID_CB_GetLock 2
#define DISPID_CB_ActiveWindow 3
#define DISPID_CB_KeyInput 4
#define DISPID_CB_IsKeyInputDone 6
#define DISPID_CB_ReportStatus 7
#define DISPID_CB_QueryVCode 9
#define DISPID_CB_QueryPhoneCode 10
#define DISPID_CB_Exit 11
#define DISPID_CB_SetElementFocus 12
#define DISPID_CB_GetHttpOnlyCookie 13
#define DISPID_CB_PutLog 14
#define DISPID_CB_ReleaseLock 15

/*
 * CImpIDispatch::CImpIDispatch
 * CImpIDispatch::~CImpIDispatch
 *
 * Parameters (Constructor):
 *  pSite           PCSite of the site we're in.
 *  pUnkOuter       LPUNKNOWN to which we delegate.
 */ 

CImpIDispatch::CImpIDispatch( void )
{
    m_cRef = 0;
}

CImpIDispatch::~CImpIDispatch( void )
{
	ASSERT( m_cRef == 0 );
}

/*
 * CImpIDispatch::QueryInterface
 * CImpIDispatch::AddRef
 * CImpIDispatch::Release
 *
 * Purpose:
 *  IUnknown members for CImpIDispatch object.
 */ 

STDMETHODIMP CImpIDispatch::QueryInterface( REFIID riid, void **ppv )
{
    *ppv = NULL;

    if ( IID_IDispatch == riid )
	{
        *ppv = this;
	}
	
	if ( NULL != *ppv )
    {
        ((LPUNKNOWN)*ppv)->AddRef();
        return NOERROR;
    }

	return E_NOINTERFACE;
}


STDMETHODIMP_(ULONG) CImpIDispatch::AddRef(void)
{
    return ++m_cRef;
}

STDMETHODIMP_(ULONG) CImpIDispatch::Release(void)
{
    return --m_cRef;
}


//IDispatch
STDMETHODIMP CImpIDispatch::GetTypeInfoCount(UINT* /*pctinfo*/)
{
	return E_NOTIMPL;
}

STDMETHODIMP CImpIDispatch::GetTypeInfo(
			/* [in] */ UINT /*iTInfo*/,
            /* [in] */ LCID /*lcid*/,
            /* [out] */ ITypeInfo** /*ppTInfo*/)
{
	return E_NOTIMPL;
}

STDMETHODIMP CImpIDispatch::GetIDsOfNames(
			/* [in] */ REFIID riid,
            /* [size_is][in] */ OLECHAR** rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID* rgDispId)
{
	HRESULT hr = NOERROR;
	UINT	i = 0;
	for ( i = 0; i < cNames; i++) {
		CString cszName  = rgszNames[i];

		if (cszName == cszCB_GetLock)
		{
			rgDispId[i] = DISPID_CB_GetLock;
		}
		/*else if (cszName == cszCB_ReleaseLock)
		{
			rgDispId[i] = DISPID_CB_ReleaseLock;
		}*/
		else if (cszName == cszCB_ActiveWindow)
		{
			rgDispId[i] = DISPID_CB_ActiveWindow;
		}
		else if (cszName == cszCB_KeyInput)
		{
			rgDispId[i] = DISPID_CB_KeyInput;
		}
		else if (cszName == cszCB_IsKeyInputDone)
		{
			rgDispId[i] = DISPID_CB_IsKeyInputDone;
		}
		else if (cszName == cszCB_GetVCodeText)
		{
			rgDispId[i] = DISPID_CB_GetVCodeText;
		}
		else if (cszName == cszCB_ReportStatus)
		{
			rgDispId[i] = DISPID_CB_ReportStatus;
		}
		else if (cszName == cszCB_QueryVCode)
		{
			rgDispId[i] = DISPID_CB_QueryVCode;
		}
		else if (cszName == cszCB_QueryPhoneCode)
		{
			rgDispId[i] = DISPID_CB_QueryPhoneCode;
		}
		else if (cszName == cszCB_Exit)
		{
			rgDispId[i] = DISPID_CB_Exit;
		}
		else if (cszName == cszCB_SetElementFocus)
		{
			rgDispId[i] = DISPID_CB_SetElementFocus;
		}
		else if (cszName == cszCB_GetHttpOnlyCookie)
		{
			rgDispId[i] = DISPID_CB_GetHttpOnlyCookie;
		}
		else if (cszName == cszCB_PutLog)
		{
			rgDispId[i] = DISPID_CB_PutLog;
		}
		else {
			// One or more are unknown so set the return code accordingly
			hr = ResultFromScode(DISP_E_UNKNOWNNAME);
			rgDispId[i] = DISPID_UNKNOWN;
		}
	}
	return hr;
}

STDMETHODIMP CImpIDispatch::Invoke(
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID /*riid*/,
            /* [in] */ LCID /*lcid*/,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS* pDispParams,
            /* [out] */ VARIANT* pVarResult,
            /* [out] */ EXCEPINFO* /*pExcepInfo*/,
            /* [out] */ UINT* puArgErr)
{
	HRESULT hr = S_OK;

	if (dispIdMember == DISPID_CB_GetLock)
	{
		if (wFlags & DISPATCH_PROPERTYGET)
		{
			if (pVarResult != NULL)
			{
				VariantInit(pVarResult);
				V_VT(pVarResult) = VT_BOOL;
				V_BOOL(pVarResult) = TRUE;
			}
		}
		if (wFlags & DISPATCH_METHOD)
		{
			bool fStat = CCustIETools::GetKeyInputLock();

			if (pVarResult != NULL) {
				VariantInit(pVarResult);
				V_VT(pVarResult) = VT_BOOL;
				V_BOOL(pVarResult) = fStat ? TRUE : FALSE;
			}
		}
	}

	/*
	if (dispIdMember == DISPID_CB_ReleaseLock)
	{
		if (wFlags & DISPATCH_PROPERTYGET)
		{
			if (pVarResult != NULL)
			{
				VariantInit(pVarResult);
				V_VT(pVarResult) = VT_BOOL;
				V_BOOL(pVarResult) = TRUE;
			}
		}
		if (wFlags & DISPATCH_METHOD)
		{
			bool fStat = CCustIETools::ReleaseKeyInputLock();

			if (pVarResult != NULL) {
				VariantInit(pVarResult);
				V_VT(pVarResult) = VT_BOOL;
				V_BOOL(pVarResult) = fStat ? TRUE : FALSE;
			}
		}
	}
	*/
	
	if (dispIdMember == DISPID_CB_ActiveWindow)
	{
		if (wFlags & DISPATCH_PROPERTYGET)
		{
			if (pVarResult != NULL)
			{
				VariantInit(pVarResult);
				V_VT(pVarResult) = VT_BOOL;
				V_BOOL(pVarResult) = TRUE;
			}
		}
		if (wFlags & DISPATCH_METHOD)
		{
			bool fStat = CCustIETools::ActiveWindow(/*fFocus*/);

			if (pVarResult != NULL) {
				VariantInit(pVarResult);
				V_VT(pVarResult) = VT_BOOL;
				V_BOOL(pVarResult) = fStat ? TRUE : FALSE;
			}
		}
	}

	if (dispIdMember == DISPID_CB_KeyInput)
	{
		if (wFlags & DISPATCH_PROPERTYGET)
		{
			if (pVarResult != NULL)
			{
				VariantInit(pVarResult);
				V_VT(pVarResult) = VT_BOOL;
				V_BOOL(pVarResult) = TRUE;
			}
		}
		if (wFlags & DISPATCH_METHOD)
		{
			//arguments come in reverse order
			//for some reason
			int nInputInterval = pDispParams->rgvarg[1].intVal;
			CString sValue = pDispParams->rgvarg[0].bstrVal; // in case you want a CString copy

			bool fStat = true;
			if (nInputInterval == 0){
				std::string strValue = TOANSI(sValue.GetBuffer());

				std::string sType = "";
				if (theApp.m_userConfig.isObject() && theApp.m_userConfig.isMember("type")){
					sType = theApp.m_userConfig["type"].asString();
				}
				if (sType == "abc"){
					for (std::string::iterator it = strValue.begin(); it != strValue.end(); it++){
						CCustIETools::HookInput_Abc(*it);
					}
				}else{
					PUTLOG("* Hook not supported");
				}
				PUTLOG("* Notify KeyInput done");
				CCustIETools::SetKeyInputDone();

				PUTLOG("* Release KeyInput Lock, TID: %u", GetCurrentThreadId());
				CCustIETools::ReleaseKeyInputLock();
			}else{
				fStat = CCustIETools::KeyInput(nInputInterval, TOANSI(sValue.GetBuffer()), true);
			}

			if (pVarResult != NULL) {
				VariantInit(pVarResult);
				V_VT(pVarResult) = VT_BOOL;
				V_BOOL(pVarResult) = fStat ? TRUE : FALSE;
			}
		}
	}

	if (dispIdMember == DISPID_CB_IsKeyInputDone)
	{
		if (wFlags & DISPATCH_PROPERTYGET)
		{
			if (pVarResult != NULL)
			{
				VariantInit(pVarResult);
				V_VT(pVarResult) = VT_BOOL;
				V_BOOL(pVarResult) = TRUE;
			}
		}
		if (wFlags & DISPATCH_METHOD)
		{
			bool fStat = CCustIETools::IsKeyInputDone();
			if (pVarResult) {
				VariantInit(pVarResult);
				V_VT(pVarResult) = VT_BOOL;
				V_BOOL(pVarResult) = fStat ? TRUE : FALSE;
			}
		}
	}

	if (dispIdMember == DISPID_CB_GetVCodeText)
	{
		if (wFlags & DISPATCH_PROPERTYGET)
		{
			if (pVarResult != NULL)
			{
				VariantInit(pVarResult);
				V_VT(pVarResult) = VT_BOOL;
				V_BOOL(pVarResult) = TRUE;
			}
		}
		if (wFlags & DISPATCH_METHOD)
		{
			CString sBankId = pDispParams->rgvarg[4].bstrVal; // in case you want a CString copy

			CComQIPtr<IHTMLElement2> spBody = pDispParams->rgvarg[3].pdispVal; // document.getElementById("");

			CComQIPtr<IHTMLElement> spElement = pDispParams->rgvarg[2].pdispVal; // document.getElementById("");

			int width = pDispParams->rgvarg[1].intVal;
			int height = pDispParams->rgvarg[0].intVal;
			PUTLOG("Image width[%d], height[%d]", width, height);

			std::string text = CCustIETools::GetVCodeTextByClip(TOANSI(sBankId.GetBuffer()), spBody, spElement, width, height);
			if (text.empty()){
				text = CCustIETools::GetVCodeText(TOANSI(sBankId.GetBuffer()), spElement, width, height);
			}

			CString strText = TOUNICODE(text.c_str());

			if (pVarResult) {
				VariantInit(pVarResult);
				V_VT(pVarResult) = VT_BSTR;
				V_BSTR(pVarResult) = strText.AllocSysString();
			}
		}
	}
	
	if (dispIdMember == DISPID_CB_ReportStatus)
	{
		if (wFlags & DISPATCH_PROPERTYGET)
		{
			if (pVarResult != NULL)
			{
				VariantInit(pVarResult);
				V_VT(pVarResult) = VT_BOOL;
				V_BOOL(pVarResult) = TRUE;
			}
		}
		if (wFlags & DISPATCH_METHOD)
		{
			// params
			unsigned int statusCode = pDispParams->rgvarg[1].uintVal; // in case you want a CString copy
			CString strJsonParams = pDispParams->rgvarg[0].bstrVal; // in case you want a CString copy

			bool fStat = CCustIETools::StatusReport(theApp.m_userConfig, statusCode, TOANSI(strJsonParams.GetBuffer()));

			// return 
			if (pVarResult) {
				VariantInit(pVarResult);
				V_VT(pVarResult) = VT_BOOL;
				V_BOOL(pVarResult) = fStat ? TRUE : FALSE;
			}
		}
	}
	if (dispIdMember == DISPID_CB_QueryVCode)
	{
		if (wFlags & DISPATCH_PROPERTYGET)
		{
			if (pVarResult != NULL)
			{
				VariantInit(pVarResult);
				V_VT(pVarResult) = VT_BOOL;
				V_BOOL(pVarResult) = TRUE;
			}
		}
		if (wFlags & DISPATCH_METHOD)
		{
			// params
			CString sReqId = pDispParams->rgvarg[0].bstrVal; // in case you want a CString copy

			PUTLOG("OCR By [%s]", TOANSI(sReqId.GetBuffer()));

			std::string response = CCustIETools::QueryVCode(TOANSI(sReqId.GetBuffer()));

			CString strVCode = TOUNICODE(response.c_str());

			// return 
			if (pVarResult) {
				VariantInit(pVarResult);
				V_VT(pVarResult) = VT_BSTR;
				V_BSTR(pVarResult) = strVCode.AllocSysString();
			}
		}
	}
	if (dispIdMember == DISPID_CB_QueryPhoneCode)
	{
		if (wFlags & DISPATCH_PROPERTYGET)
		{
			if (pVarResult != NULL)
			{
				VariantInit(pVarResult);
				V_VT(pVarResult) = VT_BOOL;
				V_BOOL(pVarResult) = TRUE;
			}
		}
		if (wFlags & DISPATCH_METHOD)
		{
			// params
			CString sReqId = pDispParams->rgvarg[0].bstrVal; // in case you want a CString copy

			PUTLOG("Query Phone Code By: %s", TOANSI(sReqId.GetBuffer()));

			std::string response = CCustIETools::QueryPhoneCode(TOANSI(sReqId.GetBuffer()));

			CString strVCode = TOUNICODE(response.c_str());

			// return 
			if (pVarResult) {
				VariantInit(pVarResult);
				V_VT(pVarResult) = VT_BSTR;
				V_BSTR(pVarResult) = strVCode.AllocSysString();
			}
		}
	}
	if (dispIdMember == DISPID_CB_Exit)
	{
		if (wFlags & DISPATCH_PROPERTYGET)
		{
			if (pVarResult != NULL)
			{
				VariantInit(pVarResult);
				V_VT(pVarResult) = VT_BOOL;
				V_BOOL(pVarResult) = TRUE;
			}
		}
		if (wFlags & DISPATCH_METHOD)
		{
			// params
			BOOL fForce = pDispParams->rgvarg[0].boolVal; // in case you want a CString copy

			bool fStat = CCustIETools::Exit(fForce ? true : false);

			// return 
			if (pVarResult) {
				VariantInit(pVarResult);
				V_VT(pVarResult) = VT_BOOL;
				V_BOOL(pVarResult) = fStat ? TRUE : FALSE;
			}
		}
	}
	if (dispIdMember == DISPID_CB_SetElementFocus)
	{
		if (wFlags & DISPATCH_PROPERTYGET)
		{
			if (pVarResult != NULL)
			{
				VariantInit(pVarResult);
				V_VT(pVarResult) = VT_BOOL;
				V_BOOL(pVarResult) = TRUE;
			}
		}
		if (wFlags & DISPATCH_METHOD)
		{
			// params
			CString sElementTag = pDispParams->rgvarg[0].bstrVal; // in case you want a CString copy

			bool fStat = CCustIETools::SetElementFocus(TOANSI(sElementTag.GetBuffer()));

			// return 
			if (pVarResult) {
				VariantInit(pVarResult);
				V_VT(pVarResult) = VT_BOOL;
				V_BOOL(pVarResult) = fStat ? TRUE : FALSE;
			}
		}
	}
	if (dispIdMember == DISPID_CB_GetHttpOnlyCookie)
	{
		if (wFlags & DISPATCH_PROPERTYGET)
		{
			if (pVarResult != NULL)
			{
				VariantInit(pVarResult);
				V_VT(pVarResult) = VT_BOOL;
				V_BOOL(pVarResult) = TRUE;
			}
		}
		if (wFlags & DISPATCH_METHOD)
		{
			// params
			CString sUrl = pDispParams->rgvarg[1].bstrVal; // in case you want a CString copy
			CString sCookieName = pDispParams->rgvarg[0].bstrVal; // in case you want a CString copy

			std::string sCookie;
			bool fStat = CCustIETools::GetHttpOnlyCookie(TOANSI(sUrl.GetBuffer()), TOANSI(sCookieName.GetBuffer()), sCookie);

			CString strCookie = TOUNICODE(sCookie);

			// return 
			if (pVarResult) {
				VariantInit(pVarResult);
				V_VT(pVarResult) = VT_BSTR;
				V_BSTR(pVarResult) = strCookie.AllocSysString();
			}
		}
	}
	if (dispIdMember == DISPID_CB_PutLog)
	{
		if (wFlags & DISPATCH_PROPERTYGET)
		{
			if (pVarResult != NULL)
			{
				VariantInit(pVarResult);
				V_VT(pVarResult) = VT_BOOL;
				V_BOOL(pVarResult) = TRUE;
			}
		}
		if (wFlags & DISPATCH_METHOD)
		{
			// params
			CString sLog = pDispParams->rgvarg[0].bstrVal; 
			PUTLOG(TOANSI(sLog.GetBuffer()));
			// return 
			if (pVarResult) {
				VariantInit(pVarResult);
				V_VT(pVarResult) = VT_BOOL;
				V_BOOL(pVarResult) = TRUE;
			}
		}
	}
	return hr;
}
