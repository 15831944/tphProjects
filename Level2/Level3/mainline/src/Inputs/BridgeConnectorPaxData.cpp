#include "stdafx.h"
#include "in_term.h"
#include "MobileElemTypeStrDB.h"
#include "../Common/ProbabilityDistribution.h"
#include "../Common/termfile.h"
#include "BridgeConnectorPaxData.h"

BridgeConnectorPaxData::BridgeConnectorPaxData() : DataSet(BridgeConnectorPaxDataFile, (float)100)
{
    m_pPaxData = new CMobileElemConstraintDatabase;
    m_pPaxData->setUnits(_T("SECONDS"));
}

BridgeConnectorPaxData::~BridgeConnectorPaxData()
{
    delete m_pPaxData;
}

void BridgeConnectorPaxData::deletePaxType(int p_level, int p_index)
{
    m_pPaxData->deletePaxType (p_level, p_index);
}

void BridgeConnectorPaxData::initDefaultValues (void)
{
    ProbabilityDistribution* defaultDist = new UniformDistribution(2, 10);
    ConstraintEntry* anEntry = new ConstraintEntry;
    anEntry->initialize( new CMobileElemConstraint(m_pInTerm),defaultDist );
    m_pPaxData->addEntry(anEntry);
}

void BridgeConnectorPaxData::clear()
{
    m_pPaxData->clear();
}

void BridgeConnectorPaxData::readData(ArctermFile& p_file)
{
    m_pPaxData->readDatabase (p_file, _T("ENTRY_FLIGHT_TIME_DESTRIBUTION"), m_pInTerm );
}

void BridgeConnectorPaxData::readObsoleteData ( ArctermFile& p_file )
{
}


void BridgeConnectorPaxData::writeData (ArctermFile& p_file) const
{
    m_pPaxData->writeDatabase (p_file, _T("ENTRY_FLIGHT_TIME_DESTRIBUTION"), m_pInTerm);
}
