// PaxFilterDlg.cpp : implementation file
//

#include "stdafx.h"
#include "termplan.h"
#include "PaxFilterDlg.h"
#include "TermPlanDoc.h"
#include "PassengerTypeDialog.h"
#include ".\paxfilterdlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPaxFilterDlg dialog


CPaxFilterDlg::CPaxFilterDlg(CWnd* pParent /*=NULL*/)
: CDialog(CPaxFilterDlg::IDD, pParent)
, m_bInitilized(FALSE)
{
	//{{AFX_DATA_INIT(CPaxFilterDlg)
	m_csName = _T("");
	m_csExpre = _T("\"DEFAULT\"");
	m_csPaxCon = _T("DEFAULT");
	//}}AFX_DATA_INIT
}


void CPaxFilterDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPaxFilterDlg)
	DDX_Control(pDX, IDC_CHECK_NOTPAXCON, m_btnNotPaxCon);
	DDX_Control(pDX, IDC_CHECK_NOTDEATHTIME, m_btnNotDeathTime);
	DDX_Control(pDX, IDC_CHECK_NOTBIRTHTIME, m_btnNotBirthTime);
	DDX_Control(pDX, IDC_CHECK_NOTAREA, m_btnNotArea);
	DDX_Control(pDX, IDC_DATETIMEPICKER_DEATHSTART, m_dtCtrlDeathStart);
	DDX_Control(pDX, IDC_DATETIMEPICKER_DEATHEND, m_dtCtrlDeathEnd);
	DDX_Control(pDX, IDC_DATETIMEPICKER_BIRTHSTART, m_dtCtrlBirthStart);
	DDX_Control(pDX, IDC_DATETIMEPICKER_BIRTHEND, m_dtCtrlBirthEnd);
	DDX_Control(pDX, IDC_CHECK_DEATH, m_btnDeathTime);
	DDX_Control(pDX, IDC_CHECK_BIRTH, m_btnBirthTime);
	DDX_Control(pDX, IDC_RADIO_PORTAL, m_radioPortal);
	DDX_Control(pDX, IDC_RADIO_AREA, m_radioArea);
	DDX_Control(pDX, IDC_DATETIMEPICKER_AREAEND, m_dtCtrlAreaEnd);
	DDX_Control(pDX, IDC_DATETIMEPICKER_AREASTART, m_dtCtrlAreaStart);
	DDX_Control(pDX, IDC_CHECK_APPLIED, m_btnApplied);
	DDX_Control(pDX, IDC_COMBO_AREAPORTAL, m_comboAreaPortal);
	DDX_Control(pDX, IDC_CHECK_AREAPORTAL, m_btnAreaPortal);
	DDX_Text(pDX, IDC_EDIT_NAME, m_csName);
	DDX_Text(pDX, IDC_STATIC_EXPRE, m_csExpre);
	DDX_Text(pDX, IDC_EDIT_PAXCON, m_csPaxCon);
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_SPIN_AREASTARTDAY, m_spinAreaStartDay);
	DDX_Control(pDX, IDC_SPIN_AREAENDDAY, m_spinAreaEndDay);
	DDX_Control(pDX, IDC_SPIN_BIRTHSTARTDAY, m_spinBirthStartDay);
	DDX_Control(pDX, IDC_SPIN_BIRTHENDDAY, m_spinBirthEndDay);
	DDX_Control(pDX, IDC_SPIN_DEATHSTARTDAY, m_spinDeathStartDay);
	DDX_Control(pDX, IDC_SPIN_DEATHENDDAY, m_spinDeathEndDay);
	DDX_Control(pDX, IDC_EDIT_AREASTARTDAY, m_editAreaStartDay);
	DDX_Control(pDX, IDC_EDIT_AREAENDDAY, m_editAreaEndDay);
	DDX_Control(pDX, IDC_EDIT_BIRTHSTARTDAY, m_editBirthStartDay);
	DDX_Control(pDX, IDC_EDIT_BIRTHENDDAY, m_editBirthEndDay);
	DDX_Control(pDX, IDC_EDIT_DEATHSTARTDAY, m_editDeathStartDay);
	DDX_Control(pDX, IDC_EDIT_DEATHENDDAY, m_editDeathEndDay);
}


