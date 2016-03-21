#include "stdafx.h"
#include "CustIETools.h"
#include "MainFrm.h"
#include "CustIE.h"

HANDLE CCustIETools::m_hKeyInputLock = INVALID_HANDLE_VALUE;

HANDLE CCustIETools::m_hEvtKeyInputDone = INVALID_HANDLE_VALUE;

#ifdef USE_DETOURS
DWORD (__stdcall * CCustIETools::Real_InternetSetCookieExW)(LPCWSTR , LPCWSTR , LPCWSTR , DWORD , DWORD_PTR ) = InternetSetCookieExW;
BOOL  (__stdcall * CCustIETools::Real_InternetSetCookieW)(LPCWSTR , LPCWSTR , LPCWSTR  ) = InternetSetCookieW;
DWORD (__stdcall * CCustIETools::Real_InternetSetCookieExA)(LPCSTR , LPCSTR , LPCSTR , DWORD , DWORD_PTR ) = InternetSetCookieExA;
BOOL  (__stdcall * CCustIETools::Real_InternetSetCookieA)(LPCSTR , LPCSTR , LPCSTR  ) = InternetSetCookieA;
BOOL  (__stdcall * CCustIETools::Real_InternetGetCookieExW)(LPCWSTR , LPCWSTR , LPWSTR , LPDWORD ,  DWORD ,  LPVOID  ) = InternetGetCookieExW;
BOOL  (__stdcall * CCustIETools::Real_InternetGetCookieExA)(LPCSTR , LPCSTR , LPSTR , LPDWORD ,  DWORD ,  LPVOID  )= InternetGetCookieExA;
#endif

CCustIETools::CCustIETools()
{
}


CCustIETools::~CCustIETools()
{
}

std::string CCustIETools::ToAnsiString(const std::wstring & wstr)
{
    std::string strResult;
    int expectedBufferLength = WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), -1, NULL, 0, NULL, NULL);
    if (expectedBufferLength > 0) 
    {
        char * pszBuffer = (char *)malloc(expectedBufferLength);
        if (pszBuffer) 
        {
            WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), -1, pszBuffer, expectedBufferLength, NULL, NULL);
            pszBuffer[expectedBufferLength-1] = '\0';
            strResult = pszBuffer;
            free(pszBuffer);
            pszBuffer = NULL;
        }
    }
    return strResult;
}

std::string CCustIETools::ToUtf8String(const std::wstring & wstr)
{
    std::string strResult;
    int expectedBufferLength = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, NULL, 0, NULL, NULL);
    if (expectedBufferLength > 0) 
    {
        char * pszBuffer = (char *)malloc(expectedBufferLength);
        if (pszBuffer) 
        {
            WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, pszBuffer, expectedBufferLength, NULL, NULL);
            pszBuffer[expectedBufferLength-1] = '\0';
            strResult = pszBuffer;
            free(pszBuffer);
            pszBuffer = NULL;
        }
    }
    return strResult;
}

std::wstring CCustIETools::ToUnicodeString(const std::string & str)
{
    std::wstring wstrResult;
    int expectedBufferLength = MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, NULL, 0);
    if (expectedBufferLength) 
    {
        wchar_t * pwszBuffer = (wchar_t *)malloc(sizeof(wchar_t) * expectedBufferLength);
        if (pwszBuffer) 
        {
            MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, pwszBuffer, expectedBufferLength);
            pwszBuffer[expectedBufferLength-1] = L'\0';
            wstrResult = pwszBuffer;
            free(pwszBuffer);
            pwszBuffer = NULL;
        }
    }
    return wstrResult;
}

std::string CCustIETools::ToHexString(const unsigned char * buff, size_t len)
{
    static const unsigned char HEXTABLE [] = "0123456789abcdef";
    std::string strRet = "";
    for (size_t i = 0; i < len ; i ++)
    {
        unsigned char ch = buff[i];
        strRet += HEXTABLE[((ch & 0xf0) >> 4)];
        strRet += HEXTABLE[((ch & 0x0f))];
    }
    return strRet;
}

#pragma warning(disable:4996)
char * CCustIETools::StrDup(const char * src)
{
    size_t slen = strlen(src);
    char * temp = (char *)malloc(slen + 1);
    strcpy(temp, src);
    temp[slen] = '\0';
    return temp;
}

std::string CCustIETools::GetAppPath()
{
    char szBuffer[256] = "";
    GetModuleFileNameA(NULL, szBuffer, sizeof(szBuffer) / sizeof(szBuffer[0]));
    strrchr(szBuffer, '\\')[1] = '\0';
    return szBuffer;
}

std::string CCustIETools::GetConfigPath()
{
    return GetAppPath() + "config\\";
}

std::string CCustIETools::GetScriptPath()
{
    return GetAppPath() + "script\\";
}

#pragma warning(disable:4996)
void CCustIETools::PutLog(const char * format, ...)
{
    return;
    char szBuffer[1024] = "";
    va_list ap;
    va_start(ap, format);
    vsnprintf(szBuffer, sizeof(szBuffer) / sizeof(szBuffer[0]), format, ap);
    va_end(ap);
    OutputDebugStringA(szBuffer);
    // For log trace
    // Start a client on production: DbgView /a /t /g /s /e
    // Connect local DbgView to remote production.
}

bool CCustIETools::CallScript(CComPtr<IDispatch> & pHtmlDocDisp, const std::string & strFunc, const std::vector<std::string> & paramArray, std::string & result)
{
    CComPtr<IHTMLDocument2> pHtmlDoc;
    pHtmlDocDisp->QueryInterface(IID_IHTMLDocument2, (void **)&pHtmlDoc);

    CComPtr<IDispatch> spScript;
    pHtmlDoc->get_Script(&spScript);
    if (!spScript)
    {
        return false;
    }

    //Find dispid for given function in the object
    CComBSTR bstrMember(TOUNICODE(strFunc));
    DISPID dispid = NULL;
    HRESULT hr = spScript->GetIDsOfNames(IID_NULL, &bstrMember, 1,LOCALE_SYSTEM_DEFAULT, &dispid);
    if (FAILED(hr))
    {
        return false;
    }

    const int arraySize = paramArray.size();

    //Putting parameters  
    DISPPARAMS dispparams;
    memset(&dispparams, 0, sizeof(dispparams));

    dispparams.cArgs = arraySize;
    dispparams.rgvarg = new VARIANT[dispparams.cArgs];
    dispparams.cNamedArgs = 0;

    for (int i = 0; i < arraySize; i++)
    {
        CComBSTR bstr = TOUNICODE(paramArray[(arraySize - 1 - i)]); // back reading
        bstr.CopyTo(&dispparams.rgvarg[i].bstrVal);
        dispparams.rgvarg[i].vt = VT_BSTR;
    }

    EXCEPINFO excepInfo;
    memset(&excepInfo, 0, sizeof excepInfo);

    UINT nArgErr = (UINT)-1;  // initialize to invalid arg
    CComVariant vaResult;
    //Call JavaScript function         
    hr = spScript->Invoke(dispid
        , IID_NULL
        , 0
        , DISPATCH_METHOD
        , &dispparams
        , &vaResult
        , &excepInfo
        , &nArgErr);

    delete[] dispparams.rgvarg;

    if (FAILED(hr))
    {
        return false;
    }
    else 
    {
        if (vaResult.vt == VT_BSTR) 
        {
            result = TOANSI(vaResult.bstrVal);
        }
    }
    return true;
}

bool CCustIETools::EvalScript(CComPtr<IHTMLDocument2> & spHtmlDoc, const std::string & code)
{
    CComVariant vaResult;

    CComPtr<IHTMLWindow2> spWindow;
    spHtmlDoc->get_parentWindow(&spWindow);
    if (!spWindow) 
    {
        return false;
    }

    CComBSTR bstrCode = TOUNICODE(code);
    CComBSTR bstrLang = L"JavaScript";
    HRESULT hResult = spWindow->execScript(bstrCode, bstrLang, &vaResult);

    if (FAILED(hResult)) 
    {
        _com_error errmsg(hResult);
        PUTLOG("ExecScript Failed! [%s]", TOANSI((LPCWSTR)errmsg.ErrorMessage()));
        return false;
    }
    else 
    {
        //PUTLOG("ExecScript OK!");
    }

    return true;
}

bool CCustIETools::LoadScript(CComPtr<IDispatch> & pHtmlDocDisp, const std::string & script, bool recursive)
{
    CComPtr<IHTMLDocument2> spHtmlDoc;
    pHtmlDocDisp->QueryInterface(IID_IHTMLDocument2, (void**)&spHtmlDoc);
    if (spHtmlDoc) 
    {
        if (recursive)
            return EvalScriptEx(spHtmlDoc, script);
        else
            return EvalScript(spHtmlDoc, script);
    }
    return false;
}

std::string CCustIETools::LastErrMsg(unsigned int dwError)
{
    LPSTR lpBuffer = NULL;
    if (FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
        NULL, dwError,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_SYS_DEFAULT),
        (LPSTR)&lpBuffer, 0, NULL))
    {
        std::string strErrMsg = lpBuffer;
        LocalFree(lpBuffer);
        return strErrMsg;
    }
    return "";
}

