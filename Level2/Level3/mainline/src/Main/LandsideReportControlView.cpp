// LandsideReportControlView.cpp : implementation file
//

#include "stdafx.h"
#include "TermPlan.h"
#include "LandsideReportControlView.h"
#include "TermPlanDoc.h"
#include "ARCReportManager.h"
#include "../LandsideReport/LandsideReportManager.h"
#include "../LandsideReport/LandsideBaseReport.h"
#include "../LandsideReport/LandsideBaseParam.h"
#include "../Common/ViewMsg.h"
#include "../LandsideReport/LandsideReportType.h"
#include "LandsideReportTreePerformer.h"
#include "Landside/LandsidePortals.h"
#include "Landside/InputLandside.h"
#include "../LandsideReport/LSVehicleThroughputParam.h"
#include "Common/SimAndReportManager.h"



// LandsideReportControlView
const CString strBtnTxt[] =
{
	"Save Para",
		"Load Para",
		"Save Report",
		"Load Report"
};
const int iEdgeWidth		= 5; 
const int iToolBarHeight	= 34;


IMPLEMENT_DYNCREATE(LandsideReportControlView, CFormView)

LandsideReportControlView::LandsideReportControlView()
	: CFormView(LandsideReportControlView::IDD)
{
	m_oleDTEndTime = COleDateTime::GetCurrentTime();
	m_oleDTStartTime = COleDateTime::GetCurrentTime();
	m_oleDTEndDate = COleDateTime::GetCurrentTime();
	m_oleDTStartDate = COleDateTime::GetCurrentTime();
	m_nRepType = 0;
	m_oleDTInterval = COleDateTime::GetCurrentTime();
	m_fThreshold = 0.0f;
	m_oleThreshold = COleDateTime::GetCurrentTime();
	m_nInterval = 10;

	m_pParameter = NULL;
	
	m_pTreePerformer = NULL;


}

LandsideReportControlView::~LandsideReportControlView()
{
}

void LandsideReportControlView::DoDataExchange(CDataExchange* pDX)
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

BEGIN_MESSAGE_MAP(LandsideReportControlView, CFormView)
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


// LandsideReportControlView diagnostics

#ifdef _DEBUG
void LandsideReportControlView::AssertValid() const
{
	CFormView::AssertValid();
}

void LandsideReportControlView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG


// LandsideReportControlView message handlers
void LandsideReportControlView::OnInitialUpdate()
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


	LandsideBaseReport *pBaseeReport = GetDocument()->GetARCReportManager().GetLandsideReportManager()->GetReport();
	if (pBaseeReport)
	{
		m_pParameter = pBaseeReport->getParam();
	}

	UpdateControlContent();

}