BEGIN_MESSAGE_MAP(CPaxFilterDlg, CDialog)
//{{AFX_MSG_MAP(CPaxFilterDlg)
ON_BN_CLICKED(IDC_CHECK_AREAPORTAL, OnCheckAreaportal)
ON_BN_CLICKED(IDC_CHECK_BIRTH, OnCheckBirth)
ON_BN_CLICKED(IDC_CHECK_DEATH, OnCheckDeath)
ON_BN_CLICKED(IDC_CHECK_NOTAREA, OnCheckNotarea)
ON_BN_CLICKED(IDC_CHECK_NOTBIRTHTIME, OnCheckNotbirthtime)
ON_BN_CLICKED(IDC_CHECK_NOTDEATHTIME, OnCheckNotdeathtime)
ON_BN_CLICKED(IDC_CHECK_APPLIED, OnCheckApplied)
ON_NOTIFY(DTN_DATETIMECHANGE, IDC_DATETIMEPICKER_AREASTART, OnDatetimechangeDatetimepickerAreastart)
ON_NOTIFY(DTN_DATETIMECHANGE, IDC_DATETIMEPICKER_AREAEND, OnDatetimechangeDatetimepickerAreaend)
ON_NOTIFY(DTN_DATETIMECHANGE, IDC_DATETIMEPICKER_BIRTHEND, OnDatetimechangeDatetimepickerBirthend)
ON_NOTIFY(DTN_DATETIMECHANGE, IDC_DATETIMEPICKER_BIRTHSTART, OnDatetimechangeDatetimepickerBirthstart)
ON_NOTIFY(DTN_DATETIMECHANGE, IDC_DATETIMEPICKER_DEATHEND, OnDatetimechangeDatetimepickerDeathend)
ON_NOTIFY(DTN_DATETIMECHANGE, IDC_DATETIMEPICKER_DEATHSTART, OnDatetimechangeDatetimepickerDeathstart)
ON_BN_CLICKED(IDC_RADIO_AREA, OnRadioArea)
ON_BN_CLICKED(IDC_RADIO_PORTAL, OnRadioPortal)
ON_BN_CLICKED(IDC_BUTTON_PAXCON, OnButtonPaxcon)
ON_BN_CLICKED(IDC_CHECK_NOTPAXCON, OnCheckNotpaxcon)
ON_CBN_CLOSEUP(IDC_COMBO_AREAPORTAL, OnCloseupComboAreaportal)
	ON_EN_KILLFOCUS(IDC_EDIT_NAME, OnKillfocusEditName)
	//}}AFX_MSG_MAP
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_AREASTARTDAY, OnDeltaposSpinAreastartday)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_AREAENDDAY, OnDeltaposSpinAreaendday)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_BIRTHSTARTDAY, OnDeltaposSpinBirthstartday)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_BIRTHENDDAY, OnDeltaposSpinBirthendday)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_DEATHSTARTDAY, OnDeltaposSpinDeathstartday)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_DEATHENDDAY, OnDeltaposSpinDeathendday)
	ON_EN_CHANGE(IDC_EDIT_AREASTARTDAY, OnEnChangeEditAreastartday)
	ON_EN_CHANGE(IDC_EDIT_AREAENDDAY, OnEnChangeEditAreaendday)
	ON_EN_CHANGE(IDC_EDIT_BIRTHSTARTDAY, OnEnChangeEditBirthstartday)
	ON_EN_CHANGE(IDC_EDIT_BIRTHENDDAY, OnEnChangeEditBirthendday)
	ON_EN_CHANGE(IDC_EDIT_DEATHSTARTDAY, OnEnChangeEditDeathstartday)
	ON_EN_CHANGE(IDC_EDIT_DEATHENDDAY, OnEnChangeEditDeathendday)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPaxFilterDlg message handlers

