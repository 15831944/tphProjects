// EconRepDlg.cpp : implementation file
//

#include "stdafx.h"
#include "termplan.h"
#include "EconRepDlg.h"
#include "TermPlanDoc.h"
#include "inputs\schedule.h"
#include "results\paxlog.h"
#include "results\outpax.h"
#include "results\fltlog.h"
#include "common\ACTypesManager.h"
#include "common\template.h"
#include "inputs\assign.h"
#include "inputs\assigndb.h"
#include "common\states.h"
#include <Common/ProbabilityDistribution.h>

#include "economic\ProcEconomicDataElement.h"
#include "common\ProgressBar.h"
#include "results\proclog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEconRepDlg dialog


enum ProcessorStates { PROC_Open, PROC_Overtime, PROC_Closed };

// Description: Constructor.
// Exception:	FileOpenError, StringError, FileVersionTooNewError
CEconRepDlg::CEconRepDlg(CTermPlanDoc* _pDoc, CWnd* pParent /*=NULL*/)
	: CDialog(CEconRepDlg::IDD, pParent)
{
	m_pDoc = _pDoc;
	m_pTerm = &m_pDoc->GetTerminal();
	m_pTerm->openLogs(m_pDoc->m_ProjInfo.path);
	m_pBar = NULL;
	//{{AFX_DATA_INIT(CEconRepDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CEconRepDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEconRepDlg)
	DDX_Control(pDX, IDCANCEL, m_btnClose);
	DDX_Control(pDX, IDC_BUTTON_PRINT, m_btnPrint);
	DDX_Control(pDX, IDC_LIST_ECONREP, m_listRep);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEconRepDlg, CDialog)
	//{{AFX_MSG_MAP(CEconRepDlg)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEconRepDlg message handlers

void CEconRepDlg::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here

	CRect rcClient;
	if( !IsWindowVisible() )
		return;

	GetClientRect( &rcClient );
	rcClient.DeflateRect( 10, 10, 10, 40 );
	m_listRep.MoveWindow( rcClient );
	CRect rcBtn;
	rcBtn.top = rcClient.bottom + 10;
	rcBtn.bottom = rcBtn.top + 23;
	rcBtn.right = rcClient.right;
	rcBtn.left = rcBtn.right - 74;
	m_btnClose.MoveWindow( rcBtn );
	rcBtn.right = rcBtn.left - 20;
	rcBtn.left = rcBtn.right - 74;
	m_btnPrint.MoveWindow( rcBtn );
	
}


#define NUM_ECON_ITEM 43

BOOL CEconRepDlg::OnInitDialog() 
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



void CEconRepDlg::LoadDatabase()
{
	double dRevenue = 0.0;
	double dCost = 0.0;

	int nNextIdx = 0;

	CWaitCursor wc;
	
	m_pBar = new CProgressBar( "Loading Economic Report", 100, 22, TRUE );

	PreLoadAnnualFactor();
	m_pBar->StepIt();

	LoadRevenue( nNextIdx, dRevenue, 2 );

	LoadExpenses( nNextIdx, dCost, 1 );

	// 
	// insert row.

	CString csTitle = GetLevelString( "Revenue - Expenses ( exclusive of Waiting Costs )", 0 );
	int nIdx = m_listRep.InsertItem( nNextIdx, csTitle );
	m_listRep.SetItemData( nIdx, 0 );
	nNextIdx++;

	CString csVal = GetLevelString( dRevenue-dCost, 0 );
	m_listRep.SetItemText( nIdx, 1, csVal );

	double dWaitingCost = 0.0;
	LoadWaitingCost( 1, nNextIdx, dWaitingCost, 1 );
	m_pBar->StepIt();

	csTitle = GetLevelString( "Revenue - Expenses ( Inclusive of Waiting Costs )", 0 );
	nIdx = m_listRep.InsertItem( nNextIdx, csTitle );
	m_listRep.SetItemData( nIdx, 0 );
	nNextIdx++;

	csVal = GetLevelString( dRevenue-dCost-dWaitingCost, 0 );
	m_listRep.SetItemText( nIdx, 1, csVal );

	LoadAnnualFactor( nNextIdx, 0  );
	m_pBar->StepIt();


	LoadCapitalBudgetDecision( nNextIdx, dRevenue-dCost );
	m_pBar->StepIt();


	delete m_pBar;
	m_pBar = NULL;	
}


// load all the revunue data
// return total revenue,

void CEconRepDlg::LoadRevenue( int& _nNextAvailIdx, double& _dResVal, int _nType  )
{
	int nLevel = 0;
	_dResVal = 0.0;

	// insert row.
	int nIdx = m_listRep.InsertItem( _nNextAvailIdx, "Revenue" );
	m_listRep.SetItemData( nIdx, _nType );
	_nNextAvailIdx++;

	double dVal = 0;
	LoadFixedRevenue( nLevel+1, _nNextAvailIdx, dVal, _nType );

	_dResVal += dVal;

	dVal = 0;
	LoadVarRevenue( nLevel+1, _nNextAvailIdx, dVal, _nType );

	_dResVal += dVal;

	CString csVal;
		csVal.Format( "%.2f", _dResVal );

	m_listRep.SetItemText( nIdx, 1, csVal );
}


void CEconRepDlg::LoadFixedRevenue( int _nLevel, int& _nNextAvailIdx, double& _dVal, int _nType )
{
	_dVal = 0.0;

	double dVal = 0.0;

	CString csTitle = GetLevelString( "Fixed Revenues", _nLevel );
	int nIdx = m_listRep.InsertItem( _nNextAvailIdx, csTitle );
	m_listRep.SetItemData( nIdx, _nType );
	_nNextAvailIdx++;

	LoadLeases( _nLevel+1, _nNextAvailIdx, dVal, _nType );
	_dVal += dVal;
	m_pBar->StepIt();

	LoadAdvertising( _nLevel+1, _nNextAvailIdx, dVal, _nType );
	_dVal += dVal;
	m_pBar->StepIt();

	LoadContracts( _nLevel+1, _nNextAvailIdx, dVal, _nType );
	_dVal += dVal;
	m_pBar->StepIt();

	CString csVal = GetLevelString( _dVal, _nLevel );
	m_listRep.SetItemText( nIdx, 1, csVal );

}


void CEconRepDlg::LoadLeases( int _nLevel, int& _nNextAvailIdx, double& _dVal, int _nType )
{
	_dVal = 0.0;

	CString csTitle = GetLevelString( "Leases", _nLevel );
	int nIdx = m_listRep.InsertItem( _nNextAvailIdx, csTitle );
	m_listRep.SetItemData( nIdx, _nType );
	_nNextAvailIdx++;
	
	std::vector<FixedCostRevDataItem>& vData = GetEconomicManager()->m_pLeasesRevenue->GetDataList();
	int nCount = vData.size();
	for( int i=0; i<nCount; i++ )
	{
		CString csTitle = GetLevelString( vData[i].m_csLabel, _nLevel+1 );
		int nIdx = m_listRep.InsertItem( _nNextAvailIdx++, csTitle );
		m_listRep.SetItemData( nIdx, _nType );
		CString csVal = GetLevelString( vData[i].m_dVal, _nLevel+1 );
		m_listRep.SetItemText( nIdx, 1, csVal );
		_dVal += vData[i].m_dVal;
	}

	CString csVal = GetLevelString( _dVal, _nLevel );
	m_listRep.SetItemText( nIdx, 1, csVal );
}


void CEconRepDlg::LoadAdvertising( int _nLevel, int& _nNextAvailIdx, double& _dVal, int _nType )
{
	_dVal = 0.0;

	CString csTitle = GetLevelString( "Advertising", _nLevel );
	int nIdx = m_listRep.InsertItem( _nNextAvailIdx, csTitle );
	m_listRep.SetItemData( nIdx, _nType );
	_nNextAvailIdx++;
	
	
	std::vector<FixedCostRevDataItem>& vData = GetEconomicManager()->m_pAdvertisingRevenue->GetDataList();
	int nCount = vData.size();
	for( int i=0; i<nCount; i++ )
	{
		CString csTitle = GetLevelString( vData[i].m_csLabel, _nLevel+1 );
		int nIdx = m_listRep.InsertItem( _nNextAvailIdx++, csTitle );
		m_listRep.SetItemData( nIdx, _nType );
		CString csVal = GetLevelString( vData[i].m_dVal, _nLevel+1 );
		m_listRep.SetItemText( nIdx, 1, csVal );
		_dVal += vData[i].m_dVal;
	}

	CString csVal = GetLevelString( _dVal, _nLevel );
	m_listRep.SetItemText( nIdx, 1, csVal );
}

void CEconRepDlg::LoadContracts( int _nLevel, int& _nNextAvailIdx, double& _dVal, int _nType )
{
	_dVal = 0.0;

	CString csTitle = GetLevelString( "Contracts", _nLevel );
	int nIdx = m_listRep.InsertItem( _nNextAvailIdx, csTitle );
	m_listRep.SetItemData( nIdx, _nType );
	_nNextAvailIdx++;
	
	
	std::vector<FixedCostRevDataItem>& vData = GetEconomicManager()->m_pContractsRevenue->GetDataList();
	int nCount = vData.size();
	for( int i=0; i<nCount; i++ )
	{
		CString csTitle = GetLevelString( vData[i].m_csLabel, _nLevel+1 );
		int nIdx = m_listRep.InsertItem( _nNextAvailIdx++, csTitle );
		m_listRep.SetItemData( nIdx, _nType );
		CString csVal = GetLevelString( vData[i].m_dVal, _nLevel+1 );
		m_listRep.SetItemText( nIdx, 1, csVal );
		_dVal += vData[i].m_dVal;
	}

	CString csVal = GetLevelString( _dVal, _nLevel );
	m_listRep.SetItemText( nIdx, 1, csVal );
}

