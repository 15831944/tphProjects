// PersonOnStation.h: interface for the CPersonOnStation class.
// 2002-10-10
// 
// Handle the operation of Person on Stations. From Station A -> Station B and between.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PERSONONSTATION_H__055DFFAC_91CF_4638_90FC_590A9DC4B8D2__INCLUDED_)
#define AFX_PERSONONSTATION_H__055DFFAC_91CF_4638_90FC_590A9DC4B8D2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "common\template.h"
//#include "Terminal.h"
#include "Car.h"
#include "IntegratedStation.h"
#include "Inputs\SingleCarSchedule.h"

enum PERSON_ON_STATION_STATE {	ENTER_STATION,
								WAITAREA_IN_CONSTRAINT,
								WAITAREA_ARRIVE_AT_SERVER,
								WAITAREA_WAIT,
								WAITAREA_LEAVE_SERVER,
								PERSON_IN_ENTRYDOORQUEUE,
								PERSON_ARRIVE_AT_ENTRYDOOR,
								PERSON_LEAVE_ENTRYDOOR,
								PERSON_ARRIVE_AT_CAR,
								CAR_LEAVE_STATION,
								CAR_ARRIVE_AT_STATION,
								PERSON_ARRIVAE_AT_EXITDOOR,
								PERSON_LEAVE_EXITDOOR };

// get the door list
class CEntryDoorInfo
{
public:
	DistanceUnit m_dDistance;
	int m_nQueueLen;
	int m_nCarIdx;
	int m_nDoorIdx;

	bool operator < ( const CEntryDoorInfo& _info ) const
	{
		return m_dDistance < _info.m_dDistance;
	}

	bool operator > ( const CEntryDoorInfo& _info ) const
	{
		return m_dDistance > _info.m_dDistance;
	}		
};
class TrainCarInSim;
class MovingTrain;
								
class CPersonOnStation  
{
protected:
	Person* m_pPerson;
	int m_nProcIndex;
	int m_nDestIndex;
	enum PERSON_ON_STATION_STATE m_enumState;
	Terminal* m_pTerm;
	int m_nCarIndex;
	int m_nEntryDoorIndex;
	int m_nExitDoorIndex;
	ElapsedTime m_curArrTime;
	ElapsedTime m_curDepTime;
	ElapsedTime m_destArrTime;
    ProcessorDistribution* m_pDestStationList;
	std::vector<CViehcleMovePath> m_movePath;

	ElapsedTime m_prevEventTime;	// remember last event time for remove purpose.

	Point m_ptOffset;	// remember the offset between position in car and service location of integration station.

	void SelectBestCar();

	void GenerateEvent( ElapsedTime eventTime );

	// select the closest car and entry door.
	void SelectBestCarAndEntryDoor( IntegratedStation* _pStation, Point _ptCurPos );

	// base on m_nCarIndex pick a best car, or just return the car.
	CCar* GetCurrentCarStation( IntegratedStation* _pStation );
	//the car number of the train and station might does not match
	//for example, there might 6 cars train parking at 8 cars station( the station could parking 8 cars train)
	TrainCarInSim* GetCarInTrain(MovingTrain* pTrain, int nCarIndex);

	
	// base on m_nEntryDoorIndex pick a best entry door or just return the entry door.
	Processor* GetCurrentEntryDoor( IntegratedStation* _pStation );

	// select the closest exit door.
	void SelectBestExitDoor( IntegratedStation* _pStation, Point _ptCurPos );

	// base on m_nExitDoorIndex pick a best exit door or just return the exit door.
	Processor* GetExitDoor( IntegratedStation* _pStation );
	
	// process the queue for entry door.
	void ProcessEntryDoorQueue( ElapsedTime _time, IntegratedStation* pStation );

	// process when arrive at preboarding area.
	void ArriveAtPreBoardArea( IntegratedStation* _pStation, ElapsedTime _time );
	
	// return the from station.
	IntegratedStation* GetSourceStation();

	// return the to station.
	IntegratedStation* GetDestStation();
	
	// get all schedule
	bool GetSchedule( ElapsedTime _time );

	void GetDestProcIndex();

	//find a best station from share preboarding area stations , from which person can go to his dest station
	// and the train stop at that station is the nearlst arrival one from current time
	IntegratedStation* GetBestStationToDepature( const ElapsedTime& _time , ElapsedTime& _timeArrival );
public:

	bool IsDone();
	void SetDestProcIndex( int _nDestProcIndex ){ m_nDestIndex = _nDestProcIndex; }
	bool Process(  ElapsedTime _time );
	CPersonOnStation( Person* _pPerson, int _nProcIndex, Terminal* _pTerm ,ProcessorDistribution* _pDestStationList );
	virtual ~CPersonOnStation();
};

#endif // !defined(AFX_PERSONONSTATION_H__055DFFAC_91CF_4638_90FC_590A9DC4B8D2__INCLUDED_)
