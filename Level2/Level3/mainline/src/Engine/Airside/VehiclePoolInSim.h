#pragma once

#include "AirsideResource.h"
#include "../../Common/point.h"
#include "../../InputAirside/VehiclePoolParking.h"
#include "VehicleStretchInSim.h"

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
class BaggageTrainServiceRequest;

enum dirtype{ _directional =1 , _bidirectional, };

class VehiclePoolLaneInSim;
class VehiclePoolItemInSim
{
public:
	virtual VehiclePoolLaneInSim* toLane(){ return NULL; }
};

class ParkingPoolNode
{
public:
	ParkingPoolNode(VehiclePoolItemInSim* itemF, DistanceUnit distF, VehiclePoolItemInSim* itemT, DistanceUnit distT)
	{
		m_from = itemF; m_fromDist = distF;
		m_to = itemT; m_toDist = distT;
	}
	VehiclePoolItemInSim * m_from;
	DistanceUnit m_fromDist;
	DistanceUnit m_toDist;
	VehiclePoolItemInSim* m_to;
	int m_idx;
};

//spots of the parking pool
class VehiclePoolParkSpotInSim : public VehiclePoolItemInSim
{
public:
	VehiclePoolParkSpotInSim(){ m_pOccupyVehicle  =NULL; }
	CPoint2008 m_pos;
	ARCVector3 m_dir;
	AirsideVehicleInSim* m_pOccupyVehicle;

	CPoint2008 m_entryPos;
	CPoint2008 m_exitPos;
	BOOL bPushback;
};

class VehiclePoolParkSpaceInSim
{
public:
	VehiclePoolParkSpaceInSim(const ParkingSpace& space);
	~VehiclePoolParkSpaceInSim(){ cpputil::deletePtrVector(m_vSpots); }
	int getSpotCount()const{ return (int)m_vSpots.size(); }
	VehiclePoolParkSpotInSim* getSpot(int idx){  return m_vSpots.at(idx); }
protected:
	std::vector<VehiclePoolParkSpotInSim*> m_vSpots;
};
//drive pipe
class VehiclePoolLaneInSim : public VehiclePoolItemInSim
{
public:
	VehiclePoolLaneInSim* toLane(){ return this; }
	const CPath2008& getPath()const{ return m_path; }
	DistanceUnit getWidth()const{ return m_dWidth; }
	CPath2008 m_path;
	DistanceUnit m_dWidth;
};

class VehiclePoolPipeInSim
{
public:
	VehiclePoolPipeInSim(const ParkingDrivePipe& dpipe,bool bLeftDrive );
	~VehiclePoolPipeInSim(){ cpputil::deletePtrVector(m_vLanes); } 
	int getLaneCount()const{ return (int)m_vLanes.size(); }
	VehiclePoolLaneInSim* getLane(int idx){ return m_vLanes.at(idx); }
	const CPath2008& getPath()const{ return m_path;	}
	DistanceUnit getWidth()const{ return m_pipeInput.m_width; }
	bool m_bLeftDrive;
	dirtype m_Dir;
protected:
	std::vector<VehiclePoolLaneInSim*> m_vLanes;
	CPath2008 m_path;
	ParkingDrivePipe m_pipeInput;
};

class VehiclePoolEntry : public VehicleLaneExit, public VehiclePoolItemInSim
{
public:
	VehiclePoolEntry(VehicleLaneInSim* pLane ,AirsideResource* pOrignRes, DistanceUnit atDist)
		:VehicleLaneExit(pLane,pOrignRes,atDist)
	{
		m_pPoolLane = NULL;
		m_distInPoolLane = 0; 
	}
	VehiclePoolLaneInSim* m_pPoolLane;
	DistanceUnit m_distInPoolLane;
};


class VehiclePoolExit : public VehicleLaneEntry,public VehiclePoolItemInSim
{
public:
	VehiclePoolExit(VehicleLaneInSim* pLane ,AirsideResource* pOrignRes, DistanceUnit atDist)
		:VehicleLaneEntry(pLane,pOrignRes,atDist)
	{
		m_pPoolLane = NULL;
		m_distInPoolLane = 0; 
	}
	VehiclePoolLaneInSim* m_pPoolLane;
	DistanceUnit m_distInPoolLane;
};


class CBoostPathFinder;


class ENGINE_TRANSFER VehiclePoolInSim : public AirsideResource
{

public:
	typedef ref_ptr<VehiclePoolInSim> RefPtr;