BOOL CPaxFilterDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	// init all the spin edit
	m_spinAreaStartDay.SetRange32(1, 366);
	m_spinAreaStartDay.SetPos32(1);

	m_spinAreaEndDay.SetRange32(1, 366);
	m_spinAreaEndDay.SetPos32(1);

	m_spinBirthStartDay.SetRange32(1, 366);
	m_spinBirthStartDay.SetPos32(1);

	m_spinBirthEndDay.SetRange32(1, 366);
	m_spinBirthEndDay.SetPos32(1);

	m_spinDeathStartDay.SetRange32(1, 366);
	m_spinDeathStartDay.SetPos32(1);

	m_spinDeathEndDay.SetRange32(1, 366);
	m_spinDeathEndDay.SetPos32(1);

	
	/*	// TODO: Add extra initialization here
	HTREEITEM hRootItem = m_treeItem.InsertItem( m_paxType.GetName() );
	char szBuf[128];
	m_paxType.GetPaxCon().screenPrint( szBuf, 128 );
	HTREEITEM hItem = m_treeItem.InsertItem( CString( szBuf ), hRootItem );
	HTREEITEM hAORPItem = m_treeItem.InsertItem( "Area or Portal", hRootItem );
	hItem = m_treeItem.InsertItem( "Area", hAORPItem );
	hItem = m_treeItem.InsertItem( "Potal", hAORPItem );
	hItem = m_treeItem.InsertItem( "Time Range", hAORPItem );
	
	  m_treeItem.Expand( hAORPItem, TVE_EXPAND );
	  
		hItem = m_treeItem.InsertItem( "Birth Time", hRootItem );
		hItem = m_treeItem.InsertItem( "Death Time", hRootItem );
		
	m_treeItem.Expand( hRootItem, TVE_EXPAND ); */

	//give m_paxType a default value,and set Inputterminal for it
	//CMobileElemConstraint tmpCon;
	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();	
	m_paxType.GetPaxCon().SetInputTerminal( (InputTerminal*)&pDoc->GetTerminal() );
	//tmpCon.SetInputTerminal( (InputTerminal*)&pDoc->GetTerminal() );
	//m_paxType.SetPaxCon( tmpCon );
	//////////////////////////////////////////////////////////////////
	
	if( m_paxType.GetName() == "Undefined" )
	{
		EnableAreaPortalControl( FALSE );
		EnableBirthTimeControl( FALSE );
		EnableDeathTimeControl( FALSE );
		
	}
	else
	{
		m_csName = m_paxType.GetName();

		char buf[128];
		m_paxType.GetPaxCon().screenPrint( buf );
		//m_csPaxCon = newPaxCon
		m_csPaxCon = buf;

		if( m_paxType.GetIsPaxConAdd() )
			m_btnNotPaxCon.SetCheck( 0 );
		else
			m_btnNotPaxCon.SetCheck( 1 );

		CArea area;
		CPortal portal;
		int nIdx;
		COleDateTime dt;
		// ENUM_APPLY_NONE, ENUM_APPLY_AREA, ENUM_APPLY_PORTAL
		AREA_PORTAL_APPLY_TYPE type = m_paxType.GetAreaportalApplyType();
		switch( type )
		{
		case ENUM_APPLY_AREA:
			m_btnAreaPortal.SetCheck( 1 );
			if( m_paxType.GetIsAreaPortalAdd() )
				m_btnNotArea.SetCheck( 0 );
			else
				m_btnNotArea.SetCheck( 1 );
			m_radioArea.SetCheck( 1 );
			LoadArea();
			area = m_paxType.GetArea();
			nIdx = m_comboAreaPortal.FindString( -1,area.name );
			m_comboAreaPortal.SetCurSel( nIdx );
			if( m_paxType.GetIsTimeApplied() )
			{
				m_btnApplied.SetCheck( 1 );

				int day = 0;
				dt = m_paxType.GetStartTime();
				day = (int)(dt.m_dt);
				m_spinAreaStartDay.SetPos32(day);
				dt.m_dt -= day;
				m_dtCtrlAreaStart.SetTime( dt );

				dt = m_paxType.GetEndTime();
				day = (int)(dt.m_dt);
				m_spinAreaEndDay.SetPos32(day);
				dt.m_dt -= day;
				m_dtCtrlAreaEnd.SetTime( dt );
			}
			else
			{
				m_btnApplied.SetCheck( 0 );
			}
			break;
		case ENUM_APPLY_PORTAL:
			m_btnAreaPortal.SetCheck( 1 );
			if( m_paxType.GetIsAreaPortalAdd() )
				m_btnNotArea.SetCheck( 0 );
			else
				m_btnNotArea.SetCheck( 1 );
			m_radioPortal.SetCheck( 1 );
			LoadPortal();
			portal = m_paxType.GetPortal();
			nIdx = m_comboAreaPortal.FindString( -1,portal.name );
			m_comboAreaPortal.SetCurSel( nIdx );
			if( m_paxType.GetIsTimeApplied() )
			{
				m_btnApplied.SetCheck( 1 );

				int day = 0;
				dt = m_paxType.GetStartTime();
				day = (int)(dt.m_dt);
				m_spinAreaStartDay.SetPos32(day);
				dt.m_dt -= day;
				m_dtCtrlAreaStart.SetTime( dt );

				dt = m_paxType.GetEndTime();
				day = (int)(dt.m_dt);
				m_spinAreaEndDay.SetPos32(day);
				dt.m_dt -= day;
				m_dtCtrlAreaEnd.SetTime( dt );
			}
			else
			{
				m_btnApplied.SetCheck( 0 );
			}
			break;
		default: /*  ENUM_APPLY_NONE  */
			m_btnAreaPortal.SetCheck( 0 );
			EnableAreaPortalControl( FALSE );
		}
		
		if( m_paxType.GetIsBirthTimeApplied() )
		{
			m_btnBirthTime.SetCheck( 1 );
			if( m_paxType.GetIsBirthTimeAdd() )
				m_btnNotBirthTime.SetCheck( 0 );
			else
				m_btnNotBirthTime.SetCheck( 1 );

			int day = 0;
			dt = m_paxType.GetMinBirthTime();
			day = (int)(dt.m_dt);
			m_spinBirthStartDay.SetPos32(day);
			dt.m_dt -= day;
			m_dtCtrlBirthStart.SetTime( dt );

			dt = m_paxType.GetMaxBirthTime();
			day = (int)(dt.m_dt);
			m_spinBirthEndDay.SetPos32(day);
			dt.m_dt -= day;
			m_dtCtrlBirthEnd.SetTime( dt );
		}
		else
			EnableBirthTimeControl( FALSE );
		
		if( m_paxType.GetIsDeathTimeApplied() )
		{
			m_btnDeathTime.SetCheck( 1 );
			if( m_paxType.GetIsDeathTimeAdd() )
				m_btnNotDeathTime.SetCheck( 0 );
			else
				m_btnNotDeathTime.SetCheck( 1 );


			int day = 0;
			dt = m_paxType.GetMinDeathTime();
			day = (int)(dt.m_dt);
			m_spinDeathStartDay.SetPos32(day);
			dt.m_dt -= day;
			m_dtCtrlDeathStart.SetTime( dt );

			dt = m_paxType.GetMaxDeathTime();
			day = (int)(dt.m_dt);
			m_spinDeathEndDay.SetPos32(day);
			dt.m_dt -= day;
			m_dtCtrlDeathEnd.SetTime( dt );
		}
		else
			EnableDeathTimeControl( FALSE );

		m_csExpre = m_paxType.ScreenPrint( TRUE );
	
		UpdateData( FALSE );


	}
	m_bInitilized = TRUE;
	
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CPaxFilterDlg::SetPaxType(const CPaxType& _paxType)
{
	m_paxType = _paxType;
}

