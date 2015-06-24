#include "StdAfx.h"
#include "termplan.h"
#include "resource.h"
#include ".\dlgstandassignment.h"
#include "TermPlanDoc.h"
#include "Inputs\GateAssignmentMgr.h"
#include "inputs\StandAssignmentMgr.h"
#include "inputs\GatePriorityInfo.h"
#include "Inputs\FlightPriorityInfo.h"
#include "GatePriorityDlg.h"
#include "FlightPriorityDlg.h"
#include "../InputAirside/CParkingStandBuffer.h"
#include "GateAssignSummaryDlg.h"

extern const CString c_setting_regsectionstring;
const CString c_gateass_sorting_entry = "Gate Assignment Sorting String";

IMPLEMENT_DYNAMIC(CDlgStandAssignment, CGateAssignDlg)
CDlgStandAssignment::CDlgStandAssignment(int nProjID, CAirportDatabase* pAirportDatabase,CWnd* pParent /*=NULL*/)
	: CGateAssignDlg(nProjID,pAirportDatabase, pParent)
{
	m_pStandBufferList = new ParkingStandBufferList(nProjID, pAirportDatabase);
	m_pStandBufferList->LoadData();
}

CDlgStandAssignment::~CDlgStandAssignment()
{
	delete m_pStandBufferList;
}

void CDlgStandAssignment::DoDataExchange( CDataExchange* pDX )
{
	CGateAssignDlg::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CDlgStandAssignment, CGateAssignDlg)
	ON_BN_CLICKED(IDC_BUTTON_PRIORITY_GATE, OnButtonPriorityGate)
	ON_BN_CLICKED(IDC_BUTTON_PRIORITY_FLIGHT, OnButtonPriorityFlight)
	ON_BN_CLICKED(IDC_BUTTON_SUMMARY, OnButtonSummary)
	ON_BN_CLICKED(IDC_BUTTON_UNASSIGNALL, OnButtonUnassignall)
	ON_BN_CLICKED(IDC_CHECK_LINKLINE, OnBtnCheckLinkLine)
END_MESSAGE_MAP()

BEGIN_EVENTSINK_MAP(CDlgStandAssignment, CGateAssignDlg)

END_EVENTSINK_MAP()

void CDlgStandAssignment::GatePriorityAssign()
{
	//first select the assigned flights 
	//and remove them from the gantt chart.
	int nGateCount = m_pGateAssignmentMgr->GetGateCount();

	for(int i = 0; i < nGateCount; i++)
	{
		CAssignGate* pGate = m_pGateAssignmentMgr->GetGate(i);
		int nFlightCount = pGate->GetFlightCount();
		for(int j = 0; j < nFlightCount; j ++)
		{
			pGate->SetFlightSelected(j, true);
		}
		// clear the gate assign flight
		m_gtcharCtrl.ClearAllItemOfLine(i);

	}
	m_pGateAssignmentMgr->UnassignAllFlight();
	SetUnassignedFlight();


	//make all the unassigned flights in status: 'selected'
	int unassignedCount = m_pGateAssignmentMgr->GetUnassignedScheduleFlightList().size();
	for(int i = 0; i < unassignedCount; i++)
	{
		m_pGateAssignmentMgr->SetUnAssignedFlightSelected(i, 1);
		FlightForAssignment* pFlight = m_pGateAssignmentMgr->GetUnassignedScheduleFlightList().at(i);
		pFlight->EmptyStand();
	}

	AssignFlightAccordingToGatePriority();

	//if still remaining unassigned flights
	//un-select them;
	//make all the unassigned flights in status: 'selected'
	unassignedCount = m_pGateAssignmentMgr->GetUnassignedScheduleFlightList().size();
	for(int i = 0; i < unassignedCount; i++)
	{
		m_pGateAssignmentMgr->SetUnAssignedFlightSelected(i, 0);
	}	

	m_pGateAssignmentMgr->SetAssignedFlightSelected(-1, -1, true);

	SetAssignedFlight();
	SetUnassignedFlight();
}

