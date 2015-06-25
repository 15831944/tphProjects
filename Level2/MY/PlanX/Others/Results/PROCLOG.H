/*****
*
*   Class       ProcLog
*   Author      Lincoln Fellingham
*   Date        November 1993
*   Purpose     Binary processor logs, store information on each
*               processor in one file, and a complete listing of each
*               processor's activities in a second file that is indexed
*               by the first.
*
*               ProcLog defines an interface to the entire list of
*               processor log entries, reading, writing, accessing, and
*               sorting.
*
*   Revisions   Feb 15, 1996 - LF - Modified to subclass from template
*               based LogBase class
*
*****/

#ifndef PROCESSOR_DESCRIPTION_LOGS_H
#define PROCESSOR_DESCRIPTION_LOGS_H

#include "common\fsstream.h"
#include "common\term_bin.h"
#include "results\baselog.h"
#include "results\procntry.h"
//#include "engine\Terminal.h"

class ProcLog : public BaseLog<ProcDescStruct,ProcEventStruct,ProcLogEntry>
{
public:
    ProcLog (EventLog<ProcEventStruct>* _pProcEventLog) : BaseLog<ProcDescStruct,ProcEventStruct,ProcLogEntry>()
    {
        eventLog = _pProcEventLog;
    }

	virtual void echoLogFile (const char *p_filename, const CString& _csProjPath, OutputTerminal* _pOutTerm);

    virtual const char *echoTitles (void) const
        { return "Name,Index,Proc Type,Queue,Event Count"; }
	
	bool getDisallowGroupFlag(int nIndex, Terminal *m_pTerm);
};

#endif
