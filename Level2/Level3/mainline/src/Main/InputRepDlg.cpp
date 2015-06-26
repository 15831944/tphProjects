// InputRepDlg.cpp : implementation file
//

#include "stdafx.h"
#include "termplan.h"
#include "InputRepDlg.h"
#include "inputs\DistList.h"
#include "inputs\Pax_DB.h"
#include "inputs\PaxData.h"
#include "inputs\fltData.h"
#include "inputs\schedule.h"
#include "common\template.h"
#include "engine\terminal.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CInputRepDlg dialog


CInputRepDlg::CInputRepDlg(CTermPlanDoc* _pDoc, CWnd* pParent /*=NULL*/)
	: CDialog(CInputRepDlg::IDD, pParent)
{
	m_pDoc = _pDoc;
	//{{AFX_DATA_INIT(CInputRepDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CInputRepDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CInputRepDlg)
	DDX_Control(pDX, IDC_LIST_INPUTREP, m_listRep);
	//}}AFX_DATA_MAP
}


BOOL CInputRepDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here

	DWORD dwStyle = m_listRep.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES;
	m_listRep.SetExtendedStyle( dwStyle );

	m_listRep.InsertColumn( 0, "  ", LVCFMT_LEFT, 300 );
	m_listRep.InsertColumn( 1, "  ", LVCFMT_RIGHT, 300 );

	LoadDatabase();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CInputRepDlg::LoadDatabase()
{
	int nNextIdx = 0;

	LoadProc(nNextIdx);
	LoadPax(nNextIdx);
	LoadBaggage(nNextIdx);
	LoadFlight(nNextIdx);
	LoadLayout(nNextIdx);
}

CString CInputRepDlg::GetLevelString( CString _str, int _nLevel )
{
	CString csTitle;
	for( int i=0; i<_nLevel; i++ )
	{
		csTitle += CString( "        " );
	}
	csTitle += _str;
	return csTitle;
}


CString CInputRepDlg::GetLevelString( double _dVal, int _nLevel )
{
	CString csVal;
	csVal.Format( "%.0f", _dVal );

	for( int i=0; i<_nLevel; i++ )
	{
		csVal += CString( "            " );
	}
	return csVal;
}

