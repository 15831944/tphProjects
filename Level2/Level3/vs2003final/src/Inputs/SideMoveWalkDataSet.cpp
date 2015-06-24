// SideMoveWalkDataSet.cpp: implementation of the CSideMoveWalkDataSet class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SideMoveWalkDataSet.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSideMoveWalkDataSet::CSideMoveWalkDataSet() : DataSet(MovingSideWalKFile)
{
	m_pDefaultData = new CSideMoveWalkProcData();
}

CSideMoveWalkDataSet::~CSideMoveWalkDataSet()
{
	if( m_pDefaultData )
	{
		delete m_pDefaultData;
		m_pDefaultData = NULL;
	}
}

CSideMoveWalkProcData * CSideMoveWalkDataSet::GetLinkedSideWalkProcData (const ProcessorID& pID)
{
	CSideMoveWalkProcData * pData = (CSideMoveWalkProcData *)getEntryPoint(pID);
	if( !pData )
		pData = m_pDefaultData;
	return pData ;
	//return (CSideMoveWalkProcData *)getEntryPoint(pID);

}
// adds a ProcessorDataElement to the list if it does not already exist
void CSideMoveWalkDataSet::addEntry (const ProcessorID& pID)
{
    CSideMoveWalkProcData *pEntry = (CSideMoveWalkProcData *)FindEntry (pID);
	if (!pEntry)
        addItem (new CSideMoveWalkProcData( pID ));
}

// if processor ID already has an entry in hierarchy, add the
// Constraint and Probability to its PassengerConstraintDatabase
// if it does not yet have one, create it
int CSideMoveWalkDataSet::insert (const ProcessorID& id,
    CSideMoveWalkProcData * _pProcData)
{
    addEntry (id);
    int index = findEntry (id);
    CSideMoveWalkProcData *pData = (CSideMoveWalkProcData *)getItem(index);
	pData->SetMoveSpeed( _pProcData->GetMoveSpeed() );
	pData->SetWidth( _pProcData->GetWidth() );
	pData->SetCapacity( _pProcData->GetCapacity() );
	return index;
}

// reads Processor Hierarchy from an Arcterm File
void CSideMoveWalkDataSet::readData (ArctermFile& p_file)
{
    p_file.getLine();
    int iCount =0;
	p_file.getInteger( iCount );
	
	CSideMoveWalkProcData tempData;
    for (int i = 0; i < iCount; i++)
    {
		tempData.SetInputTerm( m_pInTerm );
		tempData.readData( p_file );
		insert( *tempData.getID(), &tempData );
    }
	
    buildHierarchy();
}

// writes Processor Hierarchy to an Arcterm File
void CSideMoveWalkDataSet::writeData (ArctermFile& p_file) const
{
	int iCount = getCount();
	p_file.writeInt( iCount );
	p_file.writeLine();
	
    for (int i = 0; i < iCount; i++)
    {
		getDatabase(i)->writeData( p_file );
    }
}

