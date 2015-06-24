#pragma once
#include "../../Common/ARCVector.h"
#include "../EngineDll.h"
#include "AirsideResource.h"
#include <vector>
#include <set>
#include "AirportResource.h"


class InputAirside;
class CAirsideInput;

class AirWayPoint;
class AirHold;
class ARWaypoint;
class CAirRoute;
class FlightInAirsideSimulation;

using namespace std;

namespace ns_AirsideInput{
 class CFlightPlanRoute;
}

//engine hold
class hold: public AirsideResource
{ 
public:

	AirHold * m_pHold;
};

typedef ResContainer< hold > holdList;  
typedef holdList::iterator holdIter;

class AirRouteSeg : public AirsideResource
{
public:		
	AirRouteSeg(){ m_pfix1 = NULL; m_pfix2 = NULL;}
	AirRouteSeg(ARWaypoint * pfix1,ARWaypoint * pfix2 ):m_pfix1(pfix1),m_pfix2(pfix2){}

	ARWaypoint * m_pfix1;
	ARWaypoint * m_pfix2;

	inline bool operator==(const AirRouteSeg& lffix)const 
	{
		if( m_pfix1==lffix.m_pfix1 && m_pfix2 == lffix.m_pfix2 ) 
			return true;
		else
			return false;
	}
};	
typedef ResContainer<AirRouteSeg> AirRouteSegList;	typedef AirRouteSegList::iterator fixIter;

//engine air route
class airroute : public AirsideResource
{
public:	
	enum RouteType{  STAR = 0, SID, EN_ROUTE ,MIS_APPROACH, TYPECOUNT, };
	//std::vector<ARWaypoint*> m_vfixes;
	AirRouteSegList m_vAirRouteSeg;
	int m_RouteType;

	CString m_RwID;				//The runway ID related with the air route
	CString m_strID;

	AirRouteSeg* getAirRouteSeg(const CString& strStartID,const CString& strEndID)
	{
		return m_vAirRouteSeg.getResource(strStartID,strEndID);
	}

};
typedef ResContainer<airroute> routeNetwork;  typedef routeNetwork::iterator routeIter;

//Abstraction of the airspace of the airport 
class ENGINE_TRANSFER AirspaceInSim
{

public:	
	AirspaceInSim(AirportResource* _airport): m_pAirport(_airport){ m_pNextResource = new AirsideResource;}
	~AirspaceInSim(){delete m_pNextResource; m_pNextResource = NULL;}

public:

	//build the abstract model from the Input;
	bool Build();


	//get airroute
	airroute* getAirRoute(const CString& strID)
	{
		return m_vroutes.getResource(strID);
	}

	AirsideResource * AllocateNextResource( FlightInAirsideSimulation * pFlight);

	void Clear();

	bool GetFlightPlanRoute(FlightInAirsideSimulation * pFlight);

	CString GetFlightPlanRunway(FlightInAirsideSimulation * pFlight);

private:
	//pointer to the airport
	AirportResource * m_pAirport;
	//
	routeNetwork m_vroutes;
	holdList m_vholds;
	std::vector<ns_AirsideInput::CFlightPlanRoute*> m_vPlanAirRoute;
	AirsideResource* m_pNextResource;

};

