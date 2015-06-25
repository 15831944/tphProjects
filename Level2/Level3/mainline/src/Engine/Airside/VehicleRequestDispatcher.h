#pragma once
#include "VehicleServiceRequest.h"

#include "TowTruckServiceRequest.h"
#include "vector"

using namespace std;


#include <vector>
#include <set>
#include "Common\CPPUtil.h"


class CAirportDatabase;
class VehiclePoolsManagerInSim;
class VehiclePoolResourceManager;
class VehicleServiceRequest;
class FlightServiceReqirementInSim;
class AirsideFlightInSim;
class CPaxBusParkingResourceManager;
class AirsideResource;
class FlightDeiceRequirment;
class CVehicleSpecifications;
class FollowMeCarConnectionInSim;
class AirsideFollowMeCarInSim;
class StandInSim;
enum FlightOperation;
class BaggageTrainServiceRequest;
class ProcessorList;
class CBagCartsParkingSpotResourceManager;

class VehicleServiceRequestList
{
public:
	void clear(){ datalist.clear();}
	void insert(VehicleServiceRequest* rq);
	bool remove(VehicleServiceRequest* rq){ return datalist.remove(rq); }
	bool isEmpty()const{ return datalist.empty(); }
	int getCount()const{ return (int)datalist.size(); }
	VehicleServiceRequest* getItem(int i){ return datalist.at(i); }
protected:
	cpputil::TPtrVector<VehicleServiceRequest> datalist;
};

class VehicleRequestDispatcher
{
public:
	VehicleRequestDispatcher(CAirportDatabase* pAirportDB, ProcessorList* pProcList);
	~VehicleRequestDispatcher(void);

	void AddServiceRequest(AirsideFlightInSim* pFlight);
	void AddTowingServiceRequest(TowTruckServiceRequest* pRequest);
	void AddPaxBusServiceRequest(AirsideFlightInSim* pFlight,bool bArrival);
	void AddDeiceServiceRequest(AirsideFlightInSim* pFlight,const FlightDeiceRequirment& deiceRequirment); //make deice request from the deicerequirment
	void AddBaggageTrainServiceRequest(AirsideFlightInSim* pFlight,FlightOperation enumOperation);

	void ServiceRequestDispatch();
	void PaxBusServiceRequestDispatch();
	void TowTruckServiceRequestDispatch();
	void BaggageTrainServiceRequestDispatch();

	void SetVehiclePoolResourceManager( VehiclePoolResourceManager* pResManager){ m_pPoolResManager = pResManager;}
	void SetVehiclePoolsDeployment(VehiclePoolsManagerInSim* pPoolsDeploy){ m_pPoolsDeployment = pPoolsDeploy;}
	void SetFlightServiceRequirement(FlightServiceReqirementInSim* pRequirement){ m_pFlightServiceRequirement = pRequirement;}
	void SetPaxBusParkingResourceManager(CPaxBusParkingResourceManager *pPaxParkingResourceManager);
	void SetBagCartsParkingSpotResourceManager(CBagCartsParkingSpotResourceManager *pBagCartsSpotResourceManager);

	bool IsVehicleService(){ return m_bVehicleService;}
	void SetVehicleService(bool bService){ m_bVehicleService = bService; }
	bool HasVehicleServiceFlight(AirsideFlightInSim* pFlight,enumVehicleBaseType vehicleType);
	bool AvailablePaxServiceFlight(AirsideFlightInSim* pFlight,char mode);
	bool HasTowTruckServiceFlight(AirsideFlightInSim* pFlight);

	bool HasBaggageTrainServiceFlight(AirsideFlightInSim* pFlight,const ElapsedTime& eTime);


	//it has to check is there vehicle can server this request
	bool AddServiceRequest(VehicleServiceRequest* pServiceReq);
	bool IsRequestCanBeHandled(VehicleServiceRequest* pServiceReq);

	ElapsedTime getVehicleAvaibleTime(AirsideFlightInSim* pFlight, AirsideResource* pDestRes, enumVehicleBaseType vehicleType);
	/*typedef std::set<VehicleServiceRequest*, VehicleServiceRequestOrderLess> VehicleServiceRequestList;*/

	FlightServiceReqirementInSim* GetServiceRequirment()const{return m_pFlightServiceRequirement; }

	bool UnDispatchTowTruckForFlight(AirsideFlightInSim* pFlight);
	void DeleteRequest(VehicleServiceRequest* pRequest);

	AirsideFollowMeCarInSim* AssignFollowMeCarForFlight(AirsideFlightInSim* pFlight,StandInSim* pDest);
	void WakeupAllPaxBusCompleteService(AirsideFlightInSim* pFlight);

private:
	bool IsTowingRequestHandledByPools(TowTruckServiceRequest* pRequest, std::vector<int>& vPoolIDs);


private:
	VehicleServiceRequestList m_vUnhandledList;
	std::vector<VehicleServiceRequest*> m_vPaxBusServiceRequest;
	std::vector<TowTruckServiceRequest*> m_vTowTruckServiceRequest;
	std::vector<BaggageTrainServiceRequest*> m_vBagTrainServiceRequest;
	VehiclePoolResourceManager* m_pPoolResManager;
	VehiclePoolsManagerInSim* m_pPoolsDeployment;
	FlightServiceReqirementInSim* m_pFlightServiceRequirement;
	CVehicleSpecifications* m_pVehicleSpecification;

	CPaxBusParkingResourceManager * m_pPaxParkingManager;
	FollowMeCarConnectionInSim* m_pFollowMeCarConnection;
	bool m_bVehicleService;

	ProcessorList* m_pTermProcList;
	
	CBagCartsParkingSpotResourceManager *m_pBagCartsSpotResourceManager;

	void RemoveBaggageServiceRequest(BaggageTrainServiceRequest*);

};
