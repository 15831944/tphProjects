// AutoCreateProcessorContainer.cpp: implementation of the CAutoCreateProcessorContainer class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"

#include "AutoCreateProcessorContainer.h"

#include "proclist.h"
#include "process.h"

#include "main\resource.h"
#include "main\termplandoc.h"
#include "../Common/ARCTracker.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CAutoCreateProcessorContainer::CAutoCreateProcessorContainer()
{
	m_iBeUsedIndex = -1;
}

CAutoCreateProcessorContainer::~CAutoCreateProcessorContainer()
{
	clear();
}

void CAutoCreateProcessorContainer::init()
{
	clear();
}

void CAutoCreateProcessorContainer::clear()
{
	for( UINT i=0; i<m_vAllAutoProcList.size(); i++ )
	{
		if( i != m_iBeUsedIndex )
		{
			for( UINT j=0; j<m_vAllAutoProcList[i].size(); j++ )
			{
				Processor* pProc = m_vAllAutoProcList[i].at(j).second;
				delete pProc;
			}
		}

		m_vAllAutoProcList.clear();
	}

	m_vAllAutoProcList.clear();
	m_iBeUsedIndex = -1;
}

void CAutoCreateProcessorContainer::addAutoProc( const ProcessorList* _srcList, const ProcessorList* _copyList )
{
	PLACE_METHOD_TRACK_STRING();
	assert( _srcList->getProcessorCount() == _copyList->getProcessorCount() );
	
	AutoProcList autoProcList;
	for( int i=0; i<_srcList->getProcessorCount(); i++ )
	{
		Processor* pSrcProc  = _srcList->getProcessor( i );
		Processor* pCopyProc = _copyList->getProcessor( i );
		
		ProcCopyPair pair( pSrcProc, pCopyProc );
		autoProcList.push_back( pair );
	}

	m_vAllAutoProcList.push_back( autoProcList );
}

int  CAutoCreateProcessorContainer::addAutoProcToProcList( int _idx, CTermPlanDoc* _pDoc )
{
	if( m_vAllAutoProcList.empty() )
	{
		return 0;
	}
	assert( _idx>=0 && (UINT)_idx<m_vAllAutoProcList.size() );
	assert( m_iBeUsedIndex== -1 );			// sure the function only call one times during a engine run!

	m_iBeUsedIndex = _idx;
	int iCreateNum = _pDoc->CreateProcessor2ByProcList( m_vAllAutoProcList[_idx] );
	return iCreateNum;
}