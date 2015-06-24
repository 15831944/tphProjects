#if !defined(AFX_DLGSHAPESELECT_H__390954B0_34D3_4528_BF7D_D1037B9347F6__INCLUDED_)
#define AFX_DLGSHAPESELECT_H__390954B0_34D3_4528_BF7D_D1037B9347F6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgShapeSelect.h : header file
//

#include "ShapesListBox.h"

/////////////////////////////////////////////////////////////////////////////
// CDlgShapeSelect dialog

class CDlgShapeSelect : public CDialog
{
// Construction
public:
	CDlgShapeSelect(CWnd* pParent = NULL);   // standard constructor

	CShape* GetSelectedShape() { return m_pSelectedShape; }

// Dialog Data
	//{{AFX_DATA(CDlgShapeSelect)
	enum { IDD = IDD_SHAPESELECT };
	CShapesListBox	m_lstShapes;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgShapeSelect)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CShape* m_pSelectedShape;

	// Generated message map functions
	//{{AFX_MSG(CDlgShapeSelect)
	virtual BOOL OnInitDialog();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSelChangeShapeList();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGSHAPESELECT_H__390954B0_34D3_4528_BF7D_D1037B9347F6__INCLUDED_)
