// OnboardReportControlView.cpp : implementation file
//

#include "stdafx.h"
#include "TermPlan.h"
#include "OnboardReportControlView.h"
#include ".\onboardreportcontrolview.h"
#include "TermPlanDoc.h"
#include "../OnboardReport/OnboardBaseParameter.h"
#include "../OnboardReport/OnboardBaseReport.h"
#include "../OnboardReport/OnboardReportManager.h"
#include "ARCReportManager.h"
#include "../Common/ViewMsg.h"
#include "OnboardReportTreePerformer.h"
#include "../Common/UnitsManager.h"
#include "../Common/ARCUnit.h"
#include "Common/SimAndReportManager.h"




const CString strBtnTxt[] =
{
	"Save Para",
		"Load Para",
		"Save Report",
		"Load Report"
};
const int iEdgeWidth		= 5; 
const int iToolBarHeight	= 34;

// OnboardReportControlView

IMPLEMENT_DYNCREATE(OnboardReportControlView, CFormView)

OnboardReportControlView::OnboardReportControlView()
	: CFormView(OnboardReportControlView::IDD)
{
	m_oleDTEndTime = COleDateTime::GetCurrentTime();
	m_oleDTStartTime = COleDateTime::GetCurrentTime();
	m_oleDTEndDate = COleDateTime::GetCurrentTime();
	m_oleDTStartDate = COleDateTime::GetCurrentTime();
	m_nRepType = 0;
	m_nInterval = 1;
	m_oleDTInterval = COleDateTime::GetCurrentTime();
	m_fThreshold = 0.0f;
	m_oleThreshold = COleDateTime::GetCurrentTime();
	m_nInterval = 0;

	m_pParameter = NULL;


	m_hRootFlightType = NULL;
	m_hRootPaxType = NULL;

	m_pTreePerformer = NULL;
}

OnboardReportControlView::~OnboardReportControlView()
{
}

void OnboardReportControlView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
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
	DDX_Text(pDX, IDC_EDIT_INTERVAL, m_nInterval);
	DDX_Control(pDX,IDC_SPIN_INTERVAL,m_spinInterval);
	DDX_Control(pDX, IDC_EDIT_INTERVAL, m_edtInterval);
	DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKER_END, m_oleDTEndTime);
	DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKER_START, m_oleDTStartTime);
	DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKER_END2, m_oleDTEndDate);
	DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKER_START2, m_oleDTStartDate);
	DDX_Radio(pDX, IDC_RADIO_DETAILED, m_nRepType);
	DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKER_INTERVAL, m_oleDTInterval);
	DDX_Text(pDX, IDC_EDIT_THRESHOLD, m_fThreshold);
	DDV_MinMaxFloat(pDX, m_fThreshold, 0.f, 1000.f);
	DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKER_THRESHOLD, m_oleThreshold);


}

BEGIN_MESSAGE_MAP(OnboardReportControlView, CFormView)
	ON_WM_SIZE()
	ON_BN_CLICKED(ID_BUTTON_CANCEL, OnButtonCancel)
	ON_BN_CLICKED(IDC_BUTTON_APPLY, OnButtonApply)
	ON_BN_CLICKED(IDC_BUTTON_LOAD_FROM_FILE, OnButtonReportDefault)
	ON_COMMAND(ID_TOOLBARBUTTONADD, OnToolbarbuttonadd)
	ON_COMMAND(ID_TOOLBARBUTTONDEL, OnToolbarbuttondel)
	ON_UPDATE_COMMAND_UI(ID_TOOLBARBUTTONADD, OnUpdateToolBarAdd)
	ON_UPDATE_COMMAND_UI(ID_TOOLBARBUTTONDEL, OnUpdateToolBarDel)
	ON_COMMAND(ID_AIRSIDEREPORT_SAVEREPORT,OnSaveReport)
	ON_COMMAND(ID_AIRSIDEREPORT_LOADREPORT,OnLoadReport)
	ON_COMMAND(ID_AIRSIDEREPORT_SAVEPARA, OnReportSavePara)
	ON_COMMAND(ID_AIRSIDEREPORT_LOADPARA, OnReportLoadPara)
	ON_WM_CREATE()
END_MESSAGE_MAP()


// OnboardReportControlView diagnostics

