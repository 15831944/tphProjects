/*****
*
*   Class       FlightLog
*   Author      Lincoln Fellingham
*   Date        November 1993
*   Purpose     Binary flight logs, store information on each flight in
*               one file, and a complete listing of each flight's
*               activities in a second file that is indexed by the first.
*
*               FlightLog defines an interface to the entire list of
*               flight log entries, reading, writing, accessing, and
*               sorting.
*
*   Revisions   Feb 15, 1996 - LF - Modified to subclass from template
*               based LogBase class
*
*****/

#ifndef FLIGHT_DESCRIPTION_LOGS_H
#define FLIGHT_DESCRIPTION_LOGS_H

#include "common\fsstream.h"
#include "common\term_bin.h"
#include "results\baselog.h"
#include "results\fltentry.h"
#include "inputs\fltlist.h"

class FlightLog : public BaseLog<FlightDescStruct,FlightEventStruct,FlightLogEntry>,
    public ListOfFlights
{
public:
    FlightLog (EventLog<FlightEventStruct>* _pFltEventLog) : BaseLog<FlightDescStruct,FlightEventStruct,FlightLogEntry>()
    {
        eventLog = _pFltEventLog;
    }

    virtual void echoLogFile (const char *p_filename, const CString& _csProjPath, OutputTerminal* _pOutTerm);

    virtual const char *echoTitles (void) const
        { return "Carrier,Arr ID,Origin,Arr Time,Arr Load,Dep ID,"
            "Destination,Dep Time,Dep Load,AC Type"; }
    void getFlightID (char *p_str, int p_ndx, char p_mode = '\0');

    // methods used to fill out FlightSelectionTool lists
    void getCarrierList (StringList& strDict,
        const FlightConstraint &p_type);
    void getFlightList (StringList& p_strDict,
        const FlightConstraint &p_type);
    void getAirportList (StringList& p_strDict,
        const FlightConstraint &p_type);
    void getACTypeList (StringList& p_strDict,
        const FlightConstraint &p_type);
//&    void getSectors (StringList& p_strDict,
//&        const FlightConstraint &p_type);
//&    void getCategories (StringList& p_strDict,
//&        const FlightConstraint &p_type);
};

#endif
