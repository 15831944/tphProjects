/*****
*
*   Class       MobileElement
*   Author      Lincoln Fellingham, Khacthu Nguyen
*               Aviation Research Corporation
*   Date        February 29, 1993
*
*   Purpose     Base class for all mobile elements in Arcterm
*               Has the key data and functions required for writing log
*               files for animation and analysis
*
*****/

#ifndef MOBILE_ELEMENT_H
#define MOBILE_ELEMENT_H

#include "../common/containr.h"
#include "../common/point.h"
#include "../common/elaptime.h"
#include "../common/util.h"
#include  "MobElementBehavior.h"
class TerminalMobElementBehavior;
class AirsideMobElementBehavior;
class PaxOnboardBaseBehavior;
class LandsideBaseBehavior;
/*
The MobileElement is a base class for all mobile elements in Arcterm.
It has the key data and functions required for writing log files for animation and analysis.
*/

class MobileElement
{
protected:
	ElapsedTime m_prevEventTime;

    //Unique identification code.
    long  id;               

    //Current event type.
    int state;          
    
	// previous state
	int m_iPreState;
	
    //Travelling speed in DistanceUnits / sec
    DistanceUnit speed;     

private:
	MobElementBehavior *m_spBehavior;
	MobElementBehavior *m_spPrevBehavior;

	std::vector<MobElementBehavior *> m_vBehavior;
public:
	int M_DELIDX;

    MobileElement (long p_id, DistanceUnit p_speed)
	{
		id = p_id; speed = p_speed; m_prevEventTime = -1l; M_DELIDX = -1;  m_spBehavior = NULL; m_spPrevBehavior = NULL;
	}
    virtual ~MobileElement ();

	virtual void setBehavior(MobElementBehavior* _behavior);
	virtual MobElementBehavior* getCurrentBehavior()const;
	virtual MobElementBehavior* getBehavior( MobElementBehavior::BehaviorType emType);

	TerminalMobElementBehavior* getTerminalBehavior()const;
	AirsideMobElementBehavior* getAirsideBehavior()const;
	PaxOnboardBaseBehavior* getOnboardBehavior()const;
	LandsideBaseBehavior* getLandsideBehavior()const;

	//when the passenger flush log, or died in engine, clear behavior
	void ClearBehaviors();

	ElapsedTime GetPrevEventTime() const { return m_prevEventTime; }

	void SetPrevEventTime( ElapsedTime _time ){ m_prevEventTime = _time; }

    //Returns the unique identification code.
    long getID (void) const { return id; }

    //Returns current event type.
    int getState (void) const { return state; }

	// return previous mobile element state
	int getPreState()const{ return m_iPreState;	}

    //Returns travelling speed.
    DistanceUnit getSpeed (void) const { return speed; }

    //Sets current event type.
    void setState (short newState) { m_iPreState = state; state = newState; }

    //Sets travelling speed.
    void setSpeed (DistanceUnit p_speed) { speed = p_speed; }

    //Writes the element's current location, state to the log file.
    //Empty body.
	// in:
	// _bBackup: if the log is backwards.
    virtual void writeLogEntry (ElapsedTime time, bool _bBackup, bool bOffset = true ) {};

    //Event handling routine for all movements.
    //Returns zero.
    virtual int move (ElapsedTime currentTime,bool bNoLog);

    //Performs all required functions before element's destruction
    //Empty body.
    virtual void kill (ElapsedTime killTime) {};

    //Generates the next movement event for the receiver based on its current state and adds it to the event list.
    virtual void generateEvent (ElapsedTime eventTime,bool bNoLog);

    int operator == (const MobileElement& element) const { return id == element.id; }
    int operator < (const MobileElement& element) const { return id < element.id; }

	// when is follower can not be deleted.
	virtual bool CanBeDeleted(){ return true; }
};

//Unsorted container of MobileElements
class MobileElementList : public UnsortedContainer<MobileElement>
{
public:
    // default array size 8, subscript 0, delta (inc) 8
    MobileElementList () : UnsortedContainer<MobileElement> (8, 0, 8) {};
    virtual ~MobileElementList () {};

    int findElement (long p_id);
};

//Sorted container of MobileElements
class SortedMobileElementList : public SortedContainer<MobileElement>
{
public:
    // default array size 8, subscript 0, delta (inc) 8
    SortedMobileElementList () : SortedContainer<MobileElement> (8, 0, 8) {};
    virtual ~SortedMobileElementList () {};
 
   int findElement (long p_id);
};


#endif
