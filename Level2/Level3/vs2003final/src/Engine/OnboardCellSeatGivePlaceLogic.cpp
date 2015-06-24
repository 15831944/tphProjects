#include "StdAfx.h"
#include ".\onboardcellseatgiveplacelogic.h"
#include <algorithm>
#include "Person.h"
#include "EntryPointInSim.h"
#include "OnboardSeatInSim.h"
#include "PaxOnboardBaseBehavior.h"
#include "PaxEnplaneBehavior.h"
#include "OnboardPaxCellMoveToSeatEvent.h"
#include "OnboardCellFreeMoveLogic.h"
#include "OnboardCellFreeMoveEvent.h"
#include "OnboardStorageInSim.h"
#include "TargetLocationVariableInSim.h"
#include "OnboardFlightInSim.h"


OnboardCellSeatGivePlaceLogic::OnboardCellSeatGivePlaceLogic( EntryPointInSim *pEntryPoint )
:m_pEntryPoint(pEntryPoint)
,m_pCurPax(NULL)
,m_bInProcessing(false)
{
	if(m_pEntryPoint && m_pEntryPoint->GetCreateSeat())
		m_pSeatRow = m_pEntryPoint->GetCreateSeat()->GetSeatRowInSim();
}

OnboardCellSeatGivePlaceLogic::~OnboardCellSeatGivePlaceLogic(void)
{

}

void OnboardCellSeatGivePlaceLogic::ReportArrivalCheckingPlace( PaxOnboardBaseBehavior *pArrivePax, const ElapsedTime& eArrTime )
{
	if(m_bInProcessing)
	{
		AddEvent(pArrivePax, eArrTime + ElapsedTime(1L));
		//try after 1 seconds
		return;
	}

	std::vector<PaxOnboardBaseBehavior *> vPaxNeedGivePlace;
	if(CheckIfNeedGivePlace(pArrivePax, vPaxNeedGivePlace))
	{	
		
		pArrivePax->setState(WaittingAtCheckingPoint);
		//need to give place

		//check the condition is available for give place
		//check the place is clear, 
		
		//IsClearGivePlace();
		//get the place need to secure and reserve it
		//calculate the place to seating logic
		CalculateGivePlaceSpace(pArrivePax,vPaxNeedGivePlace);
		//reserve the location
		m_pCurPax = (PaxEnplaneBehavior *)pArrivePax;
		int nPaxID = m_pCurPax->getPerson()->getID();

		if(ReserveGivePlaceSpace(pArrivePax))
		{
			//start the give place processor
			if(IsReady(m_pCurPax))
			{
				bool bAllSitOnSeat = true;
				int nPaxCount = vPaxNeedGivePlace.size();
				for (int nPax = 0; nPax < nPaxCount; ++ nPax)
				{
					PaxOnboardBaseBehavior * pB = vPaxNeedGivePlace[nPax];
					if(pB && pB->getState() != SitOnSeat)
					{
						bAllSitOnSeat = false;
						break;
					}					
				}
				if(bAllSitOnSeat)
				{
					m_bInProcessing = true;
					//lock the location
					m_pCurPax->m_bCouldOverWirteReseve = false;

					StartGivePlaceOperation(pArrivePax,vPaxNeedGivePlace, eArrTime);
					return;
				}
			}
		}
		
		//try another time
		AddEvent(pArrivePax,eArrTime + ElapsedTime(1L));
		return;
	}

	//move to seat entry ==> seat front ==> sit down
	pArrivePax->setState(StrideOverSeat);
	ProcessEvent(pArrivePax, eArrTime);
}

