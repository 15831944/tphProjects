
// CustIE.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "afxwinappex.h"
#include "afxdialogex.h"
#include "CustIE.h"
#include "MainFrm.h"

#include "CustIEDoc.h"
#include "DocHostHtmlView.h"
#include "CustIEView.h"

#include "CustDispatch.h"

#include "CustIETools.h"

// CCustIEApp

BEGIN_MESSAGE_MAP(CCustIEApp, CWinApp)
	ON_COMMAND(ID_APP_ABOUT, &CCustIEApp::OnAppAbout)
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, &CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, &CWinApp::OnFileOpen)
	ON_UPDATE_COMMAND_UI(ID_FILE_MRU_FILE1, &CCustIEApp::OnUpdateFileMruFile1)
END_MESSAGE_MAP()


// CCustIEApp construction

CCustIEApp::CCustIEApp()
	: m_pCustDisp(NULL)
	, m_fShowScriptError(false)
	, m_fSuspendFlag(false)
	, m_fMinimizeWindow(false)
	, m_nKeyInputWait(2000)
	, m_sKeyInputName("KeyInput.exe")
	, m_nTimeOut(DEFAULT_TIMEOUT_SECONDS)
{
	// TODO: replace application ID string below with unique ID string; recommended
	// format for string is CompanyName.ProductName.SubProduct.VersionInformation
	SetAppID(_T("CustIE.AppID.NoVersion"));

	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

CCustIEApp::~CCustIEApp() {
}

// The one and only CCustIEApp object

CCustIEApp theApp;

void SetIECoreVersion(CString exeName, long version)
{
	wchar_t* path = L"SOFTWARE\\Microsoft\\Internet Explorer\\MAIN\\FeatureControl\\FEATURE_BROWSER_EMULATION";
	wchar_t* valueName = exeName.GetBuffer();
	wchar_t err[1024];
	HKEY hKey;
	DWORD dwDisposition;
	long ret = RegOpenKeyEx(HKEY_LOCAL_MACHINE, path, 0, REG_LEGAL_OPTION, &hKey);
	if (ret != ERROR_SUCCESS)
	{
		FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM, NULL, ret, NULL, err, sizeof(err), NULL);
		ret = RegCreateKeyEx(HKEY_LOCAL_MACHINE, path, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hKey, &dwDisposition);
		if (ret != ERROR_SUCCESS)
		{
			FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM, NULL, ret, NULL, err, sizeof(err), NULL);
			return;
		}
	}

	ret = RegSetValueEx(hKey, valueName, NULL, REG_DWORD, (BYTE*)&version, sizeof(version));
	if (ret != ERROR_SUCCESS)
		return;
}

// CCustIEApp initialization

