#pragma once
#include "MobElementBehavior.h"
#include "pax.h"
#include "terminal.h"
#include "Common/Point2008.h"


class ENGINE_TRANSFER AirsideMobElementBehavior :  public MobElementBehavior
{
public:
	AirsideMobElementBehavior(Person* pax) ;
	AirsideMobElementBehavior(Person* pax ,int curentstate) ;
	virtual ~AirsideMobElementBehavior() ;

	virtual int performanceMove(ElapsedTime p_time,bool bNoLog);
	BehaviorType getBehaviorType()const{ return MobElementBehavior::AirsideBehavior; }

private :
	Person* m_pax ; 
	BOOL m_IsOnBus ;

	CPoint2008 _offSetInBus ;
    float  m_ZPosition ;

	//Current coordinates
	CPoint2008 location;       

	//Current destination, used for
	CPoint2008 m_ptDestination;      

	//pure location( no offset )
	CPoint2008 pure_location;

	//check pax have pax bus service, just for take off passenger
	bool m_bPaxBusSerivce;

	//check pax bus is arrival
	bool m_bArrivalPax;
	//pax bus for arrival passenger
	CAirsidePaxBusInSim* m_pPaxBus;

//Mobile behavior in airside position interface
public:
	//Returns current coordinates
	const CPoint2008& getPoint (void) const { return location; }

	//Returns current destination.
	const CPoint2008& getDest (void) const { return m_ptDestination; }

	// get pure location
	const CPoint2008& getPureLocation( void ) const { return pure_location; }

	ElapsedTime moveTime (void) const;

	ElapsedTime moveTime( DistanceUnit _dExtraSpeed, bool _bExtra ) const;

	//Sets Element's destination
	virtual void setDestination (CPoint2008 p);

	virtual void setLocation( const CPoint2008& _ptLocation ){ location = _ptLocation;	}

	void CancelPaxBusService(){m_bPaxBusSerivce = false;}
	void SetAirsideBus(CAirsidePaxBusInSim* pBus){m_pPaxBus = pBus;}
public:
	void setState (short newState);
	int getState (void) const;

	CPoint2008 GetOffSetInBus() {return _offSetInBus ;}
	float GetZPrecent() { return m_ZPosition ;} ;
	int	WriteLog(ElapsedTime time,const double speed = 0, enum EntityEvents enumState = FreeMoving) ;

	virtual void writeLog (ElapsedTime time, bool _bBackup, bool bOffset = true ){};
	
	void WakeupHoldArea(ElapsedTime time);
protected:
	BOOL GetStandPosition(CPoint2008& point) ;
	CPoint2008 GetStandWaiteAreaPosition() ;
	BOOL GetGatePosition(CPoint2008& point) ;
	CPoint2008 GetGateWaiteAreaPosition() ;
	BOOL GetBusPosition(CPoint2008& point) ;
	BOOL  IsBusArrived() ;
	BOOL  IsGateOpen() ;
	BOOL  IsFlightArrived() ;
	void GenetateEvent(ElapsedTime time) ;

	bool GetFlightDoorPostion(CPoint2008& doorpoint, CPoint2008& groundpoint);

	void SetFollowerDestination(const CPoint2008& _ptCurrent, const CPoint2008& _ptDestination, float* _pRotation );



protected:
	void ProcessWhenTakeOffFlight(ElapsedTime time) ;
	void ProcessWhenWaitforBus(ElapsedTime time) ;
	void ProcessWhenTakeOnBus(ElapsedTime time) ;
	void ProcessWhenTakeOffBusToGate(ElapsedTime time);
	void ProcessWhenWaitForGateOpen(ElapsedTime time) ;
	void ProcessWhenArriveAtGate(ElapsedTime time) ;
	void ProcessWhenWalkOutGate(ElapsedTime time) ;
	void ProcessWhenTakeOffBusToFlight(ElapsedTime time) ;
	void ProcessWhenWaitForFlight(ElapsedTime time) ;
	void ProcessWhenTakeOnFlight(ElapsedTime time ) ;
	void ProcessWhenPassengerDeath(ElapsedTime time) ;
	void ProcessHoldAreaToBus(ElapsedTime time);

	void ProcessWhenTakeOffFlightWithNoBus(ElapsedTime time) ;

	CPoint2008 GetServiceHoldAreaPoint();

	void ResetTerminalToAirsideLocation();

	void ProcessEntryOnbaord(ElapsedTime time);

	//in airside behavior, do not use mobile element setDestnation, setLocation any more
	void setDestination(const Point& destinationPt, bool bRealAttitude);
	void setLocation(const Point& loactionPt, bool bRealAttitude);

	bool ConvertPointToRealAttitude(Point& point)const;
	BOOL GetGateTerminalPoint(Point& point);

private:
	void CloseDoor(const ElapsedTime& time);
};
