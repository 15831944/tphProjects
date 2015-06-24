#include "StdAfx.h"
#include ".\vehiclepoolresourcemanager.h"
#include "../../InputAirside/ALTAirport.h"

bool VehiclePoolResourceManager::Init(int nPrjId, int nAirportId)
{

	ALTObjectList vPools;
	m_vVehiclePools.clear();

	ALTAirport::GetVehiclePoolParkingList(nAirportId,vPools);

	for(int i = 0;i< (int)vPools.size(); ++i)
	{
		VehiclePoolParking * pPool =(VehiclePoolParking*) vPools.at(i).get();

		m_vVehiclePools.push_back( new VehiclePoolInSim(pPool));
	}

	return true;
}

VehiclePoolInSim* VehiclePoolResourceManager::GetVehiclePool(int nPoolId)
{
	for (int i =0; i < int(m_vVehiclePools.size()); i++)
	{
		if (m_vVehiclePools[i]->GetVehiclePoolID() == nPoolId)
			return m_vVehiclePools.at(i).get();
	}
	return NULL;
}
bool VehiclePoolResourceManager::IsVehicleCanServiced(int vehicleType)
{
	for (int i =0; i < int(m_vVehiclePools.size()); i++)
	{
		if (m_vVehiclePools[i]->IsPoolCanServiceVehicle(vehicleType))
			return true;
	}

	return false;
}
