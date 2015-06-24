#if !defined(AFX_3DGRIDOPTIONSDLG_H__4265FEAD_534C_11D4_931E_0080C8F982B1__INCLUDED_)
#define AFX_3DGRIDOPTIONSDLG_H__4265FEAD_534C_11D4_931E_0080C8F982B1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// 3DGridOptionsDlg.h : header file
//

#include "ColorBox.h"
#include "Floor2.h"
class CTermPlanDoc;

/////////////////////////////////////////////////////////////////////////////
// C3DGridOptionsDlg dialog

class C3DGridOptionsDlg : public CDialog
{
// Construction
public:
	C3DGridOptionsDlg(const CGrid& _grid,CTermPlanDoc *pDoc, CWnd* _pParent = NULL);   // standard constructor

	void UpdateGridData(CGrid* pGrid);

// Dialog Data
	//{{AFX_DATA(C3DGridOptionsDlg)
	enum { IDD = IDD_GRIDOPTIONS };
	CSliderCtrl	m_ctlSliderSubdivs;
	CColorBox	m_cbSubdivs;
	CColorBox	m_cbGridLines;
	CColorBox	m_cbGridAxes;
	CColorBox	m_GridColorBox;
	int		m_nSubdivs;
	double	m_dGridLinesEvery;
	double	m_dGridSizeX;
	CString	m_sGridUnits1;
	CString	m_sGridUnits2;
	BOOL	m_bVisible;
	CString	m_sGridUnits3;
	double	m_dGridSizeY;
	//}}AFX_DATA
	COLORREF m_cAxesCol;
	COLORREF m_cLinesCol;
	COLORREF m_cSubdivCol;

	CTermPlanDoc* m_pDoc;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(C3DGridOptionsDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(C3DGridOptionsDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnColorChange(UINT nID);
	afx_msg void OnKillfocusEditSubdivs();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_3DGRIDOPTIONSDLG_H__4265FEAD_534C_11D4_931E_0080C8F982B1__INCLUDED_)