void CInputRepDlg::LoadProc(int &nNextIdx)
{
	enum ProcessorClassList
{
    PointProc,
    DepSourceProc,
    DepSinkProc,
    LineProc,
    BaggageProc,
    HoldAreaProc,
    GateProc,
    FloorChangeProc,
    StationProc,
    BarrierProc,
    ContingentProc
};

	ProcessorList *pProcList = GetInputTerminal()->procList;
	int pointProcCount = pProcList->getProcessorsOfType(PointProc);
	int depSourceProcCount = pProcList->getProcessorsOfType(DepSourceProc);
	int depSinkProcCount = pProcList->getProcessorsOfType(DepSinkProc);
	int lineProcCount = pProcList->getProcessorsOfType(LineProc);
	int baggageProcCount = pProcList->getProcessorsOfType(BaggageProc);
	int holdAreaProcCount = pProcList->getProcessorsOfType(HoldAreaProc);
	int gateProcCount = pProcList->getProcessorsOfType(GateProc);
	int floorChangeProcCount = pProcList->getProcessorsOfType(FloorChangeProc);
	int stationProcCount = pProcList->getProcessorsOfType(StationProc);
	int barrierProcCount = pProcList->getProcessorsOfType(BarrierProc);
	int contigentProcCount = pProcList->getProcessorsOfType(ContingentProc);
	int procCounnt = pointProcCount + depSourceProcCount + depSinkProcCount + lineProcCount + baggageProcCount + holdAreaProcCount + gateProcCount + floorChangeProcCount + stationProcCount + barrierProcCount + contigentProcCount;

	CString csTitle = GetLevelString( "Processors", 0 );
	int nIdx = m_listRep.InsertItem( nNextIdx, csTitle );
	m_listRep.SetItemData( nIdx, 0 );
	csTitle = GetLevelString(procCounnt, 0);
	m_listRep.SetItemText(nNextIdx, 1, csTitle);
	nNextIdx++;

	csTitle = GetLevelString( "Point Processor", 1 );
	nIdx = m_listRep.InsertItem( nNextIdx, csTitle );
	m_listRep.SetItemData( nIdx, 0 );
	csTitle = GetLevelString(pointProcCount, 1);
	m_listRep.SetItemText(nNextIdx, 1, csTitle);
	nNextIdx++;

	csTitle = GetLevelString( "Dependent Source", 1 );
	nIdx = m_listRep.InsertItem( nNextIdx, csTitle );
	m_listRep.SetItemData( nIdx, 0 );
	csTitle = GetLevelString(depSourceProcCount, 1);
	m_listRep.SetItemText(nNextIdx, 1, csTitle);
	nNextIdx++;

	csTitle = GetLevelString( "Dependent Sink", 1 );
	nIdx = m_listRep.InsertItem( nNextIdx, csTitle );
	m_listRep.SetItemData( nIdx, 0 );
	csTitle = GetLevelString(depSinkProcCount, 1);
	m_listRep.SetItemText(nNextIdx, 1, csTitle);
	nNextIdx++;

	csTitle = GetLevelString( "Line Processor", 1 );
	nIdx = m_listRep.InsertItem( nNextIdx, csTitle );
	m_listRep.SetItemData( nIdx, 0 );
	csTitle = GetLevelString(lineProcCount, 1);
	m_listRep.SetItemText(nNextIdx, 1, csTitle);
	nNextIdx++;

	csTitle = GetLevelString( "Baggage Device", 1 );
	nIdx = m_listRep.InsertItem( nNextIdx, csTitle );
	m_listRep.SetItemData( nIdx, 0 );
	csTitle = GetLevelString(baggageProcCount, 1);
	m_listRep.SetItemText(nNextIdx, 1, csTitle);
	nNextIdx++;

	csTitle = GetLevelString( "Holding Area Processor", 1 );
	nIdx = m_listRep.InsertItem( nNextIdx, csTitle );
	m_listRep.SetItemData( nIdx, 0 );
	csTitle = GetLevelString(holdAreaProcCount, 1);
	m_listRep.SetItemText(nNextIdx, 1, csTitle);
	nNextIdx++;

	csTitle = GetLevelString( "Gate Processor", 1 );
	nIdx = m_listRep.InsertItem( nNextIdx, csTitle );
	m_listRep.SetItemData( nIdx, 0 );
	csTitle = GetLevelString(gateProcCount, 1);
	m_listRep.SetItemText(nNextIdx, 1, csTitle);
	nNextIdx++;

	csTitle = GetLevelString( "Floor Change Processor", 1 );
	nIdx = m_listRep.InsertItem( nNextIdx, csTitle );
	m_listRep.SetItemData( nIdx, 0 );
	csTitle = GetLevelString(floorChangeProcCount, 1);
	m_listRep.SetItemText(nNextIdx, 1, csTitle);
	nNextIdx++;

	csTitle = GetLevelString( "People Mover Station", 1 );
	nIdx = m_listRep.InsertItem( nNextIdx, csTitle );
	m_listRep.SetItemData( nIdx, 0 );
	csTitle = GetLevelString(stationProcCount, 1);
	m_listRep.SetItemText(nNextIdx, 1, csTitle);
	nNextIdx++;

	csTitle = GetLevelString( "Barrier", 1 );
	nIdx = m_listRep.InsertItem( nNextIdx, csTitle );
	m_listRep.SetItemData( nIdx, 0 );
	csTitle = GetLevelString(barrierProcCount, 1);
	m_listRep.SetItemText(nNextIdx, 1, csTitle);
	nNextIdx++;
}

void CInputRepDlg::LoadPax(int &nNextIdx)
{
	///////////////////////////////////////////
	// Not decided how to solve the problem yet
	///////////////////////////////////////////

	CString csTitle = GetLevelString( "Passengers", 0 );
	int nIdx = m_listRep.InsertItem( nNextIdx, csTitle );
	m_listRep.SetItemData( nIdx, 0 );
	nNextIdx++;

	csTitle = GetLevelString( "Arrival", 1 );
	nIdx = m_listRep.InsertItem( nNextIdx, csTitle );
	m_listRep.SetItemData( nIdx, 0 );
	nNextIdx++;

	csTitle = GetLevelString( "Departure", 1 );
	nIdx = m_listRep.InsertItem( nNextIdx, csTitle );
	m_listRep.SetItemData( nIdx, 0 );
	nNextIdx++;

	csTitle = GetLevelString( "Transit", 1 );
	nIdx = m_listRep.InsertItem( nNextIdx, csTitle );
	m_listRep.SetItemData( nIdx, 0 );
	nNextIdx++;

	csTitle = GetLevelString( "Transfer", 1 );
	nIdx = m_listRep.InsertItem( nNextIdx, csTitle );
	m_listRep.SetItemData( nIdx, 0 );
	nNextIdx++;
}

void CInputRepDlg::LoadBaggage(int &nNextIdx)
{
	///////////////////////////////////////////
	// Not decided how to solve the problem yet
	///////////////////////////////////////////

	CString csTitle = GetLevelString( "Baggage", 0 );
	int nIdx = m_listRep.InsertItem( nNextIdx, csTitle );
	m_listRep.SetItemData( nIdx, 0 );
	nNextIdx++;

	csTitle = GetLevelString( "Arrival", 1 );
	nIdx = m_listRep.InsertItem( nNextIdx, csTitle );
	m_listRep.SetItemData( nIdx, 0 );
	nNextIdx++;

	csTitle = GetLevelString( "Departure", 1 );
	nIdx = m_listRep.InsertItem( nNextIdx, csTitle );
	m_listRep.SetItemData( nIdx, 0 );
	nNextIdx++;

}

