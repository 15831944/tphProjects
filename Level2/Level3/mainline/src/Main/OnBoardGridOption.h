#pragma once
#include "ColorBox.h"

// COnBoardGridOption dialog
class CGrid;
class COnBoardGridOption : public CDialog
{
	DECLARE_DYNAMIC(COnBoardGridOption)

public:
	COnBoardGridOption(const CGrid& gridData, CWnd* pParent = NULL);   // standard constructor
	virtual ~COnBoardGridOption();

// Dialog Data
	enum { IDD = IDD_ONBOARD_GRIDOPTIONS };

	CSliderCtrl	m_ctlSliderSubdivs;
	CColorBox	m_cbSubdivs;
	CColorBox	m_cbGridLines;
	CColorBox	m_cbGridAxes;
	CColorBox	m_GridColorBox;
	int		m_nSubdivs;
	double	m_dGridLinesEvery;
	double	m_dGridSizeX;
	CString	m_sGridUnits;
	BOOL	m_bVisible;
	double	m_dGridSizeY;
	double m_dZGrad;
	//}}AFX_DATA
	COLORREF m_cAxesCol;
	COLORREF m_cLinesCol;
	COLORREF m_cSubdivCol;

	//get data from user set
	void GetGridData(CGrid& griddata);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	virtual BOOL OnInitDialog();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnColorChange(UINT nID);
	afx_msg void OnKillfocusEditSubdivs();

	DECLARE_MESSAGE_MAP()
};
