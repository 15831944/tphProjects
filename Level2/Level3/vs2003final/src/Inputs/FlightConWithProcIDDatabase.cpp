// FlightConWithProcIDDatabase.cpp: implementation of the FlightConWithProcIDDatabase class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "../Common/termfile.h"
#include "../Common/ProbabilityDistribution.h"
#include "probab.h"
#include "FlightConWithProcIDDatabase.h"




int ConstraintWithProcIDEntry::operator<( const ConstraintWithProcIDEntry& _entry ) const
{
	return *constraint < *(_entry.constraint)
		   ||  !(*(_entry.constraint) < *constraint) && !(procID < _entry.procID) ;
	/*
	if( *constraint == *(_entry.constraint) )
	{
		if( procID.isDefault() && _entry.procID.isDefault())
			return false;
		if( procID.isDefault() )
			return false;
		if( _entry.procID.isDefault() ) 
			return true;
		else
			return !(procID < _entry.procID);
	}
	else
		return *constraint < *(_entry.constraint);
	*/
}
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

FlightConWithProcIDDatabase::FlightConWithProcIDDatabase()
{

}

FlightConWithProcIDDatabase::~FlightConWithProcIDDatabase()
{

}


//////////////////////////////////////////////////////////////////////////
// find first entry that matches paxType
// returns NULL if no constraint fits the paxType
const ProbabilityDistribution* FlightConWithProcIDDatabase::lookup_with_procid (const FlightConstraint& p_type, const ProcessorID& _id ) const
{
    int entryCount = getCount();
    for (int i = 0; i < entryCount; i++)
    {
		ConstraintWithProcIDEntry* pEntry = (ConstraintWithProcIDEntry*) getItem(i);
        const FlightConstraint *fltConstraint = (const FlightConstraint *)(pEntry->getConstraint());
		const ProcessorID& procID = pEntry->getProcID();

        if ( fltConstraint->fits (p_type) && procID.idFits(_id)  )
            return pEntry->getValue();
    }
    return NULL;
}


void FlightConWithProcIDDatabase::addEntry (ConstraintWithProcIDEntry* anEntry, bool _bDeleteIfExist /*= false*/ )
{
	if( _bDeleteIfExist )
    {
		int index = findItem (anEntry);
		if (index != INT_MAX)
			deleteItem (index);
    }
	addItem (anEntry);
}

// text based file I/O
void FlightConWithProcIDDatabase::readDatabase (ArctermFile& p_file, const char *p_keyword, InputTerminal* _pInTerm )
{
	clear();
    p_file.reset();
    p_file.skipLines (3);
	
	// the temp sort list
    SortedContainer<ConstraintWithProcIDEntry> sortedList;
    sortedList.ownsElements (0);
	
	char str[64];
    FlightConstraint *aConst;
    ProbabilityDistribution *aProbDist;
    ConstraintWithProcIDEntry *pEntry;
	ProcessorID procID;
    
	procID.SetStrDict( _pInTerm->inStrDict );
    int probField = 1 + (p_keyword != NULL) + (units != NULL) + 1; // the last +1, because add processorID field berfore distribution
	
    while (p_file.getLine())
    {
        if (p_keyword)
        {
            p_file.getField (str, SMALL);
            if (stricmp (str, p_keyword))
                continue;
        }
		
		// read flt_con
        aConst = new FlightConstraint;
		aConst->SetAirportDB( _pInTerm->m_pAirportDB );
        aConst->readConstraint (p_file);
		// read procID
		procID.readProcessorID( p_file );
		// read unit if need
        if (units)
            p_file.getField (str, SMALL);
		// read distribution
        p_file.setToField (probField);
        aProbDist = GetTerminalRelateProbDistribution (p_file,_pInTerm);
		// init entry
        pEntry = new ConstraintWithProcIDEntry;
        pEntry->initialize (aConst, aProbDist, procID );
        sortedList.addItem (pEntry);
    }
	// copy temp sorted item to database
    for (int i = 0; i < sortedList.getCount(); i++)
        addEntry (sortedList.getItem (i));
}