BOOL CCustIEApp::InitInstance()
{	
	/*
	* Setup locale to zh_CN.GBK
	*/
	wchar_t * curlocale = _wsetlocale(LC_ALL, L"Chinese_People's Republic of China.936");
	int err = GetLastError();
	curlocale = _wsetlocale(LC_ALL, NULL);
	PUTLOG("* Set locale to [%s]", TOANSI(curlocale));

	/*
	* Note: Remember to link with /nxcompat:no options, while building with vs versions above vs2008
	* because CMB bank control use HEAP SPACE( HeapAlloc & VirtualProctect) to hold 2 instructions ( mov & jmp) to jump to real window procedure.
	*
	*/
	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	if (!AfxSocketInit())
	{
		PUTLOG("IDP_SOCKETS_INIT_FAILED");
		return FALSE;
	}

	// Initialize OLE libraries
	if (!AfxOleInit())
	{
		PUTLOG("IDP_OLE_INIT_FAILED");
		return FALSE;
	}

	AfxEnableControlContainer();

	if (!LoadConfiguration()) {
		PUTLOG("Failed to load json configuration");
		return FALSE;
	}

	do{
		/*
		* Initialize openssl
		*
		*/
		OpenSSL_add_all_algorithms();
		SSL_load_error_strings();
		ERR_load_crypto_strings();

		/**
		* Sample test for decryption of protected username/password
		* std::string text = CCustIETools::GetUnProtectedString("oTpTjQvEGG5OX5L99PjBmA==");
		*/

		/*
		* Enable debug Privilege
		*/
		EnableDebugPrivilege();

		/*
		* Initialize KeyInput lock
		*
		*/
		CCustIETools::InitKeyInputLock();

		/*
		* Init <Done> event of KeyInput
		*/
		CCustIETools::InitKeyInputNotify();

		/*
		* GDIPlus Init
		*/
		GdiplusStartupInput gdiplusStartupInput;
		GdiplusStartup(&m_gdiplusToken, &gdiplusStartupInput, NULL);

		#ifdef USE_CURL
		/*
		* Init Curl global
		*/
		curl_global_init(CURL_GLOBAL_DEFAULT);
		#endif

		/*
		* Disable script debugging
		* If you find any script error, change it to 'no' to debug. 
		*/
		CCustIETools::RegSetString(HKEY_CURRENT_USER
								, "Software\\Microsoft\\Internet Explorer\\Main"
								, "Disable Script Debugger"
								, ShowScriptError() ? "no" : "yes");

		/*
		* More Internet settings
		* // https://support.microsoft.com/en-us/kb/182569
		1406     Miscellaneous: Access data sources across domains
		1407     Scripting: Allow Programmatic clipboard access
		1408     Reserved #
		1409     Scripting: Enable XSS Filter
		1601     Miscellaneous: Submit non-encrypted form data
		1604     Downloads: Font download
		1605     Run Java #
		1606     Miscellaneous: Userdata persistence ^
		1607     Miscellaneous: Navigate sub-frames across different domains
		1608     Miscellaneous: Allow META REFRESH * ^
		1609     Miscellaneous: Display mixed content *
		*/
		CCustIETools::RegSetInt(HKEY_CURRENT_USER, "Software\\Microsoft\\Windows\\CurrentVersion\\Internet Settings\\Zones\\3", "1407", 0);
		CCustIETools::RegSetInt(HKEY_CURRENT_USER, "Software\\Microsoft\\Windows\\CurrentVersion\\Internet Settings\\Zones\\3", "1607", 0);
		CCustIETools::RegSetInt(HKEY_CURRENT_USER, "Software\\Microsoft\\Windows\\CurrentVersion\\Internet Settings\\Zones\\3", "1609", 0);

	
		/*
		* Disable MRU list
		*/
		LoadStdProfileSettings(0);

		/*
		* Detours
		*/
#ifdef USE_DETOURS
		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());
		DetourAttach(&(PVOID&)CCustIETools::Real_InternetSetCookieExW, CCustIETools::CustIE_InternetSetCookieExW);
		DetourAttach(&(PVOID&)CCustIETools::Real_InternetSetCookieW, CCustIETools::CustIE_InternetSetCookieW);
		DetourAttach(&(PVOID&)CCustIETools::Real_InternetSetCookieExA, CCustIETools::CustIE_InternetSetCookieExA);
		DetourAttach(&(PVOID&)CCustIETools::Real_InternetSetCookieA, CCustIETools::CustIE_InternetSetCookieA);
		DetourAttach(&(PVOID&)CCustIETools::Real_InternetGetCookieExW, CCustIETools::CustIE_InternetGetCookieExW);
		DetourAttach(&(PVOID&)CCustIETools::Real_InternetGetCookieExA, CCustIETools::CustIE_InternetGetCookieExA);
		LONG error = DetourTransactionCommit();
#endif
		
		/*
		* Unit test
		*/


	}while(0);

	EnableTaskbarInteraction(FALSE);

	DWORD dwTimeout = -1;  
	SystemParametersInfo(SPI_GETFOREGROUNDLOCKTIMEOUT, 0, (LPVOID)&dwTimeout, 0);  
	if (dwTimeout < 200000) {  // Lock for 2 minutes
		SystemParametersInfo(SPI_SETFOREGROUNDLOCKTIMEOUT, 200000, (LPVOID)0, SPIF_SENDCHANGE | SPIF_UPDATEINIFILE);  
	}

	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views
	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CCustIEDoc),
		RUNTIME_CLASS(CMainFrame),       // main SDI frame window
		RUNTIME_CLASS(CCustIEView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);

	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	// Ignore MFC-style parameters.
	//ParseCommandLine(cmdInfo);

	if (GetCommandLineW() == NULL) {
		Json::Value voucherMap;
		voucherMap["errmsg"] = "Invalid command line";
		CCustIETools::StatusReport(m_userConfig, StatusCode(OPEN_FAILE), voucherMap.toStyledString());
		return FALSE;
	}

	int argc = 0;
	LPWSTR * argv = CommandLineToArgvW(GetCommandLineW(), &argc);
	if (!argc || !ParseArgs(argc, argv)) {
		LocalFree(argv);
		Json::Value voucherMap;
		voucherMap["errmsg"] = "Failed to parse arguments";
		CCustIETools::StatusReport(m_userConfig, StatusCode(OPEN_FAILE), voucherMap.toStyledString());
		return FALSE;
	}
	LocalFree(argv);

	// Dispatch commands specified on the command line.  Will return FALSE if
	// app was launched with /RegServer, /Register, /Unregserver or /Unregister.
	if (!ProcessShellCommand(cmdInfo)){
		Json::Value voucherMap;
		voucherMap["errmsg"] = "Failed to ProcessShellCommand";
		CCustIETools::StatusReport(m_userConfig, StatusCode(OPEN_FAILE), voucherMap.toStyledString());
		return FALSE;
	}

	// The one and only window has been initialized, so show and update it
	m_pMainWnd->ShowWindow(SW_MINIMIZE);
	m_pMainWnd->UpdateWindow();

	if (FALSE){
		m_pMainWnd->SetWindowPos(NULL, GetSystemMetrics(SM_CXSCREEN)>>2, 0, 0, 0, SWP_NOSIZE|SWP_NOACTIVATE|SWP_NOZORDER);
	}

	Json::Value voucherMap;
	voucherMap["errmsg"] = "InitInstance OK";
	CCustIETools::StatusReport(m_userConfig, StatusCode(OPEN_SUCCESS), voucherMap.toStyledString());
	return TRUE;
}

