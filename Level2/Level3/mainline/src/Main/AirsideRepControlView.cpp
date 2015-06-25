// AirsideRepControlView.cpp : implementation file
//

#include "stdafx.h"
#include "TermPlan.h"
#include "AirsideRepControlView.h"
#include ".\airsiderepcontrolview.h"
#include "../AirsideReport/TypeDefs.h"
#include "../AirsideReport/ReportManager.h"
#include "./DlgSelectAirsideNode.h"
#include "FlightDialog.h"
#include "../AirsideReport/AirsideFlightDelayParam.h"
#include "../AirsideReport/AirsideNodeDelayReportParameter.h"
#include "../Common/UnitsManager.h"
#include "../AirsideReport/FlightOperationalParam.h"
#include "ViewMsg.h"
#include "ResourcePoolDlg.h"
#include "DlgVehicleTypeSelect.h"
#include "../AirsideReport/AirsideVehicleOperParameter.h"
#include "../AirsideReport/AirsideRunwayOperationReportParam.h"
#include "../InputAirside/ALTObject.h"
#include "../InputAirside/Runway.h"
#include "DlgSelectALTObject.h"
#include "DlgSelecteAircraftClassification.h"
#include "..\Common\WinMsg.h"
#include "../AirsideReport/AirsideFlightConflictPara.h"
#include "../AirsideReport/AirsideFlightConflictReport.h"
#include "../AirsideReport/TaxiwayDelayParameters.h"
#include "DlgSelectMulti_ALTObjectList.h"
#include "DlgRunwayExitSelect.h"

#include "../AirsideReport/AirsideFlightBurnParmater.h"
#include "../AirsideReport/FlightActivityParam.h"
#include "../AirsideGUI/DlgTaxiwayFamilySelect.h"
#include "DlgTaxiwayIntersectionNodeSelection.h"
#include "Common/SimAndReportManager.h"
// CAirsideRepControlView
const CString strBtnTxt[] =
{
	"Save Para",
	"Load Para",
	"Save Report",
	"Load Report"
};
IMPLEMENT_DYNCREATE(CAirsideRepControlView, CFormView)

CAirsideRepControlView::CAirsideRepControlView()
	: CFormView(CAirsideRepControlView::IDD)
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

	m_pParameter = NULL;


	m_hRootFlightType = NULL;
	m_hRootNodeToNode = NULL;
	m_hRootNodes = NULL;
	m_hRootRunways = NULL;
	m_hRootClassification = NULL;
	m_pTreePerformer = NULL;
}

CAirsideRepControlView::~CAirsideRepControlView()
{
}

void CAirsideRepControlView::DoDataExchange(CDataExchange* pDX)
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

void CAirsideRepControlView::InitMuiButtom()
{
	m_btnMulti.SetType(CCoolBtn::TY_AIRSIDE) ;
	m_btnMulti.SetOperation(0) ;
	m_btnMulti.SetWindowText(strBtnTxt[0]) ;
}
BEGIN_MESSAGE_MAP(CAirsideRepControlView, CFormView)
	ON_WM_SIZE()
	ON_WM_CREATE()
	ON_COMMAND(ID_TOOLBARBUTTONADD, OnToolbarbuttonadd)
	ON_COMMAND(ID_TOOLBARBUTTONDEL, OnToolbarbuttondel)
	ON_BN_CLICKED(ID_BUTTON_CANCEL, OnButtonCancel)
	ON_BN_CLICKED(IDC_BUTTON_APPLY, OnButtonApply)
	ON_BN_CLICKED(IDC_BUTTON_LOAD_FROM_FILE, OnButtonReportDefault)
	ON_UPDATE_COMMAND_UI(ID_TOOLBARBUTTONADD, OnUpdateToolBarAdd)
	ON_UPDATE_COMMAND_UI(ID_TOOLBARBUTTONDEL, OnUpdateToolBarDel)
	ON_COMMAND(ID_AIRSIDEREPORT_SAVEREPORT,OnSaveReport)
	ON_COMMAND(ID_AIRSIDEREPORT_LOADREPORT,OnLoadReport)
	ON_COMMAND(ID_AIRSIDEREPORT_SAVEPARA, OnReportSavePara)
	ON_COMMAND(ID_AIRSIDEREPORT_LOADPARA, OnReportLoadPara)
END_MESSAGE_MAP()


// CAirsideRepControlView diagnostics

#ifdef _DEBUG
void CAirsideRepControlView::AssertValid() const
{
	CFormView::AssertValid();
}

void CAirsideRepControlView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG
void CAirsideRepControlView::ClickMuiButton()
{
	int type = m_btnMulti.GetOperation() ;
	if (type == 0)
		OnReportSavePara();
	if(type == 1)
		OnReportLoadPara();
	if(type == 2)
		OnSaveReport();
	if(type == 3)
		OnLoadReport();
	
}
// void CAirsideRepControlView::OnGenerateReport()
// {
// 	m_btnMulti.SetWindowText(ExportReport) ;
// 	m_btnMulti.SetOperation(2) ;
// 	CFileDialog dlgFile( FALSE, ".csv",NULL ,OFN_OVERWRITEPROMPT|OFN_HIDEREADONLY,"Report Files (*.csv)|*.csv||" );
// 	CString sZipFileName;
// 	if (dlgFile.DoModal() == IDOK)
// 		sZipFileName = dlgFile.GetPathName();
// 	else
// 		return ;
// 	ArctermFile  _file ;
// 	if(!_file.openFile(sZipFileName,WRITE) )
// 	{
// 		MessageBox(_T("Create File error"),_T("Error"),MB_OK) ;
// 		return  ;
// 	}
// 	GetDocument()->UpdateAllViews(this,VM_GENERATE_REPORT,(CObject*)&_file);
// 	/*CString errormsg ;
// 	if(!GetDocument()->GetARCReportManager().GetAirsideReportManager()->GenerateCvsFileFormList(_file,errormsg))*/
//       
// }

void CAirsideRepControlView::OnReportSavePara()
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
		ArctermFile _file;
		if(!_file.openFile(strFileName,WRITE))
		{
			MessageBox(_T("Create File error"),_T("Error"),MB_OK);
			return  ;
		}
		BOOL res = pTermPlanDoc->GetARCReportManager().GetAirsideReportManager()->ExportParaToFile(_file);
		if (res)
		{
			MessageBox(_T("Save Report Parameter Successfully"),NULL,MB_OK) ;
		}
		_file.closeIn();
	}
}

void CAirsideRepControlView::OnReportLoadPara()
{
	CFileDialog loaddlg(TRUE,NULL,NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		"Report Parameter(*.par)|*.par||",this, 0, FALSE  );
	if( loaddlg.DoModal() == IDOK )
	{
		CString strFileName = loaddlg.GetPathName();
		ArctermFile _file;
		CTermPlanDoc *pTermPlanDoc = GetDocument();	
		ASSERT(pTermPlanDoc);
		if(!_file.openFile(strFileName,READ) )
		{
			MessageBox(_T("Read File error"),_T("Error"),MB_OK);
			return  ;
		}
		BOOL res = pTermPlanDoc->GetARCReportManager().GetAirsideReportManager()->ImportParaFromFile(_file);
		if (res)
		{
			m_pParameter = pTermPlanDoc->GetARCReportManager().GetAirsideReportManager()->GetParameters();
			MessageBox(_T("Load Report Parameter Successfully"),NULL,MB_OK) ;
			UpdateControlContent();
			InitializeTree();
			InitializeDefaultTreeData();
		}
		_file.closeIn();
	}
}

void CAirsideRepControlView::OnSaveReport()
{
	m_btnMulti.SetWindowText(strBtnTxt[2]) ;
		m_btnMulti.SetOperation(1) ;
	CFileDialog dlgFile( FALSE, ".rep",NULL ,OFN_OVERWRITEPROMPT|OFN_HIDEREADONLY,"Report Files (*.rep)|*.rep||", NULL, 0, FALSE  );
	CString sZipFileName;
	if (dlgFile.DoModal() == IDOK)
		sZipFileName = dlgFile.GetPathName();
	else
		return ;
	
	//get report type
	CTermPlanDoc *pTermPlanDoc = GetDocument();	
	ASSERT(pTermPlanDoc);
	reportType rpType = pTermPlanDoc->GetARCReportManager().GetAirsideReportManager()->GetReportType();
	if(rpType ==  Airside_RunwayOperaitons ||\
		rpType == Airside_FlightConflict ||\
		rpType == Airside_RunwayDelay ||\
		rpType == Airside_RunwayUtilization/*||
		rpType == Airside_IntersectionOperation*/)
	{
		CAirsideBaseReport *pBaseeReport = pTermPlanDoc->GetARCReportManager().GetAirsideReportManager()->GetReport();
		if(pBaseeReport)
		{
			CString strReportFile = pBaseeReport->GetReportFilePath();
			if(FileManager::CopyFile(strReportFile,sZipFileName,TRUE))
			{
				MessageBox(_T("Export Report Successfully"),NULL,MB_OK) ;
				return ;
			}
		}
		return;
	}
	


	ArctermFile  _file ;
	if(!_file.openFile(sZipFileName,WRITE) )
	{
		MessageBox(_T("Create File error"),_T("Error"),MB_OK) ;
		return  ;
	}
	CString errormsg ;
	if(!GetDocument()->GetARCReportManager().GetAirsideReportManager()->ExportReportToFile(_file,errormsg))
	{
		MessageBox(errormsg,_T("Error"),MB_OK) ;
		return  ;
	}else
	{
		_file.endFile() ;
		MessageBox(_T("Export Report Successfully"),NULL,MB_OK) ;
		return ;
	}

}
void CAirsideRepControlView::OnButtonReportDefault()
{
	CTermPlanDoc *pTermPlanDoc = GetDocument();	
	ASSERT(pTermPlanDoc);
	int iCurrentSimIdx = GetDocument()->GetTerminal().getCurrentSimResult();
	if( iCurrentSimIdx < 0 )
	{
		AfxMessageBox("No Valid SimResult!", MB_OK|MB_ICONERROR);
		return;
	}

	pTermPlanDoc->GetAirsideSimLogs().GetSimReportManager()->SetCurrentSimResult( iCurrentSimIdx );

	reportType rpType = pTermPlanDoc->GetARCReportManager().GetAirsideReportManager()->GetReportType();
	if(rpType ==  Airside_RunwayOperaitons )
	{
		CAirsideBaseReport *pBaseeReport = pTermPlanDoc->GetARCReportManager().GetAirsideReportManager()->GetReport();
		if(pBaseeReport)
		{
			pBaseeReport->LoadReportData();
		}	
		GetDocument()->UpdateAllViews(this,AIRSIDEREPORT_SHOWREPORT,NULL);
		return;
	}
	else if(rpType == Airside_RunwayUtilization)
	{
		CAirsideBaseReport *pBaseeReport = pTermPlanDoc->GetARCReportManager().GetAirsideReportManager()->GetReport();
		if(pBaseeReport)
		{
			pBaseeReport->LoadReportData();
		}	
		GetDocument()->UpdateAllViews(this,AIRSIDEREPORT_SHOWREPORT,NULL);
		return;
	}
	else if (rpType == Airside_RunwayCrossings)
	{
		CAirsideBaseReport *pBaseeReport = pTermPlanDoc->GetARCReportManager().GetAirsideReportManager()->GetReport();
		if(pBaseeReport)
		{
			pBaseeReport->LoadReportData();
		}	
		GetDocument()->UpdateAllViews(this,AIRSIDEREPORT_SHOWREPORT,NULL);
		return;
	}
	else if (rpType == Airside_StandOperations)
	{
		CAirsideBaseReport *pBaseeReport = pTermPlanDoc->GetARCReportManager().GetAirsideReportManager()->GetReport();
		if(pBaseeReport)
		{
			pBaseeReport->LoadReportData();
		}	
		GetDocument()->UpdateAllViews(this,AIRSIDEREPORT_SHOWREPORT,NULL);
		return;
	}
	else if (rpType == Airside_TakeoffProcess)
	{
		CAirsideBaseReport *pBaseeReport = pTermPlanDoc->GetARCReportManager().GetAirsideReportManager()->GetReport();
		if(pBaseeReport)
		{
			pBaseeReport->LoadReportData();
		}	
		GetDocument()->UpdateAllViews(this,AIRSIDEREPORT_SHOWREPORT,NULL);
		return;
	}
	else if(rpType == Airside_FlightConflict)
	{
		CAirsideBaseReport *pBaseeReport = pTermPlanDoc->GetARCReportManager().GetAirsideReportManager()->GetReport();
		if(pBaseeReport)
		{
			pBaseeReport->LoadReportData();
		}	
		GetDocument()->UpdateAllViews(this,AIRSIDEREPORT_SHOWREPORT,NULL);
		return;
	}
	else if(rpType == Airside_RunwayDelay)
	{
		CAirsideBaseReport *pBaseeReport = pTermPlanDoc->GetARCReportManager().GetAirsideReportManager()->GetReport();
		if(pBaseeReport)
		{
			pBaseeReport->LoadReportData();
		}	
		GetDocument()->UpdateAllViews(this,AIRSIDEREPORT_SHOWREPORT,NULL);
		return;
	}else if(rpType == Airside_FlightFuelBurning ||
		rpType == Airside_VehicleOperation || 
		rpType == Airside_TaxiwayDelay || 
		rpType == Airside_RunwayExit ||
		rpType == Airside_IntersectionOperation||
		rpType == Airside_TaxiwayUtilization ||
		rpType == Airside_ControllerWorkload)
	{
		CAirsideBaseReport *pBaseeReport = pTermPlanDoc->GetARCReportManager().GetAirsideReportManager()->GetReport();
		if(pBaseeReport)
		{
			pBaseeReport->LoadReportData();
			GetDocument()->GetARCReportManager().GetAirsideReportManager()->RefreshReport(0);
			GetDocument()->UpdateAllViews(this,AIRSIDEREPORT_SHOWREPORT,NULL);
		}	
		return;
	}
}
void CAirsideRepControlView::OnLoadReport()
{
	m_btnMulti.SetWindowText(strBtnTxt[3]) ;
	m_btnMulti.SetOperation(0) ;
	CFileDialog dlgFile( TRUE, ".rep",NULL ,OFN_OVERWRITEPROMPT|OFN_HIDEREADONLY,"Report Files (*.rep)|*.rep||", NULL, 0, FALSE  );
	CString sZipFileName;
	if (dlgFile.DoModal() == IDOK)
		sZipFileName = dlgFile.GetPathName();
	else
		return ;


	CTermPlanDoc *pTermPlanDoc = GetDocument();	
	ASSERT(pTermPlanDoc);
	reportType rpType = pTermPlanDoc->GetARCReportManager().GetAirsideReportManager()->GetReportType();
	if(rpType ==  Airside_RunwayOperaitons \
		|| rpType == Airside_FlightConflict \
		|| rpType == Airside_RunwayDelay \
		|| rpType == Airside_RunwayUtilization/*\
		|| rpType == Airside_IntersectionOperation*/)
	{
		CAirsideBaseReport *pBaseeReport = pTermPlanDoc->GetARCReportManager().GetAirsideReportManager()->GetReport();
		if(pBaseeReport)
		{
			pBaseeReport->LoadReportData(sZipFileName);
			UpdateControlContent();
		}	
		GetDocument()->UpdateAllViews(this,AIRSIDEREPORT_SHOWREPORT,NULL);
		return;
	}


	ArctermFile  _file ;
	if(!_file.openFile(sZipFileName,READ) )
	{
		MessageBox(_T("Read File error"),_T("Error"),MB_OK) ;
		return  ;
	}

	CString errormsg ;
	int nProjID = ALTAirport::GetProjectIDByName(((CTermPlanDoc *)GetDocument())->m_ProjInfo.name);
	if(!GetDocument()->GetARCReportManager().GetAirsideReportManager()->ImportReportFromFile(_file,errormsg,nProjID))
	{
		MessageBox(errormsg,_T("Error"),MB_OK) ;
		return  ;
	}else
	{
		GetDocument()->GetARCReportManager().GetAirsideReportManager()->RefreshReport(0);
		GetDocument()->UpdateAllViews(this,AIRSIDEREPORT_SHOWREPORT,NULL);
		ResetAllContent() ;
	}
	UpdateControlContent();
}
// CAirsideRepControlView message handlers

