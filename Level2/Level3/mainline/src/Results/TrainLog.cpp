// TrainLog.cpp: implementation of the CTrainLog class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TrainLog.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTrainLog::CTrainLog (EventLog<TrainEventStruct>* _pTrainEventLog) 
	: BaseLog<TrainDescStruct,TrainEventStruct,CTrainLogEntry>()
{
    eventLog = _pTrainEventLog;
}

CTrainLog::~CTrainLog()
{

}

void CTrainLog::echoLogFile (const char *p_filename, const CString& _csProjPath, OutputTerminal* _pOutTerm)
{
    ofsstream echoFile (p_filename);
    echoFile << p_filename << '\n';
    echoFile << "Log Echo File\n";
    echoFile << echoTitles() << '\n';
    CTrainLogEntry logEntry;
	logEntry.SetOutputTerminal( _pOutTerm );
	int nCount = getCount();
	for( int i=0; i<nCount; i++ )
    {
        getItem (logEntry, i);
        logEntry.echo (echoFile, _csProjPath);
    }
    echoFile.close();	
}

const char* CTrainLog::echoTitles (void) const
{
	return "ID,Start Time,End Time,Speed,Accelerate Speed,Decelerate Speed,Turn Around Time,Head Way Time"; 
}
