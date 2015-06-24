#include "StdAfx.h"
#include ".\vehiclestretchlaneinsim.h"
#include "../../Results/AirsideFlightEventLog.h"
#include "VehicleRoadIntersectionInSim.h"
#include "FlightRouteSegInSim.h"

VehicleStretchLaneInSim::VehicleStretchLaneInSim(VehicleStretchSegmentInSim * pStrech, int laneNo)
:VehicleLaneInSim(pStrech,laneNo)
{
}

VehicleStretchLaneInSim::~VehicleStretchLaneInSim(void)
{
}

AirsideResource::ResourceType VehicleStretchLaneInSim::GetType() const
{
	return AirsideResource::ResType_VehicleStretchLane;
}

CString VehicleStretchLaneInSim::GetTypeName() const
{
	return _T("VehicleLane");
}

CString VehicleStretchLaneInSim::PrintResource() const
{
	CString strName;  
	CString objStr = ((VehicleStretchSegmentInSim*)m_pParRoute)->GetStretch()->GetInput()->GetObjNameString();
	strName.Format("%s - %d ", _T(objStr), GetLaneNO());
	return strName;
}

void VehicleStretchLaneInSim::getDesc( ResourceDesc& resDesc )
{
	resDesc.fromnode = ((VehicleStretchSegmentInSim*)m_pParRoute)->GetNode1()->GetNodeInput().GetID();
	resDesc.tonode = ((VehicleStretchSegmentInSim*)m_pParRoute)->GetNode2()->GetNodeInput().GetID();
	resDesc.resid = ((VehicleStretchSegmentInSim*)m_pParRoute)->GetStretch()->GetInput()->getID();
	resDesc.resType = GetType();
	resDesc.strRes = PrintResource().GetString();
}

////////////////////////////////////////////////////////
VehicleTaxiLaneInSim::VehicleTaxiLaneInSim(FlightGroundRouteSegInSim * pSegment, int laneNo)
:VehicleLaneInSim(pSegment,laneNo)
{
}

VehicleTaxiLaneInSim::~VehicleTaxiLaneInSim(void)
{
}

AirsideResource::ResourceType VehicleTaxiLaneInSim::GetType() const
{
	return AirsideResource::ResType_VehicleTaxiLane;
}

CString VehicleTaxiLaneInSim::GetTypeName() const
{
	return _T("VehicleLane");
}

CString VehicleTaxiLaneInSim::PrintResource() const
{
	CString strName;  
	CString strNode1 = ((FlightGroundRouteSegInSim*)m_pParRoute)->GetNode1()->PrintResource();
	CString strNode2 = ((FlightGroundRouteSegInSim*)m_pParRoute)->GetNode2()->PrintResource();
	strName.Format("%s - %s(%d)",strNode1,strNode2,GetLaneNO());
	return strName;
}

void VehicleTaxiLaneInSim::getDesc( ResourceDesc& resDesc )
{
	resDesc.fromnode = ((FlightGroundRouteSegInSim*)m_pParRoute)->GetNode1()->GetNodeInput().GetID();
	resDesc.tonode = ((FlightGroundRouteSegInSim*)m_pParRoute)->GetNode2()->GetNodeInput().GetID();
	resDesc.resid = ((FlightGroundRouteSegInSim*)m_pParRoute)->GetGroundRoute()->GetId();
	resDesc.resType = GetType();
	resDesc.strRes = PrintResource().GetString();
}