void CAirsideRepControlView::OnSize(UINT nType, int cx, int cy)
{
	CFormView::OnSize(nType, cx, cy);
	DoSizeWindow(cx,cy);

	// TODO: Add your message handler code here
}
void CAirsideRepControlView::DoSizeWindow( int cx, int cy )
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
void CAirsideRepControlView::SizeDownCtrl(int cx, int cy )
{
	//m_enumReportType = ((CTermPlanDoc *)GetDocument())->m_reportType;

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
void CAirsideRepControlView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	InitToolbar();
	InitializeControl();
	ShowOrHideControls();

    InitMuiButtom();
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


	LoadData();
	reportType rpType = GetDocument()->GetARCReportManager().GetAirsideReportManager()->GetReportType();
	if (rpType == Airside_RunwayDelay && m_pParameter->getReportType() == ASReportType_Detail)
	{
		COleDateTime oleThreshold;
		oleThreshold.SetTime( 0, 1, 0 );
		m_dtctrlInterval.SetTime( oleThreshold );
	}
}
void CAirsideRepControlView::InitializeControl()
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
void CAirsideRepControlView::ShowOrHideControls()
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
	m_ProToolBar.GetToolBarCtrl().ShowWindow( SW_HIDE );
	m_ProToolBar.GetToolBarCtrl().ShowWindow( SW_HIDE );

	reportType rpType = GetDocument()->GetARCReportManager().GetAirsideReportManager()->GetReportType();
	if (rpType == Airside_StandOperations)
	{
		m_staPax.SetWindowText( "Stand" );
	}
	else if(rpType == Airside_ControllerWorkload)
	{
		m_staPax.SetWindowText( "Sectors/Areas/Weights" );
	}
	else
	{
		m_staPax.SetWindowText( "Flight Type" );
	}
	

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
void CAirsideRepControlView::EnableControl()
{
	reportType rpType = GetDocument()->GetARCReportManager().GetAirsideReportManager()->GetReportType();
	switch(rpType) {
	case Airside_DistanceTravel:
		{
			m_staticInterval.EnableWindow( FALSE );
			m_dtctrlInterval.EnableWindow(FALSE);
		}
		break;
	case Airside_AircraftOperation:
		{
			m_radioDetailed.EnableWindow(FALSE);
			m_radioSummary.EnableWindow(FALSE);
			m_staticType.EnableWindow(FALSE);

			m_spinThreshold.EnableWindow(FALSE);
			m_chkThreshold.EnableWindow(FALSE);
			m_edtThreshold.EnableWindow(FALSE);

			m_staticInterval.EnableWindow( TRUE );
			m_dtctrlInterval.EnableWindow(TRUE);
		}
		break;
	case Airside_FlightActivity:
		{
			m_radioDetailed.EnableWindow(TRUE);
			m_radioSummary.EnableWindow(TRUE);
			m_staticType.EnableWindow(FALSE);

			m_spinThreshold.EnableWindow(FALSE);
			m_chkThreshold.EnableWindow(FALSE);
			m_edtThreshold.EnableWindow(FALSE);

			m_staticInterval.EnableWindow( TRUE );
			m_dtctrlInterval.EnableWindow(TRUE);
		}
		break;
	case Airside_AircraftOperational:
		{
			m_radioDetailed.EnableWindow(FALSE);
			m_radioSummary.EnableWindow(FALSE);
			m_staticType.EnableWindow(FALSE);

			//m_staticInterval.EnableWindow( FALSE );
			//m_dtctrlInterval.EnableWindow(FALSE);
			m_staticInterval.EnableWindow( TRUE );
			m_dtctrlInterval.EnableWindow(TRUE);
			m_staticThreshold.EnableWindow(TRUE);
			m_spinThreshold.EnableWindow(TRUE);
			m_chkThreshold.EnableWindow(TRUE);
			m_edtThreshold.EnableWindow(TRUE);
		}
		break;
	case Airside_RunwayOperaitons:
	case Airside_RunwayUtilization:
	case Airside_RunwayCrossings:
	case Airside_StandOperations:
	case Airside_RunwayDelay:
	case Airside_ControllerWorkload:
	case Airside_TakeoffProcess:
		{
			m_radioDetailed.EnableWindow(TRUE);
			m_radioSummary.EnableWindow(TRUE);
			m_staticType.EnableWindow(TRUE);

			m_spinThreshold.EnableWindow(FALSE);
			m_chkThreshold.EnableWindow(FALSE);
			m_edtThreshold.EnableWindow(FALSE);

			m_staticInterval.EnableWindow( TRUE );
			m_dtctrlInterval.EnableWindow(TRUE);
			m_btLoad.ShowWindow(SW_SHOW);
		}
		break;
	case Airside_FlightConflict:
		{
			m_radioDetailed.EnableWindow(TRUE);
			m_radioSummary.EnableWindow(TRUE);
			m_staticType.EnableWindow(TRUE);

			m_spinThreshold.EnableWindow(FALSE);
			m_chkThreshold.EnableWindow(FALSE);
			m_edtThreshold.EnableWindow(FALSE);

			m_staticInterval.EnableWindow( TRUE );
			m_dtctrlInterval.EnableWindow(TRUE);
			m_btLoad.ShowWindow(SW_SHOW);
		}
		break;
	case Airside_IntersectionOperation:
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
			m_staPax.SetWindowText(_T("Intersection"));
		}
		break;
	case Airside_TaxiwayUtilization:
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
			m_staPax.SetWindowText(_T("Taxiway"));
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
		}
		break;
	} 
}
void CAirsideRepControlView::InitToolbar()
{
	CRect rc;
	m_toolbarcontenter1.GetWindowRect( &rc );
	ScreenToClient(&rc);
	m_PasToolBar.MoveWindow(rc.left,rc.top,50,rc.Height());
	m_PasToolBar.ShowWindow(SW_SHOW);
	m_toolbarcontenter1.ShowWindow(SW_HIDE);
	m_PasToolBar.GetToolBarCtrl().HideButton( ID_TOOLBARBUTTONEDIT);

	m_toolbarcontenter2.GetWindowRect( &rc );
	ScreenToClient(&rc);
	m_ProToolBar.MoveWindow(rc.left,rc.top,50,rc.Height());
	m_ProToolBar.ShowWindow(SW_SHOW);
	m_toolbarcontenter2.ShowWindow(SW_HIDE);
	m_ProToolBar.GetToolBarCtrl().HideButton( ID_PEOPLEMOVER_CHANGE);
}

int CAirsideRepControlView::OnCreate(LPCREATESTRUCT lpCreateStruct)
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
	if (!m_ProToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_ProToolBar.LoadToolBar(IDR_PEOPLEMOVEBAR))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}
	return 0;
}

void CAirsideRepControlView::ResetAllContent()
{	
	CRect rc;
	GetClientRect(&rc);
	SizeDownCtrl( rc.Width(), rc.Height() );

	m_pParameter = GetDocument()->GetARCReportManager().GetAirsideReportManager()->GetParameters();

	InitializeControl();
	InitializeTree();
	InitializeDefaultTreeData();
	LoadData();
}
void CAirsideRepControlView::InitializeTree()
{
	/************************************************************************
	Airside_Legacy			= 1,
	Airside_DistanceTravel	= 2,
	Airside_Duration		= 3,
	Airside_FlightDelay		= 4,
	Airside_FlightActivity		= 5,
	Airside_AircraftOperation = 6,
	Airside_NodeDelay = 7,                                                                     
	/************************************************************************/
	CTermPlanDoc *pTermPlanDoc = GetDocument();	
	ASSERT(pTermPlanDoc);
	reportType rpType = pTermPlanDoc->GetARCReportManager().GetAirsideReportManager()->GetReportType();
	switch(rpType)
	{
	case Airside_DistanceTravel:
	case Airside_Duration:
	case Airside_AircraftOperation:
		{
			m_treePaxType.DeleteAllItems();
            COOLTREE_NODE_INFO cni;
            CCoolTree::InitNodeInfo(this,cni);
			m_hRootFlightType = m_treePaxType.InsertItem("Flight Type", cni, FALSE);
			m_treePaxType.SetItemData(m_hRootFlightType,(DWORD_PTR)new repControlTreeNodeData(repControlTreeNodeType_FltTypeRoot));

		}
		break;

	case Airside_FlightDelay:
		{
            COOLTREE_NODE_INFO cni;
            CCoolTree::InitNodeInfo(this,cni);
			m_treePaxType.DeleteAllItems();
			m_hRootFlightType =m_treePaxType.InsertItem("Flight Type", cni, FALSE);
			m_treePaxType.SetItemData(m_hRootFlightType,(DWORD_PTR)new repControlTreeNodeData(repControlTreeNodeType_FltTypeRoot));

			m_hRootNodeToNode=m_treePaxType.InsertItem("Node To Node", cni, FALSE);
			m_treePaxType.SetItemData(m_hRootNodeToNode,(DWORD_PTR)new repControlTreeNodeData(repControlTreeNodeType_NodeFromToRoot));
		
			HTREEITEM hItemFrom=m_treePaxType.InsertItem("From", cni, FALSE, FALSE, m_hRootNodeToNode);
			m_treePaxType.SetItemData(hItemFrom,(DWORD_PTR)new repControlTreeNodeData(repControlTreeNodeType_FromRoot));
			
			HTREEITEM hItemTo=m_treePaxType.InsertItem("To", cni, FALSE, FALSE, m_hRootNodeToNode);
			m_treePaxType.SetItemData(hItemTo,(DWORD_PTR)new repControlTreeNodeData(repControlTreeNodeType_ToRoot));
            
            cni.nt = NT_CHECKBOX;
            HTREEITEM hRunsRoot = m_treePaxType.InsertItem("Multi Runs", cni, m_pParameter->GetEnableMultiRun(), FALSE);
            m_treePaxType.SetItemData(hRunsRoot, (DWORD_PTR)new repControlTreeNodeData(repControlTreeNodeType_MultiRunRoot));

            std::vector<int> vMultiRun;
            m_pParameter->GetReportRuns(vMultiRun);
            CSimAndReportManager *pSimAndReportManager = ((CTermPlanDoc *)GetDocument())->GetTerminal().GetSimReportManager();
            int nSimCount = pSimAndReportManager->getSubSimResultCout();
            for (int nSim =0; nSim < nSimCount; ++nSim )
            {
                //CSimItem *pSimItem = pSimAndReportManager->getSimItem(nSim);
                //CString strSimName = pSimItem->getSubSimName();
                CString strSimName;
                strSimName.Format(_T("RUN %d"),nSim+1);
                HTREEITEM hSubSimItem = m_treePaxType.InsertItem(strSimName, cni, FALSE, FALSE, hRunsRoot);
                repControlTreeNodeData* pNodeData = new repControlTreeNodeData(repControlTreeNodeType_Runs);
                pNodeData->m_Data = (DWORD)nSim;
                m_treePaxType.SetItemData(hSubSimItem, (DWORD)pNodeData);

                if(std::find(vMultiRun.begin(),vMultiRun.end(), nSim) != vMultiRun.end())
                {
                    m_treePaxType.SetCheckStatus(hSubSimItem,TRUE);
                }
            }
            m_treePaxType.Expand(hRunsRoot, TVE_EXPAND);
		}
		break;
	case Airside_NodeDelay:
        {
            COOLTREE_NODE_INFO cni;
            CCoolTree::InitNodeInfo(this,cni);
			m_treePaxType.DeleteAllItems();
			m_hRootFlightType=m_treePaxType.InsertItem("Flight Type", cni, FALSE);
			m_treePaxType.SetItemData(m_hRootFlightType,(DWORD_PTR)new repControlTreeNodeData(repControlTreeNodeType_FltTypeRoot));

			m_hRootNodes=m_treePaxType.InsertItem("Nodes", cni, FALSE);
			m_treePaxType.SetItemData(m_hRootNodes,(DWORD_PTR)new repControlTreeNodeData(repControlTreeNodeType_NodesRoot));
		}
		break;
	case Airside_FlightActivity:
        {
            COOLTREE_NODE_INFO cni;
            CCoolTree::InitNodeInfo(this,cni);
			m_treePaxType.DeleteAllItems();
			m_hRootFlightType=m_treePaxType.InsertItem("Flight Type", cni, FALSE);
			m_treePaxType.SetItemData(m_hRootFlightType,(DWORD_PTR)new repControlTreeNodeData(repControlTreeNodeType_FltTypeRoot));
		}
		break;
	case Airside_AircraftOperational:
        {
            COOLTREE_NODE_INFO cni;
            CCoolTree::InitNodeInfo(this,cni);
			m_treePaxType.DeleteAllItems();
			m_hRootFlightType=m_treePaxType.InsertItem("Flight Type", cni, FALSE);
			m_treePaxType.SetItemData(m_hRootFlightType,(DWORD_PTR)new repControlTreeNodeData(repControlTreeNodeType_FltTypeRoot));

			HTREEITEM hItemFromTo=m_treePaxType.InsertItem("Locations", cni, FALSE);
			m_treePaxType.SetItemData(hItemFromTo,(DWORD_PTR)new repControlTreeNodeData(repControlTreeNodeType_NodeFromToRoot));

			HTREEITEM hItemFrom=m_treePaxType.InsertItem("From", cni, FALSE, FALSE, hItemFromTo);
			m_treePaxType.SetItemData(hItemFrom,(DWORD_PTR)new repControlTreeNodeData(repControlTreeNodeType_FromRoot));

			HTREEITEM hItemTo=m_treePaxType.InsertItem("To", cni, FALSE, FALSE, hItemFromTo);
			m_treePaxType.SetItemData(hItemTo,(DWORD_PTR)new repControlTreeNodeData(repControlTreeNodeType_ToRoot));
		}
		break;
	case Airside_VehicleOperation:
        {
            COOLTREE_NODE_INFO cni;
            CCoolTree::InitNodeInfo(this,cni);
			m_treePaxType.DeleteAllItems();
			m_hRootFlightType = m_treePaxType.InsertItem("Flight Type", cni, FALSE);
			m_treePaxType.SetItemData(m_hRootFlightType,(DWORD_PTR)new repControlTreeNodeData(repControlTreeNodeType_FltTypeRoot));

			HTREEITEM hItemVehicleType=m_treePaxType.InsertItem("Vehicle Type", cni, FALSE);
			m_treePaxType.SetItemData(hItemVehicleType,(DWORD_PTR)new repControlTreeNodeData(repControlTreeNodeType_VehicleType));

			HTREEITEM hItemPool=m_treePaxType.InsertItem("Pool", cni, FALSE);
			m_treePaxType.SetItemData(hItemPool,(DWORD_PTR)new repControlTreeNodeData(repControlTreeNodeType_Pool));
		}
		break ;
	case Airside_RunwayOperaitons:
        {
            COOLTREE_NODE_INFO cni;
            CCoolTree::InitNodeInfo(this,cni);
			m_treePaxType.DeleteAllItems();
			//runway mark root
			m_hRootRunways = m_treePaxType.InsertItem(_T("Runways"), cni, FALSE);
			m_treePaxType.SetItemData(m_hRootRunways,(DWORD_PTR)new repControlTreeNodeData(repControlTreeNodeType_RunwayMarkRoot));

			m_hRootFlightType=m_treePaxType.InsertItem("Flight Type", cni, FALSE);
			m_treePaxType.SetItemData(m_hRootFlightType,(DWORD_PTR)new repControlTreeNodeData(repControlTreeNodeType_FltTypeRoot));
		
			m_hRootClassification=m_treePaxType.InsertItem("Classification", cni, FALSE);
			m_treePaxType.SetItemData(m_hRootClassification,(DWORD_PTR)new repControlTreeNodeData(repControlTreeNodeType_ClassificationRoot));
		}
		break;
	case Airside_FlightConflict:
        {
            COOLTREE_NODE_INFO cni;
            CCoolTree::InitNodeInfo(this,cni);
			m_treePaxType.DeleteAllItems();
			m_hRootReportingArea = m_treePaxType.InsertItem("Areas/Sectors", cni, FALSE);
			m_treePaxType.SetItemData(m_hRootReportingArea,(DWORD_PTR)new repControlTreeNodeData(repControlTreeNodeType_ReportingAreaRoot));
			//m_hRootSector = m_treePaxType.InsertItem("Sectors:");
			//m_treePaxType.SetItemData(m_hRootSector,(DWORD_PTR)new repControlTreeNodeData(repControlTreeNodeType_SectorRoot));
		}
		break;
	case Airside_TaxiwayDelay:
        {
            COOLTREE_NODE_INFO cni;
            CCoolTree::InitNodeInfo(this,cni);
			m_treePaxType.DeleteAllItems();
			m_hRootReportingArea = m_treePaxType.InsertItem("Taxiway", cni, FALSE);
			m_treePaxType.SetItemData(m_hRootReportingArea,(DWORD_PTR)new repControlTreeNodeData(repControlTreeNodeType_Taxiways));
		}
		break ;
	case Airside_RunwayExit:
        {
            COOLTREE_NODE_INFO cni;
            CCoolTree::InitNodeInfo(this,cni);
			m_treePaxType.DeleteAllItems() ;
			m_hRootReportingArea = m_treePaxType.InsertItem(_T("Runways"), cni, FALSE);
			m_treePaxType.SetItemData(m_hRootReportingArea,(DWORD_PTR)new repControlTreeNodeData(repControlTreeNodeType_RunwayExits)) ;
		}
		break ;
	case Airside_FlightFuelBurning:
        {
            COOLTREE_NODE_INFO cni;
            CCoolTree::InitNodeInfo(this,cni);
			m_treePaxType.DeleteAllItems();
			m_hRootFlightType=m_treePaxType.InsertItem("Flight Type", cni, FALSE);
			m_treePaxType.SetItemData(m_hRootFlightType,(DWORD_PTR)new repControlTreeNodeData(repControlTreeNodeType_FltTypeRoot));

			m_hRootNodes=m_treePaxType.InsertItem("From/To Nodes", cni, FALSE);
			m_treePaxType.SetItemData(m_hRootNodes,(DWORD_PTR)new repControlTreeNodeData(repControlTreeNodeType_FromToNode));

			HTREEITEM HFromNodeItem = m_treePaxType.InsertItem(_T("From"), cni, FALSE, FALSE, m_hRootNodes);
			repControlTreeNodeData* pNode = new repControlTreeNodeData(repControlTreeNodeType_FromRoot) ;
			m_treePaxType.SetItemData(HFromNodeItem,(DWORD_PTR)pNode) ;

			HTREEITEM HToNodeItem = m_treePaxType.InsertItem(_T("To"), cni, FALSE, FALSE, m_hRootNodes);
			pNode = new repControlTreeNodeData(repControlTreeNodeType_ToRoot) ;
			m_treePaxType.SetItemData(HToNodeItem,(DWORD_PTR)pNode) ;
		}
		break ;
	default:
		{
			m_treePaxType.DeleteAllItems();
		}
		break;
	}
}


