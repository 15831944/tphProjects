// InputDataView.cpp : implementation file
//

#include "stdafx.h"
#include "InputDataView.h"
#include "TermPlanDoc.h"
#include "Floor2.h"
#include "inputs\schedule.h"
#include "inputs\hubbingdata.h"
#include "inputs\hubbingDatabase.h"
#include "inputs\SingleCarSchedule.h"
#include "inputs\AllCarSchedule.h"
#include "inputs\srvctime.h"
#include "inputs\paxdata.h"
#include "inputs\fltData.h"
#include "inputs\distlist.h"
#include "inputs\typelist.h"
#include "inputs\paxdist.h"
#include "inputs\assigndb.h"
#include "inputs\MobileElemTypeStrDB.h"


#include <vector>
using namespace std;
typedef std::vector<PaxTypePath> PaxTypePathVector;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static LPCTSTR TitleArray[] = {_T("Floors"),_T("Processors"),_T("Schedule"),_T("Delays"),_T("Load Factors"),\
_T("Aircraft Capacities"),_T("Boarding Calls"),_T("Hubbing Data"),_T("Distributions"),_T("Mobile Element Count"),_T("Group Size"),\
_T("Lead-Lag Distribution"),_T("In-Step Distribution"),_T("Side-Step Distribution"),_T("Speed"),_T("Visit Time"),\
_T("Service Time"),_T("Roster"),_T("Train Schedule"),_T("BaggageDeviceAssignment")};

/////////////////////////////////////////////////////////////////////////////
// CInputDataView

IMPLEMENT_DYNCREATE(CInputDataView, CFormView)

CInputDataView::CInputDataView()
	: CFormView(CInputDataView::IDD)
	,m_nIndex(0)
	,m_nUnique(1)
	,m_nVHeight(0)
{
	//{{AFX_DATA_INIT(CInputDataView)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_pDocument=NULL;
}

CInputDataView::~CInputDataView()
{
}

void CInputDataView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CInputDataView)
	DDX_Control(pDX, IDC_LIST_INPUTDATA_VISITTIME, m_listVisitTime);
	DDX_Control(pDX, IDC_LIST_INPUTDATA_TRAINSHEDULE, m_listTrainSchedule);
	DDX_Control(pDX, IDC_LIST_INPUTDATA_SPEED, m_listSpeed);
	DDX_Control(pDX, IDC_LIST_INPUTDATA_SIDESTEP, m_listSideStep);
	DDX_Control(pDX, IDC_LIST_INPUTDATA_SERVICETIME, m_listServiceTime);
	DDX_Control(pDX, IDC_LIST_INPUTDATA_ROSTER, m_listRoster);
	DDX_Control(pDX, IDC_LIST_INPUTDATA_PAXDISTRIBUTION, m_listPaxDistribution);
	DDX_Control(pDX, IDC_LIST_INPUTDATA_MOBILEELEMENTCOUNT, m_listMobileElemCount);
	DDX_Control(pDX, IDC_LIST_INPUTDATA_LOADFACTORS, m_listLoadFactors);
	DDX_Control(pDX, IDC_LIST_INPUTDATA_LEADLAG, m_listLeadLag);
	DDX_Control(pDX, IDC_LIST_INPUTDATA_INSTEP, m_listInStep);
	DDX_Control(pDX, IDC_LIST_INPUTDATA_HUBBINGDATA, m_listHubbingData);
	DDX_Control(pDX, IDC_LIST_INPUTDATA_GROUPSIZE, m_listGroupSize);
	DDX_Control(pDX, IDC_LIST_INPUTDATA_BOARDINGCALL, m_listBoardingCall);
	DDX_Control(pDX, IDC_LIST_INPUTDATA_AIRCAPACITY, m_listAirCapacity);
	DDX_Control(pDX, IDC_LIST_INPUTDATA_PROCS, m_listProcs);
	DDX_Control(pDX, IDC_LIST_INPUTDATA_FLTSHEDULE, m_listFltSchedule);
	DDX_Control(pDX, IDC_LIST_INPUTDATA_FLTDELAY, m_listFltDelay);
	DDX_Control(pDX, IDC_LIST_INPUTDATA_FLOORS, m_listFloors);
	DDX_Control(pDX, IDC_LIST_INPUTDATA_BaggageDevice,m_listBaggageDevice);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CInputDataView, CFormView)
	//{{AFX_MSG_MAP(CInputDataView)
	ON_WM_ERASEBKGND()
	ON_WM_CREATE()
	ON_COMMAND(ID_INPUTDATA_PRINT,PrintInputData)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CInputDataView diagnostics

#ifdef _DEBUG
void CInputDataView::AssertValid() const
{
	CFormView::AssertValid();
}

void CInputDataView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CInputDataView message handlers

int CInputDataView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CFormView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO: Add your specialized creation code here
	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		/*| CBRS_GRIPPER*/ | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_INPUT_DATA))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	return 0;
}

void CInputDataView::OnInitToolbar()
{
	CRect rect;
	GetClientRect(&rect);
	rect.bottom = 20;
	rect.right = 20;
	m_wndToolBar.MoveWindow(rect,TRUE);
	m_wndToolBar.ShowWindow(TRUE);
	m_wndToolBar.GetToolBarCtrl().EnableButton(ID_INPUTDATA_PRINT,TRUE);
}