void LandsideReportControlView::InitMuiButtom()
{
	m_btnMulti.SetType(CCoolBtn::TY_AIRSIDE) ;
	m_btnMulti.SetOperation(0) ;
	m_btnMulti.SetWindowText(strBtnTxt[0]) ;
}
void LandsideReportControlView::ClickMuiButton()
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
void LandsideReportControlView::OnButtonReportDefault()
{
	CTermPlanDoc *pTermPlanDoc = GetDocument();	
	ASSERT(pTermPlanDoc);
	
	//load report
	LandsideBaseReport *pBaseeReport = pTermPlanDoc->GetARCReportManager().GetLandsideReportManager()->GetReport();
	if(pBaseeReport)
	{
		pBaseeReport->LoadReportData();
	}	
	GetDocument()->UpdateAllViews(this,AIRSIDEREPORT_SHOWREPORT,NULL);
	
	UpdateControlContent();
}
void LandsideReportControlView::OnReportSavePara()
{
	if(SaveData() == FALSE)
	{
		MessageBox("Save Report Parameter failed",_T("Error"),MB_OK);
		return;
	}

	CFileDialog savedlg(FALSE,".par",NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		"Report Parameter(*.par)|*.par||",this, 0, FALSE  );
	if( savedlg.DoModal() == IDOK )
	{
		CString strFileName = savedlg.GetPathName();
		ASSERT( m_pParameter );
		CTermPlanDoc *pTermPlanDoc = GetDocument();	
		ASSERT(pTermPlanDoc);

		//copy the parameter to the specified location
	/*	BOOL bResult = pTermPlanDoc->GetARCReportManager().GetLandsideReportManager()->GetReport()->SaveReportData();
		if (bResult)
		{
			MessageBox(_T("Save Report Parameter Successfully"),NULL,MB_OK);
		}*/
		LandsideBaseReport *pBaseeReport = pTermPlanDoc->GetARCReportManager().GetLandsideReportManager()->GetReport();
		if (pBaseeReport)
		{
			CString strReportFile = pBaseeReport->GetReportFilePath();
			LandsideBaseParam* pParameter = pBaseeReport->getParam();
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

void LandsideReportControlView::OnReportLoadPara()
{
	CFileDialog loaddlg(TRUE,NULL,NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		"Report Parameter(*.par)|*.par||",this,  0, FALSE  );
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

		m_pParameter = pTermPlanDoc->GetARCReportManager().GetLandsideReportManager()->GetParameters();

		//use the parameter to load the file
		m_pParameter->LoadParameterFromFile(_file);
		MessageBox(_T("Load Report Parameter Successfully"),NULL,MB_OK) ;
		UpdateControlContent();
		InitializeTree();
		InitializeDefaultTreeData();
		
		_file.closeIn();
	}
}

void LandsideReportControlView::OnSaveReport()
{
	m_btnMulti.SetWindowText(strBtnTxt[2]) ;
	m_btnMulti.SetOperation(1) ;
	CFileDialog dlgFile( FALSE, ".rep",NULL ,OFN_OVERWRITEPROMPT|OFN_HIDEREADONLY,"Report Files (*.rep)|*.rep||" , NULL, 0, FALSE );
	CString sZipFileName;
	if (dlgFile.DoModal() == IDOK)
		sZipFileName = dlgFile.GetPathName();
	else
		return ;

	//get report type
	CTermPlanDoc *pTermPlanDoc = GetDocument();	
	ASSERT(pTermPlanDoc);
	LandsideReportType rpType = pTermPlanDoc->GetARCReportManager().GetLandsideReportManager()->GetReportType();


	LandsideBaseReport *pBaseeReport = pTermPlanDoc->GetARCReportManager().GetLandsideReportManager()->GetReport();
	if(pBaseeReport)
	{
		CString strReportFile = pBaseeReport->GetReportFilePath();
		if(FileManager::CopyFile(strReportFile,sZipFileName,TRUE))
		{
			MessageBox(_T("Export Report Successfully"),NULL,MB_OK) ;
		}
	}
	

}
void LandsideReportControlView::OnLoadReport()
{
	m_btnMulti.SetWindowText(strBtnTxt[3]) ;
	m_btnMulti.SetOperation(0) ;
	CFileDialog dlgFile( TRUE, ".rep",NULL ,OFN_OVERWRITEPROMPT|OFN_HIDEREADONLY,"Report Files (*.rep)|*.rep||" , NULL, 0, FALSE );
	CString sZipFileName;
	if (dlgFile.DoModal() == IDOK)
		sZipFileName = dlgFile.GetPathName();
	else
		return ;


	CTermPlanDoc *pTermPlanDoc = GetDocument();	
	ASSERT(pTermPlanDoc);
	LandsideReportType rpType = pTermPlanDoc->GetARCReportManager().GetLandsideReportManager()->GetReportType();
	
	//load report
	LandsideBaseReport *pBaseeReport = pTermPlanDoc->GetARCReportManager().GetLandsideReportManager()->GetReport();
	if(pBaseeReport)
	{
		pBaseeReport->LoadReportData(sZipFileName);
		UpdateControlContent();
	}	
	GetDocument()->UpdateAllViews(this,AIRSIDEREPORT_SHOWREPORT,NULL);
}

void LandsideReportControlView::LoadData()
{
	m_pParameter = GetDocument()->GetARCReportManager().GetLandsideReportManager()->GetParameters();

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

	if(m_pParameter->getReportType() == LandsideReport_Detail)
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

	if(eIntervalTime == ElapsedTime(0L))
		oleInterval.SetTime(1, 0, 0);
	else
	{
		oleInterval.SetTime(eIntervalTime.GetHour() , eIntervalTime.GetMinute(), 0 );
	}

	m_oleDTInterval = oleInterval;



	m_nInterval =m_pParameter->getDistanceInterval();

	UpdateData(FALSE);
}
void LandsideReportControlView::OnSize(UINT nType, int cx, int cy)
{
	CFormView::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
	DoSizeWindow(cx,cy);
}
void LandsideReportControlView::DoSizeWindow( int cx, int cy )
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
void LandsideReportControlView::SizeDownCtrl(int cx, int cy )
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
void LandsideReportControlView::InitializeControl()
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
	m_oleDTInterval.SetTime(1,0,0);

	m_dtctrlThreshold.SetFormat( "HH:mm" );
	oleThreshold.SetTime( 1, 0, 0 );
	m_dtctrlInterval.SetTime( oleThreshold );

	//	m_spinInterval.SetRange( 0, 120 );
	m_spinThreshold.SetRange( 0, 1000 );

	EnableControl();
}
void LandsideReportControlView::ShowOrHideControls()
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

	m_staPax.SetWindowText( "Vehicle Type" );
	
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
void LandsideReportControlView::EnableControl()
{
	LandsideReportType rpType = GetDocument()->GetARCReportManager().GetLandsideReportManager()->GetReportType();
	switch(rpType) 
	{
	case LANDSIDE_VEHICLE_ACTIVITY:
		{
			m_staticInterval.EnableWindow( FALSE );
		//	m_dtctrlInterval.EnableWindow(FALSE);
			//interval for activity should be available
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

	case LANDSIDE_VEHICLE_DELAY:
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
	case LANDSIDE_VEHICLE_TOTALDELAY:
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
	case LANDSIDE_RESOURCE_QUEUELEN:
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
			//m_dtctrlInterval.ShowWindow(FALSE);

		}
		break;
	case LANDSIDE_RESOURCE_THROUGHPUT:
		{
			m_staticInterval.EnableWindow( TRUE );
			m_dtctrlInterval.EnableWindow(TRUE);

			m_radioDetailed.EnableWindow(TRUE);
			m_radioSummary.EnableWindow(TRUE);

			//have no threshold in the report
			m_spinThreshold.EnableWindow(FALSE);
			m_chkThreshold.EnableWindow(FALSE);
			m_edtThreshold.EnableWindow(FALSE);
			m_edtInterval.ShowWindow(FALSE);

			m_staticArea.SetWindowText( "Portals:" );
			m_staticArea.ShowWindow( SW_SHOW );
			m_comboAreaPortal.ShowWindow( SW_SHOW );

			//CPortals portals = ((CTermPlanDoc*)GetDocument())->m_portals;
			CLandsidePortals* pPortals = &(GetDocument()->getARCport()->m_pInputLandside->GetPortals());
			int nCount = pPortals->getCount();
			m_comboAreaPortal.ResetContent();
			for( int i=0; i<nCount; i++ )
			{		
				CLandsidePortal* pPortal = (CLandsidePortal*)pPortals->getLayoutObject(i);
				CString csStr = pPortal->name;

				int nIdx = m_comboAreaPortal.AddString( csStr );
				m_comboAreaPortal.SetItemData( nIdx, i );
			}
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

				
			m_staticArea.ShowWindow( SW_HIDE );
			m_comboAreaPortal.ShowWindow( SW_HIDE );
				

			m_edtInterval.ShowWindow(FALSE);
		}
		break;
	} 
}
void LandsideReportControlView::InitToolbar()
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

int LandsideReportControlView::OnCreate(LPCREATESTRUCT lpCreateStruct)
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
void LandsideReportControlView::ResetAllContent()
{
	CRect rc;
	GetClientRect(&rc);
	SizeDownCtrl( rc.Width(), rc.Height() );


	m_pParameter = GetDocument()->GetARCReportManager().GetLandsideReportManager()->GetParameters();

	InitializeControl();
	LoadData();
	InitializeTree();
	InitializeDefaultTreeData();
}

void LandsideReportControlView::InitializeTree()
{

	CTermPlanDoc *pTermPlanDoc = GetDocument();	
	ASSERT(pTermPlanDoc);
	LandsideReportType rpType = pTermPlanDoc->GetARCReportManager().GetLandsideReportManager()->GetReportType();
	switch(rpType)
	{
	case LANDSIDE_VEHICLE_ACTIVITY:
	case LANDSIDE_VEHICLE_DELAY:
	case LANDSIDE_VEHICLE_TOTALDELAY:
	case LANDSIDE_RESOURCE_QUEUELEN:
	case LANDSIDE_RESOURCE_THROUGHPUT:
		{
			if(GetTreePerformer())
			{
				GetTreePerformer()->LoadData();
			}
		}
		break;
	default:
		{
			//m_treePaxType.DeleteAllItems();
			//m_hRootFlightType = m_treePaxType.InsertItem("Flight Type");
			//m_treePaxType.SetItemData(m_hRootFlightType, NULL);

			//m_hRootFlightType = m_treePaxType.InsertItem("Passenger Type");
			//m_treePaxType.SetItemData(m_hRootFlightType,NULL);
		}
		break;
	}
}


void LandsideReportControlView::InitializeDefaultTreeData()
{
	if(m_pParameter == NULL)
		return;
	CTermPlanDoc *pTermPlanDoc = GetDocument();	
	ASSERT(pTermPlanDoc);
	LandsideReportType rpType = pTermPlanDoc->GetARCReportManager().GetLandsideReportManager()->GetReportType();
	switch(rpType)
	{
	case LANDSIDE_VEHICLE_ACTIVITY:
		break;
	default:
		{
		}
		break;
	}
}

BOOL LandsideReportControlView::SaveData()
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
	LandsideReportType rpType = pTermPlanDoc->GetARCReportManager().GetLandsideReportManager()->GetReportType();
	//if (rpType == LANDSIDE_VEHICLE_DISTANCETRAVEL)
	//{
	//	if (m_nInterval == 0)
	//	{
	//		MessageBox( "Interval distance should not be '0'.", "Error", MB_OK|MB_ICONWARNING );
	//		return FALSE;
	//	}
	//}
	
	m_pParameter->setDistanceInterval(m_nInterval);

	m_pParameter->setInterval( time.asSeconds() );

	if( m_comboAreaPortal.IsWindowEnabled() )
	{
		LandsideReportType rpType = pTermPlanDoc->GetARCReportManager().GetLandsideReportManager()->GetReportType();
		if (rpType == LANDSIDE_RESOURCE_THROUGHPUT)
		{
			LSVehicleThroughputParam* pThroughputPara = (LSVehicleThroughputParam*)m_pParameter;
			int nCurSel = m_comboAreaPortal.GetCurSel();
			if (nCurSel != LB_ERR)
			{
				CLandsidePortals* pPortals = &(GetDocument()->GetInputLandside()->GetPortals());
				if (pPortals)
				{
					CLandsidePortal* pPortal = (CLandsidePortal*)pPortals->getLayoutObject(nCurSel);
					pThroughputPara->SetPortalName(pPortal->name);
					CPoint2008 ptStart;
					CPoint2008 ptEnd;
					int nPtcount = pPortal->m_path.getCount();
					if (nPtcount >= 2)
					{
						ptStart = pPortal->m_path.getPoint(0);
						ptEnd = pPortal->m_path.getPoint(nPtcount - 1);
						pThroughputPara->SetStartPoint(ptStart);
						pThroughputPara->SetEndPoint(ptEnd);
					}
				}
			}
		}
	}
	//summary or detail
	if (m_radioDetailed.GetCheck())
	{
		m_pParameter->setReportType(LandsideReport_Detail);
	}
	else
	{
		m_pParameter->setReportType(LandsideReport_Summary);
	}

	//Vehicle type

	if(GetTreePerformer())
		GetTreePerformer()->SaveData();
	return TRUE;
}
void LandsideReportControlView::OnButtonApply()
{
	if(SaveData() == FALSE)
		return;
	try
	{

		GetDocument()->GetARCReportManager().GetLandsideReportManager()->GenerateReport(GetDocument()->getARCport()->m_pInputLandside);
		GetDocument()->UpdateAllViews(this,AIRSIDEREPORT_SHOWREPORT,NULL);
	}
	catch(StringError* pErr)
	{
		AfxMessageBox("The Landside log file is old, run simulation please!");
		delete pErr;		
	}

}
void LandsideReportControlView::OnButtonCancel()
{

}
void LandsideReportControlView::UpdateControlContent()
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

	if(m_pParameter->getReportType() == LandsideReport_Detail)
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

	if(eIntervalTime == ElapsedTime(0L))
		oleInterval.SetTime(1, 0, 0);
	else
	{
		oleInterval.SetTime(eIntervalTime.GetHour() , eIntervalTime.GetMinute(), 0 );
	}


	//m_dtctrlInterval.SetTime( oleInterval );
	m_oleDTInterval = oleInterval;

	UpdateData(FALSE);
	Invalidate(FALSE);
}