void CAirsideRepControlView::OnToolbarbuttonadd()
{
	CTermPlanDoc *pTermPlanDoc = GetDocument();	
	ASSERT(pTermPlanDoc);
	reportType rpType = pTermPlanDoc->GetARCReportManager().GetAirsideReportManager()->GetReportType();


	HTREEITEM hSelectItem=m_treePaxType.GetSelectedItem();
	if (hSelectItem == NULL && rpType != Airside_StandOperations)
		return;


	if(rpType== Airside_RunwayUtilization || 
		rpType == Airside_RunwayDelay || 
		rpType == Airside_RunwayCrossings ||
		rpType == Airside_IntersectionOperation ||
		rpType == Airside_TaxiwayUtilization||
		rpType == Airside_StandOperations ||
		rpType == Airside_ControllerWorkload ||
		rpType == Airside_TakeoffProcess
		)//tree performer handle the message
	{
		if(GetTreePerformer())
			GetTreePerformer()->OnToolBarAdd(hSelectItem);
		return;
	}

	repControlTreeNodeData *pData = reinterpret_cast<repControlTreeNodeData *>(m_treePaxType.GetItemData(hSelectItem));
	if (pData == NULL)
		return;

	if (pData->nodeType == repControlTreeNodeType_FltTypeRoot) // flight type
	{
		CFlightDialog dlg( this );
		if( dlg.DoModal() == IDOK )
		{
			FlightConstraint newFltConstr = dlg.GetFlightSelection();

			HTREEITEM hChildItem =  m_treePaxType.GetChildItem(hSelectItem);
			while (hChildItem != NULL)
			{
				repControlTreeNodeData *pData = reinterpret_cast<repControlTreeNodeData *>(m_treePaxType.GetItemData(hChildItem));
				ASSERT(pData != NULL);

				CString strOldFlightType;
				pData->fltCons.screenPrint(strOldFlightType.GetBuffer(1024));
				strOldFlightType.ReleaseBuffer();

				CString strNewFlightType;
				newFltConstr.screenPrint(strNewFlightType.GetBuffer(1024));
				strNewFlightType.ReleaseBuffer();

				 //if (pData->fltCons.isEqual(&newFltConstr))
				if (!strOldFlightType.CompareNoCase(strNewFlightType))
				{	
					CString csMsg;
					csMsg.Format( "Flight Type : %s exists in the list", strNewFlightType);
					MessageBox( csMsg, "Error", MB_OK|MB_ICONWARNING );
					return;
				}				 
				
				hChildItem =  m_treePaxType.GetNextSiblingItem(hChildItem);
			}
			
			CString szPaxType;
			newFltConstr.screenPrint( szPaxType, 0, 128 );

            COOLTREE_NODE_INFO cni;
            CCoolTree::InitNodeInfo(this,cni);
			HTREEITEM hNewItem = m_treePaxType.InsertItem(szPaxType, cni, FALSE, FALSE, hSelectItem, TVI_LAST);
			m_treePaxType.SetItemData(hNewItem,(DWORD_PTR)new repControlTreeNodeData(repControlTreeNodeType_FltType,newFltConstr));
		}

	}
	else if(pData->nodeType == repControlTreeNodeType_FromRoot||
			pData->nodeType == repControlTreeNodeType_ToRoot  ||
			pData->nodeType == repControlTreeNodeType_NodesRoot)
	{
		CDlgSelectAirsideNode dlg(GetDocument()->GetInputAirside().GetProjectID(GetDocument()->m_ProjInfo.name),this);
		if (dlg.DoModal() == IDOK)
		{
			CAirsideReportNode node = dlg.GetSelectNode();
			HTREEITEM hChildItem =  m_treePaxType.GetChildItem(hSelectItem);
			while (hChildItem != NULL)
			{
				repControlTreeNodeData *pData = reinterpret_cast<repControlTreeNodeData *>(m_treePaxType.GetItemData(hChildItem));
				ASSERT(pData != NULL);
				if (pData->nodeObject.EqualTo(node))
				{
					CString csMsg;
					csMsg.Format( "Airside Node : %s exists in the list", node.GetNodeName());
					MessageBox( csMsg, "Error", MB_OK|MB_ICONWARNING );
					return;
				}
				hChildItem =  m_treePaxType.GetNextSiblingItem(hChildItem);
			}
		

            COOLTREE_NODE_INFO cni;
            CCoolTree::InitNodeInfo(this,cni);
			if (pData->nodeType == repControlTreeNodeType_FromRoot)
			{	
				HTREEITEM hChildItem = m_treePaxType.GetChildItem(hSelectItem) ; 
				if(hChildItem != NULL)
					m_treePaxType.DeleteItem(hChildItem);
				HTREEITEM hNewItem = m_treePaxType.InsertItem(node.GetNodeName(), cni, FALSE, FALSE, hSelectItem, TVI_LAST);
				m_treePaxType.SetItemData(hNewItem,(DWORD_PTR)new repControlTreeNodeData(repControlTreeNodeType_FromNode,node));
			}
			else if (pData->nodeType == repControlTreeNodeType_ToRoot)
			{	
				HTREEITEM hChildItem = m_treePaxType.GetChildItem(hSelectItem) ; 
				if(hChildItem != NULL)
					m_treePaxType.DeleteItem(hChildItem);
				HTREEITEM hNewItem = m_treePaxType.InsertItem(node.GetNodeName(), cni, FALSE, FALSE, hSelectItem, TVI_LAST);
				m_treePaxType.SetItemData(hNewItem,(DWORD_PTR)new repControlTreeNodeData(repControlTreeNodeType_ToNode,node));
			}
			else if (pData->nodeType == repControlTreeNodeType_NodesRoot)
			{	
				HTREEITEM hNewItem = m_treePaxType.InsertItem(node.GetNodeName(), cni, FALSE, FALSE, hSelectItem, TVI_LAST);
				m_treePaxType.SetItemData(hNewItem,(DWORD_PTR)new repControlTreeNodeData(repControlTreeNodeType_Node,node));
			}

			m_treePaxType.Expand(hSelectItem,TVE_EXPAND);
		}
	}
	else if (pData->nodeType == repControlTreeNodeType_VehicleType)
	{
		CDlgVehicleTypeSelect dlgSelect ;
		if(dlgSelect.DoModal() == IDOK)
		{
			CString selType =  dlgSelect.GetSelectedTy() ;

			HTREEITEM SubItem = m_treePaxType.GetChildItem(hSelectItem) ;
			HTREEITEM DeletItem = NULL ;
			while(SubItem)
			{
				if(strcmp(selType,m_treePaxType.GetItemText(SubItem)) == 0)
				{
					CString errormsg ;
					errormsg.Format(_T("%s has selected !"),selType) ;
					MessageBox(errormsg,_T("Warning"),MB_OK) ;
					return ;
				}
				if(m_treePaxType.GetItemText(SubItem).CompareNoCase(_T("All Type")) == 0)
					DeletItem = SubItem ;
				SubItem = m_treePaxType.GetNextSiblingItem(SubItem) ;
			}
			if(DeletItem)
				m_treePaxType.DeleteItem(DeletItem) ;
            COOLTREE_NODE_INFO cni;
            CCoolTree::InitNodeInfo(this,cni);
			HTREEITEM newItem = m_treePaxType.InsertItem(selType, cni, FALSE, FALSE, hSelectItem);
			m_treePaxType.SetItemData(newItem,(DWORD_PTR)new repControlTreeNodeData(repControlTreeNodeType_VehicleTypeNode));
		}
	}
	else if (pData->nodeType == repControlTreeNodeType_Pool)
	{
		int nProjID = ALTAirport::GetProjectIDByName(((CTermPlanDoc *)GetDocument())->m_ProjInfo.name);
		CDlgVehiclePoolSelect dlgPoolSelect(nProjID,this) ;
		if(dlgPoolSelect.DoModal() == IDOK)
		{
			CString selType =  dlgPoolSelect.GetSelectedTy();

			HTREEITEM SubItem = m_treePaxType.GetChildItem(hSelectItem) ;
			HTREEITEM DeletItem = NULL ;
			while(SubItem)
			{
				if(strcmp(selType,m_treePaxType.GetItemText(SubItem)) == 0)
				{
					CString errormsg ;
					errormsg.Format(_T("%s has selected !"),selType) ;
					MessageBox(errormsg,_T("Warning"),MB_OK) ;
					return ;
				}
				if(m_treePaxType.GetItemText(SubItem).CompareNoCase(_T("All")) == 0)
					DeletItem = SubItem ;
				SubItem = m_treePaxType.GetNextSiblingItem(SubItem) ;
			}
			if(DeletItem)
				m_treePaxType.DeleteItem(DeletItem) ;
            COOLTREE_NODE_INFO cni;
            CCoolTree::InitNodeInfo(this,cni);
			HTREEITEM newItem = m_treePaxType.InsertItem(selType, cni, FALSE, FALSE, hSelectItem) ;
			repControlTreeNodeData* reportNode =  new repControlTreeNodeData(repControlTreeNodeType_PoolNode) ;
			reportNode->m_Data = dlgPoolSelect.GetPoolID() ;
			m_treePaxType.SetItemData(newItem,(DWORD_PTR)reportNode);
		}
	}
	else if(pData->nodeType == repControlTreeNodeType_RunwayMarkRoot)
	{
		if(GetDocument() == NULL)
			return;

		int nProjectID = GetDocument()->GetProjectID();

		CDlgSelectALTObject dlg(nProjectID, ALT_RUNWAY);
		if(dlg.DoModal() == IDOK)
		{

			ALTObjectID altRunway = dlg.GetALTObject();
			ALTObjectGroup altRunwaygroup;
			altRunwaygroup.setName(altRunway);
			
			std::vector< ALTObject> vRunwayObjct;

			altRunwaygroup.SetProjID(nProjectID);
			altRunwaygroup.setType(ALT_RUNWAY);
			altRunwaygroup.GetObjects(vRunwayObjct);

			//delete all node, if exists
			if(vRunwayObjct.size() > 0)
			{
				HTREEITEM hChildItem = m_treePaxType.GetChildItem(hSelectItem);
				if(hChildItem != NULL)
				{
					repControlTreeNodeData *pNodeData = (repControlTreeNodeData *)m_treePaxType.GetItemData(hChildItem);
					if(pNodeData != NULL && pNodeData->nodeType == repControlTreeNodeType_RunwayMarkNode)
					{
						if(pNodeData->runwayMark.m_nRunwayID == -1)//all node
						{
							m_treePaxType.DeleteItem(hChildItem);
						}
					}
				}	
			}

			//add runway node
			for (int nRunway = 0; nRunway < static_cast<int>(vRunwayObjct.size()); ++ nRunway)
			{
				int nObjID = vRunwayObjct.at(nRunway).getID();
				Runway* pRunway = (Runway *)ALTObject::ReadObjectByID(nObjID);
				if(pRunway != NULL)
				{
					CAirsideReportRunwayMark runwayMark1;
					CString strMark1 = CString(pRunway->GetMarking1().c_str());
					runwayMark1.SetRunway(pRunway->getID(),RUNWAYMARK_FIRST,strMark1);
                    COOLTREE_NODE_INFO cni;
                    CCoolTree::InitNodeInfo(this,cni);
					if(IsRunwayMarkSelected(hSelectItem,runwayMark1) == FALSE)
					{
						HTREEITEM hMark1 = m_treePaxType.InsertItem(strMark1, cni, FALSE, FALSE, hSelectItem);
						m_treePaxType.SetItemData(hMark1,(DWORD_PTR)new repControlTreeNodeData(repControlTreeNodeType_RunwayMarkNode,runwayMark1));
					}
					CAirsideReportRunwayMark runwayMark2;
					CString strMark2 = CString(pRunway->GetMarking2().c_str());
					runwayMark2.SetRunway(pRunway->getID(),RUNWAYMARK_SECOND,strMark2);
					if(IsRunwayMarkSelected(hSelectItem,runwayMark2) == FALSE)
					{
						HTREEITEM hMark2 = m_treePaxType.InsertItem(strMark1, cni, FALSE, FALSE, hSelectItem);
						m_treePaxType.SetItemData(hMark2,(DWORD_PTR)new repControlTreeNodeData(repControlTreeNodeType_RunwayMarkNode,runwayMark2));
					}
				}
			}
		}
	}
	else if(pData->nodeType == repControlTreeNodeType_ClassificationRoot)
	{
		CDlgSelecteAircraftClassification dlg;
		if(dlg.DoModal() == IDOK)
		{
			if(IsClassificationSelected(hSelectItem,(int)dlg.m_selectType))
			{
				MessageBox(_T("The classificaiton has been existed."), _T("Warning"), MB_OK);
				return;
			}
            COOLTREE_NODE_INFO cni;
            CCoolTree::InitNodeInfo(this,cni);
			HTREEITEM hClass = m_treePaxType.InsertItem(FlightClassificationBasisType_Nname[(int)dlg.m_selectType], cni, FALSE, FALSE, hSelectItem);
			m_treePaxType.SetItemData(hClass,(DWORD_PTR)new repControlTreeNodeData(repControlTreeNodeType_ClassificationNode,(int)dlg.m_selectType));
			m_treePaxType.Expand(hSelectItem,TVE_EXPAND);
		}


	}
	else if(pData->nodeType == repControlTreeNodeType_ReportingAreaRoot)
	{
		if(GetDocument() == NULL)
			return;

		int nProjectID = GetDocument()->GetProjectID();
		std::vector<ALTOBJECT_TYPE> vObjectType;
		vObjectType.push_back(ALT_REPORTINGAREA);
		vObjectType.push_back(ALT_SECTOR);

		DlgSelectMulti_ALTObjectList dlg(nProjectID, vObjectType);
		if(dlg.DoModal() == IDOK)
		{
			ALTObjectID altArea = dlg.GetALTObject();
			ALTObjectGroup altAreagroup;
			altAreagroup.setName(altArea);
			altAreagroup.setType(ALT_REPORTINGAREA);
			altAreagroup.InsertData(nProjectID);

			//delete all node, if exists

			HTREEITEM hChildItem = m_treePaxType.GetChildItem(hSelectItem);
			if(hChildItem != NULL)
			{
				repControlTreeNodeData *pNodeData = (repControlTreeNodeData *)m_treePaxType.GetItemData(hChildItem);
				if(pNodeData != NULL && pNodeData->nodeType == repControlTreeNodeType_ReportingAreaNode)
				{
					if(pNodeData->objID.IsBlank())//all areas
					{
						m_treePaxType.DeleteItem(hChildItem);
					}
				}
			}	
			
			if (IsObjectSelected(hSelectItem,repControlTreeNodeType_ReportingAreaNode,altArea))
				return;

			CString strName = altArea.GetIDString();
			if (strName == _T(""))
				strName = "All";
            COOLTREE_NODE_INFO cni;
            CCoolTree::InitNodeInfo(this,cni);
			HTREEITEM hArea = m_treePaxType.InsertItem(strName, cni, FALSE, FALSE, hSelectItem);
			repControlTreeNodeData* pNode = new repControlTreeNodeData(repControlTreeNodeType_ReportingAreaNode);
			pNode->objID = altArea;
			m_treePaxType.SetItemData(hArea,(DWORD_PTR)pNode);
			
		}

	}
	//else if(pData->nodeType == repControlTreeNodeType_SectorRoot)
	//{
	//	if(GetDocument() == NULL)
	//		return;

	//	int nProjectID = GetDocument()->GetProjectID();

	//	CDlgSelectALTObject dlg(nProjectID, ALT_SECTOR);
	//	if(dlg.DoModal() == IDOK)
	//	{
	//		ALTObjectID altArea = dlg.GetALTObject();
	//		ALTObjectGroup altAreagroup;
	//		altAreagroup.setName(altArea);
	//		altAreagroup.setType(ALT_SECTOR);
	//		altAreagroup.InsertData(nProjectID);

	//		//delete all node, if exists

	//			HTREEITEM hChildItem = m_treePaxType.GetChildItem(hSelectItem);
	//			if(hChildItem != NULL)
	//			{
	//				repControlTreeNodeData *pNodeData = (repControlTreeNodeData *)m_treePaxType.GetItemData(hChildItem);
	//				if(pNodeData != NULL && pNodeData->nodeType == repControlTreeNodeType_SectorNode)
	//				{
	//					if(pNodeData->objID.IsBlank())//all sectors
	//					{
	//						m_treePaxType.DeleteItem(hChildItem);
	//					}
	//				}
	//			}	
	//		


	//		if (IsObjectSelected(hSelectItem,repControlTreeNodeType_SectorNode,altArea))
	//			return;

	//		CString strName = altArea.GetIDString();
	//		HTREEITEM hSector = m_treePaxType.InsertItem(strName,hSelectItem);
	//		repControlTreeNodeData* pNode = new repControlTreeNodeData(repControlTreeNodeType_SectorNode);
	//		pNode->objID = altArea;
	//		m_treePaxType.SetItemData(hSector,(DWORD_PTR)pNode);
	//	
	//	}

	//}
	else if(pData->nodeType == repControlTreeNodeType_Taxiways)
	{
		CDlgTaxiwayFamilySelect dlg(GetDocument()->GetProjectID());
		if(dlg.DoModal() != IDOK)
			return;

		CString strTaxiwayName = dlg.GetSelTaxiwayFamilyName();
		bool bSelectSingleTaxiway = dlg.IsSingleTaxiwaySelected();

		//CDlgTaxiWaySelect dlgTaxiSel(GetDocument()->GetProjectID(),this) ;
		//if(dlgTaxiSel.DoModal() == IDOK)
		//{
		//	if(dlgTaxiSel.GetSelectNode().GetNodeType() != CAirsideReportNode::ReportNodeType_Taxiway)
		//	{
		//		MessageBox(_T("you should select the taxiway processors."),_T("Warning"),MB_OK) ;
		//		return ;
		//	}
		if(IsTaxiwaySelect(hSelectItem,strTaxiwayName))
		{
			CString err;
			err.Format(_T("%s has selected!"),strTaxiwayName) ;
			MessageBox(err,NULL,MB_OK) ;
			return ;
		}
        COOLTREE_NODE_INFO cni;
        CCoolTree::InitNodeInfo(this,cni);
		HTREEITEM pTaxiWayItem = m_treePaxType.InsertItem(strTaxiwayName, cni, FALSE, FALSE, hSelectItem) ;
		repControlTreeNodeData* pNode = new repControlTreeNodeData(repControlTreeNodeType_Taxiway);
		pNode->nClassification = bSelectSingleTaxiway? 1:0 ;
		m_treePaxType.SetItemData(pTaxiWayItem,(DWORD_PTR)pNode) ;

		HTREEITEM PFromNode = m_treePaxType.InsertItem(_T("From"), cni, FALSE, FALSE, pTaxiWayItem) ;
		pNode = new repControlTreeNodeData(repControlTreeNodeType_TaxiWayFrom);
		m_treePaxType.SetItemData(PFromNode,(DWORD_PTR)pNode) ;
	

		HTREEITEM newItem = m_treePaxType.InsertItem(_T("Start"), cni, FALSE, FALSE, PFromNode) ;
		repControlTreeNodeData* nodedata = new repControlTreeNodeData(repControlTreeNodeType_TaxiIntersection) ;
		nodedata->nClassification = -1 ;
		m_treePaxType.SetItemData(newItem,(DWORD_PTR)nodedata) ;


		HTREEITEM PToNode = m_treePaxType.InsertItem(_T("To"), cni, FALSE, FALSE, pTaxiWayItem) ;
		pNode = new repControlTreeNodeData(repControlTreeNodeType_TaxiwayTo);
		m_treePaxType.SetItemData(PToNode,(DWORD_PTR)pNode) ;
		

		newItem = m_treePaxType.InsertItem(_T("End"), cni, FALSE, FALSE, PToNode) ;
		nodedata = new repControlTreeNodeData(repControlTreeNodeType_TaxiIntersection) ;
		nodedata->nClassification = -1 ;
		m_treePaxType.SetItemData(newItem,(DWORD_PTR)nodedata) ;


			m_treePaxType.Expand(pTaxiWayItem,TVE_EXPAND) ;
		//}	
	}
	else if(pData->nodeType == repControlTreeNodeType_TaxiWayFrom || pData->nodeType == repControlTreeNodeType_TaxiwayTo)
	{
		HTREEITEM PtaxiwayItem = m_treePaxType.GetParentItem(hSelectItem) ;
		repControlTreeNodeData* pNode = (repControlTreeNodeData*)m_treePaxType.GetItemData(PtaxiwayItem) ;
		BOOL bSingleTaxiway = pNode->nClassification;
		if (bSingleTaxiway == FALSE)
		{
			MessageBox(_T("Cannot select a taxiway family intersection node!"),_T("Warning"),MB_OK) ;
			return;
		}

		if (pData->nodeType == repControlTreeNodeType_TaxiwayTo)
		{
			HTREEITEM hFromItem = m_treePaxType.GetPrevSiblingItem(hSelectItem);
			HTREEITEM hFromNode = m_treePaxType.GetChildItem(hFromItem);
			repControlTreeNodeData* pFromNode = (repControlTreeNodeData*)m_treePaxType.GetItemData(hFromNode) ;
			if (pFromNode->nClassification == -1)		//from node is start
			{
				MessageBox(_T("Cannot define a specific node as end because of undefine the from node!"),_T("Warning"),MB_OK) ;
				return;
			}
		}

		int nTaxiwayID = -1;
		std::vector<int> vAirportIds;
		InputAirside::GetAirportList(GetDocument()->GetProjectID(), vAirportIds);

		std::vector<ALTObject> vtaxiways;
		ALTObject::GetObjectList(ALT_TAXIWAY,vAirportIds.at(0),vtaxiways);
	
		std::vector<ALTObject>::iterator iter = vtaxiways.begin();
		for (;iter!= vtaxiways.end();++iter)
		{
			if ((*iter).GetObjNameString() == m_treePaxType.GetItemText(PtaxiwayItem))
			{
				nTaxiwayID = (*iter).getID();
				break;
			}
		}

		DlgTaxiwayIntersectionNodeSelection dlg(GetDocument()->GetProjectID(),nTaxiwayID, this) ;
		if(dlg.DoModal() != IDOK)
			return;
		
		HTREEITEM pChild = m_treePaxType.GetChildItem(hSelectItem) ;
		if(pChild)
			m_treePaxType.DeleteItem(pChild);
        COOLTREE_NODE_INFO cni;
        CCoolTree::InitNodeInfo(this,cni);
		pChild = m_treePaxType.InsertItem(dlg.GetNodeName(), cni, FALSE, FALSE, hSelectItem);
		repControlTreeNodeData* pIntersectionNode = new repControlTreeNodeData(repControlTreeNodeType_TaxiIntersection);
		pIntersectionNode->nClassification = dlg.GetNodeID();
		m_treePaxType.SetItemData(pChild,(DWORD_PTR)pIntersectionNode) ;
		
	}
	else if(pData->nodeType == repControlTreeNodeType_RunwayExits)
	{
		AddRunwayExitReportRunwayItem(hSelectItem) ;
	}
	else if(pData->nodeType == repControlTreeNodeType_Exit_ItemRoot)
	{
		AddRunwayExitReportExitItem(hSelectItem) ;
	}
	else if(pData->nodeType == repControlTreeNodeType_Exit_Oper)
	{
        m_treePaxType.DoEdit(hSelectItem);
	}
}
BOOL CAirsideRepControlView::IsRunwayMarkSelected( HTREEITEM hACClassRoot,CAirsideReportRunwayMark& reportRunwayMark )
{
	HTREEITEM hChildItem = m_treePaxType.GetChildItem(hACClassRoot);
	while (hChildItem != NULL)
	{
		repControlTreeNodeData *pNodeData = (repControlTreeNodeData *)m_treePaxType.GetItemData(hChildItem);
		if(pNodeData != NULL && pNodeData->nodeType == repControlTreeNodeType_RunwayMarkNode)
		{
			if(pNodeData->runwayMark.m_nRunwayID == reportRunwayMark.m_nRunwayID &&
				pNodeData->runwayMark.m_enumRunwayMark == reportRunwayMark.m_enumRunwayMark)
				return TRUE;

		}
		hChildItem = m_treePaxType.GetNextSiblingItem(hChildItem);
	}
	return FALSE;
}
BOOL CAirsideRepControlView::IsClassificationSelected( HTREEITEM hRunwayMarkRoot,int nClass )
{
	HTREEITEM hChildItem = m_treePaxType.GetChildItem(hRunwayMarkRoot);
	while (hChildItem != NULL)
	{
		repControlTreeNodeData *pNodeData = (repControlTreeNodeData *)m_treePaxType.GetItemData(hChildItem);
		if(pNodeData != NULL && pNodeData->nodeType == repControlTreeNodeType_ClassificationNode)
		{
			if(pNodeData->nClassification == nClass)
				return TRUE;

		}
		hChildItem = m_treePaxType.GetNextSiblingItem(hChildItem);
	}
	return FALSE;
}

