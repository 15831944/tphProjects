// PaxToResourcePoolMap.cpp: implementation of the CPaxToResourcePoolMap class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "paxtoresourcepoolmap.h"
#include "proctoresource.h"
#include "resourcepool.h"
#include "../Common/exeption.h"
#include "../Common/ProbabilityDistribution.h"
#include "in_term.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPaxToResourcePoolMap::CPaxToResourcePoolMap()
{
	clear();
}

CPaxToResourcePoolMap::~CPaxToResourcePoolMap()
{
	clear();
}

void CPaxToResourcePoolMap::clear()
{
	m_dataMap.clear();
}

void CPaxToResourcePoolMap::initMapData( InputTerminal* _inTerm, const ProcessorID& _procID )
{
	assert( _inTerm );
	clear();

	if(_inTerm->m_pProcToResourceDB->getChecked() == FALSE)
		return;

	PROC2RESSET& proc2resList = _inTerm->m_pProcToResourceDB->getProc2ResList();
	PROC2RESSET::iterator iter;
	
	// TRACE(" _procID:\t%s\r\n", _procID.GetIDString());
	bool _bHasFited = false;
	ProcessorID _firstFitID; 
	for( iter = proc2resList.begin(); iter != proc2resList.end(); ++iter )
	{
		if(iter->getChecked() == FALSE)
			continue;
		// TRACE(" iter processorID:\t%s\r\n", iter->getProcessorID().GetIDString() );	
		if( !_bHasFited && iter->getProcessorID().idFits( _procID ) )
		{
			_bHasFited = true;
			_firstFitID = iter->getProcessorID();
		}

		if( _bHasFited )
		{
			if( iter->getProcessorID() == _firstFitID )
			{
				CMobileElemConstraint mob = iter->getMobileConstraint();
				CResourcePool* _pPool = _inTerm->m_pResourcePoolDB->getResourcePoolByName( iter->getResourcePoolName() );
				if( _pPool == NULL )
				{
					static CString strError;
					strError.Format("the resource pool name: %s is invalidate", iter->getResourcePoolName() );
					throw new StringError( strError );
				}
				const ProbabilityDistribution* _pDist = iter->getProServiceTime();

				m_dataMap.insert( std::make_pair( mob, std::make_pair( _pPool, _pDist)) );
			}
			else
			{
				break;
			}
		}
	}
}


CResourcePool* CPaxToResourcePoolMap::getBestMatch( const CMobileElemConstraint& _mobType, long& _lServiceTime )
{
	mobtype_pool_map::iterator iter;

#ifdef DEBUG
	CString szBuffer;
	_mobType.screenPrint( szBuffer,0, 1024 );
//	// TRACE(" _mobType:\t%s\r\n", szBuffer.GetBuffer(0) );
#endif //DEBUG

	for( iter = m_dataMap.begin(); iter!= m_dataMap.end(); ++iter )
	{
#ifdef DEBUG
	iter->first.screenPrint( szBuffer,0, 128 );
//	// TRACE(" iter->first:\t%s\r\n", szBuffer.GetBuffer(0) );
#endif
		if( iter->first.fits( _mobType ) )
		{
			_lServiceTime =(long) ( (iter->second.second)->getRandomValue() );
			return iter->second.first;
		}
	}
	
	return NULL;
}