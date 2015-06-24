#pragma once
#define _ARRIVE  0x0001
#define _DEPARTURE 0x0002 
#include "../Results/paxlog.h"
#include "../Inputs/schedule.h"
#include "../Results/EVENTLOG.H"
#include "entry.h"
class CFlightLogPair
{
public:
	int Flight_ID ;
	PaxLog* p_Log ;
	PaxLog* m_BagLog ;
	//the index of p_log which has not been generater 
	int m_index_NoActive ;
		TerminalEntryEvent terminalEvent ;
	CFlightLogPair(PaxLog* _log , int Fli_ID):p_Log(_log),Flight_ID(Fli_ID),m_index_NoActive(0)
	{
       m_BagLog = new PaxLog(new EventLog<MobEventStruct>()) ;
	}
	void GetNoActivePaxlog(PaxLog& _log , int count) ;
	~CFlightLogPair()
	{
		if(p_Log != NULL)
			delete p_Log ;
		if(m_BagLog != NULL)
			delete m_BagLog ;
	}

};
//
//this container include the data which will be used by generating passenger . 
//
class CSimEngineConfig;
class CFlightPaxLogContainer
{
private:
	CFlightPaxLogContainer();
	virtual ~CFlightPaxLogContainer(void);
public:
	typedef std::vector<CFlightLogPair*>  TY_PAXLOG_CONTAINER ;
	typedef TY_PAXLOG_CONTAINER::iterator TY_PAXLOG_CONTAINER_ITER ;
	typedef TY_PAXLOG_CONTAINER::const_iterator TY_PAXLOG_CONTAINER_ITER_CON ;
private:

    PaxLog* p_AllPaxLog ;
	TY_PAXLOG_CONTAINER m_ArrDelayContainer ;
	TY_PAXLOG_CONTAINER m_DepDelayContainer ;

	const FlightSchedule* p_schedule ; 
	static CFlightPaxLogContainer* p_containter ;
	PaxLog* m_NodelayPaxlog ;

public:
	//get arrive PaxLog , 
	CFlightLogPair* GetDelayPaxLogByFlightID(int flight_id) ;
	CFlightLogPair* GetDepDelayPaxLogByFlightID(int flight_id);
	//get Dep PaxLog 
	PaxLog* GetNoDelayPaxLog() ;
	
	int CurrentIndexOfPaxlogByFlightID(int Flight_id) ;
	void SetCurrentIndexOfPaxlogByFlightID(int _index ,int Flight_id) ;

	static CFlightPaxLogContainer* GetInstance() ;
	
	void SetPaxLog(PaxLog* p_log) ;
	void SetFlightSchedule(const FlightSchedule* pschedule) ;
	void InitContainer(CSimEngineConfig* pSimConf) ;
	void Reset();
protected:
	
	void AddArrDelayMobLogEntry(MobLogEntry& p_entry) ;
	void AddDepDelayMobLogEntry(MobLogEntry& p_entry) ;
	CFlightLogPair* FindArrPaxLog(int fli_ID) ;
	CFlightLogPair* FindDepPaxLog(int fli_ID) ;
};
