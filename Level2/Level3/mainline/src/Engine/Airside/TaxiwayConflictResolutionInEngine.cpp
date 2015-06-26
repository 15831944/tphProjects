#include "StdAfx.h"
#include ".\taxiwayconflictresolutioninengine.h"
#include "AirsideFlightInSim.h"
#include "..\..\InputAirside\ConflictAtIntersectionOnRight.h"
#include "..\..\InputAirside\ConflictFirstInATaxiway.h"
#include "..\..\InputAirside\ConflictOnSpecificTaxiways.h"
#include "Common\ARCUnit.h"
CTaxiwayConflictResolutionInEngine::CTaxiwayConflictResolutionInEngine(int nProjID,CAirportDatabase *pAirportDatabase)
:m_nProjID(nProjID)
{
	m_conflictSolution.SetAirportDB(pAirportDatabase);
	m_conflictSolution.ReadData(nProjID);
}

CTaxiwayConflictResolutionInEngine::~CTaxiwayConflictResolutionInEngine(void)
{
}
CConflictResolution * CTaxiwayConflictResolutionInEngine::GetConflictResolution()
{
	return &m_conflictSolution;
}
bool CTaxiwayConflictResolutionInEngine::IsDefault()
{
	if(m_conflictSolution.GetAtIntersectionOnRight() == Yes)
		return false;

	if (m_conflictSolution.GetFirstInATaxiway() == Yes)
		return false;

	if (m_conflictSolution.GetOnSpecificTaxiways() == Yes)
		return false;

	
	return true;

}
bool CTaxiwayConflictResolutionInEngine::IsConflictRightFlight(AirsideFlightInSim *pFlight,AirsideFlightInSim *pRightFlight)
{
	if(m_conflictSolution.GetAtIntersectionOnRight() == No)
	{
		CAtIntersectionOnRightList* vList = m_conflictSolution.GetAtIntersectionOnRightList();
		size_t nCount = vList->GetElementCount();

		for (size_t i =0; i < nCount; ++i )
		{
			if (pFlight->fits(vList->GetItem(i)->GetFltTypeB()) && pRightFlight->fits(vList->GetItem(i)->GetFltTypeA()))
			{
				return false;
			}
		}
	}

	return true;
}
bool CTaxiwayConflictResolutionInEngine::IsConflictNextTaxiway(AirsideFlightInSim *pFlight,AirsideFlightInSim *pRightFlight)
{
	if (m_conflictSolution.GetFirstInATaxiway() == Yes)
	{
		CFirstInATaxiwayList* vList = m_conflictSolution.GetFirstInATaxiwayList();
		size_t nCount = vList->GetElementCount();
		for (size_t i =0; i < nCount; ++i )
		{
			if (pFlight->fits(vList->GetItem(i)->GetFltTypeA()) && pRightFlight->fits(vList->GetItem(i)->GetFltTypeB()))
			{
				return true;
			}
		}
	}

	return false;
}
bool CTaxiwayConflictResolutionInEngine::IsConflictSpecifiedTaxiway(int nTaxiwayID,AirsideFlightInSim *pFlight,AirsideFlightInSim *pRightFlight)
{
	if (m_conflictSolution.GetOnSpecificTaxiways() == Yes)
	{
		COnSpecificTaxiwaysList* vList = m_conflictSolution.GetOnSpecificTaxiwaysList();
		size_t nCount = vList->GetElementCount();
		for (size_t i =0; i < nCount; ++i )
		{
			if (vList->GetItem(i)->GetTaxiwayID() == nTaxiwayID)
			{
				if (pRightFlight->fits(vList->GetItem(i)->GetFltType()))
				{
					return true;
				}
			}
		}
	}

	return false;
}


ElapsedTime CTaxiwayConflictResolutionInEngine::getCrossRunwayBufferTime( AirsideFlightInSim* pFligt, double dSpd, AirsideMobileElementMode mode )
{	
	CConflictResolution* conflictReslv = GetConflictResolution();
	for(size_t i=0;i<conflictReslv->m_vCrossBuffers.GetElementCount();i++)
	{
		RunwayCrossBuffer* buffer = conflictReslv->m_vCrossBuffers.GetItem(i);
		if( (buffer->getCase() == RunwayCrossBuffer::_Landing && mode == OnLanding)
			|| (buffer->getCase()== RunwayCrossBuffer::_Takeoff && mode == OnTakeoff) )
		{
			if(pFligt->fits(buffer->getFlightType()))
			{
				ElapsedTime  bufferTime = buffer->getTime();
				ElapsedTime distTime = ARCUnit::ConvertLength(buffer->getDistanceNM(), ARCUnit::NM, ARCUnit::CM)/dSpd;
				return MAX(bufferTime, distTime);
			}
		}
	}
	ElapsedTime tDefault( (double)conflictReslv->GetRunwayCrossBuffer());
	return tDefault;

}