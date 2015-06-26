#pragma once
//#include "Airside/AirsideFlightInSim.h"

//this class is charge to generate mobileElements according to the LogData . 
//The LogData will be init before the simulation . 
//EX: PaxLog , when before Run simulation ,it will create the data base on flightID .
// when we will generate the passenger , base on the flightID ,we get the paxlog which contain the all passenger data of this flight.
//Data: 7/5/2008
//Author : CJchen
class AirsideFlightInSim ;
class CARCportEngine;
class LandsideVehicleInSim;
class LandsideCurbSideInSim;
class LandSidePublicVehicleType;
class LandsideBusStationInSim;
class Person;
class Passenger;
class ElapsedTime;
class TerminalEntryEvent;

class CMobileElementGenerator
{
public:
  CMobileElementGenerator() ;
  ~CMobileElementGenerator() ;
  //generator mobileElement , set its behavior
  //_mode : which mode when the person being generate
  //_offTime : the Active time  .
  //_TY : DEPATURE OR Arrive
  //_paxnum : the number which will be generated , -1 --- generate all
  //return the number of  generation in fact .
  virtual int GenerateDelayMobileElement(int Fli_ID, ElapsedTime& Time, std::vector<Person*>& _paxlist,bool bGenerateBaggage, int _PaxNum = -1) = 0 ;
  virtual int GenerateNodelayMobileElement(ElapsedTime& Time ,std::vector<Passenger*>& _paxlist,TerminalEntryEvent& _event) = 0 ;
};
class CPaxGenerator : public CMobileElementGenerator
{
public:
	//CPaxGenerator();
	CPaxGenerator(CARCportEngine* _pEngine) ;
	~CPaxGenerator(void);
	int GenerateDelayMobileElement(int Fli_ID, ElapsedTime& Time, std::vector<Person*>& _paxlist, bool bGenerateBaggage,int _PaxNum = -1);

    int GenerateNodelayMobileElement(ElapsedTime& Time ,std::vector<Passenger*>& _paxlist,TerminalEntryEvent& _event) ;
	
	int GenerateArrMobileElementToLandside(int Fli_ID);
	int GenerateDepMobileElementToLandside(int Fli_ID, const ElapsedTime& t);

	//p_ter will be used only in the terminal model .
	//void SetTerminal(Terminal* p_ter) { p_terminal = p_ter ;} ;
	int GenerateDelayMobileBag(int Fli_ID ,ElapsedTime& Time, std::vector<Person*>& _paxlist, bool bHasCartService) ;
	void FindNearestStation(LandsideCurbSideInSim *assCurside,LandSidePublicVehicleType *operation,LandsideBusStationInSim * &pGetOffStation);
private:
	//PaxLog* p_PaxLog ; 
	//Terminal* p_terminal ; 
	CARCportEngine* m_pEngine;
//	AirsideFlightInSim* m_Flight ;
//
//public:
//	void SetFlightInSim(AirsideFlightInSim* _flight) { m_Flight = _flight ;} 
//	AirsideFlightInSim* GetFlightInSim() {return m_Flight ;} 
};
