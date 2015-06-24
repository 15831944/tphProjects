#ifndef MISCELLANEOUS_PROCESSOR_DATA_H
#define MISCELLANEOUS_PROCESSOR_DATA_H

#include "common\template.h"
#include "common\dataset.h"
#include "inputs\procdb.h"
#include "inputs\miscproc.h"


class INPUTS_TRANSFER MiscDataElement : public ProcessorDataElement
{
protected:

    MiscData *miscData;

public:
    MiscDataElement (const ProcessorID& id) : ProcessorDataElement (id)
        { miscData = NULL; };
    MiscDataElement (const MiscDataElement& entry) { *this = entry; };
    virtual ~MiscDataElement ();


    void setData (MiscData *data) { miscData = data; };
    MiscData *getData (void) const { return miscData; };
};


class MiscProcessorData : public ProcessorDatabase
{
protected:
    int procType;

public:
    virtual ~MiscProcessorData() {};

    void setType (int type) { procType = type; };
    void allocateMiscData (MiscData* &data) const;

	bool IfUsedThisProcssor( const ProcessorID& _procID ,InputTerminal*_pTerm,std::vector<CString>& _stringVec)const;

    // returns MiscData at passed index
	MiscData *getEntry (int index) const
        { return ((MiscDataElement *)getItem(index))->getData(); };

    // removes and deallocates all entries that fit passed procID
    virtual void removeAllEntries (const ProcessorID& id,InputTerminal *_Term);

    // finds all entries that fit passed oldID and changes them to newID
    virtual void replaceAllEntries (const ProcessorID& oldID,
        const ProcessorID& newID,InputTerminal*_pTerm);

    int readMiscData (ArctermFile& file, InputTerminal* _pInTerm );
    int readObsoleteMiscData (ArctermFile& file, InputTerminal* _pInTerm );
    int writeMiscData (ArctermFile& file) const;

};


class MiscDatabase : public DataSet
{
protected:

    typedef MiscProcessorData *MiscProcDataPointer;
    MiscProcDataPointer databases[PROC_TYPES];

public:
    MiscDatabase();
    virtual ~MiscDatabase();

    MiscProcessorData *getDatabase (int type) const
        { return databases[type]; };

    void removeAllEntries (const ProcessorID& id,InputTerminal *_term);
    void replaceAllEntries (const ProcessorID& oldID,
        const ProcessorID& newID,InputTerminal*_term);

	bool IfUsedThisProcssor( const ProcessorID& _procID,InputTerminal*_pTerm,std::vector<CString>& _stringVec )const;
    virtual void clear (void);

    virtual void readData (ArctermFile& p_file);
	virtual void readObsoleteData (ArctermFile& p_file);
    virtual void writeData (ArctermFile& p_file) const;

    virtual const char *getTitle (void) const
        { return "Miscellaneous Processor data file"; }
    virtual const char *getHeaders (void) const
        { return "Proc Type,Processor,Visitors,Gates,Misc"; }
};

#endif
