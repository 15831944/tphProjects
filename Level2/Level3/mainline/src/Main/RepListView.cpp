// RepListView.cpp : implementation file
//

#include "stdafx.h"
#include "termplan.h"
#include "RepListView.h"
#include "RepControlView.h"
#include "RepGraphView.h"
#include "common/csv_file.h"
#include "TermPlanDoc.h"
#include "../common/UnitsManager.h"
#include "reports\ReportParameter.h"
#include "RepListViewMultiRunReportOperator.h"
#include "RepListViewSingleReportOperator.h"
#include "../common/SimAndReportManager.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
/*
NOTE:
	When we save all length into files, we must use meters as default unit,
	but the ARCTerm system internal's unit is centimeters, so we save length / SCALE_FACTOR into
	the files. when we get those data from files we should convert it into length * SCALE_FACTOR
*/
/////////////////////////////////////////////////////////////////////////////
// CRepListView

IMPLEMENT_DYNCREATE(CRepListView, CFormView)

CRepListView::CRepListView()
: CFormView(CRepListView::IDD)
{
	//m_nColumnCount = 0;
	//m_sExtension = _T("");
	m_pRepListViewBaseOperator= NULL;
	m_nCurrentPrintCount = 1;
}

CRepListView::~CRepListView()
{
	((CTermPlanDoc*)GetDocument())->GetARCReportManager().SetReportPara(NULL);
}

void CRepListView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//DDX_Control(pDX, IDC_REPORT_GRIDCTRL,m_wndListWnd);
}
BEGIN_MESSAGE_MAP(CRepListView, CFormView)
//{{AFX_MSG_MAP(CRepListView)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_NOTIFY_REFLECT(LVN_COLUMNCLICK, OnColumnclick)
	ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, OnCustomDraw)
	ON_COMMAND(ID_LISTVIEW_MAXIMIZE, OnListviewMaximize)
	ON_COMMAND(ID_LISTVIEW_NORMAL, OnListviewNormal)
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_LISTVIEW_PRINT, OnListviewPrint)
	ON_COMMAND(ID_LISTVIEW_EXPORT, OnListviewExport)
	ON_NOTIFY(LVN_COLUMNCLICK, IDC_LISTCTRL, OnColumnclickListCtrl)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRepListView drawing

void CRepListView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: add draw code here
}

/////////////////////////////////////////////////////////////////////////////
// CRepListView diagnostics

#ifdef _DEBUG
void CRepListView::AssertValid() const
{
	CFormView::AssertValid();
}

void CRepListView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CRepListView message handlers
void CRepListView::OnColumnclickListCtrl(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here
	int nTestIndex = pNMListView->iSubItem;
	if( nTestIndex >= 1 )
	{
		CWaitCursor	wCursor;
		if(::GetKeyState( VK_CONTROL ) < 0 ) // Is <CTRL> Key Down
		{
			m_ctlHeaderCtrl.SortColumn( nTestIndex, MULTI_COLUMN_SORT );
		}
		else
		{
			m_ctlHeaderCtrl.SortColumn( nTestIndex, SINGLE_COLUMN_SORT );
		}
		m_ctlHeaderCtrl.SaveSortList();

		for (int i = 0; i < m_wndListWnd.GetItemCount(); i++)
		{
			CString strIndex;
			strIndex.Format(_T("%d"),i+1);
			m_wndListWnd.SetItemText( i, 0, strIndex );
		}

	}	

	
	*pResult = 0;
}

void CRepListView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
{
	if(IsWindow(m_hWnd))
	{
		if(GetReportParameter()==NULL)
			return;
	
		CTermPlanDoc * pDoc=(CTermPlanDoc *)GetDocument();
		
		if(GetReportParameter()->IsMultiRunReport())
		{

			m_pRepListViewBaseOperator = new CRepListViewMultiRunReportOperator(&m_wndListWnd,&(pDoc->GetARCReportManager()),this);
		}
		else
		{
			m_pRepListViewBaseOperator = new CRepListViewSingleReportOperator(&m_wndListWnd,&(pDoc->GetARCReportManager()),this);
		}

		if(m_pRepListViewBaseOperator)
			m_pRepListViewBaseOperator->LoadReport(lHint,pHint);

		//switch(((CTermPlanDoc*)GetDocument())->GetARCReportManager().GetReportType())
		//{
		//case ENUM_BAGDELIVTIME_REP:
		//	m_nReportFileType = BagDeliveryTimeReportFile;
		//	m_sExtension = "bdt";	
		//	break;
		//	
		//case ENUM_BAGDISTRIBUTION_REP:
		//	m_nReportFileType = BagDistReportFile;
		//	m_sExtension = "bdr";
		//	break;

		//case ENUM_ACOPERATION_REP:
		//	m_nReportFileType = AcOperationsReportFile;
		//	m_sExtension = "aor";
		//	break;
		//case ENUM_PAXLOG_REP:
		//	m_nReportFileType=PaxLogReportFile;
		//	m_sExtension = "pld";
		//	break;
		//	
		//case ENUM_UTILIZATION_REP:
		//	m_nReportFileType=UtilizationReportFile;
		//	m_sExtension = "utd";
		//	break;
		//case ENUM_DISTANCE_REP:
		//	m_nReportFileType=DistanceReportFile;
		//	m_sExtension = "dsd";
		//	break;
		//case ENUM_PAXCOUNT_REP:
		//	m_nReportFileType=PassengerCountReportFile;
		//	m_sExtension = "pcd";
		//	break;
		//case ENUM_QUEUETIME_REP:
		//	m_nReportFileType=QueueTimeReportFile;
		//	m_sExtension = "qtd";
		//	break;
		//case ENUM_CONVEYOR_WAIT_TIME_REP:
		//	m_nReportFileType=ConveyorWaitTimeReport;
		//	m_sExtension = "cwtd";
		//	break;
		//case ENUM_THROUGHPUT_REP:
		//	m_nReportFileType=ThroughputReportFile;
		//	m_sExtension = "thd";
		//	break;
		//case ENUM_QUEUELENGTH_REP:
		//	m_nReportFileType=QueueLengthReportFile;
		//	m_sExtension = "qld";
		//	break;
		//case ENUM_CONVEYOR_WAIT_LENGTH_REP:
		//	m_nReportFileType=ConveyorWaitLengthReport;
		//	m_sExtension = "cwld";
		//	break;
		//	
		//case ENUM_AVGQUEUELENGTH_REP:
		//	m_nReportFileType=AverageQueueLengthReportFile;
		//	m_sExtension = "qad";
		//	break;
	
		//case ENUM_DURATION_REP:
		//	m_nReportFileType=DurationReportFile;
		//	m_sExtension = "drd";
		//	break;
		//case ENUM_SERVICETIME_REP:
		//	m_nReportFileType=ServiceTimeReportFile;
		//	m_sExtension = "srd";
		//	break;
		//case ENUM_ACTIVEBKDOWN_REP:
		//	m_nReportFileType=ActivityBreakdownReportFile;
		//	m_sExtension = "acd";
		//	break;
		//case ENUM_BAGWAITTIME_REP:
		//	m_nReportFileType=BagWaitTimeReportFile;
		//	m_sExtension = "bgd";
		//	break;
		//case ENUM_BAGCOUNT_REP:
		//	m_nReportFileType=BagCountReportFile;
		//	m_sExtension = "bcd";
		//	break;
		//case ENUM_PAXDENS_REP:			//not defined in the "Filelist", temp extension used: "txt"
		//	m_nReportFileType=PassengerDensityReportFile;
		//	m_sExtension = "pdd";
		//	break;
		//case ENUM_SPACETHROUGHPUT_REP:
		//	m_nReportFileType = SpaceThroughputReportFile;
		//	m_sExtension = "std";
		//	break;
		//case ENUM_COLLISIONS_REP:
		//	m_nReportFileType = CollisionReportFile;
		//	m_sExtension = "crf";
		//	break;
		//case ENUM_FIREEVACUTION_REP:
		//	m_nReportFileType = FireEvacuationReportFile;
		//	m_sExtension = "fir";
		//	break;
		//case ENUM_MISSFLIGHT_REP:
		//	m_nReportFileType = MissFlightReportFile;
		//	m_sExtension = "missflt";
		//	break;
		//case ENUM_BILLBOARD_EXPOSUREINCIDENCE_REP:
		//	m_nReportFileType = BillboardExposureIncidenceFile;
		//	m_sExtension = "bil";
		//	break;
		//case ENUM_BILLBOARD_LINKEDPROCINCREMENTVISIT_REP:
		//	m_nReportFileType = BillboardLinkedProcIncrementlVisitFile;
		//	m_sExtension = "bil";
		//	break;

		//}
		//Load();
	}
}

//#define MAX_NUM_COLUMN  16

