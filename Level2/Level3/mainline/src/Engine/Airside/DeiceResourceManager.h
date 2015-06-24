#pragma once

#include "ResourceManager.h"

class DeicepadGroup;

class AirsideFlighQueue
{
public:
	typedef std::vector<AirsideFlightInSim*> TY_FlightList;

	void AddFlight(AirsideFlightInSim* pFlight, ElapsedTime time);

	void RemoveFlight(AirsideFlightInSim* pFlight, ElapsedTime time);

	int getLength()const{ return (int)m_vFlights.size(); }

	bool IsFlightInQueue(AirsideFlightInSim*pFlight)const;

	TY_FlightList m_vFlights;
	ElapsedTime m_lastTime;
};

#include <inputAirside/DeicePad.h>

class IntersectionNodeInSimList;
class IntersectionNodeInSim;
class VehicleLaneEntry;
class TaxiRouteInSim;

class ENGINE_TRANSFER DeicePadInSim : public AirsideResource
{
	DynamicClassStatic(DeicePadInSim)
	RegisterInherit(DeicePadInSim,AirsideResource)

public:
	DeicePadInSim(DeicePad* pInput);
	~DeicePadInSim();
	//get queue length below
	ElapsedTime getAvaileWithinTime();
	AirsideFlighQueue m_queue;

	AirsideFlighQueue& GetQueue(){ return m_queue; }

	ElapsedTime avgHandleTime; //time a flight service in this pad
	void updateAvgHandleTime(); //update average service time 
	DeicePad* GetInput();

	ResourceType GetType()const{ return ResType_DeiceStation; }
	virtual CString GetTypeName()const{ return _T("DeicePad"); }
	virtual CString PrintResource()const ;
	void getDesc(ResourceDesc& resDesc);
	virtual CPoint2008 GetDistancePoint(double dist)const;

	const CPath2008& GetInPath()const{ return mInPath; }
	const CPath2008& GetOutPath()const{ return mOutPath; }

	IntersectionNodeInSim* m_pEntryNode;
	IntersectionNodeInSim* m_pExitNode;
	void InitRelateNodes(IntersectionNodeInSimList& nodeList);
	bool IsEntryExitNodeValid() const { return m_pEntryNode && m_pExitNode; }

	void SetExitTime(CAirsideMobileElement * pFlight, const ElapsedTime& exitT);	

	int GetOutLaneCount()const{ return m_vOutLanes.size(); }
	VehicleLaneEntry* GetOutLane(int idx){ return m_vOutLanes.at(idx); }
	void AddOutLane(VehicleLaneEntry* laneEntry);

	void SetGroup(DeicepadGroup* pGroup) { m_pGroup = pGroup; }
	AirsideFlightInSim* GetFlightOnto() const { return m_pFlightOnto; }
	void SetFlightOnto(AirsideFlightInSim* pFlight) { m_pFlightOnto = pFlight; }

	TaxiRouteInSim* GetApproachingRoute() const { return m_pApproachingRoute; }
	void SetApproachingRoute(TaxiRouteInSim* pRoute);

	//************************************
	// Method:    OnFlightLeaving
	// FullName:  DeicePadInSim::OnFlightLeaving
	// Access:    public 
	// Returns:   AirsideFlightInSim*
	// Qualifier: call it when the flight served is going to leaving, return the next AirsideFlightInSim
	//************************************
	AirsideFlightInSim* OnFlightLeaving(AirsideFlightInSim * pFlight, const ElapsedTime& exitT);

protected:
	DeicePad::RefPtr m_Input;
	CPath2008 mInPath;
	CPath2008 mOutPath;
	std::vector<VehicleLaneEntry*> m_vOutLanes;

	DeicepadGroup* m_pGroup;
	AirsideFlightInSim* m_pFlightOnto;
	TaxiRouteInSim* m_pApproachingRoute;
};

class ALTObjectGroup;
class TaxiwayResourceManager;
class DeicepadGroup;

class ENGINE_TRANSFER DeiceResourceManager : public ResourceManager
{
friend class AirportResourceManager;
public:
	DeiceResourceManager();
	~DeiceResourceManager();

	bool Init(int nPrjID, int nAirportID);
	bool InitRelations(IntersectionNodeInSimList& nodeList,TaxiwayResourceManager * pTaxiwayRes);
// 	DeicePadInSim* AssignDeicePad(const ALTObjectGroup& deicGrp, int nMaxQueueLen,AirsideFlightInSim* pFlt);
	int GetCount()const{ return (int)m_vDeicePad.size(); }
	DeicePadInSim* GetPadByIndex(int idx)const;
	DeicePadInSim* GetPadByID(int id);
	DeicePadInSim* GetPadByName(const ALTObjectID& idName);

	//************************************
	// Method:    AssignGroup
	// FullName:  DeiceResourceManager::AssignGroup
	// Access:    public 
	// Returns:   DeicepadGroup*
	// Qualifier: Assign one group for the flight to get into
	// Parameter: const ALTObjectGroup & deicGrp
	// Parameter: int nMaxQueueLen
	// Parameter: AirsideFlightInSim * pFlt
	//************************************
	DeicepadGroup* AssignGroup(const ALTObjectGroup& deicGrp, int nMaxQueueLen, AirsideFlightInSim* pFlt);

protected:
	std::vector<DeicePadInSim*> m_vDeicePad;

	typedef std::vector<DeicepadGroup*> DeicepadGroupList;
	DeicepadGroupList m_vDeicepadGroups;
};
