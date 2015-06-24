// FltSchedDefineDlg.cpp : implementation file
//

#include "stdafx.h"
#include "termplan.h"
#include "FltSchedDefineDlg.h"
#include "inputs\in_term.h"
#include "inputs\schedule.h"

#include "..\common\AirlineManager.h"
#include "..\common\AirportsManager.h"
#include "..\common\ACTypesManager.h"

#include "DlgDBAirline.h"
#include "DlgDBAircraftTypes.h"
#include "DlgDBAirports.h"
#include "TermPlanDoc.h"
#include ".\fltscheddefinedlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFltSchedDefineDlg dialog


CFltSchedDefineDlg::CFltSchedDefineDlg(InputTerminal* _pInTerm, CWnd* pParent /*=NULL*/)
: CDialog(CFltSchedDefineDlg::IDD, pParent), m_flight( (Terminal*)_pInTerm )
{
	//{{AFX_DATA_INIT(CFltSchedDefineDlg)
	m_csAirline = _T("");
	m_csDest = _T("");
	m_csOrig = _T("");
	m_csFrom = _T("");
	m_csTo = _T("");
	m_csArrID = _T("");
	m_csDepID = _T("");
	m_csACType = _T("");
	m_oGateOccTime = COleDateTime::GetCurrentTime();
	
	//Delete by adam 2007-04-24
	//m_arrDate = COleDateTime::GetCurrentTime();
	//m_depDate = COleDateTime::GetCurrentTime();
	//End Delete by adam 2007-04-24

	//}}AFX_DATA_INIT
	m_bNewFlight = true;
	m_pInTerm = _pInTerm;
    ProcessorID gate;
	gate.SetStrDict( m_pInTerm->inStrDict );
	//m_flight.setGate( gate );
	m_flight.setArrGate( gate );
	m_flight.setDepGate( gate );
	m_flight.setBaggageDevice(gate);

	m_bStartDateChanged = FALSE;
}


void CFltSchedDefineDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFltSchedDefineDlg)
    //Delete by adam 2007-04-24
	//DDX_Control(pDX, IDC_DATETIMEPICKER_DEPDATE, m_dtCtrlDepDate);
	//DDX_Control(pDX, IDC_DATETIMEPICKER_ARRDATE, m_dtCtrlArrDate);	
	//DDX_Control(pDX, IDC_CHECK_DEPDATE, m_btnDepDate);
	//DDX_Control(pDX, IDC_CHECK_ARRDATE, m_btnArrDate);
	//End Delete by adam 2007-04-24

	//add by adam 2007-04-24
    DDX_Control(pDX, IDC_EDIT_DAYONE, m_editDayOne);
	DDX_Control(pDX, IDC_EDIT_DAYTWO, m_editDayTwo);
	DDX_Control(pDX, IDC_SPIN_DAYONE, m_sbtnDayOne);
	DDX_Control(pDX, IDC_SPIN_DAYTWO, m_sbtnDayTwo);
	//End add by adam 2007-04-24

	DDX_Control(pDX, IDC_DATETIMEPICKER_GATEOCC, m_gateOccTime);
	DDX_Control(pDX, IDC_DATETIMEPICKER_DEP, m_dtCtrlDep);
	DDX_Control(pDX, IDC_DATETIMEPICKER_ARR, m_dtCtrlArr);
	DDX_Control(pDX, IDC_CHECK_DEP, m_btnDep);
	DDX_Control(pDX, IDC_CHECK_ARR, m_btnArr);
	DDX_Control(pDX, IDC_EDIT_DEPID, m_editDepID);
	DDX_Control(pDX, IDC_EDIT_ARRID, m_editArrID);
	DDX_Control(pDX, IDC_COMBO_ORIG, m_comboOrig);
	DDX_Control(pDX, IDC_COMBO_DEST, m_comboDest);
	DDX_Control(pDX, IDC_COMBO_FROM, m_comboFrom);
	DDX_Control(pDX, IDC_COMBO_TO, m_comboTo);
	DDX_Control(pDX, IDC_COMBO_AIRLINE, m_comboAirline);
	DDX_Control(pDX, IDC_COMBO_ACTYPE, m_comboACType);
	DDX_CBString(pDX, IDC_COMBO_AIRLINE, m_csAirline);
	DDV_MaxChars(pDX, m_csAirline, 32);
	DDX_CBString(pDX, IDC_COMBO_ORIG, m_csOrig);
	DDV_MaxChars(pDX, m_csOrig, 5);
	DDX_CBString(pDX, IDC_COMBO_FROM, m_csFrom);
	DDV_MaxChars(pDX, m_csFrom, 5);
	DDX_CBString(pDX, IDC_COMBO_TO, m_csTo);
	DDV_MaxChars(pDX, m_csTo, 5);
	DDX_CBString(pDX, IDC_COMBO_DEST, m_csDest);
	DDV_MaxChars(pDX, m_csDest, 5);
	DDX_Text(pDX, IDC_EDIT_ARRID, m_csArrID);
	DDV_MaxChars(pDX, m_csArrID, 4);
	DDX_Text(pDX, IDC_EDIT_DEPID, m_csDepID);
	DDV_MaxChars(pDX, m_csDepID, 8);
	DDX_CBString(pDX, IDC_COMBO_ACTYPE, m_csACType);
	DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKER_ARR, m_arrTime);
	DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKER_DEP, m_depTime);
	DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKER_GATEOCC, m_oGateOccTime);

	//Delete by adam 2007-04-24
	//DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKER_ARRDATE, m_arrDate);
	//DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKER_DEPDATE, m_depDate);
	//End Delete by adam 2007-04-24

	DDX_Control(pDX, IDC_CHECK_ARRSTOPOVER, m_btnArrStopover);
	DDX_Control(pDX, IDC_CHECK_DEPSTOPOVER, m_btnDepStopover);
	//DDX_Control(pDX, IDC_COMBO_SID,m_cmbSID );
	//DDX_Control(pDX, IDC_COMBO_STAR,m_cmbStar );
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFltSchedDefineDlg, CDialog)
	//{{AFX_MSG_MAP(CFltSchedDefineDlg)
	ON_BN_CLICKED(IDC_CHECK_ARR, OnCheckArr)
	ON_BN_CLICKED(IDC_CHECK_DEP, OnCheckDep)
	ON_NOTIFY(NM_KILLFOCUS, IDC_DATETIMEPICKER_DEP, OnKillfocusDatetimepickerDep)
	ON_NOTIFY(NM_KILLFOCUS, IDC_DATETIMEPICKER_ARR, OnKillfocusDatetimepickerArr)
	//Delete by adam 2007-04-24
	//ON_BN_CLICKED(IDC_CHECK_ARRDATE, OnCheckArrdate)
	//ON_BN_CLICKED(IDC_CHECK_DEPDATE, OnCheckDepdate)
	//ON_NOTIFY(NM_KILLFOCUS, IDC_DATETIMEPICKER_DEPDATE, OnKillfocusDatetimepickerDepdate)
	//ON_NOTIFY(NM_KILLFOCUS, IDC_DATETIMEPICKER_ARRDATE, OnKillfocusDatetimepickerArrdate)
	//End Delete by adam 2007-04-24

	ON_BN_CLICKED(IDC_CHECK_ARRSTOPOVER, OnCheckArrstopover)
	ON_BN_CLICKED(IDC_CHECK_DEPSTOPOVER, OnCheckDepstopover)
	//}}AFX_MSG_MAP

	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_DAYONE, OnDeltaposSpinDayone)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_DAYTWO, OnDeltaposSpinDaytwo)
	ON_EN_KILLFOCUS(IDC_EDIT_DAYONE, OnEnKillfocusEditDayone)
	ON_EN_KILLFOCUS(IDC_EDIT_DAYTWO, OnEnKillfocusEditDaytwo)
	ON_NOTIFY(DTN_DATETIMECHANGE, IDC_DATETIMEPICKER_ARR, OnDtnDatetimechangeDatetimepickerArr)
	ON_NOTIFY(DTN_DATETIMECHANGE, IDC_DATETIMEPICKER_DEP, OnDtnDatetimechangeDatetimepickerDep)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFltSchedDefineDlg message handlers

