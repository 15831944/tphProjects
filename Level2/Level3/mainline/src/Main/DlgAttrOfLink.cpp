// DlgAttrOfLink.cpp : implementation file
//

#include "stdafx.h"
#include "termplan.h"
#include "DlgAttrOfLink.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgAttrOfLink dialog


CDlgAttrOfLink::CDlgAttrOfLink(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgAttrOfLink::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgAttrOfLink)
	m_nRate = 0;
	m_nDistance = 0;
	//}}AFX_DATA_INIT
}


void CDlgAttrOfLink::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgAttrOfLink)
	DDX_Control(pDX, IDC_SPIN4, m_spnDistance);
	DDX_Control(pDX, IDC_SPIN1, m_spnRate);
	DDX_Text(pDX, IDC_EDIT2, m_nRate);
	DDV_MinMaxUInt(pDX, m_nRate, 0, 100);
	DDX_Text(pDX, IDC_EDIT_DISTANCE, m_nDistance);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgAttrOfLink, CDialog)
	//{{AFX_MSG_MAP(CDlgAttrOfLink)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN1, OnDeltaposSpin1)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN4, OnDeltaposSpin4)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgAttrOfLink message handlers

void CDlgAttrOfLink::OnOK() 
{
	// TODO: Add extra validation here
	UpdateData(TRUE);

	CDialog::OnOK();
}

void CDlgAttrOfLink::OnDeltaposSpin1(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	// TODO: Add your control notification handler code here
	int	nLowerBound;
	int	nUpperBound;

	m_spnRate.GetRange( nLowerBound, nUpperBound );
	if( pNMUpDown->iDelta > 0 )
	{
		m_nRate = pNMUpDown->iPos + pNMUpDown->iDelta;
		m_nRate = m_nRate > static_cast<UINT>(nUpperBound) ? nUpperBound : m_nRate;
	}
	else if( pNMUpDown->iDelta < 0 && pNMUpDown->iPos > nLowerBound )
	{
		m_nRate = pNMUpDown->iPos + pNMUpDown->iDelta;
		m_nRate = m_nRate < static_cast<UINT>(nLowerBound) ? nLowerBound : m_nRate;
	}
	
	SetDlgItemInt(IDC_EDIT2, m_nRate);

	*pResult = 0;	
}

BOOL CDlgAttrOfLink::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	m_spnRate.SetRange( 0, 100 );
	m_spnRate.SetPos(m_nRate);
	SetDlgItemInt(IDC_EDIT2, m_nRate);

	m_spnDistance.SetRange((short)0, (short)1000000);

	UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgAttrOfLink::OnDeltaposSpin4(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	m_nDistance += pNMUpDown->iDelta;

	UpdateData(FALSE);
	*pResult = 0;
}