int CCustIEApp::ExitInstance()
{
	// Unload Openssl
	ERR_free_strings();
	CRYPTO_cleanup_all_ex_data();
	EVP_cleanup();

	//TODO: handle additional resources you may have added
	AfxOleTerm(FALSE);

	if (m_pCustDisp) {
		delete m_pCustDisp;
		m_pCustDisp = NULL;
	}

#ifdef USE_CURL
	curl_global_cleanup();
#endif

#ifdef USE_DETOURS
	/* Detours helper */
	{
		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());
		DetourDetach(&(PVOID&)CCustIETools::Real_InternetSetCookieExW, CCustIETools::CustIE_InternetSetCookieExW);
		DetourDetach(&(PVOID&)CCustIETools::Real_InternetSetCookieW, CCustIETools::CustIE_InternetSetCookieW);
		DetourDetach(&(PVOID&)CCustIETools::Real_InternetSetCookieExA, CCustIETools::CustIE_InternetSetCookieExA);
		DetourDetach(&(PVOID&)CCustIETools::Real_InternetSetCookieA, CCustIETools::CustIE_InternetSetCookieA);
		DetourDetach(&(PVOID&)CCustIETools::Real_InternetGetCookieExW, CCustIETools::CustIE_InternetGetCookieExW);
		DetourDetach(&(PVOID&)CCustIETools::Real_InternetGetCookieExA, CCustIETools::CustIE_InternetGetCookieExA);
		DetourTransactionCommit();
	}
#endif

	CCustIETools::DeinitKeyInputNotify();
	
	CCustIETools::DeinitKeyInputLock();

	GdiplusShutdown(m_gdiplusToken);

	return CWinApp::ExitInstance();
}

// CCustIEApp message handlers


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()

// App command to run the dialog
void CCustIEApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

// CCustIEApp message handlers

IDispatch * CCustIEApp::GetExternalDispatch()
{
	if (!m_pCustDisp) {
		m_pCustDisp = new CImpIDispatch();
	}
	m_pCustDisp->AddRef();
	return m_pCustDisp;
}

BOOL CCustIEApp::EnableDebugPrivilege(/*LPCWSTR szPrivName*/)
{
	HANDLE _hToken = INVALID_HANDLE_VALUE;
	if (!OpenProcessToken(GetCurrentProcess(),
		TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY,
		&_hToken))
	{
		return FALSE;
	}
	if (_hToken == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}
	TOKEN_PRIVILEGES tp = { 0 };
	LUID luid = { 0 };
	TOKEN_PRIVILEGES tpPrevious = { 0 };
	DWORD cbPrevious = sizeof(TOKEN_PRIVILEGES);
	if (!LookupPrivilegeValueW(NULL, L"SeDebugPrivilege", &luid))
	{
		return FALSE;
	}
	//
	// first pass.  get current privilege setting
	//
	tp.PrivilegeCount = 1;
	tp.Privileges[0].Luid = luid;
	tp.Privileges[0].Attributes = 0;
	if (!AdjustTokenPrivileges(_hToken,
		FALSE,
		&tp,
		sizeof(TOKEN_PRIVILEGES),
		&tpPrevious,
		&cbPrevious))
	{
		return FALSE;
	}
	//
	// second pass.  set privilege based on previous setting
	//
	tpPrevious.PrivilegeCount = 1;
	tpPrevious.Privileges[0].Luid = luid;
	tpPrevious.Privileges[0].Attributes |= (SE_PRIVILEGE_ENABLED);
	if (!AdjustTokenPrivileges(_hToken,
		FALSE,
		&tpPrevious,
		cbPrevious,
		NULL,
		NULL))
	{
		return FALSE;
	}
	if (_hToken != INVALID_HANDLE_VALUE)
	{
		CloseHandle(_hToken); _hToken = INVALID_HANDLE_VALUE;
	}
	return TRUE;
}

