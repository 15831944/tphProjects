#ifndef PROCESSOR_ASSIGNMENT_DATABASE_H
#define PROCESSOR_ASSIGNMENT_DATABASE_H


#include "common\containr.h"
#include "common\dataset.h"
#include "inputs\procdb.h"
#include <CommonData/procid.h>
#include "inputs\assign.h"

class CADORecordset ;
class INPUTS_TRANSFER ProcAssignEntry : public ProcessorDataElement
{
protected:
    ProcessorRosterSchedule *schedule;
public:
    virtual ~ProcAssignEntry ();

    ProcAssignEntry (const ProcessorID& id) : ProcessorDataElement (id)
        { schedule = new ProcessorRosterSchedule; };
    ProcAssignEntry (const ProcAssignEntry& entry) ;
	const ProcAssignEntry& operator = ( const ProcAssignEntry& _anotherInstance );
		

    ProcessorRosterSchedule *getSchedule (void) const { return schedule; };
    void updateProcessors (ProcessorList* _procList);
    void scheduleFirstEvent (void) const;
};


class INPUTS_TRANSFER ProcAssignDatabase : public ProcessorDatabase, public DataSet
{
	bool m_bNeedRepeatEntry;
public:
    ProcAssignDatabase () : DataSet (ProcessorScheduleFile,(float)2.6) { m_bNeedRepeatEntry = false; };
    virtual ~ProcAssignDatabase () {};

    // adds a ProcessorDataElement to the list if it does not already exist
    virtual void addEntry (const ProcessorID& pID);

    virtual void insert (const ProcessorID& pID, ProcessorRoster& procAssign);

    // returns PassengerConstraintDatabase of ProcessorEntry at passed index
    ProcessorRosterSchedule *getDatabase (int index) const
        { return ((ProcAssignEntry *)getItem(index))->getSchedule(); };

    // schedules the first assignment change for each processor group
    void initEvents () const;

    // delete all references to the passed index at p_level
    void deletePaxType (int p_level, int p_index);

	// from relative time to absolute time
	int computeAbsolutTime( InputTerminal* _pTerminal, FlightSchedule* _pFlightSchedule );

	// divide an event into an 'opne' event and a 'close' event
	void divideEvents( InputTerminal* _pTerminal );

	// Generate Events for _nDays - 1 days
	void GenerateEntryForMultipleDays( int _nDays, InputTerminal* _pTerminal  );
	
	// create default event for processor which have no user's event
	int  createDefaultEvent();

    virtual void clear (void) { SortedContainer<ProcessorDataElement>::clear(); }

    virtual void readData (ArctermFile& p_file);
    virtual void writeData (ArctermFile& p_file) const;
	void convertOldFormatToNew();
    virtual const char *getTitle (void) const
        { return "Processor Assignment file"; }
    virtual const char *getHeaders (void) const
        { return "Processor,Status,Time,Assignment"; }
	virtual  bool getUsedProInfo(const ProcessorID& _id,InputTerminal *_pTerm,std::vector<CString>& _strVec);

protected:
	void initDaily(const ProcessorID& pID, ProcessorRoster& procAssign);
public:
	void ReadProcAssigmentFromDB() ;
	void WriteProAssigmentToDB() ;

protected:
	ProcessorID GetProcAssignment(ProcessorRoster& _proAss , CADORecordset* _set) ;
	void WriteProAssignment(CString prostr ,ProcessorRoster& _proAss) ;
	void DeleteProAssigment(ProcessorRoster& _proAss) ;
};

#endif
