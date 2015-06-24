#include "StdAfx.h"
#include ".\managedrunwaystrategyinsim.h"
#include "./InputAirside/TimeRangeRunwayAssignmentStrategyItem.h"
#include "RunwayInSim.h"
#include "RunwayResourceManager.h"

CManagedRunwayStrategyInSim::CManagedRunwayStrategyInSim(void)
{
}

CManagedRunwayStrategyInSim::~CManagedRunwayStrategyInSim(void)
{
}

LogicRunwayInSim* CManagedRunwayStrategyInSim::GetRunway(RunwayResourceManager* pRunwayRes)
{
	int nItemCount =int(m_vRunwayAssignmentPercentItems.size());
	int nRunwayID = -1, nRunwayMark = -1;
	LogicRunwayInSim* pLogicRunway = NULL;
	double dRandPercent = double(rand()%100);
	double dPercent = 0.0;
	for ( int i = 0; i< nItemCount; i++ )
	{
		if (dRandPercent>= dPercent && dRandPercent < dPercent + m_vRunwayAssignmentPercentItems[i]->m_dPercent)
		{
			nRunwayMark = m_vRunwayAssignmentPercentItems[i]->m_nRunwayMarkIndex;
			nRunwayID = m_vRunwayAssignmentPercentItems[i]->m_nRunwayID;
			pLogicRunway = pRunwayRes->GetLogicRunway(nRunwayID,RUNWAY_MARK(nRunwayMark));
			break;
		}
		dPercent += m_vRunwayAssignmentPercentItems[i]->m_dPercent;
	}
	
	return pLogicRunway;
}

bool CManagedRunwayStrategyInSim::IsAssignedRunway(int nRwyID, int nMark)
{
	int nItemCount =int(m_vRunwayAssignmentPercentItems.size());
	int nRunwayID = -1, nRunwayMark = -1;

	for ( int i = 0; i< nItemCount; i++ )
	{
		nRunwayMark = m_vRunwayAssignmentPercentItems[i]->m_nRunwayMarkIndex;
		nRunwayID = m_vRunwayAssignmentPercentItems[i]->m_nRunwayID;
		if (nRunwayID == nRwyID && nRunwayMark == nMark)
			return true;
	}

	return false;
}