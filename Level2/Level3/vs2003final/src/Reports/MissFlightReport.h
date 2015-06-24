// MissFlightReport.h: interface for the CMissFlightReport class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MISSFLIGHTREPORT_H__E654D924_9409_41CC_9B35_C741A9481499__INCLUDED_)
#define AFX_MISSFLIGHTREPORT_H__E654D924_9409_41CC_9B35_C741A9481499__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BaseReport.h"
#include "FromToProcCaculation.h"
#include "ReportParameter.h"

class CMissFlightReport : public CBaseReport  
{
public:
	CMissFlightReport(Terminal* _pTerm, const CString& _csProjPath);
	virtual ~CMissFlightReport();
	virtual int GetReportFileType (void) const { return MissFlightReportFile; }
    virtual int GetSpecFileType (void) const { return MissFlightReportFile; }
    virtual const char *GetTitle (void) const { return "Miss_Flight_Report_File"; }

    virtual void GenerateSummary( ArctermFile& p_file ) { return; }
	void GenerateDetailed( ArctermFile& p_file );

private:
	CString GetPaxLastProcIndex(int paxid);
	CString GetFlightID(int paxid);
	ElapsedTime GetPaxExitTime(int paxid);
	ElapsedTime GetDepartingTime(int paxid);
	void InitTools();

private:
	CFromToProcCaculation m_fromToProcsTools;
	CReportParameter::FROM_TO_PROCS m_fromToProcs;



};

#endif // !defined(AFX_MISSFLIGHTREPORT_H__E654D924_9409_41CC_9B35_C741A9481499__INCLUDED_)
