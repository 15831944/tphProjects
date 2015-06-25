#include "stdafx.h"
#include "in_term.h"
#include "MobileElemTypeStrDB.h"
#include "../Common/ProbabilityDistribution.h"
#include "../Common/termfile.h"
#include "BridgeConnectorPaxData.h"


char * far BridgeConnectorPaxData::paxDataName[] =
{
	"ENTRY_FLIGHT_TIME_DESTRIBUTION",
    NULL
};

char * far BridgeConnectorPaxData::units[] =
{
	"SECONDS",
    NULL
};


BridgeConnectorPaxData::BridgeConnectorPaxData() : DataSet(BridgeConnectorPaxDataFile, (float)100)
{
    for (int i = 0; i < BC_PAX_DATA_COUNT; i++)
    {
        paxData[i] = new CMobileElemConstraintDatabase;
        paxData[i]->setUnits (units[i]);
    }
}

BridgeConnectorPaxData::~BridgeConnectorPaxData()
{
    for (int i = 0; i < BC_PAX_DATA_COUNT; i++)
        delete (paxData[i]);
}

void BridgeConnectorPaxData::deletePaxType(int p_level, int p_index)
{
    for (int i = 0; i < BC_PAX_DATA_COUNT; i++)
        paxData[i]->deletePaxType (p_level, p_index);
}

void BridgeConnectorPaxData::initDefaultValues (void)
{
    ProbabilityDistribution *defaultDist;
    ConstraintEntry *anEntry;

	//init entry flight time distribution
	defaultDist = new UniformDistribution(20, 60);
	anEntry = new ConstraintEntry;
	anEntry->initialize( new CMobileElemConstraint(m_pInTerm),defaultDist );
	paxData[EntryFlightTimeDatabase]->addEntry(anEntry);

}

void BridgeConnectorPaxData::clear (void)
{
    for (int i = 0; i < BC_PAX_DATA_COUNT; i++)
        paxData[i]->clear();
}

void BridgeConnectorPaxData::readData (ArctermFile& p_file)
{
	assert( m_pInTerm );
    for (int i = 0; i < BC_PAX_DATA_COUNT; i++)
        paxData[i]->readDatabase (p_file, paxDataName[i], m_pInTerm );
}

void BridgeConnectorPaxData::readObsoleteData ( ArctermFile& p_file )
{
}


void BridgeConnectorPaxData::writeData (ArctermFile& p_file) const
{
	assert( m_pInTerm );
    for (int i = 0; i < BC_PAX_DATA_COUNT; i++)
        paxData[i]->writeDatabase (p_file, paxDataName[i], m_pInTerm);
}