#ifdef _MSC_VER
#pragma warning(disable:4996)
#endif
bool CCustIETools::Fork(const std::string & application, const std::vector<std::string> & strParams, bool waitForChild, unsigned int * pRetCode)
{
    std::string cmdline = "\"";
    cmdline += application;
    cmdline += "\"";

    for (unsigned int i = 0; i < strParams.size(); ++i) 
    {
        cmdline += " \"";
        cmdline += strParams[(i)];
        cmdline += "\"";
    }

    char * sCmdLine = CCustIETools::StrDup(cmdline.c_str());

    PROCESS_INFORMATION pi;
    memset(&pi, 0, sizeof(pi));
    STARTUPINFOA si;
    memset(&si, 0, sizeof(si));
    si.cb = sizeof(si);

    DWORD dwCreateFlags = CREATE_NO_WINDOW;

    OutputDebugStringA("KeyInput command line.c_str\n");
    OutputDebugStringA(sCmdLine);
    if (CreateProcessA(NULL, sCmdLine, NULL, NULL, FALSE, dwCreateFlags, NULL, NULL, &si, &pi)) 
    {
        if (waitForChild) 
        {
            WaitForSingleObject(pi.hProcess, INFINITE);
            DWORD dwRetCode;
            GetExitCodeProcess(pi.hProcess, &dwRetCode);
            *pRetCode = dwRetCode;
        }

        CloseHandle(pi.hThread);
        CloseHandle(pi.hProcess);
        free(sCmdLine);
        return true;
    }

    free(sCmdLine);
    return false;
}

DWORD __stdcall CCustIETools::ThreadFormFill(void * param)
{
    if (theApp.SuspendFlag())
    {
        SuspendProcByName("LoginBot.exe", true);
    }

    // Wait for GUI to focus on;
    ::Sleep(theApp.GetKeyInputWait()); 

    // Parse thread parameters
    struct FormFillParam_t  * pFormFillParam = (struct FormFillParam_t *)param;
    unsigned int nInputInterval = pFormFillParam->nInputInterval;
    char * sValue = pFormFillParam->sValue;
    bool fWait = pFormFillParam->fWait ? true : false;
    free(pFormFillParam); pFormFillParam = NULL;
    // Prepare parameters
    char szBuffer[256] = "";
    std::vector<std::string> vecStrParams;
    vecStrParams.push_back("-w");
    vecStrParams.push_back(itoa(nInputInterval, szBuffer, 10));
    vecStrParams.push_back(sValue);
    free(sValue); sValue = NULL;
    // Fork child process
    unsigned int retCode = 0;
    std::string strKeyInputPath = GetAppPath() + theApp.GetKeyInputName();
    OutputDebugStringA(strKeyInputPath.c_str());
    Fork(strKeyInputPath, vecStrParams, fWait, &retCode);

    if (theApp.SuspendFlag())
    {
        SuspendProcByName("LoginBot.exe", false);
    }

    // Release lock
    ((CMainFrame *)(AfxGetApp()->m_pMainWnd))->PostMessage(WM_USER_INPUT_DONE);

    return 0;
}

bool CCustIETools::KeyInput(unsigned int nInputInterval, const std::string & sValue, bool fWait)
{
    DWORD dwThreadId = 0;
    struct FormFillParam_t * pFormFillParam = (struct FormFillParam_t *)malloc(sizeof(FormFillParam_t));
    memset(pFormFillParam, 0, sizeof(struct FormFillParam_t));
    pFormFillParam->nInputInterval = nInputInterval;
    pFormFillParam->sValue = CCustIETools::StrDup(sValue.c_str());
    pFormFillParam->fWait = fWait;
    HANDLE hInputThread = CreateThread(NULL, 0, ThreadFormFill, pFormFillParam, 0, &dwThreadId);
    return true;
}

bool CCustIETools::IsKeyInputDone()
{
    if (WaitForSingleObject(m_hEvtKeyInputDone, 0) != WAIT_TIMEOUT)
    {
        return true;
    }
    return false;
}

bool CCustIETools::ReleaseKeyInputLock()
{
    ReleaseMutex(m_hKeyInputLock);
    return true;
}

bool CCustIETools::SetKeyInputDone()
{
    return SetEvent(m_hEvtKeyInputDone) ? true : false;
}

///////////////////////////////////////////////////////////////////////////////
bool CCustIETools::GetKeyInputLock()
{
    if (WaitForSingleObject(m_hKeyInputLock, 0) != WAIT_TIMEOUT) 
    {
        PUTLOG("* GetKeyInputLock, TID: %u", GetCurrentThreadId());
        return true;
    }
    return false;
}

bool CCustIETools::ActiveWindow(/*bool fFocus*/)
{
    PUTLOG("* Active window");
    HWND  hForeWnd =  GetForegroundWindow(); 
    DWORD dwForeThID =  GetWindowThreadProcessId( hForeWnd, NULL ); 

    HWND hWnd= AfxGetMainWnd()->GetSafeHwnd(); 
    DWORD dwCurThID  =  GetCurrentThreadId(); 

    if (dwCurThID != dwForeThID)
    {
        if (!AttachThreadInput(dwCurThID, dwForeThID, TRUE/*fFocus*/))
        {
            PUTLOG("Failed to attach thread input");
        }else{
            PUTLOG("* OK to attach thread input");
        }
    }
    // May Fail, as "https://msdn.microsoft.com/en-us/subscriptions/wzcddbek%28v=vs.84%29.aspx"
    //  An example is a Command Prompt window that is hosted by the Console Window Host process.
    // More: http://www.howtogeek.com/howto/4996/what-is-conhost.exe-and-why-is-it-running/
    if (SetForegroundWindow(hWnd))
    {
        LockSetForegroundWindow(LSFW_LOCK);
        ShowWindow( hWnd, SW_NORMAL ); 
        if (TRUE)
        {
            SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE|SWP_NOMOVE);
        }
    }
    // And how to fix:
    // mouse_event(...)
    else
    {
        ::ShowWindow(hWnd, SW_NORMAL);
        ::SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE|SWP_NOMOVE);
        ActiveWindowByMouseEvent(hWnd);
    }
    return true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
CComPtr<IHTMLDocument2> CCustIETools::GetHtmlDocFromIFrameWindow(CComPtr<IHTMLWindow2> spWindow)
{
    if (!spWindow) 
    {
        return CComPtr<IHTMLDocument2>();
    }

    CComPtr<IHTMLDocument2> spDocument;
    HRESULT hRes = spWindow->get_document(&spDocument);
    if ((S_OK == hRes) && (spDocument != NULL))
    {
        return spDocument;
    }

    // hRes could be E_ACCESSDENIED that means a security restriction that
    // prevents scripting across frames that loads documents from different internet domains.
    CComQIPtr<IServiceProvider>  spServiceProvider = spWindow;
    if (spServiceProvider == NULL)
    {
        return CComPtr<IHTMLDocument2>();
    }

    CComPtr<IWebBrowser2> spWebBrws;
    hRes = spServiceProvider->QueryService(IID_IWebBrowserApp, IID_IWebBrowser2, (void**)&spWebBrws);
    if (hRes != S_OK)
    {
        return CComPtr<IHTMLDocument2>();
    }

    CComPtr<IWebBrowser2> spBrws = spWebBrws;
    if (spBrws == NULL)
    {
        return CComPtr<IHTMLDocument2>();
    }

    // Get the document object from the IWebBrowser2 object.
    CComPtr<IDispatch> spDisp;
    hRes = spBrws->get_Document(&spDisp);
    spDocument = spDisp;
    return spDocument;
}

bool CCustIETools::EvalScriptEx(CComPtr<IHTMLDocument2> spHtmlDoc, const std::string & script)
{
    // First, Eval script on current DOM
    BSTR bstrDocUrl = NULL;
    spHtmlDoc->get_URL(&bstrDocUrl);
    if (bstrDocUrl) 
    {
        //PUTLOG("* Run script on IFrame, URL: %s", TOANSI(bstrDocUrl));
        SysFreeString(bstrDocUrl);
    }
    EvalScript(spHtmlDoc, script);

    // Second, Recursively call in all child frames.
    CComPtr<IHTMLFramesCollection2> spFrameCollection;
    spHtmlDoc->get_frames(&spFrameCollection);
    long nFrameCount = 0;
    if (spFrameCollection) 
    {
        spFrameCollection->get_length(&nFrameCount);
    }
    for (long i = 0; i < nFrameCount; i++) 
    {
        CComVariant varFrame; CComVariant varIndex(i);
        if (SUCCEEDED(spFrameCollection->item(&varIndex, &varFrame))
            && varFrame.vt == VT_DISPATCH
            && varFrame.pdispVal)
        {
            CComQIPtr<IHTMLWindow2> spFrameWin = varFrame.pdispVal;

            CComPtr<IHTMLDocument2> spFrameHtmlDoc = GetHtmlDocFromIFrameWindow(spFrameWin);
            if (!spFrameHtmlDoc) 
            {
                continue;
            }
            EvalScriptEx(spFrameHtmlDoc, script);
        }
    }

    return true;
}

