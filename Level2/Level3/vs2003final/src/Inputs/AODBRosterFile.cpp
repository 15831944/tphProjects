#include "StdAfx.h"
#include ".\aodbrosterfile.h"
#include "AODBConvertParameter.h"
#include "engine\proclist.h"
#include "inputs\assign.h"
#include "inputs\assigndb.h"
#include "inputs\in_term.h"


using namespace AODB;


//RESOURCES	START_TIME	END_TIME	AIRLINE	FLIGHT_NUMBER
///
AODB::RosterItem::RosterItem()
{

}

AODB::RosterItem::~RosterItem()
{

}

void AODB::RosterItem::ReadData( CSVFile& csvFile )
{
	m_proc.GetValue(csvFile);

	m_startTime.GetValue(csvFile);

	m_endTime.GetValue(csvFile);

	m_strAirline.GetValue(csvFile);
	m_strFltNumber.GetValue(csvFile);

}

bool AODB::RosterItem::valid()const
{
	if (!m_proc.IsValid() || !m_startTime.IsValid() || !m_endTime.IsValid() 
		||!m_strAirline.IsValid() || !m_strFltNumber.IsValid())
	{
		return false;
	}
	return true;
}


AODB::MapField* AODB::RosterItem::GetMapFile(RosterItemType type)
{
	switch (type)
	{
	case PROC_TYPE:
		{
			return &(m_proc);
		}
	case START_TIME_TYPE:
		{
			return &(m_startTime);
		}
	case END_TIME_TYPE:
		{
			return &(m_endTime);
		}
	case AIRLINE_TYPE:
		{
			return &(m_strAirline);
		}
	case FLIGHTNUM_TYPE:
		{
			return &(m_strFltNumber);
		}
	}
	return NULL;
}

COleDateTime AODB::RosterItem::GetEearistDate()
{
	if(m_startTime.getDateTime().GetStatus() == COleDateTime::valid)
	{
		if(m_endTime.getDateTime().GetStatus() == COleDateTime::valid)
		{
			if(m_startTime.getDateTime() > m_endTime.getDateTime())
				return m_endTime.getDateTime();
		}

		return m_startTime.getDateTime();

	}
	else
	{
		if(m_endTime.getDateTime().GetStatus() == COleDateTime::valid)
			return m_endTime.getDateTime();

		return m_startTime.getDateTime();
	}

}

void AODB::RosterItem::Convert( ConvertParameter& pParameter )
{

	//time convert

	m_proc.Convert(pParameter);
	m_strAirline.Convert(pParameter);
	m_strFltNumber.Convert(pParameter);
	m_startTime.Convert(pParameter);
	m_endTime.Convert(pParameter);


	bool bValid = true;
	if(!m_proc.IsValid())
		bValid = false;

	if(!m_startTime.IsValid())
		bValid = false;

	if(!m_endTime.IsValid())
		bValid = false;

	if(!m_strAirline.IsValid())
		bValid = false;

	if(!m_strFltNumber.IsValid())
		bValid = false;

	if(bValid)
	{//convert to roster


		//ElapsedTime openTime = m_startTime.getElapsedTime();
		//ElapsedTime closeTime = m_endTime.getElapsedTime();


		//CMobileElemConstraint mob(pParameter.pTerminal);
		//mob.setAirline(m_strAirline.GetMapValue());
		//mob.setFlightID(m_strFltNumber.GetMapValue());


		//// create assign event
		//ProcessorRoster* pProcAssn = new ProcessorRoster( pParameter.pTerminal );
		//pProcAssn->setFlag(1 );
		//pProcAssn->setOpenTime( openTime );
		//pProcAssn->setCloseTime( closeTime );
		//CMultiMobConstraint multiMob;
		//multiMob.addConstraint( mob );
		//pProcAssn->setAssignment(multiMob );

		//// save to assignment dataset
		//ProcessorID id = m_proc.getProcID();

	/*	int nIdx = pParameter.pTerminal->procAssignDB->findEntry( id );
		if( nIdx == INT_MAX )
		{
			pParameter.pTerminal->procAssignDB->addEntry( id );
			nIdx = pParameter.pTerminal->procAssignDB->findEntry( id );
			assert( nIdx != INT_MAX  );
		}
		ProcessorRosterSchedule* pSchd = pParameter.pTerminal->procAssignDB->getDatabase( nIdx );
		UnmergedAssignments m_unMergedAssgn;
		m_unMergedAssgn.split( pSchd );
		m_unMergedAssgn.addItem( pProcAssn );
		m_unMergedAssgn.merge( pSchd );*/

	




	}
	m_bSuccess = bValid;

}

