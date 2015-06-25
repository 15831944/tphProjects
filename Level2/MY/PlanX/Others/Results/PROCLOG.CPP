#include "stdafx.h"
#include "results\proclog.h"
#include "engine\Terminal.h"

void ProcLog::echoLogFile (const char *p_filename, const CString& _csProjPath, OutputTerminal* _pOutTerm)
{
    ofsstream echoFile (p_filename);
    echoFile << p_filename << '\n';
    echoFile << "Log Echo File\n";
    echoFile << echoTitles() << '\n';
    ProcLogEntry logEntry;
	logEntry.SetOutputTerminal( _pOutTerm );
	int nCount = getCount();
	for( int i=0; i<nCount; i++ )
    {
        getItem (logEntry, i);
        logEntry.echo (echoFile, _csProjPath);
    }
    echoFile.close();
}

bool ProcLog::getDisallowGroupFlag(int nIndex, Terminal* pTerm)
{
	ProcLogEntry logEntry;
	logEntry.SetOutputTerminal(pTerm);
	logEntry.SetEventLog(pTerm->m_pProcEventLog);
	getItem(logEntry, nIndex);
	return logEntry.GetDisallowGroupFlag();
}