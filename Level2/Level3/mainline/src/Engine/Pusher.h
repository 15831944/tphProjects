// Pusher.h: interface for the Pusher class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PUSHER_H__93422E31_C743_433D_AB63_AAFD4F4AAF2E__INCLUDED_)
#define AFX_PUSHER_H__93422E31_C743_433D_AB63_AAFD4F4AAF2E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "conveyor.h"
#include "ServiceSlot.h"
#include "common\states.h"

class AirsideBaggageTrainInSim;

class Pusher : public Conveyor  
{
	std::vector<ServiceSlot>m_vServiceSlot;
	bool m_bNeedToCheckMaxWaitTime;
	bool m_bReleaseFirstIfFull;
	bool m_bReleaseBaggageIfDestProcAvailable;
	bool m_bShouldGenerateScheduleEvent;
	bool m_bHasGeneratedScheduleEvent;
	
	ElapsedTime m_timeMaxWaittime;
	ElapsedTime m_timeSchecduleInterval;
	double m_dMovingSpeed;

public:
	Pusher();
	virtual ~Pusher();
public:
	//init pusher's service slots and every flag
	virtual void InitData( const MiscConveyorData * _pMiscData );
	virtual void beginService (Person *_pPerson, ElapsedTime curTime);
	virtual bool isVacant( Person* pPerson  = NULL )const;

	// notify conveyor that one of its dest processor is available now
	virtual bool destProcAvailable ( const ElapsedTime& _tCurrentTime );
public:
	
	//When PusherCheckMaxWaitTimeEvent comes, 
	//we travel the whole service slots to find if the interst baggage is still on the pusher, 
	//if yes, release it
	void ReleaseBaggageIfStillOnPusher( long _lWhichBaggage , const ElapsedTime& _tEventTime );

	// release all baggage on pusher when schedule event comes
	void ReleaseAllBaggageOnPusher( const ElapsedTime& _tEventTime );

	// enable generate schedule event
	void NeedGenerateScheduleEvent(){ m_bHasGeneratedScheduleEvent = false;	}
	
	void ReleaseBaggageToBaggageCart(AirsideBaggageTrainInSim *pBaggageTrain, CMobileElemConstraint& bagCons, int nReleaseCount, ElapsedTime eTime);

	bool HaveBagOfFlight(int nFlightIndex);
private:
	//to generate a max time checking event in order to release baggage which is on pusher more than max time
	void GenerateCheckMaxTimeEvent( Person* _pPerson, const ElapsedTime& _eventTime );

	// to generate a schedule event in order to release all baggage on pusher
	void GenerateScheduleEvent(  const ElapsedTime& _eventTime );

	//when a new baggage comes, firstly we should check if the pusher is full, 
	//that is all slot are not empty, if it is full,  then release the first coming baggage, 
	//and move all other baggages forward , finally, let the new baggage enter into the last service slot
	void ReleaseFirstBaggageIfFull( Person *_pPerson, const ElapsedTime& _eventTime );

	//try to move forward  every baggage on pusher ,that is,
	//for each baggage on pusher, if next service slot is empty, 
	//then move forward, otherwise ,stay on current service slot
	void MoveForwardBaggageOnPusher( const ElapsedTime& _eventTime , int _iStartPos, int _iEndPos );

	// release any baggage on pusher ramdomly
	bool RandomReleaseBaggage( const ElapsedTime& _tCurrentTime );

	//check if there is no service slot can be available
	bool IsFull()const;

	// return the baggage count on pusher now
	int GetBaggageOnPusherCount()const;

	void writeAdditionLog( EntityEvents _eState, const ElapsedTime& _time, long _lReason ) ;


private:

	//add person into approach list 
	virtual void addPersonIntoApproachList( Person *_pPerson );
	virtual void removePersonFromApproachList(const Person* _pPerson);

protected:
	virtual long getLoad()const;



public:
	void ReportBaggageTrainArrival(AirsideBaggageTrainInSim *pBaggageTrain);

	void ReportBaggageTrainLeave(AirsideBaggageTrainInSim *pBaggageTrain);
protected:
	std::vector<AirsideBaggageTrainInSim *> m_vBaggageTrain;
};

#endif // !defined(AFX_PUSHER_H__93422E31_C743_433D_AB63_AAFD4F4AAF2E__INCLUDED_)





















