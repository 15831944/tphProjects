#include "StdAfx.h"
#include ".\onboardflightinsim.h"
#include "Common\Flt_cnst.h"
#include "InputOnBoard\OnBoardAnalysisCondidates.h"
#include "Inputs\Flight.h"
#include "InputOnBoard\AircaftLayOut.h"
#include "InputOnBoard\AircraftPlacements.h"
#include "OnboardSeatInSim.h"
#include "Person.h"
#include "PaxDeplaneBehavior.h"
#include "airside/AirsideFlightInSim.h"
#include "OnboardDoorInSim.h"
#include "OnBoardingStairInSim.h"
#include "OnBoardingElevatorInSim.h"
#include "OnBoardingEscalatorInSim.h"
#include "OnBoardCorridorInSim.h"
#include "OnboardSeatRowInSim.h"
#include "OnboardDeckGroundInSim.h"
#include "OnboardSeatGroupInSim.h"
#include "GroundInSim.h"
#include "OnboardWallInSim.h"
#include "OnboardCellGraph.h"
#include "OnboardStorageInSim.h"
#include "SeatAssignmentInSim.h"
#include "terminal.h"
#include "InputOnBoard/SeatTypeSpecification.h"
#include "InputOnBoard/SeatingPreference.h"
#include "InputOnBoard/FlightPaxCabinAssign.h"
#include "terminal.h"
#include "CarryonStoragePrioritiesInSim.h"
#include "../InputOnBoard/CarryonStoragePriorities.h"
#include "../InputOnboard/EntryDoorOperationalSpecification.h"
#include "DoorAssignmentInSim.h"
#include "ARCportEngine.h"
#include "../Common/Path2008.h"
#include "../Common/Line2008.h"
#include "../Common/Point2008.h"
#include "../Results/OnboardDoorLog.h"
#include "Airside/AirsideSimulation.h"
#include "../Results/OutputAirside.h"
#include "../InputOnboard/OnBoardSeatBlockData.h"

OnboardFlightInSim::OnboardFlightInSim(Flight *pFlight,bool bArrival, COnboardFlight *pOnboardFlight)
:m_pCellGraph(NULL)
{
	m_pCarryonVolume					= NULL;
	m_pStorageDevicePriority			= NULL;
	m_pTargetLocation					= NULL;


	m_pFlight = pFlight;
	m_pOnboardFlight = pOnboardFlight;
	m_enumOP = Enum_FlightOpType_ARR;
	if(!bArrival)
		m_enumOP = Enum_FlightOpType_DEP;

	m_pCorridorMgr = new CorridorGraphMgr();

	m_pCellGraph = new OnboardCellGraph(this);

	m_bInitialized = false;

	m_pSeatAssignmentInSim = NULL;
	m_pDoorAssignmentInSim = NULL;

	m_pArcportEngine = NULL;
	m_nCloseCondition = 0;
	m_bGenerateCloseDoorEvent = false;
	m_pWaitAirsideFlight = NULL;
	Initialize();
}

OnboardFlightInSim::~OnboardFlightInSim(void)
{
	std::vector<OnboardSeatInSim *>::iterator iterSeat = m_vSeatInSim.begin();
	for (; iterSeat != m_vSeatInSim.end(); ++iterSeat)
	{
		delete *iterSeat;
	}
	m_vSeatInSim.clear();


	std::vector<OnboardDoorInSim *>::iterator iterDoor = m_vDoorInSim.begin();
	for (; iterDoor != m_vDoorInSim.end(); ++iterDoor)
	{
		delete *iterDoor;
	}
	m_vDoorInSim.clear();

	std::vector<OnboardElevatorInSim*>::iterator iterElevator = m_vElevatorInSim.begin();
	for (; iterElevator != m_vElevatorInSim.end(); ++iterElevator)
	{
		delete *iterElevator;
	}

	std::vector<OnboardStairInSim*>::iterator iterStair = m_vStairInSim.begin();
	for (; iterStair != m_vStairInSim.end(); ++iterStair)
	{
		delete *iterStair;
	}

	std::vector<OnboardEscalatorInSim*>::iterator iterEscalator = m_vEscalatorInSim.begin();
	for (; iterEscalator != m_vEscalatorInSim.end(); ++iterEscalator)
	{
		delete *iterEscalator;
	}

	std::vector<OnboardCorridorInSim*>::iterator iterCorridor = m_vCorridorInSim.begin();
	for (; iterCorridor != m_vCorridorInSim.end(); ++iterCorridor)
	{
		delete *iterCorridor;
	}

	std::vector<OnboardStorageInSim*>::iterator iterStorage = m_vStorageInSim.begin();
	for (; iterStorage != m_vStorageInSim.end(); ++iterStorage)
	{
		delete *iterStorage;
	}

	if (m_pCorridorMgr)
	{
		delete m_pCorridorMgr;
	}

	delete m_pCellGraph;
	m_pCellGraph = NULL;

	if (m_pSeatAssignmentInSim)
	{
		delete m_pSeatAssignmentInSim;
		m_pSeatAssignmentInSim = NULL;
	}

	if (m_pSeatAssignmentInSim)
	{
		delete m_pDoorAssignmentInSim;
		m_pDoorAssignmentInSim = NULL;
	}

	delete m_pStorageDevicePriority;
}