#pragma warning(disable:4996)
bool CCustIEApp::ParseArgs(int argc, LPWSTR * argv)
{
	for (int i = 0; i < argc; i++) {
		//PUTLOG("[%d]: [%s]\n", i, TOANSI(argv[i]));
		std::string optarg = TOANSI(argv[i]);

		if (!stricmp(optarg.c_str(), "-u") || !stricmp(optarg.c_str(), "--user")) {
			if (i + 1 <= argc) {
				m_userConfig["user"] = CCustIETools::GetUnProtectedString( TOANSI(argv[i + 1]), GetKey());
			}
			else {
				PUTLOG("Invalid arguments!");
				return false;
			}
		}

		if (!stricmp(optarg.c_str(), "-p") || !stricmp(optarg.c_str(), "--pass")) {
			if (i + 1 <= argc) {
				m_userConfig["pass"] = CCustIETools::GetUnProtectedString(TOANSI(argv[i + 1]), GetKey());
			}
			else {
				PUTLOG("Invalid arguments!");
				return false;
			}
		}

		if (!stricmp(optarg.c_str(), "-t") || !stricmp(optarg.c_str(), "--type")) {
			if (i + 1 <= argc) {
				m_userConfig["type"] = TOANSI(argv[i + 1]);
			}
			else {
				PUTLOG("Invalid arguments!");
				return false;
			}
		}

		if (!stricmp(optarg.c_str(), "-s") || !stricmp(optarg.c_str(), "--subtype")) {
			if (i + 1 <= argc) {
				m_userConfig["subtype"] = TOANSI(argv[i + 1]);
			}
			else {
				PUTLOG("Invalid arguments!");
				return false;
			}
		}

		if (!stricmp(optarg.c_str(), "-l") || !stricmp(optarg.c_str(), "--logintype")) {
			if (i + 1 <= argc) {
				m_userConfig["logintype"] = TOANSI(argv[i + 1]);
			}
			else {
				PUTLOG("Invalid arguments!");
				return false;
			}
		}

		if (!stricmp(optarg.c_str(), "-r") || !stricmp(optarg.c_str(), "--reqid")) {
			if (i + 1 <= argc) {
				m_userConfig["reqid"] = TOANSI(argv[i + 1]);
			}
			else {
				PUTLOG("Invalid arguments!");
				return false;
			}
		}

		if (!stricmp(optarg.c_str(), "-k") || !stricmp(optarg.c_str(), "--taskkey")) {
			if (i + 1 <= argc) {
				m_userConfig["taskkey"] = TOANSI(argv[i + 1]);
			}
			else {
				PUTLOG("Invalid arguments!");
				return false;
			}
		}

		if (!stricmp(optarg.c_str(), "-h") || !stricmp(optarg.c_str(), "--host")) {
			if (i + 1 <= argc) {
				m_userConfig["host"] = TOANSI(argv[i + 1]);
			}
			else {
				PUTLOG("Invalid arguments!");
				return false;
			}
		}
	}
	return true;
}

