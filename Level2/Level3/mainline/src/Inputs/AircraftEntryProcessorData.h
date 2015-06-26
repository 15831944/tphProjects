#pragma once
#include "common\template.h"
#include "common\dataset.h"
#include "CommonData\PROCID.H"
#include "Common\ProbabilityDistribution.h"
#include "inputs\MobileElemConstraintDatabase.h"


class INPUTS_TRANSFER AircraftEntryProcsEntry : public ConstraintEntry
{
protected:
    ProcessorID m_procID;
    ElapsedTime m_beginTime;
    ElapsedTime m_endTime;
public:
    AircraftEntryProcsEntry(){ m_procID.init(); }
    ~AircraftEntryProcsEntry(){ clear(); }

    void initialize(CMobileElemConstraint* pConst, ProbabilityDistribution* pProb, const ProcessorID& id, 
        const ElapsedTime& beginTime, const ElapsedTime& endTime)
    {
        clear();
        ConstraintEntry::initialize(pConst, pProb);
        m_procID = id;
        m_beginTime = beginTime;
        m_endTime = endTime;
    }

    void useDefaultValue(InputTerminal* _pInTerm)
    {
        ProbabilityDistribution* defaultDist = new UniformDistribution(2, 10);
        ProcessorID pID;
        pID.init();
        pID.SetStrDict(_pInTerm->inStrDict);
        initialize(new CMobileElemConstraint(_pInTerm), defaultDist, pID, ElapsedTime(0L), ElapsedTime(WholeDay-1L));
    }

    virtual void clear()
    {
        ConstraintEntry::clear();
        m_procID.init();
        m_beginTime = 0L;
        m_endTime = 0L;
    }

    void readData(ArctermFile& p_file, InputTerminal* _pInTerm);
    void writeData(ArctermFile& p_file);

    const ProcessorID& getProcID()const{ return m_procID;}
    ProcessorID obtainProcID()const{ return m_procID; }
    void setProcID(const ProcessorID& _id){ m_procID = _id; }
    ElapsedTime GetBeginTime() { return m_beginTime; }
    void SetBeginTime(const ElapsedTime& eTime) { m_beginTime = eTime; }
    ElapsedTime GetEndTime() { return m_endTime; }
    void SetEndTime(const ElapsedTime& eTime) { m_endTime = eTime; }

    AircraftEntryProcsEntry& operator=(const AircraftEntryProcsEntry& other);
    bool operator==(const AircraftEntryProcsEntry& other)const;
    bool operator<(const AircraftEntryProcsEntry& other) const;
    static bool sortEntry(const void* p1, const void* p2);
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
    int getEntryCountByProcID(const ProcessorID& procID);
    const ProbabilityDistribution* FindProbDist(const ProcessorID& procID, const CMobileElemConstraint& p_const, const ElapsedTime& curTime);
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