void FlightConWithProcIDDatabase::readObsoleteDatabase( ArctermFile& p_file, const char *p_keyword, InputTerminal* _pInTerm )
{
	clear();
    p_file.reset();
    p_file.skipLines (3);
	
    SortedContainer<ConstraintWithProcIDEntry> sortedList;
    sortedList.ownsElements (0);
	
	char str[64];
    FlightConstraint *aConst;
    ProbabilityDistribution *aProbDist;
    ConstraintWithProcIDEntry *pEntry;
    ProcessorID id;

	id.SetStrDict( _pInTerm->inStrDict );
    int probField = 1 + (p_keyword != NULL) + (units != NULL);
	
    while (p_file.getLine())
    {
        if (p_keyword)
        {
            p_file.getField (str, SMALL);
            if (stricmp (str, p_keyword))
                continue;
        }
		// read flt_con
        aConst = new FlightConstraint;
		aConst->SetAirportDB( _pInTerm->m_pAirportDB );
        aConst->readConstraint (p_file);
		// need not to read processor id
		
		// read unit if need
        if (units)
            p_file.getField (str, SMALL);
		// read distribution
        p_file.setToField (probField);
        aProbDist = GetTerminalRelateProbDistribution (p_file,_pInTerm);
		
        pEntry = new ConstraintWithProcIDEntry;
        pEntry->initialize (aConst, aProbDist,id);
        sortedList.addItem (pEntry);
    }
	
    for (int i = 0; i < sortedList.getCount(); i++)
        addEntry (sortedList.getItem (i));
}


void FlightConWithProcIDDatabase::readErrorDatabase( ArctermFile& p_file, const char *p_keyword, InputTerminal* _pInTerm )
{
	clear();
    p_file.reset();
    p_file.skipLines (3);
	
    SortedContainer<ConstraintWithProcIDEntry> sortedList;
    sortedList.ownsElements (0);
	
	char str[64];
    FlightConstraint *aConst;
    ProbabilityDistribution *aProbDist;
    ConstraintWithProcIDEntry *pEntry;
    ProcessorID id;

	id.SetStrDict( _pInTerm->inStrDict );
    int probField = 1 + (p_keyword != NULL) + (units != NULL);
	
    while (p_file.getLine())
    {
        if (p_keyword)
        {
            p_file.getField (str, SMALL);
            if (stricmp (str, p_keyword))
                continue;
        }
		
		// read flt_con
        aConst = new FlightConstraint;
		aConst->SetAirportDB( _pInTerm->m_pAirportDB );
        aConst->readConstraint (p_file);
		// need not to read processor id

		// read unit if need
        if (units)
            p_file.getField (str, SMALL);
		// read distribution
        p_file.setToField (probField);
        aProbDist = GetTerminalRelateProbDistribution (p_file,_pInTerm);
		// read error line
		p_file.getLine();
		
        pEntry = new ConstraintWithProcIDEntry;
        pEntry->initialize (aConst, aProbDist, id );
        sortedList.addItem (pEntry);
    }
	
    for (int i = 0; i < sortedList.getCount(); i++)
        addEntry (sortedList.getItem (i));
}

void FlightConWithProcIDDatabase::writeDatabase (ArctermFile& p_file, const char *p_keyword, InputTerminal* _pInTerm) const
{
	FlightConstraint *fltConstraint;
    ConstraintWithProcIDEntry *pEntry;
    const ProbabilityDistribution *aProbDist;
	ProcessorID procID;

    int entryCount = getCount();
    for (int i = 0; i < entryCount; i++)
    {
        pEntry = (ConstraintWithProcIDEntry*)getItem(i);
		// write key
        if (p_keyword)
            p_file.writeField (p_keyword);
		// write flt_con
        fltConstraint = (FlightConstraint *) pEntry->getConstraint();
		fltConstraint->SetAirportDB( _pInTerm->m_pAirportDB );
        fltConstraint->writeConstraint (p_file);
		// write procID
		procID = pEntry->obtainProcID();
		procID.SetStrDict( _pInTerm->inStrDict );
		procID.writeProcessorID( p_file );
		// write unit if need
        if (units)
            p_file.writeField (units);
		// write distribution
        aProbDist = pEntry->getValue();
        aProbDist->writeDistribution (p_file);
        p_file.writeLine();
    }
}

