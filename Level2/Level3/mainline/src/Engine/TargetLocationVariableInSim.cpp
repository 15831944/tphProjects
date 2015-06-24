#include "StdAfx.h"
#include ".\targetlocationvariableinsim.h"
#include "../Common/ProDistrubutionData.h"
#include "../common/arcunit.h"

TargetLocationVariableInSim::TargetLocationVariableInSim(void)
{
}

TargetLocationVariableInSim::~TargetLocationVariableInSim(void)
{
}

double TargetLocationVariableInSim::GetOffsetDist(const CPassengerType& paxType ) const
{
	CTargetLocationsItem* pItem = m_targetLocation.GetData(paxType);
	if(pItem ==NULL)
		return 0.0;

	//
	double dRandom = random(100);
	if(dRandom < pItem->getOnTarget())//on target
	{
		return 0.0;
	}
	else if(dRandom < pItem->getOnTarget() + pItem->getOverShoot())//over target
	{
		double dDist = pItem->getOffTargetDist().GetProbDistribution()->getRandomValue();
		return  ARCUnit::ConvertLength(dDist, ARCUnit::M, ARCUnit::CM);
	}
	else//
	{
		double dDist = pItem->getOffTargetDist().GetProbDistribution()->getRandomValue();
		return  ARCUnit::ConvertLength(dDist, ARCUnit::M, ARCUnit::CM)*(-1);
	}

	return 0.0;
}

double TargetLocationVariableInSim::GetSearchRadius(const CPassengerType& paxType ) const
{
	CTargetLocationsItem* pItem = m_targetLocation.GetData(paxType);
	if(pItem ==NULL)
		return 150.0;

	double dDist = pItem->getSearchRadiusDist().GetProbDistribution()->getRandomValue();
	return ARCUnit::ConvertLength(dDist, ARCUnit::M, ARCUnit::CM);

}

void TargetLocationVariableInSim::Initialize( int nProjID, StringDictionary *pStrDict )
{
	m_targetLocation.ReadData(nProjID, pStrDict);
}