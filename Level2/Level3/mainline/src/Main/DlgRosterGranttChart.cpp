// DlgRosterGranttChart.cpp : implementation file
//

#include "stdafx.h"
#include "TermPlan.h"
#include "DlgRosterGranttChart.h"
#include "Inputs\In_term.h"
#include "Inputs\Schedule.h"
#include "Inputs\AutoRosterRule.h"
#include "Inputs\Prochrch.h"
#include "Inputs\Assign.h"
#include "Inputs\Assigndb.h"
#include "Engine\Airside\AirsideItinerantFlightInSim.h"
#include "Inputs\MobileElemConstraint.h"
#include ".\RosterAssignSummaryDlg.h"
#include "DlgAutoRosterRules.h"

extern const CString c_setting_regsectionstring;
const CString c_autoroster_sorting_entry = "Auto Roster Sorting String";
// CDlgRosterGranttChart dialog

IMPLEMENT_DYNAMIC(CDlgRosterGranttChart, CDialog)
CDlgRosterGranttChart::CDlgRosterGranttChart(CProcRosterRules* pProcRosterRules, std::vector<ProcessorID>& vProcID, InputTerminal* pInTerm,CWnd* pParent /*=NULL*/)
	: CDialog(CDlgRosterGranttChart::IDD, pParent)
	,m_pInTerm(pInTerm)
	,m_pProcRosterRules(pProcRosterRules)
	,m_vProcID(vProcID)
{
	m_pDragImage = NULL;
	m_nMaxScheduleDay = 1;
	m_bDragging = false;
	m_nDragIndex = 0 ;
	m_pDragFlight = NULL;
}

CDlgRosterGranttChart::~CDlgRosterGranttChart()
{
}

void CDlgRosterGranttChart::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_BUTTON_UNASSIGNALL, m_btnUnassignAll);
	DDX_Control(pDX, IDC_BUTTON_SUMMARY, m_btnSummary);
	DDX_Control(pDX, IDC_BUTTON_PRIORITY_FLIGHT, m_btnAutoAssign);
	DDX_Control(pDX, IDC_BUTTON_SAVE, m_btnSave);
	DDX_Control(pDX, IDOK, m_btnOk);
	DDX_Control(pDX, IDCANCEL, m_btnCancel);
	DDX_Control(pDX, IDC_LIST_FLIGHT, m_listctrlFlightList);
	DDX_Control(pDX, IDC_ROSTERGTCHARTCTRL , m_gtcharCtrl);

	DDX_Control(pDX, IDC_STATIC_ASSIGNPRO , m_stProgAssignStatus);
	DDX_Control(pDX, IDC_PROGRESS_ASSIGNMENT, m_progAssignStatus);


}


BEGIN_MESSAGE_MAP(CDlgRosterGranttChart, CDialog)
	ON_WM_SIZE()
	ON_NOTIFY(LVN_BEGINDRAG, IDC_LIST_FLIGHT, OnBegindragListFlight)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_BN_CLICKED(IDC_BUTTON_SUMMARY, OnBnClickedButtonSummary)
	ON_BN_CLICKED(IDC_BUTTON_SAVE,OnBtnSave)
	ON_BN_CLICKED(IDC_BUTTON_PRIORITY_FLIGHT, OnBnClickedButtonAutoAssign)
	ON_BN_CLICKED(IDC_BUTTON_UNASSIGNALL, OnBnClickedButtonUnassignAll)

END_MESSAGE_MAP()

BEGIN_EVENTSINK_MAP(CDlgRosterGranttChart, CDialog)
	//{{AFX_EVENTSINK_MAP(CGateAssignDlg)
	ON_EVENT(CDlgRosterGranttChart, IDC_ROSTERGTCHARTCTRL, 4 /* ClickBtnAddItem */, OnClickBtnAddItemGtchartctrl, VTS_BOOL VTS_I4)
	ON_EVENT(CDlgRosterGranttChart, IDC_ROSTERGTCHARTCTRL, 6 /* ClickBtnDelItem */, OnClickBtnDelItemGtchartctrl, VTS_BOOL VTS_I4 VTS_I4 VTS_BSTR VTS_BSTR VTS_I4 VTS_I4 VTS_I4)
	ON_EVENT(CDlgRosterGranttChart, IDC_ROSTERGTCHARTCTRL, 7 /* DblClickItem */, OnDblClickItemGtchartctrl, VTS_I4 VTS_I4 VTS_BSTR VTS_BSTR VTS_I4 VTS_I4 VTS_I4)
	ON_EVENT(CDlgRosterGranttChart, IDC_ROSTERGTCHARTCTRL, 9 /* ItemResized */, OnItemResizedGtchartctrl, VTS_I4 VTS_I4 VTS_BSTR VTS_BSTR VTS_I4 VTS_I4)
	ON_EVENT(CDlgRosterGranttChart, IDC_ROSTERGTCHARTCTRL, 10 /* ItemMoved */, OnItemMovedGtchartctrl, VTS_I4 VTS_I4 VTS_I4 VTS_BSTR VTS_BSTR VTS_I4 VTS_I4)
	ON_EVENT(CDlgRosterGranttChart, IDC_ROSTERGTCHARTCTRL, 16 /* ShowItemToolTip */, OnShowItemToolTipGtchartctrl, VTS_I4 VTS_I4 VTS_I4)
	ON_EVENT(CDlgRosterGranttChart, IDC_ROSTERGTCHARTCTRL, 17 /* RClick */, OnRClickGtchartctrl, VTS_I4 VTS_I4)
	ON_EVENT(CDlgRosterGranttChart, IDC_ROSTERGTCHARTCTRL, 18 /* ItemCheck */, OnCheckFlightOperationMoveGtchartctral, VTS_I4 VTS_I4 VTS_I4 VTS_PI4)
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()

BOOL CDlgRosterGranttChart::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetWindowText(_T("Auto Roster Assignment Grantt Chart"));
	m_progAssignStatus.ShowWindow(SW_HIDE);
	m_stProgAssignStatus.ShowWindow(SW_HIDE);

	InitFlightListCtrl();
	InitGTCtrlLayout();
	LoadGranttChart();
	LoadFlightList();

	m_btnAutoAssign.EnableWindow(FALSE);

	return TRUE;
}


