#pragma once

#include "MobElementBehavior.h"
#include "Common/ARCVector.h"
#include "PaxOnboardRoute.h"
#include "PaxCellLocation.h"

#define PAXRADIUS 40 

// class PaxOnboardFreeMovingLogic;

class OnboardCellFreeMoveLogic;

class Person;
class OnboardAircraftElementInSim;
//it is the class which remember the pax's statues 
//current status, next status
//when pax's state changes, it record the information after free moving
class OnboardSimStatus
{
public:
	OnboardSimStatus()
	{
		m_pAircraftElement = NULL;
		m_nState = 0;		
	}
	~OnboardSimStatus()
	{

	}

public:
	OnboardAircraftElementInSim *m_pAircraftElement;//the element that the pax is on
	ARCVector3 m_position;//pax's position
	int m_nState;//pax's state
private:
};

class OnboardFlightInSim;
class PaxOnboardBaseBehavior : public MobElementBehavior
{
	friend class PaxOnboardFreeMovingLogic;
	friend class OnboardCellFreeMoveLogic;
	friend class OnboardPaxFreeMovingConflict;

public:	
	PaxOnboardBaseBehavior(Person* pPerson);
	virtual ~PaxOnboardBaseBehavior();

	//deal lock check
	bool bCanWaitFor(const PaxOnboardBaseBehavior* pOtherPax)const;
	void SetWaitPax(PaxOnboardBaseBehavior* pOtherPax){ m_pWaitPax = pOtherPax; }
	//unable to move any further	
	bool IsMovable()const{ return m_bMovable; }
	void SetMovale(bool b){ m_bMovable= b; } 

protected:
	const PaxOnboardBaseBehavior* FindEndMovablePax(const PaxOnboardBaseBehavior* pBreakPax)const;
	OnboardStorageInSim* GetOnboardStorage(GroundInSim* pGroundInSim,const ARCVector3& pt)const;
	std::vector<OnboardStorageInSim*> GetCanReachStorageDevice(GroundInSim* pGroundInSim,const ARCVector3& pt, double dReachDist = 150)const;
	void SetEntryTime(const ElapsedTime& eTime);
	void SetFollowerEntryTime(const ElapsedTime& eTime);

	//-------------------------------------------------------------------------------------
	//Summary:
	//		adjust direction to move to storage
	//-------------------------------------------------------------------------------------
	void AdjustDirectionStorage(ElapsedTime& eTime,OnboardStorageInSim* pStorageInSim,bool bDeplane);
	bool m_bMovable;
	PaxOnboardBaseBehavior* m_pWaitPax;
public:
	//BehaviorType getBehaviorType() const { return MobElementBehavior::OnboardBehavior; }
	virtual void writeLog(ElapsedTime time, bool _bBackup = false, bool bOffset  = true  );
	void writePassengerLog(ElapsedTime time, bool _bBackup = false, bool bOffset  = true  );
	void writeSidleLog(ElapsedTime time,MobDir emWalk = FORWARD);
	void writeVisitorLog(const ARCVector3& locationPt,ElapsedTime time, bool _bBackup = false, bool bOffset  = true );
	void writeVisitorSidleLog(const ARCVector3& locationPt,ElapsedTime time,MobDir emWalk = FORWARD);
	void writeFollowerLog(const ARCVector3& locationPt,PaxVisitor* pVisitor,ElapsedTime time, bool _bBackup = false, bool bOffset  = true );
	void writeFollowerSidleLog(const ARCVector3& locationPt,PaxVisitor* pVisitor,ElapsedTime time,MobDir emWalk = FORWARD);

	virtual void GenerateEvent(ElapsedTime time)=0;
	virtual OnboardFlightInSim* GetOnBoardFlight()const=0;


	virtual OnboardSeatInSim *getSeat() const { return m_pSeat; }

	long m_lCellIndex;

	bool AddWaitingPax(PaxOnboardBaseBehavior *pBehavior);

	void WakeupWaitingPax(ElapsedTime& etime);

	bool IsWaitingPax(const PaxOnboardBaseBehavior *pBehavior) const;

	bool IsWaitingPax(std::vector<const PaxOnboardBaseBehavior *> pBehavior ) const;

	Person *getPerson() const{return m_pPerson;}

	bool TheReseveCouldOverWrite() const { return m_bCouldOverWirteReseve;}

	PaxCellLocation& getCurLocation();

	void SetPaxLocation(const PaxCellLocation& cellLocation);
	ARCVector3 GetRelativePosition();

	void SetGroupBehavior();
	void SetFollowBehavior();

	virtual PaxOnboardBaseBehavior* GetOnboardBehavior(Person* pPerson) = 0;

	PaxOnboardBaseBehavior::MobDir GetDirectionSidle(CPoint2008& dir, bool bMoveIn)const;

	void SetCurrentResource(OnboardAircraftElementInSim *pCurResource);
	OnboardAircraftElementInSim *GetCurrentResource() const;