BOOL CAirsideRepControlView::IsObjectSelected(HTREEITEM hObjRoot,repControlTreeNodeType nodeType,const ALTObjectID& altID)
{
	HTREEITEM hChildItem = m_treePaxType.GetChildItem(hObjRoot);
	while (hChildItem != NULL)
	{
		repControlTreeNodeData *pNodeData = (repControlTreeNodeData *)m_treePaxType.GetItemData(hChildItem);
		if(pNodeData != NULL)
		{
			if (nodeType != repControlTreeNodeType_ReportingAreaNode /*|| nodeType != repControlTreeNodeType_SectorNode */)
				return FALSE;

			if (altID.idFits(pNodeData->objID))
				return TRUE;

		}
		hChildItem = m_treePaxType.GetNextSiblingItem(hChildItem);
	}
	return FALSE;
}

void CAirsideRepControlView::OnToolbarbuttondel()
{
	HTREEITEM hSelectItem=m_treePaxType.GetSelectedItem();

	if(GetReportType() == Airside_RunwayUtilization || 
		GetReportType() == Airside_RunwayDelay || 
		GetReportType() == Airside_RunwayCrossings ||
		GetReportType() == Airside_IntersectionOperation||
		GetReportType() == Airside_TaxiwayUtilization||
		GetReportType() == Airside_StandOperations ||
		GetReportType() == Airside_ControllerWorkload ||
		GetReportType() == Airside_TakeoffProcess)//tree performer handle the message
	{
		if(GetTreePerformer())
			GetTreePerformer()->OnToolBarDel(hSelectItem);
		return;
	}
	


	repControlTreeNodeData *pData = reinterpret_cast<repControlTreeNodeData *>(m_treePaxType.GetItemData(hSelectItem));
	if (pData == NULL)
		return;
    COOLTREE_NODE_INFO cni;
    CCoolTree::InitNodeInfo(this,cni);
	if (pData->nodeType == repControlTreeNodeType_FromNode)
	{
		m_treePaxType.SetItemText(hSelectItem, _T("All"));
		CAirsideReportNode defaultNode;
		pData->nodeObject = defaultNode;

	}
	else if (pData->nodeType == repControlTreeNodeType_ToNode )
	{
		m_treePaxType.SetItemText(hSelectItem,_T("All"));
		CAirsideReportNode defaultNode;
		pData->nodeObject = defaultNode;
	}
	else if (pData->nodeType == repControlTreeNodeType_FltType) 
	{

		delete pData;
		m_treePaxType.DeleteItem(hSelectItem);
	}
	else if (pData->nodeType == repControlTreeNodeType_Node)
	{	
		HTREEITEM hParentItem = m_treePaxType.GetParentItem(hSelectItem);

		delete pData;
		m_treePaxType.DeleteItem(hSelectItem);

		if (m_treePaxType.GetChildItem(hParentItem) == NULL)
		{		
			HTREEITEM hAllNode = m_treePaxType.InsertItem("All", cni, FALSE, FALSE, hParentItem);
			m_treePaxType.SetItemData(hAllNode,(DWORD_PTR)new repControlTreeNodeData(repControlTreeNodeType_Node,CAirsideReportNode()));
			m_treePaxType.Expand(hParentItem,TVE_EXPAND);	
		}
	}else if(pData->nodeType == repControlTreeNodeType_VehicleTypeNode||pData->nodeType ==  repControlTreeNodeType_PoolNode)
	{
		HTREEITEM hParentItem = m_treePaxType.GetParentItem(hSelectItem);
		m_treePaxType.DeleteItem(hSelectItem);
		if (m_treePaxType.GetChildItem(hParentItem) == NULL)
		{
			if(pData->nodeType == repControlTreeNodeType_VehicleTypeNode)
			{
				HTREEITEM hAllNode = m_treePaxType.InsertItem("All Type", cni, FALSE, FALSE, hParentItem);
				m_treePaxType.SetItemData(hAllNode,(DWORD_PTR)new repControlTreeNodeData(repControlTreeNodeType_VehicleTypeNode));
				m_treePaxType.Expand(hParentItem,TVE_EXPAND);	
			}else
			{
				HTREEITEM hAllNode = m_treePaxType.InsertItem("All", cni, FALSE, FALSE, hParentItem);
				m_treePaxType.SetItemData(hAllNode,(DWORD_PTR)new repControlTreeNodeData(repControlTreeNodeType_PoolNode));
				m_treePaxType.Expand(hParentItem,TVE_EXPAND);	
			}
		}
	}
	else if(pData->nodeType == repControlTreeNodeType_RunwayMarkNode)
	{
		HTREEITEM hParentItem = m_treePaxType.GetParentItem(hSelectItem);

		delete pData;
		m_treePaxType.DeleteItem(hSelectItem);

		if (m_treePaxType.GetChildItem(hParentItem) == NULL)
		{		
			HTREEITEM hAllNode = m_treePaxType.InsertItem("All", cni, FALSE, FALSE, hParentItem);
			m_treePaxType.SetItemData(hAllNode,(DWORD_PTR)new repControlTreeNodeData(repControlTreeNodeType_RunwayMarkNode,CAirsideReportRunwayMark()));
			m_treePaxType.Expand(hParentItem,TVE_EXPAND);	
		}

	}
	else if(pData->nodeType == repControlTreeNodeType_ClassificationNode)
	{
		HTREEITEM hParentItem = m_treePaxType.GetParentItem(hSelectItem);

		delete pData;
		m_treePaxType.DeleteItem(hSelectItem);
	}
	else if(pData->nodeType == repControlTreeNodeType_ReportingAreaNode )
	{
		HTREEITEM hParentItem = m_treePaxType.GetParentItem(hSelectItem);

		delete pData;
		m_treePaxType.DeleteItem(hSelectItem);

		if (m_treePaxType.GetChildItem(hParentItem) == NULL)
		{		
			HTREEITEM hAllNode = m_treePaxType.InsertItem("All", cni, FALSE, FALSE, hParentItem);
			m_treePaxType.SetItemData(hAllNode,(DWORD_PTR)new repControlTreeNodeData(repControlTreeNodeType_ReportingAreaNode));
			m_treePaxType.Expand(hParentItem,TVE_EXPAND);	
		}

	}
	//else if(pData->nodeType == repControlTreeNodeType_SectorNode )
	//{
	//	HTREEITEM hParentItem = m_treePaxType.GetParentItem(hSelectItem);

	//	delete pData;
	//	m_treePaxType.DeleteItem(hSelectItem);

	//	if (m_treePaxType.GetChildItem(hParentItem) == NULL)
	//	{		
	//		HTREEITEM hAllNode = m_treePaxType.InsertItem("All",hParentItem);
	//		m_treePaxType.SetItemData(hAllNode,(DWORD_PTR)new repControlTreeNodeData(repControlTreeNodeType_SectorNode));
	//		m_treePaxType.Expand(hParentItem,TVE_EXPAND);	
	//	}

	//}
	else if(pData->nodeType == repControlTreeNodeType_Taxiway )
	{
		delete pData;
		m_treePaxType.DeleteItem(hSelectItem);
	}
	else if(pData->nodeType == repControlTreeNodeType_TaxiIntersection)
	{
		HTREEITEM parItem = m_treePaxType.GetParentItem(hSelectItem) ;
		m_treePaxType.DeleteItem(hSelectItem);
	}else if(pData->nodeType == repControlTreeNodeType_Exit_Runway || pData->nodeType == repControlTreeNodeType_Exit_Item)
	{
		delete pData;
		m_treePaxType.DeleteItem(hSelectItem);
	}

}
void CAirsideRepControlView::InitializeDefaultTreeData()
{
	if(m_pParameter == NULL)
		return;
	CTermPlanDoc *pTermPlanDoc = GetDocument();	
	ASSERT(pTermPlanDoc);
	reportType rpType = pTermPlanDoc->GetARCReportManager().GetAirsideReportManager()->GetReportType();
	switch(rpType)
	{
	case Airside_FlightDelay:
		{
            COOLTREE_NODE_INFO cni;
            CCoolTree::InitNodeInfo(this,cni);
			CAirsideFlightDelayParam* pPara = (CAirsideFlightDelayParam*)m_pParameter;
			if (m_hRootFlightType == NULL)
				return;
			repControlTreeNodeData *pData = reinterpret_cast<repControlTreeNodeData *>(m_treePaxType.GetItemData(m_hRootFlightType));
			if (pData == NULL)
				return;
			if (pData->nodeType == repControlTreeNodeType_FltTypeRoot)
			{
				HTREEITEM hDefaultItem;
				if(pPara->getFlightConstraintCount() == 0)
				{
					hDefaultItem = m_treePaxType.InsertItem("DEFAULT", cni, FALSE, FALSE, m_hRootFlightType,TVI_LAST);
					m_treePaxType.SetItemData(hDefaultItem,(DWORD_PTR)new repControlTreeNodeData(repControlTreeNodeType_FltType,FlightConstraint()));
				}
				else
				{
					for (int i = 0; i < (int)pPara->getFlightConstraintCount(); i++)
					{
						CString strFlightType;
						pPara->getFlightConstraint(i).screenPrint(strFlightType.GetBuffer(1024));
						strFlightType.ReleaseBuffer();
						hDefaultItem = m_treePaxType.InsertItem(strFlightType, cni, FALSE, FALSE, m_hRootFlightType,TVI_LAST);
						m_treePaxType.SetItemData(hDefaultItem,(DWORD_PTR)new repControlTreeNodeData(repControlTreeNodeType_FltType,pPara->getFlightConstraint(i)));
					}
				}
			}

			m_treePaxType.Expand(m_hRootFlightType,TVE_EXPAND);

			HTREEITEM hFromToItem = m_hRootNodeToNode;
			HTREEITEM hFromItem = m_treePaxType.GetChildItem(hFromToItem);
			HTREEITEM hStartNode;
			if (pPara->GetStartObject().GetNodeName().IsEmpty())
			{
				hStartNode = m_treePaxType.InsertItem("All", cni, FALSE, FALSE, hFromItem);
			}
			else
			{
				hStartNode = m_treePaxType.InsertItem(pPara->GetStartObject().GetNodeName(), cni, FALSE, FALSE, hFromItem);
			}
			m_treePaxType.SetItemData(hStartNode,(DWORD_PTR)new repControlTreeNodeData(repControlTreeNodeType_FromNode,pPara->GetStartObject()));

			HTREEITEM hToItem = m_treePaxType.GetNextSiblingItem(hFromItem);
			HTREEITEM hEndNode;
			if (pPara->GetEndObject().GetNodeName().IsEmpty())
			{
				hEndNode = m_treePaxType.InsertItem("All", cni, FALSE, FALSE, hToItem);;
			}
			else
			{
				hEndNode = m_treePaxType.InsertItem(pPara->GetEndObject().GetNodeName(), cni, FALSE, FALSE, hToItem);
			}
			m_treePaxType.SetItemData(hEndNode,(DWORD_PTR)new repControlTreeNodeData(repControlTreeNodeType_ToNode,pPara->GetEndObject()));
			
			m_treePaxType.Expand(hFromToItem,TVE_EXPAND);
			m_treePaxType.Expand(hFromItem,TVE_EXPAND);
			m_treePaxType.Expand(hToItem,TVE_EXPAND);
		}
		break;
	case Airside_NodeDelay:
        {
            COOLTREE_NODE_INFO cni;
            CCoolTree::InitNodeInfo(this,cni);
			CAirsideNodeDelayReportParameter* pPara = (CAirsideNodeDelayReportParameter*)m_pParameter;
			HTREEITEM hFltTypeItem = m_hRootFlightType;
			if (hFltTypeItem == NULL)
				return;
			repControlTreeNodeData *pData = reinterpret_cast<repControlTreeNodeData *>(m_treePaxType.GetItemData(hFltTypeItem));
			if (pData == NULL)
				return;
			if (pData->nodeType == repControlTreeNodeType_FltTypeRoot)
			{
				HTREEITEM hDefaultItem;
				if (pPara->getFlightConstraintCount() == 0)
				{
					HTREEITEM hDefaultItem = m_treePaxType.InsertItem("DEFAULT", cni, FALSE, FALSE, hFltTypeItem, TVI_LAST);
					m_treePaxType.SetItemData(hDefaultItem,(DWORD_PTR)new repControlTreeNodeData(repControlTreeNodeType_FltType,FlightConstraint()));
				}
				else
				{
					for (int i = 0; i < (int)pPara->getFlightConstraintCount(); i++)
					{
						CString strFlightType;
						pPara->getFlightConstraint(i).screenPrint(strFlightType.GetBuffer(1024));
						strFlightType.ReleaseBuffer();
						hDefaultItem = m_treePaxType.InsertItem(strFlightType, cni, FALSE, FALSE, hFltTypeItem,TVI_LAST);
						m_treePaxType.SetItemData(hDefaultItem,(DWORD_PTR)new repControlTreeNodeData(repControlTreeNodeType_FltType,pPara->getFlightConstraint(i)));
					}
				}
			}
			m_treePaxType.Expand(hFltTypeItem,TVE_EXPAND);

			HTREEITEM hNodesItem = m_hRootNodes;
			if (pPara->getReportNodeCount() == 0)
			{
				HTREEITEM hAllNode = m_treePaxType.InsertItem("All", cni, FALSE, FALSE, hNodesItem);
				m_treePaxType.SetItemData(hAllNode,(DWORD_PTR)new repControlTreeNodeData(repControlTreeNodeType_Node,CAirsideReportNode()));
			}
			else
			{
				for (size_t i = 0; i < pPara->getReportNodeCount();i++)
				{
					CAirsideReportNode& node = pPara->getReportNode(i);
					if (node.GetNodeName().IsEmpty())
					{
						HTREEITEM hAllNode = m_treePaxType.InsertItem("All", cni, FALSE, FALSE, hNodesItem);
						m_treePaxType.SetItemData(hAllNode,(DWORD_PTR)new repControlTreeNodeData(repControlTreeNodeType_Node,node));
					}
					else
					{
						HTREEITEM hAllNode = m_treePaxType.InsertItem(node.GetNodeName(), cni, FALSE, FALSE, hNodesItem);
						m_treePaxType.SetItemData(hAllNode,(DWORD_PTR)new repControlTreeNodeData(repControlTreeNodeType_Node,node));
					}
				}
			}
			m_treePaxType.Expand(hNodesItem,TVE_EXPAND);
		}
		break;
	case Airside_FlightActivity:
        {
            COOLTREE_NODE_INFO cni;
            CCoolTree::InitNodeInfo(this,cni);
			CFlightActivityParam* pPara = (CFlightActivityParam*)m_pParameter;
			HTREEITEM hFltTypeItem =  m_hRootFlightType;
			if (hFltTypeItem == NULL)
				return;
			repControlTreeNodeData *pData = reinterpret_cast<repControlTreeNodeData *>(m_treePaxType.GetItemData(hFltTypeItem));
			if (pData == NULL)
				return;
			if (pData->nodeType == repControlTreeNodeType_FltTypeRoot)
			{
				HTREEITEM hDefaultItem;
				if (pPara->getFlightConstraintCount() == 0)
				{
					HTREEITEM hDefaultItem = m_treePaxType.InsertItem("DEFAULT", cni, FALSE, FALSE, hFltTypeItem,TVI_LAST);
					m_treePaxType.SetItemData(hDefaultItem,(DWORD_PTR)new repControlTreeNodeData(repControlTreeNodeType_FltType,FlightConstraint()));
				}
				else
				{
					for (int i = 0; i < (int)pPara->getFlightConstraintCount(); i++)
					{
						CString strFlightType;
						pPara->getFlightConstraint(i).screenPrint(strFlightType.GetBuffer(1024));
						strFlightType.ReleaseBuffer();
						hDefaultItem = m_treePaxType.InsertItem(strFlightType, cni, FALSE, FALSE, hFltTypeItem,TVI_LAST);
						m_treePaxType.SetItemData(hDefaultItem,(DWORD_PTR)new repControlTreeNodeData(repControlTreeNodeType_FltType,pPara->getFlightConstraint(i)));
					}
				}
			}
			m_treePaxType.Expand(hFltTypeItem,TVE_EXPAND);
			//HTREEITEM hNodesItem = m_treePaxType.GetNextSiblingItem(hFltTypeItem);
			//HTREEITEM hStartNode = m_treePaxType.InsertItem("Start",hNodesItem);
			//m_treePaxType.SetItemData(hStartNode,(DWORD_PTR)new repControlTreeNodeData(repControlTreeNodeType_FromNode,CAirsideReportNode()));
		}
		break;
	case Airside_AircraftOperational:
        {
            COOLTREE_NODE_INFO cni;
            CCoolTree::InitNodeInfo(this,cni);
			CFlightOperationalParam* pPara = (CFlightOperationalParam*)m_pParameter;
			HTREEITEM hFltTypeItem =  m_hRootFlightType;
			if (hFltTypeItem == NULL)
				return;
			repControlTreeNodeData *pData = reinterpret_cast<repControlTreeNodeData *>(m_treePaxType.GetItemData(hFltTypeItem));
			if (pData == NULL)
				return;
			if (pData->nodeType == repControlTreeNodeType_FltTypeRoot)
			{
				HTREEITEM hDefaultItem;
				if (pPara->getFlightConstraintCount() == 0)
				{
					HTREEITEM hDefaultItem = m_treePaxType.InsertItem("DEFAULT", cni, FALSE, FALSE, hFltTypeItem,TVI_LAST);
					m_treePaxType.SetItemData(hDefaultItem,(DWORD_PTR)new repControlTreeNodeData(repControlTreeNodeType_FltType,FlightConstraint()));
				}
				else
				{
					for (int i = 0; i < (int)pPara->getFlightConstraintCount(); i++)
					{
						CString strFlightType;
						pPara->getFlightConstraint(i).screenPrint(strFlightType.GetBuffer(1024));
						strFlightType.ReleaseBuffer();
						hDefaultItem = m_treePaxType.InsertItem(strFlightType, cni, FALSE, FALSE, hFltTypeItem,TVI_LAST);
						m_treePaxType.SetItemData(hDefaultItem,(DWORD_PTR)new repControlTreeNodeData(repControlTreeNodeType_FltType,pPara->getFlightConstraint(i)));
					}
				}
			}

			m_treePaxType.Expand(hFltTypeItem,TVE_EXPAND);

			HTREEITEM hFromToItem = m_treePaxType.GetNextSiblingItem(m_hRootFlightType);
			HTREEITEM hFromItem = m_treePaxType.GetChildItem(hFromToItem);
			HTREEITEM hStartNode;
			if (pPara->GetStartObject().GetNodeName().IsEmpty())
			{
				hStartNode = m_treePaxType.InsertItem("All", cni, FALSE, FALSE, hFromItem);
			}
			else
			{
				hStartNode = m_treePaxType.InsertItem(pPara->GetStartObject().GetNodeName(), cni, FALSE, FALSE, hFromItem);
			}
			m_treePaxType.SetItemData(hStartNode,(DWORD_PTR)new repControlTreeNodeData(repControlTreeNodeType_FromNode,pPara->GetStartObject()));

			HTREEITEM hToItem = m_treePaxType.GetNextSiblingItem(hFromItem);
			HTREEITEM hEndNode;
			if (pPara->GetEndObject().GetNodeName().IsEmpty())
			{
				hEndNode = m_treePaxType.InsertItem("All", cni, FALSE, FALSE, hToItem);
			}
			else
			{
				hEndNode = m_treePaxType.InsertItem(pPara->GetEndObject().GetNodeName(), cni, FALSE, FALSE, hToItem);
			}
			m_treePaxType.SetItemData(hEndNode,(DWORD_PTR)new repControlTreeNodeData(repControlTreeNodeType_ToNode,pPara->GetEndObject()));

			m_treePaxType.Expand(hFromToItem,TVE_EXPAND);
			m_treePaxType.Expand(hFromItem,TVE_EXPAND);
			m_treePaxType.Expand(hToItem,TVE_EXPAND);
		}
		break;

	case Airside_RunwayOperaitons:
        {
            COOLTREE_NODE_INFO cni;
            CCoolTree::InitNodeInfo(this,cni);
			//runway
			if(m_pParameter == NULL)
				return;

			AirsideRunwayOperationReportParam *pParam = (AirsideRunwayOperationReportParam *)m_pParameter;
			if(pParam == NULL)
				return;
            int nRunwayCount = pParam->m_vRunwayParameter.size();
			if(nRunwayCount == 0)
			{
				HTREEITEM hAllNode = m_treePaxType.InsertItem("All", cni, FALSE, FALSE, m_hRootRunways);
				m_treePaxType.SetItemData(hAllNode,(DWORD_PTR)new repControlTreeNodeData(repControlTreeNodeType_RunwayMarkNode,CAirsideReportRunwayMark()));
				m_treePaxType.Expand(m_hRootRunways,TVE_EXPAND);	
			}
			else
			{
				for (int nRunway = 0; nRunway < nRunwayCount; ++ nRunway)
				{
					CString strNodeName = pParam->m_vRunwayParameter[nRunway].m_strMarkName;
					if(pParam->m_vRunwayParameter[nRunway].m_nRunwayID == -1)
						strNodeName = _T("ALL");
						
					HTREEITEM hAllNode = m_treePaxType.InsertItem(strNodeName, cni, FALSE, FALSE, m_hRootRunways);
					m_treePaxType.SetItemData(hAllNode,(DWORD_PTR)new repControlTreeNodeData(repControlTreeNodeType_RunwayMarkNode,pParam->m_vRunwayParameter[nRunway]));
					m_treePaxType.Expand(m_hRootRunways,TVE_EXPAND);	
				}

			}

			//flight type
			HTREEITEM hFltTypeItem =  m_hRootFlightType;
			if(pParam->getFlightConstraintCount() == 0)
			{
				HTREEITEM hDefaultItem = m_treePaxType.InsertItem("DEFAULT", cni, FALSE, FALSE, hFltTypeItem,TVI_LAST);
				m_treePaxType.SetItemData(hDefaultItem,(DWORD_PTR)new repControlTreeNodeData(repControlTreeNodeType_FltType,FlightConstraint()));
			}
			else
			{
				for (int nFlight = 0; nFlight < static_cast<int>(pParam->getFlightConstraintCount()); ++ nFlight)
				{ 
					CString strFlightType;
					 pParam->getFlightConstraint(nFlight).screenPrint(strFlightType.GetBuffer(1024));
					 strFlightType.ReleaseBuffer();

					 HTREEITEM hNewItem = m_treePaxType.InsertItem(strFlightType, cni, FALSE, FALSE, hFltTypeItem,TVI_LAST);
					 m_treePaxType.SetItemData(hNewItem,(DWORD_PTR)new repControlTreeNodeData(repControlTreeNodeType_FltType,pParam->getFlightConstraint(nFlight)));

				}

			}

			m_treePaxType.Expand(hFltTypeItem,TVE_EXPAND);

			//classification

			if(pParam->m_vTypes.size() > 0)
			{
				for (int nType = 0; nType < static_cast<int>(pParam->m_vTypes.size()); ++nType)
				{
					if(IsClassificationSelected(m_hRootClassification,pParam->m_vTypes[nType]))
					{
						continue;
					}
		
					HTREEITEM hClass = m_treePaxType.InsertItem(FlightClassificationBasisType_Nname[pParam->m_vTypes[nType]], cni, FALSE, FALSE, m_hRootClassification);
					m_treePaxType.SetItemData(hClass,(DWORD_PTR)new repControlTreeNodeData(repControlTreeNodeType_ClassificationNode,pParam->m_vTypes[nType]));
					m_treePaxType.Expand(m_hRootClassification,TVE_EXPAND);
				}

			}


		}
		break;
	case Airside_RunwayUtilization:
	case Airside_RunwayDelay:
	case Airside_RunwayCrossings:
	case Airside_StandOperations:
	case Airside_IntersectionOperation:
	case Airside_TaxiwayUtilization:
	case Airside_ControllerWorkload:
	case Airside_TakeoffProcess:
		{
			if(GetTreePerformer())
				GetTreePerformer()->LoadData();
			
		}
		break;
	case Airside_FlightConflict:
        {
            COOLTREE_NODE_INFO cni;
            CCoolTree::InitNodeInfo(this,cni);
			AirsideFlightConflictPara* pPara = (AirsideFlightConflictPara*)m_pParameter;
			if (m_hRootReportingArea)
			{
				HTREEITEM hDefNode;
				if (pPara->GetAreaList().empty())
				{
					hDefNode = m_treePaxType.InsertItem("All", cni, FALSE, FALSE, m_hRootReportingArea,TVI_LAST);
					m_treePaxType.SetItemData(hDefNode,(DWORD_PTR)new repControlTreeNodeData(repControlTreeNodeType_ReportingAreaNode));
				}
				else
				{
					int nCount = (int)pPara->GetAreaList().size();
					for (int i = 0; i < nCount; i++)
					{
						ALTObjectID areaID = pPara->GetAreaList().at(i);
						repControlTreeNodeData* pNodeData = new repControlTreeNodeData(repControlTreeNodeType_ReportingAreaNode);
						pNodeData->objID = areaID;
						CString strName = areaID.GetIDString();
						if (strName.IsEmpty())
							strName = "All";
						hDefNode = m_treePaxType.InsertItem(strName, cni, FALSE, FALSE, m_hRootReportingArea,TVI_LAST);
						m_treePaxType.SetItemData(hDefNode,(DWORD_PTR)pNodeData);
					}
				}
			}

			m_treePaxType.Expand(m_hRootReportingArea,TVE_EXPAND);
			
		}
		break;
	case Airside_TaxiwayDelay:
        {
            COOLTREE_NODE_INFO cni;
            CCoolTree::InitNodeInfo(this,cni);
            CTaxiwayDelayParameters* pPara = (CTaxiwayDelayParameters*)m_pParameter;
            HTREEITEM pTaxiWayItem; 

            if (pPara->GetTaxiwayData()->size() == 0)
            {	
                pTaxiWayItem = m_treePaxType.InsertItem(_T("ALL"), cni, FALSE, FALSE, m_hRootReportingArea) ;
                repControlTreeNodeData* pNode = new repControlTreeNodeData(repControlTreeNodeType_Taxiway);
                pNode->nClassification = -1;
                m_treePaxType.SetItemData(pTaxiWayItem,(DWORD_PTR)pNode);

                HTREEITEM PFromNode = m_treePaxType.InsertItem(_T("From"), cni, FALSE, FALSE, pTaxiWayItem);
                pNode = new repControlTreeNodeData(repControlTreeNodeType_TaxiWayFrom);
                m_treePaxType.SetItemData(PFromNode,(DWORD_PTR)pNode) ;


                HTREEITEM newItem = m_treePaxType.InsertItem(_T("Start"), cni, FALSE, FALSE, PFromNode);
                repControlTreeNodeData* nodedata = new repControlTreeNodeData(repControlTreeNodeType_TaxiIntersection) ;
                nodedata->nClassification = -1 ;
                m_treePaxType.SetItemData(newItem,(DWORD_PTR)nodedata);


                HTREEITEM PToNode = m_treePaxType.InsertItem(_T("To"), cni, FALSE, FALSE, pTaxiWayItem) ;
                pNode = new repControlTreeNodeData(repControlTreeNodeType_TaxiwayTo);
                m_treePaxType.SetItemData(PToNode,(DWORD_PTR)pNode);


                newItem = m_treePaxType.InsertItem(_T("End"), cni, FALSE, FALSE, PToNode);
                nodedata = new repControlTreeNodeData(repControlTreeNodeType_TaxiIntersection);
                nodedata->nClassification = -1 ;
                m_treePaxType.SetItemData(newItem,(DWORD_PTR)nodedata) ;

                m_treePaxType.Expand(pTaxiWayItem,TVE_EXPAND) ;
            }
            else 
            {
                for (int i = 0; i < pPara->GetCount(); i++)
                {
                    CTaxiwaySelect* pTaxiwayItem = pPara->GetTaxiwayForShowItem(i);
                    repControlTreeNodeData* pNode = NULL;
                    if (pTaxiwayItem->GetTaxiwayName().IsEmpty())
                    {
                        pTaxiWayItem = m_treePaxType.InsertItem(_T("ALL"), cni, FALSE, FALSE, m_hRootReportingArea) ;
                        pNode = new repControlTreeNodeData(repControlTreeNodeType_Taxiway);
                        pNode->nClassification = -1;
                    }
                    else
                    {
                        pTaxiWayItem = m_treePaxType.InsertItem(pTaxiwayItem->GetTaxiwayName(), cni, FALSE, FALSE, m_hRootReportingArea) ;
                        pNode = new repControlTreeNodeData(repControlTreeNodeType_Taxiway);
                        pNode->nClassification = pTaxiwayItem->GetTaxiwayID();
                    }
                    m_treePaxType.SetItemData(pTaxiWayItem,(DWORD_PTR)pNode);

                    std::vector<CReportTaxiwaySegment>* pTYData = pTaxiwayItem->GetIntersectionSegment();
                    if (pTYData->size() == 0)
                    {
                        ASSERT(pTYData);
                        HTREEITEM PFromNode = m_treePaxType.InsertItem(_T("From"), cni, FALSE, FALSE, pTaxiWayItem) ;
                        repControlTreeNodeData* pNode = new repControlTreeNodeData(repControlTreeNodeType_TaxiWayFrom);
                        m_treePaxType.SetItemData(PFromNode,(DWORD_PTR)pNode) ;


                        HTREEITEM newItem = m_treePaxType.InsertItem(_T("Start"), cni, FALSE, FALSE, PFromNode) ;
                        repControlTreeNodeData* nodedata = new repControlTreeNodeData(repControlTreeNodeType_TaxiIntersection) ;
                        nodedata->nClassification = -1 ;
                        m_treePaxType.SetItemData(newItem,(DWORD_PTR)nodedata) ;


                        HTREEITEM PToNode = m_treePaxType.InsertItem(_T("To"), cni, FALSE, FALSE, pTaxiWayItem) ;
                        pNode = new repControlTreeNodeData(repControlTreeNodeType_TaxiwayTo);
                        m_treePaxType.SetItemData(PToNode,(DWORD_PTR)pNode) ;


                        newItem = m_treePaxType.InsertItem(_T("End"), cni, FALSE, FALSE, PToNode) ;
                        nodedata = new repControlTreeNodeData(repControlTreeNodeType_TaxiIntersection) ;
                        nodedata->nClassification = -1 ;
                        m_treePaxType.SetItemData(newItem,(DWORD_PTR)nodedata) ;
                    }
                    else
                    {
                        ASSERT(pTYData);
                        HTREEITEM PFromNode = m_treePaxType.InsertItem(_T("From"), cni, FALSE, FALSE, pTaxiWayItem) ;
                        repControlTreeNodeData* pNode = new repControlTreeNodeData(repControlTreeNodeType_TaxiWayFrom);
                        m_treePaxType.SetItemData(PFromNode,(DWORD_PTR)pNode) ;


                        HTREEITEM newItem = m_treePaxType.InsertItem(pTYData->at(i).m_strFromName, cni, FALSE, FALSE, PFromNode) ;
                        repControlTreeNodeData* nodedata = new repControlTreeNodeData(repControlTreeNodeType_TaxiIntersection) ;
                        nodedata->nClassification = pTYData->at(i).m_FormNodeID ;
                        m_treePaxType.SetItemData(newItem,(DWORD_PTR)nodedata) ;


                        HTREEITEM PToNode = m_treePaxType.InsertItem(_T("To"), cni, FALSE, FALSE, pTaxiWayItem) ;
                        pNode = new repControlTreeNodeData(repControlTreeNodeType_TaxiwayTo);
                        m_treePaxType.SetItemData(PToNode,(DWORD_PTR)pNode) ;


                        newItem = m_treePaxType.InsertItem(pTYData->at(i).m_strToName, cni, FALSE, FALSE, PToNode) ;
                        nodedata = new repControlTreeNodeData(repControlTreeNodeType_TaxiIntersection) ;
                        nodedata->nClassification =pTYData->at(i).m_ToNodeID ;
                        m_treePaxType.SetItemData(newItem,(DWORD_PTR)nodedata) ;
                    }
                    m_treePaxType.Expand(pTaxiWayItem,TVE_EXPAND) ;
                }
            }
        }
	break;
	case Airside_RunwayExit:
        {
            COOLTREE_NODE_INFO cni;
            CCoolTree::InitNodeInfo(this,cni);
			CRunwayExitParameter* pPara = (CRunwayExitParameter*)m_pParameter;
			if (m_hRootReportingArea)
			{
				RunwayExitRepDefineData* showdata = pPara->GetShowData() ;
				if (showdata->m_data.size() == 0)
				{
					HTREEITEM hRunwayItem = m_treePaxType.InsertItem(_T("All"), cni, FALSE, FALSE, m_hRootReportingArea) ;
					m_treePaxType.SetItemData(hRunwayItem,(DWORD_PTR)new repControlTreeNodeData(repControlTreeNodeType_Exit_Runway,-1));


					HTREEITEM hExits = m_treePaxType.InsertItem(_T("Exits"), cni, FALSE, FALSE, hRunwayItem) ;
					repControlTreeNodeData* pNodedata  = new repControlTreeNodeData(repControlTreeNodeType_Exit_ItemRoot,0) ;
					m_treePaxType.SetItemData(hExits,(DWORD_PTR)pNodedata);

			
					HTREEITEM exitItem = m_treePaxType.InsertItem(_T("All"), cni, FALSE, FALSE, hExits) ;
					 pNodedata  = new repControlTreeNodeData(repControlTreeNodeType_Exit_Item,0) ;
					m_treePaxType.SetItemData(exitItem,(DWORD_PTR)pNodedata);

                    cni.net = NET_COMBOBOX;
					HTREEITEM pOperITem = m_treePaxType.InsertItem(_T("Operation::All"), cni, FALSE, FALSE, hRunwayItem);
					pNodedata  = new repControlTreeNodeData(repControlTreeNodeType_Exit_Oper,0);
					m_treePaxType.SetItemData(pOperITem,(DWORD_PTR)pNodedata) ;

					m_treePaxType.Expand(hRunwayItem,TVE_EXPAND) ;
					m_treePaxType.Expand(hExits,TVE_EXPAND) ;
				}
				else
				{
					for (int i = 0; i < (int)showdata->m_data.size(); i++)
					{
						AddRunwayItem(showdata->m_data[i]) ;
					}
				}
				m_treePaxType.Expand(m_hRootReportingArea,TVE_EXPAND) ;
			}
		}
		break;
	case Airside_FlightFuelBurning:
		{
            COOLTREE_NODE_INFO cni;
            CCoolTree::InitNodeInfo(this,cni);
			CAirsideFlightFuelBurnParmater* pPara = (CAirsideFlightFuelBurnParmater*)m_pParameter;
			HTREEITEM hFltTypeItem =  m_hRootFlightType;
			if (hFltTypeItem == NULL)
				return;
			repControlTreeNodeData *pData = reinterpret_cast<repControlTreeNodeData *>(m_treePaxType.GetItemData(hFltTypeItem));
			if (pData == NULL)
				return;
			if (pData->nodeType == repControlTreeNodeType_FltTypeRoot)
			{
				HTREEITEM hDefaultItem;
				if (pPara->getFlightConstraintCount() == 0)
				{
					HTREEITEM hDefaultItem = m_treePaxType.InsertItem("DEFAULT", cni, FALSE, FALSE, hFltTypeItem, TVI_LAST);
					m_treePaxType.SetItemData(hDefaultItem,(DWORD_PTR)new repControlTreeNodeData(repControlTreeNodeType_FltType,FlightConstraint()));
				}
				else
				{
					for (int i = 0; i < (int)pPara->getFlightConstraintCount(); i++)
					{
						CString strFlightType;
						pPara->getFlightConstraint(i).screenPrint(strFlightType.GetBuffer(1024));
						strFlightType.ReleaseBuffer();
						hDefaultItem = m_treePaxType.InsertItem(strFlightType, cni, FALSE, FALSE, hFltTypeItem,TVI_LAST);
						m_treePaxType.SetItemData(hDefaultItem,(DWORD_PTR)new repControlTreeNodeData(repControlTreeNodeType_FltType,pPara->getFlightConstraint(i)));
					}
				}
			}
			m_treePaxType.Expand(hFltTypeItem,TVE_EXPAND);

			HTREEITEM hFromToItem = m_treePaxType.GetNextSiblingItem(m_hRootFlightType);
			HTREEITEM hFromItem = m_treePaxType.GetChildItem(hFromToItem);
			HTREEITEM hStartNode;
			if (pPara->GetFromNode().GetNodeName().IsEmpty())
			{
				hStartNode = m_treePaxType.InsertItem("All", cni, FALSE, FALSE, hFromItem);
			}
			else
			{
				hStartNode = m_treePaxType.InsertItem(pPara->GetFromNode().GetNodeName(), cni, FALSE, FALSE, hFromItem);
			}
			m_treePaxType.SetItemData(hStartNode,(DWORD_PTR)new repControlTreeNodeData(repControlTreeNodeType_FromNode,pPara->GetFromNode()));

			HTREEITEM hToItem = m_treePaxType.GetNextSiblingItem(hFromItem);
			HTREEITEM hEndNode;
			if (pPara->GetToNode().GetNodeName().IsEmpty())
			{
				hEndNode = m_treePaxType.InsertItem("All", cni, FALSE, FALSE, hToItem);;
			}
			else
			{
				hEndNode = m_treePaxType.InsertItem(pPara->GetToNode().GetNodeName(), cni, FALSE, FALSE, hToItem);
			}
			m_treePaxType.SetItemData(hEndNode,(DWORD_PTR)new repControlTreeNodeData(repControlTreeNodeType_ToNode,pPara->GetToNode()));

			m_treePaxType.Expand(hFromToItem,TVE_EXPAND);
			m_treePaxType.Expand(hFromItem,TVE_EXPAND);
			m_treePaxType.Expand(hToItem,TVE_EXPAND);
		}
		break;
	case Airside_VehicleOperation:
        {
            COOLTREE_NODE_INFO cni;
            CCoolTree::InitNodeInfo(this,cni);
			CAirsideVehicleOperParameter* pPara = (CAirsideVehicleOperParameter*)m_pParameter;
			HTREEITEM hFltTypeItem =  m_hRootFlightType;
			if (hFltTypeItem == NULL)
				return;
			repControlTreeNodeData *pData = reinterpret_cast<repControlTreeNodeData *>(m_treePaxType.GetItemData(hFltTypeItem));
			if (pData == NULL)
				return;
			if (pData->nodeType == repControlTreeNodeType_FltTypeRoot)
			{
				HTREEITEM hDefaultItem;
				if (pPara->getFlightConstraintCount() == 0)
				{
					HTREEITEM hDefaultItem = m_treePaxType.InsertItem("DEFAULT", cni, FALSE, FALSE, hFltTypeItem,TVI_LAST);
					m_treePaxType.SetItemData(hDefaultItem,(DWORD_PTR)new repControlTreeNodeData(repControlTreeNodeType_FltType,FlightConstraint()));
				}
				else
				{
					for (int i = 0; i < (int)pPara->getFlightConstraintCount(); i++)
					{
						CString strFlightType;
						pPara->getFlightConstraint(i).screenPrint(strFlightType.GetBuffer(1024));
						strFlightType.ReleaseBuffer();
						hDefaultItem = m_treePaxType.InsertItem(strFlightType, cni, FALSE, FALSE, hFltTypeItem,TVI_LAST);
						m_treePaxType.SetItemData(hDefaultItem,(DWORD_PTR)new repControlTreeNodeData(repControlTreeNodeType_FltType,pPara->getFlightConstraint(i)));
					}
				}
			}
			m_treePaxType.Expand(hFltTypeItem,TVE_EXPAND);

			HTREEITEM hVehicleItem = m_treePaxType.GetNextSiblingItem(m_hRootFlightType);
			int count = pPara->GetVehicleType()->size() ;
			if(count == 0)
			{
				HTREEITEM newItem = m_treePaxType.InsertItem(_T("All Type"), cni, FALSE, FALSE, hVehicleItem) ;
				m_treePaxType.SetItemData(newItem,(DWORD_PTR)new repControlTreeNodeData(repControlTreeNodeType_VehicleTypeNode));
			}
			for (int i = 0; i < (int)count; i++)
			{
				CString strVehicleType = pPara->GetVehicleType()->at(i);
				HTREEITEM newItem = m_treePaxType.InsertItem(strVehicleType, cni, FALSE, FALSE, hVehicleItem) ;
				m_treePaxType.SetItemData(newItem,(DWORD_PTR)new repControlTreeNodeData(repControlTreeNodeType_VehicleTypeNode));
			}

			HTREEITEM hPoolItem = m_treePaxType.GetNextSiblingItem(hVehicleItem);

			count =  pPara->GetPoolIdList().size() ;
			if(count == 0)
			{
				HTREEITEM newItem = m_treePaxType.InsertItem(_T("All"), cni, FALSE, FALSE, hPoolItem) ;
				repControlTreeNodeData* reportNode =  new repControlTreeNodeData(repControlTreeNodeType_PoolNode) ;
				reportNode->m_Data = -1;
				m_treePaxType.SetItemData(newItem,(DWORD_PTR)reportNode);
			}
			for (int i = 0; i < (int)pPara->GetPoolIdList().size(); i++)
			{
				ALTObject altObj;
				ALTObject* Pobj = NULL ;
				Pobj = altObj.ReadObjectByID(pPara->GetPoolIdList().at(i));
				CString str ;
				if(Pobj)
					str = Pobj->GetObjNameString() ;
				else
					str = _T("All") ;
				HTREEITEM newItem = m_treePaxType.InsertItem(str, cni, FALSE, FALSE, hPoolItem) ;
				repControlTreeNodeData* reportNode =  new repControlTreeNodeData(repControlTreeNodeType_PoolNode) ;
				reportNode->m_Data = pPara->GetPoolIdList().at(i);
				m_treePaxType.SetItemData(newItem,(DWORD_PTR)reportNode);
			}
		}
		break;
	default:
        {
            COOLTREE_NODE_INFO cni;
            CCoolTree::InitNodeInfo(this,cni);
			HTREEITEM hFltTypeItem =  m_treePaxType.GetRootItem();
			if (hFltTypeItem == NULL)
				return;
			repControlTreeNodeData *pData = reinterpret_cast<repControlTreeNodeData *>(m_treePaxType.GetItemData(hFltTypeItem));
			if (pData == NULL)
				return;
			if (pData->nodeType == repControlTreeNodeType_FltTypeRoot)
			{
				HTREEITEM hDefaultItem;
				if (m_pParameter->getFlightConstraintCount() == 0)
				{
					HTREEITEM hDefaultItem = m_treePaxType.InsertItem("DEFAULT", cni, FALSE, FALSE, hFltTypeItem,TVI_LAST);
					m_treePaxType.SetItemData(hDefaultItem,(DWORD_PTR)new repControlTreeNodeData(repControlTreeNodeType_FltType,FlightConstraint()));
				}
				else
				{
					for (int i = 0; i < (int)m_pParameter->getFlightConstraintCount(); i++)
					{
						CString strFlightType;
						m_pParameter->getFlightConstraint(i).screenPrint(strFlightType.GetBuffer(1024));
						strFlightType.ReleaseBuffer();
						hDefaultItem = m_treePaxType.InsertItem(strFlightType, cni, FALSE, FALSE, hFltTypeItem,TVI_LAST);
						m_treePaxType.SetItemData(hDefaultItem,(DWORD_PTR)new repControlTreeNodeData(repControlTreeNodeType_FltType,m_pParameter->getFlightConstraint(i)));
					}
				}
			}
			m_treePaxType.Expand(hFltTypeItem,TVE_EXPAND);
		}
		break;
	}
}
void CAirsideRepControlView::InitDefaultData()
{

}
BOOL CAirsideRepControlView::SaveData()
{
	//start time ,end time
	UpdateData();

	m_pParameter->SetProjID(GetDocument()->GetProjectID());
	m_pParameter->SetUnitManager(UNITSMANAGER);
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
	m_pParameter->setInterval( time.asSeconds() );
	
	//summary or detail
	if (m_radioDetailed.GetCheck())
	{
		m_pParameter->setReportType(ASReportType_Detail);
	}
	else
	{
		m_pParameter->setReportType(ASReportType_Summary);
	}


	//
	if(GetReportType() == Airside_RunwayUtilization || 
		GetReportType() == Airside_RunwayDelay || 
		GetReportType() == Airside_RunwayCrossings ||
		GetReportType() == Airside_IntersectionOperation|| 
		GetReportType() == Airside_TaxiwayUtilization ||
		GetReportType() == Airside_StandOperations ||
		GetReportType() == Airside_ControllerWorkload ||
		GetReportType() == Airside_TakeoffProcess)//tree performer handle the message

	{
		if(GetTreePerformer())
			 GetTreePerformer()->SaveData();
		return TRUE;
	}

	//flight type
	HTREEITEM hFltTypeItemRoot =  m_hRootFlightType;
	if (hFltTypeItemRoot != NULL)
	{
		HTREEITEM hFltTypeItem = m_treePaxType.GetChildItem(hFltTypeItemRoot);

		m_pParameter->ClearFlightConstraint();
		while (hFltTypeItem != NULL)
		{
			repControlTreeNodeData *pData = reinterpret_cast<repControlTreeNodeData *>(m_treePaxType.GetItemData(hFltTypeItem));
			m_pParameter->AddFlightConstraint(pData->fltCons);
			hFltTypeItem = m_treePaxType.GetNextSiblingItem(hFltTypeItem);
		}

	}
//nodes
	//HTREEITEM hNodeRoot = m_treePaxType.GetNextSiblingItem(hFltTypeItemRoot);
	//if (hNodeRoot == NULL)
	//	return TRUE;

	CTermPlanDoc *pTermPlanDoc = GetDocument();	
	ASSERT(pTermPlanDoc);
	reportType rpType = pTermPlanDoc->GetARCReportManager().GetAirsideReportManager()->GetReportType();
	switch(rpType)
	{
	case Airside_NodeDelay:
		{
			CAirsideNodeDelayReportParameter *pParam = (CAirsideNodeDelayReportParameter *)m_pParameter;

			if (pParam->getFlightConstraintCount() < 1)
			{
				MessageBox(_T("Must select one flight type at least."), _T("Warning"), MB_OK);
				return FALSE;
			}

			pParam->ClearReportNode();

			HTREEITEM hNodeItem = m_treePaxType.GetChildItem(m_hRootNodes);

			while (hNodeItem != NULL)
			{
				repControlTreeNodeData *pData = reinterpret_cast<repControlTreeNodeData *>(m_treePaxType.GetItemData(hNodeItem));
				pParam->AddReportNode(pData->nodeObject);
				hNodeItem = m_treePaxType.GetNextSiblingItem(hNodeItem);
			}

			if(pParam->getReportNodeCount() < 1)
			{
				MessageBox("Must select one report node at least. ",_T("Airside Report"),MB_OK);
				return FALSE;
			}
		}
		break;

	case Airside_FlightDelay:
		{
			CAirsideFlightDelayParam *pParam = (CAirsideFlightDelayParam *)m_pParameter;

			if (pParam->getFlightConstraintCount() < 1)
			{
				MessageBox(_T("Must select one flight type at least."), _T("Warning"), MB_OK);
				return FALSE;
			}

			HTREEITEM hFromItemRoot = m_treePaxType.GetChildItem(m_hRootNodeToNode);

			HTREEITEM hFromNodeItem = m_treePaxType.GetChildItem(hFromItemRoot);

			repControlTreeNodeData *pFromData = reinterpret_cast<repControlTreeNodeData *>(m_treePaxType.GetItemData(hFromNodeItem));
			pParam->SetStartNode(pFromData->nodeObject);

			HTREEITEM hToItemRoot = m_treePaxType.GetNextSiblingItem(hFromItemRoot);

			HTREEITEM hToNodeItem = m_treePaxType.GetChildItem(hToItemRoot);

			repControlTreeNodeData *pToData = reinterpret_cast<repControlTreeNodeData *>(m_treePaxType.GetItemData(hToNodeItem));
			pParam->SetEndNode(pToData->nodeObject);			
		}
		break;

	case Airside_FlightActivity:
	case Airside_AircraftOperation:
		{
			if (m_pParameter->getFlightConstraintCount() < 1)
			{
				MessageBox(_T("Must select one flight type at least."), _T("Warning"), MB_OK);
				return FALSE;
			}
		}
		break;
	case Airside_AircraftOperational:
		{
			CFlightOperationalParam *pParam = (CFlightOperationalParam *)m_pParameter;

			if (pParam->getFlightConstraintCount() < 1)
			{
				MessageBox(_T("Must select one flight type at least."), _T("Warning"), MB_OK);
				return FALSE;
			}

			HTREEITEM hFromItemRoot = m_treePaxType.GetChildItem(m_treePaxType.GetNextSiblingItem(m_hRootFlightType));

			HTREEITEM hFromNodeItem = m_treePaxType.GetChildItem(hFromItemRoot);

			repControlTreeNodeData *pFromData = reinterpret_cast<repControlTreeNodeData *>(m_treePaxType.GetItemData(hFromNodeItem));
			pParam->SetStartNode(pFromData->nodeObject);

			HTREEITEM hToItemRoot = m_treePaxType.GetNextSiblingItem(hFromItemRoot);

			HTREEITEM hToNodeItem = m_treePaxType.GetChildItem(hToItemRoot);

			repControlTreeNodeData *pToData = reinterpret_cast<repControlTreeNodeData *>(m_treePaxType.GetItemData(hToNodeItem));
			pParam->SetEndNode(pToData->nodeObject);
		}
		break;
	case  Airside_VehicleOperation:
		{
			CAirsideVehicleOperParameter* pParam = (CAirsideVehicleOperParameter *)m_pParameter;
			
			pParam->clearVehicleType() ;
			pParam->clearVehiclePool() ;
			if (pParam->getFlightConstraintCount() < 1)
			{
				MessageBox(_T("Must select one flight type at least."), _T("Warning"), MB_OK);
				return FALSE;
			}

			//////////////////////////////////////////////////////////////////////////
			HTREEITEM VehicleTypeitem = NULL ;
			VehicleTypeitem = m_treePaxType.GetNextSiblingItem(hFltTypeItemRoot) ;
			while(VehicleTypeitem != NULL && strcmp(m_treePaxType.GetItemText(VehicleTypeitem),_T("Vehicle Type")) != 0)
			{
				VehicleTypeitem = m_treePaxType.GetNextSiblingItem(VehicleTypeitem) ;
			}
			SetVehicleType(pParam,VehicleTypeitem) ;

			//////////////////////////////////////////////////////////////////////////
			HTREEITEM PoolItem = NULL ;
			PoolItem = m_treePaxType.GetNextSiblingItem(hFltTypeItemRoot) ;
			while (PoolItem != NULL && strcmp(m_treePaxType.GetItemText(PoolItem),_T("Pool")) != 0)
			{
				PoolItem = m_treePaxType.GetNextSiblingItem(PoolItem) ;
			}

			SetPoolID(pParam,PoolItem) ;
			
		}
		break ;
	case Airside_RunwayOperaitons:
		{
			AirsideRunwayOperationReportParam *pParam = (AirsideRunwayOperationReportParam *)m_pParameter;

			//flight type
			if (pParam->getFlightConstraintCount() < 1)
			{
				MessageBox(_T("Must select one flight type at least."), _T("Warning"), MB_OK);
				return FALSE;
			}
			

			//RunwayMark
			{
				pParam->ClearRunwayMark();
				HTREEITEM hRunwayItem = m_treePaxType.GetChildItem(m_hRootRunways);
				while(hRunwayItem != NULL)
				{
					repControlTreeNodeData *pNodeData = (repControlTreeNodeData *)m_treePaxType.GetItemData(hRunwayItem);
					if(pNodeData != NULL && pNodeData->nodeType == repControlTreeNodeType_RunwayMarkNode)
					{
						if(pNodeData->runwayMark.m_nRunwayID == -1)//all
						{
							pParam->ClearRunwayMark();

							pParam->m_vRunwayParameter.push_back(pNodeData->runwayMark);

							//pParam->AddRunwayMark(pNodeData->runwayMark);
							//get all runway mark
							
							std::vector< ALTObject> vRunwayObjct;		
							try
							{
								CString strSQL = _T("");

								CString strSelect = _T("");
								strSelect.Format(_T("SELECT ID, APTID, NAME_L1, NAME_L2, NAME_L3, NAME_L4, LOCKED\
													FROM         ALTOBJECT\
													WHERE     (TYPE = %d) AND (APTID IN\
													(SELECT     ID\
													FROM          ALTAIRPORT\
													WHERE      (PROJID = %d)))"),ALT_RUNWAY,GetDocument()->GetProjectID());


								long nRecordCount = -1;
								CADORecordset adoRecordset;
								CADODatabase::ExecuteSQLStatement(strSelect,nRecordCount,adoRecordset);
								while(!adoRecordset.IsEOF())
								{
									ALTObject object;
									int nObjID = -1;
									adoRecordset.GetFieldValue(_T("ID"),nObjID);
									object.ReadObject(adoRecordset);
									object.setID(nObjID);

									vRunwayObjct.push_back(object);

									adoRecordset.MoveNextData();
								}
							}
							catch (CADOException& e)
							{
								e.ErrorMessage();
							}

							//add runway node
							for (int nRunway = 0; nRunway < static_cast<int>(vRunwayObjct.size()); ++ nRunway)
							{
								int nObjID = vRunwayObjct.at(nRunway).getID();
								Runway* pRunway = (Runway *)ALTObject::ReadObjectByID(nObjID);
								if(pRunway != NULL)
								{
									CAirsideReportRunwayMark runwayMark1;
									CString strMark1 = CString(pRunway->GetMarking1().c_str());
									runwayMark1.SetRunway(pRunway->getID(),RUNWAYMARK_FIRST,strMark1);

		
									CAirsideReportRunwayMark runwayMark2;
									CString strMark2 = CString(pRunway->GetMarking2().c_str());
									runwayMark2.SetRunway(pRunway->getID(),RUNWAYMARK_SECOND,strMark2);

									pParam->AddRunwayMark(runwayMark1);
									pParam->AddRunwayMark(runwayMark2);

								}
							}
							
							
							break;
						}

						pParam->AddRunwayMark(pNodeData->runwayMark);
						pParam->m_vRunwayParameter.push_back(pNodeData->runwayMark);
					}

					hRunwayItem = m_treePaxType.GetNextSiblingItem(hRunwayItem);
				}
			}


			pParam->ClearClassification();
			//classification
			HTREEITEM hClassItem = m_treePaxType.GetChildItem(m_hRootClassification);
			while(hClassItem != NULL)
			{
				repControlTreeNodeData *pNodeData = (repControlTreeNodeData *)m_treePaxType.GetItemData(hClassItem);
				if(pNodeData != NULL && pNodeData->nodeType == repControlTreeNodeType_ClassificationNode)
				{
					pParam->AddClassification(pNodeData->nClassification);
				}

				hClassItem = m_treePaxType.GetNextSiblingItem(hClassItem);
			}
		}
		break;
	case Airside_FlightConflict:
		{
			AirsideFlightConflictPara *pParam = (AirsideFlightConflictPara *)m_pParameter;
			pParam->ClearAreas();
			//classification
			HTREEITEM hAreaItem = m_treePaxType.GetChildItem(m_hRootReportingArea);
			while(hAreaItem != NULL)
			{
				repControlTreeNodeData *pNodeData = (repControlTreeNodeData *)m_treePaxType.GetItemData(hAreaItem);
				if(pNodeData != NULL && pNodeData->nodeType == repControlTreeNodeType_ReportingAreaNode)
				{
					pParam->AddArea(pNodeData->objID);
				}

				hAreaItem = m_treePaxType.GetNextSiblingItem(hAreaItem);
			}

			//HTREEITEM hSectorItem = m_treePaxType.GetChildItem(m_hRootSector);
			//while(hSectorItem != NULL)
			//{
			//	repControlTreeNodeData *pNodeData = (repControlTreeNodeData *)m_treePaxType.GetItemData(hSectorItem);
			//	if(pNodeData != NULL && pNodeData->nodeType == repControlTreeNodeType_SectorNode)
			//	{
			//		pParam->AddArea(pNodeData->objID);
			//	}

			//	hSectorItem = m_treePaxType.GetNextSiblingItem(hSectorItem);
			//}
		}
		break;
	case Airside_TaxiwayDelay:
		{
			CTaxiwayDelayParameters* taxiwayDelayPar = (CTaxiwayDelayParameters *)m_pParameter;
			taxiwayDelayPar->cleardata() ;
			HTREEITEM rootItem = m_treePaxType.GetRootItem() ;
			HTREEITEM ptaxiwayItem = m_treePaxType.GetChildItem(rootItem) ;
			while(ptaxiwayItem)
			{
				if (!SetTaxiwayItem(ptaxiwayItem,taxiwayDelayPar))
					return FALSE;

				ptaxiwayItem = m_treePaxType.GetNextSiblingItem(ptaxiwayItem) ;
			}
		}
		break ;
	case Airside_RunwayExit:
		{
			CRunwayExitParameter* runwayExitPar = (CRunwayExitParameter*)m_pParameter ;
			runwayExitPar->ClearData();
			HTREEITEM rootItem = m_treePaxType.GetRootItem() ;
			HTREEITEM ptaxiwayItem = m_treePaxType.GetChildItem(rootItem) ;
			while(ptaxiwayItem)
			{
				SetRunwayExitReportNode(ptaxiwayItem,runwayExitPar) ;
				ptaxiwayItem = m_treePaxType.GetNextSiblingItem(ptaxiwayItem) ;
			}
			runwayExitPar->GetDataFromShowData() ;
		}
		break ;
	case Airside_FlightFuelBurning:
		{
			//flight type

			if (m_pParameter->getFlightConstraintCount() < 1)
			{
				MessageBox(_T("Must select one flight type at least."), _T("Warning"), MB_OK);
				return FALSE;
			}
			CAirsideFlightFuelBurnParmater* flightFuelBurnParmeter = (CAirsideFlightFuelBurnParmater*)m_pParameter ;
			repControlTreeNodeData* pNodedata = NULL ;
			HTREEITEM hNode = m_treePaxType.GetRootItem() ;
			while(hNode)
			{
				pNodedata = (repControlTreeNodeData*)m_treePaxType.GetItemData(hNode) ;
				if(pNodedata->nodeType == repControlTreeNodeType_FromToNode)
				{
					HTREEITEM HnodeItem = m_treePaxType.GetChildItem(hNode) ;
					if(HnodeItem)
					{
						SetFlightFuelBurnReportNodeData(HnodeItem,flightFuelBurnParmeter) ;
					}
				}
				hNode = m_treePaxType.GetNextSiblingItem(hNode) ;
			}
		}
	default:
		{
			break;
		}
	}

	return TRUE;
}

