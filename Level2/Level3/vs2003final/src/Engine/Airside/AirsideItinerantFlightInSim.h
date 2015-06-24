#pragma once
#include "airsideflightinsim.h"


class ItinerantFlight;
class Flight;
class Terminal;
class AirsideSimulation;
class CAirportDatabase;
class ItinerantFlightScheduleItem;
class AirRouteNetworkInSim;

class AirsideItinerantFlightInSim :
	public AirsideFlightInSim
{
public:
	AirsideItinerantFlightInSim(CARCportEngine *pARCPortEngine,Flight *pFlight,int nid);
	~AirsideItinerantFlightInSim(void);


	//AirsideItinerantFlightInSim * InitializeAirsideItinerantFlight(Flight *pFlt);

	ElapsedTime getArrTime (void) const { return m_arrTime; }
	ElapsedTime getDepTime (void) const { return m_depTime; }
	void getACType (char *p_str) const;
public:
	virtual int fits(const FlightConstraint& _fltcons) const;

	void SetFltConstraint(const FlightConstraint& fltType);

	void SetEntryWaypointID(int nID){m_nEntryWaypointID = nID;}
	int GetEntryWaypointID(){ return m_nEntryWaypointID;}

	void SetExitWaypointID(int nID){ m_nExitWaypointID = nID; }
	int GetExitWaypointID(){ return m_nExitWaypointID;}

	bool IsItinerantFlight(){ return true;}
	bool IsCircuitFlight(){return false;}

	BOOL IsThroughOut() const { return m_bEnroute; }
	void SetThroughOut(){ m_bEnroute = TRUE; }

	FlightRouteInSim* GetEnroute(AirRouteNetworkInSim* pRouteNetwork);
protected:
	//CString m_strFltConstraint;
	FlightConstraint m_fltCons;

	int m_nEntryWaypointID;
	int m_nExitWaypointID;
	BOOL m_bEnroute;
	FlightRouteInSim* m_pEnroute;
};

class AirsideItinerantFlightInSimGenerator
{
public:
	AirsideItinerantFlightInSimGenerator(int nProjID,InputTerminal* pInputTerminal,int nid,AirsideSimulation *pAirsideSimulation);
	~AirsideItinerantFlightInSimGenerator();

	void GenerateFlight(CARCportEngine *pARCPortEngine);

protected:
	int m_nProjID;
	InputTerminal *m_pInputTerminal;
	int m_nFlightID;
	AirsideSimulation *m_pAirsideSimulation;
	
	//Flight* InitializeTerminalFlight(ItinerantFlight *pItiFlight,int& nFltID);
	void InitializeFlightFromSchedule(CARCportEngine *pARCPortEngine,ItinerantFlightScheduleItem *pItiFlt,int nFltID);
};