//------------------------------------------------------------------------------------------------------------
//Summary:
//		init seat assignment engine and seat type
//------------------------------------------------------------------------------------------------------------
void OnboardFlightInSim::InitSeatAssignment( CSeatingPreferenceList* pSeatingPreferenceList, CFlightPaxCabinAssign* pOnboardPaxCabin,CFlightSeatTypeDefine* pOnboardSeat,SeatAssignmentType emType)
{
	m_pSeatAssignmentInSim = new SeatAssignmentInSim(this);
	m_pSeatAssignmentInSim->Initialize(pSeatingPreferenceList,pOnboardPaxCabin,emType);
	
	if (pOnboardSeat == NULL)
		return;
	
	for (int i = 0; i < pOnboardSeat->GetItemCount(); i++)
	{
		CSeatTypeDefine* pSeatGroupType = pOnboardSeat->GetItem(i);
		ALTObjectID groupID = pSeatGroupType->getSeatGroup();
		
		for (size_t j = 0; j < m_vSeatInSim.size(); j++)
		{
			OnboardSeatInSim* pSeatInSim = m_vSeatInSim[j];
			CSeat* pSeat = pSeatInSim->m_pSeat;
			ASSERT(pSeat);
			if (pSeat == NULL)
				continue;
			
			const ALTObjectID& seatID = pSeat->GetIDName();
			if (seatID.idFits(groupID))
			{
				pSeatInSim->AddSeatType(pSeatGroupType->getSeatType());
			}
		}
	}
}

void OnboardFlightInSim::InitDoorAssignment(FlightDoorOperationalSpec* pDoorOperationSpc,OnboardFlightSeatBlock* pFlightSeatBlock)
{
	if (pDoorOperationSpc == NULL)
		return;
	
	if (m_pDoorAssignmentInSim)
	{
		delete m_pDoorAssignmentInSim;
		m_pDoorAssignmentInSim = NULL;
	}

	m_pDoorAssignmentInSim = new DoorAssignmentInSim();
	m_pDoorAssignmentInSim->Initlizate(pDoorOperationSpc,pFlightSeatBlock,this);

	for (size_t i = 0; i < m_vDoorInSim.size(); i++)
	{
		OnboardDoorInSim* pDoorInSim = m_vDoorInSim.at(i);
		m_pDoorAssignmentInSim->SetDoorOperationData(pDoorInSim);
	}
}

void OnboardFlightInSim::CalculateCorridorIntersection()
{
	for (int i = 0; i < (int)m_vCorridorInSim.size(); i++)
	{
		m_vCorridorInSim[i]->CalculateIntersection(this);
	}
}

bool OnboardFlightInSim::getShortestPath(const OnboardAircraftElementInSim* pEntryElementInSim,const ARCVector3& entryPos,const OnboardAircraftElementInSim* pExitElementInSim,const ARCVector3& exitPos,CorridorGraphVertexList& _shortestPath)
{
	return m_pCorridorMgr->getShortestPath(pEntryElementInSim,entryPos,pExitElementInSim,exitPos,_shortestPath);
}

bool OnboardFlightInSim::getShortestPath(const ARCVector3& sourcePoint, const ARCVector3& destPoint, CorridorGraphVertexList& _shortestPath)
{
	return m_pCorridorMgr->getShortestPath(sourcePoint,destPoint,_shortestPath);
}

