// ElevatorProc.h: interface for the ElevatorProc class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ELEVATORPROC_H__F007466D_9257_451B_ADC8_C5B3F80163B6__INCLUDED_)
#define AFX_ELEVATORPROC_H__F007466D_9257_451B_ADC8_C5B3F80163B6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "PROCESS.H"
#include "common\pollygon.h"
#include "results\ElevatorLogEntry.h"
#include <map>
#include <list>
#include "common\ARCException.h"
#include <CommonData/ExtraProcProp.h>


class ElevatorProc;
enum ElevatorDirection
{
	UP,
	DOWN,
	PAUSE,
	HALT,
};
class DataOfEveryFloor
{
	// elevator 's waiting area 
	Pollygon m_waitArea;

	// all lift's area
	std::vector<Pollygon>m_vLiftAreas;
	
	//the location of every lift's door
	std::vector<Point>m_vLiftDoors;

	// all person who wait in elevator's waiting area
	std::list<Person*>m_listPersonInWaitArea;
public:
	DataOfEveryFloor(){};
	virtual ~DataOfEveryFloor(){};
public:
	void SetWaitArea( const Pollygon& _waitArea );
	const Pollygon& GetWaitArea()const { return m_waitArea;	}

	//return all lift areas and elevator waiting area ( for 3D view drawing purpose );
	void GetLayoutData( std::vector<Pollygon>& _vLayout );

	void SetLiftAreas( const std::vector<Pollygon>& _vlayout ){ m_vLiftAreas = _vlayout;	}

	void SetLiftDoors( const std::vector<Point>& _vDoors ){ m_vLiftDoors = _vDoors;	}
	//get location of every lift's door
	void GetLiftDoors( std::vector<Point>& _vDoors )const{ _vDoors = m_vLiftDoors;	}

	std::vector<Pollygon>& GetLiftAreas(){ return m_vLiftAreas;	}

	void AddPersonIntoWaitList( Person* _pPerson );

	void ClearPersonIntowaitList(void){m_listPersonInWaitArea.clear();}
	std::list<Person*>& GetPersonInElevatorWaitArea(){	 return m_listPersonInWaitArea;	}

};

class DataOfEveryLift
{
private:
	// current floor which lift stop
	int m_iCurrentFloor;

	// the status of lift
	ElevatorDirection m_enumDirection;

	//decide which floor should lift stop at.
	// sort by " < " order
	std::map<int,bool>m_mapUpStopFlags;
	std::map<int,bool>m_mapDownStopFlags;

	//necessary time to let lift move from one floor to another
	std::vector<ElapsedTime>m_vNecessaryTimeMoveToNextFloor;

	//all person in lift
	std::list<Person*>m_listPersonInLift;

	//log entry
	CElevatorLogEntry m_logOfLift;
	
	//every single lift's log has the same x and y , only the z is different . so
	// m_ptlogPoint store lift's x, and y position
	Point m_ptLogPoint;

	std::map<int,Point>m_mapEntryExitPoint;

	Terminal* m_pTerm;
public:
	DataOfEveryLift(){ m_enumDirection = HALT   ;};
	~DataOfEveryLift(){};
public:

	
	// set specific floor's stop flag
	/* m_iMinFloor <= _iWhichFloor && m_iMaxFloor >= _iWhichFloor */
	void SetStopFlag( int _iWhichFloor, bool _bFlag, bool _bUp );
	bool GetStopFlag( int _iWhichFloor , bool _bUp);

	void SetDirection( ElevatorDirection _enDirection ) { m_enumDirection = _enDirection; }
	ElevatorDirection GetDirection() { return m_enumDirection;	}

	int GetCurrentFloor()const { return m_iCurrentFloor;	}
	void SetCurrentFloor( int _iFloor ){ m_iCurrentFloor = _iFloor;	}

	// init stop flags
	void InitLiftData( std::vector<int>& _vStopAtFloors ,std::vector<ElapsedTime>& _vNecessaryTime ,Terminal* _pTerm, ElevatorProc* _pElevator , int _iLiftIdx);	