bool CAirsideRepControlView::SetTaxiwayItem(HTREEITEM _item,CTaxiwayDelayParameters* _taxiwayParameter)
{
	if(_item == NULL || _taxiwayParameter == NULL)
		return false;

     repControlTreeNodeData *pToData = reinterpret_cast<repControlTreeNodeData *>(m_treePaxType.GetItemData(_item));
	 if(pToData != NULL)
	 {

		 HTREEITEM FromNode = m_treePaxType.GetChildItem(_item) ;
		 CString formNodeName = m_treePaxType.GetItemText(m_treePaxType.GetChildItem(FromNode)) ;
		 HTREEITEM TChildNode = m_treePaxType.GetChildItem(FromNode) ;
		 int fromid = -1 ;
		 int toid = -1 ;
		 if(TChildNode != NULL)
			 fromid = ((repControlTreeNodeData*)m_treePaxType.GetItemData(TChildNode))->nClassification ;

		 HTREEITEM ToNode =  m_treePaxType.GetNextSiblingItem(FromNode) ;
		 CString ToNodeName = m_treePaxType.GetItemText(m_treePaxType.GetChildItem(ToNode)) ;

		 TChildNode = m_treePaxType.GetChildItem(ToNode) ;
		toid  = ((repControlTreeNodeData*)m_treePaxType.GetItemData(TChildNode))->nClassification ;

		 CString taxiwayname  = m_treePaxType.GetItemText(_item) ;
		 if (taxiwayname.CompareNoCase(_T("ALL")) == 0)
		 {
			 taxiwayname = _T("");
		 }
		 ALTObjectGroup altGroup;
		 altGroup.setName(ALTObjectID(taxiwayname));
		 altGroup.SetProjID(GetDocument()->GetProjectID()) ;
		 altGroup.setType(ALT_TAXIWAY) ;
		 std::vector<ALTObject>  objects ;
		 altGroup.GetObjects(objects) ;
		 for(int i = 0 ; i < (int)objects.size() ;i++)
		 {
			 ALTObject taxiway = objects[i] ;
			CTaxiwaySelect* taxiwaySel =   _taxiwayParameter->AddTaxiway(taxiway.getID()) ;
			taxiwaySel->SetTaxiwayName(taxiway.GetObjNameString()) ;
			
			if (toid == -1 && fromid > -1)
			{
				MessageBox(_T("You should define the end node as a specific node otherwise cannot confirm the end position!"),_T("Warning"),MB_OK) ;
				return false;
			}

			if (toid > -1 && fromid == -1)
			{
				MessageBox(_T("You should define the start node as a specific node otherwise cannot confirm the start position!"),_T("Warning"),MB_OK) ;
				return false;
			}

			if(toid != -1 && fromid != -1)
				taxiwaySel->AddIntersectionPair(fromid,formNodeName,toid,ToNodeName) ;
		 }

		 //taxiway for show
		CTaxiwaySelect* pTaxiwayForShow = _taxiwayParameter->AddTaxiwayForShow(pToData->nClassification);
		pTaxiwayForShow->SetTaxiwayName(taxiwayname);
		if (toid != -1 && fromid != -1)
		{
			pTaxiwayForShow->AddIntersectionPair(fromid,formNodeName,toid,ToNodeName);
		}
	 }
	 return true;
}
void CAirsideRepControlView::SetVehicleType(CAirsideVehicleOperParameter* pParam , HTREEITEM _item)
{
	if(pParam == NULL || _item == NULL)
		return ;
	HTREEITEM subItem = m_treePaxType.GetChildItem(_item) ;
	CString vehicleTy ;
	while(subItem)
	{
		vehicleTy = m_treePaxType.GetItemText(subItem) ;
		if(vehicleTy.CompareNoCase(_T("All Type")) == 0) 
		{
			subItem = m_treePaxType.GetNextSiblingItem(subItem) ;
			continue;
		}
		pParam->AddVehicleType(vehicleTy) ;
		subItem = m_treePaxType.GetNextSiblingItem(subItem) ;
	}
}
void CAirsideRepControlView::SetPoolID(CAirsideVehicleOperParameter* pParam , HTREEITEM _item)
{
	if(pParam == NULL || _item == NULL)
		return ;
	HTREEITEM subItem = m_treePaxType.GetChildItem(_item) ;
	int PoolID = -1 ;
	CString poolName ;
	while(subItem)
	{
		poolName = m_treePaxType.GetItemText(subItem) ;
		if(poolName.CompareNoCase(_T("All")) == 0)
		{
			subItem = m_treePaxType.GetNextSiblingItem(subItem) ;
			continue ;
		}
		repControlTreeNodeData* pData =(repControlTreeNodeData *) m_treePaxType.GetItemData(subItem);
		if(pData != NULL)
			pParam->AddPoolId(pData->m_Data) ;
		subItem = m_treePaxType.GetNextSiblingItem(subItem) ;
	}
}
void CAirsideRepControlView::OnButtonApply()
{
	if(SaveData() == FALSE)
		return;
	try
	{
		int iCurrentSimIdx = GetDocument()->GetTerminal().getCurrentSimResult();
		if( iCurrentSimIdx < 0 )
		{
			AfxMessageBox("No Valid SimResult!", MB_OK|MB_ICONERROR);
			return;
		}

		GetDocument()->GetAirsideSimLogs().GetSimReportManager()->SetCurrentSimResult( iCurrentSimIdx );

		GetDocument()->GetARCReportManager().GetAirsideReportManager()->GenerateResult();
		//if run multiple run report, need reset current run index
		GetDocument()->GetTerminal().setCurrentSimResult( iCurrentSimIdx );
		GetDocument()->UpdateAllViews(this,AIRSIDEREPORT_SHOWREPORT,NULL);
	}
	catch(StringError* pErr)
	{
		AfxMessageBox("The airside log file is old, run simulation please!");
		delete pErr;		
	}

}
void CAirsideRepControlView::OnButtonCancel()
{

}
void CAirsideRepControlView::OnUpdateToolBarAdd(CCmdUI *pCmdUI)
{

	if(GetReportType() == Airside_RunwayUtilization|| 
		GetReportType() == Airside_RunwayDelay || 
		GetReportType() == Airside_RunwayCrossings ||
		GetReportType() == Airside_IntersectionOperation|| 
		GetReportType() == Airside_TaxiwayUtilization||
		GetReportType() == Airside_StandOperations ||
		GetReportType() == Airside_ControllerWorkload||
		GetReportType() == Airside_TakeoffProcess)//tree performer handle the message

	{
		if(GetTreePerformer())
			GetTreePerformer()->OnUpdateToolBarAdd(pCmdUI);
		return;
	}


	HTREEITEM hSelectItem = m_treePaxType.GetSelectedItem();
	m_hSelectItem = hSelectItem ;
	if (hSelectItem != NULL)
	{
		repControlTreeNodeData *pData = reinterpret_cast<repControlTreeNodeData *>(m_treePaxType.GetItemData(hSelectItem));
		if (pData != NULL)
		{
			if (pData->nodeType == repControlTreeNodeType_FromRoot||
				pData->nodeType == repControlTreeNodeType_ToRoot ||
				pData->nodeType == repControlTreeNodeType_NodesRoot ||
				pData->nodeType == repControlTreeNodeType_FltTypeRoot ||
				pData->nodeType == repControlTreeNodeType_VehicleType||
				pData->nodeType == repControlTreeNodeType_ClassificationRoot ||
				pData->nodeType == repControlTreeNodeType_RunwayMarkRoot||
				pData->nodeType == repControlTreeNodeType_ReportingAreaRoot ||
				//pData->nodeType == repControlTreeNodeType_SectorRoot||
				pData->nodeType == repControlTreeNodeType_Pool||
				pData->nodeType == repControlTreeNodeType_RunwayMarkRoot||
				pData->nodeType == repControlTreeNodeType_Taxiways||
				pData->nodeType == repControlTreeNodeType_TaxiWayFrom||
				pData->nodeType == repControlTreeNodeType_TaxiwayTo||
				pData->nodeType == repControlTreeNodeType_RunwayExits||
				pData->nodeType == repControlTreeNodeType_Exit_ItemRoot||
				pData->nodeType == repControlTreeNodeType_Exit_Oper)
			{
				pCmdUI->Enable(TRUE);
				return;
			}
		}	

	}

	pCmdUI->Enable(FALSE);
}