// CDlgRosterGranttChart message handlers
void CDlgRosterGranttChart::InitGTCtrlLayout()
{
	m_gtcharCtrl.InitControl( (long) AfxGetThread() );
	m_gtcharCtrl.SetEnableOverlap( TRUE );
	m_gtcharCtrl.SetMoveItemSnapLineable(true);
	m_gtcharCtrl.SetItemColorGradual(true);
	m_gtcharCtrl.ShowCaptionInItemTooltip(FALSE);

	m_gtcharCtrl.ShowBtn(0,false);
	m_gtcharCtrl.ShowBtn(2,false);
	m_gtcharCtrl.ShowBtn(5,false);
	m_gtcharCtrl.ShowBtn(6,false);
	m_gtcharCtrl.ShowBtn(9,false);
	m_gtcharCtrl.ShowBtn(10,false);
	m_gtcharCtrl.ShowBtn(11,false);
	m_gtcharCtrl.ShowBtn(12,false);
	m_gtcharCtrl.ShowBtn(15,false);
	m_gtcharCtrl.ShowBtn(16,false);
	m_gtcharCtrl.ShowBtn(17,false);

	m_gtcharCtrl.SetResourceBarCaption(_T("Processors"));

	if(m_pInTerm == NULL)
		return;

	FlightSchedule* fs = m_pInTerm->flightSchedule;
	if (fs->getCount() < 1)
		return;

	m_nMaxScheduleDay = fs->GetFlightScheduleEndTime().GetDay();
	int nHours = m_nMaxScheduleDay * 24;
	m_gtcharCtrl.SetRulerMax(nHours);

}
void CDlgRosterGranttChart::InitFlightListCtrl()
{
	//initial flight list
	DWORD dwStyle = m_listctrlFlightList.GetExtendedStyle();
	dwStyle |= LVS_EX_HEADERDRAGDROP|LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT;
	m_listctrlFlightList.SetExtendedStyle( dwStyle );
	char* columnLabel[] = {"Pax Type",
		"Operation",
		"Airline", 
		"Arr ID", 
		"Origin", 
		"Intermediate Origin",
		"Arr Time", 
		"Dep ID",
		"Intermediate Dest",
		"Destination",
		"Dep Time",
		"Ac Type",
		"Mode",
		"Duration",
		"Number of Processors"};
	int DefaultColumnWidth[] = {160,60, 95, 80, 80, 100, 100, 80, 100, 80, 100, 85, 80, 85,120 };
	int nFormat[] = {LVCFMT_CENTER, LVCFMT_CENTER,	LVCFMT_CENTER, LVCFMT_CENTER, LVCFMT_CENTER, LVCFMT_CENTER, 
		LVCFMT_CENTER, LVCFMT_CENTER, LVCFMT_CENTER, LVCFMT_CENTER,LVCFMT_CENTER,LVCFMT_CENTER,
		LVCFMT_CENTER, LVCFMT_CENTER , LVCFMT_CENTER};

	EDataType type[] = {dtSTRING, dtSTRING, dtSTRING, dtSTRING, dtSTRING, dtDATETIME, dtSTRING, 
		dtSTRING, dtDATETIME, dtSTRING, dtSTRING, dtSTRING, dtSTRING, dtSTRING, dtSTRING };

	// column
	//m_ctlHeaderCtrl.SubclassWindow( m_listctrlFlightList.GetHeaderCtrl()->m_hWnd );
	for( int i=0; i<15; i++ )
	{
		m_listctrlFlightList.InsertColumn( i, columnLabel[i], nFormat[i], DefaultColumnWidth[i] );
		m_listctrlFlightList.SetDataType( i, CSortAndPrintListCtrl::ITEM_STRING );
	}
	//m_ctlHeaderCtrl.SetRegString( c_setting_regsectionstring, c_autoroster_sorting_entry );
	
	m_listctrlFlightList.SubClassHeadCtrl();

	m_imageList.Create(16, 16, ILC_MASK, 0, 4);
	m_imageList.Add( AfxGetApp()->LoadIcon( IDI_ICON_AIRPLANE ) );
	m_listctrlFlightList.SetImageList( &m_imageList, LVSIL_SMALL );

	return;
}

void CDlgRosterGranttChart::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
	CRect btnrc,listrc,rc;
	m_listctrlFlightList.GetWindowRect( &listrc );
	m_btnSave.GetWindowRect( &btnrc );

	m_progAssignStatus.GetWindowRect(&rc);

	m_gtcharCtrl.MoveWindow(7,200,cx-14,cy-250 );
	m_listctrlFlightList.MoveWindow( 7,25,cx-14,listrc.Height() );

	m_progAssignStatus.MoveWindow( 550, listrc.Height()+35, cx-560, rc.Height());

	m_btnUnassignAll.MoveWindow(cx-35-6*btnrc.Width(),cy-8-btnrc.Height(), btnrc.Width(),btnrc.Height());
	m_btnSummary.MoveWindow(cx-30-5*btnrc.Width(), cy-8-btnrc.Height(), btnrc.Width(),btnrc.Height());
	m_btnAutoAssign.MoveWindow(cx-25-4*btnrc.Width(), cy-8-btnrc.Height(), btnrc.Width(),btnrc.Height());
	m_btnSave.MoveWindow( cx-20-3*btnrc.Width(),cy-8-btnrc.Height(),btnrc.Width(),btnrc.Height() );
	m_btnOk.MoveWindow( cx-15-2*btnrc.Width(),cy-8-btnrc.Height(),btnrc.Width(),btnrc.Height() );
	m_btnCancel.MoveWindow( cx-10-btnrc.Width(),cy-8-btnrc.Height(),btnrc.Width(),btnrc.Height() );

	CRect rect( 0,0,cx,cy );
	InvalidateRect( rect );
}

