// CapitalBDDecisionDlg.cpp : implementation file
//

#include "stdafx.h"
#include "termplan.h"
#include "CapitalBDDecisionDlg.h"
#include "termplandoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCapitalBDDecisionDlg dialog


CCapitalBDDecisionDlg::CCapitalBDDecisionDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCapitalBDDecisionDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCapitalBDDecisionDlg)
	m_nDiscountRate = 0;
	m_nProjectLife = 1;
	m_nDiscOptions = 0;
	//}}AFX_DATA_INIT
}


void CCapitalBDDecisionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCapitalBDDecisionDlg)
	DDX_Control(pDX, IDC_EDIT_DISCRATE, m_editDiscountRate);
	DDX_Control(pDX, IDC_BUTTON_SAVE, m_btnSave);
	DDX_Control(pDX, IDC_SPIN_LIFEPROJ, m_spinProjLife);
	DDX_Control(pDX, IDC_SPIN_DISCRATE, m_spinDiscRate);
	DDX_Text(pDX, IDC_EDIT_DISCRATE, m_nDiscountRate);
	DDV_MinMaxInt(pDX, m_nDiscountRate, 0, 100);
	DDX_Text(pDX, IDC_EDIT_LIFEPROJ, m_nProjectLife);
	DDV_MinMaxInt(pDX, m_nProjectLife, 1, 100);
	DDX_Radio(pDX, IDC_RADIO_DISCOUNTRATE, m_nDiscOptions);
	DDX_Control(pDX,IDC_EDIT_LIFEPROJ,m_editProjectLife);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCapitalBDDecisionDlg, CDialog)
	//{{AFX_MSG_MAP(CCapitalBDDecisionDlg)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_DISCRATE, OnDeltaposSpinDiscrate)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_LIFEPROJ, OnDeltaposSpinLifeproj)
	ON_BN_CLICKED(IDC_BUTTON_SAVE, OnButtonSave)
	ON_BN_CLICKED(IDC_RADIO_DISCOUNTRATE, OnRadioDiscountrate)
	ON_BN_CLICKED(IDC_RADIO_OPTIONS2, OnRadioDiscountrate)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCapitalBDDecisionDlg message handlers

void CCapitalBDDecisionDlg::OnDeltaposSpinDiscrate(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	// TODO: Add your control notification handler code here
	int	nLowerBound;
	int	nUpperBound;

	m_spinDiscRate.GetRange( nLowerBound, nUpperBound );
	if( pNMUpDown->iDelta > 0 )
	{
		m_nDiscountRate = pNMUpDown->iPos + pNMUpDown->iDelta;
		m_nDiscountRate = m_nDiscountRate > nUpperBound ? nUpperBound : m_nDiscountRate;
	}
	else if( pNMUpDown->iDelta < 0 && pNMUpDown->iPos > nLowerBound )
	{
		m_nDiscountRate = pNMUpDown->iPos + pNMUpDown->iDelta;
		m_nDiscountRate = m_nDiscountRate < nLowerBound ? nLowerBound : m_nDiscountRate;
	}	
	*pResult = 0;	
	m_btnSave.EnableWindow();	
}

void CCapitalBDDecisionDlg::OnDeltaposSpinLifeproj(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	// TODO: Add your control notification handler code here
	int	nLowerBound;
	int	nUpperBound;

	m_spinProjLife.GetRange( nLowerBound, nUpperBound );
	if( pNMUpDown->iDelta > 0 )
	{
		m_nProjectLife = pNMUpDown->iPos + pNMUpDown->iDelta;
		m_nProjectLife = m_nProjectLife > nUpperBound ? nUpperBound : m_nProjectLife;
	}
	else if( pNMUpDown->iDelta < 0 && pNMUpDown->iPos > nLowerBound )
	{
		m_nProjectLife = pNMUpDown->iPos + pNMUpDown->iDelta;
		m_nProjectLife = m_nProjectLife < nLowerBound ? nLowerBound : m_nProjectLife;
	}	
	*pResult = 0;	
	m_btnSave.EnableWindow();	
}

BOOL CCapitalBDDecisionDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	m_spinDiscRate.SetRange( 0, 100 );
	m_spinProjLife.SetRange( 1, 100 );
	
	LoadDatabase();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CCapitalBDDecisionDlg::OnButtonSave() 
{
	// TODO: Add your control notification handler code here
	CWaitCursor wc;
	UpdateData();
	CCapitalBudgDecisionsDatabase* pCapitalBudgDecisions = GetEconomicManager()->m_pCapitalBudgDecisions;
	if( m_nDiscOptions == 0 )
		pCapitalBudgDecisions->SetDiscountRate( -1 );
	else
		pCapitalBudgDecisions->SetDiscountRate( m_nDiscountRate );

	pCapitalBudgDecisions->SetProjLife( m_nProjectLife );
	pCapitalBudgDecisions->saveDataSet( GetProjPath(), true );
	m_btnSave.EnableWindow( FALSE );
	
}

void CCapitalBDDecisionDlg::OnOK() 
{
	// TODO: Add extra validation here
	if( m_btnSave.IsWindowEnabled() )
	{
		OnButtonSave();
	}
	
	CDialog::OnOK();
}

void CCapitalBDDecisionDlg::OnCancel() 
{
	// TODO: Add extra cleanup here
	if( m_btnSave.IsWindowEnabled() )
	{
		CWaitCursor wc;
		CEconomicManager* pEconMan = GetEconomicManager();
		try
		{
			pEconMan->m_pCapitalBudgDecisions->loadDataSet( GetProjPath() );
		}
		catch( FileVersionTooNewError* _pError )
		{
			char szBuf[128];
			_pError->getMessage( szBuf );
			MessageBox( szBuf, "Error", MB_OK|MB_ICONWARNING );
			delete _pError;			
		}
	}	
	
	CDialog::OnCancel();
}

void CCapitalBDDecisionDlg::LoadDatabase()
{
	CEconomicManager* pEconMan = GetEconomicManager();
	int nDiscRate = pEconMan->m_pCapitalBudgDecisions->GetDiscountRate();
	if( nDiscRate == -1 )
	{
		m_nDiscOptions = 0;
		m_nDiscountRate = 0;
		m_editDiscountRate.EnableWindow( FALSE );
		m_spinDiscRate.EnableWindow( FALSE );
	}
	else
	{
		m_nDiscOptions = 1;
		m_nDiscountRate = nDiscRate;
		m_editDiscountRate.EnableWindow();
		m_spinDiscRate.EnableWindow();
	}
	
	m_nProjectLife = pEconMan->m_pCapitalBudgDecisions->GetProjLife();
	UpdateData( FALSE );
}


CEconomicManager* CCapitalBDDecisionDlg::GetEconomicManager()
{
	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();	
    return (CEconomicManager*)&pDoc->GetEconomicManager();
}

CString CCapitalBDDecisionDlg::GetProjPath()
{
	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();	
	return pDoc->m_ProjInfo.path;
}

void CCapitalBDDecisionDlg::OnRadioDiscountrate() 
{
	// TODO: Add your control notification handler code here
	UpdateData();
	BOOL nEnable = ( m_nDiscOptions == 1 );
	m_editDiscountRate.EnableWindow( nEnable );
	m_spinDiscRate.EnableWindow( nEnable );

	m_btnSave.EnableWindow();	
}