//void CRepListView::SetHeader()
//{
//	CString csColumnLabel[MAX_NUM_COLUMN];
//	int nDefaultColumnWidth[MAX_NUM_COLUMN];
//	int nFormat[MAX_NUM_COLUMN];
//	EDataType type[MAX_NUM_COLUMN];
//	// remove all the columns for the previous view.
//	m_wndListWnd.DeleteAllItems();
//	for( int i=0; i<m_nColumnCount; i++ )
//	{
//		m_wndListWnd.DeleteColumn( 0 );
//	}
//	
////	CReportSpecs *prepSpecs=((CTermPlanDoc *)GetDocument())->m_prepSpecs;	
//	int iDetailed=-1;
//	GetReportParameter()->GetReportType( iDetailed );
//	
//	switch( m_nReportFileType )
//	{
//	case MissFlightReportFile:
//		{
//			m_nColumnCount = 5;
//			csColumnLabel[0] = CString("Passenger ID");
//			csColumnLabel[1] = CString("Flight ID");
//			csColumnLabel[2] = CString("Last Processor");
//			csColumnLabel[3] = CString("Time");
//			csColumnLabel[4] = CString("Dep Time");
//			
//			nDefaultColumnWidth[0] = 70;
//			nDefaultColumnWidth[1] = 70;
//			nDefaultColumnWidth[2] = 70;
//			nDefaultColumnWidth[3] = 70;
//			nDefaultColumnWidth[4] = 70;
//
//			for( int i=0; i<5; i++ )
//			{
//				nFormat[i] = LVCFMT_CENTER; 
//			}
//
//			type[0] = dtSTRING;
//			type[1] = dtSTRING;
//			type[2] = dtSTRING;
//			type[3] = dtSTRING;
//			type[4] = dtINT;
//
//			break;
//		}
//	case BagDeliveryTimeReportFile:
//		{
//			if(iDetailed==0)//Detailed
//			{
//				m_nColumnCount = 4;
//				csColumnLabel[0] = CString("Bag #");
//				csColumnLabel[1] = CString("Pax #");
//				csColumnLabel[2] = CString("Passenger Type");
//				csColumnLabel[3] = CString("Delivery Time");
//				
//				nDefaultColumnWidth[0] = 70;
//				nDefaultColumnWidth[1] = 70;
//				nDefaultColumnWidth[2] = 70;
//				nDefaultColumnWidth[3] = 70;
//		
//				for( int i=0; i<4; i++ )
//				{
//					nFormat[i] = LVCFMT_CENTER; 
//				}
//
//				type[0] = dtINT;
//				type[1] = dtINT;
//				type[2] = dtSTRING;
//				type[3] = dtDATETIME;
//				break;						
//			}
//			else
//			{
//				m_nColumnCount = 15;
//				csColumnLabel[0] = CString("Passenger Type");
//				csColumnLabel[1] = CString("Minimum");
//				csColumnLabel[2] = CString("Average");
//				csColumnLabel[3] = CString("Maximum");
//				csColumnLabel[4] = CString("Count");
//							
//				CString str = " (s)";
//				csColumnLabel[5] = CString("Q1") + str;
//				csColumnLabel[6] = CString("Q2") + str;
//				csColumnLabel[7] = CString("Q3") + str;
//				csColumnLabel[8] = CString("P1") + str;
//				csColumnLabel[9] = CString("P5") + str;
//				csColumnLabel[10] = CString("P10") + str;
//				csColumnLabel[11] = CString("P90") + str;
//				csColumnLabel[12] = CString("P95") + str;
//				csColumnLabel[13] = CString("P99") + str;
//
//				csColumnLabel[14] = CString("Sigma") + str;
//				
//				
//				nDefaultColumnWidth[0] = 180;
//				nDefaultColumnWidth[1] = 60;
//				nDefaultColumnWidth[2] = 60;
//				nDefaultColumnWidth[3] = 60;
//				nDefaultColumnWidth[4] = 60;
//				nDefaultColumnWidth[5] = 60;
//				nDefaultColumnWidth[6] = 60;
//				nDefaultColumnWidth[7] = 60;
//				nDefaultColumnWidth[8] = 60;
//				nDefaultColumnWidth[9] = 60;
//				nDefaultColumnWidth[10] = 60;
//				nDefaultColumnWidth[11] = 60;
//				nDefaultColumnWidth[12] = 60;
//				nDefaultColumnWidth[13] = 60;
//				nDefaultColumnWidth[14] = 60;
//				
//				
//				for( int i=0; i<15; i++ )
//				{
//					nFormat[i] = LVCFMT_CENTER; 
//				}
//				
//				type[0] = dtSTRING;
//				type[1] = dtDATETIME;
//				type[2] = dtDATETIME;
//				type[3] = dtDATETIME;
//				type[4] = dtINT;
//
//				type[5] = dtDATETIME;
//				type[6] = dtDATETIME;
//				type[7] = dtDATETIME;
//				type[8] = dtDATETIME;
//				type[9] = dtDATETIME;
//				type[10] = dtDATETIME;
//				type[11] = dtDATETIME;
//				type[12] = dtDATETIME;
//				type[13] = dtDATETIME;				
//				type[14] = dtDATETIME;	
//				break;		
//			}
//		}
//	case BagDistReportFile:
//		{
//			m_nColumnCount = 6;
//			csColumnLabel[0] = CString("Flight ID(Arr)");
//			csColumnLabel[1] = CString("Flight ID(Dep)");
//			csColumnLabel[2] = CString("Scheduled Time(Arr)");
//			csColumnLabel[3] = CString("Scheduled Time(Dep)");
//			csColumnLabel[4] = CString("Bag Num(Arr)");
//			csColumnLabel[5] = CString("Bag Num(Dep)");
//			
//			nDefaultColumnWidth[0] = 70;
//			nDefaultColumnWidth[1] = 70;
//			nDefaultColumnWidth[2] = 70;
//			nDefaultColumnWidth[3] = 70;
//			nDefaultColumnWidth[4] = 70;
//			nDefaultColumnWidth[5] = 70;
//		
//			for( int i=0; i<6; i++ )
//			{
//				nFormat[i] = LVCFMT_CENTER; 
//			}
//
//			type[0] = dtSTRING;
//			type[1] = dtSTRING;
//			type[2] = dtSTRING;
//			type[3] = dtSTRING;
//			type[4] = dtINT;
//			type[5] = dtINT;
//			break;			
//		}
//	case PaxLogReportFile:
//		{
//			m_nColumnCount = 8;
//			csColumnLabel[0] = CString("ID");
//			csColumnLabel[1] = CString("Entry");
//			csColumnLabel[2] = CString("Exit");
//			csColumnLabel[3] = CString("Group");
//			csColumnLabel[4] = CString("Bags");
//			csColumnLabel[5] = CString("Carts");
//			CString csSpeed;
//			csSpeed.Format( "Speed (%s/Sec)", UNITSMANAGER->GetLengthUnitString(UNITSMANAGER->GetLengthUnits()) );
//			csColumnLabel[6] = csSpeed;
//			csColumnLabel[7] = CString("Pax Type");
//			
//			nDefaultColumnWidth[0] = 70;
//			nDefaultColumnWidth[1] = 90;
//			nDefaultColumnWidth[2] = 90;
//			nDefaultColumnWidth[3] = 70;
//			nDefaultColumnWidth[4] = 70;
//			nDefaultColumnWidth[5] = 70;
//			nDefaultColumnWidth[6] = 120;
//			nDefaultColumnWidth[7] = 220;
//			
//			for( int i=0; i<7; i++ )
//			{
//				nFormat[i] = LVCFMT_CENTER; 
//			}
//			nFormat[7] = LVCFMT_LEFT;
//			
//			type[0] = dtINT;
//			type[1] = dtDATETIME;
//			type[2] = dtDATETIME;
//			type[3] = dtINT;
//			type[4] = dtINT;
//			type[5] = dtINT;
//			type[6] = dtDEC;
//			type[7] = dtSTRING;
//			break;
//		}
//		
//	case DistanceReportFile:
//		{
//			if(iDetailed==0)//Detailed
//			{
//				//AfxMessageBox("Detailed!");
//				m_nColumnCount = 5;
//				csColumnLabel[0] = CString("Passenger ID");
//				CUnitsManager* pUM = CUnitsManager::GetInstance();
//				int nUnit = pUM->GetLengthUnits();				
//				CString str = " (" + pUM->GetLengthUnitString( nUnit ) + ")";
//				csColumnLabel[1] = CString("Distance") + str;
//				csColumnLabel[2] = CString("Passenger Type");
//
//				csColumnLabel[3] = CString("From Processor");
//				csColumnLabel[4] = CString("To Processor");
//				
//				nDefaultColumnWidth[0] = 100;
//				nDefaultColumnWidth[1] = 100;
//				nDefaultColumnWidth[2] = 180;
//				nDefaultColumnWidth[3] = 100;
//				nDefaultColumnWidth[4] = 100;
//				
//				for( int i=0; i<2; i++ )
//				{
//					nFormat[i] = LVCFMT_CENTER; 
//				}
//				nFormat[2] = LVCFMT_LEFT;
//				nFormat[3] = LVCFMT_CENTER;
//				nFormat[4] = LVCFMT_CENTER;
//				
//				type[0] = dtINT;
//				type[1] = dtDEC;
//				type[2] = dtSTRING;
//				type[3] = dtSTRING;
//				type[4] = dtSTRING;
//				break;
//			}
//			else
//			{
//				//AfxMessageBox("Summary!");
//				m_nColumnCount = 15;
//				csColumnLabel[0] = CString("Passenger Type");
//				CUnitsManager* pUM = CUnitsManager::GetInstance();
//				int nUnit = pUM->GetLengthUnits();				
//				CString str = " (" + pUM->GetLengthUnitString( nUnit ) + ")";
//				csColumnLabel[1] = CString("Minimum") + str;
//				csColumnLabel[2] = CString("Average") + str;
//				csColumnLabel[3] = CString("Maximum") + str;
//				csColumnLabel[4] = CString("Count");
//
//				csColumnLabel[5] = CString("Q1") + str;
//				csColumnLabel[6] = CString("Q2") + str;
//				csColumnLabel[7] = CString("Q3") + str;
//				csColumnLabel[8] = CString("P1") + str;
//				csColumnLabel[9] = CString("P5") + str;
//				csColumnLabel[10] = CString("P10") + str;
//				csColumnLabel[11] = CString("P90") + str;
//				csColumnLabel[12] = CString("P95") + str;
//				csColumnLabel[13] = CString("P99") + str;
//
//				csColumnLabel[14] = CString("Sigma") + str;
//				
//				
//				nDefaultColumnWidth[0] = 180;
//				nDefaultColumnWidth[1] = 60;
//				nDefaultColumnWidth[2] = 60;
//				nDefaultColumnWidth[3] = 60;
//				nDefaultColumnWidth[4] = 60;
//				nDefaultColumnWidth[5] = 60;
//				nDefaultColumnWidth[6] = 60;
//				nDefaultColumnWidth[7] = 60;
//				nDefaultColumnWidth[8] = 60;
//				nDefaultColumnWidth[9] = 60;
//				nDefaultColumnWidth[10] = 60;
//				nDefaultColumnWidth[11] = 60;
//				nDefaultColumnWidth[12] = 60;
//				nDefaultColumnWidth[13] = 60;
//				nDefaultColumnWidth[14] = 60;
//				
//				
//				for( int i=0; i<15; i++ )
//				{
//					nFormat[i] = LVCFMT_CENTER; 
//				}
//				
//				type[0] = dtSTRING;
//				type[1] = dtDEC;
//				type[2] = dtDEC;
//				type[3] = dtDEC;
//				type[4] = dtINT;
//				type[5] = dtDEC;
//				type[6] = dtDEC;
//				type[7] = dtDEC;
//				type[8] = dtDEC;
//				type[9] = dtDEC;
//				type[10] = dtDEC;
//				type[11] = dtDEC;
//				type[12] = dtDEC;
//				type[13] = dtDEC;				
//				type[14] = dtDEC;	
//				break;
//			}
//			break;
//		}
//		
//	case FireEvacuationReportFile:
//		{
//			if( iDetailed == 0 )	// detailed
//			{
//				m_nColumnCount = 5;
//				csColumnLabel[0] = CString("Pax ID");
//				csColumnLabel[1] = CString("Pax Type");
//				csColumnLabel[2] = CString("Exit Processor");
//				csColumnLabel[3] = CString("Exit Time");
//				csColumnLabel[4] = CString("Exit Duration");
//
//				nDefaultColumnWidth[0] = 80;
//				nDefaultColumnWidth[1] = 80;
//				nDefaultColumnWidth[2] = 80;
//				nDefaultColumnWidth[3] = 80;
//				nDefaultColumnWidth[4] = 80;
//
//				for( int i=0; i<m_nColumnCount; i++ )
//				{
//					nFormat[i] = LVCFMT_CENTER;
//				}
//				
//				type[0] = dtINT;
//				type[1] = dtSTRING;
//				type[2] = dtSTRING;
//				type[3] = dtDATETIME;
//				type[4] = dtDATETIME;
//				break;
//			}
//			else	//summary
//			{
//				m_nColumnCount = 15;
//				csColumnLabel[0] = CString("Passenger Type");
//				csColumnLabel[1] = CString("Minimum");
//				csColumnLabel[2] = CString("Average");
//				csColumnLabel[3] = CString("Maximum");
//				csColumnLabel[4] = CString("Count");
//				
//				CString str = " (s)";
//				csColumnLabel[5] = CString("Q1") + str;
//				csColumnLabel[6] = CString("Q2") + str;
//				csColumnLabel[7] = CString("Q3") + str;
//				csColumnLabel[8] = CString("P1") + str;
//				csColumnLabel[9] = CString("P5") + str;
//				csColumnLabel[10] = CString("P10") + str;
//				csColumnLabel[11] = CString("P90") + str;
//				csColumnLabel[12] = CString("P95") + str;
//				csColumnLabel[13] = CString("P99") + str;
//				csColumnLabel[14] = CString("Sigma") + str;
//				
//				nDefaultColumnWidth[0] = 180;
//				nDefaultColumnWidth[1] = 60;
//				nDefaultColumnWidth[2] = 60;
//				nDefaultColumnWidth[3] = 60;
//				nDefaultColumnWidth[4] = 60;
//				nDefaultColumnWidth[5] = 60;
//				nDefaultColumnWidth[6] = 60;
//				nDefaultColumnWidth[7] = 60;
//				nDefaultColumnWidth[8] = 60;
//				nDefaultColumnWidth[9] = 60;
//				nDefaultColumnWidth[10] = 60;
//				nDefaultColumnWidth[11] = 60;
//				nDefaultColumnWidth[12] = 60;
//				nDefaultColumnWidth[13] = 60;
//				nDefaultColumnWidth[14] = 60;
//					
//				for( int i=0; i<m_nColumnCount; i++ )
//				{
//					nFormat[i] = LVCFMT_CENTER; 
//				}
//				
//				type[0] = dtSTRING;
//				type[1] = dtDATETIME;
//				type[2] = dtDATETIME;
//				type[3] = dtDATETIME;
//				type[4] = dtINT;
//				
//				type[5] = dtDATETIME;
//				type[6] = dtDATETIME;
//				type[7] = dtDATETIME;
//				type[8] = dtDATETIME;
//				type[9] = dtDATETIME;
//				type[10] = dtDATETIME;
//				type[11] = dtDATETIME;
//				type[12] = dtDATETIME;
//				type[13] = dtDATETIME;	
//				type[14] = dtDATETIME;
//				break;	
//			}
//		}
//		
//	case QueueTimeReportFile:
//		{
//			if(iDetailed==0)//Detailed
//			{
//				m_nColumnCount = 5;
//				csColumnLabel[0] = CString("Pax #");
//				csColumnLabel[1] = CString("Total Time");
//				csColumnLabel[2] = CString("Average Time");
//				csColumnLabel[3] = CString("Processors");
//				csColumnLabel[4] = CString("Passenger Type");
//				
//				nDefaultColumnWidth[0] = 80;
//				nDefaultColumnWidth[1] = 80;
//				nDefaultColumnWidth[2] = 80;
//				nDefaultColumnWidth[3] = 80;
//				nDefaultColumnWidth[4] = 200;
//				
//				for( int i=0; i<4; i++ )
//				{
//					nFormat[i] = LVCFMT_CENTER; 
//				}
//				nFormat[4] = LVCFMT_LEFT;
//				
//				type[0] = dtINT;
//				type[1] = dtDATETIME;
//				type[2] = dtDATETIME;
//				type[3] = dtINT;
//				type[4] = dtSTRING;
//				break;
//			}
//			else//Summary
//			{
//				m_nColumnCount = 15;
//				csColumnLabel[0] = CString("Passenger Type");
//				csColumnLabel[1] = CString("Minimum");
//				csColumnLabel[2] = CString("Average");
//				csColumnLabel[3] = CString("Maximum");
//				csColumnLabel[4] = CString("Count");
//							
//				CString str = " (s)";
//				csColumnLabel[5] = CString("Q1") + str;
//				csColumnLabel[6] = CString("Q2") + str;
//				csColumnLabel[7] = CString("Q3") + str;
//				csColumnLabel[8] = CString("P1") + str;
//				csColumnLabel[9] = CString("P5") + str;
//				csColumnLabel[10] = CString("P10") + str;
//				csColumnLabel[11] = CString("P90") + str;
//				csColumnLabel[12] = CString("P95") + str;
//				csColumnLabel[13] = CString("P99") + str;
//
//				csColumnLabel[14] = CString("Sigma") + str;
//				
//				
//				nDefaultColumnWidth[0] = 180;
//				nDefaultColumnWidth[1] = 60;
//				nDefaultColumnWidth[2] = 60;
//				nDefaultColumnWidth[3] = 60;
//				nDefaultColumnWidth[4] = 60;
//				nDefaultColumnWidth[5] = 60;
//				nDefaultColumnWidth[6] = 60;
//				nDefaultColumnWidth[7] = 60;
//				nDefaultColumnWidth[8] = 60;
//				nDefaultColumnWidth[9] = 60;
//				nDefaultColumnWidth[10] = 60;
//				nDefaultColumnWidth[11] = 60;
//				nDefaultColumnWidth[12] = 60;
//				nDefaultColumnWidth[13] = 60;
//				nDefaultColumnWidth[14] = 60;
//				
//				
//				for( int i=0; i<15; i++ )
//				{
//					nFormat[i] = LVCFMT_CENTER; 
//				}
//				
//				type[0] = dtSTRING;
//				type[1] = dtDATETIME;
//				type[2] = dtDATETIME;
//				type[3] = dtDATETIME;
//				type[4] = dtINT;
//
//				type[5] = dtDATETIME;
//				type[6] = dtDATETIME;
//				type[7] = dtDATETIME;
//				type[8] = dtDATETIME;
//				type[9] = dtDATETIME;
//				type[10] = dtDATETIME;
//				type[11] = dtDATETIME;
//				type[12] = dtDATETIME;
//				type[13] = dtDATETIME;				
//				type[14] = dtDATETIME;	
//				break;	
//			}//end of else
//		}//end of case
//	case ConveyorWaitTimeReport:
//		{
//			if(iDetailed==0)//Detailed
//			{
//				m_nColumnCount = 5;
//				csColumnLabel[0] = CString("Pax #");
//				csColumnLabel[1] = CString("Total Time");
//				csColumnLabel[2] = CString("Average Time");
//				csColumnLabel[3] = CString("Processors");
//				csColumnLabel[4] = CString("Passenger Type");
//				
//				nDefaultColumnWidth[0] = 80;
//				nDefaultColumnWidth[1] = 80;
//				nDefaultColumnWidth[2] = 80;
//				nDefaultColumnWidth[3] = 80;
//				nDefaultColumnWidth[4] = 200;
//				
//				for( int i=0; i<4; i++ )
//				{
//					nFormat[i] = LVCFMT_CENTER; 
//				}
//				nFormat[4] = LVCFMT_LEFT;
//				
//				type[0] = dtINT;
//				type[1] = dtDATETIME;
//				type[2] = dtDATETIME;
//				type[3] = dtINT;
//				type[4] = dtSTRING;
//				break;
//			}
//			else//Summary
//			{
//				m_nColumnCount = 15;
//				csColumnLabel[0] = CString("Passenger Type");
//				csColumnLabel[1] = CString("Minimum");
//				csColumnLabel[2] = CString("Average");
//				csColumnLabel[3] = CString("Maximum");
//				csColumnLabel[4] = CString("Count");
//				
//				CString str = " (s)";
//				csColumnLabel[5] = CString("Q1") + str;
//				csColumnLabel[6] = CString("Q2") + str;
//				csColumnLabel[7] = CString("Q3") + str;
//				csColumnLabel[8] = CString("P1") + str;
//				csColumnLabel[9] = CString("P5") + str;
//				csColumnLabel[10] = CString("P10") + str;
//				csColumnLabel[11] = CString("P90") + str;
//				csColumnLabel[12] = CString("P95") + str;
//				csColumnLabel[13] = CString("P99") + str;
//				
//				csColumnLabel[14] = CString("Sigma") + str;
//				
//				
//				nDefaultColumnWidth[0] = 180;
//				nDefaultColumnWidth[1] = 60;
//				nDefaultColumnWidth[2] = 60;
//				nDefaultColumnWidth[3] = 60;
//				nDefaultColumnWidth[4] = 60;
//				nDefaultColumnWidth[5] = 60;
//				nDefaultColumnWidth[6] = 60;
//				nDefaultColumnWidth[7] = 60;
//				nDefaultColumnWidth[8] = 60;
//				nDefaultColumnWidth[9] = 60;
//				nDefaultColumnWidth[10] = 60;
//				nDefaultColumnWidth[11] = 60;
//				nDefaultColumnWidth[12] = 60;
//				nDefaultColumnWidth[13] = 60;
//				nDefaultColumnWidth[14] = 60;
//				
//				
//				for( int i=0; i<15; i++ )
//				{
//					nFormat[i] = LVCFMT_CENTER; 
//				}
//				
//				type[0] = dtSTRING;
//				type[1] = dtDATETIME;
//				type[2] = dtDATETIME;
//				type[3] = dtDATETIME;
//				type[4] = dtINT;
//				
//				type[5] = dtDATETIME;
//				type[6] = dtDATETIME;
//				type[7] = dtDATETIME;
//				type[8] = dtDATETIME;
//				type[9] = dtDATETIME;
//				type[10] = dtDATETIME;
//				type[11] = dtDATETIME;
//				type[12] = dtDATETIME;
//				type[13] = dtDATETIME;				
//				type[14] = dtDATETIME;	
//				break;	
//			}//end of else
//		}//end of case
//		
//	case DurationReportFile:
//		{
//			if(iDetailed==0)//Detailed
//			{
//				m_nColumnCount = 5;
//				csColumnLabel[0] = CString("Passenger #");
//				csColumnLabel[1] = CString("Duration");
//				csColumnLabel[2] = CString("Passenger Type");
//				csColumnLabel[3] = CString("From Processor");
//				csColumnLabel[4] = CString("To Processor");
//				
//				nDefaultColumnWidth[0] = 80;
//				nDefaultColumnWidth[1] = 80;
//				nDefaultColumnWidth[2] = 200;
//				nDefaultColumnWidth[3] = 100;
//				nDefaultColumnWidth[4] = 100;
//				
//				
//				nFormat[0] = LVCFMT_LEFT;
//				nFormat[1] = LVCFMT_CENTER; 
//				nFormat[2] = LVCFMT_LEFT;
//				nFormat[3] = LVCFMT_CENTER;
//				nFormat[4] = LVCFMT_CENTER;
//				
//				type[0] = dtINT;
//				type[1] = dtDATETIME;
//				type[2] = dtSTRING;
//				type[3] = dtSTRING;
//				type[4] = dtSTRING;
//				break;
//			}
//			else//Summary
//			{
//				m_nColumnCount = 15;
//				csColumnLabel[0] = CString("Passenger Type");
//				csColumnLabel[1] = CString("Minimum");
//				csColumnLabel[2] = CString("Average");
//				csColumnLabel[3] = CString("Maximum");
//				csColumnLabel[4] = CString("Count");
//
//				CString str = " (s)";
//				csColumnLabel[5] = CString("Q1") + str;
//				csColumnLabel[6] = CString("Q2") + str;
//				csColumnLabel[7] = CString("Q3") + str;
//				csColumnLabel[8] = CString("P1") + str;
//				csColumnLabel[9] = CString("P5") + str;
//				csColumnLabel[10] = CString("P10") + str;
//				csColumnLabel[11] = CString("P90") + str;
//				csColumnLabel[12] = CString("P95") + str;
//				csColumnLabel[13] = CString("P99") + str;
//				
//				csColumnLabel[14] = CString("Sigma") + str;
//				
//				
//				nDefaultColumnWidth[0] = 190;
//				nDefaultColumnWidth[1] = 70;
//				nDefaultColumnWidth[2] = 70;
//				nDefaultColumnWidth[3] = 70;
//				nDefaultColumnWidth[4] = 70;
//				nDefaultColumnWidth[5] = 60;
//				nDefaultColumnWidth[6] = 60;
//				nDefaultColumnWidth[7] = 60;
//				nDefaultColumnWidth[8] = 60;
//				nDefaultColumnWidth[9] = 60;
//				nDefaultColumnWidth[10] = 60;
//				nDefaultColumnWidth[11] = 60;
//				nDefaultColumnWidth[12] = 60;
//				nDefaultColumnWidth[13] = 60;
//				nDefaultColumnWidth[14] = 60;
//				
//				for( int i=0; i<15; i++ )
//				{
//					nFormat[i] = LVCFMT_CENTER; 
//				}
//				
//				type[0] = dtSTRING;
//				type[1] = dtDATETIME;
//				type[2] = dtDATETIME;
//				type[3] = dtDATETIME;
//				type[4] = dtINT;
//
//				type[5] = dtDATETIME;
//				type[6] = dtDATETIME;
//				type[7] = dtDATETIME;
//				type[8] = dtDATETIME;
//				type[9] = dtDATETIME;
//				type[10] = dtDATETIME;
//				type[11] = dtDATETIME;
//				type[12] = dtDATETIME;
//				type[13] = dtDATETIME;				
//				type[14] = dtDATETIME;	
//				break;	
//			}//end of else
//		}//end of case
//
//	case ServiceTimeReportFile:
//		{
//			if(iDetailed==0)//Detailed
//			{
//				m_nColumnCount = 5;
//				csColumnLabel[0] = CString("Pax #");
//				csColumnLabel[1] = CString("Group Service");
//				csColumnLabel[2] = CString("Single Service");
//				csColumnLabel[3] = CString("Processors");
//				csColumnLabel[4] = CString("Passenger Type");
//				
//				nDefaultColumnWidth[0] = 80;
//				nDefaultColumnWidth[1] = 80;
//				nDefaultColumnWidth[2] = 80;
//				nDefaultColumnWidth[3] = 80;
//				nDefaultColumnWidth[4] = 200;
//				
//				for( int i=0; i<4; i++ )
//				{
//					nFormat[i] = LVCFMT_CENTER; 
//				}
//				nFormat[4] = LVCFMT_LEFT;
//				
//				type[0] = dtINT;
//				type[1] = dtDATETIME;
//				type[2] = dtDATETIME;
//				type[3] = dtINT;
//				type[4] = dtSTRING;
//				break;
//			}
//			else//Summary
//			{
//				m_nColumnCount = 15;
//				csColumnLabel[0] = CString("Passenger Type");
//				csColumnLabel[1] = CString("Minimum");
//				csColumnLabel[2] = CString("Average");
//				csColumnLabel[3] = CString("Maximum");
//				csColumnLabel[4] = CString("Count");
//
//				CString str = " (s)";
//				csColumnLabel[5] = CString("Q1") + str;
//				csColumnLabel[6] = CString("Q2") + str;
//				csColumnLabel[7] = CString("Q3") + str;
//				csColumnLabel[8] = CString("P1") + str;
//				csColumnLabel[9] = CString("P5") + str;
//				csColumnLabel[10] = CString("P10") + str;
//				csColumnLabel[11] = CString("P90") + str;
//				csColumnLabel[12] = CString("P95") + str;
//				csColumnLabel[13] = CString("P99") + str;
//				csColumnLabel[14] = CString("Sigma") + str;
//				
//				nDefaultColumnWidth[0] = 190;
//				nDefaultColumnWidth[1] = 70;
//				nDefaultColumnWidth[2] = 70;
//				nDefaultColumnWidth[3] = 70;
//				nDefaultColumnWidth[4] = 70;
//				nDefaultColumnWidth[5] = 60;
//				nDefaultColumnWidth[6] = 60;
//				nDefaultColumnWidth[7] = 60;
//				nDefaultColumnWidth[8] = 60;
//				nDefaultColumnWidth[9] = 60;
//				nDefaultColumnWidth[10] = 60;
//				nDefaultColumnWidth[11] = 60;
//				nDefaultColumnWidth[12] = 60;
//				nDefaultColumnWidth[13] = 60;
//				nDefaultColumnWidth[14] = 60;
//				
//				for( int i=0; i<15; i++ )
//				{
//					nFormat[i] = LVCFMT_CENTER; 
//				}
//				
//				type[0] = dtSTRING;
//				type[1] = dtDATETIME;
//				type[2] = dtDATETIME;
//				type[3] = dtDATETIME;
//				type[4] = dtINT;
//				type[5] = dtDATETIME;
//				type[6] = dtDATETIME;
//				type[7] = dtDATETIME;
//				type[8] = dtDATETIME;
//				type[9] = dtDATETIME;
//				type[10] = dtDATETIME;
//				type[11] = dtDATETIME;
//				type[12] = dtDATETIME;
//				type[13] = dtDATETIME;				
//				type[14] = dtDATETIME;	
//				break;	
//			}//end of else
//		}//end of case
//
//	case ActivityBreakdownReportFile:
//		{
//			if(iDetailed==0)//Detailed
//			{
//				m_nColumnCount = 7;
//				csColumnLabel[0] = CString("ID");
//				csColumnLabel[1] = CString("Moving Time");
//				csColumnLabel[2] = CString("Queuing Time");
//				csColumnLabel[3] = CString("Bag Wait Time");
//				csColumnLabel[4] = CString("Hold Area Time");
//				csColumnLabel[5] = CString("Service Time");
//				csColumnLabel[6] = CString("Passenger Type");
//				
//				nDefaultColumnWidth[0] = 80;
//				nDefaultColumnWidth[1] = 80;
//				nDefaultColumnWidth[2] = 80;
//				nDefaultColumnWidth[3] = 80;
//				nDefaultColumnWidth[4] = 80;
//				nDefaultColumnWidth[5] = 80;
//				nDefaultColumnWidth[6] = 200;
//				
//				for( int i=0; i<6; i++ )
//				{
//					nFormat[i] = LVCFMT_CENTER; 
//				}
//				nFormat[6] = LVCFMT_LEFT;
//				
//				type[0] = dtINT;
//				type[1] = dtDATETIME;
//				type[2] = dtDATETIME;
//				type[3] = dtDATETIME;
//				type[4] = dtDATETIME;
//				type[5] = dtDATETIME;
//				type[6] = dtSTRING;
//				break;
//			}
//			else//Summary
//			{
//				m_nColumnCount = 7;
//				csColumnLabel[0] = CString("Passenger Type");
//				csColumnLabel[1] = CString("Moving Time");
//				csColumnLabel[2] = CString("Queuing Time");
//				csColumnLabel[3] = CString("Bag Wait Time");
//				csColumnLabel[4] = CString("Hold Area Time");
//				csColumnLabel[5] = CString("Service Time");
//				csColumnLabel[6] = CString("Count");
//				
//				nDefaultColumnWidth[0] = 200;
//				nDefaultColumnWidth[1] = 80;
//				nDefaultColumnWidth[2] = 80;
//				nDefaultColumnWidth[3] = 80;
//				nDefaultColumnWidth[4] = 80;
//				nDefaultColumnWidth[5] = 80;
//				nDefaultColumnWidth[6] = 80;
//				
//				for( int i=0; i<7; i++ )
//				{
//					nFormat[i] = LVCFMT_CENTER; 
//				}
//				
//				type[0] = dtSTRING;
//				type[1] = dtDATETIME;
//				type[2] = dtDATETIME;
//				type[3] = dtDATETIME;
//				type[4] = dtDATETIME;
//				type[5] = dtDATETIME;
//				type[6] = dtINT;
//				break;	
//			}
//		}//end of case
//
//	case UtilizationReportFile:
//		{
//			if(iDetailed==0)//Detailed
//			{
//				m_nColumnCount = 7;
//				csColumnLabel[0] = CString("Processor");
//				csColumnLabel[1] = CString("Scheduled");
//				csColumnLabel[2] = CString("Overtime");
//				csColumnLabel[3] = CString("Actual");
//				csColumnLabel[4] = CString("Service");
//				csColumnLabel[5] = CString("Idle");
//				csColumnLabel[6] = CString("Utilization(%)");
//				
//				nDefaultColumnWidth[0] = 200;
//				nDefaultColumnWidth[1] = 80;
//				nDefaultColumnWidth[2] = 80;
//				nDefaultColumnWidth[3] = 80;
//				nDefaultColumnWidth[4] = 80;
//				nDefaultColumnWidth[5] = 80;
//				nDefaultColumnWidth[6] = 80;
//				
//				for( int i=0; i<7; i++ )
//				{
//					nFormat[i] = LVCFMT_CENTER; 
//				}
//				
//				type[0] = dtSTRING;
//				type[1] = dtDATETIME;
//				type[2] = dtDATETIME;
//				type[3] = dtDATETIME;
//				type[4] = dtDATETIME;
//				type[5] = dtDATETIME;
//				type[6] = dtDEC;
//				break;
//			}
//			else//Summary
//			{
//				m_nColumnCount = 8;
//				csColumnLabel[0] = CString("Processor");
//				csColumnLabel[1] = CString("Group Size");
//				csColumnLabel[2] = CString("Scheduled");
//				csColumnLabel[3] = CString("Overtime");
//				csColumnLabel[4] = CString("Actual");
//				csColumnLabel[5] = CString("Service");
//				csColumnLabel[6] = CString("Idle");
//				csColumnLabel[7] = CString("Utilization(%)");
//				
//				nDefaultColumnWidth[0] = 190;
//				nDefaultColumnWidth[1] = 70;
//				nDefaultColumnWidth[2] = 70;
//				nDefaultColumnWidth[3] = 70;
//				nDefaultColumnWidth[4] = 70;
//				nDefaultColumnWidth[5] = 70;
//				nDefaultColumnWidth[6] = 70;
//				nDefaultColumnWidth[7] = 70;
//				
//				for( int i=0; i<8; i++ )
//				{
//					nFormat[i] = LVCFMT_CENTER; 
//				}
//				
//				type[0] = dtSTRING;
//				type[1] = dtINT;
//				type[2] = dtDATETIME;
//				type[3] = dtDATETIME;
//				type[4] = dtDATETIME;
//				type[5] = dtDATETIME;
//				type[6] = dtDATETIME;
//				type[7] = dtDEC;
//				break;	
//			}
//		}
//		
//	case ThroughputReportFile:
//		{
//		if(iDetailed==0)//Detailed
//			{
//				m_nColumnCount = 4;
//				csColumnLabel[0] = CString("Processor");
//				csColumnLabel[1] = CString("Start Time");
//				csColumnLabel[2] = CString("End Time");
//				csColumnLabel[3] = CString("Pax Served");
//				
//				nDefaultColumnWidth[0] = 200;
//				nDefaultColumnWidth[1] = 80;
//				nDefaultColumnWidth[2] = 80;
//				nDefaultColumnWidth[3] = 80;
//				
//				for( int i=0; i<4; i++ )
//				{
//					nFormat[i] = LVCFMT_CENTER; 
//				}
//				
//				type[0] = dtSTRING;
//				type[1] = dtDATETIME;
//				type[2] = dtDATETIME;
//				type[3] = dtDEC;
//				break;
//			}
//			else//Summary
//			{
//				m_nColumnCount = 6;
//				csColumnLabel[0] = CString("Processor");
//				csColumnLabel[1] = CString("Group Size");
//				csColumnLabel[2] = CString("Pax/Group");
//				csColumnLabel[3] = CString("Pax/Proc");
//				csColumnLabel[4] = CString("Group Thrpt/Hr");
//				csColumnLabel[5] = CString("Avg/Hr");
//				
//				nDefaultColumnWidth[0] = 190;
//				nDefaultColumnWidth[1] = 70;
//				nDefaultColumnWidth[2] = 70;
//				nDefaultColumnWidth[3] = 70;
//				nDefaultColumnWidth[4] = 70;
//				nDefaultColumnWidth[5] = 70;
//				
//				for( int i=0; i<6; i++ )
//				{
//					nFormat[i] = LVCFMT_CENTER; 
//				}
//				
//				type[0] = dtSTRING;
//				type[1] = dtINT;
//				type[2] = dtINT;
//				type[3] = dtDEC;
//				type[4] = dtDEC;
//				type[5] = dtDEC;
//				break;	
//			}//end of else
//		}//end of case
//
//	case QueueLengthReportFile:
//		{
//			if(iDetailed==0)//Detailed
//			{
//				m_nColumnCount = 3;
//				csColumnLabel[0] = CString("Processor");
//				csColumnLabel[1] = CString("Time");
//				csColumnLabel[2] = CString("Queue Length");
//				
//				nDefaultColumnWidth[0] = 200;
//				nDefaultColumnWidth[1] = 80;
//				nDefaultColumnWidth[2] = 80;
//				
//				for( int i=0; i<3; i++ )
//				{
//					nFormat[i] = LVCFMT_CENTER; 
//				}
//				
//				type[0] = dtSTRING;
//				type[1] = dtDATETIME;
//				type[2] = dtINT;
//				break;
//			}
//			else//Summary
//			{
//				m_nColumnCount = 7;
//				csColumnLabel[0] = CString("Processor Group");
//				csColumnLabel[1] = CString("Group Size");
//				csColumnLabel[2] = CString("Time");
//				csColumnLabel[3] = CString("Min");
//				csColumnLabel[4] = CString("Avg");
//				csColumnLabel[5] = CString("Max");
//				csColumnLabel[6] = CString("Total");
//				
//				nDefaultColumnWidth[0] = 190;
//				nDefaultColumnWidth[1] = 70;
//				nDefaultColumnWidth[2] = 70;
//				nDefaultColumnWidth[3] = 70;
//				nDefaultColumnWidth[4] = 70;
//				nDefaultColumnWidth[5] = 70;
//				nDefaultColumnWidth[6] = 70;
//				
//				for( int i=0; i<7; i++ )
//				{
//					nFormat[i] = LVCFMT_CENTER; 
//				}
//				
//				type[0] = dtSTRING;
//				type[1] = dtINT;
//				type[2] = dtDATETIME;
//				type[3] = dtINT;
//				type[4] = dtDEC;
//				type[5] = dtINT;
//				type[6] = dtINT;
//				break;	
//			}//end of else
//		}//end of case
//
//	case ConveyorWaitLengthReport:
//		{
//			if(iDetailed==0)//Detailed
//			{
//				m_nColumnCount = 3;
//				csColumnLabel[0] = CString("Processor");
//				csColumnLabel[1] = CString("Time");
//				csColumnLabel[2] = CString("Conveyour Wait Length");
//				
//				nDefaultColumnWidth[0] = 200;
//				nDefaultColumnWidth[1] = 80;
//				nDefaultColumnWidth[2] = 80;
//				
//				for( int i=0; i<3; i++ )
//				{
//					nFormat[i] = LVCFMT_CENTER; 
//				}
//				
//				type[0] = dtSTRING;
//				type[1] = dtDATETIME;
//				type[2] = dtINT;
//				break;
//			}
//			else//Summary
//			{
//				m_nColumnCount = 7;
//				csColumnLabel[0] = CString("Processor Group");
//				csColumnLabel[1] = CString("Group Size");
//				csColumnLabel[2] = CString("Time");
//				csColumnLabel[3] = CString("Min");
//				csColumnLabel[4] = CString("Avg");
//				csColumnLabel[5] = CString("Max");
//				csColumnLabel[6] = CString("Total");
//				
//				nDefaultColumnWidth[0] = 190;
//				nDefaultColumnWidth[1] = 70;
//				nDefaultColumnWidth[2] = 70;
//				nDefaultColumnWidth[3] = 70;
//				nDefaultColumnWidth[4] = 70;
//				nDefaultColumnWidth[5] = 70;
//				nDefaultColumnWidth[6] = 70;
//				
//				for( int i=0; i<7; i++ )
//				{
//					nFormat[i] = LVCFMT_CENTER; 
//				}
//				
//				type[0] = dtSTRING;
//				type[1] = dtINT;
//				type[2] = dtDATETIME;
//				type[3] = dtINT;
//				type[4] = dtDEC;
//				type[5] = dtINT;
//				type[6] = dtINT;
//				break;	
//			}//end of else
//		}//end of case			
//		
//	case AverageQueueLengthReportFile:
//		{
//			if(iDetailed==0)//Detailed
//			{
//				m_nColumnCount = 4;
//				csColumnLabel[0] = CString("Processor");
//				csColumnLabel[1] = CString("Avg Queue");
//				csColumnLabel[2] = CString("Max Queue");
//				csColumnLabel[3] = CString("Max Queue Time");
//				
//				nDefaultColumnWidth[0] = 200;
//				nDefaultColumnWidth[1] = 80;
//				nDefaultColumnWidth[2] = 80;
//				nDefaultColumnWidth[3] = 80;
//				
//				for( int i=0; i<4; i++ )
//				{
//					nFormat[i] = LVCFMT_CENTER; 
//				}
//				
//				type[0] = dtSTRING;
//				type[1] = dtDEC;
//				type[2] = dtINT;
//				type[3] = dtDATETIME;
//				break;
//			}
//			else//Summary
//			{
//				m_nColumnCount = 5;
//				csColumnLabel[0] = CString("Processor");
//				csColumnLabel[1] = CString("Group Size");
//				csColumnLabel[2] = CString("Avg Queue");
//				csColumnLabel[3] = CString("Max Queue");
//				csColumnLabel[4] = CString("Max Queue Time");
//				
//				nDefaultColumnWidth[0] = 190;
//				nDefaultColumnWidth[1] = 70;
//				nDefaultColumnWidth[2] = 70;
//				nDefaultColumnWidth[3] = 70;
//				nDefaultColumnWidth[4] = 70;
//				
//				for( int i=0; i<5; i++ )
//				{
//					nFormat[i] = LVCFMT_CENTER; 
//				}
//				
//				type[0] = dtSTRING;
//				type[1] = dtINT;
//				type[2] = dtDEC;
//				type[3] = dtINT;
//				type[4] = dtDATETIME;
//				break;	
//			}//end of else
//		}//end of case
//	
//	case PassengerCountReportFile:
//		{
//			if(iDetailed==0)//Detailed
//			{
//				m_nColumnCount = 3;
//				csColumnLabel[0] = CString("Passenger Type");
//				csColumnLabel[1] = CString("Time");
//				csColumnLabel[2] = CString("Count");
//				
//				
//				nDefaultColumnWidth[0] = 200;
//				nDefaultColumnWidth[1] = 80;
//				nDefaultColumnWidth[2] = 80;
//				
//				
//				for( int i=0; i<3; i++ )
//				{
//					nFormat[i] = LVCFMT_CENTER; 
//				}
//				
//				type[0] = dtSTRING;
//				type[1] = dtDATETIME;
//				type[2] = dtINT;
//				
//				break;
//			}
//			else
//			{
//				m_nColumnCount = 9;
//				csColumnLabel[0] = CString("Passenger Type");
//				csColumnLabel[1] = CString("Min Pax");
//				csColumnLabel[2] = CString("Min Time");
//				csColumnLabel[3] = CString("Avg/Period");
//				csColumnLabel[4] = CString("Max Pax");
//				csColumnLabel[5] = CString("Max Time");
//				csColumnLabel[6] = CString("Total Pax");
//				csColumnLabel[7] = CString("First Entry");
//				csColumnLabel[8] = CString("Last Exit");
//				
//				
//				nDefaultColumnWidth[0] = 200;
//				nDefaultColumnWidth[1] = 90;
//				nDefaultColumnWidth[2] = 90;
//				nDefaultColumnWidth[3] = 70;
//				nDefaultColumnWidth[4] = 70;
//				nDefaultColumnWidth[5] = 70;
//				nDefaultColumnWidth[6] = 70;
//				nDefaultColumnWidth[7] = 70;
//				nDefaultColumnWidth[8] = 80;
//				
//				for( int i=0; i<9; i++ )
//				{
//					nFormat[i] = LVCFMT_CENTER; 
//				}
//				
//				type[0] = dtSTRING;
//				type[1] = dtINT;
//				type[2] = dtDATETIME;
//				type[3] = dtDEC;
//				type[4] = dtINT;
//				type[5] = dtDATETIME;
//				type[6] = dtINT;
//				type[7] = dtDATETIME;
//				type[8] = dtDATETIME;
//				break;
//			}
//			break;
//		}
//	case PassengerDensityReportFile:
//		{
//			m_nColumnCount = 5;
//			csColumnLabel[0] = CString("Passenger Type");
//			csColumnLabel[1] = CString("Time");
//			csColumnLabel[2] = CString("Count");
//			csColumnLabel[3] = CString("Pax/m2");
//			csColumnLabel[4] = CString("m2/Pax");
//			
//			nDefaultColumnWidth[0] = 200;
//			nDefaultColumnWidth[1] = 80;
//			nDefaultColumnWidth[2] = 80;
//			nDefaultColumnWidth[3] = 80;
//			nDefaultColumnWidth[4] = 80;
//		
//			for( int i=0; i<5; i++ )
//			{
//				nFormat[i] = LVCFMT_CENTER; 
//			}
//		
//			type[0] = dtSTRING;
//			type[1] = dtDATETIME;
//			type[2] = dtINT;
//			type[3] = dtDEC;
//			type[4] = dtDEC;
//			break;
//		}//end of case
//
//	case SpaceThroughputReportFile:
//		{
//			m_nColumnCount = 4;
//			csColumnLabel[0] = CString("Passenger Type");
//			csColumnLabel[1] = CString("Start Time");
//			csColumnLabel[2] = CString("End Time");
//			csColumnLabel[3] = CString("Count");
//
//			nDefaultColumnWidth[0] = 200;
//			nDefaultColumnWidth[1] = 80;
//			nDefaultColumnWidth[2] = 80;
//			nDefaultColumnWidth[3] = 80;
//
//			for(int i=0; i<4; i++)
//			{
//				nFormat[i] = LVCFMT_CENTER;
//			}
//
//			type[0] = dtSTRING;
//			type[1] = dtDATETIME;
//			type[2] = dtDATETIME;
//			type[3] = dtDEC;
//			break;
//		}
//
//	case CollisionReportFile:
//		{
//			m_nColumnCount = 4;
//			csColumnLabel[0] = CString("Pax Type");
//			csColumnLabel[1] = CString("Time");
//			csColumnLabel[2] = CString("Pax ID 1");
//			csColumnLabel[3] = CString("Pax ID 2");
//
//			nDefaultColumnWidth[0] = 100;
//			nDefaultColumnWidth[1] = 60;
//			nDefaultColumnWidth[2] = 100;
//			nDefaultColumnWidth[3] = 100;
//
//			for(int i = 0; i < 4; i++)
//			{
//				nFormat[i] = LVCFMT_CENTER;
//			}
//			type[0] = dtSTRING;
//			type[1] = dtDATETIME;
//			type[2] = dtDEC;
//			type[3] = dtDEC;
//			break;
//		}
//		
//	case AcOperationsReportFile:
//		{
//			m_nColumnCount = 14;
//			csColumnLabel[0] = CString("Flight ID");
//			csColumnLabel[1] = CString("Operation");
//			csColumnLabel[2] = CString("Aircraft Type");
//			csColumnLabel[3] = CString("Capacity");
//			csColumnLabel[4] = CString("Scheduled Arr Time");
//			csColumnLabel[5] = CString("Scheduled Dep Time");
//			csColumnLabel[6] = CString("Load Factor");
//			csColumnLabel[7] = CString("Load");
//			csColumnLabel[8] = CString("Actual Arr Time");
//			csColumnLabel[9] = CString("Actual Dep Time");
//			csColumnLabel[10] = CString("Delay Arr");
//			csColumnLabel[11] = CString("Delay Dep");
//			csColumnLabel[12] = CString("Gate Occupancy");
//			csColumnLabel[13] = CString("Flight Type");
//			
//			nDefaultColumnWidth[0] = 70;
//			nDefaultColumnWidth[1] = 70;
//			nDefaultColumnWidth[2] = 70;
//			nDefaultColumnWidth[3] = 70;
//			nDefaultColumnWidth[4] = 70;
//			nDefaultColumnWidth[5] = 70;
//			nDefaultColumnWidth[6] = 70;
//			nDefaultColumnWidth[7] = 70;
//			nDefaultColumnWidth[8] = 70;
//			nDefaultColumnWidth[9] = 70;
//			nDefaultColumnWidth[10] = 70;
//			nDefaultColumnWidth[11] = 70;
//			nDefaultColumnWidth[12] = 70;
//			nDefaultColumnWidth[13] = 100;
//			
//			for( int i=0; i<14; i++ )
//			{
//				nFormat[i] = LVCFMT_CENTER; 
//			}
//
//			type[0] = dtSTRING;
//			type[1] = dtSTRING;
//			type[2] = dtSTRING;
//			type[3] = dtDEC;
//			type[4] = dtSTRING;
//			type[5] = dtSTRING;
////			type[4] = dtDATETIME;
////			type[5] = dtDATETIME;
//			type[6] = dtDEC;
//			type[7] = dtDEC;
//			type[8] = dtSTRING;
//			type[9] = dtSTRING;
////			type[8] = dtDATETIME;
////			type[9] = dtDATETIME;
//			type[10] = dtSTRING;
//			type[11] = dtSTRING;
////			type[10] = dtDEC;
////			type[11] = dtDEC;
//			type[12] = dtDATETIME;
//			type[13] = dtSTRING;
//			break;
//		}
//		
//	case BagCountReportFile:
//		{
//			if(iDetailed==0)//Detailed
//			{
//				m_nColumnCount = 3;
//				csColumnLabel[0] = CString("Processor");
//				csColumnLabel[1] = CString("Time");
//				csColumnLabel[2] = CString("Bag Count");
//				
//				nDefaultColumnWidth[0] = 100;
//				nDefaultColumnWidth[1] = 80;
//				nDefaultColumnWidth[2] = 80;
//				
//				for( int i=0; i<3; i++ )
//				{
//					nFormat[i] = LVCFMT_CENTER; 
//				}
//				
//				type[0] = dtSTRING;
//				type[1] = dtDATETIME;
//				type[2] = dtINT;
//			}
//			else//Summary
//			{
//				m_nColumnCount = 7;
//				csColumnLabel[0] = CString("Passenger Group");
//				csColumnLabel[1] = CString("Group Size");
//				csColumnLabel[2] = CString("Time");
//				csColumnLabel[3] = CString("Min");
//				csColumnLabel[4] = CString("Avg");
//				csColumnLabel[5] = CString("Max");
//				csColumnLabel[6] = CString("Total");
//				
//				nDefaultColumnWidth[0] = 200;
//				nDefaultColumnWidth[1] = 70;
//				nDefaultColumnWidth[2] = 70;
//				nDefaultColumnWidth[3] = 70;
//				nDefaultColumnWidth[4] = 70;
//				nDefaultColumnWidth[5] = 70;
//				nDefaultColumnWidth[6] = 70;
//				
//				for( int i=0; i<7; i++ )
//				{
//					nFormat[i] = LVCFMT_CENTER; 
//				}
//				
//				type[0] = dtSTRING;
//				type[1] = dtINT;
//				type[2] = dtDATETIME;
//				type[3] = dtINT;
//				type[4] = dtDEC;
//				type[5] = dtINT;
//				type[6] = dtINT;
//			}
//			break;
//		}//end of case
//	case BagWaitTimeReportFile:
//		{
//			if(iDetailed==0)//Detailed
//			{
//				m_nColumnCount = 4;
//				csColumnLabel[0] = CString("Pax #");
//				csColumnLabel[1] = CString("Bag Count");
//				csColumnLabel[2] = CString("Wait Time");
//				csColumnLabel[3] = CString("Passenger Type");
//				
//				nDefaultColumnWidth[0] = 80;
//				nDefaultColumnWidth[1] = 80;
//				nDefaultColumnWidth[2] = 80;
//				nDefaultColumnWidth[3] = 200;
//				
//				for( int i=0; i<3; i++ )
//				{
//					nFormat[i] = LVCFMT_CENTER; 
//				}
//				nFormat[3] = LVCFMT_LEFT;
//				
//				type[0] = dtINT;
//				type[1] = dtINT;
//				type[2] = dtDATETIME;
//				type[3] = dtSTRING;
//			}
//			else//Summary
//			{								
//				m_nColumnCount = 15;
//				csColumnLabel[0] = CString("Passenger Type");
//				csColumnLabel[1] = CString("Minimum");
//				csColumnLabel[2] = CString("Average");
//				csColumnLabel[3] = CString("Maximum");
//				csColumnLabel[4] = CString("Count");
//							
//				CString str = " (s)";
//				csColumnLabel[5] = CString("Q1") + str;
//				csColumnLabel[6] = CString("Q2") + str;
//				csColumnLabel[7] = CString("Q3") + str;
//				csColumnLabel[8] = CString("P1") + str;
//				csColumnLabel[9] = CString("P5") + str;
//				csColumnLabel[10] = CString("P10") + str;
//				csColumnLabel[11] = CString("P90") + str;
//				csColumnLabel[12] = CString("P95") + str;
//				csColumnLabel[13] = CString("P99") + str;
//
//				csColumnLabel[14] = CString("Sigma") + str;
//				
//				
//				nDefaultColumnWidth[0] = 180;
//				nDefaultColumnWidth[1] = 60;
//				nDefaultColumnWidth[2] = 60;
//				nDefaultColumnWidth[3] = 60;
//				nDefaultColumnWidth[4] = 60;
//				nDefaultColumnWidth[5] = 60;
//				nDefaultColumnWidth[6] = 60;
//				nDefaultColumnWidth[7] = 60;
//				nDefaultColumnWidth[8] = 60;
//				nDefaultColumnWidth[9] = 60;
//				nDefaultColumnWidth[10] = 60;
//				nDefaultColumnWidth[11] = 60;
//				nDefaultColumnWidth[12] = 60;
//				nDefaultColumnWidth[13] = 60;
//				nDefaultColumnWidth[14] = 60;
//				
//				
//				for( int i=0; i<15; i++ )
//				{
//					nFormat[i] = LVCFMT_CENTER; 
//				}
//				
//				type[0] = dtSTRING;
//				type[1] = dtDATETIME;
//				type[2] = dtDATETIME;
//				type[3] = dtDATETIME;
//				type[4] = dtINT;
//
//				type[5] = dtDATETIME;
//				type[6] = dtDATETIME;
//				type[7] = dtDATETIME;
//				type[8] = dtDATETIME;
//				type[9] = dtDATETIME;
//				type[10] = dtDATETIME;
//				type[11] = dtDATETIME;
//				type[12] = dtDATETIME;
//				type[13] = dtDATETIME;				
//				type[14] = dtDATETIME;	
//			}
//			break;
//		}//end of case
//
//	case BillboardExposureIncidenceFile:
//		{
//			if(iDetailed==0)//Detailed
//			{
//				m_nColumnCount = 5;
//				csColumnLabel[0] = CString("Processor");
//				csColumnLabel[1] = CString("Start Time");
//				csColumnLabel[2] = CString("End Time");
//				csColumnLabel[3] = CString("Exposure Served");
//				csColumnLabel[4] = CString("Total Pax Passed");
//				nDefaultColumnWidth[0] = 200;
//				nDefaultColumnWidth[1] = 80;
//				nDefaultColumnWidth[2] = 80;
//				nDefaultColumnWidth[3] = 120;
//				nDefaultColumnWidth[4] = 120;
//				for( int i=0; i<5; i++ )
//				{
//					nFormat[i] = LVCFMT_CENTER; 
//				}
//
//				type[0] = dtSTRING;
//				type[1] = dtDATETIME;
//				type[2] = dtDATETIME;
//				type[3] = dtDEC;
//				type[4] = dtDEC;
//				break;
//			}
//			else//Summary
//			{
//				m_nColumnCount = 3;
//				csColumnLabel[0] = CString("Processor");
//				//csColumnLabel[1] = CString("Group Size");
//				//csColumnLabel[2] = CString("Pax/Group");
//				//csColumnLabel[3] = CString("Pax/Proc");
//				//csColumnLabel[4] = CString("Group Thrpt/Hr");
//				csColumnLabel[1] = CString("Avg/Hr");
//				csColumnLabel[2] = CString("Avg/Hr");
//				nDefaultColumnWidth[0] = 190;
//				//nDefaultColumnWidth[1] = 70;
//				//nDefaultColumnWidth[2] = 70;
//				//nDefaultColumnWidth[3] = 70;
//				//nDefaultColumnWidth[4] = 70;
//				nDefaultColumnWidth[1] = 70;
//				nDefaultColumnWidth[2] = 70;
//				for( int i=0; i<3; i++ )
//				{
//					nFormat[i] = LVCFMT_CENTER; 
//				}
//
//				type[0] = dtSTRING;
//				//type[1] = dtINT;
//				//type[2] = dtINT;
//				//type[3] = dtDEC;
//				//type[4] = dtDEC;
//				type[1] = dtDEC;
//				type[2] = dtDEC;
//				break;	
//			}//end of else
//		}//end of case
//	case BillboardLinkedProcIncrementlVisitFile:
//		{
//			if(iDetailed==0)//Detailed
//			{
//				m_nColumnCount = 5;
//				csColumnLabel[0] = CString("Processor");
//				csColumnLabel[1] = CString("Start Time");
//				csColumnLabel[2] = CString("End Time");
//				csColumnLabel[3] = CString("Pax Severed");
//				csColumnLabel[4] = CString("Incremental Pax Severed");
//				nDefaultColumnWidth[0] = 200;
//				nDefaultColumnWidth[1] = 80;
//				nDefaultColumnWidth[2] = 80;
//				nDefaultColumnWidth[3] = 120;
//				nDefaultColumnWidth[4] = 120;
//				for( int i=0; i<5; i++ )
//				{
//					nFormat[i] = LVCFMT_CENTER; 
//				}
//
//				type[0] = dtSTRING;
//				type[1] = dtDATETIME;
//				type[2] = dtDATETIME;
//				type[3] = dtDEC;
//				type[4] = dtDEC;
//				break;
//			}
//			else//Summary
//			{
//				m_nColumnCount = 3;
//				csColumnLabel[0] = CString("Processor");
//				//csColumnLabel[1] = CString("Group Size");
//				//csColumnLabel[2] = CString("Pax/Group");
//				//csColumnLabel[3] = CString("Pax/Proc");
//				//csColumnLabel[4] = CString("Group Thrpt/Hr");
//				csColumnLabel[1] = CString("Avg/Hr");
//				csColumnLabel[2] = CString("Avg/Hr");
//				nDefaultColumnWidth[0] = 190;
//				//nDefaultColumnWidth[1] = 70;
//				//nDefaultColumnWidth[2] = 70;
//				//nDefaultColumnWidth[3] = 70;
//				//nDefaultColumnWidth[4] = 70;
//				nDefaultColumnWidth[1] = 70;
//				nDefaultColumnWidth[2] = 70;
//				for( int i=0; i<3; i++ )
//				{
//					nFormat[i] = LVCFMT_CENTER; 
//				}
//
//				type[0] = dtSTRING;
//				//type[1] = dtINT;
//				//type[2] = dtINT;
//				//type[3] = dtDEC;
//				//type[4] = dtDEC;
//				type[1] = dtDEC;
//				type[2] = dtDEC;
//				break;	
//			}//end of else
//		}//end of case
//
//	}//end of switch
//	for( i=0; i<m_nColumnCount; i++ )
//	{
//		m_wndListWnd.InsertColumn( i, csColumnLabel[i], nFormat[i], nDefaultColumnWidth[i] );
//		m_ctlHeaderCtrl.SetDataType( i, type[i] );
//	}
//}

