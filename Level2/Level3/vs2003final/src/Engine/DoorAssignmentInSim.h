//---------------------------------------------------------------------------------------
//			Class:			DoorAssignmentInSim
//			Author:			sky.wen
//			Date:			July 14,2011
//			Purpose:		retrieve door from rules of door specification
//---------------------------------------------------------------------------------------
#pragma once
#include "../Common/containr.h"
#include "../InputOnBoard/EntryDoorOperationalSpecification.h"
class FlightDoorOperationalSpec;
class OnboardFlightInSim;
class OnboardDoorInSim;
class Person;
class DoorOperationalData;
class OnboardDoorInSim;
class OnboardSeatInSim;
class OnboardFlightSeatBlock;

class DoorOperationInSim
{
public:
	DoorOperationInSim(DoorOperationalData* pDoorOperationData);
	~DoorOperationInSim();

	void InitBlockData(OnboardFlightSeatBlock* pFlightSeatBlock);
	bool fit(DoorOperationalData* pData)const;
	bool valid(OnboardSeatInSim* pSeatInSim, Person* pPerson)const;

	ElapsedTime GetStartClosure();
	ElapsedTime GetCloseOperationTime();
	DoorOperationalData::Status getStatus()const;

private:
	bool BlockFit(const ALTObjectID& objID)const;
private:
	DoorOperationalData* m_pDoorOperationData;
	std::vector<OnboardSeatBloceData*> m_vSeatBlockData;
};
//---------------------------------------------------------------------------------------
//Summary:
//		retrieve door by passed passenger
//---------------------------------------------------------------------------------------
class DoorAssignmentInSim : public UnsortedContainer<DoorOperationInSim>
{
public:
	DoorAssignmentInSim(void);
	~DoorAssignmentInSim(void);

public:
	void Initlizate(FlightDoorOperationalSpec* pDoorAssignment,OnboardFlightSeatBlock* pFlightSeatBlock,OnboardFlightInSim* pOnboardFlightInSim); 
	OnboardDoorInSim* GetDoor(OnboardSeatInSim* pSeatInSim,Person* pPerson)const;

	void SetDoorOperationData(OnboardDoorInSim* pDoorInSim);

private:
	DoorOperationInSim* GetDoorOperationData(OnboardDoorInSim* pDoorInSim)const;

private:
	FlightDoorOperationalSpec*		m_pDoorAssignment;
	OnboardFlightInSim*				m_pOnboardFlightInSim;	
};
