// CarScheduleDlg.cpp : implementation file
//

#include "stdafx.h"
#include "termplan.h"
#include "CarScheduleDlg.h"
//#include "..\engine\TrainTrafficGraph.h"
#include "TermPlanDoc.h"
#include "CarFlowNextStation.h"
#include "common\WinMsg.h"
#include ".\carscheduledlg.h"
#include "../Common/EchoSystem.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCarScheduleDlg dialog


CCarScheduleDlg* CCarScheduleDlg::m_DlgCurrentInstance = NULL;


CCarScheduleDlg::CCarScheduleDlg(TrainTrafficGraph* _pTrafficGraph,CAllCarSchedule* _pDocCarSchedule,CWnd* pParent /*=NULL*/)
	: CPropertyPage(CCarScheduleDlg::IDD,IDS_SINGLECAR)
{
	//{{AFX_DATA_INIT(CCarScheduleDlg)
	m_sCarName = _T("");
	m_iAccSpeed = 1;
	m_iDecSpeed = 1;
	m_iNorSpeed = 1;
	m_iTrainNumber = 1;
	m_iTurnAroundTime = 1;
	m_iHeadWayTime = 1;
	m_pTrafficGraph=_pTrafficGraph;
	m_allCarSchedule=*_pDocCarSchedule;
	m_iCurrentScheduleIndex=-1;
	m_bIsNew = false;
	m_pNewSingleSchedule = NULL;
	m_enumClickType = ClickTypeInvalid;
	m_pParent = pParent;
	m_iSelectStationIndex =-1;
	//}}AFX_DATA_INIT
	
	m_treeCarFlow.setInputTerminal( GetInputTerminal() );

	// record this as the current instance
	ASSERT(NULL == m_DlgCurrentInstance); // you should not create more than one instance at the same time
	m_DlgCurrentInstance = this;
}

CCarScheduleDlg::~CCarScheduleDlg()
{
	m_DlgCurrentInstance = NULL;
}

void CCarScheduleDlg::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCarScheduleDlg)
	DDX_Control(pDX, IDC_LIST_TIMETABLE, m_listTimeTalbe);
	DDX_Control(pDX, IDC_TREE_CARFLOW, m_treeCarFlow);
	DDX_Control(pDX, ID_CAR_SCHEDULE_SAVE, m_ctSave);
	DDX_Control(pDX, ID_CAR_SCHEDULE_NEW, m_ctNew);
	DDX_Control(pDX, IDC_STAT_TREE_TOOL_POSITION, m_stCarScheduleTreePos);
	DDX_Control(pDX, IDC_LIST_CARSCHEDULE, m_listCarSchedule);
	DDX_Control(pDX, IDC_SPIN_TURNAROUD_TIME, m_spinTurnAroundTime);
	DDX_Control(pDX, IDC_SPIN_HEADWAY_TIME, m_spinHeadwayTime);
	DDX_Control(pDX, IDC_SPIN_DWELL_TIME, m_spinTrainNumber);
	DDX_Control(pDX, IDC_SPIN_DECSPEED, m_spinDecSpeed);
	DDX_Control(pDX, IDC_SPIN_ACCSPEED, m_spinAccSpeed);
	DDX_Control(pDX, IDC_SPIN_NORMALSPEED, m_spinNormalSpeed);
	DDX_Control(pDX, IDC_DATETIMEPICKER2, m_endTimeControl);
	DDX_Control(pDX, IDC_DATETIMEPICKER1, m_startTimeControl);
	DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKER1, m_startTime);
	DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKER2, m_endTime);
	DDX_Text(pDX, IDC_EDIT_CARName, m_sCarName);
	DDX_Text(pDX, IDC_EDIT_ACCSPEED, m_iAccSpeed);
	DDX_Text(pDX, IDC_EDIT_DECSPEED, m_iDecSpeed);
	DDX_Text(pDX, IDC_EDIT_NORMALSPEED, m_iNorSpeed);
	DDX_Text(pDX, IDC_EDIT_DWELLING_TIME, m_iTrainNumber);
	DDX_Text(pDX, IDC_EDIT_TURNARROUND_TIME, m_iTurnAroundTime);
	DDX_Text(pDX, IDC_EDIT_HEADWAY_TIME, m_iHeadWayTime);
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_SPIN_SCHEDULESTARTDAY, m_spinStartDay);
	DDX_Control(pDX, IDC_SPIN_SCHEDULEENDDAY, m_spinEndDay);
	DDX_Control(pDX, IDC_EDIT_SCHEDULESTARTDAY, m_editStartDay);
	DDX_Control(pDX, IDC_EDIT_SCHEDULEENDDAY, m_editEndDay);
}


BEGIN_MESSAGE_MAP(CCarScheduleDlg, CPropertyPage)
	//{{AFX_MSG_MAP(CCarScheduleDlg)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_NORMALSPEED, OnDeltaposSpinNormalspeed)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_DECSPEED, OnDeltaposSpinDecspeed)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_ACCSPEED, OnDeltaposSpinAccspeed)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_DWELL_TIME, OnDeltaposSpinDwellTime)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_HEADWAY_TIME, OnDeltaposSpinHeadwayTime)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_TURNAROUD_TIME, OnDeltaposSpinTurnaroudTime)
	ON_LBN_SELCHANGE(IDC_LIST_CARSCHEDULE, OnSelchangeListCarschedule)
	ON_WM_CREATE()
	ON_BN_CLICKED(ID_CAR_SCHEDULE_NEW, OnCarScheduleNew)
	ON_BN_CLICKED(ID_CAR_SCHEDULE_SAVE, OnCarScheduleSave)
	ON_COMMAND(ID_CARSCHEDULE_NEW, OnCarScheduleFlowNew)
	ON_COMMAND(ID_CARSCHEDULE_DELETE, OnCarScheduleFlowDelete)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_CARFLOW, OnSelchangedTreeCarflow)
	ON_BN_CLICKED(ID_CAR_SCHEDULE_DELETE, OnCarScheduleDelete)
	ON_NOTIFY(LVN_COLUMNCLICK, IDC_LIST_TIMETABLE, OnColumnclickListTimetable)
	ON_NOTIFY(LVN_GETDISPINFO, IDC_LIST_TIMETABLE, OnGetdispinfoListTimetable)
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_CAR_FLOW_ADD, OnCarScheduleFlowNew)
	ON_COMMAND(ID_CAR_FLOW_DELETE, OnCarScheduleFlowDelete)
	ON_COMMAND(ID_CAR_FLOW_DWELLINGTIME, OnCarFlowDwellingtime)
	//}}AFX_MSG_MAP
	ON_EN_CHANGE(IDC_EDIT_DWELLING_TIME, OnEnChangeEditDwellingTime)
	ON_BN_CLICKED(IDC_BTN_IMPORT_TIME_TABLE, OnBnClickedBtnImportTimeTable)
	ON_BN_CLICKED(IDC_BTN_EXPORT_TIME_TABLE, OnBnClickedBtnExportTimeTable)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCarScheduleDlg message handlers

void CCarScheduleDlg::SetControl()
{
	m_spinNormalSpeed.SetRange(1,1000);
	m_spinNormalSpeed.SetBuddy( GetDlgItem(IDC_EDIT_NORMALSPEED));
	
	m_spinAccSpeed.SetRange(1,1000);
	m_spinAccSpeed.SetBuddy( GetDlgItem( IDC_EDIT_ACCSPEED ));

	m_spinDecSpeed.SetRange(1,1000);
	m_spinDecSpeed.SetBuddy( GetDlgItem( IDC_EDIT_DECSPEED ));

	m_spinTrainNumber.SetRange(1,1000);
	m_spinTrainNumber.SetBuddy( GetDlgItem ( IDC_EDIT_DWELLING_TIME ));

	m_spinHeadwayTime.SetRange(1,1000);
	m_spinHeadwayTime.SetBuddy(  GetDlgItem ( IDC_EDIT_HEADWAY_TIME ));

	m_spinTurnAroundTime.SetRange(1,1000);
	m_spinTurnAroundTime.SetBuddy(  GetDlgItem ( IDC_EDIT_TURNARROUND_TIME ));

	// time date
	m_spinStartDay.SetRange32(1,366);
	m_spinStartDay.SetPos32(1);
	m_spinStartDay.SetBuddy( GetDlgItem(IDC_EDIT_SCHEDULESTARTDAY) );

	m_spinEndDay.SetRange32(1,366);
	m_spinEndDay.SetPos32(1);
	m_spinEndDay.SetBuddy( GetDlgItem(IDC_EDIT_SCHEDULEENDDAY) );

	m_editStartDay.SetWindowText(_T("1"));
	m_editEndDay.SetWindowText(_T("1"));
}

