#include "stdafx.h" 
#include "termplan.h"
#include "GateAssignDlg.h"
#include "inputs\flight.h"
#include "engine\terminal.h"
#include "inputs\schedule.h"
#include "common\WinMsg.h"
#include "TermPlanDoc.h"
#include "inputs\paxflow.h"
#include "DlgGateAssignConstraints.h"
#include "../InputAirside/InputAirside.h"
#include ".\gateassigndlg.h"
#include "Inputs\FlightForGateAssign.h"
#include "Inputs\GateAssignmentMgr.h"
#include "../Database/ADODatabase.h"
#include "../Inputs/StandAssignmentMgr.h"

#define ID_GATEASSIGN_ARRIVAL_MENU	20000		// use to define arrival gate
#define ID_GATEASSIGN_DEPATURE_MENU	21000		// use to define departure gate

#define CHANGE_MENU_LIST_ITEMCOUNT_PER_COLUMN_MIN	16
#define CHANGE_MENU_LIST_ITEMCOUNT_PER_ROW_MIN		16
#define MAX_CLOUM_NUM								4

extern const CString c_setting_regsectionstring;
const CString c_gateass_sorting_entry = "Gate Assignment Sorting String";

/////////////////////////////////////////////////////////////////////////////
// CGateAssignDlg dialog


CGateAssignDlg::CGateAssignDlg(int nProjID,CAirportDatabase* pAirportDatabase, CWnd* pParent)
: CDialog(CGateAssignDlg::IDD, pParent)
{
	m_bDragging = false;
	m_pDragImage = NULL;
	m_nLineNum = 0;
	m_pmenuSub = NULL;
	m_nPrjID = nProjID;

}

CGateAssignDlg::~CGateAssignDlg()
{
	deleteAllNewMenu();
	if (m_pGateAssignmentMgr)
	{
		delete m_pGateAssignmentMgr;
		m_pGateAssignmentMgr = NULL;
	}
}

void CGateAssignDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGateAssignDlg)
	DDX_Control(pDX, IDC_BUTTON_UNASSIGNALL, m_btnUnassignAll);
	DDX_Control(pDX, IDC_BUTTON_SUMMARY, m_btnSummary);
	DDX_Control(pDX, IDC_BUTTON_PRIORITY_FLIGHT, m_btnFlightPriority);
	DDX_Control(pDX, IDC_BUTTON_PRIORITY_GATE, m_btnGatePriority);
	DDX_Control(pDX, IDC_BUTTON_SAVE, m_btnSave);
	DDX_Control(pDX, IDOK, m_btnOk);
	DDX_Control(pDX, IDCANCEL, m_btnCancel);
	DDX_Control(pDX, IDC_LIST_FLIGHT, m_listctrlFlightList);
	DDX_Control(pDX, IDC_GTCHARTCTRL, m_gtcharCtrl);
	DDX_Control(pDX, IDC_CHECK_LINKLINE,m_btnCheck);
	DDX_Control(pDX, IDC_PROGRESS_ASSIGNMENT, m_progAssignStatus);
	//}}AFX_DATA_MAP

}


BEGIN_MESSAGE_MAP(CGateAssignDlg, CDialog)
	//{{AFX_MSG_MAP(CGateAssignDlg)
	ON_NOTIFY(NM_CLICK, IDC_LIST_FLIGHT, OnClickListFlight)
	ON_BN_CLICKED(IDC_BUTTON_SAVE, OnButtonSave)
	ON_NOTIFY(LVN_BEGINDRAG, IDC_LIST_FLIGHT, OnBegindragListFlight)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_NOTIFY(LVN_COLUMNCLICK, IDC_LIST_FLIGHT, OnColumnclickListFlight)
	ON_WM_SIZE()
	ON_WM_GETMINMAXINFO()
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()

	//}}AFX_MSG_MAP
	ON_COMMAND_RANGE( ID_GATEASSIGN_ARRIVAL_MENU,ID_GATEASSIGN_ARRIVAL_MENU+999,OnDefineArrGate)
	ON_COMMAND_RANGE( ID_GATEASSIGN_DEPATURE_MENU,ID_GATEASSIGN_DEPATURE_MENU+999,OnDefineDepGate)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGateAssignDlg message handlers

void CGateAssignDlg::InitGTCtrl()
{
	m_gtcharCtrl.InitControl( (long) AfxGetThread() );
	m_gtcharCtrl.SetEnableOverlap( false );
	m_gtcharCtrl.SetMoveItemSnapLineable(true);
	m_gtcharCtrl.SetItemColorGradual(true);
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

	FlightSchedule* fs = GetInputTerminal()->flightSchedule;
	if (fs->getCount() < 1)
		return;

	//modify by adam 2007-05-29
	//if (!fs->GetStartDate().IsAbsoluteDate())
	//{
	//	m_gtcharCtrl.SetRulerMax(24);
	//}
	//else
	//{
	//	COleDateTimeSpan span = fs->GetEndDate().GetDate() - fs->GetStartDate().GetDate();
	//	int nHours =(int) span.GetTotalHours();
	//	nHours = nHours < 24 ? 24 : nHours+24;
	//	m_gtcharCtrl.SetRulerMax(nHours);
	//}

	int nDay =fs->GetFlightScheduleEndTime().GetDay();
	int nHours = nDay * 24;
	m_gtcharCtrl.SetRulerMax(nHours);
	//End modify by adam 2007-05-29

	/*
	FlightSchedule* fs = GetInputTerminal()->flightSchedule;
	ElapsedTime tEnd = fs->getItem(0)->getDepTime();
	for(int i = 1; i < fs->getCount(); i++)
	{
	ElapsedTime t = fs->getItem(i)->getDepTime();
	if(tEnd < t)
	tEnd = t;
	}
	ElapsedTime t = long(24*3600-1);
	tEnd += t;
	int nHours = tEnd.asHours() / 24 * 24;
	nHours = nHours < 24 ? 24 : nHours;
	m_gtcharCtrl.SetRulerMax(nHours);

	*/
}


void CGateAssignDlg::InitFlightListCtrl()
{
	//initial flight list
	DWORD dwStyle = m_listctrlFlightList.GetExtendedStyle();
	dwStyle |= LVS_EX_HEADERDRAGDROP|LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT;
	m_listctrlFlightList.SetExtendedStyle( dwStyle );
	char* columnLabel[] = {"Airline", 
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
		"Duration" };
	int DefaultColumnWidth[] = { 95, 80, 80, 100, 100, 80, 100, 80, 100, 85, 80, 85 };
	int nFormat[] = {	LVCFMT_CENTER, LVCFMT_CENTER, LVCFMT_CENTER, LVCFMT_CENTER, 
		LVCFMT_CENTER, LVCFMT_CENTER, LVCFMT_CENTER, LVCFMT_CENTER,LVCFMT_CENTER,LVCFMT_CENTER,
		LVCFMT_CENTER, LVCFMT_CENTER };

	EDataType type[] = { dtSTRING, dtSTRING, dtSTRING, dtDATETIME, dtSTRING, 
		dtSTRING, dtDATETIME, dtSTRING, dtSTRING, dtSTRING, dtSTRING, dtSTRING };

	// column
	m_ctlHeaderCtrl.SubclassWindow( m_listctrlFlightList.GetHeaderCtrl()->m_hWnd );
	for( int i=0; i<12; i++ )
	{
		m_listctrlFlightList.InsertColumn( i, columnLabel[i], nFormat[i], DefaultColumnWidth[i] );
		m_ctlHeaderCtrl.SetDataType( i, dtSTRING );
	}
	m_ctlHeaderCtrl.SetRegString( c_setting_regsectionstring, c_gateass_sorting_entry );

	m_imageList.Create(16, 16, ILC_MASK, 0, 4);
	m_imageList.Add( AfxGetApp()->LoadIcon( IDI_ICON_AIRPLANE ) );
	m_listctrlFlightList.SetImageList( &m_imageList, LVSIL_SMALL );

	return;
}

void CGateAssignDlg::ClearFlightInGtChart(CFlightOperationForGateAssign* flightOp)
{
	int nLineIdx = flightOp->GetGateIdx();
	if (nLineIdx < 0)
		return;

	int nItemIdx = m_pGateAssignmentMgr->m_vectGate[nLineIdx]->GetFlightIdx(*flightOp);

	m_gtcharCtrl.DelItem(nLineIdx,nItemIdx);
	m_gtcharCtrl.ResetItemDataOfLine(nLineIdx);
}

