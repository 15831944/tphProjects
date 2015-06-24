// anServeProcList.h: interface for the CanServeProcList class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ANSERVEPROCLIST_H__E09F893F_A859_4C83_9FCB_8825F2369089__INCLUDED_)
#define AFX_ANSERVEPROCLIST_H__E09F893F_A859_4C83_9FCB_8825F2369089__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// include
#include "inputs\mobileelemconstraint.h"
#include "inputs\assign.h"
#include <list>

// declare
class Processor;
class ProcessorArray;

/************************************************************************/
class CanServeProc
{
private:
	Processor*				m_pProcessor;
	ProcAssRelation			m_canServeReason;
	CMobileElemConstraint   m_type;

public:
	CanServeProc() : m_pProcessor( NULL ), m_canServeReason( Inclusive )
	{

	}

	CanServeProc( Processor* _proc, ProcAssRelation _assRelation, const CMobileElemConstraint* _pMobType )
				: m_pProcessor( _proc ), m_canServeReason( _assRelation )
	{
		if( _pMobType )
		{
			m_type = *_pMobType;
		}
	}
	
	//get
    Processor* getProc( void )  { return m_pProcessor;	}
	ProcAssRelation getCanServerReason( void ) const { return m_canServeReason;	}
	const CMobileElemConstraint& getType( void ) const { return m_type;	}

};

typedef std::list< CanServeProc > L_CANSERVEPROC;
class CanServeProcList  
{
private:
	L_CANSERVEPROC m_canServeProcList;

public:
	CanServeProcList();
	virtual ~CanServeProcList();

	// add a canServeProc to the list
	// and update the list if need
	void addCanServeProcToList( Processor* _pProc, ProcAssRelation _assRelation, const CMobileElemConstraint* _pMobCon = NULL );

	// get processor from list( add processor to array );
	void getProcFromList( ProcessorArray *array );

	// check whether there is an processor with the same ID
	bool hasProcWithID(const ProcessorID* _procID);

	// check whether _procID is an EXCLUSIVE processor in the CanServeProcList
	bool IsExclusiveInList(const ProcessorID* _procID);
};

#endif // !defined(AFX_ANSERVEPROCLIST_H__E09F893F_A859_4C83_9FCB_8825F2369089__INCLUDED_)
