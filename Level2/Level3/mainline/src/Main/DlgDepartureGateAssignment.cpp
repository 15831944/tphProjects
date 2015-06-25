// DlgDepartureGateAssignment.cpp : implementation file
//

#include "stdafx.h" 
#include "termplan.h"
#include "TermPlanDoc.h"
#include "DlgDepartureGateAssignment.h"
#include "DlgStd2GateMapping.h"
#include "Inputs\GateAssignmentMgr.h"
#include "Inputs\TerminalGateAssignmentMgr.h"
// CDlgDepartureGateAssignment dialog

IMPLEMENT_DYNAMIC(CDlgDepartureGateAssignment, CGateAssignDlg)
CDlgDepartureGateAssignment::CDlgDepartureGateAssignment(int nProjID, CAirportDatabase* pAirportDatabase,CWnd* pParent /*=NULL*/)
	: CGateAssignDlg(nProjID,pAirportDatabase, pParent)
{
}

CDlgDepartureGateAssignment::~CDlgDepartureGateAssignment()
{
}

void CDlgDepartureGateAssignment::DoDataExchange(CDataExchange* pDX)
{
	CGateAssignDlg::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgDepartureGateAssignment, CGateAssignDlg)
	ON_COMMAND(IDC_BUTTON_PRIORITY_FLIGHT,OnStandMapping)
	ON_BN_CLICKED(IDC_BUTTON_PRIORITY_GATE, OnButtonUnassignall)
END_MESSAGE_MAP()

BEGIN_EVENTSINK_MAP(CDlgDepartureGateAssignment, CGateAssignDlg)
	//{{AFX_EVENTSINK_MAP(CGateAssignDlg)	
	ON_EVENT(CDlgDepartureGateAssignment, IDC_GTCHARTCTRL, 17 /* RClick */, OnRClickGtchartctrl, VTS_I4 VTS_I4)
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()

void CDlgDepartureGateAssignment::OnRClickGtchartctrl(long nItemIndex, long nLineIndex)
{
}


