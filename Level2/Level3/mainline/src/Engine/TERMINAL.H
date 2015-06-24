#ifndef TERMINAL_H
#define TERMINAL_H

#include "common\template.h"
#include "engine\entry.h"
#include "engine\TerminalTrainEntryEvent.h"
#include "engine\bagenter.h"
#include "inputs\in_term.h"
#include "results\out_term.h"
#include "common\util.h"
#include "common\point.h"
#include "inputs\MobileElemConstraint.h"
#include "common\ARCException.h"
#include "engine\SimMobileElemList.h"
#include "inputs\schedule.h"
#include "engine\CongestionAreaManager.h"

class CCongestionAreaManager;
class FlightSchedule;
class AirsideSimulation;
class SelectedSimulationData;

class MobileElementTerminalEntryManager;
class FlightsBoardingCallManager;
class AirsideSimulation ;
class MovingTrain;
//This class is multi-inherited from InputTerminal and OutputTerminal.
class ENGINE_TRANSFER Terminal : public InputTerminal, public OutputTerminal
{
//protected:
//	int m_nPercent;
//	int m_nPercentOfRuns;
//	int m_nRunNumber;
//	bool m_bCancel;
//
//    AirsideSimulation*  p_simAirside ;	
//public:
//	AirsideSimulation* GetAirsideSimulation() { return p_simAirside ;} ;
//private:
//	// no pax count of every type
//	std::vector<unsigned long >m_vNonPaxCountByType;
//	ElapsedTime m_timeFireEvacuation;
//	bool m_bFireOccur;
//
//public:
//	// called in every simulation run
//	// make it ready for store all the logs.
//	// for now only clear pax / proc logs.
//	void InitLogs( const CString& _csProjPath );
//	
//
//	//when TLOS created processor dynamicly ,we need update proc index in pax log.etc to new index
//	void RefreshProcIdxInLogs();
//
//
//
//protected:
//
//	//Several binary files are created for storing animation data.
//	void createEchoFiles (const CString& _csProjPath);
//
//	// generate the logs for the all Elevators
//	void GenerateElevatorLogEntry( );
//
//
//
//public:     
//	// this is the flight schedule use in the simulation should be clear when simulation done.
//	FlightSchedule m_simFlightSchedule;
//
//    // Events
//	// use to keep the all person list in the sim engine. should be clean when sim engine done.
//	CSimMobileElemList m_simBobileelemList;
//
//	//use to recoding movingtrain,should be clean when sim engine done.
//	std::vector<MovingTrain*>m_simMovingTrainList;
//    //For mobile elements entering the terminal.
//    TerminalEntryEvent entryEvent;
//	TerminalTrainEntryEvent TrainEvent;
//
//    //For bags arriving at baggage carosels in the terminal.
//////    BaggageEntryEvent bagEvent;
//
//	char m_csDisplayMsg[512];
//	std::vector<CString> m_vstrFormatMsg;
//	EventMsg m_eventMsg;
//	HWND m_hWnd;
//	
//	CCongestionAreaManager m_congManager;
//
//	//for adjust arrival passenger birth time base on the actual flight arrival time
//	//PassengersSimulationBuffer m_PaxSimBuffer;
//
//	FlightsBoardingCallManager*		   m_pFltsBoardingCallManager;
//
//public:
//	virtual void InitCongestionParam(const CString& _pProjPath);
//
//	//FlightSchedule* m_pFlightSchedule;
//	int GetPercent();
//	void SetPercent( int _nPercent );
//	int GetPercentOfRuns();
//	void SetPercentOfRuns( int _nPercent );
//	void SetRunNumber( int _nRunNumber );
//	int GetRunNumber();
//	void SendSimMessage( const CString& _csMsg );
//	void SendSimFormatMessage( const FORMATMSG& _csFormatMsg );
//	void SendSimFormatExMessage( FORMATMSGEX& _formatMsgEx  );
//	void SendSimMessageOfRuns();
//	void SendEventMessage( int _nEventNum, ElapsedTime _eCurTime, const char* _szTypeName );
//
//    Terminal ();
//    virtual ~Terminal ();
//
//	void CancelSimulation(){ m_bCancel = true; }
//
//    void runSimulation ( HWND _hWnd, const CString& _csProjPath,const SelectedSimulationData& simSelData  );
//
//    void closeDatabase (void);
//
////    void createEchoFiles (const CString& _csProjPath);
//
//    long generatePassengers (FlightSchedule* _pfs);
//
//	long generateMobileAgents(FlightSchedule* _pfs, std::vector<int> _OnBoardFlightIDs);
//
//    //It generates all passengers for one flight.
//    void generatePaxOneFlight (int p_ndx, char p_mode, const FlightSchedule* _pfs);
//
//	//##ModelId=3E2FAD0703B8
//    void initElement ( CMobileElemConstraint  p_type,  MobLogEntry& p_elem, int p_max);
//
//	// if a bulk can service for passenger (p_type), return true; or return false;
//	int takeBulk(CMobileElemConstraint p_type, MobLogEntry& p_elem, int p_max,ElapsedTime& flyTime);
//		
//    // Methods used to generate trasfer passengers for the flight p_ndx.
//    int generateXferring ( CMobileElemConstraint  p_type, int p_ndx, int p_count, const ProcessorID& _standGate, const FlightSchedule* _pfs);
//
//    // Methods used to generate transit passengers for the flight p_ndx.
//    int generateXiting ( CMobileElemConstraint p_type,  int p_ndx, int p_count , const ProcessorID& _standGate, const FlightSchedule* _pfs);
//
//    void generateArrivals ( CMobileElemConstraint p_type, int p_ndx, int p_count, const ProcessorID& _standGate, const FlightSchedule* _pfs);
//
//    void generateDepartures ( CMobileElemConstraint p_type, int p_ndx, int p_count, const ProcessorID& _standGate, const FlightSchedule* _pfs);
//
//    //Calculates lead time and returns it. 
//    ElapsedTime getLeadTime (CMobileElemConstraint p_type, ElapsedTime p_time , const ProcessorID& _standGate);
//
//    //Group size cannot exceeds p_max, max passengers.
//    int getGroupSize (CMobileElemConstraint p_type, int p_max);
//
//    int getBags (CMobileElemConstraint p_type, int p_group);
//
//    int createBags (CMobileElemConstraint p_type, MobLogEntry& p_elem, const FlightSchedule* _pfs);
//
//    int getCarts (CMobileElemConstraint p_type, int p_group, int p_bags);
//	
//	DistanceUnit getSpeed( CMobileElemConstraint p_type );
//	//get the passenger's speed without bags
//	DistanceUnit getSpeedWithoutBags( CMobileElemConstraint p_type);
//
//
//	// 
//	int GenerateNopaxs( CMobileElemConstraint p_type, MobLogEntry* p_nopax, MobLogEntry *p_pax, int i_index, long _lCurID, bool _bGenFlag , const ProcessorID& _standGate, const FlightSchedule* _pfs);
//	
//	void SetFireEvacuation( const ElapsedTime& _time ) { m_timeFireEvacuation = _time;	}
//	ElapsedTime GetFireEvacuation()const { return m_timeFireEvacuation;	}
//
//	void SetFireOccurFlag( bool _bFlag ){ m_bFireOccur = _bFlag;}
//	bool GetFireOccurFlag()const { return m_bFireOccur ;}
//
//	void DeleteGreetPoolData();
//
//	//1.Adjust arrival passenger birth time base on the actual flight arrival time
//	//2.Adjust  boardingCall time according to flight¡¯s actual departure time
//	void ReflectOnFlightArriveStand(Flight* pFlight, const ElapsedTime& etActureTime);
//
//	void FlushMovingTrain();
//	void ClearMovingTrain();
//public:
//	BOOL IsAirsideSel() {return m_AirsideSel ;};
//	BOOL IsLandSel() { return m_LandSel ;} ;
//	BOOL IsOnboardSel() { return m_OnboardSel ;} ;
//	BOOL IsTerminal() {return m_terminal;};
//private:
//    BOOL m_AirsideSel ;
//	BOOL m_LandSel ;
//	BOOL m_OnboardSel ;
//	BOOL m_terminal ;

};


#endif
