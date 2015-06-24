// AirsideStretchVerticalProfileEditor.cpp : implementation file
//

#include "stdafx.h"
#include "TermPlan.h"
#include "AirsideStretchVerticalProfileEditor.h"
#include ".\airsidestretchverticalprofileeditor.h"


// CAirsideStretchVerticalProfileEditor dialog

IMPLEMENT_DYNAMIC(CAirsideStretchVerticalProfileEditor, CXTResizeDialog)
CAirsideStretchVerticalProfileEditor::CAirsideStretchVerticalProfileEditor(std::vector<double>&vXPos,std::vector<double>&vZPos, const CPath2008& _path, CWnd* pParent /* = NULL */)
	: CXTResizeDialog(CAirsideStretchVerticalProfileEditor::IDD, pParent)
{
	m_vXPos = vXPos;
	m_vZPos = vZPos;
	m_Path  = _path;
}

CAirsideStretchVerticalProfileEditor::~CAirsideStretchVerticalProfileEditor()
{
}

void CAirsideStretchVerticalProfileEditor::DoDataExchange(CDataExchange* pDX)
{
	CXTResizeDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SLIDER2, m_SliderCtrl);
	DDX_Text(pDX, IDC_EDIT_HEIGHT, m_strHeight);
	DDX_Text(pDX, IDC_EDIT_DISTANCE, m_strDistance);
	DDX_Control(pDX, IDC_STATIC_PTLINE, m_wndPtLineChart);
}


BEGIN_MESSAGE_MAP(CAirsideStretchVerticalProfileEditor, CXTResizeDialog)
	ON_BN_CLICKED(IDC_BUTTON_APPLY, OnBnClickedButtonApply)
	ON_WM_HSCROLL()
	ON_MESSAGE(WM_UPDATE_ZOOMVALUE, UpdateSliderCtrlPos )
	ON_MESSAGE(WM_UPDATE_VALUE_EX, UpdatePointDistanceAndHeight )	
END_MESSAGE_MAP()


// CAirsideStretchVerticalProfileEditor message handlers

BOOL CAirsideStretchVerticalProfileEditor::OnInitDialog()
{
	CXTResizeDialog::OnInitDialog();

	m_SliderCtrl.SetRange(50, 200);
	m_SliderCtrl.SetPos(100);

	m_wndPtLineChart.SetData(m_vXPos, m_vZPos, &m_Path);

	SetResize(IDC_STATIC_DISTANCE, SZ_TOP_LEFT, SZ_TOP_LEFT);
	SetResize(IDC_EDIT_DISTANCE, SZ_TOP_LEFT, SZ_TOP_LEFT);
	SetResize(IDC_STATIC_HEIGHT, SZ_TOP_LEFT, SZ_TOP_LEFT);
	SetResize(IDC_EDIT_HEIGHT, SZ_TOP_LEFT, SZ_TOP_LEFT);
	SetResize(IDC_BUTTON_APPLY, SZ_TOP_LEFT, SZ_TOP_LEFT);
	SetResize(IDC_STATIC_PTLINE, SZ_TOP_LEFT, SZ_BOTTOM_RIGHT);
	SetResize(IDC_SLIDER2, SZ_BOTTOM_LEFT, SZ_BOTTOM_LEFT);
	SetResize(IDOK, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);
	SetResize(IDCANCEL, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);

	return TRUE;
}

void CAirsideStretchVerticalProfileEditor::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	m_wndPtLineChart.SetVerticalZoomRate(m_SliderCtrl.GetPos()/100.0);
	m_wndPtLineChart.Invalidate();

	CXTResizeDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CAirsideStretchVerticalProfileEditor::OnBnClickedButtonApply()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	int nCurSelectIndex = m_wndPtLineChart.GetCurSelectIndex();

	if (nCurSelectIndex < 0)
	{
		AfxMessageBox(_T("Please select a distance point"), MB_OK|MB_ICONWARNING);
		return;
	}

	double dHeight = atof(m_strHeight);
	m_wndPtLineChart.SetHeight(nCurSelectIndex, dHeight);
	m_wndPtLineChart.RedrawWindow();
}

LRESULT CAirsideStretchVerticalProfileEditor::UpdateSliderCtrlPos( WPARAM wParam, LPARAM lParam )
{
	double dZoomRate = (double)wParam;
	m_SliderCtrl.SetPos(int(dZoomRate));
	return TRUE;
}

LRESULT CAirsideStretchVerticalProfileEditor::UpdatePointDistanceAndHeight( WPARAM wParam, LPARAM lParam )
{
	double dDistance = static_cast<double>(wParam) / SCALE_FACTOR;
	double dHeight   = static_cast<double>(lParam) / SCALE_FACTOR;

	m_strDistance.Format(_T("%.2f"), dDistance);
	m_strHeight.Format(_T("%.2f"), dHeight);

	UpdateData(FALSE);

	return TRUE;
}