void CCarScheduleDlg::OnDeltaposSpinNormalspeed(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;

	int	nLowerBound;
	int	nUpperBound;
	
	m_spinNormalSpeed.GetRange( nLowerBound, nUpperBound );

	if( pNMUpDown->iDelta > 0 )
	{
		m_iNorSpeed= pNMUpDown->iPos + pNMUpDown->iDelta;
		m_iNorSpeed = m_iNorSpeed > nUpperBound ? nUpperBound : m_iNorSpeed;
	}
	else if( pNMUpDown->iDelta < 0 && pNMUpDown->iPos > nLowerBound )
	{
		m_iNorSpeed = pNMUpDown->iPos + pNMUpDown->iDelta;
		m_iNorSpeed = m_iNorSpeed < nLowerBound ? nLowerBound : m_iNorSpeed;
	}	

	*pResult = 0;

	SetModified();	
}

void CCarScheduleDlg::OnDeltaposSpinDecspeed(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	
	int	nLowerBound;
	int	nUpperBound;
	
	m_spinDecSpeed.GetRange( nLowerBound, nUpperBound );

	if( pNMUpDown->iDelta > 0 )
	{
		m_iDecSpeed= pNMUpDown->iPos + pNMUpDown->iDelta;
		m_iDecSpeed = m_iDecSpeed > nUpperBound ? nUpperBound : m_iDecSpeed;
	}
	else if( pNMUpDown->iDelta < 0 && pNMUpDown->iPos > nLowerBound )
	{
		m_iDecSpeed = pNMUpDown->iPos + pNMUpDown->iDelta;
		m_iDecSpeed = m_iDecSpeed < nLowerBound ? nLowerBound : m_iDecSpeed;
	}	

	*pResult = 0;
	SetModified();	
}

void CCarScheduleDlg::OnDeltaposSpinAccspeed(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	
	int	nLowerBound;
	int	nUpperBound;
	
	m_spinAccSpeed.GetRange( nLowerBound, nUpperBound );

	if( pNMUpDown->iDelta > 0 )
	{
		m_iAccSpeed= pNMUpDown->iPos + pNMUpDown->iDelta;
		m_iAccSpeed = m_iAccSpeed > nUpperBound ? nUpperBound : m_iAccSpeed;
	}
	else if( pNMUpDown->iDelta < 0 && pNMUpDown->iPos > nLowerBound )
	{
		m_iAccSpeed = pNMUpDown->iPos + pNMUpDown->iDelta;
		m_iAccSpeed = m_iAccSpeed < nLowerBound ? nLowerBound : m_iAccSpeed;
	}	
	
	*pResult = 0;
	SetModified();	
}

void CCarScheduleDlg::OnDeltaposSpinDwellTime(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	// TODO: Add your control notification handler code here
	int	nLowerBound;
	int	nUpperBound;
	
	m_spinTrainNumber.GetRange( nLowerBound, nUpperBound );

	if( pNMUpDown->iDelta > 0 )
	{
		m_iTrainNumber= pNMUpDown->iPos + pNMUpDown->iDelta;
		m_iTrainNumber = m_iTrainNumber > nUpperBound ? nUpperBound : m_iTrainNumber;
	}
	else if( pNMUpDown->iDelta < 0 && pNMUpDown->iPos > nLowerBound )
	{
		m_iTrainNumber = pNMUpDown->iPos + pNMUpDown->iDelta;
		m_iTrainNumber = m_iTrainNumber < nLowerBound ? nLowerBound : m_iTrainNumber;
	}	
	
	if( m_iTrainNumber == 1 )
	{
		m_spinHeadwayTime.EnableWindow( FALSE );
		GetDlgItem( IDC_EDIT_HEADWAY_TIME )->EnableWindow( FALSE );
	}
	else
	{
		m_spinHeadwayTime.EnableWindow( TRUE );
		GetDlgItem( IDC_EDIT_HEADWAY_TIME )->EnableWindow( TRUE );
	}
	*pResult = 0;
	SetModified();
}

void CCarScheduleDlg::OnDeltaposSpinHeadwayTime(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	// TODO: Add your control notification handler code here
		// TODO: Add your control notification handler code here
	int	nLowerBound;
	int	nUpperBound;
	
	m_spinHeadwayTime.GetRange( nLowerBound, nUpperBound );

	if( pNMUpDown->iDelta > 0 )
	{
		m_iHeadWayTime= pNMUpDown->iPos + pNMUpDown->iDelta;
		m_iHeadWayTime = m_iHeadWayTime > nUpperBound ? nUpperBound : m_iHeadWayTime;
	}
	else if( pNMUpDown->iDelta < 0 && pNMUpDown->iPos > nLowerBound )
	{
		m_iHeadWayTime = pNMUpDown->iPos + pNMUpDown->iDelta;
		m_iHeadWayTime = m_iHeadWayTime < nLowerBound ? nLowerBound : m_iHeadWayTime;
	}	
	
	*pResult = 0;
	SetModified();	
}

void CCarScheduleDlg::OnDeltaposSpinTurnaroudTime(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	// TODO: Add your control notification handler code here
	
	int	nLowerBound;
	int	nUpperBound;
	
	m_spinTurnAroundTime.GetRange( nLowerBound, nUpperBound );

	if( pNMUpDown->iDelta > 0 )
	{
		m_iTurnAroundTime= pNMUpDown->iPos + pNMUpDown->iDelta;
		m_iTurnAroundTime = m_iTurnAroundTime > nUpperBound ? nUpperBound : m_iTurnAroundTime;
	}
	else if( pNMUpDown->iDelta < 0 && pNMUpDown->iPos > nLowerBound )
	{
		m_iTurnAroundTime = pNMUpDown->iPos + pNMUpDown->iDelta;
		m_iTurnAroundTime = m_iTurnAroundTime < nLowerBound ? nLowerBound : m_iTurnAroundTime;
	}	
	
	*pResult = 0;
	SetModified();	
}
void CCarScheduleDlg::InitToolbar()
{
	CRect rc;
	m_stCarScheduleTreePos.GetWindowRect( &rc );
	ScreenToClient(&rc);
	m_ToolBar.MoveWindow(&rc);
	m_ToolBar.ShowWindow(SW_SHOW);
	m_stCarScheduleTreePos.ShowWindow(SW_HIDE);
	m_ToolBar.GetToolBarCtrl().EnableButton( ID_CARSCHEDULE_NEW,FALSE );
	m_ToolBar.GetToolBarCtrl().EnableButton( ID_CARSCHEDULE_DELETE,FALSE );
	
}
BOOL CCarScheduleDlg::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	SetControl();
	InitToolbar();
	LoadExitCarScheduleList();
	LoadCarFlowTree();
	InitTimeTableList();
	//m_ctSave.EnableWindow( FALSE );
//	m_btnOk.EnableWindow( FALSE);
	EnableDlgRightData( false );

	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CCarScheduleDlg::LoadExitCarScheduleList()
{
	m_listCarSchedule.ResetContent();	
	ALLCARSCHEDULE::const_iterator iter=m_allCarSchedule.GetAllCarSchedule()->begin();
	ALLCARSCHEDULE::const_iterator iterEnd=m_allCarSchedule.GetAllCarSchedule()->end();
	for( int i=0; iter!=iterEnd; ++iter,++i )
	{
		m_listCarSchedule.InsertString( i,(*iter)->GetCarName() );
	}

	
}