CFltSchedDefineDlg::CFltSchedDefineDlg(const Flight &_flight, InputTerminal* _pInTerm, CWnd *pParent)
: CDialog(CFltSchedDefineDlg::IDD, pParent), m_flight( (Terminal*) _pInTerm )
{
	m_flight = _flight;
	m_bNewFlight = false;
	m_pInTerm = _pInTerm;
}

void CFltSchedDefineDlg::GetFlight(Flight &_flight)
{
	_flight = m_flight;
}

BOOL CFltSchedDefineDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here	m_comboAirline.AddString( "Select Airline" );	

	//Delete by adam 2007-04-24
	//m_arrDate = COleDateTime::GetCurrentTime();
	//m_depDate = COleDateTime::GetCurrentTime();
	//End Delete by adam 2007-04-24

	//Add by adam 2007-04-24
	m_sbtnDayOne.SetRange(1, 365);
	m_sbtnDayOne.SetPos(1);
	//m_editDayOne.SetWindowText("1");
	m_sbtnDayTwo.SetRange(1, 365);
	m_sbtnDayTwo.SetPos(1);
	//m_editDayTwo.SetWindowText("1");
	//End Add by adam 2007-04-24

	CAIRLINELIST* pvAirlineList;
	pvAirlineList = _g_GetActiveAirlineMan( m_pInTerm->m_pAirportDB )->GetAirlineList();
	for(int i=0; i<static_cast<int>(pvAirlineList->size()); i++) 
	{
		CAirline* pAirline = (*pvAirlineList)[i];
		int nIdx = m_comboAirline.AddString( pAirline->m_sAirlineIATACode );
		m_comboAirline.SetItemData( nIdx, i );
	}
	m_comboAirline.SetCurSel( -1 );
	
	CAIRPORTLIST* pvAirportList;
	pvAirportList = _g_GetActiveAirportMan( m_pInTerm->m_pAirportDB )->GetAirportList();
	for( i=0; i<static_cast<int>(pvAirportList->size()); i++) 
	{
		CAirport* pAirport = (*pvAirportList)[i];
		int nIdx = m_comboOrig.AddString( pAirport->m_sIATACode );
		m_comboOrig.SetItemData( nIdx, i );
		nIdx = m_comboFrom.AddString( pAirport->m_sIATACode );
		m_comboFrom.SetItemData( nIdx, i );
		nIdx = m_comboTo.AddString( pAirport->m_sIATACode );
		m_comboTo.SetItemData( nIdx, i );
		nIdx = m_comboDest.AddString( pAirport->m_sIATACode );
		m_comboDest.SetItemData( nIdx, i );
	}
	m_comboOrig.SetCurSel( -1 );
	m_comboFrom.SetCurSel( -1 );
	m_comboTo.SetCurSel( -1 );
	m_comboDest.SetCurSel( -1 );
	
	
	CACTYPELIST* pvACTList;
	pvACTList = _g_GetActiveACMan( m_pInTerm->m_pAirportDB )->GetACTypeList();
	for( i=0; i<static_cast<int>(pvACTList->size()); i++) 
	{
		CACType* pACT = (*pvACTList)[i];
		int nIdx = m_comboACType.AddString( pACT->GetIATACode() );
		m_comboACType.SetItemData( nIdx, i );
	}
	m_comboACType.SetCurSel( -1 );

	m_gateOccTime.SetFormat( "HH:mm" );
	m_oGateOccTime.SetTime( 0, 30, 0 ); // 30 min as default.
	
	m_dtCtrlArr.SetFormat( "HH:mm:ss" );
	m_dtCtrlDep.SetFormat( "HH:mm:ss" );
	
	CTermPlanDoc* pDoc = (CTermPlanDoc*) ((CMDIChildWnd *)((CMDIFrameWnd*)AfxGetApp()->m_pMainWnd)->GetActiveFrame())->GetActiveDocument();
	assert( pDoc );

	// AirsideInput& asInput = pDoc->GetAirsieInput();
	// ns_AirsideInput::AirRoutes airRoutes = asInput.GetAirRoutes();
	// const std::vector<ns_AirsideInput::AirRoute >& vecAirRoute = airRoutes.GetAirRoutes();
	// //initialize the star/sid combo box
	//std::vector<ns_AirsideInput::AirRoute >::const_iterator iter = vecAirRoute.begin();
	//std::vector<ns_AirsideInput::AirRoute >::const_iterator iterEnd = vecAirRoute.end();

	//for (;iter != iterEnd;++iter)
	//{
	//	if ((*iter).GetType() == ns_AirsideInput::AirRouteType_STAR)
	//	{
	//		m_cmbStar.AddString((*iter).GetName());
	//	}
	//	else
	//	{
	//		m_cmbSID.AddString((*iter).GetName());
	//	}
	//}

	if( !m_bNewFlight )
	{
		char str[128];
		m_flight.getAirline( str );
		m_csAirline = str;
		int	nIdx = m_comboAirline.FindString( -1, m_csAirline );
		assert( nIdx != -1 );
		m_comboAirline.SetCurSel( nIdx );

		//delete by adam 2007-04-24
		//CStartDate sDate = m_pInTerm->flightSchedule->GetStartDate();
		//COleDateTime outDate;
		//COleDateTime outTime;
		//bool bAbsDate;
		//int nOutDateIdx;
		//End delete by adam 2007-04-24
		
		if( m_flight.isArriving() )
		{
			m_btnArr.SetCheck(1);
			m_flight.getArrID( str );
			m_csArrID = str;
			m_flight.getOrigin( str );
			m_csOrig = str;
			nIdx = m_comboOrig.FindString( -1,m_csOrig );
			assert( nIdx != -1 );

			m_comboOrig.SetCurSel( nIdx );
			m_flight.getArrStopover(str);
			m_csFrom = str;
			nIdx = m_comboFrom.FindString( -1,m_csFrom );
			m_comboFrom.SetCurSel( nIdx );
			
			//Modify by adam 2007-04-24
			//sDate.GetDateTime( m_flight.getArrTime(), bAbsDate, outDate, nOutDateIdx, m_arrTime );
			m_estArrTime = m_flight.getArrTime();
			m_arrTime.SetTime(m_estArrTime.GetHour(), m_estArrTime.GetMinute(), m_estArrTime.GetSecond());
			//End modify by adam 2007-04-24

			m_dtCtrlArr.EnableWindow( true );
			m_editArrID.EnableWindow( true );
			m_comboOrig.EnableWindow( true );

			//Delete by adam 2007-04-24
			//m_btnArrDate.EnableWindow();
			//Delete by adam 2007-04-24

			//add by adam 2007-04-24
			CString strDay;
			strDay.Format("%d", m_estArrTime.GetDay());
			m_editDayOne.EnableWindow();
			m_editDayOne.SetWindowText(strDay);
			m_sbtnDayOne.EnableWindow();
			//add by adam 2007-04-24

			m_btnArrStopover.EnableWindow();

			if (!m_csFrom.IsEmpty())
			{
				m_comboFrom.EnableWindow( true );
				m_btnArrStopover.SetCheck(BST_CHECKED);
			}
			
			//Delete by adam 2007-04-24
			//if ( bAbsDate )
			//{
			//	
			//	m_arrDate = outDate;
			//	m_btnArrDate.SetCheck(1);
			//	m_dtCtrlArrDate.EnableWindow(TRUE);
			//}
			//End Delete by adam 2007-04-24
		}
		if( m_flight.isDeparting() )
		{
			m_btnDep.SetCheck(1);
			m_flight.getDepID( str );
			m_csDepID = str;
			m_flight.getDepStopover( str );
			m_csTo = str;
			nIdx = m_comboTo.FindString( -1,m_csTo );
			m_comboTo.SetCurSel( nIdx );
			m_flight.getDestination( str );
			m_csDest = str;
			nIdx = m_comboDest.FindString( -1,m_csDest );
			assert( nIdx != -1 );
			m_comboDest.SetCurSel( nIdx );
			
			//Modify by adam 2007-04-24
			//sDate.GetDateTime( m_flight.getDepTime(), bAbsDate, outDate, nOutDateIdx, m_depTime );
			m_estDepTime = m_flight.getDepTime();
			m_depTime.SetTime(m_estDepTime.GetHour(), m_estDepTime.GetMinute(), m_estDepTime.GetSecond());
			//End modify by adam 2007-04-24

			m_dtCtrlDep.EnableWindow( true );
			m_editDepID.EnableWindow( true );
			m_comboDest.EnableWindow( true );

			//Delete by adam 2007-04-24
			//m_btnDepDate.EnableWindow();
			//End Delete by adam 2007-04-24

			//add by adam 2007-04-24
			CString strDay;
			strDay.Format("%d", m_estDepTime.GetDay());
			m_editDayTwo.EnableWindow();
			m_editDayTwo.SetWindowText(strDay);
			m_sbtnDayTwo.EnableWindow();
			//add by adam 2007-04-24

			m_btnDepStopover.EnableWindow();

			if (!m_csTo.IsEmpty())
			{
				m_comboTo.EnableWindow( true );
				m_btnDepStopover.SetCheck(BST_CHECKED);
			}
			
			//Delete by adam 2007-04-24
			//if( bAbsDate )
			//{				
			//	m_depDate = outDate;
			//	m_btnDepDate.SetCheck(1);				
			//	m_dtCtrlDepDate.EnableWindow();
			//}
			//End Delete by adam 2007-04-24
		}
		m_flight.getACType( str );
		m_csACType = str;
		nIdx = m_comboACType.FindString( -1,m_csACType );
		m_comboACType.SetCurSel( nIdx );

		ElapsedTime time;
		if( m_flight.isTurnaround())
		{
			m_gateOccTime.EnableWindow( FALSE );
			time = m_flight.getDepTime() - m_flight.getArrTime();
		}
		else
		{
			time = m_flight.getServiceTime();
		}
		m_oGateOccTime.SetTime( (int)time.asHours() % 24, (int)time.asMinutes() % 60, (int)time.asSeconds() % 60 );					
		
		// Star SID

		//CString strFltStar = m_flight.GetStar();
		//int nStarCount = m_cmbStar.GetCount();
		//for (int i = 0; i < nStarCount; ++i)
		//{
		//	CString strStar = _T("");
		//	m_cmbStar.GetLBText(i,strStar);
		//	if (strStar.CompareNoCase(strFltStar) == 0)
		//	{
		//		m_cmbStar.SetCurSel(i);
		//		break;
		//	}
		//}

		//CString strFltSID = m_flight.GetSID();
		//int nSIDCount = m_cmbSID.GetCount();
		//for (int j = 0; j < nSIDCount; ++j)
		//{
		//	CString strSID = _T("");
		//	m_cmbSID.GetLBText(j,strSID);
		//	if (strSID.CompareNoCase(strFltSID) == 0)
		//	{
		//		m_cmbSID.SetCurSel(j);
		//		break;
		//	}
		//}
}
   
	UpdateData( false );	
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CFltSchedDefineDlg::OnCheckArr() 
{
	bool bEnable = true;
	if( m_btnArr.GetCheck() == 0 )
	{
		bEnable = false;
	}
	m_dtCtrlArr.EnableWindow( bEnable );
	m_editArrID.EnableWindow( bEnable );
	m_comboOrig.EnableWindow( bEnable );

	//Delete by adam 2007-04-24
	//m_btnArrDate.EnableWindow( bEnable );
	//End Delete by adam 2007-04-24

	//add by adam 2007-04-24
	m_editDayOne.EnableWindow(bEnable);
	m_sbtnDayOne.EnableWindow(bEnable);
	//add by adam 2007-04-24

	m_btnArrStopover.EnableWindow( bEnable );

	OnCheckArrstopover();

	//Delete by adam 2007-04-24
	//OnCheckArrdate();
	//End Delete by adam 2007-04-24
	
	SetGateOccupancyControl();
}

