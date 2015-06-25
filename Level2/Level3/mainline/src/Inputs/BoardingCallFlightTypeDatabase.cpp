#include "StdAfx.h"
#include "BoardingCallFlightTypeDatabase.h"

BoardingCallFlightTypeEntry::BoardingCallFlightTypeEntry(void)
{
}


BoardingCallFlightTypeEntry::~BoardingCallFlightTypeEntry(void)
{
}

BoardingCallFlightTypeDatabase::BoardingCallFlightTypeDatabase(void)
{
}


BoardingCallFlightTypeDatabase::~BoardingCallFlightTypeDatabase(void)
{
}

void BoardingCallFlightTypeDatabase::deleteItem( ConstraintEntry* pConst )
{
	removeItem(pConst);
	delete pConst;
}



void BoardingCallFlightTypeDatabase::AddFlightType(FlightConstraint* pInFlightConst, InputTerminal* _pInTerm)
{
	FlightConstraint* pFlightConstraint = new FlightConstraint();
	if(pInFlightConst)
	{
		*pFlightConstraint = *pInFlightConst;
	}
	else
	{
		pFlightConstraint->SetFltConstraintMode(ENUM_FLTCNSTR_MODE_DEP); /* Set 'Flight Type': DEPARTING */
		pFlightConstraint->SetAirportDB(_pInTerm->m_pAirportDB);
	}
	BoardingCallFlightTypeEntry* flightEntry = new BoardingCallFlightTypeEntry();
	flightEntry->initialize(pFlightConstraint, NULL);
	flightEntry->GetStandDatabase()->AddStand(NULL, _pInTerm);
	addEntry(flightEntry);
}

// For version 2.6 or older.
void BoardingCallFlightTypeDatabase::AddFlightFor260OrOlder(FlightConWithProcIDDatabase* pOldConWithProcIDDB, InputTerminal* _pInTerm)
{
	int count = pOldConWithProcIDDB->getCount();
	for(int i=0; i<count; i++)
	{
		ConstraintWithProcIDEntry* pConstWithProcIDEntry = (ConstraintWithProcIDEntry*)pOldConWithProcIDDB->getItem(i);
		FlightConstraint* pFlightConstraint = new FlightConstraint();
		*pFlightConstraint = *((FlightConstraint*)pConstWithProcIDEntry->getConstraint());
		if(pFlightConstraint->GetFltConstraintMode() != ENUM_FLTCNSTR_MODE_DEP)
		{
			pFlightConstraint->SetFltConstraintMode(ENUM_FLTCNSTR_MODE_DEP);
		}

		int fltEntryIndex = findItemByConstraint(pFlightConstraint);
		BoardingCallStandDatabase* pStandDB;
		if(fltEntryIndex == INT_MAX)
		{
			BoardingCallFlightTypeEntry* flightEntry = new BoardingCallFlightTypeEntry();
			flightEntry->initialize(pFlightConstraint, NULL);
			pStandDB = flightEntry->GetStandDatabase();
			addEntry(flightEntry, false);
		}
		else
		{
			pStandDB = ((BoardingCallFlightTypeEntry*)getItem(fltEntryIndex))->GetStandDatabase();
		}
		pStandDB->AddStandFor260OrOlder(pConstWithProcIDEntry, _pInTerm);
	}
}

void BoardingCallFlightTypeDatabase::readDatabase(ArctermFile& p_file, InputTerminal* _pInTerm, int fltTypeCount)
{
	clear();
	for(int i=0; i<fltTypeCount; i++)
	{
		p_file.getLine();
		FlightConstraint fltConst;
		fltConst.SetAirportDB( _pInTerm->m_pAirportDB );
		fltConst.readConstraint (p_file);
		AddFlightType(&fltConst, _pInTerm);

		int standCount;
		p_file.getInteger(standCount);
		((BoardingCallFlightTypeEntry*)getItem(i))->GetStandDatabase()->readDatabase(p_file, _pInTerm, standCount);
	}
}

void BoardingCallFlightTypeDatabase::writeDatabase(ArctermFile& p_file)
{
	int flightCount = getCount();
	for(int i=0; i<flightCount; i++)
	{
		BoardingCallFlightTypeEntry* pFlightEntry = (BoardingCallFlightTypeEntry*)getItem(i);
		FlightConstraint* pFlightConst = (FlightConstraint*)(pFlightEntry->getConstraint());
		pFlightConst->writeConstraint(p_file);

		int standCount = pFlightEntry->GetStandCount();
		p_file.writeInt(standCount);
		p_file.writeLine();
		
		pFlightEntry->GetStandDatabase()->writeDatabase(p_file);
	}
}

int BoardingCallFlightTypeDatabase::findItemByConstraint(FlightConstraint* pfltConst)
{
	int count = getCount();
	for(int i=0; i<count; i++)
	{
		if((*getItem(i)->getConstraint()) == *pfltConst)
			return i;
	}
	return INT_MAX;
}
