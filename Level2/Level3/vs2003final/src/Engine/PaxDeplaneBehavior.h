#pragma once
#include "PaxOnboardBaseBehavior.h"

class Person;
class CDoor;
class OnboardSeatInSim;
class OnboardFlightInSim;
class OnboardStorageInSim;
class OnboardDoorInSim;
class PaxVisitor;



class PaxDeplaneBehavior : public PaxOnboardBaseBehavior
{
public:
	PaxDeplaneBehavior(Person* pPerson);
	virtual ~PaxDeplaneBehavior();

public:
	virtual int performanceMove(ElapsedTime p_time,bool bNoLog);
	virtual BehaviorType getBehaviorType() const { return MobElementBehavior::OnboardBehavior; }
	virtual OnboardFlightInSim* GetOnBoardFlight()const{ return m_pOnboardFlight; }
	
	void setFlight(OnboardFlightInSim* pFlight);
	void setSeat(OnboardSeatInSim* pSeat);
	void GenerateEvent(ElapsedTime time) ;
	void GenerateEvent(Person *pPerson, ElapsedTime time );

	virtual PaxOnboardBaseBehavior* GetOnboardBehavior(Person* pPerson);

private:
	void processEnterOnBoardMode(ElapsedTime p_time);
	void processWayFinding(ElapsedTime p_time);
	
	void processSittingOnSeat(ElapsedTime p_time);

	void processStandUp(ElapsedTime p_time);

	void processArriveAtSeatEntry(ElapsedTime p_time);

	//pax arrives at device entry
	void processArrivalAtDeviceEntry(ElapsedTime p_time);

	//pax moves on device
	void processMovesOnDevice(ElapsedTime p_time);

	//pax leaves device
	void processLeavesDevice(ElapsedTime p_time);

	//pax leaves device to dest
	void ProcessMoveFromDevice(ElapsedTime p_time);

	void processArriveAtCheckingPoint( ElapsedTime p_time );
	//pax move from seat to seat that create entry point
	void processStrideOverSeat(ElapsedTime p_time );
	//arrive at storage
	void processArriveAtStorage(ElapsedTime p_time);


	void processArriveAtExitDoor(ElapsedTime p_time);

	void processEntryAirside(ElapsedTime p_time);
	void processEntryTerminal(ElapsedTime p_time);

	void FlushLog(ElapsedTime p_time);

	bool AssignSeatForFollower(ElapsedTime p_time);
	void processAssignSeat(ElapsedTime p_time);

protected:

	bool AssignStorageDevice(const ElapsedTime& eTime);

	//get the carryon list belong to this passenger
	//such as handbag, backpack, purse ...
	//not include pax visitor
	//void GetCarryon(std::vector<PaxVisitor*>& vCarryOn) const;
	
	//storage carryon into device
	bool AssignCarryOnIntoDevice( const ElapsedTime& eTime, PaxVisitor* pCarryon, OnboardStorageInSim* pStorageInSim);

	//merge the carryon to this passenger
	//passenger get all his carryon from storage 
	void PickUpCarryonsFromDevice( ElapsedTime& eTime );

	//the time would be changed
	void PickUpCarryOnFromStorage(ElapsedTime& eTime, PaxVisitor* pCarryon, OnboardStorageInSim *pStorage);


	void RegroupAndAttachCarryOn(const ElapsedTime& eTime, std::vector<PaxVisitor *> vCarryon);

	//put  the carryon into seat staging
	void AssignCarryonToSeat(const ElapsedTime& eTime);

	//for one carryone
	void AssignCarryOnIntoSeat( const ElapsedTime& eTime, PaxVisitor* pCarryon);


	void PickUpCarryOnFromSeat(ElapsedTime& eTime);

	//pick up the carryon before stand up
	void PickUpCarryOnFromSeat(ElapsedTime& eTime, PaxVisitor* pCarryon);

	//put down when arrive at storage, put down the carryon, in case to get his carryon
	void PutDownCarryon(ElapsedTime& eTime);

private:
	void setState (short newState);
	int getState (void) const;

	OnboardStorageInSim* GetStorage();
	ARCVector3 GetStorageLocation(OnboardStorageInSim *pStorage) const;
	
	//can lead to entry point for deplane
	bool CanLeadToEntryPoint();

	//create entry point seat to entry point direction
	//PaxOnboardBaseBehavior::MobDir GetDirectionSidle(CPoint2008& dir)const;
private:
	//OnboardDoorInSim *m_pDoor;

	bool bDisallowGroup;

public:
	//the time that the passenger would plan to stand up
	ElapsedTime m_eStandupPlaneTime;
	bool m_bGenerateStandupTime;//stand up time just generate one time. when great stand up time need check lead to entry point

	//storage the passenger visitor and the storage
	std::map<PaxVisitor *, OnboardStorageInSim *> m_mapCarryonStorage;

	std::vector<PaxVisitor *> m_vCarryonAtSeat;

};