/*****
*
*   Class       PaxLog
*   Author      Lincoln Fellingham
*   Date        November 1993
*   Purpose     Binary passenger logs, store information on each pax in
*               one file, and a complete listing of each bag's
*               activities in a second file that is indexed by the first.
*
*               PaxLog defines an interface to the entire list of
*               bag log entries, reading, writing, accessing, and
*               sorting.
*
*   Revisions   Feb 15, 1996 - LF - Modified to subclass from template
*               based LogBase class
*
*****/

#ifndef PASSENGER_DESCRIPTION_LOGS_H
#define PASSENGER_DESCRIPTION_LOGS_H

#include "common\template.h"
#include "common\fsstream.h"
#include "common\term_bin.h"
#include "results\baselog.h"
#include "results\paxentry.h"
#include <vector>
#include <map>

class TempMobileEventLog;

class RESULTS_TRANSFER PaxLog : public BaseLog<MobDescStruct,MobEventStruct,MobLogEntry>
{
	typedef struct 
	{
		long    lID;                 // Mobile Element identification
		long	lOwnerId;			// owner's id
		int    iGroupMember;        // element's group member number (0+)
		int		iIndexInLog;		//index in pax log
		int		iGroupSize;			// pax group size
	}PAXINDEXTABLE;

	//all necessary info for find pax's attachment( such as visito, baggage,etc)
	std::vector<PAXINDEXTABLE>m_vPaxIndexTable;

	//new index in pax log map to old index ( before sort passagen when complete generate pas) in pax log
	std::map<long, long >m_mapNewIdxToOldIdx;
public:

    PaxLog (EventLog<MobEventStruct>* _pPaxEventLog, TempMobileEventLog* pTempLog = NULL);
	virtual ~PaxLog() { m_vPaxIndexTable.clear();m_mapNewIdxToOldIdx.clear();	}


	void sortPassengers(CProgressBar& bar);

    virtual void echoLogFile (const char *p_filename, const CString& _csProjPath, OutputTerminal* _pOutTerm);

    virtual const char *echoTitles (void) const
        { return "ID,Entry Time,Exit Time,Arr Flight,Dep Flight,Type,Event Count"; }

    long getRangeCount (ElapsedTime p_start, ElapsedTime p_end);
    void sortPassengers (void);
    ElapsedTime getMinTime (void);
    ElapsedTime getMaxTime (void);
	
	// get all pax's attachment 's index in pax log
	void GetAttachments( std::vector<long>& _vVisIndex, long _ownerID,long _lPaxIndex );
	bool GetItemByID( MobLogEntry& _entry , long _lID );


	// Need to sort the event file
	// Event log was store to the file by order of dead time.
	// need to sort the event log by birth time, which is the pax ID order.
	virtual void SaveEventToFile( const char *p_filename );

protected:
	TempMobileEventLog* m_pTempLog;
};

#endif





















