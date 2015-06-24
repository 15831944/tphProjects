// DlgACComponentProp.cpp : implementation file
//

#include "stdafx.h"
#include "TermPlan.h"
#include "DlgACComponentProp.h"
#include ".\dlgaccomponentprop.h"
#include "../Common/UnitsManager.h"


// CDlgACComponentProp dialog

IMPLEMENT_DYNAMIC(CDlgACComponentProp, CDialog)
CDlgACComponentProp::CDlgACComponentProp(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgACComponentProp::IDD, pParent)
{
	m_pos = ARCVector3(0,0,0);
	m_rotate = ARCVector3(0,0,0);
	m_scale = ARCVector3(1,1,1);
}

CDlgACComponentProp::~CDlgACComponentProp()
{
}

void CDlgACComponentProp::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_POSX, m_editPosx);
	DDX_Control(pDX, IDC_EDIT_POSY, m_editPosY);
	DDX_Control(pDX, IDC_EDIT_POSZ, m_editPosZ);
	DDX_Control(pDX, IDC_EDIT_ROTX, m_editRotateX);
	DDX_Control(pDX, IDC_EDIT_ROTY, m_editRotateY);
	DDX_Control(pDX, IDC_EDIT_ROTZ, m_editRotateZ);
	DDX_Control(pDX, IDC_EDIT_SCALX, m_editScaleX);
	DDX_Control(pDX, IDC_EDIT_SCALY, m_editScaleY);
	DDX_Control(pDX, IDC_EDIT_SCALZ, m_editScaleZ);
	DDX_Control(pDX, IDC_STATIC_UNIT, m_staticUnit);
}


BEGIN_MESSAGE_MAP(CDlgACComponentProp, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()


// CDlgACComponentProp message handlers

void CDlgACComponentProp::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	CString strTemp;
	m_editPosx.GetWindowText(strTemp);
	CUnitsManager* pUM = CUnitsManager::GetInstance();

	m_pos[VX] =  pUM->UnConvertLength(atof(strTemp.GetString()) );

	m_editPosY.GetWindowText(strTemp);
	m_pos[VY] = pUM->UnConvertLength( atof(strTemp.GetString()) );

	m_editPosZ.GetWindowText(strTemp);
	m_pos[VZ] = pUM->UnConvertLength( atof(strTemp.GetString()) );

	m_editRotateX.GetWindowText(strTemp);
	m_rotate[VX] = atof(strTemp.GetString());

	m_editRotateY.GetWindowText(strTemp);
	m_rotate[VY] = atof(strTemp.GetString());

	m_editRotateZ.GetWindowText(strTemp);
	m_rotate[VZ] = atof(strTemp.GetString());

	m_editScaleX.GetWindowText(strTemp);
	m_scale[VX] = atof(strTemp.GetString());

	m_editScaleY.GetWindowText(strTemp);
	m_scale[VY]= atof(strTemp.GetString());

	m_editScaleZ.GetWindowText(strTemp);
	m_scale[VZ] = atof(strTemp.GetString());
	OnOK();
}
BOOL CDlgACComponentProp::OnInitDialog()
{
	CDialog::OnInitDialog();

	CUnitsManager* pUM = CUnitsManager::GetInstance();
	CString strUnit;
	strUnit.Format("(%s)", pUM->GetLengthUnitString(pUM->GetLengthUnits()) );
	m_staticUnit.SetWindowText(strUnit);


	// TODO:  Add extra initialization here
	CString strTemp;
	strTemp.Format("%.2f",  pUM->ConvertLength(m_pos[VX]) );
	m_editPosx.SetWindowText(strTemp);

	strTemp.Format("%.2f",  pUM->ConvertLength(m_pos[VY]) );
	m_editPosY.SetWindowText(strTemp);

	strTemp.Format("%.2f",  pUM->ConvertLength(m_pos[VZ]) );
	m_editPosZ.SetWindowText(strTemp);

	strTemp.Format("%.2f", m_pos[VX]);
	m_editScaleZ.SetWindowText(strTemp);

	strTemp.Format("%.2f", m_rotate[VX]);
	m_editRotateX.SetWindowText(strTemp);

	strTemp.Format("%.2f", m_rotate[VY]);
	m_editRotateY.SetWindowText(strTemp);

	strTemp.Format("%.2f", m_rotate[VZ]);
	m_editRotateZ.SetWindowText(strTemp);

	strTemp.Format("%.2f", m_scale[VX]);
	m_editScaleX.SetWindowText(strTemp);

	strTemp.Format("%.2f", m_scale[VX]);
	m_editScaleY.SetWindowText(strTemp);

	strTemp.Format("%.2f", m_scale[VX]);
	m_editScaleZ.SetWindowText(strTemp);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
