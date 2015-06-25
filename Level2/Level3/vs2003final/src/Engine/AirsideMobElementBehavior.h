#pragma once
#include "pax.h"
#include "terminal.h"
#include "Common/Point2008.h"
#include "AirsideMobElementBaseBehavior.h"


class ENGINE_TRANSFER AirsidePassengerBehavior :  public AirsideMobElementBaseBehavior
{
public:
	AirsidePassengerBehavior(Person* pax) ;
	AirsidePassengerBehavior(Person* pax ,int curentstate) ;
	virtual ~AirsidePassengerBehavior() ;

	virtual int performanceMove(ElapsedTime p_time,bool bNoLog);

private :
	Person* m_pax ; 
	BOOL m_IsOnBus ;

	CPoint2008 _offSetInBus ;
    float  m_ZPosition ;


	 


	//check pax have pax bus service, just for take off passenger
	bool m_bPaxBusSerivce;

	//check pax bus is arrival
	bool m_bArrivalPax;
	//pax bus for arrival passenger
	CAirsidePaxBusInSim* m_pPaxBus;

//Mobile behavior in airside position interface
public:





	void CancelPaxBusService(){m_bPaxBusSerivce = false;}
	void SetAirsideBus(CAirsidePaxBusInSim* pBus){m_pPaxBus = pBus;}
public:

	CPoint2008 GetOffSetInBus() {return _offSetInBus ;}
	float GetZPrecent() { return m_ZPosition ;} ;

	virtual int	WriteLog(ElapsedTime time,const double speed = 0, enum EntityEvents enumState = FreeMoving) ;
	
	void WakeupHoldArea(ElapsedTime time);

	void setDestination(CPoint2008 p);

protected:
	BOOL GetStandPosition(CPoint2008& point) ;
	CPoint2008 GetStandWaiteAreaPosition() ;
	BOOL GetGatePosition(CPoint2008& point) ;
	CPoint2008 GetGateWaiteAreaPosition() ;
	BOOL GetBusPosition(CPoint2008& point) ;
	BOOL  IsBusArrived() ;
	BOOL  IsGateOpen() ;
	BOOL  IsFlightArrived() ;


	bool GetFlightDoorPostion(CPoint2008& doorpoint, CPoint2008& groundpoint);



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

	void ProcessEntryOnbaord(ElapsedTime time);

	BOOL GetGateTerminalPoint(Point& point);


private:
	void CloseDoor(const ElapsedTime& time);
	void SetFollowerDestination(const CPoint2008& _ptCurrent, const CPoint2008& _ptDestination, float* _pRotation );
};