void CFltSchedDefineDlg::OnCheckDep() 
{
	bool bEnable = true;
	if( m_btnDep.GetCheck() == 0 )
	{
		bEnable = false;
	}
	m_dtCtrlDep.EnableWindow( bEnable );
	m_editDepID.EnableWindow( bEnable );
	m_comboDest.EnableWindow( bEnable );

	//Delete by adam 2007-04-24
	//m_btnDepDate.EnableWindow( bEnable );
	//End Delete by adam 2007-04-24

	//add by adam 2007-04-24
	m_editDayTwo.EnableWindow(bEnable);
	m_sbtnDayTwo.EnableWindow(bEnable);
	//add by adam 2007-04-24

	m_btnDepStopover.EnableWindow( bEnable );

	OnCheckDepstopover();

	//Delete by adam 2007-04-24
	//OnCheckDepdate();
	//End Delete by adam 2007-04-24

	SetGateOccupancyControl();
}

void CFltSchedDefineDlg::OnOK() 
{
	// TODO: Add extra validation here
	UpdateData();

	FlightSchedule* pfs = m_pInTerm->flightSchedule;
	//CStartDate sDate = pfs->GetStartDate();  delete by adam 2007-04-24

	// airline
	m_csAirline.TrimLeft();
	m_csAirline.TrimRight();
	if( m_csAirline.IsEmpty() )
	{
		MessageBox( "No Airline has been defined", "Error", MB_ICONEXCLAMATION | MB_OK );
		return;
	}

	m_csAirline.MakeUpper();
	int nIdx = _g_GetActiveAirlineMan( m_pInTerm->m_pAirportDB )->FindOrAddEntry( m_csAirline );
	if( nIdx == -1 )
	{
		MessageBox( "Invalid Airline Code", "Error", MB_ICONEXCLAMATION | MB_OK );
		return;
	}
	m_flight.setAirline( m_csAirline );


	//Aivin Start.
	//Delete by adam 2007-04-24
	//COleDateTime dtArr, dtDep;	
	//if (m_btnArrDate.GetCheck())
	//{
	//	dtArr.SetDateTime(m_arrDate.GetYear(), m_arrDate.GetMonth(), m_arrDate.GetDay(),
	//		m_arrTime.GetHour(), m_arrTime.GetMinute(), m_arrTime.GetSecond());
	//}	
	//else
	//{
	//	dtArr = m_arrTime;
	//}

	//if (m_btnDepDate.GetCheck())
	//{
	//	dtDep.SetDateTime(m_depDate.GetYear(), m_depDate.GetMonth(), m_depDate.GetDay(),
	//		m_depTime.GetHour(), m_depTime.GetMinute(), m_depTime.GetSecond());
	//}
	//else
	//		dtDep = m_depTime;
	////if( m_depTime > m_arrTime )
	//if (dtDep > dtArr)
	//{
	//	//COleDateTimeSpan timeSpan = m_depTime - m_arrTime;
	//	COleDateTimeSpan timeSpan = dtDep - dtArr;
	//	if (timeSpan.GetDays())
	//	{
	//		AfxMessageBox(_T("Can not more than one day(s)"));
	//		return;
	//	}
	//}
	//End Delete by adam 2007-04-24

	bool bCheckArr = ( m_btnArr.GetCheck() == 1 );
	bool bCheckDep = ( m_btnDep.GetCheck() == 1 );

	//add by adam 2007-04-24

// 	if (bCheckArr && bCheckDep)  // no need to check this now... 2009-07-08
// 	{
// 		ElapsedTime estTimeSpan = m_estDepTime - m_estArrTime;
// 		if (1 < estTimeSpan.GetDay())
// 		{
// 			AfxMessageBox(_T("Can not more than one day(s)"));
// 			return;
// 		}
// 	}	
	//End add by adam 2007-04-24
	//Aivin end.

	// airport
	
	if( !bCheckArr && !bCheckDep )
	{
		MessageBox( "Either Arriving or Departing flight has to be checked.","Error", MB_ICONEXCLAMATION | MB_OK );
		return;
	}

	
	if( !bCheckArr )
		m_flight.ClearArrOperation();
	else
	{
		int nIdx = -1;

		m_csOrig.TrimLeft();
		m_csOrig.TrimRight();
		if( !m_csOrig.IsEmpty() )
		{
			m_csOrig.MakeUpper();
			nIdx = _g_GetActiveAirportMan( m_pInTerm->m_pAirportDB )->FindOrAddEntry( m_csOrig );
			if( nIdx == -1 )
			{
				MessageBox( "Invalid Airport Code", "Error", MB_ICONEXCLAMATION | MB_OK );
				return;
			}			
		}

		if (!m_btnArrStopover.GetCheck())
			m_csFrom.Empty();
		m_csFrom.TrimLeft();
		m_csFrom.TrimRight();
		//if( m_csFrom.IsEmpty() )
		//{
		//	MessageBox( "No from Airport has been defined", "Error", MB_ICONEXCLAMATION | MB_OK );
		//	return;
		//}
		m_csFrom.MakeUpper();
		nIdx = _g_GetActiveAirportMan( m_pInTerm->m_pAirportDB )->FindOrAddEntry( m_csFrom );
		if( nIdx == -1 && !m_csFrom.IsEmpty())
		{
			MessageBox( "Invalid Airport Code", "Error", MB_ICONEXCLAMATION | MB_OK );
			return;
		}
		
		m_csArrID.TrimLeft();
		m_csArrID.TrimRight();
		int nStrLen = m_csArrID.GetLength();
		//if( nStrLen < 1 || !IsStringAllDigit( m_csArrID ) || m_csArrID == "0" )
		if( nStrLen < 1 || m_csArrID == "0" )
		{
			MessageBox( "Invalid Arrival Flight ID","Error", MB_ICONEXCLAMATION | MB_OK );
			return;
		}
				
		m_flight.setArrID( m_csArrID );
		m_flight.setOrigin( m_csOrig );
		m_flight.setStopoverAirport( m_csFrom );

		//Delete by adam 2007-04-24
		//if (bCheckDep && (m_btnArrDate.GetCheck() != m_btnDepDate.GetCheck()))
		//{
		//	AfxMessageBox(_T("Arrival date and departure date must match"));
		//	return;
		//}

		//ElapsedTime gateTime;
		//gateTime.set( m_arrTime.GetHour(), m_arrTime.GetMinute(), m_arrTime.GetSecond() );

		//if (m_btnDepDate.GetCheck())
		//{
		//	if (!pfs->getCount())
		//	{
		//		sDate.SetAbsoluteDate( true );
		//		sDate.SetDate( m_arrDate );
		//		pfs->SetStartDate( sDate );
		//	}
		//	sDate.GetRelativeTime(m_arrDate, gateTime, gateTime);
		//}
		//End Delete by adam 2007-04-24
		
		//modify by adam 2007-04-24
		//m_flight.setArrTime( gateTime );
		m_flight.setArrTime( m_estArrTime );
		//End modify by adam 2007-04-24
	}
	//else
	//{
	//	m_flight.setArrID( "" );
	//	ElapsedTime gateTime;
	//	gateTime.set( 0l );
	//	m_flight.setArrTime( gateTime );
	//	m_flight.setArrGateIndex(-1);
	//	
	//}
	
	if( !bCheckDep )
		m_flight.ClearDepOperation();
	else
	{
		if (!m_btnDepStopover.GetCheck())
			m_csTo.Empty();
		m_csTo.TrimLeft();
		m_csTo.TrimRight();
		//if( m_csTo.IsEmpty() )
		//{
		//	MessageBox( "No to Airport has been defined", "Error", MB_ICONEXCLAMATION | MB_OK );
		//	return;
		//}

		m_csTo.MakeUpper();
		int nIdx = _g_GetActiveAirportMan( m_pInTerm->m_pAirportDB )->FindOrAddEntry( m_csTo );
		if( nIdx == -1 && !m_csTo.IsEmpty())
		{
			MessageBox( "Invalid Airport Code", "Error", MB_ICONEXCLAMATION | MB_OK );
			return;
		}
		
		m_csDest.TrimLeft();
		m_csDest.TrimRight();
		if( !m_csDest.IsEmpty() )
		{
			//MessageBox( "No Dest Airport has been defined", "Error", MB_ICONEXCLAMATION | MB_OK );
			//return;

			m_csDest.MakeUpper();
			nIdx = _g_GetActiveAirportMan( m_pInTerm->m_pAirportDB )->FindOrAddEntry( m_csDest );
			if( nIdx == -1 )
			{
				MessageBox( "Invalid Airport Code", "Error", MB_ICONEXCLAMATION | MB_OK );
				return;
			}
		}



		m_csDepID.TrimLeft();
		m_csDepID.TrimRight();
		int nStrLen = m_csDepID.GetLength();
		//if( nStrLen < 1 || !IsStringAllDigit( m_csDepID ) || m_csDepID == "0" )
		if( nStrLen < 1 || m_csDepID == "0" )
		{
			MessageBox( "Invalid Departing Flight ID","Error", MB_ICONEXCLAMATION | MB_OK );
			return;
		}
				
		m_flight.setDepID( m_csDepID );
		m_flight.setStopoverAirport( m_csTo );
		m_flight.setDestination( m_csDest );

		//Delete by adam 2007-04-24
		//if (bCheckArr && (m_btnArrDate.GetCheck() != m_btnDepDate.GetCheck()))
		//{
		//	AfxMessageBox("Arrival date and departure date must match");
		//	return;
		//}
		
		//ElapsedTime gateTime;
		//gateTime.set( m_depTime.GetHour(), m_depTime.GetMinute(), m_depTime.GetSecond() );

		//if (m_btnDepDate.GetCheck())
		//{
		//	if (!pfs->getCount() && !bCheckArr)
		//	{
		//		sDate.SetAbsoluteDate( true );
		//		sDate.SetDate( m_depDate );
		//		pfs->SetStartDate( sDate );
		//	}
		//	sDate.GetRelativeTime(m_depDate, gateTime, gateTime);
		//}
		//End Delete by adam 2007-04-24

		//Modify by adam 2007-04-24
		//m_flight.setDepTime( gateTime );
		m_flight.setDepTime( m_estDepTime );
		//End modify by adam 2007-04-24
	}
	//else
	//{
	//	m_flight.setDepID( "" );
	//	ElapsedTime gateTime;
	//	gateTime.set( 0l );
	//	m_flight.setDepTime( gateTime );
	//	m_flight.setDepGateIndex(-1);
	//}
	
	if( bCheckArr && bCheckDep )
	{
		//if ( m_depTime < m_arrTime )
		//{
		//	MessageBox( "Dep Time / Arr Time are not valid.","Error", MB_ICONEXCLAMATION | MB_OK );
		//	return;
		//}

		//Delete by adam 2007-04-24
		//if (m_btnArrDate.GetCheck() && m_btnDepDate.GetCheck())
		//{
		//	if (m_arrDate > m_depDate)
		//	{
		//		MessageBox( "Dep Date / Arr Date are not valid.","Error", MB_ICONEXCLAMATION | MB_OK );
		//		return;
		//	}
		//	else if (m_arrDate == m_depDate)
		//	{
		//		if ( m_depTime < m_arrTime )
		//		{
		//			MessageBox( "Dep Time / Arr Time are not valid.","Error", MB_ICONEXCLAMATION | MB_OK );
		//			return;
		//		}
		//	}
		//}
		//else
		//{
		//	if ( m_depTime < m_arrTime )
		//	{
		//		MessageBox( "Dep Time / Arr Time are not valid.","Error", MB_ICONEXCLAMATION | MB_OK );
		//		return;
		//	}
		//}
		//End Delete by adam 2007-04-24

		//add by adam 2007-04-24
		if (m_estDepTime < m_estArrTime)
		{
			MessageBox( "Dep Time / Arr Time are not valid.","Error", MB_ICONEXCLAMATION | MB_OK );
			return;
		}
		//End add by adam 2007-04-24

	}

	//delete by adam 2007-04-24
	//if (!SniffDateChanged())
	//	return;
	//End delete by adam 2007-04-24

	// actype
	m_csACType.TrimLeft();
	m_csACType.TrimRight();
	if( m_csACType.IsEmpty() )
	{
		MessageBox( "No AC Type has been defined", "Error", MB_ICONEXCLAMATION );
		return;
	}

	m_csACType.MakeUpper();
	CACType* Pactype = NULL ;
	Pactype = _g_GetActiveACMan( m_pInTerm->m_pAirportDB )->getACTypeItem( m_csACType );
	if( Pactype == NULL )
	{
		CString errormsg ;
		errormsg.Format(_T("Invalid ACType,please define the actype entry for \'%s\'"),m_csACType) ;
		MessageBox( errormsg,"Error", MB_ICONEXCLAMATION | MB_OK );
		return;
	}
	m_flight.setACType( m_csACType );
	if( !m_flight.isTurnaround() )
	{
		long lSeconds = m_oGateOccTime.GetHour() * 3600l +
						m_oGateOccTime.GetMinute() * 60l +
						m_oGateOccTime.GetSecond();

		ElapsedTime eTime( lSeconds );
		m_flight.setServiceTime( eTime );

		if (!m_flight.isDeparting())
			m_flight.setArrParkingTime(eTime);
		
		if (!m_flight.isArriving())
			m_flight.setDepParkingTime(eTime);
	}

	if (m_flight.isTurnaround() && (m_flight.getArrStand().IsBlank()||m_flight.getDepStand().IsBlank()))
	{
		ALTObjectID emptyID;
		m_flight.setArrStand(emptyID);
		m_flight.setDepStand(emptyID);
	}
	////star sid
	//
	//int nStartSel = m_cmbStar.GetCurSel();
	//if (nStartSel != LB_ERR)
	//{
	//	CString strFltStar = _T("");
	//	m_cmbStar.GetLBText(nStartSel,strFltStar);
	//	m_flight.SetStar(strFltStar);
	//}

	//int nSIDSel = m_cmbSID.GetCurSel();
	//if (nSIDSel != LB_ERR)
	//{
	//	CString strFltSID = _T("");
	//	m_cmbSID.GetLBText(nSIDSel,strFltSID);
	//	m_flight.SetSID(strFltSID);
	//}

	CDialog::OnOK();
}

