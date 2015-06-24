#pragma once
#include "vector"
#include "../enginedll.h"
#include "common/ProbabilityDistribution.h"
#include "../../InputAirside/VehicleSpecifications.h"

class VehiclePoolInSim;
class CVehiclePoolsAndDeployment;
class CVehicleTypePools;
class CAirportDatabase;
class AirsideFlightInSim;
class CVehicleServiceTimeRange;
class AirsideMeetingPointInSim;

class ENGINE_TRANSFER VehiclePoolsManagerInSim
{
public:
	VehiclePoolsManagerInSim(void);
	~VehiclePoolsManagerInSim(void);

	void Init(int nPrjID,CAirportDatabase* pDatabase);
	int GetVehicleTypeCount();
	int GetVehicleTypeIDByIdx(int idx); 
	int GetVehicleQuantityInPool(int nVehicleTypeID, int nParkingLotID);
	CString GetVehicleTypeByID(int nID);

	void GetServicePool(AirsideFlightInSim* pFlight,int nVehicleTypeID,std::vector<int>& poolIDList);
	void GetPaxServicePool(AirsideFlightInSim* pFlight,int nVehicleTypeID,char mode,std::vector<int>& poolIDList);
	void GetFollowMeCarServicePool(AirsideMeetingPointInSim* pMeetingPoint, int nVehicleTypeID, std::vector<int>& vPoolIDs);
	void GetTowTruckServicePool(AirsideFlightInSim* pFlight,int nVehicleTypeID,std::vector<int>& poolIDList);
	//std::vector<int> GetServicePoolByBaseType(AirsideFlightInSim* pFlight, enumVehicleBaseType baseT);

	ProbabilityDistribution* GetTurnAroundTimeDistribution(int nVehicleTypeID, int nParkingLotID);
	CVehicleServiceTimeRange* GetVehicleServicePoolDeploy(int nVehicleTypeID, int nParkingLotID, AirsideFlightInSim* pFlight);
	CVehicleTypePools* GetVehicleServicePool(int nVehicleTypeID);
	//void GetVehicleServicePoolDeployByBaseType(int nVehicleTypeID, int nParkingLotID, AirsideFlightInSim* pFlight,std::vector<CVehicleServiceTimeRange* >& vTimeRange );

	//void GetServicePoolByBaseType(AirsideFlightInSim* pFlight,enumVehicleBaseType vehicleBaseType, std::vector<int>& poolIDList);
	bool IsExistServiceVehicles(int nVehicleNameID);

private:

	CVehiclePoolsAndDeployment* m_pVehiclePoolsAndDeployment;


};