void CEconRepDlg::LoadVarRevenue( int _nLevel, int& _nNextAvailIdx, double& _dVal, int _nType )
{
	_dVal = 0.0;

	double dVal = 0.0;

	CString csTitle = GetLevelString( "Variable Revenues", _nLevel );
	int nIdx = m_listRep.InsertItem( _nNextAvailIdx, csTitle );
	m_listRep.SetItemData( nIdx, _nType );
	_nNextAvailIdx++;

	LoadPaxAirportFee( _nLevel+1, _nNextAvailIdx, dVal, _nType );
	_dVal += dVal;
	m_pBar->StepIt();

	LoadLandingFee( _nLevel+1, _nNextAvailIdx, dVal, _nType );
	_dVal += dVal;
	m_pBar->StepIt();

	LoadGateUsage( _nLevel+1, _nNextAvailIdx, dVal, _nType );
	_dVal += dVal;
	m_pBar->StepIt();

	LoadProcessorUsage( _nLevel+1, _nNextAvailIdx, dVal, _nType );
	_dVal += dVal;
	m_pBar->StepIt();

	LoadRetailPercent( _nLevel+1, _nNextAvailIdx, dVal, _nType );
	_dVal += dVal;
	m_pBar->StepIt();

	LoadParkingFee( _nLevel+1, _nNextAvailIdx, dVal, _nType );
	_dVal += dVal;
	m_pBar->StepIt();

	LoadLandsideTransFee( _nLevel+1, _nNextAvailIdx, dVal, _nType );
	_dVal += dVal;
	m_pBar->StepIt();

	CString csVal = GetLevelString( _dVal, _nLevel );
	m_listRep.SetItemText( nIdx, 1, csVal );
}


void CEconRepDlg::LoadPaxAirportFee( int _nLevel, int& _nNextAvailIdx, double& _dVal, int _nType )
{
	_dVal = 0.0;

	CString csTitle = GetLevelString( "Passenger Airport Fee", _nLevel );
	int nIdx = m_listRep.InsertItem( _nNextAvailIdx, csTitle );
	m_listRep.SetItemData( nIdx, _nType );
	_nNextAvailIdx++;	


	CPaxAirportFeeDatabase* pPaxAirportFeeDB = GetEconomicManager()->m_pPaxAirportFeeRevenue;
	
	PaxLog* pPaxLog = m_pTerm->paxLog;
	long nPaxCount = pPaxLog->getCount();
	OutputPaxEntry entry;
	entry.SetOutputTerminal( m_pTerm );
	entry.SetEventLog(m_pTerm->m_pMobEventLog);
	for( int i=0; i<nPaxCount; i++ )
	{
		pPaxLog->getItem( entry, i );

		// now check which pax type is match
		std::vector<PaxAirportFeeDataItem>& vList = pPaxAirportFeeDB->GetDataList();
		int nItemCount = vList.size();
		int nCritCount = -1;
		int nHitIndex = -1;
		for( int m=0; m<nItemCount; m++ )
		{
			PaxAirportFeeDataItem item = vList[m];
			if( entry.fits( *item.m_pPaxType ) )
			{
				int n = item.m_pPaxType->getCriteriaCount();
				if( n > nCritCount )
				{
					nCritCount = n;
					nHitIndex = m;
				}
			}
		}

		if( nHitIndex != -1 )
		{
			PaxAirportFeeDataItem hitItem = vList[nHitIndex];
			double dVal = 0;
			if(hitItem.m_dFeePerPax != 0)
			{
				dVal = hitItem.m_dFeePerPax;				
			}
			else 
			{
				ElapsedTime durTime = entry.getExitTime() - entry.getEntryTime();
				dVal = durTime.asHours() * hitItem.m_dFeePerHour;
			}
			_dVal += dVal;
		}
	}



	_dVal *= m_nAnnualFactor;
	CString csVal = GetLevelString( _dVal, _nLevel );
	m_listRep.SetItemText( nIdx, 1, csVal );
}

void CEconRepDlg::LoadLandingFee( int _nLevel, int& _nNextAvailIdx, double& _dVal, int _nType )
{
	_dVal = 0.0;

	CString csTitle = GetLevelString( "Landing Fee", _nLevel );
	int nIdx = m_listRep.InsertItem( _nNextAvailIdx, csTitle );
	m_listRep.SetItemData( nIdx, _nType );
	_nNextAvailIdx++;	

	CLandingFeeRevenueDatabase* pLandingFeeDB = GetEconomicManager()->m_pLandingFeesRevenue;
	
	FlightLog* pFltLog = m_pTerm->flightLog;
	long nFltCount = pFltLog->getCount();
	FlightLogEntry entry;
	entry.SetOutputTerminal(m_pTerm);
	entry.SetEventLog(m_pTerm->m_pFltEventLog);
	for( int i=0; i<nFltCount; i++ )
	{
		pFltLog->getItem( entry, i );
		
		FlightDescStruct flightStruct;
		entry.initStruct( flightStruct );

		FlightConstraint arrType, depType;
		arrType.initFlightType (flightStruct, 'A');
		depType.initFlightType (flightStruct, 'D');
		
		// now check which pax type is match
		std::vector<LandingFeeRevDataItem>& vList = pLandingFeeDB->GetDataList();
		int nItemCount = vList.size();
		int nCritCount = -1;
		int nHitIndex = -1;
		for( int m=0; m<nItemCount; m++ )
		{
			LandingFeeRevDataItem item = vList[m];

			if( ( entry.isArriving() && item.m_pFltType->fits (arrType) )
					|| ( entry.isDeparting() && item.m_pFltType->fits (depType) ) )
			{
				int n = item.m_pFltType->getCriteriaCount();
				if( n > nCritCount )
				{
					nCritCount = n;
					nHitIndex = m;
				}
			}
		}

		if( nHitIndex != -1 )
		{
			LandingFeeRevDataItem hitItem = vList[nHitIndex];
			double dVal = 0;
			if(hitItem.m_dFeePerLanding != 0)
			{
				dVal = hitItem.m_dFeePerLanding;
			}
			else
			{
				FlightDescStruct fltDesc;
				entry.initStruct(fltDesc);
				CACType item, *anAircraft;
				item.setIATACode(fltDesc.acType.GetValue());
				int ndx = _g_GetActiveACMan( GetInputTerminal()->m_pAirportDB)->findACTypeItem(&item);				
				if(ndx != INT_MAX)
				{
					anAircraft = _g_GetActiveACMan( GetInputTerminal()->m_pAirportDB )->getACTypeItem(ndx);
					dVal = hitItem.m_dFeePer1000MLW * anAircraft->m_fMLW / 1000;
				}
			}
			_dVal += dVal;
		}

	}

	_dVal *= m_nAnnualFactor;
	CString csVal = GetLevelString( _dVal, _nLevel );
	m_listRep.SetItemText( nIdx, 1, csVal );
}

void CEconRepDlg::LoadGateUsage( int _nLevel, int& _nNextAvailIdx, double& _dVal, int _nType )
{
	_dVal = 0.0;

	CString csTitle = GetLevelString( "Gate Usage", _nLevel );
	int nIdx = m_listRep.InsertItem( _nNextAvailIdx, csTitle );
	m_listRep.SetItemData( nIdx, _nType );
	_nNextAvailIdx++;
	
	ProcessorDatabase* pProcDB = GetEconomicManager()->m_pGateUsageRevenue;
	
	ProcessorList* pProcList = GetInputTerminal()->procList;
	int nProcCount = pProcList->getProcessorCount();
	for( int i=0; i<nProcCount; i++ )
	{
		Processor* pProc = pProcList->getProcessor( i );
		if( pProc->getProcessorType() != GateProc )
			continue;


		CProcEconomicDataElement* pEntry = (CProcEconomicDataElement*)pProcDB->getEntryPoint( *(pProc->getID()) );
		if( pEntry )
		{
			CGateUsageFeeData* pData = (CGateUsageFeeData*)pEntry->getData();
				
			double dVal = 0;
		
			FlightLog* pFltLog = m_pTerm->flightLog;
			long nFltCount = pFltLog->getCount();
			FlightLogEntry entry;
			entry.SetOutputTerminal(m_pTerm);
			entry.SetEventLog(m_pTerm->m_pFltEventLog);
			for( int i=0; i<nFltCount; i++ )
			{
				pFltLog->getItem( entry, i );
				if(entry.getGate() == pProc->getIndex())
				{
					if(pData->GetFeePerFlt() != 0)
					{
						dVal = pData->GetFeePerFlt() * m_nAnnualFactor;
						_dVal += dVal;
					}
					else if(pData->GetFeePerHour() != 0)
					{
						FlightDescStruct fltDesc;
						entry.initStruct(fltDesc);
						dVal = pData->GetFeePerHour() * fltDesc.gateOccupancy.asHours() * m_nAnnualFactor;
						_dVal += dVal;
						
					}
					else
					{
						FlightDescStruct fltDesc;
						entry.initStruct(fltDesc);
						CACType item, *anAircraft;
						item.setIATACode(fltDesc.acType.GetValue());
						int ndx = _g_GetActiveACMan( GetInputTerminal()->m_pAirportDB )->findACTypeItem(&item);				
						if(ndx != INT_MAX)
						{
							anAircraft = _g_GetActiveACMan( GetInputTerminal()->m_pAirportDB )->getACTypeItem(ndx);
							dVal = pData->GetFeePer100lb() * anAircraft->m_fMLW * m_nAnnualFactor / 1000 ;
							_dVal += dVal;
						}
					}
				}
			

			}
		}
	}
	CString csVal = GetLevelString( _dVal, _nLevel );
	m_listRep.SetItemText( nIdx, 1, csVal );
}