void CPaxFilterDlg::OnCheckAreaportal() 
{
	BOOL bEnable = m_btnAreaPortal.GetCheck();
	EnableAreaPortalControl( bEnable );
	
	if( bEnable )
	{
        m_radioArea.SetCheck( 1 );
		m_paxType.SetAreaportalApplyType( ENUM_APPLY_AREA );
		LoadArea();
		CArea area;
		area.name = "";
		m_paxType.SetArea( area );

		if( !( m_btnApplied.GetCheck() ) )
			m_paxType.SetIsTimeApplied( FALSE );

	}
	else
		m_paxType.SetAreaportalApplyType( ENUM_APPLY_NONE );
//	m_paxType.SetIsAreaPortalAdd( bEnable );
	
	ShowExpression();
}


void CPaxFilterDlg::EnableAreaPortalControl( BOOL _bEnable )
{
	m_radioArea.EnableWindow( _bEnable );
	m_radioPortal.EnableWindow( _bEnable );
	m_comboAreaPortal.EnableWindow( _bEnable );
	m_btnApplied.EnableWindow( _bEnable );
//	m_dtCtrlAreaStart.EnableWindow( _bEnable );
//	m_dtCtrlAreaEnd.EnableWindow( _bEnable );
	m_btnNotArea.EnableWindow( _bEnable );
	m_radioArea.SetCheck( 0 );
	m_radioPortal.SetCheck( 0 );
	m_btnApplied.SetCheck( 0 );

	BOOL bEnable = m_btnApplied.GetCheck();
    EnableAreaTimeControl(bEnable && _bEnable);

}

