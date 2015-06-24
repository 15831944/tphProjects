#include "StdAfx.h"
#include ".\pushbackclearanceinsim.h"
#include "InputAirside/CPushBackClearanceProcessor.h"
#include "common/AirportDatabase.h"
#include "RunwayInSim.h"
#include "AirsideFlightInSim.h"
#include "StandInSim.h"

PushBackClearanceInSim::PushBackClearanceInSim(void)
{
	//m_pPushBackClearance = new CPushBackClearanceProcessor;
}

PushBackClearanceInSim::~PushBackClearanceInSim(void)
{
}

void PushBackClearanceInSim::Init(int nPrjId, CAirportDatabase* pDatabase)
{
	m_pPushBackClearance = new CPushBackClearanceProcessor(nPrjId, pDatabase);
	m_pPushBackClearance->ReadData(nPrjId);
}

int PushBackClearanceInSim::GetMaxTakeoffQueueFlightCount(LogicRunwayInSim* pLogicRunway)
{
	if (pLogicRunway == NULL)
		return 1000;

	int nRunwayID = pLogicRunway->GetRunwayInSim()->GetRunwayInput()->getID();
	int nCount = m_pPushBackClearance->GetTakeoffQueueCount();
	for (int i =0; i < nCount; i++)
	{
		if (m_pPushBackClearance->GetTakeoffQueueRunwayID(i) == nRunwayID)
		{
			return m_pPushBackClearance->GetTakeoffQueueLengthAtRunway(i);
		}
	}

	return 1000;
}

int PushBackClearanceInSim::GetMaxDeicingQueueFlightCount()
{


	return 1000;
}

ElapsedTime PushBackClearanceInSim::GetMaxSlotTime(AirsideFlightInSim* pFlight)
{


	return 0L;
}

int PushBackClearanceInSim::GetMaxInboundFlightCount(StandInSim* pStand)
{
	ALTObjectID StandId = pStand->GetStandInput()->GetObjectName();
	int nCount = m_pPushBackClearance->GetCountOfStandForInboundFlt();
	for (int i =0; i < nCount; i++)
	{
		if (StandId.idFits(m_pPushBackClearance->GetStandForInboundFlt(i)))
		{
			return m_pPushBackClearance->GetInboundFltNumberAtStand(i);
		}
	}

	return 1000;
}

ALTObjectID  PushBackClearanceInSim::GetInboundStandFamily(StandInSim* pStand)
{
	ALTObjectID id;
	int nCount = m_pPushBackClearance->GetCountOfStandForInboundFlt();
	ALTObjectID StandId = pStand->GetStandInput()->GetObjectName(); 
	for (int i =0; i < nCount; i++)
	{
		if (StandId.idFits(m_pPushBackClearance->GetStandForInboundFlt(i)))
		{
			id = m_pPushBackClearance->GetStandForInboundFlt(i) ;
		}
	}

	return id;
}

int PushBackClearanceInSim::GetMaxOutboundToAssignedRunwayFlightCount()
{
	return m_pPushBackClearance->GetMaxOutboundFltToAssignedRunway();
}

int PushBackClearanceInSim::GetMaxOutboundFlightCount()
{
	return m_pPushBackClearance->GetMaxOutboundFltToAllRunway();
}

double PushBackClearanceInSim::GetClearanceDistanceOnTaxiway()
{
	return m_pPushBackClearance->GetDistancNoPushBackWithin();
}

ElapsedTime PushBackClearanceInSim::GetTimeClearanceOnTaxiway()
{
	return m_pPushBackClearance->GetTimeNoLeadWithin();
}