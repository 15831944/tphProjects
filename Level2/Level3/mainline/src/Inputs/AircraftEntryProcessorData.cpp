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
    ElapsedTime beginTime, endTime;
    p_file.getTime(beginTime);
    p_file.getTime(endTime);
    initialize(pConst, pProb, pID, beginTime, endTime);
}

void AircraftEntryProcsEntry::writeData(ArctermFile& p_file)
{
    m_procID.writeProcessorID(p_file);
    constraint->writeConstraint(p_file);
    value->writeDistribution(p_file);
    p_file.writeTime(m_beginTime);
    p_file.writeTime(m_endTime);
}

bool AircraftEntryProcsEntry::operator<(const AircraftEntryProcsEntry& other) const
{
    CMobileElemConstraint* pThisConst = (CMobileElemConstraint*)constraint;
    CMobileElemConstraint* pOtherConst = (CMobileElemConstraint*)(other.constraint);
    if(*pThisConst < *pOtherConst)
    {
        return true;
    }
    else if(*pThisConst == *pOtherConst)
    {
        if(m_procID < other.m_procID)
        {
            return true;
        }
        else if(m_procID == other.m_procID)
        {
            if(m_beginTime < other.m_beginTime)
            {
                return true;
            }
            else if(m_beginTime == other.m_beginTime)
            {
                if(m_endTime < other.m_endTime)
                    return true;
                else
                    return false;
            }
            else
            {
                return false;
            }
        }
        else
        {
            return false;
        }
    }
    else
    {
        return false;
    }
}

AircraftEntryProcsEntry& AircraftEntryProcsEntry::operator=( const AircraftEntryProcsEntry& other )
{
    clear();
    ConstraintEntry::operator =(other);
    m_procID = other.m_procID;
    m_beginTime = other.m_beginTime;
    m_endTime = other.m_endTime;
    return *this;
}

bool AircraftEntryProcsEntry::operator==( const AircraftEntryProcsEntry& other ) const
{
    return (*constraint == *(other.constraint)) && 
        (m_procID == other.m_procID) && 
        (m_beginTime == other.m_beginTime) && 
        (m_endTime == other.m_endTime);
}

bool AircraftEntryProcsEntry::sortEntry(const void* p1, const void* p2)
{
    return *((AircraftEntryProcsEntry*)p1) < *((AircraftEntryProcsEntry*)p2);
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
    const CMobileElemConstraint& paxType, const ElapsedTime& curTime)
{
    std::vector<AircraftEntryProcsEntry*> vResult;
    AircraftEntryProcsEntry* pTempEntry = NULL;
    int nCount = getCount();
    for(int i=0; i<nCount; i++)
    {
        AircraftEntryProcsEntry* pCurEntry = (AircraftEntryProcsEntry*)getItem(i);
        const ProcessorID& curProcID = pCurEntry->getProcID();
        const CMobileElemConstraint* pCurPaxType = (CMobileElemConstraint*)pCurEntry->getConstraint();
        if(!curProcID.idFits(procID) || 
            !pCurPaxType->fitex(paxType) || 
            curTime<pCurEntry->GetBeginTime() ||
            curTime>pCurEntry->GetEndTime())
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
                    CMobileElemConstraint* pTempConst = (CMobileElemConstraint*)pTempEntry->getConstraint();
                    if(pTempConst->fits(*pCurPaxType) && pCurPaxType->fits(*pTempConst))
                    {
                        if(pCurEntry->GetBeginTime() < pTempEntry->GetBeginTime())
                        {
                            pTempEntry = pCurEntry;
                        }
                        else if(pCurEntry->GetBeginTime() == pTempEntry->GetBeginTime())
                        {
                            if(pCurEntry->GetEndTime() < pTempEntry->GetEndTime())
                                pTempEntry = pCurEntry;
                        }
                        else
                        {
                            continue;
                        }
                    }
                    else if(pTempConst->fits(*pCurPaxType))
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
        pEntry->initialize(pMbConst, pProb, pID, ElapsedTime(0L), ElapsedTime(WholeDay-1L));
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

int ACEntryTimeDistDatabase::getEntryCountByProcID(const ProcessorID& procID)
{
    int nRes=0;
    int nCount = getCount();
    for(int i=0; i<nCount; i++)
    {
        AircraftEntryProcsEntry* pEntry = (AircraftEntryProcsEntry*)getItem(i);
        if(pEntry->getProcID() == procID)
            nRes++;
    }
    return nRes;
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
    AircraftEntryProcsEntry* anEntry = new AircraftEntryProcsEntry;
    anEntry->useDefaultValue(m_pInTerm);
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