BOOL CDlgDepartureGateAssignment::OnInitDialog() 
{
	CDialog::OnInitDialog();

	AfxGetApp()->BeginWaitCursor();
	// TODO: Add extra initialization here
	//load flight assignment data
	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();	

	m_pGateAssignmentMgr = new TerminalGateAssignmentMgr(m_nPrjID);

	((TerminalGateAssignmentMgr*)m_pGateAssignmentMgr)->LoadData(GetInputTerminal(), pDoc->m_ProjInfo);
	((TerminalGateAssignmentMgr*)m_pGateAssignmentMgr)->ResetGateContent(DEP_OP,GetInputTerminal());

	//initial gtchart control
	InitGTCtrl();
	//initial flight list ctrl;
	InitFlightListCtrl();

	// load the gate list to gt ctrl.
	SetGate();		

	SetAssignedFlight();
	SetUnassignedFlight();
	GetAllGateProcessor();
	//initial rclick menu
	//InitRClickMenu();

	//init gate print information
	CString strPrintTitle(_T(""));
	strPrintTitle.Format(_T("Departure Gate Allocation - %s Project"),pDoc->m_ProjInfo.name);
	m_gtcharCtrl.SetPrintReportTitle(strPrintTitle.GetBuffer());
	m_gtcharCtrl.SetReportPath(PROJMANAGER->GetAppPath().GetBuffer());

	// set button state
	m_btnSave.EnableWindow(FALSE);
	m_btnCheck.ShowWindow(FALSE);
	//Init FlowConvetor,use to update Detail Gate Flow( only temp)
	InitFlowConvetor();
	
	GetDlgItem(IDC_BUTTON_UNASSIGNALL)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_BUTTON_SUMMARY)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_BUTTON_PRIORITY_GATE)->SetWindowText(_T("Unassign All"));
	GetDlgItem(IDC_BUTTON_PRIORITY_GATE)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_BUTTON_PRIORITY_FLIGHT)->SetWindowText(_T("Stand linkage"));
	GetDlgItem(IDC_BUTTON_PRIORITY_FLIGHT)->ShowWindow(SW_SHOW);//IDC_BUTTON_PRIORITY_FLIGHT

	m_progAssignStatus.ShowWindow(SW_HIDE);
	GetDlgItem(IDC_STATIC_ASSIGNPRO)->ShowWindow(SW_HIDE);

	/////////////////////////////////////////////////
	AfxGetApp()->EndWaitCursor();
	SetWindowText(_T("Departure Gate Assignment"));
	SetIcon( LoadIcon( theApp.m_hInstance,MAKEINTRESOURCE(IDR_MAINFRAME) ),false );
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgDepartureGateAssignment::OnStandMapping(void)
{
	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();	
    
	std::vector<int> vAirport;
	InputAirside::GetAirportList(pDoc->GetProjectID(),vAirport);
	if(vAirport.size()<1) return ;

	CDlgStd2GateMapping dlgMapping(GetInputTerminal(),vAirport.at(0),pDoc->m_ProjInfo.path, CDlgStd2GateMapping::GateType_DEP, &((TerminalGateAssignmentMgr*)m_pGateAssignmentMgr)->m_std2gateConstraint);
	if(dlgMapping.DoModal() == IDOK)
	{
		int i = 0;
		int j = 0;
		int k = 0;
		int nGateCount = m_pGateAssignmentMgr->GetGateCount();

		for(i = 0; i < nGateCount; i++)
		{
			//CAssignGate* pGate = m_pGateAssignmentMgr->GetGate(i);
			//int nFlightCount = pGate->GetFlightCount();
			//for(j = 0; j < nFlightCount; j ++)
			//{
			//	pGate->SetFlightSelected(j, true);
			//}
			//// clear the gate assign flight
			m_gtcharCtrl.ClearAllItemOfLine(i);

		}
		m_pGateAssignmentMgr->RemoveSelectedFlight();
		SetUnassignedFlight();


		//make all the unassigned flights in status: 'selected'
		int unassignedCount = m_pGateAssignmentMgr->GetUnassignedScheduleFlightList().size();
		for(i = 0; i < unassignedCount; i++)
		{
			m_pGateAssignmentMgr->SetUnAssignedFlightSelected(i, 1);
		}
		//assign arrival gate

		{
			std::vector< CFlightOperationForGateAssign*> vUnassignFlight;
			vUnassignFlight.clear();
			m_pGateAssignmentMgr->GetUnassignedFlight(vUnassignFlight);

			int nUnassign = 0;
			for (; nUnassign < (int)vUnassignFlight.size(); ++nUnassign)
			{			
				CFlightOperationForGateAssign* pCurrentGateFlight = vUnassignFlight[nUnassign];
				ALTObjectID flightStandID = pCurrentGateFlight->getFlight()->getDepStand();

				ProcessorID depGateSet = ((TerminalGateAssignmentMgr*)m_pGateAssignmentMgr)->m_std2gateConstraint.GetDepGateID(pCurrentGateFlight);
				if (!depGateSet.isBlank())
				{
					//add to chart
					for (int nGate = 0; nGate < nGateCount; ++nGate)
					{
						CAssignTerminalGate* pGate = (CAssignTerminalGate *)m_pGateAssignmentMgr->GetGate(nGate);
						if (pGate == NULL)
							continue;
						if (pGate && pGate->GetProcessID() == depGateSet)
						{
							((Flight*)pCurrentGateFlight->getFlight())->setDepGate(depGateSet);
							pCurrentGateFlight->SetGateIdx(nGate);
							pGate->AddFlight(*pCurrentGateFlight);


							vUnassignFlight.erase(vUnassignFlight.begin() + nUnassign);							

							nUnassign = nUnassign -1;
							break;
						}
					}
				}
			}
			m_pGateAssignmentMgr->AdjustAssignedFlight(vUnassignFlight);
		}
		//for (int nGate = 0; nGate < nGateCount; ++nGate)
		//{
		//	CAssignTerminalGate* pGate = (CAssignTerminalGate *)m_gateAssignmentMgr.GetGate(nGate);
		//	if (pGate == NULL)
		//		continue;


		//	std::vector< CFlightOperationForGateAssign>& vUnassignFlight = m_gateAssignmentMgr.GetUnassignedFlight();
		//	int unassignedCount = vUnassignFlight.size();

		//	for(k = 0; k < unassignedCount; k++)
		//	{
		//		CFlightOperationForGateAssign& currentGateFlight = vUnassignFlight[k];
		//		ALTObjectID flightStandID = currentGateFlight.getStand();

		//		ProcessorID depGateSet = m_gateAssignmentMgr.m_std2gateConstraint.GetDepGateID(flightStandID);
		//		if (!depGateSet.isBlank())
		//		{
		//			if (pGate && pGate->GetProcessID() == depGateSet)
		//			{
		//				currentGateFlight.setDepGate(depGateSet);
		//				pGate->AddFlight(currentGateFlight);

		//				vUnassignFlight.erase(vUnassignFlight.begin() + k);

		//				k = k -1;
		//				//if ( k < 0)
		//				//	k = 0;

		//				unassignedCount = vUnassignFlight.size();
		//			}
		//		}
		//	}
		//}


		//
		unassignedCount = m_pGateAssignmentMgr->GetUnassignedScheduleFlightList().size();
		for(int  nUnassign = 0; nUnassign < unassignedCount; nUnassign++)
		{
			m_pGateAssignmentMgr->SetUnAssignedFlightSelected(nUnassign, 0);
		}	

		m_pGateAssignmentMgr->SetAssignedFlightSelected(-1, -1, true);

		SetAssignedFlight();
		SetUnassignedFlight();
	}
	m_btnSave.EnableWindow(TRUE);
}