#ifdef _DEBUG
void OnboardReportControlView::AssertValid() const
{
	CFormView::AssertValid();
}

void OnboardReportControlView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG


// OnboardReportControlView message handlers

void OnboardReportControlView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	// TODO: Add your specialized code here and/or call the base class
	InitToolbar();
	InitializeControl();
	ShowOrHideControls();

	InitMuiButtom() ;
	//day combobox
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
		m_comboEndDay.SetCurSel(m_comboEndDay.GetCount()-1);


	OnboardBaseReport *pBaseeReport = GetDocument()->GetARCReportManager().GetOnboardReportManager()->GetReport();
	if (pBaseeReport)
	{
		m_pParameter = pBaseeReport->getParam();
	}

	UpdateControlContent();

}
void OnboardReportControlView::InitMuiButtom()
{
	m_btnMulti.SetType(CCoolBtn::TY_AIRSIDE) ;
	m_btnMulti.SetOperation(0) ;
	m_btnMulti.SetWindowText(strBtnTxt[0]) ;
}
void OnboardReportControlView::ClickMuiButton()
{
	int type = m_btnMulti.GetOperation() ;
	if (type == 0)
		OnReportSavePara();
	else if(type == 1)
		OnReportLoadPara();
	else if(type == 2)
		OnSaveReport();
	else if(type == 3)
		OnLoadReport();

}
void OnboardReportControlView::OnButtonReportDefault()
{
	CTermPlanDoc *pTermPlanDoc = GetDocument();	
	ASSERT(pTermPlanDoc);
	
	//load report
	OnboardBaseReport *pBaseeReport = pTermPlanDoc->GetARCReportManager().GetOnboardReportManager()->GetReport();
	if(pBaseeReport)
	{
		pBaseeReport->LoadReportData();
	}	
	GetDocument()->UpdateAllViews(this,AIRSIDEREPORT_SHOWREPORT,NULL);
	
	UpdateControlContent();
}
void OnboardReportControlView::OnReportSavePara()
{
	if(SaveData() == FALSE)
	{
		MessageBox("Save Report Parameter failed",_T("Error"),MB_OK);
		return;
	}

	CFileDialog savedlg(FALSE,".par",NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		"Report Parameter(*.par)|*.par||",this );
	if( savedlg.DoModal() == IDOK )
	{
		CString strFileName = savedlg.GetPathName();
		ASSERT( m_pParameter );
		CTermPlanDoc *pTermPlanDoc = GetDocument();	
		ASSERT(pTermPlanDoc);

		//copy the parameter to the specified location
	/*	BOOL bResult = pTermPlanDoc->GetARCReportManager().GetOnboardReportManager()->GetReport()->SaveReportData();
		if (bResult)
		{
			MessageBox(_T("Save Report Parameter Successfully"),NULL,MB_OK);
		}*/
		OnboardBaseReport *pBaseeReport = pTermPlanDoc->GetARCReportManager().GetOnboardReportManager()->GetReport();
		if (pBaseeReport)
		{
			CString strReportFile = pBaseeReport->GetReportFilePath();
			OnboardBaseParameter* pParameter = pBaseeReport->getParam();
			if (pParameter)
			{
				CString sZipFileName = pParameter->GetReportParamPath();
				if(FileManager::CopyFile(sZipFileName,strFileName,TRUE))
				{
					MessageBox(_T("Save Report Parameter Successfully"),NULL,MB_OK) ;
				}
			}
		}
		
	}
}

void OnboardReportControlView::OnReportLoadPara()
{
	CFileDialog loaddlg(TRUE,NULL,NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		"Report Parameter(*.par)|*.par||",this );
	if( loaddlg.DoModal() == IDOK )
	{
		CString strFileName = loaddlg.GetPathName();
		ArctermFile _file;
		CTermPlanDoc *pTermPlanDoc = GetDocument();	
		ASSERT(pTermPlanDoc);
		if(!_file.openFile(strFileName,READ) )
		{
			MessageBox(_T("Read File error"),_T("Error"),MB_OK);
			return;
		}

		m_pParameter = pTermPlanDoc->GetARCReportManager().GetOnboardReportManager()->GetParameters();

		//use the parameter to load the file
		m_pParameter->LoadParameterFromFile(_file);
		MessageBox(_T("Load Report Parameter Successfully"),NULL,MB_OK) ;
		UpdateControlContent();
		InitializeTree();
		InitializeDefaultTreeData();
		
		_file.closeIn();
	}
}

