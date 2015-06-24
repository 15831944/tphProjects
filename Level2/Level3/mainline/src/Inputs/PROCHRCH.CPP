#include "stdafx.h"
#include "inputs\prochrch.h"
#include <CommonData/procid.h>
#include "inputs\pax_db.h"
#include "inputs\probab.h"
#include "common\termfile.h"
#include "common\exeption.h"
#include "inputs\MultiMobConstraint.h"
#include "PROCIDList.h"



// returns probability distribution that applies to paxType
const ProbabilityDistribution *ProcessorEntry::hierarchyLookup
    (const CMobileElemConstraint& paxType) const
{
	const ProbabilityDistribution *prob = info->FindFit (paxType);
    if (prob != NULL)
        return prob;

    if (closestAncestor != NULL)
        return (((ProcessorEntry*)closestAncestor)->hierarchyLookup (paxType));
    return NULL;
}

// returns probability distribution that applies to paxType
const ProbabilityDistribution *ProcessorEntry::hierarchyLookupEx
(const CMobileElemConstraint& paxType, const ElapsedTime& _curTime ) const
{
	const ProbabilityDistribution *prob = info->FindFitEx (paxType, _curTime );
    if (prob != NULL)
        return prob;
	
    if (closestAncestor != NULL)
        return (((ProcessorEntry*)closestAncestor)->hierarchyLookupEx (paxType, _curTime));
	return NULL;
}

/*****
*
*   Processor Hierarchy
*
*****/

// adds a ProcessorDataElement to the list if it does not already exist
void ProcessorHierarchy::addEntry (const ProcessorID& pID)
{
    ProcessorEntry *pEntry = (ProcessorEntry *)FindEntry (pID);
	if (!pEntry)
        addItem (new ProcessorEntry(pID, units));
}

// if processor ID already has an entry in hierarchy, add the
// Constraint and Probability to its PassengerConstraintDatabase
// if it does not yet have one, create it
void ProcessorHierarchy::insert (const ProcessorID& id,
    ConstraintEntry *entry, bool _bDeleteIfExist )
{
    addEntry (id);
    int index = findEntry (id);
    ProcessorEntry *pEntry = (ProcessorEntry *)getItem(index);
    pEntry->info->addEntry (entry, _bDeleteIfExist );
}

// reads Processor Hierarchy from an Arcterm File
void ProcessorHierarchy::readData (ArctermFile& p_file, bool _bHandleProcDistWithPipe )
{
    ProcessorIDList idList;
	idList.ownsElements( 1 );
	CMultiMobConstraint multiConst;
	ProbabilityDistribution* prob;
    ConstraintEntry *anEntry;
    CMobileElemConstraint *aConst;

	assert( m_pInTerm );
    while (p_file.getLine())
    {
        idList.readIDList (p_file, m_pInTerm );
        multiConst.readConstraints (p_file, m_pInTerm );

        if (units)
            p_file.skipField(1);

        prob = GetTerminalRelateProbDistribution (p_file,m_pInTerm, _bHandleProcDistWithPipe );

        for (int i = 0; i < idList.getCount(); i++)
            for (int j = 0; j < multiConst.getCount(); j++)
            {
                aConst = new CMobileElemConstraint(m_pInTerm);
                *aConst = *multiConst.getConstraint(j);
                anEntry = new ConstraintEntry;
                anEntry->initialize (aConst, prob);
                insert (*(idList.getID (i)), anEntry);
            }
    }
    buildHierarchy();
}

void ProcessorHierarchy::readObsoleteData( ArctermFile& p_file, bool _bHandleProcDistWithPipe )
{
	assert( m_pInTerm );
	float fVersion = p_file.getVersion();
	
	if( fVersion == (float)2.3 ) // this is a error version
	{
		ProcessorIDList idList;
		idList.ownsElements( 1 );
		CMultiMobConstraint multiConst;
		ProbabilityDistribution* prob;
		ConstraintEntry *anEntry;
		CMobileElemConstraint *aConst;
		
		while (p_file.getLine())
		{
			idList.readIDList (p_file, m_pInTerm );
			multiConst.readConstraints (p_file, m_pInTerm );
			
			if (units)
				p_file.skipField(1);
			
			prob = GetTerminalRelateProbDistribution (p_file,m_pInTerm, _bHandleProcDistWithPipe );
			// need not to read the last three member

			for (int i = 0; i < idList.getCount(); i++)
				for (int j = 0; j < multiConst.getCount(); j++)
				{
					aConst = new CMobileElemConstraint(m_pInTerm);
					*aConst = *multiConst.getConstraint(j);
					anEntry = new ConstraintEntry;
					anEntry->initialize (aConst, prob);
					insert (*(idList.getID (i)), anEntry);
				}
		}
		buildHierarchy();
	}
	if(fVersion < (float)2.8)
	{
		ProcessorIDList idList;
		idList.ownsElements( 1 );
		CMultiMobConstraint multiConst;
		ProbabilityDistribution* prob;
		ConstraintEntry *anEntry;
		CMobileElemConstraint *aConst;

		assert( m_pInTerm );
		while (p_file.getLine())
		{
			idList.readIDList (p_file, m_pInTerm );
			multiConst.readConstraints (p_file, m_pInTerm );
			int nConstraintCount = 0; 
			for( ;nConstraintCount < multiConst.getCount(); ++ nConstraintCount)
			{
				if(multiConst.getConstraint(nConstraintCount)->GetMobileElementType() == enum_MobileElementType_ALL)
				{
					multiConst.getConstraint(nConstraintCount)->SetMobileElementType(enum_MobileElementType_PAX);
				}

			}
			if (units)
				p_file.skipField(1);

			prob = GetTerminalRelateProbDistribution (p_file,m_pInTerm, _bHandleProcDistWithPipe );

			for (int i = 0; i < idList.getCount(); i++)
				for (int j = 0; j < multiConst.getCount(); j++)
				{
					aConst = new CMobileElemConstraint(m_pInTerm);
					*aConst = *multiConst.getConstraint(j);
					anEntry = new ConstraintEntry;
					anEntry->initialize (aConst, prob);
					insert (*(idList.getID (i)), anEntry);
				}
		}
		buildHierarchy();
	}
	else
	{
		readData( p_file, _bHandleProcDistWithPipe );
	}
}

// writes Processor Hierarchy to an Arcterm File
void ProcessorHierarchy::writeData (ArctermFile& p_file) const
{
    char procName[256];
    const ProcessorID *procID;
    CMobileElemConstraintDatabase *database;

    int entryCount = getCount();
	assert( m_pInTerm );
    for (int i = 0; i < entryCount; i++)
    {
        procID = getProcName(i);
        procID->printID (procName);

        database = getDatabase(i);
        database->writeDatabase (p_file, procName, m_pInTerm);
    }
}

