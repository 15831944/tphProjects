// DlgAirlineProperties.cpp : implementation file
//

#include "stdafx.h"
#include "termplan.h"
#include "DlgAirlineProperties.h"
#include "..\common\AirlineManager.h"
#include "../Common/AirportDatabase.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgAirlineProperties dialog


CDlgAirlineProperties::CDlgAirlineProperties(CAirline* pAirline, CWnd* pParent /*=NULL*/)
	: CDialog(CDlgAirlineProperties::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgAirportProperties)
	m_sAirlineIATACode  = pAirline->m_sAirlineIATACode;
	m_sLongName         = pAirline->m_sLongName;
    m_sICAOCode         = pAirline->m_sICAOCode;
    m_sOtherCodes       = pAirline->m_sOtherCodes;
    m_sStreetAddress    = pAirline->m_sStreetAddress;
    m_sCity             = pAirline->m_sCity;
    m_sCountry          = pAirline->m_sCountry;
    m_sHQZipCode        = pAirline->m_sHQZipCode;
    m_sContinent        = pAirline->m_sContinent;
    m_sTelephone        = pAirline->m_sTelephone;
    m_sFax              = pAirline->m_sFax;
    m_sEMail            = pAirline->m_sEMail;
    m_sDirector         = pAirline->m_sDirector;
    m_sFleet            = pAirline->m_sFleet;
	//}}AFX_DATA_INIT
	m_pAirline = pAirline;


}


void CDlgAirlineProperties::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	//{{AFX_DATA_MAP(CDlgAirlineProperties)
	DDX_Text(pDX, IDC_EDT_ALIATACODE, m_sAirlineIATACode);
	DDX_Text(pDX, IDC_EDT_ALLONGNAME, m_sLongName);
    DDX_Text(pDX, IDC_EDT_ALICAOCODE, m_sICAOCode);
    DDX_Text(pDX, IDC_EDT_ALOTHERCODES, m_sOtherCodes);
    DDX_Text(pDX, IDC_EDT_ALSTREETADDRESS, m_sStreetAddress);
    DDX_Text(pDX, IDC_EDT_ALCITY, m_sCity);
    DDX_Text(pDX, IDC_EDT_ALCOUNTRY, m_sCountry);
    DDX_Text(pDX, IDC_EDT_ALHQZIPCODE, m_sHQZipCode);
    DDX_Text(pDX, IDC_EDT_ALCONTINENT, m_sContinent);
    DDX_Text(pDX, IDC_EDT_ALTELEPHONE, m_sTelephone);
    DDX_Text(pDX, IDC_EDT_ALFAX, m_sFax);
    DDX_Text(pDX, IDC_EDT_ALEMAIL, m_sEMail);
    DDX_Text(pDX, IDC_EDT_ALDIRECTOR, m_sDirector);
    DDX_Text(pDX, IDC_EDT_ALFLEET, m_sFleet);
	//}}AFX_DATA_MAPd

}


BEGIN_MESSAGE_MAP(CDlgAirlineProperties, CDialog)
	//{{AFX_MSG_MAP(CDlgAirlineProperties)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgAirlineProperties message handlers

void CDlgAirlineProperties::OnOK() 
{
	// TODO: Add extra validation here
	UpdateData(TRUE);
		
	if(m_sAirlineIATACode.IsEmpty()) {
		AfxMessageBox("Please enter an airline IATA code",MB_ICONEXCLAMATION | MB_OK);
		return;
	}
	m_pAirline->m_sAirlineIATACode  = m_sAirlineIATACode;
	m_pAirline->m_sLongName         = m_sLongName;
	m_pAirline->m_sICAOCode         = m_sICAOCode;
	m_pAirline->m_sOtherCodes       = m_sOtherCodes;
	m_pAirline->m_sStreetAddress    = m_sStreetAddress;
	m_pAirline->m_sCity             = m_sCity;
	m_pAirline->m_sCountry          = m_sCountry;
	m_pAirline->m_sHQZipCode        = m_sHQZipCode;
	m_pAirline->m_sContinent        = m_sContinent;
	m_pAirline->m_sTelephone        = m_sTelephone;
	m_pAirline->m_sFax              = m_sFax;
	m_pAirline->m_sEMail            = m_sEMail;
	m_pAirline->m_sDirector         = m_sDirector;
	m_pAirline->m_sFleet            = m_sFleet;

	CDialog::OnOK();
}
