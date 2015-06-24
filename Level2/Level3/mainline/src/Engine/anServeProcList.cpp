// anServeProcList.cpp: implementation of the CanServeProcList class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "anServeProcList.h"
#include "process.h"
#include "common\CodeTimeTest.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CanServeProcList::CanServeProcList()
{

}

CanServeProcList::~CanServeProcList()
{

}

// add a canServeProc to the list
// and update the list if need
void CanServeProcList::addCanServeProcToList( Processor* _pProc, ProcAssRelation _assRelation, const CMobileElemConstraint* _pMobCon )
{
		
	if( m_canServeProcList.size() == 0 )	// if the first item, add it to list
	{
		m_canServeProcList.push_back( CanServeProc( _pProc, _assRelation, _pMobCon) );	
	}

	else
	{
		const CanServeProc& _lastItem = m_canServeProcList.back();
		switch( _assRelation )
		{
		case Inclusive:
		case Not:
			{
				if( _lastItem.getCanServerReason() != Exclusive) // can not add to the list
				{
					m_canServeProcList.push_back( CanServeProc( _pProc, _assRelation, _pMobCon) );	
				}
			}
			break;

		case Exclusive:
			{
				if( _lastItem.getCanServerReason() != Exclusive ) // remove all old item, add exclusive item
				{
					m_canServeProcList.clear();
					m_canServeProcList.push_back( CanServeProc( _pProc, _assRelation, _pMobCon) );	
				}
				else											 // if the last item is exclusive
				{
					if( _lastItem.getType().isEqual( _pMobCon) )
					{
						m_canServeProcList.push_back( CanServeProc( _pProc, _assRelation, _pMobCon) );	
					}
					else if( _lastItem.getType().fits( *_pMobCon) )
					{
						m_canServeProcList.clear();
						m_canServeProcList.push_back( CanServeProc( _pProc, _assRelation, _pMobCon) );	
					}
				}
			}

			break;

		default:
			break;
		}
	}

	return;
}

// get processor from list( add processor to array );
void CanServeProcList::getProcFromList( ProcessorArray *array ) 
{
	assert( array );
	L_CANSERVEPROC::iterator _iter;
	for( _iter = m_canServeProcList.begin(); _iter != m_canServeProcList.end(); ++_iter )
		array->addItem( _iter->getProc() );
}


bool CanServeProcList::hasProcWithID(const ProcessorID* _procID)
{
	ASSERT(_procID);
	L_CANSERVEPROC::iterator ite = m_canServeProcList.begin();
	L_CANSERVEPROC::iterator iteEn = m_canServeProcList.end();
	for( ; ite!=iteEn ; ite++ )
	{
		if ( *_procID == *(ite->getProc()->getID()) )
		{
			return true;
		}
	}

	return false;
}

bool CanServeProcList::IsExclusiveInList( const ProcessorID* _procID )
{
	ASSERT(_procID);
	L_CANSERVEPROC::iterator ite = m_canServeProcList.begin();
	L_CANSERVEPROC::iterator iteEn = m_canServeProcList.end();
	for( ; ite!=iteEn ; ite++ )
	{
		if ( *_procID == *(ite->getProc()->getID()) )
		{
			return ite->getCanServerReason() == Exclusive;
		}
	}

	return false;
}

