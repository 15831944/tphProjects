// BagCountReport.h: interface for the CBagCountReport class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BAGCOUNTREPORT_H__0285B3AF_6BA5_4AE7_AA78_37FA40633574__INCLUDED_)
#define AFX_BAGCOUNTREPORT_H__0285B3AF_6BA5_4AE7_AA78_37FA40633574__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BaseReport.h"
#include "IntervalStat.h"

class CBagCountReport : public CBaseReport  
{
public:
	CBagCountReport(Terminal* _pTerm, const CString& _csProjPath);
	virtual ~CBagCountReport();
    virtual int GetReportFileType (void) const { return BagCountReportFile; };

	void initIntervalStats (void);
	virtual void InitParameter( const CReportParameter* _pPara );

protected:
    int* m_pBagCountList;
    int m_nIntervalCount;
    CIntervalStat* m_pIntervalStats;
    
	virtual int GetSpecFileType (void) const { return BagCountSpecFile; };
    virtual const char *GetTitle (void) const { return "Bag Count Report"; };

	virtual void GenerateSummary( ArctermFile& p_file );
	virtual void GenerateDetailed( ArctermFile& p_file );
	

    void getAverageBagCount (const ProcessorID *id);
    void writeAverages (const ProcessorID *id, ArctermFile& p_file) const;

    void calculateBagCounts (ProcLogEntry& logEntry);
    void updateIntervalStats (void);

    void writeEntries (const char *id, ArctermFile& p_file) const;

};

#endif // !defined(AFX_BAGCOUNTREPORT_H__0285B3AF_6BA5_4AE7_AA78_37FA40633574__INCLUDED_)
