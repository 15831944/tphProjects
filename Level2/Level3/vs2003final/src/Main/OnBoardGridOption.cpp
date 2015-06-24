// OnBoardGridOption.cpp : implementation file
//

#include "stdafx.h"
#include "Resource.h"
#include "OnBoardGridOption.h"
#include "common\FloatUtils.h"
#include "../common/UnitsManager.h"
#include <common/Grid.h>


// COnBoardGridOption dialog
class CGrid;

IMPLEMENT_DYNAMIC(COnBoardGridOption, CDialog)
COnBoardGridOption::COnBoardGridOption(const CGrid& gridData, CWnd* pParent /*=NULL*/)
	: CDialog(COnBoardGridOption::IDD, pParent)
{
	m_cAxesCol = static_cast<COLORREF>(gridData.cAxesColor);
	m_cLinesCol = static_cast<COLORREF>(gridData.cLinesColor);
	m_cSubdivCol = static_cast<COLORREF>(gridData.cSubdivsColor);
	m_nSubdivs = gridData.nSubdivs;
	m_dGridLinesEvery = gridData.dLinesEvery;
	m_dGridSizeX = gridData.dSizeX;
	m_dGridSizeY = gridData.dSizeY;
	m_sGridUnits = _T("");
	m_bVisible = gridData.bVisibility;
	m_dZGrad = gridData.dSizeZ;

}

COnBoardGridOption::~COnBoardGridOption()
{
}

void COnBoardGridOption::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COnBoardGridOption)
	DDX_Control(pDX, IDC_SLIDER_SUBDIVS, m_ctlSliderSubdivs);
	DDX_Control(pDX, IDC_GRIDSUBDIVCOLORBOX, m_cbSubdivs);
	DDX_Control(pDX, IDC_GRIDLINESCOLORBOX, m_cbGridLines);
	DDX_Control(pDX, IDC_GRIDAXESCOLORBOX, m_cbGridAxes);
	DDX_Text(pDX, IDC_EDIT_SUBDIVS, m_nSubdivs);
	DDV_MinMaxInt(pDX, m_nSubdivs, 0, 1000);
	DDX_Text(pDX, IDC_EDIT_GRIDLINESEVERY, m_dGridLinesEvery);
	DDX_Text(pDX, IDC_EDIT_GRIDSIZE, m_dGridSizeX);
	DDX_Text(pDX, IDC_GRIDUNITS1, m_sGridUnits);
	DDX_Text(pDX, IDC_GRIDUNITS2, m_sGridUnits);
	DDX_Text(pDX, IDC_GRIDUNITS3, m_sGridUnits);
	DDX_Text(pDX, IDC_STATIC_UNITZGRAD, m_sGridUnits);
	DDX_Check(pDX, IDC_CHK_VISIBLE, m_bVisible);
	DDX_Text(pDX, IDC_EDIT_GRIDSIZE2, m_dGridSizeY);
	DDX_Text(pDX,IDC_EDIT_ZGRADUATION, m_dZGrad);
	//}}AFX_DATA_MAP
}



BEGIN_MESSAGE_MAP(COnBoardGridOption, CDialog)
	//{{AFX_MSG_MAP(COnBoardGridOption)
	ON_WM_HSCROLL()
	ON_CONTROL_RANGE(BN_CLICKED,IDC_GRIDAXESCOLORCHANGE,IDC_GRIDSUBDIVCOLORCHANGE, OnColorChange)
	ON_EN_KILLFOCUS(IDC_EDIT_SUBDIVS, OnKillfocusEditSubdivs)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


// COnBoardGridOption message handlers
BOOL COnBoardGridOption::OnInitDialog()
{
	CDialog::OnInitDialog();
	
	m_sGridUnits = UNITSMANAGER->GetLengthUnitString(CUnitsManager::GetInstance()->GetLengthUnits());
	m_dGridSizeX = RoundDouble(CUnitsManager::GetInstance()->ConvertLength(m_dGridSizeX), 2);
	m_dGridSizeY = RoundDouble(CUnitsManager::GetInstance()->ConvertLength(m_dGridSizeY), 2);
	m_dGridLinesEvery = RoundDouble(CUnitsManager::GetInstance()->ConvertLength(m_dGridLinesEvery),4);	

	m_dZGrad = RoundDouble(CUnitsManager::GetInstance()->ConvertLength(m_dZGrad),4);	

	m_cbGridAxes.SetColor(m_cAxesCol);
	m_cbGridLines.SetColor(m_cLinesCol);
	m_cbSubdivs.SetColor(m_cSubdivCol);
	m_ctlSliderSubdivs.SetRange(0,20,TRUE);
	m_ctlSliderSubdivs.SetPos(m_nSubdivs);

	UpdateData(FALSE);

	return TRUE;
}

void COnBoardGridOption::OnHScroll( UINT nSBCode, UINT nPos, CScrollBar* pScrollBar )
{
	CSliderCtrl* pSlider = (CSliderCtrl*) pScrollBar;
	nPos = pSlider->GetPos();
	switch(nSBCode) {
	case TB_PAGEDOWN:
	case TB_PAGEUP:
	case TB_ENDTRACK:
	case TB_THUMBPOSITION:
	case TB_THUMBTRACK:
		m_nSubdivs = nPos;
		UpdateData(FALSE);
		break;
	}

	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);

}

void COnBoardGridOption::OnColorChange( UINT nID )
{
	CColorBox* pCb = NULL;
	COLORREF* pCol = NULL;
	switch(nID) {
	case IDC_GRIDAXESCOLORCHANGE:
		pCb = &m_cbGridAxes;
		pCol = &m_cAxesCol;
		break;
	case IDC_GRIDLINESCOLORCHANGE:
		pCb = &m_cbGridLines;
		pCol = &m_cLinesCol;
		break;
	case IDC_GRIDSUBDIVCOLORCHANGE:
		pCb = &m_cbSubdivs;
		pCol = &m_cSubdivCol;
	}
	CColorDialog colorDlg(pCb->GetColor(), CC_ANYCOLOR | CC_FULLOPEN, this);
	if(colorDlg.DoModal() == IDOK)
	{
		*pCol = colorDlg.GetColor();
		pCb->SetColor(*pCol);
	}
}

void COnBoardGridOption::OnKillfocusEditSubdivs()
{
	UpdateData(TRUE);
	m_ctlSliderSubdivs.SetPos(m_nSubdivs);
	// TRACE("Kill focus called, pos set to %d\n", m_nSubdivs);
}

void COnBoardGridOption::GetGridData( CGrid& griddata )
{
	griddata.bVisibility = m_bVisible;
	
	griddata.dSizeX = UNITSMANAGER->UnConvertLength(m_dGridSizeX);
	griddata.dSizeY = UNITSMANAGER->UnConvertLength(m_dGridSizeY);
	griddata.dLinesEvery = UNITSMANAGER->UnConvertLength(m_dGridLinesEvery);
	griddata.dSizeZ = UNITSMANAGER->UnConvertLength(m_dZGrad);

	griddata.nSubdivs = m_nSubdivs;
	griddata.cAxesColor = m_cAxesCol;
	griddata.cLinesColor = m_cLinesCol;
	griddata.cSubdivsColor = m_cSubdivCol;
}