void CAirsideRepControlView::OnUpdateToolBarDel(CCmdUI *pCmdUI)
{


	if(GetReportType() == Airside_RunwayUtilization||
		GetReportType() == Airside_RunwayDelay  ||
		GetReportType() == Airside_RunwayCrossings ||
		GetReportType() == Airside_IntersectionOperation||
		GetReportType() == Airside_TaxiwayUtilization||
		GetReportType() == Airside_StandOperations ||
		GetReportType() == Airside_ControllerWorkload ||
		GetReportType() == Airside_TakeoffProcess 
		)//tree performer handle the message
	{
		if(GetTreePerformer())
			GetTreePerformer()->OnUpdateToolBarDel(pCmdUI);
		return;
	}

	HTREEITEM hSelectItem = m_treePaxType.GetSelectedItem();
	if (hSelectItem != NULL)
	{
		repControlTreeNodeData *pData = reinterpret_cast<repControlTreeNodeData *>(m_treePaxType.GetItemData(hSelectItem));
		if (pData != NULL)
		{
			if (pData->nodeType == repControlTreeNodeType_FromNode ||
				pData->nodeType == repControlTreeNodeType_ToNode ||
				pData->nodeType == repControlTreeNodeType_Node ||
				pData->nodeType == repControlTreeNodeType_FltType||
				pData->nodeType == repControlTreeNodeType_PoolNode||
				pData->nodeType == repControlTreeNodeType_VehicleTypeNode||
				pData->nodeType == repControlTreeNodeType_ClassificationNode||
				pData->nodeType == repControlTreeNodeType_RunwayMarkNode||
				pData->nodeType == repControlTreeNodeType_Taxiway||
				pData->nodeType == repControlTreeNodeType_TaxiIntersection||
				pData->nodeType == repControlTreeNodeType_RunwayMarkNode||
				pData->nodeType == repControlTreeNodeType_ReportingAreaNode||
				pData->nodeType == repControlTreeNodeType_Exit_Runway||
				pData->nodeType == repControlTreeNodeType_Exit_Item/* ||
				pData->nodeType == repControlTreeNodeType_SectorNode*/)
			{
				pCmdUI->Enable(TRUE);
				return;
			}
		}

	}
		
	pCmdUI->Enable(FALSE);
}
void CAirsideRepControlView::UpdateControlContent()
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

	if(m_pParameter->getReportType() == ASReportType_Detail)
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

