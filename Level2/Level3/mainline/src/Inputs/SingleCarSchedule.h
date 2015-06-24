// SingleCarSchedule.h: interface for the CSingleCarSchedule class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SINGLECARSCHEDULE_H__51148711_F19F_4370_B84B_16B8F114A3CF__INCLUDED_)
#define AFX_SINGLECARSCHEDULE_H__51148711_F19F_4370_B84B_16B8F114A3CF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "ViehcleMovePath.h"
#include "..\engine\IntegratedStation.h"
#include <list>
#include "..\engine\TrainTrafficGraph.h"

class CArrDepTime
{
private:
	ElapsedTime m_arriveTime;
	ElapsedTime m_departureTime;
public:
	CArrDepTime ( ElapsedTime _arrTime, ElapsedTime _depTime) { m_arriveTime = _arrTime; m_departureTime = _depTime ;};
	void SetArrTime( ElapsedTime _time ){ m_arriveTime = _time; };
	void SetDepTime( ElapsedTime _time ){ m_departureTime = _time; };
	CString GetArrTimeString() const;
	CString GetDepTimeString() const;
	// new version to get the time string, get time string and pass day out
	CString GetArrTimeString(int& day) const;
	CString GetDepTimeString(int& day) const;
	ElapsedTime GetArrTime() const{ return m_arriveTime;};
	ElapsedTime GetDepTime() const{ return m_departureTime;};
	
};
typedef struct
{
	CString m_sCurStationName;
	CString m_sDestStationName;
	int     m_arrDay;
	CString m_arrTime;
	int     m_depDay;
	CString m_depTime;
	CString m_sScheduleName;
	int     m_nTrainIndex;
} TIMETABLE;
typedef std::vector<CArrDepTime> ArrDepTimeVector;
typedef std::list<IntegratedStation*> CARFLOW;
typedef std::vector<TIMETABLE> TimeTableVector;


// one schedule in the train schedule dialog
class CSingleCarSchedule  
{
	
	/*
	typedef struct
	{
		IntegratedStation* _pStation;
		IntegratedStation* _pNextStation;
		RailwayInfo*       _pLinkedRailway;
	} REALPATH;
	*/
protected:

	//Normal speed 
	DistanceUnit m_dNormalSpeed;

	//Acceleration speed
	DistanceUnit m_dAccelerateSpeed;

	//Deceleration  speed
	DistanceUnit m_dDecelerateSpeed;
	
	RAILWAY m_allRailWay;

	RAILWAYKEYS m_allRailWayKeys;

	// keep the template path
	std::vector<CViehcleMovePath> m_ViehcleMovePath;
	
	//all stations I will pass
	CARFLOW m_carFlow;

	std::vector<int> m_stayStationTime;

	ElapsedTime m_circleTime;
	
	
	//car id
	CString m_carName;




	//schedule time
	ElapsedTime m_scheduleStartTime;
	ElapsedTime m_scheduleEndTime;

	// actual time
	ElapsedTime m_actualEndTime;
	

	int m_iTrainNumber;
	ElapsedTime m_turnArroundTime;
	ElapsedTime m_headWayTime;
	ProcessorList* m_pSystemProcessorList;
	CRailWayData* m_pSystemRailWay;

	int m_iOneDayCount;

	//whether or not the station has a railway linked with next station 
	//it is used by the CCarScheduleDlg to display the car flow tree correctly
	std::vector<bool> m_linkedRailWayFlag;


	// input: station1, station2, start time, railway
	// output: viehcleMovePath, lastHitTime
	ElapsedTime GenerateViehcleMovePathBetweenTwoStation( IntegratedStation* _pSourceStation, IntegratedStation* _pDestStation, int _nLinkedType, RailwayInfo* _pRailwayInfo, ElapsedTime _arrTime, ElapsedTime _depTime, CViehcleMovePath& _viehcleMovePath );

	bool GetLocationAndTime( int _iSourceStationIndex, int _iDestStationIndex, ElapsedTime arriveCurStationTime , std::vector<CViehcleMovePath>& _bestMovePath );

//	bool GetArrDepTimeTable( IntegratedStation* _pCurrentStation, IntegratedStation* _pDesStation ,ArrDepTimeVector& _timeTable );
	bool GetArrDepTimeTable( int _iCurrentStationIndex ,ArrDepTimeVector& _timeTable );






public:
	
	CSingleCarSchedule();
	virtual ~CSingleCarSchedule();

public:

	bool SelectBestSchedule( IntegratedStation* _pCurrentStation, IntegratedStation* _pDesStation, ElapsedTime arriveCurStationTime, std::vector<CViehcleMovePath>& _bestMovePath );
	//bool Ch




	const CString& GetCarName() const { return m_carName;	};
	void SetCarName(CString _newCarName) { m_carName=_newCarName;};


	int GetDwellingTime( int iPosInVector )const;
	void SetDwellingTime ( int iPosInVector, int iTime );
	

