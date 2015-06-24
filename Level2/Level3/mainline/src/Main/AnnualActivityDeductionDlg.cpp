// AnnualActivityDeductionDlg.cpp : implementation file
//

#include "stdafx.h"
#include "termplan.h"
#include "AnnualActivityDeductionDlg.h"
#include "termplandoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAnnualActivityDeductionDlg dialog


CAnnualActivityDeductionDlg::CAnnualActivityDeductionDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAnnualActivityDeductionDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAnnualActivityDeductionDlg)
	m_nSel = -1;
	m_nOcPerc1 = 0;
	m_nOcPerc2 = 0;
	m_nWDPerc = 0;
	//}}AFX_DATA_INIT
}


void CAnnualActivityDeductionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAnnualActivityDeductionDlg)
	DDX_Control(pDX, IDC_SPIN_WD, m_spinWeekday);
	DDX_Control(pDX, IDC_SPIN_OCCASIONALY2, m_spinOccasionaly2);
	DDX_Control(pDX, IDC_SPIN_OCCASIONALY1, m_spinOccasionaly1);
	DDX_Control(pDX, IDC_EDIT_WD, m_editWeekday);
	DDX_Control(pDX, IDC_EDIT_OCCASIONALY2, m_editOccasionaly2);
	DDX_Control(pDX, IDC_EDIT_OCCASIONALY1, m_editOccasionaly1);
	DDX_Control(pDX, IDC_BUTTON_SAVE, m_btnSave);
	DDX_Radio(pDX, IDC_RADIO_EVERYDAY, m_nSel);
	DDX_Text(pDX, IDC_EDIT_OCCASIONALY1, m_nOcPerc1);
	DDV_MinMaxInt(pDX, m_nOcPerc1, 0, 100);
	DDX_Text(pDX, IDC_EDIT_OCCASIONALY2, m_nOcPerc2);
	DDV_MinMaxInt(pDX, m_nOcPerc2, 0, 100);
	DDX_Text(pDX, IDC_EDIT_WD, m_nWDPerc);
	DDV_MinMaxInt(pDX, m_nWDPerc, 0, 100);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAnnualActivityDeductionDlg, CDialog)
	//{{AFX_MSG_MAP(CAnnualActivityDeductionDlg)
	ON_BN_CLICKED(IDC_RADIO_EVERYDAY, OnRadioEveryday)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_OCCASIONALY1, OnDeltaposSpinOccasionaly1)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_OCCASIONALY2, OnDeltaposSpinOccasionaly2)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_WD, OnDeltaposSpinWd)
	ON_BN_CLICKED(IDC_RADIO_WEEKDAY, OnRadioEveryday)
	ON_BN_CLICKED(IDC_RADIO_OCCASIONALY, OnRadioEveryday)
	ON_BN_CLICKED(IDC_BUTTON_SAVE, OnButtonSave)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAnnualActivityDeductionDlg message handlers


void CAnnualActivityDeductionDlg::SetControl()
{
	switch( m_nSel )
	{
	case 1:
		EnableWeekdayOption( TRUE );
		EnableOccasionalyOption( FALSE );
		break;
	case 2:
		EnableWeekdayOption( FALSE );
		EnableOccasionalyOption( TRUE );
		break;
	default:
		EnableWeekdayOption( FALSE );
		EnableOccasionalyOption( FALSE );
		break;

	}
}

void CAnnualActivityDeductionDlg::OnRadioEveryday() 
{
	// TODO: Add your control notification handler code here
	UpdateData();

	SetDatabase();

	SetControl();
	
	m_btnSave.EnableWindow();
}

void CAnnualActivityDeductionDlg::SetDatabase()
{
	CEconomicManager* pEconMan = GetEconomicManager();
	enum ENUM_ANNUALACTIVITYDED enumType = pEconMan->m_pAnnualActivityDeduction->GetEnumType();

	switch( m_nSel )
	{
	case 0:
		pEconMan->m_pAnnualActivityDeduction->SetEnumType( ENUM_EVERYDAY );
		break;
	case 1:
		pEconMan->m_pAnnualActivityDeduction->SetEnumType( ENUM_WEEKDAY );
		pEconMan->m_pAnnualActivityDeduction->SetPercent1( m_nWDPerc );
		break;
	case 2:
		pEconMan->m_pAnnualActivityDeduction->SetEnumType( ENUM_OCCASIONALY );
		pEconMan->m_pAnnualActivityDeduction->SetPercent1( m_nOcPerc1 );
		pEconMan->m_pAnnualActivityDeduction->SetPercent2( m_nOcPerc2 );
		break;
	}
}

void CAnnualActivityDeductionDlg::EnableWeekdayOption( BOOL _bEnable )
{
	m_editWeekday.EnableWindow( _bEnable );
	m_spinWeekday.EnableWindow( _bEnable );
}


void CAnnualActivityDeductionDlg::EnableOccasionalyOption( BOOL _bEnable )
{
	m_spinOccasionaly2.EnableWindow( _bEnable );
	m_spinOccasionaly1.EnableWindow( _bEnable );
	
	m_editOccasionaly1.EnableWindow( _bEnable );
	m_editOccasionaly2.EnableWindow( _bEnable );

}

BOOL CAnnualActivityDeductionDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here

	m_spinWeekday.SetRange( 0, 100 );
	m_spinOccasionaly2.SetRange( 0, 100 );
	m_spinOccasionaly1.SetRange( 0, 100 );

	ReloadDatabase();

	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CAnnualActivityDeductionDlg::ReloadDatabase()
{
	CEconomicManager* pEconMan = GetEconomicManager();
	enum ENUM_ANNUALACTIVITYDED enumType = pEconMan->m_pAnnualActivityDeduction->GetEnumType();
	switch( enumType )
	{
	case ENUM_EVERYDAY:
		m_nSel = 0;
		break;
	case ENUM_WEEKDAY:
		m_nSel = 1;
		m_nWDPerc = pEconMan->m_pAnnualActivityDeduction->GetPercent1();
		break;
	case ENUM_OCCASIONALY:
		m_nSel = 2;
		m_nOcPerc1 = pEconMan->m_pAnnualActivityDeduction->GetPercent1();
		m_nOcPerc2 = pEconMan->m_pAnnualActivityDeduction->GetPercent2();
		break;
	}

	UpdateData( FALSE );
	SetControl();

}


void CAnnualActivityDeductionDlg::OnDeltaposSpinOccasionaly1(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	// TODO: Add your control notification handler code here
	int	nLowerBound;
	int	nUpperBound;

	m_spinOccasionaly1.GetRange( nLowerBound, nUpperBound );
	if( pNMUpDown->iDelta > 0 )
	{
		m_nOcPerc1 = pNMUpDown->iPos + pNMUpDown->iDelta;
		m_nOcPerc1 = m_nOcPerc1 > nUpperBound ? nUpperBound : m_nOcPerc1;
	}
	else if( pNMUpDown->iDelta < 0 && pNMUpDown->iPos > nLowerBound )
	{
		m_nOcPerc1 = pNMUpDown->iPos + pNMUpDown->iDelta;
		m_nOcPerc1 = m_nOcPerc1 < nLowerBound ? nLowerBound : m_nOcPerc1;
	}	
	*pResult = 0;	
	m_btnSave.EnableWindow();
	
	SetDatabase();
	*pResult = 0;
}

void CAnnualActivityDeductionDlg::OnDeltaposSpinOccasionaly2(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	// TODO: Add your control notification handler code here
	int	nLowerBound;
	int	nUpperBound;

	m_spinOccasionaly2.GetRange( nLowerBound, nUpperBound );
	if( pNMUpDown->iDelta > 0 )
	{
		m_nOcPerc2 = pNMUpDown->iPos + pNMUpDown->iDelta;
		m_nOcPerc2 = m_nOcPerc2 > nUpperBound ? nUpperBound : m_nOcPerc2;
	}
	else if( pNMUpDown->iDelta < 0 && pNMUpDown->iPos > nLowerBound )
	{
		m_nOcPerc2 = pNMUpDown->iPos + pNMUpDown->iDelta;
		m_nOcPerc2 = m_nOcPerc2 < nLowerBound ? nLowerBound : m_nOcPerc2;
	}	
	*pResult = 0;	
	m_btnSave.EnableWindow();
	SetDatabase();
	
	*pResult = 0;
}

void CAnnualActivityDeductionDlg::OnDeltaposSpinWd(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	// TODO: Add your control notification handler code here
	int	nLowerBound;
	int	nUpperBound;

	m_spinWeekday.GetRange( nLowerBound, nUpperBound );
	if( pNMUpDown->iDelta > 0 )
	{
		m_nWDPerc = pNMUpDown->iPos + pNMUpDown->iDelta;
		m_nWDPerc = m_nWDPerc > nUpperBound ? nUpperBound : m_nWDPerc;
	}
	else if( pNMUpDown->iDelta < 0 && pNMUpDown->iPos > nLowerBound )
	{
		m_nWDPerc = pNMUpDown->iPos + pNMUpDown->iDelta;
		m_nWDPerc = m_nWDPerc < nLowerBound ? nLowerBound : m_nWDPerc;
	}	
	*pResult = 0;	
	m_btnSave.EnableWindow();
	SetDatabase();
	
	*pResult = 0;
}


CEconomicManager* CAnnualActivityDeductionDlg::GetEconomicManager()
{
	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();	
    return (CEconomicManager*)&pDoc->GetEconomicManager();
}


CString CAnnualActivityDeductionDlg::GetProjPath()
{
	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();	
	return pDoc->m_ProjInfo.path;
}


void CAnnualActivityDeductionDlg::OnButtonSave() 
{
	// TODO: Add your control notification handler code here
	CEconomicManager* pEconMan = GetEconomicManager();
	pEconMan->m_pAnnualActivityDeduction->saveDataSet( GetProjPath(), true );
	m_btnSave.EnableWindow( FALSE );
	
}

void CAnnualActivityDeductionDlg::OnOK() 
{
	// TODO: Add extra validation here
	if( m_btnSave.IsWindowEnabled() )
	{
		CEconomicManager* pEconMan = GetEconomicManager();
		pEconMan->m_pAnnualActivityDeduction->saveDataSet( GetProjPath(), true );
	}
	
	CDialog::OnOK();
}

void CAnnualActivityDeductionDlg::OnCancel() 
{
	// TODO: Add extra cleanup here
	if( m_btnSave.IsWindowEnabled() )
	{
		CEconomicManager* pEconMan = GetEconomicManager();
		try
		{
			pEconMan->m_pAnnualActivityDeduction->loadDataSet( GetProjPath() );
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