void OnboardFlightInSim::Initialize()
{
	m_pLayout = CAircarftLayoutManager::GetInstance()->GetAircraftLayOutByFlightID(m_pOnboardFlight->getID());
	if(m_pLayout == NULL)
	{
		ASSERT(0);
		return;
	}

	InitDeckGround();

	//initialize seat
	CSeatDataSet* pSeatList = m_pLayout->GetPlacements()->GetSeatData();
	if(pSeatList == NULL)
	{
		ASSERT(0);
		return ;
	}

	int nSeatCount = pSeatList->GetItemCount();
	for (int nSeat = 0; nSeat < nSeatCount; ++nSeat)
	{
		CSeat *pSeat =  pSeatList->GetItem(nSeat);
		OnboardDeckGroundInSim* pDeckGroundInSim = GetDeckInSim(pSeat->GetDeck());
		if(pSeat && pDeckGroundInSim)
		{	
			OnboardSeatInSim *pSeatInSim = new OnboardSeatInSim(pSeat,this);
			m_vSeatInSim.push_back(pSeatInSim);
		}
	}
	//initialize door
	CDoorDataSet* pDoorList = m_pLayout->GetPlacements()->GetDoorData();
	if(pDoorList != NULL)
	{
		for(int nDoor = 0; nDoor < pDoorList->GetItemCount(); ++nDoor)
		{
			CDoor *pDoor =  pDoorList->GetItem(nDoor);
			OnboardDeckGroundInSim* pDeckGroundInSim = GetDeckInSim(pDoor->GetDeck());
			if(pDoor != NULL)
				m_vDoorInSim.push_back(new OnboardDoorInSim(pDoor,this));
		}
	}

	//initialize wall
	COnboardWallDataset* pWallList = m_pLayout->GetPlacements()->GetWallData();
	if (pDoorList != NULL)
	{
		for (int iWall = 0; iWall < pWallList->GetItemCount(); ++iWall)
		{
			COnBoardWall* pWall = pWallList->GetItem(iWall);
			OnboardDeckGroundInSim* pDeckGroundInSim = GetDeckInSim(pWall->GetDeck());
			if (pWall != NULL)
				m_vWallInSim.push_back(new OnboardWallInSim(pWall,this));
		}
	}
	//initialize stair
	COnBoardStairDataSet* pStairList = m_pLayout->GetPlacements()->GetStairData();
	if (pStairList != NULL)
	{
		for (int nStair = 0; nStair < pStairList->GetItemCount(); ++nStair)
		{
			COnBoardStair* pStair = pStairList->GetItem(nStair);
			if (pStair != NULL)
				m_vStairInSim.push_back(new OnboardStairInSim(pStair,this));
			
		}
	}

	//initialize elevator
	COnBoardElevatorDataSet* pElevatorList = m_pLayout->GetPlacements()->GetElevatorData();
	if (pElevatorList != NULL)
	{
		for (int nElevator = 0; nElevator < pElevatorList->GetItemCount(); ++nElevator)
		{
			COnBoardElevator* pElevator = pElevatorList->GetItem(nElevator);
			if (pElevator != NULL)
				m_vElevatorInSim.push_back(new OnboardElevatorInSim(pElevator,this));
			
		}
	}

	//initialize escalator
	COnBoardEscalatorDataSet* pEscalatorList = m_pLayout->GetPlacements()->GetEscalatorData();
	if (pEscalatorList != NULL)
	{
		for (int nEscalator = 0; nEscalator < pEscalatorList->GetItemCount(); ++nEscalator)
		{
			COnBoardEscalator* pEscalator = pEscalatorList->GetItem(nEscalator);
			if (pEscalator != NULL)
				m_vEscalatorInSim.push_back(new OnboardEscalatorInSim(pEscalator,this));
		}
	}

	//initialize corridor
	COnboardCorridorDataset* pCorridorList = m_pLayout->GetPlacements()->GetCorridorData();
	if (pCorridorList != NULL)
	{
		for (int nCorridor = 0; nCorridor < pCorridorList->GetItemCount(); ++nCorridor)
		{
			COnboardCorridor* pCorridor = pCorridorList->GetItem(nCorridor);
			if (pCorridor != NULL)
				m_vCorridorInSim.push_back(new OnboardCorridorInSim(pCorridor,this));

		}
	}

	//initialize storage
	CStorageDataSet* pStorageList = m_pLayout->GetPlacements()->GetStorageData();
	if (pStorageList != NULL)
	{
		for (int nStorage = 0; nStorage < pStorageList->GetItemCount(); ++nStorage)
		{
			CStorage* pStorage = pStorageList->GetItem(nStorage);
			if (pStorage != NULL)
				m_vStorageInSim.push_back(new OnboardStorageInSim(pStorage,this));
		}
	}
	//initialize seat row
	std::vector<CSeatRow*> vSeatRow;
	int iSeatInSimCount = (int)m_vSeatInSim.size();
	for (int i = 0; i < iSeatInSimCount; i++)
	{
		OnboardSeatInSim* pSeatInSim = m_vSeatInSim[i];
		CSeatRow* pSeatRow = pSeatInSim->m_pSeat->getRow();
		if (pSeatRow == NULL)
			continue;
		
		OnboardSeatRowInSim* pSeatRowInSim = GetSeatRowInSim(pSeatRow);
		if (pSeatRowInSim == NULL)
		{
			pSeatRowInSim = new OnboardSeatRowInSim(pSeatRow,this);
			m_vSeatRowInSim.push_back(pSeatRowInSim);
		}
		
		pSeatRowInSim->AddSeatInSim(pSeatInSim);
		pSeatInSim->SetSeatRowInSim(pSeatRowInSim);
		vSeatRow.push_back(pSeatRow);
	}
	CalculateSeatRowSpace();

	//initialize seat group
	std::vector<CSeatGroup*> vSeatGroup;
	for (int i = 0; i < iSeatInSimCount; i++)
	{
		OnboardSeatInSim* pSeatInSim = m_vSeatInSim[i];
		CSeatGroup* pSeatGroup = pSeatInSim->m_pSeat->getGroup();
		if (pSeatGroup == NULL)
			continue;

		OnboardSeatGroupInSim* pSeatGroupInSim = GetSeatGroupInSim(pSeatGroup);
		if (pSeatGroupInSim == NULL)
		{
			pSeatGroupInSim = new OnboardSeatGroupInSim(pSeatGroup,this);
			m_vSeatGroupInSim.push_back(pSeatGroupInSim);
		}

		pSeatGroupInSim->AddSeatInSim(pSeatInSim);
		pSeatInSim->SetSeatGroupInSim(pSeatGroupInSim);
		vSeatGroup.push_back(pSeatGroup);
	}


	CalculateSeatGroupSpace();
	CalculateSeatRowEntrypoint();
	CalculateStorageDeviceEntrypoin();
	
//set mobile occupy
	setOnboardCellBarrier();

	CalculateCorridorIntersection();

	//if (m_pCorridorMgr)
	//{
	//	m_pCorridorMgr->initGraphMgr(this);
	//}

}