//void CRepListView::Load()
//{
//	CWaitCursor wCursor;
//    char str[128];
//    //PROJMANAGER->getReportFileName( ((CTermPlanDoc*)GetDocument())->m_ProjInfo.path, m_nReportFileType, str );
//	CString sFileName;
//	if( ((CTermPlanDoc*)GetDocument())->GetARCReportManager().GetLoadReportType() == load_by_user )
//	{
//		sFileName = ((CTermPlanDoc*)GetDocument())->GetARCReportManager().GetUserLoadReportFile();
//	}
//	else	// load_by_syetem
//	{
//		sFileName = ((CTermPlanDoc*)GetDocument())->GetTerminal().GetSimReportManager()->GetCurrentReportFileName( ((CTermPlanDoc*)GetDocument())->m_ProjInfo.path );
//	}
//	
//	CSVFile csvFile( sFileName, READ );
//	// title should be on second line
//	
//	char title[128];
//	if (csvFile.getLine ())
//		csvFile.getField( title, 128 );
//	
//	m_wndListWnd.DeleteAllItems();
//	m_wndListWnd.SetRedraw( FALSE );
//	SetHeader();
//	
//	
//	// skip column headings line
//    csvFile.skipLine ();
//	int nDBIdx = 0;
//	double dLength;
//	m_vFlag.clear();
//    while (csvFile.getLine () != -1 && !csvFile.eof())
//    {
//		csvFile.getField( str, 128 );
//		int nItemIdx = m_wndListWnd.InsertItem( nDBIdx, str );
//		int i;	
//        
//		//********************************************************
//		//Modified by Luo Xiaobo 2002.4.29
//		CTermPlanDoc * pDoc=(CTermPlanDoc *)GetDocument();
//		ENUM_REPORT_TYPE enumReportType = pDoc->GetARCReportManager().GetReportType();
//		CUnitsManager* pUM = CUnitsManager::GetInstance();
//		
//		if( enumReportType == ENUM_PAXLOG_REP )
//		{
//			CString ColStr;
//			for( i=1; i<6; i++ )
//			{
//				csvFile.getField( str, 128 );
//				if( 1 == i || 2 == i)
//				{
//					ColStr = GetAbsDateTime(str);
//					m_wndListWnd.SetItemText( nItemIdx, i,  ColStr );
//				}
//				else
//					m_wndListWnd.SetItemText( nItemIdx, i,  str );
//			}
//			float fSpeed;
//			csvFile.getFloat( fSpeed );
//			fSpeed = static_cast<float>(UNITSMANAGER->ConvertLength( (double)fSpeed ));
//			sprintf( str, "%.2f", fSpeed );
//			m_wndListWnd.SetItemText( nItemIdx, 6,  str );
//			csvFile.getField( str, 128 );
//			m_wndListWnd.SetItemText( nItemIdx, 7,  str );
//
//
//		}
//		else if( enumReportType == ENUM_DISTANCE_REP )
//		{
//			//CReportSpecs *prepSpecs=((CTermPlanDoc *)GetDocument())->m_prepSpecs;	
//			int iDetailed;
//			GetReportParameter()->GetReportType( iDetailed );
//			if( iDetailed != 0 ) //Summary
//			{
//				csvFile.getFloat( dLength );
//				dLength = pUM->ConvertLength( dLength * SCALE_FACTOR );
//				sprintf( str,"%d",(int)dLength );
//				m_wndListWnd.SetItemText( nItemIdx,1,str );
//				csvFile.getFloat( dLength );
//				dLength = pUM->ConvertLength( dLength * SCALE_FACTOR );
//				sprintf( str,"%d",(int)dLength );
//				m_wndListWnd.SetItemText( nItemIdx,2,str );
//				csvFile.getFloat( dLength );
//				dLength = pUM->ConvertLength( dLength * SCALE_FACTOR );
//				sprintf( str,"%d",(int)dLength );
//				m_wndListWnd.SetItemText( nItemIdx,3,str );
//				csvFile.getField( str,128 );
//				m_wndListWnd.SetItemText( nItemIdx,4,str );
//				
//				for( int i=5; i<=14; ++i )
//				{
//					csvFile.getFloat( dLength );
//					dLength = pUM->ConvertLength( dLength * SCALE_FACTOR );
//					sprintf( str,"%d",(int)dLength );
//					m_wndListWnd.SetItemText( nItemIdx,i,str );
//				}				
//			}
//			else   //Detailed
//			{
//				csvFile.getFloat( dLength );
//				dLength = pUM->ConvertLength( dLength * SCALE_FACTOR);
//				sprintf( str,"%d",(int)dLength );
//				m_wndListWnd.SetItemText( nItemIdx,1,str );
//				csvFile.getField( str,128 );
//				m_wndListWnd.SetItemText( nItemIdx,2,str );
//				csvFile.getField( str, 128 );
//				m_wndListWnd.SetItemText( nItemIdx,3,str );
//				csvFile.getField( str, 128 );
//				m_wndListWnd.SetItemText( nItemIdx,4,str );
//			}
//		}
//		else // in the follow statement,should concern the detail and summary model
//		{	 //maybe ,the row of the models are not the same in the same position
//			 //like the following report type ENUM_THROUGHPUT_REP
//			for( i=1; i<m_nColumnCount; i++ )
//			{
//				csvFile.getField( str, 128 );				
//
//				//if( (enumReportType == ENUM_ACOPERATION_REP)&& (5 == i || 9 == i || 12 == i) )
//				//{
//				//	CString strTemp = GetAbsDateTime(str);
//				//	strcpy(str, strTemp.GetBuffer(0));
//				//}
//
//				if( (enumReportType == ENUM_MISSFLIGHT_REP) && ( 4 == i) )
//				{
//					CString strTemp = GetAbsDateTime(str);
//					strcpy(str, strTemp.GetBuffer(0));
//				}
//				
//				if ( (enumReportType == ENUM_THROUGHPUT_REP) &&	( (i == 1) || (i == 2) ))
//				{
//					int iDetailed=-1;
//					GetReportParameter()->GetReportType( iDetailed );
//					if (iDetailed == 0)
//					{
//						CString strTemp = GetAbsDateTime(str, FALSE);
//						strcpy(str, strTemp.GetBuffer(0));
//					}
//				}
//				else if ( enumReportType == ENUM_CONVEYOR_WAIT_LENGTH_REP)	
//				{
//					int iDetailed=-1;
//					GetReportParameter()->GetReportType( iDetailed );
//					if (iDetailed == 0)
//					{
//						if (i == 1)
//						{
//							CString strTemp = GetAbsDateTime(str, FALSE);
//							strcpy(str, strTemp.GetBuffer(0));
//						}
//					}
//					else
//					{
//						if ( i == 2)
//						{
//							CString strTemp = GetAbsDateTime(str, FALSE);
//							strcpy(str, strTemp.GetBuffer(0));
//						}
//					}
//				}
//				else if ((enumReportType == ENUM_QUEUELENGTH_REP)&& ( i == 2 ))
//				{
//					int iDetailed=-1;
//					GetReportParameter()->GetReportType( iDetailed );
//					if (iDetailed == 1)
//					{
//						CString strTemp = GetAbsDateTime(str, FALSE);
//						strcpy(str, strTemp.GetBuffer(0));
//					}
//				}
//				if ( (enumReportType == ENUM_PAXDENS_REP) && (i == 1))
//				{
//					CString strTemp = GetAbsDateTime(str, FALSE);
//					strcpy(str, strTemp.GetBuffer(0));
//				}
//
//				if ( (enumReportType == ENUM_SPACETHROUGHPUT_REP) &&
//					( (i == 1) || (i == 2) ) )
//				{
//					CString strTemp = GetAbsDateTime(str, FALSE);
//					strcpy(str, strTemp.GetBuffer(0));
//				}
//				//if (enumReportType == ENUM_UTILIZATION_REP && i == 7 )
//				//{
//				//	double dPercent = atof(str);
//				//	dPercent =dPercent*100;
//				//	sprintf(str,"%.0f",dPercent);
//				//}
//
//				m_wndListWnd.SetItemText( nItemIdx, i,  str );
//			}
//		}
//		
//		//********************************************************
//		//Modified by Xie Bo 2002.4.19
//		//Setting Color because of threshold 
//		//		CTermPlanDoc * pDoc=(CTermPlanDoc *)GetDocument();
//		//		CReportManager repManager=pDoc->m_repManager;
//		//		ENUM_REPORT_TYPE enumReportType= repManager.GetReportType();
//		int thresholdField,thresholdType;
//		float value;
//		ElapsedTime valuetime;
//		char temp[20];
//
//		bool bFlag = false;
//		if( enumReportType != ENUM_PAXLOG_REP && enumReportType != ENUM_UTILIZATION_REP &&
//			enumReportType != ENUM_ACTIVEBKDOWN_REP && enumReportType != ENUM_PAXDENS_REP )
//		{
//			switch(enumReportType)
//			{
//			case ENUM_DISTANCE_REP:
//				thresholdType=FLOAT_THRESHOLD;
//				int iDetailed;
//				GetReportParameter()->GetReportType( iDetailed );
//				if( iDetailed ==0)//detailed
//				{
//					thresholdField=1;
//				}
//				else
//				{
//					thresholdField=3;
//				}
//
//				long lThreahold;				
//				if(GetReportParameter()->GetThreshold( lThreahold ))
//				{
//					csvFile.setToField(thresholdField);
//					csvFile.getFloat(value);
//					value = static_cast<float>(pUM->ConvertLength( value * SCALE_FACTOR ));
//					if (value >= lThreahold)
//						bFlag = true;
//				}
//				break;
//			case ENUM_AVGQUEUELENGTH_REP:
//
//				GetReportParameter()->GetReportType( iDetailed );
//
//				if(iDetailed==0)//detailed
//				{
//					thresholdField=2;
//				}
//				else
//				{
//					thresholdField=3;
//				}
//
//				if(GetReportParameter()->GetThreshold( lThreahold ))
//				{
//					csvFile.setToField(thresholdField);
//					csvFile.getInteger(i);
//					value=static_cast<float>(i);
//					if (value >= lThreahold)
//					{
//						bFlag = true;
//					}
//				}
//				break;
//			case ENUM_QUEUELENGTH_REP:
//			case ENUM_BAGCOUNT_REP:
//			case ENUM_CONVEYOR_WAIT_LENGTH_REP:
//
//				GetReportParameter()->GetReportType( iDetailed );
//				if(iDetailed==0)//detailed
//				{
//					thresholdField=2;
//				}
//				else
//				{
//					thresholdField=5;
//				}
//
//				if(GetReportParameter()->GetThreshold( lThreahold ))
//				{
//					csvFile.setToField(thresholdField);
//					csvFile.getInteger(i);
//					value=static_cast<float>(i);
//					if (value >= lThreahold)
//					{
//						bFlag = true;
//					}
//				}
//				break;
//			case ENUM_THROUGHPUT_REP:
//				GetReportParameter()->GetReportType( iDetailed );
//				if(iDetailed==0)//detailed
//				{
//					thresholdField=2;
//				}
//				else
//				{
//					thresholdField=4;
//				}
//
//				if(GetReportParameter()->GetThreshold( lThreahold ))
//				{
//					csvFile.setToField(thresholdField);
//					csvFile.getFloat(value);
//					if (value >= lThreahold)
//						bFlag = true;
//				}
//				break;
//			case ENUM_PAXCOUNT_REP:
//
//				GetReportParameter()->GetReportType( iDetailed );
//				if(iDetailed==0)//detailed
//				{
//					thresholdField=2;
//				}
//				else
//				{
//					thresholdField=4;
//				}
//
//				if(GetReportParameter()->GetThreshold( lThreahold ))
//				{
//					csvFile.setToField(thresholdField);
//					csvFile.getInteger(i);
//					value=static_cast<float>(i);
//					if (value >= lThreahold)
//						bFlag = true;
//				}
//				break;
//				
//				//Threshold type is Time
//			case ENUM_BAGWAITTIME_REP:
//			case ENUM_QUEUETIME_REP:
//			case ENUM_CONVEYOR_WAIT_TIME_REP:
//				GetReportParameter()->GetReportType( iDetailed );
//				if(iDetailed==0)//detailed
//				{
//					thresholdField=2;
//				}
//				else
//				{
//					thresholdField=3;
//				}
//
//				if(GetReportParameter()->GetThreshold( lThreahold ))
//				{
//					csvFile.setToField(thresholdField);
//					csvFile.getField(temp, 19);
//					valuetime.SetTime (temp);
//					if (valuetime.asSeconds() >= lThreahold)
//						bFlag = true;
//				}
//				break;
//			case ENUM_DURATION_REP:
//			case ENUM_SERVICETIME_REP:
//
//				GetReportParameter()->GetReportType( iDetailed );
//				if(iDetailed==0)//detailed
//				{
//					thresholdField=1;
//				}
//				else
//				{
//					thresholdField=3;
//				}
//
//				if(GetReportParameter()->GetThreshold( lThreahold ))
//				{
//					csvFile.setToField(thresholdField);
//					csvFile.getField(temp, 19);
//					valuetime.SetTime (temp);
//					if (valuetime.asSeconds() >= lThreahold)
//						bFlag = true;
//				}
//				break;
//				
//			case ENUM_FIREEVACUTION_REP:
//				GetReportParameter()->GetReportType( iDetailed );
//				if( iDetailed == 0)	// detailed
//					thresholdField = 4;
//				else
//					thresholdField = 2;
//				if( GetReportParameter()->GetThreshold( lThreahold ))
//				{
//					csvFile.setToField( thresholdField );
//					csvFile.getField(temp,19);
//					valuetime.SetTime( temp );
//					if( valuetime.asMinutes() >= lThreahold )
//						bFlag = true;
//				}
//				break;
//			}	
//		}
//
//		m_vFlag.push_back( bFlag );
//
//		//****************************************************************
//
//		m_wndListWnd.SetItemData( nItemIdx, nDBIdx );
//		nDBIdx++;
//    }
//	
//	m_wndListWnd.SetRedraw( TRUE );
//	Invalidate();
//	UpdateWindow();
//	
//    if (strlen (title) && csvFile.getLine () != -1)
//    {
//        strcat (title, ": ");
//        csvFile.copyLine (title + strlen (title));
//    }
//	CTermPlanDoc * pDoc=(CTermPlanDoc *)GetDocument();
//	if(m_wndListWnd.GetItemCount()==0)
//	{	
//		pDoc->m_bShowGraph=FALSE;
//	}
//	else
//	{
//		pDoc->m_bShowGraph=TRUE;
//	}
//}	