void CInputDataView::LoadFloors()
{
	m_listFloors.DeleteAllItems();
	m_listFloors.InsertColumn(0,"Floor");
	m_listFloors.InsertColumn(1,"Name");
	
	m_listFloors.SetColumnWidth(0,300);
	m_listFloors.SetColumnWidth(1,300);
	
	int nCount=m_pDocument->GetCurModeFloor().m_vFloors.size();
	
	CString sLevel;
	CFloor2* pFloor=NULL;
	for(int i=0;i<nCount;i++)
	{
		pFloor=m_pDocument->GetCurModeFloor().m_vFloors[i];
		sLevel.Format("Floor %d",pFloor->Level()+1);
		m_listFloors.InsertItem(i,sLevel);
		m_listFloors.SetItemText(i,1,pFloor->FloorName());
	}
	m_vList.push_back(&m_listFloors);
}
//number of processors
void CInputDataView::LoadProcs()
{
	m_listProcs.DeleteAllItems();
	m_listProcs.InsertColumn(0,"Processors");
	m_listProcs.InsertColumn(1,"Number");

	m_listProcs.SetColumnWidth(0,300);
	m_listProcs.SetColumnWidth(1,300);
	
	const int nAryProcs[]=
	{
		PointProc,
			DepSourceProc,
			DepSinkProc,
			LineProc,
			BaggageProc,
			HoldAreaProc,
			GateProc,
			FloorChangeProc,

			BarrierProc,
			IntegratedStationProc,
			MovingSideWalkProc,
			Elevator,
			ConveyorProc,
			StairProc,
			EscalatorProc
	};
	LPCTSTR csProcsName[]=
	{
			_T("Point Processor"),
			_T("Dependent Source"),
			_T("Dependent Sink"),
			_T("Line Processor"),
			_T("Baggage Device"),
			_T("Holding Area"),
			_T("Gate"),
			_T("Floor Change Processor"),
			_T("Barrier"),
			_T("Integrated Station"),
			_T("Moving Side Walk"),
			_T("Elevator"),
			_T("Conveyor"),
			_T("Stair"),
			_T("Escalator")
			
	};
	
	ProcessorList *pProcList = m_pDocument->GetTerminal().procList;
	int nNum;
	
	int nSize=sizeof(nAryProcs)/sizeof(int);
	CString sNumber;
	int nSum=0;
	
	for(int i=0;i<nSize;i++)
	{
		m_listProcs.InsertItem(i,csProcsName[i]);
		nNum=pProcList->getProcessorsOfType(nAryProcs[i]);
		nSum+=nNum;
		sNumber.Format("%d",nNum);
		m_listProcs.SetItemText(i,1,sNumber);
	}
	m_listProcs.InsertItem(0,"Total");
	sNumber.Format("%d",nSum);
	m_listProcs.SetItemText(0,1,sNumber);
	m_vList.push_back(&m_listProcs);
}
//flight schedule
void CInputDataView::LoadFlightSchedule()
{
	m_listFltSchedule.DeleteAllItems();

	m_listFltSchedule.InsertColumn(0,"Flight");
	m_listFltSchedule.InsertColumn(1,"Number");

	m_listFltSchedule.SetColumnWidth(0,300);
	m_listFltSchedule.SetColumnWidth(1,300);

	BOOL bReturn=TRUE;
//	InputTerminal& inputTerminal=m_pDocument->GetTerminal();
	FlightSchedule& schedule=*m_pDocument->GetTerminal().flightSchedule;
	
	CString sValue("");
	
	int nCount=schedule.getCount();
	Flight* pFlight=NULL;
	
	int nArr=0;
	int nDep=0;
	int nTA=0;
	for(int i=0;i<nCount;i++)
	{
		pFlight=schedule.getItem(i);
		ASSERT(pFlight!=NULL);
		
		switch(pFlight->getFlightMode())
		{
		case 'T':
			nTA++;
			break;
		case 'A':
			nArr++;
			break;
		case 'D':
			nDep++;
			break;
		default:
			ASSERT(FALSE);
		}
	}
	
	//total
	m_listFltSchedule.InsertItem(0,"Total");
	sValue.Format("%d",nCount);
	m_listFltSchedule.SetItemText(0,1,sValue);
	//arrival
	m_listFltSchedule.InsertItem(1,"Arriving");
	sValue.Format("%d",nArr);
	m_listFltSchedule.SetItemText(1,1,sValue);
	//departure
	m_listFltSchedule.InsertItem(2,"Departing");
	sValue.Format("%d",nDep);
	m_listFltSchedule.SetItemText(2,1,sValue);
	//ture around
	m_listFltSchedule.InsertItem(3,"Turn Around");
	sValue.Format("%d",nTA);
	m_listFltSchedule.SetItemText(3,1,sValue);

	m_vList.push_back(&m_listFltSchedule);
}
void CInputDataView::LoadBaggageDeviceAssignment()
{
	m_listBaggageDevice.DeleteAllItems();
	
	m_listBaggageDevice.InsertColumn(0,"Flight");
	m_listBaggageDevice.InsertColumn(1,"ArrivalTime");
	m_listBaggageDevice.InsertColumn(2,"BaggageDevice");
	m_listBaggageDevice.SetColumnWidth(0,200);
	m_listBaggageDevice.SetColumnWidth(1,200);
	m_listBaggageDevice.SetColumnWidth(2,200);
	
	FlightSchedule& schedule=*m_pDocument->GetTerminal().flightSchedule;
	SortedContainer<Flight> sortSchedule;
	schedule.sortFlights(sortSchedule);
	CString sFlightStr("");
    char pcFlightStr[1024];
	int nCount=schedule.getCount();
	Flight* pFlight=NULL;
    int iListCount =0;
	for(int i=0;i<nCount;i++)
	{
		pFlight=sortSchedule.getItem(i);
		if(pFlight->getArrID().HasValue())
		{
			pFlight->getFlightIDString(pcFlightStr);
			m_listBaggageDevice.InsertItem(iListCount,pcFlightStr);
			m_listBaggageDevice.SetItemText(iListCount,1,pFlight->getArrTime().printTime());
			m_listBaggageDevice.SetItemText(iListCount,2,pFlight->getBaggageDevice().GetIDString());	
		    iListCount++;
		}
	}
	m_vList.push_back(&m_listBaggageDevice);
}
//Delays, Load Factors, Aircraft Capacities
void CInputDataView::LoadFlightDistribution()
{
	//Delays
	const FlightConstraintDatabase* pFlightConst=m_pDocument->GetTerminal().flightData->getDelays();
	ConstraintEntry* pEntry=NULL;
	int i;
	//TCHAR sFlightType[512]={'\0'};
	CString sFlightType;

	TCHAR sDistribution[512]={'\0'};
	
	int nCount=pFlightConst->getCount();
	//delays
	m_listFltDelay.DeleteAllItems();

	m_listFltDelay.InsertColumn(0,"Flight Type");
	m_listFltDelay.InsertColumn(1,"Probability Distribution (Minutes)");

	m_listFltDelay.SetColumnWidth(0,300);
	m_listFltDelay.SetColumnWidth(1,300);

	for(i=0;i<nCount;i++ )
	{
		sFlightType.Empty();
		pEntry=pFlightConst->getItem(i);
		ASSERT(pEntry!=NULL);
		pEntry->getConstraint()->screenPrint(sFlightType,0,512);
		pEntry->getValue()->screenPrint(sDistribution);
		m_listFltDelay.InsertItem(i,sFlightType);
		m_listFltDelay.SetItemText(i,1,sDistribution);
	}	
	m_vList.push_back(&m_listFltDelay);
	//Load Factors
	m_listLoadFactors.DeleteAllItems();

	m_listLoadFactors.InsertColumn(0,"Flight Type");
	m_listLoadFactors.InsertColumn(1,"Probability Distribution (Minutes)");

	m_listLoadFactors.SetColumnWidth(0,300);
	m_listLoadFactors.SetColumnWidth(1,300);
	
	pFlightConst=m_pDocument->GetTerminal().flightData->getLoads();
	nCount=pFlightConst->getCount();
	for(i=0;i<nCount;i++)
	{
		sFlightType.Empty();
		pEntry=pFlightConst->getItem(i);
		ASSERT(pEntry!=NULL);
		pEntry->getConstraint()->screenPrint(sFlightType,0,512);
		pEntry->getValue()->screenPrint(sDistribution);
		m_listLoadFactors.InsertItem(i,sFlightType);
		m_listLoadFactors.SetItemText(i,1,sDistribution);
	}
	m_vList.push_back(&m_listLoadFactors);
	//Aircraft Capacities
	m_listAirCapacity.DeleteAllItems();
	m_listAirCapacity.InsertColumn(0,"Flight Type");
	m_listAirCapacity.InsertColumn(1,"Probability Distribution (Minutes)");

	m_listAirCapacity.SetColumnWidth(0,300);
	m_listAirCapacity.SetColumnWidth(1,300);
	
	pFlightConst=m_pDocument->GetTerminal().flightData->getCapacity();
	nCount=pFlightConst->getCount();
	for(i=0;i<nCount;i++)
	{
		sFlightType.Empty();
		pEntry=pFlightConst->getItem(i);
		ASSERT(pEntry!=NULL);
		pEntry->getConstraint()->screenPrint(sFlightType,0,512);
		pEntry->getValue()->screenPrint(sDistribution);
		m_listAirCapacity.InsertItem(i,sFlightType);
		m_listAirCapacity.SetItemText(i,1,sDistribution);
	}
	m_vList.push_back(&m_listAirCapacity);
}
//boarding call
void CInputDataView::LoadBoardingCall()
{
	m_listBoardingCall.DeleteAllItems();

	m_listBoardingCall.InsertColumn(0,"Stage");
	m_listBoardingCall.InsertColumn(1,"Flight Type");
	m_listBoardingCall.InsertColumn(2,"Stand");
	m_listBoardingCall.InsertColumn(3,"Passenger Type");
	m_listBoardingCall.InsertColumn(4,"Trigger");
	m_listBoardingCall.InsertColumn(5,"Time range before STD(seconds)");
	m_listBoardingCall.InsertColumn(6,"Proportion of Pax(%)");

	m_listBoardingCall.SetColumnWidth(0,40);
	m_listBoardingCall.SetColumnWidth(1,120);
	m_listBoardingCall.SetColumnWidth(2,120);
	m_listBoardingCall.SetColumnWidth(3,120);
	m_listBoardingCall.SetColumnWidth(4,50);
	m_listBoardingCall.SetColumnWidth(5,180);
	m_listBoardingCall.SetColumnWidth(6,150);

	CString strStage;
	CString strFltType;
	CString strStand;
	CString strPaxType;
	CString strTrigger;
	CString strTime;
	CString strProp;
	BoardingCallFlightTypeDatabase* pStage = NULL;
	BoardingCallFlightTypeEntry* pFlightEntry = NULL;
	BoardingCallStandDatabase* pStandDB = NULL;
	BoardingCallStandEntry* pStandEntry = NULL;
	BoardingCallPaxTypeDatabase* pPaxDB = NULL;
	BoardingCallPaxTypeEntry* pPaxEntry = NULL;
	BoardingCallTrigger* pTrigger = NULL;
	ProbabilityDistribution* pTime = NULL;
	ProbabilityDistribution* pProp = NULL;
	int nListViewItem=0;
	FlightData* pFlightData=m_pDocument->GetTerminal().flightData;
	for(int iStage=0; iStage<pFlightData->GetStageCount(); iStage++)
	{
		strStage.Format("%d",iStage+1);
		pStage=pFlightData->GetFlightTypeDB(iStage);
		for(int iFlt=0; iFlt<pStage->getCount(); iFlt++)
		{
			pFlightEntry = (BoardingCallFlightTypeEntry*)pStage->getItem(iFlt);
			FlightConstraint* pFlightConst = (FlightConstraint*)pFlightEntry->getConstraint();
			strFltType.Empty();
			pFlightConst->screenPrint(strFltType);
			pStandDB = pFlightEntry->GetStandDatabase();
			for(int iStand=0; iStand<pStandDB->getCount(); iStand++)
			{
				pStandEntry = (BoardingCallStandEntry*)pStandDB->getItem(iStand);
				strStand.Empty();
				if(pStandEntry->getID()->isBlank())
				{
					strStand = _T("All Stand");
				}
				else
				{
					pStandEntry->getID()->printID(strStand.GetBuffer(256));
					strStand.ReleaseBuffer();
				}
				pPaxDB = (BoardingCallPaxTypeDatabase*)pStandEntry->GetPaxTypeDatabase();
				for(int iPax=0; iPax<pPaxDB->getCount(); iPax++)
				{
					pPaxEntry = (BoardingCallPaxTypeEntry*)pPaxDB->getItem(iPax);
					CMobileElemConstraint* pMobElemConst = (CMobileElemConstraint*)pPaxEntry->getConstraint();
					strPaxType.Empty();
					pMobElemConst->screenPrint(strPaxType);
					std::vector<BoardingCallTrigger*>& vTrigger = pPaxEntry->GetTriggersDatabase();
					for(int iTrigger=0; iTrigger<vTrigger.size(); iTrigger++)
					{
						pTrigger = vTrigger[iTrigger];
						pTime = pTrigger->m_time;
						pProp = pTrigger->m_prop;
						strTrigger.Format("%d", iTrigger+1);
						strTime = pTime->screenPrint();
						if(iTrigger == vTrigger.size()-1)
						{
							strProp = _T("Residual");
						}
						else
						{
							strProp = pProp->screenPrint();
						}
						m_listBoardingCall.InsertItem(nListViewItem, strStage);
						m_listBoardingCall.SetItemText(nListViewItem,1,strFltType);
						m_listBoardingCall.SetItemText(nListViewItem,2,strStand);
						m_listBoardingCall.SetItemText(nListViewItem,3,strPaxType);
						m_listBoardingCall.SetItemText(nListViewItem,4,strTrigger);
						m_listBoardingCall.SetItemText(nListViewItem,5,strTime);
						m_listBoardingCall.SetItemText(nListViewItem,6,strProp);
						nListViewItem++;
					}
				}
			}
		}
	}
	m_vList.push_back(&m_listBoardingCall);
}
//hubbing data
void CInputDataView::LoadHubbingData()
{
	m_listHubbingData.DeleteAllItems();

	m_listHubbingData.InsertColumn(0,"From");
	m_listHubbingData.InsertColumn(1,"Transit Percent");
	m_listHubbingData.InsertColumn(2,"Transfer Percent");
	m_listHubbingData.InsertColumn(3,"Minimum Transfer Time");
	m_listHubbingData.InsertColumn(4,"Transfer Time Window");
	m_listHubbingData.InsertColumn(5,"Transfer Destination");
	
	m_listHubbingData.SetColumnWidth(0,150);
	m_listHubbingData.SetColumnWidth(1,100);
	m_listHubbingData.SetColumnWidth(2,100);
	m_listHubbingData.SetColumnWidth(3,100);
	m_listHubbingData.SetColumnWidth(4,100);
	m_listHubbingData.SetColumnWidth(5,200);
	
	CString sConst;//[512]={'\0'};
	char sTransitPercent[1024]={'\0'};
	char sTransferPercent[1024]={'\0'};
	char sMinTransferTime[1024]={'\0'};
	char sTransferWindow[1024]={'\0'};
	char sTransferDest[1024]={'\0'};
	
	CHubbingData* pHubingData=NULL;
	CHubbingDatabase* pHubbing=m_pDocument->GetTerminal().m_pHubbing;
	int nCount=pHubbing->getCount();
	int nDestCount=0;
	int nPreProb=0;
	int nProb=0;
	int nSum=0;
	for(int i=0;i<nCount;i++)
	{
		pHubingData=pHubbing->getItem(i);
		pHubingData->GetConstraint().screenPrint(sConst,0,512);
		pHubingData->GetTransiting()->screenPrint(sTransitPercent);
		pHubingData->GetTransferring()->screenPrint(sTransferPercent);
		//pHubingData->GetMinTransferTime()->screenPrint(sMinTransferTime);
		//pHubingData->GetTransferWindow()->screenPrint(sTransferWindow);
		nDestCount=pHubingData->GetTransferDest()->getCount();
		nPreProb=0;
		
		for(int j=0;j<nDestCount;j++,nSum++)
		{
			pHubingData->GetTransferDest()->getGroup(j)->getFlightConstraint()->screenPrint(sTransferDest);
			nProb=(int)pHubingData->GetTransferDest()->getProb(j);
			sprintf(sTransferDest,"%s(%d%%)",sTransferDest,nProb-nPreProb);
			nPreProb=nProb;
			
			pHubingData->GetTransferDest()->getGroup(j)->getMinTransferTime()->screenPrint(sMinTransferTime);
			pHubingData->GetTransferDest()->getGroup(j)->getTransferWindow()->screenPrint(sTransferWindow);
			
			m_listHubbingData.InsertItem(nSum,sConst);
			m_listHubbingData.SetItemText(nSum,1,sTransitPercent);
			m_listHubbingData.SetItemText(nSum,2,sTransferPercent);
			m_listHubbingData.SetItemText(nSum,3,sMinTransferTime);
			m_listHubbingData.SetItemText(nSum,4,sTransferWindow);
			m_listHubbingData.SetItemText(nSum,5,sTransferDest);
		}
		
	}
	m_vList.push_back(&m_listHubbingData);
}
//Passengers - Distributions
void CInputDataView::LoadPaxDistribution()
{
	m_listPaxDistribution.DeleteAllItems();

	m_listPaxDistribution.InsertColumn(0,"Flights");
	m_listPaxDistribution.InsertColumn(1,"Passenger type");
	m_listPaxDistribution.InsertColumn(2,"Percent");
	
	m_listPaxDistribution.SetColumnWidth(0,100);
	m_listPaxDistribution.SetColumnWidth(1,300);
	m_listPaxDistribution.SetColumnWidth(2,50);
	
	PassengerTypeList* pInTypeList=m_pDocument->GetTerminal().inTypeList;
    PassengerDistributionList* pPaxDistList=m_pDocument->GetTerminal().paxDistList;
	
	CString sFlight;//[512]={'\0'};
	TCHAR sPaxType[512]={'\0'};
	PaxTypePath paxPath;
	PaxTypePathVector vAllPath;
	
	int nPerBranch=0;
	int nBranch=0;
	int nParentBranchs=0;
	int nPos=0;
	const int nLevelCount=pInTypeList->getLevels();
	
	ASSERT(nLevelCount<=LEVELS);
	//full permutation and combination
	int nLevel=0;
	for(; nLevel<nLevelCount;nLevel++)
	{
		nBranch=0;
		if(nLevel==0)
		{
			//Level 0
			for(int n=0;n<pInTypeList->getBranchesAtLevel(nLevel);n++)
			{
				paxPath[nLevel]=nBranch++;
				vAllPath.push_back(paxPath);
			}
		}
		else
		{
			//other Level
			nPos=vAllPath.size()-nParentBranchs;
			
			for(int nPerLevelBranch=0;nPerLevelBranch<nParentBranchs;nPerLevelBranch++)
			{
				nBranch=0;
				for(int nCurLevelBranch=0;nCurLevelBranch<pInTypeList->getBranchesAtLevel(nLevel);nCurLevelBranch++)
				{
					paxPath=vAllPath[nPos+nPerLevelBranch];
					paxPath[nLevel]=nBranch++;
					vAllPath.push_back(paxPath);
				}
			}
		}
		nParentBranchs=pInTypeList->getBranchesAtLevel(nLevel);
	}
	
	//begin insert data
	const PaxTypeDistribution* pPaxTypeDistribution=NULL;
	int nFlightsCount=pPaxDistList->getCount();
	int nPercent=0;
	TCHAR sBuf[512]={'\0'};

	int nSum=0;
	for(int nFlight=0;nFlight<nFlightsCount;nFlight++)
	{
		//Flights
		pPaxDistList->getConstraint(nFlight)->screenPrint(sFlight,0,512);
		for(int nPath=0;nPath<static_cast<int>(vAllPath.size());nPath++,nSum++)
		{
			//Percent
			pPaxTypeDistribution=(const PaxTypeDistribution*)pPaxDistList->getDist(nFlight);
			paxPath=vAllPath[nPath];
			nPercent=(int)pPaxTypeDistribution->getValue(paxPath.GetPath());
			//Level,Branch
			for(nLevel=LEVELS-1;nLevel>=0;nLevel--)
			{
				if(paxPath[nLevel]>=0)break;
			}
			//PaxType
			_tcscpy(sPaxType,_T(""));
			for(int n=nLevel;n>=0;n--)
			{
				_tcscpy(sBuf,sPaxType);
				sprintf(sPaxType,"\\%s%s",pInTypeList->getTypeName(paxPath[n],n),sBuf);
			}

			m_listPaxDistribution.InsertItem(nSum,sFlight);
			m_listPaxDistribution.SetItemText(nSum,1,sPaxType);
			
			sprintf(sBuf,"%d%%",nPercent);
			m_listPaxDistribution.SetItemText(nSum,2,sBuf);
		}
	}
	m_vList.push_back(&m_listPaxDistribution);
}
//Passengers - Mobile Element Count
void CInputDataView::LoadMobElementCount()
{
	m_listMobileElemCount.InsertColumn(0,"Mobile element");
	m_listMobileElemCount.InsertColumn(1,"Passenger Type");
	m_listMobileElemCount.InsertColumn(2,"Probability Distribution(count)");
	m_listMobileElemCount.InsertColumn(3,"Per Passenger");

	m_listMobileElemCount.SetColumnWidth(0,150);
	m_listMobileElemCount.SetColumnWidth(1,150);
	m_listMobileElemCount.SetColumnWidth(2,300);
	m_listMobileElemCount.SetColumnWidth(3,100);
	CMobileElemTypeStrDB* pMobileElemTypeStrDB=m_pDocument->GetTerminal().m_pMobElemTypeStrDB;
	int nElementTypeCount=pMobileElemTypeStrDB->GetCount();
	const PassengerData* pPaxDataList=m_pDocument->GetTerminal().paxDataList;
	const CMobileElemConstraintDatabase* pMobileElemConstraintDB=NULL;
	const ConstraintEntryEx* pEntryEx=NULL;
	CString sElementType("");
	int nEntryCount=0;


	//TCHAR sPaxType[512]={'\0'};
	TCHAR sProbDistribution[512]={'\0'};
	CString sPaxType;
	//CString sProbDistribution;
	CString sFlage("");
	int nSum=0;

	for(int nElementType=0;nElementType<nElementTypeCount;nElementType++)
	{
		sElementType = pMobileElemTypeStrDB->GetString(nElementType);//mobile element type

		pMobileElemConstraintDB=pPaxDataList->getNopaxData(nElementType);
		nEntryCount=pMobileElemConstraintDB->getCount();
		for(int nEntry=0;nEntry<nEntryCount ;nEntry++, nSum++)
		{
			pEntryEx=(ConstraintEntryEx* )pMobileElemConstraintDB->getItem(nEntry);
			pEntryEx->getConstraint()->screenPrint(sPaxType,0,512);
			pEntryEx->getValue()->screenPrint(sProbDistribution);
			sFlage.Format("%s",(pEntryEx->getFlag()?"TRUE":"FALSE"));

			m_listMobileElemCount.InsertItem(nSum,sElementType);
			m_listMobileElemCount.SetItemText(nSum,1,sPaxType);
			m_listMobileElemCount.SetItemText(nSum,2,sProbDistribution);
			m_listMobileElemCount.SetItemText(nSum,3,sFlage);
		}
	}
	m_vList.push_back(&m_listMobileElemCount);
}
//Group Size, In-Step Distribution, Side-Step Distribution, Speed, Visit Time
void CInputDataView::LoadMobElementDistribution()
{
	int i,nCount;
	//TCHAR sConst[512]={'\0'};
	TCHAR sDistribution[512]={'\0'};
	CString sConst;
	//CString sDistribution;
	ConstraintEntry* pEntry=NULL;
	
	//Group Size
	m_listGroupSize.DeleteAllItems();
	
	m_listGroupSize.InsertColumn(0,"Mobile element Type");
	m_listGroupSize.InsertColumn(1,"Probability Distribution (Count)");

	m_listGroupSize.SetColumnWidth(0,300);
	m_listGroupSize.SetColumnWidth(1,300);

	CMobileElemConstraintDatabase *pMobileElemConstDB =m_pDocument->GetTerminal().paxDataList->getPaxData(GroupSizeDatabase);
	nCount=pMobileElemConstDB->getCount();
	for(i=0;i<nCount;i++)
	{
		pEntry=pMobileElemConstDB->getItem(i);
		pEntry->getConstraint()->screenPrint(sConst,0,512);
		pEntry->getValue()->screenPrint(sDistribution);

		m_listGroupSize.InsertItem(i,sConst);
		m_listGroupSize.SetItemText(i,1,sDistribution);
		
	}
	m_vList.push_back(&m_listGroupSize);
	//In-Step Distribution
	m_listInStep.DeleteAllItems();
	m_listInStep.InsertColumn(0,"Mobile element Type");
	m_listInStep.InsertColumn(1,"Probability Distribution (Meter)");

	m_listInStep.SetColumnWidth(0,300);
	m_listInStep.SetColumnWidth(1,300);
	
	pMobileElemConstDB =m_pDocument->GetTerminal().paxDataList->getPaxData(InStepDatabase);
	nCount=pMobileElemConstDB->getCount();
	for(i=0;i<nCount;i++)
	{
		pEntry=pMobileElemConstDB->getItem(i);
		pEntry->getConstraint()->screenPrint(sConst,0,512);
		pEntry->getValue()->screenPrint(sDistribution);

		m_listInStep.InsertItem(i,sConst);
		m_listInStep.SetItemText(i,1,sDistribution);
	}
	m_vList.push_back(&m_listInStep);
	//Side-Step Distribution
	m_listSideStep.DeleteAllItems();
	m_listSideStep.InsertColumn(0,"Mobile element Type");
	m_listSideStep.InsertColumn(1,"Probability Distribution (Meter)");
	m_listSideStep.SetColumnWidth(0,300);
	m_listSideStep.SetColumnWidth(1,300);
	
	pMobileElemConstDB =m_pDocument->GetTerminal().paxDataList->getPaxData(SideStepDatabase);
	nCount=pMobileElemConstDB->getCount();
	for(i=0;i<nCount;i++)
	{
		pEntry=pMobileElemConstDB->getItem(i);
		pEntry->getConstraint()->screenPrint(sConst,0,512);
		pEntry->getValue()->screenPrint(sDistribution);

		m_listSideStep.InsertItem(i,sConst);
		m_listSideStep.SetItemText(i,1,sDistribution);
	}
	m_vList.push_back(&m_listSideStep);
	//Speed
	m_listSpeed.DeleteAllItems();
	m_listSpeed.InsertColumn(0,"Mobile element Type");
	m_listSpeed.InsertColumn(1,"Probability Distribution (Meter/Second)");
	m_listSpeed.SetColumnWidth(0,300);
	m_listSpeed.SetColumnWidth(1,300);
	
	pMobileElemConstDB =m_pDocument->GetTerminal().paxDataList->getPaxData(SpeedDatabase);
	nCount=pMobileElemConstDB->getCount();
	for(i=0;i<nCount;i++)
	{
		pEntry=pMobileElemConstDB->getItem(i);
		pEntry->getConstraint()->screenPrint(sConst,0,512);
		pEntry->getValue()->screenPrint(sDistribution);

		m_listSpeed.InsertItem(i,sConst);
		m_listSpeed.SetItemText(i,1,sDistribution);
	}
	m_vList.push_back(&m_listSpeed);
	//Visit Time
	m_listVisitTime.DeleteAllItems();
	m_listVisitTime.InsertColumn(0,"Mobile element Type");
	m_listVisitTime.InsertColumn(1,"Probability Distribution (Minutes)");
	m_listVisitTime.SetColumnWidth(0,300);
	m_listVisitTime.SetColumnWidth(1,300);
	
	pMobileElemConstDB =m_pDocument->GetTerminal().paxDataList->getPaxData(VisitTimeDatabase);
	nCount=pMobileElemConstDB->getCount();
	for(i=0;i<nCount;i++)
	{
		pEntry=pMobileElemConstDB->getItem(i);
		pEntry->getConstraint()->screenPrint(sConst,0,512);
		pEntry->getValue()->screenPrint(sDistribution);
		
		m_listVisitTime.InsertItem(i,sConst);
		m_listVisitTime.SetItemText(i,1,sDistribution);
	}
	m_vList.push_back(&m_listVisitTime);
}
//Lead-Lag Distribution
void CInputDataView::LoadLeadLag()
{
	m_listLeadLag.DeleteAllItems();

	m_listLeadLag.InsertColumn(0,"Mobile element Type");
	m_listLeadLag.InsertColumn(1,"Stand");
	m_listLeadLag.InsertColumn(2,"Probability Distribution (Minutes)");
	m_listLeadLag.SetColumnWidth(0,300);
	m_listLeadLag.SetColumnWidth(1,150);
	m_listLeadLag.SetColumnWidth(2,300);
	
	//TCHAR sMobileElemType[512]={'\0'};
	CString sProcID("");
	CString sProbDistribution;
	CString sMobileElemType;
	//CString sProbDistribution;

	CMobileConWithProcIDDatabase* pMobileConWithProcIDDatabase=m_pDocument->GetTerminal().paxDataList->getLeadLagTime();
	CMobileElemConstraint* pConst=NULL;
	ConstraintWithProcIDEntry* pEntry=NULL;
	int nCount=pMobileConWithProcIDDatabase->getCount();
	
	for(int nEntry=0;nEntry<nCount;nEntry++)
	{
		pEntry=(ConstraintWithProcIDEntry*)pMobileConWithProcIDDatabase->getItem(nEntry);
		pConst=(CMobileElemConstraint*)pEntry->getConstraint();
		pConst->screenPrint(sMobileElemType,0,512);
		if(pEntry->getProcID().isBlank())
		{
			sProcID="";
		}
		else
		{
			sProcID=pEntry->getProcID().GetIDString();
		}
		sProbDistribution = pEntry->getValue()->screenPrint();
		
		m_listLeadLag.InsertItem(nEntry,sMobileElemType);
		m_listLeadLag.SetItemText(nEntry,1,sProcID);
		m_listLeadLag.SetItemText(nEntry,2,sProbDistribution);
		
	}
	m_vList.push_back(&m_listLeadLag);
}
//////////////////////////Load Service time - start/////////////////////
//for Service Time
BOOL CInputDataView::IsSelectedServiceTime( CString strProcess, const ProcessorDatabase* _pProcDB ,int& nDBIndex)
{
	nDBIndex=-1;
	int nCount = _pProcDB->getCount();
	for( int i=0; i<nCount; i++ )
	{
		const ProcessorID* pProcID = _pProcDB->getProcName( i );
		char buf[512];
		pProcID->printID( buf );
		CString str(buf);
		if( strProcess == str )
		{
			nDBIndex=i;
			return TRUE;
		}
	}
	return FALSE;
}