void OnboardFlightInSim::CalculateSeatRowSpace()
{
	for (size_t i = 0; i < m_vSeatRowInSim.size(); i++)
	{
		OnboardSeatRowInSim* pSeatRowInSim = m_vSeatRowInSim[i];
		pSeatRowInSim->CalculateSpace();
	}
}

void OnboardFlightInSim::CalculateSeatRowEntrypoint()
{
	for (size_t i = 0; i < (int)m_vSeatRowInSim.size(); i++)
	{
		OnboardSeatRowInSim* pSeatRowInSim = m_vSeatRowInSim[i];
		pSeatRowInSim->CreateEntryPoint();
	}
}

void OnboardFlightInSim::CalculateSeatGroupSpace()
{
	for (size_t i = 0; i < m_vSeatGroupInSim.size(); i++)
	{
		OnboardSeatGroupInSim* pSeatGroupInSim = m_vSeatGroupInSim[i];
		pSeatGroupInSim->InitSeatRow();
		pSeatGroupInSim->CalculateSpace();
	}
}	

void OnboardFlightInSim::InitDeckGround()
{
	CDeckManager* pDeckManger = m_pLayout->GetDeckManager();
	if (pDeckManger && m_pCellGraph)
	{
		m_pCellGraph->InitGraph(pDeckManger);
	}
}

OnboardDeckGroundInSim* OnboardFlightInSim::GetDeckInSim(CDeck* pDeck)
{
	if(m_pCellGraph)
		return m_pCellGraph->GetDeckInSim(pDeck);

	return NULL;
}
OnboardDeckGroundInSim* OnboardFlightInSim::GetDeckInSim(int iDeck)
{	
	if(m_pCellGraph)
		return m_pCellGraph->GetDeckInSim(iDeck);

	return NULL;
}
void OnboardFlightInSim::setOnboardCellBarrier()
{
	if(m_pCellGraph)
		m_pCellGraph->setOnboardCellBarrier();
}

GroundInSim* OnboardFlightInSim::GetGroundInSim(CDeck* pDeck)
{
	if(m_pCellGraph)
		return m_pCellGraph->GetGroundInSim(pDeck);

	return NULL;
}

