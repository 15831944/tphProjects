// BagGateAssign.cpp: implementation of the BagGateAssign class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BagGateAssign.h"
#include "inputs\procdist.h"
#include "in_term.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

BagGateEntry::~BagGateEntry ()
{
	Clear();
}

void BagGateEntry::Clear(void)
{
	if( m_probDistEntry )
	{
		delete m_probDistEntry;
		m_probDistEntry= NULL;
	}
}


BagGateAssign::BagGateAssign() : DataSet(BagGateAssignFile),m_bGateHasHigherPriorty( false )
{
}

BagGateAssign::~BagGateAssign()
{		

}

// schedules the first assignment change for each processor group

void BagGateAssign::readData (ArctermFile& p_file)
{
	assert( m_pInTerm );
 
    ProcessorID procID;
	procID.SetStrDict( m_pInTerm->inStrDict );
	

	p_file.getLine();
	int iPriorityFlag = 0;
	p_file.getInteger( iPriorityFlag );
	m_bGateHasHigherPriorty = iPriorityFlag == 1 ? true : false;
	
	int iCount = -1;
	p_file.getInteger( iCount );
	for( int i=0; i< iCount; i++ )
	{
		p_file.getLine();
		procID.readProcessorID( p_file );
		
		p_file.getLine();
		ProcessorDistribution* pDist = new ProcessorDistribution;
		pDist->readDistribution( p_file,m_pInTerm );
		insert( procID, pDist );
	}
    
	buildHierarchy();
}

void BagGateAssign::writeData (ArctermFile& p_file) const
{
    const ProcessorID *procID;
	if( m_bGateHasHigherPriorty )
	{
		p_file.writeInt( 1 );
	}
	else
	{
		p_file.writeInt( 0 );
	}
    int entryCount = getCount();
	p_file.writeInt( entryCount );
	p_file.writeLine();

    for (int i = 0; i < entryCount; i++)
    {
        procID = getProcName(i);
		procID->writeProcessorID( p_file );
		p_file.writeLine();
		BagGateEntry *pEntry = (BagGateEntry *)FindEntry(*procID);
		assert( pEntry );
		pEntry->getProbDist()->writeDistribution( p_file );
		p_file.writeLine();
    }
}

// adds a ProcessorDataElement to the list if it does not already exist
void BagGateAssign::addEntry (const ProcessorID& pID)
{
    BagGateEntry *pEntry = (BagGateEntry *)FindEntry (pID);
	if (!pEntry)
        addItem ((new BagGateEntry (pID)));
}

void BagGateAssign::insert (const ProcessorID& pID, ProcessorDistribution* probDistEntry)
{
    addEntry (pID);
    int index = findEntry (pID);
    BagGateEntry *pEntry = (BagGateEntry*)getItem (index);
	pEntry->setProbDist(probDistEntry);
//    ProcAssignSchedule *sched = pEntry->getSchedule();
//    sched->addItem (new ProcAssignment (procAssign));
}

int BagGateAssign::removeAllEmptyEntry()
{
	int iDelNum = 0;

	int iCount = getCount();
	for( int i= iCount-1; i>=0; i-- )
	{
		BagGateEntry* pEntry = (BagGateEntry*)getItem( i );
		if( pEntry->getProbDist() == NULL ||
			pEntry->getProbDist()->getCount() == 0)
		{
			deleteItem( i );
			iDelNum ++;
		}
	}

	return iDelNum;
}