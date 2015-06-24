#include "stdafx.h"

#include <CommonData/procid.h>
#include "inputs\procdb.h"
#include "in_term.h"



/*****
*
*   Processor Database
*
*****/

// adds a ProcessorDataElement to the list if it does not already exist
void ProcessorDatabase::addEntry (const ProcessorID& pID)
{
    ProcessorDataElement* pEntry = FindEntry (pID);
	if (!pEntry)
        addItem (new ProcessorDataElement (pID));
}

// performs a binary search for an entry belonging to id
int ProcessorDatabase::findEntry (const ProcessorID& id) const
{
    ProcessorDataElement element(id);
    ProcessorDataElement *entry;

    int entryCount = getCount();
    for (int i = 0; i < entryCount; i++)
    {
        entry = getItem(i);
        if (*entry == element)
            return i;
    }
    return INT_MAX;

//    return findItem (&element);
}

// returns ProcessorDataElement that corresponds to passed procID
ProcessorDataElement *ProcessorDatabase::FindEntry (const ProcessorID& procID) const
{
    int index = findEntry (procID);
    return (index == INT_MAX)? NULL: getItem(index);
}

// scans backwards in the list of processor entries for an entry
// belonging to curEntry's super-group
// since the entries are sorted by processor ID, any entry's ancestor
// will be before it in the list
ProcessorDataElement *ProcessorDatabase::getAncestor (int start,
    const ProcessorDataElement& curEntry) const
{
    ProcessorDataElement *entry;

    for (int i = start-1; i >= 0; i--)
    {
        entry = getItem(i);

        if (!curEntry.procID.hasCommonGroup (entry->procID))
            break;

        if (curEntry.procID.isChildOf (entry->procID))
            return entry;
    }
	return NULL;
}

// set ancestor for each ProcessorDataElement
void ProcessorDatabase::buildHierarchy (void) const
{
    ProcessorDataElement *curEntry;
    int entryCount = getCount();
    for (int i = 0; i < entryCount; i++)
    {
        curEntry = getItem(i);
		curEntry->closestAncestor =	getAncestor (i, *curEntry);
    }
}  

// get the closest entry in hierachy for id
ProcessorDataElement *ProcessorDatabase::getEntryPoint (const ProcessorID& id) const
{
	ProcessorID key = id;
    ProcessorDataElement *entryPoint;

    while(1)
    {
//		TRACE("\nProcessorID: %s", key.GetIDString());
		entryPoint = FindEntry (key);

        if (entryPoint)
            return entryPoint;

        // if not found, search its next ancestor
        if (!key.getSuperGroup())
			return NULL;
    }
}
void ProcessorDatabase::getEntryPoint (const ProcessorID& id, std::vector<ProcessorDataElement *>& _vResultData) const
{
	ProcessorID key = id;
    ProcessorDataElement *entryPoint;
	
    while(1)
    {
//		TRACE("\nProcessorID: %s", key.GetIDString());
		entryPoint = FindEntry (key);
		
        if (entryPoint)
            _vResultData.push_back(entryPoint);
		
        // if not found, search its next ancestor
        if (!key.getSuperGroup())
			break;
    }
	
}

bool ProcessorDatabase::getUsedProInfo(const ProcessorID& _id,InputTerminal *_pTerm,std::vector<CString>& _strVec)
{
	bool nRet = false;
	ProcessorDataElement *element;
    for (int i = getCount() - 1; i >= 0; i--)
    {
        element = getItem(i);
        if (_id.idFits (*element->getID()))
		{
			CString str = element->getID()->GetIDString();
			str = " : used in base";
			_strVec.push_back(str);
            nRet = true;
		}
		else if(element->getID()->idFits(_id) && !element->getID()->isBlank())
		{
			if(!_pTerm->HasBrother(_id,*element->getID()))
			{
				CString str = element->getID()->GetIDString();
				str = " : used in base";
				_strVec.push_back(str);
				nRet = true;
			}
		}
    }
	return nRet;
}
// removes and deallocates all entries that fit passed procID
void ProcessorDatabase::removeAllEntries (const ProcessorID& id,InputTerminal*_term)
{
    ProcessorDataElement *element;
    for (int i = getCount() - 1; i >= 0; i--)
    {
        element = getItem(i);
        if (id.idFits (*(element->getID())))
            deleteItem(i);
		else if(element->getID()->idFits(id) && !element->getID()->isBlank())
		{
			if(!_term->HasBrother(id,*element->getID()))
				deleteItem(i);
		}
    }
}

// finds all entries that fit passed oldID and changes them to newID
void ProcessorDatabase::replaceAllEntries (const ProcessorID& oldID,
    const ProcessorID& newID,InputTerminal*_term)
{
    ProcessorDataElement *element;
    for (int i = getCount() - 1; i >= 0; i--)
    {
        element = getItem(i);
        if (oldID.idFits (*(element->getID())))
            element->changeID (newID);
		else if(element->getID()->idFits(oldID) && (!element->getID()->isBlank()))
		{
			if(!element->getID()->idFits(newID))
			{
				if(!_term->HasBrother(oldID,*element->getID()))
					element->changeID (newID);
			}
		}
    }
}


// found the best fit processor in this, return -1 for no result.
int ProcessorDatabase::findBestMatch(const ProcessorID &_id)
{

	int nCount = getCount();
	for( int i=nCount-1; i>=0; i-- )
	{
		ProcessorDataElement* pEntry = getItem( i );
		if( pEntry->getID()->idFits( _id ) )
		{
			return i;
		}
	}
    return -1;
}