OnboardStorageInSim* OnboardFlightInSim::GetOnboardStorage(GroundInSim* pGroundInSim,const ARCVector3& pt)
{
	std::map<double,OnboardStorageInSim*> mapEntry;
	
	for (size_t i = 0; i < m_vStorageInSim.size(); i++)
	{
		OnboardStorageInSim* pStorageInSim = m_vStorageInSim[i];
		if (pStorageInSim->GetGround() != pGroundInSim)
			continue;
		
		ARCVector3 storagePt;
		pStorageInSim->GetPosition(storagePt);
		storagePt.n[VZ] = pt.n[VZ];
		
		double dDist = pt.DistanceTo(storagePt);

		mapEntry.insert(std::make_pair(dDist,pStorageInSim));
	}

	if (mapEntry.empty())
		return NULL;

	double dShortestDist = (*mapEntry.begin()).first;
	if (dShortestDist > 150.0)//find storage in 1.0m range
	{
		return NULL;
	}
	
	return (*mapEntry.begin()).second;
}

std::vector<OnboardStorageInSim*> OnboardFlightInSim::GetCanReachStorageDevice( GroundInSim* pGroundInSim,const ARCVector3& pt, double dReachDist /*= 150*/ )
{
	std::map<double,OnboardStorageInSim*> mapEntry;
	for (size_t i = 0; i < m_vStorageInSim.size(); i++)
	{
		OnboardStorageInSim* pStorageInSim = m_vStorageInSim[i];
		if (pStorageInSim->GetGround() != pGroundInSim)
			continue;
		
		ARCVector3 storagePt;
		pStorageInSim->GetPosition(storagePt);
		storagePt.n[VZ] = pt.n[VZ];
		
		double dDist = pt.DistanceTo(storagePt);

		mapEntry.insert(std::make_pair(dDist,pStorageInSim));
	}

	std::vector<OnboardStorageInSim*> vStorageDevice;

	std::map<double,OnboardStorageInSim*>::iterator iter = mapEntry.begin();
	for (; iter != mapEntry.end(); ++iter)
	{
		if((*iter).first <= dReachDist)
            vStorageDevice.push_back((*iter).second);
	}

	return vStorageDevice;

}
OnboardSeatRowInSim* OnboardFlightInSim::GetSeatRowInSim(CSeatRow* pSeatRow)const
{
	for (size_t i = 0; i < m_vSeatRowInSim.size(); i++)
	{
		OnboardSeatRowInSim* pExistSeatRowInSim = m_vSeatRowInSim[i];
		if (pExistSeatRowInSim->GetSeatRowInput() == pSeatRow)
		{
			return pExistSeatRowInSim;
		}
	}
	return NULL;
}

OnboardSeatGroupInSim* OnboardFlightInSim::GetSeatGroupInSim(CSeatGroup* pSeatGroup)const
{
	for (size_t i = 0; i < m_vSeatGroupInSim.size(); i++)
	{
		OnboardSeatGroupInSim* pExistSeatGroupInSim = m_vSeatGroupInSim[i];
		if (pExistSeatGroupInSim->GetSeatGroupInput() ==  pSeatGroup)
		{
			return pExistSeatGroupInSim;
		}
	}
	return NULL;
}

OnboardDoorInSim* OnboardFlightInSim::GetEntryDoor() const
{
	OnboardDoorInSim *pDoor = NULL;

	if(m_vDoorInSim.size())
		pDoor = m_vDoorInSim.at(0);

	return pDoor;
}

OnboardSeatInSim* OnboardFlightInSim::GetSeat(Person* pPerson,CString& strMessage)
{
	OnboardSeatInSim* pSeatInSim = NULL;
	if (m_mapObjects.Lookup(pPerson,pSeatInSim))
		return pSeatInSim;
	
	pSeatInSim = m_pSeatAssignmentInSim->GetSeat(pPerson,strMessage);
	if (pSeatInSim)
	{
		m_mapObjects[pPerson] = pSeatInSim;
		pSeatInSim->AssignToPerson(pPerson);
	}

	return pSeatInSim;
}