void CAirsideRepControlView::LoadData()
{
	m_pParameter = GetDocument()->GetARCReportManager().GetAirsideReportManager()->GetParameters();

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

	if(m_pParameter->getReportType() == ASReportType_Detail)
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
	//m_dtctrlInterval.SetTime( oleInterval );
	m_oleDTInterval = oleInterval;

	UpdateData(FALSE);
}

LRESULT CAirsideRepControlView::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
    if(message == UM_CEW_COMBOBOX_BEGIN)
    {
        reportType repType = GetReportType();
        if(repType == Airside_RunwayUtilization|| 
            repType == Airside_RunwayDelay ||
            repType == Airside_IntersectionOperation ||
            repType == Airside_TaxiwayUtilization ||
            repType == Airside_ControllerWorkload)//tree performer handle the message
        {
            if(GetTreePerformer())
                GetTreePerformer()->DefWindowProc(message, wParam, lParam);
        }
        else if(GetReportType() == Airside_RunwayExit)
        {
            HTREEITEM hSelItem = (HTREEITEM)wParam;
            CWnd* pWnd= m_treePaxType.GetEditWnd(hSelItem);
            CComboBox* pCB=(CComboBox*)pWnd;
            pCB->ResetContent();
            pCB->SetDroppedWidth(120);
            repControlTreeNodeData *pData = reinterpret_cast<repControlTreeNodeData *>(m_treePaxType.GetItemData(hSelItem));
            ASSERT(pData);
            if(pData->nodeType == repControlTreeNodeType_Exit_Oper)
            {
                pCB->AddString(_T("All"));
                pCB->AddString(_T("T/o"));
                pCB->AddString(_T("Landing"));
                int curSel = pData->nClassification;
                pCB->SetCurSel(curSel);
            }
        }
    }
    else if(message == UM_CEW_COMBOBOX_SELCHANGE)
    {
        HTREEITEM hSelItem = (HTREEITEM)wParam;
        CWnd* pWnd= m_treePaxType.GetEditWnd(hSelItem);
        CComboBox* pCB=(CComboBox*)pWnd;
        if(GetReportType() == Airside_RunwayUtilization|| 
            GetReportType() == Airside_RunwayDelay ||
            GetReportType() == Airside_IntersectionOperation ||
            GetReportType() == Airside_TaxiwayUtilization ||
            GetReportType() == Airside_ControllerWorkload)//tree performer handle the message
        {
            if(GetTreePerformer())
                GetTreePerformer()->DefWindowProc(message, wParam, lParam);
        }
        else if(GetReportType() == Airside_RunwayExit)
        {
            if(m_hSelectItem != NULL)
            {
                int curesel = pCB->GetCurSel();
                repControlTreeNodeData* PNodeData = (repControlTreeNodeData*)m_treePaxType.GetItemData(m_hSelectItem);
                if(PNodeData->nodeType == repControlTreeNodeType_Exit_Oper && curesel != PNodeData->nClassification)
                {
                    PNodeData->nClassification = curesel ;
                    UpdateExitOperationItem(m_hSelectItem,curesel) ;
                }
            }

        }
    }
    else if (message == UM_CEW_EDITSPIN_END || message == UM_CEW_EDITSPIN_BEGIN)
    {
        if(GetReportType() == Airside_ControllerWorkload)
        {
            if(GetTreePerformer())
                GetTreePerformer()->DefWindowProc(message,wParam,lParam);
        }
    }
    else if (message == WM_LBUTTONDBLCLK)
    {
        if(AirsideReControlView::CTreePerformer * pPerfrom = GetTreePerformer())
            pPerfrom->OnTreeItemDoubleClick((HTREEITEM)wParam);
    }
    else if(message == UM_CEW_STATUS_CHANGE)
    {
        if(GetReportType() == Airside_FlightDelay)
        {
            HTREEITEM hSelItem = (HTREEITEM)wParam;
            repControlTreeNodeData* pNodeData = (repControlTreeNodeData*)m_treePaxType.GetItemData(hSelItem);
            if(pNodeData->nodeType == repControlTreeNodeType_MultiRunRoot)
            {
                m_pParameter->SetEnableMultiRun(!m_pParameter->GetEnableMultiRun());
            }
            else if(pNodeData->nodeType == repControlTreeNodeType_Runs)
            {
                if(m_treePaxType.IsCheckItem(hSelItem))
                {
                    m_pParameter->AddReportRuns(pNodeData->m_Data);
                }
                else
                {
                    m_pParameter->RemoveReportRuns(pNodeData->m_Data);
                }
            }
        }
    }

    return CFormView::DefWindowProc(message, wParam, lParam);
}

