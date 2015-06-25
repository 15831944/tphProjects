/*****
*
*   Class       BagLogEntry
*   Author      Lincoln Fellingham
*   Date        January 1994
*   Purpose     Binary baggage logs, store information on each bag in
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

#ifndef BAGGAGE_LOG_ENTRY_H
#define BAGGAGE_LOG_ENTRY_H

#include "common\fsstream.h"
#include "results\logitem.h"
#include "results\eventlog.h"
#include "common\term_bin.h"
#include "common\template.h"
#include "common\elaptime.h"
#include "resultsdll.h"
#include <assert.h>

class RESULTS_TRANSFER BagLogEntry : public TerminalLogItem<BagDescStruct,BagEventStruct>
{
protected:

public:
    BagLogEntry () : TerminalLogItem<BagDescStruct,BagEventStruct> ()
    {
    }

    virtual ~BagLogEntry () {};

    virtual void echo (ofsstream& p_stream, const CString& _csProjPath) const;


    /*****
    *
    *   BagEntry specific accessors & modifiers
    *
    *****/

    long getID (void) const { return item.id; }
    void setID (long p_id) { item.id = p_id; }

    long getPax (void) const { return item.paxID; }
    void setPax (long p_id) { item.paxID = p_id; }

    int getProc (void) const { return item.procNumber; }
    void setProc (int p_id) { item.procNumber = (short)p_id; }

    int getFlight (void) const { return item.flight; }
    void setFlight (int p_num) { item.flight = (short)p_num; }

    ElapsedTime getEntryTime (void) const;
    ElapsedTime getExitTime (void) const;
    void setEntryTime (ElapsedTime p_time) { item.startTime = (long)p_time; }
    void setExitTime (ElapsedTime p_time) { item.endTime = (long)p_time; }

	ElapsedTime getLagTime(void) const {return item.lagTime;}
	void SetLagTime(ElapsedTime lagTime) {item.lagTime = lagTime;}


    int operator == (const BagLogEntry& p_entry) const
        { return item.startTime == p_entry.item.startTime; }
    int operator < (const BagLogEntry& p_entry) const
        { return item.startTime < p_entry.item.startTime; }
};

#endif
