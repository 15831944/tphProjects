#include "StdAfx.h"
#include ".\taxiwaydirectionlockdelay.h"
#include ".\AirsideFlightInSim.h"
#include ".\TaxiwayResource.h"
#include "../../Common/termfile.h"


void TaxiwayDirectionLockDelay::WriteDelayReason( ArctermFile& outFile )
{
	outFile.writeInt(m_nNodeFrom);
	outFile.writeInt(m_nNodeTo);
}

void TaxiwayDirectionLockDelay::ReadDelayReason( ArctermFile& inFile )
{
	long integer;
	inFile.getInteger(integer);
	m_nNodeFrom = integer;
	inFile.getInteger(integer);
	m_nNodeTo = integer;
}



bool TaxiwayDirectionLockDelay::IsIdentical( const AirsideFlightDelay* otherDelay ) const
{
	if(otherDelay && otherDelay->GetType() == GetType() )
		return true;
	return false;
}

TaxiwayDirectionLockDelay::TaxiwayDirectionLockDelay( AirsideFlightInSim* pFlight, TaxiwaySegDirectionLock* pLock ,const ElapsedTime& time )
{
	ASSERT(pLock);
	SetFlightID(pFlight->GetUID());
	SetStartTime(time);
	SetLockDirection(pLock);
	CString strDetail;
	CString strRes;
	/*if(pLock->GetLockDir() == PositiveDirection )
	{
		strRes = pLock->GetSegment()->GetNegativeDirSeg()->PrintResource();
	}
	else 
	{
		strRes = pLock->GetSegment()->GetPositiveDirSeg()->PrintResource();
	}*/
	//strDetail.Format("Dest TaxiSegment %s is Lock by other Flights ", strRes );
	SetDetailString(strDetail);
	SetDelayResult(FltDelayReason_Stop);
}

TaxiwayDirectionLockDelay::TaxiwayDirectionLockDelay()
{
	m_nNodeFrom = m_nNodeTo = -1;
	SetDelayResult(FltDelayReason_Stop);
}

void TaxiwayDirectionLockDelay::SetLockDirection( TaxiwaySegDirectionLock* pLock )
{
	if(pLock)
	{
		//m_nNodeFrom = pLock->GetSegment()->GetNode1()->GetNodeInput().GetID();
		//m_nNodeTo = pLock->GetSegment()->GetNode2()->GetNodeInput().GetID();
	/*	if(pLock->GetLockDir() == NegativeDirection )
		{
			std::swap(m_nNodeFrom, m_nNodeTo);
		}*/
	/*	m_vFlightIDs.clear();
		std::vector<AirsideFlightInSim*> vLockFlights = pLock->GetLockFlights();
		for(int i=0;i<(int)vLockFlights.size(); ++i)
		{
			AirsideFlightInSim* pFlight = vLockFlights[i];
			m_vFlightIDs.push_back(pFlight->GetUID());
		}*/
	}
}
//////////////////////////////////////////////////////////////////////////

void TaxiLeadFlightDelay::WriteDelayReason( ArctermFile& outFile )
{
	outFile.writeInt(m_nLeadFlightId);
}

void TaxiLeadFlightDelay::ReadDelayReason( ArctermFile& inFile )
{
	long integer;
	inFile.getInteger(integer);
	m_nLeadFlightId = integer;
}



bool TaxiLeadFlightDelay::IsIdentical( const AirsideFlightDelay* otherDelay ) const
{
	if(otherDelay && otherDelay->GetType() == GetType() )
		return true;
	return false;
}

void TaxiLeadFlightDelay::SetLeadFlight( AirsideFlightInSim* pFlight )
{
	m_nLeadFlightId = pFlight->GetUID();
}

TaxiLeadFlightDelay::TaxiLeadFlightDelay( AirsideFlightInSim* pleadflight, const ElapsedTime& tTime )
{
	SetLeadFlight(pleadflight);
	SetStartTime(tTime);
	CString strDetail;
	strDetail.Format("Flight is Blocked by the Lead Flight %d" , pleadflight->GetUID() );
	SetDetailString(strDetail);
	SetDelayResult(FltDelayReason_Stop);
}

TaxiLeadFlightDelay::TaxiLeadFlightDelay()
{
	m_nLeadFlightId = -1;
}
//////////////////////////////////////////////////////////////////////////

void TaxiIntersectNodeDelay::WriteDelayReason( ArctermFile& outFile )
{
	outFile.writeInt(m_nConflictNodeID);
	outFile.writeInt(m_nInNodeFlightID);
}

void TaxiIntersectNodeDelay::ReadDelayReason( ArctermFile& inFile )
{
	long interger;
	inFile.getInteger(interger);
	m_nConflictNodeID = interger;

	inFile.getInteger(interger);
	m_nInNodeFlightID = interger;
}


bool TaxiIntersectNodeDelay::IsIdentical( const AirsideFlightDelay* otherDelay ) const
{
	if(otherDelay && otherDelay->GetType() == GetType() )
		return true;
	return false;
}

void TaxiIntersectNodeDelay::SetIntersectionNode( IntersectionNodeInSim* pNode )
{	
	m_nConflictNodeID = pNode->GetNodeInput().GetID();
}

TaxiIntersectNodeDelay::TaxiIntersectNodeDelay( IntersectionNodeInSim* pNode, AirsideFlightInSim* pFlight, const ElapsedTime& tTime )
{
	SetFlightID(pFlight->GetUID());
	SetIntersectionNode(pNode);
	if(pNode && pNode->GetLastInResourceFlight())
	{
		SetInNodeFlight( pNode->GetLastInResourceFlight() );
	}	
	SetStartTime(tTime);
	CString strDetail;
	strDetail.Format("Dest Node %s is Used by other Flight", pNode->PrintResource() );
	SetDetailString(strDetail);
	SetDelayResult(FltDelayReason_Stop);
}

TaxiIntersectNodeDelay::TaxiIntersectNodeDelay()
{
	m_nConflictNodeID = -1;
	m_nInNodeFlightID = -1;
	SetDelayResult(FltDelayReason_Stop);
}

void TaxiIntersectNodeDelay::SetInNodeFlight( AirsideFlightInSim* pFlight )
{
	m_nInNodeFlightID = pFlight->GetUID();
}