// RepControlView.cpp : implementation file
//

#include "stdafx.h"
#include "termplan.h"
#include "RepControlView.h"
#include "common/csv_file.h"
#include "TermPlanDoc.h"

#include "reports\PrintLog.h"
#include "reports\UtilizationReport.h"
#include "PassengerTypeDialog.h"
#include "common\termfile.h"
#include "ProcesserDialog.h"
#include "../common/UnitsManager.h"
#include "FlightDialog.h"

#include "reports\ReportParaDB.h"
#include "reports\ReportParameter.h"
#include "reports\reportParaOfTime.h"
#include "reports\reportParaOfProcs.h"
#include "reports\reportParaOfReportType.h"
#include "reports\reportParaOfThreshold.h"
#include "reports\reportParaOfPaxtype.h"
#include "reports\reportParaWithArea.h"
#include "reports\reportParaWithPortal.h"

#include "reports\PrintLog.h"
#include "reports\UtilizationReport.h"
#include "reports\DistanceReport.h"
#include "reports\PaxCountReport.h"
#include "reports\QueueReport.h"
#include "reports\ThroughputReport.h"
#include "reports\QueueLengthReport.h"
#include "reports\AverageQueueLengthReport.h"
#include "reports\DurationReport.h"
#include "reports\ServiceReport.h"
#include "reports\QueueElement.h"
#include "reports\servelem.h"
#include "reports\ActivityBreakdownReport.h"
#include "reports\ActivityElements.h"
#include "reports\BagWaitReport.h"
#include "reports\BagWaitElement.h"
#include "reports\BagCountReport.h"
#include "reports\PassengerDensityReport.h"
#include "reports\SpaceThroughputReport.h"
#include "reports\CollisionReport.h"
#include "reports\AcOperationsReport.h"
#include "reports\BagDistReport.h"
#include "reports\BagDeliveryTimeReport.h"
#include "reports\FireEvacutionReport.h"
#include "reports\ConveyorWaitLenghtReport.h"
#include "reports\ConveyorWaitTimeReport.h"
#include "Reports/BillboardExposureIncidenceReport.h"
#include "Reports/BillboardLinkedProcIncrementalVisitReport.h"
#include ".\repcontrolview.h"
#include "Reports\RetailReport.h"
#include "Common\SimAndReportManager.h"
#include "Reports\MissFlightReport.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const int iEdgeWidth		= 5; 
const int iToolBarHeight	= 34;
#define SAVEPARATOFILE		10000
#define LOADPARAFROMFILE	10001
#define SAVEREPORTTOFILE	10002
#define LOADREPORTFROMFILE	10003


#define  ID_TOOLBARBUTTONADD_ADD 100000
#define  ID_TOOLBARBUTTONDEL_DEL 100001
const CString strBtnTxt[] =
{
	"Save Para",
	"Load Para",
	"Save Report",
	"Load Report"
};
/////////////////////////////////////////////////////////////////////////////
// CRepControlView

IMPLEMENT_DYNCREATE(CRepControlView, CFormView)

CRepControlView::CRepControlView()
	: CFormView(CRepControlView::IDD)
{
	//{{AFX_DATA_INIT(CRepControlView)
	m_oleDTEndTime = COleDateTime::GetCurrentTime();
	m_oleDTStartTime = COleDateTime::GetCurrentTime();
	m_oleDTEndDate = COleDateTime::GetCurrentTime();
	m_oleDTStartDate = COleDateTime::GetCurrentTime();
	m_nRepType = 0;
	m_nInterval = 1;
	m_oleDTInterval = COleDateTime::GetCurrentTime();
	m_fThreshold = 0.0f;
	m_oleThreshold = COleDateTime::GetCurrentTime();
	//}}AFX_DATA_INIT
	m_bCanToSave = false;
	m_strCurReportFile = "";

	m_emDateType = DATETYPE_INDEX;
	m_enumReportType = ENUM_REPORT_TYPE(-1);


	m_hSelectedItem = NULL;

	m_hPaxTypeRoot= NULL;
	m_hProcRoot= NULL;
	m_hRunsRoot= NULL;
	m_hFlightTypeRoot= NULL;
	m_hItemTo= NULL;
	m_hItemFrom= NULL;
	m_hProcToProc = NULL;
}

CRepControlView::~CRepControlView()
{

}

void CRepControlView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRepControlView)
	DDX_Control(pDX, IDC_COMBO_ENDDAY, m_comboEndDay);
	DDX_Control(pDX, IDC_COMBO_STARTDAY, m_comboStartDay);
	DDX_Control(pDX, IDC_TREE_PAXTYPE, m_treePaxType);
	DDX_Control(pDX, IDC_STATIC_TIME, m_staticTime);
	DDX_Control(pDX, IDC_STATIC_STARTTIME, m_staticStartTime);
	DDX_Control(pDX, IDC_STATIC_ENDTIME, m_staticEndTime);
	DDX_Control(pDX, IDC_BUTTON_MULTI, m_btnMulti);
	DDX_Control(pDX, IDC_BUTTON_LOAD_FROM_FILE, m_btLoad);
	DDX_Control(pDX, IDC_DATETIMEPICKER_THRESHOLD, m_dtctrlThreshold);
	DDX_Control(pDX, IDC_DATETIMEPICKER_INTERVAL, m_dtctrlInterval);
	DDX_Control(pDX, IDC_SPIN_THRESHOLD, m_spinThreshold);
	DDX_Control(pDX, IDC_STATIC_AREA, m_staticArea);
	DDX_Control(pDX, IDC_COMBO_AREA, m_comboAreaPortal);
	DDX_Control(pDX, IDC_STATIC_THRESHOLD, m_staticThreshold);
	DDX_Control(pDX, IDC_STATIC_TYPE, m_staticType);
	DDX_Control(pDX, IDC_STATIC_INTERVAL, m_staticInterval);
	DDX_Control(pDX, IDC_STATICPROTYPE, m_staProc);
	DDX_Control(pDX, IDC_STATICPASTYPE, m_staPax);
	DDX_Control(pDX, IDC_TOOLBARCONTENTER2, m_toolbarcontenter2);
	DDX_Control(pDX, IDC_TOOLBARCONTENTER1, m_toolbarcontenter1);
	DDX_Control(pDX, IDC_STATIC_PROLIST, m_staticProListLabel);
	DDX_Control(pDX, IDC_LIST_PROTYPE, m_lstboProType);
	DDX_Control(pDX, IDC_STATIC_TYPELIST, m_staticTypeListLabel);
	DDX_Control(pDX, ID_BUTTON_CANCEL, m_btnCancel);
	DDX_Control(pDX, IDC_DATETIMEPICKER_START, m_dtctrlStartTime);
	DDX_Control(pDX, IDC_DATETIMEPICKER_END, m_dtctrlEndTime);
	DDX_Control(pDX, IDC_DATETIMEPICKER_START2, m_dtctrlStartDate);
	DDX_Control(pDX, IDC_DATETIMEPICKER_END2, m_dtctrlEndDate);
	DDX_Control(pDX, IDC_RADIO_DETAILED, m_radioDetailed);
	DDX_Control(pDX, IDC_RADIO_SUMMARY, m_radioSummary);
	DDX_Control(pDX, IDC_LIST_PAXTYPE, m_lstboPaxType);
	DDX_Control(pDX, IDC_EDIT_THRESHOLD, m_edtThreshold);
	DDX_Control(pDX, IDC_CHECK_THRESHOLD, m_chkThreshold);
	DDX_Control(pDX, IDC_BUTTON_APPLY, m_btnApply);
	DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKER_END, m_oleDTEndTime);
	DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKER_START, m_oleDTStartTime);
	DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKER_END2, m_oleDTEndDate);
	DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKER_START2, m_oleDTStartDate);
	DDX_Radio(pDX, IDC_RADIO_DETAILED, m_nRepType);
	DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKER_INTERVAL, m_oleDTInterval);
	DDX_Text(pDX, IDC_EDIT_THRESHOLD, m_fThreshold);
	DDV_MinMaxFloat(pDX, m_fThreshold, 0.f, 1000.f);
	DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKER_THRESHOLD, m_oleThreshold);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CRepControlView, CFormView)


	//{{AFX_MSG_MAP(CRepControlView)
	//ON_BN_CLICKED(IDC_BUTTON_MODIFY, InitGUI)
	ON_BN_CLICKED(ID_BUTTON_CANCEL, OnButtonCancel)
	ON_BN_CLICKED(IDC_BUTTON_APPLY, OnButtonApply)
	ON_WM_CREATE()
	ON_COMMAND(ID_TOOLBARBUTTONADD_TERMINALREPORT, OnToolbarbuttonadd)
	ON_COMMAND(ID_TOOLBARBUTTONDEL_TERMINALREPORT, OnToolbarbuttondel)
//	ON_COMMAND(ID_PEOPLEMOVER_NEW, OnPeoplemoverNew)
//	ON_COMMAND(ID_PEOPLEMOVER_DELETE, OnPeoplemoverDelete)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_THRESHOLD, OnDeltaposSpinThreshold)
	ON_BN_CLICKED(IDC_BUTTON_LOAD_FROM_FILE, OnButtonLoadFromFile)
	ON_COMMAND(ID_REPORT_SAVE_PARA, OnReportSavePara)
	ON_COMMAND(ID_REPORT_LOAD_PARA, OnReportLoadPara)
	ON_COMMAND(ID_REPORT_SAVE_REPORT, OnReportSaveReport)
	ON_COMMAND(ID_REPORT_LOAD_REPORT, OnReportLoadReport)


	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_RADIO_DETAILED, OnRadioDetailed)
	ON_BN_CLICKED(IDC_RADIO_SUMMARY, OnRadioSummary)

	//}}AFX_MSG_MAP
	ON_UPDATE_COMMAND_UI(ID_TOOLBARBUTTONADD_TERMINALREPORT,OnUpdateBtnAdd)
	ON_UPDATE_COMMAND_UI(ID_TOOLBARBUTTONDEL_TERMINALREPORT, OnUpdateBtnDel)
//	ON_UPDATE_COMMAND_UI(ID_PEOPLEMOVER_NEW, OnUpdateBtnAddMove)
//	ON_UPDATE_COMMAND_UI(ID_PEOPLEMOVER_DELETE, OnUpdateBtnDelMove)
	
ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_PAXTYPE, OnTvnSelchangedTreePaxtype)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRepControlView diagnostics

#ifdef _DEBUG
void CRepControlView::AssertValid() const
{
	CFormView::AssertValid();
}

void CRepControlView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CRepControlView message handlers

void CRepControlView::OnInitialUpdate() 
{
	CFormView::OnInitialUpdate();
	
	// TODO: Add your specialized code here and/or call the base class
	//LONG nStyle = GetWindowLong(GetSafeHwnd(),GWL_STYLE);
	//SetWindowLong(GetSafeHwnd(),GWL_STYLE, nStyle | ~WS_HSCROLL | ~WS_VSCROLL );
	InitToolbar();


	COleDateTime oleStartTime,oleEndTime,oleInterval,oleThreshold;
	oleStartTime.SetTime( 0, 0, 0 );
	m_dtctrlStartTime.SetTime( oleStartTime);
	oleEndTime = oleStartTime + COleDateTimeSpan( 1, 0, 0, 0 );
	m_dtctrlEndTime.SetTime(oleStartTime);

	m_dtctrlStartDate.SetTime(m_oleDTStartDate);
	m_dtctrlEndDate.SetTime(m_oleDTEndDate);

	m_dtctrlInterval.SetFormat( "HH:mm" );
	oleInterval.SetTime( 1, 0, 0 );
	m_dtctrlInterval.SetTime( oleInterval );

	m_dtctrlThreshold.SetFormat( "HH:mm" );
	oleThreshold.SetTime( 1, 0, 0 );
	m_dtctrlInterval.SetTime( oleThreshold );

	//	m_spinInterval.SetRange( 0, 120 );
	m_spinThreshold.SetRange( 0, 1000 );

	m_paxConst.initDefault((InputTerminal *)&(GetDocument()->GetTerminal()));
//	LoadPaxList();
	InitCoolBtn();

//	InitComboDay(NULL);

//by aivin	
	m_startDate = GetDocument()->GetTerminal().GetSimReportManager()->GetStartDate();//GetDocument()->m_animData.m_startDate;
	
	COleDateTime ddate, tptime;
	int nIdx;
	bool bAbsDate;
	ElapsedTime ettime;	
	ettime = GetDocument()->GetTerminal().GetSimReportManager()->GetUserEndTime();
	m_startDate.GetDateTime(ettime,bAbsDate,ddate,nIdx,tptime);
	if( bAbsDate)
		nIdx = 0;
	
	m_comboStartDay.ResetContent();
	m_comboEndDay.ResetContent();

	for(int i = 0; i <= nIdx+1; i++)
	{
		char strDay[64];
		sprintf(strDay, "Day %d", i + 1);
		m_comboStartDay.InsertString(i, strDay);
		m_comboEndDay.InsertString(i, strDay);
	}
	
	if(m_comboStartDay.GetCount() > 0)
		m_comboStartDay.SetCurSel(0);
	if(m_comboEndDay.GetCount() > 0)
		m_comboEndDay.SetCurSel(m_comboEndDay.GetCount() - 1);

//	ettime = GetDocument()->GetTerminal().GetSimReportManager()->GetUserStartTime();
//	m_startDate.GetDateTime(ettime,bAbsDate,ddate,nIdx,tptime);
//	if(bAbsDate)
//	{
//		m_oleDTStartDate = ddate;
//	}
//	else
//	{
//		m_comboStartDay.SetCurSel(nIdx);
//	}
//	m_oleDTStartTime = tptime;

//	ettime = GetDocument()->GetTerminal().GetSimReportManager()->GetUserEndTime();
//	m_startDate.GetDateTime(ettime,bAbsDate,ddate,nIdx,tptime);
//	if(bAbsDate)
//	{
//		m_oleDTEndDate = ddate;
//	}
//	else
//	{
//		m_comboEndDay.SetCurSel(nIdx);
//	}
//	m_oleDTEndTime = tptime;

	UpdateData( false );
	InitControl();

//	ShowDateTypeControl(m_emDateType);
}

