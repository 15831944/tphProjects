#pragma once


#include "Clearance.h"
#include "VehicleStretchInSim.h"

class VehicleRouteResourceManager;




class VehicleRouteNode;	

class ENGINE_TRANSFER VehicleRouteInSim
{
public:
	VehicleRouteInSim();
	VehicleRouteInSim(const std::vector<VehicleRouteNodePairDist>& vroute);
	
	
	bool FindClearanceInConcern(AirsideVehicleInSim * pVehicle,ClearanceItem& lastItem, DistanceUnit radius, Clearance& newClearance);

	void SetMode(AirsideMobileElementMode mode){ m_mode = mode; }
	void SetCurItem(int i){ m_nCurItemIdx = i; }

	void CopyDataToRoute(VehicleRouteInSim& route);
	void AddData(const std::vector<VehicleRouteNodePairDist>& vroute);
	void AddData(const VehicleRouteNodePairDist& nodePair);
	void PopBack();
	void SetVehicleBeginPos(const CPoint2008& pos);

	bool IsEmpty() const;

	const AirsideResource* GetDestResource();
	const VehicleLaneInSim* GetFirstLaneResource();

	void ClearRouteItems();

	double GetLength()const;


protected:
	

protected:	

	std::vector<VehicleRouteNodePairDist> m_vRouteItems;
	AirsideMobileElementMode m_mode;
	int m_nCurItemIdx;
	int m_nNextItemIdx;

	//VehicleRouteResourceManager *m_pVehicleRouteResourceManager;
	
};