bool CFltSchedDefineDlg::IsStringAllDigit(const CString &_str)
{
	int nStrLen = _str.GetLength();
	for (int i = 0; i < nStrLen; i++)
	{
		if( !isdigit(_str[i]))
		{
			return false;
		}
	}
	return true;
}

bool CFltSchedDefineDlg::IsStringAllAlpha(const CString &_str)
{
	int nStrLen = _str.GetLength();
	for (int i = 0; i < nStrLen; i++)
	{
		if( !isalpha(_str[i]))
		{
			return false;
		}
	}
	return true;
}


// base on the current status, define data and read only feature.
void CFltSchedDefineDlg::SetGateOccupancyControl()
{
	UpdateData();
	bool bCheckArr = ( m_btnArr.GetCheck() == 1 );
	bool bCheckDep = ( m_btnDep.GetCheck() == 1 );
	if( bCheckArr && bCheckDep )
	{
		m_gateOccTime.EnableWindow( FALSE );

		ElapsedTime estTimeSpan = m_estDepTime - m_estArrTime;
		if ((m_estArrTime<m_estDepTime)
			&& (estTimeSpan.GetDay() < 2))
		{
			
			m_oGateOccTime.SetTime(estTimeSpan.GetHour(), estTimeSpan.GetMinute(), estTimeSpan.GetSecond());
		}
		else
		{
			m_oGateOccTime.SetTime( 0, 0, 0 );
		}


		//Delete by adam 2007-04-24
//		COleDateTime dtArr, dtDep;
//		
//		if (m_btnArrDate.GetCheck())
//		{
//			dtArr.SetDateTime(m_arrDate.GetYear(), m_arrDate.GetMonth(), m_arrDate.GetDay(),
//				m_arrTime.GetHour(), m_arrTime.GetMinute(), m_arrTime.GetSecond());
//		}
//		else
//		{
//			dtArr = m_arrTime;
//		}
//
//		if (m_btnDepDate.GetCheck())
//		{
//			dtDep.SetDateTime(m_depDate.GetYear(), m_depDate.GetMonth(), m_depDate.GetDay(),
//				m_depTime.GetHour(), m_depTime.GetMinute(), m_depTime.GetSecond());
//		}
//		else
//			dtDep = m_depTime;
//		//if( m_depTime > m_arrTime )
//		if (dtDep > dtArr)
//		{
//			//COleDateTimeSpan timeSpan = m_depTime - m_arrTime;
//			COleDateTimeSpan timeSpan = dtDep - dtArr;
////			if (timeSpan.GetDays())
////			{
////				AfxMessageBox(_T("Can not more than one day(s)"));
////				return;
////			}
//			m_oGateOccTime.SetTime( timeSpan.GetHours(), timeSpan.GetMinutes(), timeSpan.GetSeconds() );
//		}
//		else
//			m_oGateOccTime.SetTime( 0, 0, 0 );
		//End Delete by adam 2007-04-24
	}
	else
	{
		m_gateOccTime.EnableWindow( TRUE );
	}

	UpdateData( FALSE );
}