void CCarScheduleDlg::SetDlgRightData()
{
	if ( m_iCurrentScheduleIndex == -1 )
	{
		if( m_allCarSchedule.GetAllCarSchedule()->size() > 0 )
		{
			CSingleCarSchedule* pCarSchedule = m_allCarSchedule.GetAllCarSchedule()->at( 0 );
			m_sCarName = pCarSchedule->GetCarName();
			ElapsedTime eTime = pCarSchedule->GetCarStartTime();
			m_spinStartDay.SetPos32(eTime.GetDay());
			m_startTime.SetTime( eTime.asHours()%24, ((int)eTime.asMinutes()) % 60, ((int)eTime.asSeconds()) % 60 );
			eTime = pCarSchedule->GetCarEndTime();
			m_spinEndDay.SetPos32(eTime.GetDay());
			m_endTime.SetTime( eTime.asHours()%24, ((int)eTime.asMinutes()) % 60, ((int)eTime.asSeconds()) % 60 );
			m_iAccSpeed = static_cast<int>(pCarSchedule->GetAccelerateSpeed() / SCALE_FACTOR);
			m_iNorSpeed = static_cast<int>(pCarSchedule->GetNormalSpeed() / SCALE_FACTOR);
			m_iDecSpeed = static_cast<int>(pCarSchedule->GetDecelerateSpeed() / SCALE_FACTOR);
			m_iTrainNumber = pCarSchedule->GetTrainNumber();
			m_iTurnAroundTime = pCarSchedule->GetTurnAroundTime().asSeconds();
			m_iHeadWayTime = pCarSchedule->GetHeadWayTime().asSeconds()/60;
			UpdateData( false );
		}

	}
	else
	{
			CSingleCarSchedule* pCarSchedule = m_allCarSchedule.GetAllCarSchedule()->at( m_iCurrentScheduleIndex );
			m_sCarName = pCarSchedule->GetCarName();
			ElapsedTime eTime = pCarSchedule->GetCarStartTime();
			m_spinStartDay.SetPos32(eTime.GetDay());
			m_startTime.SetTime( eTime.asHours()%24, ((int)eTime.asMinutes()) % 60, ((int)eTime.asSeconds()) % 60 );
			eTime = pCarSchedule->GetCarEndTime();
			m_spinEndDay.SetPos32(eTime.GetDay());
			m_endTime.SetTime( eTime.asHours()%24, ((int)eTime.asMinutes()) % 60, ((int)eTime.asSeconds()) % 60 );
			m_iAccSpeed = static_cast<int>(pCarSchedule->GetAccelerateSpeed() / SCALE_FACTOR);
			m_iNorSpeed = static_cast<int>(pCarSchedule->GetNormalSpeed() / SCALE_FACTOR);
			m_iDecSpeed = static_cast<int>(pCarSchedule->GetDecelerateSpeed() / SCALE_FACTOR);
			m_iTrainNumber = pCarSchedule->GetTrainNumber();
			m_iTurnAroundTime = pCarSchedule->GetTurnAroundTime().asSeconds();
			m_iHeadWayTime = pCarSchedule->GetHeadWayTime().asSeconds()/60;
			UpdateData( false );
	}
	
}

void CCarScheduleDlg::ClearDlgRightData()
{
	m_sCarName = _T("");
	m_iAccSpeed = 1;
	m_iDecSpeed = 1;
	m_iNorSpeed = 1;
	m_iTrainNumber = 1;
	m_iTurnAroundTime = 1;
	m_iHeadWayTime = 1;
	m_spinStartDay.SetPos32(1);
	m_startTime.SetTime(0,0,0);
	m_spinEndDay.SetPos32(1);
	m_endTime.SetTime(0,0,0);

	UpdateData( false );
}
void CCarScheduleDlg::EnableDlgRightData( bool bEnabled)
{
	
	GetDlgItem(IDC_EDIT_CARName)->EnableWindow( bEnabled );
	GetDlgItem(IDC_EDIT_NORMALSPEED)->EnableWindow( bEnabled );
	GetDlgItem(IDC_EDIT_ACCSPEED)->EnableWindow( bEnabled );
	GetDlgItem(IDC_EDIT_DECSPEED)->EnableWindow( bEnabled );
	GetDlgItem(IDC_EDIT_DWELLING_TIME)->EnableWindow( bEnabled );
	GetDlgItem(IDC_EDIT_TURNARROUND_TIME)->EnableWindow( bEnabled );
	GetDlgItem(IDC_EDIT_HEADWAY_TIME)->EnableWindow( bEnabled );
	


	m_spinTurnAroundTime.EnableWindow( bEnabled );
	m_spinHeadwayTime.EnableWindow( bEnabled );
	m_spinTrainNumber.EnableWindow( bEnabled );
	m_spinDecSpeed.EnableWindow( bEnabled );
	m_spinAccSpeed.EnableWindow( bEnabled );
	m_spinNormalSpeed.EnableWindow( bEnabled );
	m_endTimeControl.EnableWindow( bEnabled );
	m_startTimeControl.EnableWindow( bEnabled );
	m_treeCarFlow.EnableWindow( bEnabled );
	m_listTimeTalbe.EnableWindow( bEnabled );

	m_spinStartDay.EnableWindow( bEnabled );
	m_spinEndDay.EnableWindow( bEnabled );
	m_editStartDay.EnableWindow( bEnabled );
	m_editEndDay.EnableWindow( bEnabled );

	// import and export button
	//GetDlgItem(IDC_BTN_IMPORT_TIME_TABLE)->EnableWindow( bEnabled );
	//GetDlgItem(IDC_BTN_EXPORT_TIME_TABLE)->EnableWindow( bEnabled );
	
}
void CCarScheduleDlg::OnSelchangeListCarschedule() 
{
	//UpdateData(FALSE);
	m_ToolBar.GetToolBarCtrl().EnableButton( ID_CARSCHEDULE_NEW,FALSE );
	m_ToolBar.GetToolBarCtrl().EnableButton( ID_CARSCHEDULE_DELETE,FALSE );
	GetDlgItem(ID_CAR_SCHEDULE_DELETE)->EnableWindow();
	m_enumClickType = ClickTypeSelectSchedule;
	m_iCurrentScheduleIndex = m_listCarSchedule.GetCurSel();
	EnableDlgRightData( true );
	SetDlgRightData();
	
	if( m_iTrainNumber == 1 )
	{
		m_spinHeadwayTime.EnableWindow( FALSE );
		GetDlgItem( IDC_EDIT_HEADWAY_TIME )->EnableWindow( FALSE );
	}
	else
	{
		m_spinHeadwayTime.EnableWindow( TRUE );
		GetDlgItem( IDC_EDIT_HEADWAY_TIME )->EnableWindow( TRUE );
	}
	
	LoadCarFlowTree();
	SetTimeTableList();
	m_ctNew.EnableWindow( FALSE );
	m_ctSave.EnableWindow( TRUE );
//	m_btnOk.EnableWindow();
	//UpdateData(TRUE);
	//SetModified();
	
}

int CCarScheduleDlg::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CPropertyPage::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	if (!m_ToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_ToolBar.LoadToolBar(IDR_TOOLBAR_CARSCHEDULE))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}		
	return 0;
}

void CCarScheduleDlg::OnCarScheduleNew() 
{
	m_enumClickType = ClickTypeNewSchedule;
	m_bIsNew = true;
	m_pNewSingleSchedule = new CSingleCarSchedule();
	m_allCarSchedule.AddNewCarSchedule( m_pNewSingleSchedule );
	EnableDlgRightData( true );
	
	ClearDlgRightData();
	LoadCarFlowTree();
//	LoadList();
	m_ctNew.EnableWindow( FALSE );
	m_ctSave.EnableWindow();
//	m_btnOk.EnableWindow();
	GetDlgItem(ID_CAR_SCHEDULE_DELETE)->EnableWindow( FALSE );
	GetDlgItem(IDC_EDIT_HEADWAY_TIME)->EnableWindow( FALSE );
	SetTimeTableList();
	m_listCarSchedule.EnableWindow( FALSE );
	SetModified();	
}