void CRepListView::OnInitialUpdate() 
{
	CFormView::OnInitialUpdate();
	// TODO: Add your specialized code here and/or call the base class
	//DWORD dwStyle =dwStyle = m_wndListWnd.GetExtendedStyle();
	//dwStyle |= LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES;
	//m_wndListWnd.SetExtendedStyle( dwStyle );

	//m_ctlHeaderCtrl.SubclassWindow(m_wndListWnd.GetHeaderCtrl()->m_hWnd );
}
void CRepListView::OnSize(UINT nType, int cx, int cy)
{
	CFormView::OnSize(nType,cx,cy);
	
 	if (m_wndListWnd.GetSafeHwnd())
 	{
 		m_wndListWnd.MoveWindow(0,0,cx,cy);
 	}
}

int CRepListView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CFormView::OnCreate(lpCreateStruct) == -1)
		return -1;
	CRect rtEmpty;
	rtEmpty.SetRectEmpty();
	if(m_wndListWnd.Create( LBS_NOTIFY|LBS_HASSTRINGS|WS_CHILD|WS_VISIBLE|WS_BORDER|WS_VSCROLL,
		rtEmpty, this,IDC_LISTCTRL) )
	{
		DWORD dwStyle  = m_wndListWnd.GetExtendedStyle();
		dwStyle |= LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES;
		m_wndListWnd.SetExtendedStyle( dwStyle );

		m_ctlHeaderCtrl.SubclassWindow(m_wndListWnd.GetHeaderCtrl()->m_hWnd );
	}

	return 0;
}

