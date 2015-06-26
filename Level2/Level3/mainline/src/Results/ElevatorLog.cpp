// ElevatorLog.cpp: implementation of the CElevatorLog class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "ElevatorLog.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CElevatorLog::CElevatorLog(EventLog<ElevatorEventStruct>* _pElevatorEventLog)
: BaseLog<ElevatorDescStruct,ElevatorEventStruct,CElevatorLogEntry>()
{
	eventLog = _pElevatorEventLog;
}

CElevatorLog::~CElevatorLog()
{

}
void CElevatorLog::echoLogFile (const char *p_filename, const CString& _csProjPath, OutputTerminal* _pOutTerm)
{
    ofsstream echoFile (p_filename);
    echoFile << p_filename << '\n';
    echoFile << "Log Echo File\n";
    echoFile << echoTitles() << '\n';
    CElevatorLogEntry logEntry;
	logEntry.SetOutputTerminal( _pOutTerm );
	int nCount = getCount();
	for( int i=0; i<nCount; i++ )
    {
        getItem (logEntry, i);
        logEntry.echo (echoFile, _csProjPath);
    }
    echoFile.close();	
}

const char *CElevatorLog::echoTitles (void) const
{
return "ID,Start Time,End Time "; 
}