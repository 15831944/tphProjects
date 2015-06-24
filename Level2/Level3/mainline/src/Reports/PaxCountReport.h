// PaxCountReport.h: interface for the CPaxCountReport class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PAXCOUNTREPORT_H__9DCF9302_5080_4D0D_A7FA_F6221D17F31E__INCLUDED_)
#define AFX_PAXCOUNTREPORT_H__9DCF9302_5080_4D0D_A7FA_F6221D17F31E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BaseReport.h"
#include "reportsdll.h"
#include "engine\terminalevent.h"
#include "..\common\progressbar.h"

enum PaxCountEventTypes
{
    IncPaxCount = MobileMovement + 1,
    DecPaxCount
};



class CPaxCountReport;
class CARCportEngine;
class REPORTS_TRANSFER PaxCount : public TerminalEvent
{
protected:
    static CPaxCountReport* m_pReport;
    int incType;
    ElapsedTime exitTime;

public:
    static void setReport( CPaxCountReport* _pRep ) { m_pReport = _pRep; };

    void init (int type, const ElapsedTime& entryTime, const ElapsedTime& exit)
        { incType = type; time = entryTime; exitTime = exit; };

    // shut down event
    virtual int kill (void) { return 0; }

    virtual int process (CARCportEngine *);
//	{
//		report->updateCount (incType, time);
//		if (incType == IncPaxCount)
//		{
//			time = exitTime;
//			incType = DecPaxCount;
//			addEvent();
//		}
//		return 0;
//	}
    virtual const char *getTypeName (void) const { return (incType == IncPaxCount)?
        "Increment Passenger Count": "Decrement Passenger Count"; };
    int getEventType (void) const { return incType; };
};

class REPORTS_TRANSFER CPaxCountReport : public CBaseReport  
{
public:
	void updateCount (int incType, ElapsedTime time);
	void initIntervalStats();
	CPaxCountReport(Terminal* _pTerm, const CString& _csProjPath);
	virtual ~CPaxCountReport();
	virtual int GetReportFileType (void) const { return PassengerCountReportFile; };
	virtual void InitParameter( const CReportParameter* _pPara );
protected:
	void writeSummary (const CMobileElemConstraint& paxType,ArctermFile& p_file);
	void writeEntries (const CMobileElemConstraint& paxType,ArctermFile& p_file);
	void clear ();
	void calculatePassengerCount (const CMobileElemConstraint& paxType);
	void GenerateSummary( ArctermFile& p_file );
	void GenerateDetailed( ArctermFile& p_file );
	virtual int GetSpecFileType (void) const { return PassengerCountSpecFile; };
	virtual const char *GetTitle (void) const { return "Passenger_Count_Report"; };
	int m_nCount;
	PaxCount * m_eventList; 
	int intervalCount;
	long maxCount;
    long minCount;
    long totalCount;
    double averageCount;
    ElapsedTime timeOfMaxCount;
    ElapsedTime timeOfMinCount;
    ElapsedTime firstEntry;
    ElapsedTime lastExit;
	long paxCount;
	ElapsedTime curTime;
    ElapsedTime previousTime;
    int timeIndex;
	long * passengerCountList;

	CProgressBar* m_pProgressBar;
	int m_nPaxOfType;

};


#endif // !defined(AFX_PAXCOUNTREPORT_H__9DCF9302_5080_4D0D_A7FA_F6221D17F31E__INCLUDED_)