// insert data into m_listServiceTime
void CInputDataView::LoadChildServiceTime(const ServiceTimeDatabase* pServiceTimeDatabase,const ProcessorList* pProcessorList,const CString& sParentProcs,int& nPosition)
{
	//TCHAR sPaxType[512]={'\0'};
	TCHAR sProbDistribution[2048]={'\0'};
	CString sPaxType;
//	CString sProbDistribution;
	CString sBufProc("");
	ProcessorID procID;
	StringList strListMembers;
	int nDBIndex=0;
	procID.SetStrDict(m_pDocument->GetTerminal().inStrDict);
	procID.setID(sParentProcs);

	ConstraintEntryEx* pEntryEx=NULL;
	CMobileElemConstraintDatabase *pMobileElemConstraintDatabase=NULL;
	int nMobCount;
	
	pProcessorList->getMemberNames(procID,strListMembers,-1);
	int nMemberCount=strListMembers.getCount();

	for(int nMember=0;nMember<nMemberCount;nMember++)
	{
		sBufProc=sParentProcs+ "-" +strListMembers.getString(nMember);
		if(IsSelectedServiceTime(sBufProc,pServiceTimeDatabase,nDBIndex) && nDBIndex!=-1)
		{
			pMobileElemConstraintDatabase = pServiceTimeDatabase->getDatabase(nDBIndex);
			
			nMobCount=pMobileElemConstraintDatabase->getCount();
			for(int j=0;j<nMobCount;j++, nPosition++)
			{
				pEntryEx=(ConstraintEntryEx*)pMobileElemConstraintDatabase->getItem(j);
				pEntryEx->getConstraint()->screenPrint(sPaxType,0,512);//PaxType
				pEntryEx->getValue()->screenPrint(sProbDistribution);//ProbDistribution
				
				m_listServiceTime.InsertItem(nPosition,sBufProc);
				m_listServiceTime.SetItemText(nPosition,1,sPaxType);
				m_listServiceTime.SetItemText(nPosition,2,sProbDistribution);
				m_listServiceTime.SetItemText(nPosition,3,pEntryEx->getBeginTime().printTime());
				m_listServiceTime.SetItemText(nPosition,4,pEntryEx->getEndTime().printTime());
				
			}
			
		}
		LoadChildServiceTime(pServiceTimeDatabase,pProcessorList,sBufProc,nPosition);
	}

}
//Processors - Service Times
void CInputDataView::LoadServiceTime()
{
	m_listServiceTime.DeleteAllItems();
	m_listServiceTime.InsertColumn(0,"Processor");
	m_listServiceTime.InsertColumn(1,"Passenger Type");
	m_listServiceTime.InsertColumn(2,"Probability Distribution(Seconds)");
	m_listServiceTime.InsertColumn(3,"Begin Time");
	m_listServiceTime.InsertColumn(4,"End Time");

	m_listServiceTime.SetColumnWidth(0,200);
	m_listServiceTime.SetColumnWidth(1,100);
	m_listServiceTime.SetColumnWidth(2,300);
	m_listServiceTime.SetColumnWidth(3,80);
	m_listServiceTime.SetColumnWidth(4,80);
	CString sProcID("");
	//TCHAR sPaxType[512]={'\0'};
	TCHAR sProbDistribution[2048]={'\0'};
	CString sPaxType;
	//CString sProbDistribution;
	
	ServiceTimeDatabase* pServiceTimeDatabase=m_pDocument->GetTerminal().serviceTimeDB;
	ConstraintEntryEx* pEntryEx=NULL;
	CMobileElemConstraintDatabase *pMobileElemConstraintDatabase=NULL;
	
	int nMobCount;
	int nCount=pServiceTimeDatabase->getCount();
	int nSum=0;
	int nDBIndex=0;

	ProcessorList* pProcessorList=m_pDocument->GetTerminal().procList;
	StringList strListProcs;
	StringList strListMembers;
	CString sBufProc("");

	pProcessorList->getAllGroupNames(strListProcs,-1);
	nCount=strListProcs.getCount();
	int nMemberCount=0;

	for(int nProcGroup=0;nProcGroup<nCount;nProcGroup++)
	{
		sProcID=strListProcs.getString(nProcGroup);
		if(IsSelectedServiceTime(sProcID,pServiceTimeDatabase,nDBIndex) && nDBIndex!=-1)
		{
			//insert
			pMobileElemConstraintDatabase = pServiceTimeDatabase->getDatabase(nDBIndex);
			
			nMobCount=pMobileElemConstraintDatabase->getCount();
			for(int j=0;j<nMobCount;j++,nSum++)
			{
				pEntryEx=(ConstraintEntryEx*)pMobileElemConstraintDatabase->getItem(j);
				pEntryEx->getConstraint()->screenPrint(sPaxType,0,512);//PaxType
				pEntryEx->getValue()->screenPrint(sProbDistribution);//ProbDistribution
				
				m_listServiceTime.InsertItem(nSum,sProcID);
				m_listServiceTime.SetItemText(nSum,1,sPaxType);
				m_listServiceTime.SetItemText(nSum,2,sProbDistribution);
				m_listServiceTime.SetItemText(nSum,3,pEntryEx->getBeginTime().printTime());
				m_listServiceTime.SetItemText(nSum,4,pEntryEx->getEndTime().printTime());
				
			}
				
		}
		LoadChildServiceTime(pServiceTimeDatabase,pProcessorList,sProcID,nSum);
	}
	m_vList.push_back(&m_listServiceTime);
}
//////////////////////////Load Service time - end/////////////////////