void CCarScheduleDlg::OnCarScheduleSave() 
{
	UpdateData();
	//AfxGetApp()->DoWaitCursor(1);
	m_ToolBar.GetToolBarCtrl().EnableButton( ID_CARSCHEDULE_NEW,FALSE );
	m_ToolBar.GetToolBarCtrl().EnableButton( ID_CARSCHEDULE_DELETE,FALSE );

	CString path = ((CTermPlanDoc*)((CView*)m_pParent)->GetDocument())->m_ProjInfo.path;
	if( m_enumClickType == ClickTypeNewSchedule )
	{
		m_pNewSingleSchedule->SetAccelerateSpeed( (DistanceUnit)m_iAccSpeed * SCALE_FACTOR );
		ElapsedTime eTime;
		eTime.set( m_startTime.GetHour(), m_startTime.GetMinute(), m_startTime.GetSecond() );
		eTime.SetDay(m_spinStartDay.GetPos32());
		m_pNewSingleSchedule->SetCarStartTime( eTime );
		eTime.set( m_endTime.GetHour(), m_endTime.GetMinute(), m_endTime.GetSecond() );
		eTime.SetDay(m_spinEndDay.GetPos32());
		m_pNewSingleSchedule->SetCarEndtTime( eTime );
		m_pNewSingleSchedule->SetCarName( m_sCarName );
		m_pNewSingleSchedule->SetDecelerateSpeed( (DistanceUnit)m_iDecSpeed *  SCALE_FACTOR );
		m_pNewSingleSchedule->SetTrainNumber( m_iTrainNumber );
		eTime.set( m_iHeadWayTime*60);
		m_pNewSingleSchedule->SetHeadWayTime( eTime );
		m_pNewSingleSchedule->SetNormalSpeed( (DistanceUnit)m_iNorSpeed  * SCALE_FACTOR );
		eTime.set( m_iTurnAroundTime );
		m_pNewSingleSchedule->SetTurnAroundTime( eTime );
		m_pNewSingleSchedule->SetSystemProcessorList( GetInputTerminal()->procList );

		if( !m_pNewSingleSchedule->IsAValidCarFlow( m_pTrafficGraph ) )
		{
			AfxMessageBox( " The car flow definition is invalid!");
			return;

		}
		
		m_pNewSingleSchedule->GenerateRealSchedule( m_pTrafficGraph );

		if( !m_pNewSingleSchedule->IsInputDataValid())
			return;
		m_ctNew.EnableWindow();
		m_listCarSchedule.EnableWindow( true );
		LoadExitCarScheduleList();

	}
	else
	{
		if( m_iCurrentScheduleIndex >=0)
		{
	
			CSingleCarSchedule* pSingleCar = m_allCarSchedule.GetAllCarSchedule()->at( m_iCurrentScheduleIndex );
			pSingleCar->SetAccelerateSpeed( (DistanceUnit)m_iAccSpeed * SCALE_FACTOR );
			ElapsedTime eTime;
			eTime.set( m_startTime.GetHour(), m_startTime.GetMinute(), m_startTime.GetSecond() );
			eTime.SetDay(m_spinStartDay.GetPos32());
			pSingleCar->SetCarStartTime( eTime );
			eTime.set( m_endTime.GetHour(), m_endTime.GetMinute(), m_endTime.GetSecond() );
			eTime.SetDay(m_spinEndDay.GetPos32());
			pSingleCar->SetCarEndtTime( eTime );
			pSingleCar->SetCarName( m_sCarName );
			pSingleCar->SetDecelerateSpeed( (DistanceUnit)m_iDecSpeed *  SCALE_FACTOR );
			pSingleCar->SetTrainNumber( m_iTrainNumber );
			eTime.set( m_iHeadWayTime * 60);
			pSingleCar->SetHeadWayTime( eTime );
			pSingleCar->SetNormalSpeed( (DistanceUnit)m_iNorSpeed  * SCALE_FACTOR );
			eTime.set( m_iTurnAroundTime );
			pSingleCar->SetTurnAroundTime( eTime );
			pSingleCar->SetSystemProcessorList( GetInputTerminal()->procList );

			if( !pSingleCar->IsAValidCarFlow( m_pTrafficGraph ) )
			{
				AfxMessageBox( " The car flow definition is invalid!");
				return;

			}
			
			pSingleCar->GenerateRealSchedule( m_pTrafficGraph );
		
			
			if( !pSingleCar->IsInputDataValid())
				return;
			//m_bIsNew=false;
			m_ctNew.EnableWindow();
			//m_ctSave.EnableWindow( FALSE );
			//DisableDlgRightData( false );
			m_listCarSchedule.EnableWindow( true );
		}
		
	}

	SetTimeTableList();
//	m_btnOk.EnableWindow();
	/*

	*(GetInputTerminal()->pAllCarSchedule) = m_allCarSchedule;		
	GetInputTerminal()->pAllCarSchedule->saveDataSet( path, false );

	AfxGetApp()->DoWaitCursor(-1);
	*/
	
}

void CCarScheduleDlg::OnCancel() 
{
//	if( m_bIsNew )
//	{
//		delete m_pNewSingleSchedule;
		
//	}
	CPropertyPage::OnCancel();
}

InputTerminal* CCarScheduleDlg::GetInputTerminal()
{
	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParent)->GetDocument();
    return (InputTerminal*)&pDoc->GetTerminal();
}

void CCarScheduleDlg::OnCarScheduleFlowNew() 
{
	m_hCurrentSel = m_treeCarFlow.GetSelectedItem();
	if (NULL == m_hCurrentSel)
	{
		return;
	}
	
	if ( m_hCurrentSel == m_hStart )
	{
		CCarFlowNextStation dlg( m_pTrafficGraph, NULL );
		if ( dlg.DoModal() == IDOK )
		{
		  if( m_enumClickType == ClickTypeNewSchedule ) //new 
			{
				m_pNewSingleSchedule->AddNextStation( NULL,dlg.m_pSelectedStation );
				m_pNewSingleSchedule->AddStayOnStationTime( NULL , 60 );
			}
			else
			{
				m_allCarSchedule.GetAllCarSchedule()->at( m_iCurrentScheduleIndex )->AddNextStation( NULL, dlg.m_pSelectedStation );
				m_allCarSchedule.GetAllCarSchedule()->at( m_iCurrentScheduleIndex )->AddStayOnStationTime( NULL, 60 );
			}
		}

	}
	else
	{
		IntegratedStation* pStartStation = (IntegratedStation*)(m_treeCarFlow.GetItemData( m_hCurrentSel ));
		IntegratedStation* pEndStation = NULL;

		CCarFlowNextStation dlg( m_pTrafficGraph, pStartStation );
		if ( dlg.DoModal() == IDOK )
		{
			RailwayInfo* m_pSelectedRailWay =dlg.m_pSelectedRailWay;
			int iStartStationIndex = m_pTrafficGraph->GetStationIndexInTraffic( pStartStation );

			int iEndStationIndex = -1;
			iStartStationIndex == m_pSelectedRailWay->GetFirstStationIndex()
					? iEndStationIndex = m_pSelectedRailWay->GetSecondStationIndex()
					: iEndStationIndex = m_pSelectedRailWay->GetFirstStationIndex() ;
			ASSERT( iEndStationIndex!= -1 );

			pEndStation = m_pTrafficGraph->GetTrafficGraph().at( iEndStationIndex )->GetStation();

			//bool bPushBack= false;
			//if( m_hCurrentSel == m_treeCarFlow.GetParentItem( m_hEnd ) )
			//	bPushBack = true;

			if( m_enumClickType == ClickTypeNewSchedule ) //new 
			{
				m_pNewSingleSchedule->AddNextStation( pStartStation,pEndStation,true );
				m_pNewSingleSchedule->AddLinkedRailWay( pStartStation , m_pSelectedRailWay,true );
				m_pNewSingleSchedule->AddStayOnStationTime( pStartStation , 60,true );
			}
			else
			{
				m_allCarSchedule.GetAllCarSchedule()->at( m_iCurrentScheduleIndex )->AddNextStation( pStartStation,pEndStation,true );
				m_allCarSchedule.GetAllCarSchedule()->at( m_iCurrentScheduleIndex )->AddLinkedRailWay( pStartStation , m_pSelectedRailWay,true );
				m_allCarSchedule.GetAllCarSchedule()->at( m_iCurrentScheduleIndex )->AddStayOnStationTime( pStartStation , 60 ,true);
			}
		}

	}

	LoadCarFlowTree();
	SetModified();	
	
}