//by aivin add function
void CRepControlView::InitControl()
{
	if(m_startDate.IsAbsoluteDate())
	{
		m_dtctrlStartDate.ShowWindow(SW_SHOW);
		m_dtctrlEndDate.ShowWindow(SW_SHOW);
	
		m_comboStartDay.ShowWindow(SW_HIDE);
		m_comboEndDay.ShowWindow(SW_HIDE);
	}
	else								
	{	
		m_comboStartDay.ShowWindow(SW_SHOW);
		m_comboEndDay.ShowWindow(SW_SHOW);

		m_dtctrlStartDate.ShowWindow(SW_HIDE);
		m_dtctrlEndDate.ShowWindow(SW_HIDE);
	}

}

void CRepControlView::ShowDateTypeControl(const ENUM_DATE_TYPE emDateType)
{
	if(emDateType == DATETYPE_DATE)
	{
		m_dtctrlStartDate.ShowWindow(SW_SHOW);
		m_dtctrlEndDate.ShowWindow(SW_SHOW);

		m_comboStartDay.ShowWindow(SW_HIDE);
		m_comboEndDay.ShowWindow(SW_HIDE);
	}
	else if(emDateType == DATETYPE_INDEX)
	{
		m_dtctrlStartDate.ShowWindow(SW_HIDE);
		m_dtctrlEndDate.ShowWindow(SW_HIDE);

		m_comboStartDay.ShowWindow(SW_SHOW);
		m_comboEndDay.ShowWindow(SW_SHOW);
	}
}

void CRepControlView::InitComboDay(std::vector<std::string>* pvString)
{
	std::vector<std::string>* pVect = NULL;
	std::vector<std::string> vString;
	int i = 0;
	if(pvString != NULL)
		pVect = pvString;
	else
	{
		for(i = 0; i < 3; i++)
		{
			char strDay[64];
			sprintf(strDay, "Day %d", i + 1);
			vString.push_back(strDay);
		}
		pVect = &vString;
	}
	

	m_comboStartDay.ResetContent();
	m_comboEndDay.ResetContent();
	
	for(i = 0; i < static_cast<int>(pVect->size()); i++)
	{
	    m_comboStartDay.InsertString(i, pVect->at(i).c_str());
		m_comboEndDay.InsertString(i, pVect->at(i).c_str());
	}
	
	if(m_comboStartDay.GetCount() > 0)
		m_comboStartDay.SetCurSel(0);
	if(m_comboEndDay.GetCount() > 0)
		m_comboEndDay.SetCurSel(0);
}

void CRepControlView::InitCoolBtn()
{	
	/*
	m_btnMulti.AddMenuItem(SAVEPARATOFILE,	"Save Parameter To File",MF_BYCOMMAND);
	m_btnMulti.AddMenuItem(LOADPARAFROMFILE,"Load Parameter From File",MF_BYCOMMAND);
	m_btnMulti.AddMenuItem(SAVEREPORTTOFILE,"Save Report To File",MF_BYCOMMAND);
	m_btnMulti.AddMenuItem(LOADREPORTFROMFILE,"Load Parameter From File",MF_BYCOMMAND);
	*/
	m_btnMulti.SetOperation( 0 );
	m_btnMulti.SetWindowText( strBtnTxt[0] );
}

// SET UP THE CONTROL 
void CRepControlView::InitGUI() 
{
	// TODO: Add your control notification handler code here

	//ReloadSpecs();
	//CReportManager repManager=((CTermPlanDoc *)GetDocument())->m_repManager;
	//m_enumReportType=repManager.GetReportType();
	m_enumReportType= GetReportParameter()->GetReportCategory();
	//the two control not used any more
	m_lstboProType.ShowWindow( SW_HIDE );
	m_lstboPaxType.ShowWindow( SW_HIDE );

	m_treePaxType.ShowWindow(SW_SHOW);
	{

		m_staProc.ShowWindow( SW_HIDE );
		m_staticProListLabel.ShowWindow( SW_HIDE );
		m_ProToolBar.GetToolBarCtrl().ShowWindow( SW_HIDE );
		m_ProToolBar.GetToolBarCtrl().ShowWindow( SW_HIDE );
		
		if( m_enumReportType == ENUM_ACOPERATION_REP || m_enumReportType == ENUM_BAGDISTRIBUTION_REP )
		{
			m_staPax.SetWindowText( "Flight Type" );
		}
		else 
		{
			m_staPax.SetWindowText( "Passenger Type" );
		}
		m_staticTypeListLabel.ShowWindow( SW_SHOW );

		m_treePaxType.ShowWindow(SW_SHOW);
		m_PasToolBar.GetToolBarCtrl().ShowWindow( SW_SHOW );
	}
	BOOL bEnable = TRUE;
	if( m_enumReportType == ENUM_PAXLOG_REP || 
		m_enumReportType == ENUM_PAXDENS_REP ||
		m_enumReportType == ENUM_SPACETHROUGHPUT_REP ||
		m_enumReportType == ENUM_ACOPERATION_REP ||
		m_enumReportType == ENUM_BAGDISTRIBUTION_REP ||
		m_enumReportType == ENUM_COLLISIONS_REP ||
		m_enumReportType == ENUM_MISSFLIGHT_REP)
	{
		bEnable = FALSE;
	}
	m_staticType.EnableWindow( bEnable );
	m_radioDetailed.EnableWindow( bEnable );
	m_radioSummary.EnableWindow( bEnable );

	// threshold
//	bEnable = TRUE;
//	if( m_enumReportType == ENUM_PAXLOG_REP || m_enumReportType == ENUM_UTILIZATION_REP ||
//		m_enumReportType == ENUM_ACTIVEBKDOWN_REP || m_enumReportType == ENUM_PAXDENS_REP )
//		bEnable = FALSE;
//	m_staticThreshold.EnableWindow( bEnable );
//	m_edtThreshold.EnableWindow( bEnable );
//	m_chkThreshold.EnableWindow( bEnable );
	//Modified by Xie Bo 2002.4.21
	//For Threshold int/float/time type
	switch( m_enumReportType )
	{
	case ENUM_DISTANCE_REP:
	case ENUM_AVGQUEUELENGTH_REP:
	case ENUM_QUEUELENGTH_REP:
	case ENUM_BAGCOUNT_REP:
	case ENUM_THROUGHPUT_REP:
	case ENUM_BILLBOARD_EXPOSUREINCIDENCE_REP:
	case ENUM_BILLBOARD_LINKEDPROCINCREMENTVISIT_REP:
	case ENUM_PAXCOUNT_REP:
	case ENUM_FIREEVACUTION_REP:
	case ENUM_CONVEYOR_WAIT_LENGTH_REP:
		m_staticThreshold.EnableWindow( TRUE );
		m_edtThreshold.EnableWindow( TRUE );
		m_spinThreshold.EnableWindow( TRUE );
		m_chkThreshold.EnableWindow( TRUE );
		m_dtctrlThreshold.EnableWindow( FALSE );
		m_dtctrlThreshold.ShowWindow( SW_HIDE );
		m_edtThreshold.ShowWindow(SW_SHOW);
		m_spinThreshold.ShowWindow( SW_SHOW );
		break;
	case ENUM_BAGWAITTIME_REP:
	case ENUM_DURATION_REP:
	case ENUM_QUEUETIME_REP:
	case ENUM_SERVICETIME_REP:
	case ENUM_CONVEYOR_WAIT_TIME_REP:
		m_staticThreshold.EnableWindow( TRUE );
		m_edtThreshold.EnableWindow( FALSE );
		m_spinThreshold.EnableWindow( FALSE );
		m_chkThreshold.EnableWindow( TRUE );
		m_dtctrlThreshold.EnableWindow( TRUE );
		m_dtctrlThreshold.ShowWindow( SW_SHOW );
		m_edtThreshold.ShowWindow(SW_HIDE);
		m_spinThreshold.ShowWindow( SW_HIDE );
		break;
	case ENUM_COLLISIONS_REP:
		m_edtThreshold.EnableWindow(TRUE);
		m_edtThreshold.SetWindowText("1");
		m_staticThreshold.EnableWindow(TRUE);
		m_spinThreshold.EnableWindow(TRUE);
		m_dtctrlThreshold.EnableWindow( FALSE );
		m_dtctrlThreshold.ShowWindow( SW_HIDE );
		m_spinThreshold.ShowWindow(SW_SHOW);
		m_chkThreshold.SetCheck(TRUE);
		m_chkThreshold.EnableWindow(FALSE);
		break;
	case ENUM_RETAIL_REP:
		m_edtThreshold.EnableWindow(FALSE);
		m_edtThreshold.SetWindowText("1");
		m_staticThreshold.EnableWindow(FALSE);
		m_spinThreshold.EnableWindow(FALSE);
		m_dtctrlThreshold.EnableWindow( FALSE );
		m_dtctrlThreshold.ShowWindow( SW_HIDE );
		m_spinThreshold.ShowWindow(SW_SHOW);
		m_chkThreshold.SetCheck(TRUE);
		m_chkThreshold.EnableWindow(FALSE);
		break;
	default:
//		m_staticThreshold.EnableWindow( FALSE );
		m_edtThreshold.EnableWindow( FALSE );
		m_spinThreshold.EnableWindow( FALSE );
		m_chkThreshold.EnableWindow( FALSE );
		m_dtctrlThreshold.EnableWindow( FALSE );
		m_dtctrlThreshold.ShowWindow( SW_HIDE );
		m_edtThreshold.ShowWindow(SW_SHOW);
		m_spinThreshold.ShowWindow( SW_SHOW );
	}

	// interval 
	bEnable = FALSE;
	if( m_enumReportType == ENUM_QUEUELENGTH_REP || 
		m_enumReportType == ENUM_PAXDENS_REP || 
		m_enumReportType == ENUM_BAGDISTRIBUTION_REP|| 
		m_enumReportType == ENUM_BAGCOUNT_REP || 
		m_enumReportType == ENUM_PAXCOUNT_REP|| 
		m_enumReportType == ENUM_SPACETHROUGHPUT_REP || 
		m_enumReportType == ENUM_COLLISIONS_REP || 
		m_enumReportType == ENUM_ACOPERATION_REP || 	
		m_enumReportType == ENUM_THROUGHPUT_REP	|| 
		m_enumReportType == ENUM_CONVEYOR_WAIT_LENGTH_REP || 
		m_enumReportType == ENUM_BILLBOARD_EXPOSUREINCIDENCE_REP|| 
		m_enumReportType == ENUM_BILLBOARD_LINKEDPROCINCREMENTVISIT_REP || 
		m_enumReportType == ENUM_RETAIL_REP ||
		m_enumReportType == ENUM_PAXLOG_REP)
		bEnable = TRUE;
	m_staticInterval.EnableWindow( bEnable );
	m_dtctrlInterval.EnableWindow(bEnable);

	// area

	if(m_enumReportType == ENUM_COLLISIONS_REP)
	{
//		m_staticThreshold.SetWindowText("Threashold(meter)");

		m_staticArea.ShowWindow( SW_SHOW );
		m_comboAreaPortal.ShowWindow( SW_SHOW );
		m_staticArea.SetWindowText( "Areas:" );

		CAreas* pAreas = ((CTermPlanDoc*)GetDocument())->GetTerminal().m_pAreas;
		int nCount = pAreas->m_vAreas.size();
		m_comboAreaPortal.ResetContent();
		int nIdx = m_comboAreaPortal.AddString("Without Area");
		m_comboAreaPortal.SetItemData(nIdx ,-2);
		m_comboAreaPortal.SetCurSel(nIdx);
		for( int i=0; i<nCount; i++ )
		{		
			CString csStr = pAreas->m_vAreas[i]->name;

			//m_comboAreaPortal.AddString( csStr );
			//m_comboAreaPortal.SetItemData( i, i );
			
			 nIdx = m_comboAreaPortal.AddString( csStr );
			m_comboAreaPortal.SetItemData( nIdx, i );
			
		}
	} else 	if( m_enumReportType == ENUM_PAXDENS_REP )
	{
		m_staticArea.ShowWindow( SW_SHOW );
		m_comboAreaPortal.ShowWindow( SW_SHOW );
		m_staticArea.SetWindowText( "Areas:" );

		CAreas* pAreas = ((CTermPlanDoc*)GetDocument())->GetTerminal().m_pAreas;
		int nCount = pAreas->m_vAreas.size();
		m_comboAreaPortal.ResetContent();
		for( int i=0; i<nCount; i++ )
		{		
			CString csStr = pAreas->m_vAreas[i]->name;

			//m_comboAreaPortal.AddString( csStr );
			//m_comboAreaPortal.SetItemData( i, i );
			
			int nIdx = m_comboAreaPortal.AddString( csStr );
			m_comboAreaPortal.SetItemData( nIdx, i );
			
		}

	}
	else if( m_enumReportType == ENUM_SPACETHROUGHPUT_REP )
	{
		m_staticArea.SetWindowText( "Portals:" );
		m_staticArea.ShowWindow( SW_SHOW );
		m_comboAreaPortal.ShowWindow( SW_SHOW );

		//CPortals portals = ((CTermPlanDoc*)GetDocument())->m_portals;
		CPortals* pPortals = &((CTermPlanDoc*)GetDocument())->m_portals;;
		int nCount = pPortals->m_vPortals.size();
		m_comboAreaPortal.ResetContent();
		for( int i=0; i<nCount; i++ )
		{		
			CString csStr = pPortals->m_vPortals[i]->name;
			
			int nIdx = m_comboAreaPortal.AddString( csStr );
			m_comboAreaPortal.SetItemData( nIdx, i );
			
			//m_comboAreaPortal.AddString( csStr );
			//m_comboAreaPortal.SetItemData( i, i );
		}
	}
	else
	{
		m_staticArea.ShowWindow( SW_HIDE );
		m_comboAreaPortal.ShowWindow( SW_HIDE );
	}

	
	UpdateData(TRUE);
	if( m_enumReportType == ENUM_BAGDELIVTIME_REP || m_enumReportType == ENUM_BAGDISTRIBUTION_REP || m_enumReportType == ENUM_PAXDENS_REP || m_enumReportType == ENUM_SPACETHROUGHPUT_REP || m_enumReportType == ENUM_COLLISIONS_REP)
			m_nRepType = 0;
	else 
			m_nRepType = 1;

	m_btLoad.ShowWindow( bShowLoadButton ? SW_SHOW : SW_HIDE );
	UpdateData( false );
	/*
	m_ReportSpecs.m_nRepType=m_nRepType;
	m_ReportSpecs.m_oleEndTime=m_oleDTEndTime;
	m_ReportSpecs.m_oleStartTime=m_oleDTStartTime;
	m_ReportSpecs.m_paxConst=m_paxConst;
	m_ReportSpecs.m_strProcessor=m_strProcessor;
	//m_ReportSpecs.m_nAreaPortalIdx=m_comboAreaPortal.GetCurSel();
	m_ReportSpecs.m_nAreaPortalIdx = m_comboAreaPortal.GetItemData ( m_comboAreaPortal.GetCurSel() );
	*/

}