//////////////////////////Load Roster - start/////////////////////
BOOL CInputDataView::IsSelectedRoster(const CString& sProcID,const ProcAssignDatabase* pProcAssignDB,int& nDBIndex)
{
	nDBIndex=-1;
	UnmergedAssignments _unMergedAssgn;
	_unMergedAssgn.SetInputTerminal( (InputTerminal*)(&m_pDocument->GetTerminal()) );
	
	CString strTitle = pProcAssignDB->getTitle();
	if( strTitle == CString("Processor Assignment file") )		// is ProcAssignDatabase
	{
		int nCount = pProcAssignDB->getCount();
		for( int i=0; i<nCount; i++ )
		{
			const ProcessorID* pProcID = pProcAssignDB->getProcName(i);
			CString str = pProcID->GetIDString();
			if( str == sProcID )
			{
				_unMergedAssgn.clear();
				_unMergedAssgn.split( pProcAssignDB->getDatabase(i) );
				
				if( _unMergedAssgn.getCount()== 1 && _unMergedAssgn.getItem(0)->isDefault() )
					return FALSE;
				else
				{
					nDBIndex=i;
					return TRUE;
				}
			}	
		}
	}	
	return FALSE;
}
void CInputDataView::LoadChildRoster(const ProcAssignDatabase* pProcAssignDatabase,const ProcessorList* pProcsList,const CString& sParentProcs,int& nPosition)
{
	ProcessorID procID;
	StringList strListMembers;
	int nDBIndex=0;
	procID.SetStrDict(m_pDocument->GetTerminal().inStrDict);
	procID.setID(sParentProcs);
	UnmergedAssignments _unMergedAssgn;
	_unMergedAssgn.SetInputTerminal( (InputTerminal*)(&m_pDocument->GetTerminal()) );
	
	ConstraintEntryEx* pEntryEx=NULL;
	ProcessorRoster* pProAssign=NULL;
	CMobileElemConstraintDatabase *pMobileElemConstraintDatabase=NULL;
	int nMergedAssignCount;
	CString sEvent("");
	CString sBufProc("");
	TCHAR sCloseTime[512]={'\0'};
	TCHAR sOpenTime[512]={'\0'};
	//TCHAR sConst[512]={'\0'};
	CString sConst;
	
	pProcsList->getMemberNames(procID,strListMembers,-1);
	int nMemberCount=strListMembers.getCount();
	
	for(int nMember=0;nMember<nMemberCount;nMember++)
	{
		sBufProc=sParentProcs+ "-" +strListMembers.getString(nMember);
		if(IsSelectedRoster(sBufProc,pProcAssignDatabase,nDBIndex) && nDBIndex!=-1)
		{
				_unMergedAssgn.split( pProcAssignDatabase->getDatabase(nDBIndex));
				nMergedAssignCount=_unMergedAssgn.getCount();
				for(int nMergedAssign=0;nMergedAssign<nMergedAssignCount;nMergedAssign++ , nPosition++)
				{
					pProAssign=_unMergedAssgn.getItem(nMergedAssign);
					pProAssign->getAbsOpenTime().printTime(sOpenTime,0);
					pProAssign->getAbsCloseTime().printTime(sCloseTime,0);
					pProAssign->getAssignment()->getConstraint(0)->screenPrint(sConst);
					sEvent.Format("(%s - %s) %s",sOpenTime,sCloseTime,sConst);
					if(pProAssign->isOpen()==0)
					{
						sEvent+="<CLOSE>";
					}
					
					m_listRoster.InsertItem(nPosition,sBufProc);//processor
					m_listRoster.SetItemText(nPosition,1,sEvent);//event
				}
		}
		LoadChildRoster(pProcAssignDatabase,pProcsList,sBufProc,nPosition);
	}
}
//Processors - Roster
void CInputDataView::LoadRoster()
{
	m_listRoster.DeleteAllItems();

	m_listRoster.InsertColumn(0,"Processor");
	m_listRoster.InsertColumn(1,"Event");

	m_listRoster.SetColumnWidth(0,300);
	m_listRoster.SetColumnWidth(1,400);
	
	ProcessorList* pProcsList=m_pDocument->GetTerminal().procList;
	StringList strListGroupNames;
	CString sGroupName("");

	CString sEvent("");
	TCHAR sCloseTime[512]={'\0'};
	TCHAR sOpenTime[512]={'\0'};
	//TCHAR sConst[512]={'\0'};
	CString sConst;
	
	pProcsList->getAllGroupNames(strListGroupNames,-1);

	ProcAssignDatabase* pProcAssignDatabase=m_pDocument->GetTerminal().procAssignDB;
	int nProcAssignCount=pProcAssignDatabase->getCount();
	const ProcessorID* pProcessorID=NULL;
	UnmergedAssignments _unMergedAssgn;
	_unMergedAssgn.SetInputTerminal( (InputTerminal*)(&m_pDocument->GetTerminal()) );
	

	ProcessorRoster* pProAssign=NULL;
	int nMergedAssignCount=0;
	int nSum=0;
	int nDBIndex=0;
	
	for(int i=0;i<strListGroupNames.getCount();i++)
	{
		sGroupName=strListGroupNames.getString(i);
		if(IsSelectedRoster(sGroupName,pProcAssignDatabase,nDBIndex) && nDBIndex!=-1)
		{
				
			_unMergedAssgn.split(pProcAssignDatabase->getDatabase(nDBIndex));
			nMergedAssignCount=_unMergedAssgn.getCount();
			for(int nMergedAssign=0;nMergedAssign<nMergedAssignCount;nMergedAssign++ , nSum++)
			{
				pProAssign=_unMergedAssgn.getItem(nMergedAssign);
				pProAssign->getAbsOpenTime().printTime(sOpenTime,0);
				pProAssign->getAbsCloseTime().printTime(sCloseTime,0);
				pProAssign->getAssignment()->getConstraint(0)->screenPrint(sConst);
				sEvent.Format("(%s - %s) %s",sOpenTime,sCloseTime,sConst.GetBuffer(0));
				if(pProAssign->isOpen()==0)
				{
					sEvent+="<CLOSE>";
				}
				
				m_listRoster.InsertItem(nSum,sGroupName);//processor
				m_listRoster.SetItemText(nSum,1,sEvent);//event
			}
			
		}
		LoadChildRoster(pProcAssignDatabase,pProcsList,sGroupName,nSum);
	}
	m_vList.push_back(&m_listRoster);
}
//////////////////////////Load Roster - end/////////////////////