bool OnboardCellSeatGivePlaceLogic::CheckIfNeedGivePlace( PaxOnboardBaseBehavior *pCurPax, std::vector<PaxOnboardBaseBehavior *>& vPaxNeedGivePlace) const
{
	OnboardSeatInSim *pCurSeat = pCurPax->getSeat();
	//EntryPointInSim *pEntryPoint = pCurSeat->GetEntryPoint();
	OnboardSeatRowInSim *pSeatRow = pCurSeat->GetSeatRowInSim();
	ASSERT(pSeatRow != NULL);

	std::vector<OnboardSeatInSim *> vGivePlaceSeats =  pSeatRow->GetSeatNeedGivePlace(pCurSeat,m_pEntryPoint);
	int nSeatCount = static_cast<int>(vGivePlaceSeats.size());
	if(nSeatCount <= 0)// the first seat
		return false;

	//if(vGivePlaceSeats.size() > 0)
	for (int nSeat = 0; nSeat < nSeatCount; ++ nSeat)
	{
		OnboardSeatInSim *pTempSeat = vGivePlaceSeats[nSeat];
		if(pTempSeat == NULL)
			continue;
		if(pTempSeat->GetAsignedPerson() == NULL)
			continue;

		Person *pTmpPax = pTempSeat->GetAsignedPerson();
		if(pTmpPax == NULL)
			continue;

		PaxOnboardBaseBehavior *pTmpBehavior = pTmpPax->getOnboardBehavior();
		if(pTmpBehavior == NULL)
			continue;

		if(IsPaxArrivalAtSeatRow(pTmpBehavior))
			vPaxNeedGivePlace.push_back(pTmpBehavior);
	}

	if(vPaxNeedGivePlace.size() > 0)
		return true;//has passenger would be give place

	
	return false;
}	

bool OnboardCellSeatGivePlaceLogic::IsPaxArrivalAtSeatRow( PaxOnboardBaseBehavior *pPax ) const
{
	if (std::find(m_vPaxArrived.begin(), m_vPaxArrived.end(), pPax) == m_vPaxArrived.end())//no find
		return false;

	return true;
}

int OnboardCellSeatGivePlaceLogic::ProcessEvent( PaxOnboardBaseBehavior *pPax, const ElapsedTime& eTime )
{
	int nState = pPax->getPerson()->getState();

	switch (nState)
	{
	case ArriveAtCheckingPoint:
		processArriveAtCheckingPoint(pPax, eTime);
		break;
	case ArriveAtStorage:
		processArriveAtStorage(pPax, eTime);
		break;
	case ReturnToCheckingPoint:
		processReturnToCheckingPoint(pPax,eTime);
		break;
	case WaittingAtCheckingPoint:
		processWaitingAtCheckingPoint(pPax, eTime);
		break;
	case StrideOverSeat:
		processStartStrideOverSeat(pPax, eTime);

	case ArriveAtSeatEntry:
		processArriveAtSeatEntry(pPax, eTime );
		break;

	case WaitAtTempPlace:
		processWaitAtTmpPlace(pPax, eTime );
		break;

	case ArriveAtSeat://passenger arrive at seat
		processStowCarryonToSeat(pPax, eTime );
		break;

	case StowCarryonToSeat:
		processArriveAtSeat(pPax, eTime );
		break;

	default:
		{

		}
	}

	return 1;

}

void OnboardCellSeatGivePlaceLogic::processArriveAtCheckingPoint( PaxOnboardBaseBehavior *pCurPax, ElapsedTime p_time )
{
	PaxEnplaneBehavior *pPax = (PaxEnplaneBehavior *)pCurPax;

	//checking the location is available

	//if(pPax->m_paxCheckingLocation.CanReserve(pCurPax))
		//pPax->m_paxCheckingLocation.PaxReserve(pCurPax);

	if(!pPax->m_paxCheckingLocation.IsAvailable(pPax))
	{
		AddEvent(pPax, p_time + ElapsedTime(1L));//try after one second
		return;
	}
	pPax->getCurLocation().PaxClear(pCurPax, p_time);
	pPax->m_paxCheckingLocation.PaxOccupy(pCurPax);
	pPax->SetPaxLocation(pPax->m_paxCheckingLocation);
    //move to the location
	CPoint2008 ptCheck = pPax->m_paxCheckingLocation.getCellCenter()->getLocation();

	MoveTo(pCurPax, ARCVector3(ptCheck), p_time);

	//storage the  operation
	pPax->setState(ArriveAtStorage);
	AddEvent(pPax, p_time);

}

void OnboardCellSeatGivePlaceLogic::processReturnToCheckingPoint(PaxOnboardBaseBehavior *pCurPax, ElapsedTime p_time )
{
	PaxEnplaneBehavior *pPax = (PaxEnplaneBehavior *)pCurPax;
	ARCVector3 vecCheck = pPax->m_paxCheckingLocation.getCellCenter()->getLocation();

	ARCVector3 vecEntry;
	m_pEntryPoint->GetLocation(vecEntry); 
	ARCVector3 vectEntryDir = vecEntry - vecCheck;

	vectEntryDir.Normalize();
	vectEntryDir.SetLength(0.1);
	vecCheck = pCurPax->getPoint() + vectEntryDir;

	pPax->setState(WaittingAtCheckingPoint);
	MoveTo(pPax, vecCheck, p_time);

	AddEvent(pPax, p_time);
}

