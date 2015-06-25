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
		CString strStand;
		p_file.getField(strStand.GetBuffer(64), 64);
		strStand.ReleaseBuffer();
		AddStand(strStand.GetBuffer(), _pInTerm);

		int paxCount;
		p_file.getInteger(paxCount);
		((BoardingCallStandEntry*)getItem(i))->GetPaxTypeDatabase()->readDatabase(p_file, _pInTerm, paxCount);
	}
}

void BoardingCallStandDatabase::writeDatabase( ArctermFile& p_file, InputTerminal* _pInTerm )
{
	int standCount = getCount();
	for(int i=0; i<standCount; i++)
	{
		BoardingCallStandEntry* pStandEntry = (BoardingCallStandEntry*)getItem(i);
		pStandEntry->getID()->writeProcessorID(p_file);

		int paxCount = pStandEntry->GetPaxCount();
		p_file.writeInt(paxCount);
		p_file.writeLine();

		pStandEntry->GetPaxTypeDatabase()->writeDatabase(p_file, _pInTerm );
	}
}

void BoardingCallStandDatabase::AddStand( char* strProc, InputTerminal* _pInTerm )
{
	ProcessorID* pStandProcID = new ProcessorID();
	pStandProcID->SetStrDict(_pInTerm->inStrDict);
	if(strProc)
	{
		pStandProcID->setID(strProc);
	}
	else
	{
		pStandProcID->init(); /* Set 'Stand group': All Stand */
	}
	addEntry(*pStandProcID);
	BoardingCallStandEntry* pStandEntry = (BoardingCallStandEntry*)FindEntry(*pStandProcID);
	pStandEntry->GetPaxTypeDatabase()->AddPax(NULL); /*add a default mobile element constraint entry.*/
}

void BoardingCallStandDatabase::AddStandFromOld( ConstraintWithProcIDEntry* pConstEntry, InputTerminal* _pInTerm )
{
	ProcessorID* pStandProcID = new ProcessorID(pConstEntry->getProcID());
	addEntry(*pStandProcID);
	BoardingCallStandEntry* pStandEntry = (BoardingCallStandEntry*)FindEntry(*pStandProcID);
	pStandEntry->GetPaxTypeDatabase()->AddPaxFromOld(pConstEntry);
}
