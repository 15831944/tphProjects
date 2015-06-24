// PaxTransferDBDlg.cpp : implementation file
//

#include "stdafx.h"
#include "termplan.h"
#include "PaxTransferDBDlg.h"
#include "inputs\Pax_cnst.h"
#include "inputs\fltdist.h"
#include "FlightDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPaxTransferDBDlg dialog


CPaxTransferDBDlg::CPaxTransferDBDlg(ConstraintEntry* _pEntry, CWnd* pParent)
	: CDialog(CPaxTransferDBDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPaxTransferDBDlg)
	m_csPaxType = _T("");
	m_nPercentage = 0;
	//}}AFX_DATA_INIT
	m_pEntry = _pEntry;
}


void CPaxTransferDBDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPaxTransferDBDlg)
	DDX_Control(pDX, IDC_BUTTON_SET, m_btnSet);
	DDX_Control(pDX, IDC_STATIC_PERCENTAGE, m_staticPercentage);
	DDX_Control(pDX, IDC_SPIN_PERCENTAGE, m_spinPercentage);
	DDX_Control(pDX, IDC_EDIT_PERCENTAGE, m_editPercentage);
	DDX_Control(pDX, IDC_BUTTON_DELETE, m_btnDel);
	DDX_Control(pDX, IDC_LIST_FLTDISTR, m_listctrlFltDist);
	DDX_Text(pDX, IDC_EDIT_PAXTYPE, m_csPaxType);
	DDX_Text(pDX, IDC_EDIT_PERCENTAGE, m_nPercentage);
	DDV_MinMaxUInt(pDX, m_nPercentage, 0, 100);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPaxTransferDBDlg, CDialog)
	//{{AFX_MSG_MAP(CPaxTransferDBDlg)
	ON_BN_CLICKED(IDC_BUTTON_ADD, OnButtonAdd)
	ON_BN_CLICKED(IDC_BUTTON_DELETE, OnButtonDelete)
	ON_NOTIFY(NM_CLICK, IDC_LIST_FLTDISTR, OnClickListFltdistr)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_PERCENTAGE, OnDeltaposSpinPercentage)
	ON_BN_CLICKED(IDC_BUTTON_SET, OnButtonSet)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPaxTransferDBDlg message handlers

BOOL CPaxTransferDBDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	CPassengerConstraint* pPaxConstr = (CPassengerConstraint*)m_pEntry->getConstraint();
	//char buf[128];
	CString buf;
	pPaxConstr->screenPrint( buf );
	m_csPaxType.Format( "%s", buf.GetBuffer(0) );
	UpdateData( false );

	// INIT LIST CTRL
	DWORD dwStyle = m_listctrlFltDist.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_EX_GRIDLINES;
	m_listctrlFltDist.SetExtendedStyle( dwStyle );

	char* columnLabel[] = {	"Departing Flight", 
							"Percentage" };
	int DefaultColumnWidth[] = { 230, 100 };
	int nFormat[] = { LVCFMT_LEFT, LVCFMT_LEFT };


	for( int i=0; i<2; i++ )
		m_listctrlFltDist.InsertColumn( i, columnLabel[i], nFormat[i], DefaultColumnWidth[i] );


	ReloadFltDist(-1);
	
	m_spinPercentage.SetRange( 0, 100 );
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CPaxTransferDBDlg::ReloadFltDist(int _nDBIdx)
{
	m_listctrlFltDist.DeleteAllItems();
	FlightTypeDistribution* pDist = (FlightTypeDistribution*)m_pEntry->getValue();
	int nCount = pDist->getCount();
	unsigned char nPrevProb = 0;
	for( int i=0; i<nCount; i++ )
	{
		const FlightConstraint* pFltConstr = pDist->getGroup( i )->getFlightConstraint();
		char buf[128];
		CString pScreen;
		pFltConstr->screenPrint( pScreen );
		int nIdx = m_listctrlFltDist.InsertItem( i, pScreen.GetBuffer(0), 0 );
		int nProb = static_cast<int>(pDist->getProb( i ));
		sprintf( buf, "%d%%", nProb - nPrevProb );
		nPrevProb = nProb;
		m_listctrlFltDist.SetItemText( nIdx, 1, buf );
		m_listctrlFltDist.SetItemData( nIdx, i );
		if( i == _nDBIdx )
			m_listctrlFltDist.SetItemState( nIdx, LVIS_SELECTED, LVIS_SELECTED);
	}
	char str[128];
	sprintf( str, "Total = %d%%", nPrevProb );
	LVCOLUMN col;
	col.mask = LVCF_TEXT;
	col.pszText = str;
	m_listctrlFltDist.SetColumn( 1, &col );
	UpdateData( false );
	if( _nDBIdx >= 0 )
		m_btnDel.EnableWindow( true );
	else
		m_btnDel.EnableWindow( false );
}

