#pragma once

enum AirsideThreeDChartType
{
	ASBar_2D = 0,
	ASLine_2D,
	ASBar_3D
};

enum reportType
{
	UnknownType				= 0,
	Airside_Legacy			= 1,
	Airside_DistanceTravel	= 2,
	Airside_Duration		= 3,
	Airside_FlightDelay		= 4,
	Airside_FlightActivity		= 5,
	Airside_AircraftOperation = 6,
	Airside_NodeDelay = 7,
	Airside_AircraftOperational = 8,
    Airside_VehicleOperation ,
	Airside_RunwayOperaitons,
	Airside_RunwayUtilization,
	Airside_FlightConflict,
	Airside_TaxiwayDelay,
	Airside_RunwayExit,
	Airside_RunwayDelay,
	Airside_RunwayCrossings,
	Airside_FlightFuelBurning,
	Airside_StandOperations,
	Airside_IntersectionOperation,
	Airside_TaxiwayUtilization,
	Airside_ControllerWorkload,//8+13
	Airside_TakeoffProcess,




	//////////////////////////////////////////////////////////////////////////
	// warning ::  add new report type ,should also add the report name at the below name array .
	//////////////////////////////////////////////////////////////////////////

	Airside_ReportCount
};

const static CString AirsideReportName[] = 
{
	_T("UnknownType"),//0
		_T("Airside Legacy Report"),//1
		_T("Airside Distance Travel Report"),//2
		_T("Airside Duration Report"),//3
		_T("Airside Flight Delay Report"),//4
		_T("Airside Flight Activity Report"),//5
		_T("Airside Aircraft Operation Report"),//6
		_T("Airside Node Delay Report"),//7
		_T("Airside Aircraft Operational Report"),//8
		_T("Airside Vehicle Operation Report"),//9
		_T("Airside Runway Operational Report"),//10
		_T("Airside Runway Utilization Report"),//11
		_T("Airside Flight Conflict Report"),//12
		_T("Airside Taxiway Delay Report"),//13
		_T("Airside Runway Exit Report"),
		_T("Airside Runway Delay Report"),
		_T("Airside Runway Crossings Report"),
		_T("Airside Flight Fuel Burn Report"),
		_T("Airsdie Stand Operations"),
		_T("Airside Intersection Operation Report"),
		_T("Airside Taxiway Utilization Report"),
		_T("Airside Controller Workload Report"),
		_T("Airside Takeoff Process Report")




};

//enum UM_LEN
//{
//	ASUM_LEN_KILOMETERS		= 0,
//	ASUM_LEN_METERS			= 1,
//	ASUM_LEN_CENTIMETERS	= 2,
//	ASUM_LEN_MILLIMETERS	= 3,
//	ASUM_LEN_MILE			= 4,
//	ASUM_LEN_FEET			= 5,
//	ASUM_LEN_INCHES			= 6,
//	ASUM_LEN_NAUTMILE		= 7
//};