void OnboardCellSeatGivePlaceLogic::AddEvent(PaxOnboardBaseBehavior *pCurPax, const ElapsedTime& eTime)
{
	OnboardPaxCellMoveToSeatEvent *pEvent =new OnboardPaxCellMoveToSeatEvent(this, pCurPax,eTime);
	pEvent->addEvent();
}

void OnboardCellSeatGivePlaceLogic::processArriveAtStorage( PaxOnboardBaseBehavior *pCurPax, ElapsedTime p_time )
{
	//storage the carry on
	//change face direction

	ARCVector3 vecEntry;
	m_pEntryPoint->GetLocation(vecEntry); 
	GroundInSim* pGroundInSim = m_pEntryPoint->GetCreateSeat()->GetGround();
	
	ARCVector3 vecSeat;
	m_pEntryPoint->GetCreateSeat()->GetFrontPosition(vecSeat);

	ARCVector3 vDirection = vecSeat - vecEntry;
	vDirection.Normalize();
	vDirection.SetLength(0.1);

	//calculate the location
	ARCVector3 vecCurLocation = pCurPax->getPoint();
	vecCurLocation += vDirection;

	MoveTo(pCurPax, vecCurLocation, p_time);

	//start storage operation

	PaxEnplaneBehavior* pPax = (PaxEnplaneBehavior*)pCurPax;
	double dSearchDist = pPax->m_pOnboardFlight->GetTargetLocation()->GetSearchRadius(*(pPax->m_pPerson->getType().GetPaxType()));
	std::vector<OnboardStorageInSim*> vStorageInSim = pPax->GetCanReachStorageDevice(pGroundInSim,vecCurLocation, dSearchDist);
	if (vStorageInSim.size()>0)
	{
		//pCurPax->SetCurrentResource(pStorageInSim);
		pPax->StorageOperation(p_time,vStorageInSim);
	}
	
	pCurPax->SetCurrentResource(NULL);
	//start moving
	OnboardCellPath routePath;
	pPax->m_pCellFreeMoveLogic->setCellPath(routePath);//path
	pPax->m_pCellFreeMoveLogic->setCurLocation(pPax->m_paxCheckingLocation);//start position
	pPax->m_pCellFreeMoveLogic->SetEndPaxState(ReturnToCheckingPoint);


	//deliver to Free Moving Logic, the passenger would come back at state ArriveAtCheckingPoint
	OnboardCellFreeMoveEvent *pMoveEvent = new OnboardCellFreeMoveEvent(pPax->m_pCellFreeMoveLogic);
	pMoveEvent->setTime(p_time);
	pMoveEvent->addEvent();
}
void OnboardCellSeatGivePlaceLogic::processWaitingAtCheckingPoint( PaxOnboardBaseBehavior *pCurPax, ElapsedTime p_time )
{
	ReportArrivalCheckingPlace(pCurPax, p_time);
}

