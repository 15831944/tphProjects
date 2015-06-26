#include "stdafx.h"
#include "PROBAB.H"
#include "in_term.h"
#include "MobileElemTypeStrDB.h"
#include "../Common/ProbabilityDistribution.h"
#include "../Common/termfile.h"
#include "BridgeConnectorPaxData.h"

////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////

void BridgeConnectorPaxEntry::readData(ArctermFile& p_file, InputTerminal* _pInTerm)
{
    m_procID.readProcessorID(p_file);
    CMobileElemConstraint* pConst = new CMobileElemConstraint(_pInTerm);
    pConst->readConstraint (p_file);
    ProbabilityDistribution* pProb = GetTerminalRelateProbDistribution(p_file,_pInTerm);
    ConstraintEntry::initialize(pConst, pProb);
}

void BridgeConnectorPaxEntry::writeData(ArctermFile& p_file)
{
    m_procID.writeProcessorID(p_file);
    constraint->writeConstraint(p_file);
    getValue()->writeDistribution(p_file);
}

////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////

void BridgeConnectorPaxTypeWithProcIDDatabase::readDatabase(ArctermFile& p_file, InputTerminal* _pInTerm)
{
    p_file.getLine();
    CString strField;
    p_file.getField(strField.GetBuffer(255));
    strField.ReleaseBuffer();
    ASSERT(strField.CompareNoCase(_T("ENTRY_FLIGHT_TIME_DESTRIBUTION")) == 0);

    int paxCount;
    p_file.getInteger(paxCount);
    for(int i=0; i<paxCount; i++)
    {
        p_file.getLine();
        BridgeConnectorPaxEntry* pEntry = new BridgeConnectorPaxEntry();
        pEntry->readData(p_file, _pInTerm);
        addEntry(pEntry);
    }
}

void BridgeConnectorPaxTypeWithProcIDDatabase::writeDatabase(ArctermFile& p_file)
{
    p_file.writeField(_T("ENTRY_FLIGHT_TIME_DESTRIBUTION"));
    int nCount = getCount();
    p_file.writeInt(nCount);
    p_file.writeLine();
    for(int i=0; i<nCount; i++)
    {
        BridgeConnectorPaxEntry* pEntry = (BridgeConnectorPaxEntry*)getItem(i);
        pEntry->writeData(p_file);
        p_file.writeLine();
    }
}

std::vector<BridgeConnectorPaxEntry*> BridgeConnectorPaxTypeWithProcIDDatabase::FindPaxListByProcID(const ProcessorID& procID)
{
    std::vector<BridgeConnectorPaxEntry*> vResult;
    ProcessorID appropriateProcID;
    int nCount = getCount();
    for(int i=0; i<nCount; i++)
    {
        ProcessorID curProcID = ((BridgeConnectorPaxEntry*)getItem(i))->getProcID();
        if(procID == curProcID)
        {
            appropriateProcID == procID;
            break;
        }
        else if(procID.idFits(curProcID))
        {
            if(curProcID.idFits(appropriateProcID))
            {
                appropriateProcID = curProcID;
            }
        }
    }

    for(int i=0; i<nCount; i++)
    {
        ProcessorID curProcID = ((BridgeConnectorPaxEntry*)getItem(i))->getProcID();
        if(appropriateProcID == curProcID)
        {
            vResult.push_back((BridgeConnectorPaxEntry*)getItem(i));
        }
    }

    return vResult;
}

void BridgeConnectorPaxTypeWithProcIDDatabase::initFromMobElemConstDatabase(const CMobileElemConstraintDatabase& meDatabase)
{
    clear();
    int nCount = meDatabase.getCount();
    for(int i=0; i<nCount; i++)
    {
        BridgeConnectorPaxEntry* pPax = new BridgeConnectorPaxEntry();
        CMobileElemConstraint* pMbConst = new CMobileElemConstraint(*meDatabase.GetConstraint(i));
        ProbabilityDistribution* pProb = ProbabilityDistribution::CopyProbDistribution(meDatabase.getItem(i)->getValue());
        pPax->initialize(pMbConst, pProb, ProcessorID());
        addEntry(pPax);
    }
}


////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
BridgeConnectorPaxData::BridgeConnectorPaxData() : DataSet(BridgeConnectorPaxDataFile, (float)100)
{
    m_pPaxData = new CMobileElemConstraintDatabase;
    m_pPaxData->setUnits(_T("SECONDS"));
}

BridgeConnectorPaxData::~BridgeConnectorPaxData()
{
    m_pPaxData->clear();
    delete m_pPaxData;
}

void BridgeConnectorPaxData::deletePaxType(int p_level, int p_index)
{
    m_pPaxData->deletePaxType (p_level, p_index);
}

void BridgeConnectorPaxData::initDefaultValues ()
{
    ProbabilityDistribution* defaultDist = new UniformDistribution(2, 10);
    ConstraintEntry* anEntry = new ConstraintEntry;
    anEntry->initialize(new CMobileElemConstraint(m_pInTerm),defaultDist);
    m_pPaxData->addEntry(anEntry);
}

void BridgeConnectorPaxData::clear()
{
    m_pPaxData->clear();
}

void BridgeConnectorPaxData::readData(ArctermFile& p_file)
{
    m_pPaxData->readDatabase(p_file, _T("ENTRY_FLIGHT_TIME_DESTRIBUTION"), m_pInTerm);
}

void BridgeConnectorPaxData::readObsoleteData(ArctermFile& p_file)
{
}

void BridgeConnectorPaxData::writeData(ArctermFile& p_file) const
{
    m_pPaxData->writeDatabase (p_file, _T("ENTRY_FLIGHT_TIME_DESTRIBUTION"), m_pInTerm);
}