//Processors - Train Schedule
void CInputDataView::LoadTrainSchedule()
{
	m_listTrainSchedule.DeleteAllItems();
	m_listTrainSchedule.InsertColumn(0,"Schedule");
	m_listTrainSchedule.InsertColumn(1,"Start");
	m_listTrainSchedule.InsertColumn(2,"End");
	m_listTrainSchedule.InsertColumn(3,"Speed(m/s)");
	m_listTrainSchedule.InsertColumn(4,"Accelerate(m/s*s)");
	m_listTrainSchedule.InsertColumn(5,"Decelerate(m/s*s)");
	m_listTrainSchedule.InsertColumn(6,"Number of Train");
	m_listTrainSchedule.InsertColumn(7,"Turn Around(min)");
	m_listTrainSchedule.InsertColumn(8,"Headway (min)");

	m_listTrainSchedule.SetColumnWidth(0,100);
	m_listTrainSchedule.SetColumnWidth(1,80);
	m_listTrainSchedule.SetColumnWidth(2,80);
	m_listTrainSchedule.SetColumnWidth(3,80);
	m_listTrainSchedule.SetColumnWidth(4,80);
	m_listTrainSchedule.SetColumnWidth(5,80);
	m_listTrainSchedule.SetColumnWidth(6,80);
	m_listTrainSchedule.SetColumnWidth(7,80);
	m_listTrainSchedule.SetColumnWidth(8,80);

	const ALLCARSCHEDULE* pAllSchedule=m_pDocument->GetTerminal().m_pAllCarSchedule->GetAllCarSchedule();
	
	CString sValue("");
	TCHAR sStartTime[30];
	TCHAR sEndTime[30];
	int nSum=0;

	ALLCARSCHEDULE::const_iterator iter;
	for(iter=pAllSchedule->begin();iter!=pAllSchedule->end();iter++,nSum++)
	{
		(*iter)->GetCarStartTime().printTime(sStartTime);
		(*iter)->GetCarEndTime().printTime(sEndTime);

		m_listTrainSchedule.InsertItem(nSum,(*iter)->GetCarName());//shedule
		m_listTrainSchedule.SetItemText(nSum,1,sStartTime);//start
		m_listTrainSchedule.SetItemText(nSum,2,sEndTime);//end 
		
		sValue.Format("%d",(int)((*iter)->GetNormalSpeed() / SCALE_FACTOR)); //normal speed 
		m_listTrainSchedule.SetItemText(nSum,3,sValue);

		sValue.Format("%d",(int)((*iter)->GetAccelerateSpeed() / SCALE_FACTOR));//accelerate speed 
		m_listTrainSchedule.SetItemText(nSum,4,sValue);

		sValue.Format("%d",(int)((*iter)->GetDecelerateSpeed() / SCALE_FACTOR));//decelerate speed
		m_listTrainSchedule.SetItemText(nSum,5,sValue);

		sValue.Format("%d",(*iter)->GetTrainNumber()); //number of train
		m_listTrainSchedule.SetItemText(nSum,6,sValue);

		sValue.Format("%d",(*iter)->GetTurnAroundTime().asSeconds()); //turn around time
		m_listTrainSchedule.SetItemText(nSum,7,sValue);

		sValue.Format("%d",(*iter)->GetHeadWayTime().asSeconds() / 60); // headway
		m_listTrainSchedule.SetItemText(nSum,8,sValue);
		
	}
	m_vList.push_back(&m_listTrainSchedule);
}