void OnboardCellSeatGivePlaceLogic::processStartStrideOverSeat( PaxOnboardBaseBehavior *pCurPax, ElapsedTime p_time )
{

	if(std::find(m_vPaxArrived.begin(), m_vPaxArrived.end(), pCurPax) == m_vPaxArrived.end())
		m_vPaxArrived.push_back(pCurPax);

	//move to seat entry position
	pCurPax->getCurLocation().PaxClear(pCurPax, p_time);
	pCurPax->getCurLocation().Clear();
	
	ARCVector3 curLocation = pCurPax->getPoint();
	MoveTo(pCurPax, curLocation , p_time);

	ARCVector3 vecEntry;
	m_pEntryPoint->GetLocation(vecEntry);
	MoveTo(pCurPax, vecEntry, p_time);
	//occupy the entry location
	PaxCellLocation& entryLocation = m_pEntryPoint->GetEntryLocation();
	entryLocation.PaxOccupy(pCurPax);
	pCurPax->SetPaxLocation(entryLocation);



	PaxOnboardBaseBehavior::MobDir enumDir = pCurPax->GetDirectionSidle(CPoint2008(),true);
	MoveTo(pCurPax, vecEntry, p_time,enumDir);

	//get path
	std::vector<OnboardSeatInSim *> vPassSeats = m_pSeatRow->GetSeatNeedGivePlace(pCurPax->getSeat(),m_pEntryPoint);

	ARCVector3 ptDest = ARCVector3();
	for (int nSeat = 0; nSeat < static_cast<int>(vPassSeats.size()); ++ nSeat)
	{
		OnboardSeatInSim *pPassSeat = vPassSeats[nSeat];
		if (pPassSeat == NULL)
			continue;

		//set destination
		pPassSeat->GetFrontPosition( ptDest);

		pCurPax->SetCurrentResource(NULL);
		//move the location
		MoveTo(pCurPax, ptDest, p_time, enumDir);
		if(nSeat == 0)//moved to first seat
		{
			//wake up the passenger in the tmp place

			std::vector<PaxOnboardBaseBehavior *> vPaxAtTempPlace = m_vPaxAtTempPlace;
			std::reverse(vPaxAtTempPlace.begin(), vPaxAtTempPlace.end());
			int nPaxCount = static_cast<int>(vPaxAtTempPlace.size());
			for ( int nPax = 0; nPax < nPaxCount; ++ nPax)
			{
				PaxOnboardBaseBehavior *pBehavior = vPaxAtTempPlace[nPax];
				pBehavior->setState(WaitAtTempPlace);
				AddEvent(pBehavior, p_time + ElapsedTime(1L * nPax) );
			}
		}
	}

	//move to seat
	ARCVector3 seatPos;
	pCurPax->getSeat()->GetFrontPosition(seatPos);
	MoveTo(pCurPax, ARCVector3(seatPos), p_time, enumDir);

	MoveTo(pCurPax,  ARCVector3(seatPos), p_time, PaxOnboardBaseBehavior::BACKWARD);

	////sit down position
	//CPoint2008 ptSeatLocation;
	//pCurPax->getSeat()->GetLocation(ptSeatLocation);
	//MoveTo(pCurPax, ARCVector3(ptSeatLocation), p_time, PaxOnboardBaseBehavior::BACKWARD);

	////wakeup the passenger come back to his seat

	
	pCurPax->setState(ArriveAtSeat);
	AddEvent(pCurPax,p_time);

}


void OnboardCellSeatGivePlaceLogic::processArriveAtSeatEntry( PaxOnboardBaseBehavior *pCurPax,ElapsedTime p_time )
{


}

void OnboardCellSeatGivePlaceLogic::processWaitAtTmpPlace( PaxOnboardBaseBehavior *pCurPax,ElapsedTime p_time )
{
	//wait at tmp place
	//move to seat
	MoveFromTmpPlaceToSeat(pCurPax,pCurPax->getSeat(), p_time);
}

