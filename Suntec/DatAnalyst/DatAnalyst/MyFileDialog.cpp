#include "stdafx.h"
#include "MyFileDialog.h"
#include <DLGS.H>
#include <WINUSER.H>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(CMyFileDialog, CFileDialog)
WNDPROC CMyFileDialog::m_wndProc = NULL;

CMyFileDialog::CMyFileDialog(BOOL bOpenFileDialog, /* TRUE for FileOpen, FALSE for FileSaveAs */ 
                             LPCTSTR lpszDefExt /*= NULL*/, 
                             LPCTSTR lpszFileName /*= NULL*/, 
                             DWORD dwFlags /*= OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT*/, 
                             LPCTSTR lpszFilter /*= NULL*/, 
                             CWnd* pParentWnd /*= NULL*/, 
                             DWORD dwSize /*= 0*/)
: CFileDialog(bOpenFileDialog, lpszDefExt, lpszFileName, dwFlags,
              lpszFilter, pParentWnd, dwSize)
{
}

CMyFileDialog::~CMyFileDialog()
{
}


BEGIN_MESSAGE_MAP(CMyFileDialog, CFileDialog)
END_MESSAGE_MAP()

LRESULT CALLBACK WindowProcNew(HWND hwnd,UINT message, WPARAM wParam, LPARAM lParam)
{
    if (message ==  WM_COMMAND)
    {
        if(HIWORD(wParam) == BN_CLICKED)
        {
            if (LOWORD(wParam) == IDOK)
            {
                if (CMyFileDialog* pDlg = (CMyFileDialog*)CWnd::FromHandle(hwnd))
                {
                    pDlg->EndDialog(IDOK);
                    return NULL;
                }
            }
        }
    }
    return CallWindowProc(CMyFileDialog::m_wndProc, hwnd, message, wParam, lParam);
}

void CMyFileDialog::OnInitDone()
{
    CWnd* pParentWnd = GetParent();
    m_wndProc = (WNDPROC)SetWindowLong(pParentWnd->m_hWnd, GWL_WNDPROC, (long)WindowProcNew);
}

void CMyFileDialog::OnFolderChange()
{
    SetControlText(edt1, GetFolderPath());
    m_strFolderPath = GetFolderPath();
}

BOOL CMyFileDialog::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
    return CFileDialog::OnNotify(wParam, lParam, pResult);
}
