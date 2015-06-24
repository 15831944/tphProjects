#pragma once
#include "OnboardAircraftElementInSim.h"
#include "Common/ARCVector.h"
#include "PaxCellLocation.h"
class OnboardCorridorInSim;
class ARCVector3;
class OnboardSeatInSim;
class Person;
class OnboardCellInSim;
class OnboardCellSeatGivePlaceLogic;
class GroundInSim;
class OnboardStorageInSim;

#define EntryPointString _T("Entry Point")
class EntryPointInSim : public OnboardAircraftElementInSim
{
public:
	enum WalkThroughtType
	{
		Straight_Walk,
		TurnDirection_Walk
	};

	EntryPointInSim(OnboardCorridorInSim* pCorridorInSim,OnboardFlightInSim* pFlightInSim);
	EntryPointInSim(OnboardCellInSim* pCellInSim,OnboardFlightInSim* pFlightInSim);
	~EntryPointInSim();

public:
	void SetWalkType(WalkThroughtType emWalk);
	WalkThroughtType GetWalkType()const;

	void SetDistance(double dDist);
	double GetDistance()const;

	void SetLocation(const ARCVector3& location);
	bool GetLocation(ARCVector3& location)const;

	virtual void CalculateSpace(){}

	OnboardCorridorInSim* GetCorridor()const;
	virtual CString GetType()const {return EntryPointString;}

	virtual int GetResourceID() const;


	void SetCreateSeat(OnboardSeatInSim *pSeat);
	OnboardSeatInSim* GetCreateSeat() { return m_pSeatInSim;}

	OnboardCellInSim* GetOnboardCell()const {return m_pCellInSim;}
	virtual OnboardCellInSim *GetEntryCell() const{return m_pCellInSim;}
	BOOL IsAvailable() const;

	//will return the checking point list
	//if has no creat, will generate 2 checking point
	//  C---20---E---20---C
	//C, checking point, E, entry point, 20, 20cm distance between C&E
	std::vector<PaxCellLocation > GetCheckingPoints();


	//return the logic
	//if Null, create new
	OnboardCellSeatGivePlaceLogic *GetGivePlaceLogic();

	//checking location
	//find tmp place at another checking location
	bool CalculateTempLocation( PaxCellLocation& checkingLocation, PaxCellLocation & paxLocation ) const;


	bool Reserve(PaxOnboardBaseBehavior *pCurPax);
	void CancelReserve(PaxOnboardBaseBehavior *pCurPax);

	bool GetAnotherCheckingLocation(PaxCellLocation& curLocation, PaxCellLocation& nextCellLocation) const;

	PaxCellLocation GetEntryLocation() const {return m_entryLocation; }
private:
	//the logic could help the passenger moves to seat
	OnboardCellSeatGivePlaceLogic *m_pGivePlaceLogic;
	void CreateEntryPosition();

protected:
	WalkThroughtType m_emType;
	double m_dDistFromStart;
	OnboardCorridorInSim* m_pCorridorInSim;

	//use the seat to create the entry point to row or seat;
	OnboardSeatInSim *m_pSeatInSim;

	ARCVector3 m_location;

	//the cell where the entry point located at
	OnboardCellInSim* m_pCellInSim;

	//checking point list
	std::vector<PaxCellLocation > m_vCheckingPoint;

	PaxCellLocation m_entryLocation;
};