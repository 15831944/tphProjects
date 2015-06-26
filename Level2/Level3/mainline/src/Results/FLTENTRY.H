/*****
*
*   Class       FlightLogEntry
*   Author      Lincoln Fellingham
*   Date        January 1994
*   Purpose     Binary flight logs, store information on each flight in
*               in one file, and a complete listing of each flight's
*               activities in a second file that is indexed by the first
*
*               Note that the EventLog must be defined here and not in
*               the base class despite the identical functionality
*               because the class is static, and the file is not common
*               to all LogEntry classes
*
*   Revisions   Feb 15, 1996 - LF - subclass from LogItem class, used to
*               standardize interface logs and event lists
*
*****/

#ifndef FLIGHT_LOG_ENTRY_H
#define FLIGHT_LOG_ENTRY_H

#include <assert.h>
#include "common\fsstream.h"
#include "results\logitem.h"
#include "results\eventlog.h"
#include "common\term_bin.h"
#include "common\elaptime.h"
#include "common\template.h"

class FlightLogEntry : public TerminalLogItem<FlightDescStruct,FlightEventStruct>
{
protected:

public:

    FlightLogEntry () : TerminalLogItem<FlightDescStruct,FlightEventStruct> ()
    {
    }

    virtual ~FlightLogEntry () {};

    // accessors and modifiers
    virtual void echo (ofsstream& p_stream, const CString& _csProjPath) const;


    /*****
    *
    *   FlightEntry specific accessors and modifiers
    *
    *****/

    void getFlightID (char *p_str, char p_mode = 0) const;
    int getGate (void) const { return item.gate; }

	int getArrGate(void) const {return item.nArrGate;}
	int getDepGate(void) const {return item.nDepGate;}

    ElapsedTime getGateTime (void) const;

    ElapsedTime getArrTime (void) const;
    ElapsedTime getDepTime (void) const;

    int isArriving (void) const { return item.arrID.HasValue(); }
    int isDeparting (void) const { return item.depID.HasValue(); }
};

#endif
