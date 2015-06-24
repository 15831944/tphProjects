#ifndef TRANSITING_PASSENGER_H
#define TRANSITING_PASSENGER_H


#include "engine\pax.h"
#include "common\containr.h"
class CARCportEngine;

class TurnaroundPassenger : public Passenger
{
	friend class TurnaroundPaxTerminalBehavior;
	friend class TerminalMobElementBehavior;
protected:
    // Could be : DEPARTING; ARRIVING; TURNAROUND.
    int direction;

    // Gate index.
    int arrivingGate;
	
	MobLogEntry m_logMobEntry;
    // Gate index.
    int departingGate;

    // Passenger type.
    CMobileElemConstraint arrivingType;

    // Passenger type.
    CMobileElemConstraint departingType;

    ElapsedTime departureTime;

public:
    TurnaroundPassenger (MobLogEntry& p_entry, CARCportEngine* _pEngine );
    virtual ~TurnaroundPassenger () {};
	
	
	// Used to determine if the gate is used as arrival purpose.
	virtual bool CheckAndTurnArrState();
	
	// new person for group follower
	virtual void newGroupFollower( void );

	virtual const CMobileElemConstraint &getArrivalType(void) const {return arrivingType;}

//	virtual CSinglePaxTypeFlow* TryToGetPathInProcess( CSubFlow* _pProcess,const ElapsedTime& _curTime );
};

#endif
