// PrintLog.h: interface for the CPrintLog class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PRINTLOG_H__62FB9821_D5C9_4866_B5D0_34312445DCBD__INCLUDED_)
#define AFX_PRINTLOG_H__62FB9821_D5C9_4866_B5D0_34312445DCBD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "reportsdll.h"
#include "basereport.h"
class REPORTS_TRANSFER CPrintLog : public CBaseReport
{
public:
	CPrintLog(Terminal* _pTerm, const CString& _csProjPath);
	virtual ~CPrintLog();

    virtual int GetReportFileType (void) const { return PaxLogReportFile; };
protected:
	virtual void GenerateDetailed( ArctermFile& p_file );
	virtual void GenerateSummary( ArctermFile& p_file );

    virtual int GetSpecFileType (void) const { return PaxLogSpecFile; };
    virtual const char *GetTitle (void) const { return "Passenger_Log_Report"; };

};

#endif // !defined(AFX_PRINTLOG_H__62FB9821_D5C9_4866_B5D0_34312445DCBD__INCLUDED_)