void LandsideReportControlView::OnUpdate(CView* /*pSender*/, LPARAM lHint, CObject* /*pHint*/)
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

LandsideReportTreeBasePerformer* LandsideReportControlView::GetTreePerformer()
{
	CTermPlanDoc *pTermPlanDoc = GetDocument();	
	if(pTermPlanDoc == NULL)
		return NULL;

	if(m_pParameter == NULL)
		return NULL;

	LandsideReportType rpType = pTermPlanDoc->GetARCReportManager().GetLandsideReportManager()->GetReportType();

	switch (rpType)
	{
	case LANDSIDE_VEHICLE_ACTIVITY:
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
		
			m_pTreePerformer = new LandsideReportTreeDefaultPerformer(&m_treePaxType,
				(InputTerminal*)(&pTermPlanDoc->GetTerminal()),
				pTermPlanDoc->GetInputLandside(),
				m_pParameter, 
				rpType);
			return m_pTreePerformer;
		}
		break;
	case LANDSIDE_VEHICLE_DELAY:
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
		
			m_pTreePerformer = new LSReportTreeVehicleDelayPerformer(&m_treePaxType,
				(InputTerminal*)(&pTermPlanDoc->GetTerminal()),
				pTermPlanDoc->GetInputLandside(),
				m_pParameter, 
				rpType);
			return m_pTreePerformer;
		}
		break;
	case LANDSIDE_VEHICLE_TOTALDELAY:
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

			m_pTreePerformer = new LSReportTreeVehicleDelayPerformer(&m_treePaxType,
				(InputTerminal*)(&pTermPlanDoc->GetTerminal()),
				pTermPlanDoc->GetInputLandside(),
				m_pParameter, 
				rpType);
			return m_pTreePerformer;
		}
		break;
	case LANDSIDE_RESOURCE_QUEUELEN:
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
		
			m_pTreePerformer = new LSReportTreeResQueuePerformer(&m_treePaxType,
				(InputTerminal*)(&pTermPlanDoc->GetTerminal()),
				pTermPlanDoc->GetInputLandside(),
				m_pParameter, 
				rpType);
			return m_pTreePerformer;
		}
		break;
	case LANDSIDE_RESOURCE_THROUGHPUT:
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

		m_pTreePerformer = new LSReportTreeResThroughputPerformer(&m_treePaxType,
			(InputTerminal*)(&pTermPlanDoc->GetTerminal()),
			pTermPlanDoc->GetInputLandside(),
			m_pParameter, 
			rpType);
		return m_pTreePerformer;
	}
	break;
	default:
		break;
	}

	return NULL;
}