void OnboardFlightInSim::FlightTerminated(const ElapsedTime& eTime )
{
	//flush log of seat 
	std::vector<OnboardSeatInSim *>::const_iterator iter = m_vSeatInSim.begin();
	for (; iter != m_vSeatInSim.end(); ++iter)
	{
	 	OnboardSeatInSim *pSeat = *iter;
	 	if(pSeat && pSeat->GetSittingPerson())
	 	{
			if (m_enumOP == Enum_FlightOpType_DEP || pSeat->GetAsignedPerson()->getState() > EntryOnboard)
			{
				if(pSeat->GetSittingPerson()->getCurrentBehavior())
				{
					pSeat->GetSittingPerson()->getCurrentBehavior()->writeLog(eTime,false);
					pSeat->visitorWriteLog(eTime);
				}
				pSeat->GetSittingPerson()->flushLog(eTime,true);
				pSeat->visitorFlushLog(eTime);
				pSeat->PersonLeave();
				pSeat->clearVisitor();
			}
	 	}
	}

	//flush log of storage
	std::vector<OnboardStorageInSim *>::const_iterator iterStorage = m_vStorageInSim.begin();
	for (; iterStorage != m_vStorageInSim.end(); ++iterStorage)
	{
		OnboardStorageInSim *pStorage = *iterStorage;
		if(pStorage)
		{
			if (m_enumOP == Enum_FlightOpType_DEP || pStorage->onboardState())
			{
				pStorage->visitorWriteLog(eTime);
				pStorage->visitorFlushLog(eTime);
				pStorage->clearVisitor();
			}
		}
	}
}

CString OnboardFlightInSim::getFlightID() const
{
	if(m_pOnboardFlight)
		return m_pOnboardFlight->getFlightID();

	return CString("");
}

bool OnboardFlightInSim::IsSameFlight( Flight *pFlight,bool bArrival )
{
	if(m_pFlight != pFlight)
		return false;

	Enum_FlightOpType enumOP = Enum_FlightOpType_ARR;
	if(!bArrival)
		enumOP = Enum_FlightOpType_DEP;
	if(m_enumOP != enumOP)
		return false;


	return true;

}

OnboardDoorInSim* OnboardFlightInSim::GetExitDoor() const
{
	OnboardDoorInSim *pDoor = NULL;

	if(m_vDoorInSim.size())
		pDoor = m_vDoorInSim.at(0);

	return pDoor;
}

void OnboardFlightInSim::GetPaxListInFlight( std::vector<Person*>& vPaxList )
{
	std::vector<OnboardSeatInSim *>::const_iterator iter = m_vSeatInSim.begin();
	for (; iter != m_vSeatInSim.end(); ++iter)
	{
		if((*iter) && (*iter)->GetAsignedPerson() )
		{
			vPaxList.push_back((*iter)->GetAsignedPerson());
		}
	}
}

void OnboardFlightInSim::WakeupPassengerDeplane( const ElapsedTime& tTime )
{
	std::vector<OnboardSeatInSim *>::const_iterator iter = m_vSeatInSim.begin();
	for (; iter != m_vSeatInSim.end(); ++iter)
	{
		if((*iter) && (*iter)->IsAssigned())
		{
			PaxDeplaneBehavior* pBehavior = (PaxDeplaneBehavior*)(*iter)->GetSittingPerson()->getCurrentBehavior();
			pBehavior->GenerateEvent(tTime) ;
			//(*iter)->PersonLeave();
		}
	}
}

bool OnboardFlightInSim::IsSeatsAvailable( int nSeatCount )
{
	std::vector<OnboardSeatInSim *>::const_iterator iter = m_vSeatInSim.begin();
	for (; iter != m_vSeatInSim.end(); ++iter)
	{
		if((*iter) && !(*iter)->IsAssigned())
		{
			nSeatCount--;
		}
	}
	
	if (nSeatCount >0)
		return false;

	return true;
}

std::vector<EntryPointInSim* >& OnboardFlightInSim::GetSeatEntryPoint( )
{
	return m_vSeatEntryPoint;
}

OnboardCellGraph * OnboardFlightInSim::getCellGraph() const
{
	ASSERT(m_pCellGraph != NULL);
	return m_pCellGraph;
}

CarryonVolumeInSim * OnboardFlightInSim::GetCarryonVolume() const
{
	return m_pCarryonVolume;
}

void OnboardFlightInSim::SetCarryonVolume( CarryonVolumeInSim * pVolume )
{
	m_pCarryonVolume = pVolume;
}

FlightCarryonPriorityInSim * OnboardFlightInSim::GetCarryonPriority() const
{
	return m_pStorageDevicePriority;
}

void OnboardFlightInSim::SetCarryonPriority( CFlightCarryonStoragePriorities *pPriority )
{
	delete m_pStorageDevicePriority;
	m_pStorageDevicePriority = new FlightCarryonPriorityInSim(pPriority);

}

