#include "StdAfx.h"
#include ".\onboardsimulation.h"
#include "Engine\OnboardFlightInSim.h"
#include "ARCportEngine.h"
#include "InputOnBoard\CInputOnboard.h"
#include "InputOnBoard\SeatingPreference.h"
#include "InputOnBoard\SeatTypeSpecification.h"
#include "InputOnBoard\FlightPaxCabinAssign.h"
#include "InputOnBoard\OnboardPaxCabinAssign.h"
#include "InputOnBoard\CSeatingAssignmentCabinData.h"

#include "./CarryonVolumeInSim.h"
#include "./TargetLocationVariableInSim.h"
#include "./CarryonStoragePrioritiesInSim.h"
#include "InputOnBoard/EntryDoorOperationalSpecification.h"
#include "InputOnBoard/OnBoardSeatBlockData.h"
#include "./Airside/AirsideFlightInSim.h"
#include "../CommonData/ProjectCommon.h"


OnboardSimulation::OnboardSimulation(CARCportEngine *_pEngine,InputOnboard *pInputOnboard)
:m_pEngine(_pEngine)
,m_pInputOnboard(pInputOnboard)
,m_lstOnboardFlightInSim(NULL)
,m_pOnboardCabinAssign(NULL)
,m_pSeatPreferenceList(NULL)
,m_pOnboardSeatType(NULL)
,m_pSeatAssignmentStrategy(NULL)
,m_pCarryonVolume(NULL)
,m_pTagetLocation(NULL)
,m_pCarryonPriority(NULL)
,m_pArrDoorOperationSpc(NULL)
,m_pDepDoorOperationSpc(NULL)
,m_pFlightSeatBlockManager(NULL)
{
	m_pProjectCom = new ProjectCommon();
}

OnboardSimulation::~OnboardSimulation(void)
{
	if (m_pProjectCom)
	{
		delete m_pProjectCom;
		m_pProjectCom = NULL;
	}
	Clear();
}

void OnboardSimulation::Initialize()
{
	Clear();
	if(m_pEngine == NULL || 
		m_pInputOnboard == NULL )
		return;

	m_lstOnboardFlightInSim = new OnboardFlightInSimList(m_pInputOnboard->GetOnBoardFlightCandiates());

	Terminal* pTerm = m_pEngine->getTerminal();
	if (pTerm)
	{
		int nProjectID = pTerm->m_nProjID;


		m_pOnboardCabinAssign = new COnboardPaxCabinAssign();
		m_pProjectCom->setStringDictionary(pTerm->inStrDict);
		m_pOnboardCabinAssign->setPrjCommon(m_pProjectCom);
		m_pOnboardCabinAssign->setPrjID(pTerm->m_nProjID);
		m_pOnboardCabinAssign->ReadData(-1);

		m_pSeatPreferenceList = new CSeatingPreferenceList(pTerm->inStrDict);
		m_pSeatPreferenceList->ReadData(pTerm->m_nProjID,pTerm->inStrDict);

		m_pOnboardSeatType = new COnboardSeatTypeDefine();
		m_pOnboardSeatType->ReadData(pTerm->m_nProjID);

		m_pSeatAssignmentStrategy = new CSeatingAssignmentCabinList();
		m_pSeatAssignmentStrategy->ReadData(pTerm->m_nProjID);

		m_pCarryonVolume = new CarryonVolumeInSim;
		m_pCarryonVolume->Initialize(nProjectID);

		m_pCarryonPriority = new CarryonStoragePrioritiesInSim;
		m_pCarryonPriority->Initialize(pTerm->inStrDict);

		m_pTagetLocation = new TargetLocationVariableInSim;
		m_pTagetLocation->Initialize(nProjectID, pTerm->inStrDict);

		m_pArrDoorOperationSpc = new DoorOperationalSpecification(DoorOperationalSpecification::ExitDoor);
		m_pArrDoorOperationSpc->ReadData(pTerm->inStrDict);

		m_pDepDoorOperationSpc = new DoorOperationalSpecification(DoorOperationalSpecification::EntryDoor);
		m_pDepDoorOperationSpc->ReadData(pTerm->inStrDict);

		m_pFlightSeatBlockManager = new OnboardFlightSeatBlockContainr();
		m_pFlightSeatBlockManager->ReadData(-1);
	}
	
}

