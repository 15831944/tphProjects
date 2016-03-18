#pragma once

class CCustIETools
{
public:
    CCustIETools();

    virtual ~CCustIETools();

public:
    static std::string LastErrMsg(unsigned int dwLastErrorId);

    static std::string ToAnsiString(const std::wstring & wstr);

    static std::string ToUtf8String(const std::wstring & str);

    static std::wstring ToUnicodeString(const std::string & str);

    static std::string ToHexString(const unsigned char * buff, size_t len);

    static char * StrDup(const char * src);

    static std::string GetAppPath();

    static std::string GetConfigPath();

    static std::string GetScriptPath();

    static void PutLog(const char * format, ...);

    static bool LoadScript(CComPtr<IDispatch> & pHtmlDocDisp, const std::string & script, bool recursive);

    static std::string GetVCodeText(const std::string & imageId, CComPtr<IHTMLElement> spElement, int imgWidth, int imgHeight);

    static std::string QueryVCode(const std::string & sImageHash);

    static std::string QueryPhoneCode(const std::string & sTaskKey);

    static std::string GetVCodeTextByClip(const std::string & imageId, CComPtr<IHTMLElement2> spDoc2, CComPtr<IHTMLElement> spElement, int imgWidth, int imgHeight);

    static bool InitKeyInputLock();

    static bool DeinitKeyInputLock();

    static bool InitKeyInputNotify();

    static bool DeinitKeyInputNotify();

    static bool GetKeyInputLock();

    static bool ActiveWindow(/*bool fFocus*/);

    static bool KeyInput(unsigned int nInputInterval, const std::string & sValue, bool fWait);

    static bool IsKeyInputDone();

    static bool ReleaseKeyInputLock();

    static bool SetKeyInputDone();

    static bool GetEncoderClsid(const std::string & format, CLSID* pClsid);

    static std::string GetTempFilePath(const std::string & prefix, const std::string & ext, bool fUniq);

    static bool RegSetString(HKEY hRoot, const std::string & keyPath, const std::string & keyName, const std::string & value);

    static bool RegSetInt(HKEY hRoot, const std::string & keyPath, const std::string  & keyName, unsigned int nVal);

    static std::string MD5Sum(const std::string & filepath);

    static std::string GetUnProtectedString(const std::string & b64passphrase, const std::string & key);

    static std::string SetProtectedString(const std::string & text, const std::string & key);

    static bool Base64Decode(const std::string & b64str, std::vector<unsigned char > & vecResult);

    static bool Base64Encode(const std::vector<unsigned char > & vecResult, std::string & b64str);

    static bool AESDecrypt(const std::vector<unsigned char > & vecInput, const std::string & key, std::string & text);

    static bool AESEncrypt(const std::string & text, const std::string & key, std::vector<unsigned char > & vecInput);

    static int  HandleAlerts(const std::string & jsonConfig, const std::string & alertMsg);

    static void string_replace(std::string & strBig, const std::string & strsrc, const std::string &strdst);

    static void Hook_Abc();

    static void HookInput_Abc(char cValue);

#ifdef USE_CURL
    static bool HttpRequest(const std::string & url, std::string & response);

    static bool HttpRequest(const std::string & url, const std::string & body, std::string & response);

    static bool HttpUpload(const std::string & url, const std::string & filepath, const std::string & htmltag, std::string & response);

    static bool HttpPost(const std::string & url, const std::vector<std::string> & vecHeaders, const std::string & data, std::string & reponse);

    static bool HttpPostJson(const std::string & url, const std::string & data, std::string & response);

#endif

    static bool Exit(bool fForce);

    static bool SetElementFocus(const std::string & sElementTag);

    static bool DelayWindowMessage(unsigned int dwMsgId, unsigned int delayMills);

    static bool GetHttpOnlyCookie(const std::string & url, const std::string & cookieName, std::string & cookieData);

    static bool StatusReport(const Json::Value & gset, unsigned int statusCode, const std::string & sJsonParams);

    static bool SaveGlobalSettings(const Json::Value & gset);

    static bool LoadGlobalSettings(Json::Value & gset);

    static bool ActiveWindowByMouseEvent(HWND hWnd);

    static bool SuspendProcByName(const std::string & procName, bool fSuspend);

#ifdef USE_DETOURS
    static DWORD __stdcall CustIE_InternetSetCookieExW( LPCWSTR lpszUrl, LPCWSTR lpszCookieName, LPCWSTR lpszCookieData, DWORD dwFlags, DWORD_PTR dwReserved );

    static BOOL  __stdcall CustIE_InternetSetCookieW( LPCWSTR lpszUrl, LPCWSTR lpszCookieName, LPCWSTR lpszCookieData );

