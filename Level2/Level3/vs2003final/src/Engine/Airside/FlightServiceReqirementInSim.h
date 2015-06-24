#pragma once
#include "VehicleServiceRequest.h"
#include <vector>
#include "TowTruckServiceRequest.h"

class CFlightServicingRequirementList;
class AirsideFlightInSim;
class CAirportDatabase;
class CVehicleSpecifications;
class CVehicleSpecificationItem;
class CServicingRequirement;
class FlightServiceReqirementInSim
{
public:
	FlightServiceReqirementInSim(void);
	~FlightServiceReqirementInSim(void);
	
	bool GetFlightGeneralVehicleServicingRequirement(AirsideFlightInSim* pFlight, std::vector<VehicleServiceRequest*>& requestList);
	void Init( int nPrjID, CAirportDatabase* pDatabase);

	bool GetAvailableVehicleTypeByBaseType(AirsideFlightInSim* pFlight,enumVehicleBaseType baseType,
		/*std::vector<CVehicleSpecificationItem *>& vVehicleTypeSpec,*/ std::vector<CServicingRequirement* >& vServiceRequirement);

	//check if have pax truck server for the flight 
    BOOL HavePaxTruckForFlight(AirsideFlightInSim* pFlight) ;


	std::vector<int> GetVehicleTypeIDByBaseType(enumVehicleBaseType baseType);

private:
	CFlightServicingRequirementList* m_pFlightServicingRequirementList;
	CVehicleSpecifications*          m_pVehicleSpecifications;
};