void CGateAssignDlg::SetUnassignedFlight()
{
	m_listctrlFlightList.DeleteAllItems();
	std::vector<FlightForAssignment*> vectFlight = m_pGateAssignmentMgr->GetUnassignedScheduleFlightList();
	int count = vectFlight.size();

	//delete by adam 2007-05-30
	//CStartDate flightDate = GetInputTerminal()->flightSchedule->GetStartDate();
	
	//BOOL bAbsolute = FALSE;
	//if (flightDate.IsAbsoluteDate())
	//{
	//	bAbsolute = TRUE;
	//}
	//End delete by adam 2007-05-30

	for (int i = 0; i < count; i++)
	{
		ARCFlight* pFlight = vectFlight[i]->getFlight();
		vectFlight[i]->ClearGateIdx();
		char str[128];

		pFlight->getAirline( str );
		m_listctrlFlightList.InsertItem( i, str, 0 );
		if( pFlight->isArriving())
		{
			pFlight->getArrID( str );
			m_listctrlFlightList.SetItemText( i, 1, str );
			pFlight->getOrigin( str );
			m_listctrlFlightList.SetItemText( i, 2, str );
			if (pFlight->getFlightType() == TerminalFlight)
			{
				((Flight*)pFlight)->getArrStopover( str );
				m_listctrlFlightList.SetItemText( i, 3, str );
			}
			//			(aFlight.getArrTime()).printTime( str, 0 );

			//Modify by adam 2007-05-30
			//if (bAbsolute) //absolute
			//{
			//	bool bAbs = false;
			//	int nDay = 0;
			//	COleDateTime startDate,startTime;
			//	flightDate.GetDateTime(aFlight.getArrTime(),bAbs,startDate,nDay,startTime);
			//	CString strDate = startDate.Format("%Y-%m-%d");
			//	CString strTime = startTime.Format("%H:%M");
			//	CString strTemp;
			//	strTemp.Format("%s %s",strDate,strTime);
			//	strcpy(str,strTemp);				
			//}
			//else // relative 
			//{
			//	(aFlight.getArrTime()).printTime( str, 0 );

			//	/*				bool bAbs = false;
			//	int nDay = 0;
			//	COleDateTime startDate,startTime;

			//	flightDate.GetDateTime(aFlight.getArrTime(),bAbs,startDate,nDay,startTime);
			//	CString strtime = startTime.Format("%H:%M");

			//	CString strTemp;
			//	strTemp.Format("%s",strtime);
			//	strcpy(str,strTemp);		
			//	*/			
			//}

			ElapsedTime estArrTime = pFlight->getArrTime();
			COleDateTime startTime;
			startTime.SetTime(estArrTime.GetHour(), estArrTime.GetMinute(), estArrTime.GetSecond());
			CString strTime = startTime.Format("%H:%M");
			CString strTemp;
			strTemp.Format(_T("Day%d %s"), estArrTime.GetDay(), strTime);
			strcpy(str,strTemp);
			//End Modify by adam 2007-05-30
			m_listctrlFlightList.SetItemText( i, 4, str );
		}
		else
		{
			m_listctrlFlightList.SetItemText( i, 1, " " );
			m_listctrlFlightList.SetItemText( i, 2, " " );
			m_listctrlFlightList.SetItemText( i, 3, " " );
			m_listctrlFlightList.SetItemText( i, 4, " " );
		}

		if( pFlight->isDeparting() )
		{
			pFlight->getDepID( str );
			m_listctrlFlightList.SetItemText( i, 5, str );
			if (pFlight->getFlightType() == TerminalFlight)
			{
				((Flight*)pFlight)->getDepStopover( str );
				m_listctrlFlightList.SetItemText( i, 6, str );
			}
			pFlight->getDestination( str );
			m_listctrlFlightList.SetItemText( i, 7, str );

			ElapsedTime estDepTime =pFlight->getDepTime();
			COleDateTime depTime;
			depTime.SetTime(estDepTime.GetHour(), estDepTime.GetMinute(), estDepTime.GetSecond());
			CString strTime = depTime.Format("%H:%M");
			CString strTemp;
			strTemp.Format(_T("Day%d %s"), estDepTime.GetDay(), strTime);
			strcpy(str,strTemp);
			//End Modify by adam 2007-05-30

			m_listctrlFlightList.SetItemText( i, 8, str );
		}
		else
		{
			m_listctrlFlightList.SetItemText( i, 5, " " );
			m_listctrlFlightList.SetItemText( i, 6, " " );
			m_listctrlFlightList.SetItemText( i, 7, " " );
			m_listctrlFlightList.SetItemText( i, 8, " " );
		}


		pFlight->getACType( str );
		m_listctrlFlightList.SetItemText( i, 9, str );
		m_listctrlFlightList.SetItemData( i, i );

		char mode = 'T';
		if (!pFlight->isDeparting())
			mode = 'A';
		if (!pFlight->isArriving())
			mode = 'D';
		
		sprintf( str, "%c", mode );
		m_listctrlFlightList.SetItemText( i, 10, str );

		ElapsedTime time = pFlight->getDepTime() - pFlight->getArrTime();
		if (!pFlight->isDeparting()|| !pFlight->isArriving() )
			time = pFlight->getServiceTime();

		time.printTime( str, 0 );
		m_listctrlFlightList.SetItemText( i, 11, str );

		if( vectFlight[i]->IsSelected() )
		{
			m_listctrlFlightList.SetItemState( i, LVIS_FOCUSED|LVIS_SELECTED, LVIS_FOCUSED|LVIS_SELECTED );
		}
	}


	// sort 
	m_ctlHeaderCtrl.SortColumn( 0, SINGLE_COLUMN_SORT );
	//m_ctlHeaderCtrl.SortColumn( 0, SINGLE_COLUMN_SORT );
}



InputTerminal* CGateAssignDlg::GetInputTerminal()
{
	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();	
	return (InputTerminal*)&pDoc->GetTerminal();
}

CString CGateAssignDlg::GetProjPath()
{
	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();	
	return pDoc->m_ProjInfo.path;
}


// drag and drop
void CGateAssignDlg::OnBegindragListFlight(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

	// TODO: Add your control notification handler code here
	m_nDragIndex = pNMListView->iItem;
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


	int nFlightCount = m_listctrlFlightList.GetItemCount();
	for( int i=0; i<nFlightCount; i++ )
	{
		int iItemData = m_listctrlFlightList.GetItemData( i );
		if( m_listctrlFlightList.GetItemState( i, LVIS_SELECTED ) == LVIS_SELECTED )
		{
			m_pGateAssignmentMgr->SetUnAssignedFlightSelected( iItemData, 1 );
		}
		else
		{
			m_pGateAssignmentMgr->SetUnAssignedFlightSelected( iItemData, 0 );
		}
	}

	m_bDragging = true;
	SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
	SetCapture ();
	*pResult = 0;
}

void CGateAssignDlg::OnMouseMove(UINT nFlags, CPoint point) 
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

bool CGateAssignDlg::DisplayDrageErrorMessage(long lIdx,CString& strError)
{
	std::vector<FlightForAssignment*> vFlightAssignmentList;
	StandAssignmentMgr* pStandAssignmentMgr = (StandAssignmentMgr*)m_pGateAssignmentMgr;
	pStandAssignmentMgr->GetUnAssignedFlightSelected(vFlightAssignmentList);
	for (unsigned i = 0; i < vFlightAssignmentList.size(); i++)
	{
		FlightForAssignment* pFlightAssignment = vFlightAssignmentList[i];
		if (!pStandAssignmentMgr->ProcessAssignFailedError(lIdx,pFlightAssignment,strError))
		{
			return false;
		}
	}
	return true;
}

void CGateAssignDlg::OnLButtonUp(UINT nFlags, CPoint point) 
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
			std::vector<int> vGateIdx;
			vGateIdx.clear();
			int nFailedCount = m_pGateAssignmentMgr->AssignSelectedFlightToGate( lIdx,vGateIdx);
			if( nFailedCount > 0)
			{
				CString strInfo;
				if (GetGateDefineType() == StandType)
				{
					DisplayDrageErrorMessage(lIdx,strInfo);
// 					if (nFailedCount == 1)
// 						strInfo = _T("A flight cannot be assigned to the gate!");
// 					else
// 						strInfo.Format("%d flights cannot be assigned to the gate!",nFailedCount);
				}
				else
				{
					if (nFailedCount == 1)
						strInfo = _T("The flight cannot be assigned to this gate because it conflicts with an existing assignment!");
					else
						strInfo.Format("%d flights cannot be assigned to the gate because they conflict with existing assignments!",nFailedCount);
				}
				MessageBox( strInfo,NULL,MB_OK|MB_ICONINFORMATION );
			}
			for (int i = 0; i < (int)vGateIdx.size();i++)
			{
				FreshGataData( vGateIdx[i]);
			}

			for (int i = 0; i < (int)vGateIdx.size();i++)
			{
				FreshLinkLineData(vGateIdx[i]);
			}

			if (m_btnCheck.GetCheck())
			{
				m_gtcharCtrl.EnableLinkLine();
			}
			else
			{
				m_gtcharCtrl.DisableLinkLine();
			}
			SetUnassignedFlight();
		}
		m_bDragging = false;
		ReleaseCapture();
		SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));

		// end dragging
		m_pDragImage->DragLeave (GetDesktopWindow ());
		m_pDragImage->EndDrag ();

		m_btnSave.EnableWindow(TRUE);
			//m_gtcharCtrl.Invalidate();
	}

	CDialog::OnLButtonUp(nFlags, point);
}

void CGateAssignDlg::OnClickListFlight(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	CPoint ptMsg = GetMessagePos();
	m_listctrlFlightList.ScreenToClient( &ptMsg );

	UINT uFlags;
	int nHitItem = m_listctrlFlightList.HitTest( ptMsg, &uFlags );
	int nFlightCount = m_listctrlFlightList.GetItemCount();
	for( int i=0; i< nFlightCount; i++ )
	{
		int iItemData = m_listctrlFlightList.GetItemData(i);
		if( m_listctrlFlightList.GetItemState( i, LVIS_SELECTED ) == LVIS_SELECTED )
		{

			m_pGateAssignmentMgr->SetUnAssignedFlightSelected( iItemData, 1 );
		}
		else
		{
			m_pGateAssignmentMgr->SetUnAssignedFlightSelected( iItemData, 0 );
		}
	}
	*pResult = 0;
}


void CGateAssignDlg::OnButtonSave() 
{
	// TODO: Add your control notification handler code here
	m_pGateAssignmentMgr->Save(GetInputTerminal(),GetProjPath());	
//	GetInputTerminal()->flightSchedule->loadDataSet( GetProjPath() );
	//Update Detail Gate Flow
	UpdateFlow();
	//////////////////////////////////////////////////////
	m_btnSave.EnableWindow(FALSE);
}

void CGateAssignDlg::OnCancel() 
{
	// TODO: Add extra cleanup here
	GetInputTerminal()->flightSchedule->loadDataSet( GetProjPath() );
	CDialog::OnCancel();
}

void CGateAssignDlg::OnOK() 
{
	// TODO: Add extra validation here
	if( !m_btnSave.IsWindowEnabled() )  // nonzero if the window was previously disabled
	{
		CDialog::OnOK();
		return;
	}

	AfxGetApp()->BeginWaitCursor();
	//m_gateAssignmentMgr.Save(GetInputTerminal(),GetProjPath());				
	OnButtonSave();
	AfxGetApp()->EndWaitCursor();

	CDialog::OnOK();
}

void CGateAssignDlg::OnColumnclickListFlight(NMHDR* pNMHDR, LRESULT* pResult) 
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