void OnboardReportControlView::OnSaveReport()
{
	m_btnMulti.SetWindowText(strBtnTxt[2]) ;
	m_btnMulti.SetOperation(1) ;
	CFileDialog dlgFile( FALSE, ".rep",NULL ,OFN_OVERWRITEPROMPT|OFN_HIDEREADONLY,"Report Files (*.rep)|*.rep||" );
	CString sZipFileName;
	if (dlgFile.DoModal() == IDOK)
		sZipFileName = dlgFile.GetPathName();
	else
		return ;

	//get report type
	CTermPlanDoc *pTermPlanDoc = GetDocument();	
	ASSERT(pTermPlanDoc);
	OnboardReportType rpType = pTermPlanDoc->GetARCReportManager().GetOnboardReportManager()->GetReportType();


	OnboardBaseReport *pBaseeReport = pTermPlanDoc->GetARCReportManager().GetOnboardReportManager()->GetReport();
	if(pBaseeReport)
	{
		CString strReportFile = pBaseeReport->GetReportFilePath();
		if(FileManager::CopyFile(strReportFile,sZipFileName,TRUE))
		{
			MessageBox(_T("Export Report Successfully"),NULL,MB_OK) ;
		}
	}
	

}
void OnboardReportControlView::OnLoadReport()
{
	m_btnMulti.SetWindowText(strBtnTxt[3]) ;
	m_btnMulti.SetOperation(0) ;
	CFileDialog dlgFile( TRUE, ".rep",NULL ,OFN_OVERWRITEPROMPT|OFN_HIDEREADONLY,"Report Files (*.rep)|*.rep||" );
	CString sZipFileName;
	if (dlgFile.DoModal() == IDOK)
		sZipFileName = dlgFile.GetPathName();
	else
		return ;


	CTermPlanDoc *pTermPlanDoc = GetDocument();	
	ASSERT(pTermPlanDoc);
	OnboardReportType rpType = pTermPlanDoc->GetARCReportManager().GetOnboardReportManager()->GetReportType();
	
	//load report
	OnboardBaseReport *pBaseeReport = pTermPlanDoc->GetARCReportManager().GetOnboardReportManager()->GetReport();
	if(pBaseeReport)
	{
		pBaseeReport->LoadReportData(sZipFileName);
		UpdateControlContent();
	}	
	GetDocument()->UpdateAllViews(this,AIRSIDEREPORT_SHOWREPORT,NULL);
}