bool AODB::RosterItem::ImportRoster( InputTerminal *pTerminal )
{
	ElapsedTime openTime = m_startTime.getElapsedTime();
	ElapsedTime closeTime = m_endTime.getElapsedTime();


	CMobileElemConstraint mob(pTerminal);
	mob.setAirline(m_strAirline.GetMapValue());
	mob.setFlightID(m_strFltNumber.GetMapValue());


	// create assign event
	ProcessorRoster* pProcAssn = new ProcessorRoster( pTerminal );
	pProcAssn->setFlag(1 );
	pProcAssn->IsDaily(FALSE);
	pProcAssn->setOpenTime( openTime );
	pProcAssn->setCloseTime( closeTime );
	CMultiMobConstraint multiMob;
	multiMob.addConstraint( mob );
	pProcAssn->setAssignment(multiMob );

	// save to assignment dataset

	ProcessorID id;
	id.SetStrDict(pTerminal->inStrDict);
	if(!id.isValidID(m_proc.GetMapValue()))
		return true;

	id.setID(m_proc.GetMapValue().GetBuffer());
	if(id.isBlank())
		return true;

	GroupIndex groupIdx = pTerminal->procList->getGroupIndex( id );	
	if( groupIdx.start < 0 )
	{
		return true;
	}

	int nIdx = pTerminal->procAssignDB->findEntry( id );
	if( nIdx == INT_MAX )
	{
	pTerminal->procAssignDB->addEntry( id );
	nIdx = pTerminal->procAssignDB->findEntry( id );
	assert( nIdx != INT_MAX  );
	}
	ProcessorRosterSchedule* pSchd = pTerminal->procAssignDB->getDatabase( nIdx );
	UnmergedAssignments m_unMergedAssgn;
	m_unMergedAssgn.split( pSchd );

	//check the item is existed or not
	bool bFindEqual = false;

	int nCount = m_unMergedAssgn.getCount();
	for (int nItem = 0; nItem < nCount; ++ nItem)
	{
		ProcessorRoster *pRoster = m_unMergedAssgn.getItem(nItem);
		if(pRoster&& pRoster->isEqual(*pProcAssn))
			bFindEqual = true;
	}
	if(!bFindEqual)
		m_unMergedAssgn.addItem( pProcAssn );
	
	m_unMergedAssgn.merge( pSchd );

	return true;
}







//////////////////////////////////////////////////////////////////////////

RosterFile::RosterFile(void)
{

}

RosterFile::~RosterFile(void)
{
}


void AODB::RosterFile::ReadData( CSVFile& csvFile )
{
	Clear();
	while(csvFile.getLine())
	{
		if(csvFile.isBlank())
			break;

		RosterItem *pItem = new RosterItem;
		pItem->ReadData(csvFile);

		AddData(pItem);
	}

}

void AODB::RosterFile::WriteData( CSVFile& csvFile )
{

}


COleDateTime AODB::RosterFile::GetStartDate(COleDateTime earliestDate)
{
	int nCount = GetCount();

	for (int nItem = 0; nItem < nCount; ++ nItem)
	{
		RosterItem *pItem = GetItem(nItem);

		if(pItem)
		{
			COleDateTime oleDate =  pItem ->GetEearistDate();
			if(oleDate.GetStatus() == COleDateTime::valid)
			{
				if(oleDate < earliestDate)
					earliestDate = oleDate;
			}
		}

	}
	return earliestDate;
}

RosterItem * AODB::RosterFile::GetItem( int nIndex )
{
	return (RosterItem *) GetData(nIndex);
}

void AODB::RosterFile::Convert( ConvertParameter& pParameter )
{
	int nCount = GetCount();

	for (int nItem = 0; nItem < nCount; ++ nItem)
	{
		RosterItem *pItem = GetItem(nItem);

		if(pItem)
		{
			pItem->Convert(pParameter);
		}
	}
}

bool AODB::RosterFile::ImportRoster( InputTerminal *pTerminal )
{

	if(!IsReady())
		return false;

	int nCount = GetCount();

	for (int nItem = 0; nItem < nCount; ++ nItem)
	{
		RosterItem *pItem = GetItem(nItem);

		if(pItem)
		{
			pItem->ImportRoster(pTerminal);
		}
	}

	return true;
}