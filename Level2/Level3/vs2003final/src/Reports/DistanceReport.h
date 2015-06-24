// DistanceReport.h: interface for the CDistanceReport class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DISTANCEREPORT_H__45E20795_72BE_4FF7_B103_3219B7E24C5A__INCLUDED_)
#define AFX_DISTANCEREPORT_H__45E20795_72BE_4FF7_B103_3219B7E24C5A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BaseReport.h"
#include "reportsdll.h"
#include "StatisticalTools.h"
#include "ReportParameter.h"
#include "FromToProcCaculation.h"
class REPORTS_TRANSFER CDistanceReport : public CBaseReport  
{
	CReportParameter::FROM_TO_PROCS m_fromToProcs;
	CFromToProcCaculation m_fromToProcsTools;
public:
	CDistanceReport(Terminal* _pTerm, const CString& _csProjPath);
	virtual ~CDistanceReport();

    virtual int GetReportFileType (void) const { return DistanceReportFile; };

protected:
	void GetAverageDistance( const CMobileElemConstraint& _aConst ,CStatisticalTools<double>& _tools );
	long double m_ldMinimum;
	long double m_ldMaximum;
	long double m_ldAverage;
	int m_nCount;

    virtual int GetSpecFileType (void) const { return DistanceSpecFile; };
    virtual const char *GetTitle (void) const { return "Distance_Travelled_Report"; };
	virtual void GenerateSummary( ArctermFile& p_file );
	virtual void GenerateDetailed( ArctermFile& p_file );
	virtual void InitParameter( const CReportParameter* _pPara );

	void InitTools();
};

#endif // !defined(AFX_DISTANCEREPORT_H__45E20795_72BE_4FF7_B103_3219B7E24C5A__INCLUDED_)
