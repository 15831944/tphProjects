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

void BoardingCallStandDatabase::readDatabase( ArctermFile& p_file, InputTerminal* _pInTerm, int standCount)
{
	clear();
	for(int i=0; i<standCount; i++)
	{
		p_file.getLine();
		ProcessorID procID;
		procID.SetStrDict(_pInTerm->inStrDict);
		procID.readProcessorID(p_file);
		AddStand(&procID, _pInTerm);

		int paxCount;
		p_file.getInteger(paxCount);
		((BoardingCallStandEntry*)getItem(i))->GetPaxTypeDatabase()->readDatabase(p_file, _pInTerm, paxCount);
	}
}

void BoardingCallStandDatabase::writeDatabase( ArctermFile& p_file)
{
	int standCount = getCount();
	for(int i=0; i<standCount; i++)
	{
		BoardingCallStandEntry* pStandEntry = (BoardingCallStandEntry*)getItem(i);
		pStandEntry->getID()->writeProcessorID(p_file);

		int paxCount = pStandEntry->GetPaxCount();
		p_file.writeInt(paxCount);
		p_file.writeLine();

		pStandEntry->GetPaxTypeDatabase()->writeDatabase(p_file);
	}
}

void BoardingCallStandDatabase::AddStand(ProcessorID* pProcID, InputTerminal* _pInTerm )
{
	ProcessorID* pStandProcID = new ProcessorID();
	if(pProcID)
	{
		*pStandProcID = *pProcID;
	}
	else
	{
		pStandProcID->SetStrDict(_pInTerm->inStrDict);
		pStandProcID->init(); /* Set 'Stand group': All Stand */
	}
	addEntry(*pStandProcID);
	BoardingCallStandEntry* pStandEntry = (BoardingCallStandEntry*)FindEntry(*pStandProcID);
	pStandEntry->GetPaxTypeDatabase()->AddPaxType(NULL, _pInTerm); /*add a 'DEFAULT' mobile element constraint entry.*/
}

// For version 2.6 or older.
void BoardingCallStandDatabase::AddStandFor260OrOlder( ConstraintWithProcIDEntry* pConstEntry, InputTerminal* _pInTerm )
{
	if(findEntry(pConstEntry->getProcID()) == INT_MAX)
	{
		ProcessorID* pProcID = new ProcessorID(pConstEntry->getProcID());
		addEntry(*pProcID);
		BoardingCallStandEntry* pStandEntry = (BoardingCallStandEntry*)FindEntry(*pProcID);
		pStandEntry->GetPaxTypeDatabase()->AddPaxTypeFor260AndOlder(pConstEntry, _pInTerm);
	}
}