void CDlgStandAssignment::FlightPriorityAssign()
{
	//first select the assigned flights
	//and remove them from the gantt chart
	int nGateCount = m_pGateAssignmentMgr->GetGateCount();

	for(int i = 0; i < nGateCount; i++)
	{
		CAssignGate* pGate =m_pGateAssignmentMgr->GetGate(i);
		int nFlightCount = pGate->GetFlightCount();
		for(int j = 0; j < nFlightCount; j++)
		{
			pGate->SetFlightSelected(j, true);
		}
		// clear the gate assign flight
		m_gtcharCtrl.ClearAllItemOfLine(i);
	}
	m_pGateAssignmentMgr->UnassignAllFlight();
	SetUnassignedFlight();

	//make all the unassigned flights in the status: 'selected'
	int unassignedCount = m_pGateAssignmentMgr->GetUnassignedScheduleFlightList().size();
	for(int i = 0; i < unassignedCount; i++)
	{
		m_pGateAssignmentMgr->SetUnAssignedFlightSelected(i, 1);
		FlightForAssignment* pFlight = m_pGateAssignmentMgr->GetUnassignedScheduleFlightList().at(i);
		pFlight->EmptyStand();
	}

	AssignFlightAccordingToFlightPriority();



	unassignedCount = m_pGateAssignmentMgr->GetUnassignedScheduleFlightList().size();
	for(i = 0; i < unassignedCount; i++)
	{
		m_pGateAssignmentMgr->SetUnAssignedFlightSelected(i, 0);
	}

	m_pGateAssignmentMgr->SetAssignedFlightSelected(-1, -1, true);

	SetAssignedFlight();	
	SetUnassignedFlight();
}