BOOL CRepListView::PreCreateWindow(CREATESTRUCT& cs) 
{
	// TODO: Add your specialized code here and/or call the base class
//	cs.style |= LVS_SHOWSELALWAYS|LVS_REPORT;	
	return CFormView::PreCreateWindow(cs);
}

void CRepListView::OnColumnclick(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here
	int nTestIndex = pNMListView->iSubItem;
	if( nTestIndex >= 0 )
	{
		CWaitCursor	wCursor;
		if(::GetKeyState( VK_CONTROL ) < 0 ) // Is <CTRL> Key Down
			m_ctlHeaderCtrl.SortColumn( nTestIndex, MULTI_COLUMN_SORT );
		else
			m_ctlHeaderCtrl.SortColumn( nTestIndex, SINGLE_COLUMN_SORT );
		m_ctlHeaderCtrl.SaveSortList();
	}		
	*pResult = 0;
}

void CRepListView::ResetAllContent()
{
	//// remove all the columns for the previous view.
	//m_wndListWnd.DeleteAllItems();
	//for( int i=0; i<m_nColumnCount; i++ )
	//{
	//	m_wndListWnd.DeleteColumn( 0 );
	//}
	if(m_pRepListViewBaseOperator)
	{
		m_pRepListViewBaseOperator->ResetAllContent();
	}
}

