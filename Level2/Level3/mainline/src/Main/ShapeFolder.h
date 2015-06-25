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
	CEdit m_editBarName;
	CEdit m_editBarLocation;
	afx_msg void OnLoadShapeBar();
	afx_msg void OnCancel();
	afx_msg void OnOk();

protected:
    Folder_Style m_style;
    CString m_shapeBarName;
    CString m_shapeBarLocation;
public:
    CShapeFolder::Folder_Style GetStyle() const { return m_style; }
    void SetStyle(CShapeFolder::Folder_Style style) { m_style = style; }

    CString GetShapeBarName() const { return m_shapeBarName; }
    void SetShapeBarName(CString strName) { m_shapeBarName = strName; }

    CString GetShapeBarLocation() const { return m_shapeBarLocation; }
    void SetShapeBarLocation(CString strLocation) { m_shapeBarLocation = strLocation; }
};
