#include "stdafx.h"
#include "PROBAB.H"
#include "in_term.h"
#include "MobileElemTypeStrDB.h"
#include "../Common/ProbabilityDistribution.h"
#include "../Common/termfile.h"
#include "AircraftEntryProcessorData.h"

////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////

void AircraftEntryProcsEntry::readData(ArctermFile& p_file, InputTerminal* _pInTerm)
{
    ProcessorID pID;
    pID.SetStrDict(_pInTerm->inStrDict);
    pID.readProcessorID(p_file);
    CMobileElemConstraint* pConst = new CMobileElemConstraint(_pInTerm);
    pConst->readConstraint (p_file);
    ProbabilityDistribution* pProb = GetTerminalRelateProbDistribution(p_file,_pInTerm);
    initialize(pConst, pProb, pID);
}

void AircraftEntryProcsEntry::writeData(ArctermFile& p_file)
{
    m_procID.writeProcessorID(p_file);
    constraint->writeConstraint(p_file);
    value->writeDistribution(p_file);
}

////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////

ACEntryTimeDistDatabase::ACEntryTimeDistDatabase()
{
}

ACEntryTimeDistDatabase::~ACEntryTimeDistDatabase()
{
}

void ACEntryTimeDistDatabase::readDatabase(ArctermFile& p_file, InputTerminal* _pInTerm)
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
        AircraftEntryProcsEntry* pEntry = new AircraftEntryProcsEntry();
        pEntry->readData(p_file, _pInTerm);
        addEntry(pEntry);
    }
}

void ACEntryTimeDistDatabase::writeDatabase(ArctermFile& p_file)
{
    p_file.writeField(_T("ENTRY_FLIGHT_TIME_DESTRIBUTION"));
    int nCount = getCount();
    p_file.writeInt(nCount);
    p_file.writeLine();
    for(int i=0; i<nCount; i++)
    {
        AircraftEntryProcsEntry* pEntry = (AircraftEntryProcsEntry*)getItem(i);
        pEntry->writeData(p_file);
        p_file.writeLine();
    }
}

const ProbabilityDistribution*  ACEntryTimeDistDatabase::FindProbDist(const ProcessorID& procID, 
    const CMobileElemConstraint& paxType)
{
    CString strPax;
    paxType.screenPrint(strPax.GetBuffer(256));
    CString strProcID = procID.GetIDString();
    std::vector<AircraftEntryProcsEntry*> vResult;
    AircraftEntryProcsEntry* pTempEntry = NULL;
    int nCount = getCount();
    for(int i=0; i<nCount; i++)
    {
        AircraftEntryProcsEntry* pCurEntry = (AircraftEntryProcsEntry*)getItem(i);
        const ProcessorID& curProcID = pCurEntry->getProcID();
        const CMobileElemConstraint* pCurPaxType = (CMobileElemConstraint*)pCurEntry->getConstraint();

        CString strCurPax;
        pCurPaxType->screenPrint(strCurPax.GetBuffer(256));
        CString strCurProcID = curProcID.GetIDString();
        if(!curProcID.idFits(procID) || !pCurPaxType->fitex(paxType))
        {
            continue;
        }
        else
        {
            if(pTempEntry == NULL)
            {
                pTempEntry = pCurEntry;
                continue;
            }
            else
            {
                if(pTempEntry->getProcID() == curProcID)
                {
                    CMobileElemConstraint* pMobConst = (CMobileElemConstraint*)pTempEntry->getConstraint();
                    if(pMobConst->fitex(*pCurPaxType))
                    {
                        pTempEntry = pCurEntry;
                    }
                    else
                    {
                        continue;
                    }
                }
                else if(pTempEntry->getProcID().idFits(curProcID))
                {
                    pTempEntry = pCurEntry;
                }
                else
                {
                    continue;
                }
            }
        }
    }

    if(pTempEntry)
        return pTempEntry->getValue();
    else
        return NULL;
}

void ACEntryTimeDistDatabase::initFromMobElemConstDatabase(const CMobileElemConstraintDatabase& meDatabase, InputTerminal* _pInTerm)
{
    clear();
    int nCount = meDatabase.getCount();
    for(int i=0; i<nCount; i++)
    {
        AircraftEntryProcsEntry* pEntry = new AircraftEntryProcsEntry();
        CMobileElemConstraint* pMbConst = new CMobileElemConstraint(*meDatabase.GetConstraint(i));
        ProbabilityDistribution* pProb = ProbabilityDistribution::CopyProbDistribution(meDatabase.getItem(i)->getValue());
        ProcessorID pID;
        pID.init();
        pID.SetStrDict(_pInTerm->inStrDict);
        pEntry->initialize(pMbConst, pProb, pID);
        addEntry(pEntry);
    }
}