void CDlgRosterGranttChart::LoadGranttChart()
{

	m_vProAssignEntries.clear();
	ProcAssignDatabase* pAssignDB = m_pInTerm->procAssignDB ;
	ProcAssignEntry* pEntry = NULL ;

	size_t nSize = m_vProcID.size();
	for (size_t i = 0 ; i < nSize ; i++)
	{
		ProcessorID procID = m_vProcID.at(i);
		int nIdx = pAssignDB->findEntry(procID) ;
		if(nIdx ==INT_MAX)
		{
			pAssignDB->addEntry(procID) ;
			nIdx  = pAssignDB->findEntry(procID) ;	
			pEntry = (ProcAssignEntry*)pAssignDB->getItem(nIdx) ;
		}
		else
		{
			pEntry = (ProcAssignEntry*)pAssignDB->getItem(nIdx) ;
			pEntry->getSchedule()->ClearAllAssignment() ;
		}
		m_vProAssignEntries.push_back(pEntry);
	}
	m_pProcRosterRules->DoAutoRosterAssignment(m_vProAssignEntries, m_lstAssignFlights);



	m_gtcharCtrl.Clear(FALSE);

	int nProcRosterEntryCount = m_vProAssignEntries.size();
	
	for (int nProcEntry = 0; nProcEntry < nProcRosterEntryCount; ++ nProcEntry)
	{
		ProcAssignEntry *pProcEntry = m_vProAssignEntries[nProcEntry];
		if(pProcEntry == NULL)
			continue;

		const ProcessorID *pProcID = pProcEntry->getID();
		if(pProcID == NULL)
			continue;
		int ndx =  m_pInTerm->procAssignDB->findEntry(*pProcID) ;
		ProcessorRosterSchedule* _schedule =  m_pInTerm->procAssignDB->getDatabase(ndx) ;

		AddProcessor(_schedule, *pProcID);

	}



}
void CDlgRosterGranttChart::AddProcessor(ProcessorRosterSchedule* _schedule,const ProcessorID& id)
{
	ProcessorRoster* _ass = NULL;
	int lLineIdx = m_gtcharCtrl.AddLine( id.GetIDString(),NULL);
	for (int i =0 ; i < _schedule->getCount() ; i++)
	{
		_ass = _schedule->getItem(i) ;
		if(_ass->getFlag() != 0)
		{
			m_gtcharCtrl.SetLineData( lLineIdx , (long)id.getID() );
			m_gtcharCtrl.SetLineModifyable( lLineIdx,true);
			AddItemToGT(lLineIdx,_ass) ;
		}
	}
}
void CDlgRosterGranttChart::AddLineToGT(ProcessorRoster* _Ass,ProcessorID& id)
{
	int lLineIdx = m_gtcharCtrl.AddLine( id.GetIDString(),NULL);
	m_gtcharCtrl.SetLineData( lLineIdx , (long)id.getID() );
	m_gtcharCtrl.SetLineModifyable( lLineIdx,true);
	AddItemToGT(lLineIdx,_Ass) ;


}
void CDlgRosterGranttChart::AddItemToGT(int lineId , ProcessorRoster* _Ass)
{
	if(_Ass == NULL)
		return ;
	const CMultiMobConstraint* pMultiConst = _Ass->getAssignment();	
	const CMobileElemConstraint* pConst = pMultiConst->getConstraint( 0 );
	CString  paxTy ;
	pConst->screenPrint( paxTy );
	long ndx = 0 ;
	int idx = 0;
	if(_Ass->IsDaily())
	{
		for(int i = 0 ; i < m_nMaxScheduleDay ; i++)
			m_gtcharCtrl.AddItem(&ndx,lineId,paxTy.GetBuffer(0),NULL,ElapsedTime(i*WholeDay).asMinutes() +  _Ass->getAbsOpenTime().asMinutes(),(long)(_Ass->getAbsCloseTime() - _Ass->getAbsOpenTime()).asMinutes(),TRUE ,RGB(255,255,255),RGB(132,163,195)) ;
	}
	else
	{
		idx = m_gtcharCtrl.AddItem(&ndx,lineId,paxTy.GetBuffer(0),NULL, _Ass->getAbsOpenTime().asMinutes(),(long)(_Ass->getAbsCloseTime() - _Ass->getAbsOpenTime()).asMinutes(),TRUE ,RGB(255,255,255),RGB(132,163,195)) ;
	}

	int nCount = m_gtcharCtrl.GetItemNum();
	m_gtcharCtrl.SetItemData(ndx,(long)_Ass) ;
}



