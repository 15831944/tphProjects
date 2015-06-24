#pragma once
//#include "engine\entry.h"
//#include "engine\TerminalTrainEntryEvent.h"
//#include "engine\SimMobileElemList.h"
//#include "inputs\schedule.h"
//#include "common\enginediagnose.h"

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
#include "engine\MobileElementGenerator.h"
#include "common\SelectedSimulationData.h"
//#include "engine\SteppableSchedule.h"


class CARCPort;
class AirsideSimulation;
class MovingTrain;
class FlightsBoardingCallManager;
class SelectedSimulationData;
class CSimParameter;
class ProcessorList;
class PaxLog;
class FlightLog;
class CCongestionAreaManager;
class OnboardSimulation;
class LandsideSimulation;
class TerminalSimulation;
class ProjectCommon;

class CARCportEngine
{
public:
	CARCportEngine(void);
	~CARCportEngine(void);
public:
	CARCPort* m_pARCport;
public:
	int GetPercent();
	void SetPercent( int _nPercent );
	int GetPercentOfRuns();
	void SetPercentOfRuns( int _nPercent );
	void SetRunNumber( int _nRunNumber );
	int GetRunNumber();
	void SendSimMessage( const CString& _csMsg );
	void SendSimFormatMessage( const FORMATMSG& _csFormatMsg );
	void SendSimFormatExMessage( FORMATMSGEX& _formatMsgEx  );
	void SendSimMessageOfRuns();
	void SendEventMessage( int _nEventNum, ElapsedTime _eCurTime, const char* _szTypeName );

	void CancelSimulation(){ m_bCancel = true; }
	bool IsCancel()const{ return m_bCancel; } 

public:
	void runSimulation ( HWND _hWnd, const CString& _csProjPath,const SelectedSimulationData& simSelData, CARCPort *pARCport  );


public:
	bool IsAirsideSel()const {return mSimSelectData.IsAirsideSel();}
	bool IsLandSel()const { return mSimSelectData.IsLandsideSel() ;}
	bool IsOnboardSel()const { return mSimSelectData.IsOnBoardSel();}
	bool IsTerminal()const {return mSimSelectData.IsTerminalSel();}

public:
	AirsideSimulation* GetAirsideSimulation() { return p_simAirside ;} 
	OnboardSimulation* GetOnboardSimulation() { return m_pOnboardSimulation ;} 
	LandsideSimulation* GetLandsideSimulation() { return m_pLandsideSim; }
	TerminalSimulation* GetTerminalSimulation(){ return NULL; } 

public:
	//long generateMobileAgents(FlightSchedule* _pfs, std::vector<int> _OnBoardFlightIDs);
	
	//1.Adjust arrival passenger birth time base on the actual flight arrival time
	//2.Adjust  boardingCall time according to flight¡¯s actual departure time
	void ReflectOnFlightArriveStand(Flight* pFlight, const ElapsedTime& etActureTime,const ElapsedTime& depTime);



protected:
	//set processor engine pointer
	//this function should be on the top of simulation code
	void SetProcListEngine();

	//active the passenger 
	void ActivePassenger() ;

	// generate the logs for the all Elevators
	void GenerateElevatorLogEntry( );

	// generate the logs for the trains.
	void GenerateTrains();

	// called in every simulation run
	// make it ready for store all the logs.
	// for now only clear pax / proc logs.
	void InitLogs( const CString& _csProjPath );

	void CloseLogs(const CString& _csProjPath);


	//when TLOS created processor dynamicly ,we need update proc index in pax log.etc to new index
	void RefreshProcIdxInLogs();


	//Several binary files are created for storing animation data.
	void createEchoFiles (const CString& _csProjPath);


	long generatePassengers (FlightSchedule* _pfs);


	//It generates all passengers for one flight.
	void generatePaxOneFlight (int p_ndx, char p_mode, const FlightSchedule* _pfs);

	//##ModelId=3E2FAD0703B8
	void initElement ( CMobileElemConstraint  p_type,  MobLogEntry& p_elem, int p_max);

	// if a bulk can service for passenger (p_type), return true; or return false;
	int takeBulk(CMobileElemConstraint p_type, MobLogEntry& p_elem, int p_max,ElapsedTime& flyTime);

	// Methods used to generate trasfer passengers for the flight p_ndx.
	int generateXferring ( CMobileElemConstraint  p_type, int p_ndx, int p_count, const ProcessorID& _standGate, const FlightSchedule* _pfs);

