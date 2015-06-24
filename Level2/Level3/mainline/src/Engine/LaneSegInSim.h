#pragma once
#include "LandsideVehicleInSim.h"
#include "./LandsideStretchInSim.h"
//lane seg
class LaneSegInSim
{
public:
	LaneSegInSim(LandsideLaneInSim* pLane, double distF, double distT)
	{
		mpLane = pLane;
		mDistF = distF; //smaller
		mDistT = distT; //bigger
	}



	LandsideLaneInSim* mpLane;
	DistanceUnit mDistF;
	DistanceUnit mDistT;

	

	struct VehicleOcy{
		LandsideVehicleInSim* pVeh;
		double dPos;
		bool operator<(const VehicleOcy& othr)
		{
			return dPos > othr.dPos;
		}
	};
	typedef std::vector<VehicleOcy> VehicleOcyList;
	VehicleOcyList mOcyVehicles;

	bool FindFreePos(LandsideVehicleInSim* pVehicle,int& iLoop, double& dRangeF, double& dRangeT)const;
	bool FindPathToRange(LandsideVehicleInSim* pVehicle,LandsideLaneInSim* pFromLane, const CPoint2008& dFromPos, double dRangF, double dRangT,LandsideLaneNodeList& followPath);
	void removeVehicleOcy(LandsideVehicleInSim* pVeh);
	void addVehicleOcy(LandsideVehicleInSim* pVeh, double dPos);

};