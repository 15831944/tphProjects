// DlgSubairlineProperties.cpp : implementation file
//

#include "stdafx.h"
#include "termplan.h"
#include "DlgSubairlineProperties.h"
#include "..\common\AirlineManager.h"
#include "../Common/AirportDatabase.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgSubairlineProperties dialog


CDlgSubairlineProperties::CDlgSubairlineProperties(CAirlinesManager* _pAirlineMan, CSubairline* pSubairline, CWnd* pParent /*=NULL*/)
	: CDialog(CDlgSubairlineProperties::IDD, pParent), m_pAirlineMan( _pAirlineMan )
{
	//{{AFX_DATA_INIT(CDlgSectorProperties)
	m_sAirlines = _T("");
	m_sSubairlineName = pSubairline->m_sName;
	//}}AFX_DATA_INIT
	m_pSubairline = pSubairline;
	int nSize = pSubairline->m_vAirlineFilters.size();
	for(int i=0; i<(nSize-1); i++) {
		m_sAirlines = m_sAirlines + pSubairline->m_vAirlineFilters[i].code + ",";
	}
	if(pSubairline->m_vAirlineFilters.size() > 0)
		m_sAirlines = m_sAirlines + pSubairline->m_vAirlineFilters[pSubairline->m_vAirlineFilters.size()-1].code;
}


void CDlgSubairlineProperties::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgSubairlineProperties)
	DDX_Text(pDX, IDC_EDT_AIRLINES, m_sAirlines);
	DDX_Text(pDX, IDC_EDT_SUBAIRLINENAME, m_sSubairlineName);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgSubairlineProperties, CDialog)
	//{{AFX_MSG_MAP(CDlgSubairlineProperties)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgSubairlineProperties message handlers

void CDlgSubairlineProperties::OnOK() 
{
	// TODO: Add extra validation here
	UpdateData(TRUE);

	if(m_sSubairlineName.IsEmpty()) {
		AfxMessageBox("Please enter a subairline name",MB_ICONEXCLAMATION | MB_OK);
		return;
	}

	m_pSubairline->m_sName = m_sSubairlineName;
	m_pSubairline->m_vAirlineFilters.clear();
	CAIRLINEFILTER af;
	while(TRUE) {
		int nIdx = m_sAirlines.Find(',');
		if(nIdx != -1) { // found a comma
			af.code = m_sAirlines.Left(nIdx);
			m_pSubairline->AddFilter(af);
			//m_pAirlineMan->AddSubairline(m_sSubairlineName, af);
			m_sAirlines = m_sAirlines.Right(m_sAirlines.GetLength()-nIdx-1);
		}
		else if(!m_sAirlines.IsEmpty()){ // no comma, but string not empty
			af.code = m_sAirlines;
			m_pSubairline->AddFilter(af);
			//m_pAirlineMan->AddSubairline(m_sSubairlineName, af);
			break;
		}
		else { // no comma, string empty
			break;
		}
	}
		
	CDialog::OnOK();
}