void CGateAssignDlg::OnSize(UINT nType, int cx, int cy) 
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

	m_btnUnassignAll.MoveWindow(cx-40-7*btnrc.Width(),cy-8-btnrc.Height(), btnrc.Width(),btnrc.Height());
	m_btnSummary.MoveWindow(cx-35-6*btnrc.Width(), cy-8-btnrc.Height(), btnrc.Width(),btnrc.Height());
	m_btnGatePriority.MoveWindow(cx-30-5*btnrc.Width(), cy-8-btnrc.Height(), btnrc.Width(),btnrc.Height());
	m_btnFlightPriority.MoveWindow(cx-25-4*btnrc.Width(), cy-8-btnrc.Height(), btnrc.Width(),btnrc.Height());
	m_btnSave.MoveWindow( cx-20-3*btnrc.Width(),cy-8-btnrc.Height(),btnrc.Width(),btnrc.Height() );
	m_btnOk.MoveWindow( cx-15-2*btnrc.Width(),cy-8-btnrc.Height(),btnrc.Width(),btnrc.Height() );
	m_btnCancel.MoveWindow( cx-10-btnrc.Width(),cy-8-btnrc.Height(),btnrc.Width(),btnrc.Height() );

	CRect rect( 0,0,cx,cy );
	InvalidateRect( rect );
}

void CGateAssignDlg::OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI) 
{
	// TODO: Add your message handler code here and/or call default
	lpMMI->ptMinTrackSize.x = 670;
	lpMMI->ptMinTrackSize.y = 450;
	CDialog::OnGetMinMaxInfo(lpMMI);
}

BOOL CGateAssignDlg::InitFlowConvetor()
{
	m_paxFlowConvertor.SetInputTerm( GetInputTerminal() );
	m_allFlightFlow.ClearAll();

	return TRUE;
}

void CGateAssignDlg::UpdateFlow()
{
	//refresh the m_allFlightFlow
	CWaitCursor m_waitCursor;
	m_allFlightFlow.ClearAll();
	if(!m_paxFlowConvertor.BuildSpecFlightFlowFromGateAssign( m_allFlightFlow ))
		return;
	//m_allFlightFlow.PrintAllStructure();

	if( m_allFlightFlow.IfAllFlowValid( true ))
	{
		m_paxFlowConvertor.ToOldFlightFlow( m_allFlightFlow );
		GetInputTerminal()->m_pSpecificFlightPaxFlowDB->saveDataSet( GetProjPath() ,true );
	}
}
/*********************************************************************************************/

int CGateAssignDlg::FreshGataData(int _nGateIndex)
{
	char szItem[256];
	CString strToolTips;
	long lBegineTime, lSeverTime;

	m_gtcharCtrl.ClearItemSeled(false);
	m_pGateAssignmentMgr->SetAssignedFlightSelected( -1, -1, true);
	m_gtcharCtrl.ClearAllItemOfLine( _nGateIndex );
	CAssignGate* pGate = m_pGateAssignmentMgr->GetGate( _nGateIndex );
	int nFlightCount = pGate->GetFlightCount();
	for( int i=0; i<nFlightCount; i++ )
	{
		CFlightOperationForGateAssign flight = pGate->GetFlight(i);

		flight.getFlight()->getFlightIDString(szItem);
// 		CAssignStand* pGateData = dynamic_cast<CAssignStand*>(pGate);
// 		if(pGateData)
// 		{
// 			flight.getFlight()->setStand(pGateData->GetStandID());
// 		}
		pGate->GetToolTips( i, strToolTips, GetInputTerminal()->inStrDict);

		lBegineTime = flight.GetStartTime().asMinutes();

		ElapsedTime tService = flight.GetEndTime() - flight.GetStartTime();
		lSeverTime = tService.asMinutes();

		// if can't add flight to a gate , push it to unassignFlight;
		long lIdx;
		COLORREF colorBegin;
		COLORREF colorEnd;
		if (flight.getOpType() == INTPAKRING_OP)
		{
			colorBegin = RGB(255,255,255);
			colorEnd = RGB(255,182,147);
		}
		else
		{
			if(flight.getFlight()->getFlightType() == ItinerantFlightType)
			{
				colorBegin = RGB(204,255,204);
				colorEnd = RGB(150,200,156);
			}
			else if (flight.getFlight()->getFlightType() == TerminalFlight)
			{
				colorBegin =RGB(255,255,255);; 
				colorEnd = RGB(132,163,195);
			}
		}

		if( m_gtcharCtrl.AddItem( &lIdx, _nGateIndex, szItem,NULL,lBegineTime,lSeverTime,true,colorBegin,colorEnd) != 0)	//can't add the item
			m_pGateAssignmentMgr->SetAssignedFlightSelected( _nGateIndex, i, false );
		else
			m_gtcharCtrl.SetItemData( lIdx,i);
	}

	m_pGateAssignmentMgr->RemoveSelectedFlight();
	m_gtcharCtrl.ClearItemSeled( false );
	return 1;
}

int CGateAssignDlg::FreshLinkLineData(int _nGateIndex)
{
	CAssignGate* pGate = m_pGateAssignmentMgr->GetGate( _nGateIndex );
	int nFlightCount = pGate->GetFlightCount();
	for( int i=0; i<nFlightCount; i++ )
	{
		CFlightOperationForGateAssign flight = pGate->GetFlight(i);
		if (flight.getOpType() == INTPAKRING_OP)
		{
			m_gtcharCtrl.SetItemLink(_nGateIndex,i,TRUE);
		}
		else
		{
			FlightForAssignment* pFlight = flight.GetAssignmentFlight();
			if (pFlight->GetFlightIntPart())
			{
				FlightOperationPostion Pos = m_pGateAssignmentMgr->GetFlightOperationPostion(*(pFlight->GetFlightIntPart()));
				if (Pos.mnItem != -1 && Pos.mnLine != -1)
				{
					if (pFlight->GetFlightIntPart()->GetStartTime() > flight.GetStartTime())
					{
						m_gtcharCtrl.SetPreItem(Pos.mnLine,Pos.mnItem,_nGateIndex, i);
					}
					else
					{
						m_gtcharCtrl.SetNextItem(Pos.mnLine,Pos.mnItem,_nGateIndex, i);
					}
					m_gtcharCtrl.SetLinkLineItem(_nGateIndex, i,Pos.mnLine,Pos.mnItem);
				}
			}
		}
	}
	return 1;
}

BEGIN_EVENTSINK_MAP(CGateAssignDlg, CDialog)
	//{{AFX_EVENTSINK_MAP(CGateAssignDlg)
	ON_EVENT(CGateAssignDlg, IDC_GTCHARTCTRL, 4 /* ClickBtnAddItem */, OnClickBtnAddItemGtchartctrl, VTS_BOOL VTS_I4)
	ON_EVENT(CGateAssignDlg, IDC_GTCHARTCTRL, 6 /* ClickBtnDelItem */, OnClickBtnDelItemGtchartctrl, VTS_BOOL VTS_I4 VTS_I4 VTS_BSTR VTS_BSTR VTS_I4 VTS_I4 VTS_I4)
	ON_EVENT(CGateAssignDlg, IDC_GTCHARTCTRL, 7 /* DblClickItem */, OnDblClickItemGtchartctrl, VTS_I4 VTS_I4 VTS_BSTR VTS_BSTR VTS_I4 VTS_I4 VTS_I4)
	ON_EVENT(CGateAssignDlg, IDC_GTCHARTCTRL, 9 /* ItemResized */, OnItemResizedGtchartctrl, VTS_I4 VTS_I4 VTS_BSTR VTS_BSTR VTS_I4 VTS_I4)
	ON_EVENT(CGateAssignDlg, IDC_GTCHARTCTRL, 10 /* ItemMoved */, OnItemMovedGtchartctrl, VTS_I4 VTS_I4 VTS_I4 VTS_BSTR VTS_BSTR VTS_I4 VTS_I4)
	ON_EVENT(CGateAssignDlg, IDC_GTCHARTCTRL, 16 /* ShowItemToolTip */, OnShowItemToolTipGtchartctrl, VTS_I4 VTS_I4 VTS_I4)
	ON_EVENT(CGateAssignDlg, IDC_GTCHARTCTRL, 17 /* RClick */, OnRClickGtchartctrl, VTS_I4 VTS_I4)
	ON_EVENT(CGateAssignDlg, IDC_GTCHARTCTRL, 18 /* ItemCheck */, OnCheckFlightOperationMoveGtchartctral, VTS_I4 VTS_I4 VTS_I4 VTS_PI4)
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()

void CGateAssignDlg::OnClickBtnAddItemGtchartctrl(BOOL bSuccess, long nLineIndex) 
{
	// TODO: Add your control notification handler code here
	if( !bSuccess )
	{
		MessageBox( "If you want to assign flights to a gate,you must highlighted the gate!",NULL,MB_OK|MB_ICONINFORMATION );
		return;
	}

	m_pGateAssignmentMgr->SetAssignedFlightSelected( -1, -1, true);
	std::vector<int> vGateIdx;
	vGateIdx.clear();
	int nFailedCount = m_pGateAssignmentMgr->AssignSelectedFlightToGate( nLineIndex,vGateIdx);
	if( nFailedCount > 0)
	{
		CString strInfo;
		if (nFailedCount == 1)
			strInfo = _T("A flight cannot be assigned to the gate!");
		else
			strInfo.Format("%d flights cannot be assigned to the gate!",nFailedCount);
		MessageBox( strInfo,NULL,MB_OK|MB_ICONINFORMATION );
	}
	for (int i = 0; i < (int)vGateIdx.size();i++)
	{
		FreshGataData( vGateIdx[i]);
	}
	
	for (int i = 0; i < (int)vGateIdx.size();i++)
	{
		FreshLinkLineData(vGateIdx[i]);
	}

	if (m_btnCheck.GetCheck())
	{
		m_gtcharCtrl.EnableLinkLine();
	}
	else
	{
		m_gtcharCtrl.DisableLinkLine();
	}

	SetUnassignedFlight();

	m_btnSave.EnableWindow(TRUE);
}



