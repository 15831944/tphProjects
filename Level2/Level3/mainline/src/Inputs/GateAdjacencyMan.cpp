#include "StdAfx.h"
#include "GateAdjacencyMan.h"

void CGateAdjacency::writeData(ArctermFile& p_file)
{
    m_originalGate.writeProcessorID(p_file);
    m_adjacentGate.writeProcessorID(p_file);
    if(m_bReciprocate)
        p_file.writeChar('T');
    else
        p_file.writeChar('F');
    p_file.writeLine();
}

void CGateAdjacency::readData(ArctermFile& p_file, InputTerminal* pInTerm)
{
    p_file.getLine();
    m_originalGate.SetStrDict(pInTerm->inStrDict);
    m_originalGate.readProcessorID(p_file);

    m_adjacentGate.SetStrDict(pInTerm->inStrDict);
    m_adjacentGate.readProcessorID(p_file);

    char cFlag;
    p_file.getChar(cFlag);
    if(cFlag == 'T')
        m_bReciprocate = true;
    else if(cFlag == 'F')
        m_bReciprocate = false;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////

GateAdjacencyMan::GateAdjacencyMan(GateType gateType) : 
    DataSet(ArrivalGateAdjacency, 2.3f)
{
    if(gateType == ArrGate)
        setFileType(ArrivalGateAdjacency);
    else if(gateType == DepGate)
        setFileType(DepartureGateAdjacency);
}

GateAdjacencyMan::~GateAdjacencyMan()
{
    clear();
}

int GateAdjacencyMan::FindGateAdjacencyIndex(CGateAdjacency* pGateAdj)
{
    int nCount = (int)m_vAdjacency.size();
    for(int i=0; i<nCount; i++)
    {
        if(m_vAdjacency.at(i) == pGateAdj)
        {
            return i;
        }
    }
    return -1;
}

void GateAdjacencyMan::writeData(ArctermFile& p_file) const
{
    int nCount = (int)m_vAdjacency.size();
    p_file.writeInt(nCount);
    p_file.writeLine();
    for(int i=0; i<nCount; i++)
    {
        m_vAdjacency.at(i)->writeData(p_file);
    }
    return;
}

void GateAdjacencyMan::readData(ArctermFile& p_file)
{
    clear();
    int nCount = -1;
    p_file.getLine();
    p_file.getInteger(nCount);
    for(int i=0; i<nCount; i++)
    {
        CGateAdjacency* pGateAdja = new CGateAdjacency();
        pGateAdja->readData(p_file, m_pInTerm);
        m_vAdjacency.push_back(pGateAdja);
    }
    return;
}

void GateAdjacencyMan::clear()
{
    int nCount = (int)m_vAdjacency.size();
    for(int i=0; i<nCount; i++)
    {
        delete m_vAdjacency.at(i);
    }
    m_vAdjacency.clear();
}

////////////////////////////////////////////////////////////////////////////////////////////

ArrivalGateAdjacencyMgr::ArrivalGateAdjacencyMgr() : 
    GateAdjacencyMan(ArrGate)
{
}

ArrivalGateAdjacencyMgr::~ArrivalGateAdjacencyMgr()
{

}

DepartureGateAdjacencyMgr::DepartureGateAdjacencyMgr() : 
    GateAdjacencyMan(DepGate)
{
}

DepartureGateAdjacencyMgr::~DepartureGateAdjacencyMgr()
{

}
