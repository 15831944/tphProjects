#pragma once

class CMyFileDialog : public CFileDialog
{
    DECLARE_DYNAMIC(CMyFileDialog)

public:
    static WNDPROC m_wndProc;
    virtual void OnInitDone();
    virtual void OnFolderChange();
    virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
    explicit CMyFileDialog(BOOL bOpenFileDialog, // TRUE for FileOpen, FALSE for FileSaveAs
        LPCTSTR lpszDefExt = NULL,
        LPCTSTR lpszFileName = NULL,
        DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
        LPCTSTR lpszFilter = NULL,
        CWnd* pParentWnd = NULL,
        DWORD dwSize = 0);
    virtual ~CMyFileDialog();

protected:
    DECLARE_MESSAGE_MAP()
public:
    CString m_strFolderPath;
};