void CGateAssignDlg::OnClickBtnDelItemGtchartctrl(BOOL bSuccess, long nIndexLine, long nIndexItem, LPCTSTR strCaption, LPCTSTR strAddition, long timeBegin, long timeDuration, long itemData) 
{
	// TODO: Add your control notification handler code here
	if( !bSuccess )
	{
		MessageBox( "If you want to delete a item,you must highlighted it!", NULL, MB_OK|MB_ICONINFORMATION );
		return;
	}

	OnDblClickItemGtchartctrl( nIndexLine, nIndexItem, strCaption, strAddition, timeBegin, timeDuration, itemData);
}

void CGateAssignDlg::OnDblClickItemGtchartctrl(long nIndexLine, long nIndexItem, LPCTSTR strCaption, LPCTSTR strAddition, long timeBegin, long timeDuration, long itemData) 
{
	// TODO: Add your control notification handler code here
	//note: the nIndexItem is the item's index of all items, not the selected index of gate line, and the itemData is the selected index of gate line
	FlightForAssignment* pFlight = m_pGateAssignmentMgr->getSelectedFlight(nIndexLine,itemData);
	if (pFlight == NULL)
		return;

	//first delete the selected flight operation
	CFlightOperationForGateAssign _flight = m_pGateAssignmentMgr->m_vectGate[nIndexLine]->GetFlight(itemData);
 	m_pGateAssignmentMgr->m_vectGate[nIndexLine]->RemoveAssignedFlight(_flight);
 	m_gtcharCtrl.ResetItemDataOfLine(nIndexLine);

	//delete other flight operations of schedule flight

	CFlightOperationForGateAssign* flightOp = pFlight->GetFlightArrPart();
	if (flightOp)
	{
		int nLineIdx = flightOp->GetGateIdx();
		ASSERT(nLineIdx >=0);

		int nItemIdx = m_pGateAssignmentMgr->m_vectGate[nLineIdx]->GetFlightIdx(*flightOp);

		if (*flightOp != _flight)
		{
			m_gtcharCtrl.DelItem(nLineIdx,nItemIdx);
			m_pGateAssignmentMgr->m_vectGate[nLineIdx]->RemoveAssignedFlight(*flightOp);
		}

		m_gtcharCtrl.ResetItemDataOfLine(nLineIdx);

	}

	flightOp = pFlight->GetFlightDepPart();
	if (flightOp)
	{
		int nLineIdx = flightOp->GetGateIdx();
		ASSERT(nLineIdx >=0);

		int nItemIdx = m_pGateAssignmentMgr->m_vectGate[nLineIdx]->GetFlightIdx(*flightOp);

		if (*flightOp != _flight)
		{
			m_gtcharCtrl.DelItem(nLineIdx,nItemIdx);
			m_pGateAssignmentMgr->m_vectGate[nLineIdx]->RemoveAssignedFlight(*flightOp);
		}

		m_gtcharCtrl.ResetItemDataOfLine(nLineIdx);

	}

	if (pFlight->GetFlightIntPart())
	{
		CFlightOperationForGateAssign* flightOp =pFlight->GetFlightIntPart();
		int nLineIdx = flightOp->GetGateIdx();
		ASSERT(nLineIdx >=0);
		
		int nItemIdx = m_pGateAssignmentMgr->m_vectGate[nLineIdx]->GetFlightIdx(*flightOp);

		if (*flightOp != _flight)
		{
			m_gtcharCtrl.DelItem(nLineIdx,nItemIdx);
			m_pGateAssignmentMgr->m_vectGate[nLineIdx]->RemoveAssignedFlight(*flightOp);
		}

		m_gtcharCtrl.ResetItemDataOfLine(nLineIdx);
	}
	m_pGateAssignmentMgr->RemoveAssignedFlight(pFlight);

	SetUnassignedFlight();

	m_btnSave.EnableWindow(TRUE);
	
}

void CGateAssignDlg::OnCheckFlightOperationMoveGtchartctral(long nLineIndexFrom, long nLineIndexTo,long nItemIndex,long FAR* pnLineData)
{
	long lFlightData;
	m_gtcharCtrl.GetItemData( nItemIndex, &lFlightData );
	*pnLineData = 0;

	if(nLineIndexFrom != nLineIndexTo)
	{
		CAssignGate* pGate = m_pGateAssignmentMgr->GetGate( nLineIndexFrom );
		ASSERT( pGate );		

		CFlightOperationForGateAssign flight = pGate->GetFlight( lFlightData );
		flight.SetSelected(true);

		*pnLineData = m_pGateAssignmentMgr->IsFlightOperationFitInGate(nLineIndexTo,&flight)?1:0;
	}
}

void CGateAssignDlg::OnItemMovedGtchartctrl(long nLineIndexFrom, long nLineIndexTo, long nItemIndex, LPCTSTR strCaption, LPCTSTR strAddition, long timeBegin, long timeDuration) 
{
	// TODO: Add your control notification handler code here
	long lFlightData;
	m_gtcharCtrl.GetItemData( nItemIndex, &lFlightData );
	if(nLineIndexFrom != nLineIndexTo)
	{
		//////////////////////////////////////////////////////////////////////////
		//first remove from the from_line, and fresh_line
		CAssignGate* pGate = m_pGateAssignmentMgr->GetGate( nLineIndexFrom );
		assert( pGate );		// from gate

		CFlightOperationForGateAssign flight = pGate->GetFlight( lFlightData );

		// remove from flight the gate
		pGate->RemoveFlight( lFlightData );
		// fresh_line
		m_gtcharCtrl.ResetItemDataOfLine( nLineIndexFrom );

		//////////////////////////////////////////////////////////////////////////
		//then add the flight to the to_line, and fresg_line
		pGate = m_pGateAssignmentMgr->GetGate( nLineIndexTo );
		assert( pGate );

		FlightForAssignment* pFlight = flight.GetAssignmentFlight();
		if (flight.getOpType() == INTPAKRING_OP)
		{
			pFlight->GetFlightIntPart()->SetGateIdx(nLineIndexTo);
		}
		else
		{

			CFlightOperationForGateAssign* _pflight = pFlight->GetFlightArrPart();
			if (_pflight)
			{
				if (flight.getOpType() == _pflight->getOpType())
				{
					_pflight->SetGateIdx(nLineIndexTo);
				}
			}

			_pflight = pFlight->GetFlightDepPart();
			if (_pflight)
			{
				if (flight.getOpType() == _pflight->getOpType())
				{
					_pflight->SetGateIdx(nLineIndexTo);
				}
			}			
		}
		pGate->AddFlight( flight);
		m_gtcharCtrl.SetItemData( nItemIndex, pGate->GetFlightCount()-1 );
	}


	//	m_gtcharCtrl.ResetItemDataOfLine(nLineIndexFrom);
	//	FlightForAssignment* pFlight = flight.GetAssignmentFlight();
	//	m_pGateAssignmentMgr->m_vectGate[nLineIndexFrom]->RemoveAssignedFlight(flight);


	//	//////////////////////////////////////////////////////////////////////////
	//	//then add the flight to the to_line, and fresg_line

	//	pGate = m_pGateAssignmentMgr->GetGate( nLineIndexTo );
	//	assert( pGate );


	//	if (!pFlight->GetFlightIntPart() && !m_pGateAssignmentMgr->isNeedtoTowOff(pFlight->getFlight(),nLineIndexTo))
	//	{
	//		for (int k = 0; k < (int)pFlight->GetFlightOperations().size();k++)
	//		{
	//			CFlightOperationForGateAssign* _pflight = pFlight->GetFlightOperations().at(k);
	//			_pflight->SetGateIdx(nLineIndexTo);
	//			m_pGateAssignmentMgr->m_vectGate[nLineIndexTo]->AddFlight( flight);
	//			m_gtcharCtrl.SetItemData( nItemIndex, pGate->GetFlightCount()-1 );
	//			m_gtcharCtrl.ResetItemDataOfLine(nLineIndexFrom);
	//		}
	//	}
	//	else
	//	{
	//		std::vector<CFlightOperationForGateAssign*> vFlights = pFlight->GetFlightOperations();
	//		for (int i = 0; i < (int)vFlights.size(); i++)
	//		{
	//			CFlightOperationForGateAssign* flightOp = vFlights[i];
	//			int nLineIdx = flightOp->GetGateIdx();
	//			ASSERT(nLineIdx >=0);

	//			int nItemIdx = m_pGateAssignmentMgr->m_vectGate[nLineIdx]->GetFlightIdx(*flightOp);

	//			if (*flightOp != flight)
	//			{
	//				m_pGateAssignmentMgr->m_vectGate[nLineIdx]->RemoveAssignedFlight(*flightOp);
	//				m_gtcharCtrl.DelItem(nLineIdx,nItemIdx);
	//			}

	//			m_gtcharCtrl.ResetItemDataOfLine(nLineIdx);

	//		}
	//		if (pFlight->GetFlightIntPart())
	//		{
	//			CFlightOperationForGateAssign* flightOp =pFlight->GetFlightIntPart();
	//			int nLineIdx = flightOp->GetGateIdx();
	//			ASSERT(nLineIdx >=0);
	//	
	//			int nItemIdx = m_pGateAssignmentMgr->m_vectGate[nLineIdx]->GetFlightIdx(*flightOp);
	//			if (*flightOp != flight)
	//			{
	//				m_pGateAssignmentMgr->m_vectGate[nLineIdx]->RemoveAssignedFlight(*flightOp);
	//				m_gtcharCtrl.DelItem(nLineIdx,nItemIdx);
	//			}
	//	
	//			m_gtcharCtrl.ResetItemDataOfLine(nLineIdx);
	//		}
	//		
	//		m_gtcharCtrl.DelItem(nLineIndexTo,m_gtcharCtrl.GetItemNumOfLine(nLineIndexTo) - 1);
	//		m_gtcharCtrl.ResetItemDataOfLine(nLineIndexTo);
	//		m_pGateAssignmentMgr->RemoveAssignedFlight(pFlight);
	//		pFlight->ClearGateIdx();
	//		SetUnassignedFlight();
	//	}
	//}
	m_btnSave.EnableWindow(TRUE);
}