void CRepControlView::OnButtonCancel() 
{
	// TODO: Add your control notification handler code here
	CReportParameter* pReportPara = GetReportPara();
	/*
	m_nRepType=m_ReportSpecs.m_nRepType;
	m_oleDTEndTime=m_ReportSpecs.m_oleEndTime;
	m_oleDTStartTime=m_ReportSpecs.m_oleStartTime;
	m_oleDTInterval=m_ReportSpecs.m_oleInterval;
	m_paxConst=m_ReportSpecs.m_paxConst;
	m_strProcessor=m_ReportSpecs.m_strProcessor;
	*/

	pReportPara->GetReportType( m_nRepType );
	ElapsedTime tempTime;
	pReportPara->GetEndTime( tempTime );
	m_oleDTEndTime.SetTime( tempTime.asHours(), (long)tempTime.asMinutes() % 60 , (long)tempTime.asSeconds() % 60 );

	pReportPara->GetStartTime( tempTime );
	m_oleDTStartTime.SetTime( tempTime.asHours(), (long)tempTime.asMinutes() % 60 , (long)tempTime.asSeconds() % 60 );
	
	long lTemp;
	pReportPara->GetInterval( lTemp );
	tempTime.set( lTemp );
	m_oleDTInterval.SetTime( tempTime.asHours(), (long)tempTime.asMinutes() % 60 , (long)tempTime.asSeconds() % 60 );

	std::vector<CMobileElemConstraint> vPaxType;
	pReportPara->GetPaxType( vPaxType );
	m_paxConst.freeConstraints();
	int iSize = vPaxType.size();
	for( int i=0; i<iSize; ++i )
	{
		m_paxConst.addConstraint( vPaxType[i] );
	}

	std::vector<ProcessorID>vProcGroup ;
	pReportPara->GetProcessorGroup( vProcGroup );
	iSize = vProcGroup.size();
	if( iSize <= 0 )
	{
		m_strProcessor="ALL PROCESSORS";
	}
	else
	{
		int ii=0;
		for( ; ii<iSize-1; ++ii )
		{
			m_strProcessor += vProcGroup[ii].GetIDString();
			m_strProcessor += ";";
		}
		m_strProcessor += vProcGroup[ii].GetIDString();
	}	
	
	LoadTreeContent(pReportPara);
	////CReportManager repManager=((CTermPlanDoc *)GetDocument())->m_repManager;
	////m_enumReportType=repManager.GetReportType();
	//m_enumReportType= GetReportPara()->GetReportCategory();
	//switch( m_enumReportType )
	//{
	//case ENUM_PAXLOG_REP:
	//case ENUM_DISTANCE_REP:
	//case ENUM_QUEUETIME_REP:
	//case ENUM_DURATION_REP:
	//case ENUM_SERVICETIME_REP:
	//case ENUM_ACTIVEBKDOWN_REP:
	//case ENUM_PAXCOUNT_REP:
	//case ENUM_PAXDENS_REP:
	//case ENUM_BAGWAITTIME_REP:
	//case ENUM_SPACETHROUGHPUT_REP:		
	//case ENUM_BAGDELIVTIME_REP:
	//case ENUM_FIREEVACUTION_REP:
	//case ENUM_CONVEYOR_WAIT_TIME_REP:
	//case ENUM_MISSFLIGHT_REP:
	//	LoadPaxList();
	//	break;
	//case ENUM_ACOPERATION_REP:
	//case ENUM_BAGDISTRIBUTION_REP:
	//	LoadFltList();
	//	break;
	//default:
	//	// should load proc list
	//	break;
 //   }
	
	UpdateData(FALSE);
	//LoadProcList();
}


void CRepControlView::OnButtonApply() 
{
	// because after open the report view, the current sim_result can been changed,
	// must call SetCurrentSimResult() and SetCurrentReportType() before load report by system , apply report

	UpdateData(TRUE);

	GetParaFromGUI(GetReportParameter());

	//////////////////////////////////////////////////////////////////////////
	int iCurrentSimIdx = GetDocument()->GetTerminal().getCurrentSimResult();
	if( iCurrentSimIdx < 0 )
	{
		AfxMessageBox("No Valid SimResult!", MB_OK|MB_ICONERROR);
		return;
	}
	
	GetDocument()->GetTerminal().GetSimReportManager()->SetCurrentSimResult( iCurrentSimIdx );
	GetDocument()->GetTerminal().GetSimReportManager()->SetCurrentReportType( GetReportPara()->GetReportCategory() );
	//////////////////////////////////////////////////////////////////////////

	m_enumReportType= GetReportPara()->GetReportCategory();

	CString str;
	m_comboAreaPortal.GetWindowText(str);
	str.TrimLeft();
	str.TrimRight();	
	if( (m_enumReportType == ENUM_PAXDENS_REP || m_enumReportType == ENUM_COLLISIONS_REP ) && str.GetLength() == 0 )
	{
			MessageBox( "No Area has been Selected.", "Error", MB_OK|MB_ICONWARNING );
			return;
	}

	if( m_enumReportType == ENUM_SPACETHROUGHPUT_REP && str.GetLength() == 0 )
	{
			MessageBox( "No Portal has been Selected.", "Error", MB_OK|MB_ICONWARNING );
			return;
	}

	if(m_enumReportType == ENUM_QUEUELENGTH_REP || m_enumReportType == ENUM_BAGCOUNT_REP || m_enumReportType == ENUM_PAXDENS_REP 
		|| m_enumReportType == ENUM_PAXCOUNT_REP || m_enumReportType == ENUM_SPACETHROUGHPUT_REP || m_enumReportType == ENUM_BAGCOUNT_REP 
		|| m_enumReportType == ENUM_COLLISIONS_REP || m_enumReportType == ENUM_ACOPERATION_REP || m_enumReportType == ENUM_BAGDISTRIBUTION_REP
		|| m_enumReportType == ENUM_CONVEYOR_WAIT_LENGTH_REP)
	{
		if(GetTotalMinutes(m_oleDTInterval)>120 || GetTotalMinutes(m_oleDTInterval)<1)
		{
			MessageBox( "You should set interval between 00:01 and 02:00", "Error", MB_OK|MB_ICONWARNING );
			return;
		}
	}

	//////////////////////////////////////////////////////////////////////////
	if (!MultiRunValid())
	{
		int nCurrentSimResult = ((CTermPlanDoc *)GetDocument())->GetTerminal().getCurrentSimResult();
		m_vReportRuns.clear();
		m_vReportRuns.push_back(nCurrentSimResult);
	}

	if(m_vReportRuns.size() == 0)
	{
		MessageBox( "You should select at least one result to do report.", "Error", MB_OK|MB_ICONWARNING );
		return;
	}
	long intervalTime = 0 ;
	GetReportPara()->GetInterval(intervalTime) ;
	if(intervalTime == 0)
	{
		MessageBox( "Interval time should not be '0'.", "Error", MB_OK|MB_ICONWARNING );
		return;
	}
	if(m_vReportRuns.size() > 1)//multiple run report
	{
		int iDetailOrSummary = 0;
		if(GetReportParameter()->GetReportType(iDetailOrSummary))
		{
			if(iDetailOrSummary == 0)//detail
			{
				if(m_enumReportType == ENUM_DISTANCE_REP ||
					m_enumReportType == ENUM_QUEUETIME_REP ||
					m_enumReportType == ENUM_SERVICETIME_REP||
					m_enumReportType == ENUM_DURATION_REP ||
					m_enumReportType == ENUM_ACTIVEBKDOWN_REP ||
					m_enumReportType == ENUM_PAXCOUNT_REP ||
					m_enumReportType == ENUM_PAXDENS_REP)
				{

				}
				else
				{
					MessageBox(_T("This type report cannot do mulitple run report."),"Error", MB_OK|MB_ICONWARNING );
					return;
				}

			}
			else
			{
				if(m_enumReportType == ENUM_QUEUETIME_REP ||
					m_enumReportType == ENUM_AVGQUEUELENGTH_REP ||
					m_enumReportType == ENUM_SERVICETIME_REP)
				{

				}
				else
				{
					MessageBox(_T("This type report cannot do mulitple run report."),"Error", MB_OK|MB_ICONWARNING );
						return;
				}

			}
		}
		if(m_enumReportType == ENUM_DISTANCE_REP ||
			m_enumReportType == ENUM_QUEUETIME_REP ||
			m_enumReportType == ENUM_SERVICETIME_REP||
			m_enumReportType == ENUM_DURATION_REP ||
			m_enumReportType == ENUM_ACTIVEBKDOWN_REP ||
			m_enumReportType == ENUM_PAXCOUNT_REP ||
			m_enumReportType == ENUM_PAXDENS_REP ||
			m_enumReportType == ENUM_AVGQUEUELENGTH_REP )
		{

		}
		else
		{
			MessageBox(_T("This type report cannot do mulitple run report."),"Error", MB_OK|MB_ICONWARNING );
				return;
		}
		
	}

	//////////////////////////////////////////////////////////////////////////
	CReportParameter* pReportPara = GetReportPara();	
	assert( pReportPara );

	GetDocument()->GetTerminal().GetSimReportManager()->getCurReportItem()->SaveReportParaToFile( ((CTermPlanDoc *)GetDocument())->m_ProjInfo.path , pReportPara );
	GetDocument()->GetTerminal().GetSimReportManager()->getCurReportItem()->setCreateFlag( true );
	GetDocument()->GetTerminal().GetSimReportManager()->saveDataSet( GetDocument()->m_ProjInfo.path , false);
	GetDocument()->GetTerminal().m_pReportParaDB->saveDataSet( GetDocument()->m_ProjInfo.path, false );

	GetDocument()->GetARCReportManager().SetLoadReportType( load_by_system );	// 0= load_by_system
	try
	{
		GetDocument()->GenerateReport();	
	}
	catch(ARCException* e)
	{
		AfxMessageBox(e->getErrorMsg());
		delete e;
	}

	//////////////////////////////////////////////////////////////////////////
	m_bCanToSave = true;
	m_strCurReportFile = GetDocument()->GetTerminal().GetSimReportManager()->GetCurrentReportFileName( GetDocument()->m_ProjInfo.path );
}

bool CRepControlView::MultiRunValid()
{
	int iDetailOrSummary = 0;
	if(GetReportParameter()->GetReportType(iDetailOrSummary))
	{
		if(iDetailOrSummary == 0)//detail
		{
			if(m_enumReportType == ENUM_DISTANCE_REP ||
				m_enumReportType == ENUM_QUEUETIME_REP ||
				m_enumReportType == ENUM_SERVICETIME_REP||
				m_enumReportType == ENUM_DURATION_REP ||
				m_enumReportType == ENUM_ACTIVEBKDOWN_REP ||
				m_enumReportType == ENUM_PAXCOUNT_REP ||
				m_enumReportType == ENUM_PAXDENS_REP)
			{
				return true;
			}
		}
		else
		{
			if(m_enumReportType == ENUM_QUEUETIME_REP ||
				m_enumReportType == ENUM_AVGQUEUELENGTH_REP ||
				m_enumReportType == ENUM_SERVICETIME_REP)
			{
				return true;
			}
		}
	}
	if(m_enumReportType == ENUM_DISTANCE_REP ||
		m_enumReportType == ENUM_QUEUETIME_REP ||
		m_enumReportType == ENUM_SERVICETIME_REP||
		m_enumReportType == ENUM_DURATION_REP ||
		m_enumReportType == ENUM_ACTIVEBKDOWN_REP ||
		m_enumReportType == ENUM_PAXCOUNT_REP ||
		m_enumReportType == ENUM_PAXDENS_REP ||
		m_enumReportType == ENUM_AVGQUEUELENGTH_REP )
	{
		return true;
	}

	return false;
}


void CRepControlView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	if(IsWindowVisible())
	{
		//AfxMessageBox("CRepControl View OnUpdate!");
//		ReloadSpecs();
	}
	
}


// move window
void CRepControlView::ReloadSpecs()
{
	CRect rc;
	GetClientRect(&rc);

	SizeDownCtrl( rc.Width(), rc.Height() );
	InitGUI();
}