void CEconRepDlg::LoadProcessorUsage( int _nLevel, int& _nNextAvailIdx, double& _dVal, int _nType )
{
	_dVal = 0.0;
	//_passProc: Infor about Passing the Processors.
	struct _passProc 
	{
		int paxNum;
		int bagNum;
	};

	CString csTitle = GetLevelString( "Processor Usage", _nLevel );
	int nIdx = m_listRep.InsertItem( _nNextAvailIdx, csTitle );
	m_listRep.SetItemData( nIdx, _nType );
	_nNextAvailIdx++;
	
	ProcessorDatabase* pProcDB = GetEconomicManager()->m_pProcUsageRevenue;
	if( pProcDB->getCount() == 0 )
	{
		CString csVal = GetLevelString( _dVal, _nLevel );
		m_listRep.SetItemText( nIdx, 1, csVal );
		return;
	}

	//Get Processor Count
	ProcessorList* pProcList = GetInputTerminal()->procList;
	int nProcCount = pProcList->getProcessorCount();

	//Initialize the passProc's
	_passProc *passProc = new _passProc[nProcCount];
	for(int i = 0; i < nProcCount; i++)
	{
		passProc[i].paxNum = passProc[i].bagNum = 0;
	}

	//Get what is defined in the left tree
	bool bPaxFlag = false;
	bool bBagFlag = false;
	bool bFlightFlag = false;
	for(int i=0; i<nProcCount; i++ )
	{
		Processor* pProc = pProcList->getProcessor( i );
		if( pProc->getProcessorType() == BarrierProc )
			continue;

		CProcEconomicDataElement* pEntry = (CProcEconomicDataElement*)pProcDB->getEntryPoint( *(pProc->getID()) );
		if( pEntry )
		{
			CProcUsageFeeData* pData = (CProcUsageFeeData*)pEntry->getData();
			if(pData->GetFeePerDay() || pData->GetFeePerHour())
				continue;
			if(pData->GetFeePerFlt() != 0)
			{
				bFlightFlag = true;
				bPaxFlag = true;
				bBagFlag = true;
				break;
			}
			else if(pData->GetFeePerPax() != 0)
			{
				bPaxFlag = true;
			}
			else //fee per bag
			{
				if(pData->GetFeePerBag() != 0)
				{
					bBagFlag = true;
				}
			}
		}
	}

	//Initialize defining the Log & Entry variables
	PaxLog *paxLog = m_pTerm->paxLog;
	BagLog *bagLog = m_pTerm->bagLog;
	FlightLog *flightLog = m_pTerm->flightLog;
	ProcLog *procLog = m_pTerm->procLog;
	int nPaxCount = paxLog->getCount();
	int nBagCount = bagLog->getCount();
	int nFlightCount = flightLog->getCount();
	MobLogEntry paxEntry;
	BagLogEntry bagEntry;
	ProcLogEntry procEntry;
	paxEntry.SetOutputTerminal(m_pTerm);
	paxEntry.SetEventLog(m_pTerm->m_pMobEventLog);
	bagEntry.SetOutputTerminal(m_pTerm);
	bagEntry.SetEventLog(m_pTerm->m_pBagEventLog);
	procEntry.SetEventLog(m_pTerm->m_pProcEventLog);
	procEntry.SetOutputTerminal(m_pTerm);
	PaxEvent paxTrack;

	//progress bar
	CProgressBar secondBar( "Calculate Processors Usage", 100, nPaxCount+nBagCount+nProcCount, TRUE, 1 );

	//flightIndex[nProcCount][flightCount]
	int **flightIndex = new int*[nProcCount];
	for(int i = 0; i < nProcCount; i++)
	{
		flightIndex[i] = new int[nFlightCount];
		for(int j = 0; j < nFlightCount; j++)
		{
			flightIndex[i][j] = -1;
		}
	}

	//For every processor, count the passed pax count.
	if(bPaxFlag) {
	for(int i = 0; i < nPaxCount; i++)
	{
		paxLog->getItem(paxEntry, i);
		int nTrackCount = paxEntry.getCount();
		int formerProc = -1;
		for(int j = 0; j < nTrackCount; j++)
		{
			paxTrack.init(paxEntry.getEvent(j));
			int procIndex = paxTrack.getProc();
			if(procIndex >= nProcCount)
			{
				continue;
			}
			if(procIndex != formerProc)
			{
				passProc[procIndex].paxNum++;
				formerProc = procIndex;

				//flight index
				int tempIndex = paxEntry.getFlightIndex();
				if(bFlightFlag) {
				for(int k = 0; k < nFlightCount; k++)
				{
					//uses flight index to identify a certain flight.
					if(flightIndex[procIndex][k] == tempIndex)
						break;
					if(flightIndex[procIndex][k] != -1 && flightIndex[procIndex][k] != tempIndex)
						continue;
					if(flightIndex[procIndex][k] == -1)
					{
						flightIndex[procIndex][k] = tempIndex;
						break;
					}
				}
				}
			}
		}
		secondBar.StepIt();
	}
	}

	//For every processor, count the passed bag count.
	if(bBagFlag) {
	for(int i = 0; i < nBagCount; i++)
	{
		bagLog->getItem(bagEntry, i);
		int nTrackCount = bagEntry.getCount();
		int formerProc = -1;
		for(int j = 0; j < nTrackCount; j++)
		{
			int procIndex = bagEntry.getEvent(j).procNumber;
			if(procIndex >= nProcCount)
			{
				continue;
			}
			if(procIndex != formerProc)
			{
				passProc[procIndex].bagNum++;
				formerProc = procIndex;

				//flight index
				int tempIndex = paxEntry.getFlightIndex();
				if(bFlightFlag) {
				for(int k = 0; k < nFlightCount; k++)
				{

					//uses flight index to identify a certain flight.
					if(flightIndex[procIndex][k] == tempIndex)
						break;
					if(flightIndex[procIndex][k] != -1 && flightIndex[procIndex][k] != tempIndex)
						continue;
					if(flightIndex[procIndex][k] == -1)
					{
						flightIndex[procIndex][k] = tempIndex;
						break;
					}
				}

				}
			}
		}
		secondBar.StepIt();
	}
	}

	for(int i=0; i<nProcCount; i++ )
	{
		Processor* pProc = pProcList->getProcessor( i );
//		pProc->setAssignmentLink(*(GetInputTerminal()->procAssignDB));		
		if( pProc->getProcessorType() == BarrierProc )
			continue;

		CProcEconomicDataElement* pEntry = (CProcEconomicDataElement*)pProcDB->getEntryPoint( *(pProc->getID()) );
		if( pEntry )
		{
			CProcUsageFeeData* pData = (CProcUsageFeeData*)pEntry->getData();
				
			double dVal = 0;

			if(pData->GetFeePerDay() != 0)
			{
				dVal = pData->GetFeePerDay() * m_nAnnualFactor;
			}
			else if( pData->GetFeePerHour() != 0 )
			{

				///////////////////////////IMPORTANT//////////////////////////
				//Assume that the input and result infor are conrresponding.//
				//Namely there's no change in input since last simulation.	//
				///////////////////////////IMPORTANT//////////////////////////

				// base on procEventlog, calculate the total ScheduledTime and total Overtime
				ProcLogEntry logEntry; 
				logEntry.SetEventLog(m_pTerm->m_pProcEventLog);
				logEntry.SetOutputTerminal(m_pTerm);
				int procCount = m_pTerm->procLog->getCount();

				m_pTerm->procLog->getItem(logEntry, i);
				logEntry.setToFirst();
				long eventCount = logEntry.getCount();

				ElapsedTime openTime = 0l;
				ElapsedTime closeTime = -1l;
				ElapsedTime endTime = 24l * 60l * 60l;

				ElapsedTime dTime = 0l;
				ProcEventStruct event;
				int dState = 0;
				for(long j = 0; j < eventCount; j++)
				{
					event = logEntry.getNextEvent();
					if(event.type == CloseForService && dState == 0)
					{
						dTime += (float)(event.time/100l - openTime.asSeconds());
						dState = 1;
					}
					if(event.type == OpenForService && dState == 1)
					{
						openTime.setPrecisely(event.time);
						dState = 0;
					}
				}
				if(dState == 0)
				{
					dTime += endTime - openTime;
				}
				
				//caution: dTime/360000L; 3600000L is 1/100 second, not second.
				//error in ElapTime.cpp
				//by Calvin.
				dVal = pData->GetFeePerHour() * m_nAnnualFactor * ((long)dTime / 360000L);
			}
				
			else
			{
				if(pData->GetFeePerFlt() != 0)
				{
					int flightNum = 0;
					for(int j = 0; j < nFlightCount; j++)
					{
						//if the no flight found in the flight-index array
						//namely -1 is met.
						//then break;
						//else inc flightNum;
						if(flightIndex[i][j] == -1)
							break;
						flightNum++;
					}
					dVal = pData->GetFeePerFlt() * m_nAnnualFactor * flightNum;
				}
				else if(pData->GetFeePerPax() != 0)
				{
					dVal = pData->GetFeePerPax() * m_nAnnualFactor * passProc[i].paxNum;
				}
				else
				{
					dVal = pData->GetFeePerBag() * m_nAnnualFactor * passProc[i].bagNum;
				}
			}	
			_dVal += dVal;
		}
		secondBar.StepIt();
	}

	CString csVal = GetLevelString( _dVal, _nLevel );
	m_listRep.SetItemText( nIdx, 1, csVal );
	delete[] passProc;
	for(int i = 0; i < nProcCount; i++)
		delete[] flightIndex[i];
	delete[] flightIndex;
}


