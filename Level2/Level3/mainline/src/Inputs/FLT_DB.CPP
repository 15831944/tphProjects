#include "stdafx.h"
#include "flt_db.h"
#include "../Common/flt_cnst.h"
#include "probab.h"
#include "../Common/ProbabilityDistribution.h"
#include "../Common/termfile.h"
#include "../Common/fsstream.h"

// find FIRST entry that matches fltType
// returns NULL if no constraint fits the fltType
const ProbabilityDistribution *FlightConstraintDatabase::lookup
	 (const FlightConstraint& fltType) const
{
    FlightConstraint *fltConstraint;
    int entryCount = getCount();
    for (int i = 0; i < entryCount; i++)
    {
        fltConstraint = (FlightConstraint *)getItem(i)->getConstraint();
        if (fltConstraint->fits (fltType))
            return getItem(i)->getValue();
    }
    return NULL;
}

void FlightConstraintDatabase::clear (void)
{
    ConstraintEntry *entry;
    while (getCount())
    {
        entry = getItem (0);
        removeItem (0);
        delete entry;
    }
}

void FlightConstraintDatabase::readDatabase (ArctermFile& p_file,
    const char *p_keyword, InputTerminal* _pInTerm )
{
    clear();
    p_file.reset();
    p_file.skipLines (3);

    SortedContainer<ConstraintEntry> sortedList;
    sortedList.ownsElements (0);

    FlightConstraint *aConst;
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

        aConst = new FlightConstraint;
		aConst->SetAirportDB( _pInTerm->m_pAirportDB );
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

void FlightConstraintDatabase::readErrorDatabase( ArctermFile& p_file, const char *p_keyword, InputTerminal* _pInTerm )
{
	clear();
    p_file.reset();
    p_file.skipLines (3);
	
    SortedContainer<ConstraintEntry> sortedList;
    sortedList.ownsElements (0);
	
    FlightConstraint *aConst;
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
		
        aConst = new FlightConstraint;
		aConst->SetAirportDB( _pInTerm->m_pAirportDB );
        aConst->readConstraint (p_file);
        if (units)
            p_file.getField (str, SMALL);
		
        p_file.setToField (probField);
        aProbDist = GetTerminalRelateProbDistribution (p_file,_pInTerm);
		//
		p_file.getLine();

        anEntry = new ConstraintEntry;
        anEntry->initialize (aConst, aProbDist);
        sortedList.addItem (anEntry);
    }
	
    for (int i = 0; i < sortedList.getCount(); i++)
        addEntry (sortedList.getItem (i));
}

void FlightConstraintDatabase::writeDatabase (ArctermFile& p_file,
    const char *p_keyword, InputTerminal* _pInTerm) const
{
    FlightConstraint *fltConstraint;
    ConstraintEntry *anEntry;
    const ProbabilityDistribution *aProbDist;

    int entryCount = getCount();
    for (int i = 0; i < entryCount; i++)
    {
        anEntry = getItem(i);

        if (p_keyword)
            p_file.writeField (p_keyword);

        fltConstraint = (FlightConstraint *) anEntry->getConstraint();
		fltConstraint->SetAirportDB( _pInTerm->m_pAirportDB );
        fltConstraint->writeConstraint (p_file);

        if (units)
            p_file.writeField (units);

        aProbDist = anEntry->getValue();
        aProbDist->writeDistribution (p_file);
        p_file.writeLine();
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
void FlightConstraintDatabaseWithSchedData::readDatabase( ArctermFile& p_file, const char *p_keyword, InputTerminal* _pInTerm )
{
	clear();
	p_file.reset();
	p_file.skipLines (3);

	SortedContainer<ConstraintEntry> sortedList;
	sortedList.ownsElements (0);

	FlightConstraint *aConst;
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

		aConst = new FlightConstraint;
		aConst->SetAirportDB( _pInTerm->m_pAirportDB );
		aConst->readConstraint (p_file);
		if (units)
			p_file.getField (str, SMALL);

		p_file.setToField (probField);
		aProbDist = GetTerminalRelateProbDistribution (p_file,_pInTerm);

		int nValue;
		p_file.getInteger(nValue);
		m_bNeglectSchedData = nValue >0?true:false;

		anEntry = new ConstraintEntry;
		anEntry->initialize (aConst, aProbDist);
		sortedList.addItem (anEntry);
	}

	for (int i = 0; i < sortedList.getCount(); i++)
		addEntry (sortedList.getItem (i));
}

void FlightConstraintDatabaseWithSchedData::writeDatabase( ArctermFile& p_file, const char *p_keyword, InputTerminal* _pInTerm ) const
{
	FlightConstraint *fltConstraint;
	ConstraintEntry *anEntry;
	const ProbabilityDistribution *aProbDist;

	int entryCount = getCount();
	for (int i = 0; i < entryCount; i++)
	{
		anEntry = getItem(i);

		if (p_keyword)
			p_file.writeField (p_keyword);

		fltConstraint = (FlightConstraint *) anEntry->getConstraint();
		fltConstraint->SetAirportDB( _pInTerm->m_pAirportDB );
		fltConstraint->writeConstraint (p_file);

		if (units)
			p_file.writeField (units);

		aProbDist = anEntry->getValue();
		aProbDist->writeDistribution (p_file);

		p_file.writeInt(m_bNeglectSchedData?1:0);

		p_file.writeLine();
	}
}

void FlightConstraintDatabaseWithSchedData::readObsoleteDatabase( ArctermFile& p_file, const char *p_keyword, InputTerminal* _pInTerm )
{
	FlightConstraintDatabase::readDatabase(p_file, p_keyword, _pInTerm);
}

FlightConstraintDatabaseWithSchedData::FlightConstraintDatabaseWithSchedData()
:FlightConstraintDatabase()
,m_bNeglectSchedData(false)
{

}