void CRepControlView::Clear()
{
	switch( m_enumReportType )
	{
	case ENUM_COLLISIONS_REP:
		//m_lstboPaxType.ResetContent();
		break;
	case ENUM_PAXLOG_REP:
	case ENUM_DISTANCE_REP:
	case ENUM_DURATION_REP:
	case ENUM_ACTIVEBKDOWN_REP:
	case ENUM_PAXCOUNT_REP:
	case ENUM_PAXDENS_REP:
	case ENUM_SPACETHROUGHPUT_REP:
	case ENUM_BAGDELIVTIME_REP:
	case ENUM_FIREEVACUTION_REP:
	case ENUM_MISSFLIGHT_REP:
	case ENUM_THROUGHPUT_REP:
	case ENUM_RETAIL_REP:
		m_paxConst.initDefault( &GetDocument()->GetTerminal() );
		//LoadPaxList();
		break;
	case ENUM_ACOPERATION_REP:
	case ENUM_BAGDISTRIBUTION_REP:
		m_vFltList.clear();
		//LoadFltList();
		break;

	case ENUM_UTILIZATION_REP:
//	case ENUM_THROUGHPUT_REP:
	case ENUM_BILLBOARD_EXPOSUREINCIDENCE_REP:
	case ENUM_BILLBOARD_LINKEDPROCINCREMENTVISIT_REP:
	case ENUM_QUEUELENGTH_REP:
	case ENUM_AVGQUEUELENGTH_REP:
	case ENUM_BAGCOUNT_REP:
	case ENUM_CONVEYOR_WAIT_LENGTH_REP:
		//LoadProcList();
		break;

	default:
		m_paxConst.initDefault( &GetDocument()->GetTerminal() );
		//LoadPaxList();
		//LoadProcList();
		break;
    }
	ReadDataFromLib();
	//////////////////////////////////////////////////////////////////////////
	m_bCanToSave = false;
	m_strCurReportFile = "";
}



void CRepControlView::ResetAllContent()
{
	UpdateData();
	m_nInterval = 1;
	m_oleDTInterval.SetTime( 0,0,0 );
	m_fThreshold = 0.0f;
	m_oleThreshold.SetTime( 0,0,0 );
    UpdateData( FALSE );
	
	m_enumReportType = ((CTermPlanDoc *)GetDocument())->GetARCReportManager().GetReportType();
	switch( m_enumReportType )
	{
	case ENUM_THROUGHPUT_REP:
	case ENUM_RETAIL_REP:
		break ;
	case ENUM_DISTANCE_REP:	
		{
		CUnitsManager* pUM = CUnitsManager::GetInstance();
		int nUnit = pUM->GetLengthUnits();				
		CString str = "Threshold (" + pUM->GetLengthUnitString( nUnit ) + ")";
//		m_staticThreshold.SetWindowText( str );
		break;
		}
	case ENUM_AVGQUEUELENGTH_REP:
	case ENUM_QUEUELENGTH_REP:
	case ENUM_BAGCOUNT_REP:
	case ENUM_BILLBOARD_EXPOSUREINCIDENCE_REP:
	case ENUM_BILLBOARD_LINKEDPROCINCREMENTVISIT_REP:
	case ENUM_PAXCOUNT_REP:
	case ENUM_CONVEYOR_WAIT_LENGTH_REP:
		m_paxConst.initDefault((InputTerminal *)&(GetDocument()->GetTerminal()));
//		m_staticThreshold.SetWindowText( "Threshold (count)" );
		break;
	case ENUM_BAGWAITTIME_REP:
	case ENUM_DURATION_REP:
	case ENUM_QUEUETIME_REP:
	case ENUM_SERVICETIME_REP:
	case ENUM_FIREEVACUTION_REP:
	case ENUM_CONVEYOR_WAIT_TIME_REP:
		m_paxConst.initDefault((InputTerminal *)&(GetDocument()->GetTerminal()));
//		m_staticThreshold.SetWindowText( "Threshold (min)" );
		break;
	default:
		m_paxConst.initDefault((InputTerminal *)&(GetDocument()->GetTerminal()));
//		m_staticThreshold.SetWindowText( "Threshold" );
		break;
	}

	//Modified by Xie Bo 2002.4.19
	//Set Pax Type to Default
	//and Clear Processor

	//LoadPaxList();
	m_strProcessor="";

	ReloadSpecs();
	LoadTreeContent(NULL);

}
int CRepControlView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CFormView::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here
	if (!m_PasToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_PasToolBar.LoadToolBar(IDR_ADDDELEDITTOOLBAR_TERMINALREPORT))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}
	if (!m_ProToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_ProToolBar.LoadToolBar(IDR_PEOPLEMOVEBAR))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}
	
	return 0;
}

void CRepControlView::InitToolbar()
{
	CRect rc;
	m_toolbarcontenter1.GetWindowRect( &rc );
	ScreenToClient(&rc);
	m_PasToolBar.MoveWindow(rc.left,rc.top,50,rc.Height());
	m_PasToolBar.ShowWindow(SW_SHOW);
	m_toolbarcontenter1.ShowWindow(SW_HIDE);
	m_PasToolBar.GetToolBarCtrl().HideButton( ID_TOOLBARBUTTONEDIT_TERMINALREPORT);
	m_PasToolBar.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONADD_TERMINALREPORT,FALSE);
	m_PasToolBar.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONDEL_TERMINALREPORT,FALSE);

	m_toolbarcontenter2.GetWindowRect( &rc );
	ScreenToClient(&rc);
	m_ProToolBar.MoveWindow(rc.left,rc.top,50,rc.Height());
	m_ProToolBar.ShowWindow(SW_SHOW);
	m_toolbarcontenter2.ShowWindow(SW_HIDE);
	m_ProToolBar.GetToolBarCtrl().HideButton( ID_PEOPLEMOVER_CHANGE);

}

void CRepControlView::SetPaxTreeToDefaultOrAnothers(BOOL is_or_not)
{
	HTREEITEM item = m_treePaxType.GetChildItem(m_hPaxTypeRoot) ;
	while(item != NULL)
	{
		HTREEITEM node = NULL ;
		if(_tcscmp(m_treePaxType.GetItemText(item),_T("DEFAULT")) == 0 && is_or_not == FALSE)
		{

			m_paxConst.deleteConst(m_treePaxType.GetItemData(item)) ;
			m_treePaxType.DeleteItem(item) ;
			break ;
		}
		if(_tcscmp(m_treePaxType.GetItemText(item),_T("DEFAULT")) != 0 && is_or_not == TRUE)
		{

			m_paxConst.deleteConst(m_treePaxType.GetItemData(item)) ;
			node = item ;			
		}
		item = m_treePaxType.GetNextSiblingItem(item) ;
		if(node != NULL)
			m_treePaxType.DeleteItem(node) ;
	}
}
void CRepControlView::OnToolbarbuttonadd() 
{
	// TODO: Add your command handler code here


	HTREEITEM hCurItem=m_treePaxType.GetSelectedItem();
	if(hCurItem == NULL)
	{
		return;
	}
	TreeItemType enumItemType  =(TreeItemType)m_treePaxType.GetItemData(hCurItem);
	
	switch (enumItemType)
	{
	case PAX_TYPE_Root:
		{
			CPassengerTypeDialog dlg( this );
			if( dlg.DoModal() == IDOK )
			{	
				CString szPaxType;
				CMobileElemConstraint mobileConstr = dlg.GetMobileSelection();
				int nCount = m_paxConst.getCount();
				for( int i=0; i<nCount; i++ )
				{
					const CMobileElemConstraint * pPaxConstr = m_paxConst.getConstraint( i );
					if( mobileConstr.isEqual(pPaxConstr) )
					{
						mobileConstr.screenPrint( szPaxType );
						CString csMsg;
						csMsg.Format( "Pax Type : %s exists in the list", szPaxType.GetBuffer(0) );
						MessageBox( csMsg, "Error", MB_OK|MB_ICONWARNING );
						return;
					}
				}
				m_paxConst.addConstraint( mobileConstr );

				mobileConstr.screenPrint( szPaxType );
				HTREEITEM item = TreeInsertItem(szPaxType,hCurItem);
				m_treePaxType.SetItemData(item , (DWORD)PAX_TYPE) ;
			}
		}
		break;
	case PROCESSOR_ROOT:
		{
			CProcesserDialog dlg( &GetDocument()->GetTerminal() );
			if (m_enumReportType == ENUM_RETAIL_REP)
			{
				dlg.SetType(RetailProc);
			}

			if( dlg.DoModal() == IDOK )
			{
				ProcessorID id;
				ProcessorIDList idList;
				if( dlg.GetProcessorIDList(idList) )
				{
					int nIDcount = idList.getCount();
					for(int i = 0; i < nIDcount; i++)
					{
						id = *idList[i];
						// check if exist in the current list.
						int nProcCount = m_vProcList.size();
						bool bFind = false;
						for( int j=0; j<nProcCount; j++ )
						{
							if( m_vProcList[j] == id )
							{
								bFind = true;
								break;
							}
						}
						if( bFind )
							continue;
						if(m_vProcList.size() == 0 )
						{
							HTREEITEM item = m_treePaxType.GetChildItem(hCurItem) ;
							if(item != NULL)
								m_treePaxType.DeleteItem(item) ;
						}
						m_vProcList.push_back( id );
						id.SetStrDict(GetDocument()->GetTerminal().inStrDict );
						TCHAR szBuf[256]= {0};
						id.printID(szBuf);
						HTREEITEM item = TreeInsertItem(szBuf,hCurItem);
						m_treePaxType.SetItemData(item ,(DWORD)PROCESSOR );

					}
				}
			}
		}
		break;

	case Flight_Type_Root:
		{
			CFlightDialog dlg( this );
			if( dlg.DoModal() == IDOK )
			{
				FlightConstraint newFltConstr = dlg.GetFlightSelection();
				int nCount = m_vFltList.size();
				for( int i=0; i<nCount; i++ )
				{
					if( m_vFltList[i].isEqual( &newFltConstr ) )
					{
						CString szPaxType;
						//char szPaxType[128];
						//newFltConstr.screenPrint( szPaxType, 128 );
						newFltConstr.screenPrint( szPaxType, 0, 128 );
						CString csMsg;
						csMsg.Format( "Flight Type : %s exists in the list", szPaxType.GetBuffer(0) );
						MessageBox( csMsg, "Error", MB_OK|MB_ICONWARNING );
						return;
					}
				}
				m_vFltList.push_back( newFltConstr );
				CString str;
				newFltConstr.screenPrint( str,0, 128 );
				HTREEITEM item = TreeInsertItem(str,hCurItem);
				m_treePaxType.SetItemData(item ,(DWORD)Flight_Type );

			}
		}
		break;
	case PROCESSOR_FROM:
		{
			CProcesserDialog dlg( &GetDocument()->GetTerminal() );
			if( dlg.DoModal() == IDOK )
			{
				ProcessorIDList idList;
				ProcessorID id;
				if( dlg.GetProcessorIDList(idList) )
				{
					int nIDcount = idList.getCount();
					for(int i = 0; i < nIDcount; i++)
					{							
						id = *idList[i];
						char szBuf[256];
						id.printID(szBuf);

						CReportParameter::FROM_TO_PROCS _fromToProcs;
						GetReportParameter()->GetFromToProcs(_fromToProcs);

						_fromToProcs.m_vFromProcs.push_back(id);
			
						GetReportParameter()->SetFromToProcs(_fromToProcs);
						HTREEITEM hItem = TreeInsertItem(szBuf,hCurItem);
						m_treePaxType.SetItemData(hItem,(DWORD)PROCESSOR);
					}
				}
			}
		}
		break;
	case PROCESSOR_TO:
		{
			CProcesserDialog dlg( &GetDocument()->GetTerminal() );
			if( dlg.DoModal() == IDOK )
			{
				ProcessorIDList idList;
				ProcessorID id;
				if( dlg.GetProcessorIDList(idList) )
				{
					int nIDcount = idList.getCount();
					for(int i = 0; i < nIDcount; i++)
					{							
						id = *idList[i];
						char szBuf[256];
						id.printID(szBuf);

						CReportParameter::FROM_TO_PROCS _fromToProcs;
						GetReportParameter()->GetFromToProcs(_fromToProcs);

						_fromToProcs.m_vToProcs.push_back(id);
						
						GetReportParameter()->SetFromToProcs(_fromToProcs);
						HTREEITEM hItem = TreeInsertItem(szBuf,hCurItem);
						m_treePaxType.SetItemData(hItem,(DWORD)PROCESSOR);
					}
				}
			}
		}
		break;

	default:
		{
			ASSERT(0);
			break;
		}
	}
	m_treePaxType.Expand(hCurItem,TVE_EXPAND);
}