void CGateAssignDlg::OnItemResizedGtchartctrl(long nIndexLine, long nIndexItem, LPCTSTR strCaption, LPCTSTR strAddition, long timeBegin, long timeDuration) 
{
	// TODO: Add your control notification handler code here
	long lFlightData;
	m_gtcharCtrl.GetItemData( nIndexItem, &lFlightData );

	CAssignGate* pGate = m_pGateAssignmentMgr->GetGate( nIndexLine );

	// set current item selected
	m_pGateAssignmentMgr->SetAssignedFlightSelected(nIndexLine,lFlightData,true);

	m_btnSave.EnableWindow(TRUE);
}


//////////////////////////////////////////////////////////////////////////
BOOL CGateAssignDlg::OnEraseBkgnd(CDC* pDC) 
{
	// TODO: Add your message handler code here and/or call default
	return TRUE;
}

void CGateAssignDlg::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

	HWND hwndT=::GetWindow(m_hWnd, GW_CHILD);
	CRect rectCW;
	CRgn rgnCW;
	rgnCW.CreateRectRgn(0,0,0,0);
	while (hwndT != NULL)
	{
		CWnd* pWnd=CWnd::FromHandle(hwndT)  ;
		if(
			pWnd->IsKindOf(RUNTIME_CLASS(CListCtrl))||
			pWnd->IsKindOf(RUNTIME_CLASS(CGTChart)))
		{
			pWnd->GetWindowRect(rectCW);
			ScreenToClient(rectCW);
			CRgn rgnTemp;
			rgnTemp.CreateRectRgnIndirect(rectCW);
			rgnCW.CombineRgn(&rgnCW,&rgnTemp,RGN_OR);
		}
		hwndT=::GetWindow(hwndT,GW_HWNDNEXT);

	}
	CRect rect;
	GetClientRect(&rect);
	CRgn rgn;
	rgn.CreateRectRgnIndirect(rect);
	CRgn rgnDraw;
	rgnDraw.CreateRectRgn(0,0,0,0);
	rgnDraw.CombineRgn(&rgn,&rgnCW,RGN_DIFF);
	CBrush br(GetSysColor(COLOR_BTNFACE));
	dc.FillRgn(&rgnDraw,&br);
	CRect rectRight=rect;
	rectRight.left=rectRight.right-7;
	dc.FillRect(rectRight,&br);
	rectRight=rect;
	rectRight.top=rect.bottom-44;
	dc.FillRect(rectRight,&br);
}

//////////////////////////////////////////////////////////////////////////
int CGateAssignDlg::DefineArrDepGate(UINT nID, bool _bArrGate)
{
	long lItemIndex,lLineIndex;
	long lItemData,lLineData;
	//////////////////////////////////////////////////////////////////////////
	// get  procID  
	int indexInGateList;
	if( _bArrGate )	//def arr gate
		indexInGateList= nID - ID_GATEASSIGN_ARRIVAL_MENU;
	else			//def dep gate
		indexInGateList= nID - ID_GATEASSIGN_DEPATURE_MENU;

	CString strGate(_T(""));
	if (_bArrGate)
	{
		strGate = m_vectArrGateName[indexInGateList];
	}
	else
	{
		strGate = m_vectDepGateName[indexInGateList];
	}

	ProcessorID gate;
	gate.SetStrDict( GetInputTerminal()->inStrDict );
	gate.setID( strGate );

	//////////////////////////////////////////////////////////////////////////
	// get all select item index
	long lSelItemCount = m_gtcharCtrl.GetItemSeledNum();
	if( lSelItemCount <= 0)
		return 0;

	long* plSelItemIndex = new long[ lSelItemCount ];
	if( plSelItemIndex )
	{
		m_gtcharCtrl.GetCurSeledItem( plSelItemIndex );
		for( long l=0; l<lSelItemCount; l++ )
		{
			lItemIndex = plSelItemIndex[l];
			assert( lItemIndex>=0 );
			m_gtcharCtrl.GetLineIndexOfItem( lItemIndex, &lLineIndex );
			if( lItemIndex<0 || lLineIndex<0 )
				continue;

			m_gtcharCtrl.GetItemData( lItemIndex, &lItemData );
			m_gtcharCtrl.GetLineData( lLineIndex, &lLineData );
			CFlightOperationForGateAssign& flight= m_pGateAssignmentMgr->GetGate( lLineData)->GetFlight( lItemData );

			if( _bArrGate )		//def arr gate
				((Flight*)flight.getFlight())->setArrGate( gate );	
			else				//def dep gate
				((Flight*)flight.getFlight())->setDepGate( gate );
		}

		delete []plSelItemIndex;
		m_btnSave.EnableWindow( true );
		return lSelItemCount;
	}	

	return -1;
}

void CGateAssignDlg::OnDefineArrGate(UINT nID)
{
	DefineArrDepGate( nID, true );
}

void CGateAssignDlg::OnDefineDepGate(UINT nID)
{
	DefineArrDepGate( nID, false );
}

// set tool tips
void CGateAssignDlg::OnShowItemToolTipGtchartctrl(long nLineIndex, long nItemIndex, long timeCurrent) 
{
	// get line and item data
	long lLineData,lItemData;
	m_gtcharCtrl.GetLineData( nLineIndex, &lLineData );
	m_gtcharCtrl.GetItemData( nItemIndex, &lItemData );

	if( lLineData <0 || lItemData <0 )
		return;

	// get flight
	CAssignGate* pGate = m_pGateAssignmentMgr->GetGate( lLineData );
	CFlightOperationForGateAssign& pFlight = pGate->GetFlight( lItemData );

	// get flight string
	char szBuf[256];
	if (pFlight.getFlight() != NULL)
	{
		if ((pFlight.getFlight())->getFlightType() == TerminalFlight)
		{
			//pFlight.printFlight( szToolTips, 0, GetInputTerminal()->inStrDict );
			((Flight*)pFlight.getFlight())->displayFlight( szBuf );

			CString strToolTips = szBuf;
			// add gate info to tooltips
			strToolTips += CString("\n  ArrGate: ");
			strToolTips += ((Flight*)pFlight.getFlight())->getArrGate().GetIDString();

			strToolTips += CString("\n  DepGate: ");
			strToolTips += ((Flight*)pFlight.getFlight())->getDepGate().GetIDString();
			CString strError;
			if(!pFlight.FlightAssignToStandValid(strError))
			{
				strToolTips += CString("\n");
				strToolTips += strError;
			}
			m_gtcharCtrl.SetItemAdditonInfo( nItemIndex, strToolTips );
		}
	}
}

int CGateAssignDlg::GetAllGateProcessor()
{
	m_vectArrGateName.clear();
	m_vectDepGateName.clear();
	m_vectAllGateName.clear();

	ProcessorArray processArray;
	GetInputTerminal()->procList->getProcessorsOfType( GateProc, processArray );

	int nGateCount = processArray.getCount();
	for( int i = 0; i < nGateCount; i++ )
	{
		GateProcessor* pProc = (GateProcessor*)processArray[i];
		CString strGateName = pProc->getID()->GetIDString();
		if (pProc->getGateType() == ArrGate)
		{
			m_vectArrGateName.push_back(strGateName);
		}
		else if(pProc->getGateType() == DepGate)
		{
			m_vectDepGateName.push_back(strGateName);
		}
		m_vectAllGateName.push_back( strGateName );
	}

	std::sort(m_vectArrGateName.begin(),m_vectArrGateName.end());
	std::sort(m_vectDepGateName.begin(),m_vectDepGateName.end());
	std::sort(m_vectAllGateName.begin(),m_vectAllGateName.end());

	return m_vectAllGateName.size();
}


void CGateAssignDlg::OnRClickGtchartctrl(long nItemIndex, long nLineIndex) 
{
	if( m_pmenuSub )	//if have init sucess
	{
		CheckGateInPopMenu( nLineIndex, nItemIndex, m_pmenuSub );

		POINT point;
		::GetCursorPos( &point );
		m_pmenuSub->TrackPopupMenu(TPM_LEFTALIGN,point.x,point.y,this);
	}
	else				// if init failed, then rebuild it
	{
		CMenu mMenu;
		mMenu.LoadMenu(IDR_GATEASSIGN_POP);

		CMenu* pSubMenu = mMenu.GetSubMenu(0);
		CMenu* pArrMenu = pSubMenu->GetSubMenu(0);
		CMenu* pDepMenu = pSubMenu->GetSubMenu(1);
		pArrMenu->RemoveMenu(0,MF_BYPOSITION);
		pDepMenu->RemoveMenu(0,MF_BYPOSITION);

		if( !BuildPopMenu( pArrMenu, pDepMenu ) )
			return;

		long lItemData,lLineData;
		m_gtcharCtrl.GetItemData( nItemIndex, &lItemData );
		m_gtcharCtrl.GetLineData( nLineIndex, &lLineData );

		CFlightOperationForGateAssign DefineGateFlight =  m_pGateAssignmentMgr->GetGate( lLineData)->GetFlight( lItemData );

		if (DefineGateFlight.getFlight()->getFlightType() == TerminalFlight)
		{
			CheckGateInPopMenu( nLineIndex, nItemIndex, pSubMenu );

			POINT point;
			::GetCursorPos( &point );
			pSubMenu->TrackPopupMenu(TPM_LEFTALIGN,point.x,point.y,this);

			deleteAllNewMenu();
		}
	}
}

bool CGateAssignDlg::InitRClickMenu()
{
	m_menuRClick.LoadMenu(IDR_GATEASSIGN_POP);
	m_pmenuSub = m_menuRClick.GetSubMenu(0);
	assert( m_pmenuSub );
	CMenu* pArrMenu = m_pmenuSub->GetSubMenu(0);
	CMenu* pDepMenu = m_pmenuSub->GetSubMenu(1);
	pArrMenu->RemoveMenu(0,MF_BYPOSITION);
	pDepMenu->RemoveMenu(0,MF_BYPOSITION);

	if( !BuildPopMenu( pArrMenu, pDepMenu ) )
	{
		deleteAllNewMenu();
		m_pmenuSub = NULL;
		return false;
	}

	return true;
}


