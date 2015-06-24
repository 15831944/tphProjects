/*****
*
*   Class       ProcessorDatabase
*               ProcessorDataElement
*   Author      Lincoln Fellingham, Aviation Research Corporation
*   Date        March 23, 1994
*   Purpose     A ProcessorDataElement is an association between a
*               Processor and a piece of data
*
*               The ProcessorID is the string of Indexes into the
*               ProcessorNames StringDictionary. This can be defined as
*               either a unique Processor or a Processor group.
*
*****/

#ifndef PROCESSOR_DATABASE_H
#define PROCESSOR_DATABASE_H

#include "common\containr.h"
#include <CommonData/procid.h>
#include <Inputs/InputsDll.h>

class ProcessorDatabase;

class ProcessorDataElement
{
protected:
    friend ProcessorDatabase;

    ProcessorID procID;
    ProcessorDataElement *closestAncestor;

public:

    // constructor
    ProcessorDataElement () { closestAncestor = NULL; };
    ProcessorDataElement (const ProcessorID& id)
        { procID = id; closestAncestor = NULL; };
    ProcessorDataElement (const ProcessorDataElement& entry)
        { procID = entry.procID; closestAncestor = entry.closestAncestor; };
    virtual ~ProcessorDataElement () {};

    const ProcessorID *getID (void) const { return &procID; };
    void changeID (const ProcessorID& newID) { procID.copyID (newID); };

    int operator == (const ProcessorDataElement& element) const
        { return procID == element.procID; }

    int operator < (const ProcessorDataElement& element) const
        { return procID < element.procID; }
};


class InputTerminal;
class INPUTS_TRANSFER ProcessorDatabase : public SortedContainer<ProcessorDataElement>
{
protected:

    // scans backwards in the list of processor entries for an entry
    // belonging to curEntry's super-group
    // since the entries are sorted by processor ID, any entry's
    // ancestor will be before it in the list
    ProcessorDataElement *getAncestor (int start,
        const ProcessorDataElement& curEntry) const ;

public:

	// found the best fit processor in this, return -1 for no result.
	int findBestMatch( const ProcessorID& _id );

    ProcessorDatabase () : SortedContainer<ProcessorDataElement> (32,0,16)
        { ownsElements(1); };
    virtual ~ProcessorDatabase () {};

    // adds a ProcessorDataElement to the list if it does not already exist
    virtual void addEntry (const ProcessorID& pID);

    // performs a binary search for an entry belonging to id
    int findEntry (const ProcessorID& id) const;

    // returns ProcessorID of ProcessorDataElement at passed index
    const ProcessorID *getProcName (int index) const
         { return getItem (index)->getID(); };

	virtual bool getUsedProInfo(const ProcessorID& _id,InputTerminal *_pTerm,std::vector<CString>& _strVec);
    // removes and deallocates all entries that fit passed procID
    virtual void removeAllEntries (const ProcessorID& id,InputTerminal*_term);

    // finds all entries that fit passed oldID and changes them to newID
    virtual void replaceAllEntries (const ProcessorID& oldID,
        const ProcessorID& newID,InputTerminal* _term);

    // returns ProcessorDataElement that corresponds to passed procID
    ProcessorDataElement* FindEntry (const ProcessorID& procID) const;
    
	// set closestAncestor for each entry
    void buildHierarchy (void) const;

    // get the closest entry in hierachy for id
    ProcessorDataElement* getEntryPoint (const ProcessorID& id) const;
	// get all proximity entry in hierachy for id, and the first one is the closest entry,next one is closer, and so on.
	void getEntryPoint (const ProcessorID& id, std::vector<ProcessorDataElement *>& _vResultData) const;
		
};

#endif
