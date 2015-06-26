// ResourceElementLog.h: interface for the ResourceElementLog class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RESOURCEELEMENTLOG_H__83C0CA19_8E84_482B_A738_28632206AE75__INCLUDED_)
#define AFX_RESOURCEELEMENTLOG_H__83C0CA19_8E84_482B_A738_28632206AE75__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// include
#include "common\template.h"
#include "common\term_bin.h"
#include "results\baselog.h"
#include "resourceelementlogentry.h"

class RESULTS_TRANSFER ResourceElementLog : public BaseLog<ResDescStruct,ResEventStruct,ResourceElementLogEntry>
{
public:
	ResourceElementLog( EventLog<ResEventStruct>* _pResEventLog );
	virtual ~ResourceElementLog();

	virtual void echoLogFile (const char *p_filename, const CString& _csProjPath, OutputTerminal* _pOutTerm);	
    virtual const char *echoTitles (void) const;
};

#endif // !defined(AFX_RESOURCEELEMENTLOG_H__83C0CA19_8E84_482B_A738_28632206AE75__INCLUDED_)