void CEconRepDlg::LoadRetailPercent( int _nLevel, int& _nNextAvailIdx, double& _dVal, int _nType )
{
	_dVal = 0.0;

	CString csTitle = GetLevelString( "Retail Percentage", _nLevel );
	int nIdx = m_listRep.InsertItem( _nNextAvailIdx, csTitle );
	m_listRep.SetItemData( nIdx, _nType );
	_nNextAvailIdx++;	


	CProcEconDatabase2* pRetailFeeDB = GetEconomicManager()->m_pRetailPercRevenue;
	if( pRetailFeeDB->getCount() == 0 )
	{
		CString csVal = GetLevelString( _dVal, _nLevel );
		m_listRep.SetItemText( nIdx, 1, csVal );
		return;
	}

	// for each paxlog
	PaxLog* pPaxLog = m_pTerm->paxLog;
	long nPaxCount = pPaxLog->getCount();
	OutputPaxEntry entry;
	entry.SetOutputTerminal( m_pTerm );
	entry.SetEventLog(m_pTerm->m_pMobEventLog);
	CProgressBar secondBar( "Calculate Retail Percent", 100, nPaxCount, TRUE, 1 );
	int nFormerProcIndex = -1;

	for( int i=0; i<nPaxCount; i++ )
	{
		secondBar.StepIt();
		pPaxLog->getItem( entry, i );

		// for each track
		int nTrackCount = entry.getCount();
		for( int m=1; m<nTrackCount; m++ )
		{
			// found the matched processor
			PaxEvent track;
			track.init( entry.getEvent(m) );
			int nProcIdx = track.getProc();

			if(nProcIdx == nFormerProcIndex)
				continue;
			else
				nFormerProcIndex = nProcIdx;

			Processor* pProc = GetInputTerminal()->procList->getProcessor( nProcIdx );
			const ProcessorID* pID = pProc->getID();
			CProcEconomicDataElement* pElem = (CProcEconomicDataElement*)pRetailFeeDB->FindEntry( *pID );
			if( pElem != NULL )
			{
				// check if the pax type is match one of 

				CRetailFeeData* pData = (CRetailFeeData*)pElem->getData();
				int nDataCount = pData->m_vList.size();
				for( int k=0; k<nDataCount; k++ )
				{
					if( entry.fits( *pData->m_vList[k].m_pPaxType ) )
					{
						// calculate the value.

						// prob buy
						if( rand() * 100 / RAND_MAX  < pData->m_vList[k].m_dProbBuy )
						{
							double dVal = pData->m_vList[k].m_pSaleDist->getRandomValue();
							dVal *= ( pData->m_vList[k].m_dAirportCut / 100.0 );
							_dVal += dVal;
						}
						break;
					}
				}
			}
		}
	}
	

	_dVal *= m_nAnnualFactor;
	CString csVal = GetLevelString( _dVal, _nLevel );
	m_listRep.SetItemText( nIdx, 1, csVal );

}


void CEconRepDlg::LoadParkingFee( int _nLevel, int& _nNextAvailIdx, double& _dVal, int _nType )
{
	_dVal = 0.0;

	CString csTitle = GetLevelString( "Parking Fees", _nLevel );
	int nIdx = m_listRep.InsertItem( _nNextAvailIdx, csTitle );
	m_listRep.SetItemData( nIdx, _nType );
	_nNextAvailIdx++;	


	CProcEconDatabase2* pParkingFeeDB = GetEconomicManager()->m_pParkingFeeRevenue;
	if( pParkingFeeDB->getCount() == 0 )
	{
		CString csVal = GetLevelString( _dVal, _nLevel );
		m_listRep.SetItemText( nIdx, 1, csVal );
		return;
	}

	// for each paxlog
	PaxLog* pPaxLog = m_pTerm->paxLog;
	long nPaxCount = pPaxLog->getCount();
	OutputPaxEntry entry;
	entry.SetOutputTerminal( &m_pDoc->GetTerminal() );
	entry.SetEventLog(m_pTerm->m_pMobEventLog);
	int nFormerProcIndex = -1;

	CProgressBar secondBar( "Calculate Parking Fee", 100, nPaxCount, TRUE, 1 );

	for( int i=0; i<nPaxCount; i++ )
	{
		secondBar.StepIt();
		pPaxLog->getItem( entry, i );

		// for each track
		int nTrackCount = entry.getCount();
		for( int m=1; m<nTrackCount; m++ )
		{
			// found the matched processor
			PaxEvent track;
			track.init( entry.getEvent(m) );
			int nProcIdx = track.getProc();

			if(nProcIdx == nFormerProcIndex)
				continue;
			else
				nFormerProcIndex = nProcIdx;

			Processor* pProc = GetInputTerminal()->procList->getProcessor( nProcIdx );
			const ProcessorID* pID = pProc->getID();
			CProcEconomicDataElement* pElem = (CProcEconomicDataElement*)pParkingFeeDB->FindEntry( *pID );
			if( pElem != NULL )
			{
				// check if the pax type is match one of 

				CParkingFeeData* pData = (CParkingFeeData*)pElem->getData();
				int nDataCount = pData->m_vList.size();
				for( int k=0; k<nDataCount; k++ )
				{
					if( entry.fits( *pData->m_vList[k].m_pPaxType ) )
					{
						// calculate the value.

						double dVal = pData->m_vList[k].m_pDurDist->getRandomValue();
						dVal *= pData->m_vList[k].m_dRatePerHour;
						_dVal += dVal;
						break;
					}
				}
			}
		}
	}


	CString csVal = GetLevelString( _dVal, _nLevel );
	m_listRep.SetItemText( nIdx, 1, csVal );
}


void CEconRepDlg::LoadLandsideTransFee( int _nLevel, int& _nNextAvailIdx, double& _dVal, int _nType )
{
	_dVal = 0.0;

	CString csTitle = GetLevelString( "Landside Transportation Fees", _nLevel );
	int nIdx = m_listRep.InsertItem( _nNextAvailIdx, csTitle );
	m_listRep.SetItemData( nIdx, _nType );
	_nNextAvailIdx++;	


	CProcEconDatabase2* pLandsideTransRevDB = GetEconomicManager()->m_pLandsideTransRevenue;
	int nItemCount = pLandsideTransRevDB->getCount();

	if( nItemCount == 0 )
	{
		CString csVal = GetLevelString( _dVal, _nLevel );
		m_listRep.SetItemText( nIdx, 1, csVal );
		return;
	}

	int* pPaxCount;
	pPaxCount = new int[nItemCount];
	for(int i = 0; i < nItemCount; i++)
	{
		pPaxCount[i] = 0;
	}

	// for each paxlog
	PaxLog* pPaxLog = m_pTerm->paxLog;
	long nPaxCnt = pPaxLog->getCount();
	OutputPaxEntry entry;
	entry.SetOutputTerminal(&m_pDoc->GetTerminal());
	entry.SetEventLog(m_pTerm->m_pMobEventLog);
	int nFormerProcIndex = -1;

	CProgressBar secondBar( "Calculate Landside Transportation Fee", 100, nPaxCnt, TRUE, 1 );

	for(int i=0; i<nPaxCnt; i++ )
	{
		secondBar.StepIt();

		pPaxLog->getItem( entry, i );

		// for each track
		int nTrackCount = entry.getCount();
		for( int m=1; m<nTrackCount; m++ )
		{
			// found the matched processor
			PaxEvent track;
			track.init( entry.getEvent(m) );
			int nProcIdx = track.getProc();

			if(nProcIdx == nFormerProcIndex)
				continue;
			else
				nFormerProcIndex = nProcIdx;

			Processor* pProc = GetInputTerminal()->procList->getProcessor( nProcIdx );
			const ProcessorID* pID = pProc->getID();

			int nMatchedIdx = pLandsideTransRevDB->findBestMatch( *pID );
			if( nMatchedIdx >= 0 )
				pPaxCount[nMatchedIdx]++;
		}
	}

	// now got the pax count list

	for( int i=0; i<nItemCount; i++ )
	{

		CProcEconomicDataElement* pElem = (CProcEconomicDataElement*)pLandsideTransRevDB->getItem( i );
		assert( pElem );

		CLandsdTransPtFeeData* pData = (CLandsdTransPtFeeData*)pElem->getData();
		int nDataCount = pData->m_vList.size();
		for( int k=0; k<nDataCount; k++ )
		{
			int nGroupSize = (int)pData->m_vList[k].m_pProbDist->getRandomValue();
			nGroupSize = nGroupSize >= 1 ? nGroupSize : 1;	// make sure group size > 1
			int nTrans = pPaxCount[i] / nGroupSize;

			double dVal = pData->m_vList[k].m_dTripFee * nTrans;		
			_dVal += dVal;
		}
	}
	delete [] pPaxCount;

	CString csVal = GetLevelString( _dVal, _nLevel );
	m_listRep.SetItemText( nIdx, 1, csVal );
}

