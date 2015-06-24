// DlgOnBoardObjectDisplayProperty.cpp : implementation file
//

#include "stdafx.h"
#include "TermPlan.h"
#include "DlgOnBoardObjectDisplayProperty.h"
#include "../InputOnBoard/OnBoardObjectDisplayProperty.h"
#include "../InputOnBoard/AircraftElement.h"
#include <inputonboard/AircraftElmentShapeDisplay.h>

//DSPTYPE_SHAPE,
//DSPTYPE_QUEUE,
//DSPTYPE_INCONS,
//DSPTYPE_OUTCONS,
//DSPTYPE_DIRECTINARROW,
const CString CDlgOnBoardObjectDisplayProperty::DSPPROPNAME[] ={
	_T("Display object name"),
		_T("Display object shape"),
		_T("Display object queue"),
		_T("Display object in constraint"),
		_T("Display object out constraint"),
		_T("Display direction arrow"),
};

// CDlgOnBoardObjectDisplayProperty dialog

IMPLEMENT_DYNAMIC(CDlgOnBoardObjectDisplayProperty, CDialog)
CDlgOnBoardObjectDisplayProperty::CDlgOnBoardObjectDisplayProperty(CAircraftElmentShapeDisplay* pElement ,CWnd* pParent /*=NULL*/)
	: CDialog(CDlgOnBoardObjectDisplayProperty::IDD, pParent)
{
	m_pElement = pElement;
	mDispPropEdit = pElement->mDspProps;
}

CDlgOnBoardObjectDisplayProperty::~CDlgOnBoardObjectDisplayProperty()
{
}

// CDlgOnBoardObjectDisplayProperty message handlers
void CDlgOnBoardObjectDisplayProperty::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Check(pDX, IDC_CHK_DP1, m_nDisplay[0]);
	DDX_Check(pDX, IDC_CHK_DP2, m_nDisplay[1]);
	DDX_Check(pDX, IDC_CHK_DP3, m_nDisplay[2]);
	DDX_Check(pDX, IDC_CHK_DP4, m_nDisplay[3]);
	DDX_Check(pDX, IDC_CHK_DP5, m_nDisplay[4]);
	DDX_Check(pDX, IDC_CHK_DP6, m_nDisplay[5]);
	DDX_Control(pDX, IDC_CHK_DP1, m_chkDisplay[0]);
	DDX_Control(pDX, IDC_CHK_DP2, m_chkDisplay[1]);
	DDX_Control(pDX, IDC_CHK_DP3, m_chkDisplay[2]);
	DDX_Control(pDX, IDC_CHK_DP4, m_chkDisplay[3]);
	DDX_Control(pDX, IDC_CHK_DP5, m_chkDisplay[4]);
	DDX_Control(pDX, IDC_CHK_DP6, m_chkDisplay[5]);

	DDX_Control(pDX, IDC_CHANGECOLOR_DP1, m_btnColor[0]);
	DDX_Control(pDX, IDC_CHANGECOLOR_DP2, m_btnColor[1]);
	DDX_Control(pDX, IDC_CHANGECOLOR_DP3, m_btnColor[2]);
	DDX_Control(pDX, IDC_CHANGECOLOR_DP4, m_btnColor[3]);
	DDX_Control(pDX, IDC_CHANGECOLOR_DP5, m_btnColor[4]);
	DDX_Control(pDX, IDC_CHANGECOLOR_DP6, m_btnColor[5]);
}


BEGIN_MESSAGE_MAP(CDlgOnBoardObjectDisplayProperty, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_CHANGECOLOR_DP1, IDC_CHANGECOLOR_DP6, OnChangeColor)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_CHK_DP1, IDC_CHK_DP6, OnChkDisplay)

	ON_MESSAGE(CPN_SELENDOK,     OnSelEndOK)
	ON_MESSAGE(CPN_SELENDCANCEL, OnSelEndCancel)
	ON_MESSAGE(CPN_SELCHANGE,    OnSelChange)
	ON_MESSAGE(CPN_CLOSEUP,         OnCloseUp)
	ON_MESSAGE(CPN_DROPDOWN,     OnDropDown)
END_MESSAGE_MAP()


// CDlgAirsideObjectDisplayProperties message handlers
//IDC_CHK_DP1
//IDC_CHK_DP2
//IDC_CHK_DP3
//IDC_CHK_DP4
//IDC_CHK_DP5
//IDC_CHK_DP6
//
//IDC_CHANGECOLOR_DP1
//IDC_CHANGECOLOR_DP2
//IDC_CHANGECOLOR_DP3
//IDC_CHANGECOLOR_DP4
//IDC_CHANGECOLOR_DP5
//IDC_CHANGECOLOR_DP6

void CDlgOnBoardObjectDisplayProperty::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	OnOK();
}