TargetLocationVariableInSim * OnboardFlightInSim::GetTargetLocation() const
{
	return m_pTargetLocation;
}

void OnboardFlightInSim::SetTargetLocation( TargetLocationVariableInSim * pTargetLocation )
{
	m_pTargetLocation = pTargetLocation;
}

void OnboardFlightInSim::CalculateStorageDeviceEntrypoin()
{
	for (size_t i = 0; i < m_vStorageInSim.size(); i++)
	{
		OnboardStorageInSim* pStorageInSim = m_vStorageInSim[i];
		if (pStorageInSim)
			pStorageInSim->CreateEntryPoint();
	}
}

bool OnboardFlightInSim::SetOnboardDoorConnect( const ARCVector2& vDir,const CPoint2008& locationPt,const CLine2008& line )
{
	if (m_vDoorInSim.empty())
		return false;
	
	for (size_t doorIdx = 0; doorIdx < m_vDoorInSim.size(); doorIdx++)
	{
		OnboardDoorInSim* pDoorInSim = m_vDoorInSim.at(doorIdx);
		CPoint2008 groundPt;
		CPoint2008 doorPos = pDoorInSim->GetLocation();
		ARCVector3 ptDoorPos;
		pDoorInSim->GetPosition(ptDoorPos);

		doorPos.rotate(-vDir.AngleFromCoorndinateX());
		doorPos.DoOffset(locationPt.getX(),locationPt.getY(),locationPt.getZ());
		doorPos.setZ(locationPt.getZ());
		CPoint2008 pt = line.getProjectPoint(doorPos);
		groundPt = CPoint2008(doorPos - pt);
		groundPt.Normalize();
		groundPt.length(doorPos.getZ() + 2);
		groundPt += doorPos;

		CDoor* pDoor = pDoorInSim->m_pDoor;

		doorPos.setZ(ptDoorPos.n[VZ]);
		
		pDoorInSim->SetConnectionPos(doorPos);
		pDoorInSim->SetGroundPos(groundPt);
	}
	return true;
}

OnboardDoorInSim* OnboardFlightInSim::GetDoor(Person* pPerson,OnboardSeatInSim* pSeatInSim) const
{
	if (m_pDoorAssignmentInSim)
	{
		return m_pDoorAssignmentInSim->GetDoor(pSeatInSim,pPerson);
	}

	if (!m_vDoorInSim.empty())
	{
		return m_vDoorInSim.front();
	}
	return NULL;
}

void OnboardFlightInSim::GenerateCloseDoorEvent( const ElapsedTime& time )
{
	if (!m_bGenerateCloseDoorEvent)
	{
		m_bGenerateCloseDoorEvent = true;
		for (size_t i = 0; i < m_vDoorInSim.size(); i++)
		{
			OnboardDoorInSim* pDoorInSim = m_vDoorInSim.at(i);
			pDoorInSim->GenerateCloseDoorEvent(time);
		}
	}
}


bool OnboardFlightInSim::ProcessCloseDoor(const ElapsedTime& time)
{
	for (size_t i = 0; i < m_vDoorInSim.size(); i++)
	{
		OnboardDoorInSim* pDoorInSim = m_vDoorInSim.at(i);
		pDoorInSim->ProcessCloseDoor(time);
	}

	if (ReadyCloseDoor()&&m_pWaitAirsideFlight)
	{
		m_pWaitAirsideFlight->WakeUp(time);
	}

	return true;
}

bool OnboardFlightInSim::ReadyCloseDoor() const
{
	return (m_nCloseCondition == 0 && m_bGenerateCloseDoorEvent) ? true : false;
}

void OnboardFlightInSim::AddCount()
{
	m_nCloseCondition++;
}

void OnboardFlightInSim::ClearPaxFromHisSeat(int nPaxID, ElapsedTime eTime)
{
	std::vector<OnboardSeatInSim *>::const_iterator iter = m_vSeatInSim.begin();
	for (; iter != m_vSeatInSim.end(); ++iter)
	{
		if((*iter) && (*iter)->GetAsignedPerson())
		{
			Person *pPerson = (*iter)->GetAsignedPerson();

			if(pPerson)
			{
				if(pPerson->getID() == nPaxID)
				{
					(*iter)->ClearPaxFromSeat();
				}
				else if(pPerson->getOnboardBehavior())
				{
					pPerson->getOnboardBehavior()->RemoveWaitingPax(nPaxID, eTime);
				}
			}			
		}	
	}
}
void OnboardFlightInSim::DecCount()
{
	if (m_nCloseCondition != 0)
	{
		m_nCloseCondition--;
	}
}