BOOL CDlgStandAssignment::OnInitDialog()
{
	CDialog::OnInitDialog();

	AfxGetApp()->BeginWaitCursor();
	// TODO: Add extra initialization here
	//load flight assignment data
	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();	

	m_pGateAssignmentMgr = new StandAssignmentMgr(m_nPrjID);

	((StandAssignmentMgr*)m_pGateAssignmentMgr)->getConstraint().SetParkingStandBufferList(m_pStandBufferList);

	try				//init ac type stand constraint, and catch the exception
	{
		((StandAssignmentMgr*)m_pGateAssignmentMgr)->LoadData(GetInputTerminal(), pDoc->m_ProjInfo);		
	}
	catch (CADOException* error)
	{
		CString strError = _T("");
		strError.Format("Database operation error: %s",error->ErrorMessage());
		MessageBox(strError);

		delete error;
		error = NULL;
	}

	SetWindowText("Stand Assignment");
	((StandAssignmentMgr*)m_pGateAssignmentMgr)->ResetGateContent(NO_OP,GetInputTerminal());

	//initial gtchart control
	InitGTCtrl();
	//initial flight list ctrl;
	InitFlightListCtrl();

	// load the gate list to gt ctrl.
	SetGate();		

	m_gtcharCtrl.SetEnableOverlap(TRUE);
	SetAssignedFlight();
	SetUnassignedFlight();
	m_gtcharCtrl.SetEnableOverlap(FALSE);
	CString strPrintTitle(_T(""));
	strPrintTitle.Format(_T("Stand Allocation - %s Project"),pDoc->m_ProjInfo.name);
	m_gtcharCtrl.SetPrintReportTitle(strPrintTitle.GetBuffer());
	m_gtcharCtrl.SetReportPath(PROJMANAGER->GetAppPath().GetBuffer());
	GetAllGateProcessor();
	//initial rclick menu
	//InitRClickMenu();

	// set button state
	m_btnSave.EnableWindow(FALSE);

	//Init FlowConvetor,use to update Detail Gate Flow( only temp)
	InitFlowConvetor();
	/////////////////////////////////////////////////
	AfxGetApp()->EndWaitCursor();

	//SetResize(IDC_PROGRESS_ASSIGNMENT,SZ_MIDDLE_CENTER,SZ_MIDDLE_RIGHT);
	m_progAssignStatus.SetRange(0, 100);

	m_progAssignStatus.ShowWindow(SW_HIDE);
	GetDlgItem(IDC_STATIC_ASSIGNPRO)->ShowWindow(SW_HIDE);

	SetIcon( LoadIcon( theApp.m_hInstance,MAKEINTRESOURCE(IDR_MAINFRAME) ),false );
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgStandAssignment::OnButtonPriorityGate()
{
	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();	

	std::vector<int> vAirport;
	InputAirside::GetAirportList(pDoc->GetProjectID(),vAirport);
	if(vAirport.empty()) 
		return ;

//	CGatePriorityDlg gatePriorityDlg( GetInputTerminal(),vAirport.at(0) );
	CGatePriorityDlg gatePriorityDlg( GetInputTerminal(),vAirport.at(0),m_nPrjID );
	if(gatePriorityDlg.DoModal() != IDOK)
		return;

	//show progress bar
	GetDlgItem(IDC_STATIC_ASSIGNPRO)->ShowWindow(SW_SHOW);
	m_progAssignStatus.ShowWindow(SW_SHOW);

	//assign stand automatically
	GatePriorityAssign();
	m_progAssignStatus.SetPos(0);		//reset progress bar

	GetDlgItem(IDC_STATIC_ASSIGNPRO)->ShowWindow(SW_HIDE);
	m_progAssignStatus.ShowWindow(SW_HIDE);	
	m_btnSave.EnableWindow(TRUE);

}

void CDlgStandAssignment::OnButtonPriorityFlight()
{
	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();	

	std::vector<int> vAirport;
	InputAirside::GetAirportList(pDoc->GetProjectID(),vAirport);
	if(vAirport.empty()) 
		return ;

	CFlightPriorityDlg gatePriorityDlg(pDoc->GetProjectID(),vAirport.at(0));
	if(gatePriorityDlg.DoModal() != IDOK)
		return;

	//show progress bar
	GetDlgItem(IDC_STATIC_ASSIGNPRO)->ShowWindow(SW_SHOW);
	m_progAssignStatus.ShowWindow(SW_SHOW);	

	//assign stand automatically
	FlightPriorityAssign();
	m_progAssignStatus.SetPos(0);		//reset progress bar

	GetDlgItem(IDC_STATIC_ASSIGNPRO)->ShowWindow(SW_HIDE);
	m_progAssignStatus.ShowWindow(SW_HIDE);	
	m_btnSave.EnableWindow(TRUE);
}

void CDlgStandAssignment::SetGate()
{
	m_gtcharCtrl.Clear(TRUE);
	CString strGateName = _T("");

	m_gtcharCtrl.SetResourceBarCaption( "Stand Gate" );
	int nGateCount = m_pGateAssignmentMgr->GetGateCount();

	for( int i=0; i<nGateCount; i++ )
	{
		CAssignStand* pStandGate = (CAssignStand*) m_pGateAssignmentMgr->GetGate(i);
		strGateName = pStandGate->GetName();
		long lGateIndex = m_gtcharCtrl.AddLine(strGateName,NULL );
		m_gtcharCtrl.SetLineData( lGateIndex, i);
	}

}

void CDlgStandAssignment::AssignFlightAccordingToGatePriority()
{
	StandAssignmentMgr* pAssignMgr = (StandAssignmentMgr*)m_pGateAssignmentMgr;
	ASSERT(pAssignMgr);		//error

	if (pAssignMgr == NULL)
		return;

	if (pAssignMgr->GetTowoffStandDataList() == NULL)
	{
		MessageBox(_T("Stand Assignment needs information from Tow Off Criteria . \r\n It could be defined at Tow Off Criteria which under Resource Management -> Parking stands(gates), and make sure the defintions cover all the flights."),_T("ARCPort Warning"), MB_OK);
		return;
	}

	GatePriorityInfo *gatePriorityInfo = GetInputTerminal()->gatePriorityInfo;
	ALTObjectIDList &procIDList = gatePriorityInfo->altObjectIDList;

	int nListCount =(int) procIDList.size();
	int nGateCount = pAssignMgr->GetGateCount();
	vector<int> vectGate;
	for(int i = 0; i < nListCount; i++)
	{
		ALTObjectID pID = procIDList.at(i);
		for(int j = 0; j < nGateCount; j++)
		{
			CAssignStand* pGate = (CAssignStand*)pAssignMgr->GetGate(j);
			if(pGate->GetStandID().idFits(pID))
				vectGate.push_back(j);
		}
	}

	pAssignMgr->SortUnAssignedFlight();
	int unassignedCount = pAssignMgr->GetUnassignedScheduleFlightList().size();

	m_progAssignStatus.SetPos(1);
// 	for(int k = 0; k < unassignedCount; k++)
// 	{
// 		if (pAssignMgr->AssignFlightAccordingToGatePrioritySequence(k,vectGate))
// 		{
// 			k--;
// 			unassignedCount--;
// 		}
// 
// 		if (k >=0)
// 		{
// 			int nPos = k/unassignedCount; 
// 			m_progAssignStatus.SetPos( max(nPos, 1));		//set progress bar
// 		}
// 
// 
// 	}

	int nPriorityCount = (int)vectGate.size();
	for (int k = 0; k < nPriorityCount; k++)
	{
		int idx = vectGate.at(k);
		pAssignMgr->AssignFlightArccordingToGatePriority(idx,vectGate);
		int nPos = k/nPriorityCount * 100;
		m_progAssignStatus.SetPos(nPos);
	}
	m_progAssignStatus.SetPos(100);

}

void CDlgStandAssignment::AssignFlightAccordingToFlightPriority()
{
	StandAssignmentMgr* pAssignMgr = (StandAssignmentMgr*)m_pGateAssignmentMgr;
	ASSERT(pAssignMgr);		//error

	if (pAssignMgr == NULL)
		return;
	if (pAssignMgr->GetTowoffStandDataList() == NULL)
	{
		MessageBox(_T("Stand Assignment needs information from Tow Off Criteria . \r\n It could be defined at Tow Off Criteria which under Resource Management -> Parking stands(gates), and make sure the defintions cover all the flights."),_T("ARCPort Warning"), MB_OK);
		return;
	}

	pAssignMgr->SortUnAssignedFlight();
	//first get the flight-priority schedule from the input terminal
	FlightPriorityInfo *flightPriorityInfo = GetInputTerminal()->flightPriorityInfo;

	//int nGateCount = pAssignMgr->GetGateCount();
	//std::vector<StandAssignmentMgr::FLIGHTTYPESTANDIDXLIST> vFltTypeStandList;

	//std::vector<int> vStandIdxList;
	//int nCount = flightPriorityInfo->m_vectFlightGate.size();
	//for (int i = 0; i < nCount; i++)
	//{
	//	FlightGate flightGate = flightPriorityInfo->m_vectFlightGate.at(i);

	//	StandAssignmentMgr::FLIGHTTYPESTANDIDXLIST fltStandInfo;
	//	fltStandInfo.first = flightGate.flight;
	//	fltStandInfo.second = 

	//	for(int j = 0; j < nGateCount; j++)
	//	{		
	//		CAssignStand* pGate = (CAssignStand*)pAssignMgr->GetGate(j);
	//		if(pGate->GetStandID().idFits(flightGate.procID))
	//			fltStandInfo.second.push_back(j);	
	//	}

	//	vFltTypeStandList.push_back(fltStandInfo);
	//}



	int unassignedCount = m_pGateAssignmentMgr->GetUnassignedScheduleFlightList().size();

	m_progAssignStatus.SetPos(1);
// 	for(int k = 0; k < unassignedCount; k++)
// 	{
// 
// 		if (pAssignMgr->AssignFlightAccordingToFlightPrioritySequence(k,flightPriorityInfo))
// 		{
// 			k--;
// 			unassignedCount--;
// 		}
// 
// 		if (k >=0)
// 		{
// 			int nPos = k/unassignedCount; 
// 			m_progAssignStatus.SetPos( max(nPos, 1));		//set progress bar
// 		}
// 
// 
// 	}
	int nPriorityCount = (int)flightPriorityInfo->m_vectFlightGate.size();
	for (int k = 0; k < nPriorityCount; k++)
	{
		FlightGate& flightGate = flightPriorityInfo->m_vectFlightGate.at(k);
		pAssignMgr->AssignFlightAccordingToFlightPriority(flightGate,flightPriorityInfo);
		int nPos = k/nPriorityCount * 100;
		m_progAssignStatus.SetPos(nPos);
	}
	m_progAssignStatus.SetPos(100);

}

void CDlgStandAssignment::SetUnassignedFlight()
{
	m_listctrlFlightList.DeleteAllItems();
	std::vector<FlightForAssignment*> vectFlight = m_pGateAssignmentMgr->GetUnassignedScheduleFlightList();
	int count = vectFlight.size();

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

			ElapsedTime estArrTime = pFlight->getArrTime();
			COleDateTime startTime;
			startTime.SetTime(estArrTime.GetHour(), estArrTime.GetMinute(), estArrTime.GetSecond());
			CString strTime = startTime.Format("%H:%M");
			CString strTemp;
			strTemp.Format(_T("Day%d %s"), estArrTime.GetDay(), strTime);
			strcpy(str,strTemp);
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
}

void CDlgStandAssignment::SetAssignedFlight()
{

	m_pGateAssignmentMgr->SetAssignedFlightSelected( -1, -1, true);
	int nGateCount = m_pGateAssignmentMgr->GetGateCount();
	for( int i=0; i<nGateCount; i++ )
	{
		SetGateAssignedFlight(i);
	}

	for (i = 0; i < nGateCount; i++)
	{
		FreshLinkLineData(i);
	}
	if (m_btnCheck.GetCheck())
	{
		m_gtcharCtrl.EnableLinkLine();
	}
	else
	{
		m_gtcharCtrl.DisableLinkLine();
	}
	m_pGateAssignmentMgr->RemoveSelectedFlight();
	m_gtcharCtrl.ClearItemSeled(false);
}

void CDlgStandAssignment::SetGateAssignedFlight( int nGateIdx )
{
	if (nGateIdx < 0)
		return;

	char szItem[256];
	CString strToolTips;
	long lBegineTime, lSeverTime;

	CAssignGate* pGate = m_pGateAssignmentMgr->GetGate(nGateIdx);
	int nItemIndex = 0;
	for( int j=0; j<pGate->GetFlightCount(); j++ )
	{
		CFlightOperationForGateAssign flight = pGate->GetFlight(j);

		flight.getFlight()->getFlightIDString(szItem);
		pGate->GetToolTips( j, strToolTips, GetInputTerminal()->inStrDict);

		lBegineTime = flight.GetStartTime().asMinutes();

		ElapsedTime tService = flight.GetEndTime() - flight.GetStartTime();
		lSeverTime = tService.asMinutes();

		// TRACE("begine:%ld; severtime:%ld\r\n",lBegineTime,lSeverTime);
		// if can't add flight to a gate , push it to unassignFlight;
		long lIdx;
		COLORREF colorBegin;
		COLORREF colorEnd;
		if (!flight.FlightAssignToStandValid())
		{
			colorBegin = RGB(255,135,135);
			colorEnd = RGB(255,196,196);
		}
		else
		{
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
					colorBegin =RGB(255,255,255);
					colorEnd = RGB(132,163,195);
				}
			}
		}
		
		if( m_gtcharCtrl.AddItem( &lIdx, nGateIdx, szItem,NULL,lBegineTime,lSeverTime ,true,colorBegin,colorEnd) != 0)	//can't add the item
		{
//			m_pGateAssignmentMgr->SetAssignedFlightSelected( nGateIdx, j, false );


// 			if (flightOp)
// 			{
// 				if (flight == *flightOp)
// 					continue;
// 				ClearFlightInGtChart(flightOp);
// 			}
// 
// 			flightOp = flight.GetAssignmentFlight()->GetFlightDepPart();
// 
// 			if (flightOp)
// 			{
// 				if (flight == *flightOp)
// 					continue;
// 				ClearFlightInGtChart(flightOp);
// 			}
// 
// 			if (flight.GetAssignmentFlight()->GetFlightIntPart() && flight.getOpType() != INTPAKRING_OP)
// 			{
// 				CFlightOperationForGateAssign* flightOp =flight.GetAssignmentFlight()->GetFlightIntPart();
// 				ClearFlightInGtChart(flightOp);
// 			}
// 
// 			m_pGateAssignmentMgr->RemoveAssignedFlight(flight.GetAssignmentFlight());
// 			j--;
		}

		
		m_gtcharCtrl.SetItemData( lIdx, nItemIndex++ );
		
	}
}

