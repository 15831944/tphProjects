// DlgSectorProperties.cpp : implementation file
//

#include "stdafx.h"
#include "termplan.h"
#include "..\common\AirportsManager.h"
#include "DlgSectorProperties.h"
#include "engine\terminal.h"
#include "../Common/AirportDatabase.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgSectorProperties dialog


CDlgSectorProperties::CDlgSectorProperties(CAirportsManager* _pAirportMan, CSector* pSector, CWnd* pParent /*=NULL*/)
	: CDialog(CDlgSectorProperties::IDD, pParent), m_pAirportMan( _pAirportMan )
{
	//{{AFX_DATA_INIT(CDlgSectorProperties)
	m_sAirports = _T("");
	m_sSectorName = pSector->m_sName;
	//}}AFX_DATA_INIT
	m_pSector = pSector;
	int nSize = pSector->m_vAirportFilters.size();
	for(int i=0; i<(nSize-1); i++) {
		m_sAirports = m_sAirports + pSector->m_vAirportFilters[i].sIATACode + ",";
	}
	if(pSector->m_vAirportFilters.size() > 0)
		m_sAirports = m_sAirports + pSector->m_vAirportFilters[pSector->m_vAirportFilters.size()-1].sIATACode;
}


void CDlgSectorProperties::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgSectorProperties)
	DDX_Text(pDX, IDC_EDT_AIRPORTS, m_sAirports);
	DDX_Text(pDX, IDC_EDT_SECTORNAME, m_sSectorName);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgSectorProperties, CDialog)
	//{{AFX_MSG_MAP(CDlgSectorProperties)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgSectorProperties message handlers

void CDlgSectorProperties::OnOK() 
{
	UpdateData(TRUE);
	if(m_sSectorName.IsEmpty()) {
		AfxMessageBox("Please enter a sector name",MB_ICONEXCLAMATION | MB_OK);
		return;
	}
	m_pSector->m_sName = m_sSectorName;
	m_pSector->m_vAirportFilters.clear();
	CAIRPORTFILTER af;
	while(TRUE) {
		int nIdx = m_sAirports.Find(',');
		if(nIdx != -1) { // found a comma
			af.sIATACode = m_sAirports.Left(nIdx);
			//m_pAirportMan->AddSector(m_sSectorName, af);
			m_pSector->AddFilter(af);
			m_sAirports = m_sAirports.Right(m_sAirports.GetLength()-nIdx-1);
		}
		else if(!m_sAirports.IsEmpty()){ // no comma, but string not empty
			af.sIATACode = m_sAirports;
			//m_pAirportMan->AddSector(m_sSectorName, af);
			m_pSector->AddFilter(af);
			break;
		}
		else { // no comma, string empty
			break;
		}
	}
		
	CDialog::OnOK();
}