void CEconRepDlg::LoadExpenses( int& _nNextAvailIdx, double& _dResVal, int _nType  )
{
	int nLevel = 0;
	_dResVal = 0.0;

	// insert row.
	int nIdx = m_listRep.InsertItem( _nNextAvailIdx, "Expenses" );
	m_listRep.SetItemData( nIdx, _nType );
	_nNextAvailIdx++;

	double dVal = 0;
	LoadFixedCosts( nLevel+1, _nNextAvailIdx, dVal, _nType );


	_dResVal += dVal;

	dVal = 0;
	LoadVarCosts( nLevel+1, _nNextAvailIdx, dVal, _nType );

	_dResVal += dVal;

	CString csVal;
		csVal.Format( "%.2f", _dResVal );

	m_listRep.SetItemText( nIdx, 1, csVal );
}


void CEconRepDlg::LoadFixedCosts( int _nLevel, int& _nNextAvailIdx, double& _dVal, int _nType )
{
	_dVal = 0.0;

	double dVal = 0.0;

	CString csTitle = GetLevelString( "Fixed Costs", _nLevel );
	int nIdx = m_listRep.InsertItem( _nNextAvailIdx, csTitle );
	m_listRep.SetItemData( nIdx, _nType );
	_nNextAvailIdx++;

	LoadBuildingDepre( _nLevel+1, _nNextAvailIdx, dVal, _nType );
	_dVal += dVal;
	m_pBar->StepIt();

	LoadInterests( _nLevel+1, _nNextAvailIdx, dVal, _nType );
	_dVal += dVal;
	m_pBar->StepIt();

	LoadInsurance( _nLevel+1, _nNextAvailIdx, dVal, _nType );
	_dVal += dVal;
	m_pBar->StepIt();

	LoadContracedServ( _nLevel+1, _nNextAvailIdx, dVal, _nType );
	_dVal += dVal;
	m_pBar->StepIt();

	LoadParkingLotsDepre( _nLevel+1, _nNextAvailIdx, dVal, _nType );
	_dVal += dVal;
	m_pBar->StepIt();

	LoadLandsideDepre( _nLevel+1, _nNextAvailIdx, dVal, _nType );
	_dVal += dVal;
	m_pBar->StepIt();

	AirsideDepre( _nLevel+1, _nNextAvailIdx, dVal, _nType );
	_dVal += dVal;
	m_pBar->StepIt();

	CString csVal = GetLevelString( _dVal, _nLevel );
	m_listRep.SetItemText( nIdx, 1, csVal );
}

void CEconRepDlg::LoadBuildingDepre( int _nLevel, int& _nNextAvailIdx, double& _dVal, int _nType )
{
	_dVal = 0.0;

	CString csTitle = GetLevelString( "Terminal Building Depreciation", _nLevel );
	int nIdx = m_listRep.InsertItem( _nNextAvailIdx, csTitle );
	m_listRep.SetItemData( nIdx, _nType );
	_nNextAvailIdx++;
	
	std::vector<FixedCostRevDataItem>& vData = GetEconomicManager()->m_pBuildingDeprcCost->GetDataList();
	int nCount = vData.size();
	for( int i=0; i<nCount; i++ )
	{
		CString csTitle = GetLevelString( vData[i].m_csLabel, _nLevel+1 );
		int nIdx = m_listRep.InsertItem( _nNextAvailIdx++, csTitle );
		m_listRep.SetItemData( nIdx, _nType );
		CString csVal = GetLevelString( vData[i].m_dVal, _nLevel+1 );
		m_listRep.SetItemText( nIdx, 1, csVal );
		_dVal += vData[i].m_dVal;
	}

	CString csVal = GetLevelString( _dVal, _nLevel );
	m_listRep.SetItemText( nIdx, 1, csVal );
}

void CEconRepDlg::LoadInterests( int _nLevel, int& _nNextAvailIdx, double& _dVal, int _nType )
{
	_dVal = 0.0;

	CString csTitle = GetLevelString( "Interests on Capital", _nLevel );
	int nIdx = m_listRep.InsertItem( _nNextAvailIdx, csTitle );
	m_listRep.SetItemData( nIdx, _nType );
	_nNextAvailIdx++;
	
	std::vector<InterestOnCapitalDataItem>& vData = GetEconomicManager()->m_pCaptIntrCost->GetDataList();
	int nCount = vData.size();
	for( int i=0; i<nCount; i++ )
	{
		CString csTitle = GetLevelString( vData[i].m_csSourceCapital, _nLevel+1 );
		int nIdx = m_listRep.InsertItem( _nNextAvailIdx++, csTitle );
		m_listRep.SetItemData( nIdx, _nType );
		CString csVal = GetLevelString( vData[i].m_dInterestCostPerYear, _nLevel+1 );
		m_listRep.SetItemText( nIdx, 1, csVal );
		_dVal += vData[i].m_dInterestCostPerYear;
	}

	CString csVal = GetLevelString( _dVal, _nLevel );
	m_listRep.SetItemText( nIdx, 1, csVal );
}

void CEconRepDlg::LoadInsurance( int _nLevel, int& _nNextAvailIdx, double& _dVal, int _nType )
{
	_dVal = 0.0;

	CString csTitle = GetLevelString( "Insurance", _nLevel );
	int nIdx = m_listRep.InsertItem( _nNextAvailIdx, csTitle );
	m_listRep.SetItemData( nIdx, _nType );
	_nNextAvailIdx++;
	
	std::vector<FixedCostRevDataItem>& vData = GetEconomicManager()->m_pInsuranceCost->GetDataList();
	int nCount = vData.size();
	for( int i=0; i<nCount; i++ )
	{
		CString csTitle = GetLevelString( vData[i].m_csLabel, _nLevel+1 );
		int nIdx = m_listRep.InsertItem( _nNextAvailIdx++, csTitle );
		m_listRep.SetItemData( nIdx, _nType );
		CString csVal = GetLevelString( vData[i].m_dVal, _nLevel+1 );
		m_listRep.SetItemText( nIdx, 1, csVal );
		_dVal += vData[i].m_dVal;
	}

	CString csVal = GetLevelString( _dVal, _nLevel );
	m_listRep.SetItemText( nIdx, 1, csVal );
}

void CEconRepDlg::LoadContracedServ( int _nLevel, int& _nNextAvailIdx, double& _dVal, int _nType )
{
	_dVal = 0.0;

	CString csTitle = GetLevelString( "Contracted Services", _nLevel );
	int nIdx = m_listRep.InsertItem( _nNextAvailIdx, csTitle );
	m_listRep.SetItemData( nIdx, _nType );
	_nNextAvailIdx++;
	
	std::vector<FixedCostRevDataItem>& vData = GetEconomicManager()->m_pContractsCost->GetDataList();
	int nCount = vData.size();
	for( int i=0; i<nCount; i++ )
	{
		CString csTitle = GetLevelString( vData[i].m_csLabel, _nLevel+1 );
		int nIdx = m_listRep.InsertItem( _nNextAvailIdx++, csTitle );
		m_listRep.SetItemData( nIdx, _nType );
		CString csVal = GetLevelString( vData[i].m_dVal, _nLevel+1 );
		m_listRep.SetItemText( nIdx, 1, csVal );
		_dVal += vData[i].m_dVal;
	}

	CString csVal = GetLevelString( _dVal, _nLevel );
	m_listRep.SetItemText( nIdx, 1, csVal );
}

void CEconRepDlg::LoadParkingLotsDepre( int _nLevel, int& _nNextAvailIdx, double& _dVal, int _nType )
{
	_dVal = 0.0;

	CString csTitle = GetLevelString( "Parking Lots Depreciation", _nLevel );
	int nIdx = m_listRep.InsertItem( _nNextAvailIdx, csTitle );
	m_listRep.SetItemData( nIdx, _nType );
	_nNextAvailIdx++;
	
	std::vector<FixedCostRevDataItem>& vData = GetEconomicManager()->m_pParkingLotsDeprecCost->GetDataList();
	int nCount = vData.size();
	for( int i=0; i<nCount; i++ )
	{
		CString csTitle = GetLevelString( vData[i].m_csLabel, _nLevel+1 );
		int nIdx = m_listRep.InsertItem( _nNextAvailIdx++, csTitle );
		m_listRep.SetItemData( nIdx, _nType );
		CString csVal = GetLevelString( vData[i].m_dVal, _nLevel+1 );
		m_listRep.SetItemText( nIdx, 1, csVal );
		_dVal += vData[i].m_dVal;
	}

	CString csVal = GetLevelString( _dVal, _nLevel );
	m_listRep.SetItemText( nIdx, 1, csVal );
}

void CEconRepDlg::LoadLandsideDepre( int _nLevel, int& _nNextAvailIdx, double& _dVal, int _nType )
{
	_dVal = 0.0;

	CString csTitle = GetLevelString( "Landside Facilities Depreciation", _nLevel );
	int nIdx = m_listRep.InsertItem( _nNextAvailIdx, csTitle );
	m_listRep.SetItemData( nIdx, _nType );
	_nNextAvailIdx++;
	
	std::vector<FixedCostRevDataItem>& vData = GetEconomicManager()->m_pLandsideFacilitiesCost->GetDataList();
	int nCount = vData.size();
	for( int i=0; i<nCount; i++ )
	{
		CString csTitle = GetLevelString( vData[i].m_csLabel, _nLevel+1 );
		int nIdx = m_listRep.InsertItem( _nNextAvailIdx++, csTitle );
		m_listRep.SetItemData( nIdx, _nType );
		CString csVal = GetLevelString( vData[i].m_dVal, _nLevel+1 );
		m_listRep.SetItemText( nIdx, 1, csVal );
		_dVal += vData[i].m_dVal;
	}

	CString csVal = GetLevelString( _dVal, _nLevel );
	m_listRep.SetItemText( nIdx, 1, csVal );
}