void CRepControlView::OnToolbarbuttondel() 
{
	HTREEITEM hCurItem=m_treePaxType.GetSelectedItem();
	if(hCurItem == NULL)
	{
		return;
	}
	TreeItemType enumItemType  =(TreeItemType)m_treePaxType.GetItemData(hCurItem);

	switch (enumItemType)
	{
	case PAX_TYPE:
		{
			int nIndex = GetTreeItemIndex(hCurItem);
			if(nIndex >=0)
			{
				m_paxConst.deleteConst(nIndex) ;
				m_treePaxType.DeleteItem(hCurItem) ;
			}

		}
		break;
	case PROCESSOR:
		{

			HTREEITEM hParentItem = m_treePaxType.GetParentItem(hCurItem);
			if(hParentItem != NULL)
			{
				int nIndex = GetTreeItemIndex(hCurItem);
				if(nIndex >=0)
				{
					if(nIndex < static_cast<int>(m_vProcList.size()))
					{
						m_vProcList.erase( m_vProcList.begin() + nIndex);
						m_treePaxType.DeleteItem(hCurItem) ;
					}
				}

				//int nProcTypeCount = m_vProcList.size();
				//if( nProcTypeCount == 0 )
				//{	
				//	
				//	m_treePaxType.DeleteItem(hCurItem);
				//	HTREEITEM hItem = TreeInsertItem( "All Processors", hParentItem);
				//	m_treePaxType.SetItemData( hItem, PROCESSOR);
				//}
			}


		}
		break;
	case Flight_Type:
		{
			int nIndex = GetTreeItemIndex(hCurItem);
			if(nIndex >=0)
			{
				if(nIndex < static_cast<int>(m_vFltList.size()))
				{
					m_vFltList.erase( m_vFltList.begin() + nIndex );
					m_treePaxType.DeleteItem(hCurItem) ;

				}
			}
		}
		break;

	case PROCESSOR_FROM_PROC:
		{

			int nIndex = GetTreeItemIndex(hCurItem);
			if(nIndex >=0)
			{
				CReportParameter::FROM_TO_PROCS _fromToProcs;
				GetReportParameter()->GetFromToProcs(_fromToProcs);

				if(nIndex < static_cast<int>(_fromToProcs.m_vFromProcs.size()))
				{
					_fromToProcs.m_vFromProcs.erase(_fromToProcs.m_vFromProcs.begin()+nIndex);
					m_treePaxType.DeleteItem(hCurItem);
					GetReportParameter()->SetFromToProcs(_fromToProcs);
				}
			}
		}
		break;

	case PROCESSOR_TO_PROC:
		{
			int nIndex = GetTreeItemIndex(hCurItem);
			if(nIndex >=0)
			{
				CReportParameter::FROM_TO_PROCS _fromToProcs;
				GetReportParameter()->GetFromToProcs(_fromToProcs);

				if(nIndex < static_cast<int>(_fromToProcs.m_vToProcs.size()))
				{
					_fromToProcs.m_vToProcs.erase(_fromToProcs.m_vToProcs.begin()+nIndex);
					m_treePaxType.DeleteItem(hCurItem);
					GetReportParameter()->SetFromToProcs(_fromToProcs);
				}
			}
		}
		break;
	default :
		{
			ASSERT(0);
		}
		break;
	}

	//m_treePaxType.Expand()
}

void CRepControlView::OnDeltaposSpinThreshold(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	// TODO: Add your control notification handler code here
	int	nLowerBound;
	int	nUpperBound;

	m_spinThreshold.GetRange( nLowerBound, nUpperBound );
	if( pNMUpDown->iDelta > 0 )
	{
		m_fThreshold  = static_cast<float>(pNMUpDown->iPos + pNMUpDown->iDelta);
		m_fThreshold  = m_fThreshold > nUpperBound ? nUpperBound : m_fThreshold;
	}
	else if( pNMUpDown->iDelta < 0 && pNMUpDown->iPos > nLowerBound )
	{
		m_fThreshold = static_cast<float>(pNMUpDown->iPos + pNMUpDown->iDelta);
		m_fThreshold = m_fThreshold < nLowerBound ? nLowerBound : m_fThreshold;
	}	
	*pResult = 0;	
}

int CRepControlView::GetTotalMinutes(COleDateTime _oleDT)
{
	return _oleDT.GetHour()*60+_oleDT.GetMinute();
}
//////////////////////////////////////////////////////////////////////////
void CRepControlView::getReportName(CString &strReportType)
{

	switch( m_enumReportType ) 
	{
	case ENUM_PAXLOG_REP:
		strReportType.LoadString( IDS_TVNN_REP_PAX_LOG );
		break;
		
	case ENUM_DISTANCE_REP:
		strReportType.LoadString( IDS_TVNN_REP_PAX_DISTANCE );
		break;
		
	case ENUM_QUEUETIME_REP:
		strReportType.LoadString( IDS_TVNN_REP_PAX_TIMEINQ );
		break;
		
	case ENUM_DURATION_REP:
		strReportType.LoadString( IDS_TVNN_REP_PAX_TIMEINT );
		break;
		
	case ENUM_SERVICETIME_REP:
		strReportType.LoadString( IDS_TVNN_REP_PAX_TIMEINS );
		break;
		
	case ENUM_ACTIVEBKDOWN_REP:
		strReportType.LoadString( IDS_TVNN_REP_PAX_BREAKDOWN );
		break;
		
	case ENUM_UTILIZATION_REP:
		strReportType.LoadString( IDS_TVNN_REP_PROC_UTILISATION );
		break;
		
	case ENUM_THROUGHPUT_REP:
		strReportType.LoadString( IDS_TVNN_REP_PROC_THROUGHPUT );
		break;
		
	case ENUM_RETAIL_REP:
		strReportType = "Retail Activity";
		break;
	case ENUM_QUEUELENGTH_REP:
		strReportType.LoadString( IDS_TVNN_REP_PROC_QLENGTH );
		break;
		
	case ENUM_AVGQUEUELENGTH_REP:
		strReportType.LoadString( IDS_TVNN_REP_PROC_QUEUEPARAM );
		break;
		
	case ENUM_PAXCOUNT_REP:
		strReportType.LoadString( IDS_TVNN_REP_SPACE_OCCUPANCY );
		break;
		
	case ENUM_PAXDENS_REP:
		strReportType.LoadString( IDS_TVNN_REP_SPACE_DENSITY );
		break;
		
	case ENUM_SPACETHROUGHPUT_REP:
		strReportType.LoadString( IDS_TVNN_REP_SPACE_THROUGHPUT );
		break;
		
	case ENUM_COLLISIONS_REP:
		strReportType.LoadString( IDS_TVNN_REP_SPACE_COLLISIONS );
		break;
		
	case ENUM_ACOPERATION_REP:
		strReportType.LoadString( IDS_TVNN_REP_AIRCRAFT );
		break;
		
	case ENUM_BAGCOUNT_REP:
		strReportType.LoadString( IDS_TVNN_REP_BAG_COUNT );
		break;
		
	case ENUM_BAGWAITTIME_REP:
		strReportType.LoadString( IDS_TVNN_REP_BAG_WAITTIME );
		break;
		
	case ENUM_BAGDISTRIBUTION_REP:
		strReportType.LoadString( IDS_TVNN_REP_BAG_DISTRIBUTION );
		break;
		
	case ENUM_BAGDELIVTIME_REP:
		strReportType.LoadString( IDS_TVNN_REP_BAG_DELIVERYTIME );
		break;
		
	case ENUM_ECONOMIC_REP:
		strReportType.LoadString( IDS_TVNN_REP_ECONOMIC );
		break;

	case ENUM_FIREEVACUTION_REP:
		strReportType.LoadString( IDS_TVNN_REP_FIREEVACUTION );
		break;

	case ENUM_CONVEYOR_WAIT_LENGTH_REP:
		strReportType.LoadString( IDS_TVNN_REP_CONVEYOR_WAIT_LENGTH );
		break;

	case ENUM_CONVEYOR_WAIT_TIME_REP:
		strReportType.LoadString( IDS_TVNN_REP_CONVEYOR_WAIT_TIME );
		break;
	case ENUM_BILLBOARD_EXPOSUREINCIDENCE_REP:
		strReportType = _T("Exposure Incidence");
		break;
	case ENUM_BILLBOARD_LINKEDPROCINCREMENTVISIT_REP:
		strReportType = _T("Incremental Visit");
		break;
	default:
		strReportType = "ERROR!";
		break;
	}	
	return;
}

void CRepControlView::ReadDataFromLib()
{
	SetGUIFromPara(GetReportParameter(), GetDocument()->GetTerminal().GetSimReportManager()->GetStartDate() );	
}

void CRepControlView::OnButtonLoadFromFile() 
{
	// because after open the report view, the current sim_result can been changed,
	// must call SetCurrentSimResult() and SetCurrentReportType() before load report by system , apply report
	int iCurrentSimIdx = GetDocument()->GetTerminal().getCurrentSimResult();
	if( iCurrentSimIdx < 0 )
	{
		AfxMessageBox("No Valid SimResult!", MB_OK|MB_ICONERROR);
		return;
	}
	
	GetDocument()->GetTerminal().GetSimReportManager()->SetCurrentSimResult( iCurrentSimIdx );
	GetDocument()->GetTerminal().GetSimReportManager()->SetCurrentReportType( GetReportPara()->GetReportCategory() );
	//////////////////////////////////////////////////////////////////////////
	
	CReportItem* pReportItem = GetDocument()->GetTerminal().GetSimReportManager()->getCurReportItem();
	ASSERT( pReportItem );
	CReportParameter* pReportPara = pReportItem->GetReoprtParaFromFile( GetDocument()->m_ProjInfo.path , (InputTerminal*)&(GetDocument()->GetTerminal()) );
	//GetDocument()->SetReportPara( pReportPara );
	CopyParaData( pReportPara, GetReportParameter() );
	delete pReportPara;	
	Clear();

	GetDocument()->GetARCReportManager().SetLoadReportType( load_by_system );

	GetDocument()->UpdateAllViews( NULL );

	//////////////////////////////////////////////////////////////////////////
	m_bCanToSave = true;
	m_strCurReportFile = GetDocument()->GetTerminal().GetSimReportManager()->GetCurrentReportFileName( GetDocument()->m_ProjInfo.path );
}

//////////////////////////////////////////////////////////////////////////
// save & load para
// save & load report
//////////////////////////////////////////////////////////////////////////
void CRepControlView::OnReportSavePara() 
{
	m_btnMulti.SetOperation( 0 );
	m_btnMulti.SetWindowText( strBtnTxt[0] );
	OnClickMultiBtn();
}

void CRepControlView::OnReportLoadPara() 
{
	
	m_btnMulti.SetOperation( 1 );
	m_btnMulti.SetWindowText( strBtnTxt[1] );
	OnClickMultiBtn();
}

void CRepControlView::OnReportSaveReport() 
{
	
	m_btnMulti.SetOperation( 2 );
	m_btnMulti.SetWindowText( strBtnTxt[2] );
	OnClickMultiBtn();
}

void CRepControlView::OnReportLoadReport() 
{
	
	m_btnMulti.SetOperation( 3 );
	m_btnMulti.SetWindowText( strBtnTxt[3] );
	OnClickMultiBtn();
}

void CRepControlView::OnClickMultiBtn() 
{
	int iOperation = m_btnMulti.GetOperation();
	switch( iOperation )
	{
	case 0:	//SAVEPARATOFILE:
		SavePara();
		break;
	case 1:	//LOADPARAFROMFILE:
		LoadPara();
		break;
	case 2:	//SAVEREPORTTOFILE:
		SaveReport();
		break;
	case 3:	//LOADREPORTFROMFILE:
		LoadReport();
		break;
	default:
		assert(0);
	}
}

void CRepControlView::SavePara()
{
	GetParaFromGUI(GetReportParameter());

	CFileDialog savedlg(FALSE,".par",NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		"Report Parameter(*.par)|*.par||",this, 0, FALSE );
	if( savedlg.DoModal() == IDOK )
	{
		CString strFileName = savedlg.GetPathName();
		CReportParameter* pReportPara = GetReportPara();	
		assert( pReportPara );
		pReportPara->SaveReportParaToFile( strFileName );
	}
}

void CRepControlView::LoadPara()
{	
	CFileDialog loaddlg(TRUE,NULL,NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		"Report Parameter(*.par)|*.par||",this, 0, FALSE );
	if( loaddlg.DoModal() == IDOK )
	{
		CString strFileName = loaddlg.GetPathName();
		CReportParameter* pPara = NULL;
		try
		{
			pPara = CReportParameter::LoadReoprtParaFromFile( strFileName, &GetDocument()->GetTerminal() );
		}
		catch (FileFormatError* _pError)
		{
			char szBuf[128];
			_pError->getMessage( szBuf );
			AfxMessageBox( szBuf, MB_OK|MB_ICONWARNING );
			delete _pError;
			return;
		}
		if( pPara )
		{
			CReportParameter* pReportPara = GetReportParameter();	
			assert( pReportPara );

			CopyParaData( pPara ,pReportPara );
			SetGUIFromPara(GetReportParameter(), GetDocument()->GetTerminal().GetSimReportManager()->GetStartDate());
			delete pPara;
		}
	}

}

void CRepControlView::SaveReport()
{
	if( !m_bCanToSave )
	{
		AfxMessageBox("Can not save report result!",MB_OK|MB_ICONINFORMATION );
		return;
	}
	
	CString strExten	= getExtensionString();
	CString strFilter	= "Report File(*." + strExten + ")|*." + strExten + "|All Type(*.*)|*.*||";
	CFileDialog savedlg(FALSE,strExten,NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,strFilter,this, 0 ,FALSE );
	if( savedlg.DoModal() == IDOK )
	{
		CString strFileName = savedlg.GetPathName();
		CopyFile( m_strCurReportFile, strFileName, FALSE);
	}

}
	
void CRepControlView::LoadReport()
{
	CString strExten	= getExtensionString();
	CString strFilter	= "Report File(*." + strExten + ")|*." + strExten + "|All Type(*.*)|*.*||";
	CFileDialog loaddlg(TRUE,NULL,NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, strFilter,this, 0, FALSE );

	if( loaddlg.DoModal() == IDOK )
	{
		CString strFileName = loaddlg.GetPathName();
		if( !CheckReportFileFormat( strFileName ) )
		{
			AfxMessageBox("The report file format is error. Can not load the report!",MB_OK|MB_ICONINFORMATION );
			return;
		}
		
		GetDocument()->GetARCReportManager().SetLoadReportType( load_by_user );	// 1 = load_by_user
		GetDocument()->GetARCReportManager().SetUserLoadReportFile( strFileName );

		GetDocument()->UpdateAllViews( NULL );

		m_bCanToSave = true;
		m_strCurReportFile = strFileName;
	}
}