void OnboardReportControlView::LoadData()
{
	m_pParameter = GetDocument()->GetARCReportManager().GetOnboardReportManager()->GetParameters();

	if(m_pParameter == NULL)
		return;

	//start time 
	ElapsedTime eStartTime = m_pParameter->getStartTime();
	int nStartDay = eStartTime.GetDay();
	if(nStartDay > 0)
		nStartDay -= 1;//index = nEndday -1
	if(m_comboStartDay.GetCount() > nStartDay)
		m_comboStartDay.SetCurSel(nStartDay);

	COleDateTime oleStartTime;
	oleStartTime.SetTime( eStartTime.GetHour(),eStartTime.GetMinute(), eStartTime.GetSecond() );
	m_dtctrlStartTime.SetTime( oleStartTime);
	m_oleDTStartTime = oleStartTime;
	//end time

	ElapsedTime eEndTime = m_pParameter->getEndTime();
	if(eEndTime > ElapsedTime(0L))
	{
		int nEndDay = eEndTime.GetDay();
		if(nEndDay > 0)
			nEndDay -= 1;
		m_comboEndDay.SetCurSel(nEndDay);	
	}
	else
	{
		if(m_comboEndDay.GetCount() > 0)
			m_comboEndDay.SetCurSel(m_comboEndDay.GetCount()-1);
	}

	COleDateTime oleEndTime;
	oleEndTime.SetTime( eEndTime.GetHour(),eEndTime.GetMinute(), eEndTime.GetSecond() );
	m_dtctrlEndTime.SetTime( oleEndTime);
	m_oleDTEndTime = oleEndTime;
	//summary or detail

	if(m_pParameter->getReportType() == OnboardReport_Detail)
	{
		m_radioDetailed.SetCheck(1);
		m_radioSummary.SetCheck(0);
		m_nRepType = 0;
	}
	else
	{
		m_radioDetailed.SetCheck(0);
		m_radioSummary.SetCheck(1);
		m_nRepType = 1;
	}
	//interval

	ElapsedTime eIntervalTime = m_pParameter->getInterval();
	COleDateTime oleInterval;
	oleInterval.SetTime(eIntervalTime.GetHour() , eIntervalTime.GetMinute(), 0 );
	m_dtctrlInterval.SetTime( oleInterval );
	m_oleDTInterval = oleInterval;

	m_nInterval =m_pParameter->getDistanceInterval()/100;

	UpdateData(FALSE);
}
void OnboardReportControlView::OnSize(UINT nType, int cx, int cy)
{
	CFormView::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
	DoSizeWindow(cx,cy);
}
void OnboardReportControlView::DoSizeWindow( int cx, int cy )
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
	m_edtInterval.MoveWindow(x,y,iWidth,19);
	m_spinInterval.MoveWindow(x,y,20,20);
	m_spinInterval.SetBuddy(&m_edtInterval);

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
void OnboardReportControlView::SizeDownCtrl(int cx, int cy )
{
	int x,y;
	x = iEdgeWidth;
	y = 111;
	int iWidth, iHeight;
	iWidth = cx-2*iEdgeWidth;
	iHeight = cy-y-70;
	CRect rc;


	m_staticTypeListLabel.MoveWindow( x, y, iWidth , iHeight );
	m_lstboPaxType.MoveWindow( x,y+iToolBarHeight,iWidth, iHeight-iToolBarHeight );
	m_treePaxType.MoveWindow( x,y+iToolBarHeight,iWidth, iHeight-iToolBarHeight );

	x = iEdgeWidth;
	y = y+ iHeight;
	m_staticArea.MoveWindow( x, y, 30, 15);
	m_comboAreaPortal.MoveWindow( x, y+ 18, cx-2*iEdgeWidth,20 );
	////////////////////////////////////////////////////////////////////////////
	//// move button
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

	////////////////////////////////////////////////////////////////////////////	
	SetScaleToFitSize(CSize(cx,cy));
}
void OnboardReportControlView::InitializeControl()
{	
	// init start end time

	COleDateTime oleStartTime,oleEndTime,oleInterval,oleThreshold;
	oleStartTime.SetTime( 0, 0, 0 );
	m_dtctrlStartTime.SetTime( oleStartTime);
	//m_dtctrlStartTime.SetFormat("HH:mm:ss");
	oleEndTime = oleStartTime + COleDateTimeSpan( 0, 23, 59, 59 );
	m_dtctrlEndTime.SetTime(oleStartTime);
	//m_dtctrlEndTime.SetFormat("HH:mm:ss");

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

	EnableControl();
}
void OnboardReportControlView::ShowOrHideControls()
{
	//hide the control
	//m_staticInterval.EnableWindow( TRUE );
	//m_dtctrlInterval.EnableWindow(TRUE);

	m_btnMulti.ShowWindow(SW_SHOW);
	m_btLoad.ShowWindow(SW_SHOW);
	m_staticArea.ShowWindow(SW_HIDE);
	m_comboAreaPortal.ShowWindow(SW_HIDE);

	m_lstboProType.ShowWindow( SW_HIDE );
	m_staProc.ShowWindow( SW_HIDE );
	m_staticProListLabel.ShowWindow( SW_HIDE );

	m_staPax.SetWindowText( "Passenger Type" );
	
	m_staticTypeListLabel.ShowWindow( SW_SHOW );
	m_lstboPaxType.ShowWindow( SW_HIDE );
	m_treePaxType.ShowWindow(SW_SHOW);
	m_PasToolBar.GetToolBarCtrl().ShowWindow( SW_SHOW );

	//hide 
	m_dtctrlStartDate.ShowWindow(SW_HIDE);
	m_dtctrlEndDate.ShowWindow(SW_HIDE);

	m_comboStartDay.ShowWindow(SW_SHOW);
	m_comboEndDay.ShowWindow(SW_SHOW);

	//enable
	EnableControl();
}
void OnboardReportControlView::EnableControl()
{
	OnboardReportType rpType = GetDocument()->GetARCReportManager().GetOnboardReportManager()->GetReportType();
	switch(rpType) 
	{
	case ONBOARD_PAX_ACTIVITY:
		{
			m_staticInterval.EnableWindow( TRUE );
			m_dtctrlInterval.EnableWindow(TRUE);
			
			//summary or detail report
			//activity has only detail report
			m_radioDetailed.EnableWindow(FALSE);
			m_radioSummary.EnableWindow(FALSE);

			//have no threshold in the report
			m_spinThreshold.EnableWindow(FALSE);
			m_chkThreshold.EnableWindow(FALSE);
			m_edtThreshold.EnableWindow(FALSE);
			m_edtInterval.ShowWindow(FALSE);
		}
		break;
	case ONBOARD_PAX_CONFLICT:
	case ONBOARD_PAX_DURATION:
	case ONBOARD_CARRYON_TIME:
		{
			m_staticInterval.EnableWindow( TRUE );
			m_dtctrlInterval.EnableWindow(TRUE);

			//summary or detail report
			//activity has only detail report
			m_radioDetailed.EnableWindow(TRUE);
			m_radioSummary.EnableWindow(TRUE);

			//have no threshold in the report
			m_spinThreshold.EnableWindow(FALSE);
			m_chkThreshold.EnableWindow(FALSE);
			m_edtThreshold.EnableWindow(FALSE);

			m_edtInterval.ShowWindow(FALSE);
		}
		break;
	case ONBOARD_PAX_CARRYON:
		{
			m_staticInterval.EnableWindow( TRUE );
			m_dtctrlInterval.EnableWindow(TRUE);

			//summary or detail report
			//activity has only detail report
			m_radioDetailed.EnableWindow(TRUE);
			m_radioSummary.EnableWindow(TRUE);

			//have no threshold in the report
			m_spinThreshold.EnableWindow(FALSE);
			m_chkThreshold.EnableWindow(FALSE);
			m_edtThreshold.EnableWindow(FALSE);

			m_edtInterval.ShowWindow(FALSE);
		}
		break;
	case ONBOARD_PAX_DISTANCETRAVEL:
		{
			m_staticInterval.EnableWindow( TRUE );
			m_dtctrlInterval.EnableWindow(FALSE);

			//summary or detail report
			//activity has only detail report
			m_radioDetailed.EnableWindow(TRUE);
			m_radioSummary.EnableWindow(TRUE);

			//have no threshold in the report
			m_spinThreshold.EnableWindow(FALSE);
			m_chkThreshold.EnableWindow(FALSE);
			m_edtThreshold.EnableWindow(FALSE);

			m_edtInterval.ShowWindow(TRUE);
		}
		break;
	default:
		{
			m_staticInterval.EnableWindow( TRUE );
			m_dtctrlInterval.EnableWindow(TRUE);
			m_staticType.EnableWindow(TRUE);

			m_radioDetailed.EnableWindow(TRUE);
			m_radioSummary.EnableWindow(TRUE);

			//m_staticThreshold.EnableWindow(TRUE);
			m_spinThreshold.EnableWindow(TRUE);
			m_chkThreshold.EnableWindow(TRUE);
			m_edtThreshold.EnableWindow(TRUE);

			m_edtInterval.ShowWindow(FALSE);
		}
		break;
	} 
}
void OnboardReportControlView::InitToolbar()
{
	CRect rc;
	m_toolbarcontenter1.GetWindowRect( &rc );
	ScreenToClient(&rc);
	m_PasToolBar.MoveWindow(rc.left,rc.top,50,rc.Height());
	m_PasToolBar.ShowWindow(SW_SHOW);
	m_toolbarcontenter1.ShowWindow(SW_HIDE);
	m_PasToolBar.GetToolBarCtrl().HideButton( ID_TOOLBARBUTTONEDIT);

	m_toolbarcontenter2.ShowWindow(SW_HIDE);
}