void CCarScheduleDlg::OnCarScheduleFlowDelete() 
{
	int iDeletePos=-1;
	for( int i=0;i<static_cast<int>(m_treeItemVector.size()); ++i )
	{
		if( m_treeItemVector[i] == m_hCurrentSel )
		{
			iDeletePos = m_treeItemVector.size() - i ;
		}
	}


	if( m_enumClickType == ClickTypeNewSchedule ) //new 
	{
		m_pNewSingleSchedule->DeleteLinkedRailWay( iDeletePos );
		if( m_pNewSingleSchedule->GetCarFlow().size() <= 0 )
		{
			m_ToolBar.GetToolBarCtrl().EnableButton( ID_CARSCHEDULE_NEW,TRUE );
			m_ToolBar.GetToolBarCtrl().EnableButton( ID_CARSCHEDULE_DELETE,FALSE );
		}
	}
	else
	{
		m_allCarSchedule.GetAllCarSchedule()->at( m_iCurrentScheduleIndex )->DeleteLinkedRailWay( iDeletePos);
		if( m_allCarSchedule.GetAllCarSchedule()->at( m_iCurrentScheduleIndex )->GetCarFlow().size() <= 0 )
		{
			m_ToolBar.GetToolBarCtrl().EnableButton( ID_CARSCHEDULE_NEW,TRUE );
			m_ToolBar.GetToolBarCtrl().EnableButton( ID_CARSCHEDULE_DELETE,FALSE );
		}
	}
	
	LoadCarFlowTree();
	SetModified();	
}

void CCarScheduleDlg::LoadCarFlowTree()
{
	m_treeCarFlow.DeleteAllItems();
	m_treeItemVector.clear();
	//m_treeRailwayVector.clear();
	m_hStart = m_treeCarFlow.InsertItem( " Start " );
	m_treeItemVector.push_back( m_hStart );
	m_treeCarFlow.Expand( m_hStart, TVE_EXPAND );
	
	CARFLOW tempCarFlow ;
	RAILWAY tempRailWay;
	if( m_bIsNew )
	{
		tempCarFlow = m_pNewSingleSchedule->GetCarFlow();	
		tempRailWay = m_pNewSingleSchedule->GetRailWay();
	}
	else
	{
		if( m_iCurrentScheduleIndex == -1 )
		{
			//if ( m_allCarSchedule.GetAllCarSchedule()->size() >0  )
			//	m_iCurrentScheduleIndex = 0;
		}

		if( m_iCurrentScheduleIndex != -1)
		{
			tempCarFlow = m_allCarSchedule.GetAllCarSchedule()->at( m_iCurrentScheduleIndex )->GetCarFlow();
			tempRailWay = m_allCarSchedule.GetAllCarSchedule()->at( m_iCurrentScheduleIndex )->GetRailWay();
		}
		
	}
	if( m_iCurrentScheduleIndex != -1 || m_bIsNew)
	{
		CARFLOW::const_iterator iter = tempCarFlow.begin();
		CARFLOW::const_iterator iterNext =iter;
		CARFLOW::const_iterator iterEnd = tempCarFlow.end();

		RAILWAY::const_iterator iterRail = tempRailWay.begin();
		RAILWAY::const_iterator iterRailEnd = tempRailWay.end();
		
		for(int i=0 ; iter!=iterEnd ; ++iter,++i)
		{
			
			char processorName[256];
			(*iter)->getID()->printID( processorName );
			CString sDisplayStr = processorName;
			++iterNext;
			if (iterNext != iterEnd)
			{
				RAILWAY vRailWay;
				m_pTrafficGraph->GetStationLinkedRailWay( *iter, *iterNext ,0 , vRailWay );
				if( vRailWay.size() > 1 )
				{
					int iStartStation = m_pTrafficGraph->GetStationIndexInTraffic( (*iter) );
					if( (*iterRail) && iterRail!=iterRailEnd )
					{
						CString sRailWayName = GetRailWayName( (*iterRail) ,iStartStation );
						sDisplayStr += " ( ";
						sDisplayStr += sRailWayName;
						sDisplayStr += " ) ";
					}
				}
			}
			HTREEITEM tempItem = m_treeCarFlow.InsertItem( sDisplayStr, m_treeItemVector[i] );
			m_treeCarFlow.SetItemData( tempItem,(long)(*iter) );
			m_treeItemVector.push_back( tempItem );
			if (iterRail != iterRailEnd)
			{
				++iterRail;
			}	
			m_treeCarFlow.Expand( m_treeItemVector[i], TVE_EXPAND );
			if( i == m_iSelectStationIndex )
			{
				m_treeCarFlow.SelectItem( tempItem );
			}
		}
	}

	m_hLastStationItem = m_treeItemVector[ m_treeItemVector.size() -1 ];
	m_hEnd = m_treeCarFlow.InsertItem( " End ",  m_hLastStationItem);
	m_treeCarFlow.Expand( m_treeItemVector[ m_treeItemVector.size() -1 ], TVE_EXPAND );


}
void CCarScheduleDlg::ClearTree()
{
	m_treeCarFlow.DeleteAllItems();
	m_treeCarFlow.InsertItem("End",m_treeCarFlow.InsertItem("Start"));

}

void CCarScheduleDlg::OnSelchangedTreeCarflow(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	
	m_hCurrentSel = m_treeCarFlow.GetSelectedItem();
	if( m_hCurrentSel == m_hStart )
	{
		if( m_treeItemVector.size() > 2 )
		{
			m_ToolBar.GetToolBarCtrl().EnableButton( ID_CARSCHEDULE_NEW,FALSE );
			m_ToolBar.GetToolBarCtrl().EnableButton( ID_CARSCHEDULE_DELETE,FALSE );
		}
		else
		{
			m_ToolBar.GetToolBarCtrl().EnableButton( ID_CARSCHEDULE_NEW,TRUE );
			m_ToolBar.GetToolBarCtrl().EnableButton( ID_CARSCHEDULE_DELETE,FALSE );
		}
	}
	else if ( m_hCurrentSel == m_hEnd )
	{
		m_ToolBar.GetToolBarCtrl().EnableButton( ID_CARSCHEDULE_NEW,FALSE );
		m_ToolBar.GetToolBarCtrl().EnableButton( ID_CARSCHEDULE_DELETE,FALSE );
	}
	else
	{
		m_ToolBar.GetToolBarCtrl().EnableButton( ID_CARSCHEDULE_DELETE,TRUE );
		if( m_hLastStationItem == m_hCurrentSel )
		{
			m_ToolBar.GetToolBarCtrl().EnableButton( ID_CARSCHEDULE_NEW,TRUE );
		}
		else
		{
			m_ToolBar.GetToolBarCtrl().EnableButton( ID_CARSCHEDULE_NEW,FALSE );
		}
	}
	
	*pResult = 0;
}