void CFltSchedDefineDlg::OnKillfocusDatetimepickerDep(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	m_estDepTime.SetHour(m_depTime.GetHour());
	m_estDepTime.SetMinute(m_depTime.GetMinute());
	m_estDepTime.SetSecond(m_depTime.GetSecond());

	SetGateOccupancyControl();
	*pResult = 0;
}

void CFltSchedDefineDlg::OnKillfocusDatetimepickerArr(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	m_estArrTime.SetHour(m_arrTime.GetHour());
	m_estArrTime.SetMinute(m_arrTime.GetMinute());
	m_estArrTime.SetSecond(m_arrTime.GetSecond());

	SetGateOccupancyControl();	
	*pResult = 0;
}

//Delete by adam 2007-04-24
//void CFltSchedDefineDlg::OnCheckArrdate() 
//{
//	BOOL bChecked = FALSE;
//
//	if (!m_btnArrDate.IsWindowEnabled())
//		bChecked = FALSE;
//	else
//		bChecked = m_btnArrDate.GetCheck();
//
//	m_dtCtrlArrDate.EnableWindow( bChecked );
//}
//End Delete by adam 2007-04-24

//Delete by adam 2007-04-24
//void CFltSchedDefineDlg::OnCheckDepdate() 
//{
//	BOOL bChecked = FALSE;
//
//	if (!m_btnDepDate.IsWindowEnabled())
//		bChecked = FALSE;
//	else
//		bChecked = m_btnDepDate.GetCheck();
//
//	m_dtCtrlDepDate.EnableWindow( bChecked );
//}

