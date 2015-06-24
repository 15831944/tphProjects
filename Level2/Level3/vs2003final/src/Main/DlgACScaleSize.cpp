// DlgACScaleSize.cpp : implementation file
//

#include "stdafx.h"
#include "TermPlan.h"
#include "DlgACScaleSize.h"
#include ".\dlgacscalesize.h"


// CDlgACScaleSize dialog

IMPLEMENT_DYNAMIC(CDlgACScaleSize, CDialog)
CDlgACScaleSize::CDlgACScaleSize(CWnd* pParent /*=NULL*/,int fScaleRange)
	: CDialog(CDlgACScaleSize::IDD, pParent)
	, m_fScaleSize(1)
	, m_nScaleRange(fScaleRange)
{
}

CDlgACScaleSize::~CDlgACScaleSize()
{
}

void CDlgACScaleSize::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_ACSCALESIZE, m_editScale);
	DDX_Text(pDX, IDC_EDIT_ACSCALESIZE, m_fScaleSize);
	DDX_Control(pDX, IDC_SPIN_ACSCALESIZE, m_spinScale);
}


BEGIN_MESSAGE_MAP(CDlgACScaleSize, CDialog)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_SCALESIZE, OnDeltaPosSpinDensity)
END_MESSAGE_MAP()


// CDlgACScaleSize message handlers
// DlgACScaleSize.cpp : implementation file
//
void CDlgACScaleSize::OnDeltaPosSpinDensity(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;

	// TRACE("Pos %d, Delta %d\n", pNMUpDown->iPos, pNMUpDown->iDelta);
	m_fScaleSize =float( pNMUpDown->iPos + pNMUpDown->iDelta);
	if(m_fScaleSize<1) m_fScaleSize= 1;
	if(m_fScaleSize>m_nScaleRange) m_fScaleSize = (float)m_nScaleRange;
	UpdateData(FALSE);

	*pResult = 0;
}


BOOL CDlgACScaleSize::OnInitDialog()
{
	CDialog::OnInitDialog();

	CWnd* pEditWnd = GetDlgItem(IDC_EDIT_ACSCALESIZE);
	m_spinScale.SetBuddy(pEditWnd);

	m_spinScale.SetRange(1,m_nScaleRange);
// 	int _scale = int(m_fScaleSize);
// 	m_spinScale.SetPos(_scale);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