void CEconRepDlg::AirsideDepre( int _nLevel, int& _nNextAvailIdx, double& _dVal, int _nType )
{
	_dVal = 0.0;

	CString csTitle = GetLevelString( "Airside Facilities Depreciation", _nLevel );
	int nIdx = m_listRep.InsertItem( _nNextAvailIdx, csTitle );
	m_listRep.SetItemData( nIdx, _nType );
	_nNextAvailIdx++;
	
	std::vector<FixedCostRevDataItem>& vData = GetEconomicManager()->m_pAirsideFacilitiesCost->GetDataList();
	int nCount = vData.size();
	for( int i=0; i<nCount; i++ )
	{
		CString csTitle = GetLevelString( vData[i].m_csLabel, _nLevel+1 );
		int nIdx = m_listRep.InsertItem( _nNextAvailIdx++, csTitle );
		m_listRep.SetItemData( nIdx, _nType );
		CString csVal = GetLevelString( vData[i].m_dVal, _nLevel+1 );
		m_listRep.SetItemText( nIdx, 1, csVal );
		_dVal += vData[i].m_dVal;
	}

	CString csVal = GetLevelString( _dVal, _nLevel );
	m_listRep.SetItemText( nIdx, 1, csVal );
}

	
void CEconRepDlg::LoadVarCosts( int _nLevel, int& _nNextAvailIdx, double& _dVal, int _nType )
{
	_dVal = 0.0;

	double dVal = 0.0;

	CString csTitle = GetLevelString( "Fixed and Variable Costs", _nLevel );
	int nIdx = m_listRep.InsertItem( _nNextAvailIdx, csTitle );
	m_listRep.SetItemData( nIdx, _nType );
	_nNextAvailIdx++;

	LoadProcessorCost( _nLevel+1, _nNextAvailIdx, dVal, _nType );
	_dVal += dVal;
	m_pBar->StepIt();

	LoadPassengerCost( _nLevel+1, _nNextAvailIdx, dVal, _nType );
	_dVal += dVal;
	m_pBar->StepIt();

	LoadAircraftCost( _nLevel+1, _nNextAvailIdx, dVal, _nType );
	_dVal += dVal;
	m_pBar->StepIt();

	LoadUtilitiesCost( _nLevel+1, _nNextAvailIdx, dVal, _nType );
	_dVal += dVal;
	m_pBar->StepIt();

	LoadLaborCost( _nLevel+1, _nNextAvailIdx, dVal, _nType );
	_dVal += dVal;
	m_pBar->StepIt();

	LoadMaintenanceCost( _nLevel+1, _nNextAvailIdx, dVal, _nType );
	_dVal += dVal;
	m_pBar->StepIt();

	CString csVal = GetLevelString( _dVal, _nLevel );
	m_listRep.SetItemText( nIdx, 1, csVal );
}


void CEconRepDlg::LoadProcessorCost( int _nLevel, int& _nNextAvailIdx, double& _dVal, int _nType )
{
	_dVal = 0.0;

	CString csTitle = GetLevelString( "Processor Related", _nLevel );
	int nIdx = m_listRep.InsertItem( _nNextAvailIdx, csTitle );
	m_listRep.SetItemData( nIdx, _nType );
	_nNextAvailIdx++;
	

	ProcessorDatabase* pProcDB = GetEconomicManager()->m_pProcessorCost;
	
	ProcessorList* pProcList = GetInputTerminal()->procList;
	int nProcCount = pProcList->getProcessorCount();
	for( int i=0; i<nProcCount; i++ )
	{
		Processor* pProc = pProcList->getProcessor( i );
//		pProc->SetTerminal( GetInputTerminal() );
		if( pProc->getProcessorType() == BarrierProc )
			continue;

		CProcEconomicDataElement* pEntry = (CProcEconomicDataElement*)pProcDB->getEntryPoint( *(pProc->getID()) );
		if( pEntry )
		{
			CProcFixVarCostData* pData = (CProcFixVarCostData*)pEntry->getData();
				
			double dVal = pData->GetInstallCost()/pData->GetDeprPeriod();
			dVal += ( pData->GetOpCostPerHour() * 24 * m_nAnnualFactor );	// 24 hour for now.
				
			_dVal += dVal;
		}
	}

	CString csVal = GetLevelString( _dVal, _nLevel );
	m_listRep.SetItemText( nIdx, 1, csVal );
}


void CEconRepDlg::LoadPassengerCost( int _nLevel, int& _nNextAvailIdx, double& _dVal, int _nType )
{
	_dVal = 0.0;

	CString csTitle = GetLevelString( "Passenger Related", _nLevel );
	int nIdx = m_listRep.InsertItem( _nNextAvailIdx, csTitle );
	m_listRep.SetItemData( nIdx, _nType );
	_nNextAvailIdx++;
	
	CPassengerCostDatabase* pPaxCostDB = GetEconomicManager()->m_pPassengerCost;
	
	PaxLog* pPaxLog = m_pTerm->paxLog;
	long nPaxCount = pPaxLog->getCount();
	OutputPaxEntry entry;
	entry.SetOutputTerminal( m_pTerm );
	entry.SetEventLog(m_pTerm->m_pMobEventLog);
	for( int i=0; i<nPaxCount; i++ )
	{
		pPaxLog->getItem( entry, i );

		// now check which pax type is match
		std::vector<PassengerCostDataItem>& vList = pPaxCostDB->GetDataList();
		int nItemCount = vList.size();
		int nCritCount = -1;
		int nHitIndex = -1;
		for( int m=0; m<nItemCount; m++ )
		{
			PassengerCostDataItem item = vList[m];
			if( entry.fits( *item.m_pPaxType ) )
			{
				int n = item.m_pPaxType->getCriteriaCount();
				if( n > nCritCount )
				{
					nCritCount = n;
					nHitIndex = m;
				}
			}
		}
		if( nHitIndex != -1 )
		{
			PassengerCostDataItem hitItem = vList[nHitIndex];
			double dVal = 0;
			dVal = hitItem.m_dFixedPerPax;

			if(hitItem.m_dVarPerHour != 0)
			{
				ElapsedTime durTime = entry.getExitTime() - entry.getEntryTime();
				dVal = hitItem.m_dVarPerHour * durTime.asHours();
			}
			if(hitItem.m_dVarPerProc != 0)
			{
				long trackCount = entry.getCount();
				int procNum = 0;
				int formerProc = -1;
				for(long trackNum = 0; trackNum < trackCount ; trackNum++ )
				{
					PaxEvent track;
					track.init(entry.getEvent(trackNum));
					int procIndex = track.getProc();
					if(formerProc != procIndex)
					{
						formerProc = procIndex;
						procNum++;
					}
				}
				dVal = hitItem.m_dVarPerProc * procNum;
			}
			if(hitItem.m_dVarPerCart != 0)
			{
				int cartCout = entry.getCartCount();
				dVal = hitItem.m_dVarPerCart * cartCout;
			}
			if(hitItem.m_dVarPerBag != 0)
			{
				int bagCount = entry.getBagCount();
				dVal = hitItem.m_dVarPerBag * bagCount;
			}
			_dVal += dVal;
		}

	}
	_dVal *= m_nAnnualFactor;
	CString csVal = GetLevelString( _dVal, _nLevel );
	m_listRep.SetItemText( nIdx, 1, csVal );
}


void CEconRepDlg::LoadAircraftCost( int _nLevel, int& _nNextAvailIdx, double& _dVal, int _nType )
{
	_dVal = 0.0;

	CString csTitle = GetLevelString( "Aircraft Related", _nLevel );
	int nIdx = m_listRep.InsertItem( _nNextAvailIdx, csTitle );
	m_listRep.SetItemData( nIdx, _nType );
	_nNextAvailIdx++;
	
	CAircraftCostDatabase* pFltCostDB = GetEconomicManager()->m_pAircraftCost;
	
	FlightLog* pFltLog = m_pTerm->flightLog;
	long nFltCount = pFltLog->getCount();
	FlightLogEntry entry;
	entry.SetOutputTerminal(m_pTerm);
	for( int i=0; i<nFltCount; i++ )
	{
		pFltLog->getItem( entry, i );
		
		FlightDescStruct flightStruct;
		entry.initStruct( flightStruct );

		FlightConstraint arrType, depType;
		arrType.initFlightType (flightStruct, 'A');
		depType.initFlightType (flightStruct, 'D');
		
		// now check which pax type is match
		std::vector<AircraftCostDataItem>& vList = pFltCostDB->GetDataList();
		int nItemCount = vList.size();
		int nCritCount = -1;
		int nHitIndex = -1;
		for( int m=0; m<nItemCount; m++ )
		{
			AircraftCostDataItem item = vList[m];

			if( ( entry.isArriving() && item.m_pFltType->fits (arrType) )
					|| ( entry.isDeparting() && item.m_pFltType->fits (depType) ) )
			{
				int n = item.m_pFltType->getCriteriaCount();
				if( n > nCritCount )
				{
					nCritCount = n;
					nHitIndex = m;
				}
			}
		}

		if( nHitIndex != -1 )
		{
			AircraftCostDataItem hitItem = vList[nHitIndex];
			double dVal = hitItem.m_dServicePerAC;
			if(dVal == 0)
			{
				FlightDescStruct fltDesc;
				entry.initStruct(fltDesc);
				ElapsedTime durTime = fltDesc.gateOccupancy;
				dVal = durTime.asHours() * hitItem.m_dServicePerHour;
			}
			_dVal += dVal;
		}

	}


	_dVal *= m_nAnnualFactor;
	CString csVal = GetLevelString( _dVal, _nLevel );
	m_listRep.SetItemText( nIdx, 1, csVal );
}