void CPaxFilterDlg::EnableNotAreaPortalControl( BOOL _bEnable )
{
	m_radioArea.EnableWindow( _bEnable );
	m_radioPortal.EnableWindow( _bEnable );
	m_radioArea.SetCheck( 0 );
	m_radioPortal.SetCheck( 0 );
	m_btnApplied.SetCheck( 0 );
	m_comboAreaPortal.EnableWindow( _bEnable );
	m_btnApplied.EnableWindow( _bEnable );
	m_dtCtrlAreaStart.EnableWindow( _bEnable );
	m_dtCtrlAreaEnd.EnableWindow( _bEnable );
	
	BOOL bEnable = m_btnApplied.GetCheck();
	EnableAreaTimeControl(bEnable && _bEnable);
}

void CPaxFilterDlg::OnCheckBirth() 
{
	// TODO: Add your control notification handler code here
	BOOL bEnable = m_btnBirthTime.GetCheck();
	EnableBirthTimeControl( bEnable );
	
	m_paxType.SetIsBirthTimeApplied( bEnable );
	
	ShowExpression();
}


void CPaxFilterDlg::EnableAreaTimeControl(BOOL _bEnable)
{
	m_spinAreaStartDay.EnableWindow(_bEnable);
	m_spinAreaEndDay.EnableWindow(_bEnable);
	m_editAreaStartDay.EnableWindow(_bEnable);
	m_editAreaEndDay.EnableWindow(_bEnable);

	m_dtCtrlAreaStart.EnableWindow( _bEnable );
	m_dtCtrlAreaEnd.EnableWindow( _bEnable );
}


void CPaxFilterDlg::EnableBirthTimeControl( BOOL _bEnable )
{
	m_spinBirthStartDay.EnableWindow(_bEnable);
	m_spinBirthEndDay.EnableWindow(_bEnable);
	m_editBirthStartDay.EnableWindow(_bEnable);
	m_editBirthEndDay.EnableWindow(_bEnable);

	m_dtCtrlBirthStart.EnableWindow( _bEnable );
	m_dtCtrlBirthEnd.EnableWindow( _bEnable );
	m_btnNotBirthTime.EnableWindow( _bEnable );
}

void CPaxFilterDlg::EnableNotBirthTimeControl( BOOL _bEnable )
{
	m_spinBirthStartDay.EnableWindow(_bEnable);
	m_spinBirthEndDay.EnableWindow(_bEnable);
	m_editBirthStartDay.EnableWindow(_bEnable);
	m_editBirthEndDay.EnableWindow(_bEnable);

	m_dtCtrlBirthStart.EnableWindow( _bEnable );
	m_dtCtrlBirthEnd.EnableWindow( _bEnable );
}

void CPaxFilterDlg::OnCheckDeath() 
{
	// TODO: Add your control notification handler code here
	BOOL bEnable = m_btnDeathTime.GetCheck();
	EnableDeathTimeControl( bEnable );	
		
	m_paxType.SetIsDeathTimeApplied( bEnable );
	
	ShowExpression();
}