bool CCustIETools::SaveImageFromClip(const std::string & filepath)
{
    if (!OpenClipboard(AfxGetApp()->m_pMainWnd->GetSafeHwnd())){
        PUTLOG("* Failed to open clipboard, %u", GetLastError());
        return false;
    }

    HBITMAP source = (HBITMAP) GetClipboardData(CF_BITMAP);
    if (!source)
    {
        PUTLOG("* Failed to Get clipboard data");
        CloseClipboard();
        return false;
    }

    BITMAP source_info = { 0 };
    if( !::GetObject( source, sizeof( source_info ), &source_info ) )
    {
        PUTLOG("* Failed to get BITMAP");
        CloseClipboard();
        return false;
    }

    Gdiplus::PixelFormat pixel_format = PixelFormat24bppRGB;
    if (source_info.bmBitsPixel == 32)
    {
        pixel_format = PixelFormat32bppARGB;
    }

    Gdiplus::Status s;
    std::auto_ptr< Gdiplus::Bitmap > target( new Gdiplus::Bitmap( source_info.bmWidth, source_info.bmHeight, pixel_format ) );
    if( !target.get() )
    {
        PUTLOG("* Failed to new GDI:Bitmap");
        CloseClipboard();
        return false;
    }

    if( ( s = target->GetLastStatus() ) != Gdiplus::Ok )
    {
        PUTLOG("* Failed to create GDI:Bitmap");
        CloseClipboard();
        return false;
    }

    Gdiplus::BitmapData target_info;
    Gdiplus::Rect rect( 0, 0, source_info.bmWidth, source_info.bmHeight );

    s = target->LockBits( &rect, Gdiplus::ImageLockModeWrite, pixel_format, &target_info );
    if( s != Gdiplus::Ok )
    {
        PUTLOG("* Failed to LockBits");
        CloseClipboard();
        return false;
    }

    if( target_info.Stride != source_info.bmWidthBytes )
    {
        PUTLOG("* Bitmap format error, target:[%d], source:[%d]", target_info.Stride, source_info.bmWidthBytes );
        CloseClipboard();
        return false; // pixel_format is wrong!
    }

    source_info.bmBits = malloc(source_info.bmWidthBytes * source_info.bmHeight);

    GetBitmapBits(source, source_info.bmWidthBytes * source_info.bmHeight, source_info.bmBits);

    CopyMemory( target_info.Scan0, source_info.bmBits, source_info.bmWidthBytes * source_info.bmHeight );

    free(source_info.bmBits);

    s = target->UnlockBits( &target_info );
    if( s != Gdiplus::Ok )
    {
        PUTLOG("* Failed to unlock bits");
        CloseClipboard();
        return false;
    }

    CLSID clsidPNG;
    GetEncoderClsid("image/jpeg", &clsidPNG);
    target->Save(TOUNICODE(filepath.c_str()),&clsidPNG);

    EmptyClipboard();

    CloseClipboard();
    return true;
}

std::string CCustIETools::GetVCodeTextByClip(const std::string & sBankId, CComPtr<IHTMLElement2> spBody, CComPtr<IHTMLElement> spElement, int imgWidth, int imgHeight)
{
    IDispatch* pdispCtrlRange = NULL;

    CComPtr<IHTMLControlRange> spCtrlRange;

    if (FAILED(spBody->createControlRange(&pdispCtrlRange)) )
        return "";

    if (pdispCtrlRange == NULL)
        return "";

    if (FAILED(pdispCtrlRange->QueryInterface(IID_IHTMLControlRange, (void**) &spCtrlRange)) )
        return "";

    if (spCtrlRange == NULL)
        return "";

    CComPtr<IHTMLControlElement> spCtrlElement;
    if (FAILED(spElement->QueryInterface(IID_IHTMLControlElement, (void**) &spCtrlElement)) )
        return "";

    if (FAILED(spCtrlRange->add(spCtrlElement)))
        return "";

    VARIANT vEmpty;
    VariantInit(&vEmpty);
    VARIANT_BOOL vbReturn;
    if(FAILED(spCtrlRange->execCommand(_bstr_t("Copy"), VARIANT_FALSE, vEmpty, &vbReturn)))
    {
        return "";
    }

    std::string sImagePath = GetTempFilePath(sBankId, "jpeg", true);

    if (!SaveImageFromClip(sImagePath))
    {
        PUTLOG("Failed to save image from clipboard");
        return "";
    }

    PUTLOG("Try to post image to ocr engine:[%s] [%s]", sBankId.c_str(), sImagePath.c_str());

    std::string sImageHash = "";
    if (!RequestOcr(sBankId, sImagePath, sImageHash)) 
    {
        PUTLOG("Failed to Request OCR for image");
        return "";
    }
    return sImageHash;
}

std::string CCustIETools::GetVCodeText(const std::string & sBankId, CComPtr<IHTMLElement> spElement, int imgWidth, int imgHeight)
{
    CComPtr<IHTMLElementRender> spRender;
    spElement->QueryInterface(IID_IHTMLElementRender, (void **)&spRender);

    if (!spRender) 
    {
        PUTLOG("OCR: Failed to get imageRender interface");
        return "";
    }

    long cx, cy = 0;
    HRESULT hr = S_OK;
    if (imgWidth == 0)
        hr = spElement->get_offsetWidth(&cx);
    else
    {
        cx = imgWidth;
    }
    if (imgHeight == 0)
        hr = spElement->get_offsetHeight(&cy);
    else
    {
        cy = imgHeight;
    }
    if (!(cx > 0 && cy > 0)) 
    {
        PUTLOG("OCR: failed to get width [%d] & height[%d], hr=%x.", cx, cy, hr);
        return "";
    }

    Bitmap snap(cx, cy);
    Graphics canvas(&snap);
    HDC hSnapDc = canvas.GetHDC();
    if (hSnapDc == NULL) 
    {
        PUTLOG("OCR: Failed to get DC");
        return "";
    }
    spRender->DrawToDC(hSnapDc);
    canvas.ReleaseHDC(hSnapDc);
    CLSID clsidPNG;
    GetEncoderClsid("image/jpeg", &clsidPNG);
    std::string sImagePath = GetTempFilePath(sBankId, "jpeg", true);
    if (Gdiplus::Status(Ok) != snap.Save(TOUNICODE(sImagePath.c_str()), &clsidPNG, NULL))
    {
        PUTLOG("OCR: failed to save image.");
        return "";
    }

    PUTLOG("Try to post image to ocr engine:[%s] [%s]", sBankId.c_str(), sImagePath.c_str());

    std::string sImageHash = "";
    if (!RequestOcr(sBankId, sImagePath, sImageHash)) 
    {
        PUTLOG("Failed to Request OCR for image");
        return "";
    }
    return sImageHash;
}

std::string CCustIETools::QueryVCode(const std::string & sImageHash)
{
    std::string sText;
    if (QueryOcrResult(sImageHash, sText))
    {
        return sText;
    }
    return "";
}

