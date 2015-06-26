// ElevatorLog.h: interface for the CElevatorLog class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ELEVATORLOG_H__27654C8C_8371_48BB_B7DB_F5AE06B7E712__INCLUDED_)
#define AFX_ELEVATORLOG_H__27654C8C_8371_48BB_B7DB_F5AE06B7E712__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "common\term_bin.h"
#include "results\baselog.h"
#include "ElevatorLogEntry.h"
class CElevatorLog  : public BaseLog<ElevatorDescStruct,ElevatorEventStruct,CElevatorLogEntry>
{
public:
	CElevatorLog(EventLog<ElevatorEventStruct>* _pElevatorEventLog);
	virtual ~CElevatorLog();
	
	virtual void echoLogFile (const char *p_filename, const CString& _csProjPath, OutputTerminal* _pOutTerm);

    virtual const char *echoTitles (void) const;

};

#endif // !defined(AFX_ELEVATORLOG_H__27654C8C_8371_48BB_B7DB_F5AE06B7E712__INCLUDED_)