//void CDlgRosterGranttChart::InsertRosterOfProcessor(HTREEITEM ProcessNode )
//{
//	ProcessorID _id ;
//	if(!m_mainDlg->GetCurrentSelProcessID(ProcessNode,_id))
//		return ;
//	ProcessorRosterSchedule* _schedule = NULL ;
//	int ndx =  m_mainDlg->GetInputTerminal()->procAssignDB->findEntry(_id) ;
//	if(ndx == INT_MAX)
//	{
//		ProcessorID id ;
//		if(!m_mainDlg->FindDefinedRosterProcessor(ProcessNode,id) )
//		{
//			ElapsedTime time ;
//			ProcessorRoster _ass ;
//			CMultiMobConstraint assign;	
//			assign.addConstraint(CMobileElemConstraint(m_mainDlg->GetInputTerminal())) ;
//			_ass.setAbsOpenTime(time) ;
//			CMobileElemConstraint constraint(m_mainDlg->GetInputTerminal());
//			_ass.setAssignment(assign);
//			time = time + (WholeDay-1);
//			_ass.setAbsCloseTime(time) ;
//			_ass.setAssRelation(Inclusive) ;
//			_ass.IsDaily(TRUE) ;
//			AddLineToGT(&_ass,_id) ;
//			return ;
//		}
//		else
//		{
//			ndx =  m_mainDlg->GetInputTerminal()->procAssignDB->findEntry(id) ;
//			ASSERT(ndx != INT_MAX);
//			if(ndx == INT_MAX)
//				return ;
//			_schedule = m_mainDlg->GetInputTerminal()->procAssignDB->getDatabase(ndx) ;
//		}
//	}
//	else
//	{
//		ndx =  m_mainDlg->GetInputTerminal()->procAssignDB->findEntry(_id) ;
//		ASSERT(ndx != INT_MAX);
//		_schedule =  m_mainDlg->GetInputTerminal()->procAssignDB->getDatabase(ndx) ;
//		if(_schedule->getCount() == 0)
//		{
//	        ProcessorID curID;
//			if(!m_mainDlg->FindDefinedRosterProcessor(ProcessNode,curID) )
//			{
//				ElapsedTime time ;
//				ProcessorRoster _ass ;
//				CMultiMobConstraint assign;	
//				assign.addConstraint(CMobileElemConstraint(m_mainDlg->GetInputTerminal())) ;
//				_ass.setAbsOpenTime(time) ;
//				CMobileElemConstraint constraint(m_mainDlg->GetInputTerminal());
//				_ass.setAssignment(assign);
//				time = time + (WholeDay-1);
//				_ass.setAbsCloseTime(time) ;
//				_ass.setAssRelation(Inclusive) ;
//				_ass.IsDaily(TRUE) ;
//				AddLineToGT(&_ass,_id) ;
//				return ;
//			}else
//			{
//				ndx =  m_mainDlg->GetInputTerminal()->procAssignDB->findEntry(curID) ;
//				ASSERT(ndx != INT_MAX);
//				if(ndx == INT_MAX)
//					return ;
//				_schedule = m_mainDlg->GetInputTerminal()->procAssignDB->getDatabase(ndx) ;
//			}
//		}
//	};
//	if(_schedule == NULL)
//		return ;
//	AddProcessor(_schedule,_id) ;
//	/*ProcAssignment* _ass = NULL;
//	for (int i =0 ; i < _schedule->getCount() ; i++)
//	{
//		_ass = _schedule->getItem(i) ;
//		if(_ass->getFlag() != 0)
//		 AddLineToGT(_ass,_id) ;
//	}*/
//}
void CDlgRosterGranttChart::LoadFlightList()
{

	m_listctrlFlightList.DeleteAllItems();
	
	int count = m_lstAssignFlights.getCount();

	for (int nFlight = 0; nFlight < count; nFlight++)
	{
		RosterAssignFlight  *pUnassignFlight = m_lstAssignFlights.getFlight(nFlight);
		if(pUnassignFlight->isSuccessAssign())
			continue;

		ARCFlight* pFlight = pUnassignFlight->getFlight();

		CString strPaxType;
		if (pUnassignFlight->getPaxType())
		{
			const CMobileElemConstraint *pMobCons = pUnassignFlight->getPaxType()->getConstraint(0);
			if(pMobCons)
				pMobCons->screenPrint(strPaxType);
		}
		int nRowIndex = m_listctrlFlightList.InsertItem( m_listctrlFlightList.GetItemCount(), strPaxType, 0 );
		m_listctrlFlightList.SetItemData(nRowIndex, (DWORD_PTR)pUnassignFlight);


		if(pUnassignFlight->isArrival())
		{
			m_listctrlFlightList.SetItemText( nRowIndex, 1, _T("Arrival"));		
		}
		else
		{
			m_listctrlFlightList.SetItemText( nRowIndex, 1, _T("Departure"));	
		}

		int nColStartIndex = 2;

		char str[128];
		pFlight->getAirline( str );
		m_listctrlFlightList.SetItemText( nRowIndex, 2, str );
		if( pFlight->isArriving())
		{
			pFlight->getArrID( str );
			m_listctrlFlightList.SetItemText( nRowIndex,nColStartIndex + 1, str );
			pFlight->getOrigin( str );
			m_listctrlFlightList.SetItemText( nRowIndex,nColStartIndex + 2, str );
			if (pFlight->getFlightType() == TerminalFlight)
			{
				((Flight*)pFlight)->getArrStopover( str );
				m_listctrlFlightList.SetItemText( nRowIndex,nColStartIndex + 3, str );
			}

			ElapsedTime estArrTime = pFlight->getArrTime();
			COleDateTime startTime;
			startTime.SetTime(estArrTime.GetHour(), estArrTime.GetMinute(), estArrTime.GetSecond());
			CString strTime = startTime.Format("%H:%M");
			CString strTemp;
			strTemp.Format(_T("Day%d %s"), estArrTime.GetDay(), strTime);
			strcpy(str,strTemp);
			m_listctrlFlightList.SetItemText( nRowIndex,nColStartIndex + 4, str );
		}
		else
		{
			m_listctrlFlightList.SetItemText( nRowIndex,nColStartIndex + 1, " " );
			m_listctrlFlightList.SetItemText( nRowIndex,nColStartIndex + 2, " " );
			m_listctrlFlightList.SetItemText( nRowIndex,nColStartIndex + 3, " " );
			m_listctrlFlightList.SetItemText( nRowIndex,nColStartIndex + 4, " " );
		}

		if( pFlight->isDeparting() )
		{
			pFlight->getDepID( str );
			m_listctrlFlightList.SetItemText( nRowIndex,nColStartIndex + 5, str );
			if (pFlight->getFlightType() == TerminalFlight)
			{
				((Flight*)pFlight)->getDepStopover( str );
				m_listctrlFlightList.SetItemText( nRowIndex,nColStartIndex + 6, str );
			}
			pFlight->getDestination( str );
			m_listctrlFlightList.SetItemText( nRowIndex,nColStartIndex + 7, str );

			ElapsedTime estDepTime =pFlight->getDepTime();
			COleDateTime depTime;
			depTime.SetTime(estDepTime.GetHour(), estDepTime.GetMinute(), estDepTime.GetSecond());
			CString strTime = depTime.Format("%H:%M");
			CString strTemp;
			strTemp.Format(_T("Day%d %s"), estDepTime.GetDay(), strTime);
			strcpy(str,strTemp);

			m_listctrlFlightList.SetItemText( nRowIndex,nColStartIndex + 8, str );
		}
		else
		{
			m_listctrlFlightList.SetItemText( nRowIndex,nColStartIndex + 5, " " );
			m_listctrlFlightList.SetItemText( nRowIndex,nColStartIndex + 6, " " );
			m_listctrlFlightList.SetItemText( nRowIndex,nColStartIndex + 7, " " );
			m_listctrlFlightList.SetItemText( nRowIndex,nColStartIndex + 8, " " );
		}


		pFlight->getACType( str );
		m_listctrlFlightList.SetItemText( nRowIndex,nColStartIndex + 9, str );

		char mode = 'T';
		if (!pFlight->isDeparting())
			mode = 'A';
		if (!pFlight->isArriving())
			mode = 'D';

		sprintf( str, "%c", mode );
		m_listctrlFlightList.SetItemText( nRowIndex,nColStartIndex + 10, str );

		ElapsedTime time = pFlight->getDepTime() - pFlight->getArrTime();
		if (!pFlight->isDeparting()|| !pFlight->isArriving() )
			time = pFlight->getServiceTime();

		time.printTime( str, 0 );
		m_listctrlFlightList.SetItemText( nRowIndex,nColStartIndex + 11, str );

		CString strProcCount;
		strProcCount.Format(_T("%d"), pUnassignFlight->getProcAssignCount());
		m_listctrlFlightList.SetItemText(nRowIndex,nColStartIndex + 12, strProcCount);


		if( !pUnassignFlight->isFailedAssign() )
		{
			//m_listctrlFlightList.SetItemState( nRowIndex, LVIS_FOCUSED|LVIS_SELECTED, LVIS_FOCUSED|LVIS_SELECTED );
			m_listctrlFlightList.SetItemColor(nRowIndex,::GetSysColor(COLOR_WINDOWFRAME),RGB(235,235,235));
		}

	}

}
void CDlgRosterGranttChart::OnClickBtnAddItemGtchartctrl(BOOL bSuccess, long nLineIndex) 
{
	//// TODO: Add your control notification handler code here
	//if( !bSuccess )
	//{
	//	MessageBox( "If you want to assign flights to a gate,you must highlighted the gate!",NULL,MB_OK|MB_ICONINFORMATION );
	//	return;
	//}

	//m_pGateAssignmentMgr->SetAssignedFlightSelected( -1, -1, true);
	//std::vector<int> vGateIdx;
	//vGateIdx.clear();
	//int nFailedCount = m_pGateAssignmentMgr->AssignSelectedFlightToGate( nLineIndex,vGateIdx);
	//if( nFailedCount > 0)
	//{
	//	CString strInfo;
	//	if (nFailedCount == 1)
	//		strInfo = _T("A flight cannot be assigned to the gate!");
	//	else
	//		strInfo.Format("%d flights cannot be assigned to the gate!",nFailedCount);
	//	MessageBox( strInfo,NULL,MB_OK|MB_ICONINFORMATION );
	//}
	//for (int i = 0; i < (int)vGateIdx.size();i++)
	//{
	//	FreshGataData( vGateIdx[i]);
	//}

	//for (i = 0; i < (int)vGateIdx.size();i++)
	//{
	//	FreshLinkLineData(vGateIdx[i]);
	//}

	//if (m_btnCheck.GetCheck())
	//{
	//	m_gtcharCtrl.EnableLinkLine();
	//}
	//else
	//{
	//	m_gtcharCtrl.DisableLinkLine();
	//}

	//SetUnassignedFlight();

	//m_btnSave.EnableWindow(TRUE);
}



void CDlgRosterGranttChart::OnClickBtnDelItemGtchartctrl(BOOL bSuccess, long nIndexLine, long nIndexItem, LPCTSTR strCaption, LPCTSTR strAddition, long timeBegin, long timeDuration, long itemData) 
{
	// TODO: Add your control notification handler code here
	if( !bSuccess )
	{
		MessageBox( "If you want to delete a item,you must highlighted it!", NULL, MB_OK|MB_ICONINFORMATION );
		return;
	}

	OnDblClickItemGtchartctrl( nIndexLine, nIndexItem, strCaption, strAddition, timeBegin, timeDuration, itemData);
}

