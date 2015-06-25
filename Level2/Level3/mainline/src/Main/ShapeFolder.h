#pragma once
#include "afxwin.h"

// CShapeFolder dialog

class CShapeFolder : public CDialog
{
	DECLARE_DYNAMIC(CShapeFolder)
	enum Folder_Style{ NEW = 0, NAME, PATH};
public:
	CShapeFolder(CWnd* pParent = NULL);   // standard constructor
	CShapeFolder(CString name, CString path,Folder_Style style = NEW,CWnd* pParent = NULL);
	virtual ~CShapeFolder();

// Dialog Data
	enum { IDD = IDD_SHAPEFOLDER };
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	CEdit m_folderName;
	CEdit m_folderPath;
	CString folderName;
	CString folderPath;
    static int folder_id;
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedOk();

protected:
    Folder_Style m_style;

public:
    CShapeFolder::Folder_Style GetStyle() const { return m_style; }
    void SetStyle(CShapeFolder::Folder_Style style) { m_style = style; }
};
