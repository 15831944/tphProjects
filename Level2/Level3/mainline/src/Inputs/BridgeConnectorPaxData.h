#pragma once
#include "common\template.h"
#include "common\dataset.h"
#include "CommonData\PROCID.H"
#include "inputs\MobileElemConstraintDatabase.h"

class INPUTS_TRANSFER ConstraintWithProcIDEntry11111 : public ConstraintEntry
{
protected:
    ProcessorID procID;
public:
    ConstraintWithProcIDEntry11111(){}
    ~ConstraintWithProcIDEntry11111(){ clear(); }

    void initialize(Constraint *p_const, ProbabilityDistribution *p_prob, const ProcessorID& id)
    {
        ConstraintEntry::initialize(p_const, p_prob);
        procID = id;
    }
    void clear()
    {
        ConstraintEntry::clear();
        procID.init();
    }

    const ProcessorID& getProcID( void ) const{ return procID;}
    ProcessorID obtainProcID( void ) const{ return procID; }
    void setProcID( const ProcessorID& _id ){ procID = _id; }

    ConstraintWithProcIDEntry11111& operator=(const ConstraintWithProcIDEntry11111& _entry)
    {
        clear();
        ConstraintEntry::operator =(_entry);
        procID = _entry.procID;
        return *this;
    }

    int operator==(const ConstraintWithProcIDEntry11111& _entry)const
    {
        return *constraint == *(_entry.constraint)
            && procID == _entry.procID; 
    }

    int operator<( const ConstraintWithProcIDEntry11111& _entry )const
    {
        return *constraint < *(_entry.constraint) ||
            !(*(_entry.constraint) < *constraint) && !(procID < _entry.procID);
    }
};

class BridgeConnectorPaxDatabase : CMobileElemConstraintDatabase
{
public:
    BridgeConnectorPaxDatabase();
    ~BridgeConnectorPaxDatabase();

    virtual void deleteItem(ConstraintEntry* pConst);
    int FindItemByConstraint(CMobileElemConstraint* pInputConst);
    void AddPaxType(CMobileElemConstraint* pInputConst, InputTerminal* _pInTerm);
    void AddPaxTypeFor260AndOlder(ConstraintWithProcIDEntry11111* pConstEntry, InputTerminal* _pInTerm);
    void readDatabase(ArctermFile& p_file, InputTerminal* _pInTerm, int paxCount);
    void writeDatabase(ArctermFile& p_file);
};

class BridgeConnectorPaxData : public DataSet
{
protected:
    CMobileElemConstraintDatabase* m_pPaxData;

public:
    BridgeConnectorPaxData();
    virtual ~BridgeConnectorPaxData();

    void deletePaxType(int p_level, int p_index);
    virtual void initDefaultValues();
    virtual void clear();
    virtual void readData(ArctermFile& p_file);
    virtual void writeData(ArctermFile& p_file) const;
    virtual void readObsoleteData( ArctermFile& p_file );

    virtual const char *getTitle() const
    { 
        return "Miscellaneous Passenger Data"; 
    }
    virtual const char *getHeaders() const
    { 
        return "Variable,Pax Type,Units,Distribution,Parameters"; 
    }

    CMobileElemConstraintDatabase* getEntryFlightTimeDestribution() const
    {
        return m_pPaxData;
    }
};