int OnboardReportControlView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFormView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here
	if (!m_PasToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_PasToolBar.LoadToolBar(IDR_ADDDELEDITTOOLBAR))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}
	return 0;
}
void OnboardReportControlView::ResetAllContent()
{
	CRect rc;
	GetClientRect(&rc);
	SizeDownCtrl( rc.Width(), rc.Height() );


	m_pParameter = GetDocument()->GetARCReportManager().GetOnboardReportManager()->GetParameters();

	InitializeControl();
	LoadData();
	InitializeTree();
	InitializeDefaultTreeData();
}

void OnboardReportControlView::InitializeTree()
{

	CTermPlanDoc *pTermPlanDoc = GetDocument();	
	ASSERT(pTermPlanDoc);
	OnboardReportType rpType = pTermPlanDoc->GetARCReportManager().GetOnboardReportManager()->GetReportType();
	switch(rpType)
	{
	case ONBOARD_PAX_DURATION:
	case ONBOARD_PAX_ACTIVITY:
	case ONBOARD_PAX_CONFLICT:
	case ONBOARD_PAX_CARRYON:
	case ONBOARD_PAX_DISTANCETRAVEL:
	case ONBOARD_CARRYON_TIME:
		{
			if(GetTreePerformer())
			{
				GetTreePerformer()->LoadData();
			}
		}
		break;

	default:
		{
			m_treePaxType.DeleteAllItems();
			m_hRootFlightType = m_treePaxType.InsertItem("Flight Type");
			m_treePaxType.SetItemData(m_hRootFlightType, NULL);

			m_hRootFlightType = m_treePaxType.InsertItem("Passenger Type");
			m_treePaxType.SetItemData(m_hRootFlightType,NULL);
		}
		break;
	}
}
void OnboardReportControlView::InitializeDefaultTreeData()
{
	if(m_pParameter == NULL)
		return;
	CTermPlanDoc *pTermPlanDoc = GetDocument();	
	ASSERT(pTermPlanDoc);
	OnboardReportType rpType = pTermPlanDoc->GetARCReportManager().GetOnboardReportManager()->GetReportType();
	switch(rpType)
	{
	case ONBOARD_PAX_DURATION:
	case ONBOARD_PAX_ACTIVITY:
	case ONBOARD_PAX_CONFLICT:
	case ONBOARD_PAX_CARRYON:
	case ONBOARD_PAX_DISTANCETRAVEL:
	case ONBOARD_CARRYON_TIME:
		break;
	default:
		{
		}
		break;
	}
}

