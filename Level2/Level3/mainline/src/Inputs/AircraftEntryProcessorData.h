#pragma once
#include "common\template.h"
#include "common\dataset.h"
#include "CommonData\PROCID.H"
#include "inputs\MobileElemConstraintDatabase.h"


class INPUTS_TRANSFER AircraftEntryProcsEntry : public ConstraintEntry
{
protected:
    ProcessorID m_procID;
public:
    AircraftEntryProcsEntry(){ m_procID.init(); }
    ~AircraftEntryProcsEntry(){ clear(); }

    void initialize(CMobileElemConstraint* pConst, ProbabilityDistribution* pProb, const ProcessorID& id)
    {
        clear();
        ConstraintEntry::initialize(pConst, pProb);
        m_procID = id;
    }
    virtual void clear()
    {
        ConstraintEntry::clear();
        m_procID.init();
    }

    void readData(ArctermFile& p_file, InputTerminal* _pInTerm);
    void writeData(ArctermFile& p_file);

    const ProcessorID& getProcID()const{ return m_procID;}
    ProcessorID obtainProcID()const{ return m_procID; }
    void setProcID(const ProcessorID& _id){ m_procID = _id; }

    AircraftEntryProcsEntry& operator=(const AircraftEntryProcsEntry& _entry)
    {
        clear();
        ConstraintEntry::operator =(_entry);
        m_procID = _entry.m_procID;
        return *this;
    }

    int operator==(const AircraftEntryProcsEntry& _entry)const
    {
        return *constraint == *(_entry.constraint) && m_procID == _entry.m_procID; 
    }

    int operator<(const AircraftEntryProcsEntry& _entry) const
    {
        return *constraint < *(_entry.constraint)
            ||  !(*(_entry.constraint) < *constraint) && !(m_procID < _entry.m_procID);
    }

    static bool sortByPaxTypeString(const void* p1, const void* p2)
    {
        CString strPax1;
        CString strPax2;
        ((CMobileElemConstraint*)((AircraftEntryProcsEntry*)p1)->getConstraint())->screenPrint(strPax1);
        ((CMobileElemConstraint*)((AircraftEntryProcsEntry*)p2)->getConstraint())->screenPrint(strPax2);
        return strPax1 < strPax2;
    }
};

class INPUTS_TRANSFER ACEntryTimeDistDatabase : public CMobileElemConstraintDatabase
{
public:
    ACEntryTimeDistDatabase();
    ~ACEntryTimeDistDatabase();

    void readDatabase(ArctermFile& p_file, InputTerminal* _pInTerm);
    void writeDatabase(ArctermFile& p_file);

    void removeEntriesByProcID(const ProcessorID& pID, InputTerminal* _pInTerm);
    void replaceEntryProcID(const ProcessorID& pOldID, const ProcessorID& pNewID, InputTerminal* _pInTerm);
    bool DeleteEntry(AircraftEntryProcsEntry* pEntry);
    std::vector<AircraftEntryProcsEntry*> FindEntryByProcID(const ProcessorID& procID);
    const ProbabilityDistribution* FindProbDist(const ProcessorID& procID, const CMobileElemConstraint& p_const);
    void initFromMobElemConstDatabase(const CMobileElemConstraintDatabase& meDatabase, InputTerminal* _pInTerm);
};

class INPUTS_TRANSFER AircraftEntryProcessorData : public DataSet
{
protected:
    ACEntryTimeDistDatabase* m_pPaxData;

public:
    AircraftEntryProcessorData();
    virtual ~AircraftEntryProcessorData();

    void deletePaxType(int p_level, int p_index);
    void removeEntriesByProcID(const ProcessorID& pID, InputTerminal* _pInTerm);
    void replaceEntryProcID(const ProcessorID& pOldID, const ProcessorID& pNewID, InputTerminal* _pInTerm);
    void replaceProcessor();

    virtual void initDefaultValues();
    virtual void clear();
    virtual void readData(ArctermFile& p_file);
    virtual void writeData(ArctermFile& p_file) const;
    virtual void readObsoleteData(ArctermFile& p_file);
    virtual void readData100(ArctermFile& p_file);

    virtual const char *getTitle() const
    { 
        return "Miscellaneous Passenger Data"; 
    }
    virtual const char *getHeaders() const
    { 
        return "Variable,Pax Type,Units,Distribution,Parameters"; 
    }

    ACEntryTimeDistDatabase* getEntryTimeDB() const
    {
        return m_pPaxData;
    }
};