BOOL CDlgOnBoardObjectDisplayProperty::OnInitDialog()
{
	CDialog::OnInitDialog();

	//set text to the button

	//shape	
	m_chkDisplay[0].SetWindowText(DSPPROPNAME[CAircraftElmentShapeDisplay::DSPTYPE_SHAPE]);
	m_nDisplay[0] = mDispPropEdit[CAircraftElmentShapeDisplay::DSPTYPE_SHAPE].bOn?1:0;
	m_cColor[0] = (COLORREF)mDispPropEdit[CAircraftElmentShapeDisplay::DSPTYPE_SHAPE].cColor;
	m_btnColor[0].SetColor(m_cColor[0]);

	//name
	m_chkDisplay[1].SetWindowText(DSPPROPNAME[CAircraftElmentShapeDisplay::DSPTYPE_NAME]);
	m_nDisplay[1] = mDispPropEdit[CAircraftElmentShapeDisplay::DSPTYPE_NAME].bOn?1:0;
	m_cColor[1] = (COLORREF)mDispPropEdit[CAircraftElmentShapeDisplay::DSPTYPE_NAME].cColor;
	m_btnColor[1].SetColor(m_cColor[1]);

	
	m_chkDisplay[2].ShowWindow(FALSE);
	m_btnColor[2].ShowWindow(FALSE);

	m_chkDisplay[3].ShowWindow(FALSE);
	m_btnColor[3].ShowWindow(FALSE);
	m_chkDisplay[4].ShowWindow(FALSE);
	m_btnColor[4].ShowWindow(FALSE);
	m_chkDisplay[5].ShowWindow(FALSE);
	m_btnColor[5].ShowWindow(FALSE);

	if(m_pElement->IsSeat())
	{
		m_chkDisplay[2].SetWindowText(DSPPROPNAME[CAircraftElmentShapeDisplay::DSPTYPE_DIRECTINARROW]);
		m_nDisplay[2] = mDispPropEdit[CAircraftElmentShapeDisplay::DSPTYPE_DIRECTINARROW].bOn?1:0;
		m_cColor[2] = (COLORREF)mDispPropEdit[CAircraftElmentShapeDisplay::DSPTYPE_DIRECTINARROW].cColor;
		m_btnColor[2].SetColor(m_cColor[2]);
		
		m_chkDisplay[2].ShowWindow(TRUE);
		m_btnColor[2].ShowWindow(TRUE);
	}
	

	UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgOnBoardObjectDisplayProperty::OnOK()
{
	UpdateData(TRUE);



	//shape
	if (m_nDisplay[0] > 0)
		mDispPropEdit[CAircraftElmentShapeDisplay::DSPTYPE_SHAPE].bOn = true;
	else
		 mDispPropEdit[CAircraftElmentShapeDisplay::DSPTYPE_SHAPE].bOn  = false;
	 mDispPropEdit[CAircraftElmentShapeDisplay::DSPTYPE_SHAPE].cColor = ARCColor3(m_cColor[0]);

	//name
	if (m_nDisplay[1] > 0)
		 mDispPropEdit[CAircraftElmentShapeDisplay::DSPTYPE_NAME].bOn = true;
	else
		 mDispPropEdit[CAircraftElmentShapeDisplay::DSPTYPE_NAME].bOn = false;
	 mDispPropEdit[CAircraftElmentShapeDisplay::DSPTYPE_NAME].cColor = ARCColor3(m_cColor[1]);
	
	 if(m_pElement->IsSeat())
	 {
		 if (m_nDisplay[2] > 0)
			 mDispPropEdit[CAircraftElmentShapeDisplay::DSPTYPE_DIRECTINARROW].bOn = true;
		 else
			 mDispPropEdit[CAircraftElmentShapeDisplay::DSPTYPE_DIRECTINARROW].bOn = false;

		 mDispPropEdit[CAircraftElmentShapeDisplay::DSPTYPE_DIRECTINARROW].cColor = ARCColor3(m_cColor[2]);
	 }


	CDialog::OnOK();
}

void CDlgOnBoardObjectDisplayProperty::OnCancel()
{
	// TODO: Add your specialized code here and/or call the base class

	CDialog::OnCancel();
}
void CDlgOnBoardObjectDisplayProperty::OnChangeColor(UINT nID) 
{

}
void CDlgOnBoardObjectDisplayProperty::OnChkDisplay(UINT nID)
{
	//int idx = nID - IDC_CHK_DP1;
	//UpdateData(TRUE);
	//if(m_nDisplay[idx] != 2) 
	//{
	//	m_bApplyDisplay[idx] = TRUE;
	//}
	//else
	//	m_bApplyDisplay[idx] = FALSE;
}
LONG CDlgOnBoardObjectDisplayProperty::OnSelEndOK(UINT lParam, LONG wParam)
{
	m_cColor[wParam-IDC_CHANGECOLOR_DP1] = lParam;
	m_bApplyColor[wParam-IDC_CHANGECOLOR_DP1] = TRUE;
	return TRUE;
}

LONG CDlgOnBoardObjectDisplayProperty::OnSelEndCancel(UINT /*lParam*/, LONG /*wParam*/)
{
	TRACE0("Selection cancelled\n");
	return TRUE;
}

LONG CDlgOnBoardObjectDisplayProperty::OnSelChange(UINT /*lParam*/, LONG /*wParam*/)
{
	TRACE0("Selection changed\n");
	return TRUE;
}

LONG CDlgOnBoardObjectDisplayProperty::OnCloseUp(UINT /*lParam*/, LONG /*wParam*/)
{
	TRACE0("Colour picker close up\n");
	return TRUE;
}

LONG CDlgOnBoardObjectDisplayProperty::OnDropDown(UINT /*lParam*/, LONG /*wParam*/)
{
	TRACE0("Colour picker drop down\n");
	return TRUE;
}