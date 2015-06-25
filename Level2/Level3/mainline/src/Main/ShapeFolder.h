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
	CEdit m_shapeBarName;
	CEdit m_shapeBarLocation;
	afx_msg void OnLoadShapeBar();
	afx_msg void OnCancel();
	afx_msg void OnOk();

protected:
    Folder_Style m_style;
    CString shapeBarName;
    CString shapeBarLocation;
public:
    CShapeFolder::Folder_Style GetStyle() const { return m_style; }
    void SetStyle(CShapeFolder::Folder_Style style) { m_style = style; }

    CString GetShapeBarName() const { return shapeBarName; }
    void SetShapeBarName(CString strName) { shapeBarName = strName; }

    CString GetShapeBarLocation() const { return shapeBarLocation; }
    void SetShapeBarLocation(CString strLocation) { shapeBarLocation = strLocation; }
};
