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

void BoardingCallFlightTypeDatabase::DoDataCompatibleWith260AndEarlier( FlightConWithProcIDDatabase* pOldStructor )
{

}