void CEconRepDlg::LoadUtilitiesCost( int _nLevel, int& _nNextAvailIdx, double& _dVal, int _nType )
{
	_dVal = 0.0;

	CString csTitle = GetLevelString( "Utilities", _nLevel );
	int nIdx = m_listRep.InsertItem( _nNextAvailIdx, csTitle );
	m_listRep.SetItemData( nIdx, _nType );
	_nNextAvailIdx++;
	

	std::vector<UtilityCostDataItem>& vData = GetEconomicManager()->m_pUtilityCost->GetDataList();
	int nCount = vData.size();
	for( int i=0; i<nCount; i++ )
	{
		CString csTitle = GetLevelString( vData[i].m_csLabel, _nLevel+1 );
		int nIdx = m_listRep.InsertItem( _nNextAvailIdx++, csTitle );
		m_listRep.SetItemData( nIdx, _nType );
		double dVal = vData[i].m_dFixCostPerYear + 
			( 	vData[i].m_dLowCostPerHour * vData[i].m_nLowHoursPerDay +
				vData[i].m_dMedCostPerHour * vData[i].m_nMedHoursPerDay +
				vData[i].m_dHighCostPerHour * vData[i].m_nHighHoursPerDay ) * m_nAnnualFactor;
		CString csVal = GetLevelString( dVal, _nLevel+1 );
		m_listRep.SetItemText( nIdx, 1, csVal );
		_dVal += dVal;
	}

	CString csVal = GetLevelString( _dVal, _nLevel );
	m_listRep.SetItemText( nIdx, 1, csVal );
}


void CEconRepDlg::LoadLaborCost( int _nLevel, int& _nNextAvailIdx, double& _dVal, int _nType )
{
	_dVal = 0.0;
	double dBaseVal = 0.0;
	double dOTVal = 0.0;

	CString csTitle = GetLevelString( "Labour", _nLevel );
	int nIdx = m_listRep.InsertItem( _nNextAvailIdx, csTitle );
	m_listRep.SetItemData( nIdx, _nType );
	_nNextAvailIdx++;

	
	ProcessorDatabase* pProcDB = GetEconomicManager()->m_pLaborCost;


	// base on procEventlog, calculate the total ScheduledTime and total Overtime
	ProcLogEntry logEntry;
	logEntry.SetEventLog(m_pTerm->m_pProcEventLog);
	logEntry.SetOutputTerminal( m_pTerm );
	int procCount = m_pTerm->procLog->getCount();

	double dScheduledTime = 0.0;
	double dOverTime = 0.0;
	const ElapsedTime ENDTIME = 24l * 60l * 60l;


	for (int i = 0; i < procCount; i++)
	{
		m_pTerm->procLog->getItem (logEntry, i);
		logEntry.setToFirst();
		long eventCount = logEntry.getCount();
		ElapsedTime curTime = 0l;
		int state = PROC_Closed;
		ElapsedTime openTime = 0l;
		ElapsedTime overTimeStart = 0l;

		ProcessorID id;
		id.SetStrDict( m_pTerm->outStrDict );
		char szStr[256];
		logEntry.getID( szStr );
		id.setID( szStr );
		CProcEconomicDataElement* pEntry = (CProcEconomicDataElement*)pProcDB->getEntryPoint( id );
		if( pEntry )
		{
			CProcLabrFixVarCostData* pData = (CProcLabrFixVarCostData*)pEntry->getData();
				
			for (long i = 0; i < eventCount; i++)
			{
				ProcEventStruct event = logEntry.getNextEvent();
//				if( curTime == 0l )
				curTime.setPrecisely (event.time);

				if (curTime > ENDTIME)
					break;

				if (event.type == OpenForService && state == PROC_Open)
				{
					state = PROC_Closed;
					openTime = event.time/100l;
				}

				else if (event.type == StartToClose)
				{
					state = PROC_Overtime;
					overTimeStart = event.time/100l;
					dScheduledTime += ( overTimeStart - openTime ).asSeconds();
				}
				else if (event.type == CloseForService && state != PROC_Open)
				{
					if (state == PROC_Overtime)
						dOverTime += (event.time/100l - overTimeStart.asSeconds());
					else
						dScheduledTime += (event.time/100l - openTime.asSeconds());

					state = PROC_Open;
				}
			}	

			if (state == PROC_Overtime)
				dOverTime += ( ENDTIME - overTimeStart ).asSeconds();
			else if(state == PROC_Closed)
				dScheduledTime += ( ENDTIME - openTime ).asSeconds();


			// now base on the economic input calculate.
			if(pData->m_dBaseSalary != 0)
			{
				double dHeadCount = dScheduledTime / 3600.0 / (double)pData->m_nShiftPeriod + 0.5;
				int nHeadCount = (int)dHeadCount;
				dBaseVal += pData->m_dBaseSalary * nHeadCount;
			}

			else if(pData->m_dHourlyPay != 0)
			{
				dBaseVal += pData->m_dHourlyPay * m_nAnnualFactor * dScheduledTime / 3600.0;
			}

			if( pData->m_dOTPay != 0 )
			{
				dOverTime += pData->m_dOTPay * m_nAnnualFactor * dOverTime / 3600.0;
			}
		}
	}

	CString csVal = GetLevelString("Regular payment", _nLevel+1);
	int idx = m_listRep.InsertItem(_nNextAvailIdx, csVal);
	m_listRep.SetItemData(idx, _nType);
	csVal = GetLevelString(dBaseVal, _nLevel+1);
	m_listRep.SetItemText(_nNextAvailIdx, 1, csVal);
	_nNextAvailIdx++;

	csVal = GetLevelString("Overtime payment", _nLevel+1);
	idx = m_listRep.InsertItem(_nNextAvailIdx, csVal);
	m_listRep.SetItemData(idx, _nType);
	csVal = GetLevelString(dOTVal, _nLevel+1);
	m_listRep.SetItemText(_nNextAvailIdx, 1, csVal);
	_nNextAvailIdx++;

	_dVal = dBaseVal + dOTVal;
	csVal = GetLevelString( _dVal, _nLevel );
	m_listRep.SetItemText( nIdx, 1, csVal );
}



void CEconRepDlg::LoadMaintenanceCost( int _nLevel, int& _nNextAvailIdx, double& _dVal, int _nType )
{
	_dVal = 0.0;

	CString csTitle = GetLevelString( "Maintenance", _nLevel );
	int nIdx = m_listRep.InsertItem( _nNextAvailIdx, csTitle );
	m_listRep.SetItemData( nIdx, _nType );
	_nNextAvailIdx++;
	
	std::vector<MaintenanceCostDataItem>& vData = GetEconomicManager()->m_pMaintenanceCost->GetDataList();
	int nCount = vData.size();
	for( int i=0; i<nCount; i++ )
	{
		CString csTitle = GetLevelString( vData[i].m_csLabel, _nLevel+1 );
		int nIdx = m_listRep.InsertItem( _nNextAvailIdx++, csTitle );
		m_listRep.SetItemData( nIdx, _nType );

		double dVal = 0;
		if(vData[i].m_dCostPerLanding != 0)
		{
			FlightLog* pFltLog = m_pTerm->flightLog;
			long nFltCount = pFltLog->getCount();
			FlightLogEntry entry;
			entry.SetOutputTerminal(m_pTerm);
			entry.SetEventLog(m_pTerm->m_pFltEventLog);
			for( int fltNum=0; fltNum<nFltCount; fltNum++ )
			{
				pFltLog->getItem( entry, fltNum );
				if(!entry.isArriving())
					continue;
				dVal += vData[i].m_dCostPerLanding * m_nAnnualFactor;
			}

		}
		else if(vData[i].m_dCostPerYear != 0)
		{
			dVal = vData[i].m_dCostPerYear;
		}
		else
		{
			if(vData[i].m_dCostPerFlight != 0)
			{
				FlightLog* pFltLog = m_pTerm->flightLog;
				long nFltCount = pFltLog->getCount();
				dVal = vData[i].m_dCostPerFlight * nFltCount * m_nAnnualFactor;
			}
			else if(vData[i].m_dCostPerDay != 0)
			{
				dVal = m_nAnnualFactor * vData[i].m_dCostPerDay;
			}
		}

		CString csVal = GetLevelString( dVal, _nLevel+1 );
		m_listRep.SetItemText( nIdx, 1, csVal );
		_dVal += dVal;
	}


	CString csVal = GetLevelString( _dVal, _nLevel );
	m_listRep.SetItemText( nIdx, 1, csVal );
}


void CEconRepDlg::LoadWaitingCost( int _nLevel, int& _nNextAvailIdx, double& _dVal, int _nType  )
{
	_dVal = 0.0;

	CString csTitle = GetLevelString( "Waiting Cost", _nLevel );
	int nIdx = m_listRep.InsertItem( _nNextAvailIdx, csTitle );
	m_listRep.SetItemData( nIdx, _nType );
	_nNextAvailIdx++;
	
	CWaitingCostFactorDatabase* pWaitingCostFactorDB = GetEconomicManager()->m_pWaitingCostFactors;
	
	PaxLog* pPaxLog = ((Terminal*)GetInputTerminal())->paxLog;
	long nPaxCount = pPaxLog->getCount();
	for( int i=0; i<nPaxCount; i++ )
	{
		OutputPaxEntry entry;
		entry.SetOutputTerminal( (Terminal*)GetInputTerminal() );
		pPaxLog->getItem( entry, i );

		// now check which pax type is match
		std::vector<WaitingCostFactorDataItem>& vList = pWaitingCostFactorDB->GetDataList();
		int nItemCount = vList.size();
		int nCritCount = -1;
		int nHitIndex = -1;
		for( int m=0; m<nItemCount; m++ )
		{
			WaitingCostFactorDataItem item = vList[m];
			if( entry.fits( *item.m_pPaxType ) )
			{
				int n = item.m_pPaxType->getCriteriaCount();
				if( n > nCritCount )
				{
					nCritCount = n;
					nHitIndex = m;
				}
			}
		}

		if( nHitIndex != -1 )
		{
			WaitingCostFactorDataItem hitItem = vList[nHitIndex];
			double dVal = hitItem.m_dCostPerHour;
			_dVal += dVal;
		}
	}


	CString csVal = GetLevelString( _dVal, _nLevel );
	m_listRep.SetItemText( nIdx, 1, csVal );		
}