void FlightConWithProcIDDatabase::resortDatebase()
{
	int i;
	// the temp sort list
    SortedContainer<ConstraintWithProcIDEntry> sortedList;
    sortedList.ownsElements (0);
	
	// add all item to sorted list 
	for( i=0; i<getCount(); i++ )
	{
		ConstraintWithProcIDEntry *pEntry = (ConstraintWithProcIDEntry*) getItem(i);
		sortedList.addItem( pEntry );
	}
	// clear the database,not delete item
	while( getCount()>0 )
	{
		removeItem(0);	//no delete
	}
	
	// copy sorted list to data base
	for (i = 0; i < sortedList.getCount(); i++)
        addEntry(sortedList.getItem (i));
}


/*************************************************************************
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
*************************************************************************/
CMobileConWithProcIDDatabase::CMobileConWithProcIDDatabase()
{
}

CMobileConWithProcIDDatabase::~CMobileConWithProcIDDatabase()
{
}


//////////////////////////////////////////////////////////////////////////
// find first entry that matches paxType
// returns NULL if no constraint fits the paxType
const ProbabilityDistribution* CMobileConWithProcIDDatabase::lookup_with_procid (const CMobileElemConstraint& _type, const ProcessorID& _id ) const
{
	int entryCount = getCount();
    for (int i = 0; i < entryCount; i++)
    {
		ConstraintWithProcIDEntry* pEntry = (ConstraintWithProcIDEntry*) getItem(i);
        const CMobileElemConstraint *paxConstraint = (const CMobileElemConstraint *)(pEntry->getConstraint());
		const ProcessorID& procID = pEntry->getProcID();
		
        if ( paxConstraint->fits (_type) && procID.idFits(_id)  )
            return pEntry->getValue();
    }
    return NULL;
}

// text based file I/O
void CMobileConWithProcIDDatabase::readDatabase( ArctermFile& p_file, const char *p_keyword, InputTerminal* _pInTerm )
{
	clear();
    p_file.reset();
    p_file.skipLines (3);
	
	// the temp sort list
    SortedContainer<ConstraintWithProcIDEntry> sortedList;
    sortedList.ownsElements (0);
	
	char str[64];
    CMobileElemConstraint *aConst;
    ProbabilityDistribution *aProbDist;
    ConstraintWithProcIDEntry *pEntry;
	ProcessorID procID;
    
	procID.SetStrDict( _pInTerm->inStrDict );
    int probField = 1 + (p_keyword != NULL) + (units != NULL) + 1; // the last +1, because add processorID field berfore distribution
	
    while (p_file.getLine())
    {
        if (p_keyword)
        {
            p_file.getField (str, SMALL);
            if (stricmp (str, p_keyword))
                continue;
        }
		
		// read flt_con
        aConst = new CMobileElemConstraint(_pInTerm);
		aConst->SetInputTerminal( _pInTerm );
        aConst->readConstraint (p_file);
		// read procID
		procID.readProcessorID( p_file );
		// read unit if need
        if (units)
            p_file.getField (str, SMALL);
		// read distribution
        p_file.setToField (probField);
        aProbDist = GetTerminalRelateProbDistribution (p_file,_pInTerm);
		// init entry
        pEntry = new ConstraintWithProcIDEntry;
        pEntry->initialize (aConst, aProbDist, procID );
        sortedList.addItem (pEntry);
    }
	// copy temp sorted item to database
    for (int i = 0; i < sortedList.getCount(); i++)
        addEntry (sortedList.getItem (i));
}