void CDlgStandAssignment::OnBtnCheckLinkLine()
{
	if (m_btnCheck.GetCheck())
	{
		m_gtcharCtrl.EnableLinkLine();
	}
	else
	{
		m_gtcharCtrl.DisableLinkLine();
	}
	m_gtcharCtrl.UpdateViews(TRUE);
}

void CDlgStandAssignment::OnButtonSummary() 
{
	CGateAssignSummaryDlg dlg( m_pGateAssignmentMgr );
	dlg.DoModal();

}

// unassign all flight
void CDlgStandAssignment::OnButtonUnassignall() 
{
	CWaitCursor wc;
	// unassign all flight from gate assign manger
	m_pGateAssignmentMgr->UnassignAllFlight();

	// clear gt_ctrl
	for( int i=0; i<m_gtcharCtrl.GetLineNum(); i++ )
	{
		FreshGataData( i );
		//m_gtcharCtrl.ClearAllItemOfLine( i );
	}

	for(i=0; i<m_gtcharCtrl.GetLineNum(); i++ )
	{
		FreshLinkLineData(i);
	}
	if (m_btnCheck.GetCheck())
	{
		m_gtcharCtrl.EnableLinkLine();
	}
	else
	{
		m_gtcharCtrl.DisableLinkLine();
	}
	// update list_ctrl
	SetUnassignedFlight();

	m_btnSave.EnableWindow(TRUE);
}

void CDlgStandAssignment::InitFlightListCtrl()
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
}