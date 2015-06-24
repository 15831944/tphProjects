// DlgModifyThickness.cpp : implementation file
//

#include "stdafx.h"
#include "../TermPlan.h"
#include "DlgModifyThickness.h"
#include ".\dlgmodifythickness.h"

#include "Common/ARCStringConvert.h"
#include "common/UnitsManager.h"

// CDlgModifyThickness dialog

IMPLEMENT_DYNAMIC(CDlgModifyThickness, CDialog)
CDlgModifyThickness::CDlgModifyThickness(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgModifyThickness::IDD, pParent)
	, m_dThick(0)
{
}

CDlgModifyThickness::~CDlgModifyThickness()
{
}

void CDlgModifyThickness::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_THICK, m_editThick);
	DDX_Control(pDX, IDC_STATIC_UNIT, m_staticUnit);
}


BEGIN_MESSAGE_MAP(CDlgModifyThickness, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()


// CDlgModifyThickness message handlers

BOOL CDlgModifyThickness::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	CString strText;
	strText.Format("%.2f",m_dThick);
	m_editThick.SetWindowText(strText);


	CUnitsManager* pUM = CUnitsManager::GetInstance();	
	if(pUM)
		strText.Format("(%s)",pUM->GetLengthUnitString().GetString() );
	m_staticUnit.SetWindowText(strText);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgModifyThickness::setThickness( double dthick )
{
	CUnitsManager* pUM = CUnitsManager::GetInstance();	
	if(pUM)
		m_dThick = pUM->ConvertLength(dthick);
}

double CDlgModifyThickness::getThickness() const
{
	CUnitsManager* pUM = CUnitsManager::GetInstance();	
	if(pUM)
		return pUM->UnConvertLength(m_dThick);
	else 
		return 0;
}



void CDlgModifyThickness::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	CString strText;	
	m_editThick.GetWindowText(strText);

	if( !ARCStringConvert::isNumber(strText, m_dThick) )
	{
		AfxMessageBox("Please Insert a number of Thickness");
		return;
	}
	OnOK();
}