void CRepControlView::GetParaFromGUI(CReportParameter* pReportPara)
{
//	CReportParameter* pReportPara = GetReportPara();	
	assert( pReportPara );
	//waiting by aivin.
	ElapsedTime time;
	bool bAbsDate = false;
	int nDtIdx=0;
	if (m_startDate.IsAbsoluteDate())
	{
		bAbsDate = true;
		m_startDate.GetElapsedTime(bAbsDate,m_oleDTStartDate,nDtIdx,m_oleDTStartTime,time);	
		pReportPara->SetStartTime( time );	
		m_startDate.GetElapsedTime(bAbsDate,m_oleDTEndDate,nDtIdx,m_oleDTEndTime,time);	
		pReportPara->SetEndTime( time );	
	}
	else
	{
		bAbsDate = false;
		nDtIdx = m_comboStartDay.GetCurSel();
		m_startDate.GetElapsedTime(bAbsDate,m_oleDTStartDate,nDtIdx,m_oleDTStartTime,time);	
		pReportPara->SetStartTime( time );
		
		nDtIdx = m_comboEndDay.GetCurSel();
		m_startDate.GetElapsedTime(bAbsDate,m_oleDTEndDate,nDtIdx,m_oleDTEndTime,time);	
		pReportPara->SetEndTime( time );	
	}
//	time.set( m_oleDTStartTime.GetHour(), m_oleDTStartTime.GetMinute(), m_oleDTStartTime.GetSecond()  );
//	pReportPara->SetStartTime( time );	
//	time.set( m_oleDTEndTime.GetHour(), m_oleDTEndTime.GetMinute(), m_oleDTEndTime.GetSecond() );
//	pReportPara->SetEndTime( time );
	
	time.set( m_oleDTInterval.GetHour(), m_oleDTInterval.GetMinute(), m_oleDTInterval.GetSecond() );
	pReportPara->SetInterval( time.asSeconds() );
	
	pReportPara->SetReportType( m_nRepType );
	
	
	long lThreshold;
	if( m_chkThreshold.GetCheck() == 0 )	// uncheck
	{
		lThreshold = LONG_MAX;
	}
	else
	{
		if( m_dtctrlThreshold.IsWindowEnabled() )
		{
			time.set( m_oleThreshold.GetHour(), m_oleThreshold.GetMinute(), m_oleThreshold.GetSecond() );
			lThreshold = time.asSeconds();
		}
		else
		{
			lThreshold = (long) m_fThreshold;	
		}
	}
	pReportPara->SetThreshold( lThreshold );
	
	//runs
	if(m_hRunsRoot)
	{
		m_vReportRuns.clear();
		HTREEITEM hRunItem = m_treePaxType.GetChildItem(m_hRunsRoot);

		int nCurrentItem = 0;
		int NSingleSelectRunIndex = 0 ;
		while(hRunItem != NULL)
		{
			if(m_treePaxType.IsCheckItem(hRunItem))
			{
				m_vReportRuns.push_back(nCurrentItem);
				NSingleSelectRunIndex = nCurrentItem ;
			}
			nCurrentItem += 1;
			hRunItem = m_treePaxType.GetNextSiblingItem(hRunItem);
		}
		if(m_vReportRuns.size() == 1)
			GetDocument()->GetTerminal().setCurrentSimResult(NSingleSelectRunIndex) ;
	}

	pReportPara->SetReportRuns(m_vReportRuns);
	
	//pax type
	std::vector<CMobileElemConstraint> mobList;
	switch( m_enumReportType)
	{
	//case ENUM_THROUGHPUT_REP:
	//	for( int i=0; i<m_paxConst.getCount(); i++)
	//	{
	//		pReportPara->Thrognput_pax.push_back( *(m_paxConst.getConstraint(i)));
	//	}
	//	break;
	case ENUM_UTILIZATION_REP:
	//case ENUM_THROUGHPUT_REP:
	case ENUM_BILLBOARD_EXPOSUREINCIDENCE_REP:
	case ENUM_BILLBOARD_LINKEDPROCINCREMENTVISIT_REP:
	case ENUM_QUEUELENGTH_REP:
	case ENUM_AVGQUEUELENGTH_REP:
	case ENUM_BAGCOUNT_REP:
	case ENUM_CONVEYOR_WAIT_LENGTH_REP:
		break;
		
	case ENUM_ACOPERATION_REP:
	case ENUM_BAGDISTRIBUTION_REP:
		// flight constraint
		{
			for( int i=0; i<static_cast<int>(m_vFltList.size()); i++ )
			{
				CMobileElemConstraint mob(NULL);
				(FlightConstraint&)mob= m_vFltList[i];
				
				mob.SetInputTerminal(&GetDocument()->GetTerminal());
				mobList.push_back( mob );
			}
			pReportPara->SetPaxType( mobList );
			break;
		}
	default:
		{	
			for( int i=0; i<m_paxConst.getCount(); i++)
			{
				mobList.push_back( *(m_paxConst.getConstraint(i)));
			}
			pReportPara->SetPaxType( mobList );
			break;
		}
	}
	
	pReportPara->SetProcessorGroup( m_vProcList );
	
	std::vector<CString> strAreaPotal;
	if( m_comboAreaPortal.IsWindowEnabled() )
	{
		CString str;
		m_comboAreaPortal.GetWindowText(str);
		str.TrimLeft();
		str.TrimRight();
		
		if(! str.IsEmpty() )
		{
			strAreaPotal.push_back( str );
			pReportPara->SetAreas( strAreaPotal );
			pReportPara->SetPortals( strAreaPotal );
		}
	}
}

void CRepControlView::SetGUIFromPara(CReportParameter* pReportPara, const CStartDate& _startDate )
{
//	CReportParameter* pReportPara = GetReportPara();	
	assert( pReportPara );

	ElapsedTime time;
	// time
	//
	bool bAbsDate;
	COleDateTime ddate;
	int nIdx;
	COleDateTime tptime;
	if( pReportPara->GetStartTime( time ) )
	{
		_startDate.GetDateTime(time,bAbsDate,ddate,nIdx,tptime);
		if(bAbsDate)
		{
			m_oleDTStartDate = ddate;
		}
		else
		{
			m_comboStartDay.SetCurSel(nIdx);
		}
		m_oleDTStartTime = tptime;
	}	
	
	if( pReportPara->GetEndTime( time ) )
	{
		_startDate.GetDateTime(time,bAbsDate,ddate,nIdx,tptime);
		if(bAbsDate)
		{
			m_oleDTEndDate = ddate;
		}
		else
		{
			m_comboEndDay.SetCurSel(nIdx);
		}
		m_oleDTEndTime = tptime;
	}

//	if( pReportPara->GetStartTime( time ) )
//		m_oleDTStartTime.SetTime( time.asHours(), ((int)time.asMinutes()) % 60, ((int)time.asSeconds()) % 60 );
//	if( pReportPara->GetEndTime( time ) )
//		m_oleDTEndTime.SetTime( time.asHours(), ((int)time.asMinutes()) % 60, ((int)time.asSeconds()) % 60 );

	long second;
	if( pReportPara->GetInterval( second ))
	{
		time = second;
		m_oleDTInterval.SetTime( time.asHours(), ((int)time.asMinutes()) % 60, ((int)time.asSeconds()) % 60 );
	}
	
	//report type
	int iReportType;
	if( pReportPara->GetReportType( iReportType ) )
		m_nRepType = iReportType;
	//throdhold
	long lThreshold;
	if( pReportPara->GetThreshold( lThreshold) )
	{
		if( lThreshold == LONG_MAX )
		{
			m_chkThreshold.SetCheck( 0 );
			time = 0l;
		}
		else
		{
			if( lThreshold == 0 )
			{
				m_chkThreshold.SetCheck( 0 );
			}
			else
			{
				m_chkThreshold.SetCheck( 1 );
			}			
			time = lThreshold;
			
			if( lThreshold >= 1000 )
				m_fThreshold = 0 ;
			else
				m_fThreshold = static_cast<float>(lThreshold);
		}
		
		m_oleThreshold.SetTime( time.asHours(), ((int)time.asMinutes()) % 60, ((int)time.asSeconds()) % 60 );
	}

	LoadTreeContent(pReportPara);

	// area
	std::vector<CString> m_vArea;
	if( pReportPara->GetAreas( m_vArea ) )
	{
		if( m_vArea.size() >= 1)
		{
			CString strArea = m_vArea[0];
			m_comboAreaPortal.SetCurSel( -1 );
			for( int i=0; i<m_comboAreaPortal.GetCount(); i++ )
			{
				CString str;
				m_comboAreaPortal.GetLBText(i,str );
				if( strArea ==  str)
				{
					m_comboAreaPortal.SetCurSel( i );
					break;
				}
			}
		}
	}

	//portal
	std::vector<CString> m_vPortals;
	if( pReportPara->GetPortals( m_vPortals ) )
	{
		if( m_vArea.size() >= 1)
		{
			CString stPortal = m_vArea[0];
			m_comboAreaPortal.SetCurSel( -1 );
			for( int i=0; i<m_comboAreaPortal.GetCount(); i++ )
			{
				CString str;
				m_comboAreaPortal.GetLBText(i,str);
				if( stPortal == str )
				{
					m_comboAreaPortal.SetCurSel( i );
					break;
				}
			}
		}
	}

	UpdateData( FALSE );
}

void CRepControlView::CopyParaData( CReportParameter* _pCopyPara,CReportParameter* _pToPara )
{
	ElapsedTime time;
	if( _pCopyPara->GetStartTime( time ) )
		_pToPara->SetStartTime( time );
	if( _pCopyPara->GetEndTime( time ) )
		_pToPara->SetEndTime( time );
	long lTemp;
	if( _pCopyPara->GetInterval(lTemp) )
		_pToPara->SetInterval( lTemp );
	
	int iTemp;
	if( _pCopyPara->GetReportType( iTemp ))
		_pToPara->SetReportType( iTemp );
	if( _pCopyPara->GetThreshold( lTemp) )
		_pToPara->SetThreshold( lTemp );
	
	// pax type
	std::vector<CMobileElemConstraint> mobList;
	if( _pCopyPara->GetPaxType( mobList ) )
		_pToPara->SetPaxType( mobList );
	
	// processor type
	std::vector<ProcessorID> vProcessorList;
	if( _pCopyPara->GetProcessorGroup( vProcessorList ) )
		_pToPara->SetProcessorGroup( vProcessorList );
	
	// area
	std::vector<CString> vArea;
	if( _pCopyPara->GetAreas( vArea ) )
		_pToPara->SetAreas( vArea );
	
	//portal
	std::vector<CString> vPortals;
	if( _pCopyPara->GetPortals( vPortals ) )
		_pToPara->SetPortals( vPortals );

	//runs
	std::vector<int > vRuns;
	if( _pCopyPara->GetReportRuns( vRuns ) )
		_pToPara->SetReportRuns( vRuns );

	
}

CString CRepControlView::getExtensionString()
{
	CString strExtension;
	switch( m_enumReportType )
	{
	case ENUM_BAGDELIVTIME_REP:
		strExtension = "bdt";
		break;
		
	case ENUM_BAGDISTRIBUTION_REP:
		strExtension = "bdr";
		break;
		
	case ENUM_ACOPERATION_REP:
		strExtension = "aor";
		break;
	case ENUM_PAXLOG_REP:
		strExtension = "pld";
		break;
		
	case ENUM_UTILIZATION_REP:
		strExtension = "utd";
		break;
	case ENUM_DISTANCE_REP:
		strExtension = "dsd";
		break;
	case ENUM_PAXCOUNT_REP:
		strExtension = "pcd";
		break;
	case ENUM_QUEUETIME_REP:
		strExtension = "qtd";
		break;
	case ENUM_CONVEYOR_WAIT_TIME_REP:
		strExtension = "cwtd";
		break;
	case ENUM_THROUGHPUT_REP:
		strExtension = "thd";
		break;
	case ENUM_RETAIL_REP:
		strExtension = "ret";
		break;
	case ENUM_QUEUELENGTH_REP:
		strExtension = "qld";
		break;
	case ENUM_CONVEYOR_WAIT_LENGTH_REP:
		strExtension = "cwld";
		break;
	case ENUM_AVGQUEUELENGTH_REP:
		strExtension = "qad";
		break;
	case ENUM_DURATION_REP:
		strExtension = "drd";
		break;
	case ENUM_SERVICETIME_REP:
		strExtension = "srd";
		break;
	case ENUM_ACTIVEBKDOWN_REP:
		strExtension = "acd";
		break;
	case ENUM_BAGWAITTIME_REP:
		strExtension = "bgd";
		break;
	case ENUM_BAGCOUNT_REP:
		strExtension = "bcd";
		break;
	case ENUM_PAXDENS_REP:			//not defined in the "Filelist", temp extension used: "txt"
		strExtension = "pdd";
		break;
	case ENUM_SPACETHROUGHPUT_REP:
		strExtension = "std";
		break;
	case ENUM_COLLISIONS_REP:
		strExtension = "crf";
		break;
	default:
		strExtension = "txt";
		break;
	}

	return strExtension;
}

// open the file, and get the 
bool CRepControlView::CheckReportFileFormat( const CString& _strFileName  )
{
	ArctermFile file;
	if( file.init( _strFileName, READ ) == FALSE )
		return false;
	
	char szBuf[256];
	file.getLine();
	file.getField( szBuf, 256 );
	CString strFileTile = CString( szBuf );
	strFileTile.MakeUpper();
	CString strTile = GetReportTitle();
	strTile.MakeUpper();
	if( strFileTile != strTile )
	{
		file.endFile();
		return false;
	}

	file.endFile();	
	return true;
}