	VehiclePoolInSim(VehiclePoolParking * pVehiclePool);
	~VehiclePoolInSim();


public:
	ResourceType GetType()const{ return ResType_VehiclePool; }
	virtual CString GetTypeName()const{ return _T("VehiclePool"); }
	virtual CString PrintResource()const ;
	void getDesc(ResourceDesc& resDesc);


	virtual CPoint2008 GetDistancePoint(double dist)const;

	CPoint2008 GetRandPoint();
	CPoint2008 GetCenterPoint();

	//get the path parking to the spot
	CPath2008 ParkingToSpot(AirsideVehicleInSim* pV, VehicleRouteNode* pNode);
	void BirthOnPool(AirsideVehicleInSim* pV, CPoint2008& pos ,CPoint2008& dir);
	//get the path leave the pool
	CPath2008 LeavePool(AirsideVehicleInSim* pV);

	//bool IsPathOverlapPool(const CPath2008& path);

	int GetVehiclePoolID()const{ return m_pPoolInput->getID(); }
	VehiclePoolParking::RefPtr GetPoolInput(){ return m_pPoolInput; }

	void AddVehicleInPool(AirsideVehicleInSim* pVehicle);

	bool HandleServiceRequest(VehicleServiceRequest* request);

	bool HandleGeneralRequest(GeneralVehicleServiceRequest* request);	
	bool HandlePaxBusServiceRequest(CPaxBusServiceRequest* request);
	bool HandleDeiceServiceRequest(DeiceVehicleServiceRequest *request);
	bool HandleTowingServiceRequest(TowTruckServiceRequest* request);

	bool HandleBaggageTrainServiceRequest(BaggageTrainServiceRequest* pRequest);

	
	void SetVehiclePoolDeployment(VehiclePoolsManagerInSim* pPoolDeploy){ m_pPoolsDeployment = pPoolDeploy;}
	void SetVehicleRequestDispatcher(VehicleRequestDispatcher* pDispatcher){ m_pRequestDispatcher = pDispatcher;}


	//for vehicle ask for next mission
	void GetNextServiceRequest();
	void CallVehicleReturnPool();

	//int GetOutLaneCount()const{ return m_vOutLanes.size(); }
	//VehicleLaneInSim * GetOutLane(int idx){ return m_vOutLanes.at(idx); }
	//void AddOutLane(VehicleLaneInSim  *pLane){ m_vOutLanes.push_back(pLane); }

	bool IsPoolCanServiceVehicle(int vehicleType);

	AirsideFollowMeCarInSim* getAvailableFollowMeCar();


	void AddExit(VehiclePoolExit* pext)
	{
		m_vExits.push_back(pext);
	}
	void AddEntry(VehiclePoolEntry* pEnt)
	{
		m_vEntris.push_back(pEnt);
	}
	int GetEntryCount()const{ return (int)m_vEntris.size(); }
	int GetExitCount()const{ return (int)m_vExits.size(); }

	VehiclePoolPipeInSim* GetPipe(int idx)
	{ 
		return m_vpipes.at(idx);
	}
	int GetPipeCount()const{ return (int)m_vpipes.size(); }

	void Build(/*VehiclePoolParking* pPoolInput*/);
private:
	VehiclePoolParking::RefPtr  m_pPoolInput;
	std::vector<AirsideVehicleInSim*> m_vVehicleList;
	VehiclePoolsManagerInSim* m_pPoolsDeployment;
	VehicleRequestDispatcher* m_pRequestDispatcher;

	//std::vector<VehicleLaneInSim * > m_vOutLanes;



	//pool parking items
	std::vector<VehiclePoolEntry*> m_vEntris;  //ref
	std::vector<VehiclePoolExit*> m_vExits;  //ref


	std::vector<VehiclePoolPipeInSim*> m_vpipes;   //own
	std::vector<VehiclePoolParkSpaceInSim*> m_vParkSpaces; //own

	void AddNode(const ParkingPoolNode& node);
	VehiclePoolPipeInSim* getClosestPipe( const ARCPoint3& cpt, DistanceUnit& distInLane ) const;
	typedef std::vector<ParkingPoolNode> PoolNodeList;
	std::vector<ParkingPoolNode> m_vNodes; //for finding path;
	CBoostPathFinder * mpPathFinder;
	bool FindPath(VehiclePoolItemInSim* item, VehiclePoolItemInSim* itemT, PoolNodeList& path);

	VehiclePoolParkSpotInSim* getFreeSpot(AirsideVehicleInSim* pV);
};

