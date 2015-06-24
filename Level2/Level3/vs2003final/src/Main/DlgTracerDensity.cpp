// DlgTracerDensity.cpp : implementation file
//

#include "stdafx.h"
#include "termplan.h"
#include "DlgTracerDensity.h"
#include <cmath>
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgTracerDensity dialog


CDlgTracerDensity::CDlgTracerDensity(double _density, CWnd* pParent /*=NULL*/)
	: CDialog(CDlgTracerDensity::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgTracerDensity)
	m_dDensity =  floor(_density+0.5);
	//}}AFX_DATA_INIT
}


void CDlgTracerDensity::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgTracerDensity)
	DDX_Control(pDX, IDC_SPIN_DENSITY, m_spinDensity);
	DDX_Text(pDX, IDC_EDIT_DENSITY, m_dDensity);
	DDV_MinMaxDouble(pDX, m_dDensity, 0.0, 100.0);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgTracerDensity, CDialog)
	//{{AFX_MSG_MAP(CDlgTracerDensity)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_DENSITY, OnDeltaPosSpinDensity)
	ON_EN_KILLFOCUS(IDC_EDIT_DENSITY, OnKillFocusEditDensity)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgTracerDensity message handlers

void CDlgTracerDensity::OnDeltaPosSpinDensity(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;

	// TRACE("Pos %d, Delta %d\n", pNMUpDown->iPos, pNMUpDown->iDelta);
	m_dDensity = pNMUpDown->iPos + pNMUpDown->iDelta;
	if(m_dDensity<0)m_dDensity=0;
	if(m_dDensity>100)m_dDensity = 100;
	UpdateData(FALSE);
	
	*pResult = 0;
}

void CDlgTracerDensity::OnKillFocusEditDensity() 
{
	// TODO: Add your control notification handler code here
	
}

BOOL CDlgTracerDensity::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	CWnd* pEditWnd = GetDlgItem(IDC_EDIT_DENSITY);
	m_spinDensity.SetBuddy(pEditWnd);

	m_spinDensity.SetRange(0, 100);
	int _density = int(m_dDensity);
	m_spinDensity.SetPos(_density);
	
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


double CDlgTracerDensity::GetDensity()
{
	return m_dDensity;
}