BOOL CGateAssignDlg::BuildPopMenu(CMenu *_pArrMenu, CMenu *_pDepMenu)
{
	int iAllGateNum = m_vectAllGateName.size();

	if( iAllGateNum > CHANGE_MENU_LIST_ITEMCOUNT_PER_COLUMN_MIN * MAX_CLOUM_NUM )
	{
		return BuildPopMenuByLevel( _pArrMenu, _pDepMenu );
	}
	else
	{
		return BuildPopMenuByNumber( _pArrMenu, _pDepMenu );
	}
}

//////////////////////////////////////////////////////////////////////////
// if the number of all gate more than MAX,build menu by level
BOOL CGateAssignDlg::BuildPopMenuByLevel(CMenu *_pArrMenu, CMenu *_pDepMenu)
{
	CString strTemp1, strTemp2, strTemp3, strTemp4;
	CString strID1, strID2, strID3, strID4;
	int iArrItemCount1,iArrItemCount2,iArrItemCount3,iArrItemCount4;
	int iDepItemCount1,iDepItemCount2,iDepItemCount3,iDepItemCount4;
	int iIndexInList = -1;
	int iMenuItemCount = -1;
	GateType emGateType = NoGateType;

	ProcessorID procID;
	procID.SetStrDict( GetInputTerminal()->inStrDict );

	StringList strDictLevelOne;
	GetInputTerminal()->procList->getAllGroupNames (strDictLevelOne, GateProc );
	//////////////////////////////////////////////////////////////////////////
	// level one
	iArrItemCount1 = 0;
	iDepItemCount1 = 0;
	for( int iFirstLevel = 0; iFirstLevel< strDictLevelOne.getCount(); iFirstLevel++ )
	{
		strTemp1 = strDictLevelOne.getString( iFirstLevel );
		strTemp1.TrimLeft();
		strTemp1.TrimRight();
		if( strTemp1.IsEmpty() )
			continue;

		strID1 = strTemp1;
		procID.setID((LPCSTR) strID1);

		StringList strDictLevelTwo;
		GetInputTerminal()->procList->getMemberNames( procID, strDictLevelTwo, GateProc);
		if( strDictLevelTwo.getCount()==0 )
		{	
			emGateType = IfIndexInArrGateList(strID1);
			iIndexInList = getIndexInAllGateList( strID1,emGateType);

			if (emGateType == ArrGate)
			{
				assert( iIndexInList != -1);
				iMenuItemCount = _pArrMenu->GetMenuItemCount();
				if( iMenuItemCount % CHANGE_MENU_LIST_ITEMCOUNT_PER_COLUMN_MIN==0 && iMenuItemCount>0 )
				{
					_pArrMenu->AppendMenu( MF_STRING|MF_ENABLED|MF_MENUBARBREAK, ID_GATEASSIGN_ARRIVAL_MENU+iIndexInList, strID1 );
				}
				else
				{
					_pArrMenu->AppendMenu( MF_STRING|MF_ENABLED, ID_GATEASSIGN_ARRIVAL_MENU+iIndexInList, strID1 );
				}
				iArrItemCount1++;
			}
			else if(emGateType == DepGate)
			{
				assert( iIndexInList != -1);
				iMenuItemCount = _pDepMenu->GetMenuItemCount();
				if( iMenuItemCount % CHANGE_MENU_LIST_ITEMCOUNT_PER_COLUMN_MIN==0 && iMenuItemCount>0 )
				{
					_pDepMenu->AppendMenu( MF_STRING|MF_ENABLED|MF_MENUBARBREAK, ID_GATEASSIGN_DEPATURE_MENU+iIndexInList, strID1 );
				}
				else
				{
					_pDepMenu->AppendMenu( MF_STRING|MF_ENABLED, ID_GATEASSIGN_DEPATURE_MENU+iIndexInList, strID1 );
				}
				iDepItemCount1++;
			}
		}
		else
		{
			CMenu* pLevelOneArrMenu = new CMenu;
			CMenu* pLevelOneDepMenu = new CMenu;
			pLevelOneArrMenu->CreateMenu();
			pLevelOneDepMenu->CreateMenu();
			m_vectAllNewMenu.push_back( pLevelOneArrMenu );
			m_vectAllNewMenu.push_back( pLevelOneDepMenu );		
			//////////////////////////////////////////////////////////////////////////
			// level two
			iArrItemCount2 = 0;
			iDepItemCount2 = 0;
			for( int iSecondLevel = 0; iSecondLevel< strDictLevelTwo.getCount(); iSecondLevel++)
			{
				strTemp2 = strDictLevelTwo.getString( iSecondLevel );
				strTemp2.TrimLeft();
				strTemp2.TrimRight();
				if( strTemp2.IsEmpty() )
					continue;

				strID2 = strID1;
				strID2 +="-";
				strID2 += strTemp2;
				procID.setID( strID2 );

				StringList strDictLevelThree;
				GetInputTerminal()->procList->getMemberNames( procID, strDictLevelThree, GateProc );

				if( strDictLevelThree.getCount() == 0 )
				{
					emGateType = IfIndexInArrGateList(strID2);
					iIndexInList = getIndexInAllGateList( strID2,emGateType);

					if (emGateType == ArrGate)
					{
						assert( iIndexInList!= -1);
						iMenuItemCount = pLevelOneArrMenu->GetMenuItemCount();
						if( iMenuItemCount % CHANGE_MENU_LIST_ITEMCOUNT_PER_COLUMN_MIN==0 && iMenuItemCount>0)
						{
							pLevelOneArrMenu->AppendMenu( MF_STRING|MF_ENABLED|MF_MENUBARBREAK, ID_GATEASSIGN_ARRIVAL_MENU+iIndexInList, strID2 );
						}
						else
						{
							pLevelOneArrMenu->AppendMenu( MF_STRING|MF_ENABLED, ID_GATEASSIGN_ARRIVAL_MENU+iIndexInList, strID2 );
						}
						iArrItemCount2++;
					}
					else if(emGateType == DepGate)
					{
						assert( iIndexInList!= -1);
						iMenuItemCount = pLevelOneDepMenu->GetMenuItemCount();
						if( iMenuItemCount % CHANGE_MENU_LIST_ITEMCOUNT_PER_COLUMN_MIN==0 && iMenuItemCount>0)
						{
							pLevelOneDepMenu->AppendMenu( MF_STRING|MF_ENABLED|MF_MENUBARBREAK, ID_GATEASSIGN_DEPATURE_MENU+iIndexInList, strID2 );
						}
						else
						{
							pLevelOneDepMenu->AppendMenu( MF_STRING|MF_ENABLED, ID_GATEASSIGN_DEPATURE_MENU+iIndexInList, strID2 );
						}
						iDepItemCount2++;
					}
				}
				else
				{
					CMenu* pLevelTwoArrMenu = new CMenu;
					CMenu* pLevelTwoDepMenu = new CMenu;
					pLevelTwoArrMenu->CreateMenu();
					pLevelTwoDepMenu->CreateMenu();
					m_vectAllNewMenu.push_back( pLevelTwoArrMenu );
					m_vectAllNewMenu.push_back( pLevelTwoDepMenu );
					//////////////////////////////////////////////////////////////////////////
					// level three
					iArrItemCount3 = 0;
					iDepItemCount3 = 0;
					for( int iThirdLevel = 0; iThirdLevel< strDictLevelThree.getCount(); iThirdLevel++ )
					{
						strTemp3 = strDictLevelThree.getString( iThirdLevel );
						strTemp3.TrimLeft();
						strTemp3.TrimRight();
						if( strTemp3.IsEmpty() )
							continue;

						strID3 = strID2;
						strID3 += "-";
						strID3 += strTemp3;
						procID.setID( strID3 );

						StringList strDictLevelFour;
						GetInputTerminal()->procList->getMemberNames( procID, strDictLevelFour, GateProc );

						if( strDictLevelFour.getCount() == 0)
						{
							emGateType = IfIndexInArrGateList(strID3);
							iIndexInList = getIndexInAllGateList(strID3,emGateType);

							if (emGateType == ArrGate)
							{
								assert( iIndexInList != -1 );
								iMenuItemCount = pLevelTwoArrMenu->GetMenuItemCount();
								if( iMenuItemCount % CHANGE_MENU_LIST_ITEMCOUNT_PER_COLUMN_MIN == 0 && iMenuItemCount>0)
								{
									pLevelTwoArrMenu->AppendMenu( MF_STRING|MF_ENABLED|MF_MENUBARBREAK, ID_GATEASSIGN_ARRIVAL_MENU + iIndexInList, strID3 );
								}
								else
								{
									pLevelTwoArrMenu->AppendMenu( MF_STRING|MF_ENABLED, ID_GATEASSIGN_ARRIVAL_MENU + iIndexInList, strID3 );
								}
								iArrItemCount3++;
							}
							else if(emGateType == DepGate)
							{
								assert( iIndexInList != -1 );
								iMenuItemCount = pLevelTwoDepMenu->GetMenuItemCount();
								if( iMenuItemCount % CHANGE_MENU_LIST_ITEMCOUNT_PER_COLUMN_MIN == 0 && iMenuItemCount>0)
								{
									pLevelTwoDepMenu->AppendMenu( MF_STRING|MF_ENABLED|MF_MENUBARBREAK, ID_GATEASSIGN_DEPATURE_MENU + iIndexInList, strID3 );
								}
								else
								{
									pLevelTwoDepMenu->AppendMenu( MF_STRING|MF_ENABLED, ID_GATEASSIGN_DEPATURE_MENU + iIndexInList, strID3 );
								}
								iDepItemCount3++;
							}

						}
						else
						{
							CMenu* pLevelThreeArrMenu = new CMenu;
							CMenu* pLevelThreeDepMenu = new CMenu;
							pLevelThreeArrMenu->CreateMenu();
							pLevelThreeDepMenu->CreateMenu();
							m_vectAllNewMenu.push_back( pLevelThreeArrMenu );
							m_vectAllNewMenu.push_back( pLevelThreeDepMenu );
							//////////////////////////////////////////////////////////////////////////
							// level four
							iArrItemCount4 = 0;
							iDepItemCount4 = 0;
							for( int iFourLevel =0; iFourLevel< strDictLevelFour.getCount(); iFourLevel++ )
							{
								strTemp4 = strDictLevelFour.getString( iFourLevel );
								strTemp4.TrimLeft();
								strTemp4.TrimRight();
								if( strTemp4.IsEmpty() )
									continue;

								strID4 = strID3;
								strID4 += "-";
								strID4 += strTemp4;

								emGateType = IfIndexInArrGateList(strID4);
								iIndexInList = getIndexInAllGateList(strID4,emGateType);

								if (emGateType == ArrGate)
								{
									assert( iIndexInList!= -1 );
									iMenuItemCount = pLevelThreeArrMenu->GetMenuItemCount();
									if( iMenuItemCount % CHANGE_MENU_LIST_ITEMCOUNT_PER_COLUMN_MIN ==0 && iMenuItemCount>0)
									{
										pLevelThreeArrMenu->AppendMenu( MF_STRING|MF_ENABLED|MF_MENUBARBREAK, ID_GATEASSIGN_ARRIVAL_MENU + iIndexInList, strID4 );
									}
									else
									{
										pLevelThreeArrMenu->AppendMenu( MF_STRING|MF_ENABLED, ID_GATEASSIGN_ARRIVAL_MENU + iIndexInList, strID4 );
									}
									iArrItemCount4++;
								}
								else if(emGateType == DepGate)
								{
									assert( iIndexInList!= -1 );
									iMenuItemCount = pLevelThreeDepMenu->GetMenuItemCount();
									if( iMenuItemCount % CHANGE_MENU_LIST_ITEMCOUNT_PER_COLUMN_MIN ==0 && iMenuItemCount>0)
									{
										pLevelThreeDepMenu->AppendMenu( MF_STRING|MF_ENABLED|MF_MENUBARBREAK, ID_GATEASSIGN_DEPATURE_MENU + iIndexInList, strID4 );
									}
									else
									{
										pLevelThreeDepMenu->AppendMenu( MF_STRING|MF_ENABLED, ID_GATEASSIGN_DEPATURE_MENU + iIndexInList, strID4 );
									}
									iDepItemCount4++;
								}

							}// end level four
							if( iArrItemCount4 >0 )
							{
								pLevelTwoArrMenu->InsertMenu(0, MF_BYPOSITION|MF_POPUP, (UINT) pLevelThreeArrMenu->m_hMenu, strID3 );
								iArrItemCount3++;
							}
							if (iDepItemCount4 >0)
							{
								pLevelTwoDepMenu->InsertMenu(0, MF_BYPOSITION|MF_POPUP, (UINT) pLevelThreeDepMenu->m_hMenu, strID3 );
								iDepItemCount3++;
							}
						}
					}//end level three
					if( iArrItemCount3 >0 )
					{
						pLevelOneArrMenu->InsertMenu(0,MF_BYPOSITION|MF_POPUP, (UINT) pLevelTwoArrMenu->m_hMenu, strID2 );
						iArrItemCount2++;
					}
					if (iDepItemCount3 > 0)
					{
						pLevelOneDepMenu->InsertMenu(0,MF_BYPOSITION|MF_POPUP, (UINT) pLevelTwoDepMenu->m_hMenu, strID2 );
						iDepItemCount2++;
					}
				}
			}// end level two
			if( iArrItemCount2 >0 )
			{
				_pArrMenu->InsertMenu(0, MF_BYPOSITION|MF_POPUP, (UINT) pLevelOneArrMenu->m_hMenu, strID1 );
				iArrItemCount1++;
			}
			if (iDepItemCount2 > 0)
			{
				_pDepMenu->InsertMenu(0, MF_BYPOSITION|MF_POPUP, (UINT) pLevelOneDepMenu->m_hMenu, strID1 );
				iDepItemCount1++;
			}
		}
	}

	return (BOOL)(iArrItemCount1 && iDepItemCount1);		
}