CString CRepControlView::GetReportTitle()
{	
	Terminal* m_pTerm =  &(GetDocument()->GetTerminal());

	CBaseReport* pPara = NULL;
	CString strPath = GetDocument()->m_ProjInfo.path;
	switch( m_enumReportType )
	{
	case ENUM_PAXLOG_REP:
		pPara= new CPrintLog(m_pTerm, strPath );						
		break;
	case ENUM_UTILIZATION_REP:
		pPara= new CUtilizationReport(m_pTerm, strPath );						
		break;
	case ENUM_THROUGHPUT_REP:
		pPara = new CThroughputReport(m_pTerm, strPath );						
		break;
	case ENUM_RETAIL_REP:
		pPara = new CRetailReport(m_pTerm,strPath);
		break;
	case ENUM_QUEUELENGTH_REP:
		pPara = new CQueueLengthReport(m_pTerm, strPath );						
		break;
	case ENUM_CONVEYOR_WAIT_LENGTH_REP:
		pPara = new CConveyorWaitLenghtReport( m_pTerm, strPath );
		break;
	case ENUM_AVGQUEUELENGTH_REP:
		pPara = new CAverageQueueLengthReport(m_pTerm, strPath );			
		break;
	case ENUM_DISTANCE_REP:
		pPara = new CDistanceReport(m_pTerm, strPath );			
		break;
	case ENUM_PAXCOUNT_REP:
		pPara = new CPaxCountReport(m_pTerm, strPath );						
		break;
	case ENUM_QUEUETIME_REP:
		pPara = new CQueueReport(m_pTerm, strPath );					    
		break;
	case ENUM_CONVEYOR_WAIT_TIME_REP:
		pPara = new CConveyorWaitTimeReport(m_pTerm, strPath );					    
		break;
	case ENUM_DURATION_REP:
		pPara = new CDurationReport(m_pTerm, strPath );						
		break;
	case ENUM_SERVICETIME_REP:
		pPara = new CServiceReport(m_pTerm, strPath );					    
		break;
	case ENUM_ACTIVEBKDOWN_REP:
		pPara = new CActivityBreakdownReport(m_pTerm, strPath );						
		break;
		
	case ENUM_BAGWAITTIME_REP:
		pPara = new CBagWaitReport(m_pTerm, strPath );						
		break;
		
	case ENUM_BAGCOUNT_REP:
		pPara = new CBagCountReport(m_pTerm, strPath );						
		break;
		
	case ENUM_PAXDENS_REP:
		pPara = new CPassengerDensityReport(m_pTerm, strPath );						
		break;
	case ENUM_SPACETHROUGHPUT_REP:
		pPara = new CSpaceThroughputReport(m_pTerm, strPath );						
		break;
	case ENUM_COLLISIONS_REP:
		pPara = new CCollisionReport(m_pTerm, strPath, 1);			
		break;
	case  ENUM_ACOPERATION_REP:
		pPara = new CAcOperationsReport(m_pTerm, strPath );						
		break;
	case ENUM_BAGDISTRIBUTION_REP:
		pPara = new CBagDistReport(m_pTerm, strPath );						
		break;
	case ENUM_BAGDELIVTIME_REP:
		pPara = new CBagDeliveryTimeReport(m_pTerm, strPath );						
		break;
	case ENUM_FIREEVACUTION_REP:
		pPara = new CFireEvacutionReport( m_pTerm, strPath );
		break;
	case ENUM_BILLBOARD_EXPOSUREINCIDENCE_REP:
		pPara = new CBillboardExposureIncidenceReport( m_pTerm, strPath );
		break;
	case ENUM_BILLBOARD_LINKEDPROCINCREMENTVISIT_REP:
		pPara = new CBillboardLinkedProcIncrementalVisitReport( m_pTerm, strPath );
		break;
	case ENUM_MISSFLIGHT_REP:
		pPara = new CMissFlightReport(m_pTerm, strPath);
		break;
	default:
		assert(0);
		break;
	}		
	
	CString strTitle = pPara->GetTitle();
	delete pPara;

	return strTitle;	
}

void CRepControlView::OnSize(UINT nType, int cx, int cy) 
{
	CFormView::OnSize(nType, cx, cy);
	
	DoSizeWindow(cx, cy);	
}

void CRepControlView::DoSizeWindow( int cx, int cy )
{
	int x ,y;
	int iWidth,iHeight;
	CRect rc; 
	CWnd* pWnd = NULL;
	
	if( m_staticTime.m_hWnd == NULL )
		return;

	m_staticType.GetWindowRect(&rc);

	// time	//ok
	x = iEdgeWidth;
	y = iEdgeWidth;
	iWidth = cx-2*iEdgeWidth - rc.Width();//(cx-2*iEdgeWidth)*7/10;//(cx-3*iEdgeWidth)/2
	iHeight = 60;
	m_staticTime.MoveWindow( x,y, iWidth, iHeight );
	
	x = 42;
	y += 16;
	iWidth = iWidth - x;
	
//	if(m_emDateType == DATETYPE_DATE)
		m_dtctrlStartDate.MoveWindow(x,y,iWidth/2, 19);
//	else if(m_emDateType == DATETYPE_INDEX)
		m_comboStartDay.MoveWindow(x,y,iWidth/2, 19);
	m_dtctrlStartTime.MoveWindow(x+iWidth/2,y,iWidth/2, 19 );
	
	y += 21;
//	if(m_emDateType == DATETYPE_DATE)
		m_dtctrlEndDate.MoveWindow(x,y,iWidth/2, 19);
//	else if(m_emDateType == DATETYPE_INDEX)
		m_comboEndDay.MoveWindow(x,y,iWidth/2, 19);
	m_dtctrlEndTime.MoveWindow(x+iWidth/2,y,iWidth/2,19);

	// report type //ok
	iWidth = rc.Width();//(cx-2*iEdgeWidth)*3/10;
	x = cx-iEdgeWidth-iWidth;
	y = iEdgeWidth;

	m_staticType.MoveWindow(x,y, iWidth, iHeight );
	
	m_radioDetailed.MoveWindow( x+10, y+16, (iWidth-12),15);
	m_radioSummary.MoveWindow( x+10, y+16+21,(iWidth-12),15);


	// Threshold,  //ok
	x = iEdgeWidth;
	y = iEdgeWidth + iHeight + 2;
	iWidth = cx-2*iEdgeWidth;
	m_staticThreshold.MoveWindow(x,y, iWidth, 32);

	x = 52;
	y += 10;
	iWidth = (cx-3*iEdgeWidth)*4/10;
	m_dtctrlInterval.MoveWindow(x,y,iWidth,19 );
	

	pWnd = (CWnd*)GetDlgItem(IDC_STATIC_THRESHOLD2);
	pWnd->GetWindowRect(&rc);
	x += iWidth+iEdgeWidth;
	pWnd->MoveWindow(x, y, rc.Width(), rc.Height());

	x += rc.Width() + iEdgeWidth;
	m_staticThreshold.GetWindowRect(&rc);
	CRect rctemp;
	m_chkThreshold.GetWindowRect(&rctemp);
	m_chkThreshold.MoveWindow( x+4,y,rctemp.Width(),rctemp.Height());//15
	m_edtThreshold.MoveWindow( x+20,y, rc.Width()-(x+20),20);
	m_spinThreshold.MoveWindow( rc.right-28,y,20,20);
	m_spinThreshold.SetBuddy(&m_edtThreshold);
	m_dtctrlThreshold.MoveWindow( x+20,y, rc.Width()-(x+20),20);

	//////////////////////////////////////////////////////////////////////////
	// move down_ctrl
	SizeDownCtrl( cx, cy );
	
}	

void CRepControlView::SizeDownCtrl(int cx, int cy )
{
	m_enumReportType = ((CTermPlanDoc *)GetDocument())->GetARCReportManager().GetReportType();

	int x,y;
	x = iEdgeWidth;
	y = 111;
	int iWidth, iHeight;
	iWidth = cx-2*iEdgeWidth;
	iHeight = cy-y-70;
	CRect rc;

	//switch( m_enumReportType )
	//{
	//case ENUM_PAXLOG_REP:
	//case ENUM_ACTIVEBKDOWN_REP:
	//case ENUM_PAXCOUNT_REP:
	//case ENUM_PAXDENS_REP:
	//case ENUM_SPACETHROUGHPUT_REP:
	//case ENUM_ACOPERATION_REP:
	//case ENUM_BAGDISTRIBUTION_REP:
	//case ENUM_BAGDELIVTIME_REP:
	//case ENUM_COLLISIONS_REP:	// show pax list only
	//case ENUM_FIREEVACUTION_REP:
	//case ENUM_MISSFLIGHT_REP:
	//	m_staticTypeListLabel.MoveWindow( x, y, iWidth , iHeight );
	//	m_lstboPaxType.MoveWindow( x,y+iToolBarHeight,iWidth, iHeight-iToolBarHeight );
	//	break;
	//case ENUM_THROUGHPUT_REP:
	//case ENUM_DISTANCE_REP:
	//case ENUM_DURATION_REP:
		m_staticTypeListLabel.MoveWindow( x, y, iWidth , iHeight );
		//m_lstboPaxType.MoveWindow( x,y+iToolBarHeight,iWidth, iHeight-iToolBarHeight );
		m_treePaxType.MoveWindow( x,y+iToolBarHeight,iWidth, iHeight-iToolBarHeight );
	//	break;
	//case ENUM_UTILIZATION_REP:
	//case ENUM_BILLBOARD_EXPOSUREINCIDENCE_REP:
	//case ENUM_BILLBOARD_LINKEDPROCINCREMENTVISIT_REP:
	//case ENUM_QUEUELENGTH_REP:
	//case ENUM_AVGQUEUELENGTH_REP:
	//case ENUM_BAGCOUNT_REP:		// show proc list only
	//case ENUM_CONVEYOR_WAIT_LENGTH_REP:
	//	m_staticProListLabel.MoveWindow(  x, y, iWidth, iHeight );
	//	m_lstboProType.MoveWindow(  x,y+iToolBarHeight,iWidth, iHeight-iToolBarHeight );
	//	m_staPax.GetWindowRect(&rc);
	//	ScreenToClient(&rc);
	//	m_staProc.MoveWindow(&rc);
	//	m_PasToolBar.GetWindowRect(&rc);
	//	ScreenToClient(&rc);
	//	m_ProToolBar.MoveWindow(&rc);
	//	break;

	//default:				// show pax and proc list
	//	iWidth = (cx-4*iEdgeWidth)/2;
	//	m_staticTypeListLabel.MoveWindow( x, y, iWidth, iHeight );
	//	m_lstboPaxType.MoveWindow( x, y+iToolBarHeight, iWidth, iHeight-iToolBarHeight );
	//	
	//	x = x+ iWidth + 2* iEdgeWidth;
	//	m_staticProListLabel.MoveWindow( x, y, iWidth, iHeight );
	//	m_lstboProType.MoveWindow( x,y+iToolBarHeight,iWidth,iHeight-iToolBarHeight );

	//	m_staProc.MoveWindow(x,y-8, 80, 15);
	//	m_ProToolBar.MoveWindow( x+1, y+6, 50, iToolBarHeight );
	//	break;
	//}
	
	//////////////////////////////////////////////////////////////////////////
	//move area
	x = iEdgeWidth;
	y = y+ iHeight;
	m_staticArea.MoveWindow( x, y, 30, 15);
	m_comboAreaPortal.MoveWindow( x, y+ 18, cx-2*iEdgeWidth,20 );
	//////////////////////////////////////////////////////////////////////////
	// move button
	x = iEdgeWidth;
	y = y+ 42;
	iHeight = 22;
	m_btnMulti.MoveWindow( x, y, 110, iHeight );
	
	iWidth = cx/6;
	x = cx - iEdgeWidth - iWidth;
	m_btnCancel.MoveWindow( x,y, iWidth, iHeight );

	x = x - iWidth - 5;
	m_btnApply.MoveWindow( x, y, iWidth, iHeight );

	x = x - iWidth - 5;
	m_btLoad.MoveWindow( x, y, iWidth, iHeight );

	//////////////////////////////////////////////////////////////////////////	
	SetScaleToFitSize(CSize(cx,cy));
}

void CRepControlView::OnRadioDetailed() 
{
	//Add your code here
	
}

void CRepControlView::OnRadioSummary() 
{
	//Add your code here
}

void CRepControlView::OnUpdateBtnAdd(CCmdUI* pCmdUI)
{
	HTREEITEM hCurItem=m_treePaxType.GetSelectedItem();
	if(hCurItem == NULL)
	{
		pCmdUI->Enable(FALSE);
		return;
	}
	if(hCurItem == m_hPaxTypeRoot ||
		hCurItem == m_hProcRoot||
		hCurItem == m_hFlightTypeRoot||		
		hCurItem == m_hItemFrom||
		hCurItem == m_hItemTo
		)
	{
		pCmdUI->Enable(TRUE);
	}
	else
	{
		pCmdUI->Enable(FALSE);
	}
}

void CRepControlView::OnUpdateBtnDel(CCmdUI* pCmdUI)
{
	//if(m_treePaxType.GetSafeHwnd())
	
	HTREEITEM hCurItem=m_treePaxType.GetSelectedItem();
	if(hCurItem == NULL)
	{
		pCmdUI->Enable(FALSE);
		return;
	}

	HTREEITEM hParentItem = m_treePaxType.GetParentItem(hCurItem);
	if(hParentItem == NULL)
	{
		pCmdUI->Enable(FALSE);
		return;
	}

	if(hParentItem == m_hPaxTypeRoot ||
		hParentItem == m_hProcRoot||
		hParentItem == m_hFlightTypeRoot||		
		hParentItem == m_hItemFrom||
		hParentItem == m_hItemTo
		)
	{
		pCmdUI->Enable(TRUE);
	}
	else
	{
		pCmdUI->Enable(FALSE);
	}
}

CReportParameter* CRepControlView::GetReportParameter()
{
	return GetDocument()->GetARCReportManager().GetReportPara();
}

