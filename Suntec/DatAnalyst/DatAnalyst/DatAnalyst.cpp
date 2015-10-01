// DatAnalyst.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "DatAnalyst.h"
#include "DatAnalystDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(CDatAnalystApp, CWinApp)
    ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()

CDatAnalystApp::CDatAnalystApp()
{
}

CDatAnalystApp theApp;

BOOL CDatAnalystApp::InitInstance()
{
    INITCOMMONCONTROLSEX InitCtrls;
    InitCtrls.dwSize = sizeof(InitCtrls);
    InitCtrls.dwICC = ICC_WIN95_CLASSES;
    InitCommonControlsEx(&InitCtrls);
    Gdiplus::GdiplusStartupInput gdiplusStartupInput;
    Gdiplus::GdiplusStartup(&m_gdiplusToken, &gdiplusStartupInput, NULL);

    CWinApp::InitInstance();
    AfxEnableControlContainer();
    SetRegistryKey(_T("Local AppWizard-Generated Applications"));

    CDatAnalystDlg dlg;
    m_pMainWnd = &dlg;
    INT_PTR nResponse = dlg.DoModal();
    if (nResponse == IDOK)
    {
    }
    else if (nResponse == IDCANCEL)
    {
    }
    return FALSE;
}