std::string CCustIETools::QueryPhoneCode(const std::string & sKey)
{
    //TODO: Query local file by sTaskKey to get phone code
    std::string strSMSCode;

    if (sKey.empty() || sKey.find("TASK:") != 0)
    {
        return strSMSCode;
    }	
    std::string sTaskkey = sKey.substr(5);

    std::string sResultFile = GetTempFilePath(sTaskkey, "json", false);
    std::string strResult;
    do
    {
        std::ifstream ifs(sResultFile);
        strResult = std::string((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
        if (strResult.length() == 0)
        {
            return strSMSCode;
        }		
    }
    while(0);

    Json::Reader reader;
    Json::Value jdata ;
    if (!reader.parse(strResult, jdata))
    {
        return strSMSCode;
    }
    // TODO:improve it
    // Refresh GlobalSettings into cache file
    else
    {
        CCustIETools::SaveGlobalSettings(jdata);
    }

    if  (!jdata.isObject() || !jdata.isMember("extraParams"))
    {
        return strSMSCode;
    }

    Json::Value jsonExtra = jdata["extraParams"];
    if (!jsonExtra.isObject() || !jsonExtra.isMember("smsCode"))
    {
        return strSMSCode;
    }

    //TODO: Ugly code
    if (jsonExtra.isMember("reqId"))
    {
        std::string sReqId = jsonExtra["reqId"].asString();
        if (!sReqId.empty())
        {
            PUTLOG("* TODO: Update GlobalSetting <reqId> by [%s]", sReqId.c_str());
        }
    }

    strSMSCode = jsonExtra["smsCode"].asString();

    PUTLOG("Phone code result: [%s]", strSMSCode.c_str());

    if(!strSMSCode.empty())
    {
        ::DeleteFileA(sResultFile.c_str());
    }

    return strSMSCode;
}

bool CCustIETools::InitKeyInputLock()
{
    m_hKeyInputLock = CreateMutexA(NULL, FALSE, "Local\\MutexForKeyInput");
    return (m_hKeyInputLock != NULL);
}

bool CCustIETools::InitKeyInputNotify()
{
    m_hEvtKeyInputDone = CreateEventA(NULL, FALSE, FALSE, NULL);
    return (m_hEvtKeyInputDone != NULL);
}

bool CCustIETools::DeinitKeyInputLock()
{
    if (m_hKeyInputLock) 
    {
        CloseHandle(m_hKeyInputLock);
        m_hKeyInputLock = INVALID_HANDLE_VALUE;
    }
    return true;
}

bool CCustIETools::DeinitKeyInputNotify()
{
    if (m_hEvtKeyInputDone) 
    {
        CloseHandle(m_hEvtKeyInputDone);
        m_hEvtKeyInputDone = INVALID_HANDLE_VALUE;
    }
    return true;
}
bool CCustIETools::GetEncoderClsid(const std::string & format, CLSID* pClsid)
{
    UINT  num = 0;          // number of image encoders
    UINT  size = 0;         // size of the image encoder array in bytes

    ImageCodecInfo* pImageCodecInfo = NULL;

    GetImageEncodersSize(&num, &size);
    if (size == 0)
    {
        return false;
    }

    pImageCodecInfo = (ImageCodecInfo*)(malloc(size));
    if (pImageCodecInfo == NULL)
    {
        return false;
    }

    GetImageEncoders(num, size, pImageCodecInfo);

    for (UINT j = 0; j < num; ++j)
    {
        if (!wcscmp(pImageCodecInfo[j].MimeType, TOUNICODE(format.c_str())))
        {
            *pClsid = pImageCodecInfo[j].Clsid;
            free(pImageCodecInfo); pImageCodecInfo = NULL;
            return true;  // Success
        }
    }

    free(pImageCodecInfo);
    return false;  // Failure
}

std::string CCustIETools::GetTempFilePath(const std::string & prefix, const std::string & ext, bool fUniq)
{
    char szTempPath[MAX_PATH] = "";
    GetTempPathA(sizeof(szTempPath), szTempPath);
    //strcat(szTempPath, "\\");
    strcat(szTempPath, prefix.c_str());

    if (fUniq)
    {
        strcat(szTempPath, "-");
        char timestamp[MAX_PATH] = "";
        sprintf(timestamp, "%u.%u", (unsigned int)time(NULL), GetTickCount());
        strcat(szTempPath, timestamp);
    }

    strcat(szTempPath, ".");
    strcat(szTempPath, ext.c_str());
    return szTempPath;
}

bool CCustIETools::RequestOcr(const std::string & bankid, const std::string & imagePath, std::string & sImageHash)
{
    sImageHash = MD5Sum(imagePath);
    if (sImageHash.empty())
    {
        return false;
    }

    std::string sResultFile = GetTempFilePath(sImageHash, "txt", false);
    std::vector<std::string> vecParams;
    vecParams.push_back("-b");
    vecParams.push_back(bankid);
    vecParams.push_back("-t");
    vecParams.push_back(sResultFile);
    vecParams.push_back(imagePath);
    unsigned int retCode = 0;
    std::string sOcrParser = (!stricmp(bankid.c_str(), "network")) ? "NetworkOCRWrapper.exe" : "SundayOcrWrapper.exe";
    if (!Fork(GetAppPath() + sOcrParser
        , vecParams
        //, true/*Synchronism, for test*/
        , false/*Asynchronism, for production*/
        , &retCode))
    {
        PUTLOG("Failed to fork OCR process.");
        return false;
    }

    return true;
}

bool CCustIETools::QueryOcrResult(const std::string & sImageHash, std::string & sText)
{
    std::string sResultFile = GetTempFilePath(sImageHash, "txt", false);

    FILE * fin = fopen(sResultFile.c_str(), "r");
    if (!fin) 
    {
        //PUTLOG("Failed to open OCR result file: [%s]", sResultFile.c_str());
        return false;
    }
    char szBuffer[MAX_PATH] = "";
    fscanf(fin, "%s", &szBuffer);
    fclose(fin);

    PUTLOG("OCR result: %s", szBuffer);

    sText = szBuffer;
    return !sText.empty();
}

#ifdef USE_CURL
bool CCustIETools::HttpRequest(const std::string & url, std::string & response)
{
    CURL * curl = NULL;
    curl = curl_easy_init();
    if (!curl) 
    {
        PUTLOG("Failed to init curl handle");
        return false;
    }
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);

    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, CurlWriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    /* Perform the request, res will get the return code */
    CURLcode res = curl_easy_perform(curl);

    /* Check for errors */
    if (res != CURLE_OK) 
    {
        PUTLOG("curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        curl_easy_cleanup(curl);
        return false;
    }

    /* always cleanup */
    curl_easy_cleanup(curl);
    return res == CURLE_OK;
}

bool CCustIETools::HttpRequest(const std::string & url, const std::string & body, std::string & response)
{
    return false;
}

bool CCustIETools::HttpUpload(const std::string & url, const std::string & filePath, const std::string & htmltag, std::string & response)
{
    /*
    const wchar_t * formxml =
    _T("<form>")
    _T("<input type='file' name='imagelogo'></input>")
    _T("</form>");
    HttpPostForm(url, formxml, _T(""), response);
    */

    CURL *curl;
    CURLcode res;

    struct curl_httppost *formpost = NULL;
    struct curl_httppost *lastptr = NULL;
    struct curl_slist *headerlist = NULL;

    static const char buf[] = "Expect:";

    // imagelogo = {fileName: filePath};
    curl_formadd(&formpost,
        &lastptr,
        CURLFORM_COPYNAME, htmltag.c_str(),
        CURLFORM_FILE, (const char *)filePath.c_str(), //TODO: use utf8 encoding
        CURLFORM_FILENAME, filePath.substr(filePath.rfind("\\") + 1).c_str(),
        CURLFORM_END);

    curl = curl_easy_init();
    /* initalize custom header list (stating that Expect: 100-continue is not wanted */
    headerlist = curl_slist_append(headerlist, buf);
    if (curl) {
        /* what URL that receives this POST */
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

        /* only disable 100-continue header if explicitly requested */
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headerlist);

        curl_easy_setopt(curl, CURLOPT_HTTPPOST, formpost);

        /* send all data to this function  */
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, CurlWriteCallback);

        /* we pass our 'chunk' struct to the callback function */
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&response);

        /* Perform the request, res will get the return code */
        res = curl_easy_perform(curl);

        /* Check for errors */
        if (res != CURLE_OK) {
            PUTLOG("curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        }
        else {
            PUTLOG("url=[%s] [%lu] bytes retrieved\n", url.c_str(), response.length());
        }
        /* always cleanup */
        curl_easy_cleanup(curl);

        /* then cleanup the formpost chain */
        curl_formfree(formpost);

        /* free slist */
        curl_slist_free_all(headerlist);
    }

    return res == CURLE_OK;
}

bool CCustIETools::HttpPostJson(const std::string & url, const std::string & data, std::string & response)
{
    std::string resp;
    std::vector<std::string> vecHeader;
    vecHeader.push_back("content-type: application/json; charset=utf-8");
    vecHeader.push_back("accept: */*");
    return CCustIETools::HttpPost(url, vecHeader, data, resp);
}

bool CCustIETools::HttpPost(const std::string & url, const std::vector<std::string> & vecHeaders, const std::string & data, std::string & response)
{
    CURL * curl = NULL;
    curl = curl_easy_init();
    if (!curl) 
    {
        PUTLOG("Failed to init curl handle");
        return false;
    }
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);

    struct curl_slist *chunk = NULL;
    for(auto it = vecHeaders.begin(); it != vecHeaders.end(); ++it)
    {
        chunk = curl_slist_append(chunk, it->c_str());
    }
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);

    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, CurlWriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());

    /* Perform the request, res will get the return code */
    CURLcode res = curl_easy_perform(curl);

    /* Check for errors */
    if (res != CURLE_OK) 
    {
        PUTLOG("curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        curl_easy_cleanup(curl);
        curl_slist_free_all(chunk);
        return false;
    }

    /* always cleanup */
    curl_easy_cleanup(curl);
    curl_slist_free_all(chunk);
    return res == CURLE_OK;
}

#endif

size_t CCustIETools::CurlWriteCallback(void * content, size_t sz, size_t cnt, void * userp)
{
    if (!userp) 
    {
        PUTLOG("Invalid parameter for curl callback");
        return 0;
    }

    std::string * pStr = (std::string *)userp;

    pStr->append((char *)content, sz * cnt);

    return sz * cnt;
}