	//add _preStation's next station to the car flow list,if _preStation is null, add into list head
	bool AddNextStation( IntegratedStation* _preStation,  IntegratedStation* _nextStation, bool _pushToBack=false);

	void SetCarStartTime( const ElapsedTime& _carTime){ m_scheduleStartTime = _carTime;};
	const ElapsedTime GetCarStartTime( ) const { return m_scheduleStartTime;};

	void SetCarEndtTime( const ElapsedTime& _carTime){ m_scheduleEndTime = _carTime;};
	const ElapsedTime GetCarEndTime( ) const { return m_scheduleEndTime;};

	void SetTrainNumber( int _iTrainNumber){ m_iTrainNumber = _iTrainNumber;};
	int GetTrainNumber( ) const { return m_iTrainNumber;};

	void SetTurnAroundTime( ElapsedTime _turnAroundTime){ m_turnArroundTime = _turnAroundTime;};
	ElapsedTime GetTurnAroundTime( ) const { return m_turnArroundTime;};

	void SetHeadWayTime( ElapsedTime _headWayTime){ m_headWayTime = _headWayTime;};
	ElapsedTime GetHeadWayTime( ) const { return m_headWayTime;};



	void SetNormalSpeed( DistanceUnit _normalSpeed) { m_dNormalSpeed=_normalSpeed;	};
	DistanceUnit GetNormalSpeed( ) const { return m_dNormalSpeed;};

	void SetAccelerateSpeed( DistanceUnit _accelerateSpeed) { m_dAccelerateSpeed=_accelerateSpeed;	};
	DistanceUnit GetAccelerateSpeed( ) const { return m_dAccelerateSpeed;};

	
	void SetDecelerateSpeed( DistanceUnit _decelerateSpeed) { m_dDecelerateSpeed=_decelerateSpeed;	};
	DistanceUnit GetDecelerateSpeed( ) const { return m_dDecelerateSpeed;};

	const CARFLOW& GetCarFlow() const { return m_carFlow; };

	//check if defined car flow is valid
	bool IsAValidCarFlow(TrainTrafficGraph* _pTrafficGraph) const;
	
	bool IsInputDataValid() const;

	void WriteData ( ArctermFile& p_file )const;
	bool ReadData ( ArctermFile& p_file );
	void readObsoleteData ( ArctermFile& p_file );

	void SetSystemProcessorList( ProcessorList* _pProcList) { m_pSystemProcessorList=_pProcList;	};
	void SetSystemRailWay( CRailWayData* _pRailWaySys) { m_pSystemRailWay=_pRailWaySys;	};

	RAILWAY& GetRailWay() { return m_allRailWay; };
	bool AddLinkedRailWay (  IntegratedStation* _pStartStation, RailwayInfo* _pLinkedRailWay , bool _pushToBack=false);
	bool AddStayOnStationTime (  IntegratedStation* _pStation, int _iStayOnStationMini , bool _pushBack=false );
	
	
	bool DeleteLinkedRailWay( IntegratedStation* _pStartStation, RailwayInfo* _pLinkedRailWay );
	bool DeleteLinkedRailWay( int iCountFromTail );
	//you should call IsAValidCarFlow() before call this func.
	void GenerateRealSchedule( TrainTrafficGraph* _pTraffic );
 
	bool GetNearestSchedule( IntegratedStation* _pCurrentStation, IntegratedStation* _pDesStation, ElapsedTime _currentTime,  ElapsedTime& _arriveCurStationTime, ElapsedTime& _arriveDestStationTime );

//	void TraceAllResult( PAXMOVEPATH& _traceObject);
	void GenerateOneDayScheduleFromFile();
	
	void GetAllTimeTable( TimeTableVector& _timeTable );
	void GetStationTimeTable( IntegratedStation* _pStation, TimeTableVector& _timeTable);

	// get all location and time for whole day.
	void GetAllLocationAndTime( int _nTrainIdx, std::vector<CViehcleMovePath>& _vMovePath );

	ElapsedTime GetActualEndTime();
	
	ElapsedTime GetCircleTime();


	
	//we have defined the schedule according to railwaysystem,
	//but if the railway system is changed, that is the pointer of
	// RailWayInfo is changed, so we should adjust CSingleCarSchedule's data member
	// RAILWAY m_allRailWay, to adapt that change
	// if railway has been deleted ,then return true and we should delete this single schedule
	// in class CAllCarSchedule.
	bool AdjustRailway( TrainTrafficGraph* _pTraffic  );

	void AdjustRailwayKeys( int _iOldStationIndex);

	bool IsUseThisStation( int _iStationIndex );

	// test if this sechedule can take people from source station to dest station
	bool IfScheduleBetweenStations( IntegratedStation* _pSourceStation, IntegratedStation* _pDesStation )const;

	//the function will return the 1st station's layout
	//all the station's layout should be same in the schedule
	CStationLayout *GetStationLayout();


};

#endif // !defined(AFX_SINGLECARSCHEDULE_H__51148711_F19F_4370_B84B_16B8F114A3CF__INCLUDED_)
