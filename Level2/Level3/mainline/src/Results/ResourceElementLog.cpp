// ResourceElementLog.cpp: implementation of the ResourceElementLog class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "ResourceElementLog.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ResourceElementLog::ResourceElementLog( EventLog<ResEventStruct>* _pResEventLog )
				   : BaseLog<ResDescStruct,ResEventStruct,ResourceElementLogEntry>()
		
{
	eventLog = _pResEventLog;
}

ResourceElementLog::~ResourceElementLog()
{

}

void ResourceElementLog::echoLogFile (const char *p_filename, const CString& _csProjPath, OutputTerminal* _pOutTerm)
{
	ofsstream echoFile (p_filename);
    echoFile << p_filename << '\n';
    echoFile << "Log Echo File\n";
    echoFile << echoTitles() << '\n';
    ResourceElementLogEntry logEntry;
	logEntry.SetOutputTerminal( _pOutTerm );

	int nCount = getCount();
	for( int i=0; i<nCount; i++ )
    {
        getItem (logEntry, i);
        logEntry.echo (echoFile, _csProjPath);
    }

    echoFile.close();	
}
	
const char* ResourceElementLog::echoTitles (void) const
{
	return "id,name, start time,end time "; 
}