	// Methods used to generate transit passengers for the flight p_ndx.
	int generateXiting ( CMobileElemConstraint p_type,  int p_ndx, int p_count , const ProcessorID& _standGate, const FlightSchedule* _pfs);

	void generateArrivals ( CMobileElemConstraint p_type, int p_ndx, int p_count, const ProcessorID& _standGate, const FlightSchedule* _pfs);

	void generateDepartures ( CMobileElemConstraint p_type, int p_ndx, int p_count, const ProcessorID& _standGate, const FlightSchedule* _pfs);

	//Calculates lead time and returns it. 
	ElapsedTime getLeadTime (CMobileElemConstraint p_type, ElapsedTime p_time , const ProcessorID& _standGate);

	//Group size cannot exceeds p_max, max passengers.
	int getGroupSize (CMobileElemConstraint p_type, int p_max);

	int getBags (CMobileElemConstraint p_type, int p_group);

	int createBags (CMobileElemConstraint p_type, MobLogEntry& p_elem, const FlightSchedule* _pfs);

	int getCarts (CMobileElemConstraint p_type, int p_group, int p_bags);

	DistanceUnit getSpeed( CMobileElemConstraint p_type );
	//get the passenger's speed without bags
	DistanceUnit getSpeedWithoutBags( CMobileElemConstraint p_type);


	// 
	int GenerateNopaxs( CMobileElemConstraint p_type, MobLogEntry* p_nopax, MobLogEntry *p_pax, int i_index, long _lCurID, bool _bGenFlag , const ProcessorID& _standGate, const FlightSchedule* _pfs);

	void DeleteGreetPoolData();


	void FlushMovingTrain();
	void ClearMovingTrain();

	virtual void InitCongestionParam(const CString& _pProjPath);
public:
	void SetFireEvacuation( const ElapsedTime& _time ) { m_timeFireEvacuation = _time;	}
	ElapsedTime GetFireEvacuation()const { return m_timeFireEvacuation;	}

	void SetFireOccurFlag( bool _bFlag ){ m_bFireOccur = _bFlag;}
	bool GetFireOccurFlag()const { return m_bFireOccur ;}
public:
	// this is the flight schedule use in the simulation should be clear when simulation done.
	FlightSchedule m_simFlightSchedule;

	// Events
	// use to keep the all person list in the sim engine. should be clean when sim engine done.
	CSimMobileElemList m_simBobileelemList;

	//use to recoding moving train,should be clean when sim engine done.
	std::vector<MovingTrain*>m_simMovingTrainList;
	//For mobile elements entering the terminal.
	TerminalEntryEvent entryEvent;
	TerminalTrainEntryEvent TrainEvent;

	char m_csDisplayMsg[512];
	std::vector<CString> m_vstrFormatMsg;
	EventMsg m_eventMsg;
	HWND m_hWnd;

	FlightsBoardingCallManager*		   m_pFltsBoardingCallManager;

	//SteppableSchedule m_stepSchedule;
protected:
	AirsideSimulation*  p_simAirside;
	OnboardSimulation* m_pOnboardSimulation;
	LandsideSimulation* m_pLandsideSim;

	bool m_bCancel;

	int m_nPercent;
	int m_nPercentOfRuns;
	int m_nRunNumber;

private:
	// no pax count of every type
	std::vector<unsigned long >m_vNonPaxCountByType;
	ElapsedTime m_timeFireEvacuation;
	bool m_bFireOccur;



private:
	SelectedSimulationData mSimSelectData;


public:
	Terminal *getTerminal();
	
	PaxLog *getPaxLog();
	FlightLog *getFlightLog();
	BagLog* getBagLog();
	ProcLog* getProcLog();
	CTrainLog* getTrainLog();
	CElevatorLog* getElevatorLog();
	CBridgeConnectorLog* getBridgeConnectorLog();
	ResourceElementLog * getResourceLog();
	CPaxDestDiagnoseLog* getPaxDestDiagnoseLog();	

	CCongestionAreaManager& getCongestionAreaManager();

	StringDictionary *getInStrDict();

	BaggageData *getBagData();
	
	PassengerTypeList *getPaxTypeList();
	void SendSimSortLogMessage(int nPercent, int nCurFile, int nFileCount);


	ProjectCommon *getProjectCommon();

};
