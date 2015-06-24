// AverageQueueLengthReport.h: interface for the CAverageQueueLengthReport class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AVERAGEQUEUELENGTHREPORT_H__24472D61_F320_4173_827A_EC45C6A56F48__INCLUDED_)
#define AFX_AVERAGEQUEUELENGTHREPORT_H__24472D61_F320_4173_827A_EC45C6A56F48__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "BaseReport.h"
class CAverageQueueLengthReport : public CBaseReport  
{
public:
	CAverageQueueLengthReport(Terminal* _pTerm, const CString& _csProjPath);
	virtual ~CAverageQueueLengthReport();

    virtual int GetReportFileType (void) const { return AverageQueueLengthReportFile; };

protected:
   // float m_fAverageQueueLength;
    int m_nMaxQueueLength;
    ElapsedTime m_eMaxQueueTime;
    int m_nCount;


    virtual int GetSpecFileType (void) const { return AverageQueueLengthSpecFile; };
    virtual const char *GetTitle (void) const { return "Critical Queue Parameters Report"; };

	virtual void GenerateSummary( ArctermFile& p_file );
	virtual void GenerateDetailed( ArctermFile& p_file );

	void clear () ;

    void calculateAverageQueueLength (ProcLogEntry& logEntry);
    void writeEntry (const char *p_id, ArctermFile& p_file) const;

    void getAverageQueueLength (const ProcessorID *id);
    void writeAverage (const ProcessorID *id, ArctermFile& p_file) const;


};

#endif // !defined(AFX_AVERAGEQUEUELENGTHREPORT_H__24472D61_F320_4173_827A_EC45C6A56F48__INCLUDED_)
