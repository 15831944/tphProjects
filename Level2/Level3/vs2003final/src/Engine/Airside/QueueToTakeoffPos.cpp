#include "StdAfx.h"
#include "../EngineDll.h"
#include "../../Common/elaptime.h"
#include "AirsideFlightInSim.h"
#include ".\queuetotakeoffpos.h"

/************************************************************************/
/*                                                                      */
/************************************************************************/
InQueueItem::InQueueItem( AirsideFlightInSim* pFlight, const ElapsedTime& enterTime )
{
	m_pFlight = pFlight;
	m_enterTime = enterTime;
}
void QueueToTakeoffPos::AddInQueueFlight( AirsideFlightInSim* pFlight, const ElapsedTime& enterTime )
{
	if(!IsFlightInQueue(pFlight))
		m_vInQueueItems.push_back(InQueueItem(pFlight,enterTime));
}


/************************************************************************/
/*                                                                      */
/************************************************************************/
ElapsedTime QueueToTakeoffPos::GetAverageQueueTime( const ElapsedTime& curTime ) const
{
	if(GetItemCount() == 0 )
		return ElapsedTime(0L);

	ElapsedTime totalTime;
	for(int i=0;i< GetItemCount();i++)
	{
		totalTime += (curTime - ItemAt(i).GetEnterTime() );
	}
	return totalTime / (long)GetItemCount();
}

bool QueueToTakeoffPos::IsFlightInQueue(AirsideFlightInSim* pFlight) const
{
	for(int i=0;i<GetItemCount();i++)
	{
		if( ItemAt(i).GetFlight() == pFlight )
			return true;
	}
	return false;
}

void QueueToTakeoffPos::RemoveInQueueFlight( AirsideFlightInSim* pFlight )
{
	typedef std::vector<InQueueItem>::iterator ItemIterator;

	for( ItemIterator itr=  m_vInQueueItems.begin();itr!= m_vInQueueItems.end();itr++)
	{
		if( itr->GetFlight() == pFlight )
		{
			m_vInQueueItems.erase(itr);
			break;
		}
	}
	if (m_pHeadFlight == pFlight)
		m_pHeadFlight = NULL;
}

#include ".\FlightRouteSegInSim.h"
#include ".\RunwayInSim.h"
AirsideFlightInSim* QueueToTakeoffPos::GetHeadFlight(const ElapsedTime& tCurTime)
{
	//FlightGroundRouteDirectSegInSim* pSeg = m_pTakeoffPos->GetRouteSeg()->GetOppositeSegment();
	//std::vector<CAirsideMobileElement*> vElms = pSeg->GetInResouceMobileElement();
	//for(int i=0;i<(int)vElms.size();i++)
	//{
	//	CAirsideMobileElement* pElm = vElms.at(i);
	//	if( pElm->GetType()==CAirsideMobileElement::AIRSIDE_FLIGHT )
	//	{
	//		AirsideFlightInSim* pFlight = (AirsideFlightInSim*)pElm;
	//		if(IsFlightInQueue(pFlight))
	//			return pFlight;
	//	}
	//}
	////if (m_pHeadFlight)
	////	return m_pHeadFlight;

	////if (!m_vInQueueItems.empty())
	////	return m_vInQueueItems[0].GetFlight();
	if (m_pHeadFlight && m_pHeadFlight->GetTime() > tCurTime)
		return NULL;

	return m_pHeadFlight;
}

ElapsedTime QueueToTakeoffPos::GetFlightEnterTime(AirsideFlightInSim* pFlight)
{
	int nCount = GetItemCount();
	for(int i=0;i<nCount;i++)
	{
		if( m_vInQueueItems[i].GetFlight() == pFlight )
			return m_vInQueueItems[i].GetEnterTime();
	}
	return -1L;
}

ElapsedTime QueueToTakeoffPos::GetQueueTime() const
{
	ElapsedTime earlTime = ElapsedTime::Max();
	for(int i=0;i<GetItemCount();i++)
	{
		if( ItemAt(i).GetEnterTime() < earlTime )
		{
			earlTime = ItemAt(i).GetEnterTime();
		}			
	}
	return earlTime;
}

int QueueToTakeoffPos::GetAheadFlightInQ(ElapsedTime tEnterTime) const
{
	int nFltCount = 0;
	int nSize = GetItemCount();
	for(int i=0; i< nSize; i++)
	{
		if( ItemAt(i).GetEnterTime() < tEnterTime )
		{
			nFltCount++;
		}			
	}

	return nFltCount;
}
/************************************************************************/
/*                                                                      */
/************************************************************************/
QueueToTakeoffPosList::~QueueToTakeoffPosList()
{
	for(int i=0;i<(int)m_vQueues.size();i++)
	{
		delete m_vQueues[i];
	}
}

QueueToTakeoffPos* QueueToTakeoffPosList::GetAddQueue( RunwayExitInSim* pTakeoffPos )
{
	for(int i=0;i<(int)m_vQueues.size();i++)
	{
		QueueToTakeoffPos* pQueue = m_vQueues[i];
		if( pQueue->GetTakeoffPos() == pTakeoffPos )
			return pQueue;
	}
	QueueToTakeoffPos * newQueue = new QueueToTakeoffPos(pTakeoffPos);
	m_vQueues.push_back(newQueue);
	return newQueue;
}

int QueueToTakeoffPosList::GetAllFlightCount() const
{
	int nCount= 0;
	for(int i=0;i< (int)m_vQueues.size();i++)
	{
		QueueToTakeoffPos* pQueue = m_vQueues.at(i);
		nCount += pQueue->GetItemCount();
	}
	return nCount;
}

ElapsedTime QueueToTakeoffPosList::GetAvgInQueueTime(const ElapsedTime& tTime) const
{
	ElapsedTime totalTime;
	int nCont = 0;
	for(int i=0;i< (int)m_vQueues.size();i++)
	{
		QueueToTakeoffPos * pQueue = m_vQueues.at(i);
		for(int j=0;j<(int)pQueue->GetItemCount();j++)
		{
			totalTime += ( tTime - pQueue->ItemAt(j).GetEnterTime() );
		}
		nCont += pQueue->GetItemCount();
	}
	return totalTime/(long)nCont;

}

void QueueToTakeoffPosList::RemoveFlight( AirsideFlightInSim*pFlight )
{
	for(int i=0;i< (int)m_vQueues.size();i++)
	{
		QueueToTakeoffPos* pQueue = m_vQueues.at(i);
		pQueue->RemoveInQueueFlight(pFlight);
	}
}