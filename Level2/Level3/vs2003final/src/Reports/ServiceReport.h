// ServiceReport.h: interface for the CServiceReport class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SERVICEREPORT_H__EFA3E0DC_8409_485C_9082_66F5DA2EAFD9__INCLUDED_)
#define AFX_SERVICEREPORT_H__EFA3E0DC_8409_485C_9082_66F5DA2EAFD9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "reportsdll.h"
#include "basereport.h"
#include "StatisticalTools.h"
class REPORTS_TRANSFER CServiceReport : public CBaseReport  
{
public:
	void getAverageServiceTime (const CMobileElemConstraint& aConst, CStatisticalTools< ElapsedTime >& _tools);
	// summary statistics
    ElapsedTime minimum;
    ElapsedTime average;
    ElapsedTime maximum;

    int count;
	
	virtual int GetReportFileType (void) const { return ServiceTimeReportFile; };
	virtual int GetSpecFileType (void) const { return ServiceTimeSpecFile; };
    virtual const char *GetTitle (void) const { return "Service_Report"; };
	CServiceReport(Terminal* _pTerm, const CString& _csProjPath);
	virtual ~CServiceReport();
	virtual void InitParameter( const CReportParameter* _pPara );

protected:
	void GenerateDetailed( ArctermFile& p_file );
	void GenerateSummary (ArctermFile& p_file);
};

#endif // !defined(AFX_SERVICEREPORT_H__EFA3E0DC_8409_485C_9082_66F5DA2EAFD9__INCLUDED_)
