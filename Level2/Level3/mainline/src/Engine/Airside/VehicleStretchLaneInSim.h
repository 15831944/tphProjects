#pragma once
#include "VehicleStretchInSim.h"

class VehicleStretchSegmentInSim;
class FlightGroundRouteSegInSim;
class VehicleStretchLaneInSim : public VehicleLaneInSim
{
public:
	VehicleStretchLaneInSim(VehicleStretchSegmentInSim * pStrech, int laneNo);
	~VehicleStretchLaneInSim(void);

	virtual ResourceType GetType()const;
	virtual CString GetTypeName()const;

	virtual CString PrintResource()const ;
	virtual void getDesc(ResourceDesc& resDesc);


};

////////////////////////////////////
class VehicleTaxiLaneInSim : public VehicleLaneInSim
{
public:
	VehicleTaxiLaneInSim(FlightGroundRouteSegInSim* pSegment, int laneNo);
	~VehicleTaxiLaneInSim(void);

	virtual ResourceType GetType()const;
	virtual CString GetTypeName()const;

	virtual CString PrintResource()const ;
	virtual void getDesc(ResourceDesc& resDesc);
};