void CRepListView::OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult) 
{
	//LPNMLVCUSTOMDRAW lplvcd = (LPNMLVCUSTOMDRAW)pNMHDR;
	//
 //   switch(lplvcd->nmcd.dwDrawStage)
	//{
	//case CDDS_PREPAINT:
	//	// Request prepaint notifications for each item.
	//	*pResult = CDRF_NOTIFYITEMDRAW;
	//	break;
	//	
 //   case CDDS_ITEMPREPAINT: // Requested notification
	//	int nDBIdx = m_wndListWnd.GetItemData(lplvcd->nmcd.dwItemSpec);
	//	
	//	if( m_vFlag.size() != 0 && m_vFlag[nDBIdx] )
	//	{
	//		lplvcd->clrText = RGB(255, 0, 0);
	//		//lplvcd->clrTextBk = RGB(255,0,0);
	//	}
	//	*pResult = CDRF_DODEFAULT;
	//	break;
	//}

	if(m_pRepListViewBaseOperator)
		m_pRepListViewBaseOperator->OnCustomDraw(pNMHDR,pResult);

}


void CRepListView::OnListviewMaximize() 
{
	// TODO: Add your command handler code here
	CRect rectMax;
	GetParentFrame()->GetClientRect(&rectMax);
	rectMax.top+=2;
	
	CTermPlanDoc* pDoc = (CTermPlanDoc*)(GetDocument());
	CView * pTempView;
	POSITION pos = pDoc->GetFirstViewPosition();
	while (pos != NULL)
	{
		pTempView = pDoc->GetNextView(pos);
		if(pTempView->IsKindOf(RUNTIME_CLASS(CRepGraphView)))
		{
			pTempView->ShowWindow(SW_HIDE);
		}
		if(pTempView->IsKindOf(RUNTIME_CLASS(CRepControlView)))
		{
			pTempView->ShowWindow(SW_HIDE);
		}
	}
	GetParent()->MoveWindow(&rectMax);
	MoveWindow(&rectMax);
	GetParentFrame()->Invalidate();
}

