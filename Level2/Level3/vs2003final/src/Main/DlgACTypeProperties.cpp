// DlgACTypeProperties.cpp : implementation file
//

#include "stdafx.h"
#include "termplan.h"
#include <common/ARCUnit.h>
#include "..\common\Aircraft.h"

#include "DlgACTypeProperties.h"
#include ".\dlgactypeproperties.h"
#include <cmath>                    // for sqrt() and pow()

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include "../Common/AirportDatabase.h"
/////////////////////////////////////////////////////////////////////////////
// CDlgACTypeProperties dialog



CDlgACTypeProperties::CDlgACTypeProperties(CWnd* pParent /*=NULL*/)
	: CPropertyPage(CDlgACTypeProperties::IDD)
{
	
}


void CDlgACTypeProperties::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgACTypeProperties)
	DDX_Text(pDX, IDC_EDIT_IATACODE, m_sIATACode);
	DDX_Text(pDX, IDC_EDIT_DESC, m_sDesc);
	DDX_Text(pDX, IDC_EDIT_HEIGHT, m_dHeight);
	DDX_Text(pDX, IDC_EDIT_LEN, m_dLen);
	DDX_Text(pDX, IDC_EDIT_SPAN, m_dSpan);
	DDX_Text(pDX, IDC_EDIT_MZFW, m_dMZFW);
	DDX_Text(pDX, IDC_EDIT_OEW, m_dOEW);
	DDX_Text(pDX, IDC_EDIT_MTOW, m_dMTOW);
	DDX_Text(pDX, IDC_EDIT_MLW, m_dMLW);
	DDX_Text(pDX, IDC_EDIT_CAP, m_nCapacity);
	DDX_Text(pDX, IDC_EDIT_ICAOCODE, m_sWeightCategory);
	DDX_Text(pDX, IDC_EDIT_OTHERCODES, m_dMinTurn);
	DDX_Text(pDX, IDC_EDIT_CABIN, m_CabinWidth);
	DDX_Text(pDX, IDC_EDIT_WVC, m_sWVC);
	DDX_Text(pDX, IDC_EDIT_COMPANY,m_sCompany);
	DDX_Text(pDX, IDC_EDIT_GROUP,m_sGroup);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgACTypeProperties, CPropertyPage)
	//{{AFX_MSG_MAP(CDlgACTypeProperties)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgACTypeProperties message handlers

void CDlgACTypeProperties::setACType(CACType* pACType)
{
	CString sTemp;
	sTemp = pACType->m_sOtherCodes;
	sTemp.Replace('@', ',');

	ASSERT(pACType != NULL);
	//{{AFX_DATA_INIT(CDlgACTypeProperties)
	m_sDesc = pACType->m_sDescription;
	m_dHeight = InterceptDecimal(pACType->m_fHeight);
	m_dLen = InterceptDecimal(pACType->m_fLen);

	m_sIATACode = pACType->GetIATACode();
	m_dSpan = InterceptDecimal( pACType->m_fSpan);
	m_dMZFW = InterceptDecimal( ARCUnit::KgsToLBS(pACType->m_fMZFW) );
	m_dOEW = InterceptDecimal( ARCUnit::KgsToLBS(pACType->m_fOEW) );
	m_dMTOW = InterceptDecimal( ARCUnit::KgsToLBS(pACType->m_fMTOW) );
	m_dMLW = InterceptDecimal( ARCUnit::KgsToLBS(pACType->m_fMLW) );
	m_nCapacity = pACType->m_iCapacity;
	m_sICAOCode = pACType->GetICAOCode();
	m_sOtherCodes = sTemp;
	m_CabinWidth = pACType->m_dCabinWidth ;
	m_sCompany = pACType->m_sCompany;
	m_sGroup = pACType->m_sICAOGroup;
	m_sWVC = pACType->m_sWakeVortexCategory;
	m_sWeightCategory = pACType->m_sWeightCategory;
	m_dMinTurn = pACType->m_fMinTurnRad;
	//}}AFX_DATA_INIT
	m_pACT = pACType;
}

