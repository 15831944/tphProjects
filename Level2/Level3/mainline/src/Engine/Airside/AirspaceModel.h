#ifndef __AIRSIDE_AIRSPACEMODEL_DEF
#define  __AIRSIDE_AIRSPACEMODEL_DEF

#include "../../Common/ARCVector.h"

#include "../EngineDll.h"
#include "../EngineConfig.h"
#include "Entity.h"
#include <vector>
#include <set>
#include "../SimClock.h"

class AirsideInput;
class CAirsideInput;

class FixProc;
class HoldProc;

NAMESPACE_AIRSIDEENGINE_BEGINE

class AirportModel;
class SimFlight;


	enum RouteType{STAR, SID};
	//engine fix
	class fix : public Entity{
	public:		
		fix(){ m_pfix = NULL; }
		fix(FixProc * pfix ):m_pfix(pfix){}

		FixProc * m_pfix;
		
		inline bool operator==(const fix& lffix)const { return m_pfix==lffix.m_pfix; }
	};	
	typedef EntContainer<fix> fixList;	typedef fixList::iterator fixIter;

	//engine hold
	class hold: public Entity{ 
	public:
		FixProc * m_pfix;
		HoldProc * m_pHold;
	};
	typedef EntContainer< hold > holdList;  typedef holdList::iterator holdIter;

	
	//engine air route
	class airroute: public Entity{
	public:		
		fixList m_vfixes;
		RouteType m_RouteType;

		CString m_RwID;				//The runway ID related with the air route
	};
	typedef EntContainer<airroute> routeList;  typedef routeList::iterator routeIter;

//Abstraction of the airspace of the airport 
class ENGINE_TRANSFER AirspaceModel
{

public:	
	AirspaceModel(AirportModel* _airport): m_pAirport(_airport){}

public:

	//build the abstract model from the Input;
	bool Build(CAirsideInput* _inputC,AirsideInput* _inputD);

	//distribute an air route to flight fill the FEL
	void DistributeArrivalProcess(SimFlight* _flight); 
	//distribute departure process
	void DistributeDepatureProcess(SimFlight *_flight);

	//get airroute
	airroute* getAirRoute(const CString& strID)
	{
		return m_vroutes.getEntity(strID);
	}

	void Clear();

	airroute* getArrivalRoute(SimFlight * _flight);
	airroute* getDepatureRoute(SimFlight * _flight);
	double getCruiseSpeedHorizen(SimFlight* _flight);    // get the  cruise speed in knot
	double getApproachSpeedVertical(SimFlight * _flight) ;    // the Approach speed ,  in ft/min 
	double getApproachSpeedHorizen(SimFlight * _flight) ;   // get the horizen speed 
	double getTerminalSpeedHorizen(SimFlight * _flight);   //get Terminal speed
	Point getApproachPostion(airroute * pairroute, SimFlight * _flight);

private:
	//pointer to the airport
	AirportModel * m_pAirport;
	//	
	fixList m_vfixes;
	//
	holdList m_vholds;
	//
	routeList m_vroutes;

	//SimClock::TimeType m_ComputeTime;
	CAirsideInput * m_pCAirsideInput;
	AirsideInput * m_pDAirsideInput;

public:
	double getClimboutSpeedVertical(SimFlight* _flight);
	double getClimboutSpeed(SimFlight* _flight);
};



NAMESPACE_AIRSIDEENGINE_END

#endif
