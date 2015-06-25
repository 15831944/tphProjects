/*****
*
*   Class       ProcLogEntry
*   Author      Lincoln Fellingham
*   Date        January 1994
*   Purpose     Binary passenger logs, store information on each pax in
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

#ifndef PROCESSOR_LOG_ENTRY_H
#define PROCESSOR_LOG_ENTRY_H

#include <assert.h>
#include "../common/fsstream.h"
#include "logitem.h"
#include "eventlog.h"
#include "../common/term_bin.h"
#include "../common/template.h"
#include "../common/elaptime.h"
#include "resultsdll.h"


class RESULTS_TRANSFER ProcLogEntry : public TerminalLogItem<ProcDescStruct,ProcEventStruct>
{
protected:

public:
    ProcLogEntry () : TerminalLogItem<ProcDescStruct,ProcEventStruct> ()
    {
    }

    virtual ~ProcLogEntry () {};

    // accessors and modifiers
    virtual void echo (ofsstream& p_stream, const CString& _csProjPath) const;


    /*****
    *
    *   ProcEntry specific accessors & modifiers
    *
    *****/

    long getIndex (void) const { return item.index; }
    void setIndex (long p_ndx) { item.index = (short) p_ndx; }

    void getIDs (int *p_ids) const;
    void getIDs (short *p_ids) const;
    void getID (char *p_str) const;
    void setIDs (const int *p_ids);

    int getProcType (void) const { return item.procType; }
    void setProcType (int p_type) { item.procType = (char)p_type; }

    int getQueue (void) const { return item.hasQueue; }
    void setQueue (int p_queue) { item.hasQueue = (char)p_queue; }
	
	bool GetDisallowGroupFlag() const { return item.disallowGroup ? true : false; }
	void SetDisallowGroupFlag( bool _bDisallowGroup ){ item.disallowGroup = _bDisallowGroup ? 1 : 0; }
	void sortProcEventLog( void );
};

bool sortByEventTime( const ProcEventStruct& e1, const ProcEventStruct& e2 );
#endif
