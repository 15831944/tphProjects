#pragma once
/**
* Class to provide seats assignment service.
* Both Enplane and deplane agents.
*/

#include "Engine/ServiceObject.h"
#include "../OnBoardDefs.h"
#include "../carrierSpace/carrierSeat.h"
#include "../MobileAgent.h"

class SeatAssignmentService : public ServiceObject < SeatAssignmentService >
{
public:
	SeatAssignmentService();
	~SeatAssignmentService();

	void initAssignmentRules();

	void registerSeat(carrierSeat* pSeat);

	bool assignSeats( MobileAgent* pMobAgent);
	void registerMobAgent(MobileAgent* pMobAgent);

	bool isAllAgentSeatingDown();
	bool isAllAgentLeaveSeats();

	UnsortedContainer< MobileAgent >&  GetMobileAgentList(){ return mMobAgentList; }

private:
	UnsortedContainer< carrierSeat > mSeatList;
	UnsortedContainer< MobileAgent > mMobAgentList;
};

SeatAssignmentService* seatAssignService();
AFX_INLINE SeatAssignmentService* seatAssignService()
{
	return SeatAssignmentService::GetInstance();

}