bool CCustIETools::RegSetString(HKEY hRoot, const std::string & keyPath, const std::string & keyName, const std::string & value)
{
    HKEY hKey = NULL;
    long ret = RegOpenKeyExA(hRoot, keyPath.c_str(), 0, REG_LEGAL_OPTION, &hKey);
    if (ret != ERROR_SUCCESS)
    {
        DWORD dwDisposition = 0;
        ret = RegCreateKeyExA(HKEY_LOCAL_MACHINE, keyPath.c_str(), 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hKey, &dwDisposition);
        if (ret != ERROR_SUCCESS)
        {
            return false;
        }
    }

    ret = RegSetValueExA(hKey, keyName.c_str(), NULL, REG_SZ, (BYTE*)value.c_str(), value.length());

    RegCloseKey(hKey);

    if (ret != ERROR_SUCCESS) 
    {
        return false;
    }
    return true;
}

bool CCustIETools::RegSetInt(HKEY hRoot, const std::string & keyPath, const std::string & keyName, unsigned int value)
{
    HKEY hKey = NULL;
    long ret = RegOpenKeyExA(hRoot, keyPath.c_str(), 0, REG_LEGAL_OPTION, &hKey);
    if (ret != ERROR_SUCCESS)
    {
        DWORD dwDisposition = 0;
        ret = RegCreateKeyExA(HKEY_LOCAL_MACHINE, keyPath.c_str(), 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hKey, &dwDisposition);
        if (ret != ERROR_SUCCESS)
        {
            return false;
        }
    }

    ret = RegSetValueExA(hKey, keyName.c_str(), NULL, REG_DWORD, (BYTE*)&value, sizeof(value));

    RegCloseKey(hKey);

    if (ret != ERROR_SUCCESS) 
    {
        return false;
    }
    return true;
}

std::string CCustIETools::MD5Sum(const std::string & filepath)
{
    FILE * fin = fopen(filepath.c_str(), "rb");
    if (!fin)
    {
        return "";
    }
    fseek(fin, 0, SEEK_END);
    int len = ftell(fin);
    if (len <= 0)
    {
        return "";
    }

    fseek(fin, 0, SEEK_SET);

    unsigned char * buff = (unsigned char *)malloc(len);

    int fini = 0;
    while(fini < len)
    {
        int rd = fread(buff + fini, 1, len - fini, fin);
        if (rd <= 0)
        {
            break;
        }
        fini += rd;
    }

    if(fini != len)
    {
        free(buff); buff = NULL;
        return "";
    }

    unsigned char sum [MD5_DIGEST_LENGTH + 1] = "";
    memset(sum, 0, sizeof(sum));
    MD5(buff, len, sum);

    return ToHexString(sum, MD5_DIGEST_LENGTH);
}

bool CCustIETools::Base64Encode(const std::vector<unsigned char > & vchInput, std::string & b64str)
{
    EVP_ENCODE_CTX	ctx;
    EVP_EncodeInit(&ctx);

    int destbuflen = vchInput.size() * 2 + 1; // enough

    char * destbuf = (char *)malloc(destbuflen);

    int outl = 0;
    EVP_EncodeUpdate(&ctx, (unsigned char *)destbuf, &outl, (unsigned char *)&vchInput[0], vchInput.size());

    int tmplen = 0;
    EVP_EncodeFinal(&ctx, (unsigned char *)&destbuf[outl], &tmplen);
    outl += tmplen;

    b64str.append(destbuf, outl);
    free(destbuf); destbuf = NULL;

    return !b64str.empty();
}

bool CCustIETools::Base64Decode(const std::string & b64str, std::vector<unsigned char > & vecResult)
{
    EVP_ENCODE_CTX	ctx;
    int srclen = (((b64str.length()+2)/4)*3)+1;
    int destlen = 0;

    vecResult.resize(srclen);
    unsigned char * destbuf = &vecResult[0];
    EVP_DecodeInit(&ctx);

    EVP_DecodeUpdate(&ctx, (unsigned char *)destbuf, &destlen, (unsigned char *)b64str.c_str(), b64str.length());

    int tmplen = 0;

    EVP_DecodeFinal(&ctx, (unsigned char *)&vecResult[destlen], &tmplen);
    destlen += tmplen;

    vecResult.resize(destlen);

    return !vecResult.empty();
}

bool CCustIETools::AESEncrypt(const std::string & text, const std::string & key, std::vector<unsigned char > & vecDest)
{
    EVP_CIPHER_CTX ctx;
    vecDest.resize(text.length() + 64);   // enough

    EVP_CIPHER_CTX_init(&ctx);

    int ret = EVP_EncryptInit_ex(&ctx, EVP_aes_128_ecb(), NULL, (const unsigned char*)key.c_str(), NULL);

    int outl = 0;
    ret = EVP_EncryptUpdate(&ctx, (unsigned char *)&vecDest[0], &outl, (const unsigned char*)text.c_str(), text.length());

    int	templen = 0;
    ret = EVP_EncryptFinal_ex(&ctx, (unsigned char *)&vecDest[outl], &templen); 
    outl += templen;

    vecDest.resize(outl);

    ret = EVP_CIPHER_CTX_cleanup(&ctx);

    return !vecDest.empty();
}

bool CCustIETools::AESDecrypt(const std::vector<unsigned char > & vecInput, const std::string & key, std::string & text)
{
    EVP_CIPHER_CTX ctx;
    std::vector<unsigned char> vecDest;
    vecDest.resize(vecInput.size() + 64);

    EVP_CIPHER_CTX_init(&ctx);

    int ret = EVP_DecryptInit_ex(&ctx, EVP_aes_128_ecb(), NULL, (const unsigned char*)key.c_str(), NULL);

    int textlen = 0;
    ret = EVP_DecryptUpdate(&ctx, (unsigned char *)&vecDest[0], &textlen, (const unsigned char*)&vecInput[0], vecInput.size());

    int	templen = 0;
    ret = EVP_DecryptFinal_ex(&ctx, (unsigned char *)&vecDest[textlen], &templen); 
    textlen += templen;

    vecDest.resize(textlen);

    ret = EVP_CIPHER_CTX_cleanup(&ctx);

    text.append((char *)&vecDest[0], textlen);

    return !text.empty();
}

std::string CCustIETools::GetUnProtectedString(const std::string & b64passphrase, const std::string & key)
{
    std::string text = "";

    std::vector<unsigned char > vecResult;
    if (!CCustIETools::Base64Decode(b64passphrase, vecResult))
    {
        return text;
    }

    CCustIETools::AESDecrypt(vecResult, key, text);

    return text;
}

std::string CCustIETools::SetProtectedString(const std::string & text, const std::string & key)
{
    std::string b64passphrase = "";

    // Encrypt
    std::vector<unsigned char > vchPassphrase;
    CCustIETools::AESEncrypt(text, key, vchPassphrase);

    // Encode
    CCustIETools::Base64Encode(vchPassphrase, b64passphrase);

    return b64passphrase;
}

bool CCustIETools::Exit(bool fForce)
{
    PUTLOG("* Script request to quit, %s", fForce ? "Force" : "Elegant");

    if (fForce)
    {
        ExitProcess(0);
    }
    else
    {
        AfxGetApp()->m_pMainWnd->PostMessage(WM_CLOSE, 0, 0);
    }
    return true;
}

void CCustIETools::string_replace(std::string& strBig, const std::string& strsrc, const std::string&strdst)
{
    std::string::size_type pos=0;
    std::string::size_type srclen=strsrc.size();
    std::string::size_type dstlen=strdst.size();
    while( (pos=strBig.find(strsrc, pos)) != std::string::npos)
    {
        strBig.replace(pos, srclen, strdst);
        pos += dstlen;
    }
}

static DWORD BinaryCode_GetAddrByModule(const char * lpModuleName, DWORD nModuleOffset)
{	
    HMODULE hModule = GetModuleHandleA(lpModuleName);
    DWORD nModuleAddress = (DWORD )hModule;
    if (0 != nModuleAddress)
    {
        DWORD nPhysical = nModuleAddress + nModuleOffset;
        return nPhysical;
    }
    return 0;
}

static BOOL Mem_ReadAddressByteSet(DWORD nBase, BYTE* byszValue, int nNum)
{
    DWORD dwOLD = 0;
    MEMORY_BASIC_INFORMATION  mbi = {0};
    VirtualQuery((LPCVOID )nBase, &mbi, sizeof(mbi));
    VirtualProtect((LPVOID )nBase, sizeof(BYTE) * nNum, PAGE_EXECUTE_READWRITE, &dwOLD);
    memcpy((void *)byszValue, (void *)nBase, nNum);
    VirtualProtect((LPVOID )nBase, sizeof(BYTE) * nNum, dwOLD,0);
    return TRUE;
}

static BOOL Mem_ModifyAddressByteSet(DWORD nBase, BYTE* byszValue, int nNum)
{
    DWORD dwOLD = 0;
    MEMORY_BASIC_INFORMATION  mbi = {0};
    VirtualQuery((LPCVOID )nBase, &mbi, sizeof(mbi));
    VirtualProtect((LPVOID )nBase, sizeof(BYTE) * nNum, PAGE_EXECUTE_READWRITE, &dwOLD);
    memcpy((void *)nBase, (void *)byszValue, nNum);
    VirtualProtect((LPVOID )nBase, sizeof(BYTE) * nNum, dwOLD,0);
    return TRUE;
}

