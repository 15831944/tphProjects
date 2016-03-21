#include "stdafx.h"
#ifndef SHARED_HANDLERS
#include "CustIE.h"
#endif

#include "CustIEDoc.h"
#include "DocHostHtmlView.h"
#include "CustIEView.h"

#include "CustIETools.h"

IMPLEMENT_DYNCREATE(CCustIEView, CDocHostHtmlView)

BEGIN_MESSAGE_MAP(CCustIEView, CDocHostHtmlView)
    ON_WM_PARENTNOTIFY()
    ON_WM_TIMER()
END_MESSAGE_MAP()

CCustIEView::CCustIEView()
{
    m_pHtmlDoc = NULL;
}

CCustIEView::~CCustIEView()
{
}

BOOL CCustIEView::PreCreateWindow(CREATESTRUCT& cs)
{
    return CDocHostHtmlView::PreCreateWindow(cs);
}

HRESULT CCustIEView::OnShowMessage(HWND hwnd,
    LPOLESTR lpstrText,
    LPOLESTR lpstrCaption,
    DWORD dwType,
    LPOLESTR lpstrHelpFile,
    DWORD dwHelpContext,
    LRESULT * plResult)
{
    if (!lpstrText) return S_OK;

    PUTLOG("Alert: [%s]", TOANSI(lpstrText));

    if (dwType & MB_ABORTRETRYIGNORE) *plResult = IDABORT;
    else if ((dwType & MB_YESNO )|| (dwType & MB_YESNOCANCEL)) *plResult = IDYES;
    else *plResult = IDCANCEL;

    // Handle alerts in script
    int statusCode = (CCustIETools::HandleAlerts(theApp.m_userConfig.toStyledString(), TOANSI(lpstrText)));

    if (statusCode)
    {
        PUTLOG("* Catch alert error.");
        Json::Value voucherMap;
        voucherMap["errmsg"] = TOANSI(lpstrText);
        CCustIETools::StatusReport(theApp.m_userConfig, statusCode, voucherMap.toStyledString());
    }

    return S_OK;
}