void LandsideReportControlView::OnToolbarbuttonadd()
{
	CTermPlanDoc *pTermPlanDoc = GetDocument();	
	ASSERT(pTermPlanDoc);
	LandsideReportType rpType = pTermPlanDoc->GetARCReportManager().GetLandsideReportManager()->GetReportType();


	HTREEITEM hSelectItem=m_treePaxType.GetSelectedItem();
	if (hSelectItem == NULL)
		return;

	if(GetTreePerformer())
	{
		GetTreePerformer()->OnToolBarAdd(hSelectItem);
	}
}

void LandsideReportControlView::OnToolbarbuttondel()
{
	CTermPlanDoc *pTermPlanDoc = GetDocument();	
	ASSERT(pTermPlanDoc);
	LandsideReportType rpType = pTermPlanDoc->GetARCReportManager().GetLandsideReportManager()->GetReportType();


	HTREEITEM hSelectItem=m_treePaxType.GetSelectedItem();
	if (hSelectItem == NULL)
		return;

	if(GetTreePerformer())
	{
		GetTreePerformer()->OnToolBarDel(hSelectItem);
	}
}

void LandsideReportControlView::OnUpdateToolBarAdd(CCmdUI *pCmdUI)
{
	CTermPlanDoc *pTermPlanDoc = GetDocument();	
	ASSERT(pTermPlanDoc);
	LandsideReportType rpType = pTermPlanDoc->GetARCReportManager().GetLandsideReportManager()->GetReportType();


	HTREEITEM hSelectItem=m_treePaxType.GetSelectedItem();
	if (hSelectItem == NULL)
		return;

	if(GetTreePerformer())
	{
		GetTreePerformer()->OnUpdateToolBarAdd(pCmdUI);
	}
}

void LandsideReportControlView::OnUpdateToolBarDel(CCmdUI *pCmdUI)
{
	CTermPlanDoc *pTermPlanDoc = GetDocument();	
	ASSERT(pTermPlanDoc);
	LandsideReportType rpType = pTermPlanDoc->GetARCReportManager().GetLandsideReportManager()->GetReportType();


	HTREEITEM hSelectItem=m_treePaxType.GetSelectedItem();
	if (hSelectItem == NULL)
		return;

	if(GetTreePerformer())
	{
		GetTreePerformer()->OnUpdateToolBarDel(pCmdUI);
	}
}




