//void CFltSchedDefineDlg::OnKillfocusDatetimepickerDepdate(NMHDR* pNMHDR, LRESULT* pResult) 
//{
//	// TODO: Add your control notification handler code here
//	SetGateOccupancyControl();
//	*pResult = 0;
//}
//
//void CFltSchedDefineDlg::OnKillfocusDatetimepickerArrdate(NMHDR* pNMHDR, LRESULT* pResult) 
//{
//	// TODO: Add your control notification handler code here
//	SetGateOccupancyControl();
//	*pResult = 0;
//}
//End Delete by adam 2007-04-24

BOOL CFltSchedDefineDlg::SniffDateChanged()
{
	BOOL bRet = TRUE;
	FlightSchedule* pfs = m_pInTerm->flightSchedule;

	CStartDate sDate = pfs->GetStartDate();
	BOOL bPrevHasDate = sDate.IsAbsoluteDate();
	
	BOOL bHasDate = FALSE;

	//Delete by adam 2007-04-24
	//if (m_btnArrDate.GetCheck())
	//{
	//	bHasDate = TRUE;
	//	sDate.SetAbsoluteDate( true );
	//	sDate.SetDate( m_arrDate );
	//}
	//End Delete by adam 2007-04-24

	//Delete by adam 2007-04-24
	//if (m_btnDepDate.GetCheck())
	//{
	//	bHasDate = TRUE;
	//	if (!sDate.IsAbsoluteDate() )
	//	{
	//		sDate.SetAbsoluteDate( true );
	//		sDate.SetDate( m_depDate );
	//	}
	//}
	//End Delete by adam 2007-04-24

	if (!bHasDate)
	{
		sDate.SetAbsoluteDate( false );
	}

	CString strMessage;
	if (bPrevHasDate)
		strMessage = _T("This schedule has date fields. All date fields will be removed.");
	else
	{
		strMessage = _T("No date fields in this schedule, ");
		CString str;
		strMessage += _T("\"") + sDate.WriteStartDate() + _T("\"");
		strMessage += _T(" will be added as date fields.");
	}

	if (bPrevHasDate != bHasDate)
	{
		int nAnswer = MessageBox(strMessage, NULL, MB_OKCANCEL | MB_ICONQUESTION);
		if (nAnswer == IDOK)
		{
			pfs->SetStartDate(sDate);
			m_bStartDateChanged = TRUE;
		}
		else
		{
			bRet = FALSE;
		}
	}

	return bRet;
}