void CPaxFilterDlg::EnableDeathTimeControl( BOOL _bEnable )
{
	m_spinDeathStartDay.EnableWindow(_bEnable);
	m_spinDeathEndDay.EnableWindow(_bEnable);
	m_editDeathStartDay.EnableWindow(_bEnable);
	m_editDeathEndDay.EnableWindow(_bEnable);

	m_dtCtrlDeathStart.EnableWindow( _bEnable );
	m_dtCtrlDeathEnd.EnableWindow( _bEnable );
	m_btnNotDeathTime.EnableWindow( _bEnable );
}

void CPaxFilterDlg::EnableNotDeathTimeControl( BOOL _bEnable )
{
	m_spinBirthStartDay.EnableWindow(_bEnable);
	m_spinBirthEndDay.EnableWindow(_bEnable);
	m_editBirthStartDay.EnableWindow(_bEnable);
	m_editBirthEndDay.EnableWindow(_bEnable);

	m_dtCtrlDeathStart.EnableWindow( _bEnable );
	m_dtCtrlDeathEnd.EnableWindow( _bEnable );
}

void CPaxFilterDlg::OnOK() 
{
	// TODO: Add extra validation here
	UpdateData( TRUE );
	// check name
	if( m_csName.IsEmpty() )
	{
		MessageBox( "Name can't be empty!" );
		return;
	}
	m_paxType.SetName( m_csName );
	// check combox
	if( m_btnAreaPortal.GetCheck() )
	{
		if( m_comboAreaPortal.GetCurSel() == -1 )
		{
			MessageBox( "Must select one item in the combox!" );
			return;
		}
	}
	// check the time
	COleDateTime dtstart,dtend;
    m_dtCtrlAreaStart.GetTime( dtstart );
	dtstart += m_spinAreaStartDay.GetPos32();
	m_dtCtrlAreaEnd.GetTime( dtend );
	dtend += m_spinAreaEndDay.GetPos32();
	if( dtstart > dtend )
	{
		MessageBox( "Area's Start time should less then the end time" );
		return;
	}
	
	m_dtCtrlBirthStart.GetTime( dtstart );
	dtstart += m_spinBirthStartDay.GetPos32();
    m_dtCtrlBirthEnd.GetTime( dtend );
	dtend += m_spinBirthEndDay.GetPos32();
	if( dtstart > dtend )
	{
		MessageBox( "Birth's Start time should less then the end time" );
		return;
	}
	
	m_dtCtrlDeathStart.GetTime( dtstart );
	dtstart += m_spinDeathStartDay.GetPos32();
	m_dtCtrlDeathEnd.GetTime( dtend );
	dtend += m_spinDeathEndDay.GetPos32();
	if( dtstart > dtend )
	{
		MessageBox( "Death's Start time should less then the end time" );
		return;
	}
	
	CDialog::OnOK();
}

void CPaxFilterDlg::OnCheckNotarea() 
{
	// TODO: Add your control notification handler code here
	BOOL bEnable = m_btnNotArea.GetCheck();
//	EnableNotAreaPortalControl( !bEnable );
	
	m_paxType.SetIsAreaPortalAdd( !bEnable );
	
	ShowExpression();
}

void CPaxFilterDlg::OnCheckNotbirthtime() 
{
	// TODO: Add your control notification handler code here
	BOOL bEnable = m_btnNotBirthTime.GetCheck();
//	EnableNotBirthTimeControl( !bEnable );
	
	m_paxType.SetIsBirthTimeAdd( !bEnable );
	
	ShowExpression();
}

void CPaxFilterDlg::OnCheckNotdeathtime() 
{
	// TODO: Add your control notification handler code here
	BOOL bEnable = m_btnNotDeathTime.GetCheck();
//	EnableNotDeathTimeControl( !bEnable );
	
	m_paxType.SetIsDeathTimeAdd( !bEnable );
	
	ShowExpression();
}

