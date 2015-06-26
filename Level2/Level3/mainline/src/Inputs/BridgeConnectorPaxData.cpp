#include "stdafx.h"
#include "in_term.h"
#include "MobileElemTypeStrDB.h"
#include "BridgeConnectorPaxData.h"
#include "../Common/termfile.h"
#include "../Common/ProbabilityDistribution.h"

BridgeConnectorPaxDatabase::BridgeConnectorPaxDatabase() : 
    CMobileElemConstraintDatabase(),
    DataSet(BridgeConnectorPaxDataFile, (float)100)
{
}

BridgeConnectorPaxDatabase::~BridgeConnectorPaxDatabase()
{
}

void BridgeConnectorPaxDatabase::initDefaultValues()
{
    clear();
    ProbabilityDistribution* defaultDist = new UniformDistribution(2, 10);
    ConstraintEntry* anEntry = new ConstraintEntry;
    anEntry->initialize(new CMobileElemConstraint(m_pInTerm), defaultDist);
    addEntry(anEntry);
}

void BridgeConnectorPaxDatabase::clear()
{
    ConstraintDatabase::clear();
}

void BridgeConnectorPaxDatabase::readData(ArctermFile& p_file)
{
    assert( m_pInTerm );
    readDatabase(p_file, _T("ENTRY_FLIGHT_TIME_DESTRIBUTION"), m_pInTerm );
}

void BridgeConnectorPaxDatabase::readObsoleteData(ArctermFile& p_file)
{
}

void BridgeConnectorPaxDatabase::writeData(ArctermFile& p_file)const
{
    assert( m_pInTerm );
    writeDatabase(p_file, _T("ENTRY_FLIGHT_TIME_DESTRIBUTION"), m_pInTerm);
}
