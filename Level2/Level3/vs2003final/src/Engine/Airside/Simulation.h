#ifndef __AIRSIDE_SIMULATION_DEF
#define __AIRSIDE_SIMULATION_DEF
#include "../EngineDll.h"
#include "../EngineConfig.h"

#include "../SimClock.h"

#include "Flight.h"
#include "Event.h"
#include "Simulator.h"
//#include "FlightGenerator.h"
#include "ProcessGenerator.h"
//#include "FlightList.h"
#include "AirportModel.h"
#include "SimDisplayer.h"


#pragma once

NAMESPACE_AIRSIDEENGINE_BEGINE


class ENGINE_TRANSFER Simulation
{
public:
	Simulation(void);
	~Simulation(void);

	//Initialize the Simulation clock , build airside model, generate flight
	bool Init( CAirsideInput*inputC, AirsideInput* _inputD);

	//run the simulation : gen process -> simulator
	bool Run();
	

	//write airside flight log
	void writeLog();

	SimDisplayer * getDisplayer(){ return &m_displayer; }

	FlightList * getFlightList(){ return &m_flights; }

	AirportModel * getAirportModel(){ return &m_simModel; }
	
private:
	SimClock m_simClock;

	FlightGenerator m_fltGenerator;
	ProcessGenerator m_processGenerator;
	Simulator m_simulator;
	//ACTypeList m_ACTypelist;
	FlightList m_flights;
	AirportModel m_simModel;
	SimDisplayer m_displayer;

	bool IsSthToAnimate();
	void ModifyPushback();

};

#endif

NAMESPACE_AIRSIDEENGINE_END