void CRepListView::OnListviewNormal() 
{
	// TODO: Add your command handler code here

	CTermPlanDoc* pDoc = (CTermPlanDoc*)(GetDocument());
	CView * pTempView;
	POSITION pos = pDoc->GetFirstViewPosition();
	while (pos != NULL)
	{
		pTempView = pDoc->GetNextView(pos);
		if(pTempView->IsKindOf(RUNTIME_CLASS(CRepGraphView)))
		{
			pTempView->ShowWindow(SW_SHOW);
		}
		if(pTempView->IsKindOf(RUNTIME_CLASS(CRepControlView)))
		{
			pTempView->ShowWindow(SW_SHOW);
		}
	}
	GetParentFrame()->ShowWindow(SW_NORMAL);
	GetParentFrame()->ShowWindow(SW_MAXIMIZE);
}

void CRepListView::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	// TODO: Add your message handler code here
	CMenu menuPopup;
	menuPopup.LoadMenu( IDR_MENU_POPUP );
	CMenu* pMenu = menuPopup.GetSubMenu(7);
	
	pMenu->TrackPopupMenu(TPM_LEFTALIGN, point.x, point.y, this);
}

CString CRepListView::NoUnderline( CString _str )
{
	_str.Replace("_"," ");
	return _str;
}

void CRepListView::OnListviewPrint() 
{
	// TODO: Add your command handler code here
	m_sHeader = ((CTermPlanDoc*)GetDocument())->GetARCReportManager().GetRepTitle();
	CString sFileName;
	if(((CTermPlanDoc*)GetDocument())->GetARCReportManager().GetLoadReportType() == load_by_user )
	{
		sFileName = ((CTermPlanDoc*)GetDocument())->GetARCReportManager().GetUserLoadReportFile();
	}
	else								// load_by_system
	{
		sFileName = ((CTermPlanDoc*)GetDocument())->GetTerminal().GetSimReportManager()->GetCurrentReportFileName( ((CTermPlanDoc*)GetDocument())->m_ProjInfo.path );
	}
	ArctermFile chartFile;
	chartFile.init (sFileName, READ);
	
	chartFile.reset();
	chartFile.getLine();
	chartFile.getLine();
	char string[4][128];
	chartFile.getField (string[0], 128);
	chartFile.getField (string[1], 128);
	chartFile.getField (string[2], 128);
	chartFile.getField (string[3], 128);
	m_sFoot.Format("%s %s %s %s",NoUnderline(CString(string[0])),string[1],string[2],string[3]);

 	CDC dc;
 	CPrintDialog printDlg(FALSE, PD_ALLPAGES | PD_USEDEVMODECOPIES | PD_NOPAGENUMS | PD_HIDEPRINTTOFILE);
 	if (m_wndListWnd.GetSelectedCount() == 0)						// if there are no selected items in the list control
 		printDlg.m_pd.Flags = printDlg.m_pd.Flags | PD_NOSELECTION; //then disable the selection radio button
 	else															//else ...
 		printDlg.m_pd.Flags = printDlg.m_pd.Flags | PD_SELECTION;
 
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
 	info.m_rectDraw.SetRect( 0,0,dc.GetDeviceCaps( HORZRES ),dc.GetDeviceCaps( VERTRES ));
 	
 	OnBeginPrinting( &dc, &info );
	if(printDlg.PrintSelection() == FALSE || m_wndListWnd.GetSelectedCount() == 1)	//print all.
		m_nCurrentPrintCount = 1;
	else 
	{
		POSITION ps  =  m_wndListWnd.GetFirstSelectedItemPosition();
		if(ps == NULL) 
			return;
		m_nCurrentPrintCount = m_wndListWnd.GetNextSelectedItem(ps) + 1;
	}	
	
	info.SetMaxPage(info.GetMinPage());
 	for( UINT page = info.GetMinPage(); page <= info.GetMaxPage() && bPrintingOK; page++ )
 	{
 		info.m_nCurPage = page;
		dc.StartPage();
 		if(printDlg.PrintSelection() == FALSE || m_wndListWnd.GetSelectedCount() == 1)	//print all.
 			OnPrint( &dc, &info );
 		else 
 			PrintSelectedItems(&dc, &info);				//print only the selected items.
		dc.EndPage();
 		bPrintingOK = TRUE;
 	}
 	OnEndPrinting( &dc, &info );
 	
 	if( bPrintingOK )
 		dc.EndDoc();
 	else
 		dc.AbortDoc();
 	dc.Detach();

}

void CRepListView::OnPrint(CDC *pDC, CPrintInfo *pInfo)
{
	CString line; //This is the print line
	TEXTMETRIC metrics; //Font measurements
	int y = 0; //Current y position on report
	CFont TitleFont; //Font for Title
	CFont HeadingFont; //Font for headings
	CFont DetailFont; //Font for detail lines
	CFont FooterFont; //Font for footer
	//Tab stops at 1 inch, 2.5 inches, and 6.5 inches
    //	int TabStops[] = {100, 200,300,400,500};
	//Tab stops at 3.5 inches and 6.5 inches
	int FooterTabStops[] = {350, 650};
	if (!pInfo || pInfo->m_nCurPage == 1) {
		//Set the recordset at the beginning
	}
	short cxInch=pDC->GetDeviceCaps(LOGPIXELSX); 
	short cyInch=pDC->GetDeviceCaps(LOGPIXELSY); 
	
#define MYFONTSIZE 14 
#define HFONTNAME "MS Sans Serif"
	//
	int nFontHeight=MulDiv(MYFONTSIZE, -cyInch, 72); 
	if(nFontHeight % 2) nFontHeight++; 
	
	if(!TitleFont.CreateFont(static_cast<int>(nFontHeight*0.8), 0, 0, 0, FW_NORMAL, 0, 0,0, \
		GB2312_CHARSET, OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS, \
		DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, HFONTNAME)) 
		return ; 
	
	if(!HeadingFont.CreateFont(static_cast<int>(nFontHeight*0.7), 0, 0, 0, FW_NORMAL, 0, 0,0, \
		GB2312_CHARSET, OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS, \
		DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, HFONTNAME)) 
		return ; 
	
	if(!DetailFont.CreateFont(static_cast<int>(nFontHeight*0.7), 0, 0, 0, FW_NORMAL, 0, 0,0, \
		GB2312_CHARSET, OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS, \
		DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, HFONTNAME)) 
		return ; 
	
	if(!FooterFont.CreateFont(nFontHeight/2, 0, 0, 0, FW_NORMAL, 0, 0,0, \
		GB2312_CHARSET, OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS, \
		DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, HFONTNAME)) 
		return ;
	
	// Get the system's default printer's setting
	int	nPageVMargin,nPageHMargin,nPageHeight,nPageWidth;
	nPageVMargin = pDC->GetDeviceCaps(LOGPIXELSY) / 2;
	nPageHMargin = pDC->GetDeviceCaps(LOGPIXELSX) / 2;
	nPageHeight  = pDC->GetDeviceCaps(VERTRES);
	nPageWidth   = pDC->GetDeviceCaps(HORZRES);
	
	//Capture default settings when setting the footer font	
	CFont* OldFont = (CFont*)pDC->SelectObject(&FooterFont);	
	pDC->GetTextMetrics(&metrics);
	int nFooterHeight = metrics.tmHeight + metrics.tmExternalLeading;
	pDC->SelectObject(&TitleFont);
	//Retrieve the heading font measurements
	pDC->GetTextMetrics(&metrics);
	//Compute the heading line height
	int nLineHeight = metrics.tmHeight + metrics.tmExternalLeading;
	//Set Y to the line height.
	y += static_cast<int>(nLineHeight*1.5+nPageVMargin);
	pDC->TextOut(nPageHMargin, nPageVMargin, m_sHeader);
	//Draw a line under the heading
	pDC->MoveTo(nPageHMargin,y-nLineHeight/2);
	pDC->LineTo(nPageWidth-nPageHMargin,y-nLineHeight/2);
	//Set the Heading font
	pDC->SelectObject(&HeadingFont);
    y += nLineHeight;

	//Format the heading
	// get listctrl's header info
    int nCol = m_wndListWnd.GetHeaderCtrl()->GetItemCount();
	HDITEM hi;
	hi.mask = HDI_TEXT ;
	hi.cchTextMax = 19;
	char chBuffer[20];
	hi.pszText = chBuffer;
	int nSpace=(nPageWidth-2*nPageHMargin) / nCol;
	for(int i = 0; i < nCol; i++ )
	{
		::ZeroMemory( chBuffer, 20);
		m_wndListWnd.GetHeaderCtrl()->GetItem( i,&hi);
		pDC->TextOut(nPageHMargin+i*nSpace, y,hi.pszText);
	}
	
	//Compute the detail line height
	nLineHeight = metrics.tmHeight + metrics.tmExternalLeading;
	y += 2*nLineHeight; //Adjust y position
	//Set the detail font
	pDC->SelectObject(&DetailFont);
	//Retrieve detail font measurements
	pDC->GetTextMetrics(&metrics);
	//Compute the detail line height
	nLineHeight = metrics.tmHeight + metrics.tmExternalLeading;
	
	//Scroll through the list
	int nIndex=m_nCurrentPrintCount;
	int nRowCount = m_wndListWnd.GetItemCount();
	while ( nIndex <= nRowCount ) 
	{
		if (pInfo && y > nPageHeight-nPageVMargin-nFooterHeight-nLineHeight-6) 
		{   // start a new page
			pInfo->SetMaxPage(pInfo->m_nCurPage + 1);
			break;
		}
		//Format the detail line
		for( int nSubitem = 0; nSubitem < nCol; nSubitem++ )
		{
			
			char chLine[20];
			m_wndListWnd.GetItemText(nIndex-1,nSubitem,chLine,19);
			pDC->TextOut(nPageHMargin+nSubitem*nSpace, y,chLine);
		}
		
		y += nLineHeight; //Adjust y position
		nIndex++; 
	}
	m_nCurrentPrintCount = nIndex;
	if (pInfo) {
		//Draw a line above the footer
		pDC->MoveTo(nPageHMargin,static_cast<int>(nPageHeight-nPageVMargin-nFooterHeight*1.5));
		pDC->LineTo(nPageWidth-nPageHMargin,static_cast<int>(nPageHeight-nPageVMargin-nFooterHeight*1.5));
		//Set the footer font
		pDC->SelectObject(&FooterFont);
		//Format the footer
		line.Format(" \tPage %d",pInfo->m_nCurPage);
		line = m_sFoot + line;
		//Output the footer at the bottom using tabs
		pDC->TabbedTextOut(nPageHMargin, nPageHeight-nPageVMargin-nFooterHeight, line, 2, FooterTabStops, 0);
	}
	//Restore default settings
	pDC->SelectObject(OldFont);
}