	//if not assign value, re-write
	double GetVolume(OnboardFlightInSim *pFlight);

private:
	double GetSpeed(MobDir emWalk)const;
	char GetWalkDir(MobDir emWalk)const;

	bool AddVisitorTrack(const ARCVector3& locationPt,PaxVisitor* pVisitor,ElapsedTime time, bool _bBackup = false, bool bOffset  = true );
	bool AddVisitorSidleTrack(const ARCVector3& locationPt,PaxVisitor* pVisitor,ElapsedTime time,MobDir emWalk = FORWARD);

	bool WriteVisitorEntryLog(const ARCVector3& locationPt,PaxVisitor* pVisitor,ElapsedTime time, int ntype,bool _bBackup = false, bool bOffset  = true );
	bool WriteVisitorEntrySidleLog(const ARCVector3& locationPt,PaxVisitor* pVisitor,ElapsedTime time,int ntype,MobDir emWalk = FORWARD);

protected:
	//Current coordinates
	ARCVector3 location;       

	//Current destination, used for
	ARCVector3 m_ptDestination;      

	//pure location( no offset )
	ARCVector3 pure_location;

	Person* m_pPerson;

	OnboardDoorInSim *m_pDoor;

	OnboardSeatInSim *m_pSeat;

	OnboardAircraftElementInSim *m_pCurResource;

	OnboardDeviceInSim* m_pDeviceInSim;//pax choose device move form one deck to another deck
	OnboardSimStatus m_curStatus;
	OnboardSimStatus m_nextStatus;
	PaxOnboardRoute m_route;


	//Mobile behavior in onboard position interface
	//PaxOnboardFreeMovingLogic* m_freeMovingLogic; //moving on pipe

	OnboardCellFreeMoveLogic *m_pCellFreeMoveLogic;
	OnboardFlightInSim *m_pOnboardFlight;

	PaxCellLocation m_curLocation;

	//passenger who wait passenger
	std::vector< PaxOnboardBaseBehavior *> m_vWaitingPax;

	//the direction in logic move, if the passenger in logic move, the member would be used to calculate the passenger's rect
	CPoint2008 m_vecDirction;

	//the flag which mark the location could be erased or not
	bool m_bCouldOverWirteReseve;

	double GetWidth()const;  //cm
	double GetThick()const;  //cm dist 
	double GetRadiusOfConcern()const;

	const static int PAXWIDTH = 40;
	const static int PAXLENGTH = 20;

	double m_dVolume;

public:
	//Returns current coordinates
	const ARCVector3& getPoint (void) const { return location; }

	//Returns current destination.
	const ARCVector3& getDest (void) const { return m_ptDestination; }
	void SetRelativePosition(const Point& paxDirection,const Point& paxLocation,MobDir emWalk);

	// get pure location
	const ARCVector3& getPureLocation( void ) const { return pure_location; }

	ElapsedTime moveTime (void) const;

	ElapsedTime sidleMoveTime(MobDir emWalk)const;

	ElapsedTime moveTime( DistanceUnit _dExtraSpeed, bool _bExtra ) const;

	//Sets Element's destination
	virtual void setDestination (const ARCVector3& p,MobDir emWalk = FORWARD);

	virtual void setLocation( const ARCVector3& _ptLocation ){ location = _ptLocation;	}

	void setState (short newState);
	int getState (void) const;

	//passenger enplane enter onboard visitor and group follower did not have location
	void SetEnterOnboardLocation(const ARCVector3& _ptCurrent);
	void SetFollowerLocation(const ARCVector3& _ptCurrent);

	void SetEnplaneTime(const ElapsedTime& time);
	void SetFollowerEnplaneTime(const ElapsedTime& time);

	void RemoveWaitingPax(int nPaxID, ElapsedTime eTime);

protected:
	enum theVisitorLogNum
	{
		firstLog = 0,
		secondLog,
	};
	void SetFollowerDestination(const ARCVector3& _ptCurrent, const ARCVector3& _ptDestination, float* _pRotation );
	OnboardDeviceInSim* SelectDeviceMoveDest(const OnboardCellInSim* pOriginalCell,const OnboardCellInSim* pDestCell);
	void WriteCellPathLog(OnboardCellPath& cellPath,ElapsedTime& p_time);
	void WriteSidleCellPathLog(OnboardCellPath& cellPath,ElapsedTime& p_time,MobDir emWalk);

	void writeCurrentLogItem(PaxVisitor* pVisitor,Point _OwnerPreLocation, theVisitorLogNum logNum = secondLog,MobDir emWalk = FORWARD);
	void RepeatFollowersLastLogWithNewDest(PaxVisitor* pVisitor);
	void RepeatLastLogWithNewDest(PaxVisitor* pVisitor);
protected:
	//set next status of the pax
	void SetNextStatus(int nState, ARCVector3& pos, OnboardAircraftElementInSim *pElement);

};
