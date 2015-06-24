#pragma once

#include "../../InputAirside/stand.h"

class VehicleLaneInSim;
class StandInSim;
class AirsideFlightInSim;

class LaneStandIntersectionInSim
{
public:
	friend class VehicleLaneInSim;
public:


	LaneStandIntersectionInSim(void);
	~LaneStandIntersectionInSim(void);

	LaneStandIntersectionInSim(VehicleLaneInSim* pLane, StandInSim* pStand, int lineid, Stand::LeadLineType linetype, int idx);
	
	void SetHold1(DistanceUnit dist){ m_holdDist1 = dist; }
	void SetHold2(DistanceUnit dist){ m_holdDist2 = dist; }
	DistanceUnit GetHold1()const{ return m_holdDist1; }
	DistanceUnit GetHold2()const{ return m_holdDist2; }

	DistanceUnit GetIntersectDistAtLane()const{ return m_intersectDistAtLane; }
	DistanceUnit GetIntersectDistAtStand()const{ return m_intersectDistAtStandLine; }

	void SetIntersectDistAtLane(const DistanceUnit& dist){ m_intersectDistAtLane = dist; } 
	void SetIntersectDistAtStand(const DistanceUnit& dist){ m_intersectDistAtStandLine = dist; }

	VehicleLaneInSim * GetLane()const{ return m_pLane; }
	StandInSim * GetStand()const{ return m_pStand; }
	Stand::LeadLineType GetLeadLineType()const{ return m_leadlineType; }
	int GetLeadLineID()const{ return m_lineId; }
	int GetIntersectIdx()const{ return m_idx; }

	AirsideFlightInSim * GetLatestFlightInSight(const DistanceUnit& radius,const ElapsedTime&tTime);

	Point GetPosition()const;

protected:
	VehicleLaneInSim* m_pLane;
	StandInSim * m_pStand;
	int m_idx;      // intersection idx with lead line
	int m_lineId;   // lead line id
	Stand::LeadLineType m_leadlineType; //lead line type

	DistanceUnit m_holdDist1;
	DistanceUnit m_holdDist2;
	DistanceUnit m_intersectDistAtLane;
	DistanceUnit m_intersectDistAtStandLine;

};
