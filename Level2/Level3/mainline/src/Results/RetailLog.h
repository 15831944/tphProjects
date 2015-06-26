#pragma once
#include "common\term_bin.h"
#include "results\baselog.h"
#include "RetailLogEntry.h"

class CRetailLog : public BaseLog<RetailDescStruct,RetailEventStruct,CRetailLogEntry>
{
public:
	CRetailLog(EventLog<RetailEventStruct>* pRetailEventLog);
	~CRetailLog(void);

	virtual void echoLogFile (const char *p_filename, const CString& _csProjPath, OutputTerminal* _pOutTerm);

	virtual const char *echoTitles (void) const;
};
