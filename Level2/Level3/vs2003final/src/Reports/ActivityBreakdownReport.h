// ActivityBreakdownReport.h: interface for the CActivityBreakdownReport class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ACTIVITYBREAKDOWNREPORT_H__A0605C60_A945_4D5D_91A6_9BB315E3A789__INCLUDED_)
#define AFX_ACTIVITYBREAKDOWNREPORT_H__A0605C60_A945_4D5D_91A6_9BB315E3A789__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BaseReport.h"
class CActivityBreakdownReport : public CBaseReport  
{
public:
	CActivityBreakdownReport(Terminal* _pTerm, const CString& _csProjPath);
	virtual ~CActivityBreakdownReport();

    virtual int GetReportFileType (void) const { return ActivityBreakdownReportFile; };
protected:
    ElapsedTime avgFreeMovingTime,
                avgQueueWaitTime,
                avgBagWaitTime,
                avgHoldAreaWaitTime,
                avgServiceTime;

    int count;

    virtual int GetSpecFileType (void) const { return ActivityBreakdownSpecFile; };
    virtual const char *GetTitle (void) const { return "Activity_Breakdown_Report"; };

	virtual void GenerateSummary( ArctermFile& p_file );
	virtual void GenerateDetailed( ArctermFile& p_file );
	virtual void InitParameter( const CReportParameter* _pPara );

    void getAverageActivityTimes (const CMobileElemConstraint& aConst);
};

#endif // !defined(AFX_ACTIVITYBREAKDOWNREPORT_H__A0605C60_A945_4D5D_91A6_9BB315E3A789__INCLUDED_)
