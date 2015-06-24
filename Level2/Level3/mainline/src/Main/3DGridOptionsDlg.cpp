// 3DGridOptionsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "TermPlan.h"
#include "TermPlanDoc.h"
#include "3DGridOptionsDlg.h"

#include "common\FloatUtils.h"

#include "../common/UnitsManager.h"
//#include <inputairside/ARCUnitManager.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// C3DGridOptionsDlg dialog


C3DGridOptionsDlg::C3DGridOptionsDlg(const CGrid& _grid,CTermPlanDoc *pDoc, CWnd* _pParent /*= NULL*/) :
	CDialog(C3DGridOptionsDlg::IDD, _pParent),
	m_cAxesCol(static_cast<COLORREF>(_grid.cAxesColor)),
	m_cLinesCol(static_cast<COLORREF>(_grid.cLinesColor)),
	m_cSubdivCol(static_cast<COLORREF>(_grid.cSubdivsColor)),
	m_pDoc(pDoc)
{
	//{{AFX_DATA_INIT(C3DGridOptionsDlg)
	m_nSubdivs = _grid.nSubdivs;
	m_dGridLinesEvery = _grid.dLinesEvery;
	m_dGridSizeX = _grid.dSizeX;
	m_dGridSizeY = _grid.dSizeY;
	m_sGridUnits1 = _T("");
	m_sGridUnits2 = _T("");
	m_sGridUnits3 = _T("");
	m_bVisible = _grid.bVisibility;
	//}}AFX_DATA_INIT
}


void C3DGridOptionsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(C3DGridOptionsDlg)
	DDX_Control(pDX, IDC_SLIDER_SUBDIVS, m_ctlSliderSubdivs);
	DDX_Control(pDX, IDC_GRIDSUBDIVCOLORBOX, m_cbSubdivs);
	DDX_Control(pDX, IDC_GRIDLINESCOLORBOX, m_cbGridLines);
	DDX_Control(pDX, IDC_GRIDAXESCOLORBOX, m_cbGridAxes);
	DDX_Text(pDX, IDC_EDIT_SUBDIVS, m_nSubdivs);
	DDV_MinMaxInt(pDX, m_nSubdivs, 0, 1000);
	DDX_Text(pDX, IDC_EDIT_GRIDLINESEVERY, m_dGridLinesEvery);
	DDX_Text(pDX, IDC_EDIT_GRIDSIZE, m_dGridSizeX);
	DDX_Text(pDX, IDC_GRIDUNITS1, m_sGridUnits1);
	DDX_Text(pDX, IDC_GRIDUNITS2, m_sGridUnits2);
	DDX_Check(pDX, IDC_CHK_VISIBLE, m_bVisible);
	DDX_Text(pDX, IDC_GRIDUNITS3, m_sGridUnits3);
	DDX_Text(pDX, IDC_EDIT_GRIDSIZE2, m_dGridSizeY);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(C3DGridOptionsDlg, CDialog)
	//{{AFX_MSG_MAP(C3DGridOptionsDlg)
	ON_WM_HSCROLL()
	ON_CONTROL_RANGE(BN_CLICKED,IDC_GRIDAXESCOLORCHANGE,IDC_GRIDSUBDIVCOLORCHANGE, OnColorChange)
	ON_EN_KILLFOCUS(IDC_EDIT_SUBDIVS, OnKillfocusEditSubdivs)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// C3DGridOptionsDlg message handlers

BOOL C3DGridOptionsDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	/*if(m_pDoc && m_pDoc->m_systemMode == EnvMode_AirSide){		
		m_sGridUnits1 = m_sGridUnits2 = m_sGridUnits3 = CARCLengthUnit::GetLengthUnitString(m_pDoc->m_umGlobalUnitManager.GetCurSelLengthUnit());
		m_dGridSizeX = RoundDouble(CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,m_pDoc->m_umGlobalUnitManager.GetCurSelLengthUnit(),m_dGridSizeX), 2);
		m_dGridSizeY = RoundDouble(CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,m_pDoc->m_umGlobalUnitManager.GetCurSelLengthUnit(),m_dGridSizeY), 2);
		m_dGridLinesEvery = RoundDouble(CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,m_pDoc->m_umGlobalUnitManager.GetCurSelLengthUnit(),m_dGridLinesEvery),4);
	}else*/{
		m_sGridUnits1 = m_sGridUnits2 = m_sGridUnits3 = UNITSMANAGER->GetLengthUnitString(CUnitsManager::GetInstance()->GetLengthUnits());
		m_dGridSizeX = RoundDouble(CUnitsManager::GetInstance()->ConvertLength(m_dGridSizeX), 2);
		m_dGridSizeY = RoundDouble(CUnitsManager::GetInstance()->ConvertLength(m_dGridSizeY), 2);
		m_dGridLinesEvery = RoundDouble(CUnitsManager::GetInstance()->ConvertLength(m_dGridLinesEvery),4);
	}
	
	m_cbGridAxes.SetColor(m_cAxesCol);
	m_cbGridLines.SetColor(m_cLinesCol);
	m_cbSubdivs.SetColor(m_cSubdivCol);
	m_ctlSliderSubdivs.SetRange(0,20,TRUE);
	m_ctlSliderSubdivs.SetPos(m_nSubdivs);

	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void C3DGridOptionsDlg::OnColorChange(UINT nID) 
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

void C3DGridOptionsDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
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

void C3DGridOptionsDlg::OnKillfocusEditSubdivs() 
{
	UpdateData(TRUE);
	m_ctlSliderSubdivs.SetPos(m_nSubdivs);
	// TRACE("Kill focus called, pos set to %d\n", m_nSubdivs);
}

void C3DGridOptionsDlg::UpdateGridData(CGrid* pGrid)
{
	pGrid->bVisibility = m_bVisible;
	/*if(m_pDoc && m_pDoc->m_systemMode == EnvMode_AirSide){	
		pGrid->dSizeX = CARCLengthUnit::ConvertLength(m_pDoc->m_umGlobalUnitManager.GetCurSelLengthUnit(),DEFAULT_DATABASE_LENGTH_UNIT,m_dGridSizeX);
		pGrid->dSizeY = CARCLengthUnit::ConvertLength(m_pDoc->m_umGlobalUnitManager.GetCurSelLengthUnit(),DEFAULT_DATABASE_LENGTH_UNIT,m_dGridSizeY);
		pGrid->dLinesEvery = CARCLengthUnit::ConvertLength(m_pDoc->m_umGlobalUnitManager.GetCurSelLengthUnit(),DEFAULT_DATABASE_LENGTH_UNIT,m_dGridLinesEvery);
	}else*/{
		pGrid->dSizeX = UNITSMANAGER->UnConvertLength(m_dGridSizeX);
		pGrid->dSizeY = UNITSMANAGER->UnConvertLength(m_dGridSizeY);
		pGrid->dLinesEvery = UNITSMANAGER->UnConvertLength(m_dGridLinesEvery);
	}
	pGrid->nSubdivs = m_nSubdivs;
	pGrid->cAxesColor = m_cAxesCol;
	pGrid->cLinesColor = m_cLinesCol;
	pGrid->cSubdivsColor = m_cSubdivCol;
}