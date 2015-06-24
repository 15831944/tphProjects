// RepGraphView.cpp : implementation file
//

#include "stdafx.h"

#include "chart\vcplot.h"
#include "chart\vctitle.h"
#include "chart\vcfont.h"  
#include "chart\vcaxis.h"
#include "chart\vcaxistitle.h"
#include "chart\vcSeriesCollection.h"
#include "chart\VcSeries.h"
#include "chart\VcPen.h"
#include "chart\vccolor.h"
#include "chart\vcdatapoints.h"
#include "chart\vcdatapoint.h"
#include "chart\vcbrush.h"
#include "chart\vcfootnote.h"    
#include "chart\vcAxisGrid.h"
#include <comdef.h>

#include "termplan.h"
#include "RepGraphView.h"
#include "../common/UnitsManager.h"
#include "RepListView.h"
#include "reports\ReportParameter.h"

#include "RepGraphViewSingleReportOperator.h"
#include "RepGraphViewMultiRunReportOperator.h"
#include ".\repgraphview.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRepGraphView

IMPLEMENT_DYNCREATE(CRepGraphView, CFormView)

CRepGraphView::CRepGraphView()
: CFormView(CRepGraphView::IDD)
{
	//{{AFX_DATA_INIT(CRepGraphView)
	//}}AFX_DATA_INIT
	m_pRepGraphViewOperator = NULL;
}

CRepGraphView::~CRepGraphView()
{
	delete m_pRepGraphViewOperator;
	m_pRepGraphViewOperator = NULL;
}

void CRepGraphView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRepGraphView)
	DDX_Control(pDX, IDC_STATIC_TOOLBARCONTENTER, m_toolbarcontenter);
	DDX_Control(pDX, IDC_CHART_TYPE_COMBO, m_comboChartType);
	DDX_Control(pDX, IDC_CHART_SELECT_COMBO, m_comboChartSelect);
	DDX_Control(pDX, IDC_MSCHART1, m_MSChartCtrl);
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_REPORT_SUBTYPE, m_comSubType);
}


BEGIN_MESSAGE_MAP(CRepGraphView, CFormView)
//{{AFX_MSG_MAP(CRepGraphView)
ON_WM_SIZE()
ON_BN_CLICKED(IDC_SEL_COLOR_BTN, OnSelColorBtn)
ON_CBN_SELCHANGE(IDC_CHART_TYPE_COMBO, OnSelchangeChartTypeCombo)
ON_CBN_SELCHANGE(IDC_CHART_SELECT_COMBO, OnSelchangeChartSelectCombo)
	ON_BN_CLICKED(IDC_PRINT_BTN, OnPrintBtn)
	ON_WM_CREATE()
//	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_GRAPHVIEW_MAXIMAX, OnGraphviewMaximax)
	ON_COMMAND(ID_GRAPHVIEW_NORMAL, OnGraphviewNormal)
	//}}AFX_MSG_MAP
	ON_CBN_SELCHANGE(IDC_REPORT_SUBTYPE, OnCbnSelChangeReportSubType)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRepGraphView diagnostics

#ifdef _DEBUG
void CRepGraphView::AssertValid() const
{
	CFormView::AssertValid();
}

void CRepGraphView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CRepGraphView message handlers

void CRepGraphView::OnSize(UINT nType, int cx, int cy) 
{
	CFormView::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here
	if(IsWindowVisible())
	{
		CRect rcClient;
		GetClientRect(&rcClient);
		CRect rcChart;
		rcChart.right  = rcClient.right - 10;
		rcChart.left   = rcClient.left+10;
		rcChart.top    = rcClient.top+50;
		rcChart.bottom = rcClient.bottom;
		
		m_MSChartCtrl.MoveWindow(rcChart);
	}
}

void CRepGraphView::OnSelColorBtn() 
{
	// TODO: Add your control notification handler code here
//	CColorDialog colordlg(m_ColorRef, CC_ANYCOLOR | CC_FULLOPEN);
//	if(colordlg.DoModal()==IDOK)
//	{
//		m_ColorRef=colordlg.GetColor();
//		CVcSeries series=((CVcPlot)m_MSChartCtrl.GetPlot()).GetSeriesCollection().GetItem(1);
//		CVcDataPoint datapoint=series.GetDataPoints().GetItem(-1);
//		CVcBrush brush=datapoint.GetBrush();
//		brush.SetStyle(1);
//		brush.GetFillColor().Set(GetRValue(m_ColorRef),GetGValue(m_ColorRef),GetBValue(m_ColorRef));
//		m_MSChartCtrl.Refresh();
//	}
	USES_CONVERSION;
	IOleObject* pObj = NULL;
	IUnknown* pUnk = NULL;
	ISpecifyPropertyPages* pSpecify = NULL;
	CAUUID pages;
	CWnd* pChart = GetDlgItem(IDC_MSCHART1);
	// Get Property Page CLSID's
	pUnk = pChart->GetControlUnknown();
	if(FAILED(pUnk->QueryInterface(IID_ISpecifyPropertyPages,(void**)&pSpecify)))	
	{
		// TRACE("Failed to get ISpecifyPropertyPages interface!");	
		return;
	}
	pages.cElems = 0;
	pages.pElems = NULL;
	if (FAILED(pSpecify->GetPages(&pages)))
	{
		// TRACE("Files to get GetPages!");	
		pSpecify->Release();
		return;
	}
	// display the control's property pages.        
	OleCreatePropertyFrame(
		m_hWnd,                          // Parent window of the property frame
		0,                               // Horizontal position of the property frame
		0,                               // Vertical position of the property frame
		T2OLE("MSChart"),                // Property frame caption
		1,                               // Number of objects
		&pUnk,                           // Array of IUnknown pointers for objects
		pages.cElems,                    // Number of property pages
		pages.pElems,                    // Array of CLSIDs for property pages
		GetUserDefaultLCID(),            // Locale identifier
		0,                               // Reserved - 0
		NULL                             // Reserved - 0
		);
	// clean up
	pSpecify->Release();
	CoTaskMemFree(pages.pElems);
}

void CRepGraphView::OnSelchangeChartTypeCombo() 
{
	// TODO: Add your control notification handler code here
	int nCurSel=m_comboChartType.GetCurSel();
	if(nCurSel<0)
	{
		return;
	}
	switch(nCurSel)
	{
	case 0:  //3D Bar
		m_MSChartCtrl.SetChartType(0);
		break;
	case 1://2D Bar
		m_MSChartCtrl.SetChartType(1);
		break;
	case 2://2D Pie
		m_MSChartCtrl.SetChartType(14);
		break;
//	case 3://2D XY
//		m_MSChartCtrl.SetChartType(16);
//		break;
	case 3://3D Line
		m_MSChartCtrl.SetChartType(2);
		break;
	case 4://2D Line
		m_MSChartCtrl.SetChartType(3);
		break;
	case 5://3D Area
		m_MSChartCtrl.SetChartType(4);
		break;
	case 6://2D Area
		m_MSChartCtrl.SetChartType(5);
		break;
	case 7://3D Step
		m_MSChartCtrl.SetChartType(6);
		break;
	case 8://2D Step
		m_MSChartCtrl.SetChartType(7);
		break;
	case 9://3D Combination
		m_MSChartCtrl.SetChartType(8);
		break;
	case 10://2D Combination
		m_MSChartCtrl.SetChartType(9);
		break;
	}
	m_MSChartCtrl.Refresh();
}

void CRepGraphView::OnInitialUpdate() 
{
	CFormView::OnInitialUpdate();
	
	// TODO: Add your specialized code here and/or call the base class
	InitToolbar();
	//*****************Fill Chart Type Combox***********************
	m_comboChartType.AddString("3D Bar");
	m_comboChartType.AddString("2D Bar");
	m_comboChartType.AddString("2D Pie");
	//m_comboChartType.AddString("2D XY");
	m_comboChartType.AddString("3D Line");
	m_comboChartType.AddString("2D Line");
	m_comboChartType.AddString("3D Area");
	m_comboChartType.AddString("2D Area");
	m_comboChartType.AddString("3D Step");
	m_comboChartType.AddString("2D Step");
	m_comboChartType.AddString("3D Combination");
	m_comboChartType.AddString("2D Combination");
	//*****************End of fill chart type combox****************
//	m_MSChartCtrl.SetAllowSelections(FALSE);
	//m_MSChartCtrl.SetEnabled(FALSE);
	m_MSChartCtrl.ShowWindow(SW_HIDE);


	m_comSubType.ResetContent();
	//add sub type string
	m_comSubType.AddString(_T("Average"));
	m_comSubType.AddString(_T("Minimum"));
	m_comSubType.AddString(_T("Maximum"));
	m_comSubType.AddString(_T("Q1"));
	m_comSubType.AddString(_T("Q2"));
	m_comSubType.AddString(_T("Q3"));
	m_comSubType.AddString(_T("P1"));
	m_comSubType.AddString(_T("P5"));
	m_comSubType.AddString(_T("P10"));
	m_comSubType.AddString(_T("P90"));
	m_comSubType.AddString(_T("P95"));
	m_comSubType.AddString(_T("P99"));
	m_comSubType.AddString(_T("Sigma"));
	m_comSubType.SetCurSel(0);

	m_comSubType.ShowWindow(SW_HIDE);
	GetDlgItem(IDC_STATIC_SUBTYPE)->ShowWindow(SW_HIDE);

	
}

void CRepGraphView::OnPrint(CDC* pDC, CPrintInfo* pInfo) 
{
	// TODO: Add your specialized code here and/or call the base class
	//CFormView::OnPrint(pDC, pInfo);
}

void CRepGraphView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
{
	// TODO: Add your specialized code here and/or call the base class
	if(lHint & NODE_HINT_SELCHANGED)
	{
		return;
	}
	if(IsWindow(m_hWnd))
	{
		CTermPlanDoc *pTermPlanDoc = (CTermPlanDoc *)GetDocument();
		if (pTermPlanDoc)
		{
			if(pTermPlanDoc->GetARCReportManager().GetReportPara() == NULL)
				return;


			delete m_pRepGraphViewOperator;
			m_pRepGraphViewOperator = NULL;

			if(pTermPlanDoc->GetARCReportManager().GetReportPara()->IsMultiRunReport())
			{
				m_pRepGraphViewOperator = new CRepGraphViewMultiRunReportOperator(this);
				//init sub task combo box

				int nDetailOrSummary = 0;
				if(pTermPlanDoc->GetARCReportManager().GetReportPara()->GetReportType(nDetailOrSummary) &&
					nDetailOrSummary == 1 && 
					pTermPlanDoc->GetARCReportManager().GetReportType() != ENUM_AVGQUEUELENGTH_REP)
				{
					CRect rcChartSelect;
					m_comboChartSelect.GetWindowRect(&rcChartSelect);
					ScreenToClient(&rcChartSelect);

					//rc sub type static
					CRect rcStaticSubType(&rcChartSelect);
					rcStaticSubType.left = rcChartSelect.right+ 15;
					rcStaticSubType.right = rcStaticSubType.right + 80;
                    GetDlgItem(IDC_STATIC_SUBTYPE)->MoveWindow(rcStaticSubType);
                    GetDlgItem(IDC_STATIC_SUBTYPE)->ShowWindow(SW_SHOW);

					//rc sub type combo box
					CRect rcComboSubType(&rcStaticSubType);
					rcComboSubType.left = rcComboSubType.right + 3;
					rcComboSubType.right = rcComboSubType.right + 150;
					m_comSubType.MoveWindow(rcComboSubType);
					m_comSubType.ShowWindow(SW_SHOW);

					CRect rcStaticChartType(&rcComboSubType);
					rcStaticChartType.left = rcStaticChartType.right + 15;
					rcStaticChartType.right = rcStaticChartType.right + 80;
					GetDlgItem(IDC_STATIC_CHARTTYPE)->MoveWindow(&rcStaticChartType);
					GetDlgItem(IDC_STATIC_CHARTTYPE)->ShowWindow(SW_SHOW);

					CRect rcComboChartType(&rcStaticChartType);
					rcComboChartType.left = rcComboChartType.right + 3;
					rcComboChartType.right = rcComboChartType.right + 150;
					m_comboChartType.MoveWindow(rcComboChartType);
					m_comboChartType.ShowWindow(SW_SHOW);
				}
				else
				{
					CRect rcChartSelect;
					m_comboChartSelect.GetWindowRect(&rcChartSelect);
					ScreenToClient(&rcChartSelect);
	
					CRect rcStaticChartType(&rcChartSelect);
					rcStaticChartType.left = rcStaticChartType.right + 15;
					rcStaticChartType.right = rcStaticChartType.right + 80;
					GetDlgItem(IDC_STATIC_CHARTTYPE)->MoveWindow(&rcStaticChartType);
					GetDlgItem(IDC_STATIC_CHARTTYPE)->ShowWindow(SW_SHOW);

					CRect rcComboChartType(&rcStaticChartType);
					rcComboChartType.left = rcComboChartType.right + 3;
					rcComboChartType.right = rcComboChartType.right + 150;
					m_comboChartType.MoveWindow(rcComboChartType);
					m_comboChartType.ShowWindow(SW_SHOW);


                    GetDlgItem(IDC_STATIC_SUBTYPE)->ShowWindow(SW_HIDE);
					m_comSubType.ShowWindow(SW_HIDE);
				}
			
			}
			else
			{
				m_pRepGraphViewOperator = new CRepGraphViewSingleReportOperator(this);


				CRect rcChartSelect;
				m_comboChartSelect.GetWindowRect(&rcChartSelect);
				ScreenToClient(&rcChartSelect);
				CRect rcStaticChartType(&rcChartSelect);
				rcStaticChartType.left = rcStaticChartType.right + 15;
				rcStaticChartType.right = rcStaticChartType.right + 80;
				GetDlgItem(IDC_STATIC_CHARTTYPE)->MoveWindow(&rcStaticChartType);
				GetDlgItem(IDC_STATIC_CHARTTYPE)->ShowWindow(SW_SHOW);

				CRect rcComboChartType(&rcStaticChartType);
				rcComboChartType.left = rcComboChartType.right + 3;
				rcComboChartType.right = rcComboChartType.right + 150;
				m_comboChartType.MoveWindow(rcComboChartType);
				m_comSubType.ShowWindow(SW_SHOW);


				GetDlgItem(IDC_STATIC_SUBTYPE)->ShowWindow(SW_HIDE);
				m_comSubType.ShowWindow(SW_HIDE);


			}
		}
		if(m_pRepGraphViewOperator)
		{
			m_pRepGraphViewOperator->LoadReport(lHint,pHint);
		}
//		//m_MSChartCtrl.SetAllowSelections(FALSE);
//		if(((CTermPlanDoc *)GetDocument())->GetARCReportManager().GetReportPara()==NULL)
//			return;
//		
//		switch( ((CTermPlanDoc *)GetDocument())->GetARCReportManager().GetReportType() )
//		{
//		case ENUM_BAGDELIVTIME_REP:
//			m_nReportFileType = BagDeliveryTimeReportFile;
//			break;
//			
//		case ENUM_BAGDISTRIBUTION_REP:
//			m_nReportFileType = BagDistReportFile;
//			break;
//			
//		case ENUM_ACOPERATION_REP:
//			m_nReportFileType = AcOperationsReportFile;
//			break;
//			
//		case ENUM_PAXLOG_REP:
//			m_nReportFileType=PaxLogReportFile;
//			break;
//			
//		case ENUM_UTILIZATION_REP:
//			m_nReportFileType=UtilizationReportFile;
//			break;
//
//		case ENUM_DISTANCE_REP:
//			m_nReportFileType=DistanceReportFile;
//			break;
//
//		case ENUM_PAXCOUNT_REP:
//			m_nReportFileType=PassengerCountReportFile;
//			break;
//
//		case ENUM_QUEUETIME_REP:
//			m_nReportFileType=QueueTimeReportFile;
//			break;
//
//		case ENUM_THROUGHPUT_REP:
//			m_nReportFileType=ThroughputReportFile;
//			break;
//
//		case ENUM_QUEUELENGTH_REP:
//			m_nReportFileType=QueueLengthReportFile;
//			break;
//			
//		case ENUM_AVGQUEUELENGTH_REP:
//			m_nReportFileType=AverageQueueLengthReportFile;
//			break;
//
//		case ENUM_DURATION_REP:
//			m_nReportFileType=DurationReportFile;
//			break;
//
//		case ENUM_SERVICETIME_REP:
//			m_nReportFileType=ServiceTimeReportFile;
//			break;
//
//		case ENUM_ACTIVEBKDOWN_REP:
//			m_nReportFileType=ActivityBreakdownReportFile;
//			break;
//
//		case ENUM_BAGWAITTIME_REP:
//			m_nReportFileType=BagWaitTimeReportFile;
//			break;
//
//		case ENUM_BAGCOUNT_REP:
//			m_nReportFileType=BagCountReportFile;
//			break;
//
//		case ENUM_PAXDENS_REP:
//			m_nReportFileType=PassengerDensityReportFile;
//			break;
//
//		case ENUM_SPACETHROUGHPUT_REP:
//			m_nReportFileType = SpaceThroughputReportFile;
//			break;
//		case ENUM_COLLISIONS_REP:
//			m_nReportFileType = CollisionReportFile;	
//			break;
//		case ENUM_FIREEVACUTION_REP:
//			m_nReportFileType = FireEvacuationReportFile;
//			break;
//
//		case ENUM_CONVEYOR_WAIT_LENGTH_REP:
//			m_nReportFileType=ConveyorWaitLengthReport;
//			break;
//
//		case ENUM_CONVEYOR_WAIT_TIME_REP:
//			m_nReportFileType=ConveyorWaitTimeReport;
//			break;
//
//		case ENUM_MISSFLIGHT_REP:
//			m_nReportFileType = MissFlightReportFile;
//			break;
//
//		case ENUM_BILLBOARD_EXPOSUREINCIDENCE_REP:
//			m_nReportFileType = BillboardExposureIncidenceFile;
//			break;
//
//		case ENUM_BILLBOARD_LINKEDPROCINCREMENTVISIT_REP:
//			m_nReportFileType = BillboardLinkedProcIncrementlVisitFile;
//			break;
//		} 
//		
//		((CTermPlanDoc *)GetDocument())->GetARCReportManager().GetReportPara()->GetReportType( m_nSummary );
//
////matt modify 5,10,2004
//	
//		ArctermFile fFile;
//			CString sFileName;
//			if( GetLoadReportType() == load_by_user )
//			{
//				sFileName = ((CTermPlanDoc*)GetDocument())->GetARCReportManager().GetUserLoadReportFile();
//			}
//			else								// load_by_system
//			{
//				sFileName = ((CTermPlanDoc*)GetDocument())->GetTerminal().GetSimReportManager()->GetCurrentReportFileName( ((CTermPlanDoc*)GetDocument())->m_ProjInfo.path );
//			}
//	
//			try { fFile.openFile (sFileName, READ); }
//			catch (FileOpenError *exception)
//			{
//				delete exception;
//				//initDefaultValues();
//			//	saveDataSet(_pProjPath, false);
//				return;
//			}
//	
//
////matt modify
//		Load();
	}
}

//void CRepGraphView::Load()
//{
//	CWaitCursor wCur;
//
//	m_MSChartCtrl.SetRowCount(0);
//	m_MSChartCtrl.SetColumnCount(0);
//	m_MSChartCtrl.SetColumnLabelCount(0);
//	m_MSChartCtrl.SetChartType(1);
//	m_comboChartSelect.ResetContent();
//
//	switch( m_nReportFileType )
//	{
//	case BagDeliveryTimeReportFile:
//		{
//			if( m_nSummary==0 )
//			{
//				m_comboChartSelect.AddString("Bag Delivery Time(Detailed)");
//				m_comboChartSelect.SetCurSel(0);
//				m_MSChartCtrl.ShowWindow(SW_SHOW);
//				BagDeliveryCount();
//				m_MSChartCtrl.Refresh();
//				m_comboChartType.SetCurSel(1);
//				break;	
//			}
//			else
//			{
//				m_comboChartSelect.AddString("Bag Delivery Time(Summary)");
//				m_comboChartSelect.SetCurSel(0);
//				m_MSChartCtrl.ShowWindow(SW_SHOW);
//				BagDeliverySummary();
//				m_MSChartCtrl.Refresh();
//				m_comboChartType.SetCurSel(1);
//				break;				
//			}
//			break;
//		}
//	case BagDistReportFile:
//		{
//			m_comboChartSelect.AddString("Arrival Bags Count");
//			m_comboChartSelect.AddString("Depart Bags Count");
//			m_comboChartSelect.SetCurSel(0);
//			m_MSChartCtrl.ShowWindow(SW_SHOW);
//			BagArrivalDistCount();
//			m_MSChartCtrl.Refresh();
//			m_comboChartType.SetCurSel(1);
//			break;
//		}
//	case AcOperationsReportFile:
//		{
//			//m_comboChartSelect.AddString("Scheduled Arrival Time");
//			//m_comboChartSelect.AddString("Scheduled Depart Time");
//			m_comboChartSelect.AddString("Total");
//			m_comboChartSelect.SetCurSel(0);
//			m_MSChartCtrl.ShowWindow(SW_SHOW);
//			AcOpTotal();
//			m_MSChartCtrl.Refresh();
//			m_comboChartType.SetCurSel(1);
//			break;
//		}
//	case PaxLogReportFile:
//		{
////			if( m_nSummary!=0)
////			{
//				m_comboChartSelect.AddString("System Entry Distribution");
//				m_comboChartSelect.AddString("System Exit Distribution");
//				m_comboChartSelect.AddString("Group Size Distribution");
//				m_comboChartSelect.AddString("Bag Count Distribution");
//				m_comboChartSelect.AddString("Cart Count Distribution");
//				m_comboChartSelect.SetCurSel(0);
//				m_MSChartCtrl.ShowWindow(SW_SHOW);
//				PaxLogSysEntryDistribution();
//				m_MSChartCtrl.Refresh();
//				m_comboChartType.SetCurSel(1);
//				break;	
////			}
//			break;
//		}
//	case DistanceReportFile:
//		{
//			if( m_nSummary==0 )
//			{
//				m_comboChartSelect.AddString("Distance Travelled(Detailed)");
//				m_comboChartSelect.SetCurSel(0);
//				m_MSChartCtrl.ShowWindow(SW_SHOW);
//				DistanceTravelledDetailed();
//				m_MSChartCtrl.Refresh();
//				m_comboChartType.SetCurSel(1);
//				break;	
//			}
//			else
//			{
//				m_comboChartSelect.AddString("Distance Travelled(Summary)");
//				m_comboChartSelect.SetCurSel(0);
//				m_MSChartCtrl.ShowWindow(SW_SHOW);
//				DistanceTravelledSummary();
//				m_MSChartCtrl.Refresh();
//				m_comboChartType.SetCurSel(1);
//				break;				
//			}
//			break;
//		}
//	case FireEvacuationReportFile:
//		{
//			if( m_nSummary == 0)	// detailed
//			{
//				m_comboChartSelect.AddString("Fire Evacuation(Detailed)");
//				m_comboChartSelect.SetCurSel(0);
//				m_MSChartCtrl.ShowWindow( SW_SHOW );
//				FireEvacuationDetailed();
//				m_MSChartCtrl.Refresh();
//				m_comboChartType.SetCurSel(1);
//				break;
//			}
//			else					// summary
//			{
//				m_comboChartSelect.AddString("Fire Evacuation(Summary)");
//				m_comboChartSelect.SetCurSel(0);
//				m_MSChartCtrl.ShowWindow( SW_SHOW );
//				FireEvacuationSummary();
//				m_MSChartCtrl.Refresh();
//				m_comboChartType.SetCurSel(1);
//				break;
//			}
//			break;
//		}
//	case UtilizationReportFile:
//		{
//			if( m_nSummary==0 )
//			{
//				m_comboChartSelect.AddString("Utilization Time(Detailed)");
//				m_comboChartSelect.AddString("Time Available(Detailed)");
//				m_comboChartSelect.AddString("Utilization Percentage(Detailed)");
//				m_comboChartSelect.SetCurSel(0);
//				m_MSChartCtrl.ShowWindow(SW_SHOW);
//				UtilizationTimeDetailed();
//				m_MSChartCtrl.Refresh();
//				m_comboChartType.SetCurSel(1);
//				//AfxMessageBox("Detailed!");
//				break;	
//			}
//			else
//			{
//				//AfxMessageBox("Summary!");
//				m_comboChartSelect.AddString("Utilization Time(Summary)");
//				m_comboChartSelect.AddString("Time Available(Summary)");
//				m_comboChartSelect.AddString("Utilization Percentage(Summary)");
//				m_comboChartSelect.SetCurSel(0);
//				m_MSChartCtrl.ShowWindow(SW_SHOW);
//				UtilizationTimeSummary();
//				m_MSChartCtrl.Refresh();
//				m_comboChartType.SetCurSel(1);
//				break;				
//			}
//			break;
//		}
//	case DurationReportFile:
//		{
//			if( m_nSummary==0 )
//			{
//				m_comboChartSelect.AddString("Duration(Detailed)");
//				m_comboChartSelect.SetCurSel(0);
//				m_MSChartCtrl.ShowWindow(SW_SHOW);
//				DurationDetailed();
//				m_MSChartCtrl.Refresh();
//				m_comboChartType.SetCurSel(1);
//				//AfxMessageBox("Detailed!");
//				break;	
//			}
//			else
//			{
//				//AfxMessageBox("Summary!");
//				m_comboChartSelect.AddString("Duration(Summary)");
//				m_comboChartSelect.SetCurSel(0);
//				m_MSChartCtrl.ShowWindow(SW_SHOW);
//				DurationSummary();
//				m_MSChartCtrl.Refresh();
//				m_comboChartType.SetCurSel(1);
//				break;				
//			}
//			break;
//		}
//	case FlightLogReportFile:
//		{
//			break;
//		}
//	case BagLogReportFile:
//		{
//			break;
//		}
//	case ServiceTimeReportFile:
//		{
//			if( m_nSummary==0 )
//			{
//				m_comboChartSelect.AddString("Service Time(Detailed)");
//				m_comboChartSelect.SetCurSel(0);
//				m_MSChartCtrl.ShowWindow(SW_SHOW);
//				ServiceTimeDetailed();
//				m_MSChartCtrl.Refresh();
//				m_comboChartType.SetCurSel(1);
//				break;	
//			}
//			else
//			{
//				//AfxMessageBox("Summary!");
//				m_comboChartSelect.AddString("Service Time(Summary)");
//				m_comboChartSelect.SetCurSel(0);
//				m_MSChartCtrl.ShowWindow(SW_SHOW);
//				ServiceTimeSummary();
//				m_MSChartCtrl.Refresh();
//				m_comboChartType.SetCurSel(1);
//				break;				
//			}
//			break;
//		}
//	case QueueTimeReportFile:
//		{
//			if( m_nSummary==0 )
//			{
//				m_comboChartSelect.AddString("Queue Time(Detailed)");
//				m_comboChartSelect.SetCurSel(0);
//				m_MSChartCtrl.ShowWindow(SW_SHOW);
//				QueueTimeDetailed();
//				m_MSChartCtrl.Refresh();
//				m_comboChartType.SetCurSel(1);
//				//AfxMessageBox("Detailed!");
//				break;	
//			}
//			else
//			{
//				//AfxMessageBox("Summary!");
//				m_comboChartSelect.AddString("Queue Time(Summary)");
//				m_comboChartSelect.SetCurSel(0);
//				m_MSChartCtrl.ShowWindow(SW_SHOW);
//				QueueTimeSummary();
//				m_MSChartCtrl.Refresh();
//				m_comboChartType.SetCurSel(1);
//				break;				
//			}
//			break;
//		}
//	case ConveyorWaitTimeReport:
//		{
//			if( m_nSummary==0 )
//			{
//				m_comboChartSelect.AddString("Conveyor Wait Time(Detailed)");
//				m_comboChartSelect.SetCurSel(0);
//				m_MSChartCtrl.ShowWindow(SW_SHOW);
//				ConveyorWatiTimeDetailed();
//				m_MSChartCtrl.Refresh();
//				m_comboChartType.SetCurSel(1);
//				//AfxMessageBox("Detailed!");
//				break;	
//			}
//			else
//			{
//				//AfxMessageBox("Summary!");
//				m_comboChartSelect.AddString("Conveyor Wait Time(Summary)");
//				m_comboChartSelect.SetCurSel(0);
//				m_MSChartCtrl.ShowWindow(SW_SHOW);
//				ConveyorWatiTimeSummary();
//				m_MSChartCtrl.Refresh();
//				m_comboChartType.SetCurSel(1);
//				break;				
//			}
//			break;
//		}
//	
//	case ActivityBreakdownReportFile:
//		{
//			if( m_nSummary==0 )
//			{
//				m_comboChartSelect.AddString("Time Breakdown(Detailed)");
//				m_comboChartSelect.AddString("Percentage Breakdown(Detailed)");
//				m_comboChartSelect.SetCurSel(0);
//				m_MSChartCtrl.ShowWindow(SW_SHOW);
//				ActBKDownTimeBKDetailed();
//				m_MSChartCtrl.Refresh();
//				m_comboChartType.SetCurSel(1);
//				break;	
//			}
//			else
//			{
//				m_comboChartSelect.AddString("Time Breakdown(Summary)");
//				m_comboChartSelect.AddString("Percentage Breakdown(Summary)");
//				m_comboChartSelect.SetCurSel(0);
//				m_MSChartCtrl.ShowWindow(SW_SHOW);
//				ActBKDownTimeBKSummary();
//				m_MSChartCtrl.Refresh();
//				m_comboChartType.SetCurSel(1);
//				break;			
//			}
//			break;
//		}
//	case BagWaitTimeReportFile:
//		{
//			if( m_nSummary==0 )
//			{
//				m_comboChartSelect.AddString("Bag Wait Time(Detailed)");
//				m_comboChartSelect.SetCurSel(0);
//				m_MSChartCtrl.ShowWindow(SW_SHOW);
//				BagWaitTimeDetailed();
//				m_MSChartCtrl.Refresh();
//				m_comboChartType.SetCurSel(1);
//				break;	
//			}
//			else
//			{
//				m_comboChartSelect.AddString("Bag Wait Time(Summary)");
//				m_comboChartSelect.SetCurSel(0);
//				m_MSChartCtrl.ShowWindow(SW_SHOW);
//				BagWaitTimeSummary();
//				m_MSChartCtrl.Refresh();
//				m_comboChartType.SetCurSel(1);
//				break;				
//			}
//			break;
//		}
//	case PassengerCountReportFile:
//		{
//			if( m_nSummary==0 )//Detailed
//			{
//				m_comboChartSelect.AddString("Space Occupancy(Detailed)");
//				m_comboChartSelect.SetCurSel(0);
//				m_MSChartCtrl.ShowWindow(SW_SHOW);
//				PaxCountDetailed();
//				m_MSChartCtrl.Refresh();
//				m_comboChartType.SetCurSel(1);
//				break;	
//			}
//			else//Summary
//			{
//				m_comboChartSelect.AddString("Passenger/Period");
//				m_comboChartSelect.AddString("Maximum Passenger Count");
//				m_comboChartSelect.AddString("Total Passenger Count");
//				//m_comboChartSelect.AddString("Time Range");
//				m_comboChartSelect.SetCurSel(0);
//				m_MSChartCtrl.ShowWindow(SW_SHOW);
//				PaxCountPaxPeriod();
//				m_MSChartCtrl.Refresh();
//				m_comboChartType.SetCurSel(1);
//				break;				
//			}
//			break;
//		}
//	case PassengerDensityReportFile:
//		{
//			m_comboChartSelect.AddString("Count");
//			m_comboChartSelect.AddString("Pax/m2");
//			m_comboChartSelect.AddString("m2/Pax");
//			m_comboChartSelect.SetCurSel(0);
//			m_MSChartCtrl.ShowWindow(SW_SHOW);
//			PaxDensityCount();
//			m_MSChartCtrl.Refresh();
//			m_comboChartType.SetCurSel(1);
//			break;				
//		}
//	
//	case SpaceThroughputReportFile:
//		{
//			m_comboChartSelect.AddString("Count");
//			m_comboChartSelect.SetCurSel(0);
//			m_MSChartCtrl.ShowWindow(SW_SHOW);
//			SpaceThroughputCount();
//			m_MSChartCtrl.Refresh();
//			m_comboChartType.SetCurSel(1);
//		}
//		break;
//	case CollisionReportFile:
//		{
//			m_comboChartSelect.AddString("Count");
//			m_comboChartSelect.SetCurSel(0);
//			m_MSChartCtrl.ShowWindow(SW_SHOW);
//			CollisionCount();
//			m_MSChartCtrl.Refresh();
//			m_comboChartType.SetCurSel(1);
//		}
//		break;
//	case ThroughputReportFile:
//		{
//			if( m_nSummary==0 )
//			{
//				m_comboChartSelect.AddString("Pax Served");
//				m_comboChartSelect.SetCurSel(0);
//				m_MSChartCtrl.ShowWindow(SW_SHOW);
//				ThroughputTotalPaxDetailed();
//				m_MSChartCtrl.Refresh();
//				m_comboChartType.SetCurSel(1);
//				//AfxMessageBox("Detailed!");
//				break;	
//			}
//			else
//			{
//				//AfxMessageBox("Summary!");
//				m_comboChartSelect.AddString("Total Pax by Group(Summary)");
//				m_comboChartSelect.AddString("Pax per Processor(Summary)");
//				m_comboChartSelect.AddString("Group Throughput per Hour(Summary)");
//				m_comboChartSelect.AddString("Processor Throughput per Hour(Summary)");
//				m_comboChartSelect.SetCurSel(0);
//				m_MSChartCtrl.ShowWindow(SW_SHOW);
//				ThroughputTotalPaxSummary();
//				m_MSChartCtrl.Refresh();
//				m_comboChartType.SetCurSel(1);
//				break;				
//			}
//			break;
//		}
//	case AverageQueueLengthReportFile:
//		{
//			if( m_nSummary==0 )//Detailed
//			{
//				m_comboChartSelect.AddString("Queue Length(Detailed)");
//				m_comboChartSelect.AddString("Time of Max Queue(Detailed)");
//				m_comboChartSelect.AddString("Max Queue Time Histogram(Detailed)");
//				m_comboChartSelect.SetCurSel(0);
//				m_MSChartCtrl.ShowWindow(SW_SHOW);
//				AvgQueueLenDetailed();
//				m_MSChartCtrl.Refresh();
//				m_comboChartType.SetCurSel(1);
//				break;	
//			}
//			else//Summary
//			{
//				m_comboChartSelect.AddString("Queue Length(Summary)");
//				m_comboChartSelect.AddString("Time of Max Queue(Summary)");
//				m_comboChartSelect.AddString("Max Queue Time Histogram(Summary)");
//				m_comboChartSelect.SetCurSel(0);
//				m_MSChartCtrl.ShowWindow(SW_SHOW);
//				AvgQueueLenSummary();
//				m_MSChartCtrl.Refresh();
//				m_comboChartType.SetCurSel(1);
//				break;				
//			}
//			break;
//		}
//	case QueueLengthReportFile:
//		{
//			if( m_nSummary==0 )//Detailed
//			{
//				m_comboChartSelect.AddString("Queue Length(Detailed)");
//				m_comboChartSelect.SetCurSel(0);
//				m_MSChartCtrl.ShowWindow(SW_SHOW);
//				QueueLengthDetailed();
//				m_MSChartCtrl.Refresh();
//				m_comboChartType.SetCurSel(1);
//				break;	
//			}
//			else//Summary
//			{
//				m_comboChartSelect.AddString("Minimum Queue Length");
//				m_comboChartSelect.AddString("Average Queue Length");
//				m_comboChartSelect.AddString("Maximum Queue Length");
//				m_comboChartSelect.AddString("Combined Queue Length");
//				m_comboChartSelect.SetCurSel(0);
//				m_MSChartCtrl.ShowWindow(SW_SHOW);
//				QueueLengthMinSummary();
//				m_MSChartCtrl.Refresh();
//				m_comboChartType.SetCurSel(1);
//				break;				
//			}
//			break;
//		}
//	case ConveyorWaitLengthReport:
//		{
//			if( m_nSummary==0 )//Detailed
//			{
//				m_comboChartSelect.AddString("Conveyor Wait Length(Detailed)");
//				m_comboChartSelect.SetCurSel(0);
//				m_MSChartCtrl.ShowWindow(SW_SHOW);
//				ConveyorWaitLengthDetailed();
//				m_MSChartCtrl.Refresh();
//				m_comboChartType.SetCurSel(1);
//				break;	
//			}
//			else//Summary
//			{
//				m_comboChartSelect.AddString("Minimum Conveyor Wait Length");
//				m_comboChartSelect.AddString("Average Conveyor Wait Length");
//				m_comboChartSelect.AddString("Maximum Conveyor Wait Length");
//				m_comboChartSelect.AddString("Combined Conveyor Wait Length");
//				m_comboChartSelect.SetCurSel(0);
//				m_MSChartCtrl.ShowWindow(SW_SHOW);
//				ConveyorWaitLengthMinSummary();
//				m_MSChartCtrl.Refresh();
//				m_comboChartType.SetCurSel(1);
//				break;				
//			}
//			break;
//		}							
//	case BagCountReportFile:
//		{
//			if( m_nSummary==0 )//Detailed
//			{
//				m_comboChartSelect.AddString("Bag Count(Detailed)");
//				m_comboChartSelect.SetCurSel(0);
//				m_MSChartCtrl.ShowWindow(SW_SHOW);
//				BagCountDetailed();
//				m_MSChartCtrl.Refresh();
//				m_comboChartType.SetCurSel(1);
//				break;	
//			}
//			else//Summary
//			{
//				m_comboChartSelect.AddString("Minimum Bag Count");
//				m_comboChartSelect.AddString("Average Bag Count");
//				m_comboChartSelect.AddString("Maximum Bag Count");
//				m_comboChartSelect.AddString("Combined Bag Count");
//				m_comboChartSelect.SetCurSel(0);
//				m_MSChartCtrl.ShowWindow(SW_SHOW);
//				BagCountMinSummary();
//				m_MSChartCtrl.Refresh();
//				m_comboChartType.SetCurSel(1);
//				break;				
//			}
//			break;
//		}
//	case MissFlightReportFile:
//		{
//			m_comboChartSelect.AddString("Miss Flight");
//			m_comboChartSelect.SetCurSel(0);
//			m_MSChartCtrl.ShowWindow( SW_SHOW );
//			MissFlightDetailed();
//			m_MSChartCtrl.Refresh();
//			m_comboChartType.SetCurSel(1);
//
//			break;
//		}
//	case BillboardExposureIncidenceFile:
//		{
//			if( m_nSummary==0 )
//			{
//				m_comboChartSelect.AddString("Pax Served");
//				m_comboChartSelect.SetCurSel(0);
//				m_MSChartCtrl.ShowWindow(SW_SHOW);
//				BillboardExposureIncidenceDetailed();
//				m_MSChartCtrl.Refresh();
//				m_comboChartType.SetCurSel(1);
//				//AfxMessageBox("Detailed!");
//				break;	
//			}
//			else
//			{
//				//AfxMessageBox("Summary!");
////				m_comboChartSelect.AddString("Total Pax by Group(Summary)");
////				m_comboChartSelect.AddString("Pax per Processor(Summary)");
//				m_comboChartSelect.AddString("Pax Throughput per Hour(Summary)");
////				m_comboChartSelect.AddString("Processor Throughput per Hour(Summary)");
//				m_comboChartSelect.SetCurSel(0);
//				m_MSChartCtrl.ShowWindow(SW_SHOW);
//				BillboardExposureIncidenceSummary();
//				m_MSChartCtrl.Refresh();
//				m_comboChartType.SetCurSel(1);
//				break;				
//			}
//			break;
//		}
//
//	case BillboardLinkedProcIncrementlVisitFile:
//		{
//
//			if( m_nSummary==0 )
//			{
//				m_comboChartSelect.AddString("Pax Incremental Visit ");
//				m_comboChartSelect.SetCurSel(0);
//				m_MSChartCtrl.ShowWindow(SW_SHOW);
//				BillboardLinkedProcIncrementalVisitDetailed();
//				m_MSChartCtrl.Refresh();
//				m_comboChartType.SetCurSel(1);
//				//AfxMessageBox("Detailed!");
//				break;	
//			}
//			else
//			{
//				//AfxMessageBox("Summary!");
////				m_comboChartSelect.AddString("Total Pax by Group(Summary)");
////				m_comboChartSelect.AddString("Pax per Processor(Summary)");
//				m_comboChartSelect.AddString("Pax Incremental per Hour(Summary)");
////				m_comboChartSelect.AddString("Processor Throughput per Hour(Summary)");
//				m_comboChartSelect.SetCurSel(0);
//				m_MSChartCtrl.ShowWindow(SW_SHOW);
//				BillboardLinkedProcIncrementalVisitSummary();
//				m_MSChartCtrl.Refresh();
//				m_comboChartType.SetCurSel(1);
//				break;				
//			}
//			break;	
//		}
//	}	
//	//****End of doing something about chart select combox***
//	
//	//m_MSChartCtrl.ShowWindow(SW_HIDE);
//	Sleep(500);
//	CTermPlanDoc * pDoc=(CTermPlanDoc *)GetDocument();
//	//m_MSChartCtrl.ShowWindow(pDoc->m_bShowGraph);
//}

void CRepGraphView::OnSelchangeChartSelectCombo() 
{
	if(m_pRepGraphViewOperator)
		m_pRepGraphViewOperator->OnSelchangeChartSelectCombo();
//	// TODO: Add your control notification handler code here
//	//m_MSChartCtrl.ShowWindow(SW_SHOW);
//	m_MSChartCtrl.SetShowLegend(FALSE);
//	int nCurSel=m_comboChartSelect.GetCurSel();
//	if(nCurSel<0)
//	{
//		return;
//	}
//	CString strSelect;
//	m_comboChartSelect.GetLBText(nCurSel,strSelect);
//	if(strSelect.IsEmpty())
//	{
//		return;
//	}
//	
//	
//	switch( m_nReportFileType )
//	{
//	case BagDeliveryTimeReportFile:
//		{
//			if(m_nSummary == 0)
//			{
//				BagDeliveryCount();
//			}
//			else
//				BagDeliverySummary();
//		}
//	case BagDistReportFile:
//		{
//			if(m_nSummary==0)
//			{
//				if(strSelect.Compare("Arrival Bags Count")==0)
//				{
//					BagArrivalDistCount();
//				}
//				else if(strSelect.Compare("Depart Bags Count")==0)
//				{
//					BagDepartDistCount();
//				}
//			}
//		}
//	case AcOperationsReportFile:
//		{
//			if(m_nSummary!=0)
//			{
//				if(strSelect.Compare("Scheduled Arrival Time")==0)
//				{
//					AcOpArrivalCount();
//				}
//				else if(strSelect.Compare("Scheduled Depart Time")==0)
//				{
//					AcOpDepartCount();
//				}
//				else if(strSelect.Compare("Total")==0)
//				{
//					AcOpTotal();
//				}
//			}
//		}
//	case PaxLogReportFile:
//		{
//			if( m_nSummary!=0)
//			{
//				if(strSelect=="System Entry Distribution")
//				{
//					PaxLogSysEntryDistribution();
//				}
//				else
//				{
//					if(strSelect=="System Exit Distribution")
//					{
//						PaxLogSysExitDistribution();
//					}
//					else
//					{
//						if(strSelect=="Group Size Distribution")
//						{
//							//AfxMessageBox("Group Size Distribution");
//							PaxLogGroupSizeDistribution();
//						}
//						else
//						{
//							if(strSelect=="Bag Count Distribution")
//							{
//								//AfxMessageBox("Bag Count Distribution");
//								PaxLogBagCountDistribution();
//							}
//							else
//							{
//								if(strSelect=="Cart Count Distribution")
//								{
//									//AfxMessageBox("Cart Count Distribution");
//									PaxLogCartCountDistribution();
//								}
//							}
//						}
//					}
//				}	
//				break;	
//			}
//			else
//			{
//				
//				break;				
//			}
//		}
//		break;
//	case UtilizationReportFile:
//		{
//			if( m_nSummary==0 )
//			{
//				if(strSelect=="Utilization Time(Detailed)")
//				{
//					UtilizationTimeDetailed();
//				}
//				else
//				{
//					if(strSelect=="Time Available(Detailed)")
//					{
//						//AfxMessageBox("Time Available(Detailed)");
//						UtilizationTimeAvailableDetailed();
//					}
//					else
//					{
//						if(strSelect=="Utilization Percentage(Detailed)")
//						{
//							//AfxMessageBox("Utilization Percentage(Detailed)");
//							UtilizationPercentageDetailed();
//						}
//					}
//				}
//				break;	
//			}
//			else
//			{
//				if(strSelect=="Utilization Time(Summary)")
//				{
//					//AfxMessageBox("Utilization Time(Summary)");
//					UtilizationTimeSummary();
//				}
//				else
//				{
//					if(strSelect=="Time Available(Summary)")
//					{
//						UtilizationTimeAvailableSummary();
//					}
//					else
//					{
//						if(strSelect=="Utilization Percentage(Summary)")
//						{
//							UtilizationPercentageSummary();
//						}
//					}
//				}		
//				break;				
//			}
//			break;		
//		}	
//	case DistanceReportFile:
//		{
//			if( m_nSummary==0 )
//			{
//				if(strSelect=="Distance Travelled(Detailed)")
//				{
//					//AfxMessageBox("Distance Travelled(Detailed)");
//					DistanceTravelledDetailed();
//				}
//				break;	
//			}
//			else
//			{
//				if(strSelect=="Distance Travelled(Summary)")
//				{
//					//AfxMessageBox("Distance Travelled(Summary)");
//					DistanceTravelledSummary();
//				}			
//				break;				
//			}
//			break;
//		}
//	case DurationReportFile:
//		{
//			if( m_nSummary==0 )
//			{
//				if(strSelect=="Duration(Detailed)")
//				{
//					//AfxMessageBox("Distance Travelled(Detailed)");
//					DurationDetailed();
//				}
//				break;	
//			}
//			else
//			{
//				if(strSelect=="Duration(Summary)")
//				{
//					//AfxMessageBox("Distance Travelled(Summary)");
//					DurationSummary();
//				}			
//				break;				
//			}
//			break;
//		}
//	case FlightLogReportFile:
//		{
//			break;
//		}
//	case BagLogReportFile:
//		{
//			break;
//		}
//	case ServiceTimeReportFile:
//		{
//			if( m_nSummary==0 )
//			{
//				if(strSelect=="Service Time(Detailed)")
//				{
//					//AfxMessageBox("Distance Travelled(Detailed)");
//					ServiceTimeDetailed();
//				}
//				break;	
//			}
//			else
//			{
//				if(strSelect=="Service Time(Summary)")
//				{
//					//AfxMessageBox("Distance Travelled(Summary)");
//					ServiceTimeSummary();
//				}			
//				break;				
//			}
//			break;
//		}
//	case ActivityBreakdownReportFile:
//		{
//			if( m_nSummary==0 )
//			{
//				if(strSelect=="Time Breakdown(Detailed)")
//				{
//					ActBKDownTimeBKDetailed();
//				}
//				else
//				{
//					if(strSelect=="Percentage Breakdown(Detailed)")
//					{
//						ActBKDownPerBKDetailed();
//					}
//				}
//				break;	
//			}
//			else
//			{
//				if(strSelect=="Time Breakdown(Summary)")
//				{
//					ActBKDownTimeBKSummary();
//				}
//				else
//				{
//					if(strSelect=="Percentage Breakdown(Summary)")
//					{
//						ActBKDownPerBKSummary();
//					}
//				}			
//				break;				
//			}
//			break;
//		}
//	case BagWaitTimeReportFile:
//		{
//			
//			break;
//		}
//	case PassengerCountReportFile:
//		{
//			if( m_nSummary==0 )
//			{
//				if(strSelect=="Space Occupancy(Detailed)")
//				{
//					//AfxMessageBox("Distance Travelled(Detailed)");
//					PaxCountDetailed();
//				}
//				break;	
//			}
//			else
//			{
//				if(strSelect=="Passenger/Period")
//				{
//					PaxCountPaxPeriod();
//				}			
//				else
//				{
//					if(strSelect=="Maximum Passenger Count")
//					{
//						PaxCountMaxPaxCount();
//					}
//					else
//					{
//						if(strSelect=="Total Passenger Count")
//						{
//							PaxCountTotalPaxCount();
//						}
//						else
//						{
//							if(strSelect=="Time Range")
//							{
//								PaxCountTimeRange();
//							}
//						}
//					}
//				}
//				break;				
//			}
//			break;
//		}
//	case CollisionReportFile:
//		break;
//	case SpaceThroughputReportFile:
//		break;
//	case PassengerDensityReportFile:
//		{
//			if(strSelect=="Count")
//			{
//				PaxDensityCount();
//			}
//			else
//			{
//				if(strSelect=="Pax/m2")
//				{
//					PaxDensityPaxM2();
//				}
//				else
//				{
//					if(strSelect=="m2/Pax")
//					{
//						PaxDensityM2Pax();
//					}
//				}
//			}
//			break;
//		}
//	case ThroughputReportFile:
//		{
//			if( m_nSummary==0 )
//			{
//				break;	
//			}
//			else
//			{
//				if(strSelect=="Total Pax by Group(Summary)")
//				{
//					ThroughputTotalPaxSummary();
//				}
//				else
//				{
//					if(strSelect=="Pax per Processor(Summary)")
//					{
//						ThroughputPaxPerProcSummary();
//					}
//					else
//					{
//						if(strSelect=="Group Throughput per Hour(Summary)")
//						{
//							ThroughputGroupPerHourSummary();
//						}
//						else
//						{
//							if(strSelect=="Processor Throughput per Hour(Summary)")
//							{
//								ThroughputProcPerHourSummary();
//							}
//						}
//					}
//				}		
//				break;				
//			}
//			break;
//		}
//	case AverageQueueLengthReportFile:
//		{
//			if( m_nSummary==0 )
//			{
//				if(strSelect=="Queue Length(Detailed)")
//				{
//					AvgQueueLenDetailed();
//				}
//				else
//				{
//					if(strSelect=="Time of Max Queue(Detailed)")
//					{
//						AvgQueueLenTimeMaxQueDetailed();
//					}
//					else
//					{
//						if(strSelect=="Max Queue Time Histogram(Detailed)")
//						{
//							AvgQueueLenMaxQueueTimeHisDetailed();
//						}
//					}
//				}
//				break;	
//			}
//			else
//			{
//				if(strSelect=="Queue Length(Summary)")
//				{
//					AvgQueueLenSummary();
//				}
//				else
//				{
//					if(strSelect=="Time of Max Queue(Summary)")
//					{
//						AvgQueueLenTimeMaxQueSummary();
//					}
//					else
//					{
//						if(strSelect=="Max Queue Time Histogram(Summary)")
//						{
//							AvgQueueLenMaxQueTimeHisSummary();
//						}
//					}
//				}		
//				break;				
//			}
//			break;	
//		}
//	case QueueLengthReportFile:
//		{
//			if( m_nSummary==0 )
//			{
//				if(strSelect=="Queue Length(Detailed)")
//				{
//					//AfxMessageBox("Distance Travelled(Detailed)");
//					QueueLengthDetailed();
//				}
//				break;	
//			}
//			else
//			{
//				if(strSelect=="Minimum Queue Length")
//				{
//					QueueLengthMinSummary();
//				}			
//				else
//				{
//					if(strSelect=="Average Queue Length")
//					{
//						QueueLengthAvgSummary();
//					}
//					else
//					{
//						if(strSelect=="Maximum Queue Length")
//						{
//							QueueLengthMaxSummary();
//						}
//						else
//						{
//							if(strSelect=="Combined Queue Length")
//							{
//								QueueLengthCombinedSummary();
//							}
//						}
//					}
//				}
//				break;				
//			}
//			break;
//		}
//
//	case ConveyorWaitLengthReport:
//		{
//			if( m_nSummary==0 )
//			{
//				if(strSelect=="Conveyor Wait Length(Detailed)")
//				{
//					//AfxMessageBox("Distance Travelled(Detailed)");
//					ConveyorWaitLengthDetailed();
//				}
//				break;	
//			}
//			else
//			{
//				if(strSelect=="Minimum Conveyor Wait Length")
//				{
//					ConveyorWaitLengthMinSummary();
//				}			
//				else
//				{
//					if(strSelect=="Average Conveyor Wait Length")
//					{
//						ConveyorWaitLengthAvgSummary();
//					}
//					else
//					{
//						if(strSelect=="Maximum Conveyor Wait Length")
//						{
//							ConveyorWaitLengthMaxSummary();
//						}
//						else
//						{
//							if(strSelect=="Combined Conveyor Wait Length")
//							{
//								ConveyorWaitLengthCombinedSummary();
//							}
//						}
//					}
//				}
//				break;				
//			}
//			break;
//		}
//		
//	case BagCountReportFile:
//		{
//			if( m_nSummary==0 )
//			{
//				if(strSelect=="Bag Count(Detailed)")
//				{
//					BagCountDetailed();
//				}
//				break;	
//			}
//			else
//			{
//				if(strSelect=="Minimum Bag Count")
//				{
//					BagCountMinSummary();
//				}			
//				else
//				{
//					if(strSelect=="Average Bag Count")
//					{
//						BagCountAvgSummary();
//					}
//					else
//					{
//						if(strSelect=="Maximum Bag Count")
//						{
//							BagCountMaxSummary();
//						}
//						else
//						{
//							if(strSelect=="Combined Bag Count")
//							{
//								BagCountCombinedSummary();
//							}
//						}
//					}
//				}
//				break;				
//			}
//			break;
//		}
//	}
//	m_MSChartCtrl.Refresh();
//	Sleep(500);
//	CTermPlanDoc * pDoc=(CTermPlanDoc *)GetDocument();
//	//m_MSChartCtrl.ShowWindow(pDoc->m_bShowGraph);
//	//****End of doing something about chart select combox***
}

//void CRepGraphView::SetHeadXYTitle(CString strTitle, CString strXAxisTitle, CString strYAxisTitle, CString strFootnote)
//{
//	//****Do something about Title,X Axis Title,Y Axis Title*******
//	//Author:Xie Bo 2002.2.28
//	
//	CVcPlot plot(m_MSChartCtrl.GetPlot());
//	_variant_t  Index((long) 0);
//	
//	//Spruce up title
//	CVcTitle title(m_MSChartCtrl.GetTitle());
//	title.SetText(strTitle);
////	CVcFont font(title.GetVtFont());
////	font.SetName(_TEXT("Arial"));	
////	font.SetSize(24.0);
//	//font.SetStyle(VtFontStyleBold);
//	
//	// Label X Axis
//	CVcAxis xaxis(plot.GetAxis(0, Index ));
//	CVcAxisTitle xaxistitle(xaxis.GetAxisTitle());
//	xaxistitle.SetVisible(true);
//	xaxistitle.SetText(strXAxisTitle);
//	CVcFont xfont(xaxistitle.GetVtFont());
////	xfont.SetName(_TEXT("Arial"));
////	xfont.SetSize(16.5);
////	xfont.SetStyle(1);
//	//xfont.SetStyle(VtFontStyleBold);
////	xfont.SetSize(8);
//	
//	
//	// Label Y Axis
//	CVcAxis yaxis(plot.GetAxis(1, Index ));
//	CVcAxisTitle yaxistitle(yaxis.GetAxisTitle());
//	yaxistitle.SetVisible(true);
//	yaxistitle.SetText(strYAxisTitle);
////	CVcFont yfont(yaxistitle.GetVtFont());
////	yfont.SetName(_TEXT("Arial"));
////	yfont.SetSize(16.5);
////	yfont.SetStyle(1);
////	xfont.SetSize(8);
//	
//	
//	// Footnote
//	CVcFootnote footnote(m_MSChartCtrl.GetFootnote());
//	footnote.SetText(strFootnote);
//	//*****End of doing something about Title,X Axis Title,Y Axis Title****
//}

//void CRepGraphView::PaxLogSysEntryDistribution()
//{
//	//char filename[128];
//	//AfxMessageBox("System Entry Distribution");
//	//1.)Init chartFile
//	//PROJMANAGER->getReportFileName( ((CTermPlanDoc*)GetDocument())->m_ProjInfo.path, PaxLogReportFile, filename);
//
//	// load report from file
//	CString sFileName;
//	if( GetLoadReportType() == load_by_user )
//	{
//		sFileName = ((CTermPlanDoc*)GetDocument())->GetARCReportManager().GetUserLoadReportFile();
//	}
//	else								// load_by_system
//	{
//		sFileName = ((CTermPlanDoc*)GetDocument())->GetTerminal().GetSimReportManager()->GetCurrentReportFileName( ((CTermPlanDoc*)GetDocument())->m_ProjInfo.path );
//	}
//	chartFile.init (sFileName, READ);
//	if (chartFile.countLines() <= 5)
//	{
//		chartFile.closeIn();
//		return;
//	}
//	//2.)Draw Title,X Axis Title,Y Axis Title,Footnote
//	chartFile.reset();
//	chartFile.getLine();
//	chartFile.getLine();
//	char string[4][128];
//	chartFile.getField (string[0], 128);
//	chartFile.getField (string[1], 128);
//	chartFile.getField (string[2], 128);
//	chartFile.getField (string[3], 128);
//	CString strFootnote;
//	strFootnote.Format("%s %s %s %s",NoUnderline(CString(string[0])),string[1],string[2],string[3]);
//	SetHeadXYTitle("System Entry Distribution","Entry Time","Passenger Count",strFootnote);
//	//3.)Draw it
//	chartFile.reset();
//	chartFile.skipLines(3);
//	int xValue[8];
//	int yValue[8];
//	for(int i=0;i<8;i++)
//	{
//		xValue[i]=yValue[i]=0;
//	}
//	int maxXValue=0;
//	int minXValue=1440;// max minutes in 1 day
//	float ft = 0.0;   
//	ElapsedTime value;
//	while (chartFile.getLine())
//	{
//		chartFile.setToField (1);
//		chartFile.getTime (value, TRUE);
//		if (value.asMinutes() < minXValue)
//			minXValue = value.asMinutes();
//		if (value.asMinutes() > maxXValue)
//			maxXValue = value.asMinutes();
//	}
//	minXValue=minXValue-(minXValue%30);
//	maxXValue=maxXValue-(maxXValue%30)+30;
//	int nScale;//
//	int n=(maxXValue-minXValue)%210;//210=30*7;  7 Column
//	if(n==0)
//	{
//		nScale=(maxXValue-minXValue)/210;
//	}
//	else
//	{
//		nScale=(maxXValue-minXValue)/210+1;
//	}
//	
//	xValue[0]=minXValue;
//	for(i=1;i<=7;i++)
//	{
//		xValue[i]=xValue[i-1]+30*nScale;
//	}
//	
//	chartFile.reset();
//	chartFile.skipLines(3);
//	while (chartFile.getLine())
//	{
//		chartFile.setToField (1);
//		chartFile.getTime (value, TRUE);
//		for(i=1;i<=7;i++)
//		{
//			if(value.asMinutes()>=xValue[i-1] && value.asMinutes()<xValue[i])
//			{
//				yValue[i]++;
//			}
//		}
//	}
//	int nCount=7;
//	for(i=7;i>0;i--)
//	{
//		if(yValue[i]==0)
//		{
//			nCount--;
//		}
//		else
//		{
//			break;
//		}
//	}
//	m_MSChartCtrl.SetRowCount(nCount);
//	m_MSChartCtrl.SetColumnCount(1);
//	m_MSChartCtrl.SetColumnLabelCount(1);
//	
//	CString strLabel;
//	for(i=1;i<=nCount;i++)
//	{
//		m_MSChartCtrl.SetRow(i);
////		strLabel.Format("%s-%s",FormatTime(xValue[i-1]),FormatTime(xValue[i]));
//		strLabel.Format("%s-%s",
//						FormatTime(xValue[i-1] % (24 * 60)), FormatTime( xValue[i] % (24 * 60)) );
//		DealRowLabel(strLabel);
//		m_MSChartCtrl.SetRowLabel(strLabel);
//		strLabel.Format("%d",yValue[i]);
//		m_MSChartCtrl.SetData(strLabel);
//	}
//	chartFile.closeIn();				
//}
//
//void CRepGraphView::PaxLogSysExitDistribution()
//{
//	
//	CString sFileName;
//	if( GetLoadReportType() == load_by_user )
//	{
//		sFileName = GetUserLoadReportFile();
//	}
//	else								// load_by_system
//	{
//		sFileName = ((CTermPlanDoc*)GetDocument())->GetTerminal().GetSimReportManager()->GetCurrentReportFileName( ((CTermPlanDoc*)GetDocument())->m_ProjInfo.path );
//	}
//
//	chartFile.init (sFileName, READ);
//	if (chartFile.countLines() <= 5)
//	{
//		chartFile.closeIn();
//		return;
//	}
//	//2.)Draw Title,X Axis Title,Y Axis Title,Footnote
//	chartFile.reset();
//	chartFile.getLine();
//	chartFile.getLine();
//	char string[4][128];
//	chartFile.getField (string[0], 128);
//	chartFile.getField (string[1], 128);
//	chartFile.getField (string[2], 128);
//	chartFile.getField (string[3], 128);
//	CString strFootnote;
//	strFootnote.Format("%s %s %s %s",NoUnderline(CString(string[0])),string[1],string[2],string[3]);
//	SetHeadXYTitle("System Exit Distribution","Exit Time","Passenger Count",strFootnote);
//	//3.)Draw it
//	chartFile.reset();
//	chartFile.skipLines(3);
//	int xValue[8];
//	int yValue[8];
//	for(int i=0;i<8;i++)
//	{
//		xValue[i]=yValue[i]=0;
//	}
//	int maxXValue=0;
//	int minXValue=1440;// max minutes in 1 day
//	float ft = 0.0;   
//	ElapsedTime value;
//	while (chartFile.getLine())
//	{
//		chartFile.setToField (2);
//		chartFile.getTime (value, TRUE);
//		if (value.asMinutes() < minXValue)
//			minXValue = value.asMinutes();
//		if (value.asMinutes() > maxXValue)
//			maxXValue = value.asMinutes();
//	}
//	minXValue=minXValue-(minXValue%30);
//	maxXValue=maxXValue-(maxXValue%30)+30;
//	int nScale;//
//	int n=(maxXValue-minXValue)%210;//210=30*7;  7 Column
//	if(n==0)
//	{
//		nScale=(maxXValue-minXValue)/210;
//	}
//	else
//	{
//		nScale=(maxXValue-minXValue)/210+1;
//	}
//	
//	xValue[0]=minXValue;
//	for(i=1;i<=7;i++)
//	{
//		xValue[i]=xValue[i-1]+30*nScale;
//	}
//	
//	chartFile.reset();
//	chartFile.skipLines(3);
//	while (chartFile.getLine())
//	{
//		chartFile.setToField (2);
//		chartFile.getTime (value, TRUE);
//		for(i=1;i<=7;i++)
//		{
//			if(value.asMinutes()>=xValue[i-1] && value.asMinutes()<xValue[i])
//			{
//				yValue[i]++;
//			}
//		}
//	}
//	int nCount=7;
//	for(i=7;i>0;i--)
//	{
//		if(yValue[i]==0)
//		{
//			nCount--;
//		}
//		else
//		{
//			break;
//		}
//	}
//	m_MSChartCtrl.SetRowCount(nCount);
//	m_MSChartCtrl.SetColumnCount(1);
//	m_MSChartCtrl.SetColumnLabelCount(1);
//	m_MSChartCtrl.SetColumn(1);
//	m_MSChartCtrl.SetColumnLabel("Meters Travelled");
//	
//	CString strLabel;
//	for(i=1;i<=nCount;i++)
//	{
//		m_MSChartCtrl.SetRow(i);
//		strLabel.Format("%s-%s",FormatTime(xValue[i-1]),FormatTime(xValue[i]));
//		DealRowLabel(strLabel);
//		m_MSChartCtrl.SetRowLabel(strLabel);
//		
//		strLabel.Format("%d",yValue[i]);
//		m_MSChartCtrl.SetData(strLabel);
//	}
//	chartFile.closeIn();
//}
//
//CString CRepGraphView::FormatTime(int nMinute)
//{
//	CString strReturn;
//	int hour,minute;
//	hour=nMinute/60;
//	minute=nMinute%60;
//	if(hour==0)
//	{
//		if(minute==0)
//		{
//			strReturn.Format("0%d:0%d",hour,minute);
//		}
//		else
//		{
//			strReturn.Format("0%d:%d",hour,minute);
//		}
//	}
//	else
//	{
//		if(minute==0)
//		{
//			strReturn.Format("%d:0%d",hour,minute);
//		}
//		else
//		{
//			strReturn.Format("%d:%d",hour,minute);
//		}
//	}
//	return strReturn;
//}
//
//void CRepGraphView::PaxLogGroupSizeDistribution()
//{
//	// load report file from file
//	CString sFileName;
//	if( GetLoadReportType() == load_by_user )
//	{
//		sFileName = ((CTermPlanDoc*)GetDocument())->GetARCReportManager().GetUserLoadReportFile();
//	}
//	else								// load_by_system
//	{
//		sFileName = ((CTermPlanDoc*)GetDocument())->GetTerminal().GetSimReportManager()->GetCurrentReportFileName( ((CTermPlanDoc*)GetDocument())->m_ProjInfo.path );
//	}
//
//	chartFile.init (sFileName, READ);
//	int nCountLines=chartFile.countLines();
//	if (nCountLines <= 5)
//	{
//		chartFile.closeIn();
//		return;
//	}
//	//2.)Draw Title,X Axis Title,Y Axis Title,Footnote
//	chartFile.reset();
//	chartFile.getLine();
//	chartFile.getLine();
//	char string[4][128];
//	chartFile.getField (string[0], 128);
//	chartFile.getField (string[1], 128);
//	chartFile.getField (string[2], 128);
//	chartFile.getField (string[3], 128);
//	CString strFootnote;
//	strFootnote.Format("%s %s %s %s",NoUnderline(CString(string[0])),string[1],string[2],string[3]);
//	SetHeadXYTitle("Group Size Distribution","Group Size","Passenger Count",strFootnote);
//	//3.)Draw it
//	int xValue[1440];
//	int yValue[1440];
//	for(int i=0;i<1440;i++)
//	{
//		xValue[i]=yValue[i]=0;
//	}
//	int maxXValue=0;
//	int minXValue=1440;
//	float value = 0.0; 
//	chartFile.reset();
//	chartFile.skipLines(3);
//	while (chartFile.getLine())
//	{
//		chartFile.setToField (3);
//		chartFile.getFloat (value);
//        if (value < minXValue)
//            minXValue = static_cast<int>(value);
//		
//        if (value > maxXValue)
//            maxXValue = static_cast<int>(value);
//	}
//	int nCount=maxXValue;
//	chartFile.reset();
//	chartFile.skipLines(3);
//	while (chartFile.getLine())
//	{
//		chartFile.setToField (3);
//		chartFile.getFloat (value);
//		yValue[(int)value]++;
//	}
//	
//	m_MSChartCtrl.SetRowCount(nCount);
//	m_MSChartCtrl.SetColumnCount(1);
//	m_MSChartCtrl.SetColumnLabelCount(1);
//	m_MSChartCtrl.SetColumn(1);
//	m_MSChartCtrl.SetColumnLabel("Meters Travelled");
//	
//	CString strLabel;
//	for(i=1;i<=nCount;i++)
//	{
//		m_MSChartCtrl.SetRow(i);
//		strLabel.Format("%d",i);
//		DealRowLabel(strLabel);
//		m_MSChartCtrl.SetRowLabel(strLabel);
//		strLabel.Format("%d",yValue[i]);
//		m_MSChartCtrl.SetData(strLabel);
//	}
//	chartFile.closeIn();
//}
//
//void CRepGraphView::PaxLogBagCountDistribution()
//{
//	// load report file from file
//	CString sFileName;
//	if( GetLoadReportType() == load_by_user )
//	{
//		sFileName = GetUserLoadReportFile();
//	}
//	else								// load_by_system
//	{
//		sFileName = ((CTermPlanDoc*)GetDocument())->GetTerminal().GetSimReportManager()->GetCurrentReportFileName( ((CTermPlanDoc*)GetDocument())->m_ProjInfo.path );
//	}
//
//	chartFile.init (sFileName, READ);
//	int nCountLines=chartFile.countLines();
//	if (nCountLines <= 5)
//	{
//		chartFile.closeIn();
//		return;
//	}
//	//2.)Draw Title,X Axis Title,Y Axis Title,Footnote
//	chartFile.reset();
//	chartFile.getLine();
//	chartFile.getLine();
//	char string[4][128];
//	chartFile.getField (string[0], 128);
//	chartFile.getField (string[1], 128);
//	chartFile.getField (string[2], 128);
//	chartFile.getField (string[3], 128);
//	CString strFootnote;
//	strFootnote.Format("%s %s %s %s",NoUnderline(CString(string[0])),string[1],string[2],string[3]);
//	SetHeadXYTitle("Bag Count Distribution","Bag Count","Passenger Count",strFootnote);
//	//3.)Draw it
//	int xValue[1440];
//	int yValue[1440];
//	for(int i=0;i<1440;i++)
//	{
//		xValue[i]=yValue[i]=0;
//	}
//	int maxXValue=0;
//	int minXValue=1440;
//	float value = 0.0; 
//	chartFile.reset();
//	chartFile.skipLines(3);
//	while (chartFile.getLine())
//	{
//		chartFile.setToField (4);
//		chartFile.getFloat (value);
//        if (value < minXValue)
//            minXValue = static_cast<int>(value);
//		
//        if (value > maxXValue)
//            maxXValue = static_cast<int>(value);
//	}
//	int nCount=maxXValue;
//	chartFile.reset();
//	chartFile.skipLines(3);
//	while (chartFile.getLine())
//	{
//		chartFile.setToField (4);
//		chartFile.getFloat (value);
//		yValue[(int)value]++;
//	}
//	
//	m_MSChartCtrl.SetRowCount(nCount+1);
//	m_MSChartCtrl.SetColumnCount(1);
//	m_MSChartCtrl.SetColumnLabelCount(1);
//	m_MSChartCtrl.SetColumn(1);
//	m_MSChartCtrl.SetColumnLabel("Meters Travelled");
//	
//	CString strLabel;
//	for(i=1;i<=nCount+1;i++)
//	{
//		m_MSChartCtrl.SetRow(i);
//		strLabel.Format("%d",i-1);
//		DealRowLabel(strLabel);
//		m_MSChartCtrl.SetRowLabel(strLabel);
//		strLabel.Format("%d",yValue[i-1]);
//		m_MSChartCtrl.SetData(strLabel);
//	}
//	chartFile.closeIn();
//}
//
//void CRepGraphView::PaxLogCartCountDistribution()
//{
//	// load report file from file
//	CString sFileName;
//	if( GetLoadReportType() == load_by_user )
//	{
//		sFileName = GetUserLoadReportFile();
//	}
//	else								// load_by_system
//	{
//		sFileName = ((CTermPlanDoc*)GetDocument())->GetTerminal().GetSimReportManager()->GetCurrentReportFileName( ((CTermPlanDoc*)GetDocument())->m_ProjInfo.path );
//	}
//
//	chartFile.init (sFileName, READ);
//	int nCountLines=chartFile.countLines();
//	if (nCountLines <= 5)
//	{
//		chartFile.closeIn();
//		return;
//	}
//	//2.)Draw Title,X Axis Title,Y Axis Title,Footnote
//	chartFile.reset();
//	chartFile.getLine();
//	chartFile.getLine();
//	char string[4][128];
//	chartFile.getField (string[0], 128);
//	chartFile.getField (string[1], 128);
//	chartFile.getField (string[2], 128);
//	chartFile.getField (string[3], 128);
//	CString strFootnote;
//	strFootnote.Format("%s %s %s %s",NoUnderline(CString(string[0])),string[1],string[2],string[3]);
//	SetHeadXYTitle("Cart Count Distribution","Cart Count","Passenger Count",strFootnote);
//	//3.)Draw it
//	int xValue[1440];
//	int yValue[1440];
//	for(int i=0;i<1440;i++)
//	{
//		xValue[i]=yValue[i]=0;
//	}
//	int maxXValue=0;
//	int minXValue=1440;
//	float value = 0.0; 
//	chartFile.reset();
//	chartFile.skipLines(3);
//	while (chartFile.getLine())
//	{
//		chartFile.setToField (5);
//		chartFile.getFloat (value);
//        if (value < minXValue)
//            minXValue = static_cast<int>(value);
//		
//        if (value > maxXValue)
//            maxXValue = static_cast<int>(value);
//	}
//	int nCount=maxXValue;
//	chartFile.reset();
//	chartFile.skipLines(3);
//	while (chartFile.getLine())
//	{
//		chartFile.setToField (5);
//		chartFile.getFloat (value);
//		yValue[(int)value]++;
//	}
//	
//	m_MSChartCtrl.SetRowCount(nCount+1);
//	m_MSChartCtrl.SetColumnCount(1);
//	m_MSChartCtrl.SetColumnLabelCount(1);
//	m_MSChartCtrl.SetColumn(1);
//	m_MSChartCtrl.SetColumnLabel("Meters Travelled");
//	
//	CString strLabel;
//	for(i=1;i<=nCount+1;i++)
//	{
//		m_MSChartCtrl.SetRow(i);
//		strLabel.Format("%d",i-1);
//		DealRowLabel(strLabel);
//		m_MSChartCtrl.SetRowLabel(strLabel);
//		strLabel.Format("%d",yValue[i-1]);
//		m_MSChartCtrl.SetData(strLabel);
//	}
//	chartFile.closeIn();
//}
//
//void CRepGraphView::DistanceTravelledSummary()
//{
//	//char filename[128];
//	//1.)Init chartFile
//	////PROJMANAGER->getReportFileName( ((CTermPlanDoc*)GetDocument())->m_ProjInfo.path, DistanceReportFile, filename);
//	// load report from file
//	CString strFileName;
//	if( GetLoadReportType() == load_by_user )
//	{
//		strFileName = ((CTermPlanDoc*)GetDocument())->GetARCReportManager().GetUserLoadReportFile();
//	}
//	else								// load_by_system
//	{
//		strFileName = ((CTermPlanDoc*)GetDocument())->GetTerminal().GetSimReportManager()->GetCurrentReportFileName( ((CTermPlanDoc*)GetDocument())->m_ProjInfo.path );
//	}
//	chartFile.init (strFileName, READ);
//	//2.)Draw Title,X Axis Title,Y Axis Title,Footnote
//	chartFile.reset();
//	chartFile.getLine();
//	chartFile.getLine();
//	char string[4][128];
//	chartFile.getField (string[0], 128);
//	chartFile.getField (string[1], 128);
//	chartFile.getField (string[2], 128);
//	chartFile.getField (string[3], 128);
//	CString strFootnote;
//	strFootnote.Format("%s %s %s %s",NoUnderline(CString(string[0])),string[1],string[2],string[3]);
//	//Modified by Xie Bo 2002.5.5
//	//For Unit Transfer
//	CUnitsManager* pUM = CUnitsManager::GetInstance();
//	int nUnit = pUM->GetLengthUnits();				
//	CString str =  pUM->GetLengthUnitLongString( nUnit );
//	SetHeadXYTitle("Distance Travelled (Summary)","Passenger Type",str+" Travelled",strFootnote);
//	
//	//3.)Draw it
//	CString strxValue[100];//The Pax Type Name
//	int nPaxCount=0;
//	float minvalue[100];
//	float avevalue[100];
//	float maxvalue[100];
//
//	float Q1value[100];
//	float Q2value[100];
//	float Q3value[100];
//	
//	float P1value[100];
//	float P5value[100];
//	float P10value[100];
//	
//	float P90value[100];
//	float P95value[100];
//	float P99value[100];
//	
//	float Sigmavalue[100];
//
//	char buf[128];
//	for(int k=0;k<100;k++)
//	{
//		minvalue[k]=avevalue[k]=maxvalue[k]=Q1value[k]=0;
//		Q2value[k]=Q3value[k]=P1value[k]=P5value[k]=0;
//		Sigmavalue[k] = P10value[k]=P95value[k]=P99value[k]=0;
//	}
//	chartFile.reset();
//	chartFile.skipLines(3);
//	while (chartFile.getLine())
//	{
//		chartFile.setToField(0);
//		chartFile.getField(buf,128);
//		nPaxCount+=1;
//		strxValue[nPaxCount].Format("%s",buf);//Get The First Pax Type
//		chartFile.setToField (1);
//		chartFile.getFloat (minvalue[nPaxCount]);
//		chartFile.setToField (2);
//		chartFile.getFloat (avevalue[nPaxCount]);
//		chartFile.setToField (3);
//		chartFile.getFloat (maxvalue[nPaxCount]);
//
//		chartFile.setToField (5);
//		chartFile.getFloat (Q1value[nPaxCount]);
//
//		chartFile.setToField (6);
//		chartFile.getFloat (Q2value[nPaxCount]);
//
//		chartFile.setToField (7);
//		chartFile.getFloat (Q3value[nPaxCount]);
//
//		chartFile.setToField (8);
//		chartFile.getFloat (P1value[nPaxCount]);
//
//		chartFile.setToField (9);
//		chartFile.getFloat (P5value[nPaxCount]);
//
//		chartFile.setToField (10);
//		chartFile.getFloat (P10value[nPaxCount]);
//		
//		chartFile.setToField (11);
//		chartFile.getFloat (P90value[nPaxCount]);
//
//		chartFile.setToField (12);
//		chartFile.getFloat (P95value[nPaxCount]);
//
//		chartFile.setToField (13);
//		chartFile.getFloat (P99value[nPaxCount]);
//
//		chartFile.setToField (14);
//		chartFile.getFloat (Sigmavalue[nPaxCount]);
//	}
//	
//	CString strLabel;
//	m_MSChartCtrl.SetRowCount(nPaxCount);
//	m_MSChartCtrl.SetColumnCount(13);
//	m_MSChartCtrl.SetColumnLabelCount(13);
//
//	m_MSChartCtrl.SetColumn(1);
//	m_MSChartCtrl.SetColumnLabel("Minimum");
//
//	m_MSChartCtrl.SetColumn(2);
//	m_MSChartCtrl.SetColumnLabel("Average");
//
//	m_MSChartCtrl.SetColumn(3);
//	m_MSChartCtrl.SetColumnLabel("Maximum");
//
//	m_MSChartCtrl.SetColumn(4);
//	m_MSChartCtrl.SetColumnLabel("Q1");
//
//	m_MSChartCtrl.SetColumn(5);
//	m_MSChartCtrl.SetColumnLabel("Q2");
//
//	m_MSChartCtrl.SetColumn(6);
//	m_MSChartCtrl.SetColumnLabel("Q3");
//
//	m_MSChartCtrl.SetColumn(7);
//	m_MSChartCtrl.SetColumnLabel("P1");
//
//	m_MSChartCtrl.SetColumn(8);
//	m_MSChartCtrl.SetColumnLabel("P5");
//
//	m_MSChartCtrl.SetColumn(9);
//	m_MSChartCtrl.SetColumnLabel("P10");
//
//	m_MSChartCtrl.SetColumn(10);
//	m_MSChartCtrl.SetColumnLabel("P90");
//
//	m_MSChartCtrl.SetColumn(11);
//	m_MSChartCtrl.SetColumnLabel("P95");
//
//	m_MSChartCtrl.SetColumn(12);
//	m_MSChartCtrl.SetColumnLabel("P99");
//
//	m_MSChartCtrl.SetColumn(13);
//	m_MSChartCtrl.SetColumnLabel("Sigma");
//
//	for(int i=1;i<=nPaxCount;i++)
//	{
//		m_MSChartCtrl.SetRow(i);
//		DealRowLabel(strxValue[i]);		
//		m_MSChartCtrl.SetRowLabel(strxValue[i]);
//		
//		m_MSChartCtrl.SetColumn(1);
//		strLabel.Format("%f",UNITSMANAGER->ConvertLength( (double)minvalue[i])*SCALE_FACTOR);
//		m_MSChartCtrl.SetData(strLabel);
//
//		m_MSChartCtrl.SetRow(i);	//add by bird 2003/2/18
//		m_MSChartCtrl.SetColumn(2);
//		strLabel.Format("%f",UNITSMANAGER->ConvertLength( (double)avevalue[i])*SCALE_FACTOR);
//		m_MSChartCtrl.SetData(strLabel);
//
//		m_MSChartCtrl.SetRow(i);	//add by bird 2003/2/18
//		m_MSChartCtrl.SetColumn(3);
//		strLabel.Format("%f",UNITSMANAGER->ConvertLength( (double)maxvalue[i])*SCALE_FACTOR);
//		m_MSChartCtrl.SetData(strLabel);
//
//		m_MSChartCtrl.SetRow(i);	
//		m_MSChartCtrl.SetColumn(4);
//		strLabel.Format("%f",UNITSMANAGER->ConvertLength( (double)Q1value[i])*SCALE_FACTOR);
//		m_MSChartCtrl.SetData(strLabel);
//
//		m_MSChartCtrl.SetRow(i);	
//		m_MSChartCtrl.SetColumn(5);
//		strLabel.Format("%f",UNITSMANAGER->ConvertLength( (double)Q2value[i])*SCALE_FACTOR);
//		m_MSChartCtrl.SetData(strLabel);
//
//		m_MSChartCtrl.SetRow(i);	
//		m_MSChartCtrl.SetColumn(6);
//		strLabel.Format("%f",UNITSMANAGER->ConvertLength( (double)Q3value[i])*SCALE_FACTOR);
//		m_MSChartCtrl.SetData(strLabel);
//
//		m_MSChartCtrl.SetRow(i);
//		m_MSChartCtrl.SetColumn(7);
//		strLabel.Format("%f",UNITSMANAGER->ConvertLength( (double)P1value[i])*SCALE_FACTOR);
//		m_MSChartCtrl.SetData(strLabel);
//
//		m_MSChartCtrl.SetRow(i);
//		m_MSChartCtrl.SetColumn(8);
//		strLabel.Format("%f",UNITSMANAGER->ConvertLength( (double)P5value[i])*SCALE_FACTOR);
//		m_MSChartCtrl.SetData(strLabel);
//
//		m_MSChartCtrl.SetRow(i);
//		m_MSChartCtrl.SetColumn(9);
//		strLabel.Format("%f",UNITSMANAGER->ConvertLength( (double)P10value[i])*SCALE_FACTOR);
//		m_MSChartCtrl.SetData(strLabel);
//
//		m_MSChartCtrl.SetRow(i);
//		m_MSChartCtrl.SetColumn(10);
//		strLabel.Format("%f",UNITSMANAGER->ConvertLength( (double)P90value[i])*SCALE_FACTOR);
//		m_MSChartCtrl.SetData(strLabel);
//
//		m_MSChartCtrl.SetRow(i);
//		m_MSChartCtrl.SetColumn(11);
//		strLabel.Format("%f",UNITSMANAGER->ConvertLength( (double)P95value[i])*SCALE_FACTOR);
//		m_MSChartCtrl.SetData(strLabel);
//
//		m_MSChartCtrl.SetRow(i);
//		m_MSChartCtrl.SetColumn(12);
//		strLabel.Format("%f",UNITSMANAGER->ConvertLength( (double)P99value[i])*SCALE_FACTOR);
//		m_MSChartCtrl.SetData(strLabel);
//
//		m_MSChartCtrl.SetRow(i);
//		m_MSChartCtrl.SetColumn(13);
//		strLabel.Format("%f",UNITSMANAGER->ConvertLength( (double)Sigmavalue[i])*SCALE_FACTOR);
//		m_MSChartCtrl.SetData(strLabel);
//	}
//	chartFile.closeIn();
//	m_MSChartCtrl.SetShowLegend(TRUE);
//}
//
//void CRepGraphView::DistanceTravelledDetailed()
//{
////	char filename[128];
//	//1.)Init chartFile
//	//PROJMANAGER->getReportFileName( ((CTermPlanDoc*)GetDocument())->m_ProjInfo.path, DistanceReportFile, filename);
//	//chartFile.init (filename, READ);
//	// load report from file
//	CString strFileName;
//	if( GetLoadReportType() == load_by_user )
//	{
//		strFileName = GetUserLoadReportFile();
//	}
//	else								// load_by_system
//	{
//		strFileName = ((CTermPlanDoc*)GetDocument())->GetTerminal().GetSimReportManager()->GetCurrentReportFileName( ((CTermPlanDoc*)GetDocument())->m_ProjInfo.path );
//	}
//	chartFile.init (strFileName, READ);
//	//2.)Draw Title,X Axis Title,Y Axis Title,Footnote
//	chartFile.reset();
//	chartFile.getLine();
//	chartFile.getLine();
//	char string[4][128];
//	chartFile.getField (string[0], 128);
//	chartFile.getField (string[1], 128);
//	chartFile.getField (string[2], 128);
//	chartFile.getField (string[3], 128);
//	CString strFootnote;
//	strFootnote.Format("%s %s %s %s",NoUnderline(CString(string[0])),string[1],string[2],string[3]);
//	//Modified by Xie Bo 2002.5.5
//	//For Unit Transfer
//	CUnitsManager* pUM = CUnitsManager::GetInstance();
//	int nUnit = pUM->GetLengthUnits();				
//	CString str =  pUM->GetLengthUnitLongString( nUnit );
//	SetHeadXYTitle("Distance Travelled (Detailed)",str+" Travelled","Passenger Count",strFootnote);
//	
//	//3.)Draw it
//	int xValue[8];
//	int yValue[8];
//	for(int i=0;i<8;i++)
//	{
//		xValue[i]=yValue[i]=0;
//	}
//	int maxXValue=0;
//	int minXValue=14400;
//	float ft = 0.0;
//	chartFile.reset();
//	chartFile.skipLines(3);   
//	while (chartFile.getLine())
//	{
//		chartFile.setToField (1);
//		chartFile.getFloat (ft);
//		if (ft> maxXValue)
//			maxXValue = static_cast<int>(ft);
//	}
//	minXValue=0;
//	maxXValue=maxXValue-(maxXValue%100)+100;
//	int nScale;//
//	int n=(maxXValue-minXValue)%700;//700=100*7;  7 Column
//	if(n==0)
//	{
//		nScale=(maxXValue-minXValue)/700;
//	}
//	else
//	{
//		nScale=(maxXValue-minXValue)/700+1;
//	}
//	
//	xValue[0]=minXValue;
//	for(i=1;i<=7;i++)
//	{
//		xValue[i]=xValue[i-1]+100*nScale;
//	}
//	
//	chartFile.reset();
//	chartFile.skipLines(3);
//	while (chartFile.getLine())
//	{
//		chartFile.setToField (1);
//		chartFile.getFloat (ft);
//		for(i=1;i<=7;i++)
//		{
//			if(ft>=xValue[i-1] && ft<xValue[i])
//			{
//				yValue[i]++;
//			}
//		}
//	}
//	int nCount=7;
//	for(i=7;i>0;i--)
//	{
//		if(yValue[i]==0)
//		{
//			nCount--;
//		}
//		else
//		{
//			break;
//		}
//	}
//	m_MSChartCtrl.SetRowCount(nCount);
//	m_MSChartCtrl.SetColumnCount(1);
//	m_MSChartCtrl.SetColumnLabelCount(1);
//	m_MSChartCtrl.SetColumn(1);
//	m_MSChartCtrl.SetColumnLabel("Meters Traveled");	
//	
//	CString strLabel;
//	//Modified by Xie Bo 2002.5.5
//	//For Unit Transfer
//	CTermPlanDoc * pDoc=(CTermPlanDoc *)GetDocument();
//	ENUM_REPORT_TYPE enumReportType= pDoc->GetARCReportManager().GetReportType();
//	//CUnitsManager* pUM = CUnitsManager::GetInstance();
//
//	for(i=1;i<=nCount;i++)
//	{
//		m_MSChartCtrl.SetRow(i);
//		strLabel.Format("%d-%d",(int)(UNITSMANAGER->ConvertLength( (double)xValue[i-1])*SCALE_FACTOR),(int)(UNITSMANAGER->ConvertLength( (double)xValue[i])*SCALE_FACTOR));
//		DealRowLabel(strLabel);
//		m_MSChartCtrl.SetRowLabel(strLabel);
//		strLabel.Format("%d",yValue[i]);
//		m_MSChartCtrl.SetData(strLabel);
//	}
//	chartFile.closeIn();	
//	m_MSChartCtrl.SetShowLegend(FALSE);
//}
//void CRepGraphView::MissFlightDetailed()
//{
//	// load report from file
//	CString sFileName;
//	if( GetLoadReportType() == load_by_user )
//	{
//		sFileName = GetUserLoadReportFile();
//	}
//	else								// load_by_system
//	{
//		sFileName = ((CTermPlanDoc*)GetDocument())->GetTerminal().GetSimReportManager()->GetCurrentReportFileName( ((CTermPlanDoc*)GetDocument())->m_ProjInfo.path );
//	}
//	chartFile.init (sFileName, READ);
//
//	chartFile.reset();
//	chartFile.getLine();
//	chartFile.getLine();
//	char string[4][128];
//	chartFile.getField (string[0], 128);
//	chartFile.getField (string[1], 128);
//	chartFile.getField (string[2], 128);
//	chartFile.getField (string[3], 128);
//	CString strFootnote;
//	strFootnote.Format("%s %s %s %s",NoUnderline(CString(string[0])),string[1],string[2],string[3]);
//	SetHeadXYTitle("Miss Flight","Miss Time(s)","Passenger Count",strFootnote);
//	//3.)Draw it
//	chartFile.reset();
//	chartFile.skipLines(3);
//	int xValue[8];
//	int yValue[8];
//	int i;
//	for(i=0;i<8;i++)
//	{
//		xValue[i]=yValue[i]=0;
//	}
//	int maxXValue=0;
//	int minXValue=0;// max minutes in 1 day
//	float ft = 0.0;   
//	ElapsedTime value;
//	int nCount = 7;
//
//	while (chartFile.getLine())
//	{
//		chartFile.setToField (4); // go to depature time;
//		chartFile.getTime (value, TRUE);
//		if (value.asSeconds() < minXValue)
//			minXValue = value.asSeconds();
//		if (value.asSeconds() > maxXValue)
//			maxXValue = value.asSeconds();
//	}
//
//	int aPart = (maxXValue - minXValue) / 7;
//	xValue[0] = minXValue;
//	for(i=1;i<7;i++)
//	{
//		xValue[i] = minXValue + i * aPart;
//	}
//	xValue[7] = maxXValue+1;
//
//	chartFile.reset();
//	chartFile.skipLines(3);
//	while (chartFile.getLine())
//	{
//		chartFile.setToField (4);
//		chartFile.getTime (value, TRUE);
//		for(i=1;i<=7;i++)
//		{
//			if(value.asSeconds()>=xValue[i-1] && value.asSeconds()<xValue[i])
//			{
//				yValue[i]++;
//				break;
//			}
//		}
//	}
//
//
//	m_MSChartCtrl.SetRowCount(nCount);
//	m_MSChartCtrl.SetColumnCount(1);
//	m_MSChartCtrl.SetColumnLabelCount(1);
//	m_MSChartCtrl.SetColumn(1);
//	m_MSChartCtrl.SetColumnLabel("Meters Travelled");
//
//	CString strLabel;
//	CString strTemp;
//	for(i=1;i<=nCount;i++)
//	{
//		m_MSChartCtrl.SetRow(i);
////		strLabel.Format("%d-%d",(int)xValue[i-1],(int)xValue[i]);
////		strLabel.Format("%s-%s", GetAbsDateTime((long)xValue[i-1], FALSE), GetAbsDateTime((long)xValue[i], FALSE));
//		strLabel = GetAbsDateTime((long)xValue[i-1], FALSE);
//		strLabel = strLabel.Right(strLabel.GetLength()-strLabel.Find(' ')-1);
//		strTemp = GetAbsDateTime((long)xValue[i], FALSE);
//		strTemp = strTemp.Right(strTemp.GetLength()-strTemp.Find(' ')-1);
//		strLabel.Format("%s-%s",strLabel,strTemp);
//		DealRowLabel(strLabel);
//		m_MSChartCtrl.SetRowLabel(strLabel);
//		strLabel.Format("%d",yValue[i]);
//		m_MSChartCtrl.SetData(strLabel);
//	}
//	chartFile.closeIn();
//	m_MSChartCtrl.SetShowLegend(FALSE);
//
//}

//CString CRepGraphView::GetAbsDateTime(long seconds, BOOL needsec)
//{
//	CSimAndReportManager *ptSim = ((CTermPlanDoc*)GetDocument())->GetTerminal().GetSimReportManager();
//	CStartDate tstartdate = ptSim->GetStartDate();
//	ElapsedTime etime(seconds);
//	
//	CString retstr, tstr;
//	bool bAbsDate;
//	COleDateTime _tdate, _ttime;
//	int _nDtIdx;
//	tstartdate.GetDateTime(etime, bAbsDate, _tdate, _nDtIdx, _ttime);
//	if(!needsec)
//		tstr = _ttime.Format(" %H:%M");
//	else
//		tstr = _ttime.Format(" %H:%M:%S");
//	if(bAbsDate)
//	{
//		retstr = _tdate.Format("%Y-%m-%d");
//	}
//	else
//	{
//		retstr.Format("Day%d", _nDtIdx + 1 );
//	}
//	retstr += tstr;
//
//	return retstr;
//}

//void CRepGraphView::FireEvacuationDetailed()
//{
//	// load report from file
//	CString sFileName;
//	if( GetLoadReportType() == load_by_user )
//	{
//		sFileName = GetUserLoadReportFile();
//	}
//	else								// load_by_system
//	{
//		sFileName = ((CTermPlanDoc*)GetDocument())->GetTerminal().GetSimReportManager()->GetCurrentReportFileName( ((CTermPlanDoc*)GetDocument())->m_ProjInfo.path );
//	}
//	chartFile.init (sFileName, READ);
//
//	chartFile.reset();
//	chartFile.getLine();
//	chartFile.getLine();
//	char string[4][128];
//	chartFile.getField (string[0], 128);
//	chartFile.getField (string[1], 128);
//	chartFile.getField (string[2], 128);
//	chartFile.getField (string[3], 128);
//	CString strFootnote;
//	strFootnote.Format("%s %s %s %s",NoUnderline(CString(string[0])),string[1],string[2],string[3]);
//	SetHeadXYTitle("Fire Evacuation Time(Detailed)","Evacuation Time(s)","Passenger Count",strFootnote);
//	//3.)Draw it
//	chartFile.reset();
//	chartFile.skipLines(3);
//	int xValue[8];
//	int yValue[8];
//	for(int i=0;i<8;i++)
//	{
//		xValue[i]=yValue[i]=0;
//	}
//	int maxXValue=0;
//	int minXValue=1440;// max minutes in 1 day
//	float ft = 0.0;   
//	ElapsedTime value;
//	while (chartFile.getLine())
//	{
//		chartFile.setToField (4);
//		chartFile.getTime (value, TRUE);
//		if (value.asSeconds() < minXValue)
//			minXValue = value.asSeconds();
//		if (value.asSeconds() > maxXValue)
//			maxXValue = value.asSeconds();
//	}
//	minXValue=minXValue-(minXValue%10);
//	maxXValue=maxXValue-(maxXValue%10)+10;
//	int nScale;//
//	int n=(maxXValue-minXValue)%70;//70=10*7;  7 Column
//	if(n==0)
//	{
//		nScale=(maxXValue-minXValue)/70;
//	}
//	else
//	{
//		nScale=(maxXValue-minXValue)/70+1;
//	}
//	
//	xValue[0]=minXValue;
//	for(i=1;i<=7;i++)
//	{
//		xValue[i]=xValue[i-1]+10*nScale;
//	}
//	
//	chartFile.reset();
//	chartFile.skipLines(3);
//	while (chartFile.getLine())
//	{
//		chartFile.setToField (4);
//		chartFile.getTime (value, TRUE);
//		for(i=1;i<=7;i++)
//		{
//			if(value.asSeconds()>=xValue[i-1] && value.asSeconds()<xValue[i])
//			{
//				yValue[i]++;
//				break;
//			}
//		}
//	}
//	int nCount=7;
//	for(i=7;i>0;i--)
//	{
//		if(yValue[i]==0)
//		{
//			nCount--;
//		}
//		else
//		{
//			break;
//		}
//	}
//	m_MSChartCtrl.SetRowCount(nCount);
//	m_MSChartCtrl.SetColumnCount(1);
//	m_MSChartCtrl.SetColumnLabelCount(1);
//	m_MSChartCtrl.SetColumn(1);
//	m_MSChartCtrl.SetColumnLabel("Meters Travelled");
//	
//	CString strLabel;
//	for(i=1;i<=nCount;i++)
//	{
//		m_MSChartCtrl.SetRow(i);
//		strLabel.Format("%d-%d",(int)xValue[i-1],(int)xValue[i]);
//		DealRowLabel(strLabel);
//		m_MSChartCtrl.SetRowLabel(strLabel);
//		strLabel.Format("%d",yValue[i]);
//		m_MSChartCtrl.SetData(strLabel);
//	}
//	chartFile.closeIn();
//	m_MSChartCtrl.SetShowLegend(FALSE);
//}
//
//void CRepGraphView::FireEvacuationSummary()
//{
//	// load report from file
//	CString strFileName;
//	if( GetLoadReportType() == load_by_user )
//	{
//		strFileName = GetUserLoadReportFile();
//	}
//	else								// load_by_system
//	{
//		strFileName = ((CTermPlanDoc*)GetDocument())->GetTerminal().GetSimReportManager()->GetCurrentReportFileName( ((CTermPlanDoc*)GetDocument())->m_ProjInfo.path );
//	}
//	chartFile.init (strFileName, READ);
//	//2.)Draw Title,X Axis Title,Y Axis Title,Footnote
//	chartFile.reset();
//	chartFile.getLine();
//	chartFile.getLine();
//	char string[4][128];
//	chartFile.getField (string[0], 128);
//	chartFile.getField (string[1], 128);
//	chartFile.getField (string[2], 128);
//	chartFile.getField (string[3], 128);
//	CString strFootnote;
//	strFootnote.Format("%s %s %s %s",NoUnderline(CString(string[0])),string[1],string[2],string[3]);
//	
//	SetHeadXYTitle("Fire Evacuation Time (Summary)","Passenger Type","Evacuation Time(s)",strFootnote);
//	//3.)Draw it
//	CString strxValue[100];//The Pax Type Name
//	int nPaxCount=0;
//	ElapsedTime valuetime;
//	float minvalue[100];
//	float avevalue[100];
//	float maxvalue[100];
//
//	float Q1value[100];
//	float Q2value[100];
//	float Q3value[100];
//	
//	float P1value[100];
//	float P5value[100];
//	float P10value[100];
//	
//	float P90value[100];
//	float P95value[100];
//	float P99value[100];
//	
//	float Sigmavalue[100];
//	
//	char buf[128];
//	for(int k=0;k<100;k++)
//	{
//		minvalue[k]=avevalue[k]=maxvalue[k]=0;
//		Q1value[k]=Q2value[k]=Q3value[k]=P1value[k]=P5value[k]=0;
//		Sigmavalue[k] = P10value[k]=P95value[k]=P99value[k]=0;
//	}
//	chartFile.reset();
//	chartFile.skipLines(3);
//	while (chartFile.getLine())
//	{
//		chartFile.setToField(0);
//		chartFile.getField(buf,128);
//		nPaxCount+=1;
//		strxValue[nPaxCount].Format("%s",buf);//Get The First Pax Type
//		
//		chartFile.setToField (1);
//		chartFile.getTime (valuetime, TRUE);
//		minvalue[nPaxCount]=static_cast<float>(valuetime.asSeconds());
//		chartFile.setToField (2);
//		chartFile.getTime (valuetime, TRUE);
//		avevalue[nPaxCount]=static_cast<float>(valuetime.asSeconds());
//		chartFile.setToField (3);
//		chartFile.getTime (valuetime, TRUE);
//		maxvalue[nPaxCount]=static_cast<float>(valuetime.asSeconds());
//
//		
//		chartFile.setToField( 5 );
//		chartFile.getTime( valuetime, TRUE );
//		Q1value[nPaxCount]= static_cast<float>(valuetime.asSeconds());
//
//		chartFile.setToField( 6 );
//		chartFile.getTime( valuetime, TRUE );
//		Q2value[nPaxCount]= static_cast<float>(valuetime.asSeconds());
//
//		chartFile.setToField( 7 );
//		chartFile.getTime( valuetime, TRUE );
//		Q3value[nPaxCount]= static_cast<float>(valuetime.asSeconds());
//
//		chartFile.setToField( 8 );
//		chartFile.getTime( valuetime, TRUE );
//		P1value[nPaxCount]= static_cast<float>(valuetime.asSeconds());
//
//		chartFile.setToField( 9 );
//		chartFile.getTime( valuetime, TRUE );
//		P5value[nPaxCount]= static_cast<float>(valuetime.asSeconds());
//
//		chartFile.setToField( 10 );
//		chartFile.getTime( valuetime, TRUE );
//		P10value[nPaxCount]= static_cast<float>(valuetime.asSeconds());
//
//		chartFile.setToField( 11 );
//		chartFile.getTime( valuetime, TRUE );
//		P90value[nPaxCount]= static_cast<float>(valuetime.asSeconds());
//
//		chartFile.setToField( 12 );
//		chartFile.getTime( valuetime, TRUE );
//		P95value[nPaxCount]= static_cast<float>(valuetime.asSeconds());
//
//		chartFile.setToField( 13 );
//		chartFile.getTime( valuetime, TRUE );
//		P99value[nPaxCount]= static_cast<float>(valuetime.asSeconds());
//
//		chartFile.setToField( 14 );
//		chartFile.getTime( valuetime, TRUE );
//		Sigmavalue[nPaxCount]= static_cast<float>(valuetime.asSeconds());
//	}
//	
//	CString strLabel;
//	m_MSChartCtrl.SetRowCount(nPaxCount);
//	m_MSChartCtrl.SetColumnCount(13);
//	m_MSChartCtrl.SetColumnLabelCount(13);
//	m_MSChartCtrl.SetColumn(1);
//	m_MSChartCtrl.SetColumnLabel("Minimum");
//	m_MSChartCtrl.SetColumn(2);
//	m_MSChartCtrl.SetColumnLabel("Average");
//	m_MSChartCtrl.SetColumn(3);
//	m_MSChartCtrl.SetColumnLabel("Maximum");
//
//	m_MSChartCtrl.SetColumn(4);
//	m_MSChartCtrl.SetColumnLabel("Q1");
//	
//	m_MSChartCtrl.SetColumn(5);
//	m_MSChartCtrl.SetColumnLabel("Q2");
//	
//	m_MSChartCtrl.SetColumn(6);
//	m_MSChartCtrl.SetColumnLabel("Q3");
//	
//	m_MSChartCtrl.SetColumn(7);
//	m_MSChartCtrl.SetColumnLabel("P1");
//	
//	m_MSChartCtrl.SetColumn(8);
//	m_MSChartCtrl.SetColumnLabel("P5");
//	
//	m_MSChartCtrl.SetColumn(9);
//	m_MSChartCtrl.SetColumnLabel("P10");
//	
//	m_MSChartCtrl.SetColumn(10);
//	m_MSChartCtrl.SetColumnLabel("P90");
//	
//	m_MSChartCtrl.SetColumn(11);
//	m_MSChartCtrl.SetColumnLabel("P95");
//	
//	m_MSChartCtrl.SetColumn(12);
//	m_MSChartCtrl.SetColumnLabel("P99");
//	
//	m_MSChartCtrl.SetColumn(13);
//	m_MSChartCtrl.SetColumnLabel("Sigma");
//	
//
//	for(int i=1;i<=nPaxCount;i++)
//	{
//		m_MSChartCtrl.SetRow(i);
//		DealRowLabel(strxValue[i]);
//		m_MSChartCtrl.SetRowLabel(strxValue[i]);
//		m_MSChartCtrl.SetColumn(1);
//		strLabel.Format("%f",minvalue[i]);
//		m_MSChartCtrl.SetData(strLabel);
//		m_MSChartCtrl.SetRow(i);	//add by bird 2003/2/18
//		m_MSChartCtrl.SetColumn(2);
//		strLabel.Format("%f",avevalue[i]);
//		m_MSChartCtrl.SetData(strLabel);
//		m_MSChartCtrl.SetRow(i);	//add by bird 2003/2/18
//		m_MSChartCtrl.SetColumn(3);
//		strLabel.Format("%f",maxvalue[i]);
//		m_MSChartCtrl.SetData(strLabel);
//
//		m_MSChartCtrl.SetRow(i);
//		m_MSChartCtrl.SetColumn(4);
//		strLabel.Format("%f",Q1value[i]);
//		m_MSChartCtrl.SetData(strLabel);
//
//		m_MSChartCtrl.SetRow(i);
//		m_MSChartCtrl.SetColumn(5);
//		strLabel.Format("%f",Q2value[i]);
//		m_MSChartCtrl.SetData(strLabel);
//
//		m_MSChartCtrl.SetRow(i);
//		m_MSChartCtrl.SetColumn(6);
//		strLabel.Format("%f",Q3value[i]);
//		m_MSChartCtrl.SetData(strLabel);
//
//		m_MSChartCtrl.SetRow(i);
//		m_MSChartCtrl.SetColumn(7);
//		strLabel.Format("%f",P1value[i]);
//		m_MSChartCtrl.SetData(strLabel);
//
//		m_MSChartCtrl.SetRow(i);
//		m_MSChartCtrl.SetColumn(8);
//		strLabel.Format("%f",P5value[i]);
//		m_MSChartCtrl.SetData(strLabel);
//
//		m_MSChartCtrl.SetRow(i);
//		m_MSChartCtrl.SetColumn(9);
//		strLabel.Format("%f",P10value[i]);
//		m_MSChartCtrl.SetData(strLabel);
//
//		m_MSChartCtrl.SetRow(i);
//		m_MSChartCtrl.SetColumn(10);
//		strLabel.Format("%f",P90value[i]);
//		m_MSChartCtrl.SetData(strLabel);
//
//		m_MSChartCtrl.SetRow(i);
//		m_MSChartCtrl.SetColumn(11);
//		strLabel.Format("%f",P95value[i]);
//		m_MSChartCtrl.SetData(strLabel);
//
//		m_MSChartCtrl.SetRow(i);
//		m_MSChartCtrl.SetColumn(12);
//		strLabel.Format("%f",P99value[i]);
//		m_MSChartCtrl.SetData(strLabel);
//
//		m_MSChartCtrl.SetRow(i);
//		m_MSChartCtrl.SetColumn(13);
//		strLabel.Format("%f",Sigmavalue[i]);
//		m_MSChartCtrl.SetData(strLabel);
//	}
//	
//	chartFile.closeIn();
//	m_MSChartCtrl.SetShowLegend(TRUE);
//}
//
//void CRepGraphView::UtilizationTimeDetailed()
//{
//	// load report from file
//	CString sFileName;
//	if( GetLoadReportType() == load_by_user )
//	{
//		sFileName = GetUserLoadReportFile();
//	}
//	else								// load_by_system
//	{
//		sFileName = ((CTermPlanDoc*)GetDocument())->GetTerminal().GetSimReportManager()->GetCurrentReportFileName( ((CTermPlanDoc*)GetDocument())->m_ProjInfo.path );
//	}
//	chartFile.init (sFileName, READ);
//	//2.)Draw Title,X Axis Title,Y Axis Title,Footnote
//	chartFile.reset();
//	chartFile.getLine();
//	chartFile.getLine();
//	char string[4][128];
//	chartFile.getField (string[0], 128);
//	chartFile.getField (string[1], 128);
//	chartFile.getField (string[2], 128);
//	chartFile.getField (string[3], 128);
//	CString strFootnote;
//	strFootnote.Format("%s %s %s %s",NoUnderline(CString(string[0])),string[1],string[2],string[3]);
//	SetHeadXYTitle("Utilization Time(Detailed)","Processor","Service Time(Hours)",strFootnote);
//	//3.)Draw it
//	CString  strxValue[1440],stryValue[1440];
//	CString strProcessor[1440];
//	int nCount=0;
//	ElapsedTime value;
//	int i=0;
//	char str[128];
//	chartFile.reset();
//	chartFile.skipLines(3);
//	while (chartFile.getLine())
//	{
//		nCount++;
//		chartFile.setToField (0);
//		chartFile.getField(str,128);
//		strProcessor[nCount].Format("%s",str);
//
//		chartFile.setToField (4);
//		chartFile.getTime (value, TRUE);
//		strxValue[nCount].Format("%d",value.asHours());
//		
//		chartFile.setToField (5);
//		chartFile.getTime (value, TRUE);
//		stryValue[nCount].Format("%d",value.asHours());
//	}
//	m_MSChartCtrl.SetRowCount(nCount);
//	m_MSChartCtrl.SetColumnCount(2);
//	m_MSChartCtrl.SetColumnLabelCount(2);
//	m_MSChartCtrl.SetColumn(1);
//	m_MSChartCtrl.SetColumnLabel("Time in Service");
//	m_MSChartCtrl.SetColumn(2);
//	m_MSChartCtrl.SetColumnLabel("Idle Time");
//	
//	CString strLabel;
//	for(i=1;i<=nCount;i++)
//	{
//		m_MSChartCtrl.SetRow(i);
//		strLabel.Format("%s",strProcessor[i]);
//		DealRowLabel(strLabel);
//		m_MSChartCtrl.SetRowLabel(strLabel);
//		m_MSChartCtrl.SetColumn(1);
//		strLabel.Format("%s",strxValue[i]);
//		m_MSChartCtrl.SetData(strLabel);
//		m_MSChartCtrl.SetRow(i);	//add by bird 2003/2/18
//		m_MSChartCtrl.SetColumn(2);
//		strLabel.Format("%s",stryValue[i]);
//		m_MSChartCtrl.SetData(strLabel);
//	}
//	chartFile.closeIn();
//	m_MSChartCtrl.SetShowLegend(TRUE);
//}
//
//void CRepGraphView::UtilizationTimeAvailableDetailed()
//{
//	// load report from file
//	CString sFileName;
//	if( GetLoadReportType() == load_by_user )
//	{
//		sFileName = GetUserLoadReportFile();
//	}
//	else								// load_by_system
//	{
//		sFileName = ((CTermPlanDoc*)GetDocument())->GetTerminal().GetSimReportManager()->GetCurrentReportFileName( ((CTermPlanDoc*)GetDocument())->m_ProjInfo.path );
//	}
//	chartFile.init (sFileName, READ);
//	//2.)Draw Title,X Axis Title,Y Axis Title,Footnote
//	chartFile.reset();
//	chartFile.getLine();
//	chartFile.getLine();
//	char string[4][128];
//	chartFile.getField (string[0], 128);
//	chartFile.getField (string[1], 128);
//	chartFile.getField (string[2], 128);
//	chartFile.getField (string[3], 128);
//	CString strFootnote;
//	strFootnote.Format("%s %s %s %s",NoUnderline(CString(string[0])),string[1],string[2],string[3]);
//	SetHeadXYTitle("Time Available(Detailed)","Processor","Time Available(Hours)",strFootnote);
//	//3.)Draw it
//	CString  strxValue[1440],stryValue[1440];
//	CString strProcessor[1440];
//	int nCount=0;
//	ElapsedTime value;
//	int i=0;
//	char str[128];
//	chartFile.reset();
//	chartFile.skipLines(3);
//	while (chartFile.getLine())
//	{
//		nCount++;
//		chartFile.setToField (0);
//		chartFile.getField(str,128);
//		strProcessor[nCount].Format("%s",str);
//
//		chartFile.setToField (1);
//		chartFile.getTime (value, TRUE);
//		strxValue[nCount].Format("%d",value.asHours());
//		
//		chartFile.setToField (2);
//		chartFile.getTime (value, TRUE);
//		stryValue[nCount].Format("%d",value.asHours());
//	}
//	m_MSChartCtrl.SetRowCount(nCount);
//	m_MSChartCtrl.SetColumnCount(2);
//	m_MSChartCtrl.SetColumnLabelCount(2);
//	m_MSChartCtrl.SetColumn(1);
//	m_MSChartCtrl.SetColumnLabel("Scheduled Time");
//	m_MSChartCtrl.SetColumn(2);
//	m_MSChartCtrl.SetColumnLabel("Overtime");
//	
//	CString strLabel;
//	for(i=1;i<=nCount;i++)
//	{
//		m_MSChartCtrl.SetRow(i);
//		strLabel.Format("%s",strProcessor[i]);
//		DealRowLabel(strLabel);
//		m_MSChartCtrl.SetRowLabel(strLabel);
//		m_MSChartCtrl.SetColumn(1);
//		strLabel.Format("%s",strxValue[i]);
//		m_MSChartCtrl.SetData(strLabel);
//		m_MSChartCtrl.SetRow(i);	//add by bird 2003/2/18
//		m_MSChartCtrl.SetColumn(2);
//		strLabel.Format("%s",stryValue[i]);
//		m_MSChartCtrl.SetData(strLabel);
//	}
//	chartFile.closeIn();
//	m_MSChartCtrl.SetShowLegend(TRUE);
//}
//
//void CRepGraphView::UtilizationPercentageDetailed()
//{
//	// load report from file
//	CString sFileName;
//	if( GetLoadReportType() == load_by_user )
//	{
//		sFileName = GetUserLoadReportFile();
//	}
//	else								// load_by_system
//	{
//		sFileName = ((CTermPlanDoc*)GetDocument())->GetTerminal().GetSimReportManager()->GetCurrentReportFileName( ((CTermPlanDoc*)GetDocument())->m_ProjInfo.path );
//	}
//
//	chartFile.init (sFileName, READ);
//	//2.)Draw Title,X Axis Title,Y Axis Title,Footnote
//	chartFile.reset();
//	chartFile.getLine();
//	chartFile.getLine();
//	char string[4][128];
//	chartFile.getField (string[0], 128);
//	chartFile.getField (string[1], 128);
//	chartFile.getField (string[2], 128);
//	chartFile.getField (string[3], 128);
//	CString strFootnote;
//	strFootnote.Format("%s %s %s %s",NoUnderline(CString(string[0])),string[1],string[2],string[3]);
//	SetHeadXYTitle("Utilization Percentage(Detailed)","Processor","Utilization(100%)",strFootnote);
//	//3.)Draw it
//	CString  strxValue[1440],stryValue[1440];
//	int nCount=0;
//	ElapsedTime value;
//	int i=0;
//	char str[128];
//	chartFile.reset();
//	chartFile.skipLines(3);
//	while (chartFile.getLine())
//	{
//		nCount++;
//	}
//	chartFile.reset();
//	chartFile.skipLines(3);
//	while (chartFile.getLine())
//	{
//		i++;
//		chartFile.setToField (0);
//		chartFile.getField(str,128);
//		strxValue[i].Format("%s",str);
//		chartFile.setToField (4);
//		chartFile.getTime (value, TRUE);
//		//value.printTime(str);
//		double dMin = value.asMinutes();
//		stryValue[i].Format("%f",dMin/(24*60)*100.0);
//	}
//	m_MSChartCtrl.SetRowCount(nCount);
//	m_MSChartCtrl.SetColumnCount(1);
//	m_MSChartCtrl.SetColumnLabelCount(1);
//	m_MSChartCtrl.SetColumn(1);
//	m_MSChartCtrl.SetColumnLabel("Meters Travelled");
//	
//	CString strLabel;
//	for(i=1;i<=nCount;i++)
//	{
//		m_MSChartCtrl.SetRow(i);
//		strLabel.Format("%s",strxValue[i]);
//		DealRowLabel(strLabel);
//		m_MSChartCtrl.SetRowLabel(strLabel);
//		strLabel.Format("%s",stryValue[i]);
//		m_MSChartCtrl.SetData(strLabel);
//	}
//	chartFile.closeIn();
//}
//
//void CRepGraphView::UtilizationTimeSummary()
//{
//	// load report from file
//	CString sFileName;
//	if( GetLoadReportType() == load_by_user )
//	{
//		sFileName = GetUserLoadReportFile();
//	}
//	else								// load_by_system
//	{
//		sFileName = ((CTermPlanDoc*)GetDocument())->GetTerminal().GetSimReportManager()->GetCurrentReportFileName( ((CTermPlanDoc*)GetDocument())->m_ProjInfo.path );
//	}
//	chartFile.init (sFileName, READ);
//	//2.)Draw Title,X Axis Title,Y Axis Title,Footnote
//	chartFile.reset();
//	chartFile.getLine();
//	chartFile.getLine();
//	char string[4][128];
//	chartFile.getField (string[0], 128);
//	chartFile.getField (string[1], 128);
//	chartFile.getField (string[2], 128);
//	chartFile.getField (string[3], 128);
//	CString strFootnote;
//	strFootnote.Format("%s %s %s %s",NoUnderline(CString(string[0])),string[1],string[2],string[3]);
//	SetHeadXYTitle("Utilization Time(Summary)","Processor","Utilization Time(Hours)",strFootnote);
//	//3.)Draw it
////	CString  strxValue[1440],stryValue[1440],strzValue[1440];
////	int nCount=0;
////	ElapsedTime value;
////	char str[128];
////	int i=0;
////	chartFile.reset();
////	chartFile.skipLines(3);
////	while (chartFile.getLine())
////	{
////		nCount++;
////	}
////	chartFile.reset();
////	chartFile.skipLines(3);
////	while (chartFile.getLine())
////	{
////		i++;
////		chartFile.setToField(0);
////		chartFile.getField(str,128);
////		strxValue[i].Format("%s",str);
////		chartFile.setToField (5);
////		chartFile.getTime (value, TRUE);
////		stryValue[i].Format("%f",value.asMinutes());
////		chartFile.setToField (6);
////		chartFile.getTime (value, TRUE);
////		strzValue[i].Format("%f",value.asMinutes());
////	}
//////	m_MSChartCtrl.SetRowCount(nCount);
//////	m_MSChartCtrl.SetColumnCount(2);
//////	for(i=1;i<=nCount;i++)
//////	{
//////		m_MSChartCtrl.SetRow(i);
//////		m_MSChartCtrl.SetRowLabel(strxValue[i]);
//////		m_MSChartCtrl.SetColumn(1);
//////		m_MSChartCtrl.SetColumnLabel("Service Time");	
//////		m_MSChartCtrl.SetData(stryValue[i]);
//////		m_MSChartCtrl.SetColumn(2);
//////		m_MSChartCtrl.SetColumnLabel("Idle Time");
//////		m_MSChartCtrl.SetData(strzValue[i]);
//////	}
////	m_MSChartCtrl.SetRowCount(2*nCount);
////	m_MSChartCtrl.SetColumnCount(1);
////	m_MSChartCtrl.SetColumnLabelCount(1);
////	m_MSChartCtrl.SetColumn(1);
////	m_MSChartCtrl.SetColumnLabel("Meters Travelled");
////
////	for(i=1;i<=nCount;i++)
////	{
////		m_MSChartCtrl.SetRow(2*i-1);
////		m_MSChartCtrl.SetRowLabel(strxValue[i]+"-Service Time");
////		m_MSChartCtrl.SetData(stryValue[i]);
////		m_MSChartCtrl.SetRow(2*i);
////		m_MSChartCtrl.SetRowLabel(strxValue[i]+"-Idle Time");
////		m_MSChartCtrl.SetData(strzValue[i]);
////	}
////	chartFile.closeIn();
////	//m_MSChartCtrl.SetShowLegend(TRUE);
//	CString  strxValue[1440],stryValue[1440];
//	CString strProcessor[1440];
//	int nCount=0;
//	ElapsedTime value;
//	int i=0;
//	char str[128];
//	chartFile.reset();
//	chartFile.skipLines(3);
//	while (chartFile.getLine())
//	{
//		nCount++;
//		chartFile.setToField (0);
//		chartFile.getField(str,128);
//		strProcessor[nCount].Format("%s",str);
//
//		chartFile.setToField (5);
//		chartFile.getTime (value, TRUE);
//		strxValue[nCount].Format("%d",value.asHours());
//		
//		chartFile.setToField (6);
//		chartFile.getTime (value, TRUE);
//		stryValue[nCount].Format("%d",value.asHours());
//	}
//	m_MSChartCtrl.SetRowCount(nCount);
//	m_MSChartCtrl.SetColumnCount(2);
//	m_MSChartCtrl.SetColumnLabelCount(2);
//	m_MSChartCtrl.SetColumn(1);
//	m_MSChartCtrl.SetColumnLabel("Time in Service");
//	m_MSChartCtrl.SetColumn(2);
//	m_MSChartCtrl.SetColumnLabel("Idle Time");
//	
//	CString strLabel;
//	for(i=1;i<=nCount;i++)
//	{
//		m_MSChartCtrl.SetRow(i);
//		strLabel.Format("%s",strProcessor[i]);
//		DealRowLabel(strLabel);
//		m_MSChartCtrl.SetRowLabel(strLabel);
//		m_MSChartCtrl.SetColumn(1);
//		strLabel.Format("%s",strxValue[i]);
//		m_MSChartCtrl.SetData(strLabel);
//		m_MSChartCtrl.SetRow(i);	//add by bird 2003/2/18
//		m_MSChartCtrl.SetColumn(2);
//		strLabel.Format("%s",stryValue[i]);
//		m_MSChartCtrl.SetData(strLabel);
//	}
//	chartFile.closeIn();
//	m_MSChartCtrl.SetShowLegend(TRUE);
//}
//
//void CRepGraphView::UtilizationTimeAvailableSummary()
//{
//	// load report from file
//	CString sFileName;
//	if( GetLoadReportType() == load_by_user )
//	{
//		sFileName = GetUserLoadReportFile();
//	}
//	else								// load_by_system
//	{
//		sFileName = ((CTermPlanDoc*)GetDocument())->GetTerminal().GetSimReportManager()->GetCurrentReportFileName( ((CTermPlanDoc*)GetDocument())->m_ProjInfo.path );
//	}
//	chartFile.init (sFileName, READ);
//	//2.)Draw Title,X Axis Title,Y Axis Title,Footnote
//	chartFile.reset();
//	chartFile.getLine();
//	chartFile.getLine();
//	char string[4][128];
//	chartFile.getField (string[0], 128);
//	chartFile.getField (string[1], 128);
//	chartFile.getField (string[2], 128);
//	chartFile.getField (string[3], 128);
//	CString strFootnote;
//	strFootnote.Format("%s %s %s %s",NoUnderline(CString(string[0])),string[1],string[2],string[3]);
//	SetHeadXYTitle("Time Available(Summary)","Processor","Time Available(Hours)",strFootnote);
//	//3.)Draw it
////	CString  strxValue[1440],stryValue[1440],strzValue[1440];
////	int nCount=0;
////	ElapsedTime value;
////	char str[128];
////	int i=0;
////	chartFile.reset();
////	chartFile.skipLines(3);
////	while (chartFile.getLine())
////	{
////		nCount++;
////	}
////	chartFile.reset();
////	chartFile.skipLines(3);
////	while (chartFile.getLine())
////	{
////		i++;
////		chartFile.setToField(0);
////		chartFile.getField(str,128);
////		strxValue[i].Format("%s",str);
////		chartFile.setToField (2);
////		chartFile.getTime (value, TRUE);
////		stryValue[i].Format("%f",value.asMinutes());
////		chartFile.setToField (3);
////		chartFile.getTime (value, TRUE);
////		strzValue[i].Format("%f",value.asMinutes());
////	}
////	m_MSChartCtrl.SetRowCount(2*nCount);
////	m_MSChartCtrl.SetColumnCount(1);
////	m_MSChartCtrl.SetColumnLabelCount(1);
////	m_MSChartCtrl.SetColumn(1);
////	m_MSChartCtrl.SetColumnLabel("Meters Travelled");
////
////	for(i=1;i<=nCount;i++)
////	{
////		m_MSChartCtrl.SetRow(2*i-1);
////		m_MSChartCtrl.SetRowLabel(strxValue[i]+"-Schedule Time");
//////		m_MSChartCtrl.SetColumn(1);
//////		m_MSChartCtrl.SetColumnLabel("Schedule Time");	
////		m_MSChartCtrl.SetData(stryValue[i]);
//////		m_MSChartCtrl.SetColumn(2);
//////		m_MSChartCtrl.SetColumnLabel("Overtime");
////		m_MSChartCtrl.SetRow(2*i);
////		m_MSChartCtrl.SetRowLabel(strxValue[i]+"-Overtime");
////		m_MSChartCtrl.SetData(strzValue[i]);
////	}
////	chartFile.closeIn();
////	//m_MSChartCtrl.SetShowLegend(TRUE);
//	CString  strxValue[1440],stryValue[1440];
//	CString strProcessor[1440];
//	int nCount=0;
//	ElapsedTime value;
//	int i=0;
//	char str[128];
//	chartFile.reset();
//	chartFile.skipLines(3);
//	while (chartFile.getLine())
//	{
//		nCount++;
//		chartFile.setToField (0);
//		chartFile.getField(str,128);
//		strProcessor[nCount].Format("%s",str);
//
//		chartFile.setToField (2);
//		chartFile.getTime (value, TRUE);
//		strxValue[nCount].Format("%d",value.asHours());
//		
//		chartFile.setToField (3);
//		chartFile.getTime (value, TRUE);
//		stryValue[nCount].Format("%d",value.asHours());
//	}
//	m_MSChartCtrl.SetRowCount(nCount);
//	m_MSChartCtrl.SetColumnCount(2);
//	m_MSChartCtrl.SetColumnLabelCount(2);
//	m_MSChartCtrl.SetColumn(1);
//	m_MSChartCtrl.SetColumnLabel("Scheduled Time");
//	m_MSChartCtrl.SetColumn(2);
//	m_MSChartCtrl.SetColumnLabel("Overtime");
//	
//	CString strLabel;
//	for(i=1;i<=nCount;i++)
//	{
//		m_MSChartCtrl.SetRow(i);
//		strLabel.Format("%s",strProcessor[i]);
//		DealRowLabel(strLabel);
//		m_MSChartCtrl.SetRowLabel(strLabel);
//		
//		m_MSChartCtrl.SetColumn(1);
//		strLabel.Format("%s",strxValue[i]);
//		m_MSChartCtrl.SetData(strLabel);
//		m_MSChartCtrl.SetRow(i);	//add by bird 2003/2/18
//		m_MSChartCtrl.SetColumn(2);
//		strLabel.Format("%s",stryValue[i]);
//		m_MSChartCtrl.SetData(strLabel);
//	}
//	chartFile.closeIn();
//	m_MSChartCtrl.SetShowLegend(TRUE);
//}
//
//void CRepGraphView::UtilizationPercentageSummary()
//{
////	char filename[128];
////	//1.)Init chartFile
////	PROJMANAGER->getReportFileName( ((CTermPlanDoc*)GetDocument())->m_ProjInfo.path, UtilizationReportFile, filename);
////	chartFile.init (filename, READ);
////	//2.)Draw Title,X Axis Title,Y Axis Title,Footnote
////	chartFile.reset();
////	chartFile.getLine();
////	chartFile.getLine();
////	char string[4][128];
////	chartFile.getField (string[0], 128);
////	chartFile.getField (string[1], 128);
////	chartFile.getField (string[2], 128);
////	chartFile.getField (string[3], 128);
////	CString strFootnote;
////	strFootnote.Format("%s %s %s %s",NoUnderline(CString(string[0])),string[1],string[2],string[3]);
////	SetHeadXYTitle("Utilization Percentage(Summary)","All Processors","Utilization(100%)",strFootnote);
////	//3.)Draw it
////	CString  strxValue[1440],stryValue[1440];
////	int nCount=0;
////	ElapsedTime value;
////	int i=0;
////	char str[128];
////	chartFile.reset();
////	chartFile.skipLines(3);
////	while (chartFile.getLine())
////	{
////		nCount++;
////	}
////	chartFile.reset();
////	chartFile.skipLines(3);
////	while (chartFile.getLine())
////	{
////		i++;
////		chartFile.setToField(0);
////		chartFile.getField(str,128);
////		strxValue[i].Format("%s",str);
////		chartFile.setToField (5);
////		chartFile.getTime (value, TRUE);
////		stryValue[i].Format("%f",value.asMinutes()/(24*60));
////	}
////	m_MSChartCtrl.SetRowCount(nCount);
////	m_MSChartCtrl.SetColumnCount(1);
////	CString strLabel;
////	for(i=1;i<=nCount;i++)
////	{
////		m_MSChartCtrl.SetRow(i);
////		m_MSChartCtrl.SetRowLabel(strxValue[i]);
////		strLabel.Format("%s",stryValue[i]);
////		m_MSChartCtrl.SetData(stryValue[i]);
////	}
////	chartFile.closeIn();
//
//	// load report from file
//	CString sFileName;
//	if( GetLoadReportType() == load_by_user )
//	{
//		sFileName = GetUserLoadReportFile();
//	}
//	else								// load_by_system
//	{
//		sFileName = ((CTermPlanDoc*)GetDocument())->GetTerminal().GetSimReportManager()->GetCurrentReportFileName( ((CTermPlanDoc*)GetDocument())->m_ProjInfo.path );
//	}
//	chartFile.init (sFileName, READ);
//	//2.)Draw Title,X Axis Title,Y Axis Title,Footnote
//	chartFile.reset();
//	chartFile.getLine();
//	chartFile.getLine();
//	char string[4][128];
//	chartFile.getField (string[0], 128);
//	chartFile.getField (string[1], 128);
//	chartFile.getField (string[2], 128);
//	chartFile.getField (string[3], 128);
//	CString strFootnote;
//	strFootnote.Format("%s %s %s %s",NoUnderline(CString(string[0])),string[1],string[2],string[3]);
//	SetHeadXYTitle("Utilization Percentage(Summary)","Processor","Utilization(100%)",strFootnote);
//	//3.)Draw it
//	CString  strxValue[1440],stryValue[1440];
//	int nCount=0;
//	ElapsedTime value;
//	int i=0;
//	char str[128];
//	chartFile.reset();
//	chartFile.skipLines(3);
//	while (chartFile.getLine())
//	{
//		nCount++;
//	}
//	chartFile.reset();
//	chartFile.skipLines(3);
//	while (chartFile.getLine())
//	{
//		i++;
//		chartFile.setToField (0);
//		chartFile.getField(str,128);
//		strxValue[i].Format("%s",str);
//		chartFile.setToField (5);
//		chartFile.getTime (value, TRUE);
//		//value.printTime(str);
//		double dMin = value.asMinutes();
//		stryValue[i].Format("%f",dMin/(24*60)*100);
//	}
//	m_MSChartCtrl.SetRowCount(nCount);
//	m_MSChartCtrl.SetColumnCount(1);
//	m_MSChartCtrl.SetColumnLabelCount(1);
//	m_MSChartCtrl.SetColumn(1);
//	m_MSChartCtrl.SetColumnLabel("Meters Travelled");
//
//	CString strLabel;
//	for(i=1;i<=nCount;i++)
//	{
//		m_MSChartCtrl.SetRow(i);
//		strLabel.Format("%s",strxValue[i]);
//		DealRowLabel(strLabel);
//		m_MSChartCtrl.SetRowLabel(strLabel);
//		strLabel.Format("%s",stryValue[i]);
//		m_MSChartCtrl.SetData(strLabel);
//	}
//	chartFile.closeIn();
//}

void CRepGraphView::ResetAllContent()
{
	m_MSChartCtrl.ShowWindow(SW_HIDE);
	m_MSChartCtrl.SetShowLegend(FALSE);
	m_comboChartSelect.ResetContent();
	m_comboChartType.SetCurSel(1);
}

void CRepGraphView::OnPrintBtn() 
{
	// TODO: Add your control notification handler code here
	//SendMessage(WM_PRINT);
	CDC dc;
	CPrintDialog printDlg(FALSE);
	if(printDlg.DoModal() == IDCANCEL )
		return;
	dc.Attach( printDlg.GetPrinterDC() );
	dc.m_bPrinting = TRUE;
	CString sTitle;
	sTitle = "Flight Schedule";
	DOCINFO di;
	::ZeroMemory( &di, sizeof(DOCINFO) );
	di.cbSize = sizeof( DOCINFO );
	di.lpszDocName = sTitle;
	
	BOOL bPrintingOK = dc.StartDoc( &di );
	
	CPrintInfo info;
	//info.m_rectDraw.SetRect( 0,0,dc.GetDeviceCaps( HORZRES ),dc.GetDeviceCaps( VERTRES ));
	
	OnBeginPrinting( &dc, &info );
	//	for( UINT page = info.GetMinPage(); page < info.GetMaxPage() && bPrintingOK; page++ )
	//	{
	//		info.m_nCurPage = page;
	//OnPrint( &dc, &info );
	LPDEVMODE lp = (LPDEVMODE) ::GlobalLock(printDlg.m_pd.hDevMode);
	ASSERT(lp);
/*
	if(lp->dmOrientation == DMORIENT_LANDSCAPE)
	{
		m_rclBounds.left = 800;
		m_rclBounds.top = 200;
		m_rclBounds.right = 3300;
		m_rclBounds.bottom = 2800;
	}
	else
	{
		if(lp->dmOrientation == DMORIENT_PORTRAIT)
		{
			
			m_rclBounds.left = 200;
			m_rclBounds.top = 800;
			m_rclBounds.right = 2800;
			m_rclBounds.bottom = 3000;
		}
		else
		{
			AfxMessageBox("Error About Print");
			return;
		}
	}
*/
	m_rclBounds.left = 300;
	m_rclBounds.top = 300;
	m_rclBounds.right = dc.GetDeviceCaps( HORZRES ) -300;
	m_rclBounds.bottom = dc.GetDeviceCaps( VERTRES ) -300;
				
	::GlobalUnlock(printDlg.m_pd.hDevMode);
	OnDraw(&dc);
	bPrintingOK = TRUE;
	//	}
	OnEndPrinting( &dc, &info );
	
	if( bPrintingOK )
		dc.EndDoc();
	else
		dc.AbortDoc();
	dc.Detach();
}

//void CRepGraphView::PaxCountDetailed()
//{
//	//AfxMessageBox("PaxCountDetailed");
//	// load report from file
//	CString sFileName;
//	if( GetLoadReportType() == load_by_user )
//	{
//		sFileName = GetUserLoadReportFile();
//	}
//	else								// load_by_system
//	{
//		sFileName = ((CTermPlanDoc*)GetDocument())->GetTerminal().GetSimReportManager()->GetCurrentReportFileName( ((CTermPlanDoc*)GetDocument())->m_ProjInfo.path );
//	}
//	chartFile.init (sFileName, READ);
//	//2.)Draw Title,X Axis Title,Y Axis Title,Footnote
//	chartFile.reset();
//	chartFile.getLine();
//	chartFile.getLine();
//	char string[4][128];
//	chartFile.getField (string[0], 128);
//	chartFile.getField (string[1], 128);
//	chartFile.getField (string[2], 128);
//	chartFile.getField (string[3], 128);
//	CString strFootnote;
//	strFootnote.Format("%s %s %s %s",NoUnderline(CString(string[0])),string[1],string[2],string[3]);
//	SetHeadXYTitle("Space Occupancy(Detailed)","Time of Day","Occupant Count",strFootnote);
//	//3.)Draw it
//	int nPaxCount=0;	
//	char buf[128];
//	CString strTemp;
//	float value = 0.0;
//	ElapsedTime valuetime;
//	int hours,minutes;
//	int interval = GetInterval(string[1]);	//interval, unit: minute
//	CString strxValue[1000];//The Pax Type Name
//	float **yValue;
//	yValue = new float*[1000];
//	for(int k = 0; k < 1000; k++)
//	{
//		yValue[k] = new float[25*60/interval+1];
//	}
//	for(k=0;k<1000;k++)
//	{
//		for(int i=0;i<25*60/interval + 1;i++)
//		{
//			yValue[k][i]=0;
//		}
//	}
//	
//
//	int i=0;
//	chartFile.reset();
//	chartFile.skipLines(3);
//
//	if(!chartFile.getLine())
//	{
//		chartFile.closeIn();
//		return;
//	}
//	chartFile.setToField(0);
//	chartFile.getField(buf,128);
//	nPaxCount+=1;
//	strxValue[nPaxCount].Format("%s",buf);//Get The First Pax Type
//
//	int IntervalNum = 1;
//	chartFile.seekToLine(3);
//	while (chartFile.getLine())
//	{
//		chartFile.setToField(0);
//		chartFile.getField(buf,128);
//		strTemp.Format("%s",buf);
//		if(strxValue[nPaxCount]!=strTemp)
//		{
//			nPaxCount++;
//			strxValue[nPaxCount]=strTemp;
//			IntervalNum = 1;
//		}
//
//		chartFile.setToField (2);
//		chartFile.getFloat (value);
//		chartFile.setToField(1);
//		chartFile.getTime (valuetime, TRUE);
//		hours=(int)valuetime.asHours();
//		minutes=(int)valuetime.asMinutes();
//		
//		// TRACE("the time is %d hours,or %d minutes\n",hours,minutes);
//		yValue[nPaxCount][IntervalNum++]=value;
//	}
//	
//	// add by bird 2003/10/13
//	ElapsedTime BeginTime, EndTime;
//	getTime( string[1], BeginTime, EndTime );
//	int nColNum = ( EndTime.asMinutes() - BeginTime.asMinutes() )/ interval + 1;
//	
//	m_MSChartCtrl.SetRowCount(nColNum);
//	m_MSChartCtrl.SetColumnCount(nPaxCount);
//	m_MSChartCtrl.SetColumnLabelCount(nPaxCount);
//	
//	CString strLabel;
//	for(i=1;i<=nColNum;i++)
//	{
//		ElapsedTime detaTime( interval * (i-1) * 60l );
//		ElapsedTime time = BeginTime + detaTime;
//		strLabel.Format("%s", time.printTime(0));
//
//		m_MSChartCtrl.SetRow(i);
//		DealRowLabel(strLabel);
//		m_MSChartCtrl.SetRowLabel(strLabel);
//		
//		for(k=1;k<=nPaxCount;k++)
//		{
//			m_MSChartCtrl.SetRow(i);	//add by bird 2003/2/18
//			m_MSChartCtrl.SetColumn(k);
//			m_MSChartCtrl.SetColumnLabel(strxValue[k]);	
//			strLabel.Format("%f",yValue[k][i]);
//			m_MSChartCtrl.SetData(strLabel);
//		}
//		
//	}
//	for(k = 0; k < 1000; k++)
//	{
//		delete[] yValue[k];
//	}
//	delete[] yValue;
//	chartFile.closeIn();
//	m_MSChartCtrl.SetShowLegend(TRUE);
//}
//
////DEL void CRepGraphView::PaxCountSummary()
////DEL {
////DEL 	//AfxMessageBox("PaxCountSummary");
////DEL }
//
//void CRepGraphView::PaxCountPaxPeriod()
//{
//	//AfxMessageBox("PaxCountDetailed");
//	// load report from file
//	CString sFileName;
//	if( GetLoadReportType() == load_by_user )
//	{
//		sFileName = GetUserLoadReportFile();
//	}
//	else								// load_by_system
//	{
//		sFileName = ((CTermPlanDoc*)GetDocument())->GetTerminal().GetSimReportManager()->GetCurrentReportFileName( ((CTermPlanDoc*)GetDocument())->m_ProjInfo.path );
//	}
//	chartFile.init (sFileName, READ);
//	//2.)Draw Title,X Axis Title,Y Axis Title,Footnote
//	chartFile.reset();
//	chartFile.getLine();
//	chartFile.getLine();
//	char string[4][128];
//	chartFile.getField (string[0], 128);
//	chartFile.getField (string[1], 128);
//	chartFile.getField (string[2], 128);
//	chartFile.getField (string[3], 128);
//	CString strFootnote;
//	strFootnote.Format("%s %s %s %s",NoUnderline(CString(string[0])),string[1],string[2],string[3]);
//	SetHeadXYTitle("Passengers/Period","Passenger Type","Average Number of Passenger",strFootnote);
//	//3.)Draw it
//	CString strxValue[1000];//The Pax Type Name
//	int nPaxCount=0;
//	float yValue[1000];
//	char buf[128];
//	CString strTemp;
//	float value = 0.0;
//	for(int k=0;k<1000;k++)
//	{
//		yValue[k]=0;	
//	}
//	
//	int i=0;
//	chartFile.reset();
//	chartFile.skipLines(3);
//	
//	while (chartFile.getLine())
//	{
//		chartFile.setToField(0);
//		chartFile.getField(buf,128);
//		strTemp.Format("%s",buf);
//		if(strxValue[nPaxCount]!=strTemp)
//		{
//			nPaxCount++;
//			strxValue[nPaxCount]=strTemp;
//		}
//
//		chartFile.setToField (3);
//		chartFile.getFloat (value);
//		yValue[nPaxCount]=value;
//	}
//	
//	m_MSChartCtrl.SetRowCount(nPaxCount);
//	m_MSChartCtrl.SetColumnCount(1);
//	m_MSChartCtrl.SetColumnLabelCount(1);
//	m_MSChartCtrl.SetColumn(1);
//	m_MSChartCtrl.SetColumnLabel("Passenger Type");
//	
//	CString strLabel;
//	for(i=1;i<=nPaxCount;i++)
//	{
//		m_MSChartCtrl.SetRow(i);
//		strLabel.Format("%s",strxValue[i]);
//		DealRowLabel(strLabel);
//		m_MSChartCtrl.SetRowLabel(strLabel);
//		
//		strLabel.Format("%f",yValue[i]);
//		m_MSChartCtrl.SetData(strLabel);
//	}
//	chartFile.closeIn();
//	m_MSChartCtrl.SetShowLegend(FALSE);
//}
//
//void CRepGraphView::PaxCountMaxPaxCount()
//{
//	//AfxMessageBox("PaxCountDetailed");
//	CString sFileName;
//	if( GetLoadReportType() == load_by_user )
//	{
//		sFileName = GetUserLoadReportFile();
//	}
//	else								// load_by_system
//	{
//		sFileName = ((CTermPlanDoc*)GetDocument())->GetTerminal().GetSimReportManager()->GetCurrentReportFileName( ((CTermPlanDoc*)GetDocument())->m_ProjInfo.path );
//	}
//	chartFile.init (sFileName, READ);
//	//2.)Draw Title,X Axis Title,Y Axis Title,Footnote
//	chartFile.reset();
//	chartFile.getLine();
//	chartFile.getLine();
//	char string[4][128];
//	chartFile.getField (string[0], 128);
//	chartFile.getField (string[1], 128);
//	chartFile.getField (string[2], 128);
//	chartFile.getField (string[3], 128);
//	CString strFootnote;
//	strFootnote.Format("%s %s %s %s",NoUnderline(CString(string[0])),string[1],string[2],string[3]);
//	SetHeadXYTitle("Maximum Passenger Count","Passenger Type","Max Number of Passenger",strFootnote);
//	//3.)Draw it
//	CString strxValue[1000];//The Pax Type Name
//	int nPaxCount=0;
//	float yValue[1000];
//	char buf[128];
//	CString strTemp;
//	float value = 0.0;
//	for(int k=0;k<1000;k++)
//	{
//		yValue[k]=0;	
//	}
//	
//	int i=0;
//	chartFile.reset();
//	chartFile.skipLines(3);
//	
//	while (chartFile.getLine())
//	{
//		chartFile.setToField(0);
//		chartFile.getField(buf,128);
//		strTemp.Format("%s",buf);
//		if(strxValue[nPaxCount]!=strTemp)
//		{
//			nPaxCount++;
//			strxValue[nPaxCount]=strTemp;
//		}
//
//		chartFile.setToField (4);
//		chartFile.getFloat (value);
//		yValue[nPaxCount]=value;
//	}
//	
//	m_MSChartCtrl.SetRowCount(nPaxCount);
//	m_MSChartCtrl.SetColumnCount(1);
//	m_MSChartCtrl.SetColumn(1);
//	m_MSChartCtrl.SetColumnLabel("Passenger Type");
//	
//	CString strLabel;
//	for(i=1;i<=nPaxCount;i++)
//	{
//		m_MSChartCtrl.SetRow(i);
//		strLabel.Format("%s",strxValue[i]);
//		DealRowLabel(strLabel);
//		m_MSChartCtrl.SetRowLabel(strLabel);
//		
//		strLabel.Format("%f",yValue[i]);
//		m_MSChartCtrl.SetData(strLabel);
//	}
//	chartFile.closeIn();
//	m_MSChartCtrl.SetShowLegend(FALSE);
//}
//
//void CRepGraphView::PaxCountTotalPaxCount()
//{
//	//AfxMessageBox("PaxCountDetailed");
//	// load report from file
//	CString sFileName;
//	if( GetLoadReportType() == load_by_user )
//	{
//		sFileName = GetUserLoadReportFile();
//	}
//	else								// load_by_system
//	{
//		sFileName = ((CTermPlanDoc*)GetDocument())->GetTerminal().GetSimReportManager()->GetCurrentReportFileName( ((CTermPlanDoc*)GetDocument())->m_ProjInfo.path );
//	}
//	chartFile.init (sFileName, READ);
//	//2.)Draw Title,X Axis Title,Y Axis Title,Footnote
//	chartFile.reset();
//	chartFile.getLine();
//	chartFile.getLine();
//	char string[4][128];
//	chartFile.getField (string[0], 128);
//	chartFile.getField (string[1], 128);
//	chartFile.getField (string[2], 128);
//	chartFile.getField (string[3], 128);
//	CString strFootnote;
//	strFootnote.Format("%s %s %s %s",NoUnderline(CString(string[0])),string[1],string[2],string[3]);
//	SetHeadXYTitle("Total Passenger Count","Passenger Type","Total Number of Passengers",strFootnote);
//	//3.)Draw it
//	CString strxValue[1000];//The Pax Type Name
//	int nPaxCount=0;
//	float yValue[1000];
//	char buf[128];
//	CString strTemp;
//	float value = 0.0;
//	for(int k=0;k<1000;k++)
//	{
//		yValue[k]=0;	
//	}
//	
//	int i=0;
//	chartFile.reset();
//	chartFile.skipLines(3);
//	
//	while (chartFile.getLine())
//	{
//		chartFile.setToField(0);
//		chartFile.getField(buf,128);
//		strTemp.Format("%s",buf);
//		if(strxValue[nPaxCount]!=strTemp)
//		{
//			nPaxCount++;
//			strxValue[nPaxCount]=strTemp;
//		}
//
//		chartFile.setToField (6);
//		chartFile.getFloat (value);
//		yValue[nPaxCount]=value;
//	}
//	
//	m_MSChartCtrl.SetRowCount(nPaxCount);
//	m_MSChartCtrl.SetColumnCount(1);
//	m_MSChartCtrl.SetColumn(1);
//	m_MSChartCtrl.SetColumnLabel("Passenger Type");
//	
//	CString strLabel;
//	for(i=1;i<=nPaxCount;i++)
//	{
//		m_MSChartCtrl.SetRow(i);
//		strLabel.Format("%s",strxValue[i]);
//		DealRowLabel(strLabel);
//		m_MSChartCtrl.SetRowLabel(strLabel);
//		
//		strLabel.Format("%f",yValue[i]);
//		m_MSChartCtrl.SetData(strLabel);
//	}
//	chartFile.closeIn();
//	m_MSChartCtrl.SetShowLegend(FALSE);
//}
//
//void CRepGraphView::PaxCountTimeRange()
//{
//	//AfxMessageBox("PaxCountTimeRange");
//}

void CRepGraphView::OnDraw(CDC* pDC) 
{
	// TODO: Add your specialized code here and/or call the base class
	if (pDC->IsPrinting())
	{
		IViewObject* pViewObj = NULL;
		HRESULT hr =m_MSChartCtrl.GetControlUnknown()->QueryInterface(IID_IViewObject,(void**)&pViewObj);
		if (hr==S_OK)
		{
			RECT rect;
			m_MSChartCtrl.GetClientRect(&rect);
			// inflating with arbitrary value to account for difference between
			// screen and printer resolution. You'd probably want to set a scale
			
			// mode here...

			RECTL rclWBounds;
			CPoint ptOrg = pDC->GetWindowOrg();
			CSize sizeExt(pDC->GetDeviceCaps( HORZRES ),pDC->GetDeviceCaps( VERTRES )); //= pDC->GetWindowExt();
			rclWBounds.left = 100;//ptOrg.x;
			rclWBounds.top = 100;//ptOrg.y;
			rclWBounds.right = sizeExt.cx;//100;
			rclWBounds.bottom = sizeExt.cy;//100;

			
			// have control draw itself into the printer (or printpreview) DC.
			hr =pViewObj->Draw(DVASPECT_TRANSPARENT,-1,NULL,NULL,pDC->m_hAttribDC,pDC->m_hDC,&m_rclBounds,&rclWBounds,NULL,NULL);
			pViewObj->Release();
		}
	}
	
}

int CRepGraphView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CFormView::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here
	if (!m_ToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		/*| CBRS_GRIPPER*/ | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_ToolBar.LoadToolBar(IDR_REPORTTOOLBAR))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	return 0;
}

void CRepGraphView::InitToolbar()
{
	CRect rc;
	m_toolbarcontenter.GetWindowRect( &rc );
	ScreenToClient(&rc);
	m_ToolBar.MoveWindow( 2,-2,90,rc.Height() );
	m_toolbarcontenter.ShowWindow(SW_HIDE);
	m_ToolBar.ShowWindow( SW_SHOW );
//	m_toolbarcontenter.ShowWindow(SW_HIDE);
//	m_ToolBar.GetToolBarCtrl().EnableButton( ID_PEOPLEMOVER_NEW );
//	m_ToolBar.GetToolBarCtrl().EnableButton( ID_PEOPLEMOVER_CHANGE );
//	m_ToolBar.GetToolBarCtrl().EnableButton( ID_PEOPLEMOVER_DELETE,FALSE );
	
}


//void CRepGraphView::QueueTimeDetailed()
//{
//	// load report from file
//	CString sFileName;
//	if( GetLoadReportType() == load_by_user )
//	{
//		sFileName = GetUserLoadReportFile();
//	}
//	else								// load_by_system
//	{
//		sFileName = ((CTermPlanDoc*)GetDocument())->GetTerminal().GetSimReportManager()->GetCurrentReportFileName( ((CTermPlanDoc*)GetDocument())->m_ProjInfo.path );
//	}
//	chartFile.init (sFileName, READ);
//	//2.)Draw Title,X Axis Title,Y Axis Title,Footnote
//	chartFile.reset();
//	chartFile.getLine();
//	chartFile.getLine();
//	char string[4][128];
//	chartFile.getField (string[0], 128);
//	chartFile.getField (string[1], 128);
//	chartFile.getField (string[2], 128);
//	chartFile.getField (string[3], 128);
//	CString strFootnote;
//	strFootnote.Format("%s %s %s %s",NoUnderline(CString(string[0])),string[1],string[2],string[3]);
//	SetHeadXYTitle("Queue Time(Detailed)","Minutes in Queues","Passenger Count",strFootnote);
//	//3.)Draw it
//	int xValue[8];
//	int yValue[8];
//	ElapsedTime valuetime;
//	for(int i=0;i<8;i++)
//	{
//		xValue[i]=yValue[i]=0;
//	}
//	int maxXValue=0;
//	int minXValue=14400;
//	float ft = 0.0;
//	chartFile.reset();
//	chartFile.skipLines(3);   
//	while (chartFile.getLine())
//	{
//		chartFile.setToField (1);
//		chartFile.getTime (valuetime, TRUE);
//		if (valuetime.asMinutes() < minXValue)
//			minXValue = valuetime.asMinutes();
//		if (valuetime.asMinutes() > maxXValue)
//			maxXValue = valuetime.asMinutes();
//	}
//	minXValue=0;
//	maxXValue=(int)maxXValue;
//	int nScale;//
//	int n=(maxXValue-minXValue)%7;//7 Column
//	if(n==0)
//	{
//		nScale=(maxXValue-minXValue)/7;
//	}
//	else
//	{
//		nScale=(maxXValue-minXValue)/7+1;
//	}
//	
//	xValue[0]=minXValue;
//	for(i=1;i<=7;i++)
//	{
//		xValue[i]=xValue[i-1]+nScale;
//	}
//	
//	chartFile.reset();
//	chartFile.skipLines(3);
//	while (chartFile.getLine())
//	{
//		chartFile.setToField (1);
//		chartFile.getTime (valuetime, TRUE);
//		for(i=1;i<=7;i++)
//		{
//			if(valuetime.asMinutes()>=xValue[i-1] && valuetime.asMinutes()<xValue[i])
//			{
//				yValue[i]++;
//			}
//		}
//	}
//	int nCount=7;
//	for(i=7;i>0;i--)
//	{
//		if(yValue[i]==0)
//		{
//			nCount--;
//		}
//		else
//		{
//			break;
//		}
//	}
//	m_MSChartCtrl.SetRowCount(nCount);
//	m_MSChartCtrl.SetColumnCount(1);
//	m_MSChartCtrl.SetColumnLabelCount(1);
//	m_MSChartCtrl.SetColumn(1);
//	m_MSChartCtrl.SetColumnLabel("Meters Travelled");	
//	
//	CString strLabel;
//	for(i=1;i<=nCount;i++)
//	{
//		m_MSChartCtrl.SetRow(i);
//		strLabel.Format("%d-%d",xValue[i-1],xValue[i]);
//		DealRowLabel(strLabel);
//		m_MSChartCtrl.SetRowLabel(strLabel);
//		
//		strLabel.Format("%d",yValue[i]);
//		m_MSChartCtrl.SetData(strLabel);
//	}
//	chartFile.closeIn();	
//	m_MSChartCtrl.SetShowLegend(FALSE);
//}
//
//void CRepGraphView::QueueTimeSummary()
//{
//	// load report from file
//	CString sFileName;
//	if( GetLoadReportType() == load_by_user )
//	{
//		sFileName = GetUserLoadReportFile();
//	}
//	else								// load_by_system
//	{
//		sFileName = ((CTermPlanDoc*)GetDocument())->GetTerminal().GetSimReportManager()->GetCurrentReportFileName( ((CTermPlanDoc*)GetDocument())->m_ProjInfo.path );
//	}
//	chartFile.init (sFileName, READ);
//	//2.)Draw Title,X Axis Title,Y Axis Title,Footnote
//	chartFile.reset();
//	chartFile.getLine();
//	chartFile.getLine();
//	char string[4][128];
//	chartFile.getField (string[0], 128);
//	chartFile.getField (string[1], 128);
//	chartFile.getField (string[2], 128);
//	chartFile.getField (string[3], 128);
//	CString strFootnote;
//	strFootnote.Format("%s %s %s %s",NoUnderline(CString(string[0])),string[1],string[2],string[3]);
//	SetHeadXYTitle("Queue Time (Summary)","Passenger Type","Minutes in Queues",strFootnote);
//	//3.)Draw it
//	CString strxValue[100];//The Pax Type Name
//	int nPaxCount=0;
//	ElapsedTime valuetime;
//	float minvalue[100];
//	float avevalue[100];
//	float maxvalue[100];
//
//	float Q1value[100];
//	float Q2value[100];
//	float Q3value[100];
//
//	float P1value[100];
//	float P5value[100];
//	float P10value[100];
//
//	float P90value[100];
//	float P95value[100];
//	float P99value[100];
//
//	float Sigmavalue[100];
//
//	char buf[128];
//	for(int k=0;k<100;k++)
//	{
//		minvalue[k]=avevalue[k]=maxvalue[k]=Q1value[k]=0;
//		Q2value[k]=Q3value[k]=P1value[k]=P5value[k]=0;
//		Sigmavalue[k] = P10value[k]=P95value[k]=P99value[k]=0;
//	}
//	chartFile.reset();
//	chartFile.skipLines(3);
//	while (chartFile.getLine())
//	{
//		chartFile.setToField(0);
//		chartFile.getField(buf,128);
//		nPaxCount+=1;
//		strxValue[nPaxCount].Format("%s",buf);//Get The First Pax Type
//		
//		chartFile.setToField (1);
//		chartFile.getTime (valuetime, TRUE);
//		minvalue[nPaxCount]=static_cast<float>(valuetime.asMinutes());
//		chartFile.setToField (2);
//		chartFile.getTime (valuetime, TRUE);
//		avevalue[nPaxCount]=static_cast<float>(valuetime.asMinutes());
//		chartFile.setToField (3);
//		chartFile.getTime (valuetime, TRUE);
//		maxvalue[nPaxCount]=static_cast<float>(valuetime.asMinutes());
//
//		chartFile.setToField (5);
//		chartFile.getTime (valuetime, TRUE);
//		Q1value[nPaxCount] = static_cast<float>(valuetime.asMinutes());
//
//		chartFile.setToField (6);
//		chartFile.getTime (valuetime, TRUE);
//		Q2value[nPaxCount]= static_cast<float>(valuetime.asMinutes());
//
//		chartFile.setToField (7);
//		chartFile.getTime (valuetime, TRUE);
//		Q3value[nPaxCount]= static_cast<float>(valuetime.asMinutes());
//
//		chartFile.setToField (8);
//		chartFile.getTime (valuetime, TRUE);
//		P1value[nPaxCount]= static_cast<float>(valuetime.asMinutes());
//
//		chartFile.setToField (9);
//		chartFile.getTime (valuetime, TRUE);
//		P5value[nPaxCount]= static_cast<float>(valuetime.asMinutes());
//
//		chartFile.setToField (10);
//		chartFile.getTime (valuetime, TRUE);
//		P10value[nPaxCount]= static_cast<float>(valuetime.asMinutes());
//		
//		chartFile.setToField (11);
//		chartFile.getTime (valuetime, TRUE);
//		P90value[nPaxCount]= static_cast<float>(valuetime.asMinutes());
//
//		chartFile.setToField (12);
//		chartFile.getTime (valuetime, TRUE);
//		P95value[nPaxCount]= static_cast<float>(valuetime.asMinutes());
//
//		chartFile.setToField (13);
//		chartFile.getTime (valuetime, TRUE);
//		P99value[nPaxCount]= static_cast<float>(valuetime.asMinutes());
//
//		chartFile.setToField (14);
//		chartFile.getTime (valuetime, TRUE);
//		Sigmavalue[nPaxCount]= static_cast<float>(valuetime.asMinutes());
//	}
//	
//	CString strLabel;
//	m_MSChartCtrl.SetRowCount(nPaxCount);
//	m_MSChartCtrl.SetColumnCount(13);
//	m_MSChartCtrl.SetColumnLabelCount(13);
//	m_MSChartCtrl.SetColumn(1);
//	m_MSChartCtrl.SetColumnLabel("Minimum");
//	m_MSChartCtrl.SetColumn(2);
//	m_MSChartCtrl.SetColumnLabel("Average");
//	m_MSChartCtrl.SetColumn(3);
//	m_MSChartCtrl.SetColumnLabel("Maximum");
//
//	m_MSChartCtrl.SetColumn(4);
//	m_MSChartCtrl.SetColumnLabel("Q1");
//
//	m_MSChartCtrl.SetColumn(5);
//	m_MSChartCtrl.SetColumnLabel("Q2");
//
//	m_MSChartCtrl.SetColumn(6);
//	m_MSChartCtrl.SetColumnLabel("Q3");
//
//	m_MSChartCtrl.SetColumn(7);
//	m_MSChartCtrl.SetColumnLabel("P1");
//
//	m_MSChartCtrl.SetColumn(8);
//	m_MSChartCtrl.SetColumnLabel("P5");
//
//	m_MSChartCtrl.SetColumn(9);
//	m_MSChartCtrl.SetColumnLabel("P10");
//
//	m_MSChartCtrl.SetColumn(10);
//	m_MSChartCtrl.SetColumnLabel("P90");
//
//	m_MSChartCtrl.SetColumn(11);
//	m_MSChartCtrl.SetColumnLabel("P95");
//
//	m_MSChartCtrl.SetColumn(12);
//	m_MSChartCtrl.SetColumnLabel("P99");
//
//	m_MSChartCtrl.SetColumn(13);
//	m_MSChartCtrl.SetColumnLabel("Sigma");
//
//
//	for(int i=1;i<=nPaxCount;i++)
//	{
//		m_MSChartCtrl.SetRow(i);
//		DealRowLabel(strxValue[i]);
//		m_MSChartCtrl.SetRowLabel(strxValue[i]);		
//		m_MSChartCtrl.SetColumn(1);
//		strLabel.Format("%f",minvalue[i]);
//		m_MSChartCtrl.SetData(strLabel);
//		m_MSChartCtrl.SetRow(i);	//add by bird 2003/2/18
//		m_MSChartCtrl.SetColumn(2);
//		strLabel.Format("%f",avevalue[i]);
//		m_MSChartCtrl.SetData(strLabel);
//		m_MSChartCtrl.SetRow(i);	//add by bird 2003/2/18
//		m_MSChartCtrl.SetColumn(3);
//		strLabel.Format("%f",maxvalue[i]);
//		m_MSChartCtrl.SetData(strLabel);
//
//		m_MSChartCtrl.SetRow(i);	
//		m_MSChartCtrl.SetColumn(4);
//		strLabel.Format("%f",Q1value[i]);
//		m_MSChartCtrl.SetData(strLabel);
//
//		m_MSChartCtrl.SetRow(i);	
//		m_MSChartCtrl.SetColumn(5);
//		strLabel.Format("%f", Q2value[i] );
//		m_MSChartCtrl.SetData(strLabel);
//
//		m_MSChartCtrl.SetRow(i);	
//		m_MSChartCtrl.SetColumn(6);
//		strLabel.Format("%f",Q3value[i]);
//		m_MSChartCtrl.SetData(strLabel);
//
//		m_MSChartCtrl.SetRow(i);
//		m_MSChartCtrl.SetColumn(7);
//		strLabel.Format("%f",P1value[i]);
//		m_MSChartCtrl.SetData(strLabel);
//
//		m_MSChartCtrl.SetRow(i);
//		m_MSChartCtrl.SetColumn(8);
//		strLabel.Format("%f",P5value[i]);
//		m_MSChartCtrl.SetData(strLabel);
//
//		m_MSChartCtrl.SetRow(i);
//		m_MSChartCtrl.SetColumn(9);
//		strLabel.Format("%f",P10value[i]);
//		m_MSChartCtrl.SetData(strLabel);
//
//		m_MSChartCtrl.SetRow(i);
//		m_MSChartCtrl.SetColumn(10);
//		strLabel.Format("%f",P90value[i]);
//		m_MSChartCtrl.SetData(strLabel);
//
//		m_MSChartCtrl.SetRow(i);
//		m_MSChartCtrl.SetColumn(11);
//		strLabel.Format("%f",P95value[i]);
//		m_MSChartCtrl.SetData(strLabel);
//
//		m_MSChartCtrl.SetRow(i);
//		m_MSChartCtrl.SetColumn(12);
//		strLabel.Format("%f",P99value[i]);
//		m_MSChartCtrl.SetData(strLabel);
//
//		m_MSChartCtrl.SetRow(i);
//		m_MSChartCtrl.SetColumn(13);
//		strLabel.Format("%f",Sigmavalue[i]);
//		m_MSChartCtrl.SetData(strLabel);
//	}
//	
//	chartFile.closeIn();
//	m_MSChartCtrl.SetShowLegend(TRUE);
//}
//void CRepGraphView::ConveyorWatiTimeSummary()
//{
//// load report from file
//	CString sFileName;
//	if( GetLoadReportType() == load_by_user )
//	{
//		sFileName = GetUserLoadReportFile();
//	}
//	else								// load_by_system
//	{
//		sFileName = ((CTermPlanDoc*)GetDocument())->GetTerminal().GetSimReportManager()->GetCurrentReportFileName( ((CTermPlanDoc*)GetDocument())->m_ProjInfo.path );
//	}
//	chartFile.init (sFileName, READ);
//	//2.)Draw Title,X Axis Title,Y Axis Title,Footnote
//	chartFile.reset();
//	chartFile.getLine();
//	chartFile.getLine();
//	char string[4][128];
//	chartFile.getField (string[0], 128);
//	chartFile.getField (string[1], 128);
//	chartFile.getField (string[2], 128);
//	chartFile.getField (string[3], 128);
//	CString strFootnote;
//	strFootnote.Format("%s %s %s %s",NoUnderline(CString(string[0])),string[1],string[2],string[3]);
//	SetHeadXYTitle("Wait In Conveyor Time (Seconds)","Passenger Type","Seconds In Conveyor",strFootnote);
//	//3.)Draw it
//	CString strxValue[100];//The Pax Type Name
//	int nPaxCount=0;
//	ElapsedTime valuetime;
//	float minvalue[100];
//	float avevalue[100];
//	float maxvalue[100];
//
//	float Q1value[100];
//	float Q2value[100];
//	float Q3value[100];
//
//	float P1value[100];
//	float P5value[100];
//	float P10value[100];
//
//	float P90value[100];
//	float P95value[100];
//	float P99value[100];
//
//	float Sigmavalue[100];
//
//	char buf[128];
//	for(int k=0;k<100;k++)
//	{
//		minvalue[k]=avevalue[k]=maxvalue[k]=Q1value[k]=0;
//		Q2value[k]=Q3value[k]=P1value[k]=P5value[k]=0;
//		Sigmavalue[k] = P10value[k]=P95value[k]=P99value[k]=0;
//	}
//	chartFile.reset();
//	chartFile.skipLines(3);
//	while (chartFile.getLine())
//	{
//		chartFile.setToField(0);
//		chartFile.getField(buf,128);
//		nPaxCount+=1;
//		strxValue[nPaxCount].Format("%s",buf);//Get The First Pax Type
//		
//		chartFile.setToField (1);
//		chartFile.getTime (valuetime, TRUE);
//		minvalue[nPaxCount]=static_cast<float>(valuetime.asSeconds());
//		chartFile.setToField (2);
//		chartFile.getTime (valuetime, TRUE);
//		avevalue[nPaxCount]=static_cast<float>(valuetime.asSeconds());
//		chartFile.setToField (3);
//		chartFile.getTime (valuetime, TRUE);
//		maxvalue[nPaxCount]=static_cast<float>(valuetime.asSeconds());
//
//		chartFile.setToField (5);
//		chartFile.getTime (valuetime, TRUE);
//		Q1value[nPaxCount] = static_cast<float>(valuetime.asSeconds());
//
//		chartFile.setToField (6);
//		chartFile.getTime (valuetime, TRUE);
//		Q2value[nPaxCount]= static_cast<float>(valuetime.asSeconds());
//
//		chartFile.setToField (7);
//		chartFile.getTime (valuetime, TRUE);
//		Q3value[nPaxCount]= static_cast<float>(valuetime.asMinutes());
//
//		chartFile.setToField (8);
//		chartFile.getTime (valuetime, TRUE);
//		P1value[nPaxCount]= static_cast<float>(valuetime.asSeconds());
//
//		chartFile.setToField (9);
//		chartFile.getTime (valuetime, TRUE);
//		P5value[nPaxCount]= static_cast<float>(valuetime.asSeconds());
//
//		chartFile.setToField (10);
//		chartFile.getTime (valuetime, TRUE);
//		P10value[nPaxCount]= static_cast<float>(valuetime.asSeconds());
//		
//		chartFile.setToField (11);
//		chartFile.getTime (valuetime, TRUE);
//		P90value[nPaxCount]= static_cast<float>(valuetime.asSeconds());
//
//		chartFile.setToField (12);
//		chartFile.getTime (valuetime, TRUE);
//		P95value[nPaxCount]= static_cast<float>(valuetime.asSeconds());
//
//		chartFile.setToField (13);
//		chartFile.getTime (valuetime, TRUE);
//		P99value[nPaxCount]= static_cast<float>(valuetime.asSeconds());
//
//		chartFile.setToField (14);
//		chartFile.getTime (valuetime, TRUE);
//		Sigmavalue[nPaxCount]= static_cast<float>(valuetime.asSeconds());
//	}
//	
//	CString strLabel;
//	m_MSChartCtrl.SetRowCount(nPaxCount);
//	m_MSChartCtrl.SetColumnCount(13);
//	m_MSChartCtrl.SetColumnLabelCount(13);
//	m_MSChartCtrl.SetColumn(1);
//	m_MSChartCtrl.SetColumnLabel("Minimum");
//	m_MSChartCtrl.SetColumn(2);
//	m_MSChartCtrl.SetColumnLabel("Average");
//	m_MSChartCtrl.SetColumn(3);
//	m_MSChartCtrl.SetColumnLabel("Maximum");
//
//	m_MSChartCtrl.SetColumn(4);
//	m_MSChartCtrl.SetColumnLabel("Q1");
//
//	m_MSChartCtrl.SetColumn(5);
//	m_MSChartCtrl.SetColumnLabel("Q2");
//
//	m_MSChartCtrl.SetColumn(6);
//	m_MSChartCtrl.SetColumnLabel("Q3");
//
//	m_MSChartCtrl.SetColumn(7);
//	m_MSChartCtrl.SetColumnLabel("P1");
//
//	m_MSChartCtrl.SetColumn(8);
//	m_MSChartCtrl.SetColumnLabel("P5");
//
//	m_MSChartCtrl.SetColumn(9);
//	m_MSChartCtrl.SetColumnLabel("P10");
//
//	m_MSChartCtrl.SetColumn(10);
//	m_MSChartCtrl.SetColumnLabel("P90");
//
//	m_MSChartCtrl.SetColumn(11);
//	m_MSChartCtrl.SetColumnLabel("P95");
//
//	m_MSChartCtrl.SetColumn(12);
//	m_MSChartCtrl.SetColumnLabel("P99");
//
//	m_MSChartCtrl.SetColumn(13);
//	m_MSChartCtrl.SetColumnLabel("Sigma");
//
//
//	for(int i=1;i<=nPaxCount;i++)
//	{
//		m_MSChartCtrl.SetRow(i);
//		DealRowLabel(strxValue[i]);
//		m_MSChartCtrl.SetRowLabel(strxValue[i]);		
//		m_MSChartCtrl.SetColumn(1);
//		strLabel.Format("%f",minvalue[i]);
//		m_MSChartCtrl.SetData(strLabel);
//		m_MSChartCtrl.SetRow(i);	//add by bird 2003/2/18
//		m_MSChartCtrl.SetColumn(2);
//		strLabel.Format("%f",avevalue[i]);
//		m_MSChartCtrl.SetData(strLabel);
//		m_MSChartCtrl.SetRow(i);	//add by bird 2003/2/18
//		m_MSChartCtrl.SetColumn(3);
//		strLabel.Format("%f",maxvalue[i]);
//		m_MSChartCtrl.SetData(strLabel);
//
//		m_MSChartCtrl.SetRow(i);	
//		m_MSChartCtrl.SetColumn(4);
//		strLabel.Format("%f",Q1value[i]);
//		m_MSChartCtrl.SetData(strLabel);
//
//		m_MSChartCtrl.SetRow(i);	
//		m_MSChartCtrl.SetColumn(5);
//		strLabel.Format("%f", Q2value[i] );
//		m_MSChartCtrl.SetData(strLabel);
//
//		m_MSChartCtrl.SetRow(i);	
//		m_MSChartCtrl.SetColumn(6);
//		strLabel.Format("%f",Q3value[i]);
//		m_MSChartCtrl.SetData(strLabel);
//
//		m_MSChartCtrl.SetRow(i);
//		m_MSChartCtrl.SetColumn(7);
//		strLabel.Format("%f",P1value[i]);
//		m_MSChartCtrl.SetData(strLabel);
//
//		m_MSChartCtrl.SetRow(i);
//		m_MSChartCtrl.SetColumn(8);
//		strLabel.Format("%f",P5value[i]);
//		m_MSChartCtrl.SetData(strLabel);
//
//		m_MSChartCtrl.SetRow(i);
//		m_MSChartCtrl.SetColumn(9);
//		strLabel.Format("%f",P10value[i]);
//		m_MSChartCtrl.SetData(strLabel);
//
//		m_MSChartCtrl.SetRow(i);
//		m_MSChartCtrl.SetColumn(10);
//		strLabel.Format("%f",P90value[i]);
//		m_MSChartCtrl.SetData(strLabel);
//
//		m_MSChartCtrl.SetRow(i);
//		m_MSChartCtrl.SetColumn(11);
//		strLabel.Format("%f",P95value[i]);
//		m_MSChartCtrl.SetData(strLabel);
//
//		m_MSChartCtrl.SetRow(i);
//		m_MSChartCtrl.SetColumn(12);
//		strLabel.Format("%f",P99value[i]);
//		m_MSChartCtrl.SetData(strLabel);
//
//		m_MSChartCtrl.SetRow(i);
//		m_MSChartCtrl.SetColumn(13);
//		strLabel.Format("%f",Sigmavalue[i]);
//		m_MSChartCtrl.SetData(strLabel);
//	}
//	
//	chartFile.closeIn();
//	m_MSChartCtrl.SetShowLegend(TRUE);
//}
//void CRepGraphView::ConveyorWatiTimeDetailed()
//{
//	// load report from file
//	CString sFileName;
//	if( GetLoadReportType() == load_by_user )
//	{
//		sFileName = GetUserLoadReportFile();
//	}
//	else								// load_by_system
//	{
//		sFileName = ((CTermPlanDoc*)GetDocument())->GetTerminal().GetSimReportManager()->GetCurrentReportFileName( ((CTermPlanDoc*)GetDocument())->m_ProjInfo.path );
//	}
//	chartFile.init (sFileName, READ);
//	//2.)Draw Title,X Axis Title,Y Axis Title,Footnote
//	chartFile.reset();
//	chartFile.getLine();
//	chartFile.getLine();
//	char string[4][128];
//	chartFile.getField (string[0], 128);
//	chartFile.getField (string[1], 128);
//	chartFile.getField (string[2], 128);
//	chartFile.getField (string[3], 128);
//	CString strFootnote;
//	strFootnote.Format("%s %s %s %s",NoUnderline(CString(string[0])),string[1],string[2],string[3]);
//	SetHeadXYTitle("Wait In Conveyor Time(Detailed)","Minutes in  Conveyor ","Passenger Count",strFootnote);
//	//3.)Draw it
//	int xValue[8];
//	int yValue[8];
//	ElapsedTime valuetime;
//	for(int i=0;i<8;i++)
//	{
//		xValue[i]=yValue[i]=0;
//	}
//	int maxXValue=0;
//	int minXValue=14400;
//	float ft = 0.0;
//	chartFile.reset();
//	chartFile.skipLines(3);   
//	while (chartFile.getLine())
//	{
//		chartFile.setToField (1);
//		chartFile.getTime (valuetime, TRUE);
//		if (valuetime.asMinutes() < minXValue)
//			minXValue = valuetime.asMinutes();
//		if (valuetime.asMinutes() > maxXValue)
//			maxXValue = valuetime.asMinutes();
//	}
//	minXValue=0;
//	maxXValue=(int)maxXValue;
//	int nScale;//
//	int n=(maxXValue-minXValue)%7;//7 Column
//	if(n==0)
//	{
//		nScale=(maxXValue-minXValue)/7;
//	}
//	else
//	{
//		nScale=(maxXValue-minXValue)/7+1;
//	}
//	
//	xValue[0]=minXValue;
//	for(i=1;i<=7;i++)
//	{
//		xValue[i]=xValue[i-1]+nScale;
//	}
//	
//	chartFile.reset();
//	chartFile.skipLines(3);
//	while (chartFile.getLine())
//	{
//		chartFile.setToField (1);
//		chartFile.getTime (valuetime, TRUE);
//		for(i=1;i<=7;i++)
//		{
//			if(valuetime.asMinutes()>=xValue[i-1] && valuetime.asMinutes()<xValue[i])
//			{
//				yValue[i]++;
//			}
//		}
//	}
//	int nCount=7;
//	for(i=7;i>0;i--)
//	{
//		if(yValue[i]==0)
//		{
//			nCount--;
//		}
//		else
//		{
//			break;
//		}
//	}
//	m_MSChartCtrl.SetRowCount(nCount);
//	m_MSChartCtrl.SetColumnCount(1);
//	m_MSChartCtrl.SetColumnLabelCount(1);
//	m_MSChartCtrl.SetColumn(1);
//	m_MSChartCtrl.SetColumnLabel("Minutes in  Conveyor");	
//	
//	CString strLabel;
//	for(i=1;i<=nCount;i++)
//	{
//		m_MSChartCtrl.SetRow(i);
//		strLabel.Format("%d-%d",xValue[i-1],xValue[i]);
//		DealRowLabel(strLabel);
//		m_MSChartCtrl.SetRowLabel(strLabel);
//		
//		strLabel.Format("%d",yValue[i]);
//		m_MSChartCtrl.SetData(strLabel);
//	}
//	chartFile.closeIn();	
//	m_MSChartCtrl.SetShowLegend(FALSE);
//}
//
//void CRepGraphView::ThroughputTotalPaxDetailed()
//{
//	// load report from file
//	CString sFileName;
//	if( GetLoadReportType() == load_by_user )
//	{
//		sFileName = GetUserLoadReportFile();
//	}
//	else								// load_by_system
//	{
//		sFileName = ((CTermPlanDoc*)GetDocument())->GetTerminal().GetSimReportManager()->GetCurrentReportFileName( ((CTermPlanDoc*)GetDocument())->m_ProjInfo.path );
//	}
//	chartFile.init (sFileName, READ);
//
//
//	//2.)Draw Title,X Axis Title,Y Axis Title,Footnote
//	chartFile.reset();
//	chartFile.seekToLine(0);
//	chartFile.getLine();
//	chartFile.getLine();
//	char string[4][128];
//	chartFile.getField (string[0], 128);
//	chartFile.getField (string[1], 128);
//	chartFile.getField (string[2], 128);
//	chartFile.getField (string[3], 128);
//	CString strFootnote;
//	strFootnote.Format("%s %s %s %s",NoUnderline(CString(string[0])),string[1],string[2],string[3]);
//	SetHeadXYTitle("Processor Throughput (Detailed)","Time of Day","Units Served",strFootnote);
//
//	ElapsedTime eBegin;
//	ElapsedTime eEnd;
//	getTime( string[1], eBegin, eEnd );
//	int interval = GetInterval(string[1]);	//interval, unit: minute
//	int nColNum = ( eEnd.asMinutes() - eBegin.asMinutes())/interval + 1; 
//
//	//3.)Draw it
//	int nProcCount=0;
//	CString strxValue[1000];//The Proc Name
//	char buf[128];
//	CString strTemp = "";
//	int hours = 0, minutes = 0;
//	float value = 0.0;
//
////	ElapsedTime valuetime;
//	float **yValue;
//	yValue = new float*[1000];
//	for(int k = 0; k < 1000; k++)
//	{
//		yValue[k] = new float[nColNum];
//	}
//	for(k=0;k<1000;k++)
//	{
//		for(int i=0;i<nColNum;i++)
//		{
//			yValue[k][i]=0;
//		}
//	}
//	
//
//	int i=0;
//	chartFile.reset();
//	chartFile.skipLines(3);
//
//	if(!chartFile.getLine())
//	{
//		chartFile.closeIn();
//		return;
//	}
//	chartFile.setToField(0);
//	chartFile.getField(buf,128);
//	nProcCount++;
//	strxValue[nProcCount].Format("%s",buf);//Get The First Pax Type
//
//	int IntervalNum = 1;
//	chartFile.reset();
//	chartFile.skipLines(3);
//	while (chartFile.getLine())
//	{
//		chartFile.setToField(0);
//		chartFile.getField(buf,128);
//		strTemp.Format("%s",buf);
//		if(strxValue[nProcCount]!=strTemp)
//		{
//			nProcCount++;
//			strxValue[nProcCount]=strTemp;
//			IntervalNum = 1;
//		}
//
//		chartFile.setToField (3);
//		chartFile.getFloat (value);
//
////		chartFile.setToField(1);
////		chartFile.getTime (valuetime, TRUE);
////		hours=(int)valuetime.asHours();
////		minutes=(int)valuetime.asMinutes();
//		
////		// TRACE("the time is %d hours,or %d minutes\n",hours,minutes);
//		yValue[nProcCount][IntervalNum++]=value;
//
//	}
//	
//	// add by bird 2003/10/13
//	ElapsedTime BeginTime,EndTime;
//	getTime( string[1], BeginTime, EndTime );
//	int nRowNum = (EndTime.asMinutes()-BeginTime.asMinutes())/interval; 
//
//	m_MSChartCtrl.SetRowCount(nRowNum);
//	m_MSChartCtrl.SetColumnCount(nProcCount);
//	m_MSChartCtrl.SetColumnLabelCount(nProcCount);
//
//	CString strLabel;
//
//	ElapsedTime detaTime( interval * 60l );
//	ElapsedTime time( BeginTime );
//	for(i = 1; i <= nRowNum; i++)
//	{
//		time.set(time.asSeconds() % WholeDay);
//		CString strBeginTime = time.printTime(0);
//		time += detaTime;
//		time.set(time.asSeconds() % WholeDay);
//		CString strEndTime	 = time.printTime(0);
//
//		strLabel.Format( "%s~%s", strBeginTime, strEndTime );
//		m_MSChartCtrl.SetRow(i);
//		DealRowLabel(strLabel);
//		m_MSChartCtrl.SetRowLabel(strLabel);
//		
//	}
//
//	for(k = 1 ; k <= nProcCount; k++)
//	{
//		m_MSChartCtrl.SetColumn(k);
//		m_MSChartCtrl.SetColumnLabel(strxValue[k]);
//		for(i = 1; i <= nRowNum; i++)
//		{
//			m_MSChartCtrl.SetRow(i);
//			strLabel.Format("%f",yValue[k][i]);
//			m_MSChartCtrl.SetData(strLabel);
//		}
//	}
//
//	for(k = 0; k < 1000; k++)
//	{
//		delete[] yValue[k];
//	}
//	delete[] yValue;
//	chartFile.closeIn();
//	m_MSChartCtrl.SetShowLegend(TRUE);
//
//}
//
//void CRepGraphView::ThroughputAvePaxDetailed()
//{
//	//AfxMessageBox("PaxCountDetailed");
//	// load report from file
//	CString sFileName;
//	if( GetLoadReportType() == load_by_user )
//	{
//		sFileName = GetUserLoadReportFile();
//	}
//	else								// load_by_system
//	{
//		sFileName = ((CTermPlanDoc*)GetDocument())->GetTerminal().GetSimReportManager()->GetCurrentReportFileName( ((CTermPlanDoc*)GetDocument())->m_ProjInfo.path );
//	}
//	chartFile.init (sFileName, READ);
//	//2.)Draw Title,X Axis Title,Y Axis Title,Footnote
//	chartFile.reset();
//	chartFile.getLine();
//	chartFile.getLine();
//	char string[4][128];
//	chartFile.getField (string[0], 128);
//	chartFile.getField (string[1], 128);
//	chartFile.getField (string[2], 128);
//	chartFile.getField (string[3], 128);
//	CString strFootnote;
//	strFootnote.Format("%s %s %s %s",NoUnderline(CString(string[0])),string[1],string[2],string[3]);
//	SetHeadXYTitle("Averange Pax per Hour(Detailed)","Processor","Passengers Served per Hour",strFootnote);
//	//3.)Draw it
//	CString strxValue[1000];//The Proc Type Name
//	int nPaxCount=0;
//	float yValue[1000];
//	char buf[128];
//	CString strTemp;
//	float value = 0.0;
//	for(int k=0;k<1000;k++)
//	{
//		yValue[k]=0;	
//	}
//	
//	int i=0;
//	chartFile.reset();
//	chartFile.skipLines(3);
//	
//	while (chartFile.getLine())
//	{
//		chartFile.setToField(0);
//		chartFile.getField(buf,128);
//		strTemp.Format("%s",buf);
//		if(strxValue[nPaxCount]!=strTemp)
//		{
//			nPaxCount++;
//			strxValue[nPaxCount]=strTemp;
//		}
//
//		chartFile.setToField (2);
//		chartFile.getFloat(value);
//		yValue[nPaxCount]=value;
//	}
//	
//	m_MSChartCtrl.SetRowCount(nPaxCount);
//	m_MSChartCtrl.SetColumnLabelCount(1);
//	m_MSChartCtrl.SetColumnCount(1);
//	m_MSChartCtrl.SetColumn(1);
//	m_MSChartCtrl.SetColumnLabel("Processor Type");
//	
//	CString strLabel;
//	for(i=1;i<=nPaxCount;i++)
//	{
//		m_MSChartCtrl.SetRow(i);
//		strLabel.Format("%s",strxValue[i]);
//		DealRowLabel(strLabel);
//		m_MSChartCtrl.SetRowLabel(strLabel);
//		
//		strLabel.Format("%f",yValue[i]);
//		m_MSChartCtrl.SetData(strLabel);
//	}
//	chartFile.closeIn();
//	m_MSChartCtrl.SetShowLegend(FALSE);
//}
//
//void CRepGraphView::ThroughputAveTimeDetailed()
//{
//	//AfxMessageBox("PaxCountDetailed");
//	// load report from file
//	CString sFileName;
//	if( GetLoadReportType() == load_by_user )
//	{
//		sFileName = GetUserLoadReportFile();
//	}
//	else								// load_by_system
//	{
//		sFileName = ((CTermPlanDoc*)GetDocument())->GetTerminal().GetSimReportManager()->GetCurrentReportFileName( ((CTermPlanDoc*)GetDocument())->m_ProjInfo.path );
//	}
//	chartFile.init (sFileName, READ);
//	//2.)Draw Title,X Axis Title,Y Axis Title,Footnote
//	chartFile.reset();
//	chartFile.getLine();
//	chartFile.getLine();
//	char string[4][128];
//	chartFile.getField (string[0], 128);
//	chartFile.getField (string[1], 128);
//	chartFile.getField (string[2], 128);
//	chartFile.getField (string[3], 128);
//	CString strFootnote;
//	strFootnote.Format("%s %s %s %s",NoUnderline(CString(string[0])),string[1],string[2],string[3]);
//	SetHeadXYTitle("Averange Time per Pax(Detailed)","Processor","Average Minutes per Passenger",strFootnote);
//	//3.)Draw it
//	CString strxValue[1000];//The Proc Type Name
//	int nPaxCount=0;
//	float yValue[1000];
//	char buf[128];
//	CString strTemp;
//	float value = 0.0;
//	ElapsedTime valuetime;
//	for(int k=0;k<1000;k++)
//	{
//		yValue[k]=0;	
//	}
//	
//	int i=0;
//	chartFile.reset();
//	chartFile.skipLines(3);
//	
//	while (chartFile.getLine())
//	{
//		chartFile.setToField(0);
//		chartFile.getField(buf,128);
//		strTemp.Format("%s",buf);
//		if(strxValue[nPaxCount]!=strTemp)
//		{
//			nPaxCount++;
//			strxValue[nPaxCount]=strTemp;
//		}
//
//		chartFile.setToField (3);
//		chartFile.getTime(valuetime);
//		yValue[nPaxCount]=static_cast<float>(valuetime.asMinutes());
//	}
//	
//	m_MSChartCtrl.SetRowCount(nPaxCount);
//	m_MSChartCtrl.SetColumnCount(1);
//	m_MSChartCtrl.SetColumn(1);
//	m_MSChartCtrl.SetColumnLabelCount(1);
//	m_MSChartCtrl.SetColumnLabel("Processor Type");
//	
//	CString strLabel;
//	for(i=1;i<=nPaxCount;i++)
//	{
//		m_MSChartCtrl.SetRow(i);
//		strLabel.Format("%s",strxValue[i]);
//		DealRowLabel(strLabel);
//		m_MSChartCtrl.SetRowLabel(strLabel);
//		
//		strLabel.Format("%f",yValue[i]);
//		m_MSChartCtrl.SetData(strLabel);
//	}
//	chartFile.closeIn();
//	m_MSChartCtrl.SetShowLegend(FALSE);
//}
//
//void CRepGraphView::ThroughputTotalPaxSummary()
//{
//	//AfxMessageBox("PaxCountDetailed");
//	// load report from file
//	CString sFileName;
//	if( GetLoadReportType() == load_by_user )
//	{
//		sFileName = GetUserLoadReportFile();
//	}
//	else								// load_by_system
//	{
//		sFileName = ((CTermPlanDoc*)GetDocument())->GetTerminal().GetSimReportManager()->GetCurrentReportFileName( ((CTermPlanDoc*)GetDocument())->m_ProjInfo.path );
//	}
//	chartFile.init (sFileName, READ);
//	//2.)Draw Title,X Axis Title,Y Axis Title,Footnote
//	chartFile.reset();
//	chartFile.getLine();
//	chartFile.getLine();
//	char string[4][128];
//	chartFile.getField (string[0], 128);
//	chartFile.getField (string[1], 128);
//	chartFile.getField (string[2], 128);
//	chartFile.getField (string[3], 128);
//	CString strFootnote;
//	strFootnote.Format("%s %s %s %s",NoUnderline(CString(string[0])),string[1],string[2],string[3]);
//	SetHeadXYTitle("Total Pax by Group","Processor","Total Passengers Served",strFootnote);
//	//3.)Draw it
//	CString strxValue[1000];//The Proc Type Name
//	int nPaxCount=0;
//	float yValue[1000];
//	char buf[128];
//	CString strTemp;
//	float value = 0.0;
//	for(int k=0;k<1000;k++)
//	{
//		yValue[k]=0;	
//	}
//	
//	int i=0;
//	chartFile.reset();
//	chartFile.skipLines(3);
//	
//	while (chartFile.getLine())
//	{
//		chartFile.setToField(0);
//		chartFile.getField(buf,128);
//		strTemp.Format("%s",buf);
//		if(strxValue[nPaxCount]!=strTemp)
//		{
//			nPaxCount++;
//			strxValue[nPaxCount]=strTemp;
//		}
//
//		chartFile.setToField (2);
//		chartFile.getFloat (value);
//		yValue[nPaxCount]=value;
//	}
//	
//	m_MSChartCtrl.SetRowCount(nPaxCount);
//	m_MSChartCtrl.SetColumnCount(1);
//	m_MSChartCtrl.SetColumn(1);
//	m_MSChartCtrl.SetColumnLabelCount(1);
//	m_MSChartCtrl.SetColumnLabel("Processor Type");
//	
//	CString strLabel;
//	for(i=1;i<=nPaxCount;i++)
//	{
//		m_MSChartCtrl.SetRow(i);
//		strLabel.Format("%s",strxValue[i]);
//		DealRowLabel(strLabel);
//		m_MSChartCtrl.SetRowLabel(strLabel);
//		
//		strLabel.Format("%f",yValue[i]);
//		m_MSChartCtrl.SetData(strLabel);
//	}
//	chartFile.closeIn();
//	m_MSChartCtrl.SetShowLegend(FALSE);
//}
//
//void CRepGraphView::ThroughputPaxPerProcSummary()
//{
//	//AfxMessageBox("PaxCountDetailed");
//	// load report from file
//	CString sFileName;
//	if( GetLoadReportType() == load_by_user )
//	{
//		sFileName = GetUserLoadReportFile();
//	}
//	else								// load_by_system
//	{
//		sFileName = ((CTermPlanDoc*)GetDocument())->GetTerminal().GetSimReportManager()->GetCurrentReportFileName( ((CTermPlanDoc*)GetDocument())->m_ProjInfo.path );
//	}
//	chartFile.init (sFileName, READ);
//	//2.)Draw Title,X Axis Title,Y Axis Title,Footnote
//	chartFile.reset();
//	chartFile.getLine();
//	chartFile.getLine();
//	char string[4][128];
//	chartFile.getField (string[0], 128);
//	chartFile.getField (string[1], 128);
//	chartFile.getField (string[2], 128);
//	chartFile.getField (string[3], 128);
//	CString strFootnote;
//	strFootnote.Format("%s %s %s %s",NoUnderline(CString(string[0])),string[1],string[2],string[3]);
//	SetHeadXYTitle("Pax per Processor","Processor","Passengers Served by Processor",strFootnote);
//	//3.)Draw it
//	CString strxValue[1000];//The Proc Type Name
//	int nPaxCount=0;
//	float yValue[1000];
//	char buf[128];
//	CString strTemp;
//	float value = 0.0;
//	for(int k=0;k<1000;k++)
//	{
//		yValue[k]=0;	
//	}
//	
//	int i=0;
//	chartFile.reset();
//	chartFile.skipLines(3);
//	
//	while (chartFile.getLine())
//	{
//		chartFile.setToField(0);
//		chartFile.getField(buf,128);
//		strTemp.Format("%s",buf);
//		if(strxValue[nPaxCount]!=strTemp)
//		{
//			nPaxCount++;
//			strxValue[nPaxCount]=strTemp;
//		}
//
//		chartFile.setToField (3);
//		chartFile.getFloat (value);
//		yValue[nPaxCount]=value;
//	}
//	
//	m_MSChartCtrl.SetRowCount(nPaxCount);
//	m_MSChartCtrl.SetColumnCount(1);
//	m_MSChartCtrl.SetColumn(1);
//	m_MSChartCtrl.SetColumnLabelCount(1);
//	m_MSChartCtrl.SetColumnLabel("Processor Type");
//	
//	CString strLabel;
//	for(i=1;i<=nPaxCount;i++)
//	{
//		m_MSChartCtrl.SetRow(i);
//		strLabel.Format("%s",strxValue[i]);
//		DealRowLabel(strLabel);
//		m_MSChartCtrl.SetRowLabel(strLabel);
//		
//		strLabel.Format("%f",yValue[i]);
//		m_MSChartCtrl.SetData(strLabel);
//	}
//	chartFile.closeIn();
//	m_MSChartCtrl.SetShowLegend(FALSE);
//}
//
//void CRepGraphView::ThroughputGroupPerHourSummary()
//{
//	//AfxMessageBox("PaxCountDetailed");
//	// load report from file
//	CString sFileName;
//	if( GetLoadReportType() == load_by_user )
//	{
//		sFileName = GetUserLoadReportFile();
//	}
//	else								// load_by_system
//	{
//		sFileName = ((CTermPlanDoc*)GetDocument())->GetTerminal().GetSimReportManager()->GetCurrentReportFileName( ((CTermPlanDoc*)GetDocument())->m_ProjInfo.path );
//	}
//	chartFile.init (sFileName, READ);
//	//2.)Draw Title,X Axis Title,Y Axis Title,Footnote
//	chartFile.reset();
//	chartFile.getLine();
//	chartFile.getLine();
//	char string[4][128];
//	chartFile.getField (string[0], 128);
//	chartFile.getField (string[1], 128);
//	chartFile.getField (string[2], 128);
//	chartFile.getField (string[3], 128);
//	CString strFootnote;
//	strFootnote.Format("%s %s %s %s",NoUnderline(CString(string[0])),string[1],string[2],string[3]);
//	SetHeadXYTitle("Group Throughput per Hour","Processor","Passengers Served per Hour",strFootnote);
//	//3.)Draw it
//	CString strxValue[1000];//The Proc Type Name
//	int nPaxCount=0;
//	float yValue[1000];
//	char buf[128];
//	CString strTemp;
//	float value = 0.0;
//	for(int k=0;k<1000;k++)
//	{
//		yValue[k]=0;	
//	}
//	
//	int i=0;
//	chartFile.reset();
//	chartFile.skipLines(3);
//	
//	while (chartFile.getLine())
//	{
//		chartFile.setToField(0);
//		chartFile.getField(buf,128);
//		strTemp.Format("%s",buf);
//		if(strxValue[nPaxCount]!=strTemp)
//		{
//			nPaxCount++;
//			strxValue[nPaxCount]=strTemp;
//		}
//
//		chartFile.setToField (4);
//		chartFile.getFloat (value);
//		yValue[nPaxCount]=value;
//	}
//	
//	m_MSChartCtrl.SetRowCount(nPaxCount);
//	m_MSChartCtrl.SetColumnCount(1);
//	m_MSChartCtrl.SetColumn(1);
//	m_MSChartCtrl.SetColumnLabelCount(1);
//	m_MSChartCtrl.SetColumnLabel("Processor Type");
//	
//	CString strLabel;
//	for(i=1;i<=nPaxCount;i++)
//	{
//		m_MSChartCtrl.SetRow(i);
//		strLabel.Format("%s",strxValue[i]);
//		DealRowLabel(strLabel);
//		m_MSChartCtrl.SetRowLabel(strLabel);
//		
//		strLabel.Format("%f",yValue[i]);
//		m_MSChartCtrl.SetData(strLabel);
//	}
//	chartFile.closeIn();
//	m_MSChartCtrl.SetShowLegend(FALSE);
//}
//
//void CRepGraphView::ThroughputProcPerHourSummary()
//{
//	//AfxMessageBox("PaxCountDetailed");
//	// load report from file
//	CString sFileName;
//	if( GetLoadReportType() == load_by_user )
//	{
//		sFileName = GetUserLoadReportFile();
//	}
//	else								// load_by_system
//	{
//		sFileName = ((CTermPlanDoc*)GetDocument())->GetTerminal().GetSimReportManager()->GetCurrentReportFileName( ((CTermPlanDoc*)GetDocument())->m_ProjInfo.path );
//	}
//	chartFile.init (sFileName, READ);
//	//2.)Draw Title,X Axis Title,Y Axis Title,Footnote
//	chartFile.reset();
//	chartFile.getLine();
//	chartFile.getLine();
//	char string[4][128];
//	chartFile.getField (string[0], 128);
//	chartFile.getField (string[1], 128);
//	chartFile.getField (string[2], 128);
//	chartFile.getField (string[3], 128);
//	CString strFootnote;
//	strFootnote.Format("%s %s %s %s",NoUnderline(CString(string[0])),string[1],string[2],string[3]);
//	SetHeadXYTitle("Processor Throughput per Hour","Processor","Passengers Served per Processor per Hour",strFootnote);
//	//3.)Draw it
//	CString strxValue[1000];//The Proc Type Name
//	int nPaxCount=0;
//	float yValue[1000];
//	char buf[128];
//	CString strTemp;
//	float value = 0.0;
//	for(int k=0;k<1000;k++)
//	{
//		yValue[k]=0;	
//	}
//	
//	int i=0;
//	chartFile.reset();
//	chartFile.skipLines(3);
//	
//	while (chartFile.getLine())
//	{
//		chartFile.setToField(0);
//		chartFile.getField(buf,128);
//		strTemp.Format("%s",buf);
//		if(strxValue[nPaxCount]!=strTemp)
//		{
//			nPaxCount++;
//			strxValue[nPaxCount]=strTemp;
//		}
//
//		chartFile.setToField (5);
//		chartFile.getFloat (value);
//		yValue[nPaxCount]=value;
//	}
//	
//	m_MSChartCtrl.SetRowCount(nPaxCount);
//	m_MSChartCtrl.SetColumnCount(1);
//	m_MSChartCtrl.SetColumn(1);
//	m_MSChartCtrl.SetColumnLabelCount(1);
//	m_MSChartCtrl.SetColumnLabel("Processor Type");
//	
//	CString strLabel;
//	for(i=1;i<=nPaxCount;i++)
//	{
//		m_MSChartCtrl.SetRow(i);
//		strLabel.Format("%s",strxValue[i]);
//		DealRowLabel(strLabel);
//		m_MSChartCtrl.SetRowLabel(strLabel);
//		
//		strLabel.Format("%f",yValue[i]);
//		m_MSChartCtrl.SetData(strLabel);
//	}
//	chartFile.closeIn();
//	m_MSChartCtrl.SetShowLegend(FALSE);
//}
//
//void CRepGraphView::QueueLengthDetailed()
//{
//	// load report from file
//	CString sFileName;
//	if( GetLoadReportType() == load_by_user )
//	{
//		sFileName = GetUserLoadReportFile();
//	}
//	else								// load_by_system
//	{
//		sFileName = ((CTermPlanDoc*)GetDocument())->GetTerminal().GetSimReportManager()->GetCurrentReportFileName( ((CTermPlanDoc*)GetDocument())->m_ProjInfo.path );
//	}
//	chartFile.init (sFileName, READ);
//	//2.)Draw Title,X Axis Title,Y Axis Title,Footnote
//	chartFile.reset();
//	chartFile.getLine();
//	chartFile.getLine();
//	char string[4][128];
//	chartFile.getField (string[0], 128);
//	chartFile.getField (string[1], 128);
//	chartFile.getField (string[2], 128);
//	chartFile.getField (string[3], 128);
//	CString strFootnote;
//	strFootnote.Format("%s %s %s %s",NoUnderline(CString(string[0])),string[1],string[2],string[3]);
//	SetHeadXYTitle("Queue Length(Detailed)","Time of Day","Queue Length History",strFootnote);
//
//	ElapsedTime eBegin;
//	ElapsedTime eEnd;
//	getTime( string[1], eBegin, eEnd );
//	int interval = GetInterval(string[1]);	//interval, unit: minute
//	int nColNum = ( eEnd.asMinutes() - eBegin.asMinutes())/interval + 1; 
//
//
//	//3.)Draw it
//	CString strxValue[1000];//The Pax Type Name
//	int nPaxCount=0;
//	char buf[128];
//	CString strTemp;
//	float value = 0.0;
//	ElapsedTime valuetime;
//	float **yValue;
//	yValue = new float*[1000];
//	for(int k = 0; k < 1000; k++)
//	{
//		yValue[k] = new float[nColNum+1];
//	}
//	for(k=0;k<1000;k++)
//	{
//		for(int i=0;i< nColNum + 1;i++)
//		{
//			yValue[k][i]=0;
//		}
//	}
//	
//	int i=0;
//	chartFile.reset();
//	chartFile.seekToLine(0);
//	chartFile.skipLines(3);
//
//	if(!chartFile.getLine())
//	{
//		chartFile.closeIn();
//		return;
//	}
//	chartFile.setToField(0);
//	chartFile.getField(buf,128);
//	nPaxCount+=1;
//	strxValue[nPaxCount].Format("%s",buf);//Get The First Pax Type
//	
//	int IntervalNum = 1;
//	// ensure read from the 4th line
//	chartFile.reset();
//	chartFile.seekToLine(0);
//	chartFile.skipLines(3);	
//	while (chartFile.getLine())
//	{
//		chartFile.setToField(0);
//		chartFile.getField(buf,128);
//		strTemp.Format("%s",buf);
//		if(strxValue[nPaxCount]!=strTemp)
//		{
//			nPaxCount++;
//			strxValue[nPaxCount]=strTemp;
//			IntervalNum = 1;
//		}
//
//		chartFile.setToField (2);
//		chartFile.getFloat (value);
//		chartFile.setToField(1);
//		chartFile.getTime (valuetime, TRUE);
////		hours=(int)valuetime.asHours();
////		minutes=(int)valuetime.asMinutes();
////		// TRACE("the time is %d hours,or %d minutes\n",hours,minutes);
//		yValue[nPaxCount][IntervalNum++]=value;
//
////		if(minutes-hours*60<15)
////		{
////			//// TRACE("The value is %d",(int)value);
////			yValue[nPaxCount][hours]+=value;
////			//// TRACE("yValue[%d][%d]=%d\n",nPaxCount,(int)valuetime.asHours(),yValue[nPaxCount][((int)valuetime.asHours())]);
////
////		}
////		else
////		{
////			yValue[nPaxCount][hours+1]=value;
////			//// TRACE("yValue[%d][%d]=%d\n",nPaxCount,(int)valuetime.asHours()+1,yValue[nPaxCount][((int)valuetime.asHours())+1]);
////
////		}
//	}
//	
//	//////////////////////////////////////////////////////////////////////////
//	// add by bird 2003/10/13
//	ElapsedTime BeginTime,EndTime;
//	getTime( string[1], BeginTime, EndTime );
//	int nRowNum = (EndTime.asMinutes()-BeginTime.asMinutes())/interval +1; 
//
//	m_MSChartCtrl.SetRowCount(nRowNum);
//	m_MSChartCtrl.SetColumnCount(nPaxCount);
//	m_MSChartCtrl.SetColumnLabelCount(nPaxCount);
//	
//	CString strLabel;
//	for(i = 1; i <= nRowNum; i++)
//	{
//		ElapsedTime detaTime( interval * (i-1) * 60l );
//		ElapsedTime time = BeginTime + detaTime;
//		time.set(time.asSeconds() % WholeDay);
//		strLabel.Format("%s", time.printTime(0));
//		m_MSChartCtrl.SetRow(i);
//		DealRowLabel(strLabel);
//		m_MSChartCtrl.SetRowLabel(strLabel);
//		
//	}
//
//	for(k = 1 ; k <= nPaxCount; k++)
//	{
//		m_MSChartCtrl.SetColumn(k);
//		m_MSChartCtrl.SetColumnLabel(strxValue[k]);
//		for(i = 1; i <= nRowNum; i++)
//		{
//			m_MSChartCtrl.SetRow(i);
//			strLabel.Format("%f",yValue[k][i]);
//			m_MSChartCtrl.SetData(strLabel);
//		}
//	}
//
//
//	for(k = 0; k < 1000; k++)
//	{
//		delete[] yValue[k];
//	}
//	delete[] yValue;
//
//
//	chartFile.closeIn();
//	m_MSChartCtrl.SetShowLegend(TRUE);
//}
//void CRepGraphView::ConveyorWaitLengthDetailed()
//{
//	// load report from file
//	CString sFileName;
//	if( GetLoadReportType() == load_by_user )
//	{
//		sFileName = GetUserLoadReportFile();
//	}
//	else								// load_by_system
//	{
//		sFileName = ((CTermPlanDoc*)GetDocument())->GetTerminal().GetSimReportManager()->GetCurrentReportFileName( ((CTermPlanDoc*)GetDocument())->m_ProjInfo.path );
//	}
//	chartFile.init (sFileName, READ);
//	//2.)Draw Title,X Axis Title,Y Axis Title,Footnote
//	chartFile.reset();
//	chartFile.getLine();
//	chartFile.getLine();
//	char string[4][128];
//	chartFile.getField (string[0], 128);
//	chartFile.getField (string[1], 128);
//	chartFile.getField (string[2], 128);
//	chartFile.getField (string[3], 128);
//	CString strFootnote;
//	strFootnote.Format("%s %s %s %s",NoUnderline(CString(string[0])),string[1],string[2],string[3]);
//	SetHeadXYTitle("Conveyor Wait Length(Detailed)","Time of Day","onveyor Wait  Length History",strFootnote);
//
//	ElapsedTime eBegin;
//	ElapsedTime eEnd;
//	getTime( string[1], eBegin, eEnd );
//	int interval = GetInterval(string[1]);	//interval, unit: minute
//	int nColNum = ( eEnd.asMinutes() - eBegin.asMinutes())/interval + 1; 
//
//	//3.)Draw it
//	int nPaxCount=0;
//	CString strxValue[1000];//The Pax Type Name
//	char buf[128];
//	CString strTemp;
//	float value = 0.0;
//	ElapsedTime valuetime;
//	float **yValue = NULL;
//	yValue = new float*[1000];
//	for(int k = 0; k < 1000; k++)
//	{
//		yValue[k] = new float[nColNum+1];//25*60/interval+1
//		memset((void*)yValue[k], 0, sizeof(float) * (nColNum+1) );
//	}
//	
//	int i=0;
//	chartFile.reset();
//	chartFile.seekToLine(0);
//	chartFile.skipLines(3);
//
//	if(!chartFile.getLine())
//	{
//		chartFile.closeIn();
//		return;
//	}
//	chartFile.setToField(0);
//	chartFile.getField(buf,128);
//	nPaxCount+=1;
//	strxValue[nPaxCount].Format("%s",buf);//Get The First Pax Type
//	
//	int IntervalNum = 1;
//	// ensure read from the 4th line
//	chartFile.reset();
//	chartFile.seekToLine(0);
//	chartFile.skipLines(3);	
//	while (chartFile.getLine())
//	{
//		chartFile.setToField(0);
//		chartFile.getField(buf,128);
//		strTemp.Format("%s",buf);
//		if(strxValue[nPaxCount]!=strTemp)
//		{
//			nPaxCount++;
//			strxValue[nPaxCount]=strTemp;
//			IntervalNum = 1;
//		}
//
//		chartFile.setToField (2);
//		chartFile.getFloat (value);
//		chartFile.setToField(1);
//		chartFile.getTime (valuetime, TRUE);
//		yValue[nPaxCount][IntervalNum++]=value;
//	}
//	
//	//////////////////////////////////////////////////////////////////////////
//	// add by bird 2003/10/13
//	ElapsedTime BeginTime,EndTime;
//	getTime( string[1], BeginTime, EndTime );
//	int nRowNum = (EndTime.asMinutes()-BeginTime.asMinutes())/interval +1; 
//
//	m_MSChartCtrl.SetRowCount(nRowNum);
//	m_MSChartCtrl.SetColumnCount(nPaxCount);
//	m_MSChartCtrl.SetColumnLabelCount(nPaxCount);
//	
//	CString strLabel;
//	for(i = 1; i <= nRowNum; i++)
//	{
//		ElapsedTime detaTime( interval * (i-1) * 60l );
//		ElapsedTime time = BeginTime + detaTime;
//		strLabel.Format("%s", time.printTime(0));
//		m_MSChartCtrl.SetRow(i);
//		DealRowLabel(strLabel);
//		m_MSChartCtrl.SetRowLabel(strLabel);
//		
//	}
//
//	for(k = 1 ; k <= nPaxCount; k++)
//	{
//		m_MSChartCtrl.SetColumn(k);
//		m_MSChartCtrl.SetColumnLabel(strxValue[k]);
//		for(i = 1; i <= nRowNum; i++)
//		{
//			m_MSChartCtrl.SetRow(i);
//			strLabel.Format("%f",yValue[k][i]);
//			m_MSChartCtrl.SetData(strLabel);
//		}
//	}
//
//
//	if(yValue != NULL)
//	{
//		for(k = 0; k < 1000; k++)
//		{
//			if(yValue[k] != NULL)
//			{
//				delete[] yValue[k];
//				yValue[k] = NULL;
//			}
//		}
//		delete[] yValue;
//		yValue = NULL;
//	}
//
//
//	chartFile.closeIn();
//	m_MSChartCtrl.SetShowLegend(TRUE);
//}
//void CRepGraphView::ConveyorWaitLengthMaxSummary()
//{
//	// load report from file
//	CString sFileName;
//	if( GetLoadReportType() == load_by_user )
//	{
//		sFileName = GetUserLoadReportFile();
//	}
//	else								// load_by_system
//	{
//		sFileName = ((CTermPlanDoc*)GetDocument())->GetTerminal().GetSimReportManager()->GetCurrentReportFileName( ((CTermPlanDoc*)GetDocument())->m_ProjInfo.path );
//	}
//	chartFile.init (sFileName, READ);
//	//2.)Draw Title,X Axis Title,Y Axis Title,Footnote
//	chartFile.reset();
//	chartFile.getLine();
//	chartFile.getLine();
//	char string[4][128];
//	chartFile.getField (string[0], 128);
//	chartFile.getField (string[1], 128);
//	chartFile.getField (string[2], 128);
//	chartFile.getField (string[3], 128);
//	CString strFootnote;
//	strFootnote.Format("%s %s %s %s",NoUnderline(CString(string[0])),string[1],string[2],string[3]);
//	SetHeadXYTitle("Maximum Conveyor Length","Time of Day","Maximum Queue Length",strFootnote);
//	//3.)Draw it
//	CString strxValue[1000];//The Pax Type Name
//	int nPaxCount=0;
//	char buf[128];
//	CString strTemp;
//	float value = 0.0;
//	ElapsedTime valuetime;
//	float **yValue;
//	yValue = new float*[1000];
//	int interval = GetInterval(string[1]);
//	int hours,minutes;
//	for(int k=0;k<1000;k++)
//	{
//		yValue[k] = new float[25*60/interval+1];
//	}
//	for(k=0;k<1000;k++)
//	{
//		for(int i=0;i<25*60/interval + 1;i++)
//		{
//			yValue[k][i]=0;
//		}
//	}
//	int i=0;
//	chartFile.reset();
//	chartFile.skipLines(3);
//
//	if(!chartFile.getLine())
//	{
//		chartFile.closeIn();
//		return;
//	}
//	chartFile.setToField(0);
//	chartFile.getField(buf,128);
//	nPaxCount+=1;
//	strxValue[nPaxCount].Format("%s",buf);//Get The First Pax Type
//	
//	int IntervalNum = 1;
//	// ensure read from the 4th line
//	chartFile.reset();
//	chartFile.seekToLine(0);
//	chartFile.skipLines(3);	
//	while (chartFile.getLine())
//	{
//		chartFile.setToField(0);
//		chartFile.getField(buf,128);
//		strTemp.Format("%s",buf);
//		if(strxValue[nPaxCount]!=strTemp)
//		{
//			nPaxCount++;
//			strxValue[nPaxCount]=strTemp;
//			IntervalNum = 1;
//		}
//
//		chartFile.setToField (5);
//		chartFile.getFloat (value);
//		chartFile.setToField(2);
//		chartFile.getTime (valuetime, TRUE);
//		hours=(int)valuetime.asHours();
//		minutes=(int)valuetime.asMinutes();
//		
//		// TRACE("the time is %d hours,or %d minutes\n",hours,minutes);
//		yValue[nPaxCount][IntervalNum++]=value;	
//	}
//	
//	// add by bird 2003/10/13
//	ElapsedTime BeginTime, EndTime;
//	getTime( string[1], BeginTime, EndTime );
//	int nRowNum = (EndTime.asMinutes()-BeginTime.asMinutes())/interval +1; 
//
//	m_MSChartCtrl.SetRowCount(nRowNum);
//	m_MSChartCtrl.SetColumnCount(nPaxCount);
//	m_MSChartCtrl.SetColumnLabelCount(nPaxCount);
//	
//	CString strLabel;
//
//	for(i = 1; i <= nRowNum; i++)
//	{
//		ElapsedTime detaTime( interval * (i-1) * 60l );
//		ElapsedTime time = BeginTime + detaTime;
//		strLabel.Format("%s", time.printTime(0));
//		m_MSChartCtrl.SetRow(i);
//		DealRowLabel(strLabel);
//		m_MSChartCtrl.SetRowLabel(strLabel);
//		
//	}
//
//	for(k = 1 ; k <= nPaxCount; k++)
//	{
//		m_MSChartCtrl.SetColumn(k);
//		m_MSChartCtrl.SetColumnLabel(strxValue[k]);
//		for(i = 1; i <= nRowNum; i++)
//		{
//			m_MSChartCtrl.SetRow(i);
//			strLabel.Format("%f",yValue[k][i]);
//			m_MSChartCtrl.SetData(strLabel);
//		}
//	}
//
//	for(k = 0; k < 1000; k++)
//	{
//		delete[] yValue[k];
//	}
//	delete[] yValue;
//
//	chartFile.closeIn();
//	m_MSChartCtrl.SetShowLegend(TRUE);
//}
//void CRepGraphView::ConveyorWaitLengthAvgSummary()
//{
//	// load report from file
//	CString sFileName;
//	if( GetLoadReportType() == load_by_user )
//	{
//		sFileName = GetUserLoadReportFile();
//	}
//	else								// load_by_system
//	{
//		sFileName = ((CTermPlanDoc*)GetDocument())->GetTerminal().GetSimReportManager()->GetCurrentReportFileName( ((CTermPlanDoc*)GetDocument())->m_ProjInfo.path );
//	}
//	chartFile.init (sFileName, READ);
//	//2.)Draw Title,X Axis Title,Y Axis Title,Footnote
//	chartFile.reset();
//	chartFile.getLine();
//	chartFile.getLine();
//	char string[4][128];
//	chartFile.getField (string[0], 128);
//	chartFile.getField (string[1], 128);
//	chartFile.getField (string[2], 128);
//	chartFile.getField (string[3], 128);
//	CString strFootnote;
//	strFootnote.Format("%s %s %s %s",NoUnderline(CString(string[0])),string[1],string[2],string[3]);
//	SetHeadXYTitle("Average Conveyor Wait Length","Time of Day","Average Queue Length",strFootnote);
//	//3.)Draw it
//	CString strxValue[1000];//The Pax Type Name
//	int nPaxCount=0;
//	char buf[128];
//	CString strTemp;
//	float value = 0.0;
//	ElapsedTime valuetime;
//	int hours,minutes;
//	float **yValue;
//	int interval = GetInterval(string[1]);
//	yValue = new float*[1000];
//	for(int k=0;k<1000;k++)
//	{
//		yValue[k] = new float[25*60/interval+1];
//	}
//	
//	int i=0;
//	chartFile.reset();
//	chartFile.skipLines(3);
//
//	if(!chartFile.getLine())
//	{
//		chartFile.closeIn();
//		return;
//	}
//	chartFile.setToField(0);
//	chartFile.getField(buf,128);
//	nPaxCount+=1;
//	strxValue[nPaxCount].Format("%s",buf);//Get The First Pax Type
//	
//	int IntervalNum = 1;
//	// ensure read from the 4th line
//	chartFile.reset();
//	chartFile.seekToLine(0);
//	chartFile.skipLines(3);	
//	while (chartFile.getLine())
//	{
//		chartFile.setToField(0);
//		chartFile.getField(buf,128);
//		strTemp.Format("%s",buf);
//		if(strxValue[nPaxCount]!=strTemp)
//		{
//			nPaxCount++;
//			strxValue[nPaxCount]=strTemp;
//			IntervalNum = 1;
//		}
//
//		chartFile.setToField (4);
//		chartFile.getFloat (value);
//		chartFile.setToField(2);
//		chartFile.getTime (valuetime, TRUE);
//		hours=(int)valuetime.asHours();
//		minutes=(int)valuetime.asMinutes();
//		
//		// TRACE("the time is %d hours,or %d minutes\n",hours,minutes);
//		yValue[nPaxCount][IntervalNum++]=value;
//	}
//	
//	// add by bird 2003/10/13
//	ElapsedTime BeginTime, EndTime;
//	getTime( string[1], BeginTime, EndTime );
//	int nRowNum = (EndTime.asMinutes()-BeginTime.asMinutes())/interval +1; 
//
//	m_MSChartCtrl.SetRowCount(nRowNum);
//	m_MSChartCtrl.SetColumnCount(nPaxCount);
//	m_MSChartCtrl.SetColumnLabelCount(nPaxCount);
//	
//	CString strLabel;
//	for(i = 1; i <= nRowNum; i++)
//	{
//		ElapsedTime detaTime( interval * (i-1) * 60l );
//		ElapsedTime time = BeginTime + detaTime;
//		strLabel.Format("%s", time.printTime(0));
//		m_MSChartCtrl.SetRow(i);
//		DealRowLabel(strLabel);
//		m_MSChartCtrl.SetRowLabel(strLabel);
//		
//	}
//
//	for(k = 1 ; k <= nPaxCount; k++)
//	{
//		m_MSChartCtrl.SetColumn(k);
//		m_MSChartCtrl.SetColumnLabel(strxValue[k]);
//		for(i = 1; i <= nRowNum; i++)
//		{
//			m_MSChartCtrl.SetRow(i);
//			strLabel.Format("%f",yValue[k][i]);
//			m_MSChartCtrl.SetData(strLabel);
//		}
//	}
//
//	for(k = 0; k < 1000; k++)
//	{
//		delete[] yValue[k];
//	}
//	delete[] yValue;
//
//	chartFile.closeIn();
//	m_MSChartCtrl.SetShowLegend(TRUE);
//}
//void CRepGraphView::ConveyorWaitLengthMinSummary()
//{
//	// load report from file
//	// load report from file
//	CString sFileName;
//	if( GetLoadReportType() == load_by_user )
//	{
//		sFileName = GetUserLoadReportFile();
//	}
//	else								// load_by_system
//	{
//		sFileName = ((CTermPlanDoc*)GetDocument())->GetTerminal().GetSimReportManager()->GetCurrentReportFileName( ((CTermPlanDoc*)GetDocument())->m_ProjInfo.path );
//	}
//	chartFile.init (sFileName, READ);
//	//2.)Draw Title,X Axis Title,Y Axis Title,Footnote
//	chartFile.reset();
//	chartFile.getLine();
//	chartFile.getLine();
//	char string[4][128];
//	chartFile.getField (string[0], 128);
//	chartFile.getField (string[1], 128);
//	chartFile.getField (string[2], 128);
//	chartFile.getField (string[3], 128);
//	CString strFootnote;
//	strFootnote.Format("%s %s %s %s",NoUnderline(CString(string[0])),string[1],string[2],string[3]);
//	SetHeadXYTitle("Minimum Conveyor Wait Length","Time of Day","Minimum Query Length",strFootnote);
//	//3.)Draw it
//	CString strxValue[1000];//The Pax Type Name
//	int nPaxCount=0;
//	char buf[128];
//	CString strTemp;
//	float value = 0.0;
//	ElapsedTime valuetime;
//	int hours,minutes;
//	float **yValue;
//	yValue = new float*[1000];
//	int interval = GetInterval(string[1]);
//	for(int k = 0; k < 1000; k++)
//	{
//		yValue[k] = new float[25*60/interval+1];
//	}
//	for(k=0;k<1000;k++)
//	{
//		for(int i=0;i<25*60/interval+1;i++)
//		{
//			yValue[k][i]=0;
//		}
//	}
//	
//	int i=0;
//	chartFile.reset();
//	chartFile.skipLines(3);
//
//	if(!chartFile.getLine())
//	{
//		chartFile.closeIn();
//		return;
//	}
//	chartFile.setToField(0);
//	chartFile.getField(buf,128);
//	nPaxCount+=1;
//	strxValue[nPaxCount].Format("%s",buf);//Get The First Pax Type
//	
//	int IntervalNum = 1;
//	// ensure read from the 4th line
//	chartFile.reset();
//	chartFile.seekToLine(0);
//	chartFile.skipLines(3);	
//	while (chartFile.getLine())
//	{
//		chartFile.setToField(0);
//		chartFile.getField(buf,128);
//		strTemp.Format("%s",buf);
//		if(strxValue[nPaxCount]!=strTemp)
//		{
//			nPaxCount++;
//			strxValue[nPaxCount]=strTemp;
//			IntervalNum = 1;
//		}
//
//		chartFile.setToField (3);
//		chartFile.getFloat (value);
//		chartFile.setToField(2);
//		chartFile.getTime (valuetime, TRUE);
//		hours=(int)valuetime.asHours();
//		minutes=(int)valuetime.asMinutes();
//		
//		// TRACE("the time is %d hours,or %d minutes\n",hours,minutes);
//		yValue[nPaxCount][IntervalNum++]=value;
//	}
//	
//	// add by bird 2003/10/13
//	ElapsedTime BeginTime, EndTime;
//	getTime( string[1], BeginTime, EndTime );
//	int nRowNum = (EndTime.asMinutes()-BeginTime.asMinutes())/interval +1; 
//
//	m_MSChartCtrl.SetRowCount(nRowNum);
//	m_MSChartCtrl.SetColumnCount(nPaxCount);
//	m_MSChartCtrl.SetColumnLabelCount(nPaxCount);
//	
//	CString strLabel;
//	for(i = 1; i <= nRowNum; i++)
//	{
//		ElapsedTime detaTime( interval * (i-1) * 60l );
//		ElapsedTime time = BeginTime + detaTime;
//		strLabel.Format("%s", time.printTime(0));
//		m_MSChartCtrl.SetRow(i);
//		DealRowLabel(strLabel);
//		m_MSChartCtrl.SetRowLabel(strLabel);
//		
//	}
//
//	for(k = 1 ; k <= nPaxCount; k++)
//	{
//		m_MSChartCtrl.SetColumn(k);
//		m_MSChartCtrl.SetColumnLabel(strxValue[k]);
//		for(i = 1; i <= nRowNum; i++)
//		{
//			m_MSChartCtrl.SetRow(i);
//			strLabel.Format("%f",yValue[k][i]);
//			m_MSChartCtrl.SetData(strLabel);
//		}
//	}
//
//
//	for(k = 0; k < 1000; k++)
//	{
//		delete[] yValue[k];
//	}
//	delete[] yValue;
//	chartFile.closeIn();
//	m_MSChartCtrl.SetShowLegend(TRUE);
//}
//void CRepGraphView::ConveyorWaitLengthCombinedSummary()
//{
//// load report from file
//	CString sFileName;
//	if( GetLoadReportType() == load_by_user )
//	{
//		sFileName = GetUserLoadReportFile();
//	}
//	else								// load_by_system
//	{
//		sFileName = ((CTermPlanDoc*)GetDocument())->GetTerminal().GetSimReportManager()->GetCurrentReportFileName( ((CTermPlanDoc*)GetDocument())->m_ProjInfo.path );
//	}
//	chartFile.init (sFileName, READ);
//	//2.)Draw Title,X Axis Title,Y Axis Title,Footnote
//	chartFile.reset();
//	chartFile.getLine();
//	chartFile.getLine();
//	char string[4][128];
//	chartFile.getField (string[0], 128);
//	chartFile.getField (string[1], 128);
//	chartFile.getField (string[2], 128);
//	chartFile.getField (string[3], 128);
//	CString strFootnote;
//	strFootnote.Format("%s %s %s %s",NoUnderline(CString(string[0])),string[1],string[2],string[3]);
//	SetHeadXYTitle("Combined Conveyor Wait Length","Time of Day","Combined Queue Length",strFootnote);
//	//3.)Draw it
//	CString strxValue[1000];//The Pax Type Name
//	int nPaxCount=0;
//	float **yValue;
//	yValue = new float*[1000];
//	int interval = GetInterval(string[1]);
//	char buf[128];
//	CString strTemp;
//	float value = 0.0;
//	ElapsedTime valuetime;
//	int hours,minutes;
//	for(int k=0; k<1000; k++)
//	{
//		yValue[k] = new float[25*60/interval+1];
//	}
//	for(k=0;k<1000;k++)
//	{
//		for(int i=0;i<25*60/interval+1;i++)
//		{
//			yValue[k][i]=0;
//		}
//	}
//	
//	int i=0;
//	chartFile.reset();
//	chartFile.skipLines(3);
//
//	if(!chartFile.getLine())
//	{
//		chartFile.closeIn();
//		return;
//	}
//	chartFile.setToField(0);
//	chartFile.getField(buf,128);
//	nPaxCount+=1;
//	strxValue[nPaxCount].Format("%s",buf);//Get The First Pax Type
//	
//	int IntervalNum = 1;
//	// ensure read from the 4th line
//	chartFile.reset();
//	chartFile.seekToLine(0);
//	chartFile.skipLines(3);	
//	while (chartFile.getLine())
//	{
//		chartFile.setToField(0);
//		chartFile.getField(buf,128);
//		strTemp.Format("%s",buf);
//		if(strxValue[nPaxCount]!=strTemp)
//		{
//			nPaxCount++;
//			strxValue[nPaxCount]=strTemp;
//			IntervalNum = 1;
//		}
//
//		chartFile.setToField (6);
//		chartFile.getFloat (value);
//		chartFile.setToField(2);
//		chartFile.getTime (valuetime, TRUE);
//		hours=(int)valuetime.asHours();
//		minutes=(int)valuetime.asMinutes();
//		
//		// TRACE("the time is %d hours,or %d minutes\n",hours,minutes);
//		yValue[nPaxCount][IntervalNum++]=value;	
//	}
//	
//	// add by bird 2003/10/13
//	ElapsedTime BeginTime, EndTime;
//	getTime( string[1], BeginTime, EndTime );
//	int nRowNum = (EndTime.asMinutes()-BeginTime.asMinutes())/interval +1; 
//	
//	m_MSChartCtrl.SetRowCount(nRowNum);
//	m_MSChartCtrl.SetColumnCount(nPaxCount);
//	m_MSChartCtrl.SetColumnLabelCount(nPaxCount);
//	
//	CString strLabel;
//
//	for(i = 1; i <= nRowNum; i++)
//	{
//		ElapsedTime detaTime( interval * (i-1) * 60l );
//		ElapsedTime time = BeginTime + detaTime;
//		strLabel.Format("%s", time.printTime(0));
//		m_MSChartCtrl.SetRow(i);
//		DealRowLabel(strLabel);
//		m_MSChartCtrl.SetRowLabel(strLabel);
//		
//	}
//
//	for(k = 1 ; k <= nPaxCount; k++)
//	{
//		m_MSChartCtrl.SetColumn(k);
//		m_MSChartCtrl.SetColumnLabel(strxValue[k]);
//		for(i = 1; i <= nRowNum; i++)
//		{
//			m_MSChartCtrl.SetRow(i);
//			strLabel.Format("%f",yValue[k][i]);
//			m_MSChartCtrl.SetData(strLabel);
//		}
//	}
//
//	for(k = 0; k < 1000; k++)
//	{
//		delete[] yValue[k];
//	}
//	delete[] yValue;
//	chartFile.closeIn();
//	m_MSChartCtrl.SetShowLegend(TRUE);
//}
//void CRepGraphView::QueueLengthMinSummary()
//{
//	// load report from file
//	CString sFileName;
//	if( GetLoadReportType() == load_by_user )
//	{
//		sFileName = GetUserLoadReportFile();
//	}
//	else								// load_by_system
//	{
//		sFileName = ((CTermPlanDoc*)GetDocument())->GetTerminal().GetSimReportManager()->GetCurrentReportFileName( ((CTermPlanDoc*)GetDocument())->m_ProjInfo.path );
//	}
//	chartFile.init (sFileName, READ);
//	//2.)Draw Title,X Axis Title,Y Axis Title,Footnote
//	chartFile.reset();
//	chartFile.getLine();
//	chartFile.getLine();
//	char string[4][128];
//	chartFile.getField (string[0], 128);
//	chartFile.getField (string[1], 128);
//	chartFile.getField (string[2], 128);
//	chartFile.getField (string[3], 128);
//	CString strFootnote;
//	strFootnote.Format("%s %s %s %s",NoUnderline(CString(string[0])),string[1],string[2],string[3]);
//	SetHeadXYTitle("Minimum Query Length","Time of Day","Minimum Query Length",strFootnote);
//	//3.)Draw it
//	CString strxValue[1000];//The Pax Type Name
//	int nPaxCount=0;
//	char buf[128];
//	CString strTemp;
//	float value = 0.0;
//	ElapsedTime valuetime;
//	int hours,minutes;
//	float **yValue;
//	yValue = new float*[1000];
//	int interval = GetInterval(string[1]);
//	for(int k = 0; k < 1000; k++)
//	{
//		yValue[k] = new float[25*60/interval+1];
//	}
//	for(k=0;k<1000;k++)
//	{
//		for(int i=0;i<25*60/interval+1;i++)
//		{
//			yValue[k][i]=0;
//		}
//	}
//	
//	int i=0;
//	chartFile.reset();
//	chartFile.skipLines(3);
//
//	if(!chartFile.getLine())
//	{
//		chartFile.closeIn();
//		return;
//	}
//	chartFile.setToField(0);
//	chartFile.getField(buf,128);
//	nPaxCount+=1;
//	strxValue[nPaxCount].Format("%s",buf);//Get The First Pax Type
//	
//	int IntervalNum = 1;
//	// ensure read from the 4th line
//	chartFile.reset();
//	chartFile.seekToLine(0);
//	chartFile.skipLines(3);	
//	while (chartFile.getLine())
//	{
//		chartFile.setToField(0);
//		chartFile.getField(buf,128);
//		strTemp.Format("%s",buf);
//		if(strxValue[nPaxCount]!=strTemp)
//		{
//			nPaxCount++;
//			strxValue[nPaxCount]=strTemp;
//			IntervalNum = 1;
//		}
//
//		chartFile.setToField (3);
//		chartFile.getFloat (value);
//		chartFile.setToField(2);
//		chartFile.getTime (valuetime, TRUE);
//		hours=(int)valuetime.asHours();
//		minutes=(int)valuetime.asMinutes();
//		
//		// TRACE("the time is %d hours,or %d minutes\n",hours,minutes);
//		yValue[nPaxCount][IntervalNum++]=value;
//	}
//	
//	// add by bird 2003/10/13
//	ElapsedTime BeginTime, EndTime;
//	getTime( string[1], BeginTime, EndTime );
//	int nRowNum = (EndTime.asMinutes()-BeginTime.asMinutes())/interval +1; 
//
//	m_MSChartCtrl.SetRowCount(nRowNum);
//	m_MSChartCtrl.SetColumnCount(nPaxCount);
//	m_MSChartCtrl.SetColumnLabelCount(nPaxCount);
//	
//	CString strLabel;
//	for(i = 1; i <= nRowNum; i++)
//	{
//		ElapsedTime detaTime( interval * (i-1) * 60l );
//		ElapsedTime time = BeginTime + detaTime;
//		strLabel.Format("%s", time.printTime(0));
//		m_MSChartCtrl.SetRow(i);
//		DealRowLabel(strLabel);
//		m_MSChartCtrl.SetRowLabel(strLabel);
//		
//	}
//
//	for(k = 1 ; k <= nPaxCount; k++)
//	{
//		m_MSChartCtrl.SetColumn(k);
//		m_MSChartCtrl.SetColumnLabel(strxValue[k]);
//		for(i = 1; i <= nRowNum; i++)
//		{
//			m_MSChartCtrl.SetRow(i);
//			strLabel.Format("%f",yValue[k][i]);
//			m_MSChartCtrl.SetData(strLabel);
//		}
//	}
//
//
//	for(k = 0; k < 1000; k++)
//	{
//		delete[] yValue[k];
//	}
//	delete[] yValue;
//	chartFile.closeIn();
//	m_MSChartCtrl.SetShowLegend(TRUE);
//}
//
//void CRepGraphView::QueueLengthAvgSummary()
//{
//	// load report from file
//	CString sFileName;
//	if( GetLoadReportType() == load_by_user )
//	{
//		sFileName = GetUserLoadReportFile();
//	}
//	else								// load_by_system
//	{
//		sFileName = ((CTermPlanDoc*)GetDocument())->GetTerminal().GetSimReportManager()->GetCurrentReportFileName( ((CTermPlanDoc*)GetDocument())->m_ProjInfo.path );
//	}
//	chartFile.init (sFileName, READ);
//	//2.)Draw Title,X Axis Title,Y Axis Title,Footnote
//	chartFile.reset();
//	chartFile.getLine();
//	chartFile.getLine();
//	char string[4][128];
//	chartFile.getField (string[0], 128);
//	chartFile.getField (string[1], 128);
//	chartFile.getField (string[2], 128);
//	chartFile.getField (string[3], 128);
//	CString strFootnote;
//	strFootnote.Format("%s %s %s %s",NoUnderline(CString(string[0])),string[1],string[2],string[3]);
//	SetHeadXYTitle("Average Queue Length","Time of Day","Average Queue Length",strFootnote);
//	//3.)Draw it
//	CString strxValue[1000];//The Pax Type Name
//	int nPaxCount=0;
//	char buf[128];
//	CString strTemp;
//	float value = 0.0;
//	ElapsedTime valuetime;
//	int hours,minutes;
//	float **yValue;
//	int interval = GetInterval(string[1]);
//	yValue = new float*[1000];
//	for(int k=0;k<1000;k++)
//	{
//		yValue[k] = new float[25*60/interval+1];
//	}
//	
//	int i=0;
//	chartFile.reset();
//	chartFile.skipLines(3);
//
//	if(!chartFile.getLine())
//	{
//		chartFile.closeIn();
//		return;
//	}
//	chartFile.setToField(0);
//	chartFile.getField(buf,128);
//	nPaxCount+=1;
//	strxValue[nPaxCount].Format("%s",buf);//Get The First Pax Type
//	
//	int IntervalNum = 1;
//	// ensure read from the 4th line
//	chartFile.reset();
//	chartFile.seekToLine(0);
//	chartFile.skipLines(3);	
//	while (chartFile.getLine())
//	{
//		chartFile.setToField(0);
//		chartFile.getField(buf,128);
//		strTemp.Format("%s",buf);
//		if(strxValue[nPaxCount]!=strTemp)
//		{
//			nPaxCount++;
//			strxValue[nPaxCount]=strTemp;
//			IntervalNum = 1;
//		}
//
//		chartFile.setToField (4);
//		chartFile.getFloat (value);
//		chartFile.setToField(2);
//		chartFile.getTime (valuetime, TRUE);
//		hours=(int)valuetime.asHours();
//		minutes=(int)valuetime.asMinutes();
//		
//		// TRACE("the time is %d hours,or %d minutes\n",hours,minutes);
//		yValue[nPaxCount][IntervalNum++]=value;
//	}
//	
//	// add by bird 2003/10/13
//	ElapsedTime BeginTime, EndTime;
//	getTime( string[1], BeginTime, EndTime );
//	int nRowNum = (EndTime.asMinutes()-BeginTime.asMinutes())/interval +1; 
//
//	m_MSChartCtrl.SetRowCount(nRowNum);
//	m_MSChartCtrl.SetColumnCount(nPaxCount);
//	m_MSChartCtrl.SetColumnLabelCount(nPaxCount);
//	
//	CString strLabel;
//	for(i = 1; i <= nRowNum; i++)
//	{
//		ElapsedTime detaTime( interval * (i-1) * 60l );
//		ElapsedTime time = BeginTime + detaTime;
//		strLabel.Format("%s", time.printTime(0));
//		m_MSChartCtrl.SetRow(i);
//		DealRowLabel(strLabel);
//		m_MSChartCtrl.SetRowLabel(strLabel);
//		
//	}
//
//	for(k = 1 ; k <= nPaxCount; k++)
//	{
//		m_MSChartCtrl.SetColumn(k);
//		m_MSChartCtrl.SetColumnLabel(strxValue[k]);
//		for(i = 1; i <= nRowNum; i++)
//		{
//			m_MSChartCtrl.SetRow(i);
//			strLabel.Format("%f",yValue[k][i]);
//			m_MSChartCtrl.SetData(strLabel);
//		}
//	}
//
//	for(k = 0; k < 1000; k++)
//	{
//		delete[] yValue[k];
//	}
//	delete[] yValue;
//
//	chartFile.closeIn();
//	m_MSChartCtrl.SetShowLegend(TRUE);
//}
//
//void CRepGraphView::QueueLengthMaxSummary()
//{
//	// load report from file
//	CString sFileName;
//	if( GetLoadReportType() == load_by_user )
//	{
//		sFileName = GetUserLoadReportFile();
//	}
//	else								// load_by_system
//	{
//		sFileName = ((CTermPlanDoc*)GetDocument())->GetTerminal().GetSimReportManager()->GetCurrentReportFileName( ((CTermPlanDoc*)GetDocument())->m_ProjInfo.path );
//	}
//	chartFile.init (sFileName, READ);
//	//2.)Draw Title,X Axis Title,Y Axis Title,Footnote
//	chartFile.reset();
//	chartFile.getLine();
//	chartFile.getLine();
//	char string[4][128];
//	chartFile.getField (string[0], 128);
//	chartFile.getField (string[1], 128);
//	chartFile.getField (string[2], 128);
//	chartFile.getField (string[3], 128);
//	CString strFootnote;
//	strFootnote.Format("%s %s %s %s",NoUnderline(CString(string[0])),string[1],string[2],string[3]);
//	SetHeadXYTitle("Maximum Queue Length","Time of Day","Maximum Queue Length",strFootnote);
//	//3.)Draw it
//	CString strxValue[1000];//The Pax Type Name
//	int nPaxCount=0;
//	char buf[128];
//	CString strTemp;
//	float value = 0.0;
//	ElapsedTime valuetime;
//	float **yValue;
//	yValue = new float*[1000];
//	int interval = GetInterval(string[1]);
//	int hours,minutes;
//	for(int k=0;k<1000;k++)
//	{
//		yValue[k] = new float[25*60/interval+1];
//	}
//	for(k=0;k<1000;k++)
//	{
//		for(int i=0;i<25*60/interval + 1;i++)
//		{
//			yValue[k][i]=0;
//		}
//	}
//	int i=0;
//	chartFile.reset();
//	chartFile.skipLines(3);
//
//	if(!chartFile.getLine())
//	{
//		chartFile.closeIn();
//		return;
//	}
//	chartFile.setToField(0);
//	chartFile.getField(buf,128);
//	nPaxCount+=1;
//	strxValue[nPaxCount].Format("%s",buf);//Get The First Pax Type
//	
//	int IntervalNum = 1;
//	// ensure read from the 4th line
//	chartFile.reset();
//	chartFile.seekToLine(0);
//	chartFile.skipLines(3);	
//	while (chartFile.getLine())
//	{
//		chartFile.setToField(0);
//		chartFile.getField(buf,128);
//		strTemp.Format("%s",buf);
//		if(strxValue[nPaxCount]!=strTemp)
//		{
//			nPaxCount++;
//			strxValue[nPaxCount]=strTemp;
//			IntervalNum = 1;
//		}
//
//		chartFile.setToField (5);
//		chartFile.getFloat (value);
//		chartFile.setToField(2);
//		chartFile.getTime (valuetime, TRUE);
//		hours=(int)valuetime.asHours();
//		minutes=(int)valuetime.asMinutes();
//		
//		// TRACE("the time is %d hours,or %d minutes\n",hours,minutes);
//		yValue[nPaxCount][IntervalNum++]=value;	
//	}
//	
//	// add by bird 2003/10/13
//	ElapsedTime BeginTime, EndTime;
//	getTime( string[1], BeginTime, EndTime );
//	int nRowNum = (EndTime.asMinutes()-BeginTime.asMinutes())/interval +1; 
//
//	m_MSChartCtrl.SetRowCount(nRowNum);
//	m_MSChartCtrl.SetColumnCount(nPaxCount);
//	m_MSChartCtrl.SetColumnLabelCount(nPaxCount);
//	
//	CString strLabel;
//
//	for(i = 1; i <= nRowNum; i++)
//	{
//		ElapsedTime detaTime( interval * (i-1) * 60l );
//		ElapsedTime time = BeginTime + detaTime;
//		strLabel.Format("%s", time.printTime(0));
//		m_MSChartCtrl.SetRow(i);
//		DealRowLabel(strLabel);
//		m_MSChartCtrl.SetRowLabel(strLabel);
//		
//	}
//
//	for(k = 1 ; k <= nPaxCount; k++)
//	{
//		m_MSChartCtrl.SetColumn(k);
//		m_MSChartCtrl.SetColumnLabel(strxValue[k]);
//		for(i = 1; i <= nRowNum; i++)
//		{
//			m_MSChartCtrl.SetRow(i);
//			strLabel.Format("%f",yValue[k][i]);
//			m_MSChartCtrl.SetData(strLabel);
//		}
//	}
//
//	for(k = 0; k < 1000; k++)
//	{
//		delete[] yValue[k];
//	}
//	delete[] yValue;
//
//	chartFile.closeIn();
//	m_MSChartCtrl.SetShowLegend(TRUE);
//}
//
//void CRepGraphView::QueueLengthCombinedSummary()
//{
//	// load report from file
//	CString sFileName;
//	if( GetLoadReportType() == load_by_user )
//	{
//		sFileName = GetUserLoadReportFile();
//	}
//	else								// load_by_system
//	{
//		sFileName = ((CTermPlanDoc*)GetDocument())->GetTerminal().GetSimReportManager()->GetCurrentReportFileName( ((CTermPlanDoc*)GetDocument())->m_ProjInfo.path );
//	}
//	chartFile.init (sFileName, READ);
//	//2.)Draw Title,X Axis Title,Y Axis Title,Footnote
//	chartFile.reset();
//	chartFile.getLine();
//	chartFile.getLine();
//	char string[4][128];
//	chartFile.getField (string[0], 128);
//	chartFile.getField (string[1], 128);
//	chartFile.getField (string[2], 128);
//	chartFile.getField (string[3], 128);
//	CString strFootnote;
//	strFootnote.Format("%s %s %s %s",NoUnderline(CString(string[0])),string[1],string[2],string[3]);
//	SetHeadXYTitle("Combined Queue Length","Time of Day","Combined Queue Length",strFootnote);
//	//3.)Draw it
//	CString strxValue[1000];//The Pax Type Name
//	int nPaxCount=0;
//	float **yValue;
//	yValue = new float*[1000];
//	int interval = GetInterval(string[1]);
//	char buf[128];
//	CString strTemp;
//	float value = 0.0;
//	ElapsedTime valuetime;
//	int hours,minutes;
//	for(int k=0; k<1000; k++)
//	{
//		yValue[k] = new float[25*60/interval+1];
//	}
//	for(k=0;k<1000;k++)
//	{
//		for(int i=0;i<25*60/interval+1;i++)
//		{
//			yValue[k][i]=0;
//		}
//	}
//	
//	int i=0;
//	chartFile.reset();
//	chartFile.skipLines(3);
//
//	if(!chartFile.getLine())
//	{
//		chartFile.closeIn();
//		return;
//	}
//	chartFile.setToField(0);
//	chartFile.getField(buf,128);
//	nPaxCount+=1;
//	strxValue[nPaxCount].Format("%s",buf);//Get The First Pax Type
//	
//	int IntervalNum = 1;
//	// ensure read from the 4th line
//	chartFile.reset();
//	chartFile.seekToLine(0);
//	chartFile.skipLines(3);	
//	while (chartFile.getLine())
//	{
//		chartFile.setToField(0);
//		chartFile.getField(buf,128);
//		strTemp.Format("%s",buf);
//		if(strxValue[nPaxCount]!=strTemp)
//		{
//			nPaxCount++;
//			strxValue[nPaxCount]=strTemp;
//			IntervalNum = 1;
//		}
//
//		chartFile.setToField (6);
//		chartFile.getFloat (value);
//		chartFile.setToField(2);
//		chartFile.getTime (valuetime, TRUE);
//		hours=(int)valuetime.asHours();
//		minutes=(int)valuetime.asMinutes();
//		
//		// TRACE("the time is %d hours,or %d minutes\n",hours,minutes);
//		yValue[nPaxCount][IntervalNum++]=value;	
//	}
//	
//	// add by bird 2003/10/13
//	ElapsedTime BeginTime, EndTime;
//	getTime( string[1], BeginTime, EndTime );
//	int nRowNum = (EndTime.asMinutes()-BeginTime.asMinutes())/interval +1; 
//	
//	m_MSChartCtrl.SetRowCount(nRowNum);
//	m_MSChartCtrl.SetColumnCount(nPaxCount);
//	m_MSChartCtrl.SetColumnLabelCount(nPaxCount);
//	
//	CString strLabel;
//
//	for(i = 1; i <= nRowNum; i++)
//	{
//		ElapsedTime detaTime( interval * (i-1) * 60l );
//		ElapsedTime time = BeginTime + detaTime;
//		strLabel.Format("%s", time.printTime(0));
//		m_MSChartCtrl.SetRow(i);
//		DealRowLabel(strLabel);
//		m_MSChartCtrl.SetRowLabel(strLabel);
//		
//	}
//
//	for(k = 1 ; k <= nPaxCount; k++)
//	{
//		m_MSChartCtrl.SetColumn(k);
//		m_MSChartCtrl.SetColumnLabel(strxValue[k]);
//		for(i = 1; i <= nRowNum; i++)
//		{
//			m_MSChartCtrl.SetRow(i);
//			strLabel.Format("%f",yValue[k][i]);
//			m_MSChartCtrl.SetData(strLabel);
//		}
//	}
//
//	for(k = 0; k < 1000; k++)
//	{
//		delete[] yValue[k];
//	}
//	delete[] yValue;
//	chartFile.closeIn();
//	m_MSChartCtrl.SetShowLegend(TRUE);
//}
//
//void CRepGraphView::AvgQueueLenMaxQueTimeHisSummary()
//{
//	// load report from file
//	CString sFileName;
//	if( GetLoadReportType() == load_by_user )
//	{
//		sFileName = GetUserLoadReportFile();
//	}
//	else								// load_by_system
//	{
//		sFileName = ((CTermPlanDoc*)GetDocument())->GetTerminal().GetSimReportManager()->GetCurrentReportFileName( ((CTermPlanDoc*)GetDocument())->m_ProjInfo.path );
//	}
//	chartFile.init (sFileName, READ);
////	if (chartFile.countLines() <= 5)
////		return;
//	//2.)Draw Title,X Axis Title,Y Axis Title,Footnote
//	chartFile.reset();
//	chartFile.getLine();
//	chartFile.getLine();
//	char string[4][128];
//	chartFile.getField (string[0], 128);
//	chartFile.getField (string[1], 128);
//	chartFile.getField (string[2], 128);
//	chartFile.getField (string[3], 128);
//	CString strFootnote;
//	strFootnote.Format("%s %s %s %s",NoUnderline(CString(string[0])),string[1],string[2],string[3]);
//	SetHeadXYTitle("Max Queue Time Histogram(Summary)","Time of Max Queue","Processors",strFootnote);
//	//3.)Draw it
//	chartFile.reset();
//	chartFile.skipLines(3);
//	int xValue[8];
//	int yValue[8];
//	for(int i=0;i<8;i++)
//	{
//		xValue[i]=yValue[i]=0;
//	}
//	int maxXValue=0;
//	int minXValue=1440;// max minutes in 1 day
//	float ft = 0.0;   
//	ElapsedTime value;
//	while (chartFile.getLine())
//	{
//		chartFile.setToField (4);
//		chartFile.getTime (value, TRUE);
//		if (value.asMinutes() < minXValue)
//			minXValue = value.asMinutes();
//		if (value.asMinutes() > maxXValue)
//			maxXValue = value.asMinutes();
//	}
//	minXValue=minXValue-(minXValue%60);
//	maxXValue=maxXValue-(maxXValue%60)+60;
//	int nScale;//
//	int n=(maxXValue-minXValue)%420;//420=60*7;  7 Column
//	if(n==0)
//	{
//		nScale=(maxXValue-minXValue)/420;
//	}
//	else
//	{
//		nScale=(maxXValue-minXValue)/420+1;
//	}
//	
//	xValue[0]=minXValue;
//	for(i=1;i<=7;i++)
//	{
//		xValue[i]=xValue[i-1]+60*nScale;
//	}
//	
//	chartFile.reset();
//	chartFile.skipLines(3);
//	while (chartFile.getLine())
//	{
//		chartFile.setToField (4);
//		chartFile.getTime (value, TRUE);
//		for(i=1;i<=7;i++)
//		{
//			if(value.asMinutes()>=xValue[i-1] && value.asMinutes()<xValue[i])
//			{
//				yValue[i]++;
//			}
//		}
//	}
//	int nCount=7;
//	for(i=7;i>0;i--)
//	{
//		if(yValue[i]==0)
//		{
//			nCount--;
//		}
//		else
//		{
//			break;
//		}
//	}
//	m_MSChartCtrl.SetRowCount(nCount);
//	m_MSChartCtrl.SetColumnCount(1);
//	m_MSChartCtrl.SetColumnLabelCount(1);
//	m_MSChartCtrl.SetColumn(1);
//	m_MSChartCtrl.SetColumnLabel("Meters Travelled");
//	
//	CString strLabel;
//	for(i=1;i<=nCount;i++)
//	{
//		m_MSChartCtrl.SetRow(i);
//		strLabel.Format("%s-%s",FormatTime(xValue[i-1]),FormatTime(xValue[i]));
//		DealRowLabel(strLabel);
//		m_MSChartCtrl.SetRowLabel(strLabel);
//		
//		strLabel.Format("%d",yValue[i]);
//		m_MSChartCtrl.SetData(strLabel);
//	}
//	chartFile.closeIn();
//	m_MSChartCtrl.SetShowLegend(FALSE);
//}
//
//void CRepGraphView::AvgQueueLenTimeMaxQueSummary()
//{
//	// load report from file
//	CString sFileName;
//	if( GetLoadReportType() == load_by_user )
//	{
//		sFileName = GetUserLoadReportFile();
//	}
//	else								// load_by_system
//	{
//		sFileName = ((CTermPlanDoc*)GetDocument())->GetTerminal().GetSimReportManager()->GetCurrentReportFileName( ((CTermPlanDoc*)GetDocument())->m_ProjInfo.path );
//	}
//	chartFile.init (sFileName, READ);
//	//2.)Draw Title,X Axis Title,Y Axis Title,Footnote
//	chartFile.reset();
//	chartFile.getLine();
//	chartFile.getLine();
//	char string[4][128];
//	chartFile.getField (string[0], 128);
//	chartFile.getField (string[1], 128);
//	chartFile.getField (string[2], 128);
//	chartFile.getField (string[3], 128);
//	CString strFootnote;
//	strFootnote.Format("%s %s %s %s",NoUnderline(CString(string[0])),string[1],string[2],string[3]);
//	SetHeadXYTitle("Time of Max Queue(Summary)","Processor","Time of Max Queue(hours)",strFootnote);
//	//3.)Draw it
//	CString  strxValue[1440],stryValue[1440];
//	float xValue[1440];
//	CString strProcessor[1440];
//	int nCount=0;
//	ElapsedTime value;
//	int i=0;
//	char str[128];
//	chartFile.reset();
//	chartFile.skipLines(3);
//	while (chartFile.getLine())
//	{
//		nCount++;
//		chartFile.setToField (0);
//		chartFile.getField(str,128);
//		strProcessor[nCount].Format("%s",str);
//
//		chartFile.setToField (4);
//		chartFile.getTime(value);
//		xValue[nCount]=static_cast<float>(value.asHours());
//	}
//	m_MSChartCtrl.SetRowCount(nCount);
//	m_MSChartCtrl.SetColumnCount(1);
//	m_MSChartCtrl.SetColumnLabelCount(1);
//	m_MSChartCtrl.SetColumn(1);
//	m_MSChartCtrl.SetColumnLabel("Average");
//	
//	CString strLabel;
//	for(i=1;i<=nCount;i++)
//	{
//		m_MSChartCtrl.SetRow(i);
//		strLabel.Format("%s",strProcessor[i]);
//		DealRowLabel(strLabel);
//		m_MSChartCtrl.SetRowLabel(strLabel);
//		
//		strLabel.Format("%f",xValue[i]);
//		m_MSChartCtrl.SetData(strLabel);
//	}
//	chartFile.closeIn();
//	m_MSChartCtrl.SetShowLegend(FALSE);
//}
//
//void CRepGraphView::AvgQueueLenSummary()
//{
//	// load report from file
//	CString sFileName;
//	if( GetLoadReportType() == load_by_user )
//	{
//		sFileName = GetUserLoadReportFile();
//	}
//	else								// load_by_system
//	{
//		sFileName = ((CTermPlanDoc*)GetDocument())->GetTerminal().GetSimReportManager()->GetCurrentReportFileName( ((CTermPlanDoc*)GetDocument())->m_ProjInfo.path );
//	}
//	chartFile.init (sFileName, READ);
//	//2.)Draw Title,X Axis Title,Y Axis Title,Footnote
//	chartFile.reset();
//	chartFile.getLine();
//	chartFile.getLine();
//	char string[4][128];
//	chartFile.getField (string[0], 128);
//	chartFile.getField (string[1], 128);
//	chartFile.getField (string[2], 128);
//	chartFile.getField (string[3], 128);
//	CString strFootnote;
//	strFootnote.Format("%s %s %s %s",NoUnderline(CString(string[0])),string[1],string[2],string[3]);
//	SetHeadXYTitle("Queue Length(Summary)","Processor","Queue Length",strFootnote);
//	//3.)Draw it
//	CString  strxValue[1440],stryValue[1440];
//	CString strProcessor[1440];
//	int nCount=0;
//	float value;
//	int i=0;
//	char str[128];
//	chartFile.reset();
//	chartFile.skipLines(3);
//	while (chartFile.getLine())
//	{
//		nCount++;
//		chartFile.setToField (0);
//		chartFile.getField(str,128);
//		strProcessor[nCount].Format("%s",str);
//
//		chartFile.setToField (2);
//		chartFile.getFloat (value);
//		strxValue[nCount].Format("%f",value);
//		
//		chartFile.setToField (3);
//		chartFile.getFloat (value);
//		stryValue[nCount].Format("%f",value);
//	}
//	m_MSChartCtrl.SetRowCount(nCount);
//	m_MSChartCtrl.SetColumnCount(2);
//	m_MSChartCtrl.SetColumnLabelCount(2);
//	m_MSChartCtrl.SetColumn(1);
//	m_MSChartCtrl.SetColumnLabel("Average");
//	m_MSChartCtrl.SetColumn(2);
//	m_MSChartCtrl.SetColumnLabel("Maximum");
//	
//	CString strLabel;
//	for(i=1;i<=nCount;i++)
//	{
//		m_MSChartCtrl.SetRow(i);
//		strLabel.Format("%s",strProcessor[i]);
//		DealRowLabel(strLabel);
//		m_MSChartCtrl.SetRowLabel(strLabel);
//		
//		m_MSChartCtrl.SetColumn(1);
//		strLabel.Format("%s",strxValue[i]);
//		m_MSChartCtrl.SetData(strLabel);
//		m_MSChartCtrl.SetRow(i);	//add by bird 2003/2/18
//		m_MSChartCtrl.SetColumn(2);
//		strLabel.Format("%s",stryValue[i]);
//		m_MSChartCtrl.SetData(strLabel);
//	}
//	chartFile.closeIn();
//	m_MSChartCtrl.SetShowLegend(TRUE);
//}
//
//void CRepGraphView::AvgQueueLenDetailed()
//{
//	// load report from file
//	CString sFileName;
//	if( GetLoadReportType() == load_by_user )
//	{
//		sFileName = GetUserLoadReportFile();
//	}
//	else								// load_by_system
//	{
//		sFileName = ((CTermPlanDoc*)GetDocument())->GetTerminal().GetSimReportManager()->GetCurrentReportFileName( ((CTermPlanDoc*)GetDocument())->m_ProjInfo.path );
//	}
//	chartFile.init (sFileName, READ);
//	//2.)Draw Title,X Axis Title,Y Axis Title,Footnote
//	chartFile.reset();
//	chartFile.getLine();
//	chartFile.getLine();
//	char string[4][128];
//	chartFile.getField (string[0], 128);
//	chartFile.getField (string[1], 128);
//	chartFile.getField (string[2], 128);
//	chartFile.getField (string[3], 128);
//	CString strFootnote;
//	strFootnote.Format("%s %s %s %s",NoUnderline(CString(string[0])),string[1],string[2],string[3]);
//	SetHeadXYTitle("Queue Length(Detailed)","Processor","Queue Length",strFootnote);
//	//3.)Draw it
//	CString  strxValue[1440],stryValue[1440];
//	CString strProcessor[1440];
//	int nCount=0;
//	float value;
//	int i=0;
//	char str[128];
//	chartFile.reset();
//	chartFile.skipLines(3);
//	while (chartFile.getLine())
//	{
//		nCount++;
//		chartFile.setToField (0);
//		chartFile.getField(str,128);
//		strProcessor[nCount].Format("%s",str);
//
//		chartFile.setToField (1);
//		chartFile.getFloat (value);
//		strxValue[nCount].Format("%f",value);
//		
//		chartFile.setToField (2);
//		chartFile.getFloat (value);
//		stryValue[nCount].Format("%f",value);
//	}
//	m_MSChartCtrl.SetRowCount(nCount);
//	m_MSChartCtrl.SetColumnCount(2);
//	m_MSChartCtrl.SetColumnLabelCount(2);
//	m_MSChartCtrl.SetColumn(1);
//	m_MSChartCtrl.SetColumnLabel("Average");
//	m_MSChartCtrl.SetColumn(2);
//	m_MSChartCtrl.SetColumnLabel("Maximum");
//	
//	CString strLabel;
//	for(i=1;i<=nCount;i++)
//	{
//		m_MSChartCtrl.SetRow(i);
//		strLabel.Format("%s",strProcessor[i]);
//		DealRowLabel(strLabel);
//		m_MSChartCtrl.SetRowLabel(strLabel);
//		
//		m_MSChartCtrl.SetColumn(1);
//		strLabel.Format("%s",strxValue[i]);
//		m_MSChartCtrl.SetData(strLabel);
//		m_MSChartCtrl.SetRow(i);	//add by bird 2003/2/18
//		m_MSChartCtrl.SetColumn(2);
//		strLabel.Format("%s",stryValue[i]);
//		m_MSChartCtrl.SetData(strLabel);
//	}
//	chartFile.closeIn();
//	m_MSChartCtrl.SetShowLegend(TRUE);
//}
//
//void CRepGraphView::AvgQueueLenTimeMaxQueDetailed()
//{
//	// load report from file
//	CString sFileName;
//	if( GetLoadReportType() == load_by_user )
//	{
//		sFileName = GetUserLoadReportFile();
//	}
//	else								// load_by_system
//	{
//		sFileName = ((CTermPlanDoc*)GetDocument())->GetTerminal().GetSimReportManager()->GetCurrentReportFileName( ((CTermPlanDoc*)GetDocument())->m_ProjInfo.path );
//	}
//	chartFile.init (sFileName, READ);
//	//2.)Draw Title,X Axis Title,Y Axis Title,Footnote
//	chartFile.reset();
//	chartFile.getLine();
//	chartFile.getLine();
//	char string[4][128];
//	chartFile.getField (string[0], 128);
//	chartFile.getField (string[1], 128);
//	chartFile.getField (string[2], 128);
//	chartFile.getField (string[3], 128);
//	CString strFootnote;
//	strFootnote.Format("%s %s %s %s",NoUnderline(CString(string[0])),string[1],string[2],string[3]);
//	SetHeadXYTitle("Time of Max Queue(Detailed)","Processor","Time of Max Queue(hours)",strFootnote);
//	//3.)Draw it
//	CString  strxValue[1440],stryValue[1440];
//	float xValue[1440];
//	CString strProcessor[1440];
//	int nCount=0;
//	ElapsedTime value;
//	int i=0;
//	char str[128];
//	chartFile.reset();
//	chartFile.skipLines(3);
//	while (chartFile.getLine())
//	{
//		nCount++;
//		chartFile.setToField (0);
//		chartFile.getField(str,128);
//		strProcessor[nCount].Format("%s",str);
//
//		chartFile.setToField (3);
//		chartFile.getTime(value);
//		xValue[nCount]=static_cast<float>(value.asHours());
//	}
//	m_MSChartCtrl.SetRowCount(nCount);
//	m_MSChartCtrl.SetColumnCount(1);
//	m_MSChartCtrl.SetColumnLabelCount(1);
//	m_MSChartCtrl.SetColumn(1);
//	m_MSChartCtrl.SetColumnLabel("Average");
//	
//	CString strLabel;
//	for(i=1;i<=nCount;i++)
//	{
//		m_MSChartCtrl.SetRow(i);
//		strLabel.Format("%s",strProcessor[i]);
//		DealRowLabel(strLabel);
//		m_MSChartCtrl.SetRowLabel(strLabel);
//		
//		strLabel.Format("%f",xValue[i]);
//		m_MSChartCtrl.SetData(strLabel);
//	}
//	chartFile.closeIn();
//	m_MSChartCtrl.SetShowLegend(FALSE);
//}
//
//void CRepGraphView::AvgQueueLenMaxQueueTimeHisDetailed()
//{
//	// load report from file
//	CString sFileName;
//	if( GetLoadReportType() == load_by_user )
//	{
//		sFileName = GetUserLoadReportFile();
//	}
//	else								// load_by_system
//	{
//		sFileName = ((CTermPlanDoc*)GetDocument())->GetTerminal().GetSimReportManager()->GetCurrentReportFileName( ((CTermPlanDoc*)GetDocument())->m_ProjInfo.path );
//	}
//	chartFile.init (sFileName, READ);
////	if (chartFile.countLines() <= 5)
////		return;
//	//2.)Draw Title,X Axis Title,Y Axis Title,Footnote
//	chartFile.reset();
//	chartFile.getLine();
//	chartFile.getLine();
//	char string[4][128];
//	chartFile.getField (string[0], 128);
//	chartFile.getField (string[1], 128);
//	chartFile.getField (string[2], 128);
//	chartFile.getField (string[3], 128);
//	CString strFootnote;
//	strFootnote.Format("%s %s %s %s",NoUnderline(CString(string[0])),string[1],string[2],string[3]);
//	SetHeadXYTitle("Max Queue Time Histogram(Detailed)","Time of Max Queue","Processors",strFootnote);
//	//3.)Draw it
//	chartFile.reset();
//	chartFile.skipLines(3);
//	int xValue[8];
//	float yValue[8];
//	for(int i=0;i<8;i++)
//	{
//		xValue[i]=0; yValue[i]=0.0f;
//	}
//	int maxXValue=0;
//	int minXValue=1440;// max minutes in 1 day
//	float ft = 0.0;   
//	ElapsedTime value;
//	while (chartFile.getLine())
//	{
//		chartFile.setToField (3);
//		chartFile.getTime (value, TRUE);
//		if (value.asMinutes() < minXValue)
//			minXValue = value.asMinutes();
//		if (value.asMinutes() > maxXValue)
//			maxXValue = value.asMinutes();
//	}
//	minXValue=minXValue-(minXValue%60);
//	maxXValue=maxXValue-(maxXValue%60)+60;
//	int nScale;//
//	int n=(maxXValue-minXValue)%420;//420=60*7;  7 Column
//	if(n==0)
//	{
//		nScale=(maxXValue-minXValue)/420;
//	}
//	else
//	{
//		nScale=(maxXValue-minXValue)/420+1;
//	}
//	
//	xValue[0]=minXValue;
//	for(i=1;i<=7;i++)
//	{
//		xValue[i]=xValue[i-1]+60*nScale;
//	}
//	
//	chartFile.reset();
//	chartFile.skipLines(3);
//	while (chartFile.getLine())
//	{
//		chartFile.setToField (3);
//		chartFile.getTime (value, TRUE);
//		for(i=1;i<=7;i++)
//		{
//			if(value.asMinutes()>=xValue[i-1] && value.asMinutes()<xValue[i])
//			{
//				yValue[i]++;
//			}
//		}
//	}
//	int nCount=7;
//	for(i=7;i>0;i--)
//	{
//		if(yValue[i]==0)
//		{
//			nCount--;
//		}
//		else
//		{
//			break;
//		}
//	}
//	m_MSChartCtrl.SetRowCount(nCount);
//	m_MSChartCtrl.SetColumnCount(1);
//	m_MSChartCtrl.SetColumnLabelCount(1);
//	m_MSChartCtrl.SetColumn(1);
//	m_MSChartCtrl.SetColumnLabel("Meters Travelled");
//	
//	CString strLabel;
//	for(i=1;i<=nCount;i++)
//	{
//		m_MSChartCtrl.SetRow(i);
//		strLabel.Format("%s",/*FormatTime(xValue[i-1]),*/FormatTime(xValue[i]));
//		DealRowLabel(strLabel);
//		m_MSChartCtrl.SetRowLabel(strLabel);
//		
//		strLabel.Format("%f",yValue[i]);
//		m_MSChartCtrl.SetData(strLabel);
//	}
//	chartFile.closeIn();
//	m_MSChartCtrl.SetShowLegend(FALSE);
//}
//
//void CRepGraphView::DurationDetailed()
//{
//	// load report from file
//	CString sFileName;
//	if( GetLoadReportType() == load_by_user )
//	{
//		sFileName = GetUserLoadReportFile();
//	}
//	else								// load_by_system
//	{
//		sFileName = ((CTermPlanDoc*)GetDocument())->GetTerminal().GetSimReportManager()->GetCurrentReportFileName( ((CTermPlanDoc*)GetDocument())->m_ProjInfo.path );
//	}
//	chartFile.init (sFileName, READ);
////	if (chartFile.countLines() <= 5)
////		return;
//	//2.)Draw Title,X Axis Title,Y Axis Title,Footnote
//	chartFile.reset();
//	chartFile.getLine();
//	chartFile.getLine();
//	char string[4][128];
//	chartFile.getField (string[0], 128);
//	chartFile.getField (string[1], 128);
//	chartFile.getField (string[2], 128);
//	chartFile.getField (string[3], 128);
//	CString strFootnote;
//	strFootnote.Format("%s %s %s %s",NoUnderline(CString(string[0])),string[1],string[2],string[3]);
//	SetHeadXYTitle("Duration(Detailed)","Minutes in Terminal","Passenger Count",strFootnote);
//	//3.)Draw it
//	chartFile.reset();
//	chartFile.skipLines(3);
//	int xValue[8];
//	int yValue[8];
//	for(int i=0;i<8;i++)
//	{
//		xValue[i]=yValue[i]=0;
//	}
//	int maxXValue=0;
//	int minXValue=1440;// max minutes in 1 day
//	float ft = 0.0;   
//	ElapsedTime value;
//	while (chartFile.getLine())
//	{
//		chartFile.setToField (1);
//		chartFile.getTime (value, TRUE);
//		if (value.asMinutes() < minXValue)
//			minXValue = value.asMinutes();
//		if (value.asMinutes() > maxXValue)
//			maxXValue = value.asMinutes();
//	}
//	minXValue=minXValue-(minXValue%10);
//	maxXValue=maxXValue-(maxXValue%10)+10;
//	int nScale;//
//	int n=(maxXValue-minXValue)%70;//70=10*7;  7 Column
//	if(n==0)
//	{
//		nScale=(maxXValue-minXValue)/70;
//	}
//	else
//	{
//		nScale=(maxXValue-minXValue)/70+1;
//	}
//	
//	xValue[0]=minXValue;
//	for(i=1;i<=7;i++)
//	{
//		xValue[i]=xValue[i-1]+10*nScale;
//	}
//	
//	chartFile.reset();
//	chartFile.skipLines(3);
//	while (chartFile.getLine())
//	{
//		chartFile.setToField (1);
//		chartFile.getTime (value, TRUE);
//		for(i=1;i<=7;i++)
//		{
//			if(value.asMinutes()>=xValue[i-1] && value.asMinutes()<xValue[i])
//			{
//				yValue[i]++;
//			}
//		}
//	}
//	int nCount=7;
//	for(i=7;i>0;i--)
//	{
//		if(yValue[i]==0)
//		{
//			nCount--;
//		}
//		else
//		{
//			break;
//		}
//	}
//	m_MSChartCtrl.SetRowCount(nCount);
//	m_MSChartCtrl.SetColumnCount(1);
//	m_MSChartCtrl.SetColumnLabelCount(1);
//	m_MSChartCtrl.SetColumn(1);
//	m_MSChartCtrl.SetColumnLabel("Meters Travelled");
//	
//	CString strLabel;
//	for(i=1;i<=nCount;i++)
//	{
//		m_MSChartCtrl.SetRow(i);
//		strLabel.Format("%d-%d",(int)xValue[i-1],(int)xValue[i]);
//		DealRowLabel(strLabel);
//		m_MSChartCtrl.SetRowLabel(strLabel);
//		
//		strLabel.Format("%d",yValue[i]);
//		m_MSChartCtrl.SetData(strLabel);
//	}
//	chartFile.closeIn();
//	m_MSChartCtrl.SetShowLegend(FALSE);
//}
//
//void CRepGraphView::DurationSummary()
//{
//	// load report from file
//	CString sFileName;
//	if( GetLoadReportType() == load_by_user )
//	{
//		sFileName = GetUserLoadReportFile();
//	}
//	else								// load_by_system
//	{
//		sFileName = ((CTermPlanDoc*)GetDocument())->GetTerminal().GetSimReportManager()->GetCurrentReportFileName( ((CTermPlanDoc*)GetDocument())->m_ProjInfo.path );
//	}
//	chartFile.init (sFileName, READ);
//	//2.)Draw Title,X Axis Title,Y Axis Title,Footnote
//	chartFile.reset();
//	chartFile.getLine();
//	chartFile.getLine();
//	char string[4][128];
//	chartFile.getField (string[0], 128);
//	chartFile.getField (string[1], 128);
//	chartFile.getField (string[2], 128);
//	chartFile.getField (string[3], 128);
//	CString strFootnote;
//	strFootnote.Format("%s %s %s %s",NoUnderline(CString(string[0])),string[1],string[2],string[3]);
//	SetHeadXYTitle("Duration(Summary)","Passenger Type","Minutes in Terminal",strFootnote);
//	//3.)Draw it
//	CString strxValue[100];//The Pax Type Name
//	int nPaxCount=0;
//	ElapsedTime valuetime;
//	float minvalue[100];
//	float avevalue[100];
//	float maxvalue[100];
//
//	float Q1value[100];
//	float Q2value[100];
//	float Q3value[100];
//	
//	float P1value[100];
//	float P5value[100];
//	float P10value[100];
//	
//	float P90value[100];
//	float P95value[100];
//	float P99value[100];
//	
//	float Sigmavalue[100];
//	
//	char buf[128];
//	for(int k=0;k<100;k++)
//	{
//		minvalue[k]=avevalue[k]=maxvalue[k]=0;
//		Q1value[k]=Q2value[k]=Q3value[k]=P1value[k]=P5value[k]=0;
//		Sigmavalue[k] = P10value[k]=P95value[k]=P99value[k]=0;
//	}
//	chartFile.reset();
//	chartFile.skipLines(3);
//	while (chartFile.getLine())
//	{
//		chartFile.setToField(0);
//		chartFile.getField(buf,128);
//		nPaxCount+=1;
//		strxValue[nPaxCount].Format("%s",buf);//Get The First Pax Type
//		
//		chartFile.setToField (1);
//		chartFile.getTime (valuetime, TRUE);
//		minvalue[nPaxCount]=static_cast<float>(valuetime.asMinutes());
//		chartFile.setToField (2);
//		chartFile.getTime (valuetime, TRUE);
//		avevalue[nPaxCount]=static_cast<float>(valuetime.asMinutes());
//		chartFile.setToField (3);
//		chartFile.getTime (valuetime, TRUE);
//		maxvalue[nPaxCount]=static_cast<float>(valuetime.asMinutes());
//
//		
//		chartFile.setToField( 5 );
//		chartFile.getTime( valuetime, TRUE );
//		Q1value[nPaxCount]= static_cast<float>(valuetime.asMinutes());
//
//		chartFile.setToField( 6 );
//		chartFile.getTime( valuetime, TRUE );
//		Q2value[nPaxCount]= static_cast<float>(valuetime.asMinutes());
//
//		chartFile.setToField( 7 );
//		chartFile.getTime( valuetime, TRUE );
//		Q3value[nPaxCount]= static_cast<float>(valuetime.asMinutes());
//
//		chartFile.setToField( 8 );
//		chartFile.getTime( valuetime, TRUE );
//		P1value[nPaxCount]= static_cast<float>(valuetime.asMinutes());
//
//		chartFile.setToField( 9 );
//		chartFile.getTime( valuetime, TRUE );
//		P5value[nPaxCount]= static_cast<float>(valuetime.asMinutes());
//
//		chartFile.setToField( 10 );
//		chartFile.getTime( valuetime, TRUE );
//		P10value[nPaxCount]= static_cast<float>(valuetime.asMinutes());
//
//		chartFile.setToField( 11 );
//		chartFile.getTime( valuetime, TRUE );
//		P90value[nPaxCount]= static_cast<float>(valuetime.asMinutes());
//
//		chartFile.setToField( 12 );
//		chartFile.getTime( valuetime, TRUE );
//		P95value[nPaxCount]= static_cast<float>(valuetime.asMinutes());
//
//		chartFile.setToField( 13 );
//		chartFile.getTime( valuetime, TRUE );
//		P99value[nPaxCount]= static_cast<float>(valuetime.asMinutes());
//
//		chartFile.setToField( 14 );
//		chartFile.getTime( valuetime, TRUE );
//		Sigmavalue[nPaxCount]= static_cast<float>(valuetime.asMinutes());
//	}
//	
//	CString strLabel;
//	m_MSChartCtrl.SetRowCount(nPaxCount);
//	m_MSChartCtrl.SetColumnCount(13);
//	m_MSChartCtrl.SetColumnLabelCount(13);
//	m_MSChartCtrl.SetColumn(1);
//	m_MSChartCtrl.SetColumnLabel("Minimum");
//	m_MSChartCtrl.SetColumn(2);
//	m_MSChartCtrl.SetColumnLabel("Average");
//	m_MSChartCtrl.SetColumn(3);
//	m_MSChartCtrl.SetColumnLabel("Maximum");
//
//	m_MSChartCtrl.SetColumn(4);
//	m_MSChartCtrl.SetColumnLabel("Q1");
//	
//	m_MSChartCtrl.SetColumn(5);
//	m_MSChartCtrl.SetColumnLabel("Q2");
//	
//	m_MSChartCtrl.SetColumn(6);
//	m_MSChartCtrl.SetColumnLabel("Q3");
//	
//	m_MSChartCtrl.SetColumn(7);
//	m_MSChartCtrl.SetColumnLabel("P1");
//	
//	m_MSChartCtrl.SetColumn(8);
//	m_MSChartCtrl.SetColumnLabel("P5");
//	
//	m_MSChartCtrl.SetColumn(9);
//	m_MSChartCtrl.SetColumnLabel("P10");
//	
//	m_MSChartCtrl.SetColumn(10);
//	m_MSChartCtrl.SetColumnLabel("P90");
//	
//	m_MSChartCtrl.SetColumn(11);
//	m_MSChartCtrl.SetColumnLabel("P95");
//	
//	m_MSChartCtrl.SetColumn(12);
//	m_MSChartCtrl.SetColumnLabel("P99");
//	
//	m_MSChartCtrl.SetColumn(13);
//	m_MSChartCtrl.SetColumnLabel("Sigma");
//	
//
//	for(int i=1;i<=nPaxCount;i++)
//	{
//		m_MSChartCtrl.SetRow(i);
//		DealRowLabel(strxValue[i]);
//		m_MSChartCtrl.SetRowLabel(strxValue[i]);		
//		m_MSChartCtrl.SetColumn(1);
//		strLabel.Format("%f",minvalue[i]);
//		m_MSChartCtrl.SetData(strLabel);
//		m_MSChartCtrl.SetRow(i);	//add by bird 2003/2/18
//		m_MSChartCtrl.SetColumn(2);
//		strLabel.Format("%f",avevalue[i]);
//		m_MSChartCtrl.SetData(strLabel);
//		m_MSChartCtrl.SetRow(i);	//add by bird 2003/2/18
//		m_MSChartCtrl.SetColumn(3);
//		strLabel.Format("%f",maxvalue[i]);
//		m_MSChartCtrl.SetData(strLabel);
//
//		m_MSChartCtrl.SetRow(i);
//		m_MSChartCtrl.SetColumn(4);
//		strLabel.Format("%f",Q1value[i]);
//		m_MSChartCtrl.SetData(strLabel);
//
//		m_MSChartCtrl.SetRow(i);
//		m_MSChartCtrl.SetColumn(5);
//		strLabel.Format("%f",Q2value[i]);
//		m_MSChartCtrl.SetData(strLabel);
//
//		m_MSChartCtrl.SetRow(i);
//		m_MSChartCtrl.SetColumn(6);
//		strLabel.Format("%f",Q3value[i]);
//		m_MSChartCtrl.SetData(strLabel);
//
//		m_MSChartCtrl.SetRow(i);
//		m_MSChartCtrl.SetColumn(7);
//		strLabel.Format("%f",P1value[i]);
//		m_MSChartCtrl.SetData(strLabel);
//
//		m_MSChartCtrl.SetRow(i);
//		m_MSChartCtrl.SetColumn(8);
//		strLabel.Format("%f",P5value[i]);
//		m_MSChartCtrl.SetData(strLabel);
//
//		m_MSChartCtrl.SetRow(i);
//		m_MSChartCtrl.SetColumn(9);
//		strLabel.Format("%f",P10value[i]);
//		m_MSChartCtrl.SetData(strLabel);
//
//		m_MSChartCtrl.SetRow(i);
//		m_MSChartCtrl.SetColumn(10);
//		strLabel.Format("%f",P90value[i]);
//		m_MSChartCtrl.SetData(strLabel);
//
//		m_MSChartCtrl.SetRow(i);
//		m_MSChartCtrl.SetColumn(11);
//		strLabel.Format("%f",P95value[i]);
//		m_MSChartCtrl.SetData(strLabel);
//
//		m_MSChartCtrl.SetRow(i);
//		m_MSChartCtrl.SetColumn(12);
//		strLabel.Format("%f",P99value[i]);
//		m_MSChartCtrl.SetData(strLabel);
//
//		m_MSChartCtrl.SetRow(i);
//		m_MSChartCtrl.SetColumn(13);
//		strLabel.Format("%f",Sigmavalue[i]);
//		m_MSChartCtrl.SetData(strLabel);
//	}
//	
//	chartFile.closeIn();
//	m_MSChartCtrl.SetShowLegend(TRUE);
//}
//
//void CRepGraphView::ServiceTimeDetailed()
//{
//	// load report from file
//	CString sFileName;
//	if( GetLoadReportType() == load_by_user )
//	{
//		sFileName = GetUserLoadReportFile();
//	}
//	else								// load_by_system
//	{
//		sFileName = ((CTermPlanDoc*)GetDocument())->GetTerminal().GetSimReportManager()->GetCurrentReportFileName( ((CTermPlanDoc*)GetDocument())->m_ProjInfo.path );
//	}
//	chartFile.init (sFileName, READ);
//	//2.)Draw Title,X Axis Title,Y Axis Title,Footnote
//	chartFile.reset();
//	chartFile.getLine();
//	chartFile.getLine();
//	char string[4][128];
//	chartFile.getField (string[0], 128);
//	chartFile.getField (string[1], 128);
//	chartFile.getField (string[2], 128);
//	chartFile.getField (string[3], 128);
//	CString strFootnote;
//	strFootnote.Format("%s %s %s %s",NoUnderline(CString(string[0])),string[1],string[2],string[3]);
//	SetHeadXYTitle("Service Time(Detailed)","Minutes in Service","Passenger Count",strFootnote);
//	//3.)Draw it
//	chartFile.reset();
//	chartFile.skipLines(3);
//	int xValue[8];
//	int yValue[8];
//	for(int i=0;i<8;i++)
//	{
//		xValue[i]=yValue[i]=0;
//	}
//	int maxXValue=0;
//	int minXValue=1440;// max minutes in 1 day
//	float ft = 0.0;   
//	ElapsedTime value;
//	while (chartFile.getLine())
//	{
//		chartFile.setToField (1);
//		chartFile.getTime (value, TRUE);
//		if (value.asMinutes() < minXValue)
//			minXValue = value.asMinutes();
//		if (value.asMinutes() > maxXValue)
//			maxXValue = value.asMinutes();
//	}
//	minXValue=minXValue-(minXValue%10);
//	maxXValue=maxXValue-(maxXValue%10)+10;
//	int nScale;//
//	int n=(maxXValue-minXValue)%70;//70=10*7;  7 Column
//	if(n==0)
//	{
//		nScale=(maxXValue-minXValue)/70;
//	}
//	else
//	{
//		nScale=(maxXValue-minXValue)/70+1;
//	}
//	
//	xValue[0]=minXValue;
//	for(i=1;i<=7;i++)
//	{
//		xValue[i]=xValue[i-1]+10*nScale;
//	}
//	
//	chartFile.reset();
//	chartFile.skipLines(3);
//	while (chartFile.getLine())
//	{
//		chartFile.setToField (1);
//		chartFile.getTime (value, TRUE);
//		for(i=1;i<=7;i++)
//		{
//			if(value.asMinutes()>=xValue[i-1] && value.asMinutes()<xValue[i])
//			{
//				yValue[i]++;
//			}
//		}
//	}
//	int nCount=7;
//	for(i=7;i>0;i--)
//	{
//		if(yValue[i]==0)
//		{
//			nCount--;
//		}
//		else
//		{
//			break;
//		}
//	}
//	m_MSChartCtrl.SetRowCount(nCount);
//	m_MSChartCtrl.SetColumnCount(1);
//	m_MSChartCtrl.SetColumnLabelCount(1);
//	m_MSChartCtrl.SetColumn(1);
//	m_MSChartCtrl.SetColumnLabel("Meters Travelled");
//	
//	CString strLabel;
//	for(i=1;i<=nCount;i++)
//	{
//		m_MSChartCtrl.SetRow(i);
//		strLabel.Format("%d-%d",(int)xValue[i-1],(int)xValue[i]);
//		DealRowLabel(strLabel);
//		m_MSChartCtrl.SetRowLabel(strLabel);
//		
//		strLabel.Format("%d",yValue[i]);
//		m_MSChartCtrl.SetData(strLabel);
//	}
//	chartFile.closeIn();
//	m_MSChartCtrl.SetShowLegend(FALSE);
//}
//
//void CRepGraphView::ServiceTimeSummary()
//{
//	// load report from file
//	CString sFileName;
//	if( GetLoadReportType() == load_by_user )
//	{
//		sFileName = GetUserLoadReportFile();
//	}
//	else								// load_by_system
//	{
//		sFileName = ((CTermPlanDoc*)GetDocument())->GetTerminal().GetSimReportManager()->GetCurrentReportFileName( ((CTermPlanDoc*)GetDocument())->m_ProjInfo.path );
//	}
//	chartFile.init (sFileName, READ);
//	//2.)Draw Title,X Axis Title,Y Axis Title,Footnote
//	chartFile.reset();
//	chartFile.getLine();
//	chartFile.getLine();
//	char string[4][128];
//	chartFile.getField (string[0], 128);
//	chartFile.getField (string[1], 128);
//	chartFile.getField (string[2], 128);
//	chartFile.getField (string[3], 128);
//	CString strFootnote;
//	strFootnote.Format("%s %s %s %s",NoUnderline(CString(string[0])),string[1],string[2],string[3]);
//	SetHeadXYTitle("Service Time(Summary)","Passenger Type","Minutes in Service",strFootnote);
//	//3.)Draw it
//	CString strxValue[100];//The Pax Type Name
//	int nPaxCount=0;
//	ElapsedTime valuetime;
//	float minvalue[100];
//	float avevalue[100];
//	float maxvalue[100];
//
//	float Q1value[100];
//	float Q2value[100];
//	float Q3value[100];
//	
//	float P1value[100];
//	float P5value[100];
//	float P10value[100];
//	
//	float P90value[100];
//	float P95value[100];
//	float P99value[100];
//	
//	float Sigmavalue[100];
//	
//	char buf[128];
//	for(int k=0;k<100;k++)
//	{
//		minvalue[k]=avevalue[k]=maxvalue[k]=0;
//		Q1value[k]=Q2value[k]=Q3value[k]=P1value[k]=P5value[k]=0;
//		Sigmavalue[k] = P10value[k]=P95value[k]=P99value[k]=0;
//	}
//	chartFile.reset();
//	chartFile.skipLines(3);
//	while (chartFile.getLine())
//	{
//		chartFile.setToField(0);
//		chartFile.getField(buf,128);
//		nPaxCount+=1;
//		strxValue[nPaxCount].Format("%s",buf);//Get The First Pax Type
//		
//		chartFile.setToField (1);
//		chartFile.getTime (valuetime, TRUE);
//		minvalue[nPaxCount]=static_cast<float>(valuetime.asMinutes());
//		chartFile.setToField (2);
//		chartFile.getTime (valuetime, TRUE);
//		avevalue[nPaxCount]=static_cast<float>(valuetime.asMinutes());
//		chartFile.setToField (3);
//		chartFile.getTime (valuetime, TRUE);
//		maxvalue[nPaxCount]=static_cast<float>(valuetime.asMinutes());
//
//		chartFile.setToField( 5 );
//		chartFile.getTime( valuetime, TRUE );
//		Q1value[nPaxCount]= static_cast<float>(valuetime.asMinutes());
//		
//		chartFile.setToField( 6 );
//		chartFile.getTime( valuetime, TRUE );
//		Q2value[nPaxCount]= static_cast<float>(valuetime.asMinutes());
//		
//		chartFile.setToField( 7 );
//		chartFile.getTime( valuetime, TRUE );
//		Q3value[nPaxCount]= static_cast<float>(valuetime.asMinutes());
//		
//		chartFile.setToField( 8 );
//		chartFile.getTime( valuetime, TRUE );
//		P1value[nPaxCount]= static_cast<float>(valuetime.asMinutes());
//		
//		chartFile.setToField( 9 );
//		chartFile.getTime( valuetime, TRUE );
//		P5value[nPaxCount]= static_cast<float>(valuetime.asMinutes());
//		
//		chartFile.setToField( 10 );
//		chartFile.getTime( valuetime, TRUE );
//		P10value[nPaxCount]= static_cast<float>(valuetime.asMinutes());
//		
//		chartFile.setToField( 11 );
//		chartFile.getTime( valuetime, TRUE );
//		P90value[nPaxCount]= static_cast<float>(valuetime.asMinutes());
//		
//		chartFile.setToField( 12 );
//		chartFile.getTime( valuetime, TRUE );
//		P95value[nPaxCount]= static_cast<float>(valuetime.asMinutes());
//		
//		chartFile.setToField( 13 );
//		chartFile.getTime( valuetime, TRUE );
//		P99value[nPaxCount]= static_cast<float>(valuetime.asMinutes());
//		
//		chartFile.setToField( 14 );
//		chartFile.getTime( valuetime, TRUE );
//		Sigmavalue[nPaxCount]= static_cast<float>(valuetime.asMinutes());
//	}
//	
//	CString strLabel;
//	m_MSChartCtrl.SetRowCount(nPaxCount);
//	m_MSChartCtrl.SetColumnCount(13);
//	m_MSChartCtrl.SetColumnLabelCount(13);
//	m_MSChartCtrl.SetColumn(1);
//	m_MSChartCtrl.SetColumnLabel("Minimum");
//	m_MSChartCtrl.SetColumn(2);
//	m_MSChartCtrl.SetColumnLabel("Average");
//	m_MSChartCtrl.SetColumn(3);
//	m_MSChartCtrl.SetColumnLabel("Maximum");
//
//	m_MSChartCtrl.SetColumn(4);
//	m_MSChartCtrl.SetColumnLabel("Q1");
//	
//	m_MSChartCtrl.SetColumn(5);
//	m_MSChartCtrl.SetColumnLabel("Q2");
//	
//	m_MSChartCtrl.SetColumn(6);
//	m_MSChartCtrl.SetColumnLabel("Q3");
//	
//	m_MSChartCtrl.SetColumn(7);
//	m_MSChartCtrl.SetColumnLabel("P1");
//	
//	m_MSChartCtrl.SetColumn(8);
//	m_MSChartCtrl.SetColumnLabel("P5");
//	
//	m_MSChartCtrl.SetColumn(9);
//	m_MSChartCtrl.SetColumnLabel("P10");
//	
//	m_MSChartCtrl.SetColumn(10);
//	m_MSChartCtrl.SetColumnLabel("P90");
//	
//	m_MSChartCtrl.SetColumn(11);
//	m_MSChartCtrl.SetColumnLabel("P95");
//	
//	m_MSChartCtrl.SetColumn(12);
//	m_MSChartCtrl.SetColumnLabel("P99");
//	
//	m_MSChartCtrl.SetColumn(13);
//	m_MSChartCtrl.SetColumnLabel("Sigma");
//	
//	for(int i=1;i<=nPaxCount;i++)
//	{
//		m_MSChartCtrl.SetRow(i);
//		DealRowLabel(strxValue[i]);
//		m_MSChartCtrl.SetRowLabel(strxValue[i]);
//		m_MSChartCtrl.SetColumn(1);
//		strLabel.Format("%f",minvalue[i]);
//		m_MSChartCtrl.SetData(strLabel);
//		m_MSChartCtrl.SetRow(i);	//add by bird 2003/2/18
//		m_MSChartCtrl.SetColumn(2);
//		strLabel.Format("%f",avevalue[i]);
//		m_MSChartCtrl.SetData(strLabel);
//		m_MSChartCtrl.SetRow(i);	//add by bird 2003/2/18
//		m_MSChartCtrl.SetColumn(3);
//		strLabel.Format("%f",maxvalue[i]);
//		m_MSChartCtrl.SetData(strLabel);
//
//		m_MSChartCtrl.SetRow(i);
//		m_MSChartCtrl.SetColumn(4);
//		strLabel.Format("%f",Q1value[i]);
//		m_MSChartCtrl.SetData(strLabel);
//		
//		m_MSChartCtrl.SetRow(i);
//		m_MSChartCtrl.SetColumn(5);
//		strLabel.Format("%f",Q2value[i]);
//		m_MSChartCtrl.SetData(strLabel);
//		
//		m_MSChartCtrl.SetRow(i);
//		m_MSChartCtrl.SetColumn(6);
//		strLabel.Format("%f",Q3value[i]);
//		m_MSChartCtrl.SetData(strLabel);
//		
//		m_MSChartCtrl.SetRow(i);
//		m_MSChartCtrl.SetColumn(7);
//		strLabel.Format("%f",P1value[i]);
//		m_MSChartCtrl.SetData(strLabel);
//		
//		m_MSChartCtrl.SetRow(i);
//		m_MSChartCtrl.SetColumn(8);
//		strLabel.Format("%f",P5value[i]);
//		m_MSChartCtrl.SetData(strLabel);
//		
//		m_MSChartCtrl.SetRow(i);
//		m_MSChartCtrl.SetColumn(9);
//		strLabel.Format("%f",P10value[i]);
//		m_MSChartCtrl.SetData(strLabel);
//		
//		m_MSChartCtrl.SetRow(i);
//		m_MSChartCtrl.SetColumn(10);
//		strLabel.Format("%f",P90value[i]);
//		m_MSChartCtrl.SetData(strLabel);
//		
//		m_MSChartCtrl.SetRow(i);
//		m_MSChartCtrl.SetColumn(11);
//		strLabel.Format("%f",P95value[i]);
//		m_MSChartCtrl.SetData(strLabel);
//		
//		m_MSChartCtrl.SetRow(i);
//		m_MSChartCtrl.SetColumn(12);
//		strLabel.Format("%f",P99value[i]);
//		m_MSChartCtrl.SetData(strLabel);
//		
//		m_MSChartCtrl.SetRow(i);
//		m_MSChartCtrl.SetColumn(13);
//		strLabel.Format("%f",Sigmavalue[i]);
//		m_MSChartCtrl.SetData(strLabel);
//	}
//	
//	chartFile.closeIn();
//	m_MSChartCtrl.SetShowLegend(TRUE);
//}
//
//void CRepGraphView::ActBKDownTimeBKDetailed()
//{
//	// load report from file
//	CString sFileName;
//	if( GetLoadReportType() == load_by_user )
//	{
//		sFileName = GetUserLoadReportFile();
//	}
//	else								// load_by_system
//	{
//		sFileName = ((CTermPlanDoc*)GetDocument())->GetTerminal().GetSimReportManager()->GetCurrentReportFileName( ((CTermPlanDoc*)GetDocument())->m_ProjInfo.path );
//	}
//	chartFile.init (sFileName, READ);
//	//2.)Draw Title,X Axis Title,Y Axis Title,Footnote
//	chartFile.reset();
//	chartFile.getLine();
//	chartFile.getLine();
//	char string[4][128];
//	chartFile.getField (string[0], 128);
//	chartFile.getField (string[1], 128);
//	chartFile.getField (string[2], 128);
//	chartFile.getField (string[3], 128);
//	CString strFootnote;
//	strFootnote.Format("%s %s %s %s",NoUnderline(CString(string[0])),string[1],string[2],string[3]);
//	SetHeadXYTitle("Time Breakdown(Detailed)","Passenger ID","Minutes",strFootnote);
//	//3.)Draw it
//	CString strxValue[1000];//The Pax Type Name
//	int nPaxCount=0;
//	ElapsedTime valuetime;
////	float value;
//	float valueMov[1000];
//	float valueQue[1000];
//	float valueBag[1000];
//	float valueHold[1000];
//	float valueSer[1000];
//	int nMaxCount=10;
//	int nCount=0;
//	char buf[128];
//	for(int k=0;k<1000;k++)
//	{
//		valueMov[k]=valueQue[k]=valueBag[k]=valueHold[k]=valueSer[k]=0;
//	}
//	chartFile.reset();
//	chartFile.skipLines(3);
//	while (chartFile.getLine())
//	{
//		
//		if(nPaxCount>=nMaxCount)
//		{
//			break;
//		}
//		nPaxCount++;	
//		chartFile.setToField(0);
//		chartFile.getField(buf,128);
//		strxValue[nPaxCount].Format("%s",buf);//Get The First Pax Type
//		
//		chartFile.setToField (1);
//		chartFile.getTime (valuetime, TRUE);
//		valueMov[nPaxCount]=static_cast<float>(valuetime.asMinutes());
//		chartFile.setToField (2);
//		chartFile.getTime (valuetime, TRUE);
//		valueQue[nPaxCount]=static_cast<float>(valuetime.asMinutes());
//		chartFile.setToField (3);
//		chartFile.getTime (valuetime, TRUE);
//		valueBag[nPaxCount]=static_cast<float>(valuetime.asMinutes());
//		chartFile.setToField (4);
//		chartFile.getTime (valuetime, TRUE);
//		valueHold[nPaxCount]=static_cast<float>(valuetime.asMinutes());
//		chartFile.setToField (5);
//		chartFile.getTime (valuetime, TRUE);
//		valueSer[nPaxCount]=static_cast<float>(valuetime.asMinutes());
//	}
//	
//	CString strLabel;
//	m_MSChartCtrl.SetRowCount(nPaxCount);
//	m_MSChartCtrl.SetColumnCount(5);
//	m_MSChartCtrl.SetColumnLabelCount(5);
//	m_MSChartCtrl.SetColumn(1);
//	m_MSChartCtrl.SetColumnLabel("Movement Time");
//	m_MSChartCtrl.SetColumn(2);
//	m_MSChartCtrl.SetColumnLabel("Queuing Time");
//	m_MSChartCtrl.SetColumn(3);
//	m_MSChartCtrl.SetColumnLabel("Bag Wait Time");
//	m_MSChartCtrl.SetColumn(4);
//	m_MSChartCtrl.SetColumnLabel("Time in Wait Areas");
//	m_MSChartCtrl.SetColumn(5);
//	m_MSChartCtrl.SetColumnLabel("Service Time");
//
//	for(int i=1;i<=nPaxCount;i++)
//	{
//		m_MSChartCtrl.SetRow(i);
//		DealRowLabel(strxValue[i]);
//		m_MSChartCtrl.SetRowLabel(strxValue[i]);
//		m_MSChartCtrl.SetColumn(1);
//		strLabel.Format("%f",valueMov[i]);
//		m_MSChartCtrl.SetData(strLabel);
//		m_MSChartCtrl.SetRow(i);	//add by bird 2003/2/18
//		m_MSChartCtrl.SetColumn(2);
//		strLabel.Format("%f",valueQue[i]);
//		m_MSChartCtrl.SetData(strLabel);
//		m_MSChartCtrl.SetRow(i);	//add by bird 2003/2/18
//		m_MSChartCtrl.SetColumn(3);
//		strLabel.Format("%f",valueBag[i]);
//		m_MSChartCtrl.SetData(strLabel);
//		m_MSChartCtrl.SetRow(i);	//add by bird 2003/2/18
//		m_MSChartCtrl.SetColumn(4);
//		strLabel.Format("%f",valueHold[i]);
//		m_MSChartCtrl.SetData(strLabel);
//		m_MSChartCtrl.SetRow(i);	//add by bird 2003/2/18
//		m_MSChartCtrl.SetColumn(5);
//		strLabel.Format("%f",valueSer[i]);
//		m_MSChartCtrl.SetData(strLabel);
//	}
//	chartFile.closeIn();
//	m_MSChartCtrl.SetShowLegend(TRUE);
//}
//void CRepGraphView::ActBKDownPerBKDetailed()
//{
//	// load report from file
//	CString sFileName;
//	if( GetLoadReportType() == load_by_user )
//	{
//		sFileName = GetUserLoadReportFile();
//	}
//	else								// load_by_system
//	{
//		sFileName = ((CTermPlanDoc*)GetDocument())->GetTerminal().GetSimReportManager()->GetCurrentReportFileName( ((CTermPlanDoc*)GetDocument())->m_ProjInfo.path );
//	}
//	chartFile.init (sFileName, READ);
//	//2.)Draw Title,X Axis Title,Y Axis Title,Footnote
//	chartFile.reset();
//	chartFile.getLine();
//	chartFile.getLine();
//	char string[4][128];
//	chartFile.getField (string[0], 128);
//	chartFile.getField (string[1], 128);
//	chartFile.getField (string[2], 128);
//	chartFile.getField (string[3], 128);
//	CString strFootnote;
//	strFootnote.Format("%s %s %s %s",NoUnderline(CString(string[0])),string[1],string[2],string[3]);
//	SetHeadXYTitle("Percentage Breakdown(Detailed)","Passenger ID","Percentage(100%)",strFootnote);
//	//3.)Draw it
//	CString strxValue[1000];//The Pax Type Name
//	int nPaxCount=0;
//	ElapsedTime valuetime;
////	float value;
//	float valueMov[1000];
//	float valueQue[1000];
//	float valueBag[1000];
//	float valueHold[1000];
//	float valueSer[1000];
//	float valueTotal[1000];
//	int nMaxCount=10;
//	int nCount=0;
//	char buf[128];
//	for(int k=0;k<1000;k++)
//	{
//		valueMov[k]=valueQue[k]=valueBag[k]=valueHold[k]=valueSer[k]=valueTotal[k]=0;
//	}
//	chartFile.reset();
//	chartFile.skipLines(3);
//	while (chartFile.getLine())
//	{
//		
//		if(nPaxCount>=nMaxCount)
//		{
//			break;
//		}
//		nPaxCount++;	
//		chartFile.setToField(0);
//		chartFile.getField(buf,128);
//		strxValue[nPaxCount].Format("%s",buf);//Get The First Pax Type
//		
//		chartFile.setToField (1);
//		chartFile.getTime (valuetime, TRUE);
//		valueMov[nPaxCount]=static_cast<float>(valuetime.asMinutes());
//		chartFile.setToField (2);
//		chartFile.getTime (valuetime, TRUE);
//		valueQue[nPaxCount]=static_cast<float>(valuetime.asMinutes());
//		chartFile.setToField (3);
//		chartFile.getTime (valuetime, TRUE);
//		valueBag[nPaxCount]=static_cast<float>(valuetime.asMinutes());
//		chartFile.setToField (4);
//		chartFile.getTime (valuetime, TRUE);
//		valueHold[nPaxCount]=static_cast<float>(valuetime.asMinutes());
//		chartFile.setToField (5);
//		chartFile.getTime (valuetime, TRUE);
//		valueSer[nPaxCount]=static_cast<float>(valuetime.asMinutes());
//
//		valueTotal[nPaxCount]=valueMov[nPaxCount]+valueQue[nPaxCount]+valueBag[nPaxCount]+valueHold[nPaxCount]+valueSer[nPaxCount];
//	}
//	
//	CString strLabel;
//	m_MSChartCtrl.SetRowCount(nPaxCount);
//	m_MSChartCtrl.SetColumnCount(5);
//	m_MSChartCtrl.SetColumnLabelCount(5);
//	m_MSChartCtrl.SetColumn(1);
//	m_MSChartCtrl.SetColumnLabel("Movement Time");
//	m_MSChartCtrl.SetColumn(2);
//	m_MSChartCtrl.SetColumnLabel("Queuing Time");
//	m_MSChartCtrl.SetColumn(3);
//	m_MSChartCtrl.SetColumnLabel("Bag Wait Time");
//	m_MSChartCtrl.SetColumn(4);
//	m_MSChartCtrl.SetColumnLabel("Time in Wait Areas");
//	m_MSChartCtrl.SetColumn(5);
//	m_MSChartCtrl.SetColumnLabel("Service Time");
//
//	for(int i=1;i<=nPaxCount;i++)
//	{
//		m_MSChartCtrl.SetRow(i);
//		DealRowLabel(strxValue[i]);
//		m_MSChartCtrl.SetRowLabel(strxValue[i]);		
//		if(valueTotal[i]!=0)
//		{
//			m_MSChartCtrl.SetColumn(1);
//			strLabel.Format("%f",valueMov[i]/valueTotal[i]);
//			m_MSChartCtrl.SetData(strLabel);
//			m_MSChartCtrl.SetRow(i);	//add by bird 2003/2/18
//			m_MSChartCtrl.SetColumn(2);
//			strLabel.Format("%f",valueQue[i]/valueTotal[i]);
//			m_MSChartCtrl.SetData(strLabel);
//			m_MSChartCtrl.SetRow(i);	//add by bird 2003/2/18
//			m_MSChartCtrl.SetColumn(3);
//			strLabel.Format("%f",valueBag[i]/valueTotal[i]);
//			m_MSChartCtrl.SetData(strLabel);
//			m_MSChartCtrl.SetRow(i);	//add by bird 2003/2/18
//			m_MSChartCtrl.SetColumn(4);
//			strLabel.Format("%f",valueHold[i]/valueTotal[i]);
//			m_MSChartCtrl.SetData(strLabel);
//			m_MSChartCtrl.SetRow(i);	//add by bird 2003/2/18
//			m_MSChartCtrl.SetColumn(5);
//			strLabel.Format("%f",valueSer[i]/valueTotal[i]);
//			m_MSChartCtrl.SetData(strLabel);
//			// TRACE("%s,%f,%f,%f,%f,%f\n",strxValue[i],valueMov[i]/valueTotal[i],valueQue[i]/valueTotal[i],valueBag[i]/valueTotal[i],valueHold[i]/valueTotal[i],valueSer[i]/valueTotal[i]);
//		}
//	}
//	chartFile.closeIn();
//	m_MSChartCtrl.SetShowLegend(TRUE);
//}
//void CRepGraphView::ActBKDownTimeBKSummary()
//{
//	// load report from file
//	CString sFileName;
//	if( GetLoadReportType() == load_by_user )
//	{
//		sFileName = GetUserLoadReportFile();
//	}
//	else								// load_by_system
//	{
//		sFileName = ((CTermPlanDoc*)GetDocument())->GetTerminal().GetSimReportManager()->GetCurrentReportFileName( ((CTermPlanDoc*)GetDocument())->m_ProjInfo.path );
//	}
//	chartFile.init (sFileName, READ);
//	//2.)Draw Title,X Axis Title,Y Axis Title,Footnote
//	chartFile.reset();
//	chartFile.getLine();
//	chartFile.getLine();
//	char string[4][128];
//	chartFile.getField (string[0], 128);
//	chartFile.getField (string[1], 128);
//	chartFile.getField (string[2], 128);
//	chartFile.getField (string[3], 128);
//	CString strFootnote;
//	strFootnote.Format("%s %s %s %s",NoUnderline(CString(string[0])),string[1],string[2],string[3]);
//	SetHeadXYTitle("Time Breakdown(Summary)","Passenger Type","Minutes",strFootnote);
//	//3.)Draw it
//	CString strxValue[1000];//The Pax Type Name
//	int nPaxCount=0;
//	ElapsedTime valuetime;
//	float valueMov[1000];
//	float valueQue[1000];
//	float valueBag[1000];
//	float valueHold[1000];
//	float valueSer[1000];
//	char buf[128];
//	for(int k=0;k<1000;k++)
//	{
//		valueMov[k]=valueQue[k]=valueBag[k]=valueHold[k]=valueSer[k]=0;
//	}
//	chartFile.reset();
//	chartFile.skipLines(3);
//	while (chartFile.getLine())
//	{
//		chartFile.setToField(0);
//		chartFile.getField(buf,128);
//		nPaxCount+=1;
//		strxValue[nPaxCount].Format("%s",buf);//Get The First Pax Type
//		
//		chartFile.setToField (1);
//		chartFile.getTime (valuetime, TRUE);
//		valueMov[nPaxCount]=static_cast<float>(valuetime.asMinutes());
//		chartFile.setToField (2);
//		chartFile.getTime (valuetime, TRUE);
//		valueQue[nPaxCount]=static_cast<float>(valuetime.asMinutes());
//		chartFile.setToField (3);
//		chartFile.getTime (valuetime, TRUE);
//		valueBag[nPaxCount]=static_cast<float>(valuetime.asMinutes());
//		chartFile.setToField (4);
//		chartFile.getTime (valuetime, TRUE);
//		valueHold[nPaxCount]=static_cast<float>(valuetime.asMinutes());
//		chartFile.setToField (5);
//		chartFile.getTime (valuetime, TRUE);
//		valueSer[nPaxCount]=static_cast<float>(valuetime.asMinutes());
//	}
//	
//	CString strLabel;
//	m_MSChartCtrl.SetRowCount(nPaxCount);
//	m_MSChartCtrl.SetColumnCount(5);
//	m_MSChartCtrl.SetColumnLabelCount(5);
//	m_MSChartCtrl.SetColumn(1);
//	m_MSChartCtrl.SetColumnLabel("Movement Time");
//	m_MSChartCtrl.SetColumn(2);
//	m_MSChartCtrl.SetColumnLabel("Queuing Time");
//	m_MSChartCtrl.SetColumn(3);
//	m_MSChartCtrl.SetColumnLabel("Bag Wait Time");
//	m_MSChartCtrl.SetColumn(4);
//	m_MSChartCtrl.SetColumnLabel("Time in Wait Areas");
//	m_MSChartCtrl.SetColumn(5);
//	m_MSChartCtrl.SetColumnLabel("Service Time");
//
//	for(int i=1;i<=nPaxCount;i++)
//	{
//		m_MSChartCtrl.SetRow(i);
//		DealRowLabel(strxValue[i]);
//		m_MSChartCtrl.SetRowLabel(strxValue[i]);
//		m_MSChartCtrl.SetColumn(1);
//		strLabel.Format("%f",valueMov[i]);
//		m_MSChartCtrl.SetData(strLabel);
//		m_MSChartCtrl.SetRow(i);	//add by bird 2003/2/18
//		m_MSChartCtrl.SetColumn(2);
//		strLabel.Format("%f",valueQue[i]);
//		m_MSChartCtrl.SetData(strLabel);
//		m_MSChartCtrl.SetRow(i);	//add by bird 2003/2/18
//		m_MSChartCtrl.SetColumn(3);
//		strLabel.Format("%f",valueBag[i]);
//		m_MSChartCtrl.SetData(strLabel);
//		m_MSChartCtrl.SetRow(i);	//add by bird 2003/2/18
//		m_MSChartCtrl.SetColumn(4);
//		strLabel.Format("%f",valueHold[i]);
//		m_MSChartCtrl.SetData(strLabel);
//		m_MSChartCtrl.SetRow(i);	//add by bird 2003/2/18
//		m_MSChartCtrl.SetColumn(5);
//		strLabel.Format("%f",valueSer[i]);
//		m_MSChartCtrl.SetData(strLabel);
//	}
//	chartFile.closeIn();
//	m_MSChartCtrl.SetShowLegend(TRUE);
//}
//void CRepGraphView::ActBKDownPerBKSummary()
//{
//	// load report from file
//	CString sFileName;
//	if( GetLoadReportType() == load_by_user )
//	{
//		sFileName = GetUserLoadReportFile();
//	}
//	else								// load_by_system
//	{
//		sFileName = ((CTermPlanDoc*)GetDocument())->GetTerminal().GetSimReportManager()->GetCurrentReportFileName( ((CTermPlanDoc*)GetDocument())->m_ProjInfo.path );
//	}
//	chartFile.init (sFileName, READ);
//	//2.)Draw Title,X Axis Title,Y Axis Title,Footnote
//	chartFile.reset();
//	chartFile.getLine();
//	chartFile.getLine();
//	char string[4][128];
//	chartFile.getField (string[0], 128);
//	chartFile.getField (string[1], 128);
//	chartFile.getField (string[2], 128);
//	chartFile.getField (string[3], 128);
//	CString strFootnote;
//	strFootnote.Format("%s %s %s %s",NoUnderline(CString(string[0])),string[1],string[2],string[3]);
//	SetHeadXYTitle("Time Breakdown(Summary)","Passenger Type","Percentage(100%)",strFootnote);
//	//3.)Draw it
//	CString strxValue[1000];//The Pax Type Name
//	int nPaxCount=0;
//	ElapsedTime valuetime;
//	float valueMov[1000];
//	float valueQue[1000];
//	float valueBag[1000];
//	float valueHold[1000];
//	float valueSer[1000];
//	float valueTotal[1000];
//	char buf[128];
//	for(int k=0;k<1000;k++)
//	{
//		valueMov[k]=valueQue[k]=valueBag[k]=valueHold[k]=valueSer[k]=valueTotal[k]=0;
//	}
//	chartFile.reset();
//	chartFile.skipLines(3);
//	while (chartFile.getLine())
//	{
//		chartFile.setToField(0);
//		chartFile.getField(buf,128);
//		nPaxCount+=1;
//		strxValue[nPaxCount].Format("%s",buf);//Get The First Pax Type
//		
//		chartFile.setToField (1);
//		chartFile.getTime (valuetime, TRUE);
//		valueMov[nPaxCount]=static_cast<float>(valuetime.asMinutes());
//		chartFile.setToField (2);
//		chartFile.getTime (valuetime, TRUE);
//		valueQue[nPaxCount]=static_cast<float>(valuetime.asMinutes());
//		chartFile.setToField (3);
//		chartFile.getTime (valuetime, TRUE);
//		valueBag[nPaxCount]=static_cast<float>(valuetime.asMinutes());
//		chartFile.setToField (4);
//		chartFile.getTime (valuetime, TRUE);
//		valueHold[nPaxCount]=static_cast<float>(valuetime.asMinutes());
//		chartFile.setToField (5);
//		chartFile.getTime (valuetime, TRUE);
//		valueSer[nPaxCount]=static_cast<float>(valuetime.asMinutes());
//
//		valueTotal[nPaxCount]=valueMov[nPaxCount]+valueQue[nPaxCount]+valueBag[nPaxCount]+valueHold[nPaxCount]+valueSer[nPaxCount];
//	}
//	
//	CString strLabel;
//	m_MSChartCtrl.SetRowCount(nPaxCount);
//	m_MSChartCtrl.SetColumnCount(5);
//	m_MSChartCtrl.SetColumnLabelCount(5);
//	m_MSChartCtrl.SetColumn(1);
//	m_MSChartCtrl.SetColumnLabel("Movement Time");
//	m_MSChartCtrl.SetColumn(2);
//	m_MSChartCtrl.SetColumnLabel("Queuing Time");
//	m_MSChartCtrl.SetColumn(3);
//	m_MSChartCtrl.SetColumnLabel("Bag Wait Time");
//	m_MSChartCtrl.SetColumn(4);
//	m_MSChartCtrl.SetColumnLabel("Time in Wait Areas");
//	m_MSChartCtrl.SetColumn(5);
//	m_MSChartCtrl.SetColumnLabel("Service Time");
//
//	for(int i=1;i<=nPaxCount;i++)
//	{
//		m_MSChartCtrl.SetRow(i);
//		DealRowLabel(strxValue[i]);
//		m_MSChartCtrl.SetRowLabel(strxValue[i]);
//		m_MSChartCtrl.SetColumn(1);
//		if(valueTotal[i]!=0)
//		{
//			strLabel.Format("%f",valueMov[i]/valueTotal[i]);
//			m_MSChartCtrl.SetData(strLabel);
//			m_MSChartCtrl.SetRow(i);	//add by bird 2003/2/18
//			m_MSChartCtrl.SetColumn(2);
//			strLabel.Format("%f",valueQue[i]/valueTotal[i]);
//			m_MSChartCtrl.SetData(strLabel);
//			m_MSChartCtrl.SetRow(i);	//add by bird 2003/2/18
//			m_MSChartCtrl.SetColumn(3);
//			strLabel.Format("%f",valueBag[i]/valueTotal[i]);
//			m_MSChartCtrl.SetData(strLabel);
//			m_MSChartCtrl.SetRow(i);	//add by bird 2003/2/18
//			m_MSChartCtrl.SetColumn(4);
//			strLabel.Format("%f",valueHold[i]/valueTotal[i]);
//			m_MSChartCtrl.SetData(strLabel);
//			m_MSChartCtrl.SetRow(i);	//add by bird 2003/2/18
//			m_MSChartCtrl.SetColumn(5);
//			strLabel.Format("%f",valueSer[i]/valueTotal[i]);
//			m_MSChartCtrl.SetData(strLabel);
//		}
//	}
//	chartFile.closeIn();
//	m_MSChartCtrl.SetShowLegend(TRUE);
//}
//
//void CRepGraphView::BagWaitTimeDetailed()
//{
//	// load report from file
//	CString sFileName;
//	if( GetLoadReportType() == load_by_user )
//	{
//		sFileName = GetUserLoadReportFile();
//	}
//	else								// load_by_system
//	{
//		sFileName = ((CTermPlanDoc*)GetDocument())->GetTerminal().GetSimReportManager()->GetCurrentReportFileName( ((CTermPlanDoc*)GetDocument())->m_ProjInfo.path );
//	}
//	chartFile.init (sFileName, READ);
////	if (chartFile.countLines() <= 5)
////		return;
//	//2.)Draw Title,X Axis Title,Y Axis Title,Footnote
//	chartFile.reset();
//	chartFile.getLine();
//	chartFile.getLine();
//	char string[4][128];
//	chartFile.getField (string[0], 128);
//	chartFile.getField (string[1], 128);
//	chartFile.getField (string[2], 128);
//	chartFile.getField (string[3], 128);
//	CString strFootnote;
//	strFootnote.Format("%s %s %s %s",NoUnderline(CString(string[0])),string[1],string[2],string[3]);
//	SetHeadXYTitle("Baggage Wait Time(Detailed)","Minutes Wait","Passenger Count",strFootnote);
//	//3.)Draw it
//	chartFile.reset();
//	chartFile.skipLines(3);
//	int xValue[8];
//	int yValue[8];
//	for(int i=0;i<8;i++)
//	{
//		xValue[i]=yValue[i]=0;
//	}
//	int maxXValue=0;
//	int minXValue=1440;// max minutes in 1 day
//	float ft = 0.0;   
//	ElapsedTime value;
//	while (chartFile.getLine())
//	{
//		chartFile.setToField (2);
//		chartFile.getTime (value, TRUE);
//		if (value.asMinutes() < minXValue)
//			minXValue = value.asMinutes();
//		if (value.asMinutes() > maxXValue)
//			maxXValue = value.asMinutes();
//	}
//	minXValue=minXValue-(minXValue%10);
//	maxXValue=maxXValue-(maxXValue%10)+10;
//	int nScale;//
//	int n=(maxXValue-minXValue)%70;//70=10*7;  7 Column
//	if(n==0)
//	{
//		nScale=(maxXValue-minXValue)/70;
//	}
//	else
//	{
//		nScale=(maxXValue-minXValue)/70+1;
//	}
//	
//	xValue[0]=minXValue;
//	for(i=1;i<=7;i++)
//	{
//		xValue[i]=xValue[i-1]+10*nScale;
//	}
//	
//	chartFile.reset();
//	chartFile.skipLines(3);
//	while (chartFile.getLine())
//	{
//		chartFile.setToField (2);
//		chartFile.getTime (value, TRUE);
//		for(i=1;i<=7;i++)
//		{
//			if(value.asMinutes()>=xValue[i-1] && value.asMinutes()<xValue[i])
//			{
//				yValue[i]++;
//			}
//		}
//	}
//	int nCount=7;
//	for(i=7;i>0;i--)
//	{
//		if(yValue[i]==0)
//		{
//			nCount--;
//		}
//		else
//		{
//			break;
//		}
//	}
//	m_MSChartCtrl.SetRowCount(nCount);
//	m_MSChartCtrl.SetColumnCount(1);
//	m_MSChartCtrl.SetColumnLabelCount(1);
//	m_MSChartCtrl.SetColumn(1);
//	m_MSChartCtrl.SetColumnLabel("Meters Travelled");
//	
//	CString strLabel;
//	for(i=1;i<=nCount;i++)
//	{
//		m_MSChartCtrl.SetRow(i);
//		strLabel.Format("%d-%d",(int)xValue[i-1],(int)xValue[i]);
//		DealRowLabel(strLabel);
//		m_MSChartCtrl.SetRowLabel(strLabel);
//		
//		strLabel.Format("%d",yValue[i]);
//		m_MSChartCtrl.SetData(strLabel);
//	}
//	chartFile.closeIn();
//	m_MSChartCtrl.SetShowLegend(FALSE);
//}
//
//void CRepGraphView::BagWaitTimeSummary()
//{
//	// load report from file
//	CString sFileName;
//	if( GetLoadReportType() == load_by_user )
//	{
//		sFileName = GetUserLoadReportFile();
//	}
//	else								// load_by_system
//	{
//		sFileName = ((CTermPlanDoc*)GetDocument())->GetTerminal().GetSimReportManager()->GetCurrentReportFileName( ((CTermPlanDoc*)GetDocument())->m_ProjInfo.path );
//	}
//	chartFile.init (sFileName, READ);
//	//2.)Draw Title,X Axis Title,Y Axis Title,Footnote
//	chartFile.reset();
//	chartFile.getLine();
//	chartFile.getLine();
//	char string[4][128];
//	chartFile.getField (string[0], 128);
//	chartFile.getField (string[1], 128);
//	chartFile.getField (string[2], 128);
//	chartFile.getField (string[3], 128);
//	CString strFootnote;
//	strFootnote.Format("%s %s %s %s",NoUnderline(CString(string[0])),string[1],string[2],string[3]);
//	SetHeadXYTitle("Service Time(Summary)","Passenger Type","Minutes Wait",strFootnote);
//	//3.)Draw it
//	CString strxValue[100];//The Pax Type Name
//	int nPaxCount=0;
//	ElapsedTime valuetime;
//	float minvalue[100];
//	float avevalue[100];
//	float maxvalue[100];
//
//	float Q1value[100];
//	float Q2value[100];
//	float Q3value[100];
//	
//	float P1value[100];
//	float P5value[100];
//	float P10value[100];
//	
//	float P90value[100];
//	float P95value[100];
//	float P99value[100];
//	
//	float Sigmavalue[100];
//	
//	char buf[128];
//	for(int k=0;k<100;k++)
//	{
//		minvalue[k]=avevalue[k]=maxvalue[k]=0;
//		Q1value[k]=Q2value[k]=Q3value[k]=P1value[k]=P5value[k]=0;
//		Sigmavalue[k] = P10value[k]=P95value[k]=P99value[k]=0;
//	}
//
//	chartFile.reset();
//	chartFile.skipLines(3);
//	while (chartFile.getLine())
//	{
//		chartFile.setToField(0);
//		chartFile.getField(buf,128);
//		nPaxCount+=1;
//		strxValue[nPaxCount].Format("%s",buf);//Get The First Pax Type
//		
//		chartFile.setToField (1);
//		chartFile.getTime (valuetime, TRUE);
//		minvalue[nPaxCount]=static_cast<float>(valuetime.asMinutes());
//		chartFile.setToField (2);
//		chartFile.getTime (valuetime, TRUE);
//		avevalue[nPaxCount]=static_cast<float>(valuetime.asMinutes());
//		chartFile.setToField (3);
//		chartFile.getTime (valuetime, TRUE);
//		maxvalue[nPaxCount]=static_cast<float>(valuetime.asMinutes());
//
//		chartFile.setToField( 5 );
//		chartFile.getTime( valuetime, TRUE );
//		Q1value[nPaxCount]= static_cast<float>(valuetime.asMinutes());
//		
//		chartFile.setToField( 6 );
//		chartFile.getTime( valuetime, TRUE );
//		Q2value[nPaxCount]= static_cast<float>(valuetime.asMinutes());
//		
//		chartFile.setToField( 7 );
//		chartFile.getTime( valuetime, TRUE );
//		Q3value[nPaxCount]= static_cast<float>(valuetime.asMinutes());
//		
//		chartFile.setToField( 8 );
//		chartFile.getTime( valuetime, TRUE );
//		P1value[nPaxCount]= static_cast<float>(valuetime.asMinutes());
//		
//		chartFile.setToField( 9 );
//		chartFile.getTime( valuetime, TRUE );
//		P5value[nPaxCount]= static_cast<float>(valuetime.asMinutes());
//		
//		chartFile.setToField( 10 );
//		chartFile.getTime( valuetime, TRUE );
//		P10value[nPaxCount]= static_cast<float>(valuetime.asMinutes());
//		
//		chartFile.setToField( 11 );
//		chartFile.getTime( valuetime, TRUE );
//		P90value[nPaxCount]= static_cast<float>(valuetime.asMinutes());
//		
//		chartFile.setToField( 12 );
//		chartFile.getTime( valuetime, TRUE );
//		P95value[nPaxCount]= static_cast<float>(valuetime.asMinutes());
//		
//		chartFile.setToField( 13 );
//		chartFile.getTime( valuetime, TRUE );
//		P99value[nPaxCount]= static_cast<float>(valuetime.asMinutes());
//		
//		chartFile.setToField( 14 );
//		chartFile.getTime( valuetime, TRUE );
//		Sigmavalue[nPaxCount]= static_cast<float>(valuetime.asMinutes());
//	}
//	
//	CString strLabel;
//	m_MSChartCtrl.SetRowCount(nPaxCount);
//	m_MSChartCtrl.SetColumnCount(13);
//	m_MSChartCtrl.SetColumnLabelCount(13);
//	m_MSChartCtrl.SetColumn(1);
//	m_MSChartCtrl.SetColumnLabel("Minimum");
//	m_MSChartCtrl.SetColumn(2);
//	m_MSChartCtrl.SetColumnLabel("Average");
//	m_MSChartCtrl.SetColumn(3);
//	m_MSChartCtrl.SetColumnLabel("Maximum");
//
//	m_MSChartCtrl.SetColumn(4);
//	m_MSChartCtrl.SetColumnLabel("Q1");
//	
//	m_MSChartCtrl.SetColumn(5);
//	m_MSChartCtrl.SetColumnLabel("Q2");
//	
//	m_MSChartCtrl.SetColumn(6);
//	m_MSChartCtrl.SetColumnLabel("Q3");
//	
//	m_MSChartCtrl.SetColumn(7);
//	m_MSChartCtrl.SetColumnLabel("P1");
//	
//	m_MSChartCtrl.SetColumn(8);
//	m_MSChartCtrl.SetColumnLabel("P5");
//	
//	m_MSChartCtrl.SetColumn(9);
//	m_MSChartCtrl.SetColumnLabel("P10");
//	
//	m_MSChartCtrl.SetColumn(10);
//	m_MSChartCtrl.SetColumnLabel("P90");
//	
//	m_MSChartCtrl.SetColumn(11);
//	m_MSChartCtrl.SetColumnLabel("P95");
//	
//	m_MSChartCtrl.SetColumn(12);
//	m_MSChartCtrl.SetColumnLabel("P99");
//	
//	m_MSChartCtrl.SetColumn(13);
//	m_MSChartCtrl.SetColumnLabel("Sigma");
//
//	for(int i=1;i<=nPaxCount;i++)
//	{
//		m_MSChartCtrl.SetRow(i);
//		DealRowLabel(strxValue[i]);
//		m_MSChartCtrl.SetRowLabel(strxValue[i]);		
//		m_MSChartCtrl.SetColumn(1);
//		strLabel.Format("%f",minvalue[i]);
//		m_MSChartCtrl.SetData(strLabel);
//		m_MSChartCtrl.SetRow(i);	//add by bird 2003/2/18
//		m_MSChartCtrl.SetColumn(2);
//		strLabel.Format("%f",avevalue[i]);
//		m_MSChartCtrl.SetData(strLabel);
//		m_MSChartCtrl.SetRow(i);	//add by bird 2003/2/18
//		m_MSChartCtrl.SetColumn(3);
//		strLabel.Format("%f",maxvalue[i]);
//		m_MSChartCtrl.SetData(strLabel);
//
//				m_MSChartCtrl.SetRow(i);
//		m_MSChartCtrl.SetColumn(4);
//		strLabel.Format("%f",Q1value[i]);
//		m_MSChartCtrl.SetData(strLabel);
//		
//		m_MSChartCtrl.SetRow(i);
//		m_MSChartCtrl.SetColumn(5);
//		strLabel.Format("%f",Q2value[i]);
//		m_MSChartCtrl.SetData(strLabel);
//		
//		m_MSChartCtrl.SetRow(i);
//		m_MSChartCtrl.SetColumn(6);
//		strLabel.Format("%f",Q3value[i]);
//		m_MSChartCtrl.SetData(strLabel);
//		
//		m_MSChartCtrl.SetRow(i);
//		m_MSChartCtrl.SetColumn(7);
//		strLabel.Format("%f",P1value[i]);
//		m_MSChartCtrl.SetData(strLabel);
//		
//		m_MSChartCtrl.SetRow(i);
//		m_MSChartCtrl.SetColumn(8);
//		strLabel.Format("%f",P5value[i]);
//		m_MSChartCtrl.SetData(strLabel);
//		
//		m_MSChartCtrl.SetRow(i);
//		m_MSChartCtrl.SetColumn(9);
//		strLabel.Format("%f",P10value[i]);
//		m_MSChartCtrl.SetData(strLabel);
//		
//		m_MSChartCtrl.SetRow(i);
//		m_MSChartCtrl.SetColumn(10);
//		strLabel.Format("%f",P90value[i]);
//		m_MSChartCtrl.SetData(strLabel);
//		
//		m_MSChartCtrl.SetRow(i);
//		m_MSChartCtrl.SetColumn(11);
//		strLabel.Format("%f",P95value[i]);
//		m_MSChartCtrl.SetData(strLabel);
//		
//		m_MSChartCtrl.SetRow(i);
//		m_MSChartCtrl.SetColumn(12);
//		strLabel.Format("%f",P99value[i]);
//		m_MSChartCtrl.SetData(strLabel);
//		
//		m_MSChartCtrl.SetRow(i);
//		m_MSChartCtrl.SetColumn(13);
//		strLabel.Format("%f",Sigmavalue[i]);
//		m_MSChartCtrl.SetData(strLabel);
//	}
//	
//	chartFile.closeIn();
//	m_MSChartCtrl.SetShowLegend(TRUE);
//}
//
//void CRepGraphView::BagCountDetailed()
//{
//	//AfxMessageBox("PaxCountDetailed");
//	// load report from file
//	CString sFileName;
//	if( GetLoadReportType() == load_by_user )
//	{
//		sFileName = GetUserLoadReportFile();
//	}
//	else								// load_by_system
//	{
//		sFileName = ((CTermPlanDoc*)GetDocument())->GetTerminal().GetSimReportManager()->GetCurrentReportFileName( ((CTermPlanDoc*)GetDocument())->m_ProjInfo.path );
//	}
//	chartFile.init (sFileName, READ);
//	//2.)Draw Title,X Axis Title,Y Axis Title,Footnote
//	chartFile.reset();
//	chartFile.getLine();
//	chartFile.getLine();
//	char string[4][128];
//	chartFile.getField (string[0], 128);
//	chartFile.getField (string[1], 128);
//	chartFile.getField (string[2], 128);
//	chartFile.getField (string[3], 128);
//	CString strFootnote;
//	strFootnote.Format("%s %s %s %s",NoUnderline(CString(string[0])),string[1],string[2],string[3]);
//	SetHeadXYTitle("Bag Count(Detailed)","Time of Day","Bag Count",strFootnote);
//	//3.)Draw it
//	CString strxValue[1000];//The Pax Type Name
//	int nPaxCount=0;
//	char buf[128];
//	CString strTemp;
//	float value = 0.0;
//	ElapsedTime valuetime;
//	int **yValue;
//	int interval = GetInterval( string[1] );
//	yValue = new int*[1000];
//	for(int k=0;k<1000;k++)
//	{
//		yValue[k] = new int[ 25*60/interval +1];
//		for( int i=0; i<25*60/interval +1; i++ )
//		{
//			yValue[k][i] =0;
//		}
//	}
//	
//	int i=0;
//	chartFile.reset();
//	chartFile.seekToLine(0);
//	chartFile.skipLines(3);
//
//	if(!chartFile.getLine())
//	{
//		chartFile.closeIn();
//		return;
//	}
//	chartFile.setToField(0);
//	chartFile.getField(buf,128);
//	nPaxCount+=1;
//	strxValue[nPaxCount].Format("%s",buf);//Get The First Pax Type
//	
//	int IntervalNum = 1;
//	chartFile.reset();
//	chartFile.seekToLine(0);
//	chartFile.skipLines(3);	
//	while (chartFile.getLine())
//	{
//		chartFile.setToField(0);
//		chartFile.getField(buf,128);
//		strTemp.Format("%s",buf);
//		if(strxValue[nPaxCount]!=strTemp)
//		{
//			nPaxCount++;
//			strxValue[nPaxCount]=strTemp;
//			IntervalNum = 1;
//		}
//
//		chartFile.setToField (2);
//		chartFile.getFloat (value);
//		
//		yValue[nPaxCount][IntervalNum++] = static_cast<int>(value);
//	}
//	
//	// add by bird 2003/10/14
//	ElapsedTime BeginTime,EndTime;
//	getTime( string[1], BeginTime, EndTime );
//	int nRowNum = (EndTime.asMinutes()-BeginTime.asMinutes())/interval + 1; 
//
//	m_MSChartCtrl.SetRowCount(nRowNum);
//	m_MSChartCtrl.SetColumnCount(nPaxCount);
//	m_MSChartCtrl.SetColumnLabelCount(nPaxCount);
//	
//	CString strLabel;
//	for(i=1;i<=nRowNum;i++)
//	{
//		ElapsedTime detaTime( interval * (i-1) * 60l );
//		ElapsedTime time = BeginTime + detaTime;
//		strLabel.Format("%s", time.printTime(0));
//
//		m_MSChartCtrl.SetRow(i);
//		DealRowLabel(strLabel);
//		m_MSChartCtrl.SetRowLabel(strLabel);
//		
//		for(k=1;k<=nPaxCount;k++)
//		{
//			m_MSChartCtrl.SetRow(i);	//add by bird 2003/2/18
//			m_MSChartCtrl.SetColumn(k);
//			m_MSChartCtrl.SetColumnLabel(strxValue[k]);	
//			strLabel.Format("%d",yValue[k][i]);
//			m_MSChartCtrl.SetData(strLabel);
//		}
//		
//	}
//
//	for( k=0; k<1000; k++ )
//	{
//		delete []yValue[k];
//	}
//	delete []yValue;
//
//	chartFile.closeIn();
//	m_MSChartCtrl.SetShowLegend(TRUE);
//}
//
//void CRepGraphView::BagCountMinSummary()
//{
//	//AfxMessageBox("PaxCountDetailed");
//	// load report from file
//	CString sFileName;
//	if( GetLoadReportType() == load_by_user )
//	{
//		sFileName = GetUserLoadReportFile();
//	}
//	else								// load_by_system
//	{
//		sFileName = ((CTermPlanDoc*)GetDocument())->GetTerminal().GetSimReportManager()->GetCurrentReportFileName( ((CTermPlanDoc*)GetDocument())->m_ProjInfo.path );
//	}
//	chartFile.init (sFileName, READ);
//	//2.)Draw Title,X Axis Title,Y Axis Title,Footnote
//	chartFile.reset();
//	chartFile.getLine();
//	chartFile.getLine();
//	char string[4][128];
//	chartFile.getField (string[0], 128);
//	chartFile.getField (string[1], 128);
//	chartFile.getField (string[2], 128);
//	chartFile.getField (string[3], 128);
//	CString strFootnote;
//	strFootnote.Format("%s %s %s %s",NoUnderline(CString(string[0])),string[1],string[2],string[3]);
//	SetHeadXYTitle("Min Bag Count(Summary)","Time of Day","Minimum Bag Count",strFootnote);
//	//3.)Draw it
//	CString strxValue[1000];//The Pax Type Name
//	int nPaxCount=0;
//	char buf[128];
//	CString strTemp;
//	float value = 0.0;
//	ElapsedTime valuetime;
//	int interval = GetInterval( string[1] );
//	int **yValue = new int*[1000];
//	
//	for(int k=0;k<1000;k++)
//	{
//		yValue[k] = new int[ 25*60/interval +1];
//		for(int i=0;i<25*60/interval +1;i++)
//		{
//			yValue[k][i]=0;
//		}
//	}
//	
//	int i=0;
//	chartFile.reset();
//	chartFile.skipLines(3);
//
//	if(!chartFile.getLine())
//	{
//		chartFile.closeIn();
//		return;
//	}
//	chartFile.setToField(0);
//	chartFile.getField(buf,128);
//	nPaxCount+=1;
//	strxValue[nPaxCount].Format("%s",buf);//Get The First Pax Type
//	
//	int IntervalNum = 1;
//	chartFile.reset();
//	chartFile.seekToLine(0);
//	chartFile.skipLines(3);	
//	while (chartFile.getLine())
//	{
//		chartFile.setToField(0);
//		chartFile.getField(buf,128);
//		strTemp.Format("%s",buf);
//		if(strxValue[nPaxCount]!=strTemp)
//		{
//			nPaxCount++;
//			strxValue[nPaxCount]=strTemp;
//			IntervalNum = 1;
//		}
//
//		chartFile.setToField (3);
//		chartFile.getFloat (value);
//		
//		yValue[nPaxCount][IntervalNum++] = static_cast<int>(value);
//	}
//	
//	// add by bird 2003/10/14
//	ElapsedTime BeginTime,EndTime;
//	getTime( string[1], BeginTime, EndTime );
//	int nRowNum = (EndTime.asMinutes()-BeginTime.asMinutes())/interval + 1; 
//
//	m_MSChartCtrl.SetRowCount(nRowNum);
//	m_MSChartCtrl.SetColumnCount(nPaxCount);
//	m_MSChartCtrl.SetColumnLabelCount(nPaxCount);
//	
//	CString strLabel;
//	for(i=1;i<=nRowNum;i++)
//	{
//		ElapsedTime detaTime( interval * (i-1) * 60l );
//		ElapsedTime time = BeginTime + detaTime;
//		strLabel.Format("%s", time.printTime(0));
//
//		m_MSChartCtrl.SetRow(i);
//		DealRowLabel(strLabel);
//		m_MSChartCtrl.SetRowLabel(strLabel);
//		
//		for(k=1;k<=nPaxCount;k++)
//		{
//			m_MSChartCtrl.SetRow(i);	//add by bird 2003/2/18
//			m_MSChartCtrl.SetColumn(k);
//			m_MSChartCtrl.SetColumnLabel(strxValue[k]);	
//			strLabel.Format("%d",yValue[k][i]);
//			m_MSChartCtrl.SetData(strLabel);
//		}
//		
//	}
//
//	for( k=0; k<1000; k++ )
//	{
//		delete []yValue[k];
//	}
//	delete []yValue;
//	
//	chartFile.closeIn();
//	m_MSChartCtrl.SetShowLegend(TRUE);
//}
//
//void CRepGraphView::BagCountAvgSummary()
//{
//	//AfxMessageBox("PaxCountDetailed");
//	// load report from file
//	CString sFileName;
//	if( GetLoadReportType() == load_by_user )
//	{
//		sFileName = GetUserLoadReportFile();
//	}
//	else								// load_by_system
//	{
//		sFileName = ((CTermPlanDoc*)GetDocument())->GetTerminal().GetSimReportManager()->GetCurrentReportFileName( ((CTermPlanDoc*)GetDocument())->m_ProjInfo.path );
//	}
//	chartFile.init (sFileName, READ);
//	//2.)Draw Title,X Axis Title,Y Axis Title,Footnote
//	chartFile.reset();
//	chartFile.getLine();
//	chartFile.getLine();
//	char string[4][128];
//	chartFile.getField (string[0], 128);
//	chartFile.getField (string[1], 128);
//	chartFile.getField (string[2], 128);
//	chartFile.getField (string[3], 128);
//	CString strFootnote;
//	strFootnote.Format("%s %s %s %s",NoUnderline(CString(string[0])),string[1],string[2],string[3]);
//	SetHeadXYTitle("Avg Bag Count(Summary)","Time of Day","Average Bag Count",strFootnote);
//	//3.)Draw it
//	CString strxValue[1000];//The Pax Type Name
//	int nPaxCount=0;
//	char buf[128];
//	CString strTemp;
//	float value = 0.0;
//	ElapsedTime valuetime;
//	int interval = GetInterval( string[1] );
//	float **yValue = new float*[1000];
//	for(int k=0;k<1000;k++)
//	{
//		yValue[k] = new float[ 25*60/interval +1 ];
//		for(int i=0;i<25*60/interval +1;i++)
//		{
//			yValue[k][i]=0;
//		}
//	}
//	
//	int i=0;
//	chartFile.reset();
//	chartFile.skipLines(3);
//
//	if(!chartFile.getLine())
//	{
//		chartFile.closeIn();
//		return;
//	}
//	chartFile.setToField(0);
//	chartFile.getField(buf,128);
//	nPaxCount+=1;
//	strxValue[nPaxCount].Format("%s",buf);//Get The First Pax Type
//	
//	int IntervalNum = 1;
//	chartFile.reset();
//	chartFile.seekToLine(0);
//	chartFile.skipLines(3);	
//	while (chartFile.getLine())
//	{
//		chartFile.setToField(0);
//		chartFile.getField(buf,128);
//		strTemp.Format("%s",buf);
//		if(strxValue[nPaxCount]!=strTemp)
//		{
//			nPaxCount++;
//			strxValue[nPaxCount]=strTemp;
//			IntervalNum = 1;
//		}
//
//		chartFile.setToField (4);
//		chartFile.getFloat (value);
//		
//		yValue[nPaxCount][IntervalNum++]=value;
//	}
//	// add by bird 2003/10/14
//	ElapsedTime BeginTime,EndTime;
//	getTime( string[1], BeginTime, EndTime );
//	int nRowNum = (EndTime.asMinutes()-BeginTime.asMinutes())/interval + 1; 
//
//	m_MSChartCtrl.SetRowCount(nRowNum);
//	m_MSChartCtrl.SetColumnCount(nPaxCount);
//	m_MSChartCtrl.SetColumnLabelCount(nPaxCount);
//	
//	CString strLabel;
//	for(i=1;i<=nRowNum;i++)
//	{
//		ElapsedTime detaTime( interval * (i-1) * 60l );
//		ElapsedTime time = BeginTime + detaTime;
//		strLabel.Format("%s", time.printTime(0));
//
//		m_MSChartCtrl.SetRow(i);
//		DealRowLabel(strLabel);
//		m_MSChartCtrl.SetRowLabel(strLabel);
//		
//		for(k=1;k<=nPaxCount;k++)
//		{
//			m_MSChartCtrl.SetRow(i);	//add by bird 2003/2/18
//			m_MSChartCtrl.SetColumn(k);
//			m_MSChartCtrl.SetColumnLabel(strxValue[k]);	
//			strLabel.Format("%.2f",yValue[k][i]);
//			m_MSChartCtrl.SetData(strLabel);
//		}
//		
//	}
//	for( k=0; k<1000; k++ )
//	{
//		delete []yValue[k];
//	}
//	delete []yValue;
//	
//	chartFile.closeIn();
//	m_MSChartCtrl.SetShowLegend(TRUE);
//}
//
//void CRepGraphView::BagCountMaxSummary()
//{
//	//AfxMessageBox("PaxCountDetailed");
//	// load report from file
//	CString sFileName;
//	if( GetLoadReportType() == load_by_user )
//	{
//		sFileName = GetUserLoadReportFile();
//	}
//	else								// load_by_system
//	{
//		sFileName = ((CTermPlanDoc*)GetDocument())->GetTerminal().GetSimReportManager()->GetCurrentReportFileName( ((CTermPlanDoc*)GetDocument())->m_ProjInfo.path );
//	}
//	chartFile.init (sFileName, READ);
//	//2.)Draw Title,X Axis Title,Y Axis Title,Footnote
//	chartFile.reset();
//	chartFile.getLine();
//	chartFile.getLine();
//	char string[4][128];
//	chartFile.getField (string[0], 128);
//	chartFile.getField (string[1], 128);
//	chartFile.getField (string[2], 128);
//	chartFile.getField (string[3], 128);
//	CString strFootnote;
//	strFootnote.Format("%s %s %s %s",NoUnderline(CString(string[0])),string[1],string[2],string[3]);
//	SetHeadXYTitle("Max Bag Count(Summary)","Time of Day","Maximum Bag Count",strFootnote);
//	//3.)Draw it
//	CString strxValue[1000];//The Pax Type Name
//	int nPaxCount=0;
//	char buf[128];
//	CString strTemp;
//	float value = 0.0;
//	ElapsedTime valuetime;
//	int interval = GetInterval( string[1] );
//	int **yValue = new int*[1000];
//	for( int k=0; k<1000; k++ )
//	{
//		yValue[k] = new int[25*60/interval +1];
//		for( int i=0; i<25*60/interval +1; i++ )
//		{
//			yValue[k][i] = 0;
//		}
//	}
//	
//	int i=0;
//	chartFile.reset();
//	chartFile.skipLines(3);
//
//	if(!chartFile.getLine())
//	{
//		chartFile.closeIn();
//		return;
//	}
//	chartFile.setToField(0);
//	chartFile.getField(buf,128);
//	nPaxCount+=1;
//	strxValue[nPaxCount].Format("%s",buf);//Get The First Pax Type
//	
//	int IntervalNum = 1;
//	chartFile.reset();
//	chartFile.seekToLine(0);
//	chartFile.skipLines(3);	
//	while (chartFile.getLine())
//	{
//		chartFile.setToField(0);
//		chartFile.getField(buf,128);
//		strTemp.Format("%s",buf);
//		if(strxValue[nPaxCount]!=strTemp)
//		{
//			nPaxCount++;
//			strxValue[nPaxCount]=strTemp;
//			IntervalNum = 1;
//		}
//
//		chartFile.setToField (5);
//		chartFile.getFloat (value);
//		yValue[nPaxCount][IntervalNum++] = static_cast<int>(value);
//	}
//	// add by bird 2003/10/14
//	ElapsedTime BeginTime,EndTime;
//	getTime( string[1], BeginTime, EndTime );
//	int nRowNum = (EndTime.asMinutes()-BeginTime.asMinutes())/interval + 1; 
//
//	m_MSChartCtrl.SetRowCount(nRowNum);
//	m_MSChartCtrl.SetColumnCount(nPaxCount);
//	m_MSChartCtrl.SetColumnLabelCount(nPaxCount);
//	
//	CString strLabel;
//	for(i=1;i<=nRowNum;i++)
//	{
//		ElapsedTime detaTime( interval * (i-1) * 60l );
//		ElapsedTime time = BeginTime + detaTime;
//		strLabel.Format("%s", time.printTime(0));
//
//		m_MSChartCtrl.SetRow(i);
//		DealRowLabel(strLabel);
//		m_MSChartCtrl.SetRowLabel(strLabel);
//		
//		for(k=1;k<=nPaxCount;k++)
//		{
//			m_MSChartCtrl.SetRow(i);	//add by bird 2003/2/18
//			m_MSChartCtrl.SetColumn(k);
//			m_MSChartCtrl.SetColumnLabel(strxValue[k]);	
//			strLabel.Format("%d",yValue[k][i]);
//			m_MSChartCtrl.SetData(strLabel);
//		}
//		
//	}
//
//	for( k=0; k<1000; k++ )
//	{
//		delete []yValue[k];
//	}
//	delete []yValue;
//	
//	chartFile.closeIn();
//	m_MSChartCtrl.SetShowLegend(TRUE);
//}
//
//void CRepGraphView::BagCountCombinedSummary()
//{
//	//AfxMessageBox("PaxCountDetailed");
//	// load report from file
//	CString sFileName;
//	if( GetLoadReportType() == load_by_user )
//	{
//		sFileName = GetUserLoadReportFile();
//	}
//	else								// load_by_system
//	{
//		sFileName = ((CTermPlanDoc*)GetDocument())->GetTerminal().GetSimReportManager()->GetCurrentReportFileName( ((CTermPlanDoc*)GetDocument())->m_ProjInfo.path );
//	}
//	chartFile.init (sFileName, READ);
//	//2.)Draw Title,X Axis Title,Y Axis Title,Footnote
//	chartFile.reset();
//	chartFile.getLine();
//	chartFile.getLine();
//	char string[4][128];
//	chartFile.getField (string[0], 128);
//	chartFile.getField (string[1], 128);
//	chartFile.getField (string[2], 128);
//	chartFile.getField (string[3], 128);
//	CString strFootnote;
//	strFootnote.Format("%s %s %s %s",NoUnderline(CString(string[0])),string[1],string[2],string[3]);
//	SetHeadXYTitle("Combined Bag Count(Summary)","Time of Day","Combined Bag Count",strFootnote);
//	//3.)Draw it
//	CString strxValue[1000];//The Pax Type Name
//	int nPaxCount=0;
//	char buf[128];
//	CString strTemp;
//	float value = 0.0;
//	ElapsedTime valuetime;
//	int interval = GetInterval( string[1] );
//	int **yValue = new int*[1000];
//	for(int k=0;k<1000;k++)
//	{
//		yValue[k] = new int[25*60/interval +1];
//		for(int i=0;i<25*60/interval +1;i++)
//		{
//			yValue[k][i]=0;
//		}
//	}
//	
//	int i=0;
//	chartFile.reset();
//	chartFile.skipLines(3);
//
//	if(!chartFile.getLine())
//	{
//		chartFile.closeIn();
//		return;
//	}
//	chartFile.setToField(0);
//	chartFile.getField(buf,128);
//	nPaxCount+=1;
//	strxValue[nPaxCount].Format("%s",buf);//Get The First Pax Type
//	
//	int IntervalNum = 1;
//	chartFile.reset();
//	chartFile.seekToLine(0);
//	chartFile.skipLines(3);	
//	while (chartFile.getLine())
//	{
//		chartFile.setToField(0);
//		chartFile.getField(buf,128);
//		strTemp.Format("%s",buf);
//		if(strxValue[nPaxCount]!=strTemp)
//		{
//			nPaxCount++;
//			strxValue[nPaxCount]=strTemp;
//			IntervalNum = 1;
//		}
//
//		chartFile.setToField (6);
//		chartFile.getFloat (value);
//	
//		yValue[nPaxCount][IntervalNum++]=static_cast<int>(value);
//	}
//	// add by bird 2003/10/14
//	ElapsedTime BeginTime,EndTime;
//	getTime( string[1], BeginTime, EndTime );
//	int nRowNum = (EndTime.asMinutes()-BeginTime.asMinutes())/interval + 1; 
//
//	m_MSChartCtrl.SetRowCount(nRowNum);
//	m_MSChartCtrl.SetColumnCount(nPaxCount);
//	m_MSChartCtrl.SetColumnLabelCount(nPaxCount);
//	
//	CString strLabel;
//	for(i=1;i<=nRowNum;i++)
//	{
//		ElapsedTime detaTime( interval * (i-1) * 60l );
//		ElapsedTime time = BeginTime + detaTime;
//		strLabel.Format("%s", time.printTime(0));
//
//		m_MSChartCtrl.SetRow(i);
//		DealRowLabel(strLabel);
//		m_MSChartCtrl.SetRowLabel(strLabel);
//		
//		for(k=1;k<=nPaxCount;k++)
//		{
//			m_MSChartCtrl.SetRow(i);	//add by bird 2003/2/18
//			m_MSChartCtrl.SetColumn(k);
//			m_MSChartCtrl.SetColumnLabel(strxValue[k]);	
//			strLabel.Format("%d",yValue[k][i]);
//			m_MSChartCtrl.SetData(strLabel);
//		}
//		
//	}
//	
//	for( k=0; k<1000; k++ )
//	{
//		delete []yValue[k];
//	}
//	delete []yValue;
//	
//	chartFile.closeIn();
//	m_MSChartCtrl.SetShowLegend(TRUE);
//}
//
//void CRepGraphView::CollisionCount()
//{
//
////	char filename[128];
//	ElapsedTime prevTime, curTime;
//	prevTime.set( 0 );
//	curTime.set( 0 );
//	//1.)Init chartFile
//	// load report from file
//	CString sFileName;
//	if( GetLoadReportType() == load_by_user )
//	{
//		sFileName = GetUserLoadReportFile();
//	}
//	else								// load_by_system
//	{
//		sFileName = ((CTermPlanDoc*)GetDocument())->GetTerminal().GetSimReportManager()->GetCurrentReportFileName( ((CTermPlanDoc*)GetDocument())->m_ProjInfo.path );
//	}
//	chartFile.init (sFileName, READ);
//	
//	//2.)Draw Title,X Axis Title,Y Axis Title,Footnote
//	chartFile.reset();
//	chartFile.seekToLine(0);
//	chartFile.getLine();
//	chartFile.getLine();
//	char string[4][128];
//	chartFile.getField (string[0], 128);
//	chartFile.getField (string[1], 128);
//	chartFile.getField (string[2], 128);
//	chartFile.getField (string[3], 128);
//	CString strFootnote;
//	strFootnote.Format("%s %s %s %s",NoUnderline(CString(string[0])),string[1],string[2],string[3]);
//	SetHeadXYTitle("Collision (Detailed)","Time of Day","Number of Collision",strFootnote);
//	//3.)Draw it
//	int nPaxCount=0;
//	float **yValue;
//	yValue = new float*[1000];
//	char buf[128];
//	CString strTemp;
//	float value = 0.0;
//	ElapsedTime valuetime;
//	int interval = GetInterval(string[1]);	//interval, unit: minute
//	CString strxValue[1000];				//The Pax Type Name
//	for(int k=0;k<1000;k++)
//	{
//		yValue[k] = new float[25*60/interval+1];
//
//	}
//	for(k=0;k<1000;k++)
//	{
//		for(int i=0;i<25*60/interval + 1;i++)
//		{
//			yValue[k][i]=0;
//		}
//	}
//	
//	int i=0;
//	chartFile.reset();
//	chartFile.skipLines(3);
//
//	if(!chartFile.getLine())
//	{
//		chartFile.closeIn();
//		return;
//	}
//	chartFile.setToField(0);
//	chartFile.getField(buf,128);
//	nPaxCount+=1;
//	strxValue[nPaxCount].Format("%s",buf);	//Get The First Pax Type
//
//	ElapsedTime BeginTime,EndTime;
//	getTime( string[1], BeginTime, EndTime );
//	int nRowNum = (EndTime.asMinutes()-BeginTime.asMinutes())/interval; 
//
//	chartFile.reset();
//	chartFile.seekToLine(3);
//	//get line in chart file and increment the count
//	while (chartFile.getLine())
//	{
//		chartFile.setToField(0);
//		chartFile.getField(buf,128);
//		strTemp.Format("%s",buf);
//		if(strxValue[nPaxCount]!=strTemp)	//Compare The Pax Type
//		{
//			nPaxCount++;
//			strxValue[nPaxCount]=strTemp;
//		}
//		chartFile.setToField(1);
//		chartFile.getTime(curTime);
//			
//		if( curTime < BeginTime || curTime > EndTime )
//			continue;
//		int index = ( curTime.asMinutes() - BeginTime.asMinutes() ) / interval + 1;
//		yValue[ nPaxCount][ index ]++;
//	}
//	
//	m_MSChartCtrl.SetRowCount(nRowNum);
//	m_MSChartCtrl.SetColumnCount(nPaxCount);
//	m_MSChartCtrl.SetColumnLabelCount(nPaxCount);
//	
//	CString strLabel;
//	ElapsedTime detaTime( interval * 60l );
//	ElapsedTime time( BeginTime );
//	for(i = 1; i <= nRowNum; i++)
//	{
//		CString strBegin = time.printTime(0);
//		time += detaTime;
//		CString strEnd   = time.printTime(0);
//		strLabel.Format("%s~%s", strBegin, strEnd );
//
//		m_MSChartCtrl.SetRow(i);
//		DealRowLabel(strLabel);
//		m_MSChartCtrl.SetRowLabel(strLabel);
//		
//	}
//
//	for(k = 1 ; k <= nPaxCount; k++)
//	{
//		m_MSChartCtrl.SetColumn(k);
//		m_MSChartCtrl.SetColumnLabel(strxValue[k]);
//		for(i = 1; i <= nRowNum; i++)
//		{
//			m_MSChartCtrl.SetRow(i);
//			strLabel.Format("%f",yValue[k][i]);
//			m_MSChartCtrl.SetData(strLabel);
//		}
//	}
//
//	for(k = 0; k < 1000; k++)
//	{
//		delete[] yValue[k];
//	}
//	delete[] yValue;
//
//	chartFile.closeIn();
//	m_MSChartCtrl.SetShowLegend(TRUE);
//
//}
//
//void CRepGraphView::SpaceThroughputCount()
//{
//
//	// load report from file
//	CString sFileName;
//	if( GetLoadReportType() == load_by_user )
//	{
//		sFileName = GetUserLoadReportFile();
//	}
//	else								// load_by_system
//	{
//		sFileName = ((CTermPlanDoc*)GetDocument())->GetTerminal().GetSimReportManager()->GetCurrentReportFileName( ((CTermPlanDoc*)GetDocument())->m_ProjInfo.path );
//	}
//	chartFile.init (sFileName, READ);
//	//2.)Draw Title,X Axis Title,Y Axis Title,Footnote
//	chartFile.reset();
//	chartFile.seekToLine(0);
//	chartFile.getLine();
//	chartFile.getLine();
//	char string[4][128];
//	chartFile.getField (string[0], 128);
//	chartFile.getField (string[1], 128);
//	chartFile.getField (string[2], 128);
//	chartFile.getField (string[3], 128);
//	CString strFootnote;
//	strFootnote.Format("%s %s %s %s",NoUnderline(CString(string[0])),string[1],string[2],string[3]);
//	SetHeadXYTitle("Space Throughput (Detailed)","Time of Day","Space Throughput Count",strFootnote);
//	//3.)Draw it
//	int nPaxCount=0;
//	char buf[128];
//	CString strTemp;
//	float value = 0.0;
//	ElapsedTime valuetime;
//	int hours,minutes;
//	int interval = GetInterval(string[1]);	//interval, unit: minute
//	float **yValue;
//	yValue = new float*[1000];
//	for(int k = 0; k < 1000; k++)
//	{
//		yValue[k] = new float[25*60/interval+1];
//	}
//	CString strxValue[1000];//The Pax Type Name
//	for(k=0;k<1000;k++)
//	{
//		for(int i=0;i<25*60/interval + 1;i++)
//		{
//			yValue[k][i]=0;
//		}
//	}
//	
//
//	int i=0;
//	chartFile.reset();
//	chartFile.skipLines(3);
//
//	if(!chartFile.getLine())
//	{
//		chartFile.closeIn();
//		return;
//	}
//	chartFile.setToField(0);
//	chartFile.getField(buf,128);
//	nPaxCount+=1;
//	strxValue[nPaxCount].Format("%s",buf);//Get The First Pax Type
//
//	int IntervalNum = 1;
//	chartFile.reset();
//	chartFile.skipLines(3);
//	while (chartFile.getLine())
//	{
//		chartFile.setToField(0);
//		chartFile.getField(buf,128);
//		strTemp.Format("%s",buf);
//		if(strxValue[nPaxCount]!=strTemp)
//		{
//			nPaxCount++;
//			strxValue[nPaxCount]=strTemp;
//			IntervalNum = 1;
//		}
//
//		chartFile.setToField (3);
//		chartFile.getFloat (value);
//		chartFile.setToField(1);
//		chartFile.getTime (valuetime, TRUE);
//		hours=(int)valuetime.asHours();
//		minutes=(int)valuetime.asMinutes();
//		
//		// TRACE("the time is %d hours,or %d minutes\n",hours,minutes);
//		yValue[nPaxCount][IntervalNum++]=value;	
//	}
//	
//	// add by bird 2003/10/13
//	ElapsedTime BeginTime,EndTime;
//	getTime( string[1], BeginTime, EndTime );
//	int nRowNum = (EndTime.asMinutes()-BeginTime.asMinutes())/interval; 
//
//	m_MSChartCtrl.SetRowCount(nRowNum);
//	m_MSChartCtrl.SetColumnCount(nPaxCount);
//	m_MSChartCtrl.SetColumnLabelCount(nPaxCount);
//
//	CString strLabel;
//
//	ElapsedTime detaTime( interval * 60l );
//	ElapsedTime time( BeginTime );
//	for(i = 1; i <= nRowNum; i++)
//	{
//		time.set(time.asSeconds() % WholeDay);
//		CString strBeginTime = time.printTime(0);
//		time += detaTime;
//		time.set(time.asSeconds() % WholeDay);
//		CString strEndTime	 = time.printTime(0);
//
//		strLabel.Format( "%s~%s", strBeginTime, strEndTime );
//		m_MSChartCtrl.SetRow(i);
//		DealRowLabel(strLabel);
//		m_MSChartCtrl.SetRowLabel(strLabel);
//		
//	}
//
//	for(k = 1 ; k <= nPaxCount; k++)
//	{
//		m_MSChartCtrl.SetColumn(k);
//		m_MSChartCtrl.SetColumnLabel(strxValue[k]);
//		for(i = 1; i <= nRowNum; i++)
//		{
//			m_MSChartCtrl.SetRow(i);
//			strLabel.Format("%f",yValue[k][i]);
//			m_MSChartCtrl.SetData(strLabel);
//		}
//	}
//
//	for(k = 0; k < 1000; k++)
//	{
//		delete[] yValue[k];
//	}
//	delete[] yValue;
//	chartFile.closeIn();
//	m_MSChartCtrl.SetShowLegend(TRUE);
//}
//
//
//void CRepGraphView::PaxDensityCount()
//{
//	//AfxMessageBox("PaxCountDetailed");
//	// load report from file
//	CString sFileName;
//	if( GetLoadReportType() == load_by_user )
//	{
//		sFileName = GetUserLoadReportFile();
//	}
//	else								// load_by_system
//	{
//		sFileName = ((CTermPlanDoc*)GetDocument())->GetTerminal().GetSimReportManager()->GetCurrentReportFileName( ((CTermPlanDoc*)GetDocument())->m_ProjInfo.path );
//	}
//	chartFile.init (sFileName, READ);
//	//2.)Draw Title,X Axis Title,Y Axis Title,Footnote
//	chartFile.reset();
//	chartFile.getLine();
//	chartFile.getLine();
//	char string[4][128];
//	chartFile.getField (string[0], 128);
//	chartFile.getField (string[1], 128);
//	chartFile.getField (string[2], 128);
//	chartFile.getField (string[3], 128);
//	CString strFootnote;
//	strFootnote.Format("%s %s %s %s",NoUnderline(CString(string[0])),string[1],string[2],string[3]);
//	SetHeadXYTitle("Count","Time of Day","Passengers in Area",strFootnote);
//	//3.)Draw it
//	CString strxValue[1000];//The Pax Type Name
//	int nPaxCount=0;
//	char buf[128];
//	CString strTemp;
//	float value = 0.0;
//	ElapsedTime valuetime;
//	int hours,minutes;
//	int interval = GetInterval(string[1]);
//	float **yValue;
//	yValue = new float*[1000];
//	for(int k=0;k<1000;k++)
//	{
//		yValue[k] = new float[25*60/interval+1];
//	}
//	for(k=0;k<1000;k++)
//	{
//		for(int i=0;i<25*60/interval + 1;i++)
//		{
//			yValue[k][i]=0;
//		}
//	}
//
//	int i=0;
//	chartFile.reset();
//	chartFile.skipLines(3);
//
//	if(!chartFile.getLine())
//	{
//		chartFile.closeIn();
//		return;
//	}
//	chartFile.setToField(0);
//	chartFile.getField(buf,128);
//	nPaxCount+=1;
//	strxValue[nPaxCount].Format("%s",buf);//Get The First Pax Type
//	
//	int IntervalNum = 1;
//	chartFile.reset();
//	chartFile.skipLines(3);
//	while (chartFile.getLine())
//	{
//		chartFile.setToField(0);
//		chartFile.getField(buf,128);
//		strTemp.Format("%s",buf);
//		if(strxValue[nPaxCount]!=strTemp)
//		{
//			nPaxCount++;
//			strxValue[nPaxCount]=strTemp;
//			IntervalNum = 1;
//		}
//
//		chartFile.setToField (2);
//		chartFile.getFloat (value);
//		chartFile.setToField(1);
//		chartFile.getTime (valuetime, TRUE);
//		hours=(int)valuetime.asHours();
//		minutes=(int)valuetime.asMinutes();
//		
//		// TRACE("the time is %d hours,or %d minutes\n",hours,minutes);
//		yValue[nPaxCount][IntervalNum++]=value;	
//	}
//	
//	//////////////////////////////////////////////////////////////////////////
//	// add by bird 2003/10/13
//	ElapsedTime BeginTime,EndTime;
//	getTime( string[1], BeginTime, EndTime );
//	int nRowNum = (EndTime.asMinutes()-BeginTime.asMinutes())/interval +1; 
//
//	m_MSChartCtrl.SetRowCount(nRowNum);
//	m_MSChartCtrl.SetColumnCount(nPaxCount);
//	m_MSChartCtrl.SetColumnLabelCount(nPaxCount);
//	
//	CString strLabel;
//	for(i = 1; i <= nRowNum; i++)
//	{
//		ElapsedTime detaTime( interval * (i-1) * 60l );
//		ElapsedTime time = BeginTime + detaTime;
//		//int nDay = time.asHours() / 24;
//		//time -= nDay * WholeDay;
//		time.set(time.asSeconds() % WholeDay);
//		strLabel.Format("%s", time.printTime(0));
//
//		m_MSChartCtrl.SetRow(i);
//		DealRowLabel(strLabel);
//		m_MSChartCtrl.SetRowLabel(strLabel);
//		
//	}
//
//	for(k = 1 ; k <= nPaxCount; k++)
//	{
//		m_MSChartCtrl.SetColumn(k);
//		m_MSChartCtrl.SetColumnLabel(strxValue[k]);
//		for(i = 1; i <= nRowNum; i++)
//		{
//			m_MSChartCtrl.SetRow(i);
//			strLabel.Format("%f",yValue[k][i]);
//			m_MSChartCtrl.SetData(strLabel);
//		}
//	}
//
//	for(k = 0; k < 1000; k++)
//	{
//		delete[] yValue[k];
//	}
//	delete[] yValue;
//	chartFile.closeIn();
//	m_MSChartCtrl.SetShowLegend(TRUE);
//}
//
//void CRepGraphView::PaxDensityPaxM2()
//{
//	//AfxMessageBox("PaxCountDetailed");
//	// load report from file
//	CString sFileName;
//	if( GetLoadReportType() == load_by_user )
//	{
//		sFileName = GetUserLoadReportFile();
//	}
//	else								// load_by_system
//	{
//		sFileName = ((CTermPlanDoc*)GetDocument())->GetTerminal().GetSimReportManager()->GetCurrentReportFileName( ((CTermPlanDoc*)GetDocument())->m_ProjInfo.path );
//	}
//	chartFile.init (sFileName, READ);
//	//2.)Draw Title,X Axis Title,Y Axis Title,Footnote
//	chartFile.reset();
//	chartFile.getLine();
//	chartFile.getLine();
//	char string[4][128];
//	chartFile.getField (string[0], 128);
//	chartFile.getField (string[1], 128);
//	chartFile.getField (string[2], 128);
//	chartFile.getField (string[3], 128);
//	CString strFootnote;
//	strFootnote.Format("%s %s %s %s",NoUnderline(CString(string[0])),string[1],string[2],string[3]);
//	SetHeadXYTitle("Pax/m2","Time of Day","Passengers per Square Meter",strFootnote);
//	//3.)Draw it
//	CString strxValue[1000];//The Pax Type Name
//	int nPaxCount=0;
//	char buf[128];
//	CString strTemp;
//	float value = 0.0;
//	ElapsedTime valuetime;
//	int hours,minutes;
//	float **yValue;
//	yValue = new float*[1000];
//	int interval = GetInterval(string[1]);
//	for(int k = 0; k < 1000; k++)
//	{
//		yValue[k] = new float[25*60/interval+1];
//	}
//	for(k=0;k<1000;k++)
//	{
//		for(int i=0;i<25*60/interval+1;i++)
//		{
//			yValue[k][i]=0;
//		}
//	}
//	
//	int i=0;
//	chartFile.reset();
//	chartFile.skipLines(3);
//
//	if(!chartFile.getLine())
//	{
//		chartFile.closeIn();
//		return;
//	}
//	chartFile.setToField(0);
//	chartFile.getField(buf,128);
//	nPaxCount+=1;
//	strxValue[nPaxCount].Format("%s",buf);//Get The First Pax Type
//	
//	int IntervalNum = 1;
//	chartFile.reset();
//	chartFile.skipLines(3);
//	while (chartFile.getLine())
//	{
//		chartFile.setToField(0);
//		chartFile.getField(buf,128);
//		strTemp.Format("%s",buf);
//		if(strxValue[nPaxCount]!=strTemp)
//		{
//			nPaxCount++;
//			strxValue[nPaxCount]=strTemp;
//			IntervalNum = 1;
//		}
//
//		chartFile.setToField (3);
//		chartFile.getFloat (value);
//		chartFile.setToField(1);
//		chartFile.getTime (valuetime, TRUE);
//		hours=(int)valuetime.asHours();
//		minutes=(int)valuetime.asMinutes();
//		
//		// TRACE("the time is %d hours,or %d minutes\n",hours,minutes);
//		yValue[nPaxCount][IntervalNum++]=value;	
//	}
//	
//	// add by bird 2003/10/13
//	ElapsedTime BeginTime,EndTime;
//	getTime( string[1], BeginTime, EndTime );
//	int nRowNum = (EndTime.asMinutes()-BeginTime.asMinutes())/interval +1; 
//
//	m_MSChartCtrl.SetRowCount(nRowNum);
//	m_MSChartCtrl.SetColumnCount(nPaxCount);
//	m_MSChartCtrl.SetColumnLabelCount(nPaxCount);
//	
//	CString strLabel;
//	for(i = 1; i <= nRowNum; i++)
//	{
//		ElapsedTime detaTime( interval * (i-1) * 60l );
//		ElapsedTime time = BeginTime + detaTime;
//		strLabel.Format("%s", time.printTime(0));
//
//		m_MSChartCtrl.SetRow(i);
//		DealRowLabel(strLabel);
//		m_MSChartCtrl.SetRowLabel(strLabel);
//		
//	}
//
//	for(k = 1 ; k <= nPaxCount; k++)
//	{
//		m_MSChartCtrl.SetColumn(k);
//		m_MSChartCtrl.SetColumnLabel(strxValue[k]);
//		for(i = 1; i <= nRowNum; i++)
//		{
//			m_MSChartCtrl.SetRow(i);
//			strLabel.Format("%f",yValue[k][i]);
//			m_MSChartCtrl.SetData(strLabel);
//		}
//	}
//
//	for(k = 0; k < 1000; k++)
//	{
//		delete[] yValue[k];
//	}
//	delete[] yValue;
//
//	chartFile.closeIn();
//	m_MSChartCtrl.SetShowLegend(TRUE);
//}
//
//
//void CRepGraphView::AcOpArrivalCount()
//{
//	// load report from file
//	CString sFileName;
//	if( GetLoadReportType() == load_by_user )
//	{
//		sFileName = GetUserLoadReportFile();
//	}
//	else								// load_by_system
//	{
//		sFileName = ((CTermPlanDoc*)GetDocument())->GetTerminal().GetSimReportManager()->GetCurrentReportFileName( ((CTermPlanDoc*)GetDocument())->m_ProjInfo.path );
//	}
//	chartFile.init (sFileName, READ);
//	//2.)Draw Title,X Axis Title,Y Axis Title,Footnote
//	chartFile.reset();
//	chartFile.seekToLine(0);
//	chartFile.getLine();
//	chartFile.getLine();
//	char string[4][128];
//	chartFile.getField (string[0], 128);
//	chartFile.getField (string[1], 128);
//	chartFile.getField (string[2], 128);
//	chartFile.getField (string[3], 128);
//	CString strFootnote;
//	strFootnote.Format("%s %s %s %s",NoUnderline(CString(string[0])),string[1],string[2],string[3]);
//
//	
//	SetHeadXYTitle("Count","Time of Day","Number of aircraft operations  ",strFootnote);
//
//	//3.)Draw it	
//	ElapsedTime valuetime,curTime;
//	int interval = GetInterval(string[1]);
//	
//	typedef std::map<int,int>TIMEVALUE;
//	typedef std::map<CString, TIMEVALUE> MAPVALUE;
//	MAPVALUE mapValue;
//
//	int IntervalNum = 1;
//	char flttype[30];
//	chartFile.reset();
//	chartFile.seekToLine(3);
//	ElapsedTime BeginTime,EndTime;
//	getTime( string[1], BeginTime, EndTime );
//	int nRowNum = (EndTime.asMinutes()-BeginTime.asMinutes())/interval +1; 
//
//	while (chartFile.getLine())
//	{
//		chartFile.setToField(4);
//		chartFile.getTime(curTime);
//
//		if( curTime < BeginTime || curTime > EndTime )
//			continue;
//
//		int index = (curTime.asMinutes() - BeginTime.asMinutes())/interval +1;
//		chartFile.setToField(1);
//		chartFile.getField( flttype, 30 );
//		if(strcmp(flttype, "DEP") != 0)
//		{
//			char sFlightType[512];
//
//			int iFieldIdx = 13;
//			chartFile.setToField(iFieldIdx);
//			while( chartFile.getField( sFlightType, 512 ) )
//			{
//				CString sKey( sFlightType );
//				MAPVALUE::iterator iter= mapValue.find( sKey );
//				if( iter != mapValue.end() )//exist
//				{
//					mapValue[ sKey ][ index ] ++;
//				}
//				else// first time 
//				{
//					TIMEVALUE tempValue;
//					for( int i=0; i<nRowNum; ++i )
//					{
//						tempValue.insert(TIMEVALUE::value_type(i,0) );
//					}
//					
//					tempValue[ index ] = 1;
//					
//					mapValue.insert( MAPVALUE::value_type( sKey, tempValue ) );
//				}
//
//				++iFieldIdx;
//				chartFile.setToField(iFieldIdx);
//			}			
//		}
//
//	}
//
//	m_MSChartCtrl.SetRowCount(nRowNum);
//	m_MSChartCtrl.SetColumnCount(mapValue.size());
//	m_MSChartCtrl.SetColumnLabelCount(mapValue.size());
//	
//	CString strLabel;
//	for(int i = 1; i <= nRowNum; i++)
//	{
//		ElapsedTime detaTime( interval * (i-1) * 60l );
//		ElapsedTime time = BeginTime + detaTime;
//		strLabel.Format("%s", time.printTime(0));
//		m_MSChartCtrl.SetRow(i);
//		DealRowLabel(strLabel);
//		m_MSChartCtrl.SetRowLabel(strLabel);
//		
//	}
//	
//	MAPVALUE::iterator iter = mapValue.begin();
//	
//	for(int k = 1 ; k <= static_cast<int>(mapValue.size()); k++,++iter)
//	{
//		m_MSChartCtrl.SetColumn(k);
//		m_MSChartCtrl.SetColumnLabel( iter->first );
//		for(i = 1; i <= nRowNum; i++)
//		{
//			m_MSChartCtrl.SetRow(i);
//			strLabel.Format("%d",iter->second[ i] );
//			m_MSChartCtrl.SetData(strLabel);
//		}
//	}
//	chartFile.closeIn();
//	m_MSChartCtrl.SetShowLegend(TRUE);
//}
//
//void CRepGraphView::AcOpDepartCount()
//{
//	// load report from file
//	CString sFileName;
//	if( GetLoadReportType() == load_by_user )
//	{
//		sFileName = GetUserLoadReportFile();
//	}
//	else								// load_by_system
//	{
//		sFileName = ((CTermPlanDoc*)GetDocument())->GetTerminal().GetSimReportManager()->GetCurrentReportFileName( ((CTermPlanDoc*)GetDocument())->m_ProjInfo.path );
//	}
//	chartFile.init (sFileName, READ);
//	//2.)Draw Title,X Axis Title,Y Axis Title,Footnote
//	chartFile.reset();
//	chartFile.seekToLine(0);
//	chartFile.getLine();
//	chartFile.getLine();
//	char string[4][128];
//	chartFile.getField (string[0], 128);
//	chartFile.getField (string[1], 128);
//	chartFile.getField (string[2], 128);
//	chartFile.getField (string[3], 128);
//	CString strFootnote;
//	strFootnote.Format("%s %s %s %s",NoUnderline(CString(string[0])),string[1],string[2],string[3]);
//	
//	SetHeadXYTitle("Count","Time of Day","Number of aircraft operations ",strFootnote);
//	//3.)Draw it	
//	ElapsedTime valuetime,curTime;
//	int interval = GetInterval(string[1]);
//	
//	typedef std::map<int,int>TIMEVALUE;
//	typedef std::map<CString, TIMEVALUE> MAPVALUE;
//	MAPVALUE mapValue;
//	
//	int IntervalNum = 1;
//	char flttype[30];
//	chartFile.reset();
//	chartFile.seekToLine(3);
//	ElapsedTime BeginTime,EndTime;
//	getTime( string[1], BeginTime, EndTime );
//	int nRowNum = (EndTime.asMinutes()-BeginTime.asMinutes())/interval +1; 
//	
//	while (chartFile.getLine())
//	{
//		chartFile.setToField(5);
//		chartFile.getTime(curTime);
//		
//		if( curTime < BeginTime || curTime > EndTime )
//			continue;
//		
//		int index = (curTime.asMinutes() - BeginTime.asMinutes())/interval +1;
//		chartFile.setToField(1);
//		chartFile.getField( flttype, 30 );
//		if(strcmp(flttype, "ARR") != 0)
//		{
//			char sFlightType[512];
//			int iFieldIdx = 13;
//			chartFile.setToField(iFieldIdx);
//			while( chartFile.getField( sFlightType, 512 ) )
//			{
//				CString sKey( sFlightType );
//				MAPVALUE::iterator iter= mapValue.find( sKey );
//				if( iter != mapValue.end() )//exist
//				{
//					mapValue[ sKey ][ index ] ++;
//				}
//				else// first time 
//				{
//					TIMEVALUE tempValue;
//					for( int i=0; i<nRowNum; ++i )
//					{
//						tempValue.insert(TIMEVALUE::value_type(i,0) );
//					}
//					
//					tempValue[ index ] = 1;
//					
//					mapValue.insert( MAPVALUE::value_type( sKey, tempValue ) );
//				}
//				
//				++iFieldIdx;
//				chartFile.setToField(iFieldIdx);
//			}		
//		}
//		
//	}
//	
//	m_MSChartCtrl.SetRowCount(nRowNum);
//	m_MSChartCtrl.SetColumnCount(mapValue.size());
//	m_MSChartCtrl.SetColumnLabelCount(mapValue.size());
//	
//	CString strLabel;
//	for(int i = 1; i <= nRowNum; i++)
//	{
//		ElapsedTime detaTime( interval * (i-1) * 60l );
//		ElapsedTime time = BeginTime + detaTime;
//		strLabel.Format("%s", time.printTime(0));
//		m_MSChartCtrl.SetRow(i);
//		DealRowLabel(strLabel);
//		m_MSChartCtrl.SetRowLabel(strLabel);
//		
//	}
//	
//	MAPVALUE::iterator iter = mapValue.begin();
//	
//	for(int k = 1 ; k <= static_cast<int>(mapValue.size()); k++,++iter)
//	{
//		m_MSChartCtrl.SetColumn(k);
//		m_MSChartCtrl.SetColumnLabel( iter->first );
//		for(i = 1; i <= nRowNum; i++)
//		{
//			m_MSChartCtrl.SetRow(i);
//			strLabel.Format("%d",iter->second[ i] );
//			m_MSChartCtrl.SetData(strLabel);
//		}
//	}
//	chartFile.closeIn();
//	m_MSChartCtrl.SetShowLegend(TRUE);
//}
//void CRepGraphView::AcOpTotal()
//{
//	// load report from file
//	CString sFileName;
//	if( GetLoadReportType() == load_by_user )
//	{
//		sFileName = GetUserLoadReportFile();
//	}
//	else								// load_by_system
//	{
//		sFileName = ((CTermPlanDoc*)GetDocument())->GetTerminal().GetSimReportManager()->GetCurrentReportFileName( ((CTermPlanDoc*)GetDocument())->m_ProjInfo.path );
//	}
//	chartFile.init (sFileName, READ);
//	//2.)Draw Title,X Axis Title,Y Axis Title,Footnote
//	chartFile.reset();
//	chartFile.seekToLine(0);
//	chartFile.getLine();
//	chartFile.getLine();
//	char string[4][128];
//	chartFile.getField (string[0], 128);
//	chartFile.getField (string[1], 128);
//	chartFile.getField (string[2], 128);
//	chartFile.getField (string[3], 128);
//	CString strFootnote;
//	strFootnote.Format("%s %s %s %s",NoUnderline(CString(string[0])),string[1],string[2],string[3]);
//	
//	SetHeadXYTitle("Operations","Time of Day","Number of aircraft operations ",strFootnote);
//	//3.)Draw it	
//	ElapsedTime valuetime,curTime;
//	int interval = GetInterval(string[1]);
//	
//	typedef std::map<int,int>TIMEVALUE;
//	typedef std::map<CString, TIMEVALUE> MAPVALUE;
//	MAPVALUE mapValue;
//
//	ElapsedTime BeginTime,EndTime;
//	getTime( string[1], BeginTime, EndTime );
//	int nRowNum = (EndTime.asMinutes()-BeginTime.asMinutes())/interval +1; 
//	
//	int IntervalNum = 1;
//
//	chartFile.reset();
//	chartFile.seekToLine(3);
//	int index;
//
//	while (chartFile.getLine())
//	{
//		curTime.set(0L);
//		chartFile.setToField(4);//Schedule Arriving Time
//		chartFile.getTime(curTime);
//		if(curTime==0L)
//		{
//			chartFile.setToField(5);//Schedule Departing Time
//			chartFile.getTime(curTime);
//		}
//		
///*
//		if( curTime < BeginTime || curTime > EndTime )
//			continue;
//*/
//		index = (curTime.asMinutes() - BeginTime.asMinutes())/interval +1;		
//		char flttype[30];
//		chartFile.setToField(1);
//		chartFile.getField( flttype, 30 );
//		int iAddCount = 1;
//		/*
//		if(strcmp(flttype, "T/A") == 0)
//		{
//			iAddCount = 2;
//		}
//		*/
//		char sFlightType[512];
//		int iFieldIdx = 1;
//		chartFile.setToField(iFieldIdx);
//		chartFile.getField( sFlightType, 512 );
//		{
//			CString sKey( sFlightType );
//			MAPVALUE::iterator iter= mapValue.find( sKey );
//			if( iter != mapValue.end() )//exist
//			{
//				mapValue[ sKey ][ index ] += iAddCount;
//			}
//			else// first time 
//			{
//				TIMEVALUE tempValue;
//				for( int i=0; i<nRowNum; ++i )
//				{
//					tempValue.insert(TIMEVALUE::value_type(i,0) );
//				}
//				
//				tempValue[ index ] = iAddCount;
//				
//				mapValue.insert( MAPVALUE::value_type( sKey, tempValue ) );
//			}
//			
//		}				
//	}
//	
//	m_MSChartCtrl.SetRowCount(nRowNum);
//	m_MSChartCtrl.SetColumnCount(mapValue.size());
//	m_MSChartCtrl.SetColumnLabelCount(mapValue.size());
//	
//	CString strLabel;
//	for(int i = 1; i <= nRowNum; i++)
//	{
//		ElapsedTime detaTime( interval * (i-1) * 60l );
//		ElapsedTime time = BeginTime + detaTime;
//		time.set(time.asSeconds() % WholeDay);
//		strLabel.Format("%s", time.printTime(0));
//		m_MSChartCtrl.SetRow(i);
//		DealRowLabel(strLabel);
//		m_MSChartCtrl.SetRowLabel(strLabel);
//		
//	}
//	
//	MAPVALUE::iterator iter = mapValue.begin();
//
//	for(int col = 1 ; col <= static_cast<int>(mapValue.size()); col++,++iter)
//	{
//		m_MSChartCtrl.SetColumn(col);
//		m_MSChartCtrl.SetColumnLabel( iter->first );
//		for(int row = 1; row <= nRowNum; row++)
//		{
//			m_MSChartCtrl.SetRow(row);
//					strLabel.Format("%d",iter->second[ row ] );
//					m_MSChartCtrl.SetData(strLabel);
//		}
//	}
//	chartFile.closeIn();
//	m_MSChartCtrl.SetShowLegend(TRUE);
//}
//void CRepGraphView::BagArrivalDistCount()
//{
//	// load report from file
//	CString sFileName;
//	if( GetLoadReportType() == load_by_user )
//	{
//		sFileName = GetUserLoadReportFile();
//	}
//	else								// load_by_system
//	{
//		sFileName = ((CTermPlanDoc*)GetDocument())->GetTerminal().GetSimReportManager()->GetCurrentReportFileName( ((CTermPlanDoc*)GetDocument())->m_ProjInfo.path );
//	}
//	chartFile.init (sFileName, READ);
//	//2.)Draw Title,X Axis Title,Y Axis Title,Footnote
//	chartFile.reset();
//	chartFile.seekToLine(0);
//	chartFile.getLine();
//	chartFile.getLine();
//	char string[4][128];
//	chartFile.getField (string[0], 128);
//	chartFile.getField (string[1], 128);
//	chartFile.getField (string[2], 128);
//	chartFile.getField (string[3], 128);
//	CString strFootnote;
//	strFootnote.Format("%s %s %s %s",NoUnderline(CString(string[0])),string[1],string[2],string[3]);
//	SetHeadXYTitle("Count","Time of Day","Passengers in Area",strFootnote);
//	//3.)Draw it
//	CString strxValue[1000];//The Pax Type Name
//	int nPaxCount=0;
//	CString strTemp;
//	float value = 0.0;
//	ElapsedTime valuetime,curTime;
//	int interval = GetInterval(string[1]);
//	float *yValue = new float[25*60/interval+1];
//	for(int i=0;i<25*60/interval + 1;i++)
//	{
//		yValue[i]=0;
//	}
//
//	chartFile.reset();
//	chartFile.seekToLine(3);
//	
//	int IntervalNum = 1;
//	long num = 0;
//	// add by bird 2003/10/13
//	ElapsedTime BeginTime,EndTime;
//	getTime( string[1], BeginTime, EndTime );
//	int sliceTime = EndTime.asMinutes()-BeginTime.asMinutes();
//	int nRowNum = sliceTime / interval + ( sliceTime%interval ? 1 : 0 ); 
//	
//	while (chartFile.getLine())
//	{
//		chartFile.setToField(2);
//		chartFile.getTime(curTime);
//
//		if( curTime<BeginTime || curTime>EndTime )
//			continue;
//
//		int index = ( curTime.asMinutes() - BeginTime.asMinutes() )/interval +1;
//		chartFile.setToField(4);
//		chartFile.getInteger(num);
//		yValue[index] += num;
//		/*************************************
//		float minutes = curTime.asMinutes();
//		int times = 0;
//		for(int minu_num = interval; ;  )
//		{
//			times++;
//			if(minutes <= minu_num)
//				break;
//			minu_num += interval;
//		}
//		chartFile.setToField(4);
//		chartFile.getInteger(num);
//		yValue[times] += num;
//		*************************************/
//	}
//	
//	m_MSChartCtrl.SetRowCount(nRowNum);
//	m_MSChartCtrl.SetColumnCount(1);
//	m_MSChartCtrl.SetColumnLabelCount(1);
//
//	CString strLabel;
//	ElapsedTime detaTime( interval * 60l );
//	ElapsedTime time( BeginTime );
//	for(i = 1; i <= nRowNum; i++)
//	{
//		CString strBegin = time.printTime(0);
//		time = min( time + detaTime, EndTime );
//		CString strEnd   = time.printTime(0);
//		strLabel.Format("%s~%s", strBegin, strEnd );
//
//		m_MSChartCtrl.SetRow(i);
//		DealRowLabel(strLabel);
//		m_MSChartCtrl.SetRowLabel(strLabel);
//		
//	}
//
//	m_MSChartCtrl.SetColumn(1);
//	m_MSChartCtrl.SetColumnLabel("Arrival Bag Distribution");
//	for(i = 1; i <= nRowNum; i++)
//	{
//		m_MSChartCtrl.SetRow(i);
//		strLabel.Format("%f",yValue[i]);
//		m_MSChartCtrl.SetData(strLabel);
//	}
//	delete[] yValue;
//	chartFile.closeIn();
//	m_MSChartCtrl.SetShowLegend(TRUE);
//}
//
//void CRepGraphView::BagDepartDistCount()
//{
//	// load report from file
//	CString sFileName;
//	if( GetLoadReportType() == load_by_user )
//	{
//		sFileName = GetUserLoadReportFile();
//	}
//	else								// load_by_system
//	{
//		sFileName = ((CTermPlanDoc*)GetDocument())->GetTerminal().GetSimReportManager()->GetCurrentReportFileName( ((CTermPlanDoc*)GetDocument())->m_ProjInfo.path );
//	}
//	chartFile.init (sFileName, READ);	//2.)Draw Title,X Axis Title,Y Axis Title,Footnote
//	chartFile.reset();
//	chartFile.seekToLine(0);
//	chartFile.getLine();
//	chartFile.getLine();
//	char string[4][128];
//	chartFile.getField (string[0], 128);
//	chartFile.getField (string[1], 128);
//	chartFile.getField (string[2], 128);
//	chartFile.getField (string[3], 128);
//	CString strFootnote;
//	strFootnote.Format("%s %s %s %s",NoUnderline(CString(string[0])),string[1],string[2],string[3]);
//	SetHeadXYTitle("Count","Time of Day","Passengers in Area",strFootnote);
//	//3.)Draw it
//	CString strxValue[1000];//The Pax Type Name
//	int nPaxCount=0;
//	CString strTemp;
//	float value = 0.0;
//	ElapsedTime valuetime,curTime;
//	int interval = GetInterval(string[1]);
//	float *yValue = new float[25*60/interval+1];
//	for(int i=0;i<25*60/interval + 1;i++)
//	{
//		yValue[i]=0;
//	}
//
//	chartFile.reset();
//	chartFile.seekToLine(3);
//	
//	int IntervalNum = 1;
//	long num = 0;
//	// add by bird 2003/10/13
//	ElapsedTime BeginTime,EndTime;
//	getTime( string[1], BeginTime, EndTime );
//	int sliceTime = EndTime.asMinutes()-BeginTime.asMinutes();
//	int nRowNum = sliceTime/interval +( sliceTime%interval ? 1 : 0 ); 
//	
//
//	while (chartFile.getLine())
//	{
//		chartFile.setToField(3);
//		chartFile.getTime(curTime);
//
//		if( curTime<BeginTime || curTime>EndTime )
//			continue;
//		
//		int index = ( curTime.asMinutes() - BeginTime.asMinutes() )/interval +1;
//		chartFile.setToField(5);
//		chartFile.getInteger(num);
//		yValue[index] += num;
//		/***************************************
//		float minutes = curTime.asMinutes();
//		int times = 0;
//		for(int minu_num = interval; ;  )
//		{
//			times++;
//			if(minutes <= minu_num)
//				break;
//			minu_num += interval;
//		}
//		chartFile.setToField(5);
//		chartFile.getInteger(num);
//		yValue[times] += num;
//		***************************************/
//	}
//
//	m_MSChartCtrl.SetRowCount(nRowNum);
//	m_MSChartCtrl.SetColumnCount(1);
//	m_MSChartCtrl.SetColumnLabelCount(1);
//	CString strLabel;
//	ElapsedTime detaTime( interval * 60l );
//	ElapsedTime time( BeginTime );
//	for(i = 1; i <= nRowNum; i++)
//	{
//		CString strBegin = time.printTime(0);
//		time = min( time + detaTime, EndTime );
//		CString strEnd   = time.printTime(0);
//		strLabel.Format("%s~%s", strBegin, strEnd );
//
//		m_MSChartCtrl.SetRow(i);
//		DealRowLabel(strLabel);
//		m_MSChartCtrl.SetRowLabel(strLabel);
//		
//	}
//
//	m_MSChartCtrl.SetColumn(1);
//	m_MSChartCtrl.SetColumnLabel("Depart Bag Distribution");
//	for(i = 1; i <= nRowNum; i++)
//	{
//		m_MSChartCtrl.SetRow(i);
//		strLabel.Format("%f",yValue[i]);
//		m_MSChartCtrl.SetData(strLabel);
//	}
//	delete[] yValue;
//	chartFile.closeIn();
//	m_MSChartCtrl.SetShowLegend(TRUE);
//}
//
//void CRepGraphView::PaxDensityM2Pax()
//{
//	//AfxMessageBox("PaxCountDetailed");
//	// load report from file
//	CString sFileName;
//	if( GetLoadReportType() == load_by_user )
//	{
//		sFileName = GetUserLoadReportFile();
//	}
//	else								// load_by_system
//	{
//		sFileName = ((CTermPlanDoc*)GetDocument())->GetTerminal().GetSimReportManager()->GetCurrentReportFileName( ((CTermPlanDoc*)GetDocument())->m_ProjInfo.path );
//	}
//	chartFile.init (sFileName, READ);
//	//2.)Draw Title,X Axis Title,Y Axis Title,Footnote
//	chartFile.reset();
//	chartFile.getLine();
//	chartFile.getLine();
//	char string[4][128];
//	chartFile.getField (string[0], 128);
//	chartFile.getField (string[1], 128);
//	chartFile.getField (string[2], 128);
//	chartFile.getField (string[3], 128);
//	CString strFootnote;
//	strFootnote.Format("%s %s %s %s",NoUnderline(CString(string[0])),string[1],string[2],string[3]);
//	SetHeadXYTitle("m2/Pax","Time of Day","Square Meters per Passenger",strFootnote);
//	//3.)Draw it
//	CString strxValue[1000];//The Pax Type Name
//	int nPaxCount=0;
//	char buf[128];
//	CString strTemp;
//	float value = 0.0;
//	ElapsedTime valuetime;
//	int hours,minutes;
//	float **yValue;
//	yValue = new float*[1000];
//	int interval = GetInterval(string[1]);
//	for(int k = 0; k < 1000; k++)
//	{
//		yValue[k] = new float[25*60/interval];
//	}
//	for(k=0;k<1000;k++)
//	{
//		for(int i=0;i<25*60/interval;i++)
//		{
//			yValue[k][i]=0;
//		}
//	}
//	
//	int i=0;
//	chartFile.reset();
//	chartFile.skipLines(3);
//
//	if(!chartFile.getLine())
//	{
//		chartFile.closeIn();
//		return;
//	}
//	chartFile.setToField(0);
//	chartFile.getField(buf,128);
//	nPaxCount+=1;
//	strxValue[nPaxCount].Format("%s",buf);//Get The First Pax Type
//	
//	int IntervalNum = 1;
//	chartFile.reset();
//	chartFile.skipLines(3);
//	while (chartFile.getLine())
//	{
//		chartFile.setToField(0);
//		chartFile.getField(buf,128);
//		strTemp.Format("%s",buf);
//		if(strxValue[nPaxCount]!=strTemp)
//		{
//			nPaxCount++;
//			strxValue[nPaxCount]=strTemp;
//			IntervalNum = 1;
//		}
//
//		chartFile.setToField (4);
//		chartFile.getFloat (value);
//		chartFile.setToField(1);
//		chartFile.getTime (valuetime, TRUE);
//		hours=(int)valuetime.asHours();
//		minutes=(int)valuetime.asMinutes();
//		
//		// TRACE("the time is %d hours,or %d minutes\n",hours,minutes);
//		yValue[nPaxCount][IntervalNum++]=value;	
//	}
//	
//	// add by bird 2003/10/13
//	ElapsedTime BeginTime,EndTime;
//	getTime( string[1], BeginTime, EndTime );
//	int nRowNum = (EndTime.asMinutes()-BeginTime.asMinutes())/interval +1; 
//
//	m_MSChartCtrl.SetRowCount(nRowNum);
//	m_MSChartCtrl.SetColumnCount(nPaxCount);
//	m_MSChartCtrl.SetColumnLabelCount(nPaxCount);
//	
//	CString strLabel;
//	for(i = 1; i <= nRowNum; i++)
//	{
//		ElapsedTime detaTime( interval * (i-1) * 60l );
//		ElapsedTime time = BeginTime + detaTime;
//		strLabel.Format("%s", time.printTime(0));
//
//		m_MSChartCtrl.SetRow(i);
//		DealRowLabel(strLabel);
//		m_MSChartCtrl.SetRowLabel(strLabel);
//		
//	}
//
//	for(k = 1 ; k <= nPaxCount; k++)
//	{
//		m_MSChartCtrl.SetColumn(k);
//		m_MSChartCtrl.SetColumnLabel(strxValue[k]);
//		for(i = 1; i <= nRowNum; i++)
//		{
//			m_MSChartCtrl.SetRow(i);
//			strLabel.Format("%f",yValue[k][i]);
//			m_MSChartCtrl.SetData(strLabel);
//		}
//	}
//
//	for(k = 0; k < 1000; k++)
//	{
//		delete[] yValue[k];
//	}
//	delete[] yValue;
//
//	chartFile.closeIn();
//	m_MSChartCtrl.SetShowLegend(TRUE);
//}
//
//
//void CRepGraphView::BagDeliveryCount()
//{
//	// load report from file
//	CString sFileName;
//	if( GetLoadReportType() == load_by_user )
//	{
//		sFileName = GetUserLoadReportFile();
//	}
//	else								// load_by_system
//	{
//		sFileName = ((CTermPlanDoc*)GetDocument())->GetTerminal().GetSimReportManager()->GetCurrentReportFileName( ((CTermPlanDoc*)GetDocument())->m_ProjInfo.path );
//	}
//	chartFile.init (sFileName, READ);
////	if (chartFile.countLines() <= 5)
////		return;
//	//2.)Draw Title,X Axis Title,Y Axis Title,Footnote
//	chartFile.reset();
//	chartFile.getLine();
//	chartFile.getLine();
//	char string[4][128];
//	chartFile.getField (string[0], 128);
//	chartFile.getField (string[1], 128);
//	chartFile.getField (string[2], 128);
//	chartFile.getField (string[3], 128);
//	CString strFootnote;
//	strFootnote.Format("%s %s %s %s",NoUnderline(CString(string[0])),string[1],string[2],string[3]);
//	SetHeadXYTitle("Baggage Delivery Time(Detailed)","Minutes Delivery","Bag Count",strFootnote);
//	//3.)Draw it
//	chartFile.reset();
//	chartFile.skipLines(3);
//	int xValue[8];
//	int yValue[8];
//	for(int i=0;i<8;i++)
//	{
//		xValue[i]=yValue[i]=0;
//	}
//	int maxXValue=0;
//	int minXValue=1440;// max minutes in 1 day
//	float ft = 0.0;   
//	ElapsedTime value;
//	while (chartFile.getLine())
//	{
//		chartFile.setToField (3);
//		chartFile.getTime (value, TRUE);
//		if (value.asMinutes() < minXValue)
//			minXValue = value.asMinutes();
//		if (value.asMinutes() > maxXValue)
//			maxXValue = value.asMinutes();
//	}
//	minXValue=minXValue-(minXValue%10);
//	maxXValue=maxXValue-(maxXValue%10)+10;
//	int nScale;//
//	int n=(maxXValue-minXValue)%70;//70=10*7;  7 Column
//	if(n==0)
//	{
//		nScale=(maxXValue-minXValue)/70;
//	}
//	else
//	{
//		nScale=(maxXValue-minXValue)/70+1;
//	}
//	
//	xValue[0]=minXValue;
//	for(i=1;i<=7;i++)
//	{
//		xValue[i]=xValue[i-1]+10*nScale;
//	}
//	
//	chartFile.reset();
//	chartFile.skipLines(3);
//	while (chartFile.getLine())
//	{
//		chartFile.setToField (3);
//		chartFile.getTime (value, TRUE);
//		for(i=1;i<=7;i++)
//		{
//			if(value.asMinutes()>=xValue[i-1] && value.asMinutes()<xValue[i])
//			{
//				yValue[i]++;
//			}
//		}
//	}
//	int nCount=7;
//	for(i=7;i>0;i--)
//	{
//		if(yValue[i]==0)
//		{
//			nCount--;
//		}
//		else
//		{
//			break;
//		}
//	}
//	m_MSChartCtrl.SetRowCount(nCount);
//	m_MSChartCtrl.SetColumnCount(1);
//	m_MSChartCtrl.SetColumnLabelCount(1);
//	m_MSChartCtrl.SetColumn(1);
//	m_MSChartCtrl.SetColumnLabel("Meters Travelled");
//	
//	CString strLabel;
//	for(i=1;i<=nCount;i++)
//	{
//		m_MSChartCtrl.SetRow(i);
//		strLabel.Format("%d-%d",(int)xValue[i-1],(int)xValue[i]);
//		DealRowLabel(strLabel);
//		m_MSChartCtrl.SetRowLabel(strLabel);
//		
//		strLabel.Format("%d",yValue[i]);
//		m_MSChartCtrl.SetData(strLabel);
//	}
//	chartFile.closeIn();
//	m_MSChartCtrl.SetShowLegend(FALSE);
//}
//
//void CRepGraphView::BagDeliverySummary()
//{
//	// load report from file
//	CString sFileName;
//	if( GetLoadReportType() == load_by_user )
//	{
//		sFileName = GetUserLoadReportFile();
//	}
//	else								// load_by_system
//	{
//		sFileName = ((CTermPlanDoc*)GetDocument())->GetTerminal().GetSimReportManager()->GetCurrentReportFileName( ((CTermPlanDoc*)GetDocument())->m_ProjInfo.path );
//	}
//	chartFile.init (sFileName, READ);
//	//2.)Draw Title,X Axis Title,Y Axis Title,Footnote
//	chartFile.reset();
//	chartFile.getLine();
//	chartFile.getLine();
//	char string[4][128];
//	chartFile.getField (string[0], 128);
//	chartFile.getField (string[1], 128);
//	chartFile.getField (string[2], 128);
//	chartFile.getField (string[3], 128);
//	CString strFootnote;
//	strFootnote.Format("%s %s %s %s",NoUnderline(CString(string[0])),string[1],string[2],string[3]);
//	SetHeadXYTitle("Delivery Time(Summary)","Passenger Type","Minutes Delivery",strFootnote);
//	//3.)Draw it
//	CString strxValue[100];//The Pax Type Name
//	int nPaxCount=0;
//	ElapsedTime valuetime;
//	float minvalue[100];
//	float avevalue[100];
//	float maxvalue[100];
//
//	float Q1value[100];
//	float Q2value[100];
//	float Q3value[100];
//	
//	float P1value[100];
//	float P5value[100];
//	float P10value[100];
//	
//	float P90value[100];
//	float P95value[100];
//	float P99value[100];
//	
//	float Sigmavalue[100];
//	
//	char buf[128];
//	for(int k=0;k<100;k++)
//	{
//		minvalue[k]=avevalue[k]=maxvalue[k]=0;
//		Q1value[k]=Q2value[k]=Q3value[k]=P1value[k]=P5value[k]=0;
//		Sigmavalue[k] = P10value[k]=P95value[k]=P99value[k]=0;
//	}
//	
//	
//	chartFile.reset();
//	chartFile.skipLines(3);
//	while (chartFile.getLine())
//	{
//		chartFile.setToField(0);
//		chartFile.getField(buf,128);
//		nPaxCount+=1;
//		strxValue[nPaxCount].Format("%s",buf);//Get The First Pax Type
//		
//		chartFile.setToField (1);
//		chartFile.getTime (valuetime, TRUE);
//		minvalue[nPaxCount]=static_cast<float>(valuetime.asMinutes());
//		chartFile.setToField (2);
//		chartFile.getTime (valuetime, TRUE);
//		avevalue[nPaxCount]=static_cast<float>(valuetime.asMinutes());
//		chartFile.setToField (3);
//		chartFile.getTime (valuetime, TRUE);
//		maxvalue[nPaxCount]=static_cast<float>(valuetime.asMinutes());
//
//			chartFile.setToField( 5 );
//		chartFile.getTime( valuetime, TRUE );
//		Q1value[nPaxCount]= static_cast<float>(valuetime.asMinutes());
//		
//		chartFile.setToField( 6 );
//		chartFile.getTime( valuetime, TRUE );
//		Q2value[nPaxCount]= static_cast<float>(valuetime.asMinutes());
//		
//		chartFile.setToField( 7 );
//		chartFile.getTime( valuetime, TRUE );
//		Q3value[nPaxCount]= static_cast<float>(valuetime.asMinutes());
//		
//		chartFile.setToField( 8 );
//		chartFile.getTime( valuetime, TRUE );
//		P1value[nPaxCount]= static_cast<float>(valuetime.asMinutes());
//		
//		chartFile.setToField( 9 );
//		chartFile.getTime( valuetime, TRUE );
//		P5value[nPaxCount]= static_cast<float>(valuetime.asMinutes());
//		
//		chartFile.setToField( 10 );
//		chartFile.getTime( valuetime, TRUE );
//		P10value[nPaxCount]= static_cast<float>(valuetime.asMinutes());
//		
//		chartFile.setToField( 11 );
//		chartFile.getTime( valuetime, TRUE );
//		P90value[nPaxCount]= static_cast<float>(valuetime.asMinutes());
//		
//		chartFile.setToField( 12 );
//		chartFile.getTime( valuetime, TRUE );
//		P95value[nPaxCount]= static_cast<float>(valuetime.asMinutes());
//		
//		chartFile.setToField( 13 );
//		chartFile.getTime( valuetime, TRUE );
//		P99value[nPaxCount]= static_cast<float>(valuetime.asMinutes());
//		
//		chartFile.setToField( 14 );
//		chartFile.getTime( valuetime, TRUE );
//		Sigmavalue[nPaxCount]= static_cast<float>(valuetime.asMinutes());
//	}
//	
//	CString strLabel;
//	m_MSChartCtrl.SetRowCount(nPaxCount);
//	m_MSChartCtrl.SetColumnCount(13);
//	m_MSChartCtrl.SetColumnLabelCount(13);
//	m_MSChartCtrl.SetColumn(1);
//	m_MSChartCtrl.SetColumnLabel("Minimum");
//	m_MSChartCtrl.SetColumn(2);
//	m_MSChartCtrl.SetColumnLabel("Maximum");
//	m_MSChartCtrl.SetColumn(3);
//	m_MSChartCtrl.SetColumnLabel("Average");
//
//	m_MSChartCtrl.SetColumn(4);
//	m_MSChartCtrl.SetColumnLabel("Q1");
//	
//	m_MSChartCtrl.SetColumn(5);
//	m_MSChartCtrl.SetColumnLabel("Q2");
//	
//	m_MSChartCtrl.SetColumn(6);
//	m_MSChartCtrl.SetColumnLabel("Q3");
//	
//	m_MSChartCtrl.SetColumn(7);
//	m_MSChartCtrl.SetColumnLabel("P1");
//	
//	m_MSChartCtrl.SetColumn(8);
//	m_MSChartCtrl.SetColumnLabel("P5");
//	
//	m_MSChartCtrl.SetColumn(9);
//	m_MSChartCtrl.SetColumnLabel("P10");
//	
//	m_MSChartCtrl.SetColumn(10);
//	m_MSChartCtrl.SetColumnLabel("P90");
//	
//	m_MSChartCtrl.SetColumn(11);
//	m_MSChartCtrl.SetColumnLabel("P95");
//	
//	m_MSChartCtrl.SetColumn(12);
//	m_MSChartCtrl.SetColumnLabel("P99");
//	
//	m_MSChartCtrl.SetColumn(13);
//	m_MSChartCtrl.SetColumnLabel("Sigma");
//
//	for(int i=1;i<=nPaxCount;i++)
//	{
//		m_MSChartCtrl.SetRow(i);
//		DealRowLabel(strxValue[i]);
//		m_MSChartCtrl.SetRowLabel(strxValue[i]);
//		
//		m_MSChartCtrl.SetColumn(1);
//		strLabel.Format("%f",minvalue[i]);
//		m_MSChartCtrl.SetData(strLabel);
//		m_MSChartCtrl.SetRow(i);	//add by bird 2003/2/18
//		m_MSChartCtrl.SetColumn(2);
//		strLabel.Format("%f",avevalue[i]);
//		m_MSChartCtrl.SetData(strLabel);
//		m_MSChartCtrl.SetRow(i);	//add by bird 2003/2/18
//		m_MSChartCtrl.SetColumn(3);
//		strLabel.Format("%f",maxvalue[i]);
//		m_MSChartCtrl.SetData(strLabel);
//
//
//		m_MSChartCtrl.SetRow(i);
//		m_MSChartCtrl.SetColumn(4);
//		strLabel.Format("%f",Q1value[i]);
//		m_MSChartCtrl.SetData(strLabel);
//		
//		m_MSChartCtrl.SetRow(i);
//		m_MSChartCtrl.SetColumn(5);
//		strLabel.Format("%f",Q2value[i]);
//		m_MSChartCtrl.SetData(strLabel);
//		
//		m_MSChartCtrl.SetRow(i);
//		m_MSChartCtrl.SetColumn(6);
//		strLabel.Format("%f",Q3value[i]);
//		m_MSChartCtrl.SetData(strLabel);
//		
//		m_MSChartCtrl.SetRow(i);
//		m_MSChartCtrl.SetColumn(7);
//		strLabel.Format("%f",P1value[i]);
//		m_MSChartCtrl.SetData(strLabel);
//		
//		m_MSChartCtrl.SetRow(i);
//		m_MSChartCtrl.SetColumn(8);
//		strLabel.Format("%f",P5value[i]);
//		m_MSChartCtrl.SetData(strLabel);
//		
//		m_MSChartCtrl.SetRow(i);
//		m_MSChartCtrl.SetColumn(9);
//		strLabel.Format("%f",P10value[i]);
//		m_MSChartCtrl.SetData(strLabel);
//		
//		m_MSChartCtrl.SetRow(i);
//		m_MSChartCtrl.SetColumn(10);
//		strLabel.Format("%f",P90value[i]);
//		m_MSChartCtrl.SetData(strLabel);
//		
//		m_MSChartCtrl.SetRow(i);
//		m_MSChartCtrl.SetColumn(11);
//		strLabel.Format("%f",P95value[i]);
//		m_MSChartCtrl.SetData(strLabel);
//		
//		m_MSChartCtrl.SetRow(i);
//		m_MSChartCtrl.SetColumn(12);
//		strLabel.Format("%f",P99value[i]);
//		m_MSChartCtrl.SetData(strLabel);
//		
//		m_MSChartCtrl.SetRow(i);
//		m_MSChartCtrl.SetColumn(13);
//		strLabel.Format("%f",Sigmavalue[i]);
//		m_MSChartCtrl.SetData(strLabel);
//	}
//	
//	chartFile.closeIn();
//	m_MSChartCtrl.SetShowLegend(TRUE);
//}

//CString CRepGraphView::NoUnderline(CString _str)
//{
//	_str.Replace("_"," ");
//	return _str;
//}

//void CRepGraphView::OnContextMenu(CWnd* pWnd, CPoint point) 
//{
//	// TODO: Add your message handler code here
//	//AfxMessageBox("Test");
//	CMenu menuPopup;
//	menuPopup.LoadMenu( IDR_MENU_POPUP );
//	CMenu* pMenu = menuPopup.GetSubMenu(6 );
//	
//	pMenu->TrackPopupMenu(TPM_LEFTALIGN, point.x, point.y, this);
//	
//}

void CRepGraphView::OnGraphviewMaximax() 
{
	// TODO: Add your command handler code here
	CRect rectMax;
	GetParentFrame()->GetClientRect(&rectMax);
	rectMax.top+=2;
	MoveWindow(&rectMax);
}

void CRepGraphView::OnGraphviewNormal() 
{
	// TODO: Add your command handler code here
	CRect rectFrame,rectListView,rectGraphView;
	GetParentFrame()->GetClientRect(&rectFrame);

	CTermPlanDoc* pDoc = (CTermPlanDoc*)(GetDocument());
	CView * pTempView;
	POSITION pos = pDoc->GetFirstViewPosition();
	while (pos != NULL)
	{
		pTempView = pDoc->GetNextView(pos);
		if(pTempView->IsKindOf(RUNTIME_CLASS(CRepListView)))
		{
			pTempView->GetWindowRect(&rectListView);
			rectGraphView.left=rectListView.right+5;
			rectGraphView.right=rectFrame.right;
			rectGraphView.top=rectFrame.top+2;
			rectGraphView.bottom=rectFrame.bottom;
		}
	}
	MoveWindow(&rectGraphView);
}


//int CRepGraphView::GetInterval(char *time)
//{
//	char interval[20];
//	int interval_min;
//	int i = 0;
//
//	while(*time != ';')
//	{
//		time++;
//	}
//	time++;
//	while(*time != ';')
//	{
//		time++;
//	}
//	time++;
//	while(*time != ':')
//	{
//		*(interval+i) = *time++;
//		i++;
//	}
//	time++;
//	*(interval+i) = '\0';
//	interval_min = atoi(interval) * 60;
//	i = 0;
//	while(*time != '\0')
//	{
//		*(interval+i) = *time++;
//		i++;
//	}
//	*(interval+i) = '\0';
//	interval_min += atoi(interval);
//	return interval_min;
//
//}
//
//void CRepGraphView::getTime( const CString& strText, ElapsedTime& begineTime, ElapsedTime& endTime ) const
//{
//	int pos1 = strText.Find(';');
//
//	CString strTime = strText.Mid(0,pos1);
//	begineTime.SetTime( strTime );
//
//	int pos2 = strText.Find(';', pos1+1 );
//	strTime = strText.Mid( pos1+1, pos2-pos1-1 );
//	endTime.SetTime( strTime );
//
//	// TRACE("begin time: %s\r\n",begineTime.printTime() );
//	// TRACE("End time: %s\r\n",endTime.printTime() );
//}

//void CRepGraphView::DealRowLabel(CString &strRowLabel)
//{
//  int nStrLen=strRowLabel.GetLength();
//  if (nStrLen>16)
//  {
//	  strRowLabel=strRowLabel.Left(13);
//	  strRowLabel=strRowLabel+"...";
//  }
//
//}

void CRepGraphView::ShowReportProcessFlow(MathResult& result)
{
	if(m_pRepGraphViewOperator)
		m_pRepGraphViewOperator->ShowReportProcessFlow(result);
//	int nRowNum = result.m_vResult.size();
////	nRowNum = 5;//test
//	int nColCount = result.m_vstrProcess.size();
////	nPaxCount = 2;//test
//
//	
//
//	if(nRowNum <= 0 || nColCount <= 0)
//	{
//		m_MSChartCtrl.ShowWindow(SW_HIDE);
//		return ;
//	}
//
//	m_MSChartCtrl.SetChartType(1);
//
//	m_MSChartCtrl.ShowWindow(SW_SHOW);
//
//	m_MSChartCtrl.SetRowCount(nRowNum);
//	m_MSChartCtrl.SetColumnCount(nColCount);
//	m_MSChartCtrl.SetColumnLabelCount(nColCount);
//	
//	CString strLabel = "";
//
//	int i = 0, j = 0;
//	for(i = 1; i <= nRowNum; i++)
//	{
//		m_MSChartCtrl.SetRow(i);
//		ElapsedTime t = result.m_vResult.at(i-1).time;
//		t.set(t.asSeconds()%WholeDay);
//        strLabel.Format("%02d:%02d", t.asHours(), t.asMinutes()%60);
////	//	strLabel = "hello";//test
//		DealRowLabel(strLabel);
//		m_MSChartCtrl.SetRowLabel(strLabel);
//	}
//	
//	for(i = 1 ; i <= nColCount; i++)
//	{
//		m_MSChartCtrl.SetColumn(i);
//		std::vector<std::string>& vString = result.m_vstrProcess;
//		strLabel = (CString)vString.at(i-1).c_str();
////		strLabel = "Col";
//		m_MSChartCtrl.SetColumnLabel(strLabel);
//		for(j = 1; j <= nRowNum; j++)
//		{
//			m_MSChartCtrl.SetRow(j);
//			strLabel.Format("%f", result.m_vResult.at(j-1).vProcResults.at(i-1));
//			// TRACE("mschart data: %f\n", result.m_vResult.at(j-1).vProcResults.at(i-1));
////			strLabel.Format("%f", 1.0+k);
//			m_MSChartCtrl.SetData(strLabel);
//		}
//	}
//
//	m_MSChartCtrl.SetShowLegend(TRUE);
//
//	m_MSChartCtrl.Refresh();
}

void CRepGraphView::LoadReportProcessFlow(CMathResultManager *pManager, enum ENUM_REPORT_TYPE _enumRepType)
{	
	if(m_pRepGraphViewOperator)
		m_pRepGraphViewOperator->LoadReportProcessFlow(pManager,_enumRepType);
///*	case ENUM_QUEUETIME_REP:
//	case ENUM_QUEUELENGTH_REP:
//	case ENUM_THROUGHPUT_REP:
//	case ENUM_PASSENGERNO_REP:
//	case ENUM_UTILIZATION_REP:
//	*/
//
//	if(pManager == NULL)
//		return ;
//
//
//	switch(_enumRepType)
//	{
//	case ENUM_QUEUETIME_REP:
//		{
//			SetHeadXYTitle("Queue Time","Time of Day","Queue Time (Minutes)","");
//			MathResult& result = pManager->GetQTimeResult();
//			ShowReportProcessFlow(result);
//			break;
//		}
//	case ENUM_QUEUELENGTH_REP:
//		{
//			SetHeadXYTitle("Queue Length","Time of Day","Queue Length","");
//			//SetHeadXYTitle("Queue Length","Time of Day","Queue Length History","");
//			MathResult& result = pManager->GetQLengthResult();
//			ShowReportProcessFlow(result);
//			break;
//		}
//	case ENUM_THROUGHPUT_REP:
//		{
//			SetHeadXYTitle("Processor Throughput","Time of Day","Number of Passenger","");
//			MathResult& result = pManager->GetThroughputResult();
//			ShowReportProcessFlow(result);
//			break;
//		}
//	case ENUM_PASSENGERNO_REP:
//		{
//			SetHeadXYTitle("Passenger Count","Time of Day","Number of Passenger","");
//			MathResult& result = pManager->GetPaxCountResult();
//			ShowReportProcessFlow(result);
//			break;
//		}
//	case ENUM_UTILIZATION_REP:
//		{
//			SetHeadXYTitle("Utilization","Time of Day","Utilization(%)","");
//			MathResult& result = pManager->GetUtilizationResult();
//			ShowReportProcessFlow(result);
//			break;
//		}
//	default:
//		break;
//	}
}
//void CRepGraphView::BillboardExposureIncidenceDetailed()
//{
//	// load report from file
//	CString sFileName;
//	if( GetLoadReportType() == load_by_user )
//	{
//		sFileName = GetUserLoadReportFile();
//	}
//	else								// load_by_system
//	{
//		sFileName = ((CTermPlanDoc*)GetDocument())->GetTerminal().GetSimReportManager()->GetCurrentReportFileName( ((CTermPlanDoc*)GetDocument())->m_ProjInfo.path );
//	}
//	chartFile.init (sFileName, READ);
//
//
//	//2.)Draw Title,X Axis Title,Y Axis Title,Footnote
//	chartFile.reset();
//	chartFile.seekToLine(0);
//	chartFile.getLine();
//	chartFile.getLine();
//	char string[4][128];
//	chartFile.getField (string[0], 128);
//	chartFile.getField (string[1], 128);
//	chartFile.getField (string[2], 128);
//	chartFile.getField (string[3], 128);
//	CString strFootnote;
//	strFootnote.Format("%s %s %s %s",NoUnderline(CString(string[0])),string[1],string[2],string[3]);
//	SetHeadXYTitle("Billboard Incidence (Detailed)","Time of Day","Units Passed",strFootnote);
//
//	ElapsedTime eBegin;
//	ElapsedTime eEnd;
//	getTime( string[1], eBegin, eEnd );
//	int interval = GetInterval(string[1]);	//interval, unit: minute
//	int nColNum = ( eEnd.asMinutes() - eBegin.asMinutes())/interval + 1; 
//
//	//3.)Draw it
//	int nProcCount=0;
//	CString strxValue[1000];//The Proc Name
//	char buf[128];
//	CString strTemp = "";
//	int hours = 0, minutes = 0;
//	float value = 0.0;
//	float TotalValue = 0.0;
//	ElapsedTime valuetime;
//	float **yValue;
//	yValue = new float*[1000];
//	for(int k = 0; k < 1000; k++)
//	{
//		yValue[k] = new float[nColNum];
//	}
//	for(k=0;k<1000;k++)
//	{
//		for(int i=0;i<nColNum;i++)
//		{
//			yValue[k][i]=0.0;
//		}
//	}
//
//	float **yTotalValue;
//	yTotalValue = new float*[1000];
//	for(int k = 0; k < 1000; k++)
//	{
//		yTotalValue[k] = new float[nColNum];
//	}
//	for(k=0;k<1000;k++)
//	{
//		for(int i=0;i<nColNum;i++)
//		{
//			yTotalValue[k][i]=0.0;
//		}
//	}
//
//	int i=0;
//	chartFile.reset();
//	chartFile.skipLines(3);
//
//	if(!chartFile.getLine())
//	{
//		chartFile.closeIn();
//		return;
//	}
//	chartFile.setToField(0);
//	chartFile.getField(buf,128);
//	nProcCount++;
//	strxValue[nProcCount].Format("%s",buf);//Get The First Pax Type
//
//	int IntervalNum = 1;
//	chartFile.reset();
//	chartFile.skipLines(3);
//	while (chartFile.getLine())
//	{
//		chartFile.setToField(0);
//		chartFile.getField(buf,128);
//		strTemp.Format("%s",buf);
//		if(strxValue[nProcCount]!=strTemp)
//		{
//			nProcCount++;
//			strxValue[nProcCount]=strTemp;
//			IntervalNum = 1;
//		}
//
//		chartFile.setToField (3);
//		chartFile.getFloat (value);
//		//chartFile.setToField(1);
//		//chartFile.getTime (valuetime, TRUE);
//		//hours=(int)valuetime.asHours();
//		//minutes=(int)valuetime.asMinutes();
//
//		//		// TRACE("the time is %d hours,or %d minutes\n",hours,minutes);
//		yValue[nProcCount][IntervalNum]=value;	
//
//		chartFile.getFloat(TotalValue);
//		yTotalValue[nProcCount][IntervalNum]=TotalValue;
//
//		IntervalNum = IntervalNum + 1;
//	}
//
//	// add by bird 2003/10/13
//	ElapsedTime BeginTime,EndTime;
//	getTime( string[1], BeginTime, EndTime );
//	int nRowNum = (EndTime.asMinutes()-BeginTime.asMinutes())/interval; 
//
//	m_MSChartCtrl.SetRowCount(nRowNum);
//	m_MSChartCtrl.SetColumnCount(nProcCount*2);
//	m_MSChartCtrl.SetColumnLabelCount(nProcCount*2);
//
//	CString strLabel;
//
//	ElapsedTime detaTime( interval * 60l );
//	ElapsedTime time( BeginTime );
//	for(i = 1; i <= nRowNum; i++)
//	{
//		time.set(time.asSeconds() % WholeDay);
//		CString strBeginTime = time.printTime(0);
//		time += detaTime;
//		time.set(time.asSeconds() % WholeDay);
//		CString strEndTime	 = time.printTime(0);
//
//		strLabel.Format( "%s~%s", strBeginTime, strEndTime );
//		m_MSChartCtrl.SetRow(i);
//		DealRowLabel(strLabel);
//		m_MSChartCtrl.SetRowLabel(strLabel);
//
//	}
//
//	for(k = 1 ; k <= nProcCount; k++)
//	{
//		//total
//		m_MSChartCtrl.SetColumn( 2*k -1);
//		CString strColumnLabel = "";
//		strColumnLabel.Format("%s Total",strxValue[k]);
//		m_MSChartCtrl.SetColumnLabel(strColumnLabel);
//
//		for(i = 1; i <= nRowNum; i++)
//		{
//			m_MSChartCtrl.SetRow(i);
//			strLabel.Format("%f",yTotalValue[k][i]);
//			m_MSChartCtrl.SetData(strLabel);
//		}
//
//		//exposure incidence
//		m_MSChartCtrl.SetColumn(2*k);
//		strColumnLabel.Format("%s Exposure",strxValue[k]);
//		m_MSChartCtrl.SetColumnLabel(strColumnLabel);
//
//		for(i = 1; i <= nRowNum; i++)
//		{
//			m_MSChartCtrl.SetRow(i);
//			strLabel.Format("%f",yValue[k][i]);
//			m_MSChartCtrl.SetData(strLabel);
//		}
//	}
//
//	for(k = 0; k < 1000; k++)
//	{
//		delete[] yValue[k];
//		delete[] yTotalValue[k];
//	}
//	delete[] yValue;
//	delete[] yTotalValue;
//	chartFile.closeIn();
//	m_MSChartCtrl.SetShowLegend(TRUE);
//
//}
//
//void CRepGraphView::BillboardExposureIncidenceSummary()
//{
//	CString sFileName;
//	if( GetLoadReportType() == load_by_user )
//	{
//		sFileName = GetUserLoadReportFile();
//	}
//	else								// load_by_system
//	{
//		sFileName = ((CTermPlanDoc*)GetDocument())->GetTerminal().GetSimReportManager()->GetCurrentReportFileName( ((CTermPlanDoc*)GetDocument())->m_ProjInfo.path );
//	}
//	chartFile.init (sFileName, READ);
//	//2.)Draw Title,X Axis Title,Y Axis Title,Footnote
//	chartFile.reset();
//	chartFile.getLine();
//	chartFile.getLine();
//	char string[4][128];
//	chartFile.getField (string[0], 128);
//	chartFile.getField (string[1], 128);
//	chartFile.getField (string[2], 128);
//	chartFile.getField (string[3], 128);
//	CString strFootnote;
//	strFootnote.Format("%s %s %s %s",NoUnderline(CString(string[0])),string[1],string[2],string[3]);
//	SetHeadXYTitle("Pax Exposure Per Hour","Processor","Passengers Passed Per Hour",strFootnote);
//	//3.)Draw it
//	CString strxValue[1000];//The Proc Type Name
//	int nPaxCount=0;
//	float yValue[1000];
//	char buf[128];
//	CString strTemp;
//	float value = 0.0;
//	for(int k=0;k<1000;k++)
//	{
//		yValue[k]=0;	
//	}
//
//	float yTotalValue[1000];
//	for(int k=0;k<1000;k++)
//	{
//		yTotalValue[k]=0;	
//	}
//	int i=0;
//	chartFile.reset();
//	chartFile.skipLines(3);
//
//	while (chartFile.getLine())
//	{
//		chartFile.setToField(0);
//		chartFile.getField(buf,128);
//		strTemp.Format("%s",buf);
//		if(strxValue[nPaxCount]!=strTemp)
//		{
//			nPaxCount++;
//			strxValue[nPaxCount]=strTemp;
//		}
//
////		chartFile.setToField (4);
//		chartFile.getFloat (value);
//		yTotalValue[nPaxCount]=value;
//
//		chartFile.getFloat (value);
//		yValue[nPaxCount]=value;
//	}
//
//	m_MSChartCtrl.SetRowCount(nPaxCount *2);
//	m_MSChartCtrl.SetColumnCount(1);
//	m_MSChartCtrl.SetColumn(1);
//	m_MSChartCtrl.SetColumnLabel("Processor Type");
//
//	CString strLabel;
//	for(i=1;i<=nPaxCount;i++)
//	{
//		m_MSChartCtrl.SetRow(2*i -1);
//		strLabel.Format("%s Total",strxValue[i]);
//		DealRowLabel(strLabel);
//		m_MSChartCtrl.SetRowLabel(strLabel);
//
//		strLabel.Format("%f",yTotalValue[i]);
//		m_MSChartCtrl.SetData(strLabel);
//
//		m_MSChartCtrl.SetRow(2*i);
//		strLabel.Format("%s Exposure",strxValue[i]);
//		DealRowLabel(strLabel);
//		m_MSChartCtrl.SetRowLabel(strLabel);
//
//		strLabel.Format("%f",yValue[i]);
//		m_MSChartCtrl.SetData(strLabel);
//
//	}
//	chartFile.closeIn();
//	m_MSChartCtrl.SetShowLegend(FALSE);
//}
//
//void CRepGraphView::BillboardLinkedProcIncrementalVisitDetailed()
//{
//	// load report from file
//	CString sFileName;
//	if( GetLoadReportType() == load_by_user )
//	{
//		sFileName = GetUserLoadReportFile();
//	}
//	else								// load_by_system
//	{
//		sFileName = ((CTermPlanDoc*)GetDocument())->GetTerminal().GetSimReportManager()->GetCurrentReportFileName( ((CTermPlanDoc*)GetDocument())->m_ProjInfo.path );
//	}
//	chartFile.init (sFileName, READ);
//
//
//	//2.)Draw Title,X Axis Title,Y Axis Title,Footnote
//	chartFile.reset();
//	chartFile.seekToLine(0);
//	chartFile.getLine();
//	chartFile.getLine();
//	char string[4][128];
//	chartFile.getField (string[0], 128);
//	chartFile.getField (string[1], 128);
//	chartFile.getField (string[2], 128);
//	chartFile.getField (string[3], 128);
//	CString strFootnote;
//	strFootnote.Format("%s %s %s %s",NoUnderline(CString(string[0])),string[1],string[2],string[3]);
//	SetHeadXYTitle("Billboard Incremental Visit (Detailed)","Time of Day","Units Served",strFootnote);
//
//	ElapsedTime eBegin;
//	ElapsedTime eEnd;
//	getTime( string[1], eBegin, eEnd );
//	int interval = GetInterval(string[1]);	//interval, unit: minute
//	int nColNum = ( eEnd.asMinutes() - eBegin.asMinutes())/interval + 1; 
//
//	//3.)Draw it
//	int nProcCount=0;
//	CString strxValue[1000];//The Proc Name
//	char buf[128];
//	CString strTemp = "";
//	int hours = 0, minutes = 0;
//	float value = 0.0;
//	float TotalValue = 0.0;
//	ElapsedTime valuetime;
//	float **yValue;
//	yValue = new float*[1000];
//	for(int k = 0; k < 1000; k++)
//	{
//		yValue[k] = new float[nColNum];
//	}
//	for(k=0;k<1000;k++)
//	{
//		for(int i=0;i<nColNum;i++)
//		{
//			yValue[k][i]=0.0;
//		}
//	}
//
//	float **yTotalValue;
//	yTotalValue = new float*[1000];
//	for(int k = 0; k < 1000; k++)
//	{
//		yTotalValue[k] = new float[nColNum];
//	}
//	for(k=0;k<1000;k++)
//	{
//		for(int i=0;i<nColNum;i++)
//		{
//			yTotalValue[k][i]=0.0;
//		}
//	}
//
//	int i=0;
//	chartFile.reset();
//	chartFile.skipLines(3);
//
//	if(!chartFile.getLine())
//	{
//		chartFile.closeIn();
//		return;
//	}
//	chartFile.setToField(0);
//	chartFile.getField(buf,128);
//	nProcCount++;
//	strxValue[nProcCount].Format("%s",buf);//Get The First Pax Type
//
//	int IntervalNum = 1;
//	chartFile.reset();
//	chartFile.skipLines(3);
//	while (chartFile.getLine())
//	{
//		chartFile.setToField(0);
//		chartFile.getField(buf,128);
//		strTemp.Format("%s",buf);
//		if(strxValue[nProcCount]!=strTemp)
//		{
//			nProcCount++;
//			strxValue[nProcCount]=strTemp;
//			IntervalNum = 1;
//		}
//
//		chartFile.setToField (3);
//		chartFile.getFloat (value);
//		//chartFile.setToField(1);
//		//chartFile.getTime (valuetime, TRUE);
//		//hours=(int)valuetime.asHours();
//		//minutes=(int)valuetime.asMinutes();
//
//		//		// TRACE("the time is %d hours,or %d minutes\n",hours,minutes);
//		yValue[nProcCount][IntervalNum]=value;	
//
//		chartFile.getFloat(TotalValue);
//		yTotalValue[nProcCount][IntervalNum]=TotalValue;
//
//		IntervalNum = IntervalNum + 1;
//	}
//
//	// add by bird 2003/10/13
//	ElapsedTime BeginTime,EndTime;
//	getTime( string[1], BeginTime, EndTime );
//	int nRowNum = (EndTime.asMinutes()-BeginTime.asMinutes())/interval; 
//
//	m_MSChartCtrl.SetRowCount(nRowNum);
//	m_MSChartCtrl.SetColumnCount(nProcCount*2);
//	m_MSChartCtrl.SetColumnLabelCount(nProcCount*2);
//
//	CString strLabel;
//
//	ElapsedTime detaTime( interval * 60l );
//	ElapsedTime time( BeginTime );
//	for(i = 1; i <= nRowNum; i++)
//	{
//		time.set(time.asSeconds() % WholeDay);
//		CString strBeginTime = time.printTime(0);
//		time += detaTime;
//		time.set(time.asSeconds() % WholeDay);
//		CString strEndTime	 = time.printTime(0);
//
//		strLabel.Format( "%s~%s", strBeginTime, strEndTime );
//		m_MSChartCtrl.SetRow(i);
//		DealRowLabel(strLabel);
//		m_MSChartCtrl.SetRowLabel(strLabel);
//
//	}
//
//	for(k = 1 ; k <= nProcCount; k++)
//	{
//		//(normal)
//		m_MSChartCtrl.SetColumn( 2*k -1);
//		CString strColumnLabel = "";
//		strColumnLabel.Format("%s normal(no affect)",strxValue[k]);
//		m_MSChartCtrl.SetColumnLabel(strColumnLabel);
//
//		for(i = 1; i <= nRowNum; i++)
//		{
//			m_MSChartCtrl.SetRow(i);
//			strLabel.Format("%f",yValue[k][i]);
//			m_MSChartCtrl.SetData(strLabel);
//		}
//
//		//total (incremental)
//		m_MSChartCtrl.SetColumn(2*k);
//		strColumnLabel.Format("%s Total(include affect)",strxValue[k]);
//		m_MSChartCtrl.SetColumnLabel(strColumnLabel);
//
//		for(i = 1; i <= nRowNum; i++)
//		{
//			m_MSChartCtrl.SetRow(i);
//			strLabel.Format("%f",yTotalValue[k][i]);
//			m_MSChartCtrl.SetData(strLabel);
//		}
//	}
//
//	for(k = 0; k < 1000; k++)
//	{
//		delete[] yValue[k];
//		delete[] yTotalValue[k];
//	}
//	delete[] yValue;
//	delete[] yTotalValue;
//	chartFile.closeIn();
//	m_MSChartCtrl.SetShowLegend(TRUE);
//
//}
//
//void CRepGraphView::BillboardLinkedProcIncrementalVisitSummary()
//{
//	CString sFileName;
//	if( GetLoadReportType() == load_by_user )
//	{
//		sFileName = GetUserLoadReportFile();
//	}
//	else								// load_by_system
//	{
//		sFileName = ((CTermPlanDoc*)GetDocument())->GetTerminal().GetSimReportManager()->GetCurrentReportFileName( ((CTermPlanDoc*)GetDocument())->m_ProjInfo.path );
//	}
//	chartFile.init (sFileName, READ);
//	//2.)Draw Title,X Axis Title,Y Axis Title,Footnote
//	chartFile.reset();
//	chartFile.getLine();
//	chartFile.getLine();
//	char string[4][128];
//	chartFile.getField (string[0], 128);
//	chartFile.getField (string[1], 128);
//	chartFile.getField (string[2], 128);
//	chartFile.getField (string[3], 128);
//	CString strFootnote;
//	strFootnote.Format("%s %s %s %s",NoUnderline(CString(string[0])),string[1],string[2],string[3]);
//	SetHeadXYTitle("pax Incremental Visit per Hour","Processor","Passengers Served per Hour",strFootnote);
//	//3.)Draw it
//	CString strxValue[1000];//The Proc Type Name
//	int nPaxCount=0;
//	float yValue[1000];
//	char buf[128];
//	CString strTemp;
//	float value = 0.0;
//	for(int k=0;k<1000;k++)
//	{
//		yValue[k]=0;	
//	}
//
//	float yTotalValue[1000];
//	for(int k=0;k<1000;k++)
//	{
//		yTotalValue[k]=0;	
//	}
//	int i=0;
//	chartFile.reset();
//	chartFile.skipLines(3);
//
//	while (chartFile.getLine())
//	{
//		chartFile.setToField(0);
//		chartFile.getField(buf,128);
//		strTemp.Format("%s",buf);
//		if(strxValue[nPaxCount]!=strTemp)
//		{
//			nPaxCount++;
//			strxValue[nPaxCount]=strTemp;
//		}
//
////		chartFile.setToField (4);
//		chartFile.getFloat (value);
//		yTotalValue[nPaxCount]=value;
//
//		chartFile.getFloat (value);
//		yValue[nPaxCount]=value;
//	}
//
//	m_MSChartCtrl.SetRowCount(nPaxCount *2);
//	m_MSChartCtrl.SetColumnCount(1);
//	m_MSChartCtrl.SetColumn(1);
//	m_MSChartCtrl.SetColumnLabel("Processor Type");
//
//	CString strLabel;
//	for(i=1;i<=nPaxCount;i++)
//	{
//		m_MSChartCtrl.SetRow(2*i -1);
//		strLabel.Format("%s normal",strxValue[i]);
//		DealRowLabel(strLabel);
//		m_MSChartCtrl.SetRowLabel(strLabel);
//
//		strLabel.Format("%f",yTotalValue[i]);
//		m_MSChartCtrl.SetData(strLabel);
//
//		m_MSChartCtrl.SetRow(2*i);
//		strLabel.Format("%s Incremental visit",strxValue[i]);
//		DealRowLabel(strLabel);
//		m_MSChartCtrl.SetRowLabel(strLabel);
//
//		strLabel.Format("%f",yValue[i]);
//		m_MSChartCtrl.SetData(strLabel);
//
//	}
//	chartFile.closeIn();
//	m_MSChartCtrl.SetShowLegend(FALSE);
//}
//
//LoadReportType CRepGraphView::GetLoadReportType( void ) const
//{
//	return ((CTermPlanDoc*)GetDocument())->GetARCReportManager().GetLoadReportType();
//	
//}
//
//const CString& CRepGraphView::GetUserLoadReportFile( void ) const
//{
//	return ((CTermPlanDoc*)GetDocument())->GetARCReportManager().GetUserLoadReportFile();
//}
void CRepGraphView::OnCbnSelChangeReportSubType()
{
	if(m_pRepGraphViewOperator)
		m_pRepGraphViewOperator->OnCbnSelChangeReportSubType();
	// TODO: Add your control notification handler code here
}
