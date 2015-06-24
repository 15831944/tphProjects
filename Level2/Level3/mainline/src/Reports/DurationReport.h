// DurationReport.h: interface for the CDurationReport class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DURATIONREPORT_H__C2285FE0_ED6E_43EB_B6B7_4245A94B58FB__INCLUDED_)
#define AFX_DURATIONREPORT_H__C2285FE0_ED6E_43EB_B6B7_4245A94B58FB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "reportsdll.h"
#include "basereport.h"
#include "StatisticalTools.h"
#include "ReportParameter.h"
#include "FromToProcCaculation.h"
#include "FromToProcCaculationWithMode.h"
class ReportPaxEntry;
class REPORTS_TRANSFER CDurationReport : public CBaseReport  
{
	CReportParameter::FROM_TO_PROCS m_fromToProcs;
	CFromToProcCaculationWithMode m_fromToProcsTools;
public:
	void getAverageDuration (const CMobileElemConstraint& aConst, CStatisticalTools< ElapsedTime >& _tools);
	virtual int GetReportFileType (void) const { return DurationReportFile; };
	virtual int GetSpecFileType (void) const { return DurationSpecFile; };
    virtual const char *GetTitle (void) const { return "Duration_Report"; };
	CDurationReport(Terminal* _pTerm, const CString& _csProjPath);
	virtual ~CDurationReport();
	
	// summary statistics
    ElapsedTime minimum;
    ElapsedTime average;
    ElapsedTime maximum;

	int count;

	virtual void InitParameter( const CReportParameter* _pPara );
protected:
	void GenerateSummary (ArctermFile& p_file);
	void GenerateDetailed( ArctermFile& p_file );
	void InitTools();
	void CaculateEveryPax(ReportPaxEntry& _events );
};

#endif // !defined(AFX_DURATIONREPORT_H__C2285FE0_ED6E_43EB_B6B7_4245A94B58FB__INCLUDED_)
