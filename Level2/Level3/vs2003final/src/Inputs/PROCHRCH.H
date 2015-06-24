/*****
*
*   Class       ProcessorEntry
*   Author      Lincoln Fellingham & KhacThu Nguyen, Aviation Research
*   Date        November 23, 1992
*   Purpose     A ProcessorEntry is an association between a Processor
*               and a ConstraintDatabase. A ConstraintDatabase is a list
*               of probabilities that can contain entries specific to
*               different passenger types.
*
*               The ProcessorID is a list of Indexes into the
*               ProcessorNames StringDictionary, defining the processor
*               or group of processor to which this database applies.
*
*               The ConstraintDatabase contains the list of exception
*               Constraints that apply to this particular Processor
*               group. Each Constraint will in turn be associated with
*               some probability value unique to this Processor group
*               and Passenger type.
*
*               If the Passenger does not fit any of the Constraint's
*               in the Processors's ConstraintDatabase, the Processor
*               will access it's ancestor Processor
*
*               The ancestor member will be a pointer to the parent
*               processor group of this processor, if it has one. This
*               will allow the processor to access its parent's
*               ConstraintDatabase
*
*				by Lijun
*				
*				ProcessDatabase has the hierarchy of processor itself, the
*				ProcessorHierarchy add one more diamension which is passenger
*				Type.			
*
*****/

#ifndef PROCESSOR_HIERARCHY_H
#define PROCESSOR_HIERARCHY_H

#include "common\dataset.h"
#include "common\elaptime.h"
#include "inputs\procdb.h"
#include <CommonData/procid.h>
#include "inputs\MobileElemConstraintDatabase.h"
#include "common\containr.h"


class ProcessorHierarchy;

class INPUTS_TRANSFER ProcessorEntry : public ProcessorDataElement
{
protected:
    friend ProcessorHierarchy;
	CMobileElemConstraintDatabase *info;

public:
    ProcessorEntry (const ProcessorID& id, const char *units)
        : ProcessorDataElement (id) { info = new CMobileElemConstraintDatabase;
        info->setUnits (units); };
	ProcessorEntry (const ProcessorEntry& entry) { *this = entry;
		info = new CMobileElemConstraintDatabase; };
	virtual ~ProcessorEntry() { delete info;  info=NULL;};

    // returns complete database
	CMobileElemConstraintDatabase *getDatabase (void) { return info; };

    // returns probability distribution that applies to paxType
    const ProbabilityDistribution *hierarchyLookup
        (const CMobileElemConstraint& paxType) const;
	
	// returns probability distribution that applies to paxType
	const ProbabilityDistribution *hierarchyLookupEx
        (const CMobileElemConstraint& paxType, const ElapsedTime& _curTime ) const;
};


class INPUTS_TRANSFER ProcessorHierarchy : public ProcessorDatabase, public DataSet
{
protected:
    const char *units;

public:
    ProcessorHierarchy (int p_file, float _fVersion = 2.4 ) : DataSet (p_file, _fVersion) {m_pInTerm =NULL;}
    virtual ~ProcessorHierarchy () {};

    // adds ProcessorEntry to entryList if  one is not yet present
    virtual void addEntry (const ProcessorID& pID);

    // if processor ID already has an entry in hierarchy, add the
    // ConstraintEntry to its PassengerConstraintDatabase
    // if it does not yet have one, create it
    void insert (const ProcessorID& id, ConstraintEntry *entry, bool _bDeleteIfExist = false );

    // returns PassengerConstraintDatabase of ProcessorEntry at passed index
    CMobileElemConstraintDatabase *getDatabase (int index) const
        { return ((ProcessorEntry *)getItem(index))->getDatabase(); };

    virtual void clear (void) { SortedContainer<ProcessorDataElement>::clear(); }

    void readData (ArctermFile& p_file, bool _bHandleProcDistWithPipe );
	void readObsoleteData( ArctermFile& p_file, bool _bHandleProcDistWithPipe );
    virtual void writeData (ArctermFile& p_file) const;
};

#endif