void CDlgRosterGranttChart::OnDblClickItemGtchartctrl(long nIndexLine, long nIndexItem, LPCTSTR strCaption, LPCTSTR strAddition, long timeBegin, long timeDuration, long itemData) 
{
	//// TODO: Add your control notification handler code here
	////note: the nIndexItem is the item's index of all items, not the selected index of gate line, and the itemData is the selected index of gate line
	ProcessorRoster* pRoster = (ProcessorRoster*)itemData;
	ASSERT(pRoster);
	//find flight to remove assign information
	RosterAssignFlight* pFlight = m_lstAssignFlights.GetRosterFlight(pRoster);
	if (pFlight == NULL)
		return;

	pFlight->DeleteRoster(pRoster);
	//delete roster from schedule
	ProcAssignEntry* pProcEntry = m_vProAssignEntries.at(nIndexLine);
	if(pProcEntry == NULL)
		return;

	ProcessorRosterSchedule *pRosterSchedule = pProcEntry->getSchedule();
	if(pRosterSchedule == NULL)
		return;

	pRosterSchedule->RemoveProcAssignment(pRoster);

	if (GetListCtrlIndex(pFlight) != -1)
	{
		pFlight->setProcAssinCount(pFlight->getProcAssignCount() + 1);
	}
	else
	{
		pFlight->setProcAssinCount(1);
	}
	
	//FreshProcessorRoster(nIndexLine);
	AddFlightIntoList(pFlight);
	m_btnSave.EnableWindow(TRUE);
}

void CDlgRosterGranttChart::OnCheckFlightOperationMoveGtchartctral(long nLineIndexFrom, long nLineIndexTo,long nItemIndex,long FAR* pnLineData)
{
	long lRosterData;
	m_gtcharCtrl.GetItemData( nItemIndex, &lRosterData );
	*pnLineData = 0;

	ProcessorRoster* pFromRoster = (ProcessorRoster*)lRosterData;
	if(nLineIndexFrom != nLineIndexTo)
	{
		ProcAssignEntry* pFromEntry = m_vProAssignEntries.at(nLineIndexFrom);
		if(pFromEntry == NULL)
			return;	

		ProcessorRosterSchedule *pFromSchedule = pFromEntry->getSchedule();
		if(pFromSchedule == NULL)
			return;

		*pnLineData = pFromSchedule->ifExist(*pFromRoster)?1:0;
	}
}

void CDlgRosterGranttChart::OnItemMovedGtchartctrl(long nLineIndexFrom, long nLineIndexTo, long nItemIndex, LPCTSTR strCaption, LPCTSTR strAddition, long timeBegin, long timeDuration) 
{
	//// TODO: Add your control notification handler code here
	long lRosterData;
	m_gtcharCtrl.GetItemData( nItemIndex, &lRosterData );

	if(nLineIndexFrom == nLineIndexTo)
		return;

	ProcessorRoster* pFromRoster = (ProcessorRoster*)lRosterData;

	//find flight to remove assign information
	RosterAssignFlight* pFlight = m_lstAssignFlights.GetRosterFlight(pFromRoster);	
	if (pFlight == NULL)
		return;

	pFlight->DeleteRoster(pFromRoster);
	//delete roster from schedule
	ProcAssignEntry* pFromEntry = m_vProAssignEntries.at(nLineIndexFrom);
	if(pFromEntry == NULL)
		return;

	ProcessorRosterSchedule *pFromSchedule = pFromEntry->getSchedule();
	if(pFromSchedule == NULL)
		return;

	int nFromIndex = pFromSchedule->findItem(pFromRoster);
	if (nItemIndex == INT_MAX)
		return;

	pFromSchedule->RemoveProcAssignment(pFromRoster);

	//generate roster and add to entry
	ProcAssignEntry* pToEntry = m_vProAssignEntries.at(nLineIndexTo);
	if(pFromEntry == NULL)
		return;

	ProcessorRosterSchedule *pToSchedule = pToEntry->getSchedule();
	if(pToEntry == NULL)
		return;

	ProcessorRoster* pToRoster = new ProcessorRoster(*pFromRoster);
	pToRoster->SetID(-1);
	pToSchedule->addItem(pToRoster);

	pFlight->AddRoster(pToRoster);
	pFlight->m_emAssignResult = RosterAssignFlight::emAss_Sucess;

	m_gtcharCtrl.SetItemData(nItemIndex,(long)pToRoster) ;
//	FreshProcessorRoster(nLineIndexFrom);
//	FreshProcessorRoster(nLineIndexTo);
	//if (pFlight->isFailedAssign())
	//{
	//	AddFlightIntoList(pFlight);
	//}
	
	m_btnSave.EnableWindow(TRUE);
}

void CDlgRosterGranttChart::OnItemResizedGtchartctrl(long nIndexLine, long nIndexItem, LPCTSTR strCaption, LPCTSTR strAddition, long timeBegin, long timeDuration) 
{
	//// TODO: Add your control notification handler code here
	//long lFlightData;
	//m_gtcharCtrl.GetItemData( nIndexItem, &lFlightData );

	//CAssignGate* pGate = m_pGateAssignmentMgr->GetGate( nIndexLine );

	//// set current item selected
	//m_pGateAssignmentMgr->SetAssignedFlightSelected(nIndexLine,lFlightData,true);

	//m_btnSave.EnableWindow(TRUE);
}
void CDlgRosterGranttChart::OnRClickGtchartctrl(long nItemIndex, long nLineIndex) 
{
	//if( m_pmenuSub )	//if have init sucess
	//{
	//	CheckGateInPopMenu( nLineIndex, nItemIndex, m_pmenuSub );

	//	POINT point;
	//	::GetCursorPos( &point );
	//	m_pmenuSub->TrackPopupMenu(TPM_LEFTALIGN,point.x,point.y,this);
	//}
	//else				// if init failed, then rebuild it
	//{
	//	CMenu mMenu;
	//	mMenu.LoadMenu(IDR_GATEASSIGN_POP);

	//	CMenu* pSubMenu = mMenu.GetSubMenu(0);
	//	CMenu* pArrMenu = pSubMenu->GetSubMenu(0);
	//	CMenu* pDepMenu = pSubMenu->GetSubMenu(1);
	//	pArrMenu->RemoveMenu(0,MF_BYPOSITION);
	//	pDepMenu->RemoveMenu(0,MF_BYPOSITION);

	//	if( !BuildPopMenu( pArrMenu, pDepMenu ) )
	//		return;

	//	long lItemData,lLineData;
	//	m_gtcharCtrl.GetItemData( nItemIndex, &lItemData );
	//	m_gtcharCtrl.GetLineData( nLineIndex, &lLineData );

	//	CFlightOperationForGateAssign DefineGateFlight =  m_pGateAssignmentMgr->GetGate( lLineData)->GetFlight( lItemData );

	//	if (DefineGateFlight.getFlight()->getFlightType() == TerminalFlight)
	//	{
	//		CheckGateInPopMenu( nLineIndex, nItemIndex, pSubMenu );

	//		POINT point;
	//		::GetCursorPos( &point );
	//		pSubMenu->TrackPopupMenu(TPM_LEFTALIGN,point.x,point.y,this);

	//		deleteAllNewMenu();
	//	}
	//}
}



