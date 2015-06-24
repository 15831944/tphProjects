// OptionsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "termplan.h"
#include "OptionsDlg.h"
#include "UndoOptionHander.h"
#include "AutomaticSaveTimer.h"
#include "TermPlanDoc.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#include "AutoSaveHandler.h"
extern const CString c_setting_regsectionstring;
extern const CString c_undolength_entry;

/////////////////////////////////////////////////////////////////////////////
// COptionsDlg dialog


COptionsDlg::COptionsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(COptionsDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(COptionsDlg)
	m_nVal = 10;
	m_Timer = 10 ;
	//}}AFX_DATA_INIT
}


void COptionsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COptionsDlg)
	DDX_Control(pDX, IDC_SPIN_VAL, m_spinVal);

	DDX_Text(pDX, IDC_EDIT_VAL, m_nVal);
	DDV_MinMaxInt(pDX, m_nVal, 0, 1000);
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_SPIN_TIMER, m_SpinTimer);
	DDX_Text(pDX,IDC_EDIT_TIME,m_Timer) ;
	DDV_MinMaxInt(pDX, m_Timer, 0, 1000);
}


BEGIN_MESSAGE_MAP(COptionsDlg, CDialog)
	//{{AFX_MSG_MAP(COptionsDlg)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_VAL, OnDeltaposSpinVal)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_TIMER, OnDeltaposSpinTimer)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COptionsDlg message handlers

BOOL COptionsDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	m_spinVal.SetRange( 0, 1000 );
	m_SpinTimer.SetRange(0,1000) ;
	if( !CUndoOptionHander::ReadData(m_nVal,m_Timer) )
	{
		m_nVal = 10 ;
		m_Timer = 10 ;
	}
	UpdateData( FALSE );
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void COptionsDlg::OnDeltaposSpinVal(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	// TODO: Add your control notification handler code here
	int	nLowerBound;
	int	nUpperBound;

	m_spinVal.GetRange( nLowerBound, nUpperBound );
	if( pNMUpDown->iDelta > 0 )
	{
		m_nVal = pNMUpDown->iPos + pNMUpDown->iDelta;
		m_nVal = m_nVal > nUpperBound ? nUpperBound : m_nVal;
	}
	else if( pNMUpDown->iDelta < 0 && pNMUpDown->iPos > nLowerBound )
	{
		m_nVal = pNMUpDown->iPos + pNMUpDown->iDelta;
		m_nVal = m_nVal < nLowerBound ? nLowerBound : m_nVal;
	}	
	*pResult = 0;
}
void COptionsDlg::OnDeltaposSpinTimer(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	// TODO: Add your control notification handler code here
	int	nLowerBound;
	int	nUpperBound;

	m_SpinTimer.GetRange( nLowerBound, nUpperBound );
	if( pNMUpDown->iDelta > 0 )
	{
		m_Timer = pNMUpDown->iPos + pNMUpDown->iDelta;
		m_Timer = m_Timer > nUpperBound ? nUpperBound : m_Timer;
	}
	else if( pNMUpDown->iDelta < 0 && pNMUpDown->iPos > nLowerBound )
	{
		m_Timer = pNMUpDown->iPos + pNMUpDown->iDelta;
		m_Timer = m_Timer < nLowerBound ? nLowerBound : m_Timer;
	}	
	*pResult = 0;
}
void COptionsDlg::OnOK() 
{
	// TODO: Add extra validation here
    try
    {
		CADODatabase::BeginTransaction(DATABASESOURCE_TYPE_ACCESS_GLOBALDB) ;
        CUndoOptionHander::SaveData(m_nVal,m_Timer) ;
		CADODatabase::CommitTransaction(DATABASESOURCE_TYPE_ACCESS_GLOBALDB) ;
    }
    catch (CADOException e)
    {
		CADODatabase::RollBackTransation(DATABASESOURCE_TYPE_ACCESS_GLOBALDB) ;
    }
	
	CAutoSaveHandler::GetInstance()->SetTimeInterval(m_Timer) ;
	CDialog::OnOK();
}
