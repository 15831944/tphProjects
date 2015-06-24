// PaxFilterDlg.cpp : implementation file
//

#include "stdafx.h"
#include "AircraftFilterDlg.h"
#include "TermPlanDoc.h"
#include "FlightDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////
// CAircraftFilterDlg dialog


CAircraftFilterDlg::CAircraftFilterDlg(CWnd* pParent /*=NULL*/)
: CDialog(CAircraftFilterDlg::IDD, pParent)
{
	m_csName = _T("");
	m_csExpre = _T("\"DEFAULT\"");
	m_csPaxCon = _T("DEFAULT");
}


void CAircraftFilterDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

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
}


BEGIN_MESSAGE_MAP(CAircraftFilterDlg, CDialog)
//	ON_BN_CLICKED(IDC_CHECK_AREAPORTAL, OnCheckAreaportal)
	ON_BN_CLICKED(IDC_CHECK_BIRTH, OnCheckBirth)
	ON_BN_CLICKED(IDC_CHECK_DEATH, OnCheckDeath)
//	ON_BN_CLICKED(IDC_CHECK_NOTAREA, OnCheckNotarea)
	ON_BN_CLICKED(IDC_CHECK_NOTBIRTHTIME, OnCheckNotbirthtime)
	ON_BN_CLICKED(IDC_CHECK_NOTDEATHTIME, OnCheckNotdeathtime)
	ON_BN_CLICKED(IDC_CHECK_APPLIED, OnCheckApplied)
	ON_NOTIFY(DTN_DATETIMECHANGE, IDC_DATETIMEPICKER_AREASTART, OnDatetimechangeDatetimepickerAreastart)
	ON_NOTIFY(DTN_DATETIMECHANGE, IDC_DATETIMEPICKER_AREAEND, OnDatetimechangeDatetimepickerAreaend)
	ON_NOTIFY(DTN_DATETIMECHANGE, IDC_DATETIMEPICKER_BIRTHEND, OnDatetimechangeDatetimepickerBirthend)
	ON_NOTIFY(DTN_DATETIMECHANGE, IDC_DATETIMEPICKER_BIRTHSTART, OnDatetimechangeDatetimepickerBirthstart)
	ON_NOTIFY(DTN_DATETIMECHANGE, IDC_DATETIMEPICKER_DEATHEND, OnDatetimechangeDatetimepickerDeathend)
	ON_NOTIFY(DTN_DATETIMECHANGE, IDC_DATETIMEPICKER_DEATHSTART, OnDatetimechangeDatetimepickerDeathstart)
//	ON_BN_CLICKED(IDC_RADIO_AREA, OnRadioArea)
//	ON_BN_CLICKED(IDC_RADIO_PORTAL, OnRadioPortal)
	ON_BN_CLICKED(IDC_BUTTON_PAXCON, OnButtonPaxcon)
	ON_BN_CLICKED(IDC_CHECK_NOTPAXCON, OnCheckNotpaxcon)
//	ON_CBN_CLOSEUP(IDC_COMBO_AREAPORTAL, OnCloseupComboAreaportal)
	ON_EN_KILLFOCUS(IDC_EDIT_NAME, OnKillfocusEditName)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAircraftFilterDlg message handlers

