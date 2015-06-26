#pragma once

#include "ShapesListBox.h"
#include "scbarg.h"
#include "XPStyle\GfxOutBarCtrl.h"
#include "UserShapeBarManager.h"

class CShapesBar : public CSizingControlBarG  
{
// Construction
public:
	//CShapesBar();
	//virtual ~CShapesBar();

// Attributes
public:
protected:
	CBrush m_brush;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CProjectSelectBar)
	//}}AFX_VIRTUAL

// Implementation
public:
    void CreateOutlookBar(CShape::CShapeList* pSL);
    void AddUserShapesToShapesBar();
	CString m_strProjPath;

    CUserShapeBarManager* GetUserBarManager() const { return m_pUserBarMan; }
    void SetUserBarManager(CUserShapeBarManager* val) { m_pUserBarMan = val; }
	// Generated message map functions
protected:
	CFont m_font;
	CGfxOutBarCtrl m_wndOutBarCtrl;
	CImageList m_smallIL;
	CImageList m_largeIL;
	int m_iSelFolder;
	int m_iSelItem;
	int defBMP;
    CUserShapeBarManager* m_pUserBarMan;
    //{{AFX_MSG(CShapesBar)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg LRESULT OnSLBSelChanged(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnPopMenu(WPARAM wParam, LPARAM lParam);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);

	afx_msg void OnNewShapeBar();
    afx_msg void OnEditShapeBar();
	afx_msg void OnImport();
	afx_msg void OnExport();
	afx_msg void OnDeleteShapeBar();

	afx_msg void OnNewShape();
	afx_msg void OnEditShape();
	afx_msg void OnDeleteShape();
	//}}AFX_MSG
    BOOL ZipFiles(const CString& strFilePath, const std::vector<CString>& vZipFiles);
	DECLARE_MESSAGE_MAP()
private:
    // the file recorded exported bar's information
    CString m_strShapeFileName;

    // the temp file path for extract files before import
    CString m_strTempPath;
};