CString CCarScheduleDlg::GetRailWayName( RailwayInfo* _pRailWay ,int _iFirstStationIndex )

{
	char sFirstStationName [256 ];
	char sSecondStationName [256];
	CString sReturnName;
	int iLinkedType;
	m_pTrafficGraph->GetTrafficGraph().at( _iFirstStationIndex )->GetStation()->getID()->printID( sFirstStationName );
	if( _iFirstStationIndex == _pRailWay->GetFirstStationIndex() )
	{
		int iSecondStationIndex = _pRailWay->GetSecondStationIndex();
		m_pTrafficGraph->GetTrafficGraph().at( iSecondStationIndex )->GetStation()->getID()->printID( sSecondStationName );
		 iLinkedType = _pRailWay->GetTypeFromFirstViewPoint();
		
	}
	else
	{
		int iSecondStationIndex = _pRailWay->GetFirstStationIndex();
		m_pTrafficGraph->GetTrafficGraph().at( iSecondStationIndex )->GetStation()->getID()->printID( sSecondStationName );
		iLinkedType = _pRailWay->GetTypeFromSecondViewPoint();
		
	}

	switch ( iLinkedType )
		{
			case PORT1_PORT1:
				{
					sReturnName = " Port1 ";
					/*
					sReturnName = sFirstStationName;
					sReturnName += " Port1 --- ";
					sReturnName += sSecondStationName;
					sReturnName += " Port1 ";
					*/
					break;
				}
			case PORT1_PORT2 :
				{
					sReturnName = " Port1 ";
					/*
					sReturnName = sFirstStationName;
					sReturnName += " Port1 --- ";
					sReturnName += sSecondStationName;
					sReturnName += " Port2 ";
					*/
					break;
				}
			case PORT2_PORT1:
				{
					sReturnName = " Port2 ";
					/*
					sReturnName = sFirstStationName;
					sReturnName += " Port2 --- ";
					sReturnName += sSecondStationName;
					sReturnName += " Port1 ";
					*/
					break;
				}
			default:
				{
					sReturnName = " Port2 ";
					/*
					sReturnName = sFirstStationName;
					sReturnName += " Port2 --- ";
					sReturnName += sSecondStationName;
					sReturnName += " Port2 ";
					*/
				}
		}

	return sReturnName;
}

void CCarScheduleDlg::OnOK() 
{
	//if (ECHOLOG->IsEnable() && ECHOLOG->IsLogEnable(RailWayLog))
	{
		CString strLog = _T("");
		strLog = _T("AllCarSchedule modify by its Define Dialog;");
		ECHOLOG->Log(RailWayLog,strLog);
	}
	CString path = ((CTermPlanDoc*)((CView*)m_pParent)->GetDocument())->m_ProjInfo.path;
	*(GetInputTerminal()->m_pAllCarSchedule) = m_allCarSchedule;
	GetInputTerminal()->m_pAllCarSchedule->saveDataSet( path , false );
//	m_btnOk.EnableWindow( FALSE );
	EnableDlgRightData( false );
	m_ctNew.EnableWindow();
	CPropertyPage::OnOK();
}

void CCarScheduleDlg::OnCarScheduleDelete() 
{

	m_allCarSchedule.DeleteCarSchedule( m_iCurrentScheduleIndex );
	m_iCurrentScheduleIndex = -1;
	m_listTimeTalbe.DeleteAllItems();
	GetDlgItem(ID_CAR_SCHEDULE_NEW)->EnableWindow();
	GetDlgItem(ID_CAR_SCHEDULE_DELETE)->EnableWindow( FALSE );
	GetDlgItem(ID_CAR_SCHEDULE_SAVE)->EnableWindow( FALSE );
	EnableDlgRightData( false );
	SetModified();
	
	LoadExitCarScheduleList();
	
}

void CCarScheduleDlg::SetTimeTableList()
{

	m_listTimeTalbe.DeleteAllItems();
	

	if( m_bIsNew )
	{
		m_pNewSingleSchedule->GetAllTimeTable( m_vTimeTableVctor );
	}
	else if ( m_iCurrentScheduleIndex != -1 )
	{
		
		m_allCarSchedule.GetAllCarSchedule()->at( m_iCurrentScheduleIndex )->GetAllTimeTable( m_vTimeTableVctor );
	}

	TimeTableVector::const_iterator iter = m_vTimeTableVctor.begin();
	TimeTableVector::const_iterator iterEnd = m_vTimeTableVctor.end();
	for(int i=0; iter!=iterEnd ; ++iter,++i) 
	{
		LV_ITEM lvi;
		lvi.mask = LVIF_TEXT |  LVIF_PARAM;
		lvi.iItem = i;
		lvi.iSubItem = 0;
		lvi.iImage = 0;
		lvi.pszText = LPSTR_TEXTCALLBACK;
// 		lvi.lParam = (LPARAM) &(*iter);  // caution! this may cause failure when vector resized.
		lvi.lParam = (LPARAM) i; // so just record index, and then you can retrieve data from vector.
		m_listTimeTalbe.InsertItem( &lvi );
	}


}

void CCarScheduleDlg::InitTimeTableList()
{
	CRect listRcct;
	CString colName;
	m_listTimeTalbe.GetClientRect( &listRcct );

	int width = listRcct.Width()/7;
	colName.LoadString(IDS_STRING_TRAIN_INDEX);
	m_listTimeTalbe.InsertColumn(0, colName, LVCFMT_LEFT, width);
	colName.LoadString(IDS_STATION_NAME);
	m_listTimeTalbe.InsertColumn(1, colName, LVCFMT_LEFT, width);
	colName.LoadString(IDS_ARRIVE_DAY);
	m_listTimeTalbe.InsertColumn(2, colName, LVCFMT_LEFT, width);
	colName.LoadString(IDS_ARRIVE_TIME);
	m_listTimeTalbe.InsertColumn(3, colName, LVCFMT_LEFT, width);
	colName.LoadString(IDS_DEPART_DAY);
	m_listTimeTalbe.InsertColumn(4, colName, LVCFMT_LEFT, width);
	colName.LoadString(IDS_DEPARTTIME);
	m_listTimeTalbe.InsertColumn(5, colName, LVCFMT_LEFT, width);
	colName.LoadString(IDS_DESTSTATION);
	m_listTimeTalbe.InsertColumn(6, colName, LVCFMT_LEFT, width);
	m_listTimeTalbe.ModifyStyle(LVS_TYPEMASK, LVS_REPORT);
	
	
	//DWORD dwStyle = m_listTimeTalbe.GetExtendedStyle();
	//dwStyle |= LVS_EX_HEADERDRAGDROP|LVS_EX_GRIDLINES;
	//m_listTimeTalbe.SetExtendedStyle( dwStyle );


	ListView_SetExtendedListViewStyle(m_listTimeTalbe.GetSafeHwnd(),LVS_EX_HEADERDRAGDROP|LVS_EX_FULLROWSELECT |LVS_EX_GRIDLINES );
	SetTimeTableList();

}

void CCarScheduleDlg::OnColumnclickListTimetable(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	m_listTimeTalbe.SortItems(CompareFunc, pNMListView->iSubItem);
	*pResult = 0;
}
int CALLBACK CCarScheduleDlg::CompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	//static int sortorder = 0;
	ASSERT(m_DlgCurrentInstance);
	TIMETABLE* pItem1 = &m_DlgCurrentInstance->m_vTimeTableVctor[(size_t)(lParam1)];
	TIMETABLE* pItem2 = &m_DlgCurrentInstance->m_vTimeTableVctor[(size_t)(lParam2)];
	ASSERT(pItem1 && pItem2);

	int nResult;
	switch(lParamSort)
	{
		case 0://train index
		{
			CString sIndex1;
			CString sIndex2;
			sIndex1.Format( "%d",pItem1->m_nTrainIndex);
			sIndex2.Format( "%d",pItem2->m_nTrainIndex);
			nResult = sIndex1.CompareNoCase( sIndex2 );	
			break;;
		}
		case 1: // sort by station name
			nResult = pItem1->m_sCurStationName.CompareNoCase(pItem2->m_sCurStationName);
			break;
		case 2: // sort by arrive day
			nResult = pItem1->m_arrDay - pItem2->m_arrDay;
			break;
		case 3: // sort by arrive time
			nResult = pItem1->m_arrTime.CompareNoCase(pItem2->m_arrTime);
			break;
		case 4: // sort by depart day
			nResult = pItem1->m_arrDay - pItem2->m_arrDay;
			break;
		case 5: // sort by depart time
			nResult = pItem1->m_depTime.CompareNoCase(pItem2->m_depTime);
			break;
		case 6: // sort by dest station
			nResult = pItem1->m_sDestStationName.CompareNoCase(pItem2->m_sDestStationName);
			break;
		default:
			ASSERT(FALSE);
			break;
		
	}

	return nResult;
}