#pragma warning(disable:4996)
void CCustIEView::OnDocumentComplete(LPCTSTR lpszURL)
{
    PUTLOG("* Loading Page, URL: [%s]", TOANSI(lpszURL));

    m_pHtmlDoc = GetHtmlDocument();
    CComPtr<IHTMLDocument2> spDoc2 = NULL;
    m_pHtmlDoc->QueryInterface(IID_IHTMLDocument, (void **)&spDoc2);

    BSTR bstrStat = NULL;
    std::string strState;
    spDoc2->get_readyState(&bstrStat);
    if (bstrStat)
    {
        PUTLOG("* State: [%s]", TOANSI(bstrStat));
        strState = TOANSI(bstrStat);
        SysFreeString(bstrStat);
    }

    BSTR bstrCookie = NULL;
    spDoc2->get_cookie(&bstrCookie);
    if (bstrCookie){
        //PUTLOG("* Cookie: [%s]", TOANSI(bstrCookie));
        SysFreeString(bstrCookie);
    }

    // TODO: more strict, only load script for LoginUrl, PhoneUrl & WelcomeUrl
    if ( strState == "complete" ) 
    {
        do 
        {
            CCustIETools::Hook_Abc();
        }while(0);

        do 
        {
            std::string userConfig = "";
            userConfig += "if ('undefined' === typeof GlobalSettings) { ";
            userConfig += "    GlobalSettings = ";
            userConfig +=      theApp.m_userConfig.toStyledString();
            userConfig += "};";
            // Bug: url is not taken effect in next call;
            CCustIETools::LoadScript(CComPtr<IDispatch>(GetHtmlDocument()), userConfig, false);
        } while (0);

        do
        {
            std::ifstream ifs(CCustIETools::GetScriptPath() + "json.js");
            std::string str((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
            int z = str.length();
            CCustIETools::LoadScript(CComPtr<IDispatch>(GetHtmlDocument()), str, false);
        }while(0);

        do
        {
            std::ifstream ifs(CCustIETools::GetScriptPath() + "utility.js");
            std::string str((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
            int z = str.length();
            CCustIETools::LoadScript(CComPtr<IDispatch>(GetHtmlDocument()), str, false);
        }while(0);

        do
        {
            std::ifstream ifs(CCustIETools::GetConfigPath() + "statem.js");
            std::string str((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
            int z = str.length();
            CCustIETools::LoadScript(CComPtr<IDispatch>(GetHtmlDocument()), str, false);
        }while(0);

        do
        {
            std::ifstream ifs(CCustIETools::GetConfigPath() + "statuscode.js");
            std::string str((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
            int z = str.length();
            CCustIETools::LoadScript(CComPtr<IDispatch>(GetHtmlDocument()), str, false);
        }while(0);

        do
        {
            std::ifstream ifs(CCustIETools::GetConfigPath() + "banks.js");
            std::string str((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
            int z = str.length();
            CCustIETools::LoadScript(CComPtr<IDispatch>(GetHtmlDocument()), str, false);
        }while(0);

        do{
            std::ifstream ifs(CCustIETools::GetConfigPath() + "alert.js");
            std::string str((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
            int z = str.length();
            CCustIETools::LoadScript(CComPtr<IDispatch>(GetHtmlDocument()), str, false);
        }while(0);	

        do
        {
            std::ifstream ifs(CCustIETools::GetScriptPath() + "login.js");
            std::string str((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
            int z = str.length();
            CCustIETools::LoadScript(CComPtr<IDispatch>(GetHtmlDocument()), str, false);
        }while(0);

        do
        {
            std::ifstream ifs(CCustIETools::GetScriptPath() + "phone.js");
            std::string str((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
            int z = str.length();
            CCustIETools::LoadScript(CComPtr<IDispatch>(GetHtmlDocument()), str, false);
        }while(0);

        do
        {
            std::ifstream ifs(CCustIETools::GetScriptPath() + "handler.js");
            std::string str((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
            int z = str.length();
            CCustIETools::LoadScript(CComPtr<IDispatch>(GetHtmlDocument()), str, false);
        }while(0);

        do
        {
            std::ifstream ifs(CCustIETools::GetScriptPath() + "monitor.js");
            std::string str((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
            int z = str.length();
            CCustIETools::LoadScript(CComPtr<IDispatch>(GetHtmlDocument()), str, false);
        }while(0);

        do
        {
            std::ifstream ifs(CCustIETools::GetScriptPath() + "main.js");
            std::string str((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
            int z = str.length();
            CCustIETools::LoadScript(CComPtr<IDispatch>(GetHtmlDocument()), str, false);
        }while(0);

        do
        {
            std::string strStartupCode = "";
            strStartupCode += "Main('" ;
            strStartupCode += TOANSI(lpszURL);
            strStartupCode += "');";
            CCustIETools::LoadScript(CComPtr<IDispatch>(GetHtmlDocument()), strStartupCode, false);
        }while(0);
    } 
    return;
}

HRESULT CCustIEView::OnShowContextMenu(DWORD dwID, LPPOINT ppt,
    LPUNKNOWN pcmdtReserved, LPDISPATCH pdispReserved)
{
    return S_OK; // disable context menu to show.
    //return CHtmlView::OnShowContextMenu(dwID, ppt, pcmdtReserved, pdispReserved);
}

void CCustIEView::OnInitialUpdate()
{
    PUTLOG("CCustIEView::OnInitialUpdate()");
    CDocHostHtmlView::OnInitialUpdate();

    SetTimer(TIMER_ID_UPDATE_GSET, 1000, NULL);

    Json::Value & userConfig = theApp.m_userConfig;
    std::string sBankId = "";
    if (userConfig.isObject() && userConfig.isMember("type")) 
    {
        sBankId = userConfig["type"].asString();
    }
    std::string sSubType = "";
    if (userConfig.isObject() && userConfig.isMember("subtype"))
    {
        sSubType = userConfig["subtype"].asString();
    }

    std::string sLoginUrl = theApp.GetUrlSetting(sBankId, "loginUrl", sSubType);
    if (!sLoginUrl.empty())
        Navigate2(TOUNICODE(sLoginUrl.c_str()), 0, 0);
    else
    {
        //PrintHelp();
        Json::Value voucherMap;
        voucherMap["errmsg"] = "Not found type:" +sBankId;
        CCustIETools::StatusReport(theApp.m_userConfig, StatusCode(LOG_FAILE_NOTFINDTYPE), voucherMap.toStyledString());
    }
}

void CCustIEView::PrintHelp()
{
    const std::string strPageContent = "<h1> LoginBot.exe </h1>" \
        "<ul>" \
        "<li>--type:    bank id</li>" \
        "<li>--subtype: sub type</li>" \
        "<li>--user:    username</li>" \
        "<li>--pass:    password</li>" \
        "<li>--taskkey: task key</li>" \
        "<li>--reqid:   request id</li>" \
        "<li>--host:    address of callback host</li>" \
        "</ul>" 		
        ;

    std::string stmp = CCustIETools::GetTempFilePath("loginBot", "html", true);

    std::ofstream fout;
    fout.open(stmp.c_str(), std::ios::out|std::ios::binary);
    fout.write(strPageContent.c_str(), strPageContent.length());
    fout.close();

    Navigate2(TOUNICODE(stmp));
}

HRESULT CCustIEView::OnGetExternal(LPDISPATCH *lppDispatch)
{
    *lppDispatch = theApp.GetExternalDispatch();
    return S_OK;
}

// CCustIEView diagnostics

#ifdef _DEBUG
void CCustIEView::AssertValid() const
{
    CDocHostHtmlView::AssertValid();
}

void CCustIEView::Dump(CDumpContext& dc) const
{
    CDocHostHtmlView::Dump(dc);
}

CCustIEDoc* CCustIEView::GetDocument() const // non-debug version is inline
{
    ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CCustIEDoc)));
    return (CCustIEDoc*)m_pDocument;
}
#endif //_DEBUG
void CCustIEView::OnParentNotify(UINT message, LPARAM lParam)
{
    CDocHostHtmlView::OnParentNotify(message, lParam);

    if ((LOWORD(message) == WM_DESTROY) && ((HWND)lParam == m_wndBrowser))
    {
        GetParentFrame()->PostMessage(WM_CLOSE, 0, 0);
    }
}

void CCustIEView::OnNavigateComplete2(LPCTSTR lpszURL)
{
    // TODO: Add your specialized code here and/or call the base class
    PUTLOG("* Navigate done: [%s]", TOANSI(lpszURL));
    CDocHostHtmlView::OnNavigateComplete2(lpszURL);
}

void CCustIEView::OnBeforeNavigate2(LPCTSTR lpszURL, DWORD nFlags, LPCTSTR lpszTargetFrameName, CByteArray& baPostedData, LPCTSTR lpszHeaders, BOOL* pbCancel)
{
    PUTLOG("* Try to navigate frame [%s] to url [%s]", TOANSI(lpszTargetFrameName), TOANSI(lpszURL));
    // TODO: Add your specialized code here and/or call the base class
    CDocHostHtmlView::OnBeforeNavigate2(lpszURL, nFlags, lpszTargetFrameName, baPostedData, lpszHeaders, pbCancel);
}


void CCustIEView::OnTimer(UINT_PTR nIDEvent)
{
    // TODO: Add your message handler code here and/or call default
    if(nIDEvent == TIMER_ID_UPDATE_GSET){
        //PUTLOG("* Update GlobalSetting in timer");
        Json::Value gset;
        if (!CCustIETools::LoadGlobalSettings(gset)){
            //PUTLOG("* Failed update GlobalSetting from cache");
            return ;
        }else{
            // Do Update
            if (gset.isObject() && gset.isMember("reqId")){
                std::string sReqId = gset["reqId"].asString();
                if(!sReqId.empty()){
                    PUTLOG("* Update <reqId> in GlobalSettings, [%s]", sReqId.c_str());
                    theApp.m_userConfig["reqid"] = sReqId;
                    // TODO: Notify Script Context of page
                    // ...
                }else{
                    PUTLOG("* Invalid reqid, ignore");
                }
            }else{
                PUTLOG("* cached GlobalSetting is invalid");
            }
        }
    }
    CDocHostHtmlView::OnTimer(nIDEvent);
}