BOOL CAircraftFilterDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();	
	m_flightType.GetFlightConstraint().SetAirportDB( pDoc->GetTerminal().m_pAirportDB);
	
	m_btnAreaPortal.EnableWindow(FALSE);
	EnableAreaPortalControl(FALSE);

	if( m_flightType.GetName() == "Undefined" )
	{
		EnableBirthTimeControl( FALSE );
		EnableDeathTimeControl( FALSE );

	}
	else
	{
		m_csName = m_flightType.GetName();

		char buf[128];
		m_flightType.GetFlightConstraint().screenPrint( buf );
		//m_csPaxCon = newPaxCon
		m_csPaxCon = buf;

		if( m_flightType.GetIsFlightConstraintAdd() )
			m_btnNotPaxCon.SetCheck( 0 );
		else
			m_btnNotPaxCon.SetCheck( 1 );

		CArea area;
		CPortal portal;
//		int nIdx;
		COleDateTime dt;
		
/*		AREA_PORTAL_APPLY_TYPE type = m_flightType.GetAreaportalApplyType();
		switch( type )
		{
		case ENUM_APPLY_AREA:
			m_btnAreaPortal.SetCheck( 1 );
			if( m_flightType.GetIsAreaPortalAdd() )
				m_btnNotArea.SetCheck( 0 );
			else
				m_btnNotArea.SetCheck( 1 );
			m_radioArea.SetCheck( 1 );
			LoadArea();
			area = m_flightType.GetArea();
			nIdx = m_comboAreaPortal.FindString( -1,area.name );
			m_comboAreaPortal.SetCurSel( nIdx );
			if( m_flightType.GetIsTimeApplied() )
			{
				m_btnApplied.SetCheck( 1 );
				dt = m_flightType.GetStartTime();
				m_dtCtrlAreaStart.SetTime( dt );
				dt = m_flightType.GetEndTime();
				m_dtCtrlAreaEnd.SetTime( dt );
			}
			else
			{
				m_btnApplied.SetCheck( 0 );
			}
			break;
		case ENUM_APPLY_PORTAL:
			m_btnAreaPortal.SetCheck( 1 );
			if( m_flightType.GetIsAreaPortalAdd() )
				m_btnNotArea.SetCheck( 0 );
			else
				m_btnNotArea.SetCheck( 1 );
			m_radioPortal.SetCheck( 1 );
			LoadPortal();
			portal = m_flightType.GetPortal();
			nIdx = m_comboAreaPortal.FindString( -1,portal.name );
			m_comboAreaPortal.SetCurSel( nIdx );
			if( m_flightType.GetIsTimeApplied() )
			{
				m_btnApplied.SetCheck( 1 );
				dt = m_flightType.GetStartTime();
				m_dtCtrlAreaStart.SetTime( dt );
				dt = m_flightType.GetEndTime();
				m_dtCtrlAreaEnd.SetTime( dt );
			}
			else
			{
				m_btnApplied.SetCheck( 0 );
			}
			break;
		default: //  ENUM_APPLY_NONE
			m_btnAreaPortal.SetCheck( 0 );
			EnableAreaPortalControl( FALSE );
		}
*/
		if( m_flightType.GetIsBirthTimeApplied() )
		{
			m_btnBirthTime.SetCheck( 1 );
			if( m_flightType.GetIsBirthTimeAdd() )
				m_btnNotBirthTime.SetCheck( 0 );
			else
				m_btnNotBirthTime.SetCheck( 1 );
			dt = m_flightType.GetMinBirthTime();
			m_dtCtrlBirthStart.SetTime( dt );
			dt = m_flightType.GetMaxBirthTime();
			m_dtCtrlBirthEnd.SetTime( dt );
		}
		else
			EnableBirthTimeControl( FALSE );

		if( m_flightType.GetIsDeathTimeApplied() )
		{
			m_btnDeathTime.SetCheck( 1 );
			if( m_flightType.GetIsDeathTimeAdd() )
				m_btnNotDeathTime.SetCheck( 0 );
			else
				m_btnNotDeathTime.SetCheck( 1 );
			dt = m_flightType.GetMinDeathTime();
			m_dtCtrlDeathStart.SetTime( dt );
			dt = m_flightType.GetMaxDeathTime();
			m_dtCtrlDeathEnd.SetTime( dt );
		}
		else
			EnableDeathTimeControl( FALSE );

		m_csExpre = m_flightType.ScreenPrint( TRUE );

		UpdateData( FALSE );


	}

	return TRUE;
}

void CAircraftFilterDlg::SetFlightType(const CFlightType& _flightType)
{
	m_flightType = _flightType;
}

void CAircraftFilterDlg::OnCheckAreaportal() 
{
/*	BOOL bEnable = m_btnAreaPortal.GetCheck();
//	EnableAreaPortalControl( bEnable );

	if( bEnable )
	{
		m_radioArea.SetCheck( 1 );
		m_flightType.SetAreaportalApplyType( ENUM_APPLY_AREA );
		LoadArea();
		CArea area;
		area.name = "";
		m_flightType.SetArea( area );

		if( !( m_btnApplied.GetCheck() ) )
			m_flightType.SetIsTimeApplied( FALSE );

	}
	else
		m_flightType.SetAreaportalApplyType( ENUM_APPLY_NONE );

	ShowExpresion();
*/
}


void CAircraftFilterDlg::EnableAreaPortalControl( BOOL _bEnable )
{
	m_radioArea.EnableWindow( _bEnable );
	m_radioPortal.EnableWindow( _bEnable );
	m_comboAreaPortal.EnableWindow( _bEnable );
	m_btnApplied.EnableWindow( _bEnable );
	m_btnNotArea.EnableWindow( _bEnable );
	m_radioArea.SetCheck( 0 );
	m_radioPortal.SetCheck( 0 );
	m_btnApplied.SetCheck( 0 );

	BOOL bEnable = m_btnApplied.GetCheck();
	m_dtCtrlAreaStart.EnableWindow( bEnable && _bEnable );
	m_dtCtrlAreaEnd.EnableWindow( bEnable && _bEnable );

}

