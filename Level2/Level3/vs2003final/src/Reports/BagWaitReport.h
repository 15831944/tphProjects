// BagWaitReport.h: interface for the CBagWaitReport class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BAGWAITREPORT_H__2EEADEA9_21E2_4092_8B36_4673E8A4A853__INCLUDED_)
#define AFX_BAGWAITREPORT_H__2EEADEA9_21E2_4092_8B36_4673E8A4A853__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BaseReport.h"
#include "StatisticalTools.h"
class CBagWaitReport : public CBaseReport  
{
public:
	CBagWaitReport(Terminal* _pTerm, const CString& _csProjPath);
	virtual ~CBagWaitReport();

    virtual int GetReportFileType (void) const { return BagWaitTimeReportFile; };
	virtual void InitParameter( const CReportParameter* _pPara );

protected:
    ElapsedTime minimum;
    ElapsedTime average;
    ElapsedTime maximum;

    int count;


    virtual int GetSpecFileType (void) const { return BagWaitTimeSpecFile; };
    virtual const char *GetTitle (void) const { return "Baggage_Wait_Time_Report"; };

	virtual void GenerateSummary( ArctermFile& p_file );
	virtual void GenerateDetailed( ArctermFile& p_file );

    void getAverageBagWaitTime (const CMobileElemConstraint& aConst, CStatisticalTools< ElapsedTime >& _tools);
};

#endif // !defined(AFX_BAGWAITREPORT_H__2EEADEA9_21E2_4092_8B36_4673E8A4A853__INCLUDED_)
