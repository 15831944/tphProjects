#include "StdAfx.h"
#include ".\retaillog.h"

CRetailLog::CRetailLog(EventLog<RetailEventStruct>* pRetailEventLog)
{
	eventLog = pRetailEventLog;
}

CRetailLog::~CRetailLog(void)
{
}

void CRetailLog::echoLogFile( const char *p_filename, const CString& _csProjPath, OutputTerminal* _pOutTerm )
{

}

const char * CRetailLog::echoTitles( void ) const
{
	return _T("");
}