void CDlgACTypeProperties::OnOK() 
{
	UpdateData(TRUE);
	if(m_sIATACode.IsEmpty())
	{
		AfxMessageBox("Please enter an aircraft name",MB_ICONEXCLAMATION | MB_OK);
		return;
	}

    // Store data...
	m_sIATACode.Replace("\'","\'\'");
    m_pACT->setIATACode(  m_sIATACode );

	m_pACT->m_sDescription= m_sDesc;
	m_pACT->m_sDescription.Replace("\'","\'\'");
	m_pACT->m_fHeight = static_cast<float>(m_dHeight);
	m_pACT->m_fLen = static_cast<float>(m_dLen);
	m_pACT->m_fSpan = static_cast<float>(m_dSpan);
	m_pACT->m_fMZFW = static_cast<float>( ARCUnit::LBSToKG(m_dMZFW) );
    m_pACT->m_fOEW = static_cast<float>( ARCUnit::LBSToKG(m_dOEW) );
	m_pACT->m_fMTOW = static_cast<float>( ARCUnit::LBSToKG(m_dMTOW) );
    m_pACT->m_fMLW = static_cast<float>( ARCUnit::LBSToKG(m_dMLW) );
	m_pACT->m_iCapacity = m_nCapacity;
	m_pACT->m_dCabinWidth = m_CabinWidth ;
	m_pACT->m_fMinTurnRad = static_cast<float>(m_dMinTurn);
	m_pACT->m_sWeightCategory = m_sWeightCategory;
	m_pACT->m_sWakeVortexCategory = m_sWVC;
	m_pACT->m_sICAOGroup = m_sGroup;
	m_pACT->m_sCompany = m_sCompany;
	m_sICAOCode.Replace("\'","\'\'");
    m_pACT->SetICAOCode( m_sICAOCode );

    // Replace commas in other codes with @ to solve parse problems...
    m_sOtherCodes.Replace(',', '@');
    m_pACT->m_sOtherCodes = m_sOtherCodes;

    // Done...
	CPropertyPage::OnOK();
}

/*************************************************************/
//Input:
//		fValue: 
//			Type: double
//          Desc: Source double
//		nSaveNO:
//			Type: int
//			Desc: number of will be save, default is 2
//Output:
//		null
//Function:
//		intercept decimal from fValue and save nSaveNO
//Return Value:
//		the new value after intercept
/*************************************************************/
double CDlgACTypeProperties::InterceptDecimal(double fValue, int nSaveNO)
{
	double nPower = std::pow(10, nSaveNO);

	double fNewValue = int(fValue*nPower + 0.5) * std::pow(0.1, nSaveNO);

	return fNewValue;
}

BOOL CDlgACTypeProperties::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

// 	if (m_pACT->IsEdit())
// 	{
// 		GetDlgItem(IDC_EDIT_IATACODE)->EnableWindow(FALSE);
// 		GetDlgItem(IDC_EDIT_DESC)->EnableWindow(FALSE);
// 		GetDlgItem(IDC_EDIT_HEIGHT)->EnableWindow(FALSE);
// 		GetDlgItem(IDC_EDIT_LEN)->EnableWindow(FALSE);
// 		GetDlgItem(IDC_EDIT_SPAN)->EnableWindow(FALSE);
// 		GetDlgItem(IDC_EDIT_MZFW)->EnableWindow(FALSE);
// 		GetDlgItem(IDC_EDIT_OEW)->EnableWindow(FALSE);
// 		GetDlgItem(IDC_EDIT_MTOW)->EnableWindow(FALSE);
// 		GetDlgItem(IDC_EDIT_MLW)->EnableWindow(FALSE);
// 		GetDlgItem(IDC_EDIT_CAP)->EnableWindow(FALSE);
// 		GetDlgItem(IDC_EDIT_ICAOCODE)->EnableWindow(FALSE);
// 		GetDlgItem(IDC_EDIT_OTHERCODES)->EnableWindow(FALSE);
// 		GetDlgItem(IDC_EDIT_CABIN)->EnableWindow(FALSE);
// 		GetDlgItem(IDC_EDIT_COMPANY)->EnableWindow(FALSE);
// 		GetDlgItem(IDC_EDIT_GROUP)->EnableWindow(FALSE);
// 		GetDlgItem(IDC_EDIT_WVC)->EnableWindow(FALSE);
// 	}
	// TODO:  Add extra initialization here
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
