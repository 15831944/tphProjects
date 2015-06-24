#pragma once

#include "VehiclePoolInSim.h"
#include "ResourceManager.h"



class ENGINE_TRANSFER VehiclePoolResourceManager : public ResourceManager 
{
public:
	friend class AirportResourceManager;

public:

	bool Init(int nPrjId, int nAirportId);

	//init relations with other resources;
	bool InitRelations( );

	VehiclePoolInSim * GetVehiclePool(int nPoolId);

	VehiclePoolInSim * GetVehiclePoolByIdx(int idx){ return m_vVehiclePools.at(idx).get(); }

	int GetVehiclePoolCount() const{ return m_vVehiclePools.size(); }

	bool IsVehicleCanServiced(int vehicleType);


protected:
	std::vector<VehiclePoolInSim::RefPtr> m_vVehiclePools;
};
