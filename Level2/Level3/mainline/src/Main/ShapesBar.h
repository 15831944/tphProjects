// ShapesBar.h: interface for the CShapesBar class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SHAPESBAR_H__B592C191_B7B4_4123_BD3B_9E5D858B5DB9__INCLUDED_)
#define AFX_SHAPESBAR_H__B592C191_B7B4_4123_BD3B_9E5D858B5DB9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ShapesListBox.h"
#include "scbarg.h"
#include "XPStyle\GfxOutBarCtrl.h"


class CUserShapeBar
{
public:
    CUserShapeBar();
    ~CUserShapeBar();
public:
    CString GetBarName() const { return m_barName; }
    void SetBarName(CString val) { m_barName = val; }

    CString GetBarLocation() const { return m_barLocation; }
    void SetBarLocation(CString val) { m_barLocation = val; }

    CShape::CShapeList* GetUserShapeList() { return &m_vUserShapes; }
    int GetShapeCount(){ return (int)m_vUserShapes.size(); }
    CShape* GetShapeByIndex(int nIdx){ return m_vUserShapes.at(nIdx); }

    void AddShape(CShape* pShape){ m_vUserShapes.push_back(pShape); }

    CUserShapeBar& operator=(const CUserShapeBar& other)
    {
        m_barName = other.m_barName;
        m_barLocation = other.m_barLocation;
        m_vUserShapes = other.m_vUserShapes;
    }

private:
    CString m_barName;
    CString m_barLocation;
    CShape::CShapeList m_vUserShapes;
};

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
	BOOL ReadUserShapes(CUserShapeBar* pUserBar);
	BOOL WriteShapeInformation(CString strFile, int barIndex);
	BOOL ExportShapeBarAndShapes(CString strFolder);
	void ReadUserShapeBars();
	void WriteUserShapeBars();
	CString UserProjectPath;

    BOOL IsUserShapeBarExist(CString strNewBar);

	// Generated message map functions
protected:
	CFont m_font;
	CGfxOutBarCtrl m_wndOutBarCtrl;
	CImageList m_smallIL;
	CImageList m_largeIL;
	int folder_index;
	int item_index;
    std::vector<CUserShapeBar*> m_vUserBars;
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
};

#endif // !defined(AFX_SHAPESBAR_H__B592C191_B7B4_4123_BD3B_9E5D858B5DB9__INCLUDED_)