void CPaxTransferDBDlg::OnButtonAdd() 
{
	// TODO: Add your control notification handler code here
	CFlightDialog dlg( m_pParentWnd );
	if( dlg.DoModal() == IDOK )
	{
		FlightTypeDistribution* pDist = (FlightTypeDistribution*)m_pEntry->getValue();

		// get new flt constraint
		FlightConstraint newFltConstr = dlg.GetFlightSelection();

		// find if exist
		int nCount = m_listctrlFltDist.GetItemCount();
		for( int i=0; i<nCount; i++ )
		{
			int nIdx = m_listctrlFltDist.GetItemData( i );
			const FlightConstraint* pFltConstr = pDist->getGroup( nIdx )->getFlightConstraint();

			if( newFltConstr.isEqual(pFltConstr) )
			{
				// select the item
				SelectFltDist( i );
				return;
			}
		}
		if( i == nCount )
		{
			FlightConstraint* pFltConstr = new FlightConstraint;
			CTransferFlightConstraint* pTransferFlight = new CTransferFlightConstraint;
			*pFltConstr = newFltConstr;
            pTransferFlight->setFlightConstriant(*pFltConstr);
			pDist->addRow( pTransferFlight, 0 );
			ReloadFltDist(-1);
		}
	}
}


void CPaxTransferDBDlg::SelectFltDist(int _nIdx)
{
	int nCount = m_listctrlFltDist.GetItemCount();
	for( int i=0; i<nCount; i++ )
	{
		if( i == _nIdx )
			m_listctrlFltDist.SetItemState( i, LVIS_SELECTED, LVIS_SELECTED );
		else
			m_listctrlFltDist.SetItemState( i, 0, LVIS_SELECTED ); 
	}
}

void CPaxTransferDBDlg::OnButtonDelete() 
{
	// TODO: Add your control notification handler code here
	int nCount = m_listctrlFltDist.GetItemCount();
	for( int i=0; i<nCount; i++ )
	{
		if( m_listctrlFltDist.GetItemState( i, LVIS_SELECTED ) == LVIS_SELECTED )
			break;
	}
	if( i == nCount )
		return;


	// get paxdb
	FlightTypeDistribution* pDist = (FlightTypeDistribution*)m_pEntry->getValue();
	int nIdx = m_listctrlFltDist.GetItemData( i );

	pDist->deleteRow( nIdx );

	ReloadFltDist( -1 );
	
}

void CPaxTransferDBDlg::OnClickListFltdistr(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	int nCount = m_listctrlFltDist.GetItemCount();
	for( int i=0; i<nCount; i++ )
	{
		if( m_listctrlFltDist.GetItemState( i, LVIS_SELECTED ) == LVIS_SELECTED )
			break;
	}
	bool bShow = false;
	if( i < nCount )
	{
		FlightTypeDistribution* pDist = (FlightTypeDistribution*)m_pEntry->getValue();
		int nIdx = m_listctrlFltDist.GetItemData( i );
		int nPrevProb = 0;
		if( nIdx > 0 )
			nPrevProb = static_cast<int>(pDist->getProb( nIdx - 1 ));
		m_nPercentage = static_cast<int>(pDist->getProb( nIdx )) - nPrevProb;
		UpdateData( false );
		bShow = true;
	}

	m_btnDel.EnableWindow( bShow );
	m_btnSet.ShowWindow( bShow );
	m_staticPercentage.ShowWindow( bShow );
	m_spinPercentage.ShowWindow( bShow );
	m_editPercentage.ShowWindow( bShow );
	
	*pResult = 0;
}

void CPaxTransferDBDlg::OnDeltaposSpinPercentage(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	// TODO: Add your control notification handler code here
	int	nLowerBound;
	int	nUpperBound;

	m_spinPercentage.GetRange( nLowerBound, nUpperBound );
	if( pNMUpDown->iDelta > 0 )
	{
		m_nPercentage = pNMUpDown->iPos + pNMUpDown->iDelta;
		m_nPercentage = static_cast<UINT>(m_nPercentage > static_cast<UINT>(nUpperBound) ? nUpperBound : m_nPercentage);
	}
	else if( pNMUpDown->iDelta < 0 && pNMUpDown->iPos > nLowerBound )
	{
		m_nPercentage = pNMUpDown->iPos + pNMUpDown->iDelta;
		m_nPercentage = static_cast<UINT>(m_nPercentage < static_cast<UINT>(nLowerBound) ? nLowerBound : m_nPercentage);
	}	
	*pResult = 0;
}

void CPaxTransferDBDlg::OnButtonSet() 
{
	// TODO: Add your control notification handler code here
	UpdateData();

	// set the db
	int nCount = m_listctrlFltDist.GetItemCount();
	for( int i=0; i<nCount; i++ )
	{
		if( m_listctrlFltDist.GetItemState( i, LVIS_SELECTED ) == LVIS_SELECTED )
			break;
	}
	if( i == nCount )
		return;

	FlightTypeDistribution* pDist = (FlightTypeDistribution*)m_pEntry->getValue();
	int nIdx = m_listctrlFltDist.GetItemData( i );
	pDist->setProb( m_nPercentage, nIdx );


	// reload gui
	ReloadFltDist( nIdx );	
}

void CPaxTransferDBDlg::OnOK() 
{
	// TODO: Add extra validation here
	FlightTypeDistribution* pDist = (FlightTypeDistribution*)m_pEntry->getValue();
	int nTotal = pDist->getLastOfProbs();
	if( nTotal != 100 )
	{
		MessageBox( "Total Percentage is not 100%%", "Error", MB_OK );
		return;
	}
	
	CDialog::OnOK();
}