void OnboardCellSeatGivePlaceLogic::processStowCarryonToSeat( PaxOnboardBaseBehavior *pCurPax,ElapsedTime p_time )
{
	//if(m_pCurPax == pCurPax)
	//{
	//	CancelReservedGivePlaceSpace(pCurPax);
	//
	int nPaxCount = static_cast<int>(m_vPaxAtTempPlace.size());

	//checking give place logic
	if(nPaxCount > 0)
	{
		bool bProcessing = false;
		for ( int nPax = 0; nPax < nPaxCount; ++ nPax)
		{
			if(pCurPax->getState() != ArriveAtSeat &&
				pCurPax->getState() != SitOnSeat)
				bProcessing = true;
		}
		if(bProcessing == false)//finished give place 
		{
			m_vPaxAtTempPlace.clear();
			m_vTmpWaitingLocation.clear();
			CancelReservedGivePlaceSpace(m_pCurPax);
			m_pCurPax->m_bCouldOverWirteReseve = true;
			m_bInProcessing = false;
			//m_pCurPax->getCurLocation().PaxClear(m_pCurPax, p_time);
			//m_pCurPax->getCurLocation().Clear();
		}
		//if(m_pCurPax != pCurPax)
		//{
		//	pCurPax->getCurLocation().PaxClear(pCurPax, p_time);
		//	pCurPax->getCurLocation().Clear();
		//}
	}
	//else//have no give place operation
	{
		pCurPax->getCurLocation().PaxClear(pCurPax, p_time);
		pCurPax->getCurLocation().Clear();
	}

	pCurPax->SetCurrentResource(pCurPax->getSeat());
	pCurPax->setState(ArriveAtSeat);

	//////////////////////////////////////////////////////////////////////////
	//separate with non passenger
	//start to move carry on to seat staging
	PaxEnplaneBehavior* pPax = (PaxEnplaneBehavior*)pCurPax;
	pPax->SeatOperation(p_time);


	//sit down position
	CPoint2008 ptSeatLocation;
	pCurPax->getSeat()->GetLocation(ptSeatLocation);
	MoveTo(pCurPax, ARCVector3(ptSeatLocation), p_time, PaxOnboardBaseBehavior::BACKWARD);

	//wakeup the passenger come back to his seat

	ARCVector3 seatPos;
	pCurPax->getSeat()->GetPosition(seatPos);

	double dRot = pCurPax->getSeat()->GetRotation();
	ARCVector2 vdir(5,0);
	vdir.Rotate(dRot);
	seatPos+= ARCVector3(vdir,0);
	MoveTo(pCurPax, seatPos, p_time);
	
	pCurPax->setState(SitOnSeat);
	MoveTo(pCurPax, seatPos, p_time);

	//pCurPax->setState(StowCarryonToSeat);
	pCurPax->setState(StowCarryonToSeat);
	AddEvent(pCurPax,p_time);




}
void OnboardCellSeatGivePlaceLogic::processArriveAtSeat( PaxOnboardBaseBehavior *pCurPax,ElapsedTime p_time )
{
	pCurPax->SetCurrentResource(pCurPax->getSeat());
	//finished 
	pCurPax->setState(SitOnSeat);

	ARCVector3 curPos = pCurPax->getPoint();

	MoveTo(pCurPax, curPos, p_time);

}


void OnboardCellSeatGivePlaceLogic::CalculateGivePlaceSpace(PaxOnboardBaseBehavior *pCurPax, 
															std::vector<PaxOnboardBaseBehavior *>& vPaxNeedGivePlace)
{
	m_vTmpWaitingLocation.clear();
	//checking location

	//entry location

	//the first waiting location
	PaxEnplaneBehavior *pPax = (PaxEnplaneBehavior *)pCurPax;
	PaxCellLocation tmpWaitingLocation1;
	if(m_pEntryPoint->GetAnotherCheckingLocation(pPax->m_paxCheckingLocation,tmpWaitingLocation1))
		m_vTmpWaitingLocation.push_back(tmpWaitingLocation1);

    //the other checking location
	if(vPaxNeedGivePlace.size() > 1)
	{
		PaxEnplaneBehavior *pPax = (PaxEnplaneBehavior *)pCurPax;
		OnboardSeatInSim *pSeat = pCurPax->getSeat();
		EntryPointInSim *pEntryPoint = pSeat->GetEntryPoint();
		PaxCellLocation tmpWaitingLocation2;
		pEntryPoint->CalculateTempLocation(pPax->m_paxCheckingLocation, tmpWaitingLocation2);
		m_vTmpWaitingLocation.push_back(tmpWaitingLocation2);
	}

	//passenger size > 2

	std::reverse(m_vTmpWaitingLocation.begin(),m_vTmpWaitingLocation.end());
}

bool OnboardCellSeatGivePlaceLogic::ReserveGivePlaceSpace(PaxOnboardBaseBehavior *pCurPax)
{
	//checking location	
	PaxEnplaneBehavior *pPax = (PaxEnplaneBehavior *)pCurPax;
	if(pPax->m_paxCheckingLocation.CanReserve(pPax))
	{
		pPax->m_paxCheckingLocation.PaxReserve(pPax);
	}
	else
		return false;

	//entry location
	if(m_pEntryPoint->GetEntryLocation().CanReserve(pPax))
	{
		m_pEntryPoint->GetEntryLocation().PaxReserve(pPax);
	}
	else
		return false;

	//temp location
	int nTmpWaitingCount = m_vTmpWaitingLocation.size();
	for (int nPlace = 0; nPlace < nTmpWaitingCount; ++ nPlace)
	{
		PaxCellLocation& location = m_vTmpWaitingLocation[nPlace];
		if(location.CanReserve(pPax))
		{
			location.PaxReserve(pPax);
		}
		else
			return false;
	}

	return true;
}

