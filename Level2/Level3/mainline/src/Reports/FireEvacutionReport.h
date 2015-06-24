// FireEvacutionReport.h: interface for the CFireEvacutionReport class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FIREEVACUTIONREPORT_H__04E6832E_EB02_4AFE_8637_5EE158AC6115__INCLUDED_)
#define AFX_FIREEVACUTIONREPORT_H__04E6832E_EB02_4AFE_8637_5EE158AC6115__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BaseReport.h"
#include "reportsdll.h"
#include "StatisticalTools.h"

class CFireEvacutionReport : public CBaseReport  
{
public:
	CFireEvacutionReport( Terminal* _pTerm, const CString& _strProjPath );
	virtual ~CFireEvacutionReport();
	
protected:
	long double m_ldMinimum;
	long double m_ldMaximum;
	long double m_ldAverage;
	int			m_nCount;

protected:
	// generate detail report
	virtual void GenerateDetailed( ArctermFile& p_file );
	
	// generate summary report
	virtual void GenerateSummary( ArctermFile& p_file );

	bool GetAverageValue( const CMobileElemConstraint& _aConst ,CStatisticalTools<double>& _tools );
	
	virtual int GetReportFileType (void) const { return FireEvacuationReportFile; }
    virtual int GetSpecFileType (void) const { return FireEvacuationSpecFile; }
    virtual const char *GetTitle (void) const { return "Fire_Evacuation_Report_File"; }
};

#endif // !defined(AFX_FIREEVACUTIONREPORT_H__04E6832E_EB02_4AFE_8637_5EE158AC6115__INCLUDED_)