AirsideReControlView::CTreePerformer * CAirsideRepControlView::GetTreePerformer()
{
	CTermPlanDoc *pTermPlanDoc = GetDocument();	
	if(pTermPlanDoc == NULL)
		return NULL;

	if(m_pParameter == NULL)
		return NULL;

	reportType rpType = pTermPlanDoc->GetARCReportManager().GetAirsideReportManager()->GetReportType();
	switch(rpType)
	{
	case Airside_RunwayUtilization:
		{
			if(m_pTreePerformer && m_pTreePerformer->GetType() == Airside_RunwayUtilization)
			{
				m_pTreePerformer->SetParamter(m_pParameter);
				return m_pTreePerformer;
			}
			else if(m_pTreePerformer)
			{
				delete m_pTreePerformer;
				m_pTreePerformer = NULL;
			}
			m_pTreePerformer = new AirsideReControlView::CRunwayUtilizationTreePerformer(pTermPlanDoc->GetProjectID(),&m_treePaxType,m_pParameter);
			return m_pTreePerformer;
		}
		break;
	case Airside_RunwayDelay:
		{
			if(m_pTreePerformer && m_pTreePerformer->GetType() == Airside_RunwayDelay)
			{
				m_pTreePerformer->SetParamter(m_pParameter);
				return m_pTreePerformer;
			}
			else if(m_pTreePerformer)
			{
				delete m_pTreePerformer;
				m_pTreePerformer = NULL;
			}

			m_pTreePerformer = new AirsideReControlView::CRunwayDelayParaTreePerformer(pTermPlanDoc->GetProjectID(),&m_treePaxType,m_pParameter);
			return m_pTreePerformer;
		}
		break;
	case Airside_RunwayCrossings:
		{
			if (m_pTreePerformer && m_pTreePerformer->GetType() == Airside_RunwayCrossings)
			{
				m_pTreePerformer->SetParamter(m_pParameter);
				return m_pTreePerformer;
			}
			else if(m_pTreePerformer)
			{
				delete m_pTreePerformer;
				m_pTreePerformer = NULL;
			}
			m_pTreePerformer = new AirsideReControlView::CRunwayCrossingsTreePerformer(pTermPlanDoc->GetProjectID(),&m_treePaxType,m_pParameter);
			return m_pTreePerformer;
		}
		break;
	case Airside_IntersectionOperation:
		{
			if (m_pTreePerformer && m_pTreePerformer->GetType() == Airside_IntersectionOperation)
			{
				m_pTreePerformer->SetParamter(m_pParameter);
				return m_pTreePerformer;
			}
			else if(m_pTreePerformer)
			{
				delete m_pTreePerformer;
				m_pTreePerformer = NULL;
			}
			m_pTreePerformer = new AirsideReControlView::CAirsideIntersectionTreePerformer(pTermPlanDoc->GetProjectID(),&m_treePaxType,m_pParameter);
			return m_pTreePerformer;
		}
		break;
	case Airside_TaxiwayUtilization:
		{
			if (m_pTreePerformer && m_pTreePerformer->GetType() == Airside_TaxiwayUtilization)
			{
				m_pTreePerformer->SetParamter(m_pParameter);
				return m_pTreePerformer;
			}
			else if(m_pTreePerformer)
			{
				delete m_pTreePerformer;
				m_pTreePerformer = NULL;
			}
			m_pTreePerformer = new AirsideReControlView::CAirsideTaxiwayUtilizationTreePerformer(pTermPlanDoc->GetProjectID(),&m_treePaxType,m_pParameter);
			return m_pTreePerformer;
		}
		break;
		
	case Airside_StandOperations:
		{
			if (m_pTreePerformer && m_pTreePerformer->GetType() == Airside_StandOperations)
			{
				m_pTreePerformer->SetParamter(m_pParameter);
				return m_pTreePerformer;
			}
			else if (m_pTreePerformer)
			{
				delete m_pTreePerformer;
				m_pTreePerformer = NULL;
			}
			m_pTreePerformer = new AirsideReControlView::CStandOperationsTreePerformer(pTermPlanDoc->GetProjectID(),&m_treePaxType,m_pParameter);
			return m_pTreePerformer;
		}
		break;
	case Airside_TakeoffProcess:
		{
			if (m_pTreePerformer && m_pTreePerformer->GetType() == Airside_TakeoffProcess)
			{
				m_pTreePerformer->SetParamter(m_pParameter);
				return m_pTreePerformer;
			}
			else if (m_pTreePerformer)
			{
				delete m_pTreePerformer;
				m_pTreePerformer = NULL;
			}
			m_pTreePerformer = new AirsideReControlView::CTakeoffProcessTreePerformer(pTermPlanDoc->GetProjectID(),&m_treePaxType,m_pParameter);
			return m_pTreePerformer;
		}
	case Airside_ControllerWorkload:
		{

			if (m_pTreePerformer && m_pTreePerformer->GetType() == Airside_ControllerWorkload)
			{
				m_pTreePerformer->SetParamter(m_pParameter);
				return m_pTreePerformer;
			}
			else if (m_pTreePerformer)
			{
				delete m_pTreePerformer;
				m_pTreePerformer = NULL;
			}
			m_pTreePerformer = new AirsideReControlView::CAirsideControllerWorkloadTreePerformer(pTermPlanDoc->GetProjectID(),&m_treePaxType,m_pParameter);
			return m_pTreePerformer;
		}
		break;
	default:
		{
			ASSERT(0);
		}
	}

	return NULL;

}
reportType CAirsideRepControlView::GetReportType()
{
	CTermPlanDoc *pTermPlanDoc = GetDocument();	
	if(pTermPlanDoc == NULL)
		return UnknownType;
	reportType rpType = pTermPlanDoc->GetARCReportManager().GetAirsideReportManager()->GetReportType();
	return rpType;
}
BOOL CAirsideRepControlView::IsTaxiwaySelect(HTREEITEM _htaxiwayRootItem,CString _SelTaxi) 
{
	if(_htaxiwayRootItem == NULL )
		return FALSE ;
	HTREEITEM childitem = m_treePaxType.GetChildItem(_htaxiwayRootItem) ;
	CString taxiwayName ;
	HTREEITEM PdeletItem = NULL ;
	while(childitem)
	{
		taxiwayName = m_treePaxType.GetItemText(childitem) ;
		if(taxiwayName.CompareNoCase(_SelTaxi) == 0)
			return TRUE ;
		if(taxiwayName.CompareNoCase("ALL") == 0)
			PdeletItem = childitem ;
		childitem = m_treePaxType.GetNextSiblingItem(childitem) ;
	}
	if(PdeletItem != NULL)
		m_treePaxType.DeleteItem(PdeletItem) ;
	return FALSE ;
}

void CAirsideRepControlView::AddRunwayExitReportRunwayItem(HTREEITEM _rootItem)
{
	int nProjID = ALTAirport::GetProjectIDByName(((CTermPlanDoc *)GetDocument())->m_ProjInfo.name);
	CDlgSelectAirsideNodeByType dlgRunWaySel(nProjID,AIRSIDE_RUNWAY,this) ;
	HTREEITEM child = m_treePaxType.GetChildItem(_rootItem) ;
	while(child)
	{
		if(m_treePaxType.GetItemText(child).CompareNoCase("All") == 0)
		{
		m_treePaxType.DeleteItem( child );
			break ;
		}
		child = m_treePaxType.GetNextSiblingItem(child) ;
	}
	if(dlgRunWaySel.DoModal() == IDOK)
	{
		CString runwayname ;
		runwayname = dlgRunWaySel.GetSelectNode().GetNodeName() ;
		if(runwayname.IsEmpty())
		{
			MessageBox(_T("You should select one runway group"),MB_OK) ;
			return ;
		}

        COOLTREE_NODE_INFO cni;
        CCoolTree::InitNodeInfo(this,cni);
		HTREEITEM HRunwayItem = m_treePaxType.InsertItem(runwayname, cni, FALSE, FALSE, _rootItem);
		m_treePaxType.SetItemData(HRunwayItem,(DWORD_PTR)new repControlTreeNodeData(repControlTreeNodeType_Exit_Runway,-1));


		HTREEITEM exits = m_treePaxType.InsertItem(_T("Exits"), cni, FALSE, FALSE, HRunwayItem) ;
		repControlTreeNodeData* pNodedata  = new repControlTreeNodeData(repControlTreeNodeType_Exit_ItemRoot,0) ;
		m_treePaxType.SetItemData(exits,(DWORD_PTR)pNodedata);

		HTREEITEM exitItem = m_treePaxType.InsertItem(_T("All"), cni, FALSE, FALSE, exits) ;
		 pNodedata  = new repControlTreeNodeData(repControlTreeNodeType_Exit_Item,0) ;
		m_treePaxType.SetItemData(exitItem,(DWORD_PTR)pNodedata);

        cni.net = NET_COMBOBOX;
		HTREEITEM pOperITem = m_treePaxType.InsertItem(_T("Operation::All"), cni, FALSE, FALSE, HRunwayItem);
		pNodedata  = new repControlTreeNodeData(repControlTreeNodeType_Exit_Oper,0) ;
		m_treePaxType.SetItemData(pOperITem,(DWORD_PTR)pNodedata) ;
		m_treePaxType.Expand(HRunwayItem,TVE_EXPAND);
	}
}
void CAirsideRepControlView::AddRunwayExitReportExitItem(HTREEITEM _RunwayItem)
{
	if(_RunwayItem == NULL)
		return ;
	CString taxiwayname  = m_treePaxType.GetItemText(m_treePaxType.GetParentItem(_RunwayItem) );
		std::vector<ALTObject>  objects ;
	if(taxiwayname.CompareNoCase("All") == 0)
	{
		int nProjID = ALTAirport::GetProjectIDByName(((CTermPlanDoc *)GetDocument())->m_ProjInfo.name);
		std::vector<int> vAirportIds;
		InputAirside::GetAirportList(nProjID, vAirportIds);
		for (std::vector<int>::iterator iterAirportID = vAirportIds.begin(); iterAirportID != vAirportIds.end(); ++iterAirportID)
		{
			ALTAirport airport;
			airport.ReadAirport(*iterAirportID);
			airport.getName();

			ALTObjectList vObject;
			ALTAirport::GetAirsideResourceObject(*iterAirportID,vObject,AIRSIDE_RUNWAY);

			for (int i = 0 ; i < (int)vObject.size() ;i++)
			{
				ALTObject* altObject = vObject.at(i).get();
				objects.push_back(*altObject) ;
			}
		}
	}else{
		ALTObjectGroup altGroup;
		altGroup.setName(ALTObjectID(taxiwayname));
		altGroup.SetProjID(GetDocument()->GetProjectID()) ;
		altGroup.setType(ALT_RUNWAY) ;;
		altGroup.GetObjects(objects) ;
	}

	HTREEITEM PExitItem = NULL ;
	CDlgRunwayExitSelect dlgRunwayExit(&objects,false, this) ;
	repControlTreeNodeData* pNodedata = NULL ;
	if(dlgRunwayExit.DoModal() == IDOK)
    {
        COOLTREE_NODE_INFO cni;
        CCoolTree::InitNodeInfo(this,cni);
		std::vector<CSelectData>::iterator iter = dlgRunwayExit.GetSelectExitName()->begin() ;
		for ( ; iter != dlgRunwayExit.GetSelectExitName()->end() ;iter++)
		{
			if(CheckTheRunwayExitIsSelect(_RunwayItem,(*iter).m_ExitID))
				continue ;

			PExitItem = m_treePaxType.InsertItem((*iter).m_ExitName, cni, FALSE, FALSE, _RunwayItem) ;
			pNodedata = new repControlTreeNodeData(repControlTreeNodeType_Exit_Item,(*iter).m_ExitID) ;
			pNodedata->nodeObject.SetNodeName((*iter).m_RunwayName) ;
			pNodedata->nodeObject.SetNodeID((*iter).m_RunwayID) ;
			m_treePaxType.SetItemData(PExitItem,(DWORD_PTR)pNodedata);
		}
	}
}
void CAirsideRepControlView::UpdateExitOperationItem(HTREEITEM _Item , int _type)
{
	if(_Item == NULL)
		return ;
	CString OperName;
	switch(_type)
	{
	case 0 :
		OperName.Format(_T("%s"),_T("All")) ;
		break ;
	case 1 :
		OperName.Format(_T("%s"),_T("T/o")) ;
		break;
	case 2:
		OperName.Format(_T("%s"),_T("Landing")) ;
		break ;
	}
	CString newName ;
	newName.Format(_T("%s:%s"),"Operation",OperName) ;
	m_treePaxType.SetItemText(_Item,newName) ;
}
void CAirsideRepControlView::SetRunwayExitReportNode(HTREEITEM _item , CRunwayExitParameter* _Parm)
{
	if(_item == NULL || _Parm == NULL)
		return ;
	CString ItemName ;
	ItemName = m_treePaxType.GetItemText(_item) ;
	
	repControlTreeNodeData* pNodedata = NULL ;
	HTREEITEM pChildItem = m_treePaxType.GetChildItem(_item) ;
	int Operation = 0 ;
	HTREEITEM HExitsItem = NULL ;
	int oper = 0 ;
	while(pChildItem)
	{
		pNodedata = (repControlTreeNodeData*)m_treePaxType.GetItemData(pChildItem) ;
		if(pNodedata->nodeType == repControlTreeNodeType_Exit_Oper)
		{
			oper = (int)pNodedata->nClassification ;
		}else
			HExitsItem = pChildItem ;
		pChildItem = m_treePaxType.GetNextSiblingItem(pChildItem) ;
	}
	RunwayExitRepDefineData_Runway* runwayData = _Parm->GetShowData()->AddRunway(ItemName,oper) ;
	pChildItem = m_treePaxType.GetChildItem(HExitsItem) ;
	CString exitname ;
	while(pChildItem)
	{
			exitname = m_treePaxType.GetItemText(pChildItem) ;
			if(exitname.CompareNoCase("All") == 0)
				break ;
			pNodedata = (repControlTreeNodeData*)m_treePaxType.GetItemData(pChildItem) ;
		
			runwayData->AddExitName(exitname,pNodedata->nodeObject.GetNodeName(),pNodedata->nClassification,pNodedata->nodeObject.GetNodeID()) ;
			pChildItem = m_treePaxType.GetNextSiblingItem(pChildItem) ;
	}
}
BOOL CAirsideRepControlView::CheckTheRunwayExitIsSelect(HTREEITEM _item , int _id)
{
	if(_item == NULL || _id == -1)
		return FALSE ;
	repControlTreeNodeData* pNodedata = NULL ;
	HTREEITEM pChildItem = m_treePaxType.GetChildItem(_item) ;
	HTREEITEM pDelItem = NULL ;
	while(pChildItem)
	{
		if(m_treePaxType.GetItemText(pChildItem).CompareNoCase("All") == 0)
		{
			pDelItem = pChildItem ;
			pChildItem = m_treePaxType.GetNextSiblingItem(pChildItem) ;
			continue ;
		}
		pNodedata = (repControlTreeNodeData*)m_treePaxType.GetItemData(pChildItem) ;
		if(pNodedata->nClassification == _id)
			return TRUE ;
		pChildItem = m_treePaxType.GetNextSiblingItem(pChildItem) ;
	}
	if(pChildItem != NULL)
	m_treePaxType.DeleteItem(pDelItem) ;
	return FALSE ;
}

void CAirsideRepControlView::SetFlightFuelBurnReportNodeData( HTREEITEM _NodeData , CAirsideFlightFuelBurnParmater* _Parm )
{
	if(_NodeData == NULL || _Parm == NULL)
		return ;
	HTREEITEM HFromNodeData = m_treePaxType.GetChildItem(_NodeData) ;

	repControlTreeNodeData* pFromNodeData = NULL ;
	repControlTreeNodeData* pToNodeData = NULL ;

	if(HFromNodeData != NULL)
	{
		pFromNodeData = (repControlTreeNodeData*)m_treePaxType.GetItemData(HFromNodeData) ;

		_Parm->AddFromNode(pFromNodeData->nodeObject) ;
	}


	HTREEITEM HToNode = m_treePaxType.GetNextSiblingItem(_NodeData) ;

	HTREEITEM htoNodeData = m_treePaxType.GetChildItem(HToNode) ;

	if(htoNodeData != NULL)
	{
		pToNodeData = (repControlTreeNodeData*)m_treePaxType.GetItemData(htoNodeData) ;

		_Parm->AddToNode(pToNodeData->nodeObject) ;
	}
}


void CAirsideRepControlView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint)
{
	if(lHint == VM_GLOBALUNIT_CHANGER)
	{
		CAirsideBaseReport * baseReport = GetDocument()->GetARCReportManager().GetAirsideReportManager()->GetReport() ;
		if(baseReport->IsReportGenerate() == TRUE)
			GetDocument()->UpdateAllViews(this,AIRSIDEREPORT_SHOWREPORT,NULL);
	}

	if (lHint == AIRSIDEREPORT_SHOWREPORT)
	{
		UpdateControlContent();
	}
}
void CAirsideRepControlView::AddRunwayItem(RunwayExitRepDefineData_Runway* _runwayData)
{

    COOLTREE_NODE_INFO cni;
    CCoolTree::InitNodeInfo(this,cni);
	HTREEITEM hRunwayItem = m_treePaxType.InsertItem(_runwayData->m_RunwayName, cni, FALSE, FALSE, m_hRootReportingArea);
	repControlTreeNodeData* pNodedata  = new repControlTreeNodeData(repControlTreeNodeType_Exit_Runway,0) ;
	m_treePaxType.SetItemData(hRunwayItem,(DWORD_PTR)pNodedata);

	CString strOperation(_T(""));
	strOperation.Format(_T("Operation::%s"),CRunwayExitItem::GetOperationName((CRunwayExitItem::Operation_type)_runwayData->m_Operater));
	HTREEITEM pOperITem = m_treePaxType.InsertItem(strOperation, cni, FALSE, FALSE, hRunwayItem);
	 pNodedata  = new repControlTreeNodeData(repControlTreeNodeType_Exit_Oper,0) ;
	m_treePaxType.SetItemData(pOperITem,(DWORD_PTR)pNodedata);
	
	HTREEITEM exits = m_treePaxType.InsertItem(_T("Exits"), cni, FALSE, FALSE, hRunwayItem) ;
	 pNodedata  = new repControlTreeNodeData(repControlTreeNodeType_Exit_ItemRoot,0) ;
	m_treePaxType.SetItemData(exits,(DWORD_PTR)pNodedata);

	if((int)_runwayData->m_data.size() == 0)
	{
		HTREEITEM exitItem = m_treePaxType.InsertItem(_T("All"), cni, FALSE, FALSE, exits) ;
		repControlTreeNodeData* pNodedata  = new repControlTreeNodeData(repControlTreeNodeType_Exit_Item,0) ;
		m_treePaxType.SetItemData(exitItem,(DWORD_PTR)pNodedata);
	}
	for (int i = 0 ; i < (int)_runwayData->m_data.size() ;i++)
	{
		AddExitItem(_runwayData->m_data[i],exits) ;
	}
m_treePaxType.Expand(hRunwayItem,TVE_EXPAND) ;
}
void CAirsideRepControlView::AddExitItem(RunwayExitRepDefineData_Exit& exitItem ,HTREEITEM _item)
{
    COOLTREE_NODE_INFO cni;
    CCoolTree::InitNodeInfo(this,cni);
	HTREEITEM hExitItem = m_treePaxType.InsertItem(exitItem.m_ExitName, cni, FALSE, FALSE, _item) ;
	repControlTreeNodeData* pNodedata = new repControlTreeNodeData(repControlTreeNodeType_Exit_Item,exitItem.m_ExitID) ;
	pNodedata->nodeObject.SetNodeName(exitItem.m_runwayName) ;
	pNodedata->nodeObject.SetNodeID(exitItem.m_RunwayID) ;
	m_treePaxType.SetItemData(hExitItem,(DWORD_PTR)pNodedata);
	m_treePaxType.Expand(hExitItem,TVE_EXPAND) ;
}