void CAircraftFilterDlg::EnableNotAreaPortalControl( BOOL _bEnable )
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
	m_dtCtrlAreaStart.EnableWindow( bEnable && _bEnable );
	m_dtCtrlAreaEnd.EnableWindow( bEnable && _bEnable );

}

void CAircraftFilterDlg::OnCheckBirth() 
{
	// TODO: Add your control notification handler code here
	BOOL bEnable = m_btnBirthTime.GetCheck();
	EnableBirthTimeControl( bEnable );

	m_flightType.SetIsBirthTimeApplied( bEnable );

	ShowExpresion();
}

void CAircraftFilterDlg::EnableBirthTimeControl( BOOL _bEnable )
{
	m_dtCtrlBirthStart.EnableWindow( _bEnable );
	m_dtCtrlBirthEnd.EnableWindow( _bEnable );
	m_btnNotBirthTime.EnableWindow( _bEnable );
}

void CAircraftFilterDlg::EnableNotBirthTimeControl( BOOL _bEnable )
{
	m_dtCtrlBirthStart.EnableWindow( _bEnable );
	m_dtCtrlBirthEnd.EnableWindow( _bEnable );
}

void CAircraftFilterDlg::OnCheckDeath() 
{
	// TODO: Add your control notification handler code here
	BOOL bEnable = m_btnDeathTime.GetCheck();
	EnableDeathTimeControl( bEnable );	

	m_flightType.SetIsDeathTimeApplied( bEnable );

	ShowExpresion();
}


void CAircraftFilterDlg::EnableDeathTimeControl( BOOL _bEnable )
{
	m_dtCtrlDeathStart.EnableWindow( _bEnable );
	m_dtCtrlDeathEnd.EnableWindow( _bEnable );
	m_btnNotDeathTime.EnableWindow( _bEnable );
}

void CAircraftFilterDlg::EnableNotDeathTimeControl( BOOL _bEnable )
{
	m_dtCtrlDeathStart.EnableWindow( _bEnable );
	m_dtCtrlDeathEnd.EnableWindow( _bEnable );
}

void CAircraftFilterDlg::OnOK() 
{
	// TODO: Add extra validation here
	UpdateData( TRUE );
	// check name
	if( m_csName.IsEmpty() )
	{
		MessageBox( "Name can't be empty!" );
		return;
	}
	m_flightType.SetName( m_csName );
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
	m_dtCtrlAreaEnd.GetTime( dtend );
	if( dtstart > dtend )
	{
		MessageBox( "Area's Start time should less then the end time" );
		return;
	}

	m_dtCtrlBirthStart.GetTime( dtstart );
	m_dtCtrlBirthEnd.GetTime( dtend ); 
	if( dtstart > dtend )
	{
		MessageBox( "Birth's Start time should less then the end time" );
		return;
	}

	m_dtCtrlDeathStart.GetTime( dtstart );
	m_dtCtrlDeathEnd.GetTime( dtend );
	if( dtstart > dtend )
	{
		MessageBox( "Death's Start time should less then the end time" );
		return;
	}

	CDialog::OnOK();
}

void CAircraftFilterDlg::OnCheckNotarea() 
{
/*	BOOL bEnable = m_btnNotArea.GetCheck();
	//	EnableNotAreaPortalControl( !bEnable );

	m_flightType.SetIsAreaPortalAdd( !bEnable );

	ShowExpresion();
*/
}

void CAircraftFilterDlg::OnCheckNotbirthtime() 
{
	// TODO: Add your control notification handler code here
	BOOL bEnable = m_btnNotBirthTime.GetCheck();
	//	EnableNotBirthTimeControl( !bEnable );

	m_flightType.SetIsBirthTimeAdd( !bEnable );

	ShowExpresion();
}

void CAircraftFilterDlg::OnCheckNotdeathtime() 
{
	// TODO: Add your control notification handler code here
	BOOL bEnable = m_btnNotDeathTime.GetCheck();
	//	EnableNotDeathTimeControl( !bEnable );

	m_flightType.SetIsDeathTimeAdd( !bEnable );

	ShowExpresion();
}

