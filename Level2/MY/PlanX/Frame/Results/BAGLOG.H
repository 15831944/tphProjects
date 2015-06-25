/*****
*
*   Class       BagLog
*   Author      Lincoln Fellingham
*   Date        November 1993
*   Purpose     Binary bag logs, store information on each bag in
*               one file, and a complete listing of each bag's
*               activities in a second file that is indexed by the first.
*
*               BagLog defines an interface to the entire list of
*               bag log entries, reading, writing, accessing, and
*               sorting.
*
*   Revisions   Feb 15, 1996 - LF - Modified to subclass from template
*               based LogBase class
*
*****/

#ifndef BAGGAGE_DESCRIPTION_LOGS_H
#define BAGGAGE_DESCRIPTION_LOGS_H

#include "common\fsstream.h"
#include "common\term_bin.h"
#include "results\baselog.h"
#include "results\bagentry.h"

class RESULTS_TRANSFER BagLog : public BaseLog<BagDescStruct,BagEventStruct,BagLogEntry>
{
public:
    BagLog( EventLog<BagEventStruct>* _pBagEventLog ) : BaseLog<BagDescStruct,BagEventStruct,BagLogEntry>()
    {
        eventLog = _pBagEventLog;
    }

	virtual void echoLogFile (const char *p_filename, const CString& _csProjPath, OutputTerminal* _pOutTerm);

    virtual const char *echoTitles (void) const
        { return "ID,Passenger,Flight,Entry Time,Exit Time,Bag Device"; }
    void sortBags (void);
};

#endif
