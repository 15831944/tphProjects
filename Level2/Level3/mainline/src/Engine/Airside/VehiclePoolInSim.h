#pragma once

#include "AirsideResource.h"
#include "../../Common/point.h"
#include "../../InputAirside/VehiclePoolParking.h"
#include "vector"

class AirsideVehicleInSim;
class VehicleServiceRequest;
class VehiclePoolsManagerInSim;
class VehicleRequestDispatcher;
class VehicleLaneInSim;
class GeneralVehicleServiceRequest;
class CPaxBusServiceRequest;
class DeiceVehicleServiceRequest;
class TowTruckServiceRequest;
class AirsideFollowMeCarInSim;

class ENGINE_TRANSFER VehiclePoolInSim : public AirsideResource
{

public:
	typedef ref_ptr<VehiclePoolInSim> RefPtr;

	VehiclePoolInSim(VehiclePoolParking * pVehiclePool);


public:
	ResourceType GetType()const{ return ResType_VehiclePool; }
	virtual CString GetTypeName()const{ return _T("VehiclePool"); }
	virtual CString PrintResource()const ;
	void getDesc(ResourceDesc& resDesc);


	virtual CPoint2008 GetDistancePoint(double dist)const;

	CPoint2008 GetRandPoint();
	CPoint2008 GetCenterPoint();
	CPoint2008 GetParkingPos();

	bool IsPathOverlapPool(const CPath2008& path);

	int GetVehiclePoolID()const{ return m_pPoolInput->getID(); }
	VehiclePoolParking::RefPtr GetPoolInput(){ return m_pPoolInput; }

	void AddVehicleInPool(AirsideVehicleInSim* pVehicle);

	bool HandleServiceRequest(VehicleServiceRequest* request);

	bool HandleGeneralRequest(GeneralVehicleServiceRequest* request);	
	bool HandlePaxBusServiceRequest(CPaxBusServiceRequest* request);
	bool HandleDeiceServiceRequest(DeiceVehicleServiceRequest *request);
	bool HandleTowingServiceRequest(TowTruckServiceRequest* request);
	
	void SetVehiclePoolDeployment(VehiclePoolsManagerInSim* pPoolDeploy){ m_pPoolsDeployment = pPoolDeploy;}
	void SetVehicleRequestDispatcher(VehicleRequestDispatcher* pDispatcher){ m_pRequestDispatcher = pDispatcher;}


	//for vehicle ask for next mission
	void GetNextServiceRequest();

	int GetOutLaneCount()const{ return m_vOutLanes.size(); }
	void CallVehicleReturnPool();
	VehicleLaneInSim * GetOutLane(int idx){ return m_vOutLanes.at(idx); }
	void AddOutLane(VehicleLaneInSim  *pLane){ m_vOutLanes.push_back(pLane); }

	bool IsPoolCanServiceVehicle(int vehicleType);

	AirsideFollowMeCarInSim* getAvailableFollowMeCar();


private:
	VehiclePoolParking::RefPtr  m_pPoolInput;
	std::vector<AirsideVehicleInSim*> m_vVehicleList;
	VehiclePoolsManagerInSim* m_pPoolsDeployment;
	VehicleRequestDispatcher* m_pRequestDispatcher;

	std::vector<VehicleLaneInSim * > m_vOutLanes;

};
