// PaxToResourcePoolMap.h: interface for the CPaxToResourcePoolMap class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PAXTORESOURCEPOOLMAP_H__85C5D606_7BD2_41AE_A3D9_F31031F05C39__INCLUDED_)
#define AFX_PAXTORESOURCEPOOLMAP_H__85C5D606_7BD2_41AE_A3D9_F31031F05C39__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// include
#include "mobileelemconstraint.h"
#include <map>

// declare
class CResourcePool;
class ProbabilityDistribution;
typedef std::pair< CResourcePool*, const ProbabilityDistribution* > pool_dist_pair;
typedef std::multimap< CMobileElemConstraint, pool_dist_pair > mobtype_pool_map;

/************************************************************************/
/*                    CPaxToResourcePoolMap                             */
/************************************************************************/
class CPaxToResourcePoolMap  
{
public:
	CPaxToResourcePoolMap();
	virtual ~CPaxToResourcePoolMap();

// data member
private:
	mobtype_pool_map m_dataMap;

// interface
public:
	void clear();
	void initMapData( InputTerminal* _inTerm, const ProcessorID& _procID );
	CResourcePool* getBestMatch( const CMobileElemConstraint& _mobType, long& _lServiceTime ); 	
};

#endif // !defined(AFX_PAXTORESOURCEPOOLMAP_H__85C5D606_7BD2_41AE_A3D9_F31031F05C39__INCLUDED_)