void OnboardCellSeatGivePlaceLogic::CancelReservedGivePlaceSpace(PaxOnboardBaseBehavior *pCurPax)
{
	PaxEnplaneBehavior *pPax = (PaxEnplaneBehavior *)pCurPax;
	m_pCurPax->m_paxCheckingLocation.PaxCancelReserve(pPax);

	//entry location
	m_pEntryPoint->GetEntryLocation().PaxCancelReserve(pPax);
	//pPax->m_paxCheckingLocation.PaxCancelReserve(pPax);

	//temp location
	int nTmpWaitingCount = m_vTmpWaitingLocation.size();
	for (int nPlace = 0; nPlace < nTmpWaitingCount; ++ nPlace)
	{
		PaxCellLocation& location = m_vTmpWaitingLocation[nPlace];
		location.PaxCancelReserve(pPax);
	}
}

bool OnboardCellSeatGivePlaceLogic::IsReady( PaxOnboardBaseBehavior *pCurPax )
{
	//checking location	
	PaxEnplaneBehavior *pPax = (PaxEnplaneBehavior *)pCurPax;
	if(!pPax->m_paxCheckingLocation.IsAvailable(pPax))
		return false;

	//entry location
	if(!m_pEntryPoint->GetEntryLocation().IsAvailable(pPax))
		return false;

	//temp location
	int nTmpWaitingCount = m_vTmpWaitingLocation.size();
	for (int nPlace = 0; nPlace < nTmpWaitingCount; ++ nPlace)
	{
		PaxCellLocation& location = m_vTmpWaitingLocation[nPlace];
		if(!location.IsAvailable(pPax))
			return false;
	}

	return true;
}

void OnboardCellSeatGivePlaceLogic::StartGivePlaceOperation( PaxOnboardBaseBehavior *pArrivePax, std::vector<PaxOnboardBaseBehavior *>& vPaxNeedGivePlace, const ElapsedTime& eArrTime )
{
	//get the first passenger move to tmp place one by one
	ElapsedTime eFinishedTime = eArrTime;

	
	for (int nGivePlacePax = 0; nGivePlacePax < static_cast<int>(vPaxNeedGivePlace.size()); ++nGivePlacePax )
	{
		PaxOnboardBaseBehavior *pPassenger  = vPaxNeedGivePlace[nGivePlacePax];

		PaxEnplaneBehavior *pPax = (PaxEnplaneBehavior *)pPassenger;
		//passenger move to tmp place
	
		eFinishedTime = eArrTime + ElapsedTime(nGivePlacePax * 1L);
		MoveFromSeatToTmpPlace(pPax, pPax->getSeat(), m_vTmpWaitingLocation[nGivePlacePax], eFinishedTime);
		m_vPaxAtTempPlace.push_back(pPax);
	}

	//start next location

	pArrivePax->setState(StrideOverSeat);
	AddEvent(pArrivePax,eFinishedTime);

}

void OnboardCellSeatGivePlaceLogic::MoveFromSeatToTmpPlace(PaxOnboardBaseBehavior *pPax, OnboardSeatInSim *pSeat, PaxCellLocation& tmpPlace, ElapsedTime& eStartTime )
{
	//get path
	std::vector<OnboardSeatInSim *> vPassSeats = m_pSeatRow->GetSeatNeedGivePlace(pSeat,m_pEntryPoint);
	
	//write log
	//current seat
	pPax->writeLog(eStartTime);

	ElapsedTime eNextTime = eStartTime + ElapsedTime(1L);

	//stand up
	pPax->setState(StandUp);
	pPax->writeLog(eNextTime);
	
	
	pPax->setState(OnboardFreeMoving);
	//his seat location
	ARCVector3 vecMySeat;
	pPax->getSeat()->GetFrontPosition(vecMySeat);
	MoveTo(pPax, vecMySeat, eNextTime);

	PaxOnboardBaseBehavior::MobDir enumDir = pPax->GetDirectionSidle(CPoint2008(),false);
	MoveTo(pPax, vecMySeat, eNextTime, enumDir);
	//side-walk to entry point
	ARCVector3 ptDest = ARCVector3();
	for (int nSeat = 0; nSeat < static_cast<int>(vPassSeats.size()); ++ nSeat)
	{
		OnboardSeatInSim *pPassSeat = vPassSeats[nSeat];
		if (pPassSeat == NULL)
			continue;

		//set destination
		pPassSeat->GetFrontPosition( ptDest);

		//move the location
		MoveTo(pPax, ptDest, eNextTime, enumDir);
	}

	//move to entry position
	m_pEntryPoint->GetLocation( ptDest );
	MoveTo( pPax, ptDest, eNextTime , enumDir);


	//move to tmp place, backward
	MoveTo(pPax,ptDest, eNextTime, PaxOnboardBaseBehavior::BACKWARD);
	ptDest = tmpPlace.getCellCenter()->getLocation();
	
	//waiting at tmp place
	pPax->setState(WaitAtTempPlace);
	MoveTo(pPax, ptDest, eNextTime, PaxOnboardBaseBehavior::BACKWARD);

	
	//lock the paxcell location
	pPax->SetPaxLocation(tmpPlace);
	tmpPlace.PaxOccupy(pPax);

	
	
	eStartTime = eNextTime;
}

