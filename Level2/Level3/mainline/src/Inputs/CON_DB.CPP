#include "stdafx.h"
#include <string.h>
#include "inputs\con_db.h"
#include "../Common/ProbabilityDistribution.h"


ConstraintEntry& ConstraintEntry::operator = (const ConstraintEntry& p_entry)
{
	 clear();
	 constraint = p_entry.constraint;
	 value = p_entry.value;
	 return *this;
}

void ConstraintEntry::setValue (ProbabilityDistribution *p_prob)
{
// deallocation error in prob dist classes
//    if (value)
//        delete value;
	 value = p_prob;
}

// deallocates existing members
void ConstraintEntry::clear (void)
{
// deallocation error in prob dist classes
//    if (value)
//        delete value;
	 value = NULL;
	 if (constraint)
		  delete constraint;
	 constraint = NULL;
}
void ConstraintEntry::DeleteValue()
{
	if(value)
		delete value;
	value=NULL;
}

// adds newEntry to the entryList
// updates entry's probability if already in database
void ConstraintDatabase::addEntry (ConstraintEntry *p_entry, bool _bDeleteIfExist )
{
	if( _bDeleteIfExist )
    {
		int index = findItem (p_entry);
		if (index != INT_MAX)
			deleteItem (index);
    }
	addItem (p_entry);
}

void ConstraintDatabase::clear( void )
{
	ConstraintEntry *entry;
    while (getCount())
    {
        entry = getItem (0);
        removeItem (0);
        delete entry;
    }
}

//Jan98
ConstraintDatabase::ConstraintDatabase ():SortedContainer<ConstraintEntry> (8, 0, 8)
{
	ownsElements(1);
	units = NULL;
}
























