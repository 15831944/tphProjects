#pragma once
#include "PaxCellLocation.h"
#include "PaxOnboardBaseBehavior.h"


class PaxOnboardBaseBehavior;
class OnboardSeatRowInSim;
class ElapsedTime;
class EntryPointInSim;
class PaxEnplaneBehavior;
class OnboardSeatInSim;
class ARCVector3;


class OnboardCellSeatGivePlaceLogic
{
public:
	OnboardCellSeatGivePlaceLogic(EntryPointInSim *pEntryPoint);
	~OnboardCellSeatGivePlaceLogic(void);

public:
	OnboardSeatRowInSim *m_pSeatRow;
	EntryPointInSim *m_pEntryPoint;

public:
	//Passenger arrive at Checking Place(at point ahead the entry point)
	void ReportArrivalCheckingPlace(PaxOnboardBaseBehavior *pArrivePax, const ElapsedTime& eArrTime);
	
	//handle the passenger state event
	//Arrive At Checking point
	//Arrive At Entry Point
	//Arrive At Seat
	int ProcessEvent(PaxOnboardBaseBehavior *pPax, const ElapsedTime& eTime);
protected:
	//check if the passenger need to give place or not
	//true, need ; false, no need, pCurPax, the current passenger would move to the seat 
	bool CheckIfNeedGivePlace( PaxOnboardBaseBehavior *pCurPax,std::vector<PaxOnboardBaseBehavior *>& vPaxNeedGivePlace ) const;
	
	bool IsPaxArrivalAtSeatRow(PaxOnboardBaseBehavior *pPax) const;

protected:

	//Arrive At Checking Point
	void processArriveAtCheckingPoint(PaxOnboardBaseBehavior *pCurPax, ElapsedTime p_time );
	
	void processArriveAtStorage(PaxOnboardBaseBehavior *pCurPax, ElapsedTime p_time );
	
	void processReturnToCheckingPoint(PaxOnboardBaseBehavior *pCurPax, ElapsedTime p_time );
	//arrive at checking point
	void processWaitingAtCheckingPoint(PaxOnboardBaseBehavior *pCurPax, ElapsedTime p_time );


	void processStartStrideOverSeat(PaxOnboardBaseBehavior *pCurPax, ElapsedTime p_time );

	//from seat entry to 
	void processArriveAtSeatEntry(PaxOnboardBaseBehavior *pCurPax,ElapsedTime p_time);
	
	//arrive at temp place
	void processWaitAtTmpPlace(PaxOnboardBaseBehavior *pCurPax,ElapsedTime p_time);

	//pax arrives at seat
	void processStowCarryonToSeat(PaxOnboardBaseBehavior *pCurPax,ElapsedTime p_time);

	//pax arrives at seat
	void processArriveAtSeat(PaxOnboardBaseBehavior *pCurPax,ElapsedTime p_time);

	//calculate give place space
	void CalculateGivePlaceSpace(PaxOnboardBaseBehavior *pCurPax, std::vector<PaxOnboardBaseBehavior *>& vPaxNeedGivePlace);

	PaxCellLocation getCheckingLocation();
	bool IsClearGivePlace(std::vector<PaxOnboardBaseBehavior *>& vPaxNeedGivePlace);

	//void LockThePalace();

	//move to checking point
	void MoveToCheckingPoint();

protected:
	bool ReserveGivePlaceSpace(PaxOnboardBaseBehavior *pCurPax);
	void CancelReservedGivePlaceSpace(PaxOnboardBaseBehavior *pCurPax);

	//check the location are available
	bool IsReady(PaxOnboardBaseBehavior *pCurPax);

	//process give place operation

	void StartGivePlaceOperation(PaxOnboardBaseBehavior *pArrivePax, std::vector<PaxOnboardBaseBehavior *>& vPaxNeedGivePlace, const ElapsedTime& eArrTime);


private:
	//************************************
	// Method:    MoveFromSeatToTmpPlace
	// FullName:  OnboardCellSeatGivePlaceLogic::MoveFromSeatToTmpPlace
	// Access:    private 
	// Returns:   void
	// Qualifier:
	// Parameter: PaxOnboardBaseBehavior * pPax
	// Parameter: OnboardSeatInSim * pSeat
	// Parameter: PaxCellLocation & tmpPlace
	// Parameter: ElapsedTime & eStartTime
	// move to temp place from seat, seating logic
	//************************************
	void MoveFromSeatToTmpPlace(PaxOnboardBaseBehavior *pPax, OnboardSeatInSim *pSeat, PaxCellLocation& tmpPlace, ElapsedTime& eStartTime );

	void MoveFromTmpPlaceToSeat(PaxOnboardBaseBehavior *pPax, OnboardSeatInSim *pSeat,  ElapsedTime& eStartTime);


	//************************************
	// Method:    MoveTo
	// FullName:  OnboardCellSeatGivePlaceLogic::MoveTo
	// Access:    private 
	// Returns:   void
	// Qualifier: const
	// Parameter: PaxOnboardBaseBehavior * pPax
	// Parameter: ARCVector3 & ptDest
	// Parameter: ElapsedTime & eTime
	// Function: write log, to the given destination
	//************************************
	void MoveTo( PaxOnboardBaseBehavior *pPax,ARCVector3& ptDest ,ElapsedTime& eTime) const;

	void MoveTo( PaxOnboardBaseBehavior *pPax,ARCVector3& ptDest ,ElapsedTime& eTime, PaxOnboardBaseBehavior::MobDir enumDir) const;
	//add new event
	//for the pax, pCurPax, at time, etime
	void AddEvent(PaxOnboardBaseBehavior *pCurPax, const ElapsedTime& eTime);
protected:
	//remember the passenger has been arrive at the seat row
	//passed checking point
	std::vector<PaxOnboardBaseBehavior *> m_vPaxArrived;

	//passenger waiting location
	std::vector<PaxCellLocation> m_vTmpWaitingLocation;

	std::vector<PaxOnboardBaseBehavior *> m_vPaxAtTempPlace;

	PaxEnplaneBehavior *m_pCurPax;
	
	//the flag to indicate the entry point is in give place operation
	bool m_bInProcessing;

	std::vector<PaxOnboardBaseBehavior *> m_vPaxAtCheckingPoint;
};