BOOL OnboardReportControlView::SaveData()
{
	//start time ,end time
	UpdateData();

	m_pParameter->SetProjID(GetDocument()->GetProjectID());
	ElapsedTime time;
	bool bAbsDate = false;
	int nDtIdx=0;
	if (m_startDate.IsAbsoluteDate())
	{
		bAbsDate = true;
		m_startDate.GetElapsedTime(bAbsDate,m_oleDTStartDate,nDtIdx,m_oleDTStartTime,time);	
		m_pParameter->setStartTime( time );	
		m_startDate.GetElapsedTime(bAbsDate,m_oleDTEndDate,nDtIdx,m_oleDTEndTime,time);	
		m_pParameter->setEndTime( time );	
	}
	else
	{
		bAbsDate = false;
		nDtIdx = m_comboStartDay.GetCurSel();
		//m_startDate.GetElapsedTime(bAbsDate,m_oleDTStartDate,nDtIdx,m_oleDTStartTime,time);
		ElapsedTime starttime;
		starttime.set(nDtIdx*24 + m_oleDTStartTime.GetHour(), m_oleDTStartTime.GetMinute(),m_oleDTStartTime.GetSecond());
		m_pParameter->setStartTime( starttime );	

		nDtIdx = m_comboEndDay.GetCurSel();
		//m_startDate.GetElapsedTime(bAbsDate,m_oleDTEndDate,nDtIdx,m_oleDTEndTime,time);	

		ElapsedTime endtime;
		endtime.set(nDtIdx*24 + m_oleDTEndTime.GetHour() , m_oleDTEndTime.GetMinute(), m_oleDTEndTime.GetSecond());
		m_pParameter->setEndTime( endtime );	
	}

	time.set( m_oleDTInterval.GetHour(), m_oleDTInterval.GetMinute(), m_oleDTInterval.GetSecond() );
	if(time.asSeconds() == 0)
	{
		MessageBox( "Interval time should not be '0'.", "Error", MB_OK|MB_ICONWARNING );
		return FALSE;
	}

	CTermPlanDoc *pTermPlanDoc = GetDocument();	
	OnboardReportType rpType = pTermPlanDoc->GetARCReportManager().GetOnboardReportManager()->GetReportType();
	if (rpType == ONBOARD_PAX_DISTANCETRAVEL)
	{
		if (m_nInterval == 0)
		{
			MessageBox( "Interval distance should not be '0'.", "Error", MB_OK|MB_ICONWARNING );
			return FALSE;
		}
	}
	
	m_pParameter->setDistanceInterval(m_nInterval*100l);

	m_pParameter->setInterval( time.asSeconds() );

	//summary or detail
	if (m_radioDetailed.GetCheck())
	{
		m_pParameter->setReportType(OnboardReport_Detail);
	}
	else
	{
		m_pParameter->setReportType(OnboardReport_Summary);
	}

	//flight type


	//passenger type
	if(GetTreePerformer())
		GetTreePerformer()->SaveData();
	return TRUE;
}
void OnboardReportControlView::OnButtonApply()
{
	if(SaveData() == FALSE)
		return;
	try
	{
		GetDocument()->GetARCReportManager().GetOnboardReportManager()->GenerateReport();
		GetDocument()->UpdateAllViews(this,AIRSIDEREPORT_SHOWREPORT,NULL);
	}
	catch(StringError* pErr)
	{
		AfxMessageBox("The airside log file is old, run simulation please!");
		delete pErr;		
	}

}
void OnboardReportControlView::OnButtonCancel()
{

}
void OnboardReportControlView::UpdateControlContent()
{
	if(m_pParameter == NULL)
		return;

	//start time 
	ElapsedTime eStartTime = m_pParameter->getStartTime();
	int nStartDay = eStartTime.GetDay();
	if(nStartDay > 0)
		nStartDay -= 1;//index = nEndday -1
	if(m_comboStartDay.GetCount() > nStartDay)
		m_comboStartDay.SetCurSel(nStartDay);

	COleDateTime oleStartTime;
	oleStartTime.SetTime( eStartTime.GetHour(),eStartTime.GetMinute(), eStartTime.GetSecond() );
	//m_dtctrlStartTime.SetTime( oleStartTime);
	m_oleDTStartTime = oleStartTime;
	//end time

	ElapsedTime eEndTime = m_pParameter->getEndTime();
	if(eEndTime > ElapsedTime(0L))
	{
		int nEndDay = eEndTime.GetDay();
		if(nEndDay > 0)
			nEndDay -= 1;
		m_comboEndDay.SetCurSel(nEndDay);	
	}
	else
	{
		if(m_comboEndDay.GetCount() > 0)
			m_comboEndDay.SetCurSel(m_comboEndDay.GetCount()-1);
	}

	COleDateTime oleEndTime;
	oleEndTime.SetTime( eEndTime.GetHour(),eEndTime.GetMinute(), eEndTime.GetSecond() );
	//m_dtctrlEndTime.SetTime( oleEndTime);
	m_oleDTEndTime = oleEndTime;
	//summary or detail

	if(m_pParameter->getReportType() == OnboardReport_Detail)
	{
		m_radioDetailed.SetCheck(BST_CHECKED);
		m_radioSummary.SetCheck(BST_UNCHECKED);
		m_nRepType = 0;
	}
	else
	{
		m_radioDetailed.SetCheck(BST_UNCHECKED);
		m_radioSummary.SetCheck(BST_CHECKED);
		m_nRepType = 1;
	}
	//interval

	ElapsedTime eIntervalTime = m_pParameter->getInterval();
	COleDateTime oleInterval;
	oleInterval.SetTime(eIntervalTime.GetHour() , eIntervalTime.GetMinute(), 0 );
	//m_dtctrlInterval.SetTime( oleInterval );
	m_oleDTInterval = oleInterval;

	UpdateData(FALSE);
	Invalidate(FALSE);
}

