BOOL CInputDataView::OnEraseBkgnd(CDC* pDC )
{
	BOOL bRet=CFormView::OnEraseBkgnd(pDC );
	CBrush brush;
	brush.CreateStockObject(WHITE_PEN);
	CRect rect;
	GetClientRect(&rect);
	pDC->FillRect(&rect,&brush);
	
	return TRUE;
}
void CInputDataView::OnInitialUpdate() 
{
	CFormView::OnInitialUpdate();
	
	m_listVisitTime.SetExtendedStyle(LVS_EX_FULLROWSELECT);
	m_listTrainSchedule.SetExtendedStyle(LVS_EX_FULLROWSELECT);
	m_listSpeed.SetExtendedStyle(LVS_EX_FULLROWSELECT);
	m_listSideStep.SetExtendedStyle(LVS_EX_FULLROWSELECT);
	m_listServiceTime.SetExtendedStyle(LVS_EX_FULLROWSELECT);
	m_listRoster.SetExtendedStyle(LVS_EX_FULLROWSELECT);
	m_listPaxDistribution.SetExtendedStyle(LVS_EX_FULLROWSELECT);
	m_listMobileElemCount.SetExtendedStyle(LVS_EX_FULLROWSELECT);
	m_listLoadFactors.SetExtendedStyle(LVS_EX_FULLROWSELECT);
	m_listLeadLag.SetExtendedStyle(LVS_EX_FULLROWSELECT);
	m_listInStep.SetExtendedStyle(LVS_EX_FULLROWSELECT);
	m_listHubbingData.SetExtendedStyle(LVS_EX_FULLROWSELECT);
	m_listGroupSize.SetExtendedStyle(LVS_EX_FULLROWSELECT);
	m_listBoardingCall.SetExtendedStyle(LVS_EX_FULLROWSELECT);
	m_listAirCapacity.SetExtendedStyle(LVS_EX_FULLROWSELECT);
	m_listProcs.SetExtendedStyle(LVS_EX_FULLROWSELECT);
	m_listFltSchedule.SetExtendedStyle(LVS_EX_FULLROWSELECT);
	m_listFltDelay.SetExtendedStyle(LVS_EX_FULLROWSELECT);
	m_listFloors.SetExtendedStyle(LVS_EX_FULLROWSELECT);
	m_listBaggageDevice.SetExtendedStyle(LVS_EX_FULLROWSELECT);
	const UINT nTitileID[]={
		IDC_RICHEDIT_INPUTDATA_LAYOUT	,
		IDC_RICHEDIT_INPUTDATA_FLOORS	,
		IDC_RICHEDIT_INPUTDATA_PROCS	,
		IDC_RICHEDIT_INPUTDATA_FLIGHTS	,
		IDC_RICHEDIT_INPUTDATA_SCHEDULE	,
		IDC_RICHEDIT_INPUTDATA_DELAY	,
		IDC_RICHEDIT_INPUTDATA_LOADFACTORS	,
		IDC_RICHEDIT_INPUTDATA_AIRCAPACITY	,
		IDC_RICHEDIT_INPUTDATA_BOARDINGCALLS	,
		IDC_RICHEDIT_INPUTDATA_HUBBINGDATA	,
		IDC_RICHEDIT_INPUTDATA_PAX	,
		IDC_RICHEDIT_INPUTDATA_PAXDISTRIBUTION	,
		IDC_RICHEDIT_MOBELEMCOUNT	,
		IDC_RICHEDIT_INPUTDATA_GROUPSIZE	,
		IDC_RICHEDIT_INPUTDATA_LEADLAG	,
		IDC_RICHEDIT_INPUTDATA_INSTEP	,
		IDC_RICHEDIT_INPUTDATA_SIDESTEP	,
		IDC_RICHEDIT_INPUTDATA_SPEED	,
		IDC_RICHEDIT_INPUTDATA_VISITTIME	,
		IDC_RICHEDIT_INPUTDATA_PROCESSORS	,
		IDC_RICHEDIT_INPUTDATA_SERVICETIME	,
		IDC_RICHEDIT_INPUTDATA_ROSTER	,
		IDC_RICHEDIT_INPUTDATA_TRAINSCHEDULE,
		IDC_RICHEDIT_INPUTDATA_BaggageDevice		
	};
	const UINT nSpecialEffect[]=
	{
		IDC_RICHEDIT_INPUTDATA_LAYOUT	,
			IDC_RICHEDIT_INPUTDATA_FLIGHTS	,
			IDC_RICHEDIT_INPUTDATA_PAX	,
			IDC_RICHEDIT_INPUTDATA_PROCESSORS	
	};
	LPCSTR szTitle[]={
		"Layout",
			"Floors",
			"Processors",
			"Flights",
			"Schedule",
			"Delays",
			"Load Factors",
			"Aircraft Capacities",
			"Boarding Calls",
			"Hubbing Data",
			"Passengers",
			"Distributions",
			"Mobile Element Count",
			"Group Size",
			"Lead-Lag Distribution",
			"In-Step Distribution",
			"Side-Step Distribution",
			"Speed",
			"Visit Time",
			"Processors",
			"Service Times",
			"Roster",
			"Train Schedule",
			"BaggageDeviceAssignment"
	};

	CHARFORMAT cf;
	memset(&cf,0,sizeof(CHARFORMAT));
	cf.dwMask = CFM_STRIKEOUT|CFM_BOLD|CFM_SIZE|CFM_UNDERLINE;
	cf.dwEffects = CFE_BOLD|CFM_UNDERLINE;
	cf.yHeight=210;
	
	int nCount=sizeof(nTitileID)/sizeof(UINT);
	CRichEditCtrl* pRichEdit=NULL;
	int i;
	for(i=0;i<nCount;i++)
	{
		pRichEdit=(CRichEditCtrl*)GetDlgItem(nTitileID[i]);
		pRichEdit->SetWordCharFormat(cf);
		pRichEdit->SetWindowText(szTitle[i]);
	}
	nCount=sizeof(nSpecialEffect)/sizeof(UINT);
	cf.yHeight=300;
	cf.dwMask=CFM_STRIKEOUT|CFM_BOLD|CFM_SIZE|CFM_UNDERLINE;
	cf.dwEffects=CFE_BOLD;
	for(i=0;i<nCount;i++)
	{
		pRichEdit=(CRichEditCtrl*)GetDlgItem(nSpecialEffect[i]);
		pRichEdit->SetSel(0,-1);
		pRichEdit->SetWordCharFormat(cf);
	}
}