void CFltSchedDefineDlg::OnCheckArrstopover()
{
	BOOL bChecked = FALSE;

	if (!m_btnArrStopover.IsWindowEnabled())
		bChecked = FALSE;
	else
        bChecked = m_btnArrStopover.GetCheck();

	m_comboFrom.EnableWindow(bChecked);
}

void CFltSchedDefineDlg::OnCheckDepstopover()
{
	BOOL bChecked = FALSE;

	if (!m_btnDepStopover.IsWindowEnabled())
		bChecked = FALSE;
	else
		bChecked = m_btnDepStopover.GetCheck();

	m_comboTo.EnableWindow(bChecked);
}

void CFltSchedDefineDlg::OnDeltaposSpinDayone(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: Add your control notification handler code here

	CString strDay;
	m_editDayOne.GetWindowText(strDay);
	int nDay = atoi(strDay);

	//up
	if( pNMUpDown->iDelta > 0 )
	{
		if (nDay < 365)
		{
			nDay++;
			m_estArrTime.SetDay(nDay);
		}		
	}
	//down
	else
	{		
		if (1 < nDay)
		{
			nDay--;
			m_estArrTime.SetDay(nDay);
		}
	}

	SetGateOccupancyControl();

	*pResult = 0;
}

void CFltSchedDefineDlg::OnDeltaposSpinDaytwo(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: Add your control notification handler code here

	CString strDay;
	m_editDayTwo.GetWindowText(strDay);
	int nDay = atoi(strDay);

	//up
	if( pNMUpDown->iDelta > 0 )
	{
		if (nDay < 365)
		{
			nDay++;
			m_estDepTime.SetDay(nDay);
		}		
	}
	//down
	else
	{
		
		if (1 < nDay)
		{
			nDay--;
			m_estDepTime.SetDay(nDay);
		}
	}

	SetGateOccupancyControl();

	*pResult = 0;
}