void CInputRepDlg::LoadFlight(int &nNextIdx)
{
	FlightSchedule *pFlightDB = GetInputTerminal()->flightSchedule;
	int fltCount = pFlightDB->getCount();
	int arrFltCount = 0;
	int depFltCount = 0;
	int taFltCount = 0;
	for(int i = 0; i < fltCount; i++)
	{
		Flight *fltItem = pFlightDB->getItem(i);
		if(fltItem->isArriving() && !fltItem->isDeparting())
		{
			arrFltCount++;
		}
		else if(!fltItem->isArriving() && fltItem->isDeparting())
		{
			depFltCount++;
		}
		else
		{
			taFltCount++;
		}
	}
	CString csTitle = GetLevelString( "Flight", 0 );
	int nIdx = m_listRep.InsertItem( nNextIdx, csTitle );
	m_listRep.SetItemData( nIdx, 0 );
	csTitle = GetLevelString(fltCount, 0);
	nIdx = m_listRep.SetItemText(nNextIdx, 1, csTitle);
	nNextIdx++;

	csTitle = GetLevelString( "Arrival", 1 );
	nIdx = m_listRep.InsertItem( nNextIdx, csTitle );
	m_listRep.SetItemData( nIdx, 0 );
	csTitle = GetLevelString(arrFltCount, 1);
	nIdx = m_listRep.SetItemText(nNextIdx, 1, csTitle);
	nNextIdx++;

	csTitle = GetLevelString( "Departure", 1 );
	nIdx = m_listRep.InsertItem( nNextIdx, csTitle );
	m_listRep.SetItemData( nIdx, 0 );
	csTitle = GetLevelString(depFltCount, 1);
	nIdx = m_listRep.SetItemText(nNextIdx, 1, csTitle);
	nNextIdx++;

	csTitle = GetLevelString( "Turnaround", 1 );
	nIdx = m_listRep.InsertItem( nNextIdx, csTitle );
	m_listRep.SetItemData( nIdx, 0 );
	csTitle = GetLevelString(taFltCount, 1);
	nIdx = m_listRep.SetItemText(nNextIdx, 1, csTitle);
	nNextIdx++;
}

void CInputRepDlg::LoadLayout(int &nNextIdx)
{
	int nPortalCount = m_pDoc->GetTerminal().m_pPortals->m_vPortals.size();
	int nAreaCount = m_pDoc->GetTerminal().m_pAreas->m_vAreas.size();
	int nFloorCount = m_pDoc->GetCurModeFloor().m_vFloors.size();
	int nLayoutCount = nPortalCount + nAreaCount + nFloorCount;
	
	CString csTitle = GetLevelString( "Layout", 0 );
	int nIdx = m_listRep.InsertItem( nNextIdx, csTitle );
	m_listRep.SetItemData( nIdx, 0 );
	csTitle = GetLevelString(nLayoutCount, 0);
	nIdx = m_listRep.SetItemText(nNextIdx, 1, csTitle);
	nNextIdx++;

	csTitle = GetLevelString( "Floor", 1 );
	nIdx = m_listRep.InsertItem( nNextIdx, csTitle );
	m_listRep.SetItemData( nIdx, 0 );
	csTitle = GetLevelString(nFloorCount, 1);
	nIdx = m_listRep.SetItemText(nNextIdx, 1, csTitle);
	nNextIdx++;

	csTitle = GetLevelString( "Area", 1 );
	nIdx = m_listRep.InsertItem( nNextIdx, csTitle );
	m_listRep.SetItemData( nIdx, 0 );
	csTitle = GetLevelString(nAreaCount, 1);
	nIdx = m_listRep.SetItemText(nNextIdx, 1, csTitle);
	nNextIdx++;
		
	csTitle = GetLevelString( "Portal", 1 );
	nIdx = m_listRep.InsertItem( nNextIdx, csTitle );
	m_listRep.SetItemData( nIdx, 0 );
	csTitle = GetLevelString(nPortalCount, 1);
	nIdx = m_listRep.SetItemText(nNextIdx, 1, csTitle);
	nNextIdx++;

}


InputTerminal* CInputRepDlg::GetInputTerminal()
{
    return (InputTerminal*)&m_pDoc->GetTerminal();
}


BEGIN_MESSAGE_MAP(CInputRepDlg, CDialog)
	//{{AFX_MSG_MAP(CInputRepDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CInputRepDlg message handlers