// set tool tips
void CDlgRosterGranttChart::OnShowItemToolTipGtchartctrl(long nLineIndex, long nItemIndex, long timeCurrent) 
{
	// get line and item data
	long lLineData,lItemData;
	m_gtcharCtrl.GetLineData( nLineIndex, &lLineData );
	m_gtcharCtrl.GetItemData( nItemIndex, &lItemData );

	if( lLineData <0 || lItemData <0 )
		return;

	//// get flight
	int nProcCount =  m_vProAssignEntries.size();
	if (nItemIndex < 0 && nItemIndex >= nProcCount)
		return; 

	ProcAssignEntry* pProcEntry = m_vProAssignEntries.at(nLineIndex);
	if(pProcEntry == NULL)
		return;

	ProcessorRosterSchedule *pRosterSchedule = pProcEntry->getSchedule();
	if(pRosterSchedule == NULL)
		return;


	CString strToolTips;
	if(pRosterSchedule->getCount() > 0)
	{
		CString strHeaderText;
		strHeaderText.Format(_T("%30s %10s  %5s  %10s  %10s"),
			_T("Passenger Type             "),
			_T("Airline"),
			_T("ID         "),
			_T("Open       "),
			_T("Close      "));

		strToolTips += strHeaderText;
		strToolTips += _T("\r\n");
	}
	for (int i =0 ; i < pRosterSchedule->getCount() ; i++)
	{
		ProcessorRoster *pProcRoster = pRosterSchedule->getItem(i) ;
		if(pProcRoster->getFlag() != 0)
		{
			if(pProcRoster->getAbsOpenTime() < ElapsedTime(timeCurrent *60L) &&
				pProcRoster->getAbsCloseTime()> ElapsedTime(timeCurrent*60L) )
			{
				CString strConstraint;
				CMobileElemConstraint *pConstraint =pProcRoster->getAssignment()->getConstraint(0);
				pConstraint->screenPrint(strConstraint);

				//
				CString strAirline(pConstraint->getAirline().GetValue());
				CString strFlightID(pConstraint->getFlightID().GetValue());
				CString strFormatText;
				strFormatText.Format(_T("%20s  %5s  %5s  %10s  %10s"),
					strConstraint.Left(20),
					strAirline,
					strFlightID,
					pProcRoster->getAbsOpenTime().printTime(),
					pProcRoster->getAbsCloseTime().printTime());

				strToolTips += strFormatText;
				strToolTips += _T("\r\n");


			}
		}
	}
	m_gtcharCtrl.SetItemAdditonInfo( nItemIndex, strToolTips );


	//CAssignGate* pGate = m_pGateAssignmentMgr->GetGate( lLineData );
	//CFlightOperationForGateAssign& pFlight = pGate->GetFlight( lItemData );

	//// get flight string
	//char szBuf[256];
	//if (pFlight.getFlight() != NULL)
	//{
	//	if ((pFlight.getFlight())->getFlightType() == TerminalFlight)
	//	{
	//		//pFlight.printFlight( szToolTips, 0, GetInputTerminal()->inStrDict );
	//		((Flight*)pFlight.getFlight())->displayFlight( szBuf );

	//		CString strToolTips = szBuf;
	//		// add gate info to tooltips
	//		strToolTips += CString("\n  ArrGate: ");
	//		strToolTips += ((Flight*)pFlight.getFlight())->getArrGate().GetIDString();

	//		strToolTips += CString("\n  DepGate: ");
	//		strToolTips += ((Flight*)pFlight.getFlight())->getDepGate().GetIDString();
	//		CString strError;
	//		if(!pFlight.FlightAssignToStandValid(strError))
	//		{
	//			strToolTips += CString("\n");
	//			strToolTips += strError;
	//		}
	//		m_gtcharCtrl.SetItemAdditonInfo( nItemIndex, strToolTips );
	//	}
	//}
}
// drag and drop
void CDlgRosterGranttChart::OnBegindragListFlight(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

	// TODO: Add your control notification handler code here
	m_nDragIndex = pNMListView->iItem;
	m_pDragFlight = (RosterAssignFlight *)m_listctrlFlightList.GetItemData(m_nDragIndex);


	if(m_pDragImage)
		delete m_pDragImage;

	POINT pt;
	pt.x = 8;
	pt.y = 8;
	m_pDragImage = m_listctrlFlightList.CreateDragImage (m_nDragIndex, &pt);
	ASSERT (m_pDragImage);
	// changes the cursor to the drag image (DragMove() is still required in 
	// OnMouseMove())
	VERIFY (m_pDragImage->BeginDrag (0, CPoint (8, 8)));
	VERIFY (m_pDragImage->DragEnter (GetDesktopWindow (), pNMListView->ptAction));


	m_bDragging = true;
	SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
	SetCapture ();
	*pResult = 0;
}

void CDlgRosterGranttChart::OnMouseMove(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	if (m_bDragging)
	{
		CPoint pt (point);
		ClientToScreen (&pt);

		// move the drag image
		VERIFY (m_pDragImage->DragMove (pt));
		// unlock window updates
		//VERIFY (m_pDragImage->DragShowNolock (FALSE));
		m_pDragImage->DragShowNolock (FALSE);

		CWnd* pDropWnd = WindowFromPoint (pt);

		long lIdx = -1;
		BOOL bSuccess=m_gtcharCtrl.DoDropOver(&lIdx);
		if( bSuccess )
		{
			SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
		}
		else
		{
			SetCursor(AfxGetApp()->LoadStandardCursor(IDC_NO));
		}

		m_pDragImage->DragShowNolock (TRUE);
	}

	CDialog::OnMouseMove(nFlags, point);
}


