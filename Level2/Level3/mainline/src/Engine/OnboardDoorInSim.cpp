#include "StdAfx.h"
#include ".\onboarddoorinsim.h"
#include "InputOnBoard\Door.h"
#include "InputOnBoard\Deck.h"
#include "GroundInSim.h"
#include "OnboardFlightInSim.h"
#include "OnboardElementSpace.h"
#include "EntryPointInSim.h"
#include "PaxOnboardBaseBehavior.h"
#include "Person.h"
#include "../Common/ProbabilityDistribution.h"
#include "../Common/ProDistrubutionData.h"
#include ".\InputOnBoard\EntryDoorOperationalSpecification.h"
#include "OnboardCloseDoorEvent.h"
#include "ARCportEngine.h"
#include "../Results/OnboardDoorLog.h"
#include "DoorAssignmentInSim.h"
#include ".\InputOnBoard\OnBoardAnalysisCondidates.h"


OnboardDoorInSim::OnboardDoorInSim(CDoor *pDoor,OnboardFlightInSim* pFlightInSim)
:OnboardAircraftElementInSim(pFlightInSim)
,m_pDoor(pDoor)
,m_pElementSpace(NULL)
,m_pEntryPointInSim(NULL)
,m_bClose(false)
,m_pDoorOperation(NULL)
,m_pCloseDoorEvent(NULL)
{
	CalculateSpace();
	CreateEntryPoint();
}

OnboardDoorInSim::~OnboardDoorInSim(void)
{
	ClearSpace();

	if (m_pEntryPointInSim)
	{
		delete m_pEntryPointInSim;
		m_pEntryPointInSim = NULL;
	}
}

BOOL OnboardDoorInSim::GetPosition( ARCVector3& doorPos ) const
{
	ASSERT(m_pDoor != NULL);
	if(m_pDoor == NULL)
		return FALSE;
	doorPos = m_pDoor->GetPosition();	
	doorPos[VZ] = m_pDoor->GetDeck()->RealAltitude();

	return TRUE;
}

void OnboardDoorInSim::ClearSpace()
{
	if (m_pElementSpace)
	{
		delete m_pElementSpace;
		m_pElementSpace = NULL;
	}
}

CString OnboardDoorInSim::GetType() const
{
	return m_pDoor->GetType();
}

GroundInSim* OnboardDoorInSim::GetGround()
{
	return m_pFlightInSim->GetGroundInSim(m_pDoor->GetDeck());
}


