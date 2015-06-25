#include "StdAfx.h"
#include "BoardingCallFlightTypeDatabase.h"

BoardingCallFlightTypeEntry::BoardingCallFlightTypeEntry(void)
{
}


BoardingCallFlightTypeEntry::~BoardingCallFlightTypeEntry(void)
{
}

void BoardingCallFlightTypeEntry::AddStand(char* strProc, InputTerminal* _pInTerm)
{
	ProcessorID* pStandProcID = new ProcessorID();
	pStandProcID->SetStrDict(_pInTerm->inStrDict);
	if(strProc)
	{
		pStandProcID->setID(strProc);
	}
	else
	{
		pStandProcID->setID("All Stand"); /* Set 'Stand group': All Stand */
	}
	m_standDB.addEntry(*pStandProcID);
	BoardingCallStandEntry* pStandEntry = (BoardingCallStandEntry*)m_standDB.FindEntry(*pStandProcID);
	pStandEntry->AddPax(NULL);/*add a default mobile element constraint entry.*/
}

void BoardingCallFlightTypeEntry::AddStandFromOld( ConstraintWithProcIDEntry* pConstEntry, InputTerminal* _pInTerm )
{
	ProcessorID* pStandProcID = new ProcessorID(pConstEntry->getProcID());
	m_standDB.addEntry(*pStandProcID);
	BoardingCallStandEntry* pStandEntry = (BoardingCallStandEntry*)m_standDB.FindEntry(*pStandProcID);
	pStandEntry->AddPaxFromOld(pConstEntry);
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



void BoardingCallFlightTypeDatabase::AddFlight(FlightConstraint* pInFlightConst, InputTerminal* _pInTerm)
{
	FlightConstraint* pFlightConstraint = new FlightConstraint();
	if(pInFlightConst)
	{
		*pFlightConstraint = *pInFlightConst;
	}
	else
	{
		pFlightConstraint->initDefault(); /* Set 'Flight Type': DEFAULT */
	}
	BoardingCallFlightTypeEntry* flightEntry = new BoardingCallFlightTypeEntry();
	flightEntry->initialize(pFlightConstraint, NULL);
	flightEntry->AddStand(NULL, _pInTerm);
	addEntry(flightEntry, true);/* Replace if exists. */
}

void BoardingCallFlightTypeDatabase::AddFlightFromOld(FlightConWithProcIDDatabase* pOldConWithProcIDDB, InputTerminal* _pInTerm)
{
	int count = pOldConWithProcIDDB->getCount();
	for(int i=0; i<count; i++)
	{
		ConstraintWithProcIDEntry* pConstWithProcIDEntry = (ConstraintWithProcIDEntry*)pOldConWithProcIDDB->getItem(i);
		FlightConstraint* pFlightConstraint = new FlightConstraint();
		*pFlightConstraint = *((FlightConstraint*)pConstWithProcIDEntry->getConstraint());
		BoardingCallFlightTypeEntry* flightEntry = new BoardingCallFlightTypeEntry();
		flightEntry->initialize(pFlightConstraint, NULL);
		flightEntry->AddStandFromOld(pConstWithProcIDEntry, _pInTerm);
		addEntry(flightEntry, true);/* Replace if exists. */
	}
}

void BoardingCallFlightTypeDatabase::readDatabase( ArctermFile& p_file )
{
	clear();
	p_file.getLine();
	p_file.skipField(1);
	int standCount;
	p_file.getInteger(standCount);
	for(int i=0; i<standCount; i++)
	{
		BoardingCallFlightTypeEntry* pFlightEntry = (BoardingCallFlightTypeEntry*)getItem(i);
		pFlightEntry->GetStandDatabase()->read
	}
}

void BoardingCallFlightTypeDatabase::writeDatabase(ArctermFile& p_file, char* szLabel, InputTerminal* _pInTerm)
{
	int flightCount = getCount();
	for(int i=0; i<flightCount; i++)
	{
		CString strFlightType;
		BoardingCallFlightTypeEntry* pFlightEntry = (BoardingCallFlightTypeEntry*)getItem(i);
		FlightConstraint* pFlightConst = (FlightConstraint*)(pFlightEntry->getConstraint());
		pFlightConst->getFullID(strFlightType.GetBuffer(256));
		strFlightType.ReleaseBuffer();
		p_file.writeField(strFlightType);
		int standCount = pFlightEntry->GetStandCount();
		p_file.writeInt(standCount);
		p_file.writeLine();
		
		pFlightEntry->GetStandDatabase()->writeDatabase(p_file, _pInTerm);
	}
}