	////move lift to next floor to service person . if no person wait for service at all floor, then lift will halt
	void MoveToNextFloor( const ElapsedTime& _currentTime , int _iLift, ElevatorProc* _pElevator );

	void SetLogOfLift( const CElevatorLogEntry& _logEntry ){ m_logOfLift = _logEntry;}

	void SetLogPoint( const Point& _ptLog ){ m_ptLogPoint = _ptLog;	}

		// generate a new elevator move event
	void GenerateEvent( const ElapsedTime& _currentTime , int _iLift, ElevatorProc* _pElevator );
	void FlushLog();

	void SetEntryExitPoint( const std::map<int,Point>& _ptPoint ){ m_mapEntryExitPoint = _ptPoint;	}
	//Point GetEntryExitPoint()const { return m_ptEntryExitPoint;	}

private:

	//bool ShouldMoveTo( int _iCurrentFloor, bool _bHeadingToTop, bool _bCheckUpStopFlags );

	// test if this lift should stop at current floor to provide service
	bool ShouldStopAtCurrentFloor();

	// write  logs
	void WriteLog( const ElapsedTime& _currentTime );

	/* write all person in lift logs */
	void WritePersonInLiftLog( const ElapsedTime& _currentTime );


	// reset current floor and lift's status ( such as, UP, Down, or halt )
	bool RefreshCurrentFloor();

	//reset stop at flag of every lift according to current context
	void UpdataLiftStopFlags( ElevatorProc* _pElevator, int _iLift);

	// move all person who stay in lift and his destination is current floor out of lift
	void MovePersonOutElevator( const ElapsedTime& _currentTime ,int _iLift , ElevatorProc* _pElevator, ElapsedTime& _maxTimeOfPersonMovingOut );

	// move all person who wait for this lift into elevator
	void MovePersonIntoElevator(  const ElapsedTime& _currentTime ,int _iLift, ElevatorProc* _pElevator, ElapsedTime& _maxTimeOfPersonMovingIn );

	ElapsedTime GetNecessaryTimeToNextFloor( int _iPreFloor, int _iNextFloor )const;

	int GetPersonCountInLift()const;

	void CaculateEntryExitPoint(  ElevatorProc* _pElevator  , int _iLiftIdx, int _iFloor );




#ifdef _DEBUG
	void TraceStopFlags();
#endif
};

class ElevatorProc : public Processor, public ElevatorProcProp
{
	//every floor's layout and logic data
	std::vector<DataOfEveryFloor>m_vAllFloorsData;

	// every lift's logic data
	std::vector<DataOfEveryLift>m_vAllLiftData;
public:
	ElevatorProc();
	virtual ~ElevatorProc();
protected:
	//virtual Point GetPipeExitPoint( int iCurFloor, CString& _curTime,Point& outPoint,TerminalMobElementBehavior *terminalMob) ;
	// read , write elevator process's specail data, such as lift area length...etc
	virtual int readSpecialField(ArctermFile& procFile);
	virtual int writeSpecialField(ArctermFile& procFile)const;

public:

    virtual int getProcessorType (void) const { return Elevator; }    
    virtual const char *getProcessorName (void) const { return "Elevator"; }

	virtual ExtraProcProp* GetExtraProp() { return this; }

	// init elevator's stop at which floor data
	virtual void initSpecificMisc (const MiscData *miscData) ;

	// Returns service m_location.
	Point GetServiceLocationOnFloor ( int _nFloor ) const;

	// Acquire service m_location.
	virtual Point AcquireServiceLocation( Person* _pPerson  );
    
	virtual Point GetServiceLocation() const{ assert( false ); return Point(); }

	virtual void beginService (Person *_pPerson, ElapsedTime curTime);

	virtual void DoOffset( DistanceUnit _xOffset, DistanceUnit _yOffset );
	virtual void Rotate( DistanceUnit _degree );

