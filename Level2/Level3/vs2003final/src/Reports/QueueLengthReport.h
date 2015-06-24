// QueueLengthReport.h: interface for the CQueueLengthReport class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_QUEUELENGTHREPORT_H__695197AA_CCB5_4799_BACF_49BED2E38C2B__INCLUDED_)
#define AFX_QUEUELENGTHREPORT_H__695197AA_CCB5_4799_BACF_49BED2E38C2B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "BaseReport.h"
#include "IntervalStat.h"

class CQueueLengthReport : public CBaseReport  
{
public:
	CQueueLengthReport(Terminal* _pTerm, const CString& _csProjPath);
	virtual ~CQueueLengthReport();

	void initIntervalStats();

    virtual int GetReportFileType (void) const { return QueueLengthReportFile; };
	virtual void InitParameter( const CReportParameter* _pPara );
    
protected:	
	int calculateValidProcLog();
    int* m_pQueueLengthList;
    int m_nIntervalCount;

    CIntervalStat* m_pIntervalStats;

    void calculateQueueLengths (ProcLogEntry& logEntry);
    void writeEntries (const char *p_id, ArctermFile& p_file) const;

    void getAverageQueueLength (const ProcessorID *id);
    void updateIntervalStats (void);
    void writeAverages (const ProcessorID *id, ArctermFile& p_file) const;

	virtual int GetSpecFileType (void) const { return QueueLengthSpecFile; };
    virtual const char *GetTitle (void) const { return "Queue Length History Report"; };

	virtual void GenerateSummary( ArctermFile& p_file );
	virtual void GenerateDetailed( ArctermFile& p_file );

};

#endif // !defined(AFX_QUEUELENGTHREPORT_H__695197AA_CCB5_4799_BACF_49BED2E38C2B__INCLUDED_)
