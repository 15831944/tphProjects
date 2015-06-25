#pragma once
#include "../InputAirside/IntersectionNode.h"
#include "../InputAirside/ALTObject.h"
#include "../InputAirside/ALTAirport.h"
#include "../InputAirside/IntersectionNodesInAirport.h"
//#pragma message("AirsideReport -> AirsideReportObjectIntersecNodeIDNameMap.h")


class AIRSIDEREPORT_API CAirsideReportObjectIntersecNodeIDNameMap
{
public:
	class AirportData
	{
	public:
		AirportData(){}
		~AirportData(){}

	public:
		ALTAirport m_airport;
		std::vector<ALTObject> m_lstRunway;
//		ALTObjectList m_lstTaxiway;
		std::vector<ALTObject> m_lstDeice;
		std::vector<ALTObject> m_lstStand;
		IntersectionNodesInAirport m_lstIntersecNode;
	private:
	};

	class AirspaceData
	{
	public:
		AirspaceData(){}
		~AirspaceData(){}
	public:

		std::vector<ALTObject> m_lstWaypoint;

	private:
	};

	class TopographyData
	{
	public:
		TopographyData(){}
		~TopographyData(){}
	protected:

	private:
	};


public:
	CAirsideReportObjectIntersecNodeIDNameMap(void);
	~CAirsideReportObjectIntersecNodeIDNameMap(void);



public:

	std::vector<AirportData> m_vAirportData;
	AirspaceData m_airspaceData;


public:
	
	void InitData(int ProjID);
	CString GetNameString(int nObjectID,int nIntersecNodeID);
protected:
	bool m_bLoaded;

};