static DWORD l_nHookABCSetFocusOnPsdCtrlAddress = 0;
static DWORD l_dwPasswordCtrlObjectAddr = 0;

void CCustIETools::HookInput_Abc(char cValue)
{
    if (0 == l_dwPasswordCtrlObjectAddr)
        return ;

    static DWORD nCall = BinaryCode_GetAddrByModule("POWERE~1.OCX", 0x23710);

    if (0 == nCall) 
    {
        return ;
    }

    DWORD dwValue = (DWORD )cValue;
    if (!isprint(cValue))
    {
        return ;
    }

    _asm 
    {
        pushad;
        pushfd;
        push 0;
        push dwValue;
        mov ecx, dword ptr ds:[l_dwPasswordCtrlObjectAddr];
        call nCall;
        popfd;
        popad;
    }

    return ;
}

void _cdecl TraceAndCollectABCSetFocusOnPsdCtrl(DWORD dwEcx)
{
    l_dwPasswordCtrlObjectAddr = dwEcx;
}

void __declspec(naked) CodeABCSetFocusOnPsdCtrl(void)
{
    _asm
    {
        mov ecx,dword ptr ds:[esi+0x234];

        pushad;
        pushfd;
        push ecx;
        call TraceAndCollectABCSetFocusOnPsdCtrl;
        add esp,4;
        popfd;
        popad;

        push l_nHookABCSetFocusOnPsdCtrlAddress;
        add dword ptr ss:[esp], 0x6;
        retn;
    }
}

void CCustIETools::Hook_Abc()
{
    // long JUMP instruction
    BYTE byValue[5] = {0xe9, 0x00, 0x00, 0x00, 0x00};

    static BOOL s_hooked = FALSE;

    if (s_hooked)
    {
        return ;
    }

    if (0 == l_nHookABCSetFocusOnPsdCtrlAddress) 
    {
        l_nHookABCSetFocusOnPsdCtrlAddress = BinaryCode_GetAddrByModule("POWERE~1.OCX", 0x1B037);
    }

    if (0 == l_nHookABCSetFocusOnPsdCtrlAddress) 
    {
        return ;
    }

    // the distance between original and detoured, (vector, e.g. jump back if minus, jump forward if postive);
    *(DWORD* )&byValue[1] = (DWORD )CodeABCSetFocusOnPsdCtrl - ((DWORD )l_nHookABCSetFocusOnPsdCtrlAddress + 5);

    if (!s_hooked) 
    {
        Mem_ModifyAddressByteSet(l_nHookABCSetFocusOnPsdCtrlAddress, byValue, sizeof(byValue));
        s_hooked = TRUE;
    }

    return ;
}

