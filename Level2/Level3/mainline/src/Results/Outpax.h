#ifndef OUTPUT_ELEMENT_H
#define OUTPUT_ELEMENT_H


#include "results\paxentry.h"
#include "inputs\pax_cnst.h"
#include "inputs\MultiMobConstraint.h"


class OutputPaxEntry : public MobLogEntry
{
public:
    virtual ~OutputPaxEntry () {};

    /*****
    *
    *   General query functions
    *
    *****/

    // returns time Mobile Element spent in terminal
    ElapsedTime duration (void)
        { return getExitTime() - getEntryTime(); }

    // Constraint match methods
    int fits (const CMultiMobConstraint& p_multiConst) const;
    int fits (const CMobileElemConstraint& p_const) const;


	void printMobile(char* p_str) const;
    void printType (char *p_str) const;
    void printFullType (char *p_str) const;
};

class RESULTS_TRANSFER PaxEvent
{
protected:
    MobEventStruct event;

public:
    PaxEvent () { clear(); }

    void init (const MobEventStruct& p_event) { event = p_event; }
    void clear (void) { event.time = 0l; event.state = 0;
         event.procNumber = 0; }
    void setTime (ElapsedTime p_time) { event.time = (long)p_time; }

    Point getPoint (void) const;
    ElapsedTime getTime (void) const;
    int getState (void) const { return event.state; }
    int getProc (void) const { return event.procNumber; }
	bool getDynamicCreateProcFlag()const { return event.bDynamicCreatedProc;	}
	int getReason()const { return event.reason;	}
	
};

#endif
