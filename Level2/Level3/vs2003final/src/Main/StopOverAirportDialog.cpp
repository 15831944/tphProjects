// StopOverAirportDialog.cpp : implementation file
//
#include "stdafx.h"
#include "Resource.h"
#include "StopOverAirportDialog.h"
#include "../common/AirportsManager.h"
#include "../inputs/IN_TERM.H"
CStopOverAirportDialog::CStopOverAirportDialog(InputTerminal* pInputTerminal, CWnd* pParent /*=NULL*/)
	: CDialog(IDD_DIALOG_STOPOVERFLIGHT, pParent)
	, m_pInputTerminal(pInputTerminal)
{
}

CStopOverAirportDialog::~CStopOverAirportDialog()
{
}

void CStopOverAirportDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_AIRPORTSECTOR, m_wndAirpotSectorList);
}


BEGIN_MESSAGE_MAP(CStopOverAirportDialog, CDialog)
	ON_BN_CLICKED(IDC_RADIO_AIRPORT, OnRadioAirport)
	ON_BN_CLICKED(IDC_RADIO_SECTOR, OnRadioSector)
	ON_LBN_SELCHANGE(IDC_LIST_AIRPORTSECTOR, OnSelchangeAirportSectorList)
END_MESSAGE_MAP()


// CStopOverAirportDialog message handlers

BOOL CStopOverAirportDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	CheckRadioButton(IDC_RADIO_AIRPORT, IDC_RADIO_SECTOR, IDC_RADIO_AIRPORT);
	SetAirportContents();

	return TRUE;
}

void CStopOverAirportDialog::SetAirportContents()
{
	m_wndAirpotSectorList.ResetContent();

	CAIRPORTLIST* pvAirportList = _g_GetActiveAirportMan(m_pInputTerminal->m_pAirportDB)->GetAirportList();
	size_t nCount = pvAirportList->size();
	for (size_t i = 0; i < nCount; i++) 
	{
		CAirport* pAirport = pvAirportList->at(i);
		m_wndAirpotSectorList.AddString(pAirport->m_sIATACode);
	}
}

void CStopOverAirportDialog::SetSectorContents()
{
	m_wndAirpotSectorList.ResetContent();

	CSECTORLIST* pvSector = _g_GetActiveAirportMan(m_pInputTerminal->m_pAirportDB)->GetSectorList();
	size_t nCount = pvSector->size();
	for (size_t i = 0; i < nCount; i++) 
	{
		CSector* pSector = pvSector->at(i);
		m_wndAirpotSectorList.AddString(pSector->m_sName);
	}
}

void CStopOverAirportDialog::OnRadioAirport()
{
	SetAirportContents();

	// update the airport or sector
	OnSelchangeAirportSectorList();
}

void CStopOverAirportDialog::OnRadioSector()
{
	SetSectorContents();

	// update the airport or sector
	OnSelchangeAirportSectorList();
}

void CStopOverAirportDialog::OnOK()
{
	CDialog::OnOK();
}

void CStopOverAirportDialog::OnSelchangeAirportSectorList()
{
	int nIndex = m_wndAirpotSectorList.GetCurSel();
	CString strSel;
	if (nIndex != LB_ERR)
		m_wndAirpotSectorList.GetText(nIndex, strSel);

	if (IDC_RADIO_AIRPORT == GetCheckedRadioButton(IDC_RADIO_AIRPORT, IDC_RADIO_SECTOR))
	{
		m_strAirport = strSel;
		m_strSector.Empty();
	}
	else
	{
		m_strSector = strSel;
		m_strAirport.Empty();
	}
}