void OnboardReportControlView::OnUpdate(CView* /*pSender*/, LPARAM lHint, CObject* /*pHint*/)
{
	if(lHint == VM_GLOBALUNIT_CHANGER)
	{
		//CAirsideBaseReport * baseReport = GetDocument()->GetARCReportManager().()->GetReport() ;
		//if(baseReport->IsReportGenerate() == TRUE)
		//	GetDocument()->UpdateAllViews(this,AIRSIDEREPORT_SHOWREPORT,NULL);
	}

	if (lHint == AIRSIDEREPORT_SHOWREPORT)
	{
		UpdateControlContent();
	}
}

COnboardReportPerformer* OnboardReportControlView::GetTreePerformer()
{
	CTermPlanDoc *pTermPlanDoc = GetDocument();	
	if(pTermPlanDoc == NULL)
		return NULL;

	if(m_pParameter == NULL)
		return NULL;

	OnboardReportType rpType = pTermPlanDoc->GetARCReportManager().GetOnboardReportManager()->GetReportType();

	switch (rpType)
	{
	case ONBOARD_PAX_DURATION:
	case ONBOARD_PAX_ACTIVITY:
	case ONBOARD_PAX_CONFLICT:
	case ONBOARD_PAX_CARRYON:
	case ONBOARD_PAX_DISTANCETRAVEL:
	case ONBOARD_CARRYON_TIME:
		{
			if(m_pTreePerformer)
			{
				if(m_pTreePerformer->GetReportType() == rpType)//same report
				{
					m_pTreePerformer->UpdateOnboardParameter(m_pParameter);
					return m_pTreePerformer;
				}
			}

			delete m_pTreePerformer;
			m_pTreePerformer = NULL;
		
			m_pTreePerformer = new COnboardReportTreeDefaultPerformer(&m_treePaxType,(InputTerminal*)(&pTermPlanDoc->GetTerminal()),m_pParameter, rpType);
			return m_pTreePerformer;
		}
	default:
		break;
	}

	return NULL;
}

