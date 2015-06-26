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
    m_procID.SetStrDict(_pInTerm->inStrDict);
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
    value->writeDistribution(p_file);
}

////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////

BridgeConnectorPaxTypeWithProcIDDatabase::BridgeConnectorPaxTypeWithProcIDDatabase()
{
}

BridgeConnectorPaxTypeWithProcIDDatabase::~BridgeConnectorPaxTypeWithProcIDDatabase()
{
}

void BridgeConnectorPaxTypeWithProcIDDatabase::readDatabase(ArctermFile& p_file, InputTerminal* _pInTerm)
{
    p_file.getLine();
    CString strField;
    p_file.getField(strField.GetBuffer(256), 255);
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

const ProbabilityDistribution*  BridgeConnectorPaxTypeWithProcIDDatabase::FindProbDistFitsProcID(const ProcessorID& procID, 
    const CMobileElemConstraint& pPaxType)
{
    std::vector<BridgeConnectorPaxEntry*> vResult;
    BridgeConnectorPaxEntry* pAppropriate = NULL;
    int nCount = getCount();
    for(int i=0; i<nCount; i++)
    {
        BridgeConnectorPaxEntry* pCurEntry = (BridgeConnectorPaxEntry*)getItem(i);
        const ProcessorID& curProcID = pCurEntry->getProcID();
        const CMobileElemConstraint* pCurPaxType = (CMobileElemConstraint*)pCurEntry->getConstraint();
        if(procID.idFits(curProcID) && pPaxType.fitex(*pCurPaxType))
        {
            if(pAppropriate == NULL)
            {
                pAppropriate = pCurEntry;
            }
            else
            {
                if(curProcID.idFits(pAppropriate->getProcID()) && 
                    pCurPaxType->fitex(*((CMobileElemConstraint*)pAppropriate->getConstraint())))
                {
                    pAppropriate = pCurEntry;
                }
            }
        }
    }

    if(pAppropriate)
        return pAppropriate->getValue();
    else
        return NULL;
}

void BridgeConnectorPaxTypeWithProcIDDatabase::initFromMobElemConstDatabase(const CMobileElemConstraintDatabase& meDatabase, InputTerminal* _pInTerm)
{
    clear();
    int nCount = meDatabase.getCount();
    for(int i=0; i<nCount; i++)
    {
        BridgeConnectorPaxEntry* pEntry = new BridgeConnectorPaxEntry();
        CMobileElemConstraint* pMbConst = new CMobileElemConstraint(*meDatabase.GetConstraint(i));
        ProbabilityDistribution* pProb = ProbabilityDistribution::CopyProbDistribution(meDatabase.getItem(i)->getValue());
        ProcessorID pID;
        pID.init();
        pID.SetStrDict(_pInTerm->inStrDict);
        pEntry->initialize(pMbConst, pProb, pID);
        addEntry(pEntry);
    }
}

bool BridgeConnectorPaxTypeWithProcIDDatabase::DeleteEntry(BridgeConnectorPaxEntry* pEntry)
{
    int nCount = getCount();
    for(int i=nCount-1; i>=0; i--)
    {
        if(getItem(i) == pEntry)
        {
            deleteItem(i);
            return true;
        }
    }
    return false;
}

std::vector<BridgeConnectorPaxEntry*> BridgeConnectorPaxTypeWithProcIDDatabase::FindEntryByProcID(const ProcessorID& procID)
{
    std::vector<BridgeConnectorPaxEntry*> vResult;
    int nCount = getCount();
    for(int i=0; i<nCount; i++)
    {
        BridgeConnectorPaxEntry* pEntry = (BridgeConnectorPaxEntry*)getItem(i);
        if(pEntry->getProcID() == procID)
            vResult.push_back(pEntry);
    }
    return vResult;
}

////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////

BridgeConnectorPaxData::BridgeConnectorPaxData() : DataSet(BridgeConnectorPaxDataFile, (float)101)
{
    m_pPaxData = new BridgeConnectorPaxTypeWithProcIDDatabase;
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

void BridgeConnectorPaxData::initDefaultValues()
{
    ProbabilityDistribution* defaultDist = new UniformDistribution(2, 10);
    BridgeConnectorPaxEntry* anEntry = new BridgeConnectorPaxEntry;
    ProcessorID pID;
    pID.init();
    pID.SetStrDict(m_pInTerm->inStrDict);
    anEntry->initialize(new CMobileElemConstraint(m_pInTerm), defaultDist, pID);
    m_pPaxData->addEntry(anEntry);
}

void BridgeConnectorPaxData::clear()
{
    m_pPaxData->clear();
}

void BridgeConnectorPaxData::readData(ArctermFile& p_file)
{
    m_pPaxData->readDatabase(p_file, m_pInTerm);
}

void BridgeConnectorPaxData::readObsoleteData(ArctermFile& p_file)
{
    float fVersion = p_file.getVersion();
    if(abs(fVersion-100.0f) < 0.00001f)
    {
        readData100(p_file);
    }
}

void BridgeConnectorPaxData::writeData(ArctermFile& p_file) const
{
    m_pPaxData->writeDatabase(p_file);
}

void BridgeConnectorPaxData::readData100(ArctermFile& p_file)
{
    CMobileElemConstraintDatabase paxTypeDB;
    paxTypeDB.setUnits(_T("SECONDS"));
    paxTypeDB.readDatabase(p_file, _T("ENTRY_FLIGHT_TIME_DESTRIBUTION"), m_pInTerm);
    m_pPaxData->initFromMobElemConstDatabase(paxTypeDB, m_pInTerm);
}
