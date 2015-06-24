
#include "stdafx.h"
#include "LandsideStrategyStateInSim.h"
#include "Landside\LandsideCurbsideStrategy.h"
#include "ARCportEngine.h"
#include "LandsideSimulation.h"
#include "LandsideResourceManager.h"
#include "LandsideParkingLotInSim.h"

CurbsideStrategyStateInSim::CurbsideStrategyStateInSim( LandsideCurbsideSettingData* pdata )
{
	m_pData = pdata;
	m_nCurPriority = 0;
	m_bFinalState = false;
	m_nLoopedCount = 0;
	LandsideCurbsideVehicleSinglePriority* pPri = m_pData->GetItem(m_nCurPriority);
	if(pPri)
	{
		double randtime = pPri->GetProDist().GetProbDistribution()->getRandomValue();
		m_maxStopTime = ElapsedTime(randtime);
	}
}

LandsideLayoutObjectInSim* CurbsideStrategyStateInSim::GoNextPriority(CARCportEngine*pEngine)
{
	if(m_bFinalState)
		return NULL;

	if(m_nCurPriority < m_pData->GetCount()-1 )
	{
		m_nCurPriority++;		
	}
	else
	{
		m_nLoopedCount++;
		m_nCurPriority=0;		
		if(m_nLoopedCount > m_pData->GetFinalPriority().GetRecyleLimit() ) //final priority
		{
			m_bFinalState = true;
			ALTObjectID lotID= m_pData->GetFinalPriority().GetParkingLot();
			return pEngine->GetLandsideSimulation()->GetResourceMannager()->getRandomParkinglot(lotID);
		}
	}
	

	//get stop time
	LandsideCurbsideVehicleSinglePriority* pPri = m_pData->GetItem(m_nCurPriority);
	if(pPri)
	{
		double randtime = pPri->GetProDist().GetProbDistribution()->getRandomValue();
		m_maxStopTime = ElapsedTime(randtime);
		LandsideLayoutObjectInSim* pRet  = pEngine->GetLandsideSimulation()->GetResourceMannager()->getLayoutObjectInSim( pPri->GetStopResource());
		ASSERT(pRet);
		return pRet;
	}

	m_bFinalState = true;
	return NULL;
}

LandsideLayoutObjectInSim* CurbsideStrategyStateInSim::GoNextLoop( CARCportEngine* pEngine )
{
	if(m_bFinalState)
		return NULL;

	m_nLoopedCount++;
	m_nCurPriority=0;		
	if(m_nLoopedCount > m_pData->GetFinalPriority().GetRecyleLimit() ) //final priority
	{
		m_bFinalState = true;
		ALTObjectID lotID= m_pData->GetFinalPriority().GetParkingLot();
		return pEngine->GetLandsideSimulation()->GetResourceMannager()->getRandomParkinglot(lotID);
	}
	LandsideCurbsideVehicleSinglePriority* pPri = m_pData->GetItem(m_nCurPriority);
	if(pPri)
	{
		double randtime = pPri->GetProDist().GetProbDistribution()->getRandomValue();
		m_maxStopTime = ElapsedTime(randtime);
		LandsideLayoutObjectInSim* pRet  = pEngine->GetLandsideSimulation()->GetResourceMannager()->getLayoutObjectInSim( pPri->GetStopResource());
		ASSERT(pRet);
		return pRet;
	}
	m_bFinalState = true;
	return NULL;
}