bool ACEntryTimeDistDatabase::DeleteEntry(AircraftEntryProcsEntry* pEntry)
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

std::vector<AircraftEntryProcsEntry*> ACEntryTimeDistDatabase::FindEntryByProcID(const ProcessorID& procID)
{
    std::vector<AircraftEntryProcsEntry*> vResult;
    int nCount = getCount();
    for(int i=0; i<nCount; i++)
    {
        AircraftEntryProcsEntry* pEntry = (AircraftEntryProcsEntry*)getItem(i);
        if(pEntry->getProcID() == procID)
            vResult.push_back(pEntry);
    }
    return vResult;
}

void ACEntryTimeDistDatabase::removeEntriesByProcID(const ProcessorID& pID, InputTerminal* _pInTerm)
{
    int nCount = getCount();
    for(int i=nCount-1; i>=0; i--)
    {
        AircraftEntryProcsEntry* pEntry = (AircraftEntryProcsEntry*)getItem(i);
        if (pID.idFits (pEntry->getProcID()))
        {
            deleteItem(i);
        }
        else if(pEntry->getProcID().idFits(pID) && !pEntry->getProcID().isBlank())
        {
            if(!_pInTerm->HasBrother(pID, pEntry->getProcID()))
                deleteItem(i);
        }
    }
}

void ACEntryTimeDistDatabase::replaceEntryProcID(const ProcessorID& oldID, const ProcessorID& newID, InputTerminal* _pInTerm)
{
    int nCount = getCount();
    for (int i=nCount-1; i>=0; i--)
    {
        AircraftEntryProcsEntry* pEntry = (AircraftEntryProcsEntry*)getItem(i);
        if (oldID.idFits (pEntry->getProcID()))
        {
            pEntry->setProcID(newID);
        }
        else if(pEntry->getProcID().idFits(oldID) && (!pEntry->getProcID().isBlank()))
        {
            if(!pEntry->getProcID().idFits(newID))
            {
                if(!_pInTerm->HasBrother(oldID, pEntry->getProcID()))
                    pEntry->setProcID(newID);
            }
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////

AircraftEntryProcessorData::AircraftEntryProcessorData() : DataSet(BridgeConnectorPaxDataFile, (float)101)
{
    m_pPaxData = new ACEntryTimeDistDatabase;
    m_pPaxData->setUnits(_T("SECONDS"));
}

AircraftEntryProcessorData::~AircraftEntryProcessorData()
{
    m_pPaxData->clear();
    delete m_pPaxData;
}

void AircraftEntryProcessorData::deletePaxType(int p_level, int p_index)
{
    m_pPaxData->deletePaxType(p_level, p_index);
}

void AircraftEntryProcessorData::initDefaultValues()
{
    ProbabilityDistribution* defaultDist = new UniformDistribution(2, 10);
    AircraftEntryProcsEntry* anEntry = new AircraftEntryProcsEntry;
    ProcessorID pID;
    pID.init();
    pID.SetStrDict(m_pInTerm->inStrDict);
    anEntry->initialize(new CMobileElemConstraint(m_pInTerm), defaultDist, pID);
    m_pPaxData->addEntry(anEntry);
}

void AircraftEntryProcessorData::clear()
{
    m_pPaxData->clear();
}

void AircraftEntryProcessorData::readData(ArctermFile& p_file)
{
    m_pPaxData->readDatabase(p_file, m_pInTerm);
}

void AircraftEntryProcessorData::readObsoleteData(ArctermFile& p_file)
{
    float fVersion = p_file.getVersion();
    if(abs(fVersion-100.0f) < 0.00001f)
    {
        readData100(p_file);
    }
}

void AircraftEntryProcessorData::writeData(ArctermFile& p_file) const
{
    m_pPaxData->writeDatabase(p_file);
}

void AircraftEntryProcessorData::readData100(ArctermFile& p_file)
{
    CMobileElemConstraintDatabase paxTypeDB;
    paxTypeDB.setUnits(_T("SECONDS"));
    paxTypeDB.readDatabase(p_file, _T("ENTRY_FLIGHT_TIME_DESTRIBUTION"), m_pInTerm);
    m_pPaxData->initFromMobElemConstDatabase(paxTypeDB, m_pInTerm);
}

void AircraftEntryProcessorData::removeEntriesByProcID(const ProcessorID& pID, InputTerminal* _pInTerm)
{
    m_pPaxData->removeEntriesByProcID(pID, _pInTerm);
}

void AircraftEntryProcessorData::replaceEntryProcID(const ProcessorID& pOldID, const ProcessorID& pNewID, InputTerminal* _pInTerm)
{
    m_pPaxData->replaceEntryProcID(pOldID, pNewID, _pInTerm);
}
