#pragma once
#include "common\template.h"
#include "common\dataset.h"
#include "CommonData\PROCID.H"
#include "inputs\MobileElemConstraintDatabase.h"


class INPUTS_TRANSFER BridgeConnectorPaxEntry : public ConstraintEntry
{
protected:
    ProcessorID m_procID;
public:
    BridgeConnectorPaxEntry(){ m_procID.init(); }
    ~BridgeConnectorPaxEntry(){ clear(); }

    void initialize(CMobileElemConstraint* pConst, ProbabilityDistribution* pProb, const ProcessorID& id)
    {
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

    BridgeConnectorPaxEntry& operator=(const BridgeConnectorPaxEntry& _entry)
    {
        clear();
        ConstraintEntry::operator =(_entry);
        m_procID = _entry.m_procID;
        return *this;
    }

    int operator==(const BridgeConnectorPaxEntry& _entry)const
    {
        return *constraint == *(_entry.constraint) && m_procID == _entry.m_procID; 
    }

    int operator<(const BridgeConnectorPaxEntry& _entry) const
    {
        return *constraint < *(_entry.constraint)
            ||  !(*(_entry.constraint) < *constraint) && !(m_procID < _entry.m_procID);
    }

    static bool sortByPaxTypeString(const void* p1, const void* p2)
    {
        CString strProc1;
        CString strProc2;
        ((CMobileElemConstraint*)((BridgeConnectorPaxEntry*)p1)->getConstraint())->screenPrint(strProc1);
        ((CMobileElemConstraint*)((BridgeConnectorPaxEntry*)p2)->getConstraint())->screenPrint(strProc2);
        return strProc1 < strProc2;
    }
};

class INPUTS_TRANSFER BridgeConnectorPaxTypeWithProcIDDatabase : public CMobileElemConstraintDatabase
{
public:
    BridgeConnectorPaxTypeWithProcIDDatabase();
    ~BridgeConnectorPaxTypeWithProcIDDatabase();

    void readDatabase(ArctermFile& p_file, InputTerminal* _pInTerm);
    void writeDatabase(ArctermFile& p_file);

    void removeEntriesByProcID(const ProcessorID& pID, InputTerminal* _pInTerm);
    void replaceEntryProcID(const ProcessorID& pOldID, const ProcessorID& pNewID, InputTerminal* _pInTerm);
    bool DeleteEntry(BridgeConnectorPaxEntry* pEntry);
    std::vector<BridgeConnectorPaxEntry*> FindEntryByProcID(const ProcessorID& procID);
    const ProbabilityDistribution* FindProbDist(const ProcessorID& procID, const CMobileElemConstraint& p_const);
    void initFromMobElemConstDatabase(const CMobileElemConstraintDatabase& meDatabase, InputTerminal* _pInTerm);
};

class INPUTS_TRANSFER BridgeConnectorPaxData : public DataSet
{
protected:
    BridgeConnectorPaxTypeWithProcIDDatabase* m_pPaxData;

public:
    BridgeConnectorPaxData();
    virtual ~BridgeConnectorPaxData();

    void deletePaxType(int p_level, int p_index);
    void removeEntriesByProcID(const ProcessorID& pID, InputTerminal* _pInTerm);
    void replaceEntryProcID(const ProcessorID& pOldID, const ProcessorID& pNewID, InputTerminal* _pInTerm);
    void replaceProcessor();
//  ...
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

    BridgeConnectorPaxTypeWithProcIDDatabase* getEntryTimeDB() const
    {
        return m_pPaxData;
    }
};