void OnboardSimulation::Clear()
{
	if(m_lstOnboardFlightInSim)
		delete m_lstOnboardFlightInSim;
	m_lstOnboardFlightInSim = NULL;

	if (m_pOnboardCabinAssign)
	{
		delete m_pOnboardCabinAssign;
		m_pOnboardCabinAssign = NULL;
	}

	if (m_pSeatPreferenceList)
	{
		delete m_pSeatPreferenceList;
		m_pSeatPreferenceList = NULL;
	}

	if (m_pOnboardSeatType)
	{
		delete m_pOnboardSeatType;
		m_pOnboardSeatType = NULL;
	}

	if (m_pSeatAssignmentStrategy)
	{
		delete m_pSeatAssignmentStrategy;
		m_pSeatAssignmentStrategy = NULL;
	}

	delete m_pTagetLocation;
	m_pTagetLocation = NULL;

	delete m_pCarryonVolume;
	m_pCarryonVolume = NULL;

	if (m_pArrDoorOperationSpc)
	{
		delete m_pArrDoorOperationSpc;
		m_pArrDoorOperationSpc = NULL;
	}

	if (m_pDepDoorOperationSpc)
	{
		delete m_pDepDoorOperationSpc;
		m_pDepDoorOperationSpc = NULL;
	}

	if (m_pFlightSeatBlockManager)
	{
		delete m_pFlightSeatBlockManager;
		m_pFlightSeatBlockManager = NULL;
	}
}

OnboardFlightInSim * OnboardSimulation::GetOnboardFlightInSim(AirsideFlightInSim* pFlight, bool bArrival  )
{

	if(m_lstOnboardFlightInSim)
	{
		OnboardFlightInSim* pOnboardFlightInSim = m_lstOnboardFlightInSim->GetOnboardFlightInSim(pFlight,bArrival);
		if (pOnboardFlightInSim)
		{
			InitializeFlight(pOnboardFlightInSim);

			return pOnboardFlightInSim;
		}
	}

	return NULL;
}
void OnboardSimulation::InitializeFlight( OnboardFlightInSim* pOnboardFlightInSim )
{
	if(pOnboardFlightInSim->IsInitialized())
		return;
	pOnboardFlightInSim->SetInitialized();
	//seat assignment
	CFlightPaxCabinAssign* pFlightCabin = m_pOnboardCabinAssign->GetFlightCabin(pOnboardFlightInSim->GetOnboardFlight());
	CFlightSeatTypeDefine* pFlightSeat = m_pOnboardSeatType->GetFlightSeat(pOnboardFlightInSim->GetOnboardFlight());
	SeatAssignmentType emType = m_pSeatAssignmentStrategy->GetSeatAssignmentType(pOnboardFlightInSim->GetOnboardFlight());
	pOnboardFlightInSim->InitSeatAssignment(m_pSeatPreferenceList,pFlightCabin,pFlightSeat,emType);
	
	
	//carry on volume settings
	pOnboardFlightInSim->SetCarryonVolume(m_pCarryonVolume);


	//seat priority
	CFlightCarryonStoragePriorities* pPriority = m_pCarryonPriority->GetDevicePriority(pOnboardFlightInSim->GetOnboardFlight()->getID());
	pOnboardFlightInSim->SetCarryonPriority(pPriority);

	
	//target location variable
	pOnboardFlightInSim->SetTargetLocation(m_pTagetLocation);

	OnboardFlightSeatBlock* pFlightSeatBlock = m_pFlightSeatBlockManager->GetFlightSeatBlock(pOnboardFlightInSim->GetOnboardFlight());

	if (pOnboardFlightInSim->GetOnboardFlight()->getFlightType() == ArrFlight)
	{
		FlightDoorOperationalSpec* pDoorOperationSpec = m_pArrDoorOperationSpc->GetOnboardFlightDoorOperation(pOnboardFlightInSim->GetOnboardFlight());
		pOnboardFlightInSim->InitDoorAssignment(pDoorOperationSpec,pFlightSeatBlock);
	}
	else
	{
		FlightDoorOperationalSpec* pDoorOperationSpec = m_pDepDoorOperationSpc->GetOnboardFlightDoorOperation(pOnboardFlightInSim->GetOnboardFlight());
		pOnboardFlightInSim->InitDoorAssignment(pDoorOperationSpec,pFlightSeatBlock);
	}

}


void OnboardSimulation::FlightTerminated( Flight* pFlight, bool bArrival,const ElapsedTime& eTime )
{
	if(m_lstOnboardFlightInSim == NULL)
		return;

	OnboardFlightInSim *pOnboardFlight = m_lstOnboardFlightInSim->GetExistedOnboardFlightInSim(pFlight,bArrival);
	if (pOnboardFlight == NULL)
		return;

	pOnboardFlight->FlightTerminated( eTime );
	
}

void OnboardSimulation::clearPerson( const ElapsedTime& t )
{
	if(m_lstOnboardFlightInSim)
	{
		m_lstOnboardFlightInSim->TerminateAll(t);
	}
}

bool OnboardSimulation::ExistedOnboardFlightInSim( AirsideFlightInSim* pFlight,bool bArrival)
{
	Flight* pFlightInput = pFlight->GetFlightInput();
	if (pFlightInput == NULL)
		return false;

	if (m_lstOnboardFlightInSim == NULL)
		return false;
	
	return m_lstOnboardFlightInSim->GetExistedOnboardFlightInSim(pFlightInput,bArrival) ? true : false;
}
