void CRepListView::PrintSelectedItems(CDC *pDC, CPrintInfo *pInfo)
{
	CString line; //This is the print line
	TEXTMETRIC metrics; //Font measurements
	int y = 0; //Current y position on report
	CFont TitleFont; //Font for Title
	CFont HeadingFont; //Font for headings
	CFont DetailFont; //Font for detail lines
	CFont FooterFont; //Font for footer
	//Tab stops at 1 inch, 2.5 inches, and 6.5 inches
    //	int TabStops[] = {100, 200,300,400,500};
	//Tab stops at 3.5 inches and 6.5 inches
	int FooterTabStops[] = {350, 650};
	if (!pInfo || pInfo->m_nCurPage == 1) {
		//Set the recordset at the beginning
	}
	short cxInch=pDC->GetDeviceCaps(LOGPIXELSX); 
	short cyInch=pDC->GetDeviceCaps(LOGPIXELSY); 
	
#define MYFONTSIZE 14 
#define HFONTNAME "MS Sans Serif"
	//
	int nFontHeight=MulDiv(MYFONTSIZE, -cyInch, 72); 
	if(nFontHeight % 2) nFontHeight++; 
	
	if(!TitleFont.CreateFont(static_cast<int>(nFontHeight*0.8), 0, 0, 0, FW_NORMAL, 0, 0,0, \
		GB2312_CHARSET, OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS, \
		DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, HFONTNAME)) 
		return ; 
	
	if(!HeadingFont.CreateFont(static_cast<int>(nFontHeight*0.7), 0, 0, 0, FW_NORMAL, 0, 0,0, \
		GB2312_CHARSET, OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS, \
		DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, HFONTNAME)) 
		return ; 
	
	if(!DetailFont.CreateFont(static_cast<int>(nFontHeight*0.7), 0, 0, 0, FW_NORMAL, 0, 0,0, \
		GB2312_CHARSET, OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS, \
		DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, HFONTNAME)) 
		return ; 
	
	if(!FooterFont.CreateFont(nFontHeight/2, 0, 0, 0, FW_NORMAL, 0, 0,0, \
		GB2312_CHARSET, OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS, \
		DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, HFONTNAME)) 
		return ;
	
	// Get the system's default printer's setting
	int	nPageVMargin,nPageHMargin,nPageHeight,nPageWidth;
	nPageVMargin = pDC->GetDeviceCaps(LOGPIXELSY) / 2;
	nPageHMargin = pDC->GetDeviceCaps(LOGPIXELSX) / 2;
	nPageHeight  = pDC->GetDeviceCaps(VERTRES);
	nPageWidth   = pDC->GetDeviceCaps(HORZRES);
	
	//Capture default settings when setting the footer font	
	CFont* OldFont = (CFont*)pDC->SelectObject(&FooterFont);	
	pDC->GetTextMetrics(&metrics);
	int nFooterHeight = metrics.tmHeight + metrics.tmExternalLeading;
	pDC->SelectObject(&TitleFont);
	//Retrieve the heading font measurements
	pDC->GetTextMetrics(&metrics);
	//Compute the heading line height
	int nLineHeight = metrics.tmHeight + metrics.tmExternalLeading;
	//Set Y to the line height.
	y += static_cast<int>(nLineHeight*1.5+nPageVMargin);
	pDC->TextOut(nPageHMargin, nPageVMargin, m_sHeader);
	//Draw a line under the heading
	pDC->MoveTo(nPageHMargin,y-nLineHeight/2);
	pDC->LineTo(nPageWidth-nPageHMargin,y-nLineHeight/2);
	//Set the Heading font
	pDC->SelectObject(&HeadingFont);
    y += nLineHeight;

	//Format the heading
	// get listctrl's header info
    int nCol =  m_wndListWnd.GetHeaderCtrl()->GetItemCount();
	HDITEM hi;
	hi.mask = HDI_TEXT ;
	hi.cchTextMax = 19;
	char chBuffer[20];
	hi.pszText = chBuffer;
	int nSpace=(nPageWidth-2*nPageHMargin) / nCol;
	for(int i = 0; i < nCol; i++ )
	{
		::ZeroMemory( chBuffer, 20);
		m_wndListWnd.GetHeaderCtrl()->GetItem( i,&hi);
		pDC->TextOut(nPageHMargin+i*nSpace, y,hi.pszText);
	}
	
	//Compute the detail line height
	nLineHeight = metrics.tmHeight + metrics.tmExternalLeading;
	y += 2*nLineHeight; //Adjust y position
	//Set the detail font
	pDC->SelectObject(&DetailFont);
	//Retrieve detail font measurements
	pDC->GetTextMetrics(&metrics);
	//Compute the detail line height
	nLineHeight = metrics.tmHeight + metrics.tmExternalLeading;
	
	//Scroll through the list
	int nIndex=m_nCurrentPrintCount;
	POSITION ps  =  m_wndListWnd.GetFirstSelectedItemPosition();
	if(ps == NULL) 
		return;
	int curSelectedItem = m_wndListWnd.GetNextSelectedItem(ps);
	int nRowCount = curSelectedItem + m_wndListWnd.GetSelectedCount();
	while ( nIndex <= nRowCount ) 
	{
		if (pInfo && y > nPageHeight-nPageVMargin-nFooterHeight-nLineHeight-6) 
		{   // start a new page
			pInfo->SetMaxPage(pInfo->m_nCurPage + 1);
			break;
		}
		//Format the detail line
		for( int nSubitem = 0; nSubitem < nCol; nSubitem++ )
		{
			
			char chLine[20];
			m_wndListWnd.GetItemText(nIndex-1, nSubitem, chLine, 19);
			pDC->TextOut(nPageHMargin+nSubitem*nSpace, y,chLine);
		}
	//	curSelectedItem = m_wndListWnd.GetNextSelectedItem(ps);
		y += nLineHeight; //Adjust y position
		nIndex++; 
	}
	m_nCurrentPrintCount = nIndex;
	if (pInfo) {
		//Draw a line above the footer
		pDC->MoveTo(nPageHMargin,static_cast<int>(nPageHeight-nPageVMargin-nFooterHeight*1.5));
		pDC->LineTo(nPageWidth-nPageHMargin,static_cast<int>(nPageHeight-nPageVMargin-nFooterHeight*1.5));
		//Set the footer font
		pDC->SelectObject(&FooterFont);
		//Format the footer
		line.Format(" \tPage %d",pInfo->m_nCurPage);
		line = m_sFoot + line;
		//Output the footer at the bottom using tabs
		pDC->TabbedTextOut(nPageHMargin, nPageHeight-nPageVMargin-nFooterHeight, line, 2, FooterTabStops, 0);
	}
	//Restore default settings
	pDC->SelectObject(OldFont);
}
void CRepListView::OnListviewExport() 
{
	if(m_pRepListViewBaseOperator)
		m_pRepListViewBaseOperator->OnListviewExport();

//	if(m_sExtension.IsEmpty())		//If no items listed, then return;
//		return;
//	CString upExtension = m_sExtension;
//	upExtension.MakeUpper();
//	CFileDialog filedlg( FALSE,m_sExtension, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
//		"Report files (*." + m_sExtension + ") | *." + m_sExtension + ";*." + upExtension + "|All type (*.*)|*.*|", NULL );
//	if(filedlg.DoModal()!=IDOK)
//		return;
//	CString csFileName=filedlg.GetPathName();
//	
//    CString sFileName;
//	if( ((CTermPlanDoc*)GetDocument())->GetARCReportManager().GetLoadReportType() == load_by_user )	
//	{
//		sFileName = ((CTermPlanDoc*)GetDocument())->GetARCReportManager().GetUserLoadReportFile();
//	}
//	else				  // load_by_system
//	{
//		sFileName = ((CTermPlanDoc*)GetDocument())->GetTerminal().GetSimReportManager()->GetCurrentReportFileName( ((CTermPlanDoc*)GetDocument())->m_ProjInfo.path );
//	}
//
//	CFileFind fileFind;
//	if(fileFind.FindFile(csFileName))
//	{
//		if(MessageBox("File already exists, are you sure to overwrite it?", "Warning!", MB_YESNO) == IDNO)
//			return;
//	}
//	CopyFile(sFileName, csFileName, TRUE);
//
//	// unassign
//	// call saveDataSetToOtherFile()
////	GetInputTerminal()->flightSchedule->saveDataSetToOtherFile( csFileName );
}
 
void CRepListView::ShowReportProcessFlow(MathResult& result)
{
	if(m_pRepListViewBaseOperator)
		m_pRepListViewBaseOperator->ShowReportProcessFlow(result);

//	m_wndListWnd.DeleteAllItems();
//	m_wndListWnd.SetRedraw( FALSE );
//
//	m_nColumnCount = result.m_vstrProcess.size();
////	m_nColumnCount = 3;//test
//	if(m_nColumnCount == 0)
//	{
//		m_wndListWnd.SetRedraw( TRUE );
//        Invalidate();
//        return ;
//	}
//
//	EDataType type = dtDATETIME;
//	m_wndListWnd.InsertColumn(0, "Time", LVCFMT_CENTER, 60);
//	m_ctlHeaderCtrl.SetDataType(0, type);
//	
//
//	int nWidth = 80;
//	int i = 0;
//	for( i = 0; i < m_nColumnCount; i++ )
//	{
//		type = dtSTRING;
//		m_wndListWnd.InsertColumn( i+1, (CString)result.m_vstrProcess[i].c_str(), LVCFMT_CENTER, nWidth );
//		m_ctlHeaderCtrl.SetDataType( i+1, type );
//	}
//
//	const std::vector<MathResultRecord> &vRecord = result.m_vResult;
//	int nRow = vRecord.size();
////	nRow = 5;//test
//	CString strProcResult = "";
//	for( i = 0; i < nRow; i++ )
//	{
//		int nItemIdx = m_wndListWnd.InsertItem( i, GetAbsDateTime(vRecord[i].time.printTime()));//test
//
//		const std::vector<double>& vProcResults = vRecord[i].vProcResults;
//		for( int j = 0; j < m_nColumnCount; j++ )
//		{
//			strProcResult.Format("%.2f", vProcResults[j]);
////			strProcResult = "vProcResults[i]";//test
//			m_wndListWnd.SetItemText( nItemIdx, j+1, strProcResult );
//		}
//	}	
//
//	m_wndListWnd.SetRedraw( TRUE );
//	Invalidate();
//	UpdateWindow();
}

void CRepListView::LoadReportProcessFlow(CMathResultManager* pManager, enum ENUM_REPORT_TYPE _enumRepType)
{

	if(m_pRepListViewBaseOperator)
		m_pRepListViewBaseOperator->LoadReportProcessFlow(pManager,_enumRepType);

///*	case ENUM_QUEUETIME_REP:
//	case ENUM_QUEUELENGTH_REP:
//	case ENUM_THROUGHPUT_REP:
//	case ENUM_PASSENGERNO_REP:
//	case ENUM_UTILIZATION_REP:
//*/	
//	if(pManager == NULL)
//		return ;
//
//	switch(_enumRepType)
//	{
//	case ENUM_QUEUETIME_REP:
//		{
//			MathResult &result = pManager->GetQTimeResult();
//			ShowReportProcessFlow(result);
//			break;
//		}
//	case ENUM_QUEUELENGTH_REP:
//		{
//			MathResult &result = pManager->GetQLengthResult();
//			ShowReportProcessFlow(result);
//			break;
//		}
//	case ENUM_THROUGHPUT_REP:
//		{
//			MathResult &result = pManager->GetThroughputResult();
//			ShowReportProcessFlow(result);
//			break;
//		}
//	case ENUM_PASSENGERNO_REP:
//		{
//			MathResult &result = pManager->GetPaxCountResult();
//			ShowReportProcessFlow(result);
//			break;
//		}
//	case ENUM_UTILIZATION_REP:
//		{
//			MathResult &result = pManager->GetUtilizationResult();
//			ShowReportProcessFlow(result);
//			break;
//		}
//	default:
//		break;
//	}
//	
//	return ;
}

//CString CRepListView::GetAbsDateTime(LPCTSTR elaptimestr, BOOL needsec)
//{
//	CSimAndReportManager *ptSim = ((CTermPlanDoc*)GetDocument())->GetTerminal().GetSimReportManager();
//	CStartDate tstartdate = ptSim->GetStartDate();
//	ElapsedTime etime;
//	etime.SetTime(elaptimestr);
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

CReportParameter* CRepListView::GetReportParameter()
{
	CTermPlanDoc * pDoc=(CTermPlanDoc *)GetDocument();
	return pDoc->GetARCReportManager().GetReportPara();
}