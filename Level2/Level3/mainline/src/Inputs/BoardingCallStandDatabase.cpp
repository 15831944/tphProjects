#include "StdAfx.h"
#include "BoardingCallStandDatabase.h"

BoardingCallStandEntry::BoardingCallStandEntry(const ProcessorID& id)
	: ProcessorDataElement (id)
{
}


BoardingCallStandEntry::~BoardingCallStandEntry(void)
{
}

BoardingCallStandDatabase::BoardingCallStandDatabase(void)
{
}


BoardingCallStandDatabase::~BoardingCallStandDatabase(void)
{
}

// adds a ProcessorDataElement to the list if it does not already exist
void BoardingCallStandDatabase::addEntry (const ProcessorID& pID)
{
	BoardingCallStandEntry *pStandEntry = (BoardingCallStandEntry *)FindEntry (pID);
	if (pStandEntry)
		deleteItem(pStandEntry);
	addItem((new BoardingCallStandEntry(pID)));
}

void BoardingCallStandDatabase::deleteItem( BoardingCallStandEntry* pStandEntry )
{
	removeItem(pStandEntry);
	delete pStandEntry;
}
