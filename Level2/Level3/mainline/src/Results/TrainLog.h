// TrainLog.h: interface for the CTrainLog class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TRAINLOG_H__D9AAAE50_7990_4089_83EF_A82F3D20E7E6__INCLUDED_)
#define AFX_TRAINLOG_H__D9AAAE50_7990_4089_83EF_A82F3D20E7E6__INCLUDED_


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "common\term_bin.h"
#include "results\baselog.h"
#include "results\TrainLogEntry.h"

class CTrainLog : public BaseLog<TrainDescStruct,TrainEventStruct,CTrainLogEntry>
{
public:
    CTrainLog (EventLog<TrainEventStruct>* _pTrainEventLog);
	virtual ~CTrainLog();

	virtual void echoLogFile (const char *p_filename, const CString& _csProjPath, OutputTerminal* _pOutTerm);

    virtual const char *echoTitles (void) const;
};

#endif // !defined(AFX_TRAINLOG_H__D9AAAE50_7990_4089_83EF_A82F3D20E7E6__INCLUDED_)