void OnboardCellSeatGivePlaceLogic::MoveFromTmpPlaceToSeat(PaxOnboardBaseBehavior *pPax,  OnboardSeatInSim *pSeat, ElapsedTime& eStartTime )
{

	//unlock the place
	pPax->getCurLocation().PaxClear(pPax, eStartTime);
	pPax->getCurLocation().Clear();

	//lock entry position
	//occupy the entry location
	PaxCellLocation& entryLocation = m_pEntryPoint->GetEntryLocation();
	entryLocation.PaxOccupy(pPax);
	pPax->SetPaxLocation(entryLocation);

	//get path
	//now at tmp place
	//pPax->writeLog(eStartTime);
	ARCVector3 ptDest;
	ptDest = pPax->getPoint();
	MoveTo(pPax, ptDest ,eStartTime, PaxOnboardBaseBehavior::BACKWARD);
	MoveTo(pPax, ptDest ,eStartTime);

	ElapsedTime eNextTime = eStartTime;
    //move to entry position


	m_pEntryPoint->GetLocation( ptDest );
	MoveTo( pPax, ptDest, eNextTime );

	PaxOnboardBaseBehavior::MobDir enumDir = pPax->GetDirectionSidle(CPoint2008(),true);
	MoveTo( pPax, ptDest, eNextTime ,enumDir);
	//side walk to seat
	std::vector<OnboardSeatInSim *> vPassSeats = m_pSeatRow->GetSeatNeedGivePlace(pSeat,m_pEntryPoint);

	for (int nSeat = 0; nSeat < static_cast<int>(vPassSeats.size()); ++ nSeat)
	{
		OnboardSeatInSim *pPassSeat = vPassSeats[nSeat];
		if (pPassSeat == NULL)
			continue;

		//set destination
		pPassSeat->GetFrontPosition( ptDest);

		//move the location
		MoveTo(pPax, ptDest, eNextTime,enumDir);
	}

	//move to seat
	pSeat->GetFrontPosition( ptDest);
	MoveTo(pPax, ptDest, eNextTime, enumDir);

	MoveTo(pPax, ptDest, eNextTime,PaxOnboardBaseBehavior::BACKWARD);
	

	pSeat->GetPosition(ptDest);
	MoveTo(pPax, ptDest, eNextTime,PaxOnboardBaseBehavior::BACKWARD);


	pPax->setState(ArriveAtSeat);
	AddEvent(pPax,eNextTime);

	//processArriveAtSeat(pPax,eNextTime);
	////sit down
	//eNextTime+= ElapsedTime(1L);
	//pPax->setState(SitOnSeat);
	//pPax->writeLog(eNextTime);

	eStartTime = eNextTime;
}

void OnboardCellSeatGivePlaceLogic::MoveTo( PaxOnboardBaseBehavior *pPax,ARCVector3& ptDest ,ElapsedTime& eTime) const
{
	//set destination
	pPax->setDestination(ptDest);

	//calculate time
	eTime += pPax->moveTime();
	
	//reset location
//	pPax->setLocation(ptDest);
	
	//write log
	pPax->writeLog(eTime);


}

void OnboardCellSeatGivePlaceLogic::MoveTo( PaxOnboardBaseBehavior *pPax,ARCVector3& ptDest ,ElapsedTime& eTime, PaxOnboardBaseBehavior::MobDir enumDir ) const
{
		//set destination
	pPax->setDestination(ptDest);

	//calculate time
	eTime += pPax->moveTime();
	
	//reset location
//	pPax->setLocation(ptDest);
	
	//write log
	pPax->writeSidleLog(eTime,enumDir);
}














