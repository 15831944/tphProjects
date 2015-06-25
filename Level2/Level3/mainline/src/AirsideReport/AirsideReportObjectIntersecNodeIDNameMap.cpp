#include "StdAfx.h"
#include ".\airsidereportobjectintersecnodeidnamemap.h"
#include "../InputAirside/InputAirside.h"
CAirsideReportObjectIntersecNodeIDNameMap::CAirsideReportObjectIntersecNodeIDNameMap(void)
:m_bLoaded(false)
{
}

CAirsideReportObjectIntersecNodeIDNameMap::~CAirsideReportObjectIntersecNodeIDNameMap(void)
{
}
void CAirsideReportObjectIntersecNodeIDNameMap::InitData(int ProjID)
{
	return;
	if (m_bLoaded)
		return;

	std::vector<int> vAirportIds;
	InputAirside::GetAirportList(ProjID, vAirportIds);
	for (std::vector<int>::iterator iterAirportID = vAirportIds.begin(); iterAirportID != vAirportIds.end(); ++iterAirportID)
	{
		AirportData airportData;
		airportData.m_airport.ReadAirport(*iterAirportID);
		
		ALTObject::GetObjectList(ALT_RUNWAY, *iterAirportID, airportData.m_lstRunway);
		ALTObject::GetObjectList(ALT_STAND, *iterAirportID, airportData.m_lstStand);
		ALTObject::GetObjectList(ALT_DEICEBAY, *iterAirportID, airportData.m_lstDeice);
		airportData.m_lstIntersecNode.ReadData(*iterAirportID);

		m_vAirportData.push_back(airportData);
	}

	ALTObject::GetObjectList(ALT_WAYPOINT, ProjID, m_airspaceData.m_lstWaypoint);

	m_bLoaded = true;

	
}
CString CAirsideReportObjectIntersecNodeIDNameMap::GetNameString(int nObjectID,int nIntersecNodeID)
{
	for (size_t nAirport = 0; nAirport< m_vAirportData.size(); ++nAirport)
	{
		//search runway
		 m_vAirportData[nAirport].m_lstRunway.size();
	}
return "";
}