#include "StdAfx.h"
#include ".\holdpositioninsim.h"

#include <algorithm>
#include ".\RunwayInSim.h"
#include "FlightGetClearanceEvent.h"
HoldPositionInSim::HoldPositionInSim( int nDBID ,int nTaxiwayID, int nNodeID, DistanceUnit distOffNode )
{
	m_nID = nDBID;
	m_nTaxiwayID = nTaxiwayID;
	m_nNodeID = nNodeID;
	m_dDistOffNode = distOffNode;
	m_pNode = NULL;

	m_pRunwayInSim = NULL;
	m_bQueueFull=false;
	m_bWaveCrossHold = false;
	m_nWaveCrossFlightCount = 0;
}





AirsideFlightInSim* HoldPositionInSim::GetFirstFlightOfQueue() const
{
	if(m_vInQueueFlights.size())
		return m_vInQueueFlights[0];
	return NULL;
}
#include "AirsideFlightInSim.h"
void HoldPositionInSim::AddFlightToQueue( AirsideFlightInSim* pFlight,  bool bMoveToHead)
{
	std::vector<AirsideFlightInSim*>::iterator iterFind = std::find(m_vInQueueFlights.begin(), m_vInQueueFlights.end(), pFlight);
	if(iterFind ==m_vInQueueFlights.end()	)
	{
		if(bMoveToHead)
			m_vInQueueFlights.insert(m_vInQueueFlights.begin(),pFlight);
		else
			m_vInQueueFlights.push_back(pFlight);

		CString strFlightID;
		pFlight->getFullID(strFlightID.GetBuffer(1024));
		TRACE(_T("WaveCrossing -----Add to Queue %d, %s, %s, %d     "),pFlight->GetUID(), strFlightID, pFlight->GetTime().printTime(), (long)(pFlight->GetTime()));
	}
	else
	{
		if(bMoveToHead)
		{
			m_vInQueueFlights.erase(iterFind);
			m_vInQueueFlights.insert(m_vInQueueFlights.begin(),pFlight);
		}

		
	}
}

void HoldPositionInSim::RemoveFlightInQueue( AirsideFlightInSim* pFlight )
{
	std::vector<AirsideFlightInSim*>::iterator itr = std::find(m_vInQueueFlights.begin(),m_vInQueueFlights.end(),pFlight);
	if(itr!=m_vInQueueFlights.end()){
        m_vInQueueFlights.erase( itr);
		SetQueueFull(false);
		m_pRunwayInSim->RemoveWaveCrossFlight(pFlight);

		CString strFlightID;
		pFlight->getFullID(strFlightID.GetBuffer(1024));
		TRACE(_T("WaveCrossing ----- Remove from Queue, %d, %s , %s, %d   "),pFlight->GetUID(), strFlightID, pFlight->GetTime().printTime(), (long)(pFlight->GetTime()));

	}
}

BOOL HoldPositionInSim::IsQueueFull()const
{
	return m_bQueueFull;
}

AirsideFlightInSim * HoldPositionInSim::GetFlightInQueue( int nIndex )
{
	if(nIndex<GetQueueLength() && nIndex>=0)
		return m_vInQueueFlights[nIndex];
	return NULL;
}

double HoldPositionInSim::GetTakeoffQueueLength() const
{
	double dQueueLength = 0.0;
	size_t nSize = m_vInQueueFlights.size();
	for(size_t i=0; i< nSize; i++)
	{
		AirsideFlightInSim* pFlight = m_vInQueueFlights.at(i);
		double dSepDist = 0.0;
		if (i != 0)
		{
			dSepDist = pFlight->getCurSeparation();
		}

		double dFlightLength = pFlight->GetLength();
		dQueueLength += (dSepDist + dFlightLength);
	}
	return dQueueLength;
}

void HoldPositionInSim::AddEnrouteWaitList( AirsideFlightInSim* pFlight )
{
	if(pFlight != NULL)
	{
		if(std::find(m_vEnrouteFlights.begin(),m_vEnrouteFlights.end(),pFlight) == m_vEnrouteFlights.end())//not find in list
			m_vEnrouteFlights.push_back(pFlight);
	}
}

void HoldPositionInSim::WakeupEnrouteWaitingFlight( const ElapsedTime& eTime )
{
	if (m_vEnrouteFlights.empty())
		return;
	
	AirsideFlightInSim* pFlight = m_vEnrouteFlights.front();

	FlightGetClearanceEvent *pEvent = new FlightGetClearanceEvent(pFlight);
	pEvent->setTime(eTime);
	pEvent->addEvent();
	
	m_vEnrouteFlights.erase(m_vEnrouteFlights.begin());
}