void OnboardFlightInSim::WriteDoorCloseLog( OnboardDoorLog* pLog )
{
	if (m_pArcportEngine == NULL)
		return;

	AirsideSimulation* pAirsideSim = m_pArcportEngine->GetAirsideSimulation();
	if (pAirsideSim == NULL)
		return;
	
	if (pAirsideSim->m_pOuput)
	{
		pAirsideSim->m_pOuput->LogFlightDoor(pLog);
	}
		
}

void OnboardFlightInSim::GetFitestDoor( const DoorOperationInSim* pDoorOperationInSim,std::vector<OnboardDoorInSim*>& vDoorList ) const
{
	for (size_t i = 0; i < m_vDoorInSim.size(); i++)
	{
		OnboardDoorInSim* pDoorInSim = m_vDoorInSim.at(i);
		if (pDoorInSim->GetOperationData() == pDoorOperationInSim)
		{
			vDoorList.push_back(pDoorInSim);
		}
	}
}
//////////////////////////////////////////////////////////////////////////
//
OnboardFlightInSimList::OnboardFlightInSimList(COnBoardAnalysisCandidates* pFlightCandiates)
:m_pFlightCandiates(pFlightCandiates)
{

}

OnboardFlightInSimList::~OnboardFlightInSimList()
{
	std::vector<OnboardFlightInSim *>::iterator iter = m_vOnboardFlight.begin();
	for (;iter != m_vOnboardFlight.end(); ++iter)
	{
		delete (*iter);
	}
	m_vOnboardFlight.clear();
}

OnboardFlightInSim * OnboardFlightInSimList::GetOnboardFlightInSim(AirsideFlightInSim* pFlightInSim, bool bArrival)
{
	OnboardFlightInSim *pOnboardFlightInSim = NULL;
	Flight* pFlight = pFlightInSim->GetFlightInput();
	
	if(m_pFlightCandiates == NULL || pFlight == NULL)
		return pOnboardFlightInSim;

	
	//find in existed database
	pOnboardFlightInSim = GetExistedOnboardFlightInSim(pFlight,bArrival);
	if(pOnboardFlightInSim)
		return pOnboardFlightInSim;


	//first time to get on board flight

	char szID[1024] = {0};
	char szACType[1024] = {0};
	if(bArrival)
		pFlight->getFullID(szID,'A');
	else
		pFlight->getFullID(szID,'D');

	pFlight->getACType(szACType);

	int nDay = pFlight->getArrTime().GetDay();
	
	FlightType fltOpType = ArrFlight;
	if(!bArrival)
		fltOpType = DepFlight;

	COnboardFlight* pExistFlight = m_pFlightCandiates->GetExistOnboardFlight(szID,szACType,fltOpType,nDay);
	
	if(pExistFlight)
	{
		pOnboardFlightInSim  = new OnboardFlightInSim(pFlight,bArrival,pExistFlight);

		AirsideFlightDescStruct& fltDesc = pFlightInSim->GetLogEntry().GetAirsideDesc();	
		if(bArrival)
			fltDesc.nOnboardArrID = pExistFlight->getID();
		else
			fltDesc.nOnboardDepID = pExistFlight->getID();
		
		m_vOnboardFlight.push_back(pOnboardFlightInSim);
	}


	return pOnboardFlightInSim;
}

OnboardFlightInSim * OnboardFlightInSimList::GetExistedOnboardFlightInSim( Flight* pFlight, bool bArrival )
{
	OnboardFlightInSim *pOnboardFlightInSim = NULL;

	
	if(m_pFlightCandiates == NULL || pFlight == NULL)
		return pOnboardFlightInSim;

	
	std::vector<OnboardFlightInSim *>::iterator iter = m_vOnboardFlight.begin();
	for (;iter != m_vOnboardFlight.end(); ++iter)
	{
		if((*iter) && (*iter)->IsSameFlight(pFlight,bArrival))
		{
			return *iter;
		}
	}
	return NULL;
}


void OnboardFlightInSimList::TerminateAll( const ElapsedTime& t )
{
	for(size_t i=0;i<m_vOnboardFlight.size();i++)
	{
		OnboardFlightInSim* pFlight = m_vOnboardFlight[i];
		pFlight->FlightTerminated(t);
	}
}
