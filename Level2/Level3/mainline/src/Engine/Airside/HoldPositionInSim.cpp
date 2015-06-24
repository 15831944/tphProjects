#include "StdAfx.h"
#include ".\holdpositioninsim.h"

#include <algorithm>
#include ".\RunwayInSim.h"
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
