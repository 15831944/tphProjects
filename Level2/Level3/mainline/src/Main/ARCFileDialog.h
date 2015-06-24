#pragma once


// ARCFileDialog dialog

class ARCFileDialog : public CDialog
{
	DECLARE_DYNAMIC(ARCFileDialog)

public:
	ARCFileDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~ARCFileDialog();

	ARCFileDialog(BOOL bOpenFileDialog, // TRUE for FileOpen, FALSE for FileSaveAs
		LPCTSTR lpszDefExt = NULL,
		LPCTSTR lpszFileName = NULL,
		DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		LPCTSTR lpszFilter = NULL,
		CWnd* pParentWnd = NULL,
		DWORD dwSize = 0,
		BOOL bVistaStyle = TRUE);

// Dialog Data
	enum { IDD = IDD_DIALOG24 };

protected:
		BOOL m_bOpenFileDialog; // TRUE for FileOpen, FALSE for FileSaveAs
		LPCTSTR m_lpszDefExt;
		LPCTSTR m_lpszFileName;
		DWORD m_dwFlags;
		LPCTSTR m_lpszFilter;
		CWnd* m_pParentWnd;
		DWORD m_dwSize;
		BOOL m_bVistaStyle;


		CString m_strFileName;
public:
	INT_PTR ShowDialog();
	CString GetPathName() const;


protected:
	virtual INT_PTR DoModal();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
};
