#pragma once
#include "../InputOnBoard/CarryonVolumeList.h"

class CarryonVolumeInSim
{
public:
	CarryonVolumeInSim(void);
	~CarryonVolumeInSim(void);


public:
	void Initialize(int nProjID);

	double GetCarryOnVolume(int nMobElementTypeIndex) const;
protected:
	double GetCarryOnVolume( EnumCarryonType cType ) const;


protected:
	CCarryonVolumeList m_lstVolumes;
};