	virtual void GetDataAtFloorLayoutData(int nFloor, std::vector<Pollygon>& outAreas);
	virtual void GetDataAtFloorLiftDoors(int nFloor, std::vector<Point>& outDoors);
	virtual std::vector<Pollygon>& GetDataAtFloorLiftAreas(int nFloor);


public:
	void SetPlatFormLength( double _dPlatFormLen ){ m_dWaitareaLength = _dPlatFormLen;	}
	double GetPlatFormLength()const{ return m_dWaitareaLength;}

	void SetPlatFormWidth( double _dPlatFormWid ){ m_dWaitareaWidth = _dPlatFormWid;	}
	double GetPlatFormWidth()const{ return m_dWaitareaWidth;}

	void SetElevatorAreaLength( double _dElevatorAreaLen ){ m_dLiftAreaLength = _dElevatorAreaLen;	}
	double GetElevatorAreaLength()const{ return m_dLiftAreaLength;}

	void SetElevatorAreaWidth( double _dElevatorAreaWid ){ m_dLiftAreaWidth = _dElevatorAreaWid;	}
	double GetElevatorAreaWidth()const{ return m_dLiftAreaWidth;}

	void SetNumberOfLift( int _iNum ){ m_iNumberOfLift = _iNum ;}
	int GetNumberOfLift()const { return m_iNumberOfLift;}

	void SetMinFloor( int _iMinFloor ){ m_iMinFloor = _iMinFloor;	}
	int GetMinFloor()const { return m_iMinFloor;}

	void SetMaxFloor( int _iMaxFloor ){ m_iMaxFloor = _iMaxFloor;	}
	int GetMaxFloor()const { return m_iMaxFloor;}

	void SetOrientation( DistanceUnit _dDegree ) {  m_dOrientation = _dDegree; }
	DistanceUnit GetOrientation()const { return m_dOrientation;	}

	void SetPosOfWaitArea( int _iWhichFloor, bool _bLeft );	
	bool GetPosOfWaitArea( int _iWhichFloor )const ;

	// check if this processor can serve current person 
	// according to elevetor lift's current occupancy state and it's capacity attributes.
	virtual bool isVacant( Person* pPerson  = NULL) const;

	//identify if person can enter current lift
	bool  canEnterLift( Person* pPerson, std::list<Person*>& listPerson) const;


	//int GetCapacity()const { return m_iCapacity;	}
	
	//caculate the proc's layout on every floor ( include the position and shap of lift area, elevator waiting area)
	void InitLayout();

	// init all lift data of elevator  ( such as the log point of lift ,lift's stop flags )
	bool InitElevatorData( std::vector<double>& _vFloorsAltitude,Terminal* _pTerm );

	int GetFloorCount()const;

	// get related data of floor ( note: m_iMinFloor <= _iWhichFloor <= m_iMaxFloor )
	DataOfEveryFloor& GetDataAtFloor( int _iWhichFloor );

	std::vector<bool>& GetWaitAreaPos() { return m_vPosOfWaitarea;	};

	DataOfEveryLift& GetDataOfLift( int _iLift );

	// process elevator move event 
	void ElevatorMoveEventComes( int _iLift ,const ElapsedTime& _currentTime );
	
	std::map<int,short>& GetFloorHeight(){ return m_mapFloorHeight;	}

	void FlushLog();

	virtual Point GetEntryPoint(const CString& _strMobType,int _iCurFloor, const CString& _strTime);
    virtual bool CheckIfOnTheSameFloor(int _iCurFloor);

	virtual Processor* CreateNewProc();
	virtual bool PrepareCopy(Processor* _pDestProc, const int _nFloor, const int _nDestFloorTotal);
	virtual bool CopyOtherData(Processor* _pDestProc);
private:
	// start any lift whose status is HALT
	void StartElevatorIfNecessary( Person* _pPerson, const ElapsedTime& _curTime);

	//select a best lift to service person who arrival at elevator
	// now , we let all lift to try to serive. need be optimized later if necessary
	void SelectBestLiftToService( Person* _pPerson );


};

#endif // !defined(AFX_ELEVATORPROC_H__F007466D_9257_451B_ADC8_C5B3F80163B6__INCLUDED_)