void OnboardReportControlView::OnToolbarbuttonadd()
{
	CTermPlanDoc *pTermPlanDoc = GetDocument();	
	ASSERT(pTermPlanDoc);
	OnboardReportType rpType = pTermPlanDoc->GetARCReportManager().GetOnboardReportManager()->GetReportType();


	HTREEITEM hSelectItem=m_treePaxType.GetSelectedItem();
	if (hSelectItem == NULL)
		return;

	if(GetTreePerformer())
	{
		GetTreePerformer()->OnToolBarAdd(hSelectItem);
	}
}

void OnboardReportControlView::OnToolbarbuttondel()
{
	CTermPlanDoc *pTermPlanDoc = GetDocument();	
	ASSERT(pTermPlanDoc);
	OnboardReportType rpType = pTermPlanDoc->GetARCReportManager().GetOnboardReportManager()->GetReportType();


	HTREEITEM hSelectItem=m_treePaxType.GetSelectedItem();
	if (hSelectItem == NULL)
		return;

	if(GetTreePerformer())
	{
		GetTreePerformer()->OnToolBarDel(hSelectItem);
	}
}

void OnboardReportControlView::OnUpdateToolBarAdd(CCmdUI *pCmdUI)
{
	CTermPlanDoc *pTermPlanDoc = GetDocument();	
	ASSERT(pTermPlanDoc);
	OnboardReportType rpType = pTermPlanDoc->GetARCReportManager().GetOnboardReportManager()->GetReportType();


	HTREEITEM hSelectItem=m_treePaxType.GetSelectedItem();
	if (hSelectItem == NULL)
		return;

	if(GetTreePerformer())
	{
		GetTreePerformer()->OnUpdateToolBarAdd(pCmdUI);
	}
}

void OnboardReportControlView::OnUpdateToolBarDel(CCmdUI *pCmdUI)
{
	CTermPlanDoc *pTermPlanDoc = GetDocument();	
	ASSERT(pTermPlanDoc);
	OnboardReportType rpType = pTermPlanDoc->GetARCReportManager().GetOnboardReportManager()->GetReportType();


	HTREEITEM hSelectItem=m_treePaxType.GetSelectedItem();
	if (hSelectItem == NULL)
		return;

	if(GetTreePerformer())
	{
		GetTreePerformer()->OnUpdateToolBarDel(pCmdUI);
	}
}
