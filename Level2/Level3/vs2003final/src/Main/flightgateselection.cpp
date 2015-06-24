// FlightGateSelection.cpp : implementation file
//

#include "stdafx.h"
#include "termplan.h"
#include "FlightGateSelection.h"
#include "FlightPriorityDlg.h"
#include "../Common/AirsideFlightType.h"
#include "DlgNewFlightType.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFlightGateSelectionDlg dialog


CFlightGateSelectionDlg::CFlightGateSelectionDlg(CWnd* pParent /*=NULL*/, int nAirportID, int nIndex)
	: CDialog(CFlightGateSelectionDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFlightGateSelectionDlg
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	//m_flight.setFullID("DEFAULT");
	m_bChanged = false;
	m_nIndex = nIndex;	
	m_nAirportID = nAirportID;
	
}
 

void CFlightGateSelectionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFlightGateSelectionDlg)
	DDX_Control(pDX, IDC_TREE_FLIGHTSECTOR, m_treeALTobject);
	DDX_Control(pDX, IDC_EDIT_FLIGHTSECTOR, m_edtFlightSector);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFlightGateSelectionDlg, CDialog)
	//{{AFX_MSG_MAP(CFlightGateSelectionDlg)
	ON_BN_CLICKED(IDC_BUTTON_FLIGHTSECTOR_SELECTION, OnButtonFlightsectorSelection)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFlightGateSelectionDlg message handlers

void CFlightGateSelectionDlg::OnButtonFlightsectorSelection() 
{
	//CFlightDialog flightDlg(NULL);
	//if(flightDlg.DoModal() != IDOK)
	//	return;	
	////char strFlight[200];
	//CString strFlight;
	//flightDlg.GetFlightSelection().screenPrint(strFlight);
	//m_flight = flightDlg.GetFlightSelection();

	DlgNewFlightType dlg(m_flight);
	if (dlg.DoModal() != IDOK)
		return;
	m_flight = dlg.GetFltType();
	CString strFlight = m_flight.getPrintString();
	m_edtFlightSector.SetWindowText(strFlight);
}

void CFlightGateSelectionDlg::ReloadDatabase()
{
	if( m_nIndex >= 0 )
	{
		m_flight = ((CFlightPriorityDlg *)GetParent())->m_vectFlightGate[m_nIndex].flight;
		char strText[200];
		CString strText1 = m_flight.getPrintString();
		//m_flight.screenPrint(strText1);
		m_edtFlightSector.SetWindowText(strText1);

		((CFlightPriorityDlg *)GetParent())->m_vectFlightGate[m_nIndex].procID.printID( strText );
		m_treeALTobject.m_strSelectedID = strText;
	}
	
	//m_treeProc.LoadData(GetInputTerminal(), NULL, GateProc);

	m_treeALTobject.LoadData(m_nAirportID,ALT_STAND);
}

extern CTermPlanApp theApp;
InputTerminal* CFlightGateSelectionDlg::GetInputTerminal()
{
//	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)(m_pParentWnd->GetParent()))->GetDocument();
	POSITION ps = theApp.m_pDocManager->GetFirstDocTemplatePosition();
	POSITION ps2 = theApp.m_pDocManager->GetNextDocTemplate(ps)->GetFirstDocPosition();
	CTermPlanDoc *pDoc = (CTermPlanDoc*)(theApp.m_pDocManager->GetNextDocTemplate(ps)->GetNextDoc(ps2));
    return (InputTerminal*)&pDoc->GetTerminal();
}

BOOL CFlightGateSelectionDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	ReloadDatabase();	

	//set the flight/sector string.
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CFlightGateSelectionDlg::OnOK() 
{

	if(m_nIndex == -1)
	{
		FlightGate flightGate;
		
		//get gate processor id;
		HTREEITEM hSelItem = m_treeALTobject.GetSelectedItem();
		if(hSelItem == NULL)
		{
			CDialog::OnOK();
			return;
		}
		CString strSelItem = m_treeALTobject.GetTreeItemFullString(hSelItem);
	/*	ProcessorID *procID = new ProcessorID;
		procID->SetStrDict(GetInputTerminal()->inStrDict);*/
		ALTObjectID objID(strSelItem.GetBuffer(strSelItem.GetLength() + 1));
		flightGate.procID = objID;
		
		//get flight constraint;
		flightGate.flight = m_flight;
		
		((CFlightPriorityDlg *)GetParent())->m_vectFlightGate.push_back(flightGate);
		
		//delete procID;
		
		m_bChanged = true;
		CDialog::OnOK();
	}
	else
	{
		FlightGate flightGate;
		
		//get gate processor id;
		HTREEITEM hSelItem = m_treeALTobject.GetSelectedItem();
		if(hSelItem == NULL)
		{
			CDialog::OnOK();
			return;
		}
		CString strSelItem = m_treeALTobject.GetTreeItemFullString(hSelItem);
		//ProcessorID *procID = new ProcessorID;
		//procID->SetStrDict(GetInputTerminal()->inStrDict);
		//procID->setID();
		
		flightGate.procID = strSelItem.GetBuffer(strSelItem.GetLength() + 1);
		
		//get flight constraint;
		flightGate.flight = m_flight;
		
		((CFlightPriorityDlg *)GetParent())->m_vectFlightGate[m_nIndex] = flightGate;
		
		//delete procID;
		
		m_bChanged = true;
		CDialog::OnOK();
	}

}