void CDlgDepartureGateAssignment::SetAssignedFlight()
{
	m_gtcharCtrl.SetEnableOverlap(TRUE);
	char szItem[256];
	CString strToolTips;
	long lBegineTime, lSeverTime;

	m_pGateAssignmentMgr->SetAssignedFlightSelected( -1, -1, true);
	int nGateCount = m_pGateAssignmentMgr->GetGateCount();
	for( int i=0; i<nGateCount; i++ )
	{
		CAssignTerminalGate* pGate = (CAssignTerminalGate *)m_pGateAssignmentMgr->GetGate(i);
		int nFlightCount = pGate->GetFlightCount();
		int nItemIndex = 0;
		for( int j=0; j<nFlightCount; j++ )
		{
			CFlightOperationForGateAssign flight = pGate->GetFlight(j);

			flight.getFlight()->getFlightIDString(szItem);
			pGate->GetToolTips( j, strToolTips, GetInputTerminal()->inStrDict);


			lBegineTime = flight.GetStartTime().asMinutes();
			ProcessorID GateID ;
			GateID.SetStrDict(GetInputTerminal()->inStrDict) ;
			GateID = pGate->GetProcessID();	
			ElapsedTime tService ;
           CGateAssignPreferenceItem* PreferenceItem = ((TerminalGateAssignmentMgr*)m_pGateAssignmentMgr)->m_std2gateConstraint.GetDepPreferenceMan()->FindItemByGateID(GateID) ;
		   if(PreferenceItem == NULL||!PreferenceItem->GetFlightDurationtime(flight.getFlight()->getType('D'),tService)) 
				 tService = flight.GetEndTime() - flight.GetStartTime();

		   //if (PreferenceItem && PreferenceItem->GetFlightDurationtime(flight.getFlight()->getType('D'),tService))
		   //{
			  // tService = MIN(tService,flight.GetEndTime() - flight.GetStartTime());
		   //}
		   //else
		   //{
			  // tService = flight.GetEndTime() - flight.GetStartTime();
		   //}

			lSeverTime = tService.asMinutes();	   
			long lIdx;
			COLORREF colorBegin;
			COLORREF colorEnd;
			if(flight.getFlight()->getFlightType() == ItinerantFlightType)
			{
				colorBegin = RGB(204,255,204);
				colorEnd = RGB(150,200,156);
			}
			else
			{
				colorBegin =RGB(255,255,255);; 
				colorEnd = RGB(132,163,195);
			}
			if( m_gtcharCtrl.AddItem( &lIdx, i, szItem,NULL,lBegineTime,lSeverTime ,true,colorBegin,colorEnd) == 0)	// add the item
			{
				m_gtcharCtrl.SetItemData( lIdx, nItemIndex++ );
			}
		}
	}

	m_pGateAssignmentMgr->RemoveSelectedFlight();
	m_gtcharCtrl.ClearItemSeled(false);
	m_gtcharCtrl.SetEnableOverlap(FALSE);
}

void CDlgDepartureGateAssignment::SetGate()
{
	m_gtcharCtrl.Clear(TRUE);
	CString strGateName = _T("");

	m_gtcharCtrl.SetResourceBarCaption( "Departure Gate" );
	int nGateCount = m_pGateAssignmentMgr->GetGateCount();

	for( int i=0; i<nGateCount; i++ )
	{
		CAssignTerminalGate* pStandGate = (CAssignTerminalGate*) m_pGateAssignmentMgr->GetGate(i);
		strGateName = pStandGate->GetName();
		//if(strGateName.Find(_T("DEPGATE")) != -1)
		//{
			long lGateIndex = m_gtcharCtrl.AddLine(strGateName,NULL );
			m_gtcharCtrl.SetLineData( lGateIndex, i);
		//}
	}
}

void CDlgDepartureGateAssignment::ButtonConstraints()
{
	return;
}

void CDlgDepartureGateAssignment::OnButtonUnassignall()
{
	int i = 0;
	int nGateCount = m_pGateAssignmentMgr->GetGateCount();

	for(i = 0; i < nGateCount; i++)
	{
		// clear the gate assign flight
		m_gtcharCtrl.ClearAllItemOfLine(i);

	}
	m_pGateAssignmentMgr->UnassignAllFlight();
	SetUnassignedFlight();

	m_btnSave.EnableWindow(TRUE);
}
