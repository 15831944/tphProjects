#pragma once
#include "AirsideResource.h"
#include "../../Common/point.h"
#include "../../InputAirside/stand.h"
#include "TaxiwayResource.h"
#include ".\vehiclestretchinsim.h"

class ClearanceItem;
class Clearance;
class StandLeadInLineInSim;
class StandLeadOutLineInSim; 
class FlightTypeStandAssignment;
class CStandAssignmentRegisterInSim;
class StandCriteriaAssignment;
class VehicleLaneEntry;
class ENGINE_TRANSFER StandInSim : public AirsideResource, public CAirsideSubject,public RouteResource
{

public:
	typedef ref_ptr<StandInSim> RefPtr;

	StandInSim(Stand * pStand);

	void Init(IntersectionNodeInSimList& nodeList);
	void Clear();
	
public:
	ResourceType GetType()const{ return ResType_Stand; }
	virtual CString GetTypeName()const{ return _T("Stand"); }
	virtual CString PrintResource()const ;
	void getDesc(ResourceDesc& resDesc);

	virtual CPoint2008 GetDistancePoint(double dist)const;

	int GetLeadInLineCount()const{ return m_vLeadInLines.size(); }
	int GetLeadOutLineCount()const{ return m_vLeadOutLines.size(); }
	StandLeadInLineInSim* GetLeadInLineByIndex(int idx)const;
	StandLeadOutLineInSim* GetLeadOutLineByIndex(int idx)const;

	StandLeadInLineInSim* GetLeadInLineByMinDist() const;
	StandLeadOutLineInSim* GetLeadOutLineByMaxDist() const;

	IntersectionNodeInSim* GetMinDistInNode()const;
	IntersectionNodeInSim* GetMaxDistOutNode()const;

	int GetID()const{ return m_pStandInput->getID(); }
	Stand * GetStandInput(){ return m_pStandInput.get(); }

	bool FindClearanceInConcern(AirsideFlightInSim * pFlight,  ClearanceItem& lastItem ,ElapsedTime tParkingTime, Clearance& newClearance);
	bool GetExitStandClearance(AirsideFlightInSim * pFlight,  ClearanceItem& lastItem, Clearance& newClearance);
	bool GetEnterStandClearance(AirsideFlightInSim * pFlight,  ClearanceItem& lastItem,ElapsedTime tParkingTime, Clearance& newClearance);

	bool GetPullFromStandClearance(AirsideFlightInSim * pFlight,  ClearanceItem& lastItem, Clearance& newClearance);
	bool GetPushFromStandClearance(AirsideFlightInSim * pFlight,  ClearanceItem& lastItem, Clearance& newClearance);
	
	virtual void ReleaseLock(AirsideFlightInSim * pFlight, const ElapsedTime& tTime);
	virtual bool GetLock(AirsideFlightInSim * pFlight);
	virtual AirsideFlightInSim * GetLockedFlight();
	virtual bool TryLock(AirsideFlightInSim * pFlight);

	CStandAssignmentRegisterInSim* GetStandAssignmentRegister(){ return m_pStandAssignmentRegister; }
	
	int GetOutLaneCount()const{ return m_vOutLanes.size(); }
	VehicleLaneEntry* GetOutLane(int idx){ return m_vOutLanes.at(idx); }
	void AddOutLane(VehicleLaneEntry* laneEntry);

	StandLeadInLineInSim* AssignLeadInLine(AirsideFlightInSim* pFlight);
	StandLeadOutLineInSim* AssignLeadOutLine(AirsideFlightInSim* pFlight);
	bool IsStandResource(AirsideResource* pResource);

	FlightTypeStandAssignment* getFlightTypeStandAssignment(AirsideFlightInSim* pFlight);

	//set flight type assignment on each stand
	void SetFlightTypeOnEachStand(StandCriteriaAssignment* pStandCriteriaAssignment) {m_pStandCriteriaAssignment = pStandCriteriaAssignment;}

	bool IsPushbackStandForFlight(AirsideFlightInSim* pFlight);

	//
	virtual bool IsActiveFor(ARCMobileElement* pmob,const RouteDirPath* pPath)const;
	virtual bool bMayHaveConflict(const RouteDirPath *pPath)const;
	virtual bool bLocked(ARCMobileElement* pmob,const RouteDirPath* pPath)const;
	virtual bool bMobileInResource(ARCMobileElement* pmob)const;

	virtual StandInSim* IsStandResource()const;
protected:
	std::vector<VehicleLaneEntry*> m_vOutLanes;	
	Stand::RefPtr m_pStandInput;

	std::vector<StandLeadInLineInSim*> m_vLeadInLines;
	std::vector<StandLeadOutLineInSim*> m_vLeadOutLines;

	AirsideFlightInSim * m_pLockFlight;

	CStandAssignmentRegisterInSim* m_pStandAssignmentRegister;
	StandCriteriaAssignment* m_pStandCriteriaAssignment;
	bool m_bApplyForTowingService;

};


class ENGINE_TRANSFER DeiceStandInSim : public StandInSim
{
public:
	DeiceStandInSim(Stand * pStand);
	~DeiceStandInSim();

	void InitDefaultData();
};