void CFltSchedDefineDlg::OnEnKillfocusEditDayone()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	CString strDay;
	m_editDayOne.GetWindowText(strDay);
	int nDay = atoi(strDay);

	if (nDay<1 || 365<nDay)
	{
		AfxMessageBox("Day must between 1 and 365", MB_ICONEXCLAMATION | MB_OK);
		m_editDayOne.SetFocus();
	}
	else
	{
		m_estArrTime.SetDay(nDay);
		SetGateOccupancyControl();
	}	
}

void CFltSchedDefineDlg::OnEnKillfocusEditDaytwo()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	CString strDay;
	m_editDayTwo.GetWindowText(strDay);
	int nDay = atoi(strDay);

	if (nDay<1 || 365<nDay)
	{
		AfxMessageBox("Day must between 1 and 365", MB_ICONEXCLAMATION | MB_OK);
		m_editDayTwo.SetFocus();
	}
	else
	{
		m_estDepTime.SetDay(nDay);
		SetGateOccupancyControl();
	}		
}

void CFltSchedDefineDlg::OnDtnDatetimechangeDatetimepickerArr(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMDATETIMECHANGE pDTChange = reinterpret_cast<LPNMDATETIMECHANGE>(pNMHDR);
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	m_estArrTime.SetHour(m_arrTime.GetHour());
	m_estArrTime.SetMinute(m_arrTime.GetMinute());
	m_estArrTime.SetSecond(m_arrTime.GetSecond());

	SetGateOccupancyControl();	

	*pResult = 0;
}

void CFltSchedDefineDlg::OnDtnDatetimechangeDatetimepickerDep(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMDATETIMECHANGE pDTChange = reinterpret_cast<LPNMDATETIMECHANGE>(pNMHDR);
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	m_estDepTime.SetHour(m_depTime.GetHour());
	m_estDepTime.SetMinute(m_depTime.GetMinute());
	m_estDepTime.SetSecond(m_depTime.GetSecond());

	SetGateOccupancyControl();

	*pResult = 0;
}