bool CCustIEApp::LoadConfiguration()
{
	do {
		std::string strConfPath = CCustIETools::GetConfigPath();
		strConfPath += "\\";
		strConfPath += "loginbot.json";

		std::ifstream ifs(strConfPath);
		std::string str((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
		int z = str.length();
		//PUTLOG("Load configuration from file[%s], length[%d]", strConfPath.c_str(), z);
		Json::Reader reader;
		if (!reader.parse(str, m_sysConfig)){
			PUTLOG("* Failed to load system configuration");
			return false;
		}
	}while(0);

	do{
		std::string sAppPath = CCustIETools::GetAppPath();
		sAppPath += "LoginBot.ini";
		char szKey [MAX_PATH] = "";
		GetPrivateProfileStringA("default", "key", "", szKey, sizeof(szKey), sAppPath.c_str());
		m_sKey = szKey;
	}while(0);

	do{
		std::string sAppPath = CCustIETools::GetAppPath();
		sAppPath += "LoginBot.ini";
		char szKey [MAX_PATH] = "";
		GetPrivateProfileStringA("default", "KeyInput", "KeyInput.exe", szKey, sizeof(szKey), sAppPath.c_str());
		m_sKeyInputName = szKey;
	}while(0);

	do{
		std::string sAppPath = CCustIETools::GetAppPath();
		sAppPath += "LoginBot.ini";
		int configInt = GetPrivateProfileIntA("default", "MinimizeWindow", 0, sAppPath.c_str());
		m_fMinimizeWindow = (configInt == 1);
	}while(0);

	do{
		std::string sAppPath = CCustIETools::GetAppPath();
		sAppPath += "LoginBot.ini";
		int configInt = GetPrivateProfileIntA("default", "KeyInputWait", 2000, sAppPath.c_str());
		m_nKeyInputWait = configInt;
	}while(0);

	do{
		std::string sAppPath = CCustIETools::GetAppPath();
		sAppPath += "LoginBot.ini";
		int configInt = GetPrivateProfileIntA("default", "ShowScriptError", 0, sAppPath.c_str());
		m_fShowScriptError = (configInt == 1);
	}while(0);

	do{
		std::string sAppPath = CCustIETools::GetAppPath();
		sAppPath += "LoginBot.ini";
		int configInt = GetPrivateProfileIntA("default", "SuspendFlag", 0, sAppPath.c_str());
		m_fSuspendFlag = (configInt == 1);
	}while(0);

	do{
		std::string sAppPath = CCustIETools::GetAppPath();
		sAppPath += "LoginBot.ini";
		int configInt = GetPrivateProfileIntA("default", "TimeOut", DEFAULT_TIMEOUT_SECONDS, sAppPath.c_str());
		m_nTimeOut = (configInt);
	}while(0);

	return true;
}

std::string CCustIEApp::GetUrlSetting(const std::string & bankid, const std::string & type, const std::string & subType)
{
	std::string sUrl;
	if (!m_sysConfig.isObject()){
		PUTLOG("* System configuration is empty");
		return sUrl;
	}
	if (!m_sysConfig.isMember(bankid)){
		PUTLOG("* No entry for [%s] in system configuration", bankid.c_str());
		return sUrl;
	}
	Json::Value jsonBank = m_sysConfig[bankid];
	if (!jsonBank.isObject()){
		PUTLOG("* bank entry is invalid");
		return sUrl;
	}
	if (!jsonBank.isMember(type)){
		PUTLOG("* No entry for type[%s] for bank[%s]", type.c_str(), bankid.c_str());
		return sUrl;
	}
	Json::Value jsonType = jsonBank[type];
	if (!jsonType.isObject()){
		PUTLOG("* Type is invalid");
		return sUrl;
	}
	if (!jsonType.isMember(subType)){
		PUTLOG("* No subtype [%s] in %s.%s, try to use [default]", subType.c_str(), bankid.c_str(), type.c_str());
		if (jsonType.isMember("default")){
			PUTLOG("* use default entry");
			sUrl = jsonType["default"].asString();
		}else{
			PUTLOG("* Failed to get config");
		}
	}else{
		sUrl = jsonType[subType].asString();
	}
	
	PUTLOG("* Get url:[%s]", sUrl.c_str());
	return sUrl;
}

bool CCustIEApp::IsLoginUrl(const std::string & bankid, const std::string & url)
{
	return false;
}

bool CCustIEApp::IsPhoneUrl(const std::string & bankid, const std::string & url)
{
	return false;
}

bool CCustIEApp::IsErrorUrl(const std::string & bankid, const std::string & url)
{
	return false;
}

bool CCustIEApp::IsMainUrl(const std::string & bankid, const std::string & url)
{
	return false;
}

void CCustIEApp::AddToRecentFileList(LPCTSTR lpszPathName)
{
	//CWinApp::AddToRecentFileList(lpszPathName);
}


void CCustIEApp::OnUpdateFileMruFile1(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(FALSE);
}

std::string CCustIEApp::GetKey() const{
	return m_sKey;
}

bool CCustIEApp::ShowScriptError() const{
	return m_fShowScriptError;
}

unsigned int CCustIEApp::GetDefaultTimeout() const{
	return m_nTimeOut;
}

bool CCustIEApp::MinimizeWindow() const{
	return m_fMinimizeWindow;
}

unsigned int CCustIEApp::GetKeyInputWait() const{
	return m_nKeyInputWait;
}

std::string CCustIEApp::GetKeyInputName() const{
	return m_sKeyInputName;
}

bool CCustIEApp::SuspendFlag() const{
	return m_fSuspendFlag;
}