void CInputDataView::InitReport()
{
	CWaitCursor wait;
	OnInitToolbar();
	LoadFloors();
	LoadProcs();
	LoadFlightSchedule();
	LoadFlightDistribution();
	LoadBoardingCall();
	LoadHubbingData();
	LoadPaxDistribution();
	LoadMobElementCount();
	LoadMobElementDistribution();
	LoadLeadLag();
	LoadServiceTime();
	LoadRoster();
	LoadTrainSchedule();
	LoadBaggageDeviceAssignment();
}

BOOL CInputDataView::PreCreateWindow(CREATESTRUCT& cs) 
{
//	cs.style &= ~FWS_ADDTOTITLE; 
//	cs.style |=FWS_PREFIXTITLE;
	
	return CFormView::PreCreateWindow(cs);
}

void CInputDataView::PrintInputData()
{
// 	m_listFloors.PrintList(_T("Layout"),_T(""));
// 	m_listProcs.PrintList(_T("Processors"),_T(""));
//	m_listRoster.PrintList(_T("Roster"),_T(""));
	CDC dc;
	CPrintDialog printDlg(FALSE);
	if(printDlg.DoModal() == IDCANCEL )
		return;
	dc.Attach( printDlg.GetPrinterDC() );
	dc.m_bPrinting = TRUE;
	CString sTitle;
	sTitle = "InputData";
	DOCINFO di;
	::ZeroMemory( &di, sizeof(DOCINFO) );
	di.cbSize = sizeof( DOCINFO );
	di.lpszDocName = sTitle;

	BOOL bPrintingOK = dc.StartDoc( &di );

	CPrintInfo info;
//	info.m_rectDraw.SetRect( 0,0,dc.GetDeviceCaps( HORZRES ),dc.GetDeviceCaps( VERTRES ));
	int	nPageVMargin,nPageHMargin,nPageHeight,nPageWidth;
	nPageVMargin = dc.GetDeviceCaps(LOGPIXELSY) / 2;
	nPageHMargin = dc.GetDeviceCaps(LOGPIXELSX) / 2;
	nPageHeight  = dc.GetDeviceCaps(VERTRES);
	nPageWidth   = dc.GetDeviceCaps(HORZRES);
	TEXTMETRIC metrics;
	int nLineHeight;

	int nFloors = ComputerListItemCount(&m_listFloors);
	int nProcs = ComputerListItemCount(&m_listProcs);
	int nFltSchedule = ComputerListItemCount(&m_listFltSchedule);
	int nFltDelay = ComputerListItemCount(&m_listFltDelay);
	int nLoad = ComputerListItemCount(&m_listLoadFactors);
	int nBoard = ComputerListItemCount(&m_listBoardingCall);
	int nHub = ComputerListItemCount(&m_listHubbingData);
	int nPaxdist = ComputerListItemCount(&m_listPaxDistribution);
	int nMobile = ComputerListItemCount(&m_listMobileElemCount);
	int nGroup = ComputerListItemCount(&m_listGroupSize);
	int nInStep = ComputerListItemCount(&m_listInStep);
	int nSpeed = ComputerListItemCount(&m_listSpeed);
	int nSideStep = ComputerListItemCount(&m_listSideStep);
	int nVisitTime = ComputerListItemCount(&m_listVisitTime);
	int nLeadLag = ComputerListItemCount(&m_listLeadLag);
	int nSevice = ComputerListItemCount(&m_listServiceTime);
	int nRoster = ComputerListItemCount(&m_listRoster);
	int nTrain = ComputerListItemCount(&m_listTrainSchedule);
	int nBaggage = ComputerListItemCount(&m_listBaggageDevice);
	int nAirCapacity = ComputerListItemCount(&m_listAirCapacity);

	int nCount = nFloors + nProcs + nFltSchedule + nFltDelay + nLoad + nBoard + nHub + nPaxdist + \
		nMobile + nGroup + nInStep + nSideStep + nVisitTime + nLeadLag + nSevice + nRoster + \
		nAirCapacity + nTrain + nBaggage + nSpeed;

	dc.GetTextMetrics(&metrics);
	nLineHeight = metrics.tmHeight + metrics.tmExternalLeading;
	int nMaxPages = (nLineHeight * nCount + 20 *(nLineHeight*8+nPageVMargin) + (nCount/8) *nLineHeight) / nPageHeight + 1;
	info.SetMaxPage(nMaxPages);
	OnBeginPrinting( &dc, &info );
	for( UINT page = info.GetMinPage(); page <= info.GetMaxPage() && bPrintingOK; page++ )
	{
		info.m_rectDraw.SetRect( 0,0,dc.GetDeviceCaps( HORZRES ),dc.GetDeviceCaps( VERTRES ));
		dc.DPtoLP(&info.m_rectDraw);
		info.m_nCurPage = page;
		dc.StartPage();
		OnPrint( &dc, &info );
		dc.EndPage();
		bPrintingOK = TRUE;
	}
	OnEndPrinting( &dc, &info );

	if( bPrintingOK )
		dc.EndDoc();
	else
		dc.AbortDoc();
	dc.Detach();
	m_nIndex = 0;
	m_nUnique = 1;
	m_nVHeight = 0;
}

BOOL CInputDataView::OnPreparePrinting(CPrintInfo* pInfo)
{
	return DoPreparePrinting(pInfo);
}

void CInputDataView::OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo)
{
	// TODO: add extra initialization before printing
}

void CInputDataView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

