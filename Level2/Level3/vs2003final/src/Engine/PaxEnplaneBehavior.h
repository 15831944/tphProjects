#pragma once
#include "PaxOnboardBaseBehavior.h"
class Person;
class CDoor;
class OnboardSeatInSim;
class OnboardFlightInSim;
class OnboardStorageInSim;
class OnboardDoorInSim;

class ENGINE_TRANSFER PaxEnplaneBehavior : public PaxOnboardBaseBehavior
{
	friend class OnboardCellSeatGivePlaceLogic;
public:
	PaxEnplaneBehavior(Person* pPerson);
	virtual ~PaxEnplaneBehavior();

public:
	virtual int performanceMove(ElapsedTime p_time,bool bNoLog);
	virtual BehaviorType getBehaviorType() const { return MobElementBehavior::OnboardBehavior; }


	virtual OnboardFlightInSim* GetOnBoardFlight()const{ return m_pOnboardFlight; }


	virtual PaxOnboardBaseBehavior* GetOnboardBehavior(Person* pPerson);
	void SeatOperation(ElapsedTime& p_time);


	void StorageOperation(ElapsedTime& time,std::vector<OnboardStorageInSim*> vStorageInSim);


	void CarryonMoveToDeckGround(ElapsedTime& time);
	void CarryonMoveToSeatFront(ElapsedTime& time);

	void VisitorGroupSeparateAtSeat(ElapsedTime& p_time,PaxVisitor* pVis);
	void VisitorGroupSeparateAtStorage(ElapsedTime& time,PaxVisitor* pVis,std::vector<OnboardStorageInSim*> vStorageInSim);
private:
	//EntryOnboard handle the initialization
	//such as, seat, route to seat ...
	void processEnterOnBoardMode(ElapsedTime p_time);
	
	void processWayFinding(ElapsedTime p_time);

	//arrive at entry door
	void processArriveAtEntryDoor(ElapsedTime p_time);

	void processArriveAtCheckingPoint( ElapsedTime p_time );

	//from seat entry to 
	void processArriveAtSeatEntry(ElapsedTime p_time);
	
	void processReturnToCheckingPoint(ElapsedTime p_time);
	//arrive at storage
	void processArriveAtStorage(ElapsedTime p_time);

	//pax arrives at seat
	void processArriveAtSeat(ElapsedTime p_time);

	//pax arrives at device entry
	void processArrivalAtDeviceEntry(ElapsedTime p_time);

	//pax moves on device
	void processMovesOnDevice(ElapsedTime p_time);

	//pax leaves device
	void processLeavesDevice(ElapsedTime p_time);

	//pax leaves device to dest
	void ProcessMoveFromDevice(ElapsedTime p_time);
	
	//death
	void processDeath(ElapsedTime p_time);

	
	//generate next event base on the situation
	void GenerateEvent(ElapsedTime time);
	void GenerateEvent(Person *pPerson, ElapsedTime eventTime );
	//flush the pax log into file after the pax died
	void FlushLog(ElapsedTime p_time);

	void SeparateAtSeat(ElapsedTime& p_time,PaxVisitor* pVis);
private:

	bool CalculateMoveToStorageCarrion(std::vector<PaxVisitor*>& vVisitorList);
	OnboardStorageInSim* GetStorage();
	ARCVector3 GetStorageLocation(OnboardStorageInSim *pStorage) const;

	void SeparateAtStorageOperation(ElapsedTime& time,PaxVisitor* pVisitor,OnboardStorageInSim* pStorageInSim);
	//first, come to seat checking location	
	//find the path
	//passed, seat, door
	//return cell list
	void FindCellPathToSeat(OnboardDoorInSim *pDoor, OnboardSeatInSim *pSeatInSim,OnboardCellPath& movePath ) const;

	void FindCellPathToSeat(OnboardCellInSim *pStartCell, OnboardSeatInSim *pSeatInSim, OnboardCellPath& movePath ) const;

private:
	double m_dDistCheckingPointInCorridor;

	double m_dDistTempPlaceInCorridor;

	mutable PaxCellLocation m_paxCheckingLocation;

	OnboardCellPath m_routeDeviceToSeat;
protected:
};