void CAircraftFilterDlg::OnCheckApplied() 
{
	// TODO: Add your control notification handler code here
	BOOL bEnable = m_btnApplied.GetCheck();

	m_dtCtrlAreaStart.EnableWindow( bEnable );
	m_dtCtrlAreaEnd.EnableWindow( bEnable );

	m_flightType.SetIsTimeApplied( bEnable );

	ShowExpresion();
}void CAircraftFilterDlg::OnDatetimechangeDatetimepickerAreastart(NMHDR* pNMHDR, LRESULT* pResult) 



{
	// TODO: Add your control notification handler code here
	ShowExpresion();
	*pResult = 0;
}

void CAircraftFilterDlg::OnDatetimechangeDatetimepickerAreaend(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here

	ShowExpresion();
	*pResult = 0;
}

void CAircraftFilterDlg::OnDatetimechangeDatetimepickerBirthend(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here

	ShowExpresion();
	*pResult = 0;
}

void CAircraftFilterDlg::OnDatetimechangeDatetimepickerBirthstart(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here

	ShowExpresion();
	*pResult = 0;
}

void CAircraftFilterDlg::OnDatetimechangeDatetimepickerDeathend(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here

	ShowExpresion();
	*pResult = 0;
}

void CAircraftFilterDlg::OnDatetimechangeDatetimepickerDeathstart(NMHDR* pNMHDR, LRESULT* pResult) 
{
	ShowExpresion();
	*pResult = 0;
}

void CAircraftFilterDlg::ShowExpresion()
{
	UpdateData( TRUE );

	COleDateTime dt;
	m_dtCtrlAreaStart.GetTime( dt );
	m_flightType.SetStartTime( dt );
	m_dtCtrlAreaEnd.GetTime( dt );
	m_flightType.SetEndTime( dt );
	m_dtCtrlBirthEnd.GetTime( dt ); 
	m_flightType.SetMaxBirthTime( dt );
	m_dtCtrlBirthStart.GetTime( dt );
	m_flightType.SetMinBirthTime( dt );
	m_dtCtrlDeathEnd.GetTime( dt );
	m_flightType.SetMaxDeathTime( dt );
	m_dtCtrlDeathStart.GetTime( dt );
	m_flightType.SetMinDeathTime( dt );

	m_csExpre = m_flightType.ScreenPrint( TRUE );

	UpdateData( FALSE );
}


void CAircraftFilterDlg::OnRadioArea() 
{
/*	BOOL bEnable = m_radioArea.GetCheck();
	if( bEnable )
	{
		m_flightType.SetAreaportalApplyType( ENUM_APPLY_AREA );
		LoadArea();
		CArea area;
		area.name = "";
		m_flightType.SetArea( area );
	}
	else
	{
		m_flightType.SetAreaportalApplyType( ENUM_APPLY_PORTAL );
		CPortal portal;
		portal.name = "";
		m_flightType.SetPortal( portal );

		LoadPortal();
	}

	ShowExpresion();
*/
}

void CAircraftFilterDlg::OnRadioPortal() 
{
/*	BOOL bEnable = m_radioPortal.GetCheck();
	if( bEnable )
	{
		m_flightType.SetAreaportalApplyType( ENUM_APPLY_PORTAL );
		LoadPortal();
	}
	else
	{
		m_flightType.SetAreaportalApplyType( ENUM_APPLY_AREA );
		LoadArea();
	}

	ShowExpresion();
*/
}

void CAircraftFilterDlg::OnButtonPaxcon() 
{
	CFlightDialog dlg( m_pParentWnd );
	if( dlg.DoModal() == IDOK )
	{
		FlightConstraint newFltCon = dlg.GetFlightSelection();
		m_flightType.SetFlightConstraint(newFltCon);
		// add new pax constraint.

		CString buf;
		newFltCon.screenPrint( buf,0,128 );
		m_csPaxCon = buf;
		if(m_csName.IsEmpty())
			m_csName = buf;
		UpdateData( FALSE );
		ShowExpresion();
	}
}


void CAircraftFilterDlg::LoadArea()
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

void CAircraftFilterDlg::LoadPortal()
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

void CAircraftFilterDlg::OnCheckNotpaxcon() 
{
	BOOL bEnable = m_btnNotPaxCon.GetCheck();

	m_flightType.SetIsFlightConstraintAdd( !bEnable );

	ShowExpresion();

}

void CAircraftFilterDlg::OnCloseupComboAreaportal() 
{
/*	CString str;
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
		m_flightType.SetPortal( portal );
	}
	else
	{
		CArea area;
		area.name = str;
		m_flightType.SetArea( area );
	}
	ShowExpresion();
*/
}

void CAircraftFilterDlg::OnKillfocusEditName() 
{
	UpdateData( TRUE );
	m_flightType.SetName( m_csName );

}