void CPaxFilterDlg::OnCheckApplied() 
{
	// TODO: Add your control notification handler code here
	BOOL bEnable = m_btnApplied.GetCheck();

	EnableAreaTimeControl(bEnable);

	m_paxType.SetIsTimeApplied( bEnable );
	
	ShowExpression();
}


void CPaxFilterDlg::OnDatetimechangeDatetimepickerAreastart(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	ShowExpression();
	*pResult = 0;
}

void CPaxFilterDlg::OnDatetimechangeDatetimepickerAreaend(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	
	ShowExpression();
	*pResult = 0;
}

void CPaxFilterDlg::OnDatetimechangeDatetimepickerBirthend(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	
	ShowExpression();
	*pResult = 0;
}

void CPaxFilterDlg::OnDatetimechangeDatetimepickerBirthstart(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	
	ShowExpression();
	*pResult = 0;
}

void CPaxFilterDlg::OnDatetimechangeDatetimepickerDeathend(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	
	ShowExpression();
	*pResult = 0;
}

void CPaxFilterDlg::OnDatetimechangeDatetimepickerDeathstart(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	
	
	ShowExpression();
	*pResult = 0;
}

void CPaxFilterDlg::ShowExpression()
{
	UpdateData( TRUE );

	COleDateTime dt;

	// because the type of COleDateTime.m_dt is DATE
	// and DATE is just double
	// and it means the day passed by,
	// so just plus it with the day value retrieved from spin edit ctrl
    m_dtCtrlAreaStart.GetTime( dt );
	dt.m_dt += m_spinAreaStartDay.GetPos32();
	m_paxType.SetStartTime( dt );
	m_dtCtrlAreaEnd.GetTime( dt );
	dt.m_dt += m_spinAreaEndDay.GetPos32();
	m_paxType.SetEndTime( dt );


    m_dtCtrlBirthEnd.GetTime( dt ); 
	dt.m_dt += m_spinBirthStartDay.GetPos32();
	m_paxType.SetMaxBirthTime( dt );
	m_dtCtrlBirthStart.GetTime( dt );
	dt.m_dt += m_spinBirthEndDay.GetPos32();
	m_paxType.SetMinBirthTime( dt );


	m_dtCtrlDeathEnd.GetTime( dt );
	dt.m_dt += m_spinDeathStartDay.GetPos32();
	m_paxType.SetMaxDeathTime( dt );
	m_dtCtrlDeathStart.GetTime( dt );
	dt.m_dt += m_spinDeathEndDay.GetPos32();
	m_paxType.SetMinDeathTime( dt );

	
	m_csExpre = m_paxType.ScreenPrint( TRUE );
	
	UpdateData( FALSE );
}


//	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();	

void CPaxFilterDlg::OnRadioArea() 
{
	// TODO: Add your control notification handler code here
	BOOL bEnable = m_radioArea.GetCheck();
	if( bEnable )
	{
		m_paxType.SetAreaportalApplyType( ENUM_APPLY_AREA );
		LoadArea();
		CArea area;
		area.name = "";
		m_paxType.SetArea( area );
	}
	else
	{
		m_paxType.SetAreaportalApplyType( ENUM_APPLY_PORTAL );
		CPortal portal;
		portal.name = "";
        m_paxType.SetPortal( portal );

		LoadPortal();
	}
	
	ShowExpression();
}

void CPaxFilterDlg::OnRadioPortal() 
{
	// TODO: Add your control notification handler code here
	BOOL bEnable = m_radioPortal.GetCheck();
	if( bEnable )
	{
		m_paxType.SetAreaportalApplyType( ENUM_APPLY_PORTAL );
		LoadPortal();
	}
	else
	{
		m_paxType.SetAreaportalApplyType( ENUM_APPLY_AREA );
		LoadArea();
	}
	
	ShowExpression();
}