void CInputDataView::OnPrint(CDC* pDC, CPrintInfo* pInfo)
{
	// TODO: add customized printing code here
	CString line; //This is the print line
	TEXTMETRIC metrics; //Font measurements
	int y = 0; //Current y position on report
	CFont TitleFont; //Font for Title
	CFont HeadingFont; //Font for headings
	CFont DetailFont; //Font for detail lines
	CFont FooterFont; //Font for footer
	int FooterTabStops[] = {350, 650};
	if (!pInfo || pInfo->m_nCurPage == 1) {
		//Set the recordset at the beginning
	}
	short cxInch=pDC->GetDeviceCaps(LOGPIXELSX); 
	short cyInch=pDC->GetDeviceCaps(LOGPIXELSY); 

#define MYFONTSIZE 14 
#define HFONTNAME "InputData Serif"
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

	if(!FooterFont.CreateFont(static_cast<int>(nFontHeight/2), 0, 0, 0, FW_NORMAL, 0, 0,0, \
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
	if (m_nIndex > 19)
	{
		PrintFooter(pDC,pInfo);
		return;
	}
	//Set Y to the line height.
	y += m_nVHeight +static_cast<int>(nLineHeight*1.5)+nPageVMargin;
	if (y> nPageHeight-nPageVMargin-nFooterHeight-nLineHeight-6)
	{
		//exceed new page
		PrintFooter(pDC,pInfo);
		m_nVHeight = 0;
		//Restore default settings
		pDC->SelectObject(OldFont);
		return;
	}
	pDC->TextOut(nPageHMargin, y-static_cast<int>(1.5*nLineHeight), TitleArray[m_nIndex]);
	//Draw a line under the heading
	pDC->MoveTo(nPageHMargin,y-nLineHeight/2);
	pDC->LineTo(nPageWidth-nPageHMargin,y-nLineHeight/2);
	//Set the Heading font
	pDC->SelectObject(&HeadingFont);
	y += nLineHeight;

	int nCol = m_vList[m_nIndex]->GetHeaderCtrl()->GetItemCount();
	HDITEM hi;
	hi.mask = HDI_TEXT ;
	hi.cchTextMax = 49;
	char chBuffer[50];
	hi.pszText = chBuffer;
	int nSpace=(nPageWidth-2*nPageHMargin) / nCol;
	for(int i = 0; i < nCol; i++ )
	{
		::ZeroMemory( chBuffer, 50);
		m_vList[m_nIndex]->GetHeaderCtrl()->GetItem( i,&hi);
		if (nCol <= 2)
		{
			FormStringLine(CString(hi.pszText),pDC,nPageHMargin+i*nSpace,y,45);
		}
		else if ( nCol > 2 && nCol < 5)
		{
			FormStringLine(CString(hi.pszText),pDC,nPageHMargin+i*nSpace,y,25);
		}
		else if(nCol >=5 && nCol < 7)
		{
			FormStringLine(CString(hi.pszText),pDC,nPageHMargin+i*nSpace,y,15);
		}
		else if (nCol >= 7)
		{
			FormStringLine(CString(hi.pszText),pDC,nPageHMargin+i*nSpace,y,10);
		}
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
	BOOL bIncrease = TRUE;
	int nIndex=m_nUnique;
	int n = (int)m_vList.size();
	int nRowCount = m_vList[m_nIndex]->GetItemCount();
	while ( nIndex <= nRowCount ) 
	{
		int nCount = ComputerItemCount(m_vList[m_nIndex],nIndex);
		if (pInfo && (y + (nCount - 1)* nLineHeight)> nPageHeight-nPageVMargin-nFooterHeight-nLineHeight-6) 
		{   // start a new page
			m_nUnique = nIndex;
			m_nVHeight = 0;
			bIncrease = FALSE;
			break;
		}
		//Format the detail line
		int nOffset = y;
		int nMaxOffset = y;
		for( int nSubitem = 0; nSubitem < nCol; nSubitem++ )
		{

			CString chLine = _T("");
			chLine = m_vList[m_nIndex]->GetItemText(nIndex-1,nSubitem);
			if(nCol <= 2)
			{
				nOffset = FormStringLine(chLine,pDC,nPageHMargin+nSubitem*nSpace,y,45);
			}
			else if (nCol > 2 && nCol < 5)
			{
				nOffset = FormStringLine(chLine,pDC,nPageHMargin+nSubitem*nSpace,y,25);
			}
			else if (nCol >=5 && nCol < 7)
			{
				nOffset = FormStringLine(chLine,pDC,nPageHMargin+nSubitem*nSpace,y,15);
			}
			else if (nCol >= 7)
			{
				nOffset = FormStringLine(chLine,pDC,nPageHMargin+nSubitem*nSpace,y,10);
			}
			if (nOffset > nMaxOffset)
			{
				nMaxOffset = nOffset;
			}
		}
		y = nMaxOffset;
		y += nLineHeight; //Adjust y position
		nIndex++; 
	}
	if (bIncrease)
	{
		m_nUnique = 1;
		m_nVHeight = y;
		m_nIndex++;
		OnPrint(pDC,pInfo);
		pDC->SelectObject(OldFont);
		return;
	}
	PrintFooter(pDC,pInfo);
	//Restore default settings
	pDC->SelectObject(OldFont);
}

void CInputDataView::OnPrepareDC(CDC* pDC, CPrintInfo* pInfo) 
{
	// TODO: Add your specialized code here and/or call the base class
}

void CInputDataView::PrintFooter(CDC* pDC, CPrintInfo* pInfo)
{
	CString line; //This is the print line
	CFont FooterFont; //Font for footer
	TEXTMETRIC metrics; //Font measurements
	int FooterTabStops[] = {350, 650};
	pDC->GetTextMetrics(&metrics);
	short cyInch=pDC->GetDeviceCaps(LOGPIXELSY); 

	int nFooterHeight = metrics.tmHeight + metrics.tmExternalLeading;

	int nFontHeight=MulDiv(14, -cyInch, 72); 
	if(nFontHeight % 2) nFontHeight++; 

	if(!FooterFont.CreateFont(static_cast<int>(nFontHeight/2), 0, 0, 0, FW_NORMAL, 0, 0,0, \
		GB2312_CHARSET, OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS, \
		DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, HFONTNAME)) 
		return ;

	int	nPageVMargin,nPageHMargin,nPageHeight,nPageWidth;
		nPageVMargin = pDC->GetDeviceCaps(LOGPIXELSY) / 2;
		nPageHMargin = pDC->GetDeviceCaps(LOGPIXELSX) / 2;
		nPageHeight  = pDC->GetDeviceCaps(VERTRES);
		nPageWidth   = pDC->GetDeviceCaps(HORZRES);
	if (pInfo) {
		pDC->MoveTo(nPageHMargin,nPageHeight-nPageVMargin-static_cast<int>(nFooterHeight*1.5)+20);
		pDC->LineTo(nPageWidth-nPageHMargin,nPageHeight-nPageVMargin-static_cast<int>(nFooterHeight*1.5)+20);
		pDC->SelectObject(&FooterFont);
		line.Format(" \tPage %d",pInfo->m_nCurPage);
		line = _T("InputData ") + line;
		pDC->TabbedTextOut(nPageHMargin, nPageHeight-nPageVMargin-nFooterHeight, line, 2, FooterTabStops, 0);
	}
}

int CInputDataView::FormStringLine(CString sData,CDC* pDC,int nxPos,int nyPos,int nCount)
{
	TEXTMETRIC metrics;
	pDC->GetTextMetrics(&metrics);
	int nLineHeight = metrics.tmHeight + metrics.tmExternalLeading;
	while(sData.GetLength() > nCount)
	{
		pDC->TextOut(nxPos,nyPos,sData.Left(nCount));
		nyPos += nLineHeight;
		sData = sData.Right(sData.GetLength() - nCount);
	}
	pDC->TextOut(nxPos,nyPos,sData);
	return nyPos;
}

int CInputDataView::ComputerItemCount(CListCtrl* pCtrl,int nIndex)
{
	int nMaxCount = 0;
	int nCount = 0;
	for (int i = 0; i < pCtrl->GetHeaderCtrl()->GetItemCount(); i++)
	{
		CString sData = pCtrl->GetItemText(nIndex,i);
		if (pCtrl->GetHeaderCtrl()->GetItemCount() <=2 )
		{
			nCount = sData.GetLength() / 45 + 1;
		}
		else if (pCtrl->GetHeaderCtrl()->GetItemCount() > 2 && pCtrl->GetHeaderCtrl()->GetItemCount() < 5)
		{
			nCount = sData.GetLength() / 25 + 1;
		}
		else if (pCtrl->GetHeaderCtrl()->GetItemCount() >= 5 && pCtrl->GetHeaderCtrl()->GetItemCount() < 7)
		{
			nCount = sData.GetLength() / 15 + 1;
		}
		else if (pCtrl->GetHeaderCtrl()->GetItemCount() >= 7)
		{
			nCount = sData.GetLength() / 10 + 1;
		}
		if (nCount > nMaxCount)
		{
			nMaxCount = nCount;
		}
	}
	return nMaxCount;
}

int CInputDataView::ComputerListItemCount(CListCtrl* pCtrl)
{
	int nMaxCount = 0;
	int nCount = 0;
	for (int i = 0; i < pCtrl->GetItemCount(); i++)
	{
		nCount = ComputerItemCount(pCtrl,i);
		nMaxCount += nCount;
	}
	return nMaxCount;
}