CReportParameter* CRepControlView::GetReportPara()
{
	return ((CTermPlanDoc *)GetDocument())->GetARCReportManager().GetReportPara();
}
void CRepControlView::LoadTreeContent( CReportParameter* pReportPara )
{
	m_hSelectedItem = NULL;

	m_treePaxType.SetRedraw(FALSE);
	m_treePaxType.DeleteAllItems();

	m_hPaxTypeRoot= NULL;
	m_hProcRoot= NULL;
	m_hRunsRoot= NULL;
	m_hFlightTypeRoot= NULL;
	m_hItemTo= NULL;
	m_hItemFrom= NULL;
	m_hProcToProc = NULL;


	switch (m_enumReportType)
	{
		//no flight type or pax type
	case ENUM_UTILIZATION_REP:
	case ENUM_BILLBOARD_EXPOSUREINCIDENCE_REP:
	case ENUM_BILLBOARD_LINKEDPROCINCREMENTVISIT_REP:
	case ENUM_QUEUELENGTH_REP:
	case ENUM_AVGQUEUELENGTH_REP:
	case ENUM_BAGCOUNT_REP:
	case ENUM_CONVEYOR_WAIT_LENGTH_REP:
		{
			m_staPax.SetWindowText( "Processor" );
		}
		break;
		
		//flight type
	case ENUM_ACOPERATION_REP:
	case ENUM_BAGDISTRIBUTION_REP:
		{
			m_staPax.SetWindowText( "Flight Type" );
			LoadFltList(pReportPara);
		}
		break;
		//pax type
	default:
		{
			m_staPax.SetWindowText( "Passenger Type" );
			LoadPaxList(pReportPara);
		}
	}


	//have report processor list
	switch (m_enumReportType)
	{
	case ENUM_QUEUETIME_REP: //3
	case ENUM_SERVICETIME_REP:	//5
	case ENUM_BAGWAITTIME_REP://17
	case ENUM_CONVEYOR_WAIT_TIME_REP:
	case ENUM_UTILIZATION_REP: //7
	case ENUM_RETAIL_REP://8
	case ENUM_THROUGHPUT_REP: //8
	case ENUM_QUEUELENGTH_REP://9
	case ENUM_AVGQUEUELENGTH_REP://10
	case ENUM_CONVEYOR_WAIT_LENGTH_REP:
	case ENUM_BAGCOUNT_REP://17
	case ENUM_BILLBOARD_EXPOSUREINCIDENCE_REP:
	case ENUM_BILLBOARD_LINKEDPROCINCREMENTVISIT_REP:
		{
			LoadProTree(pReportPara);
		};
		break;
		//from to processor
	case ENUM_DISTANCE_REP:
	case ENUM_DURATION_REP:
		{
			LoadFromToProTree(pReportPara);
		}
		break;

	default:
		{

		}
		break;
	}

	//insert runs item
	if (MultiRunValid())
	{
		m_hRunsRoot = TreeInsertItem(_T("Multiple Run Report"),TVI_ROOT,true);
		m_treePaxType.SetItemData(m_hRunsRoot,(DWORD)Runs);
		{
			if(pReportPara)
				pReportPara->GetReportRuns(m_vReportRuns);


			if(m_vReportRuns.size() < 2)
			{//use the current as default
				int nCurrentSimResult = ((CTermPlanDoc *)GetDocument())->GetTerminal().getCurrentSimResult();
				m_vReportRuns.clear();
				m_vReportRuns.push_back(nCurrentSimResult);
			}

			CSimAndReportManager *pSimAndReportManager = ((CTermPlanDoc *)GetDocument())->GetTerminal().GetSimReportManager();
			int nSimCount = pSimAndReportManager->getSubSimResultCout();
			for (int nSim =0; nSim < nSimCount; ++nSim )
			{
				//CSimItem *pSimItem = pSimAndReportManager->getSimItem(nSim);
				//CString strSimName = pSimItem->getSubSimName();
				CString strSimName;
				strSimName.Format(_T("RUN %d"),nSim+1);
				HTREEITEM hSubSimItem = TreeInsertItem(strSimName,m_hRunsRoot,true);
				m_treePaxType.SetItemData(hSubSimItem,(DWORD)Runs);

				if(std::find(m_vReportRuns.begin(),m_vReportRuns.end(), nSim) != m_vReportRuns.end())
				{
					m_treePaxType.SetCheckStatus(hSubSimItem,TRUE);
				}
			}
			m_treePaxType.SetCheckStatus(m_hRunsRoot,TRUE);

			if(m_vReportRuns.size() >1)
			{
				m_btLoad.EnableWindow(FALSE);
				m_btnMulti.EnableWindow(FALSE);
			}
			else
			{
				m_btLoad.EnableWindow(TRUE);
				m_btnMulti.EnableWindow(TRUE);
			}

		//	m_treePaxType.Expand(m_hRunsRoot,TVE_EXPAND);
		}
	}

	m_treePaxType.SetRedraw(TRUE);;


//	m_treePaxType.SetScrollPos(SB_HORZ,0);
}
void CRepControlView::LoadFltList( CReportParameter *pReportParam )
{
	std::vector<CMobileElemConstraint> mobList;
	if(pReportParam != NULL && pReportParam->GetPaxType( mobList ) )
	{
		m_vFltList.clear();
		FlightConstraint flight;
		for( int i= 0; i<static_cast<int>(mobList.size()); i++ )
		{
			flight = (FlightConstraint) mobList[i];
			m_vFltList.push_back( flight );
		}
	}

	m_hFlightTypeRoot = TreeInsertItem(_T("Flight Type"),TVI_ROOT);
	m_treePaxType.SetItemData(m_hFlightTypeRoot,Flight_Type_Root);

	int nCount = m_vFltList.size();
	for( int i=0; i<nCount; i++ )
	{
		CString str;
		m_vFltList[i].screenPrint( str,0, 128 );
		HTREEITEM hItem = TreeInsertItem(str.GetBuffer(),m_hFlightTypeRoot);
		m_treePaxType.SetItemData(hItem,Flight_Type);
	}

}
void CRepControlView::LoadPaxList( CReportParameter *pReportParam )
{
	std::vector<CMobileElemConstraint> mobList;
	if(pReportParam && pReportParam->GetPaxType( mobList ) )
	{
		m_paxConst.initDefault( &(GetDocument()->GetTerminal()) );
		m_paxConst.deleteConst(0);
		for( int i=0; i<static_cast<int>(mobList.size()); i++ )
		{
			m_paxConst.addConstraint( mobList[i] );
		}
	}

	m_hPaxTypeRoot = TreeInsertItem(_T("Pax Type"),TVI_ROOT) ;
	m_treePaxType.SetItemData(m_hPaxTypeRoot,(DWORD)PAX_TYPE_Root) ;

	int count=m_paxConst.getCount();

	CString str;
	HTREEITEM hItem = NULL ;

	for(int i=0;i<count;i++)
	{
		const CMobileElemConstraint* paxConstr = m_paxConst.getConstraint( i );

		paxConstr->screenPrint( str, 0, 128 );
		hItem=TreeInsertItem(str,m_hPaxTypeRoot);
		m_treePaxType.SetItemData(hItem,(DWORD)PAX_TYPE);
	}

}

void CRepControlView::LoadProTree( CReportParameter *pReportParam )
{

	m_hProcRoot = TreeInsertItem(_T("Processor"),TVI_ROOT) ;
	if(pReportParam == NULL || !pReportParam->GetProcessorGroup(m_vProcList))
	{
		return;
	}


	m_treePaxType.SetItemData(m_hProcRoot,PROCESSOR_ROOT);

	int nProcTypeCount = m_vProcList.size();
	if( nProcTypeCount == 0 )
	{
		HTREEITEM hItem = TreeInsertItem( "All Processors", m_hProcRoot);
		m_treePaxType.SetItemData( hItem, PROCESSOR);
	}
	else
	{
		for( int i=0; i<nProcTypeCount; i++ )
		{
			ProcessorID id = m_vProcList[i];
			id.SetStrDict(GetDocument()->GetTerminal().inStrDict );
			char szBuf[256] = {0};
			id.printID(szBuf);
			HTREEITEM hItem = TreeInsertItem( szBuf ,m_hProcRoot);
			ProcessorID* proid = new ProcessorID(id) ;
			m_treePaxType.SetItemData( hItem, PROCESSOR);
		}
	}
}
void CRepControlView::LoadFromToProTree( CReportParameter *pReportParam )
{

	m_hProcToProc=TreeInsertItem("Processor To Processor",TVI_ROOT);
	m_treePaxType.SetItemData(m_hProcToProc,(DWORD)PROCESSORTOPROCESSOR);

	CReportParameter::FROM_TO_PROCS _fromToProcs;
	if(pReportParam == NULL ||!pReportParam->GetFromToProcs(_fromToProcs))
	{
		return;
	}

	Terminal& tmnl=GetDocument()->GetTerminal();
	Terminal* pTmnl=&tmnl;
	InputTerminal* pInTmnl=(InputTerminal*)pTmnl;

	if(_fromToProcs.m_vFromProcs.size()==0)
	{
		const ProcessorID* pID=pInTmnl->procList->getProcessor(START_PROCESSOR_INDEX)->getID();
		_fromToProcs.m_vFromProcs.push_back(*pID);
		GetDocument()->GetARCReportManager().GetReportPara()->SetFromToProcs(_fromToProcs);
	}

	if(_fromToProcs.m_vToProcs.size()==0)
	{
		const ProcessorID* pID=pInTmnl->procList->getProcessor(END_PROCESSOR_INDEX)->getID();
		_fromToProcs.m_vToProcs.push_back(*pID);
		GetReportParameter()->SetFromToProcs(_fromToProcs);

	}



	m_hItemFrom=TreeInsertItem("From",m_hProcToProc);
	m_treePaxType.SetItemData(m_hItemFrom,(DWORD)PROCESSOR_FROM);
	for(int k=0;k<static_cast<int>(_fromToProcs.m_vFromProcs.size());k++)
	{
		HTREEITEM hItem=TreeInsertItem(_fromToProcs.m_vFromProcs[k].GetIDString(),m_hItemFrom);
		m_treePaxType.SetItemData(hItem,PROCESSOR_FROM_PROC);
	}

	m_hItemTo=TreeInsertItem("To",m_hProcToProc);
	m_treePaxType.SetItemData(m_hItemTo,(DWORD)PROCESSOR_TO);
	for(int j=0;j<static_cast<int>(_fromToProcs.m_vToProcs.size());j++)
	{
		HTREEITEM hItem=TreeInsertItem(_fromToProcs.m_vToProcs[j].GetIDString(),m_hItemTo);
		m_treePaxType.SetItemData(hItem,PROCESSOR_TO_PROC);
	}
}
int CRepControlView::GetTreeItemIndex( HTREEITEM hItem )
{
	if(hItem == NULL)
		return -1;
	HTREEITEM hParentItem = m_treePaxType.GetParentItem(hItem);
	if(hParentItem == NULL)
		return -1;

	bool bFind = false;

	int nIndex = 0;
	
	HTREEITEM hChildItem = m_treePaxType.GetChildItem(hParentItem);
	while(hChildItem != NULL)
	{
		if(hChildItem == hItem)
		{
			bFind = true;
			break;
		}

		nIndex = nIndex + 1;
		hChildItem = m_treePaxType.GetNextSiblingItem(hChildItem);
	}

	if(bFind == false)
		return -1;
	else
		return nIndex;
}

HTREEITEM  CRepControlView::TreeInsertItem( const CString& strItemText, HTREEITEM hParentItem ,bool bHasCheckBox /*= false*/ )
{

	COOLTREE_NODE_INFO cni;
	CCoolTree::InitNodeInfo(this, cni);
	if(bHasCheckBox)
		cni.nt = NT_CHECKBOX;

	HTREEITEM hRunItem =  m_treePaxType.InsertItem(strItemText, cni, FALSE, FALSE, hParentItem);
	
	//m_treePaxType.Expand(hParentItem,TVE_EXPAND);
	return hRunItem;
}

























LRESULT CRepControlView::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	// TODO: Add your specialized code here and/or call the base class
	if(UM_CEW_STATUS_CHANGE == message)
	{	
		std::vector<int> vRunsCount;

		if(m_hRunsRoot)
		{
			HTREEITEM hRunItem = m_treePaxType.GetChildItem(m_hRunsRoot);

			int nCurrentItem = 0;
			while(hRunItem != NULL)
			{
				if(m_treePaxType.IsCheckItem(hRunItem))
				{
					vRunsCount.push_back(nCurrentItem);
				}
				nCurrentItem += 1;
				hRunItem = m_treePaxType.GetNextSiblingItem(hRunItem);
			}
		}

		if(vRunsCount.size() > 1)
		{
			m_btnMulti.EnableWindow(FALSE);
			m_btLoad.EnableWindow(FALSE);
		}
		else
		{
			m_btnMulti.EnableWindow(TRUE);
			m_btLoad.EnableWindow(TRUE);
		}
	}
	return CFormView::DefWindowProc(message, wParam, lParam);
}

void CRepControlView::OnTvnSelchangedTreePaxtype(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;

	//m_hSelectedItem = m_treePaxType.GetSelectedItem();

	//HTREEITEM hCurItem=m_treePaxType.GetSelectedItem();
	//if(hCurItem == NULL)
	//{
	//	m_PasToolBar.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONADD_TERMINALREPORT,FALSE);
	//	m_PasToolBar.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONDEL_TERMINALREPORT,FALSE);
	//	return;
	//}

	//CString strText = m_treePaxType.GetItemText(hCurItem);

	//TreeItemType enumItemType  =(TreeItemType)m_treePaxType.GetItemData(hCurItem);
	//if(enumItemType == PAX_TYPE_Root||
	//	enumItemType == PROCESSOR_ROOT||
	//	enumItemType == Flight_Type_Root||
	//	enumItemType == PROCESSOR_FROM||
	//	enumItemType == PROCESSOR_TO
	//	)
	//{
	//	m_PasToolBar.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONADD_TERMINALREPORT,TRUE);
	//}
	//else
	//{	
	//	m_PasToolBar.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONADD_TERMINALREPORT,FALSE);
	//}

	////del button

	//if(enumItemType == PAX_TYPE||
	//	enumItemType == PROCESSOR||
	//	enumItemType == Flight_Type||
	//	enumItemType == PROCESSOR_FROM_PROC||
	//	enumItemType == PROCESSOR_TO_PROC
	//	)
	//{	
	//	
	//	m_PasToolBar.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONDEL_TERMINALREPORT,TRUE);
	//}
	//else
	//{
	//	m_PasToolBar.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONDEL_TERMINALREPORT,FALSE);
	//}

}