void CPaxFilterDlg::OnButtonPaxcon() 
{
	// TODO: Add your control notification handler code here
	CPassengerTypeDialog dlg( m_pParentWnd );
	if (dlg.DoModal() == IDOK)
	{
		//PassengerConstraint newPaxCon = dlg.GetPaxSelection();
		CMobileElemConstraint mobileConstr = dlg.GetMobileSelection();
		m_paxType.SetPaxCon( mobileConstr );
		// add new pax constraint.
		//char buf[128];
		CString buf;
		mobileConstr.screenPrint( buf,0,128 );
		//m_csPaxCon = newPaxCon
		m_csPaxCon = buf;
		if(m_csName.IsEmpty())
			m_csName = buf;
		UpdateData( FALSE );
		ShowExpression();
	}
	
}


void CPaxFilterDlg::LoadArea()
{
	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();
	m_comboAreaPortal.ResetContent();
	CString str;
	CAreaList al = pDoc->GetTerminal().m_pAreas->m_vAreas;
	int nSize = al.size();
	for( int i=0; i< nSize; i++ )
	{
		CArea* pArea = al[i];
		str = pArea->name;
		m_comboAreaPortal.AddString( str );		
	}
	
	
}

void CPaxFilterDlg::LoadPortal()
{
	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();
    m_comboAreaPortal.ResetContent();
	CString str;
	CPortalList pl = pDoc->m_portals.m_vPortals;
	int nSize = pl.size();
	for( int i=0; i< nSize; i++ )
	{
		CPortal* pPortal = pl[i];
		str = pPortal->name;
		m_comboAreaPortal.AddString( str );		
	}
}

void CPaxFilterDlg::OnCheckNotpaxcon() 
{
	// TODO: Add your control notification handler code here
	BOOL bEnable = m_btnNotPaxCon.GetCheck();
	
	m_paxType.SetIsPaxConAdd( !bEnable );
	
	ShowExpression();
	
}

void CPaxFilterDlg::OnCloseupComboAreaportal() 
{
	// TODO: Add your control notification handler code here
	CString str;
	int nIdx = m_comboAreaPortal.GetCurSel();
	if( nIdx == -1 )
		return;
	
	m_comboAreaPortal.GetLBText( nIdx, str );
	
	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();
	
	BOOL bEnable = m_radioPortal.GetCheck();
	if( bEnable )
	{
		CPortal portal;
		portal.name = str;
        m_paxType.SetPortal( portal );
	}
	else
	{
		CArea area;
		area.name = str;
		m_paxType.SetArea( area );
	}
	ShowExpression();
}

void CPaxFilterDlg::OnKillfocusEditName() 
{
	// TODO: Add your control notification handler code here
	UpdateData( TRUE );
	m_paxType.SetName( m_csName );

}

void CPaxFilterDlg::OnDeltaposSpinAreastartday(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: Add your control notification handler code here
	ShowExpression();
	*pResult = 0;
}

void CPaxFilterDlg::OnDeltaposSpinAreaendday(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: Add your control notification handler code here
	ShowExpression();
	*pResult = 0;
}

void CPaxFilterDlg::OnDeltaposSpinBirthstartday(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: Add your control notification handler code here
	ShowExpression();
	*pResult = 0;
}

void CPaxFilterDlg::OnDeltaposSpinBirthendday(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: Add your control notification handler code here
	ShowExpression();
	*pResult = 0;
}

void CPaxFilterDlg::OnDeltaposSpinDeathstartday(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: Add your control notification handler code here
	ShowExpression();
	*pResult = 0;
}

void CPaxFilterDlg::OnDeltaposSpinDeathendday(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: Add your control notification handler code here
	ShowExpression();
	*pResult = 0;
}

void CPaxFilterDlg::OnEnChangeEditAreastartday()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	SafeShowExpression();
}

void CPaxFilterDlg::OnEnChangeEditAreaendday()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	SafeShowExpression();
}

void CPaxFilterDlg::OnEnChangeEditBirthstartday()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	SafeShowExpression();
}

void CPaxFilterDlg::OnEnChangeEditBirthendday()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	SafeShowExpression();
}

void CPaxFilterDlg::OnEnChangeEditDeathstartday()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	SafeShowExpression();
}

void CPaxFilterDlg::OnEnChangeEditDeathendday()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	SafeShowExpression();
}

void CPaxFilterDlg::SafeShowExpression(void)
{
	if (m_bInitilized)
	{
		ShowExpression();
	}
}

