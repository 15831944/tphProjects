#include "StdAfx.h"
#include ".\carryonvolumeinsim.h"
#include "../Common/ProDistrubutionData.h"



CarryonVolumeInSim::CarryonVolumeInSim(void)
{
}

CarryonVolumeInSim::~CarryonVolumeInSim(void)
{

}

double CarryonVolumeInSim::GetCarryOnVolume( EnumCarryonType cType ) const
{
	if(cType == DEFAULTCARRYON)
		return 100.0;

	CProDistrubution* pDist = m_lstVolumes.GetVolume(cType);
	if(pDist == NULL || pDist->GetProbDistribution() == NULL)
	{
		return 100.0;
	}

	return pDist->GetProbDistribution()->getRandomValue();
}

double CarryonVolumeInSim::GetCarryOnVolume( int nMobElementTypeIndex ) const
{
	EnumCarryonType cType = DEFAULTCARRYON;
	cType = GetCarryOnMapType(nMobElementTypeIndex);

	return GetCarryOnVolume(cType);
}

void CarryonVolumeInSim::Initialize( int nProjID )
{
	m_lstVolumes.ReadData(nProjID);
}