void CMobileConWithProcIDDatabase::readObsoleteDatabase( ArctermFile& p_file, const char *p_keyword, InputTerminal* _pInTerm )
{
	clear();
    p_file.reset();
    p_file.skipLines (3);
	
    SortedContainer<ConstraintWithProcIDEntry> sortedList;
    sortedList.ownsElements (0);
	
	char str[64];
    CMobileElemConstraint *paxConst;
    ProbabilityDistribution *aProbDist;
    ConstraintWithProcIDEntry *pEntry;
    ProcessorID defaultid;
	
	defaultid.SetStrDict( _pInTerm->inStrDict );
    int probField = 1 + (p_keyword != NULL) + (units != NULL);
	
    while (p_file.getLine())
    {
        if (p_keyword)
        {
            p_file.getField (str, SMALL);
            if (stricmp (str, p_keyword))
                continue;
        }
		// read flt_con
        paxConst = new CMobileElemConstraint(_pInTerm);
		paxConst->SetInputTerminal( _pInTerm );
        paxConst->readConstraint (p_file);
		// need not to read processor id
		
		// read unit if need
        if (units)
            p_file.getField (str, SMALL);
		// read distribution
        p_file.setToField (probField);
        aProbDist = GetTerminalRelateProbDistribution (p_file,_pInTerm);
		
        pEntry = new ConstraintWithProcIDEntry;
        pEntry->initialize (paxConst, aProbDist,defaultid);
        sortedList.addItem (pEntry);
    }
	
    for (int i = 0; i < sortedList.getCount(); i++)
        addEntry (sortedList.getItem (i));
}

void CMobileConWithProcIDDatabase::readErrorDatabase( ArctermFile& p_file, const char *p_keyword, InputTerminal* _pInTerm )
{
	readObsoleteDatabase(p_file, p_keyword, _pInTerm );
}

void CMobileConWithProcIDDatabase::writeDatabase( ArctermFile& p_file, const char *p_keyword, InputTerminal* _pInTerm) const
{
	CMobileElemConstraint *paxConstraint;
    ConstraintWithProcIDEntry *pEntry;
    const ProbabilityDistribution *aProbDist;
	ProcessorID procID;
	
    int entryCount = getCount();
    for (int i = 0; i < entryCount; i++)
    {
        pEntry = (ConstraintWithProcIDEntry*)getItem(i);
		// write key
        if (p_keyword)
            p_file.writeField (p_keyword);
		// write flt_con
        paxConstraint = (CMobileElemConstraint*) pEntry->getConstraint();
		paxConstraint->SetInputTerminal( _pInTerm );
        paxConstraint->writeConstraint (p_file);
		// write procID
		procID = pEntry->obtainProcID();
		procID.SetStrDict( _pInTerm->inStrDict );
		procID.writeProcessorID( p_file );
		// write unit if need
        if (units)
            p_file.writeField (units);
		// write distribution
        aProbDist = pEntry->getValue();
        aProbDist->writeDistribution (p_file);
        p_file.writeLine();
    }
}

void CMobileConWithProcIDDatabase::resortDatebase()
{
	int i;
	// the temp sort list
    SortedContainer<ConstraintWithProcIDEntry> sortedList;
    sortedList.ownsElements (0);
	
	// add all item to sorted list 
	for( i=0; i<getCount(); i++ )
	{
		ConstraintWithProcIDEntry *pEntry = (ConstraintWithProcIDEntry*) getItem(i);
		sortedList.addItem( pEntry );
	}
	// clear the database,not delete item
	while( getCount()>0 )
	{
		removeItem(0);	//not delete the pointer
	}
	
	// copy sorted list to data base
	for (i = 0; i < sortedList.getCount(); i++)
        addEntry(sortedList.getItem (i));

}