int CCustIETools::HandleAlerts(const std::string & jsonConfig, const std::string & alertMsg)
{
    std::string strParams;
    strParams += "if ('undefined' === typeof GlobalSettings){";
    strParams += "GlobalSettings = ";
    strParams += jsonConfig;
    strParams += "};";	
    strParams += "\r\n";

    do 
    {
        std::ifstream ifs(CCustIETools::GetConfigPath() + "statuscode.js");
        std::string str((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
        int z = str.length();
        strParams += str;
    }
    while(0);

    do 
    {
        std::ifstream ifs(CCustIETools::GetConfigPath() + "alert.js");
        std::string str((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
        int z = str.length();
        strParams += str;
    }
    while(0);

    do 
    {
        std::ifstream ifs(CCustIETools::GetScriptPath() + "handler.js");
        std::string str((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
        int z = str.length();
        strParams += str;
    }
    while(0);

    std::string sourceMsg = alertMsg;
    CCustIETools::string_replace(sourceMsg, "\r\n", "");
    CCustIETools::string_replace(sourceMsg, "\n", "");

    strParams += "\r\n";
    strParams += "DecodeError(\'";
    strParams += sourceMsg;
    strParams += "\');";
    strParams += "\r\n";

    //OutputDebugStringA(strParams.c_str());

    std::string sResult;
    if (!CCustIETools::CallScript(strParams, sResult))
    {
        return 0;
    }
    else
    {
        return atoi(sResult.c_str());
    }
}

bool CCustIETools::CallScript(const std::string & code, std::string & result)
{
    CLSID clsidTest;
    CLSIDFromProgID(L"MSScriptControl.ScriptControl", &clsidTest);

    CComPtr<IDispatch> spDispatch;
    CoCreateInstance(clsidTest, NULL, CLSCTX_INPROC, IID_IDispatch, (void **)&spDispatch);
    if (!spDispatch)
    {
        PUTLOG("Failed to Create script interface");
        return false;
    }

    // SetProperty(<Language>="JavaScript")
    do 
    {
        //* When GetIDsOfNames is called with more than one name
        //, the first name (rgszNames[0]) corresponds to the member name
        //, and subsequent names correspond to the names of the member's parameters.
        DISPID propidLang;
        OLECHAR * propnameLang = L"Language";
        if (FAILED(spDispatch->GetIDsOfNames(IID_NULL, &propnameLang, 1, LOCALE_USER_DEFAULT, &propidLang)))
        {
            PUTLOG("Failed to get <Language> attribute");
            return false;
        }

        DISPPARAMS dispParams = {NULL, NULL, 0, 0};
        VARIANT vpLang;
        VariantInit(&vpLang);
        vpLang.vt = VT_BSTR;
        vpLang.bstrVal = CComBSTR("JavaScript");
        dispParams.cArgs = 1;
        dispParams.rgvarg = &vpLang;
        dispParams.cNamedArgs = 1;
        DISPID dispidNamed = DISPID_PROPERTYPUT;
        dispParams.rgdispidNamedArgs = &dispidNamed;

        VARIANT vResult;
        VariantInit(&vResult);

        EXCEPINFO e;
        UINT argErr = 0;
        HRESULT hr = S_OK;
        if(hr = (spDispatch->Invoke(propidLang, IID_NULL, LOCALE_USER_DEFAULT, DISPATCH_PROPERTYPUT | DISPATCH_METHOD, &dispParams, &vResult, &e, &argErr)))
        {
            PUTLOG("Failed to invoke to setup <Language>");
            return false;
        }
        hr = hr;
    }
    while(0);


    // CallMethod("Eval('expression')");
    do 
    {
        //* When GetIDsOfNames is called with more than one name
        //, the first name (rgszNames[0]) corresponds to the member name
        //, and subsequent names correspond to the names of the member's parameters.
        DISPID propid;
        OLECHAR * propname = L"Eval";
        if (FAILED(spDispatch->GetIDsOfNames(IID_NULL, &propname, 1, LOCALE_USER_DEFAULT, &propid)))
        {
            PUTLOG("Failed to get <Eval> attribute");
            return 0;
        }

        DISPPARAMS dispParams = {NULL, NULL, 0, 0};
        VARIANT varg;
        VariantInit(&varg);
        varg.vt = VT_BSTR;
        CComBSTR bstrCode = TOUNICODE(code);
        varg.bstrVal = bstrCode;
        dispParams.cArgs = 1;
        dispParams.rgvarg = &varg;
        dispParams.cNamedArgs = 0; // it is not required while calling method.
        DISPID dispidNamed = DISPID_PROPERTYPUT;
        dispParams.rgdispidNamedArgs = &dispidNamed;

        VARIANT vResult;
        VariantInit(&vResult);

        EXCEPINFO e;
        UINT argErr = 0;
        HRESULT hr = S_OK;
        if(hr = (spDispatch->Invoke(propid, IID_NULL, LOCALE_USER_DEFAULT, DISPATCH_PROPERTYPUT | DISPATCH_METHOD, &dispParams, &vResult, &e, &argErr)))
        {
            _com_error err(hr);
            PUTLOG("Failed to <Eval>, error: %s", TOANSI(err.ErrorMessage()));
            return false;
        }
        if (vResult.vt == VT_BSTR)
        {
            result = TOANSI(vResult.bstrVal ? vResult.bstrVal : L"");
        }
        else if (vResult.vt == VT_I4 )
        {
            char szBuf [100] = "";
            itoa(vResult.intVal, szBuf, 10);
            result = szBuf;
        }
        else
        {
            PUTLOG("Unsupported return type.");
            return false;
        }
    }while(0);

    return true;
}

BOOL __stdcall CCustIETools::EnumChildProc(HWND hwnd, LPARAM lParam)
{
    char szClassName [100] = "";
    struct FindChildParam * fcParam = (struct FindChildParam *)lParam;
    GetClassNameA(hwnd, szClassName, sizeof(szClassName));
    if (!strcmp(szClassName, fcParam->className))
    {
        PUTLOG("* Found child window handle by [%s]", szClassName);
        fcParam->target = hwnd;
        return FALSE; // STOP
    }
    return TRUE ; // go on
}

HWND CCustIETools::FindChildWindow(HWND hParent, const std::string & className)
{
    struct FindChildParam fcparam;
    fcparam.target = NULL;
    fcparam.className = className.c_str();
    EnumChildWindows(hParent, CCustIETools::EnumChildProc, (LPARAM)&fcparam);
    return fcparam.target;
}

bool CCustIETools::SetElementFocus(const std::string & sElementTag)
{
    CMainFrame * pMainFrame = (CMainFrame *)(AfxGetApp()->m_pMainWnd);
    if (pMainFrame == NULL)
    {
        PUTLOG("* Failed to get parent window");
        return false;
    }

    HWND hChild = FindChildWindow(pMainFrame->m_hWnd, sElementTag);
    if (!hChild)
    {
        PUTLOG("* Failed to find child window by [%s]", sElementTag.c_str());
        return false;
    }

    if (!IsWindowVisible(hChild))
    {
        ShowWindow(hChild, SW_SHOW);
    }

    if (SetFocus(hChild) == NULL)
    {
        PUTLOG("* Failed to set focus on control, last error[%d]", GetLastError());
        return false;
    }

    PUTLOG("* OK to set focus, className=[%s]", sElementTag.c_str());
    return true;
}

DWORD __stdcall CCustIETools::ThreadDelayMessage(void * param)
{
    struct DelayMessage * dm = (struct DelayMessage *) param;
    if (dm->delayMs == 0)
    {
        PUTLOG("* Failed as of delay timeout is invalid");
        free(dm); dm = NULL;
        return 0;
    }

    CMainFrame * pMainFrame = (CMainFrame *)(AfxGetApp()->m_pMainWnd);
    if (pMainFrame == NULL)
    {
        PUTLOG("* Failed as of CMainFrame is NULL");
        free(dm); dm = NULL;
        return 0;
    }

    if (dm->msgId == 0)
    {
        PUTLOG("* Failed as of msg id is invalid");
        free(dm); dm = NULL;
        return 0;
    }

    PUTLOG("* Sleep for a while [%u]ms", dm->delayMs);
    ::Sleep(dm->delayMs);

    PUTLOG("* Post message to main frame, msg id[%u]", dm->msgId);
    pMainFrame->PostMessage(dm->msgId);

    free(dm); dm = NULL;

    return 0;
}

bool CCustIETools::DelayWindowMessage(unsigned int dwMsgId, unsigned int delayMills)
{
    struct DelayMessage * dm = (struct DelayMessage *)malloc(sizeof(struct DelayMessage));
    dm->delayMs = delayMills;
    dm->msgId = dwMsgId;
    DWORD dwThreadId = 0;
    HANDLE hDelayThread = CreateThread(NULL, 0, ThreadDelayMessage, dm, 0, &dwThreadId);
    return true;
}

bool CCustIETools::GetHttpOnlyCookie(const std::string & url, const std::string & cookieName, std::string & cookieData)
{
    //https://msdn.microsoft.com/en-us/library/windows/desktop/aa384714(v=vs.85).aspx
    //You can pass NULL for lpszCookieName to get all of the cookies for the specified URL. 
    //They are returned in a single long string with a semicolon and space between each pair, just like in an HTTP header.
    wchar_t wszCookieData [1024] = L"";
    DWORD dwCookieSize = sizeof(wszCookieData)/sizeof(wszCookieData[0]);
    BOOL fRet = InternetGetCookieExW(TOUNICODE(url.c_str())
        , cookieName.empty() ? NULL : TOUNICODE(cookieName)/*L"SPDB_PER_SESSIONID",*/ /*L"SPDB_NET_SESSIONID",*/
        , wszCookieData
        , &dwCookieSize
        , INTERNET_COOKIE_HTTPONLY
        , NULL);
    if (*wszCookieData && dwCookieSize)
    {
        cookieData = TOANSI(wszCookieData);
        PUTLOG("* Cookie [%s]", TOANSI(wszCookieData));
    }
    return (fRet == TRUE);
}

bool CCustIETools::StatusReport(const Json::Value & gset, unsigned int statusCode, const std::string & sJsonParams)
{
    //Ref: doc
    /*
    {
    "org":"cncb",
    "reqId":"zzzzz",
    "status":4010,
    "taskey":"TASK:xxxxxx",
    "voucherMap":{}
    }
    */
    Json::Value jsonReqBody;
    jsonReqBody["org"] = gset["type"].asString();
    jsonReqBody["reqId"] = gset["reqid"].asString();
    jsonReqBody["taskey"] = gset["taskkey"].asString();
    jsonReqBody["status"] = statusCode;

    Json::Value jsonParams;
    Json::Reader reader;
    if (!reader.parse(sJsonParams, jsonParams) || !jsonParams.isObject())
    {
        reader.parse("{}", jsonParams);
    }
    // C++ native params
    jsonParams["botid"] = (int)::GetCurrentProcessId();

    jsonReqBody["voucherMap"] = jsonParams;

    std::string sHost = gset["host"].asString();
    std::string sRequest = jsonReqBody.toStyledString();
    std::string sResp;

    PUTLOG("* [ReportStatus]\r\n" \
        " Target: [%s]\r\n" \
        " Info ==> %s\r\n"
        , sHost.c_str()
        , sRequest.c_str());

    if (!HttpPostJson(sHost, TOUTF8(TOUNICODE(sRequest)), sResp))
    {
        PUTLOG("* Failed notify status [%u] to Host[%s]", statusCode, sHost.c_str());
    }

    switch(statusCode)
    {
    case StatusCode(WAIT_VCODE):
        {
            PUTLOG("* LoginBot is working");
        }
        break;
    case StatusCode(TASK_WORKING):
        {
            PUTLOG("* waiting for phone code");
        }
        break;
    case StatusCode(OPEN_SUCCESS):
        {
            PUTLOG("* LoginBot created");
        }
        break;
    default:
        {
            // Quit LoginBot
            PUTLOG("* Quit LoginBot as StatusCode: [%u]", statusCode);
            Exit(true);
            //AfxGetApp()->m_pMainWnd->PostMessage(WM_CLOSE, 0, 0);
        }
        break;
    }
    return true;
}

bool CCustIETools::SaveGlobalSettings(const Json::Value & gset)
{
    DWORD dwProcId = GetCurrentProcessId();
    char szPrefix [100] = "";
    sprintf(szPrefix, "LoginBot-%u", dwProcId);

    // Get a unique temporary file to write
    std::string sTmpGSetFile = GetTempFilePath(szPrefix, "json", true);
    do 
    {
        std::ofstream fout;
        fout.open(sTmpGSetFile.c_str(), std::ios::out|std::ios::binary);
        std::string sGlobalSettings = gset.toStyledString();
        fout.write(sGlobalSettings.c_str(), sGlobalSettings.length());
        fout.close();
    }
    while(0);

    // Get target file path
    std::string sGSetFile = GetTempFilePath(szPrefix, "json", false);

    // Atomic, Rename temporary file to target path
    if (::MoveFileA(sTmpGSetFile.c_str(), sGSetFile.c_str()))
    {
        PUTLOG("* Ok to save new GlobalSetting to file [%s]", sGSetFile.c_str());
    }
    else
    {
        PUTLOG("* Failed to save GlobalSettings");
    }
    return true;
}

bool CCustIETools::LoadGlobalSettings(Json::Value & gset)
{
    DWORD dwProcId = GetCurrentProcessId();
    char szPrefix [100] = "";
    sprintf(szPrefix, "LoginBot-%u", dwProcId);

    // Check GlobalSetting cache exist
    std::string sGSetFile = GetTempFilePath(szPrefix, "json", false);
    struct _stat32 st;
    if (_stat32(sGSetFile.c_str(), &st))
    {
        //PUTLOG("* File not exist, [%s]", sGSetFile.c_str());
        return false;
    }

    // Atomic
    std::string sTmpGSetFile = GetTempFilePath(szPrefix, "json", true);
    if (!::MoveFileA(sGSetFile.c_str(), sTmpGSetFile.c_str())){
        PUTLOG("* Failed to rename GlobalSetting cache [%s]", sGSetFile.c_str());
        return false;
    }
    else
    {
        PUTLOG("* OK to rename GlobalSetting cache [%s]", sGSetFile.c_str());
    }

    // read out json string to parse it
    do 
    {
        std::ifstream ifs(sTmpGSetFile);
        std::string str((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
        int z = str.length();
        Json::Reader reader;
        if (!reader.parse(str, gset))
        {
            PUTLOG("* Failed parse GlobalSettings");
            return false;
        }
        else
        {
            PUTLOG("* Ok to parse out GlobalSettings");
            //::DeleteFileA(sTmpGSetFile.c_str());
            return true;
        }
    }
    while(0);

    return true;
}

bool CCustIETools::ActiveWindowByMouseEvent(HWND hWnd)
{
    RECT rect = {0, 0, 0, 0};
    ::GetWindowRect(hWnd, &rect);

    int x = (rect.left + rect.right)>>1;
    int y = (rect.top  + (GetSystemMetrics(SM_CYCAPTION)));

    enum 
    {
        MOUSEEVENT_MOVE = (0x0001),
        MOUSEEVENT_ABSOLUTE = (0x8000),
    };

    unsigned int nDesktopX = 800;
    unsigned int nDesktopY = 600;
    do 
    {
        nDesktopX = GetSystemMetrics(SM_CXSCREEN);
        nDesktopY = GetSystemMetrics(SM_CYSCREEN);
    } 
    while (0);

    /*
    * Something about !ABSOLUTE
    * 1) you move mouse 10mm on your desktop slowly;
    * 2) you move back mouse 10mm on your desktop fast;
    * 3) Does the mouse go back to its original position?
    * 4) that is because Acceleration take effects.
    * 5) Acceleration algorithm reference 3 parameters, threshold1, threshold2, and level
    * 6) relative distance is [0, threshold1) and level != 0, x2
    * 7) relative distance is [threshold1, threshold2), and level == 2, x4
    * 8) Done of explanation.
    */
    do 
    {
        POINT pt; memset(&pt, 0, sizeof(pt));
        GetCursorPos(&pt);
        PUTLOG("ClickWindowTitle: current postion: pt.x=%u; pt.y=%u\n", pt.x, pt.y);
    } 
    while (0);

    /*
    * About Parameters x & y
    * mouse move is between [0, 65535];
    * if you want to move by logical pixels of desktop, 
    * you need caculate map between (x/MaxXPixelsOfDesktop * 65535) or (y/MaxYPixelsOfDesktop * 65535)
    */
    mouse_event( MOUSEEVENT_MOVE | MOUSEEVENT_ABSOLUTE
        , static_cast<unsigned int>(1.0f * x / nDesktopX * 65535)
        , static_cast<unsigned int>(1.0f * y / nDesktopY * 65536)
        , 0, 0);

    do
    {
        POINT pt; memset(&pt, 0, sizeof(pt));
        GetCursorPos(&pt);
        PUTLOG("ClickWindowTitle: current position: pt.x=%u; pt.y=%u\n", pt.x, pt.y);
    } 
    while (0);

    mouse_event(MOUSEEVENTF_LEFTDOWN  | MOUSEEVENTF_LEFTUP | MOUSEEVENT_ABSOLUTE
        , 0 // ignored when click
        , 0 // ignored when click
        , 0, 0);

    return true;
}

bool CCustIETools::SuspendProc(DWORD dwProcId)
{
#if 0
    HANDLE hThreadSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
    THREADENTRY32 threadEntry; 
    threadEntry.dwSize = sizeof(THREADENTRY32);
    Thread32First(hThreadSnapshot, &threadEntry);
    do {
        if (threadEntry.th32OwnerProcessID == dwProcId)
        {
            HANDLE hThread = OpenThread(THREAD_ALL_ACCESS, FALSE, threadEntry.th32ThreadID);
            SuspendThread(hThread);
            CloseHandle(hThread);
        }
    } while (Thread32Next(hThreadSnapshot, &threadEntry));
    CloseHandle(hThreadSnapshot);
    return true;
#else
    typedef LONG (NTAPI *NtSuspendProcess)(IN HANDLE ProcessHandle);
    HANDLE processHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwProcId);
    NtSuspendProcess pfnNtSuspendProcess 
        = (NtSuspendProcess)GetProcAddress(GetModuleHandleA("ntdll"), "NtSuspendProcess");
    pfnNtSuspendProcess(processHandle);
    CloseHandle(processHandle);
    return true;
#endif
}

bool CCustIETools::ResumeProc(DWORD dwProcId)
{
#if 0
    HANDLE hThreadSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
    THREADENTRY32 threadEntry; 
    threadEntry.dwSize = sizeof(THREADENTRY32);
    Thread32First(hThreadSnapshot, &threadEntry);
    do 
    {
        if (threadEntry.th32OwnerProcessID == dwProcId)
        {
            HANDLE hThread = OpenThread(THREAD_ALL_ACCESS, FALSE, threadEntry.th32ThreadID);
            ResumeThread(hThread);
            CloseHandle(hThread);
        }
    } while (Thread32Next(hThreadSnapshot, &threadEntry));
    CloseHandle(hThreadSnapshot);
    return true;
#else
    typedef LONG (NTAPI * NtResumeProcess )( HANDLE ProcessHandle );
    HANDLE processHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwProcId);
    NtResumeProcess pfnNtResumeProcess 
        = (NtResumeProcess)GetProcAddress(GetModuleHandleA("ntdll"), "NtResumeProcess");
    pfnNtResumeProcess(processHandle);
    CloseHandle(processHandle);
    return true;
#endif
}

bool CCustIETools::SuspendProcByName(const std::string & procName, bool fSuspend)
{
    DWORD aProcesses[1024]; 
    DWORD cbNeeded; 
    DWORD cProcesses;
    unsigned int i;

    // Get the list of process identifiers.

    if ( !EnumProcesses( aProcesses, sizeof(aProcesses), &cbNeeded ) )
        return false;

    // Calculate how many process identifiers were returned.

    cProcesses = cbNeeded / sizeof(DWORD);

    // Print the names of the modules for each process.

    for ( i = 0; i < cProcesses; i++ )
    {
        DWORD dwProcId =  aProcesses[i];
        if (dwProcId == GetCurrentProcessId())
        {
            continue;
        }

        HANDLE processHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwProcId);

        if (processHandle == NULL)
        {
            continue;
        }
        char szProcName [MAX_PATH] = "";
        GetProcessImageFileNameA(processHandle, szProcName, MAX_PATH);
        CloseHandle(processHandle);

        char szBaseName [MAX_PATH] = "";
        const char * pBaseName = strrchr(szProcName, '\\');
        if (pBaseName != NULL)
        {
            strcpy(szBaseName, ++pBaseName);
        }
        PUTLOG("* Check process: %s\r\n", szBaseName);
        if (!stricmp(szBaseName, procName.c_str()))
        {
            PUTLOG("* %s process: %s, pid=%d\r\n", fSuspend ? "Suspend" : "Resume", szBaseName, dwProcId);
            if (fSuspend)
                SuspendProc(dwProcId);
            else
                ResumeProc(dwProcId);
        }
    }

    return true;
}

#ifdef USE_DETOURS
DWORD CCustIETools::CustIE_InternetSetCookieExW( LPCWSTR lpszUrl, LPCWSTR lpszCookieName, LPCWSTR lpszCookieData, DWORD dwFlags, DWORD_PTR dwReserved )
{
    if (lpszCookieName)
    {
        PUTLOG("* [%s]: CookieName: [%s]", TOANSI(__FUNCTIONW__), TOANSI(lpszCookieName));
    }
    if (lpszCookieData)
    {
        PUTLOG("* [%s]: Cookie: [%s]", TOANSI(__FUNCTIONW__), TOANSI(lpszCookieData));
    }
    return CCustIETools::Real_InternetSetCookieExW(lpszUrl, lpszCookieName, lpszCookieData, dwFlags, dwReserved);
}

BOOL CCustIETools::CustIE_InternetSetCookieW( LPCWSTR lpszUrl, LPCWSTR lpszCookieName, LPCWSTR lpszCookieData )
{
    return CCustIETools::Real_InternetSetCookieW(lpszUrl, lpszCookieName, lpszCookieData);
}

DWORD CCustIETools::CustIE_InternetSetCookieExA( LPCSTR lpszUrl, LPCSTR lpszCookieName, LPCSTR lpszCookieData, DWORD dwFlags, DWORD_PTR dwReserved )
{
    return CCustIETools::Real_InternetSetCookieExA(lpszUrl, lpszCookieName, lpszCookieData, dwFlags, dwReserved);
}

BOOL CCustIETools::CustIE_InternetSetCookieA( LPCSTR lpszUrl, LPCSTR lpszCookieName, LPCSTR lpszCookieData )
{
    return CCustIETools::Real_InternetSetCookieA(lpszUrl, lpszCookieName, lpszCookieData);
}

BOOL CCustIETools::CustIE_InternetGetCookieExW(LPCWSTR lpszUrl, LPCWSTR lpszCookieName, LPWSTR lpszCookieData, LPDWORD lpdwSize,  DWORD dwFlags,  LPVOID lpReserved )
{
    BOOL fRet = CCustIETools::Real_InternetGetCookieExW(lpszUrl, lpszCookieName, lpszCookieData, lpdwSize, dwFlags, lpReserved);
    if (lpszCookieName){
        PUTLOG("* CookieName: [%s]", TOANSI(lpszCookieName));
    }
    if (lpszCookieData != NULL && lpdwSize){
        PUTLOG("* Cookie: [%s]", TOANSI(lpszCookieData));
    }
    return fRet;
}

BOOL CCustIETools::CustIE_InternetGetCookieExA(LPCSTR lpszUrl, LPCSTR lpszCookieName, LPSTR lpszCookieData, LPDWORD lpdwSize,  DWORD dwFlags,  LPVOID lpReserved )
{
    return CCustIETools::Real_InternetGetCookieExA(lpszUrl, lpszCookieName, lpszCookieData, lpdwSize, dwFlags, lpReserved);
}
#endif