//////////////////////////////////////////////////////////////////////////
// if the number of all gate less than MAX,build menu by number
BOOL CGateAssignDlg::BuildPopMenuByNumber(CMenu *_pArrMneu, CMenu *_pDepMenu)
{
	//////////////////////////////////////////////////////////////////////////
	// insert "gate" to arrMenu and depMenu
	int nArrGateCount = m_vectArrGateName.size();
	for (int i = 0; i < nArrGateCount; i++)
	{
		CString strGateName = m_vectArrGateName[i];

		if( i%CHANGE_MENU_LIST_ITEMCOUNT_PER_COLUMN_MIN == 0 && i>0 )
		{
			_pArrMneu->AppendMenu(MF_STRING|MF_ENABLED|MF_MENUBARBREAK,
				ID_GATEASSIGN_ARRIVAL_MENU+i,strGateName);
		}
		else
		{
	
			_pArrMneu->AppendMenu(MF_STRING|MF_ENABLED,
				ID_GATEASSIGN_ARRIVAL_MENU+i,strGateName);
		}
	}

	int nDepGateCount = m_vectDepGateName.size();
	for (int i = 0; i < nDepGateCount; i++)
	{
		CString strGateName = m_vectDepGateName[i];

		if( i%CHANGE_MENU_LIST_ITEMCOUNT_PER_COLUMN_MIN == 0 && i>0 )
		{
			_pDepMenu->AppendMenu(MF_STRING|MF_ENABLED|MF_MENUBARBREAK,
				ID_GATEASSIGN_DEPATURE_MENU+i,strGateName);

		}
		else
		{
			_pDepMenu->AppendMenu(MF_STRING|MF_ENABLED,
				ID_GATEASSIGN_DEPATURE_MENU+i,strGateName);
		}	
	}
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
// delete all temp menu
bool CGateAssignDlg::deleteAllNewMenu()
{
	for( int i=0; i < static_cast<int>(m_vectAllNewMenu.size()); i++ )
	{
		if( m_vectAllNewMenu[i] )
			delete m_vectAllNewMenu[i];
	}

	m_vectAllNewMenu.clear();
	return true;
}

int CGateAssignDlg::getIndexInAllGateList(const CString &_strGate,GateType emGateType)
{
	switch(emGateType)
	{
	case ArrGate:
		{
			for (int i = 0; i < static_cast<int>(m_vectArrGateName.size()); i++)
			{
				if (_strGate == m_vectArrGateName[i])
				{
					return i;
				}
			}
		}
		break;
	case DepGate:
		{
			for (int i = 0; i < static_cast<int>(m_vectDepGateName.size()); i++)
			{
				if (_strGate == m_vectDepGateName[i])
				{
					return i;
				}
			}
		}
		break;
	default:
	    break;
	}
	
	return -1;
}

GateType CGateAssignDlg::IfIndexInArrGateList(const CString& _strGate)
{
	for (int i = 0; i < static_cast<int>(m_vectArrGateName.size()); i++)
	{
		if (_strGate == m_vectArrGateName[i])
		{
			return ArrGate;
		}
	}

	for (int i = 0; i < static_cast<int>(m_vectDepGateName.size()); i++)
	{
		if (_strGate == m_vectDepGateName[i])
		{
			return DepGate;
		}
	}
	return NoGateType;
}

int CGateAssignDlg::GetIndexInGateList( const CString& _strGate )
{
	for( int i=0; i< m_pGateAssignmentMgr->GetGateCount(); i++ )
	{
		CString strGateName;
		CAssignGate* pGate = m_pGateAssignmentMgr->GetGate(i);
		//pGate->GetStandName(strGateName);

		if( strGateName == _strGate )
			return i;
	}

	return -1;
}

// ParkingStandBufferList may be changed
void CGateAssignDlg::UpdateRelativeData()
{

}

void CGateAssignDlg::CheckGateInPopMenu( long lLine, long lItem, CMenu* _pPopMenu )
{
	bool bClearAllCheck = false;
	CString strarrGate, strdepGate;
	int iSelItemCount = m_gtcharCtrl.GetItemSeledNum();
	if( iSelItemCount != 1)
	{
		bClearAllCheck = true;
	}
	else
	{
		bClearAllCheck = false;
		long lItemData,lLineData;
		m_gtcharCtrl.GetItemData( lItem, &lItemData );
		m_gtcharCtrl.GetLineData( lLine, &lLineData );
		CFlightOperationForGateAssign DefineGateFlight =  m_pGateAssignmentMgr->GetGate( lLineData)->GetFlight( lItemData );
		ProcessorID arrgate = ((Flight*)DefineGateFlight.getFlight())->getArrGate();
		strarrGate  = arrgate.GetIDString();
		ProcessorID depgate = ((Flight*)DefineGateFlight.getFlight())->getDepGate();
		strdepGate  = depgate.GetIDString();
	}	

	CMenu* pArrMenu = _pPopMenu->GetSubMenu(0);
	CMenu* pDepMenu = _pPopMenu->GetSubMenu(1);

	CheckMenuItem( pArrMenu, strarrGate, bClearAllCheck );
	CheckMenuItem( pDepMenu, strdepGate, bClearAllCheck );
}

void CGateAssignDlg::CheckMenuItem( CMenu* _pSubMenu, const CString& _strGate, bool _bClearAllCheck )
{
	if( _pSubMenu== NULL )
		return;

	CString strMenuItem;
	int iMenuItemCount = _pSubMenu->GetMenuItemCount();
	for( int i=0; i<iMenuItemCount; i++ )
	{
		CMenu* pSubMenu = _pSubMenu->GetSubMenu( i );
		if( pSubMenu )
		{
			CheckMenuItem( pSubMenu, _strGate );
		}
		else
		{
			_pSubMenu->GetMenuString(i,strMenuItem,MF_BYPOSITION );
			if( !_bClearAllCheck&&strMenuItem == _strGate )
			{
				_pSubMenu->CheckMenuItem( i,MF_BYPOSITION|MF_CHECKED );
			}
			else
			{
				_pSubMenu->CheckMenuItem( i,MF_BYPOSITION|MF_UNCHECKED );
			}
		}
	}

}

//void CGateAssignDlg::AssignUnAssignFlightOperationAccordingToGatePriority()
//{
//	std::vector<CFlightOperationForGateAssign*> vOperations;
//	vOperations.clear();
//	m_pGateAssignmentMgr->GetUnassignedFlight(vOperations);
//
//	if (vOperations.empty())
//		return;
//
//	//first get the gate-priority schedule from the input terminal
//	GatePriorityInfo *gatePriorityInfo = GetInputTerminal()->gatePriorityInfo;
//	ALTObjectIDList &procIDList = gatePriorityInfo->altObjectIDList;
//
//	//filter all the unassigned flights according to start time
//	std::sort(vOperations.begin(),vOperations.end(),FlightOperationTimeReserveCompare);
//
//	//make all the unassigned flights in status: 'selected'
//	int unassignedCount = vOperations.size();
//	for(int k = 0; k < unassignedCount; k++)
//	{
//		vOperations[k]->SetSelected(true);
//	}
//
//	m_progAssignStatus.SetPos(1);	//set progress bar
//
//	int nGateCount = m_pGateAssignmentMgr->GetGateCount();
//
//	int nStep = 95/nGateCount;		//leave 5% for unassigned flight assign at end
//
//	//for each GATE in the procIDList
//	int nListCount =(int) procIDList.size();
//	vector<int> vectProcessor;
//	for(int i = 0; i < nListCount; i++)
//	{
//		ALTObjectID pID = procIDList.at(i);
//		for(int j = 0; j < nGateCount; j++)
//		{
//			CAssignStand* pGate = (CAssignStand*)m_pGateAssignmentMgr->GetGate(j);
//			if(pGate->GetStandID().idFits(pID))
//				vectProcessor.push_back(j);
//		}
//
//		//if no gate met in the stand vector in GateAssignmentManager, get next priority stands
//		if(vectProcessor.empty())
//			continue;
//
//		//else do the assignment here.
//		////filter all the unassigned flights according to start time
//		//std::sort(vOperations.begin(),vOperations.end(),FlightOperationTimeReserveCompare);
//
//		////make all the unassigned flights in status: 'selected'
//		/*int*/ unassignedCount = vOperations.size();
//		//for(int k = 0; k < unassignedCount; k++)
//		//{
//		//	vOperations[k]->SetSelected(true);
//		//}
//		//int nIntStep = nStep/int(vectProcessor.size()+1);
//		for(vector<int>::iterator it_processor = vectProcessor.begin(); it_processor != vectProcessor.end(); it_processor++)
//		{
//			//m_progAssignStatus.SetPos(nStep*(i-1)+nIntStep*(*it_processor)+1);		//set progress bar
//
//			m_pGateAssignmentMgr->AssignFlightOperationToGate(*it_processor,vOperations);
//			if (vOperations.empty())
//				break;
//		}
//
//		m_progAssignStatus.SetPos(nStep*i +1);		//set progress bar
//	}
//
//	unassignedCount = vOperations.size();
//	for(int k = 0; k < unassignedCount; k++)
//	{
//		vOperations[k]->SetSelected(true);
//	}
//
//	std::sort(vOperations.begin(),vOperations.end(),FlightOperationTimeReserveCompare);
//
//	//when all GATEs in the procList are filtered,assign the remaining unassigned flights;
//	if (!vOperations.empty())
//	{
//		for(i = 0; i < nGateCount; i++)
//		{
//			////make all the unassigned flights in status: 'selected'
//			//int unassignedCount = vOperations.size();
//			//for(int k = 0; k < unassignedCount; k++)
//			//{
//			//	vOperations[k]->SetSelected(true);
//			//}
//
//			//std::sort(vOperations.begin(),vOperations.end(),FlightOperationTimeReserveCompare);
//
//			m_pGateAssignmentMgr->AssignFlightOperationToGate(i,vOperations);
//
//			if (vOperations.empty())
//				break;
//		}
//	}
//
//	((StandAssignmentMgr*)m_pGateAssignmentMgr)->AdjustAssignedFlight(vOperations);
//
//	m_progAssignStatus.SetPos(100);
//}


//void CGateAssignDlg::AssignUnAssignFlightOperationAccordingToFlightPriority()
//{
//	std::vector<CFlightOperationForGateAssign*> vOperations;
//	vOperations.clear();
//	m_pGateAssignmentMgr->GetUnassignedFlight(vOperations);
//
//	if (vOperations.empty())
//		return;
//
//	//first get the flight-priority schedule from the input terminal
//	FlightPriorityInfo *flightPriorityInfo = GetInputTerminal()->flightPriorityInfo;
//	vector<FlightGate> &vectFlightGate = flightPriorityInfo->m_vectFlightGate;
//
//	int nGateCount = m_pGateAssignmentMgr->GetGateCount();
//
//
//	//make all the unassigned flights in status: 'selected'
//	int unassignedCount = vOperations.size();
//	for(int k = 0; k < unassignedCount; k++)
//	{
//		vOperations[k]->SetSelected(true);
//	}
//
//	m_progAssignStatus.SetPos(1);
//
//	//for each FLIGHT in the vector<FlightGate>
//	int nFlightCount = vectFlightGate.size();
//
//	std::vector<CFlightOperationForGateAssign*>vFitFlights;
//
//	for(int i = 0; i < nFlightCount; i++)
//	{
//		int nStep = 95/nFlightCount;
//		vFitFlights.clear();
//		//make all the unassigned flights in status: 'selected'
//		/*int*/ unassignedCount = vOperations.size();
//		//for(int k = 0; k < unassignedCount; k++)
//		//{
//		//	vOperations[k]->SetSelected(true);
//		//}
//
//		//get the corresponding flight constraint of priority.
//		FlightGate &flightGate = vectFlightGate[i];
//		for(int j = unassignedCount-1; j >=0 ; j--)
//		{
//			if(flightGate.flight.fits(vOperations[j]->getFlight()->getLogEntry()))
//			{
//				vFitFlights.push_back(vOperations[j]);
//				vOperations.erase(vOperations.begin() + j);
//			}
//		}
//
//		//if no flight met in the flight vector in the GateAssignmentManager,start a new loop
//		if(vFitFlights.empty())
//			continue;
//
//		SortFlightOperation( vFitFlights);	//sort flight
//
//		vector<int> vectProcessor;
//		ALTObjectID &procID = vectFlightGate[i].procID;
//		for(k = 0; k < nGateCount; k++)
//		{
//			CAssignStand* pGate = (CAssignStand*)m_pGateAssignmentMgr->GetGate(k);
//			if(pGate->GetStandID().idFits(procID))
//				vectProcessor.push_back(k);
//
//		}
//
//		//assign the fit flight to stand
//
//		int nFitFlightSize = vFitFlights.size();
//		for(int j = 0; j < nFitFlightSize; j++)
//		{
//			//if no gate corresponding in the gate list,assign the flight to any gate;
//			//if(vectProcessor.empty())
//			//{
//			//	//int nIntStep = nStep/int(vectProcessor.size()+1);		//progress step setting
//			//	for(int nGateNum = 0; nGateNum < nGateCount; nGateNum++)
//			//	{
//			//		//m_progAssignStatus.SetPos(nStep*(i-1)+nIntStep*nGateNum+1);		//set progress bar
//
//			//		CFlightOperationForGateAssign* pflight = vFitFlights[j];
//			//		if(m_pGateAssignmentMgr->IsFlightOperationFitInGate(nGateNum, pflight))
//			//		{
//			//			//add the flight into gate vector
//			//			if (m_bIntermediateStand &&
//			//				!((StandAssignmentMgr*)m_pGateAssignmentMgr)->tryToAssignIntermediateParkingStand(pflight->GetAssignmentFlight()->GetFlightIntPart(),nGateNum))
//			//				break;
//			//			pflight->SetGateIdx(nGateNum);
//			//			m_pGateAssignmentMgr->GetGate(nGateNum)->AddFlight(*pflight);
//
//			//			vFitFlights.erase(vFitFlights.begin() + j);
//			//			nFitFlightSize--;
//			//			j--;
//
//			//			break;
//			//		}
//			//	}
//			//}
//			//else	//if there's a gate corresponding to the flight assign to the specific gate.
//			{
//				//int nIntStep = nStep/int(vectProcessor.size()+1);		//progress step setting
//				for(vector<int>::iterator it_processor = vectProcessor.begin(); it_processor != vectProcessor.end(); it_processor++)
//				{
//					//m_progAssignStatus.SetPos(nStep*(i-1)+nIntStep*(*it_processor)+1);		//set progress bar
//
//					CFlightOperationForGateAssign* pflight = vFitFlights[j];
//					if(m_pGateAssignmentMgr->IsFlightOperationFitInGate(*it_processor, pflight))
//					{
//						//add the flight into gate vector
//						if ( !((StandAssignmentMgr*)m_pGateAssignmentMgr)->tryToAssignIntermediateParkingStand(pflight->GetAssignmentFlight()->GetFlightIntPart(),*it_processor))
//							break;
//
//						pflight->SetGateIdx(*it_processor);
//						m_pGateAssignmentMgr->GetGate(*it_processor)->AddFlight(*pflight);
//						vFitFlights.erase(vFitFlights.begin() + j);
//						nFitFlightSize--;
//						j--;
//
//						break;
//					}
//				}
//			}
//		}
//		if(!vFitFlights.empty())
//			vOperations.insert(vOperations.end(),vFitFlights.begin(),vFitFlights.end());
//
//		m_progAssignStatus.SetPos(nStep*i+1);		//set progress bar
//	}
//
//	//make all the unassigned flights in status: 'selected'
//	unassignedCount = vOperations.size();
//	for(int k = 0; k < unassignedCount; k++)
//	{
//		vOperations[k]->SetSelected(true);
//	}
//
//	//reverse sort flight operation
//	std::sort(vOperations.begin(),vOperations.end(),FlightOperationTimeReserveCompare);
//
//	//if no priority, assign flight to any gate
//	if (nFlightCount == 0)
//	{
//		for(int i = 0; i < nGateCount; i++)
//		{
//			m_pGateAssignmentMgr->AssignFlightOperationToGate(i, vOperations);
//		}
//	}
//
//
//
//	((StandAssignmentMgr*)m_pGateAssignmentMgr)->AdjustAssignedFlight(vOperations);
//	m_progAssignStatus.SetPos(100);
//}
