#ifndef REPORT_ELEMENT_H
#define REPORT_ELEMENT_H

#include "results\outpax.h"
#include "inputs\pax_cnst.h"
#include "common\termfile.h"
#include "reports\printlog.h"
#include "BaseReport.h"


class REPORTS_TRANSFER ReportPaxEntry : public OutputPaxEntry
{
protected:
//&    static CPrintLog *report;
	static CBaseReport *report;

public:
	int usesSelectedProc (ElapsedTime p_start, ElapsedTime p_end);
	static void initReport (CBaseReport *p_rep) { report = p_rep; };
//&    static void initReport( CPrintLog* p_rep ) { report = p_rep; };

    virtual ~ReportPaxEntry () {};
	ReportPaxEntry();

    void writeElement (ArctermFile& p_file);
	
	// indicates whether passed proc index is a selected one
    int isValidProc (int p_ndx) const { return (report)?
        report->ProcFitsSpecs (p_ndx): TRUE; }
/*
    // indicates whether passed proc index is a selected one
    int isValidProc (int p_ndx) const { return (report)?
        report->procFitsSpecs (p_ndx): TRUE; }

    // indicates whether the pax uses a selected processor in the passed
    // time range. Requires that the pax's events are buffered
    int usesSelectedProc (ElapsedTime p_start, ElapsedTime p_end);
*/
    // get the coordinate at time
	// return true if succeed
    bool getPointAtTime (ElapsedTime p_time, Point& p_coord );

	ElapsedTime getEntryTime();
	ElapsedTime getExitTime();

	int alive (ElapsedTime t1, ElapsedTime t2)
	{ return (getEntryTime() < t2 && t1 < getExitTime()); }
	int alive (ElapsedTime p_time)
	{ return (getEntryTime() < p_time && p_time < getExitTime()); }

	virtual MobEventStruct getEvent(long p_ndx);
	virtual MobEventStruct getNextEvent(void);
	long getCount (void);
	virtual void loadEvents (void);

	virtual void setToFirst(void){m_curPos = 0l;}

protected:
	void loadifNecessary();

private:
	long m_curPos;
};

class ReportPaxEvent : public PaxEvent
{
public:
    virtual ~ReportPaxEvent () {};
    void writeTrack (ArctermFile& p_file) const;
};

#endif
