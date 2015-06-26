#pragma once


enum LandsideReport_Detail_Summary
{
	LandsideReport_Detail = 0,
	LandsideReport_Summary
};

enum LandsideReportType
{
	LANDSIDE_REPOERT_UNKOWNTYPE = 0,

	LANDSIDE_VEHICLE_ACTIVITY,
	LANDSIDE_VEHICLE_DURATION,
	LANDSIDE_VEHICLE_DISTANCETRAVEL,
	LANDSIDE_VEHICLE_DELAY,
	LANDSIDE_VEHICLE_TOTALDELAY,

	LANDSIDE_PAX_ACTIVITY,
	LANDSIDE_PAX_DURATION,
	LANDSIDE_PAX_DISTANCETRAVEL,

	LANDSIDE_RESOURCE_DELAY,
	LANDSIDE_RESOURCE_THROUGHPUT,
	LANDSIDE_RESOURCE_QUEUELEN,



	//////////////////////////////////////////////////////////////////////////
	// warning ::  add new report type ,should also add the report name at the below name array .
	//////////////////////////////////////////////////////////////////////////
	LANDSIDE_REPORT_COUNT

};

const static CString LandsideReportName[] = 
{
	_T("UnknownType"),
		_T("Landside Vehicle Activity Report"),
		_T("Landside Vehicle Duration Report"),
		_T("Landside Vehicle Distance Travel Report"),
		_T("Landside Vehicle Delay Report"),
		_T("Landside Vehicle Total Delay Report"),

		_T("Landside Passenger Activity Report"),
		_T("Landside Passenger Duration Report"),
		_T("Landside Passenger Distance Travel Report"),

		_T("Landside Resource Delay Report"),
		_T("Landside Resource Throughput Report"),
		_T("Landside Resource Queue Length Report"),



	//////////////////////////////////////////////////////////////////////////
	// warning ::  add new report type ,should also add the report name at the below name array .
	//////////////////////////////////////////////////////////////////////////
	_T("INVLAID NAME"),
};





















