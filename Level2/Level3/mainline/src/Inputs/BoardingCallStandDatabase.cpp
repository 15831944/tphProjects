#include "StdAfx.h"
#include "BoardingCallStandDatabase.h"

BoardingCallStandEntry::BoardingCallStandEntry(const ProcessorID& id)
	: ProcessorDataElement (id)
{
}


BoardingCallStandEntry::~BoardingCallStandEntry(void)
{
}

void BoardingCallStandEntry::AddPax( CMobileElemConstraint* pInputConst )
{
	CMobileElemConstraint* pMBConst = new CMobileElemConstraint();
	if(pInputConst)
	{
		*pMBConst = *pInputConst;
	}
	else
	{
		pMBConst->SetMobileElementType(enum_MobileElementType_ALL); /* Set 'Passenger Type': DEFAULT */
	}
	BoardingCallPaxTypeEntry* pPaxEntry = new BoardingCallPaxTypeEntry();
	pPaxEntry->initialize(pMBConst, NULL);
	pPaxEntry->InitTriggerDatabase();	/* InitTriggersDatabase(): The 4 initial triggers will be added here. */
    m_paxTypeDB.addEntry(pPaxEntry, true);/* Replace if exists. */
}

void BoardingCallStandEntry::AddPaxFromOld(ConstraintWithProcIDEntry* pConstEntry)
{
	CMobileElemConstraint* pMBConst = new CMobileElemConstraint();
	pMBConst->SetMobileElementType(enum_MobileElementType_ALL); /* Set 'Passenger Type': DEFAULT */
	BoardingCallPaxTypeEntry* pPaxEntry = new BoardingCallPaxTypeEntry();
	pPaxEntry->initialize(pMBConst, NULL);
	pPaxEntry->InitTriggerDBFromOld(pConstEntry);
	m_paxTypeDB.addEntry(pPaxEntry, true);
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

void BoardingCallStandDatabase::writeDatabase( ArctermFile& p_file, InputTerminal* _pInTerm )
{
	int standCount = getCount();
	for(int i=0; i<standCount; i++)
	{
		CString strStand;
		BoardingCallStandEntry* pStandEntry = (BoardingCallStandEntry*)getItem(i);
		pStandEntry->getID()->printID(strStand.GetBuffer(256));
		strStand.ReleaseBuffer();
		p_file.writeField(strStand);
		int paxCount = pStandEntry->GetPaxCount();
		p_file.writeInt(paxCount);
		p_file.writeLine();

		pStandEntry->GetPaxTypeDatabase()->writeDatabase(p_file, _pInTerm );
	}
}
