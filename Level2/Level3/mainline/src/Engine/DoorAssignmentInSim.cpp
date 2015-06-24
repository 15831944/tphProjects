#include "StdAfx.h"
#include ".\doorassignmentinsim.h"
#include "OnboardFlightInSim.h"
#include "OnboardSeatInSim.h"
#include "OnboardDoorInSim.h"
#include "..\Common\NewPassengerType.h"
#include "person.h"
#include "ARCportEngine.h"
#include "..\InputOnboard\OnBoardSeatBlockData.h"
#include "..\InputOnboard\Seat.h"


DoorOperationInSim::DoorOperationInSim(DoorOperationalData* pDoorOperationData)
:m_pDoorOperationData(pDoorOperationData)
{

}

DoorOperationInSim::~DoorOperationInSim()
{

}

bool DoorOperationInSim::fit( DoorOperationalData* pData ) const
{
	return pData == m_pDoorOperationData ? true : false;
}

void DoorOperationInSim::InitBlockData( OnboardFlightSeatBlock* pFlightSeatBlock )
{
	for (int i = 0; i < pFlightSeatBlock->GetItemCount(); i++)
	{
		OnboardSeatBloceData* pBlockData = pFlightSeatBlock->GetItem(i);
		if(m_pDoorOperationData->BlockFit(pBlockData))
			m_vSeatBlockData.push_back(pBlockData);
	}
}

bool DoorOperationInSim::valid( OnboardSeatInSim* pSeatInSim, Person* pPerson) const
{
	CPassengerType paxType(pPerson->getEngine()->getInStrDict());
	paxType.initPaxType(pPerson->getLogEntry().GetMobDesc());

	//check passenger type
	if(!m_pDoorOperationData->valid(paxType))
		return false;

	//check seat block
	const ALTObjectID& objID = pSeatInSim->m_pSeat->GetIDName();
	if (!BlockFit(objID))
		return false;

	return true;
}
bool DoorOperationInSim::BlockFit( const ALTObjectID& objID) const
{
	for (size_t i = 0; i < m_vSeatBlockData.size(); i++)
	{
		OnboardSeatBloceData* pBlockData = m_vSeatBlockData.at(i);
		if(pBlockData->fit(objID))
			return true;
	}
	return false;
}

ElapsedTime DoorOperationInSim::GetStartClosure()
{
	return m_pDoorOperationData->GetStartClosure();
}

ElapsedTime DoorOperationInSim::GetCloseOperationTime()
{
	return m_pDoorOperationData->GetCloseOperationTime();
}

DoorOperationalData::Status DoorOperationInSim::getStatus() const
{
	return m_pDoorOperationData->getStatus();
}

DoorAssignmentInSim::DoorAssignmentInSim(void)
{
}

DoorAssignmentInSim::~DoorAssignmentInSim(void)
{

}

void DoorAssignmentInSim::Initlizate( FlightDoorOperationalSpec* pDoorAssignment,OnboardFlightSeatBlock* pFlightSeatBlock, OnboardFlightInSim* pOnboardFlightInSim)
{
	ASSERT(pDoorAssignment);
	m_pDoorAssignment = pDoorAssignment;
	m_pOnboardFlightInSim = pOnboardFlightInSim;

	for (int i = 0; i < m_pDoorAssignment->getDoorCount(); i++)
	{
		DoorOperationalData* pData = m_pDoorAssignment->getOperationDataByIdx(i);
		DoorOperationInSim* pOperationInSim = new DoorOperationInSim(pData);
		pOperationInSim->InitBlockData(pFlightSeatBlock);
		addItem(pOperationInSim);
	}
}

OnboardDoorInSim* DoorAssignmentInSim::GetDoor(OnboardSeatInSim* pSeatInSim,Person* pPerson ) const
{
	std::vector<OnboardDoorInSim*> vDoorInSim;
	for (int i = 0; i < getCount(); i++)
	{
		DoorOperationInSim* pDataInSim = getItem(i);
		if (pDataInSim->valid(pSeatInSim,pPerson))
		{
			m_pOnboardFlightInSim->GetFitestDoor(pDataInSim,vDoorInSim);
		}
	}

	if (!vDoorInSim.empty())
	{
		return vDoorInSim.front();
	}

	return NULL;
}

void DoorAssignmentInSim::SetDoorOperationData( OnboardDoorInSim* pDoorInSim )
{
	DoorOperationInSim* pDataInSim = GetDoorOperationData(pDoorInSim);
	if (pDataInSim)
	{
		pDoorInSim->SetOperationData(pDataInSim);
	}
}

DoorOperationInSim* DoorAssignmentInSim::GetDoorOperationData( OnboardDoorInSim* pDoorInSim ) const
{
	DoorOperationalData* pDoorOperationData = m_pDoorAssignment->GetDoorOperationData(pDoorInSim->m_pDoor);
	for (int i = 0; i < getCount(); i++)
	{
		DoorOperationInSim* pDoorOperationInSim = getItem(i);
		if (pDoorOperationInSim->fit(pDoorOperationData))
		{
			return pDoorOperationInSim;
		}
	}
	return NULL;
}
