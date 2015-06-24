#include "stdafx.h"
#include "common\fsstream.h"
#include <string.h>
#include "inputs\pax_db.h"
#include "inputs\pax_cnst.h"
#include "inputs\probab.h"
#include "common\termfile.h"
#include "ProcessorDistributionWithPipe.h"
//Jan98
#include "inputs\fltdist.h"

// find FIRST entry that matches p_const
// returns NULL if no constraint fits the p_const
const ProbabilityDistribution *CPassengerConstraintDatabase::lookup
    (const CPassengerConstraint& p_const) const
{
    CPassengerConstraint *paxConstraint;
    int entryCount = getCount();
    for (int i = 0; i < entryCount; i++)
    {
        paxConstraint = (CPassengerConstraint *)getItem(i)->getConstraint();
        if (paxConstraint->fits (p_const))
            return getItem(i)->getValue();
    }
    return NULL;
}

// delete all references to the passed index at p_level
void CPassengerConstraintDatabase::deletePaxType (int p_level, int p_index)
{
    for (int i = getCount() - 1; i >= 0; i--)
        if ((getConstraint (i))->contains (p_level, p_index))
            deleteItem (i);
}
void CPassengerConstraintDatabase::deletePaxType( const CPassengerConstraint* _paxConstraint )
{
	for (int i = getCount() - 1; i >= 0; i--)
	{
		if ( (getConstraint (i))->isEqual( _paxConstraint) )
		{
			deleteItem (i);
			return ;
		}   
	}
}
/*
void CPassengerConstraintDatabase::deletePipe ( int _iPipeIdx )
{
	for (int i = getCount() - 1; i >= 0; i--)
	{
		CProcessorDistributionWithPipe* pDistPipe = ( CProcessorDistributionWithPipe*) getDist( i );
		pDistPipe->DeletePipe( _iPipeIdx );
	}
}
bool CPassengerConstraintDatabase::IfUseThisPipe( int _iPipeIdx )const
{
	for (int i = getCount() - 1; i >= 0; i--)
	{
		const CProcessorDistributionWithPipe* pDistPipe = (const CProcessorDistributionWithPipe*) getDist( i );
		if( pDistPipe->IfUseThisPipe( _iPipeIdx ) )
			return true;
	}
	return false;
}
#if 0
void CPassengerConstraintDatabase::clear (void)
{
    ConstraintEntry *entry;
    while (getCount())
    {
        entry = getItem (0);
        removeItem (0);
        delete entry;
    }
}
#endif
*/
void CPassengerConstraintDatabase::readDatabase (ArctermFile& p_file,
    const char *p_keyword, InputTerminal* _pInTerm )
{
    clear();
    p_file.reset();
    p_file.skipLines (3);

    SortedContainer<ConstraintEntry> sortedList;
    sortedList.ownsElements (0);

    CPassengerConstraint *aConst;
    ProbabilityDistribution *aProbDist;
    ConstraintEntry *anEntry;
    char str[64];
    int probField = 1 + (p_keyword != NULL) + (units != NULL);

    while (p_file.getLine())
    {
        if (p_keyword)
        {
            p_file.getField (str, SMALL);
            if (_stricmp (str, p_keyword))
                continue;
        }

        aConst = new CPassengerConstraint(_pInTerm);
		aConst->SetInputTerminal( _pInTerm );
        aConst->readConstraint (p_file);
        if (units)
            p_file.getField (str, SMALL);

        p_file.setToField (probField);
        aProbDist = GetTerminalRelateProbDistribution (p_file,_pInTerm);

        anEntry = new ConstraintEntry;
        anEntry->initialize (aConst, aProbDist);
        sortedList.addItem (anEntry);
    }

    for (int i = 0; i < sortedList.getCount(); i++)
        addEntry (sortedList.getItem (i));
}


void CPassengerConstraintDatabase::readErrorDatabase( ArctermFile& p_file, const char *p_keyword, InputTerminal* _pInTerm )
{
	readDatabase( p_file, p_keyword, _pInTerm );
}


void CPassengerConstraintDatabase::writeDatabase (ArctermFile& p_file,
    const char *p_keyword, InputTerminal* _pInTerm ) const
{
    CPassengerConstraint *paxConstraint;
    ConstraintEntry *anEntry;
    const ProbabilityDistribution *aProbDist;

    int entryCount = getCount();
    for (int i = 0; i < entryCount; i++)
    {
        anEntry = getItem(i);

        if (p_keyword)
            p_file.writeField (p_keyword);

		  paxConstraint = (CPassengerConstraint *) anEntry->getConstraint();
		  paxConstraint->SetInputTerminal( _pInTerm );
		  paxConstraint->writeConstraint (p_file);

		  if (units)
				p_file.writeField (units);

		  aProbDist = anEntry->getValue();
		  aProbDist->writeDistribution (p_file);
		  p_file.writeLine();
	 }
}

//Jan98 : for protection of writing to file Hubbing.hub
int CPassengerConstraintDatabase::checkLastOfProbs()
{
	 ConstraintEntry *anEntry;
	 const ProbabilityDistribution *aProbDist;

	 int entryCount = getCount();
	 for (int i = 0; i < entryCount; i++)
	 {
		  anEntry = getItem(i);
		  aProbDist = anEntry->getValue();
		  if ((((FlightTypeDistribution*)aProbDist)->getLastOfProbs()) != 100)
				return 1;
	 }

    return 0;
}


// find FIRST entry that matches p_const
// returns NULL if no constraint fits the p_const
const ProbabilityDistribution *CPassengerConstraintDatabase::FindEqual
    (const CPassengerConstraint& p_const) const
{
    int entryCount = getCount();
    for (int i = 0; i < entryCount; i++)
    {
	    const CPassengerConstraint *paxConstraint = (CPassengerConstraint *)getItem(i)->getConstraint();
        if (paxConstraint->isEqual(&p_const))
            return getItem(i)->getValue();
    }
    return NULL;
}
/*
ProbabilityDistribution *CPassengerConstraintDatabase::FindEqual(const CPassengerConstraint& p_const)
{
	int entryCount = getCount();
    for (int i = 0; i < entryCount; i++)
    {
	    const CPassengerConstraint *paxConstraint = (CPassengerConstraint *)getItem(i)->getConstraint();
        if (paxConstraint->isEqual(&p_const))
            return (ProbabilityDistribution *) ( getItem(i)->getValue() );
    }
    return NULL;
}
*/