void CDlgRosterGranttChart::OnLButtonUp(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	if (m_bDragging)
	{
		CPoint pt (point);
		ClientToScreen (&pt);
		CWnd* pDropWnd = WindowFromPoint (pt);
		long lIdx = -1;
		BOOL bSuccess=m_gtcharCtrl.DoDropOver(&lIdx);
		if( bSuccess )
		{

			int nProcCount =  m_vProAssignEntries.size();
			if (lIdx < 0 && lIdx >= nProcCount)
				return; 

			ProcAssignEntry* pProcEntry = m_vProAssignEntries.at(lIdx);
			if(pProcEntry == NULL)
				return;

			if(m_pDragFlight == NULL)
				return;

			ElapsedTime eOpenTime;
			ElapsedTime eCloseTime;

			ElapsedTime deltaOpenTime(2*60*60L);//2 hours
			ElapsedTime deltaCloseTime(30*60L);//30 mins
		
			CMobileElemConstraint fltType( m_pInTerm );
			if(m_pDragFlight->getPaxType())
				fltType = *((*m_pDragFlight).getPaxType()->getConstraint(0));
			
			int nHours = m_nMaxScheduleDay * 24;
			ElapsedTime eMaxTime = ElapsedTime(nHours*60*60L);

			if(m_pDragFlight->isArrival())
			{
				ElapsedTime eScheTime = m_pDragFlight->getFlight()->getArrTime();
				eOpenTime = eScheTime - deltaOpenTime;//2 hours before takeoff is open
				eCloseTime = eScheTime - deltaCloseTime;//closes 30 minis before takeoff
				fltType.MergeFlightConstraint(&( m_pDragFlight->getFlight()->getType('A')));				
			}
			else
			{
				ElapsedTime eScheTime = m_pDragFlight->getFlight()->getDepTime();

				//eOpenTime = eScheTime + deltaCloseTime;//30 mins after landing, open
				//eCloseTime = eScheTime + deltaOpenTime;//2 hours after landing close

				eOpenTime = min(eScheTime + deltaCloseTime,eMaxTime);
				eCloseTime = min(eScheTime + deltaOpenTime,eMaxTime);
				fltType.MergeFlightConstraint(&( m_pDragFlight->getFlight()->getType('D')));
			}
			
			//add the flight individually
	
			CMultiMobConstraint mobType;
			mobType.addConstraint(fltType);

			ProcessorRosterSchedule *pProcSched = pProcEntry->getSchedule();
			if(pProcSched == NULL)
				return;

			ProcessorRoster *pRoster = new ProcessorRoster;
			pRoster->setOpenTime(eOpenTime);
			pRoster->setCloseTime(eCloseTime);
			pRoster->setAbsOpenTime(eOpenTime);
			pRoster->setAbsCloseTime(eCloseTime);
			pRoster->setAssignment(mobType) ;
			pRoster->setFlag(1) ;
			pRoster->setAssRelation(Inclusive) ;
			pRoster->IsDaily(false);

			pProcSched->addItem(pRoster);


//			std::vector<int> vGateIdx;
//			vGateIdx.clear();
//			int nFailedCount = m_pGateAssignmentMgr->AssignSelectedFlightToGate( lIdx,vGateIdx);
//			if( nFailedCount > 0)
//			{
//				CString strInfo;
//				if (GetGateDefineType() == StandType)
//				{
//					DisplayDrageErrorMessage(lIdx,strInfo);
//// 					if (nFailedCount == 1)
//// 						strInfo = _T("A flight cannot be assigned to the gate!");
//// 					else
//// 						strInfo.Format("%d flights cannot be assigned to the gate!",nFailedCount);
//				}
//				else
//				{
//					if (nFailedCount == 1)
//						strInfo = _T("The flight cannot be assigned to this gate because it conflicts with an existing assignment!");
//					else
//						strInfo.Format("%d flights cannot be assigned to the gate because they conflict with existing assignments!",nFailedCount);
//				}
//				MessageBox( strInfo,NULL,MB_OK|MB_ICONINFORMATION );
//			}
			//for (int i = 0; i < (int)vGateIdx.size();i++)
			//{
			//	FreshGataData( vGateIdx[i]);
			//}

			//for (i = 0; i < (int)vGateIdx.size();i++)
			//{
			//	FreshLinkLineData(vGateIdx[i]);
			//}

			//SetUnassignedFlight();
			m_pDragFlight->AddRoster(pRoster);
			if(m_pDragFlight->getProcAssignCount() > 1)
			{
				CString strProcCount;
				m_pDragFlight->setProcAssinCount(m_pDragFlight->getProcAssignCount() -1);
				m_pDragFlight->m_emAssignResult = RosterAssignFlight::emAss_Sucess;
				strProcCount.Format(_T("%d"), m_pDragFlight->getProcAssignCount());
				m_listctrlFlightList.SetItemText(m_nDragIndex, 14, strProcCount);
				m_listctrlFlightList.SetItemColor(m_nDragIndex,::GetSysColor(COLOR_WINDOWFRAME),RGB(235,235,235));
				m_listctrlFlightList.Invalidate(FALSE);
			}
			else
			{
				m_pDragFlight->m_emAssignResult = RosterAssignFlight::emAss_Sucess;
				m_listctrlFlightList.DeleteItem(m_nDragIndex);
			}
			

			//refresh Gantt chart
			
			m_gtcharCtrl.ClearAllItemOfLine(lIdx);
			const ProcessorID *pProcID = pProcEntry->getID();
			if(pProcID != NULL)
			{
				int ndx =  m_pInTerm->procAssignDB->findEntry(*pProcID) ;
				ProcessorRosterSchedule* _schedule =  m_pInTerm->procAssignDB->getDatabase(ndx) ;

				for (int i =0 ; i < _schedule->getCount() ; i++)
				{
					ProcessorRoster *_ass = _schedule->getItem(i) ;
					if(_ass->getFlag() != 0)
					{
						m_gtcharCtrl.SetLineData( lIdx , (long)(*pProcID).getID());
						m_gtcharCtrl.SetLineModifyable( lIdx,true);
						AddItemToGT(lIdx,_ass) ;
					}
				}
			}
		}
		m_bDragging = false;
		ReleaseCapture();
		SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));

		// end dragging
		m_pDragImage->DragLeave (GetDesktopWindow ());
		m_pDragImage->EndDrag ();

		m_btnSave.EnableWindow(TRUE);
	//	m_gtcharCtrl.Invalidate(TRUE);
	
	}

	CDialog::OnLButtonUp(nFlags, point);
}

void CDlgRosterGranttChart::OnBnClickedButtonSummary()
{
	CRosterAssignSummaryDlg dlg(&m_vProcID, m_pInTerm);
	dlg.DoModal();
}

void CDlgRosterGranttChart::FreshProcessorRoster( int iLine )
{
	m_gtcharCtrl.ClearItemSeled(false);
	m_gtcharCtrl.ClearAllItemOfLine( iLine );
	ProcAssignEntry* pProcEntry = m_vProAssignEntries.at(iLine);
	if(pProcEntry == NULL)
		return;
	ProcessorRosterSchedule *pSchedule = pProcEntry->getSchedule();
	ProcessorID procID = *pProcEntry->getID();


	for (int i = 0; i < pSchedule->getCount(); i++)
	{
		ProcessorRoster* pRoster = pSchedule->getItem(i);
		if(pRoster->getFlag() != 0)
		{
			m_gtcharCtrl.SetLineData( iLine , (long)procID.getID() );
			m_gtcharCtrl.SetLineModifyable( iLine,true);
			AddItemToGT(iLine,pRoster) ;
		}
	}

}
void CDlgRosterGranttChart::OnBnClickedButtonAutoAssign()
{
	m_btnAutoAssign.EnableWindow(FALSE);
	int nItemNum = m_gtcharCtrl.GetItemNum();
	for (int nItem = 0; nItem < nItemNum; ++ nItem)
	{
		m_gtcharCtrl.ClearAllItemOfLine(nItem);
	}
	LoadGranttChart();
	LoadFlightList();
}
void CDlgRosterGranttChart::OnBnClickedButtonUnassignAll()
{
	m_btnAutoAssign.EnableWindow(TRUE);
	size_t nSize = m_vProAssignEntries.size();
	for (size_t nEntry = 0 ; nEntry < nSize ; nEntry++)
	{

		ProcAssignEntry *pEntry =m_vProAssignEntries.at(nEntry);
		if(pEntry && pEntry->getSchedule())
			pEntry->getSchedule()->ClearAllAssignment();


	}

	m_lstAssignFlights.ClearAssignRosters();
	int nItemNum = m_gtcharCtrl.GetItemNum();
	for (int nItem = 0; nItem < nItemNum; ++ nItem)
	{
		m_gtcharCtrl.ClearAllItemOfLine(nItem);
	}

	LoadFlightList();
}

