// DlgArrayCopies.cpp : implementation file
//

#include "stdafx.h"
#include "termplan.h"
#include "DlgArrayCopies.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgArrayCopies dialog


CDlgArrayCopies::CDlgArrayCopies(int nCopies , CWnd* pParent /*=NULL*/)
	: CDialog(CDlgArrayCopies::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgArrayCopies)
	m_nCopies = nCopies;
	//}}AFX_DATA_INIT
}


void CDlgArrayCopies::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgArrayCopies)
	DDX_Control(pDX, IDC_SPIN_NUMCOPIES, m_spinCopies);
	DDX_Text(pDX, IDC_EDIT_NUMCOPIES, m_nCopies);
	DDV_MinMaxInt(pDX, m_nCopies, 1, 100);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgArrayCopies, CDialog)
	//{{AFX_MSG_MAP(CDlgArrayCopies)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_NUMCOPIES, OnDeltaposSpinNumCopies)
	ON_EN_KILLFOCUS(IDC_EDIT_NUMCOPIES, OnKillfocusEditNumCopies)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgArrayCopies message handlers

void CDlgArrayCopies::OnDeltaposSpinNumCopies(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;

	// TRACE("Pos %d, Delta %d\n", pNMUpDown->iPos, pNMUpDown->iDelta);
	m_nCopies = pNMUpDown->iPos + pNMUpDown->iDelta;
	UpdateData(FALSE);
	
	*pResult = 0;
}

BOOL CDlgArrayCopies::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	CWnd* pEditWnd = GetDlgItem(IDC_EDIT_DENSITY);
	m_spinCopies.SetBuddy(pEditWnd);

	m_spinCopies.SetRange(0, 100);
	m_spinCopies.SetPos(m_nCopies);
	
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgArrayCopies::OnKillfocusEditNumCopies() 
{
	// TODO: Add your control notification handler code here
	
}

int CDlgArrayCopies::GetNumCopies()
{
	return m_nCopies;
}