void CEconRepDlg::LoadAnnualFactor( int& _nNextAvailIdx, int _nType  )
{
	CString csTitle = GetLevelString( "Annualization Factor", 0 );
	int nIdx = m_listRep.InsertItem( _nNextAvailIdx, csTitle );
	m_listRep.SetItemData( nIdx, _nType );
	_nNextAvailIdx++;


	CString csVal;
	csVal.Format( "%d", m_nAnnualFactor );
	m_listRep.SetItemText( nIdx, 1, csVal );

}


void CEconRepDlg::LoadCapitalBudgetDecision( int& _nNextAvailIdx, double _dRevCost  )
{
	// skip one line
	int nIdx = m_listRep.InsertItem( _nNextAvailIdx, "" );
	m_listRep.SetItemData( nIdx, 0 );
	_nNextAvailIdx++;


	CCapitalBudgDecisionsDatabase* pCapitalBudgDecisionsDB = GetEconomicManager()->m_pCapitalBudgDecisions;

	// Life of Project
	int nProjLife = pCapitalBudgDecisionsDB->GetProjLife();
	CString csTitle = GetLevelString( "Life of Project(years)", 0 );
	nIdx = m_listRep.InsertItem( _nNextAvailIdx, csTitle );
	m_listRep.SetItemData( nIdx, 0 );
	_nNextAvailIdx++;

	CString csVal;
	csVal.Format( "%d", nProjLife );
	m_listRep.SetItemText( nIdx, 1, csVal );


	// Discount Rate (blended if applicable)
	csTitle = GetLevelString( "Discount Rate (blended if applicable)(%)", 0 );
	nIdx = m_listRep.InsertItem( _nNextAvailIdx, csTitle );
	m_listRep.SetItemData( nIdx, 0 );
	_nNextAvailIdx++;

	double dInterestCostPerYear = 0.0;
	double dPrincipalRem = 0.0;
	CInterestOnCapitalDatabase* pInterestOnCapitalDB = GetEconomicManager()->m_pCaptIntrCost;
	std::vector<InterestOnCapitalDataItem>& vData = pInterestOnCapitalDB->GetDataList();
	int nCount = vData.size();
	for( int i=0; i<nCount; i++ )
	{
		dInterestCostPerYear += vData[i].m_dInterestCostPerYear;
		dPrincipalRem += vData[i].m_dPrincipalRem;
	}

	double dDiscountRate = (double)pCapitalBudgDecisionsDB->GetDiscountRate();
	if( dDiscountRate < 0 && dPrincipalRem != 0 )
	{
		// Total Interest Cost/year / Total Principal Remaining 
		dDiscountRate = ( dInterestCostPerYear / dPrincipalRem ) * 100.0;

	}
	csVal.Format( "%.1f", dDiscountRate );
	m_listRep.SetItemText( nIdx, 1, csVal );


	// Payback Period
	csTitle = GetLevelString( "Payback Period(years)", 0 );
	nIdx = m_listRep.InsertItem( _nNextAvailIdx, csTitle );
	m_listRep.SetItemData( nIdx, 0 );
	_nNextAvailIdx++;

	double dPaybackPeriod = dPrincipalRem / _dRevCost + 0.5;
	int nPaybackPeriod = (int)dPaybackPeriod;
	csVal.Format( "%d", nPaybackPeriod );
	m_listRep.SetItemText( nIdx, 1, csVal );



	// Discounted Payback Period
	csTitle = GetLevelString( "Discounted Payback Period(years)", 0 );
	nIdx = m_listRep.InsertItem( _nNextAvailIdx, csTitle );
	m_listRep.SetItemData( nIdx, 0 );
	_nNextAvailIdx++;


	double dSum = 0.0;
	int nYears = 0;
	double dOnePlusK = 1.0;
	while( dSum < dPrincipalRem )
	{
		dSum += _dRevCost / dOnePlusK;
		if( dSum <= 0 )
			break;
		dOnePlusK *= ( 1.0 + dDiscountRate / 100.0 );
		nYears++;
	}

	csVal.Format( "%d", nYears );
	m_listRep.SetItemText( nIdx, 1, csVal );

	// Net Present Value (NRV)($)
	csTitle = GetLevelString( "Net Present Value (NRV)($)", 0 );
	nIdx = m_listRep.InsertItem( _nNextAvailIdx, csTitle );
	m_listRep.SetItemData( nIdx, 0 );
	_nNextAvailIdx++;


	double dNPV = CalculateNPV( _dRevCost, dDiscountRate, nProjLife );

	csVal.Format( "%.2f", dNPV );
	m_listRep.SetItemText( nIdx, 1, csVal );

	// Internal Rate of return (IRR)(%)
	csTitle = GetLevelString( "Internal Rate of return (IRR)(%)", 0 );
	nIdx = m_listRep.InsertItem( _nNextAvailIdx, csTitle );
	m_listRep.SetItemData( nIdx, 0 );
	_nNextAvailIdx++;

	double dIRR = CalculateIRR( dPrincipalRem, _dRevCost, nProjLife );
	csVal.Format( "%.2f", dIRR );
	m_listRep.SetItemText( nIdx, 1, csVal );

}


void CEconRepDlg::PreLoadAnnualFactor()
{
	m_nAnnualFactor = 365;
	enum ENUM_ANNUALACTIVITYDED enumType = GetEconomicManager()->m_pAnnualActivityDeduction->GetEnumType();
	int nPercent1;
	int nPercent2;
	switch( enumType )
	{
	case ENUM_EVERYDAY:
		break;
	case ENUM_WEEKDAY:
		nPercent1 = GetEconomicManager()->m_pAnnualActivityDeduction->GetPercent1();
		m_nAnnualFactor = 250 +  nPercent1 * 115 / 100;
		break;
	case ENUM_OCCASIONALY:
		nPercent1 = GetEconomicManager()->m_pAnnualActivityDeduction->GetPercent1();
		nPercent2 = GetEconomicManager()->m_pAnnualActivityDeduction->GetPercent2();
		m_nAnnualFactor = ( 250 * nPercent1  +  nPercent2 * 115 ) / 100;
		break;
	}

}

CEconomicManager* CEconRepDlg::GetEconomicManager()
{	
    return (CEconomicManager*)&m_pDoc->GetEconomicManager();
}



CString CEconRepDlg::GetLevelString( CString _str, int _nLevel )
{
	CString csTitle;
	for( int i=0; i<_nLevel; i++ )
	{
		csTitle += CString( "        " );
	}
	csTitle += _str;
	return csTitle;
}


CString CEconRepDlg::GetLevelString( double _dVal, int _nLevel )
{
	CString csVal;
	csVal.Format( "%.2f", _dVal );

	for( int i=0; i<_nLevel; i++ )
	{
		csVal += CString( "            " );
	}
	return csVal;
}


InputTerminal* CEconRepDlg::GetInputTerminal()
{
    return (InputTerminal*)&m_pDoc->GetTerminal();
}


// _dInitCost: +
// _dRevCost: +, 0, -
// _nProjLife: +, 0
double CEconRepDlg::CalculateIRR( double _dInitCost, double _dRevCost, int _nProjLife )
{
	int nIRR = 0;

	double dNPV = CalculateNPV( _dRevCost, nIRR, _nProjLife ) - _dInitCost;


	if( dNPV == 0 )
		return nIRR;
	
	if( dNPV > 0 )
	{
		while( nIRR < 1000 )
		{
			nIRR++;
			dNPV = CalculateNPV( _dRevCost, nIRR, _nProjLife ) - _dInitCost;
			if( dNPV == 0 )
			{
				// IRR make the NPV exact sati, record the current IRR
				break;
			}
			if( dNPV < 0 )
			{
				// IRR increase make NPV not sati. record the previouse IRR.
				nIRR--;
				break;
			}
		}
	}
	else
	{
		int iCircleCount =0;
		while( dNPV < 0 )
		{
			iCircleCount ++;
			nIRR--;
			dNPV = CalculateNPV( _dRevCost, nIRR, _nProjLife ) - _dInitCost;
			if( dNPV >= 0 )
			{
				//IRR decrease make NPV sati, record the current IRR
				break;
			}
			if(_nProjLife ==0 ) return 0.00000;
 		}
	}
	return nIRR;
}


// _dRevCost: + 0 -
// _dRate: +, 0, -
// _nProjList: +, 0

// return: +, 0, -
double CEconRepDlg::CalculateNPV( double _dRevCost, double _dRate, int _nProjLife )
{
	double dNPV = 0.0;
	double dOnePlusK = 1.0;
	for( int i=0; i<_nProjLife; i++ )
	{

		dNPV += _dRevCost / dOnePlusK;
		dOnePlusK *= ( 1.0 + _dRate / 100.0 );
	}

	return dNPV;
}
