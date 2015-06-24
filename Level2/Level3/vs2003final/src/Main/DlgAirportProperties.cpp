// DlgAirportProperties.cpp : implementation file
//

#include "stdafx.h"
#include "termplan.h"
#include "..\common\AirportsManager.h"
#include "DlgAirportProperties.h"
#include ".\dlgairportproperties.h"
#include "../common/AirportDatabase.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgAirportProperties dialog


CDlgAirportProperties::CDlgAirportProperties(CAirport* pAirport, CWnd* pParent /*=NULL*/)
	: CDialog(CDlgAirportProperties::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgAirportProperties)
	m_sIATACode                 = pAirport->m_sIATACode;;
	m_sLongName                 = pAirport->m_sLongName;
    m_sICAOCode                 = pAirport->m_sICAOCode;
	m_sARPCoordinates           = pAirport->m_sARPCoordinates;
    m_sCity                     = pAirport->m_sCity;
    m_sCountry                  = pAirport->m_sCountry;
    m_sCountryCode              = pAirport->m_sCountryCode;
    m_sElevation                = pAirport->m_sElevation;
    m_sNumberOfRunways          = pAirport->m_sNumberOfRunways;
    m_sMaximumRunwayLength      = pAirport->m_sMaximumRunwayLength;
    m_sAlternateAirportICAOCode = pAirport->m_sAlternateAirportICAOCode;

	//}}AFX_DATA_INIT
	m_pAirport = pAirport;

}


void CDlgAirportProperties::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Text(pDX, IDC_EDT_APIATACODE, m_sIATACode);
	DDX_Text(pDX, IDC_EDT_APLONGNAME, m_sLongName);
	DDX_Text(pDX, IDC_EDT_APICAOCODE, m_sICAOCode);
	DDX_Text(pDX, IDC_EDT_APARPCOORDINATES, m_sARPCoordinates);
	DDX_Text(pDX, IDC_EDT_APCITY, m_sCity);
	DDX_Text(pDX, IDC_EDT_APCOUNTRY, m_sCountry);
	DDX_Text(pDX, IDC_EDT_APCOUNTRYCODE, m_sCountryCode);
	DDX_Text(pDX, IDC_EDT_APELEVATION, m_sElevation);
	DDX_Text(pDX, IDC_EDT_APNUMBEROFRUNWAYS, m_sNumberOfRunways);
	DDX_Text(pDX, IDC_EDT_APMAXIMUMRUNWAYLENGTH, m_sMaximumRunwayLength);
	DDX_Text(pDX, IDC_EDT_APALTERNATEAIRPORTICAOCODE, m_sAlternateAirportICAOCode);
//	DDX_Control(pDX, IDC_LONGITUDE, m_pLongitude);
//	DDX_Control(pDX, IDC_LATITUDE, m_pLatitude);


	//	DDX_Control(pDX, IDC_LONGITUDE, m_pLongitude);
	//	DDX_Control(pDX, IDC_LATITUDE, m_pLatitude);
	//	DDX_IPAddr(pDX, IDC_LONGITUDE, m_nLongitude);
	//	DDX_IPAddr(pDX, IDC_LATITUDE, m_nLatitude);


}


BEGIN_MESSAGE_MAP(CDlgAirportProperties, CDialog)
	//{{AFX_MSG_MAP(CDlgAirportProperties)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgAirportProperties message handlers
void CDlgAirportProperties::OnOK()
{
	UpdateData(TRUE);
	if(m_sIATACode.IsEmpty()) {
		AfxMessageBox("Please enter an airport IATA code",MB_ICONEXCLAMATION | MB_OK);
		return;
	}
	m_pAirport->m_sIATACode                 = m_sIATACode;
	m_pAirport->m_sLongName                 = m_sLongName;
    m_pAirport->m_sICAOCode                 = m_sICAOCode;
    //m_pAirport->m_sARPCoordinates           = m_sARPCoordinates;
    m_pAirport->m_sCity                     = m_sCity;
    m_pAirport->m_sCountry                  = m_sCountry;
    m_pAirport->m_sCountryCode              = m_sCountryCode;
    m_pAirport->m_sElevation                = m_sElevation;
    m_pAirport->m_sNumberOfRunways          = m_sNumberOfRunways;
    m_pAirport->m_sMaximumRunwayLength      = m_sMaximumRunwayLength;
    m_pAirport->m_sAlternateAirportICAOCode = m_sAlternateAirportICAOCode;

	CString strLongitude;
	m_pLongitude.GetCtrlText(strLongitude);
	CString strLatitude;
	m_pLatitude.GetCtrlText(strLatitude);

	m_sARPCoordinates.Format(_T("%s-%s"), strLongitude, strLatitude);
	m_pAirport->m_sARPCoordinates           = m_sARPCoordinates;

	CDialog::OnOK();
}

void CDlgAirportProperties::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	OnOK();
}

BOOL CDlgAirportProperties::OnInitDialog()
{
	CDialog::OnInitDialog();
//	m_pLongitude.SetWindowText("aaa");

	CRect rectLongitude, rectLatitude;
	GetDlgItem(IDC_LONGITUDE)->GetWindowRect(&rectLongitude);
	GetDlgItem(IDC_LATITUDE)->GetWindowRect(&rectLatitude);
	ScreenToClient(rectLongitude);
	ScreenToClient(rectLatitude);

//	GetDlgItem(IDC_LONGITUDE)->MoveWindow(CRect(0,0,0,0));
//	GetDlgItem(IDC_LATITUDE)->MoveWindow(CRect(0,0,0,0));
	GetDlgItem(IDC_LONGITUDE)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_LATITUDE)->ShowWindow(SW_HIDE);

	m_pLongitude.Create(NULL, NULL, WS_CHILD|WS_VISIBLE|WS_TABSTOP, CRect(0,0,0,0), this, 10);
	m_pLatitude.Create(NULL, NULL, WS_CHILD|WS_VISIBLE|WS_TABSTOP, CRect(0,0,0,0), this, 11);
	m_pLongitude.ModifyStyleEx(0, WS_EX_CLIENTEDGE);
	m_pLatitude.ModifyStyleEx(0, WS_EX_CLIENTEDGE);
	m_pLongitude.SetWindowPos(&CWnd::wndTop, rectLongitude.left, rectLongitude.top,
							rectLongitude.Width(), rectLongitude.Height(), SWP_SHOWWINDOW);
	m_pLatitude.SetWindowPos(&CWnd::wndTop, rectLatitude.left, rectLatitude.top, 
							rectLatitude.Width(), rectLatitude.Height(), SWP_SHOWWINDOW);

	m_pLongitude.SetComboBoxItem("E");
	m_pLongitude.SetComboBoxItem("W");
	m_pLongitude.SetItemRange(180);
	

	m_pLatitude.SetComboBoxItem("N");
	m_pLatitude.SetComboBoxItem("S");
	m_pLatitude.SetItemRange(90);	
	m_pLatitude.m_Addr[1].SetLimitText(2);

	if (m_sARPCoordinates.IsEmpty())
	{
		m_pLongitude.SetCtrlText("E 0:0:0");
		m_pLatitude.SetCtrlText("N 0:0:0");
	}
	else
	{
		CString strLongitude;
		CString strLatitude;
		int nIndex = m_sARPCoordinates.Find("-");
		strLongitude = m_sARPCoordinates.Left(nIndex);
		m_pLongitude.SetCtrlText(strLongitude);
		strLatitude  = m_sARPCoordinates.Right(m_sARPCoordinates.GetLength() - nIndex-1);
		m_pLatitude.SetCtrlText(strLatitude);
	}

	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