void CCarScheduleDlg::OnGetdispinfoListTimetable(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
	
	if(pDispInfo->item.mask & LVIF_TEXT) 
	{
		// TIMETABLE* pInfo = (TIMETABLE*) pDispInfo->item.lParam;
		// here we access data by index!
		TIMETABLE* pInfo = &m_vTimeTableVctor[(size_t)(pDispInfo->item.lParam)];
		switch(pDispInfo->item.iSubItem) 
		{
			case 0://train index
				{
					CString sIndex;
					sIndex.Format( _T("%d"),pInfo->m_nTrainIndex);
					lstrcpy(pDispInfo->item.pszText, sIndex);
				}
				break;
			case 1: //station name
				lstrcpy(pDispInfo->item.pszText, pInfo->m_sCurStationName);
				break;
			case 2: // Arrive Day
				{
					CString sIndex;
					sIndex.Format( _T("%d"),pInfo->m_arrDay);
					lstrcpy(pDispInfo->item.pszText, sIndex);
				}
				break;
			case 3: //arrive time
				lstrcpy(pDispInfo->item.pszText, pInfo->m_arrTime);
				break;
			case 4: // Departure Day
				{
					CString sIndex;
					sIndex.Format( _T("%d"),pInfo->m_depDay);
					lstrcpy(pDispInfo->item.pszText, sIndex);
				}
				break;
			case 5: //depart time
				lstrcpy(pDispInfo->item.pszText, pInfo->m_depTime);
				break;
			case 6://dest station
				lstrcpy(pDispInfo->item.pszText, pInfo->m_sDestStationName);
				break;
		
		}
	}
	*pResult = 0;
}

BOOL CCarScheduleDlg::OnApply() 
{
	UpdateData();
	
	if( m_enumClickType == ClickTypeNewSchedule )
	{
		m_pNewSingleSchedule->SetAccelerateSpeed( (DistanceUnit)m_iAccSpeed * SCALE_FACTOR );
		ElapsedTime eTime;
		eTime.set( m_startTime.GetHour(), m_startTime.GetMinute(), m_startTime.GetSecond() );
		eTime.SetDay(m_spinStartDay.GetPos32());
		m_pNewSingleSchedule->SetCarStartTime( eTime );
		eTime.set( m_endTime.GetHour(), m_endTime.GetMinute(), m_endTime.GetSecond() );
		eTime.SetDay(m_spinEndDay.GetPos32());
		m_pNewSingleSchedule->SetCarEndtTime( eTime );
		m_pNewSingleSchedule->SetCarName( m_sCarName );
		m_pNewSingleSchedule->SetDecelerateSpeed( (DistanceUnit)m_iDecSpeed *  SCALE_FACTOR );
		m_pNewSingleSchedule->SetTrainNumber( m_iTrainNumber );
		eTime.set( m_iHeadWayTime*60 );
		m_pNewSingleSchedule->SetHeadWayTime( eTime );
		m_pNewSingleSchedule->SetNormalSpeed( (DistanceUnit)m_iNorSpeed  * SCALE_FACTOR );
		eTime.set( m_iTurnAroundTime );
		m_pNewSingleSchedule->SetTurnAroundTime( eTime );
		m_pNewSingleSchedule->SetSystemProcessorList( GetInputTerminal()->procList );

		if( !m_pNewSingleSchedule->IsAValidCarFlow( m_pTrafficGraph ) )
		{
			AfxMessageBox( " The car flow definition is invalid!");
			return false;

		}
		m_ctNew.EnableWindow();
		//m_ctSave.EnableWindow( FALSE );
		m_listCarSchedule.EnableWindow( true );
		LoadExitCarScheduleList();

	}
	else
	{
		if( m_iCurrentScheduleIndex>=0 )
		{
			CSingleCarSchedule* pSingleCar = m_allCarSchedule.GetAllCarSchedule()->at( m_iCurrentScheduleIndex );
			pSingleCar->SetAccelerateSpeed( (DistanceUnit)m_iAccSpeed * SCALE_FACTOR );
			ElapsedTime eTime;
			eTime.set( m_startTime.GetHour(), m_startTime.GetMinute(), m_startTime.GetSecond() );
			eTime.SetDay(m_spinStartDay.GetPos32());
			pSingleCar->SetCarStartTime( eTime );
			eTime.set( m_endTime.GetHour(), m_endTime.GetMinute(), m_endTime.GetSecond() );
			eTime.SetDay(m_spinEndDay.GetPos32());
			pSingleCar->SetCarEndtTime( eTime );
			pSingleCar->SetCarName( m_sCarName );
			pSingleCar->SetDecelerateSpeed( (DistanceUnit)m_iDecSpeed *  SCALE_FACTOR );
			pSingleCar->SetTrainNumber( m_iTrainNumber );
			eTime.set( m_iHeadWayTime*60 );
			pSingleCar->SetHeadWayTime( eTime );
			pSingleCar->SetNormalSpeed( (DistanceUnit)m_iNorSpeed  * SCALE_FACTOR );
			eTime.set( m_iTurnAroundTime );
			pSingleCar->SetTurnAroundTime( eTime );
			pSingleCar->SetSystemProcessorList( GetInputTerminal()->procList );

			if( !pSingleCar->IsAValidCarFlow( m_pTrafficGraph ) )
			{
				AfxMessageBox( " The car flow definition is invalid!");
				return false;

			}
			
			m_ctNew.EnableWindow();
			//m_ctSave.EnableWindow( FALSE );
			//m_listCarSchedule.EnableWindow( true );
		}
		
	}

	//if (ECHOLOG->IsEnable() && ECHOLOG->IsLogEnable(RailWayLog))
	{
		CString strLog = _T("");
		strLog = _T("AllCarSchedule modify by its Define Dialog;");
		ECHOLOG->Log(RailWayLog,strLog);
	}

	SetTimeTableList();
	LoadExitCarScheduleList();
	CString path = ((CTermPlanDoc*)((CView*)m_pParent)->GetDocument())->m_ProjInfo.path;
	*(GetInputTerminal()->m_pAllCarSchedule) = m_allCarSchedule;
	GetInputTerminal()->m_pAllCarSchedule->saveDataSet( path , false );
//	m_btnOk.EnableWindow( FALSE );
	//DisableDlgRightData( false );
	m_ctNew.EnableWindow();
	m_bIsNew=false;
	return CPropertyPage::OnApply();
}

