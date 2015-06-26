#pragma once
#include "afxwin.h"
#include "UserShapeBarManager.h"

class CImportUserShapeBarDlg : public CDialog
{
	DECLARE_DYNAMIC(CImportUserShapeBarDlg)
public:
	CImportUserShapeBarDlg(CWnd* pParent = NULL);
	virtual ~CImportUserShapeBarDlg();

	enum { IDD = IDD_DIALOG_IMPORT_SHAPEBAR };

    typedef enum {TopLeft, TopRight, BottomLeft, BottomRight} LayoutRef;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnLoadZip();
	afx_msg void OnSelectLocation();
	virtual BOOL OnInitDialog();
	afx_msg void OnCancel();
	afx_msg void OnOk();
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);

    CUserShapeBarManager& GetUserBarMan() { return m_userShapeMan; }
    CString GetZipFileName() const { return m_strZipFileName; }

    void SetBarLocation(CString val) { m_strBarLocation = val; }

	CString GetShapeFileName() const { return m_strShapeFileName; }
	void SetShapeFileName(CString val) { m_strShapeFileName = val; }

    CString GetTempPath() const { return m_strTempPath; }
    void SetTempPath(CString val) { m_strTempPath = val; }

    static int FindFiles(CString pathName ,std::vector<CString>& vFiles);
private:
    void LayoutControl(CWnd* pCtrl, LayoutRef refTopLeft, LayoutRef refBottomRight, int cx, int cy);

protected:
    CEdit m_editZipFileName;
    CEdit m_editBarLocation;
    CEdit m_editBarName;

    CString m_strZipFileName;
	CString m_strBarLocation;
    CUserShapeBarManager m_userShapeMan;
private:
    int m_oldWindowWidth;
    int m_oldWindowHeight;

    // the file recorded exported bar's information
    CString m_strShapeFileName;

    // the temp file path for extract files before import
    CString m_strTempPath;
};