void OnboardDoorInSim::CreatePollygon()
{
	CPath2008 path;
	m_pDoor->GetDoorPath(path);
	CPollygon2008 pollygon;
	pollygon.init(path.getCount(),path.getPointList());
	m_pElementSpace->setPollygon(pollygon);
}
 
 void OnboardDoorInSim::CalculateSpace()
 {
	ClearSpace();
	m_pElementSpace = new OnboardElementSpace(GetGround());
	//m_pElementSpace->CreatePollygon(this);
	//m_pElementSpace->CalculateSpace();
 }

 void OnboardDoorInSim::CreateEntryPoint()
 {
	 ARCVector3 doorPos;
	 GetPosition(doorPos);
	 Point ptTo(0.0,-20, 0);
	 doorPos += ptTo;

	 CPoint2008 entryPos(doorPos.n[VX],doorPos.n[VY],doorPos.n[VZ]);
	 OnboardCellInSim* pCellInSim = GetGround()->getCell(entryPos);
	 if (pCellInSim)
	 {
		m_pEntryPointInSim = new EntryPointInSim(pCellInSim,m_pFlightInSim);
		m_pEntryPointInSim->GetOnboardCell()->entryPoint(true);

		//calculate the entry place
		//
		//space
		//40 * 40 
		std::vector<CPoint2008> vSpace;
		vSpace.push_back(CPoint2008(entryPos.getX() + 20, entryPos.getY() + 20, entryPos.getZ()));
		vSpace.push_back(CPoint2008(entryPos.getX() - 20, entryPos.getY() + 20, entryPos.getZ()));
		vSpace.push_back(CPoint2008(entryPos.getX() - 20, entryPos.getY() - 20, entryPos.getZ()));
		vSpace.push_back(CPoint2008(entryPos.getX() + 20, entryPos.getY() - 20, entryPos.getZ()));

		CPollygon2008 pollygon;	
		pollygon.init(vSpace.size(),&vSpace[0]);
		

		OnboardElementSpace elementSpace(GetGround());
		elementSpace.setPollygon(pollygon);
		elementSpace.CalculateSpace();
		OnboardSpaceCellInSim spaceCellInSim;
		elementSpace.getOccupySpace(spaceCellInSim);
		
		for (int nCell = 0; nCell <spaceCellInSim.getCount(); ++nCell )
		{
			OnboardCellInSim *pCell = spaceCellInSim.getItem(nCell);
			if(pCell)
			{
				pCell->SetState(OnboardCellInSim::Idle);
				m_entryLocation.addCell(pCell);
				//pCell->AddNotifyObjectWhenPaxLeave(this);
			}
		}
		m_entryLocation.setCellCenter(pCellInSim);
		m_entryLocation.setPathCell(pCellInSim);
		
		CPath2008 posRect;
		posRect.init(vSpace.size(), &vSpace[0]);
		m_entryLocation.setPaxRect(posRect);

		ASSERT(m_entryLocation.getCellCount() > 0);
	 }
 }

 OnboardCellInSim * OnboardDoorInSim::GetEntryCell() const
 {
	return m_pEntryPointInSim->GetOnboardCell();
 }

 bool OnboardDoorInSim::IsAvailable(PaxOnboardBaseBehavior* pPax)
 {
	 for (int nCell = 0; nCell <m_entryLocation.getCellCount(); ++nCell )
	 {
		 OnboardCellInSim *pCell = m_entryLocation.getCell(nCell);
		 if(pCell)
		{
			if(pPax)
			{
				if(pCell->getBehavior()!= NULL&& pCell->getBehavior() != pPax)
					return false;
			}
			else
			{
				if(pCell->getBehavior()!= NULL)
					return false;
			}

		}
	 }
	 return true;

 }

 void OnboardDoorInSim::AddWaitingPax( PaxOnboardBaseBehavior* pPax,const ElapsedTime& eTime)
 {
	 if (std::find(m_vPaxWaitting.begin(), m_vPaxWaitting.end(), pPax) == m_vPaxWaitting.end())
	 {
		 m_vPaxWaitting.push_back(pPax);
	 }	
	 //if the first one, 
	 if(m_vPaxWaitting[0] == pPax)
	 {
		 pPax->GenerateEvent(eTime + ElapsedTime(1L));
	 }
 }

 void OnboardDoorInSim::ReleaseNextPax(const ElapsedTime& eTime )
 {
	 if(m_vPaxWaitting.size() > 0)
	 {
		 PaxOnboardBaseBehavior* pPax = m_vPaxWaitting[0];
		 if(pPax)
		 {
			 TRACE("\r\n Wake up passenger at door : %d, time: %d", pPax->getPerson()->getID(), (long)eTime);
			 pPax->GenerateEvent(eTime + ElapsedTime(1L));
		 }
		 //m_vPaxWaitting.erase(m_vPaxWaitting.begin());
	 }
 }
 void OnboardDoorInSim::DeletePaxWait( PaxOnboardBaseBehavior *pPax )
 {
	 std::vector<PaxOnboardBaseBehavior*>::iterator iter = std::find(m_vPaxWaitting.begin(), m_vPaxWaitting.end(), pPax);

	 if (iter != m_vPaxWaitting.end())
	 {
		 m_vPaxWaitting.erase(iter);
	 }
 }
 PaxCellLocation OnboardDoorInSim::getEntryLocation() const
 {
	 return m_entryLocation;
 }

 void OnboardDoorInSim::NotifyPaxLeave( OnboardCellInSim *pCell, const ElapsedTime& eTime )
 {
	 //if(IsAvailable(NULL))
	 //{
		// ReleaseNextPax(eTime);
	 //}
 }

 bool OnboardDoorInSim::CheckDoorClear(PaxOnboardBaseBehavior *pBehavior, const ElapsedTime& eTime  )
 {
	 //if(IsAvailable(NULL))
	 //{
		// std::vector<PaxOnboardBaseBehavior*>::iterator iterFind = std::find(m_vPaxWaitting.begin(), m_vPaxWaitting.end(),pBehavior);
		// if(iterFind != m_vPaxWaitting.end())
		//	m_vPaxWaitting.erase(m_vPaxWaitting.begin());

		// if(m_vPaxWaitting.size() > 0)
		// {
		//	 if(IsAvailable( *m_vPaxWaitting.begin()))
		//		ReleaseNextPax(eTime);
		// }



		 return true;
	 //}

	 //return false;
 }

 int OnboardDoorInSim::GetResourceID() const
 {
	if(m_pDoor)
		return m_pDoor->GetID();

	return -1;
 }

 void OnboardDoorInSim::SetOperationData( DoorOperationInSim* pData )
 {
	ASSERT(pData);
	m_pDoorOperation = pData;
 }

 bool OnboardDoorInSim::ConnectValid()
{
	if (m_pDoorOperation == NULL)
		return true;
	 
	if(m_pDoorOperation->getStatus() == DoorOperationalData::CLOSED)
		return false;

	return true;
}

CPoint2008 OnboardDoorInSim::GetLocation() const
{
	CPoint2008 ptLocation;
	ARCVector3 ptDoor = m_pDoor->GetPosition();
	
	ptLocation.init(ptDoor.n[VX],ptDoor.n[VY],ptDoor.n[VZ]);
	return ptLocation;
}


void OnboardDoorInSim::GenerateCloseDoorEvent( const ElapsedTime& time )
{
	//check door whether close
	if (m_bClose)
		return;
	
	//check door define data
	if (m_pDoorOperation == NULL)
		return;

	m_tStartClost = m_pDoorOperation->GetStartClosure();
	m_tCloseTime = m_pDoorOperation->GetCloseOperationTime();
	ElapsedTime eTime;
	if (m_pFlightInSim->GetOnboardFlight()->getFlightType() == ArrFlight)
	{
		eTime = max(eTime,time - m_tStartClost);
	}
	else
	{
		eTime = time + m_tStartClost;
	}
	
	if(m_pCloseDoorEvent == NULL)
	{
		m_pCloseDoorEvent = new OnboardCloseDoorEvent(this);
		m_pCloseDoorEvent->setTime(time);
		m_pCloseDoorEvent->addEvent();
	}
}	

bool OnboardDoorInSim::ProcessCloseDoor( const ElapsedTime& time )
{
	WriteCloseDoorLog(time);
	return true;
}

void OnboardDoorInSim::WriteCloseDoorLog( const ElapsedTime& time )
{
	OnboardDoorLog* pLog =new OnboardDoorLog();
	pLog->SetDoorID(m_pDoor->GetID());
	pLog->SetFlightID(m_pFlightInSim->getFlightID());
	pLog->SetStartTime(time);
	pLog->SetEndTime(time + m_tCloseTime);

	m_pFlightInSim->WriteDoorCloseLog(pLog);

	m_bClose = true;
}