void CCarScheduleDlg::OnContextMenu(CWnd* pWnd, CPoint point) 
{

	CRect rectTree;
	m_treeCarFlow.GetWindowRect( &rectTree );
	if( !rectTree.PtInRect(point) ) 
		return;

	m_treeCarFlow.SetFocus();
	CPoint pt = point;
	m_treeCarFlow.ScreenToClient( &pt );
	UINT iRet;
	m_hRClickItem = m_treeCarFlow.HitTest( pt, &iRet );
	if (iRet != TVHT_ONITEMLABEL)   // Must click on the label
	{
		m_hRClickItem = NULL;
	}

	if( m_hRClickItem )
	{
		
		CMenu menuPopup;
		menuPopup.LoadMenu( IDR_MENU_POPUP );
		CMenu* pMenu=NULL;		
		pMenu = menuPopup.GetSubMenu( 23 );

		if( m_hRClickItem == m_hStart )
		{
			if( m_treeItemVector.size() > 2 )
			{
				pMenu->EnableMenuItem(ID_CAR_FLOW_ADD, MF_GRAYED );
				pMenu->EnableMenuItem(ID_CAR_FLOW_DELETE, MF_GRAYED );
				pMenu->EnableMenuItem(ID_CAR_FLOW_DWELLINGTIME, MF_GRAYED );
				return;
			}
			else
			{
				pMenu->EnableMenuItem(ID_CAR_FLOW_ADD, MF_ENABLED );
				pMenu->EnableMenuItem(ID_CAR_FLOW_DELETE, MF_GRAYED );
				pMenu->EnableMenuItem(ID_CAR_FLOW_DWELLINGTIME, MF_GRAYED );
			}
		}
		else if ( m_hRClickItem == m_hEnd )
		{
			pMenu->EnableMenuItem(ID_CAR_FLOW_ADD, MF_GRAYED );
			pMenu->EnableMenuItem(ID_CAR_FLOW_DELETE, MF_GRAYED );
			pMenu->EnableMenuItem(ID_CAR_FLOW_DWELLINGTIME, MF_GRAYED );
			return;
		}
		else
		{
			m_treeCarFlow.SelectItem( m_hRClickItem );
			pMenu->EnableMenuItem(ID_CAR_FLOW_DELETE, MF_ENABLED );
			pMenu->EnableMenuItem(ID_CAR_FLOW_DWELLINGTIME, MF_ENABLED );

			if( m_hLastStationItem == m_hRClickItem )
			{
				pMenu->EnableMenuItem(ID_CAR_FLOW_ADD, MF_ENABLED );
			}
			else
			{
				pMenu->EnableMenuItem(ID_CAR_FLOW_ADD, MF_GRAYED );
			}
		}
	
		pMenu->TrackPopupMenu(TPM_LEFTALIGN, point.x, point.y, this);
	}
	

}

void CCarScheduleDlg::OnCarFlowDwellingtime() 
{
	
	for( int i=0;i<static_cast<int>(m_treeItemVector.size()); ++i )
	{
		if( m_treeItemVector[i] == m_hRClickItem )
		{
			m_iSelectStationIndex = i-1 ;
		}
	}

	int iDisplayNum =-1;
	if( m_enumClickType == ClickTypeNewSchedule ) //new schedule
	{
		iDisplayNum = m_pNewSingleSchedule->GetDwellingTime( m_iSelectStationIndex );
	}
	else
	{
		iDisplayNum = m_allCarSchedule.GetAllCarSchedule()->at( m_iCurrentScheduleIndex )->GetDwellingTime( m_iSelectStationIndex );		
	}


	m_treeCarFlow.SetDisplayType( 99 );
	IntegratedStation* pStation = (IntegratedStation*)m_treeCarFlow.GetItemData( m_hCurrentSel );
	char sStationName[64];
	pStation->getID()->printID( sStationName );
	CString sTitle= sStationName;
	sTitle +=" Dwelling Time ( s ) ";

	m_treeCarFlow.SetDisplayNum( iDisplayNum );
	m_treeCarFlow.SetSpinRange( 1, 10000);
	m_treeCarFlow.SpinEditLabel( m_hCurrentSel ,sTitle);	
}

LRESULT CCarScheduleDlg::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{

	if( message == WM_INPLACE_SPIN )
	{
		LPSPINTEXT pst = (LPSPINTEXT) lParam;
		
		if( m_enumClickType == ClickTypeNewSchedule ) //new schedule
		{
			m_pNewSingleSchedule->SetDwellingTime( m_iSelectStationIndex , pst->iPercent );
		}
		else
		{
			m_allCarSchedule.GetAllCarSchedule()->at( m_iCurrentScheduleIndex )->SetDwellingTime( m_iSelectStationIndex , pst->iPercent );
		}
		SetModified();
		LoadCarFlowTree();

		return TRUE;

	}
				
	
	return CPropertyPage::DefWindowProc(message, wParam, lParam);
}

void CCarScheduleDlg::OnEnChangeEditDwellingTime()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CPropertyPage::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	UpdateData(TRUE);
	if( m_iTrainNumber == 1 )
	{
		m_spinHeadwayTime.EnableWindow( FALSE );
		GetDlgItem( IDC_EDIT_HEADWAY_TIME )->EnableWindow( FALSE );
	}
	else if (m_iTrainNumber < 1)
	{
		AfxMessageBox(_T("Please input a number >= 1 "));
	}
	else
	{
		m_spinHeadwayTime.EnableWindow( TRUE );
		GetDlgItem( IDC_EDIT_HEADWAY_TIME )->EnableWindow( TRUE );
	}
	UpdateData(FALSE);
	SetModified();
}


// use this as the file filter type string
const CString CCarScheduleDlg::m_strFileFilterType = _T("Train Schedule File (*.skd)|*.skd;*.SKD|All type (*.*)|*.*|");

void CCarScheduleDlg::OnBnClickedBtnImportTimeTable()
{
	// TODO: Add your control notification handler code here
	CFileDialog filedlg( TRUE,_T("skd"), NULL,
		OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_SHAREAWARE ,
		m_strFileFilterType, NULL, 0, FALSE );
	if(filedlg.DoModal()!=IDOK)
	{
		return;
	}
	// dealing with importing
	CString csFileName=filedlg.GetPathName();
	if (ReadFile(csFileName))
	{
		SetControl();
		LoadExitCarScheduleList();
		LoadCarFlowTree();
		MessageBox(_T("Import train schedule successfully!"),"Warning",MB_OK);
	}
}

void CCarScheduleDlg::OnBnClickedBtnExportTimeTable()
{
	// TODO: Add your control notification handler code here
	CFileDialog filedlg( TRUE,_T("skd"), NULL,
		OFN_PATHMUSTEXIST | OFN_SHAREAWARE ,
		m_strFileFilterType, NULL ,0, FALSE);
	if(filedlg.DoModal()!=IDOK)
	{
		return;
	}


	CString csFileName=filedlg.GetPathName();
	// dealing with exporting
	CString Path = ((CTermPlanDoc*)((CView*)m_pParent)->GetDocument())->m_ProjInfo.path;
	 char filename[_MAX_PATH];
	PROJMANAGER->getFileName(Path,AllCarsScheduleFile,filename);
	if (CopyFile(filename,csFileName,FALSE))
	{
		MessageBox(_T("Export train schedule successfully!"),"Warning",MB_OK);
	}
	
}

bool CCarScheduleDlg::ReadFile( const CString &strFileName )
{
	ArctermFile file;
	if(!file.openFile( strFileName, CFile::modeRead ) )
	{
		CString strMsg;
		strMsg.Format("Can not open file:\r\n%s", strFileName );
		MessageBox( strMsg, "ERROR", MB_OK|MB_ICONERROR );
		return false;
	}

	file.getLine();
	int iCount=0;
	file.getInteger( iCount );

	//m_allCarSchedule.ClearCarSchedule();
	for( int i=0; i<iCount; ++i )
	{
		CSingleCarSchedule* pSingleSchedule=new CSingleCarSchedule();
		pSingleSchedule->SetSystemProcessorList(GetInputTerminal()->GetProcessorList());
		pSingleSchedule->SetSystemRailWay( GetInputTerminal()->pRailWay );
			
		if (pSingleSchedule->ReadData( file ))
		{
			m_allCarSchedule.AddNewCarSchedule( pSingleSchedule );
		}
		else
		{
			CString str;
			str.Format(_T("One or more stations don't exist in this model. Failed to import schedule!"));
			MessageBox(str,"Warning",MB_OK);
			return false;
		}	
	
	}
	return true;	
}