    static DWORD __stdcall CustIE_InternetSetCookieExA( LPCSTR lpszUrl, LPCSTR lpszCookieName, LPCSTR lpszCookieData, DWORD dwFlags, DWORD_PTR dwReserved );

    static BOOL  __stdcall CustIE_InternetSetCookieA( LPCSTR lpszUrl, LPCSTR lpszCookieName, LPCSTR lpszCookieData );

    static BOOL  __stdcall CustIE_InternetGetCookieExW(LPCWSTR lpszUrl, LPCWSTR lpszCookieName, LPWSTR lpszCookieData, LPDWORD lpdwSize,  DWORD dwFlags,  LPVOID lpReserved );

    static BOOL  __stdcall CustIE_InternetGetCookieExA(LPCSTR lpszUrl, LPCSTR lpszCookieName, LPSTR lpszCookieData, LPDWORD lpdwSize,  DWORD dwFlags,  LPVOID lpReserved );
#endif

protected:
    static CComPtr<IHTMLDocument2> CCustIETools::GetHtmlDocFromIFrameWindow(CComPtr<IHTMLWindow2> spWindow);

    static bool EvalScript(CComPtr<IHTMLDocument2> & spHtmlDoc, const std::string & code);

    static bool EvalScriptEx(CComPtr<IHTMLDocument2> spHtmlDoc, const std::string & script);

    static bool Fork(const std::string & app, const std::vector<std::string> & strParams, bool fWait, unsigned int * pRetCode);

    static bool RequestOcr(const std::string & bankid, const std::string & imagePath, std::string & sImageHash);

    static bool QueryOcrResult(const std::string & sImageHash, std::string & sText);

    static bool SaveImageFromClip(const std::string & filepath);

    static bool SuspendProc(DWORD dwProcId);

    static bool ResumeProc(DWORD dwProcId);

private:
    static bool CallScript(CComPtr<IDispatch> & pHtmlDocDisp, const std::string & function, const std::vector<std::string> & args, std::string & result);

    static bool CallScript(const std::string & code, std::string & result);

    static size_t CurlWriteCallback(void * content, size_t sz, size_t cnt, void * userp);

    static HWND FindChildWindow(HWND hParent, const std::string & className);

    struct FindChildParam
    {
        HWND target;
        const char * className;
    };

    static BOOL __stdcall EnumChildProc(HWND hwnd, LPARAM lParam);

    struct FormFillParam_t 
    {
        unsigned int nInputInterval;
        char * sValue; // free it in thread proc
        BOOL fWait;
    };

    static DWORD __stdcall ThreadFormFill(void * param);

    struct DelayMessage
    {
        unsigned int msgId;
        unsigned int delayMs;
    };

    static DWORD __stdcall ThreadDelayMessage(void * param);

private:
    static HANDLE m_hKeyInputLock;
    static HANDLE m_hEvtKeyInputDone;

public:
#ifdef USE_DETOURS
    static DWORD  (__stdcall * Real_InternetSetCookieExW)( LPCWSTR lpszUrl, LPCWSTR lpszCookieName, LPCWSTR lpszCookieData, DWORD dwFlags, DWORD_PTR dwReserved );
    static BOOL   (__stdcall * Real_InternetSetCookieW)( LPCWSTR lpszUrl, LPCWSTR lpszCookieName, LPCWSTR lpszCookieData );
    static DWORD  (__stdcall * Real_InternetSetCookieExA)( LPCSTR lpszUrl, LPCSTR lpszCookieName, LPCSTR lpszCookieData, DWORD dwFlags, DWORD_PTR dwReserved );
    static BOOL   (__stdcall * Real_InternetSetCookieA)( LPCSTR lpszUrl, LPCSTR lpszCookieName, LPCSTR lpszCookieData );
    static BOOL   (__stdcall * Real_InternetGetCookieExW)(LPCWSTR lpszUrl, LPCWSTR lpszCookieName, LPWSTR lpszCookieData, LPDWORD lpdwSize,  DWORD dwFlags,  LPVOID lpReserved );
    static BOOL   (__stdcall * Real_InternetGetCookieExA)(LPCSTR lpszUrl, LPCSTR lpszCookieName, LPSTR lpszCookieData, LPDWORD lpdwSize,  DWORD dwFlags,  LPVOID lpReserved );
#endif
};

#define PUTLOG       CCustIETools::PutLog
#define TOANSI(x)    (CCustIETools::ToAnsiString(x).c_str())
#define TOUTF8(x)    (CCustIETools::ToUtf8String(x).c_str())
#define TOUNICODE(x) (CCustIETools::ToUnicodeString(x).c_str())