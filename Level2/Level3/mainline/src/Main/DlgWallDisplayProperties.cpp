// DlgWallDisplayProperties.cpp : implementation file
//

#include "stdafx.h"
#include "TermPlan.h"
#include "DlgWallDisplayProperties.h"
#include ".\dlgwalldisplayproperties.h"


// CDlgWallDisplayProperties dialog

IMPLEMENT_DYNAMIC(CDlgWallDisplayProperties, CDialog)
CDlgWallDisplayProperties::CDlgWallDisplayProperties(WallShape* pWallShape, CWnd* pParent /*=NULL*/)
	: CDialog(CDlgWallDisplayProperties::IDD, pParent)
{
	ASSERT(pWallShape);
	m_pWallShape = pWallShape;

}

CDlgWallDisplayProperties::~CDlgWallDisplayProperties()
{
}

void CDlgWallDisplayProperties::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
//	DDX_Check(pDX, IDC_CHK_SHAPE, m_nDisplay[WSDP_DISP_SHAPE]);
//	DDX_Check(pDX, IDC_CHK_PROCNAME, m_nDisplay[WSDP_DISP_PROCNAME]);

	DDX_Control(pDX, IDC_CHK_SHAPE, m_chkDisplay[WSDP_DISP_SHAPE]);
	DDX_Control(pDX, IDC_CHK_PROCNAME, m_chkDisplay[WSDP_DISP_PROCNAME]);

	DDX_Control(pDX, IDC_CHANGESHAPECOLOR, m_btnColor[WSDP_DISP_SHAPE]);
	DDX_Control(pDX, IDC_CHANGENAMECOLOR, m_btnColor[WSDP_DISP_PROCNAME]);
}


BEGIN_MESSAGE_MAP(CDlgWallDisplayProperties, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()


// CDlgWallDisplayProperties message handlers

BOOL CDlgWallDisplayProperties::OnInitDialog()
{
	CDialog::OnInitDialog();

	WallShape::CWallShapeDispProperties disp;
	m_pWallShape->GetWallShapeDispProperties(disp);

	for(int i=WSDP_MIN; i<=WSDP_MAX; i++)
	{
		m_chkDisplay[i].SetCheck(disp.bDisplay[i]);
		m_btnColor[i].SetColor(disp.color[i]);
	}


	return TRUE;

}

void CDlgWallDisplayProperties::OnBnClickedOk()
{
	WallShape::CWallShapeDispProperties disp;
	for (int i=WSDP_MIN; i<=WSDP_MAX; i++)
	{
		disp.bDisplay[i] = (BOOL)m_chkDisplay[i].GetCheck();
		disp.color[i] = m_btnColor[i].GetColor();
	}
	m_pWallShape->SetWallShapeDispProperties(disp);

	OnOK();
}
