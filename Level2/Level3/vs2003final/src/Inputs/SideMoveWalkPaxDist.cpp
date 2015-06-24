// SideMoveWalkPaxDist.cpp: implementation of the CSideMoveWalkPaxDist class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SideMoveWalkPaxDist.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSideMoveWalkPaxDist::CSideMoveWalkPaxDist():ProcessorHierarchy (MovingSideWalkPaxDistFile)
{
	units = "%";
}

CSideMoveWalkPaxDist::~CSideMoveWalkPaxDist()
{

}

// delete all references to the passed index at p_level
void CSideMoveWalkPaxDist::deletePaxType (int p_level, int p_index)
{
    ProcessorEntry *entry;
    CMobileElemConstraintDatabase *database;

    for (int i = getCount() - 1; i >= 0; i--)
    {
        entry = (ProcessorEntry *)getItem (i);
        database = entry->getDatabase();
        database->deletePaxType (p_level, p_index);
        if (!database->getCount())
            deleteItem (i);
    }
}


void CSideMoveWalkPaxDist::readData (ArctermFile& p_file)
{
	ProcessorHierarchy::readData( p_file, true );
}

void CSideMoveWalkPaxDist::readObsoleteData(ArctermFile& p_file)
{
	ProcessorHierarchy::readObsoleteData( p_file, true );
}