void CDlgRosterGranttChart::AddFlightIntoList( RosterAssignFlight* pUnassignFlight )
{
	int idx = GetListCtrlIndex(pUnassignFlight);
	if (idx != -1)
	{
		CString strProcCount;
		strProcCount.Format(_T("%d"), pUnassignFlight->getProcAssignCount());
		m_listctrlFlightList.SetItemText(idx,14,strProcCount);
		if( !pUnassignFlight->isFailedAssign() )
		{
			m_listctrlFlightList.SetItemColor(idx,::GetSysColor(COLOR_WINDOWFRAME),RGB(235,235,235));
		}
		else
		{
			m_listctrlFlightList.SetItemColor(idx,::GetSysColor(COLOR_WINDOWFRAME),RGB(255,255,255));
		}
		m_listctrlFlightList.Invalidate(FALSE);
		return;
	}
	
	ARCFlight* pFlight = pUnassignFlight->getFlight();

	CString strPaxType;
	if (pUnassignFlight->getPaxType())
	{
		const CMobileElemConstraint *pMobCons = pUnassignFlight->getPaxType()->getConstraint(0);
		if(pMobCons)
			pMobCons->screenPrint(strPaxType);
	}
	int nRowIndex = m_listctrlFlightList.InsertItem( m_listctrlFlightList.GetItemCount(), strPaxType, 0 );
	m_listctrlFlightList.SetItemData(nRowIndex, (DWORD_PTR)pUnassignFlight);
	if(pUnassignFlight->isArrival())
	{
		m_listctrlFlightList.SetItemText( nRowIndex, 1, _T("Arrival"));		
	}
	else
	{
		m_listctrlFlightList.SetItemText( nRowIndex, 1, _T("Departure"));	
	}

	int nColStartIndex = 2;

	char str[128];
	pFlight->getAirline( str );
	m_listctrlFlightList.SetItemText( nRowIndex, 2, str );
	if( pFlight->isArriving())
	{
		pFlight->getArrID( str );
		m_listctrlFlightList.SetItemText( nRowIndex,nColStartIndex + 1, str );
		pFlight->getOrigin( str );
		m_listctrlFlightList.SetItemText( nRowIndex,nColStartIndex + 2, str );
		if (pFlight->getFlightType() == TerminalFlight)
		{
			((Flight*)pFlight)->getArrStopover( str );
			m_listctrlFlightList.SetItemText( nRowIndex,nColStartIndex + 3, str );
		}

		ElapsedTime estArrTime = pFlight->getArrTime();
		COleDateTime startTime;
		startTime.SetTime(estArrTime.GetHour(), estArrTime.GetMinute(), estArrTime.GetSecond());
		CString strTime = startTime.Format("%H:%M");
		CString strTemp;
		strTemp.Format(_T("Day%d %s"), estArrTime.GetDay(), strTime);
		strcpy(str,strTemp);
		m_listctrlFlightList.SetItemText( nRowIndex,nColStartIndex + 4, str );
	}
	else
	{
		m_listctrlFlightList.SetItemText( nRowIndex,nColStartIndex + 1, " " );
		m_listctrlFlightList.SetItemText( nRowIndex,nColStartIndex + 2, " " );
		m_listctrlFlightList.SetItemText( nRowIndex,nColStartIndex + 3, " " );
		m_listctrlFlightList.SetItemText( nRowIndex,nColStartIndex + 4, " " );
	}

	if( pFlight->isDeparting() )
	{
		pFlight->getDepID( str );
		m_listctrlFlightList.SetItemText( nRowIndex,nColStartIndex + 5, str );
		if (pFlight->getFlightType() == TerminalFlight)
		{
			((Flight*)pFlight)->getDepStopover( str );
			m_listctrlFlightList.SetItemText( nRowIndex,nColStartIndex + 6, str );
		}
		pFlight->getDestination( str );
		m_listctrlFlightList.SetItemText( nRowIndex,nColStartIndex + 7, str );

		ElapsedTime estDepTime =pFlight->getDepTime();
		COleDateTime depTime;
		depTime.SetTime(estDepTime.GetHour(), estDepTime.GetMinute(), estDepTime.GetSecond());
		CString strTime = depTime.Format("%H:%M");
		CString strTemp;
		strTemp.Format(_T("Day%d %s"), estDepTime.GetDay(), strTime);
		strcpy(str,strTemp);

		m_listctrlFlightList.SetItemText( nRowIndex,nColStartIndex + 8, str );
	}
	else
	{
		m_listctrlFlightList.SetItemText( nRowIndex,nColStartIndex + 5, " " );
		m_listctrlFlightList.SetItemText( nRowIndex,nColStartIndex + 6, " " );
		m_listctrlFlightList.SetItemText( nRowIndex,nColStartIndex + 7, " " );
		m_listctrlFlightList.SetItemText( nRowIndex,nColStartIndex + 8, " " );
	}


	pFlight->getACType( str );
	m_listctrlFlightList.SetItemText( nRowIndex,nColStartIndex + 9, str );

	char mode = 'T';
	if (!pFlight->isDeparting())
		mode = 'A';
	if (!pFlight->isArriving())
		mode = 'D';

	sprintf( str, "%c", mode );
	m_listctrlFlightList.SetItemText( nRowIndex,nColStartIndex + 10, str );

	ElapsedTime time = pFlight->getDepTime() - pFlight->getArrTime();
	if (!pFlight->isDeparting()|| !pFlight->isArriving() )
		time = pFlight->getServiceTime();

	time.printTime( str, 0 );
	m_listctrlFlightList.SetItemText( nRowIndex,nColStartIndex + 11, str );

	CString strProcCount;
	strProcCount.Format(_T("%d"), pUnassignFlight->getProcAssignCount());
	m_listctrlFlightList.SetItemText(nRowIndex,nColStartIndex + 12, strProcCount);


	m_listctrlFlightList.SetItemData(nRowIndex,(DWORD_PTR)pUnassignFlight);
	m_listctrlFlightList.EnsureVisible(nRowIndex,FALSE);

	if( !pUnassignFlight->getPaxType() )
	{
		m_listctrlFlightList.SetItemColor(nRowIndex,::GetSysColor(COLOR_WINDOWFRAME),RGB(235,235,235));
	}
}

void CDlgRosterGranttChart::OnBtnSave()
{
	if (m_pParentWnd)
	{
		CDlgAutoRosterRules* pParentDlg = (CDlgAutoRosterRules*)m_pParentWnd;
		pParentDlg->SaveTimeSettingToCurrentRule();
	}
	m_btnSave.EnableWindow(FALSE);
}

int CDlgRosterGranttChart::GetListCtrlIndex( RosterAssignFlight* pUnassignFlight )
{
	for (int i = 0; i < m_listctrlFlightList.GetItemCount(); i++)
	{
		RosterAssignFlight* pFlight = (RosterAssignFlight*)m_listctrlFlightList.GetItemData(i);
		if (pFlight == pUnassignFlight)
		{
			return i;
		}
	}
	return -1;
}



























