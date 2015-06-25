#pragma once
#include <CommonData/procid.h>
#include "Common/DATASET.H"
#include "Common\TERMFILE.H"
#include "IN_TERM.H"
#include "Engine\GATE.H"

class CGateAdjacency
{
public:
    CGateAdjacency() : m_bReciprocate(false) { m_originalGate.init(); m_adjacentGate.init();}
    ~CGateAdjacency(){}
private:
    ProcessorID m_originalGate;
    ProcessorID m_adjacentGate;
    bool m_bReciprocate;
public:
    ProcessorID GetOriginalGate() const { return m_originalGate; }
    void SetOriginalGate(ProcessorID procID) { m_originalGate = procID; }
    ProcessorID GetAdjacentGate() const { return m_adjacentGate; }
    void SetAdjacentGate(ProcessorID procID) { m_adjacentGate = procID; }
    bool GetReciprocate() const { return m_bReciprocate; }
    void SetReciprocate(bool reci) { m_bReciprocate = reci; }
    void writeData(ArctermFile& p_file);
    void readData(ArctermFile& p_file, InputTerminal* pInTerm);

    bool operator==(const CGateAdjacency& other)
    {
        if( other.m_originalGate == m_originalGate &&
            other.m_adjacentGate == m_adjacentGate)
            return true;
        return false;
    }

    CGateAdjacency& operator=(const CGateAdjacency& other)
    {
        m_originalGate = other.m_originalGate;
        m_adjacentGate = other.m_adjacentGate;
        m_bReciprocate = other.m_bReciprocate;
        return *this;
    }
};


class GateAdjacencyMan : public DataSet
{
public:
    GateAdjacencyMan(GateType gateType);
    ~GateAdjacencyMan();
protected:
    std::vector<CGateAdjacency*> m_vAdjacency;
public:
    virtual void clear();
    virtual void readData(ArctermFile& p_file);
    virtual void readObsoleteData(ArctermFile& p_file){ readData(p_file); }
    virtual void writeData(ArctermFile& p_file) const;

    virtual const char *getTitle (void) const { return "Gate Adjacency Definition"; }
    virtual const char *getHeaders (void) const{  return "Original Gate, Adjacent Gate, Reciprocate"; }
public:
    std::vector<CGateAdjacency*>* GetGateAdjacency() { return &m_vAdjacency; }
    int FindGateAdjacencyIndex(CGateAdjacency* pGateAdj);
};

class ArrivalGateAdjacencyMgr : public GateAdjacencyMan
{
public:
    ArrivalGateAdjacencyMgr();
    ~ArrivalGateAdjacencyMgr();
    virtual const char *getTitle (void) const { return "Arrival Gate Adjacency Definition"; }
};


class DepartureGateAdjacencyMgr : public GateAdjacencyMan
{
public:
    DepartureGateAdjacencyMgr();
    ~DepartureGateAdjacencyMgr();
    virtual const char *getTitle (void) const { return "Departure Gate